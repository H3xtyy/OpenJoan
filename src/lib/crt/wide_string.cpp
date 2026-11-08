// Original library sources (re/attribution.csv original_file, under lib/): crt/mbscat.c, crt/mbscpy.c, crt/wcscat.c,
//   crt/wcschr.c, crt/wcslen.c, crt/wcsncat.c, crt/wcsncpy.c.
//
// Batch 113 / list 10: MSVC 6 C runtime, 0x007b4e65..0x007b5f18 - the MBCS code-page set-up
// (mbctype.c), setlocale (setlocale.c) and the _mbs* / wcs* string functions linked after them.
//
// Identification (VC6 CRT sources; lock 0x19 = _MB_CP_LOCK, lock 0x13 = _SETLOCALE_LOCK,
// _lock 0x007b82aa / _unlock 0x007b830b; __ismbcodepage = [0x00a6cb1c], _mbctype = 0x00a6cc20
// (table read as _mbctype + 1 = 0x00a6cc21, _M1 lead-byte bit 0x04)):
//   0x007b4e65 int _setmbcp(int cp)                        STATEFUL (mbcs/locale) - left
//   0x007b5269 char* setlocale(int cat, const char* loc)   STATEFUL (locale)      - left
//   0x007b58e2 unsigned _mbsnextc(const uchar*)            reads _mbctype         - left
//   0x007b5900 uchar* _mbsinc(const uchar*)                reads _mbctype         - left
//   0x007b5917 size_t _mbslen(const uchar*)                mbcs lock + _mbctype   - left
//   0x007b5962 size_t wcslen(const wchar_t*)               PURE  - replaced here
//   0x007b5980 uchar* _mbscpy(uchar*, const uchar*)        PURE  - replaced here (strcpy.asm copy)
//   0x007b5990 uchar* _mbscat(uchar*, const uchar*)        PURE  - replaced here (strcat.asm copy)
//   0x007b5a70 wchar_t* wcscat(wchar_t*, const wchar_t*)   PURE  - replaced here
//   0x007b5a9a wchar_t* wcscpy(wchar_t*, const wchar_t*)   PURE  - replaced here
//   0x007b5abf uchar* _mbsncpy(uchar*, const uchar*, size_t)  mbcs lock + _mbctype - left
//   0x007b5b4f wchar_t* wcsncpy(wchar_t*, const wchar_t*, size_t) PURE - replaced here
//   0x007b5b8c uchar* _mbsnbcpy(uchar*, const uchar*, size_t) mbcs lock + _mbctype - left
//   0x007b5c26 uchar* _mbsncat(uchar*, const uchar*, size_t)  mbcs lock + _mbctype - left
//   0x007b5cdf wchar_t* wcsncat(wchar_t*, const wchar_t*, size_t) PURE - replaced here
//   0x007b5d1f uchar* _mbsnbcat(uchar*, const uchar*, size_t) mbcs lock + _mbctype - left
//   0x007b5de7 uchar* _mbschr(const uchar*, unsigned)      mbcs lock + _mbctype  - left
//   0x007b5e7e wchar_t* wcschr(const wchar_t*, wchar_t)    PURE  - replaced here
//   0x007b5ea7 uchar* _mbsrchr(const uchar*, unsigned)     mbcs lock + _mbctype  - left
// Full table (context functions, globals touched) in scratchpad w113_10/ids.md.
//
// The pure functions are portable C++ that performs the original's memory accesses in the same order and width
// (UTF-16 elements as std::uint16_t - not the host wchar_t, which is 4 bytes on Linux; _mbscpy / _mbscat copy
// the same aligned source dwords and store them as whole dwords, words or bytes exactly like the original, so
// even overlapping buffers end up with the same bytes; wcsncpy's zero fill writes the same elements). Results
// as the original's eax. None of them touches any global. Proof: re/p1/crt (i386 harness against the original
// code at its address).
//
// INSTALL NOTE: library functions without incremental-link thunks; installed by a 5-byte entry
// jmp (patch_entry accepts any entry in 0x765000..0x7d5000 that passes
// re/check_lib_entries.py). wcschr 0x007b5e7e has its loop head at entry+4 (0x007b5e82), but
// the only jump there is wcschr's own back-edge (0x007b5e93), which dies with the replaced
// body - checked: no other code in joan.exe targets 0x007b5e7f..0x007b5e82. _mbscpy's
// original stub jumps into _mbscat's body (0x007b59f1), far past _mbscat's patched 5 bytes.
//
// Modules: crt/b113_10.
#include "../../callconv.hpp"
#include "../../platform/win32_types.hpp"
#include <cstdint>
#include <cstring>

#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

// UTF-16 element views of the game's wchar_t strings (2 bytes in joan.exe on every host).
u16* w16(WCHAR* p) { return reinterpret_cast<u16*>(p); }
const u16* w16(const WCHAR* p) { return reinterpret_cast<const u16*>(p); }

u32 load32(const u8* p) {
    u32 v;
    std::memcpy(&v, p, 4);
    return v;
}

// VC6 strcpy.asm / strcat.asm copy loop (shared by _mbscpy and _mbscat): bytes until the source is dword
// aligned, then whole aligned source dwords (the read may cover up to 3 bytes after the terminator, inside the
// same aligned dword, as in the original); the dword with the terminator is stored as byte / word / word + NUL /
// dword like the original's tail cases.
void copy_dwords(u8* d, const u8* s) {
    while ((reinterpret_cast<std::uintptr_t>(s) & 3u) != 0) {
        const u8 c = *s++;
        *d++ = c;
        if (c == 0) return;
    }
    for (;;) {
        const u32 w = load32(s);
        s += 4;
        if ((w & 0xffu) == 0) {
            d[0] = 0;
            return;
        }
        if ((w & 0xff00u) == 0) {
            std::memcpy(d, &w, 2);
            return;
        }
        if ((w & 0xff0000u) == 0) {
            std::memcpy(d, &w, 2);
            d[2] = 0;
            return;
        }
        std::memcpy(d, &w, 4);
        if ((w & 0xff000000u) == 0) return;
        d += 4;
    }
}

// ------------------------------------------------------------------ 0x007b5962 wcslen
std::uint32_t JOAN_CDECL crt_wcslen(const WCHAR* s) {
    const u16* p = w16(s);
    u32 n = 0;
    while (p[n] != 0) ++n;
    return n;
}

// ------------------------------------------------------------------ 0x007b5980 _mbscpy
// (VC6 strcat.asm assembled a second time under the _mbscpy / _mbscat names.)
unsigned char* JOAN_CDECL crt_mbscpy(unsigned char* dst, const unsigned char* src) {
    copy_dwords(dst, src);
    return dst;
}

// ------------------------------------------------------------------ 0x007b5990 _mbscat
// The end of dst is found by reads only (bytes until aligned, then aligned dwords: the first NUL byte).
unsigned char* JOAN_CDECL crt_mbscat(unsigned char* dst, const unsigned char* src) {
    u8* d = dst;
    while (*d != 0) ++d;
    copy_dwords(d, src);
    return dst;
}

// ------------------------------------------------------------------ 0x007b5a70 wcscat
WCHAR* JOAN_CDECL crt_wcscat(WCHAR* dst, const WCHAR* src) {
    u16* d = w16(dst);
    const u16* s = w16(src);
    while (*d != 0) ++d;
    u16 c;
    do {
        c = *s++;
        *d++ = c;
    } while (c != 0);
    return dst;
}

// ------------------------------------------------------------------ 0x007b5a9a wcscpy
WCHAR* JOAN_CDECL crt_wcscpy(WCHAR* dst, const WCHAR* src) {
    u16* d = w16(dst);
    const u16* s = w16(src);
    u16 c;
    do {
        c = *s++;
        *d++ = c;
    } while (c != 0);
    return dst;
}

// ------------------------------------------------------------------ 0x007b5b4f wcsncpy
// Copies up to n elements; after a copied terminator the rest of the n elements are zeroed.
WCHAR* JOAN_CDECL crt_wcsncpy(WCHAR* dst, const WCHAR* src, std::uint32_t n) {
    u16* d = w16(dst);
    const u16* s = w16(src);
    for (; n != 0; --n) {
        const u16 c = *s++;
        *d++ = c;
        if (c == 0) {
            for (--n; n != 0; --n) *d++ = 0;
            break;
        }
    }
    return dst;
}

// ------------------------------------------------------------------ 0x007b5cdf wcsncat
// Appends up to n elements; a terminator is written after them unless the source's was copied.
WCHAR* JOAN_CDECL crt_wcsncat(WCHAR* dst, const WCHAR* src, std::uint32_t n) {
    u16* d = w16(dst);
    const u16* s = w16(src);
    while (*d != 0) ++d;
    for (; n != 0; --n) {
        const u16 c = *s++;
        *d++ = c;
        if (c == 0) return dst;
    }
    *d = 0;
    return dst;
}

// ------------------------------------------------------------------ 0x007b5e7e wcschr
// First element equal to the low 16 bits of c (the terminator itself when c is 0), else null.
WCHAR* JOAN_CDECL crt_wcschr(const WCHAR* s, WCHAR c) {
    const u16 ch = static_cast<u16>(c);
    const u16* p = w16(s);
    while (*p != 0 && *p != ch) ++p;
    return *p == ch ? const_cast<WCHAR*>(reinterpret_cast<const WCHAR*>(p)) : nullptr;
}

}  // namespace

#define L(name, addr, fn) JOAN_REPLACE("crt/b113_10", name, addr, fn)
L("crt::wcslen", 0x007b5962, &crt_wcslen);
L("crt::_mbscpy", 0x007b5980, &crt_mbscpy);
L("crt::_mbscat", 0x007b5990, &crt_mbscat);
L("crt::wcscat", 0x007b5a70, &crt_wcscat);
L("crt::wcscpy", 0x007b5a9a, &crt_wcscpy);
L("crt::wcsncpy", 0x007b5b4f, &crt_wcsncpy);
L("crt::wcsncat", 0x007b5cdf, &crt_wcsncat);
L("crt::wcschr", 0x007b5e7e, &crt_wcschr);
#undef L

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::wcslen(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::crt_wcslen>(a0)); }  // 0x7b5962
auto joan::orig::libc::_mbscpy(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_mbscpy>(a0, a1)); }  // 0x7b5980
auto joan::orig::libc::wcscpy(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_wcscpy>(a0, a1)); }  // 0x7b5a9a
