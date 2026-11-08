// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UpgradeRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct UnitTable;
struct UpgradeItem;

#pragma pack(push, 1)
struct UpgradeRes {
#include "upgrade_res_fields.inc"
#include "upgrade_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UpgradeRes) == 0x11);
static_assert(offsetof(UpgradeRes, item_count) == 0x0);
static_assert(offsetof(UpgradeRes, items) == 0x4);
static_assert(offsetof(UpgradeRes, unit_count) == 0x8);
static_assert(offsetof(UpgradeRes, units) == 0xc);
static_assert(offsetof(UpgradeRes, loaded) == 0x10);
#endif

}  // namespace joan::orig
