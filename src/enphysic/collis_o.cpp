// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/collis_o.cpp: functions of the original d:\ww\enphysic\collis_o.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enphysic/collis_o.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
// for it.

#include "../fp/fma.hpp"
#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using joan::call::cdecl_call;

using joan::call::import_call;

struct Vec {
    float x, y, z;
};
static_assert(sizeof(Vec) == 12, "vec3 by value");

JOAN_COUNTER(c_sweep, "sweep_hit_time_69e720");

// ------------------------------------------------------------------ 0x0069e720 (cdecl)
static u8 JOAN_CDECL sweep_hit_time(float* out, float ax, float ay, float az, float bx, float by, float bz, float cx,
                                 float cy, float cz, float r1, float r2) {
    c_sweep.hit();
    auto dot = [](const void* p, const void* q) { return static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, p, q)); };  // dot_product
    const Vec d{ax - cx, ay - cy, az - cz};
    const Vec b{bx, by, bz};
    const float f10 = (dot(&d, &d) + r1 * r1) - r2 * r2;
    const float d14 = dot(&d, &b);
    const float f14 = d14 + d14;
    const float f18 = dot(&b, &b);
    const Vec v24{d.x, d.y, 0.0f};
    const Vec v30{b.x, b.y, 0.0f};
    const float f58 = global<float>(d::flt_4) * r1 * r1;
    const float f34 = dot(&v24, &v24) * f58;
    const float f5c = global<float>(d::flt_8) * r1 * r1;
    const float f38 = dot(&v24, &v30) * f5c;
    const float f60 = global<float>(d::flt_4) * r1 * r1;
    const float f3c = dot(&v30, &v30) * f60;
    const float k2 = global<float>(d::flt_2);
    const float f40 = f10 * f10 - f34;
    const float f44 = k2 * f10 * f14 - f38;
    const float f48 = (f14 * f14 + k2 * f18 * f10) - f3c;
    const float f4c = k2 * f14 * f18;
    const float f50 = f18 * f18;
    if (!(f50 > global<float>(d::k_9cf5c4))) {  // test ah,0x41 je: jump only when f50 > 0
        *out = 0.0f;
        return 1;
    }
    const float t = joan::orig::arg::as<float>(joan::orig::solve_quartic_6a1460(joan::orig::arg::f32(f50), joan::orig::arg::f32(f4c), joan::orig::arg::f32(f48), joan::orig::arg::f32(f44), joan::orig::arg::f32(f40), 0.0f, 1.0f));  // 0x006a1460
    if (global<float>(d::k_9cf5c4) > t) return 0;
    if (t > global<float>(d::flt_1)) return 0;
    *out = t;
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("point_torus_collision", 0x0069e720, &sweep_hit_time);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

using joan::call::cdecl_call;

using joan::call::import_call;

struct V3f {
    float x, y, z;
};
static_assert(sizeof(V3f) == 12, "v3");
JOAN_COUNTER(c_sweep_plane, "sphere_sweep_plane");

// 0x0069e970
std::uint8_t JOAN_CDECL sphere_sweep_plane(float* out, float ax, float ay, float az, float r, float bx, float by, float bz,
                                        float nx, float ny, float nz, float d) {
    c_sweep_plane.hit();
    const float dist = ax * nx + ay * ny + az * nz + d;
    if (-r > dist) return 0;                          // test ah,0x41 / jne
    if (!(dist >= global<float>(d::k_9cf5c4))) return 0;  // test ah,1 / je
    if (!(dist > r)) {                                // test ah,0x41 / je
        *out = 0.0f;
        return 1;
    }
    const float den = bx * nx + by * ny + bz * nz;
    if (den >= global<float>(d::k_9cf5c4)) return 0;  // test ah,1 / jne continues
    *out = -(dist - r) / den;
    if (*out > global<float>(d::flt_1)) return 0;  // 1.0, test ah,0x41 / jne
    return 1;
}
#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("sphere_plane_collision", 0x0069e970, &sphere_sweep_plane);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::sweep_hit_time_69e720(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b108_14::sweep_hit_time>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }  // 0x69e720
auto joan::orig::sphere_sweep_plane(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b92_06::sphere_sweep_plane>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }  // 0x69e970
