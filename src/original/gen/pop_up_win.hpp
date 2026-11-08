// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PopUpWin class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "menu_button.hpp"

namespace joan::orig {

struct Font2d;

#pragma pack(push, 1)
struct PopUpWin {
#include "pop_up_win_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PopUpWin) == 0x6ad);
static_assert(offsetof(PopUpWin, x) == 0x0);
static_assert(offsetof(PopUpWin, y) == 0x2);
static_assert(offsetof(PopUpWin, x2) == 0x4);
static_assert(offsetof(PopUpWin, y2) == 0x6);
static_assert(offsetof(PopUpWin, w) == 0x8);
static_assert(offsetof(PopUpWin, h) == 0xa);
static_assert(offsetof(PopUpWin, text_len) == 0xc);
static_assert(offsetof(PopUpWin, flags) == 0xe);
static_assert(offsetof(PopUpWin, has_cancel) == 0xf);
static_assert(offsetof(PopUpWin, has_ok) == 0x10);
static_assert(offsetof(PopUpWin, sprite_set) == 0x11);
static_assert(offsetof(PopUpWin, texture) == 0x15);
static_assert(offsetof(PopUpWin, ok_button) == 0x19);
static_assert(offsetof(PopUpWin, cancel_button) == 0x35f);
static_assert(offsetof(PopUpWin, font) == 0x6a5);
static_assert(offsetof(PopUpWin, text) == 0x6a9);
#endif

}  // namespace joan::orig
