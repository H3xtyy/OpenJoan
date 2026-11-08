// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BodyState class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct BodyState {
#include "body_state_fields.inc"
#include "body_state_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BodyState) == 0x136);
static_assert(offsetof(BodyState, vec0_x) == 0x0);
static_assert(offsetof(BodyState, vec0_y) == 0x4);
static_assert(offsetof(BodyState, vec0_z) == 0x8);
static_assert(offsetof(BodyState, vec_c) == 0xc);
static_assert(offsetof(BodyState, inertia) == 0x18);
static_assert(offsetof(BodyState, inv_inertia) == 0x58);
static_assert(offsetof(BodyState, angle_x) == 0x98);
static_assert(offsetof(BodyState, angle_y) == 0x9c);
static_assert(offsetof(BodyState, angle_z) == 0xa0);
static_assert(offsetof(BodyState, orient) == 0xa4);
static_assert(offsetof(BodyState, pos_x) == 0xb4);
static_assert(offsetof(BodyState, pos_y) == 0xb8);
static_assert(offsetof(BodyState, pos_z) == 0xbc);
static_assert(offsetof(BodyState, cell_pos) == 0xc0);
static_assert(offsetof(BodyState, zone) == 0xcc);
static_assert(offsetof(BodyState, sub) == 0xd0);
static_assert(offsetof(BodyState, on_ground) == 0xd4);
static_assert(offsetof(BodyState, cell) == 0xd6);
static_assert(offsetof(BodyState, move_target) == 0xda);
static_assert(offsetof(BodyState, move_start) == 0xe6);
static_assert(offsetof(BodyState, move_len) == 0xea);
static_assert(offsetof(BodyState, move_end) == 0xee);
static_assert(offsetof(BodyState, move_flag) == 0xf2);
static_assert(offsetof(BodyState, goal) == 0xf4);
static_assert(offsetof(BodyState, goal_start) == 0x100);
static_assert(offsetof(BodyState, goal_len) == 0x104);
static_assert(offsetof(BodyState, goal_end) == 0x108);
static_assert(offsetof(BodyState, goal_flag) == 0x10c);
static_assert(offsetof(BodyState, vel) == 0x10e);
static_assert(offsetof(BodyState, vel_start) == 0x11a);
static_assert(offsetof(BodyState, vel_len) == 0x11e);
static_assert(offsetof(BodyState, vel_end) == 0x122);
static_assert(offsetof(BodyState, vel_flag) == 0x126);
static_assert(offsetof(BodyState, f32_128) == 0x128);
static_assert(offsetof(BodyState, u16_134) == 0x134);
#endif

}  // namespace joan::orig
