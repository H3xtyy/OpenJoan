// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MusicTrack class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MusicTrack {
#include "music_track_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MusicTrack) == 0x68);
static_assert(offsetof(MusicTrack, id) == 0x0);
static_assert(offsetof(MusicTrack, name) == 0x2);
static_assert(offsetof(MusicTrack, file) == 0x13);
static_assert(offsetof(MusicTrack, range_from) == 0x54);
static_assert(offsetof(MusicTrack, range_to) == 0x58);
static_assert(offsetof(MusicTrack, level_min) == 0x5c);
static_assert(offsetof(MusicTrack, level_max) == 0x5e);
static_assert(offsetof(MusicTrack, owner) == 0x60);
static_assert(offsetof(MusicTrack, kind) == 0x64);
#endif

}  // namespace joan::orig
