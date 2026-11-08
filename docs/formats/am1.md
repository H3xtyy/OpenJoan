# `.am1` — skinned model (SMO container + bone-weighted vertices), 588 files

`.am1` is **not** an animation file: it is the model container of [`.smo`](smo.md) (same reader, same header, same plane records) used for models that
are driven by a skeleton. The readers pick the extension from one flag: `FUN_005B0000` / `FUN_005AF5D0` / `FUN_005AFDD0` / `FUN_005AF3D0` append `.AM1`
(literal 0x9D52B0) when the "skinned" argument is non-zero and `.SMO` (0x9D52B8) otherwise, then open it with `FUN_005C2370`/`FUN_005C0EF0`
(joan.exe, `smo` family: the body of the format is specified in smo.md; this page adds only what the skin layer needs). Skinned models also load
`<name>.AM3` ([am3.md](am3.md)) next to the `.am1` (`actordef` 0x5B0620 / 0x5AF5D0: `param_1[1]` = bone tree) and are animated by
an [`.am2`](am2.md) set matched by bone name. LOD files: `<name>#2`, `#3`, `#s` (shadow) etc. load the same way (0x5C0EF0 loop `1..4`).
Skinned geometry always uses vertex type 7 ("skinned": 0x48-byte vertex incl. 2 bone slots) when `am1`, type 1 for plain `.smo` (0x5DA280, 0x5DC420).

## Header (version dispatch at 0x5DA280)

| offset | type | meaning | evidence |
|---|---|---|---|
| 0x00 | i32 | version: **500 (0x1F4)** in every shipped file; the reader also accepts 0, 50 (0x32), 100 (calls 0x5DA3D0, 0x5DB710, 0x5DC400) | 0x5DA280; heads `f4 01 00 00` x all .am1/.smo |
| 0x04 | i32 | model kind/flags (`0x1A` = 26 in alencon_p.am1; 1 in props; 0x17/0x18 in the heads of inventory files; values 5..0x18 enable a low-detail variant when a global flag is set) | 0x5DA280 `5CC7A0(0,0,0,kind)`; inventory shows `f4 01 00 00 17 00 00 00` x201, `.. 01 00 00 00` x106, `.. 18 ..` x72 — the top inventory values (0x17, 0x01, 0x18) coincide with common .am3 bone counts (23/1/24) but 0x1A for alencon_p.am1 is unchecked against alencon_p.am3, so "bone count" is only a hypothesis (TODO(re): 0x5CC7A0) |
| 0x08 | i32 | `n_planes` (3 in alencon_p.am1) | 0x5DC420 `local_18` |
| 0x0C | i32 | `n_textures` (1) | `local_1c` |
| 0x10 | i32 | `local_20` (0) — TODO(re) | |
| 0x14 | f32[4] | bounding sphere centre xyz + radius (-3.37, -5.04, 58.45, 66.64); radius is divided by a constant at load (`/DAT_009CF794`) | 0x5DC420 |

Then `n_planes` plane records starting at 0x24 (offsets for alencon_p.am1):

| rel. | type | meaning |
|---|---|---|
| +0x00 | i32 | `tri_count` (1829); the strip has `tri_count + 2` indices (**corrected**: earlier text called it `n_indices`) |
| +0x04 | i32 | `n_vertices` (623) |
| +0x08 | f32[4] x3 | three colours (material ambient/diffuse/specular?), e.g. (1,0,0,0), (1,0.588,0.588,0.588), (1,0.9,0.9,0.9); stored as bytes `ftol(255*x)`. Channel order TODO(re) |
| +0x38 | f32[4] | per-plane sphere/extra (-3.66, 0.956, 70.16, 31.57) — TODO(re) |
| +0x48 | i32 | texture slot (0) |
| +0x4C | u16[tri_count + 2] (only if `tri_count > 0`) | **triangle-strip** indices (degenerate repeats `0,1,2,3,3,4,4,4,5...` visible at 0x70); the reader rebuilds triangle lists dropping degenerates and flipping every other triangle (0x5DC420) |

Verified: 0x24 = 0x725 = 1829 and 0x28 = 0x26F = 623; strips start at 0x70 (`00 00 01 00 02 00 03 00 03 00 04 00 04 00 04 00 05 00`). After the planes of the
file come the texture-name table (`n_textures` x {char name[20], i32 embedded_size [+ embedded .dds/.tga bytes]}), then optional extension chunks keyed by a leading
dword (1 = per-plane material overrides, 2 = per-plane flag bytes) — see smo.md for these (0x5DC420 tail).

## Per-vertex skin block (the part that makes it an .am1) — RE: 0x5DC420 (v500) / 0x5DB710 (v50)

Read after the index strips, once per vertex (`n_vertices` of each plane, plane order):

| size | type | meaning |
|---|---|---|
| 8 | f32[2] | texture coordinate (u, v) -> vertex +0x40 (type 7) / +0x1C (type 1) |
| 4 | i32 | `skip` — number of dwords to skip (`skip*4` bytes; 0x5888C0). **Resolved**: a list of vertex numbers (mostly `< n_vertices`, probably duplicate/neighbour vertices), unused at load; the reader keeps them as `SkinData::extra_indices` |
| 4 | i32 | `n_inf` — number of bone influences |
| n_inf x 32 | `Influence` | |

`Influence` (file order): `i32 bone` (index into the .am3 / track index), `f32 weight`, `f32 pos[3]` (vertex position in that bone's local space),
`f32 normal[3]` (normal in bone space). v50 files carry a single vec3 that is used for both. Each influence is fed to `SkinVertex::add_bone(bone, pos, normal, weight)`
(0x5F4970, exported from v3d_*.dll as `?add_bone@SkinVertex@@QAEHHAAU_D3DVECTOR@@0M@Z`), and after the vertex `SkinVertex::normalize()` (0x5F49C0) renormalises the
weights. A vertex keeps at most **4** influences (see [SkinVertex](../re/structs/SkinVertex.md)); the first two (bone*3, weight, pos, normal) are mirrored into the
0x48-byte hardware vertex (`0x48`-stride array at model+0x34: pos0/pos1, normal0/normal1, bone offsets `u16[2] = bone*3`, weights `f32[2]`, diffuse, uv) so
the vertex shader (`hardware_skin.nvo`, constant registers 3 per bone) can blend two bones; with 3+ influences (flag `local_bc`) or when `SOFTBONE.SYS` exists in the
game directory (0x5F6C00 sets `device+0x20B = 0`) software skinning `skin_deform` (v3d export, signature below) is used instead.
**Correction:** static `.smo` files do *not* carry this block; they use the 32-byte rigid vertex (smo.md 4.2). In `.am1` the rigid position/normal exist only as bone-local offsets of the influences. Influence counts seen in the install: 1 (305,547 vertices), 2 (35,098), 3 (1,179), 4 (199), 5 (60), 7 (5); weights sum to 1. The parse of every `.am1` consumes the file exactly (588/588), see smo.md section 13.

Software path signature (v3d_sse.dll, demangled): `void skin_deform(int count, D3DVECTOR* out_pos, unsigned pos_stride, D3DVECTOR* out_normal,
unsigned normal_stride, SkinVertex* verts, unsigned vert_stride, D3DMATRIX* bone_palette, unsigned palette_count)` (parameter names inferred from the
mangled types `HPAU_D3DVECTOR@@I0IPAUSkinVertex@@IPAU_D3DMATRIX@@I`).

## Worked example — `unit/alencon_p.am1` (155,347 bytes)

```
0x00 f4 01 00 00   version 500
0x04 1a 00 00 00   kind 26
0x08 03 00 00 00   n_planes 3
0x0C 01 00 00 00   n_textures 1
0x10 00 00 00 00
0x14 d5 d4 57 c0 | 43 53 a1 c0 | 1a d0 69 42 | 06 4a 85 42   sphere (-3.372, -5.041, 58.453, 66.645)
0x24 25 07 00 00   n_indices 1829
0x28 6f 02 00 00   n_vertices 623
0x2C 00 00 80 3f 00.. (color0)  0x3C 00 00 80 3f 2b 87 16 3f x3 (color1)  0x4C 1.0, 0.9 x3 (color2)
0x5C d6 29 6a c0 ... (-3.659, 0.956, 70.16, 31.57)   0x6C 00 00 00 00 (texture slot)   0x70 u16 strip...
```
The vertex/skin region starts at 0x70 + 2*1831 = 0xEBA... (indices are `tri_count + 2`); verified on the shipped files.

## Skeleton, bind pose and vertices (derived from the shipped data)

- **Skeleton.** The `.am3` is the sibling with the same base name (also for `#2`/`#3`/`#s` LOD names), else
  `unit/<base>.am3`, else `anim/<base>.am3`. 584 of 588 `.am1` have the sibling; the other 4 have none. Bone `i` of the skin is bone `i` of the
  `.am3` ([am3.md](am3.md)); the `.am3` has no transforms.
- **Bind pose.** The `.am1` stores every influence's position/normal in *bone-local* space, so a model-space pose has to come from an `.am2`.
  The `.am2` is found as `anim/<base>.am2`, `<dir>/<base>.am2`, then both again with a `#...` LOD suffix removed; 365 of 588 `.am1` have no
  match (items, props, projectiles whose animation is bound through `ANIMATE`/`animscr.res`) and 32 matches have a different track count than
  the `.am3`. The pose the skinning data agrees with is **the key at the start of the `default` clip** (clip 0 when there is no `default`, e.g.
  the `*_p` portrait sets), converted to parent-relative like the original ([am2.md](am2.md)). Decided from the data: for `unit/alencon.AM1` +
  `anim/Alencon.am2` the model-space positions that the influences of one vertex produce (`world[bone] * influence.offset`) agree to a mean of
  0.026 units (worst 0.64; the character is 83 units tall) with the `default` pose and to 2.1-3.8 units with the first key of any other clip.
  Over all 191 `.am1` with a usable `.am2`: 168 agree to a mean < 0.1, 4 to < 1, 19 are poor (mean 1-9 units: portraits `*_p`, `Trebuchet`,
  `Matthew`, ... whose `default` clip is not the modelling pose; TODO(re) which pose they were skinned in).
- **Vertices.** *Every influence carries its own position and normal*, so the model-space bind position is the weighted blend
  `P = sum_i w_i * (World[bone_i] * offset_i)` and the normal `N = normalize(sum_i w_i * R[bone_i] * normal_offset_i)` (weights sum to 1, up
  to 7 influences); this is what `skin_deform` evaluates for the bind pose.
- **Influence counts.** 305,547 vertices have 1 influence, 35,098 have 2, 1,179 have 3, 199 have 4, 60 have 5 and 5 have 7. The engine keeps at
  most 4 per vertex.
- **Axes.** Z-up, left-handed D3D data (see [smo.md](smo.md)): `alencon.AM1` stands on z = 0 (z range -0.1 .. 83.5).

## Confidence
Container dispatch, skin influence record, 0x84-byte `SkinVertex` and the 4-influence limit: **verified** (code + struct strides). Plane-record header
offsets: **verified at the first plane** for alencon_p.am1; colour channel order, `skip` field and the unknown dwords: TODO(re). `zCripple`: appears in the
literal list near `SOFTBONE.SYS` (0x9DD6D2) but no code analysed references it, and no `zCripple.*` file ships; likely an unused debug name.

## Further notes

- The model-space vertex is the sum over influences of `weight * bone_matrix * position`: positions and normals are stored in each
  bone's space, so no bind pose or inverse bind matrices exist in the file.
- The per-vertex `ids` list (the `skip` dwords above) has no known meaning; its values exceed the plane's vertex count and are shared across
  planes, so they are probably ids from the source Max mesh. The reader never uses them.
- Influence counts per vertex: Joan has at most 2 and the horse up to 3; the install reaches 7 (histogram above). The hardware path blends only two.
