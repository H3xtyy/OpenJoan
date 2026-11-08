// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SoundStreams3 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Video;

#pragma pack(push, 1)
struct SoundStreams3 {
#include "sound_streams3_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SoundStreams3) == 0x50);
static_assert(offsetof(SoundStreams3, open) == 0x0);
static_assert(offsetof(SoundStreams3, tag) == 0x4);
static_assert(offsetof(SoundStreams3, mode) == 0x10);
static_assert(offsetof(SoundStreams3, volume) == 0x1c);
static_assert(offsetof(SoundStreams3, u32_20) == 0x20);
static_assert(offsetof(SoundStreams3, start_time) == 0x28);
static_assert(offsetof(SoundStreams3, fade_time) == 0x34);
static_assert(offsetof(SoundStreams3, u32_40) == 0x40);
static_assert(offsetof(SoundStreams3, videos) == 0x44);
#endif

}  // namespace joan::orig
