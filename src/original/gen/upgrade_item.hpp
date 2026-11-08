// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UpgradeItem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UpgradeItem {
#include "upgrade_item_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UpgradeItem) == 0x64);
static_assert(offsetof(UpgradeItem, type) == 0x0);
static_assert(offsetof(UpgradeItem, id) == 0x2);
static_assert(offsetof(UpgradeItem, name) == 0x4);
static_assert(offsetof(UpgradeItem, description) == 0x25);
static_assert(offsetof(UpgradeItem, bonus_text) == 0x46);
static_assert(offsetof(UpgradeItem, cost) == 0x5b);
static_assert(offsetof(UpgradeItem, required) == 0x5d);
#endif

}  // namespace joan::orig
