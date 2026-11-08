// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SystemTime class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SystemTime {
#include "system_time_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SystemTime) == 0x10);
static_assert(offsetof(SystemTime, year) == 0x0);
static_assert(offsetof(SystemTime, month) == 0x2);
static_assert(offsetof(SystemTime, day_of_week) == 0x4);
static_assert(offsetof(SystemTime, day) == 0x6);
static_assert(offsetof(SystemTime, hour) == 0x8);
static_assert(offsetof(SystemTime, minute) == 0xa);
static_assert(offsetof(SystemTime, second) == 0xc);
static_assert(offsetof(SystemTime, milliseconds) == 0xe);
#endif

}  // namespace joan::orig
