// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MouseClick class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MouseClick {
#include "mouse_click_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MouseClick) == 0x1c);
static_assert(offsetof(MouseClick, x) == 0x0);
static_assert(offsetof(MouseClick, y) == 0x4);
static_assert(offsetof(MouseClick, release_x) == 0x8);
static_assert(offsetof(MouseClick, release_y) == 0xc);
static_assert(offsetof(MouseClick, count) == 0x10);
static_assert(offsetof(MouseClick, time) == 0x14);
static_assert(offsetof(MouseClick, release_time) == 0x18);
#endif

}  // namespace joan::orig
