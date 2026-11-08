# Profile (0x77), HeroSave/Unit snapshot (0x4B4) and the `Player.dat` store (`profile.cpp`) — joan.exe

Complements [ProfileArray](ProfileArray.md) (which owns the field table of the container object, written by the gameloop family). This page adds: the exact on-disk
layout of `Save/Player.dat` (verified on the user's 114-byte sample), the per-profile record, the persisted subset of the 0x4B4 "save record", and what the other
modules copy in/out. Functions: [functions/savegame.md](../functions/savegame.md); file format: [formats/sav.md §8](../../formats/sav.md).
Confidence: V = read in >= 2 places or confirmed on the sample, R = read once, I = inferred.

## 1. What a Profile is

`ProfileArray` (global, 0x48 bytes, `profile.cpp`) is the **player's campaign carry-over store**. It holds a list of named `Profile` records. Each profile owns
a `current` snapshot and up to **20 slot snapshots** of a 0x4B4-byte *save record* ([Scenario](Scenario.md) calls the same object `hero_saves`: "copies of profile save records").
The record has the layout of the engine's **Unit** object (ctor `0x4155E0`, virtual dtor `0x471530`, unit type id at `+0x3B`, nation at `+2`, name strings at `+0x432`/`+0x473`), i.e. the page
named "SaveGame" in ProfileArray.md is really a persisted hero `Unit`. A scenario start copies the relevant snapshot(s) into `Scenario.hero_saves` (0x4C96F0 / 0x4CDA10 from `load_unit` 0x64F320);
a mission end writes them back (0x4C8810, not exported). `ProfileArray` also carries three campaign scalars:

| ProfileArray off | name | mirrors | evidence |
|---|---|---|---|
| +0x38 | `gold` (i32) | `GameState+0x734` (`DAT_00A596C4`): loaded at mission start (0x41C810), saved at mission end (0x720CA0) | 0x41C810, 0x720CA0; sample `Player.dat+0x38 = 100` (starting gold) V |
| +0x3C | `play_ms` (u32) | `GameState+0x740` (`DAT_00A596D0`) | 0x41C810, 0x720CA0 V |
| +0x40 | `aux` (i32) | `DAT_00A5B9F0` | 0x41C810, 0x720CA0, 0x4C8BB0 (last field) V |
| +0x28/0x2C, +0x30/0x34 | `list_a`, `list_b` u16 lists | `GameState.list_a/list_b` (0x4727B0/0x4728F0 mirror in, 0x4728F0/0x472C40 out) | ProfileArray.md V |

The currently selected profile is `GameConfig.profile_index` (1-based, default 1; `0x4CA290` returns the record; the default profile name is the config `player_name`, `"Player"`).

## 2. Profile record (sizeof 0x77 = 119, `profile_add` 0x4C80C0)

| off | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 0x21 | char[33] | `name` | NUL-padded, `strncpy(name, n, 0x21)` on add; the file stores only the first **0x20** bytes (`char[0x20]` blocks) | 0x4C80C0, 0x4C9420, 0x4C9530 V |
| 0x21 | 4 | Unit* | `current` | latest snapshot (allocated `new 0x4B4` + ctor 0x4155E0 when the file has one) | 0x4C9530 V |
| 0x25 | 2 | i16 | `slot_count` | 0..20 | 0x4C9530, 0x4C7E70 V |
| 0x27 | 80 | Unit*[20] | `slots` | 1-based in the file loop (`slots[k-1]`) | 0x4C9530 V |

`profile_array_add` (0x4C80C0): `count++`, `realloc(records, count*0x77)` (tracked, `profile.cpp`), zero the new record, copy the name; returns the pointer to the new record.
`profile_array_free` (0x4C7E70): for every record delete `current` and the 20 slot pointers with the virtual dtor, free the record array and the two u16 lists, zero counters.

## 3. `Save/Player.dat` on-disk layout (non-packed file) — all offsets verified on the 114-byte sample

```
0x00  0x48  raw ProfileArray[0..0x48)      sample: "Player" @0, i32 100 @0x38, everything else 0
0x48  0x20  head (live name, char[0x20])   sample: "Player" + zeros
0x68  4     i32 count
        count x char[0x20]  names                       (0x4C9420, first pass)
        count x { char[0x20] name; Snapshot current; i16 slot_count; slot_count x Snapshot }    (0x4C9530 + 0x4C9930, second pass)
      i16 na; na x u16   (list_a)
      i16 nb; nb x u16   (list_b)
      i16 aux
```
Sample: 0x48 + 0x20 + 4 + 2 + 2 + 2 = 0x72 = 114 bytes ⇒ `count = 0`, `na = nb = 0`, `aux = 0` (bytes 0x68..0x71 all 0/1). The header-only load (0x4C8490) stops after the name pass and zeroes the lists.
The first 0x48-byte read overwrites the object fields `+0x20..+0x47` (count, records*, list pointers); the second read restores `head`; the real `count` is the `i32` after it. Because list/aux
fields are read afterwards unconditionally the stale file values in `+0x28..+0x40` never survive (0x4C7E70 clears them first), except `+0x38` (gold) and `+0x3C` (play_ms) which are **only** present in the first block.
In-memory file name: `"SAVE\" + head + DAT_009D56F0`; `0x590B80` is the exists-check (absent file → load fails, a fresh store is used).

The `.SAV` (section 14, formats/sav.md §5) embeds the same structure with `U32` instead of `i16` for `na`, `nb`, `aux` and packed `W()` framing.

## 4. Snapshot (the persisted part of a 0x4B4 Unit record)

Reader `savegame_read_body` (0x4C9930):
1. `R(0x432)` into the object (raw image; `+0..3` is the vptr of the saving process).
2. `R(0x3B)` and the 4-byte-slot list (`0x47E600`, `0x47E700`: `R(4)`; if non-zero `U32 n; n x R(8)`).
3. If the pointer slot `+0x1C1` was non-zero in the image: `new 0x1E` (ctor 0x495950) and `0x47E9F0` (inventory-like sub-objects: 2 x `W(0x12)`, `+0xA`, `+0xE`, `3 x W(0x3E)` matrices).
4. Runtime reset `0x47E300` (clears `+0x22,+0x26`, re-derives `+0x299` from the unit_res table, zeroes `+0x2ED..+0x357` runtime counters), then the explicit resets listed in ProfileArray.md
   (`+0x02,+0x04,+0x06,+0x0A,+0x2E,+0x213,+0x215=1,+0x21B=1,+0x21D,+0x227=0xFF,+0x233,+0x235,+0x351,+0x355,+0x356,+0x1BD,+0x1BF,+0x1E2,+0x1E6,+0x2A1,+0x2D7..+0x2DD,+0x378,+0x37C,+0x3E5=1,+0x3F6=+0x3FA=DAT_00A3CD04`), `0x422F10`.
   Note the sub-objects at `+0x33` (0x58), `+0x2A1` (0x3E) and the per-type arrays `+0x378/+0x37C` are **not** read from a profile snapshot (`+0x2A1`, `+0x378`, `+0x37C` are zeroed; `+0x33`/`+0x335` keep whatever raw pointer value the image held and must be ignored), unlike the in-save Unit record (formats/sav.md §4.2).

## 5. Open points
* `TODO(re)` writer `profile_save` 0x4C8810 and the per-snapshot writers 0x4C93E0/0x4C9460/0x4C96F0 (not exported).
* `TODO(re)` meaning of `DAT_00A3CD04` (stored into `+0x3F6/+0x3FA` after a snapshot load; likely "now").
* `TODO(re)` whether `slots[k]` is indexed by scenario number or by mission; the loop only proves 1-based k ≤ 20.
