# tools/lib/en3d (`joan_en3d`)

Portable 3D asset code for the Enlight 3D engine's files, with no GPU dependency. Static library on `joan_core`; the
in-game equivalents live in `src/en3d/`. Formats: `docs/formats/`.

| File | Provides |
|---|---|
| `model.hpp` / `.cpp` | `.AM1` skinned meshes, `.SMO` static meshes, `.AM3` bone hierarchies (only file version 500, the one every shipped file has). Original loaders 0x5da280 (AM1) and 0x5ddd70 (SMO) |
| `animation.hpp` / `.cpp` | `.AM2` skeletal animation: an action table of named time ranges and sparse per-bone key tracks holding the bone's model-space transform (original 0x5b4170-0x5b9710) |
| `dds.hpp` / `.cpp` | DirectDraw Surface loader decoding the top mip level to RGBA8 on the CPU (DXT1/3/5 and uncompressed) |
| `asset_paths.hpp` / `.cpp` | finds the files that belong together: a mesh's textures (named `.tga`/`.bmp`, shipped as `.dds` under `texture/`) and a unit's animation in `anim/` |
| `math.hpp` | small vector / matrix helpers with the data's conventions (right-handed, Z up, bone transform as a row-major 3x4 `[R | t]`) |

Used by `jmodel`, `jview`, `joan_render` and `tests/test_en3d.cpp`.
