# Releasing OpenJoan

A release is a version tag and one archive per platform, built by `tools/package_release.py` from a release-flavour
build. The archive holds only named files (executable, settings, runtime libraries, licences, `docs/INSTALL.md`); no
game file can reach it.

## Before tagging

1. For the commit: `ctest`, the hazard meter, one replay per build (x86 and x64), a
   manual play test of the release flavour.
2. `python3 tools/check_no_assets.py`, `python3 tools/docs_lint.py` and `doxygen docs/Doxyfile` pass.
3. Choose the version: `vMAJOR.MINOR.PATCH`.

## Building the archives

With GitHub Actions available (public repository), pushing the tag runs `.github/workflows/release.yml`: Linux on
Ubuntu 24.04, Windows with MSVC, both archives on a **draft** release. Check the draft, then publish it by hand.

While CI cannot run (private repository, ), build on the development
host. The Windows archive is then the llvm-mingw cross-build:

```sh
export JOAN_MINGW=$JOAN_WORK_DIR/toolchains/llvm-mingw
cmake --preset linux-x64-release && cmake --build --preset linux-x64-release
cmake --preset mingw-x64-release && cmake --build --preset mingw-x64-release
python3 tools/package_release.py build/linux-x64-release v0.1.0
python3 tools/package_release.py build/mingw-x64-release v0.1.0
```

The archives appear in `dist/` (git-ignored).

A Linux archive runs only with the FFmpeg major versions it was linked against, listed in its `REQUIREMENTS.txt`. An
archive built on a rolling distribution therefore needs that distribution's FFmpeg; the Ubuntu 24.04 CI build is the
one meant for general use.

## Before the repository goes public

Search the whole history for the file names of removed assets
(`git log --all -p`), and run the asset lint over every commit, not only the last one.
