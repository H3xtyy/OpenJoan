// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UpgradeListEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UpgradeListEntry {
#include "upgrade_list_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UpgradeListEntry) == 0x8);
static_assert(offsetof(UpgradeListEntry, id) == 0x0);
static_assert(offsetof(UpgradeListEntry, max) == 0x2);
static_assert(offsetof(UpgradeListEntry, cur) == 0x4);
static_assert(offsetof(UpgradeListEntry, level) == 0x6);
#endif

}  // namespace joan::orig
