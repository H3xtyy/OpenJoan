// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Effect class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Scene;
struct Trail;

#pragma pack(push, 1)
struct Effect {
#include "effect_fields.inc"
#include "effect_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Effect, index) == 0x0);
static_assert(offsetof(Effect, u32_4) == 0x4);
static_assert(offsetof(Effect, u32_8) == 0x8);
static_assert(offsetof(Effect, u32_10) == 0x10);
static_assert(offsetof(Effect, state) == 0x1a);
static_assert(offsetof(Effect, u16_24) == 0x24);
static_assert(offsetof(Effect, u32_2a) == 0x2a);
static_assert(offsetof(Effect, u32_2e) == 0x2e);
static_assert(offsetof(Effect, trail) == 0x66);
#endif

}  // namespace joan::orig
