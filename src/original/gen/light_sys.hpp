// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightSys class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "lens_flares.hpp"
#include "light_preset.hpp"
#include "light_state.hpp"

namespace joan::orig {

struct Billboard;
struct Light;
struct LightDay;
struct Scene;

#pragma pack(push, 1)
struct LightSys {
#include "light_sys_fields.inc"
#include "light_sys_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(LightSys, set_count) == 0x0);
static_assert(offsetof(LightSys, sets) == 0x2);
static_assert(offsetof(LightSys, special_count) == 0x6);
static_assert(offsetof(LightSys, specials) == 0x8);
static_assert(offsetof(LightSys, current) == 0xc);
static_assert(offsetof(LightSys, state_a) == 0x198);
static_assert(offsetof(LightSys, state_b) == 0x32c);
static_assert(offsetof(LightSys, time) == 0x4c0);
static_assert(offsetof(LightSys, day_count) == 0x4c4);
static_assert(offsetof(LightSys, day_index) == 0x4c6);
static_assert(offsetof(LightSys, days) == 0x4c8);
static_assert(offsetof(LightSys, sky_name_b) == 0x4cc);
static_assert(offsetof(LightSys, sky_name_c) == 0x50c);
static_assert(offsetof(LightSys, sky_name_a) == 0x54c);
static_assert(offsetof(LightSys, event_a_chance) == 0x58c);
static_assert(offsetof(LightSys, event_a_rate) == 0x590);
static_assert(offsetof(LightSys, event_a_time) == 0x594);
static_assert(offsetof(LightSys, event_a_state) == 0x598);
static_assert(offsetof(LightSys, event_b_chance) == 0x59c);
static_assert(offsetof(LightSys, event_b_rate) == 0x5a0);
static_assert(offsetof(LightSys, event_b_time) == 0x5a4);
static_assert(offsetof(LightSys, event_b_state) == 0x5a8);
static_assert(offsetof(LightSys, moon) == 0x5ac);
static_assert(offsetof(LightSys, light_count) == 0x5b0);
static_assert(offsetof(LightSys, lights) == 0x5b2);
static_assert(offsetof(LightSys, flares) == 0x5d6);
#endif

}  // namespace joan::orig
