// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's NamedTable class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct WaveRes;

#pragma pack(push, 1)
struct NamedTable {
#include "named_table_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(NamedTable, table) == 0x0);
static_assert(offsetof(NamedTable, count) == 0x4);
#endif

}  // namespace joan::orig
