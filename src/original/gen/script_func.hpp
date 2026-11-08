// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptFunc class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "value.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptFunc : Value {
#include "script_func_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptFunc) == 0x18);
#endif

}  // namespace joan::orig
