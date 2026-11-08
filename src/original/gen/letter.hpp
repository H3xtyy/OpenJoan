// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Letter class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Letter {
#include "letter_fields.inc"
#include "letter_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Letter, tex) == 0x0);
static_assert(offsetof(Letter, sheet) == 0x4);
static_assert(offsetof(Letter, open_) == 0x8);
static_assert(offsetof(Letter, timer) == 0x9);
static_assert(offsetof(Letter, i16_d) == 0xd);
static_assert(offsetof(Letter, text) == 0xf);
static_assert(offsetof(Letter, i32_13) == 0x13);
static_assert(offsetof(Letter, seal) == 0x17);
static_assert(offsetof(Letter, u16_19) == 0x19);
#endif

}  // namespace joan::orig
