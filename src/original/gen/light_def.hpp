// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightDef class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LightDef {
#include "light_def_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LightDef) == 0x1a);
static_assert(offsetof(LightDef, mode) == 0x0);
static_assert(offsetof(LightDef, enabled) == 0x1);
static_assert(offsetof(LightDef, colour) == 0x2);
static_assert(offsetof(LightDef, angle_b) == 0x12);
static_assert(offsetof(LightDef, angle_a) == 0x16);
#endif

}  // namespace joan::orig
