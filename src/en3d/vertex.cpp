// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/vertex.cpp: functions of the original d:\ww\en3d\vertex.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (en3d/vertex.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../render/v3d.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::cdecl_call;

using joan::hook::global;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::stdcall_call;

using joan::call::import_call;

struct Vec3 {
    float x, y, z;
};

}  // namespace

// ---------------------------------------- from modules/en3d_helpers3.cpp (en3d/helpers3)
namespace {
namespace en3d_helpers3 {

using joan::orig::helpers::differs;

// --- cdecl maths ----------------------------------------------------------------------------
// 0x0060c180: a matrix's first row (x axis).
Vec3* matrix_axis_x(Vec3* out, const float* m) {
    out->x = m[0];
    out->y = m[1];
    out->z = m[2];
    return out;
}
// 0x0060bc50: rotation matrix (D3DX 0x00767085) copied out.
float* matrix_rotation(float* out, float angle) {
    float m[16];
    joan::orig::d3dx::matrix_rotation_z(joan::orig::arg::mptr(m), joan::orig::arg::f32(angle));
    std::memcpy(out, m, 64);
    return out;
}
// 0x0060bca0: rotation about an axis (D3DXMatrixRotationAxis 0x0076883c) copied out.
float* matrix_rotation_axis(float* out, const Vec3* axis, float angle) {
    float m[16];
    Vec3 a = *axis;
    joan::orig::d3dx::matrix_rotation_axis(joan::orig::arg::mptr(m), &a, joan::orig::arg::f32(angle));
    std::memcpy(out, m, 64);
    return out;
}

// 0x0060bd00: orbit matrix - base axes, rotated by yaw, then -pitch about its x axis, roll
// about its z axis (when non-zero), then moved back by `distance` along z (when non-zero).
float* orbit_matrix(float* out, float yaw, float pitch, float roll, float distance) {
    float m[16] = {0, -1, 0, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0, 1};
    float r[16];
    Vec3 v;
    joan::orig::matrix_rotation(joan::orig::arg::mptr(r), joan::orig::arg::f32(yaw));  // 0x0060bc50
    import_call<void*>(d::iat_v3d_87_matrix_multiply_2, m, r);    // multiply in place
    joan::orig::matrix_rotation_axis(joan::orig::arg::mptr(r), joan::orig::arg::mptr(joan::orig::arg::as<Vec3*>(joan::orig::matrix_axis_x(&v, joan::orig::arg::mptr(m)))), joan::orig::arg::f32(-pitch));  // 0x0060bca0 about axis x
    import_call<void*>(d::iat_v3d_87_matrix_multiply_2, m, r);
    if (differs(roll, 0.0f)) {
        joan::orig::matrix_rotation_axis(joan::orig::arg::mptr(r), joan::orig::arg::mptr(joan::orig::arg::as<Vec3*>(joan::orig::matrix_axis_z(&v, joan::orig::arg::ptr(m)))), joan::orig::arg::f32(roll));  // about axis z (0x0060c220)
        import_call<void*>(d::iat_v3d_87_matrix_multiply_2, m, r);
    }
    if (differs(distance, 0.0f)) {
        Vec3 scaled;
        const Vec3 t = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&scaled, joan::orig::matrix_axis_z(&v, joan::orig::arg::ptr(m)), joan::orig::arg::f32(-distance)));  // 0x0060cd00
        Vec3 move = t;
        import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, m, &move);  // multiply_translation
    }
    std::memcpy(out, m, 64);
    return out;
}

#define HELPER3(name, addr, fn) JOAN_REPLACE("en3d/helpers3", name, addr, fn)
HELPER3("matrix_get_right", 0x0060c180, &matrix_axis_x);
HELPER3("init_rotation_z", 0x0060bc50, &matrix_rotation);
HELPER3("init_rotation_axis", 0x0060bca0, &matrix_rotation_axis);
HELPER3("init_plane_60d920", 0x0060d920, &joan::orig::plane_from_points);
HELPER3("init_rotation_hvr", 0x0060bd00, &orbit_matrix);
#undef HELPER3

}  // namespace en3d_helpers3
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

JOAN_COUNTER(c_closest, "segment_closest_t");
JOAN_COUNTER(c_ypr, "matrix_rotation_ypr");

// 0x0060df10 (seg = a[3], b[3]; point): t of the closest point on the line a + t (b - a).
float JOAN_CDECL segment_closest_t(const float* seg, const float* point) {
    c_closest.hit();
    float tmp1[3], d1[3], tmp2[3], d2[3];
    std::memcpy(d1, joan::orig::arg::as<float*>(joan::orig::vec_sub(joan::orig::arg::mptr(tmp1), joan::orig::arg::ptr(seg + 3), joan::orig::arg::ptr(seg))), 12);   // 0x0060cc90 b - a
    std::memcpy(d2, joan::orig::arg::as<float*>(joan::orig::vec_sub(joan::orig::arg::mptr(tmp2), joan::orig::arg::ptr(point), joan::orig::arg::ptr(seg))), 12);     // 0x0060cc90 p - a
    const float dot = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<const float*>(d2), static_cast<const float*>(d1)));
    const float len_sq = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const float*>(d1)));
    return dot / len_sq;
}

// 0x0060bf40 m = rotation(yaw) then about the rotated axes (angles passed as their dwords).
float* JOAN_CDECL matrix_rotation_ypr(float* m, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    c_ypr.hit();
    joan::orig::matrix_rotation(joan::orig::arg::mptr(m), joan::orig::arg::f32(a));  // 0x0060bc50
    float axis1[3];
    joan::orig::matrix_row1(joan::orig::arg::mptr(axis1), joan::orig::arg::mptr(m));  // 0x0060c1d0
    float r1[16];
    joan::orig::matrix_rotation_axis(joan::orig::arg::mptr(r1), joan::orig::arg::mptr(axis1), joan::orig::arg::f32(b));  // 0x0060bca0
    import_call<void*>(d::iat_v3d_87_matrix_multiply_2, m, r1);         // matrix_multiply
    float axis2[3];
    joan::orig::matrix_axis_x(joan::orig::arg::mptr(axis2), joan::orig::arg::mptr(m));  // 0x0060c180
    float r2[16];
    joan::orig::matrix_rotation_axis(joan::orig::arg::mptr(r2), joan::orig::arg::mptr(axis2), joan::orig::arg::f32(c));  // 0x0060bca0
    import_call<void*>(d::iat_v3d_87_matrix_multiply_2, m, r2);
    return m;
}

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("line_seg_find_division", 0x0060df10, &segment_closest_t);
L81("init_rotation_actor_hvr", 0x0060bf40, &matrix_rotation_ypr);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

// 0x0060d990: D3DX 0x007681df (cross product) of copies of a and b.
Vec3* vec_cross(Vec3* out, const Vec3* a, const Vec3* b) {
    Vec3 p = *a, q = *b;
    joan::orig::d3dx::plane_from_point_normal(joan::orig::arg::mptr(out), &p, &q);
    return out;
}

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("init_plane_60d990", 0x0060d990, &vec_cross);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/game_b101_11.cpp (game/b101_11)
namespace {
namespace game_b101_11 {

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::hook::x87::Fpu;

using joan::img::obj;

JOAN_COUNTER(c_facing, "Facing::matrix");

// 0x0060c8e0
u8* JOAN_CDECL facing_matrix(void* unit) {
    c_facing.hit();
    u32 dummy;
    joan::orig::as_angle(&dummy)->construct();                    // 0x00576120 (empty ctor)
    const float v = joan::orig::arg::as<float>(joan::orig::as_angle(unit)->byte_to_float());      // 0x00576300 (fst dword)
    const i32 deg = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(v))));  // 0x00590a20
    switch (deg) {
        case 0: return &img::global<u8>(d::g_a18ab8);
        case 45: return &img::global<u8>(d::g_a18af8);
        case 90: return &img::global<u8>(d::g_a18b38);
        case 135: return &img::global<u8>(d::g_a18b78);
        case 180: return &img::global<u8>(d::g_a18bb8);
        case 225: return &img::global<u8>(d::g_a18bf8);
        case 270: return &img::global<u8>(d::g_a18c38);
        case 315: return &img::global<u8>(d::g_a18c78);
        case 360: return &img::global<u8>(d::g_a18ab8);
        default: break;
    }
    std::memcpy(obj(d::g_a69c90), obj(d::g_identity_matrix), 0x40);
    const Fpu fpu{true};
    const double r = joan::orig::arg::as<double>(joan::orig::as_angle(unit)->radians());  // 0x00576280
    const float ang = static_cast<float>(fpu.mul(r, global<float>(d::flt_1)));
    joan::orig::matrix_rotation(joan::orig::arg::mptr(obj(d::g_a69c90)), joan::orig::arg::f32(ang));  // 0x0060bc50
    return &img::global<u8>(d::g_a69c90);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("AngleI_to_direction_matrix", 0x0060c8e0, &facing_matrix);
#undef B

}  // namespace game_b101_11
}  // namespace

// ---------------------------------------- from modules/game_world4.cpp (game/world4)
namespace {
namespace game_world4 {

JOAN_COUNTER(c_sphere, "segment_hits_sphere");

// 0x0060de40 out = segment point at t (t passed as its dword).
void segment_point(void* out, const void* seg, std::uint32_t t_bits) { joan::orig::segment_point_at(joan::orig::arg::mptr(out), joan::orig::arg::ptr(seg), joan::orig::arg::f32(t_bits)); }

// 0x0060e3f0 segment (a, b) vs sphere (centre, radius): either end inside, or the closest
// point (parameter 0x0060df10) within the radius.
std::int32_t JOAN_CDECL segment_hits_sphere(const float* seg, const float* sphere) {
    c_sphere.hit();
    const float r2 = sphere[3] * sphere[3];
    float tmp[3], d[3];
    std::memcpy(d, joan::orig::arg::as<float*>(joan::orig::vec_sub(static_cast<void*>(tmp), joan::orig::arg::ptr(seg), joan::orig::arg::ptr(sphere))), 12);  // 0x0060cc90 sub
    if (!(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(d)) > static_cast<double>(r2))) return 1;  // length_sq
    std::memcpy(d, joan::orig::arg::as<float*>(joan::orig::vec_sub(static_cast<void*>(tmp), joan::orig::arg::ptr(seg + 3), joan::orig::arg::ptr(sphere))), 12);
    if (!(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(d)) > static_cast<double>(r2))) return 1;
    float t = joan::orig::arg::as<float>(joan::orig::segment_closest_t(joan::orig::arg::mptr(seg), joan::orig::arg::mptr(sphere)));  // 0x0060df10 closest parameter (fstp dword)
    std::uint32_t t_bits;
    std::memcpy(&t_bits, &t, 4);
    float p[3];
    segment_point(p, seg, t_bits);
    std::memcpy(d, joan::orig::arg::as<float*>(joan::orig::vec_sub(static_cast<void*>(tmp), joan::orig::arg::ptr(static_cast<const float*>(p)), joan::orig::arg::ptr(sphere))), 12);
    if (!(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(d)) > static_cast<double>(r2))) return 1;
    return 0;
}

#define W4(name, addr, fn) JOAN_REPLACE("game/world4", name, addr, fn)
W4("line_seg_sphere_overlap", 0x0060e3f0, &segment_hits_sphere);
W4("line_seg_box_overlap", 0x0060dfd0, &joan::orig::segment_hits_box);
#undef W4

}  // namespace game_world4
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

// 0x0060c1d0 out = matrix row 1 (xyz).
void* JOAN_CDECL matrix_row1(void* out, const void* m) {
    jo::view<std::uint32_t>(jo::as_vec3(out)->x) = jo::view<std::uint32_t>(jo::as_matrix44(m)->m[4]);
    jo::view<std::uint32_t>(jo::as_vec3(out)->y) = jo::view<std::uint32_t>(jo::as_matrix44(m)->m[5]);
    jo::view<std::uint32_t>(jo::as_vec3(out)->z) = jo::view<std::uint32_t>(jo::as_matrix44(m)->m[6]);
    return out;
}
// 0x0060c270 copy the 3x3 rotation part.
void* JOAN_CDECL matrix_copy3x3(void* out, const void* m) {
    for (std::int32_t row = 0; row < 3; ++row)
        for (std::int32_t col = 0; col < 3; ++col) jo::view<std::uint32_t>(jo::as_matrix44(out)->m[row * 4 + col]) = jo::view<std::uint32_t>(jo::as_matrix44(m)->m[row * 4 + col]);
    return out;
}

// 0x0060c3b0 matrix -> rotation axis (+ angle): D3DXQuaternionRotationMatrix /
// D3DXQuaternionToAxisAngle.
void* JOAN_CDECL matrix_axis_angle(const void* m, void* axis_out, void* angle_out) {
    std::uint32_t q[4];
    std::uint32_t axis[3];
    std::uint32_t copy[16];
    joan::orig::as_quaternion(q)->construct();           // 0x005ac230 Quaternion::Quaternion
    joan::orig::as_vector(axis)->construct();        // 0x005bd1c0 Vector::Vector
    joan::orig::as_matrix(copy)->copy(joan::orig::arg::mptr(m));     // 0x0060e550 Matrix copy
    joan::orig::d3dx::quaternion_rotation_matrix(joan::orig::arg::mptr(static_cast<void*>(q)), joan::orig::arg::mptr(static_cast<void*>(copy)));
    joan::orig::d3dx::quaternion_to_axis_angle(joan::orig::arg::mptr(static_cast<void*>(q)), joan::orig::arg::mptr(static_cast<void*>(axis)), joan::orig::arg::mptr(angle_out));
    std::memcpy(axis_out, axis, sizeof axis);
    return axis_out;
}

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("matrix_get_down", 0x0060c1d0, &matrix_row1);
FX1("matrix_set_3x3", 0x0060c270, &matrix_copy3x3);
FX1("matrix_query_rotation", 0x0060c3b0, &matrix_axis_angle);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("matrix_get_at", 0x0060c220, &joan::orig::matrix_axis_z);
HELPER("matrix_get_translation", 0x0060c310, &joan::orig::matrix_position);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/game_b111_11.cpp (game/b111_11)
namespace {
namespace game_b111_11 {

using joan::hook::global;
using u32 = std::uint32_t;

using joan::call::cdecl_call;


JOAN_COUNTER(c_euler, "matrix_to_euler");

// ============================================================================ 0x0060c430
// x87 arithmetic at the game's precision control (PC24) with the x87 exponent range: src/fp/x87.hpp.
using joan::fp::pc24_add;
using joan::fp::pc24_sub;
using joan::fp::pc24_mul_m32;
using joan::fp::pc24_div_m32;

// m[0]*m[0] + m[1]*m[1] + m[2]*m[2] (`fld dword; fmul dword; ...; faddp` under PC24); the caller passes it to
// sqrt as a qword (fstp qword: exact).
double x87_row_len2(const float* m) {
    const double x = m[0], y = m[1], z = m[2];
    return pc24_add(pc24_add(pc24_mul_m32(x, &m[0]), pc24_mul_m32(y, &m[1])), pc24_mul_m32(z, &m[2]));
}
// `fld dword [a]; fchs; fdiv dword [b]` under PC24 (passed on as a qword).
double x87_neg_div(const float* a, const float* b) { return pc24_div_m32(-static_cast<double>(*a), b); }

// The CRT atan2 / asin results (as the doubles their slot calls give) go through `fsub qword [pi/2]` and
// `fmul dword [1.0f]` under PC24 before `fstp dword`. The original CRT atan2 leaves a 64-bit fpatan st0, which the
// double cannot carry: in the gimbal branch atan2(m5, m4) - pi/2 cancels when |m4 / m5| is tiny (m4 == 0, m5 > 0:
// the original gives 565 * 2^-63 = pi/2 rounded to 64 bits minus pi/2 rounded to 53, this gives 0), elsewhere
// atan2 * 1.0f could double-round (0 of 555k normal-branch results differ; re/p1/gamefloat).
void JOAN_CDECL matrix_to_euler(const float* m, float* a, float* b, float* c) {
    c_euler.hit();
    const float s = joan::orig::arg::as<float>(joan::orig::libc::sqrt(joan::orig::arg::f64(x87_row_len2(m))));  // sqrt
    const double lo = global<double>(d::dbl_m1em10), hi = global<double>(d::dbl_1em10);  // -1e-10, 1e-10
    const float* const one = &global<float>(d::flt_1);                             // 1.0f
    const auto atan2_ = [](float y, float x) {
        return joan::orig::arg::as<double>(joan::orig::libc::atan2(joan::orig::arg::f64(static_cast<double>(y)), joan::orig::arg::f64(static_cast<double>(x))));
    };
    const auto asin_ = [](double x) { return joan::orig::arg::as<double>(joan::orig::libc::asin(joan::orig::arg::f64(x))); };
    const double m0 = m[0], m1 = m[1];
    if (m0 >= lo && !(m0 > hi) && m1 >= lo && !(m1 > hi)) {
        *a = static_cast<float>(pc24_mul_m32(pc24_sub(atan2_(m[5], m[4]), global<double>(d::dbl_1_5707963267948966)), one));  // atan2(m5, m4) - pi/2
        *b = static_cast<float>(pc24_mul_m32(asin_(x87_neg_div(&m[2], &s)), one));  // asin(-m2 / s)
        jo::view<u32>(*c) = 0;
    } else {
        *a = static_cast<float>(pc24_mul_m32(atan2_(m[1], m[0]), one));  // atan2(m1, m0)
        *b = static_cast<float>(pc24_mul_m32(asin_(x87_neg_div(&m[2], &s)), one));
        *c = static_cast<float>(pc24_mul_m32(atan2_(m[6], m[10]), one));  // atan2(m6, m10)
    }
}

#define G(name, addr, fn) JOAN_REPLACE("game/b111_11", name, addr, fn)
G("matrix_query_actor_hvr", 0x0060c430, &matrix_to_euler);
#undef G

}  // namespace game_b111_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers36.cpp (en3d/helpers36)
namespace {
namespace en3d_helpers36 {

using joan::hook::x87::Fpu;

using joan::call::import_call;

struct V3 {
    float x, y, z;
};

float bits_float(std::uint32_t bits) {
    float v;
    std::memcpy(&v, &bits, 4);
    return v;
}

JOAN_COUNTER(c_ray_triangle, "ray_triangle");

// 0x0060e810 ray_triangle(t, segment[2], triangle[3], u, v): segment p0 -> p1 against the
// triangle, front faces only (det >= 1e-4). Returns 1 with t, u, v (barycentric) on a hit.
std::int32_t JOAN_CDECL ray_triangle(float* t, const float* seg, const float* tri, float* u, float* v) {
    c_ray_triangle.hit();
    const Fpu fpu{true};
    V3 tmp;
    const V3 dir = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(seg + 3), joan::orig::arg::ptr(seg)));  // 0x0060cc90 (a - b)
    const V3 e1 = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(tri + 3), joan::orig::arg::ptr(tri)));
    const V3 e2 = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(tri + 6), joan::orig::arg::ptr(tri)));
    V3 pvec;
    import_call<void*>(d::iat_v3d_87_cross_product, &pvec, &dir, &e2);  // cross_product
    const float det = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, &e1, &pvec));  // dot_product
    if (!(det >= bits_float(0x38d1b717))) return 0;               // < 1e-4 or unordered
    const V3 tvec = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(seg), joan::orig::arg::ptr(tri)));
    *u = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, &tvec, &pvec));
    if (!(*u >= 0.0f)) return 0;
    if (*u > det) return 0;
    V3 qvec;
    import_call<void*>(d::iat_v3d_87_cross_product, &qvec, &tvec, &e1);
    *v = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, &dir, &qvec));
    if (!(*v >= 0.0f)) return 0;
    if (fpu.add(*u, *v) > static_cast<double>(det)) return 0;  // fld u; fadd v; fcomp det
    const float inv = 1.0f / det;
    const double d = import_call<double>(d::iat_v3d_87_dot_product, &e2, &qvec);  // st0 used by fmul directly
    *t = static_cast<float>(fpu.mul(d, inv));
    *u = inv * *u;
    *v = inv * *v;
    return 1;
}

struct H {
    // 0x0060eae0 Bone::Bone(owner, callback). ret 8, eax = this.
    void* bone_ctor(void* owner, void* callback) {
        jo::self<jo::Bone>(this)->parent = nullptr;
        jo::self<jo::Bone>(this)->next = nullptr;
        jo::self<jo::Bone>(this)->prev = nullptr;
        jo::self<jo::Bone>(this)->last_child = nullptr;
        jo::self<jo::Bone>(this)->first_child = nullptr;
        jo::self<jo::Bone>(this)->callback = callback;
        jo::self<jo::Bone>(this)->owner = owner;
        std::memcpy(this, img::obj(d::g_identity_matrix), 0x40);  // identity
        std::memcpy(jo::self<jo::Bone>(this)->matrix_, this, 0x40);
        jo::self<jo::Bone>(this)->dirty = 0;
        return this;
    }
};

#define HELPER36(name, addr, fn) JOAN_REPLACE("en3d/helpers36", name, addr, fn)
HELPER36("line_seg_tri_intersect", 0x0060e810, &ray_triangle);
HELPER36("Frame::Frame", 0x0060eae0, &H::bone_ctor);
#undef HELPER36

}  // namespace en3d_helpers36
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers20)
namespace {
namespace en3d_helpers20 {

using joan::call::import_call;

void* JOAN_CDECL plane_normal(float* out, const float* plane) {  // 0x0060d9e0: normalize(plane.xyz)
    float v[3] = {plane[0], plane[1], plane[2]};
    return import_call<void*>(d::iat_v3d_87_vector_normalize_2, out, v);
}

#define HELPER20(name, addr, fn) JOAN_REPLACE("en3d/helpers20", name, addr, fn)
HELPER20("plane_get_normal", 0x0060d9e0, &plane_normal);
#undef HELPER20

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {

bool below_or_nan(float a, float b) { return !(a >= b); }  // x87 C0

// --- cdecl maths (no `this`) ---------------------------------------------------------------
// 0x0060da30: signed distance of a point from a plane (a, b, c, d).
float plane_distance(const float* plane, const float* p) {
    return plane[0] * p[0] + plane[1] * p[1] + plane[2] * p[2] + plane[3];
}
// 0x0060dc80: the range of plane distances over an axis-aligned box (min xyz, max xyz).
void box_plane_extent(const float* n, const float* box, float* lo, float* hi) {
    if (!below_or_nan(n[0], 0.0f)) {
        *lo = n[0] * box[0];
        *hi = n[0] * box[3];
    } else {
        *lo = n[0] * box[3];
        *hi = n[0] * box[0];
    }
    for (int k = 1; k < 3; ++k) {
        if (!below_or_nan(n[k], 0.0f)) {
            *lo = n[k] * box[k] + *lo;
            *hi = n[k] * box[k + 3] + *hi;
        } else {
            *lo = n[k] * box[k + 3] + *lo;
            *hi = n[k] * box[k] + *hi;
        }
    }
    *lo = *lo + n[3];
    *hi = *hi + n[3];
}

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("plane_dot_product", 0x0060da30, &plane_distance);
HELPER2("plane_box_min_max_dot_product", 0x0060dc80, &box_plane_extent);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("line_seg_division", 0x0060de40, &joan::orig::segment_point_at);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/en3d_helpers7.cpp (en3d/helpers7)
namespace {
namespace en3d_helpers7 {


struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // --- Bone (0x0060exxx): +0x40 parent, +0x44 / +0x48 previous / next sibling,
    //     +0x4c first child, +0x50 last child. 0x0060ec70: re-parent (appended last).
    void bone_set_parent(std::uint8_t* parent) {
        if (jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->parent) == parent) return;
        if (jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->parent) != nullptr) {
            if (jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->prev) != nullptr)
                jo::view<std::uint8_t*>(jo::as_bone(jo::self<jo::Bone>(this)->prev)->next) = jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->next);
            else
                jo::view<std::uint8_t*>(jo::as_bone(jo::self<jo::Bone>(this)->parent)->first_child) = jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->next);
            if (jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->next) != nullptr)
                jo::view<std::uint8_t*>(jo::as_bone(jo::self<jo::Bone>(this)->next)->prev) = jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->prev);
            else
                jo::view<std::uint8_t*>(jo::as_bone(jo::self<jo::Bone>(this)->parent)->last_child) = jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->prev);
            jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->prev) = nullptr;
            jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->next) = nullptr;
            jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->parent) = nullptr;
        }
        if (parent == nullptr) return;
        jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->parent) = parent;
        std::uint8_t* const last = jo::view<std::uint8_t*>(jo::as_bone(parent)->last_child);
        std::uint8_t* next;
        if (last == nullptr) {
            next = jo::view<std::uint8_t*>(jo::as_bone(parent)->first_child);
            jo::view<std::uint8_t*>(jo::as_bone(parent)->first_child) = self();
        } else {
            next = jo::view<std::uint8_t*>(jo::as_bone(last)->next);
            jo::view<std::uint8_t*>(jo::as_bone(last)->next) = self();
        }
        jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->prev) = last;
        jo::view<std::uint8_t*>(jo::self<jo::Bone>(this)->next) = next;
        if (next != nullptr)
            jo::view<std::uint8_t*>(jo::as_bone(next)->prev) = self();
        else
            jo::view<std::uint8_t*>(jo::as_bone(parent)->last_child) = self();
    }
};

#define HELPER7(name, addr, fn) JOAN_REPLACE("en3d/helpers7", name, addr, fn)
HELPER7("Frame::set_parent", 0x0060ec70, &H::bone_set_parent);
#undef HELPER7

}  // namespace en3d_helpers7
}  // namespace

namespace joan::orig {
// 0x0060d920: plane through three points (D3DXPlaneFromPoints 0x00768ff8), normalised (0x007680a8).
float* plane_from_points(void* out_, const void* a_, const void* b_, const void* c_) {
    float* const out = static_cast<float*>(out_);
    const Vec3* const a = static_cast<const Vec3*>(a_);
    const Vec3* const b = static_cast<const Vec3*>(b_);
    const Vec3* const c = static_cast<const Vec3*>(c_);
    Vec3 p = *a, q = *b, r = *c;
    joan::orig::d3dx::plane_from_points(joan::orig::arg::mptr(out), &p, &q, &r);
    joan::orig::d3dx::plane_normalize(joan::orig::arg::mptr(out), joan::orig::arg::mptr(out));
    return out;
}
}  // namespace joan::orig

// Out-of-line definitions from modules/en3d_helpers.cpp (declared in original/vecmath.hpp and
// original/en3d.hpp, registered above).
namespace joan::orig {
Vec3* JOAN_CDECL matrix_axis_z(void* out_, const void* m_) {  // 0x0060c220: row 3
    std::uint32_t* const out = static_cast<std::uint32_t*>(out_);
    const std::uint32_t* const m = static_cast<const std::uint32_t*>(m_);
    out[0] = m[8];
    out[1] = m[9];
    out[2] = m[10];
    return static_cast<Vec3*>(out_);
}
Vec3* JOAN_CDECL matrix_position(void* out_, const void* m_) {  // 0x0060c310: row 4
    std::uint32_t* const out = static_cast<std::uint32_t*>(out_);
    const std::uint32_t* const m = static_cast<const std::uint32_t*>(m_);
    out[0] = m[12];
    out[1] = m[13];
    out[2] = m[14];
    return static_cast<Vec3*>(out_);
}
}  // namespace joan::orig

// Out-of-line definition from modules/game_leaves80b.cpp (declared in original/vecmath.hpp, registered above).
namespace {

JOAN_COUNTER(c_point_at, "Segment::point_at");

}  // namespace

namespace joan::orig {
// 0x0060de40 Segment::point_at(out, seg, t) = seg.a + t * (seg.b - seg.a).
Vec3* JOAN_CDECL segment_point_at(void* out_, const void* seg_, float t) {
    Vec3* const out = static_cast<Vec3*>(out_);
    const Vec3* const seg = static_cast<const Vec3*>(seg_);
    c_point_at.hit();
    Vec3 tmp1, tmp2, tmp3;
    const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp1, joan::orig::arg::ptr(seg + 1), joan::orig::arg::ptr(seg)));  // 0x0060cc90 sub
    const Vec3 s = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&tmp2, &d, joan::orig::arg::f32(t)));        // 0x0060cd00 scale
    const Vec3 r = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&tmp3, &s, joan::orig::arg::ptr(seg)));      // 0x0060cc20 add
    *out = r;
    return out;
}
}  // namespace joan::orig

// Out-of-line definition from modules/game_world4.cpp (declared in original/vecmath.hpp, registered above).
namespace {

JOAN_COUNTER(c_box, "segment_hits_box");

// Box (min xyz, max xyz) contains point: 0x0060d5f0 (bool in al).
bool box_contains(const void* box, const void* point) { return (joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(box)->contains(joan::orig::arg::ptr(point))) & 0xff) != 0; }
// 0x0060de40 out = segment point at t (t passed as its dword).
void segment_point(void* out, const void* seg, std::uint32_t t_bits) { joan::orig::segment_point_at(joan::orig::arg::mptr(out), joan::orig::arg::ptr(seg), joan::orig::arg::f32(t_bits)); }

}  // namespace

namespace joan::orig {
// 0x0060dfd0 segment (a, b) vs box (min, max): an end inside, else the segment's bounds
// overlap the box and one of the six face planes is crossed at t in [0, 1] inside the box.
std::int32_t JOAN_CDECL segment_hits_box(const void* seg_, const void* box_) {
    const float* const seg = static_cast<const float*>(seg_);
    const float* const box = static_cast<const float*>(box_);
    c_box.hit();
    if (box_contains(box, seg) || box_contains(box, seg + 3)) return 1;
    float bounds[6];
    joan::orig::as_aabb(static_cast<void*>(bounds))->set_point(joan::orig::arg::ptr(seg));      // 0x0060d430 from point
    joan::orig::as_aabb(static_cast<void*>(bounds))->extend(joan::orig::arg::ptr(seg + 3));  // 0x0060d490 extend
    if ((joan::orig::arg::as<std::uint8_t>(joan::orig::as_box(box)->overlaps(joan::orig::arg::mptr(static_cast<const void*>(bounds)))) & 0xff) == 0) return 0;  // 0x0060d6b0 overlap
    // (plane function thunk, box component, point component)
    struct BoxFace {
        std::uintptr_t fn;
        std::int32_t comp;
        std::int32_t axis;
    };
    const BoxFace faces[6] = {
        {fn::code_value(fn::line_seg_plane_yz_intersect), 0, 0}, {fn::code_value(fn::line_seg_plane_yz_intersect), 3, 0},  // 0x0060e720 x planes
        {fn::code_value(fn::line_seg_plane_zx_intersect), 1, 1}, {fn::code_value(fn::line_seg_plane_zx_intersect), 4, 1},  // 0x0060e790 y planes
        {fn::code_value(fn::line_seg_plane_xy_intersect), 2, 2}, {fn::code_value(fn::line_seg_plane_xy_intersect), 5, 2},  // 0x0060e6a0 z planes
    };
    for (std::int32_t k = 0; k < 6; ++k) {
        std::uint32_t plane;
        std::memcpy(&plane, &box[faces[k].comp], 4);
        float t = 0.0f;
        // The plane goes as a float: the three callees take `float` (System V x86-64 passes it in XMM0; the dword
        // bits would go in an integer register the callee does not read).
        if (cdecl_call<std::int32_t>(faces[k].fn, &t, seg, box[faces[k].comp]) != 1) continue;
        std::uint32_t t_bits;
        std::memcpy(&t_bits, &t, 4);
        float p[3];
        segment_point(p, seg, t_bits);
        std::memcpy(&p[faces[k].axis], &plane, 4);
        if (!(t >= 0.0f)) continue;  // t < 0 or NaN
        if (t > 1.0f) continue;
        if (box_contains(box, p)) return 1;
    }
    return 0;
}
}  // namespace joan::orig

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::matrix_rotation(void* a0, float a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers3::matrix_rotation>(a0, a1)); }  // 0x60bc50
auto joan::orig::matrix_rotation_axis(void* a0, void* a1, float a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers3::matrix_rotation_axis>(a0, a1, a2)); }  // 0x60bca0
auto joan::orig::matrix_axis_x(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers3::matrix_axis_x>(a0, a1)); }  // 0x60c180
auto joan::orig::matrix_row1(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_fx::matrix_row1>(a0, a1)); }  // 0x60c1d0
auto joan::orig::matrix_copy3x3(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_fx::matrix_copy3x3>(a0, a1)); }  // 0x60c270
auto joan::orig::matrix_axis_angle(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_fx::matrix_axis_angle>(a0, a1, a2)); }  // 0x60c3b0
auto joan::orig::matrix_to_euler(void* a0, void* a1, void* a2, void* a3) -> void { joan::bind::free<&::game_b111_11::matrix_to_euler>(a0, a1, a2, a3); }  // 0x60c430
auto joan::orig::plane_normal(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers20::plane_normal>(a0, a1)); }  // 0x60d9e0
auto joan::orig::plane_distance(void* a0, void* a1) -> float { return joan::bind::conv<float>(joan::bind::free<&::en3d_helpers2::plane_distance>(a0, a1)); }  // 0x60da30
auto joan::orig::box_plane_extent(void* a0, void* a1, void* a2, void* a3) -> void { joan::bind::free<&::en3d_helpers2::box_plane_extent>(a0, a1, a2, a3); }  // 0x60dc80
auto joan::orig::segment_hits_sphere(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_world4::segment_hits_sphere>(a0, a1)); }  // 0x60e3f0
auto joan::orig::ray_triangle(void* a0, void* a1, void* a2, void* a3, void* a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::en3d_helpers36::ray_triangle>(a0, a1, a2, a3, a4)); }  // 0x60e810
auto joan::orig::Bone::Bone_60eae0(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::bone_ctor>(this, a0, a1)); }  // 0x60eae0
auto joan::orig::Bone::set_parent(void* a0) -> void { joan::bind::member<&::en3d_helpers7::H::bone_set_parent>(this, a0); }  // 0x60ec70
auto joan::orig::vec_cross(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers6::vec_cross>(a0, a1, a2)); }  // 0x60d990
auto joan::orig::segment_closest_t(void* a0, void* a1) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_leaves81::segment_closest_t>(a0, a1)); }  // 0x60df10

// ---------------------------------------- from modules/en3d_helpers3.cpp (en3d/helpers3)
namespace {
namespace en3d_helpers3_2 {

struct H {
    float* fl() { return reinterpret_cast<float*>(this); }

    // 0x0060d6b0: per axis, either range starts inside the other.
    bool box_overlaps(const float* o) {
        const float* a = fl();
        for (int k = 0; k < 3; ++k) {
            const bool ok = (!(o[k] > a[k]) && !(a[k] > o[k + 3])) || (!(a[k] > o[k]) && !(o[k] > a[k + 3]));
            if (!ok) return false;
        }
        return true;
    }
};

#define HELPER3(name, addr, fn) JOAN_REPLACE("en3d/helpers3", name, addr, fn)
HELPER3("Box3d::is_overlap", 0x0060d6b0, &H::box_overlaps);
#undef HELPER3

}  // namespace en3d_helpers3_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6_2 {

struct H {
    // --- Box (min xyz, max xyz) ---------------------------------------------------------------------
    Vec3* box_centre(Vec3* out) {  // 0x0060d3a0
        const float* b = reinterpret_cast<const float*>(this);
        const Vec3 r = {(b[0] + b[3]) * 0.5f, (b[1] + b[4]) * 0.5f, (b[2] + b[5]) * 0.5f};
        *out = r;
        return out;
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("operator+", 0x0060cc20, &joan::orig::vec_add);
HELPER6("operator-_60cc90", 0x0060cc90, &joan::orig::vec_sub);
HELPER6("operator*_60cd00", 0x0060cd00, &joan::orig::vec_scale);
HELPER6("Box3d::center", 0x0060d3a0, &H::box_centre);
#undef HELPER6

}  // namespace en3d_helpers6_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers4.cpp (en3d/helpers4)
namespace {
namespace en3d_helpers4 {

#define HELPER4(name, addr, fn) JOAN_REPLACE("en3d/helpers4", name, addr, fn)
HELPER4("operator+=", 0x0060cdc0, &joan::orig::vec_add_in_place);
#undef HELPER4

}  // namespace en3d_helpers4
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units {

bool equal_or_nan(float a, float b) { return !(a < b || a > b); }

JOAN_COUNTER(c_intersect, "line_plane");

// 0x0060e590 line_plane(t_out, segment[2], plane): parameter of the segment / plane
// intersection; 1 found, 0 parallel, -1 parallel and on the plane.
std::int32_t JOAN_CDECL line_plane(float* t, const float* seg, const float* plane) {
    c_intersect.hit();
    const float d[3] = {seg[3] - seg[0], seg[4] - seg[1], seg[5] - seg[2]};
    const float denom = (d[0] * plane[0] + d[1] * plane[1]) + d[2] * plane[2];
    const float num = ((-plane[3] - plane[0] * seg[0]) - plane[1] * seg[1]) - plane[2] * seg[2];
    if (!equal_or_nan(denom, 0.0f)) {
        *t = num / denom;
        return 1;
    }
    if (!equal_or_nan(num, 0.0f)) return 0;
    return -1;
}

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("line_seg_plane_intersect", 0x0060e590, &line_plane);
#undef U1

}  // namespace game_units
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers22)
namespace {
namespace en3d_helpers22 {

struct H {
    void bone_dtor() { joan::orig::as_bone(this)->set_parent(joan::orig::arg::mptr(static_cast<void*>(nullptr))); }  // 0x0060ec40: set_parent(0)
};

#define HELPER22(name, addr, fn) JOAN_REPLACE("en3d/helpers22", name, addr, fn)
HELPER22("Frame::~Frame", 0x0060ec40, &H::bone_dtor);
#undef HELPER22

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx_2 {

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("operator-_60cbc0", 0x0060cbc0, &joan::orig::vec_negate);
FX1("operator*_60cd60", 0x0060cd60, &joan::orig::vec_scaled);
FX1("operator==", 0x0060ced0, &joan::orig::vec_equal);
#undef FX1

}  // namespace game_fx_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers20)
namespace {
namespace en3d_helpers20_2 {

float* JOAN_CDECL vec3_sub_assign(float* a, const float* b) {  // 0x0060ce20
    a[0] = a[0] - b[0];
    a[1] = a[1] - b[1];
    a[2] = a[2] - b[2];
    return a;
}

#define HELPER20(name, addr, fn) JOAN_REPLACE("en3d/helpers20", name, addr, fn)
HELPER20("operator-=", 0x0060ce20, &vec3_sub_assign);
#undef HELPER20

}  // namespace en3d_helpers20_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2_2 {

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Box3d::is_inside", 0x0060d5f0, &joan::orig::Aabb::contains);
#undef HELPER2

}  // namespace en3d_helpers2_2
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

using u8 = std::uint8_t;

using joan::call::import_call;

struct V3 {
    float x, y, z;
};


static_assert(sizeof(V3) == 12, "V3");

struct H {
    // ------------------------------------------------------------------ 0x0060d890
    bool touches(u8* other) {
        V3 tmp;
        const V3* const d = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(this), static_cast<void*>(other)));  // 0x0060cc90
        V3 v = *d;
        const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &v));  // length
        const float s = jo::self<jo::Sphere>(this)->radius + jo::as_sphere(other)->radius;
        return s >= len;  // fcompp: (s < len) or unordered -> 0
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Sphere3d::is_overlap", 0x0060d890, &H::touches);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

// 0x0060e6a0 segment (a = seg[0..2], b = seg[3..5]) vs plane z = plane: t on success (1),
// -1 when parallel and in the plane (or NaN), else 0.
std::int32_t JOAN_CDECL z_plane_hit(float* t, const float* seg, float plane) {
    if (seg[5] < seg[2] || seg[5] > seg[2]) {  // not equal and ordered
        *t = (plane - seg[2]) / (seg[5] - seg[2]);
        return 1;
    }
    if (plane < seg[2] || plane > seg[2]) return 0;
    return -1;
}

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("line_seg_plane_xy_intersect", 0x0060e6a0, &z_plane_hit);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

using i32 = std::int32_t;

// ---------------------------------------------------------------------------- 0x0060e720
static i32 JOAN_CDECL range_param(float* out, const float* range, float v) {
    const float r0 = range[0];
    const float r3 = range[3];
    if (r3 < r0 || r3 > r0) {  // not (equal or unordered)
        *out = (v - range[0]) / (range[3] - range[0]);
        return 1;
    }
    if (v < range[0] || v > range[0]) return 0;
    return -1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("line_seg_plane_yz_intersect", 0x0060e720, &range_param);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {


JOAN_COUNTER(c_key_fraction, "key_fraction");

// 0x0060e790
std::int32_t JOAN_CDECL key_fraction(float* out, const void* key, float t) {
    c_key_fraction.hit();
    const float a = static_cast<const float*>(key)[4], b = static_cast<const float*>(key)[1];  // key: segment p0, p1 (2 x 3 floats): the y components
    // fcomp; test ah,0x40; jne: equal or unordered skips.
    if (a < b || a > b) {
        *out = (t - b) / (a - b);
        return 1;
    }
    if (t < b || t > b) return 0;
    return -1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("line_seg_plane_zx_intersect", 0x0060e790, &key_fraction);
#undef B

}  // namespace game_b97_06
}  // namespace

// Out-of-line definitions from modules/game_fx.cpp (declared in original/vecmath.hpp, registered above).
namespace {

struct V3 {
    float x, y, z;
};

V3 load3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}
void store3(void* p, const V3& v) { std::memcpy(p, &v, sizeof v); }

bool equal_or_nan(float a, float b) { return !(a < b || a > b); }

}  // namespace

namespace joan::orig {
// 0x0060cbc0 out = -v.
Vec3* JOAN_CDECL vec_negate(void* out, const void* v) {
    const V3 a = load3(v);
    store3(out, V3{-a.x, -a.y, -a.z});
    return static_cast<Vec3*>(out);
}
// 0x0060cd60 out = k * v.
Vec3* JOAN_CDECL vec_scaled(void* out, float k, const void* v) {
    const V3 a = load3(v);
    store3(out, V3{k * a.x, k * a.y, k * a.z});
    return static_cast<Vec3*>(out);
}
// 0x0060ced0 a == b (x87: unordered counts as equal).
std::uint8_t JOAN_CDECL vec_equal(const void* a, const void* b) {
    return equal_or_nan(jo::as_vec3(a)->x, jo::as_vec3(b)->x) && equal_or_nan(jo::as_vec3(a)->y, jo::as_vec3(b)->y) &&
                   equal_or_nan(jo::as_vec3(a)->z, jo::as_vec3(b)->z)
               ? 1
               : 0;
}
}  // namespace joan::orig

// Out-of-line definitions from modules/en3d_helpers6.cpp (declared in original/vecmath.hpp, registered above).
namespace joan::orig {
Vec3* vec_add(void* out_, const void* a_, const void* b_) {  // 0x0060cc20
    Vec3* const out = static_cast<Vec3*>(out_);
    const Vec3* const a = static_cast<const Vec3*>(a_);
    const Vec3* const b = static_cast<const Vec3*>(b_);
    const Vec3 r = {a->x + b->x, a->y + b->y, a->z + b->z};
    *out = r;
    return out;
}
Vec3* vec_sub(void* out_, const void* a_, const void* b_) {  // 0x0060cc90
    Vec3* const out = static_cast<Vec3*>(out_);
    const Vec3* const a = static_cast<const Vec3*>(a_);
    const Vec3* const b = static_cast<const Vec3*>(b_);
    const Vec3 r = {a->x - b->x, a->y - b->y, a->z - b->z};
    *out = r;
    return out;
}
Vec3* vec_scale(void* out_, const void* v_, float s) {  // 0x0060cd00
    Vec3* const out = static_cast<Vec3*>(out_);
    const Vec3* const v = static_cast<const Vec3*>(v_);
    const Vec3 r = {s * v->x, s * v->y, s * v->z};
    *out = r;
    return out;
}
}  // namespace joan::orig

// Out-of-line definition from modules/en3d_helpers4.cpp (declared in original/vecmath.hpp, registered above).
namespace joan::orig {
Vec3* vec_add_in_place(void* a_, const void* b_) {  // 0x0060cdc0
    float* const a = static_cast<float*>(a_);
    const float* const b = static_cast<const float*>(b_);
    a[0] = a[0] + b[0];
    a[1] = a[1] + b[1];
    a[2] = a[2] + b[2];
    return static_cast<Vec3*>(a_);
}
}  // namespace joan::orig

// Out-of-line definition from modules/en3d_helpers2.cpp (declared in original/vecmath.hpp, registered above).
namespace joan::orig {
// 0x0060d5f0: box (min xyz, max xyz) contains the point (edges included; NaN passes).
bool Aabb::contains(const void* p_) {
    const float* const p = static_cast<const float*>(p_);
    const float* b = reinterpret_cast<const float*>(this);
    return !(b[0] > p[0]) && !(p[0] > b[3]) && !(b[1] > p[1]) && !(p[1] > b[4]) && !(b[2] > p[2]) &&
           !(p[2] > b[5]);
}
}  // namespace joan::orig

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::vec3_sub_assign(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers20_2::vec3_sub_assign>(a0, a1)); }  // 0x60ce20
auto joan::orig::Box::centre(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers6_2::H::box_centre>(this, a0)); }  // 0x60d3a0
auto joan::orig::Box::overlaps(void* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::en3d_helpers3_2::H::box_overlaps>(this, a0)); }  // 0x60d6b0
auto joan::orig::BoundSphere::touches(void* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b96_06::H::touches>(this, a0)); }  // 0x60d890
auto joan::orig::line_plane(void* a0, void* a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_units::line_plane>(a0, a1, a2)); }  // 0x60e590
auto joan::orig::z_plane_hit(void* a0, void* a1, float a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_03::z_plane_hit>(a0, a1, a2)); }  // 0x60e6a0
auto joan::orig::Bone::dtor() -> void { joan::bind::member<&::en3d_helpers22::H::bone_dtor>(this); }  // 0x60ec40
