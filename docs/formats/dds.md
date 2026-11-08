# `.dds` — DirectDraw Surface textures (4,346 files: `texture/**`, `int8x6/**`, `item/**`)

Standard Microsoft DDS container. **The game does not contain a pixel-format-to-`D3DFORMAT` mapper of its own in the
analysed code**: the only place it parses the header is an *alpha probe* (`dds_probe_has_alpha`, joan.exe
0x60B450, below). Actual texture creation from the file is presumably left to D3DX8 (`D3DXCreateTextureFromFile*`),
which parses the DDS itself, so FourCC → `D3DFORMAT`, mip chain and cube/volume handling are D3DX behaviour
(`TODO(re)`: confirm with a breakpoint on `D3DXCreateTextureFromFileInMemoryEx`). In OpenJoan the D3DX8 texture loaders are
ported in `src/lib/d3dx8` (`texture_from_file.cpp`, `texture_in_memory.cpp`) and the pixels reach OpenGL through `src/gl`. The
contract is therefore "everything D3DX8 accepts that appears in the shipped files".

Status of this page: header-validation logic and the search/cache semantics are **read from pseudocode**; the
census was taken over all 4,346 files.

## Layout (little-endian)
```
u32 magic 'DDS ' (0x20534444)
DDS_HEADER (124 bytes):
  +0x00 u32 size = 124; +0x04 u32 flags (DDSD_CAPS|HEIGHT|WIDTH|PIXELFORMAT|LINEARSIZE|MIPMAPCOUNT…)
  +0x08 u32 height; +0x0C u32 width; +0x10 u32 pitchOrLinearSize; +0x14 u32 depth; +0x18 u32 mipMapCount;
  +0x1C u32 reserved1[11]
  +0x48 DDS_PIXELFORMAT (32 bytes): u32 size = 32; u32 flags (DDPF_FOURCC=0x4, DDPF_RGB=0x40, DDPF_ALPHAPIXELS=0x1);
        u32 fourCC; u32 rgbBitCount; u32 rMask, gMask, bMask, aMask
  +0x68 u32 caps; u32 caps2; u32 caps3; u32 caps4; u32 reserved2
then mip level 0 .. mipMapCount-1 (mipMapCount 0 = one level), each: for block formats
  max(1, w/4) * max(1, h/4) * blockBytes (DXT1 = 8, DXT2..5 = 16); for RGB: pitch * height.
```
(offsets relative to the byte after the magic; add 4 for file offsets.)
Example `int8x6/bar_workshop.dds`: flags 0x00081007, height 8, width 64, linearSize 512, mips 0 (= 1 level), DXT5
→ file = 128 + 512 = 640 B (smallest file in the census). 1024² UI atlases: 1,048,704 B = 128 + 1 MiB DXT5, no mips;
512²: 524,416 B.

## What the original reader actually does
`dds_probe_has_alpha(const char* path)` RE: joan.exe 0x60B450 (cf. docs/re/functions/dds.md):
1. Takes the **last three characters** of the path as extension and compares case-insensitively (`_stricmp`) with
   two constant 3-char strings (`DAT_009d9ad8`, `DAT_009d9adc`). Equal to the first -> returns 1 immediately (most
   likely `tga`, "has alpha"); equal to the second (`dds`) -> parse; anything else (jpg/bmp/...) -> returns 0.
2. Opens the file (`File` class, error text "Error opening file %s."), skips the 4-byte magic and reads
   **0x7C bytes into a local buffer** (the header). **The magic and `dwSize` are NOT checked here.** Then it only
   looks at the pixel-format block (`size, flags, fourCC, bitCount, R, G, B, A masks`):

| pixel-format test (in order) | returns |
|---|---|
| `flags&DDPF_FOURCC` and fourCC == `DXT1` | 0 (opaque) |
| FOURCC `DXT3` | 1 |
| 8 dwords == constant tuple #1 (`DAT_009d9ae0`) | 1 |
| FOURCC `DXT2` / `DXT4` / `DXT5` | 1 |
| tuple #2 (`DAT_009d9b00`) | 0 |
| tuple #3 (`DAT_009d9b20`) | 1 |
| tuple #4 (`DAT_009d9b40`), tuple #5 (`DAT_009d9b60`), anything else | 0 |

Each tuple is a full DDS_PIXELFORMAT `{size=32, flags, fourCC=0, bitCount, rMask, gMask, bMask, aMask}` (dumped from
the data section, DATA.md); matching compares all 8 dwords:

| # | address | flags | bpp | R / G / B / A masks | format | probe result |
|---|---|---|---|---|---|---|
| 1 | 0x9D9AE0 | 0x41 | 32 | 00ff0000 / 0000ff00 / 000000ff / ff000000 | A8R8G8B8 | 1 |
| 2 | 0x9D9B00 | 0x41 | 16 | 7c00 / 03e0 / 001f / 8000 | A1R5G5B5 | 0 |
| 3 | 0x9D9B20 | 0x41 | 16 | f000 / 00f0 / 000f / f000 | (odd, see below) | 1 |
| 4 | 0x9D9B40 | 0x40 | 24 | 00ff0000 / 0000ff00 / 000000ff / 0 | R8G8B8 | 0 |
| 5 | 0x9D9B60 | 0x40 | 16 | f800 / 07e0 / 001f / 0 | R5G6B5 | 0 |

Constants: `DAT_009D9AD8` = "TGA" (returns 1 at once: TGA is always "has alpha"), `DAT_009D9ADC` = "DDS".
Quirks: the A4R4G4B4 files that really occur in the install (38 files, masks 0f00/00f0/000f/f000) match **no**
tuple and fall to the default (0 = no alpha); only the 13 files with the malformed masks f000/00f0/000f/f000
(red mask == alpha mask) match tuple #3 and return 1. A1R5G5B5 returns 0 although it has alpha.
Consequence: the probe treats DXT1 as alpha-less (1-bit alpha ignored), every other FourCC as having alpha;
the result selects alpha-blend vs opaque handling of the texture elsewhere (callers not analysed).

## Texture lookup by name (path builder + directory cache)
See docs/re/functions/dds.md for addresses. Behaviour:

* Callers ask for a texture by bare name (e.g. `newcloud02`, `P_Glow_01`, `Cloud01`); the cache (`en3d/texdir.cpp`)
  holds `name -> {extension mask, directory index}` records and resolves to `<dir>\<name>.<ext>`.
* **Directories registered at start-up** (`texdir_init_search_paths`, 0x608E10), in priority order:
  1. `INT8X6\` (UI atlases, fonts, icons; recursive = no, flag 0) and `INT8X6\ITEM\` (item icons, `…#M.dds`);
     repeated for each loop index `DAT_00a48858 .. 0` (a language/resolution tier count — `TODO(re)`).
  2. `SAVE\` (flag 1: registered but **never scanned** for files).
  3. The texture-quality directories. Setting `DAT_00a5804a` (0,1,2) picks the order of `TEXTUREL\`, `TEXTURE\`,
     `TEXTUREH\`: quality 0: L, normal, H; quality 1: normal, L, H; quality 2: H, normal, L. For each of the three,
     if the directory exists on disk (`FindFirstFile` + directory attribute): add sub-directories `ITEM\`, `UNIT\`,
     `TERRAIN\`, `PARTICLE\`, `MISC\` (registered in that order, non-recursive) and then the directory itself
     **recursively**. The shipped install only has `texture\` (no `textureL`/`textureH`), whose subdirs are
     `item, misc, particle, terrain, unit`.
  4. Three file extensions are registered with bit masks 1, 2, 4: `.DDS`, `.TGA`, `.BMP` (strings at 0x9D9C00,
     0x9D9BF8, 0x9D353C); the group name `DAT_009D9BF0` is "ITEM". A record's mask says which
     of them exist for that name.
* **Scan is lazy and per-directory** (states 0 = unscanned, 1 = in progress, 2 = done, separately for "files" and
  "sub-directories"). Files are enumerated once with a `*.*` wildcard; the name is split with `_splitpath`; only
  names whose extension is registered **and whose base name is <= 32 characters** are indexed. Comparison is
  case-insensitive (`_stricmp`), so the lookup is case-insensitive. **First directory wins**: if a name already
  exists from the same directory the extension masks are OR-ed, from a different directory the later one is ignored.
  A full rescan (`0x604850`) resets all states and re-scans every registered directory; it is the "refresh" call.
* Search by recursive probing (`0x609780`, `find_file_recursive(name, startdir, out_path)`): for each extension in the
  NULL-terminated list at `PTR_DAT_00a18948` build `startdir\name.ext`, test existence with `FindFirstFile`, return
  the first hit; otherwise recurse depth-first into sub-directories (`dir\*`, skipping `.` and `..`). Extension-list
  order = priority between formats: list at 0xA18948 = {"DDS", "TGA", "BMP", NULL}, so `.dds` wins over `.tga`
  over `.bmp` (the `texture/misc/hand.bmp` and `int8x6/item/*.tga` files are therefore valid targets too).
  `DAT_00A48858` is a .bss runtime variable (not in the file).
* Strings handled by the path builders are fixed 400-char buffers (`strncpy/strncat` limit 400).

## Naming conventions seen in the shipped file names (not enforced by the analysed code)
* `int8x6/item/<Name>#M.dds` — item inventory icon, medium size (`#M`; one oddity `Joan_armor_Lv1#m.dds`).
  Data files (`item.res` etc.) reference the item by name; the `#M` suffix is appended by the UI code (`TODO(re)`).
* `texture/item/e_lisword#2.dds` / `…#2_c.dds` — weapon texture variant `#n` and a `_c` companion (6 files).
* `int8x6/menu_a.dds`, `int8x6/item/Cross_a.dds` — plain names ending in `_a`; **no** `_a` alpha-variant convention
  was found (the two hits are ordinary names). Alpha comes from DXT3/DXT5 itself.
* Names may contain spaces and mixed case (`18pt dark numbers.dds`, `Celtic 12pt Yellow.dds`): lookup is
  case-insensitive (Windows semantics; on Linux see [vfs.md](vfs.md)).

## JPEG path (`elib/jpeg.cpp`, 0x5A8D80 / 0x5A8FA0)
A thin wrapper over the (statically linked) IJG libjpeg: constructor allocates a 0x15B-byte
`jpeg_decompress_struct`-like block and a 0x84-byte error/source-manager block, sets error callbacks and calls
`jpeg_create_*` (`0x7A9A60(cinfo, 0x3E, 0x15B)` = `jpeg_CreateDecompress(version 62, size 347)`); destructor
destroys and frees both. The shipped JPEGs are `images/gen_background2.jpg` and `images/border_background.jpg`
(baseline JFIF) — launcher/menu backgrounds, decoded into a texture/surface rather than going through `.dds`.
`0x60B450` returns 0 (no alpha) for `.jpg`.

## Census (4,346 files)
All 4,346 have magic `DDS ` and `dwSize` 124 (0 bad). Sizes 640 B .. 4,194,432 B. No volume textures.

| pixel format | files | note |
|---|---|---|
| FourCC DXT1 | 3,543 | BC1 |
| FourCC DXT3 | 490 | BC2 |
| FourCC DXT5 | 189 | BC3 |
| RGB32 A8R8G8B8 (masks 00ff0000/0000ff00/000000ff/ff000000) | 73 | 8-bit BGRA |
| RGB16 A4R4G4B4 (0f00/00f0/000f/f000) | 38 | 4-bit channels |
| RGB16 "odd" (f000/00f0/000f/f000) | 13 | same, standard bit positions used (below) |

Per top-level dir: `int8x6`: DXT3 282, DXT5 124, A4R4G4B4 38, odd-16 13, DXT1 7, A8R8G8B8 5; `item`: DXT1 1;
`texture`: DXT1 3,535, DXT3 208, DXT5 65, A8R8G8B8 68.

| mipMapCount | files | | flags | files | | caps2 | files |
|---|---|---|---|---|---|---|---|
| 0 | 242 | | 0x000A1007 | 4,104 | | 0 | 4,343 |
| 6 | 18 | | 0x00081007 | 154 | | 0xFE00 (cubemap, all 6 faces) | 3 |
| 7 | 128 | | 0x0000100F | 86 | | | |
| 8 | 1,353 | | 0x00001007 | 2 | | | |
| 9 | 2,040 | | | | | | |
| 10 | 341 | | | | | | |
| 11 | 223 | | | | | | |
| 12 | 1 | | | | | | |

(`0xA1007` = CAPS|HEIGHT|WIDTH|PIXELFORMAT|MIPMAPCOUNT|LINEARSIZE; `0x81007` = without MIPMAPCOUNT; `0x100F` =
CAPS|HEIGHT|WIDTH|PITCH|PIXELFORMAT, i.e. uncompressed with a pitch and no mips (86 files); `0x1007` = no size
field at all (2 files). Do not rely on flags for size computation.) Dimensions (w x h): 256x256 1,892; 128x128 1,411; 512x512 311;
1024x1024 255; 64x64 125; 128x256 88; 64x256 58; 1024x512 39; 256x512 29; 512x256 26; 256x128 21; 32x32 20;
512x128 17; 512x1024 14; 1024x256 7; 128x512 6; 64x128 5; 1024x32 4; 1024x64 3; 1024x128 3; 64x8 3; 128x32 2;
128x64 2; 128x16 1; 32x64 1; 28 distinct sizes in all, including 2048x256, 256x64 and 128x1024 (1 file each). Non-power-of-two never occurs; non-square does (UI strips).
**The "3 files with mismatching size" are the three cubemaps** `texture/misc/sky1.dds`, `texture/misc/waterS.dds`,
`texture/misc/waterSsea.dds` (256x256 DXT1, mips 0, caps 0x1008, caps2 0xFE00, 196,736 B = 128 + 6 faces x 32,768).
A single-face count undercounts them; with `face_count = 6` every one of the 4,346 files matches its header-implied size
exactly, so no shipped file has a size mismatch. Face order
is the standard +X,-X,+Y,-Y,+Z,-Z with each face's mip chain stored contiguously (here one level each).
Mip count 0 (242 files) means a single level.

## Level sizes and pixel layouts
- Header checks that hold for every shipped file: magic `DDS `, `dwSize == 124`, pixel-format size 32, no volume textures, no
  partial cubemaps, and file size = `128 + faces x sum(level sizes)` exactly.
- Level size: BC = `ceil(w/4) * ceil(h/4) * (8|16)`; uncompressed = `w*h*bytes` (tight rows; `pitchOrLinearSize` is
  not reliable because 2 files have no size flag and the flags vary). Mip dimension = `max(1, dim >> n)`; mip count =
  `max(1, dwMipMapCount)`. Faces are stored one after the other, each with its whole mip chain.
- Layouts: DXT1 = BC1, DXT2/3 = BC2, DXT4/5 = BC3 (premultiplied alpha for DXT2/4), A8R8G8B8 = 8-bit BGRA. A4R4G4B4 (51 files)
  is the only 16-bit layout that occurs; A1R5G5B5, R5G6B5 and R8G8B8 are not in the shipped files.
- The 13 `int8x6` files with masks f000/00f0/000f/f000 (red mask == alpha mask) are read with the standard A4R4G4B4 bit
  positions; the alpha channel is bits 12..15 in both variants. The texels were not compared visually against D3DX.
- The original alpha probe (0x60B450): DXT1 false, DXT2-5 true, A8R8G8B8 true, malformed A4R4G4B4 true, standard A4R4G4B4
  **false** (38 files), A1R5G5B5 false. Whether a layout stores alpha at all is true for everything except DXT1, R5G6B5 and R8G8B8.
- Name resolution (texture directory priority, case-insensitive, base name <= 32) is done by the texture manager, see
  "Texture lookup by name" above.

## S3TC block layouts
Block layouts follow the public S3TC description. BC1: two RGB565 endpoints `c0`, `c1` then 16 2-bit indices;
`c0 > c1` → four colours (`2/3·c0+1/3·c1`, `1/3·c0+2/3·c1`), else three colours (`(c0+c1)/2`) and index 3 =
transparent black. BC2: 16 4-bit alpha values (×17) then a BC1 colour block always in four-colour mode. BC3:
two alpha endpoints `a0`, `a1` and 16 3-bit indices (`a0 > a1` → 8-value ramp, else 6 values + 0 + 255), then
the colour block. Interpolation truncates (`(2a+b)/3`), which may differ from hardware rounding by ±1 LSB.
