// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Shot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "body_state.hpp"

namespace joan::orig {

struct Actor;

#pragma pack(push, 1)
struct Shot {
#include "shot_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Shot) == 0x2b2);
static_assert(offsetof(Shot, id) == 0x0);
static_assert(offsetof(Shot, type) == 0x4);
static_assert(offsetof(Shot, actor) == 0x8);
static_assert(offsetof(Shot, saved) == 0xc);
static_assert(offsetof(Shot, state) == 0x142);
static_assert(offsetof(Shot, idle_frames) == 0x278);
static_assert(offsetof(Shot, u32_27a) == 0x27a);
static_assert(offsetof(Shot, extent) == 0x27e);
static_assert(offsetof(Shot, tag) == 0x28a);
static_assert(offsetof(Shot, inertia_scale) == 0x28c);
static_assert(offsetof(Shot, hit) == 0x298);
static_assert(offsetof(Shot, hit_obj) == 0x29a);
static_assert(offsetof(Shot, blocked) == 0x29e);
static_assert(offsetof(Shot, u16_2a2) == 0x2a2);
static_assert(offsetof(Shot, i32_2a4) == 0x2a4);
static_assert(offsetof(Shot, i32_2a8) == 0x2a8);
static_assert(offsetof(Shot, u16_2ac) == 0x2ac);
static_assert(offsetof(Shot, active_link) == 0x2ae);
#endif

}  // namespace joan::orig
