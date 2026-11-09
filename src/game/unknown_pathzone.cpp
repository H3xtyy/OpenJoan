// game/unknown_pathzone.cpp: functions whose original source file is unknown (formerly game/_unplaced/pathzone.cpp;
// re/attribution.csv: between ww/enpath/pathspr.cpp and ww/enphysic/physmath.cpp). They move to their original file
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

// ---------------------------------------- from modules/game_players1.cpp (game/players1)
namespace {
namespace game_players1 {

struct H {
    // 0x006aac10 GridCell::layer(side) (ret 4): side != 0 -> +0 if set else +4; side == 0 ->
    // +4 if set else +0.
    void* cell_layer(std::int16_t side) {
        if (static_cast<std::int32_t>(side) != 0) {
            if (jo::self<jo::GridCell>(this)->layer0 != nullptr) return jo::self<jo::GridCell>(this)->layer0;
            return jo::self<jo::GridCell>(this)->layer1;
        }
        if (jo::self<jo::GridCell>(this)->layer1 != nullptr) return jo::self<jo::GridCell>(this)->layer1;
        return jo::self<jo::GridCell>(this)->layer0;
    }
};

#define P1(name, addr, fn) JOAN_REPLACE("game/players1", name, addr, fn)
P1("GridCell::layer", 0x006aac10, &H::cell_layer);
#undef P1

}  // namespace game_players1
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

JOAN_COUNTER(c_rect_dist, "rect_dist_sq");

// 0x006aada0: squared distance from (x, y) to the rectangle [r0, r3] x [r1, r4].
float JOAN_CDECL rect_dist_sq(float x, float y, const float* r) {
    c_rect_dist.hit();
    float dx, dy;
    if (!(x >= r[0])) {  // x < r0 or unordered
        dx = x - r[0];
    } else if (!(r[3] >= x)) {  // r3 < x or unordered
        dx = x - r[3];
    } else {
        dx = 0.0f;
    }
    if (!(y >= r[1])) {
        dy = y - r[1];
    } else if (!(r[4] >= y)) {
        dy = y - r[4];
    } else {
        dy = 0.0f;
    }
    return dx * dx + dy * dy;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("rect_dist_sq", 0x006aada0, &rect_dist_sq);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b109_03.cpp (game/b109_03)
namespace {
namespace game_b109_03 {

using joan::orig::helpers::wrap_add;
using joan::orig::helpers::wrap_sub;

struct P2 {
    i32 x, y;
};
struct Vtx {  // path vertex handed to 0x0066e870: float x, y, z then the _ftol'ed ints
    float x, y, z;
    i32 ix, iy, iz;
};

JOAN_COUNTER(c_route, "route_around_6ac050");

// ============================================================================ 0x006ac050
// Vertex of the output path at grid point (gx, gy) of polygon record r; z from 0x00664180.
// keep_raw: the original _ftol's the raw st0 (fst + _ftol) rather than the stored float.
Vtx path_vertex(const u8* r, void* poly, i32 key, i32 gx, i32 gy, bool keep_raw, float* zf_out) {
    Vtx v{};
    const i32 sx = wrap_add(gx, jo::as_grid_cell(r)->origin_x);
    v.x = static_cast<float>(sx);
    v.ix = sx;
    const i32 sy = wrap_add(gy, jo::as_grid_cell(r)->origin_y);
    v.y = static_cast<float>(sy);
    v.iy = sy;
    void* const h = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(poly)->zones)->item(joan::orig::arg::i32(key)));  // 0x00637da0
    const double z = joan::orig::arg::as<double>(joan::orig::as_terrain_block(h)->height(joan::orig::arg::f32(static_cast<float>(gx)), joan::orig::arg::f32(static_cast<float>(gy))));  // 0x00664180
    const float zf = static_cast<float>(z);
    v.z = zf;
    v.iz = keep_raw ? ftol(z) : ftol(static_cast<double>(zf));
    if (zf_out != nullptr) *zf_out = zf;
    return v;
}

static void JOAN_CDECL route_around_6ac050(u8* base, i32 count, i32 idx, i32 key, void* out, void* in, i32 dx, i32 dy,
                                        i32 /*unused*/, float px, float py, float pz, i32 radius) {
    c_route.hit();
    const Fpu fpu{true};
    u8* r = base + wrap_mul(idx, static_cast<i32>(sizeof(jo::GridCell)));
    void* poly = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(r)->layer(joan::orig::arg::i16(i32{1})));  // 0x006aac10
    joan::orig::as_list(in)->from_end(i32{0});              // 0x0066e8d0, result unused
    P2 s{};
    P2 t{};
    P2 d{};
    joan::orig::as_point(&s)->construct();  // 0x004eea60
    joan::orig::as_point(&t)->construct();
    joan::orig::as_point(&d)->construct();
    P2 c{};
    {
        const i32 cy = ftol(fpu.sub(py, static_cast<double>(jo::as_grid_cell(r)->origin_y)));
        const i32 cx = ftol(fpu.sub(px, static_cast<double>(jo::as_grid_cell(r)->origin_x)));
        joan::orig::as_point(&c)->set(joan::orig::arg::u32(cx), joan::orig::arg::u32(cy));  // 0x004eeb80
    }
    joan::orig::as_path(out)->clear();  // 0x004eeca0
    jo::PathRoute* const route = jo::as_path_route(out);
    route->start[0] = px;
    route->start_i_b[0] = ftol(px);
    route->start_i_a[0] = route->start_i_b[0];
    route->start[1] = py;
    route->start_i_b[1] = ftol(py);
    route->start_i_a[1] = route->start_i_b[1];
    route->start[2] = pz;
    route->start_i_b[2] = ftol(pz);
    route->start_i_a[2] = route->start_i_b[2];

    bool cut = false;  // [ebp-0x44]
    P2 hit{};          // [ebp-0x84]
    P2 last{};         // [ebp-0x90]
    i32 o30 = 0;
    float o34 = 0.0f;  // read by 0x006731d0 only after it wrote o30 (dead initial value)
    i32 o38 = 0;
    float o3c = 0.0f;
    for (i32 k = 1; k <= joan::orig::arg::as<i32>(joan::orig::as_list(in)->count_66e8a0()); ++k) {  // 0x0066e8a0
        const float* p = joan::orig::arg::as<const float*>(joan::orig::as_list(in)->from_end(joan::orig::arg::i32(k - 1)));
        s.x = ftol(fpu.sub(fpu.add(static_cast<double>(dx), p[0]), static_cast<double>(jo::as_grid_cell(r)->origin_x)));
        s.y = ftol(fpu.sub(fpu.add(static_cast<double>(dy), p[1]), static_cast<double>(jo::as_grid_cell(r)->origin_y)));
        p = joan::orig::arg::as<const float*>(joan::orig::as_list(in)->from_end(joan::orig::arg::i32(k)));
        t.x = ftol(fpu.sub(fpu.add(static_cast<double>(dx), p[0]), static_cast<double>(jo::as_grid_cell(r)->origin_x)));
        t.y = ftol(fpu.sub(fpu.add(static_cast<double>(dy), p[1]), static_cast<double>(jo::as_grid_cell(r)->origin_y)));
        {
            P2 tmp{};
            const P2* const q = joan::orig::arg::as<const P2*>(joan::orig::p2::sub(&tmp, t, s));  // 0x006639e0
            joan::orig::as_vec2i(&d)->assign(joan::orig::arg::mptr(q));                                               // 0x0066a380
        }
        const i32 cross = wrap_sub(wrap_mul(d.x, wrap_sub(c.y, s.y)), wrap_mul(d.y, wrap_sub(c.x, s.x)));
        if (!cut) {
            const float len2 = static_cast<float>(wrap_add(wrap_mul(d.x, d.x), wrap_mul(d.y, d.y)));
            if (!(len2 > global<float>(d::k_9cf5c4))) continue;  // test ah,0x41 jne
            const double q = fpu.div(static_cast<double>(wrap_mul(cross, cross)), static_cast<double>(len2));
            if (!(q < static_cast<double>(global<float>(d::flt_9)))) continue;  // test ah,1 je
            // Not in the original, which looks only at the distance to the infinite line: it relied on routes that zigzag. On a
            // straight route every stretch is on one line, and the first would send the follower back to where its leader
            // has already been. The unit must be beside this stretch (docs/PATHFINDING.md).
            const double along = static_cast<double>(d.x) * static_cast<double>(c.x - s.x) + static_cast<double>(d.y) * static_cast<double>(c.y - s.y);
            const double margin = 3.0 * std::sqrt(static_cast<double>(len2));
            if (along < -margin || along > static_cast<double>(len2) + margin) continue;
            cut = true;
            joan::orig::as_vec2i(&s)->assign(&c);
        }
        joan::orig::as_point(&hit)->construct();
        bool more = true;  // [ebp-0x88]
        last = s;
        for (;;) {
            if (key == 0) break;
            if (s.x == t.x && s.y == t.y) break;
            i32 next = 0;  // [ebp-0x7c]
            const i32 res = joan::orig::arg::as<i32>(joan::orig::as_path_space(poly)->segment_cut(&next, &hit, joan::orig::arg::i32(key), joan::orig::arg::i32(s.x), joan::orig::arg::i32(s.y), joan::orig::arg::i32(t.x), joan::orig::arg::i32(t.y)));  // 0x006736e0
            if (res == 0) {
                Vtx v = path_vertex(r, poly, key, t.x, t.y, true, nullptr);
                joan::orig::as_path(out)->insert_front(&v);  // 0x0066e870
                break;
            }
            if (res == 1) {
                if (next == 0 ||
                    jo::as_pfd_zone(joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(poly)->zones)->item(joan::orig::arg::i32(next))))->blocked == 1) {  // 0x00637da0
                    float zf = 0.0f;  // [ebp-0x98]
                    Vtx v = path_vertex(r, poly, key, hit.x, hit.y, false, &zf);
                    joan::orig::as_path(out)->insert_front(&v);
                    if (next == 0) {
                        i32 best = 0;      // [ebp-0x9c]
                        i32 best_key = 0;  // [ebp-0xa0], read only when best != 0
                        for (i32 j = 0; j < count; ++j) {
                            if (j == idx) continue;
                            o30 = 0;
                            o38 = 0;
                            void* const pj = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(base + wrap_mul(j, static_cast<i32>(sizeof(jo::GridCell))))->layer(joan::orig::arg::i16(i32{1})));
                            joan::orig::as_path(pj)->find_6731d0(joan::orig::arg::f32(static_cast<float>(hit.x)), joan::orig::arg::f32(static_cast<float>(hit.y)), joan::orig::arg::f32(zf), joan::orig::arg::bits(static_cast<float>(radius)), &o30, &o34, &o38, &o3c, joan::orig::arg::u32(i32{0}));  // 0x006731d0
                            if (o30 != 0 && o38 != 0) {
                                const float a = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(o34 - zf))));  // 0x00427d60
                                const double b = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(o3c - zf)));
                                best = j;
                                best_key = (b > static_cast<double>(a)) ? o30 : o38;  // test ah,0x41 jne
                                break;
                            }
                            if (o30 != 0) {
                                best = j;
                                best_key = o30;
                                break;
                            }
                            if (o38 != 0) {
                                best = j;
                                best_key = o38;
                                break;
                            }
                        }
                        if (best != 0) {
                            u8* const e = base + wrap_mul(best, static_cast<i32>(sizeof(jo::GridCell)));
                            void* const pe = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(e)->layer(joan::orig::arg::i16(i32{1})));
                            void* const h = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(pe)->zones)->item(joan::orig::arg::i32(best_key)));
                            if (jo::as_pfd_zone(h)->blocked != 1) {
                                s.x = wrap_add(s.x, wrap_sub(jo::as_grid_cell(r)->origin_x, jo::as_grid_cell(e)->origin_x));
                                s.y = wrap_add(s.y, wrap_sub(jo::as_grid_cell(r)->origin_y, jo::as_grid_cell(e)->origin_y));
                                t.x = wrap_add(t.x, wrap_sub(jo::as_grid_cell(r)->origin_x, jo::as_grid_cell(e)->origin_x));
                                t.y = wrap_add(t.y, wrap_sub(jo::as_grid_cell(r)->origin_y, jo::as_grid_cell(e)->origin_y));
                                joan::orig::as_vec2i(&hit)->assign(&s);
                                r = e;
                                poly = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(e)->layer(joan::orig::arg::i16(i32{1})));
                                idx = best;
                                key = best_key;
                            }
                        }
                    }
                    more = false;
                    break;
                }
                key = next;
                joan::orig::as_vec2i(&last)->assign(&hit);
            } else if (res == 2) {
                if (last.x != s.x || last.y != s.y) {
                    Vtx v = path_vertex(r, poly, key, last.x, last.y, true, nullptr);
                    joan::orig::as_path(out)->insert_front(&v);
                }
                more = false;
                break;
            }
            if (s.x == hit.x && s.y == hit.y) {
                more = false;
                break;
            }
        }
        if (!more) return;
    }
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_03", name, addr, fn)
B("route_around_6ac050", 0x006ac050, &route_around_6ac050);
#undef B

}  // namespace game_b109_03
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("init_6ac9f0", 0x006ac9f0, &static_init<fn::init_pi_a6a4fc>);
B("init_6aca50", 0x006aca50, &static_init<fn::init_a6a500>);
B("init_6acaa0", 0x006acaa0, &static_init<fn::init_third_a>);
B("init_6acaf0", 0x006acaf0, &static_init<fn::scale_6acb10>);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

float pi_from_atan() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(Fpu{true}.mul(a, 4.0));       // qword [0x9cf788] = 4.0
}
// 0x006aca10
void JOAN_CDECL init_pi_a6a4fc() { global<float>(d::g_a6a4fc) = pi_from_atan(); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("init_pi_a6a4fc", 0x006aca10, &init_pi_a6a4fc);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

// ---------------------------------------------------------------------------- static initialisers
static void JOAN_CDECL init_a6a500() { global<float>(d::g_a6a500) = global<float>(d::g_a6a4fc) / global<float>(d::flt_2); }  // 0x006aca70

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("init_a6a500", 0x006aca70, &init_a6a500);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

static void JOAN_CDECL init_third_a() { global<float>(d::g_a6a504) = global<float>(d::g_a6a4fc) / global<float>(d::flt_3); }

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("init_third_a", 0x006acac0, &init_third_a);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

static void JOAN_CDECL scale_6acb10() { global<float>(d::g_a6a508) = global<float>(d::g_a6a4fc) / global<float>(d::flt_4); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("scale_6acb10", 0x006acb10, &scale_6acb10);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::GridCell::layer(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_players1::H::cell_layer>(this, a0)); }  // 0x6aac10
auto joan::orig::route_around_6ac050(void* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, void* a4, void* a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, float a9, float a10, float a11, std::int32_t a12) -> void { joan::bind::free<&::game_b109_03::route_around_6ac050>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12); }  // 0x6ac050
