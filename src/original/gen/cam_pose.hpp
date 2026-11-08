// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamPose class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CamPose {
#include "cam_pose_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CamPose) == 0x28);
static_assert(offsetof(CamPose, u32_0) == 0x0);
static_assert(offsetof(CamPose, eye) == 0x4);
static_assert(offsetof(CamPose, target) == 0x10);
static_assert(offsetof(CamPose, u32_1c) == 0x1c);
static_assert(offsetof(CamPose, u32_20) == 0x20);
static_assert(offsetof(CamPose, u32_24) == 0x24);
#endif

}  // namespace joan::orig
