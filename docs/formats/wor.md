# `.wor` — world file (`map/scenarioN/<name>w.wor`, 7 files)

Reader (`src/game/world_s.cpp`): **0x54A6C0** (loop over
`CreateNode` -> 0x47AD40 and `CreateWater` -> 0x553AA0; also referenced by the terrain path builder 0x608E10
`TERRAIN\`). Grammar: [block.md](block.md).
```
CreateGrid { X; Y }                                   ; terrain node grid
CreateNode { scene_name; NodeName; NodeFile: <name>.STG; Path: Terrain\[scenarioN\]; X; Y; Z; HD; WIDTH; LENGTH; HEIGHT } x n
CreateWater { ... }                                   ; same fields as .stg CreateWater (stg.md)
```
Observed grids (X x Y): scenario1 8x8, scenario2 16x8, scenario3 8x16, scenario4 16x16, scenario5 16x40,
scenario6 48x24, scenario7 32x16. 32 `CreateNode` blocks in total (1, 2, 2, 4, 10, 8, 5) and 5 `CreateWater`.
`scene_name` is empty in every file; `X/Y/Z/HD` are floats (`256.000000`), `WIDTH/LENGTH` integers (512),
`HEIGHT` 1. `Path` is the original's terrain directory (`Terrain\`, `Terrain\scenario5\`, ...) and is informational:
every `NodeFile` is found **in the same directory as the `.wor`** and all 32 are `.stg` files. Sibling files with the same base: `.day` (schedule), `.map`
(binary), `.ter`.
