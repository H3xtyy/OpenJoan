// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptInit class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptInit : Script {
#include "script_init_fields.inc"
#include "script_init_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptInit) == 0x71);
static_assert(offsetof(ScriptInit, done) == 0x70);
#endif

}  // namespace joan::orig
