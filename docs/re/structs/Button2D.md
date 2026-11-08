# Button2D — clickable bitmap or owner-drawn button (vtable 0x9DA66C, ctor 0x620EE0, sizeof 0x346)

Derived from [Widget2D](Widget2D.md). Init: 0x6211D0 (bitmap states), 0x621540 (rect + draw callback). Update/hit-test 0x6218C0,
draw 0x622CC0. Source file not named by an assert; class sits between `bmpres.cpp` and `image2d.cpp` in `en2d` (button code).

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x70 | 4 | i32 | `press_source` | what pressed it: 0 left mouse, 1 right mouse, 3 aux button, else the key code (`DAT_00A51D5E`) | read 0x6218C0 |
| 0x74 | 1 | u8 | `ready` | set by init, cleared by 0x6217E0; all methods are no-ops when 0 | verified |
| 0x75 | 6x2 | i16[6] | `tri` | three points (x,y) of a triangular hit area when `shape == 1` | read 0x622CC0, 0x623580 |
| 0x81..0x87 | 4x2 | i16 | `home_rect` | untransformed copy of the rect (x0,y0,x1,y1); this is what hit-tests and draw use | verified |
| 0x89 | 1 | u8 | `hold_mode` | 0: a press toggles/pulses `down`; 1: press sets `down`, release fires | read 0x6218C0 (semantics I) |
| 0x8A | 1 | u8 | `down` | pressed state | verified |
| 0x8C | 1 | u8 | `drawn` | set by draw, cleared by hide | verified |
| 0x8D | 1 | u8 | `shape` | 0 rectangle, 1 triangle | verified |
| 0x8E | 1 | u8 | `tip_mode` | 0 none, 1 one tooltip key (+0x104), 2 two (+0x10D, +0x15E); tooltips are registered every frame via 0x489920/0x489B30 | verified |
| 0x8F | 1 | u8 | `hover` | latched hover | read |
| 0x90 | 1 | u8 | `click_sound` | play UI event `IF_BUTTON_MOUSE_DOWN` (0x4CEEF0) on press | verified |
| 0x91 | 2 | u16 | `hotkey` | keyboard code that triggers the button (`toupper`-compared) | read |
| 0x93 | 4 | BmpRes* | `bmpres` | | verified |
| 0x97 + i*0x13 | 0x13 | State[4] | `states` | i = 0 up, 1 down, 2 over, 3 disabled | verified (0x620EE0, 0x622CC0) |
| 0xE3 | 0x21 | char[33] | `back_name` | default `"MENU-BACK-1"` (exact use unknown) | read |
| 0x104 | 9 | char[9] | `tip_key` | | read |
| 0x10D / 0x15E | 0x51 | char[81] | `tip_text`, `tip_text2` | | read |
| 0x227 | 0x101 | char[257] | `label` | text drawn on top (<= 0x100 chars) | verified |
| 0x328 | 4 | Font2D* | `label_font` | | verified |
| 0x32C | 4 | u32 | `label_color` | RGBA | verified |
| 0x330 / 0x332 / 0x334 | 2 each | i16 | `label_blend_a/b/c` | | verified |
| 0x336 / 0x338 | 2 each | i16 | `press_dx`, `press_dy` | label offset while `down` (scaled by `sx, sy`) | read 0x622CC0 |
| 0x33A | 4 | fn* | `draw_cb` | owner-draw callback `(this, layer, flags) -> bool`; non-null replaces bitmap drawing | verified |
| 0x33E / 0x342 | 4+4 | u32 | `user0`, `user1` | cleared by release | read |

State record (stride 0x13):

| off | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 9 | char[9] | `entry` | BmpRes entry name; empty = nothing drawn |
| 0x09 | 4 | u8[3]+u8 | `color` | RGB, byte 3 initialised 0xFF and overwritten by widget alpha |
| 0x0D | 2 | i16 | `blend_a` | default 0 |
| 0x0F | 2 | i16 | `blend_b` | default 2 |
| 0x11 | 2 | i16 | `blend_c` | default 0 |

`button2d_update` result codes: 1 left click, 2 right click, 3 hotkey, 4 aux button; hold-mode release inside 400 ms of the press
returns 0, later returns -1 (long press).
