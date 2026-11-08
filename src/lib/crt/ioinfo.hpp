// SPDX-License-Identifier: GPL-3.0-or-later
// The MSVC 6 CRT's per-handle record (ioinfo, crt/internal.h) as the ported lowio.cpp / stdio.cpp use it: the OS
// handle is an int in the original and stays one (the platform's file-handle table hands out small integers; -1 is
// INVALID_HANDLE_VALUE; lowio.cpp os() sign-extends), the lock is a CRITICAL_SECTION of pointer-dependent size.
// __pioinfo (the image) holds pointers to blocks of 32 records. One definition for both files (Phase 2, lib-3).
#pragma once

#include <cstddef>
#include <cstdint>

namespace joan::crt {

struct IoInfo {
    std::int32_t osfhnd;  // +0x00
    std::uint8_t osfile;  // +0x04
    std::uint8_t pipech;  // +0x05
    std::uint8_t pad[2];
    std::int32_t lockinitflag;  // +0x08
#if UINTPTR_MAX != 0xffffffffu
    std::uint8_t pad_x64[4];  // x64: explicit, so the pointer-aligned lock needs no implicit padding (MSVC C4324)
#endif
    alignas(void*) std::uint8_t lock[sizeof(void*) == 4 ? 0x18 : 0x28];  // +0x0c CRITICAL_SECTION (x64: +0x10)
};
static_assert(sizeof(void*) != 4 || sizeof(IoInfo) == 0x24, "ioinfo size");
static_assert(sizeof(void*) == 4 || (offsetof(IoInfo, lock) == 0x10 && sizeof(IoInfo) == 0x38), "x64 ioinfo");

}  // namespace joan::crt
