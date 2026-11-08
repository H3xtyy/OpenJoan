// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Scroll class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "gadget.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Scroll : Gadget {
#include "scroll_fields.inc"
#include "scroll_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Scroll, enabled) == 0x70);
static_assert(offsetof(Scroll, id) == 0x71);
static_assert(offsetof(Scroll, pressed) == 0x73);
static_assert(offsetof(Scroll, extra) == 0x74);
static_assert(offsetof(Scroll, min) == 0x75);
static_assert(offsetof(Scroll, max) == 0x79);
static_assert(offsetof(Scroll, page) == 0x7d);
static_assert(offsetof(Scroll, pos) == 0x81);
static_assert(offsetof(Scroll, dragging) == 0x85);
static_assert(offsetof(Scroll, drag_x) == 0x86);
static_assert(offsetof(Scroll, drag_y) == 0x88);
static_assert(offsetof(Scroll, drag_pos) == 0x8a);
static_assert(offsetof(Scroll, callback) == 0x8e);
#endif

}  // namespace joan::orig
