# camera family (camres.cpp, trackres.cpp; camman.cpp NOT yet exported)

Reference build joan.exe. Export dir `$JOAN_WORK_DIR/decomp/engine/camera/` contains **only the loaders**:
the camera manager (`d:\ww\camman.cpp`, listed in docs/re/MODULES.md) has no exported function, so modes, projection,
collision and input are **unknown** here; see Open questions. Structs: [CamTrack](../structs/CamTrack.md),
[UnitTrack](../structs/UnitTrack.md). Formats: [cm](../../formats/cm.md), [trk](../../formats/trk.md), [pth](../../formats/pth.md).
Design notes: ../../engine/camera.md.

| address | proposed name | purpose | evidence | confidence |
|---|---|---|---|---|
| 0x428E90 | camres_load_cm | parses a `.cm` text stream: array header, then each `CreateCameraTrack`/`CreateCameraAnimatedTrack`; sets index and animated flag, calls 0x429110 | strings, record math | verified |
| 0x429110 | camres_read_track | reads one track body (name/map, static header or none, key count, per-key fields), then computes tangents and speed_scale | field offsets | verified |
| 0x429A60 | camres_calc_key_tangent | tangent from prev/cur/next positions with a tension argument (0 at the only call) | 0x429110 call | verified |
| 0x429980 | camres_read_array_header | skip 2 tokens, atoi size, alloc `size*0xACA`, memset 0 | camres.cpp string | verified |
| 0x428D50 | camres_free | if loaded: per-track no-op cleanup loop, free array, clear flag | camres.cpp string | verified |
| 0x428E30 | camres_destroy_tracks | loops i=1..count calling the indexer and an empty function (0x429F30 = `ret`) | trivial | verified |
| 0x428B30 | camres_get_track | `tracks + (i-1)*0xACA` (i is `short`, 1-based) | read | verified |
| 0x4EBB40 | trackres_load_trk | parses `.trk`: `CreateTrackArray`, then `CreateTrack` blocks; sets index, calls 0x4EBD50 | strings | verified |
| 0x4EC240 | trackres_read_array_header | as camres variant, record `0x52` | trackres.cpp | verified |
| 0x4EBD50 | trackres_read_track | reads names, point count, points (`0x54` each) and edges into adjacency pointer arrays | trackres.cpp, edge_start | verified |
| 0x60CC90 | vec3_sub | `a - b` into out, returns out | trivial | verified |
| 0x60CD60 | vec3_scale (?) | `(scalar, vec)` -> scaled vec3 (used by 0x429A60) | call shape | inferred |
| 0x60CC20 | vec3_add (?) | vec3 sum | call shape | inferred |
| 0x417D90 | identity_fastcall | returns its argument (used to pass the `FileTxt` handle/path) | trivial | verified |
| 0x5A1170 / 0x5A1440 | str_clear / str_set400 | clear a char buffer / `strncpy(dst,src,400)` + NUL: the 401-byte path buffer | read | verified |

Shared helpers used (documented elsewhere): 0x589550 FileTxt open, 0x589890 FileTxt close, 0x589C70 token reader,
0x589F10 read line into buffer (`n` = 0x40), 0x58EB30 tracked realloc, 0x58ED70 tracked free, 0x7B15CB atoi,
0x7B2110 atof, 0x7B2020 strcpy, 0x7C07B0 strcmp-like (0 = equal).

## Runtime camera (export `decomp/engine/camera2/`; structs: [CamMan](../structs/CamMan.md))
Skipped as not camera code (neighbours in 0x423000-0x426c00): 0x423010-0x423220 small helpers, 0x423d20/0x423db0/0x4254a0/0x425520/0x426280
(static destructors of UI/image objects), 0x4251b0 (object cleanup), 0x4255c0, 0x425a40, 0x4233c0 (other class), 0x425b60/0x425c50 (accessors).

| address | proposed name | purpose | evidence | confidence |
|---|---|---|---|---|
| 0x423600 | camman_init | ctor: reset modes, progress=1.0, alloc sentinel fade node (17 bytes, camman.cpp) | alloc site | verified |
| 0x4236D0 | camman_free_fade_list | frees the fade list | camman.cpp | verified |
| 0x423E70 | camman_reset_modes | blend=1, interp=1, no_collision=1 | read | verified |
| 0x423F40 | camman_set_target | `from=cur; to=*p; progress=0; start=now(ms)` | read | verified |
| 0x423EC0 | camman_set_from_to | explicit from/to (also writes cur) | read | verified |
| 0x423FB0 / 0x423FE0 | camman_get_cur / camman_get_to | copy 10 dwords out of +0x50 / +0x28 | read | verified |
| 0x4237A0 | camman_update | per-frame: progress by blend mode, wrap angles, interpolate cur, run collision | read | verified |
| 0x424010 | camman_collide | camera collision / occlusion pull-in + fade list | read | verified (structure), partly inferred (tuning) |
| 0x423290 / 0x423400 | camparams_to_eye / camparams_from_eye | orbit form <-> eye/target form | read | verified |
| 0x425720 / 0x425840 / 0x4258A0 / 0x4258F0 | fade_add / fade_mark / fade_clear_marks / fade_release_unmarked | occluder fade list | read | verified |
| 0x425AC0 / 0x425B90 / 0x425C80 | vec3_lerp / angle_lerp_shortest / lerp | interpolation primitives | read | verified |
| 0x425CB0 | camparams_equal | memcmp 0x28 bytes; true => treat transition as finished | read | verified |
| 0x4279B0 | hermite3 | cubic Hermite (P0,T0,P1,T1,t) per component | algebra | verified |
| 0x427BB0 | lerp3f | `a + (b-a)*t` | read | verified |
| 0x427D90 | camctl_play_animated_track | animated `.cm` track evaluation (time -> key segment -> Hermite pos + lerp angles/dist) + ground clamp | read | verified; elapsed-time expression TODO |
| 0x4271F0 | camctl_rail_acquire | static track: scan every segment in steps of speed_scale for the point whose distance to the unit best matches rail_dist | read | verified |
| 0x426450 | camctl_rail_follow | static track: walk forward/back along the rail (max 101 steps) to stay at rail_dist | read | verified |
| 0x425FC0 | camctl_update | per-frame unit-follow driver: animated track vs static-rail lookup | read | verified |
| 0x425E20 | camctl_init | allocs CamRes, z_offset=50.0, [0x46]=1.0 | read | verified |
| 0x426330 | camctl_reload_cm | frees and reloads CamRes via 0x428DE0 -> 0x428E90 | read | verified |
| 0x429860 / 0x4297D0 | camctl_set_mode / camctl_restore_mode | mode switch 0..3 and restore | read | inferred meaning |
| 0x429BB0 | camres_find_static_track | first track with `map_name == name` (strcmp), `animated==0`, and unit inside zone box; returns 1-based index | read | verified |
| 0x60D5F0 | aabb_contains_point | `min<=p<=max` per axis | read | verified |
| 0x60CED0 / 0x60CBC0 | vec3_equal / vec3_neg | | read | verified |
| 0x60BD00 | mat_from_euler | builds rotation matrix from (yaw, pitch, roll) times a fixed Z-up -> view basis matrix | read | inferred |
| 0x60C220 | mat_row2 | returns matrix row 2 (+0x20) = forward axis | read | verified |

### Round 4 (export `decomp/engine/camera3/`: callers of the camera manager; structs [SceneCamera](../structs/SceneCamera.md))
Skipped as not camera logic: 0x4352f0 (unit facing/turn), 0x43dab0/0x437b60/0x43d560/0x42bcd0/0x43f000/0x431790 (unit movement/animation that only read `CamMan.to`),
0x44ac60/0x447ab0/0x448340/0x445990 (picking / placement tests that read `CamMan.cur`), 0x42e1a0 (save-game), 0x451530/0x4404c0 (unit selection, only start the rts tracks).

| address | proposed name | purpose | evidence | confidence |
|---|---|---|---|---|
| 0x44C800 | game_update_camera_frame | per frame: run `camctl_update` for every flagged unit, mode 0 -> `camman_follow_default`, `camman_update(active)`, copy `cur` to the SceneCamera, derive the streaming/visibility AABB (+-`DAT_009CF7A4`/`7A0` margins) | read | verified |
| 0x563A20 | camman_follow_default | mode-0 default follow: focus = unit position, `z += DAT_009CFC18`, pitch = `DAT_00A0BBE8/EC`, dist = `DAT_00A0BBF0/F4` (two sets, picked by 0x430190), then `set_from_to` | read | verified (structure) |
| 0x438720 | camera_zoom_step | zoom: `dist -= DAT_00A500B6 * DAT_009CFC40` (arg 1 = in) or `+=` (other = out); `dist < DAT_009CFC38 -> 80.0`, `dist > DAT_009CFC1C -> 350.0`; writes the result to `DAT_00A0BBF0`/`F4` | read | verified (clamp values 80/350 are literals) |
| 0x562F40 | camera_debug_free_cam | **debug/dev free camera** (mode 3), key driven; see engine page | read | verified |
| 0x4384E0 | camman_aim_follow_mode0 | mode 0 while aiming: yaw from `0x577450` smoothing | read | inferred |
| 0x42C260 | camctl_pick_target | pick the visible unit nearest the camera forward ray (range `DAT_00A33758`) for lock-on | read | inferred |
| 0x42CB20 / 0x42C830 | camman_lock_on_start / camman_lock_on_update | mode 1 lock-on: `set_from_to` / `set_target` on mode-1 CamMan, aborts when target farther than `DAT_00A33758` or yaw/pitch error > `DAT_00A33754` | read | inferred |
| 0x42BA40 | camctl_fire_direction | stores camera forward (or direction to locked target + `DAT_009CF9CC`) into the unit (aim vector at unit+0x365) | read | inferred |
| 0x43FC90 | rts_enter_camera | enter RTS command mode: rewrites the `rts_play` template track from `rts_init`, then mode 4 `set_from_to`, `set_mode(2)`, plays `rts_play` | read | verified |
| 0x6CC6A0 (+0x6CB9E0, 0x6CAD50, 0x6CD720, 0x6CCF00) | cutscene_start_camera | builds `cut_scene_play` from `cut_scene_move` template + from/to blocks (modes 5 and 2) | read | verified |
| 0x717240 | minimap_click_camera | minimap click -> ray into terrain, back-off loop on `dist` (step `DAT_009CFCA0`) until placement valid, then `camman_update` | read | inferred |
| 0x4857C0 | camera_get_mode_params | returns the CamMan `to` block of mode `DAT_00A48CEF` (0,1,2,4,5) | read | verified |
| 0x614040 | scenecam_set_params | `prev = cur; cur = *p; dirty = 1` | read | verified |
| 0x6140A0 | scenecam_get_params | copy `cur` out | read | verified |
| 0x6141A0 | scenecam_rebuild | rebuild node matrix, view, projection, viewport, frustum | read | verified |
| 0x616460 | scenecam_frustum | 8 corners + 6 planes | read | verified |
| 0x616D50 | camparams_reset | zero block, dist = 100.0, extras 1.0 | read | verified |
| 0x616DD0 | scenecam_pan | move focus in the ground plane relative to yaw: `dx = a*sin(a0) + b*cos(a0)`, `dy = a*cos(a0) - b*sin(a0)` with `a0 = DAT_009CFBF8 - yaw` | read | verified |
| 0x7674A4 | mat_perspective_fov_lh | D3DXMatrixPerspectiveFovLH-equivalent (statically linked D3DX): see engine page | read | verified |
| 0x767723 | mat_ortho_lh | D3DXMatrixOrthoLH-equivalent | read | verified |

CRT wrappers: 0x7B24B4 cos, 0x7B2404 sin, 0x7B2240 atan2, 0x7B2344 sqrt, 0x7B1F6B fabs (verified by use); 0x7B2564 asin (pitch from direction) and 0x7B2184 acos (?) are inferred.

## Loader behaviour notes
* Both loaders are driven by a token loop: read token; if it equals `...Array` run the header reader and abort if size is 0;
  if it equals a track keyword, `++n`, stamp the index into record n and run the body reader. Anything else is skipped.
  Loop ends at end of file (token reader returns 0).
* Both body readers take two float offsets (caller args 2 and 3) added to the first two coordinates of every
  position (keys, static from/to, trk points). The z coordinate is not offset.
* Raw string fields are read as a line (max 0x40) and the last char is overwritten with NUL (drops `;`).

## Open questions
* Round 11 resolved (RE: joan.exe): `FUN_007B2184` = atan, `FUN_007B2564` = acos; 0x9E2FE8 = 1.0, 0x9E2FEC = 3.0, 0x9E2FF0 = 0.5, 0x9E2FF4 = 4.0, 0x9E2FF8 = 5.0, 0x9E2FFC = 2.0 => vertical FOV = atan(fov_param) = 45 deg by default; near 1.0, far 8193.0 / 16385.0 (ctor 0x613730); zoom clamp 80.0..350.0 at speed 0.6 (double 0x9CFC40); follow offset 50.0 (0x9CFC18); initial pitch 0.192 / 0.2618, dist 187.5. SetTransform consumer: 0x614760 (VIEW = SceneCamera+0x1EE, PROJECTION = +0x26E, SetViewport = +0x2AE). New rows: 0x613730 scenecam_ctor, 0x613BF0 scenecam_set_viewport, 0x613CA0 scenecam_set_clip, 0x613DB0, 0x5E45A0 scene_load_camera (copies the stage record into the SceneCamera), 0x614760 scene_render_begin (SetTransform/SetViewport), 0x616EC0/0x616EF0/0x616F20/0x616F50/0x616F80/0x616FB0 matrix getters, 0x4343E0 / 0x432D30 unit input handlers that call 0x438720 (actions 0x30/0x31), 0x766B4B D3DXMatrixInverse (statically linked).
* Remaining TODO(re) after round 11: `DAT_00A500B6` meaning, selector 0x430190, constants 0x9CFC20 (10.0), 0x9CFC24 (3.49e-5), 0x9CFC28, 0x9CFC2C (32.0), 0x9CFC34 (5000.0) whose users were not found, no mouse-wheel/edge-scroll rotate handler located.
* Round 4 resolved: projection/view build (0x6141A0, left-handed D3DX-style), aspect, viewport, ortho option, zoom clamp 80..350, debug free camera keys. Still missing: where `SceneCamera` near/far/fov_param/viewport are initialised (ctor of the class at 0x614xxx, writers of +0x44/+0x4C/+0x50), the real `IDirect3DDevice8::SetTransform` call site (consumer of +0xEE/+0x12E), the gameplay mouse-wheel / edge-scroll / rotate handlers that call 0x438720 and update `DAT_00A0BBE8..F4`, and the meaning of `FUN_007B2184` (acos vs asin vs atan).
* TODO(re): 0x429110 static-form key names (tokens are read positionally).
* Round-4 constants (RE: joan.exe .rdata float32): 0x9CF778 = 1000.0, 0x9CF77C = 0.5, 0x9CF878 = 5.0 (eye margin in 0x424010), 0x9CF884 = 0.017453292 (deg->rad), 0x9CF888 = -2.0, 0x9CF5C4 = 0.0, 0x9CF5C8 = 0.001, 0x9CF5D4 = 0.99, 0x9CF874 = -0.95, 0x9CF868 = double -0.005; `DAT_00A500BA` game-ms clock, `DAT_00A500BE` frame delta (docs/re/TIMING.md §2). Consequence: 0x427D90's ground clamp uses +-1000.0 vertical box and `dz*dt/1000`; collision thresholds -0.95 / 0.99 are dot-product (cosine) tests.
* Still unread (TODO(re), data section): `0x9CFC40` (zoom speed), `0x9CFC38` / `0x9CFC1C` (zoom compare limits; clamp results are literals 80.0 / 350.0), `0x9CFC18` (follow height offset), `0x9CFCA0` (back-off step), `0x9CFBF8` (probably pi/2, double), `0x9CF7A0` / `0x9CF7A4` (visibility box margins), `0x9CF9C4`, `0x9CF9CC`, `0x9CF9D0`/`D4`/`D8` (free-cam yaw wrap: presumably pi, 2*pi, -pi), `0xA336AC` (pi?), `0xA33754`, `0xA3373C`, `0xA33758` (lock-on tolerances/range, writable .data), `0xA0BBE8`/`EC`/`F0`/`F4` (initial pitch/dist defaults, .data), `DAT_00A500B6`, `0x9E2FF0` (factor in perspective: fov*factor), `0x9E2FFC`, `0x9E2FE8` (ortho constants, probably 2.0 and 1.0), and CRT `0x7B2184` / `0x7B2564`.
* Constants read (RE: joan.exe .rdata, float32): 0x9CF784 = 1.0, 0x9CF794 = 2.0, 0x9CF798 = 3.0, 0x9CF838 = 0.05 (speed_scale divisor), 0x9CF780 = 46.1, 0x9CF78C = 2.25, 0x9CF79C = 4.0, 0x9CF83C = 0.9 (last four: not yet matched to a use in camera code). The Hermite algebra (3.0, 2.0, 1.0) is thereby confirmed.
* TODO(re) constants still to read (data section): `DAT_009CF778` (clearance half-height / ground smoothing divisor in 0x427D90), `DAT_009CF878` (extra eye distance margin in 0x424010), `DAT_009CF884` (deg->rad, presumed pi/180), `DAT_009CF5C4` (0.0?), `DAT_009CF868`, `DAT_009CF874`, `DAT_009CF5D4` (thresholds in 0x424010), `DAT_00A69CD8..E0` (vec3, presumably 0,0,0), `DAT_00A500BA` (game ms clock) and `DAT_00A500BE` (frame delta). Still only inferred algebraically (not read): `DAT_009CF888` = -2.0 and `DAT_009CF77C` = 0.5.
* TODO(re): the `__ftol` operand in 0x427D90 (animated elapsed time: lost by the decompiler); callers that set `CamMan` +0x78/0x7C/0x80/0x84 (blend) and +0xA8..0xBC (collision tuning).
* TODO(re): who calls 0x428E90 / 0x4EBB40 with the two float offsets (likely 0x4F2F80).
