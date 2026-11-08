# Inventory, ItemStack, InventoryGrid and DropItem tables — joan.exe

Unit-side item storage plus the random-drop tables. Template data: [ItemRes](ItemRes.md); world objects: [Item](Item.md); shops/recruiting: [Facility](Facility.md).
Functions: [../functions/items_facilities.md §4-§5](../functions/items_facilities.md).
The inventory code has no source-file assert strings (no `d:\ww\...` literal); the addresses 0x456C70-0x4999xx sit between `unit.cpp` and `itemres.cpp` and are called through the unit's inventory pointer. **Status**: layouts V (≥2 readers), call routing R.

## 1. Fields of the *unit* that belong to this module

| unit offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x1BD | 2 | i16 | `hand_item[0]` | recno of the *world* [Item](Item.md) currently held in the right hand (0 = none). Re-assigning deletes the previous object (0x49F370) or returns it via an out-pointer | 0x50A480 (V) |
| 0x1BF | 2 | i16 | `hand_item[1]` | same for the left hand (`hand` argument `!= 0`) | 0x50A480 (V) |
| 0x1C1 | 4 | Inventory* | `inventory` | NULL for units without an inventory; every entry point (`0x50FD90`, `0x5101C0`, `0x510170`, `0x510120`) returns 0 when it is NULL | 0x50FD90 (V) |
| 0x1C5 | 2 | i16 | `drop_item` | item recno this unit drops when it dies (0 = nothing); `set_unit_drop_item_id` sets it and resets `drop_level` | 0x653210, 0x4F8550 (V) |
| 0x1C7 | 4 | i32 | `drop_level` | item level of that drop (`set_unit_drop_item_level` overrides it) | 0x653210, 0x6532A0 (V) |
| 0x135 / 0x13D | 4 / 4 | i32 | `hp?` / `base_max?` | the "immediate potion" heals `+0x135`; cap = 0x523320 = `+0x13D + 0x497E10(1, recno)` (**I**: current / max hit points; owner = unit family) | 0x50FD90, 0x523320 (I) |

Global player state used by this module: `DAT_00A596C4` = gold (i32; `get_unit_gold` returns it, `add_gold(n)` adds with no clamp; the buy window copies it back when closed), `DAT_00A5B9E8` = player unit recno, GameState `+0x71C/+0x720/+0x724` = **unique-item list** (u16[], count, capacity; capacity grows by 10; add = 0x472620, test = 0x472740) and `+0x728/+0x72C/+0x730` = **armory-seen list** (same shape; add = 0x4729C0, test = 0x472A90).

## 2. ItemStack (0x12 bytes, `new`'d; init 0x456CD0 `(item, level, quantity)`)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 2 | i16 | `item` | ItemRes recno |
| 0x02 | 2 | i16 | `quantity` | units in the stack; **negative = unlimited** (used by shop stock) |
| 0x04 | 2 | i16 | `level` | item level (ItemRes `min_level..max_level`) |
| 0x06 | 4 | i32 | `key1` | instance key (3 dwords; stacks only merge when item, key1, key2 are equal; passed through from shop stock elements `a/b/c`). Meaning of the keys: `TODO(re)` (socketed crosses of `JOAN_SWORD_LV10*`?) |
| 0x0A | 4 | i32 | `key2` | |
| 0x0E | 4 | i32 | `key3` | |

Merge rule everywhere (0x456D60): `take = min(requested, max_quantity - quantity)`, `quantity += take`, `requested -= take`.
The copy used by the buy window (0x497A10) duplicates the 0x12 bytes of every present pointer.

## 3. Inventory (0x1E bytes; ctor 0x495950 = memset 0; free 0x495CE0; allocate grids 0x4959B0 / 0x495B40; deep copy 0x497A10)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 2 | i16 | `weapon_mode` | 0 = nothing wielded, 1 = hand weapons, 2 = ranged weapon; `0x4974F0(hand)` returns the active ItemStack*: mode 2 → `ranged`, mode 1 → `hand[hand]`, else 0 | 0x4974F0 (V) |
| 0x02 | 4 | ItemStack* | `hand[0]` | right/two-handed weapon (weapon types 9, 10) | 0x4970C0, 0x495F40 (V) |
| 0x06 | 4 | ItemStack* | `hand[1]` | left-hand weapon (type 8) | (V) |
| 0x0A | 4 | ItemStack* | `ranged` | bow / crossbow (types 11, 12); created by 0x497270 which refuses any other type; getter 0x497560 | (V) |
| 0x0E | 4 | ItemStack* | `ammo` | quiver stack (types 13-18); created by 0x4973A0; getter 0x497580 | (V) |
| 0x12 | 4 | Grid* | `grid_a` | 4 columns × 1 row (`0x457290(1, 4, 1)`) — armour/accessory strip | 0x4959B0 (V) |
| 0x16 | 4 | Grid* | `grid_b` | **6 columns × 8 rows** — main backpack (the right-hand panel of the buy window) | (V) |
| 0x1A | 4 | Grid* | `grid_c` | 6 columns × 4 rows — second pocket | (V) |

Cell size on screen: **0x28 = 40 px** (all hit tests divide mouse offsets by 0x28).

### InventoryGrid (0x3E bytes; ctor 0x4571E0; dims 0x457290(layers, cols, rows))
A `dynarrb` of 0x18-byte *placed stacks* (stride at `+0x10`, count at `+0x0C`, data at `+0x19`; element getter 0x417200, "empty slot" test 0x457F60 = `element+0x17 == 0`) plus a byte map:

| grid offset | type | name | meaning |
|---|---|---|---|
| 0x2E | u8*** | `cells` | `cells[layer][row][col]` = 0 free or the element index that covers that cell |
| 0x32 | i32 | `layers` | always 1 |
| 0x36 | i32 | `cols` | |
| 0x3A | i32 | `rows` | |

Placed stack element (24 bytes): `+0x00..0x11` ItemStack, `+0x12 u8 w`, `+0x13 u8 h` (from ItemRes `width`/`height`), `+0x14 u8 layer`, `+0x15 u8 col`, `+0x16 u8 row`, `+0x17 u8 slot_marker` (the element index, low byte; 0 = free). Create = 0x4576F0 `(w,h,layer,col,row)`; footprint test = 0x4574A0 (all cells inside the grid and either 0 or equal to the "ignore" id); **first-fit search** = 0x4575A0: rows `0..rows-h`, columns `0..cols-w`, top-left first, returns `(layer=0, col, row)`.

## 4. Adding an item — `unit_give_item` 0x50FD90 → `inventory_add` 0x495F40 (R/I)

`give_item_to_unit(code, level, quantity, unit)` (script id 333, 0x65C5F0): looks up the code (0x4A36C0), calls `0x50FD90(item, quantity, level, 0)` on the unit; on success and when the unit is the player it prints the pickup message (`MessageName` with `%1d` = quantity via 0x4DD6E0 → `i_sms`) and, if `ItemRes.show_armory` and the item is not yet in the armory-seen list, appends it (0x4729C0) and opens the armory hint (0x6BBC90/0x6BA4B0).
`0x50FD90` (this = unit, `inventory != NULL`):
1. **Immediate potions**: `class == 7 && use_immediately != 0` → no slot is used. For `type == 0x15` (Red Potion) `unit+0x135 = min(unit+0x135 + damage*quantity, 0x523320(unit))`. Returns 1. (Other immediate types: consumed with no effect in this function.)
2. **Level gate**: `level < ItemRes.min_level || level > ItemRes.max_level` → return 0.
3. Builds a temporary stack (0x456CD0) and calls `0x495F40(item, level, quantity, unit_recno, key1, key2, key3, force_flag)`:
   * classes **7, 15, 16, 19** (POTION, MISSION, ARMOR, CROSS): *stack into the grids*. Mergeable capacity = Σ over existing identical stacks (`max_quantity - quantity`) (skipped when `force_flag`); if no free footprint exists (0x4575A0 on both pockets) **and** capacity < quantity → return 0. Otherwise merge into existing stacks, then create new stacks of ≤ `max_quantity` while room remains (0x4577A0). Return 1 if everything fitted, `-(remaining)` if partly, 0 if nothing.
   * types **8, 9, 10** (hand weapons; guarded by `usable_unit == 0 || == unit.type(+0x3B)`): if `hand[0]` and `hand[1]` are both empty → equip (0x4970C0: type 8 → `hand[1]`, types 9/10 → `hand[0]`), instantiate the world object for the hand (item factory 0x49EA90 + vtable `+0x18/+0x1C` + 0x50A480), return 1; otherwise a single grid placement (no stacking, 0x4976E0).
   * types **11, 12** (ranged): `ranged` empty and unit type ok and the current `ammo` (if any) matches (bow 11 ↔ ammo types 13,14,15; crossbow 12 ↔ 16,17,18) → 0x497270; otherwise grid.
   * types **13-18** (ammo): if `ammo` is empty or the same item/keys, fill the quiver up to `max_quantity`, remainder to the grid (0x4973A0, 0x4577A0); negative quantity (infinite) goes to the grid unchanged.
   * class **9** (ENHANCER: amulets, gems, rings): single grid placement (no stacking): first fit in one pocket, else in the other (0x4976E0; the pocket choice is hidden in the decompiled `this`).
   * Any item with `unique != 0` is then appended to the unique list (0x472620).
4. If the result is `<= 0` the unplaced amount (`quantity` if 0, `-ret` if negative) is **dropped on the floor** in chunks of `max_quantity` through the floor spawn 0x512830; the call still returns 1.

**Floor spawn** 0x512830 `(item, level, qty, key1, key2, key3)`: creates the world item (0x49EA90), a [FloorItem](Item.md) holder with the stack, then tries up to **10 times**: start vector `(-r, 0, 0)` with `r = unit_extent + item_extent + DAT_009CFC54` (`unit_extent` from the unit definition: collision shape `unitres+0x1C3` = 1/2/3 → value at `+0x1D3` / `+0x1D7` / `+0x1FB`; `item_extent` from the new item's actor collision override `actor+0xB8` = 1/2/3 → `actor+0xC8` / larger of `+0xCC`,`+0xD8` / `+0xF0`); rotate it by `rand(360)°` (`0x590540(0x168)`, `× π/180`) about the unit; add the unit position; probe with `0x6AE8E0` (ground height + walkability). First success: place the item at `(x, y, ground + DAT_009CFBB4)` with the unit's yaw, nation = unit's nation (`unit+0x02` copied to `Item.nation`), return its recno. All 10 failures: delete the item and return 0.

## 5. Removing / counting
* `remove_item_from_unit_inventory(item, unit, count)` (script 371, 0x65CB20 → 0x5101C0 → `0x498970(item, unit_recno, count)`): for every hand slot holding the item → unequip (0x50A480) and free the stack (0x4971E0); `ranged` → 0x497350; `ammo` → 0x497440; grids B and C → 0x458430(item, count); grid A → 0x458430(item, 0). **Equipped slots are removed whole, whatever `count` is.** The 2-argument script form passes no explicit count (default `TODO(re)`).
* `get_item_quantity_in_unit_inventory(item, unit)` (script 341, 0x65CAC0 → 0x510170 → 0x4987D0) and `get_item_class_quantity_in_unit_inventory(class_code, unit)` (script 340, 0x65CA40: class by code 0x49FAB0 → 0x510120 → 0x4985F0): bodies **not exported** (`TODO(re)` 0x4987D0, 0x4985F0); semantics = total over hand/ranged/ammo/grids. S1L1 only uses the item form with `imperial_scroll_id`.

## 6. DropItem {#dropitem}

`ITEM\DROPITEM.RES` (ResLX bank, 37 lists in the Polish install; the older `Resource\dropitem.res` with 9 lists is not read by the loader 0x463AA0). Table object: `+0 i32 count`, `+4 DropList* lists`, `+8 u8 loaded`; free = 0x463980. Lookup by name = 0x463ED0 (linear, case-insensitive on `+2`, returns 1-based id or 0); getter 0x464100 `(lists + id*0x2B - 0x2B)`.

### DropList (0x2B bytes)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 2 | i16 | `recno` | 1-based |
| 0x02 | 32 | char[32] | `code` | list name (`Code`), e.g. `NORMAL1`, `CAPTAIN1`, `POLEMAN`, `UNIQUE_RANDOM_LIST` |
| 0x22 | 1 | — | — | pad |
| 0x23 | 4 | DynArr* | `entries` | 8-byte entries, grow step 0x1E; freed by 0x463F50 / 0x463FC0 |
| 0x27 | 4 | i32 | `total` | sum of all weights |

### DropEntry (8 bytes)
`{ i16 item; i16 level; i32 weight }`.

### Text and parse (0x463CB0, same tokenizer as ItemRes)
Payload = `VERSION 1.0`, `Code : NAME`, then lines `ITEM_CODE : weight [<DAT_009D0728> level]` until the token `END` (every payload except the last is cut so that it ends in `END` and the next begins at `VERSION`; the visible text `ENDVERSION 1.0` in the file is `END`+`VERSION 1.0`). `item` = `item_find_by_code` (0 if the code is unknown — the entry is still stored); the value token is split with `strtok` on the string at 0x9D0728 (not read yet; no shipped file uses a second field): first piece = weight, optional second piece = level, default `ItemRes[item].min_level`.
Examples (item/dropitem.res): `NORMAL1` = chicken 10 (everything else 0), `CAPTAIN1` = bread_s 20, bread_L 15, fish 10, meat 7, apple 5, chicken 3, `ENGLISH_ARROW` 10; `BOW` and `DEFAULT` have total 0.

### Roll — `ditem_roll(list) -> (item, level)` 0x464030
```
if (list.entries == NULL) item = 0
r = rand(total) + 1                    // rand = 0x590540, see below; r in 1..total (total==0 -> r==1)
for i = entries.count down to 1:       // LAST entry first
    r -= entries[i].weight
    if (r < 1) return (entries[i].item, entries[i].level)
return item = 0                        // only reachable when total == 0 / weights are zero
```
So each entry is picked with probability `weight/total` (not a percentage; `NORMAL1` always drops a chicken). **RNG** `0x590540(n)` (`__thiscall`, `this` = an RNG object whose state is the u32 at `this+0x7A`): `state = state * 0x015A4E35 + 1` (mod 2^32); `result = (n * ((state >> 16) & 0x7FFF)) >> 15`, i.e. uniform in `[0, n)`. Seeded by `0x5904E0` from `time(NULL)` (two RNG objects exist, see [gameloop.md](../functions/gameloop.md)).

Call sites of the roll: (a) unit creation 0x4F8550 (drop list id = `unitres+0x179`; see [items_facilities.md](../functions/items_facilities.md) §5 for the pre-roll `rand(100)` gate and the Gameplay percentages), (b) `add_item_box` 0x651C90, (c) `.uni` box objects 0x4F3DF0.
