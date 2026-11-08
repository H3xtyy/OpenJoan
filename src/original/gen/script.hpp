// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Script class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "script_block.hpp"

namespace joan::orig {

struct DlgRes;
struct ScriptNode;
struct StringPool;

#pragma pack(push, 1)
struct Script : ScriptBlock {
#include "script_fields.inc"
#include "script_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Script) == 0x70);
static_assert(offsetof(Script, name_) == 0x20);
static_assert(offsetof(Script, strings) == 0x64);
static_assert(offsetof(Script, string_pool) == 0x68);
static_assert(offsetof(Script, pool_state) == 0x6c);
#endif

}  // namespace joan::orig
