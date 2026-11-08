// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamKey class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CamKey {
#include "cam_key_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CamKey) == 0x2c);
static_assert(offsetof(CamKey, point) == 0x0);
static_assert(offsetof(CamKey, tangent) == 0xc);
static_assert(offsetof(CamKey, yaw) == 0x18);
static_assert(offsetof(CamKey, pitch) == 0x1c);
static_assert(offsetof(CamKey, roll) == 0x20);
static_assert(offsetof(CamKey, zoom) == 0x24);
static_assert(offsetof(CamKey, time) == 0x28);
#endif

}  // namespace joan::orig
