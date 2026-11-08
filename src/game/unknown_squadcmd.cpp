// game/unknown_squadcmd.cpp: functions whose original source file is unknown (formerly game/_unplaced/squadcmd.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;

using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::img::obj;

using i32 = std::int32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;

void* const kUnits = obj(d::g_units);

void* const kUnitTable = obj(d::g_units);

// ---------------------------------------------------------------------- static initialisers
using joan::orig::helpers::div_global;

}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

struct H {
    void* record(std::int16_t i) { return static_cast<std::uint8_t*>(static_cast<void*>(jo::self<jo::UnitRes>(this)->types)) + i * static_cast<std::int32_t>(sizeof(jo::UnitType)) - static_cast<std::int32_t>(sizeof(jo::UnitType)); }  // 0x0042d0a0 (UnitRes::types: 0x203-byte UnitType records on x86)
    float span() { return jo::self<jo::AnimAction>(this)->end - jo::self<jo::AnimAction>(this)->start; }  // 0x0042d130 (this: an AnimAction)
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("Records::get", 0x0042d0a0, &H::record);
FX1("Range::span", 0x0042d130, &H::span);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units {

struct H {
    std::uint8_t dead() { return joan::orig::as_unit(this)->dead(); }  // 0x0042d0e0
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("Unit::dead", 0x0042d0e0, &H::dead);
#undef U1

}  // namespace game_units
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

struct H {
    // 0x0042d160: called only on g_a59a98, an IngameMenu (bss typing): its open byte (+0x24 on x86; SaveMenu::busy_ drifts on x64)
    char byte_24() { return static_cast<char>(jo::self<jo::IngameMenu>(this)->open); }  // 0x0042d160
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::byte_24", 0x0042d160, &H::byte_24);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

struct H {
    // ------------------------------------------------------------------ 0x0042d180
    void call_5ef400() {
        u8* const p = global<u8*>(d::g_world);
        if (p != nullptr && jo::as_world(p)->scene != nullptr) joan::orig::as_scene(jo::as_world(p)->scene)->set_6c(0u);  // 0x005ef400
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::call_5ef400", 0x0042d180, &H::call_5ef400);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

void* const kGameUnits = obj(d::g_units);  // unit manager

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_reset_42d1d0, "UnitCmd::reset_42d1d0");

struct H {
    void reset_42d1d0() {  // 0x0042d1d0
        c_reset_42d1d0.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return;
        unit_get(jo::self<jo::Player>(this)->unit_id_);
        jo::view<std::uint16_t>(jo::self<jo::Player>(this)->squad_mode) = 0;
        joan::orig::as_parser(obj(d::g_mouse))->drain();  // 0x005959c0
        std::uint8_t* const world = global<std::uint8_t*>(d::g_world);
        if (world != nullptr && jo::as_world(world)->scene != nullptr) joan::orig::as_scene(jo::as_world(world)->scene)->set_6c(1u);  // 0x005ef400
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("UnitCmd::reset_42d1d0", 0x0042d1d0, &H::reset_42d1d0);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

JOAN_COUNTER(c_42d5c0, "Obj::act_42d5c0");

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x0042d270
    u32 act_42d270(u32 /*a1*/, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6) {
        joan::orig::as_unit(this)->link_value_of();             // 0x0042fd60, st0 stored to a dead local
        joan::orig::as_player(this)->set_follow(joan::orig::arg::i32(a3));           // 0x00436190
        joan::orig::as_player(this)->check_unit(joan::orig::arg::u32(a2), joan::orig::arg::u32(a5), joan::orig::arg::u32(a6));   // 0x0042d560
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_player(this)->squad_order(joan::orig::arg::f32(a4), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6)));  // 0x0042d5c0
        return (r & 0xffff0000u) | 1u;
    }

    // 0x0042d5c0
    u32 act_42d5c0(float x, i32 a, i32 b) {
        c_42d5c0.hit();
        u32 r = joan::orig::arg::as<u32>(joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x00428b70
        if (r != 0) return r;
        void* const unit = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        // bool result: only its low byte is tested; the value returned below only reaches the upper half of
        // 0x0042d270's result, which every caller narrows to 16 bits.
        r = joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->can_act());  // 0x00500ba0
        if (static_cast<i8>(r) == 0) return r;
        r = joan::orig::arg::as<u32>(joan::orig::as_player(this)->order_icon());  // 0x004350f0
        if (a == 0) return r;
        if (static_cast<i16>(r) != 0x2f) return r;
        if (b == 0) return r;
        // void callees; the result is unused (see above).
        if (!(x >= 0.0f) || x > 180.0f) {
            joan::orig::as_unit(unit)->check_height();  // 0x00445000
            return 0;
        }
        joan::orig::as_unit(unit)->hero_height_check();                                    // 0x00518890
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Obj::act_42d270", 0x0042d270, &H::act_42d270);
B("Obj::act_42d5c0", 0x0042d5c0, &H::act_42d5c0);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    void empty() {}
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("empty_0042d2e0", 0x0042d2e0, &H::empty);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

JOAN_COUNTER(c_rejoin, "Squad::try_rejoin_42d300");

struct H {
    // ------------------------------------------------------------------------ 0x0042d300
    i16 try_rejoin(i32 flag) {
        c_rejoin.hit();
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return 0;  // 0x00428b70
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        if (joan::orig::arg::as<i32>(joan::orig::as_world(u)->flag_1fd()) != 0) return 0;                 // 0x0042d690
        if (flag == 0) return 0;
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->can_act()) == 0) return 0;  // 0x00500ba0
        if (jo::as_unit(u)->action == 1) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_world(u)->flag_1fd()) != 0) return 0;  // 0x0042d690
        i16 slot = 0;                                      // 0x005183d0 writes it first
        const i32 id = joan::orig::arg::as<i16>(joan::orig::as_unit(u)->find_near_structure(&slot));  // 0x005183d0
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->hero_special(joan::orig::arg::u32(id), joan::orig::arg::i16(i16{1}), joan::orig::arg::i16(slot))) == 0) return 0;  // 0x004fab10
        jo::self<jo::Player>(this)->squad_mode = 1;
        joan::orig::as_player(this)->reset_command();  // 0x0042a770
        joan::orig::as_player(this)->scene_flag_off();  // 0x0042d180
        jo::view<u32>(jo::self<jo::Player>(this)->jump_hold_ms) = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Squad::try_rejoin_42d300", 0x0042d300, &H::try_rejoin);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

struct H {
    // ------------------------------------------------------------------ 0x0042d410
    void nop_42d410(i32) {}
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::nop_42d410", 0x0042d410, &H::nop_42d410);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

JOAN_COUNTER(c_try_stop, "UnitRef::try_stop");

struct H {
    // ------------------------------------------------------------------ 0x0042d450
    i16 try_stop(i32 arg) {
        c_try_stop.hit();
        if (joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_)) != 0) return 0;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        if (arg == 0) return 0;
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(unit)->can_act())) == 0) return 0;  // 0x00500ba0
        if (jo::as_unit(unit)->action == 1) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_world(unit)->flag_1fd()) == 0) return 0;  // 0x0042d690
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(unit)->hero_special(joan::orig::arg::u32(jo::as_unit(unit)->held_item_id), joan::orig::arg::i16(0), joan::orig::arg::i16(2)))) == 0) return 0;  // 0x004fab10
        joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
        jo::view<i32>(jo::as_unit(unit)->act_time) = global<i32>(d::g_now);
        joan::orig::as_player(this)->reset_squad_command();  // 0x0042d1d0
        jo::self<jo::Player>(this)->jump_hold_ms = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("UnitRef::try_stop", 0x0042d450, &H::try_stop);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
namespace {
namespace game_b84_03 {

struct H {
    // ------------------------------------------------------------------ 0x0042d530
    std::uint32_t call_30(void*, std::int32_t) {
        void* const o = global<void*>(d::g_game_device);
        using Fn = std::uint32_t(JOAN_STDCALL*)(void*, std::int32_t);
        const Fn fn = reinterpret_cast<Fn>(joan::call::vtable_of(o)[0x30 / 4]);
        return fn(global<void*>(d::g_game_device), 0);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("Screen::call_30", 0x0042d530, &H::call_30);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

static void JOAN_CDECL init_a336cc() { div_global(d::g_a336cc, d::g_a336c0, d::flt_4); }  // 0x0042d7e0

struct H {
    // ------------------------------------------------------------------ 0x0042d560
    std::uintptr_t zero_42d560(u32, u32, u32) {
        const i32 m = unit_missing(jo::self<jo::Player>(this)->unit_id_);
        if (m != 0) return static_cast<u32>(m) & 0xffff0000u;  // xor ax,ax
        u8* const u = unit_get(jo::self<jo::Player>(this)->unit_id_);                      // ebp-0x8 (dead)
        return reinterpret_cast<std::uintptr_t>(u) & 0xffff0000u;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::zero_42d560", 0x0042d560, &H::zero_42d560);
B99("init_a336cc", 0x0042d7e0, &init_a336cc);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

struct H {
    // `this` is really a Unit (callers pass units; World view drifts on x64)
    std::uint32_t flag_1fd() { return static_cast<std::uint32_t>(jo::self<jo::Unit>(this)->held_item_id); }  // 0x0042d690
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("World::flag_1fd", 0x0042d690, &H::flag_1fd);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

using joan::call::cdecl_call;

using joan::call::cdecl_call;

// ---- CRT initialiser wrappers: call the body directly, return its eax ----
template <fn::Code Body>
u32 JOAN_CDECL init_wrapper() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("init_0x0042d6c0", 0x0042d6c0, &init_wrapper<fn::init_42d6e0>);
B("init_0x0042d720", 0x0042d720, &init_wrapper<fn::init_a336c4>);
B("init_0x0042d770", 0x0042d770, &init_wrapper<fn::init_a336c8>);
B("init_0x0042d7c0", 0x0042d7c0, &init_wrapper<fn::init_a336cc>);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

// 0x0042d6e0: [0xa336c0] = atan(1.0) * qword [0x9cf788] (4.0).
void JOAN_CDECL init_42d6e0() {
    const Fpu fpu{true};
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a336c0) = static_cast<float>(fpu.mul(a, global<double>(d::dbl_4)));
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("init_42d6e0", 0x0042d6e0, &init_42d6e0);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

void JOAN_CDECL init_a336c4() { global<float>(d::g_a336c4) = global<float>(d::g_a336c0) / global<float>(d::flt_2); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("init_a336c4", 0x0042d740, &init_a336c4);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

static void JOAN_CDECL init_a336c8() { div_global(d::g_a336c8, d::g_a336c0, d::flt_3); }  // 0x0042d790 (/3)

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("init_a336c8", 0x0042d790, &init_a336c8);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Records203::get_42d0a0(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::record>(this, a0)); }  // 0x42d0a0
auto joan::orig::Range::span() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_fx::H::span>(this)); }  // 0x42d130
auto joan::orig::SaveMenu::busy() -> char { return joan::bind::conv<char>(joan::bind::member<&::game_b94_01::H::byte_24>(this)); }  // 0x42d160
auto joan::orig::Player::scene_flag_off() -> void { joan::bind::member<&::game_b97_01::H::call_5ef400>(this); }  // 0x42d180
auto joan::orig::Player::reset_squad_command() -> void { joan::bind::member<&::game_b85_06::H::reset_42d1d0>(this); }  // 0x42d1d0
auto joan::orig::Player::squad_act(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_12::H::act_42d270>(this, a0, a1, a2, a3, a4, a5)); }  // 0x42d270
auto joan::orig::Player::nop_42d2e0() -> void { joan::bind::member<&::game_b82_18::H::empty>(this); }  // 0x42d2e0
auto joan::orig::Squad::try_rejoin_42d300(std::int32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b108_14::H::try_rejoin>(this, a0)); }  // 0x42d300
auto joan::orig::Screen::call_30(void* a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_03::H::call_30>(this, a0, a1)); }  // 0x42d530
auto joan::orig::Player::check_unit(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_01::H::zero_42d560>(this, a0, a1, a2)); }  // 0x42d560
auto joan::orig::Player::squad_order(float a0, std::int32_t a1, std::int32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_12::H::act_42d5c0>(this, a0, a1, a2)); }  // 0x42d5c0
auto joan::orig::World::flag_1fd() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b::H::flag_1fd>(this)); }  // 0x42d690
