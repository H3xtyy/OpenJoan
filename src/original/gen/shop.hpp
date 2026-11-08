// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Shop class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "facility.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct Shop : Facility {
#include "shop_fields.inc"
#include "shop_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Shop) == 0x8e);
static_assert(offsetof(Shop, stock) == 0x79);
static_assert(offsetof(Shop, stock_kind) == 0x7d);
static_assert(offsetof(Shop, i32_81) == 0x81);
static_assert(offsetof(Shop, restock_timer) == 0x85);
static_assert(offsetof(Shop, shop_id) == 0x89);
static_assert(offsetof(Shop, fixed_stock) == 0x8d);
#endif

}  // namespace joan::orig
