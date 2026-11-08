// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ComboBox class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "gadget.hpp"

namespace joan::orig {

struct BmpRes;
struct Button;
struct Font2d;
struct ListBox;

#pragma pack(push, 1)
struct ComboBox : Gadget {
#include "combo_box_fields.inc"
#include "combo_box_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ComboBox) == 0xad);
static_assert(offsetof(ComboBox, created) == 0x70);
static_assert(offsetof(ComboBox, style) == 0x74);
static_assert(offsetof(ComboBox, last_x) == 0x78);
static_assert(offsetof(ComboBox, last_y) == 0x7c);
static_assert(offsetof(ComboBox, selected) == 0x80);
static_assert(offsetof(ComboBox, font) == 0x84);
static_assert(offsetof(ComboBox, u32_88) == 0x88);
static_assert(offsetof(ComboBox, button) == 0x8c);
static_assert(offsetof(ComboBox, arrow_up) == 0x90);
static_assert(offsetof(ComboBox, arrow_down) == 0x94);
static_assert(offsetof(ComboBox, slider) == 0x98);
static_assert(offsetof(ComboBox, items) == 0x9c);
static_assert(offsetof(ComboBox, item_count) == 0xa0);
static_assert(offsetof(ComboBox, pressed) == 0xa4);
static_assert(offsetof(ComboBox, list) == 0xa5);
static_assert(offsetof(ComboBox, open) == 0xa9);
#endif

}  // namespace joan::orig
