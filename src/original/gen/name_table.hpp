// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's NameTable class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct ArrI32;

#pragma pack(push, 1)
struct NameTable : DynArray {
#include "name_table_fields.inc"
#include "name_table_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(NameTable, index_cap) == 0x1d);
static_assert(offsetof(NameTable, sorted) == 0x21);
static_assert(offsetof(NameTable, sorted_valid) == 0x25);
#endif

}  // namespace joan::orig
