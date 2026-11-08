// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RecruitFacility class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "facility.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct RecruitFacility : Facility {
#include "recruit_facility_fields.inc"
#include "recruit_facility_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(RecruitFacility, units) == 0x79);
static_assert(offsetof(RecruitFacility, group) == 0x81);
static_assert(offsetof(RecruitFacility, orders) == 0x89);
static_assert(offsetof(RecruitFacility, place) == 0x91);
#endif

}  // namespace joan::orig
