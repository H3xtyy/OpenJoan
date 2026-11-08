# Field-wise saves (re/a6/SAVES_PLAN.md), batch S0 tooling

- `records.json`: record schemas, one section per batch (s0 active: PathPoint, Calendar; s1..s6: the SAVES_PLAN 2.2
  inventory as `"todo": true` entries with the reader's pointer handling in `notes`). Format in its `_doc`.
- `gen_save.py [--section sN] [--check]`: libclang over `src/original/direct.hpp` (i686) -> `src/save/gen/records_sN.hpp`
  (`kFields_<R>`, `k<R>`, `put_<R>(Out&, const T&)`, `get_<R>(In&, T&)`) and `gen/schema_sN.json` (for savecmp).
  Fails when the members do not tile the range, a pointer has no kind, or the size differs from layouts.json.
- `test/build.sh [outdir]`: the serialiser test (`test/test_save.cpp`) on the real Calendar / PathPoint structs,
  freestanding i686 (x86 layout) and native x86_64; i686 writes `golden_s0.bin`, x86_64 decodes and re-encodes it.
- `savecmp.py A B [--semantic]` (files or folders, e.g. two `SAVE.replayed-*` folders), `savecmp.py --selftest`.

New batch: add the records (drop `todo`, fill `ptr` / `opaque`, `legacy`), run gen_save.py, include
`../save/gen/records_sN.hpp` in the edited file, replace each raw `file_write(p, n)` / `file_read(p, n)` of an owned
function by `save::Out o(*as_file(file), save::k<R>, p); save::put_<R>(o, *obj); ok = o.end();` (In likewise),
keeping the old call under `!save::fieldwise()`; add the step grammar to `records.json` `sav.steps` where the
step consists only of converted records; extend test_save.cpp with the record.
