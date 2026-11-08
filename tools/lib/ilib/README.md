# tools/lib/ilib (`joan_ilib`)

Readers for the formats of the original engine's `ilib` layer (the `d:\ww\ilib\*.cpp` files of the original game),
written field by field so they are portable and throw on malformed input. Static library on `joan_core`. The in-game
equivalents live in `src/ilib/`; formats are specified in `docs/formats/`.

| File | Reads | Original |
|---|---|---|
| `res_archive.hpp` / `.cpp` | `.RES` / `.SET` / `RESOURCE` archives with a short (9-byte names, `u16` count) or long (34-byte names, `u32` count) index; also writes archives (used by `jres pack`) | `ResourceIdx` 0x0059c550.., `ResourceIdxL` 0x0059b920.. |
| `database.hpp` / `.cpp` | dBase III tables (`.dbf`): header, field descriptors, records | `Database` 0x00580880..0x00582350 |
| `game_set.hpp` / `.cpp` | game sets: archives whose entries are dBase tables (`Resource/STD.SET`, `INTERFAC.SET`), each with a `HEADER` table | `GameSet` ~0x0058ae30 |
| `kv_text.hpp` / `.cpp` | `Key : value` definition files (`scenario/*.scr`, `Resource/*.pro`, `map/*.sca`, `map/*.lgt`) with `#` and `//` comments and blank-line separated records | (several loaders) |

Used by `jres`, `jdbf` and `tests/test_ilib.cpp`.
