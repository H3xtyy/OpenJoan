// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Flags class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Flags {
#include "flags_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Flags) == 0x5);
static_assert(offsetof(Flags, u16_0) == 0x0);
static_assert(offsetof(Flags, flag_a) == 0x2);
static_assert(offsetof(Flags, flag_b) == 0x3);
static_assert(offsetof(Flags, flag_c) == 0x4);
#endif

}  // namespace joan::orig
