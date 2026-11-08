// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ListBoxColumn class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ListBoxColumn {
#include "list_box_column_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ListBoxColumn) == 0x13);
static_assert(offsetof(ListBoxColumn, name) == 0x0);
static_assert(offsetof(ListBoxColumn, colour) == 0x9);
static_assert(offsetof(ListBoxColumn, u16_d) == 0xd);
static_assert(offsetof(ListBoxColumn, u16_f) == 0xf);
static_assert(offsetof(ListBoxColumn, u16_11) == 0x11);
#endif

}  // namespace joan::orig
