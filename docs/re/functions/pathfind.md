# Path finding ("path space", `d:\ww\uti3d\pathsp.cpp` + unlabelled neighbours) — joan.exe 0x663990..0x675900

Scope: the navigation mesh that is loaded from the `.pfd` files ([format](../../formats/pfd.md)), its spatial index, point/height/clearance queries, the A\* search, the
funnel smoothing, and the way unit code calls it (`0x505DD0`, `0x4F7BF0`, ground tracker `0x6AE8E0`). Structs: [PathSpace](../structs/PathSpace.md),
[PathPoly](../structs/PathPoly.md), [PathRoute](../structs/PathRoute.md); related [Plasma](../structs/Plasma.md) (height lattice), [BlockArray](../structs/BlockArray.md), [World](../structs/World.md).
Consumer of the result: `unit_follow_path` 0x52DAA0 ([flocksys.md §9.2](flocksys.md)).

Sources: `$JOAN_WORK_DIR/decomp/game/pathfind/*.c` (274 functions, depth-1 callees). Confidence: **V** = read in full and behaviour clear, **R** = read, details of
some branches/arguments unclear, **I** = inferred from shape/callers. Caveats of the export: calls through `thunk_FUN_*` lost their argument lists and the `this`
register, so several argument roles below come from the callers (`0x505DD0`, `0x4F7BF0`, `0x66CB40`) and are marked. Constants in `.rdata` (`_DAT_009...`) were not
readable from the export; values quoted for them come from other docs or are deduced from the use and marked.
Evidence for "port this exactly" quality: **sections 4.1–4.6 are enough to re-implement the search and the point queries; 4.7 (smoothing) is the weakest part** —
a bit-exact port of `0x66ECE0` needs a disassembly pass (listed in Open questions).

## 1. Architecture in one page
* A **PathSpace** is the mesh of one world block: convex polygons (3..32 vertices, counter-clockwise) over a shared integer vertex pool, each edge optionally linked to the
  polygon across it (no link = wall). Two spaces per block: `<scene>.pfd` (level space, block `+0x54`) and `<scene>w.pfd` (world space, block `+0x58`); both are registered in the
  global **scene table** (0xA57636, stride 0x34) by `0x6AD270`. Coordinates are block-local world units (position − block origin `block+0x44/+0x48`).
* A **polygon is also its own A\* node**; search state (`stamp`, `g`, `h`, `f`, `parent`, `children`, open-list link) lives inside it ([PathPoly](../structs/PathPoly.md)).
* Heights: polygons of `type 0` take z from a 16-bit lattice (coarse lattice plus refined cells), `type 1` from their vertex plane. Besides path finding **this mesh is the game's ground
  height / slope source** (`0x66CAE0`, `0x6645B0`, ground tracker `0x6AE8E0`): units, particles and actors ask it for z.
* A search is **purely 2-D** (x,y); z is used only to select the start polygon (±16 units) and to give the route points a height.
* Output is a **PathRoute** ([PathRoute](../structs/PathRoute.md)): `start, p1..p(n-1), goal`; the unit walks the points in order.
* The cost model is an A\* with a **tier key**: first the largest "weight" class on the path (lower is better), then `f`. There is **no time budget**; the limit is a **node-expansion budget** (callers pass 100).
* No dynamic obstacles inside the search. Units block each other only through physics (not here). The only run-time input is the per-polygon `flag` (1 = blocked).

## 2. Function table
### 2.1 Loading, saving, lifetime
| address | name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x670980 | `pathsp_load_pfd(path)` | reads a `.pfd` into the PathSpace that is `this`: model table (≤ 32 entries), points, polygons (`0x663CB0` per polygon), links (`0x664130`), optional lattice + refined cells; then narrow-polygon flagging and component labelling (`0x675660`). Returns 0 when the file does not exist | stack of reads; layout in pfd.md | V |
| 0x66FCD0 | `pathsp_save_pfd(path)` | editor-build writer: points, polygons, deduplicated links, lattice, refined cells. Does not write the model table or `field8` (older layout) | complete read | V |
| 0x664FD0 | `pathsp_ctor` | `memset(this, 0, 0x38)` | | V |
| 0x665000 | `pathsp_dtor` | frees points, editor lists, polygons, BVH, scratch, lattices (`0x665290`) | | R |
| 0x665290 | `pathsp_free_lattices` | frees the `w*h` refined cells, the pointer array and the coarse lattice, and four lists | | R |
| 0x4F2F80 | `world_load_block` (not this module) | loads `<scene>.pfd` -> block `+0x54`, `<scene>w.pfd` -> `+0x58` (alias if missing), sets `PathSpace+0x2C = 1`, registers both with `0x6AD270` | 0x4F2F80 listing | V |
| 0x442D30 | `arena_load_pathspace` (not this module) | loads `<name>.pfd` to `this+0x6A*4`, `bias = 0` | | V |
| 0x4797A0 | `sample_world_enter` | developer viewer loads `map\sample.pfd` | | V |
| 0x6AD270 | `scene_table_register(psA, psB, originX, originY, bounds[6])` | appends a 0x34-byte record unless one with the same `(psA, psB)` exists; then `0x68DBF0` | | V |
| 0x675660 | `pathsp_label_components` | sets `+0x70 = -1`, `+0x44 = 0` for all polygons, then flood-fills (explicit stack through `+0x44`, sentinel on the C stack) over neighbours with `flag != 1`, polygons iterated from the last to the first, ids from 0 | | V |
| 0x663CB0 | `poly_set_vertices(list, n, weight)` | stores `n`, copies the vertex-pointer list into `+0x30`, allocates the zeroed neighbour array `+0x34`, sets `weight (+0x64)`, **recomputes `bbox_min/max` from the vertices** | | V |
| 0x664130 | `poly_link(edge_a, poly_b, edge_b)` | `this.neighbours[edge_a-1] = poly_b`; if `edge_b != 0` also `poly_b.neighbours[edge_b-1] = this` | | V |
| 0x664080 | `poly_neighbour(i)` | `neighbours[i-1]` for `1 <= i <= n`, else NULL | | V |
| 0x6640D0 | `poly_edge_to(poly)` | 1-based edge index whose neighbour is `poly`, 0 if none | | V |
| 0x66B7C0 | `pathsp_autolink(upto)` | for polygons `> upto` links every unlinked edge to an unlinked reversed coincident edge of polygons `1..upto` | | V |
| 0x674590 | `pathsp_invalidate_bvh` | clears the BVH list | | V |
| 0x66B7A0 / 0x66B9F0 | `list_count` / `pathsp_poly_count` | list `+0x2C`; 0 if no polygon list | | V |

### 2.2 Spatial index (BVH over polygon boxes)
| address | name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x6745E0 | `pathsp_build_bvh` | if `bvh` empty and polygons exist: copies the polygon handles into a temp pointer array and builds with `0x6747A0(0, 0, n)` | | V |
| 0x6747A0 | `bvh_build_node(parent, lo, hi)` | node = union box of polygon boxes `[lo,hi)` + box of their centres; split axis = longest extent of the centre box (X unless Y or Z strictly longer); `hi-lo == 1` -> leaf (`+0x34 = poly`); all extents 0 -> split in the middle of the index range; else partition around the centre of the centre-box (`0x674FC0` with `0x6750B0/0x675100/0x675150`), recurse on both halves | | V |
| 0x674FC0 | `partition(arr, n, size, ref, cmp, tmp)` | moves elements with `cmp >= 0` to the tail, returns the head count | | V |
| 0x6750B0 / 0x675100 / 0x675150 | `cmp_x/y/z` | `2*floor((min+max)/2)` of the element's box against `min+max` of the reference box on that axis | 0x6750B0 only read | R |
| 0x6751A0 | `bvh_query_rect(min2, max2, out)` | pushes the polygons of all leaves whose x/y box overlaps the rectangle onto the scratch array | | V |
| 0x6752C0 | `bvh_query_box(min3, max3, out)` | same with z | | V |
| 0x675430 | `bvh_nearest(point2, &best_d2, &best_poly)` | branch-and-bound nearest polygon, node distance `0x663AD0` (point to AABB, squared), leaf distance `0x6730D0` converted with `ftol`; visits the nearer child first | | V |
| 0x663AD0 | `point_aabb_dist2(x, y, min, max)` | squared distance from a point to a rectangle (0 inside) | | V |

### 2.3 Point queries and geometry
| address | name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x6718C0 | `poly_contains(x, y, radius, override)` | see 4.3: bbox reject, edge half-plane tests, optional clearance of `radius` from wall edges (recursing into neighbours via 0x671CB0) | | V |
| 0x671CB0 | `poly_clearance_rec(x, y, id, radius)` | neighbour recursion of the clearance test, `visit_stamp` guarded | | V |
| 0x671FB0 / 0x672500 | `poly_wall_hit(...)` / recursion | finds the first wall edge closer than `radius` to the move target and returns the two end points of the blocking segment (0x672E40 picks the push-out vector) — used for wall-slide steering | 0x671FB0 | R |
| 0x672E40 | `wall_push_vectors` | from segment end points and the move point: either the segment itself or a perpendicular through the nearest end point, oriented by the side of the move | | R |
| 0x664D70 | `point_segment_dist2` | squared distance point -> segment | | V |
| 0x6730D0 | `poly_dist2(x, y)` | min over edges of `0x664D70`, initial 1e10 | | V |
| 0x664180 | `poly_height(x, y)` | type 0 + lattice: refined cell or coarse lattice (see pfd.md); otherwise plane through vertices, degenerate fallback along the first edge, `z[v1]` when the first edge is 0 | | V |
| 0x6645B0 | `poly_normal(out, x, y)` | type 0: normal from the bicubic sampler `0x4C7100` partials (two rotations about the axes); else plane through vertices 1..3 using `0x664180` for their heights | | R |
| 0x66CAE0 | `pathsp_height(poly, x, y)` | 0 if the handle is 0, else `poly_height` — the **ground height query** used by units/actors/particles | callers 0x4F2190, 0x4FEE60, 0x512830, 0x6AE8E0 | V |
| 0x66BA30 / 0x66BAA0 | `pathsp_poly_weight(i)` / `pathsp_neighbour_weight(i, edge, &out)` | weight (`+0x64`) of polygon / of its neighbour across an edge | | V |
| 0x66BB40 | `pathsp_poly_at(x, y, ?)` | box query around the point, returns the last polygon (scratch array iterated from the end) for which `poly_contains(x, y, arg, 0)` | | V |
| 0x66C020 | `pathsp_poly_at_z(x, y, z, radius)` | as above, but additionally requires `|poly_height(x,y) - z| <= 16` (`z-16 <= .. <= z+16`); `radius` is passed as the clearance radius | | V |
| 0x66C710 | `pathsp_nearest_poly(x, y)` | `bvh_nearest` from the root; 0 if none | | V |
| 0x66C900 | `pathsp_nearest_poly_b(x, y, bound_d2)` | as `0x66C710` but the initial best distance is a caller-supplied stack argument (nearest polygon within a bound) | | R |
| 0x6731D0 | `pathsp_floor_ceiling(x, y, z, radius, &poly_above, &h_above, &poly_below, &h_below, flag)` | all polygons containing the point; `below` = highest floor at or under `z`, `above` = lowest floor over `z` | | R |
| 0x673D00 / 0x673E20 | `pathsp_poly_within_hops` / `pathsp_nearest_within_hops` | recursive neighbour walk of `d` hops from a cached polygon: first polygon containing the point / nearest polygon | | V |
| 0x6736E0 / 0x6739C0 | `poly_segment_exit(&next_poly, &point, poly, a, b)` | Cyrus-Beck exit of segment a->b from a polygon; returns 0 = ends inside, 1 = leaves through an edge into `next_poly` (0 if wall) with the crossing point, 2 = no crossing / outside (0x6739C0 is the float/3-D version) | | R |
| 0x664B70 / 0x664C60 / 0x66DF30 / 0x66F910 | `seg_intersect_*` | integer / float segment-segment parameter solvers: write `(den, num_a, num_b)`, normalise the sign so `den > 0`, return `den != 0` | | V |
| 0x66FC50 | `frac_compare(a, b, c, d)` | sign of `a*d - c*b` in 64-bit (compares the fractions `a/b` and `c/d`) | | V |
| 0x66F820 | `portal_shrink(&x1,&y1,&x2,&y2, r)` | if `len² > 4*r²` (`r*r*_DAT_009CF79C`, 4.0) moves both end points inward by `r` along the segment; returns whether it did | | V |
| 0x663990 / 0x6639E0 / 0x663A30 / 0x663A80 / 0x66A380 / 0x66A3C0 / 0x66A400 / 0x66A440 / 0x66A4B0 / 0x66A4F0 / 0x66A530 | `Point2/Point3` helpers | add, sub, scale-to-int, copy, dot, cross, length², Point3 ctor, Point3 -> Point2, "within radius" | | V |

### 2.4 Search and route
| address | name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x66CB40 | `pathsp_find_route(from xyz, to xyz, owner, radius, start_poly, budget, p12, lane_offset, lane_width)` | public entry (thiscall on the PathSpace): see 4.4; returns 1 if the route reaches exactly the goal (x,y), else 0 (the route object still holds the partial path) | | V |
| 0x66CE30 | `pathsp_astar(start_poly, ..., stamp, owner, radius, budget, idx_sel, bias)` | the search itself, 4.4/4.5 | | V |
| 0x664920 | `node_init(stamp, parent)` | stamps the node, clears links, computes the node position (integer mean of the vertices' x, y, z), sizes the children array, `closed = 0`, attaches to the parent | | V |
| 0x664EB0 | `node_set_parent(new_parent)` | detaches from the old parent's children array, appends to the new one | | V |
| 0x664B00 | `node_not_worse(other)` | `cmp(this, other) <= 0` with `cmp` = `0x66FCA0` on (f, max_weight) | | V |
| 0x66FCA0 | `node_cmp(f1, acc1, max1, f2, acc2, max2)` | `max1 == max2 ? f1 - f2 : max1 - max2` (acc ignored) | | V |
| 0x66DD00 | `node_is_current(stamp)` | `search_stamp == stamp` | | V |
| 0x66DD30 / 0x66DD70 | `node_dist(x1, y1, x2, y2)` | integer Euclidean distance via `0x590410` -> `0x5903D0(dx, dy)` (integer hypot; TODO(re) read 0x5903D0) | | I |
| 0x66DDB0 | `node_propagate(bias_sel, bias)` | recomputes `g`, `f`, `acc`, `max_weight` of all descendants after a parent improvement (recursive), without re-sorting the open list | | V |
| 0x66E010 | `editor_pick(mask, x, y, r)` | editor selection: tries four object lists (`param_1[0..3]`, mask bits 1,2,4,8) and returns the bit of the first list that has an element within `r` of the cursor (`0x66A530` point proximity for list 0, `0x66E2A0` segment proximity for lists 1-3), element index through the out pointer. Editor-only | | R |
| 0x66E2A0 | `point_near_segment(x, y, r)` | distance of the point to a segment <= r (editor picking) | | R |
| 0x673F30 | `route_refine_through_mesh` | re-walks an existing route through the mesh (`0x6736E0`) inserting the edge crossings, then mirrors the second half of the points | | I |
| 0x6AC050 | `route_project(scene_table, n, scene, poly, route_in, route_out)` | converts a 2-D route into a ground-following 3-D polyline: segment by segment (`0x6736E0`), jumps to the neighbouring scene's space when the walk leaves the mesh (`0x6731D0`, scene bounds), collinear points (`(cross²/len²) < _DAT_009D56C8`, 9.0) are merged | | R |

### 2.5 Route object (`PathRoute`)
| address | name | purpose | conf |
|---|---|---|---|
| 0x66E4B0 | `route_ctor` | `dynarr(0x24, 0x10)` | V |
| 0x66E4E0 / 0x66E5B0 | `route_copy` | copy head + all records | V |
| 0x66E640 | `route_reset(a, b)` | clear storage; `head = a`; append `b` | V |
| 0x66E720 | `route_set_end(x, y, z)` | overwrite `S[1]` (goal / partial-path end) with an int point record | V |
| 0x66E790 | `route_push_portal(x1,y1,z1,x2,y2,z2)` | append record: float mid = (p1+p2)/`_DAT_009CF794` (2.0), ints p1, p2 | V |
| 0x66E840 | `route_push_record(rec)` | append a prepared record | V |
| 0x66E870 | `route_insert_front(rec)` | insert at storage index 1 | V |
| 0x66E8A0 | `route_count` | `n` | V |
| 0x66E8D0 | `route_point(i)` | logical point `i` (see PathRoute) | V |
| 0x66E940 | `route_stored_first` | `S[1]` | V |
| 0x66E970 / 0x66EB90 | `route_deserialize` / `route_serialize` | wire form | R |
| 0x66ECE0 | `route_smooth(radius, _, lane_offset, lane_width)` | funnel smoothing with look-ahead 8, see 4.7 | R |
| 0x66FA00 / 0x66FAA0 | `route_translate(dx,dy,dz)` / `route_scale(sxy, sz)` | in-place over head + records | V |
| 0x584620 / 0x584B30 / 0x5848D0 / 0x584A10 / 0x584580 / 0x417200 / 0x417AD0 | `ilib::dynarr` append / set(i) / insert(i) / remove(i) / clear / get(i) / count | generic container | V |

### 2.6 Editor-build code that ships inside `joan.exe` (not used at run time)
`0x667190` (`pathsp_build_from_lines`: turns "path lines" + "assist lines" into a triangulation, logs "Source path line", "Assist Line", "cannot form triangle on edge", "Normal finish, %d points, %d path lines, result in %d triangles"),
`0x66A640` (`pathsp_merge_polys`: merges neighbouring polygons of equal type and weight while the result stays convex and ≤ 32 vertices, rebuilds with `0x663CB0`), `0x669CC0`/`0x669E50` (helpers: point-line relations),
`0x66FB40` (marks a rectangle in a `spindex` grid), `0x665xxx`–`0x667xxx` (≈ 40 functions: typed `blkarrb` list wrappers for points, lines and an edit selection, destructor of the editor object `0x665000`),
`0x66B650`–`0x66B770` (static initialisers; `0x66B670` stores a constant into `DAT_00A6A274`). Documented only to rule them out.

## 3. Callers (what the unit code passes)
**`0x505DD0` unit_request_path(this = unit, x, y, z, budget, follow_flag, short)** is the one entry for all "go there" requests (MOVE_TO_TARGET 0x4BFA10 etc.); `0x4F7BF0` (sidestep) calls it with `budget = 100`.
The three ways `0x66CB40` is called (argument roles recovered from the call sites; p2..p4 = from xyz, p5..p7 = to xyz, p8 owner tag, p9 radius, p10 start polygon, p11 budget, p12..p14 smoothing):

| call | from | to | owner tag | radius (p9) | start poly (p10) | budget (p11) | p12 | p13 (lane offset) | p14 (lane width) |
|---|---|---|---|---|---|---|---|---|---|
| A (`0x505DD0` when the unit's block has no `+0x5C` route/track object, i.e. "whole-world" request) | `(unit.x, unit.y) / S`, z = 0 | `(x, y) / S`, z = 0 | `unit+0x473` | `BODY_R / S` (`DAT_00A3CC9C` = 32) | 0 (auto) | `budget` | `unit+0x426 / S` | `unit+0x41E / S` | -1.0 |
| B (`0x505DD0` block-local) | `unit.xyz - block.origin` | `target.xyz - block.origin` | `unit+0x432` | shape-derived (below) | cached ground polygon of the unit's physics body (`body+0x20E`) if its scene key (`body+0x212`) equals the current one, else 0 | `budget` | `unit+0x426` | `unit+0x41E` | 0.5 (shape branch) or -1.0 |
| C (sidestep `0x4F7BF0`) | `unit.xyz - block.origin` | `point.xy - block.origin`, z | actor-derived id (`0x5600C0`) | shape × 3.0 (`_DAT_009CF798`) if `unitres+2 == 8` and `unitres+0x118 != 0xB`, else `BODY_R` | 0 | 100 | 0 | 0 | 0.5 / -1.0 |

with `S = _DAT_009D0764` (TODO(re): value; call A also multiplies the whole result by 32.0 via `0x66FAA0(32.0, 1.0)` — S is therefore almost certainly 32.0, so call A runs in the
coarse world space) and, after the search, `0x66FA00(block.origin.x, block.origin.y, 0)` for B and C. A few B variants re-plan from the first/last segment of the existing route instead of from the unit position (p10 = 0).
Radius in call B: shape branch when `unitres+2 == 8`, or `unitres+0x118 == 2` (mounted), or predicate `0x430190`: the unit-res collision shape (`unitres+0x1C3`) type 1 -> `+0x1D3`, type 2 (box) -> max(|+0x1D7|, |+0x1E3|, |+0x1DB|, |+0x1E7|), type 3 -> `+0x1FB`,
times `_DAT_009CFC20` (10.0 per scene.md); otherwise `BODY_R * 2 = 64`. Return codes of `0x505DD0` (as far as visible): `0` = rejected (already moving on a route and the new one is not forced, point unreachable, ...),
`1` = a route exists (path space), `2` = the search produced an empty route, `3` = follower branch with an empty leader route.
The **"100"** that `docs/re/functions/unit.md` §10.4 calls "100 ms" is the **node-expansion budget** (p11), not a time.

Other callers: ground tracker `0x6AE8E0` (cached `(scene, polygon)` per entity, 4 hop-depth retries with `0x673D00`, then `0x6AAE70` over all scenes whose bounds contain the point with
`0x6731D0`, then `0x6AB1B0` nearest scene + nearest polygon `0x66C710/0x66C900`; accepts a polygon when `height - tol <= 0` or `height < z`, otherwise checks slope with `0x6645B0` and a unit-class dependent predicate `0x6ACCF0`), `0x6AC050` (route projection), `0x66E010` (space chooser).

## 4. Algorithms
### 4.1 Loading and post-processing (`0x670980`)
Open file (exists check `0x590B80` = `FindFirstFile`); read models, points, polygons, links, optional lattice (pfd.md). Per polygon `0x663CB0` recomputes the bbox from the vertices. After reading, polygons
with `flag != 1` get the **narrow test**: sum over edges that have a neighbour of the edge-derived length (`sqrt` via `0x7B2344`; the argument is lost in the export — presumably the edge length) and `if sum < 170 * n_portal_edges: flag = -1`.
Finally `0x675660` labels connected components. `PathSpace.bias` is set by the caller afterwards.

### 4.2 Spatial index
Lazy BVH (one polygon per leaf, nodes `0x38` bytes) built the first time a query needs it. Split axis = longest axis of the bounding box of the polygon **centres**; polygons whose centre on that axis is `>=` the middle of the centre-box
go to the right child. Queries: `0x6751A0` (2-D rectangle overlap -> list of candidate polygons), `0x6752C0` (3-D box), `0x675430` (nearest, squared distance, `0x7FFFFFFF` initial bound). The query rectangle built inside
`0x66C020/0x66BB40/0x6731D0` is derived from hidden arguments (TODO(re): probably the point ±`radius`).

### 4.3 Containment, clearance, height
`poly_contains(x, y, radius, override)`:
1. If `radius == 0` (override flag 0) and `flag == 1` -> not contained (blocked polygons are invisible to plain containment).
2. Reject if the point is outside the integer bbox in x or y (not enlarged by `radius`).
3. For every edge `i` (v_i -> v_{i+1}, wrapping): `cross = (x - xi)*(y_{i+1} - yi) - (y - yi)*(x_{i+1} - xi)`; if `cross > 0` the point is outside.
4. If `radius > 0`, for each edge: **passable edge** (neighbour exists and `flag != 1`): when the point is closer than `radius` to the edge (`0x664D70 < radius²`), recurse into the neighbour (`0x671CB0`, guarded by `visit_stamp`/`DAT_00A57632`) — the neighbour's own walls must also be farther than `radius`. **Wall edge** (no neighbour or blocked neighbour): if closer than `radius`, the point is rejected, **except** when the wall is an axis-aligned edge lying exactly on the block's boundary line (`bounds.min/max - origin`, scene-table record found by `0x4A2B20`; if the owner is not in the scene table every close wall rejects), i.e. block borders do not count as walls.
5. Success returns 1.
Heights: `poly_height` (pfd.md). Plane fallback: three-vertex plane, if the edge-vector determinant is 0 and the first edge is 0 -> z of the second vertex, else projection onto the first edge.

### 4.4 Search (`0x66CB40` / `0x66CE30`)
Setup (`0x66CB40`): clears the route; `start = p10 ?: pathsp_poly_at_z(from, radius) ?: pathsp_nearest_poly(from)`; if still 0 -> return 0 (route stays empty). If `stamp == 0`: set every `search_stamp` to 0 and `stamp = 1`. Take `id = stamp++`.
Reset the route with `head = from` (ints, truncated) and the first stored record = `goal` (ints); then `0x66CE30`.
* `radius < _DAT_009CFCA0` is treated as 0 (TODO(re): value).
* **Trivial case**: the goal (x,y) lies in the start polygon (`poly_contains(goal, 0, 0)`) -> done, route = `start -> goal`.
* **Start node**: `node_init`, `g = 0`, `h = dist(from, goal)` (integers), `acc = weight`, `max_weight = max(0, weight)`.
* **Cost function**: `f = g + h - bonus`, `bonus = (|bbox_max.x - bbox_min.x| + |bbox_max.y - bbox_min.y|) * K * bias`, `K = (index % 3 != idx_sel) ? 4 : 6`. `idx_sel = -1` in the only call, so **K = 4**; `bias = PathSpace.bias` (1 in scene spaces,
  0 in the arena). The bonus makes the search prefer big polygons (fewer, longer portals). Edge cost: `g(nb) = g(n) + dist(centroid(nb), P)` where `P` = the start point if `n` is the start polygon, else `centroid(n)`; `h(nb) = dist(centroid(nb), goal)`; all integer. `acc(nb) = acc(n) + weight(nb)` on first touch (**an original quirk: on later improvements the code adds `acc(nb)` instead of `weight(nb)`; `acc` is never compared, harmless**); `max_weight(nb) = max(weight(nb), max_weight(n))`.
* **Open list**: singly linked through `+0x44`, sorted ascending by the comparator `(max_weight, f)` — lower worst-weight tier first, then lower `f`. Insertion of a new node: at the head if it is `<=` the head, else after the last element it is strictly greater than (so equal keys: newest first). Each loop iteration pops the head and decrements the budget.
* **Expansion** of node `n`: first the goal test (`poly_contains(goal, 0, 0)` -> success). Then for every edge: neighbour `nb` must exist, differ from `n.parent`, have `flag != 1`, and (`flag != -1` or `radius <= 0`). A neighbour not yet stamped with this search is initialised and inserted. A neighbour already seen is **re-parented when strictly better** by the comparator: `parent`, `g`, `f`, `acc`, `max_weight` updated; if not closed it is removed and re-inserted (this time with the plain `f <=` head test), and its subtree is updated by `0x66DDB0` (the open list is not re-sorted for descendants).
  After the edges, `n` is marked `closed` and pushed on the closed list.
* **Termination**: open list empty or budget exhausted -> failure handling below. Closed nodes are never re-opened.
* **No heuristic admissibility guarantee**: `h` is the straight distance from the polygon centre, `f` has a negative size bonus and the weight tiers dominate; the result is a good, not shortest, path.

### 4.5 Route assembly
Walk from the final node to the start through `parent`; for every step find the edge(s) of the node whose neighbour is the parent and append a portal record (`0x66E790`: ends = vertices `i` and `i+1` of that edge, float midpoint). The route is therefore stored goal-side first (see PathRoute).
* **Success**: returns 1; `0x66CB40` then rewrites `S[1]` = goal (ints), runs the smoothing, and returns whether the (truncated) goal x,y equals the requested goal exactly (it normally does not for float goals; callers use the route length instead).
* **Failure / budget exhausted (partial path)**: choose `best` = node with the minimum **`h + g/16`** (truncating division) among the closed list (most recently closed first) and then the open list (first minimum wins). `S[1]` is set to the best node's centroid (`0x66E720`), then, for every edge of `best`, the ray from its centroid towards the goal is intersected with the edge (`0x66DF30`); the intersection with the largest ray parameter (`0x66FC50`) replaces `S[1]` (z interpolated along the edge with `0x5921A0`), i.e. **the route ends where the straight line to the goal leaves the best polygon**. Then the portals back to the start are appended as in success. Return value 0, route non-empty, smoothing still applied. `0x505DD0` accepts such a partial route.
* The search never crosses into another block's space; leaving a block is done by the route projector / movement code, not here. (TODO(re): how `ps_world` and `ps_level` are chosen for a given call: the `this` register is not in the export.)

### 4.6 Collision radius handling (summary)
Used in four places only: (1) start-polygon selection (`poly_contains` with clearance, so the unit must stand `radius` away from walls), (2) narrow polygons (`flag -1`) are skipped when `radius > 0`, (3) the goal test uses radius 0, (4) smoothing shrinks every portal by `radius` at both ends (`0x66F820`) so waypoints keep `radius` away from the portal posts; if a portal is not wide enough (`len <= 2r`) its vertex stays at the portal midpoint. There is no per-unit-class nav layer: different sizes differ only in these four effects.

### 4.7 Smoothing (`0x66ECE0`) — funnel with look-ahead (weakest section)
Inputs: radius `r`, `lane_offset d = p13 * K2`, `lane_width W = p14 * K2` (`K2 = _DAT_009DBE94`, TODO(re): value; call sites pass `d = unit.formation_depth`, `W = 0.5` or `-1`).
For `i = 1 .. n-1` (never the head and never the goal): `prev` = already smoothed point `i-1`. The portal of vertex `i` relative to `prev` is `(L, R)`; both ends are shrunk by `r` (`0x66F820`); if no shrink was possible the vertex is left on its stored midpoint. Otherwise a funnel is run over the next up to 8 portals `j = i+1 .. min(i+8, n)` (each also shrunk, or replaced by its midpoint when too narrow): the left and right bounds are narrowed to the intersection with each later portal (`0x66F910`); if a later portal lies completely to one side of the funnel the walk aims at that end of portal `i` (parameter `t = 0` or `t = 1`), if the funnel stays open it aims at `t = 0.5`; when the funnel would narrow below `W/2` (and `W != 0`) the look-ahead stops. The lane parameters offset `t` across the corridor: `t' = clamp(0.5 - d/W, 0, 1)` measured from the chosen end (the clamp helper is `0x590B00(x, lo, hi)`), scaled by `W / width`; for the centred case the shift is clamped to ±0.5. The aim point is `L + (R-L)*t`. The waypoint is finally the intersection of the ray `prev -> aim` with the **unshrunk** portal (`0x66F910`), accepted when its fraction along the portal is `< 1`, and its x, y, z are interpolated along the portal's integer end points; otherwise the stored midpoint stays.
Last pass (walking from the end): a point whose x **and** y are within `_DAT_009D3E18` of the previous point is deleted (`0x584A10`), and the penultimate point is deleted if it coincides with the goal. (`_DAT_009D3E18` appears as 0.5 in this function's use as a funnel parameter and as the dedupe epsilon; [ui2d.md] lists the same address as 0.0 — TODO(re): read the double.)

### 4.8 z / layers
Polygons of different levels (bridge over road) overlap in x,y. The search itself ignores z; the start polygon is the one whose `poly_height` is within ±16 of the unit's z (`0x66C020`), else the 2-D nearest; the goal polygon is the first one reached that contains the goal in x,y (no z test). Floor/ceiling selection for ground height is `0x6731D0` (below/above) and the ground tracker.

### 4.9 Dynamic obstacles
None in the search. Run-time changes happen only through `PathPoly.flag` (blocked = 1) and the `model_id` link (something toggles polygons of a closing/opening object; writer not in the export), after which nothing is rebuilt (the BVH only depends on boxes; components are not re-labelled — `0x675660` runs only at load). Moving units are handled by physics and the sidestep logic (`0x4F7BF0`, picks random side points and re-requests a path with budget 100).

## 5. Constants
| value | meaning | where |
|---|---|---|
| 32 (0x20) | max models per file; max vertices per polygon | 0x670980, 0x66A640 |
| 0x82 | model entry stride (u16 len 0x80 + char[0x80]) | pfd.md |
| 0x38 / 0x74 / 0x24 / 0x34 / 0x41 | BVH node / polygon / route record / scene record / PathRoute sizes | structs |
| 0x800 | blkarrb elements per block (points 0xC, BVH 0x38) | 0x6745E0, 0x665480 |
| 170 (0xAA) | narrow-polygon threshold (mean portal length) | 0x670980 |
| ±16 (0x10) | z tolerance when picking the start polygon | 0x66C020 |
| K = 4 (6 if `index % 3 == idx_sel`; `idx_sel = -1` always) | size-bonus multiplier | 0x66CE30 |
| `h + g/16` | partial-path score | 0x66CE30 |
| 8 | funnel look-ahead (portals) | 0x66ECE0 |
| 100 | node-expansion budget passed by `0x4F7BF0`/`0x505DD0` callers | 0x4F7BF0 |
| `BODY_R = 32` (`DAT_00A3CC9C`) | default collision radius (call B uses `<<1` = 64) | flocksys.md §9.2 |
| 0.0 / 1.0 / 0.5 / 2.0 / 3.0 / 4.0 / 9.0 | `_DAT_009CF5C4 / 009CF784 / 009CF77C / 009CF794 / 009CF798 / 009CF79C / 009D56C8` | other docs, plasma.h |
| 32.0f / 2.5f / 200.0f / 10.0f | `_DAT_009D0764` (S, world-space scale) / `_DAT_009DBE94` (K2) / `_DAT_009CFCA0` (radius below which it counts as 0) / `_DAT_009CFC20` (shape factor) — floats, read from .rdata (main session, 2026-10-23) | |
| 0.5 / 0.0 / 1.0 / 4.0 / 30.0 (doubles) | `_DAT_009D3E18` (dedupe epsilon; ui2d.md's 0.0 was the float read of the low word) / `_DAT_009D8BC0` / `_DAT_009CF5E8` / `_DAT_009CF788` / `_DAT_009DD790` | |
| 102400.0f | `_DAT_009D7510` (= 320², leader distance² in 0x505DD0) | |
| `isqrt(dx² + dy²)` | `0x5903D0` integer distance: `0x590340` floor integer square root of the 32-bit (wrapping) sum | |

## 6. Cross-module requests
* Ghidra: rename per table above (`pathsp_*`, `route_*`, `bvh_*`, `poly_*`); create types `PathSpace` (0x38), `PathPoly` (0x74), `PathRoute` (0x41) + `RouteRec` (0x24), `SceneRec` (0x34); set `__thiscall` + `this` on `0x66CB40`, `0x66CE30`, `0x6718C0`, `0x664180` etc. so the export keeps the arguments.
* `docs/re/structs/World.md` (WorldBlock): `+0x43` u8 flag, `+0x44/+0x48` origin x/y (i32), `+0x54` `PathSpace*` level, `+0x58` `PathSpace*` world, `+0x5C` route/track object (`0x4EB4F0`), `+0x60` camera/other (`0x425DC0`).
* `docs/re/structs/Unit.md`: `+0x432` and `+0x473` embedded [PathRoute](../structs/PathRoute.md); `+0x40E` remaining route length; `+0x41E` formation depth; `+0x426` slot y.
* `docs/re/functions/unit.md` §10.4: "a 100 ms path" is a **100-expansion budget**. `docs/re/functions/flocksys.md` §9.2: route index 0 is the start point, `0x66E8D0` order explained in PathRoute.
* `docs/formats/map.md` / `world_binary.md`: the `.pfd` reader is **0x670980**, found by its callers (`0x4F2F80`, `0x442D30`, `0x4797A0`), not by the extension string (the extension is a concatenated fragment `DAT_009D8038`).
* `docs/re/structs/Plasma.md`: the same lattice class carries the pfd height grids (`PathSpace+0x30/+0x34`).
* `TODO(re)`: `_DAT_009D3E18` is read as 0.0 elsewhere; this module's use suggests 0.5. Re-read the constant.
* Ground height queries (`0x66CAE0`) are served by the same mesh, not by the terrain grid (4.2–4.7 describe the search).

## 7. Open questions / export requests
* TODO(re): `0x66ECE0` — signs of the funnel cross products, which `local_80` branch uses which of the constants `0x9D8BC0 / 0x9D3E18 / 0x9CF5E8`, the exact lane formula and the second dedupe condition. Needs disassembly or a Binary Ninja HLIL paste.
* TODO(re): `field8` of a polygon and the origin of `weight (+0x64)`; who sets `flag = 1`.
* RESOLVED (disassembly 0x5062C6..0x507C62): call A uses `unit+0x335 -> +0x6A` (the world's coarse space, scaled by S = 32); the block-local calls take `this = 0x5600C0(block = unit+0x339, unit+0x102)`: `ai_flag != 0` -> the level space `+0x54` (else `+0x58`), `ai_flag == 0` (the player's unit) -> `+0x58` (else `+0x54`). For a scene without its own `<scene>w.pfd` both pointers are the same level space.
* Export requests: `0x5903D0` (integer hypot), `0x4A3790` (compare for `0x4A36C0`), `0x4A36C0` callers/`this`, `0x4A2B20` (scene-table lookup), `0x6AAC70`, `0x6ACCF0`, `0x60D5F0` (point-in-block test used by 0x505DD0), `0x523E50` (reachability predicate; probably compares component ids), `0x5600C0`, `0x4EEB80/0x4EEA60/0x4EECA0` (Point2 helpers), `0x43C420`, `0x68DBF0`, `0x4EB4F0..0x4EB810` (the `.por` route objects) and `.rdata` 0x9D0764, 0x9DBE94, 0x9CFCA0, 0x9CFC20, 0x9D3E18, 0x9D8BC0, 0x9CF5E8, 0x9CF788, 0x9DD790, 0x9D7510 (float values), globals 0xA57632 (stamp), 0xA57636/0xA57BB2 (scene table).
