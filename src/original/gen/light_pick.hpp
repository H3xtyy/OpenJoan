// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightPick class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "v4.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LightPick {
#include "light_pick_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LightPick) == 0x30);
static_assert(offsetof(LightPick, count) == 0x0);
static_assert(offsetof(LightPick, u32_4) == 0x4);
static_assert(offsetof(LightPick, count2) == 0x8);
static_assert(offsetof(LightPick, best) == 0xc);
static_assert(offsetof(LightPick, second) == 0x10);
static_assert(offsetof(LightPick, direction) == 0x14);
static_assert(offsetof(LightPick, colour) == 0x20);
#endif

}  // namespace joan::orig
