// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's HeightGrid class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ArrU16;

#pragma pack(push, 1)
struct HeightGrid {
#include "height_grid_fields.inc"
#include "height_grid_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(HeightGrid, width) == 0x0);
static_assert(offsetof(HeightGrid, height) == 0x2);
static_assert(offsetof(HeightGrid, cells) == 0x4);
#endif

}  // namespace joan::orig
