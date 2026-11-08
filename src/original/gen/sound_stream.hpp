// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SoundStream class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Video;

#pragma pack(push, 1)
struct SoundStream {
#include "sound_stream_fields.inc"
#include "sound_stream_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SoundStream) == 0x24);
static_assert(offsetof(SoundStream, open) == 0x0);
static_assert(offsetof(SoundStream, track) == 0x4);
static_assert(offsetof(SoundStream, mode) == 0x8);
static_assert(offsetof(SoundStream, volume) == 0xc);
static_assert(offsetof(SoundStream, start_time) == 0x10);
static_assert(offsetof(SoundStream, u32_14) == 0x14);
static_assert(offsetof(SoundStream, fade_ms) == 0x18);
static_assert(offsetof(SoundStream, last_tick) == 0x1c);
static_assert(offsetof(SoundStream, video) == 0x20);
#endif

}  // namespace joan::orig
