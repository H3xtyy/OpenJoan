# Installing OpenJoan

OpenJoan is an open-source executable for *Wars and Warriors: Joan of Arc* (Enlight, 2004). It contains no game
files: you need your own copy of the game, and OpenJoan reads that copy's data files.

## What you need

- The game's **version 1.1 data files**, from an edition you own. OpenJoan never reads the original `joan.exe`.
  The reference data set is the Steam build's; other editions should work if their data files are the 1.1 set, but
  only the reference set has been tested.
- A graphics driver with **OpenGL 4.6**.
- **Linux x86-64:** the FFmpeg libraries named in the archive's `REQUIREMENTS.txt`, from your distribution. SDL3 and
  OpenAL Soft are in the archive.
- **Windows 10 or later, x86-64:** everything is in the archive. The Windows build has not been played by a tester
  yet; reports are welcome.

## Install

1. Make a **copy** of your game folder, so the original install stays untouched.
2. Unpack the archive for your platform and copy every file from it into that copy, next to the game's `Resource`,
   `map`, `unit` and `script` folders.
3. Start `openjoan` (Linux) or `openjoan.exe` (Windows) from that folder.

Saves from the original game load in OpenJoan.

## Settings

`openjoan.ini`, next to the executable, holds the display and rendering options. Each key is explained in the file;
`docs/RUNTIME.md` is the full reference. The release build always applies the corrections of original bugs listed
there under `[Fixes]`; in this build those keys, like the debug settings, cannot be changed.

## If it does not start

The release build writes no log files. Start it from a terminal to see any message. On Linux, `ldd ./openjoan` lists any FFmpeg
library that is missing or has another major version; in that case build from source against your system's FFmpeg: `cmake --preset linux-x64-release && cmake --build --preset linux-x64-release`.

## Licence

OpenJoan is GPL-3.0-or-later (`LICENSE.txt`). FFmpeg is LGPL 2.1 or later, SDL3 is zlib, OpenAL Soft is LGPL, and the Windows cross-build's libc++ and libunwind are
Apache-2.0 WITH LLVM-exception. The game
itself is not part of OpenJoan and is not distributed with it.
