// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Building class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "facility.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Building : Facility {
#include "building_fields.inc"
#include "building_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Building) == 0x140);
static_assert(offsetof(Building, unit_id) == 0x79);
static_assert(offsetof(Building, unit_kind) == 0x7b);
static_assert(offsetof(Building, i16_7d) == 0x7d);
static_assert(offsetof(Building, unit_level) == 0x7f);
static_assert(offsetof(Building, text) == 0x81);
static_assert(offsetof(Building, order_arg) == 0x101);
static_assert(offsetof(Building, order_b) == 0x105);
static_assert(offsetof(Building, order_c) == 0x106);
static_assert(offsetof(Building, order_d) == 0x10a);
static_assert(offsetof(Building, order_e) == 0x10e);
static_assert(offsetof(Building, order_f) == 0x112);
static_assert(offsetof(Building, u32_116) == 0x116);
static_assert(offsetof(Building, delay) == 0x11e);
static_assert(offsetof(Building, queue_n) == 0x122);
static_assert(offsetof(Building, queue_kind) == 0x124);
static_assert(offsetof(Building, queue_count) == 0x12c);
static_assert(offsetof(Building, queue_d) == 0x134);
static_assert(offsetof(Building, target) == 0x13c);
#endif

}  // namespace joan::orig
