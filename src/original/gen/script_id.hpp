// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptId class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_block.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptId : ScriptBlock {
#include "script_id_fields.inc"
#include "script_id_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptId) == 0x25);
static_assert(offsetof(ScriptId, active) == 0x20);
static_assert(offsetof(ScriptId, label_id) == 0x21);
#endif

}  // namespace joan::orig
