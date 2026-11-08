// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PathPoint class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PathPoint {
#include "path_point_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PathPoint) == 0x24);
static_assert(offsetof(PathPoint, pos) == 0x0);
static_assert(offsetof(PathPoint, a) == 0xc);
static_assert(offsetof(PathPoint, b) == 0x18);
#endif

}  // namespace joan::orig
