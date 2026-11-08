// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Morph class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ArrI32;
struct MorphAnim;
struct MorphTarget;
struct Vec3f;

#pragma pack(push, 1)
struct Morph {
#include "morph_fields.inc"
#include "morph_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Morph) == 0x84);
static_assert(offsetof(Morph, loaded) == 0x0);
static_assert(offsetof(Morph, name) == 0x1);
static_assert(offsetof(Morph, target_count) == 0x34);
static_assert(offsetof(Morph, targets) == 0x38);
static_assert(offsetof(Morph, vertex_base) == 0x3c);
static_assert(offsetof(Morph, vertex_count) == 0x40);
static_assert(offsetof(Morph, vertex_indices) == 0x44);
static_assert(offsetof(Morph, blend_start) == 0x48);
static_assert(offsetof(Morph, blend_end) == 0x4c);
static_assert(offsetof(Morph, blend_duration) == 0x50);
static_assert(offsetof(Morph, last_update) == 0x54);
static_assert(offsetof(Morph, positions) == 0x58);
static_assert(offsetof(Morph, normals) == 0x5c);
static_assert(offsetof(Morph, position_step) == 0x60);
static_assert(offsetof(Morph, normal_step) == 0x64);
static_assert(offsetof(Morph, base_positions) == 0x68);
static_assert(offsetof(Morph, base_normals) == 0x6c);
static_assert(offsetof(Morph, current_target) == 0x70);
static_assert(offsetof(Morph, next_target) == 0x74);
static_assert(offsetof(Morph, anim_playing) == 0x78);
static_assert(offsetof(Morph, anim_start) == 0x7c);
static_assert(offsetof(Morph, anim) == 0x80);
#endif

}  // namespace joan::orig
