# scene family — plane / plane_sh / scene_s / reorder / spindex / blkarrb / vgabuf / vgalock

Reference build joan.exe. Source files: `d:\ww\en3d\plane.cpp`, `plane_sh.cpp` (no path string, attributed by
naming), `scene_s.cpp`, `reorder.cpp`, `vgabuf.cpp`, `vgalock.cpp`, `d:\ww\uti3d\spindex.cpp`, `blkarrb.cpp`.
Pseudocode export: `$JOAN_WORK_DIR/decomp/engine/scene/` and `scene2/` .
Structs: [Plane](../structs/Plane.md), [PlanePart](../structs/PlanePart.md), [PlaneMaterial](../structs/PlaneMaterial.md),
[PlaneChunks](../structs/PlaneChunks.md), [ReorderQueue](../structs/ReorderQueue.md), [SceneSector](../structs/SceneSector.md),
[SpatialIndex](../structs/SpatialIndex.md), [BlockArray](../structs/BlockArray.md), [PaletteBuf](../structs/PaletteBuf.md).
Rows in `smo.md` already name several plane.cpp functions; this page refines them. Confidence:
V = verified by cross-reading >=2 callers, R = read, I = inferred. D3D8 = device vtable slot on `DAT_00A52428`.

**Two corrections to earlier assumptions:** (1) `vgabuf`/`vgalock` are 8-bit palette helpers, not vertex-buffer lock
wrappers; (2) `plane` is the engine's whole mesh class (`sizeof` 0x11C), `PlanePart` entries are a transform hierarchy.

## plane.cpp — mesh container (this = Plane unless stated)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x5CBBF0 | plane_copy_ctor(src, instance) | `memset(this,0,0x11C)`; copies type/mem_mode; if `instance`==0 deep-copies vertices (by stride), indices, tri materials, strip data, parts, tangents; if `instance`!=0 shares data (`is_instance=1`, `source=src`) and AddRef's the VB/IB. Recursively clones LODs 1..4 and the shadow mesh. Replaces smo.md `model_ctor`/`model_ctor_lod` for the copy case | 0x5CBBF0 | V |
| 0x5CBB60 | plane_ctor_empty | smo.md `model_ctor_lod` | smo.md | R |
| 0x5CC7A0 | plane_init(vtx_reserve, tri_reserve, mat_reserve, part_count) | calls free; `part_count`==0 borrows `source.parts` (owns_parts=0) else allocates `part_count` x 0xA0 with array-new cookie and constructs each; reserves arrays; zeroes tangent ptrs; default tint white | 0x5CC7A0 | V |
| 0x5CCAA0 | plane_free | releases LODs (recursive), shadow/aux meshes, per-material caches, all per-type vertex arrays, indices, strip data, VB/IB (COM Release `+8`), unlinks from GPU registry, frees parts, tangent arrays; resets counters | 0x5CCAA0 | V |
| 0x5CCA00 | plane_parts_delete(flags) | array-delete of PlanePart (`flags&2` = array with cookie, `&1` = free memory) | 0x5CCA00 | V |
| 0x5CD470 | plane_alloc_materials(n) | grow material array (+7 slack) and parallel arrays `+0x84` (0x40 B each, init identity), `+0x88` (init -1), `+0xCC` cache, `+0x54` strip counts; converts the inline 4-slot material cache to heap when passing 5; returns first new index | 0x5CD470 | V |
| 0x5CD7F0 | plane_alloc_vertices(n) | grow the active type's array (+0x1F slack, `realloc`), detach instance first; returns base index | 0x5CD7F0 | V |
| 0x5CDC70 | plane_set_vertex_type(t) | allocate new-type array, convert from old type with helpers 0x60F020..0x60F4B0, free old, then recurse into LODs. Type 5 falls back to 2 if device cap `DAT_00A52623`==0; type 7 resets vertex_count to 0 | 0x5CDC70 | V |
| 0x5CDAE0 | plane_get_vertices(out_stride) | returns active vertex array pointer (follows `source` for instances) and stride | 0x5CDAE0 | V |
| 0x5CE6F0 | plane_alloc_triangles(n) | grow index (6 B/tri) and tri-material (2 B) arrays (+7 slack); returns base | 0x5CE6F0 | V |
| 0x5CEA30 | plane_append(src, bake, mat_mode) | merge `src` into this: per-material dedupe by mode (0 = always add, 1 = reuse equal material among *old* materials, 2 = among all), vertices copied then (when `bake`==0 and the two `pretransformed` flags differ) transformed by `src.parts[0].world` and/or inverse of `this.parts[0].world` (points via `point_transform`, normals via `vector_transform` if the type has a transformable normal); indices offset by the vertex base only if `idx < src.vertex_count` (so `0xFFFF` markers survive); tri materials remapped; ORs `has_nontri`; marks dirty | 0x5CEA30 | V |
| 0x5CF050 | plane_copy_strip_data(src) | copy strip arrays (+0x50..+0x60) from `src` | 0x5CF050 | V |
| 0x5CF1D0 | plane_build_material_ranges(sort) | requires >=2 triangles and >=2 materials. `sort`==0: per material table `mat_tri_end[m]=tri+1` (assumes triangles already grouped); `sort`!=0: stable counting sort of triangles by material (rewrites index/material arrays, then builds `mat_tri_end`). Always builds `mat_vtx_range[m]=(min,max)` vertex index, ignoring `0xFFFF` | 0x5CF1D0 | V |
| 0x5CF920 | plane_mark_dirty(flags) | `dirty_flags \|= flags`; flags&4 frees range tables; flags&7 sets `gpu_dirty`. Common arg `0xFFFFFFFF` after any structural change; `2` after in-place vertex edits | many | V |
| 0x5D6EE0 | plane_strip_reserve(mat, ntris) | turn on strip data; add `ntris` to material's strip; grow list (+0x100), shift tail with memmove and return the index where the caller writes `ntris+2` strip indices | 0x5D6EE0 | V |
| 0x5D7190 | plane_strip_reset(free) | clear counts, optionally free arrays | 0x5D7190 | V |
| 0x5D9BE0 | plane_detach_instance | un-share: release VB/IB refs, deep-copy vertices/indices/strip/ranges/tangent arrays from `source`; clears `is_instance`. Invoked at the top of every mutator | 0x5D9BE0 | V |
| 0x5D4A00 / 0x5D4470 / 0x5D4550 / 0x5D4880 | plane_set_parts / plane_add_lod(idx, existing) / plane_drop_lod / plane_link_lod | parts sharing; LOD child creation (new Plane via 0x5CBB60) and parent/child pointers (`+0xCC+4*idx` / `+0xE8`) | 0x5D4470 etc. | R |
| 0x5D45E0 | plane_get_lod(&idx, fallback) | returns `lod[idx]`; with fallback walks idx downward to the nearest existing | 0x5D45E0 | V |
| 0x5D4760 | plane_select_lod() | only if `lod_count>0`: camera position vs. plane/actor position; `thr(4) = 8.0 * lod_distance`, halved per level (`/2.0`): returns the first level 4..1 with `thr(level) <= distance` (4 when dist >= 8d, 3 >= 4d, 2 >= 2d, 1 >= d), else 0 (base) | 0x5D4760 + constants | V |
| 0x5D7310 | plane_set_shadow_mesh(mesh) | cross-links `shadow_mesh` / `shadow_owner` | 0x5D7310 | V |
| 0x5D6DA0 | plane_scan_passes | `no_pass2 = 1` iff no material has `pass==2` | 0x5D6DA0 | V |
| 0x5CE970 | plane_has_pass(pass) | any material with that pass (or, with no materials, the default pass 1 when triangles exist) | 0x5CE970 | V |
| 0x600B30 | pass_equal | `a==b` | trivial | V |
| 0x5D0480 | plane_nonempty | `vertex_count>0 && tri_count>0` (follows `source`) | 0x5D0480 | V |
| 0x5D2AA0 | tri_kind(indices, i) | classify triangle record i: idx[2]==0xFFFF -> (idx[1]==0xFFFF ? 1 point : 2 line) else 4 triangle | 0x5D2AA0 | V |
| 0x5D2B10 | plane_is_instance | `source != 0` | trivial | V |
| 0x5D52A0 / 0x5D53E0 / 0x5D5360 / 0x5D5500 / 0x5D5650 | material_init / copy / release_textures / equal / set_texture(slot, tex) | see [PlaneMaterial](../structs/PlaneMaterial.md) | | V |
| 0x5D5F60 | plane_effect_flags | bit0 `bump_enabled`, 0x40 `fx_flag_a`, 0x80 `tint_enabled` (part of the reorder key and of the pixel-shader choice `0x602080`) | 0x5D5F60 | V |
| 0x5D5F30 / 0x5D5EE0 | plane_get_bump / set_bump | set only when type==5 and cap `DAT_00A52627` | | V |
| 0x5D5770 | plane_compute_tangents | for type-5 vertices: zero S/T, accumulate per triangle (u,v gradients vs x/y/z; skipped when `abs(cross.x) <= 1e-12` (double `DAT_009D9738`)), normalise S and T, `N' = S x T`; if `dot(N', normal) < 0.0` replace by a fallback vector (`0x60CBC0`) | 0x5D5770 | R |
| 0x5D7BF0 | plane_update_env_uv | `env_mode==1`, type 2: for each vertex compute normalised vector to a light/camera point (light type 3: its position; type 1: direction * `-500.0` offset, `DAT_009D5384`), dot with normal/tangents, write `uv2 = (S/texW + uv.x, uv.y - T/texH)` with texture size from material 0 `tex[0]` `(+0x3C,+0x40)`; mark dirty(2) | 0x5D7BF0 | R |
| 0x5CFA10 | plane_upload_buffers | **GPU upload**, see below | 0x5CFA10 | V |
| 0x5D0570 | plane_draw_immediate(ctx, pass) | immediate-mode draw, see below | 0x5D0570 | V |
| 0x5D2B80 | plane_draw_first_range(ctx, pass) | stripped copy of the above drawing only the first material range (no state setup) — used by special passes (shadow/depth) | 0x5D2B80 | R |
| 0x5D35B0 | plane_queue_draw(ctx, view, pass) | queued-mode draw: registers batches in the reorder queue instead of drawing, see below | 0x5D35B0 | V |
| 0x5FF290 / 0x5FF380 | gpu_registry_add / remove(node) | link/unlink Plane `+0x98` node into the renderer's resource list (`+0x32F` head, `+0x333` tail) | | V |
| 0x60EE60 / 0x60EF80 / 0x60EC70 | part_update_world / part_get_world / part_set_parent | see [PlanePart](../structs/PlanePart.md) | | V |
| 0x600200 / 0x600290 | vtxtype_desc(t) / index_format | `&DAT_00A17D88 + t*16` (table dumped, see Plane.md); index format const `0x65` = `D3DFMT_INDEX16` | | V |

### Buffer management (0x5CFA10)

* Only for `mem_mode==0` and non-instances. Rebuild condition: `(dirty_flags & 6)` **or** VB or IB missing. After the work
  `dirty_flags` is zeroed and `no_pass2` re-evaluated (0x5D6DA0).
* VB size = `vertex_count * stride`. An existing VB is kept if `GetDesc` (D3D8 buffer vtable +0x34) reports `Format==100`
  (`D3DFMT_VERTEXDATA`) and `Size >= needed`; otherwise released and recreated. Creation: `CreateVertexBuffer(len, usage, fvf, pool 0)`
  (device vtable +0x5C): `usage = DAT_00A5261B & ~0x200` (global base usage with **D3DUSAGE_DYNAMIC removed** -> static),
  `fvf = descriptor[type].+0xC`; type 5 passes FVF 0 (vertex-shader declaration), type 6 adds usage `0x40` (points).
  Pool 0 = `D3DPOOL_DEFAULT`, so buffers vanish on device loss, hence the registry nodes (0x5FF290).
  On failure the error text `"Cannot create vertex buffer (%s)"` is logged via the logger `DAT_00A47D78`
  with the D3D error string from 0x6002B0 (D3DERR_* / D3DXERR_* name table at 0x7693C4).
* Fill: `Lock(0, len, &ptr, 0x800)` (`D3DLOCK_NOSYSLOCK`), `memcpy` whole array, `Unlock`. Errors: `"Cannot lock vertex buffer (%s)"`.
* IB: size `tri_count*6` (list) or `strip_idx_count*2` when `strip_active && DAT_00A678B0`; format `D3DFMT_INDEX16`;
  recreate when too small or format mismatch; same usage mask; lock flags `0x800`; log texts `"Cannot create/lock index buffer (%s)"`.
* Newly created buffers register the Plane in the GPU registry (non-instance only).
* Dynamic ring buffers (`mem_mode==1` and queue packets with no GPU buffer): see "Reorder-queue additions".
* Draw statistics: `0x5D2B40(slot, n)` adds to counters at `wrapper+0x21B+4*slot` (triangles drawn).

### plane_draw_immediate (0x5D0570) order of operations

1. Skip if `pass==2 && no_pass2`. Strip mode toggling: global `DAT_00A678B0` (strip option) vs `strip_active`; flipping drops the IB so it is rebuilt.
2. LOD: if `lod_count>0` pick level by 0x5D4760/0x5D45E0 and re-enter on that child.
3. `plane_upload_buffers`; once, if bump enabled and tangents not done and pass==1, compute tangents; `env_mode==1` -> `plane_update_env_uv`.
4. Skip if no material uses this pass. Instance: borrow the source's arrays/counts for the duration, restored at the end.
5. `SetTransform(WORLD=0x100, parts[0].world)` (identity if `pretransformed`); for type 8 with >1 texture unit also `SetTransform(0x101)`.
6. Mode 0: `SetStreamSource(0, vb, stride)` (+0x14C), `SetIndices(ib, 0)` (+0x154) with error texts `"Cannot set vertex buffer (%s)"`/`"Cannot set index buffer (%s)"`, then `0x6024E0(type)` (records the pending vertex type in the render-state cache). Mode 1: ring buffer upload.
7. If `DAT_00A5272B > 3` stencil bits and shadow cap -> `SetRenderState(0x39 = STENCILREF, 0 or 1<<(bits-1))`.
8. Loop over **runs of equal material id** in `tri_material` (run end from `mat_tri_end[m]` if present). For materials of this pass: `SetMaterial` (+0xA8) from diffuse/ambient/specular/emissive/power; `SetTexture(stage 0..3)` (+0xF4); render-state cache setters `0x5F8250(+0x38)`, `0x5FA250(+0x34)`, `0x5FEC10(+0x3C)`; tint combine `plane.tint * material.tint / 255` -> `0x5FEB50`; effect word `0x602080(effect_flags | 0x616FE0())`; `0x6026A0(+0x20 flags)`; animated-texture update; `0x6014D0(anim id)` commit pending state; pixel-shader / texture-transform setup (`0x617D50`, `0x616B80`, `0x5C09A0`).
9. Type 7 (skinned): upload bone matrices: groups of up to 0x18 bones, each `matrix_transpose(parts[i].world)`, `SetVertexShaderConstant` (+0x13C) at register `first*3+0x11`, count `n*3` (3 registers per bone: the 4th row is implicit).
10. Projective textures when `DAT_00A58055==2` and `proj_strength>0`: `SetTransform(0x10+stage, proj_matrix)`, `TSS TEXTURETRANSFORMFLAGS(0x18) = 0x104`, `TSS TEXCOORDINDEX(0xB) = stage | 0x20000` (camera-space position), undone after the draw.
11. Draw: mode 0 -> `DrawIndexedPrimitive` (+0x11C) triangle list `D3DPT=4` with `(minIndex, numVertices=max-min+1, startIndex=run_first_tri*3, primCount)`, or strip `D3DPT=5` using the per-material strip offset (`sum(count+2)` over earlier materials, `startIndex`, `strip_tri_count[mat]`). Mode 1: ring buffer + `0x600A90(prim, idx, n)`; when `has_nontri` it splits each run into points (`0x600A00`), lines and triangle runs by `tri_kind`.

### plane_queue_draw (0x5D35B0) — the queued path

Same skip/LOD/upload/tangent/env prelude as above. Then: choose bucket (`draw_category`, actors: `*(actor+8)+9`) via
`reorder_select_bucket`; fill `current key` (type, material cache handle via `0x5E79F0`, effect flags, primitive kind 4 list /
5 strip); `reorder_acquire_item(0)`; add one packet per material run through `0x5E7F70` (GPU buffers, `mem_mode 0`) or `0x5E7EC0`
(system memory) or `0x5E7E70` (points). Per-material cache handles live in `Plane +0xBC/+0xCC` and are reset once. For
`mem_mode 1` with non-triangle records it splits runs exactly like the immediate path. Nothing is submitted to the device.

## plane_sh.cpp — stencil shadow volume builder

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x5E3040 | plane_sh_build_volume(light, out_verts, out_nverts, out_idx, out_ntris, out_flag, out_world, extrude) | free function (this=Plane in ECX). Builds a **shadow volume** for vertex types 1, 2, 5 (>2 vertices). Steps: (1) light vector in model space (see below); (2) welding map `weld_map` once (O(n^2) position compare, `0x60CED0`); (3) for each triangle (with welded ids; skip if two ids equal) compute face normal and compare `n . L` to threshold `1e-7` — facing triangles are emitted as cap triangles and each of their 3 edges is toggled in a sorted edge set (`0x5E2C80`), so only edges used by exactly one facing triangle remain (silhouette); (4) each remaining edge (a,b) emits two triangles `(a, b, a+N), (b, b+N, a+N)` where the second ring is the same vertices extruded along the light direction by `300.0 + extrude` (`DAT_009D7280`); (5) remaps indices to a compact output vertex array (stride 0x24, position + diffuse `0xFF000000`). Scratch buffers are process-wide (`DAT_00A526FF/A52703` vertices, `DAT_00A52707/A5270B` indices, grown on demand, never shrunk) | 0x5E3040 | R |
| 0x5E2C80 | edge_set_toggle(edges, &n, a, b) | binary-search-based insert/erase on an array of u16 pairs sorted by (hi, lo): if the reversed pair `(b,a)` is already present it is removed (shared edge), else `(a,b)` is inserted | 0x5E2C80 | V |

## reorder.cpp — draw-batch queue (see [ReorderQueue](../structs/ReorderQueue.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x5E7BB0 | reorder_select_bucket(i) | clamp i>=0, grow bucket array (zero-fill new buckets), set `cur_bucket`, clear `cur_item` | V |
| 0x5E7CE0 | reorder_acquire_item(force_new) | binary-search the bucket for the current key (skipped when `force_new`); not found: grow items (+0x80), insert at the sorted position, copy 5 key dwords (0x5E8210); set `cur_item` and stamp `+0x14 = frame counter DAT_00A500AE` | V |
| 0x5E7B80 | reorder_current_key | returns `this+0x9E` | V |
| 0x5E7E70 / 0x5E7EC0 / 0x5E7F70 | reorder_add_points / add_indexed_sysmem / add_indexed_gpu | thin wrappers over 0x5E8400 / 0x5E8540 / 0x5E88C0 that append a packet to the current item (reusing recycled packet nodes, see struct page) | R |
| 0x5E8C40 / 0x5E8D60 | reorder_alloc_vpkt / free_vpkt | 0x20-byte packet pool (cap 0x28) | V |
| 0x5E8DE0 / 0x5E8F10 | reorder_alloc_ipkt / free_ipkt | 0x30-byte packet pool (cap 0x28) | V |
| 0x5E8B20 | reorder_pool_release | refcount--, free both pools at 0 | V |
| 0x5E8250 | reorder_item_free | return all packets of an item to the pools | V |
| 0x5E61C0 | reorder_shutdown | free all items/buckets, release caches (material 0x5D5360, light 0x5C5B50) | R |
| 0x5E79F0 | matcache_acquire(old_handle, material) | material-state cache lookup/insert (see struct page) | V |
| 0x5E76F0 / 0x5E7870 | lightcache_acquire / lightsetcache_acquire | same pattern for 0xD6-byte light records and 0x8D-byte light sets | R |
| 0x5911A0 / 0x5910F0 | sorted_array_find(base,count,elem,cmp,&insert_pos) / sorted_array_insert | generic 1-based binary search + shift insert (comparator callback) | V |
| **0x5E67B0** | reorder_flush(view, ?, pass) | **the queue flush**, see Reorder-queue additions | V |

## scene_s.cpp — scene sector (see [SceneSector](../structs/SceneSector.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x5EF900 | scene_sector_free | frees projectors, lights array, meshes (`0x459C40` = Plane delete) | V |
| 0x5F0040 | scene_sector_add_light | `new` 0xCE-byte Light (ctor 0x5C5830), append (cap +8) | V |
| 0x5F35F0 | scene_sector_add_item | append pointer (cap +8) | V |
| 0x5F39A0 | scene_sector_add_projector | append pointer (cap +5) | V |
| 0x5F03B0 | scene_sector_queue(ctx, view, pass) | queued rendering of a sector: refresh light caches (`0x5E76F0` per light), compute the sector's light-set handle (`0x5E7870`), store it in the current queue key; draw mesh `+0x38` via `plane_queue_draw`, overlay mesh `+0x40` and aux object `+0x3C` when option `DAT_00A03834`; then, if actors enabled (`DAT_00A69AB0`) and visible (`0x615D70`), iterate the scene's actor list (head `0x5EFFA0`, next `+0x20`): pass 1 draws each actor that passes its visibility test (`0x5AB470`) with `0x5ABC50`; pass 2 collects visible actors into a scratch array and **qsort**s them with comparator `0x5F0780` (descending by `actor+0xA8`) before drawing | R |
| 0x5F07F0 | scene_sector_draw_immediate(view, pass, layer) | immediate-mode counterpart (used when `DAT_00A17428`, the "use reorder queue" switch, is off or for specific passes): first-pass light activation, mesh draws via `plane_draw_immediate` (`0x5D0570`) gated by options `DAT_00A17878`/`DAT_00A03834`, then the actor lists for the given render layer (`0x5AED50`), pass 1 via `0x5B1180/0x5ABA20/0x5B1CB0` (actor draw with deferred "post" list at actor `+0x2D0`), pass 2 sorted with the same qsort comparator | R |
| 0x5F0780 | actor_depth_compare(a**, b**) | qsort comparator: `d = (*a)->[+0xA8] - (*b)->[+0xA8]`; returns 1 if `d<0`, -1 if `d>0`, else 0 -> **descending** order of the actor's second ground-plane coordinate (`+0xA4,+0xA8,+0xAC` = position x,y,z; `+0xA8` is the camera-facing depth axis used for painter's-order of translucent actors) | V |
| 0x5F2780 | scene_sector_bake_vertex_light(mesh, color) | per-vertex lighting bake: collects the lights that pass `0x5C6790`/`0x5F4440`; if none, writes the constant `color` RGB into every vertex diffuse (offset 0x18 for types 1/2/5, 0x0C for 4, 0x1C for 8); otherwise for each vertex (position, normal transformed by the part matrix unless `pretransformed`) sums light contributions: type 0 = intensity, type 1 = `max(n . dir)*intensity*...`, type 3 = point light with `(1 - d/radius)*max(n.dir)` (const 1.0) and a line-of-sight test (`0x60D810`); RGB written as `__ftol` truncation; marks mesh dirty(2) | R |
| 0x5F3A70 | scene_sector_draw_projectors(view, pass) | draws the sector's projector list in groups of up to `min(DAT_00A52508, DAT_00A524FC&0xFFFF, 4)` per pass using `SetTransform(0x10+i)` texture matrices (inverse view times projector matrix via `0x5E5660`) and one `0x5D0570` draw per group (projective light/decal textures) | R |
| 0x5F3FE0 | scene_sector_fade_overlay | distance fade of the overlay mesh: `d = |camera.xz - sector centre|`; `d <= 1450.0` (`DAT_009D982C`) full opacity (returns 1); `1450 < d <= 3072.0` (`DAT_009D9828`) faded: alpha = `ftol(...)` written as material tint alpha and material flag bits 8..11 cleared; `d > 3072` skipped (returns 0) | R |

## spindex.cpp / blkarrb.cpp / vgabuf.cpp / vgalock.cpp

See struct pages; functions listed in them: [SpatialIndex](../structs/SpatialIndex.md) (0x681EE0 dtor, 0x681FE0 init(w,h,cell),
0x6820D0 clear, 0x683910 region ctor, 0x683BB0 region reshape, 0x683B00 region free, 0x683650 region_is_free),
[BlockArray](../structs/BlockArray.md) (0x637700 alloc, 0x6379F0 free, 0x6375B0 clear, 0x637B00/0x637BE0/0x637CB0 list ops),
[PaletteBuf](../structs/PaletteBuf.md) (0x6123F0..0x612DD0, 0x613210/0x613270).

## Reorder-queue additions

### Queue flush 0x5E67B0 (`this` = renderer, args: two context words, `pass`)

1. Reset every bucket's cursor (`bucket+8 = 0`) and set `WORLD = identity`. `first = true`.
2. Loop: across **all buckets**, look at each bucket's next unconsumed item and take the smallest `item.key[0]` (light-set
   handle); the range of buckets holding that minimum is `[b_first, b_last]`. Stop when the minimum exceeds the light-set cache
   count. Then for buckets `b_first..b_last` in ascending bucket order, consume every item whose `key[0] <= min` (items are
   sorted ascending, so `== min`) in key order.
   **Order of draws is therefore: light-set handle (ascending), then bucket index, then rest of key** — bucket index is *not* the
   primary key.
3. An item is drawn only if `item.last_frame == DAT_00A500AE` (registered this frame) and the material-cache record's pass
   (`record+0x48` = material `+0x40`) equals `pass`.
4. State deltas against the previously drawn item (`prev`; all unconditional for the first item):
   * light set changed: disable the old set's lights (`0x5C6250` per light handle in `set+0xC[ i ]`, count `set+8`), enable the
     new set's lights (`0x5C6110`); if the set's flag byte `set+0x8C` differs from the cached one (`0x5E8FC0`/`0x5E8FE0`) run
     `0x5EEBC0` (flag 0) or `0x5EEC30` (flag != 0) — the fixed-function lighting/ambient mode switch.
   * effect flags changed: `0x602080(flags | 0x616FE0())`.
   * material changed: `SetMaterial`, 4x `SetTexture`, state-cache setters (+0x38,+0x34,+0x3C,+0x20 of the cached material copy at
     `record+8`), `0x6014D0(0)` commit, then pixel-shader/combiner selection (`0x6020F0`, `0x616B80` or `0x5C09A0`). `0x47B5E0()+0x5C = 1.0f`.
5. Vertex packets (item `+0x18`, next at `packet+0x1C`, skipped if count `<= 0`): set world matrix (identity if the packet's
   part pointer is null, else part-0 world) or, for type 7, upload 24-bone batches as in the immediate path; no GPU buffer ->
   copy `count` vertices into the ring buffer (`0x600490`) and `DrawPrimitive` via `0x600A00(primKind, 0, primCount)`; with a GPU
   buffer -> `SetStreamSource(0, vb, stride)` and `DrawPrimitive(prim map, startVertex, primCount)`. First draw of an item commits
   pending state `0x6014D0(0)`.
6. Indexed packets (item `+0x20`, next at `+0x2C`): same transform logic; if `vb == 0 || ib == 0` -> ring-buffer path:
   `0x600490(type, vertexPtr + minVertex*stride, vertexCount)` then `0x600510(indexPtr + firstIndex*2, indexCount, -minVertex)`
   then `0x600A90(primKind, 0, primCount)`; else `SetStreamSource` + `SetIndices(ib, 0)` and
   `DrawIndexedPrimitive(prim, minVertex, vertexCount, firstIndex, primCount)`. Triangle counter `0x5D2B40(0, primCount)`.
7. After the last item the last light set is disabled again.

Primitive kind -> D3DPRIMITIVETYPE map `0x600B60`: identity for 1..6 (POINTLIST, LINELIST, LINESTRIP, TRIANGLELIST, TRIANGLESTRIP,
TRIANGLEFAN), anything else -> TRIANGLELIST (4).

Packet field layout (corrects the earlier struct text): indexed packet dwords `[0]` vertex count, `[1]` min vertex, `[2]` system
vertex pointer, `[3]` vertex-buffer object, `[4]` index count, `[5]` first index, `[6]` system index pointer, `[7]` index-buffer
object, `[8]` primitive count, `[9]` PlanePart array (world matrix source, null = identity), `[10]` part count (skin bone count).
Vertex packet: `[0]` count, `[1]` first vertex, `[2]` system pointer, `[3]` vb object, `[4]` primitive count, `[5]` part array,
`[6]` part count, next at `[7]`.

### Dynamic ring buffers (renderer object, same offsets as the 0x5D2B40 stats block)

| renderer off | meaning |
|---|---|
| +0x08 | `IDirect3DDevice8*` |
| +0x18F + 4*type | per-vertex-type ring VB (types 0..7) |
| +0x1B3 | shared ring IB (u16) |
| +0x1B7 + 4*type | per-type VB write cursor (in vertices) |
| +0x1DB | IB write cursor (in indices) |
| +0x1DF, +0x1E3, +0x1E7, +0x1EB, +0x1EF | state of the last vertex lock: VB object, type, start vertex, stride, vertex count |
| +0x1F3, +0x1F7 | start index and index count of the last index lock |
| +0x14A / +0x163 | render-state cache: pending vertex type / dirty flag |

* `0x6005B0(type, n)` lock vertices: bytes `n*stride`; capacity **4096 vertices** (`0x1000`) per type. If the cursor is 0 or
  `cursor + n > 0x1000`: cursor = 0 and `Lock(0, bytes, flags 0x2800 = DISCARD|NOSYSLOCK)`; else
  `Lock(cursor*stride, bytes, 0x1800 = NOOVERWRITE|NOSYSLOCK)`. Records the lock state, advances the cursor, returns the write
  pointer. Error text `"Cannot lock vertex buffer (%s)"`.
* `0x600740()` unlock vertices, `SetStreamSource(0, vb, stride)`, `SetIndices(ringIB, baseVertex = start vertex)`, record vertex type via `0x6024E0`; returns the start vertex.
* `0x600820(_, n)` lock indices: capacity **16384 indices** (`0x4000`); identical DISCARD / NOOVERWRITE policy with byte offsets `cursor*2`; stores index start (`+0x1F3`) and count (`+0x1F7`).
* `0x600980()` unlock indices, `SetIndices(ringIB, start vertex)`.
* `0x600490(type, src, n)` = lock vertices + `memcpy` + commit. `0x600510(src, n, base)` = lock indices, copy `src[i] + base` (plain `memcpy` when `base == 0`), unlock.
* `0x600A90(prim, _, primCount)` -> `DrawIndexedPrimitive(prim, 0, wrapper+0x1EF, wrapper+0x1F3, primCount)`; `0x600A00(prim, firstVertex, primCount)` -> `DrawPrimitive(prim, wrapper+0x1E7 + firstVertex, primCount)`. Both add `primCount` to the triangle statistic when prim is 4 or 5.
* Index values written are relative to the vertex start: `base = -minVertex` in the queue path, so the same ring chunk serves vertices copied from `minVertex`.

### plane_sh light descriptor (param_2 of 0x5E3040)

`+0x00` type (1 = directional, 2 = point, 3 = point/spot-like; 2 also computes a distance factor via `0x425670` = `sqrt`), `+0x0C/+0x10`(/`+0x14`) light position x,y(,z), `+0x18/+0x1C` yaw and pitch for the directional vector (built by `0x60BD00(yaw, pitch, 0, 0)`; its matrix axis extracted by `0x60C220`/`0x60C310`). Point light direction = `object.world._41,_42(,_43) - light.pos` normalised (Ghidra lost the third component; assumed `_43 - pos.z`). Models with `pretransformed != 0` (or no parts) use the caster's own position (`ctx+0xA4/+0xA8/+0xAC`) and an identity world; otherwise the direction and position are transformed by the inverse part-0 world matrix. `out_world` receives the matrix to draw the volume with. Extrusion length = `300.0 + extrude` (`param_9`). The shadow-facing test threshold is the double `1e-7` (bytes `48 AF BC 9A` are the low half of `0x3E7AD7F29ABCAF48`).

### Animated textures (inside 0x5D0570)

Per material stage `s` (0..3) with an animated-set reference (`mode_a & 0xFF000000 == 0x01000000`): resource `r = 0x4B27B0(mode_a & 0xFFFF)`,
variant `v = (mode_a >> 16) & 0xFF`; frame count `n = i16 at r+0x31+2v`... (as indexed: `*(short*)(r+0x31+v*2)` = number of animated stages
for the variant; per stage count at `r+0x361+v*0x10+s*4`, period ms (float) at `r+0x301+v*0x10+s*4`, base name at
`r+0x3C1+v*0x84+s*0x21`, "is projective" flag at `r+0x6D9+s+v*4`).
Algorithm when `n > 1` and `now - tex_anim_time[s] >= period`: take the current texture's name (stage texture `+0x18`, or the base name
+ the string `"1"` (`DAT_009D8DE4`, i.e. frame 1) when no texture is bound yet); find the base name inside it (`strstr`), parse the number that
follows (`atoi`), compute the next frame **1-based, cyclic**: `cur+1 == n ? n : (cur+1) % n` (so the sequence is 1,2,..,n,1,..), and build
`base + number` (number formatted by `0x58FCA0(num, 1)`). If that texture is not already resident (`0x60A4B0`), loading is deferred
until `now - tex_anim_time >= min(500.0, period * 10.0)` (`DAT_009CFC20` = 10.0f) so slow
loads do not stall every frame. Load with `0x6099C0(name, 1 (3 when env_mode==2), 0)`, bind via `0x5D5650(stage, tex)` with the global `DAT_00A403D8`
temporarily `-5000` (so the new texture is treated as low priority), touch via `0x60A550`, store `tex_anim_time[s] = now`.

### Who sets `Plane.draw_category` (+0x70)

* Terrain sector meshes: `2` — set by the `.pln` reader 0x55F320 and by the terrain builder 0x55B720 (right after `plane_init`).
* The other world mesh built at 0x55FA40: `0` (default; bucket 0).
* Actor-attached models: `4`, `category_data` = the actor (bucket `9 + actor[+8]`). Setter lives in the actor code (not in this export).
* Values 1, 3, 5, 6, 7 were not seen assigned in the exported code.

### Remaining globals (confirmed)

| address | meaning |
|---|---|
| 0xA52428 | `IDirect3DDevice8*` |
| 0xA5261B | base buffer usage mask (u32, unaligned) |
| 0xA52623 / 0xA52627 | device caps: bump-mapping (type 5) available / enabled |
| 0xA5272B | stencil bit count |
| 0xA52518 | number of texture units (>1 enables second-UV world matrix for type 8) |
| 0xA52508, 0xA524FC | texture-stage / light limits (min used as batch size of projectors, max 4) |
| 0xA678B0 | strip-index option |
| 0xA17428 | "use reorder queue" switch; 0xA17878, 0xA03834 mesh/detail options; 0xA69AB0 actor draw enable |
| 0xA500AE / 0xA500BA | frame counter / millisecond clock (cache aging, texture animation) |
| 0xA18A78 | identity D3DMATRIX |
| 0xA58055 | render quality mode (2 enables projective texture setup) |

### Float constants resolved (from the a dump of the data section)

| address | value | used in |
|---|---|---|
| 0x9D8690 | 8.0f | LOD base multiplier (0x5D4760) |
| 0x9CF794 | 2.0f | LOD halving divisor (0x5D4760), sector centre average (0x5F3FE0) |
| 0x9CF784 | 1.0f | point-light falloff (0x5F2780) |
| 0x9CF5C4 | 0.0f | zero compare (comparators, tangent flip test, projective strength) |
| 0x9D7280 | 300.0f | shadow-volume extrusion base |
| 0x9D9828 / 0x9D982C | 3072.0f / 1450.0f | overlay mesh fade far / near distance |
| 0x9D5384 | 500.0f | env-UV offset (0x5D7BF0), anim-load deferral cap |
| 0x9D9738 | 1e-12 (double) | tangent degenerate-triangle test |
| 0x9D9748 | 1e-7 (double) | shadow facing test |
| 0x9CFC20 | 10.0f | animated-texture load-deferral factor |
| 0x9D8DE4 | string "1" | initial animation frame suffix (followed in memory by an unrelated alignment-keyword table) |
