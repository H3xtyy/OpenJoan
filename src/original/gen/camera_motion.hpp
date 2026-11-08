// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CameraMotion class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "cam_pose.hpp"

namespace joan::orig {

struct CamTrackNode;

#pragma pack(push, 1)
struct CameraMotion {
#include "camera_motion_fields.inc"
#include "camera_motion_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CameraMotion, from) == 0x0);
static_assert(offsetof(CameraMotion, to) == 0x28);
static_assert(offsetof(CameraMotion, current_) == 0x50);
static_assert(offsetof(CameraMotion, time_mode) == 0x78);
static_assert(offsetof(CameraMotion, duration_ms) == 0x7c);
static_assert(offsetof(CameraMotion, ease_in) == 0x80);
static_assert(offsetof(CameraMotion, ease_out) == 0x84);
static_assert(offsetof(CameraMotion, blend_mode) == 0x88);
static_assert(offsetof(CameraMotion, blend_callback) == 0x8c);
static_assert(offsetof(CameraMotion, t) == 0x98);
static_assert(offsetof(CameraMotion, start_ms) == 0x9c);
static_assert(offsetof(CameraMotion, skip_collision) == 0xa4);
static_assert(offsetof(CameraMotion, collide_) == 0xa8);
static_assert(offsetof(CameraMotion, tracked) == 0xc0);
static_assert(offsetof(CameraMotion, tracked_count) == 0xc4);
#endif

}  // namespace joan::orig
