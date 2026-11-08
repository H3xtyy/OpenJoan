// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Place class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Actor;
struct Cell;
struct Facility;
struct World;

#pragma pack(push, 1)
struct Place {
#include "place_fields.inc"
#include "place_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Place) == 0x1f6);
static_assert(offsetof(Place, index) == 0x0);
static_assert(offsetof(Place, id) == 0x2);
static_assert(offsetof(Place, kind) == 0x4);
static_assert(offsetof(Place, name) == 0x5);
static_assert(offsetof(Place, name_end) == 0x25);
static_assert(offsetof(Place, nation) == 0x26);
static_assert(offsetof(Place, position_) == 0x2a);
static_assert(offsetof(Place, angle) == 0x36);
static_assert(offsetof(Place, box) == 0x3a);
static_assert(offsetof(Place, counter) == 0x52);
static_assert(offsetof(Place, counter_max) == 0x56);
static_assert(offsetof(Place, timer) == 0x5a);
static_assert(offsetof(Place, entrance_count) == 0x5e);
static_assert(offsetof(Place, entrances) == 0x62);
static_assert(offsetof(Place, slot_count) == 0xc2);
static_assert(offsetof(Place, slot_pos_) == 0xc6);
static_assert(offsetof(Place, u32_156) == 0x156);
static_assert(offsetof(Place, slot_yaw) == 0x15e);
static_assert(offsetof(Place, slot_unit) == 0x192);
static_assert(offsetof(Place, u32_1c6) == 0x1c6);
static_assert(offsetof(Place, spawn_count) == 0x1ca);
static_assert(offsetof(Place, captured) == 0x1ce);
static_assert(offsetof(Place, angle_2) == 0x1d2);
static_assert(offsetof(Place, u32_1d6) == 0x1d6);
static_assert(offsetof(Place, model) == 0x1da);
static_assert(offsetof(Place, world) == 0x1de);
static_assert(offsetof(Place, cell) == 0x1e2);
static_assert(offsetof(Place, cell_prev) == 0x1e6);
static_assert(offsetof(Place, cell_next) == 0x1ea);
static_assert(offsetof(Place, facility_tail) == 0x1ee);
static_assert(offsetof(Place, facility_head_) == 0x1f2);
#endif

}  // namespace joan::orig
