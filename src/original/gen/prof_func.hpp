// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ProfFunc class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ProfEdge;
struct ProfNode;

#pragma pack(push, 1)
struct ProfFunc {
#include "prof_func_fields.inc"
#include "prof_func_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ProfFunc) == 0x1c);
static_assert(offsetof(ProfFunc, id) == 0x0);
static_assert(offsetof(ProfFunc, out_tail) == 0x4);
static_assert(offsetof(ProfFunc, out_head) == 0x8);
static_assert(offsetof(ProfFunc, in_tail) == 0xc);
static_assert(offsetof(ProfFunc, in_head) == 0x10);
static_assert(offsetof(ProfFunc, nodes_tail) == 0x14);
static_assert(offsetof(ProfFunc, nodes_head) == 0x18);
#endif

}  // namespace joan::orig
