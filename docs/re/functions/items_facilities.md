# Items, drops, shops, recruiting, upgrades, gates, walls, facilities — joan.exe (reference build)

Pseudocode: `$JOAN_WORK_DIR/decomp/game/items_facilities/` (174 functions, depth 0-2). Several callees were read from the sibling exports (`decomp/script/script_vm/`, `decomp/game/unit/`, `decomp/game/hud/`, `decomp/engine/*`); those are cited by address only.
 Structs: [ItemRes](../structs/ItemRes.md), [Item](../structs/Item.md), [Gate](../structs/Gate.md), [Wall](../structs/Wall.md), [Inventory](../structs/Inventory.md), [Facility](../structs/Facility.md).
Source-file attribution from assert strings (`d:\ww\<file>.cpp` passed to the tracked allocator 0x58E7B0 / 0x58EB30 / 0x58ED70): `itemres.cpp`, `ditemres.cpp`, `sitemres.cpp`, `recrures.cpp`, `upgrares.cpp`, `unitures.cpp`, `raceres.cpp`, `levelres.cpp`, `f_shop.cpp`, `i_gate.cpp`, `i_wall.cpp`, `world_s.cpp`. The inventory, item factory and facility-class code carry **no** file string.
Confidence: **V** verified (≥ 2 consistent readings / data sample), **R** read in one function, **I** inferred. `this` is ECX unless stated. All lookups by name are case-insensitive (`0x7C07B0` = `_stricmp`).

## 0. Shared helpers seen in this family (owned elsewhere)

| address | role | note |
|---|---|---|
| 0x58E7B0 / 0x58EB30 / 0x58ED70 | tracked `malloc` / `realloc` / `free` | |
| 0x589550 / 0x589780 / 0x589890 | FileTxt open file / open memory block / close | the text parser input |
| 0x589C70 | `tok()` — next token (separators ` , : = TAB`, crosses lines, ≤ 0x80 chars, 0 at EOF 0x1A) | [ItemRes §2.2](../structs/ItemRes.md) |
| 0x589F10 | `rest_of_line(buf, max)` — copy to end of line, then advance a line | |
| 0x5A18D0 | `strdup_n(s, -1)` heap copy | |
| 0x59B920 / 0x59BDD0 / 0x59BF70 | ResLX open / payload size / payload pointer ([res.md](../../formats/res.md)) | |
| 0x58B180 / 0x580C70 / 0x415280 | gameset: select table by name (fatal `Error Loading %s` unless `RELEASE`) / record pointer `(i)` / record count | [gameset.md](../../formats/gameset.md) |
| 0x590170 / 0x590210 | field copy: right-trimmed (≤ 0x78) / `copy-trim` with terminating NUL | |
| 0x5843D0 / 0x584620 / 0x584510 | `dynarr` ctor (elem size, grow) / push (copy elem) / grow | |
| 0x417200 / 0x417AD0 | dynarr element ptr `(i)` (1-based) / count | |
| 0x41B3F0 (0x425D20, 0x43C450, 0x41B3C0 …) | `dynarrb` slot getter (object pointer, 0 if out of range) used by every "recno → object" accessor | |
| 0x590540 | `rand(n)` — LCG, see [Inventory §6](../structs/Inventory.md) | |
| 0x590A20 | `round_to_int(double)` = `±ftol(|x| + [0x9D3E18])` (`[0x9D3E18] = 0.0` per ui2d.md ⇒ truncation; verify) | |

## 1. Resource tables

### 1.1 ItemRes (`itemres.cpp`)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x49F830 | `itemres_load_classes` | `ITEMCLAS` gameset table → `class_count`, `classes[]` (0x2C each: `index`, `code[9]` 8 chars, `name[33]` 32 chars) | strings `ITEMCLAS`, sizes | V |
| 0x49F970 | `itemres_load_types` | `ITEMTYPE` table → `types[]` (0x25 each: `class` via 0x49FAB0, `index`, `name[33]`) | string `ITEMTYPE` | V |
| 0x49FBB0 | `itemres_load_all` | builds the path `ITEM\` + `ITEM.RES`, opens the ResLX bank, allocates `N*0x2BF`, zeroes, for payload i=1..N: `recno=i`, open payload as text, `itemres_parse_record`, close; then `itemres_scan_pro("ITEM\")` and `itemres_build_sorted_index` | strings | V |
| 0x49FDF0 | `itemres_scan_pro(dir)` | pass 1: Win32 `FindFirst("dir\*.pro")` → for every file `item_count++`, `realloc(items, count*0x2BF)`, zero the new record, `recno=count`, open `dir+file`, parse. Pass 2: every sub-directory (not `.`/`..`) → recurse with `dir+name+"\"`. **Order = filesystem enumeration order** | strings `*.pro`, `..` | V |
| 0x4A0160 | `itemres_parse_record(file, rec)` | the 10.7 KB text parser (see [ItemRes §2.2](../structs/ItemRes.md)); presets `can_drop=1`, `can_sell=1`; resolves class, type, models (modelres 0x4B18D0), bone (0x423220), enums | ~50 key strings | V |
| 0x4A3590 | `itemres_build_sorted_index` | `sorted = [1..N]`, `DAT_00A39E18 = this`, `qsort(sorted, N, 4, 0x4A3650)` (compare = case-insensitive `code`) | body | V |
| 0x4A36C0 | `itemres_find_by_code(code)` → recno | `sorted == NULL` → linear scan of `code` (`+0x10`) from record 1; else `bsearch` with comparator 0x4A3790; returns 0 when not found. Backing for the script id `item_code_to_id` | body | V |
| 0x49FAB0 | `itemres_class_by_name` | linear, 1-based; 0 if unknown | body | V |
| 0x49FB30 | `itemres_type_by_name` | same on the `ITEMTYPE` rows | body | V |
| 0x425CE0 | `itemres_get(recno)` | `items + recno*0x2BF - 0x2BF`, **no range check** (recno 0 reads before the array) | body | V |
| 0x49F620 | `itemres_free` | frees `sorted`, runs 0x49F7D0, frees `items`, `types`, `classes` | body | V |
| 0x49F7D0 | `itemres_free_records` | for i=1..count: 0x4A37E0 | body | V |
| 0x4A37E0 | `itemres_free_record_ptrs` | zeroes `level_model[]`, `level_anim[]`, `description`, 4 emitter name ptrs, frees `req` (+0x249) and `fx` (+0x251) | body | V |
| 0x4A45F0 | `item_effect_by_name` | 24-name enum ([ItemRes §2.1](../structs/ItemRes.md)) | strings | V |
| 0x4A4540 | `item_collision_by_name` | `Box3d`=2, `Cylinder`=3, `Sphere`=1, else 0 | strings | V |
| 0x4B03E0 | `item_light_mod_by_name` | `RANDOM_ATTENUATION` → 1, else 0 | string | V |
| 0x423220 | `bone_id_by_name` | 10-entry static table (stride 0x38; ids at 0x9F7488, names from 0x9F748C, first `bip01_head`), 0 if not found | body | R |
| 0x4D12F0 | `sound_id_by_name` | wraps 0x4D1220 (sound table search); ≤ 0 ⇒ 0 | body | R |
| 0x5270B0 | `unitres_find_by_code` | linear on name at `+4` of the 0x203-byte unit records (`short count` at `this+4`) | body | V |
| 0x4A41C0 | `item_effect_at_level(res, level)` | `damage + (level - min_level) * inc_damage` | body | V |
| 0x4A4050 | `item_class_registers_modify` | true for classes 1,2,3,4,6,7,8,9,10,11,13,14,15,17,18,19,20 (those get a modify-list handle at creation) | body | R |
| 0x4A3E30 | `itemres_release_level(res, level)` | `--shadow_refcnt` (frees shadow model at 0), `--level_refcnt[level]` (frees the level model via 0x4B16A0 and aux handle via 0x417E00 at 0) | body | R |
| 0x4A39F0 | `itemres_acquire_level(res, level)` | `++level_refcnt[level]`; on the first use loads the level's model (modelres 0x4B0EC0/0x4B0F40) and, for `.AM1` ('A') models, its animation (`<model dir>ANIM\` + name via 0x4184B0, shared between levels that use the same model file); on the first use of the item at all (`shadow_refcnt` 0→1) also loads the shadow model and one particle resource per non-empty emitter name | body | R |
| 0x4A3F20 | `itemres_level_model(res, level)` | level 0 / empty → `res.model`; else modelres id of `"ITEM\"+dirname(code)+"\"` + `level_model[level-1]` | body | R |

### 1.2 DropItem (`ditemres.cpp`)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x463AA0 | `ditemres_load` | opens `ITEM\DROPITEM.RES`, `count = N`, `lists = N*0x2B`, for i=1..N: `recno=i`, text parse via 0x463CB0 | strings | V |
| 0x463CB0 | `ditemres_parse_list` | `VERSION n`, `Code NAME` (≤ 0x20 chars at `+2`), creates the entry array (elem 8, grow 0x1E), then until token `END`: `item = itemres_find_by_code(tok)`, value token split by `strtok` (delimiter string 0x9D0728) → `weight`, optional `level` (default `min_level`), push, `total += weight` | body | V |
| 0x464030 | `ditem_roll(list, &item, &level)` | weighted pick, last entry first (see [Inventory §6](../structs/Inventory.md)) | body | V |
| 0x463ED0 | `ditemres_find_by_name` | linear, 1-based (stride 0x2B, name at `+2`) | body | V |
| 0x464100 | `ditemres_get(id)` | `lists + id*0x2B - 0x2B` | body | V |
| 0x463980 / 0x463A40 / 0x463F50 / 0x463FC0 | `ditemres_free` / `_free_lists` / `_free_entries` / dynarr deleting dtor | | bodies | V |

### 1.3 SellItem (`sitemres.cpp`)

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x4CE420 | `sitemres_load` | `ITEM\SELLITEM.RES`; `lists = N*0x1C`; parse each via 0x4CE630 | V |
| 0x4CE630 | `sitemres_parse_list` | `VERSION`, `Code` (≤ 0x10 chars), optional `Type POTION|WEAPON` (→ `kind` 0/1), `SellingRatio r`, entries `item : a(float) : count : level` until `END` (level `-1` ⇒ `ItemRes.min_level`; unknown item ⇒ recno 0 + `min_level` of record 0, i.e. garbage — ignore such rows) | V |
| 0x469E60 | `sitemres_get(id)` | `lists + id*0x1C - 0x1C` | V |
| 0x4CE900 | `sitemres_find_by_name` | stride 0x1C, name at `+2` (script `search_item_sell_list_by_code`) | V |
| 0x4CE300 / 0x4CE3C0 / 0x4CE980 | free / free each / free entries | | V |

### 1.4 Recruit (`recrures.cpp`)

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x4CA760 | `recrures_load` | `UNIT\RECRUIT.RES`; `lists = N*0xB4`; parse via 0x4CA980 | V |
| 0x4CA980 | `recrures_parse_list` | `Code` (≤ 0x20), optional `Location` (rest of line, default = code), `Level`, `Population`, `SellingRatio`, then `UNIT : x(float) : y(int)` rows (unit by 0x5270B0) until `END` | V |
| 0x469520 | `recrures_get(id)` | `lists + id*0xB4 - 0xB4` | V |
| 0x4CAD00 | `recrures_find_by_name` | stride 0xB4 (script `search_recruit_list_by_code`) | V |
| 0x4CA640 / 0x4CA700 / 0x4CAD80 | free / free each / free entries | | V |

### 1.5 Upgrades, race, level

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x531790 | `upgrares_load_upitem` | `UPITEM` table → 100-byte rows ([ItemRes §3.3](../structs/ItemRes.md)) | V |
| 0x5319A0 | `upgrares_load_upgrade` | `UPGRADE` table → 0x35-byte rows, then 0x530CC0 for each | V |
| 0x530CC0 | `unitures_load_list` | frees old list, 0x530D70, sets `loaded=1` | V |
| 0x530D70 | `unitures_read_list` | selects the section named `rec.section`, builds `{u16 level, u16 upitem}` rows via 0x531B20 (UPITEM by code; not exported) | V |
| 0x530D00 | `unitures_free_list` | | V |
| 0x531BA0 | `upitem_type_by_name` | 15-name enum | V |
| 0x531E70 | `upgrares_get(id)` | `upgrade + id*0x35 - 0x35`; out-of-range ⇒ logs `UpgradeRes::get_unit_upgrade_list_res` | V |
| 0x531690 | `upgrares_free` | | V |
| 0x4CA480 / 0x4CA350 | `raceres_load` / `raceres_free` | `RACE` table → 0x34-byte rows | V |
| 0x4A4CA0 / 0x4A4C30 | `levelres_load` / `levelres_free` | `LEVEL` table → 7-byte rows | V |
| 0x4A4E60 | `levelres_get(level)` | linear on `+0` of the 7-byte rows; NULL if missing (used by 0x4F8550) | V |

## 2. Item instances

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x49EA90 | `item_create(res_id)` → recno | class switch picks subclass size/ctor ([Item §1](../structs/Item.md)), `vtbl slot +8 (res)`, append to the world item array (0x5857A0), `recno = 0x417520()`, store at `+4`; LADDER ⇒ `+0x7E = 0x200`; MISSION/CROSS ⇒ copy `ItemRes.name` to `+0x0C`; register in the modify list when 0x4A4050 | script_vm export | V |
| 0x49F370 | `item_delete(recno)` | unregister modify id (`+0x06`), deleting dtor, remove array slot | body | V |
| 0x49F420 | `item_find_by_name(name)` | scans `count..1`, valid slots only, `_stricmp(item+0x0C, name)`; returns recno or 0 (script 18) | body | V |
| 0x49F4B0 | `item_find_by_code(code)` | scans `count..1`, compares `ItemRes[item.res].code` (script 19) | body | V |
| 0x4251B0 | `item_base_dtor` | see [Item §4](../structs/Item.md) | body | V |
| 0x4994B0 | `item_set_scene(scene)` | if changed: unregister collision (0x54F110), unlink the actor from the scene (0x5EFDB0), store `+0xA6` | body | R |
| 0x651AD0 | `script_add_item(code, nation)` | `item_create`, `+8 = nation(name)`, and for `pickable` items allocates the 0x52-byte FloorItem holder (stack = `(res, min_level, quantity)`, `+0x34 = recno`, `+0x32 = nation`, scene 0x457120) stored at `+0xA2` | body | V |
| 0x651C90 | `script_add_item_box(code, name, x, y, z, rot, contents)` | `item_create`, scene, position, yaw (`rot*π/180` via 0x499FD0), name (`strncpy 0x40` at `+0x0C`); for class 10 resolves `contents` as a DropItem list first, else an item code, writes `content_item/content_level` | body | V |
| 0x512830 | `item_spawn_on_floor(item, level, qty, k1, k2, k3)` | floor placement around a unit, 10 random-angle attempts ([Inventory §4](../structs/Inventory.md)) | body | R |
| 0x65BCE0 | `script_drop_item_on_floor(unit, held_item, hand)` | **I**: creates a new item of the same res (0x49EA90), copies the held item's nation/actor link, places it at the held item's attach transform (0x60C430), then unequips the hand (`0x50A480(0,0,hand,0)`). The name is misleading: it converts a *held* item object into a free world object | body | I |

## 3. Gates and walls

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x48C250 / 0x432210 | `gate_ctor` / `wall_ctor` | base ctor + zero the tail (`+0xC2`, 0x19 / 0x2D bytes), set vtable | bodies | V |
| 0x48C410 | `gate_dtor` | if `closed==0` apply(1) else if `closed==1` apply(0) (collision cleanup), free `block_ids`, base dtor | `i_gate.cpp` | V |
| 0x48D4B0 | `gate_apply_collision(close)` | open/close = register/disable the gate collision and block records; refuses to close when a unit stands in the doorway ([Gate](../structs/Gate.md)) | `i_gate.cpp` | V |
| 0x487460 | `wall_dtor` | base dtor, release two extra actors + levels, free `block_ids` | `i_wall.cpp` | V |
| 0x560C70 | `world_block_alloc` | grow-by-8 array of 0x74-byte block records (`world_s.cpp`), returns index of a zeroed record | `world_s.cpp` | V |
| 0x560D30 | `world_block_get(i)` | `base + i*0x74` | body | V |
| 0x530EB0 / 0x530EE0 | `block_enable` / `block_set_vec3(i, v)` | `+0x70 = 1` / copy 3 floats at `+i*12` | bodies | R |
| 0x6AF710 / 0x6AFB10 | `collision_add_*` | register the gate collision (no-box / box variants) | callees | R |

## 4. Inventory (see [Inventory](../structs/Inventory.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x50FD90 | `unit_give_item(unit, item, qty, level, flag)` | immediate potion / level gate / `inventory_add` / floor overflow | R |
| 0x495F40 | `inventory_add(item, level, qty, unit, k1, k2, k3, force)` | routing by class/type; returns 1 / 0 / `-(left)` | R |
| 0x4575A0 / 0x4574A0 / 0x4576F0 / 0x4577A0 / 0x456D60 | `grid_find_free` / `grid_footprint_free` / `grid_place` / `grid_add_stack` / `stack_merge` | grid logic | V |
| 0x4959B0 / 0x495B40 / 0x495CE0 / 0x497A10 / 0x495950 | `inventory_alloc_grids` / `inventory_ensure_grids` / `inventory_free` / `inventory_copy` / `inventory_ctor` | | V |
| 0x4970C0 / 0x497270 / 0x4973A0 / 0x4974F0 | `inventory_set_hand` / `_set_ranged` / `_set_ammo` / `_active_weapon` | | V |
| 0x5101C0 → 0x498970 | `unit_remove_item(item, count)` | | R |
| 0x510170 → 0x4987D0 / 0x510120 → 0x4985F0 | `unit_count_item` / `unit_count_item_class` | bodies of 0x4987D0/0x4985F0 not exported | I |
| 0x50A480 | `unit_set_hand_item(item, bone, hand, out)` | replaces `unit+0x1BD/0x1BF`; destroys or returns the previous object; attaches the new one to the bone | R |
| 0x472620 / 0x472740 | `game_unique_add` / `game_unique_has` | GameState+0x71C list | V |
| 0x4729C0 / 0x472A90 | `game_armory_seen_add` / `_has` | GameState+0x728 list | V |

## 5. Shops, barracks, workshops, spawn points

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x46D4A0 | `facilities_update` | `for i in 1..count: if (!empty(i)) facility(i)->vtbl[+0x10]()`; called after places, before territories each world tick | body | V |
| 0x46D200 | `facility_find_by_name` | last→first, `_stricmp(obj+8, name)` | body | V |
| 0x4697C0 | `shop_set_list(list)` | rebuild stock from the template (full counts, owned uniques skipped) | body | V |
| 0x4699F0 | `shop_restock(list)` | add half of the template stock, capped (`f_shop.cpp`) | `f_shop.cpp` | V |
| 0x468790 | `barracks_set_lists(a, b)` | | body | V |
| 0x46B730 | `workshop_set_list(list)` | | body | R |
| 0x467DD0 / 0x467EA0 / 0x467F90 | `spawn_add_leader` / `spawn_add_member` / `spawn_reset` | | bodies | V |
| 0x469330 / 0x469380 / 0x4693B0 | `barracks_set_pool_cur` / `_set_pool_max` / `_get_pool_max` | | bodies | I |
| 0x468C10 | `barracks_swap_flags` | English ⇄ French flag items | body | V |
| 0x6C6550 / 0x6C4070 / 0x6C5A00 / 0x6C5F10 / 0x6C3540 / 0x6C3230 | buy-window input / draw / buy-by-button / buy-by-click / close+commit / open (HUD family, `i_buy.cpp`) | their transaction rules are summarised in ../../game/items_facilities.md §6 | R |

## 6. Builtin (FUNC) handlers of this family

Ids and addresses are those of [BUILTINS.md](../../scripting/BUILTINS.md). Semantics read from the pseudocode (R = body read here; N = name only). "valid" below = `0x425D50(recno) == 0` for items, `0x428B70(recno) == 0` for units, `0x44CF80(recno) == 0` for facilities. Invalid recnos make the handler return 0 (or the stated value) without side effects.

| id | name | handler | semantics |
|---|---|---|---|
| 5 | search_for_item | 0x6519D0 → 0x503680 | unit valid ⇒ nearest-item search around `(x,y,z)` (R) |
| 6 | get_item_rotation | 0x653000 | unit-relative angle via 0x4FF850/0x5059B0 + `ftol` (N/R) |
| 7 | get_item_dist | 0x653060 | `ftol(length(item.pos - unit.pos))`, 0 if either invalid (R) |
| 9 | item_code_to_id | 0x651A30 → 0x4A36C0 | code → ItemRes recno, 0 unknown (R) |
| 10 | add_item_ref | 0x651A60 | resolves the code and, if known, calls `itemres_acquire_level(res, 0)` (0x4A39F0: asset preload, refcount +1) and `scenario::fn_004CD960(item)` (register in the scenario's used-item list); always returns 1 (R) |
| 11 | add_item | 0x651AD0 | `script_add_item` — returns the new item recno (R) |
| 12 | del_item | 0x651E70 → 0x49F370 | returns 1 (R) |
| 13 | del_item_by_name | 0x651EA0 | repeated name search + delete (R, BUILTINS) |
| 14 | set_item_pos | 0x651F00 | item valid ⇒ `0x4994B0(scene DAT_00A45F3E)`, `0x41FF50(x,y,z)`; returns the recno (R) |
| 15 | set_item_dir | 0x651FA0 | `deg × π/180` (`[0x9CF884]`) → 0x499FD0 (R) |
| 16 | set_item_nation | 0x652010 | `item.+8 = nation(name)`, 0 when the name does not resolve (R) |
| 17 | add_item_box | 0x651C90 | `script_add_item_box` (R) |
| 18 | search_item_by_name | 0x65B570 → 0x49F420 | name 0 ⇒ 0 (R) |
| 19 | search_item_by_code | 0x65B5B0 → 0x49F4B0 | (R) |
| 20 | search_item_sell_list_by_code | 0x65B5F0 → 0x4CE900 | (R) |
| 21 | is_item_deleted_by_name | 0x65B630 | **true if the slot is empty or the item at that recno has a different name** (guards against recno reuse) (R) |
| 22 / 23 / 24 | is_item_box / gate / wall | 0x65B6A0 / 0x65B700 / 0x65B760 | `Item.class == 10 / 1 / 4` (R) |
| 25 | search_recruit_list_by_code | 0x65B7C0 → 0x4CAD00 | (R) |
| 26-29 | get/set_gate_hp, get/set_gate_max_hp | 0x650B90 / 0x650C90 / 0x650C10 / 0x650CF0 | field `+0xC2` / `+0xC6` of the gate view; `get_gate_hp` guarded by class 1 (R) |
| 30 | gate_open | 0x652440 | **decompiled body takes no argument and reads `+0xD1` of the object at slot 1 of container 0x485410/0x4853E0 (not the item array)** — looks like a stale/test handler; no shipped script calls it (`TODO(re)`) |
| 31 | get_gate_state | 0x6524A0 | `+0xD1` (class 1 only, else -1) (R) |
| 32 / 33 | set / get_gate_need_key | 0x652520 / 0x652590 | `+0xCE` byte, -1 for invalid (R) |
| 34 | set_gate_invincible | 0x6525F0 | `+0xCF` (R) |
| 35 | get_gate_attacked | 0x652660 | `+0xD0` (R) |
| 36-38 | get/set_wall_hp, get_wall_max_hp | 0x650D50 / 0x650DB0 / 0x650E10 | wall view `+0xC6` / `+0xCA` (R) |
| 39 | break_wall | 0x650E70 | posts event `0x40` ([Wall](../structs/Wall.md)) (R) |
| 40 | set_wall_invincible | 0x650F40 | `+0xE6` (R) |
| 65 / 66 | is_inside_facility / is_unit_inside_facility | 0x653CA0 / 0x653D60 | [Facility §1](../structs/Facility.md) (R / N) |
| 68 | set_unit_drop_item_id | 0x653210 | unit valid ⇒ `unit+0x1C5 = item`; if non-zero `unit+0x1C7 = ItemRes[item].min_level`; returns 1 (R) |
| 69 | set_unit_drop_item_level | 0x6532A0 | `unit+0x1C7 = level` (R) |
| 81 / 82 | get_unit_gold / add_gold | 0x65C200 / 0x65C220 | global `DAT_00A596C4` read / `+= n` (no clamp) (R) |
| 104 / 105 | get / set_interact_item_recno | 0x655C80 / 0x655CD0 | `unit+0x2E5` (i16) (R) |
| 117 / 118 | set / get_unit_armor_id | 0x6562B0 / 0x656300 | `unit+0x15F` (i16) (R/N) |
| 152 | disable_team_drop_item | 0x658D90 | every unit with `unit+0x31 == team` gets `+0x1C5 = 0` (R) |
| 156 | set_unit_curr_item | 0x658EA0 | item 0 ⇒ unequip hand; else bone id from the name (0x423220, 0 ⇒ fail) and `0x50A480(item, bone, hand, 0)` (R) |
| 178 | get_spawn_unit_count_in_place | 0x6541A0 | `place.+0x1CA` (R) |
| 186 | search_facility_by_name | 0x6548B0 → 0x46D200 | (R) |
| 187 / 188 / 196 | facility_barrack_add_leader / add_member / add_leader_order | 0x6548F0 / 0x654960 / 0x654D70 | [Facility §2](../structs/Facility.md) (R) |
| 189 | get_unit_facility_distance | 0x6549C0 | `ftol(length(unit.pos - (facility+0x29)))`, 0xFFFF for invalid args (R; note the facility position is read at `+0x29`) |
| 190 | set_facility_follow_unit_recno | 0x654AE0 | spawn view `+0x13C` (R) |
| 191 | get_spawning_count | 0x654B30 | Σ `member_count` (R) |
| 192 | reset_spawn_count_in_facility | 0x6541D0 | `place.+0x1CA = 0` + 0x467F90 (R) |
| 193 / 194 | set_can_recruit_flag / set_recruit_level | 0x654BC0 / 0x654C20 | barracks `+0x97` / `+0x98` (R) |
| 195 | set_barrack_captured | 0x654C80 | [Facility §3](../structs/Facility.md) (R) |
| 197 | shop_set_sell_item_list | 0x654E30 | `0x4697C0(list)` (R) |
| 198 | set_can_regen_sell_item | 0x654EA0 | shop `+0x81` (R) |
| 199 | barrack_set_recruit_list | 0x654F10 | `(f, spawn_facility, list_a, list_b)` (R) |
| 200 | workshop_set_recruit_list | 0x654FA0 | (R) |
| 304 | start_buy_interface | 0x653890 | facility and unit valid ⇒ opens the buy window (`i_buy::fn_006C8D00(1)`), stores the facility (0x655B70) and unit (0x655BA0), refresh 0x592C30; the script waits until `0x6C9040()` reports it closed (R) |
| 314 | drop_item_on_floor | 0x65BCE0 | see §2 (I) |
| 332 | add_gate_trigger | 0x65C530 | event `0x20` ([Gate](../structs/Gate.md)) (R) |
| 333 / 334 | give_item_to_unit / _troop | 0x65C5F0 / 0x65C770 | `(code, level, qty, unit)`: unit valid and code known ⇒ `0x50FD90(item, qty, level, 0)` (333) or `…, 1)` (334, `force` flag = bypass merge capacity); player-only side effects (message, armory hint) only in 333 (R) |
| 340 / 341 | get_item_class_quantity / get_item_quantity_in_unit_inventory | 0x65CA40 / 0x65CAC0 | unit valid ⇒ `0x510120(class)` / `0x510170(item)` (R) |
| 351 | mount_siege_tower_to_wall | 0x65D450 | drives `unit+0x3B4` through states 0..3 (BUILTINS R) |
| 362 | set_item_path_item_owner | 0x65DFF0 | virtual `vtbl[+0x64](2, arg)` then 0x491C40 (BUILTINS R) |
| 371 | remove_item_from_unit_inventory | 0x65CB20 | unit valid ⇒ `0x5101C0(item, count)` → [Inventory §5](../structs/Inventory.md) (R) |
| 372 | set_item_can_interact_flag | 0x65E930 | `+0x7E = on ? 0x200 : 0`, returns 1 (R) |
| 374-377 | is_item_on_floor, get_item_pos_x/y/z | 0x65EA00 / 0x65EA50 / 0x65EAD0 / 0x65EB50 | `+0xA2` pointer; position from `floor+0x12/0x16/0x1A` else `+0x56/0x5A/0x5E` (BUILTINS R) |

## 7. Constants

| value | where | meaning |
|---|---|---|
| 0x2BF (703) | 0x425CE0, 0x49FBB0 | `sizeof(ItemRes)` |
| 0x2C / 0x25 / 0x2B / 0x1C / 0xB4 / 0x35 / 100 / 0x34 / 7 | loaders | sizeof ItemClass / ItemType / DropList / SellList / RecruitList / UpgradeRow / UpItemRow / RaceRow / LevelRow |
| 0xC2 / 0xDB / 0xEF / 0xCD / 0xCE / 0x107 | 0x49EA90 | item instance sizes (base / gate / wall / box / obstacle,house,path / switch) |
| 0x52 | 0x651AD0 | FloorItem holder |
| 0x12 / 0x18 / 0x3E / 0x1E | 0x4973A0, 0x4571E0, 0x495950 | ItemStack / placed stack / grid / inventory sizes |
| 6×8, 6×4, 4×1 cells, 40 px | 0x4959B0, 0x6C6550 | inventory grids and cell size |
| 0xE (14) / 0xA (10) / 8 | shop / recruit / drop entry size | |
| 2.0 `[0x9CF794]` | 0x4699F0 | restock divisor |
| 4 | 0x467EA0 | max member groups per spawn facility |
| 10 | 0x512830 | floor-spawn attempts; 0x168 (360) angle range |
| 0x15 / 0x17 / 0x19 | 0x6C6550, 0x6C4070 | potion item types sold to potion shops |
| 1000 / 2000 / 4000 / 6000 | 0x6C4070 | gold HUD sprites `coins1..4` thresholds (< 2000 coins1, < 4000 coins2, < 6000 coins3, else coins4) |
| unread globals | | `[0x9CFCA0]`, `[0x9CF9C4]`, `[0x9D3968]` (gate doorway test), `[0x9CFC54]`, `[0x9CFBB4]` (floor spawn), `[0x9D3E18]` (rounding), `[0x9D0728]` (drop value delimiter) — `TODO(re)`: read `.rdata` |

## 8. RNG sites (determinism audit)

All through `0x590540(n)` ([Inventory §6](../structs/Inventory.md)): (1) `ditem_roll` `rand(total)` — callers 0x4F8550 (unit init), 0x651C90 (`add_item_box`), 0x4F3DF0 (`.uni` boxes); (2) `0x4F8550` `rand(100) <= chance` gate before the unit's drop roll; (3) `0x512830` `rand(360)` ×≤10. No other rand call in this family. Shop restock, buy/sell, recruit and gate logic are deterministic.

## 9. Cross-module requests

* **Ghidra names/types to apply**: structs `ItemRes`, `ItemClass`, `ItemType`, `Item`, `Gate`, `Wall`, `FloorItem`, `ItemStack`, `Inventory`, `InventoryGrid`, `DropList`, `DropEntry`, `SellList`, `StockEntry`, `RecruitList`, `RecruitEntry`, `UpItem`, `UpgradeRow`, `LevelRow`, `RaceRow`; function names from the tables above; globals `DAT_00A596C4 = g_gold`, `DAT_00A5B9E8 = g_player_unit`, `DAT_00A39E18 = g_itemres_sort_ctx`.
* **unit family**: unit fields owned by this module are listed in [Inventory §1](../structs/Inventory.md) (`+0x1BD/+0x1BF/+0x1C1/+0x1C5/+0x1C7`, `+0x2E5`, `+0x15F`, `+0x31` team, `+0x3B` type, `+0x335` scene, `+0x135/+0x13D`). The unit family should document who decrements `drop_item` pick-up, death drops, and gold pick-ups (class 8 is a no-op in `inventory_add`).
* **hud family**: buy/barracks/workshop window rules (prices, recruit costs) — see the open questions below for what this page could not read.
* **world_nation family**: `Place` fields `+0x26`, `+0x1CA`, `+0x1CE`, `+0x1DE`; `.uni` item/box/facility placement (0x4F3DF0).
* **scripting**: BUILTINS rows 30 (`gate_open`), 6 (`get_item_rotation`), 314 (`drop_item_on_floor`) have misleading names/arity (§6).

## 10. Open questions (TODO(re))
1. 0x6C5A00 / 0x6C5F10 / 0x6C6550 / 0x6C4070 — the x87 operands of the `__ftol()` cost and sell-price computations are invisible in the decompile. Expected `price = ftol(ItemRes.price * StockEntry.a * …)` for buying and `ftol(price * selling_ratio * …)` for selling; needs a disassembly look or a Wine trace (buy 10 arrows: charged amount).
2. Callers of `0x4699F0` (restock) and bodies of the facility `vtbl[+0x10]` update methods (barracks spawn timer, shop regen, workshop build progress).
3. Gate/wall state machine: writers of `Gate.state`, `Wall` break transition, event `0x20`/`0x40` handlers in the trigger system 0x4EF090, damage application.
4. `0x4987D0`, `0x4985F0` (inventory counts), `0x458430` (grid remove), `vtbl[+8]` item init (does it call 0x4A39F0?), `0x531B20` (UPITEM by code), `0x46A6F0` (workshop slot).
5. Meaning of `StockEntry.a`, `RecruitEntry.x/y`, `ItemStack.key1..3`, gate `+0xCA`.
6. Value of the constants in §7 "unread globals" and the string at 0x9D0728.
7. Class 20 name (`PTERRAIN`?) — confirm from the 20th ITEMCLAS row.
8. Gold pick-up path (class 8) and `UseImmediately` potions other than type 0x15.
