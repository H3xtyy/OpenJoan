// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TrackNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PathGraph;
struct TrackNodePtr;

#pragma pack(push, 1)
struct TrackNode {
#include "track_node_fields.inc"
#include "track_node_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TrackNode) == 0x54);
static_assert(offsetof(TrackNode, id) == 0x0);
static_assert(offsetof(TrackNode, i32_4) == 0x4);
static_assert(offsetof(TrackNode, pos) == 0x8);
static_assert(offsetof(TrackNode, link_count) == 0x14);
static_assert(offsetof(TrackNode, links) == 0x18);
static_assert(offsetof(TrackNode, search_stamp) == 0x1c);
static_assert(offsetof(TrackNode, parent) == 0x20);
static_assert(offsetof(TrackNode, children) == 0x24);
static_assert(offsetof(TrackNode, next_open) == 0x28);
static_assert(offsetof(TrackNode, grid_x) == 0x2c);
static_assert(offsetof(TrackNode, grid_y) == 0x30);
static_assert(offsetof(TrackNode, i16_34) == 0x34);
static_assert(offsetof(TrackNode, child_count) == 0x36);
static_assert(offsetof(TrackNode, child_cap) == 0x38);
static_assert(offsetof(TrackNode, closed) == 0x3b);
static_assert(offsetof(TrackNode, cost_so_far) == 0x3c);
static_assert(offsetof(TrackNode, cost_total) == 0x40);
static_assert(offsetof(TrackNode, cost_heuristic) == 0x44);
static_assert(offsetof(TrackNode, weight) == 0x48);
static_assert(offsetof(TrackNode, weight_max) == 0x4a);
static_assert(offsetof(TrackNode, weight_sum) == 0x4c);
static_assert(offsetof(TrackNode, owner) == 0x50);
#endif

}  // namespace joan::orig
