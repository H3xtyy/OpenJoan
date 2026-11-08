// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's DayLight class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "day_light_phase.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct DayLight {
#include "day_light_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(DayLight, phase0) == 0x3b);
static_assert(offsetof(DayLight, phase1) == 0x7b);
#endif

}  // namespace joan::orig
