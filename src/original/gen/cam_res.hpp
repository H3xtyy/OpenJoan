// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "cam_tracks.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CamRes : CamTracks {
#include "cam_res_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CamRes) == 0x9);
static_assert(offsetof(CamRes, loaded) == 0x8);
#endif

}  // namespace joan::orig
