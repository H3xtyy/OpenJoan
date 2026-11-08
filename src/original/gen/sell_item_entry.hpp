// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SellItemEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct SellItemEntry {
#include "sell_item_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(SellItemEntry, id) == 0x0);
static_assert(offsetof(SellItemEntry, name) == 0x2);
static_assert(offsetof(SellItemEntry, rows) == 0x13);
static_assert(offsetof(SellItemEntry, value) == 0x17);
static_assert(offsetof(SellItemEntry, type) == 0x1b);
#endif

}  // namespace joan::orig
