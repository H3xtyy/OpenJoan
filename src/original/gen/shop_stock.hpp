// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ShopStock class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ShopStock {
#include "shop_stock_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ShopStock) == 0xe);
static_assert(offsetof(ShopStock, id) == 0x0);
static_assert(offsetof(ShopStock, u32_2) == 0x2);
static_assert(offsetof(ShopStock, count) == 0x6);
static_assert(offsetof(ShopStock, u32_a) == 0xa);
#endif

}  // namespace joan::orig
