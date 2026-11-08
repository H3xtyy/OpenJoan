// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Vec3f class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Vec3f {
#include "vec3f_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Vec3f) == 0xc);
static_assert(offsetof(Vec3f, x) == 0x0);
static_assert(offsetof(Vec3f, y) == 0x4);
static_assert(offsetof(Vec3f, z) == 0x8);
#endif

}  // namespace joan::orig
