// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ListBox class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "list_box_column.hpp"

namespace joan::orig {

struct Font2d;
struct ListBoxItem;
struct Scroll;

#pragma pack(push, 1)
struct ListBox {
#include "list_box_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ListBox, u8_3) == 0x3);
static_assert(offsetof(ListBox, u8_4) == 0x4);
static_assert(offsetof(ListBox, ready) == 0x5);
static_assert(offsetof(ListBox, x0) == 0x9);
static_assert(offsetof(ListBox, y0) == 0xb);
static_assert(offsetof(ListBox, x1) == 0xd);
static_assert(offsetof(ListBox, y1) == 0xf);
static_assert(offsetof(ListBox, items) == 0x11);
static_assert(offsetof(ListBox, count) == 0x15);
static_assert(offsetof(ListBox, capacity) == 0x19);
static_assert(offsetof(ListBox, text_x) == 0x1d);
static_assert(offsetof(ListBox, text_y) == 0x1f);
static_assert(offsetof(ListBox, text_x1) == 0x21);
static_assert(offsetof(ListBox, text_y1) == 0x23);
static_assert(offsetof(ListBox, row_height) == 0x25);
static_assert(offsetof(ListBox, scroll) == 0x2b);
static_assert(offsetof(ListBox, bitmap) == 0x37);
static_assert(offsetof(ListBox, pressed) == 0x44);
static_assert(offsetof(ListBox, was_drawn) == 0x48);
static_assert(offsetof(ListBox, fixed_pages) == 0x4c);
static_assert(offsetof(ListBox, selected) == 0x50);
static_assert(offsetof(ListBox, column0) == 0x54);
static_assert(offsetof(ListBox, column1) == 0x67);
static_assert(offsetof(ListBox, column2) == 0x7a);
static_assert(offsetof(ListBox, column3) == 0x8d);
static_assert(offsetof(ListBox, font) == 0xa0);
static_assert(offsetof(ListBox, colour) == 0xa4);
static_assert(offsetof(ListBox, style) == 0xa8);
static_assert(offsetof(ListBox, draw_callback) == 0xb2);
#endif

}  // namespace joan::orig
