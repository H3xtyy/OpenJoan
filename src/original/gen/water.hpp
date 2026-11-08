// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Water class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Water {
#include "water_fields.inc"
#include "water_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Water, bytes_0) == 0x0);
static_assert(offsetof(Water, box) == 0x4);
static_assert(offsetof(Water, u32_1c) == 0x1c);
static_assert(offsetof(Water, u32_20) == 0x20);
static_assert(offsetof(Water, arr_24) == 0x24);
static_assert(offsetof(Water, u32_2c) == 0x2c);
static_assert(offsetof(Water, u32_30) == 0x30);
#endif

}  // namespace joan::orig
