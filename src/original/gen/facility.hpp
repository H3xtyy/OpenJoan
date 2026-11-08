// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Facility class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Place;

#pragma pack(push, 1)
struct Facility {
#include "facility_fields.inc"
#include "facility_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Facility) == 0x79);
static_assert(offsetof(Facility, vtable) == 0x0);
static_assert(offsetof(Facility, kind) == 0x4);
static_assert(offsetof(Facility, i16_6) == 0x6);
static_assert(offsetof(Facility, name) == 0x8);
static_assert(offsetof(Facility, name_end) == 0x28);
static_assert(offsetof(Facility, pos) == 0x29);
static_assert(offsetof(Facility, angle) == 0x35);
static_assert(offsetof(Facility, bounds) == 0x39);
static_assert(offsetof(Facility, n_entrances) == 0x51);
static_assert(offsetof(Facility, entrances) == 0x55);
static_assert(offsetof(Facility, owner) == 0x6d);
static_assert(offsetof(Facility, prev) == 0x71);
static_assert(offsetof(Facility, next) == 0x75);
#endif

}  // namespace joan::orig
