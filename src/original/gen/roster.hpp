// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Roster class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Roster {
#include "roster_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Roster, u8_0) == 0x0);
static_assert(offsetof(Roster, u16_427f) == 0x427f);
static_assert(offsetof(Roster, u16_4460) == 0x4460);
static_assert(offsetof(Roster, u16_4472) == 0x4472);
static_assert(offsetof(Roster, u16_44d8) == 0x44d8);
static_assert(offsetof(Roster, u16_44e0) == 0x44e0);
static_assert(offsetof(Roster, u16_44e2) == 0x44e2);
static_assert(offsetof(Roster, u16_44e4) == 0x44e4);
#endif

}  // namespace joan::orig
