// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Light class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../colour.hpp"

namespace joan::orig {

struct Bone;
struct Children;

#pragma pack(push, 1)
struct Light {
#include "light_fields.inc"
#include "light_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Light) == 0xce);
static_assert(offsetof(Light, type) == 0x0);
static_assert(offsetof(Light, bone_) == 0x4);
static_assert(offsetof(Light, children) == 0x8);
static_assert(offsetof(Light, position) == 0xc);
static_assert(offsetof(Light, orbit_a) == 0x18);
static_assert(offsetof(Light, orbit_b) == 0x1c);
static_assert(offsetof(Light, orbit_c) == 0x20);
static_assert(offsetof(Light, theta) == 0x24);
static_assert(offsetof(Light, phi) == 0x28);
static_assert(offsetof(Light, range) == 0x2c);
static_assert(offsetof(Light, f32_30) == 0x30);
static_assert(offsetof(Light, colour) == 0x34);
static_assert(offsetof(Light, bounds) == 0x38);
static_assert(offsetof(Light, attenuation0) == 0x48);
static_assert(offsetof(Light, attenuation1) == 0x4c);
static_assert(offsetof(Light, attenuation2) == 0x50);
static_assert(offsetof(Light, casts_shadows) == 0x54);
static_assert(offsetof(Light, active_) == 0x58);
static_assert(offsetof(Light, flag_59_) == 0x59);
static_assert(offsetof(Light, dirty) == 0x5a);
static_assert(offsetof(Light, d3d_light) == 0x5e);
static_assert(offsetof(Light, enabled) == 0xc6);
static_assert(offsetof(Light, u32_ca) == 0xca);
#endif

}  // namespace joan::orig
