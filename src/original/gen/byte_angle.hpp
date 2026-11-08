// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ByteAngle class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ByteAngle {
#include "byte_angle_fields.inc"
#include "byte_angle_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ByteAngle) == 0x1);
static_assert(offsetof(ByteAngle, value) == 0x0);
#endif

}  // namespace joan::orig
