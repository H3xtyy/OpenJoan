// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptCond class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_node.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptCond : ScriptNode {
#include "script_cond_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptCond) == 0x20);
static_assert(offsetof(ScriptCond, op) == 0x14);
static_assert(offsetof(ScriptCond, lhs) == 0x18);
static_assert(offsetof(ScriptCond, rhs) == 0x1c);
#endif

}  // namespace joan::orig
