// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Sound3D class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Sound3DBuffer;

#pragma pack(push, 1)
struct Sound3D {
#include "sound3d_fields.inc"
#include "sound3d_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Sound3D) == 0x53);
static_assert(offsetof(Sound3D, al_source) == 0x0);
static_assert(offsetof(Sound3D, pitch) == 0x4);
static_assert(offsetof(Sound3D, gain) == 0x8);
static_assert(offsetof(Sound3D, min_gain) == 0xc);
static_assert(offsetof(Sound3D, max_gain) == 0x10);
static_assert(offsetof(Sound3D, cone_outer_gain) == 0x14);
static_assert(offsetof(Sound3D, cone_inner_angle) == 0x18);
static_assert(offsetof(Sound3D, cone_outer_angle) == 0x1c);
static_assert(offsetof(Sound3D, max_distance) == 0x20);
static_assert(offsetof(Sound3D, reference_distance) == 0x24);
static_assert(offsetof(Sound3D, rolloff) == 0x28);
static_assert(offsetof(Sound3D, muted) == 0x2c);
static_assert(offsetof(Sound3D, looping) == 0x2d);
static_assert(offsetof(Sound3D, relative) == 0x2e);
static_assert(offsetof(Sound3D, position) == 0x2f);
static_assert(offsetof(Sound3D, velocity) == 0x3b);
static_assert(offsetof(Sound3D, direction) == 0x47);
#endif

}  // namespace joan::orig
