// game/unknown_i_igmenu.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_igmenu.cpp;
// re/attribution.csv: between ww/interfac/i_work.cpp and ww/interfac/list.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::img::str;

using u16 = std::uint16_t;

using joan::orig::helpers::strcpy_;  // strcpy
char* strcat_(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }  // strcat

using i16 = std::int16_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using joan::orig::helpers::memset_;  // memset

using joan::hook::x87::ftol;


void* const kRes = obj(d::g_texture_array);
void* const kText = obj(d::g_strings);

using joan::orig::helpers::widget_ctor;  // 0x00620ee0
void text_ctor(void* p) { joan::orig::as_image2d(p)->construct(); }    // 0x00629400

// Colour record 0x0042a260(r, g, b, a) on a 4-byte local; returns its dword.
u32 colour(i32 r, i32 g, i32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(p));
}

void reset_timer_a500d2() {
    if (global<u8>(d::g_game_running) == 0) return;
    global<u8>(d::g_game_running) = 0;
    global<u16>(d::g_saved_game_speed) = static_cast<u16>(ftol(static_cast<double>(global<float>(d::g_game_speed))));
    joan::orig::as_game(obj(d::g_game_a50088))->set_speed(0u);  // 0x0042f930
}

JOAN_COUNTER(c_ctor_6f2c10, "Screen::ctor_6f2c10");
JOAN_COUNTER(c_captions, "Screen::captions_6f3330");
JOAN_COUNTER(c_open, "Screen::open_6f3730");

struct H {
    void load_res(jo::Texture*& slot, img::Addr name) {
        if (slot != nullptr) return;
        slot = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(kRes)->load(joan::orig::arg::str(str(name)), 1, 0u));  // 0x006099c0
    }

    // ------------------------------------------------------------------ 0x006f2c10
    void* ctor_6f2c10() {
        c_ctor_6f2c10.hit();
        widget_ctor(&jo::self<jo::IngameMenu>(this)->button0);
        widget_ctor(&jo::self<jo::IngameMenu>(this)->button1);
        widget_ctor(&jo::self<jo::IngameMenu>(this)->button2);
        widget_ctor(&jo::self<jo::IngameMenu>(this)->button3);
        widget_ctor(&jo::self<jo::IngameMenu>(this)->button4);
        widget_ctor(&jo::self<jo::IngameMenu>(this)->button5);
        widget_ctor(&jo::self<jo::IngameMenu>(this)->button6);
        widget_ctor(&jo::self<jo::IngameMenu>(this)->button7);
        widget_ctor(&jo::self<jo::IngameMenu>(this)->back_button);
        text_ctor(&jo::self<jo::IngameMenu>(this)->title);
        text_ctor(&jo::self<jo::IngameMenu>(this)->caption);
        jo::IngameMenu* const m = jo::self<jo::IngameMenu>(this);
        memset_(this, 0, static_cast<u32>(reinterpret_cast<u8*>(&m->button0) - reinterpret_cast<u8*>(m)));  // the fields before button0 (0x28 bytes on i686)
        return this;
    }

    // ------------------------------------------------------------------ 0x006f3330
    i32 caption(i32 id, std::uintptr_t getter, jo::Button& field) {
        const u32 white = colour(0xff, 0xff, 0xff, 0xff);
        const char* const s = joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(global<u32>(d::g_cfg_control_scheme)), joan::orig::arg::i32(id));  // 0x004cb5b0 (the key name)
        const char* const t = thiscall<const char*>(getter, kText, s);
        return joan::orig::arg::as<i32>(joan::orig::as_button(&field)->set_text(t, img::obj(d::g_font_a489c8), joan::orig::arg::i32(white), 1, 2, 0, 2, 2));  // 0x00623ce0
    }
    i32 captions_6f3330() {
        c_captions.hit();
        const u32 state = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kRes)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kRes)->set_mode(1u);                 // 0x0060a740
        load_res(jo::self<jo::IngameMenu>(this)->texture[3], d::s_COMMELE2);   // "COMMELE2"
        load_res(jo::self<jo::IngameMenu>(this)->texture[4], d::s_COMMELE3);  // "COMMELE3"
        joan::orig::as_texture_manager(kRes)->set_mode(joan::orig::arg::u32(state));  // 0x0060a740
        caption(0x26, fn::code_value(fn::IGameMnu_open_1), jo::self<jo::IngameMenu>(this)->button0);           // 0x004dabd0
        caption(0x25, fn::code_value(fn::TxtRes_igamemnu_2), jo::self<jo::IngameMenu>(this)->button1);          // 0x004dac10
        caption(0x28, fn::code_value(fn::IGameMnu_open_3), jo::self<jo::IngameMenu>(this)->button2);          // 0x004dac50
        caption(0x29, fn::code_value(fn::IGameMnu_open_13), jo::self<jo::IngameMenu>(this)->button3);          // 0x004dae50
        return caption(0x27, fn::code_value(fn::Menu_igamemnu_5), jo::self<jo::IngameMenu>(this)->button5);  // 0x004dacc0
    }

    // ------------------------------------------------------------------ 0x006f3730
    void* open_6f3730() {
        c_open.hit();
        jo::view<u8>(jo::self<jo::IngameMenu>(this)->open) = 1;
        reset_timer_a500d2();
        joan::orig::as_widget_child(&jo::self<jo::IngameMenu>(this)->button4.child)->enable();  // 0x006ca000
        if (global<u32>(d::g_mission) == 6) {
            char buf[0x10c];
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kText)->autosave_text_1(joan::orig::arg::u32(global<u32>(d::g_mission))));  // 0x004dd0e0
            strcpy_(buf, name);
            strcat_(buf, str(d::s_SAV_9cfb78));  // ".SAV"
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_paths(obj(d::g_saves))->find(joan::orig::arg::str(static_cast<const char*>(buf))));  // 0x00477640
            if (p == nullptr || jo::as_game_file(p)->u32_19a != 6 || static_cast<i32>(jo::view<i8>(jo::as_game_file(p)->u8_19e)) != 1)
                joan::orig::as_widget_child(&jo::self<jo::IngameMenu>(this)->button4.child)->disable();  // 0x006ca020
        }
        const char* const t = joan::orig::as_txt_res(kText)->igamemnu_text_15();         // 0x004daec0 (IGAMEMNU 15)
        joan::orig::as_ingame_frame(obj(d::g_hud))->set_title(t);          // 0x006f2ac0
        joan::orig::as_ingame_frame(obj(d::g_hud))->slide_in();             // 0x006f24d0
        jo::view<u8>(jo::self<jo::IngameMenu>(this)->u8_26) = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen::ctor_6f2c10", 0x006f2c10, &H::ctor_6f2c10);
B("Screen::captions_6f3330", 0x006f3330, &H::captions_6f3330);
B("Screen::open_6f3730", 0x006f3730, &H::open_6f3730);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b104_14.cpp (game/b104_14)
namespace {
namespace game_b104_14 {

constexpr u32 kM1 = 0xffffffffu;


// Rgba(r, g, b, a) 0x0042a260 / Rgba(dword) 0x005e9640 into a local, value returned
u32 colour(u32 r, u32 g, u32 b, u32 a) {
    u32 storage = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&storage)->construct(r, g, b, a));  // 0x0042a260
}
// bitmap item 0x00629660 (19 args, ret 0x4c), common tail 3,0,2,0,-1,-1,-1,-1,0,-1,-1,0,1,1
u32 bitmap(void* item, u32 x, u32 y, void* font, img::Addr name, u32 col) {
    return joan::orig::arg::as<u32>(joan::orig::as_label(item)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::mptr(font), joan::orig::arg::str(img::addr(name)), joan::orig::arg::i32(col), 3, 0, 2, 0, joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), 0, joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), 0, 1, 1));  // 0x00629660
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

JOAN_COUNTER(c_load, "LoadScreen::init_6f2d70");

struct H {
    // ------------------------------------------------------------------ 0x006f2d70
    u32 init_6f2d70() {
        c_load.hit();
        jo::self<jo::IngameMenu>(this)->font_14 = obj(d::g_bmp_res_igamemnu);
        jo::self<jo::IngameMenu>(this)->texture[0] = nullptr;
        jo::self<jo::IngameMenu>(this)->font_18 = obj(d::g_bmp_res_optmenuf);
        jo::self<jo::IngameMenu>(this)->texture[1] = nullptr;
        jo::self<jo::IngameMenu>(this)->font_1c = obj(d::g_bmp_res_optmenub);
        jo::self<jo::IngameMenu>(this)->texture[2] = nullptr;
        jo::self<jo::IngameMenu>(this)->font_20 = obj(d::g_bmp_commele2);
        constexpr fn::Code kCb = fn::Menu_draw_check_6f4c40;  // 0x006f4c40
        auto b = [&](jo::Button& item, u32 y0, u32 y1) { button(&item, 0x100u, y0, 0x2ffu, y1, fn::code_value(kCb), 1, 0, 0, 1, 1, 1); };
        b(jo::self<jo::IngameMenu>(this)->button0, 0xfffffe1au, 0xfffffe59u);
        b(jo::self<jo::IngameMenu>(this)->button1, 0xfffffe5eu, 0xfffffe9du);
        b(jo::self<jo::IngameMenu>(this)->button2, 0xfffffea2u, 0xfffffee1u);
        b(jo::self<jo::IngameMenu>(this)->button3, 0xfffffee6u, 0xffffff25u);
        b(jo::self<jo::IngameMenu>(this)->button4, 0xffffff2au, 0xffffff69u);
        caption(&jo::self<jo::IngameMenu>(this)->button4, fn::code_value(fn::Menu_igamemnu_04), d::g_font_a489c8);  // 0x004dac90
        b(jo::self<jo::IngameMenu>(this)->button5, 0xffffff6eu, 0xffffffadu);
        b(jo::self<jo::IngameMenu>(this)->button6, 0xffffffb2u, 0xfffffff1u);
        caption(&jo::self<jo::IngameMenu>(this)->button6, fn::code_value(fn::IGameMnu_open_6), d::g_font_a489c8);  // 0x004dad00
        b(jo::self<jo::IngameMenu>(this)->button7, 0xfffffff6u, 0x35u);
        caption(&jo::self<jo::IngameMenu>(this)->button7, fn::code_value(fn::IGameMnu_open_7), d::g_font_a489c8);  // 0x004dad30
        {
            const u32 c = colour(0, 0, 0, 0xff);
            bitmap(&jo::self<jo::IngameMenu>(this)->title, 0x124u, 0xfffffe16u, jo::self<jo::IngameMenu>(this)->font_20, d::s_FRAME, c);  // 'FRAME'
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            bitmap(&jo::self<jo::IngameMenu>(this)->caption, 0x400u, 0x2c8u, jo::self<jo::IngameMenu>(this)->font_20, d::s_LONGBAR, c);  // 'LONGBAR'
        }
        button(&jo::self<jo::IngameMenu>(this)->back_button, 0x1ceu, 0x2c8u, 0x231u, 0x2e6u, fn::code_value(fn::draw_mark_label), 1, 1, 0, 1, 1, 1);  // cb 0x006f5200
        return caption(&jo::self<jo::IngameMenu>(this)->back_button, fn::code_value(fn::IGameMnu_open_14), d::g_font_goudy14_22_white);  // 0x004dae90
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_14", name, addr, fn)
B("LoadScreen::init_6f2d70", 0x006f2d70, &H::init_6f2d70);
#undef B

}  // namespace game_b104_14
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {


struct H {
    void reset_6f3270() {  // 0x006f3270
        joan::orig::as_ingame_menu(this)->free_sprites();  // 0x006f3630
        joan::orig::as_menu_button(&jo::self<jo::IngameMenu>(this)->button0)->clear();  // 0x006217e0
        joan::orig::as_menu_button(&jo::self<jo::IngameMenu>(this)->button1)->clear();
        joan::orig::as_menu_button(&jo::self<jo::IngameMenu>(this)->button2)->clear();
        joan::orig::as_menu_button(&jo::self<jo::IngameMenu>(this)->button3)->clear();
        joan::orig::as_menu_button(&jo::self<jo::IngameMenu>(this)->button4)->clear();
        joan::orig::as_menu_button(&jo::self<jo::IngameMenu>(this)->button5)->clear();
        joan::orig::as_menu_button(&jo::self<jo::IngameMenu>(this)->button6)->clear();
        joan::orig::as_menu_button(&jo::self<jo::IngameMenu>(this)->button7)->clear();
        joan::orig::as_image2d(&jo::self<jo::IngameMenu>(this)->title)->release();  // 0x00629830
    }

    i32 get_1238() { return jo::view<i32>(jo::self<jo::Player>(this)->selected_commander_id); }  // 0x006f5ed0 (this: the Player 0x00a5b9e8)
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6f::reset_b", 0x006f3270, &H::reset_6f3270);
B93("Obj6f::get_1238", 0x006f5ed0, &H::get_1238);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

void* const kImages = obj(d::g_texture_array);


void image_release(void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                    // 0x0060a550

struct H {

    static void release_at(jo::Texture*& texture) {
        if (texture != nullptr) {
            image_release(texture);
            texture = nullptr;
        }
    }

    // 0x006f3630
    void release5() {
        for (std::int32_t i = 0; i < 5; ++i) release_at(jo::self<jo::IngameMenu>(this)->texture[i]);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Images5::release", 0x006f3630, &H::release5);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {


JOAN_COUNTER(c_layout_buttons, "X::layout_buttons");

void anim_a(void* p, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    joan::orig::as_widget(p)->move_to_61e020(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(f));  // 0x0061e020
}
void anim_b(void* p, u32 a, u32 b, u32 c, u32 d, u32 e) {
    joan::orig::as_widget(p)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
}

struct H {
    // ------------------------------------------------------------------------ 0x006f38a0
    u32 layout_buttons() {
        c_layout_buttons.hit();
        joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();  // 0x0042fef0
        jo::view<u8>(jo::self<jo::IngameMenu>(this)->u8_27) = 1;
        const u32 fa = 0x455ac000, fb = 0xc4bb8000;  // 3500.0f, -1500.0f
        anim_a(&jo::self<jo::IngameMenu>(this)->button0, 0x100, 0xfffffe1a, fa, fb, 0, 0);
        anim_a(&jo::self<jo::IngameMenu>(this)->button1, 0x100, 0xfffffe5e, fa, fb, 0, 0x2c);
        anim_a(&jo::self<jo::IngameMenu>(this)->button2, 0x100, 0xfffffea2, fa, fb, 0, 0x58);
        anim_a(&jo::self<jo::IngameMenu>(this)->button3, 0x100, 0xfffffee6, fa, fb, 0, 0x84);
        anim_a(&jo::self<jo::IngameMenu>(this)->button4, 0x100, 0xffffff2a, fa, fb, 0, 0xb0);
        anim_a(&jo::self<jo::IngameMenu>(this)->button5, 0x100, 0xffffff6e, fa, fb, 0, 0xdc);
        anim_a(&jo::self<jo::IngameMenu>(this)->button6, 0x100, 0xffffffb2, fa, fb, 0, 0x108);
        anim_a(&jo::self<jo::IngameMenu>(this)->button7, 0x100, 0xfffffff6, fa, fb, 0, 0x134);
        anim_a(&jo::self<jo::IngameMenu>(this)->title, 0x124, 0xfffffe16, fa, fb, 0, 0x160);
        anim_a(&jo::self<jo::IngameMenu>(this)->caption, 0x400, 0x2c8, fa, fb, 0, 0x96);
        anim_b(&jo::self<jo::IngameMenu>(this)->back_button, 0, 0x40c00000, 0, 0, 0);
        return joan::orig::arg::as<u32>(joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_BUTTON_SLIDE_MANY)));  // 0x004ceef0 "BUTTON_SLIDE_MANY"
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::layout_buttons", 0x006f38a0, &H::layout_buttons);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b116_07.cpp (game/b116_07)
namespace {
namespace game_b116_07 {

i32 gsb(img::Addr a) { return static_cast<i32>(global<i8>(a)); }

constexpr img::Addr kMouse = d::g_mouse;

JOAN_COUNTER(c_menu, "IngameMenu::draw_6f3af0");

struct H {
    // 0x006f3af0 (the argument is not used)
    void menu_draw(u32 /*unused*/) {
        joan::canvas::ScreenScope canvas_scope(0x006f3af0);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_menu.hit();
        if (static_cast<i32>(jo::self<jo::IngameMenu>(this)->open) == 0) return;
        if (gsb(d::g_menu_frame_state) == 2 && static_cast<i32>(jo::self<jo::IngameMenu>(this)->u8_26) == 0 && static_cast<i32>(jo::self<jo::IngameMenu>(this)->u8_27) == 0) {
            joan::orig::as_cursor(obj(kMouse))->reset_pos();  // 0x00592c30
        }
        joan::orig::as_menu(obj(d::g_hud))->reset_fades(static_cast<std::int8_t>(joan::orig::arg::i32(u32{0})));  // 0x006f21c0
        if (gsb(d::g_menu_frame_state) != 2) return;
        if (static_cast<i32>(jo::self<jo::IngameMenu>(this)->dialog) != 0) {
            joan::orig::as_dialog(obj(d::g_popup_a52df8))->init();  // 0x006d5080
            return;
        }
        joan::orig::as_label(&jo::self<jo::IngameMenu>(this)->title)->draw(5u, 7u);  // 0x00629bd0
        jo::Button jo::IngameMenu::* const buttons[8] = {&jo::IngameMenu::button0, &jo::IngameMenu::button1, &jo::IngameMenu::button2, &jo::IngameMenu::button3, &jo::IngameMenu::button4, &jo::IngameMenu::button5, &jo::IngameMenu::button6, &jo::IngameMenu::button7};
        for (jo::Button jo::IngameMenu::* const button : buttons) {
            joan::orig::as_button(&(jo::self<jo::IngameMenu>(this)->*button))->draw(5u, 7);  // 0x00622cc0
        }
        joan::orig::as_label(&jo::self<jo::IngameMenu>(this)->caption)->draw(5u, 7u);
        joan::orig::as_button(&jo::self<jo::IngameMenu>(this)->back_button)->draw(5u, 7);
    }
};

JOAN_REPLACE("game/b116_07", "IngameMenu::draw", 0x006f3af0, &H::menu_draw);
JOAN_CANVAS_SELF(0x006f3af0);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_07
}  // namespace

// ---------------------------------------- from modules/game_b109_20.cpp (game/b109_20)
namespace {
namespace game_b109_20 {


JOAN_COUNTER(c_save_update, "SaveMenu::update_6f3c90");

struct H {
    // ---------------------------------------------------------------------------- 0x006f3c90
    static i32 button_hit(jo::Button& b, i32 a) {
        return joan::orig::arg::as<i32>(joan::orig::as_ui_button(&b)->mouse(joan::orig::arg::i32(a), i32{0}, i32{-1}, i32{0}, i32{0}, i32{0}));  // 0x006218c0
    }
    bool save_dialog(i8 mode, std::uintptr_t dir_getter) {
        void* const box = obj(d::g_popup_a52df8);
        jo::self<jo::IngameMenu>(this)->dialog = mode;
        char buf[0x410];
        strcpy_(buf, thiscall<const char*>(dir_getter, obj(d::g_strings)));
        strcat_(buf, str(d::kNl));
        strcat_(buf, joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->igamemnu_11()));  // 0x004dadf0
        joan::orig::as_popup(box)->set_title(joan::orig::arg::str(i32{0}), nullptr);                     // 0x006d52c0
        joan::orig::as_popup(box)->set_text_6d5230(joan::orig::arg::str(static_cast<const char*>(buf)), nullptr);  // 0x006d5230
        joan::orig::as_popup(box)->set_buttons(joan::orig::arg::u32(i32{3}));                             // 0x006d5340
        joan::orig::as_popup(box)->load_gpopup();                                     // 0x006d3be0
        return true;
    }
    bool set_page(i32 page) {
        global<i32>(d::g_screen_pending) = page;
        joan::orig::as_ingame_menu(this)->layout_buttons();  // 0x006f38a0
        return true;
    }
    bool save_update() {
        c_save_update.hit();
        void* const box = obj(d::g_popup_a52df8);
        if (jo::self<jo::IngameMenu>(this)->open == 0) return false;
        if (jo::self<jo::IngameMenu>(this)->u8_26 != 0 || jo::self<jo::IngameMenu>(this)->u8_27 != 0 || global<i8>(d::g_menu_frame_state) != 2) return true;
        if (jo::self<jo::IngameMenu>(this)->dialog == 0) {
            if (button_hit(jo::self<jo::IngameMenu>(this)->back_button, 0x1b) != 0 || joan::orig::arg::as<i32>(joan::orig::as_mode(obj(d::g_screen_mgr))->check_6f5a70()) != 0) {  // 0x006f5a70
                jo::view<u8>(jo::self<jo::IngameMenu>(this)->back_button.pressed) = 0;
                joan::orig::as_ingame_menu(this)->layout_buttons();  // 0x006f38a0
                return true;
            }
            if (button_hit(jo::self<jo::IngameMenu>(this)->button0, 0) != 0) return set_page(4);
            if (button_hit(jo::self<jo::IngameMenu>(this)->button1, 0) != 0) return set_page(3);
            if (button_hit(jo::self<jo::IngameMenu>(this)->button2, 0) != 0) return set_page(6);
            if (button_hit(jo::self<jo::IngameMenu>(this)->button3, 0) != 0) return set_page(7);
            if (button_hit(jo::self<jo::IngameMenu>(this)->button4, 0) != 0) return save_dialog(3, fn::code_value(fn::Menu_igamemnu_0c));  // 0x004dae20
            if (button_hit(jo::self<jo::IngameMenu>(this)->button5, 0) != 0) return set_page(5);
            if (button_hit(jo::self<jo::IngameMenu>(this)->button6, 0) != 0) return save_dialog(2, fn::code_value(fn::IGameMnu_open_10));  // 0x004dadc0
            if (button_hit(jo::self<jo::IngameMenu>(this)->button7, 0) != 0) return save_dialog(1, fn::code_value(fn::IGameMnu_open_10));  // 0x004dadc0
            return true;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_popup(box)->poll_buttons()) == 0) return true;  // 0x006d5120
        if (joan::orig::arg::as<i32>(joan::orig::as_popup(box)->result()) == 1) {             // 0x006d5370
            if (jo::self<jo::IngameMenu>(this)->dialog == 1 && joan::orig::arg::as<i8>(joan::orig::as_quit_popup(obj(d::g_quit_popup))->flag_be()) == 0) {  // 0x006e4d30
                joan::orig::as_quit_popup(obj(d::g_quit_popup))->show(joan::orig::arg::u32(i32{1}));  // 0x006e4c00
                global<u8>(d::g_game_running) = 0;
                joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::u32(i32{0}));  // 0x0042f930
                joan::orig::as_msg_log(obj(d::g_portraits))->reset();          // 0x004b31c0
                joan::orig::as_sms(obj(d::g_sms))->clear();          // 0x006e5e00
            } else if (jo::self<jo::IngameMenu>(this)->dialog == 2) {
                joan::orig::as_quit_popup(obj(d::g_quit_popup))->show(joan::orig::arg::u32(i32{0}));  // 0x006e4c00
                jo::self<jo::IngameMenu>(this)->dialog = 0;
                joan::orig::as_ingame_menu(this)->layout_buttons();                     // 0x006f38a0
                joan::orig::as_panel(this)->layout();                     // 0x006f4a10
                joan::orig::as_hud(obj(d::g_hud))->init_sliders();          // 0x006f28c0
                global<u8>(d::g_game_running) = 0;
                joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::u32(i32{0}));  // 0x0042f930
                joan::orig::as_msg_log(obj(d::g_portraits))->reset();          // 0x004b31c0
                joan::orig::as_sms(obj(d::g_sms))->clear();          // 0x006e5e00
            } else if (jo::self<jo::IngameMenu>(this)->dialog == 3 && global<u32>(d::g_mission) != 0) {
                joan::orig::as_msg_log(obj(d::g_portraits))->remove_all_of(joan::orig::arg::i32(global<u32>(d::g_mission)));  // 0x004b31f0
                jo::self<jo::IngameMenu>(this)->dialog = 0;
                joan::orig::as_ingame_menu(this)->layout_buttons();                     // 0x006f38a0
                joan::orig::as_panel(this)->layout();                     // 0x006f4a10
                joan::orig::as_hud(obj(d::g_hud))->init_sliders();          // 0x006f28c0
                global<u8>(d::g_game_running) = 0;
                joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::u32(i32{0}));  // 0x0042f930
                joan::orig::as_sms(obj(d::g_sms))->clear();          // 0x006e5e00
                joan::orig::as_battle(obj(d::g_scenarios))->load_scenario_41cc80(joan::orig::arg::i32(global<u32>(d::g_mission)), joan::orig::arg::i32(global<u32>(d::g_stage_index)), i32{0});                                // 0x0041cc80
                joan::orig::as_game(obj(d::g_game_a50088))->set_state(joan::orig::arg::u32(i32{0xc}));  // 0x004d7e70
                if (global<i32>(d::g_mission) == 6) {
                    char buf[0x410];
                    strcpy_(buf, joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->autosave_text_1(joan::orig::arg::u32(global<u32>(d::g_mission)))));  // 0x004dd0e0
                    strcat_(buf, str(d::s_SAV_9cfb78));                                  // ".SAV"
                    void* const r = joan::orig::arg::as<void*>(joan::orig::as_paths(obj(d::g_saves))->find(joan::orig::arg::str(static_cast<const char*>(buf))));  // 0x00477640
                    joan::orig::as_progress(obj(d::g_progress))->set_image(joan::orig::object<joan::orig::GameFile>(r));                  // 0x007563f0
                }
            }
        } else if (joan::orig::arg::as<i32>(joan::orig::as_popup(box)->result()) == 2) {  // 0x006d5370
            jo::self<jo::IngameMenu>(this)->dialog = 0;
        }
        joan::orig::as_popup(box)->free_gpopup();  // 0x006d3c60
        return true;
    }
};

#define B109(name, addr, fn) JOAN_REPLACE("game/b109_20", name, addr, fn)
B109("SaveMenu::update_6f3c90", 0x006f3c90, &H::save_update);
#undef B109

}  // namespace game_b109_20
}  // namespace

// ---------------------------------------- from modules/game_b109_16.cpp (game/b109_16)
namespace {
namespace game_b109_16 {

using joan::orig::helpers::fbits;

JOAN_COUNTER(c_panel, "Panel::update_6f43e0");

struct H {
    // ---- 0x004e5fc0 ---------------------------------------------------------------------------
    static void call0(std::uintptr_t thunk, img::Addr self) { thiscall<u32>(thunk, obj(self)); }

    // ---- 0x006f43e0 ---------------------------------------------------------------------------
    void panel_refresh_all() {
        static jo::Button jo::IngameMenu::* const kButtons[8] = {&jo::IngameMenu::button0, &jo::IngameMenu::button1, &jo::IngameMenu::button2, &jo::IngameMenu::button3, &jo::IngameMenu::button4, &jo::IngameMenu::button5, &jo::IngameMenu::button6, &jo::IngameMenu::button7};
        for (i32 i = 0; i < 8; ++i) joan::orig::as_widget(&(jo::self<jo::IngameMenu>(this)->*kButtons[i]))->update();  // 0x0061f3b0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->title)->update();
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->caption)->update();
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->back_button)->update();
    }
    template <class W>
    static void panel_place(W& widget, i32 a, i32 b, i32 c) {
        joan::orig::as_widget(&widget)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(fbits(0x455ac000)), joan::orig::arg::f32(fbits(0xc4bb8000)), 0, joan::orig::arg::u32(c));  // 0x0061e020
    }
    void panel_update() {  // 0x006f43e0
        if (jo::self<jo::IngameMenu>(this)->open == 0) return;
        c_panel.hit();
        if (joan::orig::arg::as<u8>(joan::orig::as_hud(obj(d::g_hud))->is_open()) == 0) {  // 0x004303f0
            jo::view<u8>(jo::self<jo::IngameMenu>(this)->open) = 0;
            joan::orig::as_ingame_frame(obj(d::g_hud))->set_title(joan::orig::arg::str(obj(d::kEmpty)));  // 0x006f2ac0
            if (global<u8>(d::g_game_running) == 0) {
                global<u8>(d::g_game_running) = 1;
                joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)))));  // 0x0042f930
            }
            if (global<u32>(d::g_screen_pending) != 0)
                call0(fn::code_value(fn::Menu_set_state_6f5cc0), d::g_screen_mgr);  // 0x006f5cc0
            else
                call0(fn::code_value(fn::Triple_clear), d::g_screen_mgr);  // 0x006f5e90
            return;
        }
        if (global<i8>(d::g_menu_frame_state) != 2) return;
        if (jo::self<jo::IngameMenu>(this)->u8_27 != 0) {
            panel_refresh_all();
        } else if (jo::self<jo::IngameMenu>(this)->u8_26 != 0) {
            if (jo::self<jo::IngameMenu>(this)->u8_26 == 1) {
                joan::orig::as_screen(this)->captions_6f3330();  // 0x006f3330
                panel_place(jo::self<jo::IngameMenu>(this)->title, 0x124, 0x6e, 0);
                panel_place(jo::self<jo::IngameMenu>(this)->button7, 0x100, 0x24e, 0x2c);
                panel_place(jo::self<jo::IngameMenu>(this)->button6, 0x100, 0x20a, 0x58);
                panel_place(jo::self<jo::IngameMenu>(this)->button5, 0x100, 0x1c6, 0x84);
                panel_place(jo::self<jo::IngameMenu>(this)->button4, 0x100, 0x182, 0xb0);
                panel_place(jo::self<jo::IngameMenu>(this)->button3, 0x100, 0x13e, 0xdc);
                panel_place(jo::self<jo::IngameMenu>(this)->button2, 0x100, 0xfa, 0x108);
                panel_place(jo::self<jo::IngameMenu>(this)->button1, 0x100, 0xb6, 0x134);
                panel_place(jo::self<jo::IngameMenu>(this)->button0, 0x100, 0x72, 0x160);
                panel_place(jo::self<jo::IngameMenu>(this)->caption, 0, 0x2c8, 0);
                joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->back_button)->scale_to(1, joan::orig::arg::f32(fbits(0x40c00000)), 0.0f, 0, 0xc8u);  // 0x0061f1b0
                joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_BUTTON_SLIDE_MANY));  // 0x004ceef0 "BUTTON_SLIDE_MANY"
                jo::view<u8>(jo::self<jo::IngameMenu>(this)->u8_26) = 2;
            }
            panel_refresh_all();
        }
        if (jo::view<i8>(jo::self<jo::IngameMenu>(this)->button0.moving) != 0 || jo::view<i8>(jo::self<jo::IngameMenu>(this)->button1.moving) != 0 || jo::view<i8>(jo::self<jo::IngameMenu>(this)->button2.moving) != 0 || jo::view<i8>(jo::self<jo::IngameMenu>(this)->button3.moving) != 0 ||
            jo::view<i8>(jo::self<jo::IngameMenu>(this)->button4.moving) != 0 || jo::view<i8>(jo::self<jo::IngameMenu>(this)->button5.moving) != 0 || jo::view<i8>(jo::self<jo::IngameMenu>(this)->button6.moving) != 0 || jo::view<i8>(jo::self<jo::IngameMenu>(this)->button7.moving) != 0 ||
            jo::view<i8>(jo::self<jo::IngameMenu>(this)->title.moving) != 0 || jo::view<i8>(jo::self<jo::IngameMenu>(this)->caption.moving) != 0 || jo::view<i8>(jo::self<jo::IngameMenu>(this)->back_button.scaling) != 0)
            return;
        if (jo::self<jo::IngameMenu>(this)->u8_26 != 0) {
            jo::view<u8>(jo::self<jo::IngameMenu>(this)->u8_26) = 0;
            return;
        }
        if (jo::self<jo::IngameMenu>(this)->u8_27 == 0) return;
        jo::view<u8>(jo::self<jo::IngameMenu>(this)->u8_27) = 0;
        joan::orig::as_ingame_menu(this)->free_sprites();  // 0x006f3630
        if (global<u32>(d::g_screen_pending) == 0 || global<u32>(d::g_screen_pending) == 2) {
            call0(fn::code_value(fn::Menu_slide_out), d::g_hud);  // 0x006f26e0
        } else if (global<u32>(d::g_screen_pending) != 0) {
            call0(fn::code_value(fn::Menu_set_state_6f5cc0), d::g_screen_mgr);  // 0x006f5cc0
            jo::view<u8>(jo::self<jo::IngameMenu>(this)->open) = 0;
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_16", name, addr, fn)
B("Panel::update_6f43e0", 0x006f43e0, &H::panel_update);
#undef B

}  // namespace game_b109_16
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {


constexpr u32 kSix = 0x40c00000u;    // 6.0f
constexpr u32 kHi = 0x455ac000u;     // 3500.0f
constexpr u32 kLo = 0xc4bb8000u;     // -1500.0f

JOAN_COUNTER(c_panel_layout, "Panel::layout");

// 0x0061e020 Widget::place(x, y, a, b, c, d)
void place(void* w, i32 x, i32 y, u32 a, u32 b, i32 c, i32 d) {
    joan::orig::as_widget(w)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(a), joan::orig::arg::f32(b), static_cast<char>(c), joan::orig::arg::u32(d));
}
// 0x0061f1b0 Widget::set_anim(a, b, c, d, e)
i32 anim(void* w, i32 a, u32 b, i32 c, i32 d, i32 e) {
    joan::orig::as_widget(w)->scale_to_61f1b0(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
    return 0;  // 0x0061f1b0 returns nothing; Panel::layout's callers ignore the result
}

struct H {
    // ------------------------------------------------------------------ 0x006f4a10
    i32 panel_layout() {
        c_panel_layout.hit();
        jo::view<u8>(jo::self<jo::IngameMenu>(this)->open) = 0;
        jo::view<u8>(jo::self<jo::IngameMenu>(this)->u8_27) = 0;
        jo::view<u8>(jo::self<jo::IngameMenu>(this)->u8_26) = 0;
        place(&jo::self<jo::IngameMenu>(this)->button0, 0x100, -0x1e6, kHi, kLo, 1, 0);
        place(&jo::self<jo::IngameMenu>(this)->button1, 0x100, -0x1a2, kHi, kLo, 1, 0);
        place(&jo::self<jo::IngameMenu>(this)->button2, 0x100, -0x15e, kHi, kLo, 1, 0);
        place(&jo::self<jo::IngameMenu>(this)->button3, 0x100, -0x11a, kHi, kLo, 1, 0);
        place(&jo::self<jo::IngameMenu>(this)->button4, 0x100, -0xd6, kHi, kLo, 1, 0);
        place(&jo::self<jo::IngameMenu>(this)->button5, 0x100, -0x92, kHi, kLo, 1, 0);
        place(&jo::self<jo::IngameMenu>(this)->button6, 0x100, -0x4e, kHi, kLo, 1, 0);
        place(&jo::self<jo::IngameMenu>(this)->button7, 0x100, -0xa, kHi, kLo, 1, 0);
        place(&jo::self<jo::IngameMenu>(this)->title, 0x124, -0x1ea, kHi, kLo, 1, 0);
        place(&jo::self<jo::IngameMenu>(this)->caption, 0x400, 0x2c8, kHi, kLo, 1, 0);
        return anim(&jo::self<jo::IngameMenu>(this)->back_button, 0, kSix, 0, 1, 0);
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Panel::layout", 0x006f4a10, &H::panel_layout);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

i32 ftol(float v) { return joan::hook::x87::ftol(static_cast<double>(v)); }

void* const kRandom = obj(d::g_misc);
void* const kSprites = obj(d::g_sprites_a46128);

JOAN_COUNTER(c_check, "Menu::draw_check_6f4c40");

// ------------------------------------------------------------------ 0x006f4c40 (cdecl)
u32 color4(i32 r, i32 g, i32 b, i32 a) {
    u32 c = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
}

static u32 JOAN_CDECL draw_check(void* item, u32 a2, u32 flags) {
    c_check.hit();
    const u32 b = flags & 0xffu;
    if (b != 7 && (b & 1u) != 0) return 0;
    i16 s4 = jo::as_button(item)->x, s8 = jo::as_button(item)->y, sc = jo::as_button(item)->x2, s10 = jo::as_button(item)->y2;
    float f14 = 1.0f, f18 = 1.0f;
    if (jo::as_button(item)->fit != 0) {
        joan::orig::as_sprite2d(kSprites)->fit_rect(joan::orig::arg::i16(jo::as_button(item)->x), joan::orig::arg::i16(jo::as_button(item)->y), joan::orig::arg::i16(jo::as_button(item)->x2), joan::orig::arg::i16(jo::as_button(item)->y2), &f14, &f18, &s4, &s8, &sc, &s10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    auto label = [&](img::Addr name) {
        const i32 alpha = ftol(global<float>(d::flt_255) * jo::as_button(item)->scale);
        const u32 col = color4(0xff, 0xff, 0xff, alpha);
        joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(s4), joan::orig::arg::i16(s8), joan::orig::arg::u32(a2), joan::orig::arg::str(str(name)), joan::orig::arg::mptr(global<void*>(d::g_a59ab8)), joan::orig::arg::u32(col), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
    };
    auto inside = [&]() {
        return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(s4), static_cast<i32>(s8), static_cast<i32>(sc), static_cast<i32>(s10))) != 0;  // 0x00593980
    };
    auto quiet = []() { return global<i8>(d::g_a59abe) == 0 && global<i8>(d::g_a59abf) == 0; };
    if (jo::as_button(item)->pressed != 0 && quiet()) {
        label(d::s_CM_DW);  // "CM_DW"
    } else if (inside() && quiet()) {
        label(d::s_CM_OV);  // "CM_OV"
    } else {
        label(d::s_CM_OT);  // "CM_OT"
    }
    if (jo::view<i8>(jo::as_button(item)->caption[0]) != 0) {
        const bool lit = (jo::as_button(item)->pressed != 0 || inside()) && quiet();
        auto scaled = [&](float s) -> i32 {
            if (jo::as_button(item)->pressed == 0) return 0;
            const float v = global<float>(d::flt_1) * s;
            return joan::orig::arg::as<i16>(joan::orig::as_misc(kRandom)->round(joan::orig::arg::f64(static_cast<double>(v))));  // 0x00590a20
        };
        const i32 r48 = scaled(f18);
        const i32 r4c = scaled(f14);
        const u32 col = lit ? color4(0xff, 0xff, 0xff, 0xff) : color4(0, 0, 0, 0xff);
        joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_3(joan::orig::arg::i32(static_cast<i32>(s4) + r4c), joan::orig::arg::i32(static_cast<i32>(s8) + r48 - 2), joan::orig::arg::i32(sc), joan::orig::arg::i32(static_cast<i32>(s10) - 2), joan::orig::arg::u32(a2), joan::orig::arg::str(jo::as_button(item)->caption), joan::orig::arg::u32(col), i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);  // 0x00626130
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Menu::draw_check_6f4c40", 0x006f4c40, &draw_check);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b109_07.cpp (game/b109_07)
namespace {
namespace game_b109_07 {


JOAN_COUNTER(c_draw_mark, "draw_mark_label_6f5200");

void* const kText = obj(d::g_sprites_a46128);
void* const kRes = obj(d::g_misc);

// 0x0042a260 colour (r, g, b, a) into a 4-byte local, dword read back.
u32 colour(u8 r, u8 g, u8 b, u8 a) {
    alignas(4) u8 slot[4];
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(p));
}
// fld 255.0f; fmul [p+0x11]; _ftol
u8 alpha_of(const void* gadget) {
    const float prod = global<float>(d::flt_255) * jo::as_button(gadget)->scale;
    return static_cast<u8>(joan::hook::x87::ftol(static_cast<double>(prod)));
}
// 0x00633500 (text / image draw, 17 args) with the tail 3, 0, k, 0, -1, -1, -1, -1, 0, -1, -1.
void text_draw(i32 x, i32 y, u32 target, const char* name, img::Addr style, u32 color, u32 k = 2) {
    joan::orig::as_sprite_queue(kText)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(target), joan::orig::arg::str(name), img::obj(style), joan::orig::arg::u32(color), 3, 0, joan::orig::arg::i16(k), 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// 0x00626130 (text in a box, 13 args) with the tail 0, 1, 2, 0, 1.0f, 1.0f.
void box_text(void* font, i32 x, i32 y, i32 w, i32 h, u32 target, const void* text, u32 color) {
    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(target), joan::orig::arg::str(text), joan::orig::arg::u32(color), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x00626130
}

// ---------------------------------------------------------------------- 0x006f5200
i32 JOAN_CDECL draw_mark_label(u8* g, u32 target, u32 flags) {
    const u32 fl = flags & 0xff;
    if (fl != 7 && (fl & 1) != 0) return 0;
    c_draw_mark.hit();
    const joan::hook::x87::Fpu fpu{true};
    alignas(4) u32 w4 = jo::view<u16>(jo::as_button(g)->x);  // word locals in their own dword slots
    alignas(4) u32 w8 = jo::view<u16>(jo::as_button(g)->y);
    alignas(4) u32 wc = jo::view<u16>(jo::as_button(g)->x2);
    alignas(4) u32 w10 = jo::view<u16>(jo::as_button(g)->y2);
    alignas(4) float f14 = 1.0f;
    alignas(4) float f18 = 1.0f;
    if (jo::as_button(g)->fit != 0)
        joan::orig::as_sprite2d(kText)->fit_rect(joan::orig::arg::i16(jo::view<u16>(jo::as_button(g)->x)), joan::orig::arg::i16(jo::view<u16>(jo::as_button(g)->y)), joan::orig::arg::i16(jo::view<u16>(jo::as_button(g)->x2)), joan::orig::arg::i16(jo::view<u16>(jo::as_button(g)->y2)), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    void* const font = jo::as_button(g)->caption_box;
    if (jo::view<i8>(jo::as_button(g)->caption[0]) == 0) return 1;
    const char* const mark = str(d::s_MARK);  // "MARK"
    const img::Addr style = d::g_bmp_commele2;
    auto x4 = [&] { return static_cast<i32>(static_cast<i16>(w4)); };
    auto y8 = [&] { return static_cast<i32>(static_cast<i16>(w8)); };
    auto xc = [&] { return static_cast<i32>(static_cast<i16>(wc)); };
    auto offsets = [&](i32& dy, i32& dx) {
        dy = 0;
        dx = 0;
        if (jo::as_button(g)->pressed != 0) {
            const float s = global<float>(d::flt_1) * f18;
            dy = joan::orig::arg::as<i16>(joan::orig::as_misc(kRes)->round(joan::orig::arg::f64(static_cast<double>(s))));  // 0x00590a20
        }
        if (jo::as_button(g)->pressed != 0) {
            const float s = global<float>(d::flt_1) * f14;
            dx = joan::orig::arg::as<i16>(joan::orig::as_misc(kRes)->round(joan::orig::arg::f64(static_cast<double>(s))));  // 0x00590a20
        }
    };
    auto label = [&](u8 rgb) {
        i32 dy = 0, dx = 0;
        offsets(dy, dx);
        const u32 c = colour(rgb, rgb, rgb, alpha_of(g));
        box_text(font, x4() + dx, y8() + dy - 1, static_cast<i32>(static_cast<u16>(wc)),
                 static_cast<i32>(static_cast<i16>(w10)) - 1, target, jo::as_button(g)->caption, c);
    };
    if (jo::as_button(g)->pressed != 0) {
        const i32 tw = joan::orig::as_font(font)->measure(joan::orig::arg::str(static_cast<void*>(jo::as_button(g)->caption)), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_font(font)->text_height(joan::orig::arg::i16(2), 1.0f);                                                        // 0x006253f0
        {
            const u32 c = colour(0xff, 0xff, 0xff, alpha_of(g));
            text_draw((x4() + xc() - tw) / 2 - 0x14, y8() + 8, target, mark, style, c);
        }
        {
            const u32 c = colour(0xff, 0xff, 0xff, alpha_of(g));
            text_draw((tw + x4() + xc()) / 2 + 6, y8() + 8, target, mark, style, c);
        }
        label(0);
        return 1;
    }
    if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(x4()), joan::orig::arg::i32(y8()), joan::orig::arg::i32(xc()), static_cast<i32>(static_cast<i16>(w10)))) != 0) {  // 0x00593980
        const i32 tw = joan::orig::as_font(font)->measure(joan::orig::arg::str(static_cast<void*>(jo::as_button(g)->caption)), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_font(font)->text_height(joan::orig::arg::i16(2), 1.0f);                                                        // 0x006253f0
        {
            const u32 c = colour(0xff, 0xff, 0xff, alpha_of(g));
            const i32 iv = x4() + xc() - tw;
            double v = fpu.mul(static_cast<double>(iv), global<double>(d::dbl_0_5));  // 0.5
            v = fpu.sub(v, global<double>(d::dbl_20));                              // 20.0
            text_draw(joan::hook::x87::ftol(v), y8() + 8, target, mark, style, c);
        }
        {
            const u32 c = colour(0xff, 0xff, 0xff, alpha_of(g));
            const i32 iv = tw + x4() + xc();
            double v = fpu.mul(static_cast<double>(iv), global<double>(d::dbl_0_5));  // 0.5
            v = fpu.add(v, global<double>(d::dbl_9ddb80));                              // 6.0
            text_draw(joan::hook::x87::ftol(v), y8() + 8, target, mark, style, c);
        }
        label(0xff);
        return 1;
    }
    label(0xff);
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_07", name, addr, fn)
B("draw_mark_label", 0x006f5200, &draw_mark_label);
#undef B

}  // namespace game_b109_07
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::orig::helpers::crt_memset;

struct H {
    H* clear_c() {  // 0x006f5a20
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::Menu)));  // 0xc (the Menu 0xa52930)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::clear_c", 0x006f5a20, &H::clear_c);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

struct H {
    // ------------------------------------------------------------------------ 0x006b1f10 etc.
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::nop_6f5a50", 0x006f5a50, &H::nop);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {


JOAN_COUNTER(c_mode_check, "Mode::check_6f5a70");

struct H {
    // ------------------------------------------------------------------ 0x006f5a70
    i32 mode_check() {
        c_mode_check.hit();
        if (global<i8>(d::g_a45cd1) != 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_counter(obj(d::g_scenarios))->at_least_10()) == 0) return 0;  // 0x0041c7e0
        if (global<i8>(d::g_a45ccf) != 0) return 0;
        u32 who = global<u32>(d::g_control_mode);
        if (who == 4) who = global<u32>(d::g_control_base_mode);
        i32 mode = 0;
        void* const flags = obj(d::g_keys_a48820);
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(flags)->test(joan::orig::arg::i32(who), 0x22)) != 0 &&                // 0x004cb820
            static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_save_menu(obj(d::g_save_menu))->is_open())) == 0 &&  // 0x00430370
            static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_opt_menu(obj(d::g_opt_menu))->is_open())) == 0 &&  // 0x004303a0
            static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_obj_menu(obj(d::g_objective_menu))->is_open())) == 0 &&  // 0x004303d0
            global<i8>(d::g_msg_log) == 0)
            mode = 1;
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(flags)->test(joan::orig::arg::i32(who), 0x23)) != 0) mode = 2;
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(flags)->test(joan::orig::arg::i32(who), 0x24)) != 0) mode = 2;
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(flags)->test(joan::orig::arg::i32(who), 0x25)) != 0) mode = 3;
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(flags)->test(joan::orig::arg::i32(who), 0x26)) != 0) mode = 4;
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(flags)->test(joan::orig::arg::i32(who), 0x27)) != 0) mode = 5;
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(flags)->test(joan::orig::arg::i32(who), 0x28)) != 0) mode = 6;
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(flags)->test(joan::orig::arg::i32(who), 0x29)) != 0) mode = 7;
        if (mode == 0) return 0;
        if (jo::view<i32>(jo::self<jo::Menu>(this)->state) == 0) {
            jo::view<i32>(jo::self<jo::Menu>(this)->next_state) = mode;
            joan::orig::as_menu(this)->set_state_6f5cc0();  // 0x006f5cc0
            return 1;
        }
        if (jo::view<i32>(jo::self<jo::Menu>(this)->state) == mode) {
            jo::view<i32>(jo::self<jo::Menu>(this)->next_state) = 0;
            return 2;
        }
        jo::view<i32>(jo::self<jo::Menu>(this)->next_state) = mode;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Mode::check_6f5a70", 0x006f5a70, &H::mode_check);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {


JOAN_COUNTER(c_state_6f5cc0, "Menu::set_state_6f5cc0");

constexpr img::Addr kUnits = d::g_units;  // unit table (0x00428b70 check, 0x0041b3c0 get)

struct H {

    // -------------------------------------------------------------- 0x006f5cc0
    std::uintptr_t set_state_6f5cc0() {
        c_state_6f5cc0.hit();
        jo::self<jo::Menu>(this)->reopen = (jo::self<jo::Menu>(this)->next_state != 0 && jo::view<i32>(jo::self<jo::Menu>(this)->state) == 1) ? 1u : 0u;
        jo::self<jo::Menu>(this)->state = jo::self<jo::Menu>(this)->next_state;
        jo::self<jo::Menu>(this)->next_state = 0;
        const u32 state = jo::self<jo::Menu>(this)->state;
        switch (state - 1) {
            case 0: return joan::orig::arg::as<u32>(joan::orig::as_screen(obj(d::g_ingame_menu))->open_6f3730());  // 0x006f3730
            case 1:
                joan::orig::as_panel(obj(d::g_a56398))->set_mode(static_cast<char>(joan::orig::arg::i32(u8{1})));  // 0x006f7f20
                if (global<i32>(d::g_control_mode) == 4 &&
                    joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_1238()))) == 0)  // 0x006f5ed0
                    joan::orig::as_screen(obj(d::g_a56398))->open_level(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_1238())));  // 0x006f7d80
                else
                    joan::orig::as_screen(obj(d::g_a56398))->open_level(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));
                return 0;  // 0x006f7d80 returns nothing; callers of set_state_6f5cc0 ignore the result
            case 2:
                global<u8>(d::g_savemenu_load_mode) = 0;
                return joan::orig::arg::as<u32>(joan::orig::as_save_menu(obj(d::g_save_menu))->open(static_cast<std::uint8_t>(joan::orig::arg::u32(u8{0})), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(jo::self<jo::Menu>(this)->reopen)))));  // 0x0074da40
            case 3:
                global<u8>(d::g_savemenu_load_mode) = 1;
                return joan::orig::arg::as<u32>(joan::orig::as_save_menu(obj(d::g_save_menu))->open(static_cast<std::uint8_t>(joan::orig::arg::u32(u8{0})), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(jo::self<jo::Menu>(this)->reopen)))));
            case 4: return joan::orig::arg::as<u32>(joan::orig::as_opt_menu(obj(d::g_opt_menu))->open(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u8>(jo::self<jo::Menu>(this)->reopen)))));  // 0x0073c9d0
            case 5: return joan::orig::arg::as<u32>(joan::orig::as_menu(obj(d::g_objective_menu))->open_7226a0(joan::orig::arg::i32(static_cast<u8>(jo::self<jo::Menu>(this)->reopen))));  // 0x007226a0
            case 6: return joan::orig::arg::as<u32>(joan::orig::as_panel(obj(d::g_msg_log))->show_6e19b0(static_cast<std::int8_t>(joan::orig::arg::i32(u8{1})), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(jo::self<jo::Menu>(this)->reopen)))));  // 0x006e19b0
            default: return state;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Menu::set_state_6f5cc0", 0x006f5cc0, &H::set_state_6f5cc0);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {


struct H {
    // 0x006f5e90
    void* triple_clear() {
        jo::self<jo::Triple>(this)->c = 0;
        jo::self<jo::Triple>(this)->a = 0;
        jo::self<jo::Triple>(this)->b = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Triple::clear", 0x006f5e90, &H::triple_clear);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::IngameMenu::release() -> void { joan::bind::member<&::game_b93_06::H::reset_6f3270>(this); }  // 0x6f3270
auto joan::orig::Screen::captions_6f3330() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_15::H::captions_6f3330>(this)); }  // 0x6f3330
auto joan::orig::IngameMenu::free_sprites() -> void { joan::bind::member<&::game_b92_06::H::release5>(this); }  // 0x6f3630
auto joan::orig::Screen::open_6f3730() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b102_15::H::open_6f3730>(this)); }  // 0x6f3730
auto joan::orig::IngameMenu::layout_buttons() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::layout_buttons>(this)); }  // 0x6f38a0
auto joan::orig::Panel::layout() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08::H::panel_layout>(this)); }  // 0x6f4a10
auto joan::orig::Mode::check_6f5a70() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_15::H::mode_check>(this)); }  // 0x6f5a70
auto joan::orig::Menu::set_state_6f5cc0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::set_state_6f5cc0>(this)); }  // 0x6f5cc0
auto joan::orig::Triple::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::triple_clear>(this)); }  // 0x6f5e90
auto joan::orig::Player::unit_1238() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_06::H::get_1238>(this)); }  // 0x6f5ed0
