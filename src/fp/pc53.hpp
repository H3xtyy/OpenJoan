// The CRT transcendental results of joan::fp (overview: fp/x87.hpp): atan / tan / log of joan.exe's MSVC 6 CRT, used by
// lib/crt/string.cpp (atan), string_format.cpp (tan) and xtoa_misc.cpp (log). No inline asm, no x87, no libm.
//
// What the originals compute (measured in the i386 harness re/p1/crt on the original code, models checked with
// mpmath at 300+ bits): the CRT switches the x87 to its own control word 0x027f (53-bit precision, nearest) and runs
// fpatan / fyl2x / fptan; the result is then stored as a double. The hardware result behaves as the exact function
// value rounded to a 64-bit significand and then to 53 bits (double rounding RN53(RN64(f))); fyl2x's f is
// log2(x) times the fldln2 constant (ln 2 rounded to 64 bits), not ln(x). fptan reduces its
// argument with pi rounded to 66 bits (P66): tan(x) = tan(x - k * P66 / 2) (-cot for odd k), k = nearest integer;
// arguments >= 2^63 are first reduced by the CRT with fprem1 by pi * 2^62 rounded to 64 bits (an integer).
// Here: the function value is evaluated in double-double arithmetic (~104 bits, error far below the 2^-64 grid; the
// fma product family of fp/exact.hpp), the reductions in exact integer arithmetic, then rounded RN64 then RN53.
// Against the CPU's x87 (Intel Xeon of the harness) the remaining differences are the x87's own approximation
// errors at the 64-bit grid: re/p1/crt/README.
//
// Requirements: IEEE double arithmetic in round-to-nearest without excess precision (SSE2: /arch:SSE2 or x64).
#pragma once

#include "fma.hpp"
#include <cmath>
#include <cstdint>

#include "bits.hpp"
#include "exact.hpp"

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

// The fma product family (fp/exact.hpp has the Dekker one).
inline DD two_prod_fma(double a, double b) {
    const double p = a * b;
    return {p, joan::fp::fma_exact(a, b, -p)};
}
inline DD dd_mul_fma(DD a, DD b) {
    DD p = two_prod_fma(a.hi, b.hi);
    p.lo += a.hi * b.lo + a.lo * b.hi;
    return fast_two_sum(p.hi, p.lo);
}
inline DD dd_mul_d_fma(DD a, double b) {
    DD p = two_prod_fma(a.hi, b);
    p.lo += a.lo * b;
    return fast_two_sum(p.hi, p.lo);
}
inline DD dd_div_fma(DD a, DD b) {  // b normal
    const double q1 = a.hi / b.hi;
    DD r = dd_sub(a, dd_mul_d_fma(b, q1));
    const double q2 = r.hi / b.hi;
    r = dd_sub(r, dd_mul_d_fma(b, q2));
    const double q3 = r.hi / b.hi;
    return dd_add_d(fast_two_sum(q1, q2), q3);
}

inline double fabs_exact(double d) { return from_bits(bits(d) & 0x7fffffffffffffffull); }

// Horner: c[n-1] z^(n-1) + ... + c[0].
inline DD horner_fma(const DD* c, int n, DD z) {
    DD p = c[n - 1];
    for (int k = n - 2; k >= 0; --k) p = dd_add(dd_mul_fma(p, z), c[k]);
    return p;
}

// Nearest integer of |t| < 2^51, ties to even (needs round-to-nearest).
inline double rint_even(double t) {
    const double c = 6755399441055744.0;  // 1.5 * 2^52
    const double r = t + c;
    return r - c;
}

// The x87 result at 53-bit precision of the value hi + lo (normal, non-zero hi): rounded to a 64-bit significand,
// then to 53 bits (ties to even both times).
inline double round_pc53(DD v) {
    const double hi = v.hi, lo = v.lo;
    if (lo == 0.0) return hi;
    const std::uint64_t b = bits(hi);
    int e = static_cast<int>((b >> 52) & 0x7ff) - 1023;  // binade of hi + lo
    const bool power = (b & 0x000fffffffffffffull) == 0;
    const bool below = power && ((lo < 0.0) != (hi < 0.0));
    if (below) e -= 1;
    // lo in units of the 64-bit grid 2^(e-63), rounded: |n| <= 1024 (half of the 53-bit grid 2^(e-52)).
    const double n = rint_even(lo * pow2(63 - e));
    if (fabs_exact(n) < 1024.0) return hi;
    // Exactly halfway between two doubles after the first rounding: ties to even.
    if (below || (b & 1u) == 0) return hi;
    const double step = pow2(e - 52);
    return n > 0.0 ? hi + step : hi - step;
}

// ---- constants (mpmath, 400 bits)
inline constexpr double kLn2Rel = 0x1.384291b5ee917p-66;  // RN64(ln 2) / ln 2 - 1
inline constexpr DD kAtanCoef[12] = {  // (-1)^k / (2k + 1)
    {0x1.0000000000000p+0, 0.0},
    {-0x1.5555555555555p-2, -0x1.5555555555555p-56},
    {0x1.999999999999ap-3, -0x1.999999999999ap-57},
    {-0x1.2492492492492p-3, -0x1.2492492492492p-57},
    {0x1.c71c71c71c71cp-4, 0x1.c71c71c71c71cp-58},
    {-0x1.745d1745d1746p-4, 0x1.745d1745d1746p-59},
    {0x1.3b13b13b13b14p-4, -0x1.3b13b13b13b14p-58},
    {-0x1.1111111111111p-4, -0x1.1111111111111p-60},
    {0x1.e1e1e1e1e1e1ep-5, 0x1.e1e1e1e1e1e1ep-61},
    {-0x1.af286bca1af28p-5, -0x1.af286bca1af28p-59},
    {0x1.8618618618618p-5, 0x1.8618618618618p-59},
    {-0x1.642c8590b2164p-5, -0x1.642c8590b2164p-60},
};
inline constexpr DD kSinCoef[15] = {  // (-1)^k / (2k + 1)!
    {0x1.0000000000000p+0, 0.0},
    {-0x1.5555555555555p-3, -0x1.5555555555555p-57},
    {0x1.1111111111111p-7, 0x1.1111111111111p-63},
    {-0x1.a01a01a01a01ap-13, -0x1.a01a01a01a01ap-73},
    {0x1.71de3a556c734p-19, -0x1.c154f8ddc6c00p-73},
    {-0x1.ae64567f544e4p-26, 0x1.c062e06d1f209p-80},
    {0x1.6124613a86d09p-33, 0x1.f28e0cc748ebep-87},
    {-0x1.ae7f3e733b81fp-41, -0x1.1d8656b0ee8cbp-97},
    {0x1.952c77030ad4ap-49, 0x1.ac981465ddc6cp-103},
    {-0x1.2f49b46814157p-57, -0x1.2650f61dbdcb4p-112},
    {0x1.71b8ef6dcf572p-66, -0x1.d043ae40c4647p-120},
    {-0x1.761b41316381ap-75, 0x1.3423c7d91404fp-130},
    {0x1.3f3ccdd165fa9p-84, -0x1.58ddadf344487p-139},
    {-0x1.d1ab1c2dccea3p-94, -0x1.054d0c78aea14p-149},
    {0x1.259f98b4358adp-103, 0x1.eaf8c39dd9bc5p-157},
};
inline constexpr DD kCosCoef[16] = {  // (-1)^k / (2k)!
    {0x1.0000000000000p+0, 0.0},
    {-0x1.0000000000000p-1, 0.0},
    {0x1.5555555555555p-5, 0x1.5555555555555p-59},
    {-0x1.6c16c16c16c17p-10, 0x1.f49f49f49f49fp-65},
    {0x1.a01a01a01a01ap-16, 0x1.a01a01a01a01ap-76},
    {-0x1.27e4fb7789f5cp-22, -0x1.cbbc05b4fa99ap-76},
    {0x1.1eed8eff8d898p-29, -0x1.2aec959e14c06p-83},
    {-0x1.93974a8c07c9dp-37, -0x1.05d6f8a2efd1fp-92},
    {0x1.ae7f3e733b81fp-45, 0x1.1d8656b0ee8cbp-101},
    {-0x1.6827863b97d97p-53, -0x1.eec01221a8b0bp-107},
    {0x1.e542ba4020225p-62, 0x1.ea72b4afe3c2fp-120},
    {-0x1.0ce396db7f853p-70, 0x1.aebcdbd20331cp-124},
    {0x1.f2cf01972f578p-80, -0x1.9ada5fcc1ab14p-135},
    {-0x1.88e85fc6a4e5ap-89, 0x1.71c37ebd16540p-143},
    {0x1.0a18a2635085dp-98, 0x1.b9e2e28e1aa54p-153},
    {-0x1.3932c5047d60ep-108, -0x1.832b7b530a627p-162},
};

// ---- atan: fld1; fpatan at 53-bit precision. x finite.
inline DD atan_unit(DD t) {  // 0 <= t <= 1
    const int j = static_cast<int>(t.hi * 16.0 + 0.5);
    DD u = t;
    if (j != 0) {
        const double c = j / 16.0;
        u = dd_div_fma(dd_add_d(t, -c), dd_add_d(dd_mul_d_fma(t, c), 1.0));  // atan(t) = atan(c) + atan((t - c) / (1 + t c))
    }
    const DD p = horner_fma(kAtanCoef, 12, dd_mul_fma(u, u));  // |u| <= 1/32
    return dd_add(dd_mul_fma(p, u), kAtan16[j]);
}
inline double atan_pc53(double x) {
    const double ax = fabs_exact(x);
    if (ax < 0x1p-28) return x;  // atan(x) = x(1 - x^2/3 ...) rounds to x (zeros and denormals included)
    DD r;
    if (ax > 1.0) {
        const DD inv = ax > 0x1p1000 ? DD{1.0 / ax, 0.0} : dd_div_fma(DD{1.0, 0.0}, DD{ax, 0.0});  // no underflow in div
        r = dd_sub(kPi2, atan_unit(inv));
    } else {
        r = atan_unit(DD{ax, 0.0});
    }
    const double y = round_pc53(r);
    return x < 0.0 ? -y : y;
}

// ---- log: fldln2; fxch; fyl2x at 53-bit precision. x > 0 finite.
inline double log_pc53(double x) {
    std::uint64_t b = bits(x);
    int e = static_cast<int>(b >> 52);
    if (e == 0) {  // denormal: scale by 2^54
        b = bits(x * 0x1p54);
        e = static_cast<int>(b >> 52) - 54;
    }
    e -= 1023;
    double m = from_bits((b & 0x000fffffffffffffull) | 0x3ff0000000000000ull);  // [1, 2)
    if (m > 1.4142135623730951) {
        m = m * 0.5;
        e += 1;
    }
    // log(m) = 2 atanh(s), s = (m - 1) / (m + 1), |s| <= 0.1716
    const DD s = dd_div_fma(DD{m - 1.0, 0.0}, two_sum(m, 1.0));
    const DD p = horner_fma(kInvOdd, 23, dd_mul_fma(s, s));
    const DD lm = dd_mul_d_fma(dd_mul_fma(p, s), 2.0);
    const DD r = e != 0 ? dd_add(dd_mul_d_fma(kLn2, static_cast<double>(e)), lm) : lm;
    if (r.hi == 0.0) return 0.0;  // log(1) = +0
    // fyl2x multiplies log2(x) by the fldln2 constant, ln 2 rounded to 64 bits: ln(x) * (1 + kLn2Rel).
    return round_pc53(dd_add(r, dd_mul_d_fma(r, kLn2Rel)));
}

// ---- tan: fptan at 53-bit precision (|x| >= 2^63 first reduced by fprem1 with pi * 2^62). x finite.
inline DD u128_to_dd(U128 a) {  // exact for values below 2^106
    const DD h = two_sum(static_cast<double>(static_cast<std::uint32_t>(a.hi >> 32)) * 0x1p96,
                         static_cast<double>(static_cast<std::uint32_t>(a.hi)) * 0x1p64);
    const DD l = two_sum(static_cast<double>(static_cast<std::uint32_t>(a.lo >> 32)) * 0x1p32, static_cast<double>(static_cast<std::uint32_t>(a.lo)));
    return dd_add(h, l);
}
inline constexpr U128 kP66{0x3u, 0x243f6a8885a308d3ull};  // pi rounded to 66 bits, * 2^64 (P66 / 2 = kP66 * 2^-65)
inline constexpr std::uint64_t kPiScaled = 0xc90fdaa22168c235ull;   // the CRT's tbyte 0x9e722a: pi * 2^62 rounded to 64 bits

// tan of a positive value X * 2^ex (0 < X < 2^64, value < 2^63) as fptan computes it.
inline double tan_fptan(std::uint64_t X, int ex) {
    DD r;
    bool odd = false;
    const int sh = ex + 65;
    if (sh < 0) {  // value < 1/4 < P66 / 4: k = 0
        r = u128_to_dd(U128{0, X});
        r = {r.hi * pow2(ex), r.lo * pow2(ex)};
    } else {
        // V = value * 2^65 = k * kP66 + rem (V < 2^128), then k = nearest integer, r = rem * 2^-65.
        const U128 v = sh >= 64 ? U128{X << (sh - 64), 0} : sh > 0 ? U128{X >> (64 - sh), X << sh} : U128{0, X};
        U128 rem{0, 0};
        for (int i = 127; i >= 0; --i) {
            const std::uint64_t bit = i >= 64 ? (v.hi >> (i - 64)) & 1u : (v.lo >> i) & 1u;
            rem = shl1(rem, bit);
            odd = ge(rem, kP66);  // quotient bit; the last one is k's parity
            if (odd) rem = sub(rem, kP66);
        }
        if (ge(shl1(rem, 0), kP66)) {  // rem > kP66 / 2 (never equal: kP66 is odd)
            r = dd_neg(u128_to_dd(sub(kP66, rem)));
            odd = !odd;
        } else {
            r = u128_to_dd(rem);
        }
        r = {r.hi * 0x1p-65, r.lo * 0x1p-65};
    }
    const DD z = dd_mul_fma(r, r);
    const DD s = dd_mul_fma(horner_fma(kSinCoef, 15, z), r);
    const DD c = horner_fma(kCosCoef, 16, z);
    return round_pc53(odd ? dd_neg(dd_div_fma(c, s)) : dd_div_fma(s, c));
}
inline double tan_pc53(double x) {
    const double ax = fabs_exact(x);
    if (ax < 0x1p-28) return x;  // tan(x) = x(1 + x^2/3 ...) rounds to x (zeros and denormals included)
    const std::uint64_t b = bits(ax);
    const int be = static_cast<int>(b >> 52) - 1075;
    const std::uint64_t mant = (b & 0x000fffffffffffffull) | 0x0010000000000000ull;  // ax = mant * 2^be
    double y;
    if (ax < 0x1p63) {
        y = tan_fptan(mant, be);
    } else {
        // fprem1 by kPiScaled (exact IEEE remainder: nearest quotient), then fptan of the remainder.
        std::uint64_t rem = mant % kPiScaled;
        for (int i = 0; i < be; ++i) rem = rem >= kPiScaled - rem ? rem - (kPiScaled - rem) : rem + rem;
        const bool negative = rem > kPiScaled - rem;  // above half: remainder rem - kPiScaled (kPiScaled is odd)
        if (negative) rem = kPiScaled - rem;
        if (rem == 0) return x < 0.0 ? -0.0 : 0.0;  // not reached (kPiScaled is odd, mant < kPiScaled)
        y = tan_fptan(rem, 0);
        if (negative) y = -y;
    }
    return x < 0.0 ? -y : y;
}

}  // namespace joan::fp

#if defined(__clang__)
#pragma float_control(pop)
#elif defined(__GNUC__) && !defined(_MSC_VER)
#pragma GCC pop_options
#endif
