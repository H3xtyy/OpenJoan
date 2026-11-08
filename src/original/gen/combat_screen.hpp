// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CombatScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"
#include "scroll_bar.hpp"

namespace joan::orig {

struct BmpRes;
struct Texture;

#pragma pack(push, 1)
struct CombatScreen {
#include "combat_screen_fields.inc"
#include "combat_screen_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CombatScreen, tex_iattcomb) == 0x0);
static_assert(offsetof(CombatScreen, tex_commele) == 0x4);
static_assert(offsetof(CombatScreen, u32_8) == 0x8);
static_assert(offsetof(CombatScreen, u32_c) == 0xc);
static_assert(offsetof(CombatScreen, buttons_0) == 0x10);
static_assert(offsetof(CombatScreen, buttons_1) == 0x356);
static_assert(offsetof(CombatScreen, buttons_2) == 0x69c);
static_assert(offsetof(CombatScreen, buttons_3) == 0x9e2);
static_assert(offsetof(CombatScreen, buttons_4) == 0xd28);
static_assert(offsetof(CombatScreen, buttons_5) == 0x106e);
static_assert(offsetof(CombatScreen, buttons_6) == 0x13b4);
static_assert(offsetof(CombatScreen, buttons_7) == 0x16fa);
static_assert(offsetof(CombatScreen, buttons_8) == 0x1a40);
static_assert(offsetof(CombatScreen, buttons_9) == 0x1d86);
static_assert(offsetof(CombatScreen, buttons_10) == 0x20cc);
static_assert(offsetof(CombatScreen, buttons_11) == 0x2412);
static_assert(offsetof(CombatScreen, buttons_12) == 0x2758);
static_assert(offsetof(CombatScreen, buttons_13) == 0x2a9e);
static_assert(offsetof(CombatScreen, buttons_14) == 0x2de4);
static_assert(offsetof(CombatScreen, buttons_15) == 0x312a);
static_assert(offsetof(CombatScreen, buttons_16) == 0x3470);
static_assert(offsetof(CombatScreen, buttons_17) == 0x37b6);
static_assert(offsetof(CombatScreen, buttons_18) == 0x3afc);
static_assert(offsetof(CombatScreen, buttons_19) == 0x3e42);
static_assert(offsetof(CombatScreen, scroll_bar) == 0x4188);
static_assert(offsetof(CombatScreen, stat_ids) == 0x4460);
static_assert(offsetof(CombatScreen, item_ids) == 0x44d8);
static_assert(offsetof(CombatScreen, sel) == 0x44de);
static_assert(offsetof(CombatScreen, scroll) == 0x44e0);
static_assert(offsetof(CombatScreen, row_count) == 0x44e2);
static_assert(offsetof(CombatScreen, i16_44e4) == 0x44e4);
static_assert(offsetof(CombatScreen, dirty) == 0x44e6);
#endif

}  // namespace joan::orig
