// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Range class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Range {
#include "range_fields.inc"
#include "range_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Range) == 0x18);
static_assert(offsetof(Range, i32_0) == 0x0);
static_assert(offsetof(Range, u32_4) == 0x4);
static_assert(offsetof(Range, current) == 0x8);
static_assert(offsetof(Range, end) == 0xc);
static_assert(offsetof(Range, u32_10) == 0x10);
static_assert(offsetof(Range, u32_14) == 0x14);
#endif

}  // namespace joan::orig
