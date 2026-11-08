# SkinVertex — 0x84 bytes (en3d::skin, `d:\ww\en3d\skin.cpp`; methods exported from `v3d_{x87,87,sse,k3d}.dll`)

Array container `SkinSet` (0x5F45F0 ctor, 0x5F46A0 clear, 0x5F47C0 `alloc(n)` returns first index): `{i32 count; i32 capacity; SkinVertex* data (+8); i32 ?(+0xC); ... ptrs at +0x36/+0x3A}`; each `SkinVertex::init()`/`deinit()` is a v3d export.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `n_inf` | influences in use (0..4) | 0x5DC420 reads `*(sv)` and clamps to 2 for the hw vertex |
| 0x04 + 0x20*k | 4 | i32 | `bone` | bone index (hw vertex stores `bone*3`) | |
| 0x08 + 0x20*k | 12 | D3DVECTOR | `pos` | position in bone space | |
| 0x14 + 0x20*k | 12 | D3DVECTOR | `normal` | normal in bone space | |
| 0x20 + 0x20*k | 4 | f32 | `weight` | normalised by `normalize()` | |

k = 0..3, so sizeof = 4 + 4*32 = 0x84 (matches the `*0x84` strides). v3d exports (demangled): `SkinVertex::init()`, `deinit()`, `operator=`, `int add_bone(int bone, D3DVECTOR& pos, D3DVECTOR& normal, float weight)`,
`normalize()`, and `void skin_deform(int, D3DVECTOR*, unsigned, D3DVECTOR*, unsigned, SkinVertex*, unsigned, D3DMATRIX*, unsigned)` — see docs/re/pe/v3d_sse.dll.md.
**CPU skinning call (RE: 0x5F4C10, in the scene export)**: `skin_deform(n = SkinSet.count, out_pos = vertex array base (0x5CDAE0), pos_stride, out_nrm = out_pos + normal offset (table 0x600200 by vertex type; a scratch buffer with stride 0 when the type has no normal), nrm_stride, SkinVertex* = SkinSet.data (+8), 0x84, D3DMATRIX* = Model.frames + 0x58 (each bone's **world** matrix, [FrameNode](FrameNode.md)), 0xA0)`.
Reading: for each vertex, `pos = sum_k w_k * (pos_k x M[bone_k])`, `normal = sum_k w_k * (normal_k x M3x3[bone_k])` over `n_inf` influences (SkinVertex positions are in **bone space**, so no inverse-bind matrices exist anywhere); exact normal handling (renormalise?) lives in the `v3d_*.dll` body - TODO(re). Before the call every bone's world matrix is refreshed with `0x60EE60`; after it the Model's vertex buffer is flagged dirty (`plane::fn_5CF920(2)`), preserving the `+0x0A` byte.
Building (`0x5DC420`, per vertex, file record `{8 bytes read straight into the vertex (uv, at vertex +0x1C for type 1 / +0x40 for type 7), i32 n_skip (dwords skipped), i32 n_inf, then n_inf x {i32 bone; f32 w; vec3 pos; vec3 nrm}}`): `init` via `0x5F47C0 alloc`, `add_bone` per influence, `normalize()` once; for the **HW path (vertex type 7)** the first two influences are then copied into a 0x48-byte vertex `{pos0[3], pos1[3], nrm0[3], nrm1[3], i16 bone0*3, i16 bone1*3, f32 w0, f32 w1, u32 diffuse, f32 uv[2]}` (missing 2nd influence = zero vec/index 0/weight 0; `n_inf > 2` just drops the rest).
Type 7 is chosen by `0x5DA280` only if the file requests it, the device supports it (`DAT_00A5262B`) and `5 <= bone_count <= 24` (`0x18`); else vertex type 1 (0x24 bytes) + CPU `skin_deform`.
Call sites: 0x5DC420 / 0x5DB710 / 0x5DA3D0 (`add_bone` = 0x5F4970, `normalize` = 0x5F49C0, both thunks of the v3d imports).
