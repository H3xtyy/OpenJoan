// Original library sources (re/attribution.csv original_file, under lib/): crt/atox.c, crt/cvt.c, crt/fltin.c,
//   crt/input.c, crt/intrncvt.c, crt/mantold.c, crt/strgtold.c.
//
// Batch 114 / list 09: family crt/scanf - the input side of the VC6 C runtime's formatting
// (input.c, strgtold.c, mantold.c, tenpow.c, cvt.c, fltintrn wrappers) plus atol.
//
// Surface replaced (all cdecl, plain `ret`; no thunks: generic library-range entry patch):
//   0x007bc273 int _input(FILE* stream, const unsigned char* format, va_list arglist)
//              scanf engine; only caller sscanf 0x007b3ec2 (C++ in lib/crt/string_format.cpp, calls this address)
//   0x007bcc98 int _hextodec(int ch)            {
//   0x007bcccf int _inc(FILE*)                   |  input.c helpers, only called by _input
//   0x007bcce9 void _un_inc(int ch, FILE*)       |  (registered anyway so no original piece of
//   0x007bcd00 int _whiteout(int* counter, FILE*)}   the family stays reachable)
//   0x007b8f3c char* _fassign(int flag, char* argument, char* number)
//              _cfltcvt_tab[2] (0x00a30fb0): the float store of %e/%f/%g; flag != 0 -> double
//   0x007b9c9d FLT* _fltin(FLT* flt, const char* str, ...)   (VC6 _fltin2; atof 0x007b2110 calls it)
//   0x007c25af int _atodbl(DOUBLE*, char*)       (from _fassign)
//   0x007c25dc int _atoldbl(LDOUBLE*, char*)     (no caller in the image)
//   0x007c260a int _atoflt(FLOAT*, char*)        (from _fassign)
//   0x007c27cc unsigned __strgtold12(_LDOUBLE* pld12, const char** end, const char* str,
//                                    int mult12, int scale, int decpt, int implicit_E)
//   0x007c2c9d unsigned $I10_INPUT / __STRINGTOLD(LDOUBLE*, const char** end, const char* str,
//                                    int mult12)  (no caller in the image)
//   0x007c2509 int _ld12tod(_LDOUBLE*, DOUBLE*)  {
//   0x007c251f int _ld12tof(_LDOUBLE*, FLOAT*)    | _ld12cvt 0x007c239d with the VC6 format
//   0x007c2535 int _ld12told(_LDOUBLE*, LDOUBLE*) }  descriptors (0x00a320a8 / 0x00a320c0)
//   0x007b1540 long atol(const char*)            (callers: atoi 0x007b15cb = C++ lib/crt/string.cpp,
//                                                 CRT _tzset 0x007bd4b5, ProcessCodePage 0x007bff64)
// strtol / strtoul / strtod: NOT linked into joan.exe (0x007b3b0a / 0x007b3b88 / 0x007b3bdd /
// 0x007b3c2d are isalpha / isdigit / isspace / isalnum, locale family). Nothing to replace.
//
// Internal pieces implemented privately (no registration; once the surface above is ours they
// are only reached from the ORIGINAL printf side, which keeps using its own copies):
//   __addl 0x7c4de0, __add_12 0x7c4e01, __shl_12 0x7c4e5f, __shr_12 0x7c4e8d, __mtold12 0x7c4eba,
//   __ld12mul 0x7c5214, __multtenpow12 0x7c5434 (also used by $I10_OUTPUT 0x7c4f81), _ld12cvt
//   0x7c239d, _RoundMan 0x7c2242, _IsZeroTail 0x7c21a3, _IncMan 0x7c21ec, _ShrMan 0x7c2310,
//   _CopyMan/_ZeroMan/_IsZeroMan 0x7c22ce/0x7c22e9/0x7c22f5. The power-of-ten tables
//   _pow10pos 0x00a32248 / _pow10neg 0x00a323a8 (29 x 12 bytes each, checked = 10^n) are copied
//   here as constants; the %c / %s scanset strings ("]" 0x00a31498, " \t-\r]" 0x00a31490) too.
//
// Exactness notes (all reproduced, not fixed):
//  * _RoundMan tests the tail from bit rndbit+2 (calls _IsZeroTail(man, rndbit + 1)), so a value
//    exactly half-way, or with only bit rndbit+1 set below the round bit, rounds DOWN; it keeps
//    the round bit itself in the mantissa (shifted out later). __strgtold12 truncates to 24
//    digits and bumps digit 24 without carry (digit value 10 is fine for __mtold12).
//  * __ld12mul: 16-bit carry increments that can wrap without propagating, half-even rounding of
//    the 96-bit product, sticky right shift for tiny results; the carry slot past the 12-byte
//    product (the loop counter in the original frame) is provably never written (the tracked
//    product stays < 2^96).
//  * _input: VC6 width bookkeeping (the '0' / "0x" prefix of %x/%i not counted, float width
//    clamped to 349 after the sign, the exponent '-' stored past the width), I64 / h / l / L / w
//    modifier byte counters (e.g. "%hhd" stores a dword), match/count rules for the return value,
//    DBCS literal pairs, mbtowc for %C/%S/%lc (called at its address with __mb_cur_max).
// Locale state is read at the original addresses on every use: __mb_cur_max 0x00a30ed0,
// _pctype 0x00a30cc4 (incl. the entry before the table for EOF), __decimal_point 0x00a30ed4;
// _isctype 0x007b7f51, isspace 0x007b3bdd, mbtowc 0x007c2f55 are called at their addresses
// (locale family). stdio: _filbuf 0x007c0b59 and _ungetc_lk 0x007c30c8 at their addresses.
// _input reaches the float store through the pointer in _cfltcvt_tab[2] (0x00a30fb0) like the
// original, so an uninitialised table still ends in _fptrap (R6002).
// x87: only _fltin touches it (fld qword / fstp qword of the result, kept as the same two
// instructions: the DE flag of a denormal result is raised exactly as in the original).
// Uninitialised value: `ch` of _input ([ebp-0x14]; the entry only zeroes [ebp-0x4] / [ebp-0x34]) is read before any
// write ONLY for (a) an empty format (0x7bc292 -> 0x7bcc79: EOF vs 0) and (b) a format whose first
// directive is %n (no input read: the 0x7bcc27 EOF test, and "%0n" ungets the garbage ch).
// A white-space-only format is NOT a case: the trailing NUL goes through the literal branch,
// which reads a character first (0x7bcbe5). The mbtowc result slot is stale only for a failing
// mbtowc under %lc/%C/%S/%ls/%l[/%w. Reviewer proof of unreachability (so plain crt/scanf, no
// fix/uninit): _input's only caller is sscanf 0x007b3ec2, whose every caller passes a constant
// format: original call sites 0x4caf9a.. (5x "%x" 0x9d5780), 0x559538.. (8x "(%f, %f, %f)"
// variants 0x9d83a0/0x9d83b4/0x9d83c4), 0x58b7c4 ("%x-%x" 0x9d8af0), 0x78b91f ("%f" 0x9e3bc0),
// 0x7aabf7 (libjpeg "%ld%c" 0x00a30920, never written), and the C++ callers game_b111_20 (%x),
// game_b111_18 / game_b83_02 (0x9d83xx), game_b103_08 (%x-%x). None is empty, starts with %n or
// uses a wide conversion, so ch is always written first; we start from 0 (deterministic).
// Verified against the original code (Linux harness running joan.exe's own code at its addresses,
// scratchpad w114_09/h): 5M numeric strings x (__strgtold12 with random mult12/scale/decpt/
// implicit_E, _fltin incl. x87 status word, _atodbl, _atoflt, _atoldbl, $I10_INPUT, _fassign,
// atol), 5M random _LDOUBLEs x (_ld12tod, _ld12tof, _ld12told), 3M denormal-band strings, 30M
// _input calls (game formats + random formats/inputs, C locale and random ctype tables /
// decimal points / __mb_cur_max 2 with stubbed _isctype and mbtowc): 0 mismatches. Mutations of
// the rounding, width, DBCS and sticky paths are all detected.
// VC6 undefined behaviour kept as is: a locale in which a format character other than '0'..'9'
// (or the decimal point / 'e') is _DIGIT can drive a width negative, and _input then overflows its
// 352-byte float buffer (the original too); no real VC6 ctype table does that.
#include "varargs.hpp"
#include <cstdarg>
#include "../../callconv.hpp"
#include <cstdint>
#include <cstring>

#include "../../original/direct.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"

namespace {
using i8 = std::int8_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

using joan::img::global;

using joan::call::cdecl_call;

JOAN_COUNTER(c_input, "crt::_input");
JOAN_COUNTER(c_strgtold12, "crt::__strgtold12");

// ------------------------------------------------------------------ byte access helpers
u16 rd16(const u8* p, int off) {
    u16 v;
    std::memcpy(&v, p + off, 2);
    return v;
}
u32 rd32(const u8* p, int off) {
    u32 v;
    std::memcpy(&v, p + off, 4);
    return v;
}
void wr16(u8* p, int off, u32 v) {
    const u16 w = static_cast<u16>(v);
    std::memcpy(p + off, &w, 2);
}
void wr32(u8* p, int off, u32 v) { std::memcpy(p + off, &v, 4); }

// ------------------------------------------------------------------ locale reads
i32 mb_cur_max() { return global<i32>(d::g_mb_cur_max); }
u8 decimal_point() { return global<u8>(d::g_decimal_point); }
// Low byte of _pctype[c] (c may be EOF: the entry before the table, as the original reads it).
u32 pctype_lo(i32 c) { return global<const u8*>(d::g_pctype)[c * 2]; }
u32 pctype_hi(u32 c) { return global<const u8*>(d::g_pctype)[c * 2 + 1]; }
// The ctype macros as compiled: __mb_cur_max > 1 ? _isctype(c, mask) : _pctype[c] & mask.
u32 ctype(i32 c, u32 mask) {
    if (mb_cur_max() > 1) return static_cast<u32>(joan::orig::arg::as<int>(joan::orig::libc::_isctype(joan::orig::arg::i32(c), joan::orig::arg::i32(static_cast<int>(mask)))));
    return pctype_lo(c) & mask;
}
bool is_digit(i32 c) { return ctype(c, 4) != 0; }
bool is_xdigit(i32 c) { return ctype(c, 0x80) != 0; }
bool isspace_fn(i32 c) { return joan::orig::arg::as<int>(joan::orig::libc::isspace(joan::orig::arg::i32(c))) != 0; }  // isspace() function

// ------------------------------------------------------------------ 12-byte mantissa arithmetic
// __addl 0x7c4de0
u32 addl(u32 x, u32 y, u32* sum) {
    const u32 r = x + y;
    *sum = r;
    return (r < x || r < y) ? 1u : 0u;
}
u32 addl_at(u8* p, int off, u32 y) {
    u32 r;
    const u32 c = addl(rd32(p, off), y, &r);
    wr32(p, off, r);
    return c;
}
// __add_12 0x7c4e01
void add_12(u8* x, const u8* y) {
    if (addl_at(x, 0, rd32(y, 0)))
        if (addl_at(x, 4, 1)) wr32(x, 8, rd32(x, 8) + 1);
    if (addl_at(x, 4, rd32(y, 4))) wr32(x, 8, rd32(x, 8) + 1);
    addl_at(x, 8, rd32(y, 8));
}
// __shl_12 0x7c4e5f
void shl_12(u8* p) {
    const u32 a = rd32(p, 0), b = rd32(p, 4), c = rd32(p, 8);
    wr32(p, 0, a << 1);
    wr32(p, 4, (b << 1) | (a >> 31));
    wr32(p, 8, (c << 1) | (b >> 31));
}
// __shr_12 0x7c4e8d
void shr_12(u8* p) {
    const u32 a = rd32(p, 0), b = rd32(p, 4), c = rd32(p, 8);
    wr32(p, 4, (b >> 1) | (c << 31));
    wr32(p, 8, c >> 1);
    wr32(p, 0, (a >> 1) | (b << 31));
}

// __mtold12 0x7c4eba: decimal digits (bytes, sign-extended as in the original) -> _LDOUBLE.
void mtold12(const i8* manptr, u32 manlen, u8* ld) {
    u32 expn = 0x404e;
    wr32(ld, 0, 0);
    wr32(ld, 4, 0);
    wr32(ld, 8, 0);
    if (manlen > 0) {
        for (u32 n = manlen; n != 0; --n) {
            u8 tmp[12];
            std::memcpy(tmp, ld, 12);
            shl_12(ld);
            shl_12(ld);
            add_12(ld, tmp);
            shl_12(ld);
            wr32(tmp, 0, static_cast<u32>(static_cast<i32>(*manptr)));
            wr32(tmp, 4, 0);
            wr32(tmp, 8, 0);
            add_12(ld, tmp);
            ++manptr;
        }
    }
    while (rd32(ld, 8) == 0) {
        const u32 hi = rd32(ld, 4), lo = rd32(ld, 0);
        wr32(ld, 8, hi >> 16);
        wr32(ld, 4, (lo >> 16) | (hi << 16));
        wr32(ld, 0, lo << 16);
        expn += 0xfff0;
    }
    while ((rd32(ld, 8) & 0x8000) == 0) {
        shl_12(ld);
        expn += 0xffff;
    }
    wr16(ld, 10, expn);
}

// __ld12mul 0x7c5214: *px *= *py (_LDOUBLE).
void ld12mul(u8* px, const u8* py) {
    u32 sticky = 0;
    u8 tm[16] = {};  // 12-byte product; the 4 extra bytes take the (never happening) top carry
    u32 expx = rd16(px, 10);
    u32 expy = rd16(py, 10);
    const u32 sign = (expx ^ expy) & 0x8000;
    expx &= 0x7fff;
    expy &= 0x7fff;
    u16 expsum = static_cast<u16>(expx + expy);
    if (expx >= 0x7fff || expy >= 0x7fff || expsum > 0xbffd) goto inf;
    if (expsum <= 0x3fbf) {
        wr32(px, 8, 0);
        wr32(px, 4, 0);
        wr32(px, 0, 0);
        return;
    }
    if (expx == 0) {
        ++expsum;
        if ((rd32(px, 8) & 0x7fffffff) == 0 && rd32(px, 4) == 0 && rd32(px, 0) == 0) {
            wr16(px, 10, 0);
            return;
        }
    }
    if (expy == 0) {
        ++expsum;
        if ((rd32(py, 8) & 0x7fffffff) == 0 && rd32(py, 4) == 0 && rd32(py, 0) == 0) {
            wr32(px, 8, 0);
            wr32(px, 4, 0);
            wr32(px, 0, 0);
            return;
        }
    }
    for (int i = 0; i < 5; ++i) {
        const int roffs = 2 * i;
        int poffs = 2 * i;
        int qoffs = 8;
        for (int j = 5 - i; j > 0; --j) {
            const u32 prod = static_cast<u32>(rd16(px, poffs)) * static_cast<u32>(rd16(py, qoffs));
            if (addl_at(tm, roffs, prod)) wr16(tm, roffs + 4, rd16(tm, roffs + 4) + 1u);
            poffs += 2;
            qoffs -= 2;
        }
    }
    expsum = static_cast<u16>(expsum + 0xc002);
    if (static_cast<std::int16_t>(expsum) > 0) {
        while (static_cast<std::int16_t>(expsum) > 0) {
            if (tm[11] & 0x80) break;
            shl_12(tm);
            expsum = static_cast<u16>(expsum - 1);
        }
        if (static_cast<std::int16_t>(expsum) > 0) goto round;
    }
    expsum = static_cast<u16>(expsum - 1);
    if (static_cast<std::int16_t>(expsum) < 0) {
        u32 n = static_cast<u32>(-static_cast<i32>(static_cast<std::int16_t>(expsum)));
        expsum = 0;
        do {
            if (tm[0] & 1) ++sticky;
            shr_12(tm);
        } while (--n != 0);
        if (sticky) tm[0] |= 1;
    }
round:
    if (rd16(tm, 0) > 0x8000 || (rd32(tm, 0) & 0x1ffff) == 0x18000) {
        if (rd32(tm, 2) == 0xffffffffu) {
            wr32(tm, 2, 0);
            if (rd32(tm, 6) == 0xffffffffu) {
                wr32(tm, 6, 0);
                if (rd16(tm, 10) == 0xffff) {
                    ++expsum;
                    wr16(tm, 10, 0x8000);
                } else {
                    wr16(tm, 10, rd16(tm, 10) + 1u);
                }
            } else {
                wr32(tm, 6, rd32(tm, 6) + 1);
            }
        } else {
            wr32(tm, 2, rd32(tm, 2) + 1);
        }
    }
    if (expsum >= 0x7fff) goto inf;
    wr16(px, 0, rd16(tm, 2));
    wr32(px, 2, rd32(tm, 4));
    wr32(px, 6, rd32(tm, 8));
    wr16(px, 10, expsum | sign);
    return;
inf:
    wr32(px, 8, sign ? 0xffff8000u : 0x7fff8000u);
    wr32(px, 4, 0);
    wr32(px, 0, 0);
}

// _pow10pos / _pow10neg (tenpow.c): 10^1..10^7, 10^8..10^56, 10^64..10^448, 10^512..10^3584,
// 10^4096 and the reciprocals, 80-bit mantissa + 16-bit extension, as in the image.
const u8 kPow10Pos[29 * 12] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x05, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfa, 0x08, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9c, 0x0c, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xc3, 0x0f, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xf4, 0x12, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x96, 0x98, 0x16, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xbc, 0xbe, 0x19, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xbf, 0xc9, 0x1b, 0x8e, 0x34, 0x40,
    0x00, 0x00, 0x00, 0xa1, 0xed, 0xcc, 0xce, 0x1b, 0xc2, 0xd3, 0x4e, 0x40,
    0x20, 0xf0, 0x9e, 0xb5, 0x70, 0x2b, 0xa8, 0xad, 0xc5, 0x9d, 0x69, 0x40,
    0xd0, 0x5d, 0xfd, 0x25, 0xe5, 0x1a, 0x8e, 0x4f, 0x19, 0xeb, 0x83, 0x40,
    0x71, 0x96, 0xd7, 0x95, 0x43, 0x0e, 0x05, 0x8d, 0x29, 0xaf, 0x9e, 0x40,
    0xf9, 0xbf, 0xa0, 0x44, 0xed, 0x81, 0x12, 0x8f, 0x81, 0x82, 0xb9, 0x40,
    0xbf, 0x3c, 0xd5, 0xa6, 0xcf, 0xff, 0x49, 0x1f, 0x78, 0xc2, 0xd3, 0x40,
    0x6f, 0xc6, 0xe0, 0x8c, 0xe9, 0x80, 0xc9, 0x47, 0xba, 0x93, 0xa8, 0x41,
    0xbc, 0x85, 0x6b, 0x55, 0x27, 0x39, 0x8d, 0xf7, 0x70, 0xe0, 0x7c, 0x42,
    0xbc, 0xdd, 0x8e, 0xde, 0xf9, 0x9d, 0xfb, 0xeb, 0x7e, 0xaa, 0x51, 0x43,
    0xa1, 0xe6, 0x76, 0xe3, 0xcc, 0xf2, 0x29, 0x2f, 0x84, 0x81, 0x26, 0x44,
    0x28, 0x10, 0x17, 0xaa, 0xf8, 0xae, 0x10, 0xe3, 0xc5, 0xc4, 0xfa, 0x44,
    0xeb, 0xa7, 0xd4, 0xf3, 0xf7, 0xeb, 0xe1, 0x4a, 0x7a, 0x95, 0xcf, 0x45,
    0x65, 0xcc, 0xc7, 0x91, 0x0e, 0xa6, 0xae, 0xa0, 0x19, 0xe3, 0xa3, 0x46,
    0x0d, 0x65, 0x17, 0x0c, 0x75, 0x81, 0x86, 0x75, 0x76, 0xc9, 0x48, 0x4d,
    0x58, 0x42, 0xe4, 0xa7, 0x93, 0x39, 0x3b, 0x35, 0xb8, 0xb2, 0xed, 0x53,
    0x4d, 0xa7, 0xe5, 0x5d, 0x3d, 0xc5, 0x5d, 0x3b, 0x8b, 0x9e, 0x92, 0x5a,
    0xff, 0x5d, 0xa6, 0xf0, 0xa1, 0x20, 0xc0, 0x54, 0xa5, 0x8c, 0x37, 0x61,
    0xd1, 0xfd, 0x8b, 0x5a, 0x8b, 0xd8, 0x25, 0x5d, 0x89, 0xf9, 0xdb, 0x67,
    0xaa, 0x95, 0xf8, 0xf3, 0x27, 0xbf, 0xa2, 0xc8, 0x5d, 0xdd, 0x80, 0x6e,
    0x4c, 0xc9, 0x9b, 0x97, 0x20, 0x8a, 0x02, 0x52, 0x60, 0xc4, 0x25, 0x75,
};
const u8 kPow10Neg[29 * 12] = {
    0xcd, 0xcc, 0xcd, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xfb, 0x3f,
    0x71, 0x3d, 0x0a, 0xd7, 0xa3, 0x70, 0x3d, 0x0a, 0xd7, 0xa3, 0xf8, 0x3f,
    0x5a, 0x64, 0x3b, 0xdf, 0x4f, 0x8d, 0x97, 0x6e, 0x12, 0x83, 0xf5, 0x3f,
    0xc3, 0xd3, 0x2c, 0x65, 0x19, 0xe2, 0x58, 0x17, 0xb7, 0xd1, 0xf1, 0x3f,
    0xd0, 0x0f, 0x23, 0x84, 0x47, 0x1b, 0x47, 0xac, 0xc5, 0xa7, 0xee, 0x3f,
    0x40, 0xa6, 0xb6, 0x69, 0x6c, 0xaf, 0x05, 0xbd, 0x37, 0x86, 0xeb, 0x3f,
    0x33, 0x3d, 0xbc, 0x42, 0x7a, 0xe5, 0xd5, 0x94, 0xbf, 0xd6, 0xe7, 0x3f,
    0xc2, 0xfd, 0xfd, 0xce, 0x61, 0x84, 0x11, 0x77, 0xcc, 0xab, 0xe4, 0x3f,
    0x2f, 0x4c, 0x5b, 0xe1, 0x4d, 0xc4, 0xbe, 0x94, 0x95, 0xe6, 0xc9, 0x3f,
    0x92, 0xc4, 0x53, 0x3b, 0x75, 0x44, 0xcd, 0x14, 0xbe, 0x9a, 0xaf, 0x3f,
    0xde, 0x67, 0xba, 0x94, 0x39, 0x45, 0xad, 0x1e, 0xb1, 0xcf, 0x94, 0x3f,
    0x24, 0x23, 0xc6, 0xe2, 0xbc, 0xba, 0x3b, 0x31, 0x61, 0x8b, 0x7a, 0x3f,
    0x61, 0x55, 0x59, 0xc1, 0x7e, 0xb1, 0x53, 0x7c, 0x12, 0xbb, 0x5f, 0x3f,
    0xd7, 0xee, 0x2f, 0x8d, 0x06, 0xbe, 0x92, 0x85, 0x15, 0xfb, 0x44, 0x3f,
    0x24, 0x3f, 0xa5, 0xe9, 0x39, 0xa5, 0x27, 0xea, 0x7f, 0xa8, 0x2a, 0x3f,
    0x7d, 0xac, 0xa1, 0xe4, 0xbc, 0x64, 0x7c, 0x46, 0xd0, 0xdd, 0x55, 0x3e,
    0x63, 0x7b, 0x06, 0xcc, 0x23, 0x54, 0x77, 0x83, 0xff, 0x91, 0x81, 0x3d,
    0x91, 0xfa, 0x3a, 0x19, 0x7a, 0x63, 0x25, 0x43, 0x31, 0xc0, 0xac, 0x3c,
    0x21, 0x89, 0xd1, 0x38, 0x82, 0x47, 0x97, 0xb8, 0x00, 0xfd, 0xd7, 0x3b,
    0xdc, 0x88, 0x58, 0x08, 0x1b, 0xb1, 0xe8, 0xe3, 0x86, 0xa6, 0x03, 0x3b,
    0xc6, 0x84, 0x45, 0x42, 0x07, 0xb6, 0x99, 0x75, 0x37, 0xdb, 0x2e, 0x3a,
    0x33, 0x71, 0x1c, 0xd2, 0x23, 0xdb, 0x32, 0xee, 0x49, 0x90, 0x5a, 0x39,
    0xa6, 0x87, 0xbe, 0xc0, 0x57, 0xda, 0xa5, 0x82, 0xa6, 0xa2, 0xb5, 0x32,
    0xe2, 0x68, 0xb2, 0x11, 0xa7, 0x52, 0x9f, 0x44, 0x59, 0xb7, 0x10, 0x2c,
    0x25, 0x49, 0xe4, 0x2d, 0x36, 0x34, 0x4f, 0x53, 0xae, 0xce, 0x6b, 0x25,
    0x8f, 0x59, 0x04, 0xa4, 0xc0, 0xde, 0xc2, 0x7d, 0xfb, 0xe8, 0xc6, 0x1e,
    0x9e, 0xe7, 0x88, 0x5a, 0x57, 0x91, 0x3c, 0xbf, 0x50, 0x83, 0x22, 0x18,
    0x4e, 0x4b, 0x65, 0x62, 0xfd, 0x83, 0x8f, 0xaf, 0x06, 0x94, 0x7d, 0x11,
    0xe4, 0x2d, 0xde, 0x9f, 0xce, 0xd2, 0xc8, 0x04, 0xdd, 0xa6, 0xd8, 0x0a,
};

// __multtenpow12 0x7c5434: *pld *= 10^pow (|pow| <= 5200 here).
void multtenpow12(u8* pld, i32 pow, u32 mult12) {
    if (pow == 0) return;
    const u8* tab = kPow10Pos;
    if (pow < 0) {
        pow = -pow;
        tab = kPow10Neg;
    }
    if (mult12 == 0) wr16(pld, 0, 0);
    int level = 0;
    while (pow != 0) {
        const i32 last3 = pow & 7;
        pow >>= 3;
        if (last3 != 0) {
            const u8* e = tab + 12 * (7 * level + last3 - 1);
            u8 unround[12];
            if (rd16(e, 0) >= 0x8000) {
                std::memcpy(unround, e, 12);
                wr32(unround, 2, rd32(unround, 2) - 1);
                e = unround;
            }
            ld12mul(pld, e);
        }
        ++level;
    }
}

// ------------------------------------------------------------------ cvt.c (_ld12cvt and helpers)
// Mantissa: man[0] = most significant dword; bit numbers count from its MSB.
struct FpFormat {
    i32 max, min, precision, exp_width, format_width, bias;
};
const FpFormat kDoubleFormat = {0x400, -0x3ff, 0x35, 0xb, 0x40, 0x3ff};  // 0x00a320a8
const FpFormat kFloatFormat = {0x80, -0x7f, 0x18, 0x8, 0x20, 0x7f};      // 0x00a320c0

// _IsZeroTail 0x7c21a3: bits after `nbit` all zero?
int is_zero_tail(const u32* man, i32 nbit) {
    i32 idx = nbit / 32;
    const u32 mask = ~(~0u << (31 - nbit % 32));
    if (man[idx] & mask) return 0;
    for (++idx; idx < 3; ++idx)
        if (man[idx] != 0) return 0;
    return 1;
}
// _IncMan 0x7c21ec: add 1 at bit `nbit`, returns the carry out of man[0].
u32 inc_man(u32* man, i32 nbit) {
    i32 idx = nbit / 32;
    u32 carry = addl(man[idx], 1u << (31 - nbit % 32), &man[idx]);
    for (--idx; idx >= 0 && carry; --idx) carry = addl(man[idx], 1, &man[idx]);
    return carry;
}
// _RoundMan 0x7c2242
u32 round_man(u32* man, i32 precision) {
    const i32 nbit = precision - 1;
    const i32 rndbit = nbit + 1;
    const i32 nlong = rndbit / 32;
    const i32 bitpos = 31 - rndbit % 32;
    u32 retval = 0;
    if (man[nlong] & (1u << bitpos))
        if (!is_zero_tail(man, rndbit + 1)) retval = inc_man(man, nbit);
    man[nlong] &= ~0u << bitpos;
    for (i32 i = nlong + 1; i < 3; ++i) man[i] = 0;
    return retval;
}
// _ShrMan 0x7c2310
void shr_man(u32* man, i32 n) {
    const i32 ws = n / 32;
    const i32 bs = n % 32;
    const u32 mask = ~(~0u << bs);
    u32 prev = 0;
    for (int i = 0; i < 3; ++i) {
        const u32 low = man[i] & mask;
        man[i] = (man[i] >> bs) | prev;
        prev = bs == 0 ? low : low << (32 - bs);  // x86 shl by 32 = shl by 0
    }
    for (int i = 2; i >= 0; --i) man[i] = i >= ws ? man[i - ws] : 0;
}
bool is_zero_man(const u32* man) { return man[0] == 0 && man[1] == 0 && man[2] == 0; }
void zero_man(u32* man) { man[0] = man[1] = man[2] = 0; }

// _ld12cvt 0x7c239d: 0 OK, 1 overflow, 2 underflow (also every denormal).
int ld12cvt(const u8* ifp, u8* d, const FpFormat& f) {
    const u32 expw = rd16(ifp, 10);
    const u32 sign = expw & 0x8000;
    u32 man[3] = {rd32(ifp, 6), rd32(ifp, 2), static_cast<u32>(rd16(ifp, 0)) << 16};
    i32 e = static_cast<i32>(expw & 0x7fff) - 0x3fff;
    int retval;
    u32 bits;
    if (e == -0x3fff) {
        bits = 0;
        if (is_zero_man(man)) {
            retval = 0;
        } else {
            zero_man(man);
            retval = 2;
        }
    } else {
        u32 saved[3] = {man[0], man[1], man[2]};
        if (round_man(man, f.precision)) ++e;
        if (e < f.min - f.precision) {
            zero_man(man);
            bits = 0;
            retval = 2;
        } else if (e <= f.min) {
            man[0] = saved[0];
            man[1] = saved[1];
            man[2] = saved[2];
            shr_man(man, f.min - e);
            round_man(man, f.precision);
            shr_man(man, f.exp_width + 1);
            bits = 0;
            retval = 2;
        } else if (e >= f.max) {
            zero_man(man);
            man[0] |= 0x80000000u;
            shr_man(man, f.exp_width);
            bits = static_cast<u32>(f.bias + f.max);
            retval = 1;
        } else {
            man[0] &= 0x7fffffffu;
            bits = static_cast<u32>(f.bias + e);
            shr_man(man, f.exp_width);
            retval = 0;
        }
    }
    bits <<= 31 - f.exp_width;
    bits |= sign ? 0x80000000u : 0u;
    bits |= man[0];
    if (f.format_width == 64) {
        wr32(d, 4, bits);
        wr32(d, 0, man[1]);
    } else if (f.format_width == 32) {
        wr32(d, 0, bits);
    }
    return retval;
}

// ------------------------------------------------------------------ 0x007c27cc __strgtold12
bool nz_digit(u8 c) { return c >= '1' && c <= '9'; }
bool exp_char(u8 c) { return c == 'D' || c == 'E' || c == 'd' || c == 'e'; }

u32 strgtold12_impl(u8* pld12, const char** p_end_ptr, const char* str, i32 mult12, i32 scale, i32 decpt,
                    i32 implicit_E) {
    i8 buf[28];
    u8 tmpld[12];
    i8* manp = buf;
    u32 man_sign = 0;
    i32 exp_sign = 1;
    u32 manlen = 0;
    i32 found_digit = 0, found_decpoint = 0, found_exponent = 0, overflow = 0, underflow = 0;
    i32 pow = 0, exp_adj = 0;
    u32 result_flags = 0;
    const u8* p = reinterpret_cast<const u8*>(str);
    const u8* savedp = p;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
    int state = 0;
    for (;;) {
        u8 c = *p++;
        switch (state) {
        case 0:  // S_INIT
            if (nz_digit(c)) {
                state = 3;
                --p;
            } else if (c == decimal_point()) {
                state = 5;
            } else if (c == '+') {
                state = 2;
                man_sign = 0;
            } else if (c == '-') {
                state = 2;
                man_sign = 0x8000;
            } else if (c == '0') {
                state = 1;
            } else {
                --p;
                goto end;
            }
            continue;
        case 1:  // S_EAT0L
            found_digit = 1;
            if (nz_digit(c)) {
                state = 3;
                --p;
            } else if (c == decimal_point()) {
                state = 4;
            } else if (c == '+' || c == '-') {
                --p;
                state = 11;
            } else if (c == '0') {
                state = 1;
            } else if (exp_char(c)) {
                state = 6;
            } else {
                --p;
                goto end;
            }
            continue;
        case 2:  // S_SIGNM
            if (nz_digit(c)) {
                state = 3;
                --p;
            } else if (c == decimal_point()) {
                state = 5;
            } else if (c == '0') {
                state = 1;
            } else {
                p = savedp;
                goto end;
            }
            continue;
        case 3:  // S_GETL
            found_digit = 1;
            while (is_digit(c)) {
                if (manlen < 25) {
                    ++manlen;
                    *manp++ = static_cast<i8>(c - '0');
                } else {
                    ++exp_adj;
                }
                c = *p++;
            }
            if (c == decimal_point()) {
                state = 4;
                continue;
            }
            goto after_digits;
        case 4:  // S_GETR
            found_digit = 1;
            found_decpoint = 1;
            if (manlen == 0)
                while (c == '0') {
                    --exp_adj;
                    c = *p++;
                }
            while (is_digit(c)) {
                if (manlen < 25) {
                    ++manlen;
                    *manp++ = static_cast<i8>(c - '0');
                    --exp_adj;
                }
                c = *p++;
            }
            goto after_digits;
        case 5:  // S_POINT
            found_decpoint = 1;
            if (is_digit(c)) {
                state = 4;
                --p;
            } else {
                p = savedp;
                goto end;
            }
            continue;
        case 6:  // S_E
            savedp = p - 2;
            if (nz_digit(c)) {
                state = 9;
                --p;
            } else if (c == '+') {
                state = 7;
            } else if (c == '-') {
                exp_sign = -1;
                state = 7;
            } else if (c == '0') {
                state = 8;
            } else {
                p = savedp;
                goto end;
            }
            continue;
        case 7:  // S_SIGNE
            if (nz_digit(c)) {
                state = 9;
                --p;
            } else if (c == '0') {
                state = 8;
            } else {
                p = savedp;
                goto end;
            }
            continue;
        case 8:  // S_EAT0E
            found_exponent = 1;
            while (c == '0') c = *p++;
            if (nz_digit(c)) {
                state = 9;
                --p;
                continue;
            }
            --p;
            goto end;
        case 9: {  // S_GETE
            found_exponent = 1;
            u32 acc = 0;
            while (is_digit(c)) {
                acc = acc * 10u + static_cast<u32>(static_cast<i32>(static_cast<i8>(c))) - 0x30u;
                if (static_cast<i32>(acc) > 0x1450) {
                    acc = 0x1451;
                    break;
                }
                c = *p++;
            }
            pow = static_cast<i32>(acc);
            while (is_digit(c)) c = *p++;
            --p;
            goto end;
        }
        case 11:  // S_E_IMPLICIT
            if (implicit_E) {
                savedp = p - 1;
                if (c == '+') {
                    state = 7;
                } else if (c == '-') {
                    exp_sign = -1;
                    state = 7;
                } else {
                    p = savedp;
                    goto end;
                }
                continue;
            }
            --p;
            goto end;
        default:  // S_END (10): not reached
            goto end;
        }
    after_digits:  // tail shared by S_GETL / S_GETR (0x7c29c1)
        if (c == '+' || c == '-') {
            --p;
            state = 11;
        } else if (exp_char(c)) {
            state = 6;
        } else {
            --p;
            goto end;
        }
    }
end:
    *p_end_ptr = reinterpret_cast<const char*>(p);
    u32 w0 = 0, d2 = 0, d6 = 0, w10 = 0;
    if (found_digit) {
        i8* q;
        if (manlen > 24) {
            if (buf[23] >= 5) ++buf[23];
            manlen = 24;
            q = manp - 1;
            ++exp_adj;
        } else {
            q = manp;
        }
        if (manlen > 0) {
            for (;;) {  // buf[0] is never 0, the original does not re-test manlen here
                --q;
                if (*q != 0) break;
                --manlen;
                ++exp_adj;
            }
            mtold12(buf, manlen, tmpld);
            i32 e = pow;
            if (exp_sign < 0) e = -e;
            e += exp_adj;
            if (!found_exponent) e += scale;
            if (!found_decpoint) e -= decpt;
            if (e > 0x1450) {
                overflow = 1;
            } else if (e < -0x1450) {
                underflow = 1;
            } else {
                multtenpow12(tmpld, e, static_cast<u32>(mult12));
                w0 = rd16(tmpld, 0);
                d2 = rd32(tmpld, 2);
                d6 = rd32(tmpld, 6);
                w10 = rd16(tmpld, 10);
            }
            if (overflow) {
                w0 = 0;
                d2 = 0;
                d6 = 0x80000000u;
                w10 = 0x7fff;
                result_flags = 2;
            } else if (underflow) {
                w0 = d2 = d6 = w10 = 0;
                result_flags = 1;
            }
        }
    } else {
        result_flags = 4;
    }
    wr16(pld12, 0, w0);
    wr32(pld12, 2, d2);
    wr32(pld12, 6, d6);
    wr16(pld12, 10, w10 | man_sign);
    return result_flags;
}

u32 JOAN_CDECL crt_strgtold12(u8* pld12, const char** p_end_ptr, const char* str, i32 mult12, i32 scale, i32 decpt,
                           i32 implicit_E) {
    c_strgtold12.hit();
    return strgtold12_impl(pld12, p_end_ptr, str, mult12, scale, decpt, implicit_E);
}

// ------------------------------------------------------------------ converters
int JOAN_CDECL crt_ld12tod(const u8* ifp, u8* d) { return ld12cvt(ifp, d, kDoubleFormat); }  // 0x007c2509
int JOAN_CDECL crt_ld12tof(const u8* ifp, u8* f) { return ld12cvt(ifp, f, kFloatFormat); }   // 0x007c251f

// 0x007c2535 _ld12told: 64-bit mantissa rounding (same _RoundMan), 10-byte result.
int JOAN_CDECL crt_ld12told(const u8* ifp, u8* ld) {
    int retval = 0;
    const u32 expw = rd16(ifp, 10);
    const u32 sign = expw & 0x8000;
    u32 e = expw & 0x7fff;
    u32 man[3] = {rd32(ifp, 6), rd32(ifp, 2), static_cast<u32>(rd16(ifp, 0)) << 16};
    u32 hi;
    if (round_man(man, 64)) {
        hi = 0x80000000u;
        ++e;
    } else {
        hi = man[0];
    }
    if (static_cast<u16>(e) == 0x7fff) retval = 1;
    wr32(ld, 4, hi);
    wr16(ld, 8, sign | e);
    wr32(ld, 0, man[1]);
    return retval;
}

int JOAN_CDECL crt_atodbl(u8* d, const char* str) {  // 0x007c25af
    u8 ld12[12];
    const char* end;
    strgtold12_impl(ld12, &end, str, 0, 0, 0, 0);
    return crt_ld12tod(ld12, d);
}
int JOAN_CDECL crt_atoldbl(u8* ld, const char* str) {  // 0x007c25dc
    u8 ld12[12];
    const char* end;
    strgtold12_impl(ld12, &end, str, 1, 0, 0, 0);
    return crt_ld12told(ld12, ld);
}
int JOAN_CDECL crt_atoflt(u8* f, const char* str) {  // 0x007c260a
    u8 ld12[12];
    const char* end;
    strgtold12_impl(ld12, &end, str, 0, 0, 0, 0);
    return crt_ld12tof(ld12, f);
}
// 0x007c2c9d $I10_INPUT (__STRINGTOLD)
u32 JOAN_CDECL crt_i10_input(u8* ld, const char** end, const char* str, i32 mult12) {
    u8 ld12[12];
    u32 flags = strgtold12_impl(ld12, end, str, mult12, 0, 0, 0);
    if (crt_ld12told(ld12, ld) == 1) flags |= 2;
    return flags;
}

// 0x007b8f3c _fassign: flag != 0 -> double, else float. Returns `argument` (eax of the original).
char* JOAN_CDECL crt_fassign(i32 flag, char* argument, const char* number) {
    if (flag != 0) {
        u8 d[8];
        crt_atodbl(d, number);
        std::memcpy(argument, d, 8);
    } else {
        u8 f[4];
        crt_atoflt(f, number);
        std::memcpy(argument, f, 4);
    }
    return argument;
}

// The original's `fld qword ptr [src] / fstp qword ptr [dst]` copy: the same bits, except that an SNaN comes out
// quieted (x87 load of a signalling NaN). (The x87 status flags it raised are not observable.)
void x87_copy_double(const void* src, void* dst) {
    std::uint64_t b;
    std::memcpy(&b, src, 8);
    if ((b & 0x7ff0000000000000ull) == 0x7ff0000000000000ull && (b & 0x000fffffffffffffull) != 0)
        b |= 0x0008000000000000ull;
    std::memcpy(dst, &b, 8);
}

// 0x007b9c9d _fltin (VC6 _fltin2): FLT { flags +0, nbytes +4, lval +8, dval +0x10 }.
struct Flt {
    i32 flags;
    i32 nbytes;
    i32 lval;
    i32 pad;
    double dval;
};
static_assert(sizeof(Flt) == 0x18, "FLT");
Flt* JOAN_CDECL crt_fltin(Flt* flt, const char* str) {
    u8 ld12[12];
    const char* end;
    u8 d[8];
    u32 flags = 0;
    const u32 r = strgtold12_impl(ld12, &end, str, 0, 0, 0, 0);
    if (r & 4) {
        flags = 0x200;
        std::memset(d, 0, 8);
    } else {
        const int c = crt_ld12tod(ld12, d);
        if ((r & 2) || c == 1) flags = 0x80;
        if ((r & 1) || c == 2) flags |= 0x100;
    }
    flt->flags = static_cast<i32>(flags);
    x87_copy_double(d, &flt->dval);
    flt->nbytes = static_cast<i32>(end - str);
    return flt;
}

// ------------------------------------------------------------------ 0x007b1540 atol
std::int32_t JOAN_CDECL crt_atol(const char* s) {  // long is 32 bits in VC6 (64 on LP64)
    const u8* p = reinterpret_cast<const u8*>(s);
    while (ctype(*p, 8)) ++p;  // _SPACE
    i32 c = *p++;
    const i32 sign = c;
    if (c == '-' || c == '+') c = *p++;
    u32 total = 0;
    while (ctype(c, 4)) {  // _DIGIT
        total = static_cast<u32>(c) + total * 10u - 0x30u;
        c = *p++;
    }
    if (sign == '-') total = 0u - total;
    return static_cast<std::int32_t>(total);
}

// ------------------------------------------------------------------ input.c
struct Iobuf {  // VC6 FILE
    char* ptr;
    i32 cnt;
    char* base;
    i32 flag;
    i32 file;
    i32 charbuf;
    i32 bufsiz;
    char* tmpfname;
};

int JOAN_CDECL crt_inc(Iobuf* s) {  // 0x007bcccf _getc_lk
    const u32 n = static_cast<u32>(s->cnt) - 1u;
    s->cnt = static_cast<i32>(n);
    if ((n & 0x80000000u) == 0) {
        const u8 v = *reinterpret_cast<const u8*>(s->ptr);
        ++s->ptr;
        return v;
    }
    return joan::orig::arg::as<int>(joan::orig::libc::_filbuf(joan::orig::arg::mptr(s)));  // _filbuf
}
void JOAN_CDECL crt_un_inc(int ch, Iobuf* s) {  // 0x007bcce9
    if (ch != -1) joan::orig::libc::_ungetc_lk(joan::orig::arg::i32(ch), joan::orig::arg::mptr(s));  // _ungetc_lk
}
int JOAN_CDECL crt_whiteout(i32* counter, Iobuf* s) {  // 0x007bcd00
    int ch;
    do {
        ++*counter;
        ch = crt_inc(s);
    } while (isspace_fn(ch));
    return ch;
}
int JOAN_CDECL crt_hextodec(int ch) {  // 0x007bcc98
    if (is_digit(ch)) return ch;
    return (ch & ~0x20) - 7;
}

const u8 kScanC[] = "]";          // 0x00a31498
const u8 kScanS[] = " \t-\r]";    // 0x00a31490

int JOAN_CDECL crt_input(Iobuf* stream, const u8* format, char* arglist) {
    c_input.hit();
    char floatstring[0x160];
    u8 table[32];
    u8 mbbuf[2] = {0, 0};
    u16 wch = 0;
    u8 match = 0;
    i32 charcount = 0;
    i32 count = 0;
    int ch = 0;  // uninitialised in the original (see header)
    std::uintptr_t pointer = 0;
#if JOAN_SYSV_VARARGS
    bool pointer_pending = false;  // the vararg of a directive that turned out not to convert, kept for the next one
#else
    char* start_arglist = nullptr;
#endif
    u64 num64 = 0;

    if (*format == 0) goto error_return;
    for (;;) {
        if (ctype(*format, 8)) {  // 0x7bc2a2: white space in the format
            --charcount;
            crt_un_inc(crt_whiteout(&charcount, stream), stream);
            while (isspace_fn(*++format)) {
            }
        }
        if (*format == '%') {
            u8 suppress = 0, done_flag = 0, longone = 1, widechar = 0, negative = 0, reject = 0;
            u8 fl_wchar_arg = 0;
            const u8 rngch = 0;
            u32 number = 0;
            i32 started = 0, widthset = 0, width = 0, integer64 = 0;
            for (;;) {  // flags / width / size modifiers
                const u32 c = *++format;
                if (is_digit(static_cast<i32>(c))) {
                    ++widthset;
                    width = static_cast<i32>(static_cast<u32>(width) * 10u + c - 0x30u);
                } else {
                    switch (c) {
                    case 'N':
                    case 'F':
                        break;
                    case '*':
                        ++suppress;
                        break;
                    case 'I':
                        if (format[1] == '6' && format[2] == '4') {
                            ++integer64;
                            num64 = 0;
                            format += 2;
                        } else {
                            ++done_flag;
                        }
                        break;
                    case 'L':
                        ++longone;
                        break;
                    case 'h':
                        --longone;
                        --widechar;
                        break;
                    case 'l':
                        ++longone;
                        ++widechar;
                        break;
                    case 'w':
                        ++widechar;
                        break;
                    default:
                        ++done_flag;
                        break;
                    }
                }
                if (done_flag) break;
            }
            if (!suppress) {
#if JOAN_SYSV_VARARGS
                // every scanf vararg is a pointer: read from the caller's va_list (lib/crt/varargs.hpp)
                if (pointer_pending) pointer_pending = false;
                else pointer = va_arg(*reinterpret_cast<va_list*>(arglist), std::uintptr_t);
#else
                start_arglist = arglist;
                // every scanf vararg is a pointer: one va_list slot (4 bytes on x86, 8 on x64)
                std::memcpy(&pointer, arglist, sizeof pointer);
                arglist += sizeof(void*);
#endif
            }
            done_flag = 0;
            if (widechar == 0) widechar = (*format == 'S' || *format == 'C') ? 1 : 0xff;
            const u32 comchr0 = *format | 0x20u;
            u32 comchr = comchr0;
            if (comchr0 != 'n') {
                if (comchr0 == 'c' || comchr0 == '{') {
                    ++charcount;
                    ch = crt_inc(stream);
                } else {
                    ch = crt_whiteout(&charcount, stream);
                }
            }
            if (widthset != 0 && width == 0) goto unget_error;

            const u8* scanptr;
            if (comchr == 'n') {
                number = static_cast<u32>(charcount);
                if (suppress) goto next_directive;
                goto assign_num;
            } else if (comchr == 'c') {
                if (!widthset) {
                    ++width;
                    widthset = 1;
                }
                if (static_cast<i8>(widechar) > 0) fl_wchar_arg = 1;
                scanptr = kScanC;
                reject = 0xff;
                goto scanset;
            } else if (comchr == 's') {
                if (static_cast<i8>(widechar) > 0) fl_wchar_arg = 1;
                scanptr = kScanS;
                reject = 0xff;
                goto scanset;
            } else if (comchr == '{') {
                if (static_cast<i8>(widechar) > 0) fl_wchar_arg = 1;
                scanptr = ++format;
                if (*scanptr == '^') {
                    ++scanptr;
                    reject = 0xff;
                }
                goto scanset;
            } else if (comchr == 'e' || comchr == 'f' || comchr == 'g') {
                goto float_conv;
            } else if (comchr == 'x') {
                goto int_xi;
            } else if (comchr == 'i') {
                comchr = 'd';
                goto int_xi;
            } else if (comchr == 'p') {
                longone = 1;
                goto int_d;
            } else if (comchr == 'd' || comchr == 'o' || comchr == 'u') {
                goto int_d;
            } else {  // default: the character must match (e.g. "%%")
                if (static_cast<i32>(*format) != ch) goto unget_error;
                --match;
#if JOAN_SYSV_VARARGS
                if (!suppress) pointer_pending = true;  // give the vararg back
#else
                if (!suppress) arglist = start_arglist;
#endif
                goto next_directive;
            }

        scanset: {
            std::memset(table, 0, sizeof table);
            u8 prevchar;
            if (comchr0 == '{' && *scanptr == ']') {
                prevchar = ']';
                ++scanptr;
                table[0x5d >> 3] = 0x20;
            } else {
                prevchar = rngch;
            }
            for (;;) {
                const u8 rch = *scanptr;
                if (rch == ']') break;
                ++scanptr;
                if (rch == '-' && prevchar != 0 && *scanptr != ']') {
                    const u8 other = *scanptr++;
                    u8 lo, hi;
                    if (prevchar >= other) {
                        hi = prevchar;
                        lo = other;
                    } else {
                        hi = other;
                        lo = prevchar;
                    }
                    if (lo <= hi)
                        for (u32 x = lo, n = static_cast<u32>(hi - lo) + 1u; n != 0; --n, ++x)
                            table[x >> 3] |= static_cast<u8>(1u << (x & 7));
                    prevchar = 0;
                } else {
                    table[rch >> 3] |= static_cast<u8>(1u << (rch & 7));
                    prevchar = rch;
                }
            }
            if (*scanptr == 0) goto error_return;  // never true (scanptr is at ']')
            if (comchr0 == '{') format = scanptr;

            --charcount;
            crt_un_inc(ch, stream);
            std::uintptr_t start = pointer;
            for (;;) {
                if (widthset) {
                    const i32 old = width--;
                    if (old == 0) goto str_done;
                }
                ++charcount;
                ch = crt_inc(stream);
                if (ch == -1) break;
                const i32 t = static_cast<i32>(static_cast<i8>(table[ch >> 3])) ^ static_cast<i32>(static_cast<i8>(reject));
                if (!(t & (1 << (ch & 7)))) break;
                if (!suppress) {
                    if (fl_wchar_arg) {
                        mbbuf[0] = static_cast<u8>(ch);
                        if (pctype_hi(static_cast<u8>(ch)) & 0x80) {
                            ++charcount;
                            mbbuf[1] = static_cast<u8>(crt_inc(stream));
                        }
                        joan::orig::libc::mbtowc(&wch, joan::orig::arg::str(mbbuf), joan::orig::arg::u32(global<i32>(d::g_mb_cur_max)));  // mbtowc
                        std::memcpy(reinterpret_cast<void*>(pointer), &wch, 2);
                        pointer += 2;
                    } else {
                        *reinterpret_cast<u8*>(pointer) = static_cast<u8>(ch);
                        pointer += 1;
                    }
                } else {
                    ++start;
                }
            }
            --charcount;
            crt_un_inc(ch, stream);
        str_done:
            if (start == pointer) goto error_return;
            if (!suppress) {
                ++count;
                if (comchr0 != 'c') {
                    if (fl_wchar_arg) {
                        const u16 z = 0;
                        std::memcpy(reinterpret_cast<void*>(pointer), &z, 2);
                    } else {
                        *reinterpret_cast<u8*>(pointer) = 0;
                    }
                }
            }
            goto next_directive;
        }

        float_conv: {
            char* sp = floatstring;
            if (ch == '-') {
                *sp++ = '-';
                goto f_sign;
            }
            if (ch == '+') goto f_sign;
            goto f_width;
        f_sign:
            --width;
            ++charcount;
            ch = crt_inc(stream);
        f_width:
            if (!widthset || width > 0x15d) width = 0x15d;
            for (;;) {
                if (!is_digit(ch)) break;
                const i32 old = width--;
                if (old == 0) break;
                ++started;
                *sp++ = static_cast<char>(ch);
                ++charcount;
                ch = crt_inc(stream);
            }
            if (static_cast<u8>(ch) == decimal_point()) {
                const i32 old = width--;
                if (old != 0) {
                    ++charcount;
                    ch = crt_inc(stream);
                    *sp++ = static_cast<char>(decimal_point());
                    for (;;) {
                        if (!is_digit(ch)) break;
                        const i32 o2 = width--;
                        if (o2 == 0) break;
                        ++started;
                        *sp++ = static_cast<char>(ch);
                        ++charcount;
                        ch = crt_inc(stream);
                    }
                }
            }
            if (started && (ch == 'e' || ch == 'E')) {
                const i32 old = width--;
                if (old != 0) {
                    *sp++ = 'e';
                    ++charcount;
                    ch = crt_inc(stream);
                    bool incwidth = false;
                    if (ch == '-') {
                        *sp++ = static_cast<char>(ch);
                        incwidth = true;
                    } else if (ch == '+') {
                        incwidth = true;
                    }
                    if (incwidth) {
                        const i32 o2 = width--;
                        if (o2 == 0) {
                            width = 0;
                        } else {
                            ++charcount;
                            ch = crt_inc(stream);
                        }
                    }
                    for (;;) {
                        if (!is_digit(ch)) break;
                        const i32 o3 = width--;
                        if (o3 == 0) break;
                        ++started;
                        *sp++ = static_cast<char>(ch);
                        ++charcount;
                        ch = crt_inc(stream);
                    }
                }
            }
            --charcount;
            crt_un_inc(ch, stream);
            if (!started) goto error_return;
            if (suppress) goto next_directive;
            ++count;
            *sp = 0;
            cdecl_call<void>(global<std::uintptr_t>(d::fns_crt_a30fb0), static_cast<i32>(static_cast<i8>(longone)) - 1,
                             reinterpret_cast<char*>(pointer), static_cast<char*>(floatstring));  // _cfltcvt_tab[2]
            goto next_directive;
        }

        int_xi:  // 0x7bc4bc: %x, %i (comchr 'd')
            if (ch == '-') {
                negative = 1;
                goto xi_sign;
            }
            if (ch == '+') goto xi_sign;
            goto xi_zero;
        xi_sign:
            if (--width == 0 && widthset) {
                done_flag = 1;
            } else {
                ++charcount;
                ch = crt_inc(stream);
            }
        xi_zero:
            if (ch == '0') {
                ++charcount;
                ch = crt_inc(stream);
                if (static_cast<u8>(ch) == 'x' || static_cast<u8>(ch) == 'X') {
                    ++charcount;
                    ch = crt_inc(stream);
                    comchr = 'x';
                } else {
                    started = 1;
                    if (comchr == 'x') {
                        --charcount;
                        crt_un_inc(ch, stream);
                        ch = '0';
                    } else {
                        comchr = 'o';
                    }
                }
            }
            goto getnum;

        int_d:  // 0x7bc986: %d %o %u %p
            if (ch == '-') {
                negative = 1;
                goto d_sign;
            }
            if (ch == '+') goto d_sign;
            goto getnum;
        d_sign:
            if (--width == 0 && widthset) {
                done_flag = 1;
            } else {
                ++charcount;
                ch = crt_inc(stream);
            }

        getnum:  // 0x7bc9bb
            if (integer64) {
                if (!done_flag) {
                    for (;;) {
                        if (comchr == 'x') {
                            if (!is_xdigit(ch)) goto num_unget;
                            num64 <<= 4;
                            ch = crt_hextodec(ch);
                        } else {
                            if (!is_digit(ch)) goto num_unget;
                            if (comchr == 'o') {
                                if (ch >= '8') goto num_unget;
                                num64 <<= 3;
                            } else {
                                num64 *= 10u;
                            }
                        }
                        ++started;
                        num64 += static_cast<u64>(static_cast<i64>(ch - 0x30));
                        if (widthset && --width == 0) goto num_done;
                        ++charcount;
                        ch = crt_inc(stream);
                    }
                }
                goto num_done;
            } else {
                if (!done_flag) {
                    for (;;) {
                        if (comchr == 'x' || comchr == 'p') {
                            if (!is_xdigit(ch)) goto num_unget;
                            number <<= 4;
                            ch = crt_hextodec(ch);
                        } else {
                            if (!is_digit(ch)) goto num_unget;
                            if (comchr == 'o') {
                                if (ch >= '8') goto num_unget;
                                number <<= 3;
                            } else {
                                number *= 10u;
                            }
                        }
                        ++started;
                        number = number + static_cast<u32>(ch) - 0x30u;
                        if (widthset && --width == 0) goto num_done;
                        ++charcount;
                        ch = crt_inc(stream);
                    }
                }
                goto num_done;
            }
        num_unget:
            --charcount;
            crt_un_inc(ch, stream);
        num_done:
            if (negative) {
                if (integer64)
                    num64 = 0u - num64;
                else
                    number = 0u - number;
            }
            if (comchr == 'F') started = 0;  // (comchr is lower case: never)
            if (!started) goto error_return;
            if (suppress) goto next_directive;
            ++count;
        assign_num:
            if (integer64)
                std::memcpy(reinterpret_cast<void*>(pointer), &num64, 8);
            else if (longone)
                std::memcpy(reinterpret_cast<void*>(pointer), &number, 4);
            else {
                const u16 w = static_cast<u16>(number);
                std::memcpy(reinterpret_cast<void*>(pointer), &w, 2);
            }
        next_directive:
            ++match;
            ++format;
        } else {  // 0x7bcbe5: literal character
            ++charcount;
            ch = crt_inc(stream);
            const i32 fc = *format++;
            if (fc != ch) {
                --charcount;
                crt_un_inc(ch, stream);
                goto error_return;
            }
            if (pctype_hi(static_cast<u8>(ch)) & 0x80) {  // isleadbyte: the trail byte must match too
                ++charcount;
                const int ch2 = crt_inc(stream);
                const i32 fc2 = *format++;
                if (fc2 != ch2) {
                    --charcount;
                    crt_un_inc(ch2, stream);
                    --charcount;
                    crt_un_inc(ch, stream);
                    goto error_return;
                }
                --charcount;
            }
        }
        // 0x7bcc27
        if (ch == -1 && (format[0] != '%' || format[1] != 'n')) goto eof_return;
        if (*format == 0) goto error_return;
    }

unget_error:  // 0x7bcc49
    --charcount;
    crt_un_inc(ch, stream);
error_return:  // 0x7bcc79
    if (ch != -1) return count;
eof_return:  // 0x7bcc7f
    if (count != 0) return count;
    if (match != 0) return 0;
    return -1;
}

#define B(name, addr, fn) JOAN_REPLACE("crt/scanf", name, addr, fn)
B("crt::_input", 0x007bc273, &crt_input);
B("crt::_hextodec", 0x007bcc98, &crt_hextodec);
B("crt::_inc", 0x007bcccf, &crt_inc);
B("crt::_un_inc", 0x007bcce9, &crt_un_inc);
B("crt::_whiteout", 0x007bcd00, &crt_whiteout);
B("crt::_fassign", 0x007b8f3c, &crt_fassign);
B("crt::_fltin", 0x007b9c9d, &crt_fltin);
B("crt::_atodbl", 0x007c25af, &crt_atodbl);
B("crt::_atoldbl", 0x007c25dc, &crt_atoldbl);
B("crt::_atoflt", 0x007c260a, &crt_atoflt);
B("crt::__strgtold12", 0x007c27cc, &crt_strgtold12);
B("crt::$I10_INPUT", 0x007c2c9d, &crt_i10_input);
B("crt::_ld12tod", 0x007c2509, &crt_ld12tod);
B("crt::_ld12tof", 0x007c251f, &crt_ld12tof);
B("crt::_ld12told", 0x007c2535, &crt_ld12told);
B("crt::atol", 0x007b1540, &crt_atol);
#undef B

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::atol(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_atol>(a0)); }  // 0x7b1540
auto joan::orig::libc::_fltin(void* a0, const char* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_fltin>(a0, a1)); }  // 0x7b9c9d
auto joan::orig::libc::_input(void* a0, void* a1, const char* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_input>(a0, a1, a2)); }  // 0x7bc273
