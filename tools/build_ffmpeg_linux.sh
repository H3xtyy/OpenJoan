#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# A minimal LGPL FFmpeg for the Linux release archive, with exactly what the game's movies and music need (ASF demuxer,
# WMV1/2/3 + VC-1 and WMA v1/v2 decoders, swscale, swresample): the same build as third_party/ffmpeg-win64.
# The libraries ship next to the executable, so the archive does not depend on the distribution's FFmpeg version.
#
#   tools/build_ffmpeg_linux.sh <FFmpeg source checkout at tag n7.1.1> <output folder> [jobs]
#
# Then configure with -DJOAN_FFMPEG_ROOT=<output folder>.
set -e
SRC=$(cd "$1" && pwd)
mkdir -p "$2"
OUT=$(cd "$2" && pwd)
JOBS=${3:-4}
WORK=$(mktemp -d)
cd "$WORK"
"$SRC/configure" --prefix="$WORK/inst" --enable-shared --disable-static --enable-pic --disable-everything \
    --disable-programs --disable-doc --disable-network --disable-avdevice --disable-avfilter --disable-postproc \
    --enable-avformat --enable-avcodec --enable-swscale --enable-swresample --enable-demuxer=asf \
    --enable-decoder=wmv1,wmv2,wmv3,vc1,wmav1,wmav2 --enable-parser=vc1 --enable-protocol=file --disable-x86asm \
    --disable-autodetect --disable-debug --enable-stripping
make -j"$JOBS"
make install
rm -rf "$OUT/include" "$OUT/lib"
cp -r inst/include "$OUT/"
mkdir -p "$OUT/lib"
cp -a inst/lib/*.so* "$OUT/lib/"
rm -rf "$WORK"
