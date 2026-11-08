// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightTrigger class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LightTrigger {
#include "light_trigger_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(LightTrigger, pending) == 0x1d);
static_assert(offsetof(LightTrigger, unit_id) == 0x22);
static_assert(offsetof(LightTrigger, toggle) == 0x26);
#endif

}  // namespace joan::orig
