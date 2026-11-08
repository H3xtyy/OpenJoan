#!/bin/bash
# Builds and runs the S1 record test (test_s1.cpp: the Unit family of re/a6/SAVES_PLAN.md batch S1) like build.sh:
# freestanding i686 Linux (x86 layout) writes golden_s1.bin, native x86_64 decodes and re-encodes it.
#   usage: build_s1.sh [out dir, default /tmp/cmp/scratch_s1/test]
set -e
H="$(cd "$(dirname "$0")" && pwd)"
SRC="$H/../../../src"
O="${1:-/tmp/cmp/scratch_s1/test}"
T="$O/tree/src"
rm -rf "$O/tree"; mkdir -p "$T/original/gen" "$T/save/gen"
# The game's real typed structs (data members as the game sees them); their *_methods.inc only declare member
# functions of the game (not needed here) and become empty.
# (unit.hpp embeds UnitStats, UpgradeList, TaskSlot, PointList: round P8)
for c in group unit_stats upgrade_list upgrade_list_entry unit_data held_item item_stack inv_item order_entry \
         task_slot point_list path_point; do
  cp "$SRC/original/gen/$c.hpp" "$SRC/original/gen/${c}_fields.inc" "$T/original/gen/"
  [ -f "$SRC/original/gen/${c}_methods.inc" ] && : > "$T/original/gen/${c}_methods.inc"
done
cp "$SRC/original/unit.hpp" "$SRC/original/dynarray.hpp" "$SRC/original/file.hpp" "$T/original/"; : > "$T/original/gen/unit_methods.inc"
for f in dyn_array dyn_array_b file; do : > "$T/original/gen/${f}_methods.inc"; done
printf '#pragma once\n' > "$T/call.hpp"; printf '#pragma once\n' > "$T/fn_names.hpp"
cp "$SRC/save/io.hpp" "$SRC/save/io.cpp" "$T/save/"; cp "$SRC/save/gen/records_s1.hpp" "$T/save/gen/"
# -Wno-invalid-offsetof: the generated x86 static_asserts use offsetof on HeldItem (derived from ItemStack).
W="-std=c++20 -Wall -Wextra -Wshadow -Wconversion -Werror -Wno-invalid-offsetof"
F32="-m32 -static -nostdlib -ffreestanding -fno-pic -fno-stack-protector -msse2 -mfpmath=sse -mstackrealign -O2"
clang $F32 -c -o "$O/rt.o" "$H/../../d3dx_harness/rt.c"
clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -I"$H/shim" -I"$T" -c -o "$O/io32.o" "$T/save/io.cpp"
clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -DSAVE_TEST_FREESTANDING -I"$H/shim" -I"$T" -c -o "$O/t32.o" "$H/test_s1.cpp"
clang++ $F32 -Wl,-Ttext-segment=0x10000000 -o "$O/test_s1_i686" "$O/t32.o" "$O/io32.o" "$O/rt.o"
clang++ $W -O2 -I"$T" -o "$O/test_s1_x86_64" "$H/test_s1.cpp" "$T/save/io.cpp"
echo "== i686"; "$O/test_s1_i686" --write-golden "$O/golden_s1.bin"
echo "== x86_64"; "$O/test_s1_x86_64" --check-golden "$O/golden_s1.bin"
echo "== i686 re-reads its own golden"; "$O/test_s1_i686" --check-golden "$O/golden_s1.bin" | tail -2
