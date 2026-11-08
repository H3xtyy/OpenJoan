// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitAttrScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitAttrScreen {
#include "unit_attr_screen_fields.inc"
#include "unit_attr_screen_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(UnitAttrScreen, u32_c) == 0xc);
static_assert(offsetof(UnitAttrScreen, u32_14) == 0x14);
static_assert(offsetof(UnitAttrScreen, u8_6ae) == 0x6ae);
static_assert(offsetof(UnitAttrScreen, level) == 0x6af);
#endif

}  // namespace joan::orig
