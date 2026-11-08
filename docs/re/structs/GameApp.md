# GameApp (window + D3D8 device + screen state machine singleton) — joan.exe

One global object (address not visible in the export; it is the `this`/ECX of `0x4D6C50`, `0x4D6CE0`, `0x499540`, `0x4D48F0`, `0x499960`, `0x603710`,
`0x6032D0`, `0x603400`). It merges the engine window/device wrapper (virtual class: vtable at `+0`) and the game's screen state. The merge is **inferred**
from the fact that the same `this` reaches `+0x3A` (state) and `+0x307..+0x32B` (render targets); the main-loop function that owns the fields
`+0xC/+0xD/+0x10/+0x11/+0x18/+0x22/+0x26/+0x2A/+0x2E/+0x32/+0x36` (`0x499960`) was not exported, so those offsets are `TODO(re)`.
Functions: [gameloop.md](../functions/gameloop.md). State semantics: engine/gameloop.md §3.

| offset | size | type | name | meaning | evidence / status |
|---|---|---|---|---|---|
| 0x00 | 4 | vtable* | `vtbl` | `[+0x08]` create window+device (called by 0x5A1BD0), `[+0x0C]` destroy/shutdown when `+0x12==0`, `[+0x10]` (on D3D device object `DAT_00A52428`, not here) = `GetAvailableTextureMem`, `[+0x18]` pre-init hook | 0x603710, 0x4D6CE0, 0x603780: read |
| 0x04 | 4 | HINSTANCE | `h_instance` | stored by 0x5A1BD0 from WinMain's first argument | 0x5A1BD0: read |
| 0x08 | 4 | HWND | `h_wnd` | passed with `h_instance` to 0x592380 by 0x6037F0 | 0x6037F0: inferred |
| 0x0F | 1 | u8 | `initialised` | set to 1 at the end of 0x5A1BD0; tested before teardown | 0x4D6CE0, 0x603780: verified |
| 0x10 | 1 | u8 | `shutting_down` | set to 1 by 0x4D6CE0 / 0x603780 before the engine teardown | verified |
| 0x12 | 1 | u8 | `external_window?` | cleared by 0x5A1BD0; if 0 shutdown is the single virtual call `[+0xC]`, else the explicit teardown path runs | read (semantics inferred) |
| 0x15 | 1 | u8 | `flag15` | cleared by 0x603710 | read |
| 0x0C | 1 | u8 | `in_game` | per TIMING.md §4: update/draw gated on it | docs/re/TIMING.md |
| 0x0D | 1 | u8 | `blocked` | loading/blocked: update/draw skipped | TIMING.md |
| 0x11 | 1 | u8 | `force_draw` | TIMING.md | |
| 0x18 | 1 | u8 | `uncapped` | **setter found: 0x603710 writes 1 unconditionally** just before the virtual pre-init call `[+0x18]` (so the 500 Hz/uncapped loop mode is the default unless that virtual or later code clears it; no clearing write in the exported code) | 0x603710: read |
| 0x22/0x26/0x2A/0x2E/0x32/0x36 | | | counters/clocks/dt | see TIMING.md §4 (update/draw counters, real ms, dt, game ms, scaled dt) | TIMING.md |
| 0x44 | 4 | f32 | `speed` | game-speed multiplier (TIMING.md); `0x4D6C50` initialises it to 1.0; `0x4E5FC0` skips the main block when 0 | verified |
| 0x24 | 4 | D3DFORMAT | `rt_format` | texture format passed to 0x603400's `CreateTexture` | read |
| 0x307 | 4 | D3DFORMAT | `depth_format` | used by 0x6032D0 `CreateDepthStencilSurface` | read |
| 0x317 | 4 | IDirect3DSurface8* | `shadow_depth_surface` | created 1024x1024 when `GameConfig.+0x37 == 2` | 0x6032D0, 0x420EA0: verified |
| 0x31B / 0x31F | 4+4 | u32 | `shadow_depth_w/h` | 0x400 / 0x400 | verified |
| 0x323 | 4 | IDirect3DTexture8* | `aux_rt_texture` | render-target texture 512x256 (WinMain, always) | read |
| 0x327 / 0x32B | 4+4 | u32 | `aux_rt_w/h` | 0x200 / 0x100 | read |
| 0x3A | 1 | u8 | `state` | current screen/mode, 0..0xF (enum in engine page §3.1) | 0x499540, 0x4D48F0, 0x4D6C…: verified |
| 0x3B | 1 | u8 | `prev_state` | previous value of `state` | 0x499540: verified |
| 0x3C | 1 | u8 | `req_state` | requested state; written by `set_next_state` (0x4D7E70), and by 0x499540 to the new state | verified |
| 0x40 | 4 | f32 | `frame_scale` | dt in 1/30 s units (<= 5.0), recomputed every loop pass (TIMING.md); initial 1.0f from 0x4D6C50 | verified |
| 0x48 | 2 | u16 | `u48` | initialised to 1 | read |
| 0x4A | 1 | u8 | `u4a` | initialised to 1 | read |
| 0x151 | 1 | u8 | `paused` | non-zero: `0x4D48F0` skips the whole frame draw except the minimal pause path (0x6E47B0 + present) | 0x4D48F0: read |
| 0x152 | 1 | u8 | `u152` | initialised to 1 in 0x4D6C50 | read |

Other globals touched by the same family (not fields): `DAT_00A52428` = IDirect3DDevice8*, `DAT_00A52424` = IDirect3D8*, `DAT_00A5242C/30` = back-buffer/depth objects,
`DAT_00A3A30C` = cutscene video playing, `DAT_00A5D874` = video start time (ms, 0 = unset), `DAT_00A5D878` = pointer to an object with a virtual deleting
destructor freed at shutdown (`TODO(re)` which), `DAT_00A50099` = "running" flag set before the main loop, `DAT_00A500C2` (update gate, see engine page).
