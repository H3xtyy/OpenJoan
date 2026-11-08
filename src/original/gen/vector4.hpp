// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Vector4 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Vector4 {
#include "vector4_fields.inc"
#include "vector4_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Vector4) == 0x10);
static_assert(offsetof(Vector4, x) == 0x0);
static_assert(offsetof(Vector4, y) == 0x4);
static_assert(offsetof(Vector4, z) == 0x8);
static_assert(offsetof(Vector4, w) == 0xc);
#endif

}  // namespace joan::orig
