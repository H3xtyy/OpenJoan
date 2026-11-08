# Place (`CreatePlace` zone / settlement record) and the "units in place" queries

Reference build: joan.exe. Pseudocode: `$JOAN_WORK_DIR/decomp/game/world_nation/`, `script/script_vm/` (handlers + predicates), `engine/lights2/` (0x4C48B0).
Packed (odd offsets). Status: V = read in >= 2 functions, R = read once, I = inferred. Ghidra drops the ECX receiver of most helpers; deductions are marked.
File format that creates places: [formats/pla.md](../../formats/pla.md) (`CreatePlace`; reader `0x55A090` is **not** in the export). Functions: [functions/world_nation.md](../functions/world_nation.md).

## Container and lookup

* Global `dynarrb` of `Place*`, 1-based, null slot = free. `0x456C70(recno)` = get (`0x41B3F0`), `0x456CA0(recno)` = "slot empty". Count = `0x417AD0` on the container (`+0xC`). Recno = creation order in the `.pla` files (I; `dynarrb::push` reuses freed slots LIFO).
* **Name -> recno** `0x4C54A0(name)` (`search_place_by_name`, handler `0x653EB0`): ascending scan `1..count`, `FUN_007C07B0(place + 5, name) == 0`, returns the **first** match, 0 if none (handler also returns 0 for a null name). The compare is the case-insensitive ASCII compare used everywhere (see [Nation.md](Nation.md) "Name lookup"); bytes are the file's code page (cp1250 in the Polish install), no normalisation. Scripts of S1L1 use ASCII names (`"Camp 1 Place"`, `"StartPointPlace"`); `PLACE_NAME` in the `.pla` is the same string. Duplicate names resolve to the lowest recno.
* Territories use a **descending** scan (last match wins), see [Territory.md](Territory.md).

## Construction from `CreatePlace` (reader 0x55A090, verified)

`sizeof(Place) == 0x1F6` (502; `operator new(0x1F6)` in 0x4C5370, ctor 0x422B50 zero-fills). Reader sequence for one block (all coordinates below get the **level origin offsets** `loader+0x44` (x) and `loader+0x48` (y) added; z is raw; the same offsets are added by `CreateLight`, 0x49B970; their source value is set by the level loader 0x4F2F80, TODO(re)):

1. `0x4C5370`: allocate, `dynarrb::push`, `place.+0x00 = recno` (u16). `0x456C70(recno)` fetches it.
2. `0x433EA0(0, scale, name)`: `+0x02 = 0` (i16), `+0x04 = scale code` (u8), `strncpy(+0x05, PLACE_NAME, 0x20)`, `+0x25 = 0`, `counter(+0x52) = 400`, `counter_max(+0x56) = 400`, `timer(+0x5A) = 0`, `spawn_count(+0x1CA) = 0`.
   Scale code: `PLACE_SCALE` compared (case-insensitively) against four literals -> `1..4`, anything else `0`. The literal-to-code order is not decoded (the data values are `Castle`, `Town`, `umbrella`, `Village`; TODO(re) read `.rdata` of the four strings); store the raw string/enum, nothing in this family reads the code.
3. `0x4366F0(scene)`: `+0x1DE = scene` (detaches the old scene object first, `0x43EDB0`/`0x5EFDB0`).
4. **Box and position** `0x42B350(X, Y, Z, &box)`: `+0x2A,+0x2E,+0x32 = X+ox, Y+oy, Z` (f32), `+0x3A..+0x51 = box[6]`, then moves the place object (`+0x1DA`, actor; or `0x437E20` if none). Heading `0x4233C0(HD_rad)`: `+0x36`.
   `HD` is multiplied by pi/180 when the unit token equals a literal, else used as-is (always 0 in the data). Box for the shipped `HD == 0`: with `hx = W/2`, `hy = L/2` and `s = |sin(hd)|`, `c = |cos(hd)|`:
   `min = (X+ox - (s*hy + c*hx), Y+oy - (s*hx + c*hy), Z - K)`, `max = (X+ox + (s*hy + c*hx), Y+oy + (s*hx + c*hy), Z + H + K)`, i.e. for HD 0: **x in X +- WIDTH/2, y in Y +- LENGTH/2, z in [Z-K, Z+HEIGHT+K]**. `K = DAT_009D0748` = **64.0** (float32 read from .rdata 2026-10-22). (`FUN_007B24B4` = cos, `FUN_007B2404` = sin per CamMan.md.) The decompile shows the rotated-AABB branch under `hd == 0` and a relative box `(-W/2,-L/2,-64)..(W/2,L/2,H+K)` otherwise; the second can only be a decompiler sense inversion (it would put places at the origin), so the rotated-AABB formula applies to every HD.
   Worked example (`S1L1.pla`, "Camp 1 Place", ox/oy unknown): x 10808.4..13189.2, y 12237.8..15067.8 (before offsets) - contains the circle territory centre (12000,14000): consistent.
5. Entrances: up to 8 `(x+ox, y+oy, z)` triples, appended by `0x4C22B0` to `+0x62 + 12*i`, count `+0x5E` (< 8 enforced; the `ENTRANCE_*` keys of `.pla` are positional, all 1,520 lists in the data are empty).
6. Parking: up to **13** entries, `0x438420(pos, dir)` writes `pos` to `+0xC6 + 12*i`, `dir` (radians; `PARKING_DIR` is multiplied by pi/180 when the unit token matches, else 1.0) to `+0x162 + 4*i`, increments `+0xC2` (refuses when already 13). The layout fits exactly: 13*12 = 0x9C (0xC6..0x161), 13*4 = 0x34 (0x162..0x195), then 13 occupant slots (0x196..0x1C9), then `spawn_count` at 0x1CA.
7. `KINGDOM`: three syntaxes (a nation **name** -> `0x4B91A0` -> `0x4B67E0`; or a numeric recno; or a numeric res id mapped through `0x4B67E0`); unknown/`UNKNOWN` -> 0. Applied with `0x44F5C0(place, nation)` (below).

`0x44F5C0(this = place, nation)` = **Place::set_nation**: if `place.+0x26 != nation`: remove the place from the old nation's `roster_c` (`0x45C460`, sorted u16 array of owned place recnos), write `+0x26 = nation`, add to the new nation's `roster_c` (`0x498B70`: insert if `place.nation == nation.recno`), and call `facility->vtable[+0x14](old, new)` for each facility on the list `+0x1EE`. So **`set_place_nation_by_code` does change `place.+0x26`** (via 0x42ACD0 -> 0x44F5C0).

## Layout (only offsets proven by use)

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x00 | 2 | u16 | `recno` | own slot number | V (0x4C5370) |
| 0x02 | 2 | i16 | `kind?` | 0 at creation | R |
| 0x04 | 1 | u8 | `scale` | `PLACE_SCALE` code 0..4 | V (0x433EA0) |
| 0x05 | 0x20 | char[32] | `name` | `PLACE_NAME` (truncated to 32 bytes; byte `+0x25` forced 0) | V (0x4C54A0, 0x433EA0) |
| 0x2A | 12 | f32[3] | `pos` | X, Y, Z (+ level offsets) | V (0x42B350) |
| 0x36 | 4 | f32 | `heading` | HD in radians | R |
| 0x5E | 4 | i32 | `entrance_count` | <= 8 | V (0x4C22B0) |
| 0x62 | 12 x 8 | f32[3][8] | `entrances` | | V |
| 0x1DA | 4 | Actor* | `actor` | place model object | R |
| 0x26 | 4 | i32 | `nation` | owner nation **recno** (0 = `KINGDOM: UNKNOWN`/none). Compared with `unit.+2`. Written only by `Place::set_nation` 0x44F5C0 | V (0x6542B0, 0x654560, 0x49C240, `ter` handler uses +0x38 for territories) |
| 0x3A | 24 | f32[6] | `box` | axis-aligned box `{min x,y,z, max x,y,z}`; the "inside" test `0x60D5F0(this = box, p)` is **inclusive on all six faces**. `this` is deduced (the lea is hidden), I. Passed explicitly to `World::query_blocks` as `place+0x3A` (V) | V/I |
| 0x52 | 4 | i32 | `counter` | slowly regenerating counter, initialised to **400** (see update) | V (0x4C3330, 0x433EA0) |
| 0x56 | 4 | i32 | `counter_max` | upper bound, initialised to **400** (`0x469380`) | V |
| 0x5A | 4 | u32 | `timer_ms` | accumulates `DAT_00A500BE` (sim step, ms) per tick; at `>= 60000` it is reset and `counter` may grow by 1 | V (0x4C3330) |
| 0xC2 | 4 | i32 | `parking_count` | number of parking spots (`PARKING_X/Y/Z/DIR` groups) | V (0x4C3330, 0x654830) |
| 0xC6 | 12 x n | f32[3][n] | `parking_pos` | parking positions (x,y,z), n <= 13 | V (0x438420) |
| 0x162 | 4 x n | f32[n] | `parking_dir` | heading of spot `j` (0-based) **in radians**; `set_parking_dir(place, deg)` writes `deg * pi/180` (`DAT_009CF884`) into all `parking_count` entries | V (0x654830) |
| 0x196 + 4*(k-1) | 4 x n | i32[n] | `parking_unit` | occupant unit recno of spot `k` (1-based, accessed as `+0x192 + 4*k`), 0 = free. Capacity is **13** (0x196..0x1C9) | V (0x4C3330) |
| 0x1CA | 4 | i32 | `spawn_count` | `get_spawn_unit_count_in_place` returns it unchanged (writer not in the export) | R (0x6541A0) |
| 0x1CE | 4 | i32 | `captured` | `get_place_captured` / `set_place_captured` (`0x654530` getter, `0x4C46F0` setter) | V |
| 0x1D2 | 4 | f32 | `view_angle` | unit view angle in radians; `set_place_unit_view_angle(deg)` stores `deg * pi/180`; the getter returns an int through `__ftol` (truncating; the scale of the returned value is lost in the export, TODO(re)) | R (0x6547E0, 0x6547A0) |
| 0x1DE | 4 | ptr | `scene` | set by `0x4366F0` during creation (the scene/world object the place lives in; also gates the 0x1DA actor); **every unit query returns 0 when it is 0**; passed as the parent object to `0x4994B0` when `change_all_item_in_place` re-creates items | R (0x49C240, 0x4C4440) |
| 0x1EE | 4 | Facility* | `facilities` | head of the facility list of this place (`0x4C48B0`); next at `Facility+0x71`, kind `i16` at `+4` (kind `1` = barrack), `vtable[+0x18]()` returns the facility data (below) | V (0x49C470, 0x4A65D0, 0x4A8C20, 0x4AB020, 0x4B79A0) |

Facility data used by the enemy counters (kind 1 = barrack): `+0x7B` i16 leader (non-zero = one queued leader), `+0x122` i16 number of member groups, `+0x12C + 2*i` i16 queued members of group `i`.

## Unit fields this family reads (unit pointer `u`; owner page is the unit family)

`u+0x00` u16 recno, `+0x02` i16 nation recno, `+0x04` i16 parking place recno, `+0x06` i16 parking slot, `+0x08` i16 current territory (written for the player unit only), `+0x0E` f32[3] position (x, y, z; **z is height**),
`+0x22` i32 (< 4 required by the territory scan), `+0x3B` i16 unit-type id (index into `unit_res`, record `0x203` bytes: `0x42D0A0(id) = table + (id-1)*0x203`, 1-based), `+0x104` i32 rank (>= 4 = "general"),
`+0x120` i32 and `+0x135` i32: **alive test** `0x42D0E0`: dead iff `u+0x135 < 1 && u+0x120 == 0`, `+0x2E1` u8 flag tested by `0x432B90` (excludes the unit from the territory scan), `+0x2EB` i16 bribe flag (value 2 is skipped by `set_units_in_place_can_bribe_flag`), `+0x335` i32 group id, `+0x33D` Unit* next in the block chain.
Unit-type record fields: `+0x02` i16 class (0, 8, 9 are non-combat specials; 8 = the flag-like units re-owned by capture), `+0x118` i16 subtype (2, 3, 7 are excluded from "fightable"/"people"), `+0x11C` u32 flags (mask `0x2443` required by `search_for_unit_in_place 'E'/'F'`).

## How a place finds its units (shared by all queries)

1. `World::query_blocks(box, &n)` (`0x429090`, calls `build_bvh` `0x44A260` first, then the BVH walk `0x54AD90` collecting every leaf whose AABB overlaps the box into `block_aux`, see [World.md](World.md)).
2. For each block `k < n`: head = `block+0x8C` (`0x54EE10`); the units of a block form a **circular singly linked list** through `unit+0x33D`; iterate `u = head; do { ...; u = u->next } while (u && u != head)`.
3. Per unit: skip when dead (`0x42D0E0`), skip when the unit position is not inside `place.box` (`0x60D5F0(u+0x0E)`), then apply the query's predicate.

A unit therefore belongs to exactly one block chain; units straddling blocks are found via the block that holds them.

## The queries (handler -> predicate; `H(p, u)` = hostility `0x4B6860(nation_a, nation_b)`, see [Nation.md](Nation.md))

Script argument order is **(place, unit)** for the unit-relative queries (checked against `S4L1_tactic.sce`: `number_of_enemy_in_place(place, unit)`); `search_for_unit_in_place(place, "P"|"E"|"F")`.
All of them first require `place.scene != 0`; a null/empty place yields 0. `R` = requesting unit `u0` (`0x41B3C0(unit)`), `N0 = u0.nation`, `O` = place owner nation object (`0x47B600(place.nation)` if `0x484870(place.nation) == 0`, else null).

| builtin id / handler | predicate (exact) | returns |
|---|---|---|
| 168 `search_place_by_name` 0x653EB0 | `0x4C54A0` above | recno or 0 |
| 169 `search_for_unit_in_place` 0x653EF0 -> `0x49C240(mode)` | first live unit inside, in block/chain order, with: mode `'P'`: `u.recno == DAT_00A5B9E8` (the player-controlled unit); mode `'E'`: `(typeflags & 0x2443) != 0` and `H(place.nation, u.nation) != 0`; mode `'F'`: `(typeflags & 0x2443) != 0` and `O` and `u`'s nation exist and (same nation object or `Nation[place.nation].rel[u.nation].status > 3`). Any other first character of the mode string matches nothing (returns 0). The `rel[..].status` receiver for `'F'` is deduced (ECX hidden). | unit recno or 0 |
| 170 `is_unit_inside_place` 0x653F30 | place slot used **and** unit slot used and `unit.pos` inside `place.box` (inclusive). **Does not test alive/dead.** | 0 / 1 |
| 171 `number_of_enemy_in_place` 0x653FE0 -> `0x49C470(unit)` | `cnt = #{live unit v in box: v.nation != 0 and H(N0, v.nation)}`; **then**, if the owner object `O` exists, `O.recno != 0` and `H(O.recno, N0) != 0`: for every facility of the place with kind 1: `cnt += (data.+0x7B != 0) + sum(data.members[i], i < data.+0x122)` (queued barrack troops count as enemies) | count |
| 172 `number_of_fightable_enemy_in_place` 0x654020 -> `0x49E500(unit)` | like 171 but units are skipped when `typedef.class in {0,8,9}` or `typedef.subtype in {2,3,7}`; same barrack addition | count |
| 173 `enemy_exist_in_place` 0x654060 -> `0x4A65D0` | boolean form of 171 (first hit returns 1) | 0 / 1 |
| 174 `fightable_enemy_exist_in_place` 0x6540A0 -> `0x4A8C20` | boolean form of 172 | 0 / 1 |
| 175 handler 0x6540E0 -> `0x4AB020(nation_name)` | **boolean**: any live unit in box (class not in {0,8,9}, subtype not in {2,3,7}) whose `nation == recno(nation_name)`; if `O.recno == target`, any barrack leader/queued member also returns 1 | 0 / 1 |
| 176 handler 0x654120 -> `0x4B79A0(nation_name)` | **count** of the same units; plus, when `O.recno == target`, the barrack leader (+1) and queued members | count |
| 177 `general_by_nation_exist_in_place` 0x654160 -> `0x4C2B50(nation_name)` | **count** of live units in box with `nation == target`, class/subtype exclusions as above **and** `u.+0x104 >= 4`; no barrack addition | count |
| 111 `set_units_in_place_can_bribe_flag(type, place, value)` 0x656030 | for each block: if the **head** unit's `+0x3B == type` (only the chain head is tested - original quirk), set `u.+0x2EB = value` for every chain member inside the box whose `+0x2EB != 2` | 0 / 1 |
| 178 `get_spawn_unit_count_in_place` 0x6541A0 | `place.+0x1CA` | int |
| 179 `change_all_item_in_place(place, old_item_type, new_item_type)` 0x654240 -> `0x4C4440` | for every item `i` with `i.+0x0A == old_item_type` and `i.pos (+0x56..) inside box`: remember pose, delete it (`0x49F370`), create an item of `new_item_type` (`0x49EA90`), attach to `place.scene`, restore position (`0x41FF50`) and the three rotation components (`0x499FD0`, `0x49AB00`, `0x49ADC0`) and set `new.+0x08 = place.nation` | 1 |
| 180 `get_place_captured` 0x654280 | `place.+0x1CE` | int |
| 181 `set_place_captured(place, flag)` 0x6542B0 | see below | 1 |
| 182 `set_place_nation_by_code(place, nation_name)` 0x654560 | see below | 1 |
| 183 / 184 view angle 0x6547A0 / 0x6547E0 | `+0x1D2` as above | |
| 185 `set_parking_dir(place, deg)` 0x654830 | all `parking_count` entries of `+0x162` = `deg * pi/180` | 1 |

(The BUILTINS table names 175 "number_of_..." and 176 "..._exist..."; the handlers behave the other way round - 0x6540E0 returns a boolean, 0x654120 a count. Shipped scripts only compare with 0 or use the count of 176, so either reading works; the implementer must map **handler address -> behaviour**. Confirm the dispatcher case order, TODO(re).)

### `set_place_captured(place, flag)` (0x6542B0)

1. `place.captured = flag` (`0x4C46F0`).
2. `F = recno("FRENCH")`, `E = recno("ENGLISH")` (name lookup as above). `new_owner = E` by default; if `place.nation == F`: `new_owner = (flag == 0) ? F : E`; else if `place.nation == E`: `new_owner = (flag != 0) ? F : E`; **any other place owner (Mercen, none): return 1 after step 1 without touching units**.
   (`place.nation` itself is **not** rewritten here; it keeps the original owner so repeated calls are idempotent.)
3. For every live unit `v` (descending recno) with `v.nation != new_owner`, `typedef(v).class == 8` and `v.pos` inside `place.box`: `0x509250(v, new_owner)` (full roster/strength transfer).
4. For every item `i` whose object-definition class (`0x425CE0(i+0x0A)`, first i16) is `1` or `4` and whose position (`0x499930`) is inside the box: `i.+0x08 = new_owner` (item owner nation, i16).

### `set_place_nation_by_code(place, name)` (0x654560)

`nation = recno(name)` (0 if unknown -> the function does nothing and returns 1). Calls `0x42ACD0(name)` -> `Place::set_nation(nation)` (0x44F5C0, see above: rewrites `place.+0x26` and the nations' place rosters). Then the same two loops as above with target nation `nation`; because `place.nation` was just rewritten, "units whose nation differs from `place.nation`" means differs from the new nation: class-8 units inside the box get `0x509250(v, nation)`; class 1/4 items inside get `i.+0x08 = nation`.

## Per-tick update `0x4C55D0` -> `0x4C3330` (once per sim tick, ascending recno)

For every used place: `0x4C3330(place)`:

1. For `k = 1..parking_count`: if `parking_unit[k] != 0`: if the unit slot is empty (unit deleted) -> `parking_unit[k] = 0`; else if the unit is **not inside `place.box`** (`0x60D5F0(u+0x0E)`) -> `parking_unit[k] = 0`, `u.+0x04 = 0`, `u.+0x06 = 0` (the unit released the spot by leaving the zone; death is **not** checked).
2. `timer_ms += DAT_00A500BE`; when `timer_ms >= 60000`: if `counter < counter_max` then `counter += 1`; `timer_ms = 0` (the leftover above 60000 is discarded). So `counter` regenerates by 1 every >= 60 s of simulation time, i.e. at most one per 60 s. What it represents (garrison/recruit stock?) is TODO(re): nothing in the export reads it.

## Open questions

* Box: settled (see Construction) except the level offsets (the margin `K = DAT_009D0748` is 64.0) `ox, oy` (`loader+0x44/+0x48`); both need one number each from the binary / the level loader. The relative-box branch of the reader looks like a decompiler inversion.
* Scale-code order of the four `PLACE_SCALE` literals; meaning of `+0x52/+0x56` (starts at 400/400, +1 per 60 s up to the max; no reader in the exported code).
* `0x4C48B0` is in the export of the lights family; the facility list is filled by the facility family.
* Latent crash in the original: the barrack addition dereferences `O` (`*local_14`) without a null check; a place with `KINGDOM: UNKNOWN` (owner 0) would fault in 171-176. Shipped scripts never call these on such places (inferred). The port should treat a missing owner as "no barrack contribution".
