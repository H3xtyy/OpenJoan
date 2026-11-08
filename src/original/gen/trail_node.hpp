// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TrailNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TrailNode {
#include "trail_node_fields.inc"
#include "trail_node_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TrailNode) == 0x84);
static_assert(offsetof(TrailNode, points) == 0x0);
static_assert(offsetof(TrailNode, tangents) == 0x3c);
static_assert(offsetof(TrailNode, time) == 0x78);
static_assert(offsetof(TrailNode, newer) == 0x7c);
static_assert(offsetof(TrailNode, older) == 0x80);
#endif

}  // namespace joan::orig
