// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptVarDecl class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_node.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptVarDecl : ScriptNode {
#include "script_var_decl_fields.inc"
#include "script_var_decl_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptVarDecl) == 0x48);
static_assert(offsetof(ScriptVarDecl, name_) == 0x14);
#endif

}  // namespace joan::orig
