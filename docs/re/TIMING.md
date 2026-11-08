# Timing — main loop, frame limiter and the simulation step

Reference build: `joan.exe` (see `docs/re/BINARY.md`). All addresses below are from that build.
Evidence: Ghidra pseudocode + instruction listing of the functions named here (session 2026-10-22) and the
apitrace capture `$JOAN_WORK_DIR/traces/menu-2026-10-22.trace` (see `docs/re/RENDER_STATES.md`).

## 1. Summary (what OpenJoan must reproduce)

| Fact | Value | Evidence |
|---|---|---|
| Clock source | `timeGetTime()` (1 ms, WinMM) through wrapper `0x591050` on a `Timer` object at `0xa45fa8` | `0x499960` call sites |
| Frame limiter | `MsgWaitForMultipleObjectsEx(0, NULL, wait_ms, 0xff, MWMO_ALERTABLE)`; a frame runs only on `WAIT_TIMEOUT` (0x102) | `0x499960` @ `0x499a9a` |
| Nominal (design) rate | **30 Hz** sim and render (`FrameRateCtrl::init(30, 30, allow_skip=1, now)`); the frame scale is defined as `dt / (1000/30)`, so 30 Hz is the authors' unit of game time even though the shipped loop runs faster (next row) | `0x4999c4..0x4999cf` |
| Shipped default rate | **500 Hz** (2 ms wait, i.e. effectively uncapped): the application-framework init `0x603710` sets `Game+0x18 = 1` unconditionally (and `+0x15 = 0`) before its virtual pre-init; no exported code clears it, and the Wine menu capture ran at ~333 FPS. The 30 Hz branch is reachable only if something clears `+0x18` (`TODO(re)`: none found in 1145 exported gameloop functions — possibly an options/debug path) | `0x49999b..0x4999b2`, `0x603710` |
| Elapsed time per frame | `dt_ms = now − last_frame`, clamped to **≤ 1000 ms**; forced to **33 ms** while video capture (`DAT_00a3a30c`) is on | `0x499c1f..0x499c4d` |
| Frame scale | `Game+0x40 = dt_ms / 1000 × 30` (float, "number of 1/30 s frames"), clamped to **≤ 5.0**; `1.0` when `dt_ms == 0` | `0x499c54..0x499ca4` |
| Game speed | `Game+0x44` (float multiplier); `Game+0x36 = (int)(dt_ms × speed)` = game milliseconds this frame | `0x499cab..0x499ccc` |
| Clocks kept | `Game+0x2a` += `dt_ms` (real ms since loop start); `Game+0x32` += `Game+0x36` (speed-scaled game ms) | `0x499ccf..0x499cf0` |
| Simulation model | **variable step**: every subsystem integrates with the frame's elapsed ms (`DAT_00a500b6`) or the frame scale; there is no fixed-step accumulator | `0x4e5fc0`, `0x4352f0`, `0x59b1c0`, `0x5866d0` |
| Frame skipping | designed into `FrameRateCtrl` but **dead**: the lag accumulator is zeroed before the test, so render is never skipped | `0x58ab10` |
| Present | `D3DPRESENT_INTERVAL_IMMEDIATE`, fullscreen 1024×768, one back buffer, `D3DSWAPEFFECT_FLIP` | apitrace call 238 |

OpenJoan runs the original main loop through the code table, so it keeps this variable-step model unchanged; the
replay recorder (`src/runtime/replay`) captures the timer reads that feed it. `TODO(re)`: measure the real `dt_ms`
distribution of the retail loop on period hardware.

## 2. Objects involved

| Address | Role | Notes |
|---|---|---|
| `0xa45fa8` | `Timer` singleton | `0x591050` = `timeGetTime()` wrapper (returns ms) |
| `0xa51658` | `FrameRateCtrl` singleton (`ilib`, name inferred) | layout in §3 |
| `0xa485a8` | `CpuUsage` singleton (`d:\ww\cpuusage.cpp`) | profiling only: `0x45c040` ctor measures QPC vs RDTSC over `Sleep(100)` to get the CPU clock; not needed for the port. The `rdtsc` helpers `0x45e750` / `0x45e7f0` that bracket each pass of the unit scheduler `0x51e280` (ECX = `0xa485a8`) only fill per-bucket 16-bit counters at `+0x40..+0x6c` of this object; they never feed back into scheduling (unit family round 2) |
| `0xa50088` | subsystem container polled by `0x421b60` | calls `0x594f90` (DirectInput mouse/keyboard buffered-data pump), `0x59b1c0` (video/stream state), `0x5866d0` (3-slot stream/music state) — all read `DAT_00a500b2`/`DAT_00a500b6` |
| `0xa58b00` | polled once per frame by `0x490eb0` | `TODO(re)` |
| `Game` (this of `0x499960`) | the game/application object | fields in §4 |

Globals: `DAT_00a500b2` = current game time (ms, speed-scaled, compared against per-object stamps),
`DAT_00a500b6` = elapsed ms of the current frame (`uint32`, the value every subsystem integrates with),
`DAT_00a500ba` = the millisecond clock the unit family compares against its deadlines ("now"), `DAT_00a500be` = **per-frame step in ms (dt; 0 while the game is paused)** — the unit family decrements its countdowns by it (`invuln_ms`, RETREAT `p3`, `fell_timer`, ...; docs/re/functions/unit.md §4) and `0x52daa0` divides the constant `21.691975` by it (corrected from `TODO(re)`, unit family round 2),
`DAT_00a500c2` = game mode byte (`5` disables the per-frame world update), `DAT_00a3a30c` = video-capture flag.
`TODO(re)`: the writer of `DAT_00a500b2..be` (not in `0x499960`; most likely the `Game` copy-out just before `0x4e5fc0`).

## 3. `FrameRateCtrl` (`0xa51658`) — layout and semantics

| Offset | Type | Name | Meaning |
|---|---|---|---|
| 0x00 | int32 | max_skip | third ctor arg (1 normal, 0 in 500 Hz mode); compared against the skip counter |
| 0x04 | int32 | sim_rate | 30 (or 500) |
| 0x08 | int32 | sim_interval_ms | `1000 / sim_rate` = 33 (or 2) |
| 0x0c | int32 | render_rate | 30 (or 500) |
| 0x10 | int32 | render_interval_ms | `1000 / render_rate` |
| 0x14 | int32 | use_render_interval | 0 → wait/accumulate with `sim_interval_ms`; `is_render_due()` (`0x58ac80`) returns `this+0x14 == 0` |
| 0x18 | uint32 | last_tick_ms | updated by `tick(now)` (`0x58ab10`) |
| 0x1c | int32 | lag_ms | accumulated overshoot; **always reset to 0** before use (`0x58ab10` dead store) |
| 0x20 | int32 | skip_count | incremented by `end_frame()` (`0x58ace0`) while `skip_render` is set; `wait_ms()` returns 0 when > 0 |
| 0x24 | int32 | frames_rendered | counter |
| 0x28 | int32 | frames_simulated | counter |
| 0x2c | int32 | skip_render | `is_sim_due()` (`0x58acb0`) returns `this+0x2c == 0`; never set because of the dead store |
| 0x38 | uint32 | fps_window_start_ms | |
| 0x3c | int32 | fps_window_frames | incremented by `end_frame()` |
| 0x40 | int32 | fps | `frames × 1000 / window_ms`, recomputed every ≥ 1000 ms |

Methods: `init(sim_rate, render_rate, max_skip, now)` `0x58a830`; `wait_ms(now)` `0x58aa90` →
`max(0, interval − (now − last_tick))`; `tick(now)` `0x58ab10`; `is_render_due()` `0x58ac80`;
`is_sim_due()` `0x58acb0`; `end_frame()` `0x58ace0`.

## 4. `Game` fields touched by the loop

| Offset | Type | Meaning | Evidence |
|---|---|---|---|
| +0x0c | bool | "in game" (update/draw gated on it) | `0x499b4e` |
| +0x0d | bool | "loading/blocked" (update/draw skipped when set) | `0x499b5d` |
| +0x0f | bool | checked by shutdown `0x4d6ce0` | |
| +0x10 | bool | quit requested (set on `WM_QUIT`) | `0x499a57` |
| +0x11 | bool | force one draw even if not due | `0x499b7a` |
| +0x12 | bool | shutdown path selector | `0x4d6ce0` |
| +0x18 | bool | **uncapped mode** → `FrameRateCtrl::init(500,500,0)`; set to 1 by the framework init `0x603710` (shipped default); no clearing write found | `0x49998e`, `0x603710` |
| +0x22 | int32 | update counter (set to 1 when loop arg is 0) | `0x499984`, `0x499d22` |
| +0x26 | int32 | draw counter | `0x499b91` |
| +0x2a | uint32 | real ms accumulator | |
| +0x2e | uint32 | `dt_ms` of this frame (clamped) | |
| +0x32 | uint32 | game ms accumulator (speed-scaled) | |
| +0x36 | int32 | `(int)(dt_ms × speed)` | |
| +0x3a | int8 | game state / screen id (switch in `0x4d48f0`: 2 and 9 are the 3D views, 9 is in-game) | |
| +0x3c | int8 | requested state; when ≠ +0x3a the loop calls `0x499540(new_state)` | `0x499b2d` |
| +0x40 | float | frame scale (dt in 1/30 s units, ≤ 5.0) | |
| +0x44 | float | game speed multiplier (0 = paused for `0x4e5fc0`) | |
| +0x151 | bool | paused/menu overlay (draw takes the reduced path) | `0x4d48f0` |

## 5. One iteration of the loop (`0x499960`)

1. If `quit` → leave. `PeekMessageA` → if a message is pending: `GetMessageA`/`TranslateMessage`/`DispatchMessageA`
   and loop (no frame).
2. `MsgWaitForMultipleObjectsEx(timeout = FrameRateCtrl::wait_ms(now))`. Anything but timeout → loop (no frame).
3. `now = timeGetTime()`; CPU-usage sample; `FrameRateCtrl::tick(now)`; poll input + stream states (`0x421b60`);
   `0x490eb0`.
4. If a state change is requested → `0x499540(state)`.
5. If in game and not blocked and (render due or forced): **draw** `0x4d48f0`, `++draw_count`, poll input again,
   `0x52f930`, `0x4d8560`, copy mouse state `DAT_00a51800→DAT_00a51802`, poll input, rotate the three
   `DAT_00a516dc/e0/e4` accumulators into `DAT_00a516e8/ec/f0` and clear them.
6. `dt_ms` / frame scale / game ms as in §1.
7. If update due and in game and not blocked: **update** `0x4e5fc0`, `++update_count`, poll input.
8. Poll input again if a draw happened; if a draw happened and video capture is on, `0x4d7f80` (grab frame).
9. `FrameRateCtrl::end_frame()`; `last_frame = now`.

Observed under Wine (menu, uncapped mode): ~333 presents/s, i.e. the loop is bounded only by the 2 ms wait plus
work. The update routine `0x4e5fc0` integrates particles with `dt = DAT_00a500b6 / 1000` seconds (call to
`0x68a170`), unit turning uses `DAT_00a500b6 × 3.4906585e-05 × degrees` (`0x4352f0`, i.e. `(π/180)/500` per ms),
so logic speed is tied to wall-clock, not to frames.

## 6. Dead code worth knowing

`0x5bf5d0` is the DirectX SDK `DXUtil_Timer` (modes 0–6: reset/start/stop/advance/get-absolute/get-app/get-elapsed,
`QueryPerformanceCounter` with `timeGetTime` fallback). It has no callers in the Steam build.
