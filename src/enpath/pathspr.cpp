// SPDX-License-Identifier: GPL-3.0-or-later
// enpath/pathspr.cpp: functions of the original d:\ww\enpath\pathspr.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (enpath/pathspr.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

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
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::hook::global;

using joan::hook::x87::ftol;

using joan::orig::helpers::wrap_mul;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

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

JOAN_COUNTER(c_box_dist2, "point_box_dist2");

double wide_add(double a, double b) {
    const double s = a + b;
    const double bb = s - a;
    return round24_wide(s, (a - (s - bb)) + (b - bb));
}
double wide_mul(double a, double b) {
    const double p = a * b;
    return round24_wide(p, joan::fp::fma_exact(a, b, -p));
}

float axis_out(float v, float mn, float mx) {
    if (!(v >= mn)) return v - mn;  // fcomp; test ah,1: less or unordered
    if (!(mx >= v)) return v - mx;
    return 0.0f;
}

static double JOAN_CDECL point_box_dist2(float x, float y, float z, const float* box) {
    c_box_dist2.hit();
    const float dx = axis_out(x, box[0], box[3]);
    const float dy = axis_out(y, box[1], box[4]);
    const float dz = axis_out(z, box[2], box[5]);
    double s = wide_mul(dx, dx);
    s = wide_add(s, wide_mul(dy, dy));
    return wide_add(s, wide_mul(dz, dz));
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("point_box_dist2", 0x006aac70, &point_box_dist2);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b111_14.cpp (fix/uninit)
namespace {
namespace game_b111_14 {

float fl(u32 bits) {
    float f;
    std::memcpy(&f, &bits, sizeof f);
    return f;
}
u32 bits_of(float f) {
    u32 b;
    std::memcpy(&b, &f, sizeof b);
    return b;
}

JOAN_COUNTER(c_zone, "find_zone_pair_6aae70");

// ---------------------------------------------------------------- 0x006aae70 (fix/uninit)
u32 JOAN_CDECL find_zone_pair(u8* arr, i32 n, u32 fxb, u32 fyb, u32 a18, u32 a1c, u32 w20, void** out1, u32* out2,
                           u32 b2c) {
    c_zone.hit();
    const float fx = fl(fxb), fy = fl(fyb);
    // *out1 receives the GridCell record (a pointer: every caller passes a pointer-wide slot).
    u8* r1 = nullptr;            // [ebp-0x10] record
    u32 k1 = 0, d1 = 0;          // [-0x14], [-0x18] (written with r1)
    u8* r2 = nullptr;            // [ebp-0x1c] record
    u32 k2 = 0, d2 = 0;          // [-0x20], [-0x24] (written with r2)
    u32 v30 = 0;                 // [ebp-0x30]: read only after a write (see header)
    u32 v38 = 0;                 // [ebp-0x38]: FIX - garbage in the original until first written
    for (i32 i = 0; i < n; ++i) {
        const u32 off = static_cast<u32>(i) * static_cast<u32>(sizeof(jo::GridCell));
        u8* const e = arr + off;
        if (jo::as_grid_cell(e)->box[0] > fx) continue;
        if (fx > jo::as_grid_cell(e)->box[3]) continue;
        if (jo::as_grid_cell(e)->box[1] > fy) continue;
        if (fy > jo::as_grid_cell(e)->box[4]) continue;
        if (r1 != nullptr && !(jo::as_grid_cell(e)->box[5] > fl(d1))) continue;
        if (r2 != nullptr && jo::as_grid_cell(e)->box[2] >= fl(d2)) continue;  // test ah,1 / je: skip unless < or NaN
        u32 v2c = 0, v34 = 0;
        const float dy = fy - jo::as_grid_cell(e)->centre_y;
        const float dx = fx - jo::as_grid_cell(e)->centre_x;
        // the original pushed registers whose upper bits were left over (the record address); the callees read the low byte / word only
        const u32 byte_arg = b2c & 0xffu;
        const u32 word_arg = w20 & 0xffffu;
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(e)->layer(joan::orig::arg::i16(word_arg)));  // 0x006aac10
        joan::orig::as_path(o)->find_6731d0(joan::orig::arg::f32(bits_of(dx)), joan::orig::arg::f32(bits_of(dy)), joan::orig::arg::f32(a18), joan::orig::arg::u32(a1c), &v2c, &v30, &v34, &v38, joan::orig::arg::u32(byte_arg));  // 0x006731d0
        if (v34 != 0) {
            if (r1 == nullptr || fl(v38) > fl(d1)) {
                r1 = e;
                k1 = v34;
                d1 = v38;
            }
        }
        if (v2c != 0) {
            if (r2 == nullptr || fl(v38) > fl(d2)) {  // the original compares [ebp-0x38] here
                r2 = e;
                k2 = v2c;
                d2 = v30;
            }
        }
    }
    if (r1 != nullptr && r2 != nullptr) {
        const float t = fl(d2) - fl(a18);
        const double ab = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(t))));  // fabs
        if (!(ab >= 4.0)) {  // test ah,1 / je: continue when < 4.0 or NaN
            const float a = fl(d2) - fl(a18);
            const float b = fl(a18) - fl(d1);
            if (b > a) {
                *out1 = r2;
                *out2 = k2;
                return 1;
            }
        }
        *out1 = r1;
        *out2 = k1;
        return 1;
    }
    if (r1 != nullptr) {
        *out1 = r1;
        *out2 = k1;
        return 1;
    }
    if (r2 != nullptr) {
        *out1 = r2;
        *out2 = k2;
        return 1;
    }
    *out1 = nullptr;
    *out2 = 0;
    return 0;
}

// fix/uninit: [ebp-0x38] compared before any write (starts at 0.0f here).
JOAN_REPLACE("fix/uninit", "path_space_rec_array_find_polygon_6aae70", 0x006aae70, &find_zone_pair);

}  // namespace game_b111_14
}  // namespace

// ---------------------------------------- from modules/game_b115_12.cpp (fix/uninit)
namespace {
namespace game_b115_12 {

using i16 = std::int16_t;
using u16 = std::uint16_t;

i32 w16(i32 v) { return static_cast<i32>(static_cast<i16>(static_cast<u16>(static_cast<u32>(v) & 0xffffu))); }

JOAN_COUNTER(c_zone, "Zones::find_path_6ab1b0");

// ------------------------------------------------------------------ 0x006ab1b0 (cdecl)
// fix/uninit: see the header. Zone records are 0x34 bytes; +0x10 / +0x14 the zone origin, +0x1c
// the box passed to 0x006aac70.
constexpr float kNoLimit = 2147483520.0f;  // used for the never-written best length
i32 JOAN_CDECL find_zone_path(u8* zones, i32 count, float x, float y, float z, i32 a6, i32 b, u8** out,
                           void* /*unused*/, void* /*unused*/) {
    c_zone.hit();
    u8* best = nullptr;     // [ebp-0x4]
    u8* nearest = nullptr;  // [ebp-0x10]
    float near_dist = 0.0f;  // [ebp-0x14], read only once nearest is set
    for (i32 i = 0; i < count; ++i) {
        const float d = joan::orig::arg::as<float>(joan::orig::point_box_dist2(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::f32(z), joan::orig::arg::mptr(jo::as_grid_cell(zones + wrap_mul(i, static_cast<i32>(sizeof(jo::GridCell))))->box)));  // 0x006aac70
        if (nearest == nullptr || !(d >= near_dist)) {
            nearest = zones + wrap_mul(i, static_cast<i32>(sizeof(jo::GridCell)));
            near_dist = d;
        }
    }
    float best_len = kNoLimit;  // [ebp-0xc]: the original leaves it unwritten (fix)
    if (nearest != nullptr) {
        void* const r = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(nearest)->layer(joan::orig::arg::i16(w16(b))));  // 0x006aac10
        const i32 iz = ftol(static_cast<double>(z));
        const i32 iy = ftol(static_cast<double>(y - jo::as_grid_cell(nearest)->centre_y));
        const i32 ix = ftol(static_cast<double>(x - jo::as_grid_cell(nearest)->centre_x));
        const i32 path = joan::orig::arg::as<i32>(joan::orig::as_path_space(r)->nearest_zone(joan::orig::arg::i32(ix), joan::orig::arg::i32(iy), joan::orig::arg::u32(iz), joan::orig::arg::u32(a6)));  // 0x0066c710
        if (path != 0) {
            void* const p2 = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(r)->zones)->item(joan::orig::arg::i32(path)));  // 0x00637da0
            const float dy = y - jo::as_grid_cell(nearest)->centre_y;
            const float dx = x - jo::as_grid_cell(nearest)->centre_x;
            best_len = joan::orig::arg::as<float>(joan::orig::as_pfd_zone(p2)->min_distance(joan::orig::arg::bits(dx), joan::orig::arg::bits(dy)));  // 0x006730d0
            best = nearest;
        }
    }
    for (i32 i = 0; i < count; ++i) {
        u8* const zone = zones + wrap_mul(i, static_cast<i32>(sizeof(jo::GridCell)));
        if (zone == nearest) continue;
        const float d = joan::orig::arg::as<float>(joan::orig::point_box_dist2(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::f32(z), joan::orig::arg::mptr(jo::as_grid_cell(zone)->box)));
        if (d > best_len) continue;
        void* const r = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(zone)->layer(joan::orig::arg::i16(w16(b))));
        const i32 limit = ftol(static_cast<double>(best_len + global<float>(d::flt_1)));
        const i32 iz = ftol(static_cast<double>(z));
        const i32 iy = ftol(static_cast<double>(y - jo::as_grid_cell(zone)->centre_y));
        const i32 ix = ftol(static_cast<double>(x - jo::as_grid_cell(zone)->centre_x));
        const i32 path = joan::orig::arg::as<i32>(joan::orig::as_map(r)->query_66c900(joan::orig::arg::i32(ix), joan::orig::arg::i32(iy), joan::orig::arg::u32(iz), joan::orig::arg::u32(a6), joan::orig::arg::u32(limit)));  // 0x0066c900
        if (path == 0) continue;
        void* const p2 = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(r)->zones)->item(joan::orig::arg::i32(path)));
        const float dy = y - jo::as_grid_cell(zone)->centre_y;
        const float dx = x - jo::as_grid_cell(zone)->centre_x;
        const float len = joan::orig::arg::as<float>(joan::orig::as_pfd_zone(p2)->min_distance(joan::orig::arg::bits(dx), joan::orig::arg::bits(dy)));
        if (best == nullptr || !(len >= best_len)) {
            best = zone;
            best_len = len;
        }
    }
    if (best == nullptr) return 0;
    *out = best;
    return 1;
}

// fix/uninit (details in the header):
// 0x006ab1b0: the nearest zone's path length [ebp-0xc] is unwritten when 0x0066c710 finds no
// path; 2147483520.0f ("no limit") is used.
JOAN_REPLACE("fix/uninit", "path_space_rec_array_find_nearest_polygon", 0x006ab1b0, &find_zone_path);

}  // namespace game_b115_12
}  // namespace

// ---------------------------------------- from modules/game_b86_03.cpp (game/b86_03)
namespace {
namespace game_b86_03 {

using joan::img::obj;

using joan::call::import_call;

constexpr Fpu kFpu{true};

struct Mat44 {
    float m[16];
};

struct Box6 {
    float v[6];
};

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

struct Vec3 {
    float x, y, z;
};
struct Plane {
    float a, b, c, d;
};
static_assert(sizeof(Vec3) == 12 && sizeof(Box6) == 24 && sizeof(Plane) == 16 && sizeof(Mat44) == 64, "layout");

JOAN_COUNTER(c_find_match, "Players::find_match");
JOAN_COUNTER(c_find_match_orig, "Players::find_match (ran original: unset zone kind)");

// ---------------------------------------------------------------------------- 0x006ab4e0
bool box_overlaps(void* entry, const Box6* box) {  // 0x0060d6b0 on entry+0x1c
    return joan::orig::arg::as<std::uint8_t>(joan::orig::as_box(jo::as_grid_cell(entry)->box)->overlaps(joan::orig::arg::mptr(box))) != 0;
}
void* path_space(void* entry, std::uint32_t sel) {  // 0x006aac10 (reads the word)
    return joan::orig::arg::as<void*>(joan::orig::as_grid_cell(entry)->layer(joan::orig::arg::i16(static_cast<std::int32_t>(static_cast<std::int16_t>(sel)))));
}
std::int32_t zone_count(void* ps) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(ps)->zone_count()); }  // 0x0066b9f0
void* zone_get(void* ps, std::int32_t j) {
    return joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(ps)->zones)->item(joan::orig::arg::i32(j)));  // 0x00637da0
}
std::int32_t* zone_point(void* zone, std::int32_t k) { return jo::view<std::int32_t**>(jo::as_terrain_block(zone)->points)[k]; }
float zone_height(void* zone, float x, float y) { return joan::orig::arg::as<float>(joan::orig::as_terrain_block(zone)->height(joan::orig::arg::f32(x), joan::orig::arg::f32(y))); }  // 0x00664180
bool box_contains(const Box6* box, const Vec3* p) {  // 0x0060d5f0
    return joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(box)->contains(joan::orig::arg::ptr(p))) != 0;
}
float plane_dot(const Plane* plane, const Vec3* p) { return joan::orig::arg::as<float>(joan::orig::plane_distance(joan::orig::arg::mptr(plane), joan::orig::arg::mptr(p))); }  // 0x0060da30

std::int32_t JOAN_CDECL find_match(std::uint8_t* arr, std::int32_t count, Box6 box, const Mat44* matrix,
                                std::uint32_t sel, void** cursor, std::int32_t* id, std::uint32_t unused,
                                std::uint32_t inside_all_arg) {
    c_find_match.hit();
    const bool inside_all = static_cast<std::int8_t>(inside_all_arg & 0xff) != 0;
    auto restart = [&]() -> std::int32_t {
        c_find_match_orig.hit();
        return joan::orig::arg::as<std::int32_t>(joan::orig::players::find_match(joan::orig::arg::mptr(arr), joan::orig::arg::i32(count), box, joan::orig::arg::mptr(matrix), joan::orig::arg::u32(sel), joan::orig::arg::mptr(cursor), joan::orig::arg::mptr(id), joan::orig::arg::u32(unused), joan::orig::arg::u32(inside_all_arg)));  // original body
    };

    std::int32_t i = 0;  // ebp-0x4
    if (*cursor != nullptr) {
        for (i = 0; i < count; ++i) {
            if (*cursor == static_cast<void*>(arr + i * static_cast<std::int32_t>(sizeof(jo::GridCell)))) break;
        }
    }

    if (matrix == nullptr ||
        joan::orig::arg::as<std::int32_t>(joan::orig::libc::memcmp(joan::orig::arg::mptr(matrix), joan::orig::arg::mptr(obj(d::g_identity_matrix)), 0x40u)) == 0) {  // memcmp with identity
        float z = 0.0f;  // ebp-0x34
        bool z_set = false;
        for (; i < count; ++i) {
            std::uint8_t* const e = arr + i * static_cast<std::int32_t>(sizeof(jo::GridCell));  // ebp-0x8
            if (!box_overlaps(e, &box)) continue;
            Box6 local;  // ebp-0x20
            local.v[0] = static_cast<float>(kFpu.sub(box.v[0], static_cast<double>(jo::as_grid_cell(e)->origin_x)));
            local.v[1] = static_cast<float>(kFpu.sub(box.v[1], static_cast<double>(jo::as_grid_cell(e)->origin_y)));
            local.v[2] = box.v[2];
            local.v[3] = static_cast<float>(kFpu.sub(box.v[3], static_cast<double>(jo::as_grid_cell(e)->origin_x)));
            local.v[4] = static_cast<float>(kFpu.sub(box.v[4], static_cast<double>(jo::as_grid_cell(e)->origin_y)));
            local.v[5] = box.v[5];
            void* const ps = path_space(e, sel);  // ebp-0x24
            std::int32_t j = (*cursor == static_cast<void*>(e)) ? *id + 1 : 1;  // ebp-0x28
            for (; j <= zone_count(ps); ++j) {
                void* const zone = zone_get(ps, j);  // ebp-0x2c
                std::int16_t all = 1;                // ebp-0x30
                for (std::int32_t k = 0; k < jo::as_terrain_block(zone)->count; ++k) {
                    Vec3 p;  // ebp-0x3c
                    p.x = static_cast<float>(zone_point(zone, k)[0]);
                    p.y = static_cast<float>(zone_point(zone, k)[1]);
                    if (jo::view<std::int32_t>(jo::as_terrain_block(zone)->u32_4) == 0) {
                        z = zone_height(zone, p.x, p.y);
                        z_set = true;
                    } else if (jo::view<std::int32_t>(jo::as_terrain_block(zone)->u32_4) == 1) {
                        z = static_cast<float>(zone_point(zone, k)[2]);
                        z_set = true;
                    } else if (!z_set && !fix_uninit()) {  // fix/uninit: the never-written slot is 0
                        return restart();
                    }
                    p.z = z;
                    if (inside_all) {
                        if (!box_contains(&local, &p)) {
                            all = 0;
                            break;
                        }
                    } else if (box_contains(&local, &p)) {
                        *cursor = e;
                        *id = j;
                        return 1;
                    }
                }
                if (inside_all && all == 1) {
                    *cursor = e;
                    *id = j;
                    return 1;
                }
            }
        }
        return 0;
    }

    Vec3 t1, t0;  // ebp-0x4c, ebp-0x58
    import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&t1), static_cast<const void*>(&box.v[3]),
                       static_cast<const void*>(matrix));  // point_transform(out, in, m)
    import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&t0), static_cast<const void*>(&box.v[0]),
                       static_cast<const void*>(matrix));
    Plane px, py, pz;  // ebp-0x74, -0x84, -0x94
    joan::orig::as_plane_equation(&px)->init_425700();  // 0x00425700 (empty ctor)
    joan::orig::as_plane_equation(&py)->init_425700();
    joan::orig::as_plane_equation(&pz)->init_425700();
    Vec3 axis;  // ebp-0x64
    axis = Vec3{1.0f, 0.0f, 0.0f};
    import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(&axis), static_cast<const void*>(matrix));  // vector_transform
    joan::orig::vec_cross(&px, &t1, &axis);  // 0x0060d990 plane from point / normal
    axis = Vec3{0.0f, 1.0f, 0.0f};
    import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(&axis), static_cast<const void*>(matrix));
    joan::orig::vec_cross(&py, &t1, &axis);
    axis = Vec3{0.0f, 0.0f, 1.0f};
    import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(&axis), static_cast<const void*>(matrix));
    joan::orig::vec_cross(&pz, &t1, &axis);

    const float zero = global<float>(d::k_9cf5c4);  // 0.0
    float lo_x, hi_x, lo_y, hi_y, lo_z, hi_z;      // ebp-0x98/-0x9c, -0xa0/-0xa4, -0xa8/-0xac
    float d = plane_dot(&px, &t0);                  // ebp-0xb0
    if (d > zero) {
        lo_x = 0.0f;
        hi_x = d;
    } else {
        lo_x = d;
        hi_x = 0.0f;
    }
    d = plane_dot(&py, &t0);
    if (d > zero) {
        lo_y = 0.0f;
        hi_y = d;
    } else {
        lo_y = d;
        hi_y = 0.0f;
    }
    d = plane_dot(&pz, &t0);
    if (d > zero) {
        lo_z = 0.0f;
        hi_z = d;
    } else {
        lo_z = d;
        hi_z = 0.0f;
    }

    Mat44 inverse;  // ebp-0xf0 (unused afterwards)
    import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(&inverse), static_cast<const void*>(matrix));  // matrix_inverse
    Box6 coarse;  // ebp-0x108
    joan::orig::matrix_position(&coarse.v[0], joan::orig::arg::ptr(matrix));  // 0x0060c310 translation
    coarse.v[3] = coarse.v[0];
    coarse.v[4] = coarse.v[1];
    coarse.v[5] = coarse.v[2];
    const float len0 = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&box.v[0])));  // length
    const float len1 = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&box.v[3])));
    const float r = (len0 > len1) ? len0 : len1;  // ebp-0x114
    coarse.v[0] = coarse.v[0] - r;
    coarse.v[3] = coarse.v[3] + r;
    coarse.v[1] = coarse.v[1] - r;
    coarse.v[4] = coarse.v[4] + r;
    coarse.v[2] = coarse.v[2] + box.v[2];
    coarse.v[5] = coarse.v[5] + box.v[5];

    float z = 0.0f;  // ebp-0x12c
    bool z_set = false;
    for (; i < count; ++i) {
        std::uint8_t* const e = arr + i * static_cast<std::int32_t>(sizeof(jo::GridCell));  // ebp-0x118
        if (!box_overlaps(e, &coarse)) continue;
        void* const ps = path_space(e, sel);                                   // ebp-0x11c
        std::int32_t j = (*cursor == static_cast<void*>(e)) ? *id + 1 : 1;     // ebp-0x120
        for (; j <= zone_count(ps); ++j) {
            void* const zone = zone_get(ps, j);  // ebp-0x124
            std::int16_t all = 1;                // ebp-0x128
            for (std::int32_t k = 0; k < jo::as_terrain_block(zone)->count; ++k) {
                Vec3 p;  // ebp-0x134
                p.x = static_cast<float>(static_cast<std::int32_t>(static_cast<std::uint32_t>(zone_point(zone, k)[0]) +
                                                                   static_cast<std::uint32_t>(jo::as_grid_cell(e)->origin_x)));
                p.y = static_cast<float>(static_cast<std::int32_t>(static_cast<std::uint32_t>(zone_point(zone, k)[1]) +
                                                                   static_cast<std::uint32_t>(jo::as_grid_cell(e)->origin_y)));
                if (jo::view<std::int32_t>(jo::as_terrain_block(zone)->u32_4) == 0) {
                    const float hy = static_cast<float>(zone_point(zone, k)[1]);
                    const float hx = static_cast<float>(zone_point(zone, k)[0]);
                    z = zone_height(zone, hx, hy);
                    z_set = true;
                } else if (jo::view<std::int32_t>(jo::as_terrain_block(zone)->u32_4) == 1) {
                    z = static_cast<float>(zone_point(zone, k)[2]);
                    z_set = true;
                } else if (!z_set && !fix_uninit()) {  // fix/uninit: the never-written slot is 0
                    return restart();
                }
                p.z = z;
                const float dx = plane_dot(&px, &p);  // ebp-0x13c
                const float dy = plane_dot(&py, &p);  // ebp-0x140
                const float dz = plane_dot(&pz, &p);  // ebp-0x144
                const bool outside = lo_x > dx || dx > hi_x || lo_y > dy || dy > hi_y || lo_z > dz || dz > hi_z;
                if (inside_all) {
                    if (outside) {
                        all = 0;
                        break;
                    }
                } else if (!outside) {
                    *cursor = e;
                    *id = j;
                    return 1;
                }
            }
            if (inside_all && all == 1) {
                *cursor = e;
                *id = j;
                return 1;
            }
        }
    }
    return 0;
}

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_03", name, addr, fn)
B86("path_space_rec_array_find_polygon_6ab4e0", 0x006ab4e0, &find_match);
#undef B86

}  // namespace game_b86_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::point_box_dist2(float a0, float a1, float a2, void* a3) -> double { return joan::bind::conv<double>(joan::bind::free<&::game_b100_15::point_box_dist2>(a0, a1, a2, a3)); }  // 0x6aac70
auto joan::orig::find_zone_pair_6aae70(void* a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, void* a7, void* a8, std::uint32_t a9) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b111_14::find_zone_pair>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x6aae70
auto joan::orig::zones::find_path_6ab1b0(void* a0, std::int32_t a1, float a2, float a3, float a4, std::int32_t a5, std::int32_t a6, void* a7, void* a8, void* a9) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b115_12::find_zone_path>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x6ab1b0
