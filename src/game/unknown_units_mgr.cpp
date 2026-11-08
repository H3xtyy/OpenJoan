// game/unknown_units_mgr.cpp: functions whose original source file is unknown (formerly game/_unplaced/units_mgr.cpp;
// re/attribution.csv: between ww/unit.cpp and ww/unitres.cpp). They move to their original file when evidence names it
// (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using i32 = std::int32_t;

using joan::hook::global;

using joan::call::cdecl_call;
using joan::img::obj;

using i16 = std::int16_t;
using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
namespace {
namespace game_b88_05 {


JOAN_COUNTER(c_units_remove, "Units::remove");

struct H {
    // ------------------------------------------------------------ 0x0051e1f0 (ret 4)
    std::uintptr_t units_remove(std::int32_t id) {
        c_units_remove.hit();
        void* const unit = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        if (unit != nullptr) joan::orig::as_unit(unit)->destroy(1u);  // 0x00471530
        const std::uint32_t r = thiscall<std::uint32_t>(fn::linkout_5858c0, this, id);  // 0x005858c0
        if (id != static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->i16_2e))) return r;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->i16_2e) = 0;
        return reinterpret_cast<std::uintptr_t>(this);

    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("Units::remove", 0x0051e1f0, &H::units_remove);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b110_13.cpp (game/b110_13)
namespace {
namespace game_b110_13 {

using joan::hook::x87::Fpu;

using i8 = std::int8_t;
using u16 = std::uint16_t;

using joan::hook::x87::ftol;

using u32 = std::uint32_t;
using u64 = std::uint64_t;

using joan::orig::helpers::gf;   // float constant in .rdata
using joan::orig::helpers::gd;  // double constant in .rdata

const Fpu fpu{true};

JOAN_COUNTER(c_units, "UnitManager::update_units_51e280");

// ---- 0x0051e280 -------------------------------------------------------------------------------
void clock_now(u64* out) { joan::orig::as_profiler(obj(d::g_cpu_usage))->read_tsc(joan::orig::arg::mptr(out)); }  // 0x0045e750
float clock_elapsed(u64* d) { return joan::orig::arg::as<float>(joan::orig::as_timer(obj(d::g_cpu_usage))->elapsed_ms(joan::orig::arg::mptr(d))); }  // 0x0045e7f0
u8* unit_at(void* self, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(self)->ptr_at(joan::orig::arg::i32(i))); }  // 0x0041b3f0
void unit_phase(u8* u, i32 phase) { joan::orig::as_unit(u)->update_52c490(joan::orig::arg::i32(phase)); }      // 0x0052c490
bool is_player_unit(const u8* u) { return jo::as_unit(u)->world == global<jo::World*>(d::g_world); }

// Unit class filter of one round-robin pass (+0x22 class, optional +0x104 test).
enum class Cls { Two, Above2, Low104Ge4, Low104Lt4, Three, Above3, Low1 };
bool class_match(const u8* u, Cls c) {
    const i32 k = jo::as_unit(u)->move_mode;
    switch (c) {
        case Cls::Two: return k > 1 && k <= 2;
        case Cls::Above2: return k > 2;
        case Cls::Low104Ge4: return k <= 1 && jo::as_unit(u)->rank >= 4;
        case Cls::Low104Lt4: return k <= 1 && jo::as_unit(u)->rank < 4;
        case Cls::Three: return k > 2 && k <= 3;
        case Cls::Above3: return k > 3;
        case Cls::Low1: return k <= 1;
    }
    return false;
}

struct Pass {
    i32 slot;              // UnitManager::counts_a[slot]: next unit index, counts_b[slot]: units per pass
    Cls cls;
    i32 phase;             // -1: phases 0 and 1
    img::Addr mul;    // float constant
    img::Addr cap;    // float constant (minimum budget)
};

void round_robin(u8* self, i32 n, const u64& t_start, u64& t_now, const Pass& ps) {
    i16& cursor = jo::self<jo::UnitManager>(self)->counts_a[ps.slot];
    i16& budget = jo::self<jo::UnitManager>(self)->counts_b[ps.slot];
    i32 idx = cursor < n ? static_cast<i32>(cursor) : n;
    cursor = static_cast<i16>(idx);
    i32 cnt = 0;
    for (;;) {
        u8* u = unit_at(self, idx);
        if (u != nullptr && is_player_unit(u) && class_match(u, ps.cls)) {
            if (ps.phase < 0) {
                unit_phase(u, 0);
                unit_phase(u, 1);
            } else {
                unit_phase(u, ps.phase);
            }
            ++cnt;
        }
        idx = static_cast<i32>(static_cast<u32>(idx) + 1u);
        if (idx > n) idx = 1;
        if (static_cast<i32>(cursor) == idx) break;
        if (cnt > static_cast<i32>(budget)) break;
    }
    cursor = static_cast<i16>(idx);
    clock_now(&t_now);
    u64 d = t_now - t_start;
    const float el = clock_elapsed(&d);
    if (el < 0.0f || el > 0.0f) {  // test ah,0x40 jne: equal or unordered skips
        const double v = fpu.div(fpu.mul(static_cast<double>(cnt), gf(ps.mul)), el);
        const float cap = gf(ps.cap);
        const float r = v > static_cast<double>(cap) ? static_cast<float>(v) : cap;  // NaN -> cap
        jo::view<u16>(budget) = static_cast<u16>(joan::hook::x87::ftol(r));
    }
}

const Pass kPasses[12] = {
    {0, Cls::Two, -1, d::flt_1, d::flt_50},        // phases 0+1, cap 50
    {1, Cls::Above2, -1, d::flt_1, d::flt_50},     // phases 0+1, cap 50
    {2, Cls::Low104Ge4, 2, d::flt_1, d::flt_1},   // cap 1
    {3, Cls::Low104Lt4, 2, d::flt_1, d::flt_10},   // cap 10
    {4, Cls::Two, 2, d::flt_1, d::flt_10},
    {5, Cls::Three, 2, d::flt_1, d::flt_10},
    {6, Cls::Above3, 2, d::flt_0_5, d::flt_10},      // x 0.5
    {7, Cls::Low104Ge4, 3, d::flt_1, d::flt_1},   // cap 1
    {8, Cls::Low1, 3, d::flt_1, d::flt_3},        // cap 3
    {9, Cls::Two, 3, d::flt_1, d::flt_3},
    {10, Cls::Three, 3, d::flt_1, d::flt_3},
    {11, Cls::Above3, 3, d::flt_0_5, d::flt_3},      // x 0.5
};

struct H {
    void update_units() {  // 0x0051e280
        c_units.hit();
        u8* self = reinterpret_cast<u8*>(this);
        if (global<i32>(d::g_a06bf4) == 0) return;
        u64 t0 = 0;
        clock_now(&t0);
        if (global<i8>(d::g_a596ab) != 0) return;
        if (joan::orig::arg::as<u8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) != 0) {  // 0x004301c0
            for (i32 i = joan::orig::as_dynarray(self)->size(); i > 0; --i) {  // 0x00417ad0
                u8* u = unit_at(self, i);
                if (u == nullptr) continue;
                if (jo::as_unit(u)->frozen != 0) {
                    jo::as_unit(u)->move_mode = 1;
                    unit_phase(u, 0);
                    unit_phase(u, 1);
                    unit_phase(u, 2);
                    unit_phase(u, 3);
                } else if (joan::orig::arg::as<u8>(joan::orig::as_game(u)->byte_2e1()) == 0) {  // 0x00432b90
                    joan::orig::as_unit(u)->play_idle(i32{0}, i32{1});  // 0x00508b90
                    unit_phase(u, 0);
                    unit_phase(u, 1);
                }
            }
            return;
        }
        const i32 n = joan::orig::as_dynarray(self)->size();  // 0x00417ad0
        if (n == 0) return;
        u64 t_start = 0, t_now = 0;
        clock_now(&t_start);
        for (i32 i = 1; i <= n; ++i) {
            u8* u = unit_at(self, i);
            if (u == nullptr) continue;
            if (!is_player_unit(u)) continue;
            jo::as_unit(u)->move_mode = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->distance_band());  // 0x00508880
            bool due = false;
            if (jo::as_unit(u)->layer != 0 && jo::as_unit(u)->hit_points <= 0 && jo::as_unit(u)->state_120 == 0 &&
                jo::as_unit(u)->markers != -2) {
                u8* q = jo::view<u8*>(jo::as_unit(u)->actor);
                if (q == nullptr) {
                    due = true;
                } else {
                    const float rate = jo::as_actor(q)->fade;
                    if (!(rate < 0.0f || rate > 0.0f)) {  // test ah,0x40 jne: equal or unordered
                        due = true;
                    } else {
                        const u32 age = global<u32>(d::g_now) - jo::as_actor(q)->fade_38;
                        const double aged = static_cast<double>(age);  // fild qword {age, 0}
                        const double a = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::as_actor(q)->fade))));
                        const double lim = fpu.div(gd(d::dbl_1000), a);  // fdivr 1000.0
                        due = !(lim >= aged);  // test ah,1 je: below or unordered
                    }
                }
            }
            if (due) {
                if (joan::orig::arg::as<u8*>(joan::orig::as_unit(u)->leader()) != nullptr) {  // 0x004216d0
                    const i32 id = jo::as_unit(u)->id;
                    u8* g = joan::orig::arg::as<u8*>(joan::orig::as_unit(u)->leader());  // 0x004216d0
                    joan::orig::as_group(jo::as_unit(g)->group)->remove_member(joan::orig::arg::i32(id));  // 0x005288f0
                }
                joan::orig::as_units(self)->remove_51e1f0(joan::orig::arg::i32(i));  // 0x0051e1f0
                continue;
            }
            if (jo::as_unit(u)->move_mode <= 1) {
                unit_phase(u, 0);
                unit_phase(u, 1);
            }
        }
        clock_now(&t_now);
        u64 d = t_now - t_start;
        clock_elapsed(&d);
        clock_now(&t_start);
        for (int k = 0; k < 12; ++k) {
            if (k > 0) clock_now(&t_start);
            round_robin(self, n, t_start, t_now, kPasses[k]);
        }
        d = t_now - t0;
        clock_elapsed(&d);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_13", name, addr, fn)
B("UnitManager::update_units_51e280", 0x0051e280, &H::update_units);
#undef B

}  // namespace game_b110_13
}  // namespace

// ---------------------------------------- from modules/game_b86_02.cpp (game/b86_02)
namespace {
namespace game_b86_02 {

JOAN_COUNTER(c_find_3b, "Units::find_by_3b");

struct H {
    // ---- 0x0051fd50 Units::find_by_3b(value) -------------------------------------------------
    std::int32_t find_by_3b(std::int32_t value) {
        c_find_3b.hit();
        for (std::int32_t id = joan::orig::as_dynarray(this)->size(); id > 0; --id) {  // 0x00417ad0
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(id)) != 0) continue;  // 0x00428b70
            const std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
            if (static_cast<std::int32_t>(jo::as_unit(u)->type_id) == value) return id;
        }
        return 0;
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_02", name, addr, fn)
B86("Units::find_by_3b", 0x0051fd50, &H::find_by_3b);
#undef B86

}  // namespace game_b86_02
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

std::int32_t stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_find_name, "List::find_name");

struct H {
    // 0x0051fdd0
    std::int32_t find_name(const char* name) {
        c_find_name.hit();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (p == nullptr) continue;
            if (stricmp_(jo::as_unit(p)->name_, name) == 0) return i;
        }
        return 0;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("List::find_name", 0x0051fdd0, &H::find_name);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

JOAN_COUNTER(c_find_31, "List::find_by_31");

struct H {
    // ------------------------------------------------------------------------ 0x0051fe60
    void* find_by_31(i32 id) {
        c_find_31.hit();
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00428b70
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3c0
            if (static_cast<i32>(jo::as_unit(e)->group_id) == id && joan::orig::as_unit(e)->leader() != nullptr)  // 0x004216d0
                return joan::orig::as_unit(e)->leader();
        }
        return nullptr;

    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("List::find_by_31", 0x0051fe60, &H::find_by_31);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_collect, "Units::collect");

struct H {
    // ------------------------------------------------------------------------ 0x0051ff00
    static void scan_list(void* unit, jo::Grid* jo::UnitData::*field) {
        auto list = [unit, field]() -> void* { return jo::self<jo::UnitData>(jo::as_unit(unit)->modifiers)->*field; };
        for (i32 j = joan::orig::as_dynarray(list())->size(); j > 0; --j) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(list())->is_empty_at(joan::orig::arg::i32(j))) != 0) continue;   // 0x00457f60
            i16* const e = joan::orig::arg::as<i16*>(joan::orig::as_ptr_list(list())->get(joan::orig::arg::i32(j)));      // 0x00457e90
            const i16* const t = joan::orig::arg::as<i16*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(*e))));  // 0x00425ce0
            if (static_cast<i32>(*t) == 0xf && global<i32>(d::g_mission) != 5)
                joan::orig::as_item_list(list())->remove(static_cast<i32>(*e), 0);  // 0x00458430
        }
    }
    void collect() {
        c_collect.hit();
        void* const out = joan::orig::arg::as<void*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
        if (out == nullptr) return;
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00428b70
            void* const u = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3c0
            if (jo::as_unit(u)->rank < 4) continue;
            if (jo::as_unit(u)->modifiers != nullptr) {
                if (jo::as_modifiers(jo::as_unit(u)->modifiers)->grid_16 != nullptr) scan_list(u, &jo::UnitData::grid_16);
                if (jo::as_modifiers(jo::as_unit(u)->modifiers)->grid_1a != nullptr) scan_list(u, &jo::UnitData::grid_1a);
            }
            joan::orig::as_profile(out)->attach_unit(joan::orig::arg::mptr(u));  // 0x004c90a0
        }
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("Units::collect", 0x0051ff00, &H::collect);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_refresh_all, "List::refresh_all");

struct H {
    // 0x0051fce0
    i32 refresh_all() {
        c_refresh_all.hit();
        i32 n = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        for (; n > 0; --n) {
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(n)) != 0) continue;  // 0x00428b70
            void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(n));  // 0x0041b3c0
            joan::orig::as_unit(e)->refresh();                            // 0x00509800
        }
        return n;
    }
};

JOAN_REPLACE(M, "List::refresh_all", 0x0051fce0, &H::refresh_all);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Units::remove_51e1f0(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_05::H::units_remove>(this, a0)); }  // 0x51e1f0
auto joan::orig::Units::find_by_3b(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b86_02::H::find_by_3b>(this, a0)); }  // 0x51fd50
auto joan::orig::List::find_name_51fdd0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::find_name>(this, a0)); }  // 0x51fdd0
auto joan::orig::List::find_by_31(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_02::H::find_by_31>(this, a0)); }  // 0x51fe60
auto joan::orig::Units::collect() -> void { joan::bind::member<&::game_b98_04::H::collect>(this); }  // 0x51ff00
