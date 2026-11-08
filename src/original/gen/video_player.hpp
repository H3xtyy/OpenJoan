// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VideoPlayer class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VideoPlayer {
#include "video_player_fields.inc"
#include "video_player_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(VideoPlayer, tex) == 0x0);
static_assert(offsetof(VideoPlayer, sheet) == 0x4);
static_assert(offsetof(VideoPlayer, stage) == 0x8);
static_assert(offsetof(VideoPlayer, u8_9) == 0x9);
static_assert(offsetof(VideoPlayer, playing) == 0xa);
static_assert(offsetof(VideoPlayer, start_time) == 0xb);
#endif

}  // namespace joan::orig
