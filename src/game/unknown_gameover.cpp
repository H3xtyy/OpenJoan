// game/unknown_gameover.cpp: functions whose original source file is unknown (formerly game/_unplaced/gameover.cpp;
// re/attribution.csv: between ww/interfac/combobox.cpp and ww/interfac/i_barr.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::img::obj;

using joan::call::cdecl_call;

using joan::img::str;

using i16 = std::int16_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

void* const kEntities = obj(d::g_units);

using joan::orig::helpers::memset_;  // memset

using joan::orig::helpers::widget_ctor;  // 0x00620ee0

i32 entity_missing(i32 id) { return joan::orig::as_dynarray(kEntities)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* entity(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kEntities)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

// Releases the unit's +0x2a1 object (0x004bb660 when +0x1d, scalar dtor 0x00415220).
void release_2a1(u8* e) {
    if (jo::as_unit(e)->orders == nullptr) return;
    void* const o = jo::as_unit(e)->orders;
    if (jo::as_unit(e)->orders->current.kind != 0) joan::orig::as_action(o)->dispatch(joan::orig::arg::mptr(e));  // 0x004bb660
    void* const p = jo::as_unit(e)->orders;
    if (p != nullptr) joan::orig::as_ai(p)->destroy(1u);  // 0x00415220
    jo::as_unit(e)->orders = nullptr;
}

void player_mode_by_state() {
    const i32 s = global<i32>(d::g_control_mode);
    if (s == 0 || s == 3)
        joan::orig::as_player(obj(d::g_player_unit_recno))->switch_unit(1);  // 0x0042b6e0
    else if (s == 1)
        joan::orig::as_player(obj(d::g_player_unit_recno))->switch_unit(0);  // 0x0042b6e0
}

JOAN_COUNTER(c_close, "Screen::close_6b8af0");

struct H {
    // ------------------------------------------------------------------ 0x006b6b70
    void* ctor_6b6b70() {
        widget_ctor(this);
        widget_ctor(&jo::self<jo::GameOver>(this)->load);  // +0x346
        widget_ctor(&jo::self<jo::GameOver>(this)->quit);  // +0x68c
        memset_(this, 0, static_cast<u32>(sizeof(jo::GameOver)));  // 0x9ed
        return this;
    }

    // ------------------------------------------------------------------ 0x006b8af0
    void close_6b8af0() {
        c_close.hit();
        jo::view<u8>(jo::self<jo::GameOver>(this)->is_open) = 1;
        joan::orig::as_game_over(this)->load_sprites();  // 0x006b6fe0
        u8 rec[0x1c] = {};
        joan::orig::as_unit_task(rec)->set(7u, static_cast<std::uint8_t>(joan::orig::arg::u32(0x7f)), 1u, 0u, 0u, 0u);  // 0x004e4520
        if (entity_missing(global<i32>(d::g_player_unit_recno)) == 0) {
            u8* const p = entity(global<i32>(d::g_player_unit_recno));
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(p)->dead()) == 0)  // 0x0042d0e0
                joan::orig::as_unit(entity(global<i32>(d::g_player_unit_recno)))->play_idle(0, 1);  // 0x00508b90
        }
        for (i32 i = joan::orig::as_dynarray(kEntities)->size(); i > 0; --i) {  // 0x00417ad0
            if (i == global<i32>(d::g_player_unit_recno)) continue;
            if (entity_missing(i) != 0) continue;
            u8* const e = entity(i);
            release_2a1(e);
            joan::orig::as_unit(e)->set_task(joan::orig::arg::mptr(static_cast<void*>(rec)));  // 0x00527fb0
            joan::orig::as_unit(e)->play_idle(0, 1);                     // 0x00508b90
        }
        player_mode_by_state();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen9ed::ctor_6b6b70", 0x006b6b70, &H::ctor_6b6b70);
B("Screen::close_6b8af0", 0x006b8af0, &H::close_6b8af0);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
namespace {
namespace game_b101_14 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

u32 colour(void* storage, u32 r, u32 g, u32 b, u32 a) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(storage)->construct(r, g, b, a));  // 0x0042a260
}

JOAN_COUNTER(c_buttons, "Screen::create_buttons");

struct H {
    // ------------------------------------------------------------------ 0x006b6c10
    void create_buttons() {
        c_buttons.hit();
        jo::self<jo::GameOver>(this)->u32_9da = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_gameover));
        jo::self<jo::GameOver>(this)->image = nullptr;
        jo::self<jo::GameOver>(this)->u32_9de = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_commele));
        jo::self<jo::GameOver>(this)->image2 = nullptr;
        static const i32 kX0[3] = {0x1e0, 0x208, 0x230};
        static const i32 kX1[3] = {0x1f8, 0x220, 0x248};
        static const std::uintptr_t kCaption[3] = {fn::code_value(fn::Menu_gameover_1), fn::code_value(fn::GameOver_open_2), fn::code_value(fn::Menu_gameover_3)};  // 0x004dcc50 / 80 / b0
        for (i32 i = 0; i < 3; ++i) {
            u8* const button = reinterpret_cast<u8*>(&jo::self<jo::GameOver>(this)->retry + i);  // retry / load / quit, 0x346 apart
            const i32 w = global<i32>(d::g_screen_w);
            const i32 h = global<i32>(d::g_screen_h);
            const i32 a4 = (h - 0x258) / 2 + kX1[i];
            const i32 a3 = (w - 0x320) / 2 + 0x1f4;
            const i32 a2 = (h - 0x258) / 2 + kX0[i];
            const i32 a1 = (w - 0x320) / 2 + 0x12c;
            joan::orig::as_button(button)->create(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), fn::code_value(fn::MenuButton_draw_6b82d0), 1, 0, 0, 1, 1, 1);  // 0x00621540
            u32 storage = 0;
            const u32 c = colour(&storage, 0xff, 0xff, 0xff, 0xff);
            const char* const caption = thiscall<const char*>(kCaption[i], obj(d::g_strings));
            joan::orig::as_button(button)->set_text(caption, img::obj(d::g_font_a489c8), joan::orig::arg::i32(c), 1, 2, 0, 2, 2);  // 0x00623ce0
        }
        jo::view<u8>(jo::self<jo::GameOver>(this)->is_open) = 0;
        jo::self<jo::GameOver>(this)->text = nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("Screen::create_buttons", 0x006b6c10, &H::create_buttons);
#undef B

}  // namespace game_b101_14
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using u8 = std::uint8_t;

using joan::orig::helpers::delete_;                                // operator delete

struct H {
    void item_release() {  // 0x006b6f50
        joan::orig::as_game_over(this)->free_sprites();                            // 0x006b70d0
        joan::orig::as_menu_button(this)->clear();                            // 0x006217e0
        joan::orig::as_menu_button(&jo::self<jo::GameOver>(this)->load)->clear();  // 0x006217e0
        joan::orig::as_menu_button(&jo::self<jo::GameOver>(this)->quit)->clear();  // 0x006217e0
        if (jo::view<void*>(jo::self<jo::GameOver>(this)->text) != nullptr) {
            delete_(jo::view<void*>(jo::self<jo::GameOver>(this)->text));
            jo::view<void*>(jo::self<jo::GameOver>(this)->text) = nullptr;
        }
    }

    void item_close() {  // 0x006b8cf0
        jo::view<u8>(jo::self<jo::GameOver>(this)->is_open) = 0;
        joan::orig::as_game_over(this)->free_sprites();  // 0x006b70d0
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("ItemScreen::release", 0x006b6f50, &H::item_release);
B94("ItemScreen::close", 0x006b8cf0, &H::item_close);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

void* const kMenus = obj(d::g_texture_array);

JOAN_COUNTER(c_gameover, "GameOver::load");

jo::Texture* menu_load(const char* name) {
    return static_cast<jo::Texture*>(joan::orig::as_texture_manager(kMenus)->load(joan::orig::arg::str(name), i32{1}, joan::orig::arg::u32(i32{0})));  // 0x006099c0
}
u32 menus_get() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kMenus)->mode()); }                // 0x0060a720
void menus_set(u32 v) { joan::orig::as_texture_manager(kMenus)->set_mode_60a740(joan::orig::arg::u32(v)); }      // 0x0060a740

struct H {
    // ------------------------------------------------------------------------ 0x006b6fe0
    u32 gameover_load() {
        c_gameover.hit();
        joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
        const u32 saved = menus_get();
        menus_set(1);
        if (jo::self<jo::GameOver>(this)->image == 0) jo::self<jo::GameOver>(this)->image = menu_load(str(d::s_GAMEOVER));  // "GAMEOVER"
        if (jo::self<jo::GameOver>(this)->image2 == 0) jo::self<jo::GameOver>(this)->image2 = menu_load(str(d::s_COMMELE));  // "COMMELE"
        menus_set(saved);
        jo::self<jo::GameOver>(this)->blink = 0;
        jo::self<jo::GameOver>(this)->blink_on = 0;
        jo::self<jo::GameOver>(this)->blink_time = 0;
        return joan::call::register_low_bits(this);  // eax residue; the only caller (close_6b8af0) ignores it
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("GameOver::load", 0x006b6fe0, &H::gameover_load);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

using i32 = std::int32_t;

void* const kRes = obj(d::g_texture_array);

void res_free(void* handle) { joan::orig::as_texture_manager(kRes)->release(handle); }  // 0x0060a550

struct H {
    void free_9d2() {  // 0x006b70d0
        if (jo::self<jo::GameOver>(this)->image != nullptr) {
            res_free(jo::self<jo::GameOver>(this)->image);
            jo::self<jo::GameOver>(this)->image = nullptr;
        }
        if (jo::self<jo::GameOver>(this)->image2 != nullptr) {
            res_free(jo::self<jo::GameOver>(this)->image2);
            jo::self<jo::GameOver>(this)->image2 = nullptr;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6b::free_9d2", 0x006b70d0, &H::free_9d2);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_ui5.cpp (game/ui5)
namespace {
namespace game_ui5 {

using joan::hook::global;
using u32 = std::uint32_t;

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue

void strcpy_(char* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void strcat_(char* d, const char* s) { std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

// 0x0042a260 Color(r, g, b, a) on a 4-byte local, returned dword copied out.
std::uint32_t color(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    std::uint32_t* const p = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
    return *p;
}
// 0x00622cc0 Button::draw(mode, flags), ret 8.
void button(void* b, std::uint32_t mode, std::uint32_t flags) { joan::orig::as_button(b)->draw(joan::orig::arg::u32(mode), static_cast<std::uint8_t>(flags)); }  // 0x00622cc0

// 0x00633500 SpriteQueue::draw_named(x, y, a3, name, sheet, colour, a7 .. a17), ret 0x44.
void sprite(std::int32_t x, std::int32_t y, std::int32_t a3, const char* name, jo::BmpRes* sheet, std::uint32_t c,
            std::int32_t a9, std::int32_t a16, std::int32_t a17) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), sheet, joan::orig::arg::u32(c), 3, 0, joan::orig::arg::i16(a9), 0, -1, -1, -1, -1, 0, joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));  // 0x00633500
}

JOAN_COUNTER(c_panel, "Panel53ff8::draw");

struct H {
    // 0x006b7160
    void panel_draw(std::uint32_t arg) {
        c_panel.hit();
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) != 0) return;  // 0x004301c0
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_shop(obj(d::g_game_a5d380))->active()) != 0) return;  // 0x0042ced0
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_fade_title)) != 0) return;
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::GameOver>(this)->is_open)) == 0) return;
        joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30

        const auto half = [](img::Addr size, std::uint32_t sub) {
            return static_cast<std::int32_t>(global<std::uint32_t>(size) - sub) / 2;
        };
        const auto flags = [this] { return static_cast<std::int32_t>(static_cast<std::int8_t>(jo::self<jo::GameOver>(this)->blink)); };
        // The centred 600x80 glow: (x, y, 2, name, sheet, colour, 3, 0, a9, 0, -1 x4, 0, -1, -1).
        const auto glow = [this](std::int32_t x, std::int32_t y, img::Addr name, std::int32_t a9, std::uint32_t c) {
            sprite(x, y, 2, str(name), jo::self<jo::GameOver>(this)->u32_9da, c, a9, -1, -1);
        };
        const auto ring = [this, &half](std::uint32_t w, std::uint32_t h, std::uint32_t a) {
            const std::int32_t x = half(d::g_screen_w, w);
            const std::int32_t right = x + static_cast<std::int32_t>(w);
            const std::int32_t y = half(d::g_screen_h, h);
            const std::int32_t bottom = y + static_cast<std::int32_t>(h);
            const std::uint32_t c = color(0xff, 0xff, 0xff, a);
            sprite(x, y, 2, str(d::s_G100), jo::self<jo::GameOver>(this)->u32_9da, c, 2, right, bottom);  // 'G100'
        };
        const auto glow_pair = [&](std::uint32_t a) {
            const std::int32_t x = half(d::g_screen_w, 0x258);
            const std::int32_t y = half(d::g_screen_h, 0x50);
            glow(x - 0x14, y - 0x14, d::s_GGLOW, 2, color(0xff, 0xff, 0xff, a));  // 'GGLOW'
            glow(x, y, d::s_G50, 0, color(0xff, 0xff, 0xff, 0xff));             // 'G50'
        };

        if ((flags() & 0x10) != 0) {
            const std::int32_t x = half(d::g_screen_w, 0x258);
            const std::int32_t y = half(d::g_screen_h, 0x50);
            glow(x, y, d::s_G50, 0, color(0xff, 0xff, 0xff, 0xff));  // 'G50'
        }
        if ((flags() & 0x8) != 0) ring(0x2d0, 0x60, 0xcc);
        if ((flags() & 0x4) != 0) ring(0x348, 0x70, 0xb2);
        if ((flags() & 0x2) != 0) ring(0x3c0, 0x80, 0x99);
        if ((flags() & 0x1) != 0) ring(0x438, 0x90, 0x7f);
        if ((flags() & 0x20) != 0) glow_pair(0x7f);
        if (flags() == 0x40) glow_pair(0xbf);
        if (flags() == 0x60) glow_pair(0xff);

        if (static_cast<std::int8_t>(jo::self<jo::GameOver>(this)->blink_on) != 0) {
            // The original also computes min(255, (b * 25 + 25) * 255 / 100) into a local that is
            // never read (the box colour is the constant 0xffffffff).
            std::uint32_t c = 0;
            c = *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_color(&c)->Color_dword(0xffffffffu));  // 0x005e9640
            const std::int32_t y2 = half(d::g_screen_h, 0x258) + 0x1f6;
            const std::int32_t x2 = half(d::g_screen_w, 0x320) + 0x2e4;
            const std::int32_t y1 = half(d::g_screen_h, 0x258) + 0x16e;
            const std::int32_t x1 = half(d::g_screen_w, 0x320) + 0x3c;
            joan::orig::as_text_renderer(obj(d::g_font_goudy18_27_white))->draw_wrapped(joan::orig::arg::u32(x1), joan::orig::arg::u32(y1), joan::orig::arg::u32(x2), joan::orig::arg::u32(y2), 2u, joan::orig::arg::str(jo::self<jo::GameOver>(this)->text), joan::orig::arg::u32(c), 2u, 0u, 0u, 0u, 1u, 1.0f, 1.0f, 2u);  // 0x00626e00
        }
        if (static_cast<std::int8_t>(jo::self<jo::GameOver>(this)->blink_on) != 4) return;
        const std::uint32_t mode = arg & 0xffu;
        if (global<std::int32_t>(d::g_mission) == 6) {
            char file[0x108];
            const char* const name =
                joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->autosave_text_1(joan::orig::arg::u32(global<std::uint32_t>(d::g_mission))));  // 0x004dd0e0
            strcpy_(file, name);
            strcat_(file, str(d::s_SAV_9cfb78));  // '.SAV'
            std::uint8_t* const save = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_paths(obj(d::g_saves))->find(joan::orig::arg::str(file)));  // 0x00477640
            if (save == nullptr || jo::view<std::int32_t>(jo::self<jo::GameFile>(save)->u32_19a) != 6 ||
                static_cast<std::int8_t>(jo::self<jo::GameFile>(save)->u8_19e) != 1) {
                joan::orig::as_widget(this)->reset_child();  // 0x0062b660
            } else {
                joan::orig::as_panel(this)->reset_6b7cf0();  // 0x006b7cf0
                button(this, 2, mode);
            }
        } else {
            button(this, 2, mode);
        }
        button(&jo::self<jo::GameOver>(this)->load, 2, mode);
        button(&jo::self<jo::GameOver>(this)->quit, 2, mode);
    }
};

#define UI5(name, addr, fn) JOAN_REPLACE("game/ui5", name, addr, fn)
UI5("Panel53ff8::draw", 0x006b7160, &H::panel_draw);
#undef UI5

}  // namespace game_ui5
}  // namespace

// ---------------------------------------- from modules/game_b82_12.cpp (game/b82_12)
namespace {
namespace game_b82_12 {

struct H {
    // 0x006b7cf0 (ret); eax = this.
    void* panel_reset() {
        joan::orig::as_widget_child(&jo::self<jo::GameOver>(this)->retry.child)->set_flag_4();  // 0x006b7d30
        jo::self<jo::GameOver>(this)->retry.drawn = 0;
        return this;
    }
};

#define B12(name, addr, fn) JOAN_REPLACE("game/b82_12", name, addr, fn)
B12("Panel::reset", 0x006b7cf0, &H::panel_reset);
#undef B12

}  // namespace game_b82_12
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

struct H {
    // 0x006b7d30
    void set_4() { jo::self<jo::WidgetChild>(this)->flag_4 = 1; }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("Obj6b7d30::set_4", 0x006b7d30, &H::set_4);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

JOAN_COUNTER(c_handle_6b7d50, "Menu::handle_6b7d50");

struct H {
    // --------------------------------------------------------------------- 0x006b7d50
    u8 handle_6b7d50() {
        c_handle_6b7d50.hit();
        if (jo::self<jo::GameOver>(this)->is_open == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(this)->mouse(i32{0}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
            if (global<u32>(d::g_mission) != 0) {
                joan::orig::as_game_over(this)->hide();  // 0x006b8cf0
                joan::orig::as_battle(obj(d::g_scenarios))->load_scenario_41cc80(joan::orig::arg::i32(global<u32>(d::g_mission)), joan::orig::arg::i32(global<u32>(d::g_stage_index)), i32{0});  // 0x0041cc80
                joan::orig::as_game(obj(d::g_game_a50088))->set_state(joan::orig::arg::u32(i32{0xc}));  // 0x004d7e70
                if (global<u32>(d::g_mission) == 6) {
                    char buf[0x108];
                    const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->autosave_text_1(joan::orig::arg::u32(global<u32>(d::g_mission))));  // 0x004dd0e0
                    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(name));  // strcpy
                    std::strcat(static_cast<char*>(joan::orig::arg::mptr(buf)), str(d::s_SAV_9cfb78));  // strcat ".SAV"
                    jo::GameFile* const v = static_cast<jo::GameFile*>(joan::orig::as_paths(obj(d::g_saves))->find(joan::orig::arg::str(static_cast<const char*>(buf))));  // 0x00477640
                    joan::orig::as_progress(obj(d::g_progress))->set_image(v);  // 0x007563f0
                }
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::GameOver>(this)->load)->mouse(i32{0}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
            joan::orig::as_game_over(this)->hide();  // 0x006b8cf0
            global<u8>(d::g_savemenu_load_mode) = 1;
            joan::orig::as_save_menu(obj(d::g_save_menu))->open(static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x0074da40
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::GameOver>(this)->quit)->mouse(i32{0}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
            joan::orig::as_quit_popup(obj(d::g_quit_popup))->show(joan::orig::arg::u32(i32{0}));  // 0x006e4c00
            joan::orig::as_game_over(this)->hide();                    // 0x006b8cf0
            return 1;
        }
        return 1;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Menu::handle_6b7d50", 0x006b7d50, &H::handle_6b7d50);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b101_15.cpp (game/b101_15)
namespace {
namespace game_b101_15 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::hook::global;
using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_blink, "Blink::step");

// Shared body of 0x006b7f20 / 0x006e7cf0. The original reads one block from the owner's blink fields: +0 state
// (char), +1 phase (char), +2 last time (dword), +0xa enabled (char, the owner's open flag). Between the time and the
// flag lies the owner's text pointer, wider on x64, so each caller passes its own fields instead of a Blink view.
void blink_step(i8& st, i8& ph, u32& last_time, i8 enabled) {
    c_blink.hit();
    if (static_cast<i32>(enabled) == 0) return;
    if (static_cast<u32>(global<u32>(d::g_game_time_ms) - last_time) <= 0x32u) return;
    last_time = global<u32>(d::g_game_time_ms);
    if (static_cast<i32>(st) == 0) {
        if (static_cast<i32>(ph) == 0) st = 1;
        if (static_cast<i32>(ph) == 3) ph = 4;
        if (static_cast<i32>(ph) == 2) ph = 3;
        if (static_cast<i32>(ph) == 1) ph = 2;
        return;
    }
    const i32 s = st;
    if (s == 0x01) { st = 0x03; return; }
    if (s == 0x03) { st = 0x07; return; }
    if (s == 0x07) { st = 0x0f; return; }
    if (s == 0x0f) { st = 0x1f; return; }
    if (s == 0x1f) { st = 0x1e; return; }
    if (s == 0x1e) { st = 0x1c; return; }
    if (s == 0x1c) { st = 0x18; return; }
    if (s == 0x18) { st = 0x20; return; }
    if (static_cast<i32>(st) == 0x10 && static_cast<i32>(ph) == 3) ph = 4;
    if (static_cast<i32>(st) == 0x20) {
        if (static_cast<i32>(ph) == 0) {
            st = 0x40;
        } else {
            st = 0x10;
            ph = 3;
        }
        return;
    }
    if (static_cast<i32>(st) == 0x40) {
        if (static_cast<i32>(ph) == 0) {
            st = 0x60;
        } else {
            st = 0x20;
            ph = 2;
        }
        return;
    }
    if (static_cast<i32>(st) == 0x60) {
        st = 0x40;
        ph = 1;
    }
}

struct H {
    // 0x006b7f20 (ret)
    void blink_9e2() {
        jo::GameOver* const g = jo::self<jo::GameOver>(this);
        blink_step(reinterpret_cast<i8&>(g->blink), reinterpret_cast<i8&>(g->blink_on), g->blink_time, g->is_open);
    }
};

#define B15(name, addr, fn) JOAN_REPLACE("game/b101_15", name, addr, fn)
B15("Blink9e2::step", 0x006b7f20, &H::blink_9e2);
#undef B15

}  // namespace game_b101_15
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

struct H {
    void nop0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("Obj::nop_6b8240", 0x006b8240, &H::nop0);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

using u32 = std::uint32_t;

using joan::orig::helpers::op_new;

struct H {
    // ------------------------------------------------------------------ 0x006b8260
    char* set_text_6b8260(const char* s) {
        if (jo::self<jo::GameOver>(this)->text == nullptr) jo::view<void*>(jo::self<jo::GameOver>(this)->text) = op_new(0x100);
        return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::GameOver>(this)->text)), joan::orig::arg::str(s), 0xffu);  // strncpy
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("Obj6b8::set_text", 0x006b8260, &H::set_text_6b8260);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b109_08.cpp (game/b109_08)
namespace {
namespace game_b109_08 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

constexpr u32 kOne = 0x3f800000u;  // 1.0f

using i8 = std::int8_t;

u32 hi(u32 v) { return v & 0xffff0000u; }
u32 lo16(i16 v) { return static_cast<u32>(static_cast<u16>(v)); }

JOAN_COUNTER(c_mbut_draw, "MenuButton::draw_6b82d0");

void* const kSprites = obj(d::g_sprites_a46128);

// ---- shared draw helpers ----------------------------------------------------------------
// 0x0042a260 (thunk 0x004098b8, ret 0x10): packs r, g, b, a into the 4-byte colour object.
u32 packed_colour(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));
}
// 0x005e9640 (thunk 0x00407577, ret 4): 4-byte colour object from a 0xAARRGGBB dword.
u32 argb_colour(u32 argb) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&tmp)->Color_dword(joan::orig::arg::u32(argb)));
}
// 0x00633500 (thunk 0x00401bbd, ret 0x44) with the trailing (3, 0, mode, 0, -1 x4, 0, -1, -1).
void sprite(u32 x, u32 y, u32 a3, const char* name, jo::BmpRes* sheet, u32 colour, i32 mode) {
    joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), joan::orig::arg::mptr(sheet), joan::orig::arg::u32(colour), 3, 0, joan::orig::arg::i16(mode), 0, -1, -1, -1, -1, 0, -1, -1);
}
// 0x00593980 (thunk 0x00403e4f, ret 0x10): mouse inside the rectangle.
i32 mouse_in(i16 x, i16 y, i16 w, i16 h) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(x), static_cast<i32>(y), static_cast<i32>(w), static_cast<i32>(h)));
}
// Scaled widgets: (short)0x00590a20(0xa45fa8, (double)(1.0f * scale)) when +0x8a is set, else 0.
i32 press_offset(const u8* p, float scale) {
    if (jo::as_button(p)->pressed == 0) return 0;
    const float v = global<float>(d::flt_1) * scale;
    return static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(v)))));  // 0x00590a20
}
// 0x00626130 (thunk 0x00402e0f, ret 0x34): text in a box.
void box_text(void* font, i32 x, i32 y, u32 w, u32 h, u32 a5, const void* text, u32 colour, u32 a8, u32 a9, u32 a10,
              u32 a11) {
    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(a5), joan::orig::arg::str(text), joan::orig::arg::u32(colour), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9), joan::orig::arg::i32(a10), joan::orig::arg::i32(a11), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
}
// The widget's rectangle (+9 / +0xb / +0xd / +0xf) and scale, rescaled by 0x00633e60 when +6.
struct Rect {
    i16 x, y, w, h;  // [ebp-4] / [ebp-8] / [ebp-0xc] / [ebp-0x10]
    float sx, sy;    // [ebp-0x14] / [ebp-0x18]
};
Rect widget_rect(u8* p) {
    Rect r{jo::as_button(p)->x, jo::as_button(p)->y, jo::as_button(p)->x2, jo::as_button(p)->y2, 1.0f, 1.0f};
    if (jo::as_button(p)->fit != 0) {
        joan::orig::as_sprite2d(kSprites)->fit_rect(joan::orig::arg::i16(jo::as_button(p)->x), joan::orig::arg::i16(jo::as_button(p)->y), joan::orig::arg::i16(jo::as_button(p)->x2), joan::orig::arg::i16(jo::as_button(p)->y2), &r.sx, &r.sy, &r.x, &r.y, &r.w, &r.h, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    }
    return r;
}

// ---- 0x006b82d0: menu button draw callback (cdecl) ----------------------------------------
u32 JOAN_CDECL mbut_draw(u8* p, u32 arg, u32 flags) {
    const u32 fl = flags & 0xff;
    if (fl != 7 && (fl & 1) != 0) return 0;
    c_mbut_draw.hit();
    Rect r = widget_rect(p);
    auto face = [&](img::Addr name) {
        const u32 c = packed_colour(0xff, 0xff, 0xff, 0xff);
        jo::BmpRes* const sheet = global<jo::BmpRes*>(d::g_a549d6);
        sprite(hi(joan::call::register_low_bits(sheet)) | lo16(r.x), hi(c) | lo16(r.y), arg, str(name), sheet, c, 0);
    };
    if (jo::as_button(p)->pressed != 0) {
        face(d::s_MBUT_DW);  // "MBUT_DW"
    } else if (mouse_in(r.x, r.y, r.w, r.h) != 0) {
        face(d::s_MBUT_OV);  // "MBUT_OV"
    } else {
        face(d::s_MBUT_OT);  // "MBUT_OT"
    }
    {
        const u32 c = packed_colour(0xff, 0xff, 0xff, 0xff);
        jo::BmpRes* const sheet = global<jo::BmpRes*>(d::g_a549d6);
        sprite(static_cast<u32>(static_cast<i32>(r.x) - 4), static_cast<u32>(static_cast<i32>(r.y) - 4), arg,
               str(d::s_MBUT_CON), sheet, c, 0);  // "MBUT_CON"
    }
    if (jo::view<i8>(jo::as_button(p)->caption[0]) == 0) return 1;
    u8* const text = reinterpret_cast<u8*>(jo::as_button(p)->caption);  // +0x227
    auto label = [&]() {
        const i32 dy = press_offset(p, r.sy);
        const i32 dx = press_offset(p, r.sx);
        const u32 c = argb_colour(0xff003366u);
        box_text(obj(d::g_font_a489c8), static_cast<i32>(r.x) + dx, static_cast<i32>(r.y) + dy, hi(joan::call::register_low_bits(text)) | lo16(r.w),
                 hi(c) | lo16(r.h), arg, text, c, 0, 0, 2, 0);
    };
    if (jo::as_button(p)->pressed == 0 && mouse_in(r.x, r.y, r.w, r.h) != 0) {
        label();
        const i32 n = static_cast<i16>(
            joan::orig::arg::as<i32>(joan::orig::as_font(obj(d::g_text_a58ac0))->measure(joan::orig::arg::str(text), -1, 0, 0, joan::orig::arg::f32(kOne))));  // 0x00624f00 text width
        const i32 left0 = static_cast<i32>(jo::as_button(p)->x2) - static_cast<i32>(jo::as_button(p)->x) - n;
        const i32 left = left0 / 2 + static_cast<i32>(jo::as_button(p)->x) - 0x19;  // [ebp-0x20]
        const i32 right = left + n + 0x19;                                       // [ebp-0x24]
        {
            const u32 c = packed_colour(0xff, 0xff, 0xff, 0xff);
            jo::BmpRes* const sheet = global<jo::BmpRes*>(d::g_a549d6);
            sprite(hi(arg) | (static_cast<u32>(left) & 0xffffu), static_cast<u32>(static_cast<i32>(jo::as_button(p)->y) + 1),
                   arg, str(d::s_MBUT_MOT), sheet, c, 0);  // "MBUT_MOT"
        }
        {
            const u32 c = packed_colour(0xff, 0xff, 0xff, 0xff);
            jo::BmpRes* const sheet = global<jo::BmpRes*>(d::g_a549d6);
            sprite(hi(arg) | (static_cast<u32>(right) & 0xffffu),
                   static_cast<u32>(static_cast<i32>(jo::as_button(p)->y) + 1), arg, str(d::s_MBUT_MOT), sheet, c, 0);
        }
        return 1;
    }
    label();
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_08", name, addr, fn)
B("MenuButton::draw_6b82d0", 0x006b82d0, &mbut_draw);
#undef B

}  // namespace game_b109_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::GameOver::release() -> void { joan::bind::member<&::game_b94_08::H::item_release>(this); }  // 0x6b6f50
auto joan::orig::GameOver::load_sprites() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::gameover_load>(this)); }  // 0x6b6fe0
auto joan::orig::GameOver::free_sprites() -> void { joan::bind::member<&::game_b93_06::H::free_9d2>(this); }  // 0x6b70d0
auto joan::orig::GameOver::draw(std::uint32_t a0) -> void { joan::bind::member<&::game_ui5::H::panel_draw>(this, a0); }  // 0x6b7160
auto joan::orig::Panel::reset_6b7cf0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_12::H::panel_reset>(this)); }  // 0x6b7cf0
auto joan::orig::WidgetChild::set_flag_4() -> void { joan::bind::member<&::game_b83_04::H::set_4>(this); }  // 0x6b7d30
auto joan::orig::GameOver::set_text(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_15::H::set_text_6b8260>(this, a0)); }  // 0x6b8260
auto joan::orig::Screen::close_6b8af0() -> void { joan::bind::member<&::game_b102_15::H::close_6b8af0>(this); }  // 0x6b8af0
auto joan::orig::GameOver::hide() -> void { joan::bind::member<&::game_b94_08::H::item_close>(this); }  // 0x6b8cf0
