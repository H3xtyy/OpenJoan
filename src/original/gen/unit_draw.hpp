// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitDraw class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Billboard;

#pragma pack(push, 1)
struct UnitDraw {
#include "unit_draw_fields.inc"
#include "unit_draw_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(UnitDraw, unit_id) == 0x0);
static_assert(offsetof(UnitDraw, i32_2) == 0x2);
static_assert(offsetof(UnitDraw, label_text) == 0x6);
static_assert(offsetof(UnitDraw, i16_46) == 0x46);
static_assert(offsetof(UnitDraw, plate_attached) == 0x48);
static_assert(offsetof(UnitDraw, label_attached) == 0x49);
static_assert(offsetof(UnitDraw, label_) == 0x4a);
static_assert(offsetof(UnitDraw, plate_) == 0x4e);
static_assert(offsetof(UnitDraw, u8_52) == 0x52);
#endif

}  // namespace joan::orig
