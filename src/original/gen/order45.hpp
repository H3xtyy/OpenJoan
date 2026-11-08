// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Order45 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "map_object.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Order45 : MapObject {
#include "order45_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Order45) == 0x107);
static_assert(offsetof(Order45, target_name) == 0xc2);
static_assert(offsetof(Order45, target_id) == 0x103);
#endif

}  // namespace joan::orig
