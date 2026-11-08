# DynGrass (`dyngrass.cpp`) — dynamic grass mesh batcher

Reference build: joan.exe. A global instance (address not visible in the export: `this` is passed in ECX) is initialised by the game-loading
sequence 0x470D40 (log text `"Loading dynamic_grass"`), which calls `0x4641A0(capacity = DAT_00A58058, enabled = DAT_00A58057)`; it is destroyed by 0x471010 -> 0x464390.
The two inputs are globals that look like user settings (grass density and on/off; `TODO(re)`: confirm in the settings code). All members are packed (odd offsets). Despite the name the file contains **no wind animation or placement
code**; it only builds merged geometry for grass objects (see engine/lights.md "Dynamic grass").

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x000 | 4 | f32 | capacity | `max(100, requested)` stored as float; entry arrays hold `ftol(capacity)` entries | verified (0x4641A0, 0x4644D0) |
| 0x004 | 1 | u8 | enabled | `param_3` of init / `0x464600(flag)` | verified |
| 0x005 | 4 | Entry* | entries_a | `capacity * 0x66` bytes, zeroed | verified |
| 0x009 | 4 | i32 | active_count | number of used entries in `entries_a`; reset to 0 by `0x464600(0)` | verified |
| 0x00D | 4 | Entry* | entries_b | second `capacity * 0x66` array, zeroed | verified |
| 0x011 | 4 | i32 | count_b | zeroed on shutdown | read |
| 0x015 | 100 x 4 | i32[100] | model_ids | grass model slots: low 16 bits = model/item id; released through `0x425CE0(id)` (item table record `base + 0x14 - 0x2BF + id*0x2BF`) and `0x4A3E30` (decrement refcounts, unload at 0) | read |
| 0x1A5 | 4 | i32 | model_count | used slots in `model_ids` (<= 100 by size) | read |
| 0x1A9 | 12 | f32[3] | vec_a | `DAT_00A18A40 * 10.0` (0x41200000) | read |
| 0x1B5 | 12 | f32[3] | vec_b | `DAT_00A69CD8/DC/E0` (camera position) | read |
| 0x1C1 | 12 | f32[3] | vec_c | same camera position (second copy) | read |
| 0x1CD | 4 | i32 | zero | cleared | read |

## Entry (0x66 bytes, only some fields known)
| offset | size | meaning | status |
|---|---|---|---|
| 0x08..0x1F | 6 x f32 | bounding box: min xyz at +0x08/+0x0C/+0x10, max xyz at +0x14/+0x18/+0x1C | read (0x466560) |
| 0x24 | 4 | source pointer (non-null guard in `0x466560`) | read |
| 0x28 | 4 | built dynamic mesh (`Plane*`, see [Plane](Plane.md)); freed by `0x464600(0)` through `0x5EFDB0` + `0x420840(1)`, cleared to 0 | read |

`TODO(re)`: remaining entry fields (placement position, per-instance state) are not touched by the four exported functions.
