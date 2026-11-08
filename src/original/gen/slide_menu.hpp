// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SlideMenu class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "image2d.hpp"

namespace joan::orig {

struct Texture;

#pragma pack(push, 1)
struct SlideMenu {
#include "slide_menu_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(SlideMenu, open) == 0x0);
static_assert(offsetof(SlideMenu, sliding) == 0x1);
static_assert(offsetof(SlideMenu, title) == 0x2);
static_assert(offsetof(SlideMenu, background) == 0x20);
static_assert(offsetof(SlideMenu, slider0) == 0x30);
static_assert(offsetof(SlideMenu, slider1) == 0xe7);
static_assert(offsetof(SlideMenu, slider2) == 0x19e);
static_assert(offsetof(SlideMenu, slider3) == 0x255);
static_assert(offsetof(SlideMenu, slider4) == 0x30c);
static_assert(offsetof(SlideMenu, slider5) == 0x3c3);
static_assert(offsetof(SlideMenu, slider6) == 0x47a);
static_assert(offsetof(SlideMenu, slider7) == 0x531);
static_assert(offsetof(SlideMenu, slider8) == 0x5e8);
static_assert(offsetof(SlideMenu, slider9) == 0x69f);
static_assert(offsetof(SlideMenu, slider10) == 0x756);
static_assert(offsetof(SlideMenu, slider11) == 0x80d);
#endif

}  // namespace joan::orig
