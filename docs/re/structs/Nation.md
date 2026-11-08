# Nation (`nation.cpp`) — runtime nation object and relation records

Reference build: joan.exe. Pseudocode: `$JOAN_WORK_DIR/decomp/game/world_nation/`, `game/savegame/` (ctor/roster helpers), `script/script_vm/` (builtin handlers).
All members are **packed** (odd offsets). Status: V = read in >= 2 functions, R = read once, I = inferred.
`this` is often lost by Ghidra (MSVC `thiscall` helpers show up as `thunk_FUN_x()` without argument); the receiver is stated where deduced.
Functions: [functions/world_nation.md](../functions/world_nation.md);  array set: [WorldArrays.md](WorldArrays.md).

## Where nations come from

* **`nation_res` table** (static data, `GameSet` `NATION` section, 3 rows in the shipped `Std.set`: English, French, Mercen — see [formats/gameset.md](../../formats/gameset.md) schema `NATIONCODE C8 / NAME C64 / RACE C8`).
  In memory the rows are `0x4E` (78) bytes (`0x4B4730(i) = table.data(+4) + (i-1)*0x4E`; `table.count` at +0). Row `+0x00` = nation **code string** (compared by name lookups),
  row `+0x09` = i16 id copied into `Nation+2`, row `+0x0B` = display name string (copied to `Nation+0x14`). `TODO(re)`: the loader of this table (`nationres`) is not in the export; race column position unknown.
* **Runtime `Nation` objects**: created once per `nation_res` row by `0x4B64C0` (`nation_array` load): `for i = 1..DAT_00A47BF8 (row count): 0x4B6640(row[i]+9)`;
  `0x4B6640` = `operator new(0x1545)` + ctor `0x4B3DB0` (`memset(this, 0, 0x1545)`) + `dynarrb::push` (`0x5857A0`) + `Nation+0 = <index returned by 0x417520 = the pushed 1-based slot>` + `0x4B3E20(id)` (initialiser: `+2 = id`, `strcpy(+0x14, row+0xB)`, `state(+5) = 3`, `+8 = id`, all 20 relation records reset by `0x4B8C60` (status 2), strength 50, the constants and six `rand_unit` draws listed below). The container is a global `dynarrb` of `Nation*` (1-based; `0x47B600(recno)` = `get`, `0x484870(recno)` = "slot empty"). So **nation recno == `nation_res` row number** (fresh array, no free-list reuse). Max 20 nations (relation array has room for 20).
* Teardown `0x4B6530` (`nation_array` free): for recno = count..1: if slot used: nation dtor `0x4B65F0(1)`, `dynarrb::remove`; then `dynarrb::reset`; clears `container+0x2E` (loaded flag).

## Layout (`sizeof == 0x1545 = 5445`, `operator new(0x1545)` @0x4B6640 and `memset` @0x4B3DB0)

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x0000 | 2 | u16 | `recno` | 1-based slot in the nation container; also the **phase** of the per-day AI update: `(DAT_00A47BB0 - recno) % 10 == 0` selects the days on which `0x4B8310` runs | V (0x4B6640, 0x4B4920) |
| 0x0002 | 2 | i16 | `res_id` | `nation_res` row id (`row+9`). `0x4B67E0(id)` scans recnos count..1 for `Nation+2 == id`. **Unit field `+2` and place field `+0x26` hold the nation recno, not this id** (they are equal while recno == row) | V (0x4B67E0, 0x4B6640) |
| 0x0004 | 1 | u8 | `dirty` | set to 1 by `0x4B40D0`, which `0x499380` calls first (roster reset) | R |
| 0x0005 | 1 | u8 | `state` | set to 3 by the initialiser; `0x4B4590(nation) == (state == 3)` ("active") gates the per-day strategic AI in `0x4B4920` | V (0x4B3E20, 0x4B4590, 0x4B4920) |
| 0x0008 | 4 | i32 | `res_id32` | `res_id` as int | V (0x4B3E20) |
| 0x0014 | 65 | char[] | `name` | display name copied from `nation_res` row `+0x0B` (`strcpy`) | V (0x4B3E20) |
| 0x0010 | 4 | i32 | `unit_count` | `+1` / `-1` by `0x509250` when a unit joins / leaves the nation | V (0x509250 x2) |
| 0x0055 | 20 x 0xF4 | `NationRelation[20]` | `rel[b-1]` | this nation's view of nation `b` (1-based index = other nation's recno). Accessor `0x4B42E0(b) = this + 0x55 + (b-1)*0xF4`. 20 x 0xF4 = 0x1310 ends exactly at 0x1365 | V (0x4B42E0, 0x4B4360, 0x42A1C0, 0x4B6860) |
| 0x1365 | 1 | u8 | `rel_changed` | set when a relation value changed during the day; consumed by `0x4B4920` (copied to `+0x1366`, then cleared; if set `+0x1367 = DAT_00A47BB0`) | R (0x4B4920) |
| 0x1366 | 1 | u8 | `rel_changed_prev` | previous `rel_changed` | R |
| 0x1367 | 4 | i32 | `rel_changed_date` | game date of the last change | R |
| 0x136D | 4 | f32 | `f136d` | initialised to 10.0 (`0x41200000`) | R (0x4B3E20) |
| 0x1371 | 4 | f32 | `f1371` | initialised to 5000.0 (`0x459C4000`) | R |
| 0x1385 | 4 | i32 | `i1385` | initialised to 100 (after a no-op loop writing 0 three times) | R |
| 0x1389 | 4 | i32 | `military_strength` | **initialised to 50 (0x32)**. `0x4B5520(delta)`: `+= delta`, clamped to `>= 0`. `0x509250` adds/subtracts `0x523320() * 0x527DD0(unit.type)` when a unit changes nation. `set_nation_military_strength` ends in `0x4B5580`, which is a **confirmed empty function** (23-byte frame only) -> the script call has no effect | V (0x4B5520, 0x509250, 0x4B3E20, 0x4B5580) |
| 0x14A5.. | 36 | u32[9] | `tbl14a5` | nine words copied from `.rdata` `0x9D52EC..0x9D530C` at init (strategic AI parameters; layout `+14A5,+14A9,+14AD,+14B1,+14B5,+14C9,+14CD,+14D1,+14D5`) | R |
| 0x1515..0x1529 | 6 x 4 | f32[6] | `rnd6` | **six `rand_unit()` draws at creation** (`+1515,+1519,+151D,+1521,+1525,+1529`), each in [0,1]; strategic-AI personality jitter. RNG sites for the determinism audit: 6 draws per nation at world load, in nation order | V (0x4B3E20) |
| 0x152D | 2 | i16 | `roster_a_count` | length of `roster_a` | V (0x499380, 0x4B5CC0, 0x4B5E40; BUILTINS get_team_size) |
| 0x152F | 2 | i16 | `roster_a_cap` | capacity | R |
| 0x1531 | 4 | u16* | `roster_a` | sorted array of unit recnos (u16), binary-searched with `0x5911A0(key, base, n, 2, cmp 0x4B5E10)`; tracked-alloc'd at `nation.cpp` line marker `+7`. Units of this nation for which `0x44A660(unit)` is true (= `*(i32*)(unit+0x33) != 0`) | V (0x4B5E40, 0x44A660) |
| 0x1535 | 2 | i16 | `roster_b_count` | | V |
| 0x1537 | 2 | i16 | `roster_b_cap` | | R |
| 0x1539 | 4 | u16* | `roster_b` | same format; units of this nation for which `0x4B62B0() == 0` (`0x4B62B0` = `!( *(i16*)(+0x37)==0 && *(i32*)(+0x33)==0 )` evaluated on the unit: "unit has team/leader links"); line marker `+0xF` | R |
| 0x153D | 2 | i16 | `roster_c_count` | third list, maintained elsewhere (only cleared/freed here) | R (0x499380) |
| 0x153F | 2 | i16 | `roster_c_cap` | | R |
| 0x1541 | 4 | u16* | `roster_c` | **places owned by this nation** (sorted u16 place recnos), maintained by `Place::set_nation` (0x44F5C0 -> `0x45C460` remove / `0x498B70` insert); line marker `+0x17` | V |

`0x499380` (`nation.cpp`) clears all three rosters: `0x4B40D0` (`dirty = 1`), set counts to 0, `tracked free(roster_x)` (`0x58ED70`, file `"d:\ww\nation.cpp"`, line = base marker `DAT_00A017EC + 7/0xF/0x17`), null the pointers.

`0x509250(unit, new_nation)` (**the only place where a unit changes nation**; used by `set_unit_nation`, `set_place_captured`, `set_place_nation_by_code`, ... ; `this` = unit):
if `unit+2 != new_nation`: (1) if the old nation exists: `old.roster_remove(unit)` (`0x4B5CC0`: removes the unit's recno from `roster_a` and `roster_b`),
`old.military_strength += -(0x523320() * 0x527DD0(unit.type))` (`0x4B5520`), `old.unit_count -= 1`; (2) `unit+2 = new_nation`; (3) if the new nation exists: `new.roster_add(unit)` (`0x4B5E40`: adds to `roster_a` if the unit's nation equals this nation and `0x44A660()`; to `roster_b` if `0x4B62B0() == 0`; else removes), `+strength`, `unit_count += 1`.
A nation recno of 0 (or a missing slot) skips the nation-side bookkeeping, so "neutral" units are in no roster.

## NationRelation (`0xF4 = 244` bytes; `Nation.rel[b-1]`, only some fields known)

`rel` lives inside nation **a** and describes how a regards b. `0x4B6860(a, b)` returns the hostility flag (below).

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 1 | ? | | unknown (zero at init) | |
| 0x01 | 1 | i8 | `hostile` | the "enemy" flag used by every enemy predicate. **Written only by `0x4B44E0(b, flag)`** (`rel.+1 = flag`), which `set_relation_status` calls with 1 for status 0 and 0 for status 4/3/1; status 2 leaves it unchanged. Initial value 0 | V (0x4B6860, 0x499770, 0x4B44E0, 0x4B8C60) |
| 0x02 | 1 | u8 | `flag2` | set to the same value on both sides by `0x4B4610(b, v)`; `set_relation_status(..., 0)` calls it with 0 (cleared on war). Meaning unknown (likely a truce/treaty marker) | R (0x4B4610) |
| 0x03 | 1 | u8 | `status` | relation status 0..4, written by `set_relation_status` (`0x499770`). **0 = war**, **4 = allied** (territory update treats `status == 4` as "not an intruder"; `search_for_unit_in_place 'F'` accepts `status > 3`), 1 and 3 are also special-cased (see rules). **Initial status of every relation = 2** (`0x4B8C60` memsets the 0xF4 record and sets `+3 = 2`), i.e. neutral: not hostile, not allied. S1L1 sets `("ENGLISH","FRENCH",0)` | V (0x499770, 0x4B4360, 0x4E4BB0, 0x49C240) |
| 0x04 | 4 | i32 | `status_date` | `DAT_00A47BB0` (game date, days) of the last status change | V (0x499770) |
| 0x08 | 4 | i32 | `friendship` | written by `set_nation_friendship` (`0x42A1C0(b, value)` on nation a): `*(i32*)(a + 0x5D + (b-1)*0xF4) = value`, **no clamping in the handler**; scripts use -77 | V (0x42A1C0, 0x653DF0) |
| 0x12 | 2 | i16 | `friend_cap` | upper bound for friendship: when `status*20 < friend_cap` the cap is lowered to `status*20` (so status 0 -> 0, 4 -> 80) | R (0x499770) |

Rules of `0x499770(this = nation a; b: i16, status: u8, mirror: char)` (`set_relation_status`):

* ignored when `b == a.recno`.
* **Hysteresis**: it also does nothing when `mirror == 0` **and** `rel[b].status == 1` **and** the new `status == 0` **and** `rel[b].status_date + 5 > DAT_00A47BB0` (a status-1 relation cannot be turned into war within 5 game days). (Compound condition read from the decompile: `mirror != 0 || rel.status != 1 || status != 0 || date+5 <= now` allows the change.)
* otherwise (`0x4B4590(this)` is just `state == 3` and its result is not used here): `rel.status = status`, `rel.status_date = now`, `friend_cap = min(friend_cap, status*20)`;
  `0x4B44E0(b, 0)` for status 4/3/1 and `0x4B44E0(b, 1)` for status 0 (this sets the `hostile` flag);
  if `mirror == 0`: call the same function on nation `b` with `(a.recno, status, mirror = 1)` (**the status is symmetric**); and if `status == 0`: `0x4B4610(b, 0)`.

Hostility predicate `0x4B6860(a, b)` (V): `a == b -> 0` (**including both 0**), `a == 0 or b == 0 -> 1` (the neutral/unknown nation 0 is hostile to every real nation), else `Nation[a].rel[b].hostile`.
All enemy counters additionally skip units whose nation is 0 explicitly (`unit.nation != 0` test), see [Place.md](Place.md).

## Who is enemy of whom in S1L1

`S1L1.sce` (lines 12-14) calls `set_nation_friendship("ENGLISH","FRENCH",-77)`, the reverse, and `set_relation_status("ENGLISH","FRENCH",0)`. Result: English and French are at war (status 0 on both sides, `hostile = 1` both ways, friendship -77 both ways). **Hostility rule (settled): `hostile(a,b)` = 1 iff the last `set_relation_status` between them was 0; statuses 1, 3, 4 clear it; status 2 (the initial state) never sets it.** There is no other writer (`friendship` is never read by the exported code).
`Mercen` has no script-set relation in S1L1: all its relations stay at the initial status 2 with `hostile = 0`, so **Mercen units are not enemies of English or French (nor allies)** unless a later scenario script changes it. Nation 0 (`KINGDOM: UNKNOWN`) remains hostile to every real nation by the `0x4B6860` rule.

## Name lookup (`0x4B91A0` + `0x4B67E0`)

`0x4B91A0(this = nation_res, name)`: linear scan of rows `0..count-1`, comparison `FUN_007C07B0(row + i*0x4E, name) == 0`, returns `i+1` or 0. `0x7C07B0` is the project-wide case-insensitive compare (inferred; evidence below).
`0x4B67E0(row)` maps the row number to the live nation recno (see `res_id`). Scripts write `"ENGLISH"`, `"FRENCH"`; `.pla` files write `english`, `French`, `Mercen` in mixed case -> the compare **must be case-insensitive** (ASCII). Handlers hard-code the literals `"FRENCH"`, `"ENGLISH"` (0x6542B0) and `"Neutral"` (0x65F850, territory only -> nation -1).
