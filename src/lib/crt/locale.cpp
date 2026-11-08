// Original library sources (re/attribution.csv original_file, under lib/): crt/a_cmp.c, crt/a_loc.c, crt/a_map.c,
//   crt/a_str.c, crt/getqloc.c, crt/mbctype.c, crt/setlocal.c.
//
// Batch 114 / list 05: family crt/locale (E writers) of the statically linked MSVC 6 C runtime:
// setlocale with all its internals, _setmbcp with its helpers, __initmbctable, and the NLS wrappers.
// VC6 sources: setlocale.c, getqloc.c, inithelp.c, initctyp.c, initnum.c, initmon.c, inittime.c,
// mbctype.c, a_map.c / a_str.c / a_cmp.c / a_loc.c / aw_*.c. Disassembly and notes: scratchpad w114_05.
//
// Surface (entry-patched; every function of the family that code outside the family reaches):
//   0x007b5269 char* setlocale(int category, const char* locale)              game 0x0058b2e5 (CodePage::setup)
//   0x007b4e65 int _setmbcp(int codepage)                                       game 0x0058b2fe, __initmbctable
//   0x007b524d void __initmbctable(void)                                        XI table, _wincmdln 0x7c11bf,
//                                                                               _setenvp 0x7c1217, _setargv 0x7c12d0
//   0x007b7fc6 int __crtLCMapStringA(lcid, flags, src, n, dst, n, cp, bError)   _strupr, _toupper_lk, _tolower_lk,
//                                                                               _mbslwr/_mbsupr, _strlwr, 0x7bd27e
//   0x007beb60 BOOL __crtGetStringTypeA(type, src, n, out, cp, lcid, bError)    _isctype 0x7b7f51
//   0x007c456b BOOL __crtGetStringTypeW(type, src, n, out, cp, lcid)            0x7c5a98
//   0x007c026c int __crtCompareStringA(lcid, flags, s1, n1, s2, n2, cp)         strcoll family, 0x7c208f
//   0x007c0514 int __crtCompareStringW(lcid, flags, s1, n1, s2, n2, cp)         wcscoll family
//   0x007c585f int __crtLCMapStringW(lcid, flags, src, n, dst, n, cp)           _wcslwr/_wcsupr, 0x7c498e
//   0x007c442d int __getlocaleinfo(type, lcid, lctype, void* out)               (family-internal; registered so
//   0x007c562e int __crtGetLocaleInfoW(lcid, lctype, wchar_t*, n, cp)            the whole NLS layer is ours)
//   0x007c5741 int __crtGetLocaleInfoA(lcid, lctype, char*, n, cp)
// Internals, reimplemented here and reached only from the functions above (not registered):
//   setlocale.c: _setlocale_set_cat 0x7b54af, _setlocale_get_all 0x7b55ca, _expandlocale 0x7b5683
//     (5 arguments, the category is unused), __init_dummy 0x7b579b, _strcats 0x7b579e, __lc_strtolc
//     0x7b57c3, __lc_lctostr 0x7b588f;
//   category initialisers (pointers in __lc_category 0xa30c48, called by _setlocale_set_cat):
//     __init_collate 0x7bf8db, __init_ctype 0x7bf696, __init_monetary 0x7bf424 (+ _get_lc_lconv 0x7bf4ef,
//     fix_grouping 0x7bf611, __free_lconv_mon 0x7bf648), __init_numeric 0x7bf218 (+ fix_grouping 0x7bf3ed),
//     __init_time 0x7beca9 (+ _get_lc_time 0x7bed3a, __free_lc_time 0x7bf088);
//   getqloc.c: __get_qualified_locale 0x7bf91a, TranslateName 0x7bfa97, GetLcidFromLangCountry 0x7bfaef,
//     LangCountryEnumProc 0x7bfb76, GetLcidFromLanguage 0x7bfd7a, LanguageEnumProc 0x7bfdd0,
//     GetLcidFromCountry 0x7bfe8d, CountryEnumProc 0x7bfec4, GetLcidFromDefault 0x7bff4a, ProcessCodePage
//     0x7bff64, TestDefaultCountry 0x7bffca, TestDefaultLanguage 0x7bffe9, IsThisWindowsNT 0x7c004b,
//     crtGetLocaleInfoA 0x7c0081 (Win9x table), LcidFromHexString 0x7c0167, GetPrimaryLen 0x7c01a0;
//   mbctype.c: getSystemCP 0x7b5012, CPtoLCID 0x7b505c, setSBCS 0x7b508f, setSBUpLow 0x7b50b8;
//   strncnt 0x7b81ea / 0x7c04e9, wcsncnt 0x7c072f / 0x7c5a68.
//   (_getmbcp 0x7b523d has no caller and stays original; localeconv 0x7c4565 is a reader, not ours.)
//
// State: everything stays at its original address with the VC6 layout, because the ctype / mbs /
// printf / scanf / time readers (original or C++) read it there: __lc_handle[6] 0xa6b2b0, __lc_codepage
// 0xa6b2c8, __lc_collate_cp 0xa6b2cc, __lc_id[6] 0xa6b3e4, __lc_category[6] 0xa30c48 (catname / locale /
// init; "C" __clocalestr 0xa30b38 compared by pointer), __setlc_active 0xa6ca7c, __unguarded_readlc_active
// 0xa6ca8c, _pctype 0xa30cc4, _pwctype 0xa30cc8 (C table _ctype+1 0xa30cce), __mb_cur_max 0xa30ed0,
// __decimal_point 0xa30ed4, __decimal_point_length 0xa30ed8, __lconv 0xa32240 (C lconv 0xa32210, static "."
// 0xa32208, static "" 0xa6b568), __lc_time_curr 0xa32150 (C data 0xa32158); mbcs: __mbcodepage 0xa6ca90,
// __ismbcodepage 0xa6cb1c, __mblcid 0xa6cd24, __mbulinfo 0xa6cb10, _mbctype 0xa6cc20[257], _mbcasemap
// 0xa6cb20[256], fSystemSet 0xa6b228, __mbctype_initialized 0xa6cd38 (also read by _setargv & co.).
// The family-private statics also stay where they were (only these functions touch them, so this is
// for simplicity): _expandlocale caches (cachein 0xa30b3c, cacheout 0xa30bc0, cacheid 0xa6b22c,
// cachecp 0xa6b234), getqloc statics 0xa6b408..0xa6b428, init statics (lc_time 0xa6b3c8, numeric
// strings 0xa6b3cc..0xa6b3d4, lconv_intl 0xa6b3d8, ctype tables 0xa6b3dc / 0xa6b3e0), the INT
// __getlocaleinfo buffer 0xa6b560, and the "which API works" caches 0xa6b24c, 0xa6b3c4, 0xa6b42c,
// 0xa6b430, 0xa6b56c, 0xa6b580, 0xa6b584, 0xa6b588. Constant tables (code-page info 0xa30a40, language /
// country tables 0xa31de0 / 0xa31d28, default-country ids 0xa31d14, Win9x locale table 0xa31870) are
// copied into this file.
//
// Other families are called through their original addresses: malloc 0x7b6c54, free 0x7b4e1d, calloc
// 0x7b7b87 (heap), _lock 0x7b82aa / _unlock 0x7b830b (mt; locks 0x13 _SETLOCALE_LOCK, 0x19 _MB_CP_LOCK),
// _stricmp 0x7c07b0, _strnicmp 0x7c0960, _isctype 0x7b7f51 (ctype; locale-dependent), atol 0x7b1540
// (scanf), _itoa 0x7c4730. Pure str*/mem* operations use the C++ library (no state). Win32 NLS functions
// are called directly, in the original order and with the original arguments (none of these imports is
// wrapped by the loader).
//
// Behaviour notes / deliberate deviations (none reachable with valid input):
//  - The _alloca buffers of the NLS wrappers (inside __try, returning 0 on stack overflow) are heap
//    buffers here (HeapAlloc on the process heap, which does not touch GetLastError on success). The
//    buffers are always fully written by the API before they are read, except in __crtGetStringTypeW's
//    Win9x path, where the guard words decide (see there; the buffer is sized max(n, cchSrc) + 1).
//  - setSBUpLow: the char-type / lower / upper tables (256 entries each) are zero-filled first. The
//    original leaves them uninitialised and reads all 256 entries even when the NLS calls write fewer:
//    that happens when __mbcodepage is 0 (_setmbcp(0), or the fSystemSet fallback), so the wrappers
//    convert with __lc_codepage, and that code page has other lead bytes than GetCPInfo(CP_ACP) (e.g.
//    setlocale(".932") followed by _setmbcp(0) on a 1252 system: 0xe1 wide chars, entries 0xe1..0xff
//    are stack garbage). Not reachable in the game: CodePage::setup 0x58b2d0 calls _setmbcp only when
//    its setlocale failed, which leaves __lc_codepage unchanged (0 = CP_ACP), and the XI call
//    _setmbcp(-3) runs in the C locale.
//  - __init_ctype: when __getlocaleinfo fails for a zero __lc_codepage the original frees two
//    uninitialised locals. That path is dead: __lc_codepage is set together with a non-C handle from
//    a validated code page (IsValidCodePage), so the handle is non-zero only with a non-zero code page.
//    Here those two pointers are NULL.
//  - Stack buffers that the original can overrun with oversized locale strings (setlocale lctemp 0x88,
//    _setlocale_set_cat lctemp 0x84, LC_STRINGS 0x88 whose code-page part is copied without a length
//    check) are larger here, so such input corrupts nothing (the original would overwrite its frame).
//    The static caches 0xa30b3c / 0xa30bc0 are the original ones and are written exactly as before.
//  - VC6 quirks kept as they are: setlocale(LC_ALL, x) with all categories equal frees
//    __lc_category[LC_ALL].locale after _setlocale_get_all already freed it and set it to NULL (free(NULL));
//    a failing category init does not restore __lc_id[] / __lc_collate_cp; the INT __getlocaleinfo writes
//    one byte; __init_numeric's C branch leaks the previous __lconv strings; fix_grouping dereferences
//    the grouping string even when its lookup failed; _setlocale_get_all does not check malloc.
//
// Verification: differential harness in scratchpad w114_05/h (the original CRT code from the image and
// this file run against the same deterministic fake NLS / heap / lock layer, 8 system configurations
// incl. DBCS ACPs, Win9x (crtGetLocaleInfoA table) and failing W APIs; 960 random scenarios of
// setlocale / _setmbcp / wrapper calls, 58k steps): identical return values, Win32 / heap / lock call
// sequences and CRT state (.data/.bss + heap) at every step.
//
// Modules: crt/locale.
#include "../../callconv.hpp"
#include "../../original/helpers.hpp"
#include "../../platform/win32_types.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../../original/direct.hpp"
#include "crt.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"
#include "../../runtime/loader/import_overrides.hpp"

namespace {

using joan::hook::global;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

JOAN_COUNTER(c_setlocale, "crt::setlocale");
JOAN_COUNTER(c_setmbcp, "crt::_setmbcp");
JOAN_COUNTER(c_lcmap_a, "crt::__crtLCMapStringA");
JOAN_COUNTER(c_lcmap_w, "crt::__crtLCMapStringW");
JOAN_COUNTER(c_gst_a, "crt::__crtGetStringTypeA");
JOAN_COUNTER(c_gst_w, "crt::__crtGetStringTypeW");
JOAN_COUNTER(c_cmp_a, "crt::__crtCompareStringA");
JOAN_COUNTER(c_cmp_w, "crt::__crtCompareStringW");
JOAN_COUNTER(c_getlocaleinfo, "crt::__getlocaleinfo");

using joan::call::cdecl_call;

// ------------------------------------------------------------------ other families (original addresses)
void* crt_malloc(std::size_t n) { return joan::orig::arg::as<void*>(joan::orig::malloc_(joan::orig::arg::u32(n))); }
using joan::orig::helpers::crt_free;
void* crt_calloc(std::size_t n, std::size_t size) { return joan::orig::arg::as<void*>(joan::orig::calloc(joan::orig::arg::u32(n), joan::orig::arg::u32(size))); }
void crt_lock(int n) { joan::orig::libc::_lock(joan::orig::arg::i32(n)); }
void crt_unlock(int n) { joan::orig::libc::_unlock(joan::orig::arg::i32(n)); }
int crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(a, b); }
int crt_strnicmp(const char* a, const char* b, std::size_t n) { return joan::orig::arg::as<int>(joan::orig::libc::_strnicmp(joan::orig::arg::str(a), joan::orig::arg::str(b), n)); }
int crt_isctype(int c, int mask) { return joan::orig::arg::as<int>(joan::orig::libc::_isctype(joan::orig::arg::i32(c), joan::orig::arg::i32(mask))); }
std::int32_t crt_atol(const char* s) { return joan::orig::arg::as<std::int32_t>(joan::orig::libc::atol(joan::orig::arg::str(s))); }
char* crt_itoa(int v, char* buf, int radix) { return joan::orig::arg::as<char*>(joan::orig::libc::_itoa(joan::orig::arg::i32(v), joan::orig::arg::str(buf), joan::orig::arg::i32(radix))); }

// ------------------------------------------------------------------ state at the original addresses
struct LcId {  // LC_ID
    u16 language;
    u16 country;
    u16 codepage;
};
static_assert(sizeof(LcId) == 6, "LC_ID");

struct LcCategory {  // __lc_category[] entry
    const char* catname;
    char* locale;
    int(JOAN_CDECL* init)();
};
static_assert(sizeof(LcCategory) == 3 * sizeof(void*), "__lc_category entry (12 bytes on x86; the image entries are uptr runs)");

LcCategory* lc_category() { return &img::global<LcCategory>(d::strs_a30c48); }
u32* lc_handle() { return &img::global<u32>(d::g_a6b2b0); }
LcId* lc_id() { return &img::global<LcId>(d::g_a6b3e4); }
u32& lc_codepage() { return global<u32>(d::g_lc_codepage); }
u32& lc_collate_cp() { return global<u32>(d::g_a6b2cc); }
volatile i32& setlc_active() { return global<volatile i32>(d::g_setlc_active); }
volatile i32& unguarded_readlc_active() { return global<volatile i32>(d::g_unguarded_readlc_active); }
constexpr img::Addr kCLocaleStr(d::g_cLocaleStr);  // __clocalestr "C"

char* cachein() { return &img::global<char>(d::s_C_a30b3c); }
char* cacheout() { return &img::global<char>(d::s_C_a30bc0); }
LcId* cacheid() { return &img::global<LcId>(d::g_a6b22c); }
u32& cachecp() { return global<u32>(d::g_cachecp); }

u16*& pctype() { return global<u16*>(d::g_pctype); }
u16*& pwctype() { return global<u16*>(d::g_pwctype); }
constexpr img::Addr kCtypeC(d::g_ctypeC);  // _ctype + 1
i32& mb_cur_max() { return global<i32>(d::g_mb_cur_max); }
char& decimal_point() { return global<char>(d::g_decimal_point); }
i32& decimal_point_length() { return global<i32>(d::g_decimal_point_length); }

struct Lconv {  // struct lconv
    char* decimal_point;
    char* thousands_sep;
    char* grouping;
    char* int_curr_symbol;
    char* currency_symbol;
    char* mon_decimal_point;
    char* mon_thousands_sep;
    char* mon_grouping;
    char* positive_sign;
    char* negative_sign;
    char int_frac_digits;
    char frac_digits;
    char p_cs_precedes;
    char p_sep_by_space;
    char n_cs_precedes;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
};
static_assert(sizeof(Lconv) == 10 * sizeof(void*) + 8, "lconv (0x30 on x86; the image's __lconv_c is 10 uptr + 8 chars)");
Lconv*& lconv_ptr() { return global<Lconv*>(d::g_lconv_ptr); }  // __lconv
Lconv& lconv_c() { return global<Lconv>(d::g_lconv_c); }      // __lconv_c
constexpr img::Addr kLconvStaticDecimal(d::g_lconvStaticDecimal);
constexpr img::Addr kLconvStaticNull(d::g_lconvStaticNull);
void*& lc_time_curr() { return global<void*>(d::g_lc_time_curr); }  // __lc_time_curr
constexpr img::Addr kLcTimeC(d::g_lcTimeC);  // __lc_time_c

// init statics
void*& s_lc_time() { return global<void*>(d::g_s_lc_time); }
char*& s_dec_pnt() { return global<char*>(d::g_s_dec_pnt); }
char*& s_thous_sep() { return global<char*>(d::g_s_thous_sep); }
char*& s_grping() { return global<char*>(d::g_s_grping); }
Lconv*& s_lconv_intl() { return global<Lconv*>(d::g_s_lconv_intl); }
u16*& s_ctype1() { return global<u16*>(d::g_s_ctype1); }
u16*& s_wctype1() { return global<u16*>(d::g_s_wctype1); }
WCHAR* s_wcbuffer() { return &img::global<WCHAR>(d::g_a6b560); }  // 4 wchar_t

// "which API works" caches (0 unknown, 1 W, 2 A)
i32& use_lcmap_a() { return global<i32>(d::g_use_lcmap_a); }
i32& use_gst_a() { return global<i32>(d::g_use_gst_a); }
i32& use_cmp_a() { return global<i32>(d::g_use_cmp_a); }
i32& use_cmp_w() { return global<i32>(d::g_use_cmp_w); }
i32& use_gst_w() { return global<i32>(d::g_use_gst_w); }
i32& use_gli_w() { return global<i32>(d::g_use_gli_w); }
i32& use_gli_a() { return global<i32>(d::g_use_gli_a); }
i32& use_lcmap_w() { return global<i32>(d::g_use_lcmap_w); }

// mbcs
i32& mbcodepage() { return global<i32>(d::g_mbcodepage); }
i32& ismbcodepage() { return global<i32>(d::g_a6cb1c); }
u32& mblcid() { return global<u32>(d::g_mblcid); }
u32* mbulinfo() { return &img::global<u32>(d::g_a6cb10); }  // 3 dwords (6 ushorts)
u8* mbctype() { return &img::global<u8>(d::g_a6cc20); }     // 257 bytes, readers use +1
u8* mbcasemap() { return &img::global<u8>(d::g_a6cb20); }   // 256 bytes
i32& fsystemset() { return global<i32>(d::g_fsystemset); }
i32& mbctype_initialized() { return global<i32>(d::g_mbctype_initialized); }

// getqloc statics
u32& g_lcid_language() { return global<u32>(d::g_lcid_language); }
i32& g_abbrev_country() { return global<i32>(d::g_abbrev_country); }
i32& g_primary_len() { return global<i32>(d::g_primary_len); }
i32& g_abbrev_language() { return global<i32>(d::g_abbrev_language); }
const char*& g_pch_language() { return global<const char*>(d::g_pch_language); }
const char*& g_pch_country() { return global<const char*>(d::g_pch_country); }
u32& g_lcid_state() { return global<u32>(d::g_lcid_state); }
u32& g_lcid_country() { return global<u32>(d::g_lcid_country); }
using GetLocaleInfoAFn = int(JOAN_STDCALL*)(LCID, LCTYPE, LPSTR, int);
GetLocaleInfoAFn& g_pfn_get_locale_info_a() { return global<GetLocaleInfoAFn>(d::g_pfn_get_locale_info_a); }

// ------------------------------------------------------------------ the KERNEL32 calls of the C runtime
// The original C runtime reaches these through joan.exe's import table, whose slots hold our portable versions
// (platform/text.cpp: code pages, case mapping, character types, comparison and locale data; platform/process.cpp:
// heap, last error, version; platform/sdl_time.cpp: Sleep). nls::X calls the same function the game's import of X
// gets - no Windows call.
namespace nls {
const void* ours(const char* name) {
    const int i = joan::loader::find_import_override("KERNEL32.dll", name, 0);
    return i >= 0 ? joan::loader::import_overrides()[static_cast<std::size_t>(i)].fn : nullptr;
}
#define JOAN_NLS(name)                                                                              \
    template <class... A>                                                                           \
    auto name(A... a) {                                                                             \
        static const auto f = reinterpret_cast<decltype(&::name)>(const_cast<void*>(ours(#name)));  \
        return f(a...);                                                                             \
    }                                                                                               \
    [[maybe_unused]] inline decltype(&::name) name##_address() {                                                     \
        return reinterpret_cast<decltype(&::name)>(const_cast<void*>(ours(#name)));                 \
    }
JOAN_NLS(CompareStringA)
JOAN_NLS(CompareStringW)
JOAN_NLS(EnumSystemLocalesA)
JOAN_NLS(GetACP)
JOAN_NLS(GetCPInfo)
JOAN_NLS(GetLastError)
JOAN_NLS(GetLocaleInfoA)
JOAN_NLS(GetLocaleInfoW)
JOAN_NLS(GetOEMCP)
JOAN_NLS(GetProcessHeap)
JOAN_NLS(GetStringTypeA)
JOAN_NLS(GetStringTypeW)
JOAN_NLS(GetUserDefaultLCID)
JOAN_NLS(GetVersionExA)
JOAN_NLS(HeapAlloc)
JOAN_NLS(HeapFree)
JOAN_NLS(IsValidCodePage)
JOAN_NLS(IsValidLocale)
JOAN_NLS(LCMapStringA)
JOAN_NLS(LCMapStringW)
JOAN_NLS(MultiByteToWideChar)
JOAN_NLS(Sleep)
JOAN_NLS(WideCharToMultiByte)
#undef JOAN_NLS
}  // namespace nls

// ------------------------------------------------------------------ constant tables (copies)
struct LocaleTab {
    const char* name;
    char abbrev[4];
};
struct Win9xLocale {
    u32 lcid;
    char ilanguage[8];        // +0x04 LOCALE_ILANGUAGE
    const char* englanguage;  // +0x0c LOCALE_SENGLANGUAGE
    char abbrevlang[4];       // +0x10 LOCALE_SABBREVLANGNAME
    const char* engcountry;   // +0x14 LOCALE_SENGCOUNTRY
    char abbrevctry[4];       // +0x18 LOCALE_SABBREVCTRYNAME
    char oemcp[8];            // +0x1c LOCALE_IDEFAULTCODEPAGE
    char ansicp[8];           // +0x24 LOCALE_IDEFAULTANSICODEPAGE
};

// __rgctypeflag 0x00a30a40 + __rgcode_page_info 0x00a30a48 (5 x 0x30), copied byte for byte
constexpr u8 kCodePageInfo[0xf8] = {
    0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0xa4, 0x03, 0x00, 0x00, 0x60, 0x82, 0x79, 0x82,
    0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa6, 0xdf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xa1, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x9f, 0xe0, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x7e, 0x80, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xa8, 0x03, 0x00, 0x00, 0xc1, 0xa3, 0xda, 0xa3,
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb5, 0x03, 0x00, 0x00, 0xc1, 0xa3, 0xda, 0xa3,
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x41, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb6, 0x03, 0x00, 0x00, 0xcf, 0xa2, 0xe4, 0xa2,
    0x1a, 0x00, 0xe5, 0xa2, 0xe8, 0xa2, 0x5b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x7e, 0xa1, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x51, 0x05, 0x00, 0x00, 0x51, 0xda, 0x5e, 0xda,
    0x20, 0x00, 0x5f, 0xda, 0x6a, 0xda, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xd3, 0xd8, 0xde, 0xe0, 0xf9, 0x00, 0x00,
    0x31, 0x7e, 0x81, 0xfe, 0x00, 0x00, 0x00, 0x00,
};
constexpr int kNumCodePages = 5;
constexpr std::size_t kCpInfoBase = 8;  // 0xa30a48 - 0xa30a40
constexpr std::size_t kCpInfoSize = 0x30;
i32 cpinfo_code_page(int icp) {
    i32 v;
    std::memcpy(&v, kCodePageInfo + kCpInfoBase + icp * kCpInfoSize, 4);
    return v;
}
const u8* cpinfo_mbulinfo(int icp) { return kCodePageInfo + kCpInfoBase + icp * kCpInfoSize + 4; }
const u8* cpinfo_range(int icp, int irg) { return kCodePageInfo + kCpInfoBase + icp * kCpInfoSize + 0x10 + irg * 8; }
u8 ctypeflag(int irg) { return kCodePageInfo[irg]; }

// __rg_language 0x00a31de0 (0x41 entries) and __rg_country 0x00a31d28 (0x17 entries)
const LocaleTab kLanguages[0x41] = {
    {"american", "ENU"},
    {"american english", "ENU"},
    {"american-english", "ENU"},
    {"australian", "ENA"},
    {"belgian", "NLB"},
    {"canadian", "ENC"},
    {"chh", "ZHH"},
    {"chi", "ZHI"},
    {"chinese", "CHS"},
    {"chinese-hongkong", "ZHH"},
    {"chinese-simplified", "CHS"},
    {"chinese-singapore", "ZHI"},
    {"chinese-traditional", "CHT"},
    {"dutch-belgian", "NLB"},
    {"english-american", "ENU"},
    {"english-aus", "ENA"},
    {"english-belize", "ENL"},
    {"english-can", "ENC"},
    {"english-caribbean", "ENB"},
    {"english-ire", "ENI"},
    {"english-jamaica", "ENJ"},
    {"english-nz", "ENZ"},
    {"english-south africa", "ENS"},
    {"english-trinidad y tobago", "ENT"},
    {"english-uk", "ENG"},
    {"english-us", "ENU"},
    {"english-usa", "ENU"},
    {"french-belgian", "FRB"},
    {"french-canadian", "FRC"},
    {"french-luxembourg", "FRL"},
    {"french-swiss", "FRS"},
    {"german-austrian", "DEA"},
    {"german-lichtenstein", "DEC"},
    {"german-luxembourg", "DEL"},
    {"german-swiss", "DES"},
    {"irish-english", "ENI"},
    {"italian-swiss", "ITS"},
    {"norwegian", "NOR"},
    {"norwegian-bokmal", "NOR"},
    {"norwegian-nynorsk", "NON"},
    {"portuguese-brazilian", "PTB"},
    {"spanish-argentina", "ESS"},
    {"spanish-bolivia", "ESB"},
    {"spanish-chile", "ESL"},
    {"spanish-colombia", "ESO"},
    {"spanish-costa rica", "ESC"},
    {"spanish-dominican republic", "ESD"},
    {"spanish-ecuador", "ESF"},
    {"spanish-el salvador", "ESE"},
    {"spanish-guatemala", "ESG"},
    {"spanish-honduras", "ESH"},
    {"spanish-mexican", "ESM"},
    {"spanish-modern", "ESN"},
    {"spanish-nicaragua", "ESI"},
    {"spanish-panama", "ESA"},
    {"spanish-paraguay", "ESZ"},
    {"spanish-peru", "ESR"},
    {"spanish-puerto rico", "ESU"},
    {"spanish-uruguay", "ESY"},
    {"spanish-venezuela", "ESV"},
    {"swedish-finland", "SVF"},
    {"swiss", "DES"},
    {"uk", "ENG"},
    {"us", "ENU"},
    {"usa", "ENU"},
};
const LocaleTab kCountries[0x17] = {
    {"america", "USA"},
    {"britain", "GBR"},
    {"china", "CHN"},
    {"czech", "CZE"},
    {"england", "GBR"},
    {"great britain", "GBR"},
    {"holland", "NLD"},
    {"hong-kong", "HKG"},
    {"new-zealand", "NZL"},
    {"nz", "NZL"},
    {"pr china", "CHN"},
    {"pr-china", "CHN"},
    {"puerto-rico", "PRI"},
    {"slovak", "SVK"},
    {"south africa", "ZAF"},
    {"south korea", "KOR"},
    {"south-africa", "ZAF"},
    {"south-korea", "KOR"},
    {"trinidad & tobago", "TTO"},
    {"uk", "GBR"},
    {"united-kingdom", "GBR"},
    {"united-states", "USA"},
    {"us", "USA"},
};
// __rgDefaultCountry 0x00a31d14 (10 language ids)
constexpr u16 kDefaultCountry[10] = {0x0c0c, 0x0c1a, 0x1007, 0x0436, 0x080c, 0x042d, 0x0403, 0x100c, 0x0810, 0x081d};
// Win9x locale table 0x00a31870 (27 x 0x2c) used by crtGetLocaleInfoA 0x007c0081
const Win9xLocale kWin9xLocales[27] = {
    {0x40a, "040a", "Spanish - Traditional Sort", "ESP", "Spain", "ESP", "850", "1252"},
    {0x40b, "040b", "Finnish", "FIN", "Finland", "FIN", "850", "1252"},
    {0x40c, "040c", "French", "FRA", "France", "FRA", "850", "1252"},
    {0x40f, "040f", "Icelandic", "ISL", "Iceland", "ISL", "850", "1252"},
    {0x41d, "041d", "Swedish", "SVE", "Sweden", "SWE", "850", "1252"},
    {0x42d, "042d", "Basque", "EUQ", "Spain", "ESP", "850", "1252"},
    {0x80a, "080a", "Spanish", "ESM", "Mexico", "MEX", "850", "1252"},
    {0x80c, "080c", "French", "FRB", "Belgium", "BEL", "850", "1252"},
    {0xc07, "0c07", "German", "DEA", "Austria", "AUT", "850", "1252"},
    {0xc09, "0c09", "English", "ENA", "Australia", "AUS", "850", "1252"},
    {0xc0a, "0c0a", "Spanish - Modern Sort", "ESN", "Spain", "ESP", "850", "1252"},
    {0xc0c, "0c0c", "French", "FRC", "Canada", "CAN", "850", "1252"},
    {0x100a, "100a", "Spanish", "ESG", "Guatemala", "GTM", "850", "1252"},
    {0x100c, "100c", "French", "FRS", "Switzerland", "CHE", "850", "1252"},
    {0x140a, "140a", "Spanish", "ESC", "Costa Rica", "CRI", "850", "1252"},
    {0x140c, "140c", "French", "FRL", "Luxembourg", "LUX", "850", "1252"},
    {0x180a, "180a", "Spanish", "ESA", "Panama", "PAN", "850", "1252"},
    {0x1c09, "1c09", "English", "ENS", "South Africa", "ZAF", "437", "1252"},
    {0x1c0a, "1c0a", "Spanish", "ESD", "Dominican Republic", "DOM", "850", "1252"},
    {0x200a, "200a", "Spanish", "ESV", "Venezuela", "VEN", "850", "1252"},
    {0x240a, "240a", "Spanish", "ESO", "Colombia", "COL", "850", "1252"},
    {0x280a, "280a", "Spanish", "ESR", "Peru", "PER", "850", "1252"},
    {0x2c0a, "2c0a", "Spanish", "ESS", "Argentina", "ARG", "850", "1252"},
    {0x300a, "300a", "Spanish", "ESF", "Ecuador", "ECU", "850", "1252"},
    {0x340a, "340a", "Spanish", "ESL", "Chile", "CHL", "850", "1252"},
    {0x380a, "380a", "Spanish", "ESY", "Uruguay", "URY", "850", "1252"},
    {0x3c0a, "3c0a", "Spanish", "ESZ", "Paraguay", "PRY", "850", "1252"},
};

// "\0" probe strings of the NLS wrappers (0x9e7084 / 0x9e7088)
const char kProbeA[2] = {0, 0};
const WCHAR kProbeW[2] = {0, 0};

// Scratch buffer in place of the original _alloca (see header).
template <class T>
struct TempBuf {
    T* p;
    explicit TempBuf(int n) {
        const std::size_t count = n > 0 ? static_cast<std::size_t>(n) : 0u;
        p = static_cast<T*>(nls::HeapAlloc(nls::GetProcessHeap(), 0, count * sizeof(T) + 16u));
    }
    ~TempBuf() {
        if (p != nullptr) nls::HeapFree(nls::GetProcessHeap(), 0, p);
    }
    TempBuf(const TempBuf&) = delete;
    TempBuf& operator=(const TempBuf&) = delete;
};

// ------------------------------------------------------------------ strncnt / wcsncnt
// 0x007b81ea / 0x007c04e9: count chars before the NUL within cnt (reads string[cnt] like the original).
int strncnt(const char* string, int cnt) {
    int n = cnt;
    const char* cp = string;
    while (n-- != 0 && *cp != '\0') ++cp;
    if (*cp == '\0') return static_cast<int>(cp - string);
    return cnt;
}
// 0x007c072f / 0x007c5a68
int wcsncnt(const WCHAR* string, int cnt) {
    int n = cnt;
    const WCHAR* cp = string;
    while (n-- != 0 && *cp != 0) ++cp;
    if (*cp == 0) return static_cast<int>(cp - string);
    return cnt;
}

// ------------------------------------------------------------------ 0x007b7fc6 __crtLCMapStringA
static int JOAN_CDECL crt_lcmap_string_a(LCID locale, DWORD flags, const char* src, int cch_src, char* dest,
                                      int cch_dest, int code_page, BOOL b_error) {
    c_lcmap_a.hit();
    i32& f_use = use_lcmap_a();
    if (f_use == 0) {
        if (nls::LCMapStringW(0, LCMAP_LOWERCASE, kProbeW, 1, nullptr, 0) != 0)
            f_use = 1;
        else if (nls::LCMapStringA(0, LCMAP_LOWERCASE, kProbeA, 1, nullptr, 0) != 0)
            f_use = 2;
        else
            return 0;
    }
    if (cch_src > 0) cch_src = strncnt(src, cch_src);
    if (f_use == 2) return nls::LCMapStringA(locale, flags, src, cch_src, dest, cch_dest);
    if (f_use != 1) return 0;

    if (code_page == 0) code_page = static_cast<int>(lc_codepage());
    const int inbuff_size = nls::MultiByteToWideChar(static_cast<UINT>(code_page),
                                                  b_error ? (MB_PRECOMPOSED | MB_ERR_INVALID_CHARS) : MB_PRECOMPOSED,
                                                  src, cch_src, nullptr, 0);
    if (inbuff_size == 0) return 0;
    TempBuf<WCHAR> inw(inbuff_size);
    if (inw.p == nullptr) return 0;
    if (nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED, src, cch_src, inw.p, inbuff_size) == 0)
        return 0;
    const int retval = nls::LCMapStringW(locale, flags, inw.p, inbuff_size, nullptr, 0);
    if (retval == 0) return 0;
    if ((flags & LCMAP_SORTKEY) != 0) {
        if (cch_dest != 0) {
            if (retval > cch_dest) return 0;
            if (nls::LCMapStringW(locale, flags, inw.p, inbuff_size, reinterpret_cast<LPWSTR>(dest), cch_dest) == 0)
                return 0;
        }
        return retval;
    }
    const int outbuff_size = retval;
    TempBuf<WCHAR> outw(outbuff_size);
    if (outw.p == nullptr) return 0;
    if (nls::LCMapStringW(locale, flags, inw.p, inbuff_size, outw.p, outbuff_size) == 0) return 0;
    const int r = cch_dest == 0
                      ? nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, outw.p,
                                              outbuff_size, nullptr, 0, nullptr, nullptr)
                      : nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, outw.p,
                                              outbuff_size, dest, cch_dest, nullptr, nullptr);
    return r;
}

// ------------------------------------------------------------------ 0x007c585f __crtLCMapStringW
static int JOAN_CDECL crt_lcmap_string_w(LCID locale, DWORD flags, const WCHAR* src, int cch_src, WCHAR* dest,
                                      int cch_dest, int code_page) {
    c_lcmap_w.hit();
    i32& f_use = use_lcmap_w();
    if (f_use == 0) {
        if (nls::LCMapStringW(0, LCMAP_LOWERCASE, kProbeW, 1, nullptr, 0) != 0)
            f_use = 1;
        else if (nls::LCMapStringA(0, LCMAP_LOWERCASE, kProbeA, 1, nullptr, 0) != 0)
            f_use = 2;
        else
            return 0;
    }
    if (cch_src > 0) cch_src = wcsncnt(src, cch_src);
    if (f_use == 1) return nls::LCMapStringW(locale, flags, src, cch_src, dest, cch_dest);
    if (f_use != 2) return 0;

    if (code_page == 0) code_page = static_cast<int>(lc_codepage());
    const int inbuff_size = nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, src,
                                                  cch_src, nullptr, 0, nullptr, nullptr);
    if (inbuff_size == 0) return 0;
    TempBuf<char> inbuf(inbuff_size);
    if (inbuf.p == nullptr) return 0;
    if (nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, src, cch_src, inbuf.p,
                              inbuff_size, nullptr, nullptr) == 0)
        return 0;
    const int outbuff_size = nls::LCMapStringA(locale, flags, inbuf.p, inbuff_size, nullptr, 0);
    if (outbuff_size == 0) return 0;
    TempBuf<char> outbuf(outbuff_size);
    if (outbuf.p == nullptr) return 0;
    if (nls::LCMapStringA(locale, flags, inbuf.p, inbuff_size, outbuf.p, outbuff_size) == 0) return 0;
    if ((flags & LCMAP_SORTKEY) != 0) {
        if (cch_dest != 0)
            std::strncpy(reinterpret_cast<char*>(dest), outbuf.p,
                         static_cast<std::size_t>(cch_dest < outbuff_size ? cch_dest : outbuff_size));
        return outbuff_size;
    }
    const int r = cch_dest == 0 ? nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED, outbuf.p,
                                                        outbuff_size, nullptr, 0)
                                : nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED, outbuf.p,
                                                        outbuff_size, dest, cch_dest);
    return r;
}

// ------------------------------------------------------------------ 0x007beb60 __crtGetStringTypeA
static BOOL JOAN_CDECL crt_get_string_type_a(DWORD info_type, const char* src, int cch_src, WORD* char_type,
                                          int code_page, int lcid, BOOL b_error) {
    c_gst_a.hit();
    i32& f_use = use_gst_a();
    if (f_use == 0) {
        WORD dummy;
        if (nls::GetStringTypeW(CT_CTYPE1, kProbeW, 1, &dummy) != 0)
            f_use = 1;
        else if (nls::GetStringTypeA(0, CT_CTYPE1, kProbeA, 1, &dummy) != 0)
            f_use = 2;
        else
            return FALSE;
    }
    if (f_use == 2) {
        if (lcid == 0) lcid = static_cast<int>(lc_handle()[2]);
        return nls::GetStringTypeA(static_cast<LCID>(lcid), info_type, src, cch_src, char_type);
    }
    if (f_use != 1) return FALSE;

    if (code_page == 0) code_page = static_cast<int>(lc_codepage());
    const int buff_size = nls::MultiByteToWideChar(static_cast<UINT>(code_page),
                                                b_error ? (MB_PRECOMPOSED | MB_ERR_INVALID_CHARS) : MB_PRECOMPOSED,
                                                src, cch_src, nullptr, 0);
    if (buff_size == 0) return FALSE;
    TempBuf<WCHAR> wbuf(buff_size);
    if (wbuf.p == nullptr) return FALSE;
    std::memset(wbuf.p, 0, static_cast<std::size_t>(buff_size) * sizeof(WCHAR));
    const int n = nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED, src, cch_src, wbuf.p, buff_size);
    if (n == 0) return FALSE;
    return nls::GetStringTypeW(info_type, wbuf.p, n, char_type);
}

// ------------------------------------------------------------------ 0x007c456b __crtGetStringTypeW
static BOOL JOAN_CDECL crt_get_string_type_w(DWORD info_type, const WCHAR* src, int cch_src, WORD* char_type,
                                          int code_page, int lcid) {
    c_gst_w.hit();
    i32& f_use = use_gst_w();
    if (f_use == 0) {
        WORD dummy;
        if (nls::GetStringTypeW(CT_CTYPE1, kProbeW, 1, &dummy) != 0)
            f_use = 1;
        else if (nls::GetStringTypeA(0, CT_CTYPE1, kProbeA, 1, &dummy) != 0)
            f_use = 2;
        else
            return FALSE;
    }
    if (f_use == 1) return nls::GetStringTypeW(info_type, src, cch_src, char_type);
    if (f_use != 2) return FALSE;

    if (code_page == 0) code_page = static_cast<int>(lc_codepage());
    const int buff_size = nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, src,
                                                cch_src, nullptr, 0, nullptr, nullptr);
    if (buff_size == 0) return FALSE;
    TempBuf<char> mbuf(buff_size);
    if (mbuf.p == nullptr) return FALSE;
    std::memset(mbuf.p, 0, static_cast<std::size_t>(buff_size));
    if (nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, src, cch_src, mbuf.p,
                              buff_size, nullptr, nullptr) == 0)
        return FALSE;
    // Original: _alloca((buff_size + 1) * 2) and guard words at [cchSrc - 1] / [cchSrc]. Sized so both
    // guard indices are inside the buffer (they are whenever buff_size >= cchSrc, the only case in which
    // the original stays inside its block).
    const int words = (buff_size > cch_src ? buff_size : cch_src) + 1;
    TempBuf<WORD> info(words);
    if (info.p == nullptr) return FALSE;
    if (lcid == 0) lcid = static_cast<int>(lc_handle()[2]);
    info.p[cch_src] = 0xffff;
    info.p[cch_src - 1] = 0xffff;
    const BOOL retval = nls::GetStringTypeA(static_cast<LCID>(lcid), info_type, mbuf.p, buff_size, info.p);
    if (info.p[cch_src - 1] == 0xffff || info.p[cch_src] != 0xffff) return FALSE;
    std::memmove(char_type, info.p, static_cast<std::size_t>(cch_src) * sizeof(WORD));
    return retval;
}

// ------------------------------------------------------------------ 0x007c026c __crtCompareStringA
static int JOAN_CDECL crt_compare_string_a(LCID locale, DWORD flags, const char* s1, int cch1, const char* s2, int cch2,
                                        int code_page) {
    c_cmp_a.hit();
    i32& f_use = use_cmp_a();
    if (f_use == 0) {
        if (nls::CompareStringW(0, 0, kProbeW, 1, kProbeW, 1) != 0)
            f_use = 1;
        else if (nls::CompareStringA(0, 0, kProbeA, 1, kProbeA, 1) != 0)
            f_use = 2;
        else
            return 0;
    }
    if (cch1 > 0) cch1 = strncnt(s1, cch1);
    if (cch2 > 0) cch2 = strncnt(s2, cch2);
    if (f_use == 2) return nls::CompareStringA(locale, flags, s1, cch1, s2, cch2);
    if (f_use != 1) return 0;

    if (code_page == 0) code_page = static_cast<int>(lc_codepage());
    if (cch1 == 0 || cch2 == 0) {
        if (cch1 == cch2) return 2;
        if (cch2 > 1) return 1;
        if (cch1 > 1) return 3;
        CPINFO cpinfo;
        if (nls::GetCPInfo(static_cast<UINT>(code_page), &cpinfo) == 0) return 0;
        if (cch1 > 0) {
            if (cpinfo.MaxCharSize < 2) return 3;
            for (const BYTE* cp = cpinfo.LeadByte; cp[0] != 0 && cp[1] != 0; cp += 2) {
                const u8 c = static_cast<u8>(*s1);
                if (c >= cp[0] && c <= cp[1]) return 2;
            }
            return 3;
        }
        if (cch2 > 0) {
            if (cpinfo.MaxCharSize < 2) return 1;
            for (const BYTE* cp = cpinfo.LeadByte; cp[0] != 0 && cp[1] != 0; cp += 2) {
                const u8 c = static_cast<u8>(*s2);
                if (c >= cp[0] && c <= cp[1]) return 2;
            }
            return 1;
        }
    }
    const int size1 = nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, s1,
                                            cch1, nullptr, 0);
    if (size1 == 0) return 0;
    TempBuf<WCHAR> w1(size1);
    if (w1.p == nullptr) return 0;
    if (nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED, s1, cch1, w1.p, size1) == 0) return 0;
    const int size2 = nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, s2,
                                            cch2, nullptr, 0);
    if (size2 == 0) return 0;
    TempBuf<WCHAR> w2(size2);
    if (w2.p == nullptr) return 0;
    if (nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED, s2, cch2, w2.p, size2) == 0) return 0;
    return nls::CompareStringW(locale, flags, w1.p, size1, w2.p, size2);
}

// ------------------------------------------------------------------ 0x007c0514 __crtCompareStringW
static int JOAN_CDECL crt_compare_string_w(LCID locale, DWORD flags, const WCHAR* s1, int cch1, const WCHAR* s2,
                                        int cch2, int code_page) {
    c_cmp_w.hit();
    i32& f_use = use_cmp_w();
    if (f_use == 0) {
        if (nls::CompareStringW(0, 0, kProbeW, 1, kProbeW, 1) != 0)
            f_use = 1;
        else if (nls::CompareStringA(0, 0, kProbeA, 1, kProbeA, 1) != 0)
            f_use = 2;
        else
            return 0;
    }
    if (cch1 > 0) cch1 = wcsncnt(s1, cch1);
    if (cch2 > 0) cch2 = wcsncnt(s2, cch2);
    if (cch1 == 0 || cch2 == 0) {
        const i32 d = static_cast<i32>(static_cast<u32>(cch1) - static_cast<u32>(cch2));
        if (d == 0) return 2;
        return d >= 0 ? 3 : 1;
    }
    if (f_use == 1) return nls::CompareStringW(locale, flags, s1, cch1, s2, cch2);
    if (f_use != 2) return 0;

    if (code_page == 0) code_page = static_cast<int>(lc_codepage());
    const int size1 = nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, s1, cch1,
                                            nullptr, 0, nullptr, nullptr);
    if (size1 == 0) return 0;
    TempBuf<char> b1(size1);
    if (b1.p == nullptr) return 0;
    if (nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, s1, cch1, b1.p, size1,
                              nullptr, nullptr) == 0)
        return 0;
    const int size2 = nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, s2, cch2,
                                            nullptr, 0, nullptr, nullptr);
    if (size2 == 0) return 0;
    TempBuf<char> b2(size2);
    if (b2.p == nullptr) return 0;
    if (nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, s2, cch2, b2.p, size2,
                              nullptr, nullptr) == 0)
        return 0;
    return nls::CompareStringA(locale, flags, b1.p, size1, b2.p, size2);
}

// ------------------------------------------------------------------ 0x007c562e __crtGetLocaleInfoW
static int JOAN_CDECL crt_get_locale_info_w(LCID locale, LCTYPE lctype, WCHAR* data, int cch_data, int code_page) {
    i32& f_use = use_gli_w();
    if (f_use == 0) {
        if (nls::GetLocaleInfoW(0, LOCALE_ILANGUAGE, nullptr, 0) != 0)
            f_use = 1;
        else if (nls::GetLocaleInfoA(0, LOCALE_ILANGUAGE, nullptr, 0) != 0)
            f_use = 2;
        else
            return 0;
    }
    if (f_use == 1) return nls::GetLocaleInfoW(locale, lctype, data, cch_data);
    if (f_use != 2) return 0;

    if (code_page == 0) code_page = static_cast<int>(lc_codepage());
    const int size = nls::GetLocaleInfoA(locale, lctype, nullptr, 0);
    if (size == 0) return 0;
    TempBuf<char> buf(size);
    if (buf.p == nullptr) return 0;
    if (nls::GetLocaleInfoA(locale, lctype, buf.p, size) == 0) return 0;
    if (cch_data == 0) return nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED, buf.p, -1, nullptr, 0);
    return nls::MultiByteToWideChar(static_cast<UINT>(code_page), MB_PRECOMPOSED, buf.p, -1, data, cch_data);
}

// ------------------------------------------------------------------ 0x007c5741 __crtGetLocaleInfoA
static int JOAN_CDECL crt_get_locale_info_a(LCID locale, LCTYPE lctype, char* data, int cch_data, int code_page) {
    i32& f_use = use_gli_a();
    if (f_use == 0) {
        if (nls::GetLocaleInfoW(0, LOCALE_ILANGUAGE, nullptr, 0) != 0)
            f_use = 1;
        else if (nls::GetLocaleInfoA(0, LOCALE_ILANGUAGE, nullptr, 0) != 0)
            f_use = 2;
        else
            return 0;
    }
    if (f_use == 2) return nls::GetLocaleInfoA(locale, lctype, data, cch_data);
    if (f_use != 1) return 0;

    if (code_page == 0) code_page = static_cast<int>(lc_codepage());
    const int size = nls::GetLocaleInfoW(locale, lctype, nullptr, 0);
    if (size == 0) return 0;
    TempBuf<WCHAR> buf(size);
    if (buf.p == nullptr) return 0;
    if (nls::GetLocaleInfoW(locale, lctype, buf.p, size) == 0) return 0;
    if (cch_data == 0)
        return nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, buf.p, -1, nullptr,
                                     0, nullptr, nullptr);
    return nls::WideCharToMultiByte(static_cast<UINT>(code_page), WC_COMPOSITECHECK | WC_SEPCHARS, buf.p, -1, data,
                                 cch_data, nullptr, nullptr);
}

// ------------------------------------------------------------------ 0x007c442d __getlocaleinfo
// lc_type 1 (LC_STR_TYPE): *(char**)address = malloc'd copy; 0 (LC_INT_TYPE): one byte of decimal digits.
static int JOAN_CDECL crt_getlocaleinfo(int lc_type, LCID handle, LCTYPE field, void* address) {
    c_getlocaleinfo.hit();
    if (lc_type == 1) {
        char cbuffer[0x80];
        char* pcbuffer = cbuffer;
        int bufferused = 0;
        int outsize = crt_get_locale_info_a(handle, field, pcbuffer, 0x80, 0);
        if (outsize == 0) {
            if (nls::GetLastError() != ERROR_INSUFFICIENT_BUFFER) return -1;
            const int buffersize = crt_get_locale_info_a(handle, field, nullptr, 0, 0);
            if (buffersize == 0) return -1;
            pcbuffer = static_cast<char*>(crt_malloc(static_cast<std::size_t>(buffersize)));
            if (pcbuffer == nullptr) return -1;
            bufferused = 1;
            outsize = crt_get_locale_info_a(handle, field, pcbuffer, buffersize, 0);
            if (outsize == 0) {
                crt_free(pcbuffer);
                return -1;
            }
        }
        char* s = static_cast<char*>(crt_malloc(static_cast<std::size_t>(outsize)));
        *static_cast<char**>(address) = s;
        if (s == nullptr) {
            if (bufferused != 0) crt_free(pcbuffer);
            return -1;
        }
        std::strncpy(s, pcbuffer, static_cast<std::size_t>(outsize));
        if (bufferused != 0) crt_free(pcbuffer);
        return 0;
    }
    if (lc_type != 0) return -1;
    WCHAR* wc = s_wcbuffer();
    if (crt_get_locale_info_w(handle, field, wc, 4, 0) == 0) return -1;
    u8* out = static_cast<u8*>(address);
    *out = 0;
    for (const WCHAR* p = wc; p < wc + 4; ++p) {
        const u8 c = static_cast<u8>(*p);  // low byte of the wide char
        int digit;
        if (mb_cur_max() > 1)
            digit = crt_isctype(c, 4);
        else
            digit = static_cast<u8>(pctype()[c]) & 4;
        if (digit == 0) break;
        *out = static_cast<u8>(static_cast<u8>(*out * 10) + c - 0x30);
    }
    return 0;
}

// ------------------------------------------------------------------ getqloc.c
// 0x007c004b IsThisWindowsNT
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
bool is_this_windows_nt() {
    OSVERSIONINFOA vi;
    vi.dwOSVersionInfoSize = sizeof vi;
    if (nls::GetVersionExA(&vi) == 0) return false;
    return vi.dwPlatformId == VER_PLATFORM_WIN32_NT;
}
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

// 0x007c0081 crtGetLocaleInfoA (stdcall): Win9x substitute table, falling back to GetLocaleInfoA.
int JOAN_STDCALL crt_get_locale_info_a_9x(LCID lcid, LCTYPE lctype, LPSTR data, int cch_data) {
    int low = 0;
    int high = 0x1a;
    while (low <= high) {
        const int i = (low + high) / 2;
        const Win9xLocale& e = kWin9xLocales[i];
        if (lcid == e.lcid) {
            const char* s;
            switch (lctype) {
            case 1: s = e.ilanguage; break;
            case 3: s = e.abbrevlang; break;
            case 7: s = e.abbrevctry; break;
            case 0xb: s = e.oemcp; break;
            case 0x1001: s = e.englanguage; break;
            case 0x1002: s = e.engcountry; break;
            case 0x1004: s = e.ansicp; break;
            default: s = nullptr; break;
            }
            if (s == nullptr || cch_data < 1) break;
            std::strncpy(data, s, static_cast<std::size_t>(cch_data - 1));
            data[cch_data - 1] = '\0';
            return 1;
        }
        if (lcid < e.lcid)
            high = i - 1;
        else
            low = i + 1;
    }
    return nls::GetLocaleInfoA(lcid, lctype, data, cch_data);
}

// 0x007c0167 LcidFromHexString
u32 lcid_from_hex_string(const char* s) {
    u32 lcid = 0;
    for (;;) {
        char ch = *s++;
        if (ch == '\0') return lcid;
        if (ch >= 'a' && ch <= 'f')
            ch = static_cast<char>(ch + ('9' + 1 - 'a'));
        else if (ch >= 'A' && ch <= 'F')
            ch = static_cast<char>(ch + ('9' + 1 - 'A'));
        lcid = ((lcid + 0x0ffffffdu) << 4) + static_cast<u32>(static_cast<i32>(static_cast<signed char>(ch)));
    }
}

// 0x007c01a0 GetPrimaryLen
int get_primary_len(const char* s) {
    int len = 0;
    for (;;) {
        const signed char ch = static_cast<signed char>(*s++);
        if (!((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))) return len;
        ++len;
    }
}

// 0x007bfa97 TranslateName (binary search with _stricmp, high = count - 1)
void translate_name(const LocaleTab* table, int high, const char** name) {
    int low = 0;
    int cmp = 1;
    while (low <= high && cmp != 0) {
        const int i = (low + high) / 2;
        cmp = crt_stricmp(*name, table[i].name);
        if (cmp == 0)
            *name = table[i].abbrev;
        else if (cmp < 0)
            high = i - 1;
        else
            low = i + 1;
    }
}

// 0x007bffca TestDefaultCountry: 0 when the language id is in the table
int test_default_country(u32 lcid) {
    const u16 lang = static_cast<u16>(lcid);
    for (int i = 0; i < 10; ++i)
        if (lang == kDefaultCountry[i]) return 0;
    return 1;
}

// 0x007bffe9 TestDefaultLanguage
int test_default_language(u32 lcid, int test_primary) {
    char info[0x78];
    const u32 lcid_default = (lcid & 0x3ffu) | 0x400u;
    if (g_pfn_get_locale_info_a()(lcid_default, LOCALE_ILANGUAGE, info, 0x78) == 0) return 0;
    if (lcid != lcid_from_hex_string(info) && test_primary != 0 &&
        get_primary_len(g_pch_language()) == static_cast<int>(std::strlen(g_pch_language())))
        return 0;
    return 1;
}

int enum_result() { return static_cast<int>((~g_lcid_state() >> 2) & 1u); }

// 0x007bfb76 LangCountryEnumProc
BOOL JOAN_STDCALL lang_country_enum_proc(LPSTR lcid_string) {
    char info[0x78];
    const u32 lcid = lcid_from_hex_string(lcid_string);
    const LCTYPE lang_type = g_abbrev_language() != 0 ? 3u : 0x1001u;
    if (g_pfn_get_locale_info_a()(lcid, g_abbrev_country() != 0 ? 7u : 0x1002u, info, 0x78) == 0) {
        g_lcid_state() = 0;
        return TRUE;
    }
    if (crt_stricmp(g_pch_country(), info) == 0) {
        if (g_pfn_get_locale_info_a()(lcid, lang_type, info, 0x78) == 0) {
            g_lcid_state() = 0;
            return TRUE;
        }
        if (crt_stricmp(g_pch_language(), info) == 0) {
            g_lcid_state() |= 0x304u;
            g_lcid_country() = lcid;
            g_lcid_language() = lcid;
        } else if ((g_lcid_state() & 2u) == 0) {
            if (g_primary_len() != 0 &&
                crt_strnicmp(g_pch_language(), info, static_cast<std::size_t>(g_primary_len())) == 0) {
                g_lcid_state() |= 2u;
                g_lcid_country() = lcid;
                if (static_cast<i32>(std::strlen(g_pch_language())) == g_primary_len()) g_lcid_language() = lcid;
            } else if ((g_lcid_state() & 1u) == 0) {
                if (test_default_country(lcid) != 0) {
                    g_lcid_state() |= 1u;
                    g_lcid_country() = lcid;
                }
            }
        }
    }
    if ((g_lcid_state() & 0x300u) != 0x300u) {
        if (g_pfn_get_locale_info_a()(lcid, lang_type, info, 0x78) == 0) {
            g_lcid_state() = 0;
            return TRUE;
        }
        bool set_language;
        if (crt_stricmp(g_pch_language(), info) == 0) {
            g_lcid_state() |= 0x200u;
            set_language = g_abbrev_language() != 0 || g_primary_len() == 0 ||
                           static_cast<i32>(std::strlen(g_pch_language())) != g_primary_len() ||
                           test_default_language(lcid, 1) != 0;
        } else {
            set_language = g_abbrev_language() == 0 && g_primary_len() != 0 &&
                           crt_strnicmp(g_pch_language(), info, static_cast<std::size_t>(g_primary_len())) == 0 &&
                           test_default_language(lcid, 0) != 0;
        }
        if (set_language) {
            g_lcid_state() |= 0x100u;
            if (g_lcid_language() == 0) g_lcid_language() = lcid;
        }
    }
    return enum_result();
}

// 0x007bfdd0 LanguageEnumProc
BOOL JOAN_STDCALL language_enum_proc(LPSTR lcid_string) {
    char info[0x78];
    const u32 lcid = lcid_from_hex_string(lcid_string);
    if (g_pfn_get_locale_info_a()(lcid, g_abbrev_language() != 0 ? 3u : 0x1001u, info, 0x78) == 0) {
        g_lcid_state() = 0;
        return TRUE;
    }
    bool found;
    if (crt_stricmp(g_pch_language(), info) == 0)
        found = g_abbrev_language() != 0 || test_default_language(lcid, 1) != 0;
    else
        found = g_abbrev_language() == 0 && g_primary_len() != 0 &&
                crt_strnicmp(g_pch_language(), info, static_cast<std::size_t>(g_primary_len())) == 0 &&
                test_default_language(lcid, 0) != 0;
    if (found) {
        g_lcid_state() |= 4u;
        g_lcid_country() = lcid;
        g_lcid_language() = lcid;
    }
    return enum_result();
}

// 0x007bfec4 CountryEnumProc
BOOL JOAN_STDCALL country_enum_proc(LPSTR lcid_string) {
    char info[0x78];
    const u32 lcid = lcid_from_hex_string(lcid_string);
    if (g_pfn_get_locale_info_a()(lcid, g_abbrev_country() != 0 ? 7u : 0x1002u, info, 0x78) == 0) {
        g_lcid_state() = 0;
        return TRUE;
    }
    if (crt_stricmp(g_pch_country(), info) == 0 && test_default_country(lcid) != 0) {
        g_lcid_state() |= 4u;
        g_lcid_country() = lcid;
        g_lcid_language() = lcid;
    }
    return enum_result();
}

// 0x007bfaef GetLcidFromLangCountry
void get_lcid_from_lang_country() {
    g_abbrev_language() = std::strlen(g_pch_language()) == 3 ? 1 : 0;
    g_abbrev_country() = std::strlen(g_pch_country()) == 3 ? 1 : 0;
    g_lcid_language() = 0;
    g_primary_len() = g_abbrev_language() != 0 ? 2 : get_primary_len(g_pch_language());
    nls::EnumSystemLocalesA(lang_country_enum_proc, LCID_INSTALLED);
    const u32 s = g_lcid_state();
    if ((s & 0x100u) == 0 || (s & 0x200u) == 0 || (s & 7u) == 0) g_lcid_state() = 0;
}

// 0x007bfd7a GetLcidFromLanguage
void get_lcid_from_language() {
    g_abbrev_language() = std::strlen(g_pch_language()) == 3 ? 1 : 0;
    g_primary_len() = g_abbrev_language() != 0 ? 2 : get_primary_len(g_pch_language());
    nls::EnumSystemLocalesA(language_enum_proc, LCID_INSTALLED);
    if ((g_lcid_state() & 4u) == 0) g_lcid_state() = 0;
}

// 0x007bfe8d GetLcidFromCountry
void get_lcid_from_country() {
    g_abbrev_country() = std::strlen(g_pch_country()) == 3 ? 1 : 0;
    nls::EnumSystemLocalesA(country_enum_proc, LCID_INSTALLED);
    if ((g_lcid_state() & 4u) == 0) g_lcid_state() = 0;
}

// 0x007bff4a GetLcidFromDefault
void get_lcid_from_default() {
    g_lcid_state() |= 0x104u;
    const u32 lcid = nls::GetUserDefaultLCID();
    g_lcid_country() = lcid;
    g_lcid_language() = lcid;
}

// 0x007bff64 ProcessCodePage
int process_code_page(const char* cp_str) {
    char cp_buf[8];
    if (cp_str == nullptr || *cp_str == '\0' || std::strcmp(cp_str, "ACP") == 0) {
        if (g_pfn_get_locale_info_a()(g_lcid_country(), LOCALE_IDEFAULTANSICODEPAGE, cp_buf, 8) == 0) return 0;
        cp_str = cp_buf;
    } else if (std::strcmp(cp_str, "OCP") == 0) {
        if (g_pfn_get_locale_info_a()(g_lcid_country(), LOCALE_IDEFAULTCODEPAGE, cp_buf, 8) == 0) return 0;
        cp_str = cp_buf;
    }
    return static_cast<int>(crt_atol(cp_str));
}

// LC_STRINGS (0x88 bytes in the original; the code-page part is padded here, see header).
struct LcStrings {
    char language[0x40];
    char country[0x40];
    char codepage[8 + 0x100];
};

// 0x007bf91a __get_qualified_locale
int get_qualified_locale(LcStrings* in, LcId* out_id, LcStrings* out_str) {
    if (g_pfn_get_locale_info_a() == nullptr)
        g_pfn_get_locale_info_a() = is_this_windows_nt() ? nls::GetLocaleInfoA_address() : &crt_get_locale_info_a_9x;
    if (in == nullptr) {
        get_lcid_from_default();
    } else {
        g_pch_language() = in->language;
        if (*in->language != '\0') translate_name(kLanguages, 0x40, &g_pch_language());
        g_pch_country() = in->country;
        if (*g_pch_country() != '\0') translate_name(kCountries, 0x16, &g_pch_country());
        g_lcid_state() = 0;
        const char* lang = g_pch_language();
        const char* ctry = g_pch_country();
        if (*lang != '\0') {
            if (*ctry != '\0')
                get_lcid_from_lang_country();
            else
                get_lcid_from_language();
        } else if (*ctry != '\0') {
            get_lcid_from_country();
        } else {
            get_lcid_from_default();
        }
    }
    if (g_lcid_state() == 0) return 0;
    const int cp = process_code_page(in->codepage);
    if (cp == 0) return 0;
    if (nls::IsValidCodePage(static_cast<u16>(cp)) == 0) return 0;
    if (nls::IsValidLocale(g_lcid_language(), LCID_INSTALLED) == 0) return 0;
    if (out_id != nullptr) {
        out_id->language = static_cast<u16>(g_lcid_language());
        out_id->country = static_cast<u16>(g_lcid_country());
        out_id->codepage = static_cast<u16>(cp);
    }
    if (out_str != nullptr) {
        if (g_pfn_get_locale_info_a()(g_lcid_language(), LOCALE_SENGLANGUAGE, out_str->language, 0x40) == 0) return 0;
        if (g_pfn_get_locale_info_a()(g_lcid_country(), LOCALE_SENGCOUNTRY, out_str->country, 0x40) == 0) return 0;
        crt_itoa(cp, out_str->codepage, 10);
    }
    return 1;
}

// ------------------------------------------------------------------ setlocale.c helpers
// 0x007b57c3 __lc_strtolc
int lc_strtolc(LcStrings* names, const char* locale) {
    std::memset(names, 0, sizeof *names);  // 0x88 in the original (padding: see header)
    if (*locale == '\0') return 0;
    if (locale[0] == '.' && locale[1] != '\0') {
        std::strcpy(names->codepage, locale + 1);
        return 0;
    }
    for (int i = 0;; ++i) {
        const std::size_t len = std::strcspn(locale, "_.,");
        if (len == 0) return -1;
        const char ch = locale[len];
        if (i == 0 && len < 0x40 && ch != '.')
            std::strncpy(names->language, locale, len);
        else if (i == 1 && len < 0x40 && ch != '_')
            std::strncpy(names->country, locale, len);
        else if (i == 2 && (ch == '\0' || ch == ','))
            std::strncpy(names->codepage, locale, len);
        else
            return -1;
        if (ch == ',' || ch == '\0') return 0;
        locale += len + 1;
    }
}

// 0x007b588f __lc_lctostr
void lc_lctostr(char* out, const LcStrings* names) {
    std::strcpy(out, names->language);
    if (names->country[0] != '\0') {
        std::strcat(out, "_");
        std::strcat(out, names->country);
    }
    if (names->codepage[0] != '\0') {
        std::strcat(out, ".");
        std::strcat(out, names->codepage);
    }
}

// 0x007b5683 _expandlocale (the 5th argument, the category, is not used)
char* expandlocale(const char* expr, char* output, LcId* id, u32* cp) {
    if (expr == nullptr) return nullptr;
    if (expr[0] == 'C' && expr[1] == '\0') {
        output[1] = '\0';
        output[0] = 'C';
        if (id != nullptr) {
            id->language = 0;
            id->country = 0;
            id->codepage = 0;
        }
        if (cp != nullptr) *cp = 0;
        return output;
    }
    if (std::strcmp(cacheout(), expr) != 0 && std::strcmp(cachein(), expr) != 0) {
        LcStrings names;
        if (lc_strtolc(&names, expr) != 0) return nullptr;
        if (get_qualified_locale(&names, cacheid(), &names) == 0) return nullptr;
        cachecp() = cacheid()->codepage;
        lc_lctostr(cacheout(), &names);
        std::strcpy(cachein(), *expr != '\0' ? expr : cacheout());
    }
    if (id != nullptr) std::memcpy(id, cacheid(), 6);
    if (cp != nullptr) std::memcpy(cp, &cachecp(), 4);
    std::strcpy(output, cacheout());
    return cacheout();
}

// ------------------------------------------------------------------ category initialisers
int JOAN_CDECL init_dummy() { return 0; }    // 0x007b579b (LC_ALL)
int JOAN_CDECL init_collate() { return 0; }  // 0x007bf8db

// 0x007bf611 / 0x007bf3ed fix_grouping: "3;2;0" -> {3, 2, 0}
void fix_grouping(char* grouping) {
    while (*grouping != '\0') {
        if (*grouping >= '0' && *grouping <= '9') {
            *grouping = static_cast<char>(*grouping - '0');
            ++grouping;
        } else if (*grouping == ';') {
            char* tmp = grouping;
            do {
                *tmp = *(tmp + 1);
            } while (*++tmp != '\0');
        } else {
            ++grouping;
        }
    }
}

// 0x007bf696 __init_ctype
int JOAN_CDECL init_ctype() {
    if (lc_handle()[2] == 0) {
        pctype() = &img::global<u16>(kCtypeC);
        pwctype() = &img::global<u16>(kCtypeC);
        crt_free(s_ctype1());
        crt_free(s_wctype1());
        s_ctype1() = nullptr;
        s_wctype1() = nullptr;
        return 0;
    }
    u16* newctype1 = nullptr;
    u16* newwctype1 = nullptr;
    u8* cbuffer = nullptr;
    u16* wbuffer = nullptr;
    CPINFO cpinfo;
    if (lc_codepage() == 0 && crt_getlocaleinfo(0, lc_id()[2].language, LOCALE_IDEFAULTANSICODEPAGE, &lc_codepage()) != 0)
        goto error_cleanup;
    newctype1 = static_cast<u16*>(crt_malloc(0x202));
    newwctype1 = static_cast<u16*>(crt_malloc(0x202));
    cbuffer = static_cast<u8*>(crt_malloc(0x101));
    wbuffer = static_cast<u16*>(crt_malloc(0x202));
    if (newctype1 == nullptr || newwctype1 == nullptr || cbuffer == nullptr || wbuffer == nullptr) goto error_cleanup;
    for (int i = 0; i < 0x100; ++i) cbuffer[i] = static_cast<u8>(i);
    if (nls::GetCPInfo(lc_codepage(), &cpinfo) == 0) goto error_cleanup;
    if (cpinfo.MaxCharSize > 2) goto error_cleanup;
    mb_cur_max() = static_cast<u16>(cpinfo.MaxCharSize);
    if (mb_cur_max() > 1) {
        for (const BYTE* cp = cpinfo.LeadByte; cp[0] != 0 && cp[1] != 0; cp += 2)
            for (int i = cp[0]; i <= cp[1]; ++i) cbuffer[i] = 0;
    }
    if (crt_get_string_type_a(CT_CTYPE1, reinterpret_cast<const char*>(cbuffer), 0x100, newctype1 + 1, 0, 0, 0) == 0)
        goto error_cleanup;
    newctype1[0] = 0;
    for (int i = 0; i < 0x100; ++i) wbuffer[i] = static_cast<u16>(i);
    if (crt_get_string_type_w(CT_CTYPE1, reinterpret_cast<const WCHAR*>(wbuffer), 0x100, newwctype1 + 1, 0, 0) == 0)
        goto error_cleanup;
    newwctype1[0] = 0;
    if (mb_cur_max() > 1) {
        for (const BYTE* cp = cpinfo.LeadByte; cp[0] != 0 && cp[1] != 0; cp += 2)
            for (int i = cp[0]; i <= cp[1]; ++i) newctype1[i + 1] = 0x8000;
    }
    pwctype() = newwctype1 + 1;
    pctype() = newctype1 + 1;
    if (s_ctype1() != nullptr) crt_free(s_ctype1());
    s_ctype1() = newctype1;
    if (s_wctype1() != nullptr) crt_free(s_wctype1());
    s_wctype1() = newwctype1;
    crt_free(cbuffer);
    crt_free(wbuffer);
    return 0;
error_cleanup:
    crt_free(newctype1);
    crt_free(newwctype1);
    crt_free(cbuffer);
    crt_free(wbuffer);
    return 1;
}

// 0x007bf218 __init_numeric
int JOAN_CDECL init_numeric() {
    const u32 ctryid = lc_id()[4].country;
    if (lc_handle()[4] != 0) {
        int ret = crt_getlocaleinfo(1, ctryid, LOCALE_SDECIMAL, &s_dec_pnt());
        ret |= crt_getlocaleinfo(1, ctryid, LOCALE_STHOUSAND, &s_thous_sep());
        ret |= crt_getlocaleinfo(1, ctryid, LOCALE_SGROUPING, &s_grping());
        fix_grouping(s_grping());
        if (ret != 0) {
            crt_free(s_dec_pnt());
            crt_free(s_thous_sep());
            crt_free(s_grping());
            s_dec_pnt() = nullptr;
            s_thous_sep() = nullptr;
            s_grping() = nullptr;
            return -1;
        }
        if (lconv_ptr()->decimal_point != &img::global<char>(kLconvStaticDecimal)) {
            crt_free(lconv_ptr()->decimal_point);
            crt_free(lconv_ptr()->thousands_sep);
            crt_free(lconv_ptr()->grouping);
        }
        lconv_ptr()->decimal_point = s_dec_pnt();
        lconv_ptr()->thousands_sep = s_thous_sep();
        lconv_ptr()->grouping = s_grping();
        decimal_point_length() = 1;
        decimal_point() = *lconv_ptr()->decimal_point;
        return 0;
    }
    crt_free(s_dec_pnt());
    crt_free(s_thous_sep());
    crt_free(s_grping());
    s_dec_pnt() = nullptr;
    s_thous_sep() = nullptr;
    s_grping() = nullptr;
    lconv_ptr()->decimal_point = static_cast<char*>(crt_malloc(2));
    if (lconv_ptr()->decimal_point == nullptr) return -1;
    std::strcpy(lconv_ptr()->decimal_point, ".");
    lconv_ptr()->thousands_sep = static_cast<char*>(crt_malloc(2));
    if (lconv_ptr()->thousands_sep == nullptr) return -1;
    *lconv_ptr()->thousands_sep = '\0';
    lconv_ptr()->grouping = static_cast<char*>(crt_malloc(2));
    if (lconv_ptr()->grouping == nullptr) return -1;
    *lconv_ptr()->grouping = '\0';
    decimal_point_length() = 1;
    decimal_point() = *lconv_ptr()->decimal_point;
    return 0;
}

// 0x007bf648 __free_lconv_mon
void free_lconv_mon(Lconv* l) {
    if (l == nullptr || l->int_curr_symbol == &img::global<char>(kLconvStaticNull)) return;
    crt_free(l->int_curr_symbol);
    crt_free(l->currency_symbol);
    crt_free(l->mon_decimal_point);
    crt_free(l->mon_thousands_sep);
    crt_free(l->mon_grouping);
    crt_free(l->positive_sign);
    crt_free(l->negative_sign);
}

// 0x007bf4ef _get_lc_lconv
int get_lc_lconv(Lconv* l) {
    const u32 ctryid = lc_id()[3].country;
    if (l == nullptr) return -1;
    int ret = crt_getlocaleinfo(1, ctryid, LOCALE_SINTLSYMBOL, &l->int_curr_symbol);
    ret |= crt_getlocaleinfo(1, ctryid, LOCALE_SCURRENCY, &l->currency_symbol);
    ret |= crt_getlocaleinfo(1, ctryid, LOCALE_SMONDECIMALSEP, &l->mon_decimal_point);
    ret |= crt_getlocaleinfo(1, ctryid, LOCALE_SMONTHOUSANDSEP, &l->mon_thousands_sep);
    ret |= crt_getlocaleinfo(1, ctryid, LOCALE_SMONGROUPING, &l->mon_grouping);
    fix_grouping(l->mon_grouping);
    ret |= crt_getlocaleinfo(1, ctryid, LOCALE_SPOSITIVESIGN, &l->positive_sign);
    ret |= crt_getlocaleinfo(1, ctryid, LOCALE_SNEGATIVESIGN, &l->negative_sign);
    ret |= crt_getlocaleinfo(0, ctryid, LOCALE_IINTLCURRDIGITS, &l->int_frac_digits);
    ret |= crt_getlocaleinfo(0, ctryid, LOCALE_ICURRDIGITS, &l->frac_digits);
    ret |= crt_getlocaleinfo(0, ctryid, LOCALE_IPOSSYMPRECEDES, &l->p_cs_precedes);
    ret |= crt_getlocaleinfo(0, ctryid, LOCALE_IPOSSEPBYSPACE, &l->p_sep_by_space);
    ret |= crt_getlocaleinfo(0, ctryid, LOCALE_INEGSYMPRECEDES, &l->n_cs_precedes);
    ret |= crt_getlocaleinfo(0, ctryid, LOCALE_INEGSEPBYSPACE, &l->n_sep_by_space);
    ret |= crt_getlocaleinfo(0, ctryid, LOCALE_IPOSSIGNPOSN, &l->p_sign_posn);
    ret |= crt_getlocaleinfo(0, ctryid, LOCALE_INEGSIGNPOSN, &l->n_sign_posn);
    return ret;
}

// 0x007bf424 __init_monetary
int JOAN_CDECL init_monetary() {
    if (lc_handle()[3] != 0) {
        Lconv* lc = static_cast<Lconv*>(crt_calloc(1, sizeof(Lconv)));
        if (lc == nullptr) return 1;
        if (get_lc_lconv(lc) != 0) {
            free_lconv_mon(lc);
            crt_free(lc);
            return 1;
        }
        lc->decimal_point = lconv_ptr()->decimal_point;
        lc->thousands_sep = lconv_ptr()->thousands_sep;
        lc->grouping = lconv_ptr()->grouping;
        lconv_ptr() = lc;
        free_lconv_mon(s_lconv_intl());
        crt_free(s_lconv_intl());
        s_lconv_intl() = lc;
        return 0;
    }
    Lconv& c = lconv_c();
    c.decimal_point = lconv_ptr()->decimal_point;
    c.thousands_sep = lconv_ptr()->thousands_sep;
    c.grouping = lconv_ptr()->grouping;
    lconv_ptr() = &c;
    free_lconv_mon(s_lconv_intl());
    crt_free(s_lconv_intl());
    s_lconv_intl() = nullptr;
    return 0;
}

// struct __lc_time_data (0xac bytes on x86): 43 string pointers (+ the 4-byte flag at 0xa8 is the 43rd here); the
// tables below give each field's x86 offset, i.e. slot off / 4.
constexpr int kLcTimeSize = static_cast<int>(43 * sizeof(char*));
char** lc_time_field(void* t, int off) { return reinterpret_cast<char**>(static_cast<u8*>(t) + off * static_cast<int>(sizeof(char*) / 4)); }

// 0x007bf088 __free_lc_time
void free_lc_time(void* t) {
    if (t == nullptr) return;
    static const int kOrder[] = {0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x00, 0x20, 0x24, 0x28, 0x2c,
                                 0x30, 0x34, 0x1c, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4c, 0x50, 0x54,
                                 0x58, 0x5c, 0x60, 0x64, 0x68, 0x6c, 0x70, 0x74, 0x78, 0x7c, 0x80,
                                 0x84, 0x88, 0x8c, 0x90, 0x94, 0x98, 0x9c, 0xa0, 0xa4, 0xa8};
    for (int off : kOrder) crt_free(*lc_time_field(t, off));
}

// 0x007bed3a _get_lc_time
int get_lc_time(void* t) {
    const u32 langid = lc_id()[5].language;
    const u32 ctryid = lc_id()[5].country;
    if (t == nullptr) return -1;
    struct Field {
        int off;
        LCTYPE type;
        bool country;
    };
    static const Field kFields[] = {
        {0x04, 0x31, false}, {0x08, 0x32, false}, {0x0c, 0x33, false}, {0x10, 0x34, false},
        {0x14, 0x35, false}, {0x18, 0x36, false}, {0x00, 0x37, false},  // abbreviated day names
        {0x20, 0x2a, false}, {0x24, 0x2b, false}, {0x28, 0x2c, false}, {0x2c, 0x2d, false},
        {0x30, 0x2e, false}, {0x34, 0x2f, false}, {0x1c, 0x30, false},  // day names
        {0x38, 0x44, false}, {0x3c, 0x45, false}, {0x40, 0x46, false}, {0x44, 0x47, false},
        {0x48, 0x48, false}, {0x4c, 0x49, false}, {0x50, 0x4a, false}, {0x54, 0x4b, false},
        {0x58, 0x4c, false}, {0x5c, 0x4d, false}, {0x60, 0x4e, false}, {0x64, 0x4f, false},  // abbreviated months
        {0x68, 0x38, false}, {0x6c, 0x39, false}, {0x70, 0x3a, false}, {0x74, 0x3b, false},
        {0x78, 0x3c, false}, {0x7c, 0x3d, false}, {0x80, 0x3e, false}, {0x84, 0x3f, false},
        {0x88, 0x40, false}, {0x8c, 0x41, false}, {0x90, 0x42, false}, {0x94, 0x43, false},  // months
        {0x98, 0x28, false}, {0x9c, 0x29, false},                                            // AM / PM
        {0xa0, 0x1f, true},  {0xa4, 0x20, true},  {0xa8, 0x1003, true},  // short / long date, time format
    };
    int ret = 0;
    for (const Field& f : kFields) ret |= crt_getlocaleinfo(1, f.country ? ctryid : langid, f.type, lc_time_field(t, f.off));
    return ret;
}

// 0x007beca9 __init_time
int JOAN_CDECL init_time() {
    if (lc_handle()[5] != 0) {
        void* lc_time_new = crt_calloc(1, kLcTimeSize);
        if (lc_time_new == nullptr) return 1;
        if (get_lc_time(lc_time_new) != 0) {
            free_lc_time(lc_time_new);
            crt_free(lc_time_new);
            return 1;
        }
        lc_time_curr() = lc_time_new;
        free_lc_time(s_lc_time());
        crt_free(s_lc_time());
        s_lc_time() = lc_time_new;
        return 0;
    }
    lc_time_curr() = img::obj(kLcTimeC);
    free_lc_time(s_lc_time());
    crt_free(s_lc_time());
    s_lc_time() = nullptr;
    return 0;
}

using InitFn = int(JOAN_CDECL*)();
// __lc_category[].init (0x7b579b, 0x7bf8db, 0x7bf696, 0x7bf424, 0x7bf218, 0x7beca9)
const InitFn kCategoryInit[6] = {init_dummy, init_collate, init_ctype, init_monetary, init_numeric, init_time};

// ------------------------------------------------------------------ 0x007b54af _setlocale_set_cat
char* setlocale_set_cat(int category, const char* locale) {
    char lctemp[0x84 + 0x100];
    LcId id_temp;
    u32 cp_temp;
    if (expandlocale(locale, lctemp, &id_temp, &cp_temp) == nullptr) return nullptr;
    char* pch = static_cast<char*>(crt_malloc(std::strlen(lctemp) + 1));
    if (pch == nullptr) return nullptr;
    LcCategory& cat = lc_category()[category];
    char* const oldlocale = cat.locale;
    const u32 oldhandle = lc_handle()[category];
    const u32 oldcodepage = lc_codepage();
    cat.locale = std::strcpy(pch, lctemp);
    lc_handle()[category] = id_temp.language;
    std::memcpy(&lc_id()[category], &id_temp, 6);
    if (category == 2) lc_codepage() = cp_temp;
    if (category == 1) lc_collate_cp() = cp_temp;
    if (kCategoryInit[category]() != 0) {
        cat.locale = oldlocale;
        crt_free(pch);
        lc_handle()[category] = oldhandle;
        lc_codepage() = oldcodepage;
        return nullptr;
    }
    if (oldlocale != &img::global<char>(kCLocaleStr)) crt_free(oldlocale);
    return cat.locale;
}

// ------------------------------------------------------------------ 0x007b55ca _setlocale_get_all
char* setlocale_get_all() {
    LcCategory* cat = lc_category();
    int same = 1;
    if (cat[0].locale == nullptr) cat[0].locale = static_cast<char*>(crt_malloc(0x351));
    char* all = cat[0].locale;
    all[0] = '\0';  // the original does not check the allocation either
    std::strcat(all, cat[1].catname);
    std::strcat(all, "=");
    std::strcat(all, cat[1].locale);
    for (int i = 1; i < 5; ++i) {
        std::strcat(cat[0].locale, ";");
        if (std::strcmp(cat[i].locale, cat[i + 1].locale) != 0) same = 0;
        std::strcat(cat[0].locale, cat[i + 1].catname);
        std::strcat(cat[0].locale, "=");
        std::strcat(cat[0].locale, cat[i + 1].locale);
    }
    if (same == 0) return cat[0].locale;
    crt_free(cat[0].locale);
    cat[0].locale = nullptr;
    return cat[2].locale;
}

// setlocale(LC_ALL, locale) body (inside the lock)
char* setlocale_all(const char* locale) {
    LcCategory* cat = lc_category();
    char lctemp[0x88 + 0x100];
    int set_count = 0;
    if (locale == nullptr) return setlocale_get_all();
    if (locale[0] == 'L' && locale[1] == 'C' && locale[2] == '_') {
        const char* p = locale;
        for (;;) {
            const char* s = std::strpbrk(p, "=;");
            if (s == nullptr) return nullptr;
            std::size_t len = static_cast<std::size_t>(s - p);
            if (len == 0 || *s == ';') return nullptr;
            int i = 1;
            for (; i <= 5; ++i)
                if (std::strncmp(cat[i].catname, p, len) == 0 && len == std::strlen(cat[i].catname)) break;
            ++s;
            len = std::strcspn(s, ";");
            if (len == 0 && *s != ';') return nullptr;
            if (i <= 5) {
                std::strncpy(lctemp, s, len);
                lctemp[len] = '\0';
                if (setlocale_set_cat(i, lctemp) != nullptr) ++set_count;
            }
            p = s + len;
            if (*p == '\0') break;
            ++p;
            if (*p == '\0') break;
        }
        return set_count != 0 ? setlocale_get_all() : nullptr;
    }
    if (expandlocale(locale, lctemp, nullptr, nullptr) == nullptr) return nullptr;
    int same = 1;
    for (int i = 1; i <= 5; ++i) {
        if (std::strcmp(lctemp, cat[i].locale) != 0) {
            if (setlocale_set_cat(i, lctemp) != nullptr)
                ++set_count;
            else
                same = 0;
        } else {
            ++set_count;
        }
    }
    if (same != 0) {
        char* ret = setlocale_get_all();
        crt_free(cat[0].locale);
        cat[0].locale = nullptr;
        return ret;
    }
    return set_count != 0 ? setlocale_get_all() : nullptr;
}

// ------------------------------------------------------------------ 0x007b5269 setlocale
static char* JOAN_CDECL crt_setlocale(int category, const char* locale) {
    c_setlocale.hit();
    if (category < 0 || category > 5) return nullptr;
    crt_lock(0x13);
    setlc_active() = setlc_active() + 1;
    while (unguarded_readlc_active() != 0) nls::Sleep(1);
    char* ret;
    if (category != 0)
        ret = locale != nullptr ? setlocale_set_cat(category, locale) : lc_category()[category].locale;
    else
        ret = setlocale_all(locale);
    crt_unlock(0x13);
    setlc_active() = setlc_active() - 1;
    return ret;
}

// ------------------------------------------------------------------ mbctype.c
// 0x007b5012 getSystemCP
int get_system_cp(int codepage) {
    fsystemset() = 0;
    if (codepage == -2) {
        fsystemset() = 1;
        return static_cast<int>(nls::GetOEMCP());
    }
    if (codepage == -3) {
        fsystemset() = 1;
        return static_cast<int>(nls::GetACP());
    }
    if (codepage == -4) {
        fsystemset() = 1;
        return static_cast<int>(lc_codepage());
    }
    return codepage;
}

// 0x007b505c CPtoLCID
u32 cp_to_lcid(int codepage) {
    switch (codepage) {
    case 932: return 0x411;
    case 936: return 0x804;
    case 949: return 0x412;
    case 950: return 0x404;
    default: return 0;
    }
}

// 0x007b508f setSBCS
void set_sbcs() {
    std::memset(mbctype(), 0, 257);
    mbcodepage() = 0;
    ismbcodepage() = 0;
    mblcid() = 0;
    mbulinfo()[0] = 0;
    mbulinfo()[1] = 0;
    mbulinfo()[2] = 0;
}

// 0x007b50b8 setSBUpLow
void set_sb_up_low() {
    CPINFO cpinfo;
    if (nls::GetCPInfo(static_cast<UINT>(mbcodepage()), &cpinfo) == 1) {
        u8 sb_vector[0x100];
        u8 lower[0x100];
        u8 upper[0x100];
        WORD char_type[0x100];
        std::memset(lower, 0, sizeof lower);
        std::memset(upper, 0, sizeof upper);
        std::memset(char_type, 0, sizeof char_type);
        for (int i = 0; i < 0x100; ++i) sb_vector[i] = static_cast<u8>(i);
        sb_vector[0] = ' ';
        for (const BYTE* cp = cpinfo.LeadByte; cp[0] != 0; cp += 2) {
            const unsigned lo = cp[0];
            const unsigned hi = cp[1];
            if (lo <= hi) std::memset(sb_vector + lo, ' ', hi - lo + 1);
        }
        crt_get_string_type_a(CT_CTYPE1, reinterpret_cast<const char*>(sb_vector), 0x100, char_type, mbcodepage(),
                              static_cast<int>(mblcid()), 0);
        crt_lcmap_string_a(mblcid(), LCMAP_LOWERCASE, reinterpret_cast<const char*>(sb_vector), 0x100,
                           reinterpret_cast<char*>(lower), 0x100, mbcodepage(), 0);
        crt_lcmap_string_a(mblcid(), LCMAP_UPPERCASE, reinterpret_cast<const char*>(sb_vector), 0x100,
                           reinterpret_cast<char*>(upper), 0x100, mbcodepage(), 0);
        for (int i = 0; i < 0x100; ++i) {
            if ((char_type[i] & 1) != 0) {
                mbctype()[i + 1] |= 0x10;
                mbcasemap()[i] = lower[i];
            } else if ((char_type[i] & 2) != 0) {
                mbctype()[i + 1] |= 0x20;
                mbcasemap()[i] = upper[i];
            } else {
                mbcasemap()[i] = 0;
            }
        }
        return;
    }
    for (int i = 0; i < 0x100; ++i) {
        if (i >= 'A' && i <= 'Z') {
            mbctype()[i + 1] |= 0x10;
            mbcasemap()[i] = static_cast<u8>(i + 0x20);
        } else if (i >= 'a' && i <= 'z') {
            mbctype()[i + 1] |= 0x20;
            mbcasemap()[i] = static_cast<u8>(i - 0x20);
        } else {
            mbcasemap()[i] = 0;
        }
    }
}

// ------------------------------------------------------------------ 0x007b4e65 _setmbcp
static int JOAN_CDECL crt_setmbcp(int codepage) {
    c_setmbcp.hit();
    crt_lock(0x19);
    int retcode = 0;
    codepage = get_system_cp(codepage);
    if (codepage == mbcodepage()) {
        retcode = 0;
    } else if (codepage == 0) {
        set_sbcs();
        set_sb_up_low();
    } else {
        int icp = 0;
        while (icp < kNumCodePages && cpinfo_code_page(icp) != codepage) ++icp;
        if (icp < kNumCodePages) {
            std::memset(mbctype(), 0, 257);
            for (int irg = 0; irg < 4; ++irg) {
                for (const u8* r = cpinfo_range(icp, irg); r[0] != 0 && r[1] != 0; r += 2)
                    for (unsigned ich = r[0]; ich <= r[1]; ++ich) mbctype()[ich + 1] |= ctypeflag(irg);
            }
            ismbcodepage() = 1;
            mbcodepage() = codepage;
            std::memcpy(mbulinfo(), cpinfo_mbulinfo(icp), 12);
            mblcid() = cp_to_lcid(codepage);
            set_sb_up_low();
        } else {
            CPINFO cpinfo;
            if (nls::GetCPInfo(static_cast<UINT>(codepage), &cpinfo) == 1) {
                mblcid() = 0;
                std::memset(mbctype(), 0, 257);
                mbcodepage() = codepage;
                if (cpinfo.MaxCharSize > 1) {
                    for (const BYTE* cp = cpinfo.LeadByte; cp[0] != 0 && cp[1] != 0; cp += 2)
                        for (unsigned ich = cp[0]; ich <= cp[1]; ++ich) mbctype()[ich + 1] |= 0x04;
                    for (int ich = 1; ich < 0xff; ++ich) mbctype()[ich + 1] |= 0x08;
                    mblcid() = cp_to_lcid(codepage);
                    ismbcodepage() = 1;
                } else {
                    ismbcodepage() = 0;
                }
                mbulinfo()[0] = 0;
                mbulinfo()[1] = 0;
                mbulinfo()[2] = 0;
                set_sb_up_low();
            } else if (fsystemset() != 0) {
                set_sbcs();
                set_sb_up_low();
            } else {
                retcode = -1;
            }
        }
    }
    crt_unlock(0x19);
    return retcode;
}

// ------------------------------------------------------------------ 0x007b524d __initmbctable
static void JOAN_CDECL crt_initmbctable() {
    if (mbctype_initialized() == 0) {
        crt_setmbcp(-3);  // _MB_CP_ANSI
        mbctype_initialized() = 1;
    }
}

}  // namespace

JOAN_REPLACE("crt/locale", "crt::setlocale", 0x007b5269, &crt_setlocale);
JOAN_REPLACE("crt/locale", "crt::_setmbcp", 0x007b4e65, &crt_setmbcp);
JOAN_REPLACE("crt/locale", "crt::__initmbctable", 0x007b524d, &crt_initmbctable);
JOAN_REPLACE("crt/locale", "crt::__crtLCMapStringA", 0x007b7fc6, &crt_lcmap_string_a);
JOAN_REPLACE("crt/locale", "crt::__crtLCMapStringW", 0x007c585f, &crt_lcmap_string_w);
JOAN_REPLACE("crt/locale", "crt::__crtGetStringTypeA", 0x007beb60, &crt_get_string_type_a);
JOAN_REPLACE("crt/locale", "crt::__crtGetStringTypeW", 0x007c456b, &crt_get_string_type_w);
JOAN_REPLACE("crt/locale", "crt::__crtCompareStringA", 0x007c026c, &crt_compare_string_a);
JOAN_REPLACE("crt/locale", "crt::__crtCompareStringW", 0x007c0514, &crt_compare_string_w);
JOAN_REPLACE("crt/locale", "crt::__getlocaleinfo", 0x007c442d, &crt_getlocaleinfo);
JOAN_REPLACE("crt/locale", "crt::__crtGetLocaleInfoW", 0x007c562e, &crt_get_locale_info_w);
JOAN_REPLACE("crt/locale", "crt::__crtGetLocaleInfoA", 0x007c5741, &crt_get_locale_info_a);

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::_setmbcp(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_setmbcp>(a0)); }  // 0x7b4e65
auto joan::orig::libc::__initmbctable() -> void { joan::bind::free<&::crt_initmbctable>(); }  // 0x7b524d
auto joan::orig::libc::setlocale(std::int32_t a0, const char* a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_setlocale>(a0, a1)); }  // 0x7b5269
