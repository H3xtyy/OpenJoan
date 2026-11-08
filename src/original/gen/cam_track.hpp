// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamTrack class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "cam_key.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CamTrack {
#include "cam_track_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CamTrack) == 0xaca);
static_assert(offsetof(CamTrack, id) == 0x0);
static_assert(offsetof(CamTrack, kind) == 0x2);
static_assert(offsetof(CamTrack, name) == 0x6);
static_assert(offsetof(CamTrack, box) == 0x48);
static_assert(offsetof(CamTrack, view_distance) == 0x60);
static_assert(offsetof(CamTrack, yaw_deg) == 0x62);
static_assert(offsetof(CamTrack, pitch_deg) == 0x66);
static_assert(offsetof(CamTrack, per_key_angles) == 0x6a);
static_assert(offsetof(CamTrack, scale_pitch) == 0x6c);
static_assert(offsetof(CamTrack, face_unit) == 0x6e);
static_assert(offsetof(CamTrack, free_pitch) == 0x70);
static_assert(offsetof(CamTrack, key_count) == 0x72);
static_assert(offsetof(CamTrack, keys) == 0x76);
static_assert(offsetof(CamTrack, rail_step) == 0xac6);
#endif

}  // namespace joan::orig
