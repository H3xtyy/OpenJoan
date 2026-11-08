// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Plane4 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Plane4 {
#include "plane4_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Plane4) == 0x10);
static_assert(offsetof(Plane4, a) == 0x0);
static_assert(offsetof(Plane4, b) == 0x4);
static_assert(offsetof(Plane4, c) == 0x8);
static_assert(offsetof(Plane4, d) == 0xc);
#endif

}  // namespace joan::orig
