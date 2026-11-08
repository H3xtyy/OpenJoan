# PlaneMaterial (`sizeof` = 0x54)

Per-material record of a [Plane](Plane.md) (`Plane::materials`, stride 0x54). Same 0x54 bytes are the on-disk "material
image" of [PlaneChunks.md](PlaneChunks.md) step 6 (texture pointers at `+0x24..+0x33` are zeroed on load and re-bound by name).
Init `0x5D52A0`, copy `0x5D53E0`, equality `0x5D5500` (plain `memcmp` of all 0x54 bytes), release textures `0x5D5360`,
bind texture `0x5D5650(slot, tex)` (AddRef new, Release old via `0x42CEF0`).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | D3DCOLOR (B,G,R,A bytes) | diffuse | converted to float RGBA `byte * (1/255)` with R/B swap for `D3DMATERIAL8` | 0x5D56F0, 0x5E9260 |
| 0x04 | 4 | D3DCOLOR | ambient | initialised to global default `DAT_00A69CD0` (diffuse also set to it) | 0x5D52A0 |
| 0x08 | 4 | D3DCOLOR | specular | | 0x5D56F0 |
| 0x0C | 4 | D3DCOLOR | emissive | | 0x5D56F0 |
| 0x10 | 4 | float | power | `D3DMATERIAL8.Power` | 0x5D56F0 (`+0x40 = this+0x10`) |
| 0x14 | 4 | ? | unknown | copied by copy ctor | 0x5D53E0 |
| 0x18 | 4 | float | proj_strength | `> 0` (const `DAT_009CF5C4`) and `Plane.proj_stage[mat] != -1` while global `DAT_00A58055 == 2` enables projective texturing (texgen camera-space position, transform flags `0x104` = COUNT4 \| PROJECTED) for that material | 0x5D0570 |
| 0x1C | 4 | u8[4] | tint | RGBA multiplier, combined with `Plane.tint_rgba` as `a*b/255` per channel | 0x5D0570 |
| 0x20 | 4 | u32 | flags | blend/alpha/cull flag word handed to the render-state cache (`0x6026A0`); bits 8..11 are the opacity/fade nibble cleared by the fade-in code `0x5F3FE0` (`& 0xFFFFF0FF`) | 0x5D0570, 0x5F3FE0 |
| 0x24 | 16 | Texture*[4] | tex | up to 4 texture stages (see [Texture.md](Texture.md)); D3D texture at `Texture+4`; name at `Texture+0x18` | 0x5D0570, 0x5D5650 |
| 0x34 | 4 | u32 | mode_a | default 1. If `(v & 0xFF000000) == 0x01000000` it is an **animated-texture-set** reference: bits 0..15 resource id (`0x4B27B0`), bits 16..23 variant | 0x5D52A0, 0x5D0570 |
| 0x38 | 4 | u32 | mode_b | default 0; render-state-cache slot (`0x5F8250`) | 0x5D52A0 |
| 0x3C | 4 | u32 | mode_c | default 0; render-state-cache slot (`0x5FEC10`) | 0x5D52A0 |
| 0x40 | 4 | u32 | pass | default 1. **Draw pass**: 1 = opaque pass, 2 = translucent pass; a material is drawn only when `pass == requested pass` (`0x600B30`) | 0x5D52A0, 0x5D0570, 0x5D35B0 |
| 0x44 | 16 | u32[4] | tex_anim_time | per stage: global time (`DAT_00A500BA`, ms) of the last animation frame switch | 0x5D0570 |

Animated texture sets: the resource at `0x4B27B0(id)` holds per variant/stage: frame count (i16 at `res+0x31+variant*2`... as indexed
in the code), frame period in ms (float at `res+0x301+variant*0x10+stage*4`), frame name table (`res+0x3C1`, 0x21 bytes per
name, 0x84 per variant) and a "projective" flag byte per stage (`res+0x6D9`). When `now - last_switch >= period` the next frame
name (cyclic, `name + frame_number` string suffix trick) is requested from the texture cache (`0x6099C0`, flag 1; 3 when
`Plane.env_mode == 2`) and bound with `DAT_00A403D8` temporarily forced to `0xFFFFEC78` (-5000, a texture priority/bias).
The exact string handling is only partly decoded: TODO(re): 0x5D0570 lines handling `local_e4` (names built with
`strcpy/strcat/strstr/atoi`).
