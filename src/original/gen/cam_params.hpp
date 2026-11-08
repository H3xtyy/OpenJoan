// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamParams class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CamParams {
#include "cam_params_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CamParams) == 0x28);
static_assert(offsetof(CamParams, projection) == 0x0);
static_assert(offsetof(CamParams, position) == 0x4);
static_assert(offsetof(CamParams, orbit) == 0x10);
static_assert(offsetof(CamParams, fov_tan) == 0x20);
static_assert(offsetof(CamParams, aspect) == 0x24);
#endif

}  // namespace joan::orig
