// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Profile class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ArrU16;
struct ProfileUnit;

#pragma pack(push, 1)
struct Profile {
#include "profile_fields.inc"
#include "profile_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Profile) == 0x48);
static_assert(offsetof(Profile, name) == 0x0);
static_assert(offsetof(Profile, unit_count) == 0x20);
static_assert(offsetof(Profile, units) == 0x24);
static_assert(offsetof(Profile, list_a_count) == 0x28);
static_assert(offsetof(Profile, list_a) == 0x2c);
static_assert(offsetof(Profile, list_b_count) == 0x30);
static_assert(offsetof(Profile, list_b) == 0x34);
static_assert(offsetof(Profile, u32_38) == 0x38);
static_assert(offsetof(Profile, u32_3c) == 0x3c);
static_assert(offsetof(Profile, i32_40) == 0x40);
static_assert(offsetof(Profile, i32_44) == 0x44);
#endif

}  // namespace joan::orig
