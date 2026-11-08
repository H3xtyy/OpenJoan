// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SpanNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Bone;

#pragma pack(push, 1)
struct SpanNode {
#include "span_node_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SpanNode) == 0x20);
static_assert(offsetof(SpanNode, count) == 0x0);
static_assert(offsetof(SpanNode, first) == 0x4);
static_assert(offsetof(SpanNode, vertices) == 0x8);
static_assert(offsetof(SpanNode, vertex_buffer) == 0xc);
static_assert(offsetof(SpanNode, u32_10) == 0x10);
static_assert(offsetof(SpanNode, bone) == 0x14);
static_assert(offsetof(SpanNode, u32_18) == 0x18);
static_assert(offsetof(SpanNode, next) == 0x1c);
#endif

}  // namespace joan::orig
