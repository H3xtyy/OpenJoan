// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptAssign class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_node.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptAssign : ScriptNode {
#include "script_assign_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptAssign) == 0x1c);
static_assert(offsetof(ScriptAssign, target) == 0x14);
static_assert(offsetof(ScriptAssign, expr) == 0x18);
#endif

}  // namespace joan::orig
