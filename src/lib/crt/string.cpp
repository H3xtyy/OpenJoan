// Original library sources (re/attribution.csv original_file, under lib/): crt/atof.c, crt/atox.c,
//   crt/intel/memset.asm, crt/intel/strcat.asm, crt/intel/strcmp.asm, crt/intel/strcspn.asm, crt/intel/strlen.asm,
//   crt/intel/strncpy.asm, crt/intel/strrchr.asm, crt/intel/strstr.asm, crt/tran/i386/87ctriga.asm,
//   crt/tran/i386/fabs.asm.
//
// Batch 113 / writer 06: statically linked libraries inside joan.exe (no thunks; entry-patched).
//
// (jcparam's jpeg_set_quality 0x007aa180 / jpeg_set_defaults 0x007aa1b0 and jdatadst's jpeg_stdio_dest 0x007aa9e0,
// module "lib/b113_06", moved to lib/jpeg/compress_api.cpp, typed, with the rest of the game's libjpeg.)
// Implemented here (module "crt/b113_06", MSVC 6 CRT):
//   0x007b15cb int atoi(const char*)            (atol 0x007b1540 inlined; reads the CRT's ctype
//                                                state __mb_cur_max 0x00a30ed0 / _pctype 0x00a30cc4
//                                                and calls _isctype 0x007b7f51 exactly as atol does)
//   0x007b1690 void* memset(void*, int, size_t)
//   0x007b16f0 int strcmp(const char*, const char*)            (-1 / 0 / +1, unsigned bytes)
//   0x007b1780 char* strstr(const char*, const char*)
//   0x007b1db0 char* strncpy(char*, const char*, size_t)       (dword algorithm kept: overlap-exact)
//   0x007b1eb0 size_t strcspn(const char*, const char*)
//   0x007b1ef0 size_t strlen(const char*)
//   0x007b1f6b double fabs(double)                             (bit operations; NaN: errno EDOM)
//   0x007b2020 char* strcpy(char*, const char*)                (dword algorithm kept: overlap-exact)
//   0x007b2030 char* strcat(char*, const char*)                (dword algorithm kept: overlap-exact)
//   0x007b2110 double atof(const char*)                        (_fltin 0x007b9c9d does the work)
//   0x007b2184 double atan(double)                             (fp/pc53.hpp; NaN: errno EDOM)
//   0x007b2210 char* strrchr(const char*, int)
// Not implemented (stateful, see scratchpad w113_06/ids.md): 0x007b1800 _strupr (locale),
// 0x007b1d2a operator delete (heap), 0x007b1d35 operator new (heap).
//
// Installation: none of these has an incremental-link thunk; patch_entry() writes a 5-byte jmp
// over every library entry (re/check_lib_entries.py: all entries here are safe), so ALL the
// registrations above are live.
// Register contract (review b113): the originals clobber eax/ecx/edx only (strlen/atoi happen to
// leave ecx = the argument, strcpy/strcat/memset keep edi via push/pop). Every one of the ~3900
// call sites (game, CRT, libjpeg, D3DX) was scanned: no caller reads ecx/edx after the call
// before writing it, and no hand-written asm caller depends on them, so the C++ cdecl versions
// (eax/ecx/edx clobbered, ebx/esi/edi/ebp kept, DF untouched) are drop-in.
//
// Calls to other library code go to the real addresses (libraries have no thunks).
#include "../../callconv.hpp"
#include "../../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "crt.hpp"
#include "../../fp/pc53.hpp"
#include "../../original/direct.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"

namespace {
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using joan::hook::global;

using joan::call::cdecl_call;
using joan::orig::helpers::at;
// ===================================================================== MSVC 6 CRT
// isspace / isdigit exactly as the CRT macros expand in atol / atof: __mb_cur_max > 1 ->
// _isctype(c, mask) else _pctype[c] & mask (low byte of the short entry).
int ctype_test(int c, int mask) {
    if (global<i32>(d::g_mb_cur_max) > 1) return joan::orig::arg::as<int>(joan::orig::libc::_isctype(joan::orig::arg::i32(c), joan::orig::arg::i32(mask)));
    return global<const u8*>(d::g_pctype)[c * 2] & mask;
}

// 0x007b15cb atoi -> atol 0x007b1540 (wrapping arithmetic as the lea sequence).
int JOAN_CDECL crt_atoi(const char* s) {
    const u8* p = reinterpret_cast<const u8*>(s);
    while (ctype_test(*p, 8)) ++p;  // _SPACE
    int c = *p++;
    const int sign = c;
    if (c == '-' || c == '+') c = *p++;
    u32 total = 0;
    while (ctype_test(c, 4)) {  // _DIGIT
        total = static_cast<u32>(c) + total * 10u - 0x30u;
        c = *p++;
    }
    if (sign == '-') total = 0u - total;
    return static_cast<int>(total);
}

// 0x007b1690 memset
void* JOAN_CDECL crt_memset(void* dst, int c, u32 n) {  // size_t of VC6: 32 bits (a slot call passes a dword)
    u8* d = static_cast<u8*>(dst);
    const u8 v = static_cast<u8>(c);
    for (u32 i = 0; i < n; ++i) d[i] = v;
    return dst;
}

// 0x007b16f0 strcmp: sbb/shl/inc -> -1 when the first differing byte is lower (unsigned), else 1.
int JOAN_CDECL crt_strcmp(const char* a, const char* b) {
    const u8* p = reinterpret_cast<const u8*>(a);
    const u8* q = reinterpret_cast<const u8*>(b);
    for (;;) {
        const u8 x = *p++;
        const u8 y = *q++;
        if (x != y) return x < y ? -1 : 1;
        if (x == 0) return 0;
    }
}

// 0x007b1780 strstr: first occurrence; empty needle -> haystack (a 1-char needle tail-jumps into
// strchr 0x007b34e6 in the original, same result).
char* JOAN_CDECL crt_strstr(const char* hay, const char* needle) {
    const u8* n = reinterpret_cast<const u8*>(needle);
    if (n[0] == 0) return const_cast<char*>(hay);
    for (const u8* h = reinterpret_cast<const u8*>(hay); *h; ++h) {
        if (*h != n[0]) continue;
        std::size_t i = 1;
        while (n[i] != 0 && h[i] == n[i]) ++i;
        if (n[i] == 0) return reinterpret_cast<char*>(const_cast<u8*>(h));
    }
    return nullptr;
}

u32 load32(const u8* p) {
    u32 v;
    std::memcpy(&v, p, 4);
    return v;
}
void store32(u8* p, u32 v) { std::memcpy(p, &v, 4); }
void store16(u8* p, u32 v) {
    const u16 w = static_cast<u16>(v);
    std::memcpy(p, &w, 2);
}
// The CRT's "may contain a zero byte" test (false positives for bytes >= 0x80 fall through).
bool maybe_zero(u32 w) { return (((w + 0x7efefeffu) ^ ~w) & 0x81010100u) != 0; }

// 0x007b2091: copy loop shared by strcpy / strcat (byte-align src, then dword reads / writes; the
// final dword is written with the exact width the original uses).
void copy_tail(u8* d, const u8* s) {
    while (reinterpret_cast<std::uintptr_t>(s) & 3) {
        const u8 c = *s++;
        if (c == 0) {
            *d = 0;
            return;
        }
        *d++ = c;
    }
    for (;;) {
        const u32 w = load32(s);
        s += 4;
        if (maybe_zero(w)) {
            if ((w & 0xff) == 0) {
                *d = 0;
                return;
            }
            if ((w & 0xff00) == 0) {
                store16(d, w);
                return;
            }
            if ((w & 0xff0000) == 0) {
                store16(d, w);
                d[2] = 0;
                return;
            }
            if ((w & 0xff000000u) == 0) {
                store32(d, w);
                return;
            }
        }
        store32(d, w);
        d += 4;
    }
}

// 0x007b1ef0 strlen (read-only; the first NUL).
std::size_t JOAN_CDECL crt_strlen(const char* s) {
    const char* p = s;
    while (*p) ++p;
    return static_cast<std::size_t>(p - s);
}

// 0x007b2020 strcpy
char* JOAN_CDECL crt_strcpy(char* dst, const char* src) {
    copy_tail(reinterpret_cast<u8*>(dst), reinterpret_cast<const u8*>(src));
    return dst;
}

// 0x007b2030 strcat: end of dst (first NUL), then the strcpy loop.
char* JOAN_CDECL crt_strcat(char* dst, const char* src) {
    u8* d = reinterpret_cast<u8*>(dst);
    while (*d) ++d;
    copy_tail(d, reinterpret_cast<const u8*>(src));
    return dst;
}

// 0x007b1db0 strncpy, the original's control flow (byte-align src, dword copy, dword / byte
// zero padding) so that overlapping buffers give the same bytes.
char* JOAN_CDECL crt_strncpy(char* dst, const char* src, u32 count) {  // size_t of VC6: 32 bits
    if (count == 0) return dst;
    u8* d = reinterpret_cast<u8*>(dst);
    const u8* s = reinterpret_cast<const u8*>(src);
    u32 ecx = count;
    u32 ebx = count;
    u32 w = 0;
    if ((reinterpret_cast<std::uintptr_t>(s) & 3) == 0) {
        ecx >>= 2;
        if (ecx != 0) goto dwords;
        goto tail_bytes;  // 0x7b1df5 with ebx = count (1..3)
    }
    for (;;) {  // 0x7b1dd4
        const u8 c = *s++;
        *d++ = c;
        if (--ecx == 0) return dst;
        if (c == 0) goto pad_align;
        if ((reinterpret_cast<std::uintptr_t>(s) & 3) == 0) break;
    }
    ebx = ecx;
    ecx >>= 2;
    if (ecx != 0) goto dwords;
tail_mod:  // 0x7b1df0
    ebx &= 3;
    if (ebx == 0) return dst;
tail_bytes:  // 0x7b1df5
    for (;;) {
        const u8 c = *s++;
        *d++ = c;
        if (c == 0) goto pad_bytes_dec;
        if (--ebx == 0) return dst;
    }
dwords:  // 0x7b1e41
    for (;;) {
        w = load32(s);
        s += 4;
        if (maybe_zero(w)) {
            u32 out = 0;
            bool nul = true;
            if ((w & 0xff) == 0) out = 0;
            else if ((w & 0xff00) == 0) out = w & 0xff;
            else if ((w & 0xff0000) == 0) out = w & 0xffff;
            else if ((w & 0xff000000u) == 0) out = w;
            else nul = false;  // false positive: 0x7b1e39
            if (nul) {
                store32(d, out);
                d += 4;
                if (--ecx != 0) {
                    do {  // 0x7b1e99
                        store32(d, 0);
                        d += 4;
                    } while (--ecx != 0);
                }
                ebx &= 3;  // 0x7b1ea1
                if (ebx == 0) return dst;
                goto pad_bytes;
            }
        }
        store32(d, w);  // 0x7b1e39
        d += 4;
        if (--ecx == 0) goto tail_mod;
    }
pad_align:  // 0x7b1e0a: zero bytes until dst is aligned, then dwords, then bytes
    while (reinterpret_cast<std::uintptr_t>(d) & 3) {
        *d++ = 0;
        if (--ecx == 0) return dst;
    }
    ebx = ecx;
    ecx >>= 2;
    if (ecx != 0) {
        do {  // 0x7b1e97
            store32(d, 0);
            d += 4;
        } while (--ecx != 0);
        ebx &= 3;
        if (ebx == 0) return dst;
    }
pad_bytes:  // 0x7b1e2b
    *d++ = 0;
pad_bytes_dec:  // 0x7b1e2e
    if (--ebx != 0) goto pad_bytes;
    return dst;
}

// 0x007b1eb0 strcspn: 256-bit set from `control` (NUL ends it), count until a member or NUL.
std::size_t JOAN_CDECL crt_strcspn(const char* str, const char* control) {
    u32 bits[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    for (const u8* c = reinterpret_cast<const u8*>(control); *c; ++c) bits[*c >> 5] |= 1u << (*c & 31);
    std::size_t n = 0;
    for (const u8* s = reinterpret_cast<const u8*>(str); *s; ++s, ++n)
        if (bits[*s >> 5] & (1u << (*s & 31))) break;
    return n;
}

// 0x007b2210 strrchr: last occurrence of (char)c, the terminator included.
char* JOAN_CDECL crt_strrchr(const char* s, int c) {
    const u8 ch = static_cast<u8>(c);
    const u8* p = reinterpret_cast<const u8*>(s);
    const u8* last = nullptr;
    for (;; ++p) {
        if (*p == ch) last = p;
        if (*p == 0) break;
    }
    return reinterpret_cast<char*>(const_cast<u8*>(last));
}

// 0x007b2110 atof: skip _SPACE, then _fltin(&flt, s, strlen(s), 0, 0) and return flt.dval (+0x10).
struct Flt {
    i32 flags;
    i32 nbytes;
    i32 lval;
    i32 pad;
    double dval;
};
static_assert(sizeof(Flt) == 0x18, "_flt");
double JOAN_CDECL crt_atof(const char* s) {
    const u8* p = reinterpret_cast<const u8*>(s);
    while (ctype_test(*p, 8)) ++p;
    Flt flt;  // filled by _fltin (flags, nbytes, dval always written)
    const char* q = reinterpret_cast<const char*>(p);
    // strlen(q), 0, 0 are pushed too, but _fltin reads only flt and str ([ebp+8], [ebp+0xc]).
    const Flt* r = joan::orig::arg::as<const Flt*>(joan::orig::libc::_fltin(&flt, q));
    return r->dval;
}

// ---- x87 math (fabs, atan): portable C++ with the original's results (proof: re/p1/crt).
// With the game's control word (all exceptions masked) the originals' special paths end as follows: a NaN
// argument (_handle_qnan1 / _except1 -> _set_errno) gives the quieted NaN and errno = EDOM (33); everything else
// leaves errno alone. errno is the CRT's (crt/mt _errno, per-thread data).
using u64 = std::uint64_t;
using joan::fp::bits;
using joan::fp::from_bits;
bool is_nan(u64 b) { return (b & 0x7ff0000000000000ull) == 0x7ff0000000000000ull && (b & 0x000fffffffffffffull) != 0; }
double nan_result(u64 b) {
    *static_cast<int*>(joan::orig::libc::_errno()) = 33;  // EDOM
    return from_bits(b | 0x0008000000000000ull);       // fld / fstp quiets an SNaN
}

// 0x007b1f6b fabs(double): the sign cleared (-inf -> +inf); NaN -> nan_result (sign kept).
double JOAN_CDECL crt_fabs(double x) {
    const u64 b = bits(x);
    if (is_nan(b)) return nan_result(b);
    return from_bits(b & 0x7fffffffffffffffull);
}

// 0x007b2184 atan(double): fpatan at 53-bit precision (fp/pc53.hpp); +-inf -> +-pi/2 (the CRT's tbyte
// 0x00a310ea through a double); NaN -> nan_result.
double JOAN_CDECL crt_atan(double x) {
    const u64 b = bits(x);
    if (is_nan(b)) return nan_result(b);
    if ((b & 0x7fffffffffffffffull) == 0x7ff0000000000000ull)
        return from_bits((b & 0x8000000000000000ull) | 0x3ff921fb54442d18ull);  // +-pi/2
    return joan::fp::atan_pc53(x);
}

#define CRT(name, addr, fn) JOAN_REPLACE("crt/b113_06", name, addr, fn)

CRT("crt::atoi", 0x007b15cb, &crt_atoi);
CRT("crt::memset", 0x007b1690, &crt_memset);
CRT("crt::strcmp", 0x007b16f0, &crt_strcmp);
CRT("crt::strstr", 0x007b1780, &crt_strstr);
CRT("crt::strncpy", 0x007b1db0, &crt_strncpy);
CRT("crt::strcspn", 0x007b1eb0, &crt_strcspn);
CRT("crt::strlen", 0x007b1ef0, &crt_strlen);
CRT("crt::fabs", 0x007b1f6b, &crt_fabs);
CRT("crt::strcpy", 0x007b2020, &crt_strcpy);
CRT("crt::strcat", 0x007b2030, &crt_strcat);
CRT("crt::atof", 0x007b2110, &crt_atof);
CRT("crt::atan", 0x007b2184, &crt_atan);
CRT("crt::strrchr", 0x007b2210, &crt_strrchr);

}  // namespace

// Direct entry points (crt.hpp).
namespace joan::crt {
int JOAN_CDECL atoi(const char* s) { return crt_atoi(s); }
}  // namespace joan::crt

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::strstr(const char* a0, const char* a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_strstr>(a0, a1)); }  // 0x7b1780
auto joan::orig::libc::strcspn(const char* a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::crt_strcspn>(a0, a1)); }  // 0x7b1eb0
auto joan::orig::libc::fabs(double a0) -> double { return joan::bind::conv<double>(joan::bind::free<&::crt_fabs>(a0)); }  // 0x7b1f6b
auto joan::orig::libc::atof(const char* a0) -> double { return joan::bind::conv<double>(joan::bind::free<&::crt_atof>(a0)); }  // 0x7b2110
auto joan::orig::libc::atan(double a0) -> double { return joan::bind::conv<double>(joan::bind::free<&::crt_atan>(a0)); }  // 0x7b2184
auto joan::orig::libc::strrchr(const char* a0, std::int32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_strrchr>(a0, a1)); }  // 0x7b2210
