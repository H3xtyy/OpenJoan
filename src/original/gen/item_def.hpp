// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ItemDef class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ItemModifier;
struct ItemRequirement;

#pragma pack(push, 1)
struct ItemDef {
#include "item_def_fields.inc"
#include "item_def_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ItemDef) == 0x2bf);
static_assert(offsetof(ItemDef, class_id) == 0x0);
static_assert(offsetof(ItemDef, type_id) == 0x2);
static_assert(offsetof(ItemDef, id) == 0x4);
static_assert(offsetof(ItemDef, icon_sprite) == 0x6);
static_assert(offsetof(ItemDef, model_sprite) == 0x8);
static_assert(offsetof(ItemDef, bone) == 0xa);
static_assert(offsetof(ItemDef, ref_count) == 0xc);
static_assert(offsetof(ItemDef, name) == 0x10);
static_assert(offsetof(ItemDef, file_name) == 0x14);
static_assert(offsetof(ItemDef, message_name) == 0x18);
static_assert(offsetof(ItemDef, struction_type) == 0x1c);
static_assert(offsetof(ItemDef, description) == 0x20);
static_assert(offsetof(ItemDef, stats) == 0x24);
static_assert(offsetof(ItemDef, price) == 0x2e);
static_assert(offsetof(ItemDef, i32_32) == 0x32);
static_assert(offsetof(ItemDef, min_level) == 0x36);
static_assert(offsetof(ItemDef, max_level) == 0x3a);
static_assert(offsetof(ItemDef, not_block) == 0x3e);
static_assert(offsetof(ItemDef, i32_42) == 0x42);
static_assert(offsetof(ItemDef, colour) == 0x46);
static_assert(offsetof(ItemDef, damage_type) == 0x4a);
static_assert(offsetof(ItemDef, damage) == 0x4e);
static_assert(offsetof(ItemDef, inc_damage) == 0x52);
static_assert(offsetof(ItemDef, defense) == 0x56);
static_assert(offsetof(ItemDef, duration) == 0x5a);
static_assert(offsetof(ItemDef, i32_5e) == 0x5e);
static_assert(offsetof(ItemDef, model_count) == 0x62);
static_assert(offsetof(ItemDef, need_key) == 0x66);
static_assert(offsetof(ItemDef, model_values) == 0x67);
static_assert(offsetof(ItemDef, model_names) == 0x77);
static_assert(offsetof(ItemDef, anim_names) == 0x87);
static_assert(offsetof(ItemDef, model_kinds) == 0x97);
static_assert(offsetof(ItemDef, slot_refs) == 0x9b);
static_assert(offsetof(ItemDef, slot_anims) == 0xaf);
static_assert(offsetof(ItemDef, blur_vertex_count) == 0xc3);
static_assert(offsetof(ItemDef, blur_name) == 0xc5);
static_assert(offsetof(ItemDef, blur_vertices) == 0xe6);
static_assert(offsetof(ItemDef, emitter_names) == 0x122);
static_assert(offsetof(ItemDef, emitter_pos) == 0x132);
static_assert(offsetof(ItemDef, emitter_scale) == 0x162);
static_assert(offsetof(ItemDef, emitter_scripts) == 0x172);
static_assert(offsetof(ItemDef, emitter_flags) == 0x182);
static_assert(offsetof(ItemDef, sound_params) == 0x186);
static_assert(offsetof(ItemDef, sound_floats) == 0x18d);
static_assert(offsetof(ItemDef, has_light2d) == 0x1b1);
static_assert(offsetof(ItemDef, light_kind) == 0x1b2);
static_assert(offsetof(ItemDef, light) == 0x1b6);
static_assert(offsetof(ItemDef, need_collision) == 0x1c2);
static_assert(offsetof(ItemDef, pickable) == 0x1c4);
static_assert(offsetof(ItemDef, has_bounding_box) == 0x1c5);
static_assert(offsetof(ItemDef, bounding_box) == 0x1c7);
static_assert(offsetof(ItemDef, can_transparent) == 0x1df);
static_assert(offsetof(ItemDef, modify_camera) == 0x1e0);
static_assert(offsetof(ItemDef, enlarge_scene_sector) == 0x1e1);
static_assert(offsetof(ItemDef, has_anti_portal) == 0x1e2);
static_assert(offsetof(ItemDef, skip_intersect_with_item) == 0x1e3);
static_assert(offsetof(ItemDef, has_path) == 0x1e4);
static_assert(offsetof(ItemDef, anti_portal_percentage) == 0x1e5);
static_assert(offsetof(ItemDef, item_usage) == 0x1e7);
static_assert(offsetof(ItemDef, show_armory) == 0x1e8);
static_assert(offsetof(ItemDef, collision_type) == 0x1e9);
static_assert(offsetof(ItemDef, collision_params) == 0x1ed);
static_assert(offsetof(ItemDef, terrain_sound) == 0x229);
static_assert(offsetof(ItemDef, terrain_sound_arg) == 0x22d);
static_assert(offsetof(ItemDef, sounds) == 0x231);
static_assert(offsetof(ItemDef, i32_23d) == 0x23d);
static_assert(offsetof(ItemDef, required_level) == 0x241);
static_assert(offsetof(ItemDef, requirement_count) == 0x245);
static_assert(offsetof(ItemDef, requirements) == 0x249);
static_assert(offsetof(ItemDef, effect_count) == 0x24d);
static_assert(offsetof(ItemDef, effects) == 0x251);
static_assert(offsetof(ItemDef, usable_unit) == 0x255);
static_assert(offsetof(ItemDef, combine_item) == 0x257);
static_assert(offsetof(ItemDef, result_item) == 0x259);
static_assert(offsetof(ItemDef, unique_item) == 0x25b);
static_assert(offsetof(ItemDef, can_drop) == 0x25c);
static_assert(offsetof(ItemDef, can_sell) == 0x25d);
static_assert(offsetof(ItemDef, open_with_interact) == 0x25e);
static_assert(offsetof(ItemDef, str_25f) == 0x25f);
static_assert(offsetof(ItemDef, str_27f) == 0x27f);
#endif

}  // namespace joan::orig
