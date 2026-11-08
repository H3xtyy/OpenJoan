# `.pln` — baked terrain mesh cache (`map/scenarioN/<name>.pln`, 3 shipped files)

Shipped: `map/scenario1/S1l1.pln` (5,735,818 B; note the lower-case `l`, file names are matched ignoring case, see [vfs.md](vfs.md)) and two in
`map/scenario3/` (`S3l1_2.pln`...). Reader **0x55F320** (`world_s.cpp`, error text `"invalid pln format"`), writer = the
tail of the terrain builder **0x55B720** (+ **0x55F9B0**, **0x5E2380**). All addresses `joan.exe`.

**What it is.** A *cache*: the terrain meshes of every scene sector, generated at load time from the `.gri` cell grid
(see [gri.md](gri.md)) and written out by the builder when the global flag `DAT_00A0B6B0` is set. The level loader
0x4F2F80 tries the `.pln` first (only when `DAT_00A0B6AC` is set and the file is non-empty); if it is missing or invalid the
terrain is rebuilt from the `.gri` (so an implementation can *ignore* `.pln` and rebuild from `.gri`, or load it for speed).
All integers are little-endian; the file is read through a `FileMem` (whole file in memory) so offsets are absolute.

> **Status: verified.** All 3 shipped files and all 354 sector blobs follow the layout below: every blob ends exactly at the next table
> offset and the sentinel offset equals the file size. Items still marked `TODO(re)` below are unverified.

## File layout

| offset | size | field | evidence |
|---|---|---|---|
| 0x00 | u32 | `hw_variant` flag (0/1; **value set in the install: {1}**). Reader computes `expected = (DAT_00A52508 >= 3 && (DAT_00A524fc & 0xFFFF) >= 4)` (a D3D device-capability test, probably >=3 texture stages) and **rejects the file if `(hw_variant != 0) != expected`**. Shipped files: `1` (lightmap vertex variant). | 0x55F320 |
| 0x04 | u32 | `version` = **3** (reader fails with "invalid pln format" otherwise) | 0x55F320; S1l1.pln 0x04 = 3 |
| 0x08 | u32 | `n_alloc` — table has `n_alloc + 1` entries (last = sentinel) | S1l1.pln 0x08 = 0x100 |
| 0x0C | u32 | `n_entries` — number of entries the reader iterates (`<= n_alloc`; equal in all three files: 256 / 49 / 49) | S1l1.pln 0x0C = 0x100 (S3l1_2.pln: both 0x31) |
| 0x10 | (n_alloc+1) * 40 | sector table, see below | reader allocs `(n+1)*0x28` and reads it in one block |
| entry[i].offset | … | sector blobs | |

### Table entry (40 = 0x28 bytes) — struct [PlnSectorEntry](../re/structs/PlnSectorEntry.md)

| +off | size | field | meaning |
|---|---|---|---|
| 0x00 | char[0x22] | `name` | scene sector name, NUL padded, e.g. `scene_sector_13` |
| 0x22 | u16 | `sector_index` | 1-based index of the sector in the world's sector table (`.stg`/`.gri` order). Fast path: reader checks `1 <= idx <= sector_count` **and** `strcmp(sector_name(idx), name)==0`; otherwise it looks the sector up by name (0x54E6A0). |
| 0x24 | u32 | `offset` | absolute file offset of the sector blob; the sentinel entry's offset is the end of the last blob (writer sets it from `tell()` after each blob) |

Worked example (S1l1.pln, dword table at the start of the file): entry 0 at 0x10 = `scene_sector_13`, `sector_index` at 0x32
= 0x000D, `offset` at 0x34 = 0x2838 = 10296 = 0x10 + 257*40 (blobs start right after the table). Entry 1 at 0x38 = `scene_sector_12`
(index 12, offset 0x801D); entries run **in descending sector order** (13,12,11,…,1) as the builder emits them. Entry
offsets increase monotonically with the entry number. **Verified:** `sector_index` is the 1-based position of `name` in the sibling `.stg`'s
`CreateSceneSector` list for every one of the 354 entries, and `name2` of the blob always equals the table name.

### Sector blob (at `offset`)

| size | field | notes |
|---|---|---|
| u32 | `present` = 1 | reader skips the blob if not 1 (0x55F320); always 1 in the shipped files |
| 0x21 B | `name2` | sector name copy (written from the sector record; reader only discards it) |
| … | plane | one [PlaneChunks](../re/structs/PlaneChunks.md) mesh (the sector's terrain mesh) read by 0x5E26C0 |

Sectors without a mesh get no blob. After loading, the reader marks the plane (`plane+0x70 = 2`), rebuilds the per-vertex
tri index (0x5CF1D0), and attaches it to the scene sector. The reader does **not** verify `n_entries` against the
table's sentinel.

### Plane chunks as found in the files (verified)
All 354 blobs have the shape: `u32 vertex_type = 2`, `u32 n_parts = 1`, one 0x40-byte part record, `PMAT`, `PVTX`, `PTRI`, `PSTP` with
`has_sort = 0`, `PEX1 0`, `PEX2 0`, `PEND`. Observed value sets (354 blobs): `vertex_type` {2}, `n_parts` {1}, `fvf` {1} (the field is **not** a D3D FVF
bit mask in these files; it is 1 for vertex type 2), `tri_flag` {0}, `has_sort` {0}. Vertex counts {289, 841, 957, 1089} = (cells+1)^2 grids
of 16x16, 28x28, 32x28 and 32x32 cells; triangle counts are 2 per cell (512 x256, 2048 x72, 1792 x23, 1568 x1) except two sectors with 1566 and 1788 (2 resp. 4 triangles fewer than the full grid; the 1792-triangle grids are 32x28 cells). All triangle indices are `< n_vertices` and all triangle material ids `< n_materials`.

**Correction to `PlaneChunks.md`:** the `n_parts * 0x40` bytes are **4x4 float matrices** (row-major D3DMATRIX), not names: the single record in every
blob is the identity matrix (`00 00 80 3F`, zeros, ... ; S1l1.pln 0x2862).

Material (PMAT entry): 0x54 raw bytes + four texture slots `{u32 tex_arg; char name[0x21] if tex_arg != 0}`. Verified for all 1,459 materials:
`slots` pattern is either `[tex, lightmap, -, -]` (651) or `[tex, tex2, lightmap, -]` (808); `tex_arg` is 1 for every used slot; the **last used slot is always the
per-sector lightmap** named `<level>_<sector>` (`S1L1_scene_sector_13`); the other slot names are `.gri` texture names (checked against the sibling `.gri`
texture table). The u32 at material `+0x34` is `0x12` for the 2-slot and `0x13` for the 3-slot pattern (= 0x10 + number of used slots, blend-mode flags of 0x55B720).
Material bytes `+0x00..+0x07` are `F0F0F0FF F0F0F0FF` (diffuse/ambient colour 0xFFF0F0F0), `+0x1C` is `FFFFFFFF`, `+0x24..+0x33` hold the (stale) texture pointers that the reader zeroes.

Vertex type 2 (stride 0x3C = 60), verified on S1l1.pln blob 0: `+0x00` position 3f (world units; x 0..16384, y 0..28160, z -1034..1640 over all files),
`+0x0C` normal 3f (unit length, already computed, not the (0,0,1) default), `+0x18` diffuse u32 (0xAARRGGBB; observed 0x??C8C8C8, alpha varies),
`+0x1C` uv0 2f (tile repeats, `4.0 .. 0.0` in steps of 0.25 across a 16-cell sector, i.e. `tile_scale` 4 divides the cell index), `+0x24` lightmap uv 2f (0..1),
`+0x2C..+0x3B` **16 bytes of uninitialised memory** (zeros for most vertices, heap garbage for edge vertices, e.g. `bc008100 04100000 88e98d0a`); kept raw, never read by the game's
vertex declaration. Vertex type 1 (stride 0x24) is the same layout without the lightmap uv and the tail; it occurs in no shipped file.
Vertex types 3..8 have the strides 0x1C, 0x18, 0x48, 0x14, 0x48, 0x28 (not seen in the shipped files).

Chunk size relations (verified by the strict chain over all 354 blobs): `PVTX` payload = `n_vertices * stride(type)` (type 2: 60); `PTRI` payload = `8 + n_tris*6 + n_tris*2` after the tag;
`PSTP` is `tag + u32 0` when `has_sort = 0`; `PEX1 0 PEX2 0 PEND` close the blob; each blob therefore ends exactly at `entry[i+1].offset`; the first blob starts at `0x10 + (n_alloc+1)*0x28`; the
sentinel offset equals the file size.

## What the mesh is (from the builder 0x55B720)
For each sector (terrain cells with the same sector id, `.gri` cell `+0x09`): a regular quad grid, **64.0 world units per
cell** (`x = cell_x * 64`, `y = cell_y * 64`, z = height from the plasma height sampler 0x4C7B70 + cell `+0x07` heights),
two triangles per quad with the diagonal flipped in a checkerboard (`(cx+cy) & 1`). Cells with subdivision level `+0x0F` = 1
or 2 are expanded to 2x2 / 4x4 sub-quads (`(n+1)^2` sub-vertices from the cell's 5-byte sub-vertex records). Vertex type 1
(stride 0x24): pos(3f), normal(3f)=(0,0,1) [recomputed later], diffuse (4 bytes: B,G,R from cell `+3,+2,+1`, A = 0xFF - cell `+4`
or the sampled height byte), uv0 = (`(maxX+1-x)/tile`, `(maxY+1-y)/tile`) with `tile` = world scale (`.gri` header u32 #3).
Vertex type 2 (stride 0x3C, lightmap variant, used when `hw_variant`=1 and a per-sector lightmap texture exists) adds a
lightmap uv at +0x24/+0x28; texture `"lightmap_default"` is the fallback lightmap (0x55B720 literal). Each quad gets a
material from the cell texture ids (`+0x0B` primary, `+0x0D` secondary/blend; blend mode flags 0x11/0x12/0x13/0x20000 in
material `+0x34`). Triangles are grouped per material.

## Open questions
- TODO(re): 0x5E2A90 (material writer) is not analysed; the 0x54-byte material image is kept raw. Known bytes: see above; the rest is not decoded.
- TODO(re): exact D3D FVF per vertex type (table at `DAT_00A17D88`, 16 dwords); the `fvf` field in the files is 1.
- TODO(re): vertex types 1 and 3..8 and the `has_sort != 0` branch of PSTP never occur in the install (implemented from the reader only; layout of the sort block unverified).
- Resolved: first blob header (`present`, `name2`, plane) confirmed on S1l1.pln 0x2838; entry order is descending inside runs (13..1, 29..14, 43, 42, 50, 41..30, 75, 57..51, ...), not globally sorted - do not rely on it, use `sector_index`.
