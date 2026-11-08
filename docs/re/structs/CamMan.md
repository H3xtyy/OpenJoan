# `CamParams` / `CamMan` / `CamCtl` — runtime camera (`camman.cpp`)

Source: joan.exe, export `decomp/engine/camera2/`. Allocations cite `d:\ww\camman.cpp` (0x423600, 0x425720).
Status: **verified** = same offset read consistently in >= 2 functions; **read** = one function; **inferred**.
Coordinate system: **Z is up** (the camera height is index 3 / `z`; terrain clamp raises `z`).

## CamParams (sizeof = 0x28 = 10 dwords, "orbit form")
Copied by `rep movs` x10 (0x423F40, 0x423FB0, 0x423FE0). The camera orbits a focus point.
| idx | offset | type | name | meaning | status |
|---|---|---|---|---|---|
| 0 | 0x00 | int | proj_type | **0 = perspective, 1 = orthographic** (read as `[0xA0]` of the scene camera by 0x6141A0); never interpolated by 0x4237A0 | verified |
| 1-3 | 0x04 | float[3] | focus | focus/look-at point x,y,z (track key x,y,z) | verified |
| 4 | 0x10 | float | yaw | radians, wrapped to [-pi, pi]; = `h_dir`*deg2rad in tracks | verified |
| 5 | 0x14 | float | pitch | radians; `v_dir`*deg2rad | verified |
| 6 | 0x18 | float | roll | radians; `rot_a`*deg2rad | read |
| 7 | 0x1C | float | dist | eye distance from focus (`cam_dist`); 0 for rail cameras (eye == focus) | verified |
| 8 | 0x20 | float | fov_param | perspective: passed through `FUN_007B2184` to the projection builder (default 1.0, see engine page) | verified (use), formula partly TODO(re) |
| 9 | 0x24 | float | ortho_scale | orthographic: multiplies viewport w,h (default 1.0) | verified |
Default block (0x616D50, "reset camera"): all zero, `dist = 100.0` (0x42C80000), idx 8 and 9 = 1.0.

Eye position from orbit form (0x423290, `dist` 0 is replaced by 1.0):
`eye = focus + dist * (-cos(pitch)*cos(yaw), -cos(pitch)*sin(yaw), +sin(pitch))`; the converted "eye form" is
`[0]=tag, [1..3]=eye, [4..6]=focus, [7]=roll, [8],[9]=extra`. The inverse (0x423400) gives
`yaw = atan2(focus.y-eye.y, focus.x-eye.x)`, `pitch = atan2(eye.z-focus.z, horizontal distance)`, `dist = |eye-focus|`.
(`FUN_007b24b4` = cos and `FUN_007b2404` = sin follow from this identity; both are CRT wrappers, 0x425670/0x4256A0.)

## CamMan (one object per camera mode; sizeof >= 0xC8, tracked allocation sites only for the fade nodes)
| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 0x28 | CamParams | from | start of the current transition (copy of `cur` when a new target is set) | verified (0x423F40) |
| 0x28 | 0x28 | CamParams | to | transition target | verified |
| 0x50 | 0x28 | CamParams | cur | interpolated result, consumed by the renderer (0x424010 reads focus +0x54..5C, yaw +0x60, pitch +0x64, roll +0x68, dist +0x6C) | verified |
| 0x78 | 4 | int | blend_mode | 1 = jump (progress=1 at once), 2 = linear over `+0x7C` ms, 3 = trapezoid ease, 4 = user callback `float cb(elapsed_ms, &+0x7C)` | verified (0x4237A0) |
| 0x7C | 4 | uint / fn* | duration_ms | transition length (modes 2, 3) or callback pointer (mode 4) | verified |
| 0x80 | 4 | float | accel_frac | mode 3: fraction of the duration spent accelerating | read |
| 0x84 | 4 | float | decel_frac | mode 3: fraction decelerating | read |
| 0x88 | 4 | int | interp_mode | 1 = built-in per-field interpolation, 2 = user callback `cb(&cur,&from,&to,progress,&+0x8C)` | read |
| 0x8C | 4 | fn* | interp_cb | | read |
| 0x98 | 4 | float | progress | 0..1; **1.0 = idle** (0x4237A0 returns 0 immediately); set to 0 by 0x423F40 | verified |
| 0x9C | 4 | uint | start_ms | `timeGetTime` wrapper 0x591050 at the moment the target was set | verified |
| 0xA4 | 4 | int | no_collision | 0x423E70 (reset) sets 1; when 0 the collision pass 0x424010 runs after each update | read |
| 0xA8,0xAC,0xB0 | 3x4 | float | hit_terrain_cfg | tuning floats used when the sight line hit terrain (see engine page) | read, values TODO(re) |
| 0xB4,0xB8,0xBC | 3x4 | float | hit_object_cfg | same when only objects were hit | read, values TODO(re) |
| 0xC0 | 4 | `FadeNode*` | fade_list | doubly linked list of objects currently faded because they occlude the focus; a sentinel tail node is always present | verified |
| 0xC4 | 4 | int | fade_count | | verified |

Constructor 0x423600: reset (blend=1, interp=1, no_collision=1), progress = 1.0, allocates the first sentinel node.
Destructor 0x4236D0 frees the list. 0x423F40 `set_target(CamParams*)`: `from = cur; to = *p; progress = 0; start = now`.
0x423EC0 `set_from_to(f,t)`: `from = cur = *f; to = *t; progress = 0; start = now`.
Instance selection: `FUN_00563C80(mode)` returns the CamMan for a mode index (0..2 seen); `FUN_00563DC0` returns the active mode.

### FadeNode (sizeof = 0x11 = 17, packed)
| offset | type | meaning |
|---|---|---|
| 0x00 | `Actor*` | object being faded |
| 0x04 | `FadeNode*` | prev |
| 0x08 | `FadeNode*` | next (NULL = end) |
| 0x0C | u8 | seen-this-frame mark (0x4258A0 clears all; 0x425720 / 0x425840 set) |
| 0x0D | int | object's `short` id (`*(short*)(obj->def + 4)`), used later to find the object and restore it with `FUN_005AE590(0)` |

0x4258F0 deletes nodes whose mark is still 0 after the pass (object no longer occluding) and restores them.

## CamCtl (unit-follow camera controller; `this` of 0x425FC0, ctor 0x425E20, reload 0x426330)
| offset | type | name | meaning | status |
|---|---|---|---|---|
| 0x00 | int | unit | recno of the followed unit | read |
| 0x04 | int | active | nonzero while a mode object exists (0x429860) | read |
| 0x06,0x0A,0x0E | float | origin x,y,(z) | map origin passed to the `.cm` loader (x,y used; z ignored by the loader) | read |
| 0x12 | `CamRes*` | tracks | `new` of 9 bytes (CamRes is `{tracks*, count, loaded}` packed = 9 bytes, see [CamTrack](CamTrack.md)) | verified |
| 0x16 | short | enabled | 0 => no auto camera | read |
| 0x18 | int | cur_track | 1-based index of the static or animated track in use, 0 = none | verified |
| 0x28 | float[3] | last_pos | unit position used the last time | read |
| 0x2F | int | mode | camera mode 0..3 (0x429860 transitions) | read |
| 0x33 | int | prev_mode | restored by 0x4297D0 | read |
| 0x34 | float | z_offset | default 50.0 (0x42480000, set by ctor 0x425E20): added to the unit height | verified |
| 0x38 | uint | anim_start | animated-track start (game ms, `DAT_00A500BA`) | read |
| 0x3C | uint | anim_end | | read |
| 0x40 | int | anim_offset | ms already played / skip | read |
| 0x44 | short | loop | animated-track loop flag | read |
| 0x46 | float | unknown_1.0 | default 1.0 | TODO(re) |
| 0x4A | short | restore_target | on stop re-target the saved params | read |
(Offsets 0x2F..0x4A are byte offsets from `param_1` typed `int` in 0x429860/0x427D90, i.e. the struct is packed.)
