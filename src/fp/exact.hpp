// Exact arithmetic for the x87 models of joan::fp (overview: fp/x87.hpp): error-free transformations, double-double
// values (hi + lo, |lo| <= ulp(hi) / 2) and 128-bit integers. Plain IEEE double / integer code, no libm.
//
// Two families of products, both exact where their callers use them, kept apart because they differ at the edges
// (Dekker's split overflows above ~2^996 and loses bits when the error term underflows; fma does neither):
//   * two_prod / dd_mul / dd_mul_d / dd_div: Dekker (plain IEEE double, no libm) - fp/pc24.hpp, fp/x87_64.hpp;
//   * two_prod_fma / dd_mul_fma / dd_mul_d_fma / dd_div_fma (std::fma): fp/pc53.hpp.
// dd_add is the accurate double-double sum (both parts' errors), dd_add_quick the cheap one (one two_sum).
//
// Requirements: IEEE double arithmetic in round-to-nearest without excess precision (SSE2: /arch:SSE2, or x64), no
// reassociation (/fp:fast / -ffast-math must not be used), and no contraction of a * b + c into an fma: switched
// off below for the definitions in the joan::fp headers (clang / GCC: only there; MSVC: fp_contract has no
// push / pop - off is its /fp:precise default).
#pragma once

#include <cstdint>

#include "bits.hpp"

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

// ------------------------------------------------------------------------------------------- double-double
struct DD {
    double hi, lo;
};

/// a + b - s, exact, for s = a + b rounded (Knuth's two-sum error).
inline double sum_err(double a, double b, double s) {
    const double bb = s - a;
    return (a - (s - bb)) + (b - bb);
}
inline DD two_sum(double a, double b) {
    const double s = a + b;
    return {s, sum_err(a, b, s)};
}
/// |a| >= |b| or a == 0.
inline DD fast_two_sum(double a, double b) {
    const double s = a + b;
    return {s, b - (s - a)};
}
/// Dekker's split: a = hi + lo, each with at most 26 significant bits.
inline DD split(double a) {
    const double c = 134217729.0 * a;  // 2^27 + 1
    const double hi = c - (c - a);
    return {hi, a - hi};
}
/// a * b - p, exact for p = a * b rounded (Dekker; no overflow in split, no underflow of the terms).
inline double prod_err(double a, double b, double p) {
    const DD x = split(a), y = split(b);
    return ((x.hi * y.hi - p) + x.hi * y.lo + x.lo * y.hi) + x.lo * y.lo;
}
inline DD two_prod(double a, double b) {
    const double p = a * b;
    return {p, prod_err(a, b, p)};
}

inline DD dd_neg(DD a) { return {-a.hi, -a.lo}; }
inline DD dd_add(DD a, DD b) {
    DD s = two_sum(a.hi, b.hi);
    const DD t = two_sum(a.lo, b.lo);
    s.lo += t.hi;
    s = fast_two_sum(s.hi, s.lo);
    s.lo += t.lo;
    return fast_two_sum(s.hi, s.lo);
}
inline DD dd_add_d(DD a, double b) {
    const DD s = two_sum(a.hi, b);
    return fast_two_sum(s.hi, s.lo + a.lo);
}
inline DD dd_add_quick(DD a, DD b) {
    const DD s = two_sum(a.hi, b.hi);
    return fast_two_sum(s.hi, s.lo + (a.lo + b.lo));
}
inline DD dd_sub(DD a, DD b) { return dd_add(a, dd_neg(b)); }

inline DD dd_mul(DD a, DD b) {
    DD p = two_prod(a.hi, b.hi);
    p.lo += a.hi * b.lo + a.lo * b.hi;
    return fast_two_sum(p.hi, p.lo);
}
inline DD dd_mul_d(DD a, double b) {
    DD p = two_prod(a.hi, b);
    p.lo += a.lo * b;
    return fast_two_sum(p.hi, p.lo);
}
inline DD dd_div(DD a, DD b) {  // b normal
    const double q1 = a.hi / b.hi;
    DD r = dd_sub(a, dd_mul_d(b, q1));
    const double q2 = r.hi / b.hi;
    r = dd_sub(r, dd_mul_d(b, q2));
    const double q3 = r.hi / b.hi;
    return dd_add(fast_two_sum(q1, q2), {q3, 0.0});
}

// ---- constants (correctly rounded double-double; mpmath, 400 bits)
inline constexpr DD kPi{0x1.921fb54442d18p+1, 0x1.1a62633145c07p-53};
inline constexpr DD kPi2{0x1.921fb54442d18p+0, 0x1.1a62633145c07p-54};  // pi / 2
inline constexpr DD kLn2{0x1.62e42fefa39efp-1, 0x1.abc9e3b39803fp-56};
inline constexpr DD kInvLn2{0x1.71547652b82fep+0, 0x1.777d0ffda0d24p-56};
inline constexpr DD kInvOdd[24] = {  // 1 / (2k + 1)
    {0x1.0000000000000p+0, 0.0},
    {0x1.5555555555555p-2, 0x1.5555555555555p-56},
    {0x1.999999999999ap-3, -0x1.999999999999ap-57},
    {0x1.2492492492492p-3, 0x1.2492492492492p-57},
    {0x1.c71c71c71c71cp-4, 0x1.c71c71c71c71cp-58},
    {0x1.745d1745d1746p-4, -0x1.745d1745d1746p-59},
    {0x1.3b13b13b13b14p-4, -0x1.3b13b13b13b14p-58},
    {0x1.1111111111111p-4, 0x1.1111111111111p-60},
    {0x1.e1e1e1e1e1e1ep-5, 0x1.e1e1e1e1e1e1ep-61},
    {0x1.af286bca1af28p-5, 0x1.af286bca1af28p-59},
    {0x1.8618618618618p-5, 0x1.8618618618618p-59},
    {0x1.642c8590b2164p-5, 0x1.642c8590b2164p-60},
    {0x1.47ae147ae147bp-5, -0x1.eb851eb851eb8p-61},
    {0x1.2f684bda12f68p-5, 0x1.2f684bda12f68p-59},
    {0x1.1a7b9611a7b96p-5, 0x1.1a7b9611a7b96p-61},
    {0x1.0842108421084p-5, 0x1.0842108421084p-60},
    {0x1.f07c1f07c1f08p-6, -0x1.f07c1f07c1f08p-61},
    {0x1.d41d41d41d41dp-6, 0x1.0750750750750p-60},
    {0x1.bacf914c1bad0p-6, -0x1.bacf914c1bad0p-60},
    {0x1.a41a41a41a41ap-6, 0x1.0690690690690p-60},
    {0x1.8f9c18f9c18fap-6, -0x1.f3831f3831f38p-61},
    {0x1.7d05f417d05f4p-6, 0x1.7d05f417d05f4p-62},
    {0x1.6c16c16c16c17p-6, -0x1.f49f49f49f49fp-61},
    {0x1.5c9882b931057p-6, 0x1.310572620ae4cp-61},
};
inline constexpr DD kAtan16[17] = {  // atan(j / 16)
    {0x0.0p+0, 0.0},
    {0x1.ff55bb72cfdeap-5, -0x1.c934d86d23f1dp-60},
    {0x1.fd5ba9aac2f6ep-4, -0x1.cd37686760c17p-59},
    {0x1.7b97b4bce5b02p-3, 0x1.347b0b4f881cap-58},
    {0x1.f5b75f92c80ddp-3, 0x1.8ab6e3cf7afbdp-57},
    {0x1.362773707ebccp-2, -0x1.963a544b672d8p-57},
    {0x1.6f61941e4def1p-2, -0x1.c63aae6f6e918p-56},
    {0x1.a64eec3cc23fdp-2, -0x1.24dec1b50b7ffp-56},
    {0x1.dac670561bb4fp-2, 0x1.a2b7f222f65e2p-56},
    {0x1.0657e94db30d0p-1, -0x1.d5b495f6349e6p-56},
    {0x1.1e00babdefeb4p-1, -0x1.928df287a668fp-58},
    {0x1.345f01cce37bbp-1, 0x1.1021137c71102p-55},
    {0x1.4978fa3269ee1p-1, 0x1.2419a87f2a458p-56},
    {0x1.5d58987169b18p-1, 0x1.0028e4bc5e7cap-57},
    {0x1.700a7c5784634p-1, -0x1.8c34d25aadef6p-56},
    {0x1.819d0b7158a4dp-1, -0x1.bf76229d3b917p-56},
    {0x1.921fb54442d18p-1, 0x1.1a62633145c07p-55},
};

// ------------------------------------------------------------------------------------------- 128-bit integers
struct U128 {
    std::uint64_t hi, lo;
};
inline U128 mul64(std::uint64_t a, std::uint64_t b) {
    const std::uint64_t a0 = a & 0xffffffffu, a1 = a >> 32, b0 = b & 0xffffffffu, b1 = b >> 32;
    const std::uint64_t p00 = a0 * b0, p01 = a0 * b1, p10 = a1 * b0, p11 = a1 * b1;
    const std::uint64_t mid = (p00 >> 32) + (p01 & 0xffffffffu) + (p10 & 0xffffffffu);
    return {p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32), (mid << 32) | (p00 & 0xffffffffu)};
}
inline bool ge(U128 a, U128 b) { return a.hi != b.hi ? a.hi > b.hi : a.lo >= b.lo; }
inline U128 sub(U128 a, U128 b) { return {a.hi - b.hi - (a.lo < b.lo ? 1u : 0u), a.lo - b.lo}; }
inline U128 shl(U128 v, int n) {  // 0 <= n < 128
    if (n == 0) return v;
    if (n >= 64) return {v.lo << (n - 64), 0};
    return {(v.hi << n) | (v.lo >> (64 - n)), v.lo << n};
}
/// v << 1 with `bit` shifted in.
inline U128 shl1(U128 a, std::uint64_t bit) { return {(a.hi << 1) | (a.lo >> 63), (a.lo << 1) | bit}; }
inline U128 shr_sticky(U128 v, int n, bool& sticky) {  // n >= 0; bits shifted out go to sticky
    if (n == 0) return v;
    if (n >= 128) {
        sticky |= (v.hi | v.lo) != 0;
        return {0, 0};
    }
    if (n >= 64) {
        sticky |= v.lo != 0 || (n > 64 && (v.hi << (128 - n)) != 0);
        return {0, v.hi >> (n - 64)};
    }
    sticky |= (v.lo << (64 - n)) != 0;
    return {v.hi >> n, (v.lo >> n) | (v.hi << (64 - n))};
}

}  // namespace joan::fp

#if defined(__clang__)
#pragma float_control(pop)
#elif defined(__GNUC__) && !defined(_MSC_VER)
#pragma GCC pop_options
#endif
