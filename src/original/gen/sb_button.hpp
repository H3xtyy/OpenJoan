// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SbButton class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Button;

#pragma pack(push, 1)
struct SbButton {
#include "sb_button_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SbButton) == 0x44);
static_assert(offsetof(SbButton, button) == 0x0);
static_assert(offsetof(SbButton, img_out) == 0x4);
static_assert(offsetof(SbButton, img_over) == 0x14);
static_assert(offsetof(SbButton, img_down) == 0x24);
static_assert(offsetof(SbButton, x0) == 0x34);
static_assert(offsetof(SbButton, y0) == 0x38);
static_assert(offsetof(SbButton, x1) == 0x3c);
static_assert(offsetof(SbButton, y1) == 0x40);
#endif

}  // namespace joan::orig
