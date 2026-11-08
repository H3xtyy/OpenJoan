// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ItemClass class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ItemClass {
#include "item_class_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ItemClass) == 0x2c);
static_assert(offsetof(ItemClass, id) == 0x0);
static_assert(offsetof(ItemClass, name) == 0x2);
static_assert(offsetof(ItemClass, desc) == 0xb);
#endif

}  // namespace joan::orig
