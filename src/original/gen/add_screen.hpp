// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AddScreen class.
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
struct AddScreen {
#include "add_screen_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(AddScreen, texture_0) == 0x0);
static_assert(offsetof(AddScreen, texture_4) == 0x4);
static_assert(offsetof(AddScreen, sprites) == 0x8);
static_assert(offsetof(AddScreen, button_sprites) == 0x19);
static_assert(offsetof(AddScreen, selected) == 0x1d);
static_assert(offsetof(AddScreen, u8_21) == 0x21);
static_assert(offsetof(AddScreen, add_button) == 0x22);
static_assert(offsetof(AddScreen, cb_button) == 0x368);
#endif

}  // namespace joan::orig
