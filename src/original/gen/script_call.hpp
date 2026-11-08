// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptCall class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_func_call.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptCall : ScriptFuncCall {
#include "script_call_fields.inc"
#include "script_call_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptCall) == 0x24);
static_assert(offsetof(ScriptCall, arg_id) == 0x22);
#endif

}  // namespace joan::orig
