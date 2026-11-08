// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightSet class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LightSet {
#include "light_set_fields.inc"
#include "light_set_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LightSet) == 0x8d);
static_assert(offsetof(LightSet, slot) == 0x0);
static_assert(offsetof(LightSet, frame) == 0x4);
static_assert(offsetof(LightSet, light_count) == 0x8);
static_assert(offsetof(LightSet, light_ids) == 0xc);
static_assert(offsetof(LightSet, lit) == 0x8c);
#endif

}  // namespace joan::orig
