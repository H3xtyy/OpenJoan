// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PathBox class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "point_list.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PathBox : PointList {
#include "path_box_fields.inc"
#include "path_box_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PathBox) == 0x41);
#endif

}  // namespace joan::orig
