// [Graphics] Shadows=2: sun shadows from a shadow map, replacing the original stencil shadow volumes
// (en3d/scene.cpp Scene::draw_shadows, which Shadows != 1 turns off).
//
// Not part of the original game. The engine's work is not repeated: while the scene's opaque pass
// is drawn, the OpenGL device records each of its 3D, depth-writing draws (program, vertex / index
// buffers and offsets, the uniform blocks' ranges in the uniform ring, the alpha-test texture). When
// the opaque world is complete (shadow_opaque_done) the recorded draws are replayed depth-only into a
// shadow map seen from the sun, with each program's shadow variant (gld::make_shadow_vertex_shader:
// the camera clip position turned back into view space, then into the light's clip space), and the
// shadows are applied to the picture in screen space: each pixel's view position from the depth
// buffer, looked up in the shadow map with a soft (Poisson-disc PCF) filter, and darkened.
//
// The shadow map is one orthographic view from the sun covering ShadowDistance around what the camera
// looks at, snapped to whole texels (no shimmering while the camera moves). Surfaces facing away from
// the sun are not darkened again (their lighting already is), and the shadows fade out towards
// ShadowDistance. Casters are what the camera's opaque pass draws and (ShadowFoliage) the alpha-tested
// cut-outs of the transparent pass (trees, bushes): those are drawn after the shadows are applied, so at
// the end of that pass they are added to the same shadow map (the opaque casters' map kept as a copy) and
// the picture is darkened once more by what they add: (1 - shadow of all) / (1 - shadow of the opaque).
#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "../gl/glapi.hpp"

namespace joan::render {

class Renderer;

/// Live parameters (initialised from openjoan.ini [Graphics] Shadows*; the developer menu changes them).
struct ShadowParams {
    int mode = 1;              ///< 0 off, 1 the original stencil shadows, 2 shadow maps
    float distance = 4000.0f;  ///< world units covered from the camera
    int map_size = 4096;       ///< texels per side
    float strength = 0.45f;    ///< darkening in full shadow, 0..1
    float softness = 3.0f;     ///< penumbra radius in shadow map texels
    bool foliage = true;       ///< the transparent pass's cut-outs cast too (the previous frame's, see ShadowGroup)
    float baked = 0.25f;       ///< part of the lightmaps' baked shadows kept (mode 2; render/baked_shadows.hpp)
    float caster_margin = 1500.0f;  ///< world units around the picture whose objects are drawn to cast (Scene::render)
    int debug = 0;  ///< developer menu: 1 tints the shadow map's darkening blue instead of applying it
};
/// The live ShadowParams.
ShadowParams& shadow_params();

/// Called by the en3d scene draw before its opaque pass: `sun` = the direction the sunlight travels
/// (world space, 3 floats), or nullptr (no shadow-casting sun, shadows off): no recording.
void shadow_scene_begin(const float* sun);
/// Called when the opaque world is complete, before the ambient occlusion (`projection`: the camera's
/// Direct3D projection matrix).
void shadow_opaque_done(const float* projection);

/// ShadowCasterMargin: Scene::render moves the camera's frustum planes out (casters off screen and behind
/// the camera still drawn) while it draws; the camera's near-range tests (Camera::sphere_near /
/// near_distance: objects too close to the camera, draw order) keep its real near plane, kept here.
struct NearPlaneOverride {
    const void* camera = nullptr;  ///< the camera whose frustum is widened, nullptr = none; identity only, not owned
    float plane[4] = {};           ///< the camera's real near plane (plane equation, 4 floats)
};
/// The process-wide NearPlaneOverride.
NearPlaneOverride& near_plane_override();

/// A Direct3D (row-major) 4x4 matrix's inverse; false = singular.
bool invert_matrix(const float* m, float* out);

/// The scene's overlays - billboards drawn before the world (Scene::render's "materials": unit name labels,
/// health plates), with their own depth: the device copies the depth buffer at overlay_begin and at
/// overlay_end, and the shadow / water passes leave alone the pixels the billboards changed whose final depth
/// is still theirs (Input::overlay_before / overlay_depth).
void overlay_begin();
/// Ends the overlay set (see overlay_begin).
void overlay_end();
/// Callback of the device for overlay_begin (`begin` true) and overlay_end (`begin` false).
using OverlayHook = void (*)(bool begin);
void set_overlay_hook(OverlayHook hook);  ///< installed by gld::Device

/// Callback of the device for the start of the shadow-map recording; `sun` is the direction sunlight travels.
using ShadowBeginHook = void (*)(const float* sun);
/// Callback of the device for the end of the opaque world: `projection` is the camera projection matrix.
using ShadowDoneHook = void (*)(const float* projection);
/// Installs the shadow-map callbacks (done by gld::Device).
void set_shadow_hooks(ShadowBeginHook begin, ShadowDoneHook done);  // installed by gld::Device

/// One recorded draw of the opaque pass.
struct ShadowDraw {
    gl::GLuint program;          ///< GL program the draw used
    gl::GLuint vao;              ///< its vertex array
    gl::GLuint vertex_buffer;    ///< vertex buffer on binding 0
    gl::GLuint index_buffer;     ///< element buffer, 0 = none
    gl::GLintptr vertex_offset;  ///< bytes into the vertex buffer
    std::uintptr_t index_offset;  ///< bytes into the index buffer
    std::uint32_t stride;        ///< bytes between vertices
    gl::GLenum mode;             ///< primitive type (GL_* enum)
    gl::GLsizei count;           ///< vertices, or indices when `indexed`
    gl::GLint first;  ///< first vertex of a non-indexed draw
    gl::GLint base_vertex;  ///< value added to each index of an indexed draw
    bool indexed;                ///< glDrawElements rather than glDrawArrays
    bool wide;                   ///< 32-bit indices, else 16-bit
    std::uint32_t ffp_offset;  ///< ring range of block Ffp (Renderer::uniform_offset 0)
    std::uint32_t vs_offset;  ///< ring range of block VsConst (Renderer::uniform_offset 1)
    std::uint32_t draw_offset;  ///< ring range of block FfpDraw (Renderer::uniform_offset 3)
    gl::GLuint texture;  ///< stage 0 texture for an alpha-tested draw, else 0
    gl::GLuint sampler;  ///< its sampler
    gl::GLuint lightmap;  ///< the ground's lightmap texture (BakedShadows' level map), else 0
    gl::GLuint lightmap_sampler;  ///< its sampler
    int lightmap_stage;          ///< texture stage of that lightmap (meaningful when `lightmap` is not 0)
};

/// The GL side of [Graphics] Shadows=2: renders the shadow map from recorded draws and applies it to the
/// scene framebuffer. Owned by gl::Device; every call needs its GL context current.
class ShadowPass {
   public:
    /// The frame the shadows are applied to.
    struct Input {
        gl::GLuint scene_fbo;        ///< the framebuffer the world was drawn into
        int width;  ///< scene framebuffer width, pixels
        int height;  ///< scene framebuffer height, pixels
        int vx;  ///< camera viewport left edge, pixels
        int vy;  ///< camera viewport top edge, pixels (top row = 0)
        int vw;  ///< camera viewport width, pixels
        int vh;  ///< camera viewport height, pixels
        float depth_min;  ///< viewport depth range minimum, 0..1
        float depth_max;  ///< viewport depth range maximum, 0..1
        const float* projection;     ///< Direct3D projection (16 floats, row-major); not owned
        const float* view;           ///< Direct3D view matrix (world -> view, row-major); not owned
        const float* sun;            ///< direction the sunlight travels (world, 3 floats); not owned
        gl::GLuint overlay_depth = 0;   ///< the depth after the billboards drawn before the world (gld::Device::overlay), 0 = none
        gl::GLuint overlay_before = 0;  ///< the depth before them
    };
    /// Renders the shadow map from `draws` and darkens the scene framebuffer; false = nothing done. Leaves
    /// GL state changed (framebuffer, viewport, raster state, program, texture units 0..5, uniform block
    /// bindings): the caller invalidates the renderer's state and rebinds what it needs.
    bool run(Renderer& r, const Input& in, const std::vector<ShadowDraw>& draws);
    /// After run, the same frame and camera (`in` as for run): adds `draws` to the shadow map and darkens
    /// the scene framebuffer by the shadow they add. Leaves GL state changed as run.
    void run_foliage(Renderer& r, const Input& in, const std::vector<ShadowDraw>& draws);
    /// Frees the GL objects (context current).
    void shutdown();
    /// Draws replayed into the shadow map since the last call ([Graphics] PerfLog).
    unsigned take_drawn() { return std::exchange(drawn_total_, 0u); }

   private:
    unsigned drawn_total_ = 0;
    bool ensure(int width, int height, int size);
    // Replays `draws` depth-only into the shadow map (bound, its raster state set).
    unsigned replay(Renderer& r, const std::vector<ShadowDraw>& draws, const float* projection);
    void begin_map();
    void end_map();
    // Darkens the scene framebuffer with `program` (prog_apply_ / prog_foliage_) from the scene's depth.
    void apply(gl::GLuint program, const Input& in);
    bool failed_ = false;
    gl::GLuint vao_ = 0, prog_apply_ = 0, prog_foliage_ = 0;
    gl::GLuint map_tex_ = 0, map_fbo_ = 0, compare_sampler_ = 0;
    gl::GLuint opaque_tex_ = 0;  // run_foliage: copy of the opaque casters' map
    int map_size_ = 0;
    // The last run's light (valid until the next run): view -> light clip space and the apply's parameters.
    bool last_valid_ = false;
    float view_to_light_[16] = {};
    float sun_view_[3] = {};
    double texel_ = 0.0, distance_ = 0.0, depth_half_ = 30000.0;
    float strength_ = 0.0f, softness_ = 0.0f;
    gl::GLuint depth_tex_ = 0, depth_fbo_ = 0;  // copy of the scene's depth
    int w_ = 0, h_ = 0;
    std::uint32_t frame_ = 0;
    unsigned logged_ = 0, foliage_logged_ = 0;
};

}  // namespace joan::render
