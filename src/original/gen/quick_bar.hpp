// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's QuickBar class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "image2d.hpp"

namespace joan::orig {

struct BmpRes;
struct Texture;

#pragma pack(push, 1)
struct QuickBar {
#include "quick_bar_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(QuickBar) == 0x45f);
static_assert(offsetof(QuickBar, texture) == 0x0);
static_assert(offsetof(QuickBar, sprites) == 0x4);
static_assert(offsetof(QuickBar, save_text) == 0x8);
static_assert(offsetof(QuickBar, save_bar) == 0xbf);
static_assert(offsetof(QuickBar, load_text) == 0x176);
static_assert(offsetof(QuickBar, load_bar) == 0x22d);
static_assert(offsetof(QuickBar, loadc_text) == 0x2e4);
static_assert(offsetof(QuickBar, loadc_bar) == 0x39b);
static_assert(offsetof(QuickBar, state) == 0x452);
static_assert(offsetof(QuickBar, timer) == 0x454);
static_assert(offsetof(QuickBar, popup_open) == 0x458);
static_assert(offsetof(QuickBar, u32_459) == 0x459);
static_assert(offsetof(QuickBar, u8_45d) == 0x45d);
static_assert(offsetof(QuickBar, active) == 0x45e);
#endif

}  // namespace joan::orig
