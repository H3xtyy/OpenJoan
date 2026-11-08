// SPDX-License-Identifier: GPL-3.0-or-later
// CJK text for the double-byte editions (ADR-0004). The game decodes double-byte text itself (RESOURCE\CODEPAGE.RES ->
// CodePage::setup 0x0058b2d0 -> the _mbs* text cursor, which hands the fonts a 16-bit code lead << 8 | trail), but
// the bitmap fonts of every edition hold only 0x20..0xff: the Chinese release drew the rest through code of its own
// double-byte build (D:\ww-double, not the 1.1 reference build). Here, a character above 0xff that a font lacks gets
// a glyph record of ours, rasterised from a system CJK font (FreeType) into atlas textures, sized to the font's capital
// height and coloured like the font; the draw sites in font2d.cpp draw it from the atlas. Nothing of this runs unless
// CODEPAGE.RES selected a double-byte code page (the English, French, German... editions have no such file).
#pragma once

#include <cstdint>

namespace joan::cjk {

/// CodePage::setup (0x0058b2d0): the code page RESOURCE\CODEPAGE.RES asked for. Turns CJK glyphs on when it is a
/// double-byte page (932 / 936 / 949 / 950).
void set_code_page(unsigned cp);

/// Font::glyph (0x00627e80) for a code above the font's last character: our FontGlyph record (14 bytes, the font's
/// own record layout) for the double-byte character `code` in `font` (a Font or Font2d), or null when CJK text is off,
/// the code is not a character, or no font has it.
std::uint8_t* glyph(void* font, std::uint16_t code);

/// The texture to draw `glyph` from: the CJK atlas page holding it when it is one of our records, otherwise
/// `font_texture` (the font's own, unchanged).
void* texture_for(const void* glyph, void* font_texture);

}  // namespace joan::cjk
