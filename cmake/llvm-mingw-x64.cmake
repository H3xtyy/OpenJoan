# SPDX-License-Identifier: GPL-3.0-or-later
# Windows x86-64 cross build on Linux with llvm-mingw (clang + lld + mingw-w64, UCRT): the local check that the game
# compiles and links for 64-bit Windows (preset mingw-x64). Not MSVC: an error only MSVC reports still needs a
# Visual Studio build. The toolchain lies outside the repository; JOAN_MINGW names its folder (the unpacked release of
# https://github.com/mstorsjo/llvm-mingw, e.g. $JOAN_WORK_DIR/toolchains/llvm-mingw). SDL3 and OpenAL Soft are built
# from source with it, FFmpeg comes from third_party/ffmpeg-win64.
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
if(NOT DEFINED ENV{JOAN_MINGW})
    message(FATAL_ERROR "llvm-mingw-x64.cmake: set JOAN_MINGW to the llvm-mingw folder (bin/x86_64-w64-mingw32-clang++)")
endif()
set(_joan_mingw "$ENV{JOAN_MINGW}")
set(CMAKE_C_COMPILER "${_joan_mingw}/bin/x86_64-w64-mingw32-clang")
set(CMAKE_CXX_COMPILER "${_joan_mingw}/bin/x86_64-w64-mingw32-clang++")
set(CMAKE_RC_COMPILER "${_joan_mingw}/bin/x86_64-w64-mingw32-windres")
set(CMAKE_AR "${_joan_mingw}/bin/llvm-ar")
set(CMAKE_RANLIB "${_joan_mingw}/bin/llvm-ranlib")
set(CMAKE_FIND_ROOT_PATH "${_joan_mingw}/x86_64-w64-mingw32")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)  # also third_party/ffmpeg-win64 (absolute PATHS)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
