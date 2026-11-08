// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Member class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Member {
#include "member_fields.inc"
#include "member_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Member, u16_0) == 0x0);
static_assert(offsetof(Member, u16_2) == 0x2);
static_assert(offsetof(Member, u8_2f) == 0x2f);
static_assert(offsetof(Member, u16_31) == 0x31);
static_assert(offsetof(Member, u16_37) == 0x37);
#endif

}  // namespace joan::orig
