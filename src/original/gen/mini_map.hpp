// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MiniMap class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BmpRes;
struct MiniRes;
struct Nation;
struct Texture2D;
struct Unit;

#pragma pack(push, 1)
struct MiniMap {
#include "mini_map_fields.inc"
#include "mini_map_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MiniMap) == 0x480);
static_assert(offsetof(MiniMap, icons) == 0x0);
static_assert(offsetof(MiniMap, view) == 0x4);
static_assert(offsetof(MiniMap, x) == 0x8);
static_assert(offsetof(MiniMap, width) == 0xa);
static_assert(offsetof(MiniMap, height) == 0xc);
static_assert(offsetof(MiniMap, active) == 0xe);
static_assert(offsetof(MiniMap, visible) == 0xf);
static_assert(offsetof(MiniMap, show_gold) == 0x11);
static_assert(offsetof(MiniMap, unit_id) == 0x13);
static_assert(offsetof(MiniMap, unit) == 0x15);
static_assert(offsetof(MiniMap, nation) == 0x19);
static_assert(offsetof(MiniMap, scale_x) == 0x1d);
static_assert(offsetof(MiniMap, scale_y) == 0x21);
static_assert(offsetof(MiniMap, u32_25) == 0x25);
static_assert(offsetof(MiniMap, key_down) == 0x29);
static_assert(offsetof(MiniMap, rotation) == 0x2b);
static_assert(offsetof(MiniMap, u8_2d) == 0x2d);
static_assert(offsetof(MiniMap, mode) == 0x2e);
static_assert(offsetof(MiniMap, blink_rate) == 0x32);
static_assert(offsetof(MiniMap, blink_time) == 0x34);
static_assert(offsetof(MiniMap, blink_on) == 0x38);
static_assert(offsetof(MiniMap, blink_rate_b) == 0x39);
static_assert(offsetof(MiniMap, blink_time_b) == 0x3b);
static_assert(offsetof(MiniMap, blink_on_b) == 0x3f);
static_assert(offsetof(MiniMap, frame) == 0x40);
static_assert(offsetof(MiniMap, frame_time) == 0x42);
static_assert(offsetof(MiniMap, marker_on) == 0x46);
static_assert(offsetof(MiniMap, marker_unit) == 0x64);
static_assert(offsetof(MiniMap, marker_pos) == 0xdc);
static_assert(offsetof(MiniMap, marker_kind) == 0x244);
static_assert(offsetof(MiniMap, marker_frame) == 0x262);
static_assert(offsetof(MiniMap, marker_time) == 0x280);
static_assert(offsetof(MiniMap, slot_used) == 0x2f8);
static_assert(offsetof(MiniMap, slot_kind) == 0x302);
static_assert(offsetof(MiniMap, slot_a) == 0x30c);
static_assert(offsetof(MiniMap, slot_b) == 0x334);
static_assert(offsetof(MiniMap, slot_c) == 0x35c);
static_assert(offsetof(MiniMap, slot_d) == 0x384);
static_assert(offsetof(MiniMap, slot_e) == 0x3ac);
static_assert(offsetof(MiniMap, slot_f) == 0x3d4);
static_assert(offsetof(MiniMap, slot_g) == 0x3fc);
static_assert(offsetof(MiniMap, slot_h) == 0x424);
static_assert(offsetof(MiniMap, slot_i) == 0x44c);
static_assert(offsetof(MiniMap, u32_474) == 0x474);
static_assert(offsetof(MiniMap, fog) == 0x478);
static_assert(offsetof(MiniMap, texture) == 0x47c);
#endif

}  // namespace joan::orig
