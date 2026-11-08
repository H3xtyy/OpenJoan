# PathSpace (`d:\ww\uti3d\pathsp.cpp`) — navigation mesh of one world block — joan.exe

One object per `.pfd` file ([format](../../formats/pfd.md)): a polygon mesh with neighbour links, a lazily built BVH over the polygons, and an
optional height lattice. Size **0x38** (constructor `0x664FD0` = `memset(this, 0, 0x38)`, allocated with `operator_new(0x38)` in `0x4F2F80` and `0x442D30`;
destructor `0x665000`). Functions: [pathfind.md](../functions/pathfind.md). Containers are `uti3d::blkarrb` handle lists ([BlockArray](BlockArray.md):
1-based, stable addresses, element count at list `+0x2C`, handle table at `+0x24`).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | BlkList<Point3>* | `points` | vertex pool, 12-byte elements (`i32 x,y,z`), created `blkarrb(0xC, 0x800)` | 0x670980 (`new 0x30`, filled with 3 reads), 0x665480 |
| 0x04 | 4 | BlkList* | `edit_list_a` | editor only (never allocated by the runtime reader); freed by the destructor | 0x665000 |
| 0x08 | 4 | BlkList* | `edit_list_b` | editor only | 0x665000 |
| 0x0C | 4 | BlkList* | `edit_list_c` | editor only | 0x665000 |
| 0x10 | 4 | BlkList* | `edit_list_d` | editor only | 0x665000 |
| 0x14 | 4 | BlkList<PathPoly>* | `polys` | the polygons, element size 0x74 ([PathPoly](PathPoly.md)); `NULL` = empty space. Count = list `+0x2C` (`0x66B7A0`, `0x66B9F0`) | 0x670980, 0x66B9F0 |
| 0x18 | 4 | BlkList<BvhNode>* | `bvh` | BVH nodes, element size 0x38, `blkarrb(0x38, 0x800)`; built on demand by `0x6745E0` when empty and polygons exist; cleared by `0x674590` | 0x6745E0, 0x674590 |
| 0x1C | 4 | DynArr<PathPoly*>* | `query_scratch` | result array of the box queries (elements 4 bytes, `dynarr(4, 0x1E)`), allocated on first query, cleared before each query | 0x66C020, 0x6731D0 |
| 0x20 | 4 | i32 | `stamp` | search generation counter. `0` = never searched: the first search zeroes `PathPoly+0x38` of every polygon and sets it to 1; every search then uses the current value and increments it | 0x66CB40 |
| 0x24 | 4 | i32 | `cell_w` | width of one coarse lattice cell (world units), from the file | 0x670980 (`ECX[9]`), 0x664180, 0x66FCD0 |
| 0x28 | 4 | i32 | `cell_h` | height of one coarse lattice cell | 0x670980 (`ECX[10]`) |
| 0x2C | 4 | i32 | `bias` | **cost-function bias** passed to the search as its last argument: `1` for scene spaces (`0x4F2F80` writes 1 after loading both files), `0` for the arena space (`0x442D30`). With 1 the search favours large polygons (see pathfind.md "cost function") | 0x66CB40 (arg 13), 0x4F2F80 `+0x2C = 1`, 0x442D30 `+0x2C = 0` |
| 0x30 | 4 | Plasma* | `lattice` | coarse height lattice: [Plasma](Plasma.md) object (0x14 bytes: `i16 w, h; i16* data`), `w = nx-1`, `h = ny-1` cells, `(w+1)*(h+1)` samples. `NULL` = none | 0x670980, 0x664180, 0x4C58A0 |
| 0x34 | 4 | Plasma** | `sub_lattices` | `w*h` pointers (cell-major `cy*w + cx`), each `NULL` or a refined [Plasma](Plasma.md) for that cell | 0x670980 (`new w*h*4`, memset 0), 0x664180, 0x66FCD0 |

## Invariants / behaviour
* All polygon and point handles are **1-based**; handle 0 means "none" everywhere (`0x66C020` returns 0 = no polygon).
* `lattice` and `sub_lattices` exist only when the file has a lattice section; `sub_lattices` is allocated only if `n_sub != 0`.
* Heights are `int16` in the lattice; interpolation in `Plasma::sample_flat` (`0x4C7B70`).
* The scene table of the world (global array at `0xA57636`, stride 0x34, count at `0xA57BB2`; inserted by `0x6AD270`) pairs two spaces per block:

### Scene table record (0x34 bytes, global at `0xA57636`; `this+0x58E + i*0x34` of the world object, count `+0xB0A`)
| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | PathSpace* | `ps_level` | `<scene>.pfd` (block `+0x54`) | 0x6AD270, 0x6AAC10 |
| 0x04 | 4 | PathSpace* | `ps_world` | `<scene>w.pfd` (block `+0x58`; equal to `ps_level` if no w file) | 0x6AD270, 0x6AAC10 |
| 0x08 | 4 | i32 | `origin_x` | block origin (world units), `block+0x44` | 0x6AD270, 0x6718C0 |
| 0x0C | 4 | i32 | `origin_y` | `block+0x48` | 0x6AD270, 0x6718C0 |
| 0x10 | 4 | f32 | `origin_xf` | `(float)origin_x` — float origin used by the ground tracker | 0x6AD270, 0x6AE8E0 |
| 0x14 | 4 | f32 | `origin_yf` | | 0x6AD270 |
| 0x18 | 4 | i32 | (0) | zero-initialised | 0x6AD270 |
| 0x1C | 24 | f32[6] | `bounds` | world AABB: min x,y,z at `0x1C/0x20/0x24`, max x,y,z at `0x28/0x2C/0x30` (copied from the 6 words passed to `0x6AD270`) | 0x6AD270, 0x6AAE70, 0x6718C0 |

`0x4A2B20(table, count, key)` finds the record whose `ps_level` (or `ps_world`) equals the PathSpace passed as key (used by the clearance test to
learn the block bounds of the polygon's owner). `0x6AAC10(rec, selector)`: `selector == 0` -> `ps_world` if non-null else `ps_level`; `selector != 0` ->
`ps_level` if non-null else `ps_world`.

## Referenced elsewhere (for the unit/world owners)
| holder | offset | meaning | evidence |
|---|---|---|---|
| world block (`unit+0x339`) | `+0x44`, `+0x48` | block origin (i32) subtracted from unit positions before a path request and added back with `0x66FA00` | 0x4F7BF0, 0x505DD0 |
| world block | `+0x54`, `+0x58` | `PathSpace*` level / world | 0x4F2F80 (`param_1[0x15]`, `[0x16]`) |
| unit | `+0x432`, `+0x473` | two embedded [PathRoute](PathRoute.md) objects (0x41 bytes apart); `+0x40E` = remaining route length | 0x505DD0 |
