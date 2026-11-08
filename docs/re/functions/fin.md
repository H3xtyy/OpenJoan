# fin family — Font2D / image2d / bmpres / ilib bitmap (joan.exe)

Exported pseudocode: `$JOAN_WORK_DIR/decomp/binfmt/fin/`. Specs: [fin.md](../../formats/fin.md), [bitmap.md](../../formats/bitmap.md).
Structs: [Font2D](../structs/Font2D.md), [Glyph](../structs/Glyph.md), [Bitmap](../structs/Bitmap.md).
Status: verified = decompiled and every field accounted for; read = read, role clear; inferred = from callers/shape.

## en2d\font2d.cpp

| address | proposed name | role | status |
|---|---|---|---|
| 0x624810 | `Font2D::load_fin(name, spacing, tighten)` (`__thiscall`, this = Font2D) | unload if loaded; build `FONT\`+name+ext; open; skip 32-byte face (copy requested name into +1); load texture `name` (mip=1) just to validate; read first/last, max_w, max_h, span, digit_w, skip u32; allocate `(last-first+1)*14` glyphs; read 7 u16 each; `line_height = max_h`; store spacing/tighten; subtract `tighten` from every glyph width; release texture; set loaded flag | verified |
| 0x623E90 | `Font2D::build_from_grid(name, cell_w, cell_h, cols, first, last, margin, spacing, tighten)` | derive the same table from a DDS grid atlas by scanning alpha (see fin.md) | verified |
| 0x624DC0 | `Font2D::unload` (`__fastcall`) | free glyph array (+0x3C), release temp texture (+0x26), clear loaded flag | verified |
| 0x628490 | `scan_cell_bbox(pix, pitch, bpp_fmt, …, &x0,&y0,&x1,&y1)` | shrink a cell rect to the tight alpha!=0 bounding box (top, bottom, left, right passes) | verified |
| 0x628940 | `pixel_alpha(ptr, d3dfmt)` | alpha of one texel by D3DFORMAT 0x14..0x1E (A8R8G8B8: >>24; A1R5G5B5: bit 15; A4R4G4B4: >>12; A8: byte; A8R3G3B2: >>8; others 0xFF) | verified |
| 0x5FFE60 | `d3dfmt_bytes_per_pixel` | D3DFORMAT -> bytes per texel (0x14=3, 0x15/0x16=4, 0x17..0x1A,0x1E=2, 0x1B/0x1C=1, …) | verified |
| 0x60A770 | `TextureMgr::wait_loads` | loop: count pending async texture loads (two arrays), `SleepEx(n*10)`, pump 0x60A990 until none | read |
| 0x60A720 / 0x60A740 | `TextureMgr::get_mip_levels` / `set_mip_levels` | global `+0x30` = MipLevels passed to the D3DX texture-from-file call; fonts force 1 | read (I: meaning of +0x30) |
| 0x6099C0 | `TextureMgr::load_texture(name, kind, flag)` | name -> directory resolver 0x6051C0 -> D3DX load (0x765AAB/0x765D4E) or async job; returns texture entry (cache check 0x60A4B0 first). `kind` 1/2/3 pick 2D / cube / volume-like paths | read (other family) |
| 0x6051C0 | `TexDir::resolve_name` | maps a bare texture name to a registered directory + `.DDS` (texdir cache, 0x3D-byte dir records) | read (other family) |
| 0x60A550 | `TextureMgr::release_texture` | drop reference to a loaded texture | read |
| 0x4D6FE0 | `Game::init_fonts` (caller, from textfmt export) | the font table below | verified |

## en2d\image2d.cpp

| address | proposed name | role | status |
|---|---|---|---|
| 0x629830 | `Image2D::reset` (`__fastcall`) | zero +0x72 (dword) and +0x76, free name string +0x94, clear +0x70 flag (in-use/loaded) | verified |
| 0x6298C0 | `Image2D::set_name(const char*)` | free old +0x94, allocate strlen+1, copy | verified |

## en2d\bmpres.cpp

| address | proposed name | role | status |
|---|---|---|---|
| 0x620AB0 | `BmpRes::free` (`__fastcall`) | free entry array (+5), clear loaded flag (+0) | verified |
| 0x620BC0 | `BmpRes::load(resource_name)` | select the `INTERFAC` resource DB (cached id in `DAT_00A69D0C`, restore previous after), open table `resource_name`, count rows (`db+0x112`), allocate `rows*0x38`, convert each fixed-width ASCII row into a 0x38-byte entry (layout in structs/Bitmap.md) | verified (field semantics TODO) |
| 0x58B250 | `ResDir::find(name)` | linear search of 0x46-byte names in the resource index; returns 1-based index or 0 | verified |
| 0x58B080 | `ResDir::select(idx)` | open `RESOURCE\` + name[idx-1] + ext (`.RES`) through resx | read |
| 0x59C950 / 0x59CA60 / 0x59CBB0 | `ResX::get_table` etc. | locate a table inside the open `.RES` and return its data pointer (offset table with 13-byte entries) | read (res family) |
| 0x580C70 | `Db::get_row(n)` | row n (1-based) of the current table; row stride at `db+0x118`, count at `db+0x112` | read |
| 0x590210 | `copy_trim_field(dst, src, n)` | copy n chars, trim trailing spaces, NUL-terminate | verified |
| 0x590270 | `atoi_field(src, n)` | copy n chars (n <= 0x78), terminate, `atoi` | verified |

## ilib\bitmap.cpp / bitmapd.cpp

| address | proposed name | role | status |
|---|---|---|---|
| 0x57F080 | `Bitmap8::create(w, h)` | allocate `w*h+4` bytes (0x57F050) and store `w`,`h` as two u16 at the start (0x57EF20) | verified |
| 0x57F230 | `Bitmap32::create(w, h)` | allocate `w*h*4+4` bytes (0x57F200) and store `w`,`h` (0x57F0F0) | verified |

Neither function reads a file; both are in-memory allocators. There is **no `.bmp` or `.tga` parser** in this family:
those files are fed to D3DX (0x765AAB: D3DXCreateTextureFromFileEx-like, which natively reads BMP/TGA/DDS/…); see bitmap.md.

## File helpers used (class `File`, ilib\file.cpp; the same family of calls appears in every reader)

| address | proposed name | role |
|---|---|---|
| 0x586DA0 | `File::open(path, report_errors, packed_mode)` | `CreateFileA(root + path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, NORMAL)`; fields: path[0x105] at +0, HANDLE +0x105, report flag +0x109, packed-mode flag +0x10D; error `Error opening file %s.` |
| 0x587260 | `File::close` | |
| 0x587C20 / 0x587D20 | `File::read_u16` (two identical copies) | `ReadFile` 2 bytes; on failure returns 0 (+ message if +0x109) |
| 0x587E20 | `File::read_u32` | 4 bytes |
| 0x587650 | `File::read_block(buf, elem_size, count)` | plain `ReadFile` of `elem*count` when packed-mode flag is 0 (0x5873A0 does the retry loop with a "Retry ?" message box); packed mode reads a u16 `w`,`h` header and copies rows into a differently pitched buffer (2-D strided/clipped read) |
| 0x587EA0 / 0x587EF0 | `File::seek(off, whence)` / `File::tell` | |

## Grid-font startup table (0x4D6FE0)

Arguments after the name: cell_w, cell_h, columns, first, last, margin, spacing (tighten = 0 always).

| name | cell | cols | first..last | margin | spacing |
|---|---|---|---|---|---|
| testfont | 16x16 | 37 | 0x21..0x7E | 1 | 1 |
| font_verdana_10, font_verdana_10_bold | 12x12 | 39 | 0x21..0xFF | 1 | 1 |
| font_verdana_12, font_verdana_12_bold | 14x14 | 34 | 0x21..0xFF | 1 | 1 |
| verdana_10_regular | 12x12 | 39 | 0x21..0x7E | 1 | 1 |
| verdana_12_regular_black_outline, verdana_12_gold_black_outline | 14x14 | 34 | 0x21..0x7E | 1 | 1 |
| font_perrygothic_10_gold | 20x20 | 37 | 0x21..0xFF | 1 | 0 |
| font_verdana_10_gold | 20x20 | 37 | 0x21..0xFF | 1 | 1 |
| 24pt large text | 41x41 | 24 | 0x21..0x7A | 1 | 1 |
| 18pt attribute numbers out / over, 18pt dark numbers, 18pt light numbers | 23x23 | 25 | 0x22..0x39 | 1 | 1 |
| 24pt large numbers gold / light | 31x31 | 25 | 0x21..0x39 | 1 | 1 |
| Goudy12(18) inner emboss / black / yellow / target unit, Goudy12 Bold Black | 17x17 | 14 | 0x21..0xFF | 1 | 0 |
| Celtic 10pt English / French | 17x17 | 28 | 0x21..0x7A | 1 | 0 |
| Celtic 18pt Black | 31x31 | 32 | 0x21..0x7A | 1 | 0 |
| Verdana 12 English / french | 14x14 | 34 | 0x21..0xFF | 1 | 0 |
| Goudy18(27) white, Goudy14(22) white | 26x26 | 18 | 0x21..0xFF | 1 | 0 |
| verdana12_reg_outline | 16x16 | 60 | 0x21..0xFF | 1 | 1 |

Note `first` is 0x21 (not space): the grid starts at `!`, unlike `.fin` (first = 0x20). Numeric-only fonts start at 0x22.
"Celtic 10pt French" vs "English" and "Verdana 12 English/french" are the per-language variants (accented glyphs differ).
Texture names are the font names; the installed copies are in `int8x6/` (e.g. `font_Verdana_10.dds`, `Verdana 12 English.dds`).
Some requested names (e.g. `font_verdana_10_gold`, `Celtic 18pt Black`) may be absent in a given install: loader logs a warning and
returns 0 (TODO(re): which are missing on the retail PL build — cross-check with a directory listing).

## Open questions

* TODO(re): text blitter / `Font2D` draw routines (advance, space width, line pitch, clipping, colour modulate) are not in this export.
* TODO(re): who calls `CreateFontA`/`ExtTextOutA` at runtime (xref IAT; no caller among exported functions).
* TODO(re): 0x620BC0 numeric fields at row offsets 0x28/0x2C/0x30/0x34 (4 chars each), 0x38 (1 char), 0x39/0x3D (4 chars): semantics unknown; confirm against `INTERFAC` table dump.
* TODO(re): `DAT_009DA6E8` / `DAT_009D8AC8` literal values (presumed `.fin` / `.RES`).
