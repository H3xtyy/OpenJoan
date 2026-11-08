# src/gl

The Direct3D 8 device the game expects, implemented on OpenGL 4.6 core (`[Graphics] Renderer=opengl`). The game creates
its device through `Direct3DCreate8` (answered by `src/d3d8/direct3d_create8.cpp` -> `gl/direct3d.cpp`) and calls it
through the vtable slots declared in `src/d3d8`. Scope is what [`docs/render/D3D8_USAGE.md`](../../docs/render/D3D8_USAGE.md)
measured; anything else logs once and fails with `D3DERR_INVALIDCALL` (`device.hpp`).

| File | What |
|---|---|
| `glapi.{hpp,cpp}` | minimal OpenGL 4.6 core loader in namespace `joan::gl` (no system GL headers) |
| `context.{hpp,cpp}` | the GL 4.6 core context: SDL's, on the SDL game window (`platform/sdl_window.cpp`); a WGL path exists for a Win32 window (`[Platform] Window=win32`) |
| `direct3d.cpp` | `IDirect3D8`: adapter queries, device creation |
| `adapter.{hpp,cpp}` | native adapter answers (`NativeCaps=1`): display devices and modes, a fixed caps table, the adapter dump |
| `device.{hpp,cpp}` | `IDirect3DDevice8`: state, transforms, fixed function and vertex shader emulation, draw calls, Present (letterboxed scaling of an off-screen framebuffer the size of the back buffer) |
| `resources.cpp` | textures, surfaces, vertex and index buffers |
| `formats.hpp` | Direct3D 8 surface formats on OpenGL: internal format, upload format, swizzle, the memory layout the game sees when it locks a surface |
| `shaders.{hpp,cpp}` | GLSL generation for the Direct3D 8 pipeline |

Direct3D matrices are row-major and multiply row vectors (`device.cpp`); images are stored top row first
(`glClipControl` upper left). The GL execution core shared by the device and the native draw paths is
`src/render/renderer.*`; the modern options (shadows, ambient occlusion, water, ...) are in [`src/render`](../render/README.md).

No registrations. The device implements OS-independent GL only; windowing stays in `src/platform`.

Verification: a manual play test on a real display; the replay hashes the game
state, not the picture. Settings: `openjoan.ini [Graphics]` (comments there are the reference).

See also: [`src/d3d8/README.md`](../d3d8/README.md), [`docs/render/EN3D_RENDER.md`](../../docs/render/EN3D_RENDER.md).
