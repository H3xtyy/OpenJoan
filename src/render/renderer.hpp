// Native OpenGL 4.6 execution core (stage 3, step N1; see notes/STAGE3.md).
//
// The parts of the Direct3D 8 emulation (gl/device.cpp) that do not depend on
// Direct3D semantics: program and vertex-array caches, samplers, the fixed
// pipeline state (depth / cull / blend / stencil / colour mask / depth bias),
// the uniform blocks and draw submission. gl::Device translates Direct3D state
// into the structs below and draws through one Renderer; native code (the C++
// en3d draw path, N2) will draw through the same object, so both share one GL
// state and one set of caches.
//
// The Renderer assumes the device's GL context is current on the calling
// thread (gl::Device::begin_native_draw checks the thread and binds the render
// target before native draws). It issues exactly the GL calls the device used
// to issue itself.
//
// Its GL objects are created by init() and freed by shutdown(), which the
// owner calls while the context is still current; the destructor does not
// touch GL.
//
// Redundant state is filtered: the raster state, viewport, scissor, program,
// vertex array (and its buffers), texture units and uniform blocks are only
// sent to GL when they differ from what this Renderer last set. Code that
// changes any of that GL state behind its back (the device's own cursor /
// overlay / clear / resolve passes, the ambient occlusion passes) must call
// invalidate_state() afterwards; deleted buffers and textures are reported
// with forget_buffer() / forget_texture(). Uniform blocks are written into a
// persistently mapped ring buffer and bound by range (no glBufferSubData into
// a buffer the GPU may still be reading, which made the driver copy or wait).
#pragma once

#include "overrides.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "../gl/glapi.hpp"
#include "../gl/shaders.hpp"

namespace joan::render {

/// OpenGL type GLenum (see gl/glapi.hpp).
using gl::GLenum;
/// OpenGL type GLint (see gl/glapi.hpp).
using gl::GLint;
/// OpenGL type GLsizei (see gl/glapi.hpp).
using gl::GLsizei;
/// OpenGL type GLuint (see gl/glapi.hpp).
using gl::GLuint;

/// One input of a vertex shader declaration (D3DVSD_REG): input register,
/// D3DVSDT_* type, byte offset in the vertex.
struct VertexElement {
    std::uint32_t reg;  ///< input register (D3DVSD_REG)
    std::uint32_t type;  ///< D3DVSDT_* type code
    std::uint32_t offset;  ///< byte offset in the vertex
};

/// Texture sampling state. Values are still numbered as Direct3D 8's for now:
/// address D3DTADDRESS_* (1 wrap, 2 mirror, 3 clamp, 4 border, 5 mirror once),
/// filters D3DTEXF_* (0 none, 1 point, 2 linear, 3 anisotropic).
struct SamplerDesc {
    std::uint32_t address_u = 1;  ///< D3DTADDRESS_* of the U coordinate (1 = wrap)
    std::uint32_t address_v = 1;  ///< D3DTADDRESS_* of the V coordinate (1 = wrap)
    std::uint32_t mag = 1;  ///< D3DTEXF_* magnification filter
    std::uint32_t min = 1;  ///< D3DTEXF_* minification filter
    std::uint32_t mip = 0;  ///< D3DTEXF_* mip filter (0 = none)
    std::uint32_t max_anisotropy = 1;            ///< taps for D3DTEXF_ANISOTROPIC (1 = off)
};

/// Fixed pipeline state, already resolved to GL values. Viewport and scissor
/// are set separately (set_viewport / set_scissor).
struct RasterState {
    float depth_min = 0.0f;  ///< window depth range minimum (glDepthRangef), 0..1
    float depth_max = 1.0f;  ///< window depth range maximum (glDepthRangef), 0..1
    bool depth_test = false;                     ///< depth test enabled
    GLenum depth_func = gl::GL_LEQUAL;           ///< depth comparison (GL_* enum)
    bool depth_write = true;                     ///< depth buffer writes enabled
    GLenum cull = 0;  ///< 0 = no culling, else GL_BACK / GL_FRONT (front faces are CCW)
    GLenum polygon_mode = gl::GL_FILL;           ///< GL_FILL or GL_LINE (wireframe)
    bool blend = false;                          ///< colour blending enabled
    GLenum blend_src = gl::GL_ONE;  ///< source blend factor (GL_* enum)
    GLenum blend_dst = gl::GL_ZERO;  ///< destination blend factor (GL_* enum)
    GLenum blend_equation = gl::GL_FUNC_ADD;     ///< blend equation (GL_* enum)
    bool stencil = false;                        ///< stencil test enabled
    GLenum stencil_func = gl::GL_ALWAYS;         ///< stencil comparison (GL_* enum)
    GLint stencil_ref = 0;                       ///< stencil reference value
    GLuint stencil_mask = 0xffffffffu;           ///< bits of the stencil buffer the comparison reads
    GLenum stencil_fail = gl::GL_KEEP;  ///< stencil action when the stencil test fails
    GLenum stencil_depth_fail = gl::GL_KEEP;  ///< stencil action when the stencil test passes and the depth test fails
    GLenum stencil_pass = gl::GL_KEEP;  ///< stencil action when both pass
    GLuint stencil_write_mask = 0xffffffffu;     ///< bits of the stencil buffer that can be written
    std::uint8_t color_mask = 0xf;  ///< colour channels written: bit 0 red, 1 green, 2 blue, 3 alpha
    float depth_bias_units = 0.0f;  ///< glPolygonOffset(0, units); 0 = polygon offset off
};

/// Per-frame counters of the Renderer's GL traffic ([Graphics] PerfLog).
struct RendererStats {
    unsigned raster_applied = 0;  ///< apply() calls that sent state to GL
    unsigned raster_skipped = 0;  ///< apply() calls that found the state unchanged
    unsigned uniform_uploads = 0;  ///< uniform block writes issued
    unsigned uniform_skipped = 0;  ///< uniform block writes skipped as redundant
    unsigned ring_waits = 0;  ///< waits on a uniform ring fence
    unsigned program_binds = 0;  ///< glUseProgram calls issued
    unsigned geometry_binds = 0;  ///< vertex array / buffer binds issued
    unsigned texture_binds = 0;  ///< texture unit binds issued
};

/// The OpenGL 4.6 execution core shared by gl::Device (Direct3D 8 emulation) and the native draw path:
/// program, vertex-array and sampler caches, redundant-state filtering, the uniform ring and draw
/// submission. Not copyable; needs the device's GL context current for every call. Owned by gl::Device;
/// init() / shutdown() bracket its GL objects (see the file comment).
class Renderer {
   public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    /// Creates the uniform buffers and binds them to blocks 0 (Ffp) and 1 (VsConst).
    void init();
    /// Deletes every GL object the Renderer owns; the context must be current.
    void shutdown();

    // --- programs -------------------------------------------------------------------
    /// Compiles and links; logs and returns 0 on failure. Not cached.
    GLuint compile_program(const std::string& vs, const std::string& fs, const std::string& what);
    /// The fixed-function program for a state combination (cached, failures too).
    GLuint ffp_program(const gld::VertexKey& vk, const gld::FragmentKey& fk);
    /// A translated vertex shader (`vs_glsl`, identified by `vs_handle`) with the
    /// fixed-function pixel pipeline `fk` (cached, failures too).
    GLuint shader_program(std::uint32_t vs_handle, const std::string& vs_glsl, const gld::FragmentKey& fk);
    /// Number of cached programs (failed ones included).
    std::size_t program_count() const { return programs_.size(); }
    /// How many of the cached programs failed to compile or link.
    unsigned failed_program_count() const;

    // --- vertex arrays (stream 0 = binding 0) -----------------------------------------
    /// Attribute layout of a flexible vertex format (locations as in shaders.hpp).
    GLuint vertex_array_fvf(std::uint32_t fvf, std::uint32_t stride);
    /// Attribute layout of a vertex shader declaration (location = input register).
    GLuint vertex_array_elements(std::uint32_t handle, std::uint32_t stride, const std::vector<VertexElement>& elements);

    // --- samplers ---------------------------------------------------------------------
    /// The sampler object for `desc` (cached), with the quality options of set_texture_quality applied.
    GLuint sampler(const SamplerDesc& desc);
    /// Graphics quality options ([Graphics] Anisotropy / TextureFilter), applied by sampler() to
    /// mipmapped, linearly filtered sampling only (3D surfaces; point-filtered pixel art and
    /// textures sampled without mip filtering are left exactly as the game asks):
    ///  * `anisotropy` >= 2: GL_TEXTURE_MAX_ANISOTROPY at least this (clamped to the driver's limit);
    ///    0 / 1 = off (only the game's own D3DTEXF_ANISOTROPIC requests use anisotropy).
    ///  * `trilinear`: a mip filter of POINT becomes LINEAR (no visible mip-level seams).
    /// Call before the first sampler() (needs the context; queries the anisotropy limit). The
    /// defaults (0, false) leave every sampler as the game asks.
    void set_texture_quality(std::uint32_t anisotropy, bool trilinear);
    /// The anisotropy set by set_texture_quality(): 0 = off, else 2..16.
    std::uint32_t anisotropy() const { return anisotropy_; }

    // --- fixed pipeline state -------------------------------------------------------------
    /// Viewport in pixels (origin as GL: bottom-left); not sent when it equals the last one set.
    void set_viewport(GLint x, GLint y, GLsizei w, GLsizei h);
    /// Enables the scissor test with the given box, or disables it (the box is then ignored).
    void set_scissor(bool enabled, GLint x, GLint y, GLsizei w, GLsizei h);
    /// Sends the fixed pipeline state `s` to GL, skipping what equals the last state applied.
    void apply(const RasterState& s);

    // --- uniform blocks -------------------------------------------------------------------
    /// Writes block Ffp (binding 0), skipped when its bytes equal the last upload and the ring segment is intact.
    void upload_ffp(const gld::FfpUniforms& u);
    /// Block FfpDraw (binding 3): appends a new per-draw record, always written.
    void upload_ffp_draw(const gld::FfpDrawUniforms& u);
    /// The next draw reads the last FfpDraw record again (only counts the skipped upload).
    void reuse_ffp_draw() { ++stats_.uniform_skipped; }
    /// The block bound at `binding` is in a ring segment that is about to be written again: upload it anew
    /// even if unchanged (upload_* do so by themselves).
    bool block_stale(GLuint binding) const;
    /// `size` bytes of `data` into the uniform ring, bound as shader storage buffer `binding` (vertex data a
    /// shader reads per draw: GpuShadowVolumes). False = no ring.
    bool stream_storage(GLuint binding, const void* data, std::uint32_t size);
    /// The first `count` vec4 constants of block VsConst.
    void upload_vs_constants(const float (*constants)[4], std::uint32_t count);

    // --- draw submission --------------------------------------------------------------------
    /// Makes `program` current (no GL call when it already is).
    void use_program(GLuint program);
    /// Binds `vao` with `vertex_buffer` on binding 0 and `index_buffer` (0 = none) as its element buffer.
    void bind_geometry(GLuint vao, GLuint vertex_buffer, std::uint32_t stride, GLuint index_buffer, gl::GLintptr vertex_offset = 0);
    /// Binds `texture` and `sampler` to texture unit `unit` (0..7), skipping what is already bound.
    void bind_texture(GLuint unit, GLuint texture, GLuint sampler);
    /// glDrawArrays with the current state; `first` and `count` are in vertices.
    void draw_arrays(GLenum mode, GLint first, GLsizei count);
    /// `byte_offset` into the element buffer; `wide` = 32-bit indices, else 16-bit.
    void draw_elements(GLenum mode, GLsizei count, bool wide, std::uintptr_t byte_offset, GLint base_vertex);
    /// [Graphics] GrassBatching: draw_elements (16-bit indices) with the current state, queued: the draws queued one
    /// after another go to the GPU as one glMultiDrawElementsIndirect, in their order. Every state change through
    /// the Renderer, every other draw and every switch of the uniform ring's segment submits the queue first, so
    /// each queued draw runs with the state it was queued with; GL state set outside the Renderer needs
    /// flush_queued() before it.
    void queue_elements(GLenum mode, GLsizei count, std::uintptr_t byte_offset, GLint base_vertex);
    /// Submits the queue of queue_elements() draws now (reason: "the caller").
    void flush_queued() { flush_why(9); }
    /// GL_ARB_bindless_texture is there (found by init).
    bool bindless() const { return bindless_; }
    /// The FFP `program` with texture stages `mask` (bits 0..2) sampled through handles in each draw's record
    /// (gld::generate_fragment_shader); 0 = not possible (no source / failed - cached too).
    GLuint bindless_variant(GLuint program, std::uint8_t mask);
    /// A resident handle of `texture` sampled with `sampler`, kept until the texture is forgotten; 0 = failed.
    gl::GLuint64 texture_handle(GLuint texture, GLuint sampler);
    /// Queued draws and the multi-draw calls they went out in, since the last call (PerfLog); `why` (11): what
    /// submitted them - raster state, program, geometry, texture, viewport / scissor, uniform block, ring segment,
    /// another draw, invalidate / forget, the caller, primitive type.
    void take_queue_stats(unsigned& draws, unsigned& calls, unsigned* why) {
        draws = queue_draws_, calls = queue_calls_;
        queue_draws_ = queue_calls_ = 0;
        for (int i = 0; i < 11; ++i) why[i] = queue_why_[i], queue_why_[i] = 0;
    }

    // --- shadow maps ([Graphics] Shadows=2, render/shadow_map.hpp) ---------------------------------
    /// A program derived for shadow-map / baked-shadow drawing and its uniform locations.
    struct ShadowVariant {
        GLuint program = 0;  ///< GL program; 0 = none (2D vertices, or failed)
        GLint proj = -1;     ///< location of u_sh_proj (-1 = none)
        GLint light = -1;    ///< location of u_sh_light (-1 = none)
        std::uint32_t frame = 0;      ///< the frame its uniforms were last set for (ShadowPass)
    };
    /// The shadow-map version of `program` (gld::make_shadow_vertex_shader + the same pixel pipeline,
    /// so alpha-tested cut-outs keep their shape), made on first use. Programs keep their sources for
    /// this only while Shadows=2.
    ShadowVariant& shadow_variant(GLuint program);
    /// [Graphics] BakedShadows: `program` drawn into the level map (render/baked_shadows.hpp): its shadow-map
    /// vertex shader (u_sh_light: view -> the map's clip space) and a pixel shader writing stage `stage`'s
    /// texture (bound to unit 0) at v_tex[stage].
    ShadowVariant& lift_variant(GLuint program, int stage);
    /// `program` with its own vertex shader and the pixel shader `fs` instead (cached under `tag`: one pixel shader
    /// per tag); 0 = no source / failed. [Graphics] Water's rivers.
    GLuint fs_variant(GLuint program, std::uint32_t tag, const std::string& fs);
    /// The ring offset of the uniform block bound at `binding` (0 Ffp, 1 VsConst, 3 FfpDraw), ~0u if none / not a ring.
    std::uint32_t uniform_offset(GLuint binding) const { return binding < 4 ? bound_offset_[binding] : ~0u; }
    /// Binds a block's range of the uniform ring (a recorded draw's) / the current ones again.
    void bind_uniform_range(GLuint binding, std::uint32_t offset);
    /// Binds the current ring range of every uniform block again.
    void rebind_uniforms();

    // --- redundant state filtering ------------------------------------------------------------
    /// GL state was changed outside the Renderer: everything is set again by the next calls.
    void invalidate_state();
    /// `buffer` / `texture` is being deleted (its name may be reused by a new object).
    void forget_buffer(GLuint buffer);
    /// See forget_buffer().
    void forget_texture(GLuint texture);
    /// The traffic counters (the owner reads and resets them per frame).
    RendererStats& stats() { return stats_; }
    /// Changes whenever the Renderer issued a global colour mask or blend call (which set every draw buffer) or
    /// its state was invalidated: code that keeps draw buffer 1's own state re-applies it then.
    std::uint32_t blend_mask_epoch() const { return blend_mask_epoch_; }
    /// The uniform ring's segment changes so far: a block written at count N stays intact while the
    /// count is below N + ring_segments() (the ring comes back to its segment then).
    std::uint32_t ring_switches() const { return ring_switches_; }
    /// Number of segments of the uniform ring.
    static constexpr std::uint32_t ring_segments() { return kRingSegments; }
    /// Changes whenever a buffer is deleted (recorded draws naming it may read a new buffer out of range).
    std::uint32_t resource_epoch() const { return resource_epoch_; }
    /// Buffers deleted since clear_deleted_buffers (recorded draws naming one are dropped).
    const std::vector<GLuint>& deleted_buffers() const { return deleted_buffers_; }
    /// Forgets the deleted_buffers() list.
    void clear_deleted_buffers() { deleted_buffers_.clear(); }

   private:
    // Uniform ring: a persistently mapped buffer split into kRingSegments parts, each fenced once
    // the writes move past it and waited for before it is written again.
    static constexpr std::uint32_t kRingSize = 32u << 20, kRingSegments = 4;  // ~13000 FFP blocks per lap
    /// `size` bytes into the ring at a multiple of `align` (0: the uniform buffer offset alignment).
    std::uint32_t ring_write(const void* data, std::uint32_t size, std::uint32_t align = 0);
    // Block FfpDraw: the ring bound whole as storage buffer 3 (an array of records); a draw's base instance is its
    // record's index (ring offset / record size) - no GL call per draw. Without the ring: ubo_draw_, record 0.
    static constexpr std::uint32_t kDrawRecord = static_cast<std::uint32_t>(sizeof(gld::FfpDrawUniforms));
    std::uint32_t draw_index_ = 0;
    // queue_elements: DrawElementsIndirectCommand records, written to the ring when submitted.
    struct IndirectDraw {
        GLuint count, instance_count, first_index;
        GLint base_vertex;
        GLuint base_instance;
    };
    std::vector<IndirectDraw> queued_;
    GLenum queued_mode_ = 0;
    bool submitting_ = false, indirect_bound_ = false;
    unsigned queue_draws_ = 0, queue_calls_ = 0, queue_why_[11] = {};
    bool bindless_ = false;
    std::unordered_map<std::uint64_t, GLuint> bindless_programs_;      // program << 8 | mask
    std::unordered_map<std::uint64_t, gl::GLuint64> texture_handles_;  // texture << 32 | sampler
    void submit_queued();
    void flush_why(int why) {
        if (queued_.empty()) return;
        ++queue_why_[why];
        submit_queued();
    }
    void upload_block(GLuint binding, const void* data, std::uint32_t size, std::vector<std::uint8_t>& shadow);
    /// Compiles `vs_src` with the pixel pipeline of `fk` and caches the result under `key`
    /// (failures too, so they are logged once). `id` is the FVF or vertex shader handle (log text).
    GLuint build_program(const std::string& key, const std::string& vs_src, const gld::FragmentKey& fk,
                         bool programmable, std::uint32_t id);

    GLuint ubo_ffp_ = 0, ubo_vs_ = 0, ubo_draw_ = 0;  // fallback (no ring): plain buffers bound to blocks 0 / 1 / 3
    GLuint ring_ = 0;
    std::uint8_t* ring_ptr_ = nullptr;
    std::uint32_t ring_cursor_ = 0, ring_align_ = 256;
    std::uint32_t ring_switches_ = 0, resource_epoch_ = 0;
    std::vector<GLuint> deleted_buffers_;
    gl::GLsync ring_fences_[kRingSegments] = {};
    std::vector<std::uint8_t> shadow_ffp_, shadow_vs_, shadow_draw_;  // last uploaded contents of blocks 0 / 1 / 3 (empty = unknown)
    std::uint32_t bound_offset_[4] = {~0u, ~0u, ~0u, ~0u};  // ring offsets bound at blocks 0 / 1 / (2: unused) / 3
    std::uint32_t bound_switch_[4] = {};                     // ring_switches_ when each was written
    struct ProgramSource {
        std::string vs;
        gld::FragmentKey fk;
    };
    std::unordered_map<GLuint, ProgramSource> sources_;  // Shadows=2 only
    std::unordered_map<GLuint, ShadowVariant> shadow_variants_;
    std::unordered_map<std::uint64_t, ShadowVariant> lift_variants_;  // program << 8 | stage
    std::unordered_map<std::uint64_t, GLuint> fs_variants_;          // program << 32 | tag

    // What the Renderer last sent (valid flags false = unknown, send again).
    bool raster_valid_ = false, viewport_valid_ = false, scissor_valid_ = false;
    RasterState raster_;
    GLint viewport_[4] = {};
    bool scissor_on_ = false;
    GLint scissor_[4] = {};
    GLuint program_ = 0, vao_ = 0;
    bool program_valid_ = false, vao_valid_ = false;
    struct VaoBuffers {
        GLuint vertex_buffer, index_buffer;
        std::uint32_t stride;
        gl::GLintptr offset;
    };
    std::unordered_map<GLuint, VaoBuffers> vao_buffers_;  // vertex array state (kept across invalidate_state)
    GLuint last_vao_buffers_vao_ = 0;            // vao_buffers_ entry of the last bind_geometry (no lookup when the
    VaoBuffers* last_vao_buffers_ = nullptr;     // same vertex array is bound again); reset when entries go
    static constexpr int kUnits = 8;
    GLuint unit_texture_[kUnits] = {}, unit_sampler_[kUnits] = {};
    bool unit_valid_[kUnits] = {};
    RendererStats stats_;
    std::uint32_t blend_mask_epoch_ = 0;
    std::unordered_map<std::string, GLuint> programs_;
    std::unordered_map<std::string, GLuint> vertex_arrays_;
    // The previous lookups (programs_ / vertex_arrays_ only change by insertion, so they stay right).
    std::string key_scratch_, last_program_key_;
    bool last_program_key_valid_ = false;
    GLuint last_program_ = 0;
    bool last_vao_valid_ = false, last_vao_shader_ = false;
    std::uint32_t last_vao_id_ = 0, last_vao_stride_ = 0;
    GLuint last_vao_ = 0;
    std::unordered_map<std::uint32_t, GLuint> samplers_;
    std::uint32_t anisotropy_ = 0;  // set_texture_quality: 0 = off, else 2..16 (clamped to the GL limit)
    bool trilinear_ = false;
};

/// The renderer of the current device (set by gl::Device::init, cleared when the
/// device is destroyed); nullptr while there is none. Native draws must go through
/// gl::Device::begin_native_draw first.
Renderer* current();
/// Sets the renderer of the current device (`nullptr` clears it).
void set_current(Renderer* renderer);

}  // namespace joan::render
