# CpuUsage (`cpuusage.cpp`) — developer profiler, RDTSC based — joan.exe

Constructed with a slot count `N`; **not needed for gameplay** (debug overlay data only). The same TSC calibration code is duplicated in `0x45C8C0` (called by WinMain when
CONFIG.DAT is missing); the measured **TSC frequency (Hz) is later used by `config_defaults` to scale default quality by CPU MHz** (see [GameConfig](GameConfig.md)).
Functions: [gameloop.md §6](../functions/gameloop.md).

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `initialised` | -1 during construction, 1 when ready, 0 after `free` | verified |
| 0x04 | 4 | i32 | `slots` | `N` | verified |
| 0x20 | 8 | u64 | `tsc_base` | `rdtsc` taken at the end of init | read |
| 0x28 | 4 | u64* | `a` | `N*8` bytes (not cleared) | read |
| 0x2C | 4 | u64* | `b` | `N*8`, zeroed | read |
| 0x30 | 4 | u64* | `c` | `N*8`, not cleared | read |
| 0x34 | 4 | u32* | `d` | `N*4`, zeroed | read |
| 0x38 | 4 | u64* | `e` | `N*8`, zeroed | read |
| 0x3C | 4 | u64* | `hist_u64` | `N*0x1E0` = N x 60 x 8 — per-slot ring of 60 samples, zeroed | read |
| 0x40 | 4 | u32* | `hist_u32` | `N*0xF0` = N x 60 x 4, zeroed | read |
| 0x44 | 8 | u64 | `tsc_hz` | TSC ticks per second from the `Sleep(100)` calibration: `(tsc1-tsc0) * QPF / (qpc1-qpc0)` (global alias `DAT_00A485EC/F0`) | verified |
| 0x4C | 4 | Node* | `nodes` | array of `N+1` nodes of 0x1C bytes; `node[i].id = i` (element dtor 0x45E860); allocated with an element-count cookie | read |
| 0x50 | 4 | List* | `list` | 0x50-byte list object (ctor 0x45EC60), then filled by 0x45ED30(nodes), 0x45EFB0(0), 0x45EE70(0) | read |
| 0x54 | 4 | List* | `cursor` | = `list` | read |
