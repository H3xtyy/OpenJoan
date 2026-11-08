// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MessageBox class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"

namespace joan::orig {

struct Font2d;

#pragma pack(push, 1)
struct MessageBox {
#include "message_box_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MessageBox) == 0xe2a);
static_assert(offsetof(MessageBox, state) == 0x0);
static_assert(offsetof(MessageBox, title) == 0x4);
static_assert(offsetof(MessageBox, text) == 0x44);
static_assert(offsetof(MessageBox, u32_444) == 0x444);
static_assert(offsetof(MessageBox, image) == 0x448);
static_assert(offsetof(MessageBox, x) == 0x44c);
static_assert(offsetof(MessageBox, y) == 0x44e);
static_assert(offsetof(MessageBox, font) == 0x450);
static_assert(offsetof(MessageBox, text_box) == 0x454);
static_assert(offsetof(MessageBox, button0) == 0x458);
static_assert(offsetof(MessageBox, button1) == 0x79e);
static_assert(offsetof(MessageBox, button2) == 0xae4);
#endif

}  // namespace joan::orig
