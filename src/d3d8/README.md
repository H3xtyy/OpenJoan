# src/d3d8

The Direct3D 8 interface the game expects, as declarations only. The Windows SDK no longer ships `d3d8.h`, and the
OpenGL renderer (`src/gl`) has to implement these interfaces itself.

| File | What |
|---|---|
| `d3d8_api.hpp` | interface declarations: `IDirect3D8`, `IDirect3DDevice8`, ... (vtable order is the original's: the game calls through vtable slots) |
| `d3d8_types.hpp` | structures, constants and the resource interfaces (textures, surfaces, vertex and index buffers) needed to implement a device |
| `direct3d_create8.cpp` | the game's `Direct3DCreate8` import (`d3d8.dll`) is answered by `gl/direct3d.cpp`: the loader puts it into the import slot, so `d3d8.dll` is never loaded (20 lines; the old tracing proxy is gone) |

No game code and no registrations live here. Verified by the build and by a manual play test
.

See also: [`src/gl/README.md`](../gl/README.md) (the implementation),
[`docs/render/D3D8_USAGE.md`](../../docs/render/D3D8_USAGE.md) (what the game uses of Direct3D 8),
[`docs/ARCHITECTURE.md`](../../docs/ARCHITECTURE.md) section 6.
