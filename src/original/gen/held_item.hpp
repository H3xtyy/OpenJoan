// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's HeldItem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "item_stack.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct HeldItem : ItemStack {
#include "held_item_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(HeldItem) == 0x12);
static_assert(offsetof(HeldItem, u16_4) == 0x4);
static_assert(offsetof(HeldItem, uses) == 0x6);
static_assert(offsetof(HeldItem, max_uses) == 0xa);
static_assert(offsetof(HeldItem, u32_e) == 0xe);
#endif

}  // namespace joan::orig
