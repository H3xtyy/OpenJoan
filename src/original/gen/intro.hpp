// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Intro class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "image2d_t.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Intro {
#include "intro_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Intro) == 0x45f);
static_assert(offsetof(Intro, image_0) == 0x0);
static_assert(offsetof(Intro, image_1) == 0xb7);
static_assert(offsetof(Intro, image_2) == 0x16e);
static_assert(offsetof(Intro, image_3) == 0x225);
static_assert(offsetof(Intro, image_4) == 0x2dc);
static_assert(offsetof(Intro, image_5) == 0x393);
static_assert(offsetof(Intro, state) == 0x452);
static_assert(offsetof(Intro, u32_454) == 0x454);
static_assert(offsetof(Intro, i8_458) == 0x458);
static_assert(offsetof(Intro, i32_459) == 0x459);
static_assert(offsetof(Intro, i8_45d) == 0x45d);
static_assert(offsetof(Intro, i8_45e) == 0x45e);
#endif

}  // namespace joan::orig
