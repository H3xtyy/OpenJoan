// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Tracks class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct TrackRes;

#pragma pack(push, 1)
struct Tracks {
#include "tracks_fields.inc"
#include "tracks_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Tracks) == 0x10);
static_assert(offsetof(Tracks, origin) == 0x0);
static_assert(offsetof(Tracks, res) == 0xc);
#endif

}  // namespace joan::orig
