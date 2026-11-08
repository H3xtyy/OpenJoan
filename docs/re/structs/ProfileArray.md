# ProfileArray / Profile / SaveGame (`profile.cpp`) — joan.exe

Player profiles and their save slots. The files live under `SAVE\` (path assembled with the ilib string helpers 0x5A1170/0x5A1440/0x5A1550 from `"SAVE\\"` +
a name held in the object + literal `DAT_009D56F0`; exact extension `TODO(re)`: `DAT_009D56F0` not exported). Reading uses the ilib `File` wrapper
(0x586DA0 open, 0x5873A0 `read(buf,n)`, 0x587E20 `read_i32`, 0x587C20 `read_i16`, 0x587650 `read_array(dst,count,elem_size)`).
Functions: [gameloop.md §4](../functions/gameloop.md). Related: [GameState](GameState.md) (`list_a/list_b` working copies), [GameConfig](GameConfig.md) (`profile_index`, `player_name`).

## ProfileArray (global; fields proven by 0x4C7E70, 0x4C80C0, 0x4C8490, 0x4C8BB0)

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 0x20 | u8[0x20] | `head` | first 0x20 bytes of the file after a 0x48-byte block that is read into the same place (so only the second read survives) — likely a tag/name | read |
| 0x20 | 4 | i32 | `count` | number of profiles | verified |
| 0x24 | 4 | Profile* | `records` | `count * 0x77` bytes | verified |
| 0x28 | 4 | i32 | `list_a_count` | u16 elements (read as i16) | verified |
| 0x2C | 4 | u16* | `list_a` | persisted copy of `GameState.list_a` | verified |
| 0x30 | 4 | i32 | `list_b_count` | | verified |
| 0x34 | 4 | u16* | `list_b` | persisted copy of `GameState.list_b` | verified |
| 0x40 | 4 | i32 | `aux` | read as i16 as the last item of the full load | read |

Load (full, 0x4C8BB0): file = `[0x48 raw][0x20 raw][i32 count][count x 0x20 name][per profile: 0x20 name + saves][i16 na][na x u16][i16 nb][nb x u16][i16 aux]`.
The header-only variant 0x4C8490 stops after the names and zeroes the lists.

## Profile (0x77 bytes, `profile_add` 0x4C80C0)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 0x21 | char[0x21] | `name` | NUL padded; `strncpy(name, n=0x21)` |
| 0x21 | 4 | SaveGame* | `current` | auto/current game (0x4B4-byte object, ctor 0x4155E0) — freed with virtual dtor (0x471530) |
| 0x25 | 2 | i16 | `slot_count` | 0..20 |
| 0x27 | 80 | SaveGame*[20] | `slots` | `slots[k-1]` at `+0x27+4(k-1)`; k is 1-based in the loader |

## SaveGame (0x4B4 bytes; fields reset after load by 0x4C9930)

**Identification:** the "SaveGame" object is a **`Unit`** record, not a separate save container. Its constructor `0x4155E0` is the unit constructor (factory `0x51DFC0`, init `0x4F8550`, destroy `0x461F10`), and the `0x432`-byte raw image written here is the persistent prefix of a unit. Complete field table: [Unit.md](Unit.md); the reset list below (`+0x1E2/+0x1E6` mount/rider, `+0x213..+0x21D` action state, `+0x227/+0x233/+0x235` combo buffer, `+0x2D7..+0x2DD` target recnos, `+0x378/+0x37C` crew arrays, ...) are exactly the runtime-only unit fields described there. The `Profile.current` and `slots[]` pointers therefore hold unit records (inferred: the profile's hero/save-slot units), loaded with the unit save code.

The body is **0x432 raw bytes** read straight into the object (file format = memory image; a `docs/formats/sav` spec belongs to the save-game owner), followed by nested
readers 0x47E600, 0x47E700 and, only when the pointer slot `+0x1C1` was non-zero in the image, a freshly allocated 0x1E-byte object (ctor 0x495950, reader 0x47E9F0).
After reading, runtime-only fields are forced to: `+0x02,+0x04,+0x06 (u16)=0`, `+0x0A (i32)=0`, `+0x2E (u8)=0`, `+0x1BD,+0x1BF (u16)=0`,
`+0x1E2,+0x1E6 (i32)=0`, `+0x213 (u16)=0`, `+0x215=1`, `+0x21B=1`, `+0x21D=0` (u16s), `+0x227 (u8)=0xFF`, `+0x233 (u8)=0`, `+0x235 (u16)=0`,
`+0x2A1 (i32)=0`, `+0x2D7,+0x2D9,+0x2DB,+0x2DD (u16)=0`, `+0x351 (i32)=0`, `+0x355,+0x356 (u8)=0`, `+0x378,+0x37C (i32)=0`, `+0x3E5 (u8)=1`,
`+0x3F6,+0x3FA (i32)=DAT_00A3CD04` (a global tick/time value, `TODO(re)`), then 0x422F10 (`TODO(re)`).
