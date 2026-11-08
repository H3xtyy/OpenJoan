// Original library sources (re/attribution.csv original_file, under lib/): crt/ctype.c, crt/intel/_stricmp.asm,
//   crt/intel/_strnicm.asm, crt/isctype.c, crt/iswctype.c, crt/strupr.c, crt/tolower.c, crt/toupper.c,
//   crt/towlower.c, crt/wcsicmp.c, crt/wcslwr.c, crt/wcsnicmp.c, crt/wcsupr.c.
//
// Batch 114 / writer 06: family crt/ctype (CRT_SYSTEM.md family E2, part 1) - the MSVC 6 CRT character
// classification / case-mapping READERS of the locale state. Entry-patched (library code has no thunks).
//
// Surface replaced here (module "crt/ctype"), every entry still reachable from original code / our C++:
//   0x007b3b0a int isalpha(int)            callers 0x4886e4 0x662477 0x6624c2 0x78a710 0x78b0a6 (+ C++ b104_02, b111_01)
//   0x007b3b88 int isdigit(int)            callers 0x6623ae 0x6624ec 0x67d70f 0x787b01 0x78a72e 0x78b185 0x78b1af (+ C++)
//   0x007b3bdd int isspace(int)            callers 0x68414b 0x6841c1 0x787abc 0x787af3 0x78ae8f 0x78af0c 0x78af2d
//                                          0x78af53, _input 0x7bc2e9 0x7bc2fb 0x7bcd14 (+ C++ b103_14)
//   0x007b3c2d int isalnum(int)            caller 0x78b0c9
//   0x007b4534 int toupper(int)            7 game sites + pointer stored in game table 0xa15830 (0x58b429..)
//   0x007b45a3 int _toupper_lk(int)        caller toupper
//   0x007b4ce3 int tolower(int)            0x599ede, 0x7b8e85 (_forcdecpt-area CRT code) + pointer in 0xa1582c
//   0x007b4d52 int _tolower_lk(int)        callers tolower, _stricmp, _strnicmp
//   0x007b7f51 int _isctype(int c, int mask)   37 CRT sites (atol, is*, _input, _toupper_lk, ...) + C++ atoi
//   0x007b1800 char* _strupr(char*)        15 game sites (+ C++)
//   0x007c5dc0 char* _strlwr(char*)        0x58e412 0x5a136e (+ C++)
//   0x007c07b0 int _stricmp(const char*, const char*)              1161 game sites + CRT (TranslateName, _stricoll)
//   0x007c0960 int _strnicmp(const char*, const char*, size_t)     0x4d1493 0x50d1e1 0x67d760 0x67d788 + CRT
//   0x007c0880 int _wcsicmp(const wchar_t*, const wchar_t*)        caller _wcsicoll 0x7b673b
//   0x007c0a61 int _wcsnicmp(const wchar_t*, const wchar_t*, size_t)   caller _wcsnicoll 0x7b6942
//   0x007c5ef4 wchar_t* _wcslwr(wchar_t*)  caller 0x58e472 (C++ b103_10)
//   0x007c6031 wchar_t* _wcsupr(wchar_t*)  caller 0x58e5b2 (C++ b103_10)
//   0x007c498e wint_t _towlower_lk(wint_t)     callers _wcsicmp, _wcsnicmp, towlower 0x7c4914 (uncalled)
//   0x007c5a98 int iswctype(wint_t, wctype_t)  callers _towlower_lk, is_wctype 0x7c5aea (uncalled)
// Left original, unreachable (no call / jmp / address constant anywhere, not used by our C++): isupper 0x7b3b38,
// islower 0x7b3b60, isxdigit 0x7b3bb0, ispunct 0x7b3c05, isprint 0x7b3c5b, isgraph 0x7b3c89, iscntrl 0x7b3cb7,
// __isascii 0x7b3cdf, __toascii 0x7b3cec, __iscsymf 0x7b3cf4, __iscsym 0x7b3d31, _toupper 0x7b452c,
// _tolower 0x7b4cdb, towlower 0x7c4914, is_wctype 0x7c5aea.
// Collation (_stricoll / _wcsicoll / ...) belongs to crt/mbs; they call _stricmp/_wcsicmp/... by address.
//
// State: none of our own. Everything is read at the ORIGINAL addresses, written by setlocale / _setmbcp
// (original or crt/locale): __lc_handle[LC_CTYPE] 0xa6b2b8, _pctype 0xa30cc4, _pwctype 0xa30cc8,
// __mb_cur_max 0xa30ed0; setlocale guard: __unguarded_readlc_active 0xa6ca8c (incremented / decremented
// through the InterlockedIncrement / InterlockedDecrement IAT slots 0xa6d930 / 0xa6d934 - the asm-written
// _stricmp/_strnicmp use an inline `lock inc/dec` instead, same atomic effect) and __setlc_active 0xa6ca7c
// (toupper/tolower/_strupr/_strlwr/_wcs*: != 0; _stricmp/_strnicmp: > 0 signed, as the originals).
// Lock 0x13 (_SETLOCALE_LOCK) through _lock 0x7b82aa / _unlock 0x7b830b. NLS work through the original
// wrapper addresses: __crtLCMapStringA 0x7b7fc6, __crtGetStringTypeA 0x7beb60, __crtLCMapStringW 0x7c585f,
// __crtGetStringTypeW 0x7c456b. Heap: malloc 0x7b6c54 / free 0x7b4e1d; strcpy 0x7b2020 / wcscpy 0x7b5a9a.
//
// Quirks reproduced deliberately:
// - _isctype's multibyte path lets __crtGetStringTypeA write its output words into the caller's argument
//   area ([ebp+0xa]); for a lead-byte pair a second word lands on the low half of `mask` ([ebp+0xc]),
//   which is then used for the final AND. Emulated with an 8-byte copy of (c, mask).
// - _stricmp / _strnicmp locale paths reload only the LOW byte of eax/ebx per character, so the upper bits
//   of the previous _tolower_lk result (non-zero only if LCMapString produced a 2-byte result) feed the next
//   _tolower_lk argument, the loop-exit tests (_strnicmp tests the full register) and _stricmp's return
//   value. Emulated with 32-bit register variables.
// - Negative / out-of-range arguments index _pctype exactly like the originals (32-bit wrap-around).
// Register contract: the originals clobber eax/ecx/edx only (callers are compiled C; none keeps ecx/edx
// live across these calls), DF untouched - the C++ cdecl versions are drop-in.
#include "../../callconv.hpp"
#include "../../platform/win32_types.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "crt.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
template <class T>
volatile T& g(img::Addr address) {
    return img::global<volatile T>(address);
}

// --------------------------------------------------------------------------- original state / helpers
constexpr img::Addr kLcCtype = d::g_lcCtype;         // __lc_handle[LC_CTYPE]
constexpr img::Addr kPctype = d::g_pctype;          // _pctype (const unsigned short*)
constexpr img::Addr kPwctype = d::g_pwctype;         // _pwctype
constexpr img::Addr kMbCurMax = d::g_mb_cur_max;        // __mb_cur_max
constexpr img::Addr kSetlcActive = d::g_setlc_active;     // __setlc_active
constexpr img::Addr kUnguardedReadlc = d::g_unguarded_readlc_active; // __unguarded_readlc_active
constexpr img::Addr kIatInterlockedIncrement = d::iat_KERNEL32_InterlockedIncrement;
constexpr img::Addr kIatInterlockedDecrement = d::iat_KERNEL32_InterlockedDecrement;
constexpr i32 kSetlocaleLock = 0x13;

constexpr fn::Code kLock = fn::crt_lock;
constexpr fn::Code kUnlock = fn::crt_unlock;
constexpr fn::Code kIsctype = fn::crt_isctype;
constexpr fn::Code kToupperLk = fn::crt_toupper_lk;
constexpr fn::Code kTolowerLk = fn::crt_tolower_lk;
constexpr fn::Code kTowlowerLk = fn::crt_towlower_lk;
constexpr fn::Code kIswctype = fn::crt_iswctype;
constexpr fn::Code kCrtLCMapStringA = fn::crt_crtLCMapStringA;
constexpr fn::Code kCrtGetStringTypeA = fn::crt_crtGetStringTypeA;
constexpr fn::Code kCrtLCMapStringW = fn::crt_crtLCMapStringW;
constexpr fn::Code kCrtGetStringTypeW = fn::crt_crtGetStringTypeW;
constexpr fn::Code kMalloc = fn::malloc;
constexpr fn::Code kFree = fn::free;
constexpr fn::Code kStrcpy = fn::crt_strcpy;
constexpr fn::Code kWcscpy = fn::crt_wcscpy;

constexpr u32 kLcmapLowercase = 0x100;
constexpr u32 kLcmapUppercase = 0x200;

void readlc_inc() {
    using Fn = std::int32_t(JOAN_STDCALL*)(volatile std::int32_t*);  // LONG: 32 bits
    reinterpret_cast<Fn>(g<std::uintptr_t>(kIatInterlockedIncrement))(&img::global<volatile std::int32_t>(kUnguardedReadlc));
}
void readlc_dec() {
    using Fn = std::int32_t(JOAN_STDCALL*)(volatile std::int32_t*);  // LONG: 32 bits
    reinterpret_cast<Fn>(g<std::uintptr_t>(kIatInterlockedDecrement))(&img::global<volatile std::int32_t>(kUnguardedReadlc));
}

// The setlocale guard of the C-written readers (toupper, _strupr, _wcslwr, ...):
//   InterlockedIncrement(&__unguarded_readlc_active);
//   if (__setlc_active) { InterlockedDecrement(&...); _lock(_SETLOCALE_LOCK); local_lock_flag = 1; }
// `signed_test` selects the asm readers' variant (_stricmp / _strnicmp: `cmp [__setlc_active],0; jg`).
bool guard_enter(bool signed_test) {
    readlc_inc();
    const i32 active = g<i32>(kSetlcActive);
    if (signed_test ? active > 0 : active != 0) {
        readlc_dec();
        cdecl_call(kLock, kSetlocaleLock);
        return true;
    }
    return false;
}
void guard_leave(bool locked) {
    if (locked)
        cdecl_call(kUnlock, kSetlocaleLock);
    else
        readlc_dec();
}

// _pctype[c] with the original's 32-bit address arithmetic (c may be negative / out of range): the 32-bit byte
// offset is signed, so a negative c reads before the table as the wrapping x86 sum does.
using Pctype = const volatile u8*;
Pctype pctype() { return g<Pctype>(kPctype); }
u8 pctype_byte(Pctype pct, i32 c, u32 extra) { return pct[static_cast<i32>(static_cast<u32>(c) * 2u + extra)]; }
u16 pctype_word(Pctype pct, i32 c) {
    return *reinterpret_cast<const volatile u16*>(pct + static_cast<i32>(static_cast<u32>(c) * 2u));
}

JOAN_COUNTER(c_isctype_mb, "crt::_isctype (multibyte path)");
JOAN_COUNTER(c_case_lk_nls, "crt::_toupper_lk/_tolower_lk (NLS path)");
JOAN_COUNTER(c_stricmp_nls, "crt::_stricmp/_strnicmp (locale path)");
JOAN_COUNTER(c_strcase, "crt::_strupr/_strlwr/_wcsupr/_wcslwr");
JOAN_COUNTER(c_wcsicmp, "crt::_wcsicmp/_wcsnicmp");

// ===================================================================== 0x007b7f51 _isctype(c, mask)
static int JOAN_CDECL crt_isctype(int c, int mask) {
    const Pctype pct = pctype();
    if (static_cast<u32>(c) + 1u <= 0x100u)  // lea ecx,[eax+1]; cmp ecx,0x100; ja
        return static_cast<int>(static_cast<u32>(pctype_word(pct, c)) & static_cast<u32>(mask));
    c_isctype_mb.hit();
    const u8 hi = static_cast<u8>(static_cast<u32>(c >> 8));  // sar ecx,8; movzx edx,cl
    u8 src[4] = {0, 0, 0, 0};                                // [ebp-4] (byte 3 never read)
    int len;
    if (pctype_byte(pct, hi, 1) & 0x80) {  // _LEADBYTE
        src[0] = hi;
        src[1] = static_cast<u8>(c);
        src[2] = 0;
        len = 2;
    } else {
        src[0] = static_cast<u8>(c);
        src[1] = 0;
        len = 1;
    }
    // The output pointer is [ebp+0xa]: inside the caller's argument area (c at +8, mask at +0xc).
    u32 args[2] = {static_cast<u32>(c), static_cast<u32>(mask)};
    u8* out = reinterpret_cast<u8*>(args) + 2;
    // __crtGetStringTypeA(dwInfoType = CT_CTYPE1, src, len, out, code_page = 0, lcid = 0, bError = TRUE)
    const int ok = cdecl_call<int>(kCrtGetStringTypeA, 1, src, len, out, 0, 0, 1);
    if (ok == 0)
        return 0;
    u16 type;
    u32 cur_mask;
    std::memcpy(&type, out, 2);
    std::memcpy(&cur_mask, reinterpret_cast<u8*>(args) + 4, 4);
    return static_cast<int>(static_cast<u32>(type) & cur_mask);
}

// ===================================================================== is* (ctype.c)
// if (__mb_cur_max > 1) return _isctype(c, mask); else return _pctype[c] & mask;
static int JOAN_CDECL crt_isalpha(int c) {
    if (g<i32>(kMbCurMax) > 1)
        return cdecl_call<int>(kIsctype, c, 0x103);
    return pctype_word(pctype(), c) & 0x103;
}
static int JOAN_CDECL crt_isdigit(int c) {
    if (g<i32>(kMbCurMax) > 1)
        return cdecl_call<int>(kIsctype, c, 0x4);
    return pctype_byte(pctype(), c, 0) & 0x4;
}
static int JOAN_CDECL crt_isspace(int c) {
    if (g<i32>(kMbCurMax) > 1)
        return cdecl_call<int>(kIsctype, c, 0x8);
    return pctype_byte(pctype(), c, 0) & 0x8;
}
static int JOAN_CDECL crt_isalnum(int c) {
    if (g<i32>(kMbCurMax) > 1)
        return cdecl_call<int>(kIsctype, c, 0x107);
    return pctype_word(pctype(), c) & 0x107;
}

// ===================================================================== _toupper_lk / _tolower_lk
// Common NLS part (from `cmp ebx,0x100; jge` on): returns the mapped character or c.
int case_lk(int c, int ctype_mask, u32 lcmap_flags) {
    if (c < 0x100) {
        int t;
        if (g<i32>(kMbCurMax) > 1)
            t = cdecl_call<int>(kIsctype, c, ctype_mask);
        else
            t = pctype_byte(pctype(), c, 0) & ctype_mask;
        if (t == 0)
            return c;
    }
    c_case_lk_nls.hit();
    const Pctype pct = pctype();
    const u8 hi = static_cast<u8>(static_cast<u32>(c >> 8));
    u8 in[4];  // the argument slot [ebp+8] itself
    std::memcpy(in, &c, 4);
    int len;
    if (pctype_byte(pct, hi, 1) & 0x80) {
        in[2] = 0;
        in[0] = hi;
        in[1] = static_cast<u8>(c);
        len = 2;
    } else {
        in[1] = 0;
        in[0] = static_cast<u8>(c);
        len = 1;
    }
    u8 out[4] = {0, 0, 0, 0};  // [ebp-4], cchDest 3
    // __crtLCMapStringA(__lc_handle[LC_CTYPE], flags, in, len, out, 3, code_page = 0, bError = TRUE)
    const int r = cdecl_call<int>(kCrtLCMapStringA, g<u32>(kLcCtype), lcmap_flags, in, len, out, 3, 0, 1);
    if (r == 0)
        return c;
    if (r == 1)
        return out[0];
    return (static_cast<int>(out[1]) << 8) | out[0];
}

// 0x007b45a3 _toupper_lk(c)
static int JOAN_CDECL crt_toupper_lk(int c) {
    if (g<u32>(kLcCtype) == 0) {
        if (c >= 0x61 && c <= 0x7a)
            return c - 0x20;
        return c;
    }
    return case_lk(c, 0x2 /* _LOWER */, kLcmapUppercase);
}

// 0x007b4d52 _tolower_lk(c)
static int JOAN_CDECL crt_tolower_lk(int c) {
    if (g<u32>(kLcCtype) == 0) {
        if (c >= 0x41 && c <= 0x5a)
            return c + 0x20;
        return c;
    }
    return case_lk(c, 0x1 /* _UPPER */, kLcmapLowercase);
}

// 0x007b4534 toupper(c)
static int JOAN_CDECL crt_toupper(int c) {
    if (g<u32>(kLcCtype) == 0) {
        if (c >= 0x61 && c <= 0x7a)
            return c - 0x20;
        return c;
    }
    const bool locked = guard_enter(false);
    const int r = cdecl_call<int>(kToupperLk, c);
    guard_leave(locked);
    return r;
}

// 0x007b4ce3 tolower(c)
static int JOAN_CDECL crt_tolower(int c) {
    if (g<u32>(kLcCtype) == 0) {
        if (c >= 0x41 && c <= 0x5a)
            return c + 0x20;
        return c;
    }
    const bool locked = guard_enter(false);
    const int r = cdecl_call<int>(kTolowerLk, c);
    guard_leave(locked);
    return r;
}

// ===================================================================== _stricmp / _strnicmp (stricmp.asm / strnicmp.asm)
inline u8 ascii_fold(u8 x) {  // sub al,'A'; cmp al,26; sbb cl,cl; and cl,0x20; add al,cl; add al,'A'
    return static_cast<u8>(x - 0x41u) < 26u ? static_cast<u8>(x + 0x20u) : x;
}
inline u32 set_low_byte(u32 reg, u8 b) {
    return (reg & 0xffffff00u) | b;
}

// 0x007c07b0 _stricmp(dst, src): edi = dst, esi = src.
static int JOAN_CDECL crt_stricmp(const char* dst, const char* src) {
    const u8* s1 = reinterpret_cast<const u8*>(dst);
    const u8* s2 = reinterpret_cast<const u8*>(src);
    if (g<u32>(kLcCtype) == 0) {
        for (;;) {
            const u8 b = *s2++;  // al
            const u8 a = *s1++;  // ah
            if (a == b) {
                if (b == 0)
                    return 0;
                continue;
            }
            const u8 fb = ascii_fold(b);
            const u8 fa = ascii_fold(a);
            if (fa == fb)
                continue;  // al = fa != 0
            return fa < fb ? -1 : 1;
        }
    }
    c_stricmp_nls.hit();
    const bool locked = guard_enter(true);
    u32 eax = 0xff;
    u32 ebx = 0;
    for (;;) {
        if (static_cast<u8>(eax) == 0)  // or al,al; je
            break;
        eax = set_low_byte(eax, *s2++);
        ebx = set_low_byte(ebx, *s1++);
        if (static_cast<u8>(eax) == static_cast<u8>(ebx))
            continue;
        ebx = static_cast<u32>(cdecl_call<int>(kTolowerLk, static_cast<int>(ebx)));
        eax = static_cast<u32>(cdecl_call<int>(kTolowerLk, static_cast<int>(eax)));
        if (static_cast<u8>(ebx) == static_cast<u8>(eax))
            continue;
        eax = static_cast<u8>(ebx) < static_cast<u8>(eax) ? 0xffffffffu : 1u;  // sbb eax,eax; sbb eax,-1
        break;
    }
    guard_leave(locked);
    return static_cast<int>(eax);
}

// 0x007c0960 _strnicmp(first, last, count): esi = first, edi = last.
static int JOAN_CDECL crt_strnicmp(const char* first, const char* last, std::size_t count) {  // only the low 32 bits are used (u32 ecx below)
    u32 ecx = static_cast<u32>(count);
    if (ecx == 0)
        return 0;
    const u8* s1 = reinterpret_cast<const u8*>(first);
    const u8* s2 = reinterpret_cast<const u8*>(last);
    if (g<u32>(kLcCtype) == 0) {
        u8 a, b;
        for (;;) {
            a = *s1;  // ah
            b = *s2;  // al
            if (a == 0 || b == 0)
                break;
            ++s1;
            ++s2;
            if (a >= 0x41 && a <= 0x5a)
                a = static_cast<u8>(a + 0x20);
            if (b >= 0x41 && b <= 0x5a)
                b = static_cast<u8>(b + 0x20);
            if (a != b)
                return a < b ? -1 : 1;
            if (--ecx == 0)
                break;
        }
        if (a == b)
            return 0;
        return a < b ? -1 : 1;
    }
    c_stricmp_nls.hit();
    const bool locked = guard_enter(true);
    u32 eax = 0;
    u32 ebx = 0;
    int result;
    for (;;) {
        eax = set_low_byte(eax, *s1);
        const bool z = eax == 0;  // or eax,eax (full register)
        ebx = set_low_byte(ebx, *s2);
        if (z || ebx == 0)
            break;
        ++s1;
        ++s2;
        ebx = static_cast<u32>(cdecl_call<int>(kTolowerLk, static_cast<int>(ebx)));
        eax = static_cast<u32>(cdecl_call<int>(kTolowerLk, static_cast<int>(eax)));
        if (eax != ebx)
            break;
        if (--ecx == 0)
            break;
    }
    if (eax == ebx)
        result = 0;
    else
        result = eax < ebx ? -1 : 1;
    guard_leave(locked);
    return result;
}

// ===================================================================== wide: _towlower_lk / iswctype / _wcs(n)icmp
// 0x007c5a98 iswctype(wint_t c, wctype_t type)
static int JOAN_CDECL crt_iswctype(u32 c, u32 type) {
    const u16 w = static_cast<u16>(c);
    if (w == 0xffff)
        return 0;
    u16 t;
    if (w < 0x100) {
        t = g<const volatile u16*>(kPwctype)[w];
    } else {
        u16 in = w;    // [ebp+8]
        u32 out = 0;   // [ebp-4]
        // __crtGetStringTypeW(CT_CTYPE1, &c, 1, &out, code_page = 0, lcid = 0)
        if (cdecl_call<int>(kCrtGetStringTypeW, 1, &in, 1, &out, 0, 0) == 0)
            return 0;
        t = static_cast<u16>(out);
    }
    return static_cast<int>(static_cast<u32>(t) & static_cast<u32>(static_cast<u16>(type)));
}

// 0x007c498e _towlower_lk(wint_t c): only the low word of the argument is used; the upper half of the
// returned eax is reproduced as well (callers only use ax).
static u32 JOAN_CDECL crt_towlower_lk(u32 c) {
    const u16 w = static_cast<u16>(c);
    if (w == 0xffff)
        return c;
    if (g<u32>(kLcCtype) == 0) {
        if (w >= 0x41 && w <= 0x5a)
            return c + 0x20u;
        return c;
    }
    if (w < 0x100) {
        if (cdecl_call<int>(kIswctype, c, 1 /* _UPPER */) == 0)
            return w;  // eax = 0 from iswctype, then mov ax,[ebp+8]
    }
    u16 in = w;   // [ebp+8]
    u16 out = 0;  // [ebp-2]
    // __crtLCMapStringW(__lc_handle[LC_CTYPE], LCMAP_LOWERCASE, &c, 1, &out, 1, code_page = 0)
    const u32 r = static_cast<u32>(
        cdecl_call<int>(kCrtLCMapStringW, g<u32>(kLcCtype), kLcmapLowercase, &in, 1, &out, 1, 0));
    if (r == 0)
        return w;  // eax = 0, mov ax,[ebp+8]
    return (r & 0xffff0000u) | out;
}

inline u16 wide_fold(u16 x) {
    return (x >= 0x41 && x <= 0x5a) ? static_cast<u16>(x + 0x20) : x;
}
inline u16 towlower_lk_call(u16 x) {
    return static_cast<u16>(cdecl_call<u32>(kTowlowerLk, static_cast<u32>(x)));
}

// 0x007c0880 _wcsicmp(dst, src)
static int JOAN_CDECL crt_wcsicmp(const WCHAR* dst, const WCHAR* src) {
    const u16* p = reinterpret_cast<const u16*>(dst);
    const u16* q = reinterpret_cast<const u16*>(src);
    u16 f1, f2;
    if (g<u32>(kLcCtype) == 0) {
        do {
            f1 = wide_fold(*p++);
            f2 = wide_fold(*q++);
        } while (f1 != 0 && f1 == f2);
        return static_cast<int>(f1) - static_cast<int>(f2);
    }
    c_wcsicmp.hit();
    const bool locked = guard_enter(false);
    do {
        f1 = towlower_lk_call(*p++);
        f2 = towlower_lk_call(*q++);
    } while (f1 != 0 && f1 == f2);
    guard_leave(locked);
    return static_cast<int>(f1) - static_cast<int>(f2);
}

// 0x007c0a61 _wcsnicmp(first, last, count)
static int JOAN_CDECL crt_wcsnicmp(const WCHAR* first, const WCHAR* last, std::size_t count) {  // only the low 32 bits are used (u32 n below)
    u32 n = static_cast<u32>(count);
    if (n == 0)
        return 0;
    const u16* p = reinterpret_cast<const u16*>(first);
    const u16* q = reinterpret_cast<const u16*>(last);
    u16 f1, f2;
    if (g<u32>(kLcCtype) == 0) {
        for (;;) {
            f1 = wide_fold(*p++);
            f2 = wide_fold(*q++);
            if (--n == 0 || f1 == 0 || f1 != f2)
                break;
        }
        return static_cast<int>(f1) - static_cast<int>(f2);
    }
    c_wcsicmp.hit();
    const bool locked = guard_enter(false);
    for (;;) {
        f1 = towlower_lk_call(*p++);
        f2 = towlower_lk_call(*q++);
        if (--n == 0 || f1 == 0 || f1 != f2)
            break;
    }
    guard_leave(locked);
    return static_cast<int>(f1) - static_cast<int>(f2);
}

// ===================================================================== _strupr / _strlwr / _wcsupr / _wcslwr
// Shape of all four (VC6 strupr.c / strlwr.c / wcsupr.c / wcslwr.c):
//   if (handle == 0) { C loop; return s; }
//   guard; if (handle == 0) { unguard; C loop; return s; }    (handle re-read under the guard)
//   n = LCMap(handle, flags, s, -1, NULL, 0); if (n && (dst = malloc(n [*2])) && LCMap(..., dst, n)) strcpy(s, dst);
//   unguard; free(dst); return s;
void str_case_c(char* s, bool upper) {
    for (u8* p = reinterpret_cast<u8*>(s); *p != 0; ++p) {
        const i32 ch = static_cast<i32>(static_cast<signed char>(*p));  // cmp cl,imm; jl/jg (signed)
        if (upper) {
            if (ch >= 0x61 && ch <= 0x7a)
                *p = static_cast<u8>(ch - 0x20);
        } else {
            if (ch >= 0x41 && ch <= 0x5a)
                *p = static_cast<u8>(ch + 0x20);
        }
    }
}

char* str_case(char* s, bool upper) {
    if (g<u32>(kLcCtype) == 0) {
        str_case_c(s, upper);
        return s;
    }
    c_strcase.hit();
    void* dst = nullptr;
    const bool locked = guard_enter(false);
    const u32 handle = g<u32>(kLcCtype);
    if (handle == 0) {
        guard_leave(locked);
        str_case_c(s, upper);
        return s;
    }
    const u32 flags = upper ? kLcmapUppercase : kLcmapLowercase;
    const int n = cdecl_call<int>(kCrtLCMapStringA, handle, flags, s, -1, static_cast<char*>(nullptr), 0, 0, 1);
    if (n != 0) {
        dst = cdecl_call<void*>(kMalloc, static_cast<u32>(n));
        if (dst != nullptr && cdecl_call<int>(kCrtLCMapStringA, g<u32>(kLcCtype), flags, s, -1, dst, n, 0, 1) != 0)
            cdecl_call<char*>(kStrcpy, s, static_cast<const char*>(dst));
    }
    guard_leave(locked);
    cdecl_call(kFree, dst);
    return s;
}

void wcs_case_c(WCHAR* s, bool upper) {
    for (u16* p = reinterpret_cast<u16*>(s); *p != 0; ++p) {
        const u16 ch = *p;  // unsigned compares (jb/ja)
        if (upper) {
            if (ch >= 0x61 && ch <= 0x7a)
                *p = static_cast<u16>(ch - 0x20);
        } else {
            if (ch >= 0x41 && ch <= 0x5a)
                *p = static_cast<u16>(ch + 0x20);
        }
    }
}

WCHAR* wcs_case(WCHAR* s, bool upper) {
    if (g<u32>(kLcCtype) == 0) {
        wcs_case_c(s, upper);
        return s;
    }
    c_strcase.hit();
    void* dst = nullptr;
    const bool locked = guard_enter(false);
    const u32 handle = g<u32>(kLcCtype);
    if (handle == 0) {
        guard_leave(locked);
        wcs_case_c(s, upper);
        return s;
    }
    const u32 flags = upper ? kLcmapUppercase : kLcmapLowercase;
    // __crtLCMapStringW(lcid, flags, src, -1, dst, cchDest, code_page)
    const int n = cdecl_call<int>(kCrtLCMapStringW, handle, flags, s, -1, static_cast<WCHAR*>(nullptr), 0, 0);
    if (n != 0) {
        dst = cdecl_call<void*>(kMalloc, static_cast<u32>(n) * 2u);  // lea eax,[edi+edi]
        if (dst != nullptr && cdecl_call<int>(kCrtLCMapStringW, g<u32>(kLcCtype), flags, s, -1, dst, n, 0) != 0)
            cdecl_call<WCHAR*>(kWcscpy, s, static_cast<const WCHAR*>(dst));
    }
    guard_leave(locked);
    cdecl_call(kFree, dst);
    return s;
}

static char* JOAN_CDECL crt_strupr(char* s) { return str_case(s, true); }
static char* JOAN_CDECL crt_strlwr(char* s) { return str_case(s, false); }
static WCHAR* JOAN_CDECL crt_wcsupr(WCHAR* s) { return wcs_case(s, true); }
static WCHAR* JOAN_CDECL crt_wcslwr(WCHAR* s) { return wcs_case(s, false); }

#define CTYPE(name, addr, fn) JOAN_REPLACE("crt/ctype", name, addr, fn)

CTYPE("crt::_strupr", 0x007b1800, &crt_strupr);
CTYPE("crt::isalpha", 0x007b3b0a, &crt_isalpha);
CTYPE("crt::isdigit", 0x007b3b88, &crt_isdigit);
CTYPE("crt::isspace", 0x007b3bdd, &crt_isspace);
CTYPE("crt::isalnum", 0x007b3c2d, &crt_isalnum);
CTYPE("crt::toupper", 0x007b4534, &crt_toupper);
CTYPE("crt::_toupper_lk", 0x007b45a3, &crt_toupper_lk);
CTYPE("crt::tolower", 0x007b4ce3, &crt_tolower);
CTYPE("crt::_tolower_lk", 0x007b4d52, &crt_tolower_lk);
CTYPE("crt::_isctype", 0x007b7f51, &crt_isctype);
CTYPE("crt::_stricmp", 0x007c07b0, &crt_stricmp);
CTYPE("crt::_wcsicmp", 0x007c0880, &crt_wcsicmp);
CTYPE("crt::_strnicmp", 0x007c0960, &crt_strnicmp);
CTYPE("crt::_wcsnicmp", 0x007c0a61, &crt_wcsnicmp);
CTYPE("crt::_towlower_lk", 0x007c498e, &crt_towlower_lk);
CTYPE("crt::iswctype", 0x007c5a98, &crt_iswctype);
CTYPE("crt::_strlwr", 0x007c5dc0, &crt_strlwr);
CTYPE("crt::_wcslwr", 0x007c5ef4, &crt_wcslwr);
CTYPE("crt::_wcsupr", 0x007c6031, &crt_wcsupr);

}  // namespace

// Direct entry points (crt.hpp).
namespace joan::crt {
int JOAN_CDECL stricmp(const char* a, const char* b) { return crt_stricmp(a, b); }
}  // namespace joan::crt

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::_strupr(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_strupr>(a0)); }  // 0x7b1800
auto joan::orig::libc::isalpha(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_isalpha>(a0)); }  // 0x7b3b0a
auto joan::orig::libc::isdigit(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_isdigit>(a0)); }  // 0x7b3b88
auto joan::orig::libc::isspace(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_isspace>(a0)); }  // 0x7b3bdd
auto joan::orig::libc::isalnum(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_isalnum>(a0)); }  // 0x7b3c2d
auto joan::orig::libc::toupper(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_toupper>(a0)); }  // 0x7b4534
auto joan::orig::libc::tolower(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_tolower>(a0)); }  // 0x7b4ce3
auto joan::orig::libc::_isctype(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_isctype>(a0, a1)); }  // 0x7b7f51
auto joan::orig::libc::_wcsicmp(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_wcsicmp>(a0, a1)); }  // 0x7c0880
auto joan::orig::libc::_strlwr(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_strlwr>(a0)); }  // 0x7c5dc0
auto joan::orig::libc::_wcslwr(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_wcslwr>(a0)); }  // 0x7c5ef4
auto joan::orig::libc::_wcsupr(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_wcsupr>(a0)); }  // 0x7c6031
