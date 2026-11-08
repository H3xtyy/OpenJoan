// Native OpenGL 4.6 execution core (see renderer.hpp). Moved out of gl/device.cpp
// unchanged: same GL calls in the same order, same cache keys, same log text.
#include "renderer.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <utility>

#include "../d3d8/d3d8_types.hpp"  // FVF bits (vertex_array_fvf)
#include "../runtime/hitch.hpp"
#include "../registry.hpp"

namespace joan::render {

using namespace joan::gl;

namespace {

constexpr GLenum GL_MIRROR_CLAMP_TO_EDGE = 0x8743;

Renderer* g_current = nullptr;

}  // namespace

Renderer* current() { return g_current; }

DrawOverrides& overrides() {
    static DrawOverrides o;
    return o;
}
void set_current(Renderer* renderer) { g_current = renderer; }

// ---------------------------------------------------------------------------
// lifetime
// ---------------------------------------------------------------------------
void Renderer::init() {
    glCreateBuffers(1, &ubo_ffp_);
    glNamedBufferData(ubo_ffp_, sizeof(gld::FfpUniforms), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_ffp_);
    glCreateBuffers(1, &ubo_vs_);
    glNamedBufferData(ubo_vs_, sizeof(float[gld::kVsConstants][4]), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_vs_);
    glCreateBuffers(1, &ubo_draw_);
    glNamedBufferData(ubo_draw_, sizeof(gld::FfpDrawUniforms), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ubo_draw_);  // block FfpDraw (storage), replaced by the ring below

    // Uniform ring (see renderer.hpp); without it the plain buffers above are updated in place.
    GLint align = 256;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &align);
    ring_align_ = static_cast<std::uint32_t>(std::max(align, 16));
    const GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    glCreateBuffers(1, &ring_);
    glNamedBufferStorage(ring_, kRingSize, nullptr, flags);
    ring_ptr_ = static_cast<std::uint8_t*>(glMapNamedBufferRange(ring_, 0, kRingSize, flags));
    if (!ring_ptr_) {
        hook::log_line("OpenGL device: no persistently mapped uniform ring (0x%x) - uniform blocks updated in place", glGetError());
        glDeleteBuffers(1, &ring_);
        ring_ = 0;
    }
    if (ring_) {
        GLint max_block = 0;
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_block);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, ring_, 0,
                          static_cast<gl::GLsizeiptr>(std::min<std::uint32_t>(kRingSize, static_cast<std::uint32_t>(std::max(max_block, 0)))));
        if (static_cast<std::uint32_t>(std::max(max_block, 0)) < kRingSize)
            hook::log_line("OpenGL device: storage blocks limited to %d bytes - per-draw records past it are not readable", max_block);
    }
    ring_cursor_ = 0;
    bindless_ = gl::has_bindless_textures();
    hook::log_line("OpenGL device: bindless textures %s", bindless_ ? "available (grass multi-draw across textures)" : "not available");
    invalidate_state();
}

void Renderer::shutdown() {
    for (auto& [k, p] : programs_)
        if (p) glDeleteProgram(p);
    for (auto& [k, v] : shadow_variants_)
        if (v.program) glDeleteProgram(v.program);
    shadow_variants_.clear();
    for (auto& [k, v] : lift_variants_)
        if (v.program) glDeleteProgram(v.program);
    lift_variants_.clear();
    for (auto& [k, p] : fs_variants_)
        if (p) glDeleteProgram(p);
    fs_variants_.clear();
    for (auto& [k, p] : bindless_programs_)
        if (p) glDeleteProgram(p);
    bindless_programs_.clear();
    for (auto& [k, h] : texture_handles_)
        if (h) glMakeTextureHandleNonResidentARB(h);
    texture_handles_.clear();
    sources_.clear();
    for (std::uint32_t& o : bound_offset_) o = ~0u;
    for (auto& [k, v] : vertex_arrays_) glDeleteVertexArrays(1, &v);
    for (auto& [k, smp] : samplers_) glDeleteSamplers(1, &smp);
    glDeleteBuffers(1, &ubo_ffp_);
    glDeleteBuffers(1, &ubo_vs_);
    glDeleteBuffers(1, &ubo_draw_);
    for (GLsync& f : ring_fences_)
        if (f) glDeleteSync(std::exchange(f, nullptr));
    if (ring_) glDeleteBuffers(1, &ring_);  // unmaps it
    ring_ = 0;
    ring_ptr_ = nullptr;
    programs_.clear();
    vertex_arrays_.clear();
    samplers_.clear();
    vao_buffers_.clear();
    last_vao_buffers_ = nullptr;
    shadow_ffp_.clear();
    shadow_vs_.clear();
    shadow_draw_.clear();
    last_program_key_valid_ = last_vao_valid_ = false;
    ubo_ffp_ = ubo_vs_ = ubo_draw_ = 0;
    invalidate_state();
}

// ---------------------------------------------------------------------------
// shadow maps
// ---------------------------------------------------------------------------
Renderer::ShadowVariant& Renderer::shadow_variant(GLuint program) {
    auto [it, added] = shadow_variants_.try_emplace(program);
    ShadowVariant& v = it->second;
    if (!added) return v;
    const auto src = sources_.find(program);
    if (src == sources_.end()) return v;
    const std::string vs = gld::make_shadow_vertex_shader(src->second.vs);
    if (vs.empty()) return v;
    v.program = compile_program(vs, gld::generate_fragment_shader(src->second.fk), "shadow map variant");
    if (!v.program) return v;
    v.proj = glGetUniformLocation(v.program, "u_sh_proj");
    v.light = glGetUniformLocation(v.program, "u_sh_light");
    return v;
}

Renderer::ShadowVariant& Renderer::lift_variant(GLuint program, int stage) {
    auto [it, added] = lift_variants_.try_emplace((static_cast<std::uint64_t>(program) << 8) | static_cast<std::uint64_t>(stage & 7));
    ShadowVariant& v = it->second;
    if (!added) return v;
    const auto src = sources_.find(program);
    if (src == sources_.end()) return v;
    const std::string vs = gld::make_shadow_vertex_shader(src->second.vs);
    if (vs.empty()) return v;
    const std::string fs = "#version 450 core\nin vec4 v_tex[8];\nlayout(binding = 0) uniform sampler2D t_map;\n"
                           "out vec4 o_colour;\nvoid main() {\n    o_colour = vec4(texture(t_map, v_tex[" +
                           std::to_string(stage & 7) + "].xy).rgb, 1.0);\n}\n";
    v.program = compile_program(vs, fs, "level map variant");
    if (!v.program) return v;
    v.proj = glGetUniformLocation(v.program, "u_sh_proj");
    v.light = glGetUniformLocation(v.program, "u_sh_light");
    return v;
}

GLuint Renderer::fs_variant(GLuint program, std::uint32_t tag, const std::string& fs) {
    const std::uint64_t key = (static_cast<std::uint64_t>(program) << 32) | tag;
    if (auto it = fs_variants_.find(key); it != fs_variants_.end()) return it->second;
    GLuint v = 0;
    if (const auto src = sources_.find(program); src != sources_.end()) v = compile_program(src->second.vs, fs, "pixel shader variant");
    fs_variants_[key] = v;  // failures too (logged once)
    return v;
}

GLuint Renderer::bindless_variant(GLuint program, std::uint8_t mask) {
    if (!bindless_ || mask == 0) return 0;
    const std::uint64_t key = (static_cast<std::uint64_t>(program) << 8) | mask;
    if (auto it = bindless_programs_.find(key); it != bindless_programs_.end()) return it->second;
    GLuint v = 0;
    if (const auto src = sources_.find(program); src != sources_.end())
        v = compile_program(src->second.vs, gld::generate_fragment_shader(src->second.fk, mask), "bindless texture variant");
    bindless_programs_[key] = v;  // failures too (logged once)
    return v;
}

gl::GLuint64 Renderer::texture_handle(GLuint texture, GLuint sampler) {
    if (!bindless_ || texture == 0 || sampler == 0) return 0;
    const std::uint64_t key = (static_cast<std::uint64_t>(texture) << 32) | sampler;
    if (auto it = texture_handles_.find(key); it != texture_handles_.end()) return it->second;
    const gl::GLuint64 h = glGetTextureSamplerHandleARB(texture, sampler);
    if (h != 0) glMakeTextureHandleResidentARB(h);
    texture_handles_[key] = h;
    return h;
}

void Renderer::bind_uniform_range(GLuint binding, std::uint32_t offset) {
    if (!ring_ || binding == 2 || binding > 3 || offset == ~0u) return;
    if (binding == 3) {  // a record of storage buffer 3: the next draws' base instance
        draw_index_ = offset / kDrawRecord;
        return;
    }
    const auto size = binding == 0   ? static_cast<gl::GLsizeiptr>(sizeof(gld::FfpUniforms))
                      : binding == 3 ? static_cast<gl::GLsizeiptr>(sizeof(gld::FfpDrawUniforms))
                                     : static_cast<gl::GLsizeiptr>(sizeof(float[gld::kVsConstants][4]));
    flush_why(5);
    glBindBufferRange(GL_UNIFORM_BUFFER, binding, ring_, static_cast<gl::GLintptr>(offset), size);
}

void Renderer::rebind_uniforms() {
    for (GLuint b : {0u, 1u, 3u}) bind_uniform_range(b, bound_offset_[b]);
}

// ---------------------------------------------------------------------------
// redundant state filtering
// ---------------------------------------------------------------------------
void Renderer::invalidate_state() {
    flush_why(8);  // (late if GL state was set already: code setting GL state itself flushes first)
    indirect_bound_ = false;
    ++blend_mask_epoch_;
    raster_valid_ = viewport_valid_ = scissor_valid_ = program_valid_ = vao_valid_ = false;
    for (bool& v : unit_valid_) v = false;
}

void Renderer::forget_buffer(GLuint buffer) {
    if (!buffer) return;
    flush_why(8);
    ++resource_epoch_;
    deleted_buffers_.push_back(buffer);
    last_vao_buffers_ = nullptr;  // its entry may go
    for (auto it = vao_buffers_.begin(); it != vao_buffers_.end();) {
        if (it->second.vertex_buffer == buffer || it->second.index_buffer == buffer) it = vao_buffers_.erase(it);
        else ++it;
    }
}

void Renderer::forget_texture(GLuint texture) {
    if (!texture) return;
    flush_why(8);
    for (int u = 0; u < kUnits; ++u)
        if (unit_texture_[u] == texture) unit_valid_[u] = false;
    // Its bindless handles go before the texture is deleted.
    for (auto it = texture_handles_.begin(); it != texture_handles_.end();) {
        if ((it->first >> 32) == texture) {
            if (it->second) glMakeTextureHandleNonResidentARB(it->second);
            it = texture_handles_.erase(it);
        } else {
            ++it;
        }
    }
}

// ---------------------------------------------------------------------------
// uniform ring
// ---------------------------------------------------------------------------
std::uint32_t Renderer::ring_write(const void* data, std::uint32_t size, std::uint32_t align) {
    constexpr std::uint32_t kSegment = kRingSize / kRingSegments;
    if (align == 0) align = ring_align_;
    auto leaves_segment = [&] {
        const std::uint32_t a = (ring_cursor_ + align - 1) / align * align;
        const std::uint32_t s = ring_cursor_ ? (ring_cursor_ - 1) / kSegment : 0;
        return a + size > (s + 1) * kSegment;
    };
    // Queued draws read records of this segment: they go to the GPU before it is fenced - submitted first, before
    // this write takes its place (submitting writes their commands into the ring and moves the cursor itself).
    if (!submitting_ && !queued_.empty() && leaves_segment()) flush_why(6);
    std::uint32_t at = (ring_cursor_ + align - 1) / align * align;
    const std::uint32_t segment = ring_cursor_ ? (ring_cursor_ - 1) / kSegment : 0;  // the segment of the last write
    if (at + size > (segment + 1) * kSegment) {
        // Leaving this segment: fence it; the next one must be done with by the GPU before it is reused.
        if (ring_fences_[segment]) glDeleteSync(ring_fences_[segment]);
        ring_fences_[segment] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        ++ring_switches_;
        const std::uint32_t next = (segment + 1) % kRingSegments;
        if (GLsync f = std::exchange(ring_fences_[next], nullptr)) {
            GLenum r = glClientWaitSync(f, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
            if (r == GL_TIMEOUT_EXPIRED) {
                ++stats_.ring_waits;
                do r = glClientWaitSync(f, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000ull);
                while (r == GL_TIMEOUT_EXPIRED);
            }
            glDeleteSync(f);
        }
        at = (next * kSegment + align - 1) / align * align;
    }
    std::memcpy(ring_ptr_ + at, data, size);
    ring_cursor_ = at + size;
    return at;
}

void Renderer::upload_block(GLuint binding, const void* data, std::uint32_t size, std::vector<std::uint8_t>& shadow) {
    if (shadow.size() == size && !block_stale(binding) && std::memcmp(shadow.data(), data, size) == 0) {
        ++stats_.uniform_skipped;
        return;
    }
    ++stats_.uniform_uploads;
    flush_why(5);  // the block the queued draws read is bound anew
    shadow.assign(static_cast<const std::uint8_t*>(data), static_cast<const std::uint8_t*>(data) + size);
    if (ring_ptr_) {
        const std::uint32_t at = ring_write(data, size);
        glBindBufferRange(GL_UNIFORM_BUFFER, binding, ring_, static_cast<gl::GLintptr>(at), static_cast<gl::GLsizeiptr>(size));
        if (binding < 4) {
            bound_offset_[binding] = at;
            bound_switch_[binding] = ring_switches_;
        }
    } else {
        glNamedBufferSubData(binding == 0 ? ubo_ffp_ : binding == 3 ? ubo_draw_ : ubo_vs_, 0, static_cast<gl::GLsizeiptr>(size), data);
    }
}

bool Renderer::stream_storage(GLuint binding, const void* data, std::uint32_t size) {
    if (!ring_ptr_ || size == 0) return false;
    flush_why(5);
    const std::uint32_t at = ring_write(data, size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding, ring_, static_cast<gl::GLintptr>(at), static_cast<gl::GLsizeiptr>(size));
    return true;
}

// The ring writes a segment again kRingSegments switches after it: a block left bound since two switches
// ago is written again before its segment comes round (and before recorded draws that name it are replayed).
bool Renderer::block_stale(GLuint binding) const {
    return ring_ptr_ && binding < 4 && ring_switches_ - bound_switch_[binding] >= 2;
}

// ---------------------------------------------------------------------------
// programs
// ---------------------------------------------------------------------------
GLuint Renderer::compile_program(const std::string& vs, const std::string& fs, const std::string& what) {
    auto compile = [&](GLenum type, const std::string& src) -> GLuint {
        const GLuint sh = glCreateShader(type);
        const char* p = src.c_str();
        glShaderSource(sh, 1, &p, nullptr);
        glCompileShader(sh);
        GLint ok = 0;
        glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[4096] = {};
            glGetShaderInfoLog(sh, sizeof log, nullptr, log);
            hook::log_line("OpenGL device: %s %s shader failed to compile:\n%s\n---- source:\n%s", what.c_str(),
                           type == GL_VERTEX_SHADER ? "vertex" : "fragment", log, src.c_str());
            glDeleteShader(sh);
            return 0;
        }
        return sh;
    };
    const GLuint v = compile(GL_VERTEX_SHADER, vs);
    const GLuint f = compile(GL_FRAGMENT_SHADER, fs);
    if (!v || !f) {
        if (v) glDeleteShader(v);
        if (f) glDeleteShader(f);
        return 0;
    }
    const GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[4096] = {};
        glGetProgramInfoLog(p, sizeof log, nullptr, log);
        hook::log_line("OpenGL device: %s program failed to link:\n%s", what.c_str(), log);
        glDeleteProgram(p);
        return 0;
    }
    return p;
}

GLuint Renderer::ffp_program(const gld::VertexKey& vk, const gld::FragmentKey& fk) {
    // Built in a reused buffer and compared with the previous draw's key first: consecutive draws
    // mostly use the same program, and this runs for every draw.
    std::string& key = key_scratch_;
    key.assign("ff", 2);
    vk.append_to(key);
    fk.append_to(key);
    if (last_program_key_valid_ && key == last_program_key_) return last_program_;
    GLuint program;
    if (auto p = programs_.find(key); p != programs_.end()) program = p->second;
    else program = build_program(key, gld::generate_ffp_vertex_shader(vk), fk, false, vk.fvf);
    last_program_key_ = key;
    last_program_key_valid_ = true;
    return last_program_ = program;
}

GLuint Renderer::shader_program(std::uint32_t vs_handle, const std::string& vs_glsl, const gld::FragmentKey& fk) {
    std::string& key = key_scratch_;
    key.assign("vs", 2);
    key += std::to_string(vs_handle);
    fk.append_to(key);
    if (last_program_key_valid_ && key == last_program_key_) return last_program_;
    GLuint program;
    if (auto p = programs_.find(key); p != programs_.end()) program = p->second;
    else program = build_program(key, vs_glsl, fk, true, vs_handle);
    last_program_key_ = key;
    last_program_key_valid_ = true;
    return last_program_ = program;
}

GLuint Renderer::build_program(const std::string& key, const std::string& vs_src, const gld::FragmentKey& fk,
                               bool programmable, std::uint32_t id) {
    char what[64];
    std::snprintf(what, sizeof what, "program #%u (%s 0x%x)", static_cast<unsigned>(programs_.size() + 1),
                  programmable ? "vertex shader" : "FVF", id);
    const auto t0 = hitch::enabled() ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
    const GLuint program = compile_program(vs_src, gld::generate_fragment_shader(fk), what);
    if (hitch::enabled())
        hitch::note("%s compiled: %.1f ms", what, std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count());
    programs_[key] = program;  // failures are cached too (logged once)
    if (program) sources_[program] = ProgramSource{vs_src, fk};  // for shadow_variant (Shadows can be switched to 2 at run time)
    if (program && hook::settings().gl_debug && programs_.size() <= 2)
        hook::log_line("OpenGL device: %s source:\n%s\n----\n%s", what, vs_src.c_str(), gld::generate_fragment_shader(fk).c_str());
    if (program && hook::settings().gl_debug && programs_.size() <= 200)
        hook::log_line("OpenGL device: %s compiled (stage 0: colour op %u, alpha op %u, texture %s)", what,
                       fk.stages[0].color_op, fk.stages[0].alpha_op, fk.stages[0].has_texture ? "yes" : "no");
    return program;
}

unsigned Renderer::failed_program_count() const {
    unsigned failed = 0;
    for (const auto& [key, program] : programs_) failed += program == 0;
    return failed;
}

// ---------------------------------------------------------------------------
// vertex arrays
// ---------------------------------------------------------------------------
namespace {
void attrib(GLuint vao, GLuint loc, GLint size, GLenum type, bool normalized, GLuint offset) {
    glEnableVertexArrayAttrib(vao, loc);
    glVertexArrayAttribFormat(vao, loc, size, type, normalized ? 1 : 0, offset);
    glVertexArrayAttribBinding(vao, loc, 0);
}
}  // namespace

GLuint Renderer::vertex_array_elements(std::uint32_t handle, std::uint32_t stride, const std::vector<VertexElement>& elements) {
    if (last_vao_valid_ && last_vao_shader_ && last_vao_id_ == handle && last_vao_stride_ == stride) return last_vao_;
    const std::string key = "v" + std::to_string(handle) + ":" + std::to_string(stride);
    if (auto it = vertex_arrays_.find(key); it != vertex_arrays_.end()) {
        last_vao_valid_ = true, last_vao_shader_ = true, last_vao_id_ = handle, last_vao_stride_ = stride;
        return last_vao_ = it->second;
    }
    GLuint vao = 0;
    glCreateVertexArrays(1, &vao);
    for (const auto& e : elements) {
        switch (e.type) {  // D3DVSDT_*
            case 0: attrib(vao, e.reg, 1, GL_FLOAT, false, e.offset); break;
            case 1: attrib(vao, e.reg, 2, GL_FLOAT, false, e.offset); break;
            case 2: attrib(vao, e.reg, 3, GL_FLOAT, false, e.offset); break;
            case 3: attrib(vao, e.reg, 4, GL_FLOAT, false, e.offset); break;
            case 4: attrib(vao, e.reg, static_cast<GLint>(GL_BGRA), GL_UNSIGNED_BYTE, true, e.offset); break;
            case 5: attrib(vao, e.reg, 4, GL_UNSIGNED_BYTE, false, e.offset); break;
            case 6: attrib(vao, e.reg, 2, GL_SHORT, false, e.offset); break;
            case 7: attrib(vao, e.reg, 4, GL_SHORT, false, e.offset); break;
            default: break;
        }
    }
    vertex_arrays_[key] = vao;
    return vao;
}

GLuint Renderer::vertex_array_fvf(std::uint32_t fvf, std::uint32_t stride) {
    using namespace joan::d3d8;
    if (last_vao_valid_ && !last_vao_shader_ && last_vao_id_ == fvf && last_vao_stride_ == stride) return last_vao_;
    const std::string key = "f" + std::to_string(fvf) + ":" + std::to_string(stride);
    if (auto it = vertex_arrays_.find(key); it != vertex_arrays_.end()) {
        last_vao_valid_ = true, last_vao_shader_ = false, last_vao_id_ = fvf, last_vao_stride_ = stride;
        return last_vao_ = it->second;
    }
    GLuint vao = 0;
    glCreateVertexArrays(1, &vao);
    const std::uint32_t pos = fvf & kFvfPositionMask;
    GLuint offset = 0;
    if (pos == kFvfXyzRhw) {
        attrib(vao, 0, 4, GL_FLOAT, false, offset);
        offset += 16;
    } else if (pos) {
        attrib(vao, 0, 3, GL_FLOAT, false, offset);
        offset += 12;
        if (pos >= kFvfXyzB1) {
            int weights = static_cast<int>(pos - kFvfXyzB1) / 2 + 1;
            if (fvf & kFvfLastBetaUByte4) --weights;  // the last "weight" holds indices
            if (weights > 0) attrib(vao, 1, std::min(weights, 4), GL_FLOAT, false, offset);
            offset += static_cast<GLuint>((pos - kFvfXyzB1) / 2 + 1) * 4;
        }
    }
    if (fvf & kFvfNormal) {
        attrib(vao, 2, 3, GL_FLOAT, false, offset);
        offset += 12;
    }
    if (fvf & kFvfPSize) {
        attrib(vao, 3, 1, GL_FLOAT, false, offset);
        offset += 4;
    }
    if (fvf & kFvfDiffuse) {
        attrib(vao, 4, static_cast<GLint>(GL_BGRA), GL_UNSIGNED_BYTE, true, offset);
        offset += 4;
    }
    if (fvf & kFvfSpecular) {
        attrib(vao, 5, static_cast<GLint>(GL_BGRA), GL_UNSIGNED_BYTE, true, offset);
        offset += 4;
    }
    const std::uint32_t sets = (fvf & kFvfTexCountMask) >> kFvfTexCountShift;
    for (std::uint32_t i = 0; i < sets && i < 8; ++i) {
        static const GLint sizes[] = {2, 3, 4, 1};
        const GLint n = sizes[(fvf >> (16 + i * 2)) & 3];
        attrib(vao, 6 + i, n, GL_FLOAT, false, offset);
        offset += static_cast<GLuint>(n) * 4;
    }
    vertex_arrays_[key] = vao;
    return vao;
}

// ---------------------------------------------------------------------------
// samplers
// ---------------------------------------------------------------------------
void Renderer::set_texture_quality(std::uint32_t anisotropy, bool trilinear) {
    anisotropy_ = 0;
    if (anisotropy >= 2) {
        GLint limit = 0;
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &limit);
        anisotropy_ = std::min<std::uint32_t>(std::min<std::uint32_t>(anisotropy, 16), static_cast<std::uint32_t>(std::max(limit, 1)));
        if (anisotropy_ < 2) anisotropy_ = 0;
    }
    trilinear_ = trilinear;
}

GLuint Renderer::sampler(const SamplerDesc& d) {
    const std::uint32_t u = std::min<std::uint32_t>(d.address_u, 7), v = std::min<std::uint32_t>(d.address_v, 7);
    const std::uint32_t mag = std::min<std::uint32_t>(d.mag, 7), min = std::min<std::uint32_t>(d.min, 7);
    std::uint32_t mip = std::min<std::uint32_t>(d.mip, 7), aniso = std::min<std::uint32_t>(d.max_anisotropy, 16);
    bool force_aniso = false;
    // Quality options (set_texture_quality; both off by default = exactly the game's sampling). Only
    // samplers that already filter linearly between texels AND between mip levels (or mip POINT)
    // are touched: that is the 3D world. Point-filtered UI pixel art and the 2D screens (sampled
    // without mip filtering) never qualify, so their look cannot change.
    if (min >= 2 && mip >= 1) {
        if (trilinear_ && mag >= 2 && mip == 1) mip = 2;  // TextureFilter=1: POINT mip -> LINEAR mip
        if (anisotropy_ >= 2) {                           // Anisotropy=N: at least N
            aniso = std::max(min == 3 ? aniso : 1u, anisotropy_);
            force_aniso = true;
        }
    }
    // The effective values are part of the key (the options are fixed while the device lives).
    const std::uint32_t key = u | v << 3 | mag << 6 | min << 9 | mip << 12 | aniso << 15 | (force_aniso ? 1u : 0u) << 20;
    if (auto it = samplers_.find(key); it != samplers_.end()) return it->second;
    auto address = [](std::uint32_t a) -> GLint {
        switch (a) {
            case 2: return GL_MIRRORED_REPEAT;
            case 3: return GL_CLAMP_TO_EDGE;
            case 4: return GL_CLAMP_TO_BORDER;
            case 5: return GL_MIRROR_CLAMP_TO_EDGE;
            default: return GL_REPEAT;
        }
    };
    GLuint s = 0;
    glCreateSamplers(1, &s);
    glSamplerParameteri(s, GL_TEXTURE_WRAP_S, address(u));
    glSamplerParameteri(s, GL_TEXTURE_WRAP_T, address(v));
    glSamplerParameteri(s, GL_TEXTURE_MAG_FILTER, mag <= 1 ? GL_NEAREST : GL_LINEAR);
    const bool lin = min >= 2;
    GLint minf;
    if (mip == 0) minf = lin ? GL_LINEAR : GL_NEAREST;
    else if (mip == 1) minf = lin ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
    else minf = lin ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
    glSamplerParameteri(s, GL_TEXTURE_MIN_FILTER, minf);
    if ((min == 3 || force_aniso) && aniso > 1) glSamplerParameteri(s, GL_TEXTURE_MAX_ANISOTROPY, static_cast<GLint>(aniso));
    samplers_[key] = s;
    return s;
}

// ---------------------------------------------------------------------------
// fixed pipeline state
// ---------------------------------------------------------------------------
void Renderer::set_viewport(GLint x, GLint y, GLsizei w, GLsizei h) {
    if (viewport_valid_ && viewport_[0] == x && viewport_[1] == y && viewport_[2] == w && viewport_[3] == h) return;
    flush_why(4);
    glViewport(x, y, w, h);
    viewport_[0] = x, viewport_[1] = y, viewport_[2] = w, viewport_[3] = h;
    viewport_valid_ = true;
}

void Renderer::set_scissor(bool enabled, GLint x, GLint y, GLsizei w, GLsizei h) {
    if (enabled) {
        if (scissor_valid_ && scissor_on_ && scissor_[0] == x && scissor_[1] == y && scissor_[2] == w && scissor_[3] == h) return;
        flush_why(4);
        if (!scissor_valid_ || !scissor_on_) glEnable(GL_SCISSOR_TEST);
        glScissor(x, y, w, h);
        scissor_[0] = x, scissor_[1] = y, scissor_[2] = w, scissor_[3] = h;
    } else {
        if (scissor_valid_ && !scissor_on_) return;
        flush_why(4);
        glDisable(GL_SCISSOR_TEST);
    }
    scissor_on_ = enabled;
    scissor_valid_ = true;
}

// Only what differs from the GL state the Renderer last set is sent. raster_ mirrors the GL
// state: a function / op / mask is recorded only when it was set (e.g. the blend function is
// left as it was while blending is off, as before).
void Renderer::apply(const RasterState& s) {
    RasterState& g = raster_;
    // Queued draws keep the state they were queued with: each GL call below submits them first (flush_queued).
    const bool all = !raster_valid_;
    bool changed = all;
    auto enable = [&](GLenum cap, bool on, bool was) {
        if (!all && on == was) return;
        if (cap == GL_BLEND) ++blend_mask_epoch_;
        flush_why(0);
        if (on) glEnable(cap);
        else glDisable(cap);
        changed = true;
    };

    if (all || g.depth_min != s.depth_min || g.depth_max != s.depth_max) {
        flush_why(0);
        glDepthRangef(s.depth_min, s.depth_max);
        g.depth_min = s.depth_min, g.depth_max = s.depth_max, changed = true;
    }
    enable(GL_DEPTH_TEST, s.depth_test, g.depth_test);
    g.depth_test = s.depth_test;
    if (all || g.depth_func != s.depth_func) flush_why(0), glDepthFunc(s.depth_func), g.depth_func = s.depth_func, changed = true;
    if (all || g.depth_write != s.depth_write) flush_why(0), glDepthMask(s.depth_write ? 1 : 0), g.depth_write = s.depth_write, changed = true;

    // Front faces are CCW in NDC (= counter-clockwise on screen with glClipControl(GL_UPPER_LEFT));
    // see gld::Device::apply_raster_state for the Direct3D cull modes.
    enable(GL_CULL_FACE, s.cull != 0, g.cull != 0);
    if (s.cull && (all || g.cull != s.cull)) {
        flush_why(0);
        glFrontFace(GL_CCW);
        glCullFace(s.cull);
        changed = true;
    }
    g.cull = s.cull;
    if (all || g.polygon_mode != s.polygon_mode) flush_why(0), glPolygonMode(GL_FRONT_AND_BACK, s.polygon_mode), g.polygon_mode = s.polygon_mode, changed = true;

    enable(GL_BLEND, s.blend, g.blend);
    g.blend = s.blend;
    if (s.blend) {
        if (all || g.blend_src != s.blend_src || g.blend_dst != s.blend_dst) {
            flush_why(0);
            glBlendFunc(s.blend_src, s.blend_dst);
            ++blend_mask_epoch_;
            g.blend_src = s.blend_src, g.blend_dst = s.blend_dst, changed = true;
        }
        if (all || g.blend_equation != s.blend_equation) flush_why(0), glBlendEquation(s.blend_equation), g.blend_equation = s.blend_equation, changed = true, ++blend_mask_epoch_;
    }

    enable(GL_STENCIL_TEST, s.stencil, g.stencil);
    g.stencil = s.stencil;
    if (s.stencil) {
        if (all || g.stencil_func != s.stencil_func || g.stencil_ref != s.stencil_ref || g.stencil_mask != s.stencil_mask) {
            flush_why(0);
            glStencilFunc(s.stencil_func, s.stencil_ref, s.stencil_mask);
            g.stencil_func = s.stencil_func, g.stencil_ref = s.stencil_ref, g.stencil_mask = s.stencil_mask, changed = true;
        }
        if (all || g.stencil_fail != s.stencil_fail || g.stencil_depth_fail != s.stencil_depth_fail || g.stencil_pass != s.stencil_pass) {
            flush_why(0);
            glStencilOp(s.stencil_fail, s.stencil_depth_fail, s.stencil_pass);
            g.stencil_fail = s.stencil_fail, g.stencil_depth_fail = s.stencil_depth_fail, g.stencil_pass = s.stencil_pass;
            changed = true;
        }
        if (all || g.stencil_write_mask != s.stencil_write_mask) flush_why(0), glStencilMask(s.stencil_write_mask), g.stencil_write_mask = s.stencil_write_mask, changed = true;
    }

    if (all || g.color_mask != s.color_mask) {
        const std::uint8_t mask = s.color_mask;
        flush_why(0);
        glColorMask(mask & 1 ? 1 : 0, mask & 2 ? 1 : 0, mask & 4 ? 1 : 0, mask & 8 ? 1 : 0);
        ++blend_mask_epoch_;
        g.color_mask = mask, changed = true;
    }

    enable(GL_POLYGON_OFFSET_FILL, s.depth_bias_units != 0.0f, g.depth_bias_units != 0.0f);
    if (s.depth_bias_units != 0.0f && (all || g.depth_bias_units != s.depth_bias_units)) flush_why(0), glPolygonOffset(0.0f, s.depth_bias_units), changed = true;
    g.depth_bias_units = s.depth_bias_units;

    raster_valid_ = true;
    if (changed) ++stats_.raster_applied;
    else ++stats_.raster_skipped;
}

// ---------------------------------------------------------------------------
// uniform blocks
// ---------------------------------------------------------------------------
void Renderer::upload_ffp(const gld::FfpUniforms& u) { upload_block(0, &u, sizeof u, shadow_ffp_); }
// Block FfpDraw: a record in the ring (storage buffer 3, bound once); the draw's base instance names it. The device
// decides when a new record is needed (its inputs' versions), so no compare here.
void Renderer::upload_ffp_draw(const gld::FfpDrawUniforms& u) {
    ++stats_.uniform_uploads;
    if (!ring_ptr_) {
        glNamedBufferSubData(ubo_draw_, 0, static_cast<gl::GLsizeiptr>(sizeof u), &u);
        draw_index_ = 0;
        return;
    }
    const std::uint32_t at = ring_write(&u, kDrawRecord, kDrawRecord);
    draw_index_ = at / kDrawRecord;
    bound_offset_[3] = at;
    bound_switch_[3] = ring_switches_;
}

void Renderer::upload_vs_constants(const float (*constants)[4], std::uint32_t count) {
    upload_block(1, constants, count * 16u, shadow_vs_);
}

// ---------------------------------------------------------------------------
// draw submission
// ---------------------------------------------------------------------------
void Renderer::use_program(GLuint program) {
    if (program_valid_ && program_ == program) return;
    flush_why(1);
    glUseProgram(program);
    program_ = program;
    program_valid_ = true;
    ++stats_.program_binds;
}

void Renderer::bind_geometry(GLuint vao, GLuint vertex_buffer, std::uint32_t stride, GLuint index_buffer, gl::GLintptr vertex_offset) {
    if (!vao_valid_ || vao_ != vao) {
        flush_why(2);
        glBindVertexArray(vao);
        vao_ = vao;
        vao_valid_ = true;
    }
    if (!last_vao_buffers_ || last_vao_buffers_vao_ != vao) {
        last_vao_buffers_ = &vao_buffers_.try_emplace(vao, VaoBuffers{~0u, ~0u, ~0u, -1}).first->second;
        last_vao_buffers_vao_ = vao;
    }
    VaoBuffers& b = *last_vao_buffers_;
    if (b.vertex_buffer != vertex_buffer || b.stride != stride || b.offset != vertex_offset) {
        flush_why(2);
        glVertexArrayVertexBuffer(vao, 0, vertex_buffer, vertex_offset, static_cast<GLsizei>(stride));
        b.vertex_buffer = vertex_buffer, b.stride = stride, b.offset = vertex_offset;
        ++stats_.geometry_binds;
    }
    if (b.index_buffer != index_buffer) {
        flush_why(2);
        glVertexArrayElementBuffer(vao, index_buffer);
        b.index_buffer = index_buffer;
    }
}

void Renderer::bind_texture(GLuint unit, GLuint texture, GLuint sampler) {
    if (unit < static_cast<GLuint>(kUnits)) {
        // The texture and the sampler apart: most texture changes keep the stage's sampler.
        const bool same_texture = unit_valid_[unit] && unit_texture_[unit] == texture;
        const bool same_sampler = unit_valid_[unit] && unit_sampler_[unit] == sampler;
        if (same_texture && same_sampler) return;
        flush_why(3);
        if (!same_texture) glBindTextureUnit(unit, texture);
        if (!same_sampler) glBindSampler(unit, sampler);
        unit_texture_[unit] = texture, unit_sampler_[unit] = sampler, unit_valid_[unit] = true;
        ++stats_.texture_binds;
        return;
    }
    flush_why(3);
    glBindTextureUnit(unit, texture);
    glBindSampler(unit, sampler);
    ++stats_.texture_binds;
}

// One instance whose base instance is the FfpDraw record (gl_BaseInstance in the vertex shader).
void Renderer::draw_arrays(GLenum mode, GLint first, GLsizei count) {
    flush_why(7);
    glDrawArraysInstancedBaseInstance(mode, first, count, 1, draw_index_);
}

void Renderer::draw_elements(GLenum mode, GLsizei count, bool wide, std::uintptr_t byte_offset, GLint base_vertex) {
    flush_why(7);
    glDrawElementsInstancedBaseVertexBaseInstance(mode, count, wide ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT,
                                                  reinterpret_cast<const void*>(byte_offset), 1, base_vertex, draw_index_);
}

void Renderer::queue_elements(GLenum mode, GLsizei count, std::uintptr_t byte_offset, GLint base_vertex) {
    if (mode != queued_mode_) flush_why(10);
    queued_mode_ = mode;
    queued_.push_back({static_cast<GLuint>(count), 1u, static_cast<GLuint>(byte_offset / 2u), base_vertex, draw_index_});
    ++queue_draws_;
}

// The queued draws as one multi-draw: their commands go into the ring (bound as the indirect buffer). A segment
// switch while writing them fenced the segment before the draw was issued: that fence is set again after it.
void Renderer::submit_queued() {
    submitting_ = true;
    const auto n = static_cast<GLsizei>(queued_.size());
    if (ring_ptr_) {
        const std::uint32_t switches = ring_switches_;
        const std::uint32_t at = ring_write(queued_.data(), static_cast<std::uint32_t>(queued_.size() * sizeof(IndirectDraw)), 4);
        if (!indirect_bound_) {
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ring_);
            indirect_bound_ = true;
        }
        glMultiDrawElementsIndirect(queued_mode_, GL_UNSIGNED_SHORT, reinterpret_cast<const void*>(static_cast<std::uintptr_t>(at)), n, 0);
        if (ring_switches_ != switches) {
            constexpr std::uint32_t kSegment = kRingSize / kRingSegments;
            const std::uint32_t left = ((ring_cursor_ - 1) / kSegment + kRingSegments - 1) % kRingSegments;
            if (ring_fences_[left]) glDeleteSync(ring_fences_[left]);
            ring_fences_[left] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }
    } else {
        for (const IndirectDraw& d : queued_)
            glDrawElementsInstancedBaseVertexBaseInstance(queued_mode_, static_cast<GLsizei>(d.count), GL_UNSIGNED_SHORT,
                                                          reinterpret_cast<const void*>(static_cast<std::uintptr_t>(d.first_index) * 2u), 1,
                                                          d.base_vertex, d.base_instance);
    }
    ++queue_calls_;
    queued_.clear();
    submitting_ = false;
}

}  // namespace joan::render
