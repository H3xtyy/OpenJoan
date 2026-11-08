// Original library sources (re/attribution.csv original_file, under lib/): crt/ismbbyte.c, crt/ismbslead.c,
//   crt/makepath.c, crt/mbbtype.c, crt/mbsbtype.c, crt/mbschr.c, crt/mbscmp.c, crt/mbscoll.c, crt/mbsdec.c,
//   crt/mbsicoll.c, crt/mbsinc.c, crt/mbslen.c, crt/mbslwr.c, crt/mbsnbcat.c, crt/mbsnbcnt.c, crt/mbsnbcpy.c,
//   crt/mbsnbico.c, crt/mbsncat.c, crt/mbsncmp.c, crt/mbsncoll.c, crt/mbsncpy.c, crt/mbsnextc.c, crt/mbsnicol.c,
//   crt/mbsrchr.c, crt/mbsstr.c, crt/mbsupr.c, crt/mbtoupr.c, crt/splitpat.c, crt/strcoll.c, crt/stricoll.c,
//   crt/strncoll.c, crt/strnicol.c, crt/wcscoll.c, crt/wcsicoll.c, crt/wcsncoll.c, crt/wcsnicol.c.
//
// Batch 114 / list 07: family crt/mbs - the MSVC 6 CRT multibyte-string (_mbs*) readers, the collation
// family (*coll) and _makepath / _splitpath (mbcs-aware path helpers).
//
// Surface replaced (all cdecl, plain `ret`, no thunks -> library-range entry patch; every entry passes
// re/check_lib_entries.py). Callers: TCHAR wrappers 0x58bad0..0x58e582 (C++ in game_b103_08/09/10, they
// call these addresses), game code (_makepath / _splitpath: 0x47cf60, 0x47d290, 0x5c0ef0, 0x5c2370,
// 0x604960, 0x6051c0, 0x605540 + en3d C++), and still-original CRT code (noted per line).
//   0x007b58e2 unsigned _mbsnextc(const uchar*)
//   0x007b5900 uchar* _mbsinc(const uchar*)
//   0x007b5917 size_t _mbslen(const uchar*)
//   0x007b5abf uchar* _mbsncpy(uchar*, const uchar*, size_t nchars)
//   0x007b5b8c uchar* _mbsnbcpy(uchar*, const uchar*, size_t nbytes)     (also from _splitpath)
//   0x007b5c26 uchar* _mbsncat(uchar*, const uchar*, size_t nchars)
//   0x007b5d1f uchar* _mbsnbcat(uchar*, const uchar*, size_t nbytes)
//   0x007b5de7 uchar* _mbschr(const uchar*, unsigned)                   (also from __crtsetenv 0x7c4b9a)
//   0x007b5ea7 uchar* _mbsrchr(const uchar*, unsigned)
//   0x007b5f59 uchar* _mbsstr(const uchar*, const uchar*)
//   0x007b6010 int _mbscmp(const uchar*, const uchar*)
//   0x007b60f4 int _mbsncmp(const uchar*, const uchar*, size_t)
//   0x007b61f4 int strcoll(const char*, const char*)
//   0x007b62c3 int _mbscoll(const uchar*, const uchar*)
//   0x007b62f3 int wcscoll(const wchar_t*, const wchar_t*)
//   0x007b63c2 int _strncoll(const char*, const char*, size_t)
//   0x007b64a3 int _mbsncoll(const uchar*, const uchar*, size_t)
//   0x007b64f8 int _wcsncoll(const wchar_t*, const wchar_t*, size_t)
//   0x007b65d9 int _stricoll(const char*, const char*)
//   0x007b66a8 int _mbsicoll(const uchar*, const uchar*)
//   0x007b66d8 int _wcsicoll(const wchar_t*, const wchar_t*)
//   0x007b67a7 int _strnicoll(const char*, const char*, size_t)
//   0x007b687b int _mbsnicoll(const uchar*, const uchar*, size_t)
//   0x007b68d0 int _wcsnicoll(const wchar_t*, const wchar_t*, size_t)
//   0x007b69b1 uchar* _mbslwr(uchar*)
//   0x007b6a3b uchar* _mbsupr(uchar*)
//   0x007b3932 void _makepath(char*, const char* drive, const char* dir, const char* fname, const char* ext)
//   0x007b39c3 void _splitpath(const char*, char* drive, char* dir, char* fname, char* ext)
//   0x007bc206 uchar* _mbsdec(const uchar* start, const uchar* cur)     (from _makepath)
//   0x007bd27e unsigned _mbctoupper(unsigned)                          (from _chdir 0x7b3ff3, always c <= 0xff)
//   0x007c01c1 int _mbsbtype(const uchar*, size_t)                     (from _mbsncat / _mbsnbcat)
//   0x007c020e int _ismbslead(const uchar* start, const uchar* cur)    (from _mbsncat)
//   0x007c075f size_t _mbsnbcnt(const uchar*, size_t)                  (from _mbsncoll / _mbsnicoll)
//   0x007c208f int _mbsnbicoll(const uchar*, const uchar*, size_t)     (from getenv 0x7b7ed4 / findenv 0x7c4d21)
//   0x007c48d0 int _mbbtype(uchar, int)                                (from _mbsbtype)
//   0x007c4a97 int _ismbblead(unsigned)                                (from _wincmdln 0x7c11bf)
// Not replaced: the other _ismbb* predicates 0x7c4a03..0x7c4ab9 and their helper x_ismbbtype 0x7c4ae0 (no
// caller in joan.exe, no address constant anywhere; _ismbblead was their only reachable member), _getmbcp
// 0x7b523d (no caller), and every writer of the tables (crt/locale: _setmbcp, setlocale, ...).
//
// State: read only, at the original addresses, exactly when the original reads it:
//   __ismbcodepage 0xa6cb1c (the "!_ISMBCP -> SBCS fallback" test), __mbcodepage 0xa6ca90, __mblcid
//   0xa6cd24, _mbctype 0xa6cc20 (indexed from 0xa6cc21: 4 = _M1 lead, 8 = _M2 trail, 0x10 _SBUP,
//   0x20 _SBLOW, 3 = kana bits), _mbcasemap 0xa6cb20, the static _ctype table 0xa30cce (_mbbtype),
//   __lc_handle[LC_COLLATE] 0xa6b2b4, __lc_codepage 0xa6b2c8, __lc_collate_cp 0xa6b2cc, setlocale guard
//   __setlc_active 0xa6ca7c / __unguarded_readlc_active 0xa6ca8c (InterlockedIncrement/Decrement through
//   the game's IAT slots 0xa6d930 / 0xa6d934, same order as the original).
// Calls other families only through their original addresses: _lock / _unlock 0x7b82aa / 0x7b830b
// (_MB_CP_LOCK 0x19, _SETLOCALE_LOCK 0x13), _errno 0x7bbfb5 (EINVAL on a CompareString failure),
// __crtCompareStringA 0x7c026c, __crtCompareStringW 0x7c0514, __crtLCMapStringA 0x7b7fc6, and the
// SBCS / C-locale fallbacks strlen 0x7b1ef0, strncpy 0x7b1db0, strncat 0x7b3d70, strchr 0x7b34e0, strrchr
// 0x7b2210, strstr 0x7b1780, strcmp 0x7b16f0, strncmp 0x7b29e0, wcscmp 0x7b60bf, wcsncmp 0x7b61bc,
// _stricmp 0x7c07b0, _strnicmp 0x7c0960, _wcsicmp 0x7c0880, _wcsnicmp 0x7c0a61. Functions of this family
// call each other directly (same module switch).
//
// Notes:
//  - _mbslwr / _mbsupr: the original passes its own argument slot as the 2-byte LCMapString output; only
//    the bytes LCMapString writes are used (cl always, ch only when it returned >= 2), so a local buffer is
//    equivalent. A return of 1 for a lead byte advances by one byte only (the trail byte is then processed
//    as a character), as in the original.
//  - _mbctoupper: for c > 0xff with a 1-byte LCMapString result the original reads one uninitialised stack
//    byte ([ebp-3], the caller's ecx). Unreachable: its only caller (_chdir) passes a zero-extended byte;
//    the byte is 0 here.
//  - _splitpath reproduces the original over-read after a lead byte followed by NUL (it skips the NUL).
//  - pointer comparisons are unsigned (uintptr_t) like the original jb/ja.
//
// Module: crt/mbs.
#include "../../callconv.hpp"
#include <cstddef>
#include <cstdint>

#include "crt.hpp"
#include "../../original/direct.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"

namespace {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using uptr = std::uintptr_t;
using wch = std::uint16_t;  // MSVC wchar_t

JOAN_COUNTER(c_mbsncat, "crt::_mbsncat");
JOAN_COUNTER(c_mbsnbcat, "crt::_mbsnbcat");
JOAN_COUNTER(c_mbslwr, "crt::_mbslwr");
JOAN_COUNTER(c_mbsupr, "crt::_mbsupr");
JOAN_COUNTER(c_makepath, "crt::_makepath");
JOAN_COUNTER(c_splitpath, "crt::_splitpath");
JOAN_COUNTER(c_coll, "crt::*coll (locale)");

using joan::img::global;

using joan::call::cdecl_call;
using Interlocked = std::int32_t(JOAN_STDCALL*)(std::int32_t*);  // LONG: 32 bits (long is 64 on LP64)

// ------------------------------------------------------------------ state / helpers
inline u32 ismbcp() { return global<volatile u32>(d::g_a6cb1c); }        // __ismbcodepage
inline u32 mbcodepage() { return global<volatile u32>(d::g_mbcodepage); }    // __mbcodepage
inline u32 mblcid() { return global<volatile u32>(d::g_mblcid); }        // __mblcid
inline u8 mbctype(u32 c) { return (&img::global<const volatile u8>(d::g_a6cc21))[c]; }
inline u8 mbcasemap(u32 c) { return (&img::global<const volatile u8>(d::g_a6cb20))[c]; }
inline bool lead(u8 c) { return (mbctype(c) & 4) != 0; }
inline void lock(int n) { joan::orig::libc::_lock(joan::orig::arg::i32(n)); }
inline void unlock(int n) { joan::orig::libc::_unlock(joan::orig::arg::i32(n)); }
constexpr int kMbCpLock = 0x19;
constexpr int kSetlocaleLock = 0x13;

inline u32 crt_strlen(const void* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
inline int compare_a(u32 lcid, u32 flags, const void* s1, int n1, const void* s2, int n2, u32 cp) {
    return joan::orig::arg::as<int>(joan::orig::libc::__crt_compare_string_a(lcid, flags, joan::orig::arg::str(s1), joan::orig::arg::i32(n1), joan::orig::arg::str(s2), joan::orig::arg::i32(n2), joan::orig::arg::i32(cp)));  // __crtCompareStringA
}
inline int compare_w(u32 lcid, u32 flags, const void* s1, int n1, const void* s2, int n2, u32 cp) {
    return joan::orig::arg::as<int>(joan::orig::libc::__crt_compare_string_w(lcid, flags, joan::orig::arg::mptr(s1), joan::orig::arg::i32(n1), joan::orig::arg::mptr(s2), joan::orig::arg::i32(n2), joan::orig::arg::i32(cp)));  // __crtCompareStringW
}
inline int lcmap_a(u32 lcid, u32 flags, const void* src, int ns, void* dst, int nd, u32 cp, int berr) {
    return joan::orig::arg::as<int>(joan::orig::libc::__crt_lcmap_string_a(lcid, flags, joan::orig::arg::str(src), joan::orig::arg::i32(ns), joan::orig::arg::str(dst), joan::orig::arg::i32(nd), joan::orig::arg::i32(cp), berr));  // __crtLCMapStringA
}

// ------------------------------------------------------------------ 0x007c48d0 _mbbtype
int JOAN_CDECL crt_mbbtype(u32 c, int ctype) {
    const u8 b = static_cast<u8>(c);
    if (ctype == 1) return (mbctype(b) & 8) ? 2 : -1;  // after a lead byte: _MBC_TRAIL / _MBC_ILLEGAL
    const u8 t = mbctype(b);
    if (t & 4) return 1;  // _MBC_LEAD
    const u16 ct = (&img::global<const volatile u16>(d::g_ctypeC))[b];  // static _ctype+1, not _pctype
    if ((ct & 0x157) != 0 || (t & 3) != 0) return 0;                       // _MBC_SINGLE
    return -1;                                                            // _MBC_ILLEGAL
}

// ------------------------------------------------------------------ 0x007c01c1 _mbsbtype
int JOAN_CDECL crt_mbsbtype(const u8* s, u32 len) {
    if (ismbcp() == 0) return 0;
    lock(kMbCpLock);
    int type = -1;
    for (;;) {
        const u8 c = *s;
        if (!c) {
            type = -1;
            break;
        }
        ++s;
        type = crt_mbbtype(c, type);
        if (len-- == 0) break;
    }
    unlock(kMbCpLock);
    return type;
}

// ------------------------------------------------------------------ 0x007c020e _ismbslead
int JOAN_CDECL crt_ismbslead(const u8* start, const u8* cur) {
    if (ismbcp() == 0) return 0;
    lock(kMbCpLock);
    const u8* p = start;
    while (reinterpret_cast<uptr>(p) <= reinterpret_cast<uptr>(cur) && *p) {
        if (lead(*p)) {
            const u8* const q = p++;
            if (q == cur) {
                unlock(kMbCpLock);
                return -1;
            }
            if (!*p) {
                unlock(kMbCpLock);
                return 0;
            }
        }
        ++p;
    }
    unlock(kMbCpLock);
    return 0;
}

// ------------------------------------------------------------------ 0x007c075f _mbsnbcnt
u32 JOAN_CDECL crt_mbsnbcnt(const u8* s, u32 n) {
    lock(kMbCpLock);
    const u8* p = s;
    while (n--) {
        const u8 c = *p;
        if (!c) break;
        if (lead(c)) {
            ++p;
            if (!*p) {
                --p;
                break;
            }
        }
        ++p;
    }
    unlock(kMbCpLock);
    return static_cast<u32>(p - s);
}

// ------------------------------------------------------------------ 0x007c4a97 _ismbblead
int JOAN_CDECL crt_ismbblead(u32 c) { return (mbctype(static_cast<u8>(c)) & 4) ? 1 : 0; }

// ------------------------------------------------------------------ 0x007b58e2 _mbsnextc
u32 JOAN_CDECL crt_mbsnextc(const u8* s) {
    u32 hi = 0;
    const u32 c = *s;
    if (lead(static_cast<u8>(c))) {
        hi = c << 8;
        ++s;
    }
    return *s + hi;
}

// ------------------------------------------------------------------ 0x007b5900 _mbsinc
const u8* JOAN_CDECL crt_mbsinc(const u8* s) { return lead(*s) ? s + 2 : s + 1; }

// ------------------------------------------------------------------ 0x007b5917 _mbslen
u32 JOAN_CDECL crt_mbslen(const u8* s) {
    if (ismbcp() == 0) return crt_strlen(s);
    lock(kMbCpLock);
    u32 n = 0;
    for (;;) {
        const u8 c = *s;
        if (!c) break;
        if (lead(c)) {
            ++s;
            if (!*s) break;
        }
        ++n;
        ++s;
    }
    unlock(kMbCpLock);
    return n;
}

// ------------------------------------------------------------------ 0x007bc206 _mbsdec
const u8* JOAN_CDECL crt_mbsdec(const u8* start, const u8* cur) {
    const uptr s = reinterpret_cast<uptr>(start);
    const uptr c = reinterpret_cast<uptr>(cur);
    if (s >= c) return nullptr;
    if (ismbcp() == 0) return reinterpret_cast<const u8*>(c - 1);
    lock(kMbCpLock);
    uptr p = c - 1;
    if (lead(*reinterpret_cast<const u8*>(p))) {
        unlock(kMbCpLock);
        return reinterpret_cast<const u8*>(p - 1);
    }
    do {
        --p;
    } while (s <= p && lead(*reinterpret_cast<const u8*>(p)));
    unlock(kMbCpLock);
    return reinterpret_cast<const u8*>(c - ((c - p) & 1) - 1);
}

// ------------------------------------------------------------------ 0x007b5abf _mbsncpy
u8* JOAN_CDECL crt_mbsncpy(u8* dst, const u8* src, u32 cnt) {
    if (ismbcp() == 0) return joan::orig::arg::as<u8*>(std::strncpy(static_cast<char*>(joan::orig::arg::mptr(dst)), joan::orig::arg::str(src), joan::orig::arg::u32(cnt)));  // strncpy
    lock(kMbCpLock);
    u8* d = dst;
    while (cnt) {
        u8 c = *src;
        --cnt;
        *d = c;
        if (lead(c)) {
            c = src[1];
            ++d;
            ++src;
            *d = c;
            ++d;
            ++src;
            if (!c) {
                d[-2] = 0;
                break;
            }
        } else {
            ++d;
            ++src;
            if (!c) break;
        }
    }
    while (cnt--) *d++ = 0;
    unlock(kMbCpLock);
    return dst;
}

// ------------------------------------------------------------------ 0x007b5b8c _mbsnbcpy
u8* JOAN_CDECL crt_mbsnbcpy(u8* dst, const u8* src, u32 cnt) {
    if (ismbcp() == 0) return joan::orig::arg::as<u8*>(std::strncpy(static_cast<char*>(joan::orig::arg::mptr(dst)), joan::orig::arg::str(src), joan::orig::arg::u32(cnt)));  // strncpy
    lock(kMbCpLock);
    u8* d = dst;
    while (cnt) {
        u8 c = *src;
        --cnt;
        *d = c;
        if (lead(c)) {
            ++d;
            ++src;
            if (!cnt) {
                d[-1] = 0;
                break;
            }
            c = *src;
            --cnt;
            *d = c;
            ++d;
            ++src;
            if (!c) {
                d[-2] = 0;
                break;
            }
        } else {
            ++d;
            ++src;
            if (!c) break;
        }
    }
    while (cnt--) *d++ = 0;
    unlock(kMbCpLock);
    return dst;
}

// ------------------------------------------------------------------ 0x007b5c26 _mbsncat
u8* JOAN_CDECL crt_mbsncat(u8* dst, const u8* src, u32 cnt) {
    c_mbsncat.hit();
    if (cnt == 0) return dst;
    if (ismbcp() == 0) return joan::orig::arg::as<u8*>(joan::orig::libc::strncat(joan::orig::arg::str(dst), joan::orig::arg::str(src), joan::orig::arg::u32(cnt)));  // strncat
    lock(kMbCpLock);
    u8* const start = dst;
    u8* d = dst;
    while (*d++) {
    }
    --d;
    if (crt_ismbslead(start, d)) --d;
    --cnt;
    for (;;) {
        u8 c = *src;
        *d = c;
        if (lead(c)) {
            c = src[1];
            ++d;
            ++src;
            *d = c;
            ++d;
            ++src;
            if (!c) {
                d[-2] = 0;
                break;
            }
        } else {
            ++d;
            ++src;
            if (!c) break;
        }
        if (cnt-- == 0) break;
    }
    if (crt_mbsbtype(start, static_cast<u32>(d - start) - 1) == 1)
        d[-1] = 0;
    else
        *d = 0;
    unlock(kMbCpLock);
    return start;
}

// ------------------------------------------------------------------ 0x007b5d1f _mbsnbcat
u8* JOAN_CDECL crt_mbsnbcat(u8* dst, const u8* src, u32 cnt) {
    c_mbsnbcat.hit();
    if (cnt == 0) return dst;
    if (ismbcp() == 0) return joan::orig::arg::as<u8*>(joan::orig::libc::strncat(joan::orig::arg::str(dst), joan::orig::arg::str(src), joan::orig::arg::u32(cnt)));  // strncat
    lock(kMbCpLock);
    u8* const start = dst;
    u8* d = dst;
    while (*d++) {
    }
    --d;
    if (crt_mbsbtype(start, static_cast<u32>(d - start) - 1) == 1) --d;
    --cnt;
    for (;;) {
        u8 c = *src;
        *d = c;
        if (lead(c)) {
            ++d;
            ++src;
            if (cnt-- == 0) {
                d[-1] = 0;
                break;
            }
            c = *src;
            *d = c;
            ++d;
            ++src;
            if (!c) {
                d[-2] = 0;
                break;
            }
        } else {
            ++d;
            ++src;
            if (!c) break;
        }
        if (cnt-- == 0) break;
    }
    if (crt_mbsbtype(start, static_cast<u32>(d - start) - 1) == 1)
        d[-1] = 0;
    else
        *d = 0;
    unlock(kMbCpLock);
    return start;
}

// ------------------------------------------------------------------ 0x007b5de7 _mbschr
const u8* JOAN_CDECL crt_mbschr(const u8* s, u32 ch) {
    if (ismbcp() == 0) return joan::orig::arg::as<const u8*>(joan::orig::libc::strchr(joan::orig::arg::str(s), joan::orig::arg::i32(ch)));  // strchr
    lock(kMbCpLock);
    u32 x;
    for (;;) {
        x = *s;
        if (!x) break;
        if (lead(static_cast<u8>(x))) {
            const u8 t = s[1];
            ++s;
            if (!t) {
                unlock(kMbCpLock);
                return nullptr;
            }
            if (ch == ((x << 8) | t)) {
                unlock(kMbCpLock);
                return s - 1;
            }
        } else if (ch == x) {
            break;
        }
        ++s;
    }
    unlock(kMbCpLock);
    return ch == x ? s : nullptr;
}

// ------------------------------------------------------------------ 0x007b5ea7 _mbsrchr
const u8* JOAN_CDECL crt_mbsrchr(const u8* s, u32 ch) {
    if (ismbcp() == 0) return joan::orig::arg::as<const u8*>(joan::orig::libc::strrchr(joan::orig::arg::str(s), joan::orig::arg::i32(ch)));  // strrchr
    lock(kMbCpLock);
    const u8* r = nullptr;
    u8 c;
    do {
        c = *s;
        const u32 v = c;
        if (lead(c)) {
            c = s[1];
            ++s;
            if (c) {
                if (ch == ((v << 8) | c)) r = s - 1;
            } else if (!r) {
                r = s;  // pointer to the '\0' after a dangling lead byte
            }
        } else if (ch == v) {
            r = s;
        }
        ++s;
    } while (c);
    unlock(kMbCpLock);
    return r;
}

// ------------------------------------------------------------------ 0x007b5f59 _mbsstr (no lock)
const u8* JOAN_CDECL crt_mbsstr(const u8* s1, const u8* s2) {
    if (ismbcp() == 0) return joan::orig::arg::as<const u8*>(joan::orig::libc::strstr(joan::orig::arg::str(s1), joan::orig::arg::str(s2)));  // strstr
    const u32 len2 = crt_strlen(s2);
    const uptr end = reinterpret_cast<uptr>(s1) - len2 + crt_strlen(s1);
    const u8* p = s1;
    while (*p && reinterpret_cast<uptr>(p) <= end) {
        const u8* q = s2;
        for (;;) {
            const u8 c = *q;
            if (!c || p[q - s2] != c) break;
            ++q;
            if (!p[q - s2]) break;
        }
        if (!*q) return p;
        p = crt_mbsinc(p);
    }
    return nullptr;
}

// ------------------------------------------------------------------ 0x007b6010 _mbscmp / 0x007b60f4 _mbsncmp
// Reads one (possibly double-byte) character; a lead byte followed by NUL reads as 0 (NUL not consumed).
inline u16 next_char(const u8*& s) {
    u16 c = *s++;
    if (lead(static_cast<u8>(c))) {
        const u8 t = *s;
        if (!t)
            c = 0;
        else {
            ++s;
            c = static_cast<u16>((c << 8) | t);
        }
    }
    return c;
}

int JOAN_CDECL crt_mbscmp(const u8* s1, const u8* s2) {
    if (ismbcp() == 0) return std::strcmp(joan::orig::arg::str(s1), joan::orig::arg::str(s2));  // strcmp
    lock(kMbCpLock);
    for (;;) {
        const u16 c1 = next_char(s1);
        const u16 c2 = next_char(s2);
        if (c1 != c2) {
            unlock(kMbCpLock);
            return c2 < c1 ? 1 : -1;
        }
        if (c1 == 0) break;
    }
    unlock(kMbCpLock);
    return 0;
}

int JOAN_CDECL crt_mbsncmp(const u8* s1, const u8* s2, u32 n) {
    if (n == 0) return 0;
    if (ismbcp() == 0) return joan::orig::arg::as<int>(joan::orig::libc::strncmp(joan::orig::arg::str(s1), joan::orig::arg::str(s2), joan::orig::arg::u32(n)));  // strncmp
    lock(kMbCpLock);
    --n;
    for (;;) {
        const u16 c1 = next_char(s1);
        const u16 c2 = next_char(s2);
        if (c1 != c2) {
            unlock(kMbCpLock);
            return c2 < c1 ? 1 : -1;
        }
        if (c1 == 0) break;
        if (n-- == 0) break;
    }
    unlock(kMbCpLock);
    return 0;
}

// ------------------------------------------------------------------ str/wcs *coll (LC_COLLATE)
// The VC6 guarded locale read: optional unguarded pre-check of __lc_handle[LC_COLLATE] (all but
// _strnicoll), InterlockedIncrement(&__unguarded_readlc_active); if setlocale is active, decrement again and
// take _SETLOCALE_LOCK; re-read the handle: 0 -> release + C-locale fallback; else CompareString (errno
// EINVAL + _NLSCMPERROR on failure, after the release), result - 2.
template <class Fallback, class Nls>
int guarded_coll(bool precheck, Fallback fallback, Nls nls) {
    c_coll.hit();
    if (precheck && global<volatile u32>(d::g_a6b2b4) == 0) return fallback();
    std::int32_t* const unguarded = &img::global<std::int32_t>(d::g_unguarded_readlc_active);
    reinterpret_cast<Interlocked>(global<uptr>(d::iat_KERNEL32_InterlockedIncrement))(unguarded);  // InterlockedIncrement
    const auto decrement = reinterpret_cast<Interlocked>(global<uptr>(d::iat_KERNEL32_InterlockedDecrement));
    bool locked = false;
    if (global<volatile u32>(d::g_setlc_active) != 0) {
        decrement(unguarded);
        lock(kSetlocaleLock);
        locked = true;
    }
    const u32 handle = global<volatile u32>(d::g_a6b2b4);
    const auto release = [&] {
        if (locked)
            unlock(kSetlocaleLock);
        else
            decrement(unguarded);
    };
    if (handle == 0) {
        release();
        return fallback();
    }
    const int r = nls(handle);
    if (r == 0) {
        release();
        *joan::orig::arg::as<int*>(joan::orig::libc::_errno()) = 0x16;  // *_errno() = EINVAL
        return 0x7fffffff;                     // _NLSCMPERROR
    }
    release();
    return r - 2;
}
inline u32 collate_cp() { return global<volatile u32>(d::g_a6b2cc); }  // __lc_collate_cp

int JOAN_CDECL crt_strcoll(const char* a, const char* b) {
    return guarded_coll(
        true, [&] { return std::strcmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); },
        [&](u32 h) { return compare_a(h, 0x1000, a, -1, b, -1, collate_cp()); });
}
int JOAN_CDECL crt_wcscoll(const wch* a, const wch* b) {
    return guarded_coll(
        true, [&] { return joan::orig::arg::as<int>(joan::orig::libc::wcscmp(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b))); },
        [&](u32 h) { return compare_w(h, 0x1000, a, -1, b, -1, collate_cp()); });
}
int JOAN_CDECL crt_strncoll(const char* a, const char* b, u32 n) {
    if (n == 0) return 0;
    return guarded_coll(
        true, [&] { return joan::orig::arg::as<int>(joan::orig::libc::strncmp(joan::orig::arg::str(a), joan::orig::arg::str(b), joan::orig::arg::u32(n))); },
        [&](u32 h) {
            return compare_a(h, 0x1000, a, static_cast<int>(n), b, static_cast<int>(n), collate_cp());
        });
}
int JOAN_CDECL crt_wcsncoll(const wch* a, const wch* b, u32 n) {
    if (n == 0) return 0;
    return guarded_coll(
        true, [&] { return joan::orig::arg::as<int>(joan::orig::libc::wcsncmp(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), n)); },
        [&](u32 h) {
            return compare_w(h, 0x1000, a, static_cast<int>(n), b, static_cast<int>(n), collate_cp());
        });
}
int JOAN_CDECL crt_stricoll(const char* a, const char* b) {
    return guarded_coll(
        true, [&] { return joan::crt::stricmp(a, b); },
        [&](u32 h) { return compare_a(h, 0x1001, a, -1, b, -1, collate_cp()); });
}
int JOAN_CDECL crt_wcsicoll(const wch* a, const wch* b) {
    // passes __lc_codepage 0xa6b2c8 (not __lc_collate_cp) like the original
    return guarded_coll(
        true, [&] { return joan::orig::arg::as<int>(joan::orig::libc::_wcsicmp(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b))); },
        [&](u32 h) { return compare_w(h, 0x1001, a, -1, b, -1, global<volatile u32>(d::g_lc_codepage)); });
}
int JOAN_CDECL crt_strnicoll(const char* a, const char* b, u32 n) {
    if (n == 0) return 0;
    return guarded_coll(
        false, [&] { return joan::orig::arg::as<int>(joan::orig::libc::_strnicmp(joan::orig::arg::str(a), joan::orig::arg::str(b), n)); },
        [&](u32 h) {
            return compare_a(h, 0x1001, a, static_cast<int>(n), b, static_cast<int>(n), collate_cp());
        });
}
int JOAN_CDECL crt_wcsnicoll(const wch* a, const wch* b, u32 n) {
    if (n == 0) return 0;
    return guarded_coll(
        true, [&] { return joan::orig::arg::as<int>(joan::orig::libc::_wcsnicmp(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), n)); },
        [&](u32 h) {
            return compare_w(h, 0x1001, a, static_cast<int>(n), b, static_cast<int>(n), collate_cp());
        });
}

// ------------------------------------------------------------------ _mbs*coll (code-page lcid, no lock)
inline int nls_result(int r) { return r == 0 ? 0x7fffffff : r - 2; }

int JOAN_CDECL crt_mbscoll(const u8* a, const u8* b) {
    return nls_result(compare_a(mblcid(), 0, a, -1, b, -1, mbcodepage()));
}
int JOAN_CDECL crt_mbsicoll(const u8* a, const u8* b) {
    return nls_result(compare_a(mblcid(), 1, a, -1, b, -1, mbcodepage()));
}
int JOAN_CDECL crt_mbsncoll(const u8* a, const u8* b, u32 n) {
    if (n == 0) return 0;
    const u32 na = crt_mbsnbcnt(a, n);
    const u32 nb = crt_mbsnbcnt(b, n);
    return nls_result(
        compare_a(mblcid(), 0, a, static_cast<int>(na), b, static_cast<int>(nb), mbcodepage()));
}
int JOAN_CDECL crt_mbsnicoll(const u8* a, const u8* b, u32 n) {
    if (n == 0) return 0;
    const u32 na = crt_mbsnbcnt(a, n);
    const u32 nb = crt_mbsnbcnt(b, n);
    return nls_result(
        compare_a(mblcid(), 1, a, static_cast<int>(na), b, static_cast<int>(nb), mbcodepage()));
}
int JOAN_CDECL crt_mbsnbicoll(const u8* a, const u8* b, u32 n) {
    if (n == 0) return 0;
    return nls_result(
        compare_a(mblcid(), 1, a, static_cast<int>(n), b, static_cast<int>(n), mbcodepage()));
}

// ------------------------------------------------------------------ 0x007b69b1 _mbslwr / 0x007b6a3b _mbsupr
// map_flags: LCMAP_LOWERCASE 0x100 / LCMAP_UPPERCASE 0x200; sb_bit: _SBUP 0x10 / _SBLOW 0x20.
u8* mbs_case(u8* s, u32 map_flags, u8 sb_bit) {
    lock(kMbCpLock);
    u8* p = s;
    if (*s) {
        do {
            const u32 c = *p;
            const u8 t = mbctype(c);
            if (t & 4) {
                u8 out[2] = {0, 0};
                const int r = lcmap_a(mblcid(), map_flags, p, 2, out, 2, mbcodepage(), 1);
                if (r == 0) {
                    unlock(kMbCpLock);
                    return nullptr;
                }
                *p = out[0];
                if (r > 1) {
                    ++p;
                    *p = out[1];
                }
            } else {
                *p = (t & sb_bit) == sb_bit ? mbcasemap(c) : static_cast<u8>(c);
            }
            ++p;
        } while (*p);
    }
    unlock(kMbCpLock);
    return s;
}
u8* JOAN_CDECL crt_mbslwr(u8* s) {
    c_mbslwr.hit();
    return mbs_case(s, 0x100, 0x10);
}
u8* JOAN_CDECL crt_mbsupr(u8* s) {
    c_mbsupr.hit();
    return mbs_case(s, 0x200, 0x20);
}

// ------------------------------------------------------------------ 0x007bd27e _mbctoupper
u32 JOAN_CDECL crt_mbctoupper(u32 c) {
    if (c > 0xff) {
        const u8 src[2] = {static_cast<u8>(c >> 8), static_cast<u8>(c)};
        if (lead(src[0])) {
            u8 out[2] = {0, 0};  // out[1]: uninitialised in the original if LCMapString returns 1 (unreachable)
            if (lcmap_a(mblcid(), 0x200, src, 2, out, 2, mbcodepage(), 1) != 0)
                return (static_cast<u32>(out[0]) << 8) + out[1];
        }
        return c;
    }
    if ((mbctype(c) & 0x20) == 0x20) return mbcasemap(c);
    return c;
}

// ------------------------------------------------------------------ 0x007b3932 _makepath
void JOAN_CDECL crt_makepath(u8* path, const u8* drive, const u8* dir, const u8* fname, const u8* ext) {
    c_makepath.hit();
    u8* p = path;
    if (drive && *drive) {
        *p++ = *drive;
        *p++ = ':';
    }
    if (dir && *dir) {
        const u8* q = dir;
        u8 c = *q;
        do {
            *p++ = c;
            c = *++q;
        } while (c);
        const u8 last = *crt_mbsdec(dir, q);
        if (last != '/' && last != '\\') *p++ = '\\';
    }
    if (fname) {
        while (*fname) *p++ = *fname++;
    }
    if (ext) {
        if (*ext && *ext != '.') *p++ = '.';
        u8 c;
        do {
            c = *ext++;
            *p++ = c;
        } while (c);
    } else {
        *p = 0;
    }
}

// ------------------------------------------------------------------ 0x007b39c3 _splitpath
void JOAN_CDECL crt_splitpath(const u8* path, u8* drive, u8* dir, u8* fname, u8* ext) {
    c_splitpath.hit();
    const u8* dot = nullptr;
    if (crt_strlen(path) >= 1 && path[1] == ':') {
        if (drive) {
            crt_mbsnbcpy(drive, path, 2);
            drive[2] = 0;
        }
        path += 2;
    } else if (drive) {
        *drive = 0;
    }
    const u8* last_slash = nullptr;
    const u8* p = path;
    bool split_dir = false;
    if (*path) {
        do {
            const u8 c = *p;
            if (lead(c))
                ++p;
            else if (c == '/' || c == '\\')
                last_slash = p + 1;
            else if (c == '.')
                dot = p;
            ++p;
        } while (*p);
        if (last_slash) {
            if (dir) {
                u32 n = static_cast<u32>(last_slash - path);
                if (n >= 0xff) n = 0xff;
                crt_mbsnbcpy(dir, path, n);
                dir[n] = 0;
            }
            path = last_slash;
            split_dir = true;
        }
    }
    if (!split_dir && dir) *dir = 0;
    const u8* const end = p;
    if (dot && reinterpret_cast<uptr>(dot) >= reinterpret_cast<uptr>(path)) {
        if (fname) {
            u32 n = static_cast<u32>(dot - path);
            if (n >= 0xff) n = 0xff;
            crt_mbsnbcpy(fname, path, n);
            fname[n] = 0;
        }
        if (ext) {
            u32 n = static_cast<u32>(end - dot);
            if (n >= 0xff) n = 0xff;
            crt_mbsnbcpy(ext, dot, n);
            ext[n] = 0;
        }
    } else {
        if (fname) {
            u32 n = static_cast<u32>(end - path);
            if (n >= 0xff) n = 0xff;
            crt_mbsnbcpy(fname, path, n);
            fname[n] = 0;
        }
        if (ext) *ext = 0;
    }
}

#define L(name, addr, fn) JOAN_REPLACE("crt/mbs", name, addr, fn)
L("crt::_makepath", 0x007b3932, &crt_makepath);
L("crt::_splitpath", 0x007b39c3, &crt_splitpath);
L("crt::_mbsnextc", 0x007b58e2, &crt_mbsnextc);
L("crt::_mbsinc", 0x007b5900, &crt_mbsinc);
L("crt::_mbslen", 0x007b5917, &crt_mbslen);
L("crt::_mbsncpy", 0x007b5abf, &crt_mbsncpy);
L("crt::_mbsnbcpy", 0x007b5b8c, &crt_mbsnbcpy);
L("crt::_mbsncat", 0x007b5c26, &crt_mbsncat);
L("crt::_mbsnbcat", 0x007b5d1f, &crt_mbsnbcat);
L("crt::_mbschr", 0x007b5de7, &crt_mbschr);
L("crt::_mbsrchr", 0x007b5ea7, &crt_mbsrchr);
L("crt::_mbsstr", 0x007b5f59, &crt_mbsstr);
L("crt::_mbscmp", 0x007b6010, &crt_mbscmp);
L("crt::_mbsncmp", 0x007b60f4, &crt_mbsncmp);
L("crt::strcoll", 0x007b61f4, &crt_strcoll);
L("crt::_mbscoll", 0x007b62c3, &crt_mbscoll);
L("crt::wcscoll", 0x007b62f3, &crt_wcscoll);
L("crt::_strncoll", 0x007b63c2, &crt_strncoll);
L("crt::_mbsncoll", 0x007b64a3, &crt_mbsncoll);
L("crt::_wcsncoll", 0x007b64f8, &crt_wcsncoll);
L("crt::_stricoll", 0x007b65d9, &crt_stricoll);
L("crt::_mbsicoll", 0x007b66a8, &crt_mbsicoll);
L("crt::_wcsicoll", 0x007b66d8, &crt_wcsicoll);
L("crt::_strnicoll", 0x007b67a7, &crt_strnicoll);
L("crt::_mbsnicoll", 0x007b687b, &crt_mbsnicoll);
L("crt::_wcsnicoll", 0x007b68d0, &crt_wcsnicoll);
L("crt::_mbslwr", 0x007b69b1, &crt_mbslwr);
L("crt::_mbsupr", 0x007b6a3b, &crt_mbsupr);
L("crt::_mbsdec", 0x007bc206, &crt_mbsdec);
L("crt::_mbctoupper", 0x007bd27e, &crt_mbctoupper);
L("crt::_mbsbtype", 0x007c01c1, &crt_mbsbtype);
L("crt::_ismbslead", 0x007c020e, &crt_ismbslead);
L("crt::_mbsnbcnt", 0x007c075f, &crt_mbsnbcnt);
L("crt::_mbsnbicoll", 0x007c208f, &crt_mbsnbicoll);
L("crt::_mbbtype", 0x007c48d0, &crt_mbbtype);
L("crt::_ismbblead", 0x007c4a97, &crt_ismbblead);
#undef L

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::_makepath(void* a0, void* a1, void* a2, void* a3, void* a4) -> void { joan::bind::free<&::crt_makepath>(a0, a1, a2, a3, a4); }  // 0x7b3932
auto joan::orig::libc::_splitpath(void* a0, void* a1, void* a2, void* a3, void* a4) -> void { joan::bind::free<&::crt_splitpath>(a0, a1, a2, a3, a4); }  // 0x7b39c3
auto joan::orig::libc::_mbsnextc(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::crt_mbsnextc>(a0)); }  // 0x7b58e2
auto joan::orig::libc::_mbsinc(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_mbsinc>(a0)); }  // 0x7b5900
auto joan::orig::libc::_mbslen(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::crt_mbslen>(a0)); }  // 0x7b5917
auto joan::orig::libc::_mbslwr(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_mbslwr>(a0)); }  // 0x7b69b1
auto joan::orig::libc::_mbsupr(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_mbsupr>(a0)); }  // 0x7b6a3b
auto joan::orig::libc::_mbctoupper(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::crt_mbctoupper>(a0)); }  // 0x7bd27e
auto joan::orig::libc::_mbsnbicoll(void* a0, void* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_mbsnbicoll>(a0, a1, a2)); }  // 0x7c208f
auto joan::orig::libc::_ismbblead(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_ismbblead>(a0)); }  // 0x7c4a97
