# Widget2D — en2d 2D widget base (ctor 0x61DE00 -> base ctor 0x634B20)

Reference build joan.exe. Derived classes: [Image2D](Image2D.md) (vtable 0x9DA70C, sizeof 0xB7), [Button2D](Button2D.md)
(0x9DA66C, sizeof 0x346), [TextBox2D](TextBox2D.md), [PopupWin](PopupWin.md); `List`/`ComboBox` ([UiList](UiList.md)) reuse the
first bytes. All coordinates are signed 16-bit pixels, **inclusive** rectangles. Status: verified = read in >= 2 functions;
read = one; inferred.

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | ptr | `vtable` | **Correction (see [hud.md](../functions/hud.md)):** `+0` is a vtable after all — the per-frame widget tick 0x61F3B0 steps the move/resize/fade tweens and then calls vtable slot 0 (the derived `update`); an earlier reading of 0x9DA64C/0x9DA66C/0x9DA70C as allocator pairs was wrong for this field. Tween units: delay ms, move speed px/s with an acceleration term, fade speed alpha/s | corrected |
| 0x02 | 1 | u8 | `flag2` | base ctor sets 1 | read 0x634B20 |
| 0x03 | 1 | u8 | `enabled2` | `widget2d_is_interactive` = `+3 && +4`; base ctor sets 1 | read 0x6226E0, 0x634B20; List ctor stores a flag here (0x6F80C0) |
| 0x04 | 1 | u8 | `visible` | returned by 0x623360, cleared by 0x62B6A0 (hide); base ctor sets 1 | verified |
| 0x06 | 1 | u8 | `virtual_coords` | non-zero: rect is in a 640x480 design space and is mapped to the screen by 0x633E60 before every use | verified (0x622CC0, 0x6218C0, 0x62FC60, 0x6304F0) |
| 0x07 | 1 | u8 | `enabled` | 0 = Button2D draws state 3 (disabled) and clears hover | read 0x622CC0 |
| 0x08 | 1 | u8 | `shown` | Image2D::draw returns 0 if clear | read 0x629BD0 |
| 0x09 | 2 | i16 | `x0` | rect left (init -1) | verified |
| 0x0B | 2 | i16 | `y0` | rect top | verified |
| 0x0D | 2 | i16 | `x1` | rect right, inclusive | verified |
| 0x0F | 2 | i16 | `y1` | rect bottom, inclusive | verified |
| 0x11 | 4 | f32 | `alpha` | current opacity, init 1.0; draw code derives the alpha byte from it | read 0x61DE00, 0x61F1B0 |
| 0x15 | 4 | u32 | `move_tag` | caller tag/callback id of the running move tween | read 0x61E020 |
| 0x19 | 4 | u32 | `resize_tag` | same for resize tween (at +0x19 in 0x61E370, i.e. a second block) | inferred |
| 0x1D | 4 | u32 | `fade_tag` | tag of the fade tween | read 0x61F1B0 |
| 0x21 / 0x23 | 2+2 | i16 | `move_x`, `move_y` | move target (0xFFFF = none) | read 0x61E020 |
| 0x25 / 0x29 | 4+4 | f32 | `move_acc_x`, `move_acc_y` | accumulated sub-pixel progress | inferred |
| 0x2D / 0x31 | 4+4 | f32 | `move_dir_cos`, `move_dir_sin` | `cos/sin(atan(dy/dx))` of the move, set when a tween starts | read 0x61E020 |
| 0x35 / 0x36 | 1+1 | u8 | `move_active`, `move_done` | | read |
| 0x37 | 4 | f32 | `move_speed` | 3rd float argument of `move_to` (pixels per second or duration; unit not proven) | read 0x61E270 |
| 0x3B | 4 | f32 | `move_delay` | 4th argument | read 0x61E2A0 |
| 0x3F / 0x40 | 1+1 | u8 | `resize_active`, `resize_done` | | read 0x61EA80 |
| 0x41 | 1 | u8 | `resize_anchor` | 0..6 | read 0x61EA80 |
| 0x42 / 0x46 | 4+4 | f32 | resize direction cos/sin | | read |
| 0x4A..0x5A | | | resize state | `+0x52/+0x54` target w/h (i16), `+0x56/+0x5A` accumulators | read |
| 0x5E / 0x5F | 1+1 | u8 | `fade_active`, `fade_done` | | read 0x61F1B0 |
| 0x60 | 4 | f32 | `fade_target` | -1.0 = none | read |
| 0x64 | 4 | f32 | `fade_progress` | | read |
| 0x68 / 0x6C | 4+4 | f32 | `fade_duration`, `fade_delay` | set via 0x61F2E0 / 0x61F310 | read |
| 0x70 | | | start of derived-class data | | |

The animation tween fields are only partially understood; time stepping happens in the per-frame update (not exported).
Screens never read the tween fields; they only call `move_to`, `fade_to` and `set_rect`.
