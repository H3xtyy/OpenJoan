# Minimap (`interfac\minires.cpp` + the vision-mask code at 0x70EC50) — joan.exe

Functions: [functions/hud.md §5](../functions/hud.md). Status: **V** = read in >= 2
functions, **R** = one function, **I** = inferred. Offsets are byte offsets, fields are packed (4-byte fields sit at odd offsets).
The *draw* side of the minimap (the functions that render the quads and the fog texture) is **not in the export**: everything
below is the data model, the update (0x70EC50), the script entry points and the loaders.

The minimap is **not** a pre-rendered picture of the terrain. It is (a) a *world window* of one "tile" size centred on the followed
unit, (b) a set of world-space textured quads (`MinimapMarker`, one per `.map` row + a cursor quad), (c) up to 30 script way points,
(d) a 200x200 fog mask that is rebuilt from the vision radius of the allied units and from script-defined vision areas.

## 1. Minimap object (the singleton the update 0x70EC50 and the script setters operate on)

`this` of 0x70EC50 / 0x714040 / 0x718010 / 0x7180F0 / 0x7196E0 / 0x719770 / 0x7197F0 / 0x7198D0 / 0x71D150 / 0x71D1E0 (the receiver is
lost by Ghidra in the callers; it is one global object, see "Open questions").

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `loaded` | cleared by the free function 0x71B080 | R 0x71B080 |
| 0x04 | 4 | MinimapLayout* | `layout` | tile table built by 0x71B6A0 (section 2) | V 0x70EC50, 0x714040, 0x719900 |
| 0x08 | 2 | i16 | `scenario` | scenario/world index passed to `0x561320(+8)` ("world valid?"); 0x70EC50 returns early when 0 | R 0x70EC50 |
| 0x0A | 2 | i16 | `view_w` | width of the window in mask units (used as `view_w / tile_w` = mask cells per world unit) | R 0x70EC50, 0x719E60 |
| 0x0C | 2 | i16 | `view_h` | same for the height | R |
| 0x0E | 1 | u8 | `enabled` | update does nothing when 0 | V 0x70EC50, 0x714040 |
| 0x0F | 2 | i16 | `layer` | 0 or 1: selects which tile size of the layout is used (`layout+0x21/0x25` or `+0x29/0x2D`) | V 0x714040, 0x719900.. |
| 0x11 | 1 | u8 | `show_gold` | `set_minimap_display_gold_enable` (builtin 383, handler 0x65EDD0 writes `this+0x11`) | R builtin |
| 0x13 | 2 | i16 | `follow_unit` | recno of the unit the window is centred on (0x714040 reads its position); update does nothing when the entity is invalid (`0x428B70`) | V 0x70EC50, 0x714040 |
| 0x15 | 4 | Unit* | `ref_unit` | the main character; `ref_unit+2` (i16) is the "friendly side" id compared with every unit's `+2` and every territory's owner | V 0x70EC50 |
| 0x19 | 4 | i16** | `ref_nation` | `**ptr` = nation id looked up via `0x4B42E0`; a nation record with `rec+3 == 4` makes that nation's units count as friendly too ("allied") | R 0x70EC50 |
| 0x1D | 4 | f32 | `origin_x` | world X of the window's left edge: `unit.x - tile_w/2`, clamped `>= 0` by `0x590740(x,0,0)` | V 0x714040, 0x719900.. |
| 0x21 | 4 | f32 | `origin_y` | world Y of the window's top edge: `unit.y + tile_h/2`, clamped `>= 0`; mask row = `(origin_y - world_y) * view_h / tile_h` | V |
| 0x25 | 4 | f32 | `f25` | zeroed by 0x714040 | R |
| 0x32 | 2 | i16 | `blink_bpm_a` | `60000 / bpm / 2` = half period (ms) of the `blink_phase` toggle | R 0x70EC50 |
| 0x34 | 4 | u32 | `blink_t` | time stamp of the last toggle (`DAT_00A500BA`, real ms clock) | R |
| 0x38 | 1 | u8 | `blink_phase` | toggles 0/1 (marker blink) | R |
| 0x39 | 2 | i16 | `blink_bpm_b` | second rate; `local = 60000/bpm/2` is computed but unused | R |
| 0x40 | 2 | i16 | `anim_frame` | 1..3, advances every 0x168 = **360 ms** | R |
| 0x42 | 4 | u32 | `anim_t` | stamp of the last `anim_frame` step | R |
| 0x46 | 30 | u8[30] | `wp_on[30]` | way point *i* (0-based; scripts pass 1..30) enabled; `minimap_set_way_point(id,0,..)` clears it | V 0x718010, 0x70EC50 |
| 0x64 | 30*4 | i32[30] | `wp_unit[30]` | `minimap_set_way_point_follow_unit` target unit recno (written at `+0x64 + 4*(id-1)`) | R 0x7180F0 |
| 0xDC | 30*12 | f32[30][3] | `wp_pos[30]` | script world position x, y, z (z = height: S1L1 passes 109.484, -169.188, ...) | R 0x718010 |
| 0x244 | 30 | u8[30] | `wp_icon[30]` | last argument of `set_way_point` / `..._follow_unit` (icon / display mode; both handlers write it) | R |
| 0x262 | 30 | u8[30] | `wp_frame[30]` | blink/anim frame 1..7, +1 every 100 ms while enabled, wraps 8 -> 1 | R 0x70EC50 |
| 0x280 | 30*4 | u32[30] | `wp_t[30]` | stamp of the last frame step; set to `now` when enabled | R |
| 0x2F8 | 10 | u8[10] | `va_on[10]` | vision area *k* enabled (scripts pass k = 1..10, the setters write `this + k + 0x2F7`) | V 0x7196E0.., 0x70EC50 |
| 0x302 | 10 | u8[10] | `va_type[10]` | 0 rectangle, 1 circle, 2 quadrilateral | V |
| 0x30C | 10*4 | f32[10] | `va_a[10]` | first world coordinate of area *k* (rect: x_min, circle: centre x, quad: x of corner 1) | V |
| 0x334 | 10*4 | f32[10] | `va_b[10]` | rect: y; circle: centre y; quad: y1 | V |
| 0x35C | 10*4 | f32[10] | `va_c[10]` | rect: x_max; quad: x2 | V |
| 0x384 | 10*4 | f32[10] | `va_d[10]` | rect: y; quad: y2 | V |
| 0x3AC / 0x3D4 / 0x3FC / 0x424 | 10*4 each | f32[10] | `va_e..h[10]` | quad corners 3 and 4 (x3,y3,x4,y4) | V 0x7197F0 |
| 0x44C | 10*4 | f32[10] | `va_radius[10]` | circle radius (world units) | V 0x719770, 0x70EC50 |
| 0x474 | 4 | u32 | `mask_t` | stamp of the last mask rebuild (rebuild runs when `mask_t < now`, i.e. every frame) | R 0x70EC50 |
| 0x478 | 4 | u8* | `mask` | 200 x 200 bytes (40000, `memset`-cleared per rebuild); 1 = revealed | V |
| 0x47C | 4 | Texture* | `fog_tex` | wrapper whose `+0x58` is the D3D8 surface; locked via vtable slot `0x40` (LockRect) and `0x44` (UnlockRect) | R |

The script setters are 1-based, the arrays 0-based: `index = id - 1`. 30 way points and 10 vision areas.

### Vision-mask rebuild (0x70EC50, per frame, only if `enabled && view_w != 0 && scenario != 0`)
1. `0x714040` recomputes `origin_x/origin_y` from the followed unit (centred, clamped at 0).
2. Blink/animation counters advance as in the table above (all on the **real** clock `DAT_00A500BA`).
3. Mask rebuild (when `mask != 0`, the world is valid): clear 40000 bytes. Scale `sx = view_w / tile_w`, `sy = view_h / tile_h`, `ratio = sx / sy`.
   For **every entity** `u` in the entity table (1-based, `0x417AD0` count, `0x41B3F0` get) that is a live unit with
   * `0x42D0E0(u) == 0` (not dead/invalid), type record `T = 0x42D0A0(u+0x3B)`: `T+0x118 != 2` and `!= 7` and `T+2 != 0`,
   * and (`u+2 == ref_unit+2` **or** the diplomacy lookup `0x509400()`/`0x4B42E0(*ref_nation)` gives a record with `+3 == 4`),
   fill a **circle** of radius `r = u+0x3C1` (the value set by `set_unit_vision_range`) around `(u.x - origin_x, origin_y - u.y)`: for every
   mask row `y` in `[cy - r*sy, cy + r*sy]` clamped to 0..199 compute the half-chord `sqrt(r^2 - dy^2) * ratio` and set the span
   `[cx - h, cx + h]` (clamped to 0..199) to 1. Units whose circle is entirely outside the window are skipped.
   Then for every **territory/place record** `p` (`0x4217E0(i)==0`, record `0x420FF0(i)`, owner `p+0x38 > 0` and equal to the friendly side or allied) loop its areas
   `k < p+0x27` (byte count, max 4): type `p+0x48+k` (0 rect -> 0x719900, 1 circle -> 0x719B40, 2 quad -> 0x719E60) with the parameters at
   `p+0x4C+4k`, `+0x5C+4k`, `+0x6C+4k`, `+0x7C+4k` (rect/quad), `+0x8C..+0xBC` (quad), radius `p+0xCC+4k` (circle).
   Finally the 10 script areas `va_*` with the same three fill routines.
4. Upload: lock the 200x200 A8R8G8B8 texture; pixel = `0x80000000` where `mask == 0`, `0x00000000` where revealed. So the fog is **50 % black** over
   everything that is not currently seen; it is not persistent exploration (the mask is rebuilt from scratch every frame).
Fill routines: rectangle 0x719900(x0,y0,x1,y1) — skipped if entirely outside; spans clamp to 0..199; circle 0x719B40(cx,cy,r) as above;
quadrilateral 0x719E60 scan-converts the 4 edges per row (min/max crossing x), also clamped.

## 2. MinimapLayout (built by 0x71B6A0 from the `INTERFAC` BmpRes table; freed by 0x71B080)

The map image is a grid of **BmpRes tiles named by convention** in the `INTERFAC` table ([bitmap](../../formats/bitmap.md), [Bitmap.md](Bitmap.md)): all entries
except `TITLE`, `OBJ01`, `OBJ02`.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `loaded` | cleared by the free function 0x71B080 | R |
| 0x01 | 4 | i32 | `count` | entries kept = `table.count - (number named TITLE/OBJ01/OBJ02)` | V 0x71B6A0 |
| 0x05 | 4 | Tile* | `tiles` | `count * 0x20` bytes, zeroed | V |
| 0x09 | 4 | i32 | `marker_count` | total `.map` records over all world blocks + 1 (cursor) — section 3 | V 0x71BF10, 0x71B080, 0x71D150 |
| 0x0D | 4 | MinimapMarker* | `markers` | `marker_count * 0xAE` bytes | V |
| 0x11 | 4 | i32 | `marker_count2` | copy of `marker_count` (0x71BF10); 0x71B080 frees the array at `+0x15` with this count | R |
| 0x15 | 4 | ptr | `markers2` | second array, same stride 0xAE, freed like `markers` (never filled by the exported code) | R |
| 0x19 | 2 | i16 | `rows0` | highest `r` seen for layer 0 | V |
| 0x1B | 2 | i16 | `cols0` | highest `c` seen for layer 0 | V |
| 0x1D | 2 | i16 | `rows1` | highest `r`, layer 1 | V |
| 0x1F | 2 | i16 | `cols1` | highest `c`, layer 1 | V |
| 0x21 | 4 | i32 | `tile_w0` | `ftol(world_extent_x) / cols0` — world units per tile column, layer 0 (the world extent is read from the scenario record `0x420240(scenario)` via an x87 value that Ghidra drops: `TODO(re)`) | R |
| 0x25 | 4 | i32 | `tile_h0` | `ftol(world_extent_y) / rows0` | R |
| 0x29 | 4 | i32 | `tile_w1` | layer 1, `/ cols1` | R |
| 0x2D | 4 | i32 | `tile_h1` | layer 1, `/ rows1` | R |

`Tile` (0x20 bytes): name[9] copied with `strncpy(.., 8)` (BmpRes entry name, 8 chars), `+9 i16 layer` (= digit after the **last `z`** of the name, atoi of 1 char),
`+0xB i16 row` (2 digits after the last `r`), `+0xD i16 col` (2 digits after the last `c`), `+0x1F u8 is_background`. Parsing order (replicate): if the name contains a **`b`**
(`strrchr(name,'b')` != 0) the tile is a *background* (`+0x1F = 1`, no row/col); otherwise row = 2 chars after the last `r`, col = 2 chars after the last `c`.
Because `b` is tested first anywhere in the string, a name with a stray `b` before `z0r..c..` is a background — the real names must be checked
when the BmpRes dump is available (`TODO(re)`).
Derived (second loop, only non-background tiles): `+0x0F i32 x0 = tile_w * (col-1)`, `+0x13 i32 y0 = tile_h * (rows - (row-1))`,
`+0x17 i32 x1 = max(0, tile_w*col - 1)`, `+0x1B i32 y1 = max(0, tile_h * (rows - row) - 1)` (layer 0 uses `tile_w0/tile_h0/rows0`, layer 1 the `*1` set).
So row 1 is the **top** row and the y axis is flipped (world y grows upward): consistent with `origin_y = unit.y + tile_h/2`.
The window shown by `Minimap.view_*` is one tile in size: **the minimap shows a square region of `tile_w x tile_h` world units centred on the followed unit.**

## 3. MinimapMarker (0xAE bytes, array at `MinimapLayout.markers` = `Minimap.layout + 0x0D`)

Built by 0x71BF10 from the per-world-block `.map` files ([docs/formats/map.md](../../formats/map.md) documents the file; **this function is the reader that page could not find**).
The loop: for each world block `b` (count at `scenario_rec+0x72`, block pointers at `+0x7A`) it opens `<scenario dir>\<block file>` through the memory-file helper
(0x588000/0x5886D0, path assembled with 0x5A1440/0x5A14F0/0x5A1550 from literals that the export dropped; the file extension is `.map`, see map.md),
reads `u32 count`, accumulates the total (+1 for the cursor quad), allocates `total * 0xAE` bytes and in a second pass reads each record:

| rec offset | size | type | name | source | evidence |
|---|---|---|---|---|---|
| 0x00 | 0x40 | char[64] | `name` | `read_lpbuf(dst, 0x40)` (u16 length prefix + bytes) — the instance name script uses (`fcampS1L1`) | V 0x71BF10, 0x71D150 |
| 0x40 | 8 | char[8] | `kind` | `read_lpbuf(.., 8)` — name of the BmpRes sprite in `INTERFAC` (compared case-insensitively by `0x620B40`) | V |
| 0x49 | 2 | i16 | `type` | `read_i32` truncated (the file's reserved u32, 0 in every shipped file); the cursor record has 2 | R |
| 0x4B | 0x40 | char[64] | `extra` | `read_lpbuf(.., 0x40)` (empty in the shipped files) | R |
| 0x8B | 4 | f32 | `x` | `(float)block.x0 (block+0x44) + (float)read_i32` | R |
| 0x8F | 4 | f32 | `y` | `(float)block.y0 (block+0x48) + (float)read_i32` | R |
| 0x93 | 4 | f32 | `z_param` | `(float)read_i32` (the `z/param` column of map.md) | R |
| 0x97 | 4 | f32 | `pad_f` | `(float)read_i32` (the 4 padding bytes) | R |
| 0x9B | 4 | f32 | `half_size` | computed: `type == 0`: `sprite_w / 2.0 - 3.0`; otherwise `sqrt(w^2 + h^2) / 2` | R |
| 0x9F | 1 | u8 | `enabled` | set to 1 at load; toggled by `set_minimap_icon_display_enable(name, on)` (0x71D150) and read by `get_...` (0x71D1E0) | V |
| 0xA0 | 1 | u8 | `no_quad` | `read_u8` (first of the two trailing padding bytes); when 0 the marker gets a quad | R |
| 0xA1 | 1 | u8 | `b_a1` | second trailing byte | R |
| 0xA2 | 4 | u32 | `a2` | cleared | R |
| 0xA6 | 4 | Actor* | `quad_actor` | `0x10C`-byte en3d actor with one plane of 4 vertices / 2 triangles (`0x11C` plane object, `0x2D8` model) | R |
| 0xAA | 4 | Image2D* | `sprite` | a 4-byte `Image2D` wrapper (0x628E70), released by 0x71B080 through 0x6292C0/0x71B330 | R |

The last record (`count` = total+1 -> index `total`) is the synthetic **cursor marker**: name `"Cursor"`, kind `"CURSOR"`, type 2, position = the global `(DAT_00A69CD8, CE0, CDC)`, `z_param = 0`.
Quad construction per marker with `no_quad == 0`: BmpRes entry `E = 0x620B40(kind)`; texture = `INTERFAC` DDS (0x6099C0 by the entry's texture name `E+9`), UV rectangle
`(E+0x2A/E+0x2C)` .. `(E+0x2E/E+0x30)` divided by (texture w-1, h-1); half extents `E+0x34/2`, `E+0x36/2`; vertices (stride **0x24**: pos xyz, normal (0,0,1), diffuse RGBA
`(0xF0,0xF0,0xF0,0x33)`, uv) at `(-w/2,-h/2)`, `(+w/2,-h/2)`, `(-w/2,+h/2)`, `(+w/2,+h/2)`; indices `0,2,1, 1,2,3`; material word `0x10002`, mode 3.
(Vertex alpha 0x33 = 20 % is the *tint*; the draw state decides blending: `TODO(re)`.)

## 4. Script entry points (handlers in `script_vm`, see [functions/hud.md §11](../functions/hud.md))
`minimap_set_way_point(id, x, y, z, on, icon)` -> 0x718010; `..._follow_unit(id, unit, icon)` -> 0x7180F0 (sets `wp_unit` and `wp_icon`, does **not** enable the point);
`set/get_minimap_icon_display_enable(name, on)` -> 0x71D150 / 0x71D1E0 (guarded by `0x44CF30() && 0x44CF10() != 0`, i.e. a world with a minimap is loaded; **linear search by exact `strcmp` of
`name` over the 0xAE records**, returns 0 if not found); `set_minimap_{rectangular,circular,quadrilateral}_vision_area_enable` -> 0x7196E0 / 0x719770 / 0x7197F0; `disable_minimap_vision_area(id)` -> 0x7198D0
(`va_on[id] = 0`); `set_minimap_rotation(v)` -> `DAT_00A4F3AC = v` (a global read by the draw code; not by anything exported); `set_minimap_display_gold_enable(v)` -> `Minimap.show_gold`.

## 5. Draw side (0x70E500 `minimap_draw(layer_mask)`, helpers 0x716010, 0x716BE0, 0x7141A0, 0x70FBE0, 0x718540, 0x71A3B0, 0x71ABA0, 0x7190B0)

Screen layout (pixels; `W` = back-buffer width, **right-anchored**, all in the 2D layer system; V = read in 0x70E500 / 0x70FBE0 / 0x718540):
* The 200x200 window occupies `x = W-0xD3 .. W-0xB` (left `W-211`, right `W-11`), `y = 10 .. 209` (`_DAT_009CFC20 = 10.0`, bottom `_DAT_009DEDDC` = 209.0 inferred). Draw order inside `0x70E500` (only when `layer_mask & 1`, minimap enabled, world valid):
  1. `0x714040` origin update; 2. **tile images** `0x716010(1)` then `0x7141A0` (the background tiles of the `INTERFAC` table, up to 9: the tile under the window centre plus its 8 neighbours looked up with `0x71B3B0(layer, col, row)`, each clipped to the window via a source rect) and `0x716010(0)`; 3. one marker pass per entity (below); 4. the followed unit's own icon; 5. the **fog quad** `0x6332B0(W-0xD3, 10, layer 9, fog_tex(+0x47C), colour 0xFF000000, ..., 199, 199)` (the 200x200 texture of section 1, drawn after the markers so it dims everything not revealed); 6. `0x716BE0` (RTS mode only: the camera frustum — 4 lines `0x717F40(...)` joining the 4 ground points `DAT_00A5BA04..` projected into minimap pixels); 7. the **frame** BmpRes `"FRAME"` at `(W-0xD7, 6)` layer 8 (texture loaded from the BmpRes entry, freed after); 8. way points: for entries 0..19 (`+0x46+i`) `0x718540(i, x, y, z, &counter)`; the first loop also *collects* items (kind `+0x4E == 0xF`, not the exclusion name `DAT_009D74E0`) into way-point slots 20.. (`+0x46+24..`) so item pickups blink on the map; 9. territory icons `0x71A3B0(i)` for places whose owner is set, `+0x46 != 0` and whose stamp is `< 0xEA61 = 60001 ms` old (`now - rec+0xDC`): places that changed hands in the last 60 s flash.
* **Rotation** `r = Minimap+0x2B mod 4` maps minimap pixel offsets `(px, py)` (`px = view_w * (x - origin_x) / tile_w`, `py = view_h * (origin_y - y) / tile_h`) to screen: `r=0`: `(L + px, T + py)`; `r=1`: `(L + py, T - px + 200)`; `r=2`: `(R - px, B - py)`; `r=3`: `(L - py + 199?, T + px)` with `L = W-211`, `T = 10`, `R = W-11`, `B = 209`; results are truncated to integer pixels (`0x590740`). The tile quads rotate by the same quarter turn (`rot` argument of 0x633500).
* **Unit icons** (`0x70FBE0(unit)`: called for every entity except the followed one, then once for the main character `Minimap+0x15`; skipped when `Minimap` disabled, the position is outside the world rectangle, or the mask cell under the unit is 0 — **units standing in fog are not drawn**): icon centre = the transform above, drawn at layer 9 (above the minimap background). The branch structure (V for the BmpRes names, I for the exact predicates because `0x5094D0`, `0x4FD980`, `0x50D550` are small helpers read only by use):
  * the unit belongs to the main character's side and is not the followed unit, and `0x4FD980(0) == 0`: **named heroes** get a BmpRes `"!<n>_M"` / `"!<n>_H"` (`_H` when `unit+0x2A5 == 0 && unit+0x2AA != 0`, wounded) with `<n>` = `joan, metz, alen, lahi, bast, flav, char, rich` for the type names `JOAN_OF_ARC, JEAN_DE_METZ, ALENCON, LAHIRE, ORLEAN, FLAVY, CHARLIE7, arthur_de_richemont`, preceded by a `"!outbox"` backing sprite of the same size (the hero icon sprite size + a margin); all clipped to the window (8 px margin);
  * other units of the side: `unit+0x104 >= 3` (and not 4) -> `"!capt"` (captain) sprite; `unit+0x104 < 3` and type class 0 -> filled square in colour global `DAT_00A6A5B8`; class 8 (siege) -> `"!siege"` sprite when `0x50D550(1/2) != 0`, else square `DAT_00A6A5A0`; any other class -> square `DAT_00A6A5B4`; the same-side-but-RTS case draws `0x718140` (an edge arrow);
  * `unit+0x104 == 4` (enemy rank) -> `"!ENEMY"` sprite (or square `DAT_00A6A5A0` for class 8 without the siege flag);
  * allied-nation units (relation status 4, `Minimap+0x19` record) that are not the main character -> square `DAT_00A6A59C`.
  `TODO(re)`: values of the colour words `0xA6A59C .. 0xA6A5B8` (runtime-initialised; dump with a trace) and the exact side/RTS predicates.
* **Way points** (`0x718540`): BmpRes name = `DAT_009DEDFC` + frame digit `wp_frame` (`1..7`, from `0x5A15D0(frame)`), centred on the transformed position at layer 9 when inside the window (9 px margin); otherwise clamped to the nearest window edge and drawn with `"WP_FAR"` at layer 8; if `wp_icon != 0` a counter text (`0x58FCA0(counter++)`) is drawn next to it (white, centred) so several way points are numbered 1, 2, ...
* **Marker quads** (`0x716010(background_flag)`): for each `MinimapMarker` with `enabled` and (`no_quad == 0` and `background_flag == 0`) or (`no_quad != 0` and `background_flag != 0`): position by type (0: stored x,y; 1: the unit named `extra`, also updating `+0x97 = heading/ (pi/180)`; 2: the cursor marker = player-controlled point), transformed as above; `no_quad == 0` markers are drawn as the quad actor with `0x6349B0` at layer 9 (type 2 uses a heading offset) or 10; `no_quad != 0` markers are **scrolling background strips**: `+0xA2` advances by `(DAT_00A500BE << 9) / 1000` per frame modulo `0x200` (a 512-step scroll cycle) and the sprite is drawn clipped to the window with alpha `+0xA2` (< 0x101 ascending, else `-value` descending: a 0..255..0 pulse).

Constants now known (`.rdata` dump): `_DAT_009CFC20` = 10.0 (window top), `_DAT_009CFCA0` = 200.0, `_DAT_009DEDDC` 209.0 and `_DAT_009DEDD8` 199.0 inferred.

## Open questions
* The address of the `Minimap` singleton (`TODO(re)`: the update `0x70EC50` is called by `0x6FD8B0` without a visible receiver). `MinimapLayout` (a.k.a. "MiniRes",
  `Minimap.layout`) is the object of 0x71B6A0 / 0x71BF10 / 0x71B080 / 0x71D150 / 0x71D1E0; the `Minimap` object (`+0x00..+0x47C`) is the one of 0x70EC50 / 0x714040 / 0x718010 and friends.
* Draw side: world-space orthographic camera for the quads, how `layer` 0/1 and `set_minimap_rotation` map to it, the fog quad, way-point icon sprites (`OBJ01`/`OBJ02`/`TITLE`) — need an export of the callers of 0x70EC50's data (the HUD draw `0x6F9970`..`0x6F9B60`).
* `view_w/view_h` (+0x0A/+0x0C) values (200?) and the scenario `world_extent` used for `tile_w/h`.
* `0x717240 minimap_click_camera` is documented in [functions/camera.md](../functions/camera.md).
