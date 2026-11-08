// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PlaneHolder class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Plane;

#pragma pack(push, 1)
struct PlaneHolder {
#include "plane_holder_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(PlaneHolder, plane) == 0x24);
#endif

}  // namespace joan::orig
