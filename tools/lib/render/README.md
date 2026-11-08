# tools/lib/render (`joan_render`)

OpenGL 3.3 code for the asset viewer and the bootstrap executable. Static library on `joan_core` and `joan_en3d`; built
only with SDL3 (see `tools/lib/README.md`).

| File | Provides |
|---|---|
| `gl.hpp` / `gl.cpp` | a minimal OpenGL function loader (no third-party loader); the entry points are listed in `JOAN_GL_FUNCTIONS`, add more there as needed |
| `model_renderer.hpp` / `.cpp` | draws `joan_en3d` models; skinning is done on the CPU (unit meshes have about 2,000 vertices) |

Not the game's renderer: that is `src/gl/` (the Direct3D 8 device on OpenGL 4.6) and `src/render/`.
