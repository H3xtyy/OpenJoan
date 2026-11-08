# Morph (0x84 bytes, `d:\ww\en3d\morph.cpp`) and MorphTarget (0x44 bytes)

One `Morph` per `.MDA` file attached to an [Actor](Actor.md) (`Actor+0xB4[]`). File format: [mda.md](../../formats/mda.md). Constructor = memset 0 (`0x5C67B0`).
Packed. Created by `Actor::add_morph` (`0x5ADF50` -> `Morph::load` `0x5C6820`) or `Actor::add_morph_copy` (`0x5AE0C0` -> copy ctor `0x5C7370`).

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `loaded` | set 1 at the end of load/copy; all loaders are no-ops if already 1; cleared by free `0x5C7B80` | verified |
| 0x01 | <=0x33 | char[] | `name` | `strcpy` of the model name argument (`param_2`) | read |
| 0x34 | 4 | i32 | `n_targets` | named morph targets | verified |
| 0x38 | 4 | MorphTarget* | `targets` | `new[n_targets*0x44]` | verified |
| 0x3C | 4 | i32 | `vertex_base` | running total of part vertex counts (global vertex index base of the next part) | verified |
| 0x40 | 4 | i32 | `n_affected` | vertices that can move (sum over parts) | verified |
| 0x44 | 4 | i32* | `affected` | `int[n_affected]` global vertex indices (strictly ascending) | verified |
| 0x48 | 4 | u32 | `t_start` | clock (`DAT_00A500BA`) at which the current transition started; `set_target` / weight-curve apply set it to "now" | verified (0x5C8960, 0x5C7FF0, 0x5C9120) |
| 0x4C | 4 | u32 | `t_end` | clock at which the transition ends (= now when nothing is pending) | verified |
| 0x50 | 4 | i32 | `duration` | transition length in ms (`t_end - t_start`); 0 when idle. The writer that arms a timed transition (sets 0x48/0x4C/0x50, 0x60/0x64, 0x70=0, 0x74) is game-side, not exported | verified (read) |
| 0x54 | 4 | u32 | `last_apply` | clock of the last `Morph::apply`; apply is a no-op if equal to now | verified (0x5C8960) |
| 0x58 | 4 | Vec3* | `cur_pos` | `n_affected x 12`, initialised from the target's absolute position array (`+0x34`) by `set_target` | verified |
| 0x5C | 4 | Vec3* | `cur_nrm` | same for normals (`target+0x3C`) | verified |
| 0x60 | 4 | Vec3* | `pos_step` | total position change to apply over `duration` (target - current at the start of the transition); integrated incrementally by apply | verified (0x5C8960) |
| 0x64 | 4 | Vec3* | `nrm_step` | same for normals | verified |
| 0x68 | 4 | Vec3* | `base_pos` | rest-pose positions of the affected vertices | verified |
| 0x6C | 4 | Vec3* | `base_nrm` | rest-pose normals | verified |
| 0x70 | 4 | i32 | `cur_target` | **1-based** index of the active (settled) target; **0 = in transition / free-form** (set to 0 by the weight-curve path and by transitions); `set_target(name)` early-outs when unchanged | verified |
| 0x74 | 4 | i32 | `pending_target` | target (1-based) that becomes `cur_target` when the transition ends | verified (0x5C8960) |
| 0x78 | 1 | u8 | `has_curve` | 1 when a [MorphCurve](#morphcurve) was copied | read |
| 0x7C | 4 | i32 | `curve_t0` | clock at which the curve was installed (`DAT_00A500BA`) | read |
| 0x80 | 4 | MorphCurve* | `curve` | 0x10-byte object | read |

All positions/normals are optionally pre-transformed into model space: the loaders take a `D3DMATRIX*`; if non-null every position goes through `point_transform` and every normal/delta through `vector_transform` (v3d exports).

## MorphTarget (0x44)

| offset | size | name | meaning |
|---|---|---|---|
| 0x00 | 0x34 | `name` | 50-char name read by `FileMem::read_string(buf,50)` |
| 0x34 | 4 | `pos` | `Vec3[n_affected]` absolute target position (file `v0`) |
| 0x38 | 4 | `pos_delta` | `Vec3[n_affected]` = `pos - base` (file `v2`) |
| 0x3C | 4 | `nrm` | `Vec3[n_affected]` absolute target normal (file `v1`) |
| 0x40 | 4 | `nrm_delta` | `Vec3[n_affected]` normal delta (file `v3`) |

Lookup by name: `0x5C90A0` (linear scan, CRT string compare 0x7C07B0 (case sensitivity unverified), returns 1-based index or 0). `load_append` (`0x5C9B10`, `0x5CA5D0` copy-append) merges by target name and extends every target array to the new `n_affected`.

## MorphCurve (0x10 bytes) <a name="morphcurve"></a>

`{ u32 inited; i32 n_entries; Entry* entries; f32 duration }` - ctor `0x5CAE20` (memset), copy `0x5CB1D0(src, owner)`: `n_entries = owner.n_targets`, `duration = src.duration` (+0xC), each entry is
`{ i32 n; Pair* p }` with `n` 8-byte pairs `{f32 time_ms, f32 weight}` deep-copied. One curve per target: weight-over-time (facial animation).

Sampling (`0x5CB3D0`, this = curve): time `c = now - Morph.curve_t0`; looping: `c = fmod(c, duration)` when `c >= duration` (the apply passes loop=1). Per target: no pair with `time <= c` -> weight 0; `c` at/after the last pair -> last weight; else linear interpolation between pair *k* and *k+1*. Then `0x5C9120(weights)` builds the pose: for every affected vertex `cur_pos = base_pos + sum_i w_i * target_i.pos_delta`, `cur_nrm = base_nrm + sum_i w_i * target_i.nrm_delta` (targets with `w == 0.0` skipped), sets `cur_target = 0`, `t_start = t_end = now`, `duration = 0`. (Normals are **not** renormalised here, see apply.) If `now - curve_t0 >= duration` and looping is off, the final pair of every target is used.
