// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitTableEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitTableEntry {
#include "unit_table_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UnitTableEntry) == 0x4);
static_assert(offsetof(UnitTableEntry, id) == 0x0);
static_assert(offsetof(UnitTableEntry, def) == 0x2);
#endif

}  // namespace joan::orig
