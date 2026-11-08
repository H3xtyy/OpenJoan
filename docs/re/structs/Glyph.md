# Glyph (14 bytes) — on disk in `.fin` and in memory in Font2D+0x3C

Seven u16, packed. Same layout in file and memory (the loader copies field by field). See [fin.md](../../formats/fin.md).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0 | 2 | u16 | `y_off` | vertical offset of ink from the line top (not normalised to 0 in the `.fin` files: Goudy_12 normal glyphs 4) | RE: 0x624810 (7 reads), 0x623E90 (writer: `bbox_top - min_top`); Goudy_12 hex |
| 2 | 2 | u16 | `x0` | atlas left (inclusive) | 0x623E90 |
| 4 | 2 | u16 | `y0` | atlas top (inclusive) | " |
| 6 | 2 | u16 | `x1` | atlas right (inclusive) | " |
| 8 | 2 | u16 | `y1` | atlas bottom (inclusive) | " |
| 10 | 2 | u16 | `width` | `x1-x0+1`, minus `Font2D.tighten` after load | 0x623E90, 0x624810 (adjusts offset +10) |
| 12 | 2 | u16 | `height` | `y1-y0+1` | 0x623E90 |

Draw-time use (0x6255D0, 0x626320): destination rect = `(pen_x, line_y + round(y_off*sy)) .. (+round(width*sx)-1, +round(height*sy)-1)`, source
rect = `(x0, y0)..(x1, y1)` (inclusive, converted to UV as `x0/tex_w`, `(x1+1)/tex_w`, ...); advance = `round(width*sx)`. A glyph with
`width < 1` counts as absent (0x627E80). Scaling is per metric with truncation toward zero (0x590A20 adds `_DAT_009D3E18 = 0.0`, so it is not a true rounding), not a texture-space scale.

sizeof = 14 (the allocation is `count * 0xE`). Verified example: Goudy_10 glyph 0x20 = `08 00 00 00 08 00 00 00 08 00 01 00 01 00`.
