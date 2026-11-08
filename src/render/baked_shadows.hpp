// [Graphics] Shadows=2 with BakedShadows < 100: the shadows baked into the levels' ground lightmaps
// (under trees, buildings, walls) are toned down, so the shadow map's shadows replace them instead of
// doubling them.
//
// Not part of the original game. The ground's lightmap is the texture of a stage that modulates
// with the second texture coordinates, clamped (SysEn texture modes 18 and 19, Cell::build_ground's
// "<cell>_<sector>" / "lightmap_default" textures). The level is lit by many such lightmaps side by side,
// so they are worked on together, in world space: each frame the ground draws the shadow map recorded are
// drawn again from straight above into the level map (4 x ShadowDistance around the camera, each texel the
// lightmap colour of the ground there). Its lit level - closed over kRadius world units (maximum, then
// minimum: dark patches narrower than the window filled, broad shading kept) and blurred as wide - is
// where the ground would be without the baked shadows; the ground's pixel shaders lift their lightmap
// texel towards it by (1 - BakedShadows) (gld::kPixelLift, next frame). Neighbouring lightmaps read the
// same map, so they still meet without a seam.
#pragma once

#include <cstdint>
#include <vector>

#include "../gl/glapi.hpp"

namespace joan::render {

class Renderer;
struct ShadowDraw;

/// Builds the level map of the ground lightmaps and its lit level for [Graphics] BakedShadows.
/// Owned by the shadow-map code of gl::Device; GL calls need its context current.
class BakedShadows {
   public:
    /// Draws the level map from the recorded ground draws (`view` / `projection`: the camera they were drawn
    /// with, Direct3D row-major) and works out its lit level; `keep` = BakedShadows (0..1). Leaves GL state
    /// changed (framebuffer, viewport, raster state, program, vertex array, texture unit 0, uniform blocks
    /// 0 / 1): the caller invalidates the renderer's state and rebinds what it needs.
    void build(Renderer& r, const std::vector<ShadowDraw>& draws, const float* view, const float* projection,
               float distance, float keep);
    /// The lit level (gld::kLiftUnit) and its uniform block (binding 2), made on first use; 0 = unavailable.
    gl::GLuint texture();
    /// No valid map (the ground draws their lightmaps unchanged).
    void invalidate();
    /// Frees the GL objects (context current).
    void shutdown();

   private:
    bool ensure();
    void write_block(const float* lift, const float* params);
    bool failed_ = false;
    gl::GLuint prog_filter_ = 0, vao_ = 0, ubo_ = 0;
    gl::GLuint map_tex_ = 0, map_depth_ = 0, map_fbo_ = 0;  // the level map (kMapSize)
    gl::GLuint small_[3] = {};                               // kSmallSize: ping-pong, then the lit level
    gl::GLuint fbo_ = 0;
    bool valid_ = false;
    std::uint32_t frame_ = 0;
    unsigned logged_ = 0;
};

}  // namespace joan::render
