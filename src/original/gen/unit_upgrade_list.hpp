// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitUpgradeList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitUpgradeList {
#include "unit_upgrade_list_fields.inc"
#include "unit_upgrade_list_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(UnitUpgradeList, slot_count) == 0x2c);
#endif

}  // namespace joan::orig
