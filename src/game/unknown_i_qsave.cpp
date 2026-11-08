// game/unknown_i_qsave.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_qsave.cpp;
// re/attribution.csv: between ww/interfac/i_letter.cpp and ww/interfac/i_sms.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
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

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

using joan::orig::helpers::addr;

using joan::img::obj;

using joan::img::str;

using joan::hook::global;

using i32 = std::int32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;

}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {


JOAN_COUNTER(c_ctor_6e32d0, "X::ctor_6e32d0");

struct H {
    std::uintptr_t ctor_6e32d0() {  // 0x006e32d0
        c_ctor_6e32d0.hit();
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->save_text)->construct();    // 0x00629400
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->save_bar)->construct();
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->load_text)->construct();
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->load_bar)->construct();
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->loadc_text)->construct();
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->loadc_bar)->construct();
        jo::self<jo::QuickBar>(this)->texture = nullptr;
        jo::self<jo::QuickBar>(this)->sprites = nullptr;
        jo::view<u16>(jo::self<jo::QuickBar>(this)->state) = 0;
        jo::self<jo::QuickBar>(this)->timer = 0;
        jo::self<jo::QuickBar>(this)->u8_45d = 0;
        return addr(this);
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::ctor_6e32d0", 0x006e32d0, &H::ctor_6e32d0);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b104_14.cpp (game/b104_14)
namespace {
namespace game_b104_14 {

constexpr u32 kOne = 0x3f800000u;    // 1.0f

constexpr u32 kM1 = 0xffffffffu;


// Rgba(r, g, b, a) 0x0042a260 / Rgba(dword) 0x005e9640 into a local, value returned
u32 colour(u32 r, u32 g, u32 b, u32 a) {
    u32 storage = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&storage)->construct(r, g, b, a));  // 0x0042a260
}
// bitmap item 0x00629660 (19 args, ret 0x4c), common tail 3,0,2,0,-1,-1,-1,-1,0,-1,-1,0,1,1
u32 bitmap(void* item, u32 x, u32 y, const void* font, img::Addr name, u32 col) {
    return joan::orig::arg::as<u32>(joan::orig::as_label(item)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::mptr(font), joan::orig::arg::str(img::addr(name)), joan::orig::arg::i32(col), 3, 0, 2, 0, joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), 0, joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), 0, 1, 1));  // 0x00629660
}
// text item 0x00629530 (15 args, ret 0x3c), common tail 0,0,2,0,1f,1f,0,1,1
u32 text(void* item, u32 x, u32 y, img::Addr font, const char* str, u32 a5, u32 col) {
    return joan::orig::arg::as<u32>(joan::orig::as_image2d(item)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), static_cast<jo::Font2d*>(obj(font)), joan::orig::arg::str(str), joan::orig::arg::i32(a5), joan::orig::arg::i32(col), 0, 0, 2, 0, joan::orig::arg::i32(kOne), joan::orig::arg::i32(kOne), 0, 1, 1));  // 0x00629530
}
void place(void* item, u32 a, u32 b, u32 c, u32 d, u32 e) {
    joan::orig::as_widget(item)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
}

JOAN_COUNTER(c_quickbar, "QuickBar::init_6e33e0");

struct H {
    // ------------------------------------------------------------------ 0x006e33e0
    H* init_6e33e0() {
        c_quickbar.hit();
        jo::self<jo::QuickBar>(this)->sprites = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_qsave));
        jo::self<jo::QuickBar>(this)->texture = nullptr;
        {
            const u32 c = colour(0, 0, 0, 0xff);
            bitmap(&jo::self<jo::QuickBar>(this)->save_bar, 0xffffff24u, 0x204u, jo::self<jo::QuickBar>(this)->sprites, d::s_qs_bar, c);  // 'qs_bar'
            place(&jo::self<jo::QuickBar>(this)->save_bar, 0, kOne, kOne, 1, 0);
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            const char* const s = joan::orig::as_txt_res(obj(d::g_strings))->qsave_text_4();  // 0x004dcd70
            text(&jo::self<jo::QuickBar>(this)->save_text, 0xffffff24u, 0x204u, d::g_font_goudy14_22_white, s, 0, c);
            place(&jo::self<jo::QuickBar>(this)->save_text, 0, kOne, kOne, 1, 0);
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            bitmap(&jo::self<jo::QuickBar>(this)->loadc_bar, 0xffffff08u, 0x204u, jo::self<jo::QuickBar>(this)->sprites, d::s_qlc_bar, c);  // 'qlc_bar'
            place(&jo::self<jo::QuickBar>(this)->loadc_bar, 0, kOne, kOne, 1, 0);
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            const char* const s = joan::orig::as_menu(obj(d::g_strings))->qsave_6();  // 0x004dcdd0
            text(&jo::self<jo::QuickBar>(this)->loadc_text, 0xffffff08u, 0x204u, d::g_font_goudy14_22_white, s, 0, c);
            place(&jo::self<jo::QuickBar>(this)->loadc_text, 0, kOne, kOne, 1, 0);
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            bitmap(&jo::self<jo::QuickBar>(this)->load_bar, 0xffffff08u, 0x204u, jo::self<jo::QuickBar>(this)->sprites, d::s_ql_bar, c);  // 'ql_bar'
            place(&jo::self<jo::QuickBar>(this)->load_bar, 0, kOne, kOne, 1, 0);
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            const char* const s = joan::orig::as_txt_res(obj(d::g_strings))->qsave_text_5();  // 0x004dcda0
            text(&jo::self<jo::QuickBar>(this)->load_text, 0xffffff08u, 0x204u, d::g_font_goudy14_22_white, s, 0, c);
            place(&jo::self<jo::QuickBar>(this)->load_text, 0, kOne, kOne, 1, 0);
        }
        jo::view<u16>(jo::self<jo::QuickBar>(this)->state) = 0;
        jo::self<jo::QuickBar>(this)->timer = 0;
        jo::self<jo::QuickBar>(this)->u8_45d = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_14", name, addr, fn)
B("QuickBar::init_6e33e0", 0x006e33e0, &H::init_6e33e0);
#undef B

}  // namespace game_b104_14
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

void* const kImages = obj(d::g_texture_array);


u32 image_mode() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kImages)->mode()); }                // 0x0060a720
void set_image_mode(u32 m) { joan::orig::as_texture_manager(kImages)->set_mode(joan::orig::arg::u32(m)); }               // 0x0060a740
jo::Texture* load_image(const char* name) { return static_cast<jo::Texture*>(joan::orig::as_texture_manager(kImages)->load(joan::orig::arg::str(name), 1, 0u)); }  // 0x006099c0

struct H {
    void panel_reset_6e37d0() {  // 0x006e37d0
        joan::orig::as_quick_save_screen(this)->free_sprite();  // 0x006e45b0
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->save_bar)->release();  // 0x00629830
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->save_text)->release();
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->loadc_bar)->release();
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->loadc_text)->release();
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->load_bar)->release();
        joan::orig::as_image2d(&jo::self<jo::QuickBar>(this)->load_text)->release();
        jo::view<u16>(jo::self<jo::QuickBar>(this)->state) = 0;
        jo::self<jo::QuickBar>(this)->timer = 0;
        jo::self<jo::QuickBar>(this)->u8_45d = 0;
    }

    void panel_reset_6e44b0() {  // 0x006e44b0
        jo::self<jo::QuickBar>(this)->active = 0;
        joan::orig::as_quick_save_screen(this)->free_sprite();  // 0x006e45b0
        jo::view<u16>(jo::self<jo::QuickBar>(this)->state) = 0;
        jo::self<jo::QuickBar>(this)->timer = 0;
        jo::self<jo::QuickBar>(this)->u8_45d = 0;
    }

    void load_qsave() {  // 0x006e4540
        const u32 mode = image_mode();
        set_image_mode(1);
        if (jo::self<jo::QuickBar>(this)->texture == nullptr) jo::self<jo::QuickBar>(this)->texture = load_image(str(d::s_QSAVE));  // "QSAVE"
        set_image_mode(mode);
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Panel::reset_6e37d0", 0x006e37d0, &H::panel_reset_6e37d0);
B94("Panel::reset_6e44b0", 0x006e44b0, &H::panel_reset_6e44b0);
B94("QuitPopup::load_image", 0x006e4540, &H::load_qsave);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {


JOAN_COUNTER(c_check_weather, "Gadget::check_weather_6e3890");

struct H {
    // ------------------------------------------------------------------ 0x006e3890
    u32 check_weather() {
        c_check_weather.hit();
        if (jo::view<i8>(jo::self<jo::QuickBar>(this)->active) == 0) return 0;
        if (jo::self<jo::QuickBar>(this)->popup_open == 0) return 0;
        void* const w = obj(d::g_popup_a52df8);
        if (joan::orig::arg::as<i32>(joan::orig::as_popup(w)->poll_buttons()) != 0) {  // 0x006d5120
            if (joan::orig::arg::as<i32>(joan::orig::as_popup(w)->result()) == 1) {  // 0x006d5370
                joan::orig::as_quick_save_screen(this)->set_confirm_open(static_cast<std::int8_t>(joan::orig::arg::i32(0)));   // 0x006e4420
                joan::orig::as_cursor(obj(d::g_mouse))->set_pos();  // 0x00592bd0
            } else if (joan::orig::arg::as<i32>(joan::orig::as_popup(w)->result()) == 2) {  // 0x006d5370
                joan::orig::as_panel(this)->reset_6e44b0();  // 0x006e44b0
                if (global<u8>(d::g_game_running) == 0) {
                    global<u8>(d::g_game_running) = 1;
                    const float v = static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)));
                    joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(v));  // 0x0042f930
                }
                joan::orig::as_quick_save_screen(this)->set_confirm_open(static_cast<std::int8_t>(joan::orig::arg::i32(0)));     // 0x006e4420
                joan::orig::as_cursor(obj(d::g_mouse))->set_pos();  // 0x00592bd0
            }
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Gadget::check_weather", 0x006e3890, &H::check_weather);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_ui6.cpp (game/ui6)
namespace {
namespace game_ui6 {


JOAN_COUNTER(c_buttons, "Panel::refresh_buttons");

struct H {
    // 0x006e39a0. ret 0.
    void refresh_buttons() {
        c_buttons.hit();
        if (static_cast<std::int8_t>(jo::self<jo::QuickBar>(this)->active) == 0) return;
        if (static_cast<std::int8_t>(jo::self<jo::QuickBar>(this)->u8_45d) == 0) {
            joan::orig::as_label(&jo::self<jo::QuickBar>(this)->save_bar)->draw(5u, 7u);  // 0x00629bd0
            joan::orig::as_label(&jo::self<jo::QuickBar>(this)->save_text)->draw(5u, 7u);
            return;
        }
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::QuickBar>(this)->popup_open)) != 0) {
            joan::orig::as_dialog(obj(d::g_popup_a52df8))->init();  // 0x006d5080
            return;
        }
        joan::orig::as_label(&jo::self<jo::QuickBar>(this)->loadc_bar)->draw(5u, 7u);
        joan::orig::as_label(&jo::self<jo::QuickBar>(this)->loadc_text)->draw(5u, 7u);
        joan::orig::as_label(&jo::self<jo::QuickBar>(this)->load_bar)->draw(5u, 7u);
        joan::orig::as_label(&jo::self<jo::QuickBar>(this)->load_text)->draw(5u, 7u);
    }
};

#define U6(name, addr, fn) JOAN_REPLACE("game/ui6", name, addr, fn)
U6("Panel::refresh_buttons", 0x006e39a0, &H::refresh_buttons);
#undef U6

}  // namespace game_ui6
}  // namespace

// ---------------------------------------- from modules/game_b109_03.cpp (game/b109_03)
namespace {
namespace game_b109_03 {


JOAN_COUNTER(c_intro, "Intro::animate_6e3a90");

struct H {
    u8* me() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x006e3a90
    void intro_animate_6e3a90() {
        c_intro.hit();
        u8* const self = me();
        auto slide = [&](jo::Image2D& w, u32 flag, u32 speed) {  // 0x0061f1b0
            joan::orig::as_widget(&w)->scale_to(joan::orig::arg::i16(flag), joan::orig::arg::f32(speed), 0.0f, 0, 0u);
        };
        auto move = [&](jo::Image2D& w, u32 a, u32 b, u32 c, u32 d) {  // 0x0061e020
            joan::orig::as_widget(&w)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), 0, 0u);
        };
        auto state = [&]() -> i32 { return jo::self<jo::QuickBar>(self)->state; };
        auto timer = [&]() -> u32& { return jo::self<jo::QuickBar>(self)->timer; };
        if (jo::view<i8>(jo::self<jo::QuickBar>(self)->active) == 0) return;
        if (jo::self<jo::QuickBar>(self)->popup_open != 0) return;
        if (jo::view<i8>(jo::self<jo::QuickBar>(self)->u8_45d) == 0) {
            if (state() == 0) {
                global<u8>(d::g_a48c78) = 0;
                slide(jo::self<jo::QuickBar>(self)->save_bar, 1, 0x3f99999a);
                move(jo::self<jo::QuickBar>(self)->save_bar, 0, 0x204, 0x442f0000, 0xc47a0000);
                jo::self<jo::QuickBar>(self)->state = 1;
            }
            if (state() == 1 &&
                static_cast<double>(static_cast<i32>(jo::self<jo::QuickBar>(self)->save_bar.x)) >= global<double>(d::dbl_m176)) {
                slide(jo::self<jo::QuickBar>(self)->save_text, 1, 0x3f99999a);
                move(jo::self<jo::QuickBar>(self)->save_text, 0, 0x204, 0x442f0000, 0xc47a0000);
                jo::self<jo::QuickBar>(self)->state = 2;
            }
            if (state() == 2 && jo::self<jo::QuickBar>(self)->save_text.x == 0) {
                jo::self<jo::QuickBar>(self)->state = 3;
                timer() = 0;
            }
            if (state() == 3) {
                if (timer() < 0x514u) {
                    timer() = timer() + global<u32>(d::g_frame_elapsed_ms);
                } else {
                    slide(jo::self<jo::QuickBar>(self)->save_text, 0, 0x3fc00000);
                    move(jo::self<jo::QuickBar>(self)->save_text, 0xffffff24, 0x204, 0, 0x44c80000);
                    jo::self<jo::QuickBar>(self)->state = 4;
                }
            }
            if (state() == 4 &&
                !(static_cast<double>(static_cast<i32>(jo::self<jo::QuickBar>(self)->save_text.x)) > global<double>(d::dbl_9de270))) {
                slide(jo::self<jo::QuickBar>(self)->save_bar, 0, 0x3fc00000);
                move(jo::self<jo::QuickBar>(self)->save_bar, 0xffffff24, 0x204, 0, 0x44c80000);
                jo::self<jo::QuickBar>(self)->state = 5;
            }
            if (state() == 5 && static_cast<i32>(jo::self<jo::QuickBar>(self)->save_bar.x) == -0xdc) {
                global<u8>(d::g_a48c78) = 1;
                joan::orig::as_panel(self)->reset_6e44b0();  // 0x006e44b0
                return;
            }
            joan::orig::as_widget(&jo::self<jo::QuickBar>(self)->save_bar)->update();  // 0x0061f3b0
            joan::orig::as_widget(&jo::self<jo::QuickBar>(self)->save_text)->update();
            return;
        }
        if (state() == 0) {
            joan::orig::as_sms(obj(d::g_sms))->clear();  // 0x006e5e00
            global<u8>(d::g_a48c78) = 0;
            slide(jo::self<jo::QuickBar>(self)->load_bar, 1, 0x3fa66666);
            move(jo::self<jo::QuickBar>(self)->load_bar, 0, 0x204, 0x442f0000, 0xc45c0000);
            move(jo::self<jo::QuickBar>(self)->loadc_bar, 0, 0x204, 0x442f0000, 0xc45c0000);
            jo::self<jo::QuickBar>(self)->state = 1;
        }
        if (state() == 1 &&
            static_cast<double>(static_cast<i32>(jo::self<jo::QuickBar>(self)->load_bar.x)) >= global<double>(d::dbl_m198_4)) {
            slide(jo::self<jo::QuickBar>(self)->load_text, 1, 0x3fa66666);
            move(jo::self<jo::QuickBar>(self)->load_text, 0, 0x204, 0x442f0000, 0xc45c0000);
            move(jo::self<jo::QuickBar>(self)->loadc_text, 0, 0x204, 0x442f0000, 0xc45c0000);
            jo::self<jo::QuickBar>(self)->state = 2;
            timer() = 0;
        }
        if (state() == 2 && jo::self<jo::QuickBar>(self)->load_text.x == 0) {
            if (timer() < 0x1f4u) {
                timer() = timer() + global<u32>(d::g_frame_elapsed_ms);
            } else {
                jo::self<jo::QuickBar>(self)->state = 3;
                timer() = 0;
                joan::orig::as_battle(obj(d::g_scenarios))->start_41ea60(joan::orig::arg::mptr(jo::self<jo::QuickBar>(self)->u32_459));  // 0x0041ea60
                if (global<u8>(d::g_game_running) == 0) {
                    global<u8>(d::g_game_running) = 1;
                    joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)))));  // 0x0042f930
                }
            }
        }
        if (state() == 3) {
            if (timer() < 0x64u) {
                timer() = timer() + global<u32>(d::g_frame_elapsed_ms);
            } else {
                slide(jo::self<jo::QuickBar>(self)->load_text, 0, 0x3f4ccccd);
                slide(jo::self<jo::QuickBar>(self)->load_bar, 0, 0x3f4ccccd);
                slide(jo::self<jo::QuickBar>(self)->loadc_text, 1, 0x3f19999a);
                slide(jo::self<jo::QuickBar>(self)->loadc_bar, 1, 0x3f19999a);
                jo::self<jo::QuickBar>(self)->state = 4;
                timer() = 0;
            }
        }
        if (state() == 4) {
            const float v = jo::self<jo::QuickBar>(self)->loadc_bar.scale;
            const float one = global<float>(d::flt_1);
            if (v == one || v != v) {  // test ah,0x40 jne: equal or unordered
                if (timer() < 0x514u) {
                    timer() = timer() + global<u32>(d::g_frame_elapsed_ms);
                } else {
                    slide(jo::self<jo::QuickBar>(self)->loadc_text, 0, 0x3fc00000);
                    move(jo::self<jo::QuickBar>(self)->loadc_text, 0xffffff08, 0x204, 0, 0x44c80000);
                    move(jo::self<jo::QuickBar>(self)->load_text, 0xffffff08, 0x204, 0, 0x44c80000);
                    jo::self<jo::QuickBar>(self)->state = 5;
                }
            }
        }
        if (state() == 5 &&
            !(static_cast<double>(static_cast<i32>(jo::self<jo::QuickBar>(self)->loadc_text.x)) > global<double>(d::dbl_m49_6))) {
            slide(jo::self<jo::QuickBar>(self)->loadc_bar, 0, 0x3fc00000);
            move(jo::self<jo::QuickBar>(self)->loadc_bar, 0xffffff08, 0x204, 0, 0x44c80000);
            move(jo::self<jo::QuickBar>(self)->load_bar, 0xffffff08, 0x204, 0, 0x44c80000);
            jo::self<jo::QuickBar>(self)->state = 6;
        }
        if (state() == 6 && static_cast<i32>(jo::self<jo::QuickBar>(self)->load_bar.x) == -0xf8) {
            joan::orig::as_sms(obj(d::g_sms))->show(i32{0});  // 0x006e5e60
            global<i16>(d::g_a48c5e) = 0;
            joan::orig::as_panel(self)->reset_6e44b0();
            return;
        }
        joan::orig::as_widget(&jo::self<jo::QuickBar>(self)->loadc_bar)->update();
        joan::orig::as_widget(&jo::self<jo::QuickBar>(self)->loadc_text)->update();
        joan::orig::as_widget(&jo::self<jo::QuickBar>(self)->load_bar)->update();
        joan::orig::as_widget(&jo::self<jo::QuickBar>(self)->load_text)->update();
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_03", name, addr, fn)
B("Intro::animate_6e3a90", 0x006e3a90, &H::intro_animate_6e3a90);
#undef B

}  // namespace game_b109_03
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using joan::hook::x87::ftol;


void reset_timer_a500d2() {
    if (global<u8>(d::g_game_running) == 0) return;
    global<u8>(d::g_game_running) = 0;
    global<u16>(d::g_saved_game_speed) = static_cast<u16>(ftol(static_cast<double>(global<float>(d::g_game_speed))));
    joan::orig::as_game(obj(d::g_game_a50088))->set_speed(0u);  // 0x0042f930
}

JOAN_COUNTER(c_set_mode, "Screen::set_mode_6e4360");

struct H {
    // ------------------------------------------------------------------ 0x006e4360
    void* set_mode_6e4360(u32 mode) {
        c_set_mode.hit();
        jo::self<jo::QuickBar>(this)->active = 1;
        jo::self<jo::QuickBar>(this)->u8_45d = static_cast<u8>(mode);
        if (static_cast<i32>(static_cast<i8>(mode)) == 1) {
            joan::orig::as_quick_save_screen(this)->set_confirm_open(static_cast<std::int8_t>(joan::orig::arg::i32(1)));  // 0x006e4420
            reset_timer_a500d2();
            joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
        }
        joan::orig::as_quit_popup(this)->load_image();  // 0x006e4540
        jo::view<u16>(jo::self<jo::QuickBar>(this)->state) = 0;
        jo::self<jo::QuickBar>(this)->timer = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen::set_mode_6e4360", 0x006e4360, &H::set_mode_6e4360);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b101_15.cpp (game/b101_15)
namespace {
namespace game_b101_15 {


void* const kMenus = obj(d::g_strings);

JOAN_COUNTER(c_popup_set_open, "PopupHost::set_open");

struct H {
    // 0x006e4420 (ret 4)
    std::uintptr_t popup_set_open(i8 on) {
        c_popup_set_open.hit();
        jo::self<jo::QuickBar>(this)->popup_open = on;
        void* const popup = obj(d::g_popup_a52df8);
        if (static_cast<i32>(on) != 0) {
            joan::orig::as_popup(popup)->set_title(joan::orig::arg::str(0u), 0u);                     // 0x006d52c0
            const char* const text = joan::orig::as_txt_res(kMenus)->qsave_text_1();      // 0x004dcce0
            joan::orig::as_popup(popup)->set_text_6d5230(text, 0u);                   // 0x006d5230
            joan::orig::as_popup(popup)->set_buttons(3u);                         // 0x006d5340
            return joan::orig::arg::as<u32>(joan::orig::as_popup(popup)->load_gpopup());                 // 0x006d3be0
        }
        return joan::orig::arg::as<u32>(joan::orig::as_popup(popup)->free_gpopup());  // 0x006d3c60
    }
};

#define B15(name, addr, fn) JOAN_REPLACE("game/b101_15", name, addr, fn)
B15("PopupHost::set_open", 0x006e4420, &H::popup_set_open);
#undef B15

}  // namespace game_b101_15
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {


struct H {
    // ------------------------------------------------------------------ 0x006e4510
    u8 byte_45e() { return jo::self<jo::QuickBar>(this)->active; }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::byte_45e", 0x006e4510, &H::byte_45e);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {


void* const kRes = obj(d::g_texture_array);

void res_free(void* handle) { joan::orig::as_texture_manager(kRes)->release(joan::orig::arg::mptr(handle)); }  // 0x0060a550

struct H {
    void free1() {  // 0x006d66f0 / 0x006e45b0 / 0x006e4dd0 / 0x006e5fc0; this one on the QuickBar (the Res1 view of +0 read 8 bytes across texture and sprites on x64)
        if (jo::self<jo::QuickBar>(this)->texture != nullptr) {
            res_free(jo::self<jo::QuickBar>(this)->texture);
            jo::self<jo::QuickBar>(this)->texture = nullptr;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Res1::free_6e45b0", 0x006e45b0, &H::free1);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Panel::reset_6e37d0() -> void { joan::bind::member<&::game_b94_08::H::panel_reset_6e37d0>(this); }  // 0x6e37d0
auto joan::orig::Panel::refresh_buttons() -> void { joan::bind::member<&::game_ui6::H::refresh_buttons>(this); }  // 0x6e39a0
auto joan::orig::Screen::set_mode_6e4360(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b102_15::H::set_mode_6e4360>(this, a0)); }  // 0x6e4360
auto joan::orig::QuickSaveScreen::set_confirm_open(std::int8_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_15::H::popup_set_open>(this, a0)); }  // 0x6e4420
auto joan::orig::Panel::reset_6e44b0() -> void { joan::bind::member<&::game_b94_08::H::panel_reset_6e44b0>(this); }  // 0x6e44b0
auto joan::orig::QuickSaveScreen::flag_45e() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b97_08::H::byte_45e>(this)); }  // 0x6e4510
auto joan::orig::QuitPopup::load_image() -> void { joan::bind::member<&::game_b94_08::H::load_qsave>(this); }  // 0x6e4540
auto joan::orig::QuickSaveScreen::free_sprite() -> void { joan::bind::member<&::game_b93_06::H::free1>(this); }  // 0x6e45b0
