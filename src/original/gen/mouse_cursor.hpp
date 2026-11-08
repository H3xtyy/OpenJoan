// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MouseCursor class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "cursor_shape.hpp"
#include "../database.hpp"

namespace joan::orig {

struct Bitmap;
struct CursorRec;
struct Surface;
struct VgaBuf;

#pragma pack(push, 1)
struct MouseCursor {
#include "mouse_cursor_fields.inc"
#include "mouse_cursor_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MouseCursor) == 0x204);
static_assert(offsetof(MouseCursor, init_flag) == 0x0);
static_assert(offsetof(MouseCursor, shape_count) == 0x1);
static_assert(offsetof(MouseCursor, hide_flag) == 0x2);
static_assert(offsetof(MouseCursor, frozen) == 0x3);
static_assert(offsetof(MouseCursor, cur_shape) == 0x4);
static_assert(offsetof(MouseCursor, old_x1) == 0x6);
static_assert(offsetof(MouseCursor, old_y1) == 0xa);
static_assert(offsetof(MouseCursor, old_x2) == 0xe);
static_assert(offsetof(MouseCursor, old_y2) == 0x12);
static_assert(offsetof(MouseCursor, x1) == 0x16);
static_assert(offsetof(MouseCursor, y1) == 0x1a);
static_assert(offsetof(MouseCursor, x2) == 0x1e);
static_assert(offsetof(MouseCursor, y2) == 0x22);
static_assert(offsetof(MouseCursor, hot_spot_x) == 0x26);
static_assert(offsetof(MouseCursor, hot_spot_y) == 0x2a);
static_assert(offsetof(MouseCursor, width) == 0x2e);
static_assert(offsetof(MouseCursor, height) == 0x32);
static_assert(offsetof(MouseCursor, processing) == 0x36);
static_assert(offsetof(MouseCursor, u8_37) == 0x37);
static_assert(offsetof(MouseCursor, icon_bitmap) == 0x38);
static_assert(offsetof(MouseCursor, u32_3c) == 0x3c);
static_assert(offsetof(MouseCursor, cursor_image) == 0x40);
static_assert(offsetof(MouseCursor, default_bitmap) == 0x44);
static_assert(offsetof(MouseCursor, default_hot_x) == 0x48);
static_assert(offsetof(MouseCursor, default_hot_y) == 0x4c);
static_assert(offsetof(MouseCursor, clip_flag) == 0x50);
static_assert(offsetof(MouseCursor, dirty_valid) == 0x51);
static_assert(offsetof(MouseCursor, dirty_x1) == 0x52);
static_assert(offsetof(MouseCursor, dirty_y1) == 0x56);
static_assert(offsetof(MouseCursor, dirty_x2) == 0x5a);
static_assert(offsetof(MouseCursor, dirty_y2) == 0x5e);
static_assert(offsetof(MouseCursor, dirty_last_x) == 0x62);
static_assert(offsetof(MouseCursor, dirty_last_y) == 0x66);
static_assert(offsetof(MouseCursor, clip_x1) == 0x6a);
static_assert(offsetof(MouseCursor, clip_y1) == 0x6e);
static_assert(offsetof(MouseCursor, clip_x2) == 0x72);
static_assert(offsetof(MouseCursor, clip_y2) == 0x76);
static_assert(offsetof(MouseCursor, shape) == 0x7a);
static_assert(offsetof(MouseCursor, u32_c2) == 0xc2);
static_assert(offsetof(MouseCursor, mode) == 0xc6);
static_assert(offsetof(MouseCursor, mode_value) == 0xca);
static_assert(offsetof(MouseCursor, i16_d2) == 0xd2);
static_assert(offsetof(MouseCursor, shape_table) == 0xd4);
static_assert(offsetof(MouseCursor, res_cursor) == 0xd8);
static_assert(offsetof(MouseCursor, vga_buf) == 0x1fc);
static_assert(offsetof(MouseCursor, show_count) == 0x200);
#endif

}  // namespace joan::orig
