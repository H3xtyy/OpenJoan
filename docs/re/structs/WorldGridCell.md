# WorldGridCell (in memory, 0x54 bytes)

`cells[x]` is an array of `(grid_h+1)` records of 0x54 bytes (allocation `(h+1)*0x54` in 0x4F2F80), `cells` has `grid_w+1`
pointers. Filled from `.gri` layers by 0x4F6EB0, consumed by the terrain builder 0x55B720. See [gri.md](../../formats/gri.md).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | sector_start | non-zero: cell origin of a scene sector | 0x4F6EB0 |
| 0x01 | 3 | u8[3] | rgb | vertex colour R,G,B | 0x55B720 |
| 0x04 | 1 | u8 | alpha | 0xFF - v = vertex alpha | 0x55B720 |
| 0x05 | 2 | ? | unknown | TODO(re) | |
| 0x07 | 2 | s16 | height | terrain height | 0x4F6EB0 |
| 0x09 | 2 | s16 | sector_idx | 1-based index into the `.gri` sector-name table | 0x4F6EB0 |
| 0x0B | 2 | s16 | tex1 | 1-based texture-name index | 0x4F6EB0 |
| 0x0D | 2 | s16 | tex2 | 1-based secondary texture index | 0x4F6EB0 |
| 0x0F | 1 | u8 | subdiv | 0, 1 (2x2), 2 (4x4) | 0x4F6EB0 |
| 0x10 | 0x20 | char[32] | tex1_name | copied from texture table | 0x4F6EB0 `strncpy` |
| 0x30 | 0x20 | char[32] | tex2_name | | |
| 0x50 | 4 | u8* | subverts | 5-byte records, 9 or 25 of them | 0x4F6EB0 |
