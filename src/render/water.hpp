// [Graphics] Water=1: modern water, drawn instead of the game's flat blended water planes.
//
// Not part of the original game. Every lake / river / sea is one flat rectangle at a fixed height
// (World::create_water / Cell::create_water build it as a grid mesh); those meshes register here
// (except ice). When the en3d scene's opaque world is complete (after the shadows and the ambient
// occlusion), one full-screen pass intersects each pixel's camera ray with the registered rectangles
// analytically and, where a water surface is in front of the scene, shades it:
//   - refraction: the scene behind it (the lake bed), bent by the waves, fading into the water's colour
//     with the distance the light travels through the water (WaterClarity);
//   - reflection: screen-space, marched through the depth buffer; the sky where the ray leaves the
//     picture; mixed in by the Fresnel term;
//   - animated waves (procedural normals), the sun's glint, soft shores with a little foam, the fog;
//   - and the surface's depth into the depth buffer, so the transparent pass that follows sorts with it.
// The game's own water planes are then not drawn in that scene (Plane::draw asks water_replaced).
#pragma once

#include <cstdint>
#include <vector>

#include "../gl/glapi.hpp"

namespace joan::render {

class Renderer;

/// Live parameters (openjoan.ini [Graphics] Water*; the developer menu changes them).
struct WaterParams {
    int mode = 1;             ///< 0 the original water, 1 modern water
    float clarity = 160.0f;   ///< world units of water the light crosses before it is mostly the water's colour
    float waves = 1.0f;       ///< wave strength (normal map scale)
    float reflection = 1.0f;  ///< reflection strength (Fresnel scale)
    float flow = 0.6f;        ///< rivers' flow speed (the game's texture scroll) scale
};
/// The live WaterParams.
WaterParams& water_params();

/// A water surface: the rectangle x0..x1 / y0..y1 (world) at height z.
struct WaterRect {
    float x0;  ///< x of one edge, world units
    float y0;  ///< y of one edge, world units
    float x1;  ///< x of the opposite edge, world units
    float y1;  ///< y of the opposite edge, world units
    float z;  ///< height of the surface, world units
};

/// World::create_water / Cell::create_water: `plane` is a water mesh; ~Plane: it is gone.
void water_register(const void* plane, const WaterRect& rect);
/// Forgets the surface of `plane` (and its waterfall record).
void water_unregister(const void* plane);
/// The registered surfaces / the waterfalls drawn in the last few scenes.
std::vector<WaterRect> water_rects();
/// Footprints of the waterfalls drawn in the last few scenes (kind 1, seen within 3 scenes).
std::vector<WaterRect> water_falls();
/// Plane::draw: true = `plane` is water that this scene's modern pass already drew.
bool water_replaced(const void* plane);

/// Waterfalls (models textured "WaterFal*", texteff.res Waterfall01 / Waterfall_mod): Plane::draw
/// classifies each plane once (kind: -1 not yet, 0 no, 1 a waterfall; lo / hi its local bounds) and
/// reports a waterfall's world footprint (x0..x1 / y0..y1, z = its lowest point) when it draws it. The
/// water pass churns the water where a recently drawn waterfall meets it.
struct WaterFallInfo {
    std::int8_t kind = -1;        ///< -1 not classified yet, 0 not a waterfall, 1 a waterfall
    float lo[3] = {};  ///< local bounds, minimum corner (x, y, z), model units
    float hi[3] = {};  ///< local bounds, maximum corner (x, y, z), model units
};
/// The waterfall record of `plane`, created when absent.
WaterFallInfo& water_fall_info(const void* plane);
void water_fall_forget(const void* plane);  ///< classified as no waterfall: Plane::draw remembers that itself
/// Records that `plane` was drawn as a waterfall with this world `footprint` in the current scene.
void water_fall_seen(const void* plane, const WaterRect& footprint);

/// Rivers and streams (models textured "river*", texteff BumpEnvWaterRiver: sloped meshes whose texture the
/// game scrolls to make them flow; not flat, so not water rectangles): Plane::draw marks the draws of such a
/// material (water_river_begin / end, `stage` = the texture stage with the river texture, 1-based); the
/// device draws them with the river shader (WaterPass::river_program), which shades the mesh like the lakes
/// - the scene behind it bent and absorbed, reflections, the sun - with the game's own flowing texture as its
/// waves and white water.
/// `blended`: a falling sheet ("WaterFal*", soft edges): drawn with the game's blending and the texture's
/// alpha; a river ("river*") is drawn opaque (the shader shows what lies behind it).
void water_river_begin(int stage, bool blended);
/// Ends the river draw set by water_river_begin.
void water_river_end();
int water_river_stage();  ///< 0 = not a river draw
/// The `blended` flag of the current river draw (see water_river_begin).
bool water_river_blended();
/// The kind of water a texture name makes a material: 0 none, 1 river, 2 falling sheet.
int water_texture_kind(const char* name);
/// SysEn's texture scroll speed is multiplied by this while a river material is set up ([Graphics]
/// WaterFlow: the original streams flow very fast); 1 otherwise.
float& water_flow_scale();

/// Scene::render: a scene starts (nothing replaced yet) / its opaque world is complete (`sun`: the
/// direction sunlight travels, world, or nullptr).
void water_scene_begin();
/// Scene::render: the scene's opaque world is complete; `projection` is the camera projection matrix and `sun` the direction the sunlight travels (world), or nullptr.
void water_scene_done(const float* projection, const float* sun);
/// The water pass of the device: called by water_scene_done with the same arguments; returns true when it drew the water.
using WaterHook = bool (*)(const float* projection, const float* sun);  // true = drawn
/// Installs the water pass hook (done by gld::Device).
void set_water_hook(WaterHook hook);                                      // installed by gld::Device

/// The GL side of [Graphics] Water=1: the full-screen water pass and the river shader. Owned by gl::Device;
/// every call needs its GL context current.
class WaterPass {
   public:
    /// The frame the water is shaded into.
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
        const float* view;           ///< Direct3D view matrix (row-major); not owned
        const float* sun;            ///< direction sunlight travels (world, 3 floats), or nullptr; not owned
        bool fog;                    ///< linear fog is on
        bool range_fog;              ///< fog distance is the range (distance), else the view depth
        float fog_start;  ///< fog start (view depth or distance, see range_fog)
        float fog_end;  ///< fog end (view depth or distance, see range_fog)
        float fog_colour[3];         ///< fog colour, red / green / blue 0..1
        gl::GLuint overlay_depth = 0;   ///< the depth after the billboards drawn before the world (gld::Device::overlay), 0 = none
        gl::GLuint overlay_before = 0;  ///< the depth before them
    };
    /// Shades the water surfaces in the scene framebuffer; false = none on screen / unavailable. Leaves
    /// GL state changed (framebuffer, viewport, raster state, program, vertex array, texture units 0..4):
    /// the caller invalidates the renderer's state and rebinds what it needs.
    bool run(const Input& in, const std::vector<WaterRect>& rects, const std::vector<WaterRect>& falls);
    /// run made GL calls (also when it drew no water: the copies for the rivers).
    bool state_changed() const { return state_changed_; }
    /// Frees the GL objects (context current).
    void shutdown();

    /// The river shader can draw this scene (run made the scene copies after its opaque world).
    bool river_ready() const;
    /// `program` (the draw's own) with the river pixel shader reading the river texture at `stage` (0-based);
    /// 0 = unavailable.
    gl::GLuint river_program(Renderer& r, gl::GLuint program, int stage);
    /// Its uniforms for this scene and the scene copies / environment and the billboard depths on texture units 8..12.
    void river_bind(gl::GLuint program);

   private:
    bool state_changed_ = false;
    // What run worked out for this scene, for the river draws that follow it.
    struct RiverFrame {
        float lin[4], proj[4], vp[4], size[2], view_inv[16], view[16], sun[4], fog[4], fog_colour[3], params[4];
    } river_{};
    std::uint32_t river_scene_ = 0;  // g_scenes of the copies (0 = none)
    gl::GLuint river_overlay_ = 0, river_overlay_before_ = 0;  // that scene's Input::overlay_depth / overlay_before
    std::vector<std::pair<gl::GLuint, std::uint32_t>> river_programs_bound_;  // program -> scene its uniforms are for
    unsigned river_logged_ = 0;
    bool ensure(int width, int height);
    bool failed_ = false;
    gl::GLuint prog_ = 0, prog_env_ = 0, vao_ = 0;
    // The environment panorama: the sky and far scenery seen so far around the camera (reflections that
    // leave the picture); reset when the camera jumps.
    gl::GLuint env_tex_ = 0, env_fbo_ = 0;
    bool env_valid_ = false;
    float env_cam_[3] = {};
    gl::GLuint colour_tex_ = 0, colour_fbo_ = 0, depth_tex_ = 0, depth_fbo_ = 0;
    int w_ = 0, h_ = 0;
    unsigned logged_ = 0;
};

}  // namespace joan::render
