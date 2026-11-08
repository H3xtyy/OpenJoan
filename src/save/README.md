# src/save

Field-wise save serialisation. The game's records are written and read field by field into exactly the bytes the 32-bit
`joan.exe` wrote, on x86 and on x64, so an original `.SAV` loads on x64 and a save written on x64 loads on x86 (Phase 2
exit; see `docs/X64.md`). Portable: no Win32, no game code in the serialiser (the game's parts come in through
`hooks()`).

| File | What |
|---|---|
| `io.{hpp,cpp}` | `Out` / `In`: one original `File::file_write(p, n)` is one chunk; `Out` collects the bytes of a record field by field and hands them over in one call, `In` reads one chunk with one `file_read`. Little-endian, floats as bit patterns. `Ptr` kinds say what the reader does with a saved pointer (on x64 a token that is 0 iff the pointer is null; image values are written as original addresses) |
| `io_file.cpp` | the game's `File` (`src/ilib/file.cpp`) as a `save::Port`, and the thin wrappers of `io.hpp` |
| `records_s3.hpp`, `records_s4.hpp`, `s5.hpp` | the hand-written chunks the generator cannot express (world, campaign, chunks shared by several game files) next to the generated records of their section |
| `gen/records_s0.hpp` .. `records_s6.hpp` | **generated**: field tables and `put_<R>` / `get_<R>` per record, by `re/save/gen_save.py` from `re/save/records.json` (libclang pass over the layouts; needs the llvm-mingw sysroot, `JOAN_MINGW`). Do not edit |

`src/save/gen/` has no README of its own: this page covers it.

Settings (`[Debug]` in `openjoan.ini`): `SaveFieldwise=0` keeps the old raw path at the converted call sites,
`SaveCompare=1` (x86) compares each field-wise chunk with the bytes the raw path would write, `SaveAudit=2` maps heap and
image values in written chunks to the record's fields (`src/runtime/save_audit.*`).

Verification: `re/save/test` (serialiser test on real structs, i686 and x86_64, golden file), `re/save/savecmp.py`
(compares two save folders), the replay (saves are part of the reference recordings), and `re/save/README.md`.

See also: [`docs/ARCHITECTURE.md`](../../docs/ARCHITECTURE.md) section 5, `docs/X64.md`.
