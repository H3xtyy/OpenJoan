// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's NewsRecord class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct NewsRecord {
#include "news_record_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(NewsRecord) == 0x93);
static_assert(offsetof(NewsRecord, id) == 0x0);
static_assert(offsetof(NewsRecord, read) == 0x2);
static_assert(offsetof(NewsRecord, time) == 0x3);
static_assert(offsetof(NewsRecord, start_time) == 0x7);
static_assert(offsetof(NewsRecord, icon) == 0xb);
static_assert(offsetof(NewsRecord, text1) == 0xd);
static_assert(offsetof(NewsRecord, text2) == 0x4f);
static_assert(offsetof(NewsRecord, weight) == 0x91);
#endif

}  // namespace joan::orig
