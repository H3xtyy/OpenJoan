// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MouseZone class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MouseZone {
#include "mouse_zone_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(MouseZone, enabled) == 0x66c);
static_assert(offsetof(MouseZone, shape) == 0x66e);
static_assert(offsetof(MouseZone, tri_a) == 0x672);
static_assert(offsetof(MouseZone, tri_b) == 0x68e);
#endif

}  // namespace joan::orig
