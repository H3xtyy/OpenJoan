# PlaneChunks — serialized `en3d::plane` mesh (inside `.pln` sector blobs)

Reader 0x5E26C0, writer 0x5E2380 (order identical), material reader 0x5E2B70. All fields little-endian; "u32" reads are
`File::read_u32` (0x5893C0). Chunk tags are ASCII stored as a u32 (bytes in file order).

| step | size | field | meaning / evidence |
|---|---|---|---|
| 1 | u32 | `vertex_type` | 1..8, selects vertex stride (table below); passed to 0x5CDC70 as `type & 0xFF`; plane `+0x04` |
| 2 | u32 | `n_parts` | number of part names; plane byte `+0x06` |
| 3 | n_parts * 0x40 B | `part_local[]` | **corrected (scene family review)**: the 0x40 bytes are each part's *local transform matrix* (D3DMATRIX), not a name: 0x5CBBF0 copies 16 dwords per part and 0x60EE60 multiplies them. Stored at stride 0xA0 in memory (the rest of [PlanePart](PlanePart.md) is runtime-only) |
| 4 | tag `PMAT` | | 0x54414D50 |
| 5 | u32 | `n_materials` | plane `+0x64` |
| 6 | per material | | 0x54 raw bytes (material image; texture-pointer slots `+0x24..0x33` and `+0x44..0x53` are zeroed after loading), then 4 texture slots: `u32 tex_arg` (0 = no texture; otherwise texture-load argument, 1 in builder calls) followed, if non-zero, by `char name[0x21]` |
| 7 | tag `PVTX` | | 0x58545650 |
| 8 | u32 | `pretransformed` | stored at plane `+0x18`. **Corrected**: every reader (0x5D0570, 0x5CEA30, 0x5D7BF0) only tests it against 0 — it is a "vertices already in model/world space, ignore part-0 matrix" flag, not an FVF (the FVF comes from the per-type table at 0xA17D88) |
| 9 | u32 | `n_vertices` | |
| 10 | n_vertices * stride | vertex array | `read_array(dst, n, stride)` 0x588C60; stride from 0x5CDAE0 |
| 11 | tag `PTRI` | | 0x49525450 |
| 12 | u32 | `tri_flag` | byte stored at plane `+0x08` |
| 13 | u32 | `n_tris` | |
| 14 | n_tris * 6 | indices | 3 x u16 per triangle |
| 15 | n_tris * 2 | `tri_material` | u16 per triangle (material index) |
| 16 | tag `PSTP` | | 0x50545350 |
| 17 | u32 | `has_sort` | plane `+0x51` |
| 18 | if `has_sort`: u32, then `n_materials` x u16, u32 `n_sorted`, `n_sorted` x u16, u32 `sort_mode` | | total tri count (`+0x52`), per-material triangle counts (`+0x54`), material-sorted index list (`+0x60/+0x5C`), mode byte (`+0x50`, 0x5D7270) |
| 19 | tag `PEX1`, u32 0, tag `PEX2`, u32 0, tag `PEND` | | extension slots (always 0) and terminator; any other tag fails the load |

## Vertex types (stride from 0x5CDAE0 / 0x5CDC70 / 0x5CD7F0)
| type | stride | notes |
|---|---|---|
| 1 | 0x24 | pos(3f) normal(3f) diffuse(u32 BGRA) uv(2f) — verified from the terrain builder 0x55B720 |
| 2 | 0x3C | type 1 + lightmap uv at 0x24 (2f); bytes 0x2C..0x3B TODO(re) |
| 3 | 0x1C | TODO(re) |
| 4 | 0x18 | TODO(re) |
| 5 | 0x48 | TODO(re) (only if device cap `DAT_00A52623`, else folded to type 2) |
| 6 | 0x14 | TODO(re) |
| 7 | 0x48 | TODO(re) |
| 8 | 0x28 | TODO(re) |

Full in-memory layout: [Plane.md](Plane.md) (vertex-type table there is more complete: type 4 = pos+diffuse, 5 = bump/tangent
frame, 7 = skinned, 8 = diffuse at 0x1C). Terrain `.pln` files use types 1 and 2 only (builder). The in-memory `plane` object is 0x11C bytes (`operator new(0x11c)` at
0x55F320); fields used here: `+0x00` part array, `+0x04` type, `+0x06` part count, `+0x10/+0x14` vertex count/capacity,
`+0x18` fvf, `+0x1C..+0x38` per-type vertex array pointers, `+0x40/+0x44` tri count/capacity, `+0x48` indices, `+0x4C` tri
material ids, `+0x50..+0x62` sort data, `+0x64/+0x68` material count/capacity, `+0x6C` material array (0x54 B each).
