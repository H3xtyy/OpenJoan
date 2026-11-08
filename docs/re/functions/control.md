# control family (ControlMgr / "charmove" / remapkey) — joan.exe

Player control of the hero: mode switching, per-frame input handlers for the five control modes, auto-target / lock-on, bow-aim mode,
click-to-move mode, RTS command mode (summary only), and the key-binding layer (`remapkey.cpp`). Evidence tags: **V** verified (>= 2 sites agree),
**R** read in one function, **I** inferred. All addresses are `joan.exe`.
object layout: [../structs/Control.md](../structs/Control.md); key table: [../structs/KeyMap.md](../structs/KeyMap.md).

**Export note.** The brief's export (`decomp/game/control`, `decomp/game/rts`) contains the dispatcher (`0x42F700`), the mode-update functions and `remapkey`,
but the *per-frame input handlers* (`0x4343E0` ... `0x440B30`) and the key-polling helpers (`0x4CB820`, `0x4CB6D0`, ...) were found only in the neighbouring exports
`decomp/engine/camera3/` (and `decomp/game/unit2/`, `decomp/engine/audio3/`); rows below say `[camera3]` for those. They are read as data like the rest.

Unit field names are those of [Unit.md](../structs/Unit.md) (`+0x213 anim_position` 1 = mounted pose, `+0x215 action`, `+0x233 combo_len`, `+0x2F5 actor` ...).
`ControlMgr` = the singleton at `0xA5B9E8` (`this` of nearly every function below). `now` = `DAT_00A500BA` (ms), `dt` = `DAT_00A500BE` (ms per tick), `DAT_00A500B6` = per-frame delta
used by the camera code, `DAT_00A500D2` = "game running" byte (0 while a menu has paused the world) — see [TIMING.md](../TIMING.md).

## 1. Function table

### 1.1 Dispatcher and mode switching

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x42F700 | control_update (`charmove_update`) | per-frame: mode-switch completion + per-mode update (see §3.1) | called by `worldview_update` 0x5634E0 | V |
| 0x42E040 | control_set_mode(new_mode, allow_flag) | request a mode change: refused when a switch is pending (`+0x81`), `DAT_00A48CEF == 2`, `+0xCD != 0`, current mode 4 and `new != +0x14`, or `new == current`; otherwise `0x42DF50(1)` (leave), RTS light toggle, `+0x81 = 1`, `+0x79 = allow_flag`, `+0x14 = old`, `+0x10 = new` | 0x42E040, 0x42E1A0, 0x42DE70 | V |
| 0x42DF50 | control_leave_mode(keep) | `keep == 0`: `0x42A770` (cancel special states), `0x43C500`, `0x42D1D0`, `recno = 0`; then per-mode leave stub (`0x4343C0`, `0x432D10`, `0x438B00`, `0x430460`, `0x4404C0`), `0x5959C0`, `+0x81 = +0x79 = 0` | 0x42DF50 | R |
| 0x42DE70 | control_reset (hud export) | keeps `recno`, leaves mode 0, clears aim state, `+0xED = 700.0`, then `control_set_mode(DAT_00A58023, 0)`; if the configured mode is 1 (joystick) and `*DAT_00A51806 == 0` (no joystick) falls back to mode 0 | 0x42DE70 | R |
| 0x42D980 | control_enter_game (hud export) | sets `+0x10 = DAT_00A58023` (configured mode), puts the hero in IDLE (action 1) if it is not mounted; mounted: plays the dismount/mount fix-up (`0x4FC...` pose, `0x501190(0x2A/0x43)`) | 0x42D980 | R |
| 0x4300E0 / 0x4301C0 | control_switch_pending / cutscene_flag | `byte this+0x81` / `byte GameState+0x744` (set_cut_scene_flag) | 0x42F700 | V |
| 0x430110 / 130 / 150 / 170 | mode_ready_0..3 | always return 1 (modes 0..3 are ready at once) | bodies | V |
| 0x447860 / 0x447900 | rts_leave_ready / rts_phase | ready only when RTS phase byte `+0xF5 == 4`; then arms the camera follow (`0x436190(1); 0x436190(0)`), runs `0x4384E0` until the follow state settles | 0x447860 | V |
| 0x592BD0 | cursor_recenter | `DAT_00A4860A = 1` and set the (software) mouse cursor to `cursor.x + off.x, cursor.y + off.y` (`this+0x14/0x18 + 0x662/0x666`) — executed when entering modes 0, 1, 3 | 0x592BD0 | R |
| 0x438AD0 | mode2_enter | `+0xDB = 0` and clear the click-order state (`0x43AC60`) | 0x438AD0 | V |
| 0x43FC90 | rts_enter | RTS entry (see §3.6) | body | R |
| 0x4404C0 | rts_leave(keep) | RTS exit (see §3.6) | body | R |
| 0x42A770 | control_cancel_special | clears `+0xAC` (aim) / `+0xC4` (mounted control) / `+0xCD` (siege control) side states | callers 0x42DF50, 0x43C7D0, 0x42D300 | I |

### 1.2 Per-mode update (inside `control_update`, runs every frame — not the key handlers)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x434F40 / 0x4338C0 / 0x431520 | mode0_update / mode1_update / mode3_update | `0x4384E0` then `0x434270` (identical bodies) | bodies | V |
| 0x439090 | mode2_update | `+0xDD` in 1..3 -> `0x43ACB0`, `== 4` -> `0x43B2D0`; then `0x4384E0` | body | V |
| 0x442EF0 | rts_update | RTS update (6.9 KB, not analysed in depth) | callees | R |
| 0x4384E0 | camera_follow_update | camera yaw follow of the hero: mode 3 + mounted + mouse-look: camera `to.yaw := unit heading`; otherwise eases the camera yaw to the stored target heading `+0x75` while follow state `+0x74 != 0`; state 1 -> 0 when converged. Skipped while `DAT_00A48CEF != 0` | 0x4384E0, 0x65EEE0 (turn_camera) | R |
| 0x436190 | camera_follow_request(on) | `on != 0`: `+0x74 = 2` (if it was 0/1) and `+0x75 = unit.heading / (pi/180)` (deg); `on == 0`: `+0x74: 2 -> 1` | 0x436190, callers | V |
| 0x434270 | autotarget_update | the melee assist. `DAT_00A5802D != 0` (manual-aim option) or `DAT_00A5802C`-style disabling: `+0xA2 = 0`. Otherwise, hero valid: if the **current** action is an attack (0x1C ATTACK, 0x1E ENGATK, 0x49 H_ATTACK or 0x53 RUSH) and the **previous** action (`Unit+0x219`) was not one of them (i.e. an attack just started this frame): `+0xA2 := find_autotarget()`; if a target exists and is within melee range (`0x433F40`) -> `face_target` (`0x434080`). If the current action is not an attack: `+0xA2 = 0` | 0x434270 | R |
| 0x433C20 | find_autotarget | when no auto-target held, no aim mode (`+0xAC`), no mounted/ladder control: among all other units that are hostile (`0x5094D0`) and alive, with `len^2 <= 22500.0` (150 units) of the hero **and** `dot(heading_dir, dir_to_target) > _DAT_009CF5C4` (front half-plane; `_DAT_009CF5C4` is the float zero constant), choose the minimum lateral miss distance `sqrt(len^2 - dot^2)` | 0x433C20 | R |
| 0x433F40 | target_in_melee_range | both units exist, neither dead, `distance <= _DAT_009CFBE0` (150.0, same constant as the player-light height in hud.md) | 0x433F40 | R |
| 0x434080 | face_target | `target_heading := atan2(dy, dx)/(pi/180)` toward `+0xA2` (via `0x50ABD0`); in mode 3 also rotates the camera `to.yaw` by the same turn delta | 0x434080 | R |

### 1.3 Click-to-move mode (mode 2) helpers

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x438B30 | mode2_input [camera3] | mouse input handler (§3.4) | body | R |
| 0x43ACB0 | mode2_order_update | approach/attack the clicked target stored in `+0xD3` (id) / `+0xD7` (kind 0 unit, 1 item/gate) with state `+0xDD` 1 melee, 2 heavy, 3 energy | body | R |
| 0x43B2D0 | mode2_interact_update | state 4: walk to an item and fire the interact trigger | body | R |
| 0x43A7D0 | mode2_set_target(id, kind, want_heavy) | validates the target (alive / item exists), reachability `0x43A580`; stores `+0xD3/+0xD7`; state 1, or state 2 when `want_heavy` and `ep >= EGATK_MP` (`DAT_00A3CD28` = 40) — the EP are spent **immediately** (`0x4FE5D0(ep - 40)`) | body | R |
| 0x43A9E0 | mode2_set_energy_target(id, kind) | same validation; state 3 | body | R |
| 0x43AC60 | mode2_clear | `+0xD3 = +0xD7 = +0xDD = 0` | body | V |
| 0x437910 | try_energy_attack(want) | **ENERGY_ATTACK**: cost = `ftol(max_ep * equip_stat(0x15) / 100)`-style (`0x523700`, stat `0x15`); HP floor `0x523320`; needs `can_attack` (`0x500BA0`), not mounted, `+0x23F == 0`, `cost <= ep`, `hp > hp_floor`, the animation table has action 0x1E (`0x500D40`) and (`0x50D4B0` skill) ; then `set_action(0x1E)`, `0x462810(0x45,0)` (voice/sound), `ep -= cost`, `hp -= hp_floor`, `last_ep_regen = now`; in `SCENARIO_INTERACT_MODE` (`0x4CD510 == 1`, Scenario.md `mode`) the function does nothing and just returns `want` | body | R |

### 1.4 Bow / crossbow aim mode (`ControlMgr+0xAC`, the brief's "scripted control")

`+0xAC` is **not** a cut-scene flag: it is set by `0x42B3C0` (the ARROW_SHOOTING key) and cleared by `0x42AD50` / `0x42B6E0` / `0x42A770` (V). While set, the camera is CamMan 1 (aim camera, `0x429860(1)`).

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x42B3C0 | aim_enter(pressed) [camera3] | needs actor, scenario mode != `SCENARIO_INTERACT_MODE` (1; with 1 the key press is just swallowed), `can_attack`, not mounted, hero alive: `0x42A6C0` (clear shoot targets, `action_alt = 1`, `use_action_alt = 1`), `set_action(IDLE)`, `hit_counter = 0`, `+0xAC = 1`, `+0xBF = 1`, `+0xC0 = now`, `+0x97 = now`, aim camera, `+0xB1 = heading_deg` (0..360), `+0xAD = heading_rad`, `+0xB5 = 0`, ranged weapon equipped -> `set_action(0x30 SHTLOAD)`, lock-on `0x42C260` -> `0x42C7C0`, holster hand 0 (`0x50A480(0,0,0,0)`) | body | R |
| 0x42AD50 | aim_update | per-frame while `+0xAC` (return at once if the hero is gone). **Exit** when `Unit.hit_counter (+0x371) != 0` (the hero was hit) or the hero is dead: `+0xAC = 0`, `use_action_alt = 0`, camera back to follow (`0x4297D0`: CamMan mode := its previous mode, resets the aim CamMan), `hit_counter = 0`, `0x427CC0(1)`, holster the ranged weapon (`0x50A480(0,0,0,0)`, `0x49F370`), re-create and re-equip the melee weapon of slot 0 (`0x4974F0(0)` -> `0x49EA90` -> `0x50A480`) when hand 0 is empty, `0x508B90(0,1)` (idle) if the action is one of the shoot actions 0x30..0x33/0x35..0x39, shadow actor shown again (`0x42CF20(1)`). **Else**: `0x42CB20` (camera); action 0x30 SHTLOAD completes when `now - ControlMgr+0x97 >= clip_length * 100 / (100 + equip_stat(0x11))` (stat 0x11 = attack-speed percent): the ranged weapon is put in hand 0 (type from `Equipment+0xE`) and the hero goes to `0x36 SHTIDLE`; while the hero is in a stationary action (`0x508AB0`) with a ranged weapon whose `[+2]` (ammo) is non-zero it starts `0x30 SHTLOAD` again (auto-reload after a shot, `+0x97 = now`, `+0x96 = 0`); when `+0xB9 == 0` (no look input this frame) re-pick the lock-on (`0x42C260 -> 0x42C7C0`) and validate it (`0x42C830`) | body | R |
| 0x42B6E0 | aim_exit(request) [camera3] | leave aim mode: `request` = key pressed in modes 0 and 3 (the shoot key is a **toggle** there), key **not held** in mode 1 (hold-to-aim); same cleanup as the exit branch of `0x42AD50` plus `0x5124B0(1)`, `+0xBB = 0`, `0x436190(1); 0x436190(0)` and a `0x4384E0` loop until the camera follow settles; RTS (mode 4) never | body | R |
| 0x42A810 | aim_input [camera3] | per-frame input in aim mode: `0x42B6E0(shoot)`, `0x42BA40(A = fire)`, `0x42BCD0(angle, stick_class)` (look) | body | R |
| 0x42BA40 | aim_fire [camera3] | if action == 0x36 SHTIDLE and both ranged slots filled: shot direction = normalised vector to the lock-on target (`+0xBB`, aim height `+ _DAT_009CF9CC`) or the camera forward vector; written to `Unit+0x365..0x36D` (**not** a zero vector: unit.md `zero_vec` is the projectile direction); `set_action(0x33 SHTOUT)`; `+0x97 = now` | body | R |
| 0x42BCD0 | aim_look [camera3] | mouse/stick look: rotates the aim yaw/pitch (`local_20/24` of CamMan 1), wraps +-pi, limits yaw to +-`AIM_HA` (60 deg, `_DAT_00A33754`) around the stored aim heading `+0xB1` (mode 3: unlimited), writes `unit.heading`, `target_heading`, `+0xAD`, clears `+0x2DB` | body | R |
| 0x42C260 | aim_find_target | lock-on: iterate all other units that are hostile (`0x5094D0`), not dead, and whose `UnitRes+0x118` (collision/shape class) is neither 2 nor 7; target point = unit position with `z += UnitRes(+0x109) * _DAT_009CF77C`; only if the camera yaw (CamMan 1 `to`, wrapped into the window `+0xB1 * (pi/180) +- _DAT_00A336AC`) is within `AIM_HA` (60 deg; mode 3: always) of the stored aim heading and the pitch within `AIM_HA`; candidate when `miss <= len / 100.0 + AIM_R (25)` (`miss` = distance of the unit point from the camera forward line, `len` = distance to the hero, `_DAT_009CF9C4 = 100.0`) and `len < AIM_MXD (2500)`; the nearest candidate wins (start value `AIM_MXD`) | body | R |
| 0x42C7C0 | aim_set_target(id) | `+0xBB = id; unit.+0x2DB = id` | body | V |
| 0x42C830 | aim_keep_target | validates the lock-on: drops it (`+0xBB = 0`, `unit.+0x2DB = 0`) when the target is gone, farther than `AIM_MXD`, or (modes other than 3) outside the aim half-angle; otherwise re-applies the camera to the target direction (`0x423F40`) | body | R |
| 0x42CB20 | aim_camera_update | positions CamMan 1 (aim camera) at the hero: focus = hero pos + (0,0, `_DAT_009CFA0C`) plus a rotated shoulder offset (`0x60C180/0x60C1D0` rotation helpers, 10.0 units) and the yaw from `+0xAD`/`+0xB1`; applied with `0x423EC0` only if the angular error to the stored heading is within `AIM_HA` (always in mode 3) | body | R |
| 0x42CF20 | actor_set_enabled | `Actor+0x10A = arg` (Actor.md `enabled`); called with 0 on aim enter / mount and 1 on exit, ecx is the hero's shadow actor (`Unit+0x319`) — I | body | I |

### 1.5 Global hotkeys and per-mode input handlers [camera3]

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x42E1A0 | control_hotkeys | per frame, from the camera controller 0x562F40: gates, screen hotkeys (`0x6F5A70`), quick save/load, hero switch, RTS toggle, then the per-mode input handler (§3.2) | body | V |
| 0x4343E0 | mode0_input | keyboard mode (§3.3) | body | V |
| 0x432D30 | mode1_input | joystick mode | body | V |
| 0x430480 | mode3_input | keyboard + mouse look mode | body | V |
| 0x438B30 | mode2_input | mouse click mode | body | V |
| 0x440B30 | rts_input | RTS mode (groups, edge scroll, orders) | body | R |

### 1.6 Input helpers [camera3 unless noted]

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x4CB820 | key_pressed(mode, action) | trigger test of logical action `action` (1-based) for scheme `mode`: 0 -> keyboard key `+0x24`; 1 -> joypad button `+0x22` (needs `*DAT_00A51806`) else key `+0x28`; 3 -> key `+0x2C` else mouse button `+0x23` (edge `0x594700`) | body | V |
| 0x4CB6D0 | key_down(mode, action) | same selection, "currently held" variant (`0x4CC3D0`, mouse `DAT_00A51DA4[btn]`) | body | V |
| 0x4CC570 | scancode_pressed(code) | `code == DAT_00A51D5A` (last key-down event scancode of this frame); special cases: 0x1C/0x9C Enter = either, 0x1D/0x9D Ctrl = bits `0xC` of `DAT_00A51800` rising, 0x2A/0x36 Shift = bits `3`, 0x38/0xB8 Alt = bits `0x30`; `0x3A, 0x45, 0x46, 0xFFFF` never | body | V |
| 0x4CC3D0 | scancode_down(code) | held: `DAT_00A51700[code]` (set by the buffered DirectInput keyboard `0x598BD0`), Ctrl/Shift/Alt from the modifier bitfield `DAT_00A51800` (bits 1,2 = L/R shift; 4,8 = L/R ctrl; 0x10,0x20 = L/R alt) | body | V |
| 0x431570 | dir_code_keys(fwd, back, left, right) | screen-space heading code in degrees: fwd 90, fwd+left 135, left 180, back+left 225, back 270, back+right 315, right 360, fwd+right 45, none -1; left+right cancel each other | body | V |
| 0x431680 | dir_code8(fwd, back, left, right) | the joystick 8-direction code: fwd 0x25, back 0x14, left 0x1C, right 0x22, fwd+left 10, fwd+right 0xB, back+left 0xC, back+right 0xD, none 0 | body | V |
| 0x598040 | stick_dir8 | joystick axes 0/1 (fallback 4/5) -> the same 8-direction code | body | V |
| 0x433AF0 / 0x433930 | stick_angle / stick_class | stick angle in degrees (`atan2`, centre -> `_DAT_009CF834`; straight up `_DAT_009CFBA8` = 90, down `_DAT_009CFBBC` = 270) / deflection class 0..3 = bucket of `max(|dx|,|dy|)*16/half_range`: `<2 -> 0`, `<3 -> 1`, `<5 -> 2`, else 3 | bodies | R |
| 0x4350F0 | locomotion_action | desired locomotion action for the speed class: (non-joystick) RUN 0xB, mounted H_RUN 0x46, ladder/climb 0x2F; joystick: class 0 -> IDLE 1 (mounted 0x43), 1-2 -> WALK 10 (mounted H_WALK 0x44, climb 0x2F), 3 -> RUN 0xB (H_RUN 0x46) | body | R |
| 0x437ED0 | locomotion_apply(moving, walk_modifier) | `can_attack` gate; `want = 0x4350F0()`; `walk_modifier`: RUN -> WALK, H_RUN -> H_WALK; if not moving (or currently blocking/guarding 0x2B..0x2D, or mounted pose 1): clears `+0xA8` when want==IDLE and stops running actions via `0x508B90(0,1)`; else stamps `+0xA8 = now` on the first RUN request and issues `set_action(want)` (aim mode: `0x5124B0(want)` = lower-body action in `unit+0x217`) | body | R |
| 0x4352F0 | steer(angle, speed_class, anim_remaining_ms) | converts the screen-space angle into the unit's target heading (`0x50ABD0`) and swings the camera yaw toward the pressed side (§3.3), air-control for action 0x1A (UP) and the idle-turn reset for the mounted pose | body | R |
| 0x438110 | strafe_apply(fwd, back, left, right) | shift/strafe actions 0xE MOVE_L, 0xF MOVE_R, 0x10 MOVE_B, 0x11 MOVE_FL, 0x12 MOVE_FR, 0x13 MOVE_BL, 0x14 MOVE_BR (aim mode: through `0x5124B0`); forward-only -> `locomotion_apply(0xC, 1)` (walk) | body | V |
| 0x438720 | camera_zoom(dir) | `dir 1` zoom in: `dist -= DAT_00A500B6 * 0.6`; `dir 2` zoom out: `dist += ...`; clamp to 80.0 .. 350.0; stores the result in both pitch/dist sets `DAT_00A0BBF0/F4` (camera.md "Zoom") | body | V |
| 0x437B60 | jump_launch(released, dir_on, angle, hold_ms) | §3.5 | body | R |
| 0x4367C0 | combo_input(A, B, rush) | attack-button state machine and combo chaining (§3.5) | body | V |
| 0x436250 | block(defend_held) | DEFEND key held -> `set_action(0x2B GUARD1)` (needs `can_attack`, not mounted, not already 0x2B/0x2D) and sound `0x462810(0x43,0)`; released while in 0x2B/0x2D -> `0x508B90(0,1)` (back to idle/battle stance) | body | V |
| 0x4363A0 | down_attack(defend_held) | DOWN_ATTACK: while DEFEND is held and a hostile unit lying in KNOCKDN/2/3 (0x40..0x42) or DNATKED (0x29), not yet finished off (`+0x1E1 == 0`), has bearing within the animscr window of action 0x22 (`animscr+0x40/+0x44`) and `len^2 <= _DAT_009CFC28` (3600) -> `set_action(0x22 DOWNATK)` | body | R |
| 0x436380 | down_attack_stub | returns 0 | body | V |
| 0x438820 | chain_attack(pressed) | CHAIN key (action 0x15, mode 3 only): allowed when `anim_remaining_ms <= (CHAINPRO(200) / 1000.0) * clip_length / chain_count(+0x9D)` (the last 20 % of the clip for the first chain step, 10 % for the second, ...) and the animation entry byte `+0xF != ' '` -> `set_action(0x1B CHAIN)`, `+0x9D++`, `+0x96 = 1`, `+0x97 = now` | body | R |
| 0x43B490 | use_heal_item(unit) | USE_HEAL_ITEM: inventory entry of item class 0x15 whose heal value (`item+0x4E`) is closest to the missing HP (`max_hp - hp`; `<=` keeps the later index on ties); none -> class 0x19 ("all" potion); consume via `0x50B4F0`/`0x50BCD0`, `count--`, spirit effect `spiritual.txt` for 5000 ms | body | R |
| 0x43B990 | use_energy_item(unit) | action 0x14: same search with item class 0x17 against the missing EP (`0x523700 - ep`), consume `0x50BA00` | body | R |
| 0x43C7D0 | mount(pressed) | INTERACT key while no other INTERACT handler took it: nearest horse `0x44F0B0(2)`, `0x4FC220(horse)` = `unit_set_mount`; success -> `+0xC4 = 1`, `+0x9E = 0`, `0x42A770`, `0x43C480` | body | R |
| 0x42D300 | man_siege_weapon(pressed) | nearest crewable weapon `0x5183D0`, `0x4FAB10(weapon, 1, seat)`; success -> `+0xCD = 1`, `0x42A770`, `0x42D180`, `+0x9E = 0`; handler while active: `0x42D270` | body | R |
| 0x43C5C0 | mounted_input(interact, A, B, shift, angle, move_code, speed_class) | per-frame handler while `ControlMgr+0xC4 != 0` (§3.7) | body, callers 0x4343E0/0x430480/0x432D30 | V |
| 0x43C920 | dismount(pressed) | INTERACT edge: hero action H_IDLE 0x43 with the horse in IDLE 1/2/3, `can_start_action`, mounted (`0x430190`): `unit_set_mount(0)` (`0x4FC220`), `+0xC4 = 0`, `+0x9E = 0` | body | V |
| 0x43CA90 | mounted_combo(A, B) | `combo_input` clone for H_ATTACK 0x49 (§3.7) | body | V |
| 0x43DAB0 | mounted_steer(angle, speed_class, remaining_ms, shift) | rider steering of the horse (§3.7) | body | R |
| 0x43D7A0 | mounted_gait(move_code, shift) | maps the locomotion action (H_IDLE 0x43 / H_WALK 0x44 / H_BACK 0x45 / H_RUN 0x46) to horse gait commands (§3.7) | body | R |
| 0x42D270 | ladder_input(interact, move, shift, angle, move_code, speed_class) | per-frame handler while `ControlMgr+0xCD != 0`: `0x42FD60`, camera follow request, `0x42D560` (stub: validates the hero, returns 0), `0x42D5C0` (climb, §3.7) | body | V |
| 0x42D5C0 | ladder_climb(angle, moving, shift) | if `can_start_action`, locomotion action (`0x4350F0`) == 0x2F CLIMB and a direction is held: angle in `[0, 180]` (forward half) -> `0x518890` (climb up), otherwise `0x445000` (climb down) | body | R |
| 0x592DC0 / 0x592F00 | key_event_push_down / key_event_push_up | ring buffer of 20 records of 0x51 bytes at `kbd+0x70D` (write index `+0xD61`, read index `+0xD65`; full when `write == read-1`): record type 2 = key down, 6 = key up, + scancode, timestamp, current mouse position `(+0x14,+0x18)`, modifier word `+0x150`, a 0x39-byte copy of the keyboard state `+0x156`. Called by `0x598BD0` (the buffered DirectInput keyboard handler) once per **DirectInput event**; nothing is pushed for a held key | bodies | V |
| 0x42FD60 | anim_remaining_ms | `0x5045F0`: `clip_duration - (now - actor.start) % clip_duration` of the hero's current animation, or `_DAT_009CF5C4` (0) | body | V |
| 0x5091D0 | is_strafe_action | `action (or action_alt)` in 0xE..0x14 | body | V |
| 0x500BA0 | can_start_action (hero input gate) | false while `now < Unit+0x20F + CLB_IDLE (DAT_00A3CCE8 = 20)`; true for the "free" actions {-1,0..6, 9..0x14, 0x2B..0x2D, 0x36, 0x37, 0x39, 0x43..0x46, 0x54..0x57, 0x59, 0x5A}, false for all attack/hit/jump/mount/climb/down actions | body | V |
| 0x503FE0 / 0x503FB0 | control_clear_run_stamp / control_reset_chain | `ControlMgr+0xA8 = arg` (called with 0 from `set_action` for every non-RUN action of the hero, see unit.md 6.2) / `ControlMgr+0x9D = 1` (chain counter reset, called when the hero's action is not ATTACK/H_ATTACK/CHAIN) | 0x501190 | V |

### 1.7 Player tick

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x416680 | player_tick | called from the unit phase-0 pass (`0x52C490`) when `unit.recno == DAT_00A5B9E8`: (1) `0x54B480(search type 69 = 'E' (the dword at 0x9CFC2C is the integer 69, not a string), radius 800, ...)`: no hostile within 800 -> `unit.mode = 3` (IDLE) else `2` (ACTIVE) (so the battle stance IDLE_BAT is used within 800 units of an enemy); (2) control mode != 4: `hp_mark - hp > CRI_DIFF (10)` -> HUD "critical combo" prompt `0x462970` (random combo from the hero's animation table, usable by `0x416ED0`), `DAT_00A4F838 = 1`, `hp_mark = hp`; (3) animscr effect of the running animation: `animscr(+0x75 != 0 && +0x76 == 1 && name(+0x77) != DAT_009CF608)` -> `0x41B640` effect start; (4) water splash: `0x469D10(&pos, 0)` finds a region containing the hero; if `pos.z < region_top` and region depth class `> 1` and `now - Unit+0x3A8 > 2000` -> particle `0x41BAF0` at `(x, y, region_top + 5.0)` (`_DAT_009CF878`), stamp `+0x3A8 = now` | body | R |
| 0x462970 | critical_prompt_pick | HUD object: picks a random usable combo of the hero (retries `0x416ED0`), copies it to `hud+0x1F` (6 chars), counts the A/B letters in `hud+0x25`, `hud+0x1D = 0`, `hud+0x37 = 0`, `hud+0x18 = 1`, `hud+0x19 = 0`, timestamp `hud+0x27 = now` (double) | body | R |

### 1.8 remapkey.cpp and key-name UI (details in [KeyMap.md](../structs/KeyMap.md))

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x4CAE10 / 0x4CAE40 | keymap_ctor / keymap_dtor | `memset(this, 0, 0x14)` / `keymap_free` | bodies | V |
| 0x4CAE70 | keymap_load | loads `DEFAULTK` (55 records) and `KEYDESC` (115 records) from `Std.set`, allocates the tables, then `keymap_reset` | `d:\ww\remapkey.cpp` strings | V |
| 0x4CB310 | keymap_free | frees `+8`, `+0xC`, `+4`; zeroes counts | `d:\ww\remapkey.cpp` | V |
| 0x4CB410 | keymap_reset_scheme(scheme) | copies one column group from the defaults array `+0xC` to the live array `+8` for every action: 0 -> `+0x24`; 1 -> `+0x28,+0x22`; 3 -> `+0x2C,+0x23` | body | V |
| 0x4CC220 | keymap_reset | if `DAT_00A5806C == 0` live := defaults; else live := the saved copy in the config block (`GameConfig+0xB5`) | body | V |
| 0x4CB580 | keymap_entry(action) | `&live[(action-1)*0x30]` | body | V |
| 0x4CB5B0 | get_key_string(scheme, action) | display name of the bound input (pointer into the KEYDESC array) or 0 | body | V |
| 0x4CB9B0 | keydesc_find(type, code) | linear search of KEYDESC for `(+0x27 == type && +0 == code)` -> pointer to the 32-byte name | body | V |
| 0x65E0A0 | script_get_key_string (builtin) | maps action names to action ids (table in KeyMap.md) and pushes `get_key_string(DAT_00A5B9F8, id)` as the string result | body | V |
| 0x65F670 | script set_charmove_detect_block_flag (builtin 391) | `DAT_00A5BA64 = arg` = **`ControlMgr+0x7C`**; while non-zero `control_hotkeys` returns 0 at once (input blocked) | 0x65F670, 0x42E1A0 | V |
| 0x7435E0 / 0x744CE0 / 0x7478A0 / 0x745720 | optmenu key pages | the option pages list the bound actions; page ranges and device mapping in KeyMap.md | bodies | R |

## 2. Per-tick order (direct control)

1. Unit phase 0 of the hero (`0x52C490`): `player_tick` (§1.7), hit-event clears, pickups.
2. Camera-controller update `0x562F40` (every frame, also in debug): `control_hotkeys 0x42E1A0` -> per-mode input handler -> orders / `set_action` calls on the hero.
3. `worldview_update 0x5634E0` -> `control_update 0x42F700` -> auto-target + camera follow (`0x4384E0`, `0x434270`) or the mode-2 order update.
4. Unit pass (`0x52C490` phases 1..3), physics (`0x6AD660`): the locomotion action chosen in step 2 moves the body (`flocksys.md` 9.3).

The relative order of step 2 and 3 inside one frame follows [gameloop.md](gameloop.md); both read the *same* key state sampled once per frame by the DirectInput pump (`0x594F90`).

## 3. Behaviour

### 3.1 control_update (0x42F700)

Skip everything when `DAT_00A596AB` (builtin `set_stop_unit_process_flag`) is set. If the cut-scene byte (`0x4301C0`) is set: in RTS mode only `ControlMgr+0x1235 = 0`. Otherwise:
1. If mode == 4 and the controlled unit exists and `is_dead` (`0x42D0E0`: `hp < 1 && lives == 0`) -> `control_set_mode(+0x14, 0)` (leave RTS).
2. If a switch is pending (`+0x81`): the *old* mode's readiness (`+0x14`): 0..3 always ready, 4 only when `0x447860()` succeeds. When ready, clear `+0x81` and run the *new* mode's enter step: 0, 1 -> `0x592BD0` (re-centre the cursor), 2 -> `0x438AD0`, 3 -> `0x592BD0`, 4 -> `0x43FC90` (RTS enter).
3. Unless aim mode (`+0xAC`) is on: run the update of `+0x10` (new mode) — or of `+0x14` while the switch is still pending: 0 -> `0x434F40`, 1 -> `0x4338C0`, 2 -> `0x439090`, 3 -> `0x431520`, 4 -> `0x442EF0`. With `+0xAC` set: `0x42AD50` (aim update) instead.

### 3.2 control_hotkeys (0x42E1A0)

Gates (all must be false/zero): `DAT_00A596AB`, cut-scene byte, `0x42CED0()`, `+0x81` pending, `+0x7C` (builtin 391 block flag), `DAT_00A48CE0`, `0x42D160()`, objective menu (`0x4303D0`), in-game menu (`0x4303F0`), `DAT_00A48CEF == 2` (animated cut-scene camera). Then:
1. `0x6F5A70` screen hotkeys (ESC menu 0x22, character screen 0x23/0x24, save/load/options/objectives/log 0x25..0x29): non-zero result ends the frame's input.
2. Table `T = mode` (mode 4 uses its base mode `+0x14`): action 0x35 quick-save, 0x36 quick-load (HudScreen.md 23), 0x2E mission-goals indicator timer.
3. While **Shift** is held: actions 0x19..0x20 -> select hero slot 1..8 (names looked up with `0x5270B0`/`0x51FD50`, `0x5121C0(unit)`); return 1.
4. Not RTS and not aim: actions 0x18 (SWITCH_CHARACTER), 0x19..0x1C -> `0x432300` (switch controlled unit, unit.md / battle.md builtin 261) when `(+0x79 != 0 || DAT_00A58FA7 > 1)` (**scenario index > 1 or enabled by script**), not busy (`0x42D690`), and the hero is not in a mount/dismount animation (0x4F..0x52) or is dead.
5. Action 0x17 (RTS mode): when `(DAT_00A58FA7 >= 5 || +0x79 != 0)` (`DAT_00A58FA7` = first dword of the Scenario object = scenario number, Scenario.md) and `DAT_00A48CEF` not 1/2: mode 4 -> `control_set_mode(+0x14, 0)`, otherwise `control_set_mode(4, 0)`; return 1.
6. Otherwise (no toggle pressed): ignore input (set `+0xA8 = 0`, return 0) while the hero's action is DIE 0x2A, STANDUP 0x3C, KNOCKDN 0x40/0x41/0x42 or H_DROP 0x4E; else call the per-mode input handler `0x4343E0 / 0x432D30 / 0x438B30 / 0x430480 / 0x440B30` for mode `0..4`.

### 3.3 Mode 0 — keyboard (0x4343E0), scheme device 0 (`K_KEY` column)

Actions used (ids in [KeyMap.md](../structs/KeyMap.md) 3): 0x12 shoot, 0xD A, 0xC DEFEND, 0xE B, 0xF ENERGY, 0x3 SHIFT, 0x30/0x31 zoom, 0x10 JUMP, 4/5/8/9 forward/back/turn left/turn right, 0x13/0x14 heal/energy items, 0x11 INTERACT. "Pressed" = `0x4CB820` (edge), "down" = `0x4CB6D0`.
1. JUMP accumulator `+0x9E`: while JUMP is down `+0x9E += dt`, and when `>= JUMPMAXD (DAT_00A3374C = 100)` the jump fires at once; on release a non-zero accumulator fires with the accumulated hold time (so a jump is launched on release or after 100 ms, hold in `(0, 100]` ms+).
2. Double-tap detector: only when a key event exists this frame (`DAT_00A51D5E != 0`): the pressed direction key (FORWARD -> code 4; with SHIFT down also BACKWARD -> 5, TURN_LEFT -> 6, TURN_RIGHT -> 7) is stored in `+0x82` with `+0x86 = now`. A *repeat of the same code within 300 ms* of the stored one is a **double tap** `dt_code`.
3. Movement code `m` = `0x431570(fwd, back, left, right)`; speed class = 3 if any of the four is down else 0; `anim_remaining = 0x42FD60()`.
4. `USE_HEAL_ITEM` pressed -> `0x43B490(0)`; the energy-item action 0x14 pressed (not with Alt) -> `0x43B990(0)`.
5. If not in aim mode, not mounted control, not siege control: INTERACT pressed -> INTERACT handler chain (HudScreen.md 24). If the world is paused (`DAT_00A500D2 == 0`) stop here.
6. Priority list — the first that returns non-zero ends the frame (`return 1`): aim enter `0x42B3C0(shoot)`; camera follow `0x436190(SHIFT down)`; zoom `0x438720`; `0x4363A0` DOWN_ATTACK; `0x436250` BLOCK; `0x4367C0(A, B, rush = (dt_code == 4))` combos / rush; `0x437910(ENERGY)`; (hero valid and not in a strafe action) mount `0x43C7D0`, siege `0x42D300`, and if exactly one direction key is down `0x437B60(jump...)`.
7. Double-tap jumps: `dt_code == 5` -> `set_action(0x15 JUMP_B)`, `7` -> `0x16 JUMP_R`, `6` -> `0x17 JUMP_L` (needs `can_attack`, not mounted, not in IDLEUP/IDLEDN 5/6; mode-1 additionally needs the animation table to contain the action `0x416DC0`). Returns 1 (frame consumed).
8. Otherwise locomotion: SHIFT down **and** moving -> `0x438110` strafes (8 directions, §1.6) and `camera_follow_request(1)`; else `strafe-stop` (`0x508B90(0,1)` when currently strafing), `0x4352F0(angle, speed_class, remaining)` then `0x437ED0(moving, SHIFT)`; with SHIFT but not moving it just stops.
   - **Heading** [V, `0x42FE30` disassembly]: `heading_deg = trunc(camera_yaw_deg + angle_deg - 90.0) mod 360`, normalised to `0..359` (whole degrees; `camera_yaw_deg = CamParams.+0x10 / 0.017453292`, first normalised to `>= 0`). Forward (angle 90) walks along the camera yaw. Sets `target_heading` (`0x50ABD0`); the unit turns by `0x4F2190`.
   - **Camera swing**: while the follow state is 0 and the active camera is mode 0: `yaw_rad += DAT_00A500B6 * 3.49066e-05 * lateral(angle)` (`3.49066e-05 = (pi/180)/500`, i.e. `lateral/500` degrees per ms; a full side press, lateral 90, turns the camera 180 deg/s), `lateral = angle - 90` for `angle < 180`, else `270 - angle` (fwd 0, left +90, back 0, right -90), then wrapped and applied with `set_target`.
   - **RUN vs WALK**: default action RUN (0xB); SHIFT held -> WALK (0xA). First RUN request stamps `+0xA8 = now`.

### 3.4 Other modes

* **Mode 1 joystick (0x432D30, device 1)**: requires a joystick (`*DAT_00A51806 != 0`, else returns 0). Shoot = `key_down(1,0x12)`, A/B edges (1,0xD)/(1,0xE) — **A and B pressed in the same frame = energy attack** (and both are then cleared), DEFEND down (1,0xC), items 0x13/0x14, zoom 0x30/0x31, JUMP (1,0x10) with the same accumulator. Movement from the stick: `stick_class` (0..3) and `stick_angle`; class 0 = idle, 1/2 = WALK, 3 = RUN (`0x4350F0`); `0x433930` buckets above. Double-flick: the stick must be at full deflection (class 3) inside an angle window — forward `_DAT_009CFBD8..9CFBD4` (code 4, no modifier), and with the SHIFT_CAMERA button (1,2) held left `_DAT_009CFBD0..9CFBCC` (code 6), back `_DAT_009CFBC8..9CFBC4` (code 5), right `<= _DAT_009CF878 || >= _DAT_009CFBC0` (code 7). A code equal to `+0x82` within 300 ms of `+0x86` **and re-armed** (`+0x8A = 1`, set when the stick class dropped below 2 in between) fires: 4 = rush, 5 = `0x15 JUMP_B`, 7 = `0x16 JUMP_R`, 6 = `0x17 JUMP_L`. With SHIFT_CAMERA held and the stick moved, `0x598040`'s 8-way code selects the strafe action (codes 0x25 up, 0x14 down, 0x1C left, 0x22 right, 10/0xB/0xC/0xD diagonals -> flag combinations for `0x438110`); without it the stick drives locomotion with its angle.
* **Mode 3 keyboard + mouse (0x430480, device 3)**: A = `FPS_KEY` or mouse button 1, B = mouse 2, W/S/A/D = FORWARD/BACKWARD/SHIFT_LEFT(6)/SHIFT_RIGHT(7) (strafing is the default: `0x438110(fwd, back, left, right)` when strafing keys are down; forward-only/back-only use locomotion), walk modifier = action 10 (`Marsz`), wheel (`DAT_00A516E4`) or ZOOM keys zoom. Double tap (300 ms, key still down) of any of the four direction actions needs **no** modifier here: forward = rush, back = `0x15 JUMP_B`, strafe right (7) = `0x16`, strafe left (6) = `0x17`. ENERGY = key 0xF edge **or** A and B edges in the same frame (`0x437910(want)`). **Mouse look** `0x431790(angle)`: `dx = DAT_00A516F4`, `dy = DAT_00A516F8`; rate `r = (sensitivity + _DAT_009CF784) * _DAT_009CFA00 + _DAT_009CF9F0` (sensitivity from the options via `0x4322D0`), times `_DAT_009CFBB4` (`_DAT_009CFBB8` while mounted); unit heading `+= dx * r` (clamped to +-45 deg per call when attacking, action 0x1C), camera pitch `+= dy * r * (invertY ? -1 : 1)` (`DAT_00A5802B`) clamped to **[-20 deg (-0.349066 rad), +35 deg (0.610865 rad)]** with a ground-collision test (pitch reverts when the camera would go under the terrain, `0x4C7B70`). Camera follow flag, chain key (action 0x15) via `0x438820`. In aim mode `0x42A810` replaces the movement block. Mounted (`+0xC4`): `0x43F000` mouse steering + `0x43C5C0`.
* **Mode 2 mouse click (0x438B30, device 0 keys + mouse)**: items/heal keys `0x13/0x14`, INTERACT (-> `0x43BC50` pickup), nothing else is polled from the keyboard. Mouse: `left = 0x5945D0(rect, 0)`, `right = 0x5945D0(rect, 1)` on the 3D viewport rectangle `DAT_00A48CCC+0x10..0x1C`; the cursor position (`0x43C3C0`, `0x43C3F0`) is picked with `0x439110` (object under cursor: kind 1 = unit, 0xB = item/gate) or `0x43A3B0` (ground point). Left click on ground: `0x43AC60`, `0x43A580(point)` (walk-to); left on a hostile unit: `0x43A7D0(id, 0, left_is_down...)` (approach+attack, state 1 — or heavy state 2 if the right button is also down and EP >= 40); left on a friendly/neutral unit: `0x43A650`/`0x43A740` talk/recruit candidates; right click on a unit: `0x43A9E0` (energy-attack approach, state 3); items: `0x43A720`/`0x43A7D0(…, 1)`. With no click: `0x4367C0(0,0,0)` to age the combo state. Per frame `mode2_order_update 0x43ACB0` walks the hero to `target_radius + _DAT_009CFC20 (10)`; within range it starts the attack: state 1 -> `0x4367C0(1,0,0)` (A), state 2 -> chance roll: `r = 0x590660()` (float in [0,1)): `r > _DAT_009CFC50` (value still unread) -> A, else `r > 0.3` (`0x9CFC4C`) -> B, else none; only once the previous attack is over (`now - +0x97 > anim START/END window`), state 3 -> `0x437910(1)` when `ep >= EGATK_MP` and then the order is cleared.
* **Mode 4 RTS**: see §3.6.

### 3.5 Combat input (shared by modes 0, 1, 3)

`combo_input 0x4367C0(A, B, rush)` is called once per frame with the A/B *edges* of the frame. State bytes in `ControlMgr`:
`+0x8E` (A), `+0x8F` (B), `+0x90` (A+B together): 0 idle, 1 pressed, 2 released, 3 pressed again while the chain is running; `+0x92/+0x93/+0x94/+0x95` release latches (set to 1 whenever `combo_len <= 0`); `+0x96` "chain queued" flag; `+0x97` time of the last attack start; `+0x9B` queued letter (1 = 'A', 2 = 'B', 3 = 'C'); `+0x9D` chain counter.
1. Update the three button states from the A/B edges (pressed: `0 -> 1`, `2 -> 3`; not pressed: `1/3 -> 2`, or `-> 2` when `combo_len > 0`).
2. `combo_len == -1` (lock-out) and `now - Unit+0x23F > 250` ms -> clear lock-out (`+0x23F = 0`, `combo_len = 0`).
3. **Rush** (`rush` = double-tap forward): only when `+0x23F == 0`, `can_attack`, not mounted, the table has action 0x53 (`0x416DC0`) and `0x500D40(0x53)`: cost = animation entry `ENERGY_PT` (`entry+0x18`) scaled by `equip_stat(0x15)/100` when the hero has equipment; needs `cost <= ep` -> `ep -= cost`, `set_action(0x53 RUSH)`, `+0xA8 = 0`, return 1.
4. **A pressed** while idle (`can_attack` false means an attack is running): if a combo is running (`combo_len > 0`, state 3 and the latch `+0x92`): the next letter is accepted only if `START_TIME(Unit+0x237) <= now - +0x97 < END_TIME(Unit+0x23B)` (the animation-table window of the current attack) and `+0x9D > 1`: queued letter `+0x9B = 1`, `+0x96 = 1`, `Unit+0x235 = 1`. When the hero is free (`can_attack`): mounted pose -> `set_action(0x1C ATTACK, combo 'D'=0x44)` after 50 ms of the last move; on foot -> `0x462810(0x41,1)`, `set_action(0x1C, 'A' = 0x41)`, `+0x97 = now`, `+0x96 = 0`; with a rush cost entry free (`rush` set and not `can_attack`), falls to the RUSH branch.
5. **B pressed**: mirror with letter 'B' (0x42); on foot it needs an ENERGY_PT payment for the B attacks that have one (`0x416E30(0x1C, "B")`, `cost = entry+0x18 * equip_stat(0x15)/100`, `ep -= cost`) and mounted: action 0x1D JATTACKB with letter 0x45 `'E'`.
6. **Chain resolution** when the window has elapsed (`now - +0x97 >= Unit+0x23B` and `+0x96 == 1`): find the animation-table entry whose (`POSITION`, `MOVEMENT`, `ITEM_USING`, `SPECIAL`) equal the hero's and whose combo string is `Unit.combo_buf[0..combo_len-1]` + the queued letter; the entry must be *known* to the hero (`0x416ED0` = all combos unlocked, or `0x50D170(combo)` = learned) and have no extra letters after it (the 6-byte COMBO field is NUL-terminated); on success the new attack starts with `set_action(0x1C, letter)` (letter 'A' 0x41 / 'B' 0x42 / 'C' 0x43; a 'B' attack pays its `ENERGY_PT`), sound `0x462810(letter, 0)`, `+0x97 = now`; on failure the whole state is reset (`+0x8E..+0x91 = 0`, `+0x96 = 0`, `Unit+0x235 = 0`) and the call returns 1.
7. When nothing is pending and the attack timer ran out (`now - +0x97 >= END_TIME` and free and `combo_len == 0`): reset the button states to 0.
Block/guard, down attack, energy attack, rush and chain are separate actions (`0x436250`, `0x4363A0`, `0x437910`, step 3, `0x438820`).
**Jump** (`0x437B60`): launched when the JUMP key is released (or after 100 ms), only if `can_attack`, not mounted pose and the action is not IDLEUP/IDLEDN (5/6): horizontal impulse `dir * 1400 * (direction_flag)` (`0x578`; the 2D vector from the 576xxx helpers = heading or `angle`), reduced to a fallback magnitude when the ground along the direction is blocked (`0x6AE500` slope probe < 0); vertical impulse `vz = K * min(hold, JUMPMAXD) / JUMPMAXD + K` with `K = 5000.0` (`0x9CFC34`): 5000 for a tap up to 10000 for a 100 ms hold, stored in `Unit+0x359/+0x35D/+0x361` (`0x505CC0`), `set_action(0x1A UP)`, sound `0x462810(0x44,0)`, `+0xA8 = 0`.

### 3.7 Mounted control (`ControlMgr+0xC4 = 1`) and ladder control (`+0xCD = 1`)

**Mounted** (set by `0x43C7D0` after `unit_set_mount` succeeded; modes 0, 1, 3 call `0x43C5C0` instead of the on-foot block, after the zoom). Per frame, in this order (first match wins):

| input | behaviour |
|---|---|
| USE_HEAL_ITEM (0x13) edge | `use_heal_item` (same as on foot) |
| energy-item key (0x14) edge, Alt not held | `use_energy_item` |
| INTERACT (0x11) edge | the usual handler chain (free prisoner, talk, hire, recruit, use entity, barracks, workshop: HudScreen.md 24); if one consumed the key nothing else happens this frame |
| camera follow | `camera_follow_request(shift)` (mode 3 with mouse look: not requested) |
| INTERACT edge, no handler took it | `dismount` (`0x43C920`): needs hero H_IDLE 0x43 and the horse idle (1/2/3) |
| A / B edges | `mounted_combo`: the same state machine as `combo_input` but starting `H_ATTACK 0x49` with letters 'A' (0x41) / 'B' (0x42, pays ENERGY_PT of the entry `0x49`/"B" if the skill `0x50D3D0(0x10)` is learned or all combos are unlocked); chain letters only A/B, combos "AAA"/"BBB" wrap the counter back to 1 (a three-hit loop); no rush, jump, block, down attack, energy attack, aim or potion-by-weapon |
| movement | `mounted_steer(angle, speed_class, remaining, shift)` then `mounted_gait(move_code, shift)` |

* **Steering** `0x43DAB0` (gate: horse mount flag `Unit+0x1EA != 0`, hero not in mount/dismount animations 0x4F..0x52, horse not in 0x3E/0x3F): heading target = same formula as on foot (`0x42FE30`); the turn per update is limited to `TURN_ANG` = **6.0 deg** (`DAT_00A33740`, via `0x590B00` clamp of the camera swing and the heading step); camera swing uses the same `lateral/500` rule as on foot (only if `0x43EF30` allows); airborne (action 0x1A) imparts the `1400 * speed_class` velocity to the horse body; mode 3 + mouse look: the horse heading follows the camera yaw (`+- 0x9CFC54` window) instead of the keys; stores the rider heading in `ControlMgr+0xC5`.
* **Gait** `0x43D7A0` (same gates): `0x4350F0` gives H_WALK 0x44 / H_RUN 0x46 (H_BACK 0x45 when `move_code < 0`); run -> horse gait command `0x470430(1)`, walk/back -> `0x470430(0)`, stop -> `0x466D50`; in mode 3 the strafe/back codes (0x14, 0xC, 0xD) use the walk gait; while the rider is in H_ATTACK 0x49 and the clip has < 100 ms left the horse is released (`0x4533E0`).
* The horse itself is moved by the unit/physics system from the gait command; the rider only supplies heading, gait and attack.

**Ladder / climb control** (`+0xCD`, set by `0x42D300` when `0x5183D0` finds a climbable object and `0x4FAB10(obj, 1, seat)` attaches the hero; mode handlers call `0x42D270` first): camera follow request on `shift`, then `ladder_climb`: while the locomotion action is 0x2F CLIMB, a held direction in the forward half-plane (angle 0..180) climbs up (`0x518890`), any other direction climbs down (`0x445000`). INTERACT (via the handler, `+0xCD != 0` path of `0x42D300`) detaches. While `+0xCD` is set `control_set_mode` refuses mode changes. [naming I: the callee names are not exported; the 0x2F CLIMB action and the `0x42D690` "on ladder" test make this a ladder/siege-ladder control, not a catapult crew control]

### 3.6 RTS mode 4 (summary; S1 does not use it)

* Toggle: key 0x17 (scenario >= 5 or script flag `+0x79`), also `DAT_00A48CEF` must not be 1/2. Enter (`0x43FC90`): resets state, texture `RTSICON`, unit `ai_flag (+0x102) = 1` (the hero is now commanded like an AI unit), animation controller byte `+4 = 1`, builds the camera templates `rts_init -> rts_play` from the current camera (z height `+0xED`, 700 default, the loop steps `DAT_009CFCA0` down until `0x453930` accepts), `CamMan` mode 2 plays `rts_play`; selection list: group `+0x1236` (byte), count `+0xF9[group]` (max 100), entries `+0x105 + group*400 + n*4` = the hero plus its team members that are alive (`0x453620`) — each gets `0x519250`. Phase byte `+0xF5` 0 (start) ... 4 (ready for leave).
* Leave (`0x4404C0`): frees `RTSICON`; hero `ai_flag = 0`, `+0x23F = 0`, clears its order queue (`0x4BB660`) and copies its task to `task_backup` then clears it (`0x4E4520(0,...)`), clears orders and tasks of all team members, resets the anim controller byte, builds the return camera (`rts_aim` template) and (if the hero moved > `DAT_009CFCA0`) camera mode 2 plays `rts_play`; `+0x1237` (hotkey hero slot 1..8 -> names JOAN_OF_ARC, JEAN_DE_METZ, LAHIRE, ALENCON, ORLEAN, charlie7, arthur_de_richemont, FLAVY) triggers `0x432300(slot)` after leaving. Phase `+0xF5 = 2`.
* RTS input `0x440B30`: camera: arrow/edge-scroll bits `DAT_00A51804`, rotate (actions 0x32/0x33 or mouse), zoom (0x30/0x31 or wheel); Ctrl+digit = assign group, Alt+digit = recall group (`0x00599240`), Shift... character hotkeys 0x19..0x20; left click select (`0x44A690`, `0x44A430`), drag box (`+0x1235`, `0x4489C0`), right click order; context actions by clicked object kind (`switch(+0x1248)`: 0 move, 1 attack unit, 2 ..., 4 move-to-point, 6/7/8 `0x44F750`, 9 "CASSTO00" castle info, 10 barracks/workshop open, 0xB `0x452720`). The strings `RTS_LTINF_*` are the HUD hints. Details TODO(re) (0x442EF0, 0x440B30 not read in full).

## 4. Constants (value / source)

| name | value | source |
|---|---|---|
| AIM_R | 25.0 | `DAT_00A3373C`, CHARMOVE (flocksys.md 12): lock-on tolerance base |
| AIM_MXD | 2500.0 | `DAT_00A33758`: lock-on maximum distance |
| AIM_HA | 60.0 deg | `DAT_00A33754`: aim half angle |
| TURN_ANG | 6.0 | `DAT_00A33740` (unused by the exported code) |
| JUMPMAXD / JUMPMIND | 100 / 100 ms | `DAT_00A3374C` (used as the cap in the three input handlers and `0x437B60`) / `DAT_00A33748` (no reader found) |
| CHAINPRO | 200 | `DAT_00A33744`: chain window numerator in `0x438820` |
| CRI_TIME / CRI_DIFF | 10000 ms / 10 HP | `DAT_00A33750` (no reader in this export) / `DAT_00A3CC78` (player_tick) |
| EGATK_MP | 40 | `DAT_00A3CD28`: EP spent when a click-mode heavy attack is chosen |
| CLB_IDLE | 20 | `DAT_00A3CCE8`: `can_start_action` guard after `Unit+0x20F` |
| double-tap window | 300 ms | `0x4343E0` / `0x432D30` / `0x430480` |
| lock-out clear | 250 ms | `0x4367C0` (`0xFA`) |
| aim shoot-load check | `clip * 100 / (100 + stat 0x11)` | `0x42AD50` |
| ground-pick melee reach | 150.0 | `_DAT_009CFBE0` |
| splash cadence | 2000 ms, +5.0 z | `0x416680` |
| enemy search for stance | radius 800 | `0x416680` |
| pitch limits (mouse look) | -20 deg .. +35 deg | literals in `0x431790` |
| zoom | 80.0 .. 350.0, 0.6/ms | `0x438720`, camera.md |
| jump horizontal impulse | 1400 (0x578) | `0x437B60`, `0x4352F0` |
| jump vertical K | 5000.0 | `0x9CFC34` |
| chain divisor | 1000.0 | `0x9CF778` |
| auto-target range squared | 22500.0 (150^2) | `0x9CFBDC` |
| camera swing rate | 3.49066e-05 = (pi/180)/500 | `0x9CFC24` |
| click-mode roll low threshold | 0.3 | `0x9CFC4C` |
| enemy search type | integer 69 ('E') | `0x9CFC2C` |
| float constants with unread values | `_DAT_009CFC50`, `_DAT_009CFBD0..BC0`, `_DAT_009CF9CC`, `_DAT_009CFA0C`, `_DAT_009CF9F0/FA00/F784`, `_DAT_009CFBB4/BB8`, `_DAT_00A336AC` | TODO(re): read the float32 from `.rdata`/`.data` (not recoverable from pseudocode) |

## 5. RNG use (determinism)

| site | what | generator |
|---|---|---|
| 0x43ACB0 | click-mode heavy attack picks A / B / nothing per call | `0x590660()` = `0x5905A0()` raw 15-bit `* DAT_009D8C10` (ilib LCG, see flocksys.md) |
| 0x462970 | critical-hit combo prompt: random usable combo | `0x590540(count)` = `rand(n)` |
| 0x416680 / 0x4343E0 / ... | none | |

All other input code is deterministic given the key stream; record inputs (key/mouse state per tick) for replays.

## 6. Cross-module requests

1. **GameConfig base address**: [GameConfig.md](../structs/GameConfig.md) puts the object at `0xA5801E`; the key code proves the block at `+0xB5` (size `0xA50` = 55 x 0x30 = the live key table) starts at `0xA5806D` (`keymap_reset` reads `DAT_00A5808F = block+0x22`, flag `DAT_00A5806C = +0xB4`). Hence **base = 0xA57FB8**, `DAT_00A58023 = GameConfig+0x6B` (control scheme, i32, default 3), `+0x6F` (`DAT_00A58027`), `+0x73` (`DAT_00A5802B` invert-Y), `+0x74` (`DAT_00A5802C` mouse-look flag), `+0x75` (`DAT_00A5802D` manual-aim flag, disables auto-target), `+0x9D = DAT_00A58055` etc. Please re-check the other `DAT_00A580xx` labels.
2. **HudScreen.md** (`ControlMgr +0xAC`): "cut-scene control" is wrong — it is the bow/crossbow **aim mode** flag. `+0x7C` = input-block byte (builtin 391), `+0x79` = "control switching / RTS enabled by script" flag, `+0x74` = camera follow state, `+0x8` = command points (RTS).
3. **Unit.md**: `+0x365..0x36D` is the shot direction written by `0x42BA40`; `+0x359/+0x35D/+0x361` the jump impulse; `+0x20F` (u32) = "action lock until" timestamp read by `0x500BA0` (writer not found); `+0x23F` = attack lock-out timestamp; `+0x237/+0x23B` = chain window START_TIME/END_TIME; `+0x235` = "chain letter queued" flag.
4. Ghidra names to apply: `0xA5B9E8 g_ControlMgr`, the function names in §1, `0xA51D5A g_last_key_scancode`, `0xA51D5E g_last_key_event`, `0xA51700 g_key_down[256]`, `0xA51800 g_modifier_bits`, `0xA516F4/F8 g_mouse_dx/dy`, `0xA516E4 g_mouse_wheel`, `0xA51806 g_joystick_present_ptr`.
5. Builtins: id 391 `set_charmove_detect_block_flag` = `0x65F670` -> `ControlMgr+0x7C`; id 380 `get_charmove_mode` handler not exported (expected: returns `ControlMgr+0x10`).

## 7. Open questions

* **Held keys do not repeat** [V]: `0x598BD0` pushes one record per DirectInput event (`0x592DC0` down, `0x592F00` up; no repeat generation, ring of 20). Every "pressed" test is therefore a true edge (the queue consumer that sets `DAT_00A51D5A` is not exported; TODO(re) whether the value stays latched until the next key event — if it does, zoom, which uses the edge test, keeps zooming while the key is the last one pressed). Port: edges from SDL key-down with `repeat == false`; treat the two zoom actions as *held* (camera.md describes them as continuous).
* **`0x485990` / aim-on-load** [V code, I effect]: it is called only by the savegame section reader `0x485930` (after the `0x126E`-byte image is read) and forces `+0xAC = 1` whenever the saved mode is neither 2 (click-to-move) nor 4 (RTS) — the two modes that have no aim state. The scenario start `0x41EA60` runs `control_reset 0x42DE70` (which clears `+0xAC` through `0x42DF50 -> 0x42A770`) **before** the load (`gamefile 0x477E80`) and `control_enter_game 0x42D980` after it; `0x42D980` does not touch `+0xAC`. So after a load in modes 0/1/3 the controller is in the aim branch: movement input is replaced by `aim_input` (no walking) and the hero only leaves it when hit (`hit_counter`), dies, or the shoot key is pressed (toggle). It only visibly arms the bow if the hero has a ranged weapon with ammo (`0x42AD50` starts SHTLOAD). Probably an original quirk/bug (the intent was likely `= 0`); needs one dynamic confirmation (load a save, try to walk). The port should **clear** aim mode on load.
* TODO(re) remaining: `0x43F000`, `0x43C480`, `0x42D180`, `0x4322D0` (mouse sensitivity), `0x43EF30`, `0x518890`/`0x445000`/`0x4533E0`/`0x470430`/`0x466D50` (mount/ladder callees), `0x5945D0 / 0x594360 / 0x597ED0 / 0x597E00 / 0x597C80` (mouse/joystick polling), `0x439110 / 0x43A3B0 / 0x43A580 / 0x43A650 / 0x43A720 / 0x43A740 / 0x43ABD0` (mode-2 picking), `0x442EF0`, value of `0x9CFC50`.
* `Unit.+0x213 anim_position == 1` is tested as "not on foot" by combo_input but `mounted_combo` runs with `+0x213 != 1`, so position 1 is not the horse-riding pose; its meaning (probably a special pose such as camp/siege) is unresolved.
* Writers of `ControlMgr+0x79` outside mode switching; the unit fields `+0x20F`, `+0x23F` writers.
* Keys 0x32/0x33 (`Obrot widoku`) are only used in RTS; the free camera rotation in modes 0..3 does not exist (camera.md).
