// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Boid class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Actor;

#pragma pack(push, 1)
struct Boid {
#include "boid_fields.inc"
#include "boid_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Boid) == 0x7f);
static_assert(offsetof(Boid, vtable) == 0x0);
static_assert(offsetof(Boid, actor) == 0x4);
static_assert(offsetof(Boid, kind) == 0x8);
static_assert(offsetof(Boid, u16_a) == 0xa);
static_assert(offsetof(Boid, member_index) == 0xc);
static_assert(offsetof(Boid, radius) == 0xe);
static_assert(offsetof(Boid, pos) == 0x12);
static_assert(offsetof(Boid, vel) == 0x1e);
static_assert(offsetof(Boid, dir) == 0x2a);
static_assert(offsetof(Boid, speed) == 0x36);
static_assert(offsetof(Boid, neighbour_count) == 0x3a);
static_assert(offsetof(Boid, u16_3c) == 0x3c);
static_assert(offsetof(Boid, nearest) == 0x3e);
static_assert(offsetof(Boid, u32_42) == 0x42);
static_assert(offsetof(Boid, nearest_dist) == 0x46);
static_assert(offsetof(Boid, u32_4a) == 0x4a);
static_assert(offsetof(Boid, prev_pos) == 0x4e);
static_assert(offsetof(Boid, prev_vel) == 0x5a);
static_assert(offsetof(Boid, next_) == 0x66);
static_assert(offsetof(Boid, prev) == 0x6a);
static_assert(offsetof(Boid, avoiding) == 0x6e);
static_assert(offsetof(Boid, avoid_blend) == 0x6f);
static_assert(offsetof(Boid, avoid_dir) == 0x73);
#endif

}  // namespace joan::orig
