// game/unknown_talk.cpp: functions whose original source file is unknown (formerly game/_unplaced/talk.cpp;
// re/attribution.csv: between ww/uti3d/t_scrres.cpp and ww/trackres.cpp). They move to their original file when
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

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::img::obj;

using u8 = std::uint8_t;

using joan::call::thiscall;

using i8 = std::int8_t;
using i32 = std::int32_t;

using joan::hook::global;

using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

u32 JOAN_CDECL init_4e36b0() { return joan::orig::arg::as<u32>(joan::orig::array_a3a530()); }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("init_4e36b0", 0x004e36b0, &init_4e36b0);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

using joan::call::stdcall_call;

// ------------------------------------------------------------------------------- cdecl
u32 JOAN_CDECL array_a3a530() {  // 0x004e36d0
    joan::orig::vector(obj(d::g_a3a530), i32{0x191}, i32{0x14}, joan::orig::arg::mptr(fn::code_value(fn::String)));  // 0x00449af0
    return 0;  // void callee; the only caller (init_4e36b0) feeds _initterm, which ignores the eax
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("StaticInit::array_a3a530", 0x004e36d0, &array_a3a530);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b100_07.cpp (game/b100_07)
namespace {
namespace game_b100_07 {

struct H {
    // ------------------------------------------------------------------------ 0x004e3700
    void* ctor_e37() {  // this = Talk (g_queue, globals.cpp 0x0056ad30)
        joan::orig::dynarray_b(&jo::self<jo::Talk>(this)->msgs)->construct(0x17, 100, 0, 0);  // 0x005856a0
        return this;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_07", name, addr, fn)
B100("X::ctor_e37", 0x004e3700, &H::ctor_e37);
#undef B100

}  // namespace game_b100_07
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

struct H {
    // ------------------------------------------------------------------------ 0x004e3740 (this = Talk, g_queue)
    void register_4e3740() {
        joan::orig::fill_except(joan::orig::arg::mptr(obj(d::g_misc)), joan::orig::arg::str(static_cast<void*>(this)), 0, 0x32u, 1, // 0x005919a0
                        static_cast<void*>(&jo::self<jo::Talk>(this)->msgs), 0x2e);
        joan::orig::as_sound(&jo::self<jo::Talk>(this)->msgs)->reset_585d20();  // 0x00585d20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("Obj::register_4e3740", 0x004e3740, &H::register_4e3740);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
namespace {
namespace game_b84_03 {

struct H {
    // ------------------------------------------------------------------ 0x004e3790
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("Obj::nop_4e3790", 0x004e3790, &H::nop);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b103_05.cpp (game/b103_05)
namespace {
namespace game_b103_05 {

void* const kPlayers = obj(d::g_nations);

JOAN_COUNTER(c_diplo, "Diplomacy::test");

struct H {
    // ------------------------------------------------------------------------ 0x004e37b0
    i32 rank_below_3(u32 a, u32 b) { return joan::orig::arg::as<i32>(joan::orig::as_talk(this)->count_recent(joan::orig::arg::i32(a), joan::orig::arg::i32(b))) < 3 ? 1 : 0; }  // 0x004e3f00

    // ------------------------------------------------------------------------ 0x004e37f0
    i32 diplomacy_test(i32 a, i32 b, i32 query) {
        c_diplo.hit();
        void* const pb = joan::orig::as_dynarray(kPlayers)->ptr_at(joan::orig::arg::i32(b));  // 0x0047b600
        void* const pa = joan::orig::as_dynarray(kPlayers)->ptr_at(joan::orig::arg::i32(a));  // 0x0047b600
        void* const rel = joan::orig::arg::as<void*>(joan::orig::as_player(pb)->relation(joan::orig::arg::i32(a)));       // 0x004b42e0
        const i32 kind = static_cast<i32>(jo::as_rel(rel)->attitude);
        switch (query) {
            case 1:
                if (kind == 4 || kind == 0) return 0;
                return jo::view<i8>(jo::as_rel(joan::orig::as_player(pa)->relation(joan::orig::arg::i32(b)))->status) == 0 ? 1 : 0;  // 0x004b42e0
            case 2: return (kind == 1 || kind == 2) ? 1 : 0;
            case 3: return (kind == 3 || kind == 2) ? 1 : 0;
            case 4:
                if (kind == 4) return 0;
                return jo::view<i8>(jo::as_rel(joan::orig::as_player(pa)->relation(joan::orig::arg::i32(b)))->status) != 0 ? 1 : 0;  // 0x004b42e0
            case 5: return kind == 3 ? 1 : 0;
            case 6: return kind == 4 ? 1 : 0;
            case 7:
                if (joan::orig::arg::as<i32>(joan::orig::as_player(pb)->has_flag_1365()) == 0) return 0;  // 0x004e4430
                return (kind == 3 || kind == 4) ? 1 : 0;
            case 8: return (kind == 3 || kind == 4) ? 1 : 0;
            case 9: return kind == 0 ? 1 : 0;
            case 10:
                if (kind != 4) return 0;
                for (i32 i = joan::orig::as_dynarray(kPlayers)->size(); i > 0; --i) {  // 0x00417ad0
                    if (joan::orig::arg::as<i32>(joan::orig::as_list(kPlayers)->is_null(joan::orig::arg::i32(i))) != 0) continue;   // 0x00484870
                    if (jo::as_rel(joan::orig::as_player(pb)->relation(joan::orig::arg::i32(i)))->attitude == 0) return 1;  // 0x004b42e0
                }
                return 0;
            case 11: return kind != 0 ? 1 : 0;
            case 12: return (kind == 4 || kind == 3 || kind == 0) ? 0 : 1;
            case 13:
            case 14: return kind <= 2 ? 1 : 0;
            case 15:
            case 16: return kind >= 3 ? 1 : 0;
            case 17: return 0;
            case 18:
            case 19: return 1;
            default: return 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("X::rank_below_3", 0x004e37b0, &H::rank_below_3);
B("Diplomacy::test", 0x004e37f0, &H::diplomacy_test);
#undef B

}  // namespace game_b103_05
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using i16 = std::int16_t;

void* const kTypes = obj(d::g_nations);

JOAN_COUNTER(c_dispatch, "Order::dispatch_4e3bb0");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x004e3bb0
    void dispatch_4e3bb0(u8* order, i32 flag) {
        c_dispatch.hit();
        order[0x11] = joan::orig::arg::as<i32>(joan::orig::as_treaty(order)->needs_reply()) != 0 ? 1 : 0;  // 0x004e3150
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kTypes)->ptr_at(static_cast<i32>(static_cast<i8>(order[0xe]))));  // 0x0047b600
        if (static_cast<i8>(t[5]) == 3 && static_cast<i8>(order[0x11]) == 1 &&
            joan::orig::arg::as<i32>(joan::orig::as_talk(this)->find_order(joan::orig::arg::mptr(order), static_cast<i32>(0))) != 0)  // 0x0048e7f0
            return;
        if (flag == 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_talk(this)->allows(static_cast<i32>(static_cast<i8>(order[0xf])), joan::orig::arg::i32(// 0x004e37f0
                              static_cast<i32>(static_cast<i8>(order[0xe]))), static_cast<i32>(jo::as_treaty(order)->kind))) == 0)
                return;
        }
        joan::orig::as_talk(this)->post(joan::orig::arg::mptr(order));  // 0x004e3d60
        if (static_cast<i8>(order[0x11]) != 0) return;
        joan::orig::as_treaty(order)->apply();  // 0x004e2a00
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Order::dispatch_4e3bb0", 0x004e3bb0, &H::dispatch_4e3bb0);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

JOAN_COUNTER(c_msg_post, "Msg::post");

struct H {
    // ------------------------------------------------------------------------ 0x004e3d60
    i32 msg_post(u8* p) {
        c_msg_post.hit();
        void* const o = joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(static_cast<i32>(jo::as_treaty(p)->to));  // 0x0047b600
        jo::as_treaty(p)->time = global<u32>(d::g_get_a47bb0);
        u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_player(o)->relation(static_cast<i32>(jo::as_treaty(p)->from)));  // 0x004b42e0
        p[0x16] = r[3];
        i32 ret = thiscall<i32>(fn::linkin_5857a0, &jo::self<jo::Msg>(this)->arg2, p);  // 0x005857a0
        if (static_cast<i32>(jo::as_treaty(p)->to) == global<i32>(d::g_player_side)) {
            const i32 x = joan::orig::as_dynarray(&jo::self<jo::Msg>(this)->arg2)->cursor();  // 0x00417520
            ret = joan::orig::arg::as<i32>(joan::orig::as_news_list(obj(d::g_tasks))->add_treaty(joan::orig::arg::u32(x), 0u));         // 0x004b9e20
        }
        return ret;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Msg::post", 0x004e3d60, &H::msg_post);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

JOAN_COUNTER(c_count_recent, "Talk::count_recent");

struct H {
    // ------------------------------------------------------------------------ 0x004e3f00
    i32 count_recent(i32 a, i32 b) {
        c_count_recent.hit();
        i32 n = 0;
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_talk(this)->count()); i > 0; --i) {  // 0x004b8c40
            if (joan::orig::arg::as<i32>(joan::orig::as_talk(this)->is_deleted(joan::orig::arg::i32(i))) != 0) continue;   // 0x004e4110
            u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_talk(this)->get_msg(joan::orig::arg::i32(i)));         // 0x004e3fe0
            if (static_cast<i32>(jo::as_treaty(m)->reply) != 1) continue;
            if (static_cast<i32>(jo::as_treaty(m)->to) != a) continue;
            if (static_cast<i32>(jo::as_treaty(m)->from) != b) continue;
            const i32 limit = static_cast<i32>(jo::as_treaty(m)->time + 0x1eu);
            if (global<i32>(d::g_get_a47bb0) >= limit) continue;
            ++n;
        }
        return n;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Talk::count_recent", 0x004e3f00, &H::count_recent);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

struct H {
    // 0x004e3fe0
    void* get_msg(std::int32_t i) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_dynarray(&jo::self<jo::Talk>(this)->msgs)->get(joan::orig::arg::i32(i)));  // 0x00417200
        if (jo::as_treaty(p)->kind == 0)
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_get_talk_msg_error_the_TalkM));  // 0x00586cb0
        return p;
    }

    // 0x004e4110
    std::int32_t is_deleted(std::int32_t i) {
        if (i < 1) return 1;
        if (i > joan::orig::as_dynarray(&jo::self<jo::Talk>(this)->msgs)->size()) return 1;  // 0x00417ad0
        const void* const p = joan::orig::arg::as<void*>(joan::orig::as_dynarray(&jo::self<jo::Talk>(this)->msgs)->get(joan::orig::arg::i32(i)));  // 0x00417200
        return jo::as_treaty(p)->kind == 0 ? 1 : 0;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Talk::get_msg", 0x004e3fe0, &H::get_msg);
M("Talk::is_deleted", 0x004e4110, &H::is_deleted);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

struct H {
    // ------------------------------------------------------------------ 0x004e42e0
    u32 call_4e42e0() { return joan::orig::arg::as<u32>(joan::orig::as_list(this)->expire_486c40()); }  // 0x00486c40

    // ------------------------------------------------------------------ 0x004e4590
    bool equals_4e4590(const u8* other) {
        if (jo::self<jo::Key>(this)->code != jo::as_key(other)->code) return false;
        if (jo::self<jo::Key>(this)->u32_5 != jo::as_key(other)->u32_5) return false;
        if (jo::self<jo::Key>(this)->u32_9 != jo::as_key(other)->u32_9) return false;
        if (jo::self<jo::Key>(this)->u32_d != jo::as_key(other)->u32_d) return false;
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("Obj::call_4e42e0", 0x004e42e0, &H::call_4e42e0);
B("Key::equals_4e4590", 0x004e4590, &H::equals_4e4590);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

struct H {
    // ------------------------------------------------------------------------ 0x004e4430
    i32 flag_1365_1366() { return (jo::self<jo::Nation>(this)->at_war != 0 || jo::self<jo::Nation>(this)->was_at_war != 0) ? 1 : 0; }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("X::flag_1365_1366", 0x004e4430, &H::flag_1365_1366);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

struct H {
    // ---- 0x004e4520 Record::set(a, b, c, d, e, g) ret 0x18 ---------------------------------
    void* record_set(std::uint32_t a, std::uint8_t b, std::uint32_t c, std::uint32_t d, std::uint32_t e,
                     std::uint32_t g) {
        jo::view<std::uint32_t>(jo::self<jo::TaskSlot>(this)->kind) = a;
        jo::view<std::uint8_t>(jo::self<jo::TaskSlot>(this)->priority) = b;
        jo::self<jo::TaskSlot>(this)->target = c;
        jo::self<jo::TaskSlot>(this)->arg_9 = d;
        jo::self<jo::TaskSlot>(this)->arg_d = e;
        jo::self<jo::TaskSlot>(this)->expire_time = g;
        jo::self<jo::TaskSlot>(this)->u32_15 = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Record::set", 0x004e4520, &H::record_set);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b87_01.cpp (game/b87_01)
namespace {
namespace game_b87_01 {

using joan::orig::helpers::stricmp_;

JOAN_COUNTER(c_task_from_name, "ai_task_from_name");

// ---- 0x004e4610 ai_task_from_name(name) (cdecl) ---------------------------------------------
std::int32_t JOAN_CDECL ai_task_from_name(char* name) {
    c_task_from_name.hit();
    struct Entry {
        img::Addr text;
        std::int32_t value;
    };
    static const Entry kNames[17] = {
        {d::s_ATTACK_PLACE, 0x1},   // ATTACK_PLACE
        {d::s_ATTACK_UNIT, 0x2},   // ATTACK_UNIT
        {d::s_RECRUIT_TROOP, 0x3},   // RECRUIT_TROOP
        {d::s_HELP_TROOP, 0x4},   // HELP_TROOP
        {d::s_FLEE, 0x5},   // FLEE
        {d::s_DEFEND_PLACE, 0x6},   // DEFEND_PLACE
        {d::s_IDLE, 0x7},   // IDLE
        {d::s_TOWER_ATTACK, 0x8},   // TOWER_ATTACK
        {d::s_SCOUT, 0x9},   // SCOUT
        {d::s_FOLLOW_UNIT, 0xa},   // FOLLOW_UNIT
        {d::s_RETREAT, 0xb},   // RETREAT
        {d::s_RTS_MOVE_TO, 0xc},   // RTS_MOVE_TO
        {d::s_RTS_ATTACK_UNIT, 0xd},   // RTS_ATTACK_UNIT
        {d::s_RTS_ATTACK_ITEM, 0x11},  // RTS_ATTACK_ITEM
        {d::s_APPROACH_PLACE, 0xe},   // APPROACH_PLACE
        {d::s_LARGE_WEAPON_PUSH, 0xf},   // LARGE_WEAPON_PUSH
        {d::s_DEFEND_UNIT, 0x12},  // DEFEND_UNIT
    };
    for (std::int32_t i = 0; i < 17; ++i) {
        if (stricmp_(name, str(kNames[i].text)) == 0) return kNames[i].value;
    }
    char* const upper = joan::orig::arg::as<char*>(joan::orig::libc::_strupr(joan::orig::arg::str(name)));  // _strupr (in place)
    if (joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(upper), str(d::s_SPECIAL))) != nullptr) {  // strstr(.., "SPECIAL")
        return joan::crt::atoi(joan::orig::arg::str(name + 7)) + 0x64;  // atoi
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b87_01", name, addr, fn)
B("ai_task_from_name", 0x004e4610, &ai_task_from_name);
#undef B

}  // namespace game_b87_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::array_a3a530() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b95_04::array_a3a530>()); }  // 0x4e36d0
auto joan::orig::Talk::nop_4e3790() -> void { joan::bind::member<&::game_b84_03::H::nop>(this); }  // 0x4e3790
auto joan::orig::Talk::allows(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_05::H::diplomacy_test>(this, a0, a1, a2)); }  // 0x4e37f0
auto joan::orig::Order::dispatch_4e3bb0(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_b108_08::H::dispatch_4e3bb0>(this, a0, a1); }  // 0x4e3bb0
auto joan::orig::Talk::post(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_05::H::msg_post>(this, a0)); }  // 0x4e3d60
auto joan::orig::Talk::count_recent(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_04::H::count_recent>(this, a0, a1)); }  // 0x4e3f00
auto joan::orig::Talk::get_msg(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_03::H::get_msg>(this, a0)); }  // 0x4e3fe0
auto joan::orig::Talk::is_deleted(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_03::H::is_deleted>(this, a0)); }  // 0x4e4110
auto joan::orig::Talk::expire() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_05::H::call_4e42e0>(this)); }  // 0x4e42e0
auto joan::orig::Player::has_flag_1365() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_04::H::flag_1365_1366>(this)); }  // 0x4e4430
auto joan::orig::UnitTask::set(std::uint32_t a0, std::uint8_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_06::H::record_set>(this, a0, a1, a2, a3, a4, a5)); }  // 0x4e4520
auto joan::orig::ai_task_from_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b87_01::ai_task_from_name>(a0)); }  // 0x4e4610
