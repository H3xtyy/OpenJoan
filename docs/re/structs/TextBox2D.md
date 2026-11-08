# TextBox2D (en2d\txtbox2d.cpp) — editable single/multi-line text field

Derived from [Widget2D](Widget2D.md). Init 0x62EAF0, free 0x62ECA0, set_text 0x631450, edit 0x62DD10 (single) / 0x62FC60 (multi) /
0x62FAB0 (multi wrapper), mouse 0x6304F0, wrap 0x630F50 / 0x631080, draw 0x62D730.

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x06 | 1 | u8 | `virtual_coords` | Widget2D flag | verified |
| 0x09..0x0F | 4x2 | i16 | rect | Widget2D | verified |
| 0x70 | 4 | char* | `text` | heap buffer of `max_len+1` | verified |
| 0x74 | 4 | Font2D* | `font` | | verified |
| 0x78 | 4 | u32 | `color` | bytes 0..2 = R,G,B (draw reads `+0x78..+0x7A`) | verified |
| 0x7C / 0x7E / 0x80 | 2 each | i16 | `blend_a/b/c` | | read |
| 0x82 | 1 | u8 | `align` | low nibble 1 left / 2 centre / 3 right; high nibble 0x10 top / 0x20 middle / 0x30 bottom (multi-line vertical placement) | verified |
| 0x83 | 1 | u8 | `focused` | editing active | verified |
| 0x84 | 1 | u8 | `dragging` | mouse selection in progress | read |
| 0x85 | 2 | i16 | `max_len` | | verified |
| 0x87 | 2 | i16 | `caret` | byte index | verified |
| 0x89 | 4 | u32 | `sel_color` | copy of `color` at init | read |
| 0x8D | 2 | i16 | `anchor` | selection anchor; selection = [min(anchor,caret), max) | verified |
| 0x8F | 2 | i16 | `key_cursor` | index of the last consumed entry in the global key queue (0x595FB0 = queue length) | verified |
| 0x91 | 4 | i16* | `line_starts` | wrapped line start offsets (`2*lines+2` bytes), NULL when single line | verified |
| 0x95 | 1 | i8 | `line_count` | number of recorded breaks | verified |
| 0x96 | 2 | i16 | `max_lines` | `(y1-y0+1) / (font.line_height + 2)` | verified |

Key codes (BIOS style: plain ASCII, or extended `scan << 8`): 0x08 backspace, 0x0D enter (ends edit / `\n` in multi-line while
`line_count < max_lines-1`), 0x1B escape (only when `allow_escape`), 0x5300 delete, 0x4B00/0x4D00 left/right, 0x4800/0x5000
up/down (multi-line), 0x4700 home, 0x4F00 end. Shift state is `DAT_00A51800 & 3`: with shift the anchor is left in place
(extends the selection). Printable range accepted: 0x20..0xFF except `~` (0x7E is returned unconsumed). Insert is refused when
`strlen == max_len - 1`; single-line additionally refuses text wider than the box (`measure >= x1-x0+1`), multi-line reverts the whole
edit if the wrap needs more than `max_lines` lines. Caret blink: 400 ms cycle (`DAT_00A69F28`), drawn while `now - t0 >= 200`.
