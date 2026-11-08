# `.lgt` — day-stage lights (`map/common.lgt`, `map/special.lgt`, `map/scenarioN/*.lgt`)

Loader: `lightsys.cpp` — joan.exe **0x4A5BF0** (takes a base name: `map/common` or the per-map name; opens
`<base>.lgt` via literal 0x9D46E4 *and* `<base>.sca` via 0x9D46DC) and **0x4A6840** (identical, fixed name `special`).
Both read the `.lgt` and the paired [`.sca`](sca.md) in one pass. Grammar: [keyvalue.md](keyvalue.md).

```
record 1 : LightNum: <n>        ; lights per stage         (first token pair, stored at +0x5b0)
           StageNum: <m>        ; number of stages          (second pair, stored at +0)
records 2..(1+n*m): one light each, grouped by stage in file order (stage-major):
           IsOn: 0|1            ; if 0 the record contains ONLY this line (the reader skips the rest)
           StageName: <name>    ; read_line, <=0x40 chars; every lit light of a stage names the same stage
           Type: 0|1            ; 0 = ambient, 1 = directional  (verified: common.lgt light 0 of each stage is Type 1,
                                ;                                light 1 is Type 0)
           Red/Green/Blue/Alpha: 0..255   (stored as floats)
           HDir: float degrees  ; horizontal direction  (stored in +0x58, multiplied by degrees->radians 0x9D37A8)
           VDir: float degrees  ; elevation             (stored in +0x54, same factor)
```
RE detail: per stage the reader allocates `m` stage structs of 0x18c bytes (name at +0, light array of 0x1a bytes
each from +0x44). The stage name is copied by `strncpy` for every *lit* light, so the stage's name is the last lit
light's `StageName`. After the lights it reads the stage's `.sca` record (11 floats, see [sca.md](sca.md)); stage `i`
of the `.lgt` pairs with record `i` of the `.sca` by **position** (the `.sca` name is read and discarded).
Count check: records after the header = `LightNum × StageNum` (verified for all 34 files).

## Census (all 34 files)
- `map/common.lgt`: 2 × 18. Stage order: Day, Sunny01, Sunny02, Cloudy, Raining, Night, Normal_06, Normal_09,
  Normal_12, Normal_12_S8, Normal_15, Normal_18, Normal_Night, Normal_Night_S8, Special_Night_1, Special_Night_2,
  Special_Night_3, Special_S3. Alpha is 255 for the first 6 and the Night stages, 0 for Normal_06..18 and Special_S3.
- `map/special.lgt`: 2 × 3 (Rain, Snow, Lightning in `special.sca`; the `.lgt` stage names match positionally).
- Of the 32 per-scenario files, 22 are `LightNum: 0 / StageNum: 0` (nothing else) and 10 are 2×1 (one stage, two
  lights). Their stage name is `ambient` (4 lit lights) or the literal text `Type:`
  (13 lit lights: the editor wrote `StageName: Type:` — an authoring glitch the original reads happily because the
  reader is positional). 3 lights over all files are `IsOn: 0`, which then has no other fields.
- `common`/`special` `.lgt` and `.sca` have the same number of stages and the same names in order.

## Open questions
- `TODO(re)`: how Alpha, and the `Type: 1` light's `HDir`/`VDir` pair, enter the D3D light (lights/day stages); Alpha is 0 in the Normal_* stages so it is probably unused for sun/ambient.
