// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MenuButton class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MenuButton : Button {
#include "menu_button_fields.inc"
#include "menu_button_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MenuButton) == 0x346);
#endif

}  // namespace joan::orig
