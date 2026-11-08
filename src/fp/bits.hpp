// Bit patterns of IEEE values and the x87's handling of signalling NaNs (joan::fp, see fp/x87.hpp for the overview).
//
// Memory operands are passed around as their bit patterns (std::uint32_t / std::uint64_t), never as float values
// where it matters: on i686 a float can travel through an x87 register (function return), which quietens an SNaN.
#pragma once

#include <cstdint>
#include <cstring>

namespace joan::fp {

inline std::uint64_t bits(double d) {
    std::uint64_t u;
    std::memcpy(&u, &d, sizeof u);
    return u;
}
inline double from_bits(std::uint64_t u) {
    double d;
    std::memcpy(&d, &u, sizeof d);
    return d;
}
inline std::uint32_t fbits(float f) {
    std::uint32_t u;
    std::memcpy(&u, &f, sizeof u);
    return u;
}
inline float fbits_to_float(std::uint32_t u) {
    float f;
    std::memcpy(&f, &u, sizeof f);
    return f;
}

/// 2^e as a double, e in [-1022, 1023] (exact).
inline double pow2(int e) { return from_bits(static_cast<std::uint64_t>(e + 1023) << 52); }

/// A float bit pattern that is a signalling NaN.
inline bool is_snan(std::uint32_t u) {
    return (u & 0x7f800000u) == 0x7f800000u && (u & 0x007fffffu) != 0 && (u & 0x00400000u) == 0;
}
/// `fld dword` + `fstp dword` of an unchanged value: an SNaN is quietened, everything else kept.
inline std::uint32_t quiet(std::uint32_t m) { return is_snan(m) ? m | 0x00400000u : m; }

}  // namespace joan::fp
