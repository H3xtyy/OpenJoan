// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Order0b class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "map_object.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Order0b : MapObject {
#include "order0b_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Order0b) == 0xcd);
static_assert(offsetof(Order0b, drop_item) == 0xc2);
static_assert(offsetof(Order0b, drop_count) == 0xc4);
static_assert(offsetof(Order0b, u8_c6) == 0xc6);
static_assert(offsetof(Order0b, state) == 0xc8);
static_assert(offsetof(Order0b, active) == 0xc9);
#endif

}  // namespace joan::orig
