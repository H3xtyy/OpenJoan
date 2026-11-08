// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Scenario class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "scenario_entry.hpp"

namespace joan::orig {

struct ArrI16;
struct UnitPtr;

#pragma pack(push, 1)
struct Scenario {
#include "scenario_fields.inc"
#include "scenario_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Scenario) == 0x289);
static_assert(offsetof(Scenario, id) == 0x0);
static_assert(offsetof(Scenario, index) == 0x4);
static_assert(offsetof(Scenario, target_stage) == 0x8);
static_assert(offsetof(Scenario, entry_count) == 0xc);
static_assert(offsetof(Scenario, entries) == 0x10);
static_assert(offsetof(Scenario, local) == 0x14);
static_assert(offsetof(Scenario, u32_1c6) == 0x1c6);
static_assert(offsetof(Scenario, timer_a) == 0x1ca);
static_assert(offsetof(Scenario, timer_b) == 0x1ce);
static_assert(offsetof(Scenario, timer_c) == 0x1d2);
static_assert(offsetof(Scenario, counters) == 0x1da);
static_assert(offsetof(Scenario, u8_20e) == 0x20e);
static_assert(offsetof(Scenario, crate_counters) == 0x20f);
static_assert(offsetof(Scenario, food_can_be_picked) == 0x21f);
static_assert(offsetof(Scenario, u8_220) == 0x220);
static_assert(offsetof(Scenario, citizen_saved) == 0x221);
static_assert(offsetof(Scenario, objective_count_time) == 0x225);
static_assert(offsetof(Scenario, rescue_count) == 0x229);
static_assert(offsetof(Scenario, rescue_limit) == 0x22d);
static_assert(offsetof(Scenario, id_list_pad) == 0x233);
static_assert(offsetof(Scenario, ids) == 0x235);
static_assert(offsetof(Scenario, id_count) == 0x275);
static_assert(offsetof(Scenario, types) == 0x277);
static_assert(offsetof(Scenario, type_count) == 0x27b);
static_assert(offsetof(Scenario, items) == 0x27d);
static_assert(offsetof(Scenario, item_count) == 0x281);
static_assert(offsetof(Scenario, squads) == 0x283);
static_assert(offsetof(Scenario, squad_count) == 0x287);
#endif

}  // namespace joan::orig
