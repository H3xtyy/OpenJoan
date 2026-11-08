// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitTemplate class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "unit_stats.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitTemplate {
#include "unit_template_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(UnitTemplate, type_id) == 0x0);
static_assert(offsetof(UnitTemplate, layer) == 0x2);
static_assert(offsetof(UnitTemplate, stats) == 0x4);
static_assert(offsetof(UnitTemplate, name) == 0x3f);
static_assert(offsetof(UnitTemplate, text) == 0xc3);
static_assert(offsetof(UnitTemplate, rank) == 0x104);
static_assert(offsetof(UnitTemplate, race_id) == 0x10a);
static_assert(offsetof(UnitTemplate, level) == 0x10c);
static_assert(offsetof(UnitTemplate, i16_10e) == 0x10e);
static_assert(offsetof(UnitTemplate, hit_points) == 0x110);
static_assert(offsetof(UnitTemplate, hit_points_base) == 0x114);
static_assert(offsetof(UnitTemplate, regen_hp) == 0x118);
static_assert(offsetof(UnitTemplate, regen_ep) == 0x11c);
static_assert(offsetof(UnitTemplate, stat_0) == 0x120);
static_assert(offsetof(UnitTemplate, experience) == 0x124);
static_assert(offsetof(UnitTemplate, i16_128) == 0x128);
static_assert(offsetof(UnitTemplate, i16_12a) == 0x12a);
static_assert(offsetof(UnitTemplate, i16_12c) == 0x12c);
static_assert(offsetof(UnitTemplate, idle_time) == 0x12e);
static_assert(offsetof(UnitTemplate, morale) == 0x13a);
static_assert(offsetof(UnitTemplate, i16_13c) == 0x13c);
#endif

}  // namespace joan::orig
