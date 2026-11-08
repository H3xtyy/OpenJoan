# `.stg` — stage/scene file (`map/scenarioN/*.stg`, 32 files, up to 800 KB)

Reader (`src/game/world_s.cpp`): block dispatcher **0x44E5F0**
(`CreateLight` -> 0x49B970, `CreateObject` -> 0x4F3DF0, `CreateWater` -> 0x4F5210, `CreateTrigger` -> 0x450EA0,
`CreateProjectiveShadow` -> 0x4749C0, `CreateParticle` -> 0x551930; `CreateNode` is compared but dispatches
nothing), sector reader **0x42C6B0** (`CreateGrid` header, then `CreateSceneSector` -> 0x551440). Grammar:
[block.md](block.md). Blocks are strictly ordered by the editor as grid, sectors, then the rest in arbitrary
interleaving; every shipped file has exactly one `CreateGrid`.

Every field is mandatory unless marked optional; all keys are matched case-insensitively (`x` vs `X`).

- `CreateGrid { X; Y }` — sector grid dimensions in cells. Observed: 256x256 in 20 files and 220x220 in the 12
  files of scenarios 3 and 5. Square in every file.
- `CreateSceneSector { scene_id; X1; X2; Y1; Y2; Z1; Z2; is_grid_scene; index_x; index_y; size_x; size_y }` x 2,828
  total — axis-aligned boxes (integers, world units; Z1/Z2 are -1000/2000 in the samples). `is_grid_scene` is 1 in
  every file, `size_x/size_y` 16. `scene_id` is what `.uni`/`CreateObject` reference in `scene_name`.
  **The sector's X1/Y1 are the origin added to object/unit X/Y** (see uni.md).
- `CreateObject { scene_name; ObjectName; ObjectFile; Path; X; Y; Z; HD; VD; WIDTH; LENGTH; HEIGHT; Kingdom; Treasure;
  TreasureLevel; Scale; InitState; FinalState [; TriggerType] }` — **32,332 in total** over the 32 files (min 294,
  max 2,006 per file); static props/buildings (`item/Object/<Path><ObjectFile>.smo` + `.pro`). `X/Y/Z` floats,
  `HD/VD` degrees, `WIDTH/LENGTH/HEIGHT` integers, `Scale` float (`1.00`). `scene_name` is usually
  `Default scene sector`. `Kingdom` and `Treasure` may be empty or `none` (both mean "no value"; seen kingdoms
  `english French Mercen`, spelled with varying case). `TreasureLevel` -1 = unset. `InitState`/`FinalState` 0|1.
  The optional trailing `TriggerType` appears on 1,796 objects, always `0`.
- `CreateLight { scene_name; light_name; light_type; Red; Green; Blue; Alpha; HD; V; X; Y; Z; ATT0; ATT1; ATT2; Radius;
  ModifyEffect; ModifyRate; ModifyRange; Group }` — 827 in total; the `.bm` `CreateLight` fields plus
  `scene_name`, `ModifyEffect` (`""` or `RANDOM_ATTENUATION`), `ModifyRate`, `ModifyRange`, `Group` (light group
  number, 1..~20). **`light_type` set: {3}** (point light; same enum as bm.md).
- `CreateTrigger { trigger_type; x; y; z; radius; target_item | code }` — 137 in total. **`trigger_type` set:
  {`TRIGGER_PLAY_ANIMATION` (115, with `target_item` = object name), `TRIGGER_PLAY_AMB_SOUND` (22, with `code` =
  ambient sound code such as `CAMPFIRE`, `BLACKSMITH`)}**. One block spells the keys `X Y Z` in capitals.
- `CreateWater { scene_name; WaterName; WaterSoundCode; X; Y; Z; TextureDirection; TextureWidth; Reflection; WaterDirection;
  Ripple; WIDTH; LENGTH; ParticleEffect1; ParticleEffect2 }` — 14 in total (the same block appears in `.wor`).
  **`Reflection` set: {`spheremap`, empty}** (7 each). `WIDTH/LENGTH` floats, the direction/width/ripple fields integers.
- `CreateParticle { ParticleName; X; Y; Z; HD }` — 69 in total; pscript emitter placement, `HD` degrees.
- `CreateProjectiveShadow` — recognised by 0x44E5F0 (reader 0x4749C0) but absent from every shipped file;
  TODO(re) 0x4749C0 for its fields.

The original silently skips unknown block names; none occurs in the shipped files. Totals over the install: sectors 2,828, objects 32,332, lights 827, triggers 137, waters 14,
particles 69.

Open: `TODO(re) 0x44E5F0` defaults of empty fields (consumers treat `none` and
empty the same); whether `TriggerType` is ever non-zero (not in the shipped data).
