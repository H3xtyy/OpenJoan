#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# objcmp-x86: configures and builds the one 32-bit x86 build OpenJoan keeps after Phase 2, the reference for
# tools/objcmp.sh (object-identical proofs of moves and renames).
#
#   tools/objcmp-x86.sh [build dir] [cmake --build arguments...]
#
# Build dir: default $OBJCMP_BUILD or build/linux-x86 (objcmp.sh's default). Configured once with what the retired
# preset linux-x86 set: Ninja, RelWithDebInfo, cmake/linux-i686.cmake (-m32, the multilib packages of skill oj-build),
# the game executable only, SDL3 and OpenAL Soft built from source. Its src/openjoan is also the x86 binary for
# replaying x86 recordings and for the x86 half of the x86-vs-x64 state check (docs/X64_STATE_CHECK.md).
# Linux only; there is no 32-bit Windows build any more.
set -eu
root=$(cd "$(dirname "$0")/.." && pwd)
build=${1:-${OBJCMP_BUILD:-$root/build/linux-x86}}
[ $# -gt 0 ] && shift
if [ ! -f "$build/CMakeCache.txt" ]; then
    cmake -S "$root" -B "$build" -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE="$root/cmake/linux-i686.cmake" \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DJOAN_BUILD_HOOK=ON -DJOAN_BUILD_ENGINE=OFF -DJOAN_BUILD_TOOLS=OFF -DJOAN_BUILD_TESTS=OFF \
        -DJOAN_SDL3_SYSTEM=OFF -DJOAN_OPENAL_SYSTEM=OFF
fi
cmake --build "$build" "$@"
