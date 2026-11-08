# `.map` — named map-marker list (`map/scenarioN/*.map`, 37 files, 4..1,808 B)

**Reader not identified** (no code references the literal `.map`, nor `.pfd`, `.gri` or `.nod`: the extension is built dynamically). The layout
below is derived from the real files; field **boundaries are verified on all 37 files**,
the **meaning** of the fields is inferred.

## Layout
`u32 count` followed by `count` fixed records. Record size = `(file size - 4) / count`: **164 (0xA4)** in 36 files, **162 (0xA2)** in
`map/scenario2/S2L1w.map` (328 B, 2 records: the trailing padding is 4 instead of 6 bytes; all other fields identical). `S1L1w.map` is the 4-byte file `00 00 00 00`
(count 0). 168 markers in total.

| rec off | size | field | evidence |
|---|---|---|---|
| 0x00 | u16 | `len1` = 0x40 | every record |
| 0x02 | char[0x40] | `name` | instance name, NUL padded, all ASCII, never empty: `fcastle_s1l1`, `fcampS1L1`, `ecampS1L1` |
| 0x42 | u16 | `len2` = 8 | every record |
| 0x44 | char[8] | `kind` | value set: `fcamp`(24) `e_camp`(24) `e_town`(19) `fcastle`(14) `fpotion`(13) `fweapon`(12) `Ftown`(12) `fbarrack`(11) `ftown`(11) `e_castle`(10) `ironmill`(7) `sawmill`(4) `Fpotion`(1) `FWeapon`(1) `Fcamp`(1) `workshop`(1) `siege`(1) — `f…` French side, `e…` English side, building/facility kinds otherwise; note inconsistent capitalisation, compare case-insensitively |
| 0x4C | u32 | reserved = 0 | every record |
| 0x50 | u16 | `len3` = 0x40 | every record |
| 0x52 | char[0x40] | `extra` | empty in all 168 records |
| 0x92 | i32 | `x` | world units, 0..~17,000 (S1L1 rec0 = 8116) |
| 0x96 | i32 | `y` | world units, 0..~17,000 (rec0 = 726) |
| 0x9A | i32 | `z`/param | 0..2500; most frequent 500 (65), 1500 (13), 0 (11), 200 (11); castle 2000 vs camp 170 in S1L1; resembles a radius or influence range, TODO(re) |
| 0x9E | 6 B (4 B in the 162-byte variant) | padding | zero in every record |

The `len` fields behave like length-prefixed fixed buffers (u16 size, then that many bytes), the same idiom as the
`gfile3` `read_u16`+`read_bytes` pairs (0x483CA0). The coordinates of the `fcamp`/`ecamp` rows (12000, 14000) match the
`Camp 1 Place` entry `X:11998.81 Y:13652.75` of `scenario1.res`'s `S1L1_PLA` text, so these are world-space marker
positions in the same units as `.uni`/`.pla` (world units, 64 per terrain cell). The name column does not match any `.pla`
`PLACE_NAME`, so it is probably an icon/marker id for the strategic or mini map.

## Open questions
- TODO(re): locate reader (search for a `count`-then-`0xA4` loop reading `0x40`/`8`/`0x40` byte fields; candidate family: `world_s.cpp` minimap, `interfac`). Needs a new export. The 162-byte record of S2L1w.map suggests the reader is length-prefix driven (it may read exactly `0x9E` bytes + a variable tail) rather than using a fixed stride.
- TODO(re): meaning of `z`/param and of `kind` (which building/icon each denotes).
