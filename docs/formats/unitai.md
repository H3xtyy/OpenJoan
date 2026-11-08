# `script/unit/unitAI.res` — per unit-type AI table (18 entries)

`script/unit/unitAI.res`, 39,117 bytes, **18 records**. Container: ResLX bank ([res.md](res.md)): `u32 count = 0x12`, 18 index entries + sentinel
(entry 0 `HUMAN_LIGHT_GENERAL`, payload offset 0x2D6 = 4 + 19*38). Reader: `uscr_load` **0x5462B0** (`uscrres.cpp`); in-memory entry
[../re/structs/UnitScriptRes.md](../re/structs/UnitScriptRes.md) (0x28 bytes); function notes [../re/functions/script_res.md](../re/functions/script_res.md) §3.

## 1. What the engine actually does with this file

1. At start-up `uscr_load` opens the bank (whole file loaded, then closed), and for each entry keeps **only the name and a numeric class**
   (`ai_class`, from a hard-coded case-insensitive name ladder, §3). **The 18 payloads are never read by the reader and are not retained.**
2. Each unit type (`unit.res` record, unitres `+0x125`) refers to one of the 18 entries by 1-based index (0 = no AI). Every AI think (period = unit-type `+0x121`
   ms, first think `now + rand(500)`) calls `uscr_run_native_ai` 0x5467B0, a `switch(ai_class)` into 15 **native C++ AI routines** (0x531F90 ... 0x533B10). Only if that
   returns 0 (class 0, or the routine declined) is the actor's `Script*` (`actor+0x299`) run via Script vtable slot 3 (`run_step`).
3. **In the shipped data the script fallback does nothing**: all 18 payloads consist of `{`, one live statement `exit`, `}` and otherwise `//` comment lines (verified
   by listing every non-comment line of the file: only `{`, `exit`, `}` appear). The commented bodies document the AI that was moved to C++.

For a port: implement the 15 native AI routines (task state machines keyed by the TASK ids in §4); loading/running `.sce`-dialect payloads is needed only for
modded content. Because the payloads are inert, an implementation may read the bank but must not rely on their text for behaviour.

## 2. Payload text format

Each payload is text ending in exactly one NUL byte (that is why `res.md` classifies the bank as "binary"). Layout:

```
"HUMAN_LIGHT_GENERAL"          <- quoted script name (equals the ResLX entry name)
{
	exit                       <- the only live statement in every shipped payload
//	INT my_unit_recno
//	my_unit_recno = FUNC("get_character_recno")
//	...
}
```

The dialect is the `.sce` statement language (see [sce.md](sce.md), [../scripting/LANGUAGE.md](../scripting/LANGUAGE.md), builtin census
[../scripting/BUILTINS.md](../scripting/BUILTINS.md)): `INT name`, assignment, `FUNC("name", args)` expression calls, `ACTION("name", args)` statement calls,
`SCRIPT("path")` include/chain, `if ( cond ) { ... }`, `==`, `&&`, `exit`, `//` line comments. Runtime semantics belong to the Script object
([../re/functions/script_vm.md](../re/functions/script_vm.md) and [../re/functions/video.md](../re/functions/video.md) §8). The builtins named by the commented code:
`get_character_recno`, `set_unit_attack_behaviour(unit, "NEAREST")`, `is_died`, `is_attacked`, `get_curr_task`, `stop_curr_task`, `get_leader_recno`,
`get_unit_horse_recno`, `search_for_unit(unit, "E"|"F", range, 0,0,0)`, `search_for_item`, `get_item_rotation`, `get_distance`, `get_cut_scene_flag`,
`change_animation_special(unit, "CAMP"|"DEFAULT")`, `random_number(n)`, `ACTION give_order(unit, "MOVE_TO_RELATIVE_TARGET", -1, 0, dx, dy, dz, 0, 5000)`,
`ACTION stop_all_order`.

The commented bodies dispatch on `curr_task` to included files `script\unit\<unit>_<task>.txt` (`idle.txt`, `retreat.txt`, `rts_move_to.txt`, `approach_place.txt`,
`training.txt`, `human_light_gen_attack_place.txt`, ...). **None of those files exist in the install** (`script/unit/` contains only `unitAI.res`).

## 3. Entries and `ai_class`

`ai_class` is computed by `uscr_class_from_name` 0x546520 with a case-insensitive compare against these literals (unknown names give 0). Entries in file order:

| # | ResLX name | ai_class | native handler (0x5467B0) | notes |
|---|---|---|---|---|
| 1 | `HUMAN_LIGHT_GENERAL` | 2 | 0x5322C0 | |
| 2 | `PIG` | 0 | – (none) | not in the ladder; the commented body is a wandering/fleeing critter |
| 3 | `HUMAN_ARCHER` | 6 | 0x532C60 | |
| 4 | `HUMAN_LIGHT_INFANTRY` | 1 | 0x531F90 | |
| 5 | `HUMAN_HEAVY_GENERAL` | 5 | 0x532A00 | |
| 6 | `HUMAN_CANNON` | 8 | 0x533170 | |
| 7 | `HUMAN_HORSE` | 12 | 0x5336E0 | literal at 0x9D7E94 (unreadable; the name is inferred from the file) |
| 8 | `ARCHER_GEN` | 7 | 0x532EF0 | |
| 9 | `HUMAN_HEAVY_INFANTRY` | 4 | 0x532780 | |
| 10 | `CAMP_HORSE` | 11 | 0x5335E0 | |
| 11 | `CITIZEN` | 13 | 0x533810 | |
| 12 | `SPECIAL_GENERAL` | 3 | 0x532520 | |
| 13 | `HUMAN_TREBUCHET` | 14 | 0x533920 | |
| 14 | `ARCHER_EX_GEN` | 7 | 0x532EF0 | shares class 7 with `ARCHER_GEN` |
| 15 | `AUTOBOW` | 9 | 0x533370 | |
| 16 | `HUMAN_ARCHER_EX` | 6 | 0x532C60 | shares class 6 with `HUMAN_ARCHER` |
| 17 | `BATTERRAM` | 10 | 0x533440 | the ladder literal is `batterram` |
| 18 | `CART` | 15 | 0x533B10 | literal at 0x9D7E70 (unreadable; inferred) |

Ladder (0x546520), in test order: `human_light_infantry` 1, `human_light_general` 2, `special_general` 3, `human_heavy_infantry` 4, `human_heavy_general` 5,
`human_archer` 6 or `human_archer_ex` 6, `archer_gen` 7 or `archer_ex_gen` 7, `human_cannon` 8, `autobow` 9, `batterram` 10, `camp_horse` 11, <0x9D7E94> 12,
`citizen` 13, `human_trebuchet` 14, <0x9D7E70> 15, else 0.
Name lookup by the unit type is by **index** (unitres `+0x125`), not by name, so adding or reordering entries changes which AI a unit gets.

## 4. TASK ids visible in the commented code

`curr_task` values used in the comments (from `// TASK_*` annotations): 0 none, 1 ATTACK_PLACE, 3 RECRUIT_TROOP, 4 HELP_TROOP, 5 FLEE, 6 DEFEND_PLACE, 7 IDLE,
10 FOLLOW_UNIT, 11 RETREAT, 12 RTS_MOVE_TO, 13 RTS_ATTACK_UNIT, 14 APPROACH_PLACE, 17 RTS_ATTACK_ITEM, 18 DEFEND_UNIT, 101 (TASK_SPECIAL + 1) training.
These ids are shared with the native handlers and with the `give_order`/task builtins (TODO(re): task enum in the game-logic pages).

## 5. Worked example

`HUMAN_LIGHT_GENERAL` payload (entry 1, offset 0x2D6, first lines):

```
"HUMAN_LIGHT_GENERAL"
{
	exit
//	INT my_unit_recno
//	my_unit_recno = FUNC("get_character_recno")
//	INT curr_task
//	FUNC("set_unit_attack_behaviour", my_unit_recno, "NEAREST" )
//	if ( FUNC("is_died", my_unit_recno) ) { exit }
...
```
Parsed by the Script object it is a script that exits immediately. Read by `uscr_load` it yields `{ recno 1, name "HUMAN_LIGHT_GENERAL", script 0, ai_class 2 }`.

## 6. Open points

- CLOSED: 0x546970 is a per-class native AI init/reset switch (classes 1-10, 14; others return 1), not a script loader. No code path parses the payload text, so the `{ exit }` bodies and the quoted-name header are never interpreted; `UnitScriptRes.script` stays 0.
- TODO(re): contents of the 15 native routines 0x531F90..0x533B10 (the real AI).
- TODO(re): unitres `+0x121`/`+0x125` field names and which `unit.res` key feeds them.
