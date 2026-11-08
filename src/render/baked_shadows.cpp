// [Graphics] BakedShadows - see baked_shadows.hpp.
#include "baked_shadows.hpp"

#include <algorithm>
#include <cmath>

#include "../gl/shaders.hpp"
#include "../registry.hpp"
#include "renderer.hpp"
#include "shadow_map.hpp"

namespace joan::render {

using namespace gl;

namespace {

constexpr int kMapSize = 4096;   // level map texels per side (8 x ShadowDistance: ~8 world units each)
constexpr int kSmallSize = 1024;  // the lit level's texels per side (kMapSize / 4)
constexpr float kRadius = 160.0f;  // closing / blur radius, world units (a house's shadow is narrower than twice this)

const char* const kFullscreenVs = R"(#version 450 core
void main() {
    vec2 p = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}
)";

// The lit level at kSmallSize. Alpha marks texels with ground (the rest - water, the map's parts the camera
// drew nothing of - take no part).
//   u_mode 0: maximum over the level map's 4x4 texels under this one (unit 1);
//   u_mode 1: maximum, 2: minimum (this texel keeps no ground if it had none), 3: mean (weighted by alpha;
//             alpha = the part with ground) along u_dir over +-u_radius texels of u_src (unit 0).
const char* const kFilterFs = R"(#version 450 core
layout(binding = 0) uniform sampler2D u_src;
layout(binding = 1) uniform sampler2D u_map;
uniform ivec2 u_dir;
uniform int u_radius;
uniform int u_mode;
out vec4 o_colour;
void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    if (u_mode == 0) {
        vec4 acc = vec4(0.0);
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; ++x) {
                vec4 c = texelFetch(u_map, p * 4 + ivec2(x, y), 0);
                if (c.a > 0.5) acc = vec4(max(acc.rgb, c.rgb), 1.0);
            }
        o_colour = acc;
        return;
    }
    ivec2 last = textureSize(u_src, 0) - 1;
    vec4 centre = texelFetch(u_src, p, 0);
    if (u_mode == 3) {
        vec3 sum = vec3(0.0);
        float weight = 0.0;
        for (int i = -u_radius; i <= u_radius; ++i) {
            vec4 c = texelFetch(u_src, clamp(p + u_dir * i, ivec2(0), last), 0);
            sum += c.rgb * c.a;
            weight += c.a;
        }
        o_colour = weight > 0.0 ? vec4(sum / weight, weight / float(2 * u_radius + 1)) : vec4(0.0);
        return;
    }
    if (u_mode == 2 && centre.a < 0.5) {
        o_colour = vec4(0.0);
        return;
    }
    vec3 acc = u_mode == 1 ? vec3(0.0) : vec3(1.0);
    bool any = false;
    for (int i = -u_radius; i <= u_radius; ++i) {
        vec4 c = texelFetch(u_src, clamp(p + u_dir * i, ivec2(0), last), 0);
        if (c.a < 0.5) continue;
        acc = u_mode == 1 ? max(acc, c.rgb) : min(acc, c.rgb);
        any = true;
    }
    o_colour = any ? vec4(acc, 1.0) : vec4(0.0);
}
)";

GLuint make_texture(GLenum format, int size, GLenum filter) {
    GLuint t = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &t);
    glTextureStorage2D(t, 1, format, size, size);
    glTextureParameteri(t, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
    glTextureParameteri(t, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
    glTextureParameteri(t, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(t, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return t;
}

// a * b, Direct3D layout (row vectors).
void mul(const float* a, const float* b, float* out) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k) s += a[i * 4 + k] * b[k * 4 + j];
            out[i * 4 + j] = s;
        }
}

}  // namespace

bool BakedShadows::ensure() {
    if (failed_) return false;
    if (prog_filter_) return true;
    Renderer* r = current();
    if (!r) return false;
    prog_filter_ = r->compile_program(kFullscreenVs, kFilterFs, "baked shadows: level map filter");
    if (!prog_filter_) {
        hook::log_line("[Graphics] BakedShadows: shader compilation failed - lightmaps unchanged");
        failed_ = true;
        return false;
    }
    glCreateVertexArrays(1, &vao_);
    glCreateBuffers(1, &ubo_);
    const float zero[8] = {};
    glNamedBufferData(ubo_, sizeof zero, zero, GL_DYNAMIC_DRAW);
    map_tex_ = make_texture(GL_RGBA8, kMapSize, GL_NEAREST);
    map_depth_ = make_texture(GL_DEPTH_COMPONENT24, kMapSize, GL_NEAREST);
    glCreateFramebuffers(1, &map_fbo_);
    glNamedFramebufferTexture(map_fbo_, GL_COLOR_ATTACHMENT0, map_tex_, 0);
    glNamedFramebufferTexture(map_fbo_, GL_DEPTH_ATTACHMENT, map_depth_, 0);
    glNamedFramebufferDrawBuffer(map_fbo_, GL_COLOR_ATTACHMENT0);
    for (GLuint& t : small_) t = make_texture(GL_RGBA8, kSmallSize, GL_LINEAR);
    glCreateFramebuffers(1, &fbo_);
    glNamedFramebufferDrawBuffer(fbo_, GL_COLOR_ATTACHMENT0);
    if (glCheckNamedFramebufferStatus(map_fbo_, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        hook::log_line("[Graphics] BakedShadows: level map framebuffer incomplete - lightmaps unchanged");
        failed_ = true;
        return false;
    }
    return true;
}

void BakedShadows::write_block(const float* lift, const float* params) {
    float block[8];
    std::copy(lift, lift + 4, block);
    std::copy(params, params + 4, block + 4);
    glNamedBufferSubData(ubo_, 0, sizeof block, block);
}

GLuint BakedShadows::texture() {
    if (!ensure()) return 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo_);
    return small_[2];
}

void BakedShadows::invalidate() {
    if (!valid_ || !ubo_) return;
    valid_ = false;
    const float lift[4] = {}, params[4] = {};
    write_block(lift, params);
}

void BakedShadows::build(Renderer& r, const std::vector<ShadowDraw>& draws, const float* view, const float* projection,
                         float distance, float keep) {
    if (!ensure()) return;
    float view_inv[16];
    if (!invert_matrix(view, view_inv)) return;

    // The map: 4 x `distance` either side of the camera - all the ground the camera draws, so baked shadows
    // do not come back in the distance - its corner snapped to whole lit-level texels (the filter's
    // result does not shimmer as the camera moves).
    const float span = 8.0f * std::max(500.0f, distance);
    const float step = span / static_cast<float>(kSmallSize);
    const float cx = std::floor(view_inv[12] / step) * step, cy = std::floor(view_inv[13] / step) * step;
    const float h = span * 0.5f;
    // World -> the map's clip space: x, y from the corner; depth from the height (world z is up; the highest
    // ground in front).
    constexpr float kDepthHalf = 30000.0f;
    const float to_map[16] = {1.0f / h,  0.0f,      0.0f, 0.0f, 0.0f, 1.0f / h, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f / kDepthHalf, 0.0f,
                              -cx / h, -cy / h, 0.5f, 1.0f};
    float view_to_map[16];
    mul(view_inv, to_map, view_to_map);
    const float proj[4] = {projection[0], projection[5], projection[8], projection[9]};

    // 1. The level map: the ground's lightmap colours seen from above.
    r.invalidate_state();
    glBindFramebuffer(GL_FRAMEBUFFER, map_fbo_);
    glViewport(0, 0, kMapSize, kMapSize);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMask(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(1);
    glDepthRangef(0.0f, 1.0f);
    const float clear[4] = {0.0f, 0.0f, 0.0f, 0.0f}, one = 1.0f;
    glClearNamedFramebufferfv(map_fbo_, GL_COLOR, 0, clear);
    glClearNamedFramebufferfv(map_fbo_, GL_DEPTH_BUFFER, 0, &one);
    ++frame_;
    std::uint32_t ffp = ~0u, vs = ~0u, per_draw = ~0u;
    unsigned drawn = 0;
    for (const ShadowDraw& d : draws) {
        if (!d.lightmap) continue;
        Renderer::ShadowVariant& v = r.lift_variant(d.program, d.lightmap_stage);
        if (!v.program) continue;
        r.use_program(v.program);
        if (v.frame != frame_) {
            glProgramUniform4fv(v.program, v.proj, 1, proj);
            glProgramUniformMatrix4fv(v.program, v.light, 1, GL_FALSE_, view_to_map);
            v.frame = frame_;
        }
        if (d.ffp_offset != ffp) r.bind_uniform_range(0, ffp = d.ffp_offset);
        if (d.vs_offset != vs && d.vs_offset != ~0u) r.bind_uniform_range(1, vs = d.vs_offset);
        if (d.draw_offset != per_draw) r.bind_uniform_range(3, per_draw = d.draw_offset);
        r.bind_geometry(d.vao, d.vertex_buffer, d.stride, d.index_buffer, d.vertex_offset);
        r.bind_texture(0, d.lightmap, d.lightmap_sampler);
        if (d.indexed) r.draw_elements(d.mode, d.count, d.wide, d.index_offset, d.base_vertex);
        else r.draw_arrays(d.mode, d.first, d.count);
        ++drawn;
    }
    r.invalidate_state();
    if (drawn == 0) {
        invalidate();
        return;
    }

    // 2. Its lit level: down to kSmallSize, closed (maximum, then minimum), blurred.
    const int radius = std::clamp(static_cast<int>(std::lround(kRadius / step)), 1, 32);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
    glUseProgram(prog_filter_);
    glBindVertexArray(vao_);
    glBindSampler(0, 0);
    glBindSampler(1, 0);
    glBindTextureUnit(1, map_tex_);
    glProgramUniform1i(prog_filter_, glGetUniformLocation(prog_filter_, "u_radius"), radius);
    const GLint loc_dir = glGetUniformLocation(prog_filter_, "u_dir"), loc_mode = glGetUniformLocation(prog_filter_, "u_mode");
    struct Pass {
        GLuint from, to;
        int dx, dy, mode;
    };
    const Pass passes[] = {{0, small_[0], 0, 0, 0},         {small_[0], small_[1], 1, 0, 1}, {small_[1], small_[0], 0, 1, 1},
                           {small_[0], small_[1], 1, 0, 2}, {small_[1], small_[0], 0, 1, 2}, {small_[0], small_[1], 1, 0, 3},
                           {small_[1], small_[2], 0, 1, 3}};
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, kSmallSize, kSmallSize);
    for (const Pass& p : passes) {
        glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0, p.to, 0);
        glProgramUniform2i(prog_filter_, loc_dir, p.dx, p.dy);
        glProgramUniform1i(prog_filter_, loc_mode, p.mode);
        glBindTextureUnit(0, p.from);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0, 0, 0);
    glDepthMask(1);

    const float lift[4] = {cx - h, cy - h, 1.0f / span, 1.0f / span};
    const float params[4] = {1.0f - std::clamp(keep, 0.0f, 1.0f), 1.0f, 0.0f, 0.0f};
    write_block(lift, params);
    valid_ = true;
    if (logged_ < 3) {
        ++logged_;
        hook::log_line("[Graphics] BakedShadows: %u ground draws into the level map (%.0f units, closing radius %d texels of %.1f units)",
                       drawn, static_cast<double>(span), radius, static_cast<double>(step));
    }
}

void BakedShadows::shutdown() {
    const GLuint fbos[] = {map_fbo_, fbo_};
    for (GLuint f : fbos)
        if (f) glDeleteFramebuffers(1, &f);
    const GLuint textures[] = {map_tex_, map_depth_, small_[0], small_[1], small_[2]};
    for (GLuint t : textures)
        if (t) glDeleteTextures(1, &t);
    if (prog_filter_) glDeleteProgram(prog_filter_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (ubo_) glDeleteBuffers(1, &ubo_);
    map_fbo_ = fbo_ = map_tex_ = map_depth_ = prog_filter_ = vao_ = ubo_ = 0;
    small_[0] = small_[1] = small_[2] = 0;
    valid_ = false;
}

}  // namespace joan::render
