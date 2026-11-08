// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitSnapshot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitSnapshot {
#include "unit_snapshot_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(UnitSnapshot, unit_id) == 0x0);
static_assert(offsetof(UnitSnapshot, layer) == 0x2);
static_assert(offsetof(UnitSnapshot, block) == 0x4);
static_assert(offsetof(UnitSnapshot, u16_3c) == 0x3c);
static_assert(offsetof(UnitSnapshot, u8_3e) == 0x3e);
static_assert(offsetof(UnitSnapshot, name) == 0x3f);
static_assert(offsetof(UnitSnapshot, name2) == 0xc3);
static_assert(offsetof(UnitSnapshot, rank) == 0x104);
static_assert(offsetof(UnitSnapshot, kingdom_id) == 0x108);
static_assert(offsetof(UnitSnapshot, race_id) == 0x10a);
static_assert(offsetof(UnitSnapshot, level) == 0x10c);
static_assert(offsetof(UnitSnapshot, u16_10e) == 0x10e);
static_assert(offsetof(UnitSnapshot, hit_points) == 0x110);
static_assert(offsetof(UnitSnapshot, hit_points_base) == 0x114);
static_assert(offsetof(UnitSnapshot, regen_hp) == 0x118);
static_assert(offsetof(UnitSnapshot, regen_ep) == 0x11c);
static_assert(offsetof(UnitSnapshot, strength) == 0x120);
static_assert(offsetof(UnitSnapshot, dexterity) == 0x122);
static_assert(offsetof(UnitSnapshot, experience) == 0x124);
static_assert(offsetof(UnitSnapshot, u16_128) == 0x128);
static_assert(offsetof(UnitSnapshot, u16_12a) == 0x12a);
static_assert(offsetof(UnitSnapshot, u16_12c) == 0x12c);
static_assert(offsetof(UnitSnapshot, idle_time) == 0x12e);
static_assert(offsetof(UnitSnapshot, morale) == 0x13a);
static_assert(offsetof(UnitSnapshot, u16_13c) == 0x13c);
#endif

}  // namespace joan::orig
