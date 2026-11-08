// [Graphics] GpuShadowVolumes (render/shadow_volumes.hpp): the game side of the GPU-made stencil volumes.
#pragma once

#include "../render/shadow_volumes.hpp"

namespace joan::en3d {

/// What ShadowPlane::build 0x005e3040 would make a volume of (en3d/plane_sh.cpp): `plane`'s geometry, the
/// light direction in its model space, its world matrix (16 floats) and the extrusion length for `light`
/// (`extent` = the model's sphere radius + z, as SceneS::shadow_volumes passes). False = no volume.
bool gpu_shadow_volume(void* plane, void* light, float* world, float extent, render::VolumeSource& src, float* dir, float& length);

}  // namespace joan::en3d
