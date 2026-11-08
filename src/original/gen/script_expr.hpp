// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptExpr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_node.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptExpr : ScriptNode {
#include "script_expr_fields.inc"
#include "script_expr_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptExpr) == 0x1c);
static_assert(offsetof(ScriptExpr, lhs) == 0x14);
static_assert(offsetof(ScriptExpr, rhs) == 0x18);
#endif

}  // namespace joan::orig
