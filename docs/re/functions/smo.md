# smo family - mesh loading (`.SMO`, `.AM1`), actordef, modelres, plane

Reference build joan.exe. Status: verified = checked against file bytes; read = read in pseudocode; inferred = from context.
Source files: `d:\ww\en3d\actordef.cpp`, `d:\ww\en3d\plane.cpp`, `d:\ww\modelres.cpp`, `d:\ww\ilib\filemem.cpp`.
Format: `docs/formats/smo.md`; structs: `SmoHeader.md`, `SmoPlane.md`, `SmoVertex.md`, `ModelMesh.md`, `ModelSet.md`, `ActorDef.md`.

| Address | Proposed name | Role | Status |
|---|---|---|---|
| 0x5DDD70 | smo_read_static | `this`=File; reads version i32, resets model (`0x5CC7A0`,`0x5CDC70(1)`), dispatches 0/50/100/500 | verified |
| 0x5E0530 | smo_read_static_v500 | static v500 parser: header, planes, texture table, LOD blocks, tag trailer | verified |
| 0x5DDE90 / 0x5DECF0 | smo_read_static_v0 / v50 | legacy versions, not in shipped data | read |
| 0x5E0510 | smo_read_static_v100 | stub `return 0` | verified |
| 0x5DA280 | smo_read_skinned | reads version + `quality`, dispatches to skinned handlers; optional vertex mode 7 | read |
| 0x5DC420 | smo_read_skinned_v500 | skinned v500 parser (`.AM1`) with bone influences, tags 1/2 | read, header verified on ripple.AM1 |
| 0x5DA3D0 / 0x5DB710 / 0x5DC400 | smo_read_skinned_v0 / v50 / v100(stub) | legacy | read |
| 0x5C0EF0 | modelset_load | synchronous load of a ModelSet: main mesh, LODs `#1..#4`, `#V`-class and shadow `#s` meshes; skin object if `param_2` | read |
| 0x5C2370 | modelset_load_async_start | same set as `0x5C0EF0` but only opens 0x164-byte async file objects (base, LODs, two more) | read |
| 0x5C3110 | modelset_pump_async | async state pump consuming the buffers started by `0x5C2370` (5,958 B) | inferred |
| 0x5C4E30 | modelset_is_loaded | polled in the `SleepEx(5)` spin loops | inferred |
| 0x5C0EC0 | modelset_ctor | constructs the 0x69-byte ModelSet | read |
| 0x5C51B0 / 0x5C53B0 | modelset_finish_static / skinned | post-LOD fix-up (static adds mip/LOD list; skinned) | inferred |
| 0x5CBAF0 | model_ctor | `memset(0x11C)`, type byte +4, flags +5, +9=0xFF | verified |
| 0x5CBB60 | model_ctor_lod | constructs a LOD model | inferred |
| 0x5CC7A0 | model_init | init counts: `(?, vtx_reserve, tri_reserve, plane_reserve, bone_budget)`; allocates `quality` x 0xA0 byte slots | read |
| 0x5CCAA0 | model_free_buffers | frees vertex/tri/plane arrays | read |
| 0x5CD470 | model_alloc_planes | grow SubMesh array (stride 0x54), returns first new index; also grows parallel arrays at +0x84 (0x40/plane, matrices), +0x88, +0xCC | verified |
| 0x5CD7F0 | plane_add_vertices | (name previously `model_alloc_vertices`) grow vertex array according to vertex type (+4) with strides 0x24,0x3C,0x1C,0x18,0x48,0x14,0x48,0x28; returns base index | verified |
| 0x5CE6F0 | model_alloc_triangles | reserves triangle slots, returns base | inferred |
| 0x5CE800 | model_set_triangle | writes one triangle (plane, a, b, c) | inferred |
| 0x5D6EE0 | model_alloc_strip_indices | reserves strip index slots for a plane (`+0x60` u16 array) | inferred |
| 0x5D52A0 | submesh_init | `memset(0x54)`, default colours, +0x34=?/+0x38 defaults, white `0x5E95B0` | read |
| 0x5D5570 / 0x5D5540 / 0x5D55A0 / 0x5D55E0 | submesh_set_color0..3 | set one of four D3DCOLOR fields from the plane record | inferred (not exported) |
| 0x5D5650 | submesh_set_texture | `(slot, texture*)` - bind texture (slots 0..2) | read |
| 0x5D4470 | model_add_lod | creates/returns a LOD Model (0x11C), registers distance | read |
| 0x5D4720 | model_set_lod_info | `+0xE4 = lod_count (byte)`, `+0xE0 = distance (float)` | verified |
| 0x5D4A00 | model_set_lod_current | attaches LOD to parent | inferred |
| 0x5CFA10 | model_upload_buffers | creates + fills D3D vertex/index buffers ("Cannot create/lock vertex/index buffer (%s)"); index buffer = triangle list (+0x48, 6 B per tri) or strip (+0x60) when flag +0x50 | read |
| 0x5CF920 | model_finalize | called after load; plane.cpp | inferred |
| 0x5D9BE0 | model_release_gpu | frees GPU buffers | inferred |
| 0x5CDC70 | plane_set_vertex_type | `(this=plane, new_type)`: allocates the array for the new type (strides 0x24, 0x3C, 0x1C, 0x18, 0x48, 0x14, 0x48, 0x28 for types 1..8; type 5 depends on a device cap, `DAT_00A52623`), converts existing vertices from the old type (helpers 0x60F020..0x60F4B0) and frees the old array. Reconciled with world_binary.md (review L1): the earlier name `model_set_mode` was wrong, the pseudocode is a vertex-format conversion, not a mode switch | read |
| 0x5D7270 / 0x5D5EE0 | model_compute_aux / model_enable_layers | post-load helpers | inferred |
| 0x5F45F0 | skin_ctor | `memset(0x3E)` skin object | verified |
| 0x5F4970 / 0x5F49C0 / 0x5F47C0 | skin_add_influence / skin_finish_vertex / skin_alloc_planes | per-vertex bone influence builder (vertex idx, bone, offset, normal offset, weight) | read |
| 0x4B18D0 | modelres_register | adds a model-table entry (0x2B bytes) from "name.ext"; type `'S'`/`'A'`; copies MORPH resource rows (0xA4 bytes) | read |
| 0x4B20E0 | modelres_find | lookup in the table | inferred |
| 0x4B2730 | modelres_get | `(index)` -> entry pointer | read |
| 0x4B27B0 | modelres_get_group | small helper for group loop (`DAT_00A47C05` count) | inferred |
| 0x5B0000 | actordef_load_async | build "<dir><name>" + `.SMO`/`.AM1`, check exists, ModelSet via `0x5C2370`, open `.AM3` via async file, state 0x15 ("Error loading 3D Model : %1s") | read |
| 0x5AFDD0 | actordef_load_sync_from_table | same but waits for pending job, `0x5C0EF0`, then `0x5B0620` | read |
| 0x5AF3D0 | actordef_load_sync | sync loader with explicit user ptr | read |
| 0x5AF5D0 | actordef_load_async_sync | variant: async with state byte 0x0B | read |
| 0x5AF890 | actordef_update_load | state machine (0x0B -> 0x0C -> 0x0D -> done; 0x15 -> 0x16) driving `0x5C3110` and `0x5B0850` | read |
| 0x5AFDA0 | actordef_is_ready | polled before load | inferred |
| 0x5AFD50 | actordef_close_file | release the `.AM3` file object | inferred |
| 0x5B03D0 | actordef_release | frees actors[5] (+0x178), buffers, ModelSet | read |
| 0x5B0620 | actordef_bind_anim_file | open `<dir><name>.AM3`, build bone-tree remap | read |
| 0x5B0850 | actordef_bind_anim_mem | same from a memory file (`0x5C5560`) | read |
| 0x5AAF10 | actor_release | actor destructor chain (`actor.cpp`) | read |
| 0x5ADF50 / 0x5AE0C0 | actor_morph_* | morph helpers (`morph.cpp`, "Error loading Morph Data") | read |
| 0x5CBBF0 / 0x5CCAA0 / 0x5CD470 / 0x5CD7F0 | plane.cpp mem mgmt | see rows above | read |
| 0x5893C0 | file_read_i32 | `File::read_i32` (via buffer or memory file); "Error reading file %s" | verified |
| 0x588A80 | file_read_block | `(dst, n)` returns bool | verified |
| 0x5892F0 | file_read_u16 | | verified |
| 0x5888C0 | file_skip | `(n, whence)` | read |

Path helper (400-char strings, `0x5A1170` clear, `0x5A1440` copy, `0x5A1550/0x5A14F0` append, `0x5A15D0` append decimal, `0x5A13A0` find, `0x5C57C0/0x5C5800` left/right substring, `0x590B80` file exists).

## Cross-module requests
- Rename 0x5DDD70 `smo_read_static`, 0x5DA280 `smo_read_skinned`, 0x5E0530 `smo_read_static_v500`, 0x5DC420 `smo_read_skinned_v500`, 0x5C0EF0 `modelset_load` in Ghidra; set `this` = File for those.
- The 0x11C struct is called `Model` here (en3d `plane.cpp` class, "plane" in the module name). The anim family should use `ModelSet` (0x69) and `ActorDef` names from `docs/re/structs`.
- `.AM3`/`.AM2` file readers (`0x5B3100`, `0x5B23D0`, ...) belong to the anim family; `ActorDef.md` documents only their hook.
- `DAT_009CF794` (global scale applied to bounds / LOD distance) - owner: engine globals.

## Open questions
- 0x5DC420 skinned vertex block A/B and the `ripple.AM1` mismatch (see smo.md 4.4).
- 0x5D5570/5540/55A0/55E0 colour channel order; which colour is diffuse/ambient/specular.
- 0x5C0EF0 literal suffix arguments for LOD / `#V` / `#s` (decompiler dropped the string args).
- Why `#V.SMO` is loaded: confirm purpose (hit volume?).
- Version 0/50 handlers `0x5DDE90`, `0x5DECF0`, `0x5DA3D0`, `0x5DB710` - legacy, never needed unless mods ship old files.
