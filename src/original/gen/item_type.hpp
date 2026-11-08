// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ItemType class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ItemType {
#include "item_type_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ItemType) == 0x25);
static_assert(offsetof(ItemType, class_id) == 0x0);
static_assert(offsetof(ItemType, id) == 0x2);
static_assert(offsetof(ItemType, name) == 0x4);
#endif

}  // namespace joan::orig
