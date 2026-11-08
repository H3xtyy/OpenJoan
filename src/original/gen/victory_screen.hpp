// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VictoryScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VictoryScreen {
#include "victory_screen_fields.inc"
#include "victory_screen_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(VictoryScreen, tex_a) == 0xd18);
static_assert(offsetof(VictoryScreen, tex_b) == 0xd1c);
static_assert(offsetof(VictoryScreen, p_d2e) == 0xd2e);
static_assert(offsetof(VictoryScreen, u8_d32) == 0xd32);
#endif

}  // namespace joan::orig
