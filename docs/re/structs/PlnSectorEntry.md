# PlnSectorEntry (on-disk, 0x28 bytes) — `.pln` sector table entry

See [../../formats/pln.md](../../formats/pln.md). RE: joan.exe 0x55F320 (reader), 0x55B720 (writer).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 0x22 | char[34] | name | scene sector name (`scene_sector_N`) | S1l1.pln 0x10; 0x55F320 `strcmp` |
| 0x22 | 2 | u16 | sector_index | 1-based world sector index | 0x55F320 reads `+0x22` as short; S1l1.pln 0x32 = 0x0D |
| 0x24 | 4 | u32 | offset | absolute file offset of the sector blob (sentinel entry = EOF) | 0x55F320 seeks `+0x24`; S1l1.pln 0x34 = 0x2838 |

`sizeof` = 0x28 (`(n+1)*0x28` allocation at 0x55F320 and 0x55B720).
