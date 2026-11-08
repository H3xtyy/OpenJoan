// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PathGraph class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct TrackNode;

#pragma pack(push, 1)
struct PathGraph {
#include "path_graph_fields.inc"
#include "path_graph_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PathGraph) == 0x52);
static_assert(offsetof(PathGraph, id) == 0x0);
static_assert(offsetof(PathGraph, name) == 0x4);
static_assert(offsetof(PathGraph, label) == 0x25);
static_assert(offsetof(PathGraph, node_count) == 0x46);
static_assert(offsetof(PathGraph, nodes) == 0x4a);
static_assert(offsetof(PathGraph, u32_4e) == 0x4e);
#endif

}  // namespace joan::orig
