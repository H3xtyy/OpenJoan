#!/bin/bash
# Builds v3d_run: the real v3d_87.dll vs src/render/v3d.cpp (freestanding i386 Linux, SSE2 float maths).
set -e
cd "$(dirname "$0")"
F="-m32 -static -nostdlib -ffreestanding -fno-pic -fno-stack-protector -msse2 -mfpmath=sse -mstackrealign -O2 -fno-builtin -fno-math-errno"
clang $F -c -o rt.o ../d3dx_harness/rt.c
clang++ $F -std=c++20 -fno-exceptions -fno-rtti -fno-threadsafe-statics -Ishim -Wall -Wextra -c -o v3d.o ../../src/render/v3d.cpp
clang++ $F -std=c++20 -fno-exceptions -fno-rtti -fno-threadsafe-statics -Ishim -Wall -c -o main.o main.cpp
clang $F -o v3d_run main.o v3d.o rt.o
rm -f rt.o v3d.o main.o
