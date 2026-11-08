# AnimKey / Track / Clip (en3d::animdef)

## AnimKey — 32 bytes (RE: 0x5B8770 set, 0x5B8840 lerp, 0x5B7BF0 alloc stride 0x20; file form of `.amt`)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | f32 | `time` | ms (30 fps grid) | 0x5B8770 `*p = time` |
| 0x04 | 16 | f32[4] | `rot` | quaternion x,y,z,w (w last; identity = 0,0,0,1) | 0x60C840 matrix->quat; bigred.AMT |
| 0x14 | 12 | f32[3] | `pos` | translation | 0x60C310 copies matrix row 3 (+0x30..0x38) |

Interpolation (0x5B8840): `time`, `pos` linear with weight t; `rot` through 0x60C8B0 (-> 0x767EEB, quaternion blend). Keys for evaluation are looked up by 0x5B7C70 (binary/linear search with a cursor).

### Added by the actor-family pass (RE: 0x5B4B90, 0x5B6DC0, 0x5B8840, 0x5B8910, 0x5B6B30)
* **Disk -> memory**: each `.am2` key is `f32 time` + 12 x `f32 m[12]` (0x34 bytes). The loader builds a D3D matrix `M` (identity, then) with `M[r][c] = m[4*c + r]` for `c=0..2, r=0..3`
  (so `m[3]`, `m[7]`, `m[11]` land in row 3 = translation) and stores `AnimKey{time, quat(M), pos=M.row3}` through `0x5B8770` (quaternion = D3DX `QuaternionRotationMatrix`, statically linked at `0x767967`). Scale/shear in the file is dropped.
* **lerp (0x5B8840, `this`=dst)**: `dst.time = (1-t)*a.time + t*b.time`; `dst.rot = Slerp(a.rot, b.rot, t)` (`0x60C8B0 -> 0x767EEB`, D3DX `QuaternionSlerp`); `dst.pos = (1-t)*a.pos + t*b.pos`. The `1.0` is `_DAT_009CF784`.
* **to_matrix (0x5B8910)**: `M = RotationQuaternion(rot)` (`0x60C880 -> 0x767101`) then `matrix_multiply_translation(M, pos)` (sets row 3 = pos; v3d export).
* Keys are stored **parent-relative**: `0x5B6B30` replaces `K_abs(child)` by `M_child * inverse(M_parent_at_same_time)` where the parent key is interpolated (lerp, `t` snapped to the nearer end when within epsilon of 0 / 1, epsilons `_DAT_009CF5C8` / `_DAT_009D56D0`, values not exported) at the child key time.

## Track — 8 bytes (array at `AnimDef+0x140`, one per bone, stride 8, MSVC `new[]` cookie before element 0)

| offset | size | type | name |
|---|---|---|---|
| 0 | 4 | i32 | `n_keys` |
| 4 | 4 | AnimKey* | `keys` (realloc'd by 0x5B7BF0 grow) |

## Clip — 65 bytes in memory (`AnimDef+0x138`, stride 0x41), 62 on disk

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 50 | char[50] | `name` | |
| 0x32 | 1 | u8 | 0 | forced terminator |
| 0x34 | 4 | i32 | `flags?` | on-disk i32 (0 in samples) |
| 0x38 | 4 | f32 | `start` | ms |
| 0x3C | 4 | f32 | `end` | ms |
| 0x40 | 1 | u8 | `loaded` | 0 until 0x5B6DC0 loaded this clip's keys (set to 1 in the full loader 0x5B4B90) |
