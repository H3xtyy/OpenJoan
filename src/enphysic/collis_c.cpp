// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/collis_c.cpp: functions of the original d:\ww\enphysic\collis_c.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enphysic/collis_c.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
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

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b107_05.cpp (game/b107_05)
namespace {
namespace game_b107_05 {

using joan::call::cdecl_call;

using joan::call::import_call;

JOAN_COUNTER(c_collide, "collide_699940");

// ---- 0x00699940 (dead code) ----
struct Frame48 {
    u32 d[12];
};
struct Vec12 {
    u32 d[3];
};
struct Point20 {
    u32 d[5];
};
struct Basis24 {
    u32 d[6];
};
struct Matrix64 {
    u32 d[16];
};

u32 JOAN_CDECL collide_699940(void* a, Frame48 frame, Vec12 dir, Point20 point) {
    c_collide.hit();
    Basis24 basis{};
    Matrix64 m{};
    joan::orig::obb_to_box_frame(&basis, &m, &frame);                     // 0x006a2b80
    import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&point), &m);        // point_transform
    import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(&dir), &m);          // vector_transform
    Vec12 tmp{};
    const Vec12 neg = *joan::orig::arg::as<Vec12*>(joan::orig::vec_negate(&tmp, &dir));         // 0x0060cbc0
    // result unused: 0x00699940 has no caller (code or data)
    return joan::orig::arg::as<u32>(joan::orig::sweep_capsule_box(joan::orig::arg::mptr(a), joan::orig::arg::f32(point.d[0]), joan::orig::arg::f32(point.d[1]), joan::orig::arg::f32(point.d[2]), joan::orig::arg::f32(point.d[3]), joan::orig::arg::f32(point.d[4]),
                                                                  joan::orig::arg::f32(neg.d[0]), joan::orig::arg::f32(neg.d[1]), joan::orig::arg::f32(neg.d[2]),
                                                                  joan::orig::arg::f32(basis.d[0]), joan::orig::arg::f32(basis.d[1]), joan::orig::arg::f32(basis.d[2]), joan::orig::arg::f32(basis.d[3]), joan::orig::arg::f32(basis.d[4]), joan::orig::arg::f32(basis.d[5])));  // 0x00695d90
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_05", name, addr, fn)
B("box_cylinder_collision", 0x00699940, &collide_699940);
#undef B

}  // namespace game_b107_05
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

using joan::call::cdecl_call;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");
struct Vec4 {
    float x, y, z, w;
};
static_assert(sizeof(Vec4) == 16, "Vec4");

JOAN_COUNTER(c_ray_cyl, "ray_hits_cylinder_699a20");

// ------------------------------------------------------------------ 0x00699a20
static u8 JOAN_CDECL ray_hits_cylinder(float* out, Vec4 o, Vec3 d, Vec3 c, float r, float h) {
    c_ray_cyl.hit();
    if (!(o.w < 0.0f || o.w > 0.0f)) return 0;  // == 0 or NaN
    if (!(r < 0.0f || r > 0.0f)) return 0;
    if (!(h < 0.0f || h > 0.0f)) return 0;
    float t = 0.0f;  // written by every callee before it returns true
    {
        Vec4 o2{o.x, o.y, 0.0f, o.w};
        Vec3 d2{d.x, d.y, 0.0f};
        Vec4 c2{c.x, c.y, 0.0f, r};
        if ((joan::orig::arg::as<u32>(joan::orig::sphere_sweep(&t, &o2, &d2, &c2)) & 0xffu) == 0) return 0;  // 0x006a0d80
        const float s = d.z * t + o.z;
        const float lo = c.z - 0.5f * h;
        const float hi = 0.5f * h + c.z;
        if (!(lo > s) && !(s > hi)) {
            std::memcpy(out, &t, 4);
            return 1;
        }
    }
    Vec4 plane;
    joan::orig::as_plane_equation(&plane)->construct(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x00699480 (0, 0, 0, 0)
    // Cap plane hit (0x0069e970), inside the disc -> hit, else the rim test 0x0069e720 on the cap
    // circle. Returns 0 when the plane is missed, 1 on a hit, 2 when neither test hits.
    auto cap = [&](float centre_z) -> i32 {
        if ((joan::orig::arg::as<u32>(joan::orig::sphere_sweep_plane(&t, o.x, o.y, o.z, o.w, d.x, d.y, d.z, plane.x, plane.y, plane.z, plane.w)) & 0xffu) == 0) return 0;  // 0x0069e970
        const float dx = d.x * t + o.x - c.x;
        const float dy = d.y * t + o.y - c.y;
        const float dist2 = dx * dx + dy * dy;
        if (r * r >= dist2) return 1;  // jne on "r*r < dist2 or NaN"
        const Vec3 o3{o.x, o.y, o.z};
        const Vec3 cc{c.x, c.y, centre_z};
        if ((joan::orig::arg::as<u32>(joan::orig::sweep_hit_time_69e720(&t, o3.x, o3.y, o3.z, d.x, d.y, d.z, cc.x, cc.y, cc.z, r, o.w)) & 0xffu) != 0) return 1;  // 0x0069e720
        return 2;
    };
    plane.z = 1.0f;
    plane.w = -(0.5f * h + c.z);
    if (cap(-plane.w) == 1) {
        std::memcpy(out, &t, 4);
        return 1;
    }
    plane.z = -1.0f;
    plane.w = c.z - 0.5f * h;
    if (cap(plane.w) == 1) {
        std::memcpy(out, &t, 4);
        return 1;
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("sphere_cylinder_collision", 0x00699a20, &ray_hits_cylinder);
#undef B

}  // namespace game_b108_05
}  // namespace

