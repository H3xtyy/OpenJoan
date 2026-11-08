# Contributing to OpenJoan

OpenJoan is the game *Wars and Warriors: Joan of Arc* rebuilt function by function; the first rule is that **the game
plays as it did before your change**. This page is the workflow and the list of rules (section 5). Start with [ARCHITECTURE.md](ARCHITECTURE.md) and
[RUNTIME.md](RUNTIME.md).

## 1. Workflow

1. **Baseline first.** Know how the module you touch is verified (a replay recording, a smoke step, the hazard counts)
   and note the numbers before you start; they go in the commit message.
2. **One kind of change per commit.** A move (functions to another file) is never mixed with a rename, a rename never
   with a 64-bit fix, a 64-bit fix never with a behaviour change. Reviewers read moves with `git diff -M`.
3. **Object-identical where possible.** For a move or rename, build the 32-bit reference (`tools/objcmp-x86.sh`) and
   prove the compiled objects did not change (section 3). A reviewed DIFF that only reorders or renumbers is stated in
   the commit message.
4. **Hazard meter never rises** (section 3). The baseline is only updated downwards.
5. **Replay gate: one recording per build per change.** A change that can touch game state is replayed on one recording
   on each build it affects, not on all of them. Test a `[Fixes]` switch
   on a recording by switching it on at a chosen frame, with audio detached, never in live mode.
6. **Docs in the same change.** Module `README.md`, the `docs/` page, `GLOSSARY.md` for new terms;
   `python3 tools/docs_lint.py` is clean for the files you touched.

## 2. Commits

Conventional commits with the module as scope: `refactor(game): move unit functions out of _unplaced`,
`fix(x64): Dialogue frame offsets typed`, `feat(runtime): ...`, `docs(ilib): module README`. State the baseline and
the verification numbers in the body. Commit and push only when asked to. Branch off `main`; do not commit on it
for anything but the owner's own work.

## 3. Verification tools

All commands run from the repository root. Each tool's header comment is its full documentation.

| Tool | Command | What it proves |
|---|---|---|
| `tools/objcmp-x86.sh` | `tools/objcmp-x86.sh [build dir]` | configures and builds the one 32-bit build kept (default `build/linux-x86`), the reference of the two tools below; needs the 32-bit multilib packages |
| `tools/objcmp.sh` | `tools/objcmp.sh <before tree> <after tree> <outdir> <file>...` | each listed file compiles to the same 32-bit object in both trees (`SAME` / `DIFF` / `NOOBJ`); the trees are two checkouts, e.g. a `git worktree` of the baseline |
| `tools/objcmp_union.py` | `tools/objcmp_union.py [--no-inline] <before tree> <after tree> <outdir> <file>...` | for a **move**: the union of the functions of the listed files is identical before and after (compared by symbol); add a new file to `src/CMakeLists.txt` and reconfigure the reference build first |
| `re/check_registrations.py` | `re/check_registrations.py --dump before.tsv`, then `--compare before.tsv` | no `JOAN_REPLACE*` registration (address, name, module, kind) was lost, added or changed; static initialisers are not compared by objcmp, this covers them |
| `tools/x64_hazards.py` | `python3 tools/x64_hazards.py --baseline re/x64_census/hazards_baseline.txt` | the 32-bit-assumption counts per subsystem did not rise; run before every commit that touches `src/` |
| `tools/x64_state_check.sh` | `tools/x64_state_check.sh <recording> [every] [--x64-recording]` | an x86 and an x64 replay of one recording reach the same game state ([X64_STATE_CHECK.md](X64_STATE_CHECK.md)) |
| `tools/check_no_assets.py` | see its header | nothing derived from the game folder is committed (CI runs it) |
| `tools/docs_lint.py` | `python3 tools/docs_lint.py` | required pages exist, every module has a README, links resolve |

Recordings live in `$JOAN_WORK_DIR/recordings`. The recorded state hash covers `.data` and `.bss` of the data image
with the types of the layout the recordings were made with, so those chunks are pinned: see
[`src/data/README.md`](../src/data/README.md) before changing the image or its generators.

### Renaming

Renames go through the generators and rename tools, never `sed` (section 5, "Names and IDs").
Each takes a committed CSV table (address or class, old name, new name, evidence); the table is the record. Run them
on a clean tree, with `--dry-run` first where the tool has it.

| Renames | Tool |
|---|---|
| a registration name (`JOAN_REPLACE("Unit::cleanup", ...)`); names are the IDs in logs, so there is no alias table | `re/rename_registrations.py --apply re/names/<table>.csv [--dry-run]` |
| the `fn::` code-slot names that derive from a renamed registration | `re/rename_fn_names.py <table.csv> [--dry-run]` |
| `d::` data names (`src/data_names.hpp`) and their uses | `re/rename_data_names.py <table.csv> [--dry-run]` |
| a layout field (`Unit::u32_2a` to a real name), found by the compiler in every use, generated layouts and save records regenerated | `re/layout/rename_fields.py <table.csv> [--build build/linux-x64]` |

A renamed registration is also renamed in `src/openjoan.ini` and the docs in the same change.

## 4. Adding a `[Fixes]` switch

A deliberate deviation from the original is allowed only behind a `[Fixes]` key, default on, so `Key=0` plays exactly
as the original. Follow an existing one (`TriggerSort`, `src/registry.hpp`, `src/runtime/registry.cpp`):

1. declare the setting in `src/registry.hpp` and read it from the `[Fixes]` section in `src/runtime/registry.cpp`;
2. branch at the single place the original behaviour lives, citing the original address and the key in a `///` comment;
3. document the key in the `[Fixes]` section of `src/openjoan.ini` (what the original does, what the fix changes) and in
   [RUNTIME.md](RUNTIME.md);
4. if the fix changes game state, the replay must apply it as the recording did: add a flag to the recording's header
   (`src/runtime/replay/stream.hpp`, as `kFlagTriggerSortFix`);
5. prove with a replay that `Key=0` is identical to the build without the fix.

## 5. Rules that are never broken

A change that breaks one of these is reverted, whatever else it achieves.

| Area | Required | Forbidden |
|---|---|---|
| Behaviour | the game plays as the reference build plays; a record/replay run stays identical where replay exists; a manual play test passes on every supported build; float results are part of the contract | "fixing" original behaviour outside the `[Fixes]` switches; changing float evaluation order in ported game code; `-ffast-math`; dropping the x87 emulation (`src/fp/`) |
| Game data | the executable's static data lives in the repository only as the generated, typed and commented source of `src/data/` (`layout.hpp`, `rdata.inc`, `data.inc`, `image.cpp`, `data_relocs.inc`, `image_xlate.inc`), regenerated by `re/gen_data_curated.py` from the reference build; the asset lint `check_no_assets` passes | committing anything from the game folder or derived from it byte for byte: icons, dialogs, bitmaps, sounds, models, movies, text tables, saves, machine code; new untyped `u8` blobs (every added member has a type, a name and a reader) |
| Build inputs | a clean clone builds every target with the toolchain alone; tests that need the game skip when `JOAN_GAME_DIR` is unset | CMake or code that looks for `joan.exe`; copying the user's files into the repository tree |
| Architecture | 64-bit correctness in every file: pointers are pointers (`T*`, `std::uintptr_t`), object sizes and strides are `sizeof`, vtables are indexed as `void**`, struct layouts come from `src/original/gen` (typed), saves go through field-wise records; the counts of `tools/x64_hazards.py` only go down | new `at<T>(p, 0x..)` raw offset reads, `static_cast<u32>(reinterpret_cast<uintptr_t>(p))`, `DWORD`/`long` for addresses, `__asm`, `alignas` padding to fake layouts, constants like `0x4b4` where `sizeof(Unit)` is meant |
| Platform stack | C++20; CMake 3.21+ with presets; SDL3 (window, input, timers), OpenAL Soft, FFmpeg 7.x or newer, OpenGL 4.6 core through `src/gl` and `src/render`; GCC 13+ / Clang 17+ on Linux, MSVC 2022+ on Windows | new direct Win32 / DirectX / WinMM / Registry use outside `src/platform` and the loader; a second graphics API; dropping Windows or Linux |
| Names and IDs | `JOAN_REPLACE` names (`"Unit::cleanup"`), original addresses and `openjoan.ini` keys stay stable or get a documented migration; renames of `fn::` / `d::` symbols go through the generators and rename tools (section 3) | `sed` renames of generated identifiers; renaming a registration without updating docs and the ini |
| Documentation | every module directory has a `README.md`; every new or renamed public function has a `///` comment with purpose, units, ownership and the original address; `docs/` is updated in the same commit | undocumented new headers; docs that describe a planned state as the current one |
| Legal | GPL-3.0-or-later (`LICENSE`, `// SPDX-License-Identifier: GPL-3.0-or-later` on new and curated files); the code is our own C++ written from analysis of the user's copy, the static data a typed transcription under | verbatim decompiler output; assets; redistribution instructions for game files; code under GPL-2-only or other incompatible licences |

### Style

New identifiers: `snake_case` functions and variables, `PascalCase` types, `k` constants, members without prefix (the
generated layouts use plain names). Keep the original's class and method names where they are known (`Unit::cleanup`,
`ResourceIdx::get_index`): they are the vocabulary of the docs and the registrations. Headers: `#pragma once`, includes
sorted, no `using namespace` at namespace scope. Every function that replaces an original one has its address on the
definition line or the line above (`// 0x00461f10 Unit::cleanup`); every behaviour note cites the address it was read
from; unknowns are `TODO(re): 0xADDR what is unknown`. No exceptions across game code. `.clang-format` (LLVM base, 120
columns) is applied to files as they are curated, never to untouched files in the same commit as a move.

## 6. Review checklist

- One kind of change; the message gives the baseline and the verification numbers.
- Moves and renames: objcmp / objcmp_union `SAME` and `check_registrations.py --compare` clean.
- `x64_hazards.py` not higher than the baseline; no new `at<T>(p, 0x..)`, pointer-to-`u32` casts, `__asm`, or
  `0x4b4` where `sizeof(Unit)` is meant.
- Behaviour unchanged, or behind a `[Fixes]` key with a replay.
- Every new or renamed public function has a `///` comment (purpose, units, ownership, original address); original
  behaviour cites its address; unknowns are `TODO(re): 0xADDR ...`.
- Docs updated in the same change; `docs_lint.py` clean; no planned state described as current.
- Nothing from the game folder in the diff.

## 7. Parallel work

When several people or tools work at once: keep ownership of files disjoint (a header or generated-file change is a
request to whoever owns it), use one compiler process and one build directory each (`build/<name>/`), treat the game,
Ghidra and the replay runner as single-tenant (one user at a time), keep reports short, and redo anything that cannot be
checked rather than trusting it. Nobody commits or pushes on behalf of the owner unasked.
