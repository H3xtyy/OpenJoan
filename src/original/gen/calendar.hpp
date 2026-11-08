// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Calendar class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Calendar {
#include "calendar_fields.inc"
#include "calendar_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Calendar) == 0x50);
static_assert(offsetof(Calendar, start_julian) == 0x0);
static_assert(offsetof(Calendar, start_year) == 0x4);
static_assert(offsetof(Calendar, day) == 0x8);
static_assert(offsetof(Calendar, time_prev) == 0xc);
static_assert(offsetof(Calendar, day_of_month) == 0x10);
static_assert(offsetof(Calendar, month) == 0x14);
static_assert(offsetof(Calendar, year) == 0x18);
static_assert(offsetof(Calendar, time_of_day) == 0x1c);
static_assert(offsetof(Calendar, minutes_per_day) == 0x20);
static_assert(offsetof(Calendar, cycle_day30) == 0x24);
static_assert(offsetof(Calendar, cycle_month30) == 0x28);
static_assert(offsetof(Calendar, cycle_year30) == 0x2c);
static_assert(offsetof(Calendar, cycle_year10) == 0x30);
static_assert(offsetof(Calendar, cycle_month6) == 0x34);
static_assert(offsetof(Calendar, cycle_month12) == 0x38);
static_assert(offsetof(Calendar, start_time) == 0x3c);
static_assert(offsetof(Calendar, u32_40) == 0x40);
static_assert(offsetof(Calendar, weekday) == 0x44);
static_assert(offsetof(Calendar, day_of_year) == 0x48);
static_assert(offsetof(Calendar, years) == 0x4c);
#endif

}  // namespace joan::orig
