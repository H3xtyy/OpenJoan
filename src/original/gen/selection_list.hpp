// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SelectionList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SelectionList : DynArrayB {
#include "selection_list_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(SelectionList, u16_2e) == 0x2e);
static_assert(offsetof(SelectionList, bytes_30) == 0x30);
static_assert(offsetof(SelectionList, bytes_38) == 0x38);
#endif

}  // namespace joan::orig
