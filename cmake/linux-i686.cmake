# SPDX-License-Identifier: GPL-3.0-or-later
# 32-bit x86 openjoan on a 64-bit Linux (multilib / multiarch): everything is compiled and linked with -m32, SDL3 and
# OpenAL Soft are built from source with it, FFmpeg comes from the distribution's 32-bit development packages through
# pkg-config. Not a cross build (the host runs the 32-bit tools). Since the Phase 2 exit only the objcmp reference build
# uses it (tools/objcmp-x86.sh; it also replays x86 recordings): the playable builds are 64-bit.
set(CMAKE_C_FLAGS_INIT "-m32")
set(CMAKE_CXX_FLAGS_INIT "-m32")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-m32")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-m32")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-m32")
set(CMAKE_ASM_FLAGS_INIT "-m32")

# The 32-bit .pc files: Debian / Ubuntu (multiarch), Arch (lib32-*), Fedora (i686 packages).
if(NOT DEFINED ENV{PKG_CONFIG_LIBDIR})
    set(_joan_pc_dirs "")
    foreach(_dir /usr/lib/i386-linux-gnu/pkgconfig /usr/lib32/pkgconfig /usr/lib/pkgconfig /usr/share/pkgconfig)
        if(EXISTS "${_dir}")
            list(APPEND _joan_pc_dirs "${_dir}")
        endif()
    endforeach()
    string(REPLACE ";" ":" _joan_pc_dirs "${_joan_pc_dirs}")
    set(ENV{PKG_CONFIG_LIBDIR} "${_joan_pc_dirs}")
endif()
# Debian / Ubuntu keep the 32-bit libraries in /usr/lib/i386-linux-gnu (find_library looks there with this set).
if(EXISTS /usr/lib/i386-linux-gnu)
    set(CMAKE_LIBRARY_ARCHITECTURE i386-linux-gnu)
endif()
