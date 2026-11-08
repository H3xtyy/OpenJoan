// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ProfEdge class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ProfFunc;
struct ProfNode;

#pragma pack(push, 1)
struct ProfEdge {
#include "prof_edge_fields.inc"
#include "prof_edge_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ProfEdge) == 0x20);
static_assert(offsetof(ProfEdge, caller) == 0x0);
static_assert(offsetof(ProfEdge, caller_prev) == 0x4);
static_assert(offsetof(ProfEdge, caller_next) == 0x8);
static_assert(offsetof(ProfEdge, callee) == 0xc);
static_assert(offsetof(ProfEdge, callee_prev) == 0x10);
static_assert(offsetof(ProfEdge, callee_next) == 0x14);
static_assert(offsetof(ProfEdge, nodes_tail) == 0x18);
static_assert(offsetof(ProfEdge, nodes_head) == 0x1c);
#endif

}  // namespace joan::orig
