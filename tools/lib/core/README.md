# tools/lib/core (`joan_core`)

Foundation of the asset libraries in `tools/lib/`; static library, no dependency beyond the standard library. Include
path: `tools/lib` (`#include "core/vfs.hpp"`).

| File | Provides |
|---|---|
| `vfs.hpp` / `vfs.cpp` | `joan::Vfs`: mounts a game installation folder and resolves DOS-style paths (`Resource\STD.SET`, any case, either slash) to real paths, also on case-sensitive file systems; `resolve`, `exists` |
| `file_util.hpp` / `file_util.cpp` | `read_file` (whole file into a byte vector) and `write_file` (creates parent folders); both throw `std::runtime_error` on failure |
| `log.hpp` / `log.cpp` | `joan::log`: `debug/info/warn/error` with `std::format` strings and a minimum level (`set_min_level`); replaces the original engine's error reporter for these libraries |
| `binary_reader.hpp` | `joan::BinaryReader`: bounds-checked little-endian reader over a byte span; malformed data throws `joan::FormatError` instead of reading out of bounds. Header only |

Built by the root `CMakeLists.txt`; linked by `ilib`, `en3d`, `platform` and `render`.
