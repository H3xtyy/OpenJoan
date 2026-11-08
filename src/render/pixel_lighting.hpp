// [Graphics] PixelLighting=1: the fixed-function pipeline's lit draws with vertex normals have their lights (the
// same Direct3D light model, gl/shaders.cpp) evaluated per pixel instead of per vertex - no faceted shading on
// low-poly models. Not part of the original game, which lights its models per vertex.
#pragma once

namespace joan::render {

/// Live parameters (openjoan.ini [Graphics] PixelLighting; the developer menu changes them).
struct SurfaceParams {
    bool pixel_lighting = true;  ///< [Graphics] PixelLighting: evaluate lights per pixel instead of per vertex
};
/// The live SurfaceParams.
SurfaceParams& surface_params();

}  // namespace joan::render
