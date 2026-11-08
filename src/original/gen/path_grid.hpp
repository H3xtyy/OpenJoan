// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PathGrid class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct TrackNode;

#pragma pack(push, 1)
struct PathGrid {
#include "path_grid_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(PathGrid, u32_46) == 0x46);
static_assert(offsetof(PathGrid, buffer) == 0x4a);
static_assert(offsetof(PathGrid, u32_4e) == 0x4e);
#endif

}  // namespace joan::orig
