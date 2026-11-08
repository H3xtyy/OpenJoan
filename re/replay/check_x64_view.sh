#!/bin/sh
# Checks of the x86-layout view of joan_image that the replay hashes on x64 (src/replay/x86_view.hpp):
#  1. src/replay/hash_types_check.cpp for x86_64 WITHOUT the layout probe: every entry of hash_types.inc against the
#     compiler's x64 layout of the data image (8-byte pointer at the x64 offset of its original address, untyped
#     arrays in one run, sizeof(Image) = image_xlate.inc's size); i686 as in build.sh (SRC = a tree with the mingw
#     shims, default src/);
#  2. the host test re/replay/test_x86_view.cpp (run table, every typed entry, an x86 and an x64 run hash equal).
# (re/replay/build.sh: the typed table itself.)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
SRC=${SRC:-$ROOT/src}
OUT=${OUT:-/tmp/joan_replay_check}
mkdir -p "$OUT"
for t in i686 x86_64; do
    /opt/llvm-mingw/bin/$t-w64-mingw32-clang++ -std=c++20 -fms-extensions -fasm-blocks -DWIN32_LEAN_AND_MEAN -DNOMINMAX \
        -I"$SRC" -isystem /tmp/sdl3/include -fsyntax-only -w -ferror-limit=0 "$SRC/replay/hash_types_check.cpp"
    echo "hash_types_check.cpp ($t, full layout): ok"
done
${CXX:-g++} -std=c++20 -O1 -Wall -Wextra -Werror -I"$ROOT/src" "$HERE/test_x86_view.cpp" "$ROOT/src/replay/typed_hash.cpp" \
    -o "$OUT/test_x86_view"
"$OUT/test_x86_view"
