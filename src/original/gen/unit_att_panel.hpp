// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitAttPanel class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BmpRes;

#pragma pack(push, 1)
struct UnitAttPanel {
#include "unit_att_panel_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(UnitAttPanel, icons) == 0x8);
static_assert(offsetof(UnitAttPanel, u32_10) == 0x10);
static_assert(offsetof(UnitAttPanel, u32_1d) == 0x1d);
static_assert(offsetof(UnitAttPanel, u8_21) == 0x21);
static_assert(offsetof(UnitAttPanel, u32_6af) == 0x6af);
#endif

}  // namespace joan::orig
