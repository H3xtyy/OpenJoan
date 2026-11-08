# What the game uses of Direct3D 8

Measured with the hook's D3D8 trace (`[Graphics] Trace=1`) on 2026-10-05: a
2.5-minute session (menus, a battle, save + load), 45,623 frames at
1920x1080 windowed. This is the specification for the OpenGL 4.6 device
(ROADMAP M3, stage 1). Shader byte code and disassembly: `re/shaders/`
(`re/vsdis.py` disassembles vs.1.x tokens).

## Device

- `CreateDevice(adapter 0, HAL, behavior 0x40 = HARDWARE_VERTEXPROCESSING)`,
  windowed, back buffer X8R8G8B8 x1, swap effect requested as FLIP (2) and
  reported as DISCARD (1) after creation, auto depth/stencil **D24S8**, no
  multisampling. vs.1.1 limits apply: 96 constants (the shaders use c0-c95).
- Per frame: `TestCooperativeLevel`, `BeginScene`/`EndScene` (2 scenes per
  frame), `SetViewport`, `Present` (no rects), `Clear` with target+Z+stencil.
- Hardware cursor: `SetCursorProperties` (32x32 A8R8G8B8 image surface),
  `SetCursorPosition`, `ShowCursor` every frame. Gamma ramp set once.
- Statically linked D3DX calls `GetDirect3D` + `GetDeviceCaps` +
  `GetDisplayMode` + `CheckDeviceFormat` around every texture it creates.
- Format support the game asks for (adapter format X8R8G8B8, textures):
  R8G8B8 no, A8R8G8B8, X8R8G8B8, R5G6B5, X1R5G5B5, A1R5G5B5, A4R4G4B4 yes,
  R3G3B2/A8R3G3B2/X4R4G4B4/A8P8/P8 no, A8, L8, A8L8, A4L4, V8U8 yes.

## Methods (49 of 97 device methods never called)

Used: TestCooperativeLevel, GetAvailableTextureMem, GetDirect3D,
GetDeviceCaps, GetDisplayMode, GetCreationParameters, SetCursorProperties,
SetCursorPosition, ShowCursor, Present, GetBackBuffer, Set/GetGammaRamp,
CreateTexture, CreateVertexBuffer, CreateIndexBuffer, CreateImageSurface,
CopyRects, Set/GetRenderTarget, GetDepthStencilSurface, Begin/EndScene,
Clear, Set/GetTransform, SetViewport, Set/GetMaterial, Set/GetLight,
LightEnable, GetLightEnable, Set/GetRenderState, SetTexture,
SetTextureStageState, DrawPrimitive, DrawIndexedPrimitive,
Create/Set/DeleteVertexShader, SetVertexShaderConstant, SetStreamSource,
SetIndices, SetPixelShader (always 0).

Not used: cube / volume textures, state blocks, clip planes, palettes,
UP draws, ProcessVertices, pixel shaders, patches, swap chains, Reset.

Volume (45k frames): 8.56M DrawIndexedPrimitive (2.44 billion triangles,
~53k per frame), 60.6M SetRenderState, 41.4M SetTextureStageState, 33.5M
SetTexture, 9.2M SetIndices, 9.1M SetTransform. Get* calls (state read-back)
are frequent: GetLightEnable 8.8M, GetRenderState 7.2M, GetLight 651k.

## Resources

| Kind | Formats | Notes |
|---|---|---|
| textures, managed | DXT1 (863, mips), DXT3, DXT5 (up to 1024²), X8R8G8B8 (1457, mips), A8R8G8B8, A4R4G4B4 (80, up to 1024²), R5G6B5 (800x450 movies/menus), V8U8 (bump) | locked by D3DX / the game |
| render target texture | X8R8G8B8 512x256, usage RENDERTARGET | `SetRenderTarget` x4, `CopyRects` x2 (+ 512x256 image surfaces) |
| vertex buffers | static (usage WRITEONLY, ~880) and dynamic (WRITEONLY+DYNAMIC, 9) | FVFs below; dynamic ones up to 288 KB |
| index buffers | INDEX16, static (887) and one dynamic | |

## Vertex formats (draw calls)

| FVF | Use | Draws |
|---|---|---|
| XYZ\|DIFFUSE\|TEX1 (0x142) | effects, sprites in 3D | 1.25M |
| XYZRHW\|DIFFUSE\|TEX1 (0x144) | 2D / UI (pre-transformed) | 19.6k |
| XYZ\|NORMAL\|DIFFUSE\|TEX1 (0x152) | static models | 1.97M |
| XYZB1\|NORMAL\|DIFFUSE\|TEX1 (0x156) | 2-matrix vertex blending (VERTEXBLEND=1, WORLD/WORLD1) | 3.09M |
| XYZ\|NORMAL\|DIFFUSE\|TEX4 (0x452) | terrain (4 texture stages) | 1.93M |
| XYZ\|PSIZE\|DIFFUSE | point sprites buffer (created, 140 points drawn) | |
| vertex shaders vs1-vs4 | see below | 323k |

Primitive types: TRIANGLELIST (indexed) almost exclusively; TRIANGLESTRIP
(19.7k DrawPrimitive, UI quads), a few POINTLIST / LINELIST.

## Fixed-function state

- Transforms: VIEW (2), PROJECTION (3), WORLD (256), WORLD1 (257),
  TEXTURE0 (16), TEXTURE1 (17).
- Lighting: up to 8 lights, point (2.8M SetLight) and directional; materials;
  AMBIENT; SPECULARENABLE; LOCALVIEWER; COLORVERTEX default.
- Fog: vertex fog, LINEAR (FOGVERTEXMODE 3), RANGEFOGENABLE, FOGSTART 900,
  FOGEND 4096 / 4596, FOGCOLOR varies (sky colour).
- Depth: ZENABLE, ZWRITEENABLE, ZBIAS 0..2. Culling: none/CW/CCW.
- Alpha: ALPHATESTENABLE (ALPHAREF 4 / 128, ALPHAFUNC GREATEREQUAL),
  ALPHABLENDENABLE with SRCALPHA / INVSRCALPHA or ONE.
- Stencil (shadows): STENCILENABLE, func ALWAYS / LESS, ops KEEP / REPLACE /
  INCRSAT / DECRSAT, ref 0 / 1 / 0x80; COLORWRITEENABLE 0 for stencil passes.
- TEXTUREFACTOR (fading alpha), CLIPPLANEENABLE always 0.

Texture stages (up to 4 with textures, 5 configured):

- ops: DISABLE, SELECTARG1/2, MODULATE, MODULATE2X, ADDSMOOTH,
  BLENDDIFFUSEALPHA, BUMPENVMAP (stage 0, V8U8, water); args DIFFUSE,
  CURRENT, TEXTURE, TFACTOR, SPECULAR.
- TEXCOORDINDEX 0..3 and CAMERASPACEREFLECTIONVECTOR (environment map on
  stage 1), TEXTURETRANSFORMFLAGS COUNT2 / COUNT3.
- Addressing WRAP / CLAMP; filters point / linear, mip linear.

## Vertex shaders (vs.1.x)

| Shader | Purpose | Inputs | Constants |
|---|---|---|---|
| vs1 (vs.1.0) | sky / atmosphere: per-vertex scattering, extinction `exp(-beta*d)`, Henyey-Greenstein phase, sun colour; texture coordinate transform | v0 pos, v3 normal, v5 colour, v7 uv | c0 consts, c1-4 world-view-proj, c5-7 world, c10 eye, c13-15 view, c23-33 scattering |
| vs2 | 2-bone skinning, 1 directional light, fog, shadow coordinate oT3.x | v0/v1 bone-space positions, v8/v9 normals, v12 bone indices (short2, x3), v13 weights, v5 colour, v7 uv | c1-4 WVP, c5-7 world, c9/c11 ambient, c17-c88 up to 24 bone 4x3 matrices, c89-92 light / fog |
| vs3 = vs4 (byte-identical, created twice) | as vs2 plus one point light with attenuation (`dst`) | same | c89-95 |

All three translate directly to GLSL (`a0`-relative constant reads become
uniform-array indexing).
