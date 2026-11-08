// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/collisio.cpp: functions of the original d:\ww\enphysic\collisio.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enphysic/Collisio.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
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

struct Capsule {  // point + radius (a) + height (b)
    float x, y, z, a, b;
};
struct Matrix {
    float m[16];
};

JOAN_COUNTER(c_overlap, "shapes_overlap");

}  // namespace

// ---------------------------------------- from modules/game_b89_02.cpp (game/b89_02)
namespace {
namespace game_b89_02 {

using joan::call::cdecl_call;

using joan::call::import_call;

// ---------------------------------------------------------------------------------------------
// 0x006a31d0 shapes
struct Sphere {  // 16 bytes: centre + radius
    float x, y, z, r;
};
static_assert(sizeof(Sphere) == 16, "sphere");
static_assert(sizeof(Capsule) == 20, "capsule");
struct Box {  // 24 bytes, copied raw from +0xcc
    std::uint32_t d[6];
};
static_assert(sizeof(Box) == 24, "box");
struct Obb {  // 48 bytes, filled by 0x006a30c0
    std::uint32_t d[12];
};
static_assert(sizeof(Obb) == 48, "obb");
static_assert(sizeof(Matrix) == 64, "matrix");

void* node(const void* s) { return jo::view<void*>(jo::as_actor(s)->bone); }
void node_update(const void* s) { joan::orig::as_bone(node(s))->update(); }  // 0x0060ee60
Matrix* node_matrix(const void* s) { return joan::orig::arg::as<Matrix*>(joan::orig::as_bone(node(s))->matrix()); }  // 0x0060ef80
std::int32_t kind(const void* s) { return jo::view<std::int32_t>(jo::as_actor(s)->aim_mode); }
float scale(const void* s) { return jo::as_actor(s)->scale; }
Box box_of(const void* s) {
    Box b;
    std::memcpy(&b, jo::as_actor(s)->aim_box, sizeof b);
    return b;
}
void transform3(void* out, const void* in, const Matrix* m) {
    import_call<void*>(d::iat_v3d_87_point_transform_3, out, in, m);  // point_transform(out, in, m)
}
void transform_inplace(void* v, const Matrix* m) {
    import_call<void*>(d::iat_v3d_87_point_transform_2, v, m);  // point_transform(v, m)
}
void inverse(Matrix* out, const Matrix* in) { import_call<void*>(d::iat_v3d_87_matrix_inverse_2, out, in); }  // matrix_inverse
void multiply(Matrix* out, const Matrix* a, const Matrix* b) {
    import_call<void*>(d::iat_v3d_87_matrix_multiply_3, out, a, b);  // matrix_multiply(out, a, b)
}

// node update + world matrix + point_transform of the vec3 at +off of s into out.
template <auto Member>  // a point member of the actor s (aim_sphere, aim_box, collide_kind: the cylinder centre)
void world_point(void* out, const void* s) {
    node_update(s);
    Matrix* const m = node_matrix(s);
    transform3(out, &(jo::as_actor(s)->*Member), m);
}
// node update + inverse world matrix of s applied in place to v.
void to_local(void* v, const void* s) {
    node_update(s);
    Matrix* const m = node_matrix(s);
    Matrix inv;
    inverse(&inv, m);
    transform_inplace(v, &inv);
}

// 0x006a31d0
std::uint8_t JOAN_CDECL shapes_overlap(void* a, void* b) {
    c_overlap.hit();
    switch (kind(a)) {
    case 0:
        return 0;
    case 1:
        switch (kind(b)) {
        case 0:
            return 0;
        case 1: {  // 0x006a323c sphere / sphere
            Sphere sa, sb;
            world_point<&jo::Actor::aim_sphere>(&sa, a);
            sa.r = jo::as_actor(a)->aim_sphere[3] * scale(a);
            world_point<&jo::Actor::aim_sphere>(&sb, b);
            sb.r = jo::as_actor(b)->aim_sphere[3] * scale(b);
            return joan::orig::arg::as<std::uint8_t>(joan::orig::sphere_sphere(sa, sb));  // 0x0069d540
        }
        case 2: {  // 0x006a330d sphere / box
            Sphere s;
            world_point<&jo::Actor::aim_sphere>(&s, a);
            s.r = jo::as_actor(a)->aim_sphere[3] * scale(a) / scale(b);
            to_local(&s, b);
            return joan::orig::arg::as<std::uint8_t>(joan::orig::sphere_box(s, box_of(b)));  // 0x0069d5d0
        }
        case 3: {  // 0x006a33d2 sphere / capsule
            Sphere s;
            world_point<&jo::Actor::aim_sphere>(&s, a);
            s.r = jo::as_actor(a)->aim_sphere[3] * scale(a);
            Capsule c;
            world_point<&jo::Actor::collide_kind>(&c, b);
            c.a = jo::as_actor(b)->collide_radius * scale(b);
            c.b = jo::as_actor(b)->collide_height * scale(b);
            return joan::orig::arg::as<std::uint8_t>(joan::orig::capsule_sphere(c, s));  // 0x0069d210
        }
        }
        [[fallthrough]];  // no default in the original: falls into case 2
    case 2:
        switch (kind(b)) {
        case 0:
            return 0;
        case 1: {  // 0x006a34e4 box / sphere
            Sphere s;
            world_point<&jo::Actor::aim_sphere>(&s, b);
            s.r = jo::as_actor(b)->aim_sphere[3] * scale(b) / scale(a);
            to_local(&s, a);
            return joan::orig::arg::as<std::uint8_t>(joan::orig::sphere_box(s, box_of(a)));  // 0x0069d5d0
        }
        case 2: {  // 0x006a35c4 box / box
            node_update(a);
            node_update(b);
            Matrix inv_b;
            inverse(&inv_b, node_matrix(b));
            Matrix* const ma = node_matrix(a);
            Matrix rel;
            multiply(&rel, ma, &inv_b);
            Obb o;
            joan::orig::obb_from_box(joan::orig::arg::mptr(static_cast<void*>(&o)), joan::orig::arg::mptr(static_cast<void*>(jo::as_actor(a)->aim_box)), joan::orig::arg::mptr(static_cast<void*>(&rel)));  // 0x006a30c0
            return joan::orig::arg::as<std::uint8_t>(joan::orig::obb_box(o, box_of(b)));  // 0x0069e4e0
        }
        case 3: {  // 0x006a3679 box / capsule
            Capsule c;
            world_point<&jo::Actor::collide_kind>(&c, b);
            c.a = jo::as_actor(b)->collide_radius * scale(b) / scale(a);
            c.b = jo::as_actor(b)->collide_height * scale(b) / scale(a);
            to_local(&c, a);
            return joan::orig::arg::as<std::uint8_t>(joan::orig::capsule_box(c, box_of(a)));  // 0x0069d390
        }
        }
        [[fallthrough]];  // no default in the original: falls into case 3
    case 3:
        switch (kind(b)) {
        case 0:
            return 0;
        case 1: {  // 0x006a378e capsule / sphere
            Sphere s;
            world_point<&jo::Actor::aim_sphere>(&s, b);
            s.r = jo::as_actor(b)->aim_sphere[3] * scale(b);
            Capsule c;
            std::memcpy(&c, &jo::as_actor(a)->collide_kind, sizeof c);
            world_point<&jo::Actor::collide_kind>(&c, a);
            c.a = jo::as_actor(a)->collide_radius * scale(a);
            c.b = jo::as_actor(a)->collide_height * scale(a);
            return joan::orig::arg::as<std::uint8_t>(joan::orig::capsule_sphere(c, s));  // 0x0069d210
        }
        case 2: {  // 0x006a3898 capsule / box
            Capsule c;
            std::memcpy(&c, &jo::as_actor(a)->collide_kind, sizeof c);
            world_point<&jo::Actor::collide_kind>(&c, a);
            c.a = jo::as_actor(a)->collide_radius * scale(a) / scale(b);
            c.b = jo::as_actor(a)->collide_height * scale(a) / scale(b);
            node_update(a);  // original quirk: a's node again, then b's matrix
            Matrix* const mb = node_matrix(b);
            Matrix inv;
            inverse(&inv, mb);
            transform_inplace(&c, &inv);
            return joan::orig::arg::as<std::uint8_t>(joan::orig::capsule_box(c, box_of(b)));  // 0x0069d390
        }
        case 3: {  // 0x006a3993 capsule / capsule
            Capsule ca;
            world_point<&jo::Actor::collide_kind>(&ca, a);
            ca.a = jo::as_actor(a)->collide_radius * scale(a);
            ca.b = jo::as_actor(a)->collide_height * scale(a);
            Capsule cb;
            world_point<&jo::Actor::collide_kind>(&cb, b);
            cb.a = jo::as_actor(b)->collide_radius * scale(b);
            cb.b = jo::as_actor(b)->collide_height * scale(b);
            return joan::orig::cylinders_overlap(ca.x, ca.y, ca.z, ca.a, ca.b, cb.x, cb.y, cb.z, cb.a, cb.b);  // 0x0069d160
        }
        }
        return 0;
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b89_02", name, addr, fn)
B("intersect", 0x006a31d0, &shapes_overlap);
#undef B

}  // namespace game_b89_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::shapes_overlap_6a31d0(void* a0, void* a1) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b89_02::shapes_overlap>(a0, a1)); }  // 0x6a31d0
