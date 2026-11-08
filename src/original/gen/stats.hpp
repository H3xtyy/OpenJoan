// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Stats class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct Stats {
#include "stats_fields.inc"
#include "stats_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Stats, i32_35) == 0x35);
static_assert(offsetof(Stats, u32_42) == 0x42);
static_assert(offsetof(Stats, u32_46) == 0x46);
static_assert(offsetof(Stats, u32_4a) == 0x4a);
static_assert(offsetof(Stats, u32_4e) == 0x4e);
static_assert(offsetof(Stats, u32_52) == 0x52);
static_assert(offsetof(Stats, u32_56) == 0x56);
static_assert(offsetof(Stats, u32_21b) == 0x21b);
static_assert(offsetof(Stats, arr_247) == 0x247);
static_assert(offsetof(Stats, arr_273) == 0x273);
static_assert(offsetof(Stats, arr_29f) == 0x29f);
static_assert(offsetof(Stats, u32_2cb) == 0x2cb);
static_assert(offsetof(Stats, u32_2cf) == 0x2cf);
static_assert(offsetof(Stats, ptr_2d3) == 0x2d3);
static_assert(offsetof(Stats, ptr_2d7) == 0x2d7);
static_assert(offsetof(Stats, f32_1375) == 0x1375);
static_assert(offsetof(Stats, f32_148d) == 0x148d);
static_assert(offsetof(Stats, f32_1495) == 0x1495);
#endif

}  // namespace joan::orig
