// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Sprite2D class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "camera.hpp"

namespace joan::orig {

struct Scene;

#pragma pack(push, 1)
struct Sprite2D : Camera {
#include "sprite2d_fields.inc"
#include "sprite2d_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Sprite2D, scene) == 0x4ca);
static_assert(offsetof(Sprite2D, count) == 0x4ce);
static_assert(offsetof(Sprite2D, u16_4d0) == 0x4d0);
#endif

}  // namespace joan::orig
