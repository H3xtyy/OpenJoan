// The calling conventions of joan.exe's code (32-bit x86), written JOAN_CDECL / JOAN_STDCALL / JOAN_FASTCALL /
// JOAN_THISCALL in our code (functions the game calls through its code slots and imports, the function pointer
// types we call it through).
//   - MSVC, clang-cl, MinGW: their own keywords (__cdecl ...), which these compilers ignore where a target has only
//     one convention (x64, ARM64);
//   - GCC / Clang elsewhere (Linux): nothing, on 32-bit x86 too. Every caller and callee is our own code there, and
//     the Itanium C++ ABI calls a member function like a plain one with `this` as its first argument (cdecl, not
//     MSVC's thiscall): with one convention for all of them, a member function, a JOAN_THISCALL function taking the
//     object first and the pointers that call either always agree - as on x64.
#pragma once

#if defined(_MSC_VER) || defined(__MINGW32__)
#define JOAN_CDECL __cdecl
#define JOAN_STDCALL __stdcall
#define JOAN_FASTCALL __fastcall
#define JOAN_THISCALL __thiscall
#if defined(_M_IX86) || defined(__i386__)
// 32-bit x86 with the four conventions told apart: distinct function types (stdcall vs cdecl), and a __fastcall
// function with an unused edx argument receives a __thiscall call. Undefined where all four are one (x64, Linux).
#define JOAN_X86_CONVENTIONS 1
#endif
#else
#define JOAN_CDECL
#define JOAN_STDCALL
#define JOAN_FASTCALL
#define JOAN_THISCALL
#endif

// A pointer to member function filled from a code address with memcpy (a vtable slot): MSVC's single-inheritance
// member pointer is that address alone; the Itanium C++ ABI's (GCC / Clang elsewhere) is {address, `this`
// adjustment}, so the pointer is declared zeroed (`Fn method{};`) and only its address word is copied
// (`std::memcpy(&method, slot, sizeof(void*))`).
#define JOAN_MEMBER_POINTER_CHECK(Fn)                                               \
    static_assert(sizeof(Fn) == sizeof(void*) || sizeof(Fn) == 2 * sizeof(void*), \
                  "single-inheritance member pointer")
