# tests

The unit tests and the data-image reference test. `ctest` in a build directory runs them.

| Test | Source | What it checks |
|---|---|---|
| `joan_tests` | `test_ilib.cpp`, `test_en3d.cpp`, `test.hpp` | the portable asset libraries in `tools/lib` |
| `data_image_reference_test` | `data_image_reference_test.cpp` (built from `src/CMakeLists.txt`) | every byte of the committed `.rdata` / `.data` against the reference `joan.exe`, and relocating the image at several bases. Needs `JOAN_GAME_DIR/joan.exe`; without it the test exits 77 and CTest reports `Skipped` (`src/data/README.md`) |

The game itself is verified by replay and manual play tests (`docs/CONTRIBUTING.md`).
