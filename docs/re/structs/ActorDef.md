# ActorDef (in memory, `actordef.cpp` / `actordeg.cpp`) and ModelRes table entry (0x2B bytes, `modelres.cpp`)

Reference build `joan.exe`. The structure is **packed (`#pragma pack(1)`)**: several dwords sit at unaligned offsets (e.g. `AnimDef+0x148`).
An `ActorDef` is the shared, per-model-file definition: ModelSet (meshes + LODs), bone tree, bone remap, and the per-LOD **instance batching state**.
Per-object state lives in [Actor](Actor.md) (`Actor+0` points to its `ActorDef`).

## ActorDef - offsets from `0x5AF890`, `0x5B03D0`, `0x5B0620`, `0x5B0850`, `0x5B0AB0`, `0x5B1180`, `0x5B1CB0`

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `ready` | set 1 when load finished; also tested by release (0x5B03D0 does nothing if 0) | verified (0x5AF890, 0x5B03D0) |
| 0x03 | 1 | u8 | `state` | load state machine: 0x0B open -> 0x0C pump -> 0x0D done -> 0; 0x15 -> 0x16 -> 0 | verified (0x5AF890) |
| 0x04 | 4 | AsyncFile* | `anim_file` | 0x164-byte object for `<name>.AM3` (suffix `DAT_009CF5B4` = `.AM3`) | read |
| 0x08 | 4 | ptr | `user` | `param_6` of the loader; passed to 0x5B0850 as the memory-file (`.AM3` already in RAM) | read (0x5AF890) |
| 0x0C | 4 | [ModelSet](ModelSet.md)* | `modelset` | freed by 0x5B05D0 | verified |
| 0x10 | 2 | i16 | `anim_res` | record index in the **0x3A-stride** animation table (`0x4B2730(i) = table.base + i*0x3A - 0x3A`, then 0x4168D0/0x4174F0 -> `AnimDef*`); 0 = none. Earlier revision called this "modelres index" - the stride 0x3A rules that out (ModelRes rows are 0x2B) | read (0x5B0620, 0x5B0850, 0x5B03D0, 0x5AF890) |
| 0x14 | 4 | i32* | `bone_map` | `int[AnimDef.n_tracks]`, track -> model bone index, `-999` = no bone; NULL = identity. Built by 0x5BA300 only when 0x5BA1D0 says the trees differ; passed as `param_7` to the pose evaluator 0x5B5E40; freed in release | verified |
| 0x18 | 4 | Model**[] | `baked_frames` | per clip: array of pre-baked vertex-animation `Model*` frames (NULL when the clip is skeletal); indexed `[clip]` then `[frame]`; chosen by `floor(t/duration*(n-1))`. Only read by 0x5AD060 (`*(def+0x18)`) - writer not exported, TODO(re) | inferred |
| 0x1C | 4 | i32*[] | `baked_counts` | frame counts matching `baked_frames` | inferred |
| 0x20 | 4 | i32 | `lods_built` | 0 until 0x5B0AB0 (`actordeg`) has run; set to 1 at its start; cleared in release | verified (0x5B0AB0, 0x5B1180) |
| 0x24 | 5 | u8[5] | `lod_capacity` | per LOD: how many instances fit in the batch Model (CPU-skinned: `min(0x1000/vtx, 0x4000/(tri*3), 16)`; static / HW-skinned: 0x10) ; 0 = LOD unusable | verified (0x5B0AB0, 0x5B1180) |
| 0x29 | 5 | u8[5] | `lod_merged` | per LOD: copies of the mesh already merged into the batch Model (static/HW: 0x10 from the start; CPU-skinned: grows by 1 each time `queued >= merged`) | verified (0x5B1180) |
| 0x2E | 5 | u8[5] | `lod_queued` | per LOD: instances queued this frame; reset to 0 by the flush 0x5B1CB0 and in release | verified |
| 0x33 | 1 | u8 | `lod_dirty_mask` | bit `lod` set when copies were merged; flush then calls the triangle-by-plane sorter (`plane::fn_5CF1D0(0)`) and clears it | read (0x5B1180, 0x5B1CB0) |
| 0x34 | 4 | Mat4* | `static_mats` | static (non-skinned) models only: 0x1400 bytes = `[lod*16 + n]` x 0x40-byte world matrices of the queued instances; freed in release | verified (0x5B0AB0 alloc 0x1400, 0x5B1CB0) |
| 0x38 | 5x16x4 | Actor*[5][16] | `lod_instances` | stride 0x40 per LOD, 4 per instance slot: the `Actor*` queued in slot `n` of `lod` | verified (0x5B1180 writes `+0x38+lod*0x40+n*4`) |
| 0x178 | 4x5 | Model*[5] | `batch_models` | one 0x11C-byte [Model](ModelMesh.md) per LOD (batch target / merged geometry), freed via 0x459C40 | verified |
| 0x18C | 5x16x4 | ptr[5][16] | `lod_bone_arrays` | HW-skinned (vertex type 7) only: the bone-frame array (`Model+0`) of each queued instance | verified (0x5B1180, 0x5B1CB0) |
| 0x2CC | 4 | List* | `list_owner` | render-list head this ActorDef is currently linked into (`{first,last}`), | read |
| 0x2D0 / 0x2D4 | 4+4 | ActorDef* | `list_prev` / `list_next` | intrusive list links | read |

sizeof >= 0x2D8 (ctor not exported; TODO(re): exact size, ctor/deleting dtor 0x47B5B0 -> 0x5B03D0).

Loader flow (smo family, see [smo.md](../functions/smo.md)): path = `<dir><name>` + (`.SMO` if static, `.AM1` if skinned); missing file -> log
"Error loading 3D Model : %1s". ModelSet is built (`0x5C0EF0` sync / `0x5C2370` async) and `<dir><name>.AM3` is opened for the model skeleton.
`0x5B0620` / `0x5B0850` then bind the skeleton ([actor.md](../functions/actor.md) section "Binding"), and `0x5B0AB0` builds the per-LOD batch Models on first use.

## ModelRes entry (`0x4B18D0`), 0x2B bytes, array `local_8[2]`, count `local_8[0]`, capacity `local_8[1]`

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 4 | char* | base_name | pooled string (extension removed) |
| 0x04 | 2 | i16 | index | 1-based position |
| 0x06 | 4 | char* | dir | directory prefix (shared with previous entry when equal) |
| 0x0A | 4 | char* | full_name | dir + name |
| 0x0E | 2 | i16 | param | caller `param_3` |
| 0x15 | 1 | u8 | kind | `'S'` (0x53, .SMO, default) or `'A'` (0x41, .AM1) |
| 0x16 | 4 | i32 | link | id of matching row in table group (`DAT_00A47C05` loop) |
| 0x23 | 4 | i32 | morph_count | rows from the "MORPH" resource whose key equals the full name |
| 0x27 | 4 | ptr | morphs | morph_count x 0xA4 bytes: char[0x20] a @0x00, char[0x20] b @0x20, char[0x20] c @0x60, i32 @0xA0 = 0 |
