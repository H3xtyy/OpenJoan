// [Graphics] AmbientOcclusion: screen-space ambient occlusion (SSAO) for the 3D world.
//
// The game has no ambient occlusion. After the opaque part of the world is drawn
// (Scene::render, between pass 1 and the transparent pass 2; en3d/scene.cpp),
// the OpenGL device (Renderer=opengl) darkens creases, contact areas and corners:
//   1. the depth buffer is copied into a texture (resolving MSAA),
//   2. half resolution: depth -> view-space distance (the camera's D3D projection),
//   3. half resolution: occlusion from 12 samples in a disc of `radius` world units
//      around each point, against the normal rebuilt from the depth,
//   4. half resolution: depth-aware blur (horizontal + vertical),
//   5. full resolution: depth-aware upsampling, multiplied onto the picture's colour.
// Transparent geometry, effects, glows and the HUD are drawn afterwards and are not
// darkened. Units: the game's world units (Joan is ~79 units tall).
//
// Foliage in the transparent pass: the game decides per material whether it is drawn in the
// opaque pass 1 or the transparent pass 2 (Material +0x40), from the texture's alpha when the
// model is loaded: .SMO alpha materials go to pass 1 only when the texture has 1-bit alpha
// (DXT1 / A1R5G5B5, Texture::alpha_bits == 1), the older plane / .AM1 readers put every
// texture with alpha into pass 2, and a texture still streaming in also gives pass 2. So the
// same kind of tree or bush is in the opaque depth on one map and missing from it on another.
// Phase 2 (transparent_scene_done) adds them: while the transparent pass is drawn, the scene
// framebuffer has a second draw buffer that only the alpha-tested, depth-writing draws (alpha
// reference >= 0x40: foliage, fences; not water or fading units, which use reference 4, nor
// effects without depth writes) reach, keeping their nearest depth (the draws are made once;
// see ao_pass.hpp). After the pass the occlusion of the opaque + cut-out depth is applied to
// the cut-out pixels, plus the extra occlusion the cut-outs cast on the opaque world.
#pragma once

#include <cstdint>

namespace joan::render {

/// Live parameters (initialised from openjoan.ini; the developer menu changes them).
struct AoParams {
    int mode = 0;               ///< 0 off, 1 on, 2 show the occlusion only (grey picture, for tuning)
    float radius = 40.0f;       ///< sample disc radius, world units
    float strength = 1.2f;      ///< occlusion multiplier, 0..4
    float fade_start = 2500.0f; ///< world units from the camera: full effect closer than this
    float fade_end = 5000.0f;   ///< world units from the camera: no effect beyond this
};
/// The live AoParams (one instance for the process).
AoParams& ao_params();

/// Called by the en3d scene draw when the opaque world is complete. `projection` is the
/// camera's Direct3D projection matrix (16 floats, row-major, row vectors) the scene was
/// drawn with. Does nothing unless Renderer=opengl and AmbientOcclusion is on, and only for
/// a perspective camera drawing into the back buffer.
void opaque_scene_done(const float* projection);

/// Called by the en3d scene draw when the world's transparent pass (pass 2 of the sectors and
/// the scene's actors) is drawn, before the camera-following planes, end-of-frame work and
/// markers. Phase 2 of the camera's last opaque_scene_done (see above); nothing otherwise.
void transparent_scene_done(const float* projection);

/// Installed by gl::Device (Renderer=opengl); nullptr otherwise.
using OpaqueSceneHook = void (*)(const float* projection);
/// Installs the callback run by opaque_scene_done (done by gl::Device).
void set_opaque_scene_hook(OpaqueSceneHook hook);
/// Callback of the device for transparent_scene_done; `projection` is the camera projection matrix.
using TransparentSceneHook = void (*)(const float* projection);
/// Installs the callback run by transparent_scene_done (done by gl::Device).
void set_transparent_scene_hook(TransparentSceneHook hook);

}  // namespace joan::render
