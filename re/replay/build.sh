#!/bin/sh
# Checks of the typed image hash of the replay (src/replay/typed_hash.*, hash_types.inc):
#  1. the table against the compiler's layout (src/replay/hash_types_check.cpp: static_asserts per entry), i686 and
#     x86_64 (llvm-mingw, like the other compile checks; SRC = a source tree with the mingw shims, default src/);
#  2. the host test re/replay/test_typed_hash.cpp (table invariants, recording form, two runs with other layouts).
# Regenerate the table first when the data image types change: python3 re/replay/gen_hash_types.py
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
SRC=${SRC:-$ROOT/src}
OUT=${OUT:-/tmp/joan_replay_check}
mkdir -p "$OUT"
# (x86_64 with JOAN_IMAGE_LAYOUT_PROBE: the member types only, whether or not data/image_xlate.inc is current)
for t in i686 x86_64; do
    probe=; [ $t = x86_64 ] && probe=-DJOAN_IMAGE_LAYOUT_PROBE
    /opt/llvm-mingw/bin/$t-w64-mingw32-clang++ -std=c++20 -fms-extensions -fasm-blocks -DWIN32_LEAN_AND_MEAN -DNOMINMAX $probe \
        -I"$SRC" -isystem /tmp/sdl3/include -fsyntax-only -w -ferror-limit=0 "$SRC/replay/hash_types_check.cpp"
    echo "hash_types_check.cpp ($t): ok"
done
${CXX:-g++} -std=c++20 -O1 -Wall -Wextra -Werror -I"$ROOT/src" "$HERE/test_typed_hash.cpp" "$ROOT/src/replay/typed_hash.cpp" \
    -o "$OUT/test_typed_hash"
"$OUT/test_typed_hash"
