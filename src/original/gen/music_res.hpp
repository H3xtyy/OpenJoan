// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MusicRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct MusicTrack;

#pragma pack(push, 1)
struct MusicRes {
#include "music_res_fields.inc"
#include "music_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MusicRes) == 0x9);
static_assert(offsetof(MusicRes, count) == 0x0);
static_assert(offsetof(MusicRes, tracks) == 0x4);
static_assert(offsetof(MusicRes, loaded) == 0x8);
#endif

}  // namespace joan::orig
