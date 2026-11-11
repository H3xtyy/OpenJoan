# tools/

Maintenance scripts and the asset tools of the repository: `jres.cpp`, `jdbf.cpp`, `jmodel.cpp`, `jview.cpp` and `CMakeLists.txt`
build the asset viewers (`tools/lib/`). The build-time data extractor was deleted in Phase 1.

| File | Purpose | Status |
|---|---|---|
| `x64_hazards.py` | counts 32-bit assumptions per directory/file (`--by-file`, `--baseline`); reads `../src` relative to itself | run from the repository root |
| `x64_scan_shells.py` | x86-sized byte shells used as typed objects, against the x64 / x86 sizes of the classes (`<x64 binary> <x86 binary>`) | run from the repository root |
| `x64_scan_byvalue.py` | by-value struct arguments whose float / integer member kinds differ between caller and replacement (XMM vs integer registers on x64) | run from the repository root |
| `x64_scan_abi_kinds.py` | facade vs replacement parameter kinds (float on one side, integer on the other); the KIND rows are the finds | run from the repository root |
| `x64_scan_ptr_as_u32.py` | pointers narrowed to 32 bits at a call site (`arg::as<u32>(p)`, `arg::u32(p)`, `static_cast<u32>(reinterpret_cast<uintptr_t>(p))` on a variable declared with a pointer type): fine on x86 and in a replay (deterministic heap below 4 GB), a truncation in the live x64 game. Advisory: the declaration lookup is by name and can pick a shadowed one |
| `x64_scan_facade_returns.py` | generated facades declared with a 32-bit result for an address whose replacement returns a pointer: the facade cuts it (Cell::get_9c, found when the deterministic heap moved above 4 GB); fix with a `"ret"` entry in `re/facades/retype_phase2.json` | run from the repository root |
| `x64_scan_u32_to_ptr.py` | call sites that turn a facade's 32-bit result back into a pointer (`arg::as<T*>(as_x(p)->m(...))`); the `as<std::uintptr_t>` hits are mostly register residue passed on | run from the repository root |
| `x64_scan_view_classes.py` (+ `x64_view_classes/`) | advisory: one object read through two class layouts that agree on x86 and not on x64 - per function, through `as_x(E)->method()` into the callee, across global objects, and views of `&owner->field`; layouts of all classes from both compilers (needs `build/linux-x86` and `build/linux-x64` configured, gdb). `x64_view_classes/q.py OUT Class [field\|0xOFF]` prints a class's x86 / x64 offsets | run from the repository root; output in `build/x64-view-scan/` |
| `x64_scan_two_layouts.py` | advisory: `this` viewed through a layout other than the facade's class (many false positives, the view-class sweep's starting point) | run from the repository root |
| `replay_image_diff.py` | diffs two `ReplayDumpImage` dumps (an x86 and an x64 replay of the same recording): per frame, the words that differ, value-like words first, the typed table's pointer and untyped words and the layout-dependent words of `re/replay/x64_layout_words.txt` (or `--mask FILE` / `--mask none`) left out; streams the dumps (docs/X64_STATE_CHECK.md) | run from the repository root (reads `src/runtime/replay/hash_types.inc` and `src/data/data_symbols.inc`) |
| `x64_scan_raw_index.py` | raw hex offsets applied to byte pointers (`p[0x124]`, `p + 0x50`): the form the hazard meter does not count; a hit is a hazard when the pointer is a game object, noise when it is a file / record buffer | run from the repository root; found hud.cpp, plane.cpp, esfuncs.cpp and unit_core.cpp reads on 2026-10-30 |
| `x64_scan_out_params.py` | 4-byte integer locals passed by address as output slots to replacements that write a pointer through them (`void**`, `u16**`): the 8-byte store clobbers the neighbour on x64 (Actor::select_mesh's callers, 2026-10-30) | run from the repository root |
| `x64_scan_raw_calls.py` | raw slot calls (`thiscall<R>(fn::X, ...)`, `cdecl_call`, `stdcall_call`) whose float / integer argument kinds disagree with the replacement's parameters (XMM vs integer registers on x64); the facade path is `x64_scan_abi_kinds.py` | run from the repository root; found the formmove.cpp slot search of 2026-10-30 |
| `check_no_assets.py` | the asset lint: fails if any tracked or untracked-unignored file contains a run of bytes of the game's data files or of `joan.exe`'s `.rsrc` (raw or as the byte streams its literals spell); `--game` defaults to `JOAN_GAME_DIR`; prints SKIP and exits 0 without a game folder (`--require-game` to fail instead); allow list `check_no_assets.allow` (absent = empty); needs numpy; about three minutes for the 1.1 GB install | works; run before every push; CMake target in Phase 1 |
| `build_ffmpeg_linux.sh` | the minimal FFmpeg shipped in the Linux release archive (`-DJOAN_FFMPEG_ROOT`) |
| `build_linux_release.sh` | the Linux release (archive and AppImage), run inside the manylinux_2_28 container; `docs/RELEASING.md` |
| `make_icon.py` | draws OpenJoan's own window icon with Pillow and writes `src/runtime/loader/openjoan.ico` (16/32/48 px, BMP-encoded) and `src/platform/openjoan_icon.inc` (32x32 ARGB words for the SDL window) | works; re-run after changing the artwork |
| `pe_inventory.py` | PE triage (sections, entropy, imports, exports, source paths) to Markdown | generic |
| `scan_formats.py` | census of a game install by extension (counts, sizes, magic) to Markdown | generic; used to regenerate `docs/formats/INVENTORY.md` |
| `docs_lint.py` | checks module READMEs, format pages, links | checks `src/<dir>/README.md` presence, `JOAN_REPLACE` tables and links; run before every docs change |
| `sce_builtins.py` | extracts the builtin table from the binary's strings | reference for `docs/scripting/BUILTINS.md` upkeep |
| `ghidra/NameByModule.java`, `ExportByStrings.java`, `ExportCallTree.java` | Ghidra scripts: name functions by `__FILE__` module, export by string reference, export call trees | usable as is in Ghidra's script manager |
| `re/*.py`, `re/*.sh` | Wine/apitrace/gdb helpers for running the original under Wine on Linux | optional; OpenJoan does not need Wine, but a reference capture of the original may still be useful |

`objcmp.sh <before tree> <after tree> <outdir> <file>...` (Phase 0 step 6) compiles each file from both trees with the
32-bit reference build's exact command (`OBJCMP_BUILD`, default `build/linux-x86`, configured and built by
`objcmp-x86.sh [build dir]`, the one 32-bit build kept after Phase 2; no PCH, fixed `-frandom-seed`),
strips debug information and compares the objects: `SAME` / `DIFF` (with a disassembly diff in `<outdir>`) / `NOOBJ`.
Tried on `src/ilib/date.cpp`: a
comment change is SAME, a changed constant is DIFF with 4 disassembly lines.
