# src/runtime

The machinery that makes OpenJoan's C++ run as joan.exe (see `docs/ARCHITECTURE.md` and `docs/RUNTIME.md`). None of it is
original game code.

| Part | Files | What it does |
|---|---|---|
| Loader | `loader/` | `joan_re.cpp`: start-up (game folder, settings, data image, imports, code slots, the stray guard), `data_image.cpp`: relocates the committed data image, `import_overrides`: the imports answered by our code, `dead_imports.inc` (re/gen_dead_imports.py), `joan_re.rc` + `openjoan.ico`: Windows resources |
| Code table | `code_table.{cpp,hpp}`, `code.hpp`, `code_slots_core.hpp`, `code_slots.inc` | every original code address is a slot of `joan_code[]`; calls of original addresses go through it (`code_slots.inc`: re/gen_code_slots.py) |
| Data image | `image.hpp`, `image_runtime.{cpp,hpp}`, `import_table.inc` | the original `.rdata/.data/.bss/.idata` as one object (`joan_image`) with typed access; its import table (re/gen_data_image.py) |
| Registry | `registry.cpp` (`../registry.hpp`) | the JOAN_REPLACE registrations, settings (openjoan.ini), counters, the log |
| Patching | `patch.{cpp,hpp}` | fills code slots and thunks with the registered replacements |
| Record / replay | `replay/` | records nondeterministic inputs, hashes the game state per frame, replays |
| Debug tools | `crash`, `profiler`, `hitch`, `det_heap`, `guard_heap`, `save_audit`, `import_census`, `host` | crash reports, the profiler, frame-hitch log, the deterministic heap of replays, the guard heap, save audits, the import census, host process helpers; debug flavour only where `build_flavor.hpp` says so |

The headers every game file includes stay at `src/`: `registry.hpp`, `call.hpp`, `callconv.hpp`, `fn_names.hpp`,
`data_names.hpp`, `x87.hpp`, `compiler.hpp`, `build_flavor.hpp`. Moved here from `src/` in Phase 3 (2026-11-07).
