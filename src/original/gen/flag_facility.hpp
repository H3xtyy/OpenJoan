// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's FlagFacility class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "facility.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct FlagFacility : Facility {
#include "flag_facility_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(FlagFacility) == 0x99);
static_assert(offsetof(FlagFacility, list_79) == 0x79);
static_assert(offsetof(FlagFacility, list_7d) == 0x7d);
static_assert(offsetof(FlagFacility, group_a) == 0x81);
static_assert(offsetof(FlagFacility, group_b) == 0x85);
static_assert(offsetof(FlagFacility, list_89) == 0x89);
static_assert(offsetof(FlagFacility, recruit_value) == 0x8d);
static_assert(offsetof(FlagFacility, i16_91) == 0x91);
static_assert(offsetof(FlagFacility, i32_93) == 0x93);
static_assert(offsetof(FlagFacility, flag_97) == 0x97);
static_assert(offsetof(FlagFacility, flag_98) == 0x98);
#endif

}  // namespace joan::orig
