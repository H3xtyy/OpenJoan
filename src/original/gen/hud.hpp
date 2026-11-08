// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Hud class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"
#include "mini_map.hpp"

namespace joan::orig {

struct Actor;
struct BmpRes;
struct Plane;
struct Texture;

#pragma pack(push, 1)
struct Hud {
#include "hud_fields.inc"
#include "hud_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Hud, tex_maingame) == 0x0);
static_assert(offsetof(Hud, tex_mgexp) == 0x4);
static_assert(offsetof(Hud, tex_08) == 0x8);
static_assert(offsetof(Hud, tex_tui) == 0xc);
static_assert(offsetof(Hud, sprites) == 0x10);
static_assert(offsetof(Hud, exp_sprites) == 0x14);
static_assert(offsetof(Hud, sprites_18) == 0x18);
static_assert(offsetof(Hud, target_sprites) == 0x1c);
static_assert(offsetof(Hud, shown_hp) == 0x20);
static_assert(offsetof(Hud, shown_145) == 0x24);
static_assert(offsetof(Hud, shown_126) == 0x28);
static_assert(offsetof(Hud, target_hp) == 0x2c);
static_assert(offsetof(Hud, health_state) == 0x30);
static_assert(offsetof(Hud, health_state_31) == 0x31);
static_assert(offsetof(Hud, u8_32) == 0x32);
static_assert(offsetof(Hud, target_id) == 0x33);
static_assert(offsetof(Hud, target_is_unit) == 0x37);
static_assert(offsetof(Hud, target_time) == 0x38);
static_assert(offsetof(Hud, exp_target) == 0x3c);
static_assert(offsetof(Hud, exp_len) == 0x60);
static_assert(offsetof(Hud, exp_shown) == 0x69);
static_assert(offsetof(Hud, exp_state) == 0x8d);
static_assert(offsetof(Hud, exp_time) == 0x96);
static_assert(offsetof(Hud, exp_start) == 0xba);
static_assert(offsetof(Hud, exp_count) == 0xbe);
static_assert(offsetof(Hud, exp_on) == 0xc0);
static_assert(offsetof(Hud, level_on) == 0xc1);
static_assert(offsetof(Hud, level_state) == 0xc2);
static_assert(offsetof(Hud, level_c3) == 0xc3);
static_assert(offsetof(Hud, level_time) == 0xc4);
static_assert(offsetof(Hud, combo_on) == 0xcc);
static_assert(offsetof(Hud, combo_state) == 0xcd);
static_assert(offsetof(Hud, combo_count) == 0xce);
static_assert(offsetof(Hud, combo_time) == 0xd0);
static_assert(offsetof(Hud, slot_unit) == 0xd5);
static_assert(offsetof(Hud, slot_flag) == 0xf5);
static_assert(offsetof(Hud, slot_time) == 0xfd);
static_assert(offsetof(Hud, slot_state) == 0x11d);
static_assert(offsetof(Hud, arrow_unit) == 0x125);
static_assert(offsetof(Hud, arrow_count) == 0x145);
static_assert(offsetof(Hud, u32_147) == 0x147);
static_assert(offsetof(Hud, u16_14b) == 0x14b);
static_assert(offsetof(Hud, blink_on) == 0x14d);
static_assert(offsetof(Hud, blink_state) == 0x14e);
static_assert(offsetof(Hud, blink_time) == 0x14f);
static_assert(offsetof(Hud, blink_start) == 0x153);
static_assert(offsetof(Hud, obj_alpha) == 0x157);
static_assert(offsetof(Hud, obj_1a7) == 0x1a7);
static_assert(offsetof(Hud, obj_count) == 0x1f7);
static_assert(offsetof(Hud, obj_time) == 0x1f9);
static_assert(offsetof(Hud, obj_id) == 0x299);
static_assert(offsetof(Hud, squad_button0) == 0x339);
static_assert(offsetof(Hud, squad_button1) == 0x67f);
static_assert(offsetof(Hud, squad_button2) == 0x9c5);
static_assert(offsetof(Hud, squad_button3) == 0xd0b);
static_assert(offsetof(Hud, squad_button4) == 0x1051);
static_assert(offsetof(Hud, squad_button5) == 0x1397);
static_assert(offsetof(Hud, squad_button6) == 0x16dd);
static_assert(offsetof(Hud, squad_button7) == 0x1a23);
static_assert(offsetof(Hud, minimap) == 0x1d69);
static_assert(offsetof(Hud, portrait_plane) == 0x21e9);
static_assert(offsetof(Hud, portrait_actor) == 0x21ed);
#endif

}  // namespace joan::orig
