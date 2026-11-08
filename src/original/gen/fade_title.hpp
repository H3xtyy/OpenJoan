// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's FadeTitle class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct FadeTitle {
#include "fade_title_fields.inc"
#include "fade_title_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(FadeTitle, active) == 0x0);
static_assert(offsetof(FadeTitle, a) == 0x1);
static_assert(offsetof(FadeTitle, b) == 0x5);
static_assert(offsetof(FadeTitle, c) == 0x9);
static_assert(offsetof(FadeTitle, d) == 0xd);
static_assert(offsetof(FadeTitle, name) == 0x11);
#endif

}  // namespace joan::orig
