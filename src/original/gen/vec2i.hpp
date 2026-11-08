// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Vec2i class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Vec2i {
#include "vec2i_fields.inc"
#include "vec2i_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Vec2i) == 0x8);
static_assert(offsetof(Vec2i, x) == 0x0);
static_assert(offsetof(Vec2i, y) == 0x4);
#endif

}  // namespace joan::orig
