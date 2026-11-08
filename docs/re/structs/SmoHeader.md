# SmoHeader (on disk, `.SMO` static and `.AM1` skinned)

Reader `RE: joan.exe 0x5E0530` (static), `0x5DC420` (skinned). Verified: see `docs/formats/smo.md` section 3.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | u32 | version | 500 (0x1F4); 0, 50 legacy, 100 stub | 0x5DDD70 dispatch; every shipped file |
| (skinned only) 0x04 | 4 | i32 | quality | passed to model init (0x5CC7A0 arg 4); 10 in ripple.AM1 | 0x5DA280 |
| 0x04 / 0x08 (s) | 4 | i32 | plane_count | | highpolytree: 1; verified |
| 0x08 / 0x0C (s) | 4 | i32 | texture_count | | highpolytree 1, alexander#v 0 |
| 0x0C / 0x10 (s) | 4 | i32 | lod_count | static: LOD blocks follow textures | 0 in all samples |
| 0x10 / 0x14 (s) | 16 | f32[4] | bounds | centre xyz + length (sphere diameter?) | tree: (0, 3.4e-5, 355.13, 1493.89) |

`(s)` = offset in the skinned `.AM1` file. Static header is 0x20 bytes, skinned 0x24 bytes. No padding.
