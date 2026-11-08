# tools/ghidra

Ghidra scripts (Java, category `OpenJoan`); run them in Ghidra's script manager or with
`analyzeHeadless` (`GHIDRA_INSTALL_DIR`). They work on the reference executable `joan.exe` (or the Steam build) and write
into a folder you name; nothing is written into the repository. See `docs/re/`.

| Script | Does | Arguments |
|---|---|---|
| `NameByModule.java` | attributes functions to their original source file from the embedded `d:\ww\...cpp` path strings, names them `<dir>::<file>::fn_<addr>`, names the CRT entry and `WinMain`; writes a function index (Markdown) and a JSON name map | `<index.md> <names.json> [retail\|steam]` (the last picks the fixed-point table; default none) |
| `ExportByStrings.java` | for each literal of a list, finds the string's references and decompiles every function that references it to `<outdir>/<addr>_<name>.c`, with an `INDEX.md` | `<outdir> <literals-file>` |
| `ExportCallTree.java` | decompiles given entry functions and their callees to a bounded depth (default 2) with an `INDEX.md`; CRT and import thunks are listed, not decompiled | `<outdir> <families-file> [depth]` |

The scripts' header comments hold the exact `analyzeHeadless` command lines. Decompiler output is analysis material, not
source: never paste it into `src/` or `docs/`.
