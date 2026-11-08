// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SkyBox class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "plane.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SkyBox : Plane {
#include "sky_box_fields.inc"
#include "sky_box_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SkyBox) == 0x124);
static_assert(offsetof(SkyBox, cube) == 0x11c);
static_assert(offsetof(SkyBox, u8_11d) == 0x11d);
static_assert(offsetof(SkyBox, size) == 0x120);
#endif

}  // namespace joan::orig
