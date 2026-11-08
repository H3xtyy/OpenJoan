# GameSet / ResourceSet manager (ilib\gameset.cpp, sizeof >= 0x286) — joan.exe

One global object: the list of `Resource\*.SET` files plus the currently opened set (a [ResX](ResX.md) and one embedded [Db](Db.md)).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x000 | 1 | u8 | `initialised` | set by scan, tested by `free` | 0x58AE30 |
| 0x001 | 2 | u16 | `current_set` | 1-based index chosen by `select` | 0x58B080 |
| 0x003 | 2 | u16 | `set_count` | number of `*.SET` files | 0x58AE90 |
| 0x005 | 4 | SetInfo* | `sets` | `set_count` x 0x46 bytes | 0x58AE90 |
| 0x009 | 0x135 | ResX | `res` | the open `.set` (+9 = the GameSet header above ends here) | 0x58B140 -> ResX::close, 0x58B180 |
| 0x13E | 0x147 | Db | `table` | the table opened by `open_table(name)` | 0x58B180 returns `this+0x13E` |
| 0x285 | 1 | u8 | `set_open` | 1 after `select`, 0 after `close_set` | 0x58B080, 0x58B140 |

SetInfo (0x46): `char name[9]` (HEADER field 1, rtrim) at +0; `char title[0x3D]` (HEADER field 2, rtrim) at +9. Filled from record 1 of each set's `HEADER` table; `select` builds the path `RESOURCE\` + name + extension literal 0x9D8AC8.
