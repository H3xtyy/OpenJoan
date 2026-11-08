// MSVC's keywords, intrinsics and C runtime names, written portably:
//   - JOAN_NOINLINE / JOAN_FORCEINLINE, JOAN_RETURN_ADDRESS() / JOAN_RETURN_ADDRESS_SLOT() (_ReturnAddress /
//     _AddressOfReturnAddress), JOAN_DEBUG_BREAK(), joan::cpu::ticks() (__rdtsc), joan::cpu::id() (__cpuid),
//     joan::cpu::pause() (YieldProcessor),
//     joan::cpu::x87_single_precision() (_control87(_PC_24, _MCW_PC)), JOAN_STACK_TRACE()
//     (RtlCaptureStackBackTrace): MSVC's own on MSVC, so its builds compile to the same code as before;
//   - the C runtime functions our code calls under MSVC's names (_stricmp, _strnicmp, _strupr, _strlwr, ...): only
//     declared here for other compilers (MSVC's C runtime has them), as functions in the global namespace - not
//     macros, so the game's own C runtime (joan::orig::libc::_strupr, crt::_stricmp ...) keeps its names. CMake
//     includes this file in every source file on those compilers (-include), as MSVC's headers declare them in each.
// JOAN_RETURN_ADDRESS* are macros: they read the frame of the function they are written in.
#pragma once

#include <cstdint>

#if defined(_MSC_VER)
#include <float.h>  // _control87
#include <intrin.h>
#else
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#if (defined(__i386__) || defined(__x86_64__)) && defined(_WIN32)
#include <intrin.h>  // MinGW: __cpuid as on MSVC (<cpuid.h>'s macro of that name breaks <intrin.h>, which SDL includes)
#include <x86intrin.h>
#elif defined(__i386__) || defined(__x86_64__)
#include <cpuid.h>
#include <x86intrin.h>
#else
#include <chrono>
#endif
#if defined(__has_include)
#if __has_include(<execinfo.h>)
#include <execinfo.h>
#define JOAN_HAVE_EXECINFO 1
#endif
#endif
#endif

#if defined(_MSC_VER)
#define JOAN_NOINLINE __declspec(noinline)
#define JOAN_FORCEINLINE __forceinline
#define JOAN_RETURN_ADDRESS() _ReturnAddress()
#define JOAN_RETURN_ADDRESS_SLOT() _AddressOfReturnAddress()
#define JOAN_DEBUG_BREAK() __debugbreak()
#define JOAN_TARGET(features)
#else
#define JOAN_NOINLINE __attribute__((noinline))
#define JOAN_FORCEINLINE inline __attribute__((always_inline))
#define JOAN_RETURN_ADDRESS() __builtin_return_address(0)
// The slot holding the return address: just above the saved frame pointer (x86 / x64 frames; GCC / Clang give a
// function that asks for __builtin_frame_address a frame pointer even with -fomit-frame-pointer).
#define JOAN_RETURN_ADDRESS_SLOT() \
    (static_cast<void*>(static_cast<char*>(__builtin_frame_address(0)) + sizeof(void*)))
#define JOAN_DEBUG_BREAK() __builtin_trap()
// A function compiled for instruction set extensions it checks for at run time ("avx", "xsave"): GCC / Clang
// compile their intrinsics only in such functions; MSVC anywhere.
#define JOAN_TARGET(features) __attribute__((target(features)))
#endif

#if defined(_WIN32)
extern "C" __declspec(dllimport) unsigned short __stdcall RtlCaptureStackBackTrace(unsigned long, unsigned long, void**,
                                                                                     unsigned long*);
#endif

namespace joan::cpu {

/// The CPU's time stamp counter (__rdtsc); elsewhere than x86 a nanosecond clock.
inline std::uint64_t ticks() {
#if defined(_MSC_VER) || defined(__i386__) || defined(__x86_64__)
    return __rdtsc();
#else
    return static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
#endif
}

/// CPUID leaf `leaf`: eax, ebx, ecx, edx in r[0..3] (__cpuid); all zero elsewhere than x86.
inline void id(int r[4], int leaf) {
#if defined(_MSC_VER) || defined(_WIN32)
    __cpuid(r, leaf);
#elif defined(__i386__) || defined(__x86_64__)
    unsigned a = 0, b = 0, c = 0, d = 0;
    __cpuid(static_cast<unsigned>(leaf), a, b, c, d);
    r[0] = static_cast<int>(a);
    r[1] = static_cast<int>(b);
    r[2] = static_cast<int>(c);
    r[3] = static_cast<int>(d);
#else
    (void)leaf;
    r[0] = r[1] = r[2] = r[3] = 0;
#endif
}

/// A spin-wait hint to the processor (YieldProcessor: pause / yield).
inline void pause() {
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    _mm_pause();
#elif defined(_MSC_VER) && defined(_M_ARM64)
    __yield();
#elif defined(__i386__) || defined(__x86_64__)
    __builtin_ia32_pause();
#elif defined(__aarch64__)
    __asm__ volatile("yield");
#endif
}

#if defined(_M_IX86) || defined(__i386__)
/// 32-bit x86: the x87 unit's precision control to 24 bits (_control87(_PC_24, _MCW_PC)).
inline void x87_single_precision() {
#if defined(_MSC_VER)
    _control87(_PC_24, _MCW_PC);
#else
    unsigned short cw = 0;
    __asm__ volatile("fnstcw %0" : "=m"(cw));
    cw = static_cast<unsigned short>(cw & ~0x300u);  // precision control 00: 24 bits
    __asm__ volatile("fldcw %0" : : "m"(cw));
#endif
}
#endif

/// Up to `n` return addresses of this thread's stack into `frames`, the innermost `skip` frames left out (0: starting
/// with the function that calls it). Returns how many were written; 0 where the stack cannot be walked. A macro on
/// Windows (RtlCaptureStackBackTrace in the caller itself, as before); elsewhere stack_trace_from below.
#if defined(_WIN32)
#define JOAN_STACK_TRACE(skip, n, frames) RtlCaptureStackBackTrace((skip), (n), (frames), nullptr)
#else
#define JOAN_STACK_TRACE(skip, n, frames) ::joan::cpu::stack_trace_from((skip) + 1u, (n), (frames))
#endif

#if !defined(_WIN32)
/// JOAN_STACK_TRACE elsewhere: `skip` counts this function's own frame (out of line so that it is one).
/// x86-64: a walk of the frame-pointer chain (the build keeps frame pointers there, src/CMakeLists.txt): glibc's
/// backtrace() unwinds through DWARF and faults on the frames of the code table's generated stubs, which have no
/// unwind information (Phase 2: the first x64 replay crashed in libgcc under the det_heap's heap trace). Each frame
/// must lie above the previous one and within 1 MB of it, and hold a return address; the walk stops otherwise.
JOAN_NOINLINE inline unsigned stack_trace_from(unsigned skip, unsigned n, void** frames) {
#if defined(__x86_64__)
    struct Frame {
        Frame* next;
        void* ret;
    };
    const Frame* f = static_cast<const Frame*>(__builtin_frame_address(0));
    unsigned out = 0, seen = 0;
    while (f != nullptr && out < n && seen < 64u) {
        const Frame* const next = f->next;
        if (next <= f || reinterpret_cast<std::uintptr_t>(next) - reinterpret_cast<std::uintptr_t>(f) > (1u << 20) ||
            (reinterpret_cast<std::uintptr_t>(next) & 7u) != 0 || f->ret == nullptr)
            break;
        if (seen >= skip) frames[out++] = f->ret;
        ++seen;
        f = next;
    }
    return out;
#elif defined(JOAN_HAVE_EXECINFO)
    void* all[64];
    const unsigned want = skip + n < 64u ? skip + n : 64u;
    const int got = backtrace(all, static_cast<int>(want));
    unsigned out = 0;
    for (int i = static_cast<int>(skip); i < got && out < n; ++i) frames[out++] = all[i];
    return out;
#else
    (void)skip;
    (void)n;
    (void)frames;
    return 0;
#endif
}
#endif

}  // namespace joan::cpu

#if !defined(_WIN32)  // MSVC and MinGW have these names
// MSVC's C runtime names (see above). Case-insensitive compares and case changes are ASCII ones, as the "C" locale
// our code runs in makes MSVC's.
inline int _stricmp(const char* a, const char* b) { return strcasecmp(a, b); }
inline int _strnicmp(const char* a, const char* b, std::size_t n) { return strncasecmp(a, b, n); }
inline int _strcmpi(const char* a, const char* b) { return strcasecmp(a, b); }
inline char* _strupr(char* s) {
    for (char* p = s; *p; ++p) *p = static_cast<char>(std::toupper(static_cast<unsigned char>(*p)));
    return s;
}
inline char* _strlwr(char* s) {
    for (char* p = s; *p; ++p) *p = static_cast<char>(std::tolower(static_cast<unsigned char>(*p)));
    return s;
}
inline int _mkdir(const char* path) { return mkdir(path, 0777); }
inline int _chdir(const char* path) { return chdir(path); }
inline char* _getcwd(char* buf, int size) { return getcwd(buf, static_cast<std::size_t>(size)); }
inline int _fileno(std::FILE* f) { return fileno(f); }
#endif
