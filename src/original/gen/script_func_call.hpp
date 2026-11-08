// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptFuncCall class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_block.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptFuncCall : ScriptBlock {
#include "script_func_call_fields.inc"
#include "script_func_call_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptFuncCall) == 0x22);
static_assert(offsetof(ScriptFuncCall, func_id) == 0x20);
#endif

}  // namespace joan::orig
