// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's NewsLog class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "news_record.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct NewsLog {
#include "news_log_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(NewsLog, type_count) == 0xa2);
static_assert(offsetof(NewsLog, type_id) == 0xa4);
static_assert(offsetof(NewsLog, type_min) == 0xb4);
static_assert(offsetof(NewsLog, type_icon) == 0xbc);
static_assert(offsetof(NewsLog, type_posted) == 0xcc);
static_assert(offsetof(NewsLog, level) == 0xf0);
static_assert(offsetof(NewsLog, count) == 0x112);
static_assert(offsetof(NewsLog, records) == 0x114);
static_assert(offsetof(NewsLog, last_time) == 0x2f06);
#endif

}  // namespace joan::orig
