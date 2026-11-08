// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Blink class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Blink {
#include "blink_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Blink, state) == 0x0);
static_assert(offsetof(Blink, phase) == 0x1);
static_assert(offsetof(Blink, last_time) == 0x2);
static_assert(offsetof(Blink, enabled) == 0xa);
#endif

}  // namespace joan::orig
