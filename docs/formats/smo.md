# `.smo` / `.am1` — static and skinned mesh files (version 500)

Status: **verified against all 1,228 `.smo` and 588 `.am1`**: every file is consumed exactly to EOF, with the two exceptions in section 13. Layout first verified on 3 real files by byte table (
`highpolytree1_large.SMO` complete to the last byte, `unit/alexander#v.smo` header + first vertices,
`model/aaaafx_2nd.SMO` header + plane header) and by regex probes of `model/ripple.AM1`. Everything
marked **TODO(re)** is not verified. Reader: `RE: joan.exe 0x5E0530` (static v500),
`0x5DC420` (skinned v500). Function table: `docs/re/functions/smo.md`. Structs: `docs/re/structs/Smo*.md`.

All integers little-endian. `f32` = IEEE-754 single. No alignment padding anywhere (records are packed).
The reader is a sequential stream reader (`File`/`FileMem` `read_i32`, `read_block`, `skip`), so every
offset below is a running position, not a table lookup.

## 1. Family and naming

| Extension | Reader | Meaning |
|---|---|---|
| `.SMO` | static reader `0x5DDD70` -> `0x5E0530` | rigid mesh (items, buildings, props, `model/*.smo`, unit `#V` proxy meshes). modelres type byte `'S'` (0x53) |
| `.AM1` | skinned reader `0x5DA280` -> `0x5DC420` | skinned mesh: same plane/texture layout plus per-vertex bone influences. modelres type byte `'A'` (0x41) |

Evidence: `0x4B18D0` (modelres) compares the model-name extension to string `0x9D52B8` (`.SMO`) -> `'S'`, `0x9D52B0`
(`.AM1`) -> `'A'`; actordef loaders (`0x5B0000`, `0x5AFDD0`, `0x5AF3D0`, `0x5AF5D0`) append `.SMO` when the "skinned"
flag is 0 and `.AM1` when it is non-zero, then call `0x590B80` (file exists) and report
"Error loading 3D Model : %1s" if missing. `.AM3` (0x9CF5B4) is the animation file opened next to the mesh and is
documented by the anim family, `.AM2` (0x9CF5BC) likewise.

Companion naming for a unit mesh `<base>` (from the file tree and the loader loops at `0x5C0EF0`):

| File | Role |
|---|---|
| `<base>.AM1` | main skinned mesh (`0x5C0EF0` skinned branch, `param_2 != 0`) |
| `<base>#2.AM1`, `#3`, (`#4`) | additional levels of detail; loader tries `#1..#4` and skips ones that do not exist (`0x590B80`) |
| `<base>#s.AM1` / `#s.smo` | shadow-volume mesh; loader aborts with "Shadow volume model %1 does not have exactly 1 material" if plane count != 1 (string at `0x5C0EF0`) and forces its material mode fields |
| `<base>#V.SMO` | untextured static proxy mesh (hit/pick volume - **inferred**: texture count 0, plane texture index -1, see 4.3) |
| `<base>#N.AM3`, `#s.AM3` | animation for the matching mesh (anim family) |

The exact literals (`#`, `V`, `s`) are passed as unnamed string constants to `0x5A1550`/`0x5A14F0`, which Ghidra left
without arguments, so the `#`+digit/`s`/`V` suffix assignment is **inferred from file names**, not read from the code.
TODO(re): 0x5C0EF0 / 0x5C2370 confirm the literal arguments of the 400-char path-string helpers (0x5A1550, 0x5A14F0, 0x5A15D0 = append decimal int).

## 2. Version dispatch

`0x5DDD70` (static) reads one i32 `version`, then dispatches:

| version | handler | status |
|---|---|---|
| 0 | `0x5DDE90` | legacy layout (3-vertex-index+UV-index triangles, 0x18-byte vertices, 8-byte UV table); not found in shipped files |
| 50 (0x32) | `0x5DECF0` | legacy; not found in shipped files |
| 100 (0x64) | `0x5E0510` | stub, returns 0 (= load failure) |
| 500 (0x1F4) | `0x5E0530` | current; **every shipped file** (INVENTORY: all 1,228 `.smo` start `F4 01 00 00`) |
| other | - | returns 0 -> caller reports "Error loading 3D Model" |

`0x5DA280` (skinned) reads **two** i32: `version` then `quality` (call it `bone_budget`; value 10 in `ripple.AM1`), and
dispatches 0/50/100/500 to `0x5DA3D0`/`0x5DB710`/`0x5DC400`(stub)/`0x5DC420`. `quality` is passed to the model
constructor (`0x5CC7A0` 4th arg). If a debug global (`DAT_00A5262B`) is set and `5 <= quality <= 24`, vertex mode 7
(2-weight, 0x48-byte vertex) is selected, else mode 1 (0x24-byte vertex). TODO(re): 0x5DA280 meaning of `quality` (bone count? skin LOD?).

## 3. Header (static `.SMO`, v500)

| Offset | Size | Type | Field | Notes / evidence |
|---|---|---|---|---|
| 0x00 | 4 | u32 | `version` = 500 | `0x5DDD70` |
| 0x04 | 4 | i32 | `plane_count` | sub-meshes ("planes"), `0x5E0530` first read; allocates that many `SubMesh` records via `0x5CD470` |
| 0x08 | 4 | i32 | `texture_count` | length of the texture table (section 5) |
| 0x0C | 4 | i32 | `lod_count` | number of extra LOD geometry blocks (section 6); 0 in all verified samples |
| 0x10 | 16 | f32[4] | `bounds` | 4 floats; sphere-like. Verified values below. 4th is multiplied by a global (`DAT_009CF794`) and passed with `lod_count` to `0x5D4720` (stores LOD count at model+0xE4 and distance at +0xE0), so it is the length used for LOD switching/culling |
| 0x20 | ... | | planes | `plane_count` plane records (section 4) |

Verified header values:

| File | 0x04 | 0x08 | 0x0C | 0x10..0x1F (f32x4) |
|---|---|---|---|---|
| `item/Object/Models_Items/Vegetation/Trees/FakeTrees/highpolytree1_large.SMO` (313 B) | 1 | 1 | 0 | `0.0, 3.4477e-5, 355.133, 1493.89` |
| `unit/alexander#v.smo` (3735 B) | 1 | 0 | 0 | `0, 0, 0, 192.0` |
| `model/aaaafx_2nd.SMO` (7731 B) | 1 | 1 | 0 | `1.759e-6, 0, 30.682, 80.498` |

Tree sample: farthest vertex is `(+-657.1, 0, 710.3)` from `(0,0,355.1)`, distance 747.1; 1493.89 = 2 x 747.1, so the 4th
float behaves like the **diameter** of the bounding sphere centred on (x,y,z) = floats 0..2 (**inferred**, one sample;
alexander has centre 0 and 192).

### 3a. Header of the skinned `.AM1`

```
0x00 u32 version = 500
0x04 i32 quality          (10 in model/ripple.AM1; INVENTORY census of second dword: 1 / 2 / 3 in .smo, 10 in ripple.AM1)
0x08 i32 plane_count
0x0C i32 texture_count
0x10 i32 lod_count        (read by 0x5DC420 as local_20; ignored later - no LOD block loop in the skinned reader)
0x14 f32[4] bounds        (0x5DC420 copies them into the skin object; 4th /= DAT_009CF794)
0x24 planes ...
```
Verified on `model/ripple.AM1`: `F4 01 00 00 | 0A 00 00 00 | 01 | 01 | 00 | 4 floats | 02 00 00 00 | 04 00 00 00`
(INVENTORY seed bytes) and the texture name `ripple.tga` begins at file offset **336** (regex probe: matches at
`\A.{336}ripple\.tga`, not at 335, 337-340, 344, 360). Note the static `.SMO` census value "second dword = 1,2,3" in
INVENTORY is therefore `plane_count` for static files.

## 4. Plane (sub-mesh) record, v500

Repeated `plane_count` times starting at offset 0x20 (static) / 0x24 (skinned). Verified in full on
`highpolytree1_large.SMO` (plane at 0x20..0x103):

| Rel. offset | Size | Type | Field | Evidence |
|---|---|---|---|---|
| +0x00 | 4 | i32 | `tri_count` | number of triangles of the strip; the index list has `tri_count + 2` entries. tree: 2 |
| +0x04 | 4 | i32 | `vertex_count` | tree: 4 |
| +0x08 | 16 | f32[4] | `color0` | 4 floats, each converted with `__ftol` to a byte and packed into a D3DCOLOR (`0x5D5570` setter) |
| +0x18 | 16 | f32[4] | `color1` | setter `0x5D5540` |
| +0x28 | 16 | f32[4] | `color2` | setter `0x5D55A0` (second arg 0) |
| +0x38 | 16 | f32[4] | `color3` | setter `0x5D55E0` |
| +0x48 | 16 | f32[4] | `plane_bounds` | read into a scratch buffer and never used by the loader; in tree/alexander/aaaafx it equals the header `bounds` |
| +0x58 | 4 | i32 | `texture_index` | index into the texture table (section 5); **-1 = untextured** (alexander `#v`: 0xFFFFFFFF). Matching is done by the loader loop `for each plane: if tex_idx_of_plane == texture k -> bind` |
| +0x5C | 2*(tri_count+2) | u16[] | `strip_indices` | triangle strip, **plane-local vertex numbers** (loader adds the plane's base vertex index returned by `0x5CD7F0`). **Present only when `tri_count > 0`** (`if (0 < tri_count)` in `0x5E0530`/`0x5DC420`); `item/Object/Models_Unique/Scenario2/watchtower.SMO` has an empty plane `tri_count=0, vertex_count=0, texture_index=-1` that is only the 0x5C-byte header. Every index is `< vertex_count` in all files |
| ... | 32*vertex_count | SmoVertex[] | `vertices` | section 4.2 (static) / 4.4 (skinned) |

**Skinned planes (`.am1`) have only three colours** (`0x5DC420` reads three 16-byte colour blocks, then the 16-byte bounds): the record is `tri_count, vertex_count, color0..2 (48 B), plane_bounds (16 B), texture_index`, i.e. the strip starts at +0x4C (`0x70` for the first plane, as `am1.md` observes). The colour setters are `0x5D5570`, `0x5D5540`, `0x5D55A0` in that order.

Colour sample data: tree `color0 = (1.0, .588, .588, .588)`, `color1` same, `color2 = (1.0, .9, .9, .9)`,
`color3 = (1.0, 0, 0, 0)`; alexander `1,1,1,1` x3 then `(1,0,0,0)`; aaaafx `(1,.929,.949,.945)` x2,
`(1,.9,.9,.9)`, `(1,0,0,0)`. In every sample float #0 is 1.0. The setters write the four `__ftol` results into bytes
D3DCOLOR order but Ghidra lists the four `__ftol` calls without arguments, so **which file float is A vs R/G/B is
unresolved**; the data pattern `(1.0, v, v, v)` strongly suggests `(A, R, G, B)` or a 1.0 scale/alpha first.
TODO(re): 0x5D5570 0x5D5540 0x5D55A0 0x5D55E0 (not analysed) - decide channel order and which colour is
diffuse / ambient / specular / emissive (SubMesh offsets 0x00..0x0F, see ModelMesh.md).

**Verified winding**: these are proper triangle strips with the usual `+2` convention (`tri_count + 2` indices, triangle `i` = `(idx[i], idx[i+1], idx[i+2])`). With the parity swap described next, `cross(p1-p0, p2-p0)` points to the same side as the vertex normals for >97% of triangles (128,731 vs 2,183 over 300 random `.smo`; the rest are degenerate/mirrored authoring), i.e. the expanded triangles are **counter-clockwise seen from the normal side** and need no flipping for a right-handed viewer's front face. The expansion rule is the engine's (below).

The strip is expanded at load time: for `i` in 2.. the triple `(i-2, i-1, i)` is emitted unless two of the three indices are
equal (degenerate), with the first two operands swapped on alternate triangles (parity bit toggled every step) to keep
winding. Counting pass first (`0x5CE6F0` allocates the triangle slots, `0x5CE800` writes one triangle). Verified
qualitatively: alexander's strip contains repeated indices (`0B 00 0B 00 0B 00 0B 00` at 0xA8) used for stitching.

### 4.2 SmoVertex (static, 32 bytes) - verified

| Offset | Type | Field | In-memory (`stride 0x24`) |
|---|---|---|---|
| 0x00 | f32[3] | `normal` | model vertex +0x0C |
| 0x0C | f32[3] | `position` | model vertex +0x00 |
| 0x18 | f32[2] | `uv` (u, v) | model vertex +0x1C, +0x20 |

The loader copies file bytes 12..23 to memory +0x00 (position), 0..11 to +0x0C (normal), sets +0x18 (diffuse) to a global
default (`DAT_00A69CD0`, white) and copies u,v. The memory vertex is D3DFVF_XYZ | NORMAL | DIFFUSE | TEX1 (12+12+4+8 = 36 = 0x24).
Verified: tree vertex 0 at 0x84: normal `(0,-1,0)`, position `(-657.123, 3.4e-5, 710.267)`, uv `(0.0005, -0.9995)`;
alexander#v vertex 0 at 0x2FA: normal `(-0.352, 0.896, -0.274)` (unit length), position `(-3.82, 8.65, -77.66)`, uv `(0,0)`
(vertex starts exactly after `0x7C + 2*(317+2)`).

### 4.3 Untextured proxy planes
`alexander#v.smo`: `texture_count = 0`, plane `texture_index = -1`, uv all 0, 317 triangles / 92 vertices; i.e. geometry only.

### 4.4 Skinned vertex (`.AM1`, `0x5DC420`)

Per vertex, in place of the 32-byte block (positions/normals are rebuilt from the bone offsets):

| Order | Size | Field |
|---|---|---|
| 1 | 8 | `uv` (u, v) -> model vertex +0x1C/+0x20 (type 1) or +0x40 (type 7) |
| 2 | 4 | i32 `skip_dwords` (A): that many dwords are then skipped (`0x5888C0(A*4, 1)`) |
| 3 | 4 | i32 `influence_count` (B) |
| 4 | 4*A | skipped payload |
| 5 | B x 32 | influences: `i32 bone_index; f32 weight; f32[3] offset; f32[3] normal_offset`; each passed to `0x5F4970(plane_skin_idx, bone, offset, normal_offset, weight)` |

After the B loop `0x5F49C0` finalises the vertex. For mode 7 (0x48-byte vertex) the first two influences are copied into the vertex
(two position+normal pairs, two 16-bit bone ids x3 at +0x30, two weights at +0x34) and weights beyond 2 are dropped
(`if B > 2` flag).

**Resolved (was "open: ripple.AM1 0x90 mismatch").** The earlier probe assumed `tri_count` indices; the strip has `tri_count + 2`
entries (as in the static reader), and the skinned plane has only 3 colours. With those, `model/ripple.AM1` parses to byte 336
(= start of the texture name): plane at 0x24 (`tri_count 2, vertex_count 4`, colours 0x2C/0x3C/0x4C, bounds 0x5C, texture_index 0x6C), strip `0 1 2 3` at 0x70,
vertices from 0x78. Vertex 0 = `uv (0,-1) | A=1 | B=1 | A dwords | 1 influence`. A is the **count of extra dwords**, which are
(in 89% of the 210,504 sampled values) plane-local vertex numbers `< vertex_count` (neighbouring/duplicate vertices, unused by the v500 loader);
B is the influence count; all 43,706 sampled vertices have weights summing to 1.0. Observed over the install: `A` is mostly 2..9 (most frequent 6, 5, 4, 7), `B` 1..7 (305,547 vertices with 1, 35,098 with 2, 1,179 with 3, 199 with 4, 60 with 5, 5 with 7), so the file may carry more than the 4 the engine keeps.
`position`/`normal` exist only as bone-local offsets, so a bind-pose mesh needs the `.am3` skeleton.

## 5. Texture table (after the last plane)

`texture_count` records, each:

| Size | Type | Field |
|---|---|---|
| 20 | char[20] | `name` NUL-padded; the extension (last '.') is cut off by the loader and the base name looked up in the texture cache (`0x60A4B0`); verified `highpolytree.tga` at 0x104 |
| 4 | i32 | `data_size`: 0 -> texture is loaded by name from disk through `0x6099C0(name, 1, flag)`; > 0 -> `data_size` bytes of an embedded image file follow and are created in memory (`D3DXCreateTextureFromFileInMemory`-style call at `0x765C3D`) and registered under `name`. If the texture is already in the cache the embedded bytes are skipped |

Verified: tree 0x104..0x117 name, 0x118 `data_size = 0`. After loading, every plane whose `texture_index == k` gets
texture `k` bound (`0x5D5650`). TODO(re): is the extension in the stored name always `.tga`, and the lookup root folder (texture/..., 0x6099C0).

## 6. LOD blocks (static reader only)

If `lod_count > 0`: for each LOD level, the reader creates a lower-detail `Model` (`0x5D4470`) and for every plane `p` reads
`i32 tri_count; i32 vertex_count; u16 strip_indices[tri_count+2] (only if tri_count > 0); SmoVertex[vertex_count]` - i.e. a plane without
colours/bounds/texture index (those are inherited). `0x5D4A00` attaches the level; afterwards `0x5D4720(lod_count, bounds[3] * DAT_009CF794)`.
No shipped sample has `lod_count > 0` yet (all three verified = 0). TODO(re): confirm with a file where 0x0C != 0.

## 7. Trailer: tagged attribute lists

After the texture table (and LODs) the reader loops `tag = read_i32`:

| tag | Content (per plane, in plane order) | Effect |
|---|---|---|
| 1 | `i32 A; i32 B` | `A != -1`: plane marked transparent/alpha-blended (SubMesh +0x40 = 2, or 1 if the texture reports `type==1`; +0x38 = 1; +0x34 = 0x20000). `B != -1`: B is a texture-table index for an extra layer; with a debug flag (`DAT_00A52627`) set it enables the 2-3 layer blend combinations (+0x34/+0x38 = 8/9 or 10/11), else only when `DAT_00A5262F` |
| 2 | `i32 C; i32 D; u8 E` | `C == -1`: plane colour 3 set to default; else SubMesh +0x10 = 50.0f (specular power) and +0x38 defaults to 0x14 if 0. `E != 0`: low nibble of SubMesh +0x20 = 2. D is read and ignored |
| 3 | `i32 F` (static reader only) | `F != 0`: every vertex normal is overwritten with the constant vector at `DAT_00A18A60` (flat-lit mesh) |
| anything else (0, or EOF -> reads 0) | - | terminates the list |

The tags may come in any order and repeat (the original loops until a non-1/2/3 value). Shipped order: `1, 2, [3], 0` (static: 1,2 in 42 and 1,2,3 in 57 of 150 sampled files; skinned: always 1,2). Value sets (sample of 150 files): tag 1 `A` in {-1, 0, 1, 2, 6}, `B` always -1; tag 2 `C` in {-1, 0, 1}, `D` always -1, `E` in {0, 1}. Full sweep: tag 3 never occurs in `.am1`, no tag value other than 0..3 occurs.

Verified end-to-end on `highpolytree1_large.SMO`: `tag=1, A=0, B=-1`, `tag=2, C=-1, D=-1, E=0x00`, `tag=0`
(file length 0x139 = 313). Tag semantics are read from code, only the (A=0,B=-1,C=-1,D=-1,E=0) combination is verified.
The skinned reader supports tags 1 and 2 only.

## 8. Worked example: `highpolytree1_large.SMO` (313 bytes)

```
0x000 f4010000 version 500          0x004 1 planes   0x008 1 textures   0x00C 0 lods
0x010 bounds (0, 3.4477e-5, 355.133, 1493.89)
0x020 tri_count 2, vertex_count 4
0x028 color0..color3 (4 x 16 B)     0x068 plane_bounds (16 B)     0x078 texture_index 0
0x07C indices 0,1,2,3 (u16 x4)      0x084 4 vertices x 32 B  -> ends 0x104
0x104 "highpolytree.tga" + pad (20 B) 0x118 data_size 0
0x11C tag 1: A=0 B=-1   0x128 tag 2: C=-1 D=-1 E=0   0x135 tag 0   -> EOF at 0x139
```

## 9. In-memory model

See `docs/re/structs/ModelMesh.md` (0x11C `Model`, 0x54 `SubMesh`), `ModelSet.md` (0x69 container with LOD models),
`ActorDef.md` (actor definition + modelres table entry). Eight vertex layouts exist (type byte at Model+4; 0x24, 0x3C, 0x1C,
0x18, 0x48, 0x14, 0x48, 0x28 bytes; `0x5CD7F0`); only types 1 (static) and 5/7 (skinned) are produced by the v500 readers.

## 10. Async vs sync loading

`0x5C0EF0` (ModelSet::load, synchronous, spins `SleepEx(5)` while a previous async job is pending) and `0x5C2370` (starts the
same load as a chain of `0x164`-byte async file objects, one per file, later consumed by the pump `0x5C3110`; `0x5C4E30` =
"all done"). They differ only in I/O; the parse code is the same readers. `param_2` (short) = skinned flag (0 = `.SMO` static,
non-zero = `.AM1` skinned + a `0x3E`-byte skin object `0x5F45F0` at ModelSet+0x2C).

## 11. `.mda`

Not read by the mesh readers of this family. It is the morph-target file read by `en3d::morph` (`0x5C6820`, `0x5C9B10`); the `modelres`
table (`0x4B18D0`) only collects the "MORPH" resource rows per model. Spec: `docs/formats/mda.md` (owned by the morph family).
Its part section lists vertices by plane-local index and therefore depends on the plane/vertex order defined above.

## 12. Texture table facts (install)

No shipped mesh embeds an image (`data_size == 0` in all 2,485 entries); `texture_index` is `-1` for 220 planes and a valid table index for the other 2,636; every `lod_count` is 0, so the LOD
reader (section 6) has not been exercised on real data.

## 13. Install exceptions (all 1,816 files)

* `item/Object/Models_Items/Vegetation/Grass/grass03.SMO` (748 B): one trailing byte (0x00) after the terminating tag 0 (tag chain `1, 2, 3(F=1), 0`, then 1 stray byte). The engine stops at tag 0 and never notices; a reader sees one unused byte.
* `item/ripple.AM1` and `model/ripple.AM1` (360 B): end after the tag-1 payload with no tag 0 (EOF reads as tag 0 in the original).
* `item/Object/Models_Unique/Scenario2/watchtower.SMO` contains an empty plane (see section 4).
* `.am1` header dword `quality` takes 34 distinct values 1..46 and 133 (census: 23 -> 201 files, 1 -> 106, 24 -> 72, ...); the meaning (bone count?) is still unverified.

## 14. Reading notes

1. Read `version`; only 500 occurs. Use `.SMO` -> static path, `.AM1` -> skinned path (extra `quality` int and skin vertex data).
2. Parse header, planes (section 4), texture table (5), LOD blocks (6, static), trailer (7).
3. Triangle expansion as in section 4. Keep strip indices plane-local; add the plane base vertex.
4. Every shipped file is consumed exactly to EOF (the trailer loop ends on tag 0 at EOF - 4).

## Versions, axes and strips

- **Versions.** Every shipped file is version 500; the readers also dispatch on 0, 50 and 100, which no shipped file uses.
- **Axes.** Positions come straight from 3ds Max: Z up, characters face -Y, Joan is about 79 units tall. The data is left-handed Direct3D data
  ([am1.md](am1.md)): the original feeds it to the Direct3D 8 device without a handedness conversion (the readers 0x5E0530 and 0x5DC420
  do none, see section 4); "right-handed" only describes how the source scene looks in 3ds Max. The projection is left-handed (`D3DXMatrixPerspectiveFovLH` 0x007674a4, called from `src/en3d/view3d.cpp`).
- **Strip winding.** Triangle winding alternates along the strip and degenerate triangles join the pieces; they are about 45 percent of
  the strip triangles (measured over the shipped files), so a reader that rebuilds triangle lists can drop them.
- **Texture table.** `name[20]` is the original source name (`.tga` or `.bmp`); the game loads the `.dds` with the same base name from
  `texture/**` (for example `texture/unit/` for unit models). `size > 0` would embed an image; no shipped file does.
- **Trailing sections** (ids 1..3, terminated by an id outside that range or by EOF): section 1 holds two i32 per mesh, always -1 so far
  (probably extra texture stages); section 2 holds two i32 and a u8 per mesh (the u8 is 1 on some map meshes); section 3 holds two u32
  (zeros so far) and ends the file. Section 13 above has the verified details.
