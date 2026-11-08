# Installing OpenJoan

OpenJoan is an open-source executable for *Wars and Warriors: Joan of Arc* (Enlight, 2004). It contains no game
files: you need your own copy of the game, and OpenJoan reads that copy's data files.

## What you need

- The game's **data files**, from an edition you own. OpenJoan never reads the original `joan.exe`.
- A graphics driver with **OpenGL 4.6**.
- **Linux x86-64:** FFmpeg, SDL3 and
  OpenAL Soft are in the archive.
- **Windows 10 or later, x86-64:** everything is in the archive.

## Install

1. Unpack the archive for your platform and copy every file from it into your game folder, next to the game's `Resource`,
   `map`, `unit` and `script` folders.
2. Start `openjoan` (Linux) or `openjoan.exe` (Windows) from that folder.

## Settings

`openjoan.ini`, next to the executable, holds the display and rendering options. Each key is explained in the file;
`docs/RUNTIME.md` is the full reference.

## Licence

OpenJoan is GPL-3.0-or-later (`LICENSE.txt`). FFmpeg is LGPL 2.1 or later, SDL3 is zlib, OpenAL Soft is LGPL, and the Windows cross-build's libc++ and libunwind are
Apache-2.0 WITH LLVM-exception. The game itself is not part of OpenJoan and is not distributed with it.
