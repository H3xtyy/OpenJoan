// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WorldSector class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct SceneS;
struct WorldItem;

#pragma pack(push, 1)
struct WorldSector {
#include "world_sector_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(WorldSector, box) == 0x8);
static_assert(offsetof(WorldSector, grid_x) == 0x20);
static_assert(offsetof(WorldSector, grid_y) == 0x22);
static_assert(offsetof(WorldSector, owner) == 0x24);
static_assert(offsetof(WorldSector, level) == 0x2c);
static_assert(offsetof(WorldSector, texture_a) == 0x38);
static_assert(offsetof(WorldSector, model) == 0x3c);
static_assert(offsetof(WorldSector, texture_b) == 0x40);
static_assert(offsetof(WorldSector, u32_4e) == 0x4e);
static_assert(offsetof(WorldSector, u32_52) == 0x52);
static_assert(offsetof(WorldSector, nodes_tail) == 0x5e);
static_assert(offsetof(WorldSector, nodes_head) == 0x62);
#endif

}  // namespace joan::orig
