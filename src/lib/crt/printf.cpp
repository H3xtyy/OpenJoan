// Original library sources (re/attribution.csv original_file, under lib/): crt/_fptostr.c, crt/cfout.c, crt/cvt.c,
//   crt/output.c, crt/sprintf.c, crt/vsprintf.c.
//
// Batch 114 / list 08: family F (output side) of the statically linked MSVC 6 C runtime - the printf
// engine and its floating-point formatting path (CRT_SYSTEM.md 5.2 family F). Module "crt/printf".
//
// Surface replaced as one unit (all cdecl, plain `ret`, entry-patched: library code has no incremental-link
// thunks; re/check_lib_entries.py: all entries safe):
//   0x007bb643 int   _output(FILE*, const char* fmt, va_list)   callers: sprintf, fprintf 0x7b3f73 (crt/stdio),
//                                                                vsprintf 0x7b4c8a (C++, lib/crt/string_format.cpp), _snprintf,
//                                                                _vsnprintf
//   0x007b2952 int   sprintf(char*, const char*, ...)           208 game sites + libjpeg format_message + C++
//   0x007b77e3 int   _snprintf(char*, size_t, const char*, ...)  D3DXGetErrorStringA 0x7693c4, game 0x6002c7
//   0x007b7b37 int   _vsnprintf(char*, size_t, const char*, va_list)   D3DX 0x789ef5
//   0x007b922c char* _cfltcvt(double*, char*, int fmt, int prec, int caps)   _cfltcvt_tab[0] / [5]
//   0x007b8ed6 void  _cropzeros(char*)                          _cfltcvt_tab[1]
//   0x007b8e7c void  _forcdecpt(char*)                          _cfltcvt_tab[3]
//   0x007b8f24 int   _positive(double*)                         _cfltcvt_tab[4] (no reader: dead, kept exact)
//   0x007c26ae STRFLT* _fltout2(double, STRFLT*, char*)         _cftoe/_cftof/_cftog + _fcvt 0x7c616e /
//                                                                _ecvt 0x7c61a1 (stay original, other family)
//   0x007c2637 void  _fptostr(char*, int, STRFLT*)              _cftoe/_cftof/_cftog + _fpcvt 0x7c61de
// Internal, reimplemented inside this file and NOT registered because every caller is in the surface above
// (they become unreachable): write_char 0x7bbd84, write_multi_char 0x7bbdb9, write_string 0x7bbdea,
// get_int_arg 0x7bbe22, get_int64_arg 0x7bbe2f, get_short_arg 0x7bbe3f, _cftoe 0x7b8f7a, _cftoe2 0x7b8fdb,
// _cftof 0x7b909d, _cftof2 0x7b90f2, _cftog 0x7b9199, _shift 0x7b927d, __dtold 0x7c270a,
// $I10_OUTPUT 0x7c4f81 (80-bit -> decimal digits), _aulldiv 0x7b2f30 / _aullrem 0x7c2ee0 (plain u64 / %).
// Shared with the scanf family and therefore NOT patched here (this file uses private copies with the same
// arithmetic): __multtenpow12 0x7c5434 (also called by __strgtold12 at 0x7c2c0c), __ld12mul 0x7c5214,
// __shl_12 0x7c4e5f, __shr_12 0x7c4e8d, __add_12 0x7c4e01, __addl 0x7c4de0 (also 0x7c2217), the power-of-ten
// tables __pow10pos 0x00a32248 / __pow10neg 0x00a323a8 (copied below as constants), and _fassign 0x7b8f3c
// (_cfltcvt_tab[2], used only by _input -> crt/scanf).
//
// State (all read live at the original addresses, nothing written; CRT_SYSTEM.md rule 2):
//   _pctype 0x00a30cc4 (lead-byte bit 0x8000 in ST_NORMAL, isdigit in _forcdecpt), __mb_cur_max 0x00a30ed0,
//   __decimal_point 0x00a30ed4 (first byte), __nullstring 0x00a31320 / __wnullstring 0x00a31324,
//   _cfltcvt_tab 0x00a30fa8 (_output calls _cfltcvt / _cropzeros / _forcdecpt THROUGH the table exactly like
//   the original, so an uninitialised table still ends in _fptrap / R6002).
// Other families, called through their original addresses: _flsbuf 0x7bb52b (stdio; string streams take its
// _IOSTRG path: flag |= _IOERR, return EOF), wctomb 0x7c2e16 (locale/errno), tolower 0x7b4ce3 and _isctype
// 0x7b7f51 (ctype).
//
// Exactness notes:
//  * The VC6 FILE layout is used as the original does: write_char decrements _cnt (+4) and stores through _ptr
//    (+0), else calls _flsbuf. fprintf (crt/stdio) passes a real FILE, so the stdio family must keep _ptr/_cnt
//    at +0/+4 while it calls this _output (or format into a string itself).
//  * The format state machine uses a copy of the VC6 __lookuptable (0x009e72a0, 0x59 bytes). Types with a
//    CH_TYPE class but no case (only 'B') re-output the previous conversion's text/textlen exactly like the
//    original (textlen starts at 0; text, textlen, radix share the original's merged variables).
//  * %e/%f/%g: the double argument is read as two dwords (no x87 load) and _cftoe/_cftof/_cftog copy it with
//    the effect of the original `fld qword / fstp qword` (x87_copy_double: an SNaN argument is quieted and
//    prints 1#QNAN; the x87 status flags it raised are not reproduced, nothing reads them). Everything else is
//    integer code.
//  * $I10_OUTPUT / __ld12mul / __multtenpow12 are transcribed operation by operation on byte buffers with the
//    original layout (including __ld12mul's partial-product loop whose carry word for the last row aliases
//    its loop counter, and the 16-bit-only carry increments), so the 96-bit arithmetic and its round-half-even
//    / sticky-bit rounding are identical.
//  * Deviation only where the original corrupts its own stack: the original's conversion buffer is 512 bytes
//    with no bound check. Here it is 0x1200 bytes; an integer precision or a float precision that would not fit
//    (precision > 0x1000, where the original has long overflowed into its frame) is clamped.
//    Frame ~4.8 KB (> 1 page: the compiler emits __chkstk probes). Reviewer check: the only game threads are
//    the two _beginthread procs (0x578fd0, stack arg 0x1000 = commit only, reserve from the exe header
//    = 1 MB in joan.exe and openjoan.exe /STACK:0x100000,0x1000; 0x578ff0 / 0x579810 async file reader) and
//    neither reaches the printf family; OpenAL / ReadFileEx completion APCs never call it. No game format
//    in the image has a width/precision large enough to reach the original's 512-byte limit.
// Verified against the original code (Linux harness running joan.exe's own bytes at their addresses, scratchpad
// w114_08/h, 4 seeds x 1.5M + 1M iterations): random structured and garbage formats through _output (C locale,
// ',' decimal point + lead bytes 0x81..0x9f, __mb_cur_max 2; PC53 and PC24; string FILEs with large and tiny
// _cnt so _flsbuf is hit), sprintf / _snprintf / _vsnprintf, _fltout2 + _fptostr, _cfltcvt / _cropzeros /
// _forcdecpt / _positive, $I10_OUTPUT (direct, random 80-bit inputs, digit counts and F-format flag), __ld12mul
// (incl. crafted denormal / sticky-bit cases) and __multtenpow12: 0 mismatches (output bytes, return values, FILE
// fields, %n stores, x87 status word). Mutation checks confirmed the harness detects single-operator changes.
#include "varargs.hpp"
#include "../../callconv.hpp"
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

JOAN_COUNTER(c_output, "crt::_output");
JOAN_COUNTER(c_cfltcvt, "crt::_cfltcvt");
JOAN_COUNTER(c_fltout2, "crt::_fltout2");

template <class T>
T& gl(img::Addr a) {
    return img::global<T>(a);
}
using joan::call::cdecl_call;

// ---- CRT state (original addresses) and other families' entry points
constexpr img::Addr kPctype = d::g_pctype;        // const unsigned short* _pctype
constexpr img::Addr kMbCurMax = d::g_mb_cur_max;      // int __mb_cur_max
constexpr img::Addr kDecimalPoint = d::g_decimal_point;  // char __decimal_point[]
constexpr img::Addr kNullString = d::g_nullString;    // char* __nullstring  -> "(null)"
constexpr img::Addr kWNullString = d::g_wNullString;   // wchar_t* __wnullstring -> L"(null)"
constexpr img::Addr kTabCfltcvt = d::g_tabCfltcvt;    // _cfltcvt_tab[0]
constexpr img::Addr kTabCropzeros = d::g_tabCropzeros;  // _cfltcvt_tab[1]
constexpr img::Addr kTabForcdecpt = d::g_tabForcdecpt;  // _cfltcvt_tab[3]
constexpr fn::Code kFlsbuf = fn::crt_flsbuf;        // int _flsbuf(int, FILE*)            crt/stdio
constexpr fn::Code kWctomb = fn::crt_wctomb;        // int wctomb(char*, wchar_t)          crt/locale
constexpr fn::Code kTolower = fn::crt_tolower;       // int tolower(int)                    crt/ctype
constexpr fn::Code kIsctype = fn::crt_isctype;       // int _isctype(int, int)              crt/ctype

// VC6 struct _iobuf (stdio.h), 32 bytes.
struct Iobuf {
    char* ptr;       // +0x00
    int cnt;         // +0x04
    char* base;      // +0x08
    int flag;        // +0x0c
    int file;        // +0x10
    int charbuf;     // +0x14
    int bufsiz;      // +0x18
    char* tmpfname;  // +0x1c
};
static_assert(sizeof(void*) != 4 || sizeof(Iobuf) == 0x20, "Iobuf size");

// VC6 STRFLT (cvt.h): what _fltout2 returns.
struct Strflt {
    i32 sign;        // +0x0 '-' or ' '
    i32 decpt;       // +0x4
    i32 flag;        // +0x8 $I10_OUTPUT return value
    char* mantissa;  // +0xc
};

// VC6 FOS ($I10_OUTPUT result), the original's _fltout2 frame gives man[] 24 bytes.
struct Fos {
    i16 exp;
    char sign;
    char manlen;
    char man[24];
};

u16 rd16(const u8* p) {
    u16 v;
    std::memcpy(&v, p, 2);
    return v;
}
u32 rd32(const u8* p) {
    u32 v;
    std::memcpy(&v, p, 4);
    return v;
}
void wr16(u8* p, u16 v) { std::memcpy(p, &v, 2); }
void wr32(u8* p, u32 v) { std::memcpy(p, &v, 4); }

std::size_t slen(const char* s) {
    std::size_t n = 0;
    while (s[n]) ++n;
    return n;
}

// The decimal point character exactly as the original loads it (`mov cl, byte ptr [__decimal_point]`).
char decimal_point() { return gl<char>(kDecimalPoint); }

// ======================================================================= 12-byte long double helpers (tenpow.c)
// 0x7c4e5f __shl_12 / 0x7c4e8d __shr_12: 96-bit shifts by one.
void shl12(u8* p) {
    const u32 a = rd32(p), b = rd32(p + 4), c = rd32(p + 8);
    wr32(p, a << 1);
    wr32(p + 4, (b << 1) | (a >> 31));
    wr32(p + 8, (c << 1) | (b >> 31));
}
void shr12(u8* p) {
    const u32 a = rd32(p), b = rd32(p + 4), c = rd32(p + 8);
    wr32(p + 4, (b >> 1) | (c << 31));
    wr32(p + 8, c >> 1);
    wr32(p, (a >> 1) | (b << 31));
}
// 0x7c4de0 __addl: *sum = x + y, returns the carry.
u32 addl(u32 x, u32 y, u32* sum) {
    const u32 s = x + y;
    *sum = s;
    return (s < x || s < y) ? 1u : 0u;
}
// 0x7c4e01 __add_12 (an exact 96-bit addition mod 2^96, carried the original's way).
void add12(u8* x, const u8* y) {
    u32 s;
    if (addl(rd32(x), rd32(y), &s)) {
        wr32(x, s);
        u32 t;
        if (addl(rd32(x + 4), 1, &t)) wr32(x + 8, rd32(x + 8) + 1);
        wr32(x + 4, t);
    } else {
        wr32(x, s);
    }
    u32 t;
    if (addl(rd32(x + 4), rd32(y + 4), &t)) {
        wr32(x + 4, t);
        wr32(x + 8, rd32(x + 8) + 1);
    } else {
        wr32(x + 4, t);
    }
    addl(rd32(x + 8), rd32(y + 8), &t);
    wr32(x + 8, t);
}

// 0x7c5214 __ld12mul(x, y): x *= y on 12-byte long doubles. Transcription of the original instruction order.
// `r` mirrors the original frame [ebp-0x24..ebp-0x11]: product r[0..11], then the inner loop counter
// [ebp-0x18] at r+12 (the carry word of the last row lands there) and the sticky count [ebp-0x14] at r+16.
void ld12mul(u8* x, const u8* y) {
    const u16 xe = rd16(x + 10), ye = rd16(y + 10);
    const u16 sign = static_cast<u16>((xe ^ ye) & 0x8000);
    const u16 ex = static_cast<u16>(xe & 0x7fff), ey = static_cast<u16>(ye & 0x7fff);
    u16 expsum = static_cast<u16>(ex + ey);
    u8 r[20] = {};
    // Padded copies so that the (unreachable) aliased extra rows read defined bytes.
    u8 xs[16] = {}, ys[16] = {};
    std::memcpy(xs, x, 12);
    std::memcpy(ys + 4, y, 12);
    const u8* yb = ys + 4;
    if (ex >= 0x7fff || ey >= 0x7fff || expsum > 0xbffd) goto overflow;
    if (expsum <= 0x3fbf) {
        wr32(x + 8, 0);
        wr32(x + 4, 0);
        wr32(x, 0);
        return;
    }
    if (ex == 0) {
        ++expsum;
        if ((rd32(x + 8) & 0x7fffffff) == 0 && rd32(x + 4) == 0 && rd32(x) == 0) {
            wr16(x + 10, 0);
            return;
        }
    }
    if (ey == 0) {
        ++expsum;
        if ((rd32(y + 8) & 0x7fffffff) == 0 && rd32(y + 4) == 0 && rd32(y) == 0) {
            wr32(x + 8, 0);
            wr32(x + 4, 0);
            wr32(x, 0);
            return;
        }
    }
    {
        u32 i = 0;
        u32 rp = 4;  // offset of the carry word; the accumulator dword is at rp - 4
        i32 n = 5;
        do {
            if (n > 0) {
                u32 xo = i * 2;  // x word offset
                i32 yo = 8;      // y word offset
                wr32(r + 12, static_cast<u32>(n));
                do {
                    const u32 xw = xo + 2 <= 16 ? rd16(xs + xo) : 0;
                    const u32 yw = yo >= -4 ? rd16(yb + yo) : 0;
                    const u32 prod = xw * yw;
                    u32 s;
                    if (addl(rd32(r + rp - 4), prod, &s)) {
                        wr32(r + rp - 4, s);
                        wr16(r + rp, static_cast<u16>(rd16(r + rp) + 1));
                    } else {
                        wr32(r + rp - 4, s);
                    }
                    xo += 2;
                    yo -= 2;
                    wr32(r + 12, rd32(r + 12) - 1);
                } while (rd32(r + 12) != 0 && xo < 16 && yo >= -4);
            }
            rp += 2;
            ++i;
            --n;
        } while (n > 0);
    }
    expsum = static_cast<u16>(expsum + 0xc002);
    if (static_cast<i16>(expsum) > 0) {
        while (!(r[11] & 0x80)) {
            shl12(r);
            expsum = static_cast<u16>(expsum + 0xffff);
            if (!(static_cast<i16>(expsum) > 0)) break;
        }
        if (static_cast<i16>(expsum) > 0) goto round;
    }
    expsum = static_cast<u16>(expsum + 0xffff);
    if (static_cast<i16>(expsum) < 0) {
        u32 k = static_cast<u32>(-static_cast<i32>(static_cast<i16>(expsum)));
        expsum = static_cast<u16>(expsum + k);
        u32 sticky = 0;
        do {
            if (r[0] & 1) ++sticky;
            shr12(r);
        } while (--k);
        if (sticky) r[0] |= 1;
    }
round:
    if (rd16(r) > 0x8000 || (rd32(r) & 0x1ffff) == 0x18000) {
        if (rd32(r + 2) == 0xffffffffu) {
            wr32(r + 2, 0);
            if (rd32(r + 6) == 0xffffffffu) {
                wr32(r + 6, 0);
                if (rd16(r + 10) == 0xffff) {
                    ++expsum;
                    wr16(r + 10, 0x8000);
                } else {
                    wr16(r + 10, static_cast<u16>(rd16(r + 10) + 1));
                }
            } else {
                wr32(r + 6, rd32(r + 6) + 1);
            }
        } else {
            wr32(r + 2, rd32(r + 2) + 1);
        }
    }
    if (expsum >= 0x7fff) goto overflow;
    wr16(x, rd16(r + 2));
    wr32(x + 2, rd32(r + 4));
    wr32(x + 6, rd32(r + 8));
    wr16(x + 10, static_cast<u16>(expsum | sign));
    return;
overflow:
    wr32(x + 4, 0);
    wr32(x, 0);
    wr32(x + 8, sign ? 0xffff8000u : 0x7fff8000u);
}

// Copies of __pow10pos (0x00a32248) / __pow10neg (0x00a323a8): 12-byte long doubles 10^1..10^7, 10^8..10^56,
// 10^64..10^448, 10^512..10^3584, 10^4096 (and the reciprocals), low byte first.
constexpr u8 kPow10Pos[29][12] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x40},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x05, 0x40},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfa, 0x08, 0x40},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9c, 0x0c, 0x40},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xc3, 0x0f, 0x40},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xf4, 0x12, 0x40},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x96, 0x98, 0x16, 0x40},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xbc, 0xbe, 0x19, 0x40},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xbf, 0xc9, 0x1b, 0x8e, 0x34, 0x40},
    {0x00, 0x00, 0x00, 0xa1, 0xed, 0xcc, 0xce, 0x1b, 0xc2, 0xd3, 0x4e, 0x40},
    {0x20, 0xf0, 0x9e, 0xb5, 0x70, 0x2b, 0xa8, 0xad, 0xc5, 0x9d, 0x69, 0x40},
    {0xd0, 0x5d, 0xfd, 0x25, 0xe5, 0x1a, 0x8e, 0x4f, 0x19, 0xeb, 0x83, 0x40},
    {0x71, 0x96, 0xd7, 0x95, 0x43, 0x0e, 0x05, 0x8d, 0x29, 0xaf, 0x9e, 0x40},
    {0xf9, 0xbf, 0xa0, 0x44, 0xed, 0x81, 0x12, 0x8f, 0x81, 0x82, 0xb9, 0x40},
    {0xbf, 0x3c, 0xd5, 0xa6, 0xcf, 0xff, 0x49, 0x1f, 0x78, 0xc2, 0xd3, 0x40},
    {0x6f, 0xc6, 0xe0, 0x8c, 0xe9, 0x80, 0xc9, 0x47, 0xba, 0x93, 0xa8, 0x41},
    {0xbc, 0x85, 0x6b, 0x55, 0x27, 0x39, 0x8d, 0xf7, 0x70, 0xe0, 0x7c, 0x42},
    {0xbc, 0xdd, 0x8e, 0xde, 0xf9, 0x9d, 0xfb, 0xeb, 0x7e, 0xaa, 0x51, 0x43},
    {0xa1, 0xe6, 0x76, 0xe3, 0xcc, 0xf2, 0x29, 0x2f, 0x84, 0x81, 0x26, 0x44},
    {0x28, 0x10, 0x17, 0xaa, 0xf8, 0xae, 0x10, 0xe3, 0xc5, 0xc4, 0xfa, 0x44},
    {0xeb, 0xa7, 0xd4, 0xf3, 0xf7, 0xeb, 0xe1, 0x4a, 0x7a, 0x95, 0xcf, 0x45},
    {0x65, 0xcc, 0xc7, 0x91, 0x0e, 0xa6, 0xae, 0xa0, 0x19, 0xe3, 0xa3, 0x46},
    {0x0d, 0x65, 0x17, 0x0c, 0x75, 0x81, 0x86, 0x75, 0x76, 0xc9, 0x48, 0x4d},
    {0x58, 0x42, 0xe4, 0xa7, 0x93, 0x39, 0x3b, 0x35, 0xb8, 0xb2, 0xed, 0x53},
    {0x4d, 0xa7, 0xe5, 0x5d, 0x3d, 0xc5, 0x5d, 0x3b, 0x8b, 0x9e, 0x92, 0x5a},
    {0xff, 0x5d, 0xa6, 0xf0, 0xa1, 0x20, 0xc0, 0x54, 0xa5, 0x8c, 0x37, 0x61},
    {0xd1, 0xfd, 0x8b, 0x5a, 0x8b, 0xd8, 0x25, 0x5d, 0x89, 0xf9, 0xdb, 0x67},
    {0xaa, 0x95, 0xf8, 0xf3, 0x27, 0xbf, 0xa2, 0xc8, 0x5d, 0xdd, 0x80, 0x6e},
    {0x4c, 0xc9, 0x9b, 0x97, 0x20, 0x8a, 0x02, 0x52, 0x60, 0xc4, 0x25, 0x75},
};
constexpr u8 kPow10Neg[29][12] = {
    {0xcd, 0xcc, 0xcd, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xfb, 0x3f},
    {0x71, 0x3d, 0x0a, 0xd7, 0xa3, 0x70, 0x3d, 0x0a, 0xd7, 0xa3, 0xf8, 0x3f},
    {0x5a, 0x64, 0x3b, 0xdf, 0x4f, 0x8d, 0x97, 0x6e, 0x12, 0x83, 0xf5, 0x3f},
    {0xc3, 0xd3, 0x2c, 0x65, 0x19, 0xe2, 0x58, 0x17, 0xb7, 0xd1, 0xf1, 0x3f},
    {0xd0, 0x0f, 0x23, 0x84, 0x47, 0x1b, 0x47, 0xac, 0xc5, 0xa7, 0xee, 0x3f},
    {0x40, 0xa6, 0xb6, 0x69, 0x6c, 0xaf, 0x05, 0xbd, 0x37, 0x86, 0xeb, 0x3f},
    {0x33, 0x3d, 0xbc, 0x42, 0x7a, 0xe5, 0xd5, 0x94, 0xbf, 0xd6, 0xe7, 0x3f},
    {0xc2, 0xfd, 0xfd, 0xce, 0x61, 0x84, 0x11, 0x77, 0xcc, 0xab, 0xe4, 0x3f},
    {0x2f, 0x4c, 0x5b, 0xe1, 0x4d, 0xc4, 0xbe, 0x94, 0x95, 0xe6, 0xc9, 0x3f},
    {0x92, 0xc4, 0x53, 0x3b, 0x75, 0x44, 0xcd, 0x14, 0xbe, 0x9a, 0xaf, 0x3f},
    {0xde, 0x67, 0xba, 0x94, 0x39, 0x45, 0xad, 0x1e, 0xb1, 0xcf, 0x94, 0x3f},
    {0x24, 0x23, 0xc6, 0xe2, 0xbc, 0xba, 0x3b, 0x31, 0x61, 0x8b, 0x7a, 0x3f},
    {0x61, 0x55, 0x59, 0xc1, 0x7e, 0xb1, 0x53, 0x7c, 0x12, 0xbb, 0x5f, 0x3f},
    {0xd7, 0xee, 0x2f, 0x8d, 0x06, 0xbe, 0x92, 0x85, 0x15, 0xfb, 0x44, 0x3f},
    {0x24, 0x3f, 0xa5, 0xe9, 0x39, 0xa5, 0x27, 0xea, 0x7f, 0xa8, 0x2a, 0x3f},
    {0x7d, 0xac, 0xa1, 0xe4, 0xbc, 0x64, 0x7c, 0x46, 0xd0, 0xdd, 0x55, 0x3e},
    {0x63, 0x7b, 0x06, 0xcc, 0x23, 0x54, 0x77, 0x83, 0xff, 0x91, 0x81, 0x3d},
    {0x91, 0xfa, 0x3a, 0x19, 0x7a, 0x63, 0x25, 0x43, 0x31, 0xc0, 0xac, 0x3c},
    {0x21, 0x89, 0xd1, 0x38, 0x82, 0x47, 0x97, 0xb8, 0x00, 0xfd, 0xd7, 0x3b},
    {0xdc, 0x88, 0x58, 0x08, 0x1b, 0xb1, 0xe8, 0xe3, 0x86, 0xa6, 0x03, 0x3b},
    {0xc6, 0x84, 0x45, 0x42, 0x07, 0xb6, 0x99, 0x75, 0x37, 0xdb, 0x2e, 0x3a},
    {0x33, 0x71, 0x1c, 0xd2, 0x23, 0xdb, 0x32, 0xee, 0x49, 0x90, 0x5a, 0x39},
    {0xa6, 0x87, 0xbe, 0xc0, 0x57, 0xda, 0xa5, 0x82, 0xa6, 0xa2, 0xb5, 0x32},
    {0xe2, 0x68, 0xb2, 0x11, 0xa7, 0x52, 0x9f, 0x44, 0x59, 0xb7, 0x10, 0x2c},
    {0x25, 0x49, 0xe4, 0x2d, 0x36, 0x34, 0x4f, 0x53, 0xae, 0xce, 0x6b, 0x25},
    {0x8f, 0x59, 0x04, 0xa4, 0xc0, 0xde, 0xc2, 0x7d, 0xfb, 0xe8, 0xc6, 0x1e},
    {0x9e, 0xe7, 0x88, 0x5a, 0x57, 0x91, 0x3c, 0xbf, 0x50, 0x83, 0x22, 0x18},
    {0x4e, 0x4b, 0x65, 0x62, 0xfd, 0x83, 0x8f, 0xaf, 0x06, 0x94, 0x7d, 0x11},
    {0xe4, 0x2d, 0xde, 0x9f, 0xce, 0xd2, 0xc8, 0x04, 0xdd, 0xa6, 0xd8, 0x0a},
};

// 0x7c5434 __multtenpow12(x, exp, mult12): x *= 10^exp, one table entry per octal digit of |exp|. Entries whose
// low word is >= 0x8000 are pre-rounded upwards: the original multiplies by a copy with dword +2 decremented.
void multtenpow12(u8* x, i32 exp, int mult12) {
    if (exp == 0) return;
    const u8(*table)[12] = kPow10Pos;
    u32 e = static_cast<u32>(exp);
    if (exp < 0) {
        e = 0u - e;
        table = kPow10Neg;
    }
    if (mult12 == 0) wr16(x, 0);
    u32 group = 0;
    while (e != 0) {
        const u32 idx = e & 7;
        e = static_cast<u32>(static_cast<i32>(e) >> 3);
        if (idx != 0) {
            const u32 k = group * 7 + idx - 1;
            if (k < 29) {
                const u8* entry = table[k];
                u8 tmp[12];
                if (rd16(entry) >= 0x8000) {
                    std::memcpy(tmp, entry, 12);
                    wr32(tmp + 2, rd32(tmp + 2) - 1);
                    entry = tmp;
                }
                ld12mul(x, entry);
            }
        }
        ++group;
    }
}

// 0x7c4f81 $I10_OUTPUT(_LDOUBLE ld, int ndigits, unsigned output_flags, FOS* fos).
int i10_output(u32 manlo, u32 manhi, u16 ldexp, i32 ndigits, u32 output_flags, Fos* fos) {
    static constexpr u8 kOneTenth[12] = {0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xfb, 0x3f};
    u8* const f = reinterpret_cast<u8*>(fos);
    char* const man = fos->man;
    const u32 sign = ldexp & 0x8000u;
    const u16 expn = static_cast<u16>(ldexp & 0x7fff);
    int retval = 1;
    fos->sign = sign ? '-' : ' ';
    if (expn == 0 && manhi == 0 && manlo == 0) goto zero;
    if (expn == 0x7fff) {
        fos->exp = 1;
        const char* text;
        if (!(manhi == 0x80000000u && manlo == 0) && !(manhi & 0x40000000u)) {
            text = "1#SNAN";
        } else if (sign && manhi == 0xc0000000u) {
            text = manlo == 0 ? "1#IND" : "1#QNAN";
        } else if (manhi == 0x80000000u && manlo == 0) {
            text = "1#INF";
        } else {
            text = "1#QNAN";
        }
        const std::size_t n = slen(text);
        std::memcpy(man, text, n + 1);
        fos->manlen = static_cast<char>(n);
        return 0;
    }
    {
        // Low estimate of log10(x) in 16.16 fixed point (hh/ll/mm terms of the original).
        const u32 r = static_cast<u32>(expn) * 0x4d10u + ((static_cast<u32>(expn) >> 8) + (manhi >> 24) * 2u) * 0x4du -
                      0x134312f4u;
        i16 ir = static_cast<i16>(static_cast<i32>(r) >> 16);
        u8 ld12[12];
        wr16(ld12, 0);
        wr32(ld12 + 2, manlo);
        wr32(ld12 + 6, manhi);
        wr16(ld12 + 10, expn);
        multtenpow12(ld12, -static_cast<i32>(ir), 1);
        if (rd16(ld12 + 10) >= 0x3fff) {
            ir = static_cast<i16>(ir + 1);
            ld12mul(ld12, kOneTenth);
        }
        fos->exp = ir;
        i32 nd = ndigits;
        if (output_flags & 1) {
            nd = static_cast<i32>(static_cast<u32>(ndigits) + static_cast<u32>(static_cast<i32>(ir)));
            if (nd <= 0) goto zero;
        }
        if (nd > 21) nd = 21;
        const i32 sh = static_cast<i32>(rd16(ld12 + 10)) - 0x3ffe;
        wr16(ld12 + 10, 0);
        for (int k = 0; k < 8; ++k) shl12(ld12);
        if (sh < 0) {
            u32 k = static_cast<u32>(-sh) & 0xff;
            while (k) {
                shr12(ld12);
                --k;
            }
        }
        // p is kept as an offset from fos so that the (unreachable for ndigits >= 0) read of man[-1] stays
        // inside the struct, like the original reads fos->ManLen there.
        i32 p = 4;
        const i32 cnt = nd + 1;
        for (i32 c = cnt; c > 0; --c) {
            u8 tmp[12];
            std::memcpy(tmp, ld12, 12);
            shl12(ld12);
            shl12(ld12);
            add12(ld12, tmp);
            shl12(ld12);
            if (p < static_cast<i32>(sizeof(Fos))) f[p] = static_cast<u8>(ld12[11] + 0x30);
            ++p;
            ld12[11] = 0;
        }
        const i8 last = static_cast<i8>(f[p - 1]);
        i32 q = p - 2;
        if (last >= 0x35) {
            while (q >= 4 && f[q] == '9') {
                f[q] = '0';
                --q;
            }
            if (q < 4) {
                ++q;
                fos->exp = static_cast<i16>(fos->exp + 1);
            }
            f[q] = static_cast<u8>(f[q] + 1);
        } else {
            while (q >= 4 && f[q] == '0') --q;
            if (q < 4) {
                fos->exp = 0;
                fos->sign = ' ';
                fos->manlen = 1;
                man[0] = '0';
                man[1] = 0;
                return 1;
            }
        }
        const u8 len = static_cast<u8>(static_cast<u8>(q) - 3);
        fos->manlen = static_cast<char>(len);
        man[static_cast<i8>(len)] = 0;
        return retval;
    }
zero:
    fos->exp = 0;
    fos->sign = ' ';
    fos->manlen = 1;
    man[0] = '0';
    man[1] = 0;
    return 1;
}

// 0x7c270a __dtold(_LDOUBLE* ld, const double* x): double bits -> 80-bit, denormals normalised, a zero of either
// sign becomes +0.
void dtold(u32 lo, u32 hi, u32* manlo, u32* manhi, u16* ldexp) {
    const u32 w = hi >> 16;
    const u32 sign = w & 0x8000;
    const u32 e11 = (w >> 4) & 0x7ff;
    u32 mhi = hi & 0xfffff;
    u32 intbit = 0x80000000u;
    u32 e16;
    if (e11 == 0) {
        if (mhi == 0 && lo == 0) {
            *manlo = 0;
            *manhi = 0;
            *ldexp = 0;
            return;
        }
        e16 = 0x3c01;
        intbit = 0;
    } else if (e11 == 0x7ff) {
        e16 = 0x7fff;
    } else {
        e16 = e11 + 0x3c00;
    }
    u32 h = (mhi << 11) | (lo >> 21) | intbit;
    u32 l = lo << 11;
    while (!(h & 0x80000000u)) {
        h = (h << 1) | (l >> 31);
        l <<= 1;
        e16 += 0xffff;
    }
    *manlo = l;
    *manhi = h;
    *ldexp = static_cast<u16>(sign | e16);
}

// ======================================================================= surface: _fltout2 / _fptostr
// 0x007c26ae STRFLT* _fltout2(double x, STRFLT* flt, char* resultstr). The double is taken as its two dwords.
Strflt* JOAN_CDECL crt_fltout2(u32 lo, u32 hi, Strflt* flt, char* resultstr) {
    c_fltout2.hit();
    u32 ml, mh;
    u16 le;
    dtold(lo, hi, &ml, &mh, &le);
    Fos fos;
    std::memset(&fos, 0, sizeof fos);
    const int ret = i10_output(ml, mh, le, 17, 0, &fos);
    flt->flag = ret;
    flt->sign = static_cast<i32>(static_cast<i8>(fos.sign));
    flt->decpt = static_cast<i32>(fos.exp);
    std::memcpy(resultstr, fos.man, slen(fos.man) + 1);
    flt->mantissa = resultstr;
    return flt;
}

// 0x007c2637 void _fptostr(char* buf, int digits, STRFLT* pflt): digits rounded (half up on the next mantissa
// digit), '0'-padded; a carry out of the first digit bumps decpt, otherwise the leading '0' is removed.
void JOAN_CDECL crt_fptostr(char* buf, int digits, Strflt* pflt) {
    const char* m = pflt->mantissa;
    char* p = buf + 1;
    buf[0] = '0';
    if (digits > 0) {
        for (int n = digits; n != 0; --n) {
            char c = *m;
            if (c != 0) {
                ++m;
            } else {
                c = '0';
            }
            *p++ = c;
        }
    }
    *p = 0;
    if (digits >= 0 && static_cast<i8>(*m) >= 0x35) {
        --p;
        while (*p == '9') {
            *p = '0';
            --p;
        }
        ++*p;
    }
    if (buf[0] == '1') {
        ++pflt->decpt;
    } else {
        std::memmove(buf, buf + 1, slen(buf + 1) + 1);
    }
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

// 0x7b927d _shift(s, dist): memmove(s + dist, s, strlen(s) + 1) when dist != 0.
void shift(char* s, int dist) {
    if (dist) std::memmove(s + dist, s, slen(s) + 1);
}

// 0x7b8fdb _cftoe2(buf, ndigit, caps, pflt, g_fmt)
char* cftoe2(char* buf, int ndigit, int caps, Strflt* pflt, char g_fmt) {
    if (g_fmt) shift(buf + (pflt->sign == '-'), ndigit > 0);
    char* p = buf;
    if (pflt->sign == '-') {
        *buf = '-';
        p = buf + 1;
    }
    if (ndigit > 0) {
        *p = p[1];
        ++p;
        *p = decimal_point();
    }
    p = p + (g_fmt == 0) + ndigit;
    std::memcpy(p, "e+000", 6);
    if (caps) *p = 'E';
    ++p;
    if (*pflt->mantissa != '0') {
        i32 exp = pflt->decpt - 1;
        if (exp < 0) {
            exp = -exp;
            *p = '-';
        }
        ++p;
        if (exp >= 100) {
            *p = static_cast<char>(*p + exp / 100);
            exp %= 100;
        }
        ++p;
        if (exp >= 10) {
            *p = static_cast<char>(*p + exp / 10);
            exp %= 10;
        }
        p[1] = static_cast<char>(p[1] + exp);
    }
    return buf;
}

// 0x7b90f2 _cftof2(buf, ndigit, pflt, g_fmt)
char* cftof2(char* buf, int ndigit, Strflt* pflt, char g_fmt) {
    const i32 decpt1 = pflt->decpt - 1;
    if (g_fmt && decpt1 == ndigit) {
        char* q = buf + (pflt->sign == '-') + decpt1;
        q[0] = '0';
        q[1] = 0;
    }
    char* p = buf;
    if (pflt->sign == '-') {
        *buf = '-';
        p = buf + 1;
    }
    if (pflt->decpt <= 0) {
        shift(p, 1);
        *p++ = '0';
    } else {
        p += pflt->decpt;
    }
    if (ndigit > 0) {
        shift(p, 1);
        *p++ = decimal_point();
        const i32 d = pflt->decpt;
        if (d < 0) {
            if (g_fmt || !(ndigit < -d)) ndigit = -d;
            shift(p, ndigit);
            std::memset(p, '0', static_cast<std::size_t>(ndigit));
        }
    }
    return buf;
}

// 0x7b8f7a _cftoe(double* arg, char* buf, int ndigit, int caps)
char* cftoe(const void* arg, char* buf, int ndigit, int caps) {
    u32 d[2];
    x87_copy_double(arg, d);
    Strflt flt;
    char resstr[24];
    Strflt* pflt = crt_fltout2(d[0], d[1], &flt, resstr);
    crt_fptostr(buf + (pflt->sign == '-') + (ndigit > 0), ndigit + 1, pflt);
    cftoe2(buf, ndigit, caps, pflt, 0);
    return buf;
}

// 0x7b909d _cftof(double* arg, char* buf, int ndigit)
char* cftof(const void* arg, char* buf, int ndigit) {
    u32 d[2];
    x87_copy_double(arg, d);
    Strflt flt;
    char resstr[24];
    Strflt* pflt = crt_fltout2(d[0], d[1], &flt, resstr);
    crt_fptostr(buf + (pflt->sign == '-'), pflt->decpt + ndigit, pflt);
    cftof2(buf, ndigit, pflt, 0);
    return buf;
}

// 0x7b9199 _cftog(double* arg, char* buf, int ndigit, int caps)
char* cftog(const void* arg, char* buf, int ndigit, int caps) {
    u32 d[2];
    x87_copy_double(arg, d);
    Strflt flt;
    char resstr[24];
    Strflt* pflt = crt_fltout2(d[0], d[1], &flt, resstr);
    const i32 magnitude = pflt->decpt - 1;
    char* p = buf + (pflt->sign == '-');
    crt_fptostr(p, ndigit, pflt);
    const i32 mag2 = pflt->decpt - 1;
    const bool round_expansion = magnitude < mag2;
    if (mag2 < -4 || mag2 >= ndigit) return cftoe2(buf, ndigit, caps, pflt, 1);
    if (round_expansion) {
        while (*p++) {
        }
        p[-2] = 0;
    }
    return cftof2(buf, ndigit, pflt, 1);
}

// 0x007b922c char* _cfltcvt(double* arg, char* buf, int format, int precision, int caps)
char* JOAN_CDECL crt_cfltcvt(void* arg, char* buf, int format, int precision, int caps) {
    c_cfltcvt.hit();
    if (format == 'e' || format == 'E') return cftoe(arg, buf, precision, caps);
    if (format == 'f') return cftof(arg, buf, precision);
    return cftog(arg, buf, precision, caps);
}

// 0x007b8ed6 void _cropzeros(char* buf): drop trailing zeros (and a then-trailing decimal point) of the fraction.
void JOAN_CDECL crt_cropzeros(char* buf) {
    const char dp = decimal_point();
    char* p = buf;
    while (*p && *p != dp) ++p;
    const char c = *p++;
    if (c == 0) return;
    while (*p && *p != 'e' && *p != 'E') ++p;
    char* stop = p;
    do {
        --p;
    } while (*p == '0');
    if (*p == dp) --p;
    char ch;
    do {
        ch = *stop++;
        *++p = ch;
    } while (ch);
}

// 0x007b8e7c void _forcdecpt(char* buf): insert the decimal point after the leading digits (for "%#.0e/f").
void JOAN_CDECL crt_forcdecpt(char* buf) {
    char* p = buf;
    if (cdecl_call<int>(kTolower, static_cast<int>(*p)) != 'e') {
        for (;;) {
            ++p;
            const int c = static_cast<int>(*p);
            int isdig;
            if (gl<i32>(kMbCurMax) > 1) {
                isdig = cdecl_call<int>(kIsctype, c, 4);
            } else {
                const u8* tab = reinterpret_cast<const u8*>(gl<std::uintptr_t>(kPctype));
                isdig = tab[c * 2] & 4;  // c is a signed char: same (possibly negative) index as the original
            }
            if (!isdig) break;
        }
    }
    char hold = *p;
    *p = decimal_point();
    char written;
    do {
        ++p;
        const char next = *p;
        *p = hold;
        written = hold;
        hold = next;
    } while (written);
}

// 0x007b8f24 int _positive(double* arg): 1 unless *arg < 0 or NaN (the original's fcomp with 0.0 / jb). No reader
// exists (_cfltcvt_tab[4]).
int JOAN_CDECL crt_positive(const void* arg) {
    double v;
    std::memcpy(&v, arg, 8);
    return v >= 0.0 ? 1 : 0;
}

// ======================================================================= _output (output.c)
// VC6 __lookuptable (0x009e72a0): low nibble = class of ' '..'x', high nibble = next state [class * 8 + state].
constexpr u8 kLookup[0x59] = {
    6,  0,  0,  6,  0,  1,  0,   0,   16,  0,  3,   6,   0,   6,  2,  16,  4,  69, 69, 69, 5,  5,  5,
    5,  5,  53, 48, 0,  80, 0,   0,   0,   0,  32,  40,  56,  80, 88, 7,   8,  0,  55, 48, 48, 87, 80,
    7,  0,  0,  32, 32, 8,  0,   0,   0,   0,  8,   96,  104, 96, 96, 96,  96, 0,  0,  112, 112, 120, 120,
    120, 120, 8,  7,  8,  0,  0,   7,   0,   8,  8,   8,   0,   0,  8,  0,   8,  0,  7,  8};

enum : u32 {
    FL_SIGN = 0x1,
    FL_SIGNSP = 0x2,
    FL_LEFT = 0x4,
    FL_LEADZERO = 0x8,
    FL_LONG = 0x10,
    FL_SHORT = 0x20,
    FL_SIGNED = 0x40,
    FL_ALTERNATE = 0x80,
    FL_NEGATIVE = 0x100,
    FL_FORCEOCTAL = 0x200,
    FL_WIDECHAR = 0x800,
    FL_I64 = 0x8000,
};

// 0x7bbd84 write_char(int ch, FILE* f, int* pnumwritten)
void write_char(int ch, Iobuf* f, int* pnum) {
    int r;
    f->cnt = static_cast<int>(static_cast<u32>(f->cnt) - 1u);
    if (f->cnt >= 0) {
        *f->ptr = static_cast<char>(ch);
        ++f->ptr;
        r = static_cast<u8>(ch);
    } else {
        r = cdecl_call<int>(kFlsbuf, ch, f);
    }
    if (r == -1)
        *pnum = -1;
    else
        *pnum = static_cast<int>(static_cast<u32>(*pnum) + 1u);
}
// 0x7bbdb9 write_multi_char(int ch, int num, FILE*, int*)
void write_multi_char(int ch, int num, Iobuf* f, int* pnum) {
    while (num-- > 0) {
        write_char(ch, f, pnum);
        if (*pnum == -1) break;
    }
}
// 0x7bbdea write_string(const char*, int len, FILE*, int*)
void write_string(const char* s, int len, Iobuf* f, int* pnum) {
    while (len-- > 0) {
        write_char(static_cast<int>(*s++), f, pnum);
        if (*pnum == -1) break;
    }
}

// 0x7bbe22 get_int_arg / 0x7bbe2f get_int64_arg / 0x7bbe3f get_short_arg. A va_list (char*) steps over one
// stack slot per vararg: 4 bytes on x86, 8 on x64 (every vararg, the low bytes hold an int / short / char).
[[maybe_unused]] constexpr int kArgSlot = static_cast<int>(sizeof(void*));
#if JOAN_SYSV_VARARGS
// System V x86-64 (lib/crt/varargs.hpp): *ap is the caller's va_list (decayed to a pointer), read with va_arg.
va_list& sysv_args(char** ap) { return *reinterpret_cast<va_list*>(*ap); }
u32 get_int_arg(char** ap) { return va_arg(sysv_args(ap), u32); }
void* get_ptr_arg(char** ap) { return va_arg(sysv_args(ap), void*); }
u64 get_int64_arg(char** ap) { return va_arg(sysv_args(ap), u64); }
u16 get_short_arg(char** ap) { return static_cast<u16>(va_arg(sysv_args(ap), u32)); }
double get_double_arg(char** ap) { return va_arg(sysv_args(ap), double); }
#else
u32 get_int_arg(char** ap) {
    u32 v;
    std::memcpy(&v, *ap, 4);
    *ap += kArgSlot;
    return v;
}
// A pointer vararg (%s / %Z / %n): pointer-sized (on x86 the same dword as get_int_arg).
void* get_ptr_arg(char** ap) {
    void* v;
    std::memcpy(&v, *ap, sizeof v);
    *ap += kArgSlot;
    return v;
}
u64 get_int64_arg(char** ap) {
    u64 v;
    std::memcpy(&v, *ap, 8);
    *ap += 8;
    return v;
}
u16 get_short_arg(char** ap) {
    u16 v;
    std::memcpy(&v, *ap, 2);
    *ap += kArgSlot;
    return v;
}
#endif

constexpr int kBufSize = 0x1200;       // the original has 0x200 (no bound check)
constexpr int kMaxFloatPrec = 0x1000;  // float precision clamp (original: stack overflow long before)
constexpr int kMaxIntPrec = 0x1000;    // integer precision clamp (likewise)

// 0x007bb643 int _output(FILE* stream, const char* format, va_list argptr)
int JOAN_CDECL crt_output(Iobuf* stream, const char* format, char* argptr) {
    c_output.hit();
    char buffer[kBufSize + 2];
    char* const int_end = buffer + kBufSize - 1;  // integer digits are written backwards from here
    char prefix[2] = {0, 0};
    char mbuf[8];
    int charsout = 0;
    int state = 0;
    u32 flags = 0;
    i32 precision = 0;
    i32 fldwidth = 0;
    i32 prefixlen = 0;
    i32 textlen = 0;  // also the radix of integer conversions ([ebp-0xc] in the original)
    int no_output = 0;
    int bufferiswide = 0;
    int capexp = 0;
    int hexadd = 0;
    char* text = buffer;
    const char* fmt = format;

    char ch = *fmt++;
    while (ch != 0 && charsout >= 0) {
        int cls = 0;
        if (ch >= 0x20 && ch <= 0x78) cls = kLookup[ch - 0x20] & 0xf;
        state = static_cast<i8>(kLookup[cls * 8 + state]) >> 4;
        switch (state) {
            case 0:  // ST_NORMAL
            normal_state:
                bufferiswide = 0;
                if (reinterpret_cast<const u8*>(gl<std::uintptr_t>(kPctype))[static_cast<u8>(ch) * 2 + 1] & 0x80) {
                    write_char(static_cast<int>(ch), stream, &charsout);
                    ch = *fmt++;
                }
                write_char(static_cast<int>(ch), stream, &charsout);
                break;
            case 1:  // ST_PERCENT
                precision = -1;
                capexp = 0;
                no_output = 0;
                fldwidth = 0;
                prefixlen = 0;
                flags = 0;
                bufferiswide = 0;
                break;
            case 2:  // ST_FLAG
                switch (ch) {
                    case ' ': flags |= FL_SIGNSP; break;
                    case '#': flags |= FL_ALTERNATE; break;
                    case '+': flags |= FL_SIGN; break;
                    case '-': flags |= FL_LEFT; break;
                    case '0': flags |= FL_LEADZERO; break;
                    default: break;
                }
                break;
            case 3:  // ST_WIDTH
                if (ch == '*') {
                    fldwidth = static_cast<i32>(get_int_arg(&argptr));
                    if (fldwidth < 0) {
                        flags |= FL_LEFT;
                        fldwidth = static_cast<i32>(0u - static_cast<u32>(fldwidth));
                    }
                } else {
                    fldwidth = static_cast<i32>(static_cast<u32>(fldwidth) * 10u + static_cast<u32>(ch) - 0x30u);
                }
                break;
            case 4:  // ST_DOT
                precision = 0;
                break;
            case 5:  // ST_PRECIS
                if (ch == '*') {
                    precision = static_cast<i32>(get_int_arg(&argptr));
                    if (precision < 0) precision = -1;
                } else {
                    precision = static_cast<i32>(static_cast<u32>(precision) * 10u + static_cast<u32>(ch) - 0x30u);
                }
                break;
            case 6:  // ST_SIZE
                switch (ch) {
                    case 'I':
                        if (fmt[0] == '6' && fmt[1] == '4') {
                            fmt += 2;
                            flags |= FL_I64;
                        } else {
                            state = 0;
                            goto normal_state;
                        }
                        break;
                    case 'h': flags |= FL_SHORT; break;
                    case 'l': flags |= FL_LONG; break;
                    case 'w': flags |= FL_WIDECHAR; break;
                    default: break;
                }
                break;
            case 7: {  // ST_TYPE
                int radix = 0;  // [ebp-0xc] in the original, shared with textlen (always set before use)
                switch (ch) {
                    case 'S':
                        if (!(flags & 0x830)) flags |= FL_WIDECHAR;
                        goto case_s;
                    case 's':
                    case_s: {
                        u32 i = precision == -1 ? 0x7fffffffu : static_cast<u32>(precision);
                        char* s = static_cast<char*>(get_ptr_arg(&argptr));
                        if (flags & (FL_LONG | FL_WIDECHAR)) {
                            if (s == nullptr) s = gl<char*>(kWNullString);
                            text = s;
                            bufferiswide = 1;
                            const char* q = s;
                            while (i-- != 0) {
                                u16 w;
                                std::memcpy(&w, q, 2);
                                if (w == 0) break;
                                q += 2;
                            }
                            textlen = static_cast<i32>(q - s) >> 1;
                        } else {
                            if (s == nullptr) s = gl<char*>(kNullString);
                            text = s;
                            const char* q = s;
                            while (i-- != 0 && *q) ++q;
                            textlen = static_cast<i32>(q - s);
                        }
                        goto output;
                    }
                    case 'C':
                        if (!(flags & 0x830)) flags |= FL_WIDECHAR;
                        goto case_c;
                    case 'c':
                    case_c:
                        if (flags & (FL_LONG | FL_WIDECHAR)) {
                            const u16 wc = get_short_arg(&argptr);
                            textlen = cdecl_call<int>(kWctomb, buffer, static_cast<u32>(wc));
                            if (textlen < 0) no_output = 1;
                        } else {
                            buffer[0] = static_cast<char>(get_int_arg(&argptr));
                            textlen = 1;
                        }
                        text = buffer;
                        goto output;
                    case 'Z': {
                        // an ANSI_STRING / UNICODE_STRING: u16 Length, u16 MaximumLength, char* Buffer
                        struct CountedString {
                            u16 length;
                            u16 max_length;
                            char* buffer;  // +4 on x86, +8 on x64
                        };
                        const u8* str = static_cast<const u8*>(get_ptr_arg(&argptr));
                        char* buf = nullptr;
                        if (str != nullptr) std::memcpy(&buf, str + offsetof(CountedString, buffer), sizeof buf);
                        if (str == nullptr || buf == nullptr) {
                            text = gl<char*>(kNullString);
                            textlen = static_cast<i32>(slen(text));
                        } else {
                            text = buf;
                            const i32 len = static_cast<i32>(static_cast<i16>(rd16(str)));
                            if (flags & FL_WIDECHAR) {
                                textlen = static_cast<i32>(static_cast<u32>(len) >> 1);
                                bufferiswide = 1;
                            } else {
                                bufferiswide = 0;
                                textlen = len;
                            }
                        }
                        goto output;
                    }
                    case 'E':
                    case 'G':
                        capexp = 1;
                        ch = static_cast<char>(ch + 0x20);
                        [[fallthrough]];
                    case 'e':
                    case 'f':
                    case 'g': {
                        flags |= FL_SIGNED;
                        text = buffer;
                        if (precision < 0)
                            precision = 6;
                        else if (precision == 0 && ch == 'g')
                            precision = 1;
                        u32 tmp[2];  // the double (8 bytes: two slots on x86, one on x64)
#if JOAN_SYSV_VARARGS
                        {
                            const double d = get_double_arg(&argptr);  // an XMM register, not a slot
                            std::memcpy(tmp, &d, sizeof tmp);
                        }
#else
                        if constexpr (kArgSlot == 4) {
                            tmp[0] = get_int_arg(&argptr);
                            tmp[1] = get_int_arg(&argptr);
                        } else {
                            const u64 d = get_int64_arg(&argptr);
                            std::memcpy(tmp, &d, sizeof tmp);
                        }
#endif
                        const int prec = precision > kMaxFloatPrec ? kMaxFloatPrec : precision;
                        cdecl_call<char*>(gl<std::uintptr_t>(kTabCfltcvt), static_cast<void*>(tmp), buffer,
                                          static_cast<int>(ch), prec, capexp);
                        const u32 alt = flags & FL_ALTERNATE;
                        if (alt && precision == 0) cdecl_call<void>(gl<std::uintptr_t>(kTabForcdecpt), buffer);
                        if (ch == 'g' && !alt) cdecl_call<void>(gl<std::uintptr_t>(kTabCropzeros), buffer);
                        if (buffer[0] == '-') {
                            flags |= FL_NEGATIVE;
                            text = buffer + 1;
                        }
                        textlen = static_cast<i32>(slen(text));
                        goto output;
                    }
                    case 'd':
                    case 'i':
                        flags |= FL_SIGNED;
                        radix = 10;
                        goto com_int;
                    case 'u':
                        radix = 10;
                        goto com_int;
                    case 'o':
                        radix = 8;
                        if (flags & FL_ALTERNATE) flags |= FL_FORCEOCTAL;
                        goto com_int;
                    case 'p':
                        precision = 2 * static_cast<i32>(sizeof(void*));  // 8 hex digits on x86
                        if constexpr (sizeof(void*) == 8) flags |= FL_I64;   // x64: the whole pointer
                        [[fallthrough]];
                    case 'X':
                        hexadd = 7;
                        goto com_hex;
                    case 'x':
                        hexadd = 0x27;
                    com_hex:
                        radix = 16;
                        if (flags & FL_ALTERNATE) {
                            prefix[0] = '0';
                            prefix[1] = static_cast<char>(hexadd + 0x51);
                            prefixlen = 2;
                        }
                        goto com_int;
                    case 'n': {
                        char* p = static_cast<char*>(get_ptr_arg(&argptr));
                        if (flags & FL_SHORT) {
                            const u16 v = static_cast<u16>(charsout);
                            std::memcpy(p, &v, 2);
                        } else {
                            std::memcpy(p, &charsout, 4);
                        }
                        no_output = 1;
                        goto next_char;
                    }
                    default:  // 'B': CH_TYPE without a case - output with the previous text / textlen
                        goto output;
                }
            com_int: {
                u64 number;
                if (flags & FL_I64) {
                    number = get_int64_arg(&argptr);
                } else if (flags & FL_SHORT) {
                    const u32 v = get_int_arg(&argptr);
                    if (flags & FL_SIGNED)
                        number = static_cast<u64>(static_cast<i64>(static_cast<i16>(v)));
                    else
                        number = static_cast<u16>(v);
                } else {
                    const u32 v = get_int_arg(&argptr);
                    if (flags & FL_SIGNED)
                        number = static_cast<u64>(static_cast<i64>(static_cast<i32>(v)));
                    else
                        number = v;
                }
                if ((flags & FL_SIGNED) && static_cast<i64>(number) < 0) {
                    number = 0 - number;
                    flags |= FL_NEGATIVE;
                }
                if (!(flags & FL_I64)) number &= 0xffffffffu;
                if (precision < 0)
                    precision = 1;
                else
                    flags &= ~static_cast<u32>(FL_LEADZERO);
                if (precision > kMaxIntPrec) precision = kMaxIntPrec;  // original: stack overflow long before
                if (number == 0) prefixlen = 0;
                char* t = int_end;
                const u64 r64 = static_cast<u64>(static_cast<i64>(radix));
                for (;;) {
                    const i32 pr = precision;
                    precision = static_cast<i32>(static_cast<u32>(precision) - 1u);
                    if (!(pr > 0) && number == 0) break;
                    i32 digit = static_cast<i32>(static_cast<u32>(number % r64)) + 0x30;
                    number /= r64;
                    if (digit > 0x39) digit += hexadd;
                    *t-- = static_cast<char>(digit);
                }
                textlen = static_cast<i32>(int_end - t);
                text = t + 1;
                if ((flags & FL_FORCEOCTAL) && (textlen == 0 || *text != '0')) {
                    *--text = '0';
                    ++textlen;
                }
                goto output;
            }
            output:
                if (!no_output) {
                    if (flags & FL_SIGNED) {
                        if (flags & FL_NEGATIVE) {
                            prefix[0] = '-';
                            prefixlen = 1;
                        } else if (flags & FL_SIGN) {
                            prefix[0] = '+';
                            prefixlen = 1;
                        } else if (flags & FL_SIGNSP) {
                            prefix[0] = ' ';
                            prefixlen = 1;
                        }
                    }
                    const i32 padding = static_cast<i32>(static_cast<u32>(fldwidth) - static_cast<u32>(prefixlen) -
                                                         static_cast<u32>(textlen));
                    if (!(flags & (FL_LEFT | FL_LEADZERO))) write_multi_char(' ', padding, stream, &charsout);
                    write_string(prefix, prefixlen, stream, &charsout);
                    if ((flags & FL_LEADZERO) && !(flags & FL_LEFT))
                        write_multi_char('0', padding, stream, &charsout);
                    if (bufferiswide && textlen > 0) {
                        const char* wp = text;
                        i32 left = textlen - 1;
                        for (;;) {
                            u16 wc;
                            std::memcpy(&wc, wp, 2);
                            wp += 2;
                            const int n = cdecl_call<int>(kWctomb, mbuf, static_cast<u32>(wc));
                            if (n <= 0) break;
                            write_string(mbuf, n, stream, &charsout);
                            const i32 was = left;
                            left = static_cast<i32>(static_cast<u32>(left) - 1u);
                            if (was == 0) break;
                        }
                    } else {
                        write_string(text, textlen, stream, &charsout);
                    }
                    if (flags & FL_LEFT) write_multi_char(' ', padding, stream, &charsout);
                }
                break;
            }
            default:
                break;
        }
    next_char:
        ch = *fmt++;
    }
    return charsout;}

// ======================================================================= string entry points
// 0x007b2952 int sprintf(char* buf, const char* fmt, ...): string FILE {_ptr = _base = buf, _cnt = INT_MAX,
// _flag = _IOWRT | _IOSTRG}; the terminating NUL goes through _flsbuf when _cnt is exhausted.
int finish_string(Iobuf* str, int r) {
    str->cnt = static_cast<int>(static_cast<u32>(str->cnt) - 1u);
    if (str->cnt < 0)
        cdecl_call<int>(kFlsbuf, 0, str);
    else
        *str->ptr = 0;
    return r;
}

int JOAN_CDECL crt_sprintf(char* buf, const char* fmt, ...) {
    Iobuf str{};
    str.base = buf;
    str.ptr = buf;
    str.flag = 0x42;
    str.cnt = 0x7fffffff;
    va_list ap;
    va_start(ap, fmt);
    // MSVC x86 va_list is a char* to the first stack vararg: the pointer the original passes (lea [ebp+0x10]).
    const int r = crt_output(&str, fmt, reinterpret_cast<char*>(ap));
    va_end(ap);
    return finish_string(&str, r);
}

// 0x007b77e3 int _snprintf(char* buf, size_t count, const char* fmt, ...): as sprintf with _cnt = count; a
// text that fills count bytes is not terminated and the result is -1 when it does not fit.
int JOAN_CDECL crt_snprintf(char* buf, std::uint32_t count, const char* fmt, ...) {
    Iobuf str{};
    str.base = buf;
    str.ptr = buf;
    str.flag = 0x42;
    str.cnt = static_cast<int>(count);
    va_list ap;
    va_start(ap, fmt);
    const int r = crt_output(&str, fmt, reinterpret_cast<char*>(ap));
    va_end(ap);
    return finish_string(&str, r);
}

// 0x007b7b37 int _vsnprintf(char* buf, size_t count, const char* fmt, va_list args)
int JOAN_CDECL crt_vsnprintf(char* buf, std::uint32_t count, const char* fmt, char* args) {
    Iobuf str{};
    str.base = buf;
    str.ptr = buf;
    str.flag = 0x42;
    str.cnt = static_cast<int>(count);
    const int r = crt_output(&str, fmt, args);
    return finish_string(&str, r);
}

#define R(name, addr, fn) JOAN_REPLACE("crt/printf", name, addr, fn)
R("crt::_output", 0x007bb643, &crt_output);
R("crt::sprintf", 0x007b2952, &crt_sprintf);
R("crt::_snprintf", 0x007b77e3, &crt_snprintf);
R("crt::_vsnprintf", 0x007b7b37, &crt_vsnprintf);
R("crt::_cfltcvt", 0x007b922c, &crt_cfltcvt);
R("crt::_cropzeros", 0x007b8ed6, &crt_cropzeros);
R("crt::_forcdecpt", 0x007b8e7c, &crt_forcdecpt);
R("crt::_positive", 0x007b8f24, &crt_positive);
R("crt::_fltout2", 0x007c26ae, &crt_fltout2);
R("crt::_fptostr", 0x007c2637, &crt_fptostr);
#undef R

}  // namespace

// Direct entry of _fltout2 for crt/mt's _fcvt (lib/crt/mt.cpp declares it; flt is a STRFLT).
namespace joan::crt {
void* JOAN_CDECL fltout2(std::uint32_t lo, std::uint32_t hi, void* flt, char* resultstr) {
    return ::crt_fltout2(lo, hi, static_cast<Strflt*>(flt), resultstr);
}
}  // namespace joan::crt

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::_output(void* a0, const char* a1, const char* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_output>(a0, a1, a2)); }  // 0x7bb643
auto joan::orig::libc::_fptostr(const char* a0, std::int32_t a1, void* a2) -> void { joan::bind::free<&::crt_fptostr>(a0, a1, a2); }  // 0x7c2637
