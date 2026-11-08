// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Dialog class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct UiButton;

#pragma pack(push, 1)
struct Dialog {
#include "dialog_fields.inc"
#include "dialog_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Dialog, open) == 0x0);
static_assert(offsetof(Dialog, u8_4) == 0x4);
static_assert(offsetof(Dialog, up_button) == 0x1d);
static_assert(offsetof(Dialog, down_button) == 0x21);
static_assert(offsetof(Dialog, u32_25) == 0x25);
static_assert(offsetof(Dialog, close_button) == 0x39);
static_assert(offsetof(Dialog, scroll) == 0x47);
static_assert(offsetof(Dialog, scroll_max) == 0x4b);
static_assert(offsetof(Dialog, flag_4d) == 0x4d);
static_assert(offsetof(Dialog, u32_454) == 0x454);
#endif

}  // namespace joan::orig
