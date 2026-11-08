// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WorldItem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "held_item.hpp"

namespace joan::orig {

struct Sector;
struct World;
struct WorldCell;

#pragma pack(push, 1)
struct WorldItem {
#include "world_item_fields.inc"
#include "world_item_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(WorldItem) == 0x52);
static_assert(offsetof(WorldItem, stack) == 0x0);
static_assert(offsetof(WorldItem, pos) == 0x12);
static_assert(offsetof(WorldItem, value) == 0x2e);
static_assert(offsetof(WorldItem, side) == 0x32);
static_assert(offsetof(WorldItem, object_id) == 0x34);
static_assert(offsetof(WorldItem, world) == 0x36);
static_assert(offsetof(WorldItem, cell) == 0x3a);
static_assert(offsetof(WorldItem, cell_prev) == 0x3e);
static_assert(offsetof(WorldItem, cell_next) == 0x42);
static_assert(offsetof(WorldItem, sector) == 0x46);
static_assert(offsetof(WorldItem, sector_prev) == 0x4a);
static_assert(offsetof(WorldItem, sector_next) == 0x4e);
#endif

}  // namespace joan::orig
