// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SpIndex class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SpIndex {
#include "sp_index_fields.inc"
#include "sp_index_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SpIndex) == 0x46);
static_assert(offsetof(SpIndex, cell_size) == 0x0);
static_assert(offsetof(SpIndex, width) == 0x4);
static_assert(offsetof(SpIndex, height) == 0x8);
static_assert(offsetof(SpIndex, cols) == 0xc);
static_assert(offsetof(SpIndex, rows) == 0x10);
static_assert(offsetof(SpIndex, cells) == 0x14);
static_assert(offsetof(SpIndex, entries) == 0x18);
#endif

}  // namespace joan::orig
