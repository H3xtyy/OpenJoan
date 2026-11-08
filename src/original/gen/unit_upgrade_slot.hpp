// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitUpgradeSlot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitUpgradeSlot {
#include "unit_upgrade_slot_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(UnitUpgradeSlot, kind) == 0x0);
static_assert(offsetof(UnitUpgradeSlot, upgrade) == 0x2);
#endif

}  // namespace joan::orig
