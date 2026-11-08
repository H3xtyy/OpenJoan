# LightSys — the day-stage / sky-light singleton (`lightsys.cpp`)

One global object at **0xA47E08** (derived: its `special_stages` pointer lives at 0xA47E10 = base + 8, which 0x614760 / 0x5EBAC0 read as
`DAT_00A47E10`; the live scatter block 0xA47F40 = base + 0x0C + 0x12C). All members are packed (unaligned `i16`/`ptr` at odd offsets).
Methods: 0x4A5970 (unload), 0x4A5BF0 / 0x4A6840 (load `.lgt` + `.sca`), 0x4A7450 (load `.day`), 0x4A9120 / 0x4A9530 (find stage by name).
Reference build: joan.exe. Stage layout: [LightStage](LightStage.md); scatter block: [ScatterParams](ScatterParams.md).

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x000 | 2 | i16 | stage_count | stages in `common`/per-map `.lgt` (`StageNum`) | verified |
| 0x002 | 4 | LightStage* | stages | `stage_count * 0x18C`, freed with tracked free (`lightsys.cpp`) | verified |
| 0x006 | 2 | i16 | special_count | stages of `special.lgt` (Rain, Snow, Lightning) | verified (0x4A6840, 0x4A9530) |
| 0x008 | 4 | LightStage* | special_stages | | verified |
| 0x00C | 0x18C | LightStage | current | embedded "current" stage; its `scatter` member (0x138) is the live block 0xA47F40 | inferred (address arithmetic) |
| 0x198 | 0x18C | LightStage-shaped | delta_a | **per-game-minute deltas** of the running stage transition: light `R,G,B,A` / `v_dir` / `h_dir` deltas at the same offsets as a LightStage entry, `is_on` bytes copied from the target, scatter deltas at block `+0x15C..+0x188` (dir, colour, rayleigh, mie, inscattering, g, dist). Written by 0x4B21A0, consumed by 0x4A7F70. Name = target stage name | verified (0x4B21A0, 0x4A7F70) |
| 0x324 | 4 | f32 | delta_a_remaining | game minutes left in the transition (the block's `+0x18C`) | verified |
| 0x328 | 4 | f32 | delta_a_factor | `1.0 - weather_intensity` (block `+0x190`); scales colour/scatter deltas (not direction deltas) | verified |
| 0x32C | 0x18C | LightStage-shaped | delta_b | same for the **weather overlay** (Rain/Snow special stage): light colour deltas (`R,G,B,A`, `is_on`) and scatter deltas except direction. Written by 0x4C02B0 | verified |
| 0x4B8 | 4 | f32 | delta_b_remaining | minutes left in the overlay blend | verified |
| 0x4BC | 4 | f32 | weather_intensity | overlay weight 0..1 (rain/snow percentage / 100), also scales the overlay deltas | verified |
| 0x4C0 | 4 | f32 | last_time | game minute at the previous update (dt source for 0x4A7F70) | verified |
| 0x4C4 | 2 | i16 | day_count | `.day` `StageCount` | verified (0x4A7450, 0x4A5970) |
| 0x4C6 | 2 | i16 | day_current | 1-based index of the schedule entry whose transition is running (0 = none) | verified |
| 0x4C8 | 4 | DayEntry* | day_schedule | `day_count * 10` bytes: `{i16 stage_index (1-based into `stages`, 0 = name not found), f32 start_min, f32 duration_min}` (packed). Index comes from 0x4A9120 | verified |
| 0x4CC | 0x40 | char[64] | cloud_tex_1 | `CloudTexture1` | read |
| 0x50C | 0x40 | char[64] | cloud_tex_2 | `CloudTexture2` | read |
| 0x54C | 0x40 | char[64] | sky_mesh | `SkyMesh` (e.g. `model\sky01.smo`) | read |
| 0x58C | 4 | f32 | rain_probability | `RainProbability` raw | read |
| 0x590 | 4 | f32 | rain_percentage | `RainPercentage / 100.0` (0x9D03C0) | read |
| 0x594 | 4 | f32 | rain_start_time | game minute at which rain will start (0 = none planned); chosen at world init: `rand(100) < rain_probability` then `rand(N) + DAT_00A47BC4` | read (0x4A5020, 0x4A7F70) |
| 0x598 | 4 | f32 | rain_cooldown | minutes of rain left: set to `rand(180) + 400` when rain starts, decremented by dt; rain ends at <= 0 | read |
| 0x59C | 4 | f32 | snow_probability | `SnowProbability` raw | read |
| 0x5A0 | 4 | f32 | snow_percentage | `SnowPercentage / 100.0` | read |
| 0x5A4, 0x5A8 | 4+4 | f32 | snow_start_time / snow_cooldown | same as rain (cooldown `rand(180)+400`) | read |
| 0x5AC | 4 | BillboardSprite* | moon_sprite | **corrected**: the 0x37-byte billboard "moon01" (100 x 100 world units) created by 0x4A7980/0x4A7BA0 when `sun_dir.z >= 0` (night), deleted when it is day;  on unload removed from the scene object list (0x5EDED0: removes pointer from `scene+0xC5` array, count `scene+0xC1`) then deleted through its vtable (slot 0, arg 1) | read |
| 0x5B0 | 2 | i16 | lights_per_stage | `LightNum` | verified |
| 0x5B2 | 4 x lights_per_stage | EngineLight*[] | light_handles | live [EngineLight](EngineLight.md) objects, one per light index, created by 0x4A5020 | verified |

The "current" stage at `+0x0C` is addressed in 0x4A7F70 as `this+0x4E + i*0x1A` (lights) and `this+0x168..0x194` (scatter fields): `0x168/16C/170` sun_dir,
`0x174/178/17C` sun_color, `0x184` rayleigh, `0x188` mie, `0x18C` inscattering, `0x190` g, `0x194` distance_modifier (the live block 0xA47F40 starts at `+0x138`).

Notes
- The offsets quoted in [day.md](../../formats/day.md) (`+0x1388`, `+0x1630`, ...; to be reconciled) are **not** those of this build: the 0x4A7450 export stores
  `StageCount` at `+0x4C4`, rain at `+0x58C/+0x590`, snow at `+0x59C/+0x5A0`, strings at `+0x54C` (SkyMesh), `+0x4CC`, `+0x50C`. The relative
  order of fields is identical.
- Unload (0x4A5970) also calls the lens-flare list release (0x6626A0, see [LensFlareList](LensFlareList.md)) and 0x689D00(0) (clears a global
  effect array, identity unresolved).
