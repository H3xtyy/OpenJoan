# ResX (ilib\resx.cpp, sizeof = 0x135) — joan.exe

Section container: `u16 count`, `(count+1)` x `{char name[9]; u32 offset}` ([gameset.md](../../formats/gameset.md)). Derived from [File](File.md). [ResLX](ResLX.md) is the same layout with other widths.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x10E | 4 | u8* | `index` | `(count+1)*13` bytes as read from the file (entry stride 0xD, offset at +9) | 0x59C550 |
| 0x112 | 4 | u8* | `buf` | load-all: all payload bytes `[off[0], off[count])`; streaming: section cache; shared mode: scratch 0xA500A4 | 0x59C550, 0x59CBB0 |
| 0x116 | 4 | u32 | `buf_cap` | capacity of the cache block | 0x59CBB0 |
| 0x11A | 4 | u8* | `sorted` | `count` entries copied from `index`, offset replaced by original 0-based index, qsorted by name | 0x59D050 |
| 0x11E | 1 | u8 | `is_open` | | 0x59C550/0x59C780 |
| 0x11F | 1 | u8 | `load_all` | | 0x59C550 |
| 0x120 | 1 | u8 | `shared_buf` | | 0x59C550 |
| 0x121 | 4 | i32 | `count` | number of sections (sign-extended u16) | 0x59C550 |
| 0x125 | 4 | i32 | `cached` | 0-based section held in `buf`, -1 none | 0x59CBB0 |
| 0x129 | 4 | u8* | `stream_buf` | optional caller-supplied buffer (set externally) | 0x59CBB0 |
| 0x12D | 4 | u32 | `stream_cap` | its capacity | 0x59CBB0 |
| 0x131 | 4 | i32 | `stream_skip` | leading bytes of each section to skip when streaming | 0x59CBB0 |
