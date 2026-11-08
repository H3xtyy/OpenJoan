// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/physys.cpp: functions of the original d:\ww\enphysic\physys.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (enphysic/PhySys.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for
// it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::hook::global;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;

using joan::hook::x87::ftol;

// Players::cells (RWorldCell[27]).
jo::RWorldCell* player_cells(const void* players) { return jo::self<jo::Players>(players)->cells; }

using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

struct H {
    // 0x006ad950
    std::uintptr_t reset_6ad950() {
        joan::orig::as_players(this)->shutdown();              // 0x006ad170
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_players(this)->init());  // 0x006ad020
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("PhysicSystem::reset", 0x006ad950, &H::reset_6ad950);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

struct H {
    // ------------------------------------------------------------------ 0x006acf80
    u32 dtor_6acf80() {
        joan::orig::as_players(this)->shutdown();                // 0x006ad170
        joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->destruct();    // 0x006b1ea0
        joan::orig::as_collision_grid(&jo::self<jo::Players>(this)->grid)->reset();        // 0x0068db50
        joan::orig::as_move_solver(&jo::self<jo::Players>(this)->move_solver)->nop();        // 0x006b07e0
        joan::orig::as_players(this)->nop_6b0740();         // 0x006b0740
        return 0;  // empty_6b0740 returns nothing; only called from the atexit stub 0x00569d30 (result unused)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("PhysicSystem::~PhysicSystem", 0x006acf80, &H::dtor_6acf80);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b104_13.cpp (game/b104_13)
namespace {
namespace game_b104_13 {

JOAN_COUNTER(c_init_ad020, "Obj6ad020::init");

using joan::orig::helpers::new_;  // operator new

struct H {
    // ------------------------------------------------------------------ 0x006ad020
    void* init_ad020() {
        c_init_ad020.hit();
        jo::self<jo::Players>(this)->running = 0;
        jo::self<jo::Players>(this)->i16_b1a = 1;
        jo::self<jo::Players>(this)->i16_b14 = 1;
        jo::view<u32>(jo::self<jo::Players>(this)->time_scale_) = 0x3f800000u;  // 1.0f
        jo::self<jo::Players>(this)->no_ground = 0;
        jo::self<jo::Players>(this)->no_route = 1;
        jo::self<jo::Players>(this)->last_time = global<u32>(d::g_now);
        joan::orig::as_players(this)->release_slots();                              // 0x006ad5d0
        joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->reset();                    // 0x006b1f10
        joan::orig::as_collision_grid(&jo::self<jo::Players>(this)->grid)->init(static_cast<jo::DynArray*>(&jo::self<jo::Players>(this)->list));  // 0x0068db80
        void* const p = new_(static_cast<std::uint32_t>(sizeof(jo::DynArrayB)));  // 0x2e on x86
        void* const list = p != nullptr ? joan::orig::dynarray_b(p)->construct(static_cast<std::int32_t>(sizeof(void*)), 5, 0, 0) : nullptr;  // 0x005856a0 (pointers: 4 on x86)
        jo::self<jo::Players>(this)->active = static_cast<jo::DynArrayB*>(list);
        return list;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_13", name, addr, fn)
B("PhysicSystem::init", 0x006ad020, &H::init_ad020);
#undef B

}  // namespace game_b104_13
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using joan::orig::helpers::addr;

JOAN_COUNTER(c_shutdown_b0e, "X::shutdown_b0e");
JOAN_COUNTER(c_pick_point, "X::pick_point");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------------ 0x006ad170
    std::uintptr_t shutdown_b0e() {
        c_shutdown_b0e.hit();
        joan::orig::as_players(this)->release_slots();          // 0x006ad5d0
        joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->clear();  // 0x006b1f30
        joan::orig::as_collision_grid(&jo::self<jo::Players>(this)->grid)->clear();      // 0x0068dbc0
        if (jo::self<jo::Players>(this)->active == nullptr) return addr(this);
        joan::orig::as_sound(jo::self<jo::Players>(this)->active)->reset_585d20();  // 0x00585d20
        void* const p = jo::self<jo::Players>(this)->active;
        std::uintptr_t r = 0;
        if (p != nullptr) r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_dyn_array_b(p)->scalar_delete(joan::orig::arg::u32(i32{1})));  // 0x006ad220

        jo::self<jo::Players>(this)->active = nullptr;
        return r;
    }

    // ------------------------------------------------------------------------ 0x006ae2a0
    void pick_point(u32* out, float* out_h, void* target, u32 sub, u32, float x, float y, u32, i32 kind) {
        c_pick_point.hit();
        if (target != nullptr && sub != 0) {
            const Fpu fpu{true};
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::placements::find(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Players>(this)->cell_count)), static_cast<jo::PathSpace*>(target)));  // 0x004a2b20
            const float fy = static_cast<float>(fpu.sub(y, static_cast<double>(jo::as_rworld_cell(p)->y)));
            const float fx = static_cast<float>(fpu.sub(x, static_cast<double>(jo::as_rworld_cell(p)->x)));
            *out_h = joan::orig::arg::as<float>(joan::orig::as_terrain(target)->height_66cae0(joan::orig::arg::i32(sub), joan::orig::arg::f32(fx), joan::orig::arg::f32(fy)));  // 0x0066cae0
            void* const q = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(target)->zones)->item(joan::orig::arg::i32(sub)));  // 0x00637da0
            const i32 iy = joan::hook::x87::ftol(fpu.sub(y, static_cast<double>(jo::as_rworld_cell(p)->y)));
            const i32 ix = joan::hook::x87::ftol(fpu.sub(x, static_cast<double>(jo::as_rworld_cell(p)->x)));
            joan::orig::as_terrain_block(q)->normal(joan::orig::arg::mptr(out), joan::orig::arg::i32(ix), joan::orig::arg::i32(iy));  // 0x006645b0
            (void)kind;  // every kind ends in the same test
            if (joan::orig::arg::as<double>(joan::orig::grid::slope(joan::orig::arg::mptr(out))) > 0.0) {  // 0x006accf0
                out[0] = 0;
                out[1] = 0;
                out[2] = 0x3f800000;
            }
            return;
        }
        *reinterpret_cast<u32*>(out_h) = static_cast<i32>(jo::self<jo::Players>(this)->no_ground) != 0 ? 0xc59c4000u : 0u;  // -5000 / 0
        out[0] = 0;
        out[1] = 0;
        out[2] = 0x3f800000;
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("PhysicSystem::deinit", 0x006ad170, &H::shutdown_b0e);
B98("PhysicSystem::get_slope_and_floor_height2", 0x006ae2a0, &H::pick_point);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

constexpr u32 kOne = 0x3f800000u;    // 1.0f

JOAN_COUNTER(c_point_to_cell, "Map::point_to_cell");

struct H {
    u8* self() { return static_cast<u8*>(static_cast<void*>(this)); }

    // ------------------------------------------------------------------ 0x006ae500
    void point_to_cell(float* out, u8* a, i32 b, i32 /*unused14*/, float x, float y, i32 /*unused20*/) {
        c_point_to_cell.hit();
        if (a != nullptr && b != 0) {
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::placements::find(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::self<jo::Players>(this)->cell_count), static_cast<jo::PathSpace*>(static_cast<void*>(a))));  // 0x004a2b20
            void* const poly = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(a)->zones)->item(joan::orig::arg::i32(b)));         // 0x00637da0
            const Fpu fpu{true};
            const i32 cy = joan::hook::x87::ftol(fpu.sub(y, static_cast<double>(jo::as_rworld_cell(p)->y)));
            const i32 cx = joan::hook::x87::ftol(fpu.sub(x, static_cast<double>(jo::as_rworld_cell(p)->x)));
            joan::orig::as_terrain_block(poly)->normal(joan::orig::arg::mptr(out), joan::orig::arg::i32(cx), joan::orig::arg::i32(cy));  // 0x006645b0
            return;
        }
        const u32 v[3] = {0, 0, kOne};
        std::memcpy(out, v, sizeof v);
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("PhysicSystem::get_true_slope", 0x006ae500, &H::point_to_cell);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_players1.cpp (game/players1)
namespace {
namespace game_players1 {

struct H {
    // 0x006ae610 (ret 0x30): forwards args 0, 1, 2 (word), 7, 8, 9, 10, 6, 11 to 0x006ae8e0;
    // args 3..5 unused. The word arg is pushed as eax whose high half holds arg 9 (callee
    // reads the word only; reproduced anyway).
    std::uint32_t find_slot_wrapper(void* a0, void* a1, std::uint32_t a2, std::uint32_t, std::uint32_t,
                                    std::uint32_t, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9,
                                    std::uint32_t a10, std::uint32_t a11) {
        const std::uint32_t word = (a9 & 0xffff0000u) | (a2 & 0xffffu);
        // find_slot returns a word; every caller of 0x006ae610 reads ax only (movsx / i16).
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_players(this)->find_slot(a0, a1, joan::orig::arg::i16(word), joan::orig::arg::f32(a7), joan::orig::arg::f32(a8), joan::orig::arg::f32(a9), joan::orig::arg::i32(a10), joan::orig::arg::f32(a6), joan::orig::arg::i32(a11)));  // 0x006ae8e0
    }
};

#define P1(name, addr, fn) JOAN_REPLACE("game/players1", name, addr, fn)
P1("PhysicSystem::check_path_pass_through", 0x006ae610, &H::find_slot_wrapper);
#undef P1

}  // namespace game_players1
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

struct Pt {
    i32 x, y;
};

JOAN_COUNTER(c_route, "Map::route_6ae670");

struct H {
    // -------------------------------------------------------------- 0x006ae670
    u16 map_route(void** a1, u32* a2, i32 key, i32 x1, i32 y1, i32 z1, i32 x2, i32 y2, i32 z2, u32 /*unused*/) {
        c_route.hit();
        if (jo::self<jo::Players>(this)->no_route != 0) return 1;
        if (*a1 == nullptr) return 1;
        if (x1 == x2 && y1 == y2 && z1 == z2) return 1;
        Pt pa{0, 0}, pb{0, 0}, pc{0, 0};
        joan::orig::as_point(&pa)->set(joan::orig::arg::u32(x1), joan::orig::arg::u32(y1));  // 0x004eeb80
        joan::orig::as_point(&pb)->set(joan::orig::arg::u32(x2), joan::orig::arg::u32(y2));  // 0x004eeb80
        joan::orig::as_point(&pc)->construct();          // 0x004eea60
        i16 r = -1;
        u32 found = 0;
        if (*a2 != 0) {
            r = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_path_space(*a1)->segment_cut(&found, &pc, joan::orig::arg::i32(*a2), joan::orig::arg::i32(pa.x), joan::orig::arg::i32(pa.y), joan::orig::arg::i32(pb.x), joan::orig::arg::i32(pb.y))));  // 0x006736e0
        }
        if (r == 0) return 1;
        if (r == 1 && found != 0) {
            *a2 = found;
            return 1;
        }
        if (r == 1 && found == 0) {
            jo::RWorldCell* const skip = joan::orig::arg::as<jo::RWorldCell*>(joan::orig::placements::find(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Players>(this)->cell_count)), static_cast<jo::PathSpace*>(*a1)));  // 0x004a2b20
            for (i32 i = 0; i < jo::self<jo::Players>(this)->cell_count; ++i) {
                jo::RWorldCell* const ent = &player_cells(this)[i];
                if (ent == skip) continue;
                const float fy = static_cast<float>(pc.y);
                const float fx = static_cast<float>(pc.x);
                void* const cell = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(ent)->layer(joan::orig::arg::i16(key)));  // 0x006aac10
                const u32 res = joan::orig::arg::as<u32>(joan::orig::as_path_space(cell)->find_region(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), joan::orig::arg::f32(i32{0})));  // 0x0066bb40
                if (res != 0) {
                    *a1 = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(&player_cells(this)[i])->layer(joan::orig::arg::i16(key)));  // 0x006aac10

                    *a2 = res;
                    return 1;
                }
            }
            return 0;
        }
        *a2 = 0;
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("PhysicSystem::check_path_pass_through2", 0x006ae670, &H::map_route);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b111_07.cpp (fix/uninit)
namespace {
namespace game_b111_07 {

JOAN_COUNTER(c_restart, "AnimTable::restart");

// ---------------------------------------------------------------------------------- 0x006af370
double fabs_(float v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(v)))); }  // CRT fabs
// [ebp-0x24/-0x20/-0x1c] = max of two fabs values, re-evaluating the chosen one like the original.
float max_abs(const void* o, std::int32_t first, std::int32_t second) {  // indices into the actor's aim_box
    const double a = fabs_(jo::as_actor(o)->aim_box[first]);
    const double b = fabs_(jo::as_actor(o)->aim_box[second]);
    const double r = !(b >= a) ? fabs_(jo::as_actor(o)->aim_box[first]) : fabs_(jo::as_actor(o)->aim_box[second]);  // fcomp: b < a or NaN
    return static_cast<float>(r);
}

struct H {
    // ------------------------------------------------------------------------------ 0x006af370
    u32 restart(u32 id, u32 a2, u32 w3, u32 w4, u32 w5, u32 w6) {
        c_restart.hit();
        const u32 missing = joan::orig::arg::as<u32>(joan::orig::as_dynarray(&jo::self<jo::Players>(this)->list)->empty_at(joan::orig::arg::i32(id)));  // 0x006b0790
        if (missing != 0) return missing;
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->get(joan::orig::arg::u32(id)));  // 0x006b0760
        void* const o = jo::as_shot(e)->actor;
        alignas(4) float pos[3] = {0.0f, 0.0f, 0.0f};
        joan::orig::as_actor(o)->position(joan::orig::arg::mptr(static_cast<void*>(pos)));  // 0x005ac270 (writes all three)
        // [ebp-0x24], [ebp-0x20], [ebp-0x1c]. fix/uninit: kinds > 3 get 0, 0, 0 (case 0).
        alignas(4) u32 ext[3] = {0, 0, 0};
        switch (jo::as_actor(o)->aim_mode) {
            case 0:
                ext[2] = 0;
                ext[1] = ext[2];
                ext[0] = ext[1];
                break;
            case 1:
                ext[2] = jo::view<u32>(jo::as_actor(o)->aim_sphere[3]);
                ext[1] = ext[2];
                ext[0] = ext[1];
                break;
            case 2: {
                const float x = max_abs(o, 3, 0);
                std::memcpy(&ext[0], &x, 4);
                const float y = max_abs(o, 4, 1);
                std::memcpy(&ext[1], &y, 4);
                const float z = max_abs(o, 5, 2);
                std::memcpy(&ext[2], &z, 4);
                break;
            }
            case 3: {
                ext[1] = jo::view<u32>(jo::as_actor(o)->collide_radius);
                ext[0] = ext[1];
                const float z = jo::as_actor(o)->collide_height / 2.0f;  // fdiv [0x9cf794] (2.0f), one float op
                std::memcpy(&ext[2], &z, 4);
                break;
            }
            default:
                break;
        }
        const i16 s4 = static_cast<i16>(w4);
        if (s4 != 0) return static_cast<u32>(static_cast<i32>(s4));
        joan::orig::as_players(this)->set_active(joan::orig::arg::i32(id), joan::orig::arg::i16(u32{0}));  // 0x006ad980
        // 0x006b08e0 and 0x006ad980 read their word args with movsx only.
        joan::orig::as_body(e)->init(joan::orig::arg::mptr(static_cast<void*>(this)), static_cast<jo::Actor*>(o), joan::orig::arg::u32(a2), joan::orig::arg::mptr(static_cast<void*>(ext)), joan::orig::arg::i16(w3), joan::orig::arg::i16(w6));  // 0x006b08e0
        jo::as_shot(e)->tag = static_cast<u16>(w5);
        jo::view<i32>(jo::as_shot(e)->state.cell_pos[0]) = ftol(pos[0]);
        jo::view<i32>(jo::as_shot(e)->state.cell_pos[1]) = ftol(pos[1]);
        jo::view<i32>(jo::as_shot(e)->state.cell_pos[2]) = ftol(pos[2]);
        return joan::orig::arg::as<u32>(joan::orig::as_players(this)->set_active(joan::orig::arg::i32(id), joan::orig::arg::i16(w6)));  // 0x006ad980
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

// fix/uninit: 0x006af370's extents [ebp-0x24..-0x1c] are uninitialised for object kinds > 3 -> 0, 0, 0.
JOAN_REPLACE("fix/uninit", "PhysicSystem::reinit_object", 0x006af370, &H::restart);

}  // namespace game_b111_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Players::init() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b104_13::H::init_ad020>(this)); }  // 0x6ad020
auto joan::orig::Players::shutdown() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::shutdown_b0e>(this)); }  // 0x6ad170
auto joan::orig::Players::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_19::H::reset_6ad950>(this)); }  // 0x6ad950
auto joan::orig::Players::pick_point(void* a0, void* a1, void* a2, std::uint32_t a3, std::uint32_t a4, float a5, float a6, std::uint32_t a7, std::int32_t a8) -> void { joan::bind::member<&::game_b98_08::H::pick_point>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8); }  // 0x6ae2a0
auto joan::orig::Map::point_to_cell(void* a0, void* a1, std::int32_t a2, std::int32_t a3, float a4, float a5, std::int32_t a6) -> void { joan::bind::member<&::game_b95_08::H::point_to_cell>(this, a0, a1, a2, a3, a4, a5, a6); }  // 0x6ae500
auto joan::orig::Players::find_slot_wrapper(void* a0, void* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, std::uint32_t a11) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_players1::H::find_slot_wrapper>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }  // 0x6ae610
auto joan::orig::AnimTable::restart_6af370(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_07::H::restart>(this, a0, a1, a2, a3, a4, a5)); }  // 0x6af370

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::img::obj;

struct V3 {
    float x, y, z;
};

}  // namespace

// ---------------------------------------- from modules/game_players1.cpp (game/players1)
namespace {
namespace game_players1_2 {

// fcomp + test ah,0x40 + je "differs": equal OR unordered falls through.
bool same_or_unordered(float a, float b) { return !(a < b || a > b); }

using joan::orig::helpers::as_float;

static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_move_actor, "Players::move_actor");

struct H {
    // 0x006af190 Players::move_actor(actor, x, y, z) (ret 0x10), returns a word.
    std::int16_t move_actor(void* actor, std::uint32_t xb, std::uint32_t yb, std::uint32_t zb) {
        c_move_actor.hit();
        float px, py, pz;
        joan::orig::as_actor(actor)->get_xyz(&px, &py, &pz);  // 0x005acc30 position
        if (same_or_unordered(as_float(xb), px) && same_or_unordered(as_float(yb), py) && same_or_unordered(as_float(zb), pz))
            return 1;
        if (jo::view<std::int32_t>(jo::as_actor(actor)->record) == 0) {
            joan::orig::as_scene_object(actor)->set_angles(joan::orig::arg::u32(xb), joan::orig::arg::u32(yb), joan::orig::arg::u32(zb));  // 0x005acaf0
            return 1;
        }
        void* const rec = joan::orig::as_dynarray(&jo::self<jo::Players>(this)->list)->ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(actor)->record)));  // 0x0041b3f0
        if (jo::view<std::int32_t>(jo::as_shot(rec)->active_link) == 0) {
            joan::orig::as_scene_object(actor)->set_angles(joan::orig::arg::u32(xb), joan::orig::arg::u32(yb), joan::orig::arg::u32(zb));  // 0x005acaf0
            return 1;
        }
        V3 pos;
        joan::orig::as_actor(actor)->position(&pos);  // 0x005ac270
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_collision_grid(&jo::self<jo::Players>(this)->grid)->check_move(joan::orig::arg::mptr(this), joan::orig::arg::mptr(rec), &pos, joan::orig::arg::u32(xb), joan::orig::arg::u32(yb), joan::orig::arg::u32(zb))) != 0) return 0;  // 0x0068ee50
        joan::orig::as_body(rec)->pull_position();  // 0x006b18a0
        jo::view<std::uint32_t>(jo::as_shot(rec)->state.angle_x) = xb;
        jo::view<std::uint32_t>(jo::as_shot(rec)->state.angle_y) = yb;
        jo::view<std::uint32_t>(jo::as_shot(rec)->state.angle_z) = zb;
        joan::orig::as_body(rec)->sync_position();  // 0x006b1350
        return 1;
    }
};

#define P1(name, addr, fn) JOAN_REPLACE("game/players1", name, addr, fn)
P1("PhysicSystem::update", 0x006af190, &H::move_actor);
#undef P1

}  // namespace game_players1_2
}  // namespace

// ---------------------------------------- from modules/game_units5.cpp (game/units5)
namespace {
namespace game_units5 {

constexpr img::Addr kPlayers(d::g_physic_sys);

JOAN_COUNTER(c_place, "Players::place");
JOAN_COUNTER(c_set_active, "Players::set_active");

struct H {
    // 0x006aee40 (ret 8)
    std::int16_t place(void* actor, V3* pos) {
        c_place.hit();
        if (jo::self<jo::Players>(this)->cell_count <= 0 || jo::view<std::int32_t>(jo::as_actor(actor)->record) == 0) {
            joan::orig::as_actor(actor)->set_position_5ac310(joan::orig::arg::mptr(pos));  // 0x005ac310
            return 1;
        }
        std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(&jo::self<jo::Players>(this)->list)->ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(actor)->record))));  // 0x0041b3f0
        const std::int32_t type = jo::as_shot(rec)->type;
        float r44;
        if (type == 9 || type == 0xa || type == 0xb || type == 0xe || type == 0xf || type == 0x11) {
            const float m = !(jo::as_shot(rec)->extent[1] >= jo::as_shot(rec)->extent[0]) ? jo::as_shot(rec)->extent[0] : jo::as_shot(rec)->extent[1];
            r44 = m / global<float>(d::flt_1);
        } else {
            r44 = 0.0f;
        }
        float r18 = r44;
        if (jo::as_shot(rec)->type == 7 || jo::as_shot(rec)->type == 6) {
            const float m = !(jo::as_shot(rec)->extent[1] >= jo::as_shot(rec)->extent[0]) ? jo::as_shot(rec)->extent[0] : jo::as_shot(rec)->extent[1];
            r18 = m / global<float>(d::flt_3);
        }
        if (jo::view<std::int32_t>(jo::as_shot(rec)->active_link) == 0) {
            joan::orig::as_actor(actor)->set_position_5ac310(joan::orig::arg::mptr(pos));  // 0x005ac310
            return 1;
        }
        V3 v;  // -0x10
        joan::orig::as_actor(actor)->position(&v);  // 0x005ac270
        V3 tmp;
        const V3* const r = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(pos), &v));  // 0x0060cc90
        V3 dir = *r;  // -0x24
        const float pz = static_cast<float>(ftol(pos->z));
        const float py = static_cast<float>(ftol(pos->y));
        const float px = static_cast<float>(ftol(pos->x));
        const std::uint32_t r218 = jo::as_shot(rec)->state.cell;
        const std::int32_t vz = ftol(v.z), vy = ftol(v.y), vx = ftol(v.x);
        const std::int32_t rb = joan::orig::arg::as<std::int32_t>(joan::orig::as_body(rec)->side());  // 0x006b1dd0 (word; 0x006ae610 reads its low half only)
        const std::int16_t ok = joan::orig::arg::as<std::int16_t>(joan::orig::as_players(this)->find_slot_wrapper(&jo::as_shot(rec)->state.sub, &jo::as_shot(rec)->state.zone, joan::orig::arg::u32(rb), joan::orig::arg::u32(vx), joan::orig::arg::u32(vy), joan::orig::arg::u32(vz), joan::orig::arg::u32(r218), joan::orig::arg::bits(px), joan::orig::arg::bits(py), joan::orig::arg::bits(pz), joan::orig::arg::bits(r18), joan::orig::arg::u32(jo::as_shot(rec)->type)));  // 0x006ae610
        if (static_cast<std::int32_t>(ok) == 0) return 0;
        std::int32_t out28;  // -0x28, out of the callee
        std::memset(&out28, 0, sizeof out28);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_players(&jo::self<jo::Players>(this)->grid)->collide_68de40(joan::orig::arg::mptr(this), joan::orig::arg::mptr(rec), &v, &dir, &out28)) != 0) return 0;  // 0x0068de40
        jo::view<std::int32_t>(jo::as_shot(rec)->state.cell_pos[0]) = ftol(pos->x);
        jo::view<std::int32_t>(jo::as_shot(rec)->state.cell_pos[1]) = ftol(pos->y);
        jo::view<std::int32_t>(jo::as_shot(rec)->state.cell_pos[2]) = ftol(pos->z);
        joan::orig::as_actor(actor)->set_position_5ac310(joan::orig::arg::mptr(pos));  // 0x005ac310
        return 1;
    }

    // 0x006ad980 (ret 8); eax at exit reproduced (callers ignore it).
    std::uintptr_t set_active(std::int32_t id, std::int16_t on) {
        c_set_active.hit();
        std::uint8_t* p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(img::obj(kPlayers))->list.ptr_at(joan::orig::arg::i32(id)));  // 0x006af310
        if (id != 0 && static_cast<std::int32_t>(on) != 0) {
            const std::uint32_t busy = joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(&jo::self<jo::Players>(this)->list)->empty_at(joan::orig::arg::i32(id)));  // 0x006b0790
            if (busy != 0) return busy;
            p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(&jo::self<jo::Players>(this)->list)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3f0
            if (jo::view<std::int32_t>(jo::as_shot(p)->active_link) != 0) return reinterpret_cast<std::uintptr_t>(p);
            joan::orig::dynarray_b(jo::self<jo::Players>(this)->active)->linkin(&p);                             // 0x005857a0
            const std::uint32_t slot = joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(jo::self<jo::Players>(this)->active)->cursor());  // 0x00417520
            jo::as_shot(p)->active_link = slot;
            return slot;
        }
        if (id == 0) return reinterpret_cast<std::uintptr_t>(p);
        // on == 0
        jo::BodyState* const q = &jo::as_shot(p)->state;
        jo::view<std::int32_t>(jo::as_shot(p)->blocked) = 0;
        if (jo::view<std::int32_t>(jo::as_shot(p)->active_link) == 0) return reinterpret_cast<std::uintptr_t>(p);
        jo::view<std::int16_t>(jo::as_shot(p)->hit) = 0;
        jo::view<std::int32_t>(jo::as_body_state(q)->move_len) = 0;
        jo::view<std::int16_t>(jo::as_body_state(q)->move_flag) = 0;
        jo::view<std::int32_t>(jo::as_body_state(q)->vel_len) = 0;
        jo::view<std::int16_t>(jo::as_body_state(q)->vel_flag) = 0;
        const Fpu fpu{true};
        jo::as_body_state(q)->pos_z = static_cast<float>(
            fpu.div(fpu.mul(jo::view<float>(jo::as_body_state(q)->cell), global<float>(d::flt_1)), global<float>(d::flt_46_1)));
        jo::view<std::int16_t>(jo::as_body_state(q)->on_ground) = 1;
        jo::as_body_state(q)->vec0_x = global<std::uint32_t>(d::g_pos);
        jo::view<std::uint32_t>(jo::as_body_state(q)->vec0_y) = global<std::uint32_t>(d::g_a69cdc);
        jo::view<std::uint32_t>(jo::as_body_state(q)->vec0_z) = global<std::uint32_t>(d::g_a69ce0);
        jo::view<std::uint32_t>(jo::as_body_state(q)->vec_c[0]) = global<std::uint32_t>(d::g_pos);
        jo::view<std::uint32_t>(jo::as_body_state(q)->vec_c[1]) = global<std::uint32_t>(d::g_a69cdc);
        jo::view<std::uint32_t>(jo::as_body_state(q)->vec_c[2]) = global<std::uint32_t>(d::g_a69ce0);
        const std::uint32_t r = thiscall<std::uint32_t>(fn::linkout_5858c0, jo::self<jo::Players>(this)->active, jo::as_shot(p)->active_link);  // 0x005858c0
        jo::view<std::int32_t>(jo::as_shot(p)->active_link) = 0;
        return r;
    }
};

#define U5(name, addr, fn) JOAN_REPLACE("game/units5", name, addr, fn)
U5("PhysicSystem::move_actor", 0x006aee40, &H::place);
U5("PhysicSystem::set_object_physic_enabled", 0x006ad980, &H::set_active);
#undef U5

}  // namespace game_units5
}  // namespace

// ---------------------------------------- from modules/game_b111_15.cpp (fix/uninit)
namespace {
namespace game_b111_15 {

JOAN_COUNTER(c_fire, "Army::fire_shot");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x006adbf0 (fix)
    static double fabs_(float v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(v)))); }
    // larger magnitude of a / b as the original picks it: |b| < |a| (or NaN) -> |a| else |b|
    static float max_abs(const u8* o, i32 a, i32 b) {  // a, b: indices into the actor's aim_box
        const double fa = fabs_(jo::as_actor(o)->aim_box[a]);
        const double fb = fabs_(jo::as_actor(o)->aim_box[b]);
        const double r = !(fb >= fa) ? fabs_(jo::as_actor(o)->aim_box[a]) : fabs_(jo::as_actor(o)->aim_box[b]);
        return static_cast<float>(r);
    }
    u32 fire_shot(u8* o, u32 a, u32 type, u32 skip, u32 tag, u32 sound) {
        c_fire.hit();
        float ext[3] = {0.0f, 0.0f, 0.0f};  // fix/uninit: shape > 3 leaves the original's garbage
        switch (jo::as_actor(o)->aim_mode) {
            case 0:
                break;
            case 1:
                std::memcpy(&ext[0], &jo::as_actor(o)->aim_sphere[3], 4);
                std::memcpy(&ext[1], &jo::as_actor(o)->aim_sphere[3], 4);
                std::memcpy(&ext[2], &jo::as_actor(o)->aim_sphere[3], 4);
                break;
            case 2:
                ext[0] = max_abs(o, 3, 0);
                ext[1] = max_abs(o, 4, 1);
                ext[2] = max_abs(o, 5, 2);
                break;
            case 3:
                std::memcpy(&ext[1], &jo::as_actor(o)->collide_radius, 4);
                std::memcpy(&ext[0], &jo::as_actor(o)->collide_radius, 4);
                ext[2] = jo::as_actor(o)->collide_height / 2.0f;  // [0x9cf794] = 2.0f
                break;
            default:
                break;
        }
        float pos[3] = {0.0f, 0.0f, 0.0f};  // all three written by 0x0060c310
        joan::orig::as_actor(o)->position(joan::orig::arg::mptr(static_cast<void*>(pos)));  // 0x005ac270
        if (static_cast<i16>(skip) != 0) return 0xfffffffeu;
        const u32 id = joan::orig::arg::as<u32>(joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->add_new(static_cast<void*>(this), static_cast<void*>(o), joan::orig::arg::i32(a), static_cast<void*>(ext), static_cast<i32>(static_cast<i16>(type)), static_cast<i32>(static_cast<i16>(sound))));  // 0x006b2040
        u8* const shot = joan::orig::arg::as<u8*>(joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->get(joan::orig::arg::u32(id)));  // 0x006b0760
        jo::as_shot(shot)->tag = static_cast<u16>(tag);
        jo::view<i32>(jo::as_shot(shot)->state.cell_pos[0]) = joan::hook::x87::ftol(static_cast<double>(pos[0]));
        jo::view<i32>(jo::as_shot(shot)->state.cell_pos[1]) = joan::hook::x87::ftol(static_cast<double>(pos[1]));
        jo::view<i32>(jo::as_shot(shot)->state.cell_pos[2]) = joan::hook::x87::ftol(static_cast<double>(pos[2]));
        if (static_cast<i16>(sound) != 0) joan::orig::as_players(this)->set_active(joan::orig::arg::i32(id), joan::orig::arg::i16(1));  // 0x006ad980
        return id;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// extent vector for shape > 3 -> (0, 0, 0)
FIX("PhysicSystem::add_object", 0x006adbf0, &H::fire_shot);
#undef FIX

}  // namespace game_b111_15
}  // namespace

// ---------------------------------------- from modules/game_camera.cpp (game/camera1)
namespace {
namespace game_camera {

struct H {
    void* list_b20_get(std::int32_t i) {                                                    // 0x006af310
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(&jo::self<jo::Players>(this)->list)->empty_at_h(joan::orig::arg::i32(i))) != 0) return nullptr;     // 0x006b0790
        return joan::orig::arg::as<void*>(joan::orig::as_list(&jo::self<jo::Players>(this)->list)->get_41b3f0(joan::orig::arg::i32(i)));
    }
};

#define C1(name, addr, fn) JOAN_REPLACE("game/camera1", name, addr, fn)
C1("PhysicSystem::get_rigid_object_ptr", 0x006af310, &H::list_b20_get);
#undef C1

}  // namespace game_camera
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Players::set_active(std::int32_t a0, std::int16_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_units5::H::set_active>(this, a0, a1)); }  // 0x6ad980
auto joan::orig::Army::fire_shot(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_15::H::fire_shot>(this, a0, a1, a2, a3, a4, a5)); }  // 0x6adbf0
auto joan::orig::Players::place(void* a0, void* a1) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_units5::H::place>(this, a0, a1)); }  // 0x6aee40
auto joan::orig::Players::move_actor(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_players1_2::H::move_actor>(this, a0, a1, a2, a3)); }  // 0x6af190
auto joan::orig::Players::get(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_camera::H::list_b20_get>(this, a0)); }  // 0x6af310
