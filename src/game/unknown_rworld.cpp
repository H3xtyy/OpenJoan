// game/unknown_rworld.cpp: functions whose original source file is unknown (formerly game/_unplaced/rworld.cpp;
// re/attribution.csv: between ww/enphysic/physys.cpp and ww/enphysic/rbody.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
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

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::hook::global;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;

using joan::hook::x87::ftol;

using joan::img::obj;

// Players::cells (RWorldCell[27]).
jo::RWorldCell* player_cells(const void* players) { return jo::self<jo::Players>(players)->cells; }

struct V3 {
    float x, y, z;
};

using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

using joan::orig::helpers::memset_;

// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

struct H {

    // 0x006aced0
    void* ctor_6aced0() {
        joan::orig::as_players(this)->construct_base();                                          // 0x006b0720
        joan::orig::as_move_solver(&jo::self<jo::Players>(this)->move_solver)->construct();  // 0x006b07c0
        joan::orig::as_collision_grid(&jo::self<jo::Players>(this)->grid)->construct();  // 0x0068db10
        joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->construct();  // 0x006b1e70
        memset_(this, 0, offsetof(jo::Players, list));  // everything before the record list (0xb20 on x86)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("init_6b0630", 0x006b0630, &static_init<fn::init_a6a528>);
B("init_6b0680", 0x006b0680, &static_init<fn::init_third_b>);
B("init_6b06d0", 0x006b06d0, &static_init<fn::init_a6a530>);
// Larger.
B("Global::ctor_6aced0", 0x006aced0, &H::ctor_6aced0);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {


static void JOAN_CDECL init_a6a530() { global<float>(d::g_a6a530) = global<float>(d::g_a6a524) / global<float>(d::flt_4); }  // 0x006b06f0

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("init_a6a530", 0x006b06f0, &init_a6a530);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b104_13.cpp (game/b104_13)
namespace {
namespace game_b104_13 {



struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }


    // ------------------------------------------------------------------ 0x006b0720
    void* self_6b0720() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_13", name, addr, fn)
B("Obj6b0720::self", 0x006b0720, &H::self_6b0720);
#undef B

}  // namespace game_b104_13
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

struct H {
    u8* self() { return static_cast<u8*>(static_cast<void*>(this)); }

    // ------------------------------------------------------------------ 0x006ad220
    void* scalar_dtor_6ad220(u32 flags) {
        joan::orig::dynarray_b(this)->destruct();  // 0x00585710
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }

    // ------------------------------------------------------------------ 0x006b0890
    void* dtor_6b0890() {
        joan::orig::as_quaternion(jo::self<jo::BodyState>(this)->orient)->construct();  // 0x005ac230
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj6ad220::scalar_dtor", 0x006ad220, &H::scalar_dtor_6ad220);
B97("Obj6b0890::~dtor", 0x006b0890, &H::dtor_6b0890);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_players1.cpp (game/players1)
namespace {
namespace game_players1 {

constexpr img::Addr kRandom(d::g_misc);

JOAN_COUNTER(c_add_cell, "Players::add_cell");
JOAN_COUNTER(c_remove_cell, "Players::remove_cell");

struct H {
    // 0x006b0760 RecordList::get(id) (ret 4).
    void* list_get(std::uint32_t id) {
        return joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3f0
    }

    // 0x006ad270 Players::add_cell(a, b, x, y, box) (ret 0x14). eax: this when (a, b) is
    // already present, else the result of 0x0068dbf0.
    std::uintptr_t add_cell(jo::PathSpace* a, jo::PathSpace* b, std::int32_t x, std::int32_t y, const void* box) {
        c_add_cell.hit();
        std::int32_t i = 0;
        for (; i < jo::self<jo::Players>(this)->cell_count; ++i) {
            if (player_cells(this)[i].key_a == a && player_cells(this)[i].key_b == b) break;
        }
        if (i < jo::self<jo::Players>(this)->cell_count) return reinterpret_cast<std::uintptr_t>(this);
        player_cells(this)[jo::self<jo::Players>(this)->cell_count].key_a = a;
        player_cells(this)[jo::self<jo::Players>(this)->cell_count].key_b = b;
        player_cells(this)[jo::self<jo::Players>(this)->cell_count].x = x;
        player_cells(this)[jo::self<jo::Players>(this)->cell_count].y = y;
        player_cells(this)[jo::self<jo::Players>(this)->cell_count].fx = static_cast<float>(x);  // fild / fstp dword
        player_cells(this)[jo::self<jo::Players>(this)->cell_count].fy = static_cast<float>(y);
        player_cells(this)[jo::self<jo::Players>(this)->cell_count].u32_18 = 0;
        std::memcpy(player_cells(this)[jo::self<jo::Players>(this)->cell_count].box, box, 24);  // rep movsd x6
        jo::self<jo::Players>(this)->cell_count = jo::self<jo::Players>(this)->cell_count + 1;
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_collision_grid(&jo::self<jo::Players>(this)->grid)->add(a, b, joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::mptr(box)));  // 0x0068dbf0
    }

    // 0x006ad450 Players::remove_cell(a, b) (ret 8). eax: this when found or when the list is
    // empty, else 0 (the last index product (1-1)*0x34).
    std::uintptr_t remove_cell(jo::PathSpace* a, jo::PathSpace* b) {
        c_remove_cell.hit();
        std::int32_t i = jo::self<jo::Players>(this)->cell_count;
        if (i <= 0) return reinterpret_cast<std::uintptr_t>(this);
        for (; i > 0; --i) {
            if (player_cells(this)[i - 1].key_a != a) continue;
            if (player_cells(this)[i - 1].key_b != b) continue;
            for (std::int32_t j = joan::orig::arg::as<std::int32_t>(joan::orig::as_players(this)->record_count()); j > 0; --j) {  // 0x006b0800 record count
                void* const rec = joan::orig::as_players(this)->list.ptr_at(joan::orig::arg::i32(j));  // 0x006af310
                if (rec == nullptr) continue;
                if (jo::as_shot(rec)->id == 0) continue;
                if (jo::as_shot(rec)->state.sub == a || jo::as_shot(rec)->state.sub == b) {
                    jo::as_shot(rec)->state.sub = nullptr;
                    jo::as_shot(rec)->state.zone = 0;
                }
            }
            joan::orig::as_collision_grid(&jo::self<jo::Players>(this)->grid)->remove(a, b);  // 0x0068dd80
            joan::orig::as_misc(img::obj(kRandom))->del_array_rec_591070(joan::orig::arg::str(player_cells(this)), joan::orig::arg::i32(jo::self<jo::Players>(this)->cell_count), joan::orig::arg::i32(sizeof(jo::RWorldCell)), joan::orig::arg::i32(i));  // 0x00591070 array remove
            jo::self<jo::Players>(this)->cell_count = jo::self<jo::Players>(this)->cell_count - 1;
            return reinterpret_cast<std::uintptr_t>(this);
        }
        return 0;
    }
};

#define P1(name, addr, fn) JOAN_REPLACE("game/players1", name, addr, fn)
P1("RecordList::get", 0x006b0760, &H::list_get);
P1("Players::add_cell", 0x006ad270, &H::add_cell);
P1("Players::remove_cell", 0x006ad450, &H::remove_cell);
#undef P1

}  // namespace game_players1
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

JOAN_COUNTER(c_release_slots, "Obj6a::release_slots");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    void release_slots() {  // 0x006ad5d0
        c_release_slots.hit();
        for (i32 n = jo::self<jo::Players>(this)->cell_count; n > 0; --n) {
            jo::PathSpace* const b = player_cells(this)[n - 1].key_b;
            jo::PathSpace* const a = player_cells(this)[n - 1].key_a;
            joan::orig::as_collision_grid(&jo::self<jo::Players>(this)->grid)->remove(a, b);  // 0x0068dd80
        }
        jo::self<jo::Players>(this)->cell_count = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6a::release_slots", 0x006ad5d0, &H::release_slots);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

JOAN_COUNTER(c_form, "Formation::update_6ad660");

struct H {
    // ------------------------------------------------------------------ 0x006ad660
    std::uintptr_t formation_update_6ad660() {  // eax at exit: this / 0 (callers ignore it)
        c_form.hit();
        jo::self<jo::Players>(this)->u32_58a = 0;
        const u32 now = global<u32>(d::g_now);
        if (jo::self<jo::Players>(this)->last_time == 0) {
            jo::self<jo::Players>(this)->last_time = now;
            return reinterpret_cast<std::uintptr_t>(this);
        }
        if (jo::self<jo::Players>(this)->running == 0) {
            jo::self<jo::Players>(this)->last_time = now;
            return 0;
        }
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_players(this)->record_count()); i > 0; --i) {  // 0x006b0800
            void* const m = joan::orig::as_players(this)->list.ptr_at(joan::orig::arg::i32(i));  // 0x006af310
            if (m != nullptr && jo::as_shot(m)->actor != nullptr && jo::as_shot(m)->actor->scene != nullptr)
                jo::view<u32>(jo::as_scene(jo::as_shot(m)->actor->scene)->light_capacity) = 0;
        }
        for (i32 i = joan::orig::as_dynarray(jo::self<jo::Players>(this)->active)->size(); i > 0; --i) {  // 0x00417ad0
            void* e = joan::orig::as_dynarray(jo::self<jo::Players>(this)->active)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (e == nullptr || jo::as_shot(e)->active_link == 0 || jo::as_shot(e)->id == 0) continue;
            joan::orig::as_body(e)->pull_position();  // 0x006b18a0
            const u32 type = jo::view<u32>(jo::as_shot(e)->type);
            const float dt = static_cast<float>(static_cast<double>(now - jo::self<jo::Players>(this)->last_time));
            if (type == 2 || type == 3 || type == 4 || type == 5 || type == 0x12)
                joan::orig::as_move_solver(&jo::self<jo::Players>(this)->move_solver)->ballistic_step(joan::orig::arg::mptr(e), joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::u32(now), joan::orig::arg::f32(dt), joan::orig::arg::mptr(&jo::self<jo::Players>(this)->grid));  // 0x006a8060
            else
                joan::orig::as_move_solver(&jo::self<jo::Players>(this)->move_solver)->form_step(joan::orig::arg::mptr(e), joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::u32(now), joan::orig::arg::f32(dt), joan::orig::arg::mptr(&jo::self<jo::Players>(this)->grid));  // 0x006a45a0
            e = joan::orig::as_dynarray(jo::self<jo::Players>(this)->active)->ptr_at(joan::orig::arg::i32(i));
            if (e == nullptr || jo::as_shot(e)->active_link == 0 || jo::as_shot(e)->id == 0) continue;
            joan::orig::as_body(e)->sync_position();  // 0x006b1350
        }
        jo::self<jo::Players>(this)->last_time = now;
        return now;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Formation::update_6ad660", 0x006ad660, &H::formation_update_6ad660);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {

struct H {
    // 0x006ad900
    void* set_timer(std::int16_t v) {
        jo::self<jo::Players>(this)->running = v;
        if (v != 0) jo::self<jo::Players>(this)->last_time = global<std::uint32_t>(d::g_now);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Obj::set_timer", 0x006ad900, &H::set_timer);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_units5.cpp (game/units5)
namespace {
namespace game_units5 {


JOAN_COUNTER(c_find_slot, "Players::find_slot");

float K0() { return global<float>(d::k_9cf5c4); }  // 0.0f

void* grid_layer(void* grid, std::int16_t side) { return joan::orig::arg::as<void*>(joan::orig::as_grid_cell(grid)->layer(joan::orig::arg::i16(side))); }  // 0x006aac10

// Stage two of 0x006ae8e0: a10 / 0x6ae8e0's kinds that only need an upward normal.
bool strict_kind(std::int32_t k) {
    return k == 9 || k == 0xa || k == 0xb || k == 0xd || k == 0xe || k == 0xf || k == 0x10 || k == 0x11 || k == 0x13;
}

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // 0x006ae8e0 (ret 0x24). zone_io: the caller's 32-bit zone index (Body / BodyState::zone, an i32 local; 0 =
    // none), read and written as a dword by the original and by the callees 0x006aae70 / 0x006ab1b0.
    std::int16_t find_slot(void** cell_io, std::int32_t* zone_io, std::int16_t side, float x, float y, float z, std::int32_t a20,
                           float tolerance, std::int32_t kind) {
        c_find_slot.hit();
        std::int32_t out = 0;                     // -0x8
        void* grid = nullptr;                     // -0xc
        V3 n{0.0f, 0.0f, 1.0f};                   // -0x18
        const std::int32_t no_slot = *zone_io == 0 ? 1 : 0;  // -0x1c
        if (*cell_io != nullptr) {
            grid = joan::orig::arg::as<void*>(joan::orig::placements::find(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::self<jo::Players>(this)->cell_count), static_cast<jo::PathSpace*>(*cell_io)));  // 0x004a2b20
            V3 d;
            d.x = x - jo::as_rworld_cell(grid)->fx;
            d.y = y - jo::as_rworld_cell(grid)->fy;
            d.z = z;
            for (std::int32_t i = 0; i <= 3; ++i) {
                const std::int32_t zone = *zone_io;
                void* const layer = grid_layer(grid, side);
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(layer)->find_zone(&out, zone, d, joan::orig::arg::u32(a20), 0, joan::orig::arg::i32(i))) == 0) continue;  // 0x00673d00
                const float h = joan::orig::arg::as<float>(joan::orig::as_terrain(grid_layer(grid, side))->height_66cae0(joan::orig::arg::i32(out), joan::orig::arg::f32(d.x), joan::orig::arg::f32(d.y)));       // 0x0066cae0
                if (!(h - tolerance > K0()) || z > h) {
                    *zone_io = out;
                    return 1;
                }
                void* const layer3 = grid_layer(grid, side);
                void* const tile = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(layer3)->zones)->item(joan::orig::arg::i32(out)));  // 0x00637da0
                joan::orig::as_terrain_block(tile)->normal(&n, joan::orig::arg::i32(ftol(d.x)), joan::orig::arg::i32(ftol(d.y)));                           // 0x006645b0
                if (strict_kind(kind)) {
                    const double up = joan::orig::arg::as<double>(joan::orig::grid::slope(&n));  // 0x006accf0
                    if (up > static_cast<double>(K0())) {
                        *zone_io = out;
                        return 1;
                    }
                    break;
                }
                const double up = joan::orig::arg::as<double>(joan::orig::grid::slope(&n));  // 0x006accf0
                if (up > static_cast<double>(K0()) || static_cast<std::int32_t>(side) != 0) {
                    *zone_io = out;
                    return 1;
                }
                break;
            }
            *zone_io = 0;
        }
        if (*cell_io != nullptr && *zone_io != 0) return 1;

        void* saved = grid;  // -0x3c
        std::int32_t cand = 0;  // -0x38 (written by the callee)
        std::memset(&cand, 0, sizeof cand);
        joan::orig::find_zone_pair_6aae70(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::self<jo::Players>(this)->cell_count), joan::orig::arg::bits(x), joan::orig::arg::bits(y), joan::orig::arg::bits(z), joan::orig::arg::u32(a20), joan::orig::arg::u32(side), &grid, &cand, 0u);  // 0x006aae70
        if (saved == grid && no_slot == 0) cand = 0;
        std::int32_t best = 0;       // -0x40
        void* best_layer = nullptr;  // -0x44
        if (cand != 0) {
            void* const layer = grid_layer(grid, side);  // -0x48
            const Fpu fpu{true};
            const float ry = static_cast<float>(fpu.sub(y, static_cast<double>(jo::as_rworld_cell(grid)->y)));
            const float rx = static_cast<float>(fpu.sub(x, static_cast<double>(jo::as_rworld_cell(grid)->x)));
            const double h = joan::orig::arg::as<double>(joan::orig::as_terrain(layer)->height_66cae0(cand, joan::orig::arg::f32(rx), joan::orig::arg::f32(ry)));  // 0x0066cae0
            const double diff = fpu.sub(h, z);
            if (joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(diff))) > global<double>(d::dbl_30)) {  // fabs, 30.0
                best = cand;
                *zone_io = cand;
                best_layer = layer;
                cand = 0;
            }
        }
        *zone_io = cand;
        if (*zone_io != 0) {
            *cell_io = grid_layer(grid, side);
            return 1;
        }
        saved = grid;
        std::int32_t local4c;  // -0x4c, out of the callee
        std::memset(&local4c, 0, sizeof local4c);
        joan::orig::zones::find_path_6ab1b0(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::self<jo::Players>(this)->cell_count), joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::f32(z), joan::orig::arg::i32(a20), joan::orig::arg::i32(side), &grid, zone_io, &local4c);  // 0x006ab1b0
        if (saved == grid) *zone_io = 0;
        if (*zone_io != 0) {
            *cell_io = grid_layer(grid, side);
            return 1;
        }
        if (best != 0 && saved == grid) {
            *zone_io = best;
            if (*zone_io != 0) {
                *cell_io = best_layer;
                return 1;
            }
        }
        *cell_io = nullptr;
        *zone_io = 0;
        return 0;
    }

    // 0x006adba0 (ret 4)
    std::int16_t is_active(std::int32_t id) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->get(joan::orig::arg::u32(id)));  // 0x006b0760
        return jo::view<std::int32_t>(jo::as_shot(p)->active_link) != 0 ? 1 : 0;
    }
};

#define U5(name, addr, fn) JOAN_REPLACE("game/units5", name, addr, fn)
U5("Players::find_slot", 0x006ae8e0, &H::find_slot);
U5("Players::is_active", 0x006adba0, &H::is_active);
#undef U5

}  // namespace game_units5
}  // namespace

// ---------------------------------------- from modules/game_units4.cpp (game/units4)
namespace {
namespace game_units4 {

struct H {
    // 0x006adf70 SoundSet::remove(id)
    std::uint32_t sound_remove(std::int32_t id) {
        if (static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_players(this)->is_active(joan::orig::arg::i32(id)))) != 0)  // 0x006adba0
            joan::orig::as_players(this)->set_active(joan::orig::arg::i32(id), joan::orig::arg::i16(0));                                              // 0x006ad980
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->remove(joan::orig::arg::i32(id)));                // 0x006b2130
    }
};

#define U4(name, addr, fn) JOAN_REPLACE("game/units4", name, addr, fn)
U4("SoundSet::remove", 0x006adf70, &H::sound_remove);
#undef U4

}  // namespace game_units4
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using joan::orig::helpers::as_float;

JOAN_COUNTER(c_pick, "Picker::pick");

static void JOAN_CDECL init_third_b() { global<float>(d::g_a6a52c) = global<float>(d::g_a6a524) / global<float>(d::flt_3); }

struct H {
    // zone: the caller's 32-bit zone index (BodyState::zone), as Players::find_slot's zone_io.
    void pick(float* out, u32* angle, void** object, std::int32_t* zone, u32 a18, u32 a1c, u32 a20, u32 a24, u32 a28,
              i32 a2c) {  // 0x006adfd0
        c_pick.hit();
        (void)a2c;
        const joan::hook::x87::Fpu fpu{true};
        joan::orig::as_players(this)->find_slot(joan::orig::arg::mptr(object), zone, joan::orig::arg::i16((a24 & 0xffff0000u) | (a18 & 0xffffu)), joan::orig::arg::f32(a1c), joan::orig::arg::f32(a20), joan::orig::arg::f32(a24), joan::orig::arg::i32(a28), joan::orig::arg::f32(*angle), joan::orig::arg::i32(a2c));  // 0x006ae8e0
        if (object != nullptr && *zone != 0) {
            u8* const r = joan::orig::arg::as<u8*>(joan::orig::placements::find(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Players>(this)->cell_count)), static_cast<jo::PathSpace*>(*object)));  // 0x004a2b20
            const float y = static_cast<float>(fpu.sub(as_float(a20), static_cast<double>(jo::as_rworld_cell(r)->y)));
            const float x = static_cast<float>(fpu.sub(as_float(a1c), static_cast<double>(jo::as_rworld_cell(r)->x)));
            const float a = joan::orig::arg::as<float>(joan::orig::as_terrain(*object)->height_66cae0(*zone, joan::orig::arg::f32(x), joan::orig::arg::f32(y)));  // 0x0066cae0
            std::memcpy(angle, &a, 4);
            void* const q = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(*object)->zones)->item(*zone));  // 0x00637da0
            const i32 iy = joan::hook::x87::ftol(fpu.sub(as_float(a20), static_cast<double>(jo::as_rworld_cell(r)->y)));
            const i32 ix = joan::hook::x87::ftol(fpu.sub(as_float(a1c), static_cast<double>(jo::as_rworld_cell(r)->x)));
            joan::orig::as_terrain_block(q)->normal(joan::orig::arg::mptr(out), joan::orig::arg::i32(ix), joan::orig::arg::i32(iy));  // 0x006645b0
            // The three a2c branches (9..0x13 subsets / others) are identical.
            if (joan::orig::arg::as<double>(joan::orig::grid::slope(joan::orig::arg::mptr(out))) > 0.0) {  // 0x006accf0
                out[0] = 0.0f;
                out[1] = 0.0f;
                out[2] = 1.0f;
            }
            return;
        }
        if (jo::self<jo::Players>(this)->no_ground != 0) {
            *object = nullptr;
            *zone = 0;
            *angle = 0xc59c4000u;
        } else {
            *object = nullptr;
            *zone = 0;
            *angle = 0;
        }
        out[0] = 0.0f;
        out[1] = 0.0f;
        out[2] = 1.0f;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Picker::pick", 0x006adfd0, &H::pick);
B94("init_third_b", 0x006b06a0, &init_third_b);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_camera.cpp (game/camera1)
namespace {
namespace game_camera {

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    std::int32_t list_empty_at(std::int32_t i) { return joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_41b3f0(joan::orig::arg::i32(i))) == nullptr ? 1 : 0; }  // 0x006b0790
};

#define C1(name, addr, fn) JOAN_REPLACE("game/camera1", name, addr, fn)
C1("List::empty_at_h", 0x006b0790, &H::list_empty_at);
#undef C1

}  // namespace game_camera
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using i8 = std::int8_t;

JOAN_COUNTER(c_path_link, "Path::link_6af710");

struct H {
    // ------------------------------------------------------------------ 0x006af710
    // Returns 0 when an existing link (+0x20e / +0x212) already matches; `want` = 0x006b1dd0 value.
    bool link_exists(u32 key, const void* node, i32 want) {
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_players(this)->record_count()); i > 0; --i) {  // 0x006b0800
            void* const o = joan::orig::as_players(this)->list.ptr_at(joan::orig::arg::i32(i));  // 0x006af310
            if (o == nullptr) continue;
            if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_body(o)->side())) != want) continue;  // 0x006b1dd0
            if (jo::as_shot(o)->state.zone != key) continue;
            if (jo::as_shot(o)->state.sub != node) continue;
            return true;
        }
        return false;
    }

    // out_obj: the grid cell found (a pointer, as Players::find_slot's `grid`); out_key: its zone index.
    u32 query_6aae70(u32 a8, u32 ac, u32 a10, i32 zero, i32 side, void** out_obj, u32* out_key) {
        return joan::orig::arg::as<u32>(joan::orig::find_zone_pair_6aae70(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Players>(this)->cell_count)), joan::orig::arg::u32(a8), joan::orig::arg::u32(ac), joan::orig::arg::u32(a10), joan::orig::arg::u32(zero), joan::orig::arg::u32(side), joan::orig::arg::mptr(// 0x006aae70
                               out_obj), joan::orig::arg::mptr(out_key), joan::orig::arg::u32(i32{1})));
    }

    void mark_6af710(void* path, u32 key, i32 mark) {
        void* const z = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(path)->zones)->item(joan::orig::arg::i32(key)));  // 0x00637da0
        jo::as_pfd_zone(z)->blocked = mark;
        joan::orig::as_path_space(path)->group_zones();  // 0x00675660
    }

    u32 path_link(u32 a8, u32 ac, u32 a10, i32 a14, i32 a18, void** out) {
        c_path_link.hit();
        void* lc = nullptr;  // grid cell
        u32 l8 = 0;
        *out = nullptr;
        const i32 mark = static_cast<i32>(static_cast<i8>(a18));
        if (a14 == -1) {
            void* l18 = nullptr;  // grid cells
            void* l1c = nullptr;
            u32 l14 = 0;
            u32 l10 = 0;
            query_6aae70(a8, ac, a10, 0, 0, &l1c, &l14);
            query_6aae70(a8, ac, a10, 0, 1, &l18, &l10);
            if (l10 != 0) {
                void* const n = joan::orig::as_grid_cell(l18)->layer(joan::orig::arg::i16(i32{1}));  // 0x006aac10
                if (link_exists(l10, n, 1)) return 0;
            }
            if (l14 != 0) {
                void* const n = joan::orig::as_grid_cell(l1c)->layer(joan::orig::arg::i16(i32{0}));
                if (link_exists(l14, n, 0)) return 0;
            }
            if (l10 != 0) {
                void* const p = joan::orig::as_grid_cell(l18)->layer(joan::orig::arg::i16(i32{1}));
                mark_6af710(p, l10, mark);
            }
            if (l14 != 0) {
                void* const p = joan::orig::as_grid_cell(l1c)->layer(joan::orig::arg::i16(i32{0}));
                mark_6af710(p, l14, mark);
            }
            if (l10 != 0) {
                *out = joan::orig::as_grid_cell(l18)->layer(joan::orig::arg::i16(i32{1}));
                return l10;
            }
            if (l14 != 0) {
                *out = joan::orig::as_grid_cell(l18)->layer(joan::orig::arg::i16(i32{0}));  // [ebp-0x18] as in the original
                return l14;
            }
            return 0;
        }
        const i32 wa = static_cast<i32>(static_cast<i16>(a14));
        const u32 r = query_6aae70(a8, ac, a10, 0, wa, &lc, &l8);
        if (l8 == 0) return r;
        void* const n = joan::orig::as_grid_cell(lc)->layer(joan::orig::arg::i16(wa));
        if (link_exists(l8, n, a14)) return 0;
        mark_6af710(n, l8, mark);
        *out = n;
        return l8;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Path::link_6af710", 0x006af710, &H::path_link);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {

// 24-byte key passed by value (bit copy).
struct Key24 {
    std::uint32_t d[6];
};
static_assert(sizeof(Key24) == 24, "Key24");
// DynArray (d:\ww\ilib\dynarr.cpp, ctor 0x005843d0 writes up to +0x1c).
struct alignas(4) DynArray {
    std::uint8_t b[((sizeof(jo::DynArray) > 0x20 ? sizeof(jo::DynArray) : 0x20) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger DynArray
};

JOAN_COUNTER(c_assign, "Formation::assign_matches");

void dyn_ctor(void* a) { joan::orig::dynarray(a)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x1e, 0); }  // 0x005843d0 (pointer lists read with ptr_at: 4 bytes on x86)
void dyn_dtor(void* a) { joan::orig::dynarray(a)->destruct(); }                // 0x005844b0
void dyn_add(void* a, void* e) { joan::orig::dynarray(a)->linkin(joan::orig::arg::ptr(e)); }  // 0x00584620
std::int32_t dyn_count(void* a) { return joan::orig::as_dynarray(a)->size(); }  // 0x00417ad0
void* dyn_get(void* a, std::int32_t i) { return joan::orig::as_dynarray(a)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

struct H {
    // 0x006ab4e0 (cdecl, 14 dwords): next match after *cursor, writes *cursor / *id.
    std::int32_t next_match(const Key24& key, void* matrix, std::int32_t sel, void** cursor,
                            std::uint32_t* id, std::uint8_t flag) {
        return joan::orig::arg::as<std::int32_t>(joan::orig::players::find_match_6ab4e0(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::self<jo::Players>(this)->cell_count), key, matrix, joan::orig::arg::u32(sel), joan::orig::arg::mptr(cursor), joan::orig::arg::mptr(id), 1u, joan::orig::arg::u32(static_cast<std::uint32_t>(flag))));
    }
    void refresh(jo::PathSpace* layer) {
        joan::orig::as_path_space(layer)->group_zones();  // 0x00675660
    }

    // ------------------------------------------------------------------ 0x006afb10
    std::int32_t assign_matches(Key24 key, void* matrix, std::int32_t sel, std::int8_t mode,  // x64: the matrix is a pointer (a dword on x86)
                                std::uint8_t flag) {
        c_assign.hit();
        if (sel == -1) {
            DynArray l30, l50;
            dyn_ctor(&l30);
            dyn_ctor(&l50);
            void* cur5c = nullptr;
            void* cur60 = nullptr;
            std::uint32_t id58 = 0, id54 = 0;
            std::uint8_t ok64 = 1;
            while (next_match(key, matrix, 0, &cur60, &id58, flag) != 0) {
                void* o68 = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(cur60)->layer(joan::orig::arg::i16(static_cast<std::int16_t>(0))));  // 0x006aac10
                void* o6c = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(o68)->zones)->item(joan::orig::arg::i32(id58)));           // 0x00637da0
                if (static_cast<std::int32_t>(mode) == 1 && jo::as_pfd_zone(o6c)->blocked != 1) {
                    for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_players(this)->record_count()); i > 0; --i) {  // 0x006b0800
                        void* q = joan::orig::as_players(this)->list.ptr_at(joan::orig::arg::i32(i));  // 0x006af310
                        if (q != nullptr) {
                            const std::int16_t s = joan::orig::arg::as<std::int16_t>(joan::orig::as_body(q)->side());  // 0x006b1dd0
                            if (static_cast<std::int32_t>(s) == 0 && jo::as_shot(q)->state.zone == id58 &&
                                joan::orig::arg::as<void*>(jo::as_shot(q)->state.sub) == o68) {
                                ok64 = 0;
                                break;
                            }
                        }
                    }
                    if (ok64 == 0) break;
                    dyn_add(&l50, &o6c);
                } else if (static_cast<std::int32_t>(mode) != 1 && jo::as_pfd_zone(o6c)->blocked == 1) {
                    dyn_add(&l50, &o6c);
                }
            }
            std::uint8_t ok78 = 1;
            while (next_match(key, matrix, 1, &cur5c, &id54, flag) != 0) {
                void* o7c = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(cur5c)->layer(joan::orig::arg::i16(static_cast<std::int16_t>(1))));  // 0x006aac10
                void* o80 = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(o7c)->zones)->item(joan::orig::arg::i32(id54)));           // 0x00637da0
                if (static_cast<std::int32_t>(mode) == 1 && jo::as_pfd_zone(o80)->blocked != 1) {
                    for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_players(this)->record_count()); i > 0; --i) {  // 0x006b0800
                        void* q = joan::orig::as_players(this)->list.ptr_at(joan::orig::arg::i32(i));  // 0x006af310
                        if (q != nullptr) {
                            const std::int16_t s = joan::orig::arg::as<std::int16_t>(joan::orig::as_body(q)->side());  // 0x006b1dd0
                            if (static_cast<std::int32_t>(s) == 1 && jo::as_shot(q)->state.zone == id54 &&
                                joan::orig::arg::as<void*>(jo::as_shot(q)->state.sub) == o7c) {
                                ok78 = 0;
                                break;
                            }
                        }
                    }
                    if (ok78 == 0) break;
                    dyn_add(&l30, &o80);
                } else if (static_cast<std::int32_t>(mode) != 1 && jo::as_pfd_zone(o80)->blocked == 1) {
                    dyn_add(&l30, &o80);
                }
            }
            std::int32_t result;
            if (ok64 != 0 && ok78 != 0) {
                for (std::int32_t i = dyn_count(&l30); i > 0; --i) {
                    jo::as_pfd_zone(dyn_get(&l30, i))->blocked = static_cast<std::int32_t>(mode);
                }
                for (std::int32_t i = dyn_count(&l50); i > 0; --i) {
                    jo::as_pfd_zone(dyn_get(&l50, i))->blocked = static_cast<std::int32_t>(mode);
                }
                for (std::int32_t i = jo::self<jo::Players>(this)->cell_count - 1; i >= 0; --i) {
                    refresh(player_cells(this)[i].key_b);
                    if (player_cells(this)[i].key_b != player_cells(this)[i].key_a) refresh(player_cells(this)[i].key_a);
                }
                result = 1;
            } else {
                result = 0;
            }
            dyn_dtor(&l50);
            dyn_dtor(&l30);
            return result;
        }

        DynArray lb4;
        dyn_ctor(&lb4);
        void* curbc = nullptr;
        std::uint32_t idb8 = 0;
        std::uint8_t okc0 = 1;
        const std::int16_t sel16 = static_cast<std::int16_t>(sel);
        while (next_match(key, matrix, static_cast<std::int32_t>(static_cast<std::uint16_t>(sel16)), &curbc, &idb8,
                          flag) != 0) {
            void* oc4 = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(curbc)->layer(joan::orig::arg::i16(sel16)));                  // 0x006aac10
            void* oc8 = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(oc4)->zones)->item(joan::orig::arg::i32(idb8)));  // 0x00637da0
            if (static_cast<std::int32_t>(mode) == 1 && jo::as_pfd_zone(oc8)->blocked != 1) {
                for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_players(this)->record_count()); i > 0; --i) {  // 0x006b0800
                    void* q = joan::orig::as_players(this)->list.ptr_at(joan::orig::arg::i32(i));  // 0x006af310
                    if (q != nullptr) {
                        const std::int16_t s = joan::orig::arg::as<std::int16_t>(joan::orig::as_body(q)->side());  // 0x006b1dd0
                        if (static_cast<std::int32_t>(s) == sel && jo::as_shot(q)->state.zone == idb8 &&
                            joan::orig::arg::as<void*>(jo::as_shot(q)->state.sub) == oc4) {
                            okc0 = 0;
                            break;
                        }
                    }
                }
                if (okc0 == 0) break;
                dyn_add(&lb4, &oc8);
            } else if (static_cast<std::int32_t>(mode) != 1 && jo::as_pfd_zone(oc8)->blocked == 1) {
                dyn_add(&lb4, &oc8);
            }
        }
        std::int32_t result;
        if (okc0 != 0) {
            for (std::int32_t i = dyn_count(&lb4); i > 0; --i) {
                jo::as_pfd_zone(dyn_get(&lb4, i))->blocked = static_cast<std::int32_t>(mode);
            }
            if (sel != 0) {
                for (std::int32_t i = jo::self<jo::Players>(this)->cell_count - 1; i >= 0; --i) refresh(player_cells(this)[i].key_a);
            } else {
                for (std::int32_t i = jo::self<jo::Players>(this)->cell_count - 1; i >= 0; --i) refresh(player_cells(this)[i].key_b);
            }
            result = 1;
        } else {
            result = 0;
        }
        dyn_dtor(&lb4);
        return result;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("Formation::assign_matches", 0x006afb10, &H::assign_matches);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b88_04.cpp (game/b88_04)
namespace {
namespace game_b88_04 {

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_place_record, "Unit::place_record");

struct H {
    // ------------------------------------------------------------ 0x006b0450 (ret 4)
    void place_record(std::int32_t id) {
        c_place_record.hit();
        if (joan::orig::as_dynarray(&jo::self<jo::Players>(this)->list)->empty_at(joan::orig::arg::i32(id)) != 0) return;  // 0x006b0790
        std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_record_list(&jo::self<jo::Players>(this)->list)->get(joan::orig::arg::u32(id)));  // 0x006b0760
        void* const owner = jo::view<void*>(jo::as_shot(rec)->actor);                                             // ebp-0x18
        Vec3 pos;                                                                           // ebp-0x14
        joan::orig::as_actor(owner)->position(&pos);                                           // 0x005ac270
        void* grid = nullptr;                                                               // ebp-0x2c
        std::uint32_t cell = 0;                                                             // ebp-0x30 (the zone index written by 0x006aae70)
        const std::int16_t side = joan::orig::arg::as<std::int16_t>(joan::orig::as_body(rec)->side());                  // 0x006b1dd0
        const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::find_zone_pair_6aae70(joan::orig::arg::mptr(player_cells(this)), joan::orig::arg::i32(jo::self<jo::Players>(this)->cell_count), joan::orig::arg::bits(pos.x), joan::orig::arg::bits(pos.y), joan::orig::arg::bits(pos.z), 0u, joan::orig::arg::u32(static_cast<std::int32_t>(side)), &grid, &cell, 0u));  // 0x006aae70
        if (r == 0) return;
        const std::int16_t side2 = joan::orig::arg::as<std::int16_t>(joan::orig::as_body(rec)->side());  // 0x006b1dd0
        void* const layer = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(grid)->layer(joan::orig::arg::i16(static_cast<std::int32_t>(side2))));  // 0x006aac10
        const float h = joan::orig::arg::as<float>(joan::orig::as_terrain(layer)->height_66cae0(static_cast<std::int32_t>(cell), joan::orig::arg::f32(pos.x), joan::orig::arg::f32(pos.y)));                    // 0x0066cae0
        jo::as_shot(rec)->state.sub = layer;
        jo::as_shot(rec)->state.zone = cell;
        jo::view<float>(jo::as_shot(rec)->state.cell) = h;
        // fld z / fcomp h / test ah,0x41 jne -> 0: 1 only when z > h.
        jo::view<std::int16_t>(jo::as_shot(rec)->state.on_ground) = (pos.z > h) ? 1 : 0;
        std::memcpy(&jo::as_shot(rec)->saved, &jo::as_shot(rec)->state, sizeof(jo::BodyState));  // rep movsd 0x4d + movsw
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_04", name, addr, fn)
B("Unit::place_record", 0x006b0450, &H::place_record);
#undef B

}  // namespace game_b88_04
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

static u32 JOAN_CDECL init_6b05d0() { joan::orig::init_a6a524(); return 0u; }  // eax dead: _initterm reloads it

JOAN_REPLACE("game/b108_07", "init_6b05d0", 0x006b05d0, &init_6b05d0);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b111_11.cpp (game/b111_11)
namespace {
namespace game_b111_11 {

// ============================================================================ 0x006b05f0
void JOAN_CDECL init_a6a524() {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a6a524) = static_cast<float>(t * global<double>(d::dbl_4));  // * 4.0
}

#define G(name, addr, fn) JOAN_REPLACE("game/b111_11", name, addr, fn)
G("init_a6a524", 0x006b05f0, &init_a6a524);
#undef G

}  // namespace game_b111_11
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

// 0x006b0650
void JOAN_CDECL init_a6a528() { global<float>(d::g_a6a528) = global<float>(d::g_a6a524) / global<float>(d::flt_2); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("init_a6a528", 0x006b0650, &init_a6a528);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

struct H {
    // 0x006b0740 / 0x006b07e0
    void empty_6b0740() {}
    void empty_6b07e0() {}
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("empty_6b0740", 0x006b0740, &H::empty_6b0740);
B91("empty_6b07e0", 0x006b07e0, &H::empty_6b07e0);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

struct H {
    // ------------------------------------------------------------------------ 0x006b07c0
    void* self_6b07c0() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::self_6b07c0", 0x006b07c0, &H::self_6b07c0);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b82_12.cpp (game/b82_12)
namespace {
namespace game_b82_12 {

struct H {
    // 0x006b0800 (ret)
    std::uint32_t record_count() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(&jo::self<jo::Players>(this)->list)->size()); }  // 0x00417ad0

    // 0x006b08c0 (ret): empty.
    void nop() {}
};

#define B12(name, addr, fn) JOAN_REPLACE("game/b82_12", name, addr, fn)
B12("Players::record_count", 0x006b0800, &H::record_count);
B12("Record::nop", 0x006b08c0, &H::nop);
#undef B12

}  // namespace game_b82_12
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x006b0830
    H* ctor_6b0830() {
        joan::orig::as_body_state(&jo::self<jo::Shot>(this)->saved)->construct();    // 0x006b0890
        joan::orig::as_body_state(&jo::self<jo::Shot>(this)->state)->construct();  // 0x006b0890
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::Shot)));  // 0x2b2 on x86
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::ctor_6b0830", 0x006b0830, &H::ctor_6b0830);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Players::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b105_19::H::ctor_6aced0>(this)); }  // 0x6aced0
auto joan::orig::DynArrayB::scalar_delete(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_08::H::scalar_dtor_6ad220>(this, a0)); }  // 0x6ad220
auto joan::orig::Players::add_cell(PathSpace* a0, PathSpace* a1, std::int32_t a2, std::int32_t a3, void* a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_players1::H::add_cell>(this, a0, a1, a2, a3, a4)); }  // 0x6ad270
auto joan::orig::Players::remove_cell(PathSpace* a0, PathSpace* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_players1::H::remove_cell>(this, a0, a1)); }  // 0x6ad450
auto joan::orig::Players::release_slots() -> void { joan::bind::member<&::game_b93_06::H::release_slots>(this); }  // 0x6ad5d0
auto joan::orig::Players::set_timer(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::set_timer>(this, a0)); }  // 0x6ad900
auto joan::orig::Players::is_active(std::int32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_units5::H::is_active>(this, a0)); }  // 0x6adba0
auto joan::orig::Players::remove_record(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_units4::H::sound_remove>(this, a0)); }  // 0x6adf70
auto joan::orig::Players::pick(void* a0, void* a1, void* a2, void* a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::int32_t a9) -> void { joan::bind::member<&::game_b94_08::H::pick>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); }  // 0x6adfd0
auto joan::orig::Players::find_slot(void* a0, void* a1, std::int16_t a2, float a3, float a4, float a5, std::int32_t a6, float a7, std::int32_t a8) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_units5::H::find_slot>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x6ae8e0
auto joan::orig::Path::link_6af710(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::int32_t a3, std::int32_t a4, void* a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_11::H::path_link>(this, a0, a1, a2, a3, a4, a5)); }  // 0x6af710
auto joan::orig::Unit::place_record(std::int32_t a0) -> void { joan::bind::member<&::game_b88_04::H::place_record>(this, a0); }  // 0x6b0450
auto joan::orig::init_a6a524() -> void { joan::bind::free<&::game_b111_11::init_a6a524>(); }  // 0x6b05f0
auto joan::orig::Players::construct_base() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b104_13::H::self_6b0720>(this)); }  // 0x6b0720
auto joan::orig::Players::nop_6b0740() -> void { joan::bind::member<&::game_b91_04::H::empty_6b0740>(this); }  // 0x6b0740
auto joan::orig::RecordList::get(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_players1::H::list_get>(this, a0)); }  // 0x6b0760
auto joan::orig::List::empty_at_h(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_camera::H::list_empty_at>(this, a0)); }  // 0x6b0790
auto joan::orig::MoveSolver::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b103_15::H::self_6b07c0>(this)); }  // 0x6b07c0
auto joan::orig::MoveSolver::nop() -> void { joan::bind::member<&::game_b91_04::H::empty_6b07e0>(this); }  // 0x6b07e0
auto joan::orig::Players::record_count() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_12::H::record_count>(this)); }  // 0x6b0800
auto joan::orig::Body::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_10::H::ctor_6b0830>(this)); }  // 0x6b0830
auto joan::orig::BodyState::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_08::H::dtor_6b0890>(this)); }  // 0x6b0890
auto joan::orig::Body::destruct() -> void { joan::bind::member<&::game_b82_12::H::nop>(this); }  // 0x6b08c0
