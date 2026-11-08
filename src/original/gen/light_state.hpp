// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightState class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "light_preset.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LightState {
#include "light_state_fields.inc"
#include "light_state_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LightState) == 0x194);
static_assert(offsetof(LightState, preset) == 0x0);
static_assert(offsetof(LightState, fade) == 0x18c);
static_assert(offsetof(LightState, scale) == 0x190);
#endif

}  // namespace joan::orig
