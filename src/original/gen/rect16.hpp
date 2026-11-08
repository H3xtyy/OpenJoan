// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Rect16 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "pop_up_win.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Rect16 : PopUpWin {
#include "rect16_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Rect16) == 0x6ad);
#endif

}  // namespace joan::orig
