# `.pro` — two unrelated formats share the extension

The flavour of a file is told by its first key: bare `VERSION …` → model; `StageCount` → day cycle; `EffectCount` → weather; `Rayleigh` → scattering;
`LightNum` → SkyBox. 824 files in the install. Grammar: [keyvalue.md](keyvalue.md).

## A. Resource profiles (5 files in `Resource/`)
No reader for these was located by the literal (`.pro` string 0x9D40E1 is used for model files). `TODO(re)`: find the
consumers; the layouts below are taken from the files.
- `day.pro` (`DayCycle`): `StageCount: 4`, then records `StageName`/`StartTime` = SunRise 7, Day 11, SunSet 17, Night 19
  (**hour of day**, 0..23 — unlike `.day`, which uses minutes).
- `WeatherEffect.pro` (`WeatherEffect`): `EffectCount: 2`, then `EffectStageName: Rain` / `Snow` (one record).
- `Scat.pro`, `Cloud.pro` (`Scattering`): the [`.sca`](sca.md) 11-field record, one per stage, each preceded by a `# Name`
  comment that is the stage name: SunRise, Day, SunSet, Night, Rain, Dummy, Dummy2, Current (8 records). `Cloud.pro` is
  `Scat.pro` plus one trailing blank line (identical content). The stage name is the comment line directly above
  the record's first line.
- `SkyBox.pro` (`SkyBoxLights`): `LightNum: 1`, `StageNum: 8`, then for each of the 8 stages a record `Duration: N`
  (120, 180, 120, 60, 60, 30, 60, 20 — unit unknown) followed by **two** light records (`# Light #1 …` and `# Ambient
  Light …`, i.e. `LightNum` + 1), each with `StageName Red Green Blue Alpha Brightness V_Dir Is_On`. Looks like an older,
  fixed-stage variant of `.lgt`/`day.pro` (stage names SunRise Day SunSet Night Rain Dummy Dummy2 Current).

## B. Model property files (819 files beside `.smo`/`.am1` under `item/Object/**`)
Bare first line `VERSION 1.0`, bare last line `END`; four files contain blank lines inside (still one logical list).
Keys in the order below for the first 20 (always present, except `HasPath` missing in 6 files), then a variable tail:

| key | type | values seen |
|---|---|---|
| `ItemClass` | enum | TERRAIN 730, WALL 23, GATE 22, BOX 11, MISC 11, LADDER 5, PTERRAIN 5, HOUSE 5, and a few 1-off: `Gate`, `KEG`, `OBSTACLE`, `Switch`, `WAVE`, `WEAPON` (case varies!) |
| `ItemType` | text | `Normal Terrain Object` 654, `Grass Terrain Object` 77, `Castle Gate`, `Castle Wall`, `Box`, `Miscellaneous Object`, `Normal Ladder`, `Permanent Terrain Object`, … (13 values) |
| `Code`, `Name` | path/text | object code = `Object\…` path without extension; `Name` is usually the same or a display name (`Treasure Box`) |
| `Model`, `ShadowModel` | file | `.SMO`/`.AM1` beside the `.pro`; `ShadowModel` `shadow_bitmap_tree.SMO` 748, `NONE`/`none` 71 |
| `AttachBoneName` | name | `NONE` (815) or a bone name |
| `Width` `Height` `Weight` `Quantity` `MaxQuantity` `Range` `Damage` `Defense` `Duration` | int | mostly 10/10/10/1/1/0/0/0/0 |
| `BlurVertexCtr`, `HasEmitter`, `CanTransparent`, `ModifyCamera` | 0/1 | `BlurVertexCtr` always 0 |
| `HasPath` | 0/1/2 | 0: 553, 1: 253, 2: 7 |

Optional keys (any order, kept verbatim in `ModelProperties::extra`): `EnlargeSceneSector`, `EmitterName/X/Y/Z/Direction/
LockType`, `LevelMargin`, `ModelLevel`, `HP`, `MP`, `ItemUsage` (BOTH), `AnimName`, `Sound`/`Sound2`/`Sound3`, `ModelName`,
`ModelName2`, `AddModelName`, `HasAntiPortal`/`AntiPortalPercentage`, `NeedCollision`, `StructionType` (Wall, Gate, Tower,
WoodDoor, WOODGATE, Trunk), `CollisionType` (Box3d) with `Box3dInfX/Y/Z`, `Box3dSupX/Y/Z`, `HasBoundingBox` + `BoxX1 BoxY1
Boxy1 BoxZ1 BoxX2 BoxY2 BoxZ2`, `Description` (localised prompt text with `%1s` and `%2?{a|b}` placeholders — in the file's
code page), `Pickable`, `NeedKey`, `OpenWithInteract`, `SkipIntersectWithItem`, `notBlock`,
`Color`, `HasLight2d`/`Light2dRange`, and in two files an embedded light block (`HasLight`, `light_type: 3;`, `Red…Radius`,
`ModifyEffect: RANDOM_ATTENUATION;` with **trailing semicolons** in the values, like [`.bm`](bm.md)). Keys are
case-inconsistent (`BoxY1` vs `Boxy1`); match case-insensitively. Every model `.pro` has the 20 base keys, without duplicates, and 0/1 flags.

## Open questions
- `TODO(re)`: reader of the model `.pro` (itemres/modelres) — meaning of `HasPath` 2, `ModelLevel`, `LevelMargin`.
- `TODO(re)`: consumers of `day.pro`, `Scat.pro`, `Cloud.pro`, `SkyBox.pro`, `WeatherEffect.pro`.
