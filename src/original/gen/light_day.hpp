// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightDay class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LightDay {
#include "light_day_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LightDay) == 0xa);
static_assert(offsetof(LightDay, preset) == 0x0);
static_assert(offsetof(LightDay, time_from) == 0x2);
static_assert(offsetof(LightDay, time_to) == 0x6);
#endif

}  // namespace joan::orig
