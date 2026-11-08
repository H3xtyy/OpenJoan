// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WorkScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Button;
struct ButtonPtrArray;
struct Image2D;

#pragma pack(push, 1)
struct WorkScreen {
#include "work_screen_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(WorkScreen) == 0x72);
static_assert(offsetof(WorkScreen, tex_a) == 0x0);
static_assert(offsetof(WorkScreen, tex_b) == 0x4);
static_assert(offsetof(WorkScreen, tex_c) == 0x8);
static_assert(offsetof(WorkScreen, row_frames) == 0xc);
static_assert(offsetof(WorkScreen, row_icons) == 0x10);
static_assert(offsetof(WorkScreen, add_buttons) == 0x14);
static_assert(offsetof(WorkScreen, remove_buttons) == 0x18);
static_assert(offsetof(WorkScreen, ok_button) == 0x1c);
static_assert(offsetof(WorkScreen, cancel_button) == 0x20);
static_assert(offsetof(WorkScreen, button_24) == 0x24);
static_assert(offsetof(WorkScreen, background) == 0x28);
static_assert(offsetof(WorkScreen, info_frame) == 0x2c);
static_assert(offsetof(WorkScreen, title_frame) == 0x30);
static_assert(offsetof(WorkScreen, open) == 0x34);
static_assert(offsetof(WorkScreen, hover_row) == 0x35);
static_assert(offsetof(WorkScreen, state) == 0x37);
static_assert(offsetof(WorkScreen, place) == 0x39);
static_assert(offsetof(WorkScreen, u32_3d) == 0x3d);
static_assert(offsetof(WorkScreen, gold) == 0x41);
static_assert(offsetof(WorkScreen, cost) == 0x45);
static_assert(offsetof(WorkScreen, total) == 0x49);
static_assert(offsetof(WorkScreen, loaded) == 0x4d);
static_assert(offsetof(WorkScreen, u8_51) == 0x51);
static_assert(offsetof(WorkScreen, stock) == 0x52);
static_assert(offsetof(WorkScreen, orders) == 0x62);
#endif

}  // namespace joan::orig
