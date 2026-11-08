#!/bin/bash
# Builds and runs the S5 serialiser test (test_s5.cpp) twice, like build.sh: freestanding i686 Linux (x86 layout, as
# joan_re.exe) and native x86_64 Linux; the i686 run writes the golden chunk stream, the x86_64 run decodes and
# re-encodes it.   usage: build_s5.sh [out dir, default /tmp/cmp/scratch_s5/test]
set -e
H="$(cd "$(dirname "$0")" && pwd)"
SRC="$H/../../../src"
O="${1:-/tmp/cmp/scratch_s5/test}"
T="$O/tree/src"
rm -rf "$O/tree"; mkdir -p "$T/original/gen" "$T/save/gen"
# the game's real typed structs; their call.hpp / fn_names.hpp includes are not needed by the data members
for f in config game_file key_binding profile profile_unit wnd sys_en_window dev; do
  cp "$SRC/original/gen/$f.hpp" "$SRC/original/gen/${f}_fields.inc" "$T/original/gen/"
  [ -f "$SRC/original/gen/${f}_methods.inc" ] && cp "$SRC/original/gen/${f}_methods.inc" "$T/original/gen/"
done
cp "$SRC/original/dynarray.hpp" "$SRC/original/file.hpp" "$T/original/"
# the member functions (slot calls) are not needed: empty method lists
for f in "$T"/original/gen/*_methods.inc; do : > "$f"; done
for f in dyn_array dyn_array_b file; do : > "$T/original/gen/${f}_methods.inc"; done
printf '#pragma once\n' > "$T/call.hpp"; printf '#pragma once\n' > "$T/fn_names.hpp"
cp "$SRC/save/io.hpp" "$SRC/save/io.cpp" "$T/save/"; cp "$SRC/save/gen/records_s5.hpp" "$T/save/gen/"
mkdir -p "$O"
W="-std=c++20 -Wall -Wextra -Wshadow -Wconversion -Werror -Wno-invalid-offsetof"
F32="-m32 -static -nostdlib -ffreestanding -fno-pic -fno-stack-protector -msse2 -mfpmath=sse -mstackrealign -O2"
clang $F32 -c -o "$O/rt.o" "$H/../../d3dx_harness/rt.c"
clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -I"$H/shim" -I"$T" -c -o "$O/io32.o" "$T/save/io.cpp"
clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -DSAVE_TEST_FREESTANDING -I"$H/shim" -I"$T" -c -o "$O/t32.o" "$H/test_s5.cpp"
clang++ $F32 -Wl,-Ttext-segment=0x10000000 -o "$O/test_s5_i686" "$O/t32.o" "$O/io32.o" "$O/rt.o"
clang++ $W -O2 -I"$T" -o "$O/test_s5_x86_64" "$H/test_s5.cpp" "$T/save/io.cpp"
echo "== i686"; "$O/test_s5_i686" --write-golden "$O/golden_s5.bin"
echo "== x86_64"; "$O/test_s5_x86_64" --check-golden "$O/golden_s5.bin"
echo "== i686 re-reads its own golden"; "$O/test_s5_i686" --check-golden "$O/golden_s5.bin" | tail -2
