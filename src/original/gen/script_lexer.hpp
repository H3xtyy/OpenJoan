// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptLexer class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"
#include "string_pool.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptLexer {
#include "script_lexer_fields.inc"
#include "script_lexer_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ScriptLexer, vtable) == 0x0);
static_assert(offsetof(ScriptLexer, strings) == 0x4);
static_assert(offsetof(ScriptLexer, tokens) == 0xc);
static_assert(offsetof(ScriptLexer, i32_29) == 0x29);
#endif

}  // namespace joan::orig
