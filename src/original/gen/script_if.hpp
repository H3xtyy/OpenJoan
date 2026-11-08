// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptIf class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_block.hpp"

namespace joan::orig {

struct ScriptNode;

#pragma pack(push, 1)
struct ScriptIf : ScriptBlock {
#include "script_if_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptIf) == 0x24);
static_assert(offsetof(ScriptIf, else_branch) == 0x20);
#endif

}  // namespace joan::orig
