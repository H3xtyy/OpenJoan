# Db (ilib\db.cpp, sizeof = 0x147) — joan.exe

In-memory view of a dBase III table ([dbf.md](../../formats/dbf.md)). Derived from [File](File.md) (0x00..0x10D).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x10E | 1 | u8 | `version` | header byte 0 (`0x03`) | 0x580880 reads 32 bytes into +0x10E |
| 0x10F | 3 | u8[3] | `date_ymd` | year-1900, month, day (Cursor.res: 67 05 03 = 2003-05-03) | dbf header |
| 0x112 | 4 | u32 | `n_records` | `Db::record_count` returns it | 0x415280, 0x580C70 |
| 0x116 | 2 | u16 | `header_len` | bytes before the first record (flag byte of record 1 is at this offset) | 0x580C70 seeks `header_len+1`; Cursor.res = 0xA1 |
| 0x118 | 2 | u16 | `record_len` | including the 1-byte delete flag | 0x580C70; Cursor.res = 0x13 |
| 0x11A | 20 | u8[20] | `reserved` | rest of the 32-byte header | |
| 0x12E | 4 | u8* | `rec_buf` | file mode: one-record buffer (`record_len` bytes) | 0x580880, 0x580C70 |
| 0x132 | 4 | i32 | `cur_record` | default record for `get_record(<1)`; init 1 | 0x580880 |
| 0x136 | 4 | i32 | `cached_record` | record held in `rec_buf`, -1 none | 0x580730 |
| 0x13A | 4 | u8* | `records` | memory mode: points at record 1's first **field** byte (`image + header_len + 1`); 0 = file mode | 0x580AE0, 0x580C70 |
| 0x13E | 1 | u8 | `owns_records` | `records` is a heap block to free | 0x580DA0 |
| 0x13F | 4 | DbField* | `fields` | descriptor array (32 B each) or 0 if not loaded | 0x580880 |
| 0x143 | 4 | i32 | `n_fields` | number of descriptors (counted up to the 0x0D terminator) | 0x580880 |
