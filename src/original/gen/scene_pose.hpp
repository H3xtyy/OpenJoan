// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScenePose class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScenePose {
#include "scene_pose_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScenePose) == 0x20);
static_assert(offsetof(ScenePose, kind) == 0x0);
static_assert(offsetof(ScenePose, pos) == 0xc);
static_assert(offsetof(ScenePose, f32_18) == 0x18);
static_assert(offsetof(ScenePose, f32_1c) == 0x1c);
#endif

}  // namespace joan::orig
