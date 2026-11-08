// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Zones class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Zones {
#include "zones_fields.inc"
#include "zones_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Zones, bytes_0) == 0x0);
static_assert(offsetof(Zones, i32_1d) == 0x1d);
static_assert(offsetof(Zones, u32_21) == 0x21);
static_assert(offsetof(Zones, u32_74) == 0x74);
static_assert(offsetof(Zones, u32_78) == 0x78);
#endif

}  // namespace joan::orig
