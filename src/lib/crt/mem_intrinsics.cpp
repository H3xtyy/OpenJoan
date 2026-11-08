// Original library sources (re/attribution.csv original_file, under lib/): crt/abs.c, crt/ehvecctr.cpp,
//   crt/ehvecdtr.cpp, crt/intel/memcmp.asm, crt/intel/memcpy.asm,
//   crt/intel/strncmp.asm, crt/intel/ulldiv.asm, crt/qsort.c, crt/tran/i386/ftol.asm.
//
// Statically linked MSVC 6 C runtime inside joan.exe, range 0x007b2240..0x007b31ec (batch 113 / list 07).
//
// Identification of the roots (full table with globals / families: scratchpad w113_07/ids.md):
//   0x007b2240 atan2(double, double)      - fp-exception family, NOT replaced (see below)
//   0x007b2274 acos(double)               - fp-exception family, NOT replaced
//   0x007b2344 sqrt(double)               - fp-exception family, NOT replaced
//   0x007b2404 sin(double)                - fp-exception family, NOT replaced
//   0x007b24b4 cos(double)                - fp-exception family, NOT replaced
//   0x007b2564 asin(double)               - fp-exception family, NOT replaced
//   0x007b2719 pow(double, double)        - fp-exception family, NOT replaced
//   0x007b2920 fmod(double, double)       - fp-exception family, NOT replaced
//   0x007b2952 sprintf(char*, const char*, ...) - printf core (_output 0x7bb643), NOT replaced
//   0x007b3148 strtok(char*, const char*) - per-thread data (_getptd 0x7b8c21, ptd+0x18), NOT replaced
//   0x007b2620 memcmp                      replaced (crt/b113_07)
//   0x007b26cc _ftol                       dead (no caller, no code pointer: game code converts with
//                                          joan::hook::x87::ftol, src/x87.hpp): deleted in port round P1
//   0x007b29a4 abs                         replaced
//   0x007b29b0 _chkstk / _alloca_probe     dead (no caller, no code pointer): deleted in cleanup C1
//   0x007b29e0 strncmp                     replaced
//   0x007b2a20 memcpy (VC6 memcpy == memmove: overlap-safe)  replaced
//   0x007b2d55 qsort (+ static shortsort 0x7b2ea9, swap 0x7b2ef7 folded in)  replaced
//   0x007b2f30 _aulldiv                    replaced (u64 division)
//   0x007b2f98 `eh vector constructor iterator' (??_L)  replaced (guard object for the original's __try/__finally)
//   0x007b3020 _allmul                     dead (no caller, no code pointer): deleted in cleanup C1
//   0x007b3054 `eh vector destructor iterator' (??_M)   replaced (guard object for the original's __try/__finally)
//
// Why the math functions stay original: the game runs with cw != 0x027f (PC24), so every call takes the
// "slow" path: the CRT switches to its own cw, computes, then 0x7b9f9b (_check_range_exit) sees the
// precision-exception mask bit in the saved cw and the sticky PE bit in the status word (practically
// always set) and calls _87except (0x7c2cd9) with _INEXACT: the st0 result is rounded through a double
// (fstp/fld qword), _handle_exc 0x7b9746 / _set_statfp 0x7b9c47 / _ctrlfp 0x7b9c24 / errno setter
// 0x7b99e5 (_getptd) run. They must be replaced together with that fp-exception / errno family.
//
// Install: patch_entry accepts any entry in 0x765000..0x7d5000 (5-byte jmp; all entries here verified
// with re/check_lib_entries.py: >= 5 bytes, no jump into entry+1..4), so every registration below is live.
//
// Module: "crt/b113_07".
#include "../../callconv.hpp"
#include <cstdint>
#include <cstring>

#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

struct Obj {};
using joan::call::thiscall;

JOAN_COUNTER(c_memcmp, "crt::memcmp");
JOAN_COUNTER(c_strncmp, "crt::strncmp");
JOAN_COUNTER(c_memcpy, "crt::memcpy");
JOAN_COUNTER(c_qsort, "crt::qsort");
JOAN_COUNTER(c_vec_ctor, "crt::eh_vector_constructor_iterator");
JOAN_COUNTER(c_vec_dtor, "crt::eh_vector_destructor_iterator");

// ------------------------------------------------------------------ 0x007b2620
// int memcmp(const void*, const void*, size_t): -1 / 0 / +1 by the first differing byte (unsigned).
// (The original's dword tail compare may read up to 3 bytes past the end; not observable.)
int JOAN_CDECL crt_memcmp(const void* a, const void* b, u32 n) {
    c_memcmp.hit();
    const u8* p = static_cast<const u8*>(a);
    const u8* q = static_cast<const u8*>(b);
    for (u32 i = 0; i < n; ++i)
        if (p[i] != q[i]) return p[i] < q[i] ? -1 : 1;
    return 0;
}

// ------------------------------------------------------------------ 0x007b29a4
// int abs(int); abs(INT_MIN) == INT_MIN like `neg`.
int JOAN_CDECL crt_abs(int x) { return x >= 0 ? x : static_cast<int>(0u - static_cast<u32>(x)); }

// ------------------------------------------------------------------ 0x007b29e0
// int strncmp(const char*, const char*, size_t): compares min(n, strlen(s1) + 1) bytes, -1 / 0 / +1
// by the first differing byte (unsigned).
int JOAN_CDECL crt_strncmp(const char* s1, const char* s2, u32 n) {
    c_strncmp.hit();
    if (n == 0) return 0;
    const u8* a = reinterpret_cast<const u8*>(s1);
    const u8* b = reinterpret_cast<const u8*>(s2);
    u32 count = 0;  // repnz scasb: bytes scanned including the NUL, at most n
    while (count < n) {
        ++count;
        if (a[count - 1] == 0) break;
    }
    for (u32 i = 0; i < count; ++i)
        if (a[i] != b[i]) return b[i] > a[i] ? -1 : 1;
    return 0;
}

// ------------------------------------------------------------------ 0x007b2a20
// void* memcpy(void* dst, const void* src, size_t n): the VC6 memcpy is the memmove code - copies
// backwards when src < dst < src + n, forwards otherwise. Returns dst.
void* JOAN_CDECL crt_memcpy(void* dst, const void* src, u32 n) {
    c_memcpy.hit();
    // Original: backwards when src < dst < src + n, forwards otherwise, in dword chunks; both directions
    // are overlap-safe for any chunk width, so the bytes written equal a memmove in every case.
    std::memmove(dst, src, n);
    return dst;
}

// ------------------------------------------------------------------ 0x007b2d55
// void qsort(void* base, size_t num, size_t width, int (*comp)(const void*, const void*)): the VC6
// qsort.c algorithm (CUTOFF 8, 30-entry explicit stack, median = middle element swapped to lo),
// with shortsort 0x7b2ea9 and swap 0x7b2ef7 inlined. The order of comp calls and swaps is the same.
using Comp = int(JOAN_CDECL*)(const void*, const void*);
using uptr = std::uintptr_t;  // element addresses (pointer-sized: the array may lie anywhere)

void swap_bytes(uptr a, uptr b, u32 width) {  // 0x007b2ef7
    if (a == b) return;
    u8* p = reinterpret_cast<u8*>(a);
    u8* q = reinterpret_cast<u8*>(b);
    while (width != 0) {
        const u8 t = *p;
        *p++ = *q;
        *q++ = t;
        --width;
    }
}
int call_comp(Comp comp, uptr a, uptr b) {
    return comp(reinterpret_cast<const void*>(a), reinterpret_cast<const void*>(b));
}
void shortsort(uptr lo, uptr hi, u32 width, Comp comp) {  // 0x007b2ea9
    while (hi > lo) {
        uptr max = lo;
        for (uptr p = lo + width; p <= hi; p += width)
            if (call_comp(comp, p, max) > 0) max = p;
        swap_bytes(max, hi, width);
        hi -= width;
    }
}

void JOAN_CDECL crt_qsort(void* base, u32 num, u32 width, Comp comp) {
    c_qsort.hit();
    if (num < 2 || width == 0) return;
    uptr lostk[30] = {};
    uptr histk[30] = {};
    i32 stkptr = 0;
    uptr lo = reinterpret_cast<uptr>(base);
    uptr hi = lo + width * (num - 1);
    for (;;) {
        const u32 size = static_cast<u32>((hi - lo) / width + 1);  // the element count fits 32 bits (num is u32)
        if (size <= 8) {
            shortsort(lo, hi, width, comp);
        } else {
            const uptr mid = lo + (size >> 1) * width;
            swap_bytes(mid, lo, width);
            uptr loguy = lo;
            uptr higuy = hi + width;
            for (;;) {
                do {
                    loguy += width;
                } while (loguy <= hi && call_comp(comp, loguy, lo) <= 0);
                do {
                    higuy -= width;
                } while (higuy > lo && call_comp(comp, higuy, lo) >= 0);
                if (higuy < loguy) break;
                swap_bytes(loguy, higuy, width);
            }
            swap_bytes(lo, higuy, width);
            if (static_cast<i32>(higuy - lo - 1) >= static_cast<i32>(hi - loguy)) {
                if (lo + width < higuy) {
                    lostk[stkptr] = lo;
                    histk[stkptr] = higuy - width;
                    ++stkptr;
                }
                if (loguy < hi) {
                    lo = loguy;
                    continue;
                }
            } else {
                if (loguy < hi) {
                    lostk[stkptr] = loguy;
                    histk[stkptr] = hi;
                    ++stkptr;
                }
                if (lo + width < higuy) {
                    hi = higuy - width;
                    continue;
                }
            }
        }
        --stkptr;
        if (stkptr < 0) return;
        lo = lostk[stkptr];
        hi = histk[stkptr];
    }
}

// ------------------------------------------------------------------ 0x007b2f30
// _aulldiv: unsigned 64 / 64 -> edx:eax, ret 16 (= __stdcall with two 64-bit arguments on x86). VC6 ulldiv.asm
// computes the exact quotient (estimate from the shifted divisor, corrected by one); a zero divisor is the
// division by zero the original's `div` faults on.
std::uint64_t JOAN_STDCALL crt_aulldiv(std::uint64_t dividend, std::uint64_t divisor) { return dividend / divisor; }

// ------------------------------------------------------------------ 0x007b2f98 / 0x007b3054
// The compiler's array helpers. Ctor / dtor are __thiscall void(void*) (ecx = element). The originals
// are __try/__finally functions (_except_handler3, scope tables 0x9e6fc8 / 0x9e6fd8): when a ctor /
// dtor raises, the __finally (0x7b3002 / 0x7b30bc) calls __ArrayUnwind 0x7b30d4 (stdcall, ret 16;
// destroys the already-built elements and calls terminate() if one of those destructors throws a C++
// exception) with the original's completion flag. Here the __finally is the destructor of a guard: it
// runs exactly when the loop is left by an exception (the flag is still 0), and calls __ArrayUnwind at its
// original address with the same arguments. A structured exception (access violation ...) raised by a ctor /
// dtor is handled by no frame of this program (no __except is left: the catch (...) of the game's catchers takes
// C++ exceptions only, original/cxx_eh.hpp), so it never unwinds these frames: it ends in the crash handler.
void array_unwind(void* ptr, u32 size, i32 count, std::uintptr_t dtor) {
    reinterpret_cast<void(JOAN_STDCALL*)(void*, u32, i32, std::uintptr_t)>(fn::code_value(fn::crt_ArrayUnwind))(ptr, size, count, dtor);
}

struct ArrayUnwindGuard {
    u8* const& p;
    const u32 size;
    const i32& count;
    const std::uintptr_t dtor;
    bool done;
    ~ArrayUnwindGuard() {
        if (!done) array_unwind(p, size, count, dtor);
    }
};

void JOAN_STDCALL crt_vec_ctor(void* ptr, u32 size, i32 count, std::uintptr_t ctor, std::uintptr_t dtor) {
    c_vec_ctor.hit();
    u8* p = static_cast<u8*>(ptr);
    i32 i = 0;
    ArrayUnwindGuard guard{p, size, i, dtor, false};
    for (i = 0; i < count; ++i) {
        u8* const e = p;
        thiscall(ctor, e);
        p = e + size;
    }
    guard.done = true;
}

void JOAN_STDCALL crt_vec_dtor(void* ptr, u32 size, i32 count, std::uintptr_t dtor) {
    c_vec_dtor.hit();
    u8* p = static_cast<u8*>(ptr) + size * static_cast<u32>(count);
    i32 n = count;
    ArrayUnwindGuard guard{p, size, n, dtor, false};
    for (;;) {
        n = static_cast<i32>(static_cast<u32>(n) - 1u);  // dec / js
        if (n < 0) break;
        p = p - size;
        thiscall(dtor, p);
    }
    guard.done = true;
}

#define CRT(name, addr, fn) JOAN_REPLACE("crt/b113_07", name, addr, fn)
CRT("crt::memcmp", 0x007b2620, &crt_memcmp);
CRT("crt::abs", 0x007b29a4, &crt_abs);
CRT("crt::strncmp", 0x007b29e0, &crt_strncmp);
CRT("crt::memcpy", 0x007b2a20, &crt_memcpy);
CRT("crt::qsort", 0x007b2d55, &crt_qsort);
CRT("crt::_aulldiv", 0x007b2f30, &crt_aulldiv);
CRT("crt::eh_vector_constructor_iterator", 0x007b2f98, &crt_vec_ctor);
CRT("crt::eh_vector_destructor_iterator", 0x007b3054, &crt_vec_dtor);
#undef CRT

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::memcmp(void* a0, void* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_memcmp>(a0, a1, a2)); }  // 0x7b2620
auto joan::orig::libc::abs(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_abs>(a0)); }  // 0x7b29a4
auto joan::orig::libc::strncmp(const char* a0, const char* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_strncmp>(a0, a1, a2)); }  // 0x7b29e0
auto joan::orig::libc::eh_vector_constructor_iterator(void* a0, std::uint32_t a1, std::int32_t a2, std::uintptr_t a3, std::uintptr_t a4) -> void { joan::bind::free<&::crt_vec_ctor>(a0, a1, a2, a3, a4); }  // 0x7b2f98
auto joan::orig::libc::eh_vector_destructor_iterator(void* a0, std::uint32_t a1, std::int32_t a2, std::uintptr_t a3) -> void { joan::bind::free<&::crt_vec_dtor>(a0, a1, a2, a3); }  // 0x7b3054
