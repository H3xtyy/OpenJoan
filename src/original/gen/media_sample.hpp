// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MediaSample class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IDirectMusicLoader8;
struct IDirectMusicPerformance8;
struct IDirectMusicSegment8;
struct IDirectMusicSegmentState8;

#pragma pack(push, 1)
struct MediaSample {
#include "media_sample_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MediaSample) == 0x18);
static_assert(offsetof(MediaSample, vtable) == 0x0);
static_assert(offsetof(MediaSample, performance) == 0x4);
static_assert(offsetof(MediaSample, loader) == 0x8);
static_assert(offsetof(MediaSample, segment) == 0xc);
static_assert(offsetof(MediaSample, segment_state) == 0x10);
static_assert(offsetof(MediaSample, downloaded) == 0x14);
#endif

}  // namespace joan::orig
