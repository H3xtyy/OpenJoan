// game/unknown_i_quit.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_quit.cpp;
// re/attribution.csv: between ww/interfac/i_letter.cpp and ww/interfac/i_sms.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using i16 = std::int16_t;

using joan::img::obj;

using u32 = std::uint32_t;

using u16 = std::uint16_t;

using joan::img::str;

constexpr u32 kOne = 0x3f800000u;    // 1.0f

using i8 = std::int8_t;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {


struct H {
    // ------------------------------------------------------------------ 0x006e4600
    void* ctor_6e4600() {
        joan::orig::as_image2d(&jo::self<jo::QuitPopup>(this)->background)->construct();  // 0x00629400
        jo::self<jo::QuitPopup>(this)->texture = nullptr;
        jo::self<jo::QuitPopup>(this)->step = 0;
        jo::view<u8>(jo::self<jo::QuitPopup>(this)->question) = 0;
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj6e4600::Obj", 0x006e4600, &H::ctor_6e4600);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {


JOAN_COUNTER(c_qm_init, "QuestionMark::init");

// 0x0042a260 Color::Color(b0, b1, b2, b3) on a 4-byte local; returns its address.
u32 make_color(i32 b0, i32 b1, i32 b2, i32 b3) {
    u32 c = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c)->construct(b0, b1, b2, b3));
}
// 0x0061f1b0 Widget::set_anim(a, b, c, d, e)
void anim(void* w, i32 a, u32 b, i32 c, i32 d, i32 e) { joan::orig::as_widget(w)->scale_to_61f1b0(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e)); }

struct H {
    // ------------------------------------------------------------------ 0x006e4650
    void* qm_init() {
        c_qm_init.hit();
        jo::self<jo::QuitPopup>(this)->texture = nullptr;
        const u32 color = make_color(0, 0, 0, 0xff);
        joan::orig::as_label(&jo::self<jo::QuitPopup>(this)->background)->create(0, 0x80, joan::orig::arg::mptr(obj(d::g_bmp_res_quit)), str(d::s_bg_qm), joan::orig::arg::i32(color), joan::orig::arg::i32(// 0x00629660
                      3), 0, 2, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);
        anim(&jo::self<jo::QuitPopup>(this)->background, 0, kOne, 0, 1, 0);
        jo::view<u16>(jo::self<jo::QuitPopup>(this)->step) = 0;
        jo::view<u8>(jo::self<jo::QuitPopup>(this)->question) = 0;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("QuestionMark::init", 0x006e4650, &H::qm_init);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {


struct H {
    void panel_reset_6e4720() {  // 0x006e4720
        joan::orig::as_quit_popup(this)->unload();  // 0x006e4dd0
        joan::orig::as_image2d(&jo::self<jo::QuitPopup>(this)->background)->release();  // 0x00629830
        jo::view<u16>(jo::self<jo::QuitPopup>(this)->step) = 0;
        jo::view<u8>(jo::self<jo::QuitPopup>(this)->question) = 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Panel::reset_6e4720", 0x006e4720, &H::panel_reset_6e4720);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {


struct H {
    // ------------------------------------------------------------------------ 0x006e4770
    i32 flag_6e4770() { return static_cast<i32>(jo::self<jo::QuitPopup>(this)->open) != 0 ? 1 : 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::flag_6e4770", 0x006e4770, &H::flag_6e4770);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b116_07.cpp (game/b116_07)
namespace {
namespace game_b116_07 {

using joan::hook::x87::ftol;

constexpr u32 kM1 = 0xffffffffu;


constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite list
constexpr img::Addr kStrings = d::g_strings;  // UI string table

// Colour helpers: 0x0042a260 (thunk 0x004098b8, r, g, b, a) and 0x005e9640 (thunk 0x00407577,
// argb dword) on a 4-byte stack object, the dword read back through the returned pointer.
u32 rgba(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
}
// Text::draw_2 0x00626080 (thunk 0x00405ee3, 11 args, ret 0x2c).
void text2(img::Addr font, u32 x, u32 y, const char* text, u32 colour, u32 a6, u32 a7, u32 a8, u32 a9) {
    joan::orig::as_text(obj(font))->draw_2(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(u32{1}), text, joan::orig::arg::u32(colour), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626080
}

JOAN_COUNTER(c_quit, "QuitPopup::draw_6e47b0");

struct H {
    // 0x006e47b0
    void quit_draw() {
        joan::canvas::ScreenScope canvas_scope(0x006e47b0);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_quit.hit();
        if (static_cast<i32>(jo::self<jo::QuitPopup>(this)->open) == 0) return;
        joan::orig::as_label(&jo::self<jo::QuitPopup>(this)->background)->draw(1u, 7u);  // 0x00629bd0
        if (static_cast<i32>(jo::self<jo::QuitPopup>(this)->step) <= 3) {
            {
                const float a = global<float>(d::flt_255) * jo::self<jo::QuitPopup>(this)->background.scale;
                const u32 c = rgba(0, 0, 0, static_cast<u32>(ftol(static_cast<double>(a))));
                fade(c);
            }
            joan::orig::as_label(&jo::self<jo::QuitPopup>(this)->background)->draw(1u, 7u);
            const float a = global<float>(d::flt_255) * jo::self<jo::QuitPopup>(this)->background.scale;
            const u32 c = rgba(0xff, 0xff, 0xff, static_cast<u32>(ftol(static_cast<double>(a))));
            const char* s;
            if (static_cast<i32>(jo::self<jo::QuitPopup>(this)->question) == 0) {
                s = joan::orig::as_txt_res(obj(kStrings))->quit_text_01();  // 0x004de0c0
            } else {
                s = joan::orig::as_menu(obj(kStrings))->quit_2();  // 0x004de0f0
            }
            text2(d::g_font_goudy_bold_gold_24, 0x3e8, 0x290, s, c, 0, 0, 2, 0);
        } else {
            fade(rgba(0, 0, 0, 0xff));
        }
    }
    // 0x00632a70 (thunk 0x00407c7a, 16 args, ret 0x40): quad over the whole screen (real size).
    static void fade(u32 c) {
        const i32 h = global<i32>(d::g_screen_h) - 1;
        const i32 w = global<i32>(d::g_screen_w) - 1;
        joan::orig::as_sprite2d(obj(kSprites))->queue(joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(c), joan::orig::arg::mptr(u32{0}), joan::orig::arg::i32(u32{3}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{2}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(u32{0}));  // 0x00632a70
    }
};

JOAN_REPLACE("game/b116_07", "QuitPopup::draw", 0x006e47b0, &H::quit_draw);
JOAN_CANVAS_SELF(0x006e47b0);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_07
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {


JOAN_COUNTER(c_intro, "Intro::step_6e4a20");

struct H {
    // ------------------------------------------------------------------ 0x006e4a20
    void intro_step_6e4a20() {
        c_intro.hit();
        if (jo::self<jo::QuitPopup>(this)->open == 0) return;
        joan::orig::as_widget(&jo::self<jo::QuitPopup>(this)->background)->update();  // 0x0061f3b0
        const i32 st = jo::self<jo::QuitPopup>(this)->step;
        if (st == 0) {
            const float v = jo::self<jo::QuitPopup>(this)->background.scale;
            if (!(v < 1.0f) && !(v > 1.0f)) jo::self<jo::QuitPopup>(this)->step = 1;  // equal or unordered
        } else if (st == 1) {
            jo::self<jo::QuitPopup>(this)->step = 2;
        } else if (st == 2) {
            joan::orig::as_handle(obj(d::g_scenarios))->release(joan::orig::arg::u32(i32{0}));  // 0x0041c780
            joan::orig::as_widget(&jo::self<jo::QuitPopup>(this)->background)->scale_to(0, 6.0f, 0.0f, 0, 0x3e8u);  // 0x0061f1b0
            jo::self<jo::QuitPopup>(this)->step = 3;
        } else if (st == 3) {
            const float v = jo::self<jo::QuitPopup>(this)->background.scale;
            if (!(v < 0.0f) && !(v > 0.0f)) jo::self<jo::QuitPopup>(this)->step = 4;
        } else if (st == 4) {
            jo::self<jo::QuitPopup>(this)->step = 5;
        } else if (st == 5) {
            const i32 next = jo::self<jo::QuitPopup>(this)->question;
            if (next == 0) {
                joan::orig::as_game(obj(d::g_game_a50088))->set_state(joan::orig::arg::u32(i32{3}));  // 0x004d7e70
            } else if (next == 1) {
                global<u8>(d::g_a50098) = 1;
                joan::orig::as_main_menu(obj(d::g_main_menu))->reset_widgets();  // 0x0070d670
            }
            joan::orig::as_quit_popup(this)->hide();  // 0x006e4cc0
            jo::self<jo::QuitPopup>(this)->step = 0;
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Intro::step_6e4a20", 0x006e4a20, &H::intro_step_6e4a20);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {


char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

// 0x0061f1b0 (5 args) / 0x0061e020 (6 args) on a button.
void button_anim(void* b, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    joan::orig::as_widget(b)->scale_to_61f1b0(joan::orig::arg::i16(a0), joan::orig::arg::f32(a1), joan::orig::arg::f32(a2), static_cast<char>(a3), a4);  // 0x0061f1b0
}
void button_reset(void* b) { button_anim(b, 0u, 0x3f800000u, 0u, 1u, 0u); }

JOAN_COUNTER(c_quit_show, "QuitPopup::show");

struct H {
    // ------------------------------------------------------------------ 0x006e4c00
    void quit_show(u32 kind) {
        c_quit_show.hit();
        jo::view<u8>(jo::self<jo::QuitPopup>(this)->open) = 1;
        jo::view<u8>(jo::self<jo::QuitPopup>(this)->question) = static_cast<u8>(kind);
        joan::orig::as_quit_popup(this)->load();  // 0x006e4d60
        char* const bg = jo::self<jo::QuitPopup>(this)->background.sprite_name;
        if (jo::self<jo::QuitPopup>(this)->question == 0)
            crt_strcpy(bg, str(d::s_bg_qm));  // "bg_qm"
        else if (jo::self<jo::QuitPopup>(this)->question == 1)
            crt_strcpy(bg, str(d::s_bg_qw));  // "bg_qw"
        button_anim(&jo::self<jo::QuitPopup>(this)->background, 1u, 0x40c00000u, 0u, 0u, 0u);
    }

    // ------------------------------------------------------------------ 0x006e4cc0
    void quit_hide() {
        jo::view<u8>(jo::self<jo::QuitPopup>(this)->open) = 0;
        button_reset(&jo::self<jo::QuitPopup>(this)->background);
        joan::orig::as_quit_popup(this)->unload();  // 0x006e4dd0
        jo::view<u16>(jo::self<jo::QuitPopup>(this)->step) = 0;
        jo::view<u8>(jo::self<jo::QuitPopup>(this)->question) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("QuitPopup::show", 0x006e4c00, &H::quit_show);
B("QuitPopup::hide", 0x006e4cc0, &H::quit_hide);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {


void* const kRes = obj(d::g_texture_array);

void res_free(void* handle) { joan::orig::as_texture_manager(kRes)->release(handle); }  // 0x0060a550

struct H {
    void free1() {  // 0x006d66f0 / 0x006e45b0 / 0x006e4dd0 / 0x006e5fc0; this one on the QuitPopup
        if (jo::self<jo::QuitPopup>(this)->texture != nullptr) {
            res_free(jo::self<jo::QuitPopup>(this)->texture);
            jo::self<jo::QuitPopup>(this)->texture = nullptr;
        }
    }

    std::uintptr_t flag_be() {  // 0x006e4d30: mov al,[this+0xbe] - upper eax keeps this
        return (reinterpret_cast<std::uintptr_t>(this) & ~std::uintptr_t{0xffu}) | jo::view<u8>(jo::self<jo::QuitPopup>(this)->open);
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6e::flag_be", 0x006e4d30, &H::flag_be);
B93("Res1::free_6e4dd0", 0x006e4dd0, &H::free1);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {


void* const kWindows = obj(d::g_texture_array);

// Window-image load with the "current window" saved / restored (0x60a720 / 0x60a740).
u32 window_save() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kWindows)->mode()); }                // 0x0060a720
void window_set(u32 v) { joan::orig::as_texture_manager(kWindows)->set_mode_60a740(v); }         // 0x0060a740
jo::Texture* window_load(const char* name) { return joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(kWindows)->load(joan::orig::arg::str(name), 1, 0u)); }  // 0x006099c0

struct H {
    // ------------------------------------------------------------------ 0x006e0ab0 / 0x006e4d60
    u32 load_window_at0(const char* name) {
        const u32 saved = window_save();
        window_set(1);
        if (jo::self<jo::QuitPopup>(this)->texture == nullptr) jo::self<jo::QuitPopup>(this)->texture = window_load(name);
        window_set(saved);
        return 0;
    }
    u32 quit_load() { return load_window_at0(str(d::s_QUIT)); }    // "QUIT"
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("QuitPopup::load", 0x006e4d60, &H::quit_load);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::QuitPopup::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_08::H::ctor_6e4600>(this)); }  // 0x6e4600
auto joan::orig::Panel::reset_6e4720() -> void { joan::bind::member<&::game_b94_08::H::panel_reset_6e4720>(this); }  // 0x6e4720
auto joan::orig::QuitPopup::show(std::uint32_t a0) -> void { joan::bind::member<&::game_b100_16::H::quit_show>(this, a0); }  // 0x6e4c00
auto joan::orig::QuitPopup::hide() -> void { joan::bind::member<&::game_b100_16::H::quit_hide>(this); }  // 0x6e4cc0
auto joan::orig::QuitPopup::flag_be() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_06::H::flag_be>(this)); }  // 0x6e4d30
auto joan::orig::QuitPopup::load() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::quit_load>(this)); }  // 0x6e4d60
auto joan::orig::QuitPopup::unload() -> void { joan::bind::member<&::game_b93_06::H::free1>(this); }  // 0x6e4dd0
