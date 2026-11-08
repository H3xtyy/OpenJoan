# Territory (`CreateTerritory` record) and its per-tick update `0x4E4BB0`

Reference build: joan.exe. Pseudocode: `$JOAN_WORK_DIR/decomp/game/world_nation/004e4bb0_FUN_004e4bb0.c` (+ helpers), `script/script_vm/0065f850` (`set_territory_nation`), `script/script_vm/004e4b20`.
File format: [formats/ter.md](../../formats/ter.md) (`CreateTerritory`; the per-territory reader `0x4F9330` is **not** in the export, so the field-to-file mapping below is by position/size and marked I).
Status: V = read in >= 2 functions, R = read once, I = inferred. Functions: [functions/world_nation.md](../functions/world_nation.md).

## Container and lookup

Global `dynarrb` of `Territory*`, 1-based, null = free (`0x420FF0(recno)` get, `0x4217E0(recno)` "slot empty", count = `0x417AD0`).
`0x4E4B20(name)` (`set_territory_nation`'s lookup): **descending** scan `count..1`, `FUN_007C07B0(record + 4, name) == 0` -> returns the **last** matching recno, 0 if none (case-insensitive ASCII compare, bytes as in the file). The handler does not check for 0 (a miss dereferences slot 0 = crash in the original; the port must no-op).

## Construction from `CreateTerritory` (reader 0x4F9330, verified)

`sizeof(Territory) == 0xE0` (`operator new(0xE0)` + `memset` in 0x4E4A50; `+0x00` u32 = recno from the `dynarr` push). Keys are read **positionally** in this order (names match [formats/ter.md](../../formats/ter.md)):
`TerritoryName` (strncpy 0x20 -> `+4`), `CenterX`, `CenterY`, `CoordinateX`, `CoordinateY` (ints -> `+0x28,+0x2C,+0x30,+0x34`, **no level offset added**), `Kingdom` (name -> `0x4B91A0` -> `0x4B67E0`; if that yields 0 and the text equals `Neutral` -> `-1`; any other unknown name stays 0) -> `+0x38`;
then optional `DisableBeingAttacked` (-> `+0x45`), optional `ScrName` (strncpy 8 -> `+0x3C`); `Type` -> `+0x26`; `Scale` -> `+0x25`; `TerritoryAreaCount` -> `+0x27`; then per area `TerritoryAreaType` and its ints (stored without sorting or validation, **max 4 areas: the loader stack arrays hold 4 entries; more would overflow**). `+0x46 = 0` at the end.
Codes: `Type`: **3 = castle, 4 = church, 5 = forest**; codes 0, 1, 2 are the remaining three values `camp`, `city`, `town` in an order not decoded (three consecutive `.rdata` literals, read 2026-10-22: 0x9D80E8 = "camp" -> 0, 0x9D80E0 = "town" -> 1, 0x9D80D8 = "city" -> 2; nothing in this family depends on the order except the notification id table below). `Scale`: `settlement` 0, `small_camp` 1, `large_camp` 2, `castle` 3, `paris` 4 (unknown text -> 0). Area type: `circle` 1, `rectangle` 0, `quadrilateral` 2.
Area value order (per area, file order): rectangle `X1,Y1,X2,Y2` -> `a0,a1,a2,a3` (the update treats `a0/a1` as minima; shipped files are already ordered), circle `X,Y,Radius` -> `a0,a1,radius`, quadrilateral `X1,Y1,X2,Y2,X3,Y3,X4,Y4` (interleaved) -> `a0..a7`. In `S7L1w.ter` the quadrilaterals have the winding that gives **negative** cross products for interior points (checked on one quad).

## Layout (packed; sizeof 0xE0)

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x04 | 0x22 | char[34] | `name` | `TerritoryName`; passed to the notification as the display name (`record+4`) | V (0x4E4B20, 0x4E4BB0) |
| 0x25 | 1 | u8 | `scale` | `Scale:` code 0..4 (see Construction) | V (0x4F9330) |
| 0x26 | 1 | u8 | `type` | `Type:` code 0..5 (3 castle, 4 church, 5 forest; 0 camp, 1 town, 2 city (literal order)). Selects the notification id (table below) | V (0x4E4BB0 x2, 0x4F9330) |
| 0x27 | 1 | i8 | `area_count` | `TerritoryAreaCount`, **max 4** (arrays below have 4 slots) | V |
| 0x28 | 16 | i32[4] | `center_x, center_y, coord_x, coord_y` | `CenterX/CenterY/CoordinateX/CoordinateY` (file order) | V (0x4F9330; not read by the update) |
| 0x38 | 4 | i32 | `nation` | owner nation recno, **-1 = neutral**, 0 = unknown name. Written by `set_territory_nation`. `-1` territories are skipped by the contest scan | V (0x65F850, 0x4E4BB0) |
| 0x3C | 8 | char[8] | `scr_name` | `ScrName` ("s1_1"...), strncpy 8 (may be unterminated) | V (0x4F9330) |
| 0x45 | 1 | u8 | `no_attack` | `DisableBeingAttacked`; when non-zero the territory is skipped by the contest scan | V (0x4F9330, 0x4E4BB0) |
| 0x46 | 1 | u8 | `contested` | 1 while an intruding unit stands inside; cleared by `set_territory_nation` | V (0x4E4BB0, 0x65F850) |
| 0x47 | 1 | u8 | `player_inside` | the player unit was inside at the previous tick | V |
| 0x48 | 4 | u8[4] | `area_type` | per area: `0 = rectangle`, `1 = circle`, `2 = quadrilateral` | V |
| 0x4C | 4x4 | i32[4] | `a0` | rect: `x_min`; circle: centre x; quad: vertex 0 x | V |
| 0x5C | 4x4 | i32[4] | `a1` | rect: `y_min`; circle: centre y; quad: vertex 0 y | V |
| 0x6C | 4x4 | i32[4] | `a2` | rect: `x_max`; quad: vertex 1 x | V |
| 0x7C | 4x4 | i32[4] | `a3` | rect: `y_max`; quad: vertex 1 y | V |
| 0x8C | 4x4 | i32[4] | `a4` | quad: vertex 2 x | V |
| 0x9C | 4x4 | i32[4] | `a5` | quad: vertex 2 y | V |
| 0xAC | 4x4 | i32[4] | `a6` | quad: vertex 3 x | V |
| 0xBC | 4x4 | i32[4] | `a7` | quad: vertex 3 y | V |
| 0xCC | 4x4 | i32[4] | `radius` | circle radius | V |
| 0xDC | 4 | u32 | `contested_since` | `DAT_00A500BA` (sim ms) at the tick the territory became contested | V (0x4E4BB0) |

Area index `i` is the `i`-th `TerritoryAreaType` block of the file; the values are file integers converted to float for the tests. In `.ter`, rectangle keys are `X1,Y1,X2,Y2` "two opposite corners" - the tests below assume `a0 <= a2`, `a1 <= a3`; the reader (0x4F9330) does **not** normalise, so keep the file order (shipped files are ordered, e.g. `S1L1w.ter`).

### Point-in-area test (identical code in both loops of 0x4E4BB0)

Point `p = (pos[0], pos[1])` = first two components of the unit position vector (`0x4FEDB0` copies `unit+0x0E, +0x12, +0x16`; **z is the third component and is ignored**: territories are 2D on the ground plane, matching `.pla` where HEIGHT/Z are the vertical axis).
A territory contains `p` if **any** of its `area_count` areas does:

* rectangle: `a0 <= x <= a2 && a1 <= y <= a3` (inclusive, floats).
* circle: `(x-a0)^2 + (y-a1)^2 < radius^2` (**strict**; `radius*radius` computed in 32-bit int then converted to float).
* quadrilateral (convex, consistent winding): all four edge tests `(x - Px)*(Qy - Py) - (y - Py)*(Qx - Px) < 0.0` for edges P0->P1, P1->P2, P2->P3, P3->P0 (strict `<` against the constant `_DAT_009CF5C4 = 0.0`). Winding must therefore be such that interior points give negative cross products; the sign convention of the data is TODO(re).

## Per-tick update (`0x4E4BB0`, once per sim tick, after the places; manager object `M` is `this`)

`M` fields: `+0x1D` i32 `scan_cursor` (1-based round-robin cursor), `+0x21` u32 `last_notify_ms`. Both `TODO(re)` which global the manager is (its address is hidden by ECX).

**Part A - player position notification** (runs only if `count > 0` and the player unit `DAT_00A5B9E8` exists):

1. `P` = player unit; `p` = its position (as above).
2. For `t = count .. 1` (descending) over used territories: `inside = contains(T_t, p)`.
3. If `sim_ms - M.last_notify_ms > 3999` (i.e. `>= 4000 ms` since the last notification, unsigned compare): when `inside != T.player_inside` (entering **or** leaving), dispatch the notification for `T.type` with argument `&T.name`, then `M.last_notify_ms = sim_ms`. (The code has two branches - "left" and "entered" - that execute the same switch; only the throttle differs from an unconditional call.)
   Notification dispatch `0x5A3090(table, &DAT_009D6300, id, arg)` (looks up the entry whose name equals the string at `0x9D6300` and whose id equals the given one, then invokes its callback; the name literal at `0x9D6300` is `"SMS"` (an event channel of the scenario/script side, probably "scenario message"; which handler subscribes to ids 5..10 is TODO(re) outside this family):

   | `type` | 0 | 1 | 2 | 3 | 4 | 5 |
   |---|---|---|---|---|---|---|
   | wrapper | 0x4DD760 | 0x4DD720 | 0x4DD860 | 0x4DD820 | 0x4DD7A0 | 0x4DD7E0 |
   | id | 6 | 5 | 10 | 9 | 7 | 8 |

4. Always (also while throttled): `T.player_inside = inside`; if `inside`, `P.+0x08 = t` (the player unit's current territory recno; **the lowest recno wins** because the loop is descending and later writes overwrite).

**Part B - contested flag, one territory per tick** (round-robin):

1. Clamp `scan_cursor`: if `count < cursor` -> `cursor = count`; else if `cursor == 0` -> `cursor = 1`.
2. Starting at `t = cursor`, wrapping `count -> 1`, stop after one full round (`t == cursor` again): a territory is *eligible* if the slot is used, `T.nation != -1` and `T.no_attack == 0`. The first eligible one is processed, then the scan ends.
3. Processing of an eligible `T`: `flag = 0`; if `area_count > 0`: iterate units `v` from the highest recno down to 1 and set `flag = 1` (stop) at the first `v` that satisfies **all**:
   * slot used; `v.+0x22 < 4`; `v.nation != T.nation` (i16 compare with the territory's int);
   * unit-type record `d = 0x42D0A0(v.+0x3B)`: `d.subtype (+0x118) not in {2, 7}`, `d.class (+0x02) not in {0, 8, 9}`;
   * `0x432B90(v) == 0` (byte `v.+0x2E1` clear) and `0x42D0E0(v) == 0` (alive);
   * if `v.nation != 0`: let `R = Nation[v.nation].rel[T.nation]` (`0x4B42E0`); if the relation exists and `R.status == 4` (allied) the unit is **ignored**;
   * `v`'s position is inside the territory (area test above).
4. Finalise: if `flag && T.contested == 0` -> `T.contested_since = sim_ms`; `T.contested = flag`; `cursor += 1` (wrap to 1 past `count`).
   If no territory is eligible in a whole round nothing changes (the cursor stays).

So a territory's `contested` flag is re-evaluated once every `eligible_count` ticks (e.g. 10 territories -> each every 10th tick), the **delay of the flag flip is therefore up to `count` ticks**; consumers (scenario flow / end conditions / AI, other families) read `contested`/`contested_since` only.

Note: `set_territory_nation(name, nation)` sets `T.nation` and clears `T.contested`; it does not touch `contested_since` or `player_inside`.

## Territory ownership change rules (summary)

* Only scripts change `nation` (`set_territory_nation` 0x65F850: nation name -> `0x4B91A0` -> `0x4B67E0`; if that gives 0 and the name equals `"Neutral"` (case-insensitive) -> `-1`; an unknown name leaves 0). There is **no** automatic capture-by-presence rule in this function: presence only raises `contested`.
* `set_end_condition_territory_link(var, str)` (id 268, handler 0x65A460) only stores a copy of `str` in the end-condition object (`0x467390`: `d:\ww\end_con.cpp`, length+1 tracked alloc, `memcpy`) for the end-condition evaluator (battle/scenario family) - the territory is resolved later by name.
