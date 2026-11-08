// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScrollBar class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "derived.hpp"
#include "sb_button.hpp"
#include "scroll.hpp"

namespace joan::orig {

struct BmpRes;
struct Texture;

#pragma pack(push, 1)
struct ScrollBar {
#include "scroll_bar_fields.inc"
#include "scroll_bar_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScrollBar) == 0x2d8);
static_assert(offsetof(ScrollBar, owner) == 0x0);
static_assert(offsetof(ScrollBar, texture) == 0x4);
static_assert(offsetof(ScrollBar, metrics) == 0x8);
static_assert(offsetof(ScrollBar, mode) == 0x74);
static_assert(offsetof(ScrollBar, vbar) == 0x76);
static_assert(offsetof(ScrollBar, hbar) == 0x108);
static_assert(offsetof(ScrollBar, u32_19a) == 0x19a);
static_assert(offsetof(ScrollBar, u32_19e) == 0x19e);
static_assert(offsetof(ScrollBar, button_0) == 0x1a2);
static_assert(offsetof(ScrollBar, button_1) == 0x1e6);
static_assert(offsetof(ScrollBar, button_2) == 0x22a);
static_assert(offsetof(ScrollBar, has_thumb) == 0x26e);
static_assert(offsetof(ScrollBar, thumb_img) == 0x26f);
static_assert(offsetof(ScrollBar, thumb_rect) == 0x27f);
static_assert(offsetof(ScrollBar, has_track) == 0x28f);
static_assert(offsetof(ScrollBar, track_img) == 0x290);
static_assert(offsetof(ScrollBar, track_rect) == 0x2c0);
static_assert(offsetof(ScrollBar, track_x) == 0x2d0);
static_assert(offsetof(ScrollBar, track_y) == 0x2d4);
#endif

}  // namespace joan::orig
