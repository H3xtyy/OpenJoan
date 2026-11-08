// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's GridCell class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct GridCell {
#include "grid_cell_fields.inc"
#include "grid_cell_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(GridCell) == 0x34);
static_assert(offsetof(GridCell, layer0) == 0x0);
static_assert(offsetof(GridCell, layer1) == 0x4);
static_assert(offsetof(GridCell, origin_x) == 0x8);
static_assert(offsetof(GridCell, origin_y) == 0xc);
static_assert(offsetof(GridCell, centre_x) == 0x10);
static_assert(offsetof(GridCell, centre_y) == 0x14);
static_assert(offsetof(GridCell, box) == 0x1c);
#endif

}  // namespace joan::orig
