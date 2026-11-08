# Image2D (en2d\image2d.cpp) — sprite or text label widget

Derived from [Widget2D](Widget2D.md); vtable 0x9DA70C; `operator new(0xB7)` = sizeof **183** in every screen
(0x71D2D0, 0x6BC530). Ctor 0x629400, init 0x629660 (image) / 0x629530 (text), draw 0x629BD0, reset 0x629830, dtor 0x6294C0.
Supersedes the partial table that used to be in [Bitmap.md](Bitmap.md).

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x70 | 1 | u8 | `in_use` | set by init, cleared by reset; init is ignored if set | verified 0x629660, 0x629530, 0x629830 |
| 0x71 | 1 | u8 | `kind` | 0 = BmpRes image, 1 = text, 0xFF = unset | verified |
| 0x72 | 4 | BmpRes* | `bmpres` | kind 0: table holding `name` (e.g. 0xA48F60 = INTERFAC table) | verified |
| 0x76 | 9 | char[9] | `name` | kind 0: BmpRes entry name (`LONGBAR`, `TXT_FV`, ...) | verified |
| 0x86 | 2 | i16 | `blend_a` (image) | forwarded as first render-state index | read 0x629BD0 |
| 0x88 / 0x8A / 0x8C / 0x8E | 2 each | i16 | `crop_x0, crop_y0, crop_x1, crop_y1` | kind 0: offsets added to the entry's source rect; -1 = unused | read |
| 0x90 | 4 | Font2D* | `font` | kind 1 | verified |
| 0x94 | 4 | char* | `text` | heap copy (strlen+1) managed by 0x6298C0/0x629830 | verified |
| 0x98 | 2 | i16 | `mode` | 0 left line, 1 centred in rect, 2 right-aligned at `x1`, 3 wrapped paragraph, 4 block (centre) | verified |
| 0x9A | 2 | i16 | `upper` | 1 = force upper case (passed as the `upper` flag) | read |
| 0x9C / 0xA0 | 4+4 | f32 | `scale_x`, `scale_y` | text scale (1.0 default) | read |
| 0xA4 / 0xA6 / 0xA8 | 2 each | i16 | `leading`, `first_line`, `draw` | mode 3 parameters, set by 0x6299B0 | read |
| 0xAA | 1 | u8 | `markup` | enables the `_..._` highlight toggle | read |
| 0xAB | 2 | i16 | `max_chars` | -1 = unlimited | read |
| 0xAD | 4 | u8[3]+pad | `color` | RGB; alpha byte is replaced by the widget alpha at draw | read |
| 0xB1 / 0xB3 / 0xB5 | 2 each | i16 | `blend_a/b/c` | indices into the 2D render-state tables (see ui2d.md "Quad primitive") | verified |

Image kind passes `x0,y0` (+9,+0xB) and optionally `x1,y1` (+0xD,+0xF): `x1 = x0 - 1 + entry.w` when the init argument was -1.
