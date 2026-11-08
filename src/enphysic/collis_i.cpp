// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/collis_i.cpp: functions of the original d:\ww\enphysic\collis_i.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enphysic/collis_i.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
// for it.

#include "../fp/fma.hpp"
#include "../original/helpers.hpp"
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

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

using joan::call::cdecl_call;

struct V3 {
    float x, y, z;
};
struct Plane {  // a, b, c, d
    float v[4];
};
struct Cyl {  // centre x, y, z, radius, height
    float v[5];
};
struct Box {  // min, max
    float v[6];
};
static_assert(sizeof(V3) == 12 && sizeof(Plane) == 16 && sizeof(Cyl) == 20 && sizeof(Box) == 24, "sizes");
static_assert(sizeof(Cyl) + 2 * sizeof(Plane) + sizeof(Box) == 0x4c, "box args");
static_assert(sizeof(Cyl) + 2 * sizeof(Plane) + sizeof(Cyl) == 0x48, "cylinder args");
static_assert(sizeof(Cyl) + 3 * sizeof(Plane) == 0x44, "sphere args");

JOAN_COUNTER(c_box, "Collide::cylinder_box");

constexpr float kZero = 0.0f;  // 0x9cf5c4
constexpr float kHalf = 0.5f;  // 0x9cf77c
constexpr float kOne = 1.0f;   // 0x9cf784

using joan::orig::helpers::plane_distance;
float sqrt_f(float x) { return joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); }    // 0x004256d0 (stored)
double sqrt_d(float x) { return joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); }  // 0x004256d0 (st0 used)
// 0x006a1250 line (a, b, d) x edge line (ex, ey, ec) intersection; writes *x / *y on success.
std::uint8_t intersect(float* x, float* y, float a, float b, float d, float ex, float ey, float ec) {
    return joan::orig::arg::as<std::uint8_t>(joan::orig::solve2x2(joan::orig::arg::mptr(x), joan::orig::arg::mptr(y), joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(d), joan::orig::arg::f32(ex), joan::orig::arg::f32(ey), joan::orig::arg::f32(ec)));
}

// One clip pass of 0x0069c0f0 against the 2D line (a, b, d) of a plane.
// Returns false when every point is outside (the original returns 0).
// z_one: second pass (new points get z += 1.0 instead of z = 0).
bool clip(V3* p, std::int32_t& n, const Plane& plane, std::int32_t* flags, bool z_one) {
    std::int32_t out = 0;
    for (std::int32_t k = 0; k < n; ++k) {
        flags[k] = !(plane_distance(&plane, &p[k]) >= kZero) ? 1 : 0;
        if (flags[k] != 0) ++out;
    }
    if (out == n) return false;
    if (out == 0) return true;
    V3 copy[7];
    std::memcpy(copy, p, 0x48);
    const std::int32_t m = n;
    n = 0;
    for (std::int32_t i = 0; i < m; ++i) {
        if (flags[i] == 0) {
            p[n] = copy[i];
            ++n;
            continue;
        }
        const std::int32_t prev = (i + m - 1) % m;
        if (flags[prev] == 0) {
            const float ex = copy[i].y - copy[prev].y;
            const float ey = copy[prev].x - copy[i].x;
            const float ec = (copy[i].x - copy[prev].x) * copy[prev].y - (copy[i].y - copy[prev].y) * copy[prev].x;
            if (intersect(&p[n].x, &p[n].y, plane.v[0], plane.v[1], plane.v[3], ex, ey, ec) != 0) {
                p[n].z = z_one ? p[n].z + kOne : 0.0f;
                ++n;
            }
        }
        const std::int32_t next = (i + 1) % m;
        if (flags[next] == 0) {
            const float ex = copy[i].y - copy[next].y;
            const float ey = copy[next].x - copy[i].x;
            const float ec = (copy[i].x - copy[next].x) * copy[next].y - (copy[i].y - copy[next].y) * copy[next].x;
            if (intersect(&p[n].x, &p[n].y, plane.v[0], plane.v[1], plane.v[3], ex, ey, ec) != 0) {
                p[n].z = z_one ? p[n].z + kOne : 0.0f;
                ++n;
            }
        }
    }
    return true;
}

// 0x0069c0f0
std::uint8_t JOAN_CDECL cylinder_box(Cyl c, Plane a, Plane b, Box s) {
    c_box.hit();
    V3 t1, t2;
    const V3 sum = *joan::orig::arg::as<V3*>(joan::orig::vec_add(&t1, &s.v[0], &s.v[3]));  // 0x0060cc20 add
    const V3 mid = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t2, &sum, joan::orig::arg::f32(kHalf)));     // 0x0060cd00 scale
    Cyl bound;
    bound.v[0] = mid.x;
    bound.v[1] = mid.y;
    bound.v[2] = mid.z;
    bound.v[3] = sqrt_f((mid.x - s.v[0]) * (mid.x - s.v[0]) + (mid.y - s.v[1]) * (mid.y - s.v[1]));
    bound.v[4] = s.v[5] - s.v[2];
    if (joan::orig::cylinders_overlap(c.v[0], c.v[1], c.v[2], c.v[3], c.v[4], bound.v[0], bound.v[1], bound.v[2], bound.v[3], bound.v[4]) == 0) return 0;  // 0x0069d160

    const float r = c.v[3];
    const float rad = bound.v[3];
    const float da = c.v[0] * a.v[0] + c.v[1] * a.v[1] + a.v[3];
    const float db = c.v[0] * b.v[0] + c.v[1] * b.v[1] + b.v[3];
    if (-r > da) return 0;
    if (-r > db) return 0;
    if (-rad > mid.x * a.v[0] + mid.y * a.v[1] + a.v[3]) return 0;
    if (-rad > mid.x * b.v[0] + mid.y * b.v[1] + b.v[3]) return 0;
    const std::uint8_t beyond_a = da > r ? 1 : 0;
    const std::uint8_t beyond_b = db > r ? 1 : 0;

    V3 p[9] = {};  // second clip: up to 3 inside + 6 cut points
    std::int32_t flags[9] = {};
    std::int32_t n = 4;
    p[0] = V3{s.v[0], s.v[1], 0.0f};
    p[1] = V3{s.v[0], s.v[4], 0.0f};
    p[2] = V3{s.v[3], s.v[4], 0.0f};
    p[3] = V3{s.v[3], s.v[1], 0.0f};
    if (beyond_a == 0 && !clip(p, n, a, flags, false)) return 0;
    if (beyond_b == 0 && !clip(p, n, b, flags, true)) return 0;

    std::uint8_t any = 0;
    for (std::int32_t k = 0; k < n; ++k) {
        const std::int32_t j = (k + 1) % n;
        const float ex = p[j].y - p[k].y;
        const float ey = p[k].x - p[j].x;
        const float ec = (p[j].x - p[k].x) * p[k].y - (p[j].y - p[k].y) * p[k].x;
        const float num = ex * c.v[0] + ey * c.v[1] + ec;
        const double len = sqrt_d(ex * ex + ey * ey);
        const float dist = static_cast<float>(joan::hook::x87::Fpu{true}.div(num, len));
        if (-r > dist) return 0;
        if (!(dist >= kZero)) {
            flags[k] = 1;
            any = 1;
        } else {
            flags[k] = 0;
        }
    }
    if (any == 0) return 1;
    for (std::int32_t k = 0; k < n; ++k) {
        if (flags[k] == 0) continue;
        if (flags[(k + n - 1) % n] == 0) continue;
        const float dx = p[k].x - c.v[0];
        const float dy = p[k].y - c.v[1];
        if (!(r * r >= dx * dx + dy * dy)) return 0;
    }
    return 1;
}
#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("cylinder_box_intersection", 0x0069c0f0, &cylinder_box);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {

using joan::call::cdecl_call;

// 0x0069d160 two vertical cylinders (x, y, z, radius, height) overlap.
std::uint8_t JOAN_CDECL cylinders_overlap(float a0, float a1, float a2, float a3, float a4, float b0, float b1, float b2, float b3,
                                       float b4) {
    const float dx = a0 - b0;
    const float dy = a1 - b1;
    const float r = a3 + b3;
    const float d2 = dx * dx + dy * dy;
    const float r2 = r * r;
    if (!(r2 >= d2)) return 0;  // r2 < d2 or unordered
    const double dz = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(a2 - b2)));  // 0x00427d60 fabs
    const float h = (a4 + b4) * 0.5f;                            // 0x009cf77c
    if (!(static_cast<double>(h) >= dz)) return 0;
    return 1;
}
#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("cylinder_cylinder_intersection", 0x0069d160, &cylinders_overlap);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------------------------------
// Collision shapes (same layouts as game_b89_02).
struct Sphere {
    float x, y, z, r;
};
static_assert(sizeof(Sphere) == 16, "sphere");
// 0x0069d540
std::uint8_t JOAN_CDECL sphere_sphere(Sphere a, Sphere b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float dz = a.x - b.z;  // sic
    const float rs = a.r + b.r;
    const float sum = dx * dx + dy * dy + dz * dz;
    return (rs * rs >= sum) ? 1 : 0;  // fcompp, C0 set (less or unordered) -> 0
}
#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("sphere_sphere_intersection", 0x0069d540, &sphere_sphere);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::cylinders_overlap(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b84_04::cylinders_overlap>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x69d160

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

struct Capsule {  // point + radius (a) + height (b)
    float x, y, z, a, b;
};

}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02_2 {


// ---------------------------------------------------------------------------------------------
// Collision shapes (same layouts as game_b89_02).
struct Sphere {
    float x, y, z, r;
};
static_assert(sizeof(Sphere) == 16, "sphere");
static_assert(sizeof(Capsule) == 20, "capsule");
struct Box {  // min xyz, max xyz
    float x0, y0, z0, x1, y1, z1;
};
static_assert(sizeof(Box) == 24, "box");

const float kHalf = 0.5f;  // 0x009cf77c

JOAN_COUNTER(c_capsule_box, "capsule_box");
JOAN_COUNTER(c_capsule_sphere, "capsule_sphere");

// 0x0069d390
std::uint8_t JOAN_CDECL capsule_box(Capsule c, Box b) {
    c_capsule_box.hit();
    const float lo = c.z - c.b * kHalf;
    const float hi = c.b * kHalf + c.z;
    if (!(hi >= b.z0)) return 0;  // test ah,1 jne: less or unordered
    if (!(b.z1 >= lo)) return 0;
    const float x0 = b.x0 - c.a;
    const float x1 = b.x1 + c.a;
    const float y0 = b.y0 - c.a;
    const float y1 = b.y1 + c.a;
    if (x0 > c.x) return 0;
    if (c.x > x1) return 0;
    if (y0 > c.y) return 0;
    if (c.y > y1) return 0;
    float cx, cy;
    if (!(c.x >= b.x0))
        cx = b.x0;
    else if (!(b.x1 >= c.x))
        cx = b.x1;
    else
        return 1;
    if (!(c.y >= b.y0))
        cy = b.y0;
    else if (!(b.y1 >= c.y))
        cy = b.y1;
    else
        return 1;
    const float dx = c.x - cx;
    const float dy = c.y - cy;
    const float sum = dx * dx + dy * dy;
    return (c.a * c.a >= sum) ? 1 : 0;
}

float sqrt_f(float x) { return joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); }  // 0x004256d0 (stored)

// 0x0069d210
std::uint8_t JOAN_CDECL capsule_sphere(Capsule c, Sphere s) {
    c_capsule_sphere.hit();
    const float lo = c.z - c.b * kHalf;
    const float hi = c.b * kHalf + c.z;
    float extra;
    if (!(lo > s.z) && !(s.z > hi)) {
        extra = s.r;
    } else if (!(hi >= s.z) && (hi + s.r > s.z)) {
        const float d = s.z - hi;
        extra = sqrt_f(s.r * s.r - d * d);
    } else if (!(lo - s.r >= s.z) && !(s.z >= lo)) {
        const float d = lo - s.z;
        extra = sqrt_f(s.r * s.r - d * d);
    } else {
        return 0;
    }
    const float dx = c.x - s.x;
    const float dy = c.y - s.y;
    const float rr = c.a + extra;
    const float sum = dx * dx + dy * dy;
    return (rr * rr >= sum) ? 1 : 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("cylinder_box_intersection_69d390", 0x0069d390, &capsule_box);
B("cylinder_sphere_intersection", 0x0069d210, &capsule_sphere);
#undef B

}  // namespace game_b90_02_2
}  // namespace
