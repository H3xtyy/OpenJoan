// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Order0c_a class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "map_object.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Order0c_a : MapObject {
#include "order0c_a_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Order0c_a) == 0xce);
static_assert(offsetof(Order0c_a, amount) == 0xc2);
static_assert(offsetof(Order0c_a, u32_c6) == 0xc6);
static_assert(offsetof(Order0c_a, u32_ca) == 0xca);
#endif

}  // namespace joan::orig
