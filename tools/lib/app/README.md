# tools/lib/app (`joan` executable)

`main.cpp`: a bootstrap executable of the asset libraries. Its own header comment calls it a bootstrap: it locates the
game data, loads the core data sets through `joan_ilib` and opens an SDL3 / OpenGL window through `joan_platform` and
`joan_render`. It is not the game: the game is the `openjoan` target built from `src/`.

`CMakeLists.txt` defines the target `joan` (links `joan_ilib`, `joan_platform`, `joan_render`); on Windows it copies the
runtime DLLs (SDL3) next to the executable. Built only when `JOAN_BUILD_ENGINE` is on and SDL3 is found.
TODO(docs): the header comment of `main.cpp` still speaks of a hook DLL and a `docs/ROADMAP.md` that does not exist; confirm
whether this target is still wanted.
