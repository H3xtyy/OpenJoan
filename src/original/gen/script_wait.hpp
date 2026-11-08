// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptWait class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_block.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptWait : ScriptBlock {
#include "script_wait_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptWait) == 0x21);
static_assert(offsetof(ScriptWait, waiting) == 0x20);
#endif

}  // namespace joan::orig
