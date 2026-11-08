// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ScriptBlock;

#pragma pack(push, 1)
struct ScriptNode {
#include "script_node_fields.inc"
#include "script_node_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptNode) == 0x14);
static_assert(offsetof(ScriptNode, vtable) == 0x0);
static_assert(offsetof(ScriptNode, parent) == 0x4);
static_assert(offsetof(ScriptNode, kind) == 0x8);
static_assert(offsetof(ScriptNode, value_type) == 0xc);
static_assert(offsetof(ScriptNode, value) == 0x10);
#endif

}  // namespace joan::orig
