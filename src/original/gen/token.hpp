// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Token class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Token {
#include "token_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Token) == 0xc);
static_assert(offsetof(Token, text) == 0x0);
static_assert(offsetof(Token, kind) == 0x4);
static_assert(offsetof(Token, line) == 0x8);
#endif

}  // namespace joan::orig
