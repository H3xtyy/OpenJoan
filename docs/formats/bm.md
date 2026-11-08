# `.bm` — objective-map scene (`script/scenario/S?L1_map.bm`, 7 files)

Loader: `interfac/minires.cpp`-adjacent **0x731A60** (references `CreateLight` 0x9D8248 and `CreateMapActor`
0x9DF43C): a plain token loop with no array header; `CreateLight` fields are read by 0x731B00, `CreateMapActor` by
0x731FF0 (neither analysed). Grammar: [block.md](block.md). Used by the 3D objective map shown in the briefing /
minimap screen.

```
CreateLight {                       ; 1..N, fields in this order (the reader looks them up by name)
    light_type: ambient | point;    ; the only values in the 7 files (7 ambient, 14 point)
    Red; Green; Blue; Alpha: 0..255 ints (Alpha is 255 everywhere);
    HD; V: degrees (0 everywhere);  X; Y; Z: position (ambient: 0 0 0);
    ATT0; ATT1; ATT2: attenuation constant/linear/quadratic (1 0 0 everywhere);
    Radius: range, 0 = unlimited (ambient 0, point 2000)
}
CreateMapActor {                    ; exactly one per file
    ModelName: <smo base name, e.g. s5_3dmap>; X; Y; Z: placement; X1; Y1; X2; Y2: map rectangle (symmetric
    about 0, e.g. -171.875 -429.6875 .. 171.875 429.6875); HD: 0|180|270 degrees; Scale: 1
}
```
Each file has one ambient light (90,90,90 in S5) followed by a few point lights (200,200,200) and one
`CreateMapActor`. Values may be floats (`171.875`). Missing field = error (the original silently keeps defaults,
`TODO(re)` 0x731B00).

## Open questions
* Whether `directional` exists in the engine's light-type table (0x731B00 not analysed; shipped files only use
  `ambient` and `point`).
* The role of `HD`/`V` on point lights (always 0) and of the `X1..Y2` rectangle (probably the minimap click
  mapping).
