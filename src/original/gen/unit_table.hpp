// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitTable class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct UnitTableEntry;

#pragma pack(push, 1)
struct UnitTable {
#include "unit_table_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UnitTable) == 0x35);
static_assert(offsetof(UnitTable, id) == 0x0);
static_assert(offsetof(UnitTable, name) == 0x2);
static_assert(offsetof(UnitTable, table_name) == 0x23);
static_assert(offsetof(UnitTable, count) == 0x2c);
static_assert(offsetof(UnitTable, entries) == 0x30);
static_assert(offsetof(UnitTable, loaded) == 0x34);
#endif

}  // namespace joan::orig
