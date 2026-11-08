// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Font2d class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct FontGlyph;
struct Texture;

#pragma pack(push, 1)
struct Font2d {
#include "font2d_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Font2d) == 0x40);
static_assert(offsetof(Font2d, loaded) == 0x0);
static_assert(offsetof(Font2d, name) == 0x1);
static_assert(offsetof(Font2d, texture) == 0x26);
static_assert(offsetof(Font2d, first_char) == 0x2a);
static_assert(offsetof(Font2d, last_char) == 0x2c);
static_assert(offsetof(Font2d, max_w) == 0x2e);
static_assert(offsetof(Font2d, max_h) == 0x30);
static_assert(offsetof(Font2d, glyph_height) == 0x32);
static_assert(offsetof(Font2d, line_height) == 0x34);
static_assert(offsetof(Font2d, space_width) == 0x36);
static_assert(offsetof(Font2d, spacing) == 0x38);
static_assert(offsetof(Font2d, kerning) == 0x3a);
static_assert(offsetof(Font2d, glyphs) == 0x3c);
#endif

}  // namespace joan::orig
