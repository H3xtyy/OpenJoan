// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Rules class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Rules {
#include "rules_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Rules, u16_0) == 0x0);
static_assert(offsetof(Rules, f32_1371) == 0x1371);
static_assert(offsetof(Rules, f32_14dd) == 0x14dd);
static_assert(offsetof(Rules, f32_14e1) == 0x14e1);
static_assert(offsetof(Rules, f32_1521) == 0x1521);
#endif

}  // namespace joan::orig
