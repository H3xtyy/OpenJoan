// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Collisions class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "rworld_cell.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct Collisions {
#include "collisions_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Collisions) == 0x584);
static_assert(offsetof(Collisions, objects) == 0x0);
static_assert(offsetof(Collisions, zones) == 0x4);
static_assert(offsetof(Collisions, zone_count) == 0x580);
#endif

}  // namespace joan::orig
