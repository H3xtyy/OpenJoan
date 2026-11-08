// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PointList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"
#include "path_point.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PointList : DynArray {
#include "point_list_fields.inc"
#include "point_list_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PointList) == 0x41);
static_assert(offsetof(PointList, start) == 0x1d);
#endif

}  // namespace joan::orig
