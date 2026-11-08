// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's EffectKey class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"
#include "float_range.hpp"

namespace joan::orig {

struct ActorDef;

#pragma pack(push, 1)
struct EffectKey {
#include "effect_key_fields.inc"
#include "effect_key_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(EffectKey, initialised) == 0x0);
static_assert(offsetof(EffectKey, lifetime) == 0x1);
static_assert(offsetof(EffectKey, emit_rate) == 0x9);
static_assert(offsetof(EffectKey, emit_rate2) == 0x11);
static_assert(offsetof(EffectKey, spawn_dir) == 0x19);
static_assert(offsetof(EffectKey, emit_radius) == 0x31);
static_assert(offsetof(EffectKey, gravity) == 0x49);
static_assert(offsetof(EffectKey, range_61) == 0x61);
static_assert(offsetof(EffectKey, follow_camera) == 0x69);
static_assert(offsetof(EffectKey, orientation) == 0x6a);
static_assert(offsetof(EffectKey, orir) == 0x76);
static_assert(offsetof(EffectKey, offset) == 0x7a);
static_assert(offsetof(EffectKey, time_delay) == 0x86);
static_assert(offsetof(EffectKey, self_rotation) == 0x8a);
static_assert(offsetof(EffectKey, self_rot_axis) == 0x8b);
static_assert(offsetof(EffectKey, self_rot_speed) == 0x97);
static_assert(offsetof(EffectKey, has_rotation) == 0x9f);
static_assert(offsetof(EffectKey, rotation_speed) == 0xa0);
static_assert(offsetof(EffectKey, rotation_center) == 0xa8);
static_assert(offsetof(EffectKey, rotation_axis) == 0xb4);
static_assert(offsetof(EffectKey, model_name) == 0xc0);
static_assert(offsetof(EffectKey, texture_name) == 0xe1);
static_assert(offsetof(EffectKey, name_102) == 0x102);
static_assert(offsetof(EffectKey, rchange) == 0x123);
static_assert(offsetof(EffectKey, texture_period) == 0x124);
static_assert(offsetof(EffectKey, texture_number) == 0x128);
static_assert(offsetof(EffectKey, u32_12c) == 0x12c);
static_assert(offsetof(EffectKey, u32_130) == 0x130);
static_assert(offsetof(EffectKey, num_particles) == 0x134);
static_assert(offsetof(EffectKey, loop) == 0x138);
static_assert(offsetof(EffectKey, is_smo) == 0x13c);
static_assert(offsetof(EffectKey, u32_141) == 0x141);
static_assert(offsetof(EffectKey, keys) == 0x145);
#endif

}  // namespace joan::orig
