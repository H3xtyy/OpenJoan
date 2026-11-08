// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Grid class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct Font2d;
struct GridRows;

#pragma pack(push, 1)
struct Grid : DynArrayB {
#include "grid_fields.inc"
#include "grid_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Grid) == 0x3e);
static_assert(offsetof(Grid, cells) == 0x2e);
static_assert(offsetof(Grid, rows) == 0x32);
static_assert(offsetof(Grid, cell_size) == 0x36);
static_assert(offsetof(Grid, cols) == 0x3a);
#endif

}  // namespace joan::orig
