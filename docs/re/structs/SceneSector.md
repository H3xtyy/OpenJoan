# SceneSector (en3d `scene_s.cpp`) and its Light records

`scene_s.cpp` is the per-**scene-sector** container: one terrain/static-geometry sector of the world (see
[PlnSectorEntry.md](PlnSectorEntry.md), [WorldGridCell.md](WorldGridCell.md), `docs/formats/pln.md`) with its meshes,
dynamic lights and projector list. Several offsets are unaligned (1-byte packing). Status V/R/I as in [Plane.md](Plane.md).
Dtor `0x5EF900`, add light `0x5F0040`, add item `0x5F35F0`, add projector `0x5F39A0`.

| off | size | type | name | meaning | evidence | st |
|---|---|---|---|---|---|---|
| 0x00 | 4 | Scene* | scene | owner; owner has `+0xCD` i32 (capacity) and `+0xD1` Actor** scratch array reused for sorting translucent actors (grown by 0x80) | 0x5F03B0, 0x5F07F0 | R |
| 0x08 | ? | embedded | bounds | embedded object passed to the visibility test `0x615D70(&this->bounds)` | 0x5F03B0 | I |
| 0x38 | 4 | Plane* | mesh | terrain/static mesh of the sector. Drawn if `0x5D0480` (>0 vertices and >0 triangles) | 0x5F03B0, 0x5EF900 | V |
| 0x3C | 4 | Object* | aux_object | second drawable (drawn through the actor draw `0x5ABC50`, only when option `DAT_00A03834`) | 0x5F07F0 | R |
| 0x40 | 4 | Plane* | overlay_mesh | second mesh (drawn after the first when `DAT_00A03834`) | 0x5F07F0 | R |
| 0x44 / 0x48 / 0x4C | 12 | ptr / i32 / i32 | items | pointer array, count, capacity (grows by 8) | 0x5F35F0 | V |
| 0xBE / 0xC2 / 0xC6 | 12 | i32 / i32 / Projector** | projectors | capacity (grows by 5), count, array; each projector is deleted by `0x5EFB90` | 0x5F39A0, 0x5EF900 | V |
| 0xD2 / 0xD6 / 0xDA | 12 | i32 / i32 / Light** | lights | count, capacity (grows by 8), array | 0x5F0040 | V |
| 0xE0 | 4 | i32 | lightset_handle | handle into the global light-set cache; recomputed on every queued draw by `0x5E7870` and stored in the queue's current key | 0x5F03B0 | V |

## Light (0xCE bytes, ctor `0x5C5830`)

| off | type | meaning (from the vertex-lighting bake 0x5F2780 and `0x5E76F0`) |
|---|---|---|
| 0x00 | i32 | type: 0 = ambient/constant, 1 = directional, 3 = point (with radius) |
| 0x30 | float | intensity (`light[0xC]`) |
| 0x38 | float[3] | position (type 3) |
| 0x44 | float | radius (falloff `1 - dist/radius`) |
| 0x9E | float[3] | direction (type 1) |
| 0xCA | i32 | handle into the global light cache (record 0xD6 B), refreshed per frame by `0x5E76F0` (`memcmp`-style equal test `0x5C6560`, then copy `0x5C63C0`) |

Light-set cache record (0x8D B) built by `0x5E7870`: compares against the sector's lights `0xCA` handles in order
(max 0x20 lights) and one more byte (`0x5E8F90`); equality test `0x5E8100`, fill `0x5E8040`. Two sectors with the same
active lights share one handle, hence one reorder key (see [ReorderQueue.md](ReorderQueue.md)).
