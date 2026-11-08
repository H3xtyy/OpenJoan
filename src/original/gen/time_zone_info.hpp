// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TimeZoneInfo class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "system_time.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TimeZoneInfo {
#include "time_zone_info_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TimeZoneInfo) == 0xac);
static_assert(offsetof(TimeZoneInfo, bias) == 0x0);
static_assert(offsetof(TimeZoneInfo, standard_name) == 0x4);
static_assert(offsetof(TimeZoneInfo, standard_date) == 0x44);
static_assert(offsetof(TimeZoneInfo, standard_bias) == 0x54);
static_assert(offsetof(TimeZoneInfo, daylight_name) == 0x58);
static_assert(offsetof(TimeZoneInfo, daylight_date) == 0x98);
static_assert(offsetof(TimeZoneInfo, daylight_bias) == 0xa8);
#endif

}  // namespace joan::orig
