# `.mda` — morph data (vertex morph targets), 17 files

Reader: `en3d::morph` (`d:\ww\en3d\morph.cpp`), `.MDA` literal at 0x9D95FC. Two readers for the same stream:
`0x5C6820` (`Morph::load(model_name, dir, name, transform?)`, first/base load; allocates the target array) and `0x5C9B10` (`Morph::load_append`, merges the
targets of another file into an existing `Morph` by target *name*, 0x5C90A0 = find target by name). Error text: `Error loading Morph Data : %1s`.
Both take an optional `D3DMATRIX*`: when non-null every position is transformed with `point_transform` and every normal/delta with
`vector_transform` (used to put the data into model space). `0x5C7370` is the copy constructor, `0x5C7B80` frees.
Reader primitives are `FileMem` (in-memory, 0x587E20 `read_i32`, 0x5873A0 `read_block`, 0x587650 `read_string(buf, 50)`).

## Layout (little-endian, packed)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0 | 4 | i32 | `n_parts` | number of model parts (planes) this file describes |
| 4 | per part | `Part[n_parts]` | | |
| next | 4 | i32 | `n_targets` | number of named morph targets (`Morph+0x34`) |
| next | per target | `Target[n_targets]` | | |

`Part` (one per plane of the model, in plane order):

| size | type | meaning |
|---|---|---|
| 4 | i32 | `n_vertices` — vertices of the plane (used to advance the global vertex base `Morph+0x3C`) |
| 4 | i32 | `n_affected` — vertices that can move |
| 28 * n_affected | `{ i32 local_index; f32 base_pos[3]; f32 base_normal[3] }` | base pose of each affected vertex; stored global index = `local_index + vertex_base_of_part` |

`Target` (a named blend shape):

| size | type | meaning |
|---|---|---|
| 50 | char[50] | target name (`read_string`, 0x32 bytes) |
| for each of the `n_parts` parts: | | |
| 4 | i32 | `n_records` |
| `n_records` x 52 | `{ i32 local_index; f32 v0[3]; f32 v1[3]; f32 v2[3]; f32 v3[3] }` | per moved vertex; destinations are `Target+0x34` (v0), `+0x3C` (v1), `+0x38` (v2), `+0x40` (v3) arrays of `n_affected_total` vec3s |

The reader finds the destination slot by scanning the base-pose index list for `vertex_base_of_part + local_index` (sequential search, indices ascending).
Arrays are zero-initialised for vertices a target does not mention. **Semantics (verified exactly on all 17 files, 4,275 records in `unit/joan_hi.mda` alone):** `v0` = absolute target position, `v1` = absolute target normal, `v2` = `v0 - base_pos` (position delta) and `v3` = `v1 - base_normal` (normal delta); float equality holds bit-for-bit.

## Worked example — `Grass01_M.MDA` (inventory head)

```
01 00 00 00          n_parts = 1
08 00 00 00          n_vertices = 8
04 00 00 00          n_affected = 4
00 00 00 00 | 00 80 01 42 b7 98 6a 41 e8 1f 1f 42 | 36 3a 23 b4 36 34 70 bf cf 10 b1 3e
                     idx 0, pos (32.5, 14.62, 39.77), normal (-1.5e-7, -0.94, 0.347)  (unit length)
02 00 00 00 | 00 80 01 c2 c2 98 6a 41 e8 1f 1f 42 | ...   idx 2, pos (-32.5, 14.62, 39.77)
```
### Target section of `Grass_C.MDA` (1342 bytes; one part with 16 vertices / 8 movable) - dumped
```
0xEC  05 00 00 00                      n_targets = 5   (base-pose block is 4 + 8 + 8*28 = 0xEC bytes)
0xF0  "GrassC" (50 bytes)               target 0: name 0xF0..0x121, n_records at 0x122 = 0 (rest pose)
0x126 "GrassC_A1" (50) | 0x158: 04 00 00 00   target 1, 4 records
0x15C 04 00 00 00 | 06 38 fd 40 09 d7 c4 c1 b9 6b 31 42 | 00 00 00 00 00 00 00 00 00 00 80 3f | b8 80 31 40 00 00 00 00 00 6c 45 bf | 0 0 0
       idx 4 | v0 (7.91, -24.60, 44.36) | v1 (0, 0, 1) | v2 (2.77, 0, -0.77) = v0 - base (5.14, -24.60, 45.13) | v3 (0, 0, 0)
```
Each record is 52 bytes (`i32 index + 12 f32`); the record vertices of every target must exist in the part's base pose (holds in every shipped file).

Install facts (17 files): one part in 16 files, four parts in `unit/joan_hi.mda` (682/1391/246/1596 vertices; 1/0/0/571 movable; 9 targets `facial_*`); the 4 flag files have 1 part, 121 vertices, 48 movable, 12 targets;
base-pose indices are strictly ascending everywhere; the stream tiles each file exactly (no trailer).

## Confidence
Part section: **verified** (bytes + code). Target section: **verified** (bytes + code + the delta identities above).
