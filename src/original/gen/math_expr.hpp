// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MathExpr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_block.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct MathExpr : ScriptBlock {
#include "math_expr_fields.inc"
#include "math_expr_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MathExpr) == 0x24);
static_assert(offsetof(MathExpr, rpn) == 0x20);
#endif

}  // namespace joan::orig
