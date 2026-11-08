// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptBlock class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_node.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct ScriptBlock : ScriptNode {
#include "script_block_fields.inc"
#include "script_block_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptBlock) == 0x20);
static_assert(offsetof(ScriptBlock, statements) == 0x14);
static_assert(offsetof(ScriptBlock, vars) == 0x18);
static_assert(offsetof(ScriptBlock, labels) == 0x1c);
#endif

}  // namespace joan::orig
