// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Player class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "cam_params.hpp"

namespace joan::orig {

struct BmpRes;
struct CamZones;

#pragma pack(push, 1)
struct Player {
#include "player_fields.inc"
#include "player_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Player) == 0x126e);
static_assert(offsetof(Player, unit_id_) == 0x0);
static_assert(offsetof(Player, i16_4) == 0x4);
static_assert(offsetof(Player, slot) == 0x6);
static_assert(offsetof(Player, i32_8) == 0x8);
static_assert(offsetof(Player, mode) == 0x10);
static_assert(offsetof(Player, prev_mode) == 0x14);
static_assert(offsetof(Player, cam_zones) == 0x18);
static_assert(offsetof(Player, view_quad) == 0x1c);
static_assert(offsetof(Player, rally_pos) == 0x44);
static_assert(offsetof(Player, camera) == 0x4c);
static_assert(offsetof(Player, follow_mode) == 0x74);
static_assert(offsetof(Player, follow_param) == 0x75);
static_assert(offsetof(Player, u8_79) == 0x79);
static_assert(offsetof(Player, u16_7a) == 0x7a);
static_assert(offsetof(Player, u8_7c) == 0x7c);
static_assert(offsetof(Player, cursor_mode) == 0x7d);
static_assert(offsetof(Player, blocked) == 0x81);
static_assert(offsetof(Player, last_dir_key) == 0x82);
static_assert(offsetof(Player, last_dir_key_ms) == 0x86);
static_assert(offsetof(Player, dir_tap_armed) == 0x8a);
static_assert(offsetof(Player, button_down) == 0x8e);
static_assert(offsetof(Player, button_up) == 0x92);
static_assert(offsetof(Player, order_state) == 0x96);
static_assert(offsetof(Player, press_time) == 0x97);
static_assert(offsetof(Player, combo_step) == 0x9b);
static_assert(offsetof(Player, combo_count) == 0x9d);
static_assert(offsetof(Player, jump_hold_ms) == 0x9e);
static_assert(offsetof(Player, attack_target_id) == 0xa2);
static_assert(offsetof(Player, turn) == 0xa6);
static_assert(offsetof(Player, u32_a8) == 0xa8);
static_assert(offsetof(Player, riding) == 0xac);
static_assert(offsetof(Player, aim_yaw_rad) == 0xad);
static_assert(offsetof(Player, aim_yaw_deg) == 0xb1);
static_assert(offsetof(Player, aim_pitch_deg) == 0xb5);
static_assert(offsetof(Player, manual_aim) == 0xb9);
static_assert(offsetof(Player, aim_lock_id) == 0xbb);
static_assert(offsetof(Player, aiming) == 0xbf);
static_assert(offsetof(Player, aim_time) == 0xc0);
static_assert(offsetof(Player, mounted) == 0xc4);
static_assert(offsetof(Player, mount_heading) == 0xc5);
static_assert(offsetof(Player, mount_heading_deg) == 0xc9);
static_assert(offsetof(Player, squad_mode) == 0xcd);
static_assert(offsetof(Player, goto_target_) == 0xd3);
static_assert(offsetof(Player, goto_kind) == 0xd7);
static_assert(offsetof(Player, goto_active) == 0xdb);
static_assert(offsetof(Player, goto_time) == 0xdd);
static_assert(offsetof(Player, f32_e1) == 0xe1);
static_assert(offsetof(Player, pan_speed) == 0xe9);
static_assert(offsetof(Player, zoom_dist) == 0xed);
static_assert(offsetof(Player, prev_rts_active) == 0xf1);
static_assert(offsetof(Player, rts_active) == 0xf5);
static_assert(offsetof(Player, group_counts) == 0xf9);
static_assert(offsetof(Player, group_units) == 0x105);
static_assert(offsetof(Player, dragging) == 0x1235);
static_assert(offsetof(Player, group) == 0x1236);
static_assert(offsetof(Player, selected_hero) == 0x1237);
static_assert(offsetof(Player, selected_commander_id) == 0x1238);
static_assert(offsetof(Player, drag_x) == 0x123c);
static_assert(offsetof(Player, drag_y) == 0x1240);
static_assert(offsetof(Player, pick_kind) == 0x1248);
static_assert(offsetof(Player, click_time) == 0x124c);
static_assert(offsetof(Player, pick_id) == 0x1250);
static_assert(offsetof(Player, point_kind) == 0x1254);
static_assert(offsetof(Player, point_name) == 0x1258);
static_assert(offsetof(Player, rtsicon_res) == 0x1261);
static_assert(offsetof(Player, rts_icon) == 0x1265);
static_assert(offsetof(Player, castle_popup_open) == 0x1269);
static_assert(offsetof(Player, prev_move_angle) == 0x126a);
#endif

}  // namespace joan::orig
