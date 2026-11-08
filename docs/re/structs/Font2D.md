# Font2D (en2d\font2d.cpp) — in-memory bitmap font

`this` of 0x624810 / 0x623E90 / 0x624DC0 (joan.exe). sizeof >= 0x40 (TODO(re): exact; last field at +0x3C).
Fonts are global objects created at startup by 0x4D6FE0.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `loaded` | 1 once metrics are valid; checked first by both loaders (unload if set) | 0x624810, 0x624DC0 |
| 0x01 | 0x25 | char[0x25] | `name` | requested font name (`strncpy(...,0x20)`); doubles as the texture name | 0x624810 |
| 0x26 | 4 | Texture* | `tex_tmp` | temporary texture pointer during load; released and zeroed before return | 0x624810, 0x624DC0 |
| 0x2A | 2 | u16 | `first_char` | | 0x624810 |
| 0x2C | 2 | u16 | `last_char` | | 0x624810 |
| 0x2E | 2 | u16 | `max_width` | | 0x624810 / 0x623E90 |
| 0x30 | 2 | u16 | `max_height` | | " |
| 0x32 | 2 | u16 | `span_height` | lowest bottom - highest top + 1 | 0x623E90 |
| 0x34 | 2 | u16 | `line_height` | copy of `max_height` | 0x624810, 0x623E90 |
| 0x36 | 2 | u16 | `digit_width` | **space advance**: the text layouter uses it as the width of ' ' and of every code without a glyph, and 8x it for TAB (0x624F00, 0x6255D0, 0x626320). Grid builder stores width of glyph '1'; `.fin` header value 3/4/5 for Goudy_10/12/dauphin_14 | verified 0x624F00, 0x626320, 0x6255D0, 0x626E00 |
| 0x38 | 2 | u16 | `spacing` | loader argument 2/`param_9`; gap added after **every** character (including space/tab/missing glyph), scaled and rounded like the glyph width | verified 0x624F00, 0x6255D0 |
| 0x3A | 2 | u16 | `tighten` | loader argument 3/`param_10`; subtracted from every glyph `width` | 0x624810 |
| 0x3C | 4 | Glyph* | `glyphs` | heap array of `(last-first+1)` x 14 bytes, indexed `code-first_char` | 0x624810 |

## Use by the text blitter (0x624F00 .. 0x626E00, see [functions/ui2d.md](../functions/ui2d.md))

* `line_height` (+0x34) is the line pitch: `round(line_height * scale_y) + round(leading * scale_y)` per line (0x6253F0).
  `span_height` (+0x32) is used only to centre a multi-line block vertically (0x626E00: `(box_h - round(span*sy)) / 2`).
* Glyph lookup (0x627E80) fails (character skipped for drawing, `digit_width` advance used for measuring) when the code is outside
  `first..last` or `glyph.width < 1`.
* `name` (+0x01) is also the texture name; `tex_tmp` (+0x26) is set/cleared around **each** glyph quad (texture fetched by name with
  mip levels forced to 1, released after the quad) — the font keeps no permanent texture reference.
* There is no kerning, no outline/shadow pass and no per-glyph colour: outline/emboss variants are separate baked atlases.
