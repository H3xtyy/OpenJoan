// Original library sources (re/attribution.csv original_file, under lib/): crt/tran/i386/87cexp.asm,
//   crt/tran/i386/87cfmod.asm, crt/tran/i386/87clog.asm, crt/tran/i386/87cpow.asm, crt/tran/i386/87csqrt.asm,
//   crt/tran/i386/87ctrig.asm, crt/tran/i386/87ctriga.asm, crt/tran/i386/87disp.asm, crt/tran/i386/87except.c,
//   crt/tran/i386/87fload.asm, crt/tran/i386/fpexcept.c, crt/tran/i386/frnd.c, crt/tran/i386/ieee87.c,
//   crt/tran/ieee87.c, crt/tran/sptype.c.
//
// Family "crt/math": the MSVC 6 libm entry points the game still calls - sin, cos, asin, acos, atan2, pow, sqrt, fmod,
// exp (cdecl, double arguments on the stack, result in st0) and _controlfp - as portable C++ (round P1). Until P1 this file was the whole x87 engine as 122 naked asm pieces; the _CI* entry points
// (st0 arguments), the float.h helpers (_control87, _clearfp, _statusfp, _fpreset, _finite, _isnan, _fpclass,
// _copysign, _chgsign, _scalb, _logb, _nextafter) and the engine's interior entry points (_fload, _math_exit,
// _check_range_exit, _fast_exit, _handle_qnan1, _except1, _sptype, _ctrlfp, _frnd, ...) have no caller left (the
// CRT files that used them by address are C++ now, atan / tan / log in src/fp/pc53.hpp) and are gone.
//
// What the original computes. Every function is a short x87 sequence: transcendental instructions (fsin, fcos,
// fpatan, fyl2x, f2xm1) at 64-bit precision, basic operations at a fixed control word (atan2, exp, fmod: 0x133f, 64
// bits) or at the caller's precision (asin, acos, sqrt, pow), then an exit that stores st0 as a double (sin, cos,
// asin, acos) or leaves the 64-bit st0 (atan2, exp; pow and sqrt leave a value already rounded to the caller's
// precision). Every C++ caller of these slots reads st0 as a double, so the observable result is st0 rounded once to
// double, plus errno (EDOM 33 / ERANGE 34 through _errno 0x7bbfb5; _matherr is never called, its flag 0x00a320d8 is
// 9876). Here each function replays its original sequence on a small software model of the x87 (joan::fp::X of
// src/fp/x87_64.hpp: 64-bit significand, wide exponent, exact rounding to 24 / 53 / 64 bits; fprem / fprem1 /
// frndint / fscale exact), the error tails included: domain errors, NaN operands with the x87 NaN rules and the
// _trandisp loader's NaN payload quirk, the overflow / underflow paths of _87except / _handle_exc (truncating
// denormalisation), pow's integer tests (y * 0.5 at the caller's precision). The transcendental instructions are modelled as correctly rounded to 64 bits
// (double-double evaluation, ~2^-100); fsin / fcos reduce by the 66-bit pi of the x87 (pi66 = 0x3243f6a8885a308d3 *
// 2^-64), not by the true pi. The real instructions are within one 64-bit ulp but not always correctly rounded
// (harness CPU: about 3 % of fsin, 16 % of fpatan results one 64-bit ulp off), so a double result can differ from the
// original's in its last bit when the exact value lies within ~2^-64 of the middle between two doubles: measured
// ~1e-5 of the calls for sin / cos / atan2 / exp, none for sqrt / pow / fmod at 24 bits, and no difference at all
// once rounded to float (re/p1/x87math/README.md).
//
// Precision control: sin, cos, atan2, exp, fmod do not depend on it; asin, acos, sqrt, pow use the caller's for
// their basic operations, from x87::current_precision() (24 bits under the game's Direct3D convention, 53 before the
// device exists, 64 bits for anything else). Rounding is round to nearest (the game never changes it).
//
// Floating point here must be strict IEEE double (SSE2 on i686, never x87 extended precision, no FMA contraction):
// the double-double kernels rely on exact error terms (every product / sum of an error-free transformation is its own
// statement, and contraction is switched off below).
#include "../../callconv.hpp"
#include <bit>
#include <cmath>
#include <cstdint>

#include "../../fn_names.hpp"
#include "../../fp/x87_64.hpp"
#include "../../registry.hpp"
#include "../../x87.hpp"

#if defined(_MSC_VER) && !defined(__clang__)
#pragma fp_contract(off)
#elif defined(__clang__)
#pragma STDC FP_CONTRACT OFF
#endif

namespace {

using u32 = std::uint32_t;
using u64 = std::uint64_t;

// The software x87 (src/fp/x87_64.hpp).
using joan::fp::X;
using joan::fp::kInf;
using joan::fp::kL2e64;
using joan::fp::kNaN;
using joan::fp::kNorm;
using joan::fp::kPi2_64;
using joan::fp::kPi64;
using joan::fp::kZero;
using joan::fp::x_abs;
using joan::fp::x_add;
using joan::fp::x_cmp;
using joan::fp::x_const;
using joan::fp::x_div;
using joan::fp::x_f2xm1;
using joan::fp::x_from_double;
using joan::fp::x_from_double_td;
using joan::fp::x_mul;
using joan::fp::x_nan_result;
using joan::fp::x_neg;
using joan::fp::x_patan;
using joan::fp::x_quiet;
using joan::fp::x_rem;
using joan::fp::x_rint;
using joan::fp::x_round_to;
using joan::fp::x_scale;
using joan::fp::x_sqrt;
using joan::fp::x_sub;
using joan::fp::x_to_double;
using joan::fp::x_to_int;
using joan::fp::x_trig;
using joan::fp::x_yl2x;

// ================================================================================================ the CRT tails
// _set_errno 0x7b99e5 through the game's _errno (0x7bbfb5, family crt/mt): type 1 (DOMAIN) EDOM, 2 / 3 (SING /
// OVERFLOW) ERANGE, anything else nothing.
void set_errno(int type) {
    if (type == 1) *joan::call::cdecl_call<int*>(fn::crt_errno) = 0x21;
    else if (type == 2 || type == 3) *joan::call::cdecl_call<int*>(fn::crt_errno) = 0x22;
}
// _87except's mapping of the _math_exit codes: 7 (a quiet NaN operand) is reported as DOMAIN.
void math_error(int code) { set_errno(code == 7 ? 1 : code); }

constexpr double kMinNormal = 0x1p-1022;
constexpr double kInfinity = std::bit_cast<double>(u64{0x7ff0000000000000u});
// Underflow tail (0x7ba02a / 0x7ba2cf, then _87except -> _handle_exc with UE masked): v * 2^1536 rounded to double,
// then denormalised by a truncating shift (sign kept; below 2^-1074 the result is +0).
double underflow_result(X v) {
    X w = x_scale(v, 1536);
    double d = x_to_double(w);
    if (!(d >= kMinNormal || d <= -kMinNormal)) d *= 0.0;
    if (d == 0.0) return d;
    const u64 b = std::bit_cast<u64>(d);
    const int ue = static_cast<int>((b >> 52) & 0x7ff) - 1023;  // unbiased exponent of d
    const int ecx = ue + 1 - 1536;                                 // _decomp's exponent - 0x600
    if (ecx < -1074) return 0.0;
    u64 bits = (b & 0xfffffffffffffu) | 0x10000000000000u;
    if (ecx < -1021) {
        const int n = -1021 - ecx;
        bits = n >= 64 ? 0 : bits >> n;
    }
    const double r = std::bit_cast<double>(bits);
    return (b >> 63) ? -r : r;
}
// Overflow tail (0x7ba053 / 0x7ba2fd, then _handle_exc with OE masked, round to nearest): +-infinity.
double overflow_result(X v) { return v.neg ? -kInfinity : kInfinity; }

// Exit check of the _trandisp functions (0x7ba25a) for a result computed by a default handler (marker -2) and of
// pow (_check_overflow_exit 0x7b9fd9): the result rounded to double decides between underflow, overflow and the
// plain result (st0 as it is; the caller reads it as a double).
double range_exit(X v, int& error) {
    const double d = x_to_double(v);
    const u64 ex = (std::bit_cast<u64>(d) >> 52) & 0x7ff;
    if (ex == 0) {
        error = 4;
        return underflow_result(v);
    }
    if (ex == 0x7ff) {
        error = 3;
        return overflow_result(v);
    }
    return d;
}

// NaN operand of the one-argument functions (_math_nan_result 0x7b9f1c): a quiet NaN is passed through (code 7),
// a signalling one quieted by fadd 1.0 (code 1); both DOMAIN.
double nan_arg(double x) {
    math_error(1);
    return x_to_double(x_quiet(x_from_double(x)));
}
double indefinite() { return std::bit_cast<double>(u64{0xfff8000000000000u}); }  // tbyte 0x00a310e0 stored

bool is_special(double x) { return ((std::bit_cast<u64>(x) >> 52) & 0x7ff) == 0x7ff; }
bool is_nan(double x) { return is_special(x) && (std::bit_cast<u64>(x) & 0xfffffffffffffu) != 0; }

// The caller's precision control (asin, acos, sqrt, pow use it for their basic operations): the emulated x87
// precision of this thread (fp/x87.hpp: 53 bits until the renderer's device switches the thread to 24), on every
// architecture. (x64 used a fixed 24 bits until 2026-11-06: results computed at 53 bits - before the device exists, or
// on another thread - then differed from x86 in their last bit, e.g. the ending screen's slide of ref-x86-2026-11-6l.)
int user_bits() {
    switch (joan::hook::x87::current_precision()) {
        case joan::hook::x87::Precision::Single: return 24;
        case joan::hook::x87::Precision::Double: return 53;
        default: return 64;
    }
}

// ============================================================================================ the CRT functions
// 0x007b240d / 0x007b24bd bodies: fsin / fcos; |x| >= 2^63 is first reduced by fprem1 with the tbyte 0x009e722a
// (pi * 2^62 rounded to 64 bits); +-inf -> indefinite, DOMAIN.
constexpr X kTwoPi62 = x_const(0xc90fdaa22168c235u, 63);
double trig(double xd, bool want_cos) {
    if (is_special(xd)) {
        if (is_nan(xd)) return nan_arg(xd);
        math_error(1);
        return indefinite();
    }
    X x = x_from_double(xd);
    if (x.k == kZero) return want_cos ? 1.0 : xd;  // fsin(+-0) = +-0, fcos(+-0) = 1, exact
    if (x.e >= 63) x = x_rem(x, kTwoPi62, true);
    if (x.k == kZero) return want_cos ? 1.0 : x_to_double(x);
    return x_to_double(x_trig(x, want_cos));
}

// 0x007b256d / 0x007b227d bodies: asin / acos. |x| < 1: s = sqrt((1 + x)(1 - x)) at the caller's precision, then
// fpatan(x, s) / fpatan(s, x); +-1 exact constants; |x| > 1, inf -> indefinite, DOMAIN.
double asin_acos(double xd, bool want_acos) {
    if (is_nan(xd)) return nan_arg(xd);
    const u64 b = std::bit_cast<u64>(xd);
    const u64 hi = b >> 32;
    if ((hi & 0x7ff00000u) >= 0x3ff00000u) {
        if ((hi & 0x7ff00000u) == 0x3ff00000u && (b & 0xfffffffffffffu) == 0) {
            const bool neg = (b >> 63) != 0;
            if (want_acos) return neg ? x_to_double(kPi64) : 0.0;
            return x_to_double(neg ? x_neg(kPi2_64) : kPi2_64);
        }
        math_error(1);
        return indefinite();
    }
    const int bits = user_bits();
    const X x = x_from_double(xd), one = x_const(u64{1} << 63, 0);
    const X t = x_add(one, x, bits);
    const X u = x_sub(one, x, bits);
    const X s = x_sqrt(x_mul(t, u, bits), bits);
    return x_to_double(want_acos ? x_patan(s, x) : x_patan(x, s));
}

// 0x007b234d body: fsqrt at the caller's precision; -0 -> -0, +inf -> +inf, x < 0 / -inf -> indefinite, DOMAIN.
double sqrt_body(double xd) {
    if (is_nan(xd)) return nan_arg(xd);
    const X x = x_from_double(xd);
    if (x.k == kZero || (x.k == kInf && !x.neg)) return xd;
    if (x.neg) {
        math_error(1);
        return indefinite();
    }
    return x_to_double(x_sqrt(x, user_bits()));
}

// _d_inttype 0x7ba737 (pow special cases): 0 not an integer (or denormal), 1 odd, 2 even
int d_inttype(double y) {
    const X v = x_from_double(y);
    const u64 ex = (std::bit_cast<u64>(y) >> 52) & 0x7ff;
    if (ex == 0 && v.k != kZero) return 0;  // _fpclass: denormal
    if (v.k != kNorm && v.k != kZero) return 0;
    if (x_cmp(x_rint(v), v) != 0) return 0;
    const X h = x_from_double(y / 2.0);
    return x_cmp(x_rint(h), h) == 0 ? 2 : 1;
}
// _pow_special 0x7ba607: x or y infinite (neither NaN). Returns the result; error = 1 for the DOMAIN case.
double pow_special(double x, double y, int& error) {
    const double ax = x < 0.0 ? -x : x;
    const u64 bx = std::bit_cast<u64>(x), by = std::bit_cast<u64>(y);
    if (by == 0x7ff0000000000000u) {  // y = +inf
        if (ax > 1.0) return kInfinity;
        if (ax < 1.0) return 0.0;
        error = 1;
        return indefinite();
    }
    if (by == 0xfff0000000000000u) {  // y = -inf
        if (ax > 1.0) return 0.0;
        if (ax >= 1.0) {
            error = 1;
            return indefinite();
        }
        return kInfinity;
    }
    if (bx == 0x7ff0000000000000u) {  // x = +inf
        if (y > 0.0) return kInfinity;
        if (y < 0.0) return 0.0;
        return 1.0;
    }
    // x = -inf (the only case left)
    const int it = d_inttype(y);
    if (y > 0.0) return it == 1 ? -kInfinity : kInfinity;
    if (y < 0.0) return it == 1 ? -0.0 : 0.0;
    return 1.0;
}
// _isintTOS 0x7b28f5: 0 not an integer, 1 odd, 2 even. y * 0.5 is an fmul at the caller's precision, so a large
// odd y whose half rounds to an integer counts as even.
int isint(X y, int bits) {
    if (y.k == kZero) return 2;
    if (y.k != kNorm || x_cmp(x_rint(y), y) != 0) return 0;
    X h = y;
    h.e -= 1;
    h = x_round_to(h, bits);
    return x_cmp(x_rint(h), h) == 0 ? 2 : 1;
}
// 0x007b2722 body: pow
double pow_body(double xd, double yd) {
    const bool ynan = is_nan(yd), xnan = is_nan(xd);
    if (ynan || xnan) {
        // st1 = y, st0 = x; faddp leaves the NaN by the x87 rules (destination y)
        math_error(1);
        return x_to_double(x_nan_result(x_from_double(yd), x_from_double(xd)));
    }
    if (is_special(yd) || is_special(xd)) {
        int error = 0;
        const double r = pow_special(xd, yd, error);
        if (error) math_error(1);
        return r;
    }
    const int bits = user_bits();
    const X x = x_from_double(xd), y = x_from_double(yd);
    if (x.k == kZero) {
        const u64 by = std::bit_cast<u64>(yd);
        if (((by >> 52) & 0x7ff) == 0) return y.k == kZero ? 1.0 : 0.0;  // y zero or denormal
        const int it = isint(y, bits);
        const bool odd_neg_zero = x.neg && it == 1;
        if (y.neg) {
            math_error(2);
            return odd_neg_zero ? -kInfinity : kInfinity;
        }
        return odd_neg_zero ? -0.0 : 0.0;
    }
    bool negate = false;
    X ax = x;
    if (x.neg) {
        const int it = isint(y, bits);
        if (it == 0) {
            math_error(1);
            return indefinite();
        }
        negate = it == 1;
        ax = x_abs(x);
    }
    // fyl2x; _twoToTOS: n = rint(t), f = t - n, 2^f = 1 + f2xm1(f), fscale
    const X t = x_yl2x(y, ax);
    const X n = x_rint(t);
    const X f = x_neg(x_sub(n, t, bits));
    X v = x_add(x_f2xm1(f), x_const(u64{1} << 63, 0), bits);
    if (n.k == kNorm) {
        if (n.e >= 31) v = x_scale(v, n.neg ? -100000 : 100000);
        else v = x_scale(v, x_to_int(n));
    }
    if (negate) v = x_neg(v);
    int error = 0;
    const double r = range_exit(v, error);
    if (error) math_error(error);
    return r;
}

// atan2 (0x7b2240, _trandisp2 with the descriptor 0x00a31110): handler by the fxam classes of (y, x).
double atan2_body(double yd, double xd) {
    const X y = x_from_double_td(yd), x = x_from_double_td(xd);
    if (y.k == kNaN || x.k == kNaN) {
        // 0x7b9e5f / 0x7b9e5d / 0x7b9e87: faddp leaves the NaN (destination = the other operand); QNaN code 7,
        // SNaN code 1, both DOMAIN
        math_error(1);
        if (x.k == kNaN && y.k != kNaN) return x_to_double(x_nan_result(y, x));
        if (y.k == kNaN && x.k != kNaN) return x_to_double(x_nan_result(x, y));
        return x_to_double(x_nan_result(y, x));
    }
    if (y.k == kInf && x.k == kInf) {  // 0x7b9ec6: indefinite, DOMAIN
        math_error(1);
        return indefinite();
    }
    if (y.k == kInf || (y.k == kNorm && x.k == kZero)) {  // 0x7ba172: +-pi/2 (sign of y)
        return x_to_double(y.neg ? x_neg(kPi2_64) : kPi2_64);
    }
    if (x.k == kInf || y.k == kZero) {  // 0x7ba155: x < 0: +-pi (sign of y); x > 0: +0
        if (x.neg) return x_to_double(y.neg ? x_neg(kPi64) : kPi64);
        return 0.0;
    }
    // 0x7ba0fc: fpatan(|y|, |x|); x < 0: fldpi, fsubrp (64 bits); y < 0: fchs
    X a = x_patan(x_abs(y), x_abs(x));
    if (x.neg) a = x_sub(kPi64, a, 64);
    if (y.neg) a = x_neg(a);
    int error = 0;
    const double r = range_exit(a, error);
    if (error) math_error(error);
    return r;
}

// exp (0x7b70e0, _trandisp1 with the descriptor 0x00a3119a)
double exp_body(double xd) {
    const X x = x_from_double_td(xd);
    switch (x.k) {
        case kZero: return 1.0;
        case kNaN: math_error(1); return x_to_double(x_quiet(x));
        case kInf: return x.neg ? 0.0 : xd;
        case kNorm: break;
    }
    const X t = x_mul(x, kL2e64, 64);
    const X lim = x_const(0xffff000000000000u, 14);  // tbyte 0x00a3117e: 32767.5
    if (x_cmp(x_abs(t), lim) >= 0) {
        if (t.neg) return 0.0;  // underflow code 4 with result +0: no errno
        math_error(3);
        return kInfinity;
    }
    const X n = x_rint(t);
    const X f = x_sub(t, n, 64);
    X v = x_add(x_const(u64{1} << 63, 0), x_f2xm1(x_abs(f)), 64);
    if (f.neg) v = x_div(x_const(u64{1} << 63, 0), v, 64);
    if (n.k == kNorm) v = x_scale(v, x_to_int(n));
    int error = 0;
    const double r = range_exit(v, error);
    if (error) math_error(error);
    return r;
}

// fmod (0x7b2920, _trandisp2 with the descriptor 0x00a309d0)
double fmod_body(double xd, double yd) {
    const X x = x_from_double_td(xd), y = x_from_double_td(yd);
    if (x.k == kNaN || y.k == kNaN) {
        math_error(1);
        if (y.k == kNaN && x.k != kNaN) return x_to_double(x_nan_result(x, y));
        if (x.k == kNaN && y.k != kNaN) return x_to_double(x_nan_result(y, x));
        return x_to_double(x_nan_result(x, y));
    }
    if (x.k == kNorm && y.k == kNorm) return x_to_double(x_rem(x, y, false));  // fprem loop: exact
    if (x.k == kZero && y.k == kNorm) return 0.0;                              // 0x7b9e24: +0
    math_error(1);                                                             // 0x7b9ec6: indefinite, DOMAIN
    return indefinite();
}

// ---- entry points (cdecl, the original signatures)
double JOAN_CDECL crt_sin(double x) { return trig(x, false); }  // 0x007b2404
double JOAN_CDECL crt_cos(double x) { return trig(x, true); }    // 0x007b24b4
double JOAN_CDECL crt_asin(double x) { return asin_acos(x, false); }  // 0x007b2564
double JOAN_CDECL crt_acos(double x) { return asin_acos(x, true); }   // 0x007b2274
double JOAN_CDECL crt_atan2(double y, double x) { return atan2_body(y, x); }  // 0x007b2240
double JOAN_CDECL crt_pow(double x, double y) { return pow_body(x, y); }      // 0x007b2719
double JOAN_CDECL crt_sqrt(double x) { return sqrt_body(x); }                 // 0x007b2344
double JOAN_CDECL crt_fmod(double x, double y) { return fmod_body(x, y); }    // 0x007b2920
double JOAN_CDECL crt_exp(double x) { return exp_body(x); }                   // 0x007b70e0

// 0x007b78af _controlfp(new, mask) = _control87(new, mask & ~_EM_DENORMAL): the abstract control word with the
// masked bits replaced. The game's only call is _setdefaultprecision's _controlfp(_PC_53, _MCW_PC) at start-up (its
// result unused), which matches the host default (MSVC: 53-bit precision); the x87 control word itself is left to
// the host (Direct3D device creation sets 24 bits, gl/device.cpp). Kept here as the abstract value, starting from
// the Windows default 0x27f (all exceptions masked, 53 bits, nearest) = 0x0009001f.
u32 g_abstract_cw = 0x0009001fu;
u32 JOAN_CDECL crt_controlfp(u32 value, u32 mask) {
    mask &= 0xfff7ffffu;
    g_abstract_cw = (g_abstract_cw & ~mask) | (value & mask);
    return g_abstract_cw;
}

#define MATH(name, addr, fn) JOAN_REPLACE("crt/math", name, addr, fn)
MATH("crt::atan2", 0x007b2240, &crt_atan2);
MATH("crt::acos", 0x007b2274, &crt_acos);
MATH("crt::sqrt", 0x007b2344, &crt_sqrt);
MATH("crt::sin", 0x007b2404, &crt_sin);
MATH("crt::cos", 0x007b24b4, &crt_cos);
MATH("crt::asin", 0x007b2564, &crt_asin);
MATH("crt::pow", 0x007b2719, &crt_pow);
MATH("crt::fmod", 0x007b2920, &crt_fmod);
MATH("crt::exp", 0x007b70e0, &crt_exp);
MATH("crt::_controlfp", 0x007b78af, &crt_controlfp);
#undef MATH

}  // namespace
