// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CampScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Actor;
struct ActorDef;
struct Button;
struct ButtonArray10;
struct Camera;
struct FileTxt;
struct Image2D;
struct Scene;
struct Texture;
struct Unit;
struct Widget;

#pragma pack(push, 1)
struct CampScreen {
#include "camp_screen_fields.inc"
#include "camp_screen_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CampScreen, state) == 0x0);
static_assert(offsetof(CampScreen, slot_masks) == 0x4);
static_assert(offsetof(CampScreen, top_bar) == 0x44);
static_assert(offsetof(CampScreen, bottom_bar) == 0x48);
static_assert(offsetof(CampScreen, side_bar) == 0x4c);
static_assert(offsetof(CampScreen, slot_buttons) == 0x50);
static_assert(offsetof(CampScreen, menu_buttons) == 0x54);
static_assert(offsetof(CampScreen, visible) == 0x80);
static_assert(offsetof(CampScreen, brief_text) == 0x81);
static_assert(offsetof(CampScreen, brief_up) == 0x85);
static_assert(offsetof(CampScreen, brief_down) == 0x89);
static_assert(offsetof(CampScreen, brief_line) == 0x8d);
static_assert(offsetof(CampScreen, brief_px) == 0x8f);
static_assert(offsetof(CampScreen, brief_hold_ms) == 0x91);
static_assert(offsetof(CampScreen, reward_up_button) == 0x95);
static_assert(offsetof(CampScreen, reward_down_button) == 0x99);
static_assert(offsetof(CampScreen, reward_slot_kinds) == 0x9d);
static_assert(offsetof(CampScreen, reward_condition_ids) == 0xa7);
static_assert(offsetof(CampScreen, u32_bb) == 0xbb);
static_assert(offsetof(CampScreen, u32_bf) == 0xbf);
static_assert(offsetof(CampScreen, u32_c3) == 0xc3);
static_assert(offsetof(CampScreen, reward_scroll) == 0xc7);
static_assert(offsetof(CampScreen, reward_count) == 0xcb);
static_assert(offsetof(CampScreen, reward_camera) == 0xcf);
static_assert(offsetof(CampScreen, reward_frame) == 0xd3);
static_assert(offsetof(CampScreen, shop_frame) == 0xd7);
static_assert(offsetof(CampScreen, shop_up_button) == 0xdb);
static_assert(offsetof(CampScreen, shop_down_button) == 0xdf);
static_assert(offsetof(CampScreen, shop_slot_kinds) == 0xe3);
static_assert(offsetof(CampScreen, shop_territory_ids) == 0xed);
static_assert(offsetof(CampScreen, shop_scroll) == 0x101);
static_assert(offsetof(CampScreen, shop_count) == 0x105);
static_assert(offsetof(CampScreen, shop_camera) == 0x109);
static_assert(offsetof(CampScreen, army_image_frame) == 0x10d);
static_assert(offsetof(CampScreen, army_text_frame) == 0x111);
static_assert(offsetof(CampScreen, army_up_button) == 0x115);
static_assert(offsetof(CampScreen, army_down_button) == 0x119);
static_assert(offsetof(CampScreen, slot_kinds) == 0x11d);
static_assert(offsetof(CampScreen, roster_unit_ids) == 0x127);
static_assert(offsetof(CampScreen, roster_french_row) == 0x13b);
static_assert(offsetof(CampScreen, roster_english_row) == 0x13f);
static_assert(offsetof(CampScreen, roster_mercenary_row) == 0x143);
static_assert(offsetof(CampScreen, roster_scroll) == 0x147);
static_assert(offsetof(CampScreen, roster_count) == 0x14b);
static_assert(offsetof(CampScreen, roster_selected) == 0x14f);
static_assert(offsetof(CampScreen, army_scene) == 0x151);
static_assert(offsetof(CampScreen, army_camera) == 0x155);
static_assert(offsetof(CampScreen, army_model) == 0x159);
static_assert(offsetof(CampScreen, army_actor_def) == 0x15d);
static_assert(offsetof(CampScreen, army_item) == 0x161);
static_assert(offsetof(CampScreen, army_item2) == 0x165);
static_assert(offsetof(CampScreen, army_model_flags) == 0x169);
static_assert(offsetof(CampScreen, army_model_id) == 0x16d);
static_assert(offsetof(CampScreen, p_171) == 0x171);
static_assert(offsetof(CampScreen, siege_scene) == 0x175);
static_assert(offsetof(CampScreen, siege_camera) == 0x179);
static_assert(offsetof(CampScreen, siege_model) == 0x17d);
static_assert(offsetof(CampScreen, siege_actor_def) == 0x181);
static_assert(offsetof(CampScreen, siege_model_flags) == 0x185);
static_assert(offsetof(CampScreen, siege_model_id) == 0x189);
static_assert(offsetof(CampScreen, siege_frame) == 0x18d);
static_assert(offsetof(CampScreen, siege_buttons) == 0x195);
static_assert(offsetof(CampScreen, siege_flags) == 0x1ad);
static_assert(offsetof(CampScreen, siege_tex_debrief) == 0x1b3);
static_assert(offsetof(CampScreen, siege_tex_siege) == 0x1b7);
static_assert(offsetof(CampScreen, siege_selected) == 0x1bb);
static_assert(offsetof(CampScreen, u32_1bf) == 0x1bf);
static_assert(offsetof(CampScreen, siege_desc_text) == 0x1c3);
static_assert(offsetof(CampScreen, help_open) == 0x1c7);
static_assert(offsetof(CampScreen, map_scene) == 0x1c8);
static_assert(offsetof(CampScreen, map_camera) == 0x1cc);
static_assert(offsetof(CampScreen, map_min_x) == 0x1d0);
static_assert(offsetof(CampScreen, map_min_y) == 0x1d4);
static_assert(offsetof(CampScreen, map_max_x) == 0x1d8);
static_assert(offsetof(CampScreen, map_max_y) == 0x1dc);
static_assert(offsetof(CampScreen, camera_view) == 0x1e0);
static_assert(offsetof(CampScreen, view_x) == 0x1e1);
static_assert(offsetof(CampScreen, view_y) == 0x1e5);
static_assert(offsetof(CampScreen, sector_min_x) == 0x1e9);
static_assert(offsetof(CampScreen, sector_min_y) == 0x1ed);
static_assert(offsetof(CampScreen, sector_max_x) == 0x1f1);
static_assert(offsetof(CampScreen, sector_max_y) == 0x1f5);
static_assert(offsetof(CampScreen, u32_1f9) == 0x1f9);
static_assert(offsetof(CampScreen, u32_1fd) == 0x1fd);
static_assert(offsetof(CampScreen, u32_201) == 0x201);
static_assert(offsetof(CampScreen, u32_205) == 0x205);
static_assert(offsetof(CampScreen, zoom_step) == 0x209);
static_assert(offsetof(CampScreen, view_toggle_button) == 0x20d);
static_assert(offsetof(CampScreen, strat_up) == 0x211);
static_assert(offsetof(CampScreen, strat_down) == 0x215);
static_assert(offsetof(CampScreen, strat_first) == 0x219);
static_assert(offsetof(CampScreen, strat_selected) == 0x21d);
static_assert(offsetof(CampScreen, strategy_text) == 0x221);
static_assert(offsetof(CampScreen, strategy_texture) == 0x225);
static_assert(offsetof(CampScreen, u32_229) == 0x229);
static_assert(offsetof(CampScreen, u32_22d) == 0x22d);
static_assert(offsetof(CampScreen, p_231) == 0x231);
static_assert(offsetof(CampScreen, hero_buttons) == 0x235);
static_assert(offsetof(CampScreen, heroes) == 0x245);
static_assert(offsetof(CampScreen, tex_debrief) == 0x255);
static_assert(offsetof(CampScreen, tex_heroes) == 0x259);
static_assert(offsetof(CampScreen, selected_hero) == 0x265);
static_assert(offsetof(CampScreen, mission) == 0x269);
static_assert(offsetof(CampScreen, map_drag_x) == 0x26c);
static_assert(offsetof(CampScreen, map_drag_y) == 0x26e);
#endif

}  // namespace joan::orig
