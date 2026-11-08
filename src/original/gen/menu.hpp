// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Menu class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Menu {
#include "menu_fields.inc"
#include "menu_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Menu) == 0xc);
static_assert(offsetof(Menu, state) == 0x0);
static_assert(offsetof(Menu, next_state) == 0x4);
static_assert(offsetof(Menu, reopen) == 0x8);
#endif

}  // namespace joan::orig
