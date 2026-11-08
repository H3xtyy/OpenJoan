// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Music class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IDirectMusicLoader8;
struct IDirectMusicPerformance8;

#pragma pack(push, 1)
struct Music {
#include "music_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Music, loader) == 0x0);
static_assert(offsetof(Music, performance) == 0x4);
static_assert(offsetof(Music, initialised) == 0x8);
#endif

}  // namespace joan::orig
