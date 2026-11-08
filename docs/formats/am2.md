# `.am2` — skeletal animation set (clip table + per-bone keyframe tracks), 357 files, up to 4.7 MB

Reader: `en3d::animdef` (`d:\ww\en3d\animdef.cpp`). Three entry points on the same stream layout (joan.exe):

| mode | function | what it reads |
|---|---|---|
| header only | 0x5B5830 | header + clip table; **tracks are not loaded** (state byte = 1) |
| full | 0x5B4B90 | header + clip table + every key of every track + trailer (state 2) |
| one clip, lazy | 0x5B6DC0 `AnimDef::load_clip(clip)` | re-opens the file, skips the header/clip table, loads only the keys inside `[clip.start, clip.end]` (re-entrant: flag `clip+0x40`) |

Orchestration/async state machine: 0x5B3100 (step), 0x5B23D0 / 0x5B2AA0 (load), 0x5B2F70 (stat only). Path = `<dir><name>.AM2`
(dir/name pair comes from the `ANIMATE` table, see [../re/functions/anim.md](../re/functions/anim.md)). After a full load the matching
[`.am3`](am3.md) is parsed (0x5B6800) and, when present, all tracks are converted from absolute to parent-relative (below).
See also [amt.md](amt.md) (converted cache of the same data) and structs [AnimDef](../re/structs/AnimDef.md), [AnimKey](../re/structs/AnimKey.md).

Little-endian, packed, no magic/version. **Time unit: milliseconds at 30 fps** (every key and clip time is a multiple of 33.333; inferred
from the samples: clip `default` ends at 766.67 = 23 frames).

## File layout

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `n_tracks` | number of bone tracks = bones in the paired `.am3` (23 for humans) | 0x5B5830 stores to `AnimDef+0x128`; Alencon.am2 = 0x17 |
| 0x04 | 4 | i32 | `n_clips` | clip-table entries | `+0x12C`; Alencon.am2 = 0x53 = 83 |
| 0x08 | 4 | f32 | `start_time` | whole-set start (0) | `+0x130` |
| 0x0C | 4 | f32 | `end_time` | whole-set end; Alencon = 153883.0 (0x481646D6) | `+0x134` |
| 0x10 | 62 * n_clips | `ClipRec[]` | clip table | | 0x5B6DC0 skips `n_clips*0x3E` after the 16-byte header |
| 0x10 + 62*n_clips | variable | `TrackBlock[n_tracks]` | key data, in bone-index order | | 0x5B4B90 loop |
| after tracks | variable | trailer | zero or more sections, terminated by a 0 dword or EOF | see below | 0x5B5350 |

`ClipRec` (62 bytes, packed):

| rel. offset | size | type | name | meaning |
|---|---|---|---|---|
| 0 | 50 | char[50] | `name` | e.g. `default`, `f_run`, `f_walk`, `f_shift_left`, `f_jump_back`, `f_idle`, `f_atkA`... (mostly `f_`-prefixed in unit sets) |
| 50 | 4 | i32 | `flags?` | **0 in every one of the 357 files' records**; stored at in-memory clip+0x34. TODO(re): 0x5B4B90 stores it; consumer unknown |
| 54 | 4 | f32 | `start` | first time of the clip (ms) |
| 58 | 4 | f32 | `end` | last time of the clip (ms) |

In memory the table is 65-byte records (`name[50]`, 0, i32, f32, f32, u8 "loaded" flag at +0x40). Clips of one file are laid out one after
another on the common time axis with ~1000 ms gaps; all clips share the same tracks.

`TrackBlock` (track *i* belongs to bone *i* of the `.am3`):

| rel. offset | size | type | name | meaning |
|---|---|---|---|---|
| 0 | 4 | i32 | `n_keys` | number of keys |
| 4 | 52 * n_keys | `FileKey[]` | | strictly increasing `time` |

`FileKey` (52 bytes):

| rel. | size | type | meaning |
|---|---|---|---|
| 0 | 4 | f32 | `time` (ms) |
| 4 | 48 | f32[12] | **3x4 transform `[R|t]`, row-major for column vectors (verified)**; equivalent D3D-terms description: file dwords d0..d11 map to D3D (row-vector) matrix M as `M[0][0]=d0, M[1][0]=d1, M[2][0]=d2, M[3][0]=d3, M[0][1]=d4, M[1][1]=d5, M[2][1]=d6, M[3][1]=d7, M[0][2]=d8, M[1][2]=d9, M[2][2]=d10, M[3][2]=d11`; remaining column fixed (0,0,0,1). So `d3,d7,d11` = translation (M row 3), the other nine are the rotation/scale block, stored transposed vs D3D (i.e. a column-vector 3x4 `[R|t]` row-major). Read at 0x5B4B90 (reads 4 + 0x30 bytes, copies onto a default 4x4 from 0x00A18A78) and 0x5B6DC0 |

In memory a key is converted to `AnimKey` = {time, quaternion(x,y,z,w) from the matrix (0x60C840 = orthonormalise + D3DX-style
matrix->quaternion, scale/shear is dropped), position(x,y,z) = matrix row 3 (0x60C310)} = 32 bytes (see [AnimKey](../re/structs/AnimKey.md)).
The `.amt` cache stores these 32-byte keys directly, which fixes the `AnimKey` order (time, qx, qy, qz, qw, px, py, pz; verified on bigred.AMT,
w = 1.0 is the 4th float after the time).

### Lazy per-clip load (RE: 0x5B6DC0)
For clip *c* (loaded flag `clip+0x40` == 0): reopen `<dir><name>.AM2`, skip 16 + 62*n_clips bytes, then for every track read `n_keys` and each
52-byte record; keys with `time < clip.start` are skipped, the first key with `time > clip.end` skips the rest of the track
(`(remaining)*0x34` bytes) and stops; the rest are appended to the track (`AnimDef::grow_keys`, 0x5B7BF0), then — if the appended block's first
time is smaller than existing keys — rotated into sorted position. Set flag. (The conversion to parent-relative below is done by the caller
sequence for the whole animation, 0x5B6A90; with lazy mode the relative flag at `+0x148` is set when the converted data is complete.)

### Absolute -> parent-relative conversion (RE: 0x5B6A90 / 0x5B6B30)
`.am2` keys are **model-space (absolute) bone transforms** as written by 3ds max/Character Studio. After the `.am3` is loaded
`0x5B6A90` runs once (`AnimDef+0x148` = 1): for each root bone (parent -1) it calls `0x5B6B30(bone)` which first recurses into all children, then, for a
bone with parent *p*: for every key *k* of the bone, interpolate the parent's still-absolute track at `k.time`
(lerp of time/position, slerp-like quaternion blend via 0x60C8B0 / 0x767EEB; `t <= eps` -> take key A, `t >= 1-eps` -> take key B), build
matrices (0x5B8910 key->matrix), compute `local = child_abs * inverse(parent_abs)` (row-vector convention) and store it back
(0x5B8770: matrix -> quaternion + translation). Post-order guarantees the parent is converted last.

### Key reducer (tool-side, 0x5B83A0)
Dead-key remover used by the cache writer: drops a key when the interpolation of its neighbours (0x5B8840) reproduces it
within 0.01 (rotation/position components) / 5e-6, and keeps a key anyway when it is >= 250.1 ms after the last kept key.
This explains the `.amt` files being much shorter than the matching `.am2`.

## Trailer sections (RE: 0x5B5350 read, 0x5B4FE0 write) — per-**plane** material curves and event labels

After the last track the reader loops `while ((id = read_i32()) != 0)`; only `id == 1` is understood (other ids are silently skipped one dword at a time).

Section 1:

| field | type | meaning |
|---|---|---|
| `n_planes` | i32 | stored at `AnimDef+0x14F` |
| per plane `i` in `0..n_planes-1`: | | **index = mesh plane/material index of the model** (0x5B5E40 uses `model+0x6C + i*0x54`), not a bone |
| `n_col` | i32 | colour keys; then `n_col` x 16 bytes (4 raw dwords; read as `float time, r, g, b`; sampled by 0x5B8B40/0x5B8960 -> 3x ftol into a 0xAARRGGBB byte colour) |
| `n_alpha` | i32 | opacity keys; then `n_alpha` x 8 bytes (`float time, float opacity` 0..1; sampled 0x5B91C0/0x5B9020, ftol -> alpha byte, also forces blend mode) |
| `n_labels` | i32 | after all planes; if > 0: `n_labels` x 59 bytes `{u32 time, i32 id (-1 -> -999), char name[50], u8 flag}` (in memory 0x3C-byte records) — animation event labels |

Then the loop continues (next section id or 0). Stream ends with a `0` dword (the writer 0x5B4FE0 always emits `1, n_planes, ..., 0`; if there are no curves
and no labels it writes only the terminating 0).
TODO(re): colour key field order (time,r,g,b) inferred from the writer/reader byte counts (4x4) - no shipped file has one; alpha key order verified on the 2 files with keys; label `flag`/`id` consumers (0x5C5610?) not traced.

## Worked example — `anim/Alencon.am2` (4,343,298 bytes)

```
0x00  17 00 00 00   n_tracks = 23
0x04  53 00 00 00   n_clips  = 83
0x08  00 00 00 00   start    = 0.0
0x0C  d6 46 16 48   end      = 153883.0
0x10  "default"     ClipRec[0].name (50 bytes, to 0x41)
0x42  00 00 00 00   flags? = 0
0x46  00 00 00 00   start = 0.0
0x4A  ab aa 3f 44   end   = 766.667 (23 frames)
0x4E  "f_run"       ClipRec[1]; flags @0x80 = 0; start @0x84 = 1766.67; end @0x88 = 2566.67
0x8C  "f_walk"      ClipRec[2]; start @0xC2 = 3566.67; end @0xC6 = 4733.33
...   record i at 0x10 + 62*i; 83 records end at 0x10+62*83 = 0x142A where TrackBlock[0] starts
```
Verified: record stride 62 (0x4E-0x10 = 0x3E), float fields at +54/+58, names at +0. The clip table ends at 0x142A where `TrackBlock[0]` starts
(dumped, see below); the trailer was dumped too.

### First track block of Alencon.am2 (verified by dump)

```
0x142A  f6 0d 00 00          n_keys = 3574 (track 0 = Bip01)
0x142E  00 00 00 00          key0.time = 0.0
0x1432  00 00 b8 35  ff ff 7f 3f  00 00 00 80  00 00 00 00      d0..d3  = 1.37e-6, 1.0, -0.0, 0.0
0x1442  ff ff 7f bf  00 00 b8 35  00 00 00 80  bc 0f 1c b2      d4..d7  = -1.0, 1.37e-6, -0.0, -9.2e-9
0x1452  00 00 00 00  00 00 00 00  00 00 80 3f  52 05 2e 42      d8..d11 = 0, 0, 1.0, 43.50
0x1462  00 00 c8 42 ...      key1.time = 100.0 (key stride 52: 0x1462 = 0x142E + 52)
```
Key times of this file: 0, 100, 133.33, 166.67, 200 ... (multiples of 33.333 ms after the first step).

### Matrix orientation (proven)
Rows `{d0 d1 d2 d3}`, `{d4 d5 d6 d7}`, `{d8 d9 d10 d11}` form `[R|t]`: the 3x3 block `R` is a proper rotation (every row and column unit length, det = +1.0000006; worst
orthonormality error < 1e-3 in all 23-bone human sets; 31 of the 357 files carry scale in some key - row norms up to ~3.5x, e.g. `item/anim/Joan_AAAA.AM2`, `anim/cannon_explosion.am2`, `anim/Flavy.am2`, `anim/lordscales.am2` - which the engine drops when it builds the quaternion), key 0 of `Bip01` is the Z-up 3ds max pose "rotate +90 deg about Z" with `t = (d3,d7,d11) = (0, ~0, 43.5)` = pelvis height. The
convention is decided by the skeleton: for a child bone *c* of parent *p* in the same key, `R_p^T * (t_c - t_p)` (R columns = bone axes, `world = R*local + t`) is
constant over time (mean per-axis spread 0.27 over 22 parent/child pairs of Alencon), while the transposed reading `R_p * (t_c - t_p)` varies (6.99). Hence **row-major 3x4,
column-vector convention**; `d9,d10,d11` is *not* a translation (it would read (0, 1, 43.5)). 

### Trailer of Alencon.am2 (verified by dump), last 48 bytes at 0x4245D2
```
01 00 00 00   section id 1
04 00 00 00   n_planes = 4
 4 x ( 00 00 00 00 n_colour   00 00 00 00 n_alpha )   = 32 zero bytes
00 00 00 00   n_labels = 0
00 00 00 00   terminator
```
(file ends at 0x424602 = 4,343,298.) The last key of the last track has time 153883.0 = header `end_time`.

### Install facts (all 357 files)
- Every file follows the layout; header-implied size (16 + 62*clips + sum(4 + 52*keys) + trailer) equals the file size; the trailer is simply "the rest".
- Key times strictly increasing in every track; `end_time` equals the maximum last-key time (within 0.5 ms); every clip has `start <= end <= end_time`; clip `flag` is 0 in all records.
- Trailer shapes seen: *absent* (0 bytes, 84 files: stream ends after the last track, no terminator), bare terminator `00000000` (76), section 1 + terminator, and 6 files whose
  section 1 is **not** followed by a terminator (EOF right after `n_labels`). Planes 1-6 per file; **no colour keys anywhere**; opacity keys only in 2 files (4 keys, 1 plane); labels 0 or 1 per file.
  Section-1 sizes: 24 B (1 plane, no label) x56, 83 B (1 plane, 1 label) x93, 91 B x12, 40 B x11, 32 B x11, 48 B x6 ...
- Label records seen: `{time = 0, id = 0, name = "<x>_heal.txt" | "exparrowfire.txt" ..., flag 0/1/4}` - the names are **particle-script (`pscript`) file names** played when the animation runs
  (`weapon_heal.txt` x36, `enhancer_heal.txt` x22, `qtm_heal.txt` x7, `food_heal.txt` x6, ...). The `time`/`id` of every shipped label is 0.
- Sibling `.am3`: 355 `.am2` have an `.am3` with the same base name in the same directory; `n_tracks` equals the `.am3` entry count in all 355.

Inventory heads: `01 00 00 00 01 00 00 00 ...` x107 (one track, one clip: props), `03 00 00 00 01 ...` x16, `17 00 00 00 06 ...` x14
(23 tracks, 6 clips). `eng_flag.am2`: `01 00 00 00 | 01 00 00 00 | 00 00 00 00 | 55 55 b7 43 | "default"` = 1 track, 1 clip, 0..366.667 ms.

## Skeleton and rest pose

- **Skeleton.** Same directory and base name `.am3`, then `unit/<base>.am3`, then `anim/<base>.am3` (355 of 357 `.am2` resolve;
  `anim/ripple.am2` and `unit/lahire_armorb#2.am2` have none). The bones are the `.am3` forest (bone index = track index). The rest pose is the key at
  the start of the `default` clip (clip 0 if absent).
- **Absolute -> parent-relative**: the original pass 0x5B6A90/0x5B6B30 (verified against the pseudocode) computes `child * inverse(parent)` in D3D
  row-vector form, with the parent sampled at the *child's key times* (segment walk, exact hit or end key when the fraction is within 1e-6 of an
  endpoint, otherwise lerp of translation/scale and slerp of the rotation). Roots keep the absolute transform. Verified on `Alencon.am2`: after the
  conversion the translations of the rigid links (forearm, hand, calf, foot, spine segments, neck, head) have a standard deviation < 1e-2 units over
  all 83 clips (e.g. `Bip01 L Forearm` = (13.128, 0, 0) always); only the animated bones (root, pelvis-adjacent thighs, clavicles, `Bip01 Spine`) vary.
- **Scale.** The original drops scale (matrix -> quaternion). 35 of the 357 files carry scale in some key (the 31 listed above plus 4 with small
  deviations from 1 beyond 1e-3).
- **Time.** File times are milliseconds; keys land on multiples of 33.333 ms (30 fps). A clip window is `start <= t <= end`; the lazy loader 0x5B6DC0
  loads only the keys inside it.
- **Axes.** Z-up, D3D left-handed data ([am1.md](am1.md)): the character lies on its back in a Y-up viewer.
- **Sizes.** `anim/bur_lightinfantry_share.am2` is 7.2 MB, `Alencon.am2` 4.3 MB (153,883 ms).

## Name lookup by scripts
Scripts and unit definitions never open an `.am2` by path: `RESOURCE\ANIM_AM2.RES` and `RESOURCE\ANIM_AM3.RES` hold the `ANIMATE` table
(0x4172A0: 58-byte records `{i16 id (1-based), char name[32] at +2, char key[8] at +0x23}`; `key` is parsed by 0x590170 and registered
by 0x414970) and `RESOURCE\animscr.res` (0x418F30, 539-byte records, one text-parsed record per animation id) gives the per-id clip names/events.
The animation handle (0x417540 `animres_acquire`, 0x417B60 `animres_update`) keeps a ref-count at +0x128 and builds the `.AM2`/`.AM3` path from
`<dir>` (record+4) and `<name>` (record+0x105). A clip is addressed by name through the `ClipRec` table (`strcmp` on `name`).
TODO(re): exact clip-name->index function (0x5B5DD0 is called with (anim, clip name) by 0x5B5E40), `animscr.res` record fields, `zCripple` use (no file named
`zCripple.*` exists in the install; the literal at 0x9DD6D2 has no code xref in the analysed set).

## Confidence
Header, clip table: **verified** (hex + code). Key layout 52 bytes: **verified by code** (two reader sites + skip arithmetic 0x34), matrix orientation
**verified** (skeleton-consistency test above, orthonormal rotation block, plausible translation). Trailer: **verified** on all 357 files (section 1, planes, labels; colour-key field
order remains unobserved since no file has colour keys).

## Further notes

- One file per unit holds every action on a single timeline. `Joan` (the hero's set) has 86 actions over 167 s and 90,672 keys (4.7 MB).
- Action names in the clip table may be padded with `0xCC` bytes after the NUL.
- Keys are sparse (about 30 per second where a bone moves, fewer where it does not). A pose is built by finding, per bone, the two keys around
  `action.start_ms + t` and interpolating (slerp for rotation, lerp for translation). The matrices are absolute model-space transforms,
  so the hierarchy ([am3.md](am3.md)) is not needed to pose a mesh; it matters for attaching items and for blending actions.
- A pseudo-action `default` spans the whole timeline; typical names are `f_run`, `f_walk`, `f_idle`, `f_atkA`, `c_up_std_a`.
- The trailer after the tracks is variable (see "Trailer sections"): a fixed 40-byte trailer (as in `joan_of_arc.am2`)
  is one instance of the shapes tabled there, not a rule.
