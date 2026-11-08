// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightSlot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "light.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LightSlot {
#include "light_slot_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LightSlot) == 0xd6);
static_assert(offsetof(LightSlot, slot) == 0x0);
static_assert(offsetof(LightSlot, frame) == 0x4);
static_assert(offsetof(LightSlot, light) == 0x8);
#endif

}  // namespace joan::orig
