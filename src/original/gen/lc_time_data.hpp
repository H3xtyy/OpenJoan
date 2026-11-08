// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LcTimeData class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LcTimeData {
#include "lc_time_data_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LcTimeData) == 0xac);
static_assert(offsetof(LcTimeData, wday_abbr) == 0x0);
static_assert(offsetof(LcTimeData, wday) == 0x1c);
static_assert(offsetof(LcTimeData, month_abbr) == 0x38);
static_assert(offsetof(LcTimeData, month) == 0x68);
static_assert(offsetof(LcTimeData, ampm) == 0x98);
static_assert(offsetof(LcTimeData, ww_sdatefmt) == 0xa0);
static_assert(offsetof(LcTimeData, ww_ldatefmt) == 0xa4);
static_assert(offsetof(LcTimeData, ww_timefmt) == 0xa8);
#endif

}  // namespace joan::orig
