# 2D bitmap / image records (`ilib\bitmap*.cpp`, `en2d\image2d.cpp`, `en2d\bmpres.cpp`) and the `.bmp` / `.tga` files

Reference build joan.exe. Function table: [fin.md](../re/functions/fin.md); structs: [Bitmap](../re/structs/Bitmap.md).
Legend: **V** verified (decompiled or real bytes), **I** inferred, **TODO(re)**.

**Headline: none of the analysed readers parses a `.bmp` or `.tga` file.** The family is three unrelated things:

1. `ilib\bitmap.cpp` / `bitmapd.cpp` — in-memory 8-bit / 32-bit raster buffers `{u16 w, u16 h, pixels}` (V, allocators only).
2. `en2d\image2d.cpp` — a UI image object (name string + flags); no I/O in its two analysed methods (V).
3. `en2d\bmpres.cpp` — a table of bitmap *descriptors* read from the `INTERFAC` resource DB (V), not pixels.

`.bmp` / `.tga` / `.dds` pixel data is decoded by Direct3DX (`D3DXCreateTextureFromFileEx`, 0x765AAB/0x765C66) behind the
texture manager 0x6099C0; the D3DX8 texture loaders are ported in `src/lib/d3dx8`.

## 1. In-memory bitmap buffers (V)

| type | allocation | layout | evidence |
|---|---|---|---|
| Bitmap (8 bpp) | `w*h + 4` bytes | `u16 width; u16 height; u8 pixels[w*h]` | 0x57F080 (alloc 0x57F050 = `w*h+4`, header store 0x57EF20) |
| Bitmap32 | `w*h*4 + 4` bytes | `u16 width; u16 height; u32 pixels[w*h]` | 0x57F230 (0x57F200, 0x57F0F0) |

Width and height are signed 16-bit in the allocator arguments. The "+4" is exactly the 4-byte header. These buffers are used
for the 8-bit mouse cursors (below) and as scratch rasters; no file format is attached to them by these functions.

### `Resource/I_cursor.res` and `Cursor.res` (mouse cursors; reader is `ilib\mousecr.cpp`, not analysed) — I

`Resource/I_cursor.res` is **2,064 bytes = 2 x 1,032** (V): each record is
`u32 size = 0x00000404` (= 32*32+4, the byte length of the `Bitmap` that follows, prefix excluded) + `Bitmap { u16 32, u16 32,
u8 pixels[1024] }`. The second record starts at 0x408. The first pixel row starts at file offset 8: `00 ff ff ff …` — the
background is **0xFF** and 0x00/other values form the outline/body, so the transparent palette index is most likely **0xFF** (I).
Pixel values are indices into the 256-entry palette. The two records are **pixel-identical** (V; both show the same pointing gauntlet), `Cursor.res` indexes them by offset. Colour key: **0xFF** (V: 595 of 1024 pixels are 0xFF and form exactly the area outside the glove outline; palette entry 255 is black, so it only works as a key). TODO(re): hot spot (`Cursor.res`, see dbf.md).

### `Resource/pal_std.res` (V layout, I colour order)

776 bytes: `u32 0x308` (= 776 = file size; same "size prefix" idea as the cursor, here the prefix is counted: 4 + 772), `u32 0x0000B123`
(TODO(re): id/checksum), then **768 bytes = 256 x 3** colour triplets starting at offset 0x08 (`00 00 00`, `00 00 2b`, `00 13 3f`,
`00 37 53`, `00 67 63`, … a dark-to-bright ramp whose third byte is the largest at the start — consistent with **B,G,R** order, I).
The reader is not analysed; values are 8-bit. Channel order still undecided: rendered as R,G,B the cursor is a gold gauntlet, as B,G,R a steel-blue gauntlet (both plausible, see pal_std.md).

## 2. Image2D (`en2d\image2d.cpp`) — V (partial)

Methods analysed: `reset` 0x629830 (zero two fields, free name, clear flag at +0x70) and `set_name` 0x6298C0 (heap copy of the
file/resource name at +0x94). The image object stores *only a name* here; pixels come from the texture manager by that name.
Fields and offsets in [structs/Bitmap.md](../re/structs/Bitmap.md). TODO(re): remaining methods (draw, blit, hit-test) not analysed.

## 3. BmpRes descriptor table (`en2d\bmpres.cpp`) — V structure, TODO field meaning

`BmpRes::load(table_name)` (0x620BC0):

1. Make sure the `INTERFAC` resource is the current DB (looked up once in the resource index with `ResDir::find`, cached in
   `DAT_00A69D0C`; the previously active resource is restored when done). `INTERFAC` resolves to `RESOURCE\<entry>.RES`
   through the 0x46-byte-per-name index in `Resource/Resource` (see the future `res` spec).
2. Open table `table_name` (0x59C950); `count` = number of rows (`db+0x112`).
3. Allocate `count * 0x38` bytes, zero them, then for row `i` (1-based) fill a 0x38-byte entry:

| entry offset | size | source (row offset, width) | conversion |
|---|---|---|---|
| 0x00 | 9 | +0x00, 8 chars | copy, trim trailing spaces, NUL (0x590210) |
| 0x09 | 0x21 | +0x08, 0x20 chars | same (image name) |
| 0x2A | 2 | +0x28, 4 chars | `atoi` (0x590270: copy, NUL, `atoi`; max 0x78 chars) |
| 0x2C | 2 | +0x2C, 4 | `atoi` |
| 0x2E | 2 | +0x30, 4 | `atoi` |
| 0x30 | 2 | +0x34, 4 | `atoi` |
| 0x32 | 2 | +0x38, 1 | `atoi` (single digit flag) |
| 0x34 | 2 | +0x39, 4 | `atoi` |
| 0x36 | 2 | +0x3D, 4 | `atoi` |

So a `.RES` DB row here is **fixed-width text** (name 8 + file 32 + numeric fields of width 4/4/4/4/1/4/4 = 65 = 0x41 bytes
minimum; actual row stride is the DB's `+0x118` field). The five-to-seven numbers are most likely `x, y, w, h` of a sub-rectangle
in the named image plus flags — TODO(re): confirm against a hex dump of the `INTERFAC` table and the draw code. The `.RES`
container framing (13-byte offset-table entries, 0x20-byte table name, header length at `db+0x116`, 0x0D line terminator
marker in `ilib\db.cpp` 0x580AE0) belongs to the `res` format spec.

## 4. `.bmp` files (texture/misc, 106 files in the install per the census) — V sample

Standard Windows BMP, decoded by D3DX. Example `texture/misc/hand.bmp` (INVENTORY hex):
`42 4d | 38 30 00 00 | 00 00 00 00 | 36 00 00 00 | 28 00 00 00 | 40 00 00 00 | 40 00 00 00 | 01 00 | 18 00 | 00 00 00 00 …`
= `BM`, file size 0x3038 (12,344), pixel offset 0x36 (54), BITMAPINFOHEADER (40), 64 x 64, 1 plane, **24 bpp**, compression 0
(BI_RGB), then bottom-up BGR rows (rows padded to 4 bytes; 64*3 = 192 already aligned). 54 + 64*64*3 = 12,342 ≠ 12,344: the file is
2 bytes longer than header+pixels (trailing pad) — harmless; use the header's offset and dimensions.
Second sample `texture/misc/archer_att.bmp`: 24,632 bytes, `42 4d 38 60 00 00` (bfSize 0x6038 = file size), offset 0x36,
**64 x 128** (`40 00 00 00`, `80 00 00 00`), 24 bpp, biSizeImage 0, 2835 ppm (`12 0b 00 00`); again 54 + 64*128*3 = 24,630, +2 trailing bytes.
A reader must accept BI_RGB 24-bit (and 8-bit palettised if met), bottom-up, BGR order. No alpha channel.

## 5. `.tga` files (99 files: `int8x6/item` 63, `texture/misc` 34, others 2) — V sample

Standard Truevision TGA. 97 files begin `00 00 02 00 00 00 00 00 …` (image type 2: uncompressed true-colour, no ID, no colour map) and
2 begin `00 00 0a …` (type 10: RLE true-colour). Example `int8x6/item/Alexandrite#M.tga`:
`00 00 02 | 00 00 00 00 00 | 00 00 | 00 00 | 38 00 | 38 00 | 20 | 08 | ff ff ff 00 …`
= no ID, no colour map, type 2, origin (0,0), **56 x 56**, **32 bpp**, descriptor 0x08 (8 alpha bits, **bottom-left origin**), then BGRA
pixels (first pixels `ff ff ff 00` = white, alpha 0). Second sample `texture/misc/caust10.tga`: 16,428 bytes = 18 + 64*64*4 (16,384) +
**26** trailing bytes (TGA 2.0 footer, expected `TRUEVISION-XFILE.\0` — not yet confirmed by hex; ignore it), type 2, 64 x 64, 32 bpp, descriptor
0x08, first pixels `4e 4e 4e ff` (alpha 0xFF = opaque). A reader must support types 2 and 10, 24 and 32 bpp, honour the origin bit (bit 5 of the
descriptor, 0 = bottom-up).

## 6. Transparency

* The texture loader passes **ColorKey = 0** (disabled; arguments of the D3DX call at 0x6099C0: `Width=Height=D3DX_DEFAULT,
  MipLevels=global (1 for fonts), Usage=0, Format=UNKNOWN (taken from file), Pool=MANAGED, Filter=MipFilter=D3DX_DEFAULT, ColorKey=0`).
  So there is no magenta/black key for `.bmp`: BMP is opaque; transparency comes only from the **alpha channel** (TGA 32 bpp,
  DDS DXT3/DXT5, A8R8G8B8).
* In the grid-font scanner a texel counts as ink when alpha != 0 (see [fin.md](fin.md)); formats without alpha are treated as
  fully opaque.

## 6a. Install census

All 205 files follow the layouts above: **106 `.bmp`** (all 24-bit BI_RGB, 3 channels) and **99 `.tga`** (all 32-bit, 4 channels; 97 type 2, 2 type 10 RLE, both 1024x768 in `int8x6/item`).
**All 205 files have trailing bytes** after the pixel payload computed from the header: BMP 106 x **2 bytes** (pad after the pixels, as in the
samples above); TGA 97 x **26 bytes** (the TGA 2.0 footer, ending in `TRUEVISION-XFILE.\0`, confirmed in all 99) and 2 RLE files x **521 bytes**
= 495-byte TGA 2.0 extension area + 26-byte footer.

## 7. Unknowns

* TODO(re): who loads the shipped `.bmp`/`.tga` (texture-dir resolver 0x6051C0 appends `.DDS`; `.bmp`/`.tga` may only be build-time
  sources and never read at runtime — the census lists them as shipped but no reader opens them by name in the analysed code).
* TODO(re): `pal_std.res` palette layout (256 x RGB? RGBA?) — reader not analysed.
* TODO(re): `I_cursor.res` exact framing (see section 1).
