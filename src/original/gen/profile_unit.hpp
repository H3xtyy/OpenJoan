// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ProfileUnit class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Unit;

#pragma pack(push, 1)
struct ProfileUnit {
#include "profile_unit_fields.inc"
#include "profile_unit_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ProfileUnit) == 0x77);
static_assert(offsetof(ProfileUnit, name) == 0x0);
static_assert(offsetof(ProfileUnit, leader) == 0x21);
static_assert(offsetof(ProfileUnit, member_count) == 0x25);
static_assert(offsetof(ProfileUnit, members) == 0x27);
#endif

}  // namespace joan::orig
