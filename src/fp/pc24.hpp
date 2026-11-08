// The PC24 register model of joan::fp (overview: fp/x87.hpp): x87 arithmetic of the statically linked D3DX8 maths
// (lib/d3dx8/math_matrix.cpp, math_geometry.cpp, objects.cpp), on doubles.
//
// joan.exe runs these routines on the x87 with the control word Direct3D 8 sets: precision control 24 bits, round
// to nearest. Every arithmetic result is rounded to a 24-bit significand, but the register keeps the 15-bit
// exponent: a value on the x87 stack does not overflow / denormalise like a float until it is stored with
// `fstp dword`. A `double` holding a 24-bit significand represents such a register exactly for everything these
// routines compute (products / quotients of at most a few floats stay far inside the double range), so:
//
//   * an x87 value (`R`) is a double with a 24-bit significand (or an infinity / quiet NaN);
//   * `fld dword` = conversion float -> double (exact; an SNaN is quietened like the x87 does);
//   * `fstp dword` = conversion double -> float (round to nearest: for a result in the float denormal range the
//     second rounding of the x87 store happens here as well, overflow gives infinity);
//   * fadd / fsub / fmul / fdiv / fsqrt = the double operation, rounded to 24 bits from the exact result (the
//     double result plus, on an exact tie only, the sign of the exact error term) - see round24;
//   * NaN operands follow the x87 rules (checked on the CPU, re/p1/d3dx/README): one NaN operand -> that NaN
//     quietened (what SSE returns too); two NaNs -> the one with the larger significand, the positive one on a
//     tie (SSE would return the first operand); an SNaN memory operand with a QNaN register operand -> the
//     register (the `*_m` functions); invalid operations -> the negative default NaN (identical on SSE / x87).
//   * fsincos -> sincos(): x87 reduction with its 66-bit pi, evaluated in double-double and rounded to float.
//
// Unlike the pc24_* operations of fp/x87.hpp the operands here are already 24-bit values and the exponent range is
// the double one (enough for these callers). Plain IEEE double code (no fma, no long double, no libm call but
// sqrt): identical on i686 SSE2, x86_64 and other hosts (contraction off: fp/exact.hpp).
#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>

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

using R = double;  // an x87 register at PC24

/// Unaligned dword access at byte offset `off` (the original's [reg + off] operands).
inline std::uint32_t rd(const void* p, std::uint32_t off) {
    std::uint32_t v;
    std::memcpy(&v, static_cast<const unsigned char*>(p) + off, sizeof v);
    return v;
}
inline void wr(void* p, std::uint32_t off, std::uint32_t v) { std::memcpy(static_cast<unsigned char*>(p) + off, &v, sizeof v); }

inline bool is_nan(double d) { return (bits(d) & 0x7fffffffffffffffull) > 0x7ff0000000000000ull; }

/// fld dword: exact, SNaN quietened (explicitly: compilers fold (float)(double)f to f, which keeps an SNaN).
inline R ld(std::uint32_t m) { return static_cast<double>(fbits_to_float(quiet(m))); }
/// fstp dword / fst dword: the stored bit pattern.
inline std::uint32_t st(R v) { return fbits(static_cast<float>(v)); }

/// Rounds the exact value v + e to a 24-bit significand (to nearest even), keeping the double exponent. v is the
/// double nearest to the exact value; `tail()` returns e (only its sign matters) and is called only on an exact tie
/// of v, the one case where the double rounding could differ from a direct rounding.
template <class Tail>
inline R round24(double v, Tail tail) {
    constexpr std::uint64_t kLow = (std::uint64_t{1} << 29) - 1, kHalf = std::uint64_t{1} << 28;
    std::uint64_t u = bits(v);
    if (((u >> 52) & 0x7ff) == 0x7ff) return v;  // infinity / NaN
    const std::uint64_t low = u & kLow;
    if (low == 0) return v;
    u &= ~kLow;
    bool up = low > kHalf;
    if (low == kHalf) {
        const double e = tail();
        if (e == 0.0)
            up = ((u >> 29) & 1) != 0;
        else
            up = (e < 0.0) == ((u >> 63) != 0);  // error in the direction of v's sign: magnitude above the tie
    }
    if (up) u += kLow + 1;  // a carry into the exponent is the correct next binade
    return from_bits(u);
}
inline double no_tail() { return 0.0; }

/// x87 choice between two NaN operands: larger significand, positive on a tie.
inline R nan_pick(R a, R b) {
    const std::uint64_t ma = bits(a) & 0x000fffffffffffffull, mb = bits(b) & 0x000fffffffffffffull;
    if (ma != mb) return ma > mb ? a : b;
    return (bits(a) >> 63) == 0 ? a : b;
}
inline R nan_fix(R a, R b, R result) { return is_nan(a) && is_nan(b) ? nan_pick(a, b) : result; }
/// Register operand a with memory operand m (both NaN): an SNaN in memory loses against the register QNaN.
inline R nan_fix_m(R a, std::uint32_t m, R result) {
    if (!is_nan(a) || !is_nan(ld(m))) return result;
    return is_snan(m) ? a : nan_pick(a, ld(m));
}

inline R add_raw(R a, R b, R s) {
    return round24(s, [&] { return sum_err(a, b, s); });
}
inline R div_raw(R a, R b, R q) {
    return round24(q, [&] {
        const double p = q * b;
        return (a - p) - prod_err(q, b, p);  // a - q*b: sign of the exact remainder
    });
}

// Register-register (and register-constant) operations. Operand order as written in the x87 source:
// fsub(a, b) = a - b, fdiv(a, b) = a / b.
inline R fadd(R a, R b) {
    const R s = a + b;
    if (is_nan(s)) return nan_fix(a, b, s);
    return add_raw(a, b, s);
}
inline R fsub(R a, R b) {
    const R s = a - b;
    if (is_nan(s)) return nan_fix(a, b, s);
    return add_raw(a, -b, s);
}
inline R fmul(R a, R b) {  // 24 x 24 bits: the double product is exact
    const R p = a * b;
    if (is_nan(p)) return nan_fix(a, b, p);
    return round24(p, no_tail);
}
inline R fdiv(R a, R b) {
    const R q = a / b;
    if (is_nan(q)) return nan_fix(a, b, q);
    return div_raw(a, b, q);
}
inline R fsqrt(R a) {
    if (is_nan(a)) return a;
    if (a < 0.0) return from_bits(0xfff8000000000000ull);  // invalid: the default NaN (x87 indefinite)
    const R s = std::sqrt(a);
    return round24(s, [&] {
        const double p = s * s;
        return (a - p) - prod_err(s, s, p);
    });
}

// Register op dword memory operand: fadd_m(st, m) = fadd dword [m], fsub_m = fsub (st - m), fsubr_m = fsubr
// (m - st), fmul_m = fmul, fdiv_m = fdiv (st / m), fdivr_m = fdivr (m / st).
inline R fadd_m(R a, std::uint32_t m) {
    const R d = ld(m), s = a + d;
    if (is_nan(s)) return nan_fix_m(a, m, s);
    return add_raw(a, d, s);
}
inline R fsub_m(R a, std::uint32_t m) {
    const R d = ld(m), s = a - d;
    if (is_nan(s)) return nan_fix_m(a, m, s);
    return add_raw(a, -d, s);
}
inline R fsubr_m(R a, std::uint32_t m) {
    const R d = ld(m), s = d - a;
    if (is_nan(s)) return nan_fix_m(a, m, s);
    return add_raw(d, -a, s);
}
inline R fmul_m(R a, std::uint32_t m) {
    const R d = ld(m), p = a * d;
    if (is_nan(p)) return nan_fix_m(a, m, p);
    return round24(p, no_tail);
}
inline R fdiv_m(R a, std::uint32_t m) {
    const R d = ld(m), q = a / d;
    if (is_nan(q)) return nan_fix_m(a, m, q);
    return div_raw(a, d, q);
}
inline R fdivr_m(R a, std::uint32_t m) {
    const R d = ld(m), q = d / a;
    if (is_nan(q)) return nan_fix_m(a, m, q);
    return div_raw(d, a, q);
}

// fcom / fcomp + fnstsw: C0 (ah bit 0) = less or unordered, C3 (ah bit 6) = equal or unordered.
/// `test ah, 0x41; jne` taken: st <= src or unordered.
inline bool le_or_unordered(R a, R b) { return !(a > b); }
/// `test ah, 0x41; jp` taken (C0 == C3): st > src or unordered.
inline bool gt_or_unordered(R a, R b) { return !(a < b) && !(a == b); }
/// `test ah, 0x1; jne` taken: st < src or unordered.
inline bool lt_or_unordered(R a, R b) { return !(a >= b); }

// ------------------------------------------------------------------------------------------------ fsincos

/// sin and cos of r (double-double, |r| <= pi/4 + 2^-60), relative error < 2^-90: Taylor series, the terms of
/// degree >= 9 / 8 in plain double (their weight is <= 2^-23 there, so their rounding error stays below 2^-90).
inline void sincos_taylor_dd(DD r, DD& s, DD& c) {
    const DD r2 = dd_mul(r, r);
    const double x = r2.hi;
    // 1/n! as double-double (mpmath, 200 bits).
    double ts = 1.1309962886447716e-31;   // 1/29!
    ts = ts * -x + 9.183689863795546e-29;  // 1/27!
    ts = ts * -x + 6.446950284384474e-26;
    ts = ts * -x + 3.868170170630684e-23;
    ts = ts * -x + 1.9572941063391263e-20;
    ts = ts * -x + 8.22063524662433e-18;
    ts = ts * -x + 2.8114572543455206e-15;
    ts = ts * -x + 7.647163731819816e-13;
    ts = ts * -x + 1.6059043836821613e-10;
    ts = ts * -x + 2.505210838544172e-08;
    ts = ts * -x + 2.7557319223985893e-06;  // 1/9! (lo part below)
    DD ps = {ts, -1.858393274046472e-22};
    ps = dd_add_quick(dd_mul(dd_neg(r2), ps), DD{1.984126984126984e-04, 1.7209558293420705e-22});    // 1/7!
    ps = dd_add_quick(dd_mul(dd_neg(r2), ps), DD{8.333333333333333e-03, 1.1564823173178714e-19});    // 1/5!
    ps = dd_add_quick(dd_mul(dd_neg(r2), ps), DD{1.6666666666666666e-01, 9.25185853854297e-18});     // 1/3!
    ps = dd_add_quick(dd_mul(dd_neg(r2), ps), DD{1.0, 0.0});
    s = dd_mul(r, ps);

    double tc = 3.7699876288159054e-33;    // 1/30!
    tc = tc * -x + 3.279889237069838e-30;   // 1/28!
    tc = tc * -x + 2.4795962632247976e-27;
    tc = tc * -x + 1.6117375710961184e-24;
    tc = tc * -x + 8.896791392450574e-22;
    tc = tc * -x + 4.110317623312165e-19;
    tc = tc * -x + 1.5619206968586225e-16;
    tc = tc * -x + 4.779477332387385e-14;
    tc = tc * -x + 1.1470745597729725e-11;
    tc = tc * -x + 2.08767569878681e-09;
    tc = tc * -x + 2.755731922398589e-07;   // 1/10!
    tc = tc * -x + 2.48015873015873e-05;    // 1/8! (lo part below)
    DD pc = {tc, 2.1511947866775882e-23};
    pc = dd_add_quick(dd_mul(dd_neg(r2), pc), DD{1.388888888888889e-03, -5.300543954373577e-20});   // 1/6!
    pc = dd_add_quick(dd_mul(dd_neg(r2), pc), DD{4.1666666666666664e-02, 2.3129646346357427e-18});  // 1/4!
    pc = dd_add_quick(dd_mul(dd_neg(r2), pc), DD{0.5, 0.0});
    pc = dd_add_quick(dd_mul(dd_neg(r2), pc), DD{1.0, 0.0});
    c = pc;
}

/// The float next to f (finite) towards +infinity (up) or -infinity.
inline float next_float(float f, bool up) {
    if (f == 0.0f) return fbits_to_float(up ? 0x00000001u : 0x80000001u);
    const std::uint32_t u = fbits(f);
    return fbits_to_float((f > 0.0f) == up ? u + 1 : u - 1);
}

/// Correctly rounded float of hi + lo (|lo| <= half an ulp of hi; finite, inside the float range).
inline float dd_to_float(DD v) {
    float f = static_cast<float>(v.hi);
    const double fd = f;
    if (fd != v.hi && v.lo != 0.0) {
        const float other = next_float(f, v.hi > fd);
        if ((fd + static_cast<double>(other)) * 0.5 == v.hi) {  // hi on the midpoint: lo decides
            const float up = f > other ? f : other, down = f > other ? other : f;
            f = v.lo > 0.0 ? up : down;
        }
    }
    return f;
}

/// Reduction of |x| (a float >= 0.5, < 2^63) by the x87's pi/2 = N / 2^65, N = 0x3243F6A8885A308D3 (pi rounded to
/// 66 bits, as the FPU's internal constant): x = k * N / 2^65 + r with k the nearest integer, exactly, by long
/// division on 32-bit limbs. Returns r as a double-double and k mod 4.
inline unsigned reduce_pi66_float(float ax, DD& r) {
    // ax = mant * 2^(E - 150), X = ax * 2^65 = mant * 2^(E - 85); E - 85 in [41, 104] for 0.5 <= ax < 2^63.
    const std::uint32_t ub = fbits(ax);
    const std::uint64_t mant = (ub & 0x007fffffu) | 0x00800000u;
    std::uint32_t x[5] = {};
    {
        const int sh = static_cast<int>(ub >> 23) - 85;
        const int w = sh / 32, b = sh % 32;
        const std::uint64_t v = mant << b;  // < 2^56
        x[w] = static_cast<std::uint32_t>(v);
        x[w + 1] = static_cast<std::uint32_t>(v >> 32);
    }
    // N (66 bits) as limbs.
    static constexpr std::uint32_t kN[5] = {0x85a308d3u, 0x243f6a88u, 0x3u, 0u, 0u};
    // Long division, bit by bit: rem = X mod N, k = X / N (k < 2^63).
    std::uint32_t rem[5] = {};
    std::uint64_t k = 0;
    int top = 159;
    while (top >= 0 && ((x[top / 32] >> (top % 32)) & 1) == 0) --top;
    for (int i = top; i >= 0; --i) {
        // rem = rem * 2 + bit i
        for (int j = 4; j > 0; --j) rem[j] = (rem[j] << 1) | (rem[j - 1] >> 31);
        rem[0] = (rem[0] << 1) | ((x[i / 32] >> (i % 32)) & 1);
        // rem >= N ?
        bool ge = true;
        for (int j = 4; j >= 0; --j) {
            if (rem[j] != kN[j]) {
                ge = rem[j] > kN[j];
                break;
            }
        }
        k <<= 1;
        if (ge) {
            std::uint64_t borrow = 0;
            for (int j = 0; j < 5; ++j) {
                const std::uint64_t d = static_cast<std::uint64_t>(rem[j]) - kN[j] - borrow;
                rem[j] = static_cast<std::uint32_t>(d);
                borrow = (d >> 32) & 1;
            }
            k |= 1;
        }
    }
    // Nearest: if 2 * rem > N, rem -= N (negative remainder), k += 1. rem < N < 2^66.
    const std::uint64_t rem_hi = (static_cast<std::uint64_t>(rem[2]) << 32) | rem[1];  // bits 32..95
    const std::uint64_t n_hi = (static_cast<std::uint64_t>(kN[2]) << 32) | kN[1];
    const bool above_half = rem_hi * 2 + (rem[0] >> 31) > n_hi || (rem_hi * 2 + (rem[0] >> 31) == n_hi && (rem[0] << 1) > kN[0]);
    // value of rem (or rem - N) as a double-double: exact (<= 66 bits)
    const double rv_hi = static_cast<double>(rem_hi) * 4294967296.0;  // rem_hi < 2^34: exact
    DD rv = two_sum(rv_hi, static_cast<double>(rem[0]));
    if (above_half) {
        const DD n = two_sum(static_cast<double>(n_hi) * 4294967296.0, static_cast<double>(kN[0]));
        rv = dd_add_quick(rv, dd_neg(n));  // exact: both 66-bit integers, difference < 2^66
        ++k;
    }
    const double scale = 1.0 / 36893488147419103232.0;  // 2^-65
    r = {rv.hi * scale, rv.lo * scale};
    return static_cast<unsigned>(k & 3);
}

/// `fld dword x; fsincos; fstp dword cos; fstp dword sin` with an empty x87 stack below (the D3DX callers'):
/// for |x| >= 2^63 the FPU leaves x unchanged (C2 set, nothing pushed), so the first store writes x and the second
/// one pops an empty register (masked stack underflow: the default NaN). Infinity: both the default NaN.
inline void sincos(std::uint32_t xb, std::uint32_t& s, std::uint32_t& c) {
    const float ax = fbits_to_float(xb & 0x7fffffffu);
    if (!(ax < 9.2233720368547758e18f)) {  // >= 2^63, infinity, NaN
        if ((xb & 0x7fffffffu) > 0x7f800000u) {
            s = c = quiet(xb);  // NaN
        } else if ((xb & 0x7fffffffu) == 0x7f800000u) {
            s = c = 0xffc00000u;  // infinity: invalid
        } else {
            c = xb;
            s = 0xffc00000u;
        }
        return;
    }
    DD r{static_cast<double>(ax), 0.0};
    unsigned q = 0;
    if (ax >= 0.5f) q = reduce_pi66_float(ax, r);
    DD sr, cr;
    sincos_taylor_dd(r, sr, cr);
    DD sv = sr, cv = cr;
    switch (q) {
        case 1: sv = cr; cv = dd_neg(sr); break;
        case 2: sv = dd_neg(sr); cv = dd_neg(cr); break;
        case 3: sv = dd_neg(cr); cv = sr; break;
        default: break;
    }
    if (xb >> 31) sv = dd_neg(sv);
    s = fbits(dd_to_float(sv));
    c = fbits(dd_to_float(cv));
}

}  // namespace joan::fp

#if defined(__clang__)
#pragma float_control(pop)
#elif defined(__GNUC__) && !defined(_MSC_VER)
#pragma GCC pop_options
#endif
