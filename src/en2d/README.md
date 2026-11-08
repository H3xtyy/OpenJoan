# en2d - the game's 2D engine library

The functions of the original `d:\ww\en2d\*.cpp` files (one file each, by `re/attribution.csv`), plus one module of
ours for the double-byte editions.

| File | Original | What it holds |
|---|---|---|
| `bmpres.cpp` | `en2d\bmpres.cpp` | sprite / bitmap resource sets (`BmpRes::find` 0x00620b40 ...) |
| `font2d.cpp` | `en2d\font2d.cpp` | fonts: the bitmap fonts made from a texture (`Font2d::init` 0x00623e90), the `FONT\*.fin` fonts (`Font::init` 0x00624810), glyph lookup (`Font::glyph` 0x00627e80), measuring and drawing text (`Font::draw_glyphs` 0x00625c50, `Text::draw_line` 0x006255d0, wrapped text) |
| `image2d.cpp` | `en2d\image2d.cpp` | 2D images and labels |
| `popupwin.cpp` | `en2d\popupwin.cpp` | pop-up windows |
| `txtbox2d.cpp` | `en2d\txtbox2d.cpp` | text boxes |
| `widget2d.cpp` | `en2d\widget2d.cpp` | widgets: buttons, movement / scaling animations |
| `cjk_glyphs.cpp` | ours | glyphs for characters above 0xff that the fonts lack, in editions whose `RESOURCE\CODEPAGE.RES` selects a double-byte code page (Chinese, Japanese, Korean): rasterised from a system CJK font with FreeType, coloured like the font, drawn from atlas textures. `Font::glyph` and the three glyph draw sites of `font2d.cpp` call it; it does nothing for the other editions |

Text in the game is a byte string walked by the text cursor of `game/unknown_mbtext.cpp`, which `CodePage::setup`
(0x0058b2d0) switches to the multi-byte functions for the double-byte editions; a font is asked for one 8-bit or
16-bit character code at a time. The fonts' glyph records are 14 bytes (`jo::FontGlyph`: top, source rectangle
x0 / y0 / x1 / y1 inclusive, width = advance, height).
