// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TextBox class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "gadget.hpp"

namespace joan::orig {

struct ArrI16;
struct Font2d;

#pragma pack(push, 1)
struct TextBox : Gadget {
#include "text_box_fields.inc"
#include "text_box_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(TextBox, text) == 0x70);
static_assert(offsetof(TextBox, font) == 0x74);
static_assert(offsetof(TextBox, colour) == 0x78);
static_assert(offsetof(TextBox, text_7c) == 0x7c);
static_assert(offsetof(TextBox, text_7e) == 0x7e);
static_assert(offsetof(TextBox, text_80) == 0x80);
static_assert(offsetof(TextBox, align) == 0x82);
static_assert(offsetof(TextBox, focused) == 0x83);
static_assert(offsetof(TextBox, u8_84) == 0x84);
static_assert(offsetof(TextBox, max_len) == 0x85);
static_assert(offsetof(TextBox, cursor) == 0x87);
static_assert(offsetof(TextBox, caret_colour) == 0x89);
static_assert(offsetof(TextBox, sel_anchor) == 0x8d);
static_assert(offsetof(TextBox, view_start) == 0x8f);
static_assert(offsetof(TextBox, line_starts) == 0x91);
static_assert(offsetof(TextBox, lines_valid) == 0x95);
static_assert(offsetof(TextBox, line_count) == 0x96);
#endif

}  // namespace joan::orig
