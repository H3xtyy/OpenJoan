# `Resource/PAL_STD.RES` — standard 256-colour palette

Reader: `MouseCursors::init` 0x493EB0 (opens the file with `File`, `seek(8)`, reads 0x300 bytes into a 256 x 3 table, builds a `vgabuf` palette object 0x612600).

```
0x000  u32  0x00000308      = file size (776) — verified (the reader seeks past it)
0x004  u32  0x0000B123      unknown (TODO(re): checksum / id; the reader skips it; 0xB123 in the install)
0x008  u8[768]  256 x 3 bytes palette entries (channel order **undecided**: R,G,B renders the I_cursor gauntlet gold, B,G,R steel-blue; TODO(re): vgabuf code 0x612600)
```
File size 776 = 8 + 768 (verified). Entry 0 is `00 00 00`. Values reach 0xFF (8-bit, not 6-bit VGA). Used for the 8-bit cursor bitmaps ([i_cursor.md](i_cursor.md)).

Entries 1..8 run `00002b, 00133f, 003753, 006763, 007b4b, 008f47, 00a743, 00bb37`; index 255 is `000000`.
