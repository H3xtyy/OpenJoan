// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Missile class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Trail;

#pragma pack(push, 1)
struct Missile {
#include "missile_fields.inc"
#include "missile_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Missile, index) == 0x0);
static_assert(offsetof(Missile, model_id) == 0x4);
static_assert(offsetof(Missile, object_id) == 0x8);
static_assert(offsetof(Missile, state) == 0xc);
static_assert(offsetof(Missile, sound_handle) == 0x10);
static_assert(offsetof(Missile, launch_time) == 0x14);
static_assert(offsetof(Missile, origin) == 0x18);
static_assert(offsetof(Missile, stopped) == 0x24);
static_assert(offsetof(Missile, expired) == 0x26);
static_assert(offsetof(Missile, shooter_unit_id) == 0x2a);
static_assert(offsetof(Missile, shooter_record) == 0x2e);
static_assert(offsetof(Missile, lifetime_ms) == 0x32);
static_assert(offsetof(Missile, max_range) == 0x36);
static_assert(offsetof(Missile, u32_3a) == 0x3a);
static_assert(offsetof(Missile, u32_3e) == 0x3e);
static_assert(offsetof(Missile, u32_42) == 0x42);
static_assert(offsetof(Missile, u32_46) == 0x46);
static_assert(offsetof(Missile, straight_speed) == 0x4a);
static_assert(offsetof(Missile, u32_4e) == 0x4e);
static_assert(offsetof(Missile, u32_52) == 0x52);
static_assert(offsetof(Missile, shot_start) == 0x56);
static_assert(offsetof(Missile, shot_end) == 0x5a);
static_assert(offsetof(Missile, hit_time) == 0x5e);
static_assert(offsetof(Missile, hit_object) == 0x62);
static_assert(offsetof(Missile, trail) == 0x66);
static_assert(offsetof(Missile, hit_done) == 0x6a);
#endif

}  // namespace joan::orig
