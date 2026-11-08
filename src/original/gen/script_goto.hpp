// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptGoto class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_node.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptGoto : ScriptNode {
#include "script_goto_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptGoto) == 0x18);
static_assert(offsetof(ScriptGoto, target_label) == 0x14);
#endif

}  // namespace joan::orig
