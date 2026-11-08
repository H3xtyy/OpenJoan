// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct World;

#pragma pack(push, 1)
struct UnitList : DynArrayB {
#include "unit_list_fields.inc"
#include "unit_list_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(UnitList, current_id) == 0x32);
static_assert(offsetof(UnitList, world) == 0x36);
#endif

}  // namespace joan::orig
