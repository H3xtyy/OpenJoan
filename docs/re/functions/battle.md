# battle.cpp, scenario.cpp, gameplay.cpp, scenario flow, trigger system, calendar, cut-scene builtins — joan.exe (reference build)

Pseudocode: `$JOAN_WORK_DIR/decomp/game/battle/` (490 functions, depth 0-2) plus the handler exports `decomp/script/script_vm/` (0x659xxx-0x65Bxxx, 0x4720xx, 0x4CD7xx) and `decomp/engine/{gameloop,camera3}/`
(for the end-condition, cut-scene and date helpers). Structs: [Scenario](../structs/Scenario.md) (+ ScenarioStage),
[EndCondition](../structs/EndCondition.md), [Trigger](../structs/Trigger.md) (TriggerEvent), [Calendar](../structs/Calendar.md); engine context: [GameState](../structs/GameState.md),
docs/engine/gameloop.md, [SceScript](../structs/SceScript.md), BUILTINS ([docs/scripting/BUILTINS.md](../../scripting/BUILTINS.md)).

Confidence: **V** verified (>= 2 consistent readings, or strings), **R** read in one function, **I** inferred (naming/semantics).
Caveat for the whole export: Ghidra dropped the arguments of most `thunk_FUN_xxx(...)` calls (ECX and stack values are missing), so many call sites only show *which* function runs, not with what value.
Where an argument is needed for a rule it is cross-checked against another reader and marked `inferred`.
Source-file attribution: `d:\ww\battle.cpp` (0x41BF40, 0x41CC80, 0x41D400, 0x41DFE0, 0x41EA60), `d:\ww\scenario.cpp` (0x4CC850-0x4CDF00), `d:\ww\gameplay.cpp` (0x47C070-0x47CCA0), `d:\ww\end_con.cpp` (0x467260-0x467390).
All other rows are attributed by call-graph (no file string).

## 1. The scenario lifecycle entry points (battle.cpp)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x41CC80 | `battle_load_scenario(scenario, stage, keep_world)` | `__thiscall` on the battle object (ECX hidden = the load-progress counter). Steps: (1) if the *current* scenario number is 0, clear the message log (0x4B31C0); (2) clear the in-game SMS list (0x6E5E00); (3) `DAT_00A48C5E = 0`; (4) `world_unload(keep_world)` 0x41C780 (`progress = 0`); (5) `scenario_set_number(scenario)` 0x4CCF40; (6) `stage_index = target_stage = stage`, `end_state = 0`, `progress = 0`; (7) `game_load_world_arrays` 0x470D40 (empty arrays; `DAT_00A5BA64 = 0`); (8) compile `SCRIPT\SCENARIO\<stage.script_file>.sce` into `stage.script` (deleting a previous script object; reads the file through the ilib File wrapper into a tracked buffer, `sce_compile` 0x649280, then attaches `<name>EX.res` via 0x63F580) — silently skipped when the file does not exist (then `scenario_set_number(0)`); (9) compile `<stage.desc_file>.sce` (the `*_des` script), **run it once** (`run_step(0)`, vtable slot 3) and delete it; (10) `progress += 1`. The world itself is **not** created here (see 0x41C810). Callers: new game `(1,1,0)` (0x70ACB0), continue `(scenario+1, 1, 1)` (0x755D80), retry `(scenario, stage, 0)` (0x720CA0), debug console `(scenario+1,1,0)` (0x52F930 case 0x74), stage switch (0x471620) | strings `SCRIPT\SCENARIO\`, `d:\ww\battle.cpp`; callers | V |
| 0x41C810 | `battle_enter_world()` | "Start" of a loaded scenario (briefing screen button 0x75FA50, loading phase 5 of 0x75F180, stage switch in 0x471620): cut-scene reset (0x6CA170), stream pumps (0x59B1C0, 0x5866D0), clear a 3-int helper (0x6F5E90). If `scenario != 6`: create the world container (0x5611B0, index of a new 0x92-byte object in the world array), `World::create` 0x44E8C0(1, stage.world_file, stage.world_file, 0) (reads `MAP\<world>.<ext>` sizes and the territory file), loading progress base **5 %**, `0x561350(world)` makes it current, `0x5615C0` (scene/renderer set-up: camera, light, post effects), `0x42DE70` (camera-manager reset, loads `RESOURCE\rts.cm`), progress **15 %**. Then, if a profile exists (0x4CA290): `list_a/list_b` from the profile (0x4728F0/0x472C40), `gold = profile+0x38` (`GameState+0x734`), `play_ms = profile+0x3C` (`+0x740`); runs the stage script's **first pass**: slot 9 `self()`, cursor rewind (0x63ED40), slot 3 `run_step` (its `init` blocks create/load units); `0x6AD900(1)`, `0x4AE6C0` (message-array reset); builds two boxes around the controlled unit (position ± `DAT_009CF7A4` and ± `DAT_009CF7A0`) and calls `World::stream_sectors(near, far, 1)` 0x547630 (loads every sector of the near box synchronously, advancing the loading bar per sector via 0x75F3C0/0x75F4F0); `0x45F860(0)`, camera target `0x5639F0(DAT_00A47C36)`, camera mode 0 (0x429860(0)), drain input (0x5959C0), `DAT_00A5B9F0 = profile+0x40` (hero point pool), `progress = 10`, `DAT_00A5916D = now`, **`set_speed(1.0)` 0x42F930**, loading bar current = 100 (`_DAT_00A5025D`), final redraw, and sets **`GameState.deferred_start (+6, DAT_00A58F96) = 1`** (the draw function then runs `0x472D10(scenario)` = autosave after the first frame) | decompile; 0x472D10 | V |
| 0x41D400 | `battle_switch_stage(stage)` | in-place switch to another step of the same scenario (`stage != stage_index`): `stage_index = target = stage`, `end_state = 0`, `progress = 0`, compile+run `_des` and compile the main script of the new record exactly as 0x41CC80, then load the world **if `0x5613D0(world_file)` finds none** (else reuse), parse the per-sector unit files (`MAP\` + sector name + `DAT_009CF774` + `stage.unit_file` + `DAT_009CF810`) with `0x558A30`, run the first script pass, sector streaming, camera, `progress = 10`. No caller found in the exports (0x471620 performs stage switches with 0x41CC80 + 0x41C810 instead). Probably dead code; the shipped data never has more than one step (StepNo = 1 everywhere) | decompile | R |
| 0x41DFE0 | `battle_load_sandbox(world_path, script_name)` | debug/developer load by name (console key 0x79: file dialogs "load world" `*.wor` and "load script" `*.sce`, 0x52F930): msglog clear, `world_unload(0)`, `scenario_set_number(0)`, `stage_index = target = 0`, `end_state = 0`, copies the script name into `GameState+0xAD` and name+`_des` into `+0xEE`, `time_limit = time_left = −1`, loads the arrays, compiles `<name>.sce` into `DAT_00A59141` (the embedded script slot) and `<name>_des.sce` (run once), creates the world from the picked `.wor`, streams sectors, `progress = 10`, `memset(GameState+0x2B4, 0, 0x50)` (clears the 20 challenge timers). Used with `scenario == 0` | strings; 0x52F930 | V |
| 0x41EA60 | `battle_load_savegame(slot)` | loads a `SAVE\*.SAV`: `42DE70`, delete `DAT_00A5BA00`, SMS clear, `world_unload(0)`, bar 5 %, `0x70E3C0`, `0x4C9F00`, `0x470D40` (arrays), `0x6CA170`, bar 8 %, **`gamefile::0x477E80`** (restores arrays/units/flags/end conditions/scenario fields from the file), bar 13 %; saves the weather tables (`DAT_00A48394..B0`, 99+101+101 dwords at `DAT_00A47E14/7FA0/8134`), then recompiles the stage script (and for sandbox: `GameState+0xAD`) — **only compiled, not run** (script state comes from the file) — and the `_des` script, which **is run again** (harmless: its `init` is guarded by `get_scenario_stage() == 0`), bars 20 % / 25 % / 26 %; `0x70E1D0` (HUD restore), main-character animation bundle (`<model>_P.AM1`), world/scene re-initialisation as in 0x41C810 with bar 28 %, sector streaming, restores the saved weather tables, `progress = 10`, `DAT_00A5916D = now`, `set_speed(1.0)`, bar 100 %. **If the file was saved in victory state (`end_state == 1`) and the victory sequence is not running, it calls `0x6E7900`** | decompile; savegame family | R |
| 0x41BF40 | `battle_legacy_start(character)` | **legacy/unused**: `scenario_set_number(0)`, stage 1, then builds `SCRIPT\SCENARIO\` + `*(char*)(stages + 0x82)` with `stages == NULL` after the previous call (would fault), compares `character` with the ids of `MAIN_CHARACTER_HUMAN` / `MAIN_CHARACTER_AMAZON` (0x5270B0) and swaps the main character. No caller found | decompile | I |
| 0x41C780 | `world_unload(keep)` | see gameloop.md; `if (scenario != 5 \|\| keep == 0) scene_teardown 0x5624C0`; `game_world_unload(keep)` 0x471010; `progress = 0` | gameloop.md | V |
| 0x41C7E0 | `battle_is_world_ready()` | `progress > 9` (builtin `battle_is_in_game` 260 → 0x65A2D0 → this); gates `4E5FC0`'s counters, `4CD5D0`, the main world-update chain | 0x41C7E0, 0x65A2D0 | V |
| 0x41BF10 | `layer_enabled(layer)` | `return *(u8*)(this + layer + 1)` — per-layer enable byte of the weather/particle layer table; validates `start_weather_effect`'s argument and gates the two particle layers in `4E5FC0` | 0x41BF10; BUILTINS 3 | I |

Progress-bar convention: the loading screen object (global near `0xA501F0`, fields in §8) shows `current% = base% + (100 − base%) * (k − 1) / n [+ sub-step term]`; the loaders set `base%` (`_DAT_00A50259`) to the milestone, `n/k` come from `World::stream_sectors`, then call `0x75F3C0` (recompute) and `0x75F4F0` (draw a frame immediately **only if `GameApp.state == 0xC`**).

## 2. scenario.cpp (the `Scenario` object at 0xA58FA7 = GameState+0x17)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x4CCF40 | `scenario_set_number(n)` | if `n == 0 \|\| n != number`: `scenario_free`, `number = n`, `scenario_load_scr`, embedded `stage_value = 0`, `msglog_select_scenario(n)` 0x4B3160 | decompile | V |
| 0x4CC850 | `scenario_free()` | deletes stage scripts / name / description strings, the stage array (inside the loop: only correct for 1 stage), include cache 0x639030, embedded record, ref lists, hero saves, statistic block; zeroes `number, stage_index, target_stage, stage_count`; 0x4CDDC0 | `scenario.cpp` allocs | V |
| 0x4CCFB0 | `scenario_load_scr()` | parses `SCENARIO\scenario_NN.scr` (FileTxt) into `stage_count` × 0x1B2 records | strings `SCENARIO\`, `scenario_` | V |
| 0x4CD510 | `scenario_get_mode()` | `mode` of the current stage record (embedded when `number == 0`) | decompile | V |
| 0x4CD570 | `scenario_set_mode(m)` | store `mode` | 0x65AAD0 | V |
| 0x4CD790 | `scenario_mode_from_string(s)` | NORMAL 0, INTERACT 1, TRAINING 2, BATTLE 3 (default 3) | strings | V |
| 0x4CD5D0 | `scenario_tick_timers()` | if `end_state == 0 && battle_is_world_ready()`: `time_left -= DAT_00A500BE` when `time_limit > 0` (current stage record or embedded); `time_left < 1` → `end_state = 2` (defeat, no animation of its own); then if `DAT_00A500D2 != 0 && cut_scene_flag == 0`: `count_time[0..2] += DAT_00A500BE` | decompile | V |
| 0x4CD8B0 / 0x4CD960 | `scenario_add_unit_def_ref(id)` / `_item_def_ref(id)` | append a u16 to the def-reference lists (called by builtins `add_unit_ref` 0x65B190 and 0x651A60) | decompile | V |
| 0x4CDA10 | `scenario_add_hero_save(src)` | `new SaveGame(0x4B4)`, copy of `src` (0x4C96F0), append to `hero_saves` | decompile | V |
| 0x4CDE30 / 0x4CDF00 | `stage_set_name(s)` / `stage_set_description(s)` | free + realloc + strcpy into the stage record `name (+0x19E)` / `description (+0x1A2)` (called by `set_scenario_name/description` builtins 0x65AB10/0x65AB50) | decompile | V |
| 0x472000 / 0x4720D0 | `bits0_set(n, v)` / `bits0_get(n)` | bank `+0x18A`; `n` 1-based: `w = (w & ~(1 << ((n-1)&31))) \| ((v & 0xFF) << ((n-1)&31))` — **a value other than 0/1 shifts into other bits** (v = 2 sets bit n); get returns `(w >> (n-1)) & 1` | decompile | V |
| 0x472150 / 0x472220 / 0x472540 | `bits1_set/get/reset` | bank `+0x18E` (event flags); reset = `w = 0` | decompile | V |
| 0x4722A0 / 0x472370 | `bits2_set/get` | bank `+0x192` | 0x4722A0 read, 0x472370 by pattern | R |
| 0x4723F0 / 0x4724C0 | `bits3_set/get` | bank `+0x196` | 0x4723F0 read, 0x4724C0 by pattern | R |

All bit-bank functions use `number == 0 ? embedded_record.bank : stages[stage_index-1].bank` (embedded banks at `GameState+0x1B5/0x1B9/0x1BD/0x1C1`).

## 3. gameplay.cpp (GAMEPLAY.RES text index) — documented in [functions/gameloop.md](gameloop.md) §3 (V)

0x47C070 `gameplay_category_free`, 0x47C250 `gameplay_category_add_entry`, 0x47C3D0 `gameplay_parse_file`, 0x47C9E0 `gameplay_free`, 0x47CCA0 `gameplay_find_or_add_category`,
helpers 0x47C150/0x47CB30/0x47C6C0/0x47C330/0x47CF20/0x47C520. Nothing in this module touches scenario state; the `[GAME]` section holds only the difficulty multipliers
(`EASY_D 0.7, NORM_D 1.0, DIFF_D 1.5, VDIFF_D 2.0, EASY_A 0.5 ... 2.0, EASY_B 0.5 ... 2.0`, shipped `Resource/Gameplay.res`); consumers belong to the unit family.

## 4. Per-tick scenario flow `0x471620` (`scenario_flow_update`, `__fastcall`, ECX = GameState)

Called from `game_update` 0x4E5FC0 as the last element of the world-update chain (only if a world is loaded and `GameApp.state != 5`).

| step | behaviour | evidence | conf |
|---|---|---|---|
| 1 timers | `mode == 2` (training): only `challenge_duration (+0x304)` is decremented; otherwise the 20 `challenge_timer[4][5]` (`+0x2B4`). Decrement = `DAT_00A500BE` (game ms), **saturating at 0** | 0x471620 | V |
| 2 script | unless `GameApp.state (DAT_00A500C2) == 5` or `end_state == 2`: `run_step(0)` on the current main script — the embedded `GameState+0x1B1` object when `number == 0`, else `stages[stage_index-1].script`; return value ignored (the script re-runs from the top each tick, see SceScript.md §2.1) | 0x471620, SceScript.md §6 | V |
| 3 gate | the rest runs only if `number == 0 \|\| stage_index == target_stage`; else → step 7 | | V |
| 4 limits | `scenario_tick_timers()` 0x4CD5D0 (time limit → defeat; count_time accumulators) | | V |
| 5 end conditions | only if `end_state == 0 && end_cond_count != 0`: award loop, win test, lose test, win timer (see below) | | V |
| 6 victory | if `end_state == 0 && win_timer != 0 && now − win_timer >= delay_before_win && cut_scene_flag == 0`: `end_state = 1`; all enabled **type 3** records get `finish = 1`; if `scenario != 5` every enabled, described **type 2** record still at `finish == 0` is set to `finish = 2`; then, if the victory sequence is not already running (`0x6E7B50()` = interface `+0xD32`), start it with `0x6E7900()` | | V |
| 7 stage switch | if `stage_index != target_stage` (and `number != 0`): `battle_load_scenario(number, target_stage, 0)`, `battle_enter_world()`, `set_next_state(9)` | | V |

End-condition passes of step 5 (all over records `0..count-1`, a record participates only if `description != NULL`; "enabled" = `enabled != 0`):
1. **Award pass**: for `type ∈ {0, 2}` (enabled flag *not* checked), `finish == 1` and `awarded == 0`: set `awarded = 1`; `gold += gold_award` (the money mirror `GameState+0x734`); if `exp_award != 0`: for every unit recno `count..1` that exists, whose `ranking (unit+0x104) > 3` and for which `0x42FF90(recno)` is true (the unit's definition name equals one of the four hero names in the table at `0x9F861C`, stride 0x104, first entry `JOAN_OF_ARC`) call `unit_add_experience(exp_award, propagate = 1, 0)` 0x5229A0.
2. **Win test**: `win = true`; any enabled `type == 0` record with `finish != 1` → `win = false`.
3. **Lose test**: any enabled `type == 1` record with `finish == 2` → `end_state = 2`, `win = false`.
4. `win` → arm `win_timer = now` if it was 0; not `win` → `win_timer = 0`.
No enabled type-0 record at all (but `count > 0`) counts as won: the delay starts immediately. With `count == 0` none of this runs (`end_cond_count == 0` skips step 5).

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x471E20 | `end_condition_add()` | see [EndCondition](../structs/EndCondition.md); builtin 0x65A350 | V |
| 0x471EF0 | `end_condition_del_last()` | builtin 0x65A370 | V |
| 0x471F90 | `end_condition_release(idx)` | save-load unload helper | V |
| 0x467260 / 0x467300 / 0x467390 | `EndCond::free_strings / set_description / set_territory_link` | `end_con.cpp` | V |
| 0x5229A0 | `unit_add_experience(exp, propagate, flag)` | `if (scenario mode != 2 && unit_def.+2 != 8)`: `unit.exp (+0x126) += exp`; if `propagate`: when the unit is a group leader (`0x44A660`) its members get `ftol(exp * share)` recursively, else the leader (`0x4216D0`) gets a share (GAMEPLAY.RES `MEM_EXP 0.4` / `LD_EXP 0.4`); for the main character `0x6FE520(exp)` (HUD feedback), `DAT_00A5B9F0 += exp` capped at `DAT_00A3CC64` unless `DAT_00A5B9F8 == 4`; `0x522C40(flag)` (level-up check, `LEVELUP`); posts message `EXPUP` (0x494810). **Training mode (scenario mode 2) grants no experience.** Owned by the unit family; the battle-relevant rule is the mode check | R |

## 5. Victory / results flow functions

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x6E7900 | `victory_begin()` | `interface+0xD32 = 1` ("ended"); `0x6E7B80` (creates the two victory sprites `DAT_009D2DF8` and `COMMELE` if absent, resets `+0xD28/0xD29/0xD2A`); `0x4AE6C0` (clear message array); sets the victory title text (`+0xD2E`) to the literal `"Yow win!"` if `scenario == 0`, else the `VICTS` string table entry for the scenario (`0x4DD9C0`); plays event sound `0x4E4520(7, 0x7F, 1, 0, 0, 0)`; stops the main unit (`0x508B90(0,1)`) and, for every unit, drops follower links (`unit+0x2A1`) and calls `0x527FB0` (unit-family victory handler; both skipped for scenario 5), then `0x508B90(0,1)` (stop/idle the unit); finally sets the UI mode `0x42B6E0(0 \| 1)` from `DAT_00A5B9F8` (1 → 0, 0 or 3 → 1) | decompile | R |
| 0x6E7B50 | `victory_is_running()` | `interface+0xD32` | decompile | V |
| 0x720CA0 | `ingame_menu_update` (hud) | the end-of-scenario popup: choice 1 (`retry`): `battle_load_scenario(number, stage_index, 0)`, `set_next_state(0xC)` (+ a savegame restore for scenario 6); choice 2 (`continue`): scenario 8 → `0x6D19E0(0)` (ending); else writes `profile+0x38 = gold`, `+0x3C = play_ms`, `+0x40 = DAT_00A5B9F0`, saves the two lists (0x4727B0, 0x472B00) and the profile (0x4C8810), `DAT_00A58F95 = 1`, **`end_state = 3`**, `switch_state(0xD)` (results screen) | script_vm/engine export | R |
| 0x755D80 | `scenario_end_continue()` | results-screen "continue": `battle_load_scenario(number + 1, 1, 1)`; `set_next_state(0xC)`. There is **no bound check** on `number + 1` (`scenario_09.scr` does not exist → `scenario_load_scr` returns with `stage_count = 0`); the objectives screen opens the ending for mission 8 instead (OpenJoan `[Fixes] MissionCount` makes that the last mission present, docs/scripting/ADDING_MISSIONS.md) | gameloop.md | V |
| 0x4E6FE0 | audio manager | picks music cue 6 when `end_state == 2`, 7 when `end_state == 1` | engine export | R |

## 6. Calendar functions — see [Calendar](../structs/Calendar.md)

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x495290 | `Calendar::Calendar(year, month, day)` | `memset(0x50)`, fields at `+0x10/+0x14/+0x18`, serial, `day_of_year`, `+0x1C = 0`, `+0x20 = 1440.0`, `created_at = time(NULL)` | V |
| 0x4953D0 | `Calendar::day_roll()` | see Calendar.md §2.1 | V |
| 0x476BB0 | `calendar_update()` | per tick: `minute_of_day += ((K/60)/1000) * DAT_00A500BE` while `speed != 0`; on `>= 1440.0` reset to `day_start_minute` and `day_roll` | V |
| 0x4957E0 / 0x495810 | `year_of(serial)` / `month_of(serial)` | `0x57FA80(serial, 'Y'/'M')` | R |
| 0x57FEF0 / 0x57F990 | `day_of_year(y,m,d)` / `calc_serial(y,m,d)` | Calendar.md §1 | V |
| 0x4B68D0 / 0x4B6960 / 0x4B69D0 | `nations_daily / monthly / yearly` | iterate nations `1..count` that exist (`0x4B6780`) and call `Nation::daily 0x4B4920 / monthly 0x4B49F0 / yearly 0x4B4A10` (+ nation-level 0x4B6A40, 0x4B6D40 in the daily pass) | R |
| 0x45B160 | `weather_daily_roll()` | rain/snow start rolls (Calendar.md §2.2) | R |
| 0x4E42E0 → 0x486C40 | `daily_hook_3` | unknown daily hook | I |

## 7. Trigger system — see [Trigger](../structs/Trigger.md)

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x4EF090 | `trigger_update()` | per tick: sort (0x5850B0), then for `i = count..1` process each TriggerEvent (regions, hit volumes, gate events, sounds) | V (structure), I (semantics) |
| 0x4EEF00 | `trigger_post(...)` | `new TriggerEvent` (0x4EFF60), fill with 0x47E200, push (0x584620); called by `0x501190` (attack timing), `add_gate_trigger` 0x65C530, `break_wall` 0x650E70 | R |
| 0x4EF020 | `trigger_remove(i)` | `if (triggers[i]) virtual dtor 0x4EEEB0(1)`; dynarr erase 0x584A10 | V |
| 0x43A530 | `trigger_expired(t)` | `hold == 0 && end_ms != 0 && now >= end_ms` | V |
| 0x468A80 | `trigger_follow_owner(t)` | copy owner position into `t.x,y,z` when `follow_owner` | V |
| 0x41C5E0 | `item_footprint_contains(x, y, z, margin)` | oriented-rectangle test against an item's `def.+0x24/+0x26` extents, rotated by `item.+0x62` | R |
| 0x510F80 | `unit_state_is_attack(state)` | states `{0x08, 0x19, 0x1C..0x22, 0x46, 0x49, 0x53}` | V |
| 0x508FB0 | `unit_state_is_40_42(state)` | `0x40 <= state <= 0x42` | V |
| 0x4C0680 | `trigger_play_sound(i)` | positional sound for kind-4 events | R |
| 0x75D540 | `unit_apply_hit(unit)` | (unit family) hit resolution; return 2 aborts the volume loop | I |

## 8. State-screen handlers listed in the brief

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x75FA50 | `scenario_start_pressed()` | briefing "Start": `battle_enter_world()`; `set_next_state(9)`; returns 1 | decompile | V |
| 0x75F180 | `scenario_load_screen_update()` | `__fastcall` on the loading/briefing screen object; byte `+0x68` started (first call stamps `+0x86 = DAT_00A500B2`), phase byte **`+0x85`**, mode byte `+0x8F`, argument `+0x90`: phase 0 → `set_speed(0)` each tick; phase 4 with mode 2: after **5000 ms** (`now − +0x86 > 4999`) → phase 7; phase 5 and current% (`+0x6D`) == 0 → mode 0: `battle_enter_world()`, mode 1: `battle_load_savegame(+0x90)`, then phase 6, stamp, `set_speed(0)`; phase 8 → `set_next_state(9)`, `+0x68 = 0`, `+0x85 = 0`, `+0x8F = 0`, `set_speed(1.0)`, `+0x8E = 0`, return 1. Phases 1-3, 6, 7 are advanced by the screen's draw/input code (not exported) | decompile | R |
| 0x75F3C0 | `loading_recompute_percent()` | `cur = base` if `n == 0`; else `cur = clamp(base + (100−base)*(k−1)/n [+ ((100−base)/n)*(sub_k−1)/sub_n], 0, 100)` with `base +0x69`, `cur +0x6D`, `n +0x75`, `k +0x79`, `sub_n +0x7D`, `sub_k +0x81` | decompile | V |
| 0x75F4F0 | `loading_present()` | if `GameApp.state == 0xC`: draw a frame (`0x4D48F0`) and present (`0x603AF0`); a second draw when `cur == 100` | decompile | V |
| 0x75F5A0 / 0x755860 | `screen_exit_*` | `*obj = 0`; `0x42FEF0` (stop the screen's audio streams 0x592C30/0x592BD0). Exit handlers of the scenario-start screen (state 0xB) and the results screen (0xD) | decompile | R |
| 0x756190 | `loading_screen_release_images()` | release 13 UI image handles (`obj+0 .. +0x30`) with `0x60A550`; exit handler of state 0xC | decompile | V |
| 0x42F930 | `set_speed(f)` | `GameApp.speed (+0x44, DAT_00A500CC) = f`; `f == 0` → mute/pause three audio subsystems (0x45F6B0(0), 0x49C080(0), 0x63C6D0(0)); else restore (`0x45F6B0(DAT_00A57FF9)`, `0x49C080(1)`, `0x63C6D0(1)`). Called with 0 during loads and 1.0 at the end | decompile | V |

Open point: the progress-bar draw guard `GameApp.state == 0xC` suggests the object with fields `+0x68..+0x90` is the **state-0xC** screen while gameloop.md assigns `0x75F5A0` (exit) / `0x75FA50` (start button) to state 0xB; see Open questions.

## 9. Builtin handlers whose logic lives in this family

Argument order as in BUILTINS.md; "R" = reads only, "W" = writes game state. All handlers return 1 unless stated. `n` indices are 1-based. Names of globals as in the structs pages.

| id | builtin | handler | effect |
|---|---|---|---|
| 3 | `start_weather_effect(id)` | 0x651130 | `layer_enabled(id)` 0x41BF10 validates, then copies `DAT_00A47BB4` (calendar minute) into the weather start stamps `DAT_00A4839C/AC` (lights.md) |
| 39 / 332 | `break_wall` / `add_gate_trigger(item, delay_ms)` | 0x650E70 / 0x65C530 | post TriggerEvents (flags 0x40 / 0x20) at `now + delay` ([Trigger](../structs/Trigger.md)) |
| 204 | `set_game_finish("WIN"\|"LOSE")` | 0x6594E0 | `end_state = 1 / 2` directly (no delay, no victory sequence, no award); unknown strings: no change. Unused by the shipped scripts |
| 205 | `set_game_start_date(y, m, d, minute, lo, hi)` | 0x659550 | Calendar.md §3 |
| 206 | `is_game_finish()` | 0x6595E0 | `end_state != 0` |
| 207 / 208 | `set_game_time_limit(ms)` / `get_game_time_limit()` | 0x659610 / 0x6596A0 | stage record (embedded if `number == 0`) `time_limit = time_left = ms` / returns `time_left` (units: game ms) |
| 209-215 | `calculate_date`, `get_game_date/hour/minute/day/month/year` | 0x6596F0.. | Calendar.md §3 |
| 216 | `change_scenario_step(n)` | 0x659830 | `target_stage = n` → switch on the next `scenario_flow_update` (no validation of `n`; unused beyond an orphan cut-scene script `S2L0_cut_scene_1.sce`) |
| 217 | `set_ride_mode(b)` | 0x659860 | `0x51C4B0(b)` (unit family) |
| 218 | `set_fog_distance(n)` | 0x659890 | `DAT_00A48CDC = (float) n` (same global as fog-region triggers) |
| 219 / 220 | `set_objective_count_time(n)` / `get_..` | 0x6598C0 / 0x6598F0 | `GameState+0x23C` |
| 221-226 | `set/get_scenario_count_time[_2/_3]` | 0x659910-0x6599E0 | `set` zeroes, `get` returns the three accumulators `GameState+0x1E1/0x1E5/0x1E9` (ms of game time outside cut scenes) |
| 227 / 228 | `set_scenario_rescue_count(n)` / `set_scenario_rescue_limit(n)` | 0x659A00 / 0x659A30 | `GameState+0x240` / `+0x244` (read by the HUD/results; no getter) |
| 229-231 | `add_reputation(n)`, `set_reputation(n)`, `get_reputation()` | 0x659A60 / 0x659A90 / 0x659AC0 | `GameState+0x248` (`+= n` / `= n` / read) |
| 232-235 | `set/get_citizen_killed_count`, `set/get_fcitizen_killed_count` | 0x659AE0 / 0x659E80 / 0x659EA0 / 0x659ED0 | the "set" variants take no argument and **reset to 0**; "get" reads `+0x1FD` / `+0x209` |
| 236 / 237 | `set_enemy_bribed_count()` / `get_..` | 0x659B30 / 0x659B10 | reset / read `+0x219` |
| 238-243 | `get_enemy_bribed_cost`, `get_gold_crates_get`, `change_mission_gold_to_real_gold`, `get_food_crates_get`, `get_food_picked_up`, `set_food_can_be_picked` | 0x659E40, 0x659B60, 0x659B80, 0x659CA0, 0x659CC0, 0x659CE0 | read `+0x21D`, `+0x226`, `+0x22E`, `+0x232`; set byte `+0x236`; 0x659B80 converts every item whose def name is `s7_goldbag` (def kind `+0x4E == 10`) into `S7_Barrels` (model index of `S7_LGOLDBAG` written into its object) — scenario 7 only |
| 244 / 245 | `set_total_food(n)` / `get_total_food()` | 0x659D10 / 0x659D40 | `+0x22A` |
| 246-248 | `set_citizen_saved()`, `get_citizen_saved()`, `increase_citizen_saved()` | 0x659D60 / 0x659D90 / 0x659DB0 | reset / read / **post-increment (returns the old value)** `+0x238` |
| 249 / 250 | `add_scenario_general(id)` / `remove_scenario_general(id)` | 0x659DE0 / 0x659E10 | `0x4CDD40(id)` / `0x4CDC40(id)` (scenario general list; not exported, `TODO(re)`) |
| 251 | `get_bribed_enemy_kill_count()` | 0x659E60 | `+0x221` |
| 252 / 253 | `set_house_burnt_count()` / `get_house_burnt_count()` | 0x659EF0 / 0x659F20 | reset / read `+0x215` |
| 254 / 255 | `set_challenge_restart_duration(attr, idx, ms)` / `get_..(attr, idx)` | 0x659F40 / 0x65A010 | `challenge_timer[row][idx]` (ms), `row` from `"STRENGTH"/"DEXTERITY"/"LEADERSHIP"/"DEFENSE"` (default row 0), counted down every tick by `scenario_flow_update` |
| 256 / 257 | `set_challenge_flag(attr, idx, b)` / `get_..` | 0x65A0E0 / 0x65A1B0 | `challenge_flag[row][idx]` signed byte |
| 258 / 259 | `set_challenge_duration(ms)` / `get_challenge_duration()` | 0x65A2A0 / 0x65A280 | `GameState+0x304` |
| 260 | `battle_is_in_game()` | 0x65A2D0 | `battle_is_world_ready()` |
| 261 | `switch_controllable_unit(n)` | 0x65A2F0 → 0x432300 | camera-controller switch (hero table at `0x9F8B10`, 4 names, stride 0x104, `JOAN_OF_ARC` first): `n == 0` cycles to the next living hero in table order; `n >= 1` selects table entry `n-1` (if different from the current and eligible: alive, not state `+0x2F`, `+0x114 != −1`, reachable); sets the controlled unit (`0x42F9C0`), mode flags, refreshes the HUD (`0x70FB70`, `0x5639F0`); camera-mode-4 (RTS) special cases |
| 262 | `quit_rts_mode()` | 0x65A320 | `0x42E040(DAT_00A5B9FC, 0)` = camera controller mode change to 0 (leave RTS) |
| 263-276 | `add/del_end_condition`, `set_end_condition_*` | 0x65A350-0x65A840 | [EndCondition](../structs/EndCondition.md) |
| 277 / 278 | `set/get_scenario_bit_flag(n[, v])` | 0x65A870 / 0x65A8A0 | bank 0 (`bits0_*`), `n` 1-based (1..32), `v` byte; get returns a signed char (0/1) |
| 279-281 | `set/get_scenario_event_bit_flag`, `reset_..` | 0x65A8D0 / 0x65A900 / 0x65A930 | bank 1; reset clears the whole word |
| 282-285 | `set/get_scenario_event_bit_flag_2/_3` | 0x65A960 / 0x65A990 / 0x65A9C0 / 0x65A9F0 | banks 2 and 3 |
| 286 / 287 | `set_scenario_stage(v)` / `get_scenario_stage()` | 0x65AA20 / 0x65AA80 | `stage_value` of the current record (`number == 0` → `DAT_00A59155`) |
| 288 | `set_scenario_mode(str)` | 0x65AAD0 | `scenario_mode_from_string` → `scenario_set_mode` |
| 289 / 290 | `set_scenario_name(s)` / `set_scenario_description(s)` | 0x65AB10 / 0x65AB50 | 0x4CDE30 / 0x4CDF00 (the text arrives already localised by the VM) |
| 291 / 292 | `backup_bit_flag()` / `restore_bit_flag()` | 0x65AB90 / 0x65ACA0 | copy the four banks of the current record to / from `GameState+0x07/0x0B/0x0F/0x13` (single slot; restore overwrites all four) |
| 293 | `set_music_side_environment_enable(b)` | 0x65ADB0 | `GameApp+0x152 (DAT_00A501DA) = (b != 0)` |
| 294 | `set_cut_scene_flag(b)` | 0x65ADE0 | `b == 1` → `CutScene::start(now, +128.0, no_camera = 1, 0, ...)` 0x6CB9E0; else `CutScene::end(now, −128.0, 0, 0)` 0x6CC6A0; returns the method's bool |
| 295 | `get_cut_scene_flag()` | 0x65B130 | `cut_scene_flag` (`0x4301C0`) |
| 296 / 297 | `start_cut_scene` / `start_cut_scene_2` (…) | 0x65AEF0 (n=6/7) / 0x65AE60 (n=8/9) | see §10 |
| 298 / 299 | `end_cut_scene(a[, b])` | 0x65AFF0 | `CutScene::end(now, −128.0, a, b)`; with one script argument the second is missing/0 |
| 300 | `get_cut_scene_status()` | 0x65AFD0 → 0x65AFB0 | `CutScene.state` (`+0xF`); registered with a `void` tag in BUILTINS |
| 302 | `change_camera_in_cut_scene` | 0x65B030 (5 args) / 0x65B070 (7 args, ints converted to float) | `0x6CD720` / `0x6CCF00` (camera-track rewrite, camera.md) |
| 303 | `is_cut_scene_status_black_screen()` | 0x65B0E0 | `!CutScene.active (0x42CED0)` → true; else `state == 7` |
| 307 | `start_scenario_part_interface(b)` | 0x6539A0 | loading-screen `obj+0x8E = b`, `obj+0x8F = 2` (mode "interstitial"), `set_next_state(0xC)`; the screen then waits 5 s (phase 4 → 7 → ... → 8) and goes back to state 9 |
| 315 | `save_game(name, desc[, flag])` | 0x65BED0 | `SAVE\<name>.<ext 0x9CFB78>`; finds/creates the slot (0x477640 / 0x430410), stores `desc` at `slot+0x11A`, `slot+0x19A = scenario number`, `slot+0x19E = flag`, writes the file (`gamefile::0x478180`, scene snapshot 0x4731E0). `flag` is the implicit third parameter (= 1 for the scenario-start autosave `0x472D10`) |
| 402 | `set_delay_before_win(ms)` | 0x65FBA0 | `GameState+0x738 = ms` |

## 10. Cut-scene object (`CutScene`, methods 0x6CB9E0 / 0x6CAD50 / 0x6CC6A0 / 0x6CA660)

Global object (address hidden; `6CA170` is its reset: `+0 = 0`, `+4 = 0`, `+7/+0xB/+0xF = 0`, `+0x18..0x20 = DAT_00A69CD8..E0` (the zero vector), `+0x24..0x2C = 0`, `+0x41/+0x42/+0x43 = 1`).

| offset | type | meaning |
|---|---|---|
| +0x00 | u32 | start stamp (`now` passed to the start/end call, via `0x6CAB90`) |
| +0x04 | u8 | **active** (`0x42CED0`); `0x6CAB90` sets it; start refuses to run when already active, end refuses when not active |
| +0x05/+0x06 | u8 | start-wipe mode (`wipe_in`) / end-wipe mode (`wipe_out`) |
| +0x07 | f32 | fade rate (+128.0 per second on start, −128.0 on end) |
| +0x0B | f32 | screen-overlay alpha 0..255 (`0x437F0000` = 255.0; set to 255 when the wipe hold (state 7) starts and to 0 when it ends; otherwise integrated by `rate * dt / 1000` in the fade branch of `0x6CA660`, `dt` clamped by `DAT_009CFC04` when `DAT_00A500BE >= 1000`) |
| +0x0F | i32 | **state** (below) — `get_cut_scene_status` |
| +0x13 | u8 | a camera track (`cut_scene_play`) is running |
| +0x14 | ptr | optional per-tick callback object (non-null → `0x425FC0`) |
| +0x18..+0x2C | f32×6 | destination: x, y, z-offset, and three camera parameters (script args 4-6) |
| +0x30 | i32 | camera mode saved at start (`0x563DC0`), restored by `0x429860` at the end |
| +0x34 | i16 | wipe curtain x offset (starts at screen width `DAT_00A4885C`, moves by `(tex_w + screen_w) * DAT_00A500BE / 1500` px per tick) |
| +0x36 | u32 | stamp for state 7 |
| +0x3A | tex* | `cut_scene_texture` (width at `tex+0x3C`), created on demand (0x6099C0), released at the end |
| +0x3E | u8 | wipe direction toggle |
| +0x3F | u8 | "entering" (1 while starting) |
| +0x40 | u8 | "ending" (1 once the end was requested) |
| +0x41/+0x42/+0x43 | u8 | saved copies of the camera-lock flags `DAT_00A48CE3/E7/EB` (zeroed during the cut scene, restored at the end) |
| +0x44 | i32 | saved value of `0x4D2A00()` (restored by `0x5EF400`) |

State values `+0x0F`: 0 idle, **1** camera track playing, **2** static (no camera motion / track finished), **4** end sequence with return camera track, **6** wipe moving across the screen, **7** hold fully covered for **0x2ED = 749 ms**, **8** wipe moving off (reveal), other values (3, 5): alpha fade branch (the final `else` of `0x6CA660`). Transitions are in

`start_cut_scene` argument mapping (inferred from `0x65AEF0` → `0x6CB9E0(now, 128.0, a1, a2, a3, a4, a5, a6, a7)` and `0x65AE60` → `0x6CAD50(now, 128.0, a1, a2, x, y, z, ...)`): `a1` (char) = **no camera move** (1: state 2 immediately), `a2` (char) = **build a 15-key camera track** (1) or just jump (0), `a3` = main subject unit recno (9-argument form: `a3..a5` are x, y, z floats), remaining numbers = yaw/pitch/distance targets, last byte = **wipe-in** mode (`+0x05`). `start_cut_scene` (296) is the form without the last byte (it defaults to 0), `start_cut_scene_2` (297) passes it (the "implicit extra parameter" TODO of BUILTINS §6.4 is therefore the last byte).

## 11. RNG use in this family

The project RNG is a 32-bit LCG: `state = state * 0x015A4E35 + 1`; `rand(n) = (n * ((state >> 16) & 0x7FFF)) >> 15` (`0x590540`, state at `object+0x7A`; two objects are seeded with `time(NULL)` at start via `rng_seed_time` 0x5904E0; the object used by each call site is not visible).
Call sites in this family: `0x45B160` (two `rand(100)` rain/snow probability rolls + two start-time rolls, per day roll). `0x4EF090` itself calls none; `0x75D540` (unit family) rolls `rand(100)` for the block chance. `random_number` (script builtin) is documented in script_builtins.md.
`0x590B00` = clamp(double), `0x590AA0` = clamp(int).

## 12. Constants

| value | meaning | where |
|---|---|---|
| 0x1B2 (434) | sizeof(ScenarioStage) | 0x4CC850, 0x4CCFB0 |
| 0x41 (65) | filename field size (6 per stage) | 0x4CCFB0 |
| 0x1A (26) × 0x28 (40) | EndCondition record × capacity | 0x471E20 |
| 0x43 (67) | sizeof(TriggerEvent) | 0x4EEF00 |
| 0x4B4 | sizeof(SaveGame) record in `hero_saves` | 0x4CDA10 |
| 4 × 5 | challenge rows (STRENGTH, DEXTERITY, LEADERSHIP, DEFENSE) × slots | 0x659F40 |
| 5000 ms | "scenario part" interstitial hold | 0x75F180 |
| 749 ms (0x2ED) | cut-scene black hold | 0x6CA660 |
| 1500 | wipe divisor (ms) | 0x6CA660 |
| ±128.0 | cut-scene fade rate per second | 0x65ADE0, 0x65AE60, 0x65AEF0, 0x65AFF0 |
| 255.0 | alpha cap | 0x6CA660 |
| 1440.0 | minutes per day (literal in 0x476BB0) | 0x476BB0 |
| 30 | day-of-month clamp | 0x4953D0 |
| 0x1A4451 | serial offset | 0x57F990 |
| 0x15A4E35 | RNG multiplier | 0x590540 |
| 5 / 15 / 100 % | loading milestones of 0x41C810; 5/8/13/15/20/25/26/28/100 for 0x41EA60 | 0x41C810, 0x41EA60 |
| 10 | "world ready" progress value (`> 9`) | 0x41C7E0 |
| 79 ms (0x4F) | owner animation grace in hit events | 0x4EF090 |

## 13. Cross-module requests

* Types: `Scenario` at `0xA58FA7` (embedded in `GameState`), `ScenarioStage[stage_count]` (`0x1B2`), `EndCondition[40]` at `0xA5929B`, `Calendar` at `0xA47BA8`, `TriggerEvent` (`0x43`).
* Renames: 0x41CC80 `battle_load_scenario`, 0x41C810 `battle_enter_world`, 0x41D400 `battle_switch_stage`, 0x41DFE0 `battle_load_sandbox`, 0x41EA60 `battle_load_savegame`, 0x41BF40 `battle_legacy_start`, 0x41C7E0 `battle_is_world_ready`, 0x41BF10 `layer_enabled`, 0x471620 `scenario_flow_update`, 0x4EF090 `trigger_update`, 0x4EEF00 `trigger_post`, 0x476BB0 `calendar_update`, 0x4953D0 `calendar_day_roll`, 0x495290 `Calendar_ctor`, 0x4CCF40 `scenario_set_number`, 0x4CC850 `scenario_free`, 0x4CCFB0 `scenario_load_scr`, 0x4CD5D0 `scenario_tick_timers`, 0x6E7900 `victory_begin`, 0x42F930 `set_speed`, 0x75F3C0 `loading_recompute_percent`, 0x75F4F0 `loading_present`, 0x590540 `rng_next(n)`.
* Globals: `DAT_00A58FAB g_stage_index`, `DAT_00A58FAF g_target_stage`, `DAT_00A58FB7 g_stages`, `DAT_00A59298 g_end_state`, `DAT_00A596C4 g_gold`, `DAT_00A596C8 g_delay_before_win`, `DAT_00A596CC g_win_timer`, `DAT_00A500BA g_game_time_ms`, `DAT_00A500BE g_dt_game_ms`, `DAT_00A500CC g_game_speed`, `DAT_00A500C2 g_app_state`, `DAT_00A47BB4 g_cal_minute`, `DAT_00A48CDC g_fog_distance`.
* GameState page corrections for its owner: `+0x17..` is the [Scenario](../structs/Scenario.md) record; `+0x1D1/+0x1D5` are the embedded record's `time_limit/time_left`; `+0x2B4` is `challenge_timer[4][5]` with `+0x29F..0x2B3` the 20 challenge flag bytes (overlapping the GameState note `u2a1`); `+0x308` is `end_state` (0/1/2/3), `+0x734 gold`, `+0x738 delay_before_win`, `+0x73C win_timer`, `+0x744 cut_scene_flag`, `+0x07..0x16` flag backup; `+0x1F1` is the first of the statistic words.
* Hit-event consumers (`0x75D540`, `0x47E200`) belong to the unit family; fog/region globals (`0x5F0340`) to the camera/lights families.

## 14. Open questions (addresses)

* `TODO(re)` address of the battle `progress` counter (ECX of 0x41C7E0/0x41CC80): read the `mov ecx, imm32` before the calls.
* Resolved from the executable (final): `0x9D3D74` = 24.0f (calendar K), `0x9CF9CC` = 60.0, `0x9DB8A0` = "WIN", `0x9DB898` = "LOSE", `0x9CF774` = "_", `0x9CF810` = ".UNI", `0x9CF818` = "MAP\", `0x9CF7F4` = ".SCE". So 0x41D400 builds `MAP\<sector name>_<unit_file>.UNI`, and scripts load from `SCRIPT\SCENARIO\<name>.SCE`.
* `TODO(re)`: `0x9CF7A0/0x9CF7A4` (sector-streaming box half-extents), `0x9CF820` (`_des`, assumed), `0x9CFB78` (save extension).
* `TODO(re)`: which GameApp state owns the loading-screen object at `0xA501F0` (0xB vs 0xC); who advances its phases 1-3, 6, 7.
* `TODO(re) 0x5850B0` comparator; `0x4853E0/0x485410` array identity (Trigger.md).
* `TODO(re)`: `unit.+0x104` (`ranking`) threshold `> 3` in the award pass and the meaning of def `+0x11C`, `+0x1C3`.
* `TODO(re) 0x4DD0E0 / 0x4DD120`: the scenario-name / description getters used by the autosave `0x472D10`.
* `TODO(re)`: `change_scenario_step` writer chain: confirm that the only way to reach `battle_switch_stage` (0x41D400) is none (no caller).
* Defeat flow: nothing in this export reacts to `end_state == 2` except stopping the script (`0x471620`), the music cue (`0x4E6FE0`) and `call_gameover_msg` (script); the game-over popup is in the HUD family (`0x720CA0` choice handling).
