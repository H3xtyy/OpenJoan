# world_nation family — world arrays, nations, places, territories, light modifiers, water

Reference build: `joan.exe`. Export: `$JOAN_WORK_DIR/decomp/game/world_nation/` (442 functions, depth <= 2; most depth-2 entries are unrelated `world_s`/`plane`/`ilib` helpers pulled in by callee closure).
Handler bodies for the script builtins of this family were read from `$JOAN_WORK_DIR/decomp/script/script_vm/` and the unit/item helpers from the neighbouring exports.
Confidence: **V** = read in >= 2 consistent places, **R** = read once, **I** = inferred (reason given). Ghidra loses the ECX receiver of most MSVC `thiscall`/`fastcall` helpers (they show as `thunk_FUN_x()` with no argument, or `local_8` is used undeclared);
the receiver is stated where it could be deduced. Dynarr/dynarrb layout: [flocksys.md §2](flocksys.md). Game clock: `DAT_00A500BA` = absolute sim time in ms, `DAT_00A500BE` = this tick's step in ms ([gameloop.md](gameloop.md)), `DAT_00A47BB0` = game date (days), `DAT_00A5B9E8` = player-controlled unit recno.
Structs: [Nation](../structs/Nation.md), [Place](../structs/Place.md), [Territory](../structs/Territory.md), [WorldArrays](../structs/WorldArrays.md), [World](../structs/World.md).

## 0. What this family contains (read first)

* **World loading/unloading of the per-scenario array set** (0x470D40 / 0x471010 order) - almost all array `load` routines are *empty*; the arrays are filled later by the level loader (see WorldArrays.md).
* **Per-tick updates** called from `0x4E5FC0` ([gameloop.md](gameloop.md) §4): `0x4B0780` modify_light, `0x487960` glowing lights, `0x547030` water (empty), `0x431F20` water-wave activation, `0x4913C0` throw items, `0x4C55D0` places, `0x4E4BB0` territories.
* **Nations**: creation from `nation_res`, relation records, hostility predicate, roster/strength bookkeeping on nation change.
* **Place and territory queries** behind the script builtins 111, 168-185, 201-203, 268, 342-343, 397 (handlers in section 4).
* `world.cpp` / `world_s.cpp` functions in the export are **already documented**: container/BVH in [gameloop.md §7](gameloop.md) + [World.md](../structs/World.md), level loading in [world_binary.md](world_binary.md), `.pln/.gri` formats in docs/formats. They are listed in section 6 only as cross-references.
* Round 2 (`$JOAN_WORK_DIR/decomp/game/world_nation2/`) resolved: `0x4B3E20` (nation init), `0x4B4590` (= `state == 3`, **not** a hostility recompute), `0x4B44E0` (sets `hostile`), `0x4B4610`, `0x55A090` (`CreatePlace` reader), `0x4F9330` (`CreateTerritory` reader), `0x44F5C0` (`Place::set_nation`), `0x4B5580` (confirmed empty), `0x44A660`, the place/territory allocators.

## 1. Load / unload

| Address | Name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x470D40 | `game_load_world_arrays` | guard `GameState+4 == 0`; clears scratch (`+0x308`,`+0x744`,`+0x71B`,`+0x738/+0x73C`, `DAT_00A4D765`); unless `scenario(+0x17) == 6`: clears `+0x309`/`DAT_00A4D80F` and runs the 18 loads in the order modify_light, world, nation, place, facility, territory, unit, item, modify_item, throw_item, water, water_wave, talk, news, animation_particle_effect, glowing_light, trigger, dynamic_grass; sets `+4 = 1` | log strings `"Loading ..."` | V |
| 0x471580 | `game_load_progress_text` | `ret` only (the progress strings are discarded) | body | V |
| 0x471010 | `game_world_unload` | reverse sequence of frees; documented in [WorldArrays.md](../structs/WorldArrays.md) (export in `game/battle/`) | body | V |
| 0x4B0510 / 0x4C5240 / 0x46CDA0 / 0x4E4990 / 0x49E9A0 / 0x4AF9E0 / 0x4E9C30 / 0x546D60 / 0x41B510 / 0x4876B0 / 0x4EEDF0 | `*_array_load` (11 empties) | `ret` only; arrays are static and filled by `Create*` readers | bodies | V |
| 0x561050 | `world_array_load` | `+0x32 = 0`, `+0x36 = 0`, `+0x2E = 1` | body | R |
| 0x4B64C0 | `nation_array_load` | for `i = 1..DAT_00A47BF8`: `0x4B6640(nation_res.row(i)+9)` | body | V |
| 0x4B6640 | `nation_create(res_id)` | `new Nation(0x1545)` (ctor 0x4B3DB0 zero-fills), `dynarrb::push`, `Nation+0 = slot`, `0x4B3E20(res_id)` | body | V |
| 0x4B6530 | `nation_array_free` | descending: dtor `0x4B65F0(1)` + `remove`; `reset`; `container+0x2E = 0` | body | V |
| 0x4B0530 | `modify_light_free_all` | descending: `0x4B05F0(1)` (dtor + delete) + `0x5858C0(i)`; then `0x585D20` | body | V |
| 0x4876D0 | `glowing_light_free_all` | same shape (dtor `0x487790`) | body | V |
| 0x4E9F10 | `throw_item_free_all` | same shape (dtor `0x4EA370`) | body | V |
| 0x51DCC0 | `unit_array_load` | unit-manager u16 defaults + the four selection-ring models `"MODEL\\select"`..`select4` (`modelres 0x4B18D0`) | body | V |
| 0x56ED40 | `water_wave_load` | wave radius 800, shared `"WAVE_EFF"` model + anim, material flag `0x100` | body | V |
| 0x4641A0 | `dyngrass_init(capacity, enabled)` | see [DynGrass.md](../structs/DynGrass.md) | body | V |
| 0x4E3740 | `talk_res_load` | `0x5919A0(&DAT_00A45FA8, this, 0, 0x32, 1, this+4, 0x2E)` + `dynarrb::reset` | body | I (args only) |
| 0x4B9250 | `news_array_load` | `0x4B92C0()`, `0x4B9340(&DAT_00A48EC8)` | body | I |

## 2. Nation functions

| Address | Name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x4B4730 | `nation_res_row(i)` | `*(table+4) + (i-1)*0x4E` | body | V |
| 0x4B91A0 | `nation_res_find(name)` | linear scan of rows, `0x7C07B0(row+0, name) == 0` -> `i+1`, else 0 (case-insensitive) | body + script literals | V |
| 0x4B67E0 | `nation_recno_from_res_id(id)` | descending scan `Nation.+2 == id` | body | V |
| 0x47B600 | `nation_get(recno)` | `dynarr_get` wrapper; `0x484870(recno)` = slot empty | bodies | V |
| 0x4B42E0 | `nation_relation(b)` | `this + 0x55 + (b-1)*0xF4`, this = Nation a | body | V |
| 0x4B4360 | `nation_relation_status(b)` | byte at `this + 0x58 + (b-1)*0xF4` (= `rel.status`) | body | V |
| 0x4B6860 | `nation_is_hostile(a, b)` | `a == b -> 0`; either 0 -> 1; else `Nation[a].rel[b].hostile` (byte `+1`) | body | V |
| 0x42A1C0 | `nation_set_friendship(b, v)` | `*(i32*)(this + 0x5D + (b-1)*0xF4) = v` (no clamp) | body | V |
| 0x499770 | `nation_set_relation_status(b, status, mirror)` | symmetric status update with 5-day hysteresis; see Nation.md | body | V |
| 0x4B5520 | `nation_add_strength(delta)` | `strength(+0x1389) += delta`, floor 0 | body | V |
| 0x4B5CC0 | `nation_roster_remove(unit)` | binary-search remove of `unit.recno` from both rosters | body | R |
| 0x4B5E40 | `nation_roster_add(unit)` | add/remove in roster A (predicate `0x44A660`) and B (`0x4B62B0 == 0`) | body | R |
| 0x499380 | `nation_reset_rosters` (`nation.cpp`) | `dirty = 1`, free and null the three rosters (counts to 0) | body | V |
| 0x509250 | `unit_set_nation(new)` | full change of a unit's nation incl. rosters, strength, unit counts | body | V |
| 0x509400 | `unit_nation_object` | `0x47B600(unit.+2)` unless the slot is empty (-> 0) | body | R |
| 0x4B4920 | `nation_day_update` (battle export) | per game day: `0x4B5750`, `0x4B5A90`, relation-changed bookkeeping, `0x4B4590`; if it returns non-0: `0x4B7CB0`, every 10th day offset by `recno` `0x4B8310`, then `0x4B8B20`, `0x4B8C20` (strategic AI; owner: battle family) | body | R |

### 2b. Round-2 additions (creation and relation setters)

| Address | Name | Purpose | Conf |
|---|---|---|---|
| 0x4B3E20 | `nation_init(res_id)` | `+2 = id`, name copy, `state = 3`, 20 relation records reset (`0x4B8C60`: memset 0xF4, status 2), strength 50, constants, 6 x `rand_unit` | V |
| 0x4B4590 | `nation_is_active` | `state(+5) == 3` | V |
| 0x4B44E0 | `nation_set_hostile(b, flag)` | `rel[b].+1 = flag` | V |
| 0x4B4610 | `nation_set_rel_flag2(b, v)` | `rel[b].+2 = v` on this and `Nation[b].rel[this].+2 = v` | V |
| 0x4B8C60 | `nation_relation_reset` | memset 0xF4, status 2 | V |
| 0x44A660 | `unit_has_link33` | `*(i32*)(unit+0x33) != 0` (roster A predicate) | V |
| 0x55A090 | `place_read_create_place` | `CreatePlace` reader: alloc 0x1F6, name/scale, box, entrances (<=8), parking (<=13), kingdom; see Place.md | V |
| 0x4C5370 | `place_alloc` | `new Place(0x1F6)` + push + `recno` | V |
| 0x433EA0 | `place_init(kind, scale, name)` | name (32), counter/max 400/400, timer 0, spawn 0 | V |
| 0x42B350 | `place_set_pos_box(x,y,z,box)` | position + box | V |
| 0x438420 / 0x4C22B0 | `place_add_parking(pos, dir)` / `place_add_entrance(pos)` | caps 13 / 8 | V |
| 0x44F5C0 | `place_set_nation(nation)` | rewrites `+0x26`, nation `roster_c`, notifies facilities | V |
| 0x45C460 / 0x498B70 | `nation_places_remove(place)` / `nation_places_insert_if_owned(place)` | sorted u16 place roster `+0x1541` | V |
| 0x4F9330 | `territory_read_create_territory` | `CreateTerritory` reader; see Territory.md | V |
| 0x4E4A50 | `territory_alloc` | `new Territory(0xE0)` zeroed + push + recno | V |

## 3. Place / territory / light / water / projectile updates

| Address | Name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x4C55D0 | `place_update_all` | ascending `1..count`: if used, `0x4C3330(place)` | body | V |
| 0x4C3330 | `place_update` | free parking slots whose unit vanished or left the box; every >= 60000 ms `counter += 1` up to `counter_max` | body | V |
| 0x456C70 / 0x456CA0 | `place_get` / `place_slot_empty` | `dynarr_get` wrappers | bodies | V |
| 0x4C54A0 | `place_find_by_name` | ascending, `0x7C07B0(place+5, name)`, first match | body | V |
| 0x4C46F0 / 0x654530 | `place_set_captured` / `place_get_captured` | `+0x1CE` | bodies | V |
| 0x4C48B0 | `place_facility_head` | `+0x1EE` | body | V |
| 0x4E4BB0 | `territory_update` | player-position notification (4 s throttle) + one-territory-per-tick contested-flag scan; see Territory.md | body | V |
| 0x420FF0 / 0x4217E0 / 0x4E4B20 | `territory_get` / `territory_slot_empty` / `territory_find_by_name` | descending name scan (last match) | bodies | V |
| 0x4DD720 / 0x4DD760 / 0x4DD7A0 / 0x4DD7E0 / 0x4DD820 / 0x4DD860 | `territory_notify_*` | `0x5A3090(table, &DAT_009D6300, id, name)` with id 5 / 6 / 7 / 8 / 9 / 10 | bodies | V |
| 0x5A3090 | `event_table_dispatch(name, id, arg)` | linear search in a table of 0x15-byte entries `{.., +4 id, +8 name[9], +0x11 callback}` for `(name, id)`, then call the callback; (a miss uses index = count: out-of-range read in the original) | body | R |
| 0x4B0780 | `modify_light_update_all` | descending: `0x4AFD60(entry)`; delete the entry when its light list is empty | body | V |
| 0x4AFD60 | `modify_light_update` | mode 1 flicker / mode 2 ramp, writes `light+0x4C`, sets dirty (`0x5C5C00`) | body | V |
| 0x4B01E0 | `modify_light_light_count` | `entry+0x20 == 0 ? 0 : count(list)` | body | V |
| 0x590660 / 0x590B00 | `rand_unit()` / `clamp(x, lo, hi)` | `rand15 * DAT_009D8C10` (0x5905A0 = LCG) / `x < lo ? lo : x > hi ? hi : x` | bodies | V |
| 0x49B970 | `light_parse_create_light` (CreateLight tail) | parses `Group`, `ModifyRate`, `ModifyRange`; creates/joins the ModifyLight group | body | R |
| 0x487960 | `glowing_light_update_all` | fade all, then one occlusion-tested light per tick (round robin, cursor `+0x56`) | body | V |
| 0x486910 | `glowing_light_update(with_test)` | distance-based target intensity and fade; returns `needs_los` | body | V |
| 0x486DE0 | `glowing_light_line_of_sight` | camera-to-light ray vs scene collision planes | body | R |
| 0x547030 | `water_update` | **empty** (`ret`) | body | V |
| 0x431F20 | `water_wave_update` | wave-actor activation around the camera-followed unit (radius 800) | body | R |
| 0x469D10 | `water_body_at(pos, radius)` | descending scan of water bodies for XY rectangle containing `pos` expanded by `radius` | body | R |
| 0x4913C0 | `throw_item_update_all` | descending; expiry rules then `0x4FCC00` | body | R |
| 0x4FCC00 | `throw_item_step` | flight/impact step (arrow trail, explosive-hit, cannon) | body | R |
| 0x4EA9C0 | `throw_item_remove(id)` | dtor + `dynarrb::remove` | body | V |
| 0x590540 | `rng_next_below(n)` | `s = s*22695477 + 1; ((s>>16)&0x7FFF) * n >> 15`, state at `rng+0x7A` | body | V |

Common predicates/accessors used above: `0x60D5F0(box, p)` inclusive 3D point-in-AABB (`box[0..2]` min, `box[3..5]` max) - V; `0x4FEDB0(unit, out)` copies position `unit+0x0E/+0x12/+0x16` - V; `0x428B70(recno)`/`0x41B3C0(recno)` unit slot empty / unit get; `0x42D0A0(type)` unit-type record (0x203 stride, 1-based); `0x42D0E0(unit)` = dead (`+0x135 < 1 && +0x120 == 0`); `0x432B90(unit)` = byte `+0x2E1`; `0x429090(box, &n)` = `World::query_blocks` (BVH walk 0x54AD90); `0x54EE10(block)` = `block+0x8C` (unit chain head).

## 4. Script builtin handlers (ids from [BUILTINS.md](../../scripting/BUILTINS.md))

"R+" = handler body and helper read in this round (upgrade from the `N` of BUILTINS.md). Argument order = script order. All return 1 on success, 0 on a failed lookup unless stated.

| id | name | handler | what it does to the family's state | Conf |
|---|---|---|---|---|
| 111 | set_units_in_place_can_bribe_flag | 0x656030 | `(type, place, value)`; see Place.md (chain-head type test quirk); 0 if place empty/`scene == 0` | R+ |
| 168 | search_place_by_name | 0x653EB0 -> 0x4C54A0 | ascending, case-insensitive, first match; null name -> 0 | R+ |
| 169 | search_for_unit_in_place | 0x653EF0 -> 0x49C240 | modes `"P"`, `"E"`, `"F"` on `*mode`; returns the unit recno or 0 | R+ |
| 170 | is_unit_inside_place | 0x653F30 | place used, unit slot used, unit position inside the place box; no alive check | R+ |
| 171 / 172 | number_of_enemy / number_of_fightable_enemy_in_place | 0x653FE0 / 0x654020 -> 0x49C470 / 0x49E500 | `(place, unit)`; counts incl. queued barrack troops, see Place.md | R+ |
| 173 / 174 | enemy_exist / fightable_enemy_exist_in_place | 0x654060 / 0x6540A0 -> 0x4A65D0 / 0x4A8C20 | boolean forms | R+ |
| 175 | number_of_people_by_nation_in_place | 0x6540E0 -> 0x4AB020 | **boolean** (any), see Place.md | R+ |
| 176 | people_by_nation_exist_in_place | 0x654120 -> 0x4B79A0 | **count** | R+ |
| 177 | general_by_nation_exist_in_place | 0x654160 -> 0x4C2B50 | count of units with rank (`+0x104`) >= 4 | R+ |
| 178 | get_spawn_unit_count_in_place | 0x6541A0 | `place+0x1CA` | R+ |
| 179 | change_all_item_in_place | 0x654240 -> 0x4C4440 | replace items of one type by another inside the box, keeping pose, owner = place nation; returns 1 | R+ |
| 180 | get_place_captured | 0x654280 -> 0x654530 | `place+0x1CE` | R+ |
| 181 | set_place_captured | 0x6542B0 | flag + flip class-8 units and class 1/4 items inside to the other side; Mercen/none places only store the flag | R+ |
| 182 | set_place_nation_by_code | 0x654560 | `Place::set_nation(nation)` (rewrites `place.+0x26` and rosters) then re-owner class-8 units / class 1/4 items inside to that nation | R+ |
| 183 / 184 | get/set_place_unit_view_angle | 0x6547A0 / 0x6547E0 | `+0x1D2`, degrees <-> radians (`DAT_009CF884`) | R+ |
| 185 | set_parking_dir | 0x654830 | all parking headings = `deg * pi/180` | R+ |
| 201 | set_nation_friendship | 0x653DF0 | `(a, b, v)`: both names must resolve to a nation; `Nation_a.rel[b].friendship = v` (directional, no clamp); returns 1 / 0 | R+ |
| 202 | set_relation_status | 0x655010 -> 0x499770 | `(a, b, status u8)`: symmetric status with hysteresis (Nation.md); returns 1 / 0 | R+ |
| 203 | set_nation_military_strength | 0x6550D0 -> 0x4B5580 | resolves the nation, then calls `0x4B5580(value)` which is a **confirmed empty function** (no effect; the nation's initial strength is already 50, S2L1 passes 50) | V |
| 268 | set_end_condition_territory_link | 0x65A460 -> 0x467390 | stores a copy of the string in the end-condition object (`end_con.cpp`) | R+ |
| 342 | number_of_enemy_in_world | 0x65CB80 | for unit `u0` with `u0.+0x335 != 0`: count live units `v` with `v.+0x335 == u0.+0x335`, `typedef(v).subtype in {0,1}`, `typedef(v).class != 0`, `H(u0.nation, v.nation) != 0`; else 0 | R+ |
| 343 | number_of_friend_in_world | 0x65CD00 | same `+0x335` group, `v.+0x2F == 0`, alive, subtype in {0,1}, class not in {0,8,9}, `v.nation == u0.nation` (includes `u0` itself) | R+ |
| 397 | set_territory_nation | 0x65F850 | descending name lookup (0x4E4B20); nation by name, `"Neutral"` -> -1, unknown -> 0; `T.+0x38 = nation`, `T.+0x46 = 0` | R+ |
| 93 | set_unit_nation | 0x655750 | nation by name; unknown -> `unit.+2 = 0` (no roster work), else `0x509250` | R+ |
| 16 / 144 / 145 | set_item_nation / set_team_nation(2) | 0x652010 / 0x6582F0 / 0x658420 | use the same name -> recno chain (0x4B91A0 -> 0x4B67E0); bodies in the item/unit families | R |

Arithmetic note for 342/343: field `+0x335` is read from the unit as an `int`; BUILTINS.md calls `+0x31` the team id - whether `+0x335` is a different grouping (e.g. current formation/squad) is TODO(re) for the unit family.

## 5. Constants

| Value | Meaning | Where |
|---|---|---|
| `0x1545` (5445) | `sizeof(Nation)` | 0x4B6640 / 0x4B3DB0 |
| `0xF4` (244), 20 entries | `NationRelation` stride / capacity (`0x55 + 20*0xF4 = 0x1365`) | 0x4B42E0 |
| `0x4E` (78) | `nation_res` row size in memory | 0x4B4730, 0x4B91A0 |
| `0x203` (515) | unit-type record size | 0x42D0A0 |
| 4 | max areas per territory | 0x4E4BB0 |
| 3999 (`> 3999` = `>= 4000` ms) | territory notification throttle | 0x4E4BB0 |
| 60000 ms | place counter regeneration period | 0x4C3330 |
| 5 days | status-1 -> 0 hysteresis (game-date days) | 0x499770 |
| `status * 20` | friendship cap per relation status | 0x499770 |
| 10 days | nation AI cadence (phase = nation recno) | 0x4B4920 |
| 800 | wave activation half-extent | 0x56ED40 |
| 1000.0 (`DAT_009CF778`), 1.0 (`DAT_009CF784`), 0.5 (`DAT_009CF77C`), 0.0 (`DAT_009CF5C4`), pi/180 (`DAT_009CF884`) | confirmed float constants (other docs) | various |
| `DAT_009CFF30` = 2500.0, `DAT_009D3778` = 1500.0, `DAT_009D3774` = 0.007 | glowing-light max range, falloff length, fade rate per ms (decoded floats) | 0x486910 |
| `DAT_009D8C10` = 3.0518e-05 (1/32767) | `rand_unit` scale | 0x590660 |
| `DAT_009D6300` = `"SMS"` | name of the territory notification channel | 0x4E4BB0 |
| 400 / 400 | initial place `counter` / `counter_max` | 0x433EA0 |
| 13 / 8 | max parking spots / entrances per place | 0x438420, 0x4C22B0 |
| 0x1F6 / 0xE0 | `sizeof(Place)` / `sizeof(Territory)` | 0x4C5370, 0x4E4A50 |
| 50 / 100 / 10.0 / 5000.0 | nation init: strength, `+0x1385`, `+0x136D`, `+0x1371` | 0x4B3E20 |
| `0x2443` | unit-type flag mask for 'E'/'F' searches | 0x49C240 |
| 0x28 (40) | end-condition slots cleared on unload | 0x471010 |

RNG sites (determinism audit): (0) `0x4B3E20`: six `rand_unit` draws per nation at world load (nation order 1..3); (1) `0x4AFD60` mode 1: `rand_unit` (0x590660) once per update plus once per phase restart, 1 draw per light group per tick on average; (2) `0x4FCC00`: `0x590540` picks `EXPLOSIVEHIT1..3` once per impact. No other randomness in this family (water, glowing lights, places, territories, nations are deterministic).

## 6. Cross-references: functions in the export that are documented elsewhere

| Address | Name | Page |
|---|---|---|
| 0x42CE10, 0x440320, 0x44A260, 0x547BF0, 0x4426A0 | `World::add_scene`, `destroy`, `build_bvh`, `add_blocks`, `unload_contents` | [gameloop.md §7](gameloop.md), [World.md](../structs/World.md) |
| 0x4F2F80, 0x4F6EB0, 0x558140, 0x55B720, 0x55F320, 0x55FA40, 0x560120, 0x560C70, 0x54E930, 0x42B270, 0x43D340 | `world_s.cpp` level loading, terrain build, pln/gri | [world_binary.md](world_binary.md), docs/formats/{pln,gri}.md |
| 0x5EA460, 0x5C* , 0x5CD* , 0x5D* | en3d scene/plane/morph/actor | [scene.md](scene.md), [actor.md](actor.md) |
| 0x58E7B0, 0x58EB30, 0x58ED70, 0x585xxx | tracked alloc/free/dynarr | [flocksys.md §2](flocksys.md) |

## 7. Cross-module requests

* Rename in Ghidra: `0x470D40 game_load_world_arrays`, `0x4C55D0 place_update_all`, `0x4C3330 place_update`, `0x4E4BB0 territory_update`, `0x4B0780 modify_light_update_all`, `0x4AFD60 modify_light_update`, `0x487960 glowing_light_update_all`, `0x486910 glowing_light_update`, `0x4913C0 throw_item_update_all`, `0x4FCC00 throw_item_step`, `0x431F20 water_wave_update`, `0x499770 nation_set_relation_status`, `0x42A1C0 nation_set_friendship`, `0x4B6860 nation_is_hostile`, `0x4B91A0 nation_res_find`, `0x4B67E0 nation_recno_from_res_id`, `0x4C54A0 place_find_by_name`, `0x4E4B20 territory_find_by_name`, `0x509250 unit_set_nation`, `0x429090 world_query_blocks`.
* Types to create: `Nation` (0x1545) + `NationRelation` (0xF4), `Place`, `Territory`, `ModifyLight`, `GlowingLight`, `ThrowItem` (offsets in the struct pages). Set `__thiscall` receivers: 0x4B42E0, 0x4B4360, 0x42A1C0, 0x499770 (Nation), 0x4C3330 (Place, `__fastcall`), 0x4AFD60 (ModifyLight), 0x486910 (GlowingLight), 0x4FCC00 (ThrowItem, `__fastcall`).
* Unit family must agree on: `unit.+0x00 recno`, `+0x02 nation`, `+0x04/+0x06 parking place/slot`, `+0x08 territory`, `+0x0E position (z up)`, `+0x3B type`, `+0x104 rank`, `+0x120/+0x135 alive test`, `+0x2E1`, `+0x2EB bribe flag`, `+0x335`, `+0x33D next`; unit-type `+0x02 class`, `+0x118 subtype`, `+0x11C flags`.
* BUILTINS.md: update rows 111, 168-185, 201-203, 268, 342, 343, 397 from `N` to `R` using section 4; note the 175/176 behaviour swap.
* Lights family: `ModifyLight` writes `light+0x4C` and sets `light+0x5A`; the creation-side token list of `CreateLight` is in 0x49B970.
* Round-2 corrections to apply in Ghidra: `0x4B4590 nation_is_active`, `0x4B44E0 nation_set_hostile`, `0x44F5C0 place_set_nation`, `0x55A090 place_read_create_place`, `0x4F9330 territory_read_create_territory`, `0x4B3E20 nation_init`, `0x4B5580` = empty stub.

## 8. Open questions (remaining after round 2)

* TODO(re) `.rdata` values: `DAT_009D0748` (place box z margin `K`), the three territory `Type` literals at `0x9D80D8/E0/E8` (camp/city/town order), the four `PLACE_SCALE` literals, the multiplier turning `rand_unit` into the modify-light phase length (0x4AFD60 first branch), the 9-word table `0x9D52EC..0x9D530C` (nation AI parameters).
* TODO(re) level origin offsets `loader+0x44/+0x48` added to place/light X,Y (set by 0x4F2F80).
* TODO(re) meaning of place `+0x52/+0x56` (400/400, +1 per 60 s), `rel.+2` flag, nation `+0x1385/+0x136D/+0x1371`; who writes place `+0x1CA`.
* TODO(re) dispatcher case order for ids 175/176 (handler swap); `unit.+0x335` vs team `+0x31`; subscriber of event channel `"SMS"` ids 5..10.
* Latent original bug: null owner dereference in the barrack addition of 0x49C470/49E500/4A65D0/4A8C20/4AB020/4B79A0; `set_territory_nation` with an unknown territory name.
