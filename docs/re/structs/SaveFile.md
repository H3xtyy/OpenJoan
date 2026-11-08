# SaveFile (`GameFile` slot entry, 0x1A7 bytes) and SaveList (`gamefila.cpp`) — joan.exe

One entry of the save-slot list: the data the load/save menu shows for a `.SAV` file, and the object whose members are written into / read from the
`.SAV` header ([formats/sav.md](../../formats/sav.md) §3). Classes live in `gamefile.cpp` (load/save drivers, header), `gamefila.cpp` (list management),
`gamefil2.cpp` (the 20 body sections). Functions: [functions/savegame.md](../functions/savegame.md). Design: game/savegame.md.
Confidence: V = read in >= 2 places (or confirmed on the sample file), R = read once, I = inferred.

## 1. SaveFile entry (sizeof = **0x1A7 = 423**)

Allocated by `savelist_add_entry` (0x477440): `tracked_malloc(0x1A7, "d:\ww\gamefila.cpp")` then `memset(0, 0x1A7)` (0x477CF0). Entries are held in a
1-based `ilib::dynarr` of pointers (the save list); `entry+0` is rewritten to the entry's own index by 0x477B10 after every sort.

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x000 | 2 | i16 | `id` | 1-based position in the list (renumbered by 0x477B10 after sorting); `savelist_add_entry` returns `+id` on success, `-id` when the header check failed | 0x477440, 0x477B10 | V |
| 0x002 | 1 | u8 | `flags` | written as the first byte of a 4-byte block (see formats/sav.md §3); **bit 7 set = file rejected** by `savefile_check_header` (0x477D80). The writer clears it (0x478480) | 0x477D80, 0x478480, 0x478520, sample | V |
| 0x003 | 0x105 | char[261] | `path` | save path relative to the install, e.g. `SAVE\Misja1-Autozapis.SAV`. Used as the file name for open/create (`FUN_00586DA0(entry+3, …)`); found by case-insensitive compare (0x477640: full path, else basename) | 0x477E80, 0x65BED0, 0x477640 | V |
| 0x108 | 8 | FILETIME | `time_utc` | `GetSystemTimeAsFileTime` at save | 0x478480 | V |
| 0x110 | 8 | FILETIME | `time_local` | `FileTimeToLocalFileTime(time_utc)`; compared with `CompareFileTime` by the `_AUTO%03d` ring (oldest overwritten) | 0x478480, 0x477770 | V |
| 0x118 | 1 | u8 | `game_mode` | `DAT_00A500C2` (9 = in-game) at save time | 0x478480; sample byte 34 = 9 | V |
| 0x119 | 1 | u8 | `game_sub` | `DAT_00A500C3` (sample 12; meaning unknown) | 0x478480; sample byte 37 | R |
| 0x11A | 0x80 | char[128] | `title` | display name shown in the list (2nd argument of the `save_game` builtin; txr `AUTOSAVE|2` for autosaves). **Truncation: the header stores exactly 128 bytes** | 0x65BED0 (`strcpy(entry+0x11A, desc)`), sample | V |
| 0x19A | 4 | i32 | `scenario` | `GameState+0x17` (`DAT_00A58FA7`) when saved; re-read by a full load | 0x65BED0, 0x478CC0, 0x6F3730 | V |
| 0x19E | 1 | u8 | `autosave` | third argument of 0x65BED0: **1 = autosave**. Scenario 6 uses it to decide whether a "continue" save exists (`entry.scenario == 6 && entry.autosave == 1`, 0x6F3730) | 0x65BED0, 0x6F3730 | V |
| 0x19F | 4 | i32 | `cal_serial` | Calendar `serial` (`DAT_00A47BB0`, Julian Day Number of the in-game date) | 0x478480, sample | V |
| 0x1A3 | 4 | f32 | `cal_minute` | Calendar `minute_of_day` (`DAT_00A47BB4`) | 0x478480, sample | V |

Header order on disk: `+2(4) +0x108(8) +0x110(8) +0x118(1) +0x119(1) +0x19F(4) +0x1A3(4) +0x11A(0x80) +0x19A(4) +0x19E(1)` (0x478CC0 / 0x478520, identical order).
Note `+2` is written with **4 bytes** (its three trailing bytes are `path[0..2]`) but read back as 1 byte.

## 2. SaveList manager (gamefila.cpp / gamefile.cpp, the object that owns the entries)

`ilib::dynarr` (count at `+0xC`) with extra members; entries are looked up with `0x430410(i)` (= `dynarr::get`).

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x01D | 0x105 | char[261] | `dir` | current directory of the file browser (`"SAVE\"`), copied by `FUN_007B2020(this+0x1D, dir)`; `..\` strips the last component, `.\` is a no-op | 0x4770A0, 0x477770 | V |
| 0x122 | 0x104 | char[260] | `last_error` | last error text: `strncpy(this+0x122, msg, 0x104); this[0x226] = 0` (0x4775F0). Messages (English literals, never localised): `"Error opening file."`, `"Error loading file."`, `"Error creating file."`, `"Error saving file."`, `"Invalid file format."` — only stored when `DAT_00A54B0A == 0` | 0x4775F0, 0x477E80, 0x478180, 0x478520 | V |

## 3. Behaviour summary

* **Scan** (0x4770A0): clear the list (0x477030 -> `savelist_remove_entry` for each), find `dir + "*.SAV"`, open each hit with `open(path, 0, 1)` and call `savelist_add_entry(File)`; entries whose header check fails
  (`id <= 0`) are removed again; finally sort (comparator `LAB_004024C3`) and renumber (0x477B10). The `File` is passed as `this`-less argument; `strcpy(entry+3, file.path)` takes the path from `File+0`.
* **Header-only load** `savefile_check_header` (0x477D80): `read_header` then `(entry.flags & 0x80) == 0`.
* **Full load** (0x477E80): `File f; f.open(entry+3, 0, 1); read_header(f); read_sections(f); close`. Errors set `last_error` and return 0.
* **Save** (0x478180): `stamp_header()` (0x478480); `f.create(entry+3, 0, 1)`; `write_header(f)`; `write_sections(f)`; close.
* **Lookup/creation by name** (0x65BED0, the `save_game` builtin handler): `name + ".SAV"` -> existing entry by 0x477640, else new entry (0x477440 with `File = 0`, which returns the new id via the always-true 0x477D60); then `entry.path = "SAVE\" + name + ".SAV"`, `entry.title = description`, `entry.scenario = GameState.scenario`, `entry.autosave = arg3`, save, refresh the list UI (0x4731E0) and re-sort.
* **New manual file names** (0x477770, `is_auto == 0`): `"SAVE\%s%%04d.%s"` formatted with base name and extension (`DAT_009D35C0` = `"SAV"`), probing `n = 0..9999` until `FileExists` fails (stops at 9999).
* **Auto ring** (0x477770, `is_auto != 0`): names `"_AUTO%03d.SAV"` for `n = 0..14` (15 slots, `local_c = 0xF`); first name that has no entry wins; if all 15 exist the entry with the smallest `time_local` (oldest) is overwritten.

## 3b. Section singletons
`0x474EC0` loads `ECX` with a fixed global before each section (list in [formats/sav.md §2.1](../../formats/sav.md)); the save-file writer object is 0xA549E8.

## 4. `File` packed flag
`File+0x10D` (see [File](File.md)) = 1 for `.SAV` files: the sized read/write primitives prefix `u16 length`. Detailed semantics in [formats/sav.md §1](../../formats/sav.md).
