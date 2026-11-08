# src/original

The game's own classes and services as C++ that the ported code calls:

- `gen/`: the typed layouts and method facades, generated from `re/layout/layouts.json` (`re/layout/gen_types.py`);
  2,700+ files, one per class, never edited by hand.
- hand-written headers: typed views and call helpers for classes and services the generated layouts do not cover yet
  (`direct.hpp`: `joan::orig::arg` argument conventions and the `as_<class>` casts; `engine.hpp`, `en3d.hpp`, `ui.hpp`,
  `vecmath.hpp`, `dynarray.hpp`, `file*.hpp`, `mem.hpp`, `bind.hpp` (the facade bridges), `layout_access.hpp` ...).
- `helpers.hpp`: the small helpers every batch module used to define again (CRT wrappers `memset_` / `strcpy_` /
  `strlen_` / `stricmp_`, `game_new` / `game_delete` / `op_new` / `op_delete`, `at<T>`, `addr` / `u`, the vtable and
  import call helpers `virtual_call` / `vcall` / `com_call` / `win_call`, and about 55 more self-contained ones); merged in
  Phase 3 (2026-11-07): each former copy is a `using joan::orig::helpers::<name>;` where it stood.
