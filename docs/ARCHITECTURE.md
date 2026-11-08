# Architecture

State of 2026-11-08 (the code tree is the authority; line counts are approximate). Pages that go deeper:
`docs/RUNTIME.md`, `docs/X64.md` and `docs/render/` (the renderer).

## 1. What the executable is

OpenJoan is not an engine next to the game: it is *Wars and Warriors: Joan of Arc* rebuilt function by function in
C++, running as its own executable (`openjoan`; `joan_re` before Phase 1) on SDL3, OpenAL Soft, FFmpeg and
OpenGL 4.6. Nothing of the original executable's machine code exists at run time. Its static data (`.rdata`,
`.data`) is one C++ object of ours, committed as typed source. The game's data files are read from the
user's installation by the game's own loaders, ported like everything else.

Three ideas carry the whole design:

1. **Every original code address is a slot.** The original's call graph is preserved: a ported function calls
   its callees through the same addresses the original used, resolved by a table (`joan_code[]`) that leads each
   address to our replacement. A call to an address nobody has ported yet traps with "original code needed".
2. **The original's data layout is preserved.** Game objects keep their byte layout (`#pragma pack(1)` structs
   generated from `re/layout/layouts.json`), and the executable's globals keep their relative positions inside one
   relocatable image. Pointers inside that image are relocated at start-up; on x64 they are widened.
3. **Behaviour is bit-exact.** The x87 floating point of the 2004 build is emulated on SSE2; the CRT, D3DX and
   skinning libraries the game linked statically were ported too; nondeterministic inputs can be recorded and
   replayed with per-frame hashes of the game state.

## 2. Source tree

| Directory | Lines | What |
|---|---|---|
| `src/` (top level) | 56k | the headers every game file includes (`call.hpp`, `callconv.hpp`, `registry.hpp`, `x87.hpp`, `compiler.hpp`, `build_flavor.hpp`, `fn_names.hpp` generated 1.5 MB, `data_names.hpp` generated), display and canvas glue (`display.*`, `canvas_*`), `fixes.*`, `freecam.*`, `debug_menu.*`, `openjoan.ini` / `openjoan_release.ini`; the runtime machinery itself lives in `src/runtime/` |
| `src/runtime/` | | code table, registry, patching, host start/stop, data image runtime, crash handler, profiler, deterministic and guard heaps, save audit, import census (`src/runtime/README.md`); `loader/` and `replay/` below are its subdirectories |
| `src/game/` | 213k | the game proper (`d:\ww\*.cpp` of the original): files named after their original source file, and `unknown_<topic>.cpp` (196 files, formerly `_unplaced/`) holding the functions whose original file is not known yet (`re/attribution.csv`; `src/game/README.md`) |
| `src/interfac/`, `src/en3d/`, `src/en2d/`, `src/uti3d/`, `src/ilib/`, `src/elib/`, `src/enparticle/`, `src/enpath/`, `src/enphysic/` | 26k, 27k, 4k, 11k, 17k, 0.3k, | the original's engine libraries (interface screens, 3D engine, 2D widgets, utilities, resource/file/database library, JPEG glue, particles, path finding, physics), one file per original file where the original is known |
| `src/lib/` | 23k | libraries the original linked statically, ported: `crt/` (MSVC 6 C runtime), `d3dx8/` (math, texture loaders), `jpeg/` (IJG), `openal/` (ALut and the OpenAL calls on OpenAL Soft), `v3d/` (`v3d_87.dll` skinning math), `imports/` (Win32 imports implemented on the platform layer) |
| `src/fp/` | 2k | x87 emulation: 24- and 53-bit precision control, exact `sqrt`/`pow`/`ftol`, FMA |
| `src/original/` | 58k | typed layouts and method facades, generated (`gen/`, 2,739 files) plus hand-written layouts of the engine library (`file.hpp`, `dynarray.hpp`, `database.hpp`, `vecmath.hpp`, ..., `helpers.hpp`) and `bind.hpp` |
| `src/data/` | 74k | the data image: `layout.hpp` (91 chunk structs, 10,564 members), `rdata.inc` + `data.inc` (the values), `image.cpp` (the `constinit` object), `dead.cpp` (ranges left out), `data_relocs.inc`, `image_xlate.inc` (x64 map) |
| `src/runtime/loader/` | 1k | `joan_re.cpp` (`main`: data image, stray guard, imports, replacements, CRT entry), `data_image.cpp` (relocation, section structure), `import_overrides.*`, `dead_imports.inc`, `joan_re.rc` (version info) |
| `src/platform/` | 8k | SDL3 window, input, time; files (case-insensitive game folder, files-in-memory), ini, memory (page protection, fault handlers), process, text tables, `win32_types.hpp` (the Win32 type vocabulary the ported code uses) |
| `src/d3d8/`, `src/gl/` | 0.5k, 7k | the Direct3D 8 interface the game expects (`direct3d_create8.cpp` answers Direct3DCreate8) and its implementation on OpenGL 4.6 (`device.cpp`, `context.cpp`, `adapter.cpp`, formats, shaders) |
| `src/render/` | 9k | native render features layered on the GL device: shadow maps and GPU shadow volumes, ambient occlusion, water, grass batching, pixel lighting, the `.nvo` shader linker replacement (`nvlink*`) |
| `src/media/` | 1k | FFmpeg: movie and music decoding behind the Windows Media reader interface the game calls |
| `src/runtime/replay/` | 10k | record/replay at the import boundary, typed image hash, deterministic heap integration (on every platform, through the SDL layer) |
| `src/save/` | 12k | save-file I/O and the generated field-wise records (`gen/records_s*.hpp`) for the x64 port |
| `tools/`, `tests/`, `tools/lib/` | | asset tools (`jres`, `jdbf`, `jmodel`, `jview`), the asset lint, the hazard meter, `objcmp.sh`; unit tests; the portable asset libraries |
| `re/` | | the reverse-engineering databases (`functions.csv` 11,332 rows, `attribution.csv`, `classes.csv`, `layout/layouts.json`) and the generators (`gen_*.py`, `rewrite_*.py`, `layout/`, `facades/`, `image/`, `save/`); `history/` holds the round briefs |

Build targets (`src/CMakeLists.txt`): `openjoan_game` (OBJECT library: everything above except the loader) and
`openjoan` (`runtime/loader/joan_re.cpp`, `runtime/loader/data_image.cpp`, `runtime/image_runtime.cpp`, `data/image.cpp`, `data/dead.cpp`,
the resource script on Windows), output name `openjoan`. Dependencies: SDL3 (`cmake/sdl3.cmake`), OpenAL Soft
(`cmake/openal_soft.cmake`), FFmpeg (`cmake/ffmpeg.cmake`).

## 3. Start-up

`runtime/loader/joan_re.cpp` `main` -> `run()`:

1. **Data image first.** Before any other static initialiser (`init_priority(101)` on GCC/Clang, `init_seg` on
   MSVC) `runtime/image_runtime.cpp` / `runtime/loader/data_image.cpp` relocate the stored data pointers of `joan_image` to the
   object's real address and check that every dead range lies inside its section (`data/dead.cpp kSections`; the
   start-up CRC check of the typed members against the reference build went with the extraction path in Phase 1:
   a checksum of our own committed source proves nothing). The loader log shows: image base, both sections with
   their typed and dead bytes, "770 data / 1568 code" relocations.
2. **Stray guard.** A fault at one of the original's old addresses (0x00400000..0x00a96000) is logged with its
   symbol and ends the game; nothing is reserved there (the executable is relocatable, ASLR-compatible: on Linux
   x86-64 a PIE, on Windows x64 large-address-aware with high-entropy ASLR, so the code, the data image and the
   heaps lie above 4 GB; only the Linux x86 reference build is linked `-no-pie`). While recording or replaying, the
   game's CRT heap is the deterministic heap at a fixed base (`det_heap.cpp`; x64: 0x7e00'2000'0000 and up).
3. **Imports.** `src/runtime/import_table.inc` names every import of the original (12 DLLs, 254 functions). None of those
   DLLs is loaded: 199 overrides from `runtime/loader/import_overrides.hpp` substitute our implementations (`lib/imports`,
   `platform/`, `v3d_87.dll` entirely, OpenAL, DirectInput, Direct3D 8), the rest are dead imports that trap
   (`runtime/loader/dead_imports.inc`).
4. **Replacements.** `host::start` (`runtime/host.cpp`) loads `openjoan.ini`, then `code_table::begin()`, installs every
   `JOAN_REPLACE` registration (`registry.hpp`; 9,772 registrations at the 2026-11-08 review), the `[Fixes]` (`fixes.cpp`), the
   display and canvas hooks, the Direct3D 8 device hook (`d3d8/direct3d_create8.cpp` -> `gl/`), the replay wrappers, and
   `code_table::seal()`: every slot without a registration traps. The code pointers stored in the data image (vtables,
   CRT initialisers, callbacks) are pointed at their slots' targets and `.rdata` is made read-only.
5. **The original's entry.** `fn::WinMainCRTStartup` (0x007b71e6, our port in `lib/crt/startup.cpp`) is called
   through its slot; from there the game runs exactly as the original's call graph says: CRT initialisation,
   `WinMain`, the game loop.

At exit `code_table::write_report()` writes the counters, the traps that fired and the stray addresses seen into
`openjoan.log` (debug build).

## 4. Code table and calls

- `fn::Code` (`code.hpp`) is a scoped enum: the index of a slot in `joan_code[]` (`code_table.cpp`,
  `code_slots.inc`, generated by `re/gen_code_slots.py`). `fn::Unit_cleanup` and the 9,655 other `fn::` constants live
  in `fn_names.hpp` (generated by `re/gen_fn_names.py` from `re/functions.csv`); each carries the original address.
- `fn::target(code)` is what a call reaches (the installed replacement or the trap stub for that address);
  `fn::code_value(code)` the same as a number, for storing into game objects or vtables; `fn::va(code)` the
  original address for logs. Computed addresses read from game data go through `fn::target(raw)`, which checks
  values in the old code range out of line.
- Calls between ported functions use the helpers of `call.hpp`: `thiscall<R>(target, self, args...)`,
  `cdecl_call<R>(...)`, `stdcall_call<R>(...)`, `import_call<R>(d::iat_x, ...)`. On 32-bit MSVC the four x86
  conventions are distinct; on Linux and on x64 they collapse into one. Module-local variants (`com_call`, `vcall`)
  exist in the batch files; they read the vtable through `joan::call::vtable_of(object)` and turn the original's
  byte offset into a slot with `x86_vslot(offset)` (entries are pointer-sized). Two named narrowings exist for the
  cases where 32 bits of a pointer are right: `register_low_bits(p)` (a result that was only a register left at an
  address, which no caller uses beyond its low bits) and `pointer_low_dword(p)` (an emulated x86 frame dword, or a
  4-byte image slot only the original's assembly read). Any other pointer is kept pointer-sized.
- A replacement registers itself next to its definition:

  ```cpp
  // ---- 0x00461f10 Unit::cleanup ----
  H* cleanup() { ... }
  JOAN_REPLACE("Unit::cleanup", "game/unit.cpp", fn::Unit_cleanup, &H::cleanup);
  ```

  `name` and `module` are the stable IDs of the logs and of `openjoan.ini [Hooks] Enable/Disable`.
  `JOAN_REPLACE_YIELDING` (left out while another module replaces the same address) and `JOAN_REPLACE_IMPORT`
  (an IAT slot) are the two variants; the former `JOAN_REPLACE_OPT_IN` was merged into `JOAN_REPLACE` in Phase 3.
  `redirect_function` fills every thunk slot that leads to the original, `patch_entry` serves functions
  without a thunk, `patch_import` IAT slots.
- A trap stub reached ends the game with `original code needed: <address> <name>` in `openjoan.log`. The 32-bit
  reference build fires none during the smoke checklist; `code_table::write_report()` lists any that did.

## 5. Data image and typed layouts

- `joan_image` (`image.hpp`) is `.rdata | .data | .bss | .idata` of the original, original VA
  0x009cf000..0x00a70000 (0xa1000 bytes on x86, 0xa8a6c on x64 with 7,835 pointer slots widened). Its typed layout
  is `data/layout.hpp`: 91 chunk structs, every object a named member at its original offset with a comment
  (address, kind, which modules read it). The values are the committed source `data/rdata.inc` + `data/data.inc`,
  included by `data/image.cpp` into one `constinit` object; stored pointers are `JOAN_DATA_PTR(addr)` /
  `JOAN_CODE_PTR(addr)` and are relocated at start-up (`data_relocs.inc`). 55,832 bytes of dead objects (exception
  tables, data only replaced library code read) are left out (`data/dead.cpp`); `[Loader] PoisonDead=1` fills
  them with 0xCD to prove nothing reads them.
- Names: `d::g_units`, `d::s_some_string` (`data_names.hpp`, generated by `re/gen_data_names.py`) are
  `img::Sym<T>` / `img::Addr`. Access is explicit: `img::obj(d::x)` (pointer), `hook::global<T>(d::x)` (typed
  reference), `img::str(d::s_...)`, `img::offset_of(va)`. An address used as a number must say so.
- On x64 the layout is the compiler's 64-bit layout of `layout.hpp`; `image_xlate.inc` (generated by
  `re/image/gen_xlate.py`) maps an original address to its offset in the widened layout. A 4-byte slot stays
  `u32`; a widened slot is a pointer and must be read as one.
- Game classes: `original/gen/<class>.hpp` wraps `<class>_fields.inc` (members at original offsets,
  `#pragma pack(1)`, `static_assert(offsetof)` on x86) and `<class>_methods.inc` (facades that call the original's
  method through its slot). Source of truth: `re/layout/layouts.json` + `re/layout/spec/`, regenerated by
  `re/layout/gen_types.py` and `re/facades/gen2.py`. **Never hand-edit `gen/`.** `jo::self<jo::Unit>(this)` casts
  the `this` of a hand-written replacement to the typed layout; `joan::orig::arg::*` adapters keep the x86 calling
  sequence explicit. Raw offset access (`at<T>(p, 0x..)`) is what Phase 2 removes; the hazard meter
  (`tools/x64_hazards.py`) counts what is left (`raw_ptr`, `raw_u32`, `ptr_u32` and `vtbl4` are at zero since
  2026-11-06). `src/save/gen` is generated by `re/save/gen_save.py`, which needs the llvm-mingw sysroot
  (`JOAN_MINGW`, default `/opt/llvm-mingw`) for its libclang pass; where that is missing a layout change is carried
  into the save records by hand in the generator's form (the Missile record of 2026-11-06).

## 6. Libraries, floating point, platform

- **CRT (`lib/crt`).** The MSVC 6 runtime the game linked: heap (`heap.cpp`, with the deterministic and guard
  heaps behind it), stdio and lowio on `platform/files`, `printf` family, locale, time, multithreading state
  (`mt.cpp` `_tiddata`), `startup.cpp`, `exit.cpp`, x87 math (`x87_math.cpp`).
- **Floating point (`fp/`, `x87.hpp`).** The original ran on the x87 FPU with its own `_ftol`, `sqrt`, `pow`
  sequences at 24- or 53-bit precision. The port reproduces the exact results on SSE2 (`x87::Fpu`, `x87::ftol`,
  `fp/pc24.hpp`, `pc53.hpp`, `exact.hpp`, `fma.hpp`). Compiler flags are part of the behaviour: GCC/Clang
  `-fno-strict-aliasing -fwrapv -ffp-contract=off`, x86 `-msse2 -mfpmath=sse`; MSVC `/arch:SSE2 /fp:precise`.
  Never reorder float expressions in ported code; never `-ffast-math`.
- **D3DX8, JPEG, v3d (`lib/d3dx8`, `lib/jpeg`, `lib/v3d`).** Math, texture loaders, IJG decoding, the skinning
  math of `v3d_87.dll`, ported bit-exactly.
- **Platform (`platform/`).** SDL3 for the window, events, keyboard, mouse, timers; the game folder is read with
  case-insensitive names and, with `[Platform] FilesInMemory`, preloaded; `ini.cpp` reads `openjoan.ini`;
  `memory.cpp` provides page protection and the fault handlers the stray guard and crash handler use;
  `win32_types.hpp` is the Win32 vocabulary (`DWORD`, `HRESULT`, `RECT`, ...) the ported code keeps using.
  Anything that calls the OS directly belongs here or in the loader.
- **Graphics (`d3d8/`, `gl/`, `render/`).** The game creates a Direct3D 8 device; `d3d8/direct3d_create8.cpp` hands it
  our implementation on OpenGL 4.6 core (`gl/device.cpp` and friends: state, vertex/index buffers, textures, fixed
  function and vertex shader emulation, the swap chain through SDL). `render/` adds the modern options of
  `openjoan.ini [Graphics]` (shadows, ambient occlusion, water, grass, pixel lighting, anisotropy) and replaces the
  original's `nvlinker.dll` shader linker. `docs/render/D3D8_USAGE.md` is the measured inventory of what the game
  uses; `docs/render/EN3D_RENDER.md` how a frame is drawn.
- **Audio and media.** The game's OpenAL 1.0 calls go to OpenAL Soft (`lib/openal`); `.wmv` movies and `.wma`
  music are decoded by FFmpeg behind the Windows Media reader interface (`media/`, `uti3d/video.cpp`).

## 7. Configuration and the two flavours

`src/openjoan.ini` (debug build) and `src/openjoan_release.ini` (player build) are the reference for every option;
their comments are the best documentation of the runtime and `docs/RUNTIME.md` (Phase 4) is written from them.
Sections: `[Loader]`, `[Hooks]` (per-registration enable/disable, logging), `[Graphics]` (35 keys: the modern
render options, each with a value that gives the original look), `[Display]`, `[Audio]`, `[Video]`, `[Platform]`,
`[Fixes]` (15 deliberate deviations from the original, such as `MouseTurn`, default on), `[Game]`, `[Debug]` (31
keys: record/replay, heap guard, profiler, hitch log, save audit, free camera, import census).

`build_flavor.hpp` and `JOAN_RELEASE=ON` compile the debug tools, counters and `[Hooks] Log` output out of the
player build, which reads only `openjoan_release.ini`.

## 8. Verification

- `tools/objcmp.sh`: a refactor or a type fix is behaviour-free when the 32-bit objects of the changed files are
  byte-identical (debug information stripped) before and after.
- `tools/x64_hazards.py --baseline re/x64_census/hazards_baseline.txt`: the count of 32-bit assumptions per
  directory never rises.
- `tools/check_no_assets.py`: no file of the repository contains a run of a game file.
- `src/runtime/replay/`: a recording of every nondeterministic input plus per-frame hashes of the game state; a replay
  must be identical (the Linux recorder exists since Phase 1).

## 9. What changes in the migration

Phase 1 (done 2026-10-29 for these two items) made the replay recorder run on Linux and deleted the extraction path
(`tools/joan_extract`, `JOAN_DATA_FROM_EXE`, the section CRC check); Phase 2 removes every 32-bit assumption (typed layouts for the raw
reads, field-wise saves, `void**` vtables, no address-space crutches) until the x86 reference recording replays on
x64; Phase 3 reorganised the tree into the current layout (`runtime/`, `game/` with no `_unplaced/`
directory); Phase 4 completes the documentation and ships releases.

**Phase 3 exit state of the names (2026-11-08).** The renaming did not
reach every generated placeholder; the owner accepted stopping at this long tail and it stays open. Remaining:

| Placeholder | Remaining | Of |
|---|---|---|
| `d::` names of the form `g_<hex>` | 1,448 | 5,753 |
| generated layout fields of the form `<type>_<hex>` | 670 | 4,876 |
| `fn::` names with an address suffix | 3,921 | 9,655 |
| batch namespaces `game_bNNN_NN` | 334 files | |

Names that exist are evidence-based (718 registrations, 325 `d::` globals and 449 layout fields were renamed in
Phase 3 from `re/names/*.csv`); the rest of the long tail keeps its address in the name, which is also the stable
ID in logs and docs. Also placed by reason rather than by original file: 196 `src/game/unknown_*.cpp` files
(`src/game/README.md`).
