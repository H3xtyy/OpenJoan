// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Control2D class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Control2D {
#include "control2d_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Control2D, vtable) == 0x0);
static_assert(offsetof(Control2D, u8_4) == 0x4);
static_assert(offsetof(Control2D, rect) == 0x5);
static_assert(offsetof(Control2D, i16_d) == 0xd);
static_assert(offsetof(Control2D, i16_f) == 0xf);
static_assert(offsetof(Control2D, flags) == 0x11);
#endif

}  // namespace joan::orig
