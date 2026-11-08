# `Resource/I_cursor.res` — mouse-cursor bitmaps (`ResDb` blob file)

Reader: `ResDb::open` 0x59B6D0 (load-all) + `ResDb::get_blob` 0x59B830, caller `MouseCursors::init` 0x493EB0 / `load` 0x4915F0.
Index = [dbf.md](dbf.md) `Resource/Cursor.res` field `HBITMAPPTR`. No header of its own.

```
blob (repeated, back to back):
  u32  size          = 4 + w*h
  u16  width
  u16  height
  u8   pixels[w*h]   8-bit palette indices (palette: pal_std.md), row-major
```
Verified: file size 2064 = 2 x 1032; blob 0 at offset 0 = `04 04 00 00 | 20 00 | 20 00 | pixels` (size 0x404 = 1028 = 4 + 32*32);
`Cursor.res` record 2 points at offset 0x408 = 1032. `get_blob(offset)` returns the address after the `size` word, i.e. at `width`
(the 8-bit bitmap header is `u16 w, u16 h` — same as [Bitmap.md](../re/structs/Bitmap.md)). Colour key = **0xFF** (inferred from the data: 595 of 1024 pixels, all outside the outline; index 0 is the black outline, 60 pixels). Both blobs are pixel-identical (V).
