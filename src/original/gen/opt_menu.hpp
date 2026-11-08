// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's OptMenu class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BmpRes;
struct Button;
struct ButtonArray10;
struct ComboBox;
struct Image2D;
struct KeyIdArray10;
struct ScrollBox;
struct Texture;

#pragma pack(push, 1)
struct OptMenu {
#include "opt_menu_fields.inc"
#include "opt_menu_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(OptMenu, images) == 0x0);
static_assert(offsetof(OptMenu, u32_18) == 0x18);
static_assert(offsetof(OptMenu, title) == 0x28);
static_assert(offsetof(OptMenu, frame_top) == 0x2c);
static_assert(offsetof(OptMenu, frame_bottom) == 0x30);
static_assert(offsetof(OptMenu, tab_button_0) == 0x34);
static_assert(offsetof(OptMenu, tab_button_1) == 0x38);
static_assert(offsetof(OptMenu, tab_button_2) == 0x3c);
static_assert(offsetof(OptMenu, tab_button_3) == 0x40);
static_assert(offsetof(OptMenu, tab_button_4) == 0x44);
static_assert(offsetof(OptMenu, snd_slider_48) == 0x48);
static_assert(offsetof(OptMenu, snd_btn_4c) == 0x4c);
static_assert(offsetof(OptMenu, snd_slider_50) == 0x50);
static_assert(offsetof(OptMenu, snd_btn_54) == 0x54);
static_assert(offsetof(OptMenu, snd_slider_58) == 0x58);
static_assert(offsetof(OptMenu, snd_btn_5c) == 0x5c);
static_assert(offsetof(OptMenu, snd_slider_60) == 0x60);
static_assert(offsetof(OptMenu, snd_btn_64) == 0x64);
static_assert(offsetof(OptMenu, snd_slider_68) == 0x68);
static_assert(offsetof(OptMenu, snd_btn_6c) == 0x6c);
static_assert(offsetof(OptMenu, snd_slider_70) == 0x70);
static_assert(offsetof(OptMenu, snd_btn_74) == 0x74);
static_assert(offsetof(OptMenu, snd_combo_78) == 0x78);
static_assert(offsetof(OptMenu, snd_btn_7c) == 0x7c);
static_assert(offsetof(OptMenu, snd_combo_80) == 0x80);
static_assert(offsetof(OptMenu, snd_btn_84) == 0x84);
static_assert(offsetof(OptMenu, snd_btn_88) == 0x88);
static_assert(offsetof(OptMenu, snd_btn_8c) == 0x8c);
static_assert(offsetof(OptMenu, game_combo_90) == 0x90);
static_assert(offsetof(OptMenu, game_btn_94) == 0x94);
static_assert(offsetof(OptMenu, game_btn_98) == 0x98);
static_assert(offsetof(OptMenu, game_btn_9c) == 0x9c);
static_assert(offsetof(OptMenu, game_slider_a0) == 0xa0);
static_assert(offsetof(OptMenu, game_btn_a4) == 0xa4);
static_assert(offsetof(OptMenu, game_slider_a8) == 0xa8);
static_assert(offsetof(OptMenu, game_btn_ac) == 0xac);
static_assert(offsetof(OptMenu, game_slider_b0) == 0xb0);
static_assert(offsetof(OptMenu, game_btn_b4) == 0xb4);
static_assert(offsetof(OptMenu, key_combo_b8) == 0xb8);
static_assert(offsetof(OptMenu, key_btn_bc) == 0xbc);
static_assert(offsetof(OptMenu, key_combo_c0) == 0xc0);
static_assert(offsetof(OptMenu, key_btn_c4) == 0xc4);
static_assert(offsetof(OptMenu, key_slider_c8) == 0xc8);
static_assert(offsetof(OptMenu, key_btn_cc) == 0xcc);
static_assert(offsetof(OptMenu, key_slider_d0) == 0xd0);
static_assert(offsetof(OptMenu, key_btn_d4) == 0xd4);
static_assert(offsetof(OptMenu, key_btn_d8) == 0xd8);
static_assert(offsetof(OptMenu, key_btn_dc) == 0xdc);
static_assert(offsetof(OptMenu, key_btn_e0) == 0xe0);
static_assert(offsetof(OptMenu, key_combo_e4) == 0xe4);
static_assert(offsetof(OptMenu, key_btn_e8) == 0xe8);
static_assert(offsetof(OptMenu, video_combo_ec) == 0xec);
static_assert(offsetof(OptMenu, video_btn_f0) == 0xf0);
static_assert(offsetof(OptMenu, credit_btn_f4) == 0xf4);
static_assert(offsetof(OptMenu, credit_btn_f8) == 0xf8);
static_assert(offsetof(OptMenu, credit_btn_fc) == 0xfc);
static_assert(offsetof(OptMenu, credit_combo_100) == 0x100);
static_assert(offsetof(OptMenu, credit_lines) == 0x104);
static_assert(offsetof(OptMenu, credit_labels) == 0x108);
static_assert(offsetof(OptMenu, key_count) == 0x10c);
static_assert(offsetof(OptMenu, keys) == 0x110);
static_assert(offsetof(OptMenu, key_slot) == 0x114);
static_assert(offsetof(OptMenu, video_combo_115) == 0x115);
static_assert(offsetof(OptMenu, video_btn_119) == 0x119);
static_assert(offsetof(OptMenu, video_btn_11d) == 0x11d);
static_assert(offsetof(OptMenu, page) == 0x121);
static_assert(offsetof(OptMenu, slide_state) == 0x122);
static_assert(offsetof(OptMenu, slide_flag) == 0x123);
#endif

}  // namespace joan::orig
