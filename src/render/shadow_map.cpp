// [Graphics] Shadows=2 - see shadow_map.hpp.
#include "shadow_map.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "../registry.hpp"
#include "renderer.hpp"

namespace joan::render {

using namespace gl;

namespace {

ShadowBeginHook g_begin = nullptr;
ShadowDoneHook g_done = nullptr;

// Full-screen triangle (no vertex buffer), as the ambient occlusion passes.
const char* const kFullscreenVs = R"(#version 450 core
void main() {
    vec2 p = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}
)";

// The scene's depth -> view position -> the shadow map: blend colour *= the returned grey.
const char* const kApplyFs = R"(#version 450 core
layout(binding = 0) uniform sampler2D u_depth;
layout(binding = 1) uniform sampler2DShadow u_map;
layout(binding = 2) uniform sampler2D u_map_depth;  // the same map, raw depths (blocker search)
layout(binding = 4) uniform sampler2D u_overlay;  // the depth after the billboards drawn before the world
layout(binding = 5) uniform sampler2D u_overlay_before;  // and before them
uniform int u_overlay_on;
uniform int u_debug;  // ShadowParams::debug
uniform vec4 u_lin;    // P33, P43, depth range min, depth range max
uniform vec4 u_proj;    // 1/P11, 1/P22, P31, P32
uniform vec4 u_vp;      // viewport x, y, w, h (pixels)
uniform vec4 u_params;  // strength, filter radius (shadow map uv), fade start, fade end (world units)
uniform vec4 u_bias;    // offset towards the sun (world units), depth bias (map depth), world units per map depth,
                        // widest filter radius (uv)
uniform vec4 u_self;    // blocker distance (world units) where shadows start / are full, search radius (uv),
                        // penumbra growth (uv per world unit of blocker distance)
uniform vec4 u_sun;     // xyz: direction the sunlight travels, view space
uniform mat4 u_light;   // view space -> the light's clip space
#ifdef FOLIAGE
// ShadowFoliage: u_map / u_map_depth have the cut-outs too, this is the opaque casters' map alone (the
// picture is darkened by that already): only what the cut-outs add darkens.
layout(binding = 3) uniform sampler2DShadow u_map_opaque;
#endif
out vec4 o_colour;

const vec2 kDisc[16] = vec2[](
    vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725), vec2(-0.09418410, -0.92938870),
    vec2(0.34495938, 0.29387760), vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379), vec2(0.44323325, -0.97511554),
    vec2(0.53742981, -0.47373420), vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590), vec2(0.19984126, 0.78641367),
    vec2(0.14383161, -0.14100790));

vec3 view_pos(vec2 px, float z) {
    vec2 ndc = vec2((px.x - u_vp.x) / u_vp.z * 2.0 - 1.0, 1.0 - (px.y - u_vp.y) / u_vp.w * 2.0);
    return vec3((ndc - u_proj.zw) * u_proj.xy * z, z);
}

void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    float raw = texelFetch(u_depth, p, 0).r;
    // A name label / health plate nothing was drawn over: its depth is no surface of the world.
    if (u_overlay_on != 0) {
        float o = texelFetch(u_overlay, p, 0).r;
        if (o != texelFetch(u_overlay_before, p, 0).r && o == raw) discard;
    }
    float d = (raw - u_lin.z) / max(u_lin.w - u_lin.z, 1e-6);
    bool sky = d >= 0.999999;
    float z = sky ? 1.0 : u_lin.y / (d - u_lin.x);
    if (sky || !(z > 0.0)) discard;
    vec3 P = view_pos(gl_FragCoord.xy, z);
    float fade = 1.0 - smoothstep(u_params.z, u_params.w, length(P));
    if (fade <= 0.0) discard;

    // No surface normal is used for the shadow map lookup: rebuilt from the depth buffer it is the flat
    // normal of each triangle (not the models' smooth vertex normals), and anything derived from it shows
    // the polygons. The receiver is moved towards the sun instead, which keeps lit surfaces free of acne.
    vec4 lc = u_light * vec4(P - u_sun.xyz * u_bias.x, 1.0);
    vec2 uv = vec2(0.5 + 0.5 * lc.x, 0.5 - 0.5 * lc.y);
    if (uv.x <= 0.0 || uv.y <= 0.0 || uv.x >= 1.0 || uv.y >= 1.0 || lc.z >= 1.0) discard;
    vec2 edge = min(uv, 1.0 - uv);  // fading out before the map ends, not cut off
    fade *= smoothstep(0.0, 0.03, min(edge.x, edge.y));
    float ref = lc.z - u_bias.y;

    float a = 6.2831853 * fract(52.9829189 * fract(dot(gl_FragCoord.xy, vec2(0.06711056, 0.00583715))));
    mat2 rot = mat2(cos(a), sin(a), -sin(a), cos(a));

    // Shadow map darkness, 0 (lit) .. 1: of all the casters, and (FOLIAGE) of the opaque ones.
    float sm = 0.0, sm_opaque = 0.0;
    // 1. Blocker search: how far the occluders are from this surface (towards the sun).
    float blockers = 0.0, count = 0.0;
    for (int i = 0; i < 16; ++i) {
        float dm = texture(u_map_depth, uv + rot * kDisc[i] * u_self.z).r;
        if (dm < ref) { blockers += dm; count += 1.0; }
    }
    float dist = count > 0.0 ? (ref - blockers / count) * u_bias.z : 0.0;  // world units
    // An occluder only a few units away is the surface's own model (an arm over the chest, the far side
    // of a low-poly body): the original game's lighting shades that already, and a shadow map draws it
    // with the polygons' outline - such self-shadowing fades out.
    float self_k = count > 0.0 ? smoothstep(u_self.x, u_self.y, dist) : 0.0;
    if (self_k > 0.0) {
        // 2. Filter, wider the further the occluder (contact-hardening: sharp at the base, soft further away;
        // a cliff's shadow far down its slope blends out over a wide band instead of ending in a line).
        // A golden-angle spiral (rotated per pixel) covers the wide disc evenly with 32 taps.
        float radius = min(u_params.y + dist * u_self.w, u_bias.w);
        float lit = 0.0, lit_opaque = 0.0;
        for (int i = 0; i < 32; ++i) {
            float r = sqrt((float(i) + 0.5) / 32.0);
            float t = float(i) * 2.3999632 + a;
            vec3 c = vec3(uv + vec2(cos(t), sin(t)) * (r * radius), ref);
            lit += texture(u_map, c);
#ifdef FOLIAGE
            lit_opaque += texture(u_map_opaque, c);
#endif
        }
        sm = (1.0 - lit / 32.0) * self_k;
        sm_opaque = (1.0 - lit_opaque / 32.0) * self_k;
    }

    float shadow = sm * fade;
#ifndef FOLIAGE
    if (u_debug == 1) { o_colour = vec4(1.0 - sm, 1.0 - sm, 1.0, 1.0); return; }
#else
    if (u_debug != 0) discard;
#endif
    if (shadow <= 0.0) discard;
    float f = 1.0 - u_params.x * shadow;
#ifdef FOLIAGE
    float before = 1.0 - u_params.x * sm_opaque * fade;
    f = clamp(f / max(before, 1e-3), 0.0, 1.0);
    if (f >= 0.998) discard;
#endif
    o_colour = vec4(f, f, f, 1.0);
}
)";

// --- 4x4 maths in double, Direct3D layout (row-major, row vectors: v' = v * M) ------------------
struct M4 {
    double m[16];
};
M4 load(const float* f) {
    M4 r;
    for (int i = 0; i < 16; ++i) r.m[i] = f[i];
    return r;
}
M4 mul(const M4& a, const M4& b) {
    M4 r;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            double s = 0.0;
            for (int k = 0; k < 4; ++k) s += a.m[i * 4 + k] * b.m[k * 4 + j];
            r.m[i * 4 + j] = s;
        }
    return r;
}
bool inverse(const M4& a, M4& out) {  // cofactors
    const double* m = a.m;
    double inv[16];
    inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
    inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
    inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
    inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
    inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
    inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
    inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
    inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
    inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
    const double det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    if (!(std::fabs(det) > 1e-30)) return false;
    for (int i = 0; i < 16; ++i) out.m[i] = inv[i] / det;
    return true;
}
void store(const M4& a, float* f) {
    for (int i = 0; i < 16; ++i) f[i] = static_cast<float>(a.m[i]);
}

void set4(GLuint program, GLint location, float x, float y, float z, float w) {
    const float v[4] = {x, y, z, w};
    glProgramUniform4fv(program, location, 1, v);
}
void set4(GLuint program, const char* name, float x, float y, float z, float w) {
    set4(program, glGetUniformLocation(program, name), x, y, z, w);
}

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
GLuint make_depth_fbo(GLuint texture, GLenum attachment) {
    GLuint f = 0;
    glCreateFramebuffers(1, &f);
    glNamedFramebufferTexture(f, attachment, texture, 0);
    glNamedFramebufferDrawBuffer(f, GL_NONE_);
    glNamedFramebufferReadBuffer(f, GL_NONE_);
    return f;
}

}  // namespace

ShadowParams& shadow_params() {
    static ShadowParams p = [] {
        const auto& s = hook::settings();
        ShadowParams r;
        r.mode = std::clamp(s.shadows, 0, 2);
        r.distance = static_cast<float>(std::max(500, s.shadow_distance));
        r.map_size = std::clamp(s.shadow_map_size, 512, 8192);
        r.strength = static_cast<float>(std::clamp(s.shadow_strength, 0, 100)) / 100.0f;
        r.softness = static_cast<float>(std::clamp(s.shadow_softness, 0, 32));
        r.foliage = s.shadow_foliage != 0;
        r.baked = static_cast<float>(std::clamp(s.baked_shadows, 0, 100)) / 100.0f;
        r.caster_margin = static_cast<float>(std::clamp(s.shadow_caster_margin, 0, 20000));
        return r;
    }();
    return p;
}

namespace {
OverlayHook g_overlay = nullptr;
}  // namespace
NearPlaneOverride& near_plane_override() {
    static NearPlaneOverride o;
    return o;
}
bool invert_matrix(const float* m, float* out) {
    M4 r;
    if (!inverse(load(m), r)) return false;
    store(r, out);
    return true;
}
void set_overlay_hook(OverlayHook hook) { g_overlay = hook; }
void overlay_begin() {
    if (g_overlay) g_overlay(true);
}
void overlay_end() {
    if (g_overlay) g_overlay(false);
}

void set_shadow_hooks(ShadowBeginHook begin, ShadowDoneHook done) {
    g_begin = begin;
    g_done = done;
}
void shadow_scene_begin(const float* sun) {
    if (g_begin) g_begin(sun);
}
void shadow_opaque_done(const float* projection) {
    if (g_done && projection) g_done(projection);
}


// --- ShadowPass -----------------------------------------------------------------------------
bool ShadowPass::ensure(int width, int height, int size) {
    if (failed_) return false;
    Renderer* r = current();
    if (!prog_apply_) {
        if (!r) return false;
        prog_apply_ = r->compile_program(kFullscreenVs, kApplyFs, "shadow map: apply");
        std::string foliage = kApplyFs;
        foliage.insert(foliage.find('\n') + 1, "#define FOLIAGE\n");
        prog_foliage_ = prog_apply_ ? r->compile_program(kFullscreenVs, foliage, "shadow map: foliage") : 0;
        if (!prog_apply_ || !prog_foliage_) {
            hook::log_line("[Graphics] Shadows: shader compilation failed - shadow maps off");
            failed_ = true;
            return false;
        }
        glCreateVertexArrays(1, &vao_);
        glCreateSamplers(1, &compare_sampler_);
        glSamplerParameteri(compare_sampler_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(compare_sampler_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(compare_sampler_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(compare_sampler_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(compare_sampler_, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glSamplerParameteri(compare_sampler_, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }
    if (map_size_ != size) {
        if (map_fbo_) glDeleteFramebuffers(1, &map_fbo_);
        if (map_tex_) glDeleteTextures(1, &map_tex_);
        if (opaque_tex_) glDeleteTextures(1, &opaque_tex_);
        opaque_tex_ = 0;
        map_tex_ = make_texture(GL_DEPTH_COMPONENT32F, size, size, GL_LINEAR);
        map_fbo_ = make_depth_fbo(map_tex_, GL_DEPTH_ATTACHMENT);
        map_size_ = size;
        if (glCheckNamedFramebufferStatus(map_fbo_, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            hook::log_line("[Graphics] Shadows: shadow map framebuffer incomplete - shadow maps off");
            failed_ = true;
            return false;
        }
        hook::log_line("[Graphics] Shadows: shadow map %dx%d", size, size);
    }
    if (w_ != width || h_ != height) {
        if (depth_fbo_) glDeleteFramebuffers(1, &depth_fbo_);
        if (depth_tex_) glDeleteTextures(1, &depth_tex_);
        depth_tex_ = make_texture(GL_DEPTH24_STENCIL8, width, height, GL_NEAREST);
        depth_fbo_ = make_depth_fbo(depth_tex_, GL_DEPTH_STENCIL_ATTACHMENT);
        w_ = width;
        h_ = height;
    }
    return true;
}

void ShadowPass::shutdown() {
    const GLuint fbos[] = {map_fbo_, depth_fbo_};
    const GLuint textures[] = {map_tex_, depth_tex_, opaque_tex_};
    for (GLuint f : fbos)
        if (f) glDeleteFramebuffers(1, &f);
    for (GLuint t : textures)
        if (t) glDeleteTextures(1, &t);
    if (prog_apply_) glDeleteProgram(prog_apply_);
    if (prog_foliage_) glDeleteProgram(prog_foliage_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (compare_sampler_) glDeleteSamplers(1, &compare_sampler_);
    map_fbo_ = depth_fbo_ = map_tex_ = depth_tex_ = opaque_tex_ = prog_apply_ = prog_foliage_ = vao_ = compare_sampler_ = 0;
    map_size_ = w_ = h_ = 0;
    last_valid_ = false;
}

// The shadow map bound for depth-only drawing (cleared by the caller when it starts over).
void ShadowPass::begin_map() {
    glBindFramebuffer(GL_FRAMEBUFFER, map_fbo_);
    glViewport(0, 0, map_size_, map_size_);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(1);
    glDepthRangef(0.0f, 1.0f);
    glColorMask(0, 0, 0, 0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.5f, 4.0f);
}
void ShadowPass::end_map() {
    glDisable(GL_POLYGON_OFFSET_FILL);
    glColorMask(1, 1, 1, 1);
}

// The camera's draws: their clip positions -> view space (`projection`) -> the light's clip space.
unsigned ShadowPass::replay(Renderer& r, const std::vector<ShadowDraw>& draws, const float* projection) {
    r.invalidate_state();  // program / vertex array / texture caches: set again below
    ++frame_;              // each shadow variant takes this light's uniforms on its first draw
    const float shadow_proj[4] = {projection[0], projection[5], projection[8], projection[9]};
    std::uint32_t ffp = ~0u, vs = ~0u, per_draw = ~0u;
    unsigned drawn = 0;
    for (const ShadowDraw& d : draws) {
        Renderer::ShadowVariant& v = r.shadow_variant(d.program);
        if (!v.program) continue;
        r.use_program(v.program);
        if (v.frame != frame_) {
            glProgramUniform4fv(v.program, v.proj, 1, shadow_proj);
            glProgramUniformMatrix4fv(v.program, v.light, 1, GL_FALSE_, view_to_light_);
            v.frame = frame_;
        }
        if (d.ffp_offset != ffp) r.bind_uniform_range(0, ffp = d.ffp_offset);
        if (d.vs_offset != vs && d.vs_offset != ~0u) r.bind_uniform_range(1, vs = d.vs_offset);
        if (d.draw_offset != per_draw) r.bind_uniform_range(3, per_draw = d.draw_offset);
        r.bind_geometry(d.vao, d.vertex_buffer, d.stride, d.index_buffer, d.vertex_offset);
        if (d.texture) r.bind_texture(0, d.texture, d.sampler);
        if (d.indexed) r.draw_elements(d.mode, d.count, d.wide, d.index_offset, d.base_vertex);
        else r.draw_arrays(d.mode, d.first, d.count);
        ++drawn;
    }
    drawn_total_ += drawn;
    return drawn;
}

// Onto the picture: the scene's depth (one sample per pixel), then the soft lookup.
void ShadowPass::apply(GLuint program, const Input& in) {
    const float* p = in.projection;
    const int size = map_size_;
    glBlitNamedFramebuffer(in.scene_fbo, depth_fbo_, 0, 0, w_, h_, 0, 0, w_, h_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, in.scene_fbo);
    glViewport(0, 0, w_, h_);
    glEnable(GL_SCISSOR_TEST);
    glScissor(in.vx, in.vy, in.vw, in.vh);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
    glColorMask(1, 1, 1, 0);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);  // colour *= the shadow factor
    glUseProgram(program);
    set4(program, "u_lin", p[10], p[14], in.depth_min, in.depth_max);
    set4(program, "u_proj", 1.0f / p[0], 1.0f / p[5], p[8], p[9]);
    set4(program, "u_vp", static_cast<float>(in.vx), static_cast<float>(in.vy), static_cast<float>(in.vw), static_cast<float>(in.vh));
    set4(program, "u_params", strength_, softness_ / static_cast<float>(size), static_cast<float>(distance_ * 0.8),
         static_cast<float>(distance_));
    // Self-shadowing below kSelfNear world units of occluder distance is dropped, full from kSelfFar (Joan is
    // ~79 tall); the blocker search covers at least 16 texels; the penumbra widens 0.1 texels per unit of
    // occluder distance, up to 40 texels (a character's shadow stays crisp, a cliff's blends out).
    constexpr float kSelfNear = 6.0f, kSelfFar = 24.0f;
    const float texel_uv = 1.0f / static_cast<float>(size);
    set4(program, "u_bias", static_cast<float>(texel_ * 1.5), static_cast<float>(1.0 / (2.0 * depth_half_)),
         static_cast<float>(2.0 * depth_half_), std::max(softness_, 40.0f) * texel_uv);
    set4(program, "u_self", kSelfNear, kSelfFar, std::max(softness_, 16.0f) * texel_uv, 0.1f * texel_uv);
    set4(program, "u_sun", sun_view_[0], sun_view_[1], sun_view_[2], 0.0f);
    glProgramUniformMatrix4fv(program, glGetUniformLocation(program, "u_light"), 1, GL_FALSE_, view_to_light_);
    glBindVertexArray(vao_);
    glBindTextureUnit(0, depth_tex_);
    glBindSampler(0, 0);
    glBindTextureUnit(1, map_tex_);
    glBindSampler(1, compare_sampler_);
    glBindTextureUnit(2, map_tex_);  // raw depths: the texture's own (linear, no compare) parameters
    const bool overlay = in.overlay_depth && in.overlay_before;
    glProgramUniform1i(program, glGetUniformLocation(program, "u_overlay_on"), overlay ? 1 : 0);
    glProgramUniform1i(program, glGetUniformLocation(program, "u_debug"), shadow_params().debug);
    if (overlay) {
        glBindTextureUnit(4, in.overlay_depth);
        glBindSampler(4, 0);
        glBindTextureUnit(5, in.overlay_before);
        glBindSampler(5, 0);
    }
    glBindSampler(2, 0);
    if (program == prog_foliage_) {
        glBindTextureUnit(3, opaque_tex_);
        glBindSampler(3, compare_sampler_);
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glColorMask(1, 1, 1, 1);
    glDepthMask(1);
    glBindSampler(1, 0);
    glBindSampler(3, 0);
}

bool ShadowPass::run(Renderer& r, const Input& in, const std::vector<ShadowDraw>& draws) {
    last_valid_ = false;
    const ShadowParams& s = shadow_params();
    const int size = std::clamp(s.map_size, 512, 8192);
    const double distance = std::max(500.0f, s.distance);
    const float* p = in.projection;
    // Direct3D perspective only (as the occlusion's decode): x' = x P11 + z P31, y' = y P22 + z P32, w' = z.
    if (!(p[11] == 1.0f && p[0] != 0.0f && p[5] != 0.0f && p[14] < 0.0f && p[10] > 1.0f)) return false;
    if (draws.empty() || !ensure(in.width, in.height, size)) return false;

    // Camera: view -> world.
    M4 view_inv;
    if (!inverse(load(in.view), view_inv)) return false;
    const double* vi = view_inv.m;
    const double cam[3] = {vi[12], vi[13], vi[14]};

    // Sun basis (Direct3D LookAtLH along the sunlight): z = sun, x = up x z, y = z x x; world z is up.
    double sz[3] = {in.sun[0], in.sun[1], in.sun[2]};
    const double sl = std::sqrt(sz[0] * sz[0] + sz[1] * sz[1] + sz[2] * sz[2]);
    if (!(sl > 1e-6)) return false;
    for (double& c : sz) c /= sl;
    const bool steep = std::fabs(sz[2]) >= 0.95;  // a sun almost overhead: another up vector
    const double up[3] = {0.0, steep ? 1.0 : 0.0, steep ? 0.0 : 1.0};
    auto cross = [](const double* a, const double* b, double* o) {
        o[0] = a[1] * b[2] - a[2] * b[1];
        o[1] = a[2] * b[0] - a[0] * b[2];
        o[2] = a[0] * b[1] - a[1] * b[0];
    };
    double sx[3], sy[3];
    cross(up, sz, sx);
    const double xl = std::sqrt(sx[0] * sx[0] + sx[1] * sx[1] + sx[2] * sx[2]);
    for (double& c : sx) c /= xl;
    cross(sz, sx, sy);

    // Covered area: a disc of `distance` around the camera (the shadows reach as far whichever way it turns -
    // a disc ahead of it moved its edge over distant ground as it turned, shadows coming and going); the centre
    // snapped to whole texels of the light's view so the shadows do not shimmer as the camera moves.
    const double radius = distance;
    const double centre[3] = {cam[0], cam[1], cam[2]};
    const double texel = 2.0 * radius / size;
    auto dot = [](const double* a, const double* b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; };
    const double cx = std::floor(dot(sx, centre) / texel) * texel, cy = std::floor(dot(sy, centre) / texel) * texel;
    const double cz = dot(sz, centre);
    M4 light_view = {{sx[0], sy[0], sz[0], 0.0, sx[1], sy[1], sz[1], 0.0, sx[2], sy[2], sz[2], 0.0, -cx, -cy, -cz, 1.0}};
    M4 ortho = {{1.0 / radius, 0, 0, 0, 0, 1.0 / radius, 0, 0, 0, 0, 0.5 / depth_half_, 0, 0, 0, 0.5, 1.0}};
    store(mul(view_inv, mul(light_view, ortho)), view_to_light_);
    for (int i = 0; i < 3; ++i)
        sun_view_[i] = static_cast<float>(sz[0] * in.view[i] + sz[1] * in.view[4 + i] + sz[2] * in.view[8 + i]);
    texel_ = texel;
    distance_ = distance;
    strength_ = std::clamp(s.strength, 0.0f, 1.0f);
    softness_ = std::clamp(s.softness, 0.0f, 32.0f);

    // 1. The shadow map: the recorded draws, depth only.
    begin_map();
    const float one = 1.0f;
    glClearNamedFramebufferfv(map_fbo_, GL_DEPTH_BUFFER, 0, &one);
    const unsigned drawn = replay(r, draws, p);
    end_map();

    // 2. Onto the picture.
    apply(prog_apply_, in);
    last_valid_ = true;
    if (logged_ < 3) {
        ++logged_;
        hook::log_line("[Graphics] Shadows: %u of %zu recorded draws into the %dx%d shadow map (distance %.0f, texel %.2f units)", drawn,
                       draws.size(), size, size, distance, texel);
    }
    return true;
}

void ShadowPass::run_foliage(Renderer& r, const Input& in, const std::vector<ShadowDraw>& draws) {
    if (!last_valid_ || draws.empty() || failed_ || w_ != in.width || h_ != in.height) return;
    last_valid_ = false;
    // The opaque casters' map kept, then the cut-outs added to it.
    if (!opaque_tex_) opaque_tex_ = make_texture(GL_DEPTH_COMPONENT32F, map_size_, map_size_, GL_LINEAR);
    glCopyImageSubData(map_tex_, GL_TEXTURE_2D, 0, 0, 0, 0, opaque_tex_, GL_TEXTURE_2D, 0, 0, 0, 0, map_size_, map_size_, 1);
    begin_map();
    const unsigned drawn = replay(r, draws, in.projection);
    end_map();
    apply(prog_foliage_, in);
    if (foliage_logged_ < 3) {
        ++foliage_logged_;
        hook::log_line("[Graphics] Shadows: %u of %zu foliage draws added to the shadow map", drawn, draws.size());
    }
}

}  // namespace joan::render
