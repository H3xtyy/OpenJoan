# PopupWin (en2d\popupwin.cpp) — modal message box with OK / Cancel

Init 0x62A670, free 0x62A8E0, draw 0x62A9A0. Not derived from Widget2D (own 16-bit header); two [Button2D](Button2D.md) objects are
embedded (offsets unknown, they are reached through `this` implicitly in 0x62A8E0).

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 / 0x02 | 2 each | i16 | `x0`, `y0` | | verified |
| 0x04 / 0x06 | 2 each | i16 | `x1`, `y1` | `x0 - 1 + w`, `y0 - 1 + h` | verified |
| 0x08 / 0x0A | 2 each | i16 | `w`, `h` | clamped to >= 0x10D x 0x7F | verified |
| 0x0C | 2 | i16 | `text_cap` | | read |
| 0x0E | 1 | u8 | `flags` | bit0 = open (cleared by init) | read |
| 0x0F | 1 | u8 | `has_ok` | 0 = OK button hidden | read |
| 0x10 | 1 | u8 | `has_cancel` | 0 = Cancel hidden | read |
| 0x11 | 4 | BmpRes* | `bmpres` | `&DAT_00A5D320` | read |
| 0x15 | 4 | Texture* | `tex` | released in free | read |
| 0x6A5 | 4 | u32 | `user` | callback / user value (init arg 7) | read |
| 0x6A9 | 4 | char* | `text` | heap buffer `text_cap+1` | read |

Layout constants (draw, 0x62A9A0): left column 16 px, right column 0x68 (104) px, top row 0x18 (24) px, bottom row 0x33 (51) px;
frame pieces are BmpRes entries `TopLe`, `TopCe`, `TopRi`, `MidLe`, `MidRi`, centre fill (`DAT_009DA7A8`), `BotLe`, `BotCe`, `BotRi`
drawn with stretch (x1/y1 given). OK button at `(x1-0x47, y1-0x28)`, Cancel at `(x1-0x29, y1-0x28)` (bitmap names `cancelU/D/H`;
OK names at `DAT_009DA784/88/8C`). Text rect = (x0+0x14, y0+0x14, x1-0x14, y1-0x3C), centred block (align 2).
