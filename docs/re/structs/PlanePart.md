# PlanePart (transform node, `sizeof` = 0xA0)

Element of `Plane::parts`. A node of a small transform hierarchy (bones of skinned models, sub-objects of static ones).
Serialised form: the **first 0x40 bytes only** (the local matrix). This corrects [PlaneChunks.md](PlaneChunks.md) step 3,
which called those 0x40 bytes a "part name": the copy loop in `0x5CBBF0` copies 16 dwords per part and `0x60EE60` multiplies
them as a matrix.

Constructor `0x60EB90`, destructor `0x60EC40` (array helper `0x5CCA00`). Update logic `0x60EE60`, accessor `0x60EF80`.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 0x40 | D3DMATRIX | local | local transform relative to parent (row-major, row-vector convention) | 0x60EE60, 0x5CBBF0 |
| 0x40 | 4 | PlanePart* | parent | | 0x60EC70 |
| 0x44 | 4 | PlanePart* | prev_sibling | | 0x60EC70 |
| 0x48 | 4 | PlanePart* | next_sibling | | 0x60EC70 |
| 0x4C | 4 | PlanePart* | first_child | | 0x60EC70 |
| 0x50 | 4 | PlanePart* | last_child | | 0x60EC70 |
| 0x54 | 4 | i32 | dirty | world matrix stale; set on all children when a node is recomputed | 0x60EE60 |
| 0x58 | 0x40 | D3DMATRIX | world | `local * parent.world` (or `local` copy for roots) | 0x60EE60 |
| 0x98 | 4 | fnptr | on_update | called with `arg` after the world matrix was recomputed | 0x60EE60 |
| 0x9C | 4 | void* | on_update_arg | | 0x60EE60 |

`0x60EC70(parent)` unlinks the node from its current parent and appends it to the new parent's child list.
`0x60EE60()` recursively refreshes the parent first (if the parent became dirty the child is flagged), then recomputes
`world`, calls the callback and flags every child dirty. `0x60EF80()` = "refresh and return &world".
