// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/collis_b.cpp: functions of the original d:\ww\enphysic\collis_b.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enphysic/collis_b.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
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

using i32 = std::int32_t;

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b106_08.cpp (game/b106_08)
namespace {
namespace game_b106_08 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_sweep, "sweep_sphere_box_697140");

// ---------------------------------------------------------------------------
// 0x00697140 sweep_sphere_box
// ---------------------------------------------------------------------------
struct Vec3 {
    float v[3];
};
struct Plane {
    float v[4];
};
static_assert(sizeof(Vec3) == 12 && sizeof(Plane) == 16, "by-value sizes");

Vec3 vec(const float* p) {
    Vec3 r;
    std::memcpy(&r, p, 12);
    return r;
}
float axis_at(float t, const float* sph, const float* dir, i32 k) {
    const float m = t * dir[k];
    return m + sph[k];
}

// Face: plane (nx, ny, nz, d) through 0x00699480, ray / plane 0x0069e640; on a hit the two
// other coordinates must lie inside the box (a NaN passes, as the original's compares).
bool face(float& t, const float* sph, const float* dir, const float* box, u32 nx, u32 ny, u32 nz, float d,
          i32 k1, i32 k2) {
    Plane tmp;
    Plane* const made = joan::orig::arg::as<Plane*>(joan::orig::as_plane_equation(&tmp)->construct(joan::orig::arg::u32(nx), joan::orig::arg::u32(ny), joan::orig::arg::u32(nz), joan::orig::arg::bits(d)));  // 0x00699480
    const Plane pl = *made;
    const u32 hit = joan::orig::arg::as<u32>(joan::orig::ray_plane(&t, vec(sph), vec(dir), pl));  // 0x0069e640
    if ((hit & 0xff) == 0) return false;
    const float c1 = axis_at(t, sph, dir, k1);
    const float c2 = axis_at(t, sph, dir, k2);
    if (box[k1] > c1) return false;
    if (c1 > box[k1 + 3]) return false;
    if (box[k2] > c2) return false;
    if (c2 > box[k2 + 3]) return false;
    return true;
}
// Edge: cylinder test 0x006a09e0 at point (px, py, pz) along `axis`; the hit's coordinate k
// must lie in [box[lo], box[lo + 3]].
bool edge(float& t, const float* sph, const float* dir, const float* box, i32 px, i32 py, i32 pz, i32 axis, i32 k,
          i32 lo) {
    Vec3 p;
    std::memcpy(&p.v[0], &box[px], 4);
    std::memcpy(&p.v[1], &box[py], 4);
    std::memcpy(&p.v[2], &box[pz], 4);
    const u32 radius = jo::view<u32>(sph[3]);
    const u32 hit = joan::orig::arg::as<u32>(joan::orig::sphere_sweep_cylinder(&t, sph[0], sph[1], sph[2], dir[0], dir[1], dir[2], p.v[0], p.v[1], p.v[2],
                                                                            joan::orig::arg::f32(radius), axis));  // 0x006a09e0
    if ((hit & 0xff) == 0) return false;
    const float c = axis_at(t, sph, dir, k);
    if (box[lo] > c) return false;
    if (c > box[lo + 3]) return false;
    return true;
}
// Corner: sphere test 0x006a0f90 at (px, py, pz). The callee takes a pointer to a 16-byte
// sphere {x, y, z, radius} and reads the radius at +0xc (the original fills [ebp-0x84]).
bool corner(float& t, const float* sph, const float* dir, const float* box, i32 px, i32 py, i32 pz) {
    Plane p;
    std::memcpy(&p.v[0], &box[px], 4);
    std::memcpy(&p.v[1], &box[py], 4);
    std::memcpy(&p.v[2], &box[pz], 4);
    std::memcpy(&p.v[3], &sph[3], 4);
    const u32 hit = joan::orig::arg::as<u32>(joan::orig::shape::sphere_hit(&t, vec(sph), vec(dir), &p));  // 0x006a0f90
    return (hit & 0xff) != 0;
}

constexpr u32 kM1 = 0xbf800000u;  // -1.0f
constexpr u32 kP1 = 0x3f800000u;  // 1.0f

bool JOAN_CDECL sweep_sphere_box(float* out, const float* sph, const float* dir, const float* box) {
    c_sweep.hit();
    Plane local;
    joan::orig::as_plane_equation(&local)->init_425700();  // 0x00425700 (empty ctor of the plane local)
    float t = 0.0f;                       // written by every test before a hit is reported
    const float r = sph[3];
    bool hit = false;
    // faces (box: 0..2 min, 3..5 max)
    if (!hit) hit = face(t, sph, dir, box, kM1, 0, 0, box[0] - r, 1, 2);
    if (!hit) hit = face(t, sph, dir, box, kP1, 0, 0, -box[3] - r, 1, 2);
    if (!hit) hit = face(t, sph, dir, box, 0, kM1, 0, box[1] - r, 0, 2);
    if (!hit) hit = face(t, sph, dir, box, 0, kP1, 0, -box[4] - r, 0, 2);
    if (!hit) hit = face(t, sph, dir, box, 0, 0, kM1, box[2] - r, 0, 1);
    if (!hit) hit = face(t, sph, dir, box, 0, 0, kP1, -box[5] - r, 0, 1);
    // edges along z (the original checks the x coordinate against the z range)
    if (!hit) hit = edge(t, sph, dir, box, 0, 1, 2, 2, 0, 2);
    if (!hit) hit = edge(t, sph, dir, box, 3, 1, 2, 2, 0, 2);
    if (!hit) hit = edge(t, sph, dir, box, 3, 4, 2, 2, 0, 2);
    if (!hit) hit = edge(t, sph, dir, box, 0, 4, 2, 2, 0, 2);
    // edges along y
    if (!hit) hit = edge(t, sph, dir, box, 0, 1, 2, 1, 1, 1);
    if (!hit) hit = edge(t, sph, dir, box, 3, 1, 2, 1, 1, 1);
    if (!hit) hit = edge(t, sph, dir, box, 3, 1, 5, 1, 1, 1);
    if (!hit) hit = edge(t, sph, dir, box, 0, 1, 5, 1, 1, 1);
    // edges along x
    if (!hit) hit = edge(t, sph, dir, box, 0, 1, 2, 0, 0, 0);
    if (!hit) hit = edge(t, sph, dir, box, 0, 4, 2, 0, 0, 0);
    if (!hit) hit = edge(t, sph, dir, box, 0, 4, 5, 0, 0, 0);
    if (!hit) hit = edge(t, sph, dir, box, 0, 1, 5, 0, 0, 0);
    // corners
    if (!hit) hit = corner(t, sph, dir, box, 0, 1, 2);
    if (!hit) hit = corner(t, sph, dir, box, 3, 1, 2);
    if (!hit) hit = corner(t, sph, dir, box, 3, 4, 2);
    if (!hit) hit = corner(t, sph, dir, box, 0, 4, 2);
    if (!hit) hit = corner(t, sph, dir, box, 0, 1, 5);
    if (!hit) hit = corner(t, sph, dir, box, 3, 1, 5);
    if (!hit) hit = corner(t, sph, dir, box, 3, 4, 5);
    if (!hit) hit = corner(t, sph, dir, box, 0, 4, 5);
    if (!hit) return false;
    std::memcpy(out, &t, 4);
    return true;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b106_08", name, addr, fn)
B("sphere_box_collision", 0x00697140, &sweep_sphere_box);
#undef B

}  // namespace game_b106_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::sweep_sphere_box_697140(void* a0, void* a1, void* a2, void* a3) -> bool { return joan::bind::conv<bool>(joan::bind::free<&::game_b106_08::sweep_sphere_box>(a0, a1, a2, a3)); }  // 0x697140
