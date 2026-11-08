// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitType class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitType {
#include "unit_type_fields.inc"
#include "unit_type_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UnitType) == 0x203);
static_assert(offsetof(UnitType, type_id) == 0x0);
static_assert(offsetof(UnitType, unit_class) == 0x2);
static_assert(offsetof(UnitType, name) == 0x4);
static_assert(offsetof(UnitType, anim_name) == 0x25);
static_assert(offsetof(UnitType, anim_name_b) == 0x46);
static_assert(offsetof(UnitType, model_a) == 0x68);
static_assert(offsetof(UnitType, model_b) == 0x6c);
static_assert(offsetof(UnitType, anim_id_b) == 0x70);
static_assert(offsetof(UnitType, extra_model_count) == 0x74);
static_assert(offsetof(UnitType, model_refs) == 0x78);
static_assert(offsetof(UnitType, model_ids) == 0x84);
static_assert(offsetof(UnitType, model_flags) == 0x90);
static_assert(offsetof(UnitType, speed_walk) == 0x9c);
static_assert(offsetof(UnitType, speed_run) == 0xa0);
static_assert(offsetof(UnitType, speed_strafe) == 0xa4);
static_assert(offsetof(UnitType, speed_strafe_b) == 0xa8);
static_assert(offsetof(UnitType, speed_back) == 0xac);
static_assert(offsetof(UnitType, refcount) == 0xb0);
static_assert(offsetof(UnitType, title_b4) == 0xb4);
static_assert(offsetof(UnitType, text_de) == 0xde);
static_assert(offsetof(UnitType, portrait) == 0x100);
static_assert(offsetof(UnitType, height) == 0x109);
static_assert(offsetof(UnitType, body_type) == 0x10b);
static_assert(offsetof(UnitType, str_10d) == 0x10d);
static_assert(offsetof(UnitType, gender) == 0x117);
static_assert(offsetof(UnitType, kind) == 0x118);
static_assert(offsetof(UnitType, trigger_flags) == 0x11c);
static_assert(offsetof(UnitType, is_hero) == 0x120);
static_assert(offsetof(UnitType, idle_delay) == 0x121);
static_assert(offsetof(UnitType, script_id) == 0x125);
static_assert(offsetof(UnitType, str_129) == 0x129);
static_assert(offsetof(UnitType, rank) == 0x149);
static_assert(offsetof(UnitType, kingdom_id) == 0x14b);
static_assert(offsetof(UnitType, race) == 0x14d);
static_assert(offsetof(UnitType, hit_points) == 0x151);
static_assert(offsetof(UnitType, base_max_ep) == 0x155);
static_assert(offsetof(UnitType, lives_max) == 0x159);
static_assert(offsetof(UnitType, xp_value) == 0x15d);
static_assert(offsetof(UnitType, cost) == 0x161);
static_assert(offsetof(UnitType, siege_165) == 0x165);
static_assert(offsetof(UnitType, upgrade_id) == 0x169);
static_assert(offsetof(UnitType, i16_16d) == 0x16d);
static_assert(offsetof(UnitType, armor_code) == 0x16f);
static_assert(offsetof(UnitType, item_weapon) == 0x171);
static_assert(offsetof(UnitType, item_weapon_b) == 0x173);
static_assert(offsetof(UnitType, item_shield) == 0x175);
static_assert(offsetof(UnitType, item_stack) == 0x177);
static_assert(offsetof(UnitType, treasure_list) == 0x179);
static_assert(offsetof(UnitType, strength) == 0x17b);
static_assert(offsetof(UnitType, dexterity) == 0x17d);
static_assert(offsetof(UnitType, defense) == 0x17f);
static_assert(offsetof(UnitType, leadership) == 0x181);
static_assert(offsetof(UnitType, i16_183) == 0x183);
static_assert(offsetof(UnitType, i16_185) == 0x185);
static_assert(offsetof(UnitType, i16_187) == 0x187);
static_assert(offsetof(UnitType, i16_189) == 0x189);
static_assert(offsetof(UnitType, i16_18b) == 0x18b);
static_assert(offsetof(UnitType, i16_18d) == 0x18d);
static_assert(offsetof(UnitType, i16_18f) == 0x18f);
static_assert(offsetof(UnitType, i16_191) == 0x191);
static_assert(offsetof(UnitType, i16_193) == 0x193);
static_assert(offsetof(UnitType, i16_195) == 0x195);
static_assert(offsetof(UnitType, i16_197) == 0x197);
static_assert(offsetof(UnitType, level) == 0x199);
static_assert(offsetof(UnitType, i16_19b) == 0x19b);
static_assert(offsetof(UnitType, i16_19d) == 0x19d);
static_assert(offsetof(UnitType, i16_19f) == 0x19f);
static_assert(offsetof(UnitType, i16_1a1) == 0x1a1);
static_assert(offsetof(UnitType, idle_kind) == 0x1a3);
static_assert(offsetof(UnitType, regen_hp) == 0x1a5);
static_assert(offsetof(UnitType, regen_ep) == 0x1a7);
static_assert(offsetof(UnitType, morale) == 0x1a9);
static_assert(offsetof(UnitType, i16_1ab) == 0x1ab);
static_assert(offsetof(UnitType, slot_count) == 0x1ad);
static_assert(offsetof(UnitType, buffers) == 0x1b1);
static_assert(offsetof(UnitType, shape_kind) == 0x1c3);
static_assert(offsetof(UnitType, shape_sphere) == 0x1c7);
static_assert(offsetof(UnitType, shape_box) == 0x1d7);
static_assert(offsetof(UnitType, shape_cylinder) == 0x1ef);
#endif

}  // namespace joan::orig
