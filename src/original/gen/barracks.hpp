// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Barracks class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Button;
struct ButtonPtrArray;
struct Image2d;

#pragma pack(push, 1)
struct Barracks {
#include "barracks_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Barracks) == 0x82);
static_assert(offsetof(Barracks, tex_barr) == 0x0);
static_assert(offsetof(Barracks, tex_bg) == 0x4);
static_assert(offsetof(Barracks, tex_commele) == 0x8);
static_assert(offsetof(Barracks, row_frames) == 0xc);
static_assert(offsetof(Barracks, row_icons) == 0x10);
static_assert(offsetof(Barracks, add_buttons) == 0x14);
static_assert(offsetof(Barracks, remove_buttons) == 0x18);
static_assert(offsetof(Barracks, ok_button) == 0x1c);
static_assert(offsetof(Barracks, cancel_button) == 0x20);
static_assert(offsetof(Barracks, background) == 0x24);
static_assert(offsetof(Barracks, info_frame) == 0x28);
static_assert(offsetof(Barracks, title_frame) == 0x2c);
static_assert(offsetof(Barracks, open) == 0x30);
static_assert(offsetof(Barracks, hover_row) == 0x31);
static_assert(offsetof(Barracks, state) == 0x33);
static_assert(offsetof(Barracks, place) == 0x35);
static_assert(offsetof(Barracks, army) == 0x39);
static_assert(offsetof(Barracks, u8_3d) == 0x3d);
static_assert(offsetof(Barracks, gold) == 0x3e);
static_assert(offsetof(Barracks, type_counts) == 0x42);
static_assert(offsetof(Barracks, orders) == 0x5a);
static_assert(offsetof(Barracks, pool) == 0x72);
static_assert(offsetof(Barracks, recruited) == 0x76);
static_assert(offsetof(Barracks, army_size) == 0x7a);
static_assert(offsetof(Barracks, cost) == 0x7e);
#endif

}  // namespace joan::orig
