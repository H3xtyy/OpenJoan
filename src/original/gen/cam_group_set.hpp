// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamGroupSet class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PtrSlot;
struct Scene;

#pragma pack(push, 1)
struct CamGroupSet {
#include "cam_group_set_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CamGroupSet, scene) == 0x6e);
static_assert(offsetof(CamGroupSet, group_count) == 0x86);
static_assert(offsetof(CamGroupSet, groups) == 0x8e);
#endif

}  // namespace joan::orig
