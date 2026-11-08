# FFmpeg for the 64-bit Windows build (movies and music)

A minimal build of [FFmpeg](https://ffmpeg.org) 7.1.1 (tag `n7.1.1`, commit `db69d06eeeab4f46da15030a80d539efb4503ca8`) for 64-bit
Windows, used by `src/media/` to decode the game's `movie\*.wmv` (WMV3 video, WMA v2 audio) and `music\*.wma` on Windows.

- `bin/`: avformat-61, avcodec-61, avutil-59, swscale-8, swresample-5 (.dll), copied next to openjoan.exe by the build;
  they need only KERNEL32, bcrypt and the Windows 10+ universal CRT
- `lib/`: MSVC import libraries (llvm-dlltool); `include/`: the FFmpeg headers
- only the ASF demuxer, the WMV1/2/3 / VC-1 and WMA v1/v2 decoders, swscale and swresample are built in
- license: LGPL 2.1 or later (`COPYING.LGPLv2.1`); linked dynamically
- reproduce: `./build.sh <FFmpeg checkout at n7.1.1> <jobs>` with llvm-mingw on PATH

Instead of this folder, cmake/ffmpeg.cmake also takes vcpkg's FFmpeg
(`vcpkg install ffmpeg[core,avcodec,avformat,swscale,swresample]:x64-windows`) or `-DJOAN_FFMPEG_ROOT=<folder with
include/ lib/ bin/>`.
