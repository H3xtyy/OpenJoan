# DbField (dBase III field descriptor, 32 bytes on disk and in memory)

Array at `Db+0x13F`; loaded only when `load_fields` is set. Verified on `Resource/Cursor.res` descriptors at file offsets 0x20, 0x40, 0x60, 0x80.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 11 | char[11] | `name` | NUL padded (`HFILENAME`, `HHOTSPOT_X`, `HHOTSPOT_Y`, `HBITMAPPTR`) | SAMPLES.md Cursor.res 0x20.. |
| 0x0B | 1 | char | `type` | `C` character, `N` numeric (ASCII digits, right aligned, space padded) | Cursor.res 0x2B/0x4B; writer 0x581DF0 tests `'N'` |
| 0x0C | 4 | u32 | `offset_in_record` | byte offset of the field inside a record **counting the delete flag** (1, 9, 0xC, 0xF in Cursor.res) | Cursor.res 0x2C.. |
| 0x10 | 1 | u8 | `length` | field width (8, 3, 3, 4); for `C` the writer treats bytes 0x10..0x11 as a u16 length | Cursor.res; 0x581DF0 |
| 0x11 | 1 | u8 | `decimals` | 0 in all samples | |
| 0x12 | 14 | u8[14] | reserved | zero | Cursor.res |

The runtime readers never look descriptors up by name: they use hard-coded offsets (offset_in_record - 1 from the pointer returned by `Db::get_record`).
