// [Graphics] AmbientOcclusion - see ambient_occlusion.hpp.
#include "ambient_occlusion.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "../registry.hpp"
#include "ao_pass.hpp"
#include "renderer.hpp"

namespace joan::render {

using namespace gl;

namespace {

OpaqueSceneHook g_hook = nullptr;
TransparentSceneHook g_transparent_hook = nullptr;

AoParams initial_params() {
    const auto& s = hook::settings();
    AoParams p;
    p.mode = std::clamp(s.ao_mode, 0, 2);
    if (s.ao_radius > 0.0f) p.radius = s.ao_radius;
    if (s.ao_strength >= 0.0f) p.strength = s.ao_strength;
    if (s.ao_fade > 0.0f) {
        p.fade_start = s.ao_fade;
        p.fade_end = s.ao_fade * 2.0f;
    }
    return p;
}

// --- shaders --------------------------------------------------------------------------
// Every pass draws one triangle covering the target (no vertex buffer). Pixel positions are
// gl_FragCoord (row 0 = the game's top row, glClipControl(GL_UPPER_LEFT)); all reads are
// texelFetch, so no sampler state matters.
const char* const kFullscreenVs = R"(#version 450 core
void main() {
    vec2 p = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}
)";

// Full-resolution depth -> half-resolution view-space distance (0 = nothing drawn there).
const char* const kLinearFs = R"(#version 450 core
layout(binding = 0) uniform sampler2D u_depth;
uniform vec4 u_lin;  // P33, P43, depth range min, depth range max
out float o_z;
void main() {
    ivec2 lim = textureSize(u_depth, 0) - 1;
    ivec2 p = min(ivec2(gl_FragCoord.xy) * 2, lim);
    float d = (texelFetch(u_depth, p, 0).r - u_lin.z) / max(u_lin.w - u_lin.z, 1e-6);
    float z = (d >= 0.999999) ? 0.0 : u_lin.y / (d - u_lin.x);
    o_z = (z > 0.0 && z < 1e7) ? z : 0.0;
}
)";

// Phase 2: the cut-out depth -> half-resolution (view distance, 1 = a cut-out is nearer than
// the opaque world there).
const char* const kLinearCutFs = R"(#version 450 core
layout(binding = 0) uniform sampler2D u_depth;   // opaque world + cut-outs
layout(binding = 1) uniform sampler2D u_opaque;  // opaque world (phase 1)
uniform vec4 u_lin;  // P33, P43, depth range min, depth range max
out vec2 o_z;
void main() {
    ivec2 lim = textureSize(u_depth, 0) - 1;
    ivec2 p = min(ivec2(gl_FragCoord.xy) * 2, lim);
    float dc = texelFetch(u_depth, p, 0).r;
    float d = (dc - u_lin.z) / max(u_lin.w - u_lin.z, 1e-6);
    float z = (d >= 0.999999) ? 0.0 : u_lin.y / (d - u_lin.x);
    bool valid = z > 0.0 && z < 1e7;
    o_z = vec2(valid ? z : 0.0, (valid && dc < texelFetch(u_opaque, p, 0).r) ? 1.0 : 0.0);
}
)";

// Phase 2, full resolution: the cut-out target (nearest cut-out window depth, 1.0 = none; sample 0
// of a multisampled one, like the depth blits) -> opaque world + cut-outs. Compiled with MS 0 / 1.
const char* const kCutResolveFsBody = R"(
#if MS
layout(binding = 0) uniform sampler2DMS u_cut;
#else
layout(binding = 0) uniform sampler2D u_cut;
#endif
layout(binding = 1) uniform sampler2D u_opaque;  // opaque world (phase 1)
out float o_depth;
void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    o_depth = min(texelFetch(u_cut, p, 0).r, texelFetch(u_opaque, p, 0).r);
}
)";

// Half-resolution occlusion. Compiled twice: CUTOUT 0 (phase 1) and CUTOUT 1 (phase 2: a
// cut-out pixel gets the occlusion of everything around it, an opaque pixel only the extra
// occlusion of the cut-outs near it - its own was applied in phase 1).
const char* const kAoFsBody = R"(
layout(binding = 0) uniform sampler2D u_z;  // r: view distance; g (CUTOUT): 1 = cut-out pixel
uniform vec4 u_proj;  // 1/P11, 1/P22, P31, P32
uniform vec4 u_vp;    // viewport x, y, w, h (full-resolution pixels)
uniform vec4 u_ao;    // radius, strength, pixels per unit at distance 1 (full res), max radius (full res px)
uniform vec4 u_fade;  // start, end
out float o_ao;

vec3 view_pos(vec2 px, float z) {
    vec2 ndc = vec2((px.x - u_vp.x) / u_vp.z * 2.0 - 1.0, 1.0 - (px.y - u_vp.y) / u_vp.w * 2.0);
    return vec3((ndc - u_proj.zw) * u_proj.xy * z, z);
}
vec4 fetch(ivec2 h, out float cut) {  // xyz = view position, w = 1 if valid
    cut = 0.0;
    ivec2 lim = textureSize(u_z, 0) - 1;
    if (h.x < 0 || h.y < 0 || h.x > lim.x || h.y > lim.y) return vec4(0.0);
    vec2 t = texelFetch(u_z, h, 0).rg;
#if CUTOUT
    cut = t.y;
#endif
    return t.x > 0.0 ? vec4(view_pos(vec2(h) * 2.0 + 0.5, t.x), 1.0) : vec4(0.0);
}

void main() {
    ivec2 c = ivec2(gl_FragCoord.xy);
    float cut_c, unused;
    vec4 P4 = fetch(c, cut_c);
    if (P4.w == 0.0) { o_ao = 1.0; return; }
    vec3 P = P4.xyz;
    float fade = 1.0 - smoothstep(u_fade.x, u_fade.y, P.z);
    if (fade <= 0.0) { o_ao = 1.0; return; }

    // Normal from the neighbours, using the side with the smaller depth step (no smearing
    // across silhouettes).
    vec4 l = fetch(c - ivec2(1, 0), unused), r = fetch(c + ivec2(1, 0), unused);
    vec4 u = fetch(c - ivec2(0, 1), unused), d = fetch(c + ivec2(0, 1), unused);
    bool use_r = r.w > 0.0 && (l.w == 0.0 || abs(r.z - P.z) < abs(l.z - P.z));
    bool use_d = d.w > 0.0 && (u.w == 0.0 || abs(d.z - P.z) < abs(u.z - P.z));
    if ((!use_r && l.w == 0.0) || (!use_d && u.w == 0.0)) { o_ao = 1.0; return; }
    vec3 dx = use_r ? r.xyz - P : P - l.xyz;
    vec3 dy = use_d ? d.xyz - P : P - u.xyz;
    vec3 n = cross(dx, dy);
    float nl = length(n);
    if (nl <= 0.0) { o_ao = 1.0; return; }
    n /= nl;
    if (dot(n, P) > 0.0) n = -n;  // towards the camera (at the origin)

    float radius = u_ao.x;
    float rad_px = min(radius * u_ao.z / P.z, u_ao.w);
    if (rad_px < 3.0) { o_ao = 1.0; return; }
    // Per-pixel rotation (interleaved gradient noise); the blur removes the pattern.
    float noise = fract(52.9829189 * fract(dot(gl_FragCoord.xy, vec2(0.06711056, 0.00583715))));
    const int N = 12;
    float occ = 0.0, r2 = radius * radius;
    vec2 centre = vec2(c) * 2.0 + 0.5;
    for (int i = 0; i < N; ++i) {
        float t = (float(i) + noise) / float(N);
        float a = noise * 6.2831853 + float(i) * 2.39996323;
        vec2 off = vec2(cos(a), sin(a)) * rad_px * max(t, 1.5 / rad_px);
        ivec2 s = ivec2(floor((centre + off) * 0.5));
        float cut_s;
        vec4 S = fetch(s, cut_s);
        if (S.w == 0.0) continue;
#if CUTOUT
        if (cut_c < 0.5 && cut_s < 0.5) continue;
#endif
        vec3 v = S.xyz - P;
        float vv = dot(v, v);
        float cosv = dot(v, n) * inversesqrt(vv + 1e-4);
        float fall = clamp(1.0 - vv / r2, 0.0, 1.0);
        occ += max(cosv - 0.15, 0.0) * fall;
    }
    float ao = 1.0 - clamp(occ / float(N) * 2.4 * u_ao.y, 0.0, 1.0);
    o_ao = mix(1.0, ao, fade);
}
)";

// Half-resolution depth-aware blur (9 taps along u_dir).
const char* const kBlurFs = R"(#version 450 core
layout(binding = 0) uniform sampler2D u_src;
layout(binding = 1) uniform sampler2D u_z;
uniform vec4 u_dir;  // x, y step in texels
out float o_ao;
void main() {
    ivec2 c = ivec2(gl_FragCoord.xy);
    ivec2 lim = textureSize(u_src, 0) - 1;
    float zc = texelFetch(u_z, c, 0).r;
    float a = texelFetch(u_src, c, 0).r;
    if (zc <= 0.0) { o_ao = a; return; }
    float sum = a, wsum = 1.0;
    ivec2 dir = ivec2(u_dir.xy);
    for (int i = -4; i <= 4; ++i) {
        if (i == 0) continue;
        ivec2 p = clamp(c + dir * i, ivec2(0), lim);
        float z = texelFetch(u_z, p, 0).r;
        if (z <= 0.0) continue;
        float w = exp(-float(i * i) / 12.0) * max(0.0, 1.0 - abs(z - zc) / (0.04 * zc + 1.0));
        sum += texelFetch(u_src, p, 0).r * w;
        wsum += w;
    }
    o_ao = sum / wsum;
}
)";

// Depth-aware upsampling of the half-resolution occlusion at full-resolution pixel p, whose
// view distance is z (shared by both composites).
const char* const kUpsampleGlsl = R"(
layout(binding = 0) uniform sampler2D u_ao;
layout(binding = 1) uniform sampler2D u_z;
float upsample(ivec2 p, float z) {
    ivec2 lim = textureSize(u_ao, 0) - 1;
    vec2 h = vec2(p) * 0.5;  // half-res texel q holds full-res pixel 2q
    ivec2 b = ivec2(floor(h));
    vec2 f = h - vec2(b);
    float sum = 0.0, wsum = 0.0, best = 1.0, best_dz = 1e30;
    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 2; ++i) {
            ivec2 q = clamp(b + ivec2(i, j), ivec2(0), lim);
            float zq = texelFetch(u_z, q, 0).r;
            float a = texelFetch(u_ao, q, 0).r;
            float dz = abs(zq - z);
            if (dz < best_dz) { best_dz = dz; best = a; }
            float bw = (i == 0 ? 1.0 - f.x : f.x) * (j == 0 ? 1.0 - f.y : f.y);
            float w = bw / (1e-3 + dz / (0.02 * z + 0.5));
            if (zq > 0.0) { sum += a * w; wsum += w; }
        }
    return (wsum > 1e-6 && best_dz < 0.05 * z + 2.0) ? sum / wsum : best;
}
)";

// Phase 1, full resolution: onto the picture (blend: colour *= occlusion; mode 2 replaces).
const char* const kCompositeFs = R"(
layout(binding = 2) uniform sampler2D u_depth;
uniform vec4 u_lin;   // P33, P43, depth range min, depth range max
uniform vec4 u_mode;  // x: 2 = show the occlusion only
out vec4 o_colour;
void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    float d = (texelFetch(u_depth, p, 0).r - u_lin.z) / max(u_lin.w - u_lin.z, 1e-6);
    if (d >= 0.999999) {
        if (u_mode.x == 2.0) { o_colour = vec4(1.0); return; }
        discard;
    }
    float ao = upsample(p, u_lin.y / (d - u_lin.x));
    o_colour = vec4(ao, ao, ao, 1.0);
}
)";

// Phase 2, full resolution: only where the cut-out depth is still the nearest depth-writing
// surface after the transparent pass (not under water, a fading unit, ...). Mode 1 blends
// colour *= occlusion. Mode 2 blends ONE / SRC_ALPHA: a cut-out pixel (drawn over phase 1's
// grey picture) is replaced by its occlusion, an opaque pixel multiplied by the extra one.
const char* const kCompositeCutFs = R"(
layout(binding = 2) uniform sampler2D u_depth;   // opaque world + cut-outs
layout(binding = 3) uniform sampler2D u_opaque;  // opaque world (phase 1)
layout(binding = 4) uniform sampler2D u_final;   // the scene after the transparent pass
uniform vec4 u_lin;   // P33, P43, depth range min, depth range max
uniform vec4 u_mode;  // x: 2 = show the occlusion only
out vec4 o_colour;
float view_z(float depth) {
    float d = (depth - u_lin.z) / max(u_lin.w - u_lin.z, 1e-6);
    return d >= 0.999999 ? 0.0 : u_lin.y / (d - u_lin.x);
}
void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    float dc = texelFetch(u_depth, p, 0).r;
    float z = view_z(dc);
    if (z <= 0.0) discard;
    // The scene's depth may come from another sample of a multisampled pixel: same surface
    // within 2% (+1 unit).
    float zf = view_z(texelFetch(u_final, p, 0).r);
    if (zf <= 0.0 || abs(zf - z) > 0.02 * z + 1.0) discard;
    bool cut = dc < texelFetch(u_opaque, p, 0).r;
    float ao = upsample(p, z);
    if (u_mode.x == 2.0)
        o_colour = cut ? vec4(ao, ao, ao, 0.0) : vec4(0.0, 0.0, 0.0, ao);
    else
        o_colour = vec4(ao, ao, ao, 1.0);
}
)";

GLuint make_texture(GLenum format, int w, int h) {
    GLuint t = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &t);
    glTextureStorage2D(t, 1, format, w, h);
    glTextureParameteri(t, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(t, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(t, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(t, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return t;
}

GLuint make_fbo(GLenum attachment, GLuint texture) {
    GLuint f = 0;
    glCreateFramebuffers(1, &f);
    glNamedFramebufferTexture(f, attachment, texture, 0);
    if (attachment == GL_COLOR_ATTACHMENT0) {
        glNamedFramebufferDrawBuffer(f, GL_COLOR_ATTACHMENT0);
        glNamedFramebufferReadBuffer(f, GL_COLOR_ATTACHMENT0);
    } else {
        glNamedFramebufferDrawBuffer(f, GL_NONE_);
        glNamedFramebufferReadBuffer(f, GL_NONE_);
    }
    return f;
}

void set4(GLuint program, const char* name, float x, float y, float z, float w) {
    const float v[4] = {x, y, z, w};
    glProgramUniform4fv(program, glGetUniformLocation(program, name), 1, v);
}

// Direct3D perspective (row vectors): x' = x P11 + z P31, y' = y P22 + z P32,
// z' = z P33 + P43, w' = z P34 (P34 = 1). Anything else (orthographic, odd) is skipped.
struct Projection {
    float p11, p22, p31, p32, p33, p43;
};
bool decode(const float* m, Projection& p) {
    p = {m[0], m[5], m[8], m[9], m[10], m[14]};
    return m[11] == 1.0f && p.p11 != 0.0f && p.p22 != 0.0f && p.p43 < 0.0f && p.p33 > 1.0f;  // LH: P33 = f / (f - n) > 1
}

// State shared by every pass.
void set_common_state(GLuint vao) {
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMask(1, 1, 1, 1);
    glBindVertexArray(vao);
    for (GLuint u = 0; u < 5; ++u) glBindSampler(u, 0);
}

void set_occlusion_uniforms(GLuint program, const AoPass::Input& in, const Projection& m, const AoParams& p) {
    set4(program, "u_proj", 1.0f / m.p11, 1.0f / m.p22, m.p31, m.p32);
    set4(program, "u_vp", static_cast<float>(in.vx), static_cast<float>(in.vy), static_cast<float>(in.vw),
         static_cast<float>(in.vh));
    const float px_per_unit = m.p22 * 0.5f * static_cast<float>(in.vh);  // projected size of 1 unit at distance 1
    set4(program, "u_ao", p.radius, p.strength, px_per_unit, 0.12f * static_cast<float>(in.vh));
    set4(program, "u_fade", p.fade_start, std::max(p.fade_end, p.fade_start + 1.0f), 0.0f, 0.0f);
}

}  // namespace

AoParams& ao_params() {
    static AoParams p = initial_params();
    return p;
}

void set_opaque_scene_hook(OpaqueSceneHook hook) { g_hook = hook; }
void set_transparent_scene_hook(TransparentSceneHook hook) { g_transparent_hook = hook; }

void opaque_scene_done(const float* projection) {
    if (g_hook && projection && ao_params().mode != 0) g_hook(projection);
}

void transparent_scene_done(const float* projection) {
    if (g_transparent_hook && projection) g_transparent_hook(projection);
}

// --- AoPass ---------------------------------------------------------------------------
bool AoPass::ensure_programs() {
    if (prog_composite_cut_) return true;
    Renderer* r = current();
    if (!r) return false;
    const std::string head = "#version 450 core\n";
    prog_linear_ = r->compile_program(kFullscreenVs, kLinearFs, "ambient occlusion: depth");
    prog_ao_ = r->compile_program(kFullscreenVs, head + "#define CUTOUT 0\n" + kAoFsBody, "ambient occlusion: occlusion");
    prog_blur_ = r->compile_program(kFullscreenVs, kBlurFs, "ambient occlusion: blur");
    prog_composite_ = r->compile_program(kFullscreenVs, head + kUpsampleGlsl + kCompositeFs, "ambient occlusion: composite");
    prog_linear_cut_ = r->compile_program(kFullscreenVs, kLinearCutFs, "ambient occlusion: cut-out depth");
    prog_ao_cut_ = r->compile_program(kFullscreenVs, head + "#define CUTOUT 1\n" + kAoFsBody,
                                      "ambient occlusion: cut-out occlusion");
    prog_composite_cut_ = r->compile_program(kFullscreenVs, head + kUpsampleGlsl + kCompositeCutFs,
                                             "ambient occlusion: cut-out composite");
    prog_cut_resolve_[0] = r->compile_program(kFullscreenVs, head + "#define MS 0\n" + kCutResolveFsBody,
                                              "ambient occlusion: cut-out resolve");
    prog_cut_resolve_[1] = r->compile_program(kFullscreenVs, head + "#define MS 1\n" + kCutResolveFsBody,
                                              "ambient occlusion: cut-out resolve (multisampled)");
    if (!prog_linear_ || !prog_ao_ || !prog_blur_ || !prog_composite_ || !prog_linear_cut_ || !prog_ao_cut_ ||
        !prog_composite_cut_ || !prog_cut_resolve_[0] || !prog_cut_resolve_[1]) {
        hook::log_line("[Graphics] AmbientOcclusion: shader compilation failed - ambient occlusion off");
        shutdown();
        failed_ = true;
        return false;
    }
    glCreateVertexArrays(1, &vao_);
    return true;
}

void AoPass::release_targets() {
    end_cutout();
    const GLuint fbos[] = {depth_fbo_, z_fbo_, ao_fbo_[0], ao_fbo_[1], cut_fbo_, final_fbo_, zc_fbo_};
    const GLuint textures[] = {depth_tex_, z_tex_, ao_tex_[0], ao_tex_[1], cut_tex_, final_tex_, zc_tex_, cutz_tex_};
    for (GLuint f : fbos)
        if (f) glDeleteFramebuffers(1, &f);
    for (GLuint t : textures)
        if (t) glDeleteTextures(1, &t);
    depth_fbo_ = z_fbo_ = depth_tex_ = z_tex_ = 0;
    ao_fbo_[0] = ao_fbo_[1] = ao_tex_[0] = ao_tex_[1] = 0;
    cut_fbo_ = final_fbo_ = zc_fbo_ = cut_tex_ = final_tex_ = zc_tex_ = 0;
    cutz_tex_ = 0;
    cutz_samples_ = -1;
    cut_ready_ = false;
    w_ = h_ = 0;
}

bool AoPass::ensure_targets(int width, int height) {
    if (w_ == width && h_ == height && depth_fbo_) return true;
    release_targets();
    w_ = width;
    h_ = height;
    hw_ = (width + 1) / 2;
    hh_ = (height + 1) / 2;
    depth_tex_ = make_texture(GL_DEPTH24_STENCIL8, w_, h_);
    depth_fbo_ = make_fbo(GL_DEPTH_STENCIL_ATTACHMENT, depth_tex_);
    z_tex_ = make_texture(GL_R32F, hw_, hh_);
    z_fbo_ = make_fbo(GL_COLOR_ATTACHMENT0, z_tex_);
    for (int i = 0; i < 2; ++i) {
        ao_tex_[i] = make_texture(GL_R8, hw_, hh_);
        ao_fbo_[i] = make_fbo(GL_COLOR_ATTACHMENT0, ao_tex_[i]);
    }
    const GLuint fbos[] = {depth_fbo_, z_fbo_, ao_fbo_[0], ao_fbo_[1]};
    for (GLuint f : fbos)
        if (glCheckNamedFramebufferStatus(f, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            hook::log_line("[Graphics] AmbientOcclusion: framebuffer incomplete - ambient occlusion off");
            shutdown();
            failed_ = true;
            return false;
        }
    hook::log_line("[Graphics] AmbientOcclusion: buffers %dx%d (occlusion at %dx%d)", w_, h_, hw_, hh_);
    return true;
}

void AoPass::shutdown() {
    const GLuint programs[] = {prog_linear_,     prog_ao_,           prog_blur_,          prog_composite_,     prog_linear_cut_,
                               prog_ao_cut_,     prog_composite_cut_, prog_cut_resolve_[0], prog_cut_resolve_[1]};
    for (GLuint p : programs)
        if (p) glDeleteProgram(p);
    prog_linear_ = prog_ao_ = prog_blur_ = prog_composite_ = 0;
    prog_linear_cut_ = prog_ao_cut_ = prog_composite_cut_ = 0;
    prog_cut_resolve_[0] = prog_cut_resolve_[1] = 0;
    if (vao_) glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
    release_targets();
}

bool AoPass::run(const Input& in) {
    const AoParams& p = ao_params();
    cut_ready_ = false;
    if (failed_ || p.mode == 0 || in.width <= 0 || in.height <= 0 || in.vw <= 0 || in.vh <= 0) return false;
    Projection m;
    if (!decode(in.projection, m)) return false;
    if (!ensure_programs() || !ensure_targets(in.width, in.height)) return false;
    ++frames_;

    const float znear = -m.p43 / m.p33, zfar = m.p43 / (1.0f - m.p33);
    if (log_count_ < 20 && (std::fabs(znear - logged_near_) > 0.01f * znear || std::fabs(zfar - logged_far_) > 0.01f * zfar)) {
        ++log_count_;
        logged_near_ = znear;
        logged_far_ = zfar;
        hook::log_line("[Graphics] AmbientOcclusion: camera near %.2f far %.1f, viewport %d,%d %dx%d (frame %u; "
                       "radius %.1f strength %.2f fade %.0f..%.0f)",
                       static_cast<double>(znear), static_cast<double>(zfar), in.vx, in.vy, in.vw, in.vh, frames_,
                       static_cast<double>(p.radius), static_cast<double>(p.strength),
                       static_cast<double>(p.fade_start), static_cast<double>(p.fade_end));
    }

    set_common_state(vao_);

    // 1. Depth copy (the multisampled depth resolves to one sample per pixel); phase 2 resolves
    //    its cut-outs against it. (The stencil comes along with the packed format.)
    glBlitNamedFramebuffer(in.scene_fbo, depth_fbo_, 0, 0, w_, h_, 0, 0, w_, h_,
                           GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

    // 2. View distance at half resolution.
    glBindFramebuffer(GL_FRAMEBUFFER, z_fbo_);
    glViewport(0, 0, hw_, hh_);
    glUseProgram(prog_linear_);
    set4(prog_linear_, "u_lin", m.p33, m.p43, in.depth_min, in.depth_max);
    glBindTextureUnit(0, depth_tex_);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 3. Occlusion.
    glBindFramebuffer(GL_FRAMEBUFFER, ao_fbo_[0]);
    glUseProgram(prog_ao_);
    set_occlusion_uniforms(prog_ao_, in, m, p);
    glBindTextureUnit(0, z_tex_);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 4. Blur, 5. onto the picture.
    blur_and_composite(in, z_tex_, prog_composite_);
    if (p.mode != 2) {
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);  // colour *= occlusion
    }
    glUseProgram(prog_composite_);
    set4(prog_composite_, "u_lin", m.p33, m.p43, in.depth_min, in.depth_max);
    set4(prog_composite_, "u_mode", static_cast<float>(p.mode), 0.0f, 0.0f, 0.0f);
    glBindTextureUnit(2, depth_tex_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisable(GL_BLEND);
    glColorMask(1, 1, 1, 1);
    glDisable(GL_SCISSOR_TEST);
    return true;
}

// Steps 4 and the set-up of 5 of both phases: blur the occlusion in ao_tex_[0] (horizontal into
// [1], vertical back into [0]) with the view distances `z_tex` (.r), then bind the scene
// framebuffer, scissored to the camera's viewport with alpha writes off, and the composite's
// inputs on units 0 / 1. The caller sets the blend, the composite's own uniforms and depths.
void AoPass::blur_and_composite(const Input& in, GLuint z_tex, GLuint composite_program) {
    glUseProgram(prog_blur_);
    glBindTextureUnit(1, z_tex);
    glBindFramebuffer(GL_FRAMEBUFFER, ao_fbo_[1]);
    set4(prog_blur_, "u_dir", 1.0f, 0.0f, 0.0f, 0.0f);
    glBindTextureUnit(0, ao_tex_[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindFramebuffer(GL_FRAMEBUFFER, ao_fbo_[0]);
    set4(prog_blur_, "u_dir", 0.0f, 1.0f, 0.0f, 0.0f);
    glBindTextureUnit(0, ao_tex_[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindFramebuffer(GL_FRAMEBUFFER, in.scene_fbo);
    glViewport(0, 0, w_, h_);
    glEnable(GL_SCISSOR_TEST);
    glScissor(in.vx, in.vy, in.vw, in.vh);
    glColorMask(1, 1, 1, 0);
    glUseProgram(composite_program);
    glBindTextureUnit(0, ao_tex_[0]);
    glBindTextureUnit(1, z_tex);
}

bool AoPass::begin_cutout(GLuint scene_fbo, int samples) {
    end_cutout();
    cut_ready_ = false;
    if (failed_ || cut_failed_ || !depth_fbo_ || !scene_fbo) return false;
    if (!cut_fbo_) {
        cut_tex_ = make_texture(GL_R32F, w_, h_);
        cut_fbo_ = make_fbo(GL_COLOR_ATTACHMENT0, cut_tex_);
        final_tex_ = make_texture(GL_DEPTH24_STENCIL8, w_, h_);
        final_fbo_ = make_fbo(GL_DEPTH_STENCIL_ATTACHMENT, final_tex_);
        zc_tex_ = make_texture(GL_RG32F, hw_, hh_);
        zc_fbo_ = make_fbo(GL_COLOR_ATTACHMENT0, zc_tex_);
        const GLuint fbos[] = {cut_fbo_, final_fbo_, zc_fbo_};
        for (GLuint f : fbos)
            if (glCheckNamedFramebufferStatus(f, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                hook::log_line("[Graphics] AmbientOcclusion: framebuffer incomplete - ambient occlusion off");
                shutdown();
                failed_ = true;
                return false;
            }
    }
    if (cutz_tex_ && cutz_samples_ != samples) {
        glDeleteTextures(1, &cutz_tex_);
        cutz_tex_ = 0;
    }
    if (!cutz_tex_) {
        if (samples > 0) {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &cutz_tex_);
            glTextureStorage2DMultisample(cutz_tex_, samples, GL_R32F, w_, h_, GL_TRUE_);  // fixed locations: next to renderbuffers
        } else {
            cutz_tex_ = make_texture(GL_R32F, w_, h_);
        }
        cutz_samples_ = samples;
    }
    glNamedFramebufferTexture(scene_fbo, GL_COLOR_ATTACHMENT1, cutz_tex_, 0);
    const GLenum buffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glNamedFramebufferDrawBuffers(scene_fbo, 2, buffers);
    cut_attached_fbo_ = scene_fbo;
    const GLenum status = glCheckNamedFramebufferStatus(scene_fbo, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        hook::log_line("[Graphics] AmbientOcclusion: scene framebuffer incomplete with the cut-out target (0x%x, %d samples) - "
                       "transparent-pass cut-outs left out",
                       status, samples);
        end_cutout();
        cut_failed_ = true;
        return false;
    }
    glDisable(GL_SCISSOR_TEST);
    glColorMaski(1, 1, 1, 1, 1);
    const float far_depth[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glClearNamedFramebufferfv(scene_fbo, GL_COLOR, 1, far_depth);
    cut_ready_ = true;
    return true;
}

void AoPass::cutout_draw_state(bool cutout) {
    glColorMaski(1, cutout ? 1 : 0, 0, 0, 0);
    if (!cutout) return;
    glEnablei(GL_BLEND, 1);
    glBlendEquationi(1, GL_MIN);
    glBlendFunci(1, GL_ONE, GL_ONE);
}

void AoPass::end_cutout() {
    if (!cut_attached_fbo_) return;
    glNamedFramebufferDrawBuffer(cut_attached_fbo_, GL_COLOR_ATTACHMENT0);
    glNamedFramebufferTexture(cut_attached_fbo_, GL_COLOR_ATTACHMENT1, 0, 0);
    cut_attached_fbo_ = 0;
}

void AoPass::run_cutout(const Input& in) {
    const AoParams& p = ao_params();
    end_cutout();
    if (failed_ || p.mode == 0 || !cut_ready_ || in.width != w_ || in.height != h_ || in.vw <= 0 || in.vh <= 0) return;
    cut_ready_ = false;
    Projection m;
    if (!decode(in.projection, m) || !ensure_programs()) return;
    if (cutout_frames_++ == 0)
        hook::log_line("[Graphics] AmbientOcclusion: transparent-pass cut-outs (foliage) added (frame %u)", frames_);

    set_common_state(vao_);

    // 0. Opaque world + cut-outs from the cut-out target.
    glBindFramebuffer(GL_FRAMEBUFFER, cut_fbo_);
    glViewport(0, 0, w_, h_);
    const GLuint resolve = prog_cut_resolve_[cutz_samples_ > 0 ? 1 : 0];
    glUseProgram(resolve);
    glBindTextureUnit(0, cutz_tex_);
    glBindTextureUnit(1, depth_tex_);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 1. The scene's depth after the transparent pass.
    glBlitNamedFramebuffer(in.scene_fbo, final_fbo_, 0, 0, w_, h_, 0, 0, w_, h_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // 2. View distance and cut-out flag of the cut-out depth at half resolution.
    glBindFramebuffer(GL_FRAMEBUFFER, zc_fbo_);
    glViewport(0, 0, hw_, hh_);
    glUseProgram(prog_linear_cut_);
    set4(prog_linear_cut_, "u_lin", m.p33, m.p43, in.depth_min, in.depth_max);
    glBindTextureUnit(0, cut_tex_);
    glBindTextureUnit(1, depth_tex_);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 3. Occlusion.
    glBindFramebuffer(GL_FRAMEBUFFER, ao_fbo_[0]);
    glUseProgram(prog_ao_cut_);
    set_occlusion_uniforms(prog_ao_cut_, in, m, p);
    glBindTextureUnit(0, zc_tex_);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 4. Blur, 5. onto the picture.
    blur_and_composite(in, zc_tex_, prog_composite_cut_);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    if (p.mode == 2)
        glBlendFuncSeparate(GL_ONE, GL_SRC_ALPHA, GL_ZERO, GL_ONE);  // cut-out: = occlusion; opaque: *= extra
    else
        glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);  // colour *= occlusion
    set4(prog_composite_cut_, "u_lin", m.p33, m.p43, in.depth_min, in.depth_max);
    set4(prog_composite_cut_, "u_mode", static_cast<float>(p.mode), 0.0f, 0.0f, 0.0f);
    glBindTextureUnit(2, cut_tex_);
    glBindTextureUnit(3, depth_tex_);
    glBindTextureUnit(4, final_tex_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisable(GL_BLEND);
    glColorMask(1, 1, 1, 1);
    glDisable(GL_SCISSOR_TEST);
}

}  // namespace joan::render
