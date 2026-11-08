# tools/lib: portable asset libraries

Small, game-independent C++20 libraries that read the original game's data files without any of the game code
(`src/`). They serve the asset tools (`jres`, `jdbf`, `jmodel`, `jview`
in `tools/`), the unit tests (`tests/test_ilib.cpp`, `tests/test_en3d.cpp`) and a bootstrap executable. They are **not**
part of the game build: the game executable (`openjoan`) does not link them. All of them use the namespace `joan`.

| Directory | CMake target | Needs | What it holds |
|---|---|---|---|
| `core/` | `joan_core` (static) | nothing | file reading, case-insensitive path resolution, logging, bounds-checked binary reader |
| `ilib/` | `joan_ilib` | `joan_core` | resource archives, dBase tables, game sets, `Key : value` text files |
| `en3d/` | `joan_en3d` | `joan_core` | `.AM1`/`.SMO`/`.AM3` meshes, `.AM2` animation, DDS decoding, asset path lookup, vector maths |
| `platform/` | `joan_platform` | `joan_core`, SDL3 | one SDL3 window with an OpenGL context |
| `render/` | `joan_render` | `joan_core`, `joan_en3d` | a small OpenGL 3.3 function loader and a model renderer |
| `app/` | `joan` (executable) | `joan_ilib`, `joan_platform`, `joan_render` | bootstrap entry point (see its README) |

## Building

The root `CMakeLists.txt` adds `core`, `ilib` and `en3d` always (no third-party dependency). `platform`, `render` and
`app` are added when `JOAN_BUILD_ENGINE` is on and CMake finds SDL3 (`find_package(SDL3 CONFIG)`); without SDL3 a
warning is printed and only the portable libraries, the tools that need no window and the tests are built. The tools are
controlled by `JOAN_BUILD_TOOLS` (default ON). A clone does not need the game to build any of this; running the tools
and the tests that read data needs `JOAN_GAME_DIR`.

Formats are specified in `docs/formats/`; each header cites the original source file and address it mirrors.
