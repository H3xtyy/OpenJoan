// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptKeyword class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptKeyword {
#include "script_keyword_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptKeyword) == 0x8);
static_assert(offsetof(ScriptKeyword, name) == 0x0);
static_assert(offsetof(ScriptKeyword, type) == 0x4);
#endif

}  // namespace joan::orig
