# PScriptToken (12 bytes) and token dynarr

Token array = dynarr (`ilib/dynarr.cpp`) with elem size 0xC, grow 0x1E; created by 0x67CAA0, vtable `PTR_LAB_009DC6C0`.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | char* | text | NUL-terminated token text (quotes kept for strings) | 0x677390 `*token` is passed to strlen/strcmp/atof |
| 0x04 | 4 | i32 | kind | 1 number, 2 `=`, 3 `,`, 4 `(`, 5 `)`, 6 `{`, 7 `}`, 8 string, 9 ParticleSystem, 0xA EventSequence, 0xB Random, 0xC XYZ, 0xE Texture, 0xF Model?, 0x10/0x12/0x13/0x14 event start, 0x17 system vec key, 0x18 numeric key, 0x19 vec key, 0x1A colour-op key, 0x1B alpha-op key, 0x1C-0x1E event value key | many sites, verified |
| 0x08 | 4 | i32 | line | 1-based source line (for error reports) | 0x6840D0 passes `line` as second arg to the emit callback |

## dynarr header (`ilib::dynarr`, verified in 0x5843D0/0x584580/0x584620/0x4B9C30/0x63EE20)
`+0 cap, +4 grow, +8 cursor (1-based, 0 = none), +0xC count, +0x10 elem size, +0x14 = -1, +0x19 data ptr` (unaligned: byte at +0x18 unused). Element i (1-based) at `data + (i-1)*elem`.
