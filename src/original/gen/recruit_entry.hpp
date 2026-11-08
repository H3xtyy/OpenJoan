// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RecruitEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct RecruitEntry {
#include "recruit_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(RecruitEntry) == 0xb4);
static_assert(offsetof(RecruitEntry, id) == 0x0);
static_assert(offsetof(RecruitEntry, name) == 0x2);
static_assert(offsetof(RecruitEntry, rows) == 0x23);
static_assert(offsetof(RecruitEntry, value) == 0x27);
static_assert(offsetof(RecruitEntry, location) == 0x2b);
static_assert(offsetof(RecruitEntry, level) == 0xac);
static_assert(offsetof(RecruitEntry, population) == 0xb0);
#endif

}  // namespace joan::orig
