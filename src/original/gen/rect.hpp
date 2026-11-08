// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Rect class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Palette16;
struct Palette24;
struct Palette32;
struct Palette8;

#pragma pack(push, 1)
struct Rect {
#include "rect_fields.inc"
#include "rect_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Rect) == 0x10);
static_assert(offsetof(Rect, left) == 0x0);
static_assert(offsetof(Rect, top) == 0x4);
static_assert(offsetof(Rect, right) == 0x8);
static_assert(offsetof(Rect, bottom) == 0xc);
#endif

}  // namespace joan::orig
