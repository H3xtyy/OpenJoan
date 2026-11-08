# `CamTrack` / `CamTrackKey` / `CamRes` — camera track records (`camres.cpp`)

Source: loader `0x428E90` (record index/type), per-track reader `0x429110`, array header `camres::fn_00429980`
**0x429980**, free `camres::fn_00428d50` **0x428D50**, indexer `0x428B30`. Format: [../../formats/cm.md](../../formats/cm.md).
The record is **byte-packed** (`#pragma pack(1)`: u16 at +0, int at +2, floats at odd offsets). Read it
normal struct; do not copy the layout.

## CamRes (container, `this` of 0x429980 / 0x428D50)
| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | `CamTrack*` | tracks | array of `count` records, allocated by tracked realloc 0x58EB30 as `count*0xACA` (file `camres.cpp`) | 0x429980, verified |
| 0x04 | 4 | int | count | `size:N` from `CreateCameraTrackArray`; 0 => nothing is read | 0x429980, 0x428E90 verified |
| 0x08 | 1 | u8 | loaded | 1 when allocated; destructor 0x428D50 frees only if 1, then zeroes tracks/count/flag | 0x428D50 read |

Indexing is 1-based: `0x428B30(i) = tracks - 0xACA + i*0xACA` (i is a `short`).
The array is zero-filled (`memset`) on allocation. The loader sets `record.index = running 1-based counter`.

## CamTrack (sizeof = 0xACA = 2762)
| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x000 | 2 | u16 | index | 1-based position in the array | verified (0x428E90) |
| 0x002 | 4 | int | animated | 0 when the block keyword was `CreateCameraTrack` (static), 1 for `CreateCameraAnimatedTrack` | verified |
| 0x006 | 33 | char[33] | track_name | only read when animated==1 (second string); trailing char of the raw line (the `;`) overwritten by NUL | read |
| 0x027 | 33 | char[33] | map_name | first string (`AnyMap` / `sample`) | read |
| 0x048 | 12 | float[3] | zone_min | **static ("rail") form only**: AABB min x+off_x, y+off_y, z. 0x429BB0 tests the followed unit against this box with 0x60D5F0 (min<=p<=max) | verified (0x429BB0/0x60D5F0) |
| 0x054 | 12 | float[3] | zone_max | AABB max | verified |
| 0x060 | 2 | i16 | rail_dist | desired distance unit-to-camera along the rail | verified (0x426450, 0x4271F0) |
| 0x062 | 4 | float | fixed_yaw_deg | used when `use_key_angles`==0 | verified |
| 0x066 | 4 | float | fixed_pitch_deg | used when `use_key_angles`==0 | verified |
| 0x06A | 2 | i16 | use_key_angles | 1 = interpolate per-key `h_dir`/`v_dir` instead of the fixed values | verified |
| 0x06C | 2 | i16 | pitch_sign_mode | pitch mode (see engine page) | read |
| 0x06E | 2 | i16 | yaw_from_path | 1 = yaw = atan2 of rail tangent, else yaw = deg2rad(yaw) | verified |
| 0x070 | 2 | i16 | pitch_from_path | 1 = pitch from rail tangent (asin(-dir.z)) | verified |
| 0x072 | 4 | int | key_count | `track_point_count` (atoi) | verified |
| 0x076 | 60x0x2C | CamTrackKey[60] | keys | 0xAC6-0x76 = 0xA50 = 60 * 44, so **at most 60 keys** (no bounds check in the loader) | verified (layout), the 60 limit is inferred from the record size |
| 0xAC6 | 4 | float | speed_scale | derived at load; for rails it is the **parameter step `dt`** of the Hermite scan/follow (0x4271F0, 0x426450) | verified |

`off_x`/`off_y` are the two float parameters passed down from 0x428E90's caller (`param_3`, `param_4`); they are added to
the file's first two coordinates (x and y) of every key and of the static from/to points. Origin unknown: TODO(re)
caller of 0x428E90 (probably in `world_s.cpp` 0x4F2F80 -> world origin shift).

## CamTrackKey (sizeof = 0x2C = 44, relative to key start 0x76 + i*0x2C)
| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 4 | float | x | file `x` + off_x | verified |
| 0x04 | 4 | float | y | file `y` + off_y | verified |
| 0x08 | 4 | float | z | file `z` | verified |
| 0x0C | 12 | float[3] | tangent | Hermite tangent computed at load (see engine page) | verified |
| 0x18 | 4 | float | h_dir | file `h_dir` (degrees; no conversion at load) | read |
| 0x1C | 4 | float | v_dir | file `v_dir` | read |
| 0x20 | 4 | float | rot_a | animated only | read |
| 0x24 | 4 | float | cam_dist | animated only | read |
| 0x28 | 4 | int | frame_time | animated only, ms from start (per cm.md); read FIRST in file order | read |

File order per key (animated): frame_time, x, y, z, h_dir, v_dir, rot_a, cam_dist. Static keys: x, y, z, h_dir, v_dir.
Offsets for rot_a/cam_dist/frame_time assume the cm.md field order matches the read order (it does: the reads at
+0x9e, 0x76, 0x7a, 0x7e, 0x8e, 0x92, 0x96, 0x9a are issued in exactly that sequence).
