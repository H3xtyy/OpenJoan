// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamZones class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct CamRes;

#pragma pack(push, 1)
struct CamZones {
#include "cam_zones_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CamZones) == 0x4c);
static_assert(offsetof(CamZones, unit_id) == 0x0);
static_assert(offsetof(CamZones, origin) == 0x6);
static_assert(offsetof(CamZones, tracks) == 0x12);
static_assert(offsetof(CamZones, enabled) == 0x16);
static_assert(offsetof(CamZones, current_zone) == 0x18);
static_assert(offsetof(CamZones, rail_segment) == 0x1c);
static_assert(offsetof(CamZones, rail_segment_b) == 0x20);
static_assert(offsetof(CamZones, rail_param) == 0x24);
static_assert(offsetof(CamZones, last_position) == 0x28);
static_assert(offsetof(CamZones, height_offset) == 0x34);
static_assert(offsetof(CamZones, script_start) == 0x38);
static_assert(offsetof(CamZones, script_end) == 0x3c);
static_assert(offsetof(CamZones, key_base) == 0x40);
static_assert(offsetof(CamZones, looping) == 0x44);
static_assert(offsetof(CamZones, keys_per_ms) == 0x46);
static_assert(offsetof(CamZones, hand_back) == 0x4a);
#endif

}  // namespace joan::orig
