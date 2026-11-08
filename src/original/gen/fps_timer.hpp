// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's FpsTimer class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct FpsTimer {
#include "fps_timer_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(FpsTimer, max_lag_frames) == 0x0);
static_assert(offsetof(FpsTimer, frame_ms) == 0x8);
static_assert(offsetof(FpsTimer, frame_ms_alt) == 0x10);
static_assert(offsetof(FpsTimer, use_alt) == 0x14);
static_assert(offsetof(FpsTimer, last_time) == 0x18);
static_assert(offsetof(FpsTimer, lag) == 0x1c);
static_assert(offsetof(FpsTimer, paused) == 0x20);
static_assert(offsetof(FpsTimer, skip_frame) == 0x2c);
static_assert(offsetof(FpsTimer, fps_time) == 0x38);
static_assert(offsetof(FpsTimer, frame_count) == 0x3c);
static_assert(offsetof(FpsTimer, fps) == 0x40);
#endif

}  // namespace joan::orig
