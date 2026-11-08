// SPDX-License-Identifier: GPL-3.0-or-later
// How the ported CRT's printf / scanf engines (_output 0x007bb643, _input 0x007bc273) read their varargs.
//
// The original is MSVC x86: a va_list is a char* to the first stack vararg and every vararg is one 4-byte slot
// (a double two). The port keeps that scheme wherever a va_list IS a pointer to stack slots: x86 everywhere and
// Windows x64 (the Microsoft x64 convention spills the varargs to 8-byte stack slots and its va_list points at
// them). On System V x86-64 (Linux, macOS) the varargs travel in registers and va_list is a struct, so there the
// engines take the caller's va_list itself (every wrapper - sprintf, sscanf, vsprintf, error_fatal ... - passes
// its own, decayed to a pointer) and read it with va_arg. One switch, read by lib/crt/printf.cpp and scanf.cpp.
#pragma once

#if defined(__x86_64__) && !defined(_WIN32)
#define JOAN_SYSV_VARARGS 1
#else
#define JOAN_SYSV_VARARGS 0
#endif
