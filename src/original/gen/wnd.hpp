// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Wnd class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "sys_en_window.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Wnd : SysEnWindow {
#include "wnd_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Wnd) == 0x153);
static_assert(offsetof(Wnd, u8_20) == 0x20);
static_assert(offsetof(Wnd, frame_count) == 0x22);
static_assert(offsetof(Wnd, tick_count) == 0x26);
static_assert(offsetof(Wnd, real_time) == 0x2a);
static_assert(offsetof(Wnd, frame_ms) == 0x2e);
static_assert(offsetof(Wnd, game_time) == 0x32);
static_assert(offsetof(Wnd, game_dt) == 0x36);
static_assert(offsetof(Wnd, mode) == 0x3a);
static_assert(offsetof(Wnd, prev_mode) == 0x3b);
static_assert(offsetof(Wnd, next_mode) == 0x3c);
static_assert(offsetof(Wnd, u8_3d) == 0x3d);
static_assert(offsetof(Wnd, frame_scale) == 0x40);
static_assert(offsetof(Wnd, speed) == 0x44);
static_assert(offsetof(Wnd, i16_48) == 0x48);
static_assert(offsetof(Wnd, u8_4a) == 0x4a);
static_assert(offsetof(Wnd, cd_drive) == 0x4b);
static_assert(offsetof(Wnd, music_path) == 0x4c);
static_assert(offsetof(Wnd, u8_151) == 0x151);
static_assert(offsetof(Wnd, music_flag) == 0x152);
#endif

}  // namespace joan::orig
