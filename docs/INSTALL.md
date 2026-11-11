# Installing OpenJoan

OpenJoan is an open-source executable for *Wars and Warriors: Joan of Arc* (Enlight, 2004). It contains no game
files: you need your own copy of the game, and OpenJoan reads that copy's data files.

## What you need

- The game's **data files**, from an edition you own. OpenJoan never reads the original `joan.exe`.
- A graphics driver with **OpenGL 4.6**.
- **Linux x86-64:** glibc 2.28 or newer (any distribution from 2018 on; systemd is not needed) and X11 or Wayland.
  FFmpeg, SDL3 and OpenAL Soft are in the archive and in the AppImage; sound goes through ALSA, PulseAudio or PipeWire,
  whichever the system has. Distributions built on musl instead of glibc (Alpine, Void musl, Chimera) cannot run them.
- **Windows 10 or later, x86-64:** everything is in the archive.

## Install

1. Unpack the archive for your platform and copy every file from it into your game folder, next to the game's `Resource`,
   `map`, `unit` and `script` folders.
2. Start `openjoan` (Linux) or `openjoan.exe` (Windows) from that folder.

On Linux the AppImage, `OpenJoan-<version>-x86_64.AppImage`, is the same program in one file: copy it into the game folder,
make it executable (`chmod +x`) and start it. It writes `openjoan.ini` into the game folder the first time. It needs FUSE
(`/dev/fuse` and `fusermount`), which almost every desktop has; without it, start it with `--appimage-extract-and-run`.

OpenJoan does not work with a `d3d8.dll` in the game folder, such as the community widescreen patch or another graphics
wrapper. Remove it first.

## Settings

`openjoan.ini`, next to the executable, holds the display and rendering options. Each key is explained in the file;
`docs/RUNTIME.md` is the full reference.

## Saves when the game is installed under Program Files

Windows does not let a program write inside `C:\Program Files`. When the game folder cannot be written, OpenJoan keeps its
saves, `CONFIG.DAT`, settings and any other file it writes in `%LOCALAPPDATA%\OpenJoan` (`~/.local/share/openjoan` on Linux)
and reads them back from there. Installing somewhere writable, such as `C:\Games`, keeps everything in the game folder.

## Licence

OpenJoan is GPL-3.0-or-later (`LICENSE.txt`). FFmpeg is LGPL 2.1 or later, SDL3 is zlib, OpenAL Soft is LGPL, and the Windows cross-build's libc++ and libunwind are
Apache-2.0 WITH LLVM-exception. The game itself is not part of OpenJoan and is not distributed with it.
