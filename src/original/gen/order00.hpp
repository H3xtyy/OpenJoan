// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Order00 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "map_object.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Order00 : MapObject {
#include "order00_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Order00) == 0xc2);
#endif

}  // namespace joan::orig
