// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ItemStack class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ItemStack {
#include "item_stack_fields.inc"
#include "item_stack_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ItemStack) == 0x4);
static_assert(offsetof(ItemStack, item) == 0x0);
static_assert(offsetof(ItemStack, count) == 0x2);
#endif

}  // namespace joan::orig
