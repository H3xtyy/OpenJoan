# OpenJoan documentation

One line per page.

## Project

- [ARCHITECTURE.md](ARCHITECTURE.md): the runtime model (code table, data image, layouts, libraries, platform, renderer), the source tree, start-up, verification .
- [RUNTIME.md](RUNTIME.md): how the executable runs the game, start-up, the data image, every ini key, logs and the debug tools.
- [X64_STATE_CHECK.md](X64_STATE_CHECK.md): how an x64 replay of an x86 recording is checked (dumped state, not the recorded hashes).
- [PATHFINDING.md](PATHFINDING.md): the new route search and its plan.
- [COOP.md](COOP.md): the co-op plan (several players, each a hero).
- [X64.md](X64.md): the 64-bit design (widths, data image, saves, hazard meter, replay across widths).
- [CONTRIBUTING.md](CONTRIBUTING.md): workflow, commits, verification tools, review checklist.
- [INSTALL.md](INSTALL.md): installing a release into a copy of the game.
- [RELEASING.md](RELEASING.md): building the archives and publishing a release.
- [GLOSSARY.md](GLOSSARY.md): game and engine terms.
- `../src/data/README.md`: the data image as committed source (files, generators, start-up, verification).
- `../tools/README.md`: the scripts (asset lint `check_no_assets.py`, hazard meter `x64_hazards.py`, `objcmp.sh`, the PE and format tools).

## The original game

- `formats/`: one specification per file format (archives, tables, SMO/AM1/AM2/AM3, DDS, fonts) and [formats/INVENTORY.md](formats/INVENTORY.md) (census of an install).
- `re/structs/`: 110 struct layouts of the original with offsets, meanings and evidence.
- `re/functions/`: function maps per module (address, name, purpose).
- [re/MODULES.md](re/MODULES.md), [re/BINARY.md](re/BINARY.md), [re/BINARY_pe_inventory.md](re/BINARY_pe_inventory.md),
  `re/pe/`: the binary's modules, sections, imports, DLLs.
- [re/ATTRIBUTION.md](re/ATTRIBUTION.md): how functions are attributed to original source files.
- [re/RENDER_STATES.md](re/RENDER_STATES.md), [re/TIMING.md](re/TIMING.md): Direct3D 8 render states observed in the original; main loop, frame limiter and simulation step.
- `render/`: [D3D8_USAGE.md](render/D3D8_USAGE.md) (what the game uses of Direct3D 8),
  [EN3D_RENDER.md](render/EN3D_RENDER.md) (how a frame is drawn).
- `scripting/`: the `.sce` language, 408 builtins, the headless harness idea. [ADDING_MISSIONS.md](scripting/ADDING_MISSIONS.md): what a
  mission is made of and how a ninth one is added (`[Fixes] MissionCount`).
