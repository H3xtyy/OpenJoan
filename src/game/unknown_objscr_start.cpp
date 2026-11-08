// game/unknown_objscr_start.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/objscr_start.cpp; re/attribution.csv: between ww/interfac/optmenui.cpp and end). They move to their
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

using joan::call::thiscall;

using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------ 0x0075f550
    void* clear_1() {
        memset_(this, 0, 1);
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj75f550::Obj", 0x0075f550, &H::clear_1);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

struct H {
    void set_flag() { jo::self<jo::StartObjectivesScreen>(this)->shown = 1; }  // 0x00755840 / 0x0075f580
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::set_flag_75f580", 0x0075f580, &H::set_flag);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

struct H {
    std::uint32_t init_75f5a0() {
        jo::self<jo::StartObjectivesScreen>(this)->shown = 0;
        joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();  // 0x0042fef0
        return 0;  // void callee; all consumers (shutdown table, atexit, mode leave table) ignore the result
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj75f::init", 0x0075f5a0, &H::init_75f5a0);
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
B("Obj::clear1_75f5d0", 0x0075f5d0, &H::clear1);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b82_15.cpp (game/b82_15)
namespace {
namespace game_b82_15 {

struct H {
    void dead_draw_b() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b82_15", name, addr, fn)
B("ObjectiveList::draw_b", 0x0075f600, &H::dead_draw_b);
#undef B

}  // namespace game_b82_15
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

struct H {
    // ------------------------------------------------------------------ 0x0075fa50
    u32 menu_close() {
        joan::orig::as_battle(obj(d::g_scenarios))->start_scenario();                       // 0x0041c810
        joan::orig::as_game(obj(d::g_game_a50088))->set_state(9u);  // 0x004d7e70
        return 1u;                                // mov al, 1 (upper bits were the void callee's eax; the only consumer ignores them)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Menu::close_75fa50", 0x0075fa50, &H::menu_close);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

struct H {
    // ------------------------------------------------------------------ empty stubs
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Obj::nop_75fa90", 0x0075fa90, &H::nop);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::StartObjectivesScreen::zero() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_08::H::clear_1>(this)); }  // 0x75f550
auto joan::orig::StartObjectivesScreen::show() -> void { joan::bind::member<&::game_b94_08::H::set_flag>(this); }  // 0x75f580
auto joan::orig::StartObjectivesScreen::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_04::H::init_75f5a0>(this)); }  // 0x75f5a0
auto joan::orig::StartObjectivesScreen::draw() -> void { joan::bind::member<&::game_b82_15::H::dead_draw_b>(this); }  // 0x75f600
auto joan::orig::Menu::close_75fa50() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_15::H::menu_close>(this)); }  // 0x75fa50
