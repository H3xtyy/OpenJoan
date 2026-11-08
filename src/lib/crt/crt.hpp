// Direct C++ entry points of the ported MSVC 6 CRT (src/lib/crt/*.cpp) for game code that called the CRT by its
// original address (cdecl_call(fn::crt_..., ...)). Same functions as the registered replacements, without the
// slot indirection.
#pragma once
#include "../../callconv.hpp"
#include <cstdint>

namespace joan::crt {

void* JOAN_CDECL operator_new(std::uint32_t n);                     // 0x007b1d35 heap.cpp
void JOAN_CDECL operator_delete(void* p);                           // 0x007b1d2a heap.cpp
int JOAN_CDECL stricmp(const char* a, const char* b);               // 0x007c07b0 ctype.cpp (_stricmp, locale-aware)
int JOAN_CDECL atoi(const char* s);                                 // 0x007b15cb string.cpp
int JOAN_CDECL atexit(std::uintptr_t func);                         // 0x007b361a exit.cpp (func: a code address)
inline int JOAN_CDECL atexit(const void* func) { return atexit(reinterpret_cast<std::uintptr_t>(func)); }

#if UINTPTR_MAX != 0xffffffffu  // x64: _iob[20] is a native array of 48-byte FILEs (stdio.cpp), not image data
int iob_index(const void* fp);  // index of a stream of _iob[20], -1 for any other (a malloc'd _FILEX)
void* filex_cs(void* fp);       // the CRITICAL_SECTION of a _FILEX (after its FILE)
#endif

/// The per-thread gmtime buffer slot of the CRT's _tiddata (mt.cpp Tiddata::gmtimebuf: +0x40 on x86, +0x70 on x64),
/// for time.cpp's gmtime; `ptd` is what _getptd returned. Phase 2 (request lib-1): no raw offset in time.cpp.
void*& ptd_gmtimebuf(void* ptd) noexcept;

}  // namespace joan::crt
