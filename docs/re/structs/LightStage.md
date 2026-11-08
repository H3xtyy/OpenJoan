# LightStage (0x18C bytes) and LightEntry (0x1A bytes) — `lightsys.cpp`

One **day stage** of the light system: the lights of a [`.lgt`](../../formats/lgt.md) stage plus the sky-scattering record of the paired
[`.sca`](../../formats/sca.md) stage. Allocated as `stage_count * 0x18C` zeroed bytes (`0x58E7B0`, `lightsys.cpp`) by the loaders
0x4A5BF0 (`common`/per-map) and 0x4A6840 (`special`); index `i` of the `.lgt` pairs with record `i` of the `.sca`.
Owner object: [LightSys](LightSys.md). Reference build: joan.exe. Status column: **verified** = read consistently in
>= 2 functions, **read** = one function, **inferred** = deduced from sizes/usage.

## LightStage

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 0x40 | char[64] | name | stage name (`strncpy` of `StageName`, copied for every *lit* light, so the last lit light wins); compared case-insensitively (0x7C07B0) by 0x4A9120 / 0x4A9530 | verified |
| 0x40 | 2 | i16 | light_count | = `LightNum` of the file (same for all stages), written for every stage even if its lights are off | verified (0x4A5BF0, 0x4A6840, loop bounds) |
| 0x42 | 9 x 0x1A | LightEntry[9] | lights | packed (unaligned) light records, see below. Capacity is **9** lights: 0x42 + 9*0x1A = 0x12C exactly | inferred (arithmetic; shipped data uses <= 2) |
| 0x12C | 0x60 | [ScatterParams](ScatterParams.md) | scatter | sky scattering block; the last 0x60 bytes of the stage (0x12C + 0x60 = 0x18C). `+0x15C`..`+0x188` are the `.sca` fields | verified |

Because `scatter` starts at 0x12C, pseudocode that adds `300` to a stage pointer (`DAT_00A47E10 + 300 + (i-1)*0x18C`, 0x614760 / 0x5EBAC0)
is addressing `ScatterParams` directly.

## LightEntry (packed, stride 0x1A, base = stage + 0x42 + n*0x1A)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | type | `Type:` 0 = ambient, 1 = directional (same numbering as the engine [EngineLight](EngineLight.md) types 0/1) | 0x4A5BF0 (+0x42) |
| 0x01 | 1 | u8 | is_on | `IsOn:`; when 0 the file record holds nothing else and the rest of the entry stays zero | 0x4A5BF0 (+0x43) |
| 0x02 | 4 | f32 | red | `Red:` 0..255 (int in the file, stored as float) | +0x44 |
| 0x06 | 4 | f32 | green | | +0x48 |
| 0x0A | 4 | f32 | blue | | +0x4C |
| 0x0E | 4 | f32 | alpha | `Alpha:` 0..255 (see lights.md: no consumer found yet) | +0x50 |
| 0x12 | 4 | f32 | v_dir | `VDir:` elevation, **radians** (degrees x constant at 0x9D37A8, presumably pi/180) | +0x54 |
| 0x16 | 4 | f32 | h_dir | `HDir:` horizontal direction, radians (same factor) | +0x58 |

File read order inside a lit entry is `IsOn, StageName, Type, Red, Green, Blue, Alpha, HDir, VDir` (see lgt.md); note `HDir` is stored
*after* `VDir` in memory (+0x58 vs +0x54).

## Allocation facts
- Loaders memset the whole array to 0, so unlit entries read as type 0 / off / colour 0.
- Stage count lives in [LightSys](LightSys.md) `+0x00` (common) / `+0x06` (special); `light_count` is duplicated per stage and in `LightSys +0x5B0`.
- `TODO(re)`: no function that consumes `lights[]` (turning entries into [EngineLight](EngineLight.md) objects, time-of-day blending) is in the export;
  see "Open questions" in [lights.md](../functions/lights.md).
