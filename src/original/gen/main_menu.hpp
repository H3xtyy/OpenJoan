// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MainMenu class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"
#include "control2d.hpp"
#include "image2d.hpp"

namespace joan::orig {

struct Texture;

#pragma pack(push, 1)
struct MainMenu : Control2D {
#include "main_menu_fields.inc"
#include "main_menu_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MainMenu) == 0x3454);
static_assert(offsetof(MainMenu, images) == 0x15);
static_assert(offsetof(MainMenu, font) == 0x31);
static_assert(offsetof(MainMenu, font2) == 0x35);
static_assert(offsetof(MainMenu, button0) == 0x39);
static_assert(offsetof(MainMenu, button1) == 0x37f);
static_assert(offsetof(MainMenu, button2) == 0x6c5);
static_assert(offsetof(MainMenu, button3) == 0xa0b);
static_assert(offsetof(MainMenu, button4) == 0xd51);
static_assert(offsetof(MainMenu, button5) == 0x1097);
static_assert(offsetof(MainMenu, button6) == 0x13dd);
static_assert(offsetof(MainMenu, button7) == 0x1723);
static_assert(offsetof(MainMenu, button8) == 0x1a69);
static_assert(offsetof(MainMenu, button9) == 0x1daf);
static_assert(offsetof(MainMenu, button10) == 0x20f5);
static_assert(offsetof(MainMenu, button11) == 0x243b);
static_assert(offsetof(MainMenu, button12) == 0x2781);
static_assert(offsetof(MainMenu, img_main) == 0x2ac7);
static_assert(offsetof(MainMenu, img_alab_l) == 0x2b7e);
static_assert(offsetof(MainMenu, img_alab_r) == 0x2c35);
static_assert(offsetof(MainMenu, img_alab_s) == 0x2cec);
static_assert(offsetof(MainMenu, img_gold_l) == 0x2da3);
static_assert(offsetof(MainMenu, img_gold_r) == 0x2e5a);
static_assert(offsetof(MainMenu, img_lea_l) == 0x2f11);
static_assert(offsetof(MainMenu, img_lea_r) == 0x2fc8);
static_assert(offsetof(MainMenu, img_sphere) == 0x307f);
static_assert(offsetof(MainMenu, img_stripe) == 0x3136);
static_assert(offsetof(MainMenu, img_logo_bar) == 0x31ed);
static_assert(offsetof(MainMenu, img_logo) == 0x32a4);
static_assert(offsetof(MainMenu, img_joan) == 0x335b);
static_assert(offsetof(MainMenu, open) == 0x3412);
static_assert(offsetof(MainMenu, busy) == 0x3413);
static_assert(offsetof(MainMenu, slide) == 0x3414);
static_assert(offsetof(MainMenu, panel_save) == 0x3415);
static_assert(offsetof(MainMenu, panel_options) == 0x3416);
static_assert(offsetof(MainMenu, panel_quit_game) == 0x3417);
static_assert(offsetof(MainMenu, panel_quit) == 0x3418);
static_assert(offsetof(MainMenu, difficulty_0_chosen) == 0x3419);
static_assert(offsetof(MainMenu, difficulty_1_chosen) == 0x341a);
static_assert(offsetof(MainMenu, difficulty_2_chosen) == 0x341b);
static_assert(offsetof(MainMenu, difficulty_3_chosen) == 0x341c);
static_assert(offsetof(MainMenu, back_requested) == 0x341d);
static_assert(offsetof(MainMenu, buttons_off) == 0x341e);
static_assert(offsetof(MainMenu, release_time) == 0x341f);
static_assert(offsetof(MainMenu, layout_width) == 0x3423);
static_assert(offsetof(MainMenu, layout_height) == 0x3425);
static_assert(offsetof(MainMenu, u8_3427) == 0x3427);
static_assert(offsetof(MainMenu, intro_delay_ms) == 0x342a);
static_assert(offsetof(MainMenu, cheat_buf) == 0x342e);
static_assert(offsetof(MainMenu, cheat_len) == 0x344e);
static_assert(offsetof(MainMenu, u32_3450) == 0x3450);
#endif

}  // namespace joan::orig
