# `.res` binary banks (ResLX): `u32 count` + `{char name[34]; u32 offset}` x (count+1) + payloads

Reader: `ilib\reslx.cpp` `ResLX::open` **0x59B920**, `get_payload` 0x59BF70, `payload_size` 0x59BDD0 (joan.exe;
see [functions/resdb.md](../re/functions/resdb.md), [structs/ResLX.md](../re/structs/ResLX.md)). Sibling container with narrower names
(u16 count, name[9]): [gameset.md](gameset.md) (`ResX`). A third flavour, the dBase table `Cursor.res`, is [dbf.md](dbf.md).
Not every `*.res` is this format: `help.res` and `script/scenario/*EX.res` are plain text ([strtab.md](strtab.md)), `Gameplay.res` is ResX
([gameplay.md](gameplay.md)), `Cursor.res` is dBase, `pal_std.res` / `I_cursor.res` are raw ([pal_std.md](pal_std.md), [i_cursor.md](i_cursor.md)).

## Layout (all little-endian)

```
0x0000  u32   count                       number of payloads N
0x0004  entry[N+1]  each 38 (0x26) bytes:
          char  name[34]   NUL padded (<= 33 chars used), case as written
          u32   offset     ABSOLUTE file offset of the payload (from byte 0 of the file)
        entry[N] is a SENTINEL: name all zero, offset = end of the last payload (= file size)
4+38*(N+1)  payload 0 starts here (= entry[0].offset), payloads are contiguous in table order
```
- Payload `i` (0-based) = bytes `[entry[i].offset, entry[i+1].offset)`; size = difference (0x59BDD0). The reader REQUIRES the sentinel
  (it reads `(count+1)*38` bytes and `get_payload(i)` uses `entry[i+1]`).
- 1-based API: `get_payload(idx)` with `idx = 1..N`; `entry(idx)` returns the entry whose name is at its start (0x59C3B0).
- Offsets are 32-bit, absolute (`SetFilePointer(FILE_BEGIN)` in 0x59BF70). The format cannot exceed 4 GiB (Wave.res is 80,396,036 B).
- Name lookup: the opener builds a sorted copy (qsort stride 0x26, comparator thunk 0x4070FE) used by its callers;
  TODO(re): comparator / case rule of the ResLX name lookup (names are upper case in every shipped file; `unitAI.res` names are matched against
  lower-case literals with a case-insensitive compare, 0x546520). Iteration keeps file order.
- Open modes (caller choices, no effect on the format): `load_all` reads all payload bytes into RAM and closes the file; streaming reads one
  payload at a time into a cache (or the global scratch buffer) — the returned pointer is valid only until the next call.
- Zero-byte file = empty bank (count reads as 0): `Resource/Anim_am2.res` and `Anim_am3.res` are 0 bytes in this install and 0x4172A0 opens both.

## Verified header values

| file | size | count | entry 0 | sentinel / first payload |
|---|---|---|---|---|
| `script/unit/unitAI.res` | 39,117 | 0x12 = 18 | `HUMAN_LIGHT_GENERAL` @0x04, offset @0x26 = 0x2D6 = 4+19*38 | payload 0 at 0x2D6 |
| `unit/unit.res` | 188,578 | 0x72 = 114 | `ALENCON`, offset 0x1116; entry 1 `ALENCON_2` 0x17E8 (name column 0x04, offset @0x26) | |
| `item/item.res` | 273,900 | 0xFC = 252 | `ALENCON_ARMOR_LV1`, offset 0x2592 | |
| `Resource/dropitem.res` | 1,318 | 9 | `DEFAULT`, offset 0x180 = 4+10*38; entry1 `ARROW` 0x1B7 | |
| `Resource/animscr.res` | 352,350 | 0x15B = 347 | `SWORD` offset 0x33AC; `DEFAULT` 0x3501 | |
| `Resource/texteff.res` | 70,606 | 0x33 = 51 | `BIGRED` | |
| `Resource/A_wave2.res` | 28,290 | 1 | `TEST2`, offset 0x50 = 4+2*38 | entry 1: name zero, offset 0x6E82 = 28,290 = file size; payload at 0x50 begins `RIFF` |
| `Resource/Wave.res` | 80,396,036 | 0x1F7 = 503 | `64-COMBATBEINGHIT6` | |
| `Resource/S7L1_finish.RES` | 305,130 | 1 | `0023` | |

Entry stride 38 and offset at +0x22 are confirmed by the first two entries of unit.res / dropitem.res / animscr.res (second name at 0x2A,
its offset at 0x4C). The other shipped banks (`unit/recruit.res`, `item/sellitem.res`, `item/DynGrass.res`, `item/XDynGras.res`,
`map/scenarioN/scenarioN.res`, `Resource/A_wave1.res`, `Resource/S?L1_{tactic,finish}.RES`) have the same shape by inspection of the first bytes;
their opening functions are not identified (TODO(re): xref 0x59B920).

## Payload kinds

| bank | names | payload |
|---|---|---|
| `unit/unit.res` | unit codes (`ALENCON`, `ENGLISH_ARCHER_CAPTAIN`...) | text record: `VERSION 1.0` line then `Key : value` lines (`UnitClass`, `Code`, `Model`, `Animation`...) — owned by the unitres family |
| `item/item.res`, `dropitem.res`, `sellitem.res`, `recruit.res` | item / list codes | text records, same `VERSION 1.0` header (`Code : DEFAULT` ...) — itemres family |
| `animscr.res` | animation-script names (`SWORD`, `ATTACK`, `JUMP_HUM`, `HUM_GEN_ATK_A`...) | text parsed by 0x419320 (keys `StartTime`, `EndTime`, `Range`, `StartForceTime`, ...), see functions/resdb.md |
| `script/unit/unitAI.res` | unit AI classes (`HUMAN_LIGHT_GENERAL`, `PIG`, `HUMAN_ARCHER`...) | AI script text (`INT`, `FUNC`, `SCRIPT TASK_*` blocks); table reader 0x5462B0 only copies names |
| `map/scenarioN/scenarioN.res` | `S1L1_PLA`, `S1L1_STG`, `S1L1_UNI` | text files bundled together (payload starts `CreatePlace {` ...) = the `.pla`/`.stg`/`.uni` of the scenario ([pla.md](pla.md), [stg.md](stg.md), [uni.md](uni.md)) |
| `Resource/texteff.res` | text-effect names (`BIGRED`, `F_MODEL`, `N_WATER`...) | text; TODO(re) |
| `Resource/Wave.res`, `A_wave1.res`, `A_wave2.res` | sound-effect names (`64-COMBATBEINGHIT6`, `RM_A1`, `TEST2`) | **complete RIFF/WAVE files** (`RIFF`, size, `WAVEfmt `): verified at A_wave2 @0x50; see [wav.md](wav.md) |
| `Resource/S?L1_tactic.RES`, `S?L1_finish.RES` | 4-digit string IDs (`0037`, `0013`, `0023`) matching the `[0037N]` prefixes of the scenario texts | speech lines as RIFF/WAVE files (`WAVEfmt ` found in S1L1_tactic.RES) |

## Reading notes
1. Read `count`, then `(count+1)*38` bytes; validate `entry[i].offset` non-decreasing and `entry[count].offset <= file size`.
2. Treat names as NUL-terminated within 34 bytes; unused name bytes are zero (no garbage observed).
3. Keep the sentinel out of the public name list; expose payload spans, not copies (the Wave bank is 80 MB).
4. Round trip: header + table + payload concatenation is byte-exact reproducible if offsets are recomputed (they are contiguous).

## Install census

Every ResLX bank of the Polish install follows the layout (33 files incl. the two 0-byte banks):
`first offset == 4 + 38*(count+1)`, offsets non-decreasing and contiguous, sentinel name all-zero, **sentinel offset == file size**,
no bytes after the first NUL of any name (name columns are zero padded), and rebuilding a file from its entries reproduces it byte for byte for all 33.
Payload kinds in the table are told by content: empty / `RIFF....WAVE` / text (no control bytes except TAB CR LF 0x1A) / binary.

| file | size | entries | longest name | payload kinds |
|---|---|---|---|---|
| `Resource/A_wave1.res` | 294,921 | 11 | 8 | riff_wave 11 |
| `Resource/A_wave2.res` | 28,290 | 1 | 5 | riff_wave 1 |
| `Resource/Anim_am3.res` | 0 | 0 | 0 | - |
| `Resource/S1L1_tactic.RES` | 1,445,968 | 5 | 4 | riff_wave 5 |
| `Resource/S2L1_finish.RES` | 444,506 | 2 | 4 | riff_wave 2 |
| `Resource/S2L1_tactic.RES` | 2,689,766 | 8 | 4 | riff_wave 8 |
| `Resource/S3L1_tactic.RES` | 9,176,192 | 33 | 4 | riff_wave 33 |
| `Resource/S4L1_tactic.RES` | 5,128,270 | 18 | 4 | riff_wave 18 |
| `Resource/S5L1_tactic.RES` | 5,784,452 | 21 | 4 | riff_wave 21 |
| `Resource/S6L1_tactic.RES` | 11,231,840 | 39 | 4 | riff_wave 39 |
| `Resource/S7L1_finish.RES` | 305,130 | 1 | 4 | riff_wave 1 |
| `Resource/S7L1_tactic.RES` | 9,806,152 | 28 | 4 | riff_wave 28 |
| `Resource/S9L1_tactic.RES` | 13,817,946 | 47 | 4 | riff_wave 47 |
| `Resource/Wave.res` | 80,396,036 | 503 | 23 | riff_wave 503 |
| `Resource/anim_am2.res` | 0 | 0 | 0 | - |
| `Resource/animscr.res` | 352,350 | 347 | 32 | text 347 |
| `Resource/dropitem.res` | 1,318 | 9 | 11 | text 9 |
| `Resource/texteff.res` | 70,606 | 51 | 16 | text 51 |
| `item/DynGrass.res` | 9,377 | 26 | 17 | text 26 |
| `item/XDynGras.res` | 804 | 5 | 13 | text 5 |
| `item/dropitem.res` | 5,868 | 37 | 18 | text 37 |
| `item/item.res` | 273,900 | 252 | 25 | text 252 |
| `item/sellitem.res` | 12,024 | 47 | 12 | text 47 |
| `unit/recruit.res` | 9,349 | 38 | 18 | text 38 |
| `unit/unit.res` | 188,578 | 114 | 30 | text 114 |
| `script/unit/unitAI.res` | 39,117 | 18 | 20 | binary 18 |
| `map/scenario1/scenario1.res` | 411,055 | 3 | 8 | text 3 |
| `map/scenario2/scenario2.res` | 685,050 | 6 | 10 | text 6 |
| `map/scenario3/scenario3.res` | 1,271,357 | 6 | 10 | text 6 |
| `map/scenario4/scenario4.res` | 2,433,777 | 15 | 10 | empty 2, text 13 |
| `map/scenario5/scenario5.res` | 4,875,167 | 30 | 11 | text 30 |
| `map/scenario6/scenario6.res` | 3,384,493 | 24 | 10 | text 24 |
| `map/scenario7/scenario7.res` | 2,086,497 | 15 | 10 | text 15 |

Notes
- Longest name is 30 bytes (`unit.res`), so the 34-byte column never fills.
- `unitAI.res` payloads are text followed by exactly one NUL byte each (hence "binary"): the AI script reader consumes NUL-terminated text.
- `scenario4.res` has 15 entries (`S4L1_{1..4}_{PLA,STG,UNI}` for the four sub-levels plus `S4L1_PLA`, `S4L1_STG`, `S4L1_UNI`);
  `S4L1_PLA` and `S4L1_UNI` are **empty** (size 0), so empty payloads are legal. Other scenarios: `SnL1_{PLA,STG,UNI}` (+ numbered variants).
- Non-ResLX files with the extension: `Cursor.res` (dBase, [dbf.md](dbf.md)), `Gameplay.res` (ResX), `help.res` and `script/scenario/*EX.res` (text),
  `I_cursor.res`, `pal_std.res` (raw). A count that makes `4+38*(count+1)` exceed the file size identifies them.

## Files in this format

`WAVE.RES`, `A_WAVE*.RES`, `S*L1_*.RES`, `animscr.res`, `texteff.res`, `dropitem.res`, `scenario/*.res` banks. The `scenario/*EX.res` files are not in this format: they are plain text ([strtab.md](strtab.md)). The audio entries of
`WAVE.RES` and the `A_WAVE*` banks are RIFF WAVE PCM (the first payload bytes are `RIFF`, see the verified header table). The lookup rule is the one in [gameset.md](gameset.md).
