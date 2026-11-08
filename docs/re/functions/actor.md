# actor family - actor / actordef / actordeg / animdef (runtime) / bonetree / morph / skin (joan.exe)

Pseudocode: `$JOAN_WORK_DIR/decomp/engine/actor/` (plus `engine/scene/005b5e40`, `005b1180`, `005b1cb0`, `005d4760`, `005f4c10`, `engine/ui2d/005ad060` read for context).
Structs: [Actor](../structs/Actor.md), [ActorDef](../structs/ActorDef.md), [ModelSet](../structs/ModelSet.md), [ModelMesh](../structs/ModelMesh.md), [FrameNode](../structs/FrameNode.md), [BoneTree](../structs/BoneTree.md),
[AnimDef](../structs/AnimDef.md), [AnimKey](../structs/AnimKey.md), [SkinVertex](../structs/SkinVertex.md), [Morph](../structs/Morph.md).
Formats (not repeated here): [smo](../../formats/smo.md), [am1](../../formats/am1.md), [am2](../../formats/am2.md), [am3](../../formats/am3.md), [amt](../../formats/amt.md), [mda](../../formats/mda.md).
 Older tables for the file-reading side: [anim.md](anim.md), [smo.md](smo.md) - this page adds the runtime side and corrects/extends them.
Source files: `d:\ww\en3d\actor.cpp`, `actordef.cpp`, `actordeg.cpp`, `animdef.cpp`, `bonetree.cpp`, `morph.cpp`, `skin.cpp`. All structures are packed (odd offsets are real).
Confidence: **V** verified (>=2 consistent reads), **R** read, **I** inferred. Names are proposals (`?` = uncertain).

## Actor (actor.cpp)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x5AAF10 | `actor_destroy` (`this`=Actor) | tear-down: unlink from owner list (0x5EFDB0); free `blend_from`(+0x78)/`cursors`(+0x88); clear its `dynarrb` slot (+0x3C) if that entry still points back; delete `model`(+0x90) and `attach`(+0x8C); destroy `aux`(+0x5C); for `kind` 1/2/3 delete `def`(+0) (kind 1 also the virtual `extra`(+0x14)); delete every Morph (+0xB4[]) then the array | `actor.cpp` assert string `0x5AAF10` | V |
| 0x5ADF50 | `actor_add_morph` (`this`=Actor; args `(model_name, dir, name, D3DMATRIX* xform)`) | grow `morphs` by 2 when full, allocate a 0x84 Morph (memset), load `<dir><name>.MDA` via `Morph::load`, append | `actor.cpp` + `morph.cpp` | V |
| 0x5AE0C0 | `actor_add_morph_copy` (`(Morph* src, D3DMATRIX*)`) | same but deep-copies an existing Morph (0x5C7370), optionally transforming the vertices | | V |
| 0x5AD060 | `actor_select_pose` (ui2d export; `this`=Actor; out `Model**`, out aux) | picks LOD (0x5D4760), re-evaluates the pose only if the clock moved (`last_eval`), CPU-skins (0x5F4C10), runs the Morphs (0x5C8960 per morph) and returns the Model to draw | read in full | R |
| 0x5B1180 | `actordef_queue_instance` (scene export; `this`=ActorDef; `(Actor*, lod_param, pass, list*)`) | the **instance batcher**; see "Batching" below | read in full | R |
| 0x5B1CB0 | `actordef_flush_batches` (`(cam params, pass, lod or -1)`) | draws every queued instance per LOD and resets the counters | read in full | R |
| 0x5D4760 | `model_select_lod` | camera-distance LOD pick, returns 0..4 | read | R |
| 0x5F4C10 | `model_deform_cpu` (`this`=Model with skin) | CPU skinning / bone update | read | V |

## ActorDef (actordef.cpp / actordeg.cpp)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x5B03D0 | `actordef_release` (`this`=ActorDef) | spin `SleepEx(5)` + `0x5AF890` until the async load is idle, then (if `ready`) delete `batch_models[5]`, zero `lod_*[5]`, free `static_mats`(+0x34), `bone_map`(+0x14), release the animation resource (`0x4B2730(anim_res)` + `0x4156D0`, a ref-count dec) and the ModelSet (0x5B05D0) | `actordef.cpp` | V |
| 0x5B0620 | `actordef_bind_skeleton_from_path` (`this`=ActorDef; `(dir, name, AnimDef* fallback)`) | bind skeleton: see "Binding" | | V |
| 0x5B0850 | `actordef_bind_skeleton_from_mem` (`(FileMem* am3, AnimDef* fallback)`) | same, `.AM3` already in memory (async load result); `0` file => use the AnimDef's tree | | V |
| 0x5B0AB0 | `actordef_build_lods` (actordeg.cpp, `this`=ActorDef) | one-time creation of the per-LOD batch Models (+0x178) - see "Batching" | `actordeg.cpp` | V |
| 0x5AF890 | `actordef_update_load` | state machine +0x03: 0x0B (`0x5C3110` pump ModelSet) -> 0x0C (wait `.AM3` read, bind with 0x5B0850) -> 0x0D (close file, `ready=1`) ; 0x15/0x16 = same for the animation-table path (up to a 5 ms budget per call, `timeGetTime` via 0x591050) | | R |
| 0x5AFDA0 / 0x5AFD50 | `actordef_is_idle` / `actordef_close_am3` | | | I |
| 0x5B05D0 | `modelset_delete` | deleting dtor of ModelSet | | R |
| 0x47B5B0 / 0x4B0C20 | `actordef_dtor` / `actordef_deleting_dtor` | -> 0x5B03D0 | | R |

### Binding (0x5B0620 / 0x5B0850) - what happens when a model gets its skeleton
1. Needs either an animation record (`ActorDef.anim_res != 0`; resolves to an `AnimDef`) or a caller-supplied `AnimDef`; else returns 0.
2. If a model-specific `<name>.AM3` exists (`0x590B80` file-exists / non-NULL memory file): parse it into a **new** BoneTree stored at `ModelSet+0x44` (`0x5C5400`/`0x5C5560` -> `0x5B9AF0`). Otherwise the AnimDef must already be loaded (`AnimDef+0x148 != 0`, i.e. parent-relative) and its tree is **copied** (`0x5C5610`, only if `tree.count == Model.bone_count`).
3. If the AnimDef is parent-relative: build the frame hierarchy `0x5C56D0` (`frame(i).set_parent(frame(parent))`, `ModelSet+0x48 = 1`).
4. `0x5BA1D0` decides whether the model's bones and the animation's tracks line up; if not, allocate `bone_map = int[AnimDef.n_tracks]` and fill it with `0x5BA300` (by bone **name id**, `-999` = track has no bone).
Returns 1 on success. Order matters: the bone map is built only after the hierarchy exists.

### Batching (0x5B0AB0, 0x5B1180, 0x5B1CB0, ActorDef offsets +0x24..+0x33, +0x34, +0x38, +0x178, +0x18C)
* Goal: draw up to **16 instances of the same ActorDef per LOD with one draw call** (5 LODs: 0 = main model, 1..4 = `ModelSet` LOD models, `Model+0xCC[i]`).
* `build_lods` (once, guarded by `+0x20`) - for each LOD `i` present, three cases by `ModelSet.skinned` (+2) and vertex type (`Model+4`):
  1. **CPU-skinned** (`skinned==1`, type != 7): capacity `c = min(0x1000 / vertex_count, 0x4000 / (tri_count*3), 16)` (16 if the counts are 0); only if `c > 1`, the LOD has exactly 1 plane with a single sub-mesh (`planes[0]+0x40 == 1`) and is not type 7: allocate an empty Model (type copied from main, `0x5CBAF0(type,1)`), give it `bone_count` bone frames (`0x5CC7A0(0,0,0,bones)`), `lod_capacity[i] = c`, `lod_merged[i] = 0`.
  2. **HW-skinned** (`skinned==1`, type 7): new Model(type 7, flag 0) with bone frames, geometry of the LOD appended (`plane::fn_5CEA30(lod,1,0)`), flags copied (`5CF050`), all dirty (`5CF920(-1)`), triangle sort (`5CF1D0(0)`); capacity = merged = 0x10.
  3. **static** (`skinned==0`, type != 7): same as 2, additionally `static_mats = alloc(0x1400)`; capacity = merged = 0x10.
* `queue_instance` accepts an actor only if: not `hidden`, no active Morph, model-dirty byte `Model+0x0A == 0`, `Actor+0x30 == 0.0`, and a render pass is selected; LOD chosen by `model_select_lod` (falls back to a lower existing LOD via `0x5D45E0(&lod,1)`); returns 0 (caller draws it itself) if the LOD has no batch Model / zero capacity. Then per case:
  * CPU-skinned: ensure `lod_merged > lod_queued` (else append another copy of the LOD mesh with `5CEA30(lod,1,2)`, `5CF050`, set `lod_dirty_mask` bit, `lod_merged++`); evaluate the pose into the actor's Model frames (0x5B5E40 with time `= (clock - start_time)`, clock = `DAT_00A500BA`, or `DAT_00A500B2` when `Actor+8 == -1`); then deform **into slot `lod_queued`** of the batch Model: vertex pointer advanced by `queued * vertex_count * stride` (strides 0x24 / 0x3C / 0x18 / 0x28 for vertex types 1/2/4/8) while `Model+0x10` is temporarily set to the LOD's vertex_count (0x5F4C10).
  * HW-skinned: evaluate pose into the actor's own frames; remember `Model+0` (frame array) in `lod_bone_arrays[lod][queued]`.
  * static: copy the actor's world matrix (`frame(0).world`, via 0x60EF80) - or identity if `Model+0x18 != 0` - into `static_mats[lod*16+queued]`.
  Always: store the Actor* in `lod_instances[lod][queued]`, `lod_queued++`, link the ActorDef into the caller's render list (+0x2CC/0x2D0/0x2D4). If `lod_queued >= lod_capacity` (checked before **and** after) the batch is flushed immediately via `0x5B1CB0`.
* `flush_batches`: for each LOD with `queued > 0`: (if dirty bit) re-sort triangles by plane; CPU-skinned: temporarily set batch `vertex_count = queued*lod.vertex_count`, `tri_count = queued*lod.tri_count`, draw once (`0x5D0570` with `Model+0x70 = 4` render mode); HW-skinned: per instance set `batch.frames = lod_bone_arrays[..]` and draw (first/multi-plane: `0x5D0570`, later single-plane: `0x5D2B80`); static: per instance copy `static_mats[..]` into `frame(0).local`, `touch()`, draw. Reset `lod_queued = 0`; when all LODs were flushed, unlink the ActorDef from the render list.

## animdef.cpp - runtime/pose side (loader rows are in [anim.md](anim.md))

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x5B5E40 | `animdef_evaluate_pose` (scene export; `this`=AnimDef; `(Model*, i32 clip, i16 set_override, f32 t_ms, i32 loop, i32* bone_map, Key* blend_from, f32 blend_ms, AnimDef** cursor_owner, i32* cursors)`) | **the pose evaluator**; algorithm in engine/actor.md | read in full | V |
| 0x5B5DD0 | `animdef_get_clip` | `clips + (idx-1)*0x41`; with `set_override != 0` first resolves that AnimDef (0x5B7650) | | V |
| 0x5B7C70 | `track_find_keys` (`this`=Track; `(f32 t, Key** A, Key** B, i32* cursor)` -> f32) | bracket search. n<2: A=key0 (NULL if none), B=NULL, f=0. Else with a valid cursor `c`: look +-4 keys around it (backwards if `t < key[c].time`, forwards otherwise) and finish with a linear walk; on miss do an **interpolation search** (guess index `lo + (t-t_lo)/(t_hi-t_lo)*(hi-lo)` via `__ftol`, narrow). Result: `t <= first.time` -> A=first, B=NULL; `t >= last.time` -> A=last, B=NULL; else `A=key[i], B=key[i+1]` with `A.time < t <= B.time` (if `t == B.time` A is key[i]), `f=(t-A.time)/(B.time-A.time)`; `*cursor=i`. Pure function of the key times, so any bracket search is equivalent | V |
| 0x5B6DC0 | `animdef_load_clip` | lazy: re-opens `<dir><name>.AM2` (`DAT_009CF5BC`), skips the header (16 B + `n_clips*0x3E`) and for every track keeps only keys with `clip.start <= time <= clip.end`, appending them with `grow_keys` and re-inserting the block in sorted position; sets `clip.loaded=1` | | R |
| 0x5B4170 | `animdef_unload(keep_header)` | waits for the async step machine (`SleepEx(5)` + 0x5B3100), frees bones (0x5B46F0), tracks (0x5B4920 -> `delete[]` of Track, each `0x5B7B80` frees its keys), colour (0x5B4790) / alpha (0x5B4830) curves, labels (0x5B48D0), the clip-order table (+0x13C), clips (+0x138), releases `n_instances` playback cursors (0x5B7650 / 0x417250); `keep_header` variant only frees tracks/curves and keeps clips | | R |
| 0x5B5A70 | `animdef_build_clip_order` | when state==0 and n_clips>0: `+0x13C = int[n_clips]` identity, `qsort(.., 4, 0x5B5B70)` (comparator reads the AnimDef through global `DAT_00A67684`) | | R |
| 0x5B7BF0 | `track_grow` (`this`=Track; `n`) | `realloc(keys, (count+n)*0x20)`, returns old count | | V |
| 0x5B7B80 / 0x5B8A50 / 0x5B90D0 / 0x5B96A0 | `track_free` / `colour_curve_free` / `alpha_curve_free` / `label_free` | free one array of the matching element type | | R |
| 0x5B8AC0 / 0x5B9140 / 0x5B9710 | `colour_curve_grow` / `alpha_curve_grow` / `label_grow` | tracked realloc of the curve/label arrays | | R |
| 0x5B8B40 | `colour_curve_find(this=Track-like {n, keys*16B}; t, Key16** A, Key16** B)` -> f | same bracket/interpolation search as 0x5B7C70 without cursor, 16-byte keys `{t,r,g,b}`; `t <= first` -> A=first,B=NULL; `t >= last` -> A=last,B=NULL | V |
| 0x5B91C0 | `alpha_curve_find` | same, 8-byte keys `{t,a}` | V |
| 0x5B8960 / 0x5B9020 | `colour_lerp` / `alpha_lerp` | `dst[i] = (1-f)*a[i] + f*b[i]` over 4 resp. 2 floats (time component included, ignored by caller) | V |
| 0x5B9DF0 / 0x5BA040 / 0x5BA0A0 | `bonetree_set_parent(child, parent)` / `bonetree_last_child(b)` / `bonetree_prev_sibling(b, c)` | slot layout in [BoneTree](../structs/BoneTree.md) | V |
| 0x5CC9E0 | `model_bone_changed_cb` | empty no-op callback | V |
| 0x5B8770 / 0x5B8840 / 0x5B8910 | `animkey_set_from_matrix` / `animkey_lerp` / `animkey_to_matrix` | see [AnimKey](../structs/AnimKey.md) | | V |
| 0x5B6A90 / 0x5B6B30 | `animdef_make_relative` / `_node` | children-first conversion of absolute to parent-relative keys; sets `relative=1`. For every key of bone *b* with parent *p*: find the p-key bracket for `time_b` (cursor walk), interpolate it (snap to endpoint if `t` within epsilon of 0/1), `K_b := M_b * inverse(M_p)` (D3D row-vector order), re-pack with 0x5B8770 | | V |
| 0x5B83A0 | `animdef_reduce_keys` | exporter-side key thinning (see anim.md) | | R |
| 0x5B4B90 / 0x5B5830 / 0x5B3100 / 0x5B6DC0 | loaders | see anim.md; the matrix-to-key packing is described in AnimKey.md | | V |

## bonetree.cpp

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x5B9830 | `bonetree_copy` | copy ctor: `memset(12)`, count, `(count+1)*0x1C` storage, copy slot 0 (virtual root) + every node (7 dwords) | V |
| 0x5B9990 | `bonetree_init(n)` | alloc `(n+1)*0x1C` zeroed, `nodes = storage+0x1C`, reset root (-1) and every node via 0x5B9790 | V |
| 0x5B9A80 | `bonetree_free` | | V |
| 0x5B9790 | `bonenode_reset(index)` | `{index, name_id=0, parent/links = -999}` | V |
| 0x5BA570 / 0x5BA660 / 0x5BA760 | `bone_names_init(array, n)` / `_destroy` / `_add(name)` | global name registry (`DAT_00A67698` = current registry): sorted index table of 0x38-byte `{id, char name[50]}` records; add copies <= 50 chars and inserts the id at its sorted position (bsearch 0x5911A0 + insert 0x5910F0), returns the new id | R |
| 0x5BA1D0 / 0x5BA300 / 0x5BA160 | `bonetree_compatible` / `bonetree_build_remap` / `bonetree_find_by_name_id` | see [BoneTree](../structs/BoneTree.md) | V |
| 0x5C5400 / 0x5C5560 / 0x5C5610 / 0x5C56D0 / 0x5B9AF0 | `modelset_load_am3_file` / `_mem` / `modelset_copy_tree` / `modelset_build_frames` / `bonetree_parse_am3` | see Binding | V |

## morph.cpp (format: [mda.md](../../formats/mda.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x5C67B0 | `morph_ctor` | memset 0x84 | V |
| 0x5C6820 | `morph_load(model_name, dir, name, D3DMATRIX*)` | first load of `<dir><name>.MDA` (`DAT_009D95FC`; 0x417D90 returns the assembled path, 0x590B80 existence test, error `Error loading Morph Data : %1s`). Reads part table -> `affected`, `base_pos`, `base_nrm` (+ optional transform); targets (`0x44` each) with 4 arrays, searching the destination slot by ascending index; allocates `cur_pos/cur_nrm/buf_a/buf_b`; `loaded=1`; finally `set_target(targets[0].name)` (0x5C7FF0) | V |
| 0x5C9B10 | `morph_load_append` | merge a second .MDA into an existing Morph by target name (0x5C90A0) | V |
| 0x5CA5D0 | `morph_append_copy(Morph*, D3DMATRIX*)` | append the vertex set of another Morph (index offset `vertex_base`), extend each target | R |
| 0x5C7370 | `morph_copy(Morph* src, D3DMATRIX*)` | deep copy (+ MorphCurve via 0x5CB1D0 if present) | V |
| 0x5C7B80 | `morph_free` | frees every array/target, curve; resets counts | V |
| 0x5C7FF0 | `morph_set_target(name)` | idx = 0x5C90A0(name); if changed: `cur_pos <- targets[idx-1].pos`, `cur_nrm <- ...nrm`; `t_a = t_b = DAT_00A500BA; blend = 0` | V |
| 0x5C90A0 | `morph_find_target(name)` | 1-based index or 0 | V |
| 0x5CAE20 / 0x5CAE50 / 0x5C7FA0 | `morph_curve_ctor` / `_dtor` / `_deleting_dtor` | | R |
| 0x5CB1D0 | `morph_curve_copy` | | R |
| 0x5C8960 | `morph_apply` (`this`=Morph; `(Model*, SkinSet* or NULL)`) | per-frame, once per Morph from 0x5AD060. Skips if `last_apply == now`. (1) `has_curve` -> `curve.sample(now - curve_t0)` (0x5CB3D0, loops) which rebuilds `cur_pos/cur_nrm` from weights (0x5C9120). (2) else if `cur_target == 0` (timed transition): while `now < t_end`: once `now >= t_start`, `cur += frac * step` with `frac = (now - max(t_start,last_apply)) / duration` (incremental linear integration of `pos_step/nrm_step`); when `now >= t_end` (and `last_apply < t_end`): `cur_target = pending_target` and snap `cur` to that target's absolute arrays. (3) write into the Model's vertex array: type 7: position of vertex `affected[j]` (stride 0x48) = `cur_pos[j]`, normal (+0x18) = `normalize(cur_nrm[j])`; non-skinned: position + (if the vertex type has a normal) `normalize(cur_nrm[j])`; CPU-skinned (SkinSet given): the vertex is assumed rigid on its **first influence bone** (`SkinVertex.bone[0]`): `pos = point_transform(cur_pos[j], world(bone0))`, then a normalize (the pseudocode normalises the transformed point into the normal slot - likely a decompiler loss of a `vector_transform(cur_nrm)`, TODO(re) verify with a trace). Then `last_apply = now`, vertex buffer dirty (`plane::fn_5CF920(2)`) | V (flow) / I (skinned normal) |
| 0x5CB3D0 / 0x5C9120 | `morph_curve_sample` / `morph_build_from_weights` | see [Morph](../structs/Morph.md) | V |

## skin.cpp and the v3d bridge

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x5F45F0 | `skinset_ctor` | | V |
| 0x5F46A0 | `skinset_clear` | `SkinVertex::deinit` of every element (`count` down to 0), free data | V |
| 0x5F47C0 | `skinset_alloc(n)` | grow tracked realloc by `n` x 0x84, `SkinVertex::init` each new element, returns first new index | V |
| 0x5F4970 | `skinset_add_bone(vtx, bone, pos*, nrm*, weight)` | thunk -> `v3d SkinVertex::add_bone` on `data + vtx*0x84` | V |
| 0x5F49C0 | `skinset_normalize(vtx)` | thunk -> `v3d SkinVertex::normalize` | V |
| 0x5DA280 | `smo_read_skinned` | picks vertex type 7 (HW) iff hw-flag && `DAT_00A5262B` && `5 <= quality <= 24`, else type 1; `Model+0x70 = 4`; dispatches version 100/0/50/500 | V |
| 0x5DC420 | `smo_read_skinned_v500` | builds SkinVertex sets (see SkinVertex.md) | R |
| v3d `skin_deform`, `point_transform`, `vector_transform`, `matrix_multiply[_translation]`, `matrix_inverse` | called through import thunks; selected at start-up per CPU (`x87/87/sse/k3d`) | | V |

## Math helpers seen
`0x60C840` matrix -> quaternion (D3DX `QuaternionRotationMatrix`, static at 0x767967); `0x60C8B0 -> 0x767EEB` quaternion slerp; `0x60C880 -> 0x767101` quaternion -> matrix; `0x60C310` copies row 3 (translation); `0x5AAEE0 -> 0x5AC230` is a trivial 22-byte routine passed as element ctor to `new[]` of 0x20-byte keys and called before key loops (inferred: AnimKey default ctor).

## Cross-module requests
* `TODO(docs)`: `ModelSet+0x44` (BoneTree*) and `+0x48` (hierarchy-built flag) are used by 0x5C5610/0x5C56D0/0x5B0620 and are missing from ModelSet.md (size 0x69 is consistent). `Model+0x06` is the **bone count** (`quality` slots) - ModelMesh.md calls it "slot_count"; `Model+0x00` is the array of [FrameNode](../structs/FrameNode.md) (0xA0 each), `Model+0x18` is set to 1 by `0x5F4C10` after CPU skinning and tested by the static-instance path (`!= 0` -> identity matrix) - meaning TODO(re).
* ActorDef.md `+0x10` was documented as a modelres index: it is an index into the 0x3A-stride animation table (0x4B2730). The modelres owner should confirm.
* Ghidra: apply names above; create struct Actor, FrameNode, Morph; set `this` types: `0x5B1180` = ActorDef, `0x5B5E40` = AnimDef, `0x60EE60/0x60EC70/0x60EE10` = FrameNode.
* `0x5B1180`, `0x5B1CB0`, `0x5D4760`, `0x5F4C10`, `0x5B5E40`, `0x5AD060` are documented here from the actor angle; link instead of duplicating.
* `DAT_00A500BA` (ms game clock used for all animation/morph timing) and `DAT_00A500B2` (alternate clock selected by `Actor.clock_sel == -1`) - confirm definition and pause behaviour.

## Scene integration, culling, bounds (`scene_s.cpp`)

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x5F07F0 | `scene_draw_actors(Scene* this, Camera*, pass, layer)` | per-frame actor draw (`d:\ww\en3d\scene_s.cpp`). **Pass 1 (opaque)**: walk the actor list (head `0x5EFFA0`, next = `Actor+0x20`), keep actors of the requested layer (`0x5AED50`) that pass `0x5AB470`; try `ActorDef::queue_instance` (0x5B1180); if it declines (returns 0) draw the actor alone with `0x5ABA20`; afterwards flush every ActorDef left in the batch list (`0x5B1CB0(cam,1,-1)` following `+0x2D0`). **Pass 2 (transparent)**: collect visible actors into a growable array (step 0x80), `qsort` with comparator `0x5F0780` (by `view_depth`), then queue/draw in that order and flush. Also draws sky/terrain/shadow hooks (not this family) | R |
| 0x5AB470 | `actor_cull(this=Actor; cam, pass)` | pass 1: `visible=0`; reject if `!enabled`; bounding sphere `(cx,cy,cz,r)` from `0x5ACC80(&s,1)`; **frustum test** `0x6159F0` (6 camera planes at `cam+0x326` + `cam+0x3FE` extra planes at `cam+0x39E`, each plane `n.p + d < -r` => outside; result 0 outside / 1 intersects / 2 inside); **portal/sector test** `0x615ED0`; if `cam+0xCC` set and `Actor+8 == 10`: far-distance test `0x615E10` (distance minus r < `cam+0xC8`); then `0x5AB620` (global render enables `DAT_00A0382C`/`DAT_00A03834`, `enabled`, `0x5D0480`); on success stores `view_depth` (`0x615E80`) and `visible=1`. Other passes return the cached `visible` | R |
| 0x5ABA20 | `actor_draw_single` | non-batched draw: applies fade (`0x5AB6E0`) if `fade_rate != 0`, `0x5AB6A0 -> 0x5AD060` selects/poses the model, then draws (`0x5D0570`); baked-frame clips are drawn by parenting the model's frame 0 under `world_frame` | R |
| 0x5AB6E0 | `actor_apply_fade` | see [Actor](../structs/Actor.md) `fade_rate` | R |
| 0x675B30 / 0x4230F0 | `game_object_free_actor` / `actor_deleting_dtor_thunk` | callers of `actor_destroy`; owner object keeps Actor at `+0x28`, an extra mesh at `+0x24`, and frees them (with `0x5EFDB0` = unlink from the actor list) | R |

**Bounding volumes**: each Actor exposes a bounding **sphere** (`vec4 center+radius`, getter `0x5ACC80`, not exported; likely derived from the Model/ActorDef bounds and the world frame). All culling (frustum, portal, far distance, transparent sort depth) uses this sphere. **Attachment points**: none as data; attaching = parenting a [FrameNode](../structs/FrameNode.md) under a bone frame (`0x60EC70`).

## Actor lifecycle and control API 

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x5AAB20 | `actor_ctor` | `memset(0x10C)`, `scale = 1.0`, `enabled = 1` (**sizeof(Actor) = 0x10C**) | V |
| 0x5AAB70 | `actor_init_model` (`this`=Actor with `def` set) | create instance: `world_frame` = new FrameNode (0xA0, identity, ctor 0x60EAE0 with callback 0x5AAEC0); `model` = new Model(0x11C) cloned from the ActorDef's LOD0 model (`plane::fn_5CBBF0(def.modelset.model, 1)`), render category `Model+0x70 = 4`, bind (0x5D03F0), `Model+0x0A = 0`; every root bone frame (parent 0) is parented under `world_frame`; allocates `blend_from = new AnimKey[bone_count]` (0x20 each), `blend_on = 0`, `cursor_owner = 0`, `cursors = int[bone_count]` iff `ModelSet.skinned == 1` else NULL; `clip = 1`, `start_time = DAT_00A500BA`, `scale = 1.0`, `fade_max = 0xFF`, `+0xFD = 1` | V |
| 0x5ADD40 | `actor_set_skin_variant(id)` | stores `id` in `Actor+0x2C`; for every plane of the model (and of the shared LOD0 model) whose index is in the variant table row (`0x4B27B0(id)`: 6 plane indices `i16[6]` at row+0x25, per-slot texture counts at +0x31, 0x21-byte texture names at +0x3C1 + slot*0x84) binds the listed textures (`0x6099C0` load, `0x5D5650` set slot) and tags the blend field `id | 0x1000000 | slot<<16`, flags `|= 0x100`. Team/colour texture variants | R |
| 0x5AC310 | `actor_set_position(vec3*)` | `world_frame.local.translation = pos` + touch; mirrors the position into the owner by `owner_type` (1/10, 6..9: owner+0x56 and `0x4311D0`; 0xB: owner+0xE and `0x4FEE60`) | V |
| 0x5AC6A0 (and clones 0x5AC810, 0x5AC980, 0x5ACAF0) | `actor_set_rotation(yaw)` / `_x/_y/_z` | builds a rotation from Euler angles (`0x60BF40(&m, yaw, f102, f106)`), multiplies all 9 elements by `scale`, writes the 3x3 into `world_frame.local` (`0x60C270`) + touch | R |
| 0x5ABE30 | `actor_play(AnimDef*, const char* action_id, i32 start_ms, i32 loop, i32 blend_on, f32 blend_ms)` | `anim = AnimDef`; `0x5B4A60` resolves the **action name** to `(clip_index, set_override)` (searches the AnimDef clip table `0x5B5BE0`, then its override sets; log `Cannot find a action for ACTION_ID: %s in %s!`), stored in `clip(+0x44)` / `anim_set(+0x48)`; `start_time = start_ms`, `loop`; if `blend_on` and `blend_from` exists: captures the **current local matrix of every bone frame** into `blend_from[i]` (`animkey_set_from_matrix(0, frame)`), `blend_on = 1`, `blend_ms = arg`. Then **spawns particle effects from the AnimDef labels** (see below) | V |
| 0x5AE400 | `actor_set_morph_curve(const char* morph_name, dir, file)` | finds the Morph whose name (`Morph+1`) matches, creates a MorphCurve, loads `<dir><file>` (suffix `DAT_009D9630`, error `Error loading Morph Animation : %1s`) via `0x5CAE80`, sets `Morph.curve_t0 = DAT_00A500BA`, `has_curve = 1` | V |
| 0x5CAE80 | `morph_curve_load(dir, file, Morph*)` | file = **the `.MA` format** ([ma.md](../../formats/ma.md)): `i32 n`, `f32 duration` (-> `curve.duration`, +0xC), then `n` records `{char name[50]; i32 n_keys; {f32 time, f32 weight}[n_keys]}`; each name is matched with `0x5C90A0` against the Morph's target names and the pairs land in `curve.entries[target-1]` (unmatched records are read and dropped) | V |
| 0x5AE590 | `actor_set_highlight(mode)` | `Actor+0x138 = mode`; mode 0 restores the original per-plane blend state/colour (white `0xFFFFFFFF`) from the shared LOD0 model | I |
| 0x52B970 | (unit spawn, other family) | shows the creation sequence: `new Actor` (`0x5AAB20`), `Actor.def = ActorDef`, `actor_init_model`, `set_skin_variant`, `set_position`, `owner_type = 0xB`, `owner = unit`, `set_rotation`, then one `actor_add_morph` per ModelRes morph row | R |
| 0x4B16A0 | `modelres_release` | ref-count (`entry+0x10`) dec; at 0: delete the shared `ActorDef` (`entry+0x1F`, via 0x4B0C20), release the anim record, delete the shared `Morph` cached in each morph row (`row+0xA0`) | R |

**Particle effects from animation labels** (consumer of AnimDef `+0x15B`, in 0x5ABE30): for every label (0x3C bytes `{f32 time, f32 bone, char name[50], ..., u8 flag at +0x3B}`) with `clip.start <= time <= clip.end` (skipped when `owner_type == -1`) the name is resolved to a particle script (`0x4C2060`, pscript, e.g. `weapon_heal.txt`) and an emitter is created (`0x41BAF0`, emitter table entry = `dynarrb` slot; `Actor+0x3C` keeps the slot for looping clips). Non-looping: lifetime `time .. time+5000 ms`. Emitter owner/attach = `entry.set_owner(actor, frame)` (`0x41B0D0`): `bone` (remapped through `ActorDef.bone_map` if present, `-NaN` = none) selects which bone [FrameNode](../structs/FrameNode.md) the emitter follows. Destroy (`0x5AAF10`) detaches every emitter whose owner is the actor.

## Bounds helpers 

| address | name | purpose |
|---|---|---|
| 0x5ACC80 | `actor_get_bounds(this=Actor, vec4* out, bool world)` | `out = ActorDef.modelset +0x34..0x43` = `{center xyz, radius}`; if `world`: kind 0: transform the center by `world_frame` (local matrix if it has no parent, else `world`); kind 1: add the vec3 at `extra+9`; kind 2: add the vec3 at `extra+0x44` (translation only); then `radius *= Actor.scale` |
| 0x5AEDB0 | `model_get_e8` | returns `this+0xE8` (the LOD-parent / baked list pointer of a Model) |
| 0x5CF8D0 | `model_get_frames` | returns `*this` = the FrameNode array pointer |

## Cross-module notes 
* **Plane draw_category (`Model+0x70`)**: value **4** is "actor instance model": written by the skinned-mesh reader `0x5DA280` / static reader `0x5DDD70` (see smo.md), by `actor_init_model` 0x5AAB70 on every cloned actor Model, and temporarily by the batch flush `0x5B1CB0` (set 4 around each draw, restored to 0). `0x5D4760` (LOD select) only works for category 4. The other categories (1, 3, 5-7) are not written anywhere in the actor export; they must come from the scene/terrain/world loaders.
* **ma.md**: `.MA` **is** the morph-animation format (loader `0x5CAE80`, error text `Error loading Morph Animation`): the records are per-**morph-target** weight curves `{time_ms, weight}`, not per-plane opacity; record names match the sibling `.mda` target names (as ma.md already observed). Please correct its title/intro.
* **ModelSet.md**: add `+0x34..+0x43` bounding sphere `{cx,cy,cz,radius}`, `+0x44` BoneTree*, `+0x48` hierarchy flag.
* **ModelRes (0x4B18D0 table)**: entry `+0x10` i32 ref-count, `+0x1F` ActorDef* (shared), `+0xE` i16 anim record id, morph rows (0xA4 each at `+0x27`) hold a shared `Morph*` at row `+0xA0`.

## Open questions (TODO(re))
* ActorDef constructor / exact sizeof (>= 0x2D8) and its creation site (callers of `0x5B03D0`'s counterpart `0x5AF3D0/0x5B0000` loaders; ModelRes entry `+0x1F`).
* **Morph timed transitions** (`t_start/t_end/duration/pos_step/pending_target`): no exported code arms them (only `set_target` 0x5C7FF0 and `build_from_weights` 0x5C9120, both of which set `t_end = now`), so that mode is probably dead in the shipped game; facial animation uses the `.MA` weight curves (0x5AE400).
* `Morph::apply` skinned-normal line (see above) and `v3d skin_deform` normal handling; the `0x5AE590` highlight semantics; `Actor` bytes listed as untouched; draw `0x5D0570`/`0x5D2B80`.

## Constants (float32 unless noted; read from .rdata)
`0x9CF5C4 = 0.0`, `0x9CF5C8 = 0.001`, `0x9D56D0 = 0.999` (snap epsilons in `make_relative`: `t <= 0.001` -> parent key A, `t >= 0.999` -> parent key B, else lerp), `0x9D525C = 0.25`, `0x9D8690 = 8.0`, `0x9CF794 = 2.0`, `0x9CF784 = 1.0`, `0x9CF77C = 0.5`, `0x9D8670 = 2*pi` (double), `0x9D74C0 = pi/2` (double), `0x9CF884 = pi/180`.
Consequences: **LOD**: `thr_4 = 8.0 * Model.lod_distance`, each lower LOD halves it (`/2.0`): LOD *n* is used when `distance >= 8 * lod_distance / 2^(4-n)`, i.e. thresholds 8d, 4d, 2d, d for LOD 4,3,2,1; closer than `d` -> LOD 0. **Cross-fade end test** (0x5B5E40): when the clip is no longer than `blend_ms`, the key at the clip end is used unless its blend factor `f < 0.25` (then the earlier key A); otherwise the key at `start+blend_ms` is used unless `f <= 0.0`.
