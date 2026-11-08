// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PathRoute class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "list.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PathRoute : List {
#include "path_route_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(PathRoute, start) == 0x1d);
static_assert(offsetof(PathRoute, start_i_a) == 0x29);
static_assert(offsetof(PathRoute, start_i_b) == 0x35);
#endif

}  // namespace joan::orig
