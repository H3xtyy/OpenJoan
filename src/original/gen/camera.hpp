// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Camera class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "cam_params.hpp"
#include "../colour.hpp"

namespace joan::orig {

struct Bone;
struct Fog;
struct Reorder;
struct Scene;
struct SceneS;
struct Surface;

#pragma pack(push, 1)
struct Camera {
#include "camera_fields.inc"
#include "camera_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Camera) == 0x4ca);
static_assert(offsetof(Camera, bone_) == 0x0);
static_assert(offsetof(Camera, scene) == 0x4);
static_assert(offsetof(Camera, target) == 0x8);
static_assert(offsetof(Camera, depth_target) == 0xc);
static_assert(offsetof(Camera, view_x1) == 0x10);
static_assert(offsetof(Camera, view_y1) == 0x14);
static_assert(offsetof(Camera, view_x2) == 0x18);
static_assert(offsetof(Camera, view_y2) == 0x1c);
static_assert(offsetof(Camera, view_width) == 0x20);
static_assert(offsetof(Camera, view_height) == 0x24);
static_assert(offsetof(Camera, u32_28) == 0x28);
static_assert(offsetof(Camera, angle_min) == 0x2c);
static_assert(offsetof(Camera, angle_max) == 0x34);
static_assert(offsetof(Camera, angle_extra) == 0x3c);
static_assert(offsetof(Camera, near_plane) == 0x44);
static_assert(offsetof(Camera, far_plane) == 0x48);
static_assert(offsetof(Camera, far_perspective) == 0x4c);
static_assert(offsetof(Camera, far_ortho) == 0x50);
static_assert(offsetof(Camera, fog) == 0x54);
static_assert(offsetof(Camera, fog_kind) == 0x58);
static_assert(offsetof(Camera, light_) == 0x5c);
static_assert(offsetof(Camera, fog_start) == 0x60);
static_assert(offsetof(Camera, fog_end) == 0x64);
static_assert(offsetof(Camera, fog_density) == 0x68);
static_assert(offsetof(Camera, fog_height_a) == 0x6c);
static_assert(offsetof(Camera, fog_height_b) == 0x70);
static_assert(offsetof(Camera, fog_colour) == 0x74);
static_assert(offsetof(Camera, prev_params) == 0x78);
static_assert(offsetof(Camera, params) == 0xa0);
static_assert(offsetof(Camera, near_threshold) == 0xc8);
static_assert(offsetof(Camera, u8_cc) == 0xcc);
static_assert(offsetof(Camera, clear_target) == 0xce);
static_assert(offsetof(Camera, clear_depth) == 0xcf);
static_assert(offsetof(Camera, clear_colour) == 0xd0);
static_assert(offsetof(Camera, i32_d4) == 0xd4);
static_assert(offsetof(Camera, multiply_blend) == 0xd8);
static_assert(offsetof(Camera, reorder) == 0xdc);
static_assert(offsetof(Camera, use_portals) == 0xe0);
static_assert(offsetof(Camera, portal_sector) == 0xe1);
static_assert(offsetof(Camera, mirror) == 0xe5);
static_assert(offsetof(Camera, mirror_z) == 0xe6);
static_assert(offsetof(Camera, matrices_valid) == 0xea);
static_assert(offsetof(Camera, dirty_) == 0xeb);
static_assert(offsetof(Camera, inv_bone_matrix) == 0xee);
static_assert(offsetof(Camera, projection) == 0x12e);
static_assert(offsetof(Camera, view_projection) == 0x16e);
static_assert(offsetof(Camera, matrix_1ae) == 0x1ae);
static_assert(offsetof(Camera, view_) == 0x1ee);
static_assert(offsetof(Camera, matrix_22e_) == 0x22e);
static_assert(offsetof(Camera, d3d_projection) == 0x26e);
static_assert(offsetof(Camera, viewport_x) == 0x2ae);
static_assert(offsetof(Camera, viewport_y) == 0x2b2);
static_assert(offsetof(Camera, viewport_width) == 0x2b6);
static_assert(offsetof(Camera, viewport_height) == 0x2ba);
static_assert(offsetof(Camera, viewport_min_z) == 0x2be);
static_assert(offsetof(Camera, viewport_max_z) == 0x2c2);
static_assert(offsetof(Camera, frustum_corners) == 0x2c6);
static_assert(offsetof(Camera, frustum_planes) == 0x326);
static_assert(offsetof(Camera, frustum_bounds) == 0x386);
static_assert(offsetof(Camera, extra_planes) == 0x39e);
static_assert(offsetof(Camera, extra_plane_count) == 0x3fe);
static_assert(offsetof(Camera, clip_planes_world) == 0x402);
static_assert(offsetof(Camera, clip_planes_view) == 0x462);
static_assert(offsetof(Camera, clip_plane_mask) == 0x4c2);
static_assert(offsetof(Camera, clip_planes_loaded) == 0x4c6);
#endif

}  // namespace joan::orig
