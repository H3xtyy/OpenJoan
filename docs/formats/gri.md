# `.gri` — terrain cell grid (`map/scenarioN/<name>.gri`, 32 files)

The binary companion of the text `.stg` (`CreateGrid X:256 Y:256`, `CreateSceneSector`…) and `.wor`. It holds the
per-vertex terrain attributes (height, vertex colour, sector id, texture ids, subdivision) for the whole map; the terrain
meshes ([pln.md](pln.md)) are generated from it. Reader **0x4F6EB0** (`world_s.cpp`; called by the level loader 0x4F2F80
after the text files, through a `FileMem`), light variant **0x558140** (used when a valid `.pln` was loaded: it only re-reads
the texture-index map). Layer decoder **0x59DC10**. Sizes: S1L1.gri = 196,925 B, S5L1_5.gri etc. 220x220 maps. `joan.exe`.

> **Status: fully decoded for the shipped data.** The layout below consumes **all 32 files to the last byte**:
> header, sector names, texture table, 16 layers (each exactly `(w+1)*(h+1)` bytes) and the (empty) sub-vertex section. The layer decoder 0x59DC10 was
> disassembled directly from `joan.exe` (no pseudocode export exists for it) and is a plain byte RLE, below.

## Layout

| offset | size | field | notes |
|---|---|---|---|
| 0x00 | u32 | `grid_w` | 256 (20 files) or 220 (12 files: scenario3 and scenario5). The reader discards it and takes sizes from the already parsed `.stg` grid; **verified** equal to `CreateGrid X` in the sibling `.stg` for all 32 files |
| 0x04 | u32 | `grid_h` | always equal to `grid_w` |
| 0x08 | u32 | `tile_scale` | stored at `world+0x50`, clamped to >= 1 (shipped: 4 in all 32). Divisor of terrain texture coordinates |
| 0x0C | u32 | `n_sectors` | 256 (S1L1, S4L1_*: 5 files), 64 (S2*, S6*, S7*: 15 files), 49 (S3*, S5*, the 220-cell maps: 12 files). **Verified**: equals the sibling `.stg`'s `CreateSceneSector` count and `sector_name[i] == stg.sectors[i].scene_id` for every i |
| 0x10 | n_sectors * 0x20 | `sector_name[]` | `scene_sector_N`, NUL padded (no garbage after the NUL). Resolved by name through 0x54E6A0 into the world sector table |
| 0x10 + 0x20*n | u32 | `n_textures` | 5..15 in the shipped files (`world+0x80`) |
| … | n_textures * 0x40 | texture entries | two `read_bytes(0x20)` per entry (0x4F7226 / 0x5582BD). First = terrain texture name (`rock_001`, `drygrass_L01`, 46 distinct names, copied into the cell by 0x4F6EB0). Second = another NUL-padded string with stack garbage after the NUL (`none` in 278 of 288 entries, otherwise `Grass01`, `grass 16`, `vegetation02`; only S1L1/S2L1/S2L1_2 differ). It is **not used** by 0x4F6EB0 (probably a surface/footstep material); TODO(re) meaning. One shipped name has a typo, `ro#k_002` (S4L1_2.gri), which must be kept verbatim |
| … | 16 blocks | `layer[k]`, k = 0..15 | each block is the RLE stream below and decodes to exactly `(grid_w+1) * (grid_h+1)` bytes (257² = 66,049 or 221² = 48,841), index `x*(grid_h+1)+y` (x-major), copied into byte `k` of `cells[x][y]` (0x4F6EB0) |
| … | per cell with `layer15 != 0`, x-major then y | sub-vertex data | for each of 5 channels one RLE block of 9 (level 1) or 25 (level 2) bytes. **Never present in the shipped data** (`layer 15` is all zero in all 32 files); the reader handles level 1 and 2 only |

The file ends right after the last block (verified: parse position == file size for all 32 files).

### Layer RLE (0x59DC10, escape byte 0xFF passed by 0x4F6EB0)
`decode(file, dst, escape=0xFF)`: read a byte `c`; if `c != 0xFF` store it as a literal; otherwise read `n`; `n == 0` terminates the block; else read
`v` and store `n` copies of `v` (`memset(dst + total, v, n)`). A literal 0xFF byte is therefore written `FF 01 FF`. The decoder returns the decoded
length; 0x4F6EB0 does not check it, in every shipped file it equals the plane size. Runs are at most 255 bytes, so a flat layer is `FF FF v` repeated plus a short tail
(S1L1 layer 0: `FF FF 01 | FF 01 01 | 00` per 257-byte column).

Grid dimensions are *vertex* counts minus one: arrays are `(grid_w+1) x (grid_h+1)`; one cell = 64.0 world units
(0x40), world origin of the grid at `world+0x44/+0x48` (floats added to `x*64`, `y*64`).

## Cell record (in memory only, 0x54 bytes) — [WorldGridCell](../re/structs/WorldGridCell.md)
The 16 `.gri` layers fill bytes 0..15 of the cell. Value sets observed over all 32 files:

| layer / byte | meaning | observed |
|---|---|---|
| 0 | cell-present flag (creates the quad/sector object in 0x4F6EB0 when non-zero) | {0,1}; equals 1 exactly for `x < grid_w && y < grid_h` in 30 files, differs in S3L1_2.gri and S5L1_2.gri (some quads switched off) |
| 1,2,3 | vertex colour R,G,B | byte (not tabulated; layer 1 of S1L1 starts with runs of 255) |
| 4 | alpha byte (vertex alpha = 0xFF - value in the plain path) | byte (not tabulated) |
| 5,6 | unknown | **all zero** in all 32 files |
| 7,8 | s16 height `z` (also used for the sector bounding box) | signed: -1742 .. +1752 over all files (pairs read little-endian: `lo = layer 7`) |
| 9,10 | s16 sector index (1-based into the sector-name table; 0 = default sector) | 0..`n_sectors`; in S1L1 `id = 1 + 16*(x/16) + y/16` (16x16-cell sectors; checked on that file only); 0 for the last row/column (x == w or y == h) |
| 11,12 | s16 primary texture id (1-based into the texture table; 0 = none) | 0..`n_textures` |
| 13,14 | s16 secondary (blend) texture id; blend weight `(a0+a1+a2+a3)/1020.0 >= 0.5` (four corner alpha bytes) chooses which texture is the base (0x4F74EA / 0x558140) | 0..`n_textures` |
| 15 | subdivision level 0/1/2 (=> 1x1, 2x2, 4x4 sub-quads; 9 or 25 sub-vertices) | **always 0** |
| 0x10..0x2F / 0x30..0x4F | filled at load time: primary / secondary texture file names copied from the texture table | — |
| 0x50 | pointer to the sub-vertex array (5 bytes per sub-vertex: R,G,B + s16 height offset) | unused in shipped data |

## Linking to the text files
`n_sectors` names are exactly the `CreateSceneSector` names of the `.stg` in the same order; `.uni` units carry `scene_name:` of the same
sectors; `.pln` entries are keyed by the same names (`sector_index` = position in this list). Grid size matches `CreateGrid X/Y`.

## Open questions
- TODO(re): meaning of the second string of a texture entry (`none`, `Grass01`, ...); 0x4F7226 copies only the first.
- TODO(re): layers 5 and 6 (always zero) and the colour/alpha semantics of layers 1-4 beyond the terrain builder 0x55B720.
- TODO(re): the sub-vertex section is implemented from the reader only; no shipped file exercises it.
- Request (resolved): a dump of the file at `0x2010` confirmed the 0x40-byte texture entries; the layer RLE was read from the disassembly of 0x59DC10.
