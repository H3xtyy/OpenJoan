// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's StringBlock class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct StringBlock {
#include "string_block_fields.inc"
#include "string_block_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(StringBlock) == 0xfa8);
static_assert(offsetof(StringBlock, next) == 0x0);
static_assert(offsetof(StringBlock, used) == 0x4);
static_assert(offsetof(StringBlock, text) == 0x8);
#endif

}  // namespace joan::orig
