// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Actor class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ActorDef;
struct AnimDef;
struct AnimKey;
struct Attachment;
struct Bone;
struct MorphPtr;
struct Plane;
struct Scene;

#pragma pack(push, 1)
struct Actor {
#include "actor_fields.inc"
#include "actor_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Actor) == 0x10c);
static_assert(offsetof(Actor, def) == 0x0);
static_assert(offsetof(Actor, anim) == 0x4);
static_assert(offsetof(Actor, kind) == 0x8);
static_assert(offsetof(Actor, owner) == 0xc);
static_assert(offsetof(Actor, holder_kind) == 0x10);
static_assert(offsetof(Actor, holder) == 0x14);
static_assert(offsetof(Actor, scene) == 0x18);
static_assert(offsetof(Actor, scene_params0) == 0x1c);
static_assert(offsetof(Actor, scene_prev) == 0x20);
static_assert(offsetof(Actor, scene_next) == 0x24);
static_assert(offsetof(Actor, scale) == 0x28);
static_assert(offsetof(Actor, skin) == 0x2c);
static_assert(offsetof(Actor, fade) == 0x30);
static_assert(offsetof(Actor, alpha) == 0x34);
static_assert(offsetof(Actor, fade_38) == 0x38);
static_assert(offsetof(Actor, effect) == 0x3c);
static_assert(offsetof(Actor, list_index) == 0x40);
static_assert(offsetof(Actor, action) == 0x44);
static_assert(offsetof(Actor, sub_action) == 0x48);
static_assert(offsetof(Actor, start_time) == 0x4c);
static_assert(offsetof(Actor, anim_flags) == 0x50);
static_assert(offsetof(Actor, last_eval) == 0x54);
static_assert(offsetof(Actor, anim_time) == 0x58);
static_assert(offsetof(Actor, record) == 0x5c);
static_assert(offsetof(Actor, velocity) == 0x60);
static_assert(offsetof(Actor, impulse) == 0x6c);
static_assert(offsetof(Actor, keys) == 0x78);
static_assert(offsetof(Actor, key_count) == 0x7c);
static_assert(offsetof(Actor, blend_ms) == 0x80);
static_assert(offsetof(Actor, cursor_owner) == 0x84);
static_assert(offsetof(Actor, bone_mats) == 0x88);
static_assert(offsetof(Actor, bone) == 0x8c);
static_assert(offsetof(Actor, plane) == 0x90);
static_assert(offsetof(Actor, last_model) == 0x94);
static_assert(offsetof(Actor, last_lod) == 0x9c);
static_assert(offsetof(Actor, vis_a7) == 0xa7);
static_assert(offsetof(Actor, vis_time) == 0xa8);
static_assert(offsetof(Actor, morph_cap) == 0xac);
static_assert(offsetof(Actor, morph_count) == 0xb0);
static_assert(offsetof(Actor, morphs) == 0xb4);
static_assert(offsetof(Actor, aim_mode) == 0xb8);
static_assert(offsetof(Actor, aim_sphere) == 0xbc);
static_assert(offsetof(Actor, aim_box) == 0xcc);
static_assert(offsetof(Actor, collide_kind) == 0xe4);
static_assert(offsetof(Actor, collide_offset) == 0xe8);
static_assert(offsetof(Actor, collide_radius) == 0xf0);
static_assert(offsetof(Actor, collide_height) == 0xf4);
static_assert(offsetof(Actor, faded) == 0xf8);
static_assert(offsetof(Actor, byte_fc) == 0xfc);
static_assert(offsetof(Actor, flag_fd_) == 0xfd);
static_assert(offsetof(Actor, yaw) == 0xfe);
static_assert(offsetof(Actor, pitch) == 0x102);
static_assert(offsetof(Actor, roll) == 0x106);
static_assert(offsetof(Actor, visible) == 0x10a);
#endif

}  // namespace joan::orig
