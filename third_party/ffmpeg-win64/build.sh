#!/bin/sh
# Rebuilds this folder: a minimal LGPL FFmpeg for 64-bit Windows with exactly what the game's movies and music need
# (ASF demuxer, WMV1/2/3 + VC-1 and WMA v1/v2 decoders, swscale, swresample). Cross-built with llvm-mingw
# (https://github.com/mstorsjo/llvm-mingw, x86_64-w64-mingw32-clang on PATH); the DLLs need only the Windows 10+
# universal CRT. Usage: ./build.sh <FFmpeg source checkout at tag n7.1.1> [jobs]
# (cmake/ffmpeg.cmake uses this folder for x64 builds when it exists).
set -e
SRC=$(cd "$1" && pwd)
JOBS=${2:-4}
OUT=$(cd "$(dirname "$0")" && pwd)
TRIPLE=x86_64-w64-mingw32; CONF_ARCH="--arch=x86_64"; EXTRA_CFLAGS=""; DLLTOOL_M=i386:x86-64
mkdir -p "$OUT"
WORK=$(mktemp -d)
cd "$WORK"
"$SRC/configure" --prefix="$WORK/inst" --target-os=mingw32 $CONF_ARCH --cross-prefix=$TRIPLE- \
    --cc=$TRIPLE-clang --enable-cross-compile --enable-shared --disable-static --disable-everything \
    --disable-programs --disable-doc --disable-network --disable-avdevice --disable-avfilter --disable-postproc \
    --enable-avformat --enable-avcodec --enable-swscale --enable-swresample --enable-demuxer=asf \
    --enable-decoder=wmv1,wmv2,wmv3,vc1,wmav1,wmav2 --enable-parser=vc1 --enable-protocol=file --disable-x86asm \
    --disable-autodetect --disable-debug --enable-stripping ${EXTRA_CFLAGS:+--extra-cflags=$EXTRA_CFLAGS}
make -j"$JOBS"
make install
rm -rf "$OUT/include" "$OUT/bin" "$OUT/lib"
mkdir -p "$OUT/bin" "$OUT/lib"
cp -r inst/include "$OUT/"
cp inst/bin/*.dll "$OUT/bin/"
# MSVC import libraries from the export lists (i386: the C names get their leading underscore here).
for def in inst/lib/*.def; do
    n=$(basename "$def" .def)
    llvm-dlltool -m $DLLTOOL_M -d "$def" -D "$n.dll" -l "$OUT/lib/${n%-*}.lib"
done
cp "$SRC/COPYING.LGPLv2.1" "$OUT/"
rm -rf "$WORK"
