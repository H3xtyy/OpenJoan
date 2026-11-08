// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ChoiceDialog class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "ui_button.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ChoiceDialog {
#include "choice_dialog_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ChoiceDialog, button_mask) == 0x0);
static_assert(offsetof(ChoiceDialog, choice) == 0x444);
static_assert(offsetof(ChoiceDialog, button_a) == 0x458);
static_assert(offsetof(ChoiceDialog, button_b) == 0x79e);
static_assert(offsetof(ChoiceDialog, button_c) == 0xae4);
#endif

}  // namespace joan::orig
