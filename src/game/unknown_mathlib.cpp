// game/unknown_mathlib.cpp: functions whose original source file is unknown (formerly game/_unplaced/mathlib.cpp;
// re/attribution.csv: between ww/en3d/vertex.cpp and ww/en3d/vertex.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

void JOAN_CDECL init_60bad0() { joan::orig::init_60baf0(); }
void JOAN_CDECL init_60bb10() { joan::orig::copy_60bb30(); }

JOAN_REPLACE("game/b105_17", "init_60bad0", 0x0060bad0, &init_60bad0);
JOAN_REPLACE("game/b105_17", "init_60bb10", 0x0060bb10, &init_60bb10);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
namespace {
namespace game_b104_10 {

u32 JOAN_CDECL init_60baf0() {
    const u32 v = global<u32>(d::g_a18a6c);
    global<u32>(d::g_init_white) = v;
    return v;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("init_60baf0", 0x0060baf0, &init_60baf0);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

// 0x0060bb30 (cdecl)
u32 JOAN_CDECL copy_60bb30() {
    const u32 v = global<u32>(d::g_a18a70);
    global<u32>(d::g_init_black) = v;
    return v;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Init::copy_60bb30", 0x0060bb30, &copy_60bb30);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
namespace {
namespace en3d_b82_17 {

struct M64 {
    std::uint32_t m[16];
};

// 0x0060bbb0 (cdecl, hidden return pointer first)
M64* JOAN_CDECL matrix_from_quat(M64* out, const void* q) {
    M64 m;
    joan::orig::as_matrix(&m)->construct();                                   // 0x005bea30
    joan::orig::d3dx::matrix_rotation_x(joan::orig::arg::mptr(static_cast<void*>(&m)), joan::orig::arg::f32(q));       // D3DX
    *out = m;
    return out;
}

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("matrix_from_quat", 0x0060bbb0, &matrix_from_quat);
#undef B82_17E

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/en3d_helpers3.cpp (en3d/helpers3)
namespace {
namespace en3d_helpers3 {

struct H {
    float* fl() { return reinterpret_cast<float*>(this); }

    bool box_empty() {  // 0x0060cfb0: some min > max
        return fl()[0] > fl()[3] || fl()[1] > fl()[4] || fl()[2] > fl()[5];
    }
};

#define HELPER3(name, addr, fn) JOAN_REPLACE("en3d/helpers3", name, addr, fn)
HELPER3("Box::set_point", 0x0060d430, &joan::orig::Aabb::set_point);
HELPER3("Box::empty", 0x0060cfb0, &H::box_empty);
HELPER3("Box::extend", 0x0060d490, &joan::orig::Aabb::extend);
#undef HELPER3

}  // namespace en3d_helpers3
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

// --- cdecl vector maths (results built in a temporary, then stored: aliasing-safe) ---------
// 0x0060c000: copy a matrix with its second row negated; returns the source (eax as left).
const float* matrix_negate_row1(float* out, const float* in) {
    std::memcpy(out, in, 64);
    out[4] = -in[4];
    out[5] = -in[5];
    out[6] = -in[6];
    out[7] = -in[7];
    return in;
}


#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("matrix_negate_row1", 0x0060c000, &matrix_negate_row1);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/en3d_helpers29.cpp (en3d/helpers29)
namespace {
namespace en3d_helpers29 {

// fld dword + fstp dword: bit-exact copy except that a signalling NaN comes out quiet.
std::uint32_t x87_float_copy(std::uint32_t bits) {
    if ((bits & 0x7f800000u) == 0x7f800000u && (bits & 0x007fffffu) != 0 && (bits & 0x00400000u) == 0) bits |= 0x00400000u;
    return bits;
}
using joan::orig::helpers::as_float;

struct H {
    // 0x0060d030: box = min x/y/z (+0..+8), max x/y/z (+0xc..+0x14). Empty test 0x0060cfb0.
    // x is copied as a dword; the others go through fld / fstp (SNaN quieted). NaN handling
    // follows the original's compares.
    void* bbox_merge(std::uint32_t* other) {
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_box(other)->empty()) != 0) return this;  // 0x0060cfb0
        std::uint32_t* const self = reinterpret_cast<std::uint32_t*>(this);
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_box(this)->empty()) != 0) {
            for (int i = 0; i < 6; ++i) self[i] = other[i];
            return this;
        }
        // fcomp self.x, other.x; test ah,1 je: self.x < other.x or unordered -> self.x.
        self[0] = !(as_float(self[0]) >= as_float(other[0])) ? self[0] : other[0];
        // st0 = other, st1 = self; test ah,0x41 jne: other > self (ordered) -> self, else other.
        for (int i = 1; i <= 2; ++i) self[i] = x87_float_copy(as_float(other[i]) > as_float(self[i]) ? self[i] : other[i]);
        // test ah,1 je: other < self or unordered -> self, else other.
        for (int i = 3; i <= 5; ++i) self[i] = x87_float_copy(!(as_float(other[i]) >= as_float(self[i])) ? self[i] : other[i]);
        return this;
    }
};

#define HELPER29(name, addr, fn) JOAN_REPLACE("en3d/helpers29", name, addr, fn)
HELPER29("BBox::merge", 0x0060d030, &H::bbox_merge);
#undef HELPER29

}  // namespace en3d_helpers29
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
namespace {
namespace en3d_b83_03 {

using joan::img::obj;


// 0x0060cf40 (cdecl): fcomp / test ah,0x40 / je -> 1 per component (equal or NaN -> next).
std::uint8_t JOAN_CDECL vector_differs(const float* a, const float* b) {
    auto differs = [](float x, float y) { return x < y || x > y; };
    if (differs(a[0], b[0]) || differs(a[1], b[1]) || differs(a[2], b[2])) return 1;
    return 0;
}

// 0x0060bb50 (cdecl): out = identity (0xa18a78), translation row = v; returns out.
void* JOAN_CDECL matrix_translation(void* out, const std::uint32_t* v) {
    std::memcpy(out, obj(d::g_identity_matrix), 0x40);
    jo::view<std::uint32_t>(jo::as_matrix44(out)->m[12]) = v[0];
    jo::view<std::uint32_t>(jo::as_matrix44(out)->m[13]) = v[1];
    jo::view<std::uint32_t>(jo::as_matrix44(out)->m[14]) = v[2];
    return out;
}

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("vector_differs", 0x0060cf40, &vector_differs);
B83_03("matrix_translation", 0x0060bb50, &matrix_translation);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

using joan::call::thiscall;
using joan::call::stdcall_call;


// 0x0060bc00 rotation matrix (D3DX 0x0076700a) into a local, copied to out.
void* JOAN_CDECL matrix_rotation(void* out, float angle) {
    std::uint32_t m[16];
    joan::orig::as_matrix(m)->construct();  // 0x005bea30 Matrix::Matrix
    joan::orig::d3dx::matrix_rotation_y(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::f32(angle));
    std::memcpy(out, m, sizeof m);
    return out;
}
struct H {
    H* matrix_copy(const void* m) {                                // 0x0060e550
        std::memmove(this, m, 0x40);
        return this;
    }
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("Matrix::copy", 0x0060e550, &H::matrix_copy);
FX1("matrix_rotation", 0x0060bc00, &matrix_rotation);
FX1("vec_scale", 0x0060ce80, &joan::orig::vec_scale_in_place);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
namespace {
namespace game_b102_11 {

using joan::hook::global;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;


JOAN_COUNTER(c_quat, "quat_rot");

// fld dword + fchs + fstp dword: sign flip, a signalling NaN comes out quiet.
u32 x87_negate_bits(u32 bits) {
    if ((bits & 0x7f800000u) == 0x7f800000u && (bits & 0x007fffffu) != 0 && (bits & 0x00400000u) == 0) bits |= 0x00400000u;
    return bits ^ 0x80000000u;
}

// 0x0060c070 (cdecl): eax at the end is src.
const void* JOAN_CDECL matrix_negate_row1(void* dst, const void* src) {
    // rep movsd: forward dword copy.
    for (i32 i = 0; i < 16; ++i) jo::view<u32>(jo::as_matrix44(dst)->m[i]) = jo::view<u32>(jo::as_matrix44(src)->m[i]);
    for (i32 i = 4; i <= 7; ++i) jo::view<u32>(jo::as_matrix44(dst)->m[i]) = x87_negate_bits(jo::view<u32>(jo::as_matrix44(src)->m[i]));
    return src;
}

// 0x0060c620 / 0x0060c6a0: fld dword * fmul qword 0.5 (PC24) -> fstp qword -> sin / cos -> fstp dword.
double half_angle_d(float angle) {
    const Fpu fpu{true};
    return fpu.mul(angle, global<double>(d::dbl_0_5));
}

// 0x0060c620 (cdecl)
float* JOAN_CDECL quat_rot_x(float* out, float angle) {
    c_quat.hit();
    out[0] = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(half_angle_d(angle))));  // sin
    jo::view<u32>(out[1]) = 0;
    jo::view<u32>(out[2]) = 0;
    out[3] = joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(half_angle_d(angle))));  // cos
    return out;
}

// 0x0060c6a0 (cdecl)
float* JOAN_CDECL quat_rot_y(float* out, float angle) {
    c_quat.hit();
    jo::view<u32>(out[0]) = 0;
    out[1] = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(half_angle_d(angle))));  // sin
    jo::view<u32>(out[2]) = 0;
    out[3] = joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(half_angle_d(angle))));  // cos
    return out;
}

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("matrix_negate_row1", 0x0060c070, &matrix_negate_row1);
B11("quat_rot_x", 0x0060c620, &quat_rot_x);
B11("quat_rot_y", 0x0060c6a0, &quat_rot_y);
#undef B11

}  // namespace game_b102_11
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

using u32 = std::uint32_t;

// fld dword + fchs + fstp dword: sign flip, a signalling NaN comes out quiet.
u32 x87_negate_bits(u32 b) {
    if ((b & 0x7f800000u) == 0x7f800000u && (b & 0x007fffffu) != 0 && (b & 0x00400000u) == 0) b |= 0x00400000u;
    return b ^ 0x80000000u;
}

// 0x0060c0e0 / 0x0060c130
static u32 JOAN_CDECL flip_y(u32* out, const u32* in) {
    out[0] = in[0];
    out[1] = in[1];
    const u32 z = in[2];
    out[2] = z;
    out[1] = x87_negate_bits(in[1]);
    return z;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("flip_y_60c0e0", 0x0060c0e0, &flip_y);
B("flip_y_60c130", 0x0060c130, &flip_y);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {


// Every helper shares one class so `this` is just the object's address.
struct H {
    // --- Bone (en3d/bonetree.cpp area) --------------------------------------------------------------
    void bone_invalidate() {  // 0x0060ee10: changed - notify the owner's callback
        jo::self<jo::Bone>(this)->dirty = 1;
        if (jo::self<jo::Bone>(this)->callback != nullptr) reinterpret_cast<void(JOAN_CDECL*)(void*)>(jo::self<jo::Bone>(this)->callback)(jo::self<jo::Bone>(this)->owner);
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Bone::invalidate", 0x0060ee10, &H::bone_invalidate);
HELPER("Bone::matrix", 0x0060ef80, &joan::orig::Bone::matrix);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04 {


// 0x0060c360 (cdecl)
void* JOAN_CDECL set_30(void* dst, const std::uint32_t* v) {
    jo::view<std::uint32_t>(jo::as_matrix44(dst)->m[12]) = v[0];
    jo::view<std::uint32_t>(jo::as_matrix44(dst)->m[13]) = v[1];
    jo::view<std::uint32_t>(jo::as_matrix44(dst)->m[14]) = v[2];
    return dst;
}

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("set_30", 0x0060c360, &set_30);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;


JOAN_COUNTER(c_box_clip, "Box6::clip");
JOAN_COUNTER(c_box_dist, "Plane::box_max_dist");

// ------------------------------------------------------------------ 0x0060c720
u8* JOAN_CDECL quat_from_z_angle(u8* out, float a) {
    jo::view<u32>(jo::as_quat(out)->x) = 0;
    jo::view<u32>(jo::as_quat(out)->y) = 0;
    // fld a; fmul qword 0.5 (exact); fstp qword -> CRT; the result goes straight to a float.
    jo::as_quat(out)->z = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(a) * 0.5)));   // sin
    jo::as_quat(out)->w = joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(a) * 0.5))); // cos
    return out;
}

// ------------------------------------------------------------------ 0x0060db80
float JOAN_CDECL plane_box_max_dist(const u8* plane, const u8* box, u8* out) {
    c_box_dist.hit();
    const float zero = global<float>(d::k_9cf5c4);
    // fcomp; test ah,1 jne: n < 0 or unordered -> min corner (+0/+4/+8), else max (+0xc..).
    const u32 xb = !(jo::as_plane4(plane)->a >= zero) ? jo::view<u32>(jo::as_aabb(box)->min[0]) : jo::view<u32>(jo::as_aabb(box)->max[0]);
    const u32 yb = !(jo::as_plane4(plane)->b >= zero) ? jo::view<u32>(jo::as_aabb(box)->min[1]) : jo::view<u32>(jo::as_aabb(box)->max[1]);
    const u32 zb = !(jo::as_plane4(plane)->c >= zero) ? jo::view<u32>(jo::as_aabb(box)->min[2]) : jo::view<u32>(jo::as_aabb(box)->max[2]);
    if (out != nullptr) {
        jo::view<u32>(jo::as_vec3(out)->x) = xb;
        jo::view<u32>(jo::as_vec3(out)->y) = yb;
        jo::view<u32>(jo::as_vec3(out)->z) = zb;
    }
    float x, y, z;
    std::memcpy(&x, &xb, 4);
    std::memcpy(&y, &yb, 4);
    std::memcpy(&z, &zb, 4);
    // one statement per x87 op (no contraction)
    const float px = x * jo::as_plane4(plane)->a;
    const float py = y * jo::as_plane4(plane)->b;
    const float s = px + py;
    const float pz = z * jo::as_plane4(plane)->c;
    const float t = s + pz;
    return t + jo::as_plane4(plane)->d;
}

struct H {
    // ------------------------------------------------------------------ 0x0060d210
    H* box_clip(u8* o) {
        c_box_clip.hit();
        // fld this; fcomp other; test ah,0x41 jne -> this only when this > other.
        jo::view<u32>(jo::self<jo::Aabb>(this)->min[0]) = (jo::self<jo::Aabb>(this)->min[0] > jo::as_aabb(o)->min[0]) ? jo::view<u32>(jo::self<jo::Aabb>(this)->min[0]) : jo::view<u32>(jo::as_aabb(o)->min[0]);
        // fld this; fld other; fcompp; test ah,1 je -> other unless other < this (or NaN).
        jo::self<jo::Aabb>(this)->min[1] = !(jo::as_aabb(o)->min[1] >= jo::self<jo::Aabb>(this)->min[1]) ? jo::self<jo::Aabb>(this)->min[1] : jo::as_aabb(o)->min[1];
        jo::self<jo::Aabb>(this)->min[2] = !(jo::as_aabb(o)->min[2] >= jo::self<jo::Aabb>(this)->min[2]) ? jo::self<jo::Aabb>(this)->min[2] : jo::as_aabb(o)->min[2];
        // fld this; fld other; fcompp; test ah,0x41 jne -> this only when other > this.
        jo::self<jo::Aabb>(this)->max[0] = (jo::as_aabb(o)->max[0] > jo::self<jo::Aabb>(this)->max[0]) ? jo::self<jo::Aabb>(this)->max[0] : jo::as_aabb(o)->max[0];
        jo::self<jo::Aabb>(this)->max[1] = (jo::as_aabb(o)->max[1] > jo::self<jo::Aabb>(this)->max[1]) ? jo::self<jo::Aabb>(this)->max[1] : jo::as_aabb(o)->max[1];
        jo::self<jo::Aabb>(this)->max[2] = (jo::as_aabb(o)->max[2] > jo::self<jo::Aabb>(this)->max[2]) ? jo::self<jo::Aabb>(this)->max[2] : jo::as_aabb(o)->max[2];
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("Quat::from_z_angle", 0x0060c720, &quat_from_z_angle);
B("Box6::clip", 0x0060d210, &H::box_clip);
B("Plane::box_max_dist", 0x0060db80, &plane_box_max_dist);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

using joan::call::cdecl_call;

// 0x0060c7a0 quat_from_axis_angle(out, axis, angle)
float* JOAN_CDECL quat_from_axis_angle(float* out, const float* axis, float angle) {
    const float s = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(angle) * 0.5)));  // sin
    out[0] = s * axis[0];
    out[1] = s * axis[1];
    out[2] = s * axis[2];
    out[3] = joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(angle) * 0.5)));  // cos
    return out;
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("quat_from_axis_angle", 0x0060c7a0, &quat_from_axis_angle);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers36.cpp (en3d/helpers36)
namespace {
namespace en3d_helpers36 {

using joan::call::thiscall;
using joan::call::stdcall_call;

using joan::hook::x87::Fpu;

using joan::img::obj;

using joan::call::import_call;

struct alignas(4) Matrix {
    float m[16];
};

// 0x0060c840 quat_from_matrix(out, matrix).
void* JOAN_CDECL quat_from_matrix(void* out, void* matrix) {
    Matrix m;
    joan::orig::as_matrix(&m)->copy(joan::orig::arg::mptr(matrix));                               // 0x0060e550
    return joan::orig::arg::as<void*>(joan::orig::d3dx::quaternion_rotation_matrix(joan::orig::arg::mptr(out), joan::orig::arg::mptr(static_cast<void*>(&m))));  // D3DXQuaternionRotationMatrix
}

#define HELPER36(name, addr, fn) JOAN_REPLACE("en3d/helpers36", name, addr, fn)
HELPER36("quat_from_matrix", 0x0060c840, &quat_from_matrix);
#undef HELPER36

}  // namespace en3d_helpers36
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

using joan::call::stdcall_call;

void* JOAN_CDECL quaternion_to_matrix(void* out, const void* q) {  // 0x0060c880
    joan::orig::d3dx::matrix_rotation_quaternion(joan::orig::arg::mptr(out), joan::orig::arg::mptr(q));                      // D3DXMatrixRotationQuaternion
    return out;
}
void* JOAN_CDECL quaternion_slerp(void* out, const void* a, const void* b, float t) {  // 0x0060c8b0
    return joan::orig::arg::as<void*>(joan::orig::d3dx::quaternion_slerp(joan::orig::arg::mptr(out), joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), joan::orig::arg::f32(t)));                             // D3DXQuaternionSlerp
}

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
HELPER16("quaternion_to_matrix", 0x0060c880, &quaternion_to_matrix);
HELPER16("quaternion_slerp", 0x0060c8b0, &quaternion_slerp);
#undef HELPER16

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers20)
namespace {
namespace en3d_helpers20 {

using joan::img::obj;

using joan::call::import_call;


struct H {
    void bone_set_owner(void* owner, void* callback) {         // 0x0060edd0
        jo::self<jo::Bone>(this)->callback = callback;
        jo::self<jo::Bone>(this)->owner = owner;
    }
    H* bone_ctor() {  // 0x0060eb90
        jo::self<jo::Bone>(this)->parent = nullptr;
        jo::self<jo::Bone>(this)->next = nullptr;
        jo::self<jo::Bone>(this)->prev = nullptr;
        jo::self<jo::Bone>(this)->last_child = nullptr;
        jo::self<jo::Bone>(this)->first_child = nullptr;
        jo::self<jo::Bone>(this)->callback = nullptr;
        jo::self<jo::Bone>(this)->owner = nullptr;
        std::memcpy(this, img::obj(d::g_identity_matrix), 64);  // identity
        std::memcpy(jo::self<jo::Bone>(this)->matrix_, this, 64);
        jo::self<jo::Bone>(this)->dirty = 0;
        return this;
    }
};

#define HELPER20(name, addr, fn) JOAN_REPLACE("en3d/helpers20", name, addr, fn)
HELPER20("Bone::set_owner", 0x0060edd0, &H::bone_set_owner);
HELPER20("Bone::Bone", 0x0060eb90, &H::bone_ctor);
#undef HELPER20

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {

using joan::call::thiscall;

using joan::call::import_call;


// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    // --- Bone (0x0060exxx) ----------------------------------------------------------------------
    // 0x0060ee60: world matrix (+0x58) = local (+0) * parent's world when dirty (+0x54, or the
    // parent changed); then the callback (+0x98 with +0x9c) and the children (+0x4c, next +0x48)
    // are marked dirty. Returns 1 when it recomputed.
    // The parent's update and the matrix product are called directly (this function and v3d::matrix_multiply, what
    // the dispatcher and the v3d_87 import table lead to): the bone chain is walked for every bone of every model.
    std::int32_t bone_update() {
        void* const parent = jo::self<jo::Bone>(this)->parent;
        if (parent && static_cast<H*>(parent)->bone_update() != 0) jo::self<jo::Bone>(this)->dirty = 1;  // 0x0060ee60
        if (jo::self<jo::Bone>(this)->dirty == 0) return 0;
        if (jo::self<jo::Bone>(this)->parent)
            joan::render::v3d::matrix_multiply(reinterpret_cast<joan::render::v3d::Matrix*>(jo::self<jo::Bone>(this)->matrix_),
                                               reinterpret_cast<const joan::render::v3d::Matrix*>(jo::self<jo::Bone>(this)->local),
                                               reinterpret_cast<const joan::render::v3d::Matrix*>(jo::self<jo::Bone>(this)->parent->matrix_));  // matrix_multiply(out, a, b)
        else
            std::memcpy(jo::self<jo::Bone>(this)->matrix_, jo::self<jo::Bone>(this)->local, 64);
        if (jo::view<std::uintptr_t>(jo::self<jo::Bone>(this)->callback) != 0)
            reinterpret_cast<void(JOAN_CDECL*)(void*)>(jo::view<std::uintptr_t>(jo::self<jo::Bone>(this)->callback))(jo::self<jo::Bone>(this)->owner);
        for (void* child = jo::self<jo::Bone>(this)->first_child; child; child = jo::as_bone(child)->next) jo::as_bone(child)->dirty = 1;
        jo::self<jo::Bone>(this)->dirty = 0;
        return 1;
    }
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Bone::update", 0x0060ee60, &H::bone_update);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

using joan::call::import_call;

struct Vec3 {
    float x, y, z;
};


constexpr img::Addr kLength = d::iat_v3d_87_length;  // v3d_87.dll length(const D3DVECTOR&)

JOAN_COUNTER(c_sphere_contains, "Sphere::contains");

struct H {
    // 0x0060d810
    std::uint8_t sphere_contains(const Vec3* p) {
        c_sphere_contains.hit();
        Vec3 tmp;
        const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(p), static_cast<void*>(this)));  // 0x0060cc90 sub
        const float len = static_cast<float>(import_call<double>(kLength, &d));
        return (len > jo::self<jo::Sphere>(this)->radius) ? 0 : 1;
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Sphere::contains", 0x0060d810, &H::sphere_contains);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


static float JOAN_CDECL box_plane_distance(const float* n, const u8* box, u32* out) {  // 0x0060da80
    const float zero = global<float>(d::k_9cf5c4);
    const u32 x = !(n[0] >= zero) ? jo::view<u32>(jo::as_aabb(box)->max[0]) : jo::view<u32>(jo::as_aabb(box)->min[0]);
    const u32 y = !(n[1] >= zero) ? jo::view<u32>(jo::as_aabb(box)->max[1]) : jo::view<u32>(jo::as_aabb(box)->min[1]);
    const u32 z = !(n[2] >= zero) ? jo::view<u32>(jo::as_aabb(box)->max[2]) : jo::view<u32>(jo::as_aabb(box)->min[2]);
    if (out != nullptr) {
        out[0] = x;
        out[1] = y;
        out[2] = z;
    }
    float fx, fy, fz;
    std::memcpy(&fx, &x, 4);
    std::memcpy(&fy, &y, 4);
    std::memcpy(&fz, &z, 4);
    return ((fx * n[0] + fy * n[1]) + fz * n[2]) + n[3];
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("box_plane_distance", 0x0060da80, &box_plane_distance);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------------ 0x0060efb0
    u32 any_flag() {
        u32 r = joan::orig::arg::as<u32>(joan::orig::as_bone(this)->update());  // 0x0060ee60
        for (u8* c = jo::view<u8*>(jo::self<jo::Bone>(this)->first_child); c != nullptr; c = jo::view<u8*>(jo::as_bone(c)->next)) r |= joan::orig::arg::as<u32>(joan::orig::as_bone(c)->update_tree());  // 0x0060efb0
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("Node::any_flag", 0x0060efb0, &H::any_flag);
#undef B

}  // namespace game_b108_17
}  // namespace

// Out-of-line definitions from modules/game_fx.cpp (declared in original/vecmath.hpp, registered above).
namespace {

// (used by the joan::orig definitions below, outside the module namespaces)
using joan::orig::helpers::at;


}  // namespace

namespace joan::orig {
// 0x0060ce80 v *= k.
Vec3* JOAN_CDECL vec_scale_in_place(void* v, float k) {
    jo::as_vec3(v)->x = k * jo::as_vec3(v)->x;
    jo::as_vec3(v)->y = k * jo::as_vec3(v)->y;
    jo::as_vec3(v)->z = k * jo::as_vec3(v)->z;
    return static_cast<Vec3*>(v);
}
}  // namespace joan::orig

// Out-of-line definitions from modules/en3d_helpers3.cpp (declared in original/vecmath.hpp, registered above).
namespace {

bool below_or_nan(float a, float b) { return !(a >= b); }

}  // namespace

namespace joan::orig {

// --- Box (min xyz, max xyz) ------------------------------------------------------------------
Aabb* Aabb::set_point(const void* p_) {  // 0x0060d430
    const Vec3* const p = static_cast<const Vec3*>(p_);
    jo::view<Vec3>(min) = *p;
    jo::view<Vec3>(max) = jo::view<Vec3>(min);
    return this;
}
Aabb* Aabb::extend(const void* p_) {  // 0x0060d490
    const float* const p = static_cast<const float*>(p_);
    if (joan::orig::arg::as<bool>(joan::orig::as_box(this)->empty())) {  // 0x0060cfb0
        jo::view<Vec3>(min) = *reinterpret_cast<const Vec3*>(p);
        jo::view<Vec3>(max) = jo::view<Vec3>(min);
        return this;
    }
    float* b = reinterpret_cast<float*>(this);
    if (below_or_nan(p[0], b[0])) b[0] = p[0];
    if (below_or_nan(p[1], b[1])) b[1] = p[1];
    if (below_or_nan(p[2], b[2])) b[2] = p[2];
    if (p[0] > b[3]) b[3] = p[0];
    if (p[1] > b[4]) b[4] = p[1];
    if (p[2] > b[5]) b[5] = p[2];
    return this;
}
}  // namespace joan::orig


std::uint8_t* joan::orig::Bone::matrix() {  // 0x0060ef80
    reinterpret_cast<::en3d_helpers2::H*>(this)->bone_update();  // 0x0060ee60 update (called directly)
    return matrix_;
}



// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_60baf0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_10::init_60baf0>()); }  // 0x60baf0
auto joan::orig::copy_60bb30() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_11::copy_60bb30>()); }  // 0x60bb30
auto joan::orig::matrix_translation(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_b83_03::matrix_translation>(a0, a1)); }  // 0x60bb50
auto joan::orig::matrix_from_quat(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_b82_17::matrix_from_quat>(a0, a1)); }  // 0x60bbb0
auto joan::orig::matrix_rotation_60bc00(void* a0, float a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_fx::matrix_rotation>(a0, a1)); }  // 0x60bc00
auto joan::orig::matrix_negate_row1(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers6::matrix_negate_row1>(a0, a1)); }  // 0x60c000
auto joan::orig::set_30(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_b85_04::set_30>(a0, a1)); }  // 0x60c360
auto joan::orig::quat_from_axis_angle(void* a0, void* a1, float a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b94_06::quat_from_axis_angle>(a0, a1, a2)); }  // 0x60c7a0
auto joan::orig::quat_from_matrix(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers36::quat_from_matrix>(a0, a1)); }  // 0x60c840
auto joan::orig::quaternion_to_matrix(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers16::quaternion_to_matrix>(a0, a1)); }  // 0x60c880
auto joan::orig::quaternion_slerp(void* a0, void* a1, void* a2, float a3) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers16::quaternion_slerp>(a0, a1, a2, a3)); }  // 0x60c8b0
auto joan::orig::vector_differs(void* a0, void* a1) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::en3d_b83_03::vector_differs>(a0, a1)); }  // 0x60cf40
auto joan::orig::Box::empty() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::en3d_helpers3::H::box_empty>(this)); }  // 0x60cfb0
auto joan::orig::Box::merge(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers29::H::bbox_merge>(this, a0)); }  // 0x60d030
auto joan::orig::BoundSphere::contains(void* a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_leaves80b::H::sphere_contains>(this, a0)); }  // 0x60d810
auto joan::orig::box_plane_distance(void* a0, void* a1, void* a2) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b100_11::box_plane_distance>(a0, a1, a2)); }  // 0x60da80
auto joan::orig::Matrix::copy(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::matrix_copy>(this, a0)); }  // 0x60e550
auto joan::orig::Bone::Bone_60eb90() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers20::H::bone_ctor>(this)); }  // 0x60eb90
auto joan::orig::Bone::set_owner(void* a0, void* a1) -> void { joan::bind::member<&::en3d_helpers20::H::bone_set_owner>(this, a0, a1); }  // 0x60edd0
auto joan::orig::Bone::invalidate() -> void { joan::bind::member<&::en3d_helpers::H::bone_invalidate>(this); }  // 0x60ee10
auto joan::orig::Bone::update() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers2::H::bone_update>(this)); }  // 0x60ee60
auto joan::orig::Bone::update_tree() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_17::H::any_flag>(this)); }  // 0x60efb0
