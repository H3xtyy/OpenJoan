// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Quaternion class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Quaternion {
#include "quaternion_fields.inc"
#include "quaternion_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Quaternion) == 0x10);
static_assert(offsetof(Quaternion, x) == 0x0);
static_assert(offsetof(Quaternion, y) == 0x4);
static_assert(offsetof(Quaternion, z) == 0x8);
static_assert(offsetof(Quaternion, w) == 0xc);
#endif

}  // namespace joan::orig
