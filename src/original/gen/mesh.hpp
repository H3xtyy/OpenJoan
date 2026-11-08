// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Mesh class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "plane.hpp"

namespace joan::orig {

struct Bone;

#pragma pack(push, 1)
struct Mesh : Plane {
#include "mesh_fields.inc"
#include "mesh_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Mesh) == 0x11c);
#endif

}  // namespace joan::orig
