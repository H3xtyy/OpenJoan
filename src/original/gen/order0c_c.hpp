// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Order0c_c class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "map_object.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Order0c_c : MapObject {
#include "order0c_c_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Order0c_c) == 0xce);
static_assert(offsetof(Order0c_c, durability) == 0xc2);
static_assert(offsetof(Order0c_c, u32_c6) == 0xc6);
#endif

}  // namespace joan::orig
