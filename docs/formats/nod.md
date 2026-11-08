# `.nod` — building navigation triangles (`item/Object/**/<model>.nod`, 46 files, 132 B..148 KB)

**Reader not identified** (see [map.md](map.md) for what was searched). The layout below was derived from `Lc_01.nod` and **now fits all 46 shipped files
byte-exactly**: every file is consumed to its last byte. The file extension appears in upper case (`.NOD`) for most
files; file names are matched ignoring case, see [vfs.md](vfs.md). Meaning (walkable triangles of a building plus shared-edge table) is inferred.

## Layout
| item | size | notes |
|---|---|---|
| `n_polys` | u32 | 2 .. 2,198 (sp_cliff.NOD); 4,728 polygons in all 46 files |
| polygon[n_polys] | variable | below |
| `n_edges` | u32 | shared-edge count (1 .. 3,092) |
| edge[n_edges] | 16 B each | `u32 poly_a, edge_a, poly_b, edge_b` |
| `trailer` | u32 | 0 in every file (a second count of an empty list? TODO(re)) |

Polygon:
| size | field | evidence |
|---|---|---|
| u32 | `n_verts` | 3 in all 4,728 polygons (triangles) |
| n_verts * 12 | `i32 x, y, z` vertices, model-local units | (Lc_01: x -2545..1691, y -147..267, z about 465); `z` takes 1 .. 704 distinct values per file (constant in the small ones) |
| u32 | `n_adj` | 0..2 observed (always `<= n_verts`) |
| n_adj * u32 | edge numbers 1..3 | **boundary edges**: the edges of this triangle that have no neighbour |

Edge table: `poly_a`, `poly_b` are **1-based** polygon indices, `edge_a`, `edge_b` are 1-based edge numbers (1..3) of that triangle; the pair says "edge `edge_a` of polygon
`poly_a` is the same edge as `edge_b` of `poly_b`". Consistency check (all 4,728 polygons, 46 files): the set of edge numbers a polygon appears with in the edge table
and its `boundary edges` are disjoint and together equal {1,2,3}, except two polygons of `St.Pierre/sp_cliff.NOD` (polygons 46 and 820, which lack one boundary-edge entry).
(The earlier guess that the adjacency list held neighbour polygon indices was wrong: Lc_15.nod polygon 1 has `n_adj = 2, [1, 2]` and shares only its edge 3.)

Size relation (verified, all files): `size = 4 + sum_p(4 + 12*3 + 4 + 4*n_adj_p) + 4 + 16*n_edges + 4`.

Relation to `.pfd`: `.pfd` lists model paths without extension; 40 of the 239 listed paths have a `.nod` beside the `.smo` in `item/Object/Models_Unique/…` or `Models_Items/Buildings/…`.

## Open questions
- TODO(re): reader, edge-number convention (edge k = vertex k-1 -> k?), meaning of `trailer`, whether `z` is a floor height or an elevation of a ramp.
