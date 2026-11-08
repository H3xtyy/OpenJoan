# Plane (en3d mesh object, `sizeof` = 0x11C)

The engine class declared in `d:\ww\en3d\plane.cpp`. Despite the name it is a **multi-material triangle mesh with a part
(bone/transform node) array, strip data, LOD links and cached GPU buffers**, not a geometric plane. Static `.smo`, skinned
`.am1` and every terrain sector are loaded into one of these. Serialised form: [PlaneChunks.md](PlaneChunks.md); related
file structs: [SmoPlane.md](SmoPlane.md), [ModelMesh.md](ModelMesh.md).

Allocation: `operator new(0x11c)` then ctor `0x5CBB60` (empty) or copy-ctor `0x5CBBF0` (see
[functions/scene.md](../functions/scene.md)). Compiled with 1-byte packing for the surrounding structs (several globals
and engine objects have unaligned offsets such as `+0xCD`, `+0x1EF`, `+0x32F`), the Plane itself is naturally aligned.
Status: V = read consistently in >=2 functions, R = read in one, I = inferred.

| offset | size | type | name | meaning | evidence | st |
|---|---|---|---|---|---|---|
| 0x00 | 4 | PlanePart* | parts | array of `part_count` nodes, stride 0xA0 ([PlanePart.md](PlanePart.md)); heap block is preceded by a u32 count (array-new cookie) when `owns_parts` | 0x5CC7A0, 0x5CBBF0, 0x5D0570 | V |
| 0x04 | 1 | u8 | vertex_type | 1..8, selects vertex layout / stride / array slot | 0x5CDAE0, 0x5CD7F0, 0x5CDC70 | V |
| 0x05 | 1 | u8 | mem_mode | 0 = static GPU vertex+index buffers (`+0x90/+0x94`), 1 = vertex data streamed every draw through the dynamic ring buffer (system-memory model) | 0x5D0570 branches, 0x5CFA10 only builds buffers when 0 | V |
| 0x06 | 1 | u8 | part_count | number of PlanePart | 0x5CC7A0 | V |
| 0x07 | 1 | u8 | owns_parts | 1 = `parts` allocated by this Plane (freed by 0x5CCA00), 0 = pointer borrowed from another Plane | 0x5CC7A0, 0x5D4A00 | V |
| 0x08 | 1 | u8 | has_nontri | OR-merged on append; non-zero = index list may contain point / line records (marker `0xFFFF`, see tri-kind below) and the slow per-run draw loop is used | 0x5CEA30, 0x5D0570 | V |
| 0x09 | 1 | u8 | no_pass2 | 1 when no material has `pass == 2` (translucent pass is skipped entirely); recomputed by 0x5D6DA0 on every GPU upload | 0x5D6DA0, 0x5D0570 `(param_3 != 2 \|\| +9 != 1)` | V |
| 0x0A | 1 | u8 | gpu_dirty | set by `plane_mark_dirty` (flags & 7) | 0x5CF920 | R |
| 0x0B | 1 | u8 | is_instance | 1 = geometry/GPU buffers shared with `source` (copy-on-write: any mutator first calls 0x5D9BE0 which deep-copies) | 0x5CBBF0, 0x5D9BE0 | V |
| 0x0C | 4 | Plane* | source | the Plane this one is an instance of (non-null iff `is_instance`) | 0x5D2B10 | V |
| 0x10 | 4 | i32 | vertex_count | | 0x5CD7F0 | V |
| 0x14 | 4 | i32 | vertex_cap | grows to `count + 0x1F + n` | 0x5CD7F0 | V |
| 0x18 | 4 | u32 | pretransformed | 0 = vertices are in part-0 space and must be multiplied by `parts[0]` world matrix when drawn/merged; non-zero = already in world/model space (world matrix forced to identity). This is the dword written as "fvf" in `.pln` step 8: **it is a flag, not an FVF** | 0x5D0570 `(*this==0 \|\| +0x18!=0)`, 0x5CEA30, 0x5D7BF0 | V |
| 0x1C..0x38 | 8x4 | void*[8] | vtx[type-1] | vertex array pointer slot per type (only slot `vertex_type` is non-null). Strides: 0x24, 0x3C, 0x1C, 0x18, 0x48, 0x14, 0x48, 0x28 for types 1..8 | 0x5CDAE0 | V |
| 0x3C | 4 | u8[4] | tint_rgba | model tint multiplied with material `+0x1C..1F` /255 (bytes at +0x3C,+0x3D,+0x3E,+0x3F) when `+0x104 != 0` | 0x5D0570 | R |
| 0x40 | 4 | i32 | tri_count | | 0x5CE6F0 | V |
| 0x44 | 4 | i32 | tri_cap | grows to `count + 7 + n` | 0x5CE6F0 | V |
| 0x48 | 4 | u16* | indices | `tri_count * 3` u16, value `0xFFFF` in slot 2 / 1 marks point / line records (see 0x5D2AA0) | 0x5CE6F0, 0x5D2AA0 | V |
| 0x4C | 4 | u16* | tri_material | one u16 material index per triangle | 0x5CE6F0 | V |
| 0x50 | 1 | u8 | strip_active | currently using the strip index buffer (needs `+0x51` and global `DAT_00A678B0`) | 0x5D0570 top, 0x5D7190 | V |
| 0x51 | 1 | u8 | has_strip_data | strip lists present | 0x5D6EE0, 0x5CD470 | V |
| 0x52 | 2 | u16 | strip_tri_total | total triangles in strip data | 0x5D6EE0, 0x5D7190 | V |
| 0x54 | 4 | u16* | strip_tri_count | per material: triangles in that material's strip; a strip with n>0 tris occupies `n+2` indices | 0x5D6EE0, 0x5D0570 | V |
| 0x58 | 4 | i32 | strip_idx_cap | grows `count + 0x100` | 0x5D6EE0 | V |
| 0x5C | 4 | i32 | strip_idx_count | total strip indices (also the D3D index-buffer element count when strips are used) | 0x5D6EE0, 0x5CFA10 (`+0x17<<1` bytes) | V |
| 0x60 | 4 | u16* | strip_indices | | 0x5D6EE0 | V |
| 0x64 | 4 | i32 | mat_count | | 0x5CD470 | V |
| 0x68 | 4 | i32 | mat_cap | grows `count + 7 + n` | 0x5CD470 | V |
| 0x6C | 4 | PlaneMaterial* | materials | stride 0x54 ([PlaneMaterial.md](PlaneMaterial.md)) | 0x5CD470 | V |
| 0x70 | 4 | i32 | draw_category | selects reorder bucket; `2` = terrain sector mesh (set by 0x55F320 / 0x55B720), `0` = default / other world mesh (0x55FA40), `4` = actor-attached model | 0x5D35B0, 0x5D4760, 0x55F320 | V |
| 0x74 | 4 | void* | category_data | for category 4: owning actor (int at `actor+8` is the bucket offset, see engine/scene.md) | 0x5D35B0 `*(+0x74 + 8)+9` | R |
| 0x78 | 12 | float[3] | env_point | point fed through a matrix for the mode-2 projective effect | 0x5D0570 `local_8+0x1E` | I |
| 0x84 | 4 | D3DMATRIX* | proj_matrix | one 0x40-byte projective-texture matrix per material slot (default identity `DAT_00A18A78`) | 0x5CD470, 0x5D0570 | V |
| 0x88 | 4 | i32* | proj_stage | per material: texture stage 0..7 receiving `proj_matrix`, -1 = none | 0x5CD470, 0x5D0570 | V |
| 0x8C | 4 | u32 | dirty_flags | bit0 vertices, bit1 (aux), bit2 topology (frees `+0xB0/+0xB4`); `(flags & 6) != 0` triggers GPU re-upload | 0x5CF920, 0x5CFA10 | V |
| 0x90 | 4 | IDirect3DVertexBuffer8* | vb | COM vtable slots used: +4 AddRef, +8 Release, +0x2C Lock, +0x30 Unlock, +0x34 GetDesc | 0x5CFA10, 0x5CBBF0 | V |
| 0x94 | 4 | IDirect3DIndexBuffer8* | ib | same slots | 0x5CFA10 | V |
| 0x98 | 0x18 | node | gpu_registry_node | intrusive doubly-linked list node (owner/next/prev at `+0xA4/+0xA8/+0xAC`) linking every Plane that owns GPU buffers into a global list on the renderer object (head `+0x32F`, tail `+0x333`) so they can be released/rebuilt on device loss. Linked by 0x5FF290, unlinked by 0x5FF380 | 0x5FF290/380, 0x5CCAA0 | V |
| 0xB0 | 4 | u16* | mat_tri_end | per material: exclusive end triangle index of its run (triangles are sorted by material); freed when flags&4 | 0x5CF1D0, 0x5D35B0 | V |
| 0xB4 | 4 | u16 (min,max)* | mat_vtx_range | per material: min and max vertex index used (device `MinIndex`, `NumVertices = max-min+1`) | 0x5CF1D0, 0x5D35B0 | V |
| 0xB8 | 4 | i32 | has_mat_cache | non-zero once the per-material render-state cache handles below are initialised | 0x5D35B0, 0x5CD470 | R |
| 0xBC | 16 | i32[4] | mat_cache_inline | per-material cache handle (reorder material cache), inline when `mat_count < 5` | 0x5D35B0 | R |
| 0xCC | 4 | i32* | mat_cache_heap | heap array when `mat_count >= 5`. **The same slot index is reused by `lod[0]`**; LOD slots start at idx 1 so they do not collide | 0x5D35B0, 0x5D45E0 | R |
| 0xD0..0xDC | 4x4 | Plane*[4] | lod[1..4] | child LOD models (also addressed as `+0xCC + 4*idx`) | 0x5D45E0, 0x5D4550, 0x5CBBF0 | V |
| 0xE0 | 4 | float | lod_distance | base switch distance | 0x5D4760 | V |
| 0xE4 | 1 | i8 | lod_count | `>0` enables LOD selection | 0x5D4760, 0x5D35B0 | V |
| 0xE8 | 4 | Plane* | lod_parent | back link set by 0x5D4880 | 0x5D4880 | V |
| 0xEC | 4 | Plane* | shadow_mesh | silhouette/shadow proxy mesh (cloned in copy-ctor); proxy's `+0xF4` points back | 0x5D7310, 0x5CBBF0 | V |
| 0xF0 | 4 | Plane* | aux_mesh | deleted in 0x5CCAA0; set elsewhere (TODO(re)) | 0x5CCAA0 | R |
| 0xF4 | 4 | Plane* | shadow_owner | back pointer (set on the proxy) | 0x5D7310 | V |
| 0xF8 | 4 | u32 | bump_enabled | per-pixel lighting (vertex type 5) is on; settable only if type==5 and device cap `DAT_00A52627` | 0x5D5EE0, 0x5D5F30 | V |
| 0xFC | 4 | u32 | tangents_done | set after 0x5D5770 computed tangents once | 0x5D0570 | R |
| 0x100 | 4 | u32 | fx_flag_a | contributes bit 0x40 to the effect word | 0x5D5F60 | R |
| 0x104 | 4 | u32 | tint_enabled | contributes bit 0x80 and enables `tint_rgba` | 0x5D5F60, 0x5D0570 | R |
| 0x108 | 4 | u16* | weld_map | per-vertex index of the first earlier vertex with identical position (shadow volume welding); lazily built | 0x5E3040 | V |
| 0x110 | 4 | i32 | env_mode | 1 = per-vertex fake env/specular UV shift (0x5D7BF0), 2 = projective reflection variant | 0x5D35B0, 0x5D0570 | R |
| 0x114 | 4 | float3* | vtx_tangent_s | `vertex_count * 12` bytes | 0x5CBBF0, 0x5D7BF0 | V |
| 0x118 | 4 | float3* | vtx_tangent_t | `vertex_count * 12` bytes | same | V |

Unconfirmed: `+0x10C` (never touched), `+0x7C..+0x83`, `+0xA4..+0xAF` (part of the registry node), `+0xFC` semantics.

## Vertex layouts used by the code (offset within vertex)

| type | stride | layout deduced from code |
|---|---|---|
| 1 | 0x24 | pos f3 @0, normal f3 @0xC, diffuse u32 (BGRA bytes) @0x18, uv f2 @0x1C (see PlaneChunks) |
| 2 | 0x3C | type 1 + uv2 f2 @0x24 (written by 0x5D7BF0) + uv3, uv4 @0x2C, @0x34 (FVF TEX4) |
| 4 | 0x18 | pos f3 @0, diffuse u32 @0xC, uv f2 @0x10 (no normal; `0x5F2780` takes a constant normal) |
| 5 | 0x48 | pos, normal @0xC, diffuse @0x18, uv @0x1C, tangent S f3 @0x24, tangent T f3 @0x30, S x T f3 @0x3C (bump mapping; computed by 0x5D5770) |
| 7 | 0x48 | skinned (bone index stream; drawn with vertex-shader constants c17.., 24 bone matrices per batch, 3 regs each, matrices transposed from `parts[i].world`) |
| 8 | 0x28 | diffuse @0x1C (read by 0x5F2780) |
| 3, 6 | 0x1C, 0x14 | never touched in the exported code (TODO(re)) |

Per-type descriptor table `DAT_00A17D88` (16 bytes per entry, indexed by vertex type, accessor 0x600200). Layout:
`+0` u8 index, `+1` u8 texture-coordinate sets (nominal), `+2` u8 reserved, `+3` u8 `no_normal` (1 = no transformable normal),
`+4` u32 stride (also `SetStreamSource` stride and reorder element size), `+8` u32 normal offset, `+0xC` u16 D3D FVF,
`+0xE` u16 extra flags (meaning unknown). Dumped contents (entries 0..8):

| entry | stride | normal ofs | FVF | no_normal | extra | meaning |
|---|---|---|---|---|---|---|
| 0 | 0x1C | - | 0x144 XYZRHW\|DIFFUSE\|TEX1 | 1 | 0 | pre-transformed 2D vertex (not a Plane type) |
| 1 | 0x24 | 0x0C | 0x152 XYZ\|NORMAL\|DIFFUSE\|TEX1 | 0 | 0 | standard lit mesh |
| 2 | 0x3C | 0x0C | 0x452 XYZ\|NORMAL\|DIFFUSE\|TEX4 | 0 | 0 | 4 UV sets (60 B) |
| 3 | 0x1C | - | 0x142 XYZ\|DIFFUSE\|TEX1 (+4 pad) | 1 | 1 | unlit + 4 extra bytes |
| 4 | 0x18 | - | 0x142 XYZ\|DIFFUSE\|TEX1 | 1 | 0 | unlit: pos @0, diffuse @0xC, uv @0x10 (correct the earlier "no uv" note) |
| 5 | 0x48 | 0x0C | (FVF 0x452 in table, but 0 is passed to CreateVertexBuffer; vertex-shader path) | 0 | 0x54 | bump / tangent frame |
| 6 | 0x14 | - | 0x62 XYZ\|PSIZE\|DIFFUSE | 1 | 0 | point sprites (usage +0x40) |
| 7 | 0x48 | 0x18 | 0 (vertex shader) | 0 | 0 | skinned |
| 8 | 0x28 | 0x10 | 0x156 XYZB1\|NORMAL\|DIFFUSE\|TEX1 | 0 | 0 | position + one blend weight (dyngrass: weight `1 - f*(z/zmax)^2`); normal @0x10, diffuse @0x1C |

Type 2 therefore has up to four UV sets (the code only writes the second, `+0x24`); bytes 0x2C..0x3B are UV sets 3 and 4.
