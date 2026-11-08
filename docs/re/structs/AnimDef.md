# AnimDef — one animation set (`.am2`/`.amt` + `.am3`), 0x18B bytes (`operator_new(0x18b)`, ctor 0x5B2390)

Class `en3d::animdef` (`d:\ww\en3d\animdef.cpp`). Fields seen via `this+off`:

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x000 | 1 | u8 | `state` | 0 empty, 1 header only (clip table loaded), 2 fully loaded | 0x5B23D0, 0x5B2F70 |
| 0x001 | 1 | u8 | `step` | async loader state: 0 idle, 1 start, 3/4/0x0D/0x0E reading AM2/AM3/AMT steps | 0x5B3100 (0x5B4140 = `is_idle`) |
| 0x006 | 256 | char[256] | `dir` | e.g. `ANIM\` | 0x5B2F70 (`< 0x100`) |
| 0x107 | 32 | char[32] | `name` | set name without extension (`< 0x20`) | |
| 0x128 | 4 | i32 | `n_tracks` | | 0x5B5830 |
| 0x12C | 4 | i32 | `n_clips` | | |
| 0x130 | 4 | f32 | `start` | | |
| 0x134 | 4 | f32 | `end` | | |
| 0x138 | 4 | Clip* | `clips` | [AnimKey](AnimKey.md) | |
| 0x13C | 4 | void* | ? | freed on unload; TODO(re) | 0x5B4170 |
| 0x140 | 4 | Track* | `tracks` | | |
| 0x144 | 4 | BoneTree* | `bones` | [BoneTree](BoneTree.md) | 0x5B69C0 |
| 0x148 | 4 | i32 | `relative` | tracks converted to parent-relative | 0x5B6A90 |
| 0x14C | 2 | i16 | `n_instances` | per-actor playback cursors released in 0x5B4170 (0x5B7650/0x417250) | |
| 0x14F | 4 | i32 | `n_planes` | trailer: planes with curves | 0x5B5350 |
| 0x153 | 4 | ptr | `colour_curves` | `n_planes` x {i32 n, f32[4]*} | |
| 0x157 | 4 | ptr | `alpha_curves` | `n_planes` x {i32 n, f32[2]*} | |
| 0x15B | 4 | ptr | `labels` | `{i32 n, Label*}` 8-byte header, 60-byte labels | |
| 0x17F | 4 | File* | `f_am2` | 0x164-byte FileMem/File handles used by the async loader | 0x5B3100 |
| 0x183 | 4 | File* | `f_am3` | | |
| 0x187 | 4 | File* | `f_amt` | | |

Added by the actor-family pass: `Clip` record = `clips + (clip_index-1)*0x41` (1-based clip ids everywhere, `0x5B5DD0`; `+0x38 start`, `+0x3C end`, `+0x40 loaded`); `+0x13C` = `int[n_clips]` clip order table (identity then qsorted by `0x5B5B70`, built by `0x5B5A70`, only while state==0);
`+0x148` set to 1 once all tracks are parent-relative (`0x5B6A90`); full pose evaluation: [actor.md](../functions/actor.md) `0x5B5E40`.

Resource wrapper (animres.cpp, 0x417540): entry has `AnimDef*` at +0, `dir` at +4 (0x100), `name` at +0x105, load-state byte at +0x126, ref-count at +0x128
(`0x417540 acquire` increments; first acquire creates the AnimDef and loads both files; `0x417B60 update` finishes async loads).
