// game/unknown_i_endscr.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_endscr.cpp;
// re/attribution.csv: between ww/interfac/i_sms.cpp and ww/interfac/i_work.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::str;

using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using joan::orig::helpers::memset_;  // memset

using joan::orig::helpers::widget_ctor;  // 0x00620ee0

struct H {
    // ------------------------------------------------------------------ 0x006e88d0
    void* ctor_6e88d0() {
        widget_ctor(&jo::self<jo::EndScreen>(this)->next_button);
        widget_ctor(&jo::self<jo::EndScreen>(this)->next1_button);
        widget_ctor(&jo::self<jo::EndScreen>(this)->quit_button);
        widget_ctor(&jo::self<jo::EndScreen>(this)->es_button);
        widget_ctor(&jo::self<jo::EndScreen>(this)->joan_button);
        memset_(this, 0, sizeof(jo::EndScreen));  // 0x1084 on i686
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen::ctor_6e88d0", 0x006e88d0, &H::ctor_6e88d0);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {


// 0x006211d0 (13 args): button ctor.
u32 button_ctor(void* b, u32 x, u32 y, void* owner, const char* ot, const char* dw, const char* ov) {
    return joan::orig::arg::as<u32>(joan::orig::as_menu_button(b)->setup(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::mptr(owner), joan::orig::arg::str(ot), joan::orig::arg::str(dw), joan::orig::arg::str(ov), joan::orig::arg::str(0u), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u))));
}

JOAN_COUNTER(c_end_ctor, "EndScreen::ctor");

struct H {
    // ------------------------------------------------------------------ 0x006e89a0
    void* end_ctor() {
        c_end_ctor.hit();
        jo::self<jo::EndScreen>(this)->sheet_a = img::obj(d::g_bmp_res_victory);
        jo::self<jo::EndScreen>(this)->sheet_b = img::obj(d::g_bmp_res_victory2);
        jo::self<jo::EndScreen>(this)->sheet_c = img::obj(d::g_bmp_res_victory3);
        jo::self<jo::EndScreen>(this)->font = img::obj(d::g_bmp_res_victorye);
        jo::self<jo::EndScreen>(this)->victory_image = nullptr;
        jo::self<jo::EndScreen>(this)->victory2_image = nullptr;
        jo::self<jo::EndScreen>(this)->victory3_image = nullptr;
        jo::self<jo::EndScreen>(this)->victory_e_image = nullptr;
        button_ctor(&jo::self<jo::EndScreen>(this)->next_button, 0x378, 0x2d0, jo::self<jo::EndScreen>(this)->font, str(d::s_ENEXT_OT), nullptr, str(d::s_ENEXT_OV));   // ENEXT
        button_ctor(&jo::self<jo::EndScreen>(this)->next1_button, 0x378, 0x2d0, jo::self<jo::EndScreen>(this)->font, str(d::s_1NEXT_OT), nullptr, str(d::s_1NEXT_OV));  // 1NEXT
        button_ctor(&jo::self<jo::EndScreen>(this)->quit_button, 0x378, 0x2d0, jo::self<jo::EndScreen>(this)->font, str(d::s_2QUIT_OT), nullptr, str(d::s_2QUIT_OV));  // 2QUIT
        button_ctor(&jo::self<jo::EndScreen>(this)->es_button, 0x1c, 0x2a8, jo::self<jo::EndScreen>(this)->font, str(d::s_2ES_OT), nullptr, str(d::s_2ES_OV));   // 2ES
        button_ctor(&jo::self<jo::EndScreen>(this)->joan_button, 0x13c, 0x2b8, jo::self<jo::EndScreen>(this)->font, str(d::s_2JOA_OT), nullptr, str(d::s_2JOA_OV));  // 2JOA
        jo::self<jo::EndScreen>(this)->u8_107f = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x006e8d20
    u8 byte_107e() { return jo::self<jo::EndScreen>(this)->open; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("EndScreen::ctor", 0x006e89a0, &H::end_ctor);
B("Obj::byte_107e", 0x006e8d20, &H::byte_107e);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

struct H {
    void reset_6e8b90() {  // 0x006e8b90
        joan::orig::as_panel(this)->release_textures4();            // 0x006e8e40
        joan::orig::as_menu_button(&jo::self<jo::EndScreen>(this)->next_button)->clear();    // 0x006217e0
        joan::orig::as_menu_button(&jo::self<jo::EndScreen>(this)->next1_button)->clear();
        joan::orig::as_menu_button(&jo::self<jo::EndScreen>(this)->quit_button)->clear();
        joan::orig::as_menu_button(&jo::self<jo::EndScreen>(this)->es_button)->clear();
        joan::orig::as_menu_button(&jo::self<jo::EndScreen>(this)->joan_button)->clear();
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6e::reset", 0x006e8b90, &H::reset_6e8b90);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_ui6.cpp (game/ui6)
namespace {
namespace game_ui6 {

struct H {
    // 0x006e8c10. ret 0.
    void overlay_tick() {
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::EndScreen>(this)->open)) != 0) joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
    }
};

#define U6(name, addr, fn) JOAN_REPLACE("game/ui6", name, addr, fn)
U6("Overlay::tick", 0x006e8c10, &H::overlay_tick);
#undef U6

}  // namespace game_ui6
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using i8 = std::int8_t;

struct H {
    bool flag_107e() { return static_cast<i8>(jo::self<jo::EndScreen>(this)->open) != 0; }  // 0x006e8c50
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::flag_107e", 0x006e8c50, &H::flag_107e);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using joan::hook::global;

struct H {
    u32 open_1080() {  // 0x006e8c80
        jo::self<jo::EndScreen>(this)->open = 1;
        jo::self<jo::EndScreen>(this)->u8_107f = 0;
        jo::view<u8>(jo::self<jo::EndScreen>(this)->next_button.enabled) = 1;
        jo::view<u8>(jo::self<jo::EndScreen>(this)->next1_button.enabled) = 1;
        jo::self<jo::EndScreen>(this)->open_time_ms = global<u32>(d::g_game_time_ms);
        joan::orig::as_victory_screen(this)->load_sprites();                   // 0x006e8d50
        joan::orig::as_game_over(obj(d::g_game_over_screen))->hide();  // 0x006b8cf0
        return 0;  // void callee; the only caller ignores the result
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::open_1080", 0x006e8c80, &H::open_1080);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {

struct H {
    void reset_6e8cf0() {
        jo::self<jo::EndScreen>(this)->open = 0;
        joan::orig::as_panel(this)->release_textures4();  // 0x006e8e40
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Obj6e8cf0::reset", 0x006e8cf0, &H::reset_6e8cf0);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

void* const kRes = obj(d::g_texture_array);

// 0x006099c0 resource load (3 args) through thunk 0x00404d13.
void* res_load(const char* name) { return joan::orig::as_texture_manager(kRes)->load(joan::orig::arg::str(name), 1, 0u); }

JOAN_COUNTER(c_load_victory, "Obj::load_victory");

struct H {
    // ------------------------------------------------------------------ 0x006e8d50
    void load_victory() {
        c_load_victory.hit();
        const i32 saved = joan::orig::arg::as<i32>(joan::orig::as_texture_manager(kRes)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kRes)->set_mode(1u);                      // 0x0060a740
        if (jo::self<jo::EndScreen>(this)->victory_image == nullptr) jo::self<jo::EndScreen>(this)->victory_image = res_load(str(d::s_VICTORY));    // "VICTORY"
        if (jo::self<jo::EndScreen>(this)->victory2_image == nullptr) jo::self<jo::EndScreen>(this)->victory2_image = res_load(str(d::s_VICTORY2));    // "VICTORY2"
        if (jo::self<jo::EndScreen>(this)->victory3_image == nullptr) jo::self<jo::EndScreen>(this)->victory3_image = res_load(str(d::s_VICTORY3));    // "VICTORY3"
        if (jo::self<jo::EndScreen>(this)->victory_e_image == nullptr) jo::self<jo::EndScreen>(this)->victory_e_image = res_load(str(d::s_VICTORYE));  // "VICTORYE"
        joan::orig::as_texture_manager(kRes)->set_mode(joan::orig::arg::u32(saved));                  // 0x0060a740
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::load_victory", 0x006e8d50, &H::load_victory);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {

void* const kTextures = obj(d::g_texture_array);

void release_texture(void* t) { joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(t)); }         // 0x0060a550

struct H {
    // ------------------------------------------------------------------ 0x006e8e40
    void release_textures4() {
        if (jo::self<jo::EndScreen>(this)->victory_image != nullptr) {
            release_texture(joan::orig::arg::mptr(jo::self<jo::EndScreen>(this)->victory_image));
            jo::self<jo::EndScreen>(this)->victory_image = nullptr;
        }
        if (jo::self<jo::EndScreen>(this)->victory2_image != nullptr) {
            release_texture(joan::orig::arg::mptr(jo::self<jo::EndScreen>(this)->victory2_image));
            jo::self<jo::EndScreen>(this)->victory2_image = nullptr;
        }
        if (jo::self<jo::EndScreen>(this)->victory3_image != nullptr) {
            release_texture(joan::orig::arg::mptr(jo::self<jo::EndScreen>(this)->victory3_image));
            jo::self<jo::EndScreen>(this)->victory3_image = nullptr;
        }
        if (jo::self<jo::EndScreen>(this)->victory_e_image != nullptr) {
            release_texture(joan::orig::arg::mptr(jo::self<jo::EndScreen>(this)->victory_e_image));
            jo::self<jo::EndScreen>(this)->victory_e_image = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("Panel::release_textures4", 0x006e8e40, &H::release_textures4);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::VictoryScreen::release() -> void { joan::bind::member<&::game_b93_06::H::reset_6e8b90>(this); }  // 0x6e8b90
auto joan::orig::VictoryScreen::tick() -> void { joan::bind::member<&::game_ui6::H::overlay_tick>(this); }  // 0x6e8c10
auto joan::orig::VictoryScreen::open() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::open_1080>(this)); }  // 0x6e8c80
auto joan::orig::VictoryScreen::close() -> void { joan::bind::member<&::game_b84_04::H::reset_6e8cf0>(this); }  // 0x6e8cf0
auto joan::orig::VictoryScreen::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b100_16::H::byte_107e>(this)); }  // 0x6e8d20
auto joan::orig::VictoryScreen::load_sprites() -> void { joan::bind::member<&::game_b97_08::H::load_victory>(this); }  // 0x6e8d50
auto joan::orig::Panel::release_textures4() -> void { joan::bind::member<&::game_b85_05::H::release_textures4>(this); }  // 0x6e8e40
