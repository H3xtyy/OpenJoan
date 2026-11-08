// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamTrackNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CamTrackNode {
#include "cam_track_node_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CamTrackNode) == 0x11);
static_assert(offsetof(CamTrackNode, u32_0) == 0x0);
static_assert(offsetof(CamTrackNode, prev) == 0x4);
static_assert(offsetof(CamTrackNode, next) == 0x8);
static_assert(offsetof(CamTrackNode, marked) == 0xc);
static_assert(offsetof(CamTrackNode, unit) == 0xd);
#endif

}  // namespace joan::orig
