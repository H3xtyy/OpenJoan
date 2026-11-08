#!/bin/bash
# Builds and runs the S4 save test (test_s4.cpp) twice: freestanding i686 Linux (x86 layout, as joan_re.exe; no 32-bit
# libc on this host: re/d3dx_harness/rt.c + shim/ + a minimal <type_traits>) and native x86_64 Linux. The i686 run
# writes the golden chunk stream, the x86_64 run decodes and re-encodes it.
#   usage: build_s4.sh [out dir, default /tmp/cmp/scratch_s4/test]
set -e
H="$(cd "$(dirname "$0")" && pwd)"
SRC="$H/../../../src"
O="${1:-/tmp/cmp/scratch_s4/test}"
T="$O/tree/src"
rm -rf "$O/tree" "$O/shim"; mkdir -p "$T/save/gen" "$O/shim"
# the game's real typed structs (data members only: the *_methods.inc call into the game and are emptied)
cp -r "$SRC/original" "$T/"
for f in "$T"/original/gen/*_methods.inc "$T"/original/gen/*_slots.inc; do [ -e "$f" ] && : > "$f"; done
printf '#pragma once\n' > "$T/call.hpp"; printf '#pragma once\n' > "$T/fn_names.hpp"
cp "$SRC/save/io.hpp" "$SRC/save/io.cpp" "$SRC/save/records_s4.hpp" "$T/save/"; cp "$SRC/save/gen/records_s4.hpp" "$T/save/gen/"
cp "$H"/shim/* "$O/shim/"
cat > "$O/shim/type_traits" <<'EOF'
// Minimal <type_traits> for the freestanding i686 build (src/original/colour.hpp, ui.hpp).
#pragma once
namespace std {
template <class T> inline constexpr bool is_integral_v = __is_integral(T);
template <class T> inline constexpr bool is_enum_v = __is_enum(T);
template <class T> inline constexpr bool is_pointer_v = __is_pointer(T);
template <class A, class B> inline constexpr bool is_same_v = __is_same(A, B);
}  // namespace std
EOF
W="-std=c++20 -Wall -Wextra -Wshadow -Wconversion -Werror"
F32="-m32 -static -nostdlib -ffreestanding -fno-pic -fno-stack-protector -msse2 -mfpmath=sse -mstackrealign -O2"
clang $F32 -c -o "$O/rt.o" "$H/../../d3dx_harness/rt.c"
clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -I"$O/shim" -I"$T" -c -o "$O/io32.o" "$T/save/io.cpp"
clang++ $F32 $W -fno-exceptions -fno-rtti -fno-threadsafe-statics -DSAVE_TEST_FREESTANDING -I"$O/shim" -I"$T" -c -o "$O/t32.o" "$H/test_s4.cpp"
clang++ $F32 -Wl,-Ttext-segment=0x10000000 -o "$O/test_s4_i686" "$O/t32.o" "$O/io32.o" "$O/rt.o"
clang++ $W -O2 -I"$T" -o "$O/test_s4_x86_64" "$H/test_s4.cpp" "$T/save/io.cpp"
echo "== i686"; "$O/test_s4_i686" --write-golden "$O/golden_s4.bin"
echo "== x86_64"; "$O/test_s4_x86_64" --check-golden "$O/golden_s4.bin"
echo "== i686 re-reads its own golden"; "$O/test_s4_i686" --check-golden "$O/golden_s4.bin" | tail -2
