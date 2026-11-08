# `.amt` — converted animation cache (quaternion keys, parent-relative), 16 files shipped

Writer: `FUN_005B7810` (0x5B7810, `AnimDef::write_cache`) + trailer writer `FUN_005B4FE0`; reader `FUN_005B7930` (0x5B7930) + trailer reader 0x5B5350
(joan.exe). Extension literal `.AMT` is at 0x9D9454 (inferred from the string table order; used at 0x5B3100/0x5B23D0/0x5B2AA0).
It is the pre-converted form of an [`.am2`](am2.md): keys are already 32-byte quaternion keys, already parent-relative, already reduced
(0x5B83A0). Read instead of the `.am2`+`.am3` when **all** hold (RE: 0x5B3100 state 4, 0x5B2AA0): the cache setting `DAT_00A6768C` is on, the
`.AMT` exists, and `file_newer(AMT, AM2) > 0` and (no `.AM3` or `file_newer(AMT, AM3) > 0`) (0x590E20 = mtime comparison). With setting
`DAT_00A67688` on the engine also **creates** the cache: directory `ANIM_TMP` is created (`CreateDirectoryA("ANIM_TMP")`) and
`ANIM_TMP\<name>.AMT` is written after a full `.am2` load. The shipped `.amt` files sit in `item/**/anim` beside the `.am2` (9 in `item/anim`, flags, tent, roast pig).
An implementation can ignore `.amt` completely (the `.am2` is authoritative) or use it as a fast path.

## Layout (little-endian)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0 | 4 | i32 | `n_tracks` | must equal the `n_tracks` of the `.am2` (reader fails otherwise) | 0x5B7930 compares with `AnimDef+0x128` |
| 4 | 4 | i32 | reserved | 0 | writer 0x5B7810 writes literal 0 |
| 8 | 4 | i32 | `relative` | 1 = keys already parent-relative (copied to `AnimDef+0x148`, suppresses the 0x5B6A90 pass) | |
| 12 | 4 | i32 | reserved | 0 | |
| 16 | 4 | i32 | reserved | 0 | |
| 20 | variable | `Track[n_tracks]` | | | |
| after | variable | trailer | same as [am2 trailer](am2.md#trailer-sections-re-0x5b5350-read-0x5b4fe0-write--per-plane-material-curves-and-event-labels) | | 0x5B4FE0 |

`Track`: `i32 n_keys` then `n_keys * 32` raw bytes = `AnimKey[n_keys]` ([AnimKey](../re/structs/AnimKey.md)): `f32 time; f32 q[4] (x,y,z,w); f32 pos[3]`.
(The reader pulls the block with one `read_block(n*0x20)`; the writer with `write_block(ptr, n*0x20)`.) No clip table: clips come from the `.am2` header
(the cache is only valid next to it).

## Worked example — `item/anim/bigred.AMT` (4,224 bytes)

```
0x000  03 00 00 00           n_tracks = 3
0x004  00 00 00 00
0x008  01 00 00 00           relative = 1
0x00C  00 00 00 00 | 0x010 00 00 00 00
0x014  10 00 00 00           track0.n_keys = 16
0x018  key0: time 0.0 | q = (0.7071, 0, 0, 0.7071) | pos = (0.0, -1.9e-6, 1.9e-21)
0x038  key1: time 0x43695555 = 233.33 | q = (0.690, -0.154, -0.154, 0.690) | pos (0, ...)   -> keys are 0x20 apart
0x218  63 00 00 00           track1.n_keys = 99 (0x218 = 0x18 + 16*0x20)
0x21C  key0: time 0.0 | q = (-9e-8, 0, 0, 1.0 [0x3f7fffff]) | pos = (0, -1e-20, 0)
```
Verified: 16 keys x 32 bytes tile exactly from 0x18 to 0x218; quaternion w is the 4th float after `time` (identity key ends in `ff ff 7f 3f`).
Inventory head of `Eng_Flag.AMT`: `01 00 00 00 | 00.. | 01 00 00 00 | 00.. | 00.. | 02 00 00 00 | ...` = 1 track, relative, 2 keys (first key identity quaternion,
pos (0.0806, -0.0173, 226.88), 2nd key time 366.667 (0x43B75555)).

## Install facts (all 16 files; `item/anim/bigred.AMT` last 128 bytes dumped)
- `bigred.AMT` (4224 B) = 20 + (4 + 16*32) + (4 + 99*32) + (4 + 16*32): the **last key ends exactly at EOF - there is no trailer, not even a 0 terminator**. This holds for all 16 files
  (`raw_trailer` empty); the reader tolerates EOF where the trailer loop expects a section id. Last key of bigred: time 0x45505555 = 3333.33, q = (~0, 0, ~0, 1.0), pos (-4.8, ..., 0).
- Header: dwords 1, 3, 4 are 0 and `relative` = 1 in every file; quaternions are unit length (|q|^2 within 1e-3).
- Track counts: tent01 8, camp_entrance 5, e_bmlb family 7 (identical 6032-byte files), bigred/mediumred/smallred 3, roastpig 3, dizzy01 6, flags 1.

## Confidence
Header and key layout: **verified** (hex). Trailer: same status as `.am2`. Cache-selection rule: read from 0x5B3100/0x5B2AA0 decompilation.
