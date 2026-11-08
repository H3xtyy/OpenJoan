// Original library sources (re/attribution.csv original_file, under lib/): crt/bsearch.c, crt/intel/strchr.asm,
//   crt/intel/strncat.asm, crt/lfind.c, crt/tran/i386/ceil.c, crt/tran/i386/floor.c.
//
// Batch 113/08: statically linked MSVC 6 C runtime, range 0x007b31ec..0x007b3e93.
//
// Identification of the roots (full table with globals / families: scratchpad w113_08/ids.md):
//   0x007b31ec ceil(double)                 PURE  - implemented (see below)
//   0x007b32bb floor(double)                PURE  - implemented
//   0x007b338a remove(const char*)          STATEFUL errno (DeleteFileA + _dosmaperr 0x7bbf42)
//   0x007b33b4 _unlink(const char*)         STATEFUL errno (calls remove)
//   0x007b33bf time(time_t*)                STATEFUL time (cache 0xa6b1c8 / 0xa6b1d0, __loctotime_t)
//   0x007b349b _mkdir(const char*)          STATEFUL errno (CreateDirectoryA + _dosmaperr)
//   0x007b34e0 strchr(const char*, int)     PURE  - implemented
//   0x007b361a atexit(void(*)())            STATEFUL exit (_onexit 0x7b359c, table 0xa6cd40/0xa6cd3c)
//   0x007b365b bsearch(key, base, num, width, cmp)   PURE - implemented
//   0x007b36e7 _lfind(key, base, unsigned* num, width, cmp)  PURE - implemented
//   0x007b3719 _findfirst(spec, _finddata_t*)        STATEFUL errno + time (_tzset state)
//   0x007b37e6 _findnext(handle, _finddata_t*)       STATEFUL errno + time
//   0x007b38ae _findclose(handle)                    STATEFUL errno
//   0x007b3932 _makepath(path, drive, dir, fname, ext) STATEFUL locale/mbcs (_mbsdec 0x7bc206)
//   0x007b39c3 _splitpath(path, drive, dir, fname, ext) STATEFUL locale/mbcs (_mbctype 0xa6cc21,
//              _mbsnbcpy 0x7b5b8c)
//   0x007b3b0a isalpha / 0x7b3b38 isupper / 0x7b3b60 islower      STATEFUL locale (_pctype
//   0x007b3b88 isdigit / 0x7b3bb0 isxdigit                         0xa30cc4, __mb_cur_max
//   0x007b3bdd isspace / 0x7b3c05 ispunct                          0xa30ed0, _isctype 0x7b7f51)
//   0x007b3d70 strncat(char*, const char*, size_t)  PURE - implemented
//
//  * module crt/b113_08 (all cdecl, plain `ret`):
//    - crt_ceil 0x007b31ec / crt_floor 0x007b32bb: std::ceil / std::floor (the original's frndint at 64-bit
//      precision in round-up / round-down mode, cw 0x1b3f / 0x173f, is exact, signed zeros included). +-inf
//      returns x; a NaN gives the quieted NaN and errno = EDOM (33): _handle_qnan1 / _except1 with the game's
//      masked exceptions (measured on the original, re/p1/crt). The original's inexact report (_except1 with
//      FP_P) fires only when the caller's control word unmasks the precision exception: never (Direct3D's
//      control word masks all exceptions).
//    - crt_strchr 0x007b34e0, crt_strncat 0x007b3d70: byte-wise C++; the originals scan aligned
//      dwords (reads never cross a page, written bytes identical - partial-word tails write
//      exactly the same bytes c0[,c1[,c2]],NUL).
//    - crt_bsearch 0x007b365b, crt_lfind 0x007b36e7: VC6 bsearch.c / lfind.c with 32-bit
//      unsigned pointer arithmetic (imul / jbe as in the original), *num re-read every
//      iteration in _lfind, compare called as cmp(key, element).
//
// Prologue note: none of the implemented roots has a thunk; patch_entry accepts any library entry
// in 0x765000..0x7d5000 that passes re/check_lib_entries.py (all six pass: no outside jump into
// entry+1..4; strchr's found-stub 0x7b34d0 lies BEFORE its entry and is untouched), so all six
// registrations are live.
// Review note (strncat / strchr): the originals read src / the scanned string in aligned dwords.
// For non-overlapping buffers the written bytes and results are identical to the byte loops here;
// only an (undefined-behaviour) strncat whose src overlaps the bytes being appended could differ.
// The 14 callers in joan.exe and our modules pass distinct buffers.
#include "../../callconv.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>

#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {
using u32 = std::uint32_t;
using Cmp = int(JOAN_CDECL*)(const void*, const void*);

JOAN_COUNTER(c_bsearch, "crt::bsearch");

// NaN argument of ceil / floor: the quieted NaN, errno = EDOM.
double nan_result(double x) {
    *static_cast<int*>(joan::orig::libc::_errno()) = 33;  // EDOM (crt/mt)
    std::uint64_t b;
    std::memcpy(&b, &x, 8);
    b |= 0x0008000000000000ull;
    std::memcpy(&x, &b, 8);
    return x;
}

// 0x007b31ec ceil(double)
double JOAN_CDECL crt_ceil(double x) { return x != x ? nan_result(x) : std::ceil(x); }

// 0x007b32bb floor(double)
double JOAN_CDECL crt_floor(double x) { return x != x ? nan_result(x) : std::floor(x); }

// 0x007b34e0 strchr: first byte equal to (char)c, the terminator included when c == 0.
static char* JOAN_CDECL crt_strchr(const char* s, int c) {
    const char ch = static_cast<char>(c);
    for (;; ++s) {
        if (*s == ch) return const_cast<char*>(s);
        if (*s == 0) return nullptr;
    }
}

// 0x007b3d70 strncat: append at most n chars of src, then a NUL unless src's NUL was copied.
static char* JOAN_CDECL crt_strncat(char* dst, const char* src, u32 n) {
    if (n != 0) {
        char* p = dst;
        while (*p != 0) ++p;
        for (; n != 0; --n) {
            const char ch = *src++;
            *p++ = ch;
            if (ch == 0) return dst;
        }
        *p = 0;
    }
    return dst;
}

// 0x007b365b bsearch (VC6 bsearch.c).
static void* JOAN_CDECL crt_bsearch(const void* key, const void* base, u32 num, u32 width, Cmp cmp) {
    c_bsearch.hit();
    std::uintptr_t lo = reinterpret_cast<std::uintptr_t>(base);
    std::uintptr_t hi = lo + (num - 1u) * width;
    while (lo <= hi) {
        const u32 half = num >> 1;
        if (half == 0) {
            if (num == 0) return nullptr;
            void* const p = reinterpret_cast<void*>(lo);
            return cmp(key, p) == 0 ? p : nullptr;
        }
        const u32 odd = num & 1u;
        const std::uintptr_t mid = lo + (odd != 0 ? half : half - 1u) * width;
        void* const mp = reinterpret_cast<void*>(mid);
        const int r = cmp(key, mp);
        if (r == 0) return mp;
        if (r < 0) {
            hi = mid - width;
            num = odd != 0 ? half : half - 1u;
        } else {
            lo = mid + width;
            num = half;
        }
    }
    return nullptr;
}

// 0x007b36e7 _lfind (VC6 lfind.c); *num is re-read on every iteration.
static void* JOAN_CDECL crt_lfind(const void* key, const void* base, const u32* num, u32 width, Cmp cmp) {
    std::uintptr_t p = reinterpret_cast<std::uintptr_t>(base);
    for (u32 i = 0; i < *num; ++i) {
        void* const e = reinterpret_cast<void*>(p);
        if (cmp(key, e) == 0) return e;
        p += width;
    }
    return nullptr;
}

#define B(name, addr, fn) JOAN_REPLACE("crt/b113_08", name, addr, fn)
B("crt::ceil", 0x007b31ec, &crt_ceil);
B("crt::floor", 0x007b32bb, &crt_floor);
B("crt::strchr", 0x007b34e0, &crt_strchr);
B("crt::bsearch", 0x007b365b, &crt_bsearch);
B("crt::_lfind", 0x007b36e7, &crt_lfind);
B("crt::strncat", 0x007b3d70, &crt_strncat);
#undef B

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::floor(double a0) -> double { return joan::bind::conv<double>(joan::bind::free<&::crt_floor>(a0)); }  // 0x7b32bb
auto joan::orig::libc::strchr(const char* a0, std::int32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_strchr>(a0, a1)); }  // 0x7b34e0
auto joan::orig::libc::strncat(const char* a0, const char* a1, std::uint32_t a2) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_strncat>(a0, a1, a2)); }  // 0x7b3d70
