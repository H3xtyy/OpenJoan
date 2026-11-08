// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's IngameMenu class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"
#include "label.hpp"

namespace joan::orig {

struct Texture;

#pragma pack(push, 1)
struct IngameMenu {
#include "ingame_menu_fields.inc"
#include "ingame_menu_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(IngameMenu, texture) == 0x0);
static_assert(offsetof(IngameMenu, font_14) == 0x14);
static_assert(offsetof(IngameMenu, font_18) == 0x18);
static_assert(offsetof(IngameMenu, font_1c) == 0x1c);
static_assert(offsetof(IngameMenu, font_20) == 0x20);
static_assert(offsetof(IngameMenu, open) == 0x24);
static_assert(offsetof(IngameMenu, dialog) == 0x25);
static_assert(offsetof(IngameMenu, u8_26) == 0x26);
static_assert(offsetof(IngameMenu, u8_27) == 0x27);
static_assert(offsetof(IngameMenu, button0) == 0x28);
static_assert(offsetof(IngameMenu, button1) == 0x36e);
static_assert(offsetof(IngameMenu, button2) == 0x6b4);
static_assert(offsetof(IngameMenu, button3) == 0x9fa);
static_assert(offsetof(IngameMenu, button4) == 0xd40);
static_assert(offsetof(IngameMenu, button5) == 0x1086);
static_assert(offsetof(IngameMenu, button6) == 0x13cc);
static_assert(offsetof(IngameMenu, button7) == 0x1712);
static_assert(offsetof(IngameMenu, back_button) == 0x1a58);
static_assert(offsetof(IngameMenu, title) == 0x1d9e);
static_assert(offsetof(IngameMenu, caption) == 0x1e55);
#endif

}  // namespace joan::orig
