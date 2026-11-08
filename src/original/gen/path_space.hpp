// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PathSpace class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BlkArr;
struct BoxTree;
struct DynArray;
struct HeightGrid;
struct HeightGridPtrArray;
struct Triple;

#pragma pack(push, 1)
struct PathSpace {
#include "path_space_fields.inc"
#include "path_space_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PathSpace) == 0x38);
static_assert(offsetof(PathSpace, points) == 0x0);
static_assert(offsetof(PathSpace, pairs4) == 0x4);
static_assert(offsetof(PathSpace, pairs8) == 0x8);
static_assert(offsetof(PathSpace, links) == 0xc);
static_assert(offsetof(PathSpace, groups) == 0x10);
static_assert(offsetof(PathSpace, zones) == 0x14);
static_assert(offsetof(PathSpace, tree) == 0x18);
static_assert(offsetof(PathSpace, items) == 0x1c);
static_assert(offsetof(PathSpace, search_stamp) == 0x20);
static_assert(offsetof(PathSpace, cell_w) == 0x24);
static_assert(offsetof(PathSpace, cell_h) == 0x28);
static_assert(offsetof(PathSpace, map_id) == 0x2c);
static_assert(offsetof(PathSpace, grid) == 0x30);
static_assert(offsetof(PathSpace, sub_grids) == 0x34);
#endif

}  // namespace joan::orig
