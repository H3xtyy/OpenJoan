// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's InvItem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct InvItem {
#include "inv_item_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(InvItem) == 0x18);
static_assert(offsetof(InvItem, type) == 0x0);
static_assert(offsetof(InvItem, count) == 0x2);
static_assert(offsetof(InvItem, level) == 0x4);
static_assert(offsetof(InvItem, u32_6) == 0x6);
static_assert(offsetof(InvItem, u32_a) == 0xa);
static_assert(offsetof(InvItem, u32_e) == 0xe);
static_assert(offsetof(InvItem, w) == 0x12);
static_assert(offsetof(InvItem, h) == 0x13);
static_assert(offsetof(InvItem, u8_14) == 0x14);
static_assert(offsetof(InvItem, x) == 0x15);
static_assert(offsetof(InvItem, y) == 0x16);
static_assert(offsetof(InvItem, placed) == 0x17);
#endif

}  // namespace joan::orig
