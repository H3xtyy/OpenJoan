# SpatialIndex (uti3d `spindex.cpp`) — uniform 2D grid over a rectangle

Derived from `ilib::dynarrb` (dtor `0x585710`; the base class is a handle array of *regions*, accessed through
`0x417200(handle)` get / `0x417ad0()` count). Purpose is inferred from names and shape (no caller in the scene export
queries it): a coarse grid where each cell stores 8 bytes (presumably `{first, count}` into region lists). Use is likely in
`world`/AI code (TODO(re): find callers of 0x681FE0).

## Grid header (object fields)

| off | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | u32 | cell_size | grid cell edge in world units | 0x681FE0 |
| 0x04 | 4 | u32 | width | covered width | 0x681FE0 |
| 0x08 | 4 | u32 | height | covered height | 0x681FE0 |
| 0x0C | 4 | u32 | cols | `ceil(width / cell_size)` = `(w-1+cs)/cs` | 0x681FE0 |
| 0x10 | 4 | u32 | rows | `ceil(height / cell_size)` | 0x681FE0 |
| 0x14 | 4 | void* | cells | `rows*cols*8` bytes, zeroed | 0x681FE0 |

`0x681FE0(w, h, cell)` = (re)initialise: calls `0x6820D0` (clear: free all regions that have `owner == 0`, drop `cells`,
zero size fields), stores the fields, allocates and zeroes `cells`, then `0x585D20` (clears the handle array).
`0x681EE0` = destructor (same sweep, frees `cells`, base dtor).

## Region record (0x58 bytes, element of the handle array)

| off | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | u32 | id | copied from the first arg pair | 0x683910 |
| 0x04 | 1 | u8 | kind | | 0x683910 |
| 0x08 | 4 | void* | owner | free-slot marker: `0x683650` returns "free" when 0; set by callers | 0x683650 |
| 0x0C | 16 | 4 x 32-bit | bounds | four values passed in (likely float min/max x/y) | 0x683910 |
| 0x1C | 16 | i32[4] | cell_rect | inclusive cell rectangle `x0,y0,x1,y1` | 0x683910, 0x683BB0 |
| 0x2C | 4 | i32* | list_a | per-cell array `(x1-x0+1)*(y1-y0+1)` ints; NULL when it fits the inline array | 0x683910 |
| 0x30 | 16 | i32[4] | list_a_inline | | 0x683910 |
| 0x40 | 4 | i32* | list_b | second per-cell array, same size | 0x683910 |
| 0x44 | 16 | i32[4] | list_b_inline | | 0x683910 |
| 0x54 | 4 | i32 | inline_cap | 4 initially; replaced by the cell count when larger | 0x683910 |

`0x683910` constructs, `0x683BB0` re-shapes an existing record (realloc both arrays, zero them), `0x683B00` frees both
heap arrays and resets `inline_cap = 4`. The two arrays are zero-initialised per cell on every (re)shape: they look like a
visited/occupancy mark plus a value/height per cell (TODO(re)).
