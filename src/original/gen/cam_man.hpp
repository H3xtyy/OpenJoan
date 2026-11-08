// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CamMan class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "ui_button.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CamMan {
#include "cam_man_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CamMan, button_0) == 0x20);
static_assert(offsetof(CamMan, button_1) == 0x366);
static_assert(offsetof(CamMan, button_2) == 0x6ac);
static_assert(offsetof(CamMan, button_3) == 0x9f2);
static_assert(offsetof(CamMan, button_4) == 0xd38);
#endif

}  // namespace joan::orig
