// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Timer class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Timer {
#include "timer_fields.inc"
#include "timer_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Timer, initialized) == 0x0);
static_assert(offsetof(Timer, rate) == 0x4);
static_assert(offsetof(Timer, rate_ms) == 0x8);
static_assert(offsetof(Timer, rate2) == 0xc);
static_assert(offsetof(Timer, rate2_ms) == 0x10);
static_assert(offsetof(Timer, paused) == 0x14);
static_assert(offsetof(Timer, time_offset) == 0x18);
static_assert(offsetof(Timer, lag_ms) == 0x1c);
static_assert(offsetof(Timer, lag_frames) == 0x20);
static_assert(offsetof(Timer, rate_ticks) == 0x24);
static_assert(offsetof(Timer, on_time_ticks) == 0x28);
static_assert(offsetof(Timer, skip_frame) == 0x2c);
static_assert(offsetof(Timer, pause_marked) == 0x30);
static_assert(offsetof(Timer, pause_start) == 0x34);
static_assert(offsetof(Timer, fps_time) == 0x38);
static_assert(offsetof(Timer, frame_count) == 0x3c);
static_assert(offsetof(Timer, fps) == 0x40);
#endif

}  // namespace joan::orig
