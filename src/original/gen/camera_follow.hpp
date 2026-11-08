// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CameraFollow class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Camera;
struct CameraMotion;
struct Light;

#pragma pack(push, 1)
struct CameraFollow {
#include "camera_follow_fields.inc"
#include "camera_follow_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CameraFollow, active) == 0x0);
static_assert(offsetof(CameraFollow, motions) == 0x4);
static_assert(offsetof(CameraFollow, i32_8) == 0x8);
static_assert(offsetof(CameraFollow, camera) == 0xc);
static_assert(offsetof(CameraFollow, light) == 0x10);
static_assert(offsetof(CameraFollow, far_limit) == 0x14);
static_assert(offsetof(CameraFollow, height) == 0x18);
static_assert(offsetof(CameraFollow, target_distance) == 0x1c);
static_assert(offsetof(CameraFollow, u8_20) == 0x20);
static_assert(offsetof(CameraFollow, u8_21) == 0x21);
static_assert(offsetof(CameraFollow, u8_22) == 0x22);
static_assert(offsetof(CameraFollow, key_move) == 0x23);
static_assert(offsetof(CameraFollow, follow_target) == 0x2f);
static_assert(offsetof(CameraFollow, mode) == 0x33);
#endif

}  // namespace joan::orig
