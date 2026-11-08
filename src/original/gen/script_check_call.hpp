// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptCheckCall class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_func_call.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptCheckCall : ScriptFuncCall {
#include "script_check_call_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptCheckCall) == 0x22);
#endif

}  // namespace joan::orig
