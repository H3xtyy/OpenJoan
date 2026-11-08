# `.ter` — territories (`map/scenarioN/<name>w.ter`, 7 files, 63 territories)

Reader: dispatch **0x547590** (`CreateTerritory` 0x9D8050) -> per-territory reader 0x4F9330. Grammar: [block.md](block.md).
```
CreateTerritory { TerritoryName; CenterX; CenterY; CoordinateX; CoordinateY; Kingdom; [DisableBeingAttacked: 1;]
                  [ScrName;] Type; Scale; TerritoryAreaCount: n;
                  n x ( TerritoryAreaType: circle|rectangle|quadrilateral; area keys ) }
```
Area keys carry the original's misspelling `Territroy…`:
- `circle`: `TerritroyAreaX`, `TerritroyAreaY`, `TerritroyAreaRadius`
- `rectangle`: `TerritroyAreaX1`, `Y1`, `X2`, `Y2` (two opposite corners)
- `quadrilateral`: `TerritroyAreaX1..X4`, `Y1..Y4`

All numbers are integers. Scripts use `set_territory_nation("name", "nation")`.

## Observations over all 7 files
- `Type`: `camp` (20), `castle` (12), `church` (1), `city` (2), `forest` (2), `town` (26).
- `Scale`: `castle`, `large_camp`, `settlement`, `small_camp` — one block spells the key lowercase (`scale`, S4); keys are
  case-insensitive.
- `TerritoryAreaType`: `circle` (47), `rectangle` (13), `quadrilateral` (5). 65 areas in 63 territories; `TerritoryAreaCount` always
  equals the number of areas.
- `Kingdom`: `English`, `French`/`french`, `Mercen`, `Neutral`.
- `ScrName` is **optional** (absent in 7 blocks, all in scenario 6); `DisableBeingAttacked:1` occurs once (S3, Orléans).
- Names contain cp1250 letters (`Orléans`).

## Open questions
- `TODO(re)`: read 0x4F9330 for the exact semantics of `CenterX/Y` vs `CoordinateX/Y` and of `DisableBeingAttacked`.
