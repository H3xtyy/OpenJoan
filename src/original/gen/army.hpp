// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Army class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct World;

#pragma pack(push, 1)
struct Army {
#include "army_fields.inc"
#include "army_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Army, u32_1d) == 0x1d);
static_assert(offsetof(Army, u8_58) == 0x58);
static_assert(offsetof(Army, world) == 0x1de);
#endif

}  // namespace joan::orig
