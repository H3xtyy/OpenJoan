// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Fader class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct Fader {
#include "fader_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Fader) == 0x24);
static_assert(offsetof(Fader, state) == 0x0);
static_assert(offsetof(Fader, u32_4) == 0x4);
static_assert(offsetof(Fader, direction) == 0x8);
static_assert(offsetof(Fader, lo) == 0xc);
static_assert(offsetof(Fader, hi) == 0x10);
static_assert(offsetof(Fader, speed) == 0x14);
static_assert(offsetof(Fader, start_time) == 0x18);
static_assert(offsetof(Fader, duration) == 0x1c);
static_assert(offsetof(Fader, lights) == 0x20);
#endif

}  // namespace joan::orig
