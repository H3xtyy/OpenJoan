# OpenJoan

An open-source reimplementation of **Wars and Warriors: Joan of Arc** (Enlight Software, 2004) for 64-bit Windows
and Linux. OpenJoan rebuilt the game function by function as its own executable on SDL3,
OpenAL Soft, FFmpeg and OpenGL 4.6; this repository makes that code 64-bit, organised, documented and buildable
from a clean clone.

> You need your own copy of the game. This repository contains no game asset, resource, text or save and no machine
> code of the original; the executable's static data (strings, tables, constants) is committed as typed source
> and everything else is read from your installation.

## What's new compared with the original game

- **Native 64-bit** builds for Linux and Windows, built from this repository alone. Only the game's data
  files are needed.
- **Modern displays:** any resolution including widescreen, borderless window, DPI awareness, a UI scale option, high
  frame rates and vsync control.
- **OpenGL 4.6 renderer:** ambient occlusion, new water with reflections and waves, anisotropic
  filtering and optional MSAA, with GPU-generated shadow volumes for the original shadows.
- **Game pads:** full menu control, camera sticks, assignable
  buttons, and automatic switching between pad and keyboard.
- **Better mouse control:** smooth mouse look, a wider camera sensitivity range, a separate cursor sensitivity, and
  wheel zoom that behaves the same at any frame rate.

## Building

You need CMake 3.21 or newer, Ninja, and a C++20 compiler. No game files are needed to build.

**Linux x86-64** (GCC 13+ or Clang 17+), with the development packages of SDL3, OpenAL Soft and FFmpeg installed. If
your distribution lacks SDL3 or OpenAL Soft, add `-DJOAN_SDL3_SYSTEM=OFF -DJOAN_OPENAL_SYSTEM=OFF` to the first command
to download and build them.

```sh
cmake --preset linux-x64-release
cmake --build --preset linux-x64-release
```

**Windows x86-64** (Visual Studio 2022 or newer), from a Developer Command Prompt for x64. The other libraries are
downloaded or bundled:

```sh
cmake --preset windows-hook-x64-release
cmake --build --preset windows-hook-x64-release
```

The result is in `build/<preset>/src/`. Copy the executable, `openjoan.ini` and its libraries into a copy of your game
folder and start it there (`docs/INSTALL.md`). The `linux-x64` preset is the development build, with logs and debug tools.

## Documentation

Start at [docs/README.md](docs/README.md). The format specifications (`docs/formats/`), struct layouts
(`docs/re/structs/`) and function maps (`docs/re/functions/`) describe the original game and are valid regardless
of the engine.

## Legal

Wars and Warriors: Joan of Arc is the property of its respective owners. OpenJoan is an independent reimplementation
for interoperability and preservation; it does not distribute any part of the original game. The code is licensed
under the GNU General Public License, version 3 or later (`LICENSE`).
