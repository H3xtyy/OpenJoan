// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RecruitRowItem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct RecruitRowItem {
#include "recruit_row_item_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(RecruitRowItem) == 0xa);
static_assert(offsetof(RecruitRowItem, kind) == 0x0);
static_assert(offsetof(RecruitRowItem, rate) == 0x2);
static_assert(offsetof(RecruitRowItem, count) == 0x6);
#endif

}  // namespace joan::orig
