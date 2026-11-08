// The 64-bit software x87 of joan::fp (overview: fp/x87.hpp), from the CRT maths engine (lib/crt/x87_math.cpp).
//
// X is an x87 register value: 64-bit significand, wide exponent (kNorm: |value| = m * 2^(e - 63), bit 63 of m set;
// the model never meets the extended range limits except through fscale, which saturates), with exact rounding to
// 24 / 53 / 64 bits (round to nearest even); fadd / fsub / fmul / fdiv / fsqrt, fprem / fprem1 / frndint / fscale
// exact; fld / fst qword; the x87 NaN rules. The transcendental instructions (fsin / fcos with the FPU's 66-bit pi,
// fpatan, fyl2x, f2xm1) are modelled as correctly rounded to 64 bits (double-double evaluation, ~2^-100, Dekker
// products of fp/exact.hpp). The real instructions are within one 64-bit ulp but not always correctly rounded
// (Intel Xeon of the harness: about 3 % of fsin, 16 % of fpatan results one 64-bit ulp off): re/p1/x87math/README.md.
//
// Floating point must be strict IEEE double (SSE2 on i686, never x87 extended precision, no FMA contraction: off
// below): the double-double kernels rely on exact error terms.
#pragma once

#include <bit>
#include <cmath>
#include <cstdint>

#include "bits.hpp"
#include "exact.hpp"
#include "precision.hpp"

#if defined(__clang__)
#pragma float_control(push)
#pragma clang fp contract(off)
#elif defined(_MSC_VER)
#pragma fp_contract(off)
#elif defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("fp-contract=off")
#endif

namespace joan::fp {

/// An unsigned 64-bit integer as a double, as the 32-bit x86 reference build converts it: GCC's i386 code loads it as
/// signed (fildll) and, when bit 63 is set, adds 2^64 with an x87 fadd - which rounds to the precision control of the
/// real x87, 24 bits once the renderer has set it as Direct3D does (gl/device.cpp, joan::cpu::x87_single_precision).
/// Values below 2^63 and every other precision convert exactly to nearest. x64 has no x87 in this path, so it repeats
/// that rounding from the emulated precision (fp/precision.hpp) - the model's results then match the x86 build's bit
/// for bit (found 2026-11-06: the ending screen's slide angle, cos 0x3f5e452e on x86 against 0x3f5e452f).
inline double u64_to_double(std::uint64_t v) {
#if defined(__i386__) || defined(_M_IX86)
    return static_cast<double>(v);
#else
    if ((v >> 63) == 0 || current_precision() != Precision::Single) return static_cast<double>(v);
    std::uint64_t r = v >> 40;  // 24 significant bits of a 64-bit value
    const std::uint64_t rem = v & ((std::uint64_t{1} << 40) - 1), half = std::uint64_t{1} << 39;
    if (rem > half || (rem == half && (r & 1) != 0)) ++r;  // round to nearest even (r may become 2^24: 2^64)
    return static_cast<double>(r) * 0x1p40;
#endif
}

// ================================================================================================ soft x87 values
// kNorm: |value| = m * 2^(e - 63) with bit 63 of m set (any exponent: the model never meets the extended range
// limits except through fscale, which saturates). kNaN: m = the 64-bit significand (bit 63 set, bit 62 = quiet).
enum Kind : std::uint8_t { kZero, kNorm, kInf, kNaN };
struct X {
    std::uint64_t m = 0;
    std::int32_t e = 0;
    bool neg = false;
    Kind k = kZero;
};
inline constexpr X x_zero(bool neg) { return {0, 0, neg, kZero}; }
inline constexpr X x_inf(bool neg) { return {0x8000000000000000u, 0, neg, kInf}; }
inline constexpr X x_const(std::uint64_t m, std::int32_t e) { return {m, e, false, kNorm}; }
// fldpi / fldl2e (each correctly rounded to 64 bits, as the x87 loads them with RC nearest)
inline constexpr X kPi64 = x_const(0xc90fdaa22168c235u, 1);
inline constexpr X kPi2_64 = x_const(0xc90fdaa22168c235u, 0);  // tbyte 0x00a310ea
inline constexpr X kL2e64 = x_const(0xb8aa3b295c17f0bcu, 0);

inline X x_norm(std::uint64_t m, std::int32_t e, bool neg) {  // m != 0: m * 2^(e - 63)
    const int z = std::countl_zero(m);
    return {m << z, e - z, neg, kNorm};
}

// Rounds |value| = (v.hi:v.lo + sticky fraction) * 2^(e - 127), bit 63 of v.hi set, to `nbits` significant bits
// (24, 53 or 64), round to nearest even.
inline X x_round(bool neg, std::int32_t e, U128 v, bool sticky, int nbits) {
    const int drop = 64 - nbits;
    std::uint64_t kept = drop ? v.hi >> drop : v.hi;
    bool up;
    if (drop) {
        const std::uint64_t rem = v.hi & ((std::uint64_t{1} << drop) - 1), half = std::uint64_t{1} << (drop - 1);
        up = rem > half || (rem == half && (v.lo != 0 || sticky || (kept & 1)));
    } else {
        const std::uint64_t half = std::uint64_t{1} << 63;
        up = v.lo > half || (v.lo == half && (sticky || (kept & 1)));
    }
    if (up) {
        ++kept;
        if (nbits == 64 ? kept == 0 : (kept >> nbits) != 0) {
            kept = std::uint64_t{1} << (nbits - 1);
            ++e;
        }
    }
    if (e > 16383) return x_inf(neg);  // overflow (masked, nearest)
    return {drop ? kept << drop : kept, e, neg, kNorm};
}
inline X x_round_to(X a, int nbits) {
    if (a.k != kNorm || nbits == 64) return a;
    return x_round(a.neg, a.e, {a.m, 0}, false, nbits);
}

// fld qword: exact. A signalling NaN is loaded as it is (the CRT's _fload builds the tbyte by hand); quiet() below
// is what an arithmetic operation does with it.
inline X x_from_double(double d) {
    const std::uint64_t b = std::bit_cast<std::uint64_t>(d);
    const bool neg = (b >> 63) != 0;
    const int ex = static_cast<int>((b >> 52) & 0x7ff);
    const std::uint64_t f = b & 0xfffffffffffffu;
    if (ex == 0x7ff) return f ? X{(f << 11) | 0x8000000000000000u, 0, neg, kNaN} : x_inf(neg);
    if (ex == 0) return f ? x_norm(f, -1074 + 63, neg) : x_zero(neg);
    return {((f | 0x10000000000000u) << 11), ex - 1023, neg, kNorm};
}
// _fload of the _trandisp functions (0x7ba3be): an infinity / NaN is built by hand with the low dword NOT shifted
// (mantissa = (hi << 11 | lo >> 21) : lo), which changes the payload of a NaN.
inline X x_from_double_td(double d) {
    X r = x_from_double(d);
    if (r.k == kNaN) {
        const std::uint64_t b = std::bit_cast<std::uint64_t>(d);
        const std::uint32_t hi = static_cast<std::uint32_t>(b >> 32), lo = static_cast<std::uint32_t>(b);
        r.m = (std::uint64_t{(hi << 11) | (lo >> 21)} << 32) | lo;
    }
    return r;
}
// fst qword with round to nearest: correctly rounded, gradual underflow, overflow to infinity; a NaN is stored
// quiet with its payload truncated.
inline double x_to_double(X a) {
    const std::uint64_t s = std::uint64_t{a.neg} << 63;
    switch (a.k) {
        case kZero: return std::bit_cast<double>(s);
        case kInf: return std::bit_cast<double>(s | 0x7ff0000000000000u);
        case kNaN: return std::bit_cast<double>(s | 0x7ff8000000000000u | ((a.m >> 11) & 0xfffffffffffffu));
        case kNorm: break;
    }
    if (a.e >= -1022) {
        const X r = x_round(a.neg, a.e, {a.m, 0}, false, 53);
        if (r.k == kInf || r.e > 1023) return std::bit_cast<double>(s | 0x7ff0000000000000u);
        return std::bit_cast<double>(s | (std::uint64_t(r.e + 1023) << 52) | ((r.m >> 11) & 0xfffffffffffffu));
    }
    // subnormal: value / 2^-1074 = m * 2^(e + 1011), rounded to an integer
    const int shift = -(a.e + 1011);  // > 11
    std::uint64_t q;
    bool up;
    if (shift >= 65) {
        q = 0;
        up = false;
    } else if (shift == 64) {
        q = 0;
        up = a.m > (std::uint64_t{1} << 63);
    } else {
        q = a.m >> shift;
        const std::uint64_t rem = a.m & ((std::uint64_t{1} << shift) - 1), half = std::uint64_t{1} << (shift - 1);
        up = rem > half || (rem == half && (q & 1));
    }
    if (up) ++q;
    return std::bit_cast<double>(s | q);  // q == 2^52 is the smallest normal, same bits
}
inline X x_quiet(X a) {
    if (a.k == kNaN) a.m |= 0x4000000000000000u;
    return a;
}
inline X x_neg(X a) {
    a.neg = !a.neg;
    return a;
}
inline X x_abs(X a) {
    a.neg = false;
    return a;
}

// x87 NaN propagation for fadd and friends (Intel SDM vol. 1, table 4-7): a NaN operand gives that NaN, quiet; of two
// NaNs the one with the larger significand (a QNaN wins over an SNaN); equal significands give the positive one
// (measured).
inline X x_nan_result(X dst, X src) {
    if (dst.k != kNaN) return x_quiet(src);
    if (src.k != kNaN) return x_quiet(dst);
    const bool qd = ((dst.m >> 62) & 1) != 0, qs = ((src.m >> 62) & 1) != 0;
    if (qd != qs) return x_quiet(qd ? dst : src);
    const std::uint64_t md = dst.m & 0x3fffffffffffffffu, ms = src.m & 0x3fffffffffffffffu;
    if (md == ms) {
        X r = x_quiet(dst);
        r.neg = dst.neg && src.neg;
        return r;
    }
    return x_quiet(ms > md ? src : dst);
}

// |a| >= |b| for two kNorm values
inline bool mag_ge(X a, X b) { return a.e != b.e ? a.e > b.e : a.m >= b.m; }

// a + b at `nbits` precision, round to nearest; finite operands.
inline X x_add(X a, X b, int nbits) {
    if (a.k == kZero && b.k == kZero) return x_zero(a.neg && b.neg);
    if (a.k == kZero) return x_round_to(b, nbits);
    if (b.k == kZero) return x_round_to(a, nbits);
    if (!mag_ge(a, b)) {
        const X t = a;
        a = b;
        b = t;
    }
    bool sticky = false;
    const U128 A{a.m, 0};
    const std::int64_t d = std::int64_t{a.e} - b.e;
    const U128 B = shr_sticky({b.m, 0}, d > 200 ? 200 : static_cast<int>(d), sticky);
    std::int32_t e = a.e;
    U128 S;
    if (a.neg == b.neg) {
        S = {A.hi + B.hi, A.lo + B.lo};
        if (S.lo < A.lo) ++S.hi;
        if (S.hi < A.hi || (S.hi == A.hi && B.hi != 0)) {  // carry out of bit 127
            S = shr_sticky(S, 1, sticky);
            S.hi |= std::uint64_t{1} << 63;
            ++e;
        }
    } else {
        S = sub(A, B);
        if (sticky) S = sub(S, {0, 1});
        if (S.hi == 0 && S.lo == 0 && !sticky) return x_zero(false);
        const int z = S.hi ? std::countl_zero(S.hi) : 64 + std::countl_zero(S.lo);
        S = shl(S, z);
        e -= z;
    }
    return x_round(a.neg, e, S, sticky, nbits);
}
inline X x_sub(X a, X b, int nbits) { return x_add(a, x_neg(b), nbits); }

inline X x_mul(X a, X b, int nbits) {
    const bool neg = a.neg != b.neg;
    if (a.k == kZero || b.k == kZero) return x_zero(neg);
    U128 p = mul64(a.m, b.m);
    std::int32_t e = a.e + b.e + 1;
    if (!(p.hi >> 63)) {
        p = shl(p, 1);
        --e;
    }
    return x_round(neg, e, p, false, nbits);
}

inline X x_div(X a, X b, int nbits) {  // b != 0, finite
    const bool neg = a.neg != b.neg;
    if (a.k == kZero) return x_zero(neg);
    std::uint64_t rem = a.m;
    const std::uint64_t d = b.m;
    std::int32_t e = a.e - b.e;
    bool carry = false;
    if (rem < d) {  // quotient in [0.5, 1): start one bit lower
        carry = (rem >> 63) != 0;
        rem <<= 1;
        --e;
    }
    std::uint64_t q = 0, guard = 0;
    for (int i = 0; i < 65; ++i) {
        std::uint64_t bit = 0;
        if (carry || rem >= d) {
            rem -= d;
            bit = 1;
        }
        if (i < 64) q = (q << 1) | bit;
        else guard = bit;
        carry = (rem >> 63) != 0;
        rem <<= 1;
    }
    return x_round(neg, e, {q, guard << 63}, rem != 0 || carry, nbits);
}

// fsqrt of a positive finite value
inline X x_sqrt(X a, int nbits) {
    if (a.k == kZero) return a;
    if (nbits <= 53 && (a.m & 0x7ff) == 0 && a.e >= -1022 && a.e <= 1023) {
        // a normal double: the correctly rounded double root, then (24 bits) the same side of every 24-bit rounding
        // boundary as the exact root unless it is one; then the sign of a - s^2 (exact) decides.
        const double d = x_to_double(a);
        const double sd = std::sqrt(d);
        const X r = x_from_double(sd);
        if (nbits == 53) return r;
        constexpr std::uint64_t kLow = (std::uint64_t{1} << 40) - 1, kHalf = std::uint64_t{1} << 39;
        if ((r.m & kLow) != kHalf) return x_round_to(r, 24);
        const DD sq = two_prod(sd, sd);
        const double t = d - sq.hi;  // exact (Sterbenz)
        if (t > sq.lo) return x_round(false, r.e, {r.m, 0}, true, 24);       // root above the midpoint
        if (t < sq.lo) return x_round(false, r.e, {r.m - 1, 0}, true, 24);   // below
        return x_round(false, r.e, {r.m, 0}, false, 24);                     // exactly the midpoint
    }
    const std::int32_t k = a.e >= 0 ? a.e / 2 : -((1 - a.e) / 2);  // floor(e / 2)
    const U128 rr = (a.e - 2 * k) ? U128{a.m, 0} : U128{a.m >> 1, a.m << 63};
    // digit by digit integer square root of rr (< 2^128): s < 2^64, rr = s^2 + rem
    std::uint64_t s = 0;
    U128 rem{0, 0};
    for (int i = 63; i >= 0; --i) {
        const std::uint64_t two = i >= 32 ? (rr.hi >> (2 * (i - 32))) & 3 : (rr.lo >> (2 * i)) & 3;
        rem = shl(rem, 2);
        rem.lo |= two;
        const U128 trial = shl({0, s}, 2);
        const U128 t1{trial.hi, trial.lo | 1};
        if (ge(rem, t1)) {
            rem = sub(rem, t1);
            s = (s << 1) | 1;
        } else {
            s <<= 1;
        }
    }
    if (nbits == 64) {  // nearest: up iff rem > s (no ties possible)
        const bool up = rem.hi != 0 || rem.lo > s;
        if (up) {
            ++s;
            if (s == 0) return {std::uint64_t{1} << 63, k + 1, false, kNorm};
        }
        return {s, k, false, kNorm};
    }
    return x_round(false, k, {s, 0}, rem.hi != 0 || rem.lo != 0, nbits);
}

// frndint, round to nearest even
inline X x_rint(X a) {
    if (a.k != kNorm || a.e >= 63) return a;
    if (a.e < -1) return x_zero(a.neg);
    const int shift = 63 - a.e;  // 1..64
    std::uint64_t kept, rem, half;
    if (shift == 64) {
        kept = 0;
        rem = a.m;
        half = std::uint64_t{1} << 63;
    } else {
        kept = a.m >> shift;
        rem = a.m & ((std::uint64_t{1} << shift) - 1);
        half = std::uint64_t{1} << (shift - 1);
    }
    if (rem > half || (rem == half && (kept & 1))) ++kept;
    if (kept == 0) return x_zero(a.neg);
    return x_norm(kept, 63, a.neg);
}
// fscale by an integer n (|n| small or saturating)
inline X x_scale(X a, std::int64_t n) {
    if (a.k != kNorm) return a;
    const std::int64_t e = a.e + n;
    if (e > 16383) return x_inf(a.neg);
    if (e < -16446) return x_zero(a.neg);
    a.e = static_cast<std::int32_t>(e);
    return a;
}
// integer value of an integral X with |a| < 2^63
inline std::int64_t x_to_int(X a) {
    if (a.k != kNorm) return 0;
    const std::uint64_t v = a.m >> (63 - a.e);
    return a.neg ? -static_cast<std::int64_t>(v) : static_cast<std::int64_t>(v);
}
// compare two ordered (non-NaN) values: -1, 0, 1
inline int x_cmp(X a, X b) {
    auto key = [](X v) -> int { return v.k == kZero ? 0 : v.neg ? -1 : 1; };
    const int sa = key(a), sb = key(b);
    if (sa != sb) return sa < sb ? -1 : 1;
    if (sa == 0) return 0;
    int mag;  // compare magnitudes
    if (a.k == kInf || b.k == kInf) mag = a.k == b.k ? 0 : a.k == kInf ? 1 : -1;
    else mag = a.e != b.e ? (a.e > b.e ? 1 : -1) : a.m != b.m ? (a.m > b.m ? 1 : -1) : 0;
    return sa > 0 ? mag : -mag;
}

// fprem (truncating quotient) / fprem1 (nearest-even quotient) to completion: the exact remainder.
inline X x_rem(X a, X b, bool nearest) {
    if (a.e < b.e) {
        if (!nearest || a.e < b.e - 1 || a.m <= b.m) return a;  // |a| < |b| / 2 or the tie (quotient 0, even)
        return x_norm(2 * (b.m - (a.m >> 1)) - (a.m & 1), a.e, !a.neg);  // |b| - |a| = (2 mb - ma) 2^(ea-63)
    }
    std::uint64_t r = a.m;
    bool qbit = false;
    if (r >= b.m) {
        r -= b.m;
        qbit = true;
    }
    for (std::int64_t i = 0, d = std::int64_t{a.e} - b.e; i < d; ++i) {
        const bool carry = (r >> 63) != 0;
        r <<= 1;
        qbit = carry || r >= b.m;
        if (qbit) r -= b.m;
    }
    bool neg = a.neg;
    if (nearest && (r > b.m - r || (r == b.m - r && qbit))) {
        r = b.m - r;
        neg = !neg;
    }
    if (r == 0) return x_zero(a.neg);
    return x_norm(r, b.e, neg);
}

// X (kNorm, moderate exponent) -> DD, exact
inline DD x_to_dd(X a) {
    const double s = a.neg ? -1.0 : 1.0;
    const double hi = u64_to_double(a.m & ~std::uint64_t{0x7ff}) * s;
    const double lo = static_cast<double>(a.m & 0x7ff) * s;
    const double sc = pow2(a.e - 63);
    return fast_two_sum(hi * sc, lo * sc);
}
// The exact value hi + lo (|lo| <= ulp(hi), hi != 0, normal range) rounded to `nbits` bits.
inline X dd_round(DD v, int nbits) {
    if (v.hi == 0.0) return x_zero(std::bit_cast<std::uint64_t>(v.hi) >> 63 != 0);
    const std::uint64_t bh = std::bit_cast<std::uint64_t>(v.hi);
    const bool neg = (bh >> 63) != 0;
    const int eh = static_cast<int>((bh >> 52) & 0x7ff) - 1023;  // hi = mh * 2^(eh - 52)
    const std::uint64_t mh = (bh & 0xfffffffffffffu) | 0x10000000000000u;
    U128 S{mh << 11, 0};  // value scale 2^(eh - 127): mh occupies bits 75..127
    bool sticky = false;
    if (v.lo != 0.0) {
        const std::uint64_t bl = std::bit_cast<std::uint64_t>(v.lo);
        const bool lneg = (bl >> 63) != 0;
        const int exl = static_cast<int>((bl >> 52) & 0x7ff);
        const std::uint64_t ml = exl ? (bl & 0xfffffffffffffu) | 0x10000000000000u : bl & 0xfffffffffffffu;
        const int el = (exl ? exl : 1) - 1075;    // lo = ml * 2^el
        const int pos = el - (eh - 127);           // bit position of ml's lsb in S
        U128 L{0, ml};
        if (pos >= 0) L = shl(L, pos);
        else L = shr_sticky(L, -pos, sticky);
        if (lneg == neg) {
            const std::uint64_t lo = S.lo + L.lo;
            S = {S.hi + L.hi + (lo < S.lo ? 1u : 0u), lo};
        } else {
            S = sub(S, L);
            if (sticky) S = sub(S, {0, 1});
        }
    }
    std::int32_t e = eh;
    if (!(S.hi >> 63)) {  // lo < 0 took the value below 2^eh: renormalise (|lo| <= ulp(hi): no carry out above)
        const int z = S.hi ? std::countl_zero(S.hi) : 64 + std::countl_zero(S.lo);
        S = shl(S, z);
        e -= z;
    }
    return x_round(neg, e, S, sticky, nbits);
}

// ---- constants (correctly rounded double-double; kPi, kPi2, kLn2, kInvLn2, kInvOdd, kAtan16: fp/exact.hpp)
inline constexpr DD kInvFact[32] = {
    {1.0, 0.0},
    {0x1.0000000000000p+0, 0.0},
    {0x1.0000000000000p-1, 0.0},
    {0x1.5555555555555p-3, 0x1.5555555555555p-57},
    {0x1.5555555555555p-5, 0x1.5555555555555p-59},
    {0x1.1111111111111p-7, 0x1.1111111111111p-63},
    {0x1.6c16c16c16c17p-10, -0x1.f49f49f49f49fp-65},
    {0x1.a01a01a01a01ap-13, 0x1.a01a01a01a01ap-73},
    {0x1.a01a01a01a01ap-16, 0x1.a01a01a01a01ap-76},
    {0x1.71de3a556c734p-19, -0x1.c154f8ddc6c00p-73},
    {0x1.27e4fb7789f5cp-22, 0x1.cbbc05b4fa99ap-76},
    {0x1.ae64567f544e4p-26, -0x1.c062e06d1f209p-80},
    {0x1.1eed8eff8d898p-29, -0x1.2aec959e14c06p-83},
    {0x1.6124613a86d09p-33, 0x1.f28e0cc748ebep-87},
    {0x1.93974a8c07c9dp-37, 0x1.05d6f8a2efd1fp-92},
    {0x1.ae7f3e733b81fp-41, 0x1.1d8656b0ee8cbp-97},
    {0x1.ae7f3e733b81fp-45, 0x1.1d8656b0ee8cbp-101},
    {0x1.952c77030ad4ap-49, 0x1.ac981465ddc6cp-103},
    {0x1.6827863b97d97p-53, 0x1.eec01221a8b0bp-107},
    {0x1.2f49b46814157p-57, 0x1.2650f61dbdcb4p-112},
    {0x1.e542ba4020225p-62, 0x1.ea72b4afe3c2fp-120},
    {0x1.71b8ef6dcf572p-66, -0x1.d043ae40c4647p-120},
    {0x1.0ce396db7f853p-70, -0x1.aebcdbd20331cp-124},
    {0x1.761b41316381ap-75, -0x1.3423c7d91404fp-130},
    {0x1.f2cf01972f578p-80, -0x1.9ada5fcc1ab14p-135},
    {0x1.3f3ccdd165fa9p-84, -0x1.58ddadf344487p-139},
    {0x1.88e85fc6a4e5ap-89, -0x1.71c37ebd16540p-143},
    {0x1.d1ab1c2dccea3p-94, 0x1.054d0c78aea14p-149},
    {0x1.0a18a2635085dp-98, 0x1.b9e2e28e1aa54p-153},
    {0x1.259f98b4358adp-103, 0x1.eaf8c39dd9bc5p-157},
    {0x1.3932c5047d60ep-108, 0x1.832b7b530a627p-162},
    {0x1.434d2e783f5bcp-113, 0x1.0b87b91be9affp-167},
};

// ---- kernels (relative error ~2^-100)
// sin r and cos r for |r| <= 0.8: Taylor series; the terms below 2^-53 relative in plain double.
inline DD dd_sin(DD r) {
    const DD s = dd_mul(r, r);
    double t = 0.0;
    for (int n = 29; n >= 17; n -= 2) t = t * s.hi + (n & 2 ? -kInvFact[n].hi : kInvFact[n].hi);
    DD p{t, 0.0};
    for (int n = 15; n >= 3; n -= 2) {
        const DD c = (n & 2) ? dd_neg(kInvFact[n]) : kInvFact[n];
        p = dd_add(dd_mul(p, s), c);
    }
    p = dd_mul(p, s);  // r^2 (-1/6 + ...)
    return dd_add(r, dd_mul(r, p));
}
inline DD dd_cos(DD r) {
    const DD s = dd_mul(r, r);
    double t = 0.0;
    for (int n = 30; n >= 18; n -= 2) t = t * s.hi + (n & 2 ? -kInvFact[n].hi : kInvFact[n].hi);
    DD p{t, 0.0};
    for (int n = 16; n >= 2; n -= 2) {
        const DD c = (n & 2) ? dd_neg(kInvFact[n]) : kInvFact[n];
        p = dd_add(dd_mul(p, s), c);
    }
    return dd_add({1.0, 0.0}, dd_mul(p, s));
}
// atan q for 0 <= q <= 1
inline DD dd_atan01(DD q) {
    const int j = static_cast<int>(q.hi * 8.0 + 0.5);
    const double c = j / 8.0;
    DD t = q;
    if (j) t = dd_div(dd_sub(q, {c, 0.0}), dd_add({1.0, 0.0}, dd_mul_d(q, c)));
    const DD s = dd_mul(t, t);
    double tail = 0.0;
    for (int k = 14; k >= 7; --k) tail = tail * s.hi + (k & 1 ? -kInvOdd[k].hi : kInvOdd[k].hi);
    DD p{tail, 0.0};
    for (int k = 6; k >= 1; --k) p = dd_add(dd_mul(p, s), (k & 1) ? dd_neg(kInvOdd[k]) : kInvOdd[k]);
    p = dd_mul(p, s);
    const DD a = dd_add(t, dd_mul(t, p));
    return j ? dd_add(kAtan16[2 * j], a) : a;
}
// ln m for m in [sqrt(1/2), sqrt(2)]: 2 atanh((m - 1) / (m + 1))
inline DD dd_log_m(DD m) {
    const DD s = dd_div(dd_sub(m, {1.0, 0.0}), dd_add(m, {1.0, 0.0}));
    const DD s2 = dd_mul(s, s);
    double tail = 0.0;
    for (int k = 23; k >= 11; --k) tail = tail * s2.hi + kInvOdd[k].hi;
    DD p{tail, 0.0};
    for (int k = 10; k >= 1; --k) p = dd_add(dd_mul(p, s2), kInvOdd[k]);
    p = dd_mul(p, s2);
    const DD a = dd_add(s, dd_mul(s, p));
    return {2.0 * a.hi, 2.0 * a.lo};
}
// e^z - 1 for |z| <= 0.7
inline DD dd_expm1(DD z) {
    double tail = 0.0;
    for (int n = 27; n >= 14; --n) tail = tail * z.hi + kInvFact[n].hi;
    DD p{tail, 0.0};
    for (int n = 13; n >= 2; --n) p = dd_add(dd_mul(p, z), kInvFact[n]);
    p = dd_mul(p, z);
    return dd_add(z, dd_mul(z, p));
}

// ===================================================================== x87 transcendental instructions (the model)
// Argument reduction of fsin / fcos / fptan for |x| < 2^63: r = x - k pi66/2 exactly (pi66/2 = kP66 * 2^-65),
// r as a double-double, k mod 4.
inline constexpr std::uint64_t kP66hi = 0x3, kP66lo = 0x243f6a8885a308d3u;  // 0x3243f6a8885a308d3 (66 bits)
struct Reduced {
    DD r;
    int quadrant;
};
// a * kP66 for a < 2^64: 130-bit product as (top, mid, low)
inline void mul_p66(std::uint64_t a, std::uint64_t& w2, std::uint64_t& w1, std::uint64_t& w0) {
    const U128 p = mul64(a, kP66lo);
    const U128 q = mul64(a, kP66hi);  // < 2^66
    w0 = p.lo;
    w1 = p.hi + q.lo;
    w2 = q.hi + (w1 < p.hi ? 1u : 0u);
}
inline Reduced reduce_pi66(X x) {  // kNorm, |x| < 2^63
    if (x.e < -1 || (x.e == -1 && x.m < 0xc90fdaa22168c234u)) return {x_to_dd(x), 0};  // |x| < pi66/4: k = 0
    // X_int = |x| * 2^65 = m * 2^(e + 2) as two words (e + 2 in 1..64)
    const int sh = x.e + 2;
    const std::uint64_t x2 = sh == 64 ? x.m : x.m >> (64 - sh), x1 = sh == 64 ? 0 : x.m << sh;
    // k = round(X_int / P): estimate with doubles, then correct from the remainder
    const double xd = u64_to_double(x.m) * pow2(x.e - 63);
    std::uint64_t k = static_cast<std::uint64_t>(xd * 0.63661977236758134 + 0.5);
    constexpr double kP = 0x3243f6a8885a308d3p0 / 1.0;
    for (;;) {
        std::uint64_t w2, w1, w0;
        mul_p66(k, w2, w1, w0);
        // d = X_int - k P (192-bit two's complement; X_int = x2 2^64 + x1)
        const std::uint64_t d0 = x1 - w0;
        const std::uint64_t b0 = x1 < w0 ? 1u : 0u;
        const std::uint64_t d1 = x2 - w1 - b0;
        const std::uint64_t b1 = (x2 < w1 || (x2 == w1 && b0)) ? 1u : 0u;
        const std::uint64_t d2 = 0 - w2 - b1;
        const bool neg = (d2 >> 63) != 0;
        // |d| as 192 bits
        std::uint64_t a2 = d2, a1 = d1, a0 = d0;
        if (neg) {
            a0 = ~a0 + 1;
            a1 = ~a1 + (a0 == 0 ? 1u : 0u);
            a2 = ~a2 + ((a0 == 0 && a1 == 0) ? 1u : 0u);
        }
        // |d| <= P / 2 ? (P / 2 = 0x1921fb54442d18469.8, 65 bits)
        const bool small = a2 == 0 && (a1 < 1 || (a1 == 1 && a0 <= 0x921fb54442d18469u));
        if (!small) {
            const double dd = (u64_to_double(a2) * 0x1p128 + u64_to_double(a1) * 0x1p64 + u64_to_double(a0)) / kP;
            const std::uint64_t step = dd < 1.0 ? 1u : static_cast<std::uint64_t>(dd + 0.5);
            k = neg ? k - step : k + step;
            continue;
        }
        // r = d * 2^-65 (|d| < 2^66) = a1 2^-1 + (a0 >> 32) 2^-33 + (a0 & 0xffffffff) 2^-65: the first two add
        // exactly (34 bits), two_sum adds the third without error
        const double head = static_cast<double>(a1) * 0.5 + static_cast<double>(a0 >> 32) * pow2(-33);
        DD r = two_sum(head, static_cast<double>(a0 & 0xffffffffu) * pow2(-65));
        if (neg != x.neg) r = dd_neg(r);
        const int q = static_cast<int>(k & 3);
        return {r, x.neg ? (4 - q) & 3 : q};
    }
}
// fsin / fcos of a kNorm |x| < 2^63, correctly rounded to 64 bits.
inline X x_trig(X x, bool want_cos) {
    if (x.e < -40) return want_cos ? x_const(std::uint64_t{1} << 63, 0) : x;  // tiny: sin x rounds to x, cos x to 1
    const Reduced red = reduce_pi66(x);
    const int k = (red.quadrant + (want_cos ? 1 : 0)) & 3;  // cos x = sin(x + pi/2)
    DD v = (k & 1) ? dd_cos(red.r) : dd_sin(red.r);
    if (k & 2) v = dd_neg(v);
    if (v.hi == 0.0) return x_zero(x.neg);
    return dd_round(v, 64);
}
// fpatan: atan(y / x) with the quadrant of (x, y), finite operands not both zero; correctly rounded to 64 bits.
inline X x_patan(X y, X x) {
    const bool yneg = y.neg, xneg = x.neg;
    DD a;
    if (y.k == kZero) {
        a = {0.0, 0.0};
    } else if (x.k == kZero) {
        a = kPi2;
    } else {
        const X ay = x_abs(y), ax = x_abs(x);
        const bool swap = !mag_ge(ax, ay);  // |y| > |x|: pi/2 - atan(|x| / |y|)
        const X num = swap ? ax : ay, den = swap ? ay : ax;
        const std::int64_t de = std::int64_t{num.e} - den.e;  // <= 0
        if (de < -900) {
            // ratio q below 2^-900: atan q = q, pi - q, pi/2 -+ q all round to 64 bits as q, pi, pi/2 do
            X r = swap ? kPi2_64 : xneg ? kPi64 : x_div(num, den, 64);
            r.neg = yneg;
            return r;
        }
        X n = num, d = den;
        n.e = static_cast<std::int32_t>(de);
        d.e = 0;
        a = dd_atan01(dd_div(x_to_dd(n), x_to_dd(d)));
        if (swap) a = dd_sub(kPi2, a);
    }
    if (xneg) a = dd_sub(kPi, a);
    if (a.hi == 0.0) return x_zero(yneg);
    X r = dd_round(a, 64);
    r.neg = yneg;
    return r;
}
// fyl2x: y * log2(x), x > 0 finite, y finite; correctly rounded to 64 bits.
inline X x_yl2x(X y, X x) {
    // x = m 2^e with m in [sqrt(1/2), sqrt(2))
    X mx = x;
    std::int32_t e = x.e;
    mx.e = 0;
    if (mx.m > 0xb504f333f9de6484u) {  // m > sqrt(2)
        mx.e = -1;
        ++e;
    }
    const DD lm = dd_mul(dd_log_m(x_to_dd(mx)), kInvLn2);
    const DD l = dd_add({static_cast<double>(e), 0.0}, lm);
    if (l.hi == 0.0 || y.k == kZero) return x_zero(y.neg != (l.hi < 0.0));
    if (y.e > 900 || y.e < -900) {  // keep the double-double in range: scale y (exact power of two)
        X ys = y;
        ys.e = 0;
        X r = dd_round(dd_mul(l, x_to_dd(ys)), 64);
        return x_scale(r, y.e);
    }
    return dd_round(dd_mul(l, x_to_dd(y)), 64);
}
// f2xm1: 2^f - 1 for |f| <= 1; correctly rounded to 64 bits.
inline X x_f2xm1(X f) {
    if (f.k == kZero) return f;
    if (f.e < -80) {  // 2^f - 1 = f ln2 (1 + f ln2 / 2 + ...): f ln2 to far beyond 64 bits; scaled into range
        X g = f;
        g.e = 0;
        return x_scale(dd_round(dd_mul(x_to_dd(g), kLn2), 64), f.e);
    }
    const DD z = dd_mul(x_to_dd(f), kLn2);
    return dd_round(dd_expm1(z), 64);
}

// ------------------------------------------------------------------------------- raw instructions on a register
// (the CRT functions above check their operands first; D3DX code runs the bare instructions)
/// The x87 default NaN (indefinite) of an invalid operation.
inline constexpr X kIndefinite{0xc000000000000000u, 0, true, kNaN};
/// fsin of a register: 64-bit result (precision control does not apply); |x| >= 2^63 leaves the operand unchanged
/// (C2 set), an infinity is invalid (the default NaN), a NaN or a zero stays.
inline X x_fsin(X a) {
    if (a.k == kInf) return kIndefinite;
    if (a.k != kNorm || a.e >= 63) return a;
    return x_trig(a, false);
}
/// fmul of two registers rounded to `nbits` (24 under the game's precision control), the x87 NaN / infinity rules.
inline X x_fmul(X dst, X src, int nbits) {
    if (dst.k == kNaN || src.k == kNaN) return x_nan_result(dst, src);
    if (dst.k == kInf || src.k == kInf) {
        if (dst.k == kZero || src.k == kZero) return kIndefinite;  // 0 * inf: invalid
        return x_inf(dst.neg != src.neg);
    }
    return x_mul(dst, src, nbits);
}

}  // namespace joan::fp

#if defined(__clang__)
#pragma float_control(pop)
#elif defined(__GNUC__) && !defined(_MSC_VER)
#pragma GCC pop_options
#endif
