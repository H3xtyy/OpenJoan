// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's DropItemRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DropItem;

#pragma pack(push, 1)
struct DropItemRes {
#include "drop_item_res_fields.inc"
#include "drop_item_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(DropItemRes) == 0x9);
static_assert(offsetof(DropItemRes, count) == 0x0);
static_assert(offsetof(DropItemRes, rows) == 0x4);
static_assert(offsetof(DropItemRes, loaded) == 0x8);
#endif

}  // namespace joan::orig
