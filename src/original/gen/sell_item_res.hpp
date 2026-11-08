// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SellItemRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct SellItemEntry;

#pragma pack(push, 1)
struct SellItemRes {
#include "sell_item_res_fields.inc"
#include "sell_item_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(SellItemRes, count) == 0x0);
static_assert(offsetof(SellItemRes, entries) == 0x4);
#endif

}  // namespace joan::orig
