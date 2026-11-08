// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/portal.cpp: functions of the original d:\ww\en3d\portal.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (en3d/portal.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../original/engine.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::cdecl_call;

using joan::call::thiscall;

using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using i32 = std::int32_t;

i32 ftol(float v) { return joan::hook::x87::ftol(static_cast<double>(v)); }

struct Vec {
    float x, y, z;
};
static_assert(sizeof(Vec) == 12, "vec3 by value");

JOAN_COUNTER(c_camset, "CamSet::apply_5e45a0");

struct H {
    // ------------------------------------------------------------------------ 0x005e45a0
    u8 camset_apply(void* key, void* src, void* cam) {
        c_camset.hit();
        if (joan::orig::arg::as<void*>(joan::orig::as_portal(this)->other_side(key)) == nullptr) return 0;  // 0x005e4460
        if (joan::orig::arg::as<u8>(joan::orig::as_portal(this)->facing(key, joan::orig::arg::mptr(src))) == 0) return 0;      // 0x005e44b0
        if (joan::orig::arg::as<i32>(joan::orig::as_camera(cam)->is_ready()) == 0) joan::orig::as_sys_en(cam)->init_613730();  // 0x00613ba0 / 0x00613730
        u32 info[10];
        joan::orig::as_camera(cam)->set_frame(joan::orig::arg::mptr(joan::orig::arg::as<void*>(joan::orig::as_camera(src)->frame(joan::orig::arg::mptr(static_cast<u32*>(info))))));  // 0x00614040(0x006140a0)
        joan::orig::as_camera(cam)->set_viewport(jo::as_camera(src)->target, jo::as_camera(src)->depth_target, joan::orig::arg::u32(jo::view<u32>(jo::as_camera(src)->view_x1)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(src)->view_y1)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(src)->view_x2)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(src)->view_y2)));  // 0x00613bf0
        joan::orig::as_camera(cam)->set_ranges(joan::orig::arg::f32(jo::view<u32>(jo::as_camera(src)->near_plane)), joan::orig::arg::f32(jo::view<u32>(jo::as_camera(src)->far_perspective)), joan::orig::arg::f32(jo::view<u32>(jo::as_camera(src)->far_ortho)));  // 0x00613ca0
        joan::orig::as_camera(cam)->set_cf(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(src)->clear_colour)));                         // 0x00613d30
        joan::orig::as_camera(cam)->set_scene(jo::as_camera(src)->scene);                                                                         // 0x00613db0
        jo::as_camera(cam)->u32_28 = jo::as_camera(src)->u32_28;
        {
            const i32 p40 = ftol(jo::as_camera(src)->angle_extra[1]);
            const i32 p3c = ftol(jo::as_camera(src)->angle_extra[0]);
            const i32 p38 = ftol(jo::as_camera(src)->angle_max[1]);
            const i32 p34 = ftol(jo::as_camera(src)->angle_max[0]);
            const i32 p30 = ftol(jo::as_camera(src)->angle_min[1]);
            const i32 p2c = ftol(jo::as_camera(src)->angle_min[0]);
            joan::orig::as_viewport(cam)->set_int(joan::orig::arg::i32(p2c), joan::orig::arg::i32(p30), joan::orig::arg::i32(p34), joan::orig::arg::i32(p38), joan::orig::arg::i32(p3c), joan::orig::arg::i32(p40));  // 0x00614110
        }
        u8 col[4];
        col[0] = jo::as_camera(src)->fog_colour.b;
        col[1] = jo::as_camera(src)->fog_colour.g;
        col[2] = jo::as_camera(src)->fog_colour.r;
        col[3] = jo::as_camera(src)->fog_colour.a;
        joan::orig::as_camera(cam)->set_light(joan::orig::as_camera(src)->light());  // 0x006169d0(0x00616b40)
        joan::orig::as_viewport(cam)->set_58(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_camera(src)->get_58())));  // 0x00616a60(0x00616b60)
        joan::orig::as_camera(cam)->set_fog_colour(joan::orig::arg::mptr(static_cast<u8*>(col)));           // 0x00616c10
        joan::orig::as_viewport(cam)->set_size(joan::orig::arg::i32(jo::view<u32>(jo::as_camera(src)->fog_start)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(src)->fog_end)));                      // 0x00616a00
        joan::orig::as_camera(cam)->set_fog_range(joan::orig::arg::u32(jo::view<u32>(jo::as_camera(src)->fog_density)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(src)->fog_height_b)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(src)->fog_height_a)));  // 0x00616a90
        joan::orig::as_camera(cam)->update_matrices();  // 0x006141a0
        joan::orig::as_camera(cam)->clear_planes();  // 0x00617120
        if (info[0] != 0) return 1;

        float plane[4];
        joan::orig::as_plane_equation(static_cast<float*>(plane))->init_425700();  // 0x00425700 (empty ctor)
        Vec t0{}, t1{}, t2{}, t3{};
        const Vec s1 = *joan::orig::arg::as<Vec*>(joan::orig::vec_add(&t0, joan::orig::arg::ptr(jo::self<jo::Portal>(this)->corners), joan::orig::arg::ptr(jo::self<jo::Portal>(this)->corners + 3)));    // 0x0060cc20
        const Vec s2 = *joan::orig::arg::as<Vec*>(joan::orig::vec_add(&t1, &s1, joan::orig::arg::ptr(jo::self<jo::Portal>(this)->corners + 6)));    // 0x0060cc20
        const Vec s3 = *joan::orig::arg::as<Vec*>(joan::orig::vec_add(&t2, &s2, joan::orig::arg::ptr(jo::self<jo::Portal>(this)->corners + 9)));    // 0x0060cc20
        const Vec centre = *joan::orig::arg::as<Vec*>(joan::orig::vec_scale(&t3, &s3, 0.25f));         // 0x0060cd00
        auto make = [&](std::int32_t p, std::int32_t q) {  // corners p, q
            joan::orig::plane_from_points(joan::orig::arg::mptr(static_cast<float*>(plane)), &info[1], joan::orig::arg::ptr(jo::self<jo::Portal>(this)->corners + p * 3), joan::orig::arg::ptr(jo::self<jo::Portal>(this)->corners + q * 3));  // 0x0060d920
        };
        make(0, 1);
        const double dist = joan::orig::arg::as<double>(joan::orig::plane_distance(joan::orig::arg::mptr(static_cast<float*>(plane)), joan::orig::arg::mptr(&centre)));  // 0x0060da30
        const bool flip = !(dist >= static_cast<double>(global<float>(d::k_9cf5c4)));
        auto emit = [&]() {
            if (flip) {
                plane[0] = -plane[0];
                plane[1] = -plane[1];
                plane[2] = -plane[2];
                plane[3] = -plane[3];
            }
            joan::orig::as_camera(cam)->push_plane(joan::orig::arg::mptr(static_cast<float*>(plane)));  // 0x006170a0
        };
        emit();
        make(1, 2);
        emit();
        make(2, 3);
        emit();
        make(3, 0);
        emit();
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Portal::create_view_through_portal", 0x005e45a0, &H::camset_apply);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {

// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    // --- Portal (between plane_sh.cpp and scene.cpp) -----------------------------------------
    // 0x005e4b00: the portal's box (+0x40) against the camera frustum.
    bool portal_box_visible(void* camera) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(camera)->box_visible(joan::orig::arg::mptr(jo::self<jo::Portal>(this)->box))) != 0; }
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Portal::is_inside_camera", 0x005e4b00, &H::portal_box_visible);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
namespace {
namespace game_b102_10 {

JOAN_COUNTER(c_set_sides, "Portal::set_sides");

struct H {
    // 0x005e4b40
    u8 set_sides(u8* a, u8 ia, u8* b, u8 ib) {
        c_set_sides.hit();
        if (a == nullptr || b == nullptr) return 0;
        joan::orig::as_scene(a)->push(this);  // 0x005f35f0
        joan::orig::as_scene(b)->push(this);
        u32 center[3];
        void* const pc = joan::orig::arg::as<void*>(joan::orig::as_box(a + 8)->centre(joan::orig::arg::mptr(static_cast<void*>(center))));  // 0x0060d3a0
        // 0x0060da30 plane distance, st0 taken unrounded
        const double d = joan::orig::arg::as<double>(joan::orig::plane_distance(joan::orig::arg::mptr(jo::self<jo::Portal>(this)->plane), joan::orig::arg::mptr(pc)));
        if (d >= 0.0) {  // fcomp 0.0, test ah,1 jne -> d < 0 or unordered goes to the else branch
            jo::view<u8*>(jo::self<jo::Portal>(this)->side_a) = a;
            jo::self<jo::Portal>(this)->open_b = ia;
            jo::view<u8*>(jo::self<jo::Portal>(this)->side_b) = b;
            jo::self<jo::Portal>(this)->open_a = ib;
        } else {
            jo::view<u8*>(jo::self<jo::Portal>(this)->side_a) = b;
            jo::self<jo::Portal>(this)->open_b = ib;
            jo::view<u8*>(jo::self<jo::Portal>(this)->side_b) = a;
            jo::self<jo::Portal>(this)->open_a = ia;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("Portal::link_to_sector", 0x005e4b40, &H::set_sides);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/game_b101_11.cpp (game/b101_11)
namespace {
namespace game_b101_11 {

using joan::call::import_call;

void copy3(void* dst, const void* src) { std::memcpy(dst, src, 12); }

JOAN_COUNTER(c_tetra_set, "Tetra::set");

struct H {
    // ------------------------------------------------------------------ 0x005e4c30
    std::uintptr_t tetra_set(u8* a, u8* b, u8* c, u8* d) {
        c_tetra_set.hit();
        u32 tmp[3];
        u32 e0[3], e1[3], n0[3], e2[3], e3[3], n1[3];
        copy3(e0, joan::orig::arg::as<void*>(joan::orig::vec_sub(static_cast<void*>(tmp), joan::orig::arg::ptr(c), joan::orig::arg::ptr(b))));  // 0x0060cc90
        copy3(e1, joan::orig::arg::as<void*>(joan::orig::vec_sub(static_cast<void*>(tmp), joan::orig::arg::ptr(b), joan::orig::arg::ptr(a))));
        import_call<void*>(d::iat_v3d_87_cross_product, static_cast<void*>(n0), static_cast<void*>(e1), static_cast<void*>(e0));
        copy3(e2, joan::orig::arg::as<void*>(joan::orig::vec_sub(static_cast<void*>(tmp), joan::orig::arg::ptr(d), joan::orig::arg::ptr(c))));
        copy3(e3, joan::orig::arg::as<void*>(joan::orig::vec_sub(static_cast<void*>(tmp), joan::orig::arg::ptr(c), joan::orig::arg::ptr(b))));
        import_call<void*>(d::iat_v3d_87_cross_product, static_cast<void*>(n1), static_cast<void*>(e3), static_cast<void*>(e2));
        const double dot = import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(n0), static_cast<void*>(n1));
        if (!(dot >= static_cast<double>(global<float>(d::k_9cf5c4)))) {  // test ah,1 jne
            copy3(jo::self<jo::Portal>(this)->corners + 0, a);
            copy3(jo::self<jo::Portal>(this)->corners + 3, c);
            copy3(jo::self<jo::Portal>(this)->corners + 6, b);
            copy3(jo::self<jo::Portal>(this)->corners + 9, d);
        } else {
            copy3(jo::self<jo::Portal>(this)->corners + 0, a);
            copy3(jo::self<jo::Portal>(this)->corners + 3, b);
            copy3(jo::self<jo::Portal>(this)->corners + 6, c);
            copy3(jo::self<jo::Portal>(this)->corners + 9, d);
        }
        joan::orig::plane_from_points(joan::orig::arg::mptr(jo::self<jo::Portal>(this)->plane), joan::orig::arg::ptr(jo::self<jo::Portal>(this)->corners), joan::orig::arg::ptr(jo::self<jo::Portal>(this)->corners + 3), joan::orig::arg::ptr(jo::self<jo::Portal>(this)->corners + 6));  // 0x0060d920
        joan::orig::as_aabb(jo::self<jo::Portal>(this)->box)->set_point(static_cast<void*>(jo::self<jo::Portal>(this)->corners));          // 0x0060d430
        joan::orig::as_aabb(jo::self<jo::Portal>(this)->box)->extend(static_cast<void*>(jo::self<jo::Portal>(this)->corners + 3));    // 0x0060d490
        joan::orig::as_aabb(jo::self<jo::Portal>(this)->box)->extend(static_cast<void*>(jo::self<jo::Portal>(this)->corners + 6));
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_aabb(jo::self<jo::Portal>(this)->box)->extend(static_cast<void*>(jo::self<jo::Portal>(this)->corners + 9)));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("Portal::set_portal_position", 0x005e4c30, &H::tetra_set);
#undef B

}  // namespace game_b101_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2_2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2_2 {

using joan::call::cdecl_call;

using joan::call::thiscall;

using joan::call::thiscall;
using joan::call::cdecl_call;
bool below_or_nan(float a, float b) { return !(a >= b); }  // x87 C0

// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    // 0x005e44b0: seen from `sector`, does the portal face the camera (+0xa4 position), and is
    // its box in the frustum? +0x30 plane, +0x58 / +0x5c sectors, +0x60 / +0x61 open per side.
    bool portal_facing(void* sector, void* camera) {
        const float* const eye = jo::as_camera(camera)->params.position;
        if (sector == jo::self<jo::Portal>(this)->side_a) {
            if (jo::self<jo::Portal>(this)->open_a == 0) return false;
            const float d = joan::orig::arg::as<float>(joan::orig::plane_distance(joan::orig::arg::mptr(jo::self<jo::Portal>(this)->plane), joan::orig::arg::mptr(eye)));  // 0x0060da30
            if (!(d > 0.0f)) return false;
        } else if (sector == jo::self<jo::Portal>(this)->side_b) {
            if (jo::self<jo::Portal>(this)->open_b == 0) return false;
            const float d = joan::orig::arg::as<float>(joan::orig::plane_distance(joan::orig::arg::mptr(jo::self<jo::Portal>(this)->plane), joan::orig::arg::mptr(eye)));
            if (!below_or_nan(d, 0.0f)) return false;
        }
        return joan::orig::arg::as<bool>(joan::orig::as_portal(this)->box_visible(joan::orig::arg::mptr(camera)));  // 0x005e4b00
    }
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Portal::can_see_another_sector", 0x005e44b0, &H::portal_facing);
#undef HELPER2

}  // namespace en3d_helpers2_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Portal::facing(void* a0, void* a1) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::en3d_helpers2_2::H::portal_facing>(this, a0, a1)); }  // 0x5e44b0
auto joan::orig::Portal::box_visible(void* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::en3d_helpers2::H::portal_box_visible>(this, a0)); }  // 0x5e4b00
