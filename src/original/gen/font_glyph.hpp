// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's FontGlyph class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct FontGlyph {
#include "font_glyph_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(FontGlyph) == 0xe);
static_assert(offsetof(FontGlyph, top) == 0x0);
static_assert(offsetof(FontGlyph, x0) == 0x2);
static_assert(offsetof(FontGlyph, y0) == 0x4);
static_assert(offsetof(FontGlyph, x1) == 0x6);
static_assert(offsetof(FontGlyph, y1) == 0x8);
static_assert(offsetof(FontGlyph, width) == 0xa);
static_assert(offsetof(FontGlyph, height) == 0xc);
#endif

}  // namespace joan::orig
