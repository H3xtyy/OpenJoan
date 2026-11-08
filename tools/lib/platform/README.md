# tools/lib/platform (`joan_platform`)

`window.hpp` / `window.cpp`: an SDL3 window with an OpenGL context for the asset viewer and the bootstrap
executable (resizable, high-DPI aware, borderless full screen). It replaces, for these tools only, the original's
Win32 window and Direct3D 8 device set-up. Static library on `joan_core` and `SDL3::SDL3`.

Not the game's platform layer: that is `src/platform/` (SDL3 window, input, time, files, ini for `openjoan`). Built only
when `JOAN_BUILD_ENGINE` is on and SDL3 is found (see `tools/lib/README.md`).
