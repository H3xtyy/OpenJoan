// SPDX-License-Identifier: GPL-3.0-or-later
// enpath/pathsp.cpp: path-finding functions the Restaurant Empire 2 engine keeps in enpath/pathsp.cpp
// (re/re2_engine_matches.csv). joan.exe's own anchor for a file of this name is ww/uti3d/pathsp.cpp
// (re/attribution_summary.md), so this may be the same original file under the later engine's directory;
// see also uti3d/pathsp.cpp.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cmath>
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

using joan::hook::x87::Fpu;

using joan::call::thiscall;

using joan::call::cdecl_call;

struct P2 {
    std::int32_t x, y;
};

using joan::hook::global;

using joan::call::import_call;

using u8 = std::uint8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::img::obj;

// ---------------------------------------------------------------------------- 0x00669e50
struct Pt {
    i32 x, y;
};

using u16 = std::uint16_t;

using joan::hook::x87::ftol;

using u32 = std::uint32_t;
}  // namespace

// ---------------------------------------- from modules/game_collide1.cpp (game/collide1)
namespace {
namespace game_collide1 {

using joan::hook::x87::ftol;

JOAN_COUNTER(c_height, "TerrainBlock::height");

struct H {
    // 0x00664180 TerrainBlock::height(x, y).
    static void* vertex(H* self, std::int32_t k) { return jo::self<jo::TerrainBlock>(self)->points[k].item; }
    static const P2* xy(void* v, P2* tmp) { return joan::orig::arg::as<P2*>(joan::orig::as_vec2i(v)->copy_to(joan::orig::arg::mptr(tmp))); }  // 0x0066a4f0
    double height(float x, float y) {
        c_height.hit();
        const Fpu fpu{true};
        if (jo::view<std::int32_t>(jo::self<jo::TerrainBlock>(this)->u32_4) == 0 && jo::view<void*>(jo::self<jo::TerrainBlock>(this)->space) != nullptr && jo::self<jo::TerrainBlock>(this)->space->grid != nullptr) {
            std::uint8_t* const g = jo::view<std::uint8_t*>(jo::self<jo::TerrainBlock>(this)->space);
            float fx = static_cast<float>(fpu.div(x, static_cast<double>(jo::as_path_space(g)->cell_w)));
            float fy = static_cast<float>(fpu.div(y, static_cast<double>(jo::as_path_space(g)->cell_h)));
            std::int32_t ix = joan::hook::x87::ftol(fx);
            std::int32_t iy = joan::hook::x87::ftol(fy);
            if (jo::view<void*>(jo::as_path_space(g)->sub_grids) != nullptr) {
                if (ix == static_cast<std::int32_t>(jo::as_path_space(g)->grid->width)) ix = ix - 1;
                if (iy == static_cast<std::int32_t>(jo::as_path_space(g)->grid->height)) iy = iy - 1;
                const std::int32_t w = jo::as_path_space(g)->grid->width;
                const std::int32_t idx = static_cast<std::int32_t>(static_cast<std::uint32_t>(iy) * static_cast<std::uint32_t>(w) + static_cast<std::uint32_t>(ix));
                void* const tile = jo::view<void*>(jo::as_path_space(g)->sub_grids[idx].item);
                if (tile != nullptr) {
                    fx = static_cast<float>(fpu.mul(fpu.sub(fx, static_cast<double>(ix)), static_cast<double>(jo::as_height_grid(tile)->width)));
                    fy = static_cast<float>(fpu.mul(fpu.sub(fy, static_cast<double>(iy)), static_cast<double>(jo::as_height_grid(tile)->height)));
                    return joan::orig::arg::as<double>(joan::orig::as_terrain(tile)->height_4c7b70(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), static_cast<char>(joan::orig::arg::i32(0))));  // 0x004c7b70
                }
            }
            const std::int32_t sum = static_cast<std::int32_t>(static_cast<std::uint32_t>(ix) + static_cast<std::uint32_t>(iy));
            const std::int32_t even = (sum % 2 == 0) ? 1 : 0;
            return joan::orig::arg::as<double>(joan::orig::as_terrain(jo::view<void*>(jo::as_path_space(g)->grid))->height_4c7b70(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), static_cast<char>(joan::orig::arg::i32(even))));
        }
        P2 tmp[8];
        (void)xy(vertex(this, 1), &tmp[0]);  // read and discarded by the original
        const std::int32_t z0 = jo::view<std::int32_t>(jo::as_triple(vertex(this, 1))->c);
        P2 e1, e2;
        {
            const P2 b = *xy(vertex(this, 1), &tmp[1]);
            const P2 a = *xy(vertex(this, 0), &tmp[2]);
            e1 = *joan::orig::arg::as<P2*>(joan::orig::p2::sub(&tmp[3], a, b));  // 0x006639e0 (a - b)
        }
        {
            const P2 b = *xy(vertex(this, 1), &tmp[4]);
            const P2 a = *xy(vertex(this, 2), &tmp[5]);
            e2 = *joan::orig::arg::as<P2*>(joan::orig::p2::sub(&tmp[6], a, b));
        }
        const float lx = static_cast<float>(fpu.sub(x, static_cast<double>(xy(vertex(this, 1), &tmp[7])->x)));
        const float ly = static_cast<float>(fpu.sub(y, static_cast<double>(xy(vertex(this, 1), &tmp[0])->y)));
        const std::int32_t det = static_cast<std::int32_t>(static_cast<std::uint32_t>(e1.x) * static_cast<std::uint32_t>(e2.y) -
                                                           static_cast<std::uint32_t>(e1.y) * static_cast<std::uint32_t>(e2.x));
        if (det != 0) {
            const float a = static_cast<float>(fpu.sub(fpu.mul(e2.y, lx), fpu.mul(e2.x, ly)));
            const float b = static_cast<float>(fpu.sub(fpu.mul(e1.x, ly), fpu.mul(e1.y, lx)));
            const std::int32_t dz0 = static_cast<std::int32_t>(jo::as_triple(vertex(this, 0))->c - static_cast<std::uint32_t>(z0));
            const std::int32_t dz2 = static_cast<std::int32_t>(jo::as_triple(vertex(this, 2))->c - static_cast<std::uint32_t>(z0));
            double r = fpu.add(fpu.mul(dz0, a), fpu.mul(dz2, b));
            r = fpu.div(r, det);
            return fpu.add(r, z0);
        }
        if (e1.x == 0 && e1.y == 0) return static_cast<double>(z0);  // fild: exact integer in st0
        const float s = static_cast<float>(fpu.add(fpu.mul(e1.x, lx), fpu.mul(e1.y, ly)));
        const std::int32_t n = joan::orig::arg::as<std::int32_t>(joan::orig::as_vec2i(&e1)->length_sq());  // 0x0066a440
        const float nf = static_cast<float>(n);
        const std::int32_t dz0 = static_cast<std::int32_t>(jo::as_triple(vertex(this, 0))->c - static_cast<std::uint32_t>(z0));
        return fpu.add(static_cast<double>(z0), fpu.div(fpu.mul(dz0, s), nf));
    }
};

#define CL1(name, addr, fn) JOAN_REPLACE("game/collide1", name, addr, fn)
CL1("PathPolygon::calc_polygon_z", 0x00664180, &H::height);
#undef CL1

}  // namespace game_collide1
}  // namespace

// ---------------------------------------- from modules/game_light1.cpp (game/light1)
namespace {
namespace game_light1 {

struct V3 {
    float x, y, z;
};

struct M44 {
    float m[16];
};
struct Plane4 {
    float a, b, c, d;
};
static_assert(sizeof(P2) == 8 && sizeof(V3) == 12 && sizeof(M44) == 64 && sizeof(Plane4) == 16, "sizes");

JOAN_COUNTER(c_normal, "TerrainBlock::normal");
JOAN_COUNTER(c_pfd_destroy, "PfdData::~PfdData");

struct H {
    // 0x006645b0 TerrainBlock::normal(out, x, y) (ret 0xc) -> 1.
    std::int32_t normal(V3* out, std::int32_t x, std::int32_t y) {
        c_normal.hit();
        const Fpu fpu{true};
        if (jo::view<std::int32_t>(jo::self<jo::TerrainBlock>(this)->u32_4) == 0 && jo::view<std::uint8_t*>(jo::self<jo::TerrainBlock>(this)->space) != nullptr && jo::view<void*>(jo::self<jo::TerrainBlock>(this)->space->grid) != nullptr) {
            std::uint8_t* const g = jo::view<std::uint8_t*>(jo::self<jo::TerrainBlock>(this)->space);
            const float u = static_cast<float>(fpu.div(static_cast<double>(x), static_cast<double>(jo::as_path_space(g)->cell_w)));
            const float v = static_cast<float>(fpu.div(static_cast<double>(y), static_cast<double>(jo::as_path_space(g)->cell_h)));
            // ebp-0x14 / -0x18 = _ftol(u) / _ftol(v): dead, _ftol has no side effects.
            float sx = 0.0f, sy = 0.0f;  // ebp-0x1c / -0x20, always written by the callee
            joan::orig::as_height_grid(jo::view<void*>(jo::self<jo::TerrainBlock>(this)->space->grid))->sample(joan::orig::arg::f32(u), joan::orig::arg::f32(v), &sx, &sy);  // 0x004c7100
            sx = static_cast<float>(fpu.mul(fpu.div(1.0, static_cast<double>(jo::self<jo::TerrainBlock>(this)->space->cell_w)), sx));
            sy = static_cast<float>(fpu.mul(fpu.div(1.0, static_cast<double>(jo::self<jo::TerrainBlock>(this)->space->cell_h)), sy));
            M44 mx;  // ebp-0x64
            M44 my;  // ebp-0xa4
            const float ax = joan::orig::arg::as<float>(joan::orig::libc::atan(joan::orig::arg::f64(static_cast<double>(sx))));  // atan, st0 -> float
            joan::orig::matrix_rotation_60bc00(&mx, joan::orig::arg::f32(-ax));                                 // 0x0060bc00
            const float ay = joan::orig::arg::as<float>(joan::orig::libc::atan(joan::orig::arg::f64(static_cast<double>(sy))));  // atan
            joan::orig::matrix_from_quat(&my, joan::orig::arg::mptr(ay));                                  // 0x0060bbb0
            out->x = global<float>(d::g_a18a60);
            out->y = global<float>(d::g_a18a64);
            out->z = global<float>(d::g_a18a68);
            import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(out), static_cast<void*>(&mx));  // vector_transform
            import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(out), static_cast<void*>(&my));
            return 1;
        }
        V3 p[3];  // ebp-0xb0, -0xbc, -0xc8
        for (std::int32_t k = 0; k < 3; ++k) {
            P2 t0, t1;
            const P2* const a = joan::orig::arg::as<P2*>(joan::orig::as_vec2i(joan::orig::arg::as<void*>(joan::orig::as_pfd_zone(this)->point(joan::orig::arg::i32(k + 1))))->copy_to(&t0));  // 0x00671890, 0x0066a4f0
            p[k].x = static_cast<float>(a->x);
            const P2* const b = joan::orig::arg::as<P2*>(joan::orig::as_vec2i(joan::orig::arg::as<void*>(joan::orig::as_pfd_zone(this)->point(joan::orig::arg::i32(k + 1))))->copy_to(&t1));
            p[k].y = static_cast<float>(b->y);
            p[k].z = joan::orig::arg::as<float>(joan::orig::as_terrain_block(this)->height(joan::orig::arg::f32(p[k].x), joan::orig::arg::f32(p[k].y)));  // 0x00664180
        }
        Plane4 plane;                                // ebp-0xd8
        joan::orig::as_plane_equation(&plane)->init_425700();         // 0x00425700
        joan::orig::plane_from_points(&plane, &p[0], &p[1], &p[2]);  // 0x0060d920
        joan::orig::plane_normal(joan::orig::arg::mptr(out), &plane);  // 0x0060d9e0
        return 1;
    }

    // 0x00665000 PfdData::~PfdData() (ret 0); the members are deleted but not cleared.
    std::uintptr_t pfd_destroy() {  // EAX: the last destructor's result / this (pointer-wide on x64)
        c_pfd_destroy.hit();
        joan::orig::as_path_space(this)->clear_groups();  // 0x00666d80
        joan::orig::as_path_space(this)->clear_grid();  // 0x00665290
        std::uintptr_t r = 0;
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->items) != nullptr) r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::PathSpace>(this)->items))->destroy(1u));  // 0x00463fc0
        jo::BlkArr* jo::PathSpace::* const members[7] = {&jo::PathSpace::tree, &jo::PathSpace::zones, &jo::PathSpace::links, &jo::PathSpace::pairs8, &jo::PathSpace::pairs4, &jo::PathSpace::groups, &jo::PathSpace::points};  // +0x18 +0x14 +0xc +0x8 +0x4 +0x10 +0x0
        for (std::int32_t k = 0; k < 7; ++k) {
            void* const m = jo::self<jo::PathSpace>(this)->*members[k];
            if (m != nullptr) {
                r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_blk_arr(m)->destroy(1u));  // 0x006651e0 scalar deleting destructor
            } else {
                r = reinterpret_cast<std::uintptr_t>(this);
            }
        }
        return r;  // eax: the last destructor's result, or this when +0x0 is null
    }
};

#define L1(name, addr, fn) JOAN_REPLACE("game/light1", name, addr, fn)
L1("PathPolygon::calc_polygon_pos_normal", 0x006645b0, &H::normal);
L1("PathSpace::~PathSpace", 0x00665000, &H::pfd_destroy);
#undef L1

}  // namespace game_light1
}  // namespace

// ---------------------------------------- from modules/game_b100_14.cpp (game/b100_14)
namespace {
namespace game_b100_14 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_recentre, "Group::recentre");

struct H {
    // ------------------------------------------------------------------ 0x00664920
    i32 recentre(i32 a, void* b) {
        c_recentre.hit();
        jo::self<jo::TerrainBlock>(this)->buffer.a = a;
        jo::self<jo::TerrainBlock>(this)->buffer.parent = nullptr;
        jo::self<jo::TerrainBlock>(this)->buffer.u32_c = 0;
        jo::self<jo::TerrainBlock>(this)->buffer.cx = 0;
        jo::self<jo::TerrainBlock>(this)->buffer.cy = 0;
        u32 sz = 0;  // [ebp-0x8]
        u32 sx = 0;  // [ebp-0xc]
        u32 sy = 0;  // [ebp-0x10]
        for (i32 i = 1; i <= jo::self<jo::TerrainBlock>(this)->count; ++i) {
            u32 t1[2];
            u32* r1 = joan::orig::arg::as<u32*>(joan::orig::as_vec2i(jo::self<jo::TerrainBlock>(this)->points[i - 1].item)->copy_to(joan::orig::arg::mptr(static_cast<void*>(t1))));  // 0x0066a4f0
            sx += r1[0];
            u32 t2[2];
            u32* r2 = joan::orig::arg::as<u32*>(joan::orig::as_vec2i(jo::self<jo::TerrainBlock>(this)->points[i - 1].item)->copy_to(joan::orig::arg::mptr(static_cast<void*>(t2))));  // 0x0066a4f0
            sy += r2[1];
            sz += jo::view<u32>(jo::self<jo::TerrainBlock>(this)->points[i - 1].item->xyz[2]);
        }
        jo::self<jo::TerrainBlock>(this)->buffer.cx = static_cast<i32>(sx) / jo::self<jo::TerrainBlock>(this)->count;
        jo::self<jo::TerrainBlock>(this)->buffer.cy = static_cast<i32>(sy) / jo::self<jo::TerrainBlock>(this)->count;
        jo::self<jo::TerrainBlock>(this)->buffer.cz = static_cast<i16>(static_cast<i32>(sz) / jo::self<jo::TerrainBlock>(this)->count);
        if (jo::self<jo::TerrainBlock>(this)->count > static_cast<i32>(jo::self<jo::TerrainBlock>(this)->buffer.capacity)) {
            jo::self<jo::TerrainBlock>(this)->buffer.capacity = static_cast<i16>(jo::self<jo::TerrainBlock>(this)->count);
            if (jo::self<jo::TerrainBlock>(this)->buffer.children != nullptr) joan::crt::operator_delete(joan::orig::arg::mptr(jo::self<jo::TerrainBlock>(this)->buffer.children));  // operator delete
            jo::self<jo::TerrainBlock>(this)->buffer.children = joan::crt::operator_new(joan::orig::arg::u32(static_cast<u32>(static_cast<i32>(jo::self<jo::TerrainBlock>(this)->buffer.capacity)) * static_cast<u32>(sizeof(void*))));  // new (child pointers)
        }
        jo::self<jo::TerrainBlock>(this)->buffer.child_count = 0;
        jo::self<jo::TerrainBlock>(this)->buffer.u8_1f = 0;
        joan::orig::as_member(this)->set_group_664eb0(b);  // 0x00664eb0
        return 0;
    }

    // ------------------------------------------------------------------ 0x00667000
    i32 poly_first(i32 key) {
        return *joan::orig::arg::as<i32*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->groups))->item(joan::orig::arg::i32(key)));  // 0x00637da0
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_14", name, addr, fn)
B100("PathPolygon::init_search_node", 0x00664920, &H::recentre);
B100("PathSpace::get_obstacle_vertex_count", 0x00667000, &H::poly_first);
#undef B100

}  // namespace game_b100_14
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
namespace {
namespace game_b92_05 {

void* const kMath = obj(d::g_misc);

JOAN_COUNTER(c_set_group, "Member::set_group");

struct H {
    // 0x00664eb0
    void set_group(std::uint8_t* g) {
        c_set_group.hit();
        if (jo::view<std::uint8_t*>(jo::self<jo::TerrainBlock>(this)->buffer.parent) == g) return;
        if (jo::self<jo::TerrainBlock>(this)->buffer.parent != nullptr) {
            for (std::int32_t i = jo::self<jo::TerrainBlock>(this)->buffer.parent->buffer.child_count; i > 0; --i) {
                if (static_cast<H**>(jo::self<jo::TerrainBlock>(this)->buffer.parent->buffer.children)[i - 1] != this) continue;
                joan::orig::as_misc(kMath)->del_array_rec_591070(joan::orig::arg::str(jo::self<jo::TerrainBlock>(this)->buffer.parent->buffer.children), static_cast<std::int32_t>(jo::self<jo::TerrainBlock>(this)->buffer.parent->buffer.child_count), static_cast<std::int32_t>(sizeof(void*)), joan::orig::arg::i32(i));  // 0x00591070
                jo::self<jo::TerrainBlock>(this)->buffer.parent->buffer.child_count = static_cast<std::int16_t>(jo::self<jo::TerrainBlock>(this)->buffer.parent->buffer.child_count - 1);
                jo::self<jo::TerrainBlock>(this)->buffer.parent = nullptr;
                break;
            }
        }
        if (g != nullptr) {
            static_cast<H**>(jo::as_terrain_block(g)->buffer.children)[jo::as_terrain_block(g)->buffer.child_count] = this;
            jo::as_terrain_block(g)->buffer.child_count = static_cast<std::int16_t>(jo::as_terrain_block(g)->buffer.child_count + 1);
            jo::view<std::uint8_t*>(jo::self<jo::TerrainBlock>(this)->buffer.parent) = g;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("PathPolygon::set_path_parent", 0x00664eb0, &H::set_group);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14 {

using joan::orig::helpers::op_delete;

JOAN_COUNTER(c_clear_grid, "PfdData::clear_grid");

struct H {
    // 0x00665290 PfdData::clear_grid() (ret 0).
    std::uintptr_t clear_grid() {  // EAX: a reset() result / this (pointer-wide on x64)
        c_clear_grid.hit();
        joan::orig::as_path_space(this)->clear_children();  // 0x006670e0
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->grid) != nullptr) {
            if (jo::view<void*>(jo::self<jo::PathSpace>(this)->sub_grids) != nullptr) {
                for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::PathSpace>(this)->grid->width) *
                                                 static_cast<std::int32_t>(jo::self<jo::PathSpace>(this)->grid->height);
                     ++i) {
                    void* const c = jo::view<void*>(jo::self<jo::PathSpace>(this)->sub_grids[i].item);
                    if (c != nullptr) joan::orig::as_height_grid(c)->destroy(1u);  // 0x00665430
                }
                op_delete(jo::view<void*>(jo::self<jo::PathSpace>(this)->sub_grids));
                jo::view<void*>(jo::self<jo::PathSpace>(this)->sub_grids) = nullptr;
            }
            void* const g = jo::view<void*>(jo::self<jo::PathSpace>(this)->grid);
            if (g != nullptr) joan::orig::as_height_grid(g)->destroy(1u);  // 0x00665430
            jo::view<void*>(jo::self<jo::PathSpace>(this)->grid) = nullptr;
        }
        // eax = result of reset() on +0x0 if non-null, else on +0x4 if non-null, else this
        // (eax is reloaded with this before the +0x4 test, so the +0xc/+0x8 results never survive).
        std::uintptr_t r = reinterpret_cast<std::uintptr_t>(this);
        jo::BlkArr* jo::PathSpace::* const members[4] = {&jo::PathSpace::links, &jo::PathSpace::pairs8, &jo::PathSpace::pairs4, &jo::PathSpace::points};  // +0xc +0x8 +0x4 +0x0
        for (std::int32_t m = 0; m < 4; ++m)
            if (jo::self<jo::PathSpace>(this)->*members[m] != nullptr) {
                const std::uintptr_t v = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_blk_arr(jo::self<jo::PathSpace>(this)->*members[m])->reset());  // 0x00637bb0
                if (m >= 2) r = v;  // +0x4 / +0x0
            }
        return r;
    }

};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("PathSpace::clear_path_list", 0x00665290, &H::clear_grid);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00665260
    u32 call_666d80_665290() {
        joan::orig::as_path_space(this)->clear_groups();         // 0x00666d80
        return joan::orig::arg::as<u32>(joan::orig::as_path_space(this)->clear_grid());  // 0x00665290
    }

    // ------------------------------------------------------------------ 0x00666f70
    i32 touch_10(i32 i) {
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->groups))->item(joan::orig::arg::i32(i));  // 0x00637da0 (result unused)
        joan::orig::as_list(jo::view<void*>(jo::self<jo::PathSpace>(this)->groups))->remove_at(joan::orig::arg::i32(i));    // 0x00637e20
        return 1;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("PathSpace::deinit", 0x00665260, &H::call_666d80_665290);
B103("PathSpace::del_obstacle_polygon", 0x00666f70, &H::touch_10);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

struct H {
    // 0x006670e0
    void clear_children() {
        joan::orig::as_path_space(this)->flush_bvh();  // 0x00674590
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) == nullptr) return;
        for (std::int32_t i = 1; i <= joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->get_2c()); ++i) {  // 0x0066b7a0
            void* const child = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(i)));                    // 0x00637da0
            joan::orig::as_zone(child)->free_points();                                                            // 0x00663f70
        }
        joan::orig::as_blk_arr(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->reset();  // 0x00637bb0
    }

};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("PathSpace::clear_path_polygons", 0x006670e0, &H::clear_children);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_b102_13.cpp (game/b102_13)
namespace {
namespace game_b102_13 {

using u32 = std::uint32_t;

// Block array helpers (uti3d blkarrb).
i32 arr_count(void* a) { return joan::orig::arg::as<i32>(joan::orig::as_path_space(a)->get_2c()); }          // 0x0066b7a0
u8* arr_get(void* a, i32 n) { return joan::orig::arg::as<u8*>(joan::orig::as_billboard(a)->item(joan::orig::arg::i32(n))); }  // 0x00637da0
u32 arr_add(void* a, void* e) { return joan::orig::arg::as<u32>(joan::orig::as_block_array(a)->add(e)); }  // 0x00637be0
void* arr_new(u32 elem, u32 grow) {
    void* const p = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::BlkArr)));  // 0x30 on x86; operator new
    if (p == nullptr) return nullptr;
    return joan::orig::as_blk_arr(p)->construct(joan::orig::arg::u32(elem), joan::orig::arg::u32(grow));  // 0x00637aa0
}
const u32* make_pt(u32* pt, u32 x, u32 y) { return joan::orig::arg::as<const u32*>(joan::orig::as_point(pt)->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y))); }  // 0x004eeb80

JOAN_COUNTER(c_regions, "Regions::b102_13");

struct H {
    // ================================================================== Regions (dead code)
    // ------------------------------------------------------------------ 0x00665480
    u32 add_point(u32 x, u32 y, u32 z) {
        c_regions.hit();
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->points) == nullptr) jo::view<void*>(jo::self<jo::PathSpace>(this)->points) = arr_new(static_cast<u32>(sizeof(jo::Triple)), 0x800);
        u32 pt[2];
        const u32* const p = make_pt(pt, x, y);
        u32 tri[3];
        joan::orig::as_triple(tri)->construct(joan::orig::arg::u32(p[0]), joan::orig::arg::u32(p[1]), joan::orig::arg::u32(z));  // 0x0066a4b0
        return arr_add(jo::view<void*>(jo::self<jo::PathSpace>(this)->points), tri);
    }

    // ------------------------------------------------------------------ 0x00665b20 / 0x00666160
    u32 add_pair(jo::BlkArr* jo::PathSpace::* off, u32 grow, i32 a, i32 b) {
        c_regions.hit();
        if (a == b) return 0;
        if (jo::self<jo::PathSpace>(this)->*off == nullptr) jo::self<jo::PathSpace>(this)->*off = static_cast<jo::BlkArr*>(arr_new(static_cast<u32>(2 * sizeof(void*)), grow));  // PathLine records (two Triple pointers): 8 bytes on x86
        u8* const eb = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->points), b);
        u8* const ea = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->points), a);
        void* pair[2];
        joan::orig::as_path_line(pair)->construct(joan::orig::as_triple(ea), joan::orig::as_triple(eb));  // 0x00665c20
        return arr_add(jo::self<jo::PathSpace>(this)->*off, pair);
    }
    u32 add_pair_4(i32 a, i32 b) { return add_pair(&jo::PathSpace::pairs4, 0x800, a, b); }
    u32 add_pair_8(i32 a, i32 b) { return add_pair(&jo::PathSpace::pairs8, 0x80, a, b); }

    // ------------------------------------------------------------------ 0x00665f70 / 0x00666590
    i32 get_pair_idx(jo::BlkArr* jo::PathSpace::* off, i32 n, u32* oa, u32* ob) {
        c_regions.hit();
        if (jo::self<jo::PathSpace>(this)->*off == nullptr || n <= 0 || n > arr_count(jo::self<jo::PathSpace>(this)->*off)) return 0;
        u8* const e = arr_get(jo::self<jo::PathSpace>(this)->*off, n);
        *oa = joan::orig::arg::as<u32>(joan::orig::as_path_space(this)->point_index(jo::as_pair(e)->a));  // 0x00665940
        *ob = joan::orig::arg::as<u32>(joan::orig::as_path_space(this)->point_index(jo::as_pair(e)->b));  // 0x00665940
        return n;
    }
    i32 get_pair_idx_4(i32 n, u32* oa, u32* ob) { return get_pair_idx(&jo::PathSpace::pairs4, n, oa, ob); }
    i32 get_pair_idx_8(i32 n, u32* oa, u32* ob) { return get_pair_idx(&jo::PathSpace::pairs8, n, oa, ob); }

};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_13", name, addr, fn)
B("PathSpace::add_point", 0x00665480, &H::add_point);
B("PathSpace::add_path_line", 0x00665b20, &H::add_pair_4);
B("PathSpace::get_path_line", 0x00665f70, &H::get_pair_idx_4);
B("PathSpace::add_assist_line", 0x00666160, &H::add_pair_8);
B("PathSpace::get_assist_line", 0x00666590, &H::get_pair_idx_8);
#undef B

}  // namespace game_b102_13
}  // namespace

// ---------------------------------------- from modules/game_b101_13.cpp (game/b101_13)
namespace {
namespace game_b101_13 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_regions, "Regions::check");

struct H {
    // ------------------------------------------------------------------ 0x00665570 / 0x00665c60 / 0x00666880
    u32 check_lists(jo::BlkArr* jo::PathSpace::* src_off, i32 n) {
        c_regions.hit();
        if (jo::self<jo::PathSpace>(this)->*src_off == nullptr || n <= 0) return 0;
        if (n > joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::self<jo::PathSpace>(this)->*src_off)->get_2c())) return 0;  // 0x0066b7a0
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::self<jo::PathSpace>(this)->*src_off)->item(joan::orig::arg::i32(n)));  // 0x00637da0
        struct L {
            jo::BlkArr* jo::PathSpace::* off;
            std::uintptr_t test;
        };
        const L lists[5] = {{&jo::PathSpace::points, fn::code_value(fn::stub_zero)},    // 0x0066a5e0
                            {&jo::PathSpace::pairs4, fn::code_value(fn::Pair_contains)},    // 0x006657d0
                            {&jo::PathSpace::pairs8, fn::code_value(fn::Pair_contains)},    // 0x006657d0
                            {&jo::PathSpace::links, fn::code_value(fn::Pair_contains)},    // 0x006657d0
                            {&jo::PathSpace::zones, fn::code_value(fn::PathPolygon_is_depend)}};  // 0x00663ff0
        for (int k = 0; k < 5; ++k) {
            if (jo::self<jo::PathSpace>(this)->*lists[k].off == nullptr) continue;
            for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::self<jo::PathSpace>(this)->*lists[k].off)->get_2c()); i > 0; --i) {
                void* const o = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::self<jo::PathSpace>(this)->*lists[k].off)->item(joan::orig::arg::i32(i)));
                if (thiscall<i32>(lists[k].test, o, e) != 0) return 0;
            }
        }
        return 1;
    }
    u32 check_665570(i32 n) { return check_lists(&jo::PathSpace::points, n); }
    u32 check_665c60(i32 n) { return check_lists(&jo::PathSpace::pairs4, n); }
    u32 check_666880(i32 n) { return check_lists(&jo::PathSpace::links, n); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_13", name, addr, fn)
B("PathSpace::can_del_point", 0x00665570, &H::check_665570);
B("PathSpace::can_del_path_line", 0x00665c60, &H::check_665c60);
B("PathSpace::can_del_sub_path_line", 0x00666880, &H::check_666880);
#undef B

}  // namespace game_b101_13
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

using u32 = std::uint32_t;

using joan::orig::helpers::op_new;

// Block array (0x00637aa0 ctor): +0x2c count.
u8* arr_get(void* arr, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_billboard(arr)->item(joan::orig::arg::i32(i))); }        // 0x00637da0
i32 arr_count(void* arr) { return joan::orig::arg::as<i32>(joan::orig::as_path_space(arr)->get_2c()); }                // 0x0066b7a0
u32 arr_push(void* arr, const void* e) { return joan::orig::arg::as<u32>(joan::orig::as_block_array(arr)->add(const_cast<void*>(e))); }  // 0x00637be0
void* new_block_array(i32 elem, i32 grow) {
    void* const p = op_new(static_cast<std::uint32_t>(sizeof(jo::BlkArr)));  // 0x30 on x86
    if (p == nullptr) return nullptr;
    return joan::orig::as_blk_arr(p)->construct(joan::orig::arg::u32(elem), joan::orig::arg::u32(grow));  // 0x00637aa0
}

JOAN_COUNTER(c_add_link, "LinkSet::add_link");
JOAN_COUNTER(c_add_group, "LinkSet::add_group");

struct H {
    void* list() { return jo::view<void*>(jo::self<jo::PathSpace>(this)->points); }

    // ------------------------------------------------------------------ 0x00666780
    u32 add_link(i32 a, i32 b) {
        c_add_link.hit();
        if (a == b) return 0;
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->links) == nullptr) jo::view<void*>(jo::self<jo::PathSpace>(this)->links) = new_block_array(8, 0x80);
        u8* const eb = arr_get(list(), b);
        u8* const ea = arr_get(list(), a);
        void* pair[2];
        joan::orig::as_path_line(pair)->construct(joan::orig::as_triple(ea), joan::orig::as_triple(eb));  // 0x00665c20
        return arr_push(jo::view<void*>(jo::self<jo::PathSpace>(this)->links), pair);
    }
    // ------------------------------------------------------------------ 0x00666b90
    i32 link_ids(i32 i, i32* a, i32* b) {
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->links) == nullptr || i <= 0 || i > arr_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->links))) return 0;
        u8* const e = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->links), i);
        *a = joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->point_index(jo::as_pair(e)->a));  // 0x00665940
        *b = joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->point_index(jo::as_pair(e)->b));
        return i;
    }
    // ------------------------------------------------------------------ 0x00666db0
    u32 add_group() {
        c_add_group.hit();
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->groups) == nullptr) jo::view<void*>(jo::self<jo::PathSpace>(this)->groups) = new_block_array(0x34, 0x80);
        u32 group[0x34 / 4];
        joan::orig::as_path_group(group)->construct();  // 0x00666e80
        return arr_push(jo::view<void*>(jo::self<jo::PathSpace>(this)->groups), group);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("PathSpace::add_sub_path_line", 0x00666780, &H::add_link);
B("PathSpace::get_sub_path_line", 0x00666b90, &H::link_ids);
B("PathSpace::add_obstacle_polygon", 0x00666db0, &H::add_group);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b115_08.cpp (fix/uninit)
namespace {
namespace game_b115_08 {

using joan::img::str;

using u32 = std::uint32_t;
using uptr = std::uintptr_t;

using Vtx = jo::MeshVertex;  // the 0x14-byte (x86) vertex record: point, next, prev, assist next, assist prev
// The edge items are PathRec records (Rec::set 0x0066a320: the two vertices and the triangle zone as pointers).
bool at_point(const Vtx* r, const void* point) { return static_cast<const void*>(r->point) == point; }
using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;
i32 wmul(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) * static_cast<u32>(b)); }
i32 sx16(u32 v) { return static_cast<i32>(static_cast<i16>(static_cast<u16>(v))); }

JOAN_COUNTER(c_mesh, "PathMesh::build_667190");

// The edge item (0x0066a320 Rec::set): PathRec a, b (the edge's vertices), (u16)arg3 -> c, (u16)arg5 -> e, arg4 -> d
// (the triangle zone); 16 bytes on x86.
using Item = jo::PathRec;
using LogFn = void(JOAN_CDECL*)(const char*, ...);

Pt rec_pt(Vtx* rec) {  // 0x0066a2e0 (thunk 0x00407fd1), struct return through out pointer
    Pt o{0, 0};
    const Pt* r = joan::orig::arg::as<Pt*>(joan::orig::as_mesh_vertex(rec)->position(&o));
    return *r;
}
Pt seg_pt(const void* point) {  // 0x0066a4f0 (thunk 0x0040a45c)
    Pt o{0, 0};
    const Pt* r = joan::orig::arg::as<Pt*>(joan::orig::as_vec2i(point)->copy_to(&o));
    return *r;
}
Pt vec(i32 x1, i32 y1, i32 x2, i32 y2) {  // 0x006639e0 (thunk 0x00408035), cdecl, out first
    Pt o{0, 0};
    const Pt* r = joan::orig::arg::as<Pt*>(joan::orig::p2::sub(&o, P2{x1, y1}, P2{x2, y2}));
    return *r;
}
i32 count_of(const void* o) { return joan::orig::arg::as<i32>(joan::orig::as_path_space(o)->get_2c()); }   // 0x0066b7a0
i32 list_count(const void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0
void list_add(void* l, const void* item) { joan::orig::dynarray(l)->linkin(joan::orig::arg::ptr(item)); }  // 0x00584620
void list_remove(void* l, i32 i) { joan::orig::dynarray(l)->linkout(joan::orig::arg::i32(i)); }  // 0x00584a10
u8* list_at(void* l, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(l)->get(joan::orig::arg::i32(i))); }  // 0x00417200
void list_dtor(void* l) { joan::orig::dynarray(l)->destruct(); }                    // 0x005844b0
void grid_dtor(void* g) { joan::orig::as_sp_index(g)->dtor(); }                         // 0x00681ee0
void item_ctor(Item* it, Vtx* a, Vtx* b, u32 w8, void* c, u32 wa) {
    joan::orig::as_mesh_edge(it)->set(a, b, joan::orig::arg::i32(w8), c, joan::orig::arg::i32(wa));  // 0x0066a320
}
// 0x0066fb40 (direct, cdecl): enters the edge (last item of `list`) into `grid`.
void grid_enter(void* list, Pt p1, Pt p2, void* grid) {
    // argument evaluation order of the original: p2's 0x0066a2e0 first, then p1's (done by the
    // callers), then 0x00417520 on the list.
    const std::uint32_t last = static_cast<std::uint32_t>(joan::orig::as_dynarray(list)->cursor());  // 0x00417520 (the index of the last item)
    joan::orig::sp_index::add_rect(last, joan::orig::arg::i32(p1.x), joan::orig::arg::i32(p1.y), joan::orig::arg::i32(p2.x), joan::orig::arg::i32(p2.y), joan::orig::arg::mptr(grid));
}

struct H {
    // ------------------------------------------------------------------ 0x00667190 (fix)
    i32 mesh_build(i32 flag, uptr log_fn) {
        c_mesh.hit();
        const LogFn log = reinterpret_cast<LogFn>(log_fn);
        void* const err = obj(d::g_err);
        void* lines = jo::view<void*>(jo::self<jo::PathSpace>(this)->pairs4);  // [ebp-0x14]
        u32 kind = 1;               // [ebp-0x18]
        if (flag != 0) {
            lines = jo::view<void*>(jo::self<jo::PathSpace>(this)->links);
            kind = 4;
        }
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->points) == nullptr || lines == nullptr) {
            if (log != nullptr) log(str(d::s_no_point_or_no_path_line_fin));
            return 1;
        }
        i32 total = 0;
        if (count_of(jo::view<void*>(jo::self<jo::PathSpace>(this)->points)) < wmul(count_of(lines), 2))
            total = count_of(jo::view<void*>(jo::self<jo::PathSpace>(this)->points));
        else
            total = wmul(count_of(lines), 2);
        for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->group_count()); ++i)  // 0x00666fc0
            total = wadd(total, joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->group_size(joan::orig::arg::i32(i))));  // 0x00667000
        alignas(alignof(void*)) u8 blob[sizeof(jo::Blob)];  // 8 bytes on x86 (data pointer, size)
        joan::orig::as_blob(static_cast<void*>(blob))->construct(joan::orig::arg::i32(wmul(total, static_cast<i32>(sizeof(Vtx)))));  // 0x0057f770
        Vtx* const recs = joan::orig::arg::as<Vtx*>(joan::orig::as_blob(static_cast<void*>(blob))->data_56de20());  // 0x0056de20
        auto rec = [recs](i32 i) { return recs + (i - 1); };  // 1-based
        i32 cnt = 0;  // [ebp-0x30]
        alignas(alignof(void*)) u8 open_pts[((sizeof(jo::DynArray) > 0x20 ? sizeof(jo::DynArray) : 0x20) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger DynArray
        joan::orig::dynarray(static_cast<void*>(open_pts))->construct(static_cast<i32>(sizeof(Vtx*)), 0x800, 0);  // 0x005843d0
        auto fail0 = [&]() {
            list_dtor(open_pts);
            joan::orig::as_blob(static_cast<void*>(blob))->destruct();  // 0x0057f850
            return 0;
        };
        auto report = [&](img::Addr fmt_log, img::Addr fmt_err, const Vtx* r) {
            if (log != nullptr) log(str(fmt_log), r->point->xyz[0], r->point->xyz[1]);
            joan::orig::warning_box(joan::orig::arg::mptr(err), joan::orig::arg::str(str(fmt_err)), r->point->xyz[0], r->point->xyz[1]);  // 0x00586bc0
        };
        auto log_segment = [&](img::Addr fmt, const jo::Pair* seg) {
            if (log == nullptr) return;
            const i32 by = seg_pt(seg->b).y;
            const i32 bx = seg_pt(seg->b).x;
            const i32 ay = seg_pt(seg->a).y;
            const i32 ax = seg_pt(seg->a).x;
            log(str(fmt), ax, ay, bx, by);
        };
        auto zero_length = [&](const void* a) {  // a: the segment's (PfdPoint) end
            if (log != nullptr) log(str(d::s_an_zero_length_link_at_d_d), jo::as_pfd_point(a)->xyz[0], jo::as_pfd_point(a)->xyz[1]);
            joan::orig::warning_box(joan::orig::arg::mptr(err), str(d::s_an_zero_length_link_at_d_d), jo::as_pfd_point(a)->xyz[0], jo::as_pfd_point(a)->xyz[1]);
        };
        auto find_or_add = [&](const void* point, Vtx*& out) {
            if (out != nullptr) return;
            cnt = cnt + 1;
            out = rec(cnt);
            joan::orig::as_mesh_vertex(out)->construct(joan::orig::arg::mptr(point));  // 0x0066a280
        };
        for (i32 i = 1; i <= count_of(lines); ++i) {
            const jo::Pair* const seg = joan::orig::arg::as<jo::Pair*>(joan::orig::as_billboard(lines)->item(joan::orig::arg::i32(i)));  // 0x00637da0
            log_segment(d::s_Source_path_line_d_d_d_d, seg);
            const void* const a = seg->a;
            const void* const b = seg->b;
            if (b == a) {
                zero_length(a);
                return fail0();
            }
            Vtx *ra = nullptr, *rb = nullptr;  // [ebp-0x64] / [ebp-0x68]
            for (i32 k = 1; k <= cnt; ++k) {
                if (at_point(rec(k), a)) ra = rec(k);
                if (at_point(rec(k), b)) rb = rec(k);
            }
            find_or_add(a, ra);
            find_or_add(b, rb);
            if (ra->next == nullptr && ra->prev == nullptr) list_add(open_pts, &ra);
            if (ra->next != nullptr) {
                report(d::s_point_at_d_d_is_linked_more_9dbddc, d::s_point_at_d_d_is_linked_more_9dbddc, ra);
                return fail0();
            }
            if (rb->prev != nullptr) {
                report(d::s_point_at_d_d_is_linked_more_9dbddc, d::s_point_at_d_d_is_linked_more_9dbddc, rb);
                return fail0();
            }
            ra->next = rb;
            rb->prev = ra;
        }
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->groups) != nullptr) {
            for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->group_count()); ++i) {
                const i32 k = joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->group_size(joan::orig::arg::i32(i)));
                if (k < 3) continue;
                const u8* const pts = joan::orig::arg::as<u8*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->groups))->item(joan::orig::arg::i32(i)));
                const i32 first = cnt;
                cnt = wadd(cnt, k);
                for (i32 j = 1; j <= k; ++j)
                    joan::orig::as_mesh_vertex(rec(first + j))->construct(joan::orig::arg::mptr(&jo::as_path_group(pts)->entries[j - 1]));
                Vtx* ring = rec(first + 1);  // [ebp-0x84]
                list_add(open_pts, &ring);
                for (i32 j = 1; j <= k; ++j) {
                    i32 nj = j + 1;
                    if (nj > k) nj = 1;
                    ring[j - 1].next = &ring[nj - 1];
                    ring[nj - 1].prev = &ring[j - 1];
                }
            }
        }
        for (i32 i = list_count(open_pts); i > 0; --i) {
            Vtx* const r = joan::orig::arg::as<Vtx*>(joan::orig::as_dynarray(static_cast<void*>(open_pts))->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (r->next == nullptr && r->prev == nullptr) {
                list_remove(open_pts, i);
                continue;
            }
            if (r->next == nullptr || r->prev == nullptr) {
                report(d::s_point_at_d_d_is_not_linked_t, d::s_point_at_d_d_is_not_linked_t, r);
                return fail0();
            }
            i32 limit = cnt + 2;
            for (Vtx* q = r->next; q != r; q = q->next) {
                limit = limit - 1;
                if (limit <= 0) {
                    report(d::s_point_at_d_d_is_closed_in_ci, d::s_point_at_d_d_is_closed_in_ci, r);
                    return fail0();
                }
                if (q->next == nullptr || q->prev == nullptr) {
                    report(d::s_point_at_d_d_is_not_linked_t, d::s_point_at_d_d_is_not_linked_t, q);
                    return fail0();
                }
            }
        }
        void* const assist = jo::view<void*>(jo::self<jo::PathSpace>(this)->pairs8);
        if (assist != nullptr && count_of(assist) > 0) {
            for (i32 i = 1; i <= count_of(assist); ++i) {
                const jo::Pair* const seg = joan::orig::arg::as<jo::Pair*>(joan::orig::as_billboard(assist)->item(joan::orig::arg::i32(i)));
                log_segment(d::s_Assist_Line_d_d_d_d, seg);
                const void* const a = seg->a;
                const void* const b = seg->b;
                if (b == a) {
                    zero_length(a);
                    return fail0();
                }
                Vtx *ra = nullptr, *rb = nullptr;  // [ebp-0xb0] / [ebp-0xb4]
                for (i32 k = 1; k <= cnt; ++k) {
                    if (at_point(rec(k), a)) ra = rec(k);
                    if (at_point(rec(k), b)) rb = rec(k);
                }
                find_or_add(a, ra);
                find_or_add(b, rb);
                if (ra->assist_next != nullptr) {
                    report(d::s_point_at_d_d_is_linked_more_9dbd04, d::s_point_at_d_d_is_linked_more_9dbddc, ra);
                    return fail0();
                }
                if (rb->assist_prev != nullptr) {
                    report(d::s_point_at_d_d_is_linked_more_9dbd04, d::s_point_at_d_d_is_linked_more_9dbddc, rb);
                    return fail0();
                }
                ra->assist_next = rb;
                rb->assist_prev = ra;
            }
        }
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) == nullptr) {
            void* const raw = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::BlkArr)));  // 0x30 on x86; operator new
            jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) = raw != nullptr ? joan::orig::as_blk_arr(raw)->construct(static_cast<u32>(sizeof(jo::PfdZone)), 0x800u) : nullptr;  // 0x00637aa0 (PfdZone records: 0x74 bytes on x86)
        }
        const i32 tri_before = count_of(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones));  // [ebp-0xbc]
        alignas(alignof(void*)) u8 open_edges[((sizeof(jo::DynArray) > 0x20 ? sizeof(jo::DynArray) : 0x20) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0xdc] (x64: the larger DynArray)
        alignas(alignof(void*)) u8 edges[((sizeof(jo::DynArray) > 0x20 ? sizeof(jo::DynArray) : 0x20) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];       // [ebp-0xfc] (x64: the larger DynArray)
        alignas(alignof(void*)) u8 grid[((sizeof(jo::SpIndex) > 0x48 ? sizeof(jo::SpIndex) : 0x48) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];        // [ebp-0x144] (x64: the larger SpIndex)
        alignas(alignof(void*)) u8 assist_edges[((sizeof(jo::DynArray) > 0x20 ? sizeof(jo::DynArray) : 0x20) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x164] (x64: the larger DynArray)
        alignas(alignof(void*)) u8 assist_grid[((sizeof(jo::SpIndex) > 0x48 ? sizeof(jo::SpIndex) : 0x48) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];   // [ebp-0x1ac] (x64: the larger SpIndex)
        joan::orig::dynarray(static_cast<void*>(open_edges))->construct(static_cast<i32>(sizeof(Item)), 0x800, 0);
        joan::orig::dynarray(static_cast<void*>(edges))->construct(static_cast<i32>(sizeof(Item)), 0x800, 0);
        joan::orig::as_grid(static_cast<void*>(grid))->Grid_681ea0();  // 0x00681ea0
        joan::orig::dynarray(static_cast<void*>(assist_edges))->construct(static_cast<i32>(sizeof(Item)), 0x800, 0);
        joan::orig::as_grid(static_cast<void*>(assist_grid))->Grid_681ea0();
        i32 maxx = 0, maxy = 0;
        for (i32 i = 1; i <= cnt; ++i) {
            if (rec_pt(rec(i)).x > maxx) maxx = rec_pt(rec(i)).x;
            if (rec_pt(rec(i)).y > maxy) maxy = rec_pt(rec(i)).y;
        }
        i32 maxlen = 0;  // fix/uninit: [ebp-0x1bc] is compared before its first write
        for (i32 i = 1; i <= cnt; ++i) {
            Item dead{};  // [ebp-0x1cc], constructed and never used
            item_ctor(&dead, rec(i), rec(i)->next, 1, nullptr, 0);
            const Pt pn = rec_pt(rec(i)->next);
            const Pt pc = rec_pt(rec(i));
            Pt d = vec(pc.x, pc.y, pn.x, pn.y);
            const i32 sq = joan::orig::arg::as<i32>(joan::orig::as_vec2i(&d)->length_sq());  // 0x0066a440
            const i32 len = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->sqrt_int_590340(joan::orig::arg::i32(sq)));  // 0x00590340
            if (len > maxlen) maxlen = len;
        }
        i32 cell = maxlen;
        if (cell < (maxx > maxy ? maxx : maxy) / 16) cell = (maxx > maxy ? maxx : maxy) / 16;
        joan::orig::as_sp_index(static_cast<void*>(grid))->init_681fe0(joan::orig::arg::u32(maxx + 1), joan::orig::arg::u32(maxy + 1), joan::orig::arg::u32(cell));  // 0x00681fe0
        joan::orig::as_sp_index(static_cast<void*>(assist_grid))->init_681fe0(joan::orig::arg::u32(maxx + 1), joan::orig::arg::u32(maxy + 1), joan::orig::arg::u32(cell));
        for (i32 i = 1; i <= cnt; ++i) {
            Item it{};  // [ebp-0x1e4]
            item_ctor(&it, rec(i), rec(i)->next, 1, nullptr, 0);
            list_add(open_edges, &it);
            list_add(edges, &it);
            {
                const Pt pb = rec_pt(it.b);
                const Pt pa = rec_pt(it.a);
                grid_enter(edges, pa, pb, grid);
            }
            if (rec(i)->assist_next != nullptr) {
                Item as{};  // [ebp-0x1f4]
                item_ctor(&as, rec(i), rec(i)->assist_next, 0, nullptr, 0);
                list_add(assist_edges, &as);
                const Pt pb = rec_pt(as.b);
                const Pt pa = rec_pt(as.a);
                grid_enter(assist_edges, pa, pb, assist_grid);
            }
        }
        alignas(alignof(void*)) u8 hits[((sizeof(jo::DynArray) > 0x20 ? sizeof(jo::DynArray) : 0x20) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x214] (x64: the larger DynArray)
        joan::orig::dynarray(static_cast<void*>(hits))->construct(8, 0x800, 0);
        auto fail_all = [&]() {
            list_dtor(hits);
            grid_dtor(assist_grid);
            list_dtor(assist_edges);
            grid_dtor(grid);
            list_dtor(edges);
            list_dtor(open_edges);
            list_dtor(open_pts);
            joan::orig::as_blob(static_cast<void*>(blob))->destruct();
            return 0;
        };
        while (list_count(open_edges) > 0) {
            joan::orig::as_dynarray(static_cast<void*>(open_edges))->go_end();  // 0x00585630
            Item cur{};                                // [ebp-0x22c]
            std::memcpy(&cur, joan::orig::arg::as<void*>(joan::orig::as_pool(static_cast<void*>(open_edges))->last_used()), sizeof cur);  // 0x004b9c30
            list_remove(open_edges, -1);
            Vtx* const a = cur.a;
            Vtx* const b = cur.b;
            Pt dir{0, 0};  // [ebp-0x234]
            {
                const Pt pa = rec_pt(a);
                const Pt pb = rec_pt(b);
                dir = vec(pb.x, pb.y, pa.x, pa.y);
            }
            auto cross_pos = [&](Vtx* r) {
                const Pt pb = rec_pt(b);
                const Pt pr = rec_pt(r);
                const Pt v = vec(pr.x, pr.y, pb.x, pb.y);
                return joan::orig::arg::as<i32>(joan::orig::as_vec2i(&dir)->cross(joan::orig::arg::i32(v.x), joan::orig::arg::i32(v.y))) > 0;  // 0x0066a400
            };
            auto score = [&](Vtx* r) {
                const Pt pb = rec_pt(b);
                const Pt pa = rec_pt(a);
                const Pt pr = rec_pt(r);
                return joan::orig::arg::as<i32>(joan::orig::point_segment_dist2_669cc0(joan::orig::arg::i32(pr.x), joan::orig::arg::i32(pr.y), joan::orig::arg::i32(pa.x), joan::orig::arg::i32(pa.y), joan::orig::arg::i32(pb.x), joan::orig::arg::i32(pb.y)));
            };
            Vtx* best = nullptr;  // [ebp-0x238]
            i32 best_d = 0;   // [ebp-0x240], read only when best != 0
            Vtx* cand_r[32];  // [ebp-0x2c0]
            i32 cand_d[32];   // [ebp-0x340]
            std::memset(cand_r, 0, sizeof cand_r);
            std::memset(cand_d, 0, sizeof cand_d);
            i32 nc = 0;       // [ebp-0x344]
            for (i32 i = 1; i <= cnt; ++i) {
                Vtx* r = rec(i);  // [ebp-0x23c]
                if (!cross_pos(r)) continue;
                i32 d = score(r);  // [ebp-0x34c]
                if (nc == 0) {
                    cand_r[nc] = r;
                    cand_d[nc] = d;
                    nc = nc + 1;
                    continue;
                }
                if (nc >= 0x20 && d >= cand_d[nc - 1]) continue;
                if (nc >= 0x20) nc = nc - 1;
                i32 pos = 0;  // [ebp-0x350]
                joan::orig::as_misc(obj(d::g_misc))->bin_search_5911a0(&d, joan::orig::arg::str(static_cast<void*>(cand_d)), joan::orig::arg::i32(nc), 4, fn::code_value(fn::cmp_int_667170), &pos);  // 0x005911a0
                joan::orig::as_misc(obj(d::g_misc))->insert_array_rec_5910f0(joan::orig::arg::str(static_cast<void*>(cand_r)), joan::orig::arg::i32(nc), static_cast<i32>(sizeof(Vtx*)), joan::orig::arg::i32(pos), &r);  // 0x005910f0
                joan::orig::as_misc(obj(d::g_misc))->insert_array_rec_5910f0(joan::orig::arg::str(static_cast<void*>(cand_d)), joan::orig::arg::i32(nc), 4, joan::orig::arg::i32(pos), &d);
                nc = nc + 1;
            }
            // edge test against the edges near the triangle's box (grid query into `hits`)
            auto blocked = [&](void* g, void* list, Vtx* r, i32 x0, i32 y0, i32 x1, i32 y1) {
                joan::orig::dynarray(static_cast<void*>(hits))->zap(0);  // 0x00584580
                joan::orig::as_sp_index(g)->query(joan::orig::arg::mptr(static_cast<void*>(hits)), joan::orig::arg::i32(x0 - 1), joan::orig::arg::i32(y0 - 1), joan::orig::arg::i32(x1 + 1), joan::orig::arg::i32(y1 + 1));  // 0x00682800
                for (i32 k = list_count(hits); k > 0; --k) {
                    const i32 idx = *reinterpret_cast<const i32*>(list_at(hits, k));  // the grid query yields edge indices
                    const u8* const ed = list_at(list, idx);
                    const Pt pr = rec_pt(r);
                    const Pt pb = rec_pt(b);
                    const Pt pa = rec_pt(a);
                    const Pt e2 = rec_pt(reinterpret_cast<const Item*>(ed)->b);
                    const Pt e1 = rec_pt(reinterpret_cast<const Item*>(ed)->a);
                    if (joan::orig::arg::as<i32>(joan::orig::tri::segment_hits(joan::orig::arg::i32(e1.x), joan::orig::arg::i32(e1.y), joan::orig::arg::i32(e2.x), joan::orig::arg::i32(e2.y), joan::orig::arg::i32(pa.x), joan::orig::arg::i32(pa.y), joan::orig::arg::i32(pb.x), joan::orig::arg::i32(pb.y), joan::orig::arg::i32(pr.x), joan::orig::arg::i32(pr.y))) != 0)
                        return true;
                }
                return false;
            };
            for (i32 i = -0x20; i <= cnt; ++i) {
                Vtx* r = nullptr;
                if (i + 0x20 < nc)
                    r = cand_r[i + 0x20];
                else if (i >= 1)
                    r = rec(i);
                else
                    continue;
                if (r == a || r == b) continue;
                if (!cross_pos(r)) continue;
                const i32 d = score(r);  // [ebp-0x354]
                if (best != nullptr && d >= best_d) continue;
                i32 k = list_count(open_edges);
                for (; k > 0; --k) {
                    const u8* const it = list_at(open_edges, k);
                    if (r == reinterpret_cast<const Item*>(it)->a || r == reinterpret_cast<const Item*>(it)->b) break;
                }
                if (k <= 0) continue;
                // box of a, b, r; the getter calls in the original's order (a, b, then the winner)
                auto pick2 = [&](bool want_less, bool y) {
                    const Pt pa = rec_pt(a);
                    const Pt pb = rec_pt(b);
                    const i32 va = y ? pa.y : pa.x;
                    const i32 vb = y ? pb.y : pb.x;
                    if (want_less ? va < vb : va > vb) return y ? rec_pt(a).y : rec_pt(a).x;
                    return y ? rec_pt(b).y : rec_pt(b).x;
                };
                auto pick_r = [&](i32 cur_v, bool want_less, bool y) {
                    const Pt pr = rec_pt(r);
                    const i32 vr = y ? pr.y : pr.x;
                    if (want_less ? cur_v < vr : cur_v > vr) return cur_v;
                    return y ? rec_pt(r).y : rec_pt(r).x;
                };
                i32 x0 = pick2(true, false);
                i32 x1 = pick2(false, false);
                i32 y0 = pick2(true, true);
                i32 y1 = pick2(false, true);
                x0 = pick_r(x0, true, false);
                x1 = pick_r(x1, false, false);
                y0 = pick_r(y0, true, true);
                y1 = pick_r(y1, false, true);
                if (blocked(grid, edges, r, x0, y0, x1, y1)) continue;
                if (blocked(assist_grid, assist_edges, r, x0, y0, x1, y1)) continue;
                best = r;
                best_d = d;
                if (i <= 0) break;
            }
            if (best == nullptr) {
                if (log != nullptr) {
                    const i32 by = rec_pt(b).y;
                    const i32 bx = rec_pt(b).x;
                    const i32 ay = rec_pt(a).y;
                    const i32 ax = rec_pt(a).x;
                    log(str(d::s_cannot_form_triangle_on_edge), ax, ay, bx, by);
                }
                const i32 by = rec_pt(b).y;
                const i32 bx = rec_pt(b).x;
                const i32 ay = rec_pt(a).y;
                const i32 ax = rec_pt(a).x;
                joan::orig::warning_box(joan::orig::arg::mptr(err), str(d::s_cannot_form_triangle_on_edge), ax, ay, bx, by);
                return fail_all();
            }
            if (log != nullptr) {
                const i32 ry = rec_pt(best).y;
                const i32 rx = rec_pt(best).x;
                const i32 by = rec_pt(b).y;
                const i32 bx = rec_pt(b).x;
                const i32 ay = rec_pt(a).y;
                const i32 ax = rec_pt(a).x;
                log(str(d::s_resolving_line_d_d_d_d_resul), ax, ay, bx, by, rx, ry);
            }
            jo::PfdPoint* tri_pts[3] = {a->point, b->point, best->point};  // [ebp-0x388] (Zone::set_points copies 3 pointers)
            const i32 tidx = joan::orig::arg::as<i32>(joan::orig::as_block_array(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->add(0));  // 0x00637be0
            u8* const tri = joan::orig::arg::as<u8*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(tidx)));
            joan::orig::as_zone(tri)->set_points(joan::orig::arg::mptr(static_cast<void*>(tri_pts)), 3, joan::orig::arg::u16(kind));  // 0x00663cb0
            jo::as_terrain_block(tri)->index = tidx;
            jo::view<void*>(jo::as_terrain_block(tri)->space) = this;
            jo::as_terrain_block(tri)->u32_4 = 0;
            if (static_cast<i16>(cur.c) == 0) {
                list_add(edges, &cur);
                {
                    const Pt pb = rec_pt(b);
                    const Pt pa = rec_pt(a);
                    grid_enter(edges, pa, pb, grid);
                }
                if (cur.d != nullptr) joan::orig::as_zone(tri)->link(1, joan::orig::arg::mptr(cur.d), joan::orig::arg::i32(sx16(cur.e)));  // 0x00664130
            }
            // edge (b, best)
            i32 k = list_count(open_edges);
            for (; k > 0; --k) {
                const u8* const it = list_at(open_edges, k);
                if (reinterpret_cast<const Item*>(it)->a != b || reinterpret_cast<const Item*>(it)->b != best) continue;
                if (reinterpret_cast<const Item*>(it)->d != nullptr) joan::orig::as_zone(tri)->link(2, joan::orig::arg::mptr(reinterpret_cast<const Item*>(it)->d), joan::orig::arg::i32(sx16(reinterpret_cast<const Item*>(it)->e)));
                break;
            }
            if (k > 0) {
                list_remove(open_edges, k);
            } else {
                Item it{};  // [ebp-0x3a8]
                item_ctor(&it, best, b, 0, tri, 2);
                list_add(open_edges, &it);
            }
            if (best != b->next) {
                Item it{};  // [ebp-0x3b8]
                item_ctor(&it, b, best, 0, nullptr, 0);
                list_add(edges, &it);
                const Pt p2 = rec_pt(it.b);
                const Pt p1 = rec_pt(it.a);
                grid_enter(edges, p1, p2, grid);
            }
            // edge (best, a)
            k = list_count(open_edges);
            for (; k > 0; --k) {
                const u8* const it = list_at(open_edges, k);
                if (reinterpret_cast<const Item*>(it)->a != best || reinterpret_cast<const Item*>(it)->b != a) continue;
                if (reinterpret_cast<const Item*>(it)->d != nullptr) joan::orig::as_zone(tri)->link(3, joan::orig::arg::mptr(reinterpret_cast<const Item*>(it)->d), joan::orig::arg::i32(sx16(reinterpret_cast<const Item*>(it)->e)));
                break;
            }
            if (k > 0) {
                list_remove(open_edges, k);
            } else {
                Item it{};  // [ebp-0x3cc]
                item_ctor(&it, a, best, 0, tri, 3);
                list_add(open_edges, &it);
            }
            if (best != a->prev) {
                Item it{};  // [ebp-0x3dc]
                item_ctor(&it, best, a, 0, nullptr, 0);
                list_add(edges, &it);
                const Pt p2 = rec_pt(it.b);
                const Pt p1 = rec_pt(it.a);
                grid_enter(edges, p1, p2, grid);
            }
        }
        if (tri_before > 0) joan::orig::as_path_space(this)->link_edges(joan::orig::arg::i32(tri_before));  // 0x0066b7c0
        if (log != nullptr) {
            const i32 tris = wsub(count_of(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones)), tri_before);
            const i32 nl = count_of(lines);
            const i32 np = count_of(jo::view<void*>(jo::self<jo::PathSpace>(this)->points));
            log(str(d::s_Normal_finish_d_points_d_pat), np, nl, tris);
        }
        list_dtor(hits);
        grid_dtor(assist_grid);
        list_dtor(assist_edges);
        grid_dtor(grid);
        list_dtor(edges);
        list_dtor(open_edges);
        list_dtor(open_pts);
        joan::orig::as_blob(static_cast<void*>(blob))->destruct();
        return 1;
    }
};

#define FIX(name, address, fn) JOAN_REPLACE("fix/uninit", name, address, fn)
// fix/uninit (dead code): maximum edge length [ebp-0x1bc] starts at 0
FIX("PathSpace::gen_path_triangles_unassisted", 0x00667190, &H::mesh_build);
#undef FIX

}  // namespace game_b115_08
}  // namespace

// ---------------------------------------- from modules/game_b107_04.cpp (game/b107_04)
namespace {
namespace game_b107_04 {

using u32 = std::uint32_t;

// two's complement wrap like the original's lea / add
using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;

JOAN_COUNTER(c_merge_pairs, "PolySet::merge_pairs");

// ---- 0x0066a640 helpers ----
struct XY {
    i32 x, y;
};
struct Vec {
    float x, y, z;
};
static_assert(sizeof(XY) == 8 && sizeof(Vec) == 12);

i32 list_count(void* list) { return joan::orig::arg::as<i32>(joan::orig::as_path_space(list)->get_2c()); }           // 0x0066b7a0
u8* list_get(void* list, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_billboard(list)->item(joan::orig::arg::i32(i))); }  // 0x00637da0
i32 list_add(void* list) { return joan::orig::arg::as<i32>(joan::orig::as_block_array(list)->add(0)); }          // 0x00637be0
i32 edge_of(void* poly, void* other) { return joan::orig::arg::as<i32>(joan::orig::as_zone(poly)->link_index(other)); }  // 0x006640d0
void set_edge(void* poly, i32 edge, void* other, i32 back) {
    joan::orig::as_zone(poly)->link(joan::orig::arg::i32(edge), joan::orig::arg::mptr(other), joan::orig::arg::i32(back));  // 0x00664130
}
void poly_init(void* poly, void* verts, i32 n, u16 kind) {
    joan::orig::as_zone(poly)->set_points(joan::orig::arg::mptr(verts), joan::orig::arg::i32(n), joan::orig::arg::u16(static_cast<u32>(kind)));  // 0x00663cb0
}
void poly_clear(void* poly) { joan::orig::as_zone(poly)->free_points(); }  // 0x00663f70

i32* vtx(void* poly, i32 i) { return jo::view<i32**>(jo::as_terrain_block(poly)->points)[i]; }
u8* nbr(void* poly, i32 i) { return jo::view<u8**>(jo::as_terrain_block(poly)->links)[i]; }
XY xy(const i32* p) { return XY{p[0], p[1]}; }
float isub(const i32* a, const i32* b, int c) {  // fild a; fisub b; fstp float (PC24)
    return static_cast<float>(static_cast<double>(a[c]) - static_cast<double>(b[c]));
}
XY edge2d(XY a, XY b) {  // 0x006639e0 (cdecl, out first)
    XY tmp{0, 0};
    const XY* const r = joan::orig::arg::as<XY*>(joan::orig::p2::sub(&tmp, a, b));
    return *r;
}
i32 cross2d(XY& a, XY b) { return joan::orig::as_vec2i(&a)->cross(b.x, b.y); }  // 0x0066a400
double crt_fabs(float v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(v)))); }

// cross products of the edge e with (p - poly vertex i) until one is not ~0, then normalised.
Vec face_normal(void* poly, i32 edge, const i32* p, const i32* q) {
    const double eps = global<double>(d::dbl_0_00001);
    Vec n{0.0f, 0.0f, 0.0f};
    Vec e{isub(q, p, 0), isub(q, p, 1), isub(q, p, 2)};
    for (i32 i = 0; i < jo::as_terrain_block(poly)->count; ++i) {
        if (i == edge - 1 || i == edge || wadd(i, jo::as_terrain_block(poly)->count) == edge) continue;
        const i32* const v = vtx(poly, i);
        Vec d{isub(p, v, 0), isub(p, v, 1), isub(p, v, 2)};
        import_call<Vec*>(d::iat_v3d_87_cross_product, &n, static_cast<const Vec*>(&d), static_cast<const Vec*>(&e));  // cross
        if (crt_fabs(n.x) >= eps || crt_fabs(n.y) >= eps || crt_fabs(n.z) >= eps) {
            import_call<Vec*>(d::iat_v3d_87_vector_normalize_1, &n);  // normalize in place
            break;
        }
    }
    return n;
}

struct H {
    // 0x0066a640
    i32 merge_pairs(i32 check_normals) {
        c_merge_pairs.hit();
        joan::orig::as_path_space(this)->flush_bvh();  // 0x00674590
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) == nullptr) return 0;
        i32 merged = 0;
        for (i32 a = 1; a <= list_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones)); ++a) {
            u8* const A = list_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), a);
            if (jo::view<u32>(jo::as_terrain_block(A)->index) == 0) continue;
            const i32 n0 = jo::as_terrain_block(A)->count;
            for (i32 j = 1; j <= jo::as_terrain_block(A)->count; ++j) {
                u8* const B = nbr(A, j - 1);
                if (B == nullptr) continue;
                const i32 n1 = jo::as_terrain_block(B)->count;
                const i32 k = edge_of(B, A);
                if (k == 0) continue;
                if (wadd(wadd(jo::as_terrain_block(A)->count, jo::as_terrain_block(B)->count), -2) > 0x20) continue;
                if (static_cast<i32>(static_cast<i16>(jo::as_terrain_block(A)->kind)) != static_cast<i32>(static_cast<i16>(jo::as_terrain_block(B)->kind)) ||
                    jo::as_terrain_block(A)->u32_4 != jo::as_terrain_block(B)->u32_4)
                    continue;
                const i32* const p38 = vtx(A, wadd(wadd(j, jo::as_terrain_block(A)->count), -2) % jo::as_terrain_block(A)->count);
                const i32* const p3c = vtx(A, j - 1);
                const i32* const p40 = vtx(A, j % jo::as_terrain_block(A)->count);
                const i32* const p44 = vtx(A, wadd(j, 1) % jo::as_terrain_block(A)->count);
                const i32* const p48 = vtx(B, wadd(wadd(k, jo::as_terrain_block(B)->count), -2) % jo::as_terrain_block(B)->count);
                const i32* const p4c = vtx(B, k - 1);
                const i32* const p50 = vtx(B, k % jo::as_terrain_block(B)->count);
                const i32* const p54 = vtx(B, wadd(k, 1) % jo::as_terrain_block(B)->count);
                XY s5c = edge2d(xy(p3c), xy(p38));
                const XY s64 = edge2d(xy(p54), xy(p3c));
                XY s6c = edge2d(xy(p4c), xy(p48));
                const XY s74 = edge2d(xy(p44), xy(p4c));
                if (cross2d(s5c, s64) <= 0) continue;
                if (cross2d(s6c, s74) <= 0) continue;
                if (check_normals != 0) {
                    const Vec na = face_normal(A, j, p3c, p40);
                    const Vec nb = face_normal(B, k, p4c, p50);
                    const double tol = global<double>(d::dbl_0_01);
                    const float dx = na.x - nb.x;
                    if (crt_fabs(dx) > tol) continue;
                    const float dy = na.y - nb.y;
                    if (crt_fabs(dy) > tol) continue;
                    const float dz = na.z - nb.z;
                    if (crt_fabs(dz) > tol) continue;
                }
                merge_into_new(a, A, B, j, k, n0, n1);
                merged = 1;
                break;
            }
        }
        if (merged == 0) return merged;

        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::BlkArr)));  // 0x30 on x86; operator new
        void* list2 = nullptr;
        if (mem != nullptr) list2 = joan::orig::as_blk_arr(mem)->construct(static_cast<u32>(sizeof(jo::PfdZone)), 0x800u);  // 0x00637aa0 (PfdZone records: 0x74 bytes on x86)
        for (i32 a = 1; a <= list_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones)); ++a) {
            u8* const A = list_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), a);
            if (jo::view<u32>(jo::as_terrain_block(A)->index) == 0) continue;
            const i32 idx = list_add(list2);
            u8* const N = list_get(list2, idx);
            poly_init(N, jo::as_terrain_block(A)->points, jo::as_terrain_block(A)->count, jo::as_terrain_block(A)->kind);
            jo::as_terrain_block(N)->index = idx;
            jo::view<void*>(jo::as_terrain_block(N)->space) = this;
            jo::as_terrain_block(N)->u32_4 = jo::as_terrain_block(A)->u32_4;
            jo::as_terrain_block(A)->index = idx;
        }
        for (i32 a = 1; a <= list_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones)); ++a) {
            u8* const A = list_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), a);
            if (jo::view<u32>(jo::as_terrain_block(A)->index) == 0) continue;
            u8* const N = list_get(list2, jo::as_terrain_block(A)->index);
            for (i32 m = 1; m <= jo::as_terrain_block(A)->count; ++m) {
                u8* const X = nbr(A, m - 1);
                if (X == nullptr) continue;
                const i32 back = edge_of(X, A);
                u8* const NX = list_get(list2, jo::as_terrain_block(X)->index);
                set_edge(N, m, NX, back);
            }
        }
        for (i32 a = 1; a <= list_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones)); ++a) {
            u8* const A = list_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), a);
            if (jo::view<u32>(jo::as_terrain_block(A)->index) == 0) continue;
            poly_clear(A);
        }
        void* const old = jo::view<void*>(jo::self<jo::PathSpace>(this)->zones);
        if (old != nullptr) joan::orig::as_blk_arr(old)->destroy(1u);  // 0x006651e0 scalar deleting dtor
        jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) = list2;
        return merged;
    }

    // merge block 0x0066aca0..0x0066b002
    void merge_into_new(i32 a, u8* A, u8* B, i32 j, i32 k, i32 n0, i32 n1) {
        // slots[0..31] = vertex buffer [ebp-0x148], slots[32] = [ebp-0xc8] (new polygon),
        // slots[33] = [ebp-0xc4] (its index): the fill loop may run into slot 32 like the original.
        std::uintptr_t slots[34] = {};  // pointer-sized (the vertex pointers and the new polygon)
        slots[33] = static_cast<std::uintptr_t>(list_add(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones)));
        slots[32] = reinterpret_cast<std::uintptr_t>(list_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), static_cast<i32>(slots[33])));
        const i32 cnt = wadd(wadd(jo::as_terrain_block(A)->count, jo::as_terrain_block(B)->count), -2);
        i32 i = 0;
        for (; i < n0; ++i) slots[i] = reinterpret_cast<std::uintptr_t>(vtx(A, wadd(j, i) % n0));
        for (; i < cnt; ++i) slots[i] = reinterpret_cast<std::uintptr_t>(vtx(B, wsub(wadd(k, i), wsub(n0, 1)) % n1));
        poly_init(reinterpret_cast<void*>(slots[32]), &slots[0], cnt, jo::as_terrain_block(A)->kind);
        jo::view<u32>(jo::as_terrain_block(reinterpret_cast<void*>(slots[32]))->index) = static_cast<u32>(slots[33]);
        jo::view<void*>(jo::as_terrain_block(reinterpret_cast<void*>(slots[32]))->space) = this;
        jo::as_terrain_block(reinterpret_cast<void*>(slots[32]))->u32_4 = jo::as_terrain_block(A)->u32_4;
        i32 m = 1;
        for (; m < n0; ++m) {
            const i32 s = wadd(wadd(j, m), -1) % n0 + 1;
            u8* const X = nbr(A, s - 1);
            if (X == nullptr) continue;
            const i32 back = edge_of(X, A);
            set_edge(reinterpret_cast<void*>(slots[32]), m, X, back);
        }
        for (; m <= cnt; ++m) {
            const i32 s = wsub(wadd(k, m), n0) % n1 + 1;
            u8* const X = nbr(B, s - 1);
            if (X == nullptr) continue;
            const i32 back = edge_of(X, B);
            set_edge(reinterpret_cast<void*>(slots[32]), m, X, back);
        }
        for (i32 q = 1; q <= list_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones)); ++q) {
            if (q == a || q == static_cast<i32>(slots[33])) continue;
            u8* const C = list_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), q);
            if (jo::view<u32>(jo::as_terrain_block(C)->index) == 0 || C == B) continue;
            i32 e = edge_of(C, A);
            if (e != 0) set_edge(C, e, reinterpret_cast<void*>(slots[32]), 0);
            e = edge_of(C, B);
            if (e != 0) set_edge(C, e, reinterpret_cast<void*>(slots[32]), 0);
        }
        poly_clear(A);
        jo::view<u32>(jo::as_terrain_block(A)->index) = 0;
        poly_clear(B);
        jo::view<u32>(jo::as_terrain_block(B)->index) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_04", name, addr, fn)
B("PathSpace::merge_convex_polygons", 0x0066a640, &H::merge_pairs);
#undef B

}  // namespace game_b107_04
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04_2 {

// Integer point built with 0x004eeb80 (x, y) on a stack local, then copied to `out`.
void* make_point(void* out, i32 x, i32 y) {
    i32 local[2];
    const i32* const p = joan::orig::arg::as<const i32*>(joan::orig::as_point(local)->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y)));  // 0x004eeb80
    const i32 px = p[0];
    const i32 py = p[1];
    static_cast<i32*>(out)[0] = px;
    static_cast<i32*>(out)[1] = py;
    return out;
}

// ---------------------------------------------------------------------------- 0x00663a30
static void* JOAN_CDECL point_scale(void* out, i32 ax, i32 ay, float s) {
    const Fpu fpu{true};
    const i32 y = joan::hook::x87::ftol(fpu.mul(static_cast<double>(ay), static_cast<double>(s)));
    const i32 x = joan::hook::x87::ftol(fpu.mul(static_cast<double>(ax), static_cast<double>(s)));
    return make_point(out, x, y);
}

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("operator*", 0x00663a30, &point_scale);
#undef B93

}  // namespace game_b93_04_2
}  // namespace

// ---------------------------------------- from modules/game_light1.cpp (game/light1)
namespace {
namespace game_light1_2 {

struct P2 {
    std::int32_t x, y;
};

// 0x006639e0 P2 operator-(a, b) into *out (cdecl).
P2* JOAN_CDECL p2_sub(P2* out, P2 a, P2 b) {
    P2 t;  // ebp-0x8
    const P2* const r = joan::orig::arg::as<P2*>(joan::orig::as_point(&t)->set(joan::orig::arg::u32(static_cast<std::int32_t>(static_cast<std::uint32_t>(a.x) - static_cast<std::uint32_t>(b.x))), joan::orig::arg::u32(static_cast<std::int32_t>(static_cast<std::uint32_t>(a.y) - static_cast<std::uint32_t>(b.y)))));  // 0x004eeb80
    out->x = r->x;
    out->y = r->y;
    return out;
}

#define L1(name, addr, fn) JOAN_REPLACE("game/light1", name, addr, fn)
L1("operator-", 0x006639e0, &p2_sub);
#undef L1

}  // namespace game_light1_2
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04_2 {

struct H {
    // 0x00663c50
    H* ctor_663c50() {
        joan::orig::as_triple(jo::self<jo::PfdZone>(this)->bbox_min)->construct_default();     // 0x0066a480
        joan::orig::as_triple(jo::self<jo::PfdZone>(this)->bbox_max)->construct_default();     // 0x0066a480
        joan::orig::as_path_node(&jo::self<jo::PfdZone>(this)->search_stamp)->construct();  // 0x00663b90
        std::memset(static_cast<void*>(this), 0, offsetof(jo::PfdZone, children));          // memset (0x40 on x86)
        return this;
    }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("PathPolygon::PathPolygon", 0x00663c50, &H::ctor_663c50);
#undef B83_04

}  // namespace game_b83_04_2
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04_3 {

using joan::call::cdecl_call;

struct H {
    void free_663f70() {
        void* const a = jo::self<jo::TerrainBlock>(this)->points;
        joan::crt::operator_delete(joan::orig::arg::mptr(a));  // operator delete
        jo::self<jo::TerrainBlock>(this)->points = nullptr;
        void* const b = jo::self<jo::TerrainBlock>(this)->links;
        joan::crt::operator_delete(joan::orig::arg::mptr(b));
        jo::self<jo::TerrainBlock>(this)->links = nullptr;
        joan::orig::as_path_node(&jo::self<jo::TerrainBlock>(this)->buffer)->free_buffer();  // 0x00663c00
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("PathPolygon::deinit", 0x00663f70, &H::free_663f70);
#undef M

}  // namespace game_b84_04_3
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04_2 {

struct H {
    // 0x00663ff0
    std::int32_t contains_663ff0(void* p) {
        for (std::int32_t i = 1; i <= jo::self<jo::TerrainBlock>(this)->count; ++i) {
            void* const e = jo::self<jo::TerrainBlock>(this)->points[i - 1].item;
            if (e == p) return 1;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_triple(jo::self<jo::TerrainBlock>(this)->points[i - 1].item)->ret0_66a5e0(joan::orig::arg::mptr(p))) != 0) return 1;  // 0x0066a5e0
        }
        return 0;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("PathPolygon::is_depend", 0x00663ff0, &H::contains_663ff0);
#undef B91

}  // namespace game_b91_04_2
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04_3 {

JOAN_COUNTER(c_index_of, "IntArray::index_of");

struct H {
    // ------------------------------------------------------------------ 0x006640d0
    i32 index_of(void* x) {
        c_index_of.hit();
        for (i32 i = 1; i <= jo::self<jo::TerrainBlock>(this)->count; ++i) {
            if (static_cast<void* const*>(jo::self<jo::TerrainBlock>(this)->links)[i - 1] == x) return i;
        }
        return 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("PathPolygon::find_adj_polygon_edge", 0x006640d0, &H::index_of);
#undef B93

}  // namespace game_b93_04_3
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
namespace {
namespace game_b92_05_2 {

JOAN_COUNTER(c_intersect, "line_intersect_terms");

// 0x00664b70
std::uint32_t JOAN_CDECL line_intersect_terms(std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t d, std::int32_t e,
                                           std::int32_t f, std::int32_t g, std::int32_t h, std::int32_t* p1, std::int32_t* p2,
                                           std::int32_t* p3) {
    c_intersect.hit();
    using U = std::uint32_t;
    *p1 = static_cast<std::int32_t>(U(d - b) * U(g - e) - U(c - a) * U(h - f));
    *p2 = static_cast<std::int32_t>(U(f - b) * U(g - e) - U(e - a) * U(h - f));
    *p3 = static_cast<std::int32_t>(U(c - a) * U(f - b) - U(d - b) * U(e - a));
    if (*p1 < 0) {
        *p1 = static_cast<std::int32_t>(0u - U(*p1));
        *p2 = static_cast<std::int32_t>(0u - U(*p2));
        *p3 = static_cast<std::int32_t>(0u - U(*p3));
    }
    return *p1 != 0 ? 1u : 0u;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("SPointInt::line_segment_intersection", 0x00664b70, &line_intersect_terms);
#undef B

}  // namespace game_b92_05_2
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04_3 {

JOAN_COUNTER(c_tri, "tri_normal_2d");

struct H {
    // 0x00665940
    std::int32_t index_of(jo::Triple* v) {
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->points) == nullptr) return 0;
        for (std::int32_t i = 1; i <= joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->points))->get_2c()); ++i)  // 0x0066b7a0
            if (joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->points))->item(joan::orig::arg::i32(i))) == v) return i;          // 0x00637da0
        return 0;
    }
};

// 0x00664c60
std::int32_t JOAN_CDECL tri_normal_2d(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz,
                                   float dx, float dy, float dz, float* o1, float* o2, float* o3) {
    c_tri.hit();
    (void)az;
    (void)bz;
    (void)cz;
    (void)dz;
    *o1 = (by - ay) * (dx - cx) - (bx - ax) * (dy - cy);
    *o2 = (cy - ay) * (dx - cx) - (cx - ax) * (dy - cy);
    *o3 = (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
    const float zero = global<float>(d::k_9cf5c4);
    if (!(*o1 >= zero)) {  // < 0 or NaN
        *o1 = -*o1;
        *o2 = -*o2;
        *o3 = -*o3;
    }
    if (*o1 == zero || *o1 != *o1) return 0;  // equal or unordered
    return 1;
}

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("SPointInt::line_segment_intersection2", 0x00664c60, &tri_normal_2d);
B91("PathSpace::find_point", 0x00665940, &H::index_of);
#undef B91

}  // namespace game_b91_04_3
}  // namespace

// ---------------------------------------- from modules/game_b88_04.cpp (game/b88_04)
namespace {
namespace game_b88_04 {

JOAN_COUNTER(c_seg_dist, "point_segment_dist2");

// 0x00664d70
float JOAN_CDECL point_segment_dist2(float x0, float y0, float x1, float y1, float px, float py) {
    c_seg_dist.hit();
    const float ax = x1 - x0;
    const float ay = y1 - y0;
    const float sx = ax * ax;
    const float sy = ay * ay;
    const float len2 = sx + sy;  // ebp-0x4
    const float dx = px - x0;
    const float dy = py - y0;
    const float tx = dx * ax;
    const float ty = dy * ay;
    const float dot = tx + ty;  // ebp-0x8
    // fcomp 0.0 / test ah,1 jne: endpoint path when dot < 0 or unordered.
    if (dot >= global<float>(d::k_9cf5c4)) {
        // fcomp len2 / test ah,0x41 je: endpoint path when dot > len2.
        if (!(dot > len2)) {
            const float c1 = dx * ay;
            const float c2 = dy * ax;
            const float c = c1 - c2;  // ebp-0xc
            const float cc = c * c;
            return cc / len2;
        }
    }
    const float ex = px - x0;
    const float ey = py - y0;
    const float ex2 = ex * ex;
    const float ey2 = ey * ey;
    const float d0 = ex2 + ey2;  // ebp-0x10
    const float fx = px - x1;
    const float fy = py - y1;
    const float fx2 = fx * fx;
    const float fy2 = fy * fy;
    const float d1 = fx2 + fy2;  // ebp-0x14
    // fcomp d1 / test ah,1 je: d1 unless d0 < d1 or unordered.
    return !(d0 >= d1) ? d0 : d1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b88_04", name, addr, fn)
B("SPointInt::rect_dist2_to_line_segment", 0x00664d70, &point_segment_dist2);
#undef B

}  // namespace game_b88_04
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14_2 {

struct H {
    // 0x00666d80 PfdData::free_10() (ret 0).
    std::uintptr_t free_10() {  // EAX: reset()'s result / this (pointer-wide on x64)
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->groups) == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_blk_arr(jo::view<void*>(jo::self<jo::PathSpace>(this)->groups))->reset());  // 0x00637bb0
    }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("PathSpace::clear_obstacle_polygon", 0x00666d80, &H::free_10);
#undef B82_14

}  // namespace game_b82_14_2
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14_2 {

using u32 = std::uint32_t;

struct H {
    std::uintptr_t self_addr() { return reinterpret_cast<std::uintptr_t>(this); }

    using Slot = jo::BlkArr* jo::PathSpace::*;
    std::uintptr_t list_item(Slot m, i32 i) {  // EAX: this / the count / the removed item (pointer-wide on x64)
        if (jo::self<jo::PathSpace>(this)->*m == nullptr) return self_addr();
        if (!(i > 0)) {
            const u32 n = joan::orig::arg::as<u32>(joan::orig::as_path_space(jo::self<jo::PathSpace>(this)->*m)->get_2c());  // 0x0066b7a0
            if (i > static_cast<i32>(n)) return n;
        }
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_list(jo::self<jo::PathSpace>(this)->*m)->remove_at(joan::orig::arg::i32(i)));  // 0x00637e20
    }
    std::uintptr_t list_item_at4(i32 i) { return list_item(&jo::PathSpace::pairs4, i); }    // 0x00665ed0
    std::uintptr_t list_item_atc(i32 i) { return list_item(&jo::PathSpace::links, i); }    // 0x00666af0
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("PathSpace::del_path_line", 0x00665ed0, &H::list_item_at4);
B103("PathSpace::del_sub_path_line", 0x00666af0, &H::list_item_atc);
#undef B103

}  // namespace game_b103_14_2
}  // namespace

// ---------------------------------------- from modules/game_b102_13.cpp (game/b102_13)
namespace {
namespace game_b102_13_2 {

using u32 = std::uint32_t;

// Block array helpers (uti3d blkarrb).
i32 arr_count(void* a) { return joan::orig::arg::as<i32>(joan::orig::as_path_space(a)->get_2c()); }          // 0x0066b7a0
u8* arr_get(void* a, i32 n) { return joan::orig::arg::as<u8*>(joan::orig::as_billboard(a)->item(joan::orig::arg::i32(n))); }  // 0x00637da0
const u32* make_pt(u32* pt, u32 x, u32 y) { return joan::orig::arg::as<const u32*>(joan::orig::as_point(pt)->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y))); }  // 0x004eeb80

JOAN_COUNTER(c_regions, "Regions::b102_13");

struct H {
    // ------------------------------------------------------------------ 0x00665a00
    i32 get_point(i32 n, u32* ox, u32* oy, u32* oz) {
        c_regions.hit();
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->points) == nullptr || n <= 0 || n > arr_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->points))) return 0;
        u8* const e = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->points), n);
        u32 tmp[2];
        const u32* const p = joan::orig::arg::as<const u32*>(joan::orig::as_vec2i(e)->copy_to(joan::orig::arg::mptr(tmp)));  // 0x0066a4f0
        const u32 px = p[0], py = p[1];
        *ox = px;
        *oy = py;
        *oz = jo::as_triple(e)->c;
        return n;
    }

    // ------------------------------------------------------------------ 0x00665aa0
    std::uintptr_t set_point(i32 n, u32 x, u32 y, u32 r) {  // EAX: this / the count / the entry (pointer-wide on x64)
        c_regions.hit();
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->points) == nullptr || n <= 0) return reinterpret_cast<std::uintptr_t>(this);
        const i32 cnt = arr_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->points));
        if (n > cnt) return static_cast<u32>(cnt);
        u32 pt[2];
        const u32* const p = make_pt(pt, x, y);
        const u32 px = p[0], py = p[1];
        u8* const e = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->points), n);
        return joan::orig::arg::as<u32>(joan::orig::as_triple(e)->set(joan::orig::arg::i32(px), joan::orig::arg::i32(py), joan::orig::arg::i32(r)));  // 0x0066a5a0
    }

    // ------------------------------------------------------------------ 0x00666010 / 0x00666630
    i32 get_pair_pts(jo::BlkArr* jo::PathSpace::* off, i32 n, u32* o1, u32* o2, u32* o3, u32* o4, u32* o5, u32* o6) {
        c_regions.hit();
        if (jo::self<jo::PathSpace>(this)->*off == nullptr || n <= 0 || n > arr_count(jo::self<jo::PathSpace>(this)->*off)) return 0;
        u8* const e = arr_get(jo::self<jo::PathSpace>(this)->*off, n);
        *o1 = jo::as_pair(e)->a->a;
        *o2 = jo::as_pair(e)->a->b;
        *o3 = jo::as_pair(e)->a->c;
        *o4 = jo::as_pair(e)->b->a;
        *o5 = jo::as_pair(e)->b->b;
        *o6 = jo::as_pair(e)->b->c;
        return n;
    }
    i32 get_pair_pts_4(i32 n, u32* o1, u32* o2, u32* o3, u32* o4, u32* o5, u32* o6) {
        return get_pair_pts(&jo::PathSpace::pairs4, n, o1, o2, o3, o4, o5, o6);
    }
    i32 get_pair_pts_8(i32 n, u32* o1, u32* o2, u32* o3, u32* o4, u32* o5, u32* o6) {
        return get_pair_pts(&jo::PathSpace::pairs8, n, o1, o2, o3, o4, o5, o6);
    }

    // ------------------------------------------------------------------ 0x006660e0 / 0x00666700
    std::uintptr_t swap_pair(jo::BlkArr* jo::PathSpace::* off, i32 n) {  // EAX: this / the count / the entry (pointer-wide on x64)
        c_regions.hit();
        if (jo::self<jo::PathSpace>(this)->*off == nullptr || n <= 0) return reinterpret_cast<std::uintptr_t>(this);
        const i32 cnt = arr_count(jo::self<jo::PathSpace>(this)->*off);
        if (n > cnt) return static_cast<u32>(cnt);
        u8* const e = arr_get(jo::self<jo::PathSpace>(this)->*off, n);
        jo::Triple* const t = jo::as_pair(e)->a;
        jo::as_pair(e)->a = jo::as_pair(e)->b;
        jo::as_pair(e)->b = t;
        return reinterpret_cast<std::uintptr_t>(e);
    }
    std::uintptr_t swap_pair_4(i32 n) { return swap_pair(&jo::PathSpace::pairs4, n); }
    std::uintptr_t swap_pair_8(i32 n) { return swap_pair(&jo::PathSpace::pairs8, n); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_13", name, addr, fn)
B("PathSpace::get_point", 0x00665a00, &H::get_point);
B("PathSpace::move_point", 0x00665aa0, &H::set_point);
B("PathSpace::get_path_line_pos", 0x00666010, &H::get_pair_pts_4);
B("PathSpace::reverse_path_line", 0x006660e0, &H::swap_pair_4);
B("PathSpace::get_assist_line_pos", 0x00666630, &H::get_pair_pts_8);
B("PathSpace::reverse_assist_line", 0x00666700, &H::swap_pair_8);
#undef B

}  // namespace game_b102_13_2
}  // namespace

// ---------------------------------------- from modules/game_b104_12.cpp (game/b104_12)
namespace {
namespace game_b104_12 {

JOAN_COUNTER(c_seg_hits, "Tri::segment_hits");


Pt* pt_diff(Pt* out, i32 x1, i32 y1, i32 x2, i32 y2) {  // 0x006639e0
    return joan::orig::arg::as<Pt*>(joan::orig::p2::sub(out, P2{x1, y1}, P2{x2, y2}));
}
i32 pt_cross(const Pt* p, i32 x, i32 y) { return joan::orig::arg::as<i32>(joan::orig::as_vec2i(p)->cross(joan::orig::arg::i32(x), joan::orig::arg::i32(y))); }  // 0x0066a400
i32 ratio_cmp(i32 a, i32 b, i32 c, i32 d) { return joan::orig::arg::as<i32>(joan::orig::rational_compare(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d))); }

i32 JOAN_CDECL segment_hits(i32 x0, i32 y0, i32 x1, i32 y1, i32 ax, i32 ay, i32 bx, i32 by, i32 cx,
                         i32 cy) {
    c_seg_hits.hit();
    i32 lo = x0 < x1 ? x0 : x1;
    if (lo > ax && lo > bx && lo > cx) return 0;
    i32 hi = x0 > x1 ? x0 : x1;
    if (hi < ax && hi < bx && hi < cx) return 0;
    lo = y0 < y1 ? y0 : y1;
    if (lo > ay && lo > by && lo > cy) return 0;
    hi = y0 > y1 ? y0 : y1;
    if (hi < ay && hi < by && hi < cy) return 0;

    i32 n_lo = 0, d_lo = 1, n_hi = 1, d_hi = 1;
    Pt args[3] = {{ax, ay}, {bx, by}, {cx, cy}};  // the original's by-value argument slots
    for (i32 e = 0; e < 3; ++e) {
        Pt p, q;
        joan::orig::as_point(&p)->construct();  // 0x004eea60 (no-op ctor)
        joan::orig::as_point(&q)->construct();
        switch (e) {
            case 0:
                joan::orig::as_vec2i(&p)->assign(&args[0]);  // 0x0066a380 copy
                joan::orig::as_vec2i(&q)->assign(&args[1]);
                break;
            case 1:
                joan::orig::as_vec2i(&p)->assign(&args[1]);
                joan::orig::as_vec2i(&q)->assign(&args[2]);
                break;
            case 2:
                joan::orig::as_vec2i(&p)->assign(&args[2]);
                joan::orig::as_vec2i(&q)->assign(&args[0]);
                break;
            default: break;
        }
        Pt t50, t48, t60, t58;
        const Pt* r = pt_diff(&t50, x0, y0, q.x, q.y);
        const Pt r0 = *r;
        const i32 s0 = pt_cross(pt_diff(&t48, q.x, q.y, p.x, p.y), r0.x, r0.y);
        r = pt_diff(&t60, x1, y1, q.x, q.y);
        const Pt r1 = *r;
        const i32 s1 = pt_cross(pt_diff(&t58, q.x, q.y, p.x, p.y), r1.x, r1.y);
        if (s0 <= 0 && s1 <= 0) return 0;
        i32 o30, o34, o38;
        if (s0 < 0 && s1 > 0) {
            if (joan::orig::arg::as<i32>(joan::orig::line_intersect_terms(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(p.x), joan::orig::arg::i32(p.y), joan::orig::arg::i32(q.x), joan::orig::arg::i32(q.y), &o30, &o34, &o38)) != 0) {  // 0x00664b70
                if (ratio_cmp(o34, o30, n_lo, d_lo) > 0) {
                    n_lo = o34;
                    d_lo = o30;
                }
            }
        }
        if (s1 < 0 && s0 > 0) {
            if (joan::orig::arg::as<i32>(joan::orig::line_intersect_terms(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(p.x), joan::orig::arg::i32(p.y), joan::orig::arg::i32(q.x), joan::orig::arg::i32(q.y), &o30, &o34, &o38)) != 0) {
                if (ratio_cmp(o34, o30, n_hi, d_hi) < 0) {
                    n_hi = o34;
                    d_hi = o30;
                }
            }
        }
    }
    return ratio_cmp(n_lo, d_lo, n_hi, d_hi) < 0 ? 1 : 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b104_12", name, addr, fn)
B("line_segment_overlap_triangle", 0x00669e50, &segment_hits);
#undef B

}  // namespace game_b104_12
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14_2 {

using u32 = std::uint32_t;

// Block array (0x00637aa0 ctor): +0x2c count.
u8* arr_get(void* arr, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_billboard(arr)->item(joan::orig::arg::i32(i))); }        // 0x00637da0
i32 arr_count(void* arr) { return joan::orig::arg::as<i32>(joan::orig::as_path_space(arr)->get_2c()); }                // 0x0066b7a0

JOAN_COUNTER(c_link_ends, "LinkSet::link_ends");
JOAN_COUNTER(c_group_add, "LinkSet::group_add");

struct H {
    // ------------------------------------------------------------------ 0x00666c30
    i32 link_ends(i32 i, u32* a0, u32* a1, u32* a2, u32* b0, u32* b1, u32* b2) {
        c_link_ends.hit();
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->links) == nullptr || i <= 0 || i > arr_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->links))) return 0;
        u8* const e = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->links), i);
        *a0 = jo::as_pair(e)->a->a;
        *a1 = jo::as_pair(e)->a->b;
        *a2 = jo::as_pair(e)->a->c;
        *b0 = jo::as_pair(e)->b->a;
        *b1 = jo::as_pair(e)->b->b;
        *b2 = jo::as_pair(e)->b->c;
        return i;
    }
    // ------------------------------------------------------------------ 0x00666d00
    std::uintptr_t swap_link(i32 i) {  // EAX: this / the count / the entry (pointer-wide on x64)
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->links) == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        if (i <= 0) return reinterpret_cast<std::uintptr_t>(this);
        const i32 n = arr_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->links));
        if (i > n) return static_cast<u32>(n);
        u8* const e = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->links), i);
        jo::Triple* const t = jo::as_pair(e)->a;
        jo::as_pair(e)->a = jo::as_pair(e)->b;
        jo::as_pair(e)->b = t;
        return reinterpret_cast<std::uintptr_t>(e);
    }
    // ------------------------------------------------------------------ 0x00666ed0
    i32 group_add(i32 g, u32 a, u32 b, u32 c) {
        c_group_add.hit();
        u8* const e = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->groups), g);
        if (jo::as_path_group(e)->count >= 4) return 0;
        u32 pair[2];
        u32* const p = joan::orig::arg::as<u32*>(joan::orig::as_point(pair)->set(joan::orig::arg::u32(a), joan::orig::arg::u32(b)));  // 0x004eeb80
        u32 trip[3];
        void* const t = joan::orig::as_triple(trip)->construct(joan::orig::arg::u32(p[0]), joan::orig::arg::u32(p[1]), joan::orig::arg::u32(c));  // 0x0066a4b0
        joan::orig::as_triple(&jo::as_path_group(e)->entries[jo::as_path_group(e)->count])->assign(joan::orig::arg::mptr(t));  // 0x00663f30
        jo::as_path_group(e)->count = jo::as_path_group(e)->count + 1;
        return jo::as_path_group(e)->count;
    }
    // ------------------------------------------------------------------ 0x00667040
    i32 group_entry(i32 g, i32 i, u32* a, u32* b, u32* c) {
        u8* const e = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->groups), g);
        if (i <= 0 || i > jo::as_path_group(e)->count) return 0;
        *a = jo::as_path_group(e)->entries[i - 1].a;
        *b = jo::as_path_group(e)->entries[i - 1].b;
        *c = jo::as_path_group(e)->entries[i - 1].c;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("PathSpace::get_sub_path_line_pos", 0x00666c30, &H::link_ends);
B("PathSpace::reverse_sub_path_line", 0x00666d00, &H::swap_link);
B("PathSpace::add_obstacle_vertex", 0x00666ed0, &H::group_add);
B("PathSpace::get_obstacle_vertex", 0x00667040, &H::group_entry);
#undef B

}  // namespace game_b102_14_2
}  // namespace

// ---------------------------------------- from modules/game_b99_09.cpp (game/b99_09)
namespace {
namespace game_b99_09 {

using u32 = std::uint32_t;

struct H {
    u32 call_66b7a0() {  // 0x00666fc0
        if (jo::self<jo::PathSpace>(this)->groups == nullptr) return 0;
        return joan::orig::arg::as<u32>(joan::orig::as_path_space(jo::self<jo::PathSpace>(this)->groups)->get_2c());  // 0x0066b7a0
    }
};

#define B99(name, addr_, fn) JOAN_REPLACE("game/b99_09", name, addr_, fn)
B99("PathSpace::get_obstacle_count", 0x00666fc0, &H::call_66b7a0);
#undef B99

}  // namespace game_b99_09
}  // namespace

// ---------------------------------------- from modules/game_b95_07.cpp (game/b95_07)
namespace {
namespace game_b95_07 {

JOAN_COUNTER(c_seg_dist2, "point_segment_dist2");

// 0x006639e0 (cdecl): out = (a - c, b - d), returns out.
const i32* sub2(i32* out, i32 a, i32 b, i32 c, i32 d) {
    return joan::orig::arg::as<const i32*>(joan::orig::p2::sub(static_cast<void*>(out), P2{a, b}, P2{c, d}));
}

// ---------------------------------------------------------------------------- 0x00669cc0
static i32 JOAN_CDECL point_segment_dist2(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2) {
    c_seg_dist2.hit();
    i32 l14[2];
    const i32 len2 = joan::orig::arg::as<i32>(joan::orig::as_vec2i(sub2(l14, x2, y2, x1, y1))->length_sq());  // 0x0066a440
    i32 l24[2];
    const i32* const d = sub2(l24, x2, y2, x1, y1);
    const i32 dx = d[0];
    const i32 dy = d[1];
    i32 l1c[2];
    const i32 dot = joan::orig::arg::as<i32>(joan::orig::as_vec2i(sub2(l1c, x0, y0, x1, y1))->dot(joan::orig::arg::i32(dx), joan::orig::arg::i32(dy)));  // 0x0066a3c0
    if (dot <= 0) {
        i32 l2c[2];
        return joan::orig::arg::as<i32>(joan::orig::as_vec2i(sub2(l2c, x0, y0, x1, y1))->length_sq());  // 0x0066a440
    }
    if (dot >= len2) {
        i32 l34[2];
        return joan::orig::arg::as<i32>(joan::orig::as_vec2i(sub2(l34, x0, y0, x2, y2))->length_sq());  // 0x0066a440
    }
    i32 l44[2];
    const i32* const e = sub2(l44, x2, y2, x1, y1);
    const i32 ex = e[0];
    const i32 ey = e[1];
    i32 l3c[2];
    const i32 cr = joan::orig::arg::as<i32>(joan::orig::as_vec2i(sub2(l3c, x0, y0, x1, y1))->cross(joan::orig::arg::i32(ex), joan::orig::arg::i32(ey)));  // 0x0066a400
    return joan::orig::arg::as<i32>(joan::orig::mul_div(joan::orig::arg::i32(cr), joan::orig::arg::i32(cr), joan::orig::arg::i32(len2)));                               // 0x00592170
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_07", name, addr, fn)
B95("point_to_line_segment_dist2", 0x00669cc0, &point_segment_dist2);
#undef B95

}  // namespace game_b95_07
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {

using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;                      // operator delete

struct H {
    // ------------------------------------------------------------------ 0x00663c00
    void buffer_free() {
        if (jo::self<jo::Buffer>(this)->children != nullptr) {
            game_delete(jo::self<jo::Buffer>(this)->children);
            jo::self<jo::Buffer>(this)->children = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("PathSearchNode::deinit", 0x00663c00, &H::buffer_free);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07_2 {

struct H {
    // ------------------------------------------------------------------ 0x00664080
    void* get(i32 i) {  // the neighbour (links[i - 1]) or null
        if (i > 0 && i <= jo::self<jo::TerrainBlock>(this)->count) return static_cast<void* const*>(jo::self<jo::TerrainBlock>(this)->links)[i - 1];
        return nullptr;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("PathPolygon::enum_adj_polygon", 0x00664080, &H::get);
#undef B94

}  // namespace game_b94_07_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::point::scale(void* a0, std::int32_t a1, std::int32_t a2, float a3) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b93_04_2::point_scale>(a0, a1, a2, a3)); }  // 0x663a30
auto joan::orig::Zone::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_04_2::H::ctor_663c50>(this)); }  // 0x663c50
auto joan::orig::Zone::free_points() -> void { joan::bind::member<&::game_b84_04_3::H::free_663f70>(this); }  // 0x663f70
auto joan::orig::Zone::link_index(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04_3::H::index_of>(this, a0)); }  // 0x6640d0
auto joan::orig::TerrainBlock::height(float a0, float a1) -> double { return joan::bind::conv<double>(joan::bind::member<&::game_collide1::H::height>(this, a0, a1)); }  // 0x664180
auto joan::orig::TerrainBlock::normal(void* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_light1::H::normal>(this, a0, a1, a2)); }  // 0x6645b0
auto joan::orig::Group::recentre(std::int32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_14::H::recentre>(this, a0, a1)); }  // 0x664920
auto joan::orig::Member::set_group_664eb0(void* a0) -> void { joan::bind::member<&::game_b92_05::H::set_group>(this, a0); }  // 0x664eb0
auto joan::orig::PathSpace::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_light1::H::pfd_destroy>(this)); }  // 0x665000
auto joan::orig::PathSpace::clear_grid() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_14::H::clear_grid>(this)); }  // 0x665290
auto joan::orig::PathSpace::group_size(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_14::H::poly_first>(this, a0)); }  // 0x667000
auto joan::orig::PathSpace::clear_children() -> void { joan::bind::member<&::game_b83_04::H::clear_children>(this); }  // 0x6670e0
auto joan::orig::PathNode::free_buffer() -> void { joan::bind::member<&::game_b85_05::H::buffer_free>(this); }  // 0x663c00
auto joan::orig::Zone::link_at(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_07_2::H::get>(this, a0)); }  // 0x664080
auto joan::orig::line_intersect_terms(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, void* a8, void* a9, void* a10) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b92_05_2::line_intersect_terms>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)); }  // 0x664b70
auto joan::orig::tri_normal_2d(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, void* a12, void* a13, void* a14) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_04_3::tri_normal_2d>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)); }  // 0x664c60
auto joan::orig::point_segment_dist2(float a0, float a1, float a2, float a3, float a4, float a5) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b88_04::point_segment_dist2>(a0, a1, a2, a3, a4, a5)); }  // 0x664d70
auto joan::orig::PathSpace::point_index(Triple* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_04_3::H::index_of>(this, a0)); }  // 0x665940
auto joan::orig::PathSpace::clear_groups() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_14_2::H::free_10>(this)); }  // 0x666d80
auto joan::orig::PathSpace::group_count() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_09::H::call_66b7a0>(this)); }  // 0x666fc0
auto joan::orig::point_segment_dist2_669cc0(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::point_segment_dist2>(a0, a1, a2, a3, a4, a5)); }  // 0x669cc0
auto joan::orig::tri::segment_hits(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b104_12::segment_hits>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x669e50
