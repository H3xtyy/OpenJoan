#!/bin/bash
# Builds and runs the S3 (world) record test (test_s3.cpp) twice, like build.sh: freestanding i686 Linux (x86 layout)
# writing golden_s3.bin, then native x86_64 Linux decoding and re-encoding it.   usage: build_s3.sh [out dir]
set -e
H="$(cd "$(dirname "$0")" && pwd)"
SRC="$H/../../../src"
O="${1:-/tmp/cmp/scratch_s3/test}"
T="$O/tree/src"
rm -rf "$O/tree"; mkdir -p "$T/original/gen" "$T/save/gen"
# the game's real typed structs and the headers they include (their call.hpp / fn_names.hpp are not needed)
todo="world.hpp scene.hpp cell.hpp scene_s.hpp player.hpp cam_pose.hpp light_sys.hpp"
while [ -n "$todo" ]; do
  next=""
  for f in $todo; do
    [ -e "$T/original/gen/$f" ] && continue
    cp "$SRC/original/gen/$f" "$T/original/gen/"
    for i in $(grep -o '#include "[a-z0-9_]*\.\(hpp\|inc\)"' "$SRC/original/gen/$f" | sed 's/#include "\(.*\)"/\1/'); do next="$next $i"; done
  done
  todo="$next"
done
# member functions do not change the data layout (none is virtual): empty, so no game call / fn tables are needed
for f in "$T"/original/gen/*_methods.inc; do : > "$f"; done
printf '#pragma once\n' > "$T/call.hpp"; printf '#pragma once\n' > "$T/fn_names.hpp"
cp "$SRC/save/io.hpp" "$SRC/save/io.cpp" "$SRC/save/records_s3.hpp" "$T/save/"; cp "$SRC/save/gen/records_s3.hpp" "$T/save/gen/"
W="-std=c++20 -Wall -Wextra -Wshadow -Wconversion -Werror"
F32="-m32 -static -nostdlib -ffreestanding -fno-pic -fno-stack-protector -msse2 -mfpmath=sse -mstackrealign -O2"
clang $F32 -c -o "$O/rt.o" "$H/../../d3dx_harness/rt.c"
clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -I"$H/shim" -I"$T" -c -o "$O/io32.o" "$T/save/io.cpp"
clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -DSAVE_TEST_FREESTANDING -I"$H/shim" -I"$T" -c -o "$O/t32.o" "$H/test_s3.cpp"
clang++ $F32 -Wl,-Ttext-segment=0x10000000 -o "$O/test_s3_i686" "$O/t32.o" "$O/io32.o" "$O/rt.o"
clang++ $W -O2 -I"$T" -o "$O/test_s3_x86_64" "$H/test_s3.cpp" "$T/save/io.cpp"
echo "== i686"; "$O/test_s3_i686" --write-golden "$O/golden_s3.bin"
echo "== x86_64"; "$O/test_s3_x86_64" --check-golden "$O/golden_s3.bin"
echo "== i686 re-reads its own golden"; "$O/test_s3_i686" --check-golden "$O/golden_s3.bin" | tail -2
