// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScenarioEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Script;

#pragma pack(push, 1)
struct ScenarioEntry {
#include "scenario_entry_fields.inc"
#include "scenario_entry_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScenarioEntry) == 0x1b2);
static_assert(offsetof(ScenarioEntry, texts) == 0x0);
static_assert(offsetof(ScenarioEntry, script) == 0x186);
static_assert(offsetof(ScenarioEntry, u32_18a) == 0x18a);
static_assert(offsetof(ScenarioEntry, u32_18e) == 0x18e);
static_assert(offsetof(ScenarioEntry, u32_192) == 0x192);
static_assert(offsetof(ScenarioEntry, u32_196) == 0x196);
static_assert(offsetof(ScenarioEntry, u32_19a) == 0x19a);
static_assert(offsetof(ScenarioEntry, name) == 0x19e);
static_assert(offsetof(ScenarioEntry, name2) == 0x1a2);
static_assert(offsetof(ScenarioEntry, time_limit_ms) == 0x1a6);
static_assert(offsetof(ScenarioEntry, time_left_ms) == 0x1aa);
static_assert(offsetof(ScenarioEntry, kind) == 0x1ae);
#endif

}  // namespace joan::orig
