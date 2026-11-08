// Original library sources (re/attribution.csv original_file, under lib/): crt/intel/memcpy.asm, crt/sscanf.c,
//   crt/tran/i386/87ctrig.asm, crt/tran/i386/modf.asm, crt/vsprintf.c.
//
// Batch 113 / list 09: statically linked MSVC 6 C runtime, range 0x007b3e93..0x007b4e65.
//
// Identification of the roots (VC6 CRT sources; _lock 0x007b82aa / _unlock 0x007b830b,
// _lock_file 0x007bcde0 / _unlock_file 0x007bce32, _getptd 0x007b8c21; full table with the
// context functions and the globals each family touches: scratchpad w113_09/ids.md):
//   0x007b3e93 void srand(unsigned)                    STATEFUL rand (ptd->_holdrand, ptd+0x14)
//              (rand() follows at 0x007b3ea0, same family)
//   0x007b3ec2 int sscanf(const char*, const char*, ...)   string-FILE wrapper of _input  - replaced
//   0x007b3ef6 int fclose(FILE*)                       STATEFUL stdio
//   0x007b3f73 int fprintf(FILE*, const char*, ...)    STATEFUL stdio
//   0x007b3fe0 FILE* fopen(const char*, const char*)   STATEFUL stdio (_fsopen(.., _SH_DENYNO))
//   0x007b3ff3 int _chdir(const char*)                 STATEFUL env / errno
//   0x007b4079 char* _getcwd(char*, int)               STATEFUL env / errno / heap
//   0x007b41d5 int rename(const char*, const char*)    STATEFUL errno (MoveFileA + _dosmaperr)
//   0x007b4224 double tan(double)                      PURE x87 (+ errno/_matherr tail) - replaced
//   0x007b42c2 double modf(double, double*)            PURE x87 (+ errno/_matherr tail) - replaced
//   0x007b43e3 unsigned long _beginthread(fn, stack, arg)  STATEFUL thread (ptd, TLS)
//   0x007b44ed void _endthread(void)                   STATEFUL thread
//   0x007b466f struct tm* localtime(const time_t*)     STATEFUL time (_tzset state)
//   0x007b47fc void exit(int)                          STATEFUL exit
//   0x007b4910 void* memcpy(void*, const void*, size_t) PURE (VC6 memcpy.asm = overlap-safe) - replaced
//   0x007b4c45 _PNH _set_new_handler(_PNH)             STATEFUL heap (0x00a6b224)
//   0x007b4c8a int vsprintf(char*, const char*, va_list)   string-FILE wrapper of _output - replaced
//   0x007b4e1d void free(void*)                        STATEFUL heap (SBH + _crtheap)
//
// Module crt/b113_09 (all cdecl, plain `ret`):
//  * crt_tan 0x007b4224: fptan at 53-bit precision (the CRT's fldcw 0x027f), arguments >= 2^63 first reduced by
//    fprem1 with pi * 2^62 (tbyte 0x9e722a), computed in portable C++ (fp/pc53.hpp). With the game's control
//    word every call ends in _math_exit 0x7ba097 -> _87except(_INEXACT): the result is rounded through a double
//    and nothing else is written. Error paths (as measured on the original): +-inf -> default NaN
//    0xfff8000000000000, NaN -> the quieted NaN; both set errno = EDOM (33) through _errno (crt/mt).
//  * crt_modf 0x007b42c2: *ip = trunc(x), result x - trunc(x) (the original's frndint in chop mode at 64-bit
//    precision and fsub are exact, so a negative integer gives +0 as in the original); x = +-0: both +-0;
//    +-inf: *ip = x, result +-0; NaN: both the quieted NaN, errno = EDOM (_handle_qnan1 / _except1 with the
//    game's masked exceptions). The precision-exception report of the original fires only with an unmasked
//    precision exception (never: Direct3D's control word masks all exceptions).
//  * crt_memcpy 0x007b4910: VC6 memcpy.asm is memmove (backward byte copy iff dst > src and dst < src + n, else
//    rep movsd / movsb forward: for any overlap the bytes written equal memmove's); std::memmove. The original's
//    32-bit wrap of src + n cannot happen for a real buffer.
//  * crt_vsprintf 0x007b4c8a / crt_sscanf 0x007b3ec2: the stack _iobuf built exactly as the
//    original (vsprintf: _ptr = _base = buf, _cnt = 0x7fffffff, _flag = _IOWRT|_IOSTRG 0x42;
//    sscanf: _ptr = _base = s, _flag = _IOREAD|_IOSTRG|_IOMYBUF 0x49, _cnt = strlen(s) via the
//    original strlen 0x007b1ef0, called after the _ptr/_base stores), then the ORIGINAL _output
//    0x007bb643 / _input 0x007bc273 (locale-reading formatter / scanner, separate family). The
//    original leaves _file/_charbuf/_bufsiz/_tmpfname uninitialised; they are provably dead for
//    string streams: _input only hands the stream to _getc 0x7bcccf (-> _filbuf 0x7c0b59 returns
//    EOF on _IOSTRG before reading _file) and _ungetc_lk 0x7c30c8 (reads _ptr/_cnt/_base/_flag
//    only); _output only to write_char 0x7bbd84 (_flsbuf only when _cnt < 0, i.e. after 2 GB).
//    We zero them. vsprintf's tail: --_cnt, if negative _flsbuf(0, &str) 0x007bb52b, else
//    *_ptr = 0.
//
// Prologue note: none of these has an incremental-link thunk; all five are installed by the
// generic library-range 5-byte entry patch (re/check_lib_entries.py: all entries safe).
// Review (batch 113): sscanf / vsprintf take no stdio lock in the original either (no
// _lock_file / _lock_str call; _input / _output do not lock); _filbuf / _flsbuf return EOF on
// _IOSTRG before using _file (_flsbuf loads it into ebx, unused on that path).
#include "../../callconv.hpp"
#include <bit>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstring>

#include "../../fp/pc53.hpp"
#include "../../original/direct.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"

namespace {
using u32 = std::uint32_t;

JOAN_COUNTER(c_sscanf, "crt::sscanf");
JOAN_COUNTER(c_vsprintf, "crt::vsprintf");

using joan::call::cdecl_call;

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

using u64 = std::uint64_t;
using joan::fp::bits;
using joan::fp::from_bits;
constexpr u64 kExp = 0x7ff0000000000000ull, kMant = 0x000fffffffffffffull, kQuiet = 0x0008000000000000ull;
void set_errno(int v) { *static_cast<int*>(joan::orig::libc::_errno()) = v; }

// ------------------------------------------------------------------ 0x007b4224 tan
double JOAN_CDECL crt_tan(double x) {
    const u64 b = bits(x);
    if ((b & kExp) == kExp) {
        set_errno(33);  // EDOM
        return (b & kMant) == 0 ? from_bits(0xfff8000000000000ull) : from_bits(b | kQuiet);
    }
    return joan::fp::tan_pc53(x);
}

// ------------------------------------------------------------------ 0x007b42c2 modf
double JOAN_CDECL crt_modf(double x, double* ip) {
    const u64 b = bits(x);
    if ((b & kExp) == kExp) {
        if ((b & kMant) == 0) {  // +-inf
            *ip = x;
            return from_bits(b & 0x8000000000000000ull);
        }
        set_errno(33);  // EDOM
        const double q = from_bits(b | kQuiet);
        *ip = q;
        return q;
    }
    if (x == 0.0) {
        *ip = x;
        return x;
    }
    // trunc(x) and x - trunc(x), both exact (as the original's frndint and its subtraction at 64 bits), made from the
    // bit pattern with integer operations only. A floating-point subtraction would not do on 32-bit x86: the double
    // result returns in st0 and the compiler subtracts there, on the x87 at the precision in force (24 bits in the
    // game), which rounds the fraction.
    constexpr u64 kSign = 0x8000000000000000ull;
    const int e = static_cast<int>((b >> 52) & 0x7ff) - 1023;
    if (e < 0) {  // |x| < 1
        *ip = from_bits(b & kSign);
        return x;
    }
    if (e >= 52) {  // no fraction bits: x - x = +0
        *ip = x;
        return from_bits(0);
    }
    const int frac_bits = 52 - e;                                  // 1 .. 52
    *ip = from_bits(b & ~((u64{1} << frac_bits) - 1));             // the fraction bits cleared
    const u64 f = ((b & kMant) | (u64{1} << 52)) & ((u64{1} << frac_bits) - 1);
    if (f == 0) return from_bits(0);                               // an integer: x - x = +0
    const int top = static_cast<int>(std::bit_width(f)) - 1;       // f * 2^-frac_bits = 1.m * 2^(top - frac_bits)
    const u64 mant = (f << (52 - top)) & kMant;
    const u64 exp = static_cast<u64>(top - frac_bits + 1023);
    return from_bits((b & kSign) | (exp << 52) | mant);
}

// ------------------------------------------------------------------ 0x007b4910 memcpy
void* JOAN_CDECL crt_memcpy(void* dst, const void* src, u32 n) {
    std::memmove(dst, src, n);
    return dst;
}

// ------------------------------------------------------------------ 0x007b4c8a vsprintf
int JOAN_CDECL crt_vsprintf(char* buf, const char* fmt, char* args) {
    c_vsprintf.hit();
    Iobuf str{};
    str.ptr = buf;
    str.base = buf;
    str.flag = 0x42;
    str.cnt = 0x7fffffff;
    const int r = joan::orig::arg::as<int>(joan::orig::libc::_output(&str, joan::orig::arg::str(fmt), joan::orig::arg::str(args)));  // _output
    str.cnt = static_cast<int>(static_cast<u32>(str.cnt) - 1u);
    if (str.cnt < 0)
        joan::orig::libc::_flsbuf(0, &str);  // _flsbuf('\0', &str)
    else
        *str.ptr = 0;
    return r;
}

// ------------------------------------------------------------------ 0x007b3ec2 sscanf
int JOAN_CDECL crt_sscanf(const char* s, const char* fmt, ...) {
    c_sscanf.hit();
    Iobuf str{};
    str.flag = 0x49;
    str.base = const_cast<char*>(s);
    str.ptr = const_cast<char*>(s);
    str.cnt = joan::orig::arg::as<int>(std::strlen(joan::orig::arg::str(s)));  // strlen
    va_list ap;
    va_start(ap, fmt);
    // MSVC x86 va_list is a char* to the first stack vararg: the same pointer the original
    // passes (lea eax, [ebp+0x10]).
    const int r = joan::orig::arg::as<int>(joan::orig::libc::_input(&str, joan::orig::arg::mptr(fmt), joan::orig::arg::str(ap)));  // _input
    va_end(ap);
    return r;
}

#define B(name, addr, fn) JOAN_REPLACE("crt/b113_09", name, addr, fn)
B("crt::sscanf", 0x007b3ec2, &crt_sscanf);
B("crt::tan", 0x007b4224, &crt_tan);
B("crt::modf", 0x007b42c2, &crt_modf);
B("crt::memcpy", 0x007b4910, &crt_memcpy);
B("crt::vsprintf", 0x007b4c8a, &crt_vsprintf);
#undef B

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::tan(double a0) -> double { return joan::bind::conv<double>(joan::bind::free<&::crt_tan>(a0)); }  // 0x7b4224
auto joan::orig::libc::modf(double a0, void* a1) -> double { return joan::bind::conv<double>(joan::bind::free<&::crt_modf>(a0, a1)); }  // 0x7b42c2
auto joan::orig::libc::memcpy(void* a0, void* a1, std::uint32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_memcpy>(a0, a1, a2)); }  // 0x7b4910
auto joan::orig::libc::vsprintf(const char* a0, const char* a1, const char* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_vsprintf>(a0, a1, a2)); }  // 0x7b4c8a
