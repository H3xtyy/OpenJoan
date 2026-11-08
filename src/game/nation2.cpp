// game/nation2.cpp: functions of the original d:\ww\nation2.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
//
//    - grow_array 0x0043fc10 (cdecl, ptr, short* capacity, short needed): when needed exceeds
//      the capacity, capacity = needed + needed / 2 + 7 and realloc (0x0058eb30, 2-byte
//      elements, "d:\ww\nation2.cpp", line 0xa01804 + 4). Returns the (new) pointer.
namespace {
namespace game_b88_05 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

void* const kMemory = obj(d::g_mem);     // 0x0058eb30 realloc

JOAN_COUNTER(c_grow, "grow_array");

// ------------------------------------------------------------ 0x0043fc10 (cdecl)
void* JOAN_CDECL grow_array(void* data, std::int16_t* capacity, std::int16_t needed) {
    c_grow.hit();
    const std::int32_t n = needed;
    if (n <= *capacity) return data;
    *capacity = static_cast<std::int16_t>(n + n / 2 + 7);
    return joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(data), joan::orig::arg::u32(static_cast<std::int32_t>(*capacity) * 2), joan::orig::arg::str(// 0x0058eb30
                           str(d::s_d_ww_nation2_cpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a01804)) + 4));
}

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("grow_array", 0x0043fc10, &grow_array);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ zero fills
    void* zero_1545() {  // 0x004b3db0
        memset_(this, 0, sizeof(jo::Nation));  // 0x1545 on x86
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Struct1545::zero", 0x004b3db0, &H::zero_1545);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

using joan::call::thiscall;

struct H {
    void wrap_4b3df0() { joan::orig::as_nation(this)->clear(); }  // 0x00499380
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Wrap::f_4b3df0", 0x004b3df0, &H::wrap_4b3df0);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::img::obj;

using u16 = std::uint16_t;

void* const kUnits = obj(d::g_units);
void* const kPlayers = obj(d::g_nations);

using joan::orig::helpers::at;
using joan::img::str;

using u32 = std::uint32_t;

void* const kText = obj(d::g_misc);

JOAN_COUNTER(c_nation_init, "Nation::init");
JOAN_COUNTER(c_apply_units, "Nation::apply_units");

char* strcpy_(void* d, const void* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }  // strcpy

i32 count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------------ 0x004b3e20
    void nation_init(u32 word_n) {
        c_nation_init.hit();
        const i16 n = static_cast<i16>(word_n);
        jo::self<jo::Nation>(this)->kind = n;
        const u8* const src = joan::orig::arg::as<const u8*>(joan::orig::as_nation_res(obj(d::g_nation_res))->entry(static_cast<i32>(n)));  // 0x004b4730
        strcpy_(jo::self<jo::Nation>(this)->name_, jo::as_nation_def(src)->text);
        jo::self<jo::Nation>(this)->state = 3;
        jo::self<jo::Nation>(this)->kind32 = static_cast<i32>(n);
        joan::orig::as_color(&jo::self<jo::Nation>(this)->colour)->set_bgr(joan::orig::arg::mptr(obj(d::g_init_white)));  // 0x005e9430
        for (i32 i = 1; i <= 0x14; ++i) {
            joan::orig::as_relation(&jo::self<jo::Nation>(this)->relation1 + (i - 1))->construct();  // 0x004b8c60
            if (joan::orig::arg::as<i32>(joan::orig::as_list(kPlayers)->is_null(joan::orig::arg::i32(i))) != 0) continue;  // 0x00484870
            u8* const pl = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kPlayers)->ptr_at(joan::orig::arg::i32(i)));       // 0x0047b600
            const i32 me = static_cast<i32>(jo::self<jo::Nation>(this)->id);
            joan::orig::as_relation(&jo::as_nation(pl)->relation1 + (me - 1))->construct();  // 0x004b8c60
        }
        jo::view<u32>(jo::self<jo::Nation>(this)->gold) = 0x459c4000;  // 5000.0f
        jo::view<u32>(jo::self<jo::Nation>(this)->mood) = 0x41200000;  // 10.0f
        jo::view<u32>(jo::self<jo::Nation>(this)->military_strength) = 0x32;
        jo::view<u32>(jo::self<jo::Nation>(this)->population) = 0;  // written three times by a loop in the original
        jo::view<u32>(jo::self<jo::Nation>(this)->population) = 0x64;
        jo::view<u32>(jo::self<jo::Nation>(this)->goods) = global<u32>(d::k_9d52ec);
        jo::view<u32>(jo::self<jo::Nation>(this)->demand) = global<u32>(d::blob_9d52f0);
        jo::view<u32>(jo::self<jo::Nation>(this)->price_trend) = global<u32>(d::k_9d52f4);
        jo::view<u32>(jo::self<jo::Nation>(this)->production) = global<u32>(d::k_9d52f8);
        jo::view<u32>(jo::self<jo::Nation>(this)->tax) = global<u32>(d::blob_9d52fc);
        jo::view<u32>(jo::self<jo::Nation>(this)->consume_limit) = global<u32>(d::k_9d5300);
        jo::view<u32>(jo::self<jo::Nation>(this)->own_tax) = global<u32>(d::k_9d5304);
        jo::view<u32>(jo::self<jo::Nation>(this)->import_tax) = global<u32>(d::blob_9d5308);
        jo::view<u32>(jo::self<jo::Nation>(this)->export_tax) = global<u32>(d::k_9d530c);
        jo::self<jo::Nation>(this)->traits[0] = joan::orig::arg::as<float>(joan::orig::as_misc(kText)->rand_unit_inclusive());  // 0x00590660
        jo::self<jo::Nation>(this)->traits[1] = joan::orig::arg::as<float>(joan::orig::as_misc(kText)->rand_unit_inclusive());
        jo::self<jo::Nation>(this)->traits[2] = joan::orig::arg::as<float>(joan::orig::as_misc(kText)->rand_unit_inclusive());
        jo::self<jo::Nation>(this)->traits[3] = joan::orig::arg::as<float>(joan::orig::as_misc(kText)->rand_unit_inclusive());
        jo::self<jo::Nation>(this)->traits[4] = joan::orig::arg::as<float>(joan::orig::as_misc(kText)->rand_unit_inclusive());
        jo::self<jo::Nation>(this)->traits[5] = joan::orig::arg::as<float>(joan::orig::as_misc(kText)->rand_unit_inclusive());
    }

    // ------------------------------------------------------------------------ 0x004b5170
    float year_value() {
        const Fpu fpu{true};
        const i32 d = 0x16d - global<i32>(d::g_a47bf0);
        double x = fpu.mul(static_cast<double>(d), jo::self<jo::Nation>(this)->f32_1481);
        x = fpu.div(x, 365.0f);  // 0x9d5338
        x = fpu.add(x, jo::self<jo::Nation>(this)->f32_147d);
        return static_cast<float>(x);
    }

    // ------------------------------------------------------------------------ 0x004b56a0
    void apply_units(u32 word_w) {
        c_apply_units.hit();
        global<i16>(d::g_a3ce48) = jo::self<jo::Nation>(this)->id;
        for (i32 i = count(kUnits); i > 0; --i) {
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (u == nullptr) continue;
            if (static_cast<i32>(jo::as_unit(u)->side) != static_cast<i32>(jo::self<jo::Nation>(this)->id)) continue;
            joan::orig::as_unit(u)->set_side(joan::orig::arg::i32(static_cast<u32>(static_cast<u16>(word_w))));  // 0x00509250
        }
        joan::orig::as_nation(this)->mark();  // 0x004b40d0
        global<i16>(d::g_a3ce48) = 0;
    }

    // ------------------------------------------------------------------------ 0x004b6420
    void* list4_ctor() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Nation::init", 0x004b3e20, &H::nation_init);
B99("Nation::year_value", 0x004b5170, &H::year_value);
B99("Nation::apply_units", 0x004b56a0, &H::apply_units);
B99("List4_14::ctor", 0x004b6420, &H::list4_ctor);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

using joan::orig::helpers::at;

struct H {
    // 0x004b62b0
    std::int32_t active() {
        if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->leader_id) != 0 || jo::self<jo::Unit>(this)->group != nullptr) return 1;
        return 0;
    }

    // 0x004b5520
    void add_1389(std::int32_t n) {
        jo::self<jo::Nation>(this)->military_strength = jo::self<jo::Nation>(this)->military_strength + n;
        if (jo::self<jo::Nation>(this)->military_strength < 0) jo::self<jo::Nation>(this)->military_strength = 0;
    }

    // 0x004b40d0
    void mark() { jo::self<jo::Nation>(this)->dirty = 1; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj4b6::active", 0x004b62b0, &H::active);
M("Obj4b5::add_1389", 0x004b5520, &H::add_1389);
M("Obj4b4::mark", 0x004b40d0, &H::mark);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

using joan::call::thiscall;

using joan::img::obj;

using joan::orig::helpers::at;

struct H {
    // 0x004b4120
    std::int32_t kind_word() {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_nation_res(obj(d::g_nation_res))->entry(static_cast<std::int32_t>(jo::self<jo::Nation>(this)->kind)));  // 0x004b4730
        return jo::view<std::int16_t>(jo::as_nation_def(p)->race);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Ref::kind_word", 0x004b4120, &H::kind_word);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using i32 = std::int32_t;


struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x004b4280
    void* addr_14() { return jo::self<jo::Nation>(this)->name_; }  // +0x14

    // 0x004b4360
    std::uint8_t byte_58(std::int16_t i) {
        // relation i (1-based, 0xf4-byte Rel records from +0x55), byte +3
        return static_cast<std::uint8_t>(jo::as_relation(&jo::self<jo::Nation>(this)->relation1 + (static_cast<std::int32_t>(i) - 1))->attitude);
    }

    // 0x004b6780
    std::int32_t is_unset(std::int32_t i) {
        const std::uint8_t* const r = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(self())->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
        if (r != nullptr && static_cast<std::int8_t>(r[4]) == 0) return 0;
        return 1;
    }

    // 0x004b8c40
    std::int32_t count_4() { return joan::orig::as_dynarray(&jo::self<jo::Talk>(this)->msgs)->size(); }  // 0x00417ad0 (Talk +4)
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::addr_14", 0x004b4280, &H::addr_14);
M("Table::byte_58", 0x004b4360, &H::byte_58);
M("List::is_unset", 0x004b6780, &H::is_unset);
M("Obj::count_4", 0x004b8c40, &H::count_4);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units {

using joan::call::thiscall;

struct H {
    // 0x004b6860 Players::hostile(a, b).
    std::int32_t players_hostile(std::int32_t a, std::int32_t b) {
        if (a == b) return 0;
        if (a == 0 || b == 0) return 1;
        void* const player = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_h(joan::orig::arg::i32(a)));         // 0x0047b600
        const jo::Relation* const entry = jo::as_relation(joan::orig::as_player(player)->relation(joan::orig::arg::i32(b)));  // 0x004b42e0
        return static_cast<std::int8_t>(entry->u8_1);
    }
    void* relation(std::int32_t i) { return &jo::self<jo::Nation>(this)->relation1 + (i - 1); }  // 0x004b42e0 (0xf4-byte Rel records from +0x55)
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("Players::hostile", 0x004b6860, &H::players_hostile);
U1("Player::relation", 0x004b42e0, &H::relation);
#undef U1

}  // namespace game_units
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::orig::helpers::at;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x004b44e0
    void* set_byte1(i32 id, i32 v) {
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->relation(static_cast<i32>(static_cast<i16>(id))));  // 0x004b42e0
        jo::as_relation(e)->u8_1 = static_cast<u8>(v);
        return e;
    }

    // ------------------------------------------------------------------ 0x004b52f0
    float year_value() {
        constexpr Fpu fpu{true};
        const i32 n = static_cast<i32>(0x16du - static_cast<u32>(global<i32>(d::g_a47bf0)));
        double v = fpu.mul(static_cast<double>(n), jo::self<jo::Nation>(this)->surplus_prev);
        v = fpu.div(v, global<float>(d::flt_365));
        v = fpu.add(v, jo::self<jo::Nation>(this)->surplus);
        return static_cast<float>(v);
    }

    // ------------------------------------------------------------------ 0x004b8310
    void nop() {}

    // ------------------------------------------------------------------ 0x004b8f20
    void* clear_8() {
        jo::self<jo::NationRes>(this)->loaded = 0;
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Table::set_byte1", 0x004b44e0, &H::set_byte1);
B97("Obj::year_value", 0x004b52f0, &H::year_value);
B97("Obj::nop_4b8310", 0x004b8310, &H::nop);
B97("Obj::clear_8", 0x004b8f20, &H::clear_8);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x004b4510
    u32 set_4b4510(i32 a) {
        return joan::orig::arg::as<u32>(joan::orig::as_nation(this)->set_relation_state(static_cast<i32>(static_cast<i16>(a)), i32{3}, i32{0}));  // 0x00499770
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Obj::set_4b4510", 0x004b4510, &H::set_4b4510);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    u32 set_4b4550(u32 a) {
        joan::orig::as_nation(this)->set_relation_state(static_cast<i32>(static_cast<i16>(a)), 4, 0);  // 0x00499770
        joan::orig::as_player(this)->set_relation(joan::orig::arg::i32(a), static_cast<char>(1));  // 0x004b4610
        return 0;  // callers discard the result
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("X::set_4b4550", 0x004b4550, &H::set_4b4550);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {

using joan::hook::global;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::img::obj;

void* const kPlayers = obj(d::g_nations);

using joan::orig::helpers::at;

std::uint8_t* player(std::int32_t i) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kPlayers)->ptr_at(joan::orig::arg::i32(i))); }  // 0x0047b600
std::uint8_t* relation(void* p, std::int32_t i) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_player(p)->relation(joan::orig::arg::i32(i))); }  // 0x004b42e0

JOAN_COUNTER(c_add_mood, "Player::add_mood");

struct H {
    // 0x004b4590
    std::int32_t is_3() { return static_cast<std::int32_t>(static_cast<std::int8_t>(jo::self<jo::Nation>(this)->state)) == 3 ? 1 : 0; }  // Nation::is_kind_3

    // 0x004b4610
    // eax: the other player's relation entry (0x004b42e0's result).
    std::uint8_t* set_relation(std::int32_t i, char v) {
        relation(this, i)[2] = static_cast<std::uint8_t>(v);
        const std::int32_t me = jo::self<jo::Nation>(this)->id;
        std::uint8_t* const e = relation(player(i), me);
        e[2] = static_cast<std::uint8_t>(v);
        return e;
    }

    // 0x004b4ce0
    void add_stat(std::int32_t i, float v, std::int32_t also) {
        jo::self<jo::Nation>(this)->gold = v + jo::self<jo::Nation>(this)->gold;
        jo::self<jo::Nation>(this)->income_by_kind[i] = v + jo::self<jo::Nation>(this)->income_by_kind[i];
        jo::self<jo::Nation>(this)->income_total = v + jo::self<jo::Nation>(this)->income_total;
        jo::self<jo::Nation>(this)->income = v + jo::self<jo::Nation>(this)->income;
        if (also != 0) jo::self<jo::Nation>(this)->trade_income = v + jo::self<jo::Nation>(this)->trade_income;
    }

    // 0x004b5420
    void add_mood(float v) {
        c_add_mood.hit();
        const float zero = global<float>(d::k_9cf5c4);
        const float k150 = global<float>(d::flt_9cfbe0);
        if (v > zero && jo::self<jo::Nation>(this)->mood > zero) v = (k150 - jo::self<jo::Nation>(this)->mood) * v / k150;
        jo::self<jo::Nation>(this)->mood = v + jo::self<jo::Nation>(this)->mood;
        if (jo::self<jo::Nation>(this)->mood > global<float>(d::flt_100)) jo::view<std::uint32_t>(jo::self<jo::Nation>(this)->mood) = 0x42c80000u;
        if (!(jo::self<jo::Nation>(this)->mood >= global<float>(d::flt_m100))) jo::view<std::uint32_t>(jo::self<jo::Nation>(this)->mood) = 0xc2c80000u;
        jo::self<jo::Nation>(this)->mood_delta = v + jo::self<jo::Nation>(this)->mood_delta;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Rel::is_3", 0x004b4590, &H::is_3);
B("Player::set_relation", 0x004b4610, &H::set_relation);
B("Player::add_stat", 0x004b4ce0, &H::add_stat);
B("Player::add_mood", 0x004b5420, &H::add_mood);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b91_01.cpp (game/b91_01)
namespace {
namespace game_b91_01 {

using joan::orig::helpers::at;

struct H {
    // 0x004b4730
    void* slot4e(std::int32_t i) {
        return static_cast<void*>(&jo::self<jo::NationRes>(this)->nations[i - 1]);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_01", name, addr, fn)
B("Table4e::slot", 0x004b4730, &H::slot4e);
#undef B

}  // namespace game_b91_01
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::img::obj;

void* const kUnits = obj(d::g_units);

using u32 = std::uint32_t;

using joan::orig::helpers::at;

JOAN_COUNTER(c_mark_4b8cf0, "Obj::mark_4b8cf0");
JOAN_COUNTER(c_remove_4b6710, "List::remove_4b6710");

// 0x004b5e10
i32 JOAN_CDECL compare_short_4b5e10(const i16* a, const i16* b) {
    return static_cast<i32>(*a) - static_cast<i32>(*b);
}

struct H {
    // (365 - day) as int (fild) * rate / 365.0f + base, x87 at PC24.
    static float yearly(const float& rate, const float& base) {
        const Fpu fpu{true};
        const i32 days = 0x16d - global<i32>(d::g_a47bf0);
        double v = fpu.mul(static_cast<double>(days), rate);
        v = fpu.div(v, global<float>(d::flt_365));  // 365.0
        v = fpu.add(v, base);
        return static_cast<float>(v);
    }

    // ------------------------------------------------------------------ 0x004b48d0
    void* unit_4b48d0() {  // the king unit (a pointer; no facade caller)
        if (joan::orig::as_dynarray(kUnits)->empty_at(static_cast<i32>(jo::self<jo::Nation>(this)->king_unit)) != 0) return nullptr;  // 0x00428b70
        return joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(jo::self<jo::Nation>(this)->king_unit));  // 0x0041b3c0
    }

    // ------------------------------------------------------------------ yearly getters
    float yearly_4b4ec0() { return yearly(jo::self<jo::Nation>(this)->income_total_prev, jo::self<jo::Nation>(this)->income_total); }
    float yearly_4b4f10(i32 i) { return yearly(jo::self<jo::Nation>(this)->income_by_kind_prev[i], jo::self<jo::Nation>(this)->income_by_kind[i]); }
    float yearly_4b50c0() { return yearly(jo::self<jo::Nation>(this)->spent_prev, jo::self<jo::Nation>(this)->spent); }
    float yearly_4b5110(i32 i) { return yearly(jo::self<jo::Nation>(this)->spend_by_kind_prev[i], jo::self<jo::Nation>(this)->spend_by_kind[i]); }
    float yearly_4b5200() { return yearly(jo::self<jo::Nation>(this)->time_net_prev, jo::self<jo::Nation>(this)->time_net); }
    float yearly_4b5250() { return yearly(jo::self<jo::Nation>(this)->mood_delta_prev, jo::self<jo::Nation>(this)->mood_delta); }
    float yearly_4b52a0() { return yearly(jo::self<jo::Nation>(this)->sold_prev, jo::self<jo::Nation>(this)->sold); }

    // a - b: a stored to a float local, then fsubr with b's st0.
    float diff(std::uintptr_t first, std::uintptr_t second) {
        const float a = thiscall<float>(first, this);
        const double b = thiscall<double>(second, this);
        return static_cast<float>(Fpu{true}.sub(a, b));
    }
    float diff_4b4e80() { return diff(fn::code_value(fn::Obj_year_frac), fn::code_value(fn::Calendar_value)); }  // 0x004b4d90 - 0x004b4de0
    float diff_4b51c0() { return diff(fn::code_value(fn::Economy_projected), fn::code_value(fn::Nation_year_value)); }  // 0x004b4e30 - 0x004b5170

    // ------------------------------------------------------------------ 0x004b6710
    u32 remove_4b6710(u32 key) {
        c_remove_4b6710.hit();
        void* const p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(key));  // 0x0047b600
        if (p != nullptr) joan::orig::as_nation(p)->destroy(1u);     // 0x004b65f0 (scalar deleting dtor)
        return thiscall<u32>(fn::linkout_5858c0, this, key);           // 0x005858c0
    }

    // ------------------------------------------------------------------ 0x004b8cf0
    i32 mark_4b8cf0(const i16* list) {
        c_mark_4b8cf0.hit();
        for (i32 i = 0; static_cast<i32>(list[i]) != 0; ++i) {
            at<u8>(this, static_cast<i32>(list[i]) + 0xcf) = 1;  // x64: needs Nation+0xd0 u8[20] (flags by id; overlaps relation1 now)
        }
        return 0;  // eax = the terminating 0 (movsx)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::unit_4b48d0", 0x004b48d0, &H::unit_4b48d0);
B("Economy::diff_4b4e80", 0x004b4e80, &H::diff_4b4e80);
B("Economy::yearly_4b4ec0", 0x004b4ec0, &H::yearly_4b4ec0);
B("Economy::yearly_4b4f10", 0x004b4f10, &H::yearly_4b4f10);
B("Economy::yearly_4b50c0", 0x004b50c0, &H::yearly_4b50c0);
B("Economy::yearly_4b5110", 0x004b5110, &H::yearly_4b5110);
B("Economy::diff_4b51c0", 0x004b51c0, &H::diff_4b51c0);
B("Economy::yearly_4b5200", 0x004b5200, &H::yearly_4b5200);
B("Economy::yearly_4b5250", 0x004b5250, &H::yearly_4b5250);
B("Economy::yearly_4b52a0", 0x004b52a0, &H::yearly_4b52a0);
B("compare_short_4b5e10", 0x004b5e10, &compare_short_4b5e10);
B("List::remove_4b6710", 0x004b6710, &H::remove_4b6710);
B("Obj::mark_4b8cf0", 0x004b8cf0, &H::mark_4b8cf0);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using i8 = std::int8_t;

using u32 = std::uint32_t;

using joan::orig::helpers::at;

JOAN_COUNTER(c_tick_4b4920, "Obj::tick_4b4920");

struct H {
    // ------------------------------------------------------------------ 0x004b4920
    void tick_4b4920() {
        c_tick_4b4920.hit();
        joan::orig::as_nation(this)->update_income();  // 0x004b5750
        joan::orig::as_nation(this)->age_entries();  // 0x004b5a90
        if (jo::view<i8>(jo::self<jo::Nation>(this)->at_war) != 0) jo::self<jo::Nation>(this)->war_day = global<u32>(d::g_get_a47bb0);
        jo::self<jo::Nation>(this)->was_at_war = jo::self<jo::Nation>(this)->at_war;
        jo::self<jo::Nation>(this)->at_war = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_nation(this)->is_kind_3()) != 0) {  // 0x004b4590
            joan::orig::as_nation(this)->scan_treaties();            // 0x004b7cb0
            const i32 d = static_cast<i32>(global<u32>(d::g_get_a47bb0) - static_cast<u32>(static_cast<i32>(jo::self<jo::Nation>(this)->id)));
            if (d % 10 == 0) joan::orig::as_nation(this)->nop_4b8310();  // 0x004b8310
            joan::orig::as_nation(this)->nop_4b8b20();                    // 0x004b8b20
            joan::orig::as_nation(this)->nop_4b8c20();                    // 0x004b8c20
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Obj::tick_4b4920", 0x004b4920, &H::tick_4b4920);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

struct H {
    void nop0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Obj::nop_4b49f0", 0x004b49f0, &H::nop0);
B("Obj::nop_4b8c20", 0x004b8c20, &H::nop0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b101_05.cpp (game/b101_05)
namespace {
namespace game_b101_05 {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::img::obj;

void* const kMath = obj(d::g_misc);

using u32 = std::uint32_t;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::orig::helpers::at;
// Word read that may hit a null pointer exactly where the original faults.
i32 read_short(const void* p) {
    return static_cast<i32>(*reinterpret_cast<const volatile i16*>(reinterpret_cast<std::uintptr_t>(p)));
}

void* const kTowns = obj(d::g_nation_res);
void* const kGroups = obj(d::g_types);
void* const kWorld = obj(d::g_nations);

i32 random(i32 n) { return joan::orig::as_misc(kMath)->random(joan::orig::arg::i32(n)); }  // 0x00590540

JOAN_COUNTER(c_roll_stats, "Town::roll_stats");
JOAN_COUNTER(c_add_all_buildings, "Town::add_all_buildings");
JOAN_COUNTER(c_relation_shift, "Town::random_relation_shift");
JOAN_COUNTER(c_count_strength, "Army::count_strength");

struct H {
    // ------------------------------------------------------------------ 0x004b4a10
    u32 roll_stats() {
        c_roll_stats.hit();
        jo::view<u32>(jo::self<jo::Nation>(this)->income_total_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->income_total);
        jo::view<u32>(jo::self<jo::Nation>(this)->income_total) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->spent_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->spent);
        jo::view<u32>(jo::self<jo::Nation>(this)->spent) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->trade_income_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->trade_income);
        jo::view<u32>(jo::self<jo::Nation>(this)->trade_income) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->f32_13a1) = jo::view<u32>(jo::self<jo::Nation>(this)->f32_139d);
        jo::view<u32>(jo::self<jo::Nation>(this)->f32_139d) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->income_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->income);
        jo::view<u32>(jo::self<jo::Nation>(this)->income) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->f32_1481) = jo::view<u32>(jo::self<jo::Nation>(this)->f32_147d);
        jo::view<u32>(jo::self<jo::Nation>(this)->f32_147d) = 0;
        for (i32 i = 0; i < 8; ++i) {
            jo::view<u32>(jo::self<jo::Nation>(this)->income_by_kind_prev[i]) = jo::view<u32>(jo::self<jo::Nation>(this)->income_by_kind[i]);
            jo::view<u32>(jo::self<jo::Nation>(this)->income_by_kind[i]) = 0;
        }
        for (i32 i = 0; i < 0x11; ++i) {
            jo::view<u32>(jo::self<jo::Nation>(this)->spend_by_kind_prev[i]) = jo::view<u32>(jo::self<jo::Nation>(this)->spend_by_kind[i]);
            jo::view<u32>(jo::self<jo::Nation>(this)->spend_by_kind[i]) = 0;
        }
        jo::view<u32>(jo::self<jo::Nation>(this)->time_gained_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->time_gained);
        jo::view<u32>(jo::self<jo::Nation>(this)->time_gained) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->time_transferred_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->time_transferred);
        jo::view<u32>(jo::self<jo::Nation>(this)->time_transferred) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->time_net_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->time_net);
        jo::view<u32>(jo::self<jo::Nation>(this)->time_net) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->sold_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->sold);
        jo::view<u32>(jo::self<jo::Nation>(this)->sold) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->surplus_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->surplus);
        jo::view<u32>(jo::self<jo::Nation>(this)->surplus) = 0;
        jo::view<u32>(jo::self<jo::Nation>(this)->mood_delta_prev) = jo::view<u32>(jo::self<jo::Nation>(this)->mood_delta);
        jo::view<u32>(jo::self<jo::Nation>(this)->mood_delta) = 0;
        for (i32 i = 0; i < 0x14; ++i) {
            joan::orig::as_relation(reinterpret_cast<u8*>(&jo::self<jo::Nation>(this)->relation1) + i * static_cast<i32>(sizeof(jo::Rel)))->roll_stats();  // 0x004b8ca0
        }
        return 0x14;
    }

    // ------------------------------------------------------------------ 0x004b64c0
    void add_all_buildings() {
        c_add_all_buildings.hit();
        for (i32 i = 1; i <= global<i32>(d::g_nation_res); ++i) {
            u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_nation_res(kTowns)->entry(joan::orig::arg::i32(i)));  // 0x004b4730
            joan::orig::as_nation_list(this)->add_nation(joan::orig::arg::i16(static_cast<i32>(jo::view<i16>(jo::as_nation_def(r)->id))));  // 0x004b6640
        }
    }

    // ------------------------------------------------------------------ 0x004b6a40
    i32 count_free() { return joan::orig::as_dynarray(this)->size(); }  // 0x00417ad0
    i32 slot_used(i32 i) { return joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_unset(joan::orig::arg::i32(i))); }             // 0x004b6780
    void* pair(i32 a) { return joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(a)); }  // 0x0047b600
    i32 pair_value(i32 a, i32 b) {
        void* const p = pair(a);
        u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_player(p)->relation(joan::orig::arg::i32(b)));  // 0x004b42e0
        return jo::as_relation(r)->opinion;
    }
    void random_relation_shift() {
        c_relation_shift.hit();
        i32 count = 0;
        for (i32 i = 1; i <= count_free(); ++i) {
            if (slot_used(i) == 0) ++count;
        }
        if (count <= 1) return;
        const i32 r1 = random(count) + 1;
        i32 a = 0;
        i32 k = 0;
        for (i32 i = 1; i <= count_free(); ++i) {
            if (slot_used(i) == 0) {
                ++k;
                if (k == r1) a = i;
            }
        }
        i32 r2 = random(count - 1) + 1;
        i32 b = 0;
        if (r2 >= r1) ++r2;
        k = 0;
        for (i32 i = 1; i <= count_free(); ++i) {
            if (slot_used(i) == 0) {
                ++k;
                if (k == r2) b = i;
            }
        }
        i32 va = pair_value(a, b);
        i32 vb = pair_value(b, a);
        const i32 avg = (va + vb) / 2;
        i32 delta;
        if (avg >= 0x14) {
            delta = global<i32>(d::g_a01808 + random(9) * 4);
        } else if (avg > -0x14) {
            delta = global<i32>(d::g_a0182c + random(9) * 4);
        } else {
            delta = global<i32>(d::g_a01850 + random(5) * 4);
        }
        va = joan::orig::arg::as<i32>(joan::orig::as_misc(kMath)->limit_int(joan::orig::arg::i32(va + delta), joan::orig::arg::i32(-100), 100));  // 0x00590aa0
        vb = joan::orig::arg::as<i32>(joan::orig::as_misc(kMath)->limit_int(joan::orig::arg::i32(vb + delta), joan::orig::arg::i32(-100), 100));  // 0x00590aa0
        void* const pa = pair(a);
        joan::orig::as_nation(pa)->set_relation_value(joan::orig::arg::i32(b), joan::orig::arg::u32(va));  // 0x0042a1c0
        void* const pb = pair(b);
        joan::orig::as_nation(pb)->set_relation_value(joan::orig::arg::i32(a), joan::orig::arg::u32(vb));  // 0x0042a1c0
    }

    // ------------------------------------------------------------------ shared cell walk
    // Lists the cells around +0x3a; for each non-null chain head (0x0054ee10) walks the +0x33d
    // ring of units, calling `visit` on the units that are active (+0x135 > 0 or +0x120 != 0)
    // and inside +0x3a (0x0060d5f0). visit returns true to stop (result stored in *out).
    void* own_side() {
        void* side = nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kWorld)->is_null(joan::orig::arg::i32(jo::self<jo::Place>(this)->nation))) == 0) {  // 0x00484870
            side = joan::orig::as_dynarray(kWorld)->ptr_at(joan::orig::arg::i32(jo::self<jo::Place>(this)->nation));  // 0x0047b600
        }
        return side;
    }
    template <class V>
    bool walk_cells(V visit, i32* out) {
        i32 n;
        void** const list = joan::orig::arg::as<void**>(joan::orig::as_world(jo::self<jo::Place>(this)->world)->cells_in_box(joan::orig::arg::mptr(jo::self<jo::Place>(this)->box), &n));  // 0x00429090
        for (i32 i = 0; i < n; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(list[i])->get_8c());  // 0x0054ee10
            if (first == nullptr) continue;
            u8* p = first;
            do {
                if (jo::as_unit(p)->hit_points > 0 || jo::as_unit(p)->state_120 != 0) {
                    const u32 in = joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::self<jo::Place>(this)->box)->contains(joan::orig::arg::ptr(jo::as_unit(p)->position_)));  // 0x0060d5f0
                    if ((in & 0xffu) != 0 && visit(p, out)) return true;
                }
                p = jo::view<u8*>(jo::as_unit(p)->cell_prev);
            } while (p != nullptr && p != first);
        }
        return false;
    }

    // ------------------------------------------------------------------ 0x004b79a0
    i32 count_strength(const char* id) {
        c_count_strength.hit();
        if (id == nullptr) return 0;
        const i32 t = joan::orig::arg::as<i32>(joan::orig::as_name_table(kTowns)->find_4b91a0(id));  // 0x004b91a0
        const i32 side = joan::orig::arg::as<i32>(joan::orig::as_list(kWorld)->find_by_word2(joan::orig::arg::i32(t)));  // 0x004b67e0
        if (jo::self<jo::Place>(this)->world == nullptr) return 0;
        i32 count = 0;
        void* const mine = own_side();
        walk_cells(
            [&](u8* p, i32*) {
                const i16 g16 = jo::as_unit(p)->type_id;
                u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kGroups)->get(joan::orig::arg::i16(g16)));  // 0x0042d0a0
                const i32 g2 = static_cast<i32>(jo::as_unit_type(g)->unit_class);
                if (g2 == 0 || g2 == 8 || g2 == 9) return false;
                const i32 g118 = static_cast<i32>(jo::as_unit_type(g)->kind);
                if (g118 == 7 || g118 == 2 || g118 == 3) return false;
                if (static_cast<i32>(jo::as_unit(p)->side) == side) ++count;
                return false;
            },
            nullptr);
        u8* q = joan::orig::arg::as<u8*>(joan::orig::as_place(this)->facility_head());  // 0x004c48b0
        if (read_short(mine) != side) return count;
        for (; q != nullptr; q = jo::view<u8*>(jo::as_facility(q)->prev)) {
            if (static_cast<i32>(jo::as_facility(q)->kind) != 1) continue;
            u8* const o = virtual_call<u8*>(q, 0x18);
            if (static_cast<i32>(jo::as_building(o)->unit_kind) != 0) ++count;
            for (i32 j = 0; j < static_cast<i32>((jo::as_building(virtual_call<u8*>(q, 0x18)))->queue_n); ++j) {
                u8* const o2 = virtual_call<u8*>(q, 0x18);
                count += static_cast<i32>(jo::as_building(o2)->queue_count[j]);
            }
        }
        return count;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_05", name, addr, fn)
B("Town::roll_stats", 0x004b4a10, &H::roll_stats);
B("Town::add_all_buildings", 0x004b64c0, &H::add_all_buildings);
B("Town::random_relation_shift", 0x004b6a40, &H::random_relation_shift);
B("Army::count_strength", 0x004b79a0, &H::count_strength);
#undef B

}  // namespace game_b101_05
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::orig::helpers::at;

using u32 = std::uint32_t;

JOAN_COUNTER(c_spend, "Obj::spend");

void* crt_memset(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // ------------------------------------------------------------------ 0x004b4d90
    float year_frac() {
        constexpr Fpu fpu{true};
        const i32 days = 0x16d - global<i32>(d::g_a47bf0);
        double x = fpu.mul(static_cast<double>(days), static_cast<double>(jo::self<jo::Nation>(this)->trade_income_prev));
        x = fpu.div(x, static_cast<double>(global<float>(d::flt_365)));
        x = fpu.add(x, static_cast<double>(jo::self<jo::Nation>(this)->trade_income));
        return static_cast<float>(x);
    }

    // ------------------------------------------------------------------ 0x004b5bf0
    i32 spend(i32 dry_run, i32 /*unused*/, i32 /*unused*/) {
        c_spend.hit();
        const float k1 = global<float>(d::flt_9d5314);
        const float k2 = global<float>(d::flt_1);
        // fcomp / test ah,1 jne fail: below or unordered -> 0.
        if (!(jo::self<jo::Nation>(this)->gold >= k1)) return 0;
        if (!(jo::self<jo::Nation>(this)->goods >= k2)) return 0;
        if (jo::self<jo::Nation>(this)->population < 1) return 0;
        if (dry_run == 0) {
            joan::orig::as_economy(this)->spend(9, joan::orig::arg::f32(k1), 0);  // 0x004b5010
            jo::self<jo::Nation>(this)->goods = jo::self<jo::Nation>(this)->goods - k2;
            jo::self<jo::Nation>(this)->population = jo::self<jo::Nation>(this)->population - 1;
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x004b8c60
    void* clear_f4() {
        void* const r = crt_memset(this, 0, sizeof(jo::Rel));  // 0xf4
        jo::view<u8>(jo::self<jo::Rel>(this)->attitude) = 2;
        return r;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::year_frac", 0x004b4d90, &H::year_frac);
B98("Obj::spend", 0x004b5bf0, &H::spend);
B98("Obj::clear_f4", 0x004b8c60, &H::clear_f4);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::orig::helpers::at;

using u32 = std::uint32_t;

JOAN_COUNTER(c_age_entries, "Calendar::age_entries");

struct H {
    // ------------------------------------------------------------------ 0x004b4de0
    float calendar_value() {
        const i32 d = 0x16d - global<i32>(d::g_a47bf0);
        const Fpu fpu{true};
        double v = fpu.mul(static_cast<double>(d), jo::self<jo::Nation>(this)->f32_13a1);
        v = fpu.div(v, global<float>(d::flt_365));
        v = fpu.add(v, jo::self<jo::Nation>(this)->f32_139d);
        return static_cast<float>(v);
    }

    // ------------------------------------------------------------------ 0x004b5a90
    void* age_entries() {
        c_age_entries.hit();
        const i32 cur = joan::hook::x87::ftol(jo::self<jo::Nation>(this)->goods);
        if (cur < jo::self<jo::Nation>(this)->population) jo::self<jo::Nation>(this)->population = cur;
        if (cur < jo::self<jo::Nation>(this)->population + jo::self<jo::Nation>(this)->recruit_count) jo::self<jo::Nation>(this)->recruit_count = cur - jo::self<jo::Nation>(this)->population;
        i32 w = 0;
        for (i32 r = 0; r < jo::self<jo::Nation>(this)->recruit_count; ++r) {
            jo::self<jo::Nation>(this)->recruit_ages[r] = static_cast<u8>(jo::self<jo::Nation>(this)->recruit_ages[r] + 1);
            if (static_cast<i32>(static_cast<std::int8_t>(jo::self<jo::Nation>(this)->recruit_ages[r])) >= 7) {
                jo::self<jo::Nation>(this)->population = jo::self<jo::Nation>(this)->population + 1;
            } else {
                if (w != r) jo::self<jo::Nation>(this)->recruit_ages[w] = jo::self<jo::Nation>(this)->recruit_ages[r];
                ++w;
            }
        }
        jo::self<jo::Nation>(this)->recruit_count = w;
        return this;
    }

    // ------------------------------------------------------------------ 0x004b8ca0
    u32 shift_1c_24() {
        jo::view<u32>(jo::self<jo::Rel>(this)->import_prev) = jo::view<u32>(jo::self<jo::Rel>(this)->import_value);
        jo::view<u32>(jo::self<jo::Rel>(this)->import_value) = 0;
        const u32 v = jo::view<u32>(jo::self<jo::Rel>(this)->export_value);  // eax on exit = old +0x24 (mov eax, [edx+0x24])
        jo::view<u32>(jo::self<jo::Rel>(this)->export_prev) = v;
        jo::view<u32>(jo::self<jo::Rel>(this)->export_value) = 0;
        return v;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Calendar::value", 0x004b4de0, &H::calendar_value);
B95("Calendar::age_entries", 0x004b5a90, &H::age_entries);
B95("Obj::shift_1c_24", 0x004b8ca0, &H::shift_1c_24);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::img::obj;

using joan::hook::global;

using joan::orig::helpers::at;
using joan::img::obj;

struct H {
    // 0x004b4e30
    float projected() {
        const joan::hook::x87::Fpu fpu{true};
        const std::int32_t days = static_cast<std::int32_t>(0x16du - global<std::uint32_t>(d::g_a47bf0));
        double v = fpu.mul(static_cast<double>(days), jo::self<jo::Nation>(this)->income_prev);
        v = fpu.div(v, global<float>(d::flt_365));  // 365.0
        v = fpu.add(v, jo::self<jo::Nation>(this)->income);
        return static_cast<float>(v);
    }

    // 0x004b5010
    void spend(std::int32_t i, float v, std::int32_t flag) {
        jo::self<jo::Nation>(this)->gold = jo::self<jo::Nation>(this)->gold - v;
        jo::self<jo::Nation>(this)->spend_by_kind[i] = v + jo::self<jo::Nation>(this)->spend_by_kind[i];
        jo::self<jo::Nation>(this)->spent = v + jo::self<jo::Nation>(this)->spent;
        jo::self<jo::Nation>(this)->income = jo::self<jo::Nation>(this)->income - v;
        if (flag != 0) jo::self<jo::Nation>(this)->f32_139d = v + jo::self<jo::Nation>(this)->f32_139d;
    }

    // 0x004b8d60
    std::int32_t raise_to(std::int32_t i, std::int32_t pct) {
        const std::uint32_t rnd = static_cast<std::uint32_t>(joan::orig::as_misc(obj(d::g_misc))->random(0x3c)) + 0x46u;  // 0x00590540
        const std::int32_t q = static_cast<std::int32_t>(rnd * static_cast<std::uint32_t>(pct)) / 100;
        const std::int32_t lim = static_cast<std::int32_t>(global<std::uint32_t>(d::g_get_a47bb0) + static_cast<std::uint32_t>(q));
        std::int32_t& slot = jo::self<jo::Rel>(this)->deadlines[i];
        const std::int32_t v = slot > lim ? slot : lim;
        slot = v;
        return v;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Economy::projected", 0x004b4e30, &H::projected);
B("Economy::spend", 0x004b5010, &H::spend);
B("Table::raise_to", 0x004b8d60, &H::raise_to);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::hook::x87::ftol;
using u32 = std::uint32_t;

using joan::orig::helpers::at;
using joan::orig::helpers::addr;

JOAN_COUNTER(c_total_4b4f70, "Obj::total_4b4f70");
JOAN_COUNTER(c_push_4b5990, "Obj::push_4b5990");
JOAN_COUNTER(c_update_4b69d0, "List::update_4b69d0");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x004b4f70
    float total_4b4f70() {
        c_total_4b4f70.hit();
        const Fpu fpu{true};
        float a = 0.0f, b = 0.0f;
        for (i32 i = 0; i < 7; ++i) {
            a = static_cast<float>(fpu.add(a, jo::self<jo::Nation>(this)->income_by_kind[i]));
            b = static_cast<float>(fpu.add(b, jo::self<jo::Nation>(this)->income_by_kind_prev[i]));
        }
        const i32 days = 0x16d - global<i32>(d::g_a47bf0);
        double v = fpu.mul(static_cast<double>(days), b);
        v = fpu.div(v, global<float>(d::flt_365));  // 365.0
        v = fpu.add(v, a);
        return static_cast<float>(v);
    }

    // ------------------------------------------------------------------ 0x004b5990
    i32 push_4b5990(i32 n) {
        c_push_4b5990.hit();
        const i32 top = ftol(jo::self<jo::Nation>(this)->goods);
        if (n > top - jo::self<jo::Nation>(this)->recruit_count) n = top - jo::self<jo::Nation>(this)->recruit_count;
        if (jo::self<jo::Nation>(this)->recruit_count + n > 0x14) n = 0x14 - jo::self<jo::Nation>(this)->recruit_count;
        for (i32 i = 0; i < n; ++i) {
            jo::self<jo::Nation>(this)->recruit_ages[jo::self<jo::Nation>(this)->recruit_count] = 0;
            jo::self<jo::Nation>(this)->recruit_count = jo::self<jo::Nation>(this)->recruit_count + 1;
            joan::orig::as_economy(this)->spend(8, joan::orig::arg::f32(global<u32>(d::k_9d5310)), 0);  // 0x004b5010
        }
        return n;
    }

    // ------------------------------------------------------------------ 0x004b69d0
    u32 update_4b69d0() {
        c_update_4b69d0.hit();
        i32 i = 1;
        for (;;) {
            const i32 count = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
            if (i > count) return static_cast<u32>(count);
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_unset(joan::orig::arg::i32(i))) == 0) {          // 0x004b6780
                void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
                joan::orig::as_nation(e)->roll_stats();                        // 0x004b4a10
            }
            ++i;
        }
    }

    // ------------------------------------------------------------------ 0x004b8df0
    std::uintptr_t clear_flags_4b8df0(const i16* list) {  // eax: memset's result or list
        if (list == nullptr) {
            return addr(std::memset(static_cast<void*>(self() + 0xd0), 0, 0x14u));  // x64: needs Nation+0xd0 u8[20]                       // memset
        }
        for (i32 i = 0; static_cast<i32>(list[i]) != 0; ++i) {
            at<u8>(this, static_cast<i32>(list[i]) + 0xcf) = 0;  // x64: needs Nation+0xd0 u8[20]
        }
        return addr(list);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::total_4b4f70", 0x004b4f70, &H::total_4b4f70);
B("Obj::push_4b5990", 0x004b5990, &H::push_4b5990);
B("List::update_4b69d0", 0x004b69d0, &H::update_4b69d0);
B("Obj::clear_flags_4b8df0", 0x004b8df0, &H::clear_flags_4b8df0);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

using joan::orig::helpers::at;

struct H {
    void nop_4b8b20() {}

    // ------------------------------------------------------------------------ 0x004b5340
    void add_time(float dt) {
        jo::self<jo::Nation>(this)->time = dt + jo::self<jo::Nation>(this)->time;
        jo::self<jo::Nation>(this)->time_gained = dt + jo::self<jo::Nation>(this)->time_gained;
        jo::self<jo::Nation>(this)->time_net = dt + jo::self<jo::Nation>(this)->time_net;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::add_time", 0x004b5340, &H::add_time);
B94("Obj::nop_4b8b20", 0x004b8b20, &H::nop_4b8b20);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::orig::helpers::at;

using u32 = std::uint32_t;

struct H {
    u32 dtor_m() {  // 0x004b6450
        joan::orig::as_list(this)->clear_4b6530();       // 0x004b6530
        return thiscall<u32>(fn::dtor_DynArrayB, this);  // 0x00585710
    }

    // ------------------------------------------------------------------------ 0x004b53b0
    void transfer(float d) {
        // The object is a Nation (diplo_text msg_apply passes the receiving Nation); Stats is a partial view whose x64 offsets drift.
        jo::self<jo::Nation>(this)->time = jo::self<jo::Nation>(this)->time - d;
        jo::self<jo::Nation>(this)->time_transferred = d + jo::self<jo::Nation>(this)->time_transferred;
        jo::self<jo::Nation>(this)->time_net = jo::self<jo::Nation>(this)->time_net - d;
    }

    // ------------------------------------------------------------------------ 0x004b8ed0
    float day_value() {
        const i32 days = static_cast<i32>(0x16du - global<u32>(d::g_a47bf0));
        const Fpu fpu{true};
        double r = fpu.mul(static_cast<double>(days), static_cast<double>(jo::self<jo::Rel>(this)->export_prev));
        r = fpu.div(r, static_cast<double>(global<float>(d::flt_365)));  // 365.0f
        r = fpu.add(r, static_cast<double>(jo::self<jo::Rel>(this)->export_value));
        return static_cast<float>(r);
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("Stats::transfer", 0x004b53b0, &H::transfer);
B96("ObjM::dtor", 0x004b6450, &H::dtor_m);
B96("X::day_value", 0x004b8ed0, &H::day_value);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
namespace {
namespace game_b100_05 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::img::obj;

void* const kMath = obj(d::g_misc);

using u16 = std::uint16_t;

using u32 = std::uint32_t;

using joan::orig::helpers::at;

void* const kEntities = obj(d::g_units);

using joan::orig::helpers::op_new;

u8* entity(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kEntities)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_release_6, "Slot::release_6");
JOAN_COUNTER(c_update_income, "Town::update_income");
JOAN_COUNTER(c_add_building, "Town::add_building");
JOAN_COUNTER(c_score, "Town::score");
JOAN_COUNTER(c_days_value, "Town::days_value");

struct H {
    // ------------------------------------------------------------------ 0x004b5580
    // jmp to the epilogue right after the prologue: the whole body is unreachable.
    void recount_1389(i32) {}

    // ------------------------------------------------------------------ 0x004b5630
    u32 release_6(u32 arg) {
        c_release_6.hit();
        if (static_cast<i32>(jo::self<jo::Nation>(this)->king_unit) != 0) {
            u8* const e = entity(static_cast<i32>(jo::self<jo::Nation>(this)->king_unit));
            joan::orig::as_unit(e)->set_level_state(4);  // 0x005089e0
            jo::self<jo::Nation>(this)->king_unit = 0;
        }
        joan::orig::as_nation(this)->apply_units(joan::orig::arg::u32(arg));  // 0x004b56a0
        return 0;  // callers discard the result
    }

    // ------------------------------------------------------------------ 0x004b5750
    void update_income() {
        c_update_income.hit();
        const Fpu fpu{true};
        jo::self<jo::Nation>(this)->score_goods = jo::self<jo::Nation>(this)->goods * global<float>(d::flt_0_01);
        float sum = 0.0f;
        for (i32 i = 0; i < static_cast<i32>(jo::view<i16>(jo::self<jo::Nation>(this)->leader_count)); ++i) {
            const i32 id = static_cast<i32>(jo::view<i16>(jo::self<jo::Nation>(this)->leaders[i].value));
            if (joan::orig::as_dynarray(kEntities)->empty_at(joan::orig::arg::i32(id)) == 0) {  // 0x00428b70
                const double v = joan::orig::arg::as<double>(joan::orig::as_group(entity(id))->total());  // 0x00509710
                sum = static_cast<float>(fpu.add(v, sum));
            }
        }
        for (i32 i = 0; i < static_cast<i32>(jo::view<i16>(jo::self<jo::Nation>(this)->idle_count)); ++i) {
            const i32 id = static_cast<i32>(jo::view<i16>(jo::self<jo::Nation>(this)->idle_units[i].value));
            if (joan::orig::as_dynarray(kEntities)->empty_at(joan::orig::arg::i32(id)) == 0) {  // 0x00428b70
                const double v = joan::orig::arg::as<double>(joan::orig::as_unit(entity(id))->stat_pct());  // 0x005097d0
                sum = static_cast<float>(fpu.add(v, sum));
            }
        }
        const double base = fpu.mul(static_cast<double>(jo::self<jo::Nation>(this)->population), global<float>(d::flt_0_1));
        sum = static_cast<float>(fpu.add(base, sum));
        jo::self<jo::Nation>(this)->strength = sum;
        const double r = joan::orig::arg::as<double>(joan::orig::as_economy(this)->projected());  // 0x004b4e30
        const double q = fpu.div(jo::self<jo::Nation>(this)->gold, global<float>(d::flt_5));
        jo::self<jo::Nation>(this)->wealth = static_cast<float>(fpu.add(q, r));
        jo::view<u32>(jo::self<jo::Nation>(this)->mood_copy) = jo::view<u32>(jo::self<jo::Nation>(this)->mood);
        jo::view<u32>(jo::self<jo::Nation>(this)->mood_score) = 0;
        for (i32 i = 0; i < 8; ++i) {
            const double z = global<float>(d::g_a017b4 + i * 4);
            const double y = global<float>(d::g_a01794 + i * 4);
            const double x = jo::self<jo::Nation>(this)->mood_copy * global<float>(d::g_a01774 + i * 4);
            const double v = joan::orig::arg::as<double>(joan::orig::as_misc(kMath)->limit_double(joan::orig::arg::f64(x), joan::orig::arg::f64(y), joan::orig::arg::f64(z)));  // 0x00590b00
            jo::self<jo::Nation>(this)->mood_score = static_cast<float>(fpu.add(v, jo::self<jo::Nation>(this)->mood_score));
        }
    }

    // ------------------------------------------------------------------ 0x004b6640
    u32 add_building(i16 kind) {
        c_add_building.hit();
        void* const mem = op_new(sizeof(jo::Nation));  // 0x1545 on x86
        u8* p = mem != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_nation(mem)->construct()) : nullptr;  // 0x004b3db0
        joan::orig::dynarray_b(this)->linkin(&p);                                   // 0x005857a0
        const u32 n = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        jo::view<u16>(jo::as_nation(p)->id) = static_cast<u16>(n);
        joan::orig::as_nation(p)->init(joan::orig::arg::u32(kind));       // 0x004b3e20
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
    }

    // ------------------------------------------------------------------ 0x004b8a10
    float score(i32 slot, float b) {
        c_score.hit();
        const double bb = static_cast<double>(b) + static_cast<double>(b);  // exact on the x87
        if (bb >= static_cast<double>(jo::self<jo::Nation>(this)->gold)) return global<float>(d::k_9cf5c4);
        const Fpu fpu{true};
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->relation(joan::orig::arg::i32(slot)));  // 0x004b42e0
        double t = fpu.div(static_cast<double>(jo::as_relation(rec)->opinion), global<float>(d::flt_100));
        const float v8 = static_cast<float>(fpu.mul(t, global<float>(d::flt_0_1)));
        const float vc = global<float>(d::g_a0177c) * jo::self<jo::Nation>(this)->wealth * global<float>(d::flt_0_002);
        const float v10 = b / jo::self<jo::Nation>(this)->gold * global<float>(d::flt_0_5);
        float v14 = 0.0f;
        const double r = joan::orig::arg::as<double>(joan::orig::as_economy(this)->projected());  // 0x004b4e30
        if (!(r >= static_cast<double>(b))) {                 // test ah,1: r < b or unordered
            v14 = 0.5f;
        } else {
            const double r2 = joan::orig::arg::as<double>(joan::orig::as_economy(this)->projected());  // 0x004b4e30
            t = fpu.div(b, r2);
            v14 = static_cast<float>(fpu.mul(t, global<float>(d::flt_0_5)));
        }
        return v8 + vc - v10 - v14;
    }

    // ------------------------------------------------------------------ 0x004b8e80
    float days_value() {
        c_days_value.hit();
        const Fpu fpu{true};
        const i32 days = static_cast<i32>(0x16du - global<u32>(d::g_a47bf0));
        double v = fpu.mul(static_cast<double>(days), jo::self<jo::Rel>(this)->import_prev);
        v = fpu.div(v, global<float>(d::flt_365));
        return static_cast<float>(fpu.add(v, jo::self<jo::Rel>(this)->import_value));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Town::recount_1389", 0x004b5580, &H::recount_1389);
B("Slot::release_6", 0x004b5630, &H::release_6);
B("Town::update_income", 0x004b5750, &H::update_income);
B("Town::add_building", 0x004b6640, &H::add_building);
B("Town::score", 0x004b8a10, &H::score);
B("Town::days_value", 0x004b8e80, &H::days_value);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b87_04.cpp (game/b87_04)
namespace {
namespace game_b87_04 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::img::obj;

using joan::orig::helpers::at;

void* const kArrays = obj(d::g_misc);  // array helper (search / insert / remove / random)
const fn::Code kCompareShort = fn::compare_short_4b5e10;  // passed as data (function pointer)

JOAN_COUNTER(c_update_unit, "Nation::update_unit");
JOAN_COUNTER(c_remove_unit, "Nation::remove_unit");


// 0x005911a0 search(key*, array, count, size, cmp, out_index*) ret 0x18.
std::int32_t search(void* key, void* array, std::int32_t count, std::int32_t* out) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kArrays)->bin_search_5911a0(joan::orig::arg::mptr(key), joan::orig::arg::str(array), joan::orig::arg::i32(count), 2, fn::code_value(kCompareShort), joan::orig::arg::mptr(out)));
}

struct H {
    // One sorted id list: count (word), capacity (word), array.
    void list_sync(std::uint16_t& count, std::uint16_t& cap, jo::ArrI16*& arr, std::uint32_t* key, bool want, std::int32_t found, std::int32_t index) {
        if (want) {
            if (found != 0) return;
            void* const grown = joan::orig::arg::as<void*>(joan::orig::grow_array(joan::orig::arg::mptr(arr), joan::orig::arg::mptr(&cap), joan::orig::arg::i16(static_cast<std::int32_t>(jo::view<std::int16_t>(count)) + 1)));
            arr = static_cast<jo::ArrI16*>(grown);
            joan::orig::as_misc(kArrays)->insert_array_rec_5910f0(joan::orig::arg::str(arr), joan::orig::arg::i32(static_cast<std::int32_t>(jo::view<std::int16_t>(count))), 2, joan::orig::arg::i32(index), joan::orig::arg::mptr(static_cast<void*>(key)));  // 0x005910f0
            count = static_cast<std::uint16_t>(count + 1);
            return;
        }
        if (found == 0) return;
        joan::orig::as_misc(kArrays)->del_array_rec_591070(joan::orig::arg::str(arr), joan::orig::arg::i32(static_cast<std::int32_t>(jo::view<std::int16_t>(count))), 2, joan::orig::arg::i32(found));  // 0x00591070
        count = static_cast<std::uint16_t>(count - 1);
    }

    // ---- 0x004b5e40 Nation::update_unit(unit) ret 4 ------------------------------------------
    void update_unit(void* unit) {
        c_update_unit.hit();
        std::uint32_t key = 0;  // [ebp-0x8], low word = unit id
        std::memcpy(&key, unit, 2);
        std::int32_t index = 0;  // [ebp-0xc]
        const std::int32_t found1 = search(&key, jo::view<void*>(jo::self<jo::Nation>(this)->leaders), static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::Nation>(this)->leader_count)), &index);
        bool want = static_cast<std::int32_t>(jo::as_unit(unit)->side) == static_cast<std::int32_t>(jo::self<jo::Nation>(this)->id) && joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->leads()) != 0;  // 0x0044a660
        list_sync(jo::self<jo::Nation>(this)->leader_count, jo::self<jo::Nation>(this)->leader_cap, jo::self<jo::Nation>(this)->leaders, &key, want, found1, index);
        const std::int32_t found2 = search(&key, jo::view<void*>(jo::self<jo::Nation>(this)->idle_units), static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::Nation>(this)->idle_count)), &index);
        want = static_cast<std::int32_t>(jo::as_unit(unit)->side) == static_cast<std::int32_t>(jo::self<jo::Nation>(this)->id) && joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->is_busy()) == 0;  // 0x004b62b0
        list_sync(jo::self<jo::Nation>(this)->idle_count, jo::self<jo::Nation>(this)->idle_cap, jo::self<jo::Nation>(this)->idle_units, &key, want, found2, index);
    }

    // ---- 0x004b5cc0 Nation::remove_unit(unit) ret 4 ------------------------------------------
    void remove_unit(void* unit) {
        c_remove_unit.hit();
        std::uint32_t key = 0;
        std::memcpy(&key, unit, 2);
        const std::int32_t found1 = search(&key, jo::view<void*>(jo::self<jo::Nation>(this)->leaders), static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::Nation>(this)->leader_count)), nullptr);
        if (found1 != 0) {
            joan::orig::as_misc(kArrays)->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Nation>(this)->leaders)), joan::orig::arg::i32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::Nation>(this)->leader_count))), 2, joan::orig::arg::i32(found1));  // 0x00591070
            jo::self<jo::Nation>(this)->leader_count = static_cast<std::uint16_t>(jo::self<jo::Nation>(this)->leader_count - 1);
        }
        const std::int32_t found2 = search(&key, jo::view<void*>(jo::self<jo::Nation>(this)->idle_units), static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::Nation>(this)->idle_count)), nullptr);
        if (found2 != 0) {
            joan::orig::as_misc(kArrays)->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Nation>(this)->idle_units)), joan::orig::arg::i32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::Nation>(this)->idle_count))), 2, joan::orig::arg::i32(found2));  // 0x00591070
            jo::self<jo::Nation>(this)->idle_count = static_cast<std::uint16_t>(jo::self<jo::Nation>(this)->idle_count - 1);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_04", name, addr, fn)
B("Nation::update_unit", 0x004b5e40, &H::update_unit);
B("Nation::remove_unit", 0x004b5cc0, &H::remove_unit);
#undef B

}  // namespace game_b87_04
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

using i32 = std::int32_t;

using joan::call::thiscall;

using joan::orig::helpers::at;

JOAN_COUNTER(c_clear3, "List::clear_4b6530");
JOAN_COUNTER(c_findw2, "List::find_by_word2");

using joan::orig::helpers::count_of;  // 0x00417ad0
void remove_at(const void* list, std::int32_t i) { joan::orig::dynarray_b(list)->linkout(joan::orig::arg::i32(i)); }  // 0x005858c0
void compact(const void* list) { joan::orig::as_sound(list)->reset_585d20(); }  // 0x00585d20

struct H {
    // ------------------------------------------------------------------ 0x004b6530
    void* clear_4b6530() {
        c_clear3.hit();
        for (std::int32_t i = count_of(this); i >= 1; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null(joan::orig::arg::i32(i))) != 0) continue;  // 0x00484870
            void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0047b600
            if (e != nullptr) joan::orig::as_nation(e)->destroy(joan::orig::arg::u32(static_cast<std::uint32_t>(1)));  // 0x004b65f0 deleting dtor
            remove_at(this, i);
        }
        compact(this);
        jo::self<jo::ListB>(this)->next_id = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x004b67e0
    std::int32_t find_by_word2(std::int32_t id) {
        c_findw2.hit();
        for (std::int32_t i = count_of(this); i > 0; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null(joan::orig::arg::i32(i))) != 0) continue;  // 0x00484870
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)));  // 0x0047b600
            if (jo::as_nation(e)->kind == id) return i;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("List::clear_4b6530", 0x004b6530, &H::clear_4b6530);
B("List::find_by_word2", 0x004b67e0, &H::find_by_word2);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;                      // operator delete

using joan::orig::helpers::scalar_delete;

struct H {
    void* sdd_4b65f0(std::uint32_t flags) { joan::orig::as_wrap(this)->f_4b3df0(); scalar_delete(this, flags); return this; }  // 0x004b3df0
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj4b65f0::sdd", 0x004b65f0, &H::sdd_4b65f0);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using i32 = std::int32_t;

using joan::call::thiscall;

using u32 = std::uint32_t;

JOAN_COUNTER(c_refresh_4b68d0, "Units::refresh_4b68d0");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x004b68d0
    u32 refresh_4b68d0() {
        c_refresh_4b68d0.hit();
        for (i32 i = 1; i <= joan::orig::as_dynarray(me())->size(); ++i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(me())->is_unset(joan::orig::arg::i32(i))) != 0) continue;     // 0x004b6780
            joan::orig::as_nation(joan::orig::as_dynarray(me())->ptr_at(joan::orig::arg::i32(i)))->tick();  // 0x004b4920(0x0041b3f0)
        }
        joan::orig::as_nation_list(me())->random_relation_shift();         // 0x004b6a40
        return joan::orig::arg::as<u32>(joan::orig::as_economy(me())->trade_4b6d40());  // 0x004b6d40
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Units::refresh_4b68d0", 0x004b68d0, &H::refresh_4b68d0);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b106_08.cpp (game/b106_08)
namespace {
namespace game_b106_08 {

using i32 = std::int32_t;

using joan::call::thiscall;

using u32 = std::uint32_t;

JOAN_COUNTER(c_unflagged, "List::process_unflagged_4b6960");

struct H {
    // 0x004b6960
    void process_unflagged() {
        c_unflagged.hit();
        for (i32 i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_unset(joan::orig::arg::i32(i))) != 0) continue;     // 0x004b6780
            void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            joan::orig::as_nation(e)->nop_4b49f0();                              // 0x004b49f0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_08", name, addr, fn)
B("List::process_unflagged_4b6960", 0x004b6960, &H::process_unflagged);
#undef B

}  // namespace game_b106_08
}  // namespace

// ---------------------------------------- from modules/game_b106_20.cpp (game/b106_20)
namespace {
namespace game_b106_20 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::img::obj;

using i8 = std::int8_t;

using u32 = std::uint32_t;
using u64 = std::uint64_t;

using joan::orig::helpers::at;
using joan::orig::helpers::as_float;
double as_double(u64 bits) {
    double d;
    std::memcpy(&d, &bits, sizeof d);
    return d;
}
float kf(img::Addr a) { return global<float>(a); }
double kd(img::Addr a) { return global<double>(a); }

void* const kRandom = obj(d::g_misc);

JOAN_COUNTER(c_trade_4b6d40, "Economy::trade_4b6d40");

struct H {
    // ------------------------------------------------------------------ 0x004b6d40
    u32 trade_4b6d40() {
        c_trade_4b6d40.hit();
        // [ebp-0x1a8] E, [ebp-0x154] D, [ebp-0x100] C, [ebp-0xac] B, [ebp-0x58] A (21 floats each)
        float fr[105];
        std::memset(fr, 0, sizeof fr);
        float* const E = fr;
        float* const D = fr + 21;
        float* const C = fr + 42;
        float* const B = fr + 63;
        float* const A = fr + 84;
        const float one = kf(d::flt_1);
        const float zero = kf(d::k_9cf5c4);
        const float days = kf(d::flt_365);  // 365
        auto count = [this]() { return joan::orig::as_dynarray(this)->size(); };  // 0x00417ad0
        auto blocked = [this](i32 i) { return joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_unset(joan::orig::arg::i32(i))); };  // 0x004b6780
        auto nation = [this](i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i))); };  // 0x0047b600
        auto pay = [](u8* p, float v) { joan::orig::as_player(p)->add_stat(i32{2}, joan::orig::arg::f32(v), i32{1}); };  // 0x004b4ce0
        auto skip = [&](i32 i) {  // shared filter of loops 1, 3, 4
            if (blocked(i) != 0) return true;
            return !(jo::as_nation(nation(i))->goods >= one);
        };
        u32 last = 0;

        // ---- loop 1: production / demand
        for (i32 i = 1;; ++i) {
            const i32 n = count();
            last = static_cast<u32>(n);
            if (i > n) break;
            A[i] = 0;
            B[i] = 0;
            C[i] = 0;
            D[i] = 0;
            E[i] = 0;
            if (skip(i)) continue;
            u8* const p = nation(i);
            const float r = joan::orig::arg::as<float>(joan::orig::as_misc(kRandom)->random_float(joan::orig::arg::f32(u32{0x3f7d70a4}), joan::orig::arg::f32(u32{0x3f8147ae})));  // 0x005906f0
            A[i] = jo::as_nation(p)->production / days * jo::as_nation(p)->goods * r;
            B[i] = jo::as_nation(p)->demand / days * r;
            C[i] = jo::as_nation(p)->price_trend / days;
            float t = jo::as_nation(p)->consume_limit / days;
            if (!(B[i] >= t)) std::memcpy(&t, &B[i], 4);
            B[i] = B[i] - t;
            pay(p, t * jo::as_nation(p)->goods);
            float s = C[i] * jo::as_nation(p)->tax;
            if ((one + jo::as_nation(p)->own_tax) * s > B[i]) s = B[i] / (one + jo::as_nation(p)->own_tax);
            if (s * jo::as_nation(p)->goods > A[i]) s = A[i] / jo::as_nation(p)->goods;
            const float u = s * jo::as_nation(p)->goods;
            A[i] = A[i] - u;
            B[i] = B[i] - (one + jo::as_nation(p)->own_tax) * s;
            C[i] = C[i] - s;
            pay(p, u * jo::as_nation(p)->own_tax);
            jo::as_nation(p)->sold = u + jo::as_nation(p)->sold;
            D[i] = s + D[i];
        }

        // ---- loop 2: trade between nations
        const i32 n = count();
        for (i32 k = 1; k <= n; ++k) {
            const i32 i = static_cast<i32>(static_cast<u32>(global<i32>(d::g_get_a47bb0)) + static_cast<u32>(k)) % n + 1;
            if (blocked(i) != 0) {
                if (!(jo::as_nation(nation(i))->goods >= one)) continue;
            }
            u8* const q = nation(i);
            const float cval = C[i];
            float sum = as_float(0);  // [ebp-0x1d4] = 0 (dword store)
            const i32 r = joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n));  // 0x00590540
            auto partner = [&](i32 j, i32& m) {
                m = static_cast<i32>(static_cast<u32>(r) + static_cast<u32>(j)) % n + 1;
                if (m == i) return false;
                if (blocked(m) != 0) return false;
                return jo::as_relation(joan::orig::arg::as<u8*>(joan::orig::as_player(q)->relation(joan::orig::arg::i32(m))))->status != 0;  // 0x004b42e0
            };
            for (i32 j = 0; j < n; ++j) {
                i32 m;
                if (!partner(j, m)) continue;
                sum = sum + A[m];
            }
            if (!(sum > zero)) continue;
            for (i32 j = 0; j < n; ++j) {
                i32 m;
                if (!partner(j, m)) continue;
                u8* const pm = nation(m);
                float w = cval * A[m] / sum;
                if ((one + jo::as_nation(q)->export_tax + jo::as_nation(pm)->import_tax) * w > B[i])
                    w = B[i] / (one + jo::as_nation(q)->export_tax + jo::as_nation(pm)->import_tax);
                if (w * jo::as_nation(q)->goods > A[m]) w = A[m] / jo::as_nation(q)->goods;
                const float u = w * jo::as_nation(q)->goods;
                A[m] = A[m] - u;
                B[i] = B[i] - (one + jo::as_nation(q)->export_tax + jo::as_nation(pm)->import_tax) * w;
                C[i] = C[i] - w;
                pay(q, u * jo::as_nation(q)->export_tax);
                pay(q, u * jo::as_nation(pm)->import_tax);
                float* const x = &jo::as_relation(joan::orig::as_player(q)->relation(joan::orig::arg::i32(m)))->export_value;   // 0x004b42e0
                *x = u + *x;
                float* const y = &jo::as_relation(joan::orig::as_player(pm)->relation(joan::orig::arg::i32(i)))->import_value;  // 0x004b42e0
                *y = u + *y;
                E[i] = w + E[i];
            }
        }

        // ---- loop 3: own consumption
        for (i32 i = 1;; ++i) {
            const i32 c = count();
            last = static_cast<u32>(c);
            if (i > c) break;
            if (skip(i)) continue;
            u8* const p = nation(i);
            float s = C[i];
            if ((one + jo::as_nation(p)->own_tax) * s > B[i]) s = B[i] / (one + jo::as_nation(p)->own_tax);
            if (s * jo::as_nation(p)->goods > A[i]) s = A[i] / jo::as_nation(p)->goods;
            const float u = s * jo::as_nation(p)->goods;
            A[i] = A[i] - u;
            B[i] = B[i] - (one + jo::as_nation(p)->own_tax) * s;
            C[i] = C[i] - s;
            pay(p, u * jo::as_nation(p)->own_tax);
            jo::as_nation(p)->sold = u + jo::as_nation(p)->sold;
            D[i] = s + D[i];
            if (A[i] > zero) jo::as_nation(p)->surplus = jo::as_nation(p)->surplus + A[i];
        }

        // ---- loop 4: price / tax adjustment
        const double lo = as_double(0x3fb99999a0000000ull);  // 0.1f as double
        const double hi = as_double(0x3ff0000000000000ull);  // 1.0
        for (i32 i = 1;; ++i) {
            const i32 c = count();
            last = static_cast<u32>(c);
            if (i > c) break;
            if (skip(i)) continue;
            u8* const p = nation(i);
            const double k1 = kd(d::dbl_0_0001);  // 1e-4
            const double k2 = kd(d::dbl_0_00001);  // 1e-5
            // fld B; fcomp k1; test ah,1; jne alt  /  fld C; fcomp k2; test ah,1; je alt
            if (static_cast<double>(B[i]) >= k1 && !(static_cast<double>(C[i]) >= k2)) {
                jo::as_nation(p)->price_trend = B[i] * kf(d::flt_0_01) + jo::as_nation(p)->price_trend;
                const float v = jo::as_nation(p)->tax - B[i] * kf(d::flt_9d5360);
                jo::as_nation(p)->tax = static_cast<float>(
                    joan::orig::arg::as<double>(joan::orig::as_misc(kRandom)->limit_double(joan::orig::arg::f64(static_cast<double>(v)), joan::orig::arg::f64(lo), joan::orig::arg::f64(hi))));  // 0x00590b00
            } else if (static_cast<double>(C[i]) >= k1 && !(static_cast<double>(B[i]) >= k2)) {
                jo::as_nation(p)->price_trend = jo::as_nation(p)->price_trend - C[i] * kf(d::flt_0_05);
                const float v = C[i] * kf(d::flt_9d5360) + jo::as_nation(p)->tax;
                jo::as_nation(p)->tax = static_cast<float>(
                    joan::orig::arg::as<double>(joan::orig::as_misc(kRandom)->limit_double(joan::orig::arg::f64(static_cast<double>(v)), joan::orig::arg::f64(lo), joan::orig::arg::f64(hi))));  // 0x00590b00
            }
            jo::as_nation(p)->goods =
                jo::as_nation(p)->price_trend * kf(d::flt_0_01) * jo::as_nation(p)->goods / days + jo::as_nation(p)->goods;
        }
        return last;
    }
};

JOAN_REPLACE("game/b106_20", "Economy::trade_4b6d40", 0x004b6d40, &H::trade_4b6d40);

}  // namespace game_b106_20
}  // namespace

// ---------------------------------------- from modules/game_b109_08.cpp (game/b109_08)
namespace {
namespace game_b109_08 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::img::obj;

using i8 = std::int8_t;

using u32 = std::uint32_t;

using joan::orig::helpers::at;
// [base + index * scale + disp] with 32-bit wrap-around.

JOAN_COUNTER(c_scan, "Rules::scan_4b7cb0");

struct H {
    // ---------------------------------------------------------------- 0x004b7cb0 helpers
    static void set_state(i32 i, i32 v) { joan::orig::as_unit(obj(d::g_queue))->set_order_480ff0(joan::orig::arg::u32(i), static_cast<std::uint8_t>(joan::orig::arg::u32(v))); }  // 0x00480ff0
    static float roll() { return static_cast<float>(joan::orig::arg::as<double>(joan::orig::as_misc(obj(d::g_misc))->rand_unit())); }  // 0x00590630
    // a = roll(); b = stat(); state 2 if b > a else 3.
    static void compare(i32 i, float a, double b) { set_state(i, b > static_cast<double>(a) ? 2 : 3); }
    // state 2 if r * (1 - base) + base > a else 3 (the stat comes from 0x00498460).
    void blend(i32 i, const u8* rec, float base) {
        const float a = roll();
        const float d = global<float>(d::flt_1) - base;
        const double r = joan::orig::arg::as<double>(joan::orig::as_nation(this)->rank(static_cast<i32>(jo::as_treaty(rec)->from)));  // 0x00498460
        const Fpu fpu{true};
        const double v = fpu.add(fpu.mul(r, static_cast<double>(d)), static_cast<double>(base));
        set_state(i, v > static_cast<double>(a) ? 2 : 3);
    }

    // ------------------------------------------------------------------------ 0x004b7cb0
    void scan_4b7cb0() {
        c_scan.hit();
        void* const list = obj(d::g_queue);
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_talk(list)->count()); i > 0; --i) {  // 0x004b8c40
            if (joan::orig::arg::as<i32>(joan::orig::as_talk(list)->is_deleted(joan::orig::arg::i32(i))) != 0) continue;   // 0x004e4110
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_talk(list)->get_msg(joan::orig::arg::i32(i)));      // 0x004e3fe0
            if (jo::as_treaty(rec)->reply != 1) continue;
            if (static_cast<i32>(jo::as_treaty(rec)->to) != static_cast<i32>(jo::self<jo::Nation>(this)->id)) continue;
            if (global<i32>(d::g_get_a47bb0) >= static_cast<i32>(jo::as_treaty(rec)->time + 0x1eu)) continue;
            u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->relation(static_cast<i32>(jo::as_treaty(rec)->from)));  // 0x004b42e0
            if (joan::orig::arg::as<i32>(joan::orig::as_treaty(rec)->still_valid()) == 0) continue;                                  // 0x004e3180
            if (global<i32>(d::g_get_a47bb0) < jo::as_relation(q)->deadlines[static_cast<i32>(jo::as_treaty(rec)->kind) + 1]) continue;
            const i32 side = static_cast<i32>(jo::as_treaty(rec)->from);
            const i32 a2 = static_cast<i32>(jo::as_treaty(rec)->arg);
            switch (static_cast<i32>(jo::as_treaty(rec)->kind)) {
                case 1: {
                    const float a = roll();
                    compare(i, a, joan::orig::arg::as<double>(joan::orig::as_army(this)->morale_term(joan::orig::arg::i32(side))));  // 0x0048f290
                    break;
                }
                case 2: {
                    const float a = roll();
                    compare(i, a, joan::orig::arg::as<double>(joan::orig::as_army(this)->win_ratio_4b8360(joan::orig::arg::i32(side))));  // 0x004b8360
                    break;
                }
                case 3:
                case 11: {
                    const float a = roll();
                    compare(i, a, joan::orig::arg::as<double>(joan::orig::as_nation(this)->rank(joan::orig::arg::i32(side))));  // 0x00498460
                    break;
                }
                case 7:
                case 10: {
                    const float base = static_cast<float>(joan::orig::arg::as<double>(joan::orig::as_player(this)->score(joan::orig::arg::i32(a2))));  // 0x004a8f20
                    blend(i, rec, base);
                    break;
                }
                case 8: {
                    const double s = joan::orig::arg::as<double>(joan::orig::as_army(this)->morale_term(joan::orig::arg::i32(a2)));  // 0x0048f290
                    const float base =
                        static_cast<float>(Fpu{true}.sub(static_cast<double>(global<float>(d::flt_1)), s));
                    blend(i, rec, base);
                    break;
                }
                case 9: {
                    const float a = roll();
                    compare(i, a, joan::orig::arg::as<double>(joan::orig::as_player(this)->score_curve(joan::orig::arg::u32(a2))));  // 0x004b8710
                    break;
                }
                case 13:
                case 15: set_state(i, 2); break;
                case 14: {
                    const float a = roll();
                    compare(i, a,
                            joan::orig::arg::as<double>(joan::orig::as_nation(this)->time_score(joan::orig::arg::u32(side), joan::orig::arg::f32(static_cast<float>(a2)))));  // 0x004944c0
                    break;
                }
                case 16: {
                    const float a = roll();
                    compare(i, a,
                            joan::orig::arg::as<double>(joan::orig::as_nation(this)->score(joan::orig::arg::i32(side), joan::orig::arg::f32(static_cast<float>(a2)))));  // 0x004b8a10
                    break;
                }
                case 18: set_state(i, 3); break;
                default: break;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_08", name, addr, fn)
B("Rules::scan_4b7cb0", 0x004b7cb0, &H::scan_4b7cb0);
#undef B

}  // namespace game_b109_08
}  // namespace

// ---------------------------------------- from modules/game_b104_04.cpp (game/b104_04)
namespace {
namespace game_b104_04 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::img::obj;

using joan::orig::helpers::at;

void* const kSides = obj(d::g_nations);

JOAN_COUNTER(c_ratio, "Army::win_ratio_4b8360");

struct H {
    // ---------------------------------------------------------------- 0x004b8360
    float win_ratio(i32 side) {
        c_ratio.hit();
        i32 n = 0, w = 0;
        for (i32 i = 1; i <= joan::orig::as_dynarray(kSides)->size(); ++i) {  // 0x00417ad0
            if (i == jo::self<jo::Nation>(this)->id) continue;
            if (joan::orig::arg::as<i32>(joan::orig::as_list(kSides)->is_unset(joan::orig::arg::i32(i))) != 0) continue;  // 0x004b6780
            ++n;
            u8* const a = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->relation(joan::orig::arg::i32(i)));     // 0x004b42e0
            u8* const b = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->relation(joan::orig::arg::i32(side)));  // 0x004b42e0
            if (jo::as_relation(a)->opinion > jo::as_relation(b)->opinion) ++w;
        }
        if (n > 1) {
            const float t = static_cast<float>(Fpu{true}.div(static_cast<double>(n - w), static_cast<double>(n)));
            const float u = jo::self<jo::Nation>(this)->traits[3] * global<float>(d::flt_0_25);
            return t - u;
        }
        const float h = global<float>(d::flt_0_5);
        return h - jo::self<jo::Nation>(this)->traits[3] * h;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_04", name, addr, fn)
B("Army::win_ratio_4b8360", 0x004b8360, &H::win_ratio);
#undef B

}  // namespace game_b104_04
}  // namespace

// ---------------------------------------- from modules/game_b111_18.cpp (game/b111_18)
namespace {
namespace game_b111_18 {

using joan::hook::x87::Fpu;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::img::obj;

using u32 = std::uint32_t;

JOAN_COUNTER(c_score_curve, "Player::score_curve_4b8710");

struct H {
    // ------------------------------------------------------------ 0x004b8710
    double score_curve(u32 other) {
        c_score_curve.hit();
        const double s = joan::orig::arg::as<double>(joan::orig::as_player(this)->score(joan::orig::arg::i32(other)));                     // 0x004a8f20
        const double v = joan::orig::arg::as<double>(joan::orig::as_misc(obj(d::g_misc))->limit_double(joan::orig::arg::f64(s), joan::orig::arg::f64(0.0), joan::orig::arg::f64(1.0)));   // 0x00590b00
        const Fpu fpu{true};
        const double t = fpu.sub(1.0, v);     // fsubr qword [0x9cf5e8], fstp qword [ebp-0xc]
        const double base = fpu.sub(1.0, t);  // fld qword 1.0; fsub [ebp-0xc]; fstp qword [esp]
        return joan::orig::arg::as<double>(joan::orig::libc::pow(joan::orig::arg::f64(base), joan::orig::arg::f64(4.0)));  // pow; st0 returned (see header)
    }
};

JOAN_REPLACE("game/b111_18", "Player::score_curve", 0x004b8710, &H::score_curve);

}  // namespace game_b111_18
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::call::thiscall;

using joan::img::obj;

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;

void* const kUnits = obj(d::g_nations);

JOAN_COUNTER(c_give, "Player::give");

struct H {
    // ------------------------------------------------------------------ 0x004b4790
    u32 give(u32 id, i32 n) {
        c_give.hit();
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)));  // 0x0047b600
        joan::orig::as_economy(this)->spend(0xe, joan::orig::arg::f32(static_cast<float>(n)), 0);           // 0x004b5010
        joan::orig::as_player(unit)->add_stat(5, joan::orig::arg::f32(static_cast<float>(n)), 0);             // 0x004b4ce0
        u8* const a = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->relation(joan::orig::arg::i32(id)));                       // 0x004b42e0
        jo::as_relation(a)->last_gift_day = global<u32>(d::g_get_a47bb0);
        jo::as_relation(a)->gift_total = jo::as_relation(a)->gift_total + n;
        joan::orig::as_table(a)->raise_to(0xf, 0xb4);  // 0x004b8d60
        joan::orig::as_table(a)->raise_to(0x10, 0xb4);
        joan::orig::as_table(a)->raise_to(0xd, 0xb4);
        joan::orig::as_table(a)->raise_to(0xe, 0xb4);
        u8* const b = joan::orig::arg::as<u8*>(joan::orig::as_player(unit)->relation(static_cast<i32>(jo::self<jo::Nation>(this)->id)));
        joan::orig::as_table(b)->raise_to(0xf, 0xb4);
        joan::orig::as_table(b)->raise_to(0x10, 0xb4);
        joan::orig::as_table(b)->raise_to(0xd, 0xb4);
        return joan::orig::arg::as<u32>(joan::orig::as_table(b)->raise_to(0xe, 0xb4));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Player::give", 0x004b4790, &H::give);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::grow_array(void* a0, void* a1, std::int16_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b88_05::grow_array>(a0, a1, a2)); }  // 0x43fc10
auto joan::orig::Nation::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_02::H::zero_1545>(this)); }  // 0x4b3db0
auto joan::orig::Wrap::f_4b3df0() -> void { joan::bind::member<&::game_b86_06::H::wrap_4b3df0>(this); }  // 0x4b3df0
auto joan::orig::Nation::init(std::uint32_t a0) -> void { joan::bind::member<&::game_b99_03::H::nation_init>(this, a0); }  // 0x4b3e20
auto joan::orig::Nation::mark() -> void { joan::bind::member<&::game_b88_06::H::mark>(this); }  // 0x4b40d0
auto joan::orig::Nation::race() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_02::H::kind_word>(this)); }  // 0x4b4120
auto joan::orig::Nation::name() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_03::H::addr_14>(this)); }  // 0x4b4280
auto joan::orig::Player::relation(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_units::H::relation>(this, a0)); }  // 0x4b42e0
auto joan::orig::Table::byte_58(std::int16_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b90_03::H::byte_58>(this, a0)); }  // 0x4b4360
auto joan::orig::Table::set_byte1(std::int32_t a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_02::H::set_byte1>(this, a0, a1)); }  // 0x4b44e0
auto joan::orig::Nation::set_state_3(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_11::H::set_4b4510>(this, a0)); }  // 0x4b4510
auto joan::orig::Nation::set_state_4(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::set_4b4550>(this, a0)); }  // 0x4b4550
auto joan::orig::Nation::is_kind_3() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_03::H::is_3>(this)); }  // 0x4b4590
auto joan::orig::Player::set_relation(std::int32_t a0, char a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_03::H::set_relation>(this, a0, a1)); }  // 0x4b4610
auto joan::orig::NationRes::entry(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_01::H::slot4e>(this, a0)); }  // 0x4b4730
auto joan::orig::Player::give(std::uint32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::give>(this, a0, a1)); }  // 0x4b4790
auto joan::orig::Nation::tick() -> void { joan::bind::member<&::game_b108_13::H::tick_4b4920>(this); }  // 0x4b4920
auto joan::orig::Nation::nop_4b49f0() -> void { joan::bind::member<&::game_b105_05::H::nop0>(this); }  // 0x4b49f0
auto joan::orig::Nation::roll_stats() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_05::H::roll_stats>(this)); }  // 0x4b4a10
auto joan::orig::Player::add_stat(std::int32_t a0, float a1, std::int32_t a2) -> void { joan::bind::member<&::game_b92_03::H::add_stat>(this, a0, a1, a2); }  // 0x4b4ce0
auto joan::orig::Economy::projected() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b91_02::H::projected>(this)); }  // 0x4b4e30
auto joan::orig::Economy::spend(std::int32_t a0, float a1, std::int32_t a2) -> void { joan::bind::member<&::game_b91_02::H::spend>(this, a0, a1, a2); }  // 0x4b5010
auto joan::orig::Nation::year_value() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b97_02::H::year_value>(this)); }  // 0x4b52f0
auto joan::orig::Nation::add_time(float a0) -> void { joan::bind::member<&::game_b94_02::H::add_time>(this, a0); }  // 0x4b5340
auto joan::orig::Stats::transfer(float a0) -> void { joan::bind::member<&::game_b96_02::H::transfer>(this, a0); }  // 0x4b53b0
auto joan::orig::Player::add_mood(float a0) -> void { joan::bind::member<&::game_b92_03::H::add_mood>(this, a0); }  // 0x4b5420
auto joan::orig::Nation::add_1389(std::int32_t a0) -> void { joan::bind::member<&::game_b88_06::H::add_1389>(this, a0); }  // 0x4b5520
auto joan::orig::Nation::recount_1389(std::int32_t a0) -> void { joan::bind::member<&::game_b100_05::H::recount_1389>(this, a0); }  // 0x4b5580
auto joan::orig::Slot::release_6(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_05::H::release_6>(this, a0)); }  // 0x4b5630
auto joan::orig::Nation::apply_units(std::uint32_t a0) -> void { joan::bind::member<&::game_b99_03::H::apply_units>(this, a0); }  // 0x4b56a0
auto joan::orig::Nation::update_income() -> void { joan::bind::member<&::game_b100_05::H::update_income>(this); }  // 0x4b5750
auto joan::orig::Nation::age_entries() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_02::H::age_entries>(this)); }  // 0x4b5a90
auto joan::orig::Nation::spend(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_02::H::spend>(this, a0, a1, a2)); }  // 0x4b5bf0
auto joan::orig::Nation::remove_unit(void* a0) -> void { joan::bind::member<&::game_b87_04::H::remove_unit>(this, a0); }  // 0x4b5cc0
auto joan::orig::Nation::update_unit(void* a0) -> void { joan::bind::member<&::game_b87_04::H::update_unit>(this, a0); }  // 0x4b5e40
auto joan::orig::Unit::is_busy() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b88_06::H::active>(this)); }  // 0x4b62b0
auto joan::orig::List::clear_4b6530() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_06::H::clear_4b6530>(this)); }  // 0x4b6530
auto joan::orig::Nation::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::sdd_4b65f0>(this, a0)); }  // 0x4b65f0
auto joan::orig::NationList::add_nation(std::int16_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_05::H::add_building>(this, a0)); }  // 0x4b6640
auto joan::orig::List::is_unset(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_03::H::is_unset>(this, a0)); }  // 0x4b6780
auto joan::orig::List::find_by_word2(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_06::H::find_by_word2>(this, a0)); }  // 0x4b67e0
auto joan::orig::Players::hostile(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_units::H::players_hostile>(this, a0, a1)); }  // 0x4b6860
auto joan::orig::Units::refresh_4b68d0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_10::H::refresh_4b68d0>(this)); }  // 0x4b68d0
auto joan::orig::List::process_unflagged_4b6960() -> void { joan::bind::member<&::game_b106_08::H::process_unflagged>(this); }  // 0x4b6960
auto joan::orig::List::update_4b69d0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_03::H::update_4b69d0>(this)); }  // 0x4b69d0
auto joan::orig::NationList::random_relation_shift() -> void { joan::bind::member<&::game_b101_05::H::random_relation_shift>(this); }  // 0x4b6a40
auto joan::orig::Economy::trade_4b6d40() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_20::H::trade_4b6d40>(this)); }  // 0x4b6d40
auto joan::orig::Army::count_strength(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_05::H::count_strength>(this, a0)); }  // 0x4b79a0
auto joan::orig::Nation::scan_treaties() -> void { joan::bind::member<&::game_b109_08::H::scan_4b7cb0>(this); }  // 0x4b7cb0
auto joan::orig::Nation::nop_4b8310() -> void { joan::bind::member<&::game_b97_02::H::nop>(this); }  // 0x4b8310
auto joan::orig::Army::win_ratio_4b8360(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b104_04::H::win_ratio>(this, a0)); }  // 0x4b8360
auto joan::orig::Player::score_curve(std::uint32_t a0) -> double { return joan::bind::conv<double>(joan::bind::member<&::game_b111_18::H::score_curve>(this, a0)); }  // 0x4b8710
auto joan::orig::Nation::score(std::int32_t a0, float a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b100_05::H::score>(this, a0, a1)); }  // 0x4b8a10
auto joan::orig::Nation::nop_4b8b20() -> void { joan::bind::member<&::game_b94_02::H::nop_4b8b20>(this); }  // 0x4b8b20
auto joan::orig::Nation::nop_4b8c20() -> void { joan::bind::member<&::game_b105_05::H::nop0>(this); }  // 0x4b8c20
auto joan::orig::Talk::count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_03::H::count_4>(this)); }  // 0x4b8c40
auto joan::orig::Relation::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_02::H::clear_f4>(this)); }  // 0x4b8c60
auto joan::orig::Relation::roll_stats() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_02::H::shift_1c_24>(this)); }  // 0x4b8ca0
auto joan::orig::Table::raise_to(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::raise_to>(this, a0, a1)); }  // 0x4b8d60
auto joan::orig::Relation::days_value() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b100_05::H::days_value>(this)); }  // 0x4b8e80
auto joan::orig::Relation::day_value() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b96_02::H::day_value>(this)); }  // 0x4b8ed0
auto joan::orig::NationRes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_02::H::clear_8>(this)); }  // 0x4b8f20
