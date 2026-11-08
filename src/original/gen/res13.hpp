// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Res13 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Res13 {
#include "res13_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Res13, u32_4) == 0x4);
static_assert(offsetof(Res13, u32_8) == 0x8);
static_assert(offsetof(Res13, u32_c) == 0xc);
static_assert(offsetof(Res13, u32_10) == 0x10);
#endif

}  // namespace joan::orig
