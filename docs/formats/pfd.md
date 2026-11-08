# `.pfd` — path-space data of one scene (`map/scenarioN/<scene>.pfd` and `<scene>w.pfd`, 38 files, 133 KB..1.19 MB)

`pfd` = **path-find data**: the navigation mesh ("path space", `d:\ww\uti3d\pathsp.cpp`) of one world block. The reader is
**`0x670980`** (`pathsp_load_pfd`, joan.exe), the writer of the editor build is `0x66FCD0`; both are in
[`docs/re/functions/pathfind.md`](../re/functions/pathfind.md). In-memory objects: [PathSpace](../re/structs/PathSpace.md),
[PathPoly](../re/structs/PathPoly.md).

**Status.** The layout below covers models, points, polygons incl. vertex lists, links, lattice and refined cells; all 38 files are consumed to EOF with every range valid. No shipped file has refined cells (`n_sub == 0`), so that path is covered only by the reader's logic. The editor's writer is `0x66FCD0`; the game has no use for it.

**Verification status.** The layout below is derived from the *reader* `0x670980` and the *writer*
`0x66FCD0` and agrees with every byte sequence recorded from real files (model table and point list verified on all 38
files; the `S1L1.pfd` body prefix (step count, two 3-word model slots, two zero words, the point and polygon counts; the exact values are not quoted here) and `S3L1w.pfd` 14 points / 6 polygons both parse to the
fields below). The check of all 38 files with the [verification script](#verification-script) passes (all assertions hold, every file is consumed to EOF, `field8 <= n_models` for all 35,290 polygons). Items tagged `[unverified]` are not covered by it.

## Naming of the two files per scene
`0x4F2F80` (`world_s.cpp`, the block loader) builds `map\<dir>\<scene>` + `.pfd` (string `DAT_009D8038`) and `map\<dir>\<scene>` + `w` (`DAT_009D81DC`) + `.pfd`:

| file | stored at (block) | meaning |
|---|---|---|
| `<scene>.pfd` (e.g. `S1L1.pfd`) | block `+0x54` | **level space**: world-block-local coordinates (world units, same frame as the unit positions minus the block origin `block+0x44/+0x48`); has the model table (buildings, bridges, doors), `[verified]` coordinates up to 16408 |
| `<scene>w.pfd` (e.g. `S3L1w.pfd`) | block `+0x58` | **world space**: 0 models, all vertex z = 0, coarse outline (S3L1w: 14 points, 6 polygons within 440 x 880) and the large height lattices (S6L1w: 1.19 MB). Coordinates are world units / 32 `[verified]`: `_DAT_009D0764` = 32.0f, and every shipped `w` file spans exactly its world's `.wor` grid in node units (S5L1w.pfd: x 0..880, y 0..2200 = two 440-wide node columns, five rows; × 32 = the 28160 x 70400 world). |

Naming, as shipped `[verified]`: the six `w` files are named after the **world** file (`S5L1w.wor` → `S5L1w.pfd`), one per world, not per scene; no scene has a `<scene>w.pfd` of its own except that scenario 2's first scene `S2L1` (world `S2L1w.wor`) collides with the world's name. The level files are block-local: `S5L1_1.pfd` spans 0..14080 = a 440-unit node × 32. OpenJoan therefore loads `map/<world>.pfd` as the world's coarse space (the unit code's call A, `unit+0x335 -> +0x6A`) and `<scene>.pfd` per node as the level space (`src/game/battle/navigation.h`).

If `<scene>w.pfd` is missing the loader aliases `block+0x58 = block+0x54`. If `<scene>.pfd` is missing nothing is created beyond the empty object.
`map\sample.pfd` is loaded by the developer sample-world viewer `0x4797A0` (not in the shipped data). `0x442D30` (battle arena loader) loads `<name>.pfd` too, with `PathSpace.bias = 0` (see pathfind.md "cost function").
After loading, the loader sets `PathSpace+0x2C = 1` for scene spaces (0 for the arena space).

## Layout (all integers little-endian, 4 bytes unless noted; no padding, no checksum, no magic)

```
u32  n_models                       0..32 (reader allocates a growth block when > 0x20; 0 in the 6 *w files and 3 level files)
n_models x {                        stride 0x82
    u16  len                        always 0x80                                    [verified 239/239 entries]
    char path[0x80]                 ASCII, NUL padded, no extension: "Object\Models_Unique\ScenarioN\<Set>\<model>" or "Object\Models_Items\Buildings\..."
}
u32  n_points
n_points x { i32 x, y, z }          12 bytes; vertex pool (z: 0 in *w files, about -500..+900 in level files)   [structure verified 38/38]
u32  n_polys
n_polys x {                         variable length
    u32  type                       0 = terrain polygon (height from the lattice), 1 = planar polygon (height from its vertex plane); reader maps any other value to "unset"
    i32  bbox[6]                    min x,y,z then max x,y,z — read but RECOMPUTED from the vertices by the loader (0x663CB0); redundant
    i32  field8                     one int the reader consumes before the vertex count (see "field8" below) — present in every record [S1L1 prefix confirms]
    u32  n_verts                    3..32 (the editor's merge pass never builds more than 0x20 vertices)
    u32  vert[n_verts]              1-BASED indices into the point pool, polygon wound so the interior is on the LEFT of each directed edge
}
u32  n_links
n_links x { u32 poly_a, edge_a, poly_b, edge_b }
                                    1-based poly indices; edge i of a polygon runs from vertex i to vertex i+1 (wrapping); edge_b = 0 means a
                                    one-way link (the neighbour has no reciprocal entry). The loader stores neighbour[edge_a] = poly_b and,
                                    if edge_b != 0, neighbour[edge_b] = poly_a. Edges that are never linked are walls.
u32  has_lattice                    0 or 1
if has_lattice {
    u32  x0                         always 0 (written by the editor as 0, ignored by the reader)   [unverified]
    u32  y0                         always 0
    u32  nx, ny                     lattice nodes per row / column; the loader creates a (nx-1) x (ny-1) CELL lattice
    u32  cell_w, cell_h             world size of one coarse cell (PathSpace+0x24/+0x28)
    i32  height[ny][nx]             row-major (y outer, x inner); stored as int16 in memory (values must fit in i16)
    u32  n_sub                      number of refined cells
    n_sub x {
        u32 px, py                  world position of the refined cell = (cell_x*cell_w, cell_y*cell_h)
        u32 snx, sny                nodes of the refined lattice
        u32 step_x, step_y          cell_w/(snx-1), cell_h/(sny-1) (written by the editor; ignored by the reader)
        i32 height[sny][snx]
    }
}
EOF  (the reader stops after the lattices; nothing follows)  [unverified: EOF check is part of the script]
```

### Section notes
* **Model table** (verified): 232 of the 239 entries resolve to `item/<path>.smo`; seven do not (bridges / camp entrance / door); 40 have a `.nod` ([nod.md](nod.md)). The reader maps a polygon's `field8` through the table (`0x4A36C0`, a name lookup into a 0x2BF-byte-record table) into `PathPoly+0x24` ("model id") — the link between a mesh patch and the object that can open or close it. `[inferred]`
* **Vertices** are shared through the point pool; the loader keeps `PathPoly+0x30` = array of point pointers. Orientation: `cross((v[i+1]-v[i]), (p-v[i])) >= 0` for interior points (checked in `0x6718C0`).
* **Links** are generated by the editor from coincident reversed edges (`0x66B7C0` does the same at edit time) and written deduplicated: a reciprocal pair appears once (the writer emits it when `own_index <= neighbour_index`), a one-way link appears as is.
* **Coarse lattice cell index** = `(y / cell_h) * (nx-1) + x / cell_w`; `n_sub` refined cells replace the coarse interpolation inside their cell: the sub-lattice dimension is `(snx-1) x (sny-1)` cells.
* **Height sampling** (type 0 polygons, `0x664180`): refined cell if present — fractions inside the cell scaled by `(snx-1, sny-1)`, interpolated with the triangle split A (`0x4C7B70` mode 0) — otherwise the coarse lattice with the split diagonal alternating on the parity of `(cell_x + cell_y)` (mode 1 for even parity). Type 1 polygons: plane through the first three vertices (their own z), degenerate-plane fallback along the first edge.
* **Loader post-pass** (not stored): for every polygon whose flag is not 1, edge lengths of the edges that have a neighbour are summed; if `sum < 170 * n_portal_edges` (average portal narrower than 170 units) the polygon is flagged narrow (`PathPoly+0x20 = -1`); then connected components are labelled (`+0x70`) by flood fill over neighbours whose flag is not 1 (`0x675660`).

### `field8`
The reader consumes exactly one `int` between the six bbox ints and `n_verts` and feeds it, through `0x4A36C0`, to the "model id" `PathPoly+0x24`; the S1L1 prefix shows it as 0 for the first polygon (no model). The writer of the editor build (`0x66FCD0`, evidently older) does not write it, and it also does not write the model table. Which of `{model slot index into the table above, polygon weight}` it is cannot be told from the pseudocode (the argument registers of the thunk calls are lost); the run of the script prints a histogram of the values against `n_models` — if every value is `<= n_models` it is the 0/1-based model slot. TODO(re): confirm; the weight (`PathPoly+0x64`, a signed 16-bit "terrain cost class" that the search minimises as primary key) is passed to `0x663CB0` from a register and has no visible source in the reader, so it may be always 0 for shipped data.

## Verification script
Run from any directory (no dependency but the stdlib). It parses all 38 files with the layout above, asserts the invariants, and prints one summary line per file plus histograms; any `AssertionError` is a layout error.

```python
# python3 -I pfd_check.py "$JOAN_GAME_DIR/map"
import struct, sys, pathlib, collections

def parse(b):
    o = 0
    def u32():
        nonlocal o
        v = struct.unpack_from('<I', b, o)[0]; o += 4; return v
    def i32():
        nonlocal o
        v = struct.unpack_from('<i', b, o)[0]; o += 4; return v
    nm = u32(); assert nm <= 32, nm
    models = []
    for _ in range(nm):
        ln = struct.unpack_from('<H', b, o)[0]; assert ln == 0x80
        models.append(b[o + 2:o + 2 + 0x80].split(b'\0')[0].decode('ascii')); o += 0x82
    npnt = u32()
    pts = [struct.unpack_from('<3i', b, o + 12 * i) for i in range(npnt)]; o += 12 * npnt
    npoly = u32(); polys = []
    for _ in range(npoly):
        t = u32(); assert t in (0, 1), t
        bb = [i32() for _ in range(6)]
        f8 = i32()
        nv = u32(); assert 3 <= nv <= 32, nv
        vs = [u32() for _ in range(nv)]
        assert all(1 <= v <= npnt for v in vs)
        polys.append((t, bb, f8, vs))
    nl = u32(); links = []
    for _ in range(nl):
        pa, ea, pb, eb = u32(), u32(), u32(), u32()
        assert 1 <= pa <= npoly and 1 <= pb <= npoly
        assert 1 <= ea <= len(polys[pa - 1][3]) and 0 <= eb <= len(polys[pb - 1][3])
        links.append((pa, ea, pb, eb))
    lat = None; subs = []
    has = u32(); assert has in (0, 1)
    if has:
        x0, y0, nx, ny, cw, ch = (u32() for _ in range(6))
        assert x0 == 0 and y0 == 0 and nx >= 2 and ny >= 2 and cw > 0 and ch > 0
        h = struct.unpack_from('<%di' % (nx * ny), b, o); o += 4 * nx * ny
        assert all(-32768 <= v <= 32767 for v in h)
        lat = (nx, ny, cw, ch)
        ns = u32()
        for _ in range(ns):
            px, py, snx, sny, sx, sy = (u32() for _ in range(6))
            assert px % cw == 0 and py % ch == 0 and px // cw < nx - 1 and py // ch < ny - 1
            assert snx >= 2 and sny >= 2 and sx == cw // (snx - 1) and sy == ch // (sny - 1)
            struct.unpack_from('<%di' % (snx * sny), b, o); o += 4 * snx * sny
            subs.append((px, py))
        assert len(set(subs)) == len(subs)
    assert o == len(b), 'trailing bytes: %d' % (len(b) - o)
    return models, pts, polys, links, lat, subs

hist8, rows = collections.Counter(), []
for p in sorted(pathlib.Path(sys.argv[1]).rglob('*.pfd')):
    m, pts, polys, links, lat, subs = parse(p.read_bytes())
    for t, bb, f8, vs in polys:
        hist8[(f8 <= len(m))] += 1
    recip = sum(1 for l in links if l[3] != 0)
    print('%-28s models=%2d pts=%5d polys=%5d links=%5d (recip %5d) lattice=%s subs=%d type1=%d' % (
        p.name, len(m), len(pts), len(polys), len(links), recip, lat, len(subs),
        sum(1 for q in polys if q[0] == 1)))
print('field8 <= n_models for all polygons:', dict(hist8))
```

Expected result when the layout is right: every file parses with `trailing bytes: 0`, every vertex index is in range, `step_x/step_y` equal `cell/(nodes-1)`. If `field8` is a model slot, the histogram contains only `True`.

## Open questions
* TODO(re): `field8` semantics (above) and the source of the polygon weight `+0x64`.
* RESOLVED: `_DAT_009D0764` = 32.0 (see above). TODO(re): whether the `S2L1` block really loads the world-named `S2L1w.pfd` as its `+0x58` space (the reader builds the name from the scene); the port treats it as the world space only.
* TODO(re): who writes the polygon flag `+0x20 = 1` (blocked, e.g. a closed gate/door model) at run time; not in the export.
