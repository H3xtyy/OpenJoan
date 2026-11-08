// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's GoalPanel class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct GoalPanel {
#include "goal_panel_fields.inc"
#include "goal_panel_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(GoalPanel, tex) == 0x0);
static_assert(offsetof(GoalPanel, sheet) == 0x4);
static_assert(offsetof(GoalPanel, show_main) == 0x8);
static_assert(offsetof(GoalPanel, show_side) == 0x9);
static_assert(offsetof(GoalPanel, goal_main) == 0xa);
static_assert(offsetof(GoalPanel, goal_side) == 0xe);
static_assert(offsetof(GoalPanel, kind_main) == 0x12);
static_assert(offsetof(GoalPanel, kind_side) == 0x13);
#endif

}  // namespace joan::orig
