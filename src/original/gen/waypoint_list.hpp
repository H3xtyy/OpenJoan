// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WaypointList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct WaypointList {
#include "waypoint_list_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(WaypointList, count) == 0xc2);
static_assert(offsetof(WaypointList, positions) == 0xc6);
static_assert(offsetof(WaypointList, values) == 0x162);
#endif

}  // namespace joan::orig
