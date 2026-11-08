#!/bin/bash
# Builds and runs the S0 serialiser test (test_save.cpp) twice: freestanding i686 Linux (x86 layout, as joan_re.exe;
# no 32-bit libc on this host: re/d3dx_harness/rt.c + shim/) and native x86_64 Linux. The i686 run writes the golden
# chunk stream, the x86_64 run decodes and re-encodes it.   usage: build.sh [out dir, default /tmp/cmp/scratch_s0/test]
set -e
H="$(cd "$(dirname "$0")" && pwd)"
SRC="$H/../../../src"
O="${1:-/tmp/cmp/scratch_s0/test}"
T="$O/tree/src"
rm -rf "$O/tree"; mkdir -p "$T/original/gen" "$T/save/gen"
# the game's real typed structs; their call.hpp / fn_names.hpp includes are not needed by the data members
for f in calendar.hpp calendar_fields.inc calendar_methods.inc path_point.hpp path_point_fields.inc; do
  cp "$SRC/original/gen/$f" "$T/original/gen/"
done
printf '#pragma once\n' > "$T/call.hpp"; printf '#pragma once\n' > "$T/fn_names.hpp"
cp "$SRC/save/io.hpp" "$SRC/save/io.cpp" "$T/save/"; cp "$SRC/save/gen/records_s0.hpp" "$T/save/gen/"
W="-std=c++20 -Wall -Wextra -Wshadow -Wconversion -Werror"
F32="-m32 -static -nostdlib -ffreestanding -fno-pic -fno-stack-protector -msse2 -mfpmath=sse -mstackrealign -O2"
clang $F32 -c -o "$O/rt.o" "$H/../../d3dx_harness/rt.c"
for f in io.cpp; do clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -I"$H/shim" -I"$T" -c -o "$O/io32.o" "$T/save/$f"; done
clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -DSAVE_TEST_FREESTANDING -I"$H/shim" -I"$T" -c -o "$O/t32.o" "$H/test_save.cpp"
clang++ $F32 -Wl,-Ttext-segment=0x10000000 -o "$O/test_save_i686" "$O/t32.o" "$O/io32.o" "$O/rt.o"
clang++ $W -O2 -I"$T" -o "$O/test_save_x86_64" "$H/test_save.cpp" "$T/save/io.cpp"
echo "== i686"; "$O/test_save_i686" --write-golden "$O/golden_s0.bin"
echo "== x86_64"; "$O/test_save_x86_64" --check-golden "$O/golden_s0.bin"
echo "== i686 re-reads its own golden"; "$O/test_save_i686" --check-golden "$O/golden_s0.bin" | tail -2
