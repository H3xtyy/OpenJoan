// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Video class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IWMSyncReader;
struct Sound3D;
struct Texture2D;

#pragma pack(push, 1)
struct Video {
#include "video_fields.inc"
#include "video_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Video) == 0x30);
static_assert(offsetof(Video, open) == 0x0);
static_assert(offsetof(Video, texture) == 0x4);
static_assert(offsetof(Video, sound) == 0x8);
static_assert(offsetof(Video, video_stream) == 0xc);
static_assert(offsetof(Video, audio_stream) == 0xe);
static_assert(offsetof(Video, width) == 0x10);
static_assert(offsetof(Video, height) == 0x14);
static_assert(offsetof(Video, channels) == 0x18);
static_assert(offsetof(Video, bits) == 0x1a);
static_assert(offsetof(Video, sample_rate) == 0x1c);
static_assert(offsetof(Video, start_time) == 0x20);
static_assert(offsetof(Video, reader) == 0x24);
static_assert(offsetof(Video, video_time) == 0x28);
static_assert(offsetof(Video, audio_time) == 0x2c);
#endif

}  // namespace joan::orig
