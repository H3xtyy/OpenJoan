// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SaveMenu class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"
#include "combo_box.hpp"
#include "edit_box.hpp"
#include "label.hpp"
#include "scroll_bar.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SaveMenu {
#include "save_menu_fields.inc"
#include "save_menu_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(SaveMenu, sprites) == 0x0);
static_assert(offsetof(SaveMenu, u32_4) == 0x4);
static_assert(offsetof(SaveMenu, sprites2) == 0x10);
static_assert(offsetof(SaveMenu, preview_texture) == 0x14);
static_assert(offsetof(SaveMenu, font) == 0x18);
static_assert(offsetof(SaveMenu, u32_1c) == 0x1c);
static_assert(offsetof(SaveMenu, u32_20) == 0x20);
static_assert(offsetof(SaveMenu, busy_) == 0x24);
static_assert(offsetof(SaveMenu, u8_25) == 0x25);
static_assert(offsetof(SaveMenu, font2) == 0x28);
static_assert(offsetof(SaveMenu, slots) == 0x2c);
static_assert(offsetof(SaveMenu, list) == 0x20e8);
static_assert(offsetof(SaveMenu, frame) == 0x2195);
static_assert(offsetof(SaveMenu, pic) == 0x224c);
static_assert(offsetof(SaveMenu, info_bar) == 0x2303);
static_assert(offsetof(SaveMenu, info) == 0x2649);
static_assert(offsetof(SaveMenu, bar_top) == 0x2700);
static_assert(offsetof(SaveMenu, bar_bottom) == 0x27b7);
static_assert(offsetof(SaveMenu, btn_286e) == 0x286e);
static_assert(offsetof(SaveMenu, btn_2bb4) == 0x2bb4);
static_assert(offsetof(SaveMenu, btn_2efa) == 0x2efa);
static_assert(offsetof(SaveMenu, btn_3240) == 0x3240);
static_assert(offsetof(SaveMenu, btn_3586) == 0x3586);
static_assert(offsetof(SaveMenu, mark) == 0x38cc);
static_assert(offsetof(SaveMenu, btn_3c12) == 0x3c12);
static_assert(offsetof(SaveMenu, btn_3f58) == 0x3f58);
static_assert(offsetof(SaveMenu, in_frame) == 0x429e);
static_assert(offsetof(SaveMenu, in_bg) == 0x4355);
static_assert(offsetof(SaveMenu, name_edit) == 0x440c);
static_assert(offsetof(SaveMenu, name) == 0x449d);
static_assert(offsetof(SaveMenu, edit_button) == 0x44dd);
static_assert(offsetof(SaveMenu, edit_scroll) == 0x4823);
static_assert(offsetof(SaveMenu, sliding) == 0x4afb);
static_assert(offsetof(SaveMenu, selected_slot) == 0x4afc);
static_assert(offsetof(SaveMenu, page) == 0x4afd);
static_assert(offsetof(SaveMenu, state) == 0x4afe);
static_assert(offsetof(SaveMenu, u8_4aff) == 0x4aff);
static_assert(offsetof(SaveMenu, u8_4b00) == 0x4b00);
static_assert(offsetof(SaveMenu, save_mode) == 0x4b01);
static_assert(offsetof(SaveMenu, slide_pos) == 0x4b02);
static_assert(offsetof(SaveMenu, name_edit_state) == 0x4b04);
static_assert(offsetof(SaveMenu, is_open_) == 0x4b05);
static_assert(offsetof(SaveMenu, from_victory) == 0x4b06);
static_assert(offsetof(SaveMenu, from_defeat) == 0x4b07);
static_assert(offsetof(SaveMenu, load_done) == 0x4b08);
#endif

}  // namespace joan::orig
