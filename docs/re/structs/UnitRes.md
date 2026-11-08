# UnitRes (`unitres.cpp`) — unit-type template record, sizeof = 0x203 (515)

One record per `[VERSION 1.0 ... END]` block of `unit/unit.res` (shipped: **114 records**, 30-byte longest code). Table object: `+0 UnitRes* base`,
`+4 i16 count`; accessor `0x42D0A0(type_id)` = `base + (type_id-1)*0x203` (this = the table). Loader `unitres::fn_00524E30` (**0x524E30**: opens
`UNIT\unit.res` through ResLX with `(path, load_all=1, shared=0)`, allocates `count*0x203` with `memset(0)`, per entry sets `rec.recno = i` (u16 @+0), wraps the
payload in a `FileTxt` and calls the record parser, then copies the ResLX entry name (<= 0x20 chars) to `rec+4`, i.e. **the ResLX index name overrides the `Code :` line**).
Record parser `unitres::fn_00525080` (**0x525080**, 6.5 KB). Free: `unitres::fn_005273B0` (**0x5273B0**) frees only the four attach-point arrays
(`+0x1B1,+0x1B5,+0x1B9,+0x1BD`). File format: [../../formats/res.md](../../formats/res.md) (container) + this page (keys).

Text grammar (token reader 0x589C70, see [../functions/script_res.md](../functions/script_res.md) §1): the parser is **positional** — every line is `Key : Value`,
the key token is read and **discarded**, only the value token is used, strictly in the order of the table below. Key spelling is therefore cosmetic and a missing/extra
line shifts every later field. Exceptions that make the order data-dependent: `HighPolygonAnimation` is skipped when `HighPolygonModel` is `none`; `ModelName/AnimationName`
pairs repeat `ModelLevel` times; `TriggerType` is a list ended by `TRIGGER_END`; `EArmorCode` is optional (probed by key name); `Resource` exists only when `UnitClass == SIEGE_WEAPON`;
the collision block depends on `CollisionType`; attach points repeat `AttachPointCount` times. Numbers: `atoi` (0x7B15CB) unless noted `atof` (0x7B2110).
Sibling lines such as `AttachPointUsage` are keyed by position only.

## Layout (offsets in the in-memory record)

| offset | size | type | name (unit.res key) | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 2 | u16 | `recno` | 1-based index in the table (= unit type id stored in `Unit+0x3B`) | 0x524E30 |
| 0x02 | 2 | u16 | `UnitClass` | `0x527220`: CITIZEN 0, LIGHT_INFANTRY 1, HEAVY_INFANTRY 2, POLEMAN 3, MACEMAN 4, ARCHER 5, CROSSBOW_MAN 6, CAVALRY 7, SIEGE_WEAPON 8, literal@0x9D7B3C 9 (unreadable; probably a cart/vehicle class), anything else 1. Drives target priority (0x527790), search ranges (0x65B280), drop tables, formation columns | 0x525080 verified |
| 0x04 | 0x21 | char[] | `Code` | unit code (`ALENCON`, `ENGLISH_ARCHER`...), overwritten by the ResLX entry name | 0x524E30 |
| 0x25 | 0x21 | char[] | `Model` | base model file (`alencon_armorA.am1`) | 0x525080 |
| 0x46 | 0x21 | char[] | `HighPolygonModel` | `none` (literal 0x9CF608, case-insensitive) or a file; the following `HighPolygonAnimation` is read only if not `none` | 0x525080 |
| 0x68 | 4 | u32 | `ShadowModel` handle | `modelres::fn_004B18D0("MODEL\\", name, 0)` | verified |
| 0x6C | 4 | u32 | high-poly model handle | 0 when `none` | read |
| 0x70 | 4 | u32 | high-poly anim-group id | | read |
| 0x74 | 4 | i32 | `ModelLevel` | number of extra model levels (0..2 in data) | verified |
| 0x84 | 3x4 | u32[3] | model handles | `[0]` = `Model`, `[1..2]` = `ModelName` entries (armour tiers A/B/C: `alencon_armorB.am1`...); `modelres::fn_004B18D0("UNIT\\", file, animgrp)` | verified |
| 0x90 | 3x4 | u32[3] | anim-group ids | `Animation` / `AnimationName` names resolved by 0x418430 (animgrp lookup) | verified |
| 0x9C | 4 | f32 | `WalkSpeed` | **atof**; used by movement (0x4F0B50): action WALK(10) | verified |
| 0xA0 | 4 | f32 | `RunSpeed` | action RUN(11) | verified |
| 0xA4 | 4 | f32 | `ShiftLeftSpeed` | MOVE_L(0xE), MOVE_FL/FR/BL/BR (0x11..0x14) | verified |
| 0xA8 | 4 | f32 | `ShiftRightSpeed` | MOVE_R(0xF) | verified |
| 0xAC | 4 | f32 | `ShiftBackSpeed` | MOVE_B(0x10) | verified |
| 0xB4 | 0x28 | char[] | `Name` | display name (cp1250 on the Polish install; `[0001N]`-style string ids are NOT used here) | verified |
| 0xDE | 0x20 | char[] | `ShortName` | | verified |
| 0x100 | 8 | char[] | `CFileName` | character-file stem (`None`, `richemon`); sound/voice lookups | read |
| 0x109 | 2 | u16 | `Height` | | read |
| 0x10B | 2 | u16 | `BodyShape` | `NORMAL`=1, `SHORT`=2, literal 0x9D7BA4 (probably `NONE`)=0 | read |
| 0x10D | 8 | char[] | `Birthday` | `07/07/77` | read |
| 0x117 | 1 | char | `Sex` | first char of the token: `M`/`F` (used by level-up text 0x4DE580 vs 0x4DE5C0 when `=='F'`) | verified (0x522C40) |
| 0x118 | 2 | u16 | `Special` | `NORMAL` 0, literal@0x9D4294 1 (the data word is `MAIN`), `HORSE` 2, `ELEPHANT` 3, `CANNON` 4, `FAUCONNEAU` 5, `CROSSBOW` 6, `ANIMAL` 7, `SEIGE_TOWER` 8, `BOMBARD` 9, `TREBUCHET` 10, `BATTERING_RAM` 11, `AUTOBOW` 12, literal@0x9D7B3C 13 (cart), `SPECIAL` 14. Selects siege behaviour (crew slots, actions) | verified |
| 0x11C | 4 | u32 | `TriggerType` flags | `TRIGGER_ATTACK`=0x1, `_ARROW_ATTACK`=0x2, `_THROW`=0x10, `_STONE_ATTACK`=0x40, `_INTERACT`=0x200, `_DOWN_ATTACK`=0x400, `_EXPLOSIVE_ARROW_ATTACK`=0x2000, `_EXPLOSIVE_KEG_ATTACK`=0x4000; `TRIGGER_END` terminates. A timed hit event (0x4EEF00, mask bits = same values) damages this unit only if `(event.mask & TriggerType) != 0` (0x4EF090) | verified |
| 0x120 | 1 | u8 | `is_hero` | `0x430050(Code)`: 1 if Code equals one of the 4 controllable-hero names at 0x9F861C (stride 0x104; first = `JOAN_OF_ARC`). Heroes are excluded from the AI auto-upgrade timer | verified (0x52C490) |
| 0x121 | 4 | i32 | `ScriptDuration` | **AI think period in ms** (250 for generals/heroes, 500 for troops/siege); read by 0x49D810 | verified |
| 0x125 | 4 | i32 | `ScriptFile` -> AI recno | `NONE`/no `.` -> 0; otherwise the text before the first `.` is looked up case-insensitively in the `unitAI.res` name table (`0x5464A0`, stride 0x28, 1-based). e.g. `human_light_infantry.scr` -> 4 | verified |
| 0x129 | 0x20 | char[] | `Title` | | read |
| 0x149 | 2 | u16 | `Ranking` | `0x527130`: INFANTRY 0, ARCHER 1, CAVALRY 2, CAPTAIN 3, GENERAL 4, literal@0x9D0B10 5; anything else 0. Copied to `Unit+0x104` via 0x5089E0 | verified |
| 0x14B | 2 | u16 | `KingdomId` | `Unit+0x108` | read |
| 0x14D | 2 | u16 | `RaceId` | `0x4CA400` race code -> index (0 = unknown -> fatal log "Unknown Race %s in %s") | verified |
| 0x151 | 4 | i32 | `HP` | base max HP | verified |
| 0x155 | 4 | i32 | `EP` | base max EP (energy points) | verified |
| 0x159 | 4 | i32 | `Life` | extra lives (0 for ordinary units; revive in place when HP reaches 0 while > 0) | verified (0x4FE510) |
| 0x15D | 4 | i32 | `Experience` | experience reward / (heroes: starting value; see UnitLevel) | read |
| 0x161 | 4 | i32 | `Gold` | | read |
| 0x165 | 4 | i32 | `Resource` | present only for `UnitClass == 8`; siege-weapon build cost | read |
| 0x169 | 4 | i32 | `UpgradeList` | index of the unit-upgrade list (`0x531DF0`, stride 0x35, 1-based, 0 = unknown) | verified |
| 0x16D | 2 | u16 | `EArmorId` | | read |
| 0x16F | 2 | u16 | `EArmorCode` | item recno via 0x4A36C0 (only if the next key is literally `EArmorCode`) | read |
| 0x171 | 2 | u16 | `PWeaponId` | item recno (0x4A36C0): primary melee weapon (`NONE` -> 0) | verified |
| 0x173 | 2 | u16 | `SWeaponId` | secondary melee (shield) | verified |
| 0x175 | 2 | u16 | `PRangeWeaponId` | primary ranged (bow) — only item classes 0xB/0xC accepted by 0x497270 | verified |
| 0x177 | 2 | u16 | `SRangeWeaponId` | ammo/quiver | verified |
| 0x179 | 2 | u16 | `DropItemList` | index into the drop-list table (0x463ED0, stride 0x2B, 1-based) | verified |
| 0x17B | 2 | i16 | `Strength` | -> `Unit+0x155` (setter 0x4FE6E0) | verified |
| 0x17D | 2 | i16 | `Dexterity` | -> `Unit+0x157` | verified |
| 0x17F | 2 | i16 | `Defense` | -> `Unit+0x159` | verified |
| 0x181 | 2 | i16 | `Leadership` | -> `Unit+0x15B` | verified |
| 0x183 | 2 | i16 | `ArcherySkill` | -> `Unit+0x16F` | verified |
| 0x185 | 2 | i16 | `HorseRiding` | -> `Unit+0x173` | verified |
| 0x187 | 2 | i16 | `SwimmingSkill` | -> `Unit+0x177` | verified |
| 0x189 | 2 | i16 | `ElephantRiding` | -> `Unit+0x17B` | verified |
| 0x18B | 2 | i16 | `WideAnimal` | -> `Unit+0x17F` | verified |
| 0x18D | 2 | i16 | `Preaching` | -> `Unit+0x183` | verified |
| 0x18F | 2 | i16 | `PowerJump` | -> `Unit+0x187` | verified |
| 0x191 | 2 | i16 | `Disguise` | -> `Unit+0x18B` | verified |
| 0x193 | 2 | i16 | `Persuade` | -> `Unit+0x18F` | verified |
| 0x195 | 2 | i16 | `LockPicking` | -> `Unit+0x193` | verified |
| 0x197 | 2 | i16 | `Mountain` | -> `Unit+0x197` | verified |
| 0x199 | 2 | i16 | `Level` | -> `Unit+0x124` (also seeds `Unit+0x126` experience floor) | verified |
| 0x19B | 2 | u16 | (not parsed) | always 0 from the loader; copied to `Unit+0x12E` | verified |
| 0x19D | 2 | i16 | `Loyalty` | -> `Unit+0x161` | verified |
| 0x19F | 2 | i16 | `Morale` | -> `Unit+0x163` | verified |
| 0x1A1 | 2 | i16 | `Reputation` | -> `Unit+0x165` | verified |
| 0x1A3 | 2 | i16 | `Behavior` | 1..4 -> `Unit+0x167` (1 ATTACK, 2 ACTIVE, 3 IDLE per `set_unit_mode`; 4 unused) | verified |
| 0x1A5 | 2 | i16 | `RegenHp` | -> `Unit+0x141` | read |
| 0x1A7 | 2 | i16 | `RegenEp` | -> `Unit+0x14D` | read |
| 0x1A9 | 2 | i16 | `Falling` | -> `Unit+0x1CB` (balance meter start, 0..100) | verified |
| 0x1AB | 2 | i16 | `Dizzy` | not copied by the exported unit init | read |
| 0x1AD | 4 | i32 | `AttachPointCount` | n crew/attachment points (siege weapons: 2..8) | verified |
| 0x1B1 | 4 | f32* | attach positions | `n*12` bytes, x,y,z each parsed with **atoi then cast to float** (so fractional digits in the file are dropped); `Unit` rotates them with the actor matrix | verified |
| 0x1B5 | 4 | f32* | attach `Dir` | `n*4`, atof, degrees; unit heading is set to `heading + Dir*pi/180` | verified |
| 0x1B9 | 4 | u8* | attach `Usage` | `n` bytes: `M`=1, `F`=2, `B`=3 (bit0/bit1 flags; bit0 = pusher/driver seat, bit1 = loader/gunner seat in 0x501190 / 0x4F2190) | read |
| 0x1BD | 4 | u16* | attach item | `n*2`: item recno (0x4A36C0) held by that crew position | read |
| 0x1C3 | 4 | i32 | `CollisionType` | `0x527D20`: `None` 0 (and unknown), `Sphere` 1, `Box3d` 2, `Cylinder` 3 | verified |
| 0x1C7..0x1D3 | 4x4 | f32[4] | sphere: centre xyz, radius | (atoi -> float) | verified |
| 0x1D7..0x1EB | 6x4 | f32[6] | box: min/max xyz (order as read) | (atoi -> float) | read |
| 0x1EF..0x1FF | 5x4 | f32[5] | cylinder: `CylinderCenterX/Y/Z`, `CylinderRadius` (0x1FB), `CylinderHeight` (0x1FF) | (atoi -> float). **Z is the vertical axis** (shipped CenterZ = 40, Height = 80 for a 73-tall man) | verified |

Total = 0x203; the three collision blocks overlap-free (only the block of the declared type is written).

## Shipped data facts (cross-check of `unit/unit.res`, 188,578 bytes, 114 records)

* Soldiers: `ScriptFile` is `human_light_infantry/human_heavy_infantry/human_archer(_ex)/...` for the rank-and-file and `*_general.scr`/`archer_gen.scr`/`archer_ex_gen.scr` for captains;
  heroes and named characters use `special_general.scr`; `citizen.scr` for civilians, `pig.scr` (-> AI recno of `PIG`, class 0 = no native AI), `cart.scr`, `human_horse.scr`,
  `camp_horse.scr`, `human_cannon.scr`, `human_trebuchet.scr`, `autobow.scr`, `batterram.scr`, `NONE` for shop-keepers/priests.
* `ScriptDuration`: 250 ms for `special_general`, 500 ms for most others.
* `Ranking` strings in the file that the ladder does not know (`Batterram`, `CANNON`) fall through to 0.

## Open points

* TODO(re): literal 0x9D0B10 (Ranking 5), 0x9D7B3C (UnitClass 9 / Special 13), 0x9D4294 (Special 1), 0x9D7BA4 (BodyShape 0) are unreadable in the export; the names above are inferred from the file.
* TODO(re): `Dizzy` (+0x1AB) has no reader in the exported code (stun resistance?).
