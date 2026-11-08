// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamTracks class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct CamTrack;

#pragma pack(push, 1)
struct CamTracks {
#include "cam_tracks_fields.inc"
#include "cam_tracks_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CamTracks, tracks) == 0x0);
static_assert(offsetof(CamTracks, count) == 0x4);
#endif

}  // namespace joan::orig
