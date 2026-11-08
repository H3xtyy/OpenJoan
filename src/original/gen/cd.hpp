// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Cd class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Cd {
#include "cd_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Cd, u8_3) == 0x3);
static_assert(offsetof(Cd, u8_6) == 0x6);
static_assert(offsetof(Cd, u8_4b) == 0x4b);
#endif

}  // namespace joan::orig
