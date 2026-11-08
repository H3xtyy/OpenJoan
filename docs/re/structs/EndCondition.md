# EndCondition — one scenario objective record (`end_con.cpp`) — joan.exe

Table of **40** records of **0x1A (26) bytes** inside the GameState object: `GameState+0x30B + i*0x1A` (= `0xA5929B + i*0x1A`, `Scenario+0x2F4`),
live count `u16` at `GameState+0x309` (`Scenario+0x2F2`). Capacity check `count < 0x28` in `end_condition_add` 0x471E20. Records are 1-based for scripts
(`add_end_condition` returns the new count; every `*_end_condition_*(n, ...)` handler addresses `base + (n-1)*0x1A`; **no bounds check** in the handlers).
Owner struct: [Scenario](Scenario.md). Functions: [functions/battle.md](../functions/battle.md) §5.
Source file: `d:\ww\end_con.cpp` (allocation strings in 0x467260/0x467300/0x467390).

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `type` | 0 = **win** (main goal), 1 = **lose**, 2 = **side quest**, 3 = **imperative** (hidden/mandatory, auto-completed at victory). Script comments: `0 for win / 1 for lose / 2 for side quest / 3 for imperative` (S1L1_des.sce, S3L1_des.sce, S5L1_des.sce) | 0x65A390 writes it; read by 0x471620 (compares with 0, 1, 2, 3) | verified |
| 0x01 | 4 | char* | `description` | heap copy of the (already localised, ID-prefix stripped by the VM) text; `NULL` until `set_end_condition_des`. **A record with `description == NULL` is ignored by every evaluation loop** (awards, win check, lose check) | 0x467300 (realloc `strlen+1`, source `end_con.cpp`), 0x471620 | verified |
| 0x05 | 4 | char* | `territory_link` | heap copy of a territory/place name (e.g. `"Orléans"`) used by the HUD to put a marker on the map; only freed/copied here | 0x467390, 0x467260 | read (consumer not exported) |
| 0x09 | 2 | i16 | `finish` | status: **0 = pending** (`"NORMAL"`), **1 = achieved** (`"WIN"`), **2 = failed** (`"LOSE"`). Written by `set_end_condition_finish(n, "WIN"/"LOSE"/"NORMAL")`, read by `get_end_condition_finish(n)` | 0x65A4A0, 0x65A670; string literals 0x9DB8A0 ("WIN") and 0x9DB898 ("LOSE") deduced from script usage + comparison order | verified |
| 0x0B | 2 | i16 | `para1` | free parameter (stored by `set_end_condition_para1`; S5L1_des.sce sets 1 / 2). Consumer not found in the exported code (HUD) | 0x65A6A0 | read |
| 0x0D | 2 | i16 | `para2` | free parameter (`set_end_condition_para2`) | 0x65A6E0 | read |
| 0x0F | 2 | i16 | `enabled` | 0 = disabled (not shown in the objective list, ignored by the win/lose checks), non-zero = active. **`add_end_condition` initialises it to 1** (the record is otherwise zeroed). Changing it to/from 0 calls the objective-menu add/remove hooks | 0x471E20 (`+0x31A = 1`), 0x65A720, 0x65A7E0 | verified |
| 0x11 | 4 | i32 | `gold_award` | gold added to `GameState+0x734` (the running money mirror of the profile) when the record is first seen achieved | 0x65A810 writes; 0x471620 reads | verified |
| 0x15 | 4 | i32 | `exp_award` | experience given to each qualifying hero (see battle.md §6.3) | 0x65A840; 0x471620 | verified |
| 0x19 | 1 | u8 | `awarded` | set to 1 the first time the award block ran for this record (guards against double award). Not reset by `set_end_condition_finish("NORMAL")` | 0x471620 | verified |

`sizeof = 0x1A` (consistent with `stride 0x1A` in 0x471620 and 0x65A390/0x65A4A0/0x65A720; `memset(..., 0, 0x1A)` in 0x471E20).

## Operations

| function | address | behaviour |
|---|---|---|
| `end_condition_add` | 0x471E20 (via builtin 0x65A350) | if `count < 0x28`: zero the 26 bytes at index `count`, set `enabled = 1`, `count++`, call objective-menu hook `0x7078C0(count-1)`, return the new count (1-based id); else return 0 |
| `end_condition_del_last` | 0x471EF0 (via 0x65A370) | if `count > 0`: objective-menu remove hook `0x7072C0(count-1)`, free the two strings of the record (the implicit `this` of `0x467260` is not visible in the export; almost certainly the record being removed, i.e. index `count-1`), `count--`; returns the new count. The record bytes are **not** zeroed (only re-zeroed by the next `end_condition_add`) |
| `end_condition_release` | 0x471F90 | used when unloading a savegame world: remove-hook `0x7072C0(idx)` + free strings; does not change `count` |
| `EndCond::free_strings` | 0x467260 | frees `description` and `territory_link` (tracked free, `end_con.cpp`) and nulls them |
| `EndCond::set_description` | 0x467300 | `if (text != NULL)`: realloc to `strlen+1`, copy. (Setting a NULL text leaves the old one.) The unload code (0x471010) calls 0x467260 on all 40 records |
| `EndCond::set_territory_link` | 0x467390 | same for the territory link |

## Builtins that touch it (handler → field)

`add_end_condition` 0x65A350, `del_end_condition` 0x65A370, `set_end_condition_type` 0x65A390 (+0x00), `set_end_condition_des` 0x65A3C0 (→ 0x467300),
`set_end_condition_des_2` 0x65A400 (sound/notification event `"OBJECTIVE_UPDATE"` via 0x4CEEF0, then 0x467300, then objective-menu update hook 0x7076C0(idx-1)),
`set_end_condition_territory_link` 0x65A460, `set_end_condition_finish` 0x65A4A0 / `get_end_condition_finish` 0x65A670 (+0x09), `set_end_condition_para1` 0x65A6A0 (+0x0B),
`set_end_condition_para2` 0x65A6E0 (+0x0D), `set_end_condition_enabled` 0x65A720 / `get_..._enabled` 0x65A7E0 (+0x0F), `set_end_condition_gold_award` 0x65A810 (+0x11),
`set_end_condition_experience_award` 0x65A840 (+0x15). Details in functions/battle.md §9.

`set_end_condition_finish` has an **idempotence guard**: if the requested state already equals the stored one nothing happens (no event, no hook). Otherwise it stores the
new state and fires: `"WIN"` → sound event `"OBJECTIVE_FINISH"` (0x4CEEF0) + objective-menu hook `0x707960(idx-1)`; `"LOSE"` → hook `0x707B20(idx-1)` (no sound event);
`"NORMAL"` → hook `0x707400(idx-1)` (re-add as pending). Unknown strings are ignored.
`set_end_condition_enabled(n, v)`: if the old value is 0 and `v != 0` → event `"OBJECTIVE_ADD"`; if the value changes: `v == 0` → hook `0x7072C0` (remove from list), else `0x707400` (add/show).

## Objective-menu hooks (HUD, not part of the rules)

The objective list object (`0x7078C0/0x7072C0/0x7076C0/0x707960/0x707B20/0x707400`, in the in-game menu, `TODO(re)` belongs to the hud family) keeps up to N entries
`{ id = idx+1 (u32 array +0x299), text length (+0x157 u16), state (+0x1A7 u16: 0 achieved, 1 failed, 2 new/updated), timestamp (+0x1F9 u32 = game ms `DAT_00A500BA`) }` and a
pop-up request word (+0x14F). Pop-up text prefixes come from string getters 0x4DCAD0 (achieved) / 0x4DCB00 (failed) / 0x4DCB30 (new/updated). In scenario 6 while the scenario's
"stage value" is 1 the "new objective" text is suppressed (0x7076C0, 0x707400: `if (scenario != 6 || stage_value != 1)`).

## Open questions

* `TODO(re) 0x471EF0`: confirm (dynamic or Ghidra register view) which record's strings `0x467260` frees in `end_condition_del_last` (implicit ECX not visible; assumed `count-1`).
* `TODO(re)`: readers of `para1`/`para2` and `territory_link` (HUD/minimap).
* 0x9DB8A0 = "WIN", 0x9DB898 = "LOSE" confirmed from the executable (also matches scripts (`S1L1_finish.sce`: `set_end_condition_finish(1, "WIN")`, `(3, "LOSE")`) and from `get_end_condition_finish(...) == 1 / 2` in the same scripts.
