# ResDb (ilib\resdb.cpp, sizeof = 0x124) — joan.exe

Blob file addressed by externally stored offsets (`I_cursor.res`). Derived from [File](File.md).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x10E | 1 | u8 | `is_open` | | 0x59B6D0 (=1), 0x59B440 |
| 0x10F | 4 | u32 | `field_10F` | zeroed on open, unused here | 0x59B6D0 |
| 0x113 | 4 | u32 | `rec_ptr_off` | byte offset in a Db row of the 4-byte blob offset; set by callers (not in export) | 0x59B4F0 |
| 0x117 | 4 | u32 | `shared_buf` | streaming mode: use the global scratch (0xA500A4) as buffer | 0x59B6D0 |
| 0x11B | 4 | u8* | `buf` | load-all: whole file; streaming: current blob | 0x59B830 |
| 0x11F | 4 | u32 | `buf_size` | file size (load-all) / last blob size | 0x59B6D0, 0x59B830 |
| 0x123 | 1 | u8 | `load_all` | whole file resident, handle closed | 0x59B6D0 |

Blob in the file: `u32 size`, then `size` bytes; `get_blob(off)` returns the address of the first byte after the size.
