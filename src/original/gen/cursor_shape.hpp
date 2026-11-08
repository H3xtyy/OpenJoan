// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CursorShape class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CursorShape {
#include "cursor_shape_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CursorShape) == 0x16);
static_assert(offsetof(CursorShape, u16_0) == 0x0);
static_assert(offsetof(CursorShape, buf) == 0x2);
#endif

}  // namespace joan::orig
