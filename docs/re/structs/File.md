# File (ilib file wrapper, sizeof = 0x10E) — joan.exe

Win32 file handle wrapper; base of ResDb, ResX, ResLX, Db, FileTxt. Methods: [functions/resdb.md](../functions/resdb.md) (0x4179C0 ctor, 0x586DA0 open, 0x5873A0 read ...).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x000 | 0x105 | char[261] | `path` | path as passed to `open` (**without** the install-dir prefix); max strlen 0x104 | 0x586DA0 (`strcpy(this, path)`, length check `> 0x104`) |
| 0x105 | 4 | HANDLE | `handle` | `INVALID_HANDLE_VALUE (-1)` when closed (ctor 0x4179C0, close 0x587260) | 0x4179C0, 0x587260 |
| 0x109 | 4 | u32 | `report_errors` | nonzero: read/write failures raise `fatal_error` / Retry box | 0x587E20, 0x5873A0 |
| 0x10D | 1 | u8 | `packed` | length-prefixed ("packed") read/write mode, see `File::read` | 0x5873A0, 0x5872E0 |

Derived classes start their own fields at 0x10E. The install-dir prefix is `*(char**)0xA1579C` (string pointer), applied only by `open`, not by `create`.
