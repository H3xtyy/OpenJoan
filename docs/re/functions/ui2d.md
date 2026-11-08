# ui2d family — en2d widgets / text blitter, interfac screens, mouse cursor (joan.exe)

Exported pseudocode: `$JOAN_WORK_DIR/decomp/engine/ui2d/` (590 functions; INDEX.md) plus the en2d draw functions that the
export tool filed under `decomp/engine/lights/` (0x61E370, 0x61EA80, 0x622CC0, 0x6255D0..0x626E00, 0x629BD0, 0x62A9A0,
0x62D730, 0x632A70, 0x633500, 0x6332B0 — same address range `0x61xxxx..0x63xxxx`, source `en2d\*.cpp`).
 Earlier font/bitmap tables: [fin.md](fin.md).
Structs: [Font2D](../structs/Font2D.md), [Glyph](../structs/Glyph.md), [Bitmap / BmpResEntry](../structs/Bitmap.md),
[Widget2D](../structs/Widget2D.md), [Image2D](../structs/Image2D.md), [Button2D](../structs/Button2D.md),
[TextBox2D](../structs/TextBox2D.md), [PopupWin](../structs/PopupWin.md), [UiList](../structs/UiList.md),
[MouseCursors](../structs/MouseCursors.md). Formats: [fin](../../formats/fin.md), [bitmap](../../formats/bitmap.md),
[i_cursor](../../formats/i_cursor.md), [strtab](../../formats/strtab.md), [dbf](../../formats/dbf.md).

Names are proposed (not yet applied in Ghidra). Evidence: **S** = source-path string at an assert/alloc site (attribution),
**R** = pseudocode read, **C** = inferred from callers/callees only. Confidence: H / M / L.
Calling convention is `__thiscall` unless noted; decompiler argument lists for 0x6255D0..0x626E00 are scrambled by
register/stack mixing, the "args" column lists the *semantic* order recovered from the call sites.

## 1. Font2D (en2d\font2d.cpp) and the text blitter

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x624810 | `font2d_load_fin(name, spacing, tighten)` | `.fin` loader (see fin.md) | S,R | H |
| 0x623E90 | `font2d_build_from_grid(name, cw, ch, cols, first, last, margin, spacing, tighten)` | derive metrics from a DDS grid atlas; prints `Warning! Cannot initialize font "<name>"` and returns 0 if the texture cannot be loaded (same message in `load_fin`) | S,R | H |
| 0x624DC0 | `font2d_unload` | frees glyph array, drops temp texture | S,R | H |
| 0x628490 / 0x628940 | `scan_cell_bbox` / `pixel_alpha` | grid scan helpers (see fin.md) | R | H |
| 0x627E80 | `font2d_get_glyph(code:u16) -> Glyph*` | NULL if `code < first_char`, `code > last_char` **or `glyph.width < 1`**; else `glyphs + (code-first)*14` | R | H |
| 0x624F00 | `font2d_measure_text(text, max_chars, wrap_w, upper, scale) -> i16 width` | pixel width of the widest line; side effect: `g_text_lines` (DAT_00A69ECE) = number of lines (1 + breaks + wraps) | R | H |
| 0x6253F0 | `font2d_text_height(leading, scale_y)` | `round(line_height*sy)*g_text_lines + round(leading*sy)*(g_text_lines-1)` (uses lines from the last measure) | R | H |
| 0x6255D0 | `font2d_draw_line(x, y, layer, text, color, clip_x1, upper, blendA, blendB, blendC, sx, sy) -> x_end` | single-line draw; truncates with `...` and registers the full text as a hover tooltip via 0x489B30 | R | H |
| 0x626080 | `font2d_draw_line_right(right_x, y, ...)` | `x = right_x - measure + 1`, then draw_line | R | H |
| 0x626130 | `font2d_draw_line_centered(x0, y0, x1, y1, ...)` | centre one line in a rect: `x = x0 + (w - textw)/2` (clamped to >= x0 and >= 0), `y = y0 + (h - round(lh*sy))/2` | R | H |
| 0x626320 | `font2d_draw_wrapped(x, y, x1, y1, layer, text, color, leading, first_line, draw, blendA,B,C, sx, sy, markup, max_glyphs) -> 0/1` | word-wrapping paragraph into a rect, resumable (remaining string -> DAT_00A69EC4, next y -> DAT_00A69EC8, lines emitted -> DAT_00A69ECA); `_`-markup colour switch; `first_line` > 1 skips lines (scrolling); `draw == 0` measures only | R | M |
| 0x626E00 | `font2d_draw_block(x0, y0, x1, y1, layer, text, color, leading, upper, blendA,B,C, sx, sy, align)` | two-pass block layout: pass 0 measures wrapped line widths/heights, pass 1 draws vertically centred in the rect; `align` 1 left / 2 centre / 3 right per line | R | M |
| 0x627D80 | `font2d_count_wrapped_lines(x0,y0,x1,y1, text, ..., &total_lines, &first_chunk_lines)` | loops `draw_wrapped` in measure mode over successive chunks until NUL or 0x1A (Ctrl-Z = end-of-text marker) | R | L |
| 0x590A20 | `scale_trunc(double) -> int` (was `round_half_away`) | `x<0 ? -ftol(-x + h) : ftol(x + h)` with `h = _DAT_009D3E18 = 0.0` => **truncation toward zero**; used for **every** scaled metric | R | H |
| 0x58B2D0 | `text_codepage_init(name, codepage, mode, lead_byte_ranges)` | selects the string-iterator vtable (21 function pointers at 0xA157D8..0xA15838) for mode 0 = CRT/system MBCS, 1 = single byte, 2 = DBCS, 3 = custom lead-byte ranges parsed from text `"%x-%x"` into a 256-byte table at 0xA6452C; error `Code Page %u not supported` | R | M |
| 0x58B9C0 / 0x58BA70 / 0x58BBC0 / 0x58C190 | `mbcs_iter_init` / `mbcs_iter_cur` / `mbcs_iter_next` / `mbcs_strlen` | thin wrappers: iterator state is one pointer; `cur` returns the next character code (1 or 2 bytes) via the table; end = 0 (or 0x1A) | R | H |
| 0x58E380 | `mbcs_toupper(code)` | pointer 0xA15830 = CRT `toupper`-class function; applied to every character when the `upper` flag == 1 | R | M |
| 0x4D6FE0 | `game_init_fonts` | caller of the loaders; table in fin.md. Also loads `RESOURCE\CODEPAGE.RES` (3 text lines: name, codepage, mode, optional lead-byte string) and calls `text_codepage_init` first | S,R | H |

## 2. 2D quad primitive and virtual coordinates

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x632A70 | `ui_draw_quad(x0, y0, x1, y1, layer, color, tex, mode3, blendA, blendB, blendC, sx0, sy0, sx1, sy1, rot)` | **the 2D drawing primitive**: appends 4 vertices (24 bytes each: `x,y,z=0`, packed diffuse, `u,v`) and 2 triangles to the layer's batch (en3d::plane); returns 0 when the batch is full (> 4096 vertices or > 5461 triangles) so the quad is silently dropped | R | H |
| 0x633500 | `ui_draw_bmpres(x, y, layer, name, bmpres, color, blendA..C, crop_x0, crop_y0, crop_x1, crop_y1, rot_add, x1, y1)` | looks up a `BmpResEntry` by name (0x620B40), its texture by entry+9 (0x60A4B0), computes the source rectangle (entry rect, `-1` = texture edge, plus optional crop offsets), rotation = `entry.rot + rot_add`, dest rect = `(x,y)`..`(x+w-1,y+h-1)` unless `x1,y1` are given; calls `ui_draw_quad` | R | H |
| 0x6332B0 | `ui_draw_texture_rect(x, y, layer, tex, color, blendA..C, sx0, sy0, sx1, sy1, rot, x1, y1)` | same without BmpRes lookup (source rect given directly) | R | M |
| 0x633E60 | `ui_virtual_to_screen(x0,y0,x1,y1, &sx, &sy, &ox0,&oy0,&ox1,&oy1, 640, 480)` | `sx = screen_w / 640`, `sy = screen_h / 480` (DAT_00A4885C / DAT_00A48860), then 0x633CE0 (not exported) scales the rect. Applied only by widgets whose byte `+6` is non-zero | R | H (scale), L (rounding in 0x633CE0) |
| 0x6340E0 | `ui_virtual_to_screen_tri(...)` | the same for the three points of a triangular hit area | R | M |
| 0x629230 | `ui_plane_set_flags(plane)` | writes the batch's material word: `+0x38 = 3`, `+0x34 = 0x10022`, `+0x40 = 2` (used for minires sprites) | R | L |
| 0x5EA6E0 | `scene_layer_get(handle) -> Layer*` | resolves the `layer` handle each widget draw receives; `+0x38` is its plane batch | R | M |
| 0x5E9640 | `scene_layer_default(-1)` | default layer used by 0x627D80 | C | L |

## 3. BmpRes (en2d\bmpres.cpp)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x620BC0 | `bmpres_load(table_name)` | builds the entry array from the `INTERFAC` resource DB (see Bitmap.md) | S,R | H |
| 0x620AB0 | `bmpres_free` | | S,R | H |
| 0x620B40 | `bmpres_find(name) -> BmpResEntry*` | linear search (string compare 0x7C07B0, 0 = equal; case-insensitivity unverified, assume `_stricmp`) over `count` entries of 0x38 bytes; NULL if absent | R | H |

## 4. Widget2D base and animation (no source-path string; class vtables 0x9DA64C base, 0x9DA66C Button2D, 0x9DA70C Image2D)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x61DE00 | `widget2d_ctor` | rect = -1, `alpha = 1.0`, all tween state cleared, `fade_target = -1.0` (calls base ctor 0x634B20, not exported) | R | H |
| 0x623360 | `widget2d_is_visible` | returns byte `+4` | R | M |
| 0x6226E0 | `widget2d_is_interactive` | `+4 != 0 && +3 != 0` | R | M |
| 0x623580 | `widget2d_set_rect(x, y, x1, y1, relative)` | absolute or relative move/resize of the rect (+9..+0xF) and its "home" copy (+0x81..+0x87); triangle variant shifts the 3 points | R | M |
| 0x61E020 | `widget2d_move_to(x, y, duration, delay, immediate, tag)` | immediate: teleport keeping size; animated: stores target (+0x21,+0x23), direction `cos/sin(atan(dy/dx))` (+0x2D,+0x31), `duration` (+0x37, via 0x61E270), `delay` (+0x3B, via 0x61E2A0), tag/callback id (+0x15), flags +0x35 active / +0x36 done. No-op (cancels) if already there or `duration < 0` | R | M |
| 0x61E2D0 | `widget2d_cancel_move` | clears the move tween | R | H |
| 0x61E370 / 0x61EA80 | `widget2d_resize_to` / `widget2d_scale_to(anchor 0..6)` | animated or immediate size change keeping one of 7 anchors (0 top-left, 1 centre, 2 bottom-right, 3 top-right, 4 bottom-left, 5 top-centre, 6 left-centre in 61E370; 61EA80 variant 5 = bottom-centre) | R | L |
| 0x61F1B0 | `widget2d_fade_to(target, duration, delay, immediate, tag)` | alpha tween: `target` 0/1, immediate sets `+0x11` directly; animated sets `+0x60` target, `+0x68` duration, `+0x6C` delay, `+0x5E` active, `+0x1D` tag. Screens call `fade_to(0, 1.0, 0, 1, 0)` right after construction = start hidden | R | M |
| 0x61F340 | `widget2d_cancel_fade` | resets fade tween (`+0x60 = -1.0`) | R | H |

## 5. Image2D (en2d\image2d.cpp) — sprite or text label

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x629400 | `image2d_ctor` | vtable 0x9DA70C, rect -1, kind 0xFF | R | H |
| 0x629660 | `image2d_init_image(x, y, bmpres, name, color, blendA..C, crop0..3, w, h, scaled, visible, enabled)` | kind 0; rect from entry size unless `w/h == -1`; one-shot (ignored if `in_use`) | R | H |
| 0x629530 | `image2d_init_text(x, y, font, text, mode, color, upper, blendA..C, scale_x, scale_y, flags)` | kind 1 | R | H |
| 0x6299B0 | `image2d_set_text_layout(x1, y1, leading, first_line, draw, markup, max_chars)` | extra params for mode 3 | R | M |
| 0x6298C0 | `image2d_set_text(const char*)` | frees and re-allocates +0x94 | S,R | H |
| 0x629830 | `image2d_reset` | | S,R | H |
| 0x6294C0 | `image2d_dtor` | calls reset, then the base dtor 0x61DFB0 | R | H |
| 0x629BD0 | `image2d_draw(layer)` | kind 0 -> `ui_draw_bmpres`; kind 1 dispatches on `mode` (+0x98): 0 left line (0x6255D0), 1 centred in rect (0x626130), 2 right-aligned (0x626080), 3 wrapped paragraph (0x626320), 4 block (0x626E00, align 2); alpha byte = `ftol(widget alpha ...)` ORed over the stored RGB | R | H |
| 0x629230 / 0x6292C0 | `minires_plane_setup` / `minires_obj_free` | helpers for minires sprites | R | L |

## 6. Button2D

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x620EE0 | `button2d_ctor` (sizeof 0x346 from `operator new`) | | R | H |
| 0x6211D0 | `button2d_init_bitmap(x, y, bmpres, name_up, name_down, name_over, name_disabled, toggle, pressed0, scaled, visible, enabled, click_sound)` | rect from the first non-null entry's size; names copied only if `strlen < 9`; installs default `back_name = "MENU-BACK-1"` | S,R | H |
| 0x621540 | `button2d_init_owner_draw(x0, y0, x1, y1, draw_cb, toggle, pressed0, scaled, visible, enabled, click_sound)` | rect button with a custom draw callback stored at `+0x33A` | R | H |
| 0x6217E0 | `button2d_release` | clears bmpres, label font, callback, `+0x74` | R | H |
| 0x6218C0 | `button2d_update(key_a, key_b, aux_button, want_right_click, latch, repeat_value) -> int` | hit-test + state machine; result: 1 = left click, 2 = right click, 3 = hotkey (`key_a`, `key_b`, or widget hotkey `+0x91`, compared after `toupper`), 4 = aux button (joystick) `aux_button`; negative values = press held/repeat | R | M |
| 0x622CC0 | `button2d_draw(layer, flags)` | picks state 0 normal / 1 pressed / 2 hover / 3 disabled; draws the state's BmpRes entry (flags bit0) and the label (flags bit2, via `font2d_draw_block` mode 2, shifted by the pressed offset when pressed); `flags & 2` = skip; calls the owner-draw callback instead when `+0x33A != 0` | R | H |
| 0x623CE0 | `button2d_set_label(text, font, color, blendA..C, press_dx, press_dy)` | text copied only if `strlen <= 0x100` | R | H |
| 0x623BD0 | `button2d_set_tooltip(key)` | 9-char string key, sets tooltip mode 1 | R | H |
| 0x62B660 / 0x62B6A0 | `button2d_hide` / `widget2d_clear_visible` | `+4 = 0`, `+0x8C = 0` | R | H |

## 7. PopupWin (en2d\popupwin.cpp)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x62A670 | `popupwin_init(x, y, w, h, show_ok, cb, show_cancel, text_cap)` | min size 0x10D x 0x7F (269x127); creates OK button at `(x1-0x47, y1-0x28)` (`PopUpOk` tooltip) and cancel at `(x1-0x29, y1-0x28)` using bmp names `cancelU/cancelD/cancelH`; text buffer `text_cap+1` | S,R | H |
| 0x62A8E0 | `popupwin_free` | releases both buttons, texture, text buffer | S,R | H |
| 0x62A9A0 | `popupwin_draw(layer, flags)` | 9-slice frame from BmpRes entries `TopLe TopCe TopRi MidLe MidRi (centre) BotLe BotCe BotRi`; bit0 frame, buttons, bit2 text (centred block in the rect inset by 0x14 left/top, 0x14 right, 0x3C bottom) | R | H |

## 8. TextBox2D (en2d\txtbox2d.cpp)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x62EAF0 | `txtbox_init(x,y,x1,y1, text, max_len, font, color, blendA..C, align, scaled)` | allocates `max_len+1`; `max_lines = box_h / (line_height + 2)` | S,R | H |
| 0x62ECA0 | `txtbox_free` | | S,R | H |
| 0x631450 | `txtbox_set_text(const char*)` | `strncpy` to max_len, rewrap, truncate at `max_lines` (drops a trailing `\n`) | R | H |
| 0x62DD10 | `txtbox_edit_single(allow_escape) -> key` | one-line editor key handler | S,R | H |
| 0x62FAB0 | `txtbox_update_multi(allow_escape)` | wrapper: save text/caret, run 0x62FC60, **revert** if the result exceeds `max_lines`; arms the 200 ms blink offset (`DAT_00A69F2C = now - 200`); falls through to the mouse handler 0x6304F0 | S,R | H |
| 0x62FC60 | `txtbox_edit_multi(allow_escape) -> key` | multi-line key handler (Enter inserts `\n` while `line_count < max_lines - 1`; Up/Down move by one wrapped line) | R | H |
| 0x6304F0 | `txtbox_mouse(...)` | click gives focus and places the caret, drag extends the selection, double click selects all | R | M |
| 0x630F50 | `txtbox_rewrap` | rebuilds the line-start table | S,R | H |
| 0x631080 | `txtbox_wrap_recursive(text, base)` | break at `\n`; else when a line reaches the box width back up to the last space/tab, else to the last character that fits; records `base+1+index` per line (max 255 lines, u8) | R | H |
| 0x630AB0 / 0x630770 / 0x630D60 / 0x630CC0 / 0x630D10 | `txtbox_line_y(caret)` / `txtbox_caret_from_xy` / `txtbox_set_caret_from_xy` / `txtbox_clear_selection` / `txtbox_select_all` | helpers; vertical alignment from the high nibble of `+0x82`: 0x10 top, 0x20 middle, 0x30 bottom; row pitch `round((line_height+2)*sy)` | R | M |
| 0x62D730 | `txtbox_draw(layer, show_caret, focused_flag, flags)` | text via `draw_line` (align 1), `draw_line_centered` (2) or `draw_line_right` (3), selection highlight and caret via 0x632100; caret blink period 400 ms (shown when `now - t0 >= 200`) | R | M |
| 0x631570 / 0x6315D0 / 0x631630 / 0x631670 / 0x6316D0 / 0x631730 / 0x631770 | `sel_min/max`, `has_selection` (single and multi variants) | selection anchor `+0x8D` vs caret `+0x87` | R | M |

## 9. List / ComboBox (interfac\list.cpp, interfac\combobox.cpp)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x6F80C0 | `list_init(x,y,x1,y1, ..., bmpres, ..., names, font, ...)` | item list with scroll buttons; 4 button-name slots | R | M |
| 0x6F8470 | `list_free` | | S,R | H |
| 0x6F8670 | `list_add(text) -> index` | growth step 5 items; item stride 0x195 (0x191 text + i32 user data); updates scroll window via 0x62CD40 / 0x62CED0 | S,R | H |
| 0x6F8830 | `list_remove(index)` | | R | M |
| 0x6B3130 | `combobox_free` | releases 4 buttons, the list, the string array | S,R | H |
| 0x6B3930 | `combobox_add_item(text) -> count` | type 2 = flat string array (stride 0x191), else forwards to the list; first item becomes the selection | S,R | H |
| 0x6B3B90 | `combobox_remove_item(index)` | | S,R | H |
| 0x6B44E0 | `combobox_set_selected(index)` | updates the caption label (set_label with text = item text, white, offset 6,6) | R | M |
| 0x6B3D90 / 0x6F9360 | `combobox_item_text` / `list_item_text` | | R | L |
| 0x62CD40 / 0x62CED0 | `scroll_set_window(first, total, top)` / `scroll_set_total(total)` | `top = min(top, total - rows_per_page + visible_rows)`, then `>= first` | R | M |

## 10. Mouse cursor (ilib\mousecr.cpp)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x4915F0 | `mousecr_load` | opens `RESOURCE\CURSOR.RES` (dBase), allocates `rows*8`; per row: hotspot x = atoi(3 chars @ row+8), hotspot y = atoi(3 chars @ row+0xB), blob key = 4 chars @ row+0xE -> `resdb` lookup (0x59B830) into `I_cursor.res` | S,R | H |
| 0x4941B0 | `mousecr_shutdown` | DirectInput mouse `Unacquire` (vtable slot 0x30), unwinds `ShowCursor` balance (+0x200), frees per-cursor buffers (`stride 0x16`, 4 buffers each at +0x7C..+0x8C) | S,R | M |

## 11. Interface screens (interfac\*.cpp) — all build their widgets once, hidden, and slide them in

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x71D2D0 | `objmenu_build` (4337 B) | in-game object menu: two `LONGBAR` Image2D strips (y=0x84 and y=0x298, parked at x=-1024 / +1024), `TXT_FV` caption, 10 owner-draw list rows (x 0x17..0x17F, y = 0xDC + 0x28*i), 6+ panel buttons (rect 0x100,0x3E..0x300,0x6E) whose labels come from string group `OBJMENU` (0x4DBB20..0x4DC580, each 38 B: `strtab_get("OBJMENU", n)` = 0x5A3090) | S,R | M |
| 0x71E800 | `objmenu_free` | | S | M |
| 0x7246F0 | `objmenub_mission_brief` | loads `MISSION_BRIEF_1..8` text from `SCRIPT\SCENARIO\` | S | L |
| 0x73A320 | `objmenuw_siege` | siege-weapon panel: BATTERRAM, BOMBARD, CANNON, CROSSBOW, FAUCONNEAU, TREBUCHET models (`F_IDLE` anim) and help texts `battering_ram.txt`, `bombard.txt`, `cannon.txt`, `crossbow.txt`, `fauconneau.txt`, `trebuchet.txt`, `siege_help.txt` | S | L |
| 0x7435E0 / 0x744CE0 / 0x7478A0 | `optmenui_build` / `optmenui_free` / `optmenui_collect_actions(category 0..6)` | options screen (comboboxes, sliders, `aw_l_*`/`aw_r_*` arrow buttons, `lopt/sopt/lcm/scmc` boxes); the last builds the list of key-binding action ids per category (ranges 1-10, 11-21, 22-32, 35-41, 42-46, 47-51, 53-55) skipping actions named `Not Available` or `Escape` (device index 0/3/1 from combobox value 1/2/3) | S,R | M |
| 0x71B080 / 0x71B6A0 / 0x71BF10 | `minires_*` | map-marker sprite resources: reads per-file `filemem` chunks, builds 0xAE-byte records and world-space textured quads (FVF with normal, stride 0x24) from INTERFAC entries `OBJ01`, `OBJ02`, `TITLE`, `CURSOR`; computes tile cell sizes from the `z<n>b<n>r<nn>c<nn>` entry names | S,R | L |
| 0x6BC530 / 0x6BCF00 | `i_barr_build` / `i_barr_free` | barracks screen (6+6 slot buttons, `aw_*` arrows, `ia_ov`) | S | M |
| 0x6C27C0 / 0x6C3230 / 0x6C8D00 | `i_buy_free` / `i_buy_open` / `i_buy_*` | shop screen (item list via 0x469DD0/0x469E10) | S | L |
| 0x6E8F40 / 0x6E9A50 / 0x6EC480 | `i_work_build` / `i_work_free` / `i_work_update` | workshop screen (`workshop_normal_help.txt`, `workshop_resource_help.txt`) | S | M |
| 0x6E0520 / 0x6E0B70 | `i_letter_free` / `i_letter_load(item)` | loads `ITEM_DES\<file>` text; language flag 1 = `English`, 2 = `French`, 0 otherwise from the item's language field | S,R | H |
| 0x6E5700 / 0x6E5D00 | `i_sms_push(text, kind, mode)` / `i_sms_pop` | message ticker: doubly linked queue of 0x14-byte nodes `{char* text, i32 kind, i32 mode, prev, next}`; new message slides in from x = -text_width at y = 0x206 / 0x204; `mode == 1` purges older mode-1 entries | S,R | M |

## 12. Shared utility functions touched by this family (other owners)

`0x58E7B0/0x58ED70/0x58EB30` tracked alloc/free/realloc; `0x591050` ms timer; `0x5A1170/1440/1550` string helpers;
`0x42A260(r,g,b,a)` pack colour (returns pointer to a static dword); `0x5E95B0` writes a colour into a vertex;
`0x5945D0/0x594900/0x594A10/0x593980/0x5947C0/0x594AB0/0x596140/0x597ED0` input hit-tests (inputs family, see
); `0x4CEEF0(name)` plays a UI event sound by name (`IF_BUTTON_MOUSE_DOWN`);
`0x489920/0x489B30` hover-tooltip registration; `0x4CB5B0(device, action)` key-binding name.

## 13. Round-6 additions (export `ui2d2/`)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x633CE0 | `ui_scale_rect(x0,y0,x1,y1, sx, sy) -> x0',y0',x1',y1'` | `x0' = trunc(sx*x0)`, `x1' = x0' - 1 + trunc(sx*w)`, same for y; w,h = inclusive sizes | R | H |
| 0x634B20 | `widget2d_base_ctor` | `u16 +0 = 0; u8 +2 = 1; u8 +3 = 1; u8 +4 = 1` (visible + interactive by default) | R | H |
| 0x634AE0 / 0x634AA0 | `minf` / `maxf` | float min / max | R | H |
| 0x61F120 | `widget2d_cancel_resize` | clears resize tween (+0x19, +0x41, +0x3F/+0x40, +0x4A/+0x4E, +0x52/+0x54 = 0xFFFF, +0x56/+0x5A) | R | H |
| 0x632100 / 0x631E90 / 0x6324B0 | `ui_draw_rect_fill` / `ui_draw_point` / `ui_draw_triangle` | other 2D primitives (see engine page) | R | M |
| 0x762A00 / 0x62B9E0 / 0x62C080 / 0x62C250 / 0x62C410 / 0x62CFB0 / 0x62D130 / 0x62D2F0 / 0x62C930 | `slider_update`, `scrollbar_update` and arrow/thumb helpers | scroll bar / slider widget (thumb drag via `DAT_00A516D0` and mouse y; thumb size from `(rect_h)/(range)`), uses `ui_virtual_to_screen` | R | L |
| 0x6B82D0, 0x6E80B0 (`MBUT_*`), 0x6D53A0 (`CBUT_*`), 0x754A80/0x755120 (`MBUT_*`, `SBUT_*`), 0x6B23B0/0x75FBA0 (`PAGE_*`), 0x6F4C40 (`CM_*`), 0x722990/0x73EE40 (`CS_*`, `DIAMOND`), 0x73F490/0x7538B0 (`SLOT_*`), 0x73FDE0 (`TB_*`), 0x6C1180/0x6EE790 (`FRAME`, `goldbar`, `titlebar`), 0x6C0570/0x6ED650 (`CHECK`/`CROSS`), 0x6C0C00/0x6EE210 (`graybar`), 0x6C1C70/0x6EEE90 (`L_ARROW`,`R_ARROW`,`TITLE`) | per-screen owner-draw handlers | each draws BmpRes entries `<X>_OT` (normal), `_OV` (over), `_DW` (down), `_DS` (disabled), `_CON` (contents) with `ui_draw_bmpres`, hover from 0x593980, text via `font2d_draw_line_centered`; `END_GOOD_n`/`END_BAD_n` + `good1..8`, `mask_t/mask_b` belong to the end-of-game screen 0x6CE450 (10 KB) | S | M |

Naming convention of BmpRes entries (from these strings): suffix `_OT` out, `_OV` over, `_DW` down, `_DS` disabled; prefix = widget family (`MBUT` mission button, `CBUT`, `SBUT`, `CS`, `CM`, `PAGE`, `SLOT`, `TB`, `aw_l/aw_r/aw_u/aw_d` arrows left/right/up/down).

## 14. Open questions

* Corrected: classes have **no vtable** — `+0` of Widget2D/Button2D/Image2D is an allocator tag (`0x9DA64C/66C/70C` are tracked-allocator pairs).
* TODO(re): the routine that steps the move/resize/fade tweens (no reader of `+0x35/+0x3F/+0x5E` in `ui2d2/`); duration units.
* TODO(re): meaning of blend indices 1, 3, 4 (tables `0xA1977C = {2,1,0,3,4}`, `0xA19790 = {0x20,0x10,0,0x30,0x40}`, `0xA197A4 = {1,0,0,0,0}`, `0xA197B8 = 0x36`).
* Observed (RENDER_STATES.md, apitrace): back buffer 1024x768 X8R8G8B8; so `+6` widgets are scaled 1.6x and `+6 == 0` layouts are 1:1. Whether other resolutions are selectable: TODO(re) (`DAT_00A4885C/60` writers).
* TODO(re): who draws the cursor sprite / selects the cursor kind (callers of the `MouseCursors` table); not in the ui2d2 callers.
* TODO(re): the Direct3D projection/state set-up of the UI pass; 0x5CCAA0 plane draw is outside this export.
* TODO(re): who writes `DAT_00A51D5E / DAT_00A51DB5 / DAT_00A51800` and `DAT_00A516C4/C8/D0/D4` (input family).
