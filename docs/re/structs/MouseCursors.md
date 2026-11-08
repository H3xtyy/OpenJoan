# MouseCursors (ilib\mousecr.cpp)

Loader 0x4915F0 (`RESOURCE\CURSOR.RES`, dBase; see [dbf.md](../../formats/dbf.md)), shutdown 0x4941B0. Blobs: `I_cursor.res`
([i_cursor.md](../../formats/i_cursor.md)).

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `active` | shutdown is a no-op when 0 | read |
| 0x01 | 1 | i8 | `n_buffers` | loop count for the 0x16-stride table below | read |
| 0x38 / 0x40 | 4 each | ptr | resource handles released at shutdown (0x596760 for +0x40) | | inferred |
| 0x7C + i*0x16 (+0x80, +0x84, +0x88, +0x8C) | 4 each | void* | per-cursor image buffers (freed one by one) | | read |
| 0xD2 | 2 | i16 | `cursor_count` | = DB row count | verified |
| 0xD4 | 4 | Cursor* | `cursors` | array of 8-byte entries | verified |
| 0x1FC | 4 | ptr | helper object freed by 0x596B30 | | inferred |
| 0x200 | 4 | i32 | `showcursor_balance` | undone with `ShowCursor` at shutdown | read |

Cursor entry (8 bytes): `i16 hot_x`, `i16 hot_y`, `u32 blob` (resdb handle returned by 0x59B830 from the 4-char key at row+0xE).
Row layout of CURSOR.RES as read by 0x4915F0 (pointer from `0x580C70(row)`, offsets relative to that pointer): hotspot x = 3 chars at
+8, hotspot y = 3 chars at +0xB, blob key = 4 chars at +0xE (the 8-char name, e.g. `NORMAL`, is at +0; see dbf.md sample).
DirectInput mouse device global: `DAT_00A52428` (IDirectInputDevice8, vtable slot 0x30 = `Unacquire`).
