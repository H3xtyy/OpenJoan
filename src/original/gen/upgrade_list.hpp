// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UpgradeList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct UpgradeList {
#include "upgrade_list_fields.inc"
#include "upgrade_list_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UpgradeList) == 0x4);
static_assert(offsetof(UpgradeList, list) == 0x0);
#endif

}  // namespace joan::orig
