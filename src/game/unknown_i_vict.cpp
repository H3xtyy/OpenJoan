// game/unknown_i_vict.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_vict.cpp;
// re/attribution.csv: between ww/interfac/i_sms.cpp and ww/interfac/i_work.cpp). They move to their original file when
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
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../canvas_api.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "mission_count.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

using joan::call::cdecl_call;

using i16 = std::int16_t;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using joan::orig::helpers::memset_;  // memset

void* const kEntities = obj(d::g_units);
void* const kText = obj(d::g_strings);

using joan::orig::helpers::widget_ctor;  // 0x00620ee0

i32 entity_missing(i32 id) { return joan::orig::as_dynarray(kEntities)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* entity(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kEntities)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

// Releases the unit's +0x2a1 object (0x004bb660 when +0x1d, scalar dtor 0x00415220).
void release_2a1(u8* e) {
    if (jo::as_unit(e)->orders == nullptr) return;
    void* const o = jo::as_unit(e)->orders;
    if (jo::view<u32>(jo::as_order(o)->current.kind) != 0) joan::orig::as_action(o)->dispatch(joan::orig::arg::mptr(e));  // 0x004bb660
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

JOAN_COUNTER(c_win, "Screen::win_6e7900");

struct H {
    // ------------------------------------------------------------------ 0x006e6830
    void* ctor_6e6830() {
        widget_ctor(&jo::self<jo::Glory>(this)->button0);
        widget_ctor(&jo::self<jo::Glory>(this)->button1);
        widget_ctor(&jo::self<jo::Glory>(this)->button2);
        widget_ctor(&jo::self<jo::Glory>(this)->button3);
        memset_(this, 0, sizeof(jo::Glory));  // 0xd33 on i686
        return this;
    }

    // ------------------------------------------------------------------ 0x006e7900
    void win_6e7900() {
        c_win.hit();
        jo::self<jo::Glory>(this)->done = 1;
        joan::orig::as_menu(this)->load_vict();              // 0x006e7b80
        joan::orig::as_message_list(obj(d::g_names))->reset();   // 0x004ae6c0
        if (const u32 mission = global<u32>(d::g_mission);
            mission != 0 && joan::mission_count::has_victory_text(kText, static_cast<i32>(mission))) {  // [Fixes] MissionCount
            const char* const t = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kText)->victs_text(joan::orig::arg::i32(mission)));  // 0x004dd9c0
            joan::orig::as_glory_screen(this)->set_text(t);                                     // 0x006e8010
        } else {
            joan::orig::as_glory_screen(this)->set_text(str(d::s_Yow_win));  // 0x006e8010 "Yow win!"
        }
        u8 rec[0x1c] = {};
        joan::orig::as_unit_task(rec)->set(7u, static_cast<std::uint8_t>(joan::orig::arg::u32(0x7f)), 1u, 0u, 0u, 0u);  // 0x004e4520
        if (entity_missing(global<i32>(d::g_player_unit_recno)) == 0)
            joan::orig::as_unit(entity(global<i32>(d::g_player_unit_recno)))->play_idle(0, 1);  // 0x00508b90
        for (i32 i = joan::orig::as_dynarray(kEntities)->size(); i > 0; --i) {  // 0x00417ad0
            if (entity_missing(i) != 0) continue;
            u8* const e = entity(i);
            if (global<u32>(d::g_mission) != 5) {
                release_2a1(e);
                joan::orig::as_unit(e)->set_task(joan::orig::arg::mptr(static_cast<void*>(rec)));  // 0x00527fb0
            }
            joan::orig::as_unit(e)->play_idle(0, 1);  // 0x00508b90
        }
        player_mode_by_state();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen::ctor_6e6830", 0x006e6830, &H::ctor_6e6830);
B("Screen::win_6e7900", 0x006e7900, &H::win_6e7900);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b104_14.cpp (game/b104_14)
namespace {
namespace game_b104_14 {

// Rgba(r, g, b, a) 0x0042a260 / Rgba(dword) 0x005e9640 into a local, value returned
u32 colour(u32 r, u32 g, u32 b, u32 a) {
    u32 storage = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&storage)->construct(r, g, b, a));  // 0x0042a260
}
u32 button(void* item, u32 a1, u32 a2, u32 a3, u32 a4, std::uintptr_t cb, u32 b1, u32 b2, u32 b3, u32 b4, u32 b5,
           u32 b6) {
    return joan::orig::arg::as<u32>(joan::orig::as_button(item)->create(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), cb, joan::orig::arg::i32(b1), joan::orig::arg::i32(b2), joan::orig::arg::i32(b3), joan::orig::arg::i32(b4), joan::orig::arg::i32(b5), joan::orig::arg::i32(b6)));  // 0x00621540
}
// colour(ff,ff,ff,ff), caption getter on 0xa57c08, TextDesc::set 0x00623ce0
u32 caption(void* item, std::uintptr_t getter, img::Addr font) {
    const u32 col = colour(0xff, 0xff, 0xff, 0xff);
    const char* const str = thiscall<const char*>(getter, obj(d::g_strings));
    return joan::orig::arg::as<u32>(joan::orig::as_button(item)->set_text(str, img::obj(font), joan::orig::arg::i32(col), 1, 2, 0, 2, 2));  // 0x00623ce0
}

JOAN_COUNTER(c_menu4, "Menu4::init_6e68f0");

struct H {
    // ------------------------------------------------------------------ 0x006e68f0
    H* init_6e68f0() {
        c_menu4.hit();
        jo::self<jo::Glory>(this)->sprites = static_cast<jo::BmpRes*>(obj(d::g_bmp_res_vict));
        jo::self<jo::Glory>(this)->button_sprites = static_cast<jo::BmpRes*>(obj(d::g_bmp_commele));
        jo::self<jo::Glory>(this)->u32_d18 = nullptr;
        jo::self<jo::Glory>(this)->u32_d1c = nullptr;
        static jo::Button jo::Glory::* const kButtons[4] = {&jo::Glory::button3, &jo::Glory::button0, &jo::Glory::button1, &jo::Glory::button2};
        static const i32 kY0[4] = {0x208, 0x212, 0x23a, 0x262};
        static const i32 kY1[4] = {0x220, 0x22a, 0x252, 0x27a};
        static const std::uintptr_t kCap[4] = {fn::code_value(fn::Vict_open_1), fn::code_value(fn::Menu_vict_2), fn::code_value(fn::TxtRes_vict_3),
                                               fn::code_value(fn::Vict_open_4)};  // 0x004db9a0 / 9d0 / a00 / a30
        for (i32 i = 0; i < 4; ++i) {
            u8* const item = reinterpret_cast<u8*>(&(jo::self<jo::Glory>(this)->*kButtons[i]));
            const i32 w = global<i32>(d::g_screen_w);
            const i32 h = global<i32>(d::g_screen_h);
            const i32 a4 = (h - 0x258) / 2 + kY1[i];
            const i32 a3 = (w - 0x320) / 2 + 0x1f4;
            const i32 a2 = (h - 0x258) / 2 + kY0[i];
            const i32 a1 = (w - 0x320) / 2 + 0x12c;
            button(item, static_cast<u32>(a1), static_cast<u32>(a2), static_cast<u32>(a3),
                   static_cast<u32>(a4), fn::code_value(fn::mbut_draw_6e80b0), 1, 0, 0, 1, 1, 1);  // cb 0x006e80b0
            caption(item, kCap[i], d::g_font_a489c8);
        }
        jo::self<jo::Glory>(this)->done = 0;
        jo::self<jo::Glory>(this)->text = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_14", name, addr, fn)
B("Menu4::init_6e68f0", 0x006e68f0, &H::init_6e68f0);
#undef B

}  // namespace game_b104_14
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

struct H {
    // 0x006e6d20 (this: the Glory screen; its fields through jo::Glory, the object's layout - the four-field
    // VictoryScreen view put the text pointer 8 bytes short of it on x64 and operator delete got -1 / 0)
    void screen_shutdown() {
        joan::orig::as_panel(this)->release_textures2();               // 0x006e7c60
        joan::orig::as_menu_button(this)->clear();               // 0x006217e0
        joan::orig::as_menu_button(&jo::self<jo::Glory>(this)->button1)->clear();
        joan::orig::as_menu_button(&jo::self<jo::Glory>(this)->button2)->clear();
        joan::orig::as_menu_button(&jo::self<jo::Glory>(this)->button3)->clear();
        if (jo::self<jo::Glory>(this)->text != nullptr) {
            joan::crt::operator_delete(joan::orig::arg::mptr(jo::self<jo::Glory>(this)->text));  // operator delete
            jo::self<jo::Glory>(this)->text = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Screen::shutdown_6e6d20", 0x006e6d20, &H::screen_shutdown);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_ui6.cpp (game/ui6)
namespace {
namespace game_ui6 {

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue
constexpr std::int32_t M1 = -1;

using joan::orig::helpers::screen_w;
using joan::orig::helpers::screen_h;

// 0x005e9640 Color(dword) on a 4-byte local, the dword copied out. ret 4.
std::uint32_t colour_of(std::uint32_t v) {
    std::uint32_t c = 0;
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_color(&c)->Color_dword(joan::orig::arg::u32(v)));  // 0x005e9640
}
// 0x0042a260 Color(r, g, b, a). ret 0x10.
std::uint32_t colour4(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
}

// 0x00633500 on the sprite queue (17 dword arguments). ret 0x44.
void sprite(std::int32_t x, std::int32_t y, std::int32_t layer, const char* name, void* texture, std::uint32_t colour,
            std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10, std::int32_t a11, std::int32_t a12,
            std::int32_t a13, std::int32_t a14, std::int32_t a15, std::int32_t a16, std::int32_t a17) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(layer), joan::orig::arg::str(name), joan::orig::arg::mptr(texture), joan::orig::arg::u32(colour), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(a15), joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));  // 0x00633500
}

JOAN_COUNTER(c_glory, "Glory::draw");

struct H {

    // ---- 0x006e6dc0 helpers ----
    // G100 ring: centred w x h box, alpha.
    void ring(std::int32_t w, std::int32_t h, std::uint32_t alpha) {
        const std::int32_t x = (screen_w() - w) / 2;
        const std::int32_t x2 = x + w;
        const std::int32_t y = (screen_h() - h) / 2;
        const std::int32_t y2 = y + h;
        const std::uint32_t c = colour4(0xff, 0xff, 0xff, alpha);
        sprite(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 1, str(d::s_G100), jo::self<jo::Glory>(this)->sprites, c, 3, 0, 2, 0,
               M1, M1, M1, M1, 0, static_cast<std::int16_t>(x2), static_cast<std::int16_t>(y2));  // 'G100'
    }
    // GGLOW (alpha) + G50 at the centred 0x200 x 0x80 box.
    void glow(std::uint32_t alpha) {
        const std::int32_t x = (screen_w() - 0x200) / 2;
        const std::int32_t y = (screen_h() - 0x80) / 2;
        const std::uint32_t c = colour4(0xff, 0xff, 0xff, alpha);
        sprite(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 1, str(d::s_GGLOW), jo::self<jo::Glory>(this)->sprites, c, 3, 0, 2, 0,
               M1, M1, M1, M1, 0, M1, M1);  // 'GGLOW'
        const std::uint32_t c2 = colour4(0xff, 0xff, 0xff, 0xff);
        sprite(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 1, str(d::s_G50), jo::self<jo::Glory>(this)->sprites, c2, 3, 0, 0, 0,
               M1, M1, M1, M1, 0, M1, M1);  // 'G50'
    }
    std::int32_t combo() { return static_cast<std::int8_t>(jo::self<jo::Glory>(this)->blink_on); }
    std::int32_t level() { return static_cast<std::int8_t>(jo::self<jo::Glory>(this)->blink_state); }

    // 0x006e6dc0 Glory::draw. ret 4.
    void glory_draw(std::uint8_t arg) {
        joan::canvas::ScreenScope canvas_scope(0x006e6dc0);  // canvas: a panel centred on the screen (ui_center, UiScale)
        c_glory.hit();
        if (static_cast<std::int8_t>(joan::orig::arg::as<char>(joan::orig::as_game(obj(d::g_game_state))->flag_744())) != 0) return;  // 0x004301c0
        if (static_cast<std::int8_t>(joan::orig::arg::as<char>(joan::orig::as_shop(obj(d::g_game_a5d380))->active())) != 0) return;  // 0x0042ced0
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_fade_title)) != 0) return;
        if (static_cast<std::int8_t>(jo::self<jo::Glory>(this)->done) == 0) return;
        joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
        if (combo() == 0x10) {
            const std::int32_t x = (screen_w() - 0x200) / 2;
            const std::int32_t y = (screen_h() - 0x80) / 2;
            const std::uint32_t c = colour4(0xff, 0xff, 0xff, 0xff);
            sprite(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 1, str(d::s_G50), jo::self<jo::Glory>(this)->sprites, c, 3, 0, 0, 0,
                   M1, M1, M1, M1, 0, M1, M1);  // 'G50'
        }
        if ((combo() & 8) != 0) ring(0x266, 0x99, 0xcc);
        if ((combo() & 4) != 0) ring(0x2cc, 0xb3, 0xb2);
        if ((combo() & 2) != 0) ring(0x333, 0xcc, 0x99);
        if ((combo() & 1) != 0) ring(0x399, 0xe6, 0x7f);
        if (combo() == 0x20) glow(0x7f);
        if (combo() == 0x40) glow(0xbf);
        if (combo() == 0x60) glow(0xff);
        if (level() != 0 && level() != 8) {
            // (level * 25 + 25) * 255 / 100 clamped to 255: computed and unused by the original.
            const std::uint32_t c = colour_of(0xffffffffu);
            const char* const portrait = jo::self<jo::Glory>(this)->text;
            const std::int32_t y2 = (screen_h() - 0x258) / 2 + 0x1f6;
            const std::int32_t x2 = (screen_w() - 0x320) / 2 + 0x2e4;
            const std::int32_t y1 = (screen_h() - 0x258) / 2 + 0x16e;
            const std::int32_t x1 = (screen_w() - 0x320) / 2 + 0x3c;
            joan::orig::as_text_renderer(obj(d::g_font_goudy18_27_white))->draw_wrapped(joan::orig::arg::u32(x1), joan::orig::arg::u32(y1), joan::orig::arg::u32(x2), joan::orig::arg::u32(y2), 1u, joan::orig::arg::str(portrait), joan::orig::arg::u32(c), 2u, 0u, 0u, 0u, 1u, 1.0f, 1.0f, 2u);  // 0x00626e00
        }
        const std::uint32_t a = arg;
        if (level() == 4) {
            jo::view<std::uint8_t>(jo::self<jo::Glory>(this)->button3.i8_8) = 1;
            joan::orig::as_button(&jo::self<jo::Glory>(this)->button3)->draw(1u, static_cast<std::uint8_t>(a));  // 0x00622cc0
        } else {
            jo::view<std::uint8_t>(jo::self<jo::Glory>(this)->button3.i8_8) = 0;
        }
        if (level() == 8) {
            jo::view<std::uint8_t>(jo::self<jo::Glory>(this)->button0.i8_8) = 1;
            joan::orig::as_button(this)->draw(1u, static_cast<std::uint8_t>(a));
            jo::view<std::uint8_t>(jo::self<jo::Glory>(this)->button1.i8_8) = 1;
            joan::orig::as_button(&jo::self<jo::Glory>(this)->button1)->draw(1u, static_cast<std::uint8_t>(a));
            jo::view<std::uint8_t>(jo::self<jo::Glory>(this)->button2.i8_8) = 1;
            joan::orig::as_button(&jo::self<jo::Glory>(this)->button2)->draw(1u, static_cast<std::uint8_t>(a));
        } else {
            jo::view<std::uint8_t>(jo::self<jo::Glory>(this)->button0.i8_8) = 0;
            jo::view<std::uint8_t>(jo::self<jo::Glory>(this)->button1.i8_8) = 0;
            jo::view<std::uint8_t>(jo::self<jo::Glory>(this)->button2.i8_8) = 0;
        }
    }
};

#define U6(name, addr, fn) JOAN_REPLACE("game/ui6", name, addr, fn)
U6("Glory::draw", 0x006e6dc0, &H::glory_draw);
JOAN_CANVAS_SELF(0x006e6dc0);  // ScreenScope in the body (no thunk wrapper)
#undef U6

}  // namespace game_ui6
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

struct H {
    // ------------------------------------------------------------------ 0x006e7880
    bool click_6e7880() {
        if (jo::view<i8>(jo::self<jo::Glory>(this)->done) == 0) return false;
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::Glory>(this)->button3)->mouse(i32{0}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
            joan::orig::as_menu(obj(d::g_objective_menu))->open_7226a0(i32{2});  // 0x007226a0
            joan::orig::as_glory_screen(this)->close();                      // 0x006e7b20
        }
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Popup::click_6e7880", 0x006e7880, &H::click_6e7880);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {

struct H {
    void reset_6e7b20() {
        jo::self<jo::Glory>(this)->done = 0;
        joan::orig::as_panel(this)->release_textures2();  // 0x006e7c60
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Obj6e7b20::reset", 0x006e7b20, &H::reset_6e7b20);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {

struct H {
    // 0x006e7b50
    std::uint8_t get_d32() { return jo::self<jo::Glory>(this)->done; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Obj::get_d32", 0x006e7b50, &H::get_d32);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

void* const kSprites = obj(d::g_texture_array);

JOAN_COUNTER(c_load_vict, "Menu::load_vict");

i32 sprite_get_flag() { return joan::orig::arg::as<i32>(joan::orig::as_texture_manager(kSprites)->mode()); }                  // 0x0060a720
void sprite_set_flag(i32 v) { joan::orig::as_texture_manager(kSprites)->set_mode_60a740(joan::orig::arg::u32(v)); }          // 0x0060a740
jo::Texture* sprite_load(img::Addr name) { return static_cast<jo::Texture*>(joan::orig::as_texture_manager(kSprites)->load(joan::orig::arg::str(str(name)), 1, 0u)); }  // 0x006099c0

struct H {
    // ------------------------------------------------------------------ 0x006e7b80
    void* load_vict() {
        c_load_vict.hit();
        const i32 saved = sprite_get_flag();
        sprite_set_flag(1);
        if (jo::self<jo::Glory>(this)->u32_d18 == nullptr) jo::self<jo::Glory>(this)->u32_d18 = sprite_load(d::s_VICT);  // 'VICT'
        if (jo::self<jo::Glory>(this)->u32_d1c == nullptr) jo::self<jo::Glory>(this)->u32_d1c = sprite_load(d::s_COMMELE);  // 'COMMELE'
        sprite_set_flag(saved);
        jo::self<jo::Glory>(this)->blink_on = 0;
        jo::self<jo::Glory>(this)->blink_state = 0;
        jo::view<i32>(jo::self<jo::Glory>(this)->blink_time) = 0;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Menu::load_vict", 0x006e7b80, &H::load_vict);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {

void* const kTextures = obj(d::g_texture_array);

void release_texture(void* t) { joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(t)); }         // 0x0060a550

struct H {
    // ------------------------------------------------------------------ 0x006e7c60
    void release_textures2() {
        if (jo::self<jo::Glory>(this)->u32_d18 != nullptr) {
            release_texture(jo::self<jo::Glory>(this)->u32_d18);
            jo::self<jo::Glory>(this)->u32_d18 = nullptr;
        }
        if (jo::self<jo::Glory>(this)->u32_d1c != nullptr) {
            release_texture(jo::self<jo::Glory>(this)->u32_d1c);
            jo::self<jo::Glory>(this)->u32_d1c = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("Panel::release_textures2", 0x006e7c60, &H::release_textures2);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::orig::helpers::op_new;
using joan::orig::helpers::op_delete;
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

struct H {
    // ------------------------------------------------------------------ 0x006e8010
    char* set_text_6e8010(const char* s) {
        if (jo::self<jo::Glory>(this)->text != nullptr) op_delete(jo::self<jo::Glory>(this)->text);
        jo::self<jo::Glory>(this)->text = static_cast<char*>(op_new(crt_strlen(s) + 1));
        return crt_strcpy(jo::self<jo::Glory>(this)->text, s);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::set_text_6e8010", 0x006e8010, &H::set_text_6e8010);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b109_06.cpp (game/b109_06)
namespace {
namespace game_b109_06 {

using joan::hook::x87::Fpu;

void* const kRandom = obj(d::g_misc);
void* const kScreen = obj(d::g_sprites_a46128);

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), ret 0x10; returns this.
u32 make_color(u32 r, u32 g, u32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::as_colour(p));
}
// 0x005e9640 (thunk 0x00407577): colour from packed ARGB, ret 4; returns this.
u32 packed_color(u32 argb) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_color(c)->Color_dword(joan::orig::arg::u32(argb)));
    return jo::view<u32>(*jo::as_colour(p));  // the 4 colour bytes
}
// 0x00633500 on 0xa46128 (17 args, ret 0x44).
void draw_sprite(i32 x, i32 y, u32 surf, img::Addr name, void* pal, u32 color, i32 a7, i32 a8, i32 a9, i32 a10) {
    joan::orig::as_sprite_queue(kScreen)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surf), joan::orig::arg::str(img::addr(name)), joan::orig::arg::mptr(pal), joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, -1, -1);
}
// 0x00626130 (text, 13 args, ret 0x34).
void draw_text(void* dest, i32 x, i32 y, i32 w, i32 h, u32 surf, const char* text, u32 color, i32 a8, i32 a9) {
    joan::orig::as_text(dest)->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(surf), text, joan::orig::arg::u32(color), i32{0}, joan::orig::arg::i32(a8), i32{2}, joan::orig::arg::i32(a9), joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}));
}

JOAN_COUNTER(c_mbut, "mbut_draw_6e80b0");

// 0x006e80b0
u32 JOAN_CDECL mbut_draw(void* ctl, u32 surf, u32 flags) {
    c_mbut.hit();
    if ((flags & 0xff) != 7 && ((flags & 0xff) & 1) != 0) return 0;
    const Fpu fpu{true};
    i16 s4 = jo::as_button(ctl)->x;
    i16 s8 = jo::as_button(ctl)->y;
    i16 sc = jo::as_button(ctl)->x2;
    i16 s10 = jo::as_button(ctl)->y2;
    float f14 = 1.0f, f18 = 1.0f;
    if (jo::as_button(ctl)->fit != 0) {
        joan::orig::as_sprite2d(kScreen)->fit_rect(joan::orig::arg::i16(jo::as_button(ctl)->x), joan::orig::arg::i16(jo::as_button(ctl)->y), joan::orig::arg::i16(jo::as_button(ctl)->x2), joan::orig::arg::i16(jo::as_button(ctl)->y2), &f14, &f18, &s4, &s8, &sc, &s10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    auto sprite = [&](i32 x, i32 y, img::Addr name) {
        const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
        draw_sprite(x, y, surf, name, global<void*>(d::g_a41194), color, 3, 0, 0, 0);
    };
    auto in_rect = [&]() {
        return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{s4}, i32{s8}, i32{sc}, i32{s10}));  // 0x00593980
    };
    auto scaled = [&](float s) -> i32 {  // 0x00590a20 on 0xa45fa8, short result
        if (jo::as_button(ctl)->pressed == 0) return 0;
        const double d = fpu.mul(static_cast<double>(global<float>(d::flt_1)), static_cast<double>(s));
        return static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_misc(kRandom)->round(joan::orig::arg::f64(d))));
    };
    auto label = [&]() {
        const i32 dy = scaled(f18);
        const i32 dx = scaled(f14);
        const u32 color = packed_color(0xff003366);
        draw_text(obj(d::g_font_a489c8), s4 + dx, s8 + dy, sc, s10, surf, static_cast<const char*>(jo::as_button(ctl)->caption), color, 0, 0);
    };
    if (jo::as_button(ctl)->pressed != 0) {
        sprite(s4, s8, d::s_MBUT_DW);  // "MBUT_DW"
    } else if (in_rect() != 0) {
        sprite(s4, s8, d::s_MBUT_OV);  // "MBUT_OV"
    } else {
        sprite(s4, s8, d::s_MBUT_OT);  // "MBUT_OT"
    }
    sprite(s4 - 4, s8 - 4, d::s_MBUT_CON);  // "MBUT_CON"
    if (jo::view<i8>(jo::as_button(ctl)->caption[0]) == 0) return 1;
    if (jo::as_button(ctl)->pressed == 0 && in_rect() != 0) {
        label();
        const i32 w = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_font(obj(d::g_text_a58ac0))->measure(joan::orig::arg::str(jo::as_button(ctl)->caption), -1, 0, 0, joan::orig::arg::f32(u32{0x3f800000}))));  // 0x00624f00
        const i32 left = (jo::as_button(ctl)->x2 - jo::as_button(ctl)->x - w) / 2 + jo::as_button(ctl)->x - 0x19;
        const i32 right = left + w + 0x19;
        sprite(static_cast<i16>(left), jo::as_button(ctl)->y + 1, d::s_MBUT_MOT);   // "MBUT_MOT"
        sprite(static_cast<i16>(right), jo::as_button(ctl)->y + 1, d::s_MBUT_MOT);  // "MBUT_MOT"
        return 1;
    }
    label();
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_06", name, addr, fn)
B("mbut_draw_6e80b0", 0x006e80b0, &mbut_draw);
#undef B

}  // namespace game_b109_06
}  // namespace

// ---------------------------------------- from modules/game_b101_15.cpp (game/b101_15)
namespace {
namespace game_b101_15 {

using joan::hook::global;

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
    // 0x006e7cf0 (ret)
    void blink_d28() {
        jo::Glory* const g = jo::self<jo::Glory>(this);
        blink_step(reinterpret_cast<i8&>(g->blink_on), reinterpret_cast<i8&>(g->blink_state), g->blink_time,
                   static_cast<i8>(g->done));
    }
};

#define B15(name, addr, fn) JOAN_REPLACE("game/b101_15", name, addr, fn)
B15("BlinkD28::step", 0x006e7cf0, &H::blink_d28);
#undef B15

}  // namespace game_b101_15
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Screen::shutdown_6e6d20() -> void { joan::bind::member<&::game_b92_06::H::screen_shutdown>(this); }  // 0x6e6d20
auto joan::orig::GloryScreen::draw(std::uint8_t a0) -> void { joan::bind::member<&::game_ui6::H::glory_draw>(this, a0); }  // 0x6e6dc0
auto joan::orig::Screen::win_6e7900() -> void { joan::bind::member<&::game_b102_15::H::win_6e7900>(this); }  // 0x6e7900
auto joan::orig::GloryScreen::close() -> void { joan::bind::member<&::game_b84_04::H::reset_6e7b20>(this); }  // 0x6e7b20
auto joan::orig::GloryScreen::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b90_06::H::get_d32>(this)); }  // 0x6e7b50
auto joan::orig::Menu::load_vict() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_08::H::load_vict>(this)); }  // 0x6e7b80
auto joan::orig::Panel::release_textures2() -> void { joan::bind::member<&::game_b85_05::H::release_textures2>(this); }  // 0x6e7c60
auto joan::orig::GloryScreen::set_text(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_10::H::set_text_6e8010>(this, a0)); }  // 0x6e8010
