// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's DropItem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct DropItem {
#include "drop_item_fields.inc"
#include "drop_item_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(DropItem) == 0x2b);
static_assert(offsetof(DropItem, id) == 0x0);
static_assert(offsetof(DropItem, name) == 0x2);
static_assert(offsetof(DropItem, drops) == 0x23);
static_assert(offsetof(DropItem, total_weight) == 0x27);
#endif

}  // namespace joan::orig
