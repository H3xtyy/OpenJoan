// joan::fp - bit-exact models of the x87 arithmetic of joan.exe, in portable C++ (SSE2 / x64 doubles, no x87, no
// inline asm). One namespace, one directory:
//
//   fp/bits.hpp    bit patterns, pow2, the x87's SNaN quietening on fld / fstp dword (quiet).
//   fp/x87.hpp     (this file; included by src/x87.hpp, whose joan::hook::x87 names forward here) the precision
//                  control in force, Fpu (one double operation rounded to the current precision), ftol (MSVC 6
//                  _ftol), and the pc24_* operations: PC24 rounding of any double operands with the x87 exponent
//                  range and the x87 NaN rules.
//   fp/exact.hpp   error-free transformations, double-double arithmetic, 128-bit integers (the kernels below).
//   fp/pc24.hpp    the PC24 register model of the D3DX8 maths (R = a double holding a 24-bit significand):
//                  fadd / fsub / fmul / fdiv / fsqrt (register and dword memory forms), fcom predicates, and
//                  fsincos + fstp dword (float results).
//   fp/pc53.hpp    the CRT's atan / tan / log at 53-bit precision (fpatan / fptan / fyl2x as RN53(RN64(f))).
//   fp/x87_64.hpp  a software x87 value (64-bit significand, wide exponent) with exact rounding to 24 / 53 / 64
//                  bits and models of fsin / fcos / fpatan / fyl2x / f2xm1 (correctly rounded to 64 bits).
//
// The models are distinct on purpose: each one is the proven model of its callers (differential harnesses under
// re/p1/), and they differ in corner cases (exponent range, NaN rules, rounding of the transcendental results), so
// a caller keeps the one its proof used.
//
// This file: the original evaluates on the x87 FPU, where every operation is rounded to the precision-control
// setting in force. Direct3D 8 sets single precision (the game creates its device without
// D3DCREATE_FPU_PRESERVE), so results are rounded to 24-bit significands after each step. Here each step is
// emulated exactly on doubles:
//
//  * precision control 53 bits: plain double arithmetic is identical;
//  * precision control 24 bits: the exact result of each double operation is recovered (two-sum / fma error
//    terms) and rounded once to float;
//  * precision control 64 bits (or a non-nearest rounding mode): not reproducible with doubles - callers run the
//    original function instead.
//
// Validated against a real x87 at PC24 (4M random operations incl. ties).
#pragma once

#include "fma.hpp"
#include <float.h>

#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

#include "bits.hpp"
#include "precision.hpp"

#if defined(_MSC_VER)
#define JOAN_FP_INLINE __forceinline  // (MSVC kept these out of line: a call per emulated x87 operation)
#else
#define JOAN_FP_INLINE inline __attribute__((always_inline))
#endif

namespace joan::fp {

// The emulated precision control (Precision, t_precision, set_precision, current_precision): fp/precision.hpp.

/// The float next to f towards `target` (!= f): std::nextafter(f, +-infinity) by its bit pattern (no CRT call).
JOAN_FP_INLINE float float_toward(float f, double target) {
    if (f == 0.0f) return fbits_to_float(target > 0.0 ? 0x00000001u : 0x80000001u);
    const std::uint32_t u = fbits(f);
    return fbits_to_float((f > 0.0f) == (target > static_cast<double>(f)) ? u + 1 : u - 1);
}

/// Correctly rounds hi + lo to float, lo = `lo()`: the exact value's error term (|lo| <= half an ulp of hi in
/// double). Only its sign matters, and only when hi lies exactly between two floats, so it is computed only then
/// (an fma or a division: a CRT call on i686).
template <class Lo>
JOAN_FP_INLINE double round_to_float_lazy(double hi, Lo lo) {
    float f = static_cast<float>(hi);
    const double fd = f;
    if (fd != hi && hi == hi) {  // inexact, not a NaN (an infinite hi converts exactly)
        const float other = float_toward(f, hi);
        const double mid = (fd + static_cast<double>(other)) * 0.5;  // exact
        if (mid == hi) {  // exact tie in double: lo decides
            const double e = lo();
            if (e != 0.0) {
                const float up = f > other ? f : other, down = f > other ? other : f;
                f = e > 0.0 ? up : down;
            }
        }
    }
    return f;
}
// Correctly rounds hi + lo (exact value, |lo| <= half an ulp of hi in double) to float.
inline double round_to_float(double hi, double lo) {
    return round_to_float_lazy(hi, [lo] { return lo; });
}

// One x87 operation at the current precision.
struct Fpu {
    bool single;
    JOAN_FP_INLINE double add(double a, double b) const {
        const double s = a + b;
        if (!single) return s;
        return round_to_float_lazy(s, [&] {
            const double bb = s - a;
            return (a - (s - bb)) + (b - bb);
        });
    }
    JOAN_FP_INLINE double sub(double a, double b) const { return add(a, -b); }
    JOAN_FP_INLINE double mul(double a, double b) const {
        const double p = a * b;
        if (!single) return p;
        return round_to_float_lazy(p, [&] { return joan::fp::fma_exact(a, b, -p); });
    }
    JOAN_FP_INLINE double div(double a, double b) const {
        const double q = a / b;
        if (!single) return q;
        return round_to_float_lazy(q, [&] {
            const double r = joan::fp::fma_exact(-q, b, a);  // a - q*b, exact
            return b != 0.0 ? r / b : 0.0;
        });
    }
};

// MSVC 6 _ftol (0x007b26cc): truncating fistp to 64 bits, low dword returned.
inline std::int32_t ftol(double v) {
    if (!(std::fabs(v) < 9.2233720368547758e18)) return 0;  // integer indefinite 0x8000000000000000
    return static_cast<std::int32_t>(static_cast<std::uint64_t>(static_cast<std::int64_t>(v)));
}

// ------------------------------------------------------------------------------------- PC24, x87 exponent range
// x87 arithmetic at the game's precision control (PC24, round to nearest) with the x87 exponent range: the
// significand is rounded to 24 bits, the exponent is not limited to float's (no overflow to inf, no
// denormalisation while the value stays in st0). The operands are scaled to [0.5, 1) (exact), Fpu{true} rounds
// there, the result is scaled back (exact, or rounded once into the double denormal range like fstp qword).
// `a` / `st0` is the st0 operand (quiet: fld quietens), `b` the other one. A NaN result is the x87 one: a QNaN
// beats an SNaN, then the larger significand, then the positive one, quieted.
inline double pc24_nan(std::uint64_t a, std::uint64_t b) {
    constexpr std::uint64_t kQuiet = 0x0008000000000000ull, kFrac = 0x000fffffffffffffull, kInf = 0x7ff0000000000000ull;
    const bool na = (a & ~(1ull << 63)) > kInf, nb = (b & ~(1ull << 63)) > kInf;
    if (na) a |= kQuiet;
    std::uint64_t r = na ? a : b;
    if (na && nb && (b & kQuiet) != 0) {
        if ((a & kFrac) != (b & kFrac)) r = (b & kFrac) > (a & kFrac) ? b : a;
        else if ((a >> 63) != 0) r = b;
    }
    r |= kQuiet;
    double d = 0.0;
    std::memcpy(&d, &r, 8);
    return d;
}
inline double pc24_mul(double a, double b) {
    if (std::isnan(a) || std::isnan(b)) return pc24_nan(bits(a), bits(b));
    if (!std::isfinite(a) || !std::isfinite(b) || a == 0.0 || b == 0.0) return a * b;
    int ea = 0, eb = 0;
    const double ma = std::frexp(a, &ea), mb = std::frexp(b, &eb);
    return std::ldexp(Fpu{true}.mul(ma, mb), ea + eb);
}
inline double pc24_div(double a, double b) {
    if (std::isnan(a) || std::isnan(b)) return pc24_nan(bits(a), bits(b));
    if (!std::isfinite(a) || !std::isfinite(b) || a == 0.0 || b == 0.0) return a / b;
    int ea = 0, eb = 0;
    const double ma = std::frexp(a, &ea), mb = std::frexp(b, &eb);
    return std::ldexp(Fpu{true}.div(ma, mb), ea - eb);
}
inline double pc24_add(double a, double b) {
    if (std::isnan(a) || std::isnan(b)) return pc24_nan(bits(a), bits(b));
    if (!std::isfinite(a) || !std::isfinite(b) || (a == 0.0 && b == 0.0)) return a + b;
    if (b == 0.0) return pc24_mul(a, 1.0);
    if (a == 0.0) return pc24_mul(b, 1.0);
    const int e = std::ilogb(a) > std::ilogb(b) ? std::ilogb(a) : std::ilogb(b);
    double sa = std::ldexp(a, -e), sb = std::ldexp(b, -e);  // the larger one in [1, 2)
    if (sa == 0.0) sa = std::copysign(std::numeric_limits<double>::denorm_min(), a);  // far below: only its sign counts
    if (sb == 0.0) sb = std::copysign(std::numeric_limits<double>::denorm_min(), b);
    return std::ldexp(Fpu{true}.add(sa, sb), e);
}
inline double pc24_sub(double a, double b) {
    if (std::isnan(a) || std::isnan(b)) return pc24_nan(bits(a), bits(b));
    return pc24_add(a, -b);
}
// A float memory operand that is a NaN, widened as the x87 reads it (an SNaN stays signalling).
inline std::uint64_t pc24_m32_nan(std::uint32_t u) {
    return (static_cast<std::uint64_t>(u & 0x80000000u) << 32) | 0x7ff0000000000000ull | (static_cast<std::uint64_t>(u & 0x007fffffu) << 29);
}
// `fmul dword [m]` / `fdiv dword [m]` on st0.
inline double pc24_mul_m32(double st0, const float* m) {
    std::uint32_t u = 0;
    std::memcpy(&u, m, 4);
    if ((u & 0x7fffffffu) > 0x7f800000u) return pc24_nan(bits(st0), pc24_m32_nan(u));
    return pc24_mul(st0, static_cast<double>(*m));
}
inline double pc24_div_m32(double st0, const float* m) {
    std::uint32_t u = 0;
    std::memcpy(&u, m, 4);
    if ((u & 0x7fffffffu) > 0x7f800000u) return pc24_nan(bits(st0), pc24_m32_nan(u));
    return pc24_div(st0, static_cast<double>(*m));
}

}  // namespace joan::fp
