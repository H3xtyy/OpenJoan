# `.fin` — bitmap font metrics (`font/*.fin`, 19 files, 3,184 bytes each) and the grid-font variant

Reader: `en2d\font2d.cpp`, `Font2D::load_fin` **0x624810** (joan.exe; retail equivalents in
`docs/re/functions/_index_retail.md`: 0x623710). Struct: [Font2D](../re/structs/Font2D.md), [Glyph](../re/structs/Glyph.md).
Function table: [fin.md](../re/functions/fin.md). Confidence legend: **V** = verified against a real file or read
directly in the reader, **I** = inferred, **TODO(re)** = not determined.

A `.fin` holds **metrics only**. The pixels live in a DDS texture with the *same name* (see "Atlas" below).
The draw-text routines have not been analysed, so layout rules (advance, line height, kerning)
are only partly known; see "Layout rules".

## File layout (little-endian, no version field, no magic)

Total size = 48 + 224 x 14 = **3,184** (V: min = max = 3,184 for all 19 files; RE: 0x624810 reads exactly
these items through the `File` helpers 0x587650 (block), 0x587d20/0x587c20 (u16), 0x587e20 (u32)).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 32 | char[32] | `face` | GDI face name the font was baked from (`GoudyOlSt BT`, `Times New…`, `Dauphin`), NUL padded. **Discarded by the reader**: the in-memory name is overwritten with the requested name (`strncpy(name, requested, 0x20)` after the read) | V 0x624810; census starts `47 6f 75 64 79 4f 6c 53` x16 |
| 0x20 | 2 | u16 | `first_char` | first code covered; **0x20** in every file | V (Goudy_10: `20 00`) |
| 0x22 | 2 | u16 | `last_char` | last code covered; **0xFF** (INVENTORY called it "count 0xFF"; the reader treats it as the last code: `(last-first+1)` records follow) | V 0x624810 (record count = last-first+1 = 224) |
| 0x24 | 2 | u16 | `max_width` | widest glyph, pixels (Goudy_10: 14) | V Goudy_10; stored Font2D+0x2E |
| 0x26 | 2 | u16 | `max_height` | tallest glyph, pixels (Goudy_10: 13). The reader also copies this into `line_height` (+0x34) | V Goudy_10; Font2D+0x30 |
| 0x28 | 2 | u16 | `span_height` | (lowest glyph bottom - highest glyph top + 1) over the whole font (Goudy_10: 16). The grid builder computes it exactly so (0x623E90); Font2D+0x32 | V Goudy_10; I (semantics from builder) |
| 0x2A | 2 | u16 | `digit_width` (= nominal space advance) | Font2D+0x36, kept raw. Install values over the 19 files: 3, 4, 5, 6, 7, 9, 13, 17, always about `max_height * 4/15` (13 -> 3, 16 -> 4, 19 -> 5, 24 -> 6, 27 -> 7, 32 -> 9, 48 -> 13, 64 -> 17; exceptions dauphin_14_bold 23 -> 5 and testfont 16 -> 9). It is **not** the width of glyph `'1'` in the `.fin` files (differs in 15 of 19; the grid builder 0x623E90 does take glyph 0x31's width, the `.fin` baker evidently used a different rule). Every file's space glyph (0x20) is the collapsed 1x1 cell, so the field is the only plausible space advance (I; confirm in the text blitter, TODO(re)) | V value (all 19 files, `fin_test.cpp`); I meaning |
| 0x2C | 4 | u32 | `reserved` | read and **discarded**; always 0xE0 = 224 (= glyph count) in Goudy_10 | V 0x624810 (result of 0x587E20 unused) |
| 0x30 | 224 x 14 | Glyph[last-first+1] | `glyphs` | one record per code, index `code - first_char` | V |

All 19 files must have identical size, so `first_char = 0x20`, `last_char = 0xFF` for every file
(V: `first=0x20`, `last=0xFF`, `reserved=0xE0` in all 19 files).

### Glyph record (14 bytes, 7 x u16)

Read in this order by 0x624810 (7 calls of 0x587C20). Meaning taken from the builder 0x623E90 that produces the
same layout from a texture:

| # | off | name | meaning |
|---|---|---|---|
| 0 | 0 | `y_off` | vertical offset of the glyph's ink from the line top: draw the quad at `line_y + y_off`. Not normalised to 0 in the files (Goudy_12: ordinary glyphs 4, space 10; dauphin: space 12), and independent of the glyph's atlas row (Goudy_12 code 0x43 has y_off 4 but y0 = 24) |
| 1 | 2 | `x0` | atlas pixel column of the left edge (inclusive) |
| 2 | 4 | `y0` | atlas pixel row of the top edge (inclusive) |
| 3 | 6 | `x1` | atlas column of the right edge (**inclusive**) |
| 4 | 8 | `y1` | atlas row of the bottom edge (**inclusive**) |
| 5 | 10 | `width` | ink width = `x1-x0+1`; this is also what the layout uses as the character width. Note: the reader subtracts `tighten` (Font2D+0x3A) from this field for all glyphs when `tighten != 0` |
| 6 | 12 | `height` | ink height = `y1-y0+1` |

Worked example, `font/Goudy_10.fin` bytes 0x30..0x3D (code 0x20, space): `08 00 | 00 00 | 08 00 | 00 00 | 08 00 | 01 00 | 01 00`
= y_off 8, x0 0, y0 8, x1 0, y1 8, width 1, height 1 (an empty cell collapses to a 1x1 rect at (0, 8)).
Header bytes of the same file: `20 00 ff 00 0e 00 0d 00 10 00 03 00 e0 00 00 00` =
first 0x20, last 0xFF, max_w 14, max_h 13, span 16, digit_w 3, reserved 0xE0.

More verified data:
* `Goudy_12.fin` header (off 0x20): `20 00 ff 00 11 00 10 00 13 00 04 00 e0 00 00 00` = first 0x20, last 0xFF, max_w 17, max_h 16, span 19, 4, reserved 0xE0.
  Glyph `'!'` (0x21, record at 0x3E): `04 00 02 00 04 00 03 00 0e 00 02 00 0b 00` = y_off 4, x0 2, y0 4, x1 3, y1 14, w 2, h 11
  (x1-x0+1 = 2, y1-y0+1 = 11 check out). Glyph at record 35 (file 0x21A) = y_off 4, x0 0, y0 24, x1 9, y1 34, w 10, h 11.
* `dauphin_14_bold.fin` header: first 0x20, last 0xFF, max_w 18, max_h 23, span 24, 5, reserved 0xE0; space = `0c 00 00 00 0c 00 00 00 0c 00 01 00 01 00`.
* Face strings: Goudy_12 begins `GoudyOlSt BT` (NUL padded to 32).
* Atlas names: all 19 fonts have an `int8x6/<name>.dds` (Goudy*, Times_10, dauphin_14_bold, testfont).
The other 16 files were checked for size only (3,184 = 48 + 224 x 14) here; the install census below covers all glyphs of all 19 files.

## Atlas

* Texture name = the **requested font name** (not the `face` field): `Font2D::load_fin(name, spacing, tighten)` builds the
  path `FONT\` + name + ext (`DAT_009DA6E8`, `.fin`; I), opens it with `CreateFileA` relative to the install root
  (0x586DA0), and asks the texture manager for texture `name` (`FUN_006099C0(name, kind=1, 0)`; the directory
  resolver 0x6051C0 searches the registered texture directories, `INT8X6\` is the one holding fonts, extension `.DDS`).
  Names match **case-insensitively** (code requests `goudy_bold_14`, files are `Goudy_bold_14`; request `Goudy_bold_10`,
  file `Goudy_Bold_10`). On Linux the match is made by the platform file layer ([vfs.md](vfs.md)).
* `int8x6` is a texture *directory* (I: "interface @ 800x600"; other directories: `TEXTURE\`, `UNIT\`, `ITEM\`).
  It is **not** a cell layout. Glyph rectangles are arbitrary, stored explicitly; the atlas is a normal DXT-compressed
  or ARGB DDS (see [dds.md](dds.md)). Atlas size is simply the DDS size; glyph coordinates are texels (not normalised).
* The reader loads the texture with the texture manager's global mip-level setting forced to **1** (0x60A720 saves
  `+0x30`, 0x60A740(1) sets it, restored afterwards): font atlases are loaded **without mipmaps** (I: field is the
  `MipLevels` argument of the D3DX call at 0x765AAB). Use point/bilinear sampling, no mip chain.
* The texture is only needed while loading the metrics (the temporary texture pointer at Font2D+0x26 is released and
  zeroed at the end of both readers); drawing re-fetches the texture by name (I: Font2D holds no texture pointer after load).
* Failure: `Warning! Cannot initialize font "<name>"` is logged; the function returns 0 and the font stays unloaded.

## Which fonts the game loads (RE: 0x4D6FE0, startup; 17 `.fin` + 30 grid fonts)

`.fin` fonts, all with `spacing = 1`, `tighten = 0`:
`Goudy_bold_24`, `Goudy_Bold_Gold_24`, `Goudy_bold_20`, `Goudy_bold_18`, `Goudy_bold_10`, `Times_10`, `Goudy_10`,
`Goudy_bold_48`, `Goudy_18`, `Goudy_20`, `goudy_bold_14`, `goudy_bold_italic_12`, `goudy_bold_italic_18`,
`goudy_bold_italic_24`, `goudy_12`, `goudy_italic_12`, `dauphin_14_bold`.
Shipped but **never requested** by the startup table: `Goudy_bold_italic_36.fin`, `testfont.fin` (also a grid font name).
Every one of the 19 has a same-named `int8x6/<name>.dds`.

## Grid fonts (no `.fin` at all): `Font2D::build_from_grid` 0x623E90

The other 30 fonts (`testfont`, `font_Verdana_10[_bold|_Gold]`, `font_Verdana_12[_bold]`, `Verdana 12 English/French`,
`verdana*`, `font_perrygothic_10_gold`, `Celtic 10pt English/French`, `Celtic 18pt Black`, `24pt large text`,
`18pt/24pt … numbers …`, `Goudy12(18) …`, `Goudy14(22) white`, `Goudy18(27) white`, `Goudy12 Bold Black`, …) have **no metrics
file**. Their metrics are derived at startup by scanning the DDS: call
`build_from_grid(name, cell_w, cell_h, columns, first, last, margin, spacing, tighten)`:

1. Load texture `name` (mip = 1), wait for the async texture loader to drain (0x60A770 pumps until idle), lock level 0.
2. Cells are laid out left-to-right, top-to-bottom, starting at pixel `(margin, margin)`; advance per cell
   `x += margin + cell_w`; a new row starts (`x = margin; y += margin + cell_h`) when `x + cell_w` exceeds the texture
   width **or** `columns` cells were placed. Code `first` is cell 0.
3. In each cell find the tight bounding box of pixels whose **alpha != 0** (0x628490 scanning edges inward; alpha read
   per D3DFORMAT by 0x628940: A8R8G8B8 -> bits 31..24, A1R5G5B5 -> bit 15, A4R4G4B4 -> bits 15..12, A8 -> byte,
   A8R3G3B2 -> high byte, formats without alpha -> 0xFF i.e. always solid). An empty cell collapses to the
   cell origin (1 x 1 as in the .fin space example).
4. Fill the same 14-byte record: `y_off` = top row in cell, `x0..y1` = cell origin + bbox, `width`, `height`; track
   `max_width`, `max_height`, min top, max bottom. After all cells: `y_off -= min_top`, `span = max_bottom - min_top + 1`,
   `line_height = max_height`, `digit_width = glyph(0x31).width`, `spacing`, `tighten`; if `tighten != 0` it is subtracted
   from every glyph `width`.

So `.fin` is just a **cache of the result of this scan** for the Goudy/Times/Dauphin fonts (which were baked from GDI
TrueType by an offline tool; the 32-byte `face` is a leftover of it). A port can use the scan for any atlas and `.fin`
for the 17 fonts, with identical results.

Table of the 30 startup calls (cell WxH, columns, first..last, margin, spacing; tighten is always 0): see
[fin functions page](../re/functions/fin.md#grid-font-startup-table-0x4d6fe0). Characters with code < `first` or
> `last` have no glyph (zero-filled records exist only for the range).

## Layout rules (what is known)

* Metrics available to the layouter: `line_height` = `max_height` (+0x34), `span_height` (+0x32), `digit_width` (+0x36),
  per-code `width/height/y_off`, `spacing` (+0x38, the second argument of the readers: **1** for all `.fin` and most grid fonts,
  **0** for some grid fonts such as the Celtic/Goudy12(18) ones), `tighten` (+0x3A).
* There is **no advance or kerning table** in the file: the advance is `glyph.width` (+ `spacing` inter-character gap, I). The
  draw-text routine has not been analysed — TODO(re): the text blitter (callers of Font2D, vtable/regions around
  0x6249xx..0x625xxx) to confirm advance = width + spacing, space width rule (space is a 1x1 glyph), and whether `line_height`
  or `span_height` is the line pitch.
* Code points are bytes of the localisation code page (cp1250 for PL): a glyph exists for 0x20..0xFF, so the atlas
  carries the code-page letters. text is a code-page byte per glyph; glyphs for other scripts do not exist in the atlas.

## TrueType (GDI) coexistence

* `joan.exe` imports GDI (`CreateFontA`, `ExtTextOutA`, `CreateDIBSection`, ...) but **none of the functions in
  `font2d.cpp` use it** (0x624810, 0x623E90, 0x624DC0 touch only files, the texture manager and the tables above).
  All UI text that goes through `Font2D` is **bitmap-only**.
* The `.fin` `face` field proves the Goudy/Times/Dauphin atlases were produced from TrueType fonts by an offline tool; GDI at
  runtime is probably used elsewhere (system/debug/message text, video subtitles?). TODO(re): xrefs of the `CreateFontA` IAT
  entry to find the runtime GDI user.
* The original has no fallback for a code point outside `first..last` or one the baked atlas lacks. Localised editions with
  double-byte code pages get a CJK font through `src/platform/dbcs.cpp` ().

## Install census (all 19 files)

* Header: `first_char = 0x20`, `last_char = 0xFF`, `reserved = 0xE0` in every file; faces `GoudyOlSt BT` (16 files), `Times New Roman`
  (Times_10), `Dauphin`, `testfont`.
* Glyph invariants hold for **all 19 x 224 records**: `x1-x0+1 == width`, `y1-y0+1 == height`, `x1 < atlas width`, `y1 < atlas height`
  (atlas size read from the DDS header: height u32 at byte 12, width u32 at byte 16). Every font has `int8x6/<name>.dds` (case-insensitive).
* `max_width` equals the widest glyph in 18 files (dauphin_14_bold: header 18, widest record 17) and `max_height` equals the tallest in all 19.
  `span_height` is `max(y_off+height) - min(y_off)` or that plus one.
* The 32-byte face field is NUL-padded with no garbage in every file.
