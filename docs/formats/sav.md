# `.SAV` save game, `.BMP` thumbnail and `Player.dat` profile store

Reference build: `joan.exe` (the Steam build is the RE reference; the retail `joan.exe` differs only in the CD check). Writer/reader code:
`gamefile.cpp`, `gamefil2.cpp`, `gfile3.cpp`, `gamefila.cpp`, `profile.cpp` (function table: [functions/savegame.md](../re/functions/savegame.md);
structs: [SaveFile](../re/structs/SaveFile.md), [Profile](../re/structs/Profile.md), [ProfileArray](../re/structs/ProfileArray.md); port: `src/save/README.md`).

Sample used for verification (user's install, never modified): `Save/Misja1-Autozapis.SAV` (**189,136 bytes**), `Save/Misja1-Autozapis.BMP`
(393,270 bytes), `Save/Player.dat` (114 bytes). The sample was probed with byte-exact regex queries (every structural claim marked **[S]** below was
matched against the real bytes; **[C]** = derived from the decompiled code only; **[?]** = inferred/unknown).

All integers are **little endian**. Text is the install's 8-bit code page (cp1250 on the Polish install).

> **Warning.** Most records are *raw memory images* of 32-bit MSVC objects (packed, unaligned). They contain heap pointers,
> vtable pointers and `time_t` stamps of the process that wrote them. Pointer-valued fields are *only* used as "non-null = a sub-block follows"
> flags; the numeric value is meaningless. A save made by one build of the exe is not guaranteed to load in another (vtable pointers are read back
> verbatim). OpenJoan is a port of the game: it loads and writes this format through the original's own code paths, field by field (`src/save/README.md`).

---

## 1. The "packed" File mode (record framing)

Every file access goes through the ilib `File` wrapper ([File](../re/structs/File.md), 0x10E bytes: `+0x105` HANDLE, `+0x109` report_errors, `+0x10D` **packed** flag).
`.SAV` files are opened with `open(path, report_errors = 0, packed = 1)` (0x477E80) and `create(path, 0, 1)` (0x478180). `Player.dat` is opened with
`packed = 0` (plain `ReadFile`, 0x4C8BB0). In packed mode the *sized* primitives prefix their payload; the *raw* scalar primitives do not:

| primitive | writer | reader | bytes on disk |
|---|---|---|---|
| `W(ptr, n)` / `R(ptr, n)` | 0x5872E0 | 0x5873A0 | `u16 n'` then `n` bytes, where `n' = n` if `n < 0x10000` else **0**. (n = 0 also writes `u16 0`.) |
| `U16(v)` / `u16 = ru16()` | 0x587BA0 | 0x587C20 | 2 raw bytes, **no prefix** |
| `U32(v)` / `u32 = ru32()` | 0x587DA0 | 0x587E20 | 4 raw bytes, **no prefix** |
| `WA(ptr, count, elem)` / `RA(...)` | 0x587510 | 0x587650 | if `count < 0x10000 && elem < 0x10000 && (count|elem) != 0`: `u16 count, u16 elem`; else `u16 0, u16 0, u32 count, u32 elem`; then `count*elem` raw bytes |

Reader semantics of `R(dst, n)` (0x5873A0, tolerant to struct growth): read `u16 s`. If `s == 0` read `n` bytes. Otherwise read `min(s, n)` bytes,
zero-fill `dst[min..n)` when `s < n`, and **seek forward `s - n` bytes** when `s > n`. So a parser that does not know the in-memory size can always
trust `s` when it is non-zero; `s == 0` means "exactly the size the code expects" (records >= 64 KiB; none occur in practice).
`RA` (0x587650) does the same for count and element size (re-striding when the stored element size differs).
Retry-on-failure message boxes ("Error reading file %s, Retry ?") exist only when `report_errors != 0`; the save code runs silent.

Notation below: `W(n)` = a packed block of `n` bytes **plus its 2-byte prefix** (total `n + 2`); `U16`, `U32` are raw scalars.

## 2. File layout

```
Header                      189 bytes  (section 3)
repeat 20 times:  Marker (6 bytes: W(4) of an uninitialised stack dword), Section k
Trailer marker              6 bytes   (last 6 bytes of the file, [S] at 189,130)
```

* **Marker** = `04 00` + 4 bytes of **stack garbage** (the code writes `W(&uninit_local, 4)` before each section; the loader reads 4 bytes into the same local
  and ignores them). The garbage is not constant (sample [S]: bytes 191..194 are not all 0/1). Skip 6 bytes.
* Order and addresses (writer `gamefil2::fn_00474EC0`, reader `gamefil2::fn_004735C0`; the reader is the mirror image and ignores the markers):

| # | writer | reader | content | owner object (this) |
|---|---|---|---|---|
| 1 | 0x476890 | 0x4767C0 | `W(0x50)` raw **Calendar** | [Calendar](../re/structs/Calendar.md) @0xA47BA8 (size 0x50, `+0x3C time(NULL)`) [S: +0x3C top byte 0x69..0x6C = 2026] |
| 2 | 0x476C40 | 0x476990 | `W4(+0x44) W2(+0x48) W1(+0x4A) W4(+0x32)` of the game-clock block (speed, saved speed, running, `now`) | 0xA50088 [S: sizes 4,2,1,4] |
| 3 | 0x4854C0 | 0x4855B0 | **World** array | world_array manager (+0x2E loaded, +0x32 current) |
| 4 | 0x484780 | 0x4848A0 | **GameState** + Scenario + stages + hero saves + end conditions + two u16 lists | [GameState](../re/structs/GameState.md) @0xA58F90 |
| 5 | 0x483B10 | 0x483B50 | `W(4)` of the global at 0xA51294 | Battle `progress` (§2.1) |
| 6 | 0x483130 | 0x4831C0 | dense list of inline 0x52-byte records (0xA50020) | news array (§2.1) |
| 7 | 0x47FA50 | 0x47FB30 | **Nation** array (0x1545-byte records) | [Nation](../re/structs/Nation.md) |
| 8 | 0x47FE80 | 0x47FF50 | **Place** array (0x1F6-byte records) | [Place](../re/structs/Place.md) |
| 9 | 0x480C00 | 0x480CE0 | **Facility** array (typed, virtual save/load) | facility_array |
| 10 | 0x481FB0 | 0x4820D0 | **Item** array (typed, virtual save/load) | item_array |
| 11 | 0x482B50 | 0x482C60 | item-reference array (u16 recnos) | modify_item list (§2.1) |
| 12 | 0x482EA0 | 0x482F70 | array of 0x6C-byte records | unit–item attachment array (§2.1) |
| 13 | 0x47D7A0 | 0x47D8F0 | **Unit** array (0x4B4-byte records + sub-objects) | unit manager (defaults at +0x30..+0x6E) |
| 14 | 0x4838E0 | 0x4839C0 | array of 0x48-byte **ProfileArray** objects | "players" list **[?]** |
| 15 | 0x4857C0 | 0x485930 | `W(0x126E)` + `W(0x28)` | **ControlMgr** 0xA5B9E8 (+ mode block `DAT_00A48CEF`) |
| 16 | 0x485AB0 | 0x485BF0 | 8x`W(4)` + `W(0x18C)` + 2x`W(0x194)` | **LightSys** (weather `+0x58C..+0x5A8`, day schedule tables) |
| 17 | 0x4843D0 | 0x4845A0 | 9 small blocks at +0x14D..+0x299 | HUD objective / end-condition pop-up state (HudScreen +0x14D.. / EndCondition.md) |
| 18 | 0x485D30 | 0x485DC0 | dense list of 0xE0-byte records | **Territory** array |
| 19 | 0x485E70 | 0x485F50 | `W(1)` + dense list of 0x43-byte records | trigger_system |
| 20 | 0x486110 | 0x486280 | MsgLog | [MsgLog](../re/structs/MsgLog.md) |

(The orders of writer and reader are identical; the load path additionally clears/frees the existing contents of arrays 7, 8, 18, 19, 20 first
(0x4B6530, 0x4C5260, 0x4E49B0, 0x4EEE10, 0x4B31C0).)

### 2.1 Singleton addresses (read from the instruction listing of 0x474EC0: `mov ecx, <singleton>` before each section call)

| # | singleton | identification |
|---|---|---|
| 1 | 0xA47BA8 | Calendar |
| 2 | 0xA50088 | game-clock/speed block (TIMING.md §2 container): `+0x32` = `DAT_A500BA` game clock ms (`now`), `+0x44` = `DAT_A500CC` game speed (f32), `+0x48` = `DAT_A500D0` saved speed, `+0x4A` = `DAT_A500D2` game_running (forced to 1 on load). So S2 persists *now*, speed and saved speed (the earlier "Calendar +0x44.." reading was wrong) |
| 3 | 0xA45F08 | world_array |
| 4 | 0xA58F90 | GameState |
| 5 | 0xA51294 | the Battle object's `progress` counter (`battle_is_world_ready` 0x41C7E0 is `*0xA51294 > 9`, called with `mov ecx, 0xA51294` at 0x65A2D9 and from the game tick 0x4E5FC0; docs/re/functions/battle.md). Not the RNG state. |
| 6 | 0xA50020 | **news array** (Enlight's `NewsArray` idiom): `this = 0xA50020` in 0x4B95F0 `news_remove(type, para1, para2)` (loops `size` 0x417AD0 / `get` 0x4B96C0, matches `+0` type byte, `+0x18` para1, `+0x0C` para2, removes via 0x584A10 and decrements `+0x20`), called from the talk-message delete 0x46AD90 (`news_remove(1, talk_recno, 0/1)` when either nation is the player's); 0x4B9E20(recno, 1) adds the diplomacy news (talk-message add 0x4E3D60, reply 0x480FF0). TODO(re): reconcile with the `DAT_A48EC8` init in WorldArrays.md (likely the message ticker, not the news array). |
| 7 | 0xA57F80 | nation_array |
| 8 | 0xA484D0 | place_array |
| 9 | 0xA5D100 | facility_array |
| 10 | 0xA460A0 | item_array |
| 11 | 0xA50530 | the **modify_item list**: `item_delete` 0x49F370 does `mov ecx, 0xA50530` before 0x485410 (`contains(modify_id)`) and 0x4AFAF0 (`unregister(modify_id)`), docs/re/structs/Item.md `+0x06`; `hud_status_update` 0x6FD8B0 walks it. |
| 12 | 0xA48500 | **unit–item attachment array** (0x6C records: `+0x08` item recno, `+0x2A` unit recno): 0x467C30 `nearest_attached_item(type_code, radius)` walks it (`this = 0xA48500`, get 0x485730, deleted-check 0x485760, type code `'E'` needs 0x5094D0(unit)), the unit detach 0x42AC40 removes through 0x44CDD0(unit), the unit main step 0x7C7AB0 reads it. TODO(re): confirm it is the large-weapon crew / mount table (`LARGE_WEAPON_PUSH`). |
| 13 | 0xA47C08 | unit_array |
| 14 | 0xA47CA8 | player/profile list (adjacent to the unit array; `0x4CA290` current profile) |
| 15 | 0xA5B9E8 | **ControlMgr** (charmove; `+0` = controlled unit recno, `+0x10` mode) — *not* CamMan; the `0x28` block goes to `DAT_A5BA34` = ControlMgr+0x4C |
| 16 | 0xA47E08 | LightSys |
| 17 | 0xA4D618 | HUD objective / end-condition pop-up state |
| 18 | 0xA52940 | 0xE0-record array directly after ScreenMgr (0xA52930); matches territory_array (reader frees it via 0x4E49B0) |
| 19 | 0xA5CCD0 | trigger_system |
| 20 | 0xA53C28 | MsgLog |

The file writer object used between calls is 0xA549E8.

## 3. Header (189 bytes) [S]

Written by `gamefile::fn_00478CC0` (0x478CC0), read by `gamefile::fn_00478520`. The fields live in the 0x1A7-byte slot entry
([SaveFile](../re/structs/SaveFile.md)); `entry+N` below is that struct. Before writing, 0x478480 stamps the time/mode fields.

| off | len | on-disk | field | notes |
|---|---|---|---|---|
| 0 | 2 | `04 00` | prefix | [S] |
| 2 | 4 | `52 53 61 76` | magic `"RSav"` (0x76615352) | wrong magic -> "Invalid file format." [S] |
| 6 | 2 | `04 00` | prefix | [S] |
| 8 | 4 | `00 53 41 56` | `entry+2` u8 **flags** + 3 stray bytes | The writer dumps 4 bytes at `entry+2`; byte 0 is the flags (0x478480 clears it), bytes 1..3 are the first three characters of the path (`"SAV"` of `SAVE\...`) living at `entry+3`. The reader keeps only the first byte (min rule). Bit 7 set = refuse (0x477D80 returns "invalid"). [S] |
| 12 | 2 | `08 00` | prefix | [S] |
| 14 | 8 | FILETIME | `entry+0x108` UTC write time (`GetSystemTimeAsFileTime`) | [C] |
| 22 | 2 | `08 00` | prefix | [S] |
| 24 | 8 | FILETIME | `entry+0x110` local write time (`FileTimeToLocalFileTime`) | the save list sorts on / the autosave ring compares this [C] |
| 32 | 2 | `01 00` | prefix | [S] |
| 34 | 1 | u8 | `entry+0x118` = `DAT_00A500C2` game-mode byte (9 = in-game world) | sample = 9 [S] |
| 35 | 2 | `01 00` | prefix | [S] |
| 37 | 1 | u8 | `entry+0x119` = `DAT_00A500C3` | sample = 12 [S] (meaning unknown) |
| 38 | 2 | `04 00` | prefix | [S] |
| 40 | 4 | i32 | `entry+0x19F` = `DAT_00A47BB0` = Calendar `serial` (Julian Day Number of the in-game date) | sample bytes 41/42 = `3A 22`: ~0x223A2x = 1429-04-29 (JDN 0x223A26) [S] |
| 44 | 2 | `04 00` | prefix | [S] |
| 46 | 4 | f32 | `entry+0x1A3` = `DAT_00A47BB4` = Calendar `minute_of_day` | sample top bytes `xx xx 3x 44` = 704..~800 (start value 720 = 12:00) [S] |
| 50 | 2 | `80 00` | prefix (0x80) | [S] |
| 52 | 128 | char[128] | `entry+0x11A` **display name** (the second argument of `save_game`), NUL-padded | sample = `"Misja 1 Autozapis"` (txr `AUTOSAVE|2` with scenario 1) [S] |
| 180 | 2 | `04 00` | prefix | [S] |
| 182 | 4 | i32 | `entry+0x19A` **scenario number** (copy of `GameState+0x17`) | sample = 1 [S] |
| 186 | 2 | `01 00` | prefix | [S] |
| 188 | 1 | u8 | `entry+0x19E` **autosave flag** (third arg of 0x65BED0: 1 = autosave) | sample = 1 [S] |
| 189 | | | first marker | |

The slot list (load menu) reads only this header from each `*.SAV` (0x477D80 -> 0x477440) and shows name, local time, scenario and the thumbnail.
After a **full** load the same header fields are re-read into the entry (so `entry+0x19A` is the scenario to start).

## 4. Generic sub-grammars

### 4.1 `dynarr` slot lists ("Slots")
All object arrays are 1-based pointer arrays (`ilib::dynarrb`, +0xC count; free slots are null; see [WorldArrays](../re/structs/WorldArrays.md)).
Unless a section says otherwise:

```
U32 count                      // dynarr count (417AD0)
for i in 1..count:
    U16 present                // 0 = free slot (nothing follows); otherwise 1 or a class tag (see section)
    if present: record(i)
IndexTail                      // see below (absent in sections 6, 10, 18, 19; section 12 and 11 have it, 8/9/13/14/20 too)
```
**IndexTail** (writer 0x585B40, reader `dynarrb::fn_00585BB0`): `U32 n; if (n > 0) W(n*8)`. This is the `dynarrb` **free list** (`arr+0x1D` table, `arr+0x25 = n`, `+0x21` capacity):
n entries of 8 bytes, the first dword being a freed slot index (`dynarrb::remove(i)` 0x5858C0 appends `i` and zeroes the slot; `push` 0x5857A0 pops the *last* entry first, so freed recnos are reused LIFO).
Slots with `present == 0` are recreated with `dynarr::push_empty(1)` (0x584BD0: `count++`, zero the element). Exact recno reuse behaviour needs this table kept verbatim.
Sample: S3's tail is `n = 0` [S].

Slot lists with *inline* elements (sections 6 and the sub-lists of 4.2: `0x417200(idx)` returns `data + (idx-1)*elem_size`) have no pointer indirection; sections 18 and 19 are pointer arrays that are
written densely (no `present` flags).

### 4.2 Unit record (0x4B4 bytes in memory; "Unit" ctor 0x4155E0, writer 0x47DBE0, reader `gfile3::fn_0047DE30`)
```
W(0x432)                       // raw object image (first 0x432 bytes; includes vptr at +0, nation +2, type +0x3B, many pointer flags)
if (unit+0x33 != 0) subobject A: W(0x58)                         // new 0x58 object (ctor 0x528660)           [C]
W(0x3B)                        // second raw block (47E5C0 writer / 47E600 reader), destination not visible   [C, dest ?]
U32-slot list (47E640 / 47E700): R(4) into a pointer slot; if non-zero: U32 n; n x W(8)   // dynarr of 8-byte entries [C]
if (unit+0x1C1 != 0) object B (0x1E): W(0x1E); for the 1x2 matrix + (+0xA, +0xE) pointers: each non-null -> W(0x12);
                     for (+0x12, +0x16, +0x1A) non-null -> 0x3E-byte "matrix" object: nested  count0 x count1 x R(elemsize)
                     then U32 n + n x (U16 present, W(0x18))                                      // 47E850 / 47E9F0 / 47EF30 [C]
if (unit+0x2A1 != 0) object C (0x3E): W(0x21); U32 n; n x W(0x21)                                  // 47F080 / 47F130 [C]
type = unit+0x3B ; k = unit_res[type].+0x1AD (0x42D0A0 stride 0x203)
k x U32 (unit+0x378[i]) ; k x U32 (unit+0x37C[i])               // per-skill/slot arrays              [C]
2 x ( W(0x24); U32 n; n x W(0x24) )                              // 47F1E0 / 47F290                 [C]
if (unit+0x335 != 0) U16 (*(u16*)(unit+0x335 + 2))               // leader/target unit recno          [C]
```
Reader fix-ups afterwards: 0x47E300 (clears runtime fields), 0x4FED00/0x4FEDF0 (re-link `+0x335`), 0x509250 (re-apply nation `+2` bookkeeping).
The exact bytes of 47E540/47E5C0/47E640/47E850/47F080/47F1E0 (writers) were **not analysed** -> sizes above are taken from the reader and are exact; the
order of writer == order of reader (verified for 47DBE0 vs 47DE30 call order).

### 4.3 End-condition record
`W(0x1A)` raw; `if (rec+1 != 0)` (a packed `char*`): `U16 len = strlen+1; W(len)`; `if (rec+5 != 0)` same. (0x4841D0 writer, `gfile3::fn_004842C0` reader — the reader
uses `U16 len` followed by `R(len)`.) [S: first record at 4291 = `1A 00` + 26 bytes, followed by a string block].

### 4.4 Stage record (Scenario stage, 0x1B2 bytes)
`W(0x1B2)`; `if (stage+0x19E != 0) { U16 len; W(len) }` (scenario title); `if (stage+0x1A2 != 0) { U16 len; W(len) }` (description). `len = strlen+1`.
(reader `gfile3::fn_00483CA0`; writer 0x483B90, round 2.) **Original bug (0x483B90):** the description block is written with `len = strlen(description)+1` but its *bytes are read from the title pointer* (`+0x19E`), so the stored "description" is the first `len` bytes of the title buffer (over-reading past the title's NUL when the description is longer — sample: 18 bytes vs a 10-byte title). Readers must take `len` from the prefix and may treat the bytes as garbage; the loaded description is therefore unreliable. [S: stage 0 title len 10, description len 18, see §6].

## 5. Section grammars

Record sizes are the exact `W(n)` sizes. "[C]" = from code, "[S]" = also confirmed on the sample.

**S1 Calendar** — before writing: `calendar+0x40 += time(NULL) - calendar+0x3C; calendar+0x3C = time(NULL)` (0x476890; so `+0x40` accumulates wall-clock seconds
played); then `W(0x50)`. Reader: `R(0x50)`, then `calendar+0x3C = time(NULL)`. [S] sample: `+0x3C` (file 257..260) has top byte in 0x69..0x6C (a 2026 `time_t`), `+0x40` (261..264) is < 0x10000 (bytes 263/264 are 0/1).

**S2** (singleton 0xA50088, §2.1) `W(4)@+0x44 (game speed f32), W(2)@+0x48 (saved speed), W(1)@+0x4A (game_running), W(4)@+0x32 (game clock ms `now`)`; the reader forces `+0x4A = 1`. [S: sizes 4,2,1,4]

**S3 World array** (0x4854C0): `U32 +0x2E (loaded flag)`, `U32 +0x32 (current world index)`, `U32 count`; slots with `present` 0/1; record = `W(0x92)` World image; `if (world+0x6E)` `W(0xD5)`;
`for j in 0..*(i32*)(world+0x72)-1: W(0xAC)` (WorldBlock; its three owned arrays are *not* saved — re-allocated and zeroed on load, 0x484E40); IndexTail.
Reader additionally: zero the pointer fields (0x485450 / 0x484B60 / 0x484F50), `world+0x72 = 0` then re-add each block with `world::fn_00547BF0(1)`; sets `+0x36` from `+0x32`.
[S]: `+0x2E = 1`, count = 1; World at 322..469, sub-object at 470..684, WorldBlock at 685..858, IndexTail `n = 0` at 859.

**S4 GameState** (0x484780 / `gfile3::fn_004848A0`):
```
W(0x745)                                  // GameState image (offsets in GameState.md; list_a/list_b pointers inside are dead)
W(0x289)                                  // Scenario[0..0x289) = GameState+0x17.. (second copy, StageSet header: stage_count @+0x0C, stages ptr @+0x10)
for k in 0..stage_count-1: Stage(k)       // 4.4
Stage(cur_stage)                          // one extra record (the embedded cur_stage, Scenario+0x14)
for k in 0..hero_save_count-1: Unit       // hero_save_count = i16 at GameState+0x29E (Scenario+0x287); 4.2
for k in 0..end_cond_count-1: EndCond     // end_cond_count = u16 at GameState+0x309; 4.3
WA(list_a_ptr @+0x71C, list_a_count @+0x720, 2)   // u16 ids
WA(list_b_ptr @+0x728, list_b_count @+0x72C, 2)
```
Reader: frees the old list_a/list_b buffers first; after the 0x289 block allocates `stage_count*0x1B2` bytes for the stage array, reads the stages, re-reads the extra
record, zeroes the runtime pointers (0x483DD0 script ptr `+0x186`; 0x484160 hero/unit-ref lists), allocates `hero_save_count` Units (new 0x4B4 + 0x47DE30 each),
reads the end conditions, then `list_a_cap = list_a_count + 10`, `list_b_cap = list_b_count + 10` and `RA` each list. Script objects are **not** saved: they are
re-compiled from `SCRIPT\SCENARIO\<script_file>.sce` / `<desc_file>.sce` by the load flow (`battle::0x41EA60`); script progress lives in the four scenario bit banks (Scenario+0x18A..0x196, inside the 0x745 image).
[S] verified offsets: 0x745 image at 871..2731 (scenario number `01` at 894 = `GameState+0x17`), `stage_count = 1` (u32 at 906), `hero_save_count = 0` (i16 at 1541), 0x289 block at 2734..3382,
stage 0 at 3385..3818 (title `len 10`: 3819..3832, description `len 18`: 3833..3854), extra stage record 3857..4290 (no strings), first end condition 4293..4318 + string block at 4319.

**S5** `W(4)` of a 4-byte global (reader `R(4)`). **[?]**

**S6** `U32 count; count x W(0x52)` (dense **inline** 0x52-byte elements, no `present` flags, no IndexTail; the reader does `dynarr::push(0)` then `R(0x52)` into the new inline element, 0x4830F0). **news_array** (element accessor 0x4B96C0 lies in the `news.cpp` address range 0x4B9250..0x4B9340, which init/free the news list; round-2 export).

**S7 Nation** (0x47FA50): `U32 +0x2E; U32 count`; slots; record `W(0x1545)`; then 20 x `W(0xF4)` (the `rel[20]` block is saved a second time; the reader re-reads it with 0x47F620);
then `u16` arrays: if `nation+0x1531` non-null `n=i16@+0x152D` x U16; likewise `+0x1539 (n=@+0x1535)` and `+0x1541 (n=@+0x153D)` (roster_a/b/c); IndexTail.

**S8 Place** (0x47FE80): `U32 count`; slots; record `W(0x1F6)`; `if (place+0x1DE != 0)` `U16 (*(u16*)(place+0x1DE + 2))` (`+0x1DE` is the Place's `scene` pointer, [Place](../re/structs/Place.md); the saved word is the scene/world-block recno, re-resolved on load with 0x420240 and attached with 0x4366F0); IndexTail. Reader post-fixes: 0x47FDF0, 0x4366F0, 0x42B350 (creates the place's runtime state at the saved position), 0x44F5C0.

**S9 Facility** (0x480C00): `U32 count`; slot tag `U16 = *(u16*)(facility+4)` (the facility type; 0 = free); then the object's own **virtual** writer `(*vtbl+0x54)(File)`; IndexTail.
Reader: tag 1 -> `new 0x140`, 2 -> `0x8E`, 3 -> `0x99`, 0x10 -> `0x2F14` (shop), any other non-zero tag -> `0x79`; each loaded with virtual reader `(*vtbl+0x58)`. (The per-class layouts are in the facility family.)

**S10 Item** (0x481FB0): `U32 count`; per slot `U16 tag`: 0 if free **or if the item type is not persistable** (0x4A4050 on the item_res row `item+10`), else `tag = *(u16*)(item+0x4E)` (the item *kind*) followed by the
virtual writer `(*vtbl+0x70)`. **No IndexTail.** Reader: tag 0 -> slot is created as a placeholder and removed again after the loop (recnos stay stable); tag -> allocation size:
1 -> 0xDB, 2 -> 0xCE, 3 -> 0xCE, 4 -> 0xEF, 10 -> 0xCD, 13 -> 0x107, 14 -> 0xCE, 18 -> 0xC2, default -> 0xC2; virtual reader `(*vtbl+0x74)`.

**S11** (0x482B50): `U32 count`; per slot: `U16 0` if free or not persistable (same 0x4A4050 test on `item_res[*(u16*)(e+10)]`), else `U16 1; U16 *(u16*)(e+4)` (item recno); IndexTail.
Reader: tag 0 -> null slot; else `U16 recno` -> pointer to item `recno` (0x425D20); IndexTail; then for every element with kind 1 or 2 whose virtual `[0x44]` returns non-zero -> `0x49F370(recno)`.

**S12** (0x482EA0): `U32 count`; slots; record `W(0x6C)`; IndexTail. Reader post-fix 0x482E50.

**S13 Unit array** (0x47D7A0): `U16 +0x2E; 4 x U16 @+0x30; 4 x U16 @+0x38; U32 count`; slots (`present` 0/1); record = Unit (4.2); IndexTail. Reader post: 0x47DAC0 (reloads the four selection-ring models).

**S14** (0x4838E0): `U32 count; U16 +0x2E`; slots; record = ProfileArray (`gfile3::fn_00483430`):
`W(0x48) [object image]; W(0x20) [head name]; U32 count_p (+0x20); count_p x R(0x20) [names, 0x4C9420]; count_p x { W(0x20) name; Unit-snapshot (current); U16 slot_count; slot_count x Unit-snapshot }`
`U32 na; na x U16; U32 nb; nb x U16; U32 aux (+0x40)`. **Note:** inside the save the list counts/aux are `U32`; in `Player.dat` they are `i16` (section 8). The Unit-snapshot is the short form of 4.2
(0x4C9930): `R(0x432); R(0x3B); slot list (4-byte pointer + U32 n + n x R(8)); if (unit+0x1C1) object B` — no sub-objects A/C and no per-type arrays.
Writers 0x4C93E0/0x4C9460 not analysed (mirror of the reader). IndexTail.

**S15 ControlMgr** (0x4857C0; singleton 0xA5B9E8, not CamMan): `W(0x126E)` at the object; then `W(0x28)` = the 40-byte "to" block selected by `DAT_00A48CEF` (0,1,2,4,5 via 0x563C80 / 0x423FE0; if the mode is 0 or `cam+0xAC != 0` mode 0 is used, mode 3 is not stored).
Reader: `R(0x126E)`, `R(0x28)` -> global `DAT_00A5BA34`, then 0x485990 clears runtime fields (`+0x8E..+0x91 = 0`, `+0x92..+0x95 = 1`, `+0x82,+0x86,+0x126A,+0x18,+0x1261,+0x1265,+0x7C = 0`, `+0x9B = 0`, `+0xAC = 1` unless `+0x10 ∈ {2,4}`).

**S16 LightSys** (0x485AB0): `W(4)` at `+0x58C,+0x590,+0x594,+0x598,+0x59C,+0x5A0,+0x5A4,+0x5A8` (rain/snow probability, start minute, ... see Calendar.md §2.2), then `W(0x18C)@+0xC`, `W(0x194)@+0x198`, `W(0x194)@+0x32C`.

**S17** (0x4843D0): `W(1)@+0x14D, W(1)@+0x14E, W(4)@+0x14F, W(4)@+0x153, W(0x28)@+0x157, W(0x28)@+0x1A7, W(2)@+0x1F7, W(0x50)@+0x1F9, W(0xA0)@+0x299` — the objective banner / end-condition pop-up state
([EndCondition](../re/structs/EndCondition.md): text length, state, timestamp, id array).

**S18 Territory** (0x485D30): `U32 count; count x W(0xE0)` (dense, `element = get(i)`; a null element would fault — the array has no holes at save time). Reader: clears the territory array (0x4E49B0) then `new 0xE0; R(0xE0); push` per record.

**S19 triggers** (0x485E70): `W(1)@+0x1D; U32 count; count x W(0x43)` (dense). Reader clears triggers first (0x4EEE10), creates each with 0x4EFF60.

**S20 MsgLog** (0x486110): `U32 +0x2E (count); U32 +0x32 (cursor); U32 +0x36; 9 x U32 @+0x3A (cat_head); 9 x U32 @+0x5E (cat_count); U32 slotcount`; slots (`present` 0/1);
record = MsgRecord: `W(0x1F); W(len+1) text; if (extra_len(+0x15) > 0) W(extra_len+1)` with `len = u16@+0x13`; IndexTail. Reader clears the log first (0x4B31C0).

## 6. Worked offsets of `Misja1-Autozapis.SAV` (all [S] unless noted)

| offset | length | what |
|---|---|---|
| 0 | 189 | header; name `"Misja 1 Autozapis"` @52, scenario `1` @182, autosave `1` @188 |
| 189 | 6 | marker 0 |
| 195 | 82 | S1 Calendar `W(0x50)` (prefix `50 00` @195, image 197..276) |
| 277 | 6 | marker 1 |
| 283 | 19 | S2: `04 00`+4 @283, `02 00`+2 @289, `01 00`+1 @293... = 6+4+3+6 = 19 bytes (ends 301) -> next marker at **302** |
| 302 | 6 | marker 2 |
| 308 | 12 | S3 header: `+0x2E = 1` @308, `+0x32 ∈ {0,1}` @312, `count = 1` @316 |
| 320 | 2 | slot 1 `present = 1` |
| 322 | 148 | `92 00` + World image (324..469) |
| 470 | 215 | `D5 00` + sub-object (472..684) |
| 685 | 174 | `AC 00` + WorldBlock (687..858) |
| 859 | 4 | IndexTail `n = 0` |
| 863 | 6 | marker 3 |
| 869 | 1863 | S4: `45 07` @869 + GameState image 871..2731 |
| 2732 | 651 | `89 02` + Scenario block 2734..3382 |
| 3383 | 436 | `B2 01` + stage 0 image 3385..3818 |
| 3819 | 14 | title: `0A 00 0A 00` + 10 bytes (begins with a capital letter N–Z, e.g. a place name) |
| 3833 | 22 | description: `12 00 12 00` + 18 bytes |
| 3855 | 436 | `B2 01` + `cur_stage` record (no strings) -> 4291 |
| 4291 | 28 | `1A 00` + end condition 0 (26 bytes); string block follows @4319 |
| ... | | sections 4 (rest) .. 20 (not walked byte-by-byte; their writers were partially unanalysed) |
| 189,130 | 6 | trailer marker (`04 00` + 4 bytes); EOF at 189,136 |

Derived checks: header length 189 + marker 6 = 195 ✓; S1 82 bytes ✓; S2 19 ✓; GameState image length 0x745 = 1861 -> next prefix at 871+1861 = 2732 ✓.

## 7. Thumbnail `<name>.BMP`

Same basename as the `.SAV`, in `Save/`. Sample [S]: standard Windows BMP, `BM`, BITMAPINFOHEADER (size 0x28), **512 x 256**, **24 bpp**, bottom-up, pixel data offset 0x36, file size 0x60036 (393,270). The writer has not been analysed (it is called from the save flow `0x65BED0` -> `0x4731E0`, whose decompilation failed). The save list uses it as the slot preview (`int8x6/savemenu.dds`, `qsave.dds` are the dialog skin).

## 8. `Save/Player.dat` (114 bytes) — profile store [S]

Plain (non-packed) file; the `ProfileArray` object ([Profile](../re/structs/Profile.md)) is written/read as (0x4C8BB0 full load, 0x4C8490 header-only):

| off | len | content | sample |
|---|---|---|---|
| 0x00 | 0x48 | raw image of `ProfileArray[0..0x48)`: `head[0x20]` name, `+0x20 count`, `+0x24 records*`, list fields (all dead), **`+0x38` gold (i32)**, `+0x3C` play_ms, `+0x40` aux | `"Player"` @0, `+0x38 = 0x64 = 100`, all else 0 |
| 0x48 | 0x20 | `head` again (overwrites the first 0x20 bytes of the object; this is the live name) | `"Player"` + zeros |
| 0x68 | 4 | `i32 count` of profiles | 0 |
| ... | | `count` x `char[0x20]` names; then `count` x { `char[0x20]` name; Unit-snapshot `current` (0x4C9930); `i16 slot_count`; `slot_count` x Unit-snapshot } | (none) |
| 0x6C | 2 | `i16 na`; `na` x u16 (list_a) | 0 |
| 0x6E | 2 | `i16 nb`; `nb` x u16 (list_b) | 0 |
| 0x70 | 2 | `i16 aux` (+0x40) | 0 |

File name: `SAVE\` + `head` + `DAT_009D56F0` (".dat"), i.e. `Save/Player.dat` for the default profile name `"Player"` (config `player_name`). Snapshot form: `R(0x432); R(0x3B); slot list; [object B]` **unpacked** (no u16 prefixes in this file because `packed = 0`).
Gold (`+0x38`) = `GameState+0x734`, `+0x3C` = `GameState+0x740` (play ms): copied in at mission start (0x41C810) and out at mission end (0x720CA0 -> 0x4C8810 profile save, **not analysed**).

## 9. Open questions (addresses)

* Round 2 resolved: S6 = news_array; S14 = the global player/profile list (its accessor 0x485790/0x485900 is used by `0x4CA290` = current profile). The decompiler still hides ECX in 0x474EC0, so S5 (4-byte global; RNG state unproven — TODO(re): read `mov ecx` before the call to 0x483B10 in disassembly), S11 (item refs, accessors 0x4853E0/0x485410 also used by 0x50B390) and S12 (0x6C records, accessors 0x485730/0x485760) stay "inferred".
* Script path of `save_game` (0x649920 case 0x13B): the handler's 3rd argument is not pushed by the VM (stale register byte) → the autosave flag stored for script saves is unspecified (TODO(re)); only 0x472D10 passes a defined `1`.
* The sample walk beyond S4 offset 4319 was not extended in this round (sections 5–20 grammar comes from reader+writer pairs).
* Thumbnail writer 0x4731E0 still fails to decompile (callees unknown), `profile_save` 0x4C8810 TODO(re).
* Meaning of header byte 37 (`DAT_00A500C3`, sample 12).
