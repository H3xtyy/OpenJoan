# savegame family (gamefile.cpp, gamefila.cpp, gamefil2.cpp, gfile3.cpp, profile.cpp, sdateres.cpp)

Reference build: `joan.exe`. Export: `$JOAN_WORK_DIR/decomp/game/savegame/` (255 functions, depth <= 2). Confidence: V = read in >= 2 consistent places or confirmed on the sample
`Misja1-Autozapis.SAV` / `Player.dat`; R = read once; I = inferred. Ghidra hides `this` (ECX): in this family the **File** object is *always* the hidden ECX of the `FUN_005873A0/5872E0/…` calls and the
`File*` argument of the section functions is their only visible stack argument; the object being (de)serialised is `this` of the section method and is not visible in the export — its identity is deduced from sizes/offsets
and the sibling pages. Formats and layouts: [formats/sav.md](../../formats/sav.md), [SaveFile](../structs/SaveFile.md), [Profile](../structs/Profile.md), [ProfileArray](../structs/ProfileArray.md).
Design/port notes: game/savegame.md.

## 1. Drivers (gamefile.cpp / gamefila.cpp / gamefil2.cpp)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x477E80 | `savefile_load` | full load of `entry.path` (`entry+3`): `File f; f.open(path, report=0, packed=1)`; `savefile_read_header(f)`; `savefile_read_sections(f)`; close. Failure messages "Error opening file." / "Error loading file." are copied into `SaveList.last_error` (only when `DAT_00A54B0A == 0`) | strings, call order | V |
| 0x478180 | `savefile_save` | `stamp_header()` (0x478480); `f.create(entry+3, 0, 1)`; `savefile_write_header`; `savefile_write_sections`; close. Messages "Error creating file." / "Error saving file." | strings, call order | V |
| 0x477D80 | `savefile_check_header` | `savefile_read_header(f)`; returns 0 when it fails **or** when `entry.flags & 0x80` | code | V |
| 0x478520 | `savefile_read_header` | 10 `R()` calls (magic `0x76615352 "RSav"` else "Invalid file format."), fills entry fields `+2,+0x108,+0x110,+0x118,+0x119,+0x19F,+0x1A3,+0x11A(0x80),+0x19A,+0x19E` | code; sample | V |
| 0x478CC0 | `savefile_write_header` | mirror writer; `+2` is written with size 4 | code; sample | V |
| 0x478480 | `savefile_stamp_header` | `GetSystemTimeAsFileTime(+0x108)`, `FileTimeToLocalFileTime(+0x108 -> +0x110)`, `+0x118 = DAT_00A500C2`, `+0x119 = DAT_00A500C3`, `+0x19F = DAT_00A47BB0 (calendar serial)`, `+0x1A3 = DAT_00A47BB4 (calendar minute)`, `+2 = 0` | code | V |
| 0x4735C0 | `savefile_read_sections` | reads, in order: 4-byte marker + S1…S20 (table below), each section followed by its marker read; returns 0 on first failure | code | V |
| 0x474EC0 | `savefile_write_sections` | `__chkstk`; writes marker (`W(&uninit_dword, 4)`) before every section and one after the last | code; sample trailer | V |
| 0x477440 | `savelist_add_entry(File*)` | `tracked_malloc(0x1A7, "d:\ww\gamefila.cpp")`, zero, if `File*` is null → entry is valid (0x477D60 returns 1) else `strcpy(entry+3, file.path)` + `savefile_check_header`; pushes on the list (0x584620), sets `entry.id = list cursor`; returns `±id` (negative = bad file) | code | V |
| 0x477520 | `savelist_remove_entry(id)` | get (0x430410), destroy (0x477E60 no-op + 0x4775A0), tracked free, `dynarr::remove` (0x584A10) | code | V |
| 0x4775A0 | `savelist_entry_dtor` | calls 0x477D30 (→ 0x477E60 no-op); `delete` if bit 0 | code | R |
| 0x4775F0 | `savelist_set_error(msg)` | `strncpy(this+0x122, msg, 0x104); this[0x226] = 0` | code | V |
| 0x477CF0 | `savelist_entry_zero` | `memset(entry, 0, 0x1A7)` | code | V |
| 0x477D60 | `savelist_entry_always_ok` | `return 1` (used when no File is given) | code | V |
| 0x477E60 | `nop` | empty | code | R |
| 0x4770A0 | `savelist_scan(dir, nav)` | (outside this export; from other families' export) `nav == 0`: clear list (0x477030), enumerate `dir + "*.SAV"` (0x582AD0/0x582B00), for each open + `savelist_add_entry`, drop bad ones, sort (0x477B10 with comparator `LAB_004024C3`), store `dir` at `this+0x1D`; `nav != 0`: handle `".\"` / `"..\"` directory navigation then recurse | strings `*.SAV`, `..\`, `.\` | R |
| 0x477770 | `savelist_make_name(buf, is_auto)` | manual: `"SAVE\%s%%04d.%s"` probe 0..9999 until the name is free; auto: ring of 15 names `_AUTO%03d.SAV`, first unused else the entry with the oldest `time_local` (`CompareFileTime`) | strings | V |
| 0x477640 | `savelist_find(path)` | descending scan, case-insensitive compare with `entry.path` (full), else with the basename after the last `\` (only when `path` has no `\`) | code | V |
| 0x477B10 | `savelist_sort_and_renumber(cmp)` | sort (0x5850B0), then `entry.id = index` for all | code | R |
| 0x65BED0 | `script_fn_save_game` / `savegame_write(name, title, autosave)` | **builtin 315 `save_game` handler** (BUILTINS.md): `path = name + DAT_009CFB78 (".SAV")`; `entry = savelist_find(path)`; if none: new entry via `savelist_add_entry(0)`, `entry.path = "SAVE\" + path`, `entry.title = title`; `entry.scenario = DAT_00A58FA7`; `entry.autosave = arg3`; `savefile_save`; `0x4731E0(DAT_00A48CCC)` (UI refresh; **also writes the `.BMP` thumbnail, decompilation failed**); `0x477B10`; returns 1 | code | V |
| 0x472D10 | `autosave_request(scenario)` | `0x65BED0(txr "AUTOSAVE|1" (0x4DD0E0), txr "AUTOSAVE|2" (0x4DD120), 1)`; txr strings `Misja%1d-Autozapis` / `Misja %1d Autozapis` formatted with `scenario` | lights.md, code | V |

### Callers of the save/load drivers (outside the export, read for context)

| caller | role |
|---|---|
| `battle::fn_0041EA60` | **load-game flow**: reset (0x42DE70, 0x41C780 world unload, 0x470D40 world arrays) → `savefile_load` (0x477E80) → open `SCRIPT\SCENARIO\<script_file>.sce` and `<desc_file>.sce` (script objects `new 0x70`), restore HUD/camera/lights/scene, `GameState` progress codes 5, 8, 0xD, 0xF, 0x14, 0x19, 0x1A, 0x1C, 100 written to `_DAT_00A50259` for the load bar, ends with game mode 10 / `DAT_00A5916D = DAT_00A500BA` |
| `FUN_0041C810` (mission start) | at its end sets `GameState.deferred_start = 1` (`DAT_00A58F96`) — the trigger of the mission-start autosave |
| `FUN_004D48F0` (per-frame draw, state 9) | `if (DAT_00A58F96 && screen_mode == 9) { DAT_00A58F96 = 0; autosave_request(DAT_00A58FA7); }` (lights.md step 15) |
| `FUN_00720CA0` (in-game menu), `FUN_006B7D50`/`FUN_006F3C90`/`FUN_006F3730`/`FUN_0071F690`/`FUN_006B7160` | restart/continue/results screens; for scenario 6 they look up the autosave entry `AUTOSAVE|1(6)+".SAV"` (0x477640) and load it (`FUN_007563F0`) |
| `.sce` `FUNC("save_game", name, title)` | `S1L1_tactic.sce` line 935 (the only shipped call): after the messenger cut scene `FUNC("save_game","Misja 1-Autozapis","Misja 1-Autozapis")` |

## 2. Body sections (gamefil2.cpp → gfile3.cpp). Writer / reader pairs

The writer of section k is called between two marker writes; ECX (the serialised object) is not visible. "This" below is deduced. Grammar of each section: formats/sav.md §5.

| # | writer | reader | proposed names | object / content | conf |
|---|---|---|---|---|---|
| 1 | 0x476890 | 0x4767C0 | `calendar_write_a` / `calendar_read_a` | Calendar `W(0x50)`; writer first does `+0x40 += time(0) - +0x3C; +0x3C = time(0)`; reader sets `+0x3C = time(0)` (0x7B33BF = `time`) | V (Calendar.md size 0x50, `+0x3C time`; sample) |
| 2 | 0x476C40 | 0x476990 | `calendar_write_b` / `calendar_read_b` | Calendar `+0x44(4) +0x48(2) +0x4A(1) +0x32(4)`; reader forces `+0x4A = 1` | V (sample) |
| 3 | 0x4854C0 | 0x4855B0 | `world_array_write` / `world_array_read` | World array; per world `W(0x92)`, `W(0xD5)`, `W(0xAC)`×blocks; element helpers 0x485070 (write), `gfile3::fn_00485130` (read), 0x484AD0/0x484E00 (writers, not exported), 0x484B10 (read 0xD5), `gfile3::fn_00484E40` (read block 0xAC) | V (sample, World.md) |
| 4 | 0x484780 | `gfile3::fn_004848A0` | `gamestate_write` / `gamestate_read` | `W(0x745)` GameState + Scenario block (0x483E00 / `fn_00483F10`) + stages (0x483B90† / `fn_00483CA0`) + hero saves + end conditions (0x4841D0 / `fn_004842C0`) + two u16 lists (`WA`/`RA`) | V (sample to offset 4319) |
| 5 | 0x483B10 | 0x483B50 | `global4_write` / `global4_read` | `W(4)` of one global dword | I (unknown global) |
| 6 | 0x483130 | 0x4831C0 | `inline52_list_write` / `_read` | dense list of inline 0x52-byte elements (0x4830B0 write, 0x4830F0 read) | R |
| 7 | 0x47FA50 | 0x47FB30 | `nation_array_write` / `_read` | `W(0x1545)` Nation + `20×W(0xF4)` + 3 u16 rosters + IndexTail; element helpers 0x47F660 (write), `gfile3::fn_0047F800` (read), 0x47F5E0/0x47F620 (rel block) | V (Nation.md size) |
| 8 | 0x47FE80 | 0x47FF50 | `place_array_write` / `_read` | `W(0x1F6)` Place + scene-recno; 0x47FC80 (write), 0x47FD00 (read, post-fix 0x47FDF0/0x4366F0/0x42B350/0x44F5C0) | V (Place.md size, +0x1DE scene) |
| 9 | 0x480C00 | 0x480CE0 | `facility_array_write` / `_read` | typed by `u16 type` at `+4`; virtual `save(File)` `[+0x54]` / `load(File)` `[+0x58]`; classes 0x140/0x8E/0x99/0x2F14/0x79 | R |
| 10 | 0x481FB0 | `gfile3::fn_004820D0` | `item_array_write` / `_read` | typed by `u16 kind` at `+0x4E`; virtual `[+0x70]` / `[+0x74]`; classes 0xDB,0xCE,0xCE,0xEF,0xCD,0x107,0xCE,0xC2,0xC2; placeholders for non-persistable items | R |
| 11 | 0x482B50 | 0x482C60 | `item_ref_array_write` / `_read` | `U16 present, U16 item recno` | I |
| 12 | 0x482EA0 | 0x482F70 | `rec6c_array_write` / `_read` | `W(0x6C)` records (0x482DC0 write, 0x482E00 read + 0x482E50 post) | I (throw_item) |
| 13 | 0x47D7A0 | 0x47D8F0 | `unit_array_write` / `_read` | Unit manager scalars + Unit records (0x47DBE0 write, `gfile3::fn_0047DE30` read, post 0x47DAC0) | V |
| 14 | 0x4838E0 | 0x4839C0 | `profile_array_array_write` / `_read` | `U32 count, U16 +0x2E`, ProfileArray objects (0x483250 write, `gfile3::fn_00483430` read) | R |
| 15 | 0x4857C0 | 0x485930 | `camman_write` / `_read` | `W(0x126E)` + `W(0x28)` mode block; post 0x485990 | V (camera.md `DAT_00A48CEF`) |
| 16 | 0x485AB0 | 0x485BF0 | `lightsys_write` / `_read` | 8×`W(4)`, `W(0x18C)`, 2×`W(0x194)` | V (Calendar.md §2.2 offsets +0x58C…) |
| 17 | 0x4843D0 | 0x4845A0 | `hud_objective_write` / `_read` | 9 blocks (+0x14D…+0x299) | V (HudScreen.md/EndCondition.md) |
| 18 | 0x485D30 | 0x485DC0 | `territory_array_write` / `_read` | dense `W(0xE0)` pointer elements; reader clears with 0x4E49B0 | V (WorldArrays.md free fn) |
| 19 | 0x485E70 | 0x485F50 | `trigger_system_write` / `_read` | `W(1)@+0x1D` + dense `W(0x43)` (0x486090 write, 0x4860D0 read); reader clears with 0x4EEE10 | V (WorldArrays.md free fn) |
| 20 | 0x486110 | 0x486280 | `msglog_write` / `_read` | MsgLog scalars + MsgRecord (0x486420 write, `gfile3::fn_004864B0` read) | V (MsgLog.md) |

† = writer not exported.

### Record helpers

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x47DBE0 | `unit_write` | `W(0x432)`; sub-objects/lists in the order of the reader (see formats/sav.md §4.2) | V |
| 0x47DE30 | `unit_read` | mirror reader; post-fix 0x47E300 (runtime resets), 0x509250 (nation bookkeeping: `unit+2 = nation`, adds/removes `upkeep = 0x523320 × 0x527DD0(type)` to the nation, ±1 on nation `+0x10`) | V |
| 0x47E300 | `unit_reset_runtime` | clears `+0x22,+0x26`, `+0x299 = unit_res[+0x125 type]…+0x22`, runtime bytes/ints `+0x2ED…+0x357` | R |
| 0x47E580/0x47E600/0x47E700/0x47E9F0/0x47EDA0/0x47EF30/0x47F130/0x47F290/0x47F620 | `unit_read_*` | readers of the unit sub-objects (0x58, 0x3B, slot list, 0x1E object, 0x12 leaf, 0x3E matrix, 0x21 name list, 0x24 table, 0xF4 relation) | V |
| 0x4C9930 | `profile_snapshot_read` | profile variant of `unit_read` (no 0x33/0x2A1/per-type arrays) | V |
| 0x4C9530 | `profile_read_body` | `R(0x20)` name, snapshot `current`, `i16 slot_count`, `slot_count` snapshots | V |
| 0x4C9420 | `profile_read_name` | `R(0x20)` | V |
| 0x484E40 / 0x484F50 | `worldblock_read` / `worldblock_reset_ptrs` | `R(0xAC)`, keep `block[0]` (back pointer), allocate `[0x1A]×4`, `[0x1A]×0x21`, `[0x1F]×0x74`, zero the pointer fields | V |
| 0x485130 / 0x485450 | `world_read` / `world_reset_ptrs` | see section 3 above | V |
| 0x483F10, 0x483CA0, 0x4842C0, 0x483DD0, 0x484160 | `scenario_read`, `stage_read`, `endcond_read`, `scenario_reset_script`, `scenario_reset_refs` | see S4 grammar | V |
| 0x4864B0 / 0x4865C0 | `msgrecord_read` / `msgrecord_reset_ptrs` | `R(0x1F)`, then `text` (len `+0x13`, +1) and `extra` (`+0x15`) | V |
| 0x482C60 | `item_ref_array_read` | see S11 | R |
| 0x485990 | `camman_reset_runtime` | | R |

## 3. profile.cpp / sdateres.cpp

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x4C7E70 | `profile_array_free` | for each profile: delete `current` and the 20 slots (virtual dtor 0x471530(1)), free the record array (tracked, `profile.cpp`), free both u16 lists; zero `+0x28,+0x30,+0x40` | V |
| 0x4C80C0 | `profile_array_add(name)` | grow `records` (0x58EB30 realloc), `memset` the new 0x77-byte record, `strncpy(name, 0x21)`, returns it | V |
| 0x4C8490 | `profile_array_load_header` | `Player.dat` header only (names) | V |
| 0x4C8BB0 | `profile_array_load` | full `Player.dat` load; path `"SAVE\" + head + DAT_009D56F0`; `590B80` exists-check; `open(path, 1, 0)` (**not packed**); reads as described in [Profile](../structs/Profile.md) §3 | V (sample) |
| 0x4C8810 | `profile_array_save` | **not exported**; called by 0x720CA0 after copying `GameState.gold/play_ms` into the profile | I |
| 0x4CE0F0 | `sdate_load` | `open_table("SDATE")`, `count = u16`, array `count × 0x33`: name (rtrim 0x20), `YEAR int(4)`, `MONTH int(2)`, `DAY int(2)`, `WEEKDAY int(1)`, `recno = i+1` at `+0x31`. See resdb.md/gameset.md. **Not a save-game structure**: sdateres only supplies the scenario start dates | V |
| 0x4CE070 | `sdate_free` | frees the array if `loaded` | V |

## 4. File primitives (ilib file.cpp) as used here

| address | name | behaviour |
|---|---|---|
| 0x4179C0 | `File::File()` | `handle = -1` |
| 0x586DA0 | `File::open(path, report, packed)` | `strcpy(this, path)`; `+0x109 = report`; `+0x10D = packed`; `CreateFileA(install_dir + path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING)`; error box "Error opening file %s." if failed (loop ends after the first attempt) |
| 0x586F00 | `File::create(path, report, packed)` | `CreateFileA(path (no install prefix!), GENERIC_WRITE, 0, CREATE_ALWAYS)`; "Error creating file %s" |
| 0x587260 / 0x5872B0 | `close` / `~File` | `CloseHandle`, `handle = -1` |
| 0x5873A0 | `read(dst, n)` | packed: tolerant sized read (see formats/sav.md §1); unpacked: `ReadFile`; zero-fills short reads |
| 0x5872E0 | `write(src, n)` | packed: `u16 (n < 0x10000 ? n : 0)` then `WriteFile` |
| 0x587510 / 0x587650 | `write_array` / `read_array(dst, count, elem)` | see formats/sav.md §1 |
| 0x587BA0 / 0x587DA0 | `write_u16` / `write_u32` | raw, never prefixed |
| 0x587C20 / 0x587E20 | `read_u16` / `read_u32` | raw; on failure return 0 |
| 0x587CA0 / 0x587D20 | `write_len_prefix` / `read_len_prefix` | the u16 prefix of the sized primitives |
| 0x587EA0 / 0x587EF0 | `seek(off, whence)` / `tell` | `SetFilePointer` (`whence -1` ⇒ 0 = begin; 1 = current) |
| 0x586BC0 / 0x586CB0 / 0x5A8BE0 | `fatal_error(fmt, …)` / `message_box_retry` | caption `"Wars & Warriors: Joan of Arc"` |
| 0x58E7B0 / 0x58ED70 / 0x58EB30 | tracked malloc / free / realloc | `(size, file, line)`; file strings `d:\ww\gamefila.cpp`, `gfile3.cpp`, `profile.cpp`, `sdateres.cpp` identify the module |
| 0x584510, 0x584620, 0x584A10, 0x584BD0, 0x5857A0, 0x585B40, 0x585BB0, 0x5858C0, 0x585D20, 0x41B3F0, 0x417200, 0x417520, 0x417AD0 | dynarr / dynarrb | `grow`, `push(src)` (copy elem), `remove`, `push_empty(n)`, `dynarrb::push` (reuses last freed index), `write_free_list`, `read_free_list`, `dynarrb::remove(i)`, `reset`, `get(i)` (pointer element), `addr(i)` (inline element), `cursor (+8)`, `count (+0xC)` — layout: `+0 cap, +4 grow, +8 cursor, +0xC count, +0x10 elem, +0x19 data, +0x1D free-list table, +0x21 free cap, +0x25 free n` |

## 5. Constants

| value | meaning | where |
|---|---|---|
| `0x76615352` | magic "RSav" | 0x478520 / 0x478CC0 |
| 189 | header size (see sav.md §3) | derived |
| 0x1A7 | `SaveFile` entry size | 0x477440, 0x477CF0 |
| 0x80 | display-name field (128 bytes) | 0x478520 |
| 15 | autosave ring size `_AUTO%03d.SAV` (`n = 0..14`) | 0x477770 |
| 10000 | max manual file index `%04d` | 0x477770 |
| 20 | slots per profile; 0x77 profile record; 0x21 name | 0x4C7E70, 0x4C80C0 |
| 0x4B4 / 0x432 / 0x3B / 0x58 / 0x1E / 0x3E | Unit record / raw prefix / second block / sub-objects | 0x4C9530, 0x47DE30, 0x47E9F0 |
| 0x745, 0x289, 0x1B2, 0x1A, 40 | GameState image, Scenario block, stage, end-condition record, max end conditions | 0x484780, 0x483F10, 0x483CA0 |
| 0x1545, 0xF4×20, 0x1F6, 0xE0, 0x43, 0x6C, 0x52 | Nation, relation, Place, Territory, trigger, S12 record, S6 record | sections 7, 8, 18, 19, 12, 6 |
| 0x126E, 0x28 | ControlMgr (0xA5B9E8) image, mode block at 0xA5BA34 | 0x4857C0 |
| 512×256×24 | thumbnail size | sample |
| `AUTOSAVE` keys 1 and 2 | txr strings of the autosave file name (`Misja%1d-Autozapis`) and title (`Misja %1d Autozapis`) | 0x4DD0E0/0x4DD120, `Resource/t_interf.txr` rows 80/81 |

No `rand()`/RNG call sites exist in this family (none of the 255 exported functions lists 0x590540, 0x5904E0 or a CRT `rand` among its callees). Section 5 may be the RNG state word (see open questions) — that would be the only place where determinism state is persisted.

## 6. Behaviour notes (quirks worth keeping in mind)
* The markers between sections are `uninitialised stack` bytes: harmless, but **do not compare them**.
* `calendar+0x40` is the only play-time counter that survives in a `.SAV` (wall-clock seconds, accumulated in `calendar_write_a`); the profile `play_ms` is game milliseconds. They are different quantities.
* Saving scenario 6 uses the autosave as "continue" state: `0x6F3730` shows the continue option only if the autosave entry has `scenario == 6 && autosave == 1`.
* Errors are reported by returning 0 and writing an *English* literal into `SaveList.last_error`; nothing is displayed unless a UI polls it. The port should return a typed error.
* A `.SAV` written by `create()` ignores the install prefix (0x586F00) — i.e. it is written relative to the **current directory**, while `open()` prepends `*(char**)0xA1579C`. The original therefore requires CWD == install dir.

## 7. Cross-module requests
* Rename `FUN_00586DA0 → file_open`, `FUN_00586F00 → file_create`, `FUN_005873A0 → file_read`, `FUN_005872E0 → file_write`, `FUN_00587510 → file_write_array`, `FUN_00587650 → file_read_array`, `FUN_00587BA0 → file_write_u16`, `FUN_00587DA0 → file_write_u32`, `FUN_00587C20 → file_read_u16`, `FUN_00587E20 → file_read_u32` (resdb.md/world_binary.md use partly different names — pick one set).
* ProfileArray.md: rename the "SaveGame" 0x4B4 type to **Unit** and cross-link Profile.md (the 0x4B4 object is the engine `Unit`, ctor 0x4155E0). Scenario.md already calls it `hero_saves`.
* Name the gamefil2 section methods as in §2 and their `this` globals (from the `mov ecx` before each call in 0x474EC0; full list in [sav.md §2.1](../../formats/sav.md)): S1 = Calendar (0xA47BA8), S2 = game-clock block (0xA50088: speed, saved speed, running, `now`), S3 = world_array (0xA45F08), S4 = GameState (0xA58F90), S7 = nation_array (0xA57F80), S8 = place_array (0xA484D0), S9 = facility array (0xA5D100), S10 = item array (0xA460A0), S13 = unit_array (0xA47C08), S14 = player list (0xA47CA8), S15 = ControlMgr (0xA5B9E8), S16 = LightSys (0xA47E08), S17 = HUD objective (0xA4D618), S18 = territory_array (0xA52940), S19 = trigger_system (0xA5CCD0), S20 = MsgLog (0xA53C28). Writer object between sections: 0xA549E8.
* Please export (writers): 0x483B90, 0x484AD0, 0x484E00, 0x47E540, 0x47E5C0, 0x47E640, 0x47E850, 0x47F080, 0x47F1E0, 0x47F5E0, 0x4C93E0, 0x4C9460, 0x4C96F0, 0x4C8810 and 0x4731E0 (thumbnail writer; the decompiler crashed on it).

## 8. Open questions
* S5 = Battle `progress` (0xA51294), S6 = news array (0xA50020), S11 = modify_item list (0xA50530), S12 = unit–item attachment array (0xA48500): see sav.md §2.1. TODO(re): confirm S12's role (large-weapon crews / mounts) and the `DAT_A48EC8` vs 0xA50020 news question.
* TODO(re) 0x4731E0: thumbnail writer (BMP 512×256×24) and whether it grabs the back buffer or re-renders.
* TODO(re) `DAT_00A500C3` (header byte 37, sample 12).
* TODO(re) 0x4C8810 `profile_array_save` and when exactly profile snapshots are written (mission end 0x720CA0 is the only caller seen).
* TODO(re) `DAT_00A54B0A` (error-message suppression flag) and `DAT_009CFB9C` (default message buffer).
