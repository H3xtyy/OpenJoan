// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's EndScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct EndScreen {
#include "end_screen_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(EndScreen) == 0x1084);
static_assert(offsetof(EndScreen, victory_image) == 0x0);
static_assert(offsetof(EndScreen, victory2_image) == 0x4);
static_assert(offsetof(EndScreen, victory3_image) == 0x8);
static_assert(offsetof(EndScreen, victory_e_image) == 0xc);
static_assert(offsetof(EndScreen, sheet_a) == 0x10);
static_assert(offsetof(EndScreen, sheet_b) == 0x14);
static_assert(offsetof(EndScreen, sheet_c) == 0x18);
static_assert(offsetof(EndScreen, font) == 0x1c);
static_assert(offsetof(EndScreen, next_button) == 0x20);
static_assert(offsetof(EndScreen, next1_button) == 0x366);
static_assert(offsetof(EndScreen, quit_button) == 0x6ac);
static_assert(offsetof(EndScreen, es_button) == 0x9f2);
static_assert(offsetof(EndScreen, joan_button) == 0xd38);
static_assert(offsetof(EndScreen, open) == 0x107e);
static_assert(offsetof(EndScreen, u8_107f) == 0x107f);
static_assert(offsetof(EndScreen, open_time_ms) == 0x1080);
#endif

}  // namespace joan::orig
