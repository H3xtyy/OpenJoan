// game/unknown_collide.cpp: functions whose original source file is unknown (formerly game/_unplaced/collide.cpp;
// re/attribution.csv: between ww/enphysic/collis_s.cpp and ww/enphysic/collisio.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../fp/fma.hpp"
#include "../original/helpers.hpp"
#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;

using joan::img::obj;

struct Shape30 {  // 0x30 bytes by value: three axis vectors, centre at +0x24
    float v[12];
};
struct Box18 {  // min (+0) / max (+0xc)
    float v[6];
};

using joan::hook::x87::Fpu;

struct Box48 {
    u32 v[12];
};
struct Box24 {
    u32 v[6];
};

using i16 = std::int16_t;

struct Mat {
    float m[16];
};

JOAN_COUNTER(c_overlap, "shapes_overlap");

struct Plane4 {
    float a, b, c, d;
};

struct Capsule {  // point + radius (a) + height (b)
    float x, y, z, a, b;
};
struct Matrix {
    float m[16];
};

}  // namespace

// ---------------------------------------- from modules/game_b104_13.cpp (game/b104_13)
namespace {
namespace game_b104_13 {

using joan::call::cdecl_call;

using joan::call::import_call;

// x87 `fcomp` + `test ah,0x40` jnz: equal or unordered.
bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }

JOAN_COUNTER(c_slab, "slab_box_hit");
JOAN_COUNTER(c_cyl, "cylinder_sweep_hit");

// ------------------------------------------------------------------ 0x00694b80
u32 JOAN_CDECL slab_box_hit(float* out, float ax, float ay, float az, float bx, float by, float bz, float dx, float dy,
                         float dz, float mnx, float mny, float mnz, float mxx, float mxy, float mxz) {
    c_slab.hit();
    const float a[3] = {ax, ay, az};
    const float b[3] = {bx, by, bz};
    const float d[3] = {dx, dy, dz};
    const float mn[3] = {mnx, mny, mnz};
    const float mx[3] = {mxx, mxy, mxz};
    float lo = 0.0f;  // [ebp-0xc]
    float hi = 1.0f;  // [ebp-0x10]
    for (i32 k = 0; k < 3; ++k) {
        if (!eq_or_nan(d[k], global<float>(d::k_9cf5c4))) {
            float v[3];
            float t = mn[k] - b[k];
            v[0] = t / d[k];
            t = mn[k] - a[k];
            v[1] = t / d[k];
            t = mx[k] - b[k];
            v[2] = t / d[k];
            t = mx[k] - a[k];
            float vmax = t / d[k];  // [ebp-0x8]
            float vmin = vmax;      // [ebp-0x4]
            for (i32 i = 0; i < 3; ++i) {
                if (!(v[i] >= vmin)) {
                    vmin = v[i];
                } else if (v[i] > vmax) {
                    vmax = v[i];
                }
            }
            if (!(lo >= vmin)) lo = vmin;
            if (!(vmax >= hi)) hi = vmax;
            if (!(hi >= lo)) return 0;
        } else {
            if ((mn[k] > b[k]) || (b[k] > mx[k])) {
                if (mn[k] > a[k]) return 0;
                if (a[k] > mx[k]) return 0;
            }
        }
    }
    *out = lo;
    return 1;
}

// ------------------------------------------------------------------ 0x006994d0
u32 JOAN_CDECL cylinder_sweep_hit(float* out, float x, float y, float z, float ra, float ha, float dx, float dy, float dz,
                               float xb, float yb, float zb, float rb, float hb) {
    c_cyl.hit();
    const float zero = global<float>(d::k_9cf5c4);
    if (eq_or_nan(ra, zero) || eq_or_nan(rb, zero) || eq_or_nan(ha, zero) || eq_or_nan(hb, zero)) return 0;
    float circ_a[4] = {x, y, 0.0f, ra};    // [ebp-0x10]
    float dir[3] = {dx, dy, 0.0f};         // [ebp-0x2c]
    float circ_b[4] = {xb, yb, 0.0f, rb};  // [ebp-0x20]
    float t = 0.0f;                        // [ebp-0x30], written by 0x006a0d80 whenever it returns 1
    const u32 r = joan::orig::arg::as<u32>(joan::orig::sphere_sweep(&t, static_cast<void*>(circ_a), static_cast<void*>(dir),
                                  static_cast<void*>(circ_b)));  // 0x006a0d80
    if ((r & 0xff) == 0) return 0;
    const float half = global<float>(d::flt_0_5);  // 0.5
    {
        float p = half * ha;
        float a1 = z - p;
        float q = dz * t;
        a1 = a1 + q;  // [ebp-0x34]
        p = half * ha;
        float a2 = p + z;
        q = dz * t;
        a2 = a2 + q;  // [ebp-0x38]
        p = half * hb;
        const float b1 = zb - p;  // [ebp-0x3c]
        p = half * hb;
        const float b2 = p + zb;  // [ebp-0x40]
        bool overlap;
        if (!(b1 > a1) && !(a1 > b2)) {
            overlap = true;
        } else if (!(b1 > a2) && !(a2 > b2)) {
            overlap = true;
        } else if (!(a1 > b1) && !(b1 > a2)) {
            overlap = true;
        } else if (a1 > b2) {
            overlap = false;
        } else if (b2 > a2) {
            overlap = false;
        } else {
            overlap = true;
        }
        if (overlap && !eq_or_nan(b1, a2) && !eq_or_nan(a1, b2)) {
            *out = t;
            return 1;
        }
    }
    if (z > zb && dz > zero) return 0;
    if (!(z >= zb) && !(dz >= zero)) return 0;
    float e1, e2;  // [ebp-0x44], [ebp-0x48]
    if (!(dz >= zero)) {
        float p = half * ha;
        e1 = z - p;
        p = half * hb;
        e2 = p + zb;
        if (!(e1 >= e2)) return 0;
    } else {
        if (!(dz > zero)) return 0;
        float p = half * ha;
        e1 = p + z;
        p = half * hb;
        e2 = zb - p;
        if (e1 > e2) return 0;
    }
    float t2 = e2 - e1;
    t2 = t2 / dz;
    if (zero > t2) return 0;
    if (t2 > global<float>(d::flt_1)) return 0;  // 1.0
    float v[3];
    float s = dx * t2;
    s = s + x;
    v[0] = s - xb;
    s = dy * t2;
    s = s + y;
    v[1] = s - yb;
    v[2] = 0.0f;
    const float rs = ra + rb;
    const float lsq = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(v)));  // length_sq
    const float rr = rs * rs;
    if (!(rr > lsq)) return 0;
    *out = t2;
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b104_13", name, addr, fn)
B("slab_box_hit", 0x00694b80, &slab_box_hit);
B("cylinder_sweep_hit", 0x006994d0, &cylinder_sweep_hit);
#undef B

}  // namespace game_b104_13
}  // namespace

// ---------------------------------------- from modules/game_b109_09.cpp (game/b109_09)
namespace {
namespace game_b109_09 {

using joan::call::cdecl_call;

using joan::call::import_call;

u32 bits(float v) {
    u32 r;
    std::memcpy(&r, &v, 4);
    return r;
}

struct Vec3f {
    float x, y, z;
};
struct Plane16 {
    u32 d[4];
};
// The 22 dwords 0x0069ea70 takes after `out`, pushed as one by-value block (same stack image as the separate
// shape / direction / plane / extent arguments).
struct FaceHitArgs {
    Shape30 s;
    Vec3f d;
    Plane16 p;
    u32 e[3];
};
static_assert(sizeof(FaceHitArgs) == 88, "FaceHitArgs");

JOAN_COUNTER(c_sweep, "box_sweep_planes_694fc0");

// ---------------------------------------------------------------------------------------------
// 0x00694fc0
u8 JOAN_CDECL box_sweep_planes_694fc0(float* out, Shape30 s, Vec3f d, Box18 b, u8 corners) {
    c_sweep.hit();
    if ((global<u8>(d::g_a6a3f8) & 1u) == 0) {
        global<u8>(d::g_a6a3f8) = static_cast<u8>(global<u8>(d::g_a6a3f8) | 1u);
        const u32 kPlanes[6][4] = {
            {0u, 0u, 0x3f800000u, 0u}, {0u, 0u, 0xbf800000u, 0u}, {0x3f800000u, 0u, 0u, 0u},
            {0u, 0x3f800000u, 0u, 0u}, {0xbf800000u, 0u, 0u, 0u}, {0u, 0xbf800000u, 0u, 0u},
        };
        for (i32 i = 0; i < 6; ++i) {
            u32 tmp[4];
            const u32* const r =
                joan::orig::arg::as<u32*>(joan::orig::as_plane_equation(tmp)->construct(joan::orig::arg::u32(kPlanes[i][0]), joan::orig::arg::u32(kPlanes[i][1]), joan::orig::arg::u32(kPlanes[i][2]), joan::orig::arg::u32(kPlanes[i][3])));  // 0x00699480
            std::memcpy(obj(d::g_a6a460 + static_cast<u32>(i) * 16u), r, 16);
        }
        joan::crt::atexit(fn::code_ptr(fn::dtor_695930));  // atexit (the pointer overload: the CRT table owns the width)
    }
    if (corners != 0) {
        if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(&b)->contains(&s.v[9])) & 0xffu) != 0) {  // 0x0060d5f0 point in box
            *out = 0.0f;
            return 1;
        }
        // Sign classification of the axes against d: only feeds dead locals.
        import_call<double>(d::iat_v3d_87_dot_product, static_cast<const void*>(&s.v[0]), static_cast<const void*>(&d));
        import_call<double>(d::iat_v3d_87_dot_product, static_cast<const void*>(&s.v[3]), static_cast<const void*>(&d));
        import_call<double>(d::iat_v3d_87_dot_product, static_cast<const void*>(&s.v[6]), static_cast<const void*>(&d));
        for (i32 i = -1; i <= 0; ++i)
            for (i32 j = -1; j <= 0; ++j)
                for (i32 k = -1; k <= 0; ++k) {
                    float p[3];
                    for (i32 c = 0; c < 3; ++c) {
                        const float a0 = i >= 0 ? s.v[c] : -s.v[c];
                        const float a1 = j >= 0 ? s.v[3 + c] : -s.v[3 + c];
                        const float a2 = k >= 0 ? s.v[6 + c] : -s.v[6 + c];
                        p[c] = s.v[9 + c] - (a0 + a1 + a2) * 0.5f;
                    }
                    if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(&b)->contains(joan::orig::arg::ptr(p))) & 0xffu) != 0) {  // 0x0060d5f0
                        *out = 0.0f;
                        return 1;
                    }
                }
    }
    Plane16* const planes = &img::global<Plane16>(d::g_a6a460);
    if (!(b.v[0] < 0.0f || b.v[0] > 0.0f) && !(b.v[1] < 0.0f || b.v[1] > 0.0f) && !(b.v[2] < 0.0f || b.v[2] > 0.0f)) {
        global<float>(d::g_a6a46c) = -b.v[5];
        global<float>(d::g_a6a48c) = -b.v[3];
        global<float>(d::g_a6a49c) = -b.v[4];
        for (i32 n = 0; n < 6; ++n) {
            float t = 0.0f;
            const FaceHitArgs g{s, d, planes[n], {bits(b.v[3]), bits(b.v[4]), bits(b.v[5])}};
            if ((joan::orig::arg::as<u32>(joan::orig::box_fns::face_hit_69ea70(&t, g)) & 0xffu) != 0) {  // 0x0069ea70
                std::memcpy(out, &t, 4);
                return 1;
            }
        }
        return 0;
    }
    global<float>(d::g_a6a46c) = -(b.v[5] - b.v[2]);
    global<float>(d::g_a6a48c) = -(b.v[3] - b.v[0]);
    global<float>(d::g_a6a49c) = -(b.v[4] - b.v[1]);
    Shape30 local = s;
    local.v[9] = local.v[9] - b.v[0];
    local.v[10] = local.v[10] - b.v[1];
    local.v[11] = local.v[11] - b.v[2];
    for (i32 n = 0; n < 6; ++n) {
        const float ez = b.v[5] - b.v[2];
        const float ey = b.v[4] - b.v[1];
        const float ex = b.v[3] - b.v[0];
        float t = 0.0f;
        const FaceHitArgs g{local, d, planes[n], {bits(ex), bits(ey), bits(ez)}};
        if ((joan::orig::arg::as<u32>(joan::orig::box_fns::face_hit_69ea70(&t, g)) & 0xffu) != 0) {  // 0x0069ea70
            std::memcpy(out, &t, 4);
            return 1;
        }
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_09", name, addr, fn)
B("box_sweep_planes_694fc0", 0x00694fc0, &box_sweep_planes_694fc0);
#undef B

}  // namespace game_b109_09
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

// 0x00695930 / 0x006970a0: empty atexit destructors.
void JOAN_CDECL empty_cdecl() {}

// 0x006a2a50: eax = arg6 with its low byte cleared.
u32 JOAN_CDECL stub_6a2a50(u32, u32, u32, u32, u32, u32 a6) { return a6 & 0xffffff00u; }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("dtor_695930", 0x00695930, &empty_cdecl);
B("dtor_6970a0", 0x006970a0, &empty_cdecl);
B("stub_6a2a50", 0x006a2a50, &stub_6a2a50);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b111_19.cpp (game/b111_19)
namespace {
namespace game_b111_19 {

using joan::call::cdecl_call;

using joan::call::import_call;

using joan::orig::helpers::gf;

struct Mat44 {
    float m[16];
};

u32 fbits(float f) {
    u32 b = 0;
    std::memcpy(&b, &f, 4);
    return b;
}

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");
static_assert(sizeof(Box48) == 48, "box48");
static_assert(sizeof(Box24) == 24, "box24");
static_assert(sizeof(Mat44) == 64, "mat44");

const Fpu fpu{true};

JOAN_COUNTER(c_sweep, "Collide::sweep_test_695950");

// ---- 0x00695950 (dead code) -------------------------------------------------------------------
struct Sphere {  // {x, y, 0, radius}
    float x, y, z, r;
};
float sphere_radius(const void* a, const void* b) {  // sqrt(|a|^2 + |b|^2) * 0.5
    const float la = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, a));  // length_sq
    const float lb = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, b));
    const float s = lb + la;
    const double q = joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(s)));  // 0x004256d0 sqrt wrapper (st0)
    return static_cast<float>(fpu.mul(q, gf(d::flt_0_5)));
}
u32 JOAN_CDECL sweep_test(float* out, Box48 a, Vec3 p, Box48 b) {
    c_sweep.hit();
    Sphere sa{0.0f, 0.0f, 0.0f, 0.0f};  // [ebp-0xa8]
    std::memcpy(&sa.x, &a.v[9], 4);
    std::memcpy(&sa.y, &a.v[10], 4);
    sa.r = sphere_radius(&a.v[0], &a.v[3]);
    Sphere sb{0.0f, 0.0f, 0.0f, 0.0f};  // [ebp-0xb8]
    std::memcpy(&sb.x, &b.v[9], 4);
    std::memcpy(&sb.y, &b.v[10], 4);
    sb.r = sphere_radius(&b.v[0], &b.v[3]);
    Vec3 cp{p.x, p.y, 0.0f};  // [ebp-0xc4]
    float t = 0.0f;           // [ebp-0x98]
    if ((joan::orig::arg::as<u32>(joan::orig::sphere_sweep(&t, &sa, &cp, &sb)) & 0xffu) == 0) return 0;  // 0x006a0d80
    const u32 flag = !(t < 0.0f || t > 0.0f) ? 1u : 0u;  // fcomp 0; test ah,0x40: equal or unordered
    Box24 s24{};  // [ebp-0x58]
    Mat44 m{};    // [ebp-0x40]
    Box48 s48{};  // [ebp-0x88]
    Vec3 tv{0.0f, 0.0f, 0.0f};  // [ebp-0x94]
    joan::orig::obb_to_box_frame(&s24, &m, &b);    // 0x006a2b80
    joan::orig::box_fns::transform(&s48, &a, &m);    // 0x006a2fa0
    import_call<void*>(d::iat_v3d_87_vector_transform_3, &tv, &p, &m);    // vector_transform(out, in, m) (result unused)
    if ((joan::orig::arg::as<u32>(joan::orig::box_sweep_planes_694fc0(&t, s48, p, s24, static_cast<u8>(flag))) & 0xffu) != 0) {  // 0x00694fc0
        *out = t;
        return (joan::call::register_low_bits(out) & 0xffffff00u) | 1u;  // eax = out, al = 1; dead code, no caller of 0x00695950
    }
    joan::orig::obb_to_box_frame(&s24, &m, &a);  // 0x006a2b80
    joan::orig::box_fns::transform(&s48, &b, &m);  // 0x006a2fa0
    tv.x = -p.x;
    tv.y = -p.y;
    tv.z = -p.z;
    import_call<void*>(d::iat_v3d_87_vector_transform_2, &tv, &m);  // vector_transform(v, m) in place
    if ((joan::orig::arg::as<u32>(joan::orig::box_sweep_planes_694fc0(&t, s48, tv, s24, static_cast<u8>(flag))) & 0xffu) != 0) {
        *out = t;
        return (fbits(t) & 0xffffff00u) | 1u;
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b111_19", name, addr, fn)
B("Collide::sweep_test_695950", 0x00695950, &sweep_test);
#undef B

}  // namespace game_b111_19
}  // namespace

// ---------------------------------------- from modules/game_b107_13.cpp (game/b107_13)
namespace {
namespace game_b107_13 {

using joan::call::cdecl_call;

using joan::call::import_call;

JOAN_COUNTER(c_xform_box, "transform_box_695ce0");

// ---- dead vector helpers ----
struct Blk12 {
    u32 d[3];
};
struct Blk20 {
    u32 d[5];
};
struct Blk24 {
    u32 d[6];
};
struct Blk36 {
    u32 d[9];
};
struct Mat64 {
    u32 d[16];
};
static_assert(sizeof(Blk20) == 20 && sizeof(Blk24) == 24 && sizeof(Blk36) == 36, "layout");

// 0x00695ce0: args at +8 a, +0xc b (20), +0x20 c (12), +0x2c d (36); 0x3c bytes to 0x695d90.
u32 JOAN_CDECL transform_box(void* a, Blk20 b, Blk12 c, Blk36 d) {
    c_xform_box.hit();
    Blk24 m24{};
    Mat64 mat{};
    joan::orig::obb_to_box_frame(&m24, &mat, &d);           // 0x006a2b80
    import_call<void*>(d::iat_v3d_87_point_transform_2, &b, &mat);               // point_transform (b.xyz)
    import_call<void*>(d::iat_v3d_87_vector_transform_2, &c, &mat);               // vector_transform
    // result unused: 0x00695ce0 has no caller (code or data)
    return joan::orig::arg::as<u32>(joan::orig::sweep_capsule_box(joan::orig::arg::mptr(a), joan::orig::arg::f32(b.d[0]), joan::orig::arg::f32(b.d[1]), joan::orig::arg::f32(b.d[2]), joan::orig::arg::f32(b.d[3]), joan::orig::arg::f32(b.d[4]),
                                                                  joan::orig::arg::f32(c.d[0]), joan::orig::arg::f32(c.d[1]), joan::orig::arg::f32(c.d[2]),
                                                                  joan::orig::arg::f32(m24.d[0]), joan::orig::arg::f32(m24.d[1]), joan::orig::arg::f32(m24.d[2]), joan::orig::arg::f32(m24.d[3]), joan::orig::arg::f32(m24.d[4]), joan::orig::arg::f32(m24.d[5])));  // 0x00695d90
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_13", name, addr, fn)
// B("Actor::set_model_4f1760", 0x004f1760, &H::set_model);  // unregistered, see header
B("transform_box_695ce0", 0x00695ce0, &transform_box);
#undef B

}  // namespace game_b107_13
}  // namespace

// ---------------------------------------- from modules/game_b106_13.cpp (game/b106_13)
namespace {
namespace game_b106_13 {

using joan::call::cdecl_call;

using joan::call::import_call;

struct Box6 {
    float v[6];
};

struct V3 {
    float x, y, z;
};
struct V4 {
    float x, y, z, w;
};
struct Cap5 {
    float v[5];
};
struct Blk12 {
    float v[12];
};
static_assert(sizeof(V3) == 12 && sizeof(V4) == 16 && sizeof(Box6) == 24 && sizeof(Cap5) == 20 &&
              sizeof(Blk12) == 48 && sizeof(Mat) == 64);

JOAN_COUNTER(c_sweep, "sweep_capsule_box");

// x87 compare helpers (NaN goes the way the original's flag test sends it).
bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }  // test ah,0x40 set

// ======================= sweep_capsule_box 0x00695d90 =======================
struct Frame695 {  // [ebp-0x30] .. [ebp-0x4], kept contiguous like the original
    float t;
    float dir2[3];
    float circle[4];
    float pos2[4];
};
static_assert(sizeof(Frame695) == 0x30);

bool z_overlap(float lo, float hi, float zlo, float zhi) {
    if (!(zlo > lo) && !(lo > zhi)) return true;
    if (!(zlo > hi) && !(hi > zhi)) return true;
    if (!(lo > zlo) && !(zlo > hi)) return true;
    if (!(lo > zhi) && !(zhi > hi)) return true;
    return false;
}

std::uint8_t JOAN_CDECL sweep_capsule_box(float* out, float px, float py, float pz, float r, float h, float dx, float dy,
                                       float dz, float x0, float y0, float zlo, float x1, float y1, float zhi) {
    c_sweep.hit();
    if (eq_or_nan(r, 0.0f)) return 0;
    if (eq_or_nan(h, 0.0f)) return 0;
    Frame695 f;
    // [ebp-0x30] is uninitialised in the original but dead: every read follows a write by this
    // function (dz branches, side==1) or by a callee that stores *t on each al=1 path
    // (0x0069e640: 0x0069e67f / 0x0069e6df, 0x006a09e0: 0x006a0aa4 / 0x006a0b5e; also
    // 0x006a0d80: 0x006a0e1f / 0x006a0edc), and reads only happen after such a success.
    f.t = 0.0f;
    f.pos2[0] = px;
    f.pos2[1] = py;
    f.pos2[2] = 0.0f;
    f.pos2[3] = r;
    f.circle[0] = (x0 + x1) * 0.5f;
    f.circle[1] = (y0 + y1) * 0.5f;
    f.circle[2] = 0.0f;
    f.circle[3] = joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0))));  // 0x004256d0 sqrt
    f.dir2[0] = dx;
    f.dir2[1] = dy;
    f.dir2[2] = 0.0f;
    if (eq_or_nan(dx, 0.0f) && eq_or_nan(dy, 0.0f) && !eq_or_nan(dz, 0.0f)) {
        V4 a, b;
        std::memcpy(&a, f.pos2, sizeof a);
        std::memcpy(&b, f.circle, sizeof b);
        if ((joan::orig::arg::as<std::uint32_t>(joan::orig::sphere_sphere(a, b)) & 0xff) == 0) return 0;  // 0x0069d540
    } else {
        if ((joan::orig::arg::as<std::uint32_t>(joan::orig::sphere_sweep(&f.t, f.pos2, f.dir2, f.circle)) & 0xff) == 0) return 0;  // 0x006a0d80
    }

    std::uint8_t& guard = global<std::uint8_t>(d::g_a6a3f9);
    if ((guard & 1) == 0) {
        guard = static_cast<std::uint8_t>(guard | 1);
        static const std::uint32_t kInit[6][4] = {
            {0, 0, 0x3f800000u, 0}, {0, 0, 0xbf800000u, 0}, {0x3f800000u, 0, 0, 0},
            {0, 0x3f800000u, 0, 0}, {0xbf800000u, 0, 0, 0}, {0, 0xbf800000u, 0, 0},
        };
        for (int k = 0; k < 6; ++k) {
            V4 tmp;
            const void* v = joan::orig::arg::as<const void*>(joan::orig::as_plane_equation(&tmp)->construct(joan::orig::arg::u32(kInit[k][0]), joan::orig::arg::u32(kInit[k][1]), joan::orig::arg::u32(kInit[k][2]), joan::orig::arg::u32(kInit[k][3])));  // 0x00699480
            std::memcpy(obj(d::g_a6a400 + k * 16), v, 16);
        }
        joan::crt::atexit(fn::code_ptr(fn::dtor_6970a0));  // atexit (the pointer overload: the CRT table owns the width)
    }
    global<float>(d::g_a6a40c) = -zhi;
    global<float>(d::g_a6a41c) = zlo;
    global<float>(d::g_a6a42c) = -(x1 + r);
    global<float>(d::g_a6a43c) = -(y1 + r);
    global<float>(d::g_a6a44c) = x0 - r;
    global<float>(d::g_a6a45c) = y0 - r;
    const float face[6] = {zhi, zlo, x1, y1, x0, y0};                            // [ebp-0x48]
    const float face_r[6] = {zhi, zlo, x1 + r, y1 + r, x0 - r, y0 - r};          // [ebp-0x60]
    std::uint8_t edge[4] = {0, 0, 0, 0};                                         // [ebp-0x64]

    if (!(dz >= 0.0f)) {
        const float t = (pz - h * 0.5f) - zhi;
        if (!(0.0f > t) && -dz >= t) {
            f.t = -t / dz;
            const float ex = dx * f.t + px;
            const float ey = dy * f.t + py;
            if (!((x0 - r) > ex) && (x1 + r) >= ex && !((y0 - r) > ey) && (y1 + r) >= ey) {
                *out = f.t;
                return 1;
            }
        }
    } else if (dz > 0.0f) {
        const float t = (zlo - pz) - h * 0.5f;
        if (!(0.0f > t) && !(t >= dz)) {  // test ah,1 je: continue on t < dz OR NaN
            f.t = t / dz;
            const float ex = dx * f.t + px;
            const float ey = dy * f.t + py;
            if (!((x0 - r) > ex) && (x1 + r) >= ex && !((y0 - r) > ey) && (y1 + r) >= ey) {
                *out = f.t;
                return 1;
            }
        }
    }

    for (int i = 2; i < 6; ++i) {
        int side;
        switch (i) {
            case 2: side = !(px >= face[i]) ? 0 : (px >= face_r[i] ? 2 : 1); break;
            case 3: side = !(py >= face[i]) ? 0 : (py >= face_r[i] ? 2 : 1); break;
            case 4: side = (px > face[i]) ? 0 : ((px > face_r[i]) ? 1 : 2); break;
            default: side = (py > face[i]) ? 0 : ((py > face_r[i]) ? 1 : 2); break;
        }
        if (side == 0) continue;
        if (side == 1) {
            f.t = 0.0f;
        } else {
            V4 pl;
            std::memcpy(&pl, obj(d::g_a6a400 + i * 16), sizeof pl);
            if ((joan::orig::arg::as<std::uint32_t>(joan::orig::ray_plane(&f.t, V3{px, py, pz}, V3{dx, dy, dz}, pl)) & 0xff) == 0)  // 0x0069e640
                continue;
        }
        const float ex = dx * f.t + px;
        const float ey = dy * f.t + py;
        const float ez = dz * f.t + pz;
        const float lo = ez - h * 0.5f;
        const float hi = h * 0.5f + ez;
        if (!z_overlap(lo, hi, zlo, zhi)) continue;
        if (i == 2 || i == 4) {  // y range of the face
            if (!(y0 > ey) && !(ey > y1)) {
                *out = f.t;
                return 1;
            }
            if (!((y0 - r) > ey) && (y1 + r) >= ey) {
                if (i == 2) {
                    edge[1] = 1;
                    edge[0] = edge[1];
                } else {
                    edge[3] = 1;
                    edge[2] = edge[3];
                }
            }
        } else {  // x range of the face
            if (!(x0 > ex) && !(ex > x1)) {
                *out = f.t;
                return 1;
            }
            if (!((x0 - r) > ex) && (x1 + r) >= ex) {
                if (i == 3) {
                    edge[3] = 1;
                    edge[0] = edge[3];
                } else {
                    edge[2] = 1;
                    edge[1] = edge[2];
                }
            }
        }
    }

    const float ex_[4] = {x1, x1, x0, x0};
    const float ey_[4] = {y1, y0, y0, y1};
    for (int k = 0; k < 4; ++k) {
        if (edge[k] == 0) continue;
        if ((joan::orig::arg::as<std::uint32_t>(joan::orig::sphere_sweep_cylinder(&f.t, px, py, pz, dx, dy, dz, ex_[k], ey_[k], 0.0f, r, 2)) &
             0xff) == 0)  // 0x006a09e0
            continue;
        const float ez = dz * f.t + pz;
        const float lo = ez - h * 0.5f;
        const float hi = h * 0.5f + ez;
        if (z_overlap(lo, hi, zlo, zhi)) {
            *out = f.t;
            return 1;
        }
    }
    return 0;
}

// ======================= shapes_overlap 0x006a3d10 =======================
void xform(void* out, const void* in, const void* m) {
    import_call<void*>(d::iat_v3d_87_point_transform_3, out, in, m);  // point_transform(out, in, m)
}
void xform_in_place(void* v, const void* m) { import_call<void*>(d::iat_v3d_87_point_transform_2, v, m); }
void inverse(Mat* out, const void* m) { import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(out), m); }
Box6 box_of(const void* s) {
    Box6 b;
    std::memcpy(&b, jo::as_unit_type(s)->shape_box, sizeof b);
    return b;
}
V4 sphere_of(const void* s, const void* m) {
    V4 v;
    xform(&v, jo::as_unit_type(s)->shape_sphere, m);
    v.w = jo::as_unit_type(s)->shape_sphere[3];
    return v;
}
Cap5 capsule_of(const void* s, const void* m) {
    Cap5 c;
    xform(&c, jo::as_unit_type(s)->shape_cylinder, m);
    c.v[3] = jo::as_unit_type(s)->shape_cylinder[3];
    c.v[4] = jo::as_unit_type(s)->shape_cylinder[4];
    return c;
}

std::uint32_t JOAN_CDECL shapes_overlap(std::int32_t ida, const Mat* ma, std::int32_t idb, const Mat* mb) {
    c_overlap.hit();
    const void* a = joan::orig::arg::as<const void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(ida)));  // 0x0042d0a0
    const void* b = joan::orig::arg::as<const void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(idb)));
    const std::uint32_t ta = jo::as_unit_type(a)->shape_kind;
    const std::uint32_t tb = jo::as_unit_type(b)->shape_kind;
    if (ta == 1 && tb == 1) {
        const V4 sa = sphere_of(a, ma);
        const V4 sb = sphere_of(b, mb);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::sphere_sphere(sa, sb));  // 0x0069d540
    }
    if (ta == 1 && tb == 2) {
        V4 s = sphere_of(a, ma);
        Mat inv;
        inverse(&inv, mb);
        xform_in_place(&s, &inv);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::sphere_box(s, box_of(b)));  // 0x0069d5d0
    }
    if (ta == 1 && tb == 3) {
        const V4 s = sphere_of(a, ma);
        const Cap5 c = capsule_of(b, mb);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::capsule_sphere(c, s));  // 0x0069d210
    }
    if (ta == 2 && tb == 1) {
        V4 s = sphere_of(b, mb);
        Mat inv;
        inverse(&inv, ma);
        xform_in_place(&s, &inv);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::sphere_box(s, box_of(a)));  // 0x0069d5d0
    }
    if (ta == 2 && tb == 2) {
        Mat inv, rel;
        inverse(&inv, mb);
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, static_cast<void*>(&rel), ma, static_cast<const void*>(&inv));  // matrix_multiply
        Blk12 obb;
        joan::orig::obb_from_box(joan::orig::arg::mptr(static_cast<void*>(&obb)), joan::orig::arg::mptr(jo::as_unit_type(a)->shape_box), joan::orig::arg::mptr(static_cast<const void*>(&rel)));  // 0x006a30c0
        return joan::orig::arg::as<std::uint32_t>(joan::orig::obb_box(obb, box_of(b)));  // 0x0069e4e0
    }
    if (ta == 2 && tb == 3) {
        Cap5 c = capsule_of(b, mb);
        Mat inv;
        inverse(&inv, ma);
        xform_in_place(&c, &inv);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::capsule_box(c, box_of(a)));  // 0x0069d390
    }
    if (ta == 3 && tb == 1) {
        const V4 s = sphere_of(b, mb);
        Cap5 c;
        std::memcpy(&c, jo::as_unit_type(a)->shape_cylinder, sizeof c);
        c = capsule_of(a, ma);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::capsule_sphere(c, s));  // 0x0069d210
    }
    if (ta == 3 && tb == 2) {
        Cap5 c;
        std::memcpy(&c, jo::as_unit_type(a)->shape_cylinder, sizeof c);
        c = capsule_of(a, ma);
        Mat inv;
        inverse(&inv, mb);
        xform_in_place(&c, &inv);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::capsule_box(c, box_of(b)));  // 0x0069d390
    }
    if (ta == 3 && tb == 3) {
        const Cap5 ca = capsule_of(a, ma);
        const Cap5 cb = capsule_of(b, mb);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::cylinders_overlap(ca.v[0], ca.v[1], ca.v[2], ca.v[3], ca.v[4], cb.v[0], cb.v[1], cb.v[2], cb.v[3], cb.v[4]));  // 0x0069d160
    }
    return 0;  // xor al,al (type 0 or unknown)
}

#define M(name, addr, fn) JOAN_REPLACE("game/b106_13", name, addr, fn)
M("sweep_capsule_box", 0x00695d90, &sweep_capsule_box);
M("shapes_overlap", 0x006a3d10, &shapes_overlap);
#undef M

}  // namespace game_b106_13
}  // namespace

// ---------------------------------------- from modules/game_b107_07.cpp (game/b107_07)
namespace {
namespace game_b107_07 {

using joan::call::cdecl_call;

using joan::call::import_call;

JOAN_COUNTER(c_pick_ray, "pick_ray_6970c0");

// ---- 0x006970c0 ----
// By-value arguments: point (12) + w (4) + direction (12) + basis source pointer (4). Declared
// as one 32-byte block so the pointers handed to 0x00697140 keep the original contiguity
// (it reads point+0xc).
struct RayArgs {
    float p[3];
    float w;
    float d[3];
    u32 src;
};
static_assert(sizeof(RayArgs) == 32, "by-value argument block");

u32 JOAN_CDECL pick_ray_6970c0(void* out, RayArgs a) {
    c_pick_ray.hit();
    u8 basis[0x18];
    u8 m[0x40];
    std::memset(basis, 0, sizeof basis);  // fully written by 0x006a2b80
    std::memset(m, 0, sizeof m);
    joan::orig::obb_to_box_frame(joan::orig::arg::mptr(basis), joan::orig::arg::mptr(m), joan::orig::arg::mptr(a.src));  // 0x006a2b80
    import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(a.p), static_cast<void*>(m));  // point_transform
    import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(a.d), static_cast<void*>(m));  // vector_transform
    // result unused: 0x006970c0 has no caller (code or data)
    return joan::orig::arg::as<u32>(joan::orig::sweep_sphere_box_697140(joan::orig::arg::mptr(out), static_cast<void*>(a.p), static_cast<void*>(a.d),
                           static_cast<void*>(basis)));  // 0x00697140
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_07", name, addr, fn)
B("pick_ray_6970c0", 0x006970c0, &pick_ray_6970c0);
#undef B

}  // namespace game_b107_07
}  // namespace

// ---------------------------------------- from modules/game_collide1.cpp (game/collide1)
namespace {
namespace game_collide1 {

using joan::call::cdecl_call;

using joan::call::import_call;

struct V3 {
    float x, y, z;
};
struct B20 {  // cylinder: centre x, y, base z, radius, height
    float cx, cy, cz, r, h;
};
struct B24 {  // box: min x, y, z, max x, y, z
    float mn[3], mx[3];
};

float vlength(V3* v) { return static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<void*>(v))); }
float vlength_sq(V3* v) { return static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<void*>(v))); }
float vdot(V3* a, V3* b) { return static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(a), static_cast<void*>(b))); }
double crt_sqrt(float x) { return joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); }  // 0x004256d0
float crt_sqrt_f(float x) { return joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); }  // stored straight to a float
double crt_abs(float x) { return joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(x))); }   // 0x00427d60

JOAN_COUNTER(c_cylinder, "Shape::cylinder_hit");
JOAN_COUNTER(c_box, "Shape::box_hit");
JOAN_COUNTER(c_sphere, "Shape::sphere_hit");

// 0x00699ec0 Shape::cylinder_hit(out, p, d, cylinder): segment p + t d (t in [0, 1]) against a
// vertical cylinder; side first, then the cap facing p.
std::uint8_t JOAN_CDECL cylinder_hit(float* out, V3 p, V3 d, B20 b) {
    c_cylinder.hit();
    const Fpu fpu{true};
    if (!(b.r < 0.0f || b.r > 0.0f)) return 0;  // r == 0 or NaN
    if (!(b.h < 0.0f || b.h > 0.0f)) return 0;
    V3 to{b.cx - p.x, b.cy - p.y, 0.0f};  // ebp-0xc
    V3 dir{d.x, d.y, 0.0f};               // ebp-0x18
    float t;                              // ebp-0x1c
    const float dist = vlength(&to);
    if (b.r >= dist) {  // r < dist or unordered computes the side hit
        t = 0.0f;
    } else {
        const float len = vlength(&dir);
        if (b.r - dist > len) return 0;
        const float proj = vdot(&dir, &to) / len;
        if (!(proj > 0.0f)) return 0;
        const float lsq = vlength_sq(&to);
        const float d2 = lsq - proj * proj;
        const float rr = b.r * b.r;
        const float e = rr - d2;
        if (!(e >= 0.0f)) return 0;
        const float q = static_cast<float>(fpu.sub(proj, crt_sqrt(e)));
        if (!(len >= q)) return 0;
        t = q / len;
    }
    const float z = d.z * t + p.z;
    const double az = crt_abs(z - b.cz);
    const float half_h = b.h * 0.5f;
    if (half_h >= az) {
        *out = t;
        return 1;
    }
    const float top = b.r * 0.5f + b.cz;
    float cap;
    if (!(top > p.z)) {
        cap = top;
    } else {
        const float bottom = b.cz - b.r * 0.5f;
        if (!(bottom >= p.z)) return 0;
        cap = bottom;
    }
    Plane4 plane;
    joan::orig::as_plane_equation(&plane)->construct(joan::orig::arg::bits(0.0f), joan::orig::arg::bits(0.0f), joan::orig::arg::bits(1.0f), joan::orig::arg::bits(-cap));  // 0x00699480
    if (joan::orig::arg::as<std::uint8_t>(joan::orig::ray_plane(&t, p, d, plane)) == 0) return 0;  // 0x0069e640
    const float ex = d.x * t + p.x - b.cx;
    const float ey = d.y * t + p.y - b.cy;
    const float sum = ex * ex + ey * ey;
    const float rr = b.r * b.r;
    if (!(rr >= sum)) return 0;
    *out = t;
    return 1;
}

// 0x00698970 Shape::box_hit(out, p, d, box): slab test of p + t d, t in [0, 1].
std::uint8_t JOAN_CDECL box_hit(float* out, V3 p, V3 d, B24 b) {
    c_box.hit();
    float tmin = 0.0f, tmax = 1.0f;
    const float pv[3] = {p.x, p.y, p.z};
    const float dv[3] = {d.x, d.y, d.z};
    for (int k = 0; k < 3; ++k) {
        if (!(dv[k] < 0.0f || dv[k] > 0.0f)) {  // d == 0 or NaN: inside the slab?
            if (b.mn[k] > pv[k] || pv[k] > b.mx[k]) return 0;
            continue;
        }
        const float t1 = (b.mn[k] - pv[k]) / dv[k];
        float hi = (b.mx[k] - pv[k]) / dv[k];
        float lo = hi;
        if (!(t1 >= lo)) {
            lo = t1;
        } else if (t1 > hi) {
            hi = t1;
        }
        if (!(tmin >= lo)) tmin = lo;
        if (!(hi >= tmax)) tmax = hi;
        if (!(tmax >= tmin)) return 0;
    }
    *out = tmin;
    return 1;
}

// 0x006a0f90 Shape::sphere_hit(out, p, d, sphere): 0 in *out when p is inside the sphere.
std::uint8_t JOAN_CDECL sphere_hit(float* out, V3 p, V3 d, const float* s) {
    c_sphere.hit();
    const Fpu fpu{true};
    if (!(s[3] < 0.0f || s[3] > 0.0f)) return 0;
    V3 to{s[0] - p.x, s[1] - p.y, s[2] - p.z};  // ebp-0xc
    const float lsq = vlength_sq(&to);
    const float dist = crt_sqrt_f(lsq);
    if (s[3] >= dist) {  // r < dist or unordered goes on
        *out = 0.0f;
        return 1;
    }
    const float len = vlength(&d);
    if (s[3] - dist > len) return 0;
    const float proj = vdot(&d, &to) / len;
    if (!(proj > 0.0f)) return 0;
    const float d2 = lsq - proj * proj;
    const float rr = s[3] * s[3];
    const float e = rr - d2;
    if (!(e >= 0.0f)) return 0;
    const float q = static_cast<float>(fpu.sub(proj, crt_sqrt(e)));
    if (!(len >= q)) return 0;
    *out = q / len;
    return 1;
}

#define CL1(name, addr, fn) JOAN_REPLACE("game/collide1", name, addr, fn)
CL1("Shape::cylinder_hit", 0x00699ec0, &cylinder_hit);
CL1("Shape::box_hit", 0x00698970, &box_hit);
CL1("Shape::sphere_hit", 0x006a0f90, &sphere_hit);
#undef CL1

}  // namespace game_collide1
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using joan::call::cdecl_call;

using joan::call::import_call;

// x87 fcomp + test ah,0x40: equal or unordered.
bool eq_or_nan(double a, double b) { return !(a < b) && !(a > b); }

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");
static_assert(sizeof(Box18) == 0x18, "Box18");
static_assert(sizeof(Shape30) == 0x30, "Shape30");

JOAN_COUNTER(c_box_sweep, "Collide::box_sweep_698cb0");

// 0x00698cb0 swept box test: 0x30-byte shape s (two vectors, centre +0x24), direction d,
// axis box b (min / max). Writes the hit time to *out and returns 1 on a hit.
u32 JOAN_CDECL box_sweep_698cb0(float* out, Shape30 s, Vec3 d, Box18 b) {
    c_box_sweep.hit();
    const Fpu fpu{true};
    float t = 0.0f;
    float sphere[4] = {s.v[9], s.v[10], 0.0f, 0.0f};
    {
        const float l1 = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(&s.v[0])));
        const double l2 = import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(&s.v[3]));
        const float sum = static_cast<float>(fpu.add(l2, l1));
        const double r = joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(sum)));  // 0x004256d0 sqrt
        sphere[3] = static_cast<float>(fpu.mul(r, 0.5f));
    }
    float centre[4] = {(b.v[0] + b.v[3]) * 0.5f, (b.v[1] + b.v[4]) * 0.5f, 0.0f, 0.0f};
    {
        const float dx = b.v[3] - b.v[0], dy = b.v[4] - b.v[1];
        const float sum = dx * dx + dy * dy;
        const double r = joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(sum)));
        centre[3] = static_cast<float>(fpu.mul(r, 0.5f));
    }
    float dir2[3] = {d.x, d.y, 0.0f};
    if ((joan::orig::arg::as<u32>(joan::orig::sphere_sweep(&t, sphere, dir2, centre)) & 0xffu) == 0) return 0;  // 0x006a0d80
    const char flag = eq_or_nan(t, 0.0) ? 1 : 0;
    if ((joan::orig::arg::as<u32>(joan::orig::box_sweep_planes_694fc0(&t, s, d, b, static_cast<u8>(flag))) & 0xffu) != 0) {  // 0x00694fc0
        std::memcpy(out, &t, 4);
        return 1;
    }
    Box18 local_box{};
    alignas(4) float m[16] = {};
    joan::orig::obb_to_box_frame(&local_box, joan::orig::arg::mptr(m), &s);  // 0x006a2b80
    Shape30 ext{};
    ext.v[0] = b.v[3] - b.v[0];
    ext.v[4] = b.v[4] - b.v[1];
    ext.v[8] = b.v[5] - b.v[2];
    ext.v[9] = (b.v[3] + b.v[0]) * 0.5f;
    ext.v[10] = (b.v[4] + b.v[1]) * 0.5f;
    ext.v[11] = (b.v[5] + b.v[2]) * 0.5f;
    joan::orig::obb_transform(&ext, joan::orig::arg::mptr(m));  // 0x006a3040
    Vec3 nd{-d.x, -d.y, -d.z};
    import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(&nd), static_cast<void*>(m));  // vector_transform
    if ((joan::orig::arg::as<u32>(joan::orig::box_sweep_planes_694fc0(&t, ext, nd, local_box, static_cast<u8>(flag))) & 0xffu) != 0) {
        std::memcpy(out, &t, 4);
        return 1;
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Collide::box_sweep_698cb0", 0x00698cb0, &box_sweep_698cb0);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b111_11.cpp (game/b111_11)
namespace {
namespace game_b111_11 {

using joan::call::cdecl_call;

using joan::call::import_call;


JOAN_COUNTER(c_box, "box_hit_699090");

// ============================================================================ 0x00699090
struct F3 {
    float v[3];
};
struct F6 {
    float v[6];
};
struct F12 {
    float v[12];
};
// 0x004256d0 (thunk 0x00406d11): CRT sqrt of a float, raw st0.
double sqrtf_st0(float x) { return joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); }

u32 JOAN_CDECL box_hit(float* out, F6 box, F3 pt, F12 shape) {
    c_box.hit();
    const Fpu fpu{true};
    const float half = global<float>(d::flt_0_5);  // 0.5f
    float t = 0.0f;                                // [ebp-0x98], written by 0x006a0d80 before use
    float centre[4];                               // [ebp-0xa8]
    centre[0] = (box.v[0] + box.v[3]) * half;
    centre[1] = (box.v[1] + box.v[4]) * half;
    jo::view<u32>(centre[2]) = 0;
    {
        const float dx = box.v[3] - box.v[0], dy = box.v[4] - box.v[1];
        centre[3] = static_cast<float>(fpu.mul(sqrtf_st0(dx * dx + dy * dy), half));
    }
    float dir[4];  // [ebp-0xb8]
    dir[0] = shape.v[9];
    dir[1] = shape.v[10];
    jo::view<u32>(dir[2]) = 0;
    {
        const float l0 = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, &shape.v[0]));  // length_sq
        const float l1 = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, &shape.v[3]));
        dir[3] = static_cast<float>(fpu.mul(sqrtf_st0(l1 + l0), half));
    }
    float p2[3];  // [ebp-0xc4]
    p2[0] = pt.v[0];
    p2[1] = pt.v[1];
    jo::view<u32>(p2[2]) = 0;
    if ((joan::orig::arg::as<u32>(joan::orig::sphere_sweep(&t, centre, p2, dir)) & 0xffu) == 0) return 0;  // 0x006a0d80
    const u32 flag = !(t < 0.0f || t > 0.0f) ? 1u : 0u;                           // t == 0 or NaN
    F3 n;
    {
        F3 tmp = {};
        std::memcpy(&n, joan::orig::arg::as<F3*>(joan::orig::vec_negate(&tmp, &pt)), 12);  // 0x0060cbc0 negate
    }
    if ((joan::orig::arg::as<u32>(joan::orig::box_sweep_planes_694fc0(&t, shape, n, box, static_cast<u8>(flag))) & 0xffu) != 0) {  // 0x00694fc0
        *out = t;
        return 1;
    }
    F6 frame = {};     // [ebp-0x58] (written by 0x006a2b80)
    float m[16] = {};  // [ebp-0x40] (written by 0x006a2b80)
    joan::orig::obb_to_box_frame(&frame, joan::orig::arg::mptr(m), &shape);  // 0x006a2b80
    F12 b;  // [ebp-0x88]
    b.v[0] = box.v[3] - box.v[0];
    jo::view<u32>(b.v[1]) = 0;
    jo::view<u32>(b.v[2]) = 0;
    jo::view<u32>(b.v[3]) = 0;
    b.v[4] = box.v[4] - box.v[1];
    jo::view<u32>(b.v[5]) = 0;
    jo::view<u32>(b.v[6]) = 0;
    jo::view<u32>(b.v[7]) = 0;
    b.v[8] = box.v[5] - box.v[2];
    b.v[9] = (box.v[3] + box.v[0]) * half;
    b.v[10] = (box.v[4] + box.v[1]) * half;
    b.v[11] = (box.v[5] + box.v[2]) * half;
    joan::orig::obb_transform(&b, joan::orig::arg::mptr(m));  // 0x006a3040
    F3 w = {};  // [ebp-0x94] (written by vector_transform)
    import_call<void*>(d::iat_v3d_87_vector_transform_3, &w, &pt, m);  // vector_transform(out, in, m)
    if ((joan::orig::arg::as<u32>(joan::orig::box_sweep_planes_694fc0(&t, b, w, frame, static_cast<u8>(flag))) & 0xffu) != 0) {
        *out = t;
        return (joan::call::register_low_bits(out) & 0xffffff00u) | 1u;  // eax = out, al = 1; no caller of 0x00699090
    }
    return 0;
}

#define G(name, addr, fn) JOAN_REPLACE("game/b111_11", name, addr, fn)
G("box_hit_699090", 0x00699090, &box_hit);
#undef G

}  // namespace game_b111_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers37.cpp (en3d/helpers37)
namespace {
namespace en3d_helpers37 {

using joan::call::cdecl_call;

using joan::call::import_call;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12 && sizeof(Plane4) == 16, "by-value sizes");

JOAN_COUNTER(c_ray_plane, "ray_plane");

struct H {
    // 0x00699480 Plane4::Plane4(a, b, c, d): raw dword copies.
    void* plane_ctor(std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d) {
        jo::view<std::uint32_t>(jo::self<jo::Plane4>(this)->a) = a;
        jo::view<std::uint32_t>(jo::self<jo::Plane4>(this)->b) = b;
        jo::view<std::uint32_t>(jo::self<jo::Plane4>(this)->c) = c;
        jo::view<std::uint32_t>(jo::self<jo::Plane4>(this)->d) = d;
        return this;
    }
};

// 0x0069e640 ray_plane(out, origin, dir, plane) (cdecl, 0x2c bytes of args; callers test al).
bool JOAN_CDECL ray_plane(float* out, V3 origin, V3 dir, Plane4 plane) {
    c_ray_plane.hit();
    V3 normal;
    joan::orig::plane_normal(joan::orig::arg::mptr(static_cast<void*>(&normal)), joan::orig::arg::mptr(static_cast<void*>(&plane)));  // 0x0060d9e0
    const float dist = joan::orig::arg::as<float>(joan::orig::plane_distance(joan::orig::arg::mptr(static_cast<void*>(&plane)), joan::orig::arg::mptr(static_cast<void*>(&origin))));  // 0x0060da30
    const float zero = 0.0f;  // 0x009cf5c4
    if (dist == zero || dist != dist) {  // test ah,0x40: equal or unordered
        *out = 0.0f;
        return true;
    }
    if (dist < zero) return false;  // ordered here
    const float dot = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(&normal), static_cast<void*>(&dir)));
    if (dot >= zero) return false;   // continues when dot < 0 or unordered
    if (!(-dot >= dist)) return false;  // -dot < dist or unordered
    *out = -dist / dot;
    return true;
}

#define HELPER37(name, addr, fn) JOAN_REPLACE("en3d/helpers37", name, addr, fn)
HELPER37("Plane4::Plane4", 0x00699480, &H::plane_ctor);
HELPER37("ray_plane", 0x0069e640, &ray_plane);
#undef HELPER37

}  // namespace en3d_helpers37
}  // namespace

// ---------------------------------------- from modules/game_b107_18.cpp (game/b107_18)
namespace {
namespace game_b107_18 {

using joan::call::cdecl_call;

// 12-byte vector (raw bits, no float conversions when copied / passed by value).
struct V3 {
    u32 a, b, c;
};
static_assert(sizeof(V3) == 12, "V3");
struct S20 {
    u32 d[5];
};
static_assert(sizeof(S20) == 20, "S20");
struct S24 {
    u32 d[6];
};
static_assert(sizeof(S24) == 24, "S24");

// ---- 0x006998b0 ----
u32 JOAN_CDECL forward_6998b0(void* p, S24 a, V3 b, S20 c) {
    V3 tmp{0, 0, 0};
    const V3* neg = joan::orig::arg::as<const V3*>(joan::orig::vec_negate(&tmp, &b));  // 0x0060cbc0 (negate)
    const V3 n = *neg;
    // stack order of the original call: p, the 20-byte struct, the negated vec3, the 24-byte struct
    // result unused: 0x006998b0 has no caller (code or data)
    return joan::orig::arg::as<u32>(joan::orig::sweep_capsule_box(joan::orig::arg::mptr(p), joan::orig::arg::f32(c.d[0]), joan::orig::arg::f32(c.d[1]), joan::orig::arg::f32(c.d[2]), joan::orig::arg::f32(c.d[3]), joan::orig::arg::f32(c.d[4]),
                                                                  joan::orig::arg::f32(n.a), joan::orig::arg::f32(n.b), joan::orig::arg::f32(n.c),
                                                                  joan::orig::arg::f32(a.d[0]), joan::orig::arg::f32(a.d[1]), joan::orig::arg::f32(a.d[2]), joan::orig::arg::f32(a.d[3]), joan::orig::arg::f32(a.d[4]), joan::orig::arg::f32(a.d[5])));  // 0x00695d90
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_18", name, addr, fn)
B("forward_6998b0", 0x006998b0, &forward_6998b0);
#undef B

}  // namespace game_b107_18
}  // namespace

// ---------------------------------------- from modules/game_b106_18.cpp (game/b106_18)
namespace {
namespace game_b106_18 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_box, "box_outside_region");

// ---------------------------------------------------------------- 0x0069a300 helpers
struct RV {
    u32 x, y, z;
};
static_assert(sizeof(RV) == 12, "RV");
struct Plane {
    u32 v[4];
};
RV v_add(const RV& a, const RV& b) {  // 0x0060cc20
    RV out;
    return *joan::orig::arg::as<RV*>(joan::orig::vec_add(&out, &a, &b));
}
RV v_sub(const RV& a, const RV& b) {  // 0x0060cc90
    RV out;
    return *joan::orig::arg::as<RV*>(joan::orig::vec_sub(&out, &a, &b));
}
RV v_half(const RV& a) {  // 0x0060cd60
    RV out;
    return *joan::orig::arg::as<RV*>(joan::orig::vec_scaled(&out, 0.5f, &a));
}
RV v_neg(const RV& a) {  // 0x0060cbc0
    RV out;
    return *joan::orig::arg::as<RV*>(joan::orig::vec_negate(&out, &a));
}
// D - 0.5 * (first +/- C)
RV corner_tail(const RV& first, const RV& c, bool add_c, const RV& d) {
    const RV t = add_c ? v_add(first, c) : v_sub(first, c);
    const RV h = v_half(t);
    return v_sub(d, h);
}
double plane_dist(Plane* p, const RV* v) { return joan::orig::arg::as<double>(joan::orig::plane_distance(joan::orig::arg::mptr(p), joan::orig::arg::mptr(v))); }  // 0x0060da30

// The four by-value vectors as the caller (Box::face_hit 0x0069ea70) passes them: float members, because on System V
// x86-64 three floats go in XMM registers while three dwords (RV) go in integer registers.
struct FV {
    float x, y, z;
};
static_assert(sizeof(FV) == 12, "FV");
RV bits_of(const FV& v) {
    RV r;
    std::memcpy(&r, &v, sizeof r);
    return r;
}

// ---------------------------------------------------------------- 0x0069a300
u8 JOAN_CDECL box_outside_region(FV fa, FV fb, FV fc, FV fd, float ax, float ay, u32 az, u32 h, u32 i, u32 j,
                              u32 k) {
    c_box.hit();
    (void)az;
    RV a = bits_of(fa), b = bits_of(fb), c = bits_of(fc), d = bits_of(fd);
    RV corner[8];
    corner[0] = corner_tail(v_add(a, b), c, true, d);
    corner[1] = corner_tail(v_add(v_neg(a), b), c, true, d);
    corner[2] = corner_tail(v_sub(v_neg(a), b), c, true, d);
    corner[3] = corner_tail(v_sub(a, b), c, true, d);
    corner[4] = corner_tail(v_add(a, b), c, false, d);
    corner[5] = corner_tail(v_add(v_neg(a), b), c, false, d);
    corner[6] = corner_tail(v_sub(v_neg(a), b), c, false, d);
    corner[7] = corner_tail(v_sub(a, b), c, false, d);

    // x87 `fcomp 0.0; test ah,0x40; jne` - taken when equal or unordered.
    const bool use_x = ax < 0.0f || ax > 0.0f;
    const bool use_y = ay < 0.0f || ay > 0.0f;
    u32 acc = 0xffffffffu;
    for (i32 n = 0; n < 8; ++n) {
        const RV& p = corner[n];
        u32 r;
        if (use_x) {
            r = joan::orig::arg::as<u32>(joan::orig::outcode(joan::orig::arg::f32(p.x), joan::orig::arg::f32(p.y), joan::orig::arg::f32(p.z), joan::orig::arg::f32(h), joan::orig::arg::f32(h), joan::orig::arg::f32(0u), joan::orig::arg::f32(j), joan::orig::arg::f32(0u), joan::orig::arg::f32(k)));  // 0x0068d690
        } else if (use_y) {
            r = joan::orig::arg::as<u32>(joan::orig::outcode(joan::orig::arg::f32(p.x), joan::orig::arg::f32(p.y), joan::orig::arg::f32(p.z), joan::orig::arg::f32(0u), joan::orig::arg::f32(i), joan::orig::arg::f32(h), joan::orig::arg::f32(h), joan::orig::arg::f32(0u), joan::orig::arg::f32(k)));
        } else {
            r = joan::orig::arg::as<u32>(joan::orig::outcode(joan::orig::arg::f32(p.x), joan::orig::arg::f32(p.y), joan::orig::arg::f32(p.z), joan::orig::arg::f32(0u), joan::orig::arg::f32(i), joan::orig::arg::f32(0u), joan::orig::arg::f32(j), joan::orig::arg::f32(h), joan::orig::arg::f32(h)));
        }
        acc &= r;
    }
    if (acc != 0) return 0;

    Plane plane;
    joan::orig::as_plane_equation(&plane)->init_425700();  // 0x00425700 (empty ctor)
    RV q[4];
    if (use_x) {
        q[0] = RV{h, 0u, 0u};
        q[1] = RV{h, j, 0u};
        q[2] = RV{h, j, k};
        q[3] = RV{h, 0u, k};
    } else if (use_y) {
        q[0] = RV{0u, h, 0u};
        q[1] = RV{i, h, 0u};
        q[2] = RV{i, h, k};
        q[3] = RV{0u, h, k};
    } else {
        q[0] = RV{0u, 0u, h};
        q[1] = RV{i, 0u, h};
        q[2] = RV{i, j, h};
        q[3] = RV{0u, j, h};
    }
    for (i32 n = 0; n < 6; ++n) {
        RV normal;
        switch (n) {
            case 0:
                normal = v_neg(a);
                joan::orig::vec_cross(&plane, &corner[0], &normal);  // 0x0060d990
                break;
            case 1:
                joan::orig::vec_cross(&plane, &corner[6], &a);
                break;
            case 2:
                normal = v_neg(b);
                joan::orig::vec_cross(&plane, &corner[0], &normal);
                break;
            case 3:
                joan::orig::vec_cross(&plane, &corner[6], &b);
                break;
            case 4:
                normal = v_neg(c);
                joan::orig::vec_cross(&plane, &corner[0], &normal);
                break;
            default:
                joan::orig::vec_cross(&plane, &corner[6], &c);
                break;
        }
        // `fcomp 0.0; test ah,0x41; jne next` - all four strictly in front -> 0.
        if (plane_dist(&plane, &q[0]) > 0.0 && plane_dist(&plane, &q[1]) > 0.0 &&
            plane_dist(&plane, &q[2]) > 0.0 && plane_dist(&plane, &q[3]) > 0.0) {
            return 0;
        }
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b106_18", name, addr, fn)
B("box_outside_region", 0x0069a300, &box_outside_region);
#undef B

}  // namespace game_b106_18
}  // namespace

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

JOAN_COUNTER(c_cyl, "Collide::cylinder_cylinder");
JOAN_COUNTER(c_sphere, "Collide::cylinder_sphere");

constexpr float kZero = 0.0f;  // 0x9cf5c4
constexpr float kHalf = 0.5f;  // 0x9cf77c

using joan::orig::helpers::plane_distance;
float sqrt_f(float x) { return joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); }    // 0x004256d0 (stored)
// 0x0069b390
std::uint8_t JOAN_CDECL cylinder_cylinder(Cyl c, Plane a, Plane b, Cyl s) {
    c_cyl.hit();
    float d = plane_distance(&a, &s);
    if (!(d + s.v[3] >= kZero)) return 0;
    d = plane_distance(&b, &s);
    if (!(d + s.v[3] >= kZero)) return 0;
    if (-c.v[3] > plane_distance(&a, &c)) return 0;
    if (-c.v[3] > plane_distance(&b, &c)) return 0;
    const float d2 = (c.v[0] - s.v[0]) * (c.v[0] - s.v[0]) + (c.v[1] - s.v[1]) * (c.v[1] - s.v[1]);
    const float rr = (c.v[3] + s.v[3]) * (c.v[3] + s.v[3]);
    if (d2 > rr) return 0;
    // (dead `if (0)` block of the original here)
    const float lo1 = c.v[2] - c.v[4] * kHalf;
    const float hi1 = c.v[4] * kHalf + c.v[2];
    const float lo2 = s.v[2] - s.v[4] * kHalf;
    const float hi2 = s.v[4] * kHalf + s.v[2];
    if (!(lo1 > lo2) && !(lo2 > hi1)) return 1;
    if (!(hi1 > hi2) && !(hi2 > hi1)) return 1;
    if (!(lo2 > lo1) && !(lo1 > hi2)) return 1;
    if (!(lo2 > hi1) && !(hi1 > hi2)) return 1;
    return 0;
}

// Cut circle part of 0x0069bd60: radius rad at the sphere.
std::uint8_t sphere_circle(const Cyl& c, const Plane& a, const Plane& b, const Plane& s, float rad) {
    const float d2 = (c.v[0] - s.v[0]) * (c.v[0] - s.v[0]) + (c.v[1] - s.v[1]) * (c.v[1] - s.v[1]);
    const float rr = (c.v[3] + rad) * (c.v[3] + rad);
    if (!(plane_distance(&a, &s) + rad >= kZero)) return 0;
    if (!(plane_distance(&b, &s) + rad >= kZero)) return 0;
    return d2 > rr ? 0 : 1;
}

// 0x0069bd60
std::uint8_t JOAN_CDECL cylinder_sphere(Cyl c, Plane a, Plane b, Plane s) {
    c_sphere.hit();
    const float lo = c.v[2] - c.v[4] * kHalf;
    const float hi = c.v[4] * kHalf + c.v[2];
    const float z = s.v[2];
    const float r = s.v[3];
    if (!(lo > z) && !(z > hi)) return sphere_circle(c, a, b, s, r);
    if (!(lo - r > z) && !(z >= lo)) {
        const float dz = lo - z;
        const float rad = sqrt_f(r * r - dz * dz);
        return sphere_circle(c, a, b, s, rad);
    }
    if (hi >= z) return 0;
    if (!(hi + r >= z)) return 0;
    const float dz = z - hi;
    const float rad = sqrt_f(r * r - dz * dz);
    return sphere_circle(c, a, b, s, rad);
}

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("Collide::cylinder_cylinder", 0x0069b390, &cylinder_cylinder);
B83_04("Collide::cylinder_sphere", 0x0069bd60, &cylinder_sphere);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {

using joan::call::cdecl_call;

// 0x006a1250 solves | a b | ... with Cramer's rule; 0 when the determinant is 0 (or NaN).
std::uint8_t JOAN_CDECL solve2x2(float* out_x, float* out_y, float a, float b, float c, float d, float e, float g) {
    const float det = a * e - b * d;
    if (!(det < 0.0f || det > 0.0f)) return 0;  // test ah,0x40: equal or unordered
    *out_x = (b * g - c * e) / det;
    *out_y = (c * d - a * g) / det;
    return 1;
}

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("solve2x2", 0x006a1250, &solve2x2);
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
static_assert(sizeof(Capsule) == 20, "capsule");
struct Box {  // min xyz, max xyz
    float x0, y0, z0, x1, y1, z1;
};
static_assert(sizeof(Box) == 24, "box");
struct Obb {  // three axes (scaled) + centre
    float d[12];
};
static_assert(sizeof(Obb) == 48, "obb");
static_assert(sizeof(Matrix) == 64, "matrix");

const float kHalf = 0.5f;  // 0x009cf77c

JOAN_COUNTER(c_sphere_box, "sphere_box");
JOAN_COUNTER(c_obb_box, "obb_box");

// 0x0069d5d0
std::uint8_t JOAN_CDECL sphere_box(Sphere s, Box b) {
    c_sphere_box.hit();
    const float x0 = b.x0 - s.r;
    const float x1 = b.x1 + s.r;
    const float y0 = b.y0 - s.r;
    const float y1 = b.y1 + s.r;
    const float z0 = b.z0 - s.r;
    const float z1 = b.z1 + s.r;
    if (x0 > s.x) return 0;
    if (s.x > x1) return 0;
    if (y0 > s.y) return 0;
    if (s.y > y1) return 0;
    if (z0 > s.z) return 0;
    if (s.z > z1) return 0;
    float cx, cy, cz;
    if (!(s.x >= b.x0))
        cx = b.x0;
    else if (!(b.x1 >= s.x))
        cx = b.x1;
    else
        return 1;
    if (!(s.y >= b.y0))
        cy = b.y0;
    else if (!(b.y1 >= s.y))
        cy = b.y1;
    else
        return 1;
    if (!(s.z >= b.z0))
        cz = b.z0;
    else if (!(b.z1 >= s.z))
        cz = b.z1;
    else
        return 1;
    const float dx = s.x - cx;
    const float dy = s.y - cy;
    const float dz = s.z - cz;
    const float sum = dx * dx + dy * dy + dz * dz;
    return (s.r * s.r >= sum) ? 1 : 0;
}

// 0x006a30c0
void* JOAN_CDECL obb_from_box(Obb* out, const Box* in, Matrix* m) {
    out->d[9] = (in->x1 + in->x0) * kHalf;
    out->d[10] = (in->y1 + in->y0) * kHalf;
    out->d[11] = (in->z1 + in->z0) * kHalf;
    out->d[0] = in->x1 - in->x0;
    jo::view<std::uint32_t>(out->d[1]) = 0;
    jo::view<std::uint32_t>(out->d[2]) = 0;
    jo::view<std::uint32_t>(out->d[3]) = 0;
    out->d[4] = in->y1 - in->y0;
    jo::view<std::uint32_t>(out->d[5]) = 0;
    jo::view<std::uint32_t>(out->d[6]) = 0;
    jo::view<std::uint32_t>(out->d[7]) = 0;
    out->d[8] = in->z1 - in->z0;
    return joan::orig::arg::as<void*>(joan::orig::obb_transform(joan::orig::arg::mptr(static_cast<void*>(out)), joan::orig::arg::mptr(static_cast<void*>(m))));  // 0x006a3040
}

std::uint8_t separated_test(Obb o, Box b) { return joan::orig::arg::as<std::uint8_t>(joan::orig::obb_box_edges(o, b)); }  // 0x0069d7d0

// 0x0069e4e0
std::uint8_t JOAN_CDECL obb_box(Obb o, Box b) {
    c_obb_box.hit();
    if (separated_test(o, b) != 0) return 1;
    Box local_box;
    Matrix m;
    joan::orig::obb_to_box_frame(joan::orig::arg::mptr(static_cast<void*>(&local_box)), joan::orig::arg::mptr(static_cast<void*>(&m)), joan::orig::arg::mptr(static_cast<void*>(&o)));  // 0x006a2b80
    Obb n;
    n.d[0] = b.x1 - b.x0;
    jo::view<std::uint32_t>(n.d[1]) = 0;
    jo::view<std::uint32_t>(n.d[2]) = 0;
    jo::view<std::uint32_t>(n.d[3]) = 0;
    n.d[4] = b.y1 - b.y0;
    jo::view<std::uint32_t>(n.d[5]) = 0;
    jo::view<std::uint32_t>(n.d[6]) = 0;
    jo::view<std::uint32_t>(n.d[7]) = 0;
    n.d[8] = b.z1 - b.z0;
    n.d[9] = (b.x1 + b.x0) * kHalf;
    n.d[10] = (b.y1 + b.y0) * kHalf;
    n.d[11] = (b.z1 + b.z0) * kHalf;
    joan::orig::obb_transform(joan::orig::arg::mptr(static_cast<void*>(&n)), joan::orig::arg::mptr(static_cast<void*>(&m)));  // 0x006a3040
    if (separated_test(n, local_box) != 0) return 1;
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("sphere_box", 0x0069d5d0, &sphere_box);
B("obb_box", 0x0069e4e0, &obb_box);
B("obb_from_box", 0x006a30c0, &obb_from_box);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b91_01.cpp (game/b91_01)
namespace {
namespace game_b91_01 {

using joan::call::import_call;

// ---------------------------------------------------------------------------------------------
// Shapes (same layouts as game_b90_02).
struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");
struct Box {  // min xyz, max xyz
    float x0, y0, z0, x1, y1, z1;
};
static_assert(sizeof(Box) == 24, "box");
struct Obb {  // three axes (scaled) + centre
    Vec3 a0, a1, a2, c;
};
static_assert(sizeof(Obb) == 48, "obb");

const float kOne = 1.0f;  // 0x009cf784

// Vector helpers (cdecl, hidden out pointer first, return out).
Vec3* v_add(Vec3* out, const Vec3* a, const Vec3* b) {
    return joan::orig::arg::as<Vec3*>(joan::orig::vec_add(joan::orig::arg::mptr(out), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));  // 0x0060cc20
}
Vec3* v_sub(Vec3* out, const Vec3* a, const Vec3* b) {
    return joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(joan::orig::arg::mptr(out), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));  // 0x0060cc90
}
Vec3* v_half(Vec3* out, const Vec3* a) {
    return joan::orig::arg::as<Vec3*>(joan::orig::vec_scaled(joan::orig::arg::mptr(out), 0.5f, joan::orig::arg::ptr(a)));  // 0x0060cd60 (push 0x3f000000)
}
Vec3* v_neg(Vec3* out, const Vec3* a) {
    return joan::orig::arg::as<Vec3*>(joan::orig::vec_negate(joan::orig::arg::mptr(out), joan::orig::arg::ptr(a)));  // 0x0060cbc0
}
Vec3* v_scale(Vec3* out, const Vec3* a, float s) {
    return joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(joan::orig::arg::mptr(out), joan::orig::arg::ptr(a), joan::orig::arg::f32(s)));  // 0x0060cd00
}

JOAN_COUNTER(c_obb_box_edges, "obb_box_edges");
JOAN_COUNTER(c_obb_to_box_frame, "obb_to_box_frame");

// 0x0069d7d0
std::uint8_t JOAN_CDECL obb_box_edges(Obb o, Box b) {
    c_obb_box_edges.hit();
    if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(&b)->contains(&o.c)) != 0) return 1;  // 0x0060d5f0
    Vec3 p[8];
    Vec3 t1, t2, t3, t4, t5, w;
    // corner = c - 0.5 * ((+-a0 +- a1) +- a2), helper order as in the original
    w = *v_add(&t1, &o.a0, &o.a1);
    w = *v_add(&t2, &w, &o.a2);
    w = *v_half(&t3, &w);
    p[0] = *v_sub(&t4, &o.c, &w);

    w = *v_add(&t1, &o.a0, &o.a1);
    w = *v_sub(&t2, &w, &o.a2);
    w = *v_half(&t3, &w);
    p[1] = *v_sub(&t4, &o.c, &w);

    w = *v_sub(&t1, &o.a0, &o.a1);
    w = *v_add(&t2, &w, &o.a2);
    w = *v_half(&t3, &w);
    p[2] = *v_sub(&t4, &o.c, &w);

    w = *v_sub(&t1, &o.a0, &o.a1);
    w = *v_sub(&t2, &w, &o.a2);
    w = *v_half(&t3, &w);
    p[3] = *v_sub(&t4, &o.c, &w);

    w = *v_neg(&t5, &o.a0);
    w = *v_add(&t1, &w, &o.a1);
    w = *v_add(&t2, &w, &o.a2);
    w = *v_half(&t3, &w);
    p[4] = *v_sub(&t4, &o.c, &w);

    w = *v_neg(&t5, &o.a0);
    w = *v_add(&t1, &w, &o.a1);
    w = *v_sub(&t2, &w, &o.a2);
    w = *v_half(&t3, &w);
    p[5] = *v_sub(&t4, &o.c, &w);

    w = *v_neg(&t5, &o.a0);
    w = *v_sub(&t1, &w, &o.a1);
    w = *v_add(&t2, &w, &o.a2);
    w = *v_half(&t3, &w);
    p[6] = *v_sub(&t4, &o.c, &w);

    w = *v_neg(&t5, &o.a0);
    w = *v_sub(&t1, &w, &o.a1);
    w = *v_sub(&t2, &w, &o.a2);
    w = *v_half(&t3, &w);
    p[7] = *v_sub(&t4, &o.c, &w);

    // The callee takes twelve floats: typed as float here so that x86-64 passes them in the XMM registers it reads
    // (as dwords they went to the integer registers and the clip saw garbage).
    using Seg = std::uint8_t(JOAN_CDECL*)(float, float, float, float, float, float, float, float, float, float, float,
                                       float);
    const Seg seg = reinterpret_cast<Seg>(fn::code_value(fn::clip_segment_3d));  // 0x0068d760
    auto edge = [&](std::int32_t e0, std::int32_t e1) -> bool {
        return seg(p[e0].x, p[e0].y, p[e0].z, p[e1].x, p[e1].y, p[e1].z, b.x0, b.x1, b.y0, b.y1, b.z0, b.z1) != 0;
    };
    for (std::int32_t i = 0; i <= 1; ++i) {
        for (std::int32_t j = 0; j <= 1; ++j) {
            for (std::int32_t k = 0; k <= 1; ++k) {
                if (i == 0 && edge(k + j * 2, k + j * 2 + 4)) return 1;
                if (j == 0 && edge(k + i * 4, k + i * 4 + 2)) return 1;
                if (k == 0 && edge(j * 2 + i * 4, j * 2 + i * 4 + 1)) return 1;
            }
        }
    }
    return 0;
}

// 0x006a2b80
void* JOAN_CDECL obb_to_box_frame(void* out, void* m, const Obb* o) {
    c_obb_to_box_frame.hit();
    Matrix* const mm = static_cast<Matrix*>(m);
    Box* const ob = static_cast<Box*>(out);
    Vec3 a0 = o->a0;
    Vec3 a1 = o->a1;
    Vec3 a2 = o->a2;
    const float l0 = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const Vec3*>(&a0)));  // length
    const float l1 = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const Vec3*>(&a1)));
    const float l2 = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const Vec3*>(&a2)));
    Vec3 t1, t2, t3, t4;
    const Vec3 r0 = *v_scale(&t1, &a0, kOne / l0);
    const Vec3 r1 = *v_scale(&t2, &a1, kOne / l1);
    const Vec3 r2 = *v_scale(&t3, &a2, kOne / l2);
    mm->m[0] = r0.x;
    mm->m[1] = r1.x;
    mm->m[2] = r2.x;
    jo::view<std::uint32_t>(mm->m[3]) = 0;
    mm->m[4] = r0.y;
    mm->m[5] = r1.y;
    mm->m[6] = r2.y;
    jo::view<std::uint32_t>(mm->m[7]) = 0;
    mm->m[8] = r0.z;
    mm->m[9] = r1.z;
    mm->m[10] = r2.z;
    jo::view<std::uint32_t>(mm->m[11]) = 0;
    Vec3 w = *v_add(&t1, &a0, &a1);
    w = *v_add(&t2, &w, &a2);
    w = *v_half(&t3, &w);
    Vec3 c = *v_sub(&t4, &o->c, &w);
    mm->m[12] = -static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<const Vec3*>(&r0),
                                             static_cast<const Vec3*>(&c)));  // dot
    mm->m[13] = -static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<const Vec3*>(&r1),
                                             static_cast<const Vec3*>(&c)));
    mm->m[14] = -static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<const Vec3*>(&r2),
                                             static_cast<const Vec3*>(&c)));
    jo::view<std::uint32_t>(mm->m[15]) = 0x3f800000u;
    jo::view<std::uint32_t>(ob->z0) = 0;
    jo::view<std::uint32_t>(ob->y0) = 0;
    jo::view<std::uint32_t>(ob->x0) = 0;
    ob->x1 = l0;
    ob->y1 = l1;
    ob->z1 = l2;
    return out;
}

// 0x006a3040
void* JOAN_CDECL obb_transform(void* o, void* m) {
    import_call<void*>(d::iat_v3d_87_vector_transform_2, o, m);  // vector_transform
    import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(static_cast<std::uint8_t*>(o) + 0xc), m);
    import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(static_cast<std::uint8_t*>(o) + 0x18), m);
    return import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(static_cast<std::uint8_t*>(o) + 0x24),
                              m);  // point_transform
}

#define B(name, addr, fn) JOAN_REPLACE("game/b91_01", name, addr, fn)
B("obb_box_edges", 0x0069d7d0, &obb_box_edges);
B("obb_to_box_frame", 0x006a2b80, &obb_to_box_frame);
B("obb_transform", 0x006a3040, &obb_transform);
#undef B

}  // namespace game_b91_01
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

JOAN_COUNTER(c_sweep_cyl, "sphere_sweep_cylinder");

// 0x006a09e0
std::uint8_t JOAN_CDECL sphere_sweep_cylinder(float* out, float px, float py, float pz, float vx, float vy, float vz,
                                           float cx, float cy, float cz, float r, std::int32_t axis) {
    c_sweep_cyl.hit();
    if (r == global<float>(d::k_9cf5c4) || r != r) return 0;  // test ah,0x40 / je
    V3f d{cx - px, cy - py, cz - pz};
    V3f v{vx, vy, vz};
    if (axis == 0) {
        d.x = 0.0f;
        v.x = 0.0f;
    } else if (axis == 1) {
        d.y = 0.0f;
        v.y = 0.0f;
    } else if (axis == 2) {
        d.z = 0.0f;
        v.z = 0.0f;
    }
    const float lsq = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const V3f*>(&d)));  // length_sq
    const float dl = joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(lsq)));                             // 0x004256d0 sqrt
    if (r >= dl) {                                                                   // test ah,1 / jne
        *out = 0.0f;
        return 1;
    }
    const float vl = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const V3f*>(&v)));  // length
    if (r - dl > vl) return 0;                                                     // test ah,0x41 / jne
    const float proj = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<const V3f*>(&v), static_cast<const V3f*>(&d))) / vl;
    if (!(proj > global<float>(d::k_9cf5c4))) return 0;  // test ah,0x41 / je
    const float e = lsq - proj * proj;
    const float rr = r * r;
    const float h = rr - e;
    if (!(h >= global<float>(d::k_9cf5c4))) return 0;  // test ah,1 / je
    const double sq = joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(h)));  // 0x004256d0
    const joan::hook::x87::Fpu fpu{true};
    const float q = static_cast<float>(fpu.sub(proj, sq));
    if (!(vl >= q)) return 0;  // test ah,1 / je
    *out = q / vl;
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("sphere_sweep_cylinder", 0x006a09e0, &sphere_sweep_cylinder);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b111_09.cpp (fix/uninit)
namespace {
namespace game_b111_09 {

using joan::call::cdecl_call;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");
struct Vec4 {
    float v[4];
};
static_assert(sizeof(Vec4) == 16, "Vec4");
struct Mat12 {
    float v[12];
};
static_assert(sizeof(Mat12) == 48, "Mat12");

JOAN_COUNTER(c_face_hit, "Box::face_hit_69ea70");

// ------------------------------------------------------------------ 0x0069ea70
struct FaceArgs {   // the 22 dwords after `out`
    float a[3];     // +0x0c
    float b[3];     // +0x18
    float c[3];     // +0x24
    float p[3];     // +0x30
    float d[3];     // +0x3c
    float n[4];     // +0x48
    float e[3];     // +0x58
};
static_assert(sizeof(FaceArgs) == 88, "FaceArgs");

bool nz(float x) { return x < 0.0f || x > 0.0f; }  // fcomp 0; test ah,0x40: equal or unordered -> false
float sg(float x, float v) { return !(v >= 0.0f) ? -x : x; }  // negated when v < 0 or unordered
float half_off(float h, float s) { return h - s * 0.5f; }     // fmul [0x9cf77c] (0.5); fsubr h

u8 bool_call_cb50(float* a, float* b, float* c, float* d, float e1, float e2) {
    return joan::orig::arg::as<u8>(joan::orig::clip_line(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), joan::orig::arg::mptr(c), joan::orig::arg::mptr(d), 0.0f, joan::orig::arg::f32(e1), 0.0f, joan::orig::arg::f32(e2)));  // 0x0068cb50
}
u8 bool_call_ce00(float* poly, float e1, float e2) {
    return joan::orig::arg::as<u8>(joan::orig::polygon_overlaps_rect(joan::orig::arg::mptr(poly), i32{4}, joan::orig::arg::bits(0.0f), joan::orig::arg::bits(e1), joan::orig::arg::bits(0.0f), joan::orig::arg::bits(e2)));  // 0x0068ce00
}

static u8 JOAN_CDECL face_hit(float* out, FaceArgs g) {
    c_face_hit.hit();
    const float* const A = g.a;
    const float* const B = g.b;
    const float* const C = g.c;
    float v0 = 0.0f;  // ebp-0x4 (every path below assigns v0..v2)
    float v1 = 0.0f;  // ebp-0x8
    float v2 = 0.0f;  // ebp-0xc
    if (nz(g.n[0])) {
        v0 = A[0] * g.n[0];
        v1 = B[0] * g.n[0];
        v2 = C[0] * g.n[0];
    } else if (nz(g.n[1])) {
        v0 = A[1] * g.n[1];
        v1 = B[1] * g.n[1];
        v2 = C[1] * g.n[1];
    } else {
        v0 = A[2] * g.n[2];
        v1 = B[2] * g.n[2];
        v2 = C[2] * g.n[2];
    }
    // fabs wrapper 0x00427d60 (thunk 0x004010f0): the result is exact (|x| of a float).
    auto fabs_ = [](float x) { return static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(x)))); };
    const float s1 = fabs_(v0);
    const float s2 = fabs_(v1) + s1;
    const float r = (fabs_(v2) + s2) * 0.5f;  // ebp-0x10
    const Vec4 q = {{g.p[0], g.p[1], g.p[1], r}};  // ebp-0x24 (the original copies +0x34 twice)
    const Vec3 dir = {g.d[0], g.d[1], g.d[2]};
    Vec4 nn;
    std::memcpy(&nn, g.n, sizeof nn);
    float t = 0.0f;  // ebp-0x14, written by 0x0069e970 whenever it returns 1
    if (joan::orig::sphere_sweep_plane(&t, q.v[0], q.v[1], q.v[2], q.v[3], dir.x, dir.y, dir.z, nn.v[0], nn.v[1], nn.v[2], nn.v[3]) == 0) return 0;  // 0x0069e970
    if (!nz(t)) {
        const float sel = nz(g.n[0]) ? g.d[0] : (nz(g.n[1]) ? g.d[1] : g.d[2]);  // ebp-0x28
        if (!(sel > 0.0f)) return 0;
        // a, b, c, p (+0x0c..+0x3b), the plane (two floats + two raw dwords) and e, as 0x0069a300 takes them
        const Vec3 ga{g.a[0], g.a[1], g.a[2]};
        const Vec3 gb{g.b[0], g.b[1], g.b[2]};
        const Vec3 gc{g.c[0], g.c[1], g.c[2]};
        const Vec3 gp{g.p[0], g.p[1], g.p[2]};
        if (joan::orig::box_outside_region(ga, gb, gc, gp, nn.v[0], nn.v[1], joan::orig::arg::bits(nn.v[2]), joan::orig::arg::bits(nn.v[3]),
                                           joan::orig::arg::bits(g.e[0]), joan::orig::arg::bits(g.e[1]), joan::orig::arg::bits(g.e[2])) == 0)  // 0x0069a300
            return 0;
        *out = t;
        return 1;
    }
    auto small = [&](float v) { return !(fabs_(v) >= global<float>(d::flt_9d5360)) ? u8{1} : u8{0}; };  // 1e-4
    const u8 f0 = small(v0);  // ebp-0x2c
    const u8 f1 = small(v1);  // ebp-0x30
    const u8 f2 = small(v2);  // ebp-0x34
    const float H[3] = {t * g.d[0] + g.p[0], t * g.d[1] + g.p[1], t * g.d[2] + g.p[2]};  // ebp-0x40 / -0x3c / -0x38
    if (f0 == 0 && f1 == 0 && f2 == 0) {
        float m[3];  // ebp-0x4c / -0x48 / -0x44
        for (int k = 0; k < 3; ++k) m[k] = half_off(H[k], sg(A[k], v0) + sg(B[k], v1) + sg(C[k], v2));
        bool hit = false;
        if (nz(g.n[0])) {
            hit = !(m[1] < 0.0f || m[1] > g.e[1] || m[2] < 0.0f || m[2] > g.e[2]);
        } else if (nz(g.n[1])) {
            hit = !(m[0] < 0.0f || m[0] > g.e[0] || m[2] < 0.0f || m[2] > g.e[2]);
        } else {
            hit = !(m[0] < 0.0f || m[0] > g.e[0] || m[1] < 0.0f || m[1] > g.e[1]);
        }
        if (!hit) return 0;
        *out = t;
        return 1;
    }
    if (((f0 ^ f1) ^ f2) != 0) {
        float p1[3], p2[3];  // ebp-0x58.. and ebp-0x64..
        for (int k = 0; k < 3; ++k) {
            if (f0 != 0) {
                p1[k] = half_off(H[k], A[k] + sg(B[k], v1) + sg(C[k], v2));
            } else if (f1 != 0) {
                p1[k] = half_off(H[k], sg(A[k], v0) + B[k] + sg(C[k], v2));
            } else {
                p1[k] = half_off(H[k], sg(A[k], v0) + sg(B[k], v1) + C[k]);
            }
        }
        for (int k = 0; k < 3; ++k) {
            if (f0 != 0) {
                p2[k] = half_off(H[k], -A[k] + sg(B[k], v1) + sg(C[k], v2));
            } else if (f1 != 0) {
                p2[k] = half_off(H[k], sg(A[k], v0) - B[k] + sg(C[k], v2));
            } else {
                p2[k] = half_off(H[k], sg(A[k], v0) + sg(B[k], v1) - C[k]);
            }
        }
        u8 hit = 0;
        if (nz(g.n[0])) {
            hit = bool_call_cb50(&p1[1], &p1[2], &p2[1], &p2[2], g.e[1], g.e[2]);
        } else if (nz(g.n[1])) {
            hit = bool_call_cb50(&p1[0], &p1[2], &p2[0], &p2[2], g.e[0], g.e[2]);
        } else {
            hit = bool_call_cb50(&p1[0], &p1[1], &p2[0], &p2[1], g.e[0], g.e[1]);
        }
        if (hit == 0) return 0;
        *out = t;
        return 1;
    }
    // Two flags set: four corners q[0..3] (ebp-0x94.., -0x88.., -0x7c.., -0x70..).
    float q4[4][3];
    for (int k = 0; k < 3; ++k) {
        if (f0 == 0) {  // f1 && f2
            const float s = sg(A[k], v0);
            q4[0][k] = half_off(H[k], s + B[k] + C[k]);
            q4[1][k] = half_off(H[k], s - B[k] + C[k]);
            q4[2][k] = half_off(H[k], s - B[k] - C[k]);
            q4[3][k] = half_off(H[k], s + B[k] - C[k]);
        } else if (f1 == 0) {  // f0 && f2
            const float s = sg(B[k], v1);
            q4[0][k] = half_off(H[k], A[k] + s + C[k]);
            q4[1][k] = half_off(H[k], -A[k] + s + C[k]);
            q4[2][k] = half_off(H[k], -A[k] + s - C[k]);
            q4[3][k] = half_off(H[k], A[k] + s - C[k]);
        } else {  // f0 && f1
            const float s = sg(C[k], v2);
            q4[0][k] = half_off(H[k], A[k] + B[k] + s);
            q4[1][k] = half_off(H[k], -A[k] + B[k] + s);
            q4[2][k] = half_off(H[k], -A[k] - B[k] + s);
            q4[3][k] = half_off(H[k], A[k] - B[k] + s);
        }
    }
    if (f0 != 0 && f1 == 0) {
        // 0x0069fe38: the original stores the 4th corner's x to [ebp-0x7c] (corner 3) and never
        // writes [ebp-0x70]. Keep the overwrite; [ebp-0x70] gets the value meant for it (fix/uninit).
        q4[2][0] = q4[3][0];
    }
    float poly[8];
    u8 hit = 0;
    if (nz(g.n[0])) {
        for (int j = 0; j < 4; ++j) {
            poly[2 * j] = q4[j][1];
            poly[2 * j + 1] = q4[j][2];
        }
        hit = bool_call_ce00(poly, g.e[1], g.e[2]);
    } else if (nz(g.n[1])) {
        for (int j = 0; j < 4; ++j) {
            poly[2 * j] = q4[j][0];
            poly[2 * j + 1] = q4[j][2];
        }
        hit = bool_call_ce00(poly, g.e[0], g.e[2]);
    } else {
        for (int j = 0; j < 4; ++j) {
            poly[2 * j] = q4[j][0];
            poly[2 * j + 1] = q4[j][1];
        }
        hit = bool_call_ce00(poly, g.e[0], g.e[1]);
    }
    if (hit == 0) return 0;
    *out = t;
    return 1;
}

struct H {
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// [ebp-0x70] (4th corner x, block 0x0069fbb0) is never written (store misdirected to [ebp-0x7c]):
// the value computed for it is used.
F("Box::face_hit_69ea70", 0x0069ea70, &face_hit);
#undef F

}  // namespace game_b111_09
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using joan::call::cdecl_call;

struct Raw3 {
    u32 v[3];
};
struct Raw4 {
    u32 v[4];
};
struct Raw5 {
    u32 v[5];
};
static_assert(sizeof(Raw3) == 12 && sizeof(Raw4) == 16 && sizeof(Raw5) == 20, "raw");

// --------------------------------------------------------------------------- cdecl 0x006a0bd0
u32 JOAN_CDECL forward_6a0bd0(void* a, Raw5 b, Raw3 c, Raw4 d) {
    Raw3 tmp;
    const Raw3* const r = joan::orig::arg::as<const Raw3*>(joan::orig::vec_negate(&tmp, &c));  // 0x0060cbc0
    const Raw3 t = *r;
    // result unused: 0x006a0bd0 has no caller (code or data)
    const Raw3 bc{{b.v[0], b.v[1], b.v[2]}};
    return joan::orig::arg::as<u32>(joan::orig::ray_hits_cylinder_699a20(joan::orig::arg::mptr(a), d, t, bc, joan::orig::arg::f32(b.v[3]), joan::orig::arg::f32(b.v[4])));  // 0x00699a20
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("forward_6a0bd0", 0x006a0bd0, &forward_6a0bd0);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b107_15.cpp (game/b107_15)
namespace {
namespace game_b107_15 {

using joan::call::cdecl_call;

using joan::call::import_call;

using joan::call::cdecl_call;
using joan::call::import_call;

struct Vec3 {
    u32 v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");
struct Frame48 {
    u32 v[12];
};
static_assert(sizeof(Frame48) == 48, "Frame48");

// ---- 0x006a0c70 (cdecl, dead code) ----
u32 JOAN_CDECL frame_transform(void* out, Frame48 frame, Vec3 dir, Vec3 pos) {
    float box[6] = {};
    float m[16] = {};
    joan::orig::obb_to_box_frame(joan::orig::arg::mptr(box), joan::orig::arg::mptr(m), &frame);  // 0x006a2b80
    import_call<void*>(d::iat_v3d_87_point_transform_2, &pos, m);          // point_transform in place
    import_call<void*>(d::iat_v3d_87_vector_transform_2, &dir, m);          // vector_transform in place
    Vec3 tmp{};
    const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_negate(&tmp, &dir));  // 0x0060cbc0
    // result unused: 0x006a0c70 has no caller (code or data)
    return joan::orig::arg::as<u32>(joan::orig::sweep_sphere_box_697140(joan::orig::arg::mptr(out), &pos, joan::orig::arg::mptr(&d), box));  // 0x00697140
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_15", name, addr, fn)
B("Frame::transform_6a0c70", 0x006a0c70, &frame_transform);
#undef B

}  // namespace game_b107_15
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {

using joan::call::cdecl_call;

using joan::call::import_call;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

double sqrt_f(float v) { return joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(v))); }                      // 0x004256d0

bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }  // fcomp + test ah,0x40 jnz

JOAN_COUNTER(c_sweep, "sphere_sweep");

// 0x006a0d80: sphere s0 (x y z r) moving along dir towards sphere s1; on contact within
// the move, *t = fraction of dir (0 when already touching). Returns contact.
std::uint8_t JOAN_CDECL sphere_sweep(float* t, const float* s0, const float* dir, const float* s1) {
    c_sweep.hit();
    const joan::hook::x87::Fpu fpu{true};
    const float zero = global<float>(d::k_9cf5c4);
    if (eq_or_nan(s0[3], zero)) return 0;
    if (eq_or_nan(s1[3], zero)) return 0;
    V3 d;
    d.x = s1[0] - s0[0];
    d.y = s1[1] - s0[1];
    d.z = s1[2] - s0[2];
    const float lsq = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(&d)));  // length_sq
    const float dist = static_cast<float>(sqrt_f(lsq));
    const float rs = s1[3] + s0[3];
    if (rs >= dist) {  // fcomp + test ah,1 jnz: less / unordered go on
        *t = 0.0f;
        return 1;
    }
    const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(dir)));  // length
    if (rs - dist > len) return 0;  // test ah,0x41 jnz: <=, unordered go on
    const double dot = import_call<double>(d::iat_v3d_87_dot_product, static_cast<const void*>(dir), static_cast<const void*>(&d));
    const float proj = static_cast<float>(fpu.div(dot, len));
    if (!(proj > zero)) return 0;
    const float perp = lsq - proj * proj;
    const float rr = rs * rs;
    const float disc = rr - perp;
    if (!(disc >= zero)) return 0;
    const float hit = static_cast<float>(fpu.sub(proj, sqrt_f(disc)));
    if (!(len >= hit)) return 0;
    *t = hit / len;
    std::int32_t ok = 0;
    if (*t >= zero && !(*t > global<float>(d::flt_1))) ok = 1;
    return static_cast<std::uint8_t>(ok);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("sphere_sweep", 0x006a0d80, &sphere_sweep);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_quadratic, "solve_quadratic");

// 0x006a1130: roots of a*x^2 + b*x + c; returns the number of roots written.
i16 JOAN_CDECL solve_quadratic(float* r1, float* r2, float a, float b, float c) {
    c_quadratic.hit();
    const float zero = global<float>(d::k_9cf5c4);
    if (a == zero || a != a) {  // test ah,0x40 jne: equal or unordered
        *r2 = -c / b;
        std::memcpy(r1, r2, 4);
        return 1;
    }
    const float bb = b * b;
    float ac = global<float>(d::flt_4) * a;  // 4.0
    ac = ac * c;
    const float d = bb - ac;
    const float inv = global<float>(d::flt_1) / (global<float>(d::flt_2) * a);  // 1 / (2 a)
    if (d == zero || d != d) {
        *r2 = -b * inv;
        std::memcpy(r1, r2, 4);
        return 1;
    }
    if (!(d > zero)) return 0;  // test ah,0x41 jne
    const float s = joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(d)));  // 0x004256d0 sqrt (stored)
    *r1 = (-b - s) * inv;
    *r2 = (-b + s) * inv;
    return 2;
}

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("solve_quadratic", 0x006a1130, &solve_quadratic);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------------- 0x006aac70
// x87 op at precision control 24 with the extended exponent range (copied from game/b99_10).
double round24_wide(double hi, double lo) {
    if (hi == 0.0 || !std::isfinite(hi)) return joan::hook::x87::round_to_float(hi, lo);
    double scale = 1.0;  // powers of two: exact
    while (std::fabs(hi) > 1e30) {
        hi *= 0x1p-64;
        lo *= 0x1p-64;
        scale *= 0x1p64;
    }
    while (std::fabs(hi) < 1e-30) {
        hi *= 0x1p64;
        lo *= 0x1p64;
        scale *= 0x1p-64;
    }
    return joan::hook::x87::round_to_float(hi, lo) * scale;
}

double crt_fabs(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }

JOAN_COUNTER(c_quartic, "quartic_root");

struct FpuWide {
    double add(double a, double b) const {
        const double s = a + b;
        const double bb = s - a;
        return round24_wide(s, (a - (s - bb)) + (b - bb));
    }
    double sub(double a, double b) const { return add(a, -b); }
    double mul(double a, double b) const {
        const double p = a * b;
        return round24_wide(p, joan::fp::fma_exact(a, b, -p));
    }
    double div(double a, double b) const {
        const double q = a / b;
        const double r = joan::fp::fma_exact(-q, b, a);
        return round24_wide(q, b != 0.0 ? r / b : 0.0);
    }
};

static double JOAN_CDECL quartic_root(float a, float b, float c, float d, float e, float x0) {
    c_quartic.hit();
    const FpuWide fpu{};
    const double k4 = global<float>(d::flt_4), k3 = global<float>(d::flt_3), k2 = global<float>(d::flt_2);
    auto value = [&](double t) {
        double v = fpu.mul(a, t);
        v = fpu.add(v, b);
        v = fpu.mul(v, t);
        v = fpu.add(v, c);
        v = fpu.mul(v, t);
        v = fpu.add(v, d);
        v = fpu.mul(v, t);
        return fpu.add(v, e);
    };
    auto slope = [&](double t) {
        double v = fpu.mul(k4, a);
        v = fpu.mul(v, t);
        v = fpu.add(v, fpu.mul(k3, b));
        v = fpu.mul(v, t);
        v = fpu.add(v, fpu.mul(k2, c));
        v = fpu.mul(v, t);
        return fpu.add(v, d);
    };
    double t = x0;
    double fv = value(t);
    double dv = slope(t);
    double best = t;
    double best_abs = crt_fabs(fv);
    for (i32 i = 8; i > 0; --i) {
        t = fpu.sub(t, fpu.div(fv, dv));
        fv = value(t);
        dv = slope(t);
        if (!(crt_fabs(fv) >= best_abs)) {  // fcomp; test ah,1: less or unordered
            best = t;
            best_abs = crt_fabs(fv);
        }
    }
    return best;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("quartic_root", 0x006a12d0, &quartic_root);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b109_18.cpp (game/b109_18)
namespace {
namespace game_b109_18 {

using joan::call::cdecl_call;

float bits_f(u32 b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

// x87 compare outcomes (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool lt_or_nan(double a, double b) { return !(a >= b); }
bool gt(double a, double b) { return a > b; }
bool eq_or_nan(double a, double b) { return !(a < b) && !(a > b); }

JOAN_COUNTER(c_quartic, "solve_quartic_6a1460");

// ------------------------------------------------------------------ 0x006a1460
double quartic_poly(const Fpu& fpu, float a, float b, float c, float d, float e, double t) {
    double v = fpu.mul(a, t);
    v = fpu.add(v, b);
    v = fpu.mul(v, t);
    v = fpu.add(v, c);
    v = fpu.mul(v, t);
    v = fpu.add(v, d);
    v = fpu.mul(v, t);
    return fpu.add(v, e);
}
double quartic_deriv(const Fpu& fpu, float a, float b, float c, float d, double t) {
    double v = fpu.mul(4.0, a);
    v = fpu.mul(v, t);
    v = fpu.add(v, fpu.mul(3.0, b));
    v = fpu.mul(v, t);
    v = fpu.add(v, fpu.mul(2.0, c));
    v = fpu.mul(v, t);
    return fpu.add(v, d);
}
double quartic_self(float a, float b, float c, float d, float e, float lo, float hi) {
    return joan::orig::arg::as<double>(joan::orig::solve_quartic_6a1460(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), joan::orig::arg::f32(e), joan::orig::arg::f32(lo), joan::orig::arg::f32(hi)));  // 0x006a1460
}
double quartic_refine(float a, float b, float c, float d, float e, float x) {
    return joan::orig::arg::as<double>(joan::orig::quartic_root(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), joan::orig::arg::f32(e), joan::orig::arg::f32(x)));  // 0x006a12d0
}

static double JOAN_CDECL solve_quartic(float a, float b, float c, float d, float e, float lo, float hi) {
    c_quartic.hit();
    const Fpu fpu{true};
    if (eq_or_nan(a, 0.0) && eq_or_nan(b, 0.0)) {
        if (eq_or_nan(c, 0.0)) {
            if (eq_or_nan(d, 0.0)) {
                e = 0.0f;  // the original overwrites its argument, then tests it
                if (!eq_or_nan(e, 0.0f)) return hi;
                return lo - 1.0f;
            }
            return -e / d;
        }
        const float disc = d * d - 4.0f * c * e;
        if (lt_or_nan(disc, 0.0)) return lo - 1.0f;
        const double sq = joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(disc))));  // sqrt
        float r = static_cast<float>(fpu.mul(fpu.div(fpu.sub(-static_cast<double>(d), sq), c), 0.5));
        if (lt_or_nan(r, lo)) r = static_cast<float>(fpu.mul(fpu.div(fpu.add(-static_cast<double>(d), sq), c), 0.5));
        return r;
    }
    if (!gt(hi - bits_f(0x358637bdu), lo)) return lo - 1.0f;  // 1e-6
    double t = lo;
    double f = quartic_poly(fpu, a, b, c, d, e, t);
    double fp = quartic_deriv(fpu, a, b, c, d, t);
    for (i32 i = 1; i <= 16; ++i) {
        const double pt = t;
        const double pf = f;
        const double pfp = fp;
        t = fpu.add(fpu.div(fpu.mul(static_cast<double>(hi - lo), static_cast<double>(i)), 16.0), lo);
        f = quartic_poly(fpu, a, b, c, d, e, t);
        fp = quartic_deriv(fpu, a, b, c, d, t);
        const float ptf = static_cast<float>(pt);
        if (gt(pf, 0.0) && lt_or_nan(f, 0.0)) {
            if (lt_or_nan(pfp, 0.0)) return quartic_refine(a, b, c, d, e, ptf);
            return quartic_self(a, b, c, d, e, ptf, static_cast<float>(t));
        }
        if (lt_or_nan(pf, 0.0) && gt(f, 0.0)) {
            if (gt(pfp, 0.0)) return quartic_refine(a, b, c, d, e, ptf);
            return quartic_self(a, b, c, d, e, ptf, static_cast<float>(t));
        }
        if (gt(pf, 0.0) && gt(f, 0.0) && lt_or_nan(pfp, 0.0) && gt(pf, 0.0)) {
            const double r = quartic_self(0.0f, 4.0f * a, 3.0f * b, 2.0f * c, d, ptf, static_cast<float>(t));
            if (!lt_or_nan(r, pt)) {
                const double v = quartic_poly(fpu, a, b, c, d, e, r);
                if (eq_or_nan(v, 0.0)) return r;
                if (lt_or_nan(v, 0.0)) return quartic_refine(a, b, c, d, e, ptf);
            }
            continue;
        }
        if (lt_or_nan(pf, 0.0) && lt_or_nan(f, 0.0) && gt(pfp, 0.0) && lt_or_nan(pf, 0.0)) {
            const double r = quartic_self(0.0f, 4.0f * a, 3.0f * b, 2.0f * c, d, ptf, static_cast<float>(t));
            if (!lt_or_nan(r, pt)) {
                const double v = quartic_poly(fpu, a, b, c, d, e, r);
                if (eq_or_nan(v, 0.0)) return r;
                if (gt(v, 0.0)) return quartic_refine(a, b, c, d, e, ptf);
            }
            continue;
        }
        if (eq_or_nan(f, 0.0)) return t;
    }
    return lo - 1.0f;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_18", name, addr, fn)
B("solve_quartic_6a1460", 0x006a1460, &solve_quartic);
#undef B

}  // namespace game_b109_18
}  // namespace

// ---------------------------------------- from modules/game_b109_17.cpp (game/b109_17)
namespace {
namespace game_b109_17 {

using joan::call::cdecl_call;

using joan::call::import_call;

JOAN_COUNTER(c_collide, "Collide::test_6a1bb0");

// ------------------------------------------------------------------ 0x006a1bb0
struct V3 {
    float x, y, z;
};
struct Sphere {  // centre + radius (16 bytes)
    V3 c;
    float r;
};
struct Cyl {  // centre + radius + height (20 bytes)
    V3 c;
    float r, h;
};
static_assert(sizeof(V3) == 12 && sizeof(Sphere) == 16 && sizeof(Cyl) == 20 && sizeof(Box24) == 24 &&
                  sizeof(Box48) == 48 && sizeof(Mat) == 64,
              "sizes");

void node_update(const u8* o) { joan::orig::as_bone(jo::as_actor(o)->bone)->update(); }  // 0x0060ee60
void* node_matrix(const u8* o) { return joan::orig::arg::as<void*>(joan::orig::as_bone(jo::as_actor(o)->bone)->matrix()); }  // 0x0060ef80
void pt_transform(void* out, const void* in, void* m) { import_call<void*>(d::iat_v3d_87_point_transform_3, out, in, m); }
void pt_transform(void* v, void* m) { import_call<void*>(d::iat_v3d_87_point_transform_2, v, m); }
void vec_transform(void* v, void* m) { import_call<void*>(d::iat_v3d_87_vector_transform_2, v, m); }
void mat_inverse(void* out, void* in) { import_call<void*>(d::iat_v3d_87_matrix_inverse_2, out, in); }
void mat_multiply(void* out, void* a, void* b) { import_call<void*>(d::iat_v3d_87_matrix_multiply_3, out, a, b); }
V3 diff(const float* a, const float* b) { return V3{a[0] - b[0], a[1] - b[1], a[2] - b[2]}; }
Box24 box_of(const u8* o) {
    Box24 v;
    std::memcpy(&v, jo::as_actor(o)->aim_box, sizeof v);
    return v;
}
u32 low_cleared(u32 v) { return v & 0xffffff00u; }

static u32 JOAN_CDECL collide_test(void* out, u8* a, float* ap, u8* b, float* bp) {
    c_collide.hit();
    const u32 ka = jo::as_actor(a)->aim_mode;
    if (ka > 3 || ka == 0) return low_cleared(joan::call::register_low_bits(a));
    const u32 kb = jo::as_actor(b)->aim_mode;
    if (kb > 3) return low_cleared(jo::as_actor(b)->aim_mode);  // falls through the outer cases to "false"
    if (kb == 0) {
        if (ka == 3) return low_cleared(jo::as_actor(b)->aim_mode);
        return low_cleared(joan::call::register_low_bits(b));  // callers (colgrid.cpp) test only the low byte
    }
    if (ka == 1) {
        if (kb == 1) {  // sphere / sphere
            node_update(a);
            Sphere sa;
            pt_transform(&sa.c, jo::as_actor(a)->aim_sphere, node_matrix(a));
            sa.r = jo::as_actor(a)->aim_sphere[3] * jo::as_actor(a)->scale;
            node_update(b);
            Sphere sb;
            pt_transform(&sb.c, jo::as_actor(b)->aim_sphere, node_matrix(b));
            sb.r = jo::as_actor(b)->aim_sphere[3] * jo::as_actor(b)->scale;
            V3 d = diff(ap, bp);
            return joan::orig::arg::as<u32>(joan::orig::sphere_sweep(out, &sa, &d, &sb));  // 0x006a0d80
        }
        if (kb == 2) {  // sphere / box
            node_update(a);
            Sphere sa;
            pt_transform(&sa.c, jo::as_actor(a)->aim_sphere, node_matrix(a));
            const float t = jo::as_actor(a)->aim_sphere[3] * jo::as_actor(a)->scale;
            sa.r = t / jo::as_actor(b)->scale;
            V3 d = diff(ap, bp);
            node_update(b);
            Mat inv;
            mat_inverse(&inv, node_matrix(b));
            pt_transform(&sa.c, &inv);
            vec_transform(&d, &inv);
            return joan::orig::arg::as<u32>(joan::orig::sweep_sphere_box_697140(out, &sa, &d, jo::as_actor(b)->aim_box));  // 0x00697140
        }
        // sphere / cylinder
        node_update(a);
        Sphere sa;
        pt_transform(&sa.c, jo::as_actor(a)->aim_sphere, node_matrix(a));
        sa.r = jo::as_actor(a)->aim_sphere[3] * jo::as_actor(a)->scale;
        node_update(b);
        Cyl cb;
        pt_transform(&cb.c, &jo::as_actor(b)->collide_kind, node_matrix(b));
        cb.r = jo::as_actor(b)->collide_radius * jo::as_actor(b)->scale;
        cb.h = jo::as_actor(b)->collide_height * jo::as_actor(b)->scale;
        const V3 d = diff(ap, bp);
        return joan::orig::arg::as<u32>(joan::orig::ray_hits_cylinder_699a20(out, sa, d, cb.c, cb.r, cb.h));  // 0x00699a20
    }
    if (ka == 2) {
        if (kb == 1) {  // box / sphere
            node_update(b);
            Sphere sb;
            pt_transform(&sb.c, jo::as_actor(b)->aim_sphere, node_matrix(b));
            const float t = jo::as_actor(b)->aim_sphere[3] * jo::as_actor(b)->scale;
            sb.r = t / jo::as_actor(a)->scale;
            V3 d = diff(bp, ap);
            node_update(a);
            Mat inv;
            mat_inverse(&inv, node_matrix(a));
            pt_transform(&sb.c, &inv);
            vec_transform(&d, &inv);
            return joan::orig::arg::as<u32>(joan::orig::sweep_sphere_box_697140(out, &sb, &d, jo::as_actor(a)->aim_box));  // 0x00697140
        }
        if (kb == 2) {  // box / box
            V3 d = diff(ap, bp);
            node_update(a);
            node_update(b);
            Mat inv;
            mat_inverse(&inv, node_matrix(b));
            Mat rel;
            mat_multiply(&rel, node_matrix(a), &inv);
            Box48 box;
            joan::orig::obb_from_box(&box, joan::orig::arg::mptr(jo::as_actor(a)->aim_box), &rel);  // 0x006a30c0
            vec_transform(&d, &inv);
            return joan::orig::arg::as<u32>(joan::orig::collide::box_sweep_698cb0(joan::orig::arg::mptr(out), box, d, box_of(b)));  // 0x00698cb0
        }
        // box / cylinder
        node_update(b);
        Cyl cb;
        pt_transform(&cb.c, &jo::as_actor(b)->collide_kind, node_matrix(b));
        const float tr = jo::as_actor(b)->collide_radius * jo::as_actor(b)->scale;
        cb.r = tr / jo::as_actor(a)->scale;
        const float th = jo::as_actor(b)->collide_height * jo::as_actor(b)->scale;
        cb.h = th / jo::as_actor(a)->scale;
        V3 d = diff(bp, ap);
        node_update(a);
        Mat inv;
        mat_inverse(&inv, node_matrix(a));
        pt_transform(&cb.c, &inv);
        vec_transform(&d, &inv);
        const Box24 ba = box_of(a);
        return joan::orig::arg::as<u32>(joan::orig::sweep_capsule_box(out, cb.c.x, cb.c.y, cb.c.z, cb.r, cb.h, d.x, d.y, d.z, joan::orig::arg::f32(ba.v[0]), joan::orig::arg::f32(ba.v[1]),
                                                                  joan::orig::arg::f32(ba.v[2]), joan::orig::arg::f32(ba.v[3]), joan::orig::arg::f32(ba.v[4]), joan::orig::arg::f32(ba.v[5])));  // 0x00695d90
    }
    // ka == 3
    if (kb == 1) {  // cylinder / sphere
        node_update(b);
        Sphere sb;
        pt_transform(&sb.c, jo::as_actor(b)->aim_sphere, node_matrix(b));
        sb.r = jo::as_actor(b)->aim_sphere[3] * jo::as_actor(b)->scale;
        Cyl ca;
        std::memcpy(&ca, &jo::as_actor(a)->collide_kind, sizeof ca);
        node_update(a);
        pt_transform(&ca.c, &jo::as_actor(a)->collide_kind, node_matrix(a));
        ca.r = jo::as_actor(a)->collide_radius * jo::as_actor(a)->scale;
        ca.h = jo::as_actor(a)->collide_height * jo::as_actor(a)->scale;
        const V3 d = diff(bp, ap);
        return joan::orig::arg::as<u32>(joan::orig::ray_hits_cylinder_699a20(out, sb, d, ca.c, ca.r, ca.h));  // 0x00699a20
    }
    if (kb == 2) {  // cylinder / box
        Cyl ca;
        std::memcpy(&ca, &jo::as_actor(a)->collide_kind, sizeof ca);
        node_update(a);
        pt_transform(&ca.c, &jo::as_actor(a)->collide_kind, node_matrix(a));
        const float tr = jo::as_actor(a)->collide_radius * jo::as_actor(a)->scale;
        ca.r = tr / jo::as_actor(b)->scale;
        const float th = jo::as_actor(a)->collide_height * jo::as_actor(a)->scale;
        ca.h = th / jo::as_actor(b)->scale;
        V3 d = diff(ap, bp);
        node_update(a);  // the original updates a's node again, then inverts b's matrix
        Mat inv;
        mat_inverse(&inv, node_matrix(b));
        pt_transform(&ca.c, &inv);
        vec_transform(&d, &inv);
        const Box24 bb = box_of(b);
        return joan::orig::arg::as<u32>(joan::orig::sweep_capsule_box(out, ca.c.x, ca.c.y, ca.c.z, ca.r, ca.h, d.x, d.y, d.z, joan::orig::arg::f32(bb.v[0]), joan::orig::arg::f32(bb.v[1]),
                                                                  joan::orig::arg::f32(bb.v[2]), joan::orig::arg::f32(bb.v[3]), joan::orig::arg::f32(bb.v[4]), joan::orig::arg::f32(bb.v[5])));  // 0x00695d90
    }
    // cylinder / cylinder
    node_update(a);
    Cyl ca;
    pt_transform(&ca.c, &jo::as_actor(a)->collide_kind, node_matrix(a));
    ca.r = jo::as_actor(a)->collide_radius * jo::as_actor(a)->scale;
    ca.h = jo::as_actor(a)->collide_height * jo::as_actor(a)->scale;
    node_update(b);
    Cyl cb;
    pt_transform(&cb.c, &jo::as_actor(b)->collide_kind, node_matrix(b));
    cb.r = jo::as_actor(b)->collide_radius * jo::as_actor(b)->scale;
    cb.h = jo::as_actor(b)->collide_height * jo::as_actor(b)->scale;
    const V3 d = diff(ap, bp);
    return joan::orig::cylinder_sweep_hit(out, ca.c.x, ca.c.y, ca.c.z, ca.r, ca.h, d.x, d.y, d.z, cb.c.x, cb.c.y, cb.c.z, cb.r, cb.h);  // 0x006994d0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_17", name, addr, fn)
B("Collide::test_6a1bb0", 0x006a1bb0, &collide_test);
#undef B

}  // namespace game_b109_17
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

JOAN_COUNTER(c_box, "Box::from_minmax");

struct H {
    // ------------------------------------------------------------------ 0x006a2a70
    void* box_from_minmax(const float* m) {
        c_box.hit();
        const float half = global<float>(d::flt_0_5);  // 0.5f
        jo::self<jo::OrientedBox>(this)->axes[0] = m[3] - m[0];
        jo::view<u32>(jo::self<jo::OrientedBox>(this)->axes[1]) = 0;
        jo::view<u32>(jo::self<jo::OrientedBox>(this)->axes[2]) = 0;
        jo::view<u32>(jo::self<jo::OrientedBox>(this)->axes[3]) = 0;
        jo::self<jo::OrientedBox>(this)->axes[4] = m[4] - m[1];
        jo::view<u32>(jo::self<jo::OrientedBox>(this)->axes[5]) = 0;
        jo::view<u32>(jo::self<jo::OrientedBox>(this)->axes[6]) = 0;
        jo::view<u32>(jo::self<jo::OrientedBox>(this)->axes[7]) = 0;
        jo::self<jo::OrientedBox>(this)->axes[8] = m[5] - m[2];
        jo::self<jo::OrientedBox>(this)->centre[0] = (m[3] + m[0]) * half;
        jo::self<jo::OrientedBox>(this)->centre[1] = (m[4] + m[1]) * half;
        jo::self<jo::OrientedBox>(this)->centre[2] = (m[5] + m[2]) * half;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("Box::from_minmax", 0x006a2a70, &H::box_from_minmax);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using joan::call::import_call;

JOAN_COUNTER(c_box_transform, "Box::transform");

// 0x006a2fa0
void* JOAN_CDECL box_transform(u8* out, u8* in, void* m) {
    c_box_transform.hit();
    import_call<void*>(d::iat_v3d_87_vector_transform_3, out, in, m);                 // vector_transform
    import_call<void*>(d::iat_v3d_87_vector_transform_3, out + 0xc, in + 0xc, m);     // vector_transform
    import_call<void*>(d::iat_v3d_87_vector_transform_3, out + 0x18, in + 0x18, m);   // vector_transform
    return import_call<void*>(d::iat_v3d_87_point_transform_3, out + 0x24, in + 0x24, m);  // point_transform
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Box::transform", 0x006a2fa0, &box_transform);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::sweep_capsule_box(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, float a12, float a13, float a14) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b106_13::sweep_capsule_box>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)); }  // 0x695d90
auto joan::orig::PlaneEquation::construct(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers37::H::plane_ctor>(this, a0, a1, a2, a3)); }  // 0x699480
auto joan::orig::cylinder_sweep_hit(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, float a12, float a13) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_13::cylinder_sweep_hit>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)); }  // 0x6994d0
auto joan::orig::sphere_sweep_cylinder(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, std::int32_t a11) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b92_06::sphere_sweep_cylinder>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }  // 0x6a09e0
auto joan::orig::sphere_sweep(void* a0, void* a1, void* a2, void* a3) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b90_06::sphere_sweep>(a0, a1, a2, a3)); }  // 0x6a0d80
auto joan::orig::solve2x2(void* a0, void* a1, float a2, float a3, float a4, float a5, float a6, float a7) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b84_04::solve2x2>(a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x6a1250
auto joan::orig::quartic_root(float a0, float a1, float a2, float a3, float a4, float a5) -> double { return joan::bind::conv<double>(joan::bind::free<&::game_b99_10::quartic_root>(a0, a1, a2, a3, a4, a5)); }  // 0x6a12d0
auto joan::orig::solve_quartic_6a1460(float a0, float a1, float a2, float a3, float a4, float a5, float a6) -> double { return joan::bind::conv<double>(joan::bind::free<&::game_b109_18::solve_quartic>(a0, a1, a2, a3, a4, a5, a6)); }  // 0x6a1460
auto joan::orig::collide::test_6a1bb0(void* a0, void* a1, void* a2, void* a3, void* a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b109_17::collide_test>(a0, a1, a2, a3, a4)); }  // 0x6a1bb0
auto joan::orig::obb_to_box_frame(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b91_01::obb_to_box_frame>(a0, a1, a2)); }  // 0x6a2b80
auto joan::orig::box_fns::transform(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b96_08::box_transform>(a0, a1, a2)); }  // 0x6a2fa0
auto joan::orig::obb_transform(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b91_01::obb_transform>(a0, a1)); }  // 0x6a3040
auto joan::orig::obb_from_box(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b90_02::obb_from_box>(a0, a1, a2)); }  // 0x6a30c0
auto joan::orig::shapes_overlap(std::int32_t a0, void* a1, std::int32_t a2, void* a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b106_13::shapes_overlap>(a0, a1, a2, a3)); }  // 0x6a3d10
