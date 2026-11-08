# GameState (`game.cpp` singleton "Game") — joan.exe

Global object at **0xA58F90** (verified: `DAT_00A58FA7` = `+0x17` = current scenario number, `DAT_00A59298` = `+0x308`, `DAT_00A58F96` = `+6`). Packed, unaligned
`i32` fields. Size >= 0x745. It is **not** the CONFIG.DAT image ([GameConfig](GameConfig.md) lives at 0xA5801E) and not the window object ([GameApp](GameApp.md)).
Functions: [gameloop.md §2](../functions/gameloop.md). Lifetime: `game_reset` (0x46F200) <-> first use in `game_load_resources` (0x46F3A0).

| offset | size | type | name | meaning | evidence / status |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `active` | `game_reset` runs only when set; cleared at its end | 0x46F200: verified |
| 0x02 | 1 | u8 | `res_loaded` | all `*_res` tables + interface screens loaded (0x46F3A0 sets, 0x470630 clears) | verified |
| 0x04 | 1 | u8 | `world_loaded` | per-scenario arrays loaded (0x470D40 sets, 0x471010 clears) | verified |
| 0x05 | 1 | u8 | `flag05` | set to 1 by the debug/console load path (0x52F930 case 0x74, `DAT_00A58F95`) | read |
| 0x06 | 1 | u8 | `deferred_start` | `DAT_00A58F96`: when set while `state==9`, `0x4D48F0` clears it and calls `0x472D10(scenario)` at the end of the frame | 0x4D48F0: read |
| 0x17 | 4 | i32 | `scenario` | current scenario number (0..6). `==6`: `game_load_world_arrays` skips the array load; `==5`: special "from save" unload path (0x471010) and 0x41C780 skips 0x5624C0 | 0x470D40, 0x471010, 0x41C780: verified |
| 0x1B | 4 | i32 | `stage_index` | `DAT_00A58FAB`, 1-based current stage/mission within the scenario; second argument of `battle::0x41CC80(scenario, stage, …)`; indexes the array at `+0x27` (stride **0x1B2**, object pointer at `+0x186` with a virtual `[+0xC]` called by 0x471620) | 0x471620: read |
| 0x1F | 4 | i32 | `target_stage` | `DAT_00A58FAF`; 0x471620 acts when `stage_index == target_stage` (or scenario==0) | read |
| 0x27 | 4 | StageArr* | `stages` | see above | read |
| 0x1F1 | 4 | u32 | `mission_ms` | `DAT_00A59181`, += dt each update while a world is loaded | 0x4E5FC0: read |
| 0x2B4 | 4x5x4 | u32[4][5] | `timers` | countdown timers (ms), decremented by `DAT_00A500BE` (clamped at 0) when `0x4CD510() != 2`; else only `+0x304` runs | 0x471620: read |
| 0x304 | 4 | u32 | `timer304` | | read |
| 0x740 | 4 | u32 | `play_ms` | `DAT_00A596D0`, total play time ms | 0x4E5FC0: read |
| 0xAD | 0x41 | char[] | `scenario_name` | `DAT_00A5903D`, copy of the scenario name passed to 0x41DFE0 | read |
| 0xEE | 0x41 | char[] | `scenario_des_name` | `DAT_00A5907E`: name + literal `_des` (0x9CF820) | read |
| 0x1D1 / 0x1D5 | 4+4 | i32 | `ids` | both set to -1 by 0x41DFE0 | read |
| 0x29A | 4 | void** | `ref_list` | array of owned objects freed with `0x471530(1)` (virtual dtor) on the "from save" unload; count in `+0x29E` | 0x471010: read |
| 0x29E | 2 | i16 | `ref_count` | | read |
| 0x2A1 | 4 | i32 | `u2a1` | cleared when a savegame body is read (0x4C9930) — field of the same offset in the save record, not this object | note |
| 0x308 | 1 | u8 | `b308` | cleared on world load/unload (also `DAT_00A59298`) | verified |
| 0x309 | 2 | u16 | `end_cond_count?` | cleared after clearing the end-condition table | 0x471010, 0x470D40: read |
| 0x30B | 40 x 0x1A | EndCond[40] | `end_conditions` | 40 records of 0x1A bytes (`end_con.cpp`, cleared by 0x467260); in the "from save" unload, record k is released (0x471F90) when `i16@+0x31A+k*0x1A != 0 && i16@+0x314+k*0x1A != 0 && u8@+0x30B+k*0x1A != 3` | 0x471010: read |
| 0x71B | 1 | u8 | `b71b` | cleared by 0x470D40 | read |
| 0x71C | 4 | u16* | `list_a` | growable u16 set: ids de-duplicated (0x472620), cap += 10; mirrored to/from the current profile (0x4727B0/0x4728F0) | verified |
| 0x720 | 4 | i32 | `list_a_count` | | verified |
| 0x724 | 4 | i32 | `list_a_cap` | | verified |
| 0x728 | 4 | u16* | `list_b` | growable u16 list, duplicates allowed (0x4729C0); mirrored to profile `+0x30/+0x34` | verified |
| 0x72C | 4 | i32 | `list_b_count` | | verified |
| 0x730 | 4 | i32 | `list_b_cap` | | verified |
| 0x738 / 0x73C | 4+4 | i32 | `i738`, `i73c` | zeroed by 0x470D40 | read |
| 0x744 | 1 | u8 | `b744` | zeroed by 0x470D40 | read |

`list_a` / `list_b` are persisted in each profile (see [ProfileArray](ProfileArray.md)); meaning (unlocked/seen/completed ids) `TODO(re)`.
The tracked allocator calls pass `d:\ww\game.cpp` with a line base held in `DAT_009FED60..7C` (`+k` = relative source line), which is how `game.cpp` was attributed.
