# `.day` — per-map day-stage schedule and weather (`map/scenarioN/*.day`, 9 files)

Reader: `lightsys.cpp` — joan.exe **0x4A7450** (`.day` literal 0x9D46F8; builds the path from the world name).
Grammar: [keyvalue.md](keyvalue.md).

> **Offset note (2026-10-22):** the in-memory offsets quoted below (+0x1388 …) and the ones in
> [`docs/re/structs/LightSys.md`](../re/structs/LightSys.md) (+0x4C4 count, +0x58C/+0x590 rain, +0x59C/+0x5A0 snow,
> +0x4C8 schedule) were read from the same Steam build against different base pointers; `TODO(re)`: reconcile
> against `0x4A7450` with the LightSys base `0xA47E08`. The field *order and semantics* agree. Stage values are the
> state at the **end** of a schedule entry — the engine blends linearly toward them over `Duration` game minutes
> (see `docs/engine/lights.md` §3).

```
record 1:  StageCount: n                      ; int16 at +0x1388
           RainProbability: %      RainPercentage: %      ; floats at +0x1630 (raw) / +0x1634 (value / 100.0 = 0x9D03C0)
           SnowProbability: %      SnowPercentage: %      ; floats at +0x1670 (raw) / +0x1674 (value / 100.0)
           SkyMesh: model\sky01.smo                       ; read_line, 0x40 chars, +0x1530
           CloudTexture1: name     CloudTexture2: name    ; read_line, 0x40 chars, +0x1330 / +0x14b0
records 2..n+1: StageName: <name>  StartTime: <minutes>  Duration: <minutes>
                ; 10-byte entry: int16 stage index (name looked up by 0x4A9120), float start, float duration
```
Positional order of the original is exactly the one above and **the snow pair is not optional there**: it always
reads four floats before `SkyMesh`. All seven world files (`S?L1w.day`) have it. The two leftovers `S1L1.day` (no snow
lines) and `S2L1.day` (no snow, no `SkyMesh`; one stage `Sunny02` 100..220) are not referenced by any `.scr`
(`WorldFileName` + `.day` always names a `...w.day`); they lack the snow pair and `SkyMesh`
and would be mis-read by the original.

Times are game minutes. Stages are contiguous in file order (`StartTime` of the next = previous start + `Duration`)
and cover exactly 1440 minutes in every file except `S2L1.day`. The cycle need not start at 0: world files start at 180
(`Normal_06` 180..540, `Normal_09`, `Normal_12`, `Normal_15`, `Normal_18` 1080..1400, `Normal_Night` 1400..1620 = wraps
midnight) and `S1L1.day` uses `Special_Night_1..3` at 0/480/960. Stage names all occur in `common.lgt`/`special.lgt`.

Values in the install: `RainProbability` 20/90/…, `RainPercentage` 70, snow 0/0, `SkyMesh` always `model\sky01.smo`,
cloud textures `newcloud02`/`newcloud01` (`S2L1.day`: `sunny_sky02a`/`sunny_sky02`).

## Open questions
- `TODO(re)`: semantics of probability vs percentage (chance per day vs coverage fraction) — consumer of +0x1630..0x1674.
