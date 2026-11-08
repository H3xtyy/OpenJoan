// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SdateEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SdateEntry {
#include "sdate_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SdateEntry) == 0x33);
static_assert(offsetof(SdateEntry, name) == 0x0);
static_assert(offsetof(SdateEntry, year) == 0x21);
static_assert(offsetof(SdateEntry, month) == 0x25);
static_assert(offsetof(SdateEntry, day) == 0x29);
static_assert(offsetof(SdateEntry, weekday) == 0x2d);
static_assert(offsetof(SdateEntry, id) == 0x31);
#endif

}  // namespace joan::orig
