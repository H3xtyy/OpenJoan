# PathPoly — one convex navigation polygon and its A* node (`pathsp.cpp`) — joan.exe

Element of `PathSpace.polys` ([PathSpace](PathSpace.md)); **0x74 bytes** (116: the stack sentinel in `0x675660` is `char[116]`, the last field is `+0x70`).
The search state lives inside the polygon itself (no separate node array); a per-space `stamp` makes it valid for one search only.
Handles are 1-based indices into the list; a polygon's own handle is `+0x00`.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `index` | own 1-based handle (`*piVar6 = i` in the loader) | 0x670980 |
| 0x04 | 4 | i32 | `type` | 0 = terrain polygon (height from the lattice), 1 = planar polygon (height from the vertex plane) | 0x670980, 0x664180, 0x66FCD0 |
| 0x08 | 12 | i32[3] | `bbox_min` | min x,y,z of the vertices — **recomputed** from the vertices by `0x663CB0` | 0x663CB0 |
| 0x14 | 12 | i32[3] | `bbox_max` | max x,y,z | 0x663CB0 |
| 0x20 | 4 | i32 | `flag` | 0 = normal; **1 = blocked** (excluded from search, connectivity labelling and the "contains point" test unless the caller passes the override flag; edges to a blocked neighbour count as walls for the clearance test); **-1 = narrow** (average portal edge < 170: skipped by searches with radius > 0, allowed for radius 0). Set to -1 by the loader post-pass `0x670980`; who sets 1 is not in the export | 0x6718C0, 0x66CE30, 0x670980, 0x675660 |
| 0x24 | 4 | i32 | `model_id` | result of the name lookup `0x4A36C0` for the polygon's `field8` model slot (0 = none); the link to a door/bridge object | 0x670980 |
| 0x28 | 4 | i32 | `n_verts` | vertex (= edge) count, 3..32 | 0x663CB0 |
| 0x2C | 4 | i32 | `visit_stamp` | scratch for the wall/clearance recursions (`0x671CB0`, `0x672500`): equals the call's id while the polygon is on the recursion path or done; reset to 0 on failure. Global id counter `DAT_00A57632` | 0x6718C0, 0x671CB0 |
| 0x30 | 4 | Point3** | `verts` | array of `n_verts` pointers into the point pool (`PathSpace.points`); edge `i` (1-based) = `verts[i-1]` -> `verts[i % n]` | 0x663CB0, 0x6718C0 |
| 0x34 | 4 | PathPoly** | `neighbours` | `n_verts` pointers, `neighbours[i-1]` = polygon across edge `i`, or NULL (wall). Zero-filled at creation | 0x663CB0, 0x664130, 0x664080 |
| 0x38 | 4 | i32 | `search_stamp` | equals `PathSpace.stamp` of the search that touched the node (valid-state test `0x66DD00`) | 0x66DD00, 0x664920 |
| 0x3C | 4 | PathPoly* | `parent` | search-tree parent (0 for the start polygon and before init) | 0x664920, 0x664EB0 |
| 0x40 | 4 | PathPoly** | `children` | array of search-tree children (capacity `+0x54`, count `+0x52`); lets a cost improvement be pushed down the subtree (`0x66DDB0`) | 0x664920, 0x664EB0 |
| 0x44 | 4 | PathPoly* | `next` | singly-linked list link: open list (sorted), closed list; also the stack link of the component flood fill | 0x66CE30, 0x675660 |
| 0x48 | 4 | i32 | `cx` | node position x = integer mean of the vertex x (polygon centroid by vertices) | 0x664920 |
| 0x4C | 4 | i32 | `cy` | node position y | 0x664920 |
| 0x50 | 2 | i16 | `cz` | node z = mean vertex z (truncated to 16 bit) | 0x664920 |
| 0x52 | 2 | i16 | `child_count` | | 0x664920, 0x664EB0 |
| 0x54 | 2 | i16 | `child_cap` | grown to `n_verts` when smaller (children array reallocated) | 0x664920 |
| 0x56 | 1 | u8 | (pad) | | |
| 0x57 | 1 | u8 | `closed` | 1 once expanded; cleared by `0x664920` | 0x66CE30, 0x664920 |
| 0x58 | 4 | i32 | `g` | cost from the start | 0x66CE30 |
| 0x5C | 4 | i32 | `f` | priority key (`g + h - size bonus`, see pathfind.md) | 0x66CE30 |
| 0x60 | 4 | i32 | `h` | integer distance from the node position to the goal point | 0x66CE30 |
| 0x64 | 2 | i16 | `weight` | terrain cost class (signed; negative treated as 0 at the start node). Set by `0x663CB0`; readers `0x66BA30`, `0x66BAA0`; merge pass requires equal weight | 0x663CB0, 0x66A640, 0x66CE30 |
| 0x66 | 2 | i16 | `max_weight` | max `weight` along the search path to this node | 0x66CE30 |
| 0x68 | 4 | i32 | `acc_weight` | sum of weights along the path (kept, but not used by the comparator) | 0x66CE30, 0x66FCA0 |
| 0x6C | 4 | PathSpace* | `owner` | back pointer (set in the loader) | 0x670980, 0x664180 |
| 0x70 | 4 | i32 | `component` | connected-component id (`0x675660`: flood fill over neighbours with `flag != 1`, ids 0.. in descending polygon order); `-1` for blocked polygons | 0x675660 |

## Derived rules
* `n_verts` is capped at 32 by the editor's merge (`0x66A640`: merged polygon vertex count `n1 + n2 - 2 <= 0x20`; merge also needs equal `type` and `weight`).
* Vertices are counter-clockwise: a point is inside when for every edge `(px - x_i)*(y_{i+1} - y_i) - (py - y_i)*(x_{i+1} - x_i) <= 0` (`0x6718C0`).
* Portals: the edge shared with a neighbour is the pair `(verts[i-1], verts[i % n])`; the route builder stores those two points (+ their float midpoint).
* BVH node (`0x38` bytes, same shape as the world BVH in [World](World.md)): `+0` parent, `+4/+8` children, `+0xC` split axis (0=X, 1=Y, 2=Z), `+0x10` bbox min xyz, `+0x1C` bbox max xyz, `+0x28` centre xyz, `+0x34` polygon (leaf only; one polygon per leaf). Source: `0x6747A0`.
