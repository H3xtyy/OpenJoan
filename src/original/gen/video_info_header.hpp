// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VideoInfoHeader class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "bitmap_info_header.hpp"
#include "rect.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VideoInfoHeader {
#include "video_info_header_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VideoInfoHeader) == 0x58);
static_assert(offsetof(VideoInfoHeader, source) == 0x0);
static_assert(offsetof(VideoInfoHeader, target) == 0x10);
static_assert(offsetof(VideoInfoHeader, bit_rate) == 0x20);
static_assert(offsetof(VideoInfoHeader, bit_error_rate) == 0x24);
static_assert(offsetof(VideoInfoHeader, avg_time_per_frame) == 0x28);
static_assert(offsetof(VideoInfoHeader, bmi) == 0x30);
#endif

}  // namespace joan::orig
