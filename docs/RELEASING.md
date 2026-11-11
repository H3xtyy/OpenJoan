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

With GitHub Actions available (public repository), pushing the tag runs `.github/workflows/release.yml`: Linux in the
manylinux_2_28 container, Windows with MSVC, the archives and the Linux AppImage on a **draft** release. Check the draft, then publish it by hand.

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

The Linux release is built in the manylinux_2_28 container (AlmaLinux 8: glibc 2.28, GCC 14), the same way in CI and on
the development host (`podman` or `docker`):

```sh
podman run --rm -v "$PWD":/src:Z -w /src quay.io/pypa/manylinux_2_28_x86_64 tools/build_linux_release.sh v0.1.0
```

It links the C++ runtime statically, builds SDL3, OpenAL Soft and FFmpeg, gives every shipped file the RUNPATH `$ORIGIN`
only, refuses a result that needs a glibc newer than 2.28, and writes both the archive and the AppImage. A build on the
host itself (above) needs the host's glibc and C++ runtime, which is fine for testing but not for a release: an archive
built on a 2024 distribution does not start on an older one.

The CI release builds a minimal FFmpeg (`tools/build_ffmpeg_linux.sh`) and ships it in the Linux archive, so the archive
does not depend on the distribution's FFmpeg. An archive built against the system's FFmpeg instead lists the libraries it
needs in its `REQUIREMENTS.txt`.

## Before the repository goes public

Search the whole history for the file names of removed assets
(`git log --all -p`), and run the asset lint over every commit, not only the last one.
