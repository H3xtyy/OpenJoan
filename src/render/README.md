# src/render

Native render features layered on the GL device ([`src/gl`](../gl/README.md)): everything the 2004 game did not have.
Each is controlled by `openjoan.ini [Graphics]` with a value that gives the original look (comments there are the
reference). No registrations.

| Files | What |
|---|---|
| `renderer.{hpp,cpp}`, `overrides.hpp` | the OpenGL execution core: program and vertex-array caches, samplers, fixed pipeline state, uniform blocks, draw submission; `gl::Device` and native draw paths share one GL state (call `invalidate_state()` after changing GL state behind its back) |
| `shadow_map.*`, `baked_shadows.*`, `shadow_volumes.*` | shadow maps, baked shadows, GPU shadow volumes |
| `ambient_occlusion.*`, `ao_pass.hpp` | ambient occlusion passes |
| `water.*` | water |
| `grass_batch.*` | grass batching |
| `pixel_lighting.*` | per-pixel lighting |
| `nvlink.*`, `nvlink_com.*`, `nvlink_log.*` | the replacement of the original's `SHADER\nvlinker.dll` (NVIDIA NVLink 2.0): `.nvo` fragment parsing, symbol binding, the temporary allocator, the `mov` optimiser, the `INVLink` COM object around it |
| `image.*` | D3DX8 texture loaders reproduced bit-exactly (BMP / DDS / TGA decode, texture planning, mip building; originals around 0x00765258) |
| `v3d.*` | native v3d vector maths used by the draw path |
| `image_test.cpp`, `nvlink_test.cpp` | stand-alone test programs; not in `src/CMakeLists.txt` (TODO(docs): how are they built and run?) |

Fixes in the renderer that deviate from the original are switched by `[Fixes]` (for example `Shaders`, `nvlink.hpp`).

Verification: a manual play test; the `nvlink` output is documented in `nvlink.hpp` as equal to
the DLL on the game's 1,810 fragment lists (its reference model `re/nvlink/nvlink.py` and the harness are not in this
repository: `re/nvlink/` holds `fixcheck.py` and `gen_n3.py` only).

See also: [`docs/render/`](../../docs/render/D3D8_USAGE.md).
