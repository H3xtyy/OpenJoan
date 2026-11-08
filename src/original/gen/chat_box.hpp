// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ChatBox class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Font;
struct LineEdit;

#pragma pack(push, 1)
struct ChatBox {
#include "chat_box_fields.inc"
#include "chat_box_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ChatBox, open_) == 0x0);
static_assert(offsetof(ChatBox, edit) == 0x4);
static_assert(offsetof(ChatBox, line_count) == 0x8);
static_assert(offsetof(ChatBox, history_pos) == 0xc);
static_assert(offsetof(ChatBox, lines) == 0x10);
static_assert(offsetof(ChatBox, prompt) == 0x290);
static_assert(offsetof(ChatBox, i32_2d0) == 0x2d0);
static_assert(offsetof(ChatBox, box_x1) == 0x2d4);
static_assert(offsetof(ChatBox, box_y1) == 0x2d6);
static_assert(offsetof(ChatBox, box_x2) == 0x2d8);
static_assert(offsetof(ChatBox, box_y2) == 0x2da);
static_assert(offsetof(ChatBox, font) == 0x2dc);
static_assert(offsetof(ChatBox, colour) == 0x2e0);
static_assert(offsetof(ChatBox, u8_2e4) == 0x2e4);
#endif

}  // namespace joan::orig
