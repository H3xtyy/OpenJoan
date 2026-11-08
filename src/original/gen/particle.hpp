// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Particle class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Actor;

#pragma pack(push, 1)
struct Particle {
#include "particle_fields.inc"
#include "particle_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Particle) == 0x80);
static_assert(offsetof(Particle, id) == 0x0);
static_assert(offsetof(Particle, lifetime) == 0x4);
static_assert(offsetof(Particle, age) == 0x8);
static_assert(offsetof(Particle, delay) == 0xc);
static_assert(offsetof(Particle, time) == 0x10);
static_assert(offsetof(Particle, f32_14) == 0x14);
static_assert(offsetof(Particle, node) == 0x18);
static_assert(offsetof(Particle, scale) == 0x1c);
static_assert(offsetof(Particle, scale_rate) == 0x20);
static_assert(offsetof(Particle, colour) == 0x24);
static_assert(offsetof(Particle, colour_rate) == 0x28);
static_assert(offsetof(Particle, u32_38) == 0x38);
static_assert(offsetof(Particle, rot_speed) == 0x3c);
static_assert(offsetof(Particle, self_angle) == 0x40);
static_assert(offsetof(Particle, self_rot_speed) == 0x44);
static_assert(offsetof(Particle, offset) == 0x48);
static_assert(offsetof(Particle, pos) == 0x54);
static_assert(offsetof(Particle, vel) == 0x60);
static_assert(offsetof(Particle, accel) == 0x6c);
static_assert(offsetof(Particle, key_index) == 0x78);
static_assert(offsetof(Particle, frame_time) == 0x7a);
static_assert(offsetof(Particle, frame) == 0x7e);
#endif

}  // namespace joan::orig
