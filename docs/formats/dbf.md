# dBase III tables (`Resource/Cursor.res` and every table section inside `Resource`, `Std.set`, `Interfac.set`)

Reader: `ilib\db.cpp` `Db::open` **0x580880**, `Db::open_memory` 0x580AE0, `Db::get_record` 0x580C70; writer helper 0x581DF0 (joan.exe).
Structs: [Db.md](../re/structs/Db.md), [DbField.md](../re/structs/DbField.md). Functions: [functions/resdb.md](../re/functions/resdb.md).
Standard dBase III layout, no memo file, no index. Files that are dBase tables on disk: `Cursor.res` only; all others live inside containers ([gameset.md](gameset.md)).

## Header (32 bytes)

| offset | size | name | value / meaning |
|---|---|---|---|
| 0 | 1 | version | `0x03` (dBase III, no memo) |
| 1 | 3 | date | year-1900, month, day of last update (Cursor.res: `67 05 03` = 2003-05-03) |
| 4 | 4 | n_records | u32 (Cursor.res: 2) |
| 8 | 2 | header_len | u16 = 32 + 32*n_fields + 1 (Cursor.res 0xA1 = 161) |
| 10 | 2 | record_len | u16 including the delete-flag byte (Cursor.res 0x13 = 19) |
| 12 | 20 | reserved | zero |

## Field descriptors (n_fields x 32 bytes, starting at 0x20)

Terminated by a single `0x0D` byte at `header_len - 1`. Layout of one descriptor: see [DbField.md](../re/structs/DbField.md):
`char name[11]` (NUL padded), `char type` (`C`/`N`), `u32 offset_in_record` (field start counting the flag byte, i.e. 1, 9, 0xC, 0xF...),
`u8 length`, `u8 decimals`, 14 reserved. The runtime counts descriptors until the first byte is `0x0D` (0x580880).

## Records (from file offset `header_len`)

Each record: 1 flag byte (`0x20` live, `0x2A` deleted) + fields concatenated, `record_len` bytes total. **The game ignores the flag**: `get_record(n)`
returns a pointer to the first field byte, `base + header_len + 1 + record_len*(n-1)` (0x580C70). The file ends with `0x1A`. In file mode a record read pulls
`record_len` bytes (one over-read into the next flag); in memory mode the image is not copied.
- `C` fields: ASCII/cp1250 text, left aligned, space padded (no NUL). Joan trims trailing spaces (0x590210) — `trim_right` semantics.
- `N` fields: ASCII digits right aligned, space padded; joan converts with `atoi` after copying at most 0x78 bytes (0x590270). Empty = 0.
- **`C` field abused as binary** (`HBITMAPPTR` C4): four raw bytes = u32 little-endian offset, not text (see below).
- Record numbers are 1-based; `n < 1` means "current record" (initially 1); `n > n_records` yields NULL.

## Worked example: `Resource/Cursor.res` (200 bytes)

```
0x00  03 67 05 03 | 02 00 00 00 | a1 00 | 13 00 | 00*20           version, 2003-05-03, 2 records, header 161, record 19
0x20  "HFILENAME\0\0" 'C' 01 00 00 00  08           field 1: C8 at record offset 1
0x40  "HHOTSPOT_X\0"  'N' 09 00 00 00  03           field 2: N3 at 9
0x60  "HHOTSPOT_Y\0"  'N' 0c 00 00 00  03           field 3: N3 at 12
0x80  "HBITMAPPTR\0"  'C' 0f 00 00 00  04           field 4: C4 at 15
0xa0  0d                                            descriptor terminator (header_len-1)
0xa1  20 "NORMAL  " "  2" "  2" 00 00 00 00         record 1: hotspot (2,2), bitmap blob at I_cursor.res offset 0
0xb4  20 "WAIT    " "  2" "  2" 08 04 00 00         record 2: hotspot (2,2), blob offset 0x408 = 1032
0xc7  1a
```
Field offsets in the pointer returned by `get_record`: HFILENAME +0 (8), HHOTSPOT_X +8 (3), HHOTSPOT_Y +0xB (3), HBITMAPPTR +0xE (4)
(reader 0x4915F0). Blob offsets address [i_cursor.md](i_cursor.md).

## How the game uses field names
It does not: every table reader (SDATE 0x4CE0F0, RACE 0x4CA480, ANIMATE 0x4172A0, cursors 0x4915F0, `BmpRes::load` 0x620BC0, HEADER 0x58AE90) uses hard-coded
row offsets. Field names are documentation (and for editors). A reader written from this page should read the descriptors and check that
`type/length/offset` match the expected schema before using named columns.

## Writer (tool only)
0x581DF0 adds one column: `header_len = (n_fields+1)*0x20 + 0x21`, `record_len += width`, writes the old descriptors, the new one, `0x0D`, then every
record as `0x20` + old data + `width` spaces, then `0x1A`. Date bytes are copied unchanged. For `C` fields the length is stored as u16 over descriptor bytes 0x10..0x11.
Not needed at run time.

## Implementation notes
- Validate: `32 + 32*n + 1 == header_len`, offsets contiguous, `sum(lengths)+1 == record_len`, `file_size == header_len + n*record_len + 1` (Cursor.res: 161 + 38 + 1 = 200).
- Section images inside containers have the same layout; the size of the section = `header_len + n*record_len + 1` (HEADER section of `Resource`: 97 + 1*69 + 1 = 167).
- Keep numeric fields as strings + parsed int; do not re-format on write.

## Verified deviations (all 239 embedded tables + Cursor.res)
- **Descriptor offset (+0x0C) may be 0.** 119 sections have 0 in every descriptor (tables last saved by the Enlight editor); the others store the
  contiguous offset (1, 9, ...). The game ignores it (hard-coded row offsets). Both 0 and the contiguous value occur.
- **The trailing 0x1A is optional**: image size is `header_len + n*record_len` or that +1.
- All tables: version byte 3, only `C`/`N` columns, descriptor terminator 0x0D at `header_len-1`, `record_len == 1 + sum(widths)`. Update dates in the
  header are not meaningful (some read as year 1902). `N` fields with decimals exist (e.g. `GAIN N6.3` in Std.set `SOUND`); `SOUNDTIME1/2` are `N19`.
- `HBITMAPPTR` in `Cursor.res` is a binary `C4` field, to be read as a u32.

## Container tables and scratch fields

Container tables: `STD.SET` holds 140 sections (one per table, plus the per-unit-kind animation-state tables), `INTERFAC.SET` the UI layout
tables and `Resource` 20 (counts in [gameset.md](gameset.md); `INTERFAC.SET` has 81 sections). Some `C`
fields (for example `HBITMAPPTR`) are scratch space that the engine fills with pointers at run time; they hold no meaningful data on disk.
Animation-state tables (one per unit kind) carry the columns `ANIM_CODE`, `POSITION`, `MOVEMENT`, `COMBO`, `ENERGY_PT`, `DAMAGE`,
`BLEND_TIME`, `START_TIME`/`END_TIME`, `UPPERFILE`, `SCRIPT` and `SOUND...`; section "What the sections contain" in
[gameset.md](gameset.md) maps the readers.
