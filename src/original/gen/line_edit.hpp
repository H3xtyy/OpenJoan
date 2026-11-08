// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LineEdit class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "gadget.hpp"

namespace joan::orig {

struct Font2d;

#pragma pack(push, 1)
struct LineEdit : Gadget {
#include "line_edit_fields.inc"
#include "line_edit_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LineEdit) == 0x91);
static_assert(offsetof(LineEdit, text) == 0x70);
static_assert(offsetof(LineEdit, font) == 0x74);
static_assert(offsetof(LineEdit, colour) == 0x78);
static_assert(offsetof(LineEdit, text_7c) == 0x7c);
static_assert(offsetof(LineEdit, text_7e) == 0x7e);
static_assert(offsetof(LineEdit, text_80) == 0x80);
static_assert(offsetof(LineEdit, align) == 0x82);
static_assert(offsetof(LineEdit, focused) == 0x83);
static_assert(offsetof(LineEdit, u8_84) == 0x84);
static_assert(offsetof(LineEdit, max_len) == 0x85);
static_assert(offsetof(LineEdit, cursor) == 0x87);
static_assert(offsetof(LineEdit, caret_colour) == 0x89);
static_assert(offsetof(LineEdit, sel_anchor) == 0x8d);
static_assert(offsetof(LineEdit, view_start) == 0x8f);
#endif

}  // namespace joan::orig
