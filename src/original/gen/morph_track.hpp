// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MorphTrack class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MorphTrack {
#include "morph_track_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MorphTrack) == 0x8);
static_assert(offsetof(MorphTrack, key_count) == 0x0);
static_assert(offsetof(MorphTrack, keys) == 0x4);
#endif

}  // namespace joan::orig
