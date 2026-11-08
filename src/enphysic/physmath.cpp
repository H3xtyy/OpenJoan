// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/physmath.cpp: functions of the original d:\ww\enphysic\physmath.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enphysic/physmath.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
// for it.

#include "../callconv.hpp"
#include <cstdint>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_players1.cpp (game/players1)
namespace {
namespace game_players1 {

// fcomp + test ah,0x40 + je "differs": equal OR unordered falls through.
bool same_or_unordered(float a, float b) { return !(a < b || a > b); }

JOAN_COUNTER(c_slope, "Grid::slope");

// 0x006accf0 (cdecl, ret): v[2] / sqrt(v[0]^2 + v[1]^2); the constant +inf (0x009dd78c) when
// both v[0] and v[1] compare equal to 0 (or unordered).
float JOAN_CDECL slope(const float* v) {
    c_slope.hit();
    if (same_or_unordered(v[0], global<float>(d::k_9cf5c4)) && same_or_unordered(v[1], global<float>(d::k_9cf5c4)))
        return global<float>(d::flt_inf);
    const double z = v[2];  // fstp qword [ebp-8]
    const float xx = v[0] * v[0];
    const float yy = v[1] * v[1];
    const float sum = xx + yy;
    const double root = joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sum))));  // sqrt
    return static_cast<float>(Fpu{true}.div(z, root));  // fdivr qword: z / root
}

#define P1(name, addr, fn) JOAN_REPLACE("game/players1", name, addr, fn)
P1("PhysicMath::vector_tan_z_xy", 0x006accf0, &slope);
#undef P1

}  // namespace game_players1
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::grid::slope(void* a0) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_players1::slope>(a0)); }  // 0x6accf0
