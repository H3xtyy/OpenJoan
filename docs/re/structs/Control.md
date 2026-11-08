# Control (`ControlMgr`, the "charmove" singleton) — sizeof = **0x126E** (4718), joan.exe

Singleton at `DAT_00A5B9E8` (`this` of `control_update 0x42F700`, `control_hotkeys 0x42E1A0`, the five input handlers, `combo_input 0x4367C0`, the aim code, ...). The whole object is
saved/restored as one `0x126E`-byte image in section S15 of the savegame ([sav.md](../../formats/sav.md), writer `0x4857C0`; after reading, `0x485990` clears the volatile fields listed below).
Functions: [../functions/control.md](../functions/control.md). Offsets are hex; `V` verified (>= 2 readers/writers), `R` read in one function, `I` inferred. Globals that alias fields:
`DAT_00A5B9F8 = +0x10`, `DAT_00A5B9FC = +0x14`, `DAT_00A5B9F0 = +0x08`, `DAT_00A5BA5C = +0x74`, `DAT_00A5BA61 = +0x79`, `DAT_00A5BA64 = +0x7C`.

`Unit*` below means the hero: `0x41B3C0(*this)`; a recno of 0 or a removed unit makes almost every function return without effect (`0x428B70(recno) == true`).

## Core

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `recno` | recno of the controlled hero (the "main character"); every unit comparison with `DAT_00A5B9E8` in unit.md is this field | V |
| 0x08 | 4 | i32 | `command_points` | RTS command points (+= experience gained, capped by `MAX_COM` 2000, unit.md 8) | R unit.md |
| 0x10 | 4 | i32 | `mode` | current control scheme: **0** keyboard (third person), **1** joystick, **2** mouse click-to-move, **3** keyboard + mouse look, **4** RTS command mode | V |
| 0x14 | 4 | i32 | `prev_mode` | mode being left while `pending` is set, and the mode RTS returns to (`control_set_mode` only accepts `prev_mode` as target while in RTS) | V 0x42E040 |
| 0x24 | 1 | u8 | `busy` | byte read by `0x42D160` (gate of hotkeys and aim input); writer not found | R |
| 0x74 | 1 | u8 | `follow_state` | camera yaw follow: 0 off, 2 held (target heading re-captured every request), 1 released (eases to `+0x75` then 0) | V 0x436190, 0x4384E0, 0x65EEE0 |
| 0x75 | 4 | f32 | `follow_heading` | target camera heading, degrees (unit heading / 0.0174533) | V |
| 0x79 | 1 | u8 | `allow_switch` | "hero switching and RTS toggle allowed" flag: copied from `control_set_mode`'s 2nd argument, cleared by `leave_mode`; tested by `control_hotkeys` (`+0x79 != 0` overrides the scenario-index gates 1 / 5) | R |
| 0x7C | 1 | u8 | `input_blocked` | builtin 391 (`set_charmove_detect_block_flag`): non-zero makes `control_hotkeys` return 0 | V 0x65F670, 0x42E1A0 |
| 0x7D | 4 | i32 | `last_screen` | last UI request code (3 = drag start, 10 = building opened ...) used by the RTS input | R 0x440B30 |
| 0x81 | 1 | u8 | `pending` | mode switch in progress (set by `control_set_mode`, cleared by `control_update` once the old mode is ready) | V |

## Combat / locomotion input state (modes 0, 1, 3)

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x82 | 4 | i32 | `last_dir_code` | last pressed direction code for double-tap detection (4 forward, 5 back, 6 left, 7 right; 0 none) | V 0x4343E0, 0x432D30, 0x430480 |
| 0x86 | 4 | u32 | `last_dir_time` | `now` of that press (double tap window **300 ms**) | V |
| 0x8A | 4 | i32 | `dir_rearm` | mode 1 only: set to 1 when the stick class fell below 2, cleared when a code is stored | R 0x432D30 |
| 0x8E | 1 | u8 | `btn_a` | A (light attack) state: 0 idle, 1 pressed, 2 released, 3 pressed again | V 0x4367C0 |
| 0x8F | 1 | u8 | `btn_b` | B (strong attack) state | V |
| 0x90 | 1 | u8 | `btn_ab` | A+B together state | V |
| 0x91 | 1 | u8 | `btn_c` | reserved (cleared with the others) | R |
| 0x92..0x95 | 4 | u8[4] | `latch` | release latches for A, B, A+B, (4th); set to 1 whenever `Unit.combo_len <= 0` and by `0x485990` | V |
| 0x96 | 1 | u8 | `chain_queued` | a next combo letter is queued / chain step in progress | V |
| 0x97 | 4 | u32 | `attack_time` | `now` when the current attack (or the aim reload) started | V |
| 0x9B | 2 | i16 | `queued_letter` | 1 = 'A', 2 = 'B', 3 = 'C' | V |
| 0x9D | 1 | u8 | `chain_count` | chain step counter (reset to 1 by `0x503FB0`) | V |
| 0x9E | 4 | i32 | `jump_hold_ms` | JUMP key hold accumulator (`+= dt`; fires at >= JUMPMAXD 100 ms or on release) | V |
| 0xA2 | 4 | i32 | `autotarget` | melee assist target recno, set when an attack starts (`0x434270`) | V |
| 0xA6 | 2 | i16 | `air_phase` | mounted/airborne turn helper used by `0x4352F0` (0 / 1 / 2) | R |
| 0xA8 | 4 | u32 | `run_start` | time of the first RUN request of the current run (0 = not running); cleared by `0x503FE0(0)` from `set_action` for every non-RUN action | V |

## Aim mode (bow / crossbow)

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0xAC | 1 | u8 | `aim_active` | 1 while aiming (replaces movement input by `0x42A810`, update by `0x42AD50`, camera CamMan 1). Not a cut-scene flag (HudScreen.md is wrong). `0x485990` (called only by the S15 loader `0x485930`) sets it to 1 after a load unless the mode is 2 or 4; nothing clears it afterwards (control.md "aim-on-load"), the port clears it | V 0x42B3C0, 0x42AD50, 0x42A770, 0x4343E0 |
| 0xAD | 4 | f32 | `aim_yaw_rad` | aim yaw in radians | V |
| 0xB1 | 4 | f32 | `aim_yaw_deg` | stored aim heading in degrees (0..360) — centre of the +-60 deg lock-on window | V |
| 0xB5 | 4 | f32 | `aim_pitch_deg` | stored aim pitch (starts 0) | R |
| 0xB9 | 2 | i16 | `aim_moved` | 1 when look input rotated the aim this frame (suppresses target re-pick) | R |
| 0xBB | 4 | i32 | `lock_target` | lock-on target recno (mirrored to `Unit.+0x2DB shoot_target_unit`) | V |
| 0xBF | 1 | u8 | `aim_entered` | set on entry | R |
| 0xC0 | 4 | u32 | `aim_enter_time` | `now` at entry | R |

## Mounted / siege control

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0xC4 | 1 | u8 | `mounted_control` | 1 while the hero rides a horse under player control (`0x43C7D0` sets it; mode 3 uses it to enable mouse steering) | R |
| 0xC5 | 4 | f32 | `rider_heading` | rider heading (deg) stored by `0x43DAB0` | R |
| 0xC9 | 4 | f32 | `horse_heading` | horse heading (deg) stored by `0x43D7A0` (`0x50AC00`) | R |
| 0xCD | 2 | i16 | `ladder_control` | 1 while the hero climbs a ladder-type object (`0x42D300`, handler `0x42D270`, action 0x2F CLIMB); blocks `control_set_mode` | R |

## Click-to-move (mode 2)

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0xD3 | 4 | i32 | `click_target` | target id (unit recno or item recno) | V |
| 0xD7 | 4 | i32 | `click_kind` | 0 unit, 1 item/gate | V |
| 0xDB | 2 | i16 | `click_flag` | set to 1 after the first swing of a heavy attack order | R |
| 0xDD | 4 | i32 | `click_state` | 0 none, 1 melee approach, 2 heavy attack (EP already paid), 3 energy attack, 4 interact with an item | V |

## RTS (mode 4)

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0xED | 4 | f32 | `rts_height` | RTS camera distance, 700.0 after `control_reset` | R |
| 0xF1 / 0xF5 | 4 / 4 | i32 | `rts_phase`, `rts_phase2` | camera transition phase: 0 start .. 4 ready; `0x447900` returns `+0xF5` | V |
| 0xF9 | 11 | u8[11] | `sel_count[group]` | selection size per group (index `+0x1236`), max 100 (`'d'`); addressed `+0xF9 + group` | R |
| 0x105 | 4400 | i32[11][100] | `sel_units[group][100]` | recnos of the selected units (`+0x105 + group*400 + n*4`); `0x105 + 4400 = 0x1235` | R |
| 0x1235 | 1 | u8 | `drag_active` | box selection / drag latch; cleared when the cut-scene flag stops RTS | R |
| 0x1238 | 4 | i32 | `rts_leader` | the hero recno stored at RTS entry | R 0x43FC90 |
| 0x123C / 0x1240 | 4 / 4 | f32 | `drag_x`, `drag_y` | mouse position where the drag started | R 0x440B30 |
| 0x1236 | 1 | u8 | `sel_group` | current selection group | R |
| 0x1237 | 1 | u8 | `hero_slot` | hero hotkey slot (1..8) requested before leaving RTS | R |
| 0x1248 | 4 | i32 | `rts_action_kind` | context action of the current right click (0 move, 1 attack, 2, 4, 6-8, 9 castle info, 10 building, 0xB) | R 0x440B30 (`local_8[0x492]`) |
| 0x124C | 4 | i32 | `rts_action_time` | `now` of the action | R |
| 0x1250 | 4 | i32 | `rts_action_id` | target id of that action | R |
| 0x1254 | 4 | i32 | `rts_marker_kind` | cursor/marker kind (1 = POINT, 0xB = CASSTO) | R |
| 0x1258 | 8 | char[8] | `rts_marker_name` | `"POINT00"`, `"CASSTO00"`, ... | R |
| 0x1261 | 4 | ptr | `rts_cursor_ptr` | `&DAT_00A48F50` | R |
| 0x1265 | 4 | handle | `rts_icon_tex` | `RTSICON` texture | R |
| 0x1269 | 1 | u8 | `rts_popup` | confirmation popup request | R |
| 0x126A | 4 | f32 | `last_move_angle` | last screen-space movement angle passed to `0x4352F0` (reset to 0 on load) | R |

The RTS part (everything from `+0xED` on) is only a first reading; the S1 port does not need it.

## Volatile fields reset after a load (`0x485990`)

`+0x8E..+0x91 = 0`, `+0x92..+0x95 = 1`, `+0x82 = +0x86 = 0`, `+0x9B = 0`, `+0x126A = 0`, `+0x18 = 0`, `+0x1261 = +0x1265 = 0`, `+0xAC = 1` (modes != 2, 4), `+0x7C = 0`.
