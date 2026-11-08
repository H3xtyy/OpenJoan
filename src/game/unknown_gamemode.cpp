// game/unknown_gamemode.cpp: functions whose original source file is unknown (formerly game/_unplaced/gamemode.cpp;
// re/attribution.csv: between ww/gamefile.cpp and ww/gameplay.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::global;

using joan::img::obj;

using i32 = std::int32_t;

namespace jo = joan::orig;

}  // namespace

// ---------------------------------------- from modules/game_b105_03.cpp (game/b105_03)
namespace {
namespace game_b105_03 {

// ---------------------------------------------------------------- CRT initialisers
template <fn::Code Callee>
u32 JOAN_CDECL init_call() {
    return cdecl_call<u32>(Callee);
}

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}

    u32 get_13_47c050() {
        return (joan::call::register_low_bits(this) & 0xffffff00u) | jo::self<jo::Control2D>(this)->flags[2];
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_03", name, addr, fn)
B("Obj::stub_47b780", 0x0047b780, &H::stub0);
B("Obj::stub_47b7f0", 0x0047b7f0, &H::stub0);
B("Obj::stub_47b810", 0x0047b810, &H::stub0);
B("Obj::stub_47b920", 0x0047b920, &H::stub0);
B("Obj::stub_47b940", 0x0047b940, &H::stub0);
B("Obj::stub_47b980", 0x0047b980, &H::stub0);
B("Obj::stub_47b9a0", 0x0047b9a0, &H::stub0);
B("Obj::stub_47b9c0", 0x0047b9c0, &H::stub0);
B("Obj::stub_47ba00", 0x0047ba00, &H::stub0);
B("Obj::stub_47ba20", 0x0047ba20, &H::stub0);
B("Obj::stub_47ba40", 0x0047ba40, &H::stub0);
B("Obj::stub_47ba60", 0x0047ba60, &H::stub0);
B("Obj::stub_47baa0", 0x0047baa0, &H::stub0);
B("Obj::stub_47bac0", 0x0047bac0, &H::stub0);
B("Obj::stub_47bae0", 0x0047bae0, &H::stub0);
B("Obj::stub_47bb20", 0x0047bb20, &H::stub0);
B("Obj::stub_47bb40", 0x0047bb40, &H::stub0);
B("Obj::stub_47bb60", 0x0047bb60, &H::stub0);
B("Obj::stub_47bba0", 0x0047bba0, &H::stub0);
B("Obj::get_13_47c050", 0x0047c050, &H::get_13_47c050);
B("init_47b630", 0x0047b630, &init_call<fn::init_a39390>);
B("init_47b690", 0x0047b690, &init_call<fn::init_ratio>);
B("init_47b6e0", 0x0047b6e0, &init_call<fn::init_a399e4>);
B("init_47b730", 0x0047b730, &init_call<fn::init_a399e8>);
#undef B

}  // namespace game_b105_03
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

using joan::hook::x87::Fpu;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0047bd70
    std::uintptr_t call_a501f0() { return joan::orig::arg::as<u32>(joan::orig::as_progress(obj(d::g_progress))->free_images()); }  // 0x00756190

    // ------------------------------------------------------------------ 0x0047bf40
    u32 call_a46030() { return joan::orig::arg::as<u32>(joan::orig::as_video_player(obj(d::g_video_player))->skip()); }  // 0x006d6130
};

void JOAN_CDECL init_a39390() {
    constexpr Fpu fpu{true};
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a39390) = static_cast<float>(fpu.mul(a, global<double>(d::dbl_4)));
}

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("init_a39390", 0x0047b650, &init_a39390);
B99("Obj::call_a501f0", 0x0047bd70, &H::call_a501f0);
B99("Obj::call_a46030", 0x0047bf40, &H::call_a46030);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b100_03.cpp (game/b100_03)
namespace {
namespace game_b100_03 {

using joan::call::thiscall;

// 0x0047b6b0 (float / float at PC24)
void JOAN_CDECL init_ratio() { global<float>(d::g_a399e0) = global<float>(d::g_a39390) / global<float>(d::flt_2); }

struct H {
    // ------------------------------------------------------------------------ forwards
    void fwd_755840() { joan::orig::as_next_objectives_screen(obj(d::g_next_objectives_screen))->show(); }  // 0x0047bda0 -> 0x00755840
    void fwd_6d61d0() { joan::orig::as_video_player(obj(d::g_video_player))->start(); }  // 0x0047be90 -> 0x006d61d0
    void fwd_6b5880() { joan::orig::as_credits(obj(d::g_credits_screen))->close(); }  // 0x0047bfa0 -> 0x006b5880
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_03", name, addr, fn)
B100("init_ratio", 0x0047b6b0, &init_ratio);
B100("X::fwd_755840", 0x0047bda0, &H::fwd_755840);
B100("X::fwd_6d61d0", 0x0047be90, &H::fwd_6d61d0);
B100("X::fwd_6b5880", 0x0047bfa0, &H::fwd_6b5880);
#undef B100

}  // namespace game_b100_03
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0047be60
    u32 reset_a56a4b() {
        return joan::orig::arg::as<u32>(joan::orig::as_next_objectives_screen(obj(d::g_next_objectives_screen))->reset());  // 0x00755860
    }
};

void JOAN_CDECL init_a399e4() { global<float>(d::g_a399e4) = global<float>(d::g_a39390) / global<float>(d::flt_3); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("init_a399e4", 0x0047b700, &init_a399e4);
B97("Obj::reset_a56a4b", 0x0047be60, &H::reset_a56a4b);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0047bec0
    u32 stop_a46030() {
        joan::orig::as_video_player(obj(d::g_video_player))->stop();  // 0x006d6230
        return 0;  // void callee; Mode::set_499540 drops the leave hook's eax
    }
};

void JOAN_CDECL init_a399e8() { global<float>(d::g_a399e8) = global<float>(d::g_a39390) / global<float>(d::flt_4); }

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("init_a399e8", 0x0047b750, &init_a399e8);
B98("Obj::stop_a46030", 0x0047bec0, &H::stop_a46030);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::call::thiscall;

void sprites_flush() {
    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->begin_frame()) != 0)  // 0x00631d50
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->end_frame();                       // 0x00631d80
    else
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->release_list();                       // 0x00631dc0
}

struct H {
    void empty() {}  // 0x004eb550, 0x004d69f0, 0x0047b7a0 ...

    // ---------------------------------------------------------------- screens (this unused)
    void screen_47bbf0() {
        joan::orig::as_start_objectives_screen(obj(d::g_start_objectives_screen))->draw();  // 0x0075f600
        sprites_flush();
    }
    void screen_47bdd0() {
        joan::orig::as_next_objectives_screen(obj(d::g_next_objectives_screen))->draw();  // 0x007558c0
        sprites_flush();
    }
    void screen_47bef0() {
        joan::orig::as_video_player(obj(d::g_video_player))->draw(0u);  // 0x006d5ae0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->begin_frame()) != 0)  // 0x00631d50
            joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->end_frame();                       // 0x00631d80
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Screen::empty_47b7a0", 0x0047b7a0, &H::empty);
L82("Screen::empty_47b960", 0x0047b960, &H::empty);
L82("Screen::empty_47b9e0", 0x0047b9e0, &H::empty);
L82("Screen::empty_47ba80", 0x0047ba80, &H::empty);
L82("Screen::empty_47bb00", 0x0047bb00, &H::empty);
L82("Screen::empty_47bb80", 0x0047bb80, &H::empty);
L82("Screen::update_47bbf0", 0x0047bbf0, &H::screen_47bbf0);
L82("Screen::update_47bdd0", 0x0047bdd0, &H::screen_47bdd0);
L82("Screen::update_47bef0", 0x0047bef0, &H::screen_47bef0);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ wrappers
    u32 call_a50088_3() { joan::orig::as_game(obj(d::g_game_a50088))->set_state(3u); return 0; }  // 0x004d7e70
    u32 call_a52968() { joan::orig::as_screen(obj(d::g_credits_screen))->reset_slots(); return 0; } // 0x00417410
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("X::call_a50088_3", 0x0047b7c0, &H::call_a50088_3);
B96("X::call_a52968", 0x0047c020, &H::call_a52968);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b116_14.cpp (game/b116_14)
namespace {
namespace game_b116_14 {

// Not joan::call::thiscall: a __thiscall function pointer, not a pointer to member (a class-type R is returned
// as a free function returns it); the target is an fn:: name or a computed address (see src/call.hpp).
template <class R = void, class T, class... A>
R thiscall(T address, const void* self, A... args) {
    return reinterpret_cast<R(JOAN_THISCALL*)(void*, A...)>(joan::fn::target(address))(const_cast<void*>(self), args...);
}

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite drawer

// Sprite flush after a menu: 0x00631d50 ? 0x00631d80 : 0x00631dc0 (returns that eax).
u32 flush_sprites() {
    if (joan::orig::arg::as<i32>(joan::orig::as_sprite_queue(obj(kSprites))->begin_frame()) != 0) return joan::orig::arg::as<u32>(joan::orig::as_sprite_queue(obj(kSprites))->end_frame());  // 0x00631d50 / 0x00631d80
    return joan::orig::arg::as<u32>(joan::orig::as_sprite_queue(obj(kSprites))->release_list());                                                      // 0x00631dc0
}

struct H {
    // -----------------------------------------------------------------------------------------
    // 0x0047b830 Game::draw_ending
    u32 draw_ending() {
        joan::canvas::ScreenScope canvas_scope(0x0047b830);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        joan::orig::as_ctl2d(obj(d::g_main_menu))->activate(joan::orig::arg::u32(i32{7}));  // 0x00628df0
        joan::orig::as_sms(obj(d::g_sms))->draw(joan::orig::arg::u32(i32{1}));  // 0x006e5140 PickupNotice::draw
        joan::orig::as_quit_popup(obj(d::g_quit_popup))->draw();          // 0x006e47b0 QuitPopup::draw
        joan::orig::as_victory_screen(obj(d::g_victory_screen))->tick();          // 0x006e8c10
        joan::orig::as_credits(obj(d::g_credits_screen))->draw();          // 0x006b5e60
        joan::orig::as_ending(obj(d::g_ending_screen))->draw();          // 0x006ce450
        return flush_sprites();
    }
    // 0x0047bce0 Game::draw_loading_screen
    u32 draw_loading_screen() {
        joan::canvas::ScreenScope canvas_scope(0x0047bce0);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        joan::orig::as_progress(obj(d::g_progress))->draw();  // 0x00756430
        return flush_sprites();
    }
    // 0x0047bfd0 Game::draw_credits (no 0x00631dc0 branch)
    u32 draw_credits() {
        joan::canvas::ScreenScope canvas_scope(0x0047bfd0);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        joan::orig::as_credits(obj(d::g_credits_screen))->draw();  // 0x006b5e60
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_sprite_queue(obj(kSprites))->begin_frame());  // 0x00631d50
        if (r == 0) return r;
        return joan::orig::arg::as<u32>(joan::orig::as_sprite_queue(obj(kSprites))->end_frame());  // 0x00631d80
    }
};

JOAN_REPLACE("game/b116_14", "Game::draw_ending", 0x0047b830, &H::draw_ending);
JOAN_CANVAS_SELF(0x0047b830);  // ScreenScope in the body (no thunk wrapper)
JOAN_REPLACE("game/b116_14", "Game::draw_loading_screen", 0x0047bce0, &H::draw_loading_screen);
JOAN_CANVAS_SELF(0x0047bce0);  // ScreenScope in the body (no thunk wrapper)
JOAN_REPLACE("game/b116_14", "Game::draw_credits", 0x0047bfd0, &H::draw_credits);
JOAN_CANVAS_SELF(0x0047bfd0);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_14
}  // namespace

// ---------------------------------------- from modules/game_b106_01.cpp (game/b106_01)
namespace {
namespace game_b106_01 {

using i8 = std::int8_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0047b8d0
    u32 toggle_47b8d0() {
        void* const g = obj(d::g_main_menu);
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_widget(g)->get_13())) == 0)  // 0x0047c050
            joan::orig::as_main_menu(g)->reset();                             // 0x0070db10
        return joan::orig::arg::as<u32>(joan::orig::as_widget(g)->hit_628c10(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0}))));             // 0x00628c10
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_01", name, addr, fn)
B("Game::toggle_47b8d0", 0x0047b8d0, &H::toggle_47b8d0);
#undef B

}  // namespace game_b106_01
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0047bbc0
    // thiscall whose `this` is unused: sets the flag byte of the global 0xa3cf30.
    void* set_a3cf30() { joan::orig::as_start_objectives_screen(obj(d::g_start_objectives_screen))->show(); return nullptr; }  // 0x0075f580 (the replacement returns nothing; Mode::set_499540's callers drop the eax)
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Obj::set_a3cf30", 0x0047bbc0, &H::set_a3cf30);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

using joan::call::thiscall;

struct H {
    // --------------------------------------------------------------------- 0x0047bc50
    u32 fwd_75fa50() { return joan::orig::arg::as<u32>(joan::orig::as_menu(obj(d::g_start_objectives_screen))->close_75fa50()); }  // 0x0075fa50
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("X::fwd_75fa50", 0x0047bc50, &H::fwd_75fa50);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x0047bc80
    u32 dtor_global() { return joan::orig::arg::as<u32>(joan::orig::as_start_objectives_screen(obj(d::g_start_objectives_screen))->reset()); }  // 0x0075f5a0
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::dtor_global_a3cf30", 0x0047bc80, &H::dtor_global);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
namespace {
namespace game_b102_02 {

using joan::call::thiscall;

struct H {
    // 0x0047bcb0
    std::uintptr_t init_47bcb0() { return joan::orig::arg::as<u32>(joan::orig::as_progress(obj(d::g_progress))->load_images()); }  // 0x00755e30
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("init_47bcb0", 0x0047bcb0, &H::init_47bcb0);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x0047bd40
    u32 forward_47bd40() { return joan::orig::arg::as<u32>(joan::orig::as_progress(obj(d::g_progress))->step()); }  // 0x0075f180
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("forward_47bd40", 0x0047bd40, &H::forward_47bd40);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0047be30
    u32 forward_755d80() { return joan::orig::arg::as<u32>(joan::orig::as_sound(obj(d::g_next_objectives_screen))->play_755d80()); }  // 0x00755d80
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("forward_755d80", 0x0047be30, &H::forward_755d80);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using joan::call::thiscall;

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x0047bf70
    u32 call_47bf70() { return joan::orig::arg::as<u32>(joan::orig::as_credits(obj(d::g_credits_screen))->start()); }  // 0x006b56b0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Obj::call_47bf70", 0x0047bf70, &H::call_47bf70);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Screen::empty_47b7a0() -> void { joan::bind::member<&::game_leaves82b::H::empty>(this); }  // 0x47b7a0
auto joan::orig::Game::update_mode1() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_02::H::call_a50088_3>(this)); }  // 0x47b7c0
auto joan::orig::Game::toggle_47b8d0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_01::H::toggle_47b8d0>(this)); }  // 0x47b8d0
auto joan::orig::Screen::empty_47b960() -> void { joan::bind::member<&::game_leaves82b::H::empty>(this); }  // 0x47b960
auto joan::orig::Game::update_mode4() -> void { joan::bind::member<&::game_b105_03::H::stub0>(this); }  // 0x47b980
auto joan::orig::Screen::empty_47b9e0() -> void { joan::bind::member<&::game_leaves82b::H::empty>(this); }  // 0x47b9e0
auto joan::orig::Game::update_mode5() -> void { joan::bind::member<&::game_b105_03::H::stub0>(this); }  // 0x47ba00
auto joan::orig::Screen::empty_47ba80() -> void { joan::bind::member<&::game_leaves82b::H::empty>(this); }  // 0x47ba80
auto joan::orig::Game::update_mode6() -> void { joan::bind::member<&::game_b105_03::H::stub0>(this); }  // 0x47baa0
auto joan::orig::Screen::empty_47bb00() -> void { joan::bind::member<&::game_leaves82b::H::empty>(this); }  // 0x47bb00
auto joan::orig::Game::update_mode7() -> void { joan::bind::member<&::game_b105_03::H::stub0>(this); }  // 0x47bb20
auto joan::orig::Screen::empty_47bb80() -> void { joan::bind::member<&::game_leaves82b::H::empty>(this); }  // 0x47bb80
auto joan::orig::Game::update_mode8() -> void { joan::bind::member<&::game_b105_03::H::stub0>(this); }  // 0x47bba0
auto joan::orig::Screen::update_47bbf0() -> void { joan::bind::member<&::game_leaves82b::H::screen_47bbf0>(this); }  // 0x47bbf0
auto joan::orig::Game::update_mode11() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::fwd_75fa50>(this)); }  // 0x47bc50
auto joan::orig::Game::update_mode12() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_17::H::forward_47bd40>(this)); }  // 0x47bd40
auto joan::orig::Screen::update_47bdd0() -> void { joan::bind::member<&::game_leaves82b::H::screen_47bdd0>(this); }  // 0x47bdd0
auto joan::orig::Game::update_mode13() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_18::H::forward_755d80>(this)); }  // 0x47be30
auto joan::orig::Screen::update_47bef0() -> void { joan::bind::member<&::game_leaves82b::H::screen_47bef0>(this); }  // 0x47bef0
auto joan::orig::Game::update_mode14() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::call_a46030>(this)); }  // 0x47bf40
auto joan::orig::Game::update_mode15() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_02::H::call_a52968>(this)); }  // 0x47c020
auto joan::orig::Widget::get_13() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_03::H::get_13_47c050>(this)); }  // 0x47c050
