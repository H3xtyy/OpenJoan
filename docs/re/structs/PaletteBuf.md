# PaletteBuf (en3d `vgabuf.cpp`) and VgaLock (`vgalock.cpp`)

**Not** D3D vertex-buffer wrappers (the earlier guess in MODULES.md "back buffer / lock surfaces" and the brief's
"vgabuf/vgalock = vertex buffer lock/unlock" are wrong). They are the legacy **8-bit palette (VGA) helpers**: a 256-entry RGB
palette with cached conversions to the display's native pixel width. The real vertex/index buffer code lives in
`plane.cpp` (0x5CFA10) and the dynamic ring buffer (0x600490/0x600510/0x600A90, unexported helpers 0x6005B0, 0x600740, 0x600820).

## PaletteBuf (16 bytes, four optional heap blocks)

| off | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | u8* | rgb | 0x300 bytes, 256 x (R,G,B) | 0x612AF0, 0x612DD0 |
| 0x04 | 4 | u8* | idx8 | 0x100 bytes; filled by `0x6101F0`, which is an empty function (identity / no conversion for 8-bit targets) | 0x612C40 |
| 0x08 | 4 | u16* | pix16 | 0x200 bytes: 256 packed 16-bit pixels via `0x610160` (copies the 3 RGB bytes to a dword, calls the display packer `0x571205`) | 0x610210 |
| 0x0C | 4 | u32* | pix32 | 0x400 bytes: 256 packed native pixels (same packer) or, via `0x612DD0`, 256 x `0xFFRRGGBB` | 0x610270 |

Which cache is built depends on a pixel-format class at `target+0x28`: 1 = 8 bpp (idx8), 2 = 16 bpp, 3 or 4 = 24/32 bpp.

| addr | name | behaviour |
|---|---|---|
| 0x612EE0 | palettebuf_set | store the four pointers |
| 0x6123F0 | palettebuf_clone | allocate matching blocks (only those present in the source) and `memcpy` |
| 0x612600 | palettebuf_from_arrays | build from up to four raw arrays |
| 0x612800 | palettebuf_from_rgb | copy RGB, then build only the cache needed by the format class |
| 0x612AF0 | palettebuf_set_rgb | replace RGB (0x300), drop caches, optionally rebuild for a format |
| 0x612C40 | palettebuf_ensure_cache | lazily build the cache matching the format class |
| 0x612DD0 | palettebuf_build_argb | `pix32[i] = 0xFF000000 \| R<<16 \| G<<8 \| B`; if the flag arg is non-zero index 255 gets alpha 0 (colour-key) |
| 0x612A00 | palettebuf_free | free the four blocks |

## VgaLock (4 bytes: pointer to a 0x400-byte scratch buffer)

`0x613270` allocates the 0x400-byte buffer on first use and always returns 1 ("lock succeeded"); `0x613210` is the dtor
(`0x6134A0` constant-true unlock, then free). No hardware surface is involved.

Only relevant if a shipped asset uses 8-bit indexed bitmaps (`.bm` palettes).
