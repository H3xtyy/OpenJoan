# `.uni` — unit placement (`map/scenarioN/*.uni`, 32 files, 6,113 `CreateUnit` blocks)

Reader: `world_s.cpp` — block loop **0x558890**, per-unit reader **0x558A60** (joan.exe; retail equivalents
TODO(re)). Grammar: [block.md](block.md). The reader is positional; the field order below is what it expects.

| # | key | type | reader behaviour (RE: 0x558A60) |
|---|---|---|---|
| 1 | `scene_name` | id | token skipped (unit is placed into the scene sector named here — its origin is added to X/Y, see below) |
| 2 | `UnitName` | text ≤ 63 | `read_line` into a 64-byte buffer, trailing `;` stripped; stored at unit+0x3D (display name used by scripts' `search_unit_by_name`) |
| 3 | `UnitShortName` | text ≤ 63 | **optional**: only if the next key token equals `UnitShortName`; stored at +0x7F |
| 4 | `UnitFile` | name | unit definition name (`unit/<name>.*`), resolved via 0x5270B0 → unit type id |
| 5 | `Path` | path | skipped (`read_line` into a 255-byte buffer) |
| 6 | `X`, `Y` | float | `atof` + **sector origin** (`sector[0x11]`, `[0x12]` added) → world position |
| 7 | `Z` | float | world z |
| 8 | `HD` | float | heading; the key token is compared with the literal at 0x9D4234 = `"HD"`: spelled `HD` -> degrees, value × π/180 (0x3F91DF46A2529D39); any other spelling -> radians as written. All 6,113 shipped units use `HD` (degrees). |
| 9 | `WIDTH`, `LENGTH`, `HEIGHT` | int | three `read_line` skips (bounding box, unused by the reader) |
| 10 | `Kingdom` | name | nation by name (0x4B91A0) → nation recno |
| 11 | `Group` | int (`atoi`) | stored at unit+0x31 (team/group id). Read with `atoi`: editor leftovers are accepted and become 0 — **1,608 units have `Group:none`, one has `Group:TOWER_ATTACK`** (a `Task` value in the wrong slot). |
| 12+ | optional until `}` | | `Task:<name>` (key compared as `"TASK"`, case-insensitive; task by name 0x4E4610; 8 = idle variant with `Range`), `Range:<int>`, `Treasure:<item name>` (drop item 0x463ED0, else item 0x4A36C0), `TreasureLevel:<int>` (-1 = default from item table), `UnitLevel:<int>` (default 1) |

The world position of a unit needs the `.stg` sector table: the file holds the position relative to the sector named by `scene_name`, and the engine adds the sector origin.

## Observations over all 32 files
- 32 files (7 scenarios), 6,113 `CreateUnit` blocks, every block has every key (incl. `UnitShortName`, `Task`, `Range`,
  `Treasure`, `TreasureLevel`, `UnitLevel`) in exactly the order of the table; `Path` is always `Unit\`.
- `Task` values (case as written): `none`, `idle`, `Idle`, `TOWER_ATTACK` (5,910 / 19 / 2 / 182).
- `Kingdom` values (case as written): `English`, `english`, `French`, `french`, `Mercen`, `mercen`, `none`, `NONE`, and
  empty (`Kingdom:;`, 4 units). Compare case-insensitively.
- `UnitFile`: 108 distinct names. `Treasure` is empty in most units; `TreasureLevel` `-1` = item default.
- Names may contain cp1250 letters (`UnitName:Orléans`) — stored in the release code page.
- The first 14 keys always come in the order of the table; the five optional keys follow in any order.
