// [Graphics] Water - see water.hpp.
#include "water.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>

#include "../registry.hpp"
#include "renderer.hpp"

namespace joan::render {

using namespace gl;

namespace {

std::vector<std::pair<const void*, WaterRect>> g_waters;
bool g_replaced = false;  // this scene's water was drawn by the modern pass
WaterHook g_hook = nullptr;
struct Fall {
    WaterFallInfo info;
    WaterRect footprint{};
    std::uint32_t seen = 0;  // g_scenes when last drawn (0 = never)
};
std::unordered_map<const void*, Fall> g_falls;  // every plane Plane::draw classified (while Water=1)
std::uint32_t g_scenes = 1;                      // water_scene_done calls
std::uint32_t g_river_seen = 0;  // g_scenes when a river was last drawn (0 = never)
int g_river_stage = 0;           // the river draw in progress (water_river_begin), 1-based stage; 0 = none
bool g_river_blended = false;    // ... a falling sheet (the game's blending, the texture's alpha)
bool g_scene_copies = false;     // this scene's copies were made after its opaque world (the rivers can read them)

constexpr int kMaxRects = 32;

const char* const kFullscreenVs = R"(#version 450 core
void main() {
    vec2 p = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}
)";

const char* const kWaterFs = R"(#version 450 core
layout(binding = 0) uniform sampler2D u_depth;   // the scene's depth (copy, one sample per pixel)
layout(binding = 1) uniform sampler2D u_colour;  // the scene's colour (copy)
uniform vec4 u_lin;     // P33, P43, depth range min, depth range max
uniform vec4 u_proj;    // 1/P11, 1/P22, P31, P32
uniform vec4 u_vp;      // viewport x, y, w, h (pixels)
uniform vec2 u_size;    // framebuffer size (pixels)
uniform mat4 u_view_inv;  // view -> world
uniform mat4 u_view;      // world -> view
uniform vec4 u_sun;     // xyz: direction sunlight travels (world); w: 1 = a sun
uniform vec4 u_fog;     // start, end, 1 = on, 1 = range fog (else view depth)
uniform vec3 u_fog_colour;
uniform vec4 u_params;  // time (s), absorption (1 / world units), wave strength, reflection strength
uniform int u_count;
uniform vec4 u_rect[32];     // x0, y0, x1, y1 (world)
uniform float u_height[32];  // z (world)
uniform int u_fall_count;
uniform vec4 u_fall[8];      // waterfalls: x0, y0, x1, y1 of the falling sheet (world)
uniform float u_fall_z[8];   // their lowest point (world z)
layout(binding = 2) uniform sampler2D u_env;  // environment panorama (env_uv)
layout(binding = 3) uniform sampler2D u_overlay;  // the depth after the billboards drawn before the world
layout(binding = 4) uniform sampler2D u_overlay_before;  // and before them
uniform int u_overlay_on;
out vec4 o_colour;

// World direction (z >= 0) -> environment panorama: u = azimuth, v = sqrt(elevation / 90 degrees)
// (more texels near the horizon, where the hills and the sunset are).
vec2 env_uv(vec3 d) {
    float el = asin(clamp(d.z, 0.0, 1.0));
    return vec2(atan(d.y, d.x) / 6.2831853 + 0.5, sqrt(el / 1.5707963));
}

const float kSky = 1e9;

float scene_z(ivec2 p) {  // view depth of the scene at pixel p (kSky = nothing drawn)
    float d = (texelFetch(u_depth, p, 0).r - u_lin.z) / max(u_lin.w - u_lin.z, 1e-6);
    if (d >= 0.999999) return kSky;
    float z = u_lin.y / (d - u_lin.x);
    return z > 0.0 ? z : kSky;
}
// A name label / health plate drawn before the world that nothing covered: in the copies, but no surface of the world
// (its depth is the billboard's). Reflections and refraction look past it.
bool label_px(ivec2 p) {
    if (u_overlay_on == 0) return false;
    float o = texelFetch(u_overlay, p, 0).r;
    return o != texelFetch(u_overlay_before, p, 0).r && o == texelFetch(u_depth, p, 0).r;
}
vec3 view_dir(vec2 px) {  // view-space point at view depth 1 through pixel px
    vec2 ndc = vec2((px.x - u_vp.x) / u_vp.z * 2.0 - 1.0, 1.0 - (px.y - u_vp.y) / u_vp.w * 2.0);
    return vec3((ndc - u_proj.zw) * u_proj.xy, 1.0);
}
vec2 project(vec3 p) {  // view space -> pixel
    vec2 ndc = (p.xy / p.z) / u_proj.xy + u_proj.zw;
    return vec2(u_vp.x + (ndc.x + 1.0) * 0.5 * u_vp.z, u_vp.y + (1.0 - ndc.y) * 0.5 * u_vp.w);
}
bool on_screen(vec2 px) { return px.x >= u_vp.x && px.y >= u_vp.y && px.x < u_vp.x + u_vp.z && px.y < u_vp.y + u_vp.w; }

float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}
float noise(vec2 p) {
    vec2 i = floor(p), f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i), hash(i + vec2(1.0, 0.0)), u.x), mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x), u.y);
}
// Gradient noise (-0.7..0.7, smooth, no grid-aligned blobs).
vec2 gradient(vec2 i) {
    float a = 6.2831853 * hash(i);
    return vec2(cos(a), sin(a));
}
float gnoise(vec2 p) {
    vec2 i = floor(p), f = fract(p);
    vec2 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);
    float a = dot(gradient(i), f), b = dot(gradient(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0));
    float c = dot(gradient(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)), d = dot(gradient(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0));
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}
// The surface's height (world units) at p: five layers of chop, ~70 down to ~4 units across, each turned
// another way and moving at a speed that grows with its size (as real waves: ~25 units per second for the
// largest, ~6 for the smallest); every layer is two drifts blended, so it changes shape instead of
// sliding. `px`: world units per pixel here - layers finer than a few pixels fade out (no shimmering).
float height(vec2 p, float t, float px) {
    float h = 0.0, lambda = 70.0, angle = 0.4;
    for (int i = 0; i < 5; ++i) {
        float w = smoothstep(2.0 * px, 5.0 * px, lambda);
        if (w <= 0.0) break;
        float c = cos(angle), s = sin(angle);
        vec2 q = mat2(c, s, -s, c) * p / lambda;
        float sp = 3.0 / sqrt(lambda) * t;  // wavelengths travelled
        float n = 0.5 * (gnoise(q + vec2(sp, 0.31 * sp)) + gnoise(q * 1.13 + vec2(-0.47 * sp, 0.88 * sp) + 17.0));
        h += w * lambda * 0.035 * n;
        lambda *= 0.48;
        angle += 1.13;
    }
    return h;
}
// Churning water (a waterfall's foot): fast, fine, directionless.
float churn_waves(vec2 p, float t) {
    return noise(p * 0.06 + vec2(t * 1.9, -t * 1.4)) + 0.6 * noise(p * 0.13 + vec2(-t * 3.1, -t * 2.6)) +
           0.3 * noise(p * 0.29 + vec2(t * 4.3, t * 5.1));
}

void main() {
    ivec2 ip = ivec2(gl_FragCoord.xy);
    // A name label / health plate nothing was drawn over stays as it is.
    if (label_px(ip)) discard;
    float zs = scene_z(ip);
    vec3 vdir = view_dir(gl_FragCoord.xy);  // view point at view depth s = vdir * s
    vec3 C = u_view_inv[3].xyz;
    vec3 D = mat3(u_view_inv) * vdir;       // world displacement per unit of view depth

    // The nearest water surface in front of the scene.
    float s = 1e30;
    int hit = -1;
    for (int i = 0; i < u_count; ++i) {
        float h = u_height[i];
        if (C.z <= h + 0.5 || D.z >= 0.0) continue;  // camera below the surface / looking up
        float t = (h - C.z) / D.z;
        if (t <= 0.0 || t >= s || t >= zs) continue;
        vec2 w = C.xy + D.xy * t;
        vec4 r = u_rect[i];
        if (w.x < r.x || w.y < r.y || w.x > r.z || w.y > r.w) continue;
        s = t;
        hit = i;
    }
    if (hit < 0) discard;

    vec3 Pw = C + D * s;  // the surface point (world)
    vec3 Pv = vdir * s;   // (view)
    float dist = length(Pv);
    vec3 V = normalize(D);
    float t = u_params.x;

    // Waterfalls: how close this point is to where one falls into this surface (1 = in it).
    float churn = 0.0;
    for (int i = 0; i < u_fall_count; ++i) {
        vec4 r = u_fall[i];
        if (abs(u_fall_z[i] - u_height[hit]) > 250.0) continue;  // falls into another surface
        vec2 c = 0.5 * (r.xy + r.zw), half_size = 0.5 * (r.zw - r.xy);
        float d = length(max(abs(Pw.xy - c) - half_size, 0.0));
        churn = max(churn, 1.0 - smoothstep(0.0, 160.0 + 0.5 * max(half_size.x, half_size.y), d));
    }

    // Waves: normal from the height field's slope, calmer with distance (no shimmering far away).
    // World units per pixel at the surface (wider at grazing angles).
    float px = Pv.z * 2.0 * u_proj.y / u_vp.w / max(abs(V.z), 0.15);
    const float e = 0.6;
    float hx = (height(Pw.xy + vec2(e, 0.0), t, px) - height(Pw.xy - vec2(e, 0.0), t, px)) / (2.0 * e);
    float hy = (height(Pw.xy + vec2(0.0, e), t, px) - height(Pw.xy - vec2(0.0, e), t, px)) / (2.0 * e);
    if (churn > 0.0) {
        float k = churn * 3.5 / (2.0 * e);
        hx += k * (churn_waves(Pw.xy + vec2(e, 0.0), t) - churn_waves(Pw.xy - vec2(e, 0.0), t)) / max(1.0, px);
        hy += k * (churn_waves(Pw.xy + vec2(0.0, e), t) - churn_waves(Pw.xy - vec2(0.0, e), t)) / max(1.0, px);
    }
    vec3 N = normalize(vec3(-hx * u_params.z, -hy * u_params.z, 1.0));

    // What lies under the surface: how deep (vertically) and how far the light travels through it.
    float under_z = zs;
    float thickness = under_z >= kSky ? 1e4 : (under_z - s) * length(D);
    float depth = under_z >= kSky ? 1e4 : max(Pw.z - (C.z + D.z * under_z), 0.0);
    vec3 ambient_c = u_fog.z > 0.5 ? u_fog_colour : vec3(0.6);
    float ambient = clamp(dot(ambient_c, vec3(0.299, 0.587, 0.114)) * 1.4, 0.25, 1.2);

    // Refraction: the lake bed bent by the waves (not where the bent sample is above the water).
    vec2 off = N.xy * (u_size.y * 0.025) * min(depth / 40.0, 1.0) / max(dist / 400.0, 1.0);
    vec2 rp = gl_FragCoord.xy + off;
    if (!on_screen(rp) || scene_z(ivec2(rp)) < s || label_px(ivec2(rp))) rp = gl_FragCoord.xy;
    vec3 bed = texture(u_colour, rp / u_size).rgb;
    vec3 trans = exp(-vec3(1.6, 0.85, 0.65) * u_params.y * thickness);
    vec3 deep = vec3(0.045, 0.10, 0.10) * ambient;
    vec3 under = bed * trans + deep * (1.0 - trans);

    // Reflection: marched through the depth buffer from the surface point; the sky beyond.
    vec3 R = reflect(V, N);
    R.z = max(R.z, 0.02);
    R = normalize(R);
    vec3 Rv = (u_view * vec4(R, 0.0)).xyz;
    // Beyond the picture, and for the sky: the environment panorama in the reflected direction (the same
    // whichever way the camera turns).
    vec3 sky = texture(u_env, env_uv(R)).rgb;
    vec3 refl = sky;
    {
        float step_len = 12.0;
        vec3 p = Pv, prev = Pv;
        for (int i = 0; i < 40; ++i) {
            prev = p;
            p += Rv * step_len;
            step_len *= 1.16;
            if (p.z <= 1.0) break;
            vec2 px = project(p);
            if (!on_screen(px)) break;
            if (label_px(ivec2(px))) continue;  // a label: look past it
            float zq = scene_z(ivec2(px));
            bool sky_px = zq >= kSky;
            if (sky_px) break;  // the sky: from the panorama
            if (p.z > zq && p.z - zq < max(step_len * 2.0, 60.0)) {
                vec3 a = prev, b = p;  // refine between the last two points
                for (int k = 0; k < 5; ++k) {
                    vec3 m = 0.5 * (a + b);
                    if (m.z > scene_z(ivec2(project(m)))) b = m; else a = m;
                }
                px = project(b);
                if (label_px(ivec2(px))) continue;  // refined onto a label: look past it
                vec2 q = (px - u_vp.xy) / u_vp.zw;
                float edge = smoothstep(0.0, 0.08, min(min(q.x, 1.0 - q.x), min(q.y, 1.0 - q.y)));
                refl = mix(sky, texture(u_colour, px / u_size).rgb, edge);
                break;
            }
        }
    }

    float cos_v = max(dot(-V, N), 0.0);
    float fresnel = clamp((0.02 + 0.98 * pow(1.0 - cos_v, 5.0)) * u_params.w, 0.0, 1.0);
    vec3 colour = mix(under, refl, fresnel);


    // Shore: a little foam, and the surface fades into the shallow bed.
    float foam = (1.0 - smoothstep(0.0, 6.0, depth)) * smoothstep(0.45, 0.8, noise(Pw.xy * 0.08 + vec2(t * 0.2, -t * 0.15)));
    colour += foam * 0.25 * ambient;
    // A waterfall's foot: white water, thinning out with the distance from the fall.
    if (churn > 0.0) {
        float bubbles = smoothstep(0.55, 1.25, churn_waves(Pw.xy * 1.3, t * 1.2) + 0.6 * churn);
        float white = clamp(churn * churn * 0.85 + churn * bubbles * 0.6, 0.0, 0.92);
        colour = mix(colour, vec3(0.86, 0.90, 0.92) * ambient, white);
    }
    colour = mix(texelFetch(u_colour, ip, 0).rgb, colour, smoothstep(0.0, 8.0, depth));

    float fog_d = u_fog.w > 0.5 ? dist : Pv.z;  // range fog / view depth, as the game's vertex fog
    if (u_fog.z > 0.5) colour = mix(u_fog_colour, colour, clamp((u_fog.y - fog_d) / max(u_fog.y - u_fog.x, 1e-3), 0.0, 1.0));

    gl_FragDepth = mix(u_lin.z, u_lin.w, u_lin.x + u_lin.y / s);
    o_colour = vec4(colour, 1.0);
}
)";

// The environment panorama's update: each texel's direction (u = azimuth, v = sqrt(elevation / 90 degrees),
// as env_uv) looked up in the picture; where the picture shows the sky or something at least u_far away,
// the texel moves u_rate of the way to it (blended).
const char* const kEnvFs = R"(#version 450 core
layout(binding = 0) uniform sampler2D u_depth;
layout(binding = 1) uniform sampler2D u_colour;
uniform vec4 u_lin;     // P33, P43, depth range min, depth range max
uniform vec4 u_proj;    // 1/P11, 1/P22, P31, P32
uniform vec4 u_vp;      // viewport x, y, w, h (pixels)
uniform mat4 u_view;    // world -> view
uniform vec2 u_env_size;
uniform float u_far;
uniform float u_rate;
out vec4 o_colour;
void main() {
    vec2 uv = gl_FragCoord.xy / u_env_size;
    float az = (uv.x - 0.5) * 6.2831853, el = uv.y * uv.y * 1.5707963;
    vec3 d = vec3(cos(el) * cos(az), cos(el) * sin(az), sin(el));
    vec3 v = (u_view * vec4(d, 0.0)).xyz;
    if (v.z < 0.05) discard;
    vec2 ndc = (v.xy / v.z) / u_proj.xy + u_proj.zw;
    vec2 px = vec2(u_vp.x + (ndc.x + 1.0) * 0.5 * u_vp.z, u_vp.y + (1.0 - ndc.y) * 0.5 * u_vp.w);
    if (px.x < u_vp.x || px.y < u_vp.y || px.x >= u_vp.x + u_vp.z || px.y >= u_vp.y + u_vp.w) discard;
    ivec2 ip = ivec2(px);
    float dz = (texelFetch(u_depth, ip, 0).r - u_lin.z) / max(u_lin.w - u_lin.z, 1e-6);
    if (dz < 0.999999) {
        float z = u_lin.y / (dz - u_lin.x);
        if (z > 0.0 && z < u_far) discard;
    }
    o_colour = vec4(texelFetch(u_colour, ip, 0).rgb, u_rate);
}
)";
constexpr int kEnvW = 256, kEnvH = 64;

// Rivers (water_river_begin): the draw's own vertex shader, this pixel shader. The surface is the mesh's
// (sloped, with its depth); its relief is the game's flowing river texture at the coordinates the game
// animates (so it flows as fast and as far as the original), turned into a normal by its screen-space
// gradient; the rest is the lakes' shading against the scene copies made after the opaque world. Its uniforms
// are named u_w_* (the game's vertex shader already has u_view / u_proj in its block).
const char* const kRiverFs = R"(
in vec4 v_tex[8];
layout(binding = 8) uniform sampler2D u_depth;    // the scene's depth before the transparent pass (copy)
layout(binding = 9) uniform sampler2D u_colour;   // its colour (copy)
layout(binding = 10) uniform sampler2D u_env;     // environment panorama (env_uv)
layout(binding = 11) uniform sampler2D u_w_overlay;  // the depth after the billboards drawn before the world
layout(binding = 12) uniform sampler2D u_w_overlay_before;  // and before them
uniform int u_w_overlay_on;
layout(binding = STAGE) uniform sampler2D t_river;  // the game's river texture, as the game binds it
uniform vec4 u_w_lin;     // P33, P43, depth range min, depth range max
uniform vec4 u_w_proj;    // 1/P11, 1/P22, P31, P32
uniform vec4 u_w_vp;      // viewport x, y, w, h (pixels)
uniform vec2 u_w_size;    // framebuffer size (pixels)
uniform mat4 u_w_view_inv;
uniform mat4 u_w_view;
uniform vec4 u_w_fog;     // start, end, 1 = on, 1 = range fog
uniform vec3 u_w_fog_colour;
uniform vec4 u_w_params;  // time (s), absorption (1 / world units), wave strength, reflection strength
uniform int u_w_blended;  // 1 = a falling sheet: the texture's alpha for the game's blending (river_bind)
in vec4 v_diffuse;
layout(location = 0) out vec4 frag_color;
layout(location = 1) out float frag_cut_depth;

const float kSky = 1e9;
vec2 env_uv(vec3 d) {
    float el = asin(clamp(d.z, 0.0, 1.0));
    return vec2(atan(d.y, d.x) / 6.2831853 + 0.5, sqrt(el / 1.5707963));
}
float scene_z(ivec2 p) {
    float d = (texelFetch(u_depth, p, 0).r - u_w_lin.z) / max(u_w_lin.w - u_w_lin.z, 1e-6);
    if (d >= 0.999999) return kSky;
    float z = u_w_lin.y / (d - u_w_lin.x);
    return z > 0.0 ? z : kSky;
}
bool label_px(ivec2 p) {  // a name label / health plate nothing covered (see the lakes' shader)
    if (u_w_overlay_on == 0) return false;
    float o = texelFetch(u_w_overlay, p, 0).r;
    return o != texelFetch(u_w_overlay_before, p, 0).r && o == texelFetch(u_depth, p, 0).r;
}
vec3 view_dir(vec2 px) {
    vec2 ndc = vec2((px.x - u_w_vp.x) / u_w_vp.z * 2.0 - 1.0, 1.0 - (px.y - u_w_vp.y) / u_w_vp.w * 2.0);
    return vec3((ndc - u_w_proj.zw) * u_w_proj.xy, 1.0);
}
vec2 project(vec3 p) {
    vec2 ndc = (p.xy / p.z) / u_w_proj.xy + u_w_proj.zw;
    return vec2(u_w_vp.x + (ndc.x + 1.0) * 0.5 * u_w_vp.z, u_w_vp.y + (1.0 - ndc.y) * 0.5 * u_w_vp.w);
}
bool on_screen(vec2 px) { return px.x >= u_w_vp.x && px.y >= u_w_vp.y && px.x < u_w_vp.x + u_w_vp.z && px.y < u_w_vp.y + u_w_vp.w; }
float lum(vec3 c) { return dot(c, vec3(0.299, 0.587, 0.114)); }

void main() {
    ivec2 ip = ivec2(gl_FragCoord.xy);
    vec2 uv = v_tex[STAGE].xy;
    // The surface point: this fragment's own depth.
    float d = (gl_FragCoord.z - u_w_lin.z) / max(u_w_lin.w - u_w_lin.z, 1e-6);
    float s = u_w_lin.y / (d - u_w_lin.x);
    vec3 vdir = view_dir(gl_FragCoord.xy);
    vec3 Pv = vdir * s;
    vec3 Pw = (u_w_view_inv * vec4(Pv, 1.0)).xyz;
    vec3 V = normalize(mat3(u_w_view_inv) * vdir);
    float dist = length(Pv);

    // The normal: the mesh's facet (towards the camera), raised by the flowing texture's brightness (two
    // scales) through its screen-space gradient (surface gradient bump mapping).
    vec3 dpdx = dFdx(Pw), dpdy = dFdy(Pw);
    vec3 Ng = cross(dpdx, dpdy);
    Ng = dot(Ng, Ng) > 1e-12 ? normalize(Ng) : vec3(0.0, 0.0, 1.0);
    if (dot(Ng, V) > 0.0) Ng = -Ng;
    vec4 tex4 = texture(t_river, uv);
    vec3 tex = tex4.rgb;
    float h = lum(tex) + 0.5 * lum(texture(t_river, uv * 2.03 + vec2(0.37, 0.61)).rgb);
    vec3 r1 = cross(dpdy, Ng), r2 = cross(Ng, dpdx);
    float det = dot(dpdx, r1);
    vec3 N = Ng;
    if (abs(det) > 1e-8) {
        vec3 grad = sign(det) * (dFdx(h) * r1 + dFdy(h) * r2) * (5.0 * u_w_params.z);
        N = normalize(abs(det) * Ng - grad);
    }

    // Under the surface: the scene behind it (the river bed), bent and absorbed with the distance through it.
    float zs = scene_z(ip);
    float thickness = zs >= kSky ? 1e4 : max(zs - s, 0.0) * length(vdir);
    float depth = thickness * max(abs(dot(V, Ng)), 0.2);
    vec3 ambient_c = u_w_fog.z > 0.5 ? u_w_fog_colour : vec3(0.6);
    float ambient = clamp(lum(ambient_c) * 1.4, 0.25, 1.2);
    vec2 off = (N.xy - Ng.xy) * (u_w_size.y * 0.04) * min(depth / 30.0, 1.0) / max(dist / 400.0, 1.0);
    vec2 rp = gl_FragCoord.xy + off;
    if (!on_screen(rp) || scene_z(ivec2(rp)) < s || label_px(ivec2(rp))) rp = gl_FragCoord.xy;
    vec3 bed = texture(u_colour, rp / u_w_size).rgb;
    vec3 trans = exp(-vec3(1.6, 0.85, 0.65) * u_w_params.y * thickness);
    vec3 deep = vec3(0.05, 0.10, 0.10) * ambient;
    vec3 under = bed * trans + deep * (1.0 - trans);

    // Reflection: marched through the depth copy; the environment panorama beyond the picture.
    vec3 R = reflect(V, N);
    R.z = max(R.z, 0.02);
    R = normalize(R);
    vec3 Rv = (u_w_view * vec4(R, 0.0)).xyz;
    vec3 sky = texture(u_env, env_uv(R)).rgb;
    vec3 refl = sky;
    {
        float step_len = 12.0;
        vec3 p = Pv, prev = Pv;
        for (int i = 0; i < 40; ++i) {
            prev = p;
            p += Rv * step_len;
            step_len *= 1.16;
            if (p.z <= 1.0) break;
            vec2 px = project(p);
            if (!on_screen(px)) break;
            if (label_px(ivec2(px))) continue;  // a label: look past it
            float zq = scene_z(ivec2(px));
            if (zq >= kSky) break;
            if (p.z > zq && p.z - zq < max(step_len * 2.0, 60.0)) {
                vec3 a = prev, b = p;
                for (int k = 0; k < 5; ++k) {
                    vec3 m = 0.5 * (a + b);
                    if (m.z > scene_z(ivec2(project(m)))) b = m; else a = m;
                }
                px = project(b);
                if (label_px(ivec2(px))) continue;  // refined onto a label: look past it
                vec2 q = (px - u_w_vp.xy) / u_w_vp.zw;
                float edge = smoothstep(0.0, 0.08, min(min(q.x, 1.0 - q.x), min(q.y, 1.0 - q.y)));
                refl = mix(sky, texture(u_colour, px / u_w_size).rgb, edge);
                break;
            }
        }
    }
    float cos_v = max(dot(-V, N), 0.0);
    float fresnel = clamp((0.02 + 0.98 * pow(1.0 - cos_v, 5.0)) * u_w_params.w, 0.0, 1.0);
    vec3 colour = mix(under, refl, fresnel);

    // White water: the texture's bright streaks.
    float white = smoothstep(0.55, 0.95, lum(tex)) * 0.55;
    colour = mix(colour, vec3(0.86, 0.90, 0.92) * ambient, white);
    // The shore: into the shallow bed (a falling sheet: its own alpha instead).
    if (u_w_blended == 0) colour = mix(texelFetch(u_colour, ip, 0).rgb, colour, smoothstep(0.0, 6.0, depth));

    float fog_d = u_w_fog.w > 0.5 ? dist : Pv.z;
    if (u_w_fog.z > 0.5) colour = mix(u_w_fog_colour, colour, clamp((u_w_fog.y - fog_d) / max(u_w_fog.y - u_w_fog.x, 1e-3), 0.0, 1.0));
    frag_color = vec4(colour, u_w_blended != 0 ? tex4.a * v_diffuse.a : 1.0);
    frag_cut_depth = gl_FragCoord.z;
}
)";

GLuint make_texture(GLenum format, int w, int h, GLenum filter) {
    GLuint t = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &t);
    glTextureStorage2D(t, 1, format, w, h);
    glTextureParameteri(t, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
    glTextureParameteri(t, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
    glTextureParameteri(t, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(t, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return t;
}
GLuint make_fbo(GLuint texture, GLenum attachment) {
    GLuint f = 0;
    glCreateFramebuffers(1, &f);
    glNamedFramebufferTexture(f, attachment, texture, 0);
    if (attachment != GL_COLOR_ATTACHMENT0) {
        glNamedFramebufferDrawBuffer(f, GL_NONE_);
        glNamedFramebufferReadBuffer(f, GL_NONE_);
    }
    return f;
}
void set4(GLuint program, const char* name, float x, float y, float z, float w) {
    const float v[4] = {x, y, z, w};
    glProgramUniform4fv(program, glGetUniformLocation(program, name), 1, v);
}

}  // namespace

WaterParams& water_params() {
    static WaterParams p = [] {
        const auto& s = hook::settings();
        WaterParams r;
        r.mode = std::clamp(s.water, 0, 1);
        r.clarity = static_cast<float>(std::clamp(s.water_clarity, 10, 5000));
        r.waves = static_cast<float>(std::clamp(s.water_waves, 0, 400)) / 100.0f;
        r.reflection = static_cast<float>(std::clamp(s.water_reflection, 0, 200)) / 100.0f;
        r.flow = static_cast<float>(std::clamp(s.water_flow, 5, 200)) / 100.0f;
        return r;
    }();
    return p;
}

void water_register(const void* plane, const WaterRect& rect) {
    if (!plane) return;
    water_unregister(plane);
    g_waters.emplace_back(plane, rect);
}
void water_unregister(const void* plane) {
    g_waters.erase(std::remove_if(g_waters.begin(), g_waters.end(), [plane](const auto& w) { return w.first == plane; }), g_waters.end());
    g_falls.erase(plane);
}
WaterFallInfo& water_fall_info(const void* plane) { return g_falls[plane].info; }
void water_fall_forget(const void* plane) { g_falls.erase(plane); }
void water_fall_seen(const void* plane, const WaterRect& footprint) {
    Fall& f = g_falls[plane];
    static unsigned logged = 0;
    if (f.seen == 0 && logged < 5) {
        ++logged;
        hook::log_line("[Graphics] Water: waterfall at (%.0f..%.0f, %.0f..%.0f), foot z %.0f", static_cast<double>(footprint.x0),
                       static_cast<double>(footprint.x1), static_cast<double>(footprint.y0), static_cast<double>(footprint.y1),
                       static_cast<double>(footprint.z));
    }
    f.footprint = footprint;
    f.seen = g_scenes;
}
std::vector<WaterRect> water_falls() {
    std::vector<WaterRect> r;
    for (const auto& [plane, f] : g_falls)
        if (f.info.kind == 1 && f.seen != 0 && g_scenes - f.seen <= 3) r.push_back(f.footprint);
    return r;
}
std::vector<WaterRect> water_rects() {
    std::vector<WaterRect> r;
    r.reserve(g_waters.size());
    for (const auto& w : g_waters) r.push_back(w.second);
    return r;
}
bool water_replaced(const void* plane) {
    if (!g_replaced) return false;
    for (const auto& w : g_waters)
        if (w.first == plane) return true;
    return false;
}

void water_river_begin(int stage, bool blended) {
    g_river_stage = stage;
    g_river_blended = blended;
    g_river_seen = g_scenes;
}
void water_river_end() { g_river_stage = 0; }
int water_river_stage() { return g_river_stage; }
bool water_river_blended() { return g_river_blended; }
int water_texture_kind(const char* name) {
    auto starts = [name](const char* prefix) {
        for (int i = 0; prefix[i]; ++i)
            if (name[i] == 0 || (name[i] | 0x20) != prefix[i]) return false;
        return true;
    };
    if (!name) return 0;
    if (starts("river")) return 1;
    if (starts("waterfal")) return 2;
    return 0;
}
float& water_flow_scale() {
    static float scale = 1.0f;
    return scale;
}

// Rivers drawn in the last scenes: the pass makes its copies for them even with no lake in sight.
bool rivers_recent() { return g_river_seen != 0 && g_scenes - g_river_seen <= 30; }

void water_scene_begin() {
    g_replaced = false;
    g_scene_copies = false;
}
void water_scene_done(const float* projection, const float* sun) {
    g_replaced = false;
    g_scene_copies = false;
    ++g_scenes;
    // Only a perspective scene (the 3D world; D3D _34 = 1): the 2D layers (unit labels, health bars, the HUD) are en3d
    // scenes too, with an orthographic projection (_34 = 0). Running the pass after them re-shaded the water from copies
    // that already held the labels, which then showed in the reflections.
    const bool perspective = projection != nullptr && projection[11] != 0.0f;
    if (g_hook && perspective && water_params().mode == 1 && (!g_waters.empty() || rivers_recent())) g_replaced = g_hook(projection, sun);
}
void set_water_hook(WaterHook hook) { g_hook = hook; }

// --- WaterPass ------------------------------------------------------------------------------
bool WaterPass::ensure(int width, int height) {
    if (failed_) return false;
    if (!prog_) {
        Renderer* r = current();
        if (!r) return false;
        prog_ = r->compile_program(kFullscreenVs, kWaterFs, "water");
        prog_env_ = prog_ ? r->compile_program(kFullscreenVs, kEnvFs, "water: environment") : 0;
        if (!prog_ || !prog_env_) {
            hook::log_line("[Graphics] Water: shader compilation failed - the original water");
            failed_ = true;
            return false;
        }
        glCreateVertexArrays(1, &vao_);
        env_tex_ = make_texture(GL_RGBA16F, kEnvW, kEnvH, GL_LINEAR);
        glTextureParameteri(env_tex_, GL_TEXTURE_WRAP_S, GL_REPEAT);  // azimuth wraps around
        env_fbo_ = make_fbo(env_tex_, GL_COLOR_ATTACHMENT0);
        env_valid_ = false;
    }
    if (w_ != width || h_ != height) {
        const GLuint fbos[] = {colour_fbo_, depth_fbo_};
        const GLuint textures[] = {colour_tex_, depth_tex_};
        for (GLuint f : fbos)
            if (f) glDeleteFramebuffers(1, &f);
        for (GLuint t : textures)
            if (t) glDeleteTextures(1, &t);
        colour_tex_ = make_texture(GL_RGBA8, width, height, GL_LINEAR);
        colour_fbo_ = make_fbo(colour_tex_, GL_COLOR_ATTACHMENT0);
        depth_tex_ = make_texture(GL_DEPTH24_STENCIL8, width, height, GL_NEAREST);
        depth_fbo_ = make_fbo(depth_tex_, GL_DEPTH_STENCIL_ATTACHMENT);
        w_ = width;
        h_ = height;
    }
    return true;
}

void WaterPass::shutdown() {
    const GLuint fbos[] = {colour_fbo_, depth_fbo_, env_fbo_};
    const GLuint textures[] = {colour_tex_, depth_tex_, env_tex_};
    for (GLuint f : fbos)
        if (f) glDeleteFramebuffers(1, &f);
    for (GLuint t : textures)
        if (t) glDeleteTextures(1, &t);
    if (prog_) glDeleteProgram(prog_);
    if (prog_env_) glDeleteProgram(prog_env_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    colour_fbo_ = depth_fbo_ = env_fbo_ = colour_tex_ = depth_tex_ = env_tex_ = prog_ = prog_env_ = vao_ = 0;
    w_ = h_ = 0;
    env_valid_ = false;
}

bool WaterPass::run(const Input& in, const std::vector<WaterRect>& rects, const std::vector<WaterRect>& falls) {
    state_changed_ = false;
    const float* p = in.projection;
    // Direct3D perspective only: x' = x P11 + z P31, y' = y P22 + z P32, w' = z.
    if (!(p[11] == 1.0f && p[0] != 0.0f && p[5] != 0.0f && p[14] < 0.0f && p[10] > 1.0f)) return false;
    const bool rivers = rivers_recent();
    if ((rects.empty() && !rivers) || !ensure(in.width, in.height)) return false;

    // Camera: view -> world (rotation transposed, translation back).
    const float* v = in.view;
    float inv[16] = {v[0], v[4], v[8], 0.0f, v[1], v[5], v[9], 0.0f, v[2], v[6], v[10], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    for (int c = 0; c < 3; ++c) inv[12 + c] = -(v[12] * inv[c] + v[13] * inv[4 + c] + v[14] * inv[8 + c]);
    const float cam[3] = {inv[12], inv[13], inv[14]};

    // The surfaces the camera is above, nearest first (at most kMaxRects).
    std::vector<std::pair<float, WaterRect>> list;
    for (const WaterRect& r : rects) {
        if (!(cam[2] > r.z + 0.5f)) continue;
        const float dx = std::max({r.x0 - cam[0], 0.0f, cam[0] - r.x1}), dy = std::max({r.y0 - cam[1], 0.0f, cam[1] - r.y1});
        list.emplace_back(dx * dx + dy * dy, r);
    }
    if (list.empty() && !rivers) return false;
    std::sort(list.begin(), list.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    const int count = std::min(static_cast<int>(list.size()), kMaxRects);
    float rect_v[kMaxRects * 4], height_v[kMaxRects];
    for (int i = 0; i < count; ++i) {
        const WaterRect& r = list[static_cast<std::size_t>(i)].second;
        rect_v[i * 4 + 0] = std::min(r.x0, r.x1);
        rect_v[i * 4 + 1] = std::min(r.y0, r.y1);
        rect_v[i * 4 + 2] = std::max(r.x0, r.x1);
        rect_v[i * 4 + 3] = std::max(r.y0, r.y1);
        height_v[i] = r.z;
    }

    state_changed_ = true;
    // The scene so far: colour and depth copies (one sample per pixel).
    glBlitNamedFramebuffer(in.scene_fbo, colour_fbo_, 0, 0, w_, h_, 0, 0, w_, h_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBlitNamedFramebuffer(in.scene_fbo, depth_fbo_, 0, 0, w_, h_, 0, 0, w_, h_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // The environment panorama: started over (the fog's colour) on the first use and when the camera jumps;
    // then what the picture shows of the sky and far scenery blends in (all at once on a fresh start).
    const float dx = cam[0] - env_cam_[0], dy = cam[1] - env_cam_[1], dz = cam[2] - env_cam_[2];
    const bool fresh = !env_valid_ || dx * dx + dy * dy + dz * dz > 3000.0f * 3000.0f;
    std::memcpy(env_cam_, cam, sizeof env_cam_);
    if (fresh) {
        const float c[4] = {in.fog ? in.fog_colour[0] : 0.6f, in.fog ? in.fog_colour[1] : 0.65f, in.fog ? in.fog_colour[2] : 0.7f, 1.0f};
        glClearNamedFramebufferfv(env_fbo_, GL_COLOR, 0, c);
        env_valid_ = true;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, env_fbo_);
    glViewport(0, 0, kEnvW, kEnvH);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glDepthMask(0);
    glColorMask(1, 1, 1, 1);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(prog_env_);
    set4(prog_env_, "u_lin", p[10], p[14], in.depth_min, in.depth_max);
    set4(prog_env_, "u_proj", 1.0f / p[0], 1.0f / p[5], p[8], p[9]);
    set4(prog_env_, "u_vp", static_cast<float>(in.vx), static_cast<float>(in.vy), static_cast<float>(in.vw), static_cast<float>(in.vh));
    glProgramUniformMatrix4fv(prog_env_, glGetUniformLocation(prog_env_, "u_view"), 1, GL_FALSE_, v);
    const float env_size[2] = {static_cast<float>(kEnvW), static_cast<float>(kEnvH)};
    glProgramUniform2fv(prog_env_, glGetUniformLocation(prog_env_, "u_env_size"), 1, env_size);
    const float far = 1500.0f, rate = fresh ? 1.0f : 0.2f;
    glProgramUniform1fv(prog_env_, glGetUniformLocation(prog_env_, "u_far"), 1, &far);
    glProgramUniform1fv(prog_env_, glGetUniformLocation(prog_env_, "u_rate"), 1, &rate);
    glBindVertexArray(vao_);
    glBindTextureUnit(0, depth_tex_);
    glBindSampler(0, 0);
    glBindTextureUnit(1, colour_tex_);
    glBindSampler(1, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisable(GL_BLEND);
    glDepthMask(1);

    const WaterParams& wp = water_params();
    static const auto t0 = std::chrono::steady_clock::now();
    const float seconds = static_cast<float>(std::fmod(std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count(), 3600.0));

    // What the rivers drawn after this (the transparent pass) shade with.
    {
        RiverFrame& f = river_;
        const float lin[4] = {p[10], p[14], in.depth_min, in.depth_max};
        const float proj[4] = {1.0f / p[0], 1.0f / p[5], p[8], p[9]};
        const float vp[4] = {static_cast<float>(in.vx), static_cast<float>(in.vy), static_cast<float>(in.vw), static_cast<float>(in.vh)};
        std::memcpy(f.lin, lin, sizeof lin);
        std::memcpy(f.proj, proj, sizeof proj);
        std::memcpy(f.vp, vp, sizeof vp);
        f.size[0] = static_cast<float>(w_);
        f.size[1] = static_cast<float>(h_);
        std::memcpy(f.view_inv, inv, sizeof inv);
        std::memcpy(f.view, v, sizeof f.view);
        const float fog[4] = {in.fog_start, in.fog_end, in.fog ? 1.0f : 0.0f, in.range_fog ? 1.0f : 0.0f};
        std::memcpy(f.fog, fog, sizeof fog);
        std::memcpy(f.fog_colour, in.fog_colour, sizeof f.fog_colour);
        const float params[4] = {seconds, 1.0f / std::max(wp.clarity, 1.0f), wp.waves, wp.reflection};
        std::memcpy(f.params, params, sizeof params);
        river_scene_ = g_scenes;
        river_overlay_ = in.overlay_depth;
        river_overlay_before_ = in.overlay_before;
        g_scene_copies = true;
    }
    if (count == 0) return false;  // only rivers in sight: they are shaded as they are drawn

    glBindFramebuffer(GL_FRAMEBUFFER, in.scene_fbo);
    glViewport(0, 0, w_, h_);
    glEnable(GL_SCISSOR_TEST);
    glScissor(in.vx, in.vy, in.vw, in.vh);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);  // the shader decides (the surface against the copied depth) and writes the depth
    glDepthMask(1);
    glDepthRangef(0.0f, 1.0f);
    glColorMask(1, 1, 1, 0);
    glColorMaski(1, 0, 0, 0, 0);  // [Graphics] AmbientOcclusion's cut-out target, if attached
    glUseProgram(prog_);
    set4(prog_, "u_lin", p[10], p[14], in.depth_min, in.depth_max);
    set4(prog_, "u_proj", 1.0f / p[0], 1.0f / p[5], p[8], p[9]);
    set4(prog_, "u_vp", static_cast<float>(in.vx), static_cast<float>(in.vy), static_cast<float>(in.vw), static_cast<float>(in.vh));
    const float size[2] = {static_cast<float>(w_), static_cast<float>(h_)};
    glProgramUniform2fv(prog_, glGetUniformLocation(prog_, "u_size"), 1, size);
    glProgramUniformMatrix4fv(prog_, glGetUniformLocation(prog_, "u_view_inv"), 1, GL_FALSE_, inv);
    glProgramUniformMatrix4fv(prog_, glGetUniformLocation(prog_, "u_view"), 1, GL_FALSE_, v);
    if (in.sun) {
        const float l = std::sqrt(in.sun[0] * in.sun[0] + in.sun[1] * in.sun[1] + in.sun[2] * in.sun[2]);
        if (l > 1e-6f) set4(prog_, "u_sun", in.sun[0] / l, in.sun[1] / l, in.sun[2] / l, 1.0f);
        else set4(prog_, "u_sun", 0.0f, 0.0f, -1.0f, 0.0f);
    } else {
        set4(prog_, "u_sun", 0.0f, 0.0f, -1.0f, 0.0f);
    }
    set4(prog_, "u_fog", in.fog_start, in.fog_end, in.fog ? 1.0f : 0.0f, in.range_fog ? 1.0f : 0.0f);
    glProgramUniform3fv(prog_, glGetUniformLocation(prog_, "u_fog_colour"), 1, in.fog_colour);
    set4(prog_, "u_params", seconds, 1.0f / std::max(wp.clarity, 1.0f), wp.waves, wp.reflection);
    glProgramUniform1i(prog_, glGetUniformLocation(prog_, "u_count"), count);
    glProgramUniform4fv(prog_, glGetUniformLocation(prog_, "u_rect"), count, rect_v);
    glProgramUniform1fv(prog_, glGetUniformLocation(prog_, "u_height"), count, height_v);
    // The waterfalls nearest the camera (at most 8).
    std::vector<std::pair<float, WaterRect>> near_falls;
    for (const WaterRect& r : falls) {
        const float fx = 0.5f * (r.x0 + r.x1) - cam[0], fy = 0.5f * (r.y0 + r.y1) - cam[1];
        near_falls.emplace_back(fx * fx + fy * fy, r);
    }
    std::sort(near_falls.begin(), near_falls.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    const int fall_count = std::min(static_cast<int>(near_falls.size()), 8);
    float fall_v[8 * 4], fall_z[8];
    for (int i = 0; i < fall_count; ++i) {
        const WaterRect& r = near_falls[static_cast<std::size_t>(i)].second;
        fall_v[i * 4 + 0] = r.x0;
        fall_v[i * 4 + 1] = r.y0;
        fall_v[i * 4 + 2] = r.x1;
        fall_v[i * 4 + 3] = r.y1;
        fall_z[i] = r.z;
    }
    glProgramUniform1i(prog_, glGetUniformLocation(prog_, "u_fall_count"), fall_count);
    if (fall_count > 0) {
        glProgramUniform4fv(prog_, glGetUniformLocation(prog_, "u_fall"), fall_count, fall_v);
        glProgramUniform1fv(prog_, glGetUniformLocation(prog_, "u_fall_z"), fall_count, fall_z);
    }
    glBindVertexArray(vao_);
    glBindTextureUnit(0, depth_tex_);
    glBindSampler(0, 0);
    glBindTextureUnit(1, colour_tex_);
    glBindSampler(1, 0);
    glBindTextureUnit(2, env_tex_);
    const bool overlay = in.overlay_depth && in.overlay_before;
    glProgramUniform1i(prog_, glGetUniformLocation(prog_, "u_overlay_on"), overlay ? 1 : 0);
    if (overlay) {
        glBindTextureUnit(3, in.overlay_depth);
        glBindSampler(3, 0);
        glBindTextureUnit(4, in.overlay_before);
        glBindSampler(4, 0);
    }
    glBindSampler(2, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_SCISSOR_TEST);
    glColorMask(1, 1, 1, 1);
    if (logged_ < 3) {
        ++logged_;
        hook::log_line("[Graphics] Water: %d of %zu water surfaces shaded (clarity %.0f, waves %.2f, reflection %.2f)", count,
                       rects.size(), static_cast<double>(wp.clarity), static_cast<double>(wp.waves),
                       static_cast<double>(wp.reflection));
    }
    return true;
}

bool WaterPass::river_ready() const { return g_scene_copies && river_scene_ == g_scenes && prog_ != 0; }

GLuint WaterPass::river_program(Renderer& r, GLuint program, int stage) {
    if (stage < 0 || stage > 3) return 0;
    const std::string fs = "#version 450 core\n#define STAGE " + std::to_string(stage) + "\n" + kRiverFs;
    const GLuint v = r.fs_variant(program, 0x52000000u | static_cast<std::uint32_t>(stage), fs);
    if (v && river_logged_ < 3) {
        ++river_logged_;
        hook::log_line("[Graphics] Water: river drawn with the water shader (texture stage %d)", stage);
    }
    return v;
}

void WaterPass::river_bind(GLuint program) {
    // The uniforms once per program and scene.
    auto it = std::find_if(river_programs_bound_.begin(), river_programs_bound_.end(), [program](const auto& e) { return e.first == program; });
    if (it == river_programs_bound_.end()) {
        river_programs_bound_.emplace_back(program, 0u);
        it = river_programs_bound_.end() - 1;
    }
    if (it->second != river_scene_) {
        it->second = river_scene_;
        const RiverFrame& f = river_;
        glProgramUniform4fv(program, glGetUniformLocation(program, "u_w_lin"), 1, f.lin);
        glProgramUniform4fv(program, glGetUniformLocation(program, "u_w_proj"), 1, f.proj);
        glProgramUniform4fv(program, glGetUniformLocation(program, "u_w_vp"), 1, f.vp);
        glProgramUniform2fv(program, glGetUniformLocation(program, "u_w_size"), 1, f.size);
        glProgramUniformMatrix4fv(program, glGetUniformLocation(program, "u_w_view_inv"), 1, GL_FALSE_, f.view_inv);
        glProgramUniformMatrix4fv(program, glGetUniformLocation(program, "u_w_view"), 1, GL_FALSE_, f.view);
        glProgramUniform4fv(program, glGetUniformLocation(program, "u_w_fog"), 1, f.fog);
        glProgramUniform3fv(program, glGetUniformLocation(program, "u_w_fog_colour"), 1, f.fog_colour);
        glProgramUniform4fv(program, glGetUniformLocation(program, "u_w_params"), 1, f.params);
    }
    glProgramUniform1i(program, glGetUniformLocation(program, "u_w_blended"), g_river_blended ? 1 : 0);
    glBindTextureUnit(8, depth_tex_);
    glBindSampler(8, 0);
    glBindTextureUnit(9, colour_tex_);
    glBindSampler(9, 0);
    glBindTextureUnit(10, env_tex_);
    glBindSampler(10, 0);
    const bool overlay = river_overlay_ && river_overlay_before_;
    glProgramUniform1i(program, glGetUniformLocation(program, "u_w_overlay_on"), overlay ? 1 : 0);
    if (overlay) {
        glBindTextureUnit(11, river_overlay_);
        glBindSampler(11, 0);
        glBindTextureUnit(12, river_overlay_before_);
        glBindSampler(12, 0);
    }
}

}  // namespace joan::render
