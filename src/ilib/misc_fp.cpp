// Reimplementation of the floating-point helpers of Misc (d:\ww\ilib\misc.cpp):
// random floats, round(), random(lo, hi) and round-to-decimals.
//
// The x87 rounding of the original is emulated step by step (../x87.hpp): 24-bit precision
// control (the game's) rounds every step to float, any other state to double.
#include <cmath>
#include <cstdint>

#include "../original/misc.hpp"
#include "../registry.hpp"
#include "../x87.hpp"

namespace {
using joan::hook::x87::current_precision;
using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using joan::hook::x87::Precision;

JOAN_COUNTER(c_rand_unit, "Misc::rand_unit");
JOAN_COUNTER(c_rand_unit2, "Misc::rand_unit_inclusive");
JOAN_COUNTER(c_rand_range, "Misc::random(lo, hi)");
JOAN_COUNTER(c_rand_float, "Misc::random(float lo, float hi)");
JOAN_COUNTER(c_round, "Misc::round");
JOAN_COUNTER(c_round_dec, "Misc::round_decimals");

constexpr float kInv32768 = 3.0517578125e-05f;          // 0x009d8c0c
constexpr float kInv32767 = 3.0518509447574615e-05f;    // 0x009d8c10
constexpr float k32767 = 32767.0f;                       // 0x009d8c14

// The emulated FPU at the precision control in force.
Fpu current_fpu() { return Fpu{current_precision() == Precision::Single}; }
}  // namespace

namespace joan::orig {

struct MiscFp : Misc {
    // 0x00590630 - rand() / 32768, in [0, 1).
    double rand_unit(const Fpu& fpu) {
        c_rand_unit.hit();
        return fpu.mul(rand(), kInv32768);
    }
    // 0x00590660 - rand() / 32767, in [0, 1].
    double rand_unit_inclusive(const Fpu& fpu) {
        c_rand_unit2.hit();
        return fpu.mul(rand(), kInv32767);
    }
    // 0x00590690 - lo + (int)(rand() / 32767 * (hi - lo) + 0.5), i.e. lo..hi inclusive.
    std::int32_t random_range(const Fpu& fpu, std::int32_t lo, std::int32_t hi) {
        c_rand_range.hit();
        const double q = fpu.div(rand(), k32767);
        const auto span = static_cast<std::int32_t>(static_cast<std::uint32_t>(hi) - static_cast<std::uint32_t>(lo));
        const double v = fpu.add(fpu.mul(q, span), 0.5);
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(ftol(v)) + static_cast<std::uint32_t>(lo));
    }
    // 0x005906f0 - lo + rand() / 32767 * (hi - lo).
    double random_float(const Fpu& fpu, float lo, float hi) {
        c_rand_float.hit();
        const double q = fpu.div(rand(), k32767);
        return fpu.add(fpu.mul(q, fpu.sub(hi, lo)), lo);
    }
    // 0x00590a20 - round half away from zero: floor(|x| + 0.5) with the sign restored.
    std::int32_t round(const Fpu& fpu, double x) {
        c_round.hit();
        if (!(x >= 0.0)) return -ftol(std::floor(fpu.add(-x, 0.5)));  // x87 "less" includes NaN
        return ftol(std::floor(fpu.add(x, 0.5)));
    }
    // 0x00590740 - rounds v to `places` decimals: mode 1 down, 2 up, otherwise to
    // nearest (fractions exactly 0.5 / -0.5 are not rounded away). Unrounded ST0 result.
    double round_decimals(const Fpu& fpu, float v, std::int32_t places, std::int32_t mode) {
        c_round_dec.hit();
        double scale = 1.0;
        for (; places > 0; --places) scale = fpu.mul(scale, 10.0);
        const double scaled = fpu.mul(v, scale);
        double whole = 0.0;
        const double frac = std::modf(scaled, &whole);
        switch (mode) {
            case 1: return !(frac >= 0.0) ? fpu.div(fpu.sub(whole, 1.0), scale) : fpu.div(whole, scale);
            case 2: return frac > 0.0 ? fpu.div(fpu.add(whole, 1.0), scale) : fpu.div(whole, scale);
            default:
                if (frac > 0.5) return fpu.div(fpu.add(whole, 1.0), scale);
                if (!(frac >= -0.5)) return fpu.div(fpu.sub(whole, 1.0), scale);
                return fpu.div(whole, scale);
        }
    }
};
static_assert(sizeof(MiscFp) == sizeof(Misc));

// 0x00590a20 entry.
std::int32_t Misc::round(double x) { return static_cast<MiscFp*>(this)->round(current_fpu(), x); }

}  // namespace joan::orig

// ---------------------------------------------------------------------------
// Entry points (thiscall, same signatures as the original)
// ---------------------------------------------------------------------------
namespace {
using joan::orig::MiscFp;

struct Entry : MiscFp {
    double rand_unit_entry() { return rand_unit(current_fpu()); }
    double rand_unit_inclusive_entry() { return rand_unit_inclusive(current_fpu()); }
    std::int32_t random_range_entry(std::int32_t lo, std::int32_t hi) {
        return random_range(current_fpu(), lo, hi);
    }
    double random_float_entry(float lo, float hi) { return random_float(current_fpu(), lo, hi); }
    double round_decimals_entry(float v, std::int32_t places, std::int32_t mode) {
        return round_decimals(current_fpu(), v, places, mode);
    }
};
static_assert(sizeof(Entry) == sizeof(joan::orig::Misc));
}  // namespace

#define MISC(name, addr, fn) JOAN_REPLACE("ilib/misc.cpp", "Misc::" name, addr, fn)
MISC("rand_float", 0x00590630, &Entry::rand_unit_entry);
MISC("rand_float_1", 0x00590660, &Entry::rand_unit_inclusive_entry);
MISC("random_range_590690", 0x00590690, &Entry::random_range_entry);
MISC("random_range", 0x005906f0, &Entry::random_float_entry);
MISC("round_decimals", 0x00590740, &Entry::round_decimals_entry);
#undef MISC
