// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MapObject class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Actor;
struct Cell;
struct Scene;
struct World;
struct WorldItem;

#pragma pack(push, 1)
struct MapObject {
#include "map_object_fields.inc"
#include "map_object_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MapObject) == 0xc2);
static_assert(offsetof(MapObject, vtable) == 0x0);
static_assert(offsetof(MapObject, index) == 0x4);
static_assert(offsetof(MapObject, portrait_unit) == 0x6);
static_assert(offsetof(MapObject, side) == 0x8);
static_assert(offsetof(MapObject, type_id) == 0xa);
static_assert(offsetof(MapObject, name) == 0xc);
static_assert(offsetof(MapObject, class_id) == 0x4e);
static_assert(offsetof(MapObject, item_type) == 0x50);
static_assert(offsetof(MapObject, busy) == 0x52);
static_assert(offsetof(MapObject, pos) == 0x56);
static_assert(offsetof(MapObject, yaw_) == 0x62);
static_assert(offsetof(MapObject, pitch) == 0x66);
static_assert(offsetof(MapObject, roll_) == 0x6a);
static_assert(offsetof(MapObject, scale) == 0x6e);
static_assert(offsetof(MapObject, damage) == 0x72);
static_assert(offsetof(MapObject, defense) == 0x76);
static_assert(offsetof(MapObject, duration) == 0x7a);
static_assert(offsetof(MapObject, interact_trigger) == 0x7e);
static_assert(offsetof(MapObject, active) == 0x82);
static_assert(offsetof(MapObject, refresh_depth) == 0x83);
static_assert(offsetof(MapObject, u16_84) == 0x84);
static_assert(offsetof(MapObject, actor) == 0x86);
static_assert(offsetof(MapObject, u32_8a) == 0x8a);
static_assert(offsetof(MapObject, actor_b) == 0x8e);
static_assert(offsetof(MapObject, light_slot) == 0x92);
static_assert(offsetof(MapObject, glow_slot) == 0x94);
static_assert(offsetof(MapObject, sound_slots) == 0x96);
static_assert(offsetof(MapObject, world_item) == 0xa2);
static_assert(offsetof(MapObject, world) == 0xa6);
static_assert(offsetof(MapObject, cell) == 0xaa);
static_assert(offsetof(MapObject, cell_prev) == 0xae);
static_assert(offsetof(MapObject, cell_next) == 0xb2);
static_assert(offsetof(MapObject, scene) == 0xb6);
static_assert(offsetof(MapObject, u32_ba) == 0xba);
static_assert(offsetof(MapObject, u32_be) == 0xbe);
#endif

}  // namespace joan::orig
