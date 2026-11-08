// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimGroupRec class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AnimGroupRec {
#include "anim_group_rec_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AnimGroupRec) == 0x99);
static_assert(offsetof(AnimGroupRec, is_jump) == 0x0);
static_assert(offsetof(AnimGroupRec, kind) == 0x4);
static_assert(offsetof(AnimGroupRec, name_8) == 0x8);
static_assert(offsetof(AnimGroupRec, u8_e) == 0xe);
static_assert(offsetof(AnimGroupRec, u8_f) == 0xf);
static_assert(offsetof(AnimGroupRec, weapon_kind) == 0x10);
static_assert(offsetof(AnimGroupRec, direction) == 0x14);
static_assert(offsetof(AnimGroupRec, min_mana) == 0x18);
static_assert(offsetof(AnimGroupRec, i32_1c) == 0x1c);
static_assert(offsetof(AnimGroupRec, duration) == 0x20);
static_assert(offsetof(AnimGroupRec, combo_min) == 0x24);
static_assert(offsetof(AnimGroupRec, combo_max) == 0x28);
static_assert(offsetof(AnimGroupRec, anim_name) == 0x2c);
static_assert(offsetof(AnimGroupRec, bip_name) == 0x4d);
static_assert(offsetof(AnimGroupRec, anim_id) == 0x6e);
static_assert(offsetof(AnimGroupRec, model_ref) == 0x72);
static_assert(offsetof(AnimGroupRec, sound_a) == 0x76);
static_assert(offsetof(AnimGroupRec, sound_a_name) == 0x7a);
static_assert(offsetof(AnimGroupRec, sound_a_delay) == 0x83);
static_assert(offsetof(AnimGroupRec, sound_b) == 0x87);
static_assert(offsetof(AnimGroupRec, sound_b_name) == 0x8b);
static_assert(offsetof(AnimGroupRec, sound_b_delay) == 0x94);
static_assert(offsetof(AnimGroupRec, flag_98) == 0x98);
#endif

}  // namespace joan::orig
