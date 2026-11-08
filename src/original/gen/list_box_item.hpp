// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ListBoxItem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ListBoxItem {
#include "list_box_item_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ListBoxItem) == 0x195);
static_assert(offsetof(ListBoxItem, text) == 0x0);
static_assert(offsetof(ListBoxItem, selected) == 0x191);
#endif

}  // namespace joan::orig
