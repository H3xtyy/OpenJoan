# Direct3D 8 render states observed in the original (apitrace)

Captures live in `$JOAN_WORK_DIR/traces/` (never committed).

## Capture 1 — intro + main menu, 2026-10-22 (`menu-2026-10-22.trace`, 50 s, Wine + DXVK d3d8)

Device: `CreateDevice(HAL, HARDWARE_VERTEXPROCESSING, 1024×768 X8R8G8B8 fullscreen, 1 back buffer, FLIP, D24S8,
PRESENT_INTERVAL_IMMEDIATE)`. SDK version 120 (DX 8.0). No vertex or pixel shader is ever set in this capture
(`SetVertexShader` is called once with the FVF `XYZ|DIFFUSE|TEX1`; `SetPixelShader(0)` once).

Per-frame shape (16 643 presents): `TestCooperativeLevel` → `SetViewport` → `BeginScene` → 2D batches → `EndScene`
→ `Present`. Only two `Clear` calls in the whole capture (both `D3DCLEAR_TARGET`, black) — the menu repaints every
pixel with full-screen quads, so no clear per frame.

### Render states (set once at device init, `0x270`–)

| State | Value |
|---|---|
| `D3DRS_CULLMODE` | **`D3DCULL_CW`** (clockwise faces culled → counter-clockwise front faces, left-handed D3D space) |
| `D3DRS_LIGHTING` | FALSE |
| `D3DRS_SPECULARENABLE` | FALSE |
| `D3DRS_LOCALVIEWER` | FALSE |
| `D3DRS_ALPHABLENDENABLE` | TRUE, `SRCBLEND = SRCALPHA`, `DESTBLEND = INVSRCALPHA` |
| `D3DRS_ALPHATESTENABLE` | FALSE, `ALPHAREF = 128`, `ALPHAFUNC = GREATEREQUAL` (prepared for the 3D scene) |
| `D3DRS_ZENABLE` | `D3DZB_FALSE`, `ZWRITEENABLE` FALSE (2D only) |
| `D3DRS_FOGENABLE` | FALSE — re-set every frame |
| `D3DRS_CLIPPLANEENABLE` | 0 — set twice per frame |

Texture stage defaults set at init for stages 0–7: `MAG/MIN/MIPFILTER = LINEAR`, stage 1 `ADDRESSU/V = WRAP`,
`TEXCOORDINDEX = 1`, `TEXTURETRANSFORMFLAGS = DISABLE`.

### 2D drawing idiom

* Stage 0: `COLOROP = SELECTARG1 (TEXTURE)`; `ALPHAOP = SELECTARG1 (TEXTURE)` for textured quads or
  `SELECTARG2 (DIFFUSE)` for tinted/faded ones; stage 1 `COLOROP/ALPHAOP = DISABLE`.
* Stage 0 filters are toggled between `POINT` and `LINEAR` roughly once per frame (pixel-exact UI vs scaled video).
* Geometry: `DrawIndexedPrimitive(TRIANGLELIST)` from vertex/index buffers (`SetStreamSource`/`SetIndices`),
  ~2 draws per frame in the menu; `SetTransform` ×4 and `SetMaterial` ×3 per frame even though lighting is off.

### Textures created in the menu

| Format | Count | Use |
|---|---|---|
| `A4R4G4B4` 1 level (256–1024 px) | 38 | menu images (`.bm` → 16-bit with alpha) |
| `DXT5` with full mip chain | 3 | `.dds` assets |
| `DXT3` 512×512 ×10 levels | 1 | `.dds` |
| `R5G6B5` 512×288 / 800×450 | 2 | video frame targets (WMV decode → texture) |
| `X8R8G8B8` 512×256 `RENDERTARGET` | 1 | render-to-texture (TODO(re): used by?) |
| `A8R8G8B8` 256×256 | 1 | |
| `V8U8` 256×256 | 1 | capability probe, released immediately (call 262–263) |

Conclusion: the original culls **clockwise** triangles; the `.smo` strips were found counter-clockwise when seen from the
normal side, which matches. The OpenGL device (`src/gl`) applies the net winding after the Y flip of its viewport; see
[../render/D3D8_USAGE.md](../render/D3D8_USAGE.md).

## Capture 2 — scenario 1 gameplay, 2026-10-22 (`gameplay-1-2026-10-22.trace`, 1.3 GB, ~33 k presents)

Captured by the user with `tools/re/apitrace_capture.sh gameplay-1` (intro → menu → scenario 1 play). Histogram:
`$JOAN_WORK_DIR/traces/gameplay-1-states.txt`; the two vertex-shader sources are saved next to it
(`vs_1_1_call4164434.txt`, `vs_1_0_call4163966.txt`, game assets — not committed).

### Shaders (the "nvlinker" fragments of the original)

| Handle | Kind | Declaration | What it does |
|---|---|---|---|
| 5 (and 7, 9: same program re-created per scene) | `vs_1_1`, 2212 + 1411 + 50 binds | `v0` pos, `v1` second pos? (`XYZB1` stream), `v5` diffuse, `v7` uv, `v8/v9` normals, `v12` blend indices (x,y), `v13` blend weights | **Hardware skinning**: two influences, bone matrices as 4×3 at `c17 + index` (`mov a.x, v12.x; m4x3 r0, v0, c[a+17]; … mad with v13.y`), world-view-proj in `c1..c4`, view (4×3) in `c5..c7`, one directional light `c89` (dir) / `c90` (colour) with `max(N·L, 0)` + ambient `c9 × c11`, `oD0 = light × v5 + ambient`, `oT3.x = dp4(pos, c91)` (a planar projection — shadow/lightmap), range fog `oFog = 1 − (dist × c92.z + c92.w)` from the eye `c10`. Constant uploads per draw: `c17` blob of 1152 or 1104 bytes (24/23 bones × 48 B). |
| 3 | `vs_1_0`, 1274 binds | `v0` pos, `v7` uv | **Sky scattering** (`shader/scatter.nvo`): `oPos` from `c1..c4`, eye-relative direction from `c5..c7` and eye `c10`, texture matrix `c23/c24`, sun direction `c25`, `exp()` of `−c26 × depth × c0.w` per channel (the Beta·distance extinction) scaled by `c27`, … (full listing in the saved file) |
| pixel shader | none ever set (`SetPixelShader(0)` only) | | all fragment work is texture-stage fixed function |

Everything else draws fixed-function with FVFs: `XYZ|NORMAL|DIFFUSE|TEX1` (8165 binds: static meshes, terrain
base), `XYZB1|NORMAL|DIFFUSE|TEX1` (6296: grass / skinned streams), `XYZ|NORMAL|DIFFUSE|TEX4` (4239: terrain with
four UV sets), `XYZ|DIFFUSE|TEX1` (2790: particles, overlays), `XYZRHW|DIFFUSE|TEX1` (1274: UI).

### Render states that change per pass (counts per 33 k frames)

| State | Values seen | Notes |
|---|---|---|
| `CULLMODE` | `CW` 59 k, `NONE` 14 k, `CCW` 8 k | default CW; NONE for foliage/particles/two-sided; CCW for the stencil shadow back faces (paired with `STENCILPASS INCRSAT`/`DECRSAT`, 8184 each) |
| `ZBIAS` | 0 (34 k), 1 (28 k), 2 (3 k) | decals/overlay mesh and shadows |
| fog | `FOGENABLE` toggled, `FOGVERTEXMODE LINEAR`, `FOGTABLEMODE NONE`, `RANGEFOGENABLE TRUE`, `FOGSTART`/`FOGEND` pairs (`0x4461_0000`=900 / `0x4580_0000`=4096 and `0x4434_0000`=720 / `0x4538_0000`=2944), `FOGCOLOR` changes ~every frame (day cycle) | vertex range fog, colour from the scatter model |
| lighting | `LIGHTING` TRUE 12.7 k / FALSE 7.3 k; `AMBIENT` `0xC2C2C2`-ish and black; `SetLight`/`LightEnable` index 1 (75 k) and 2 (34 k); `GetLightEnable` polled | light 0 is the sun; 1–2 are scene point lights toggled per draw (the light-set delta of the reorder queue) |
| blending | `ALPHABLENDENABLE` TRUE 10.6 k / FALSE 8.2 k, `SRCALPHA`/`INVSRCALPHA` only; `ALPHATESTENABLE` TRUE 6.9 k with `ALPHAREF 128` | no additive blend state seen in this capture |
| depth | `ZENABLE` TRUE 15 k / FALSE 5 k, `ZWRITEENABLE` TRUE 12.5 k / FALSE 7.5 k | translucent pass writes no depth |
| stencil | `STENCILENABLE` TRUE 36.5 k / FALSE 34 k, `STENCILREF` 0/128, `STENCILFUNC ALWAYS` (3530) then `LESS` (1274), `STENCILPASS REPLACE`/`INCRSAT`/`DECRSAT`, `KEEP` otherwise, mask all | CPU stencil shadow volumes + a full-screen darkening quad with `STENCILFUNC LESS` |
| `TEXTUREFACTOR` | `0x65FFFFFF` (2548) | stage-1 `TFACTOR` modulate (6280) for the overlay fade |
| `SPECULARENABLE` | FALSE always | |

### Texture-stage programs

* Standard lit mesh: stage 0 `COLOROP SELECTARG1(TEXTURE)` or `MODULATE(TEXTURE, DIFFUSE)`, `ALPHAOP MODULATE/
  SELECTARG1`, stage 1 `DISABLE`.
* Terrain (4 UV sets, 30 726 draws): stage 0 texture, stage 1 `BLENDDIFFUSEALPHA(CURRENT, TEXTURE)` with
  `TEXCOORDINDEX 0`, stage 2 `MODULATE(TEXTURE, CURRENT)` with `TEXCOORDINDEX 1` (lightmap), stage 3
  `MODULATE(CURRENT, DIFFUSE)`; alpha `SELECTARG1(CURRENT)` through stages 1–3.
* Overlay/fade: stage 1 `MODULATE(TFACTOR, CURRENT)` colour and alpha.
* Particles/sky: stage 1 `ADDSMOOTH` (1274 — sky dome), `TEXTURETRANSFORMFLAGS COUNT2` on stage 0 (2942 —
  scrolling cloud UVs), vertex colour only (`COLOROP SELECTARG2(DIFFUSE)`, 135 k).
* Filters: stages 1–3 linear; stage 0 toggles point/linear (UI vs 3D) as in the menu.

The OpenGL device (`src/gl`) has to honour these states: the terrain 4-stage path, `TFACTOR`, `ADDSMOOTH`, UV scrolling, range
fog, the 2-influence skinning rule, the stencil shadow volumes and the planar `oT3` projection. See
[../render/D3D8_USAGE.md](../render/D3D8_USAGE.md) and [../render/EN3D_RENDER.md](../render/EN3D_RENDER.md).
