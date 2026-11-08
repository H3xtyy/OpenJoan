// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MusicStream class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MusicStream {
#include "music_stream_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MusicStream) == 0x30);
static_assert(offsetof(MusicStream, source) == 0x0);
static_assert(offsetof(MusicStream, file) == 0x2);
static_assert(offsetof(MusicStream, file_start) == 0x6);
static_assert(offsetof(MusicStream, data) == 0xa);
static_assert(offsetof(MusicStream, data_pos) == 0xe);
static_assert(offsetof(MusicStream, width) == 0x12);
static_assert(offsetof(MusicStream, height) == 0x14);
static_assert(offsetof(MusicStream, frame_count) == 0x16);
static_assert(offsetof(MusicStream, frame) == 0x18);
static_assert(offsetof(MusicStream, user_buffers) == 0x1a);
static_assert(offsetof(MusicStream, pixels) == 0x1b);
static_assert(offsetof(MusicStream, palette) == 0x1f);
static_assert(offsetof(MusicStream, prev) == 0x23);
static_assert(offsetof(MusicStream, next) == 0x27);
static_assert(offsetof(MusicStream, loop) == 0x2b);
static_assert(offsetof(MusicStream, palette_cb) == 0x2c);
#endif

}  // namespace joan::orig
