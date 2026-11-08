// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptCompare class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "expr.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptCompare : Expr {
#include "script_compare_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptCompare) == 0x20);
#endif

}  // namespace joan::orig
