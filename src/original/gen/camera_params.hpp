// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CameraParams class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CameraParams {
#include "camera_params_fields.inc"
#include "camera_params_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CameraParams) == 0x28);
static_assert(offsetof(CameraParams, target) == 0x4);
static_assert(offsetof(CameraParams, fov) == 0x10);
static_assert(offsetof(CameraParams, distance) == 0x1c);
#endif

}  // namespace joan::orig
