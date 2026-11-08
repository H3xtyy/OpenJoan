// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ShadowPlane class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "plane.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ShadowPlane : Plane {
#include "shadow_plane_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ShadowPlane) == 0x11c);
#endif

}  // namespace joan::orig
