# Facility (shop / barracks / workshop / NPC spawn point) and its list tables SellItemRes, RecruitRes — joan.exe

Scenario *facilities* are buildings with a service attached: item shops (`f_shop.cpp`), player barracks (recruit soldiers), workshops (buy siege engines) and **NPC spawn points** (the `dock_spawn_a/b/c`, `reinforce*` facilities of the scripts).
Only `f_shop.cpp` carries an assert path; the other facility code was reached through script handlers and the sell/recruit res loaders, so most of the object layout below is
reconstructed from field accesses (status per row). Functions: [../functions/items_facilities.md §5-§6](../functions/items_facilities.md).

## 1. Container and object model

* Facilities live in a world `dynarrb` (1-based `recno`; count = `+0x0C` of the container, getter `0x43C450(recno)` → object* or NULL, emptiness test `0x44CF80(recno)`, count getter `0x417AD0`).
  The per-tick update `0x46D4A0` (`facilities_update`) is: `for i = 1 .. count: if slot i is not empty: obj->vtbl[+0x10]()`. It is called by the per-frame world update **after places (0x4C55D0) and before territories (0x4E4BB0)** (gameloop.md §4). The `[+0x10]` bodies are **not exported** (`TODO(re)`).
* All facility kinds share one object model with *typed views*: vtable slot `+0x18` = **spawn** view, `+0x1C` = **workshop** view, `+0x20` = **shop** view, `+0x24` = **barracks (recruit)** view. Script handlers always call the matching accessor first and then operate on the returned object (the view type is not checked in the handlers).
* Name lookup `search_facility_by_name` → `0x46D200`: scans `count..1` (last first), skips empty slots, compares **`obj+0x08` (char[], case-insensitive)**; returns 0 when missing.

### Common fields (reached by several views)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | vptr | `vtbl` | `+0x10` update, `+0x18/+0x1C/+0x20/+0x24` typed views | 0x46D4A0, handlers (V) |
| 0x08 | ≥ 0x20 | char[] | `name` | instance name given in the scenario (`search_facility_by_name`) | 0x46D200 (V) |
| 0x29 | 12 | f32[3] | `pos` | world position (read by `get_unit_facility_distance`, script 189) | 0x6549C0 (R) |
| 0x52 | 4 | i32 | `pool_cur` | barracks: current recruit pool (set to `Population` when the list is assigned, refilled to `pool_max` on capture) | 0x469330, 0x468790, 0x654C80 (I) |
| 0x56 | 4 | i32 | `pool_max` | barracks: maximum pool (`RecruitRes.population`, getter 0x4693B0) | (I) |
| 0x6D | 4 | Place* | `place` | the *place* (region) the facility stands on (world_nation). Place fields used here: `+0x1CA i32 spawn_count`, `+0x1CE i32 captured`, `+0x26 i32 owner_nation`, `+0x1DE` scene id | 0x467DD0, 0x654C80, 0x653CA0 (V/R) |

`is_inside_facility(unit, facility)` (script 65 → 0x653CA0; the one-argument `is_unit_inside_facility`, id 66, is 0x653D60, not read): true iff both exist, `facility.place.+0x1DE == unit.+0x335` (same scene) and the unit position `(unit+0x0E…)` is inside the place region (0x60D5F0).
`get_spawn_unit_count_in_place(place)` (script 178 → 0x6541A0) returns `place.+0x1CA`.

## 2. Spawn view (NPC barracks / spawn point) — `facility_barrack_add_leader/member`

Reset (script 192 `reset_spawn_count_in_facility`, 0x6541D0): `place.+0x1CA = 0`, then `0x467F90` = `memset(spawn+0x79, 0, 0xC3)` (clears `+0x79..+0x13B`).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x79 | 2 | u16 | `cur` | cleared to 0 by add_leader (spawned-so-far counter, runtime) | 0x467DD0 (R) |
| 0x7B | 2 | u16 | `leader_type` | unit-type recno of the leader; **a slot is "free" while 0** — add_leader is ignored when already set | 0x467DD0 (V) |
| 0x7D | 2 | u16 | `leader_level` | script 4th argument (S2: 8 for English, 5 for French) | 0x467DD0 (R) |
| 0x7F | 2 | u16 | `team` | script 3rd argument (S2: 100, 101, 4, 5): overwritten by every add_member too | 0x467DD0, 0x467EA0 (R) |
| 0x81 | 64 | char[64] | `leader_name` | full name (`"Degare Molendinarius"`) | 0x467DD0 (V) |
| 0xC1 | 64 | char[64] | `leader_short` | short name (`"Molendinarius"`) | (V) |
| 0x101 | 4 | i32 | `order` | leader initial order id (name → id via 0x4BBBC0) set by `facility_barrack_add_leader_order` (script 196) | 0x654D70 (R) |
| 0x105 | 1 | u8 | `order_flag` | | (R) |
| 0x106/0x10A/0x10E/0x112/0x11E | 4 each | i32 | `order_arg[5]` | order parameters (positions/ids) | (R) |
| 0x122 | 2 | i16 | `member_groups` | 0..4 | 0x467EA0 (V) |
| 0x124 | 8 | u16[4] | `member_type` | unit-type recno per group | (V) |
| 0x12C | 8 | u16[4] | `member_count` | units of that type per spawn | (V) |
| 0x134 | 8 | u16[4] | `member_level` | level of that group | (V) |
| 0x13C | 4 | i32 | `follow_unit` | `set_facility_follow_unit_recno` (script 190, 0x654AE0); *not* cleared by the reset | 0x654AE0 (V) |

`add_leader(f, type, team, level, name, short)` also does `place.+0x1CA += 1`; `add_member(f, type, team, count, level)` (0x467EA0) does nothing when `member_groups >= 4`, otherwise appends `(type, count, level)`, sets `team`, and `place.+0x1CA += count`. `get_spawning_count` (script 191, 0x654B30) = Σ `member_count[0..member_groups)`.
Scripts re-arm the same facility with new leaders per wave (S2L1: `reset` is not used, add_leader is ignored while the leader slot is taken).

## 3. Barracks (recruit) view

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x79 | 4 | DynArr* | `list_a` | 10-byte [RecruitEntry](#recruitres) rows copied from RecruitRes `list_a` (deleted and rebuilt by 0x468790) | 0x468790 (V) |
| 0x7D | 4 | DynArr* | `list_b` | second list (copied from the RecruitRes named by the 2nd argument) | 0x468790 (V) |
| 0x81 | 4 | i32 | `list_a_id` | RecruitRes recno | (V) |
| 0x85 | 4 | i32 | `list_b_id` | | (V) |
| 0x8D | 4 | f32 | `ratio` | `RecruitRes.selling_ratio` of list A | (V) |
| 0x91 | 2 | i16 | `spawn_facility` | recno of the paired spawn facility (script 199 `barrack_set_recruit_list(f, spawn_facility, list_a, list_b)`: argument 2 is stored here, arguments 3/4 go to 0x468790) | 0x654F10 (V) |
| 0x97 | 1 | u8 | `can_recruit` | `set_can_recruit_flag` (script 193, 0x654BC0) | (V) |
| 0x98 | 1 | u8 | `recruit_level` | `set_recruit_level` (script 194, 0x654C20); 0x468790 also sets it from `RecruitRes.level` when `level > 0` | (V) |

Assigning a list (0x468790): frees both arrays, rebuilds `list_a` from RecruitRes[A] (if its entry array exists), sets `ratio`, `recruit_level` (if `level > 0`), and if `population > 0` calls `0x469380(pop)`/`0x469330(pop)` (sets `pool_max`/`pool_cur`).
**Capture** (script 195 `set_barrack_captured(f, 1)`, 0x654C80): `place.+0x1CE = 1`; `place.+0x26 = player_unit.nation` (`unit+0x02` of `DAT_00A5B9E8`); `pool_cur = pool_max` (0x469330(0x4693B0())); clears the spawn data of facility `spawn_facility` (0x467F90, so the garrison stops spawning); swaps the flag items (0x468C10: with `captured==1` every `english_flag` (`Object\Models_Items\Flags\EnglishFlag\english_flag`) item is replaced by a `french_flag` item through 0x4C4440; with `captured==0` the reverse). Any value other than 1 does nothing.
The recruit window itself (costs, level checks, `population` decrement) is `interfac\i_barr.cpp` (HUD family) — not decoded here.

## 4. Workshop view
`workshop_set_recruit_list(f, list)` (script 200, 0x654FA0): view `+0x1C`, then `0x46B730(list)`: if RecruitRes[list] has entries: `+0xEC = list`, `+0xA2 = 0`, and for each entry `0x46A6F0(unit_type, entry.y (low 16 bits), unitres[unit_type].+0x161, ftol(…))` creates a build slot; `+0x112 = 0`. Entries of the shipped workshop lists are `CANNON/FAUCONNEAU/TREBUCHET/BATTERRAM : 2 : 0`. UI = `interfac\i_work.cpp` (HUD family).

## 5. Shop view — stock and restock

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x79 | 4 | DynArr* | `stock` | [StockEntry](#stockentry) rows | 0x4697C0, 0x4699F0 (V) |
| 0x7D | 4 | f32 | `selling_ratio` | from SellItemRes (`0.5` everywhere) | 0x4697C0 (V) |
| 0x81 | 4 | i32 | `can_regen` | `set_can_regen_sell_item` (script 198, 0x654EA0) | (V) |
| 0x89 | 4 | i32 | `list_id` | SellItemRes recno | (V) |
| 0x8D | 1 | u8 | `kind` | 0 = potion shop, 1 = weapon shop (`SellItemRes.kind`) | (V) |

* `shop_set_sell_item_list(f, list)` (script 197, 0x654E30) → `0x4697C0(list)`: deletes the old stock, creates a fresh array (element 14 B, grow 0x1E) and copies **every template entry verbatim** (item, a, count, level) **except** entries whose ItemRes has `unique != 0` and whose id is already in the GameState unique list (0x472740). Then `kind`, `selling_ratio`, `list_id` are stored.
* **Restock** `f_shop::fn_004699f0(list_id)` (this = shop): snapshot the current stock; rebuild from the template; for each template entry that passes the unique filter: `new.count = -1` if the template count is `-1` (unlimited); else `new.count = round(template.count / 2.0)` (`2.0 = [0x9CF794]`, rounding helper `0x590A20` = `ftol(x + [0x9D3E18])`, documented constant `0.0` ⇒ truncation, **verify**) and, if the item was in the snapshot (first id match), `new.count = min(template.count, new.count + old.count)`. Then `kind`, `selling_ratio`, `list_id` stored again. I.e. every restock adds **half of the template stock**, capped at the template. No caller is in the export; presumably invoked from the calendar day change for shops with `can_regen != 0` (`TODO(re)` xref 0x4699F0 — it is probably a virtual call).
* Closing the buy window writes the counters back (`0x6C3540`): for every row with `count >= 0` the window's remaining count; rows that reach 0 are deleted (0x584A10); gold is written back to `DAT_00A596C4`.

### StockEntry (14 bytes) {#stockentry}
`{ i16 item; f32 a; i32 count; i32 level }` — `a` is 1.0 in all shipped sell lists (likely a price multiplier; applied in the x87 part of the cost computation, `TODO(re)`), `count = -1` unlimited, `level` = item level handed to the buyer (`-1` in the file ⇒ `ItemRes.min_level`).

## 6. SellItemRes (`ITEM\SELLITEM.RES`, sitemres.cpp 0x4CE420; free 0x4CE300)
Table object `+0 count`, `+4 lists*`, `+8 loaded`; getter `0x469E60(recno) = lists + recno*0x1C - 0x1C`; lookup by name `0x4CE900` (script `search_item_sell_list_by_code`, case-insens., 1-based, 0 = none).

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 2 | i16 | `recno` | |
| 0x02 | 16 | char[16] | `code` | e.g. `S3L1_POTION`, `S5L1_WEAPON`, `DEFAULT` |
| 0x13 | 4 | DynArr* | `entries` | 14-byte rows (grow 0x1E) |
| 0x17 | 4 | f32 | `selling_ratio` | `SellingRatio` (0.5) |
| 0x1B | 1 | u8 | `kind` | `Type: POTION`→0 / `WEAPON`→1; absent ⇒ 0 |

Text: `VERSION 1.0 / Code : X / [Type : POTION|WEAPON] / SellingRatio : r / ITEM : a : count : level … / END`. Entry = `item_code`(→ recno, 0 if unknown), `a` (float), `count` (int, `-1` unlimited), `level` (int, `-1` ⇒ `ItemRes[item].min_level`). Examples: `fd_bread_S : 1 : 10 : 1` (10 breads, level 1), `Bkstar : 1 : 1 : 7` (one level-7 gem), `ARROW : 1 : -1 : -1` (unlimited arrows at min level). 47 lists in the Polish install.

## 7. RecruitRes (`UNIT\RECRUIT.RES`, recrures.cpp 0x4CA760; free 0x4CA640) {#recruitres}
Table object `+0 count`, `+4 lists*`, `+8 loaded`; getter `0x469520(recno) = lists + recno*0xB4 - 0xB4`; lookup by name `0x4CAD00` (script `search_recruit_list_by_code`).

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 2 | i16 | `recno` | |
| 0x02 | 32 | char[32] | `code` | `DEFAULT`, `S5L1_BARRACK`, `IRON_WORKSHOP`, … (38 lists) |
| 0x23 | 4 | DynArr* | `entries` | 10-byte rows (grow 0x1E) |
| 0x27 | 4 | f32 | `selling_ratio` | `SellingRatio` |
| 0x2B | 128 | char[128] | `location` | `Location` text (localised building name; default = `code`) |
| 0xAC | 4 | i32 | `level` | `Level` (recruit level, default -1) |
| 0xB0 | 4 | i32 | `population` | `Population` (default -1) |

Text order is fixed: `Code`, [`Location`], [`Level`], [`Population`], `SellingRatio`, then entries `UNIT_CODE : x : y` until `END`.
### RecruitEntry (10 bytes)
`{ u16 unit_type; f32 x; i32 y }` — `unit_type` = unit-res recno by code (0x5270B0). Barracks lists carry `x = 1, y = 50` for the six French/English line troops; workshop lists carry `x = 2, y = 0` for `CANNON FAUCONNEAU TREBUCHET BATTERRAM`. Meaning of `x`/`y` is decided in the HUD code (`TODO(re)`: likely `x` = unit category/cost class, `y` = batch or pool size).
