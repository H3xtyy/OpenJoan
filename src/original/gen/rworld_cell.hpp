// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RWorldCell class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PathSpace;

#pragma pack(push, 1)
struct RWorldCell {
#include "rworld_cell_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(RWorldCell) == 0x34);
static_assert(offsetof(RWorldCell, key_a) == 0x0);
static_assert(offsetof(RWorldCell, key_b) == 0x4);
static_assert(offsetof(RWorldCell, x) == 0x8);
static_assert(offsetof(RWorldCell, y) == 0xc);
static_assert(offsetof(RWorldCell, fx) == 0x10);
static_assert(offsetof(RWorldCell, fy) == 0x14);
static_assert(offsetof(RWorldCell, u32_18) == 0x18);
static_assert(offsetof(RWorldCell, box) == 0x1c);
#endif

}  // namespace joan::orig
