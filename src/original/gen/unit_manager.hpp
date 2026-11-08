// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitManager class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitManager : DynArrayB {
#include "unit_manager_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UnitManager) == 0x80);
static_assert(offsetof(UnitManager, i16_2e) == 0x2e);
static_assert(offsetof(UnitManager, counts_30) == 0x30);
static_assert(offsetof(UnitManager, counts_38) == 0x38);
static_assert(offsetof(UnitManager, counts_a) == 0x40);
static_assert(offsetof(UnitManager, counts_b) == 0x58);
static_assert(offsetof(UnitManager, select_models) == 0x70);
#endif

}  // namespace joan::orig
