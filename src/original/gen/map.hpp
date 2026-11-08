// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Map class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BmpRes;

#pragma pack(push, 1)
struct Map {
#include "map_fields.inc"
#include "map_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Map, bitmaps) == 0x0);
static_assert(offsetof(Map, i16_a) == 0xa);
static_assert(offsetof(Map, ptr_14) == 0x14);
static_assert(offsetof(Map, ptr_18) == 0x18);
static_assert(offsetof(Map, name) == 0x20);
static_assert(offsetof(Map, i16_40) == 0x40);
static_assert(offsetof(Map, res_a) == 0x44);
static_assert(offsetof(Map, res_b) == 0x48);
static_assert(offsetof(Map, node_count) == 0x72);
static_assert(offsetof(Map, node_cap) == 0x76);
static_assert(offsetof(Map, nodes) == 0x7a);
static_assert(offsetof(Map, node_aux) == 0x82);
static_assert(offsetof(Map, sector_count) == 0x86);
static_assert(offsetof(Map, sectors) == 0x8e);
static_assert(offsetof(Map, i32_b0a) == 0xb0a);
static_assert(offsetof(Map, i16_b1c) == 0xb1c);
static_assert(offsetof(Map, i16_b1e) == 0xb1e);
#endif

}  // namespace joan::orig
