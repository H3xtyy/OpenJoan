# HudScreen — the in-game interface screens (`interfac\*`, joan.exe)

Per-screen field layouts, widget inventories, layout constants, the game state each screen reads and how it reacts to input. Function table:
[functions/hud.md](../functions/hud.md). Design/port input: game/hud.md. Widget toolkit: engine/ui2d.md
(base classes [Widget2D](Widget2D.md), [Button2D](Button2D.md), [Image2D](Image2D.md); not repeated here). Minimap: [Minimap.md](Minimap.md).
Status: **V** read in >= 2 functions, **R** read once, **I** inferred. Offsets are byte offsets into the screen object (`this`), packed.
Only update/open/close/free code of the screens is in the export; the **draw** functions (0x6F97D0.., 0x6B82D0 ..) are not, so *what is drawn where* is partly
inferred from widget rectangles and BmpRes names.

## 0. Conventions shared by every screen

* **Layout space.** All rectangles below are **1024x768 raw pixels** (`virtual_coords` byte `+6 == 0`; ui2d.md §2). Screens opened from inside a running game (`DAT_00A500C2 == 9`)
  use a vertical baseline that is **0x30 (48) px higher** than the same screen opened from the main menu (`y_base = (DAT_00A500C2 != 9 ? 0x30 : 0) + 0x200`, 0x73DB40; options/save menus
  add `-0x30` to every y when mode 9). Off-screen parking positions: x = -0x400 / +0x400 (left / right), y = -100 (above), 0x2C8 (below).
* **Widget lifecycle (V).** Every widget is created once in the screen constructor (`new 0xB7` Image2D via 0x629400 + init 0x629660; `new 0x346` Button2D via 0x620EE0 + init 0x621540/0x6211D0),
  then `fade_to(0, ..., immediate=1)`. Opening = `move_to` from the parking position + `fade_to(1)` with per-widget **delay in ms** (stagger 0x1E = 30 ms per row);
  closing = the reverse; a small per-screen **state counter** advances when the last widget reaches its target (read via `widget.x/y/alpha`), and the screen is deactivated when the state reaches its
  last value. The tween setters and their exact units are in §1 of functions/hud.md (`move_to(x,y,speed,accel,immediate,delay_ms)`, `fade_to(alpha,speed,accel,immediate,delay_ms)`).
* **Standard tween constants.** Slide-in/out of rows and panels: `speed 3500.0` px/s (`0x455AC000`), `accel -1500.0` px/s^2 (`0xC4BB8000`); fades: `speed 6.0` alpha/s (`0x40C00000`) for panels and
  rows, `10.0` (`0x41200000`) for the in-game menu frame, `5.0` (`0x40A00000`) for the menu title/bars, `4.0` (`0x40800000`) for object-menu lists, `12.0` (`0x41400000`) for save-menu rows,
  `1.5`/`1.2`/`1.3` for the ticker/banners. Row stagger `delay = k * 30 ms`; after-open sounds `BUTTON_SLIDE_MANY` (many rows) and `MENU_SLIDE_IN`/`MENU_SLIDE_OUT` (menu frame) through 0x4CEEF0.
* **Opening any full-screen interface (V; buy, barracks/workshop, letter, objmenu, options, save/load, game menu).** The open function does, in this order:
  `if (DAT_00A500D2 != 0) { DAT_00A500D2 = 0; DAT_00A500D0 = ftol(current game speed); GameApp::set_speed(0) (0x42F930) }` — i.e. **the world is paused (speed 0) and the previous speed is remembered**;
  then `0x592C30` (sets `DAT_00A4860A = 0` and re-applies the mouse rectangle via 0x4918D0; inferred meaning: "UI cursor mode — free pointer"). Closing does the inverse: `if (DAT_00A500D2 == 0) { DAT_00A500D2 = 1; set_speed((float)DAT_00A500D0) }` and `0x42FEF0` (per control-mode pointer mode: ControlMgr mode 0, 1, 3 -> `0x592BD0` = `DAT_00A4860A = 1` "game pointer mode", mode 2 -> `0x592C30`, mode 4 -> clears `ControlMgr+0x1248`). `set_speed(0)` also stops the sound-effect and AI-sound processing (`0x45F6B0`, `0x49C080`, `0x63C6D0` with arg 0); speed != 0 restores them.
  `DAT_00A500D2` therefore means "no interface is suspending the game" (1 = running). Several update roots test it (0x6E5280).
* **Per-frame, not per-ms.** Most counters inside the screens advance **once per update call** (HP chase 9 per call, ...). The original update runs every frame (~333/s uncapped under Wine, nominally 30/s);
  a re-implementation has to step them once per update call. Tween delays/timers use the frame delta `DAT_00A500B6` (ms) and are tick-safe.

## 1. Singletons and globals read by the screens

| address | what | evidence |
|---|---|---|
| `DAT_00A5B9E8` | **ControlMgr** (charmove) object (receiver of 0x42F700): `+0` = recno of the controlled character (the "main character"); `+0x10` current control mode (`DAT_00A5B9F8`, 4 = RTS), `+0x14` target mode; `+0x81` mode-switch pending; `+0x1235` / `+0x1248` input-latch bytes cleared when a cut scene runs / the cursor mode is 4; `+0xAC` "cut-scene control" byte | V 0x42F700, 0x42FEF0, 0x6D3590 |
| `GameState+0x1E2` (0x430190) | i32, non-zero while the player is mounted? — makes 0x6D3590 skip the "mount" candidate (inferred) | R |
| `GameState+0x744` (0x4301C0) | cut-scene flag (`set_cut_scene_flag`) | V |
| `DAT_00A52930/34/38` | **ScreenMgr** `{ cur, pending, from_menu }` (i32 each) — §2 | V 0x6F5CC0, 0x6F43E0 |
| `DAT_00A55AD0` | **MenuFrame** `{ active, state, ... }`; `DAT_00A55AD1` = `state` (0 none, 1 sliding in, 2 shown, 3 sliding out) — §3 | V 0x6F24D0, 0x6F2340 |
| `DAT_00A500D2` / `D0` | "game running" flag / saved speed (see §0) | V |
| `DAT_00A500C2` | game-mode byte: 9 = in-game world running, 5 = frozen world (no world update) | V (0x4E5FC0, 0x73DB40, 0x751750) |
| `DAT_00A500B6` / `B2` / `BA` / `BE` | frame dt ms / game time ms (speed scaled) / real clock ms / scenario-flow step ms | TIMING.md |
| `DAT_00A58FA7` | GameState.scenario (the HUD tests 5, 6, 8 for per-scenario UI variants) | V |
| `DAT_00A596C4` | **GameState+0x734: player gold** (snapshot by the buy screen 0x6C3230: `this+0x57 = gold`) | R |
| `DAT_00A591C7` | GameState+0x237 `objmenu_open` byte (builtin `is_objective_menu_opened_in_scenario`) | V 0x7226A0, builtin 403 |
| `DAT_00A5D384` | modal-UI-busy byte: while set the status/ticker updates are skipped (0x721270, 0x6E5280) | R |
| `DAT_00A4FCA8`, `DAT_00A48950`, `DAT_00A48564` | other "UI busy" bytes consulted by the ticker | R |
| `DAT_00A47BB4` | game minute of the day (0..1440) | V (camera/lights docs) |
| `DAT_00A45F3E` | World* (`+0x6E` = light system, `+0x72` entity count, `+0x7A` entity array) | R |
| `DAT_00A4885C/60` | back-buffer width/height | V ui2d |

## 2. ScreenMgr (0xA52930) — which full-screen interface is open

`{ i32 cur; i32 pending; i32 from_menu }`. `0x6F5E90` resets all three. `0x6F5CC0` (activate): `from_menu = (pending != 0 && cur == 1)`, `cur = pending`, `pending = 0`, then opens screen `cur`:

| id | screen | opener | notes |
|---|---|---|---|
| 1 | in-game menu (`IGAMEMNU`) | 0x6F3730 | the 0x6F43E0 "update" below |
| 2 | character/combo interface (`CB_*`, `WP_*` buttons) | 0x6F7F20(1) + 0x6F7D80(DAT_00A5B9E8) | in RTS mode (`DAT_00A5B9F8 == 4`) uses the unit chosen by 0x6F5ED0 instead; the update 0x6F7D40 is an empty stub |
| 3 | save game (`SAVEMENU`) | 0x74DA40(0, from_menu) with `DAT_00A45CCB = 0` | list of 10 slots per page |
| 4 | load game | 0x74DA40(0, from_menu) with `DAT_00A45CCB = 1` | |
| 5 | options (`OPTMENU`) | 0x73C9D0(from_menu) | |
| 6 | objective menu (`OBJMENU`) | 0x7226A0(from_menu) | script `show_scenario_brief` sets pending = 6 then calls 0x6F5CC0 |
| 7 | message log (journal) | 0x6E19B0(1, from_menu) | sums `0x6E2910(i)` over the 9 message categories (MsgLog.cat_count) and shows page `0x6E29A0(page)` |

`from_menu` is forwarded to the opener (`1` = "opened from the in-game menu": closing returns to the menu; `2` seen as an argument of 0x74DA40 = opened for load-after-death).
Closing a screen that was opened from a hotkey: if `pending == 0` the manager is reset (0x6F5E90), otherwise the next screen is activated (chain: options -> menu).

## 3. MenuFrame (0xA55AD0) and the in-game menu (0x6F43E0)

`MenuFrame`: `+0 active`, `+1 state`, `+2 title[0x1D]` (`0x6F2AC0(title)` copies the caption and centres two decorations `l_arrow`/`title` BmpRes entries at `x = (title_w - text_w)/2 - l_arrow_w + 0x136` and `(title_w + text_w)/2 + 0x13A`, `y = 0x36`, immediate),
`+0x0C/+0x10` textures `COMMELE2`/`COMMELE3` (loaded by 0x6F3330 with the texture flag set to 1; freed by 0x6F3630), `+0x20..+0x2C` four more textures (freed by 0x6F20F0).
* **Open** (0x6F24D0): play `MENU_SLIDE_IN` if it was inactive; `state = 1; active = 1`; load frame textures (0x6F1FF0); four frame widgets slide to their places (`move_to` speed 5000 px/s: `(0,0)`, `(0x200,0)`, then
  `(0,0)` and `(0,0x2C0)` at 900/320 px/s), three fades at 5.0 and five at 10.0 with delay 200 ms.
* **Close** (0x6F26E0): `state = 3`; widgets slide away (`(-0x200,0)`, `(0x400,0)` at 5000 px/s; `(0,-0xB8)`, `(0,0x300)`), fades to 0 at 5.0/10.0; sound `MENU_SLIDE_OUT`.
* **Update** (0x6F2340, per frame): ticks 12 widgets (0x61F3B0 each). `state == 1` and last fade reached `alpha == 1.0` -> `state = 2` and 0x6CDFA0 (abort the cut-scene overlay if one is active, section 12);
  `state == 3` and alpha `== 0` -> `state = 0; active = 0`, free textures (0x6F20F0).
* **In-game menu screen** (id 1; *this* = the object of 0x6F43E0): `+0x24` open flag, `+0x26` open request/phase, `+0x27` closing, and 11 widget "present" bytes at `+0x5D, +0x3A3, +0x6E9, +0xA2F, +0xD75, +0x10BB, +0x1401, +0x1747, +0x1AB6, +0x1DD3, +0x1E8A`
  (each widget is a 0x346 Button2D; the byte tested is its `visible/ready` field, stride 0x346 = 838 -> widgets at `+0x5D-0x74`...). Phase 1 (frame shown, `DAT_00A55AD1 == 2`, `+0x26 == 1`): the title (0x124, 0x6E) and **eight** rows at `x = 0x100`, `y = 0x24E, 0x20A, 0x1C6, 0x182, 0x13E, 0xFA, 0xB6, 0x72` (pitch 0x44 = 68 px) slide in with `delay = 44 ms * row` (0x2C ... 0x160 = 352 ms), speed 3500 / accel -1500, the bottom bar to `(0, 0x2C8)`, then one `fade_to(1, 6.0, delay 200 ms)`; sound `BUTTON_SLIDE_MANY`; `+0x26 = 2`. Only 5 of the 8 rows have `IGAMEMNU` captions below; the remaining three are `TODO(re)`.
  When all button states are idle the closing logic runs: `+0x27` set -> 0x6F3630 (free), then either `0x6F26E0` (menu frame out) when `pending == 0 || 2`, or the next screen (0x6F5CC0).
  `0x6F43E0` returns immediately when the menu is closed (`+0x24 == 0`); when `0x4303F0` (menu "alive") becomes false it clears `+0x24`, sets the title (0x6F2AC0 with the default caption `DAT_009CFB9C`), restores game speed if `DAT_00A500D2 == 0`, and activates the pending screen / resets ScreenMgr.
  Captions: `IGAMEMNU` strings 1, 2, 3, 5, 0xD (getters 0x4DABD0, 0x4DAC10, 0x4DACC0, 0x4DAC50, 0x4DAE50), each formatted with the key bound to action index `0x26, 0x25, 0x27, 0x28, 0x29` of the current key table (`0x4CB5B0(DAT_00A58023, idx)` = key display string).
  `0x6F3730` (opener) also does `0x6CA000()` (set `cutscene_fx+3`), and for scenario 6 loads a savegame header `dir + DAT_009CFB78 ...` (0x477640) and clears the flag again (0x6CA020) when that save is not a finished scenario-6 save (`+0x19A == 6 && +0x19E == 1`).

## 4. HudMain — status bars and the objective banner (`this` of 0x6FD8B0; also receiver of 0x707CE0 show_objective)

Updated every frame (gameloop step 2b) while a main character exists (`0x428B70(DAT_00A5B9E8) == 0`). `U` = `0x41B3C0(DAT_00A5B9E8)` (the main character unit).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x20 | 4 | i32 | `hp_shown` | displayed HP: chases `U+0x135` (current HP) by **at most 9 per update** (`min(|delta|, 9)`) | V 0x6FD8B0 |
| 0x24 | 4 | i32 | `ep_shown` | displayed **energy points (EP)**: chases `U+0x145` by at most **3** per update (the draw code labels it PEPBAR, max EP from 0x523700) | V |
| 0x28 | 4 | i32 | `exp_shown` (experience points; `U+0x124` = level, max 30; thresholds `0x4A4E60(level)+1`) | follows `U+0x126`: **rises by at most 500 per update, falls immediately** (`delta = diff < 0x1F5 ? diff : 500`, `diff` negative -> jump) | V |
| 0x2C | 4 | i32 | `target_hp_shown` | displayed HP of the **target** bar: chases the target's HP by at most **0x28 = 40** per update; snaps to 0 when the target HP is 0 | V |
| 0x30 | 1 | u8 | `frame_state` | HP bar frame colour: 0 healthy, 3 warning, 5 critical. Set via 0x6FE4A0 (`prev = cur; cur = v`): `hp_shown < (2*U+0x13D)/3` -> 3, and `< U+0x13D / 3` -> 5; otherwise 0 (integer division) | V |
| 0x31 | 1 | u8 | `frame_prev` | previous state | V 0x6FE4A0 |
| 0x33 | 4 | i32 | `target` | recno of the target whose bar is shown (0 = none). Set by 0x6FE280(recno) which also stamps `+0x38 = DAT_00A500BA`, sets `target_hp_shown = target HP (+0x135)` and `+0x37 = 1` (unit) when the entity is a live unit. The target expires **3000 ms** after the stamp (0x6FD8B0 calls 0x6FE280(0)) | V |
| 0x37 | 1 | u8 | `target_is_unit` | 1 = unit target (HP at `+0x135`), else a structure: entity `0x4853E0(target)`, kind `entity+0x4E`: 1 -> HP from `(*vtbl[0x4C])()+0xC2`, 4 -> `(*vtbl[0x58])()+0xC6` | R |
| 0x38 | 4 | u32 | `target_t` | stamp (real ms clock) of the last target set | V |
| 0x14D | 1 | u8 | `obj_active` | objective banner running (`show_objective` sets 1) | V 0x707CE0, 0x6FD8B0 |
| 0x14E | 1 | u8 | `obj_flag` | cleared by `show_objective` | R |
| 0x14F | 4 | i32 | `obj_state` | 0 hidden, 1 shown, 2 hidden-but-armed; `show_objective`: `0 -> 1 (obj_active=1, obj_t=now)`; `2 -> 1 (obj_t = ftol(x87 value), obj_active=1)` | R 0x707CE0 |
| 0x153 | 4 | u32 | `obj_t` | start stamp; while the cut-scene flag (`GameState+0x744`, `0x4301C0`) is set the update **adds `DAT_00A500B6` to it each frame**, i.e. the banner timer is frozen during cut scenes | V |

After the chase code the update calls the **minimap update** (0x70EC50, Minimap.md). Everything else on the bars (positions, bitmaps `HP`/`goldbar`...) is in the draw code (not exported).
Unit fields used: `+0x135` HP, `+0x13D` max HP, `+0x145`, `+0x126` (see §14; semantics of the last two are inferred: stamina/focus and experience or gold).

## 5. InteractPrompt (`this` of 0x6D3590) — "press INTERACT" candidates

Runs every frame while the main character is alive. If `DAT_00A5B9F8 == 4` (RTS mode) the prompt is hidden/shown from the stored `+0x1D` only. Otherwise it recomputes the candidates (all searches use the character `P` = the receiver of the helpers, distance = |P.pos - candidate.pos|, forward cone +-90 degrees via 0x505960):

| field | candidate | search | radius | evidence |
|---|---|---|---|---|
| `+0x11` | talk target | 0x46C1F0: units in P's nearby-unit list (head at `P+0x339`, link `unit+0x33D`) with `unit+0x2E9 != 0`, not P, not main char, not dead (0x42D0E0), type flag `T+0x11C & 0x200`; nearest in cone | 100 | V |
| `+0x15` | second unit interaction class (unknown) | 0x46FA80: `0x5094D0(unit)` true, `unit+0x2EB == 1`, height difference `|dz| <= _DAT_009D03C0`; `TODO(re)` what `+0x2EB == 1` means | 200 | R |
| `+0x19` | prisoner to free | 0x471C80: `0x5094D0(main) == 0`, `0x432B90(unit)` (prisoner flag), `unit+0x2E9 == 0` | 100 | R |
| `+0x25` | recruit target | 0x48DFB0: same nation (`+2`), different team id (`+0x31`), `unit+0x104 < 3`, not hidden (`+0x2F == 0`), `unit+0x384 == 0`, type class `T+2 not in {0,8,9}`, `T+0x118 in {0,1}`, not prisoner; only if P has a team (`P+0x31 != 0`, `P+0x33->+2 < 0x448080()` = team-size cap) | 100 | R |
| `+0x21` | rideable unit | `0x44F0B0(2)`: first unit (scan from the last recno down) with `unit+0x3C0 != 0` (can-be-ridden flag, set by builtin `set_unit_can_rided_flag`), not P, and `T+0x118 == 2`, within 70 (arg 3 -> 150 for category 3); skipped when `GameState+0x1E2 != 0`; cleared if the found unit has `unit+0x1E6 != 0` (0x4F2170) | 70 | R |
| `+0x0D` | item / usable entity | 0x4D6320(200): entities with `entity+0x7E == 0x200` (the `set_item_can_interact_flag` value) or kind `+0x4E == 0xB`; kind 1 entities also need a low state byte; kind 0xB uses a front/back test with a 2-plane dot product | 200 | R |
| `+0x29` | facility of kind 3 | 0x514B80: facility object `0x43C450(i)` with `[1] == 3`, owner nation == P's, and P inside the facility area (0x60D5F0) | - | R |
| `+0x2D` | facility of kind 0x10 | 0x5156D0 same with kind 0x10 | - | R |

Evaluation order (V): `+0x11, +0x15, +0x19, +0x25`, then `+0x21` (skipped when mounted), and only if **all five are 0** the fallback chain `+0x0D` (item), then `+0x29`, then `+0x2D` (each only if the previous was 0).
If anything is non-zero the prompt is shown (0x6D3450: `+8 = 1`, texture load 0x6D34D0), if everything is 0 it is hidden (0x6D3480: `+8 = 0`, free 0x6D3540). Which candidate the INTERACT key acts on when several are set is decided in the input code (not exported): `TODO(re)`.

## 6. TickerSms (`interfac\i_sms.cpp`, "message ticker"; `this` of 0x6E5280)

Two embedded Image2D widgets: `A` at `this+0x08` (plaque, BmpRes) and `B` at `this+0xBF` (text label); `this+0` is the `QSAVE` texture handle (0x6E5F50 loads it with texture flag 1).

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x176 | 2 | i16 | `state` | 0..5 |
| 0x178 | 4 | u32 | `timer` | ms elapsed in state 3 |
| 0x17C | 4 | u32 | `hold_ms` | display time; 0x6E5E60(ms) sets it, **default 3000** when `ms < 1` |
| 0x180 | 4 | i32 | `text_w` | width of the current text in px (`0x624F00(text, -1, 0, 0, 1.0)`) |
| 0x184 / 0x188 | 4 / 4 | node* | `head`, `tail` | queue (doubly linked) |
| 0x18C | 4 | i32 | `count` | queued messages |
| 0x190 | 1 | u8 | `active` | set by 0x6E5E60 |

Node (0x14 bytes, allocated `new 0x14`): `+0 char* text` (heap copy), `+4 i32 kind`, `+8 i32 flag`, `+0xC prev`, `+0x10 next`. **Enqueue 0x6E5700(text, kind, flag)**: ignore empty text; if `flag == 1` first delete every queued node whose `flag == 1`;
`kind` 1 or 2 = **priority** (pushed at the *front* — becomes the current message, aborting the one shown: restarts state 0, resets position via `move_to(-text_w, 0x204/0x206, 1.0, 0, immediate)`), any other kind appends at the tail. Then `0x6E5E60(0)` (activate, default hold).
**Update 0x6E5280** (only while `active && !DAT_00A5D384 && !DAT_00A4FCA8 && !DAT_00A48950 && !DAT_00A48564`):
state 0: fade A and B in (speed 1.5) and slide to `x = 0` at `y = 0x204` (A) / `0x206` (B) at 2700 px/s (`0x4528C000`), accel -1000; state 1 -> 2; state 2 -> 3 when both reached `x == 0`, `alpha == 1`;
state 3: wait `timer += DAT_00A500B6 >= hold_ms`, then fade out (1.5) and slide to `x = -text_w` (speed 0, accel +1600 px/s^2); state 4 -> 5 when `A.x <= text_w * _DAT_009DE2A8`; state 5 when both reached `-text_w`: pop the head (0x6E5D00), `state = 0`, and if more messages: set the label text (`Image2D::set_text` 0x6298C0), re-measure.
The widgets are ticked (0x61F3B0 x2) only if the current message kind is 1/2 or `DAT_00A500D2 != 0` (game running).
Producers (not exported but visible in callers): item received by the main character (`give_item_to_unit` handlers 0x65C5F0/0x65C770 pass the formatted item name via 0x4DD6E0), quick-save/quick-load (0x6E3A90 -> `0x6E5E60`), kill/limit messages (0x4DD8A0..0x4DD920 string getters).

## 7. Quick-save / quick-load banner (0x6E3A90; `this`: `+0x45E` active, `+0x458` busy, `+0x45D` mode, `+0x452` state, `+0x454` timer, `+0x459` argument)

Mode 0 (quick-save): the `QSAVE` banner (widgets at `+0xC8`/`+0x11`) fades in (1.2/s) and slides to `x = 0, y = 0x204` at 700 px/s, holds `0x514 = 1300 ms`, fades out (1.5/s) and accelerates off to `x = -220` (accel 1600); at the end `DAT_00A48C78 = 1` and 0x6E44B0 (deactivate).
Mode 1 (quick-load): first clears the ticker queue (0x6E5E00), slides three banner pieces in (fade 1.3/1.4), waits **500 ms** in state 2, then calls **`battle::0x41EA60(arg)`** (the scenario/stage loader), restores speed (`DAT_00A500D2`), holds 100 ms, flips alphas, holds 1300 ms, slides out, calls `0x6E5E60(0)` (ticker) and `DAT_00A48C5E = 0`.

## 8. TitleScreen (`display_title_screen`, object checked by 0x761630 / 0x7616F0)

`{ u8 active @0; u32 start @1; u32 duration_ms @5; u32 p3 @9; u32 p4 @0xD; char text[0x1E] @0x11 }`. Builtin `display_title_screen(text, a, b, c)` (0x65C420) -> `0x761680(now = DAT_00A500BA, text, a, b, c)` sets `active = 1`, `start = now`, `duration = a`, `+9 = b`, `+0xD = c`, `strncpy(text, 0x1E)`.
Update 0x761630 (gameloop 2b): `active && start + duration < now(DAT_00A500BA)` -> `active = 0`. The poll of the WAIT is `!active` (0x7616F0). `b`/`c` are the fade-in/out ms (S1L1: `"Relieving the Siege of Orleans", 6000, 2000, 2000`, commented out in S1L1.sce). Draw code: not exported.

## 9. KillInterface (object at ~0xA50518; builtins 365..370)

`+8 u8 shown1`, `+9 u8 shown2` (= `DAT_00A50520/21`), `+0xA i32 limit1` (`set_kill_citizen_limit(n)`: `DAT_00A50522 = n` if `n > 0`), `+0xE i32 limit2` (`DAT_00A50526`), `+0x12 u8 mode1`, `+0x13 u8 mode2`.
`start_kill_interface(mode=0)` (0x65E810, only if `shown1 == 0`) -> 0x6DFE10: `shown1 = 1; mode1 = mode`; if `mode in {0,1,4}` then for every live unit whose type record has `+2 == 0` (class 0 — the "citizen" class, inferred) and that has an overhead bar (`unit+0x31D != 0`) set `bar+0x52 = 0` (hide); if `shown2 == 0` show the counter (0x6E03D0).
`end_kill_interface` -> 0x6DFF10: `shown1 = 0`; if `shown2 == 0` restore the bars (`+0x52 = 1`) and hide (0x6E0440). The `_2` variants (0x6E00B0, 0x6E01D0) mirror this with `shown2/mode2`. The counting/draw code is not exported (`TODO(re)`).

## 10. LetterScreen (`interfac\i_letter.cpp`; object: `+0x08 open`, `+0x09 flag`, `+0x0D i16 item`, `+0x0F char* text`, `+0x13 i32 len`, `+0x17 i16 lang`, `+0x19 i16 page`)

`show_letter_interface(item)` = `0x6E0B70(item)` then `0x6E0900()`. 0x6E0B70: free the old text; build the path `"ITEM_DES\" + item_record+0x25F` (the item record is `0x425CE0(item)`; `+0x25F` is a 32-char description file name); language code from `item_record+0x27F`: string `"English"` -> `lang = 1`,
`"French"` -> 2, else 0; open the file (0x586DA0), read it whole into a `len+2`-byte buffer (0x5873A0). 0x6E0900 (open): `open = 1; page = 0`, prepare textures (0x6E0DC0), hide the cursor-capture (0x6F... `0x6FE240`), compute the page count `0x6E0DF0()` -> if `> 0` run `0x6F7E90` (close character interface), run `0x6E0AB0`; pause the game as in §0.
`is_letter_closed` (WAIT poll) = `!open` (0x6E0A90). Close = 0x6E0520 (free text). Page layout/draw code: not exported.

## 11. Shop (`interfac\i_buy.cpp`; object fields)

| offset | size | name | meaning |
|---|---|---|---|
| 0x10 | u8 | `open` | open flag (`0x6C9040` returns it; WAIT poll of `start_buy_interface` is `!open`) |
| 0x12 | i32 | `shop` | facility recno (`start_buy_interface` arg 1; `0x655B70` setter; validated with `0x44CF80`) |
| 0x16 | i32 | `keeper` | unit recno shown beside the shop (arg 2; `0x655BA0` setter; validated with `0x428B70`) |
| 0x30 | Inventory* | `stock_copy` | working copy of the player's inventory (`0x497A10(U+0x1C1)` copy-construct) |
| 0x56 | u8 | `built` | lists built for this opening |
| 0x57 | i32 | `gold0` | gold at opening (`DAT_00A596C4`) |
| 0x5B | i32* | `shop_items` | one i32 per shop entry (`item.+6` of each facility stock entry), allocated `4*count` |
| 0x134 / 0x21D | ptr | `cam_a`, `cam_b` | per-side scene cameras for the two character portraits |
| 0x138 / 0x221 | Actor* | `actor_a`, `actor_b` | the two displayed characters (side 0 = keeper, side 1 = main character); `0x6C92E0(side)` builds them from the unit's type record: animation script `T+0x84+4*appearance`, model `T+0x90+4*appearance` (appearance index = `unit+0x3C5`) |
| 0x13C/0x225, 0x140/0x229, 0x144/0x22D | | per-side model/anim bookkeeping | |

* **Open** `0x6C8D00(1)`: `open = 1`, 0x6C9060 (load textures), pause game (§0), `+0x231 = 0`. **Close** `0x6C8D00(0)`: free the shop-item array and the stock copy, free both actors/animations, `open = 0`, restore input (0x42FEF0) and speed, 0x6C90E0 (free textures), 0x595A20 (clear input).
* **Build lists** `0x6C3230` (called each frame until `built`): requires the main character and the shop facility (`0x43C450(shop)`, virtual slot `+0x20` returns the stock object); then spawns the portraits (0x6C2DD0: both actors play `F_IDLE`; cameras placed at `(65, -225, 228)` with offset +375 or `50` + `_DAT_009D74CC` for the unit named `LAHIRE`), copies the player inventory, creates three grid containers (`0x4571E0`: sizes via `0x457290(1,4,1)`, `(1,6,8)`, `(1,6,4)`), and for each shop entry `0x4577A0(item, ?, &slot, a, b, c, 0)` places the item into the shop grid. Shop entries have `{i16 item; i16 count?; i16 price?; i32 +6..}` — layout `TODO(re)` (owned by the items family: items_facilities.md).
* **Idle animation** `0x6C8C00` (per frame while `open`): if `0x6C9130(side)` (remaining time of the current clip: `clip_len - (now - clip_start)`, looping with `%`) is 0 for `actor_a` / `actor_b`, restart `F_IDLE` via `0x5ABE30(actor.+4, "F_IDLE", DAT_00A500B2, 1, 1, 500.0)`.
Buy/sell rule code (price lookup, `+0x161`/`+0x165` costs, gold changes) is in the input handler that was not exported; `i_work` shows the same arithmetic (§12).

## 12. Barracks (`i_barr.cpp`, 6 rows) and Workshop (`i_work.cpp`, 4 rows)

Widgets (constructor 0x6BC530; workshop 0x6E8F40 identical plus a third button): background Image2D (`+0x24`, full screen, BmpRes `DAT_009DDBCC`), panel button `(0x400,0x5B)-(0x568,0x29F)` (`+0x28`, parked right, slides to x=0x27B), title button `(0x138,7)-(0x2C4,0x23)` (`+0x2C`), `N` row buttons `(0x24,-100)-(0x224,0)` (`+0x0C` array, slide to `y = 0x68 + 100*i`), `N` icon buttons `ia_ov/ia_ov/ia_ov` at `x = 0x1D8, y = 100*i + 0x68` (`+0x10`), `N` "right arrow" `aw_r_ot/dw/ov` at `(0x1EC, 100*i + 0x7C)` (`+0x14`), `N` "left arrow" `aw_l_*` at `(0x1EC, 100*i + 0x9C)` (`+0x18`), OK `(0x28B,0x2BE)-(0x327,0x2EA)` and Cancel `(0x32E,0x2BE)-(0x3CA,0x2EA)` buttons (captions `BARRACK` strings 1 and 2, getters 0x4DDA00/0x4DDA30; workshop `WORKSHOP` 1, 2, 0x12, 0x13), workshop help button `(0x26,0x2BE)-(0xC2,0x2EA)`.
* **Update** (barracks 0x6BFE00: `+0x30 active`, `+0x33 state`; workshop 0x6ECE80: `+0x34 active`, `+0x37 state`, bg widget `+0x24`/`+0x28`): ticks all widgets (24+5 / 16+6 ticks); state 0 (opening, once the background alpha is 0): rows `i = N-1 .. 0` get `move_to(0x24, 100*i + 0x68, 3500, -1500, 0, delay = 30 * (N-1-i))` + `fade_to(1, 6.0)`, plus the extra fades and the panel `move_to(0x27B, 0x5B)`; goes to state 1 when row 0 reached `alpha == 1 && y == 0x68`;
  state 2 (closing, when the background alpha is 1): rows to `y = -100`, fades to 0, panel to `x = 0x400`; goes to state 3 when the last row is at `y == -100`, `alpha == 0`; state 3: `active = 0`, restore input (0x42FEF0), free textures (0x6C04D0 / 0x6ED5B0), restore speed if `DAT_00A500D2 == 0`, `0x592BD0`, `state = 0`, `_DAT_00A5BA65 = 0`.
* **Workshop input** (0x6EC480; `this`: `+0x34 active`, `+0x35 i16 hovered row`, `+0x37 i16 phase (1 = interactive)`, `+0x39 i32 facility recno`, `+0x41 i32 budget`, `+0x45 i32 spent`, `+0x49 i32 queued_total`, `+0x51 help_open`, `+0x62 i32 queued[k]`): the facility `F = 0x43C450(+0x39)` and its object `W = F->vtbl[0x1C]()` give `W+0xA2 i16 n` (number of producible unit types), `W+0xA4+2k` (unit-type ids), `W+0x79 i32 kind` (0 normal, 1/2 resource), `W+0x7D i32 max_queue`.
  Hover = point in row rect (`0x593980`, widened by `+0x4C` on the right). **"+" click** on row `k`: if `queued_total >= max_queue` nothing; cost `c = unit_type[ids[k]].+0x161` (kind 0) or `.+0x165` (kind 1, 2); if `budget < spent + c` nothing; else `spent += c; queued[k]++; queued_total++`. **"-" click**: if `queued[k] < 1` nothing; `spent -= c; queued[k]--; queued_total--`. ESC (key 0x1B) or Cancel -> `0x6ED3E0(0)` (close without queueing); Enter (0xD) -> `0x6EA1A0()` (commit) then close; a middle button opens the help page text `workshop_normal_help.txt` / `workshop_resource_help.txt` (file read whole, shown by 0x6D5230/0x6D52C0 in a `GPOPUP` window `0x6D3BE0`).
  The commit (0x6EA1A0, 288 B) and what the queue does to the world are in the facilities family.

## 13. ObjMenu — the objective menu (`interfac\objmenu.cpp`, screen id 6; constructor 0x71D2D0, destructor 0x71E800)

Object fields: `+0x44` top `LONGBAR` strip (Image2D at `(0,0x84)`, parked `x = -0x400`), `+0x48` bottom `LONGBAR` strip (`(0,0x298)`, parked `x = 0x400`), `+0x4C` caption `TXT_FV` at `(0x19,0xB8)` (parked `x = -0x164`), `+0x50` array of **10 list-row buttons** `(0x17, 0xDC + 0x28*i)-(0x17F, 0x103 + 0x28*i)` (pitch 40 px, callback `LAB_00402293`), `+0x54..+0x6C` seven hub buttons `(0x100,0x3E)-(0x300,0x6E)` (captions `OBJMENU` strings 4, 5, 6, 7, 8, 0x2C, 0x3F), and the bottom tab buttons (captions strings 9, 10, 0xB, 0xC, 0x1A, 0x4B):
`(0x1CE,0x298)-(0x232,0x2B6)` (`+0x70`, str 9), `(0xA2,..)-(0x16A,..)` (`+0x74`, str 10), `(0x19C,..)-(0x264,..)` (`+0x78`, str 0xB), `(0x296,..)-(0x35E,..)` (`+0x7C`, str 0xC), `(0x183,..)-(0x1E7,..)` (`+0x20D`, str 0x1A), `(0x219,..)-(0x27D,..)` (`+0x171`, str 0x4B); `+0x80 u8 page`, `+0x1`/`+0x2`/`+0x3` byte flags (`+1`: opened from menu variant "hub shows 4 buttons instead of 7"), `+0x269 i16 mission number` (selects `MISSION_BRIEF_1..8`).
* **Opener 0x7226A0(variant)**: `*this = 1; DAT_00A591C7 = 1`; variant 1 -> shows `OBJMENU` string 4 as the caption (`param_1[2] = 1`), variant 2 -> string 0x10 (`param_1[1] = 1`), else string 4; in scenario 8 first shows string 5 of group `DAT_009D2DF8`; `0x6CA000/0x6CA020` toggles cut-scene fx `+3` (scenario `< 5` clears, else sets) twice; `+3 = 0`; resets all sub-pages (0x7215F0: parks every strip/button, 8 sub-page resets 0x724670/0x7349B0/0x7388A0/0x728290/0x730BC0/0x72CA10/0x7364C0/0x73B140); opens the menu frame (0x6F24D0), selects page 0 (0x721A60(0)), loads textures (0x71EFC0: `COMMELE2/3`, `MB_COM`, `MB_FOCUS`, `MB_FRAME`, `MB_OBJ`, `MB_TOWN`, `OBJMASK`, `S1_SCR..S8_SCR`), pauses the game (§0).
* **Page switch 0x721A60(page)**: closes the old page (page 1 0x724BF0 etc.), `+0x80 = page`, opens the new one with staggered tweens. **Pages**: 0 hub (7 buttons slide to `x = 0x100`, `y = 0x24C, 0x20D, 0x1CE, 0x18F, 0x150, 0x111, 0xD2` at delays 500, 550, 600, 650, 700, 750, 800 ms; strips to `x = 0`; bottom tabs `0xA2, 0x19C, 0x296` appear); 1 mission brief (0x7246F0 `objmenub`: loads `SCRIPT\SCENARIO\<MISSION_BRIEF_n>` text, `n = +0x269` 1..8, via `0x4D0D80` -> `0x4CECD0`); 2 list page (0x7353A0: row fade 4.0, rows reset, 10 slots `+0x9D..`, `0x734A30`); 3 (0x738B50); 4 **commanders list** (0x728A90: widgets `(0x19,0xB8)`, `(0x226,0xB6)`, `(0x226,0x1E2)`; fills the list with every live unit whose `unit+0x104 > 3` through 0x4CDD40 after clearing 0x4CDDC0, then `0x728390(0)`); 5 **heroes** (0x730BE0: for each unit of the main character's side (`unit+2` equal), `unit+0x104 > 3`, `unit+0x30 == 0` (not hidden by `set_unit_hide_in_command_interface`), other than the main character, create a 3D portrait actor `MODEL\!<name>_M` or `_H` — `_H` when `unit+0x2A5 == 0 && unit+0x2AA != 0` (wounded/hurt variant, inferred) — with names `joan, metz, alen, lahi, bast, flav, char, rich` selected by the type name `Joan_of_Arc / Jean_de_Metz / Alencon / LaHire / Orlean / FLAVY / CHARLIE7 / arthur_de_richemont`; then one actor per **visible minimap marker** (type 0 at its own position, type 1 at the unit named by the marker, type 2 at the main character's position); position = world position * `_DAT_009DF420` snapped to the terrain via 0x5D4A80); 7, 8, 9 further list pages (siege/equipment: 0x72CA30/0x7365E0/0x73B160; `objmenuw` 0x73A320 hosts the six siege weapons `BATTERRAM, BOMBARD, CANNON, CROSSBOW, FAUCONNEAU, TREBUCHET` each with a `*.txt` help text and `F_IDLE` animation).
* **Close** (0x71F360 via the frame close): parks everything, `DAT_00A591C7 = 0`, restores speed, cursor.
Strings: group `OBJMENU` (77 entries, [txr.md](../../formats/txr.md)); getters 0x4DBB20 (4), 0x4DBB50 (5), 0x4DBB80 (6), 0x4DBBB0 (7), 0x4DBBE0 (8), 0x4DBC10 (9), 0x4DBC40 (10), 0x4DBC70 (0xB), 0x4DBCA0 (0xC), 0x4DBD30 (0xF), 0x4DBD60 (0x10), 0x4DBF40 (0x1A), 0x4DC2C0 (0x2C), 0x4DC330 (0x3F), 0x4DC550 (0x4A), 0x4DC580 (0x4B).

## 14. Options (`optmenui.cpp`, 0x7435E0 / 0x73DB40) and Save/Load (`SAVEMENU`, 0x751750 / 0x74DA40)

* **Options** (screen 5): update 0x73DB40 (`+0x122 active`, `+0x121 phase`, `+0x123 from_menu`): ticks 8 widgets + four group updaters (0x746FF0, 0x741960, 0x74B550, 0x742EE0); plays `BUTTON_SLIDE_MANY` once when the lead widget starts moving to `y = (mode == 9 ? 0x200 : 0x230)`; phase 6 (closing, lead widget at `y = 0x3E - (mode==9 ? 0x30 : 0)`, alpha 0, side strips at `x = -0x400/0x400`): close — `active = 0; phase = 0; 0x73C8A0 (free textures)`; restore speed and `0x592BD0` if `DAT_00A500D2 == 0`; if `from_menu`/pending: return to the menu. Opener 0x73C9D0: `active = 1; from_menu`, 0x73DED0 (reset groups), open menu frame if mode 9 (0x6F24D0), build pages (0x73CC00), textures `COMMELE2/3, MENU_B, OPTMENU, OPTMENUB, OPTMENUF` (0x73C740). Pages by key-binding category: ui2d.md §6. Group `OPTMENU` (56 strings). Combobox/slider helpers 0x6B29B0 (`aw_l_*`, `aw_r_*`, `lcm_*`, `lopt_*`, `scmc_*`, `sopt_*`, `soptbox`, `loptbox`).
* **Save/Load** (screens 3, 4): object fields `+0x4B05 active`, `+0x4B01 mode (0 save, 1 load, 2 death-load)`, `+0x4B02 i16 phase (0 opening, 1 steady, 2 closing, 3 closed)`, `+0x4B06/07/08` flags, `+0x4AFC..FE` page bytes (`+0x4AFE = ceil(slot_count/10)` pages), 10 row buttons at stride `0x346` from `+0xB6`. Phase 0 slides the title, rows (`x = 0x74`, `y = base + 0x12C + 0x28*(r-1)`, fade 12.0) and buttons in; when the row widgets are in place it fills a combo box with page captions `SAVEMENU` string 0x16 (`"%d / %d"` style, `0x4DCA00(page, pages)`) — pages = `ceil(0x417AD0() / 10)` where `0x417AD0` = number of saves. Phase 3 closes: mode 2 reopens the objmenu (`0x7226A0(2)`), mode 1 (load) runs `0x6B8AF0`; else restores speed and the menu.
  File I/O of the saves: savegame.md.

## 15. Quit flow (0x6E4A20, always updated) and quit prompt (0x6E4C00)

`0x6E4C00(mode)` (called with 1 from the paused handler, gameloop §4 step 6): `active = 1; +0xBD = mode`, loads the background texture (0x6E4D60, `DAT_009D2B18`), sets the BmpRes name `"bg_qm"` (mode 0, quit to main menu) or `"bg_qw"` (mode 1, quit to Windows), fades the plate in (6.0).
`0x6E4A20` (`this`: `+0xBE active`, `+0xBB i16 step`, `+0xBD mode`, `+0x15` the plate's alpha): ticks the plate; step 0 waits until alpha `== 1.0`; step 1 -> 2; step 2: **`0x41C780(0)` = `world_unload(0)`** and fade the plate to 0 (6.0, delay 1000 ms); step 3 waits alpha 0; steps 4 -> 5; step 5: mode 0 -> `set_next_state(3)` (main menu); mode 1 -> `DAT_00A50098 = 1; 0x70D670()` (exit-to-OS path); then deactivate (0x6E4CC0: alpha 1 immediate, free texture).

## 16. Cut-scene wipe/fade overlay (0x6CA660 update; `this`: `+4 active`, `+7 f32 rate`, `+0xB f32 alpha 0..255`, `+0xF i32 state`, `+0x13 u8 chain`, `+0x14 ptr camera_track`, `+0x30 arg`, `+0x34 i16 wipe_x`, `+0x36 u32 t`, `+0x3A Texture*`, `+0x3C` (texture width, via the pointer), `+0x3E u8 toggle`, `+0x3F u8 reverse`, `+0x40 u8`)

Started by the script handlers 0x65ADE0 / 0x65AEF0 (`start_cut_scene`, via 0x6CB9E0) and 0x65AFF0 (`end_cut_scene`, via 0x6CC6A0); `get_cut_scene_status` returns `+0xF`; `is_cut_scene_status_black_screen` is true when no cut scene is active or `+0xF == 7`.
States: **fade mode** (`state` not in 6..8): per frame, run the camera track when `+0x14 != 0` (0x425FC0), `alpha += step * rate / 1000` with `step = DAT_00A500BE` (clamped to 1000 ms), clamp `0..255` (`0x437F0000`); `alpha == 255 && 0x428AA0() == 0` -> `state = 2` (full black, steady), `reverse = 0`; `alpha == 0 && 0x428AA0() == 0` -> end: `active = 0`, 0x49B5E0(0), 0x4725F0(0), `state = 0`, camera restore 0x429860(arg), `+0x40 = 0`.
**Wipe mode** (a texture slides across the screen): state 6 (`reverse == 0`): `wipe_x += (tex_w + screen_w) * DAT_00A500BE / 0x5DC (1500)` until `>= screen_w` (the sweep takes **1.5 s**); then end (as above) when `chain == 0`, else `state = 1`; with `reverse != 0` the x decreases to `-tex_w` and `state = 7`, `t = DAT_00A500BA`, `alpha = 255`. State 7 (**black hold**): after `0x2ED = 749 ms` -> `reverse == 0`: `state = 6`, `wipe_x = -1`, camera callback 0x429860(arg); else `state = 8`, `wipe_x = screen_w`, `toggle ^= 1`. State 8: the mirrored sweep; at the end `state = 2` (or 1 when chained), `reverse = 0`, texture freed (0x60A550).
0x6CDFA0 aborts the overlay (`active = 0`, 0x49B5E0(0), 0x4725F0(0), free texture); the in-game menu frame calls it when it reaches state 2.

## 17. GlowBar patterns (0x6B7F20 `this+0x9E2..0x9EC`, 0x6E7CF0 `this+0xD28..0xD32`)

Two identical state machines (two screens; owners unknown, draw code not exported). Fields: `A` (`+0x9E2`/`+0xD28`, u8 pattern), `B` (`+0x9E3`/`+0xD29`, u8 phase 0..4), `t` (`+0x9E4`/`+0xD2A`, game time `DAT_00A500B2`), `active` (`+0x9EC`/`+0xD32`).
Runs only when `active` and `DAT_00A500B2 - t > 0x32` (**50 ms** of game time); then `t = now` and, in this order:
`A == 0`: `if B == 0: A = 1`; then, sequentially, `if B == 3: B = 4`, `if B == 2: B = 3`, `if B == 1: B = 2` (so `B` counts up by one per step).
`A == 1 -> 3 -> 7 -> 0xF -> 0x1F -> 0x1E -> 0x1C -> 0x18 -> 0x20` (a 5-bit pattern growing from bit 0 to all five bits and then receding).
Otherwise (`A` in {0x10, 0x20, 0x40, 0x60}): `if A == 0x10 && B == 3: B = 4`; `A == 0x20`: `B == 0 ? A = 0x40 : (A = 0x10, B = 3)`; `A == 0x40`: `B == 0 ? A = 0x60 : (A = 0x20, B = 2)`; `A == 0x60`: `A = 0x40, B = 1`.
Interpretation (inferred): `A` is a bit mask of lit segments of a 5-segment gauge (+3 trailing bits), `B` a hold/phase counter; the draw code tests the bits.

## 18. Entity / unit fields the HUD reads (cross-module — names are the HUD's guess; the unit family owns the page)

Entity table: `0x41B3F0(i)` (raw pointer, 1-based, null/free slot), `0x41B3C0(recno)` (checked unit), `0x428B70(recno)` ("is invalid"), count `0x417AD0`. Units and items share the table (`entity+0x4E` kind code: 1 and 0xB seen in the interact search).

| unit offset | type | meaning | used by |
|---|---|---|---|
| +0x00 | i16 | recno | everywhere |
| +0x02 | i16 | nation / side id (friend test) | minimap, heroes page, workshop |
| +0x0E/+0x12/+0x16 | f32 x3 | world position x, y, height z (camera target = z + `_DAT_009CFBE0`) | 0x5634E0, minimap |
| +0x30 | u8 | hidden in command interface (`set_unit_hide_in_command_interface`) | objmenu pages 4/5 |
| +0x31 / +0x33 | i16 / ptr | team id / team record (`team+2` = member count, ids from `+3`) | recruit candidate |
| +0x3B | u16 | unit type recno -> `0x42D0A0` type record | minimap, shop, interact |
| +0x104 | i32 | > 3 qualifies a unit for the commander/hero lists | objmenu pages 4/5 |
| +0x126 | i32 | status-bar value (chase 500/update) | HudMain `+0x28` |
| +0x135 / +0x13D | i32 | HP / max HP | HudMain |
| +0x145 | i32 | status-bar value (chase 3/update) | HudMain `+0x24` |
| +0x15F | u16 | equipped armour item recno (item name `ALENCON_ARMOR_LV1..3`, `LAHIRE_ARMOR_LV1..3`, `JEAN_ARMOR_LV1..3`) | attention-call portrait choice |
| +0x1C1 | ptr | inventory | shop |
| +0x2A5 / +0x2AA | i32 | `(+0x2A5 == 0 && +0x2AA != 0)` selects the `_H` hero portrait | objmenu page 5 |
| +0x2AE | ptr | animation/actor state (0 = not animated) | 0x6AD660 |
| +0x2E9 / +0x2EB | i16 | talk flag / second interaction class | interact |
| +0x31D | ptr | overhead bar object (`+0x52` visible byte) | kill interface |
| +0x339 / +0x33D | ptr | nearby-unit list head / next link | interact |
| +0x3C0 | u8 | can-be-ridden (`set_unit_can_rided_flag`) | interact |
| +0x3C1 | i32 | vision radius (`set_unit_vision_range`) | minimap fog |
| +0x3C5 | i32 | appearance index into the type record arrays `T+0x84/+0x90` | shop |

Unit type record `T = 0x42D0A0(type)`: `+2 i16 class` (0 = citizen-like; excluded from minimap vision and affected by the kill interface), `+4 char[] name` (`Joan_of_Arc`, `Jean_de_Metz`, `Alencon`, `LaHire`, `Orlean`, `FLAVY`, `CHARLIE7`, `arthur_de_richemont`), `+0x84/+0x90 i32[] anim/model per appearance`, `+0x118 i16 category` (0/1 recruitable, 2 mount, 7 excluded from fog), `+0x11C u32 flags` (`0x200` talkable), `+0x161 i32 cost` (normal), `+0x165 i32 cost` (resource), `+0x100 char[] display name` (the name shown in the HUD panel; `+0xB4` = class/troop name used by the target panel).

---

# Round 2 — draw side, input handlers, hotkeys, INTERACT (all from `decomp/game/hud2/` and `decomp/engine/{audio3,camera3,gameloop2,video2}/`)

Draw primitives (all take **screen pixels**, layer = small int, higher layers on top): `0x633500(x, y, layer, bmpres_name, texture, colour, 3, rs_a, rs_b, ..., src_x0, src_y0, src_x1, src_y1, rot)` = `draw_bmpres` (ui2d.md §3; `-1` source args = whole sprite; passing explicit source args crops, which is how every bar is filled); `0x6255D0(x, y, layer, text, colour, ...)` single-line text; `0x626080(x, y, layer, text, ...)` **right-aligned** text; `0x626130(x0, y0, x1, y1, layer, text, ...)` centred text in a box; `0x626E00(x0,y0,x1,y1, layer, text, ...)` wrapped block; `0x632A70` filled rect; `0x6332B0` textured rect from a raw texture. White = colour `0xFFFFFFFF`; the dimmed party portraits use `0x80FFFFFF`. Numbers are formatted by `0x4D69A0(n, digits)` / `0x58FCA0(n, digits)` (zero-free decimal, `digits` is a minimum width of 1 in all HUD calls).

## 19. Main HUD (`0x6F9B60(layer_mask)`; mask bit0 = sprite pass, bit2 = text pass; gameloop calls it for masks 1, 2, 4)

Skipped while the cut-scene flag (`GameState+0x744`), `0x42CED0()`, `DAT_00A48A38` or `DAT_00A46048` is set. `H` = back-buffer height, `W` = width. Layer 9 unless stated. HudMain = the object of §4 (`this`).
**Target panel (top-left)** — when `HudMain.target` is a live unit (`+0x37 == 1`):
* sprite pass: for type category `T+0x118 in {0,1}` (people): the name sprite `0x703880(target)` (a BmpRes chosen by unit type name — ~90 names: `ENGLISH_*`, `FRENCH_*`, `BUR_*`, `MER_*`, captains `*C`, named lords `TALBOT, SCALES, GLASDALE, ...`, siege `Batter, CANNON, TREBUCHET, bombard, canon, fauconneau, SIEGE_TOWER, CART, GOLD_CART...`) at `(0, 0x15)` when non-null; `"SWORD"` at `(0, 0x68)`; `"SWORDBG"` at `(0x2E, 0x72)`; `"EHPBAR"` at `(0x2F, 0x72)` cropped to `w = bar_w * HudMain.target_hp_shown / bar_max` pixels (min 1 px when the value is non-zero); `"EHPTIP"` (the bar end cap) at `(0x2F + w, 0x72)`. For other categories (siege, animals): `"EHPBAR"` at `(0x43, 0x72)`, `"EHPTIP"`, plate `"SBASE"` at `(0, 0x50)`, name sprite at `(8, 0x3C)`.
* structure target (`+0x37 == 0`, entity `0x425D20(target)`, record `0x425CE0(entity+10)`; shown only if `record+0x1C > 0`): `"EHPBAR"` `(0x43, 0x72)` + `"EHPTIP"` + `"SBASE"` `(0, 0x50)` and an icon at `(8, 0x3C)` by `record+0x1C`: 1 `TOWER`, 2 / 3 / 5 sprites whose name strings are `DAT_009D4464 / 009D445C / 009DE8A8` (unread), 4 `WOODGATE`, 6 `TRUNK` (kind 3 uses the HP from `entity->vtbl[0x4C]()`).
* text pass (bit2, units with an overhead bar `unit+0x31D`): at `(8, 0x82)` layer 9, white, font `0x5E9640(-1)`: if the unit's team leader (`unit+0x33` team, member 0 -> unit `+0x104 == 4`) is *not* a rank-4 unit: `"<type name T+0xB4> <sep> <level 0x4E1360(unit+0x124,1)> <sep>"`, otherwise the leader's overhead string `unit+0x31D + 6`.
**Player panel (bottom-left)** — `0x428B70(0x4203C0())` valid (main character `U`):
* `"PANEL"` at `(0, H-0x6C)`; name text (type name `T+0x100` + a suffix chosen by `HudMain.frame_state`: 0 / 3 / other all append `DAT_009D8DE4` — i.e. one fixed suffix) at `(0xF, H-0x6C)`; **`PHPBAR`** at `(0x8D, H-0x3A)` cropped to `bar_w * hp_shown / maxHP` (maxHP = `0x523320()`), **`PEPBAR`** at `(0x8D, H-0x2A)` cropped to `bar_w * ep_shown / maxEP` (`0x523700()`), **`PLVLBAR`** at `(0x8D, H-0x14)` cropped to `bar_w * (exp_shown - T(l-1)) / (T(l) - T(l-1))` where `T(l) = 0x4A4E60(l)->+1` (cumulative exp per level; `T(0..0)`=0; not drawn at level 30 `0x1E`).
* then `0x6FE660` (`EXPTEXT`, `EXPTEXT2` floating "+exp" text), `0x6FFED0` (`LEVEL1..3`, `LEVELUP0..3` level / level-up indicators), `0x702740` (`COMBO0`, `COMBO1`), `0x45FAE0` (attack-state glows: `CFGLOW01..03`, `CKGLOW01..03`, `CRITICAL`, `CRITI_K`, `GLOW01..03`, `HEAVY`, `HEAVYOFF`, `LIGHT`, `LIGHTOFF`).
* text pass: `"<hp>/<maxhp>"` right-aligned at `x = 0xF2`, `y = H-0x3D`; `"<ep>/<maxep>"` at `y = H-0x2D`; exp-to-next / percentage text (3 digits) at `y = H-0x17` (when level != 30); two centred boxes `(0x68, H-0x18)-(0x7B, H-0xD)` (class name from `0x497580`) and `(0x69, H-0x38)-(0x7B, H-0x2D)` (level number).
**Party row (bottom-right)**: `n = HudMain.+0x145` (i16, <= 8) live slots, slot recnos at `HudMain+0x125 + 4*i`. For slot *i* (`k = (8 - n) + i`, so the row is right-aligned), x origin = `DAT_009DE64C[k] - 800 + W` (a table of 8 x-positions, `0x9DE64C`; sibling tables `0x9DE68C`, `0x9DE6AC`, `0x9DE6CC`, `0x9DE6E4` give the portrait, HP-bar, moral-tick and count-text x; dump them with a hex view — not decodable from the code):
`"HR_FRAME"` at `(x, H-0x5A)`; the slot's button sprites are set to `HOLD_OT/DW/OV` (hold order) when the unit is idle (`unit+0x2A5 == 0 && unit+0x2AA(0x155) != 0`) else `RESUME_OT/DW/OV`, then drawn via `0x622CC0`; **danger icon**: if `hp/maxhp >= _DAT_00A3CD24` (a runtime fraction) the slot is normal, else for 1000 ms (from `0x414FE0(1)` event) it flashes `"DICON"` at `(x-0x30C+W, H-0x8C)` and the animated sword `"DSWORD1..9"` (frame `% 9 + 1`, step 100 ms of `DAT_00A500BA`) at the same place, and `"CPTOP"` + the numbered overlay at `(x-800+W, H-0x64)` (a digit sprite `DAT_009DE83C + n`); otherwise (healthy) just `"CPTOP"` at `(x-800+W, H-0x64)`; portrait sprite by type (`JOANC1-3`, `JEANC1-3`, `LAHIREC1-3`, `ALENCOC1-3` by armour tier LV1/LV2/LV3, `RICHEMON`, `CHARLES`, `FLAVY`, `BASTARD`) at `(DAT_009DE68C[k]-800+W, H-0x78)`; tint `0xFFFFFFFF` if the unit is on its own side, not hidden (`unit+0x2F == 0`), not a prisoner and `0x717A90(pos)` says it is on the minimap, else `0x80FFFFFF`; **selection highlight** `CSELECT1..3` (character interface open on that unit or RTS-selected unit) / `MSELECT1..3` (`0x430340() && 0x6E0DF0() == unit`) animated by `phase = ((DAT_00A500B2 - HudMain+0x147) / 100) % 8` using phases 1..3 only; the **morale/HP strip** for team leaders (`unit+0x33 != 0`, team count `> 0`): `"CMBASE"` at `(x-0x31E+W, H-0x19)`, `"CMORAL"` ticks (`n_ticks = morale * 6 / 100`, `morale = 0x6FFC40(recno)`) at `(DAT_009DE6CC[j] + x - 800 + W, H-0xD)`, and the HP bar sprite `DAT_009DE808` cropped to `bar_w*hp/maxhp` at `(DAT_009DE6AC[k]-800+W + (bar_w - w), H-0x1D)`; the team size text `team.+2` centred at `(DAT_009DE6E4[k]-800+W, H-0x23)-(+0x10-0x310.., H-0xC)` in the text pass.
Then `0x704260` (`ARROW1..3`: arrow-ammo indicators), `0x704810` (`DARKBAR`, `LIGHTBAR` — the thin bars of the objective/limit counters; `TODO(re)`) and `0x70E500(mask)` (minimap, Minimap.md §5).
Companion draws: `0x6F9970(mask)` — at `(0x12, H-0x4D)` layer 9 the bitmap `DAT_009CF2C4` from texture `HudMain+0x10` when `mask & 1` and nothing blocks (the small "MAINGAME" corner decoration); `0x6F9A60(mask)` — scenario 8 only: text `0x4DCB60(DAT_00A591D8)` (group `MAINGAME`) at `(0x337, 0xDC)`; `0x6F97D0` loads the textures `MAINGAME`, `MGEXP`, `DAT_009D3190` into `HudMain+0 / +4 / +0xC` (+0x10 = MAINGAME for the draw calls); `0x62A9A0(win, mask)` = the generic **9-slice window frame** (`TopLe/TopCe/TopRi/MidLe/MidRi/Mid centre (DAT_009DA7A8)/BotLe/BotCe/BotRi`, corner size 0x10 x 0x18 / 0x33, stretched centre/edges) used by all pop-up windows; `0x6BBAC0` / `0x6E39A0` call the generic widget-list draw `0x629BD0` for the new-item popup and the quick-save banner; `0x6E8C10` shows the cursor (`0x592C30`) when the letter screen's `+0x107E` flag is set.

## 20. Character interface (screen id 2): draw `0x6F6280`, input `0x6F7780`

Layer 1, 1024x768 layout, `U` = the displayed unit (`+0x6AF`; main character or RTS-selected). Mouse hit table: six rectangles at `0xA277D8` (stride 0x10: `{x0, y0, text_x, text_y, ...}`; size from the `"highligh"` sprite) = HP, EP and the 4 attributes; the hovered one (`HudMain-like +0x1D`, -1 none) draws `"highligh"` at its rect+8 and tooltips.
* Fixed art: background sprite `DAT_009DE028` at `(8, 8)` (texture `MGEXP`); `"EXPBAR"` at `(0x4A, 0xA8)` cropped to `bar_w * (exp - T(l-1)) / (T(l) - T(l-1))` (not at level 30); coins `coins1..4` (gold `< 2000 / < 4000 / < 6000 / more`) at `(0x18, 0xCC)`.
* Text (white unless the row is hovered): unit name block `(0x24,0x1F)-(0x108,0x66)` (auto-fit, font 5, 0x627D80 measures; wrapped by 0x626E00 if >= 3 lines); level `0x4E1360(U+0x124, 1)` centred in `(0x1A,0x7B)-(0x44,0xAD)`; exp `0x4D69A0(U+0x126, 4)` right-aligned at `(0x108, 0x7A)`; exp-to-next `T(l)` at `(0x108, 0x8F)` (string `0x4DD520` "max" at level 30); gold `DAT_00A596C4` at `(0x108, 0xD2)`; `"%d/%d"` HP (`U+0x135`, `0x523320()`) and EP (`U+0x145`, `0x523700()`) at the stat rect text positions (the hovered row is redrawn highlighted); attributes: `0x5237E0 / 0x5238C0 / 0x523860 / 0x523920` (`(4)` digits); four equipment/stat numbers `0x4E1360(U+0x1B0 / +0x1B2 / +0x1B5 / +0x1B7)` centred in `(0xD8,0x1A2)-(0x11B,0x1B7)`, `(0xD8,0x1BE)-(0x11B,0x1D5)`, `(0xD8,0x1DA)-(0x11B,0x1F1)`, `(0xD8,0x1F5)-(0x11B,0x20C)`; **unspent points** `U+0x12C` at `(0x108, 0x21C)` (attribute points) and `U+0x12A` at `(0x108, 0x230)` (combo points); when the pointer is over an up-arrow the arrow widget is drawn via `0x623580/0x622CC0`.
* Tooltips: after the pointer rests `> 0xFA = 250 ms` (`DAT_00A500B2 - this+0x10`), a `UNITATT` string pair (title, text; getters 0x4DD160/0x4DD190 HP, 0x4DD1C0/0x4DD1F0 EP, 0x4DD220/0x4DD250, 0x4DD280/0x4DD2B0, 0x4DD2E0/0x4DD310, 0x4DD340/0x4DD370 for the attributes, 0x4DD3A0/0x4DD3D0 level `(0x49,0x7E)-(0xA8,0x89)`, 0x4DD400/0x4DD430 exp `(0x49,0x96)-(0xA1..)`, 0x4DD460/0x4DD490 attribute points `(0x18,0x222)-(0xDD,0x22C)`, 0x4DD4C0/0x4DD4F0 combo points `(0x18,0x236)-(0xDD,0x240)`) is drawn by `0x6B8DB0(mouse_x, mouse_y, title, text, layer 1)` as a 9-slice window (`des_p_ul/u/ur/l/c/r/bl/b/br`).
* Two pages, selected by `+0x21`: `+0x21 == 0` -> `0x6EF9E0` (skills/combos page: sprites `PANEL`, `SRBAR`), `+0x21 == 1` -> `0x6D6740` (inventory page: `INVENTBG`, `PANEL`; item icons = BmpRes named by `item_record+0x10`; the upgraded sword `"Saint-Catherine-de-Fierbois"` (record name `+0x14`) uses the icon `"JOAN_SWORD_LV10"` + `"_"` + a digit = `strlen(record+0x10) - strlen("JOAN_SWORD_LV10")`). The page button at `(0xF6, 0x13)` shows the **other** page: `CB_OT/DW/OV` while on the inventory page, `WP_OT/DW/OV` while on the combo page (screen opens on the inventory page: `0x6F7F20(1)`). With unspent combo points (`+0x370 != 0 && U+0x12A != 0`, and `DAT_00A4D4D6 == 0`) the `CB_` button toggles every 500 ms of game time between `CB_*` and the green `CB_OTG/DWG/OVG`.
* Input `0x6F7780` (only when `0x4D8BB0()` true): ESC key or a screen hotkey (0x6F5A70) -> close (0x6F7E90) then `pending == 0 ? screenmgr_reset : screenmgr_activate`; the six `+0x1D` hot rects: left click on the hovered one -> `0x50D640(k)` (spend a point: k = 6 HP, 7 EP, 1/3/2/5 the four attributes, from `+0x1D` = 0..5); the page button swaps pages; click inside the party-portrait rect (`DAT_00A48868..74`) selects another commander: `0x6FFD90(slot)` -> recno, valid only if `unit+0x104 > 3` (sets `+0x6AF`, reloads buttons, clears hover).

## 21. Shop (`start_buy_interface`) — draw `0x6C4070` / `0x6C3690`, input `0x6C6550`, buy `0x6C5F10` / `0x6C5A00`, commit `0x6C3540`

Everything is in an **800x600 box centred on screen** (`ox = (W-800)/2`, `oy = (H-600)/2`), layer 6. Draw: frame sprite `DAT_009DDDC0` at `(ox, oy)`; `coinsN` at `(ox+0x10, oy+0x1D0)` (gold `<2000` 1, `<4000` 2, `<6000` 3, else 4 — same thresholds as the char interface); gold number right-aligned at `(ox+0xC9, oy+0x1D4)`; caption boxes `(ox+0x12C, oy+0x10)-(ox+0x1C4, oy+0x38)` (keeper / shop name `0x4DA650`) and `(ox+0x23C, oy+0x10)-(ox+0x2D4, oy+0x38)` (player name `U+0x3D`); two `"portrait"` sprites at `(ox+8, oy+8)` and `(ox+8, oy+0xE0)` with the two 3D actors rendered over them (§11) and name labels `(ox+0x3C, oy+0xB0)-(ox+0xA4, oy+0xD0)` (keeper name `unit+0x7F`) and `(ox+0x3C, oy+0x188)-(ox+0xA4, oy+0x1A8)` (player `U+0x3D`).
Grids (cell **40 px**, `0x28`): **player inventory** origin `(ox+0x100, oy+0x38)`, 6 columns x 13 rows (`0x1F0-0x100 = 240`, `0x240-0x38 = 520`); **shop stock** `(ox+0x210, oy+0x38)`, 6 x 8 (`0x300-0x210`, `0x178-0x38`). Each item (`{i16 item; i16 count; i16 flags/slot; ...}` records of the grid containers, cell size `item_rec+0x24 x +0x26`) is drawn at its cell origin with the icon named `item_rec+0x10` (sword exception as in §20) and, when `count` is 2..999 (shop: also shows the stock if >= 2), the count as small text at the cell's top-left (`0x6255D0`, layer 6); class-9 items (ammo/charges) show `0x4DCE00(charges - base + 1)`; the cell under the pointer is outlined (`0x6C2BA0(x, y, name)` re-draws the icon with the hover suffix `DAT_009DDD9C`). A dragged item (state `+0x231 == 2`) follows the pointer and, over a legal cell, is previewed with a translucent rectangle (`0x632A70`, colour `DAT_00A6A54C`).
Tooltip (pointer over an item, `0x6B8DB0`): title = item name from the item record (`+0x20` format string via `0x5A4C30`, filled with the stat values `[0x4A41C0(level)] + per-level table at record+0x251 (stride 12) and +0x249 (stride 16)`, scaled by `(item_level - record+0x36)`), then the price text: shop items `0x4DA6C0(price)` (+ stock `0x4DA730(n)` or `0x4DA700` for unlimited), player items sell price `0x4DA770(price)` or `0x4DA840` ("not sold here").
**State machine** (`this+0x231`): 0 idle, 1 mouse down on an item (picks it up, remembers `+0x23D` index/`+0x241` source: 0 = shop, 1 = inventory/`+0x245` item/`+0x249` count/`+0x24B` slot), 2 dragging (drop on release), 3 "placement preview". A modal one-button popup `+0x11` (1 = no room, 2 = not enough gold; `0x6D5230` text, `0x4DA810`/`0x4DA7E0` strings) blocks the screen until dismissed (`0x6D5120`).
**Buy** (drop shop item on the player grid, or right/left-click per `0x594360`): `price = ftol(x87)` (computed from the item record — `TODO(re)`: the factor is in a dropped x87 operand; shop price display uses the same value); if `gold < price` -> popup 2; stock `0x5B[idx]`: `-1` (>= 0x80000000) infinite, otherwise must be `> 0` and is decremented; place into the player grid at a free spot (`0x6C9D20` find slot for the item's w x h, `0x6C9F10` try stack-merge); no spot -> restore the stock, popup 1; success: `0x457BA0` insert, `gold -= price`, if `item_rec+0x1E8 != 0` and not yet announced (`0x472A90`) add it to the GameState list (`game::0x4729C0`, which also feeds the "new item" popup), and when the stock reaches 0 the shop cell is removed (`0x457EC0`). **Sell** (drop player item on the shop grid, or right-click own item): allowed when `item_rec+0x25D != 0` and either (`item_rec+2 in {0x15,0x17,0x19}` — weapon classes — and shop-object `+0x8D == 0`) or (any other class and `+0x8D == 1`) — i.e. weapon shops buy weapons, potion shops everything else; the sale price (another dropped x87 value) is added to the working gold and the item removed; if the shop kind does not match, the drag snaps back (state 2).
**Commit** `0x6C3540` (called from the close path): for each shop entry `i` the stored stock (`item.+6`) is rewritten from the working array (entries with count 0 are deleted from the facility's list `0x584A10`), `DAT_00A596C4 = this+0x57` (working gold), and the player's inventory is replaced by the working copy (`0x497A10(this+0x38)`). **Closing the shop therefore keeps every transaction — there is no per-transaction undo**; (the clone is only the working copy that is committed on close; whether a Cancel button exists is `TODO(re)`).

## 22. Barracks (input `0x6BF630`, commit `0x6BDC00`) and workshop (input `0x6EC480`, commit `0x6EA1A0`)

**Barracks** (`this`: `+0x30 active`, `+0x31 i16 hovered row`, `+0x33 i16 phase`, `+0x35 i32 barracks facility`, `+0x39 i32 commander unit recno`, `+0x3E i32 gold`, `+0x5A i32 hired[6]`, `+0x72 i32 pool` (recruits available in this barracks), `+0x76 i32 hired_total`, `+0x7A i32 team_added`, `+0x7E i32 cost_total`). Rows (6) are the French troop types in this order: `FRENCH_LIGHT_INFANTRY, FRENCH_HEAVY_INFANTRY, FRENCH_MACEMAN, FRENCH_POLEMAN, FRENCH_ARCHER, FRENCH_CROSSBOW_MAN`; row *k*'s icon/arrow widgets slide right by 0x14 px and highlight while hovered (hover = point in `(row.x, row.y)..(row.x1 + 0x4C, row.y1)`). Keys: ESC -> `0x6C0310(0)` close, Enter -> commit `0x6BDC00` then close. "+" (row *k*): fails silently if the commander exists and `0x448080() <= team_added` (team-size cap), or `pool < 1`, or `gold < cost_total + unit_type.+0x161`; else `cost_total += cost`, `pool -= 1`, `hired_total += 1`, `team_added += 1`, `hired[k] += 1`. "-" (row *k*): needs `hired[k] > 0`, reverses all five counters. **Commit**: for each type with `hired[k] > 0`: `0x468CD0(type_recno, cap, count)` where `cap` = `9999` when there is no commander, else `commander.+0x31` (the commander's team id) — the new troops are created into that team; then `gold -= cost_total; DAT_00A596C4 = gold`; the global recruit pool `0x469330(0x469360() - hired_total)`; `hired_total = cost_total = 0`.
**Workshop commit** (`0x6EA1A0`): for each producible type *k* (`W+0xA4+2k`) `queued[k]` times `0x46AB40(type, 9999, "", "")` (create one unit of that type at the workshop); `budget -= spent`; the budget lives in `DAT_00A596C4` (gold) for workshop kind 0 and in `W+0x2F0A` (the facility's own resource store) for kinds 1 and 2; `spent = 0`. (Input arithmetic: §12.)

## 23. Hotkeys that open screens — `0x6F5A70` (ScreenMgr::poll_hotkeys, `this` = ScreenMgr)

Preconditions: `DAT_00A45CD1 == 0`, a world is loaded (`0x41C7E0`), `DAT_00A45CCF == 0`. The key table is the one of the current control mode (`mode = DAT_00A5B9F8`; in RTS mode 4 the base mode `DAT_00A5B9FC`); `0x4CB820(mode, action)` = "action key pressed this frame". Tested **in this order, the last pressed wins**:

| action | screen id | extra condition |
|---|---|---|
| 0x22 | 1 in-game menu (ESC) | none of save/load (0x430370), options (0x4303A0), objective menu (0x4303D0) active and `DAT_00A56FA0 == 0` |
| 0x23, 0x24 | 2 character interface (two bindings: character / combo interface) | |
| 0x25 | 3 save | |
| 0x26 | 4 load | |
| 0x27 | 5 options | |
| 0x28 | 6 objective menu (`GAME_OBJECTIVE_KEY`) | |
| 0x29 | 7 message log | |

Result: if no screen is open (`cur == 0`) -> `pending = id`, `screenmgr_activate()`, return 1; if `cur == id` -> `pending = 0`, return 2 (the caller closes the screen — a second press of the same key toggles it closed); otherwise (a different screen is open) -> `pending = id`, return 1 (the caller closes the current screen and the manager chains to `pending`). Called every frame by the control input `0x42E1A0` (gameplay; a non-zero return ends that frame's input processing) and by the input of each open screen (`0x6F3C90` ESC menu, `0x6F7780`, `0x6E1250`, `0x720CA0`, `0x74F230`, `0x73D830`). The captions of the ESC menu show the keys for actions 0x25..0x29 (that is why `IGAMEMNU` getters pass `0x26, 0x25, 0x28, 0x29, 0x27`).
Other actions handled in `0x42E1A0` (control-mode input; screens above first): `0x35` quick-save (writes `SAVE\<name>` through gamefile and starts the banner `0x6E4360`), `0x36` quick-load (existing file -> `_DAT_00A5D861 = save; 0x6E4360()` banner mode 1), `0x2E` (cycles `DAT_00A4D767` 0..3 with a timestamp — a held-modifier state, `TODO(re)`), `0x19..0x20` with Shift held = **switch controlled character** slots 1..8 (looks the unit up by name `0x5270B0` -> `0x51FD50`, then `0x5121C0(unit)`; the 8 names are in string arguments the export dropped), `0x18..0x1C` (not RTS, not scripted) = team orders `0x432300`, `0x17` = **RTS-mode toggle** (`0x42E040(4)` / back to the saved mode; ignored in scenarios < 5 unless `ControlMgr+0x79`, or while `DAT_00A48CEF` is 1/2), then the per-mode update (`0x4343E0 / 0x432D30 / 0x438B30 / 0x430480 / 0x440B30` for modes 0..4).
**ESC menu rows** (`0x6F3C90`, in widget order): row 1 = resume (also ESC key or the 0x22 hotkey: closes via `0x6F38A0`), row 2 = **load** (pending 4), row 3 = **save** (3), row 4 = **objectives** (6), row 5 = **message log** (7), row 6 = **restart scenario** (confirm mode 3: popup text `0x4DAE20 + 0x4DADF0`; on Yes: `0x4B31F0(scenario)`, `battle::0x41CC80(scenario, stage, 0)`, state `0xC`; scenario 6 also re-reads its save header), row 7 = **options** (5), row 8 = **quit to main menu** (confirm mode 2 -> `0x6E4C00(0)` quit flow), row 9 = **quit to Windows** (confirm mode 1 -> `0x6E4C00(1)`, only if the quit flow is idle `0x6E4D30() == 0`). The popup is a `GPOPUP` window (`0x6D5230` text, `0x6D5340(3)` style, buttons Yes = result 1 / No = 2; `0x6D5120/0x6D5370` poll). Confirming sets `DAT_00A500D2 = 0; set_speed(0); 0x4B31C0 (msglog cursor reset); 0x6E5E00 (clear ticker)`.

## 24. The INTERACT action (control action `0x11`, third-person mode: `0x4343E0`)

Evaluated every frame while the key is pressed and the player is not in a scripted/cut-scene state (`ControlMgr+0x2B == 0`, `+0x31 == 0`, `+0xCD == 0`). Candidate handlers are tried **in this order; the first that returns 1 consumes the key**:
1. `0x43BEF0` **free prisoner** — candidate `0x471C80(100)` (§5): `0x5199B0(prisoner)` (clear the prisoner state), `0x528300`, set `unit+0x3C1 = 2000` (vision radius), `GameState+0x238 (DAT_00A591C8) += 1` (freed-prisoner counter).
2. `0x43BD40` **talk** — `0x46C1F0(100)`: post a trigger-system event `0x4EEF00(type 0x200, sub 1, target, 1, self, 1, self.x, self.y, self.z, range 100.0, 0, now, now, 1, 0, 0)` (script `OnTalk`-style trigger) and clear `ControlMgr+0x9E`.
3. `0x43BE40` **hire mercenary** — `0x46FA80(200)` then `0x4E8500(self)`: only if `candidate+0x2EB == 1`; cost is a dropped x87 value, **two formulas** (one when `candidate+0x104 < 3`, one otherwise; the type names `MER_EX_ARCHER` / `MER_EX_ARCHER_CAPTAIN` use a third); if `gold < cost`: once per game (`DAT_00A591B5`) an attention call with `STRTAB` message `0x4DD580(0,0)`; else `0x5094D0(candidate)` must be true, `gold -= cost`, the unit leaves its current owner link (`+0x384/+0x388`, order object freed), joins the player's team (`0x42FCB0`, `0x438F80`; for team members each is processed), `candidate+0x2EB = 2`, `GameState counters DAT_00A591A9 += 1` (hired) and `DAT_00A591AD += cost` (spent), voice line `RTS_OTHER_NARRATOR_001` (0x492C40).
4. `0x43C210` **recruit** — `0x48DFB0(100)`: the target joins the player's team (`0x42FCB0`, `0x438F80(self)`, `0x528300`), its order object freed, and its overhead-bar side effect object released.
5. `0x43BC50` **use item / entity** — `0x4D6320(200)`: event `0x4EEF00(0x200, sub 2, item, 1, self, 1, pos, range 200.0 (0x43480000), ...)`.
6. fallback `0x43BFE0` **barracks** — facility of kind 3 (`0x514B80`): `DAT_00A48959 = self; DAT_00A48955 = facility; 0x6C0310(1)` (open).
7. then `0x43C080` **workshop** — facility of kind 0x10 (`0x5156D0`): allowed if the workshop's kind (`W+0x79`) is 0, or if kind != 0 and the unit named `ALENCON` is the player-controlled one and within `_DAT_009CFC68` (value unread) of the facility's reference point (`W+0x29`); then `DAT_00A4856D = self; DAT_00A48569 = facility; 0x6ED3E0(1)` (open; `0x6ED3E0(0)` = close).
The mount candidate (`+0x21`) is not handled here (a separate action). In RTS mode 4 the corresponding handlers are in `0x442EF0`.

## 25. Message-log screen (id 7: textures `MSGLOG`, `MSGLOG_B`, `COMMELE2`, `MESSAGE`; opener `0x6E19B0`)

Page buttons: `aw_l_*` at `(0x78, 0x28C)` and `aw_r_*` at `(0x364, 0x28C)`. The log is shown as a **two-page spread, 4 messages per page**: category *c* (0..8, = scenarios) has `ceil(cat_count[c] / 4)` pages (`0x6E2910(c)`: `pages = floor(count / 4) (+1 if count % 4)`); pages of consecutive categories are numbered consecutively; `total = sum` is kept in `+0x4B`. `0x6E29A0(page)` returns the record index of the first message on a page: find the category whose cumulative page range contains `page`, take `cat_head[c]` (`MsgLog` at `0xA53C28`: `cat_head` `+0x3A`, `cat_count` `+0x5E`), then follow `link_b (+0xB)` `(page - first_page_of_category) * 4` times. The screen opens at the **last page**: `cur = total` (if `cur == 0 && total != 0` or `total < cur`) and, if `cur` is even, `cur -= 1` so the left page is always odd; `+0x49 = 0x6E29A0(cur)`. Message text = record `+0x17`, portrait key = record `+0x1B`.

## 26. Remaining notes
* The ticker draw (`0x6E5140`) and the objective banner draw were not in the round-2 export (`TODO(re)`); their widget positions are in §6.
* `0x6B8AF0` (after a **load**, called from the save/load screen when mode 1 closes): sets the HUD flag `+0x9EC`, runs `0x6B6FE0` (load textures `COMMELE`/`GAMEOVER`), `0x4E4520(7, 0x7F, 1, 0, 0, 0)`, then for the main character and for every other unit: clears its current order object (`unit+0x2A1`, freeing it and calling `0x4BB660` if it has a task) and calls `0x508B90(0, 1)` (reset movement); finally re-selects the control mode: `DAT_00A5B9F8 == 1` -> `0x42B6E0(0)`, `0` or `3` -> `0x42B6E0(1)`.
* `0x6C... 0x6E4D30` returns the quit-flow-active byte; `0x6E4360` starts the quick-save/quick-load banner.
