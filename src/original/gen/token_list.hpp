// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TokenList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"
#include "string_pool.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TokenList {
#include "token_list_fields.inc"
#include "token_list_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(TokenList, vtable) == 0x0);
static_assert(offsetof(TokenList, strings) == 0x4);
static_assert(offsetof(TokenList, tokens) == 0xc);
#endif

}  // namespace joan::orig
