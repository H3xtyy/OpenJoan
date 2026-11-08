// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Cell class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct CamZones;
struct MapObject;
struct Occluder;
struct PathSpace;
struct Place;
struct Tracks;
struct U8PtrArray;
struct Unit;
struct World;
struct WorldItem;
struct WorldSectorPtrArray;

#pragma pack(push, 1)
struct Cell {
#include "cell_fields.inc"
#include "cell_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Cell) == 0xac);
static_assert(offsetof(Cell, world) == 0x0);
static_assert(offsetof(Cell, ptr_4) == 0x4);
static_assert(offsetof(Cell, box) == 0x8);
static_assert(offsetof(Cell, name) == 0x20);
static_assert(offsetof(Cell, loaded) == 0x43);
static_assert(offsetof(Cell, origin_x) == 0x44);
static_assert(offsetof(Cell, origin_y) == 0x48);
static_assert(offsetof(Cell, sector_rows) == 0x4c);
static_assert(offsetof(Cell, sector_cols) == 0x4e);
static_assert(offsetof(Cell, i32_50) == 0x50);
static_assert(offsetof(Cell, path_space) == 0x54);
static_assert(offsetof(Cell, path_space_b) == 0x58);
static_assert(offsetof(Cell, tracks) == 0x5c);
static_assert(offsetof(Cell, camera_zones) == 0x60);
static_assert(offsetof(Cell, sector_count) == 0x64);
static_assert(offsetof(Cell, u32_68) == 0x68);
static_assert(offsetof(Cell, sectors) == 0x6c);
static_assert(offsetof(Cell, sector_names) == 0x70);
static_assert(offsetof(Cell, occluders) == 0x74);
static_assert(offsetof(Cell, occluder_count) == 0x78);
static_assert(offsetof(Cell, occluder_cap) == 0x7c);
static_assert(offsetof(Cell, p84_count) == 0x80);
static_assert(offsetof(Cell, p_84) == 0x84);
static_assert(offsetof(Cell, rows) == 0x88);
static_assert(offsetof(Cell, units_tail) == 0x8c);
static_assert(offsetof(Cell, units_head) == 0x90);
static_assert(offsetof(Cell, objects_tail) == 0x94);
static_assert(offsetof(Cell, objects_head) == 0x98);
static_assert(offsetof(Cell, items_tail) == 0x9c);
static_assert(offsetof(Cell, items_head) == 0xa0);
static_assert(offsetof(Cell, places_tail) == 0xa4);
static_assert(offsetof(Cell, places_head) == 0xa8);
#endif

}  // namespace joan::orig
