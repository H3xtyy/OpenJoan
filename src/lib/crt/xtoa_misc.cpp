// Original library sources (re/attribution.csv original_file, under lib/): crt/intel/lldiv.asm, crt/throw.cpp,
//   crt/tran/i386/87clog.asm, crt/xtoa.c.
//
// Batch 113 / list 12: statically linked MSVC 6 C runtime, 0x007b68d0..0x007c5ef4 (string / locale,
// stdio, heap, time, math, EH helpers). Full table of the roots and the reachable context functions,
// with the stateful families and their globals: scratchpad w113_12/ids.md.
//
// Roots (identification, class):
//   0x007b68d0 _wcsnicoll(const wchar_t*, const wchar_t*, size_t)   cdecl  STATEFUL locale (LC_COLLATE)
//   0x007b69b1 _mbslwr(unsigned char*)                               cdecl  STATEFUL locale (mbcs)
//   0x007b6a3b _mbsupr(unsigned char*)                               cdecl  STATEFUL locale (mbcs)
//   0x007b6ac5 clock(void)                                           cdecl  STATEFUL time (start 0xa6b238)
//   0x007b6b3d fread(void*, size_t, size_t, FILE*)                   cdecl  STATEFUL stdio
//   0x007b6c54 malloc(size_t)                                        cdecl  STATEFUL heap
//   0x007b6ce0 ftell(FILE*)                                          cdecl  STATEFUL stdio
//   0x007b6e63 fseek(FILE*, long, int)                               cdecl  STATEFUL stdio
//   0x007b6f4d getc(FILE*) (calls fgetc 0x7b6f1c)                    cdecl  STATEFUL stdio
//   0x007b6f60 _alldiv(__int64, __int64) -> edx:eax                  ret 0x10  IMPLEMENTED (stdcall)
//   0x007b7024 log(double) -> st0                                    cdecl  IMPLEMENTED
//   0x007b70e0 exp(double) -> st0 (mov edx,0xa3119a; jmp _trandisp1) cdecl  SKIPPED (x87 engine)
//   0x007b71ac _CxxThrowException: deleted in P2 (the game throws with C++ throw, original/cxx_eh.hpp)
//   0x007c07b0 _stricmp(const char*, const char*)                    cdecl  STATEFUL locale (LC_CTYPE)
//   0x007c0960 _strnicmp(const char*, const char*, size_t)           cdecl  STATEFUL locale (LC_CTYPE)
//   0x007c4730 _itoa(int, char*, int)                                cdecl  IMPLEMENTED
//   0x007c47b9 _ltoa(long, char*, int)                               cdecl  IMPLEMENTED
//   0x007c47e3 _ultoa(unsigned long, char*, int)                     cdecl  IMPLEMENTED
//   0x007c5afa _strdup(const char*)                                  cdecl  STATEFUL heap
//   0x007c5dc0 _strlwr(char*)                                        cdecl  STATEFUL locale (LC_CTYPE) + heap
//
// The locale-dependent string functions are left original: the game calls setlocale(LC_ALL, name)
// (CodePage::setup 0x0058b2d0 -> 0x007b5269) and _setmbcp, so __lc_handle[] (0xa6b2b0) and the
// mbcs tables can be non-C at run time; their C-locale fast paths are pure, the rest goes through
// LCMapStringA / CompareStringW with the CRT's locale state.
//
// Install note: patch_entry accepts any entry in 0x765000..0x7d5000 that passes
//   re/check_lib_entries.py (>= 5 bytes, no outside jump into entry+1..4); all six registrations
//   below pass it and install.
//
// Behaviour notes:
//  - _itoa/_ltoa/_ultoa: radix 0 divides by zero (integer #DE) exactly like the original `div`.
//    Radix > 36 produces the same bytes as the original (digit + 0x57, truncated to 8 bits).
//  - log: portable C++ (fp/pc53.hpp for fyl2x at 53-bit precision); see crt_log for the error paths.
//
// Modules: crt/b113_12.
#include "../../callconv.hpp"
#include <cstdint>
#include <cstring>

#include "../../fp/pc53.hpp"
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {

using u32 = std::uint32_t;

// ------------------------------------------------------------------------ 0x007c475d xtoa
// static void xtoa(u32 val, char* buf, unsigned radix, int is_neg)
void xtoa(u32 val, char* buf, unsigned radix, int is_neg) {
    char* p = buf;
    if (is_neg != 0) {
        *p++ = '-';
        val = 0u - val;
    }
    char* first = p;
    do {
        const u32 digval = val % radix;
        val /= radix;
        if (digval > 9)
            *p++ = static_cast<char>(digval + 0x57);
        else
            *p++ = static_cast<char>(digval + 0x30);
    } while (val > 0);
    *p-- = '\0';
    do {  // reverse the digits (the original swaps at least once, even for one digit)
        const char temp = *p;
        *p = *first;
        *first = temp;
        --p;
        ++first;
    } while (first < p);
}

// 0x007c4730 _itoa
static char* JOAN_CDECL crt_itoa(int val, char* buf, int radix) {
    if (radix == 10 && val < 0)
        xtoa(static_cast<u32>(val), buf, 10, 1);
    else
        xtoa(static_cast<u32>(val), buf, static_cast<unsigned>(radix), 0);
    return buf;
}

// 0x007c47b9 _ltoa
static char* JOAN_CDECL crt_ltoa(std::int32_t val, char* buf, int radix) {
    xtoa(static_cast<u32>(val), buf, static_cast<unsigned>(radix), (radix == 10 && val < 0) ? 1 : 0);
    return buf;
}

// 0x007c47e3 _ultoa
static char* JOAN_CDECL crt_ultoa(u32 val, char* buf, int radix) {
    xtoa(val, buf, static_cast<unsigned>(radix), 0);
    return buf;
}

// ------------------------------------------------------------------------ 0x007b6f60
// _alldiv: signed 64-bit divide, (dividend, divisor) on the stack, quotient in edx:eax, ret 16 (= __stdcall with
// two 64-bit arguments on x86). VC6 lldiv.asm divides the magnitudes (exact unsigned quotient) and negates when
// exactly one operand is negative, so INT64_MIN / -1 = INT64_MIN (no trap); a zero divisor is the division by
// zero the original's `div` faults on.
std::int64_t JOAN_STDCALL crt_alldiv(std::int64_t dividend, std::int64_t divisor) {
    const std::uint64_t ua = static_cast<std::uint64_t>(dividend), ub = static_cast<std::uint64_t>(divisor);
    const std::uint64_t a = dividend < 0 ? 0u - ua : ua;
    const std::uint64_t b = divisor < 0 ? 0u - ub : ub;
    const std::uint64_t q = a / b;
    return static_cast<std::int64_t>((dividend < 0) != (divisor < 0) ? 0u - q : q);
}

// ------------------------------------------------------------------------ 0x007b7024 log
// fldln2 / fyl2x at 53-bit precision (fp/pc53.hpp); with the game's control word the result always leaves
// through _math_exit -> _87except(_INEXACT) (rounded through a double, nothing written). Error paths as measured
// on the original (re/p1/crt), errno through _errno (crt/mt): +-0 -> -inf, ERANGE (34); x < 0 or -inf -> default
// NaN 0xfff8000000000000, EDOM (33); NaN -> the quieted NaN, EDOM; +inf -> +inf. A negative denormal goes through
// fyl2x (exponent field 0 is taken as "finite") and gives the default NaN without errno.
void set_errno(int v) { *static_cast<int*>(joan::orig::libc::_errno()) = v; }

double JOAN_CDECL crt_log(double x) {
    std::uint64_t b;
    std::memcpy(&b, &x, 8);
    const std::uint64_t mag = b & 0x7fffffffffffffffull;
    if (mag > 0x7ff0000000000000ull) {  // NaN
        set_errno(33);
        b |= 0x0008000000000000ull;
    } else if (mag == 0) {
        set_errno(34);
        b = 0xfff0000000000000ull;
    } else if (b >> 63) {
        if (mag >= 0x0010000000000000ull) set_errno(33);
        b = 0xfff8000000000000ull;
    } else if (mag == 0x7ff0000000000000ull) {
        return x;
    } else {
        return joan::fp::log_pc53(x);
    }
    double r;
    std::memcpy(&r, &b, 8);
    return r;
}

JOAN_REPLACE("crt/b113_12", "crt::_alldiv", 0x007b6f60, &crt_alldiv);
JOAN_REPLACE("crt/b113_12", "crt::log", 0x007b7024, &crt_log);
JOAN_REPLACE("crt/b113_12", "crt::_itoa", 0x007c4730, &crt_itoa);
JOAN_REPLACE("crt/b113_12", "crt::_ltoa", 0x007c47b9, &crt_ltoa);
JOAN_REPLACE("crt/b113_12", "crt::_ultoa", 0x007c47e3, &crt_ultoa);

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::_itoa(std::int32_t a0, const char* a1, std::int32_t a2) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_itoa>(a0, a1, a2)); }  // 0x7c4730
auto joan::orig::libc::_ltoa(std::int32_t a0, const char* a1, std::int32_t a2) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_ltoa>(a0, a1, a2)); }  // 0x7c47b9
auto joan::orig::libc::_ultoa(std::uint32_t a0, const char* a1, std::int32_t a2) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_ultoa>(a0, a1, a2)); }  // 0x7c47e3
