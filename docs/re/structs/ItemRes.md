# ItemRes (`itemres.cpp`) — item/gate/wall/box *template* record and its class/type tables — joan.exe

One `ItemRes` record is the parsed form of one item definition: either one entry of `ITEM\ITEM.RES` (252 text records, e.g. `ALENCON_ARMOR_LV1`, `ARROW`,
`GOLDCOIN`) or one `*.pro` file found recursively under `ITEM\` (819 files in the Polish install: gates, walls, boxes, houses, terrain objects, ladders, torches ...).
It is the *static* data every world item instance ([Item](Item.md)), inventory stack ([Inventory](Inventory.md)), shop/drop/sell list and script call
(`item_code_to_id`, `give_item_to_unit`...) refers to by 1-based `recno` (`i16`).
Functions: [../functions/items_facilities.md §1](../functions/items_facilities.md).
Text syntax: [../../formats/pro.md §B](../../formats/pro.md) (the item `.pro` files) — **this page is authoritative for what the engine does with each key** (positional vs keyed reads).
`ItemClass` census of the 819 `.pro` files (pro.md): TERRAIN 730, WALL 23, GATE 22, BOX 11, MISC 11, LADDER 5, PTERRAIN 5, HOUSE 5, 1-offs KEG/OBSTACLE/SWITCH/WAVE/WEAPON.

Status legend: **V** = read in the parser and consistent with data samples, **R** = read in one place, **I** = inferred.

## 1. Manager object `ItemResTable` (global; `this` of 0x49F620/0x49F830/0x49F970/0x49FBB0)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `item_count` | number of `ItemRes` records (ITEM.RES entries + `.pro` files), 1-based ids `1..item_count` | 0x49FBB0, 0x49FDF0 (V) |
| 0x04 | 4 | i32 | `type_count` | rows of ITEMTYPE (39 in Std.set) | 0x49F970 (V) |
| 0x08 | 4 | i32 | `class_count` | rows of ITEMCLAS (20 in Std.set) | 0x49F830 (V) |
| 0x0C | 4 | ItemType* | `types` | `type_count * 0x25` bytes | 0x49F970 (V) |
| 0x10 | 4 | ItemClass* | `classes` | `class_count * 0x2C` bytes | 0x49F830 (V) |
| 0x14 | 4 | ItemRes* | `items` | `item_count * 0x2BF` bytes; getter `0x425CE0(recno) = items + recno*0x2BF - 0x2BF` (no bounds check) | 0x425CE0 (V) |
| 0x18 | 4 | i32* | `sorted` | `recno` list `1..N` sorted by `Code` (case-insensitive) — built by 0x4A3590 (`qsort`, comparator 0x4A3650); used by the code lookup 0x4A36C0 (bsearch, comparator 0x4A3790) | 0x4A3590, 0x4A36C0 (V) |
| 0x24 | 1 | u8 | `loaded` | guard flag for 0x49F620 (free) | 0x49F620 (V) |

Free = 0x49F620: for each record runs 0x4A37E0 (zero the pointer fields), frees `sorted`, `items`, `types`, `classes` (tracked frees, `d:\ww\itemres.cpp`).

### ItemClass (0x2C bytes; table `ITEMCLAS` of the active gameset, dBase `CODE C8, NAME C32`)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 2 | i16 | `index` | 1-based |
| 0x02 | 9 | char[9] | `code` | trimmed copy of the 8-char `CODE` column |
| 0x0B | 33 | char[33] | `name` | trimmed copy of the 32-char `NAME` |

Lookup by name = 0x49FAB0 (case-insensitive, returns index or 0). **Class ids (Std.set, order verified from the section bytes):**

| id | code | id | code | id | code | id | code |
|---|---|---|---|---|---|---|---|
| 1 | GATE | 6 | WEAPON | 11 | LADDER | 16 | ARMOR |
| 2 | OBSTACLE | 7 | POTION | 12 | WAVE | 17 | MISC |
| 3 | HOUSE | 8 | GOLD | 13 | SWITCH | 18 | KEG |
| 4 | WALL | 9 | ENHANCER (amulet/gem/ring) | 14 | PATH | 19 | CROSS |
| 5 | TERRAIN | 10 | BOX | 15 | MISSION | 20 | PTERRAIN (**I**: 20th record not read; the code string occurs as ITEMTYPE.CLASS) |

The numeric ids are hard-coded all over the engine (`class == 10`, `== 0x12`...), so the table order must be preserved.

### ItemType (0x25 bytes; table `ITEMTYPE`, dBase `CLASS C8, TYPE C32`)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 2 | i16 | `class` | ITEMCLAS index of the `CLASS` column (0x49FAB0) |
| 0x02 | 2 | i16 | `index` | 1-based |
| 0x04 | 33 | char[33] | `name` | trimmed `TYPE` text, matched case-insensitively against the item's `ItemType` line (0x49FB30) |

**Type ids (Std.set):** 1 Castle Gate, 2 Obstacle, 3 House, 4 Castle Wall, 5 Normal Terrain Object, 6 Grass Terrain object, 7 Permanent Terrain Object,
8 Normal Left Hand Weapon, 9 Normal Right Hand Weapon, 10 Normal Two Hand Weapon, 11 Normal Range Weapon, 12 Normal Crossbow Weapon, 13 Normal Arrow,
14 Projectile Arrow, 15 Explosive Arrow, 16 Normal Bolt, 17 Projectile Bolt, 18 Explosive Bolt, 19 Cannonball, 20 Normal Stone, 21 Red Potion, 22 Green Potion,
23 Blue Potion, 24 Yellow Potion, 25 All Potion, 26 Gold, 27 Amulet, 28 Gemstone, 29 Ring, 30 Box, 31 Normal Ladder, 32 Switch, 33 Path, 34 Mission item,
35 Mission Letter, 36 Armor, 37 Miscellaneous Object, 38 Keg, 39 Cross. Type ids are also hard-coded (`type == 0x15/0x17/0x19` = sellable potions in 0x6C6550,
`type 8..0x12` equip/ammo routing in 0x495F40, `type 0x25` special branch in the parser, `type == 4` wall branch).

## 2. `ItemRes` record (0x2BF = 703 bytes, `thiscall` getter 0x425CE0)

Offsets are byte offsets from the record start. `i16` fields of the parser are `short*` indexed in the decompile; the byte offsets below were recomputed.
Conditional fields are only written for the classes named; everything else stays zero (the record is `memset` to 0 first, then `CanDrop = 1` (0x25C) and `CanSell = 1` (0x25D) are preset).

| offset | size | type | name | meaning / source key | evidence |
|---|---|---|---|---|---|
| 0x000 | 2 | i16 | `class` | `ItemClass` → ITEMCLAS index | 0x4A0160 (V) |
| 0x002 | 2 | i16 | `type` | `ItemType` → ITEMTYPE index (rest of line, may contain spaces) | (V) |
| 0x004 | 2 | i16 | `recno` | own 1-based id (written by the loader before the parse) | 0x49FBB0/0x49FDF0 (V) |
| 0x006 | 2 | i16 | `model` | `Model` file → modelres id; directory = `"ITEM\" + dirname(Code) + "\"` | (V) |
| 0x008 | 2 | i16 | `shadow_model` | `ShadowModel`; `NONE`/`none` → 0, else modelres id with prefix `"MODEL\"` | (V) |
| 0x00A | 2 | i16 | `attach_bone` | `AttachBoneName` → id from a 10-entry static table (stride 0x38, names like `bip01 R Hand`, `NONE`) via 0x423220 | (R) |
| 0x00C | 4 | i32 | `shadow_refcnt` | run-time refcount of `shadow_model` (0x4A3E30 decrements, frees at 0) | 0x4A3E30 (R) |
| 0x010 | 4 | char* | `code` | `Code` (rest of line, ≤ 0x80), heap copy (0x5A18D0). Unique key; compared case-insensitively. For `.pro`: path relative to `ITEM\`, e.g. `Object\Models_Items\Buildings\Door\WoodGate` | (V) |
| 0x014 | 4 | char* | `name` | `Name` (≤ 0x100): text after the last `\` (display name; in ITEM.RES it is the localised name) | (V) |
| 0x018 | 4 | char* | `message_name` | `MessageName` printf-like pickup/log text with `%1d`/`%1p{a\|b}` plural syntax | (R) |
| 0x01C | 2 | i16 | `struction_type` | `StructionType`: `TOWER`=1, second keyword (static string 0x9D4464; data uses `Wall`)=2, third (0x9D445C)=3, `WOODGATE`=4, `WOODDOOR`=5, `TRUNK`=6 | (R) |
| 0x01E | 2 | i16 | — | zeroed together with 0x1C | (R) |
| 0x020 | 4 | char* | `description` | `Description` (rest of line; the two-character token `\n` is replaced by LF); only read if present | (R) |
| 0x024 | 2 | i16 | `width` | grid cells in the 40-px inventory grid | 0x4577A0 reads +0x24/+0x26 as (w,h) (V) |
| 0x026 | 2 | i16 | `height` | | (V) |
| 0x028 | 2 | i16 | `weight` | read, no consumer seen | (R) |
| 0x02A | 2 | i16 | `quantity` | units per purchase/pick-up stack (arrows: 10, apple: 1) — buy click gives this many | 0x6C5A00 (V) |
| 0x02C | 2 | i16 | `max_quantity` | stack cap in one grid cell | 0x456D60, 0x4577A0 (V) |
| 0x02E | 4 | i32 | `price` | optional key `Price` (only items sold in shops have it); shop prices and sell-back derive from it | 0x4A0160 (V) |
| 0x032 | 4 | i32 | `range` | `Range` (pick-up / interaction radius, 70 for pick-ups, 0 for gates; 41-44 for armour/weapons) | (R) |
| 0x036 | 4 | i32 | `min_level` | `MinLevel`; also the default *item level* of a stack | 0x464030, 0x4CE630 (V) |
| 0x03A | 4 | i32 | `max_level` | `MaxLevel` (level range accepted by `give_item_to_unit`) | 0x50FD90 (V) |
| 0x03E | 4 | i32 | `not_block` | gates only: `notBlock` (0/1); 1 = door has no collision | 0x48D4B0 (V) |
| 0x042 | 4 | i32 | `mp` | class 6 (WEAPON) only: first value after `Range` (`MP`) | (R) |
| 0x046 | 2 | i16 | `color` | class 6 only: second value (`Color`): 0 = (string 0x9D43DC, data `BLUE`), 1 = `GREEN`, 2 = `YELLOW` | (R) |
| 0x048 | 2 | i16 | — | zeroed with 0x46 | (R) |
| 0x04A | 4 | i32 | `damage_type` | `DamageType` → effect enum (§3) | 0x4A45F0 (V) |
| 0x04E | 4 | i32 | `damage` | `Damage` — base effect value | (V) |
| 0x052 | 4 | i32 | `inc_damage` | `IncDamage` — per-level increment. **Effect value at level L = `damage + (L - min_level) * inc_damage`** (0x4A41C0) | 0x4A41C0 (V) |
| 0x056 | 4 | i32 | `defense` | `Defense` | (V) |
| 0x05A | 4 | i32 | `duration` | `Duration` | (V) |
| 0x05E | 4 | i32 | `hp` | classes 1,2,3 and type 4 (gate/obstacle/house/wall): first value after `Name` (`HP`) — initial hit points | 0x4A0160 (V, gate 1500 / wall 500) |
| 0x062 | 4 | i32 | `level_count` | `ModelLevel` (≤ 4): number of model/animation *states* (e.g. intact / damaged / broken) | (V) |
| 0x066 | 1 | u8 | `need_key` | `NeedKey` (gate, box, keg) | (V) |
| 0x067 | 16 | i32[4] | `level_margin` | `LevelMargin` per level (HP threshold; semantics of the compare lives in the gate/wall update, not exported) | (R) |
| 0x077 | 16 | char*[4] | `level_model` | `ModelName`/`ModelName2`/`AddModelName` per level (heap copies; zeroed by 0x4A37E0) | (V) |
| 0x087 | 16 | char*[4] | `level_anim` | `AnimName` per level | (V) |
| 0x097 | 4 | u8[4] | `level_kind` | 0 = anim only, 1 = model, 2 = model+anim, 3 = `AddModelName`, 4 = `ModelName2` | (V) |
| 0x09B | 20 | i32[5] | `level_refcnt` | run-time refcount of model level L (index 0 = base model); zeroed after the parse | 0x4A3E30 (V) |
| 0x0AF | 20 | i32[5] | `level_aux` | per-level run-time handle freed by 0x417E00 | 0x4A3E30 (R) |
| 0x0C3 | 2 | i16 | `blur_count` | `BlurVertexCtr` (0 or 2) | (V) |
| 0x0C5 | 32 | char[32] | `blur_texture` | `BlurTextureName` (e.g. `SLASH_01`), only if `blur_count != 0` | (R) |
| 0x0E6 | 24 | f32[2][3] | `blur_vertex` | `BlurVertexX1..Z1`, `X2..Z2` (weapon trail edge points, model space) | (R) |
| 0x122 | 16 | char*[4] | `emitter_name` | `EmitterName` per slot; `none` (case-insens.) is not stored | (V) |
| 0x132 | 48 | f32[4][3] | `emitter_pos` | X at +12i, Y at +12i+4, Z at +12i+8 | (V) |
| 0x162 | 16 | f32[4] | `emitter_dir` | `EmitterDirection` | (R) |
| 0x172 | 16 | i32[4] | `emitter_res` | run-time particle-resource handle per emitter slot, loaded on the first acquisition of the item (0x4A39F0: `<emitter_name>` + extension → 0x4C2060); not parsed from text | 0x4A39F0 (R) |
| 0x182 | 4 | u8[4] | `emitter_lock` | `EmitterLockType` | (R) |
| 0x186 | 1 | u8 | `has_light` | `HasLight` | (R) |
| 0x187 | 1 | u8 | `light_type` | `light_type:N;` | (R) |
| 0x188 | 1 | u8 | `light_mod` | `ModifyEffect`: 1 = `RANDOM_ATTENUATION` (flicker), else 0 | 0x4B03E0 (V) |
| 0x189 | 4 | u8[4] | `light_rgba` | `Red`,`Green`,`Blue`,`Alpha` | (R) |
| 0x18D | 12 | f32[3] | `light_pos` | `X`,`Y`,`Z` | (R) |
| 0x199 | 8 | f32[2] | `light_cone` | `HD`,`V` (degrees in the file, stored in **radians**, `× 0.017453292`) | (R) |
| 0x1A1 | 12 | f32[3] | `light_att` | `ATT0`,`ATT1`,`ATT2` | (R) |
| 0x1AD | 4 | f32 | `light_radius` | `Radius` | (R) |
| 0x1B1 | 1 | u8 | `has_light2d` | `HasLight2d` | (R) |
| 0x1B2 | 4 | i32 | `light2d_range` | `Light2dRange` | (R) |
| 0x1B6 | 12 | f32[3] | `light2d_pos` | `X`,`Y`,`Z` | (R) |
| 0x1C2 | 2 | i16 | `need_collision` | `NeedCollision` | (R) |
| 0x1C4 | 1 | u8 | `pickable` | `Pickable` (1 = can be picked up; also gates creation of the 0x52-byte floor holder in 0x651AD0) | 0x651AD0 (V) |
| 0x1C5 | 2 | i16 | `has_bbox` | `HasBoundingBox` | (R) |
| 0x1C7 | 24 | f32[6] | `bbox` | `BoxX1,Y1,Z1,X2,Y2,Z2` (min/max) | (R) |
| 0x1DF | 1 | u8 | `can_transparent` | `CanTransparent` | (R) |
| 0x1E0 | 1 | u8 | `modify_camera` | `ModifyCamera` | (R) |
| 0x1E1 | 1 | u8 | `enlarge_scene_sector` | `EnlargeSceneSector` | (R) |
| 0x1E2 | 1 | u8 | `has_antiportal` | `HasAntiPortal` | (R) |
| 0x1E3 | 1 | u8 | `skip_intersect_item` | `SkipIntersectWithItem` | (R) |
| 0x1E4 | 1 | u8 | `has_path` | `HasPath` | (R) |
| 0x1E5 | 2 | i16 | `antiportal_pct` | `AntiPortalPercentage` | (R) |
| 0x1E7 | 1 | u8 | `item_usage` | `ItemUsage`: string 0x9D429C (data `BOTH`)=0, 0x9D4294=2, 0x9D4290=1 | (R) |
| 0x1E8 | 1 | u8 | `show_armory` | `ShowArmoryInterface` — first acquisition adds the item to the *armory-seen* list (GameState+0x728) | 0x6C5A00 (V) |
| 0x1E9 | 4 | i32 | `collision_type` | `CollisionType`: 0 none, 1 Sphere, 2 Box3d, 3 Cylinder (0x4A4540) | (V) |
| 0x1ED | 16 | f32[4] | `col_sphere` | read as ints → float, 4 values | (R) |
| 0x1FD | 24 | f32[6] | `col_box` | `Box3dInfX,Y,Z,SupX,Y,Z` | (V) |
| 0x215 | 20 | f32[5] | `col_cyl` | 5 values | (R) |
| 0x229 | 4 | i32 | `terrain_sound` | `TerrainSound` → sound id (0x4D12F0, 0 = none) | (R) |
| 0x22D | 4 | i32 | `terrain_sound_arg` | the integer following it | (R) |
| 0x231/0x235/0x239 | 3×4 | i32 | `sound`, `sound2`, `sound3` | `Sound`/`Sound2`/`Sound3` sound ids (gate: open/close/break) | (R) |
| 0x23D | 4 | i32 | `use_immediately` | class 7 (POTION) only: value after `Name` (`UseImmediately`); non-zero = consumed on pick-up | 0x50FD90 (V) |
| 0x241 | 4 | i32 | `required_level` | `RequiredLevel` (unit level needed to equip) | (R) |
| 0x245 | 4 | i32 | `req_count` | `EffectiveRequirementType` entries | (V) |
| 0x249 | 4 | Req* | `req` | array of 16-byte `{i32 type_enum, i32 a, i32 b, i32 below_flag}`; `a + b*(L-min_level)` is the requirement at level L; flag: `BELOW`=0, string 0x9D41E0 (`ABOVE`)=1 | 0x4A0160, tooltip 0x6C4070 (V) |
| 0x24D | 4 | i32 | `fx_count` | `AdditionalEffectType` entries | (V) |
| 0x251 | 4 | Fx* | `fx` | array of 12-byte `{i32 type_enum, i32 a, i32 b}` (`AdditionalEffectValue`, `AdditionalEffectIncValue`); value at level L = `a + b*(L-min_level)` | tooltip 0x6C4070 (V) |
| 0x255 | 2 | i16 | `usable_unit` | `UsableUnit` unit-type recno (0x5270B0, 0 = anybody); checked against `unit+0x3B` in 0x495F40 | (V) |
| 0x257 | 2 | i16 | `combine_item` | `CombineItem` → item recno (0x4A36C0): crosses combine into `JOAN_SWORD_LV10` | (R) |
| 0x259 | 2 | i16 | `result_item` | `ResultItem` → item recno (`JOAN_SWORD_LV10A` ...) | (R) |
| 0x25B | 1 | u8 | `unique` | `UniqueItem`: once picked up the id is appended to GameState+0x71C and shops stop stocking it | 0x495F40, 0x4699F0 (V) |
| 0x25C | 1 | u8 | `can_drop` | `CanDrop` (default 1) | (R) |
| 0x25D | 1 | u8 | `can_sell` | `CanSell` (default 1) | 0x6C6550 (V) |
| 0x25E | 1 | u8 | `open_with_interact` | `OpenWithInteract` (boxes) | (R) |
| 0x25F | 32 | char[32] | `detail_file` | `DetailDescriptionFilename` (unless `NONE`) | (R) |
| 0x27F | 32 | char[32] | `detail_nation` | `DetailDescriptionNation` | (R) |
| 0x29F | 32 | — | — | never written | |

### 2.1 Effect enum (`DamageType`, `EffectiveRequirementType`, `AdditionalEffectType`; 0x4A45F0)
1 MAX_HIT_POINT, 2 MAX_ENERGY_POINT, 3 STRENGTH, 4 DEXTERITY, 5 DEFENSE, 6 LEADERSHIP, 7 WEAPON_DAMAGE, 8 WEAPON_DAMAGE_MULTIPLIER, 9 REDUCE_MELEE_DAMAGE_SUSTAINED,
10 ARROW_DEFLECT_OFF, 11 A_ATTACK_DAMAGE, 12 B_ATTACK_DAMAGE, 13 JUMP_ATTACK_B_DAMAGE, 14 RUSH_ATTACK_DAMAGE, 15 DOWN_ATTACK_DAMAGE, 16 RANGE_DISTANCE, 17 RATE_OF_FIRE,
18 MORALE_BASE, 19 MORALE_DROP, 20 FATAL_KILL, 21 EP_CONSUME, 22 DAMAGE_SUSTAINED, 23 TROOP_SIZE, 24 `NONE`; unknown text → 0. (Amulets: `DamageType: NONE`, effect given by `AdditionalEffectType`.)
The upgrade-item enum (§3.3) is a different enum (it has `CHARISMA`, `HIT_POINT`, the combo keys).

### 2.2 How the text is parsed (port rules — 0x4A0160, V)
* Tokens: reader 0x589C70 splits on ` ` `,` `:` `=` TAB (any run) and crosses line ends; max 0x80 chars per token; EOF (0x1A) = end. `0x589F10(n)` = "rest of the current line" (after skipping the same separators), then advances to the next line. Consequence: `light_type:3;` tokenises to `light_type`,`3;` and `atoi` ignores the `;`.
* The first part is **positional**, the middle part is **positional with per-class branches**, the tail is a **keyed `while (tok) { if key == ... }` loop that skips unknown tokens one at a time** (`HP`, `EP`, `Strength`, `Weight`... keys that the tail does not know are silently ignored; a wrong key such as `Inc Damage` therefore yields the tokens `Inc`,`Damage`,`10` and **sets `Damage := 10`** — data quirk of `ARROW`; reproduce by using the same tokenizer).
* Order: `VERSION x` (2 tokens) → `ItemClass`/`ItemType` (value; type is rest-of-line) → `Code` (rest-of-line) → `Model` (rest-of-line) → `ShadowModel` (one token) → `AttachBoneName` (rest-of-line) → optional `Description` (rest-of-line) → optional `StructionType` (one token) → `Name` (rest-of-line) → **class branch** (below) → `Width Height Weight Quantity MaxQuantity` (5 shorts) → optional `Price` → `Range` → [class 6: `MP`, `Color`] → keyed tail.
* Class branches after `Name`:
  * type 37 (`Miscellaneous Object`, tested **before** the class branches): skip key, `+0x62` = next number (a `ModelLevel`-like count), then one more key and one rest-of-line string stored in `level_anim[0]` (+0x87).
  * class 6 WEAPON: `ModelLevel n` then n rest-of-line model names into `level_model[]`.
  * class 1 GATE: `HP`, optional `notBlock`, optional `NeedKey`, `ModelLevel n`, then n groups (`LevelMargin v`, then `AnimName x` or `ModelName x [AnimName y]`).
  * class 2, 3 and type 4 (OBSTACLE, HOUSE, WALL): `HP`, `ModelLevel n`, n groups as above plus `AddModelName x` (kind 3) and `ModelName2 x` (kind 4). Because the shipped walls list `ModelName2` before `LevelMargin`, the first group of a wall is mis-aligned and **wall level 0 gets margin 0 and no model, level 1 gets `AddModelName`** — port the tokenizer, not a "sane" parser.
  * class 10 BOX and 18 KEG: optional `NeedKey`, `ModelLevel n`, n groups of (`AnimName`|`ModelName`+`AnimName`) without `LevelMargin`.
  * class 7 POTION: `UseImmediately v`.
  * all other classes: one token skipped.
* Tail keys (exact spellings): `Damage IncDamage DamageType MinLevel MaxLevel Defense Duration BlurVertexCtr HasEmitter NeedCollision CanTransparent ModifyCamera HasAntiPortal SkipIntersectWithItem AntiPortalPercentage EnlargeSceneSector Pickable ItemUsage TerrainSound Sound Sound2 Sound3 HasBoundingBox CollisionType HasLight HasLight2d RequiredLevel EffectiveRequirementType AdditionalEffectType UsableUnit DetailDescriptionFilename DetailDescriptionNation UniqueItem CanDrop CanSell CombineItem ResultItem HasPath OpenWithInteract MessageName ShowArmoryInterface`. `HasEmitter n` with n != 0 reads exactly **4** emitter groups (name, X, Y, Z, Direction, LockType).

## 3. Sibling static tables parsed by the same module family (all `thiscall`, `this` = table object)

### 3.1 LevelRes — `LEVEL` (levelres.cpp 0x4A4CA0; free 0x4A4C30) — 30 rows
Table object: `+0 count`, `+4 rows*`, `+8 loaded`. Row = 7 bytes: `+0 u8 level`, `+1 i32 exp`, `+5 u8 att_point`, `+6 u8 skill_point` (dBase `LEVEL N2, EXPERIENCE N9, ATT_POINT N2, SKI_POINT N2`). Row lookup by level = 0x4A4E60 (linear on `+0`).
Values in the Polish Std.set (read from the section): L1 1000/5/1, L2 3200/5/1, L3 5800/5/1, L4 10400/6/1, L5 17800/6/1, L6 28500/6/1, L7 45600/6/1, L8 73000/7/1, L9 109600/7/1, L10 161070/7/1,
L11 209470/7/1, L12 272260/8/1, L13 326830/8/1, L14 392170/8/1, L15 485870/8/1, L16 573300/9/2, L17 676600/9/2, L18 784870/9/2, L19 910440/9/2, L20 1108880/10/2, L21 1347470/10/2, L22 1536160/10/2,
L23 1735850/10/2, L24 1961480/11/2, L25 2306660/11/2, L26 2583550/11/2, L27 2893510/11/2, L28 3240780/12/2, L29 3629740/12/2, L30 4065260/12/2 (exp / attribute points per level-up / skill points).
The unit initialiser (0x4F8550) sets `unit.exp_base = row(level-1).exp` for level ≥ 2, i.e. `exp` is the cumulative XP at which that level is *completed* (consumer: the unit family, ../../game/unit.md).

### 3.2 RaceRes — `RACE` (raceres.cpp 0x4CA480; free 0x4CA350)
Table object: `+0 loaded/count`, `+4 count`, `+8 rows*`. Row = 0x34 bytes: `+0 u8 index`, `+1 char code[9]` (`HUMAN`), `+0x0A char name[0x15]`, `+0x1F char adjective[0x15]` (dBase `RACE_CODE C8, NAME C20, ADJECTIVE C20`). One row ships.

### 3.3 UpgradeRes — `UPITEM` + `UPGRADE` (upgrares.cpp 0x531790/0x5319A0; free 0x531690) and per-unit lists (`unitures.cpp`)
Table object: `+0 upitem_count`, `+4 upitem*` (rows 100 bytes), `+8 upgrade_count`, `+0xC upgrade*` (rows 0x35 bytes), `+0x10 loaded`.
* **UpItem row (100 B)** from dBase `TYPE C16 CODE C32 DESC C32 PARA1 C6 PARA12 C6 PARA3 C6 COST N2 REQUIRE C6` (72 rows): `+0 i16 type` (enum 0x531BA0: 1 STRENGTH, 2 DEXTERITY, 3 DEFENSE, 4 CHARISMA, 5 LEADERSHIP, 6 HIT_POINT, 7 ENERGY_POINT, 9 NEW_COMBO, 10 JUMP_ATTACK_B, 11 DOWN_ATTACK, 12 RUSH_ATTACK, 13 ENERGY_ATTACK, 14 RANGE_ATTACK, 15 HORSE_ATTACK_A, 16 HORSE_ATTACK_B; 8 unused, unknown = 0), `+2 i16 recno`, `+4 char code[33]` (`HP+4`, `STRENGTH+1`, `NEW_COMBO_AA`...), `+0x25 char desc[33]` (localised text), `+0x46 char para[3][7]`, `+0x5B i16 cost` (skill-point cost), `+0x5D char require[7]`.
* **Upgrade row (0x35 B)** from `CODE C32, DBFNAME C8` (43 rows, e.g. `JOANUP`): `+0 i16 recno`, `+2 char code[33]`, `+0x23 char section[9]` (name of the per-unit table), `+0x2C i32 count`, `+0x30 UpgradeStep*` (4-byte `{u16 level, u16 upitem_recno}` rows built by `unitures::fn_00530D70` from the section's `LEVEL N2, CODE C32` rows, e.g. `1  HP+4`), `+0x34 u8 loaded`. Getter 0x531E70 (`recno` 1-based, logs `UpgradeRes::get_unit_upgrade_list_res` when out of range).
* The unit-side consumer (level-up menu) is in the unit/HUD families; this module only supplies the tables.
