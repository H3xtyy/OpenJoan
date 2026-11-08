// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Weather class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"
#include "effect_state.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Weather {
#include "weather_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Weather) == 0x1c8);
static_assert(offsetof(Weather, initialized) == 0x0);
static_assert(offsetof(Weather, slot_flags) == 0x1);
static_assert(offsetof(Weather, rain_voice) == 0x4);
static_assert(offsetof(Weather, last_spawn) == 0x8);
static_assert(offsetof(Weather, u32_c) == 0xc);
static_assert(offsetof(Weather, u32_10) == 0x10);
static_assert(offsetof(Weather, model_res) == 0x14);
static_assert(offsetof(Weather, anim_res) == 0x18);
static_assert(offsetof(Weather, i32_1c) == 0x1c);
static_assert(offsetof(Weather, active) == 0x20);
static_assert(offsetof(Weather, u32_22) == 0x22);
static_assert(offsetof(Weather, bolts) == 0x26);
static_assert(offsetof(Weather, thunder) == 0x43);
static_assert(offsetof(Weather, rain) == 0x60);
static_assert(offsetof(Weather, snow) == 0xd8);
static_assert(offsetof(Weather, ripple) == 0x150);
#endif

}  // namespace joan::orig
