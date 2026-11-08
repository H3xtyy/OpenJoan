// Original library sources (re/attribution.csv original_file, under lib/): crt/wcscmp.c, crt/wcsncmp.c,
//   crt/wcsrchr.c, crt/wcsstr.c.
//
// MSVC 6 C runtime, 0x007b5f19..0x007b68d0: wide-string compare/search and the *coll
// (locale collation) family. Batch 113 / list 11.
//
// Identification (all cdecl, plain `ret`):
//   0x007b5f19 wcsrchr(const wchar_t*, wchar_t)            PURE   implemented
//   0x007b5f59 _mbsstr(const uchar*, const uchar*)          locale (mbcs)        - left original
//   0x007b5fcf wcsstr(const wchar_t*, const wchar_t*)      PURE   implemented
//   0x007b6010 _mbscmp                                     locale (mbcs)        - left original
//   0x007b60bf wcscmp(const wchar_t*, const wchar_t*)      PURE   implemented
//   0x007b60f4 _mbsncmp                                    locale (mbcs)        - left original
//   0x007b61bc wcsncmp(const wchar_t*, const wchar_t*, size_t) PURE implemented
//   0x007b61f4 strcoll, 0x007b62c3 _mbscoll, 0x007b62f3 wcscoll, 0x007b63c2 _strncoll,
//   0x007b64a3 _mbsncoll, 0x007b64f8 _wcsncoll, 0x007b65d9 _stricoll, 0x007b66a8 _mbsicoll,
//   0x007b66d8 _wcsicoll, 0x007b67a7 _strnicoll, 0x007b687b _mbsnicoll
//                                                           locale (collate)     - left original
//   (details and globals: scratchpad w113_11/ids.md)
//
// The four wide-string functions only read the caller's strings. They are written like the VC6
// sources (wcsrchr.c / wcsstr.c / wcscmp.c / wcsncmp.c); every comparison is on unsigned 16-bit
// units (movzx), wcscmp returns -1/0/1, wcsncmp the raw difference.
//
// Install note: none of these has an incremental-link thunk; patch_entry installs them by entry
// patch (library range 0x765000..0x7d5000, verified safe by re/check_lib_entries.py).
//
// Module: crt/b113_11.
#include "../../callconv.hpp"
#include <cstddef>
#include <cstdint>

#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {

using wch = std::uint16_t;  // MSVC wchar_t

// 0x007b5f19 wcsrchr: only the low word of the char argument is used (mov si,[esp+0xc]).
const wch* JOAN_CDECL crt_wcsrchr(const wch* string, std::uint32_t ch) {
    const wch c = static_cast<wch>(ch);
    const wch* const start = string;
    while (*string++) {
    }
    while (--string != start && *string != c) {
    }
    return *string == c ? string : nullptr;
}

// 0x007b5fcf wcsstr: NULL when wcs1 is empty (even for an empty wcs2), else wcs1 for an empty wcs2.
const wch* JOAN_CDECL crt_wcsstr(const wch* wcs1, const wch* wcs2) {
    for (const wch* cp = wcs1; *cp; ++cp) {
        const wch* s1 = cp;
        const wch* s2 = wcs2;
        while (*s1 && *s2 && *s1 == *s2) {
            ++s1;
            ++s2;
        }
        if (!*s2) return cp;
    }
    return nullptr;
}

// 0x007b60bf wcscmp
std::int32_t JOAN_CDECL crt_wcscmp(const wch* src, const wch* dst) {
    std::int32_t ret = 0;
    while ((ret = static_cast<std::int32_t>(*src) - static_cast<std::int32_t>(*dst)) == 0 && *dst) {
        ++src;
        ++dst;
    }
    if (ret < 0) return -1;
    if (ret > 0) return 1;
    return 0;
}

// 0x007b61bc wcsncmp: returns *first - *last (zero-extended units) at the stop position.
std::int32_t JOAN_CDECL crt_wcsncmp(const wch* first, const wch* last, std::uint32_t count) {  // size_t of VC6: 32 bits
    if (!count) return 0;
    while (--count && *first && *first == *last) {
        ++first;
        ++last;
    }
    return static_cast<std::int32_t>(*first) - static_cast<std::int32_t>(*last);
}

JOAN_REPLACE("crt/b113_11", "crt::wcsrchr", 0x007b5f19, &crt_wcsrchr);
JOAN_REPLACE("crt/b113_11", "crt::wcsstr", 0x007b5fcf, &crt_wcsstr);
JOAN_REPLACE("crt/b113_11", "crt::wcscmp", 0x007b60bf, &crt_wcscmp);
JOAN_REPLACE("crt/b113_11", "crt::wcsncmp", 0x007b61bc, &crt_wcsncmp);

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::wcscmp(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_wcscmp>(a0, a1)); }  // 0x7b60bf
