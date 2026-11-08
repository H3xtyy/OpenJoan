# Scenario (`scenario.cpp`) and ScenarioStage — the scenario record inside GameState — joan.exe

The `scenario::` methods (`0x4CC850`, `0x4CCFB0`, `0x4CCF40`, `0x4CD510`, `0x4CD5D0`, ...) take as `this` the address **`0xA58FA7` = `GameState + 0x17`**
(verified: `scenario::fn_004cc850` and `fn_004ccfb0` index `this+0x00/+0x0C/+0x10`, while `game.cpp` code addresses the same bytes as `GameState+0x17/+0x23/+0x27`).
So the scenario is **embedded in [GameState](GameState.md)** (that page lists `+0x17 scenario`, `+0x1B stage_index`, `+0x1F target_stage`, `+0x27 stages`, `+0x2B4 timers`, `+0x30B end_conditions`
as GameState fields: they are the fields below). Offsets in the first column are **relative to `0xA58FA7`**; the second column is the GameState offset (`+0x17`).
Source file `d:\ww\scenario.cpp`. Functions: [functions/battle.md](../functions/battle.md) §2.
End-condition table: [EndCondition](EndCondition.md). Calendar: [Calendar](Calendar.md). Hit-event queue: [Trigger](Trigger.md).

## 1. ScenarioStage record (stride **0x1B2 = 434** bytes)

One record per `.scr` step group ([docs/formats/scr.md](../../formats/scr.md)): array on the heap at `Scenario+0x10` (`GameState+0x27`, global `DAT_00A58FB7`),
`stage_count` records, allocated by `0x4CCFB0` as `stage_count * 0x1B2` bytes zeroed with `memset`, addressed everywhere as `stages + (stage_index-1)*0x1B2`.
**The same layout is embedded at `Scenario+0x14` (`GameState+0x2B`, 0x1B2 bytes, "cur_stage")** and is used instead of the array when `Scenario.number == 0`
(sandbox/debug scenario loaded by name, `0x41DFE0`): every accessor tests `number == 0 ? cur_stage.field : stages[stage_index-1].field`.

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x000 | 0x41 | char[65] | `world_file` | `.scr` `WorldFileName` (e.g. `scenario1\S1L1w`); passed to world creation `0x44E8C0(1, name, name, 0)` | 0x4CCFB0 (`FUN_007b2020` = strcpy into `+0`), 0x41C810 | verified |
| 0x041 | 0x41 | char[65] | `unit_file` | `UnitFileName`; used by `0x41D400` to build per-sector `.uni` paths | 0x4CCFB0 (`+0x41`), 0x41D400 | verified |
| 0x082 | 0x41 | char[65] | `script_file` | `ScriptFileName` (e.g. `S1L1`); compiled from `SCRIPT\SCENARIO\<script_file>.sce` | 0x4CCFB0 (`+0x82`), 0x41CC80 | verified |
| 0x0C3 | 0x41 | char[65] | `desc_file` | `DescriptionFileName` (e.g. `S1L1_des`); compiled from `SCRIPT\SCENARIO\<desc_file>.sce` | 0x4CCFB0 (`+0xC3`), 0x41CC80 | verified |
| 0x104 | 0x41 | char[65] | `obj_file` | `ObjectiveFileName` (`.obj`) — read by the HUD, not by this family | 0x4CCFB0 (`+0x104`) | read |
| 0x145 | 0x41 | char[65] | `objmap_file` | `ObjectiveMapFileName` (`.bm`) | 0x4CCFB0 (`+0x145`) | read |
| 0x186 | 4 | Script* | `script` | compiled main scenario script ([SceScript](SceScript.md), `operator new(0x70)`, ctor 0x63F190); `NULL` until `0x41CC80`/`0x41D400` load it; deleted by virtual dtor `(*obj)(1)` | 0x4CCFB0 sets 0; 0x41CC80 | verified |
| 0x18A | 4 | u32 | `bits0` | bit bank used by `set/get_scenario_bit_flag` | 0x472000/0x4720D0 | verified |
| 0x18E | 4 | u32 | `bits1` | bit bank `set/get_scenario_event_bit_flag`; `reset_scenario_event_bit_flag` zeroes it | 0x472150/0x472220/0x472540 | verified |
| 0x192 | 4 | u32 | `bits2` | bit bank `set/get_scenario_event_bit_flag_2` | 0x4722A0/0x472370 | verified |
| 0x196 | 4 | u32 | `bits3` | bit bank `set/get_scenario_event_bit_flag_3` | 0x4723F0/0x4724C0 | verified |
| 0x19A | 4 | i32 | `stage_value` | the script-visible "scenario stage": `set_scenario_stage(n)` / `get_scenario_stage()` (0 on load). **Not** the same thing as `stage_index`: scripts use it as a progress counter (S1L1: 0 = first run → set start date/load units; 1 → 2 → normal play). Preserved in saves; the `_des` script is guarded by `get_scenario_stage() == 0` | 0x65AA20/0x65AA80; S1L1.sce | verified |
| 0x19E | 4 | char* | `name` | scenario title (`set_scenario_name`), heap copy (`strlen+1`) | 0x4CDE30 | verified |
| 0x1A2 | 4 | char* | `description` | scenario description (`set_scenario_description`) | 0x4CDF00 | verified |
| 0x1A6 | 4 | i32 | `time_limit_ms` | `set_game_time_limit(ms)` stores the value here **and** in `time_left`; **−1 = none** (set to −1 by the `.scr` loader). Informational copy | 0x659610, 0x4CCFB0 | verified |
| 0x1AA | 4 | i32 | `time_left_ms` | counted down by `dt` in `0x4CD5D0` while `time_limit_ms > 0`; reaching `< 1` ends the scenario in **defeat** (`end_state = 2`). `get_game_time_limit()` returns *this* field | 0x4CD5D0, 0x6596A0 | verified |
| 0x1AE | 4 | i32 | `mode` | `set_scenario_mode(str)`: `"SCENARIO_NORMAL_MODE"` = 0, `"SCENARIO_INTERACT_MODE"` = 1, `"SCENARIO_TRAINING_MODE"` = 2, `"SCENARIO_BATTLE_MODE"` = 3 (**any other string also 3**). 0 on load | 0x4CD790, 0x4CD570, 0x4CD510 | verified |
| 0x1B2 | | | end of record | | | |

Sandbox (`number == 0`) note: the embedded record's `script_file`/`desc_file` bytes are `GameState+0xAD` (`DAT_00A5903D`) and `+0xEE` (`DAT_00A5907E` = name + literal `_des`)
— this explains the GameState entries `scenario_name`/`scenario_des_name` — and `time_limit`/`time_left` are `GameState+0x1D1/+0x1D5` (`DAT_00A59161/65`, both set to −1 by `0x41DFE0`).
The embedded `script` pointer is `DAT_00A59141` (`GameState+0x1B1`), the four bit banks `GameState+0x1B5/+0x1B9/+0x1BD/+0x1C1` (`DAT_00A59145..51`), `stage_value` `DAT_00A59155`
(`GameState+0x1C5`), `mode` `GameState+0x1D9`.

## 2. Scenario object fields (relative to `0xA58FA7`)

| rel. offset | GS offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|---|
| 0x000 | 0x17 | 4 | i32 | `number` | scenario number: **0 = none/sandbox** (no `.scr`), 1..8 = `SCENARIO\scenario_%02d.scr` (file number, not the `code:` text: `scenario_02.scr` holds `SCENARIO_03`). Scenario **5** and **6** are special-cased | 0x4CCFB0 builds the name from it, 0x4CCF40 | verified |
| 0x004 | 0x1B | 4 | i32 | `stage_index` | 1-based current step (`DAT_00A58FAB`); 0 only in sandbox loads by name (`0x41DFE0`) | 0x41CC80 (`= param_3`), 0x41DFE0 (`= 0`) | verified |
| 0x008 | 0x1F | 4 | i32 | `target_stage` | requested step (`DAT_00A58FAF`). **Written by `change_scenario_step(n)` (0x659830)**; `scenario_flow_update` (0x471620) switches when `target_stage != stage_index` | 0x659830, 0x471620 | verified |
| 0x00C | 0x23 | 4 | i32 | `stage_count` | `StepNo` of the `.scr` (1 in all 8 shipped files) | 0x4CCFB0 (`atoi` of 4th token) | verified |
| 0x010 | 0x27 | 4 | Stage* | `stages` | see §1 | 0x4CCFB0 | verified |
| 0x014 | 0x2B | 0x1B2 | Stage | `cur_stage` | embedded record for `number == 0` | 0x4CC850 (memset `this+0x14`, 0x1B2), 0x4CD510 | verified |
| 0x1C6 | 0x1DD | 4 | u32 | `start_stamp` | `DAT_00A5916D`: set to the game clock `DAT_00A500BA` at the end of every load (0x41C810, 0x41DFE0, 0x41EA60); no reader in this export | 0x41C810, 0x41DFE0, 0x41EA60 | read |
| 0x1CA | 0x1E1 | 4 | u32 | `count_time[0]` | `set_scenario_count_time()` zeroes it, `get_scenario_count_time()` returns it; **accumulates `DAT_00A500BE` (game ms) each update while `cut_scene_flag == 0`** | 0x4CD5D0, 0x659910/0x659940 | verified |
| 0x1CE | 0x1E5 | 4 | u32 | `count_time[1]` | same, `_2` builtins 0x659960/0x659990 | verified |
| 0x1D2 | 0x1E9 | 4 | u32 | `count_time[2]` | same, `_3` builtins 0x6599B0/0x6599E0 | verified |
| 0x1DA | 0x1F1 | 4 | u32 | `mission_ms` | elapsed mission time (`+= DAT_00A500B6`, 0x4E5FC0) — GameState page | 0x4E5FC0 | verified |
| 0x1E6 | 0x1FD | 4 | u32 | `citizen_killed` | `get_citizen_killed_count` 0x659E80 reads; `set_citizen_killed_count()` (0x659AE0, no args) **resets to 0** | read |
| 0x1F2 | 0x209 | 4 | u32 | `fcitizen_killed` | 0x659EA0 (reset) / 0x659ED0 (get) | read |
| 0x1FE | 0x215 | 4 | u32 | `house_burnt` | 0x659EF0 (reset) / 0x659F20 (get) | read |
| 0x202 | 0x219 | 4 | u32 | `enemy_bribed_count` | 0x659B30 (reset) / 0x659B10 (get) | read |
| 0x206 | 0x21D | 4 | u32 | `enemy_bribed_cost` | `get_enemy_bribed_cost` 0x659E40 | read |
| 0x20A | 0x221 | 4 | u32 | `bribed_enemy_kill_count` | 0x659E60 | read |
| 0x20F | 0x226 | 4 | u32 | `gold_crates_get` | 0x659B60 | read |
| 0x213 | 0x22A | 4 | u32 | `total_food` | `set_total_food(n)` 0x659D10 / get 0x659D40 | read |
| 0x217 | 0x22E | 4 | u32 | `food_crates_get` | 0x659CA0 | read |
| 0x21B | 0x232 | 4 | u32 | `food_picked_up` | 0x659CC0 | read |
| 0x21F | 0x236 | 1 | u8 | `food_can_be_picked` | 0x659CE0 | read |
| 0x221 | 0x238 | 4 | u32 | `citizen_saved` | `set_citizen_saved()` resets 0x659D60; `get` 0x659D90; `increase_citizen_saved()` 0x659DB0 returns the **old** value then ++ | read |
| 0x225 | 0x23C | 4 | i32 | `objective_count_time` | `set/get_objective_count_time(n)` 0x6598C0/0x6598F0 (`DAT_00A591CC`). **Not cleared** by scenario unload | read |
| 0x229 | 0x240 | 4 | i32 | `rescue_count` | `set_scenario_rescue_count(n)` 0x659A00 (`DAT_00A591D0`) | read |
| 0x22D | 0x244 | 4 | i32 | `rescue_limit` | `set_scenario_rescue_limit(n)` 0x659A30 (`DAT_00A591D4`) | read |
| 0x231 | 0x248 | 4 | i32 | `reputation` | `add_reputation(n)` 0x659A60 (`+= n`), `set_reputation` 0x659A90, `get_reputation` 0x659AC0 (`DAT_00A591D8`) | read |
| 0x277 | 0x28E | 4+2 | u16* + u16 | `unit_def_refs` / count (`+0x27B`) | def indices recorded by `add_unit_ref` (0x4CD8B0: realloc `(count+1)*2`, append). On free each id → `0x42D0A0(id)` (unit def) → `0x527690` (release) | 0x4CD8B0, 0x4CC850 | verified |
| 0x27D | 0x294 | 4+2 | u16* + u16 | `item_def_refs` / count (`+0x281`) | filled by 0x4CD960 (called by builtin 0x651A60); on free → `0x425CE0(id)` item def → `0x4A3E30` | 0x4CD960, 0x4CC850 | verified |
| 0x283 | 0x29A | 4+2 | SaveGame** + u16 | `hero_saves` / count (`+0x287`) | copies of profile save records (`operator new(0x4B4)`, ctor `0x4155E0`, filled by `0x4C96F0`; copies fields `+0x1B0`, `+0x1BB`, `+0x1B9` of the source) appended by 0x4CDA10 (called from `load_unit`, 0x64F320); deleted with virtual dtor on unload | 0x4CDA10, 0x4CC850 | verified |
| 0x288+n | 0x29F+n | 4×5 | u8[4][5] | `challenge_flag[row][idx]` | `set/get_challenge_flag(name, idx[, v])` (0x65A0E0/0x65A1B0): address `0xA5922F + row*5 + idx`, `idx` is **1-based** (1..5), `row` = 0 `"STRENGTH"`, 1 `"DEXTERITY"`, 2 `"LEADERSHIP"`, 3 `"DEFENSE"` (anything else → row 0) | verified |
| 0x29D+4n | 0x2B4+... | 4×5×4 | u32[4][5] | `challenge_timer[row][k]` | `set/get_challenge_restart_duration(name, idx[, ms])` (0x659F40/0x65A010): address `0xA59240 + row*0x14 + idx*4`, idx 1-based → first at `0xA59244 = GameState+0x2B4`. **Counted down by `DAT_00A500BE` each update, clamped at 0, in `scenario_flow_update`** (when `mode != 2`) | 0x471620 | verified |
| 0x2ED | 0x304 | 4 | u32 | `challenge_duration` | `set/get_challenge_duration` (0x65A2A0/0x65A280, `DAT_00A59294`); the **only** timer counted down when `mode == 2` (training) | 0x471620, 0x65A280 | verified |
| 0x2F1 | 0x308 | 1 | u8 | `end_state` | `DAT_00A59298`: **0 running, 1 victory, 2 defeat, 3 left for results** (set by 0x720CA0). `is_game_finish()` = `end_state != 0`; `set_game_finish("WIN"/"LOSE")` writes 1/2. Reset to 0 by every load | 0x6594E0, 0x6595E0, 0x471620, 0x720CA0 | verified |
| 0x2F2 | 0x309 | 2 | u16 | `end_cond_count` | live [EndCondition](EndCondition.md) records (≤ 40) | 0x471E20 | verified |
| 0x2F4 | 0x30B | 40×0x1A | EndCondition[40] | `end_conditions` | 0x410 bytes, up to `GameState+0x71A` | 0x471620, 0x471010 | verified |

Remaining GameState fields written by the battle code (outside the `Scenario` span, `GameState` offsets):

| GS offset | size | name | meaning | evidence |
|---|---|---|---|---|
| 0x00 | | (load-progress counter?) | the `battle::` methods keep a progress int (`*this = 0`, `+= 1`, `= 10`; `0x41C7E0` returns `*this > 9`). Its address is **not visible** (ECX lost in the export) and cannot be `GameState+0` (that would overlap `res_loaded` at +2) — treat as a separate global | 0x41CC80, 0x41C810, 0x41C7E0 |
| 0x07..0x16 | 4×4 | `bits_backup[4]` | `backup_bit_flag` (0x65AB90) copies the four banks of the current stage into `DAT_00A58F97/9B/9F/A3`; `restore_bit_flag` (0x65ACA0) copies them back | 0x65AB90/0x65ACA0 |
| 0x734 | 4 | `gold` | `DAT_00A596C4`: running money; loaded from `profile+0x38` in 0x41C810, increased by end-condition gold awards, written back to `profile+0x38` by 0x720CA0 | 0x41C810, 0x471620, 0x720CA0 |
| 0x738 | 4 | `delay_before_win_ms` | `set_delay_before_win(ms)` (0x65FBA0, `DAT_00A596C8`); default 0. Shipped values: S1 15000, S2 15000, S3 10000, S4 15000, S5 15000, S6(S6L1) 5000, S7 15000, S9L1 15000; **S8L1 never sets it** (0) | script grep |
| 0x73C | 4 | `win_timer` | game ms stamp when "all main goals achieved" first became true (0 = not armed) | 0x471620 |
| 0x740 | 4 | `play_ms` | `DAT_00A596D0`: total play time, loaded from `profile+0x3C`, written back by 0x720CA0 | 0x41C810 |
| 0x744 | 1 | `cut_scene_flag` | "a cut scene owns the game": written by `0x4725F0(on)` — set to 1 by the cut-scene start methods (0x6CB9E0, 0x6CAD50) and cleared to 0 by the cut-scene interface update (0x6CA660) when the end fade finishes; read by `0x4301C0` (= `get_cut_scene_flag` builtin 0x65B130, `is_cut_scene_finished`, the victory check and the `count_time` accumulation). Cleared by `game_load_world_arrays` | 0x4725F0, 0x4301C0, 0x65B130 |

## 3. Lifetime

* `scenario_set_number(n)` (0x4CCF40): runs only if `n == 0 || n != number`: `scenario_free()` (0x4CC850) → `number = n` → `scenario_load_scr()` (0x4CCFB0) → `cur_stage.stage_value = 0` (`this+0x1AE` of the Scenario base) → `msglog_select_scenario(n)` (0x4B3160: clears the message list `n`, remembers it).
  Calling it with the **current** number (n ≠ 0) is a no-op: the stage array, flags and scripts stay.
* `scenario_free()` (0x4CC850) deletes each stage's script object, frees `name`/`description` strings, frees the array, clears the include cache (0x639030), the embedded record (script, strings, memset), the three ref lists, zeroes the statistic fields from `0x1DA` to `0x221`, and calls `0x4CDDC0`.
  **Quirk:** the free of the stage array is inside the per-stage loop (`param_1[4] = 0` after the first free), so it is only correct for `stage_count == 1` (true for all shipped files). A port must free once after the loop.
* `scenario_load_scr()` (0x4CCFB0): builds `"SCENARIO\scenario_" + (n<10 ? "0" : "") + n + ".scr"` (`DAT_009CFCF8` is the `"0"`, `DAT_009D57C0` the extension), silently returns when the file does not exist (`0x590B80`).
  FileTxt token stream: 4 tokens skipped/consumed (`code`, value, `StepNo`, count-value→`atoi`), then per stage 6 × (key token + value token) copied with `strcpy` into the six 0x41-byte fields (**no length check**: names ≥ 65 chars overflow into the next field); then `script`, all four bit banks, `stage_value`, `name`, `description` = 0 and `time_limit = time_left = −1`.

## 4. Open questions

* `TODO(re)`: purpose of the unnamed stat fields `GameState+0x1F5/0x1F9/0x201/0x205/0x20D/0x211/0x225` (cleared with the block; probably kill/loss/item counters of the results screen).
* `TODO(re) 0x4CDDC0`: last call of `scenario_free`.
* `TODO(re)`: address of the battle load-progress counter.
* `DAT_009CF7F4` = ".SCE" (verified); `TODO(re)`: `DAT_009CFCF8` / `DAT_009D57C0` (assumed `"0"`, `".scr"`).
