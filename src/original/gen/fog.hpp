// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Fog class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "tex_cache.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Fog {
#include "fog_fields.inc"
#include "fog_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Fog) == 0x50);
static_assert(offsetof(Fog, height_tex) == 0x0);
static_assert(offsetof(Fog, height_colour) == 0x10);
static_assert(offsetof(Fog, height_b) == 0x14);
static_assert(offsetof(Fog, height_a) == 0x18);
static_assert(offsetof(Fog, height_c) == 0x1c);
static_assert(offsetof(Fog, near_z) == 0x20);
static_assert(offsetof(Fog, far_z) == 0x24);
static_assert(offsetof(Fog, length) == 0x28);
static_assert(offsetof(Fog, f32_2c) == 0x2c);
static_assert(offsetof(Fog, range_tex) == 0x30);
static_assert(offsetof(Fog, colour) == 0x40);
static_assert(offsetof(Fog, dirty) == 0x44);
static_assert(offsetof(Fog, active) == 0x45);
static_assert(offsetof(Fog, start) == 0x48);
static_assert(offsetof(Fog, end) == 0x4c);
#endif

}  // namespace joan::orig
