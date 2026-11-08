// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitAttScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"

namespace joan::orig {

struct BmpRes;
struct Texture;

#pragma pack(push, 1)
struct UnitAttScreen {
#include "unit_att_screen_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UnitAttScreen) == 0x6b3);
static_assert(offsetof(UnitAttScreen, tex_iunitatt) == 0x0);
static_assert(offsetof(UnitAttScreen, tex_commele) == 0x4);
static_assert(offsetof(UnitAttScreen, bmp_iunitatt) == 0x8);
static_assert(offsetof(UnitAttScreen, last_mouse_x) == 0xc);
static_assert(offsetof(UnitAttScreen, last_mouse_y) == 0xe);
static_assert(offsetof(UnitAttScreen, mouse_moved_ms) == 0x10);
static_assert(offsetof(UnitAttScreen, blink_ms) == 0x14);
static_assert(offsetof(UnitAttScreen, blink_on) == 0x18);
static_assert(offsetof(UnitAttScreen, button_sprites) == 0x19);
static_assert(offsetof(UnitAttScreen, hover_slot) == 0x1d);
static_assert(offsetof(UnitAttScreen, show_inventory) == 0x21);
static_assert(offsetof(UnitAttScreen, add_button) == 0x22);
static_assert(offsetof(UnitAttScreen, cb_button) == 0x368);
static_assert(offsetof(UnitAttScreen, is_open) == 0x6ae);
static_assert(offsetof(UnitAttScreen, unit_id) == 0x6af);
#endif

}  // namespace joan::orig
