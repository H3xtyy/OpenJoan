// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's HeightMap class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "height_grid.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct HeightMap : HeightGrid {
#include "height_map_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(HeightMap) == 0x14);
static_assert(offsetof(HeightMap, roughness) == 0x8);
static_assert(offsetof(HeightMap, scale) == 0xc);
static_assert(offsetof(HeightMap, depth) == 0x10);
#endif

}  // namespace joan::orig
