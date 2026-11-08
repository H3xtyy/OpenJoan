// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's View class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct View {
#include "view_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(View, i16_8) == 0x8);
static_assert(offsetof(View, u8_e) == 0xe);
static_assert(offsetof(View, word_f) == 0xf);
#endif

}  // namespace joan::orig
