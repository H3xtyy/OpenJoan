// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ViewRect class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ViewRect {
#include "view_rect_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ViewRect, x0) == 0x10);
static_assert(offsetof(ViewRect, y0) == 0x14);
static_assert(offsetof(ViewRect, x1) == 0x18);
static_assert(offsetof(ViewRect, y1) == 0x1c);
static_assert(offsetof(ViewRect, w) == 0x20);
static_assert(offsetof(ViewRect, h) == 0x24);
#endif

}  // namespace joan::orig
