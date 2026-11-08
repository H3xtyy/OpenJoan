# ScatterParams (0x60 bytes) — sky scattering block of a LightStage

The last 0x60 bytes of a [LightStage](LightStage.md) (`stage + 0x12C`), and also the layout of the **live** block at global `0xA47F40`
(the "current stage" that the sky/fog code reads every frame). Offsets below are relative to the block. Reference build: joan.exe.
Formats: [sca.md](../../formats/sca.md) (the 11 file fields). Algorithm that consumes it: engine/lights.md "Sky scattering".

Defaults are written by **0x617EE0** (`scatter_params_set_defaults`, called once per stage inside the `.lgt` loader before the `.sca` record
overrides the file fields). The 24 default dwords decode as follows (values decoded from the hex immediates).

| offset | size | type | name | default (0x617EE0) | meaning | status |
|---|---|---|---|---|---|---|
| 0x00 | 12 | f32[3] | unknown_a | (0.0138, 0.0113, 0.0080) | not read by 0x614760; probably a terrain/ambient term of the vertex shader (`c_terrain_reflectance`?) | `TODO(re)` |
| 0x0C | 12 | f32[3] | beta_rayleigh | (6.95e-6, 1.18e-5, 2.44e-5) | per-channel Rayleigh scattering coefficient (blue scatters most) | verified (0x617EE0 values + 0x614760 use) |
| 0x18 | 12 | f32[3] | beta_mie | (2.0e-5, 3.0e-5, 4.0e-5) | per-channel Mie scattering coefficient; also the Mie *angular* term base | verified (0x614760) |
| 0x24 | 12 | f32[3] | beta_mie_abs | (3.7e-5, 4.4e-5, 6.5e-5) | added to `beta_mie` for the extinction sum (`beta_mie + beta_mie_abs`) | read (0x614760); role name inferred |
| 0x30 | 12 | f32[3] | sun_dir | (0.0, 0.0628, -0.9980) | unit vector; `.sca` `DirectionX/Y/Z` normalised on load. Dot-multiplied with the camera forward vector in the phase function | verified (0x4A5BF0 normalise; 0x614760; debug sliders 0x4D48F0 "Direction X/Y/Z") |
| 0x3C | 12 | f32[3] | sun_color | (0.8764, 0.8328, 0.7375 — raw 0x3f3ccccd; an earlier reading said 0.75) | `.sca` `Red/Green/Blue`, 0..1 | verified |
| 0x48 | 4 | f32 | sun_intensity | 100.0 (0x42C80000) | constant; the `.sca` loader forces 100.0. Multiplied with `inscattering` at the end | verified (0x4A5BF0 sets 0x174 = 0x42C80000; 0x614760 uses) |
| 0x4C | 4 | f32 | rayleigh | 0.2 | `.sca` `Rayleigh`; scales `beta_rayleigh` | verified |
| 0x50 | 4 | f32 | mie | 0.01 | `.sca` `Mie`; scales `beta_mie` (+ abs) | verified |
| 0x54 | 4 | f32 | inscattering | 0.3 | `.sca` `Inscattering`; final brightness factor | verified |
| 0x58 | 4 | f32 | g | 0.8 | `.sca` `G`, Henyey-Greenstein-style asymmetry | verified |
| 0x5C | 4 | f32 | distance_modifier | 1.0 | `.sca` `DistanceModifier`; multiplies the extinction (scales apparent path length) | verified |

Sizes cross-check: 12 floats (0x30) + dir 3 + colour 3 + 6 = 24 dwords = 0x60.

## The live block (0xA47F40)
- Global `0xA47F40` is the same struct. Its fields are individually addressable globals used by the debug panel: `sun_dir` 0xA47F70/74/78,
  `sun_color` 0xA47F7C/80/84, `sun_intensity` 0xA47F88, `rayleigh` 0xA47F8C, `mie` 0xA47F90, `inscattering` 0xA47F94, `g` 0xA47F98,
  `distance_modifier` 0xA47F9C. These match the overlay at 0x4D48F0 one-to-one.
- It is embedded in the [LightSys](LightSys.md) singleton (0xA47E08 + 0x138) as the `scatter` member of a stage-sized "current stage" struct
  at singleton `+0x0C`. The code that blends the day stages into it is not in the export (`TODO(re)`).
- 0x61D200 (`scatter_params_apply`) is only a `memcpy(this + 0x20, params, 0x60)`: it copies the whole block into a 0x60-byte member of the sky object
  (called by 0x5EBAC0 with the live block, or the special stage "Lightning"). The conversion into the vertex-shader constants of `shader/scatter.nvo`
  (`c_beta1_plus_beta2`, `c_beta_dash_1/2`, `c_f_hg`, ...) therefore happens later, in the (unexported) sky draw / shader-constant code.
- The live block is written by the stage-blend code 0x4A7F70: `sun_dir`, `sun_color` and the five scalars
  are interpolated; `unknown_a`/`beta_*` vectors are never blended (they stay at the 0x617EE0 defaults for every stage).
- Constants resolved (dump of the data section): `k = 100.0` (0x9CF9C4), `KR = 5.9683` (0x9D9D38 = 3/(16 pi) * 100), `KM = 7.9577` (0x9D9D34 = 1/(4 pi) * 100).
