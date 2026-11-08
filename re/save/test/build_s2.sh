#!/bin/bash
# Builds and runs the S2 (map records) serialiser test test_s2.cpp like build.sh does for S0: freestanding i686 Linux
# (x86 layout) writes golden_s2.bin, native x86_64 Linux decodes and re-encodes it.
#   usage: build_s2.sh [out dir, default /tmp/cmp/scratch_s2/test]
set -e
H="$(cd "$(dirname "$0")" && pwd)"
SRC="$H/../../../src"
O="${1:-/tmp/cmp/scratch_s2/test}"
T="$O/tree/src"
rm -rf "$O/tree"; mkdir -p "$T/original/gen" "$T/save/gen"
# the game's real typed structs (data members only: the *_methods.inc slot wrappers are replaced by empty files)
for h in place facility building siege siege_order shop shop_stock flag_facility map_object order0b order00 gate order0c_a \
         order0c_b order0c_c order45 wall world_item missile held_item item_stack; do
  cp "$SRC/original/gen/$h.hpp" "$SRC/original/gen/${h}_fields.inc" "$T/original/gen/"
  : > "$T/original/gen/${h}_methods.inc"
done
cp "$SRC/original/records.hpp" "$T/original/"; : > "$T/original/gen/recruit_row_methods.inc"
printf '#pragma once\n' > "$T/call.hpp"; printf '#pragma once\n' > "$T/fn_names.hpp"
cp "$SRC/save/io.hpp" "$SRC/save/io.cpp" "$T/save/"; cp "$SRC/save/gen/records_s2.hpp" "$T/save/gen/"
W="-std=c++20 -Wall -Wextra -Wshadow -Wconversion -Werror -Wno-invalid-offsetof"
F32="-m32 -static -nostdlib -ffreestanding -fno-pic -fno-stack-protector -msse2 -mfpmath=sse -mstackrealign -O2"
NS="-fno-exceptions -fno-rtti -fno-threadsafe-statics"
clang $F32 -c -o "$O/rt.o" "$H/../../d3dx_harness/rt.c"
clang++ $F32 $W $NS -I"$H/shim" -I"$T" -c -o "$O/io32.o" "$T/save/io.cpp"
clang++ $F32 $W $NS -DSAVE_TEST_FREESTANDING -I"$H/shim" -I"$T" -c -o "$O/t32.o" "$H/test_s2.cpp"
clang++ $F32 -Wl,-Ttext-segment=0x10000000 -o "$O/test_s2_i686" "$O/t32.o" "$O/io32.o" "$O/rt.o"
clang++ $W -O2 -I"$T" -o "$O/test_s2_x86_64" "$H/test_s2.cpp" "$T/save/io.cpp"
echo "== i686"; "$O/test_s2_i686" --write-golden "$O/golden_s2.bin"
echo "== x86_64"; "$O/test_s2_x86_64" --check-golden "$O/golden_s2.bin"
echo "== i686 re-reads its own golden"; "$O/test_s2_i686" --check-golden "$O/golden_s2.bin" | tail -2
