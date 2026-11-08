// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ItemRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "string_pool.hpp"

namespace joan::orig {

struct ArrI32;
struct ItemClass;
struct ItemDef;
struct ItemType;

#pragma pack(push, 1)
struct ItemRes {
#include "item_res_fields.inc"
#include "item_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ItemRes) == 0x25);
static_assert(offsetof(ItemRes, item_count) == 0x0);
static_assert(offsetof(ItemRes, type_count) == 0x4);
static_assert(offsetof(ItemRes, class_count) == 0x8);
static_assert(offsetof(ItemRes, types) == 0xc);
static_assert(offsetof(ItemRes, classes) == 0x10);
static_assert(offsetof(ItemRes, items) == 0x14);
static_assert(offsetof(ItemRes, sorted_index) == 0x18);
static_assert(offsetof(ItemRes, strings) == 0x1c);
static_assert(offsetof(ItemRes, loaded) == 0x24);
#endif

}  // namespace joan::orig
