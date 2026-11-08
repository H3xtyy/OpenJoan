# UnitLevel — level table, upgrade lists, upgrade records (`levelres.cpp`, `unitlvl.cpp`, `unitures.cpp`, `upgrares.cpp`)

Everything that turns experience into stats. The unit-side fields (`Unit+0x124 level`, `+0x126 experience`, `+0x12A skill points`, `+0x12C attribute points`, `+0x130/+0x131 level-up flag/time`,
`+0x1C1->Equipment`, per-unit upgrade slots) are on [Unit.md](Unit.md); the functions are in [../functions/unit.md](../functions/unit.md) §8.

## 1. LEVEL table (`levelres.cpp`)

Source: DBF section `LEVEL` of `Resource/Std.set` (schema `LEVEL N2, EXPERIENCE N9, ATT_POINT N2, SKI_POINT N2`, 30 rows — [../../formats/gameset.md](../../formats/gameset.md) schema N).
Loader `levelres::fn_004A4CA0` (**0x4A4CA0**, opens section `LEVEL` through 0x58B180): allocates `count*7` bytes (`levelres.cpp`),
row `i` (0-based) at `base + i*7`:

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0 | 1 | u8 | `level` | 1..30 |
| 1 | 4 | i32 | `exp` | cumulative experience at which this level is **completed** (the unit advances to `level+1` when `experience >= exp`) |
| 5 | 1 | i8 | `att_points` | attribute points granted on completing the level |
| 6 | 1 | i8 | `skill_points` | skill points granted |

Table object: `+0 i32 count`, `+4 row*`, `+8 u8 loaded` (free `levelres::fn_004A4C30`). Lookup by *level number*: `0x4A4E60(level)` scans rows for `row.level == level`
(linear, returns pointer or NULL; `level` 0 and `level 31` are NULL). Shipped values (read from `Std.set`, all 30 rows verified against the file):

| level | exp | att | skill | level | exp | att | skill | level | exp | att | skill |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 1 | 1000 | 5 | 1 | 11 | 209470 | 7 | 1 | 21 | 1347470 | 10 | 2 |
| 2 | 3200 | 5 | 1 | 12 | 272260 | 8 | 1 | 22 | 1536160 | 10 | 2 |
| 3 | 5800 | 5 | 1 | 13 | 326830 | 8 | 1 | 23 | 1735850 | 10 | 2 |
| 4 | 10400 | 6 | 1 | 14 | 392170 | 8 | 1 | 24 | 1961480 | 11 | 2 |
| 5 | 17800 | 6 | 1 | 15 | 485870 | 8 | 1 | 25 | 2306660 | 11 | 2 |
| 6 | 28500 | 6 | 1 | 16 | 573300 | 9 | 2 | 26 | 2583550 | 11 | 2 |
| 7 | 45600 | 6 | 1 | 17 | 676600 | 9 | 2 | 27 | 2893510 | 11 | 2 |
| 8 | 73000 | 7 | 1 | 18 | 784870 | 9 | 2 | 28 | 3240780 | 12 | 2 |
| 9 | 109600 | 7 | 1 | 19 | 910440 | 9 | 2 | 29 | 3629740 | 12 | 2 |
| 10 | 161070 | 7 | 1 | 20 | 1108880 | 10 | 2 | 30 | 4065260 | 12 | 2 |

Unit init (0x4F8550) seeds `Unit+0x126 = 0` when `Level < 2` and `row(Level-1).exp` otherwise, i.e. a level-3 unit starts with 3200 experience.

## 2. Unit upgrade lists (`unitures.cpp`)

Source: DBF section `UPGRADE` of `Std.set` (43 rows, schema `CODE C32, DBFNAME C8`; each row names another section such as `JOANUP`, `E_INFUP`, ... whose rows are `LEVEL N2, CODE C32`).
Loader `unitures::fn_00530D70` (**0x530D70**; free 0x530D00): list-table object `+0x23 header, +0x2C i32 count, +0x30 entry* (count*4), +0x34 u8 loaded`.
Per row: `i16 level = atoi(row[0..2])`, `i16 upgrade_id = 0x531B20(row+2, 0x20)` (case-insensitive search of the upgrade CODE in the UPITEM table, 1-based, 0 = unknown).
So an **upgrade list** is an array of `{level, upgrade_id}` (4 bytes). Per unit type the list is selected by `UnitRes+0x169` (`0x531E70(idx)`; list record stride **0x35**, 1-based, fatal
"UpgradeRes::get_unit_upgrade_list_res" when out of range). Accessor of the n-th entry: `0x5239E0(n)` = `array + (n-1)*4`.

`Unit` side (0x523240 `unlock_upgrades_for_level(level)`): for every entry whose `level == level` call `0x5313D0(upgrade_id)`; that function walks the unit's **upgrade slot array**
(8-byte elements `{i16 id, i16 max_level, i16 cur_level, i16 flags=1}`, dynarr created lazily with elem-size 8) and either increments the existing slot's `max_level` or appends `{id,1,0,1}`.
Called at unit creation with level 0 (0x4F8550) and on every level-up with the level just completed (0x522C40). `0x5315A0(slot)` = `if cur<max then ++cur, return true`.
`0x531520(upgrade_id)` finds the slot and calls it. (Slots live in the unit's progress object; offset on the Unit page.)

## 3. Upgrade records (`upgrares.cpp`, UPITEM) — 100 bytes

Loader `upgrares::fn_00531790` (**0x531790**), source `UPITEM` (72 rows, schema `TYPE C16, CODE C32, DESC C32, PARA1 C6, PARA12 C6, PARA3 C6, COST N2, REQUIRE C6`),
accessor `0x50D240(n)` = `base + (n-1)*100`.

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 2 | u16 | `type` | `0x531BA0`: STRENGTH 1, DEXTERITY 2, DEFENSE 3, CHARISMA 4, LEADERSHIP 5, HIT_POINT 6, ENERGY_POINT 7, NEW_COMBO 9, JUMP_ATTACK_B 10, DOWN_ATTACK 11, RUSH_ATTACK 12, ENERGY_ATTACK 13, RANGE_ATTACK 14, HORSE_ATTACK_A 15, HORSE_ATTACK_B 16, other 0 |
| 0x02 | 2 | u16 | `recno` | 1-based row index |
| 0x04 | 0x21 | char[] | `code` | UPITEM CODE (key used by upgrade lists) |
| 0x25 | 0x21 | char[] | `desc` | |
| 0x46 | 3x7 | char[7][3] | `para1`, `para12`, `para3` | numeric strings; `para1` is the increment (`atoi(rec+0x46)`) |
| 0x5B | 2 | i16 | `cost` | points to spend |
| 0x5D | 7 | char[] | `require` | name of a prerequisite upgrade, compared against **other records' `+0x46` string** by the AI allocator |

`0x531ED0(upgrade_index)`: type 1,2,3,5,6,7 -> **attribute** upgrade (pays from `Unit+0x12C`); type 8..16 -> **skill** upgrade (pays from `Unit+0x12A`); others (4 CHARISMA, 0) -> attribute.

## 4. Spending points — `unit_apply_upgrade` 0x4949F0 (verified)

`apply(slot)`: allowed iff the matching pool (`+0x12C` for attribute types, `+0x12A` for skills) `>= cost` **and** `0x5315A0(slot)` (cur < max) succeeds. Then pool `-= cost` and, by type:

| type | effect (n = `atoi(para1)`) |
|---|---|
| 1 STRENGTH | `strength (+0x155) += n` ; **max HP += n * STR_HP** (Gameplay `UNIT/STR_HP`, shipped 0 -> no HP gain) |
| 2 DEXTERITY | `dexterity (+0x157) += n` |
| 3 DEFENSE | `defense (+0x159) += n` ; **max HP += n * DEF_HP** (shipped 0) |
| 5 LEADERSHIP | `leadership (+0x15B) += n` |
| 6 HIT_POINT | `max HP (+0x13D) += n`, then HP set to the new maximum (0x45B720 raises HP by the same delta; here `HP = max`, `hp_mark (+0x139) = max`) |
| 4, 7, 8..16 | no stat change here: skill types are only *unlocked* (the slot `cur_level` is what the combat code tests via 0x50D3D0 / 0x50D320 / 0x50D170) |

`0x45B720(new_max)`: if the new maximum is larger, HP is raised by the difference, otherwise only the maximum is lowered.

## 5. AI auto-spend — `unitlvl::fn_004A95A0` (0x4A95A0, verified)

Triggered by the per-tick timer of non-hero units ([../functions/unit.md](../functions/unit.md) §3, `UPG_TIME` 600000 ms after the last level-up).
1. Walk the unit's upgrade slots from last to first. For a slot with `cur < max`: for each missing level, **attribute** upgrades stop when `+0x12C < cost` else `apply(slot)`;
   **skill** upgrades stop when `+0x12A < cost`, otherwise they are bought only if the prerequisite holds: this upgrade's `require` string is empty, or some slot with `cur != 0`
   has an upgrade whose `+0x46` (PARA1) string equals this `require` (`strcmp`, 0x7C07B0). For a slot with `cur >= max` whose upgrade type is 9 (`NEW_COMBO`) and whose slot counter
   (`slot+6`) is `< 10`, remember its index in a temp array. The walk ends early once both pools are `< 1`.
2. Leftover **skill** points: while `skill_points >= 1` and the temp array is non-empty, choose one entry uniformly with **`rand(count)`** (0x590540), `++slot.counter (slot+6)`,
   `--skill_points`; an entry whose counter exceeds 9 is removed. (A sink that extends combos up to 10 times.)
3. Free the temp array.
RNG sites: step 2 only.

## 6. Constants

| name | value | address | meaning |
|---|---|---|---|
| `UPG_TIME` | 600000 ms | `DAT_00A3CC24` | AI spends points this long after a level-up (Gameplay `UNIT/UPG_TIME`) |
| `LEVELARR` | 5000 ms | `DAT_00A3CC58` | duration of the "level-up arrow" HUD flag (`Unit+0x130`) |
| `LEVSTEP` | 300 ms | `DAT_00A3CC14` | HUD arrow step duration |
| `STR_HP`, `DEF_HP` | 0, 0 | `DAT_00A3CC1C`, `DAT_00A3CCBA` | HP per STR / DEF upgrade point |
| `HP_PER`, `EP_PER`, `STR_PER`, `DEF_PER`, `DEX_PER`, `LDS_PER` | 4, 3, 1, 1, 1, 1 | `DAT_00A3CC7E`, `…CC4C`, `…CD1A`, `…CC1E`, `…CC18`, `…CC10` | displayed "per point" values for the upgrade UI (no reader in the exported unit code) |
