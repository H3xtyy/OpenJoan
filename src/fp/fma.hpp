// Fused multiply-add (a * b + c with one rounding), which the exact float / x87 emulation uses for the error terms of
// products and quotients. fma is exactly rounded wherever it comes from, so every path below gives the same bits.
//   - MSVC: std::fma (its C runtime uses the processor's FMA instruction when there is one);
//   - GCC / Clang on x86: the FMA instruction when the processor has it, else std::fma - glibc's fma has no hardware
//     path on 32-bit x86, a multi-precision software routine (~1.5% of a frame in a Linux profile).
#pragma once

#include <cmath>

#include "../compiler.hpp"

namespace joan::fp {

#if !defined(_MSC_VER) && (defined(__i386__) || defined(__x86_64__))
namespace fma_detail {

JOAN_TARGET("fma") inline double hardware(double a, double b, double c) { return __builtin_fma(a, b, c); }

JOAN_TARGET("xsave") inline bool cpu_has_fma() {
    int r[4] = {};
    joan::cpu::id(r, 1);
    if ((r[2] & (1 << 12)) == 0 || (r[2] & (1 << 27)) == 0 || (r[2] & (1 << 28)) == 0) return false;  // FMA, OSXSAVE, AVX
    return (_xgetbv(0) & 6u) == 6u;  // the system saves the YMM registers
}

// Zero (std::fma) until static initialisation sets it: both give the same result.
inline const bool g_hardware = cpu_has_fma();

}  // namespace fma_detail

inline double fma_exact(double a, double b, double c) {
    return fma_detail::g_hardware ? fma_detail::hardware(a, b, c) : std::fma(a, b, c);
}
#else
inline double fma_exact(double a, double b, double c) { return std::fma(a, b, c); }
#endif

}  // namespace joan::fp
