// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's StripNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Bone;

#pragma pack(push, 1)
struct StripNode {
#include "strip_node_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(StripNode) == 0x30);
static_assert(offsetof(StripNode, vertex_range) == 0x0);
static_assert(offsetof(StripNode, min_index) == 0x4);
static_assert(offsetof(StripNode, vertices) == 0x8);
static_assert(offsetof(StripNode, vertex_buffer) == 0xc);
static_assert(offsetof(StripNode, index_count) == 0x10);
static_assert(offsetof(StripNode, start_index) == 0x14);
static_assert(offsetof(StripNode, indices) == 0x18);
static_assert(offsetof(StripNode, index_buffer) == 0x1c);
static_assert(offsetof(StripNode, primitive_count) == 0x20);
static_assert(offsetof(StripNode, bones) == 0x24);
static_assert(offsetof(StripNode, bone_count) == 0x28);
static_assert(offsetof(StripNode, next) == 0x2c);
#endif

}  // namespace joan::orig
