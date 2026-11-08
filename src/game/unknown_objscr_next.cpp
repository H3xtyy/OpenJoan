// game/unknown_objscr_next.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/objscr_next.cpp; re/attribution.csv: between ww/interfac/optmenui.cpp and end). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstddef>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using i32 = std::int32_t;

using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_1() {  // 0x00755810
        memset_(this, 0, 1);
        return this;
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("Struct1::zero", 0x00755810, &H::zero_1);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

struct H {
    void set_flag() { jo::self<jo::NextObjectivesScreen>(this)->shown = 1; }  // 0x00755840 / 0x0075f580
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::set_flag_755840", 0x00755840, &H::set_flag);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

struct H {
    // 0x00755860 / 0x0075f5a0
    std::uint32_t init_755860() {
        jo::self<jo::NextObjectivesScreen>(this)->shown = 0;
        return thiscall<std::uint32_t>(fn::Unit_state_sound, obj(d::g_player_unit_recno));  // 0x0042fef0
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj755::init", 0x00755860, &H::init_755860);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

void* game_memset(void* p, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    void* clear1() { return game_memset(this, 0, 1); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Obj::clear1_755890", 0x00755890, &H::clear1);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b82_15.cpp (game/b82_15)
namespace {
namespace game_b82_15 {

struct H {
    // 0x007558c0 / 0x0075f600: the body is jumped over. ret 0.
    void dead_draw_a() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b82_15", name, addr, fn)
B("ObjectiveList::draw_a", 0x007558c0, &H::dead_draw_a);
#undef B

}  // namespace game_b82_15
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using joan::hook::global;

std::uintptr_t low_byte_1(std::uintptr_t eax) { return (eax & ~std::uintptr_t{0xffu}) | 1u; }

struct H {
    // -------------------------------------------------------------- 0x00755d80
    std::uintptr_t play_755d80() {
        const u32 v = global<u32>(d::g_mission);
        joan::orig::as_battle(obj(d::g_scenarios))->load_scenario_41cc80(joan::orig::arg::i32(v + 1), i32{1}, i32{1});  // 0x0041cc80
        return low_byte_1(joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_a50088))->set_state(joan::orig::arg::u32(u8{0xc}))));  // 0x004d7e70
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Sound::play_755d80", 0x00755d80, &H::play_755d80);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

struct H {
    // ------------------------------------------------------------------ empty stubs
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Obj::nop_755dd0", 0x00755dd0, &H::nop);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::NextObjectivesScreen::show() -> void { joan::bind::member<&::game_b94_08::H::set_flag>(this); }  // 0x755840
auto joan::orig::NextObjectivesScreen::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_04::H::init_755860>(this)); }  // 0x755860
auto joan::orig::NextObjectivesScreen::draw() -> void { joan::bind::member<&::game_b82_15::H::dead_draw_a>(this); }  // 0x7558c0
