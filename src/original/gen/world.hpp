// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's World class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "bmp_res.hpp"

namespace joan::orig {

struct BoxTree;
struct CellPtrArray;
struct PathSpace;
struct Scene;
struct SceneSPtr;

#pragma pack(push, 1)
struct World {
#include "world_fields.inc"
#include "world_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(World, active) == 0x0);
static_assert(offsetof(World, loaded) == 0x1);
static_assert(offsetof(World, world_id) == 0x2);
static_assert(offsetof(World, id) == 0x4);
static_assert(offsetof(World, name) == 0x6);
static_assert(offsetof(World, title) == 0x27);
static_assert(offsetof(World, bounds) == 0x48);
static_assert(offsetof(World, u8_60) == 0x60);
static_assert(offsetof(World, bitmaps) == 0x61);
static_assert(offsetof(World, path_space) == 0x6a);
static_assert(offsetof(World, scene) == 0x6e);
static_assert(offsetof(World, cell_count) == 0x72);
static_assert(offsetof(World, cell_capacity) == 0x76);
static_assert(offsetof(World, cells) == 0x7a);
static_assert(offsetof(World, box_tree) == 0x7e);
static_assert(offsetof(World, cell_query) == 0x82);
static_assert(offsetof(World, entry_count) == 0x86);
static_assert(offsetof(World, entry_cap) == 0x8a);
static_assert(offsetof(World, entries) == 0x8e);
static_assert(offsetof(World, u8_f5) == 0xf5);
static_assert(offsetof(World, u32_1fd) == 0x1fd);
#endif

}  // namespace joan::orig
