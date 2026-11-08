# hud family — in-game interface screens, minimap, message ticker, scene/view update (joan.exe)

Reference build `joan.exe`. Export: `$JOAN_WORK_DIR/decomp/game/hud/` (847 functions, depth <= 2; INDEX.md). The script-builtin handlers and a few callees were read from
`$JOAN_WORK_DIR/decomp/script/script_vm*/` and `.../engine/particles3/` (same binary). Confidence: **V** read in >= 2 consistent places, **R** read once, **I** inferred.
Struct pages: [HudScreen](../structs/HudScreen.md) (every screen), [Minimap](../structs/Minimap.md). Design/port input: game/hud.md. Widget toolkit: engine/ui2d.md,
[functions/ui2d.md](ui2d.md). Loop order: engine/gameloop.md §4.

Reading hints: the decompiler lost the receiver of almost every `thunk_FUN_x()` call (MSVC `thiscall`/`fastcall` with ECX set by the caller); where the receiver is deduced it is said so.
`__ftol` = truncation. Float literals shown as hex in the pseudocode (`0x455AC000`) are decoded here (`3500.0f`).

## 0. Corrections to the brief / the earlier pages (read first)

* **`0x6F43E0` is not an "action bar". It is the update of the in-game (ESC) menu** (`IGAMEMNU` strings, 8 sliding rows, `BUTTON_SLIDE_MANY`). There is **no** action-bar update among the roots; the combat/combo buttons
  (`CB_*`, `WP_*`) belong to the *character interface* (screen id 2, opener 0x6F7D80 / 0x6F7F20) whose update stub `0x6F7D40` is empty. Likewise `0x6E67B0`, `0x4AF590` are empty stubs.
* **`0x5634E0` is not the controllable-unit switch.** It is the per-frame *view update*: player control dispatch (`0x42F700`), light/day update (`lightsys_update 0x4A7F70`), audio listener from the camera,
  view-distance/fog easing. Character switching (`SWITCH_CHARACTER`, ...) is performed by the control-mode functions reached from `0x42F700` (charmove family) and writes the recno at `ControlMgr+0` (`DAT_00A5B9E8`); the writer is not in any export.
* **Widget tween stepper found** (open question of [ui2d.md](ui2d.md) §5): see §1. Delays are **milliseconds** (frame delta `DAT_00A500B6`), `move_to` speed is **pixels/second** with an **acceleration** term, `fade_to` speed is **alpha/second**,
  and the `+0` field of every widget **is a vtable** (slot 0 = the class' per-frame update, called by `Widget2D::tick`), contradicting the "no virtual functions" note of Widget2D.md.
* `0x6CA660` is the **cut-scene wipe/fade overlay**, `0x6E3A90` the **quick-save/quick-load banner**, `0x6BFE00`/`0x6ECE80` the **barracks / workshop** open-close controllers, `0x721270` the **objective-menu** controller,
  `0x6B7F20`/`0x6E7CF0` two copies of a 50 ms **gauge-glow** animation, `0x6E13C0`/`0x6BB750` the **message-log screen** / **new-item popup** controllers (see rows).

## 1. Widget tick and tween setters (en2d; shared by every screen)

| Address | Proposed name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x61F3B0 | widget2d_tick | `this` = any widget. Calls `move_step` (0x61F450), `resize_step` (0x61F980), `fade_step` (0x620810), then **vtable slot 0** (the class' own per-frame `update`). Every screen update root simply calls it once per owned widget | all roots | V |
| 0x61E020 | widget2d_move_to(x:i16, y:i16, speed:f32, accel:f32, immediate:u8, delay_ms:u32) | `immediate` -> set rect to `(x,y)` keeping size, cancel tween. If already there -> cancel (0x61E2D0). Else if `speed >= 0 && (speed != 0 \|\| accel > 0)`: `delay (+0x15) = delay_ms`, target `(+0x21,+0x23)`, direction `cos/sin(atan(dy/dx))` (`+0x2D/+0x31`; sign handling per axis done in the step), accumulators `+0x25/+0x29 = 0`, `speed (+0x37)`, `accel (+0x3B)`, `active (+0x35) = 1`. Otherwise cancel | 0x61E020, 0x61E270, 0x61E2A0, 0x61E2D0 | V |
| 0x61F450 | widget2d_move_step | if delay > dt: `delay -= dt`; else consume the rest: if `accel != 0` apply it to `speed` (0x61F910/0x61F400; inferred `speed += accel * dt/1000`); if `speed > 0`: `acc_x += dt/1000 * speed * cosd`, `acc_y += ... * sind`; whole pixels (`min(ftol(acc), remaining)`) are added to `x0,x1` / `y0,y1`, `acc -= whole`; **if `speed <= 0` the widget snaps to the target**. Completes (0x61E2D0) when at target | 0x61F450 | V (units) / I (accel call) |
| 0x61F1B0 | widget2d_fade_to(target:i16 (0/1), speed:f32, accel:f32, immediate:u8, delay_ms:u32) | `immediate` -> `alpha = target`. Else same validity rule as move; `fade_target (+0x60) = target`, `progress (+0x64) = 0`, `delay (+0x1D)`, `speed (+0x68)`, `accel (+0x6C)`, `active (+0x5E) = 1` | 0x61F1B0, 0x61F2E0, 0x61F310, 0x61F340 | V |
| 0x620810 | widget2d_fade_step | `delay` as above; `progress += dt/1000 * speed`; `step = clamp(progress, 0, |target - alpha|)`; `alpha += step * sign`, `progress -= step`. So `speed` = **alpha units per second** (6.0 = 0.17 s, 1.5 = 0.67 s). Completes (0x61F340) when `alpha == target` | 0x620810 | V |
| 0x61F980 | widget2d_resize_step | same pattern for width/height with `speed (+0x4A)`, direction `+0x42/+0x46`, anchor `+0x41` 0..6 (0 left/top fixed, 1 centred, 2 right/bottom fixed, 3 left, 4 right, 5/6 centred on one axis) | 0x61F980 | V |
| 0x590B00 | clamp_f64(x, lo, hi) | `x < lo ? lo : (x > hi ? hi : x)` | 0x590B00 | V |
| 0x590AA0 | min_i32 (inferred) | used for whole-pixel steps | 0x61F450 | I |
| 0x61E2D0 / 0x61F340 / 0x61F120 | cancel_move / cancel_fade / cancel_resize | reset tween state (fade target back to -1.0) | | V |

Port rule (replaces the guess in widget.h): per tick with `dt_ms`: `delay_ms -= dt` first, leftover time drives the motion; move speed px/s along |cos|,|sin| (per-axis sign from the remaining delta; the `atan`-based direction in the binary loses the sign for opposite-sign deltas — the original calls only use axis-dominant slides, so a straight `normalize(delta)` is equivalent);
fade `alpha += clamp(speed*dt/1000, 0, |remaining|)*sign`; all screens use `immediate = 1` to park widgets.

## 2. Per-frame roots (see [gameloop.md](gameloop.md))

| Address | Proposed name | Purpose / behaviour | Evidence | Conf |
|---|---|---|---|---|
| 0x6FD8B0 | hud_status_update | step 2b. Eases the displayed HP/stamina/exp numbers toward the main character's values (max step 9/3/500 per call), picks the HP-bar frame state (0/3/5 by 1/3 and 2/3 of max HP), expires the target bar after 3000 ms, steers the target bar, then **calls the minimap update 0x70EC50**; freezes the objective banner timer during cut scenes. Fields: HudMain in HudScreen.md §4 | 0x6FD8B0 | V |
| 0x6CA660 | cutscene_fx_update | cut-scene fade/wipe overlay state machine (HudScreen.md §16) | 0x6CA660, 0x65B0E0, 0x65AF50 | R |
| 0x761630 | title_screen_update | `display_title_screen` timer: `active && start + duration < DAT_00A500BA` -> `active = 0` | 0x761630, 0x761680 | V |
| 0x6D3590 | interact_prompt_update | recompute interaction candidates around the main character (HudScreen.md §5) | 0x6D3590 + 9 helpers | V |
| 0x721270 | objmenu_update | ticks the objective-menu widgets (3 + 10 + 16 widgets and 8 sub-page updaters 0x724620, 0x7343B0, 0x738340, 0x728230, 0x730BA0, 0x72C9F0, 0x736480, 0x73B040); closing: when the page is the hub (`+0x80 == 6`), the frame left the screen (`widget.y == 0x3E`, alpha 0, side strip `x == 0x400`) -> frees (0x71F360), restores speed if `DAT_00A500D2 == 0` (+ `0x42F930`, `0x592BD0`), and activates the pending screen (`DAT_00A52934 == 2 ? 0x6F5CC0 : 0x6F5E90`). Skipped while `DAT_00A5D384` | 0x721270 | V |
| 0x6B7F20, 0x6E7CF0 | glowbar_step_a / _b | 50 ms game-time gauge glow pattern (HudScreen.md §17) | both | V |
| 0x6F7D40, 0x6E67B0, 0x4AF590 | stubs | empty (`return`) — the gameloop guards (`0x4D8BB0`, `0x6E6810`, `0x4AF5D0`) therefore have no effect | | V |
| 0x6C8C00 | shop_idle_anim_update | guarded by `0x6C9040` (shop open): restart `F_IDLE` on the two portrait actors when their clip ended | 0x6C8C00, 0x6C9130 | V |
| 0x6BFE00 | barracks_update | open/close choreography (HudScreen.md §12), 6 rows | 0x6BFE00 | V |
| 0x6ECE80 | workshop_update | same, 4 rows | 0x6ECE80 | V |
| 0x6F2340 | menuframe_update | MenuFrame state machine (HudScreen.md §3) | 0x6F2340 | V |
| 0x6E3A90 | quicksave_banner_update | quick-save / quick-load banner (HudScreen.md §7); mode 1 calls `battle::0x41EA60` | 0x6E3A90 | R |
| 0x751750 | savemenu_update | save/load screen (HudScreen.md §14) | 0x751750 | R |
| 0x6E5280 | ticker_update | message-ticker state machine (HudScreen.md §6) | 0x6E5280 | V |
| 0x6F43E0 | gamemenu_update | in-game (ESC) menu controller (HudScreen.md §3) | 0x6F43E0 | V |
| 0x73DB40 | optmenu_update | options menu controller (HudScreen.md §14) | 0x73DB40 | R |
| 0x6E13C0 | msglog_screen_update | screen id 7: if `DAT_00A55AD1 == 2` ticks 2 widgets and closes when the strip reached `x == 0x400` and alpha 0 (-> `0x6F26E0` menu-frame out / pending screen); else (no menu frame) `0x6F2340`, and when `DAT_00A55AD1 == 0` closes: frees (0x6E1810), clears `+0x45..+0x48`, `active = 0` | 0x6E13C0, 0x6E19B0 | R |
| 0x6BB750 | newitem_popup_update | "item discovered" popup: `+0 active`, `+1 state` (1 opening, 2 shown, 3 closing); ticks 16 widgets, sets the global tint `0x5D5E80(alpha<<24 \| 0xFFFFFF)` from a widget alpha; when all 14 gadgets are idle and alpha is 255 (state 1 -> 2) / 0 (state 3 -> 0, `0x6BA810`: free textures `+0xC,+0x10,+0x14` and the item actor `+0x24`, restore input and speed) | 0x6BB750, 0x6BA810, 0x6BB020 | R |
| 0x6E4A20 | quit_flow_update | always updated; HudScreen.md §15 | 0x6E4A20 | V |
| 0x6D1980 | ending_fx_update | ticks 3 widgets when `+0x234` (ending overlay) is active | 0x6D1980 | R |
| 0x6E4C00 | quit_prompt_open(mode) | gameloop §4 step 6 (paused): mode 0 `bg_qm` / 1 `bg_qw` | 0x6E4C00 | R |
| 0x5634E0 | worldview_update | see §10 | 0x5634E0 | R |
| 0x6AD660 | unit_anim_update | see §10 | 0x6AD660 | R |
| 0x42F700 | charmove_update | player control dispatch per mode (§10) | 0x42F700 | R |

## 3. Screen manager and generic helpers

| Address | Proposed name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x6F5CC0 | screenmgr_activate | `from_menu = (pending && cur == 1)`, `cur = pending`, `pending = 0`, open screen `cur` (table in HudScreen.md §2) | 0x6F5CC0 | V |
| 0x6F5E90 | screenmgr_reset | zero the three fields | | V |
| 0x6F5ED0 | screenmgr_rts_unit | returns the unit shown by the character interface in RTS mode | | I |
| 0x6F3730 | gamemenu_open | open the ESC menu (pause + frame + `IGAMEMNU` captions) | 0x6F3730 | R |
| 0x6F24D0 | menuframe_open | HudScreen.md §3 | | V |
| 0x6F26E0 | menuframe_close | HudScreen.md §3 | | V |
| 0x6F2AC0 | menuframe_set_title(text) | copy 0x1D chars, centre the two title decorations | | V |
| 0x6F3330 | gamemenu_load_textures | `COMMELE2`, `COMMELE3` + five key-formatted captions | | V |
| 0x6F3630 / 0x6F20F0 / 0x6CDFA0 | free_textures / free_frame_textures / cutscene_fx_abort | | | V |
| 0x42F930 | gameapp_set_speed(f32) | `GameApp+0x44 = v`; `v == 0` -> `0x45F6B0(0)`, `0x49C080(0)`, `0x63C6D0(0)` (stop sound/AI-sound processing); else restore with `DAT_00A57FF9` and arg 1 | 0x42F930 | V |
| 0x42FEF0 | controlmgr_restore_pointer | per `ControlMgr+0x10` mode: pointer capture state | | R |
| 0x592BD0 / 0x592C30 | pointer_mode_game / pointer_mode_ui | set `DAT_00A4860A` = 1 / 0 then 0x4918D0 (re-apply mouse rectangle with the clip rectangle at `+0x6A..+0x76`) | | R |

## 4. Status HUD, interact prompt, target bar

| Address | Proposed name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x6FE280 | hud_set_target(recno) | stamp `+0x38`; 0 clears (`+0x33 = 0`, `+0x2C = 0`); else if different/not-unit: store, `target_hp_shown = target.+0x135`, `+0x37 = 1` | 0x6FE280 | V |
| 0x6FE4A0 | hud_set_frame_state(v) | `prev = cur; cur = v` | | V |
| 0x707CE0 | hud_show_objective | builtin 306 handler body (see §11) | 0x707CE0 | R |
| 0x46C1F0, 0x46FA80, 0x471C80, 0x48DFB0, 0x44F0B0, 0x4D6320, 0x514B80, 0x5156D0 | interact_find_* | candidate searches of the prompt (HudScreen.md §5): talk(100), unit class 1(200), free prisoner(100), recruit(100), mount(70/150), item(200), facility kind 3, facility kind 0x10 | | V/R |
| 0x6D3450 / 0x6D3480 | prompt_show / prompt_hide | `+8 = 1/0` + texture load/free (0x6D34D0 / 0x6D3540) | | V |
| 0x505960 | angle_to_unit(recno) | signed angle (deg) between P's heading and the bearing to the unit; the searches accept `-90..+90` | | R |

## 5. Minimap

| Address | Proposed name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x70EC50 | minimap_update | per frame: window origin, blink counters, rebuild the 200x200 fog mask from allied units' vision radius + territory areas + script areas, upload as A8R8G8B8 (`0x80000000` fog / `0` clear) (Minimap.md §1) | 0x70EC50 | V |
| 0x714040 | minimap_update_origin | origin = followed unit position minus half a tile, clamped >= 0 | 0x714040 | V |
| 0x719900 / 0x719B40 / 0x719E60 | minimap_fill_rect / _circle / _quad | mask fills (clamped to 0..199) | | V |
| 0x718010 | minimap_set_way_point(id, on, x, y, z, icon) | only writes when `on` changed; on enable stamps time and sets frame 1 | 0x718010 | V |
| 0x7180F0 | minimap_set_way_point_follow(id, unit, icon) | | | R |
| 0x7196E0 / 0x719770 / 0x7197F0 / 0x7198D0 | minimap_set_vision_{rect,circle,quad} / minimap_disable_vision | 10 script vision areas (1-based ids) | | V |
| 0x71D150 / 0x71D1E0 | minimap_set_icon_enabled / minimap_get_icon_enabled (name) | linear `strcmp` search of the 0xAE-byte marker records; `+0x9F` | | V |
| 0x71B6A0 | minimap_build_layout(scenario) | tile table from `INTERFAC` entries named `...z<n>(b \| r<RR>c<CC>)` (Minimap.md §2) | 0x71B6A0 | V |
| 0x71BF10 | minimap_load_markers(scenario) | reads the per-block `.map` files (docs/formats/map.md) -> `MinimapMarker[]`, builds the marker quads | 0x71BF10 | V |
| 0x71B080 / 0x71B330 | minimap_free / minimap_free_sprite | | | R |
| 0x717240 | minimap_click_camera | documented in functions/camera.md | | |

## 6. Message ticker, attention call, message log

| Address | Proposed name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x6E5700 | ticker_enqueue(text, kind, flag) | HudScreen.md §6 (priority kinds 1, 2 go to the front; `flag == 1` replaces other flagged nodes) | 0x6E5700 | V |
| 0x6E5D00 | ticker_pop | pop and free the head node | | V |
| 0x6E5E00 | ticker_clear | pop `count` nodes, `state = 0` | | V |
| 0x6E5E60 | ticker_activate(hold_ms) | `active = 1`, load `QSAVE` texture (0x6E5F50), `hold = hold_ms < 1 ? 3000 : hold_ms` | | V |
| 0x4AD710 | attention_call_create(text, portrait_speaker, voice_id) | builtin 344/345 backend. `new 0x5B` record (0x4AA2E0 zero), init 0x4AA3C0, `voice_id -> rec+0x19`. **Duplicate check**: if a live record already has the same text (`strcmp(rec+0x21 ptr)`), the new one is deleted and 0 returned. Else `mgr+0x36++`; `n` = number of existing records (other than the new one) with `+8 == 0` (the "still sliding" flag has been cleared), `state (+4) in {0, 1}` and `y (+0xD) <= screen_h - 0x112` (minus 1 if the manager's `0x4AEDE0()` count equals `n`); `target_y (+0x15) = screen_h - 0x112 - n * 0x28` (each stacked pop-up is 40 px higher; 0x112 = 274); push into the list (0x5857A0); then **append to the message log**: `MsgLog.add(category = scenario number DAT_00A58FA7, flags 1, text, sort_key = rec+0x3F, extra = portrait key)` (0x4B2D30; the portrait key `M0009` is used if the key is `M0000`) | 0x4AD710, 0x4AD9A0 | V |
| 0x4AD9A0 | attention_call_create_unit(text, unit_recno, voice_id) | identical body, but the record is initialised by **0x4AB330** (read): same defaults as 0x4AA3C0 (`+9 = 24.0`, `+0xD = 415.0`, `+0x57 = 255.0`), then for a *valid unit recno* (`0x428B70`) it picks the 8-char portrait key from the unit's **type code name** (`T+4`, e.g. `ALENCON`, `BASTARD`, `FLAVY`, `BEDFORD`, `ENGLISH_POLEMAN_CAPTAIN`, `FRENCH_ARCHER_CAPTAIN`, `CITIZEN_01`, `POTION_SHOPKEEPER`, `WEAPON_SHOPKEEPER_2`, ... ~120 names; the same key vocabulary as the speaker-name table below: `EPOLECAP, ELINFCAP, EHINFCAP, EMACECAP, ELINF, BLINF, BLINFC, F_ARCCAP, FCROSSC, FHINFCAP, FLINFCAP, FPOLECAP, CITI1.., SHOPPE, ...`) with armour-tier variants `ALENCON1-3 / LAHIRE1-3 / METZ1-3` chosen by the unit's equipped armour (`unit+0x15F` item name `*_ARMOR_LV1..3`); unknown types fall back to `CITI1`. The chain is a plain `strcmp` ladder; port it as a data table extracted from the function (S1L1: `attention_call_message_2("[0001N]...", jean_recno)` -> `METZ1..3`); dispatcher argument type 1 (variable) selects it, type 3 (string speaker) selects 0x4AD710 | R |
| 0x4AA3C0 | attention_call_init(text, speaker) | defaults: `state +4 = 0`, `+9 f32 = 24.0` (`0x42180000`), `y +0xD = 415.0`, `+0x57 = 255.0`, alpha channels 0. **Portrait/flipbook key (`rec+0x2E`, 8 chars)** from the speaker name `speaker`: `NULL` -> `M0000` (flag `+8 = 1`); `"TMESS"` -> `TMESS` (+8 = 1); otherwise a name table (below) and `+8 = 1`. Then the text: if `+8` and speaker given and key != `TMESS` the text is built as `<DAT_009CF774><speaker><DAT_009D4718><text>` (name prefix + separator); else just the text. **Sort key (`+0x3F`)**: `max(now, DAT_00A55ABA + 1)` style monotonic counter: `now == last ? now+1 : (now < last ? last+1 : now)`, stored back to `DAT_00A55ABA`; `+0x43 = now (DAT_00A500BA)`; `+0x17 = strlen(text)`; `+0x21 = heap copy` | 0x4AA3C0 | V |
| 0x4AF410 | attention_call_none_pending | (read) true when no pop-up is queued; used by `set_victory_display_flag` | script_vm | I |
| 0x4B2D30 | msglog_add(category, flags, text, key, extra) | dedup: scan existing records for same category and same text; else `new 0x1F` node, `msglog::fn_004B2940` init, push, `mgr.count++`, `cat_count[cat]++`, `cat_head[cat] = new` if it was empty, `cursor = new`; returns the new index (0 if duplicate) | 0x4B2D30 | V |
| 0x4B2940 / 0x4B2C00 / 0x4B2F10 | msglog_record_init / msglog_clear_all / msglog_remove | documented in structs/MsgLog.md | | V |
| 0x4AF2D0 / 0x4AF300 | attncall_get / attncall_slot_empty | | | V |
| 0x4AE390 | attncall_draw_all | `if (mgr+0x3A == 0)` draw every record via 0x4D8C10 | | R |
| 0x4AF530 | attncall_gate | `if (!cutscene_flag) 0x42CED0()` | | R |
| 0x4AF5D0 | attncall_enabled | returns `*(u8*)this` | | V |
| 0x488400 / 0x4884C0 / 0x488590 | help_load / help_free / help_parse | `help.cpp`: allocate 0x8340 bytes, open `RESOURCE\<name>` text, parse lines: a line starting with a digit = 19 numbers (`i16` at record `+9..+0x2B`) then a 60-char caption and a text block; a line starting with a letter (first char fails the digit test) = a section name (8 chars). Records are 0x70 bytes: `+0 name[8+1]`, `+9..+0x2B` 19 i16, `+0x2D` caption[0x3C], `+0x6A` text ptr, `+0x6E` text length. `+0x148` record count, `+0x14A/+0x14C` first/last name record, `+0x14E/+0x150` first/last numeric record, `+0x152` text pool (0x8340 bytes), `+0x156/+0x15A` growable buffer | 0x488590 | R |

Speaker-name -> portrait key table of 0x4AA3C0 (exact `strcmp` on the speaker string, first match wins; equipment suffix chosen from the item name of the speaker unit's armour `unit(+0x15F)` via `0x425CE0(item)+0x10`):
`Duke of Alencon` -> unit `ALENCON` (lookup 0x5270B0 + 0x51FD50): armour `ALENCON_ARMOR_LV1/2/3` -> `ALENCON1/2/3`, unit missing -> `CITI1`; `Pierre Ossier`, `Renaud Encrier`, `Renier Chanevacier`, `Eudes Masuier` -> `CITI3A`; `Capellarius` -> `CITI1`; (name at `DAT_009D4B34`, a bastard-like name) -> `BASTARD`;
`Guillaume de Flavy` -> `FLAVY`; `French Archer Captain` -> `F_ARCCAP`; `French CrossbowMan Captain` -> `FCROSSC`; `French Heavy Infantry Captain` -> `FHINFCAP`; `French Poleman Captain` -> `FPOLECAP`; `Sir William Glasdale` -> `GLASDALE`; `d'Illiers`, `Dunois` -> `FHINFCAP`;
`La Hire` -> unit `LaHire`, armour `LAHIRE_ARMOR_LV1/2/3` -> `LAHIRE1/2/3` (else `CITI1`); `Jean de Metz` -> unit `JEAN_DE_METZ`, `JEAN_ARMOR_LV1/2/3` -> `METZ1/2/3`; `Arthur de Richemont` -> `RICHEMON`; `Regent Bedford` -> `Bedford`; `Lord Talbot` -> `TALBOT`; `William de la Pole` -> `WILLIAM`; `Alexander de la Pole` -> `ALEXDLP`; `John de la Pole` -> `JOHNDLP`;
`Marcel Parcheminier` -> `F_ARCCAP`; `Radulfus Bradcroft`, `Jacobus Sprode` -> `EHINFCAP`; `Nicholas Weston` -> `ELINFCAP`; `Triston Walsoken` -> `EMACECAP`; `Tower Guard` -> `F_ARCCAP`; `Female Citizen` -> `CITI4`; `Renier Balancier` -> `flinfcap`; `Charles VII` -> `CHARLES`; `Reiner Mille` -> `EARCCAP`;
`Luxembourg` -> `LUX`; `WANDOMME`, `HUDLEG` -> `WANDOMME`; `MATTHEWGOUGH` -> `GOUGH`; `Perrinet` -> `GRESSART`; **anything else** -> `CITI1`. (The "armour tier" test is an `strcmp` chain LV1 -> LV2 -> LV3; if none matches the key stays empty.) The keys name BmpRes/texture entries in the portrait atlas (`M0001..M0009` flipbook frames are used by the pop-up animation: [flocksys.md](flocksys.md) §3).

## 7. Screens

| Address | Proposed name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x6C8D00 | shop_open(flag) | `flag > 0`: open (load textures 0x6C9060, pause game, `+0x231 = 0`); `flag <= 0`: close (free lists, actors, camera objects; restore input/speed; 0x6C90E0; 0x595A20) | 0x6C8D00 | V |
| 0x6C3230 | shop_build_lists | per-frame until built: portraits (0x6C2DD0), clone inventory (0x497A10), three grids (0x4571E0 + 0x457290), shop entries via 0x4577A0 | 0x6C3230 | R |
| 0x6C2DD0 | shop_setup_portraits | calls 0x6C92E0(0), 0x6C92E0(1), starts `F_IDLE` on both, places cameras, lights | 0x6C2DD0 | R |
| 0x6C92E0 | shop_make_portrait(side) | builds a 0x10C-byte actor from the unit's type record (`T+0x84/+0x90` per appearance `unit+0x3C5`), white/black material pass, registers (0x5EA1F0) | 0x6C92E0 | R |
| 0x6C27C0 | shop_destroy | | | R |
| 0x6C9040 | shop_is_open | `return *(u8*)(this+0x10)` | | V |
| 0x6C9130 | shop_idle_remaining(side) | remaining ms of the current clip (looping modulo) | | R |
| 0x653890 / 0x655B70 / 0x655BA0 | start_buy_interface handler / set_shop / set_keeper | | 0x653890 | V |
| 0x6BC530 / 0x6BCF00 | barracks_ctor / barracks_dtor | HudScreen.md §12 | | V |
| 0x6E8F40 / 0x6E9A50 | workshop_ctor / workshop_dtor | | | V |
| 0x6EC480 | workshop_input | queue arithmetic (HudScreen.md §12) | 0x6EC480 | V |
| 0x6EA1A0 | workshop_commit | 288 bytes, not read in depth (uses 0x44CF80, 0x43C450, 0x46AB40) | | I |
| 0x6E0B70 / 0x6E0520 / 0x6E0900 / 0x6E0A90 | letter_load(item) / letter_free / letter_open / letter_is_open | HudScreen.md §10 | | V |
| 0x71D2D0 / 0x71E800 | objmenu_ctor / objmenu_dtor | HudScreen.md §13 | | V |
| 0x7226A0 | objmenu_open(variant) | | | V |
| 0x721A60 | objmenu_set_page(page) | | | V |
| 0x7246F0 | objmenub_open_brief | mission brief page: text file by mission number | 0x7246F0, 0x724BF0 | V |
| 0x73A320 | objmenuw_ctor | siege-weapon page (6 weapons, help texts `battering_ram.txt`, `bombard.txt`, `cannon.txt`, `crossbow.txt`, `fauconneau.txt`, `trebuchet.txt`, `siege_help.txt`, `F_IDLE` animation) | 0x73A320 | R |
| 0x7435E0 / 0x744CE0 / 0x7478A0 | optmenu_ctor / optmenu_free / optmenu_key_names | builds the key-binding pages; hides actions named `Escape` and `Not Available`; `Escape`/`Not Available` strings come from 0x4CB5B0 | | R |
| 0x74DA40 | savemenu_open(mode, from_menu) | HudScreen.md §14 | | V |
| 0x73C9D0 | optmenu_open | | | V |
| 0x6E19B0 | msglog_screen_open(open, from_menu) | open: 0x6E1570, sum category counts, page = `last page (even-adjusted)`, pause, menu frame in; close variant moves the strip out | 0x6E19B0 | R |
| 0x6BBC90 / 0x6BA4B0 | newitem_popup_set(item, count) / newitem_popup_open | opened by `give_item_to_unit` the first time a flagged item is given to the main character | 0x65C5F0 | R |
| 0x65C5F0 / 0x65C770 | give_item_to_unit(_troop) | add item (0x50FD90); if the receiver is the main character: ticker message (`item_rec+0x18` formatted with the count, 0x5A6660 + string getter 0x4DD6E0, kind 0, flag 0); if `item_rec+0x1E8 != 0` and the item is not yet in GameState list (0x472A90): append (0x4729C0), open the new-item popup | 0x65C5F0 | R |

## 8. Cut-scene overlay entry points (script handlers)
`0x65ADE0 set_cut_scene_flag(v)`: `v == 1` -> `0x6CB9E0(now, 128.0, 1,0,0,0,0,0,0)` (start overlay, speed 128 = fade rate per second), else `0x6CC6A0(now, -128.0, 0, 0)` (reverse). `0x65AEF0 start_cut_scene(...)` -> `0x6CB9E0(now, 128.0, a, b, c, d, e, f, g)`. `0x65AFF0 end_cut_scene(a,b)` -> `0x6CC6A0(now, -128.0, a, b)`. `rate = +7` of the overlay object is the 128.0 (alpha units/s: full black in 2 s).

## 9. Misc helpers read
Open-state accessors (each returns the first/active byte of the screen object; they are what the gameloop draw gate and the WAIT polls test): `0x430340` character interface open (`+0x6AE`), `0x430370` save/load active (`+0x4B05`), `0x4303A0` options active (`+0x122`), `0x4303D0` objective menu active (`+0`), `0x4303F0` in-game menu alive (`+0`), `0x6C9040` shop open, `0x6E0A90` letter open, `0x4D6A10`/`0x4AF5D0` attention-call enabled. Strings: `0x5A3090(table, "GROUP", idx, ...)` = `strtab::format(group, index, args)` (searches the in-memory `t_interf`-style table entries of 0x15 bytes: `+4 index`, `+8 group name`, `+0x11 format ptr`; `0x5A31E0` = vsprintf). `0x4CB5B0(keytable, idx)` = key display string. `0x4CEEF0(name)` = play UI sound by name. `0x5A6660(text, n, 0, 0)` = format helper. `0x42A260(r,g,b,a)` = pack colour (returns pointer to a temp u32 `0xAARRGGBB`).

## 10. View update, control dispatch, unit animation (scene-side roots in this family)

| Address | Proposed name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x5634E0 | worldview_update | `this` = view object (`+0` enabled, `+0xC` SceneCamera*, `+0x10` player-light object, `+0x14` max view distance `D`, `+0x18` current view distance `C`, `+0x1C` base distance `B`). Order: (1) `0x42F700()` charmove_update; (2) `lightsys_update` (0x4A7F70, receiver = `World+0x6E`); (3) camera helper 0x459C90; (4) if audio enabled (`DAT_00A57EF8`): listener from camera block `cam+0xA0` (position at `+0xA4`, angles `+0xB0/+0xB4`) via 0x5A0110 -> OpenAL listener orientation (`alListenerfv`); (5) player light at the main character: `light.pos = unit.(x,y,z + 150.0)` (`_DAT_009CFBE0`); (6) **view distance**: `step = clamp(DAT_00A500BE, 20, 50)/1000 * 500.0` world units; `target = min(B * k_night * k_weather, D - 200.0)` with `k = 0.7` (`_DAT_009CFC7C`) applied once if the game minute `DAT_00A47BB4 > 1080.0 (18:00) \|\| <= 360.0 (06:00)` (night; `_DAT_009CF030`, `_DAT_009CF9C8`) and again if weather layer 1 or 2 is active (`0x41BF10`); ease `C` toward `target` with a **dead band of 100.0** (`_DAT_009CF9C4`): move by `step` only if `\|C - target\| > 100`; (7) far clip: if `\|cam.far(+0x4C) - min(D, C+200)\| >= 1.0` call `camera.set_clip(near = 4.0, far, ortho_far = 16384.0)`; (8) **fog**: `fog_end = C`, `fog_start = C < 2000.0 (_DAT_009CFB6C) ? max(1, 900.0 (_DAT_009D84AC) - (2000.0 - C) * 0.5) : 900.0`, `fog_start` multiplied by `0.8` (`_DAT_009D727C`) for night and again for weather, `>= 1.0` -> `0x616A00(fog_start, fog_end)` (stores `+0x60/+0x64`, forwards to the scene fog); (9) `cam+0xCC = 1`, `cam+0xC8 = C / 4.0` (detail/LOD distance) | 0x5634E0 | R |
| 0x42F700 | charmove_update (receiver `DAT_00A5B9E8`) | skipped when `DAT_00A596AB` (builtin `set_stop_unit_process_flag`) or the cut-scene flag. If mode (+0x10) is 4 (RTS) and the controlled unit is dead -> 0x42E040. If a mode switch is pending (`+0x81`): per target mode `+0x14` check `0x430110/130/150/170` (always true for modes 0..3) / `0x447860` (mode 4: only when `0x447900() == 4`; runs the camera `0x4384E0` loop while `DAT_00A48CEF == 0 && DAT_00A5BA5C != 0`), then the per-mode enter handler and clear `+0x81`. Then the per-mode update by `+0x10` (or `+0x14` while switching): 0 -> 0x434F40, 1 -> 0x4338C0, 2 -> 0x439090, 3 -> 0x431520, 4 -> 0x442EF0 (RTS, 6.9 KB); `+0xAC != 0` -> 0x42AD50 (scripted/cut-scene control). Modes 0..3 = third-person/free/..., `get_charmove_mode` returns the mode | 0x42F700 | R |
| 0x6AD660 | unit_anim_update | `this` = animation system (`+0x586` last time, `+0x58A` cleared, `+0xB12` enable). Uses the **real** clock `DAT_00A500BA`: `dt = now - last`. If `+0x586 == 0` or `+0xB12 == 0`: just store `now`. Else: for every animation object `0x6AF310(i)` (count 0x6B0800, descending) clear `obj->+8->+0x18->+0x50`; then for every entity `i = count..1` (`0x41B3F0`) with `entity+0x2AE != 0` (has anim state) and `*entity != 0`: `0x6B18A0()`; if `entity+4 ∈ {2,3,4,5,0x12}` (non-humanoid entity classes) -> `0x6A8060(entity, this, now, dt, this+2)` (4.3 KB) else `0x6A45A0(...)` (12 KB, humanoid/unit animation state machine); re-check the entity, `0x6B1350()` (post-pass, 1 KB). Store `last = now`. The callees belong to the movement/animation family | 0x6AD660 | R |

## 11. Builtin handlers of this family

Ids and census are in [docs/scripting/BUILTINS.md](../../scripting/BUILTINS.md); addresses below were read from `script_vm`.

| id | builtin | handler | what it does to this family's state | Conf |
|---|---|---|---|---|
| 304 | start_buy_interface(shop, keeper) | 0x653890 | `if (0x44CF80(shop) == 0 && 0x428B70(keeper) == 0)`: `shop_open(1)` (0x6C8D00), `shop = arg1` (0x655B70), `keeper = arg2` (0x655BA0), `0x592C30` (free pointer); return 1; else return 0 (invalid shop or keeper: nothing happens, the WAIT poll `!shop_is_open` then passes at once). Closing is done by the player in the screen | V |
| 305 | close_inventory_interface | 0x653930 | `if (0x430340()) 0x6F7E90()` — closes the character interface (screen 2): `+0x6AE = 0`, 0x6F61E0, `0x42FEF0`, restore speed (`DAT_00A500D2`), `+0x6AF = 0` | R |
| 306 | show_objective | 0x653970 | `0x707CE0()` = HudMain objective banner: `state 0 -> 1 { active = 1; start = now; flag = 0 }`, `state 2 -> 1 { start = (x87 value); active = 1; flag = 0 }`; return 1. Not the objective *menu* | R |
| 307 | start_scenario_part_interface(v) | 0x6539A0 | `DAT_00A5027E = v; DAT_00A5027F = 2; set_next_state(0xC)` — switches the app to the scenario loading/transition screen (state 0xC) | V |
| 308 | minimap_set_way_point(id,x,y,z,on,icon) | 0x6539E0 | `0x718010(id, on, x, y, z, icon)` | V |
| 309 | minimap_set_way_point_follow_unit(id,unit,icon) | 0x653A50 | `0x7180F0(id, unit, icon)` | V |
| 329 | display_title_screen(text,a,b,c) | 0x65C420 | `0x761680(now, text, a, b, c)` (a = duration ms, b/c = fade ms) | V |
| 344 | attention_call_message(text [,speaker-flag]) | 0x65CEA0 | `(text == 0) -> 0`; `arg2 == 0` -> `0x4AD710(text, NULL, 0)`; else `0x4AD710(text, "TMESS", 0)` (the "TMESS" key = tutorial message style) | V |
| 345 | attention_call_message_2(text, speaker [,v2,v3]) | 0x65CF00 (arg 2 is a string speaker name) / 0x65CF40 (arg 2 is a unit recno variable, the S1L1 usage) | `text == 0 -> 0`; else `0x4AD710(text, speaker_name, voice)` / `0x4AD9A0(text, unit_recno, voice)` — creates the pop-up; returns the new record index (`0x417520`) or 0 for a duplicate text. `voice` = the derived voice id (BUILTINS.md §1.5) | R |
| 346 | set_attention_call_message_voice(rec, voice) | 0x65CF80 | `if (slot empty) 0 else rec+0x19 = voice` | V |
| 356 | display_string(text,x,y) | 0x65D9C0 | draw `text` once with `0x6255D0(x, y, 1, text, 0xFFFFFFFF, -1, 0, 1, 2, 0, 1.0, 1.0)` — a **single-frame immediate draw** (no persistence: the script must call it every frame it wants the text) | V |
| 357 | display_number(n,x,y) | 0x65DA50 | same with `text = 0x58FCA0(n, 4)` (number -> string) | V |
| 365 | set_kill_citizen_limit(n) | 0x65E7D0 | `if n > 0: DAT_00A50522 = n`; returns `n > 0` | V |
| 366 / 367 | start_kill_interface([mode]) / end_kill_interface | 0x65E810 / 0x65E850 | `if (DAT_00A50520 == 0) 0x6DFE10(mode)` / `0x6DFF10()` (HudScreen.md §9) | V |
| 368, 369, 370 | set_kill_citizen_limit_2 / start_kill_interface_2 / end_kill_interface_2 | 0x65E880 / 0x65E8C0 / 0x65E900 | `DAT_00A50526 = n` / `if (DAT_00A50521 == 0) 0x6E00B0(mode)` / `0x6E01D0()` | V |
| 378 / 379 | set/get_minimap_icon_display_enable(name,v) | 0x65EC90 / 0x65ECF0 | `if (0x44CF30() && 0x44CF10() != 0) return 0x71D150(name, v) / 0x71D1E0(name)` else 0 | V |
| 382 | set_minimap_rotation(v) | 0x65EDA0 | `DAT_00A4F3AC = v` (consumer: minimap draw, not exported) | V |
| 383 | set_minimap_display_gold_enable(v) | 0x65EDD0 | `Minimap.show_gold (+0x11) = v` (global minimap object, `this` lost) | R |
| 385 | show_scenario_brief | 0x65EE60 | `DAT_00A52934 = 6; 0x6F5CC0()` = open the **objective menu** (ScreenMgr id 6, `from_menu = 0`); poll `!0x4303D0()` = "objective menu closed" (objmenu object byte `+0`, set to 1 by 0x7226A0) | R |
| 392 | set_victory_display_flag | 0x65F6A0 | pause (§0 protocol: `DAT_00A500D2 = 0`, save speed, `set_speed(0)`); `if (0x4AF410() == 0)` (no attention call pending) `0x6E8C80()` (victory banner) and clear `DAT_00A59171/75/79` (three GameState result bytes `+0x1E1/1E5/1E9`) | R |
| 393..396 | set_minimap_{rectangular,circular,quadrilateral}_vision_area_enable, disable_minimap_vision_area | 0x65F740 / 0x65F780 / 0x65F7C0 / 0x65F820 | forward to 0x7196E0 / 0x719770 / 0x7197F0 / 0x7198D0 | V |
| 398 | show_letter_interface(item) | 0x65F910 | `0x6E0B70(item)` (load `ITEM_DES\<file>`), `0x6E0900()` (open, pause); poll `!0x6E0A90()` | V |
| 403 | is_objective_menu_opened_in_scenario | 0x65FBD0 | returns `DAT_00A591C7` | V |
| 404 | is_attention_call_message_shown(text) | 0x65FBF0 | scan live records (descending): if a record's text equals `text` -> **0**, none -> **1** (inverted with respect to the name: returns "not shown") | V |
| 406 | show_ending(v) | 0x65FCC0 | `if 0 <= v <= 1: 0x6D19E0(v)` (ending overlay) | R |
| 333 / 334 | give_item_to_unit(_troop) | 0x65C5F0 / 0x65C770 | see §7 (ticker message + first-pickup popup) | R |
| 391, 384 | set_charmove_detect_block_flag / set_critical_attack_enabled | 0x65F670 / 0x65EE30 | `DAT_00A5BA64 = v` / `DAT_00A4F858 = v` (read by the charmove / combat code) | V |

## 12. Constants (exact values; functions in brackets)

| Value | Meaning | Source |
|---|---|---|
| 9 / 3 / 500 / 40 | per-update catch-up step of displayed HP / stamina / exp / target HP | 0x6FD8B0 |
| 1/3, 2/3 of max HP | bar frame state thresholds (state 5 below 1/3, 3 below 2/3, integer division) | 0x6FD8B0 |
| 3000 ms | target bar lifetime; default ticker hold | 0x6FD8B0, 0x6E5E60 |
| 200 x 200 | minimap mask / texture size; fog pixel `0x80000000` | 0x70EC50 |
| 30 / 10 / 0xAE | way points / vision areas / marker record size | 0x718010, 0x7196E0, 0x71BF10 |
| 100 ms / 360 ms | way-point frame period / minimap anim frame period | 0x70EC50 |
| 0x5B | attention-call record size | 0x4AA2E0 |
| screen_h - 0x112 - 0x28 * n | attention-call rest line | 0x4AD710 |
| 0x14 | ticker node size | 0x6E5700 |
| 3500.0 / -1500.0 | standard slide speed / accel (px/s, px/s^2) | many |
| 6.0 / 10.0 / 5.0 / 4.0 / 1.5 | standard fade speeds (alpha/s) | many |
| 30 ms | row stagger of barracks/workshop (44 ms in the game menu) | 0x6BFE00 |
| 1500 ms / 749 ms | cut-scene wipe sweep / black hold | 0x6CA660 |
| 128.0 | cut-scene fade rate (alpha units/s of 0..255) | 0x65ADE0 |
| 500.0 world units/s, clamp(dt, 20, 50) ms | view-distance easing speed | 0x5634E0 |
| 100.0 | view-distance dead band; 200.0 = far margin; 4.0 near; 16384.0 ortho far; 900.0 fog start default | 0x5634E0 |
| 50 ms | gauge-glow step (game time) | 0x6B7F20 |
| 10 / 6 / 4 rows | save-slots per page / barracks rows / workshop rows | 0x751750, 0x6BC530, 0x6E8F40 |

## 13. Cross-module requests
* Rename: `0x61F3B0 widget2d_tick`, `0x61F450 widget2d_move_step`, `0x620810 widget2d_fade_step`, `0x61F980 widget2d_resize_step`, `0x61E020 widget2d_move_to`, `0x61F1B0 widget2d_fade_to`, `0x590B00 clamp_f64`,
  `0x6F5CC0 screenmgr_activate`, `0x6F5E90 screenmgr_reset`, `0x6F43E0 gamemenu_update`, `0x6CA660 cutscene_fx_update`, `0x6E3A90 quicksave_banner_update`, `0x70EC50 minimap_update`, `0x71BF10 minimap_load_markers`, `0x71B6A0 minimap_build_layout`,
  `0x5634E0 worldview_update`, `0x42F700 charmove_update`, `0x6AD660 unit_anim_update`, `0x4AD710 attention_call_create`, `0x4AA3C0 attention_call_init`, `0x4B2D30 msglog_add`, `0x42F930 gameapp_set_speed`.
* Types/globals: `DAT_00A5B9E8 g_controlmgr (ControlMgr, +0 = main character recno)`, `DAT_00A52930 g_screenmgr`, `DAT_00A55AD0 g_menuframe`, `DAT_00A500D2 g_game_running`, `DAT_00A500D0 g_saved_speed`, `DAT_00A591C7 g_objmenu_open`, `DAT_00A596C4 -> GameState.gold (+0x734)`,
  `DAT_00A4860A g_pointer_game_mode`, `DAT_00A55ABA g_attncall_last_key`.
* **[ui2d.md](ui2d.md)**: tween units (this page §1) and the widget vtable correction.
* **docs/formats/map.md**: the reader is `0x71BF10` (Minimap.md §3): field meanings `kind` = BmpRes sprite name in `INTERFAC`, `x/y` = offsets added to the world block origin (`block+0x44/+0x48`), the two trailing bytes are read as two single bytes.
* **docs/re/functions/flocksys.md / structs/Flock.md (AttentionCall)**: creator documented here (§6): `+0x2E` is an 8-char key (not a 401-byte string), `+0x17` is `strlen(text)`, `+0x19` voice id, `+0x21` heap text pointer, `+0x3F` monotonic sort key (`DAT_00A55ABA`), `+0x43` creation stamp.

## 14. Open questions
* **Draw side — round 2 done** for the main HUD, minimap, character interface, shop (HudScreen.md §19-21, Minimap.md §5). Still not read: the ticker draw `0x6E5140`, the objective banner draw, damage numbers, `0x6EF9E0` (skills page), `0x6D6740` (inventory page internals), `0x704810` (`DARKBAR/LIGHTBAR`), `0x70D670` (main-menu/loading overlay), objmenu page draws.
* Meaning of `unit+0x104 > 3`, `+0x126`, `+0x145`, `+0x2E9`, `+0x2EB`; `GameState+0x1E2`; `DAT_00A5BA5C`, `DAT_00A48CEF`.
* `ScreenMgr.pending` hotkeys: **resolved** (HudScreen.md §23, `0x6F5A70`). Open: `from_menu == 2` meaning; the 8 names behind the Shift+slot character switch (`0x42E1A0`).
* The three unlabelled rows of the in-game menu; contents of `IGAMEMNU` indices 4, 6..0xC.
* Resolved in round 2: message-log paging (`0x6E1570/0x6E2910/0x6E29A0`, HudScreen.md §25), workshop commit `0x6EA1A0` and barracks commit `0x6BDC00` (§22), `0x6B8AF0` (§26), attention call from a unit `0x4AB330` (below). Still open: letter paging (`0x6E0DC0/0x6E0DF0/0x6E0AB0`), the x87 price/cost formulas of the shop, mercenary hire and barracks units (operand dropped by the decompiler: need a disassembly of `0x6C5F10`, `0x4E8500`).
* **Resolved by the `.rdata` dump **: `_DAT_009CF030 = 1080.0`, `_DAT_009CF9C8 = 360.0` (night = minute > 1080 or <= 360), `_DAT_009CFC7C = 0.7`, `_DAT_009D727C = 0.8`, `_DAT_009CFB6C = 2000.0`, `_DAT_009D84AC = 900.0`, `_DAT_009DE2A8 = -0.2` (double; ticker state 4 -> 5 when `A.x <= text_w * -0.2`), `_DAT_009DF420 = 1/81.92` (objmenu hero-portrait world -> model scale), `_DAT_009D03C0 = 100.0` (double; interact class-1 height window), `_DAT_009CFBE0 = 150.0`.
* Still unread: `_DAT_009DE250/260/270/280` (the quick-save banner thresholds), `_DAT_009D74CC` (shop camera offset for LaHire), `_DAT_009DF7D0`, `_DAT_009CFC68` (workshop proximity radius), `_DAT_00A3CD24` (runtime HP fraction for the party danger icon), the party-row x tables `0x9DE64C / 68C / 6AC / 6CC / 6E4`, the colour words `0xA6A59C..0xA6A5B8`, `_DAT_009DEDD8 / DDC / DEDF4 / DEDF8` (minimap edges: 199 / 209 inferred).
