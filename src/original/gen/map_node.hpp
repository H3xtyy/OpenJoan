// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MapNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Map;

#pragma pack(push, 1)
struct MapNode {
#include "map_node_fields.inc"
#include "map_node_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MapNode) == 0xac);
static_assert(offsetof(MapNode, map) == 0x0);
#endif

}  // namespace joan::orig
