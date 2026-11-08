# `.sca` — sky scattering per day stage (`map/common.sca`, `map/special.sca`)

Loader: `lightsys.cpp` 0x4A5BF0 / 0x4A6840 — the `.sca` is opened right after the `.lgt` (literal 0x9D46DC) and read
inside the per-stage loop, one record per stage, **positionally** (the `StageName` text is read with `read_line`
into a scratch buffer and discarded; stage `i` of the `.sca` belongs to stage `i` of the `.lgt`). Value consumer:
scattering **0x4D48F0** (references `Rayleigh` 0x9D5CD8). Grammar: [keyvalue.md](keyvalue.md).

Record read order (RE: 0x4A5BF0, stage struct offsets +0x178.. / +0x168.. / +0x15c..):

| # | key | type | stored at | meaning |
|---|---|---|---|---|
| 0 | `StageName` | name | — (discarded) | day stage; same order as the `.lgt` |
| 1 | `Rayleigh` | float | +0x178 | Rayleigh coefficient |
| 2 | `Mie` | float | +0x17c | Mie coefficient |
| 3 | `Inscattering` | float | +0x180 | in-scatter factor |
| 4 | `G` | float | +0x184 | Mie phase asymmetry |
| 5 | `DistanceModifier` | float | +0x188 | distance scale for in-scattering |
| 6-8 | `Red`, `Green`, `Blue` | float 0..1 | +0x168/16c/170 | sun colour (+0x174 is set to the constant 100.0f = 0x42c80000) |
| 9-11 | `DirectionX/Y/Z` | float | +0x15c/160/164 | sun direction; **normalised on load** (`vector_normalize`) — the files hold `0,0,-1`, `1,0,-0.1`… |

Census: `common.sca` has 18 records, `special.sca` 3 (`Rain`, `Snow`, `Lightning`). Both match the paired `.lgt`
in count and names. `Resource/Scat.pro` / `Cloud.pro` hold the same 11 fields but name each record with a
`# <StageName>` comment instead of `StageName:` (see [pro.md](pro.md)). The `DirectionZ` of Rain/Snow is `+1.0`.

## Open questions
- `TODO(re) 0x4D48F0`: how `G` and `DistanceModifier` enter the shader constants.
