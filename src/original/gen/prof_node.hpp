// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ProfNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ProfEdge;
struct ProfFunc;

#pragma pack(push, 1)
struct ProfNode {
#include "prof_node_fields.inc"
#include "prof_node_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ProfNode) == 0x50);
static_assert(offsetof(ProfNode, last) == 0x0);
static_assert(offsetof(ProfNode, total) == 0x8);
static_assert(offsetof(ProfNode, start_) == 0x10);
static_assert(offsetof(ProfNode, nesting) == 0x18);
static_assert(offsetof(ProfNode, average) == 0x1c);
static_assert(offsetof(ProfNode, parent) == 0x24);
static_assert(offsetof(ProfNode, sibling_prev) == 0x28);
static_assert(offsetof(ProfNode, sibling_next) == 0x2c);
static_assert(offsetof(ProfNode, children_tail) == 0x30);
static_assert(offsetof(ProfNode, children_head) == 0x34);
static_assert(offsetof(ProfNode, func) == 0x38);
static_assert(offsetof(ProfNode, func_prev) == 0x3c);
static_assert(offsetof(ProfNode, func_next) == 0x40);
static_assert(offsetof(ProfNode, edge) == 0x44);
static_assert(offsetof(ProfNode, edge_prev) == 0x48);
static_assert(offsetof(ProfNode, edge_next) == 0x4c);
#endif

}  // namespace joan::orig
