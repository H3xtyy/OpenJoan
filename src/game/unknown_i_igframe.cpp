// game/unknown_i_igframe.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_igframe.cpp;
// re/attribution.csv: between ww/interfac/i_work.cpp and ww/interfac/list.cpp). They move to their original file when
// evidence names it (Phase 3).
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

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;


using i16 = std::int16_t;

using joan::img::obj;

using joan::img::str;


using joan::hook::global;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_ctor_6f1660, "Obj::ctor_6f1660");

struct H {
    // ------------------------------------------------------------------ 0x006f1660
    H* ctor_6f1660() {
        c_ctor_6f1660.hit();
        static jo::Image2D jo::SlideMenu::* const kSliders[12] = {&jo::SlideMenu::slider0, &jo::SlideMenu::slider1, &jo::SlideMenu::slider2, &jo::SlideMenu::slider3, &jo::SlideMenu::slider4, &jo::SlideMenu::slider5, &jo::SlideMenu::slider6, &jo::SlideMenu::slider7, &jo::SlideMenu::slider8, &jo::SlideMenu::slider9, &jo::SlideMenu::slider10, &jo::SlideMenu::slider11};
        for (i32 i = 0; i < 12; ++i) joan::orig::as_image2d(&(jo::self<jo::SlideMenu>(this)->*kSliders[i]))->construct();  // 0x00629400
        crt_memset(this, 0, static_cast<u32>(reinterpret_cast<u8*>(&jo::self<jo::SlideMenu>(this)->slider0) - reinterpret_cast<u8*>(this)));  // the fields before slider0 (0x30 bytes on i686)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::ctor_6f1660", 0x006f1660, &H::ctor_6f1660);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

JOAN_COUNTER(c_dtor6f17e0, "Obj6f17e0::~dtor");

struct H {
    // ------------------------------------------------------------------ 0x006f17e0
    u32 dtor_6f17e0() {
        c_dtor6f17e0.hit();
        joan::orig::as_ingame_frame(this)->release();  // 0x006f1f00
        static jo::Image2D jo::SlideMenu::* const kSliders[11] = {&jo::SlideMenu::slider11, &jo::SlideMenu::slider10, &jo::SlideMenu::slider9, &jo::SlideMenu::slider8, &jo::SlideMenu::slider7, &jo::SlideMenu::slider6, &jo::SlideMenu::slider5, &jo::SlideMenu::slider4, &jo::SlideMenu::slider3, &jo::SlideMenu::slider2, &jo::SlideMenu::slider1};
        for (i32 k = 0; k < 11; ++k) joan::orig::as_widget(&(jo::self<jo::SlideMenu>(this)->*kSliders[k]))->dtor_6294c0();  // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider0)->dtor_6294c0();                         // 0x006294c0
        return 0;  // ~Widget returns nothing; only called from the atexit stub 0x0056d080 (result unused)
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj6f17e0::~dtor", 0x006f17e0, &H::dtor_6f17e0);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b104_14.cpp (game/b104_14)
namespace {
namespace game_b104_14 {

constexpr u32 kM1 = 0xffffffffu;
constexpr u32 kOne = 0x3f800000u;  // 1.0f

u32 colour1(u32 v) {
    u32 storage = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&storage)->Color_dword(joan::orig::arg::u32(v)));  // 0x005e9640
}
// bitmap item 0x00629660 (19 args, ret 0x4c), common tail 3,0,2,0,-1,-1,-1,-1,0,-1,-1,0,1,1
u32 bitmap(void* item, u32 x, u32 y, void* font, img::Addr name, u32 col) {
    return joan::orig::arg::as<u32>(joan::orig::as_label(item)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), font, joan::orig::arg::str(img::addr(name)), joan::orig::arg::i32(col), 3, 0, 2, 0, joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), 0, joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), 0, 1, 1));  // 0x00629660
}
// text item 0x00629530 (15 args, ret 0x3c), common tail 0,0,2,0,1f,1f,0,1,1
u32 text(void* item, u32 x, u32 y, img::Addr font, const char* str, u32 a5, u32 col) {
    return joan::orig::arg::as<u32>(joan::orig::as_image2d(item)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::as_font2d(img::obj(font)), str, joan::orig::arg::i32(a5), joan::orig::arg::i32(col), 0, 0, 2, 0, joan::orig::arg::i32(kOne), joan::orig::arg::i32(kOne), 0, 1, 1));  // 0x00629530
}
void place(void* item, u32 a, u32 b, u32 c, u32 d, u32 e) {
    joan::orig::as_widget(item)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
}

JOAN_COUNTER(c_frame, "Frame::init_6f1940");

struct H {
    // ------------------------------------------------------------------ 0x006f1940
    u32 init_6f1940() {
        c_frame.hit();
        struct Bm {
            jo::Image2D jo::SlideMenu::* slider;
            u32 x, y;
            img::Addr name;
        };
        static const Bm kBm[11] = {
            {&jo::SlideMenu::slider0, 0xfffffe00u, 0, d::s_left},   // 'left'
            {&jo::SlideMenu::slider1, 0x400u, 0, d::s_right},        // 'right'
            {&jo::SlideMenu::slider2, 0, 0xffffff48u, d::s_top},  // 'top'
            {&jo::SlideMenu::slider3, 0, 0x300u, d::s_bottom},       // 'bottom'
            {&jo::SlideMenu::slider4, 0, 0, d::s_woodtex},            // 'woodtex'
            {&jo::SlideMenu::slider8, 0x28u, 0x18u, d::s_sphere},    // 'sphere'
            {&jo::SlideMenu::slider9, 0, 0, d::s_ring},            // 'ring'
            {&jo::SlideMenu::slider5, 0, 0, d::s_titlebar},            // 'titlebar'
            {&jo::SlideMenu::slider6, 0x138u, 0x30u, d::s_title},   // 'title'
            {&jo::SlideMenu::slider10, 0, 0, d::s_l_arrow},            // 'l_arrow'
            {&jo::SlideMenu::slider11, 0, 0, d::s_r_arrow},            // 'r_arrow'
        };
        for (i32 i = 0; i < 11; ++i) {
            const u32 c = colour1(kM1);
            bitmap(&(jo::self<jo::SlideMenu>(this)->*kBm[i].slider), kBm[i].x, kBm[i].y, img::obj(d::g_sprites_a460d8), kBm[i].name, c);
        }
        const u32 c = colour1(kM1);
        text(&jo::self<jo::SlideMenu>(this)->slider7, 0x138u, 0x32u, d::g_font_goudy14_22_white, img::str(d::kEmpty), 1, c);
        u8* const bm = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(obj(d::g_sprites_a460d8))->find(joan::orig::arg::str(img::addr(d::s_title))));  // 0x00620b40 'title'
        const i32 x = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(bm)->width)) + 0x138;
        joan::orig::as_label(&jo::self<jo::SlideMenu>(this)->slider7)->set_layout(joan::orig::arg::u32(x), 0x44u, 2u, 1u, 1u, 0u, joan::orig::arg::u32(kM1));  // 0x006299b0
        static jo::Image2D jo::SlideMenu::* const kPlace[8] = {&jo::SlideMenu::slider4, &jo::SlideMenu::slider8, &jo::SlideMenu::slider9, &jo::SlideMenu::slider5, &jo::SlideMenu::slider6, &jo::SlideMenu::slider7, &jo::SlideMenu::slider10, &jo::SlideMenu::slider11};
        for (i32 i = 0; i < 8; ++i) place(&(jo::self<jo::SlideMenu>(this)->*kPlace[i]), 0, 0, 0, 1, 0);
        return 0;  // Widget::scale_to returns nothing; callers (0x0046fa0d, Game run) discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_14", name, addr, fn)
B("Frame::init_6f1940", 0x006f1940, &H::init_6f1940);
#undef B

}  // namespace game_b104_14
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

struct H {
    void reset_6f1f00() {  // 0x006f1f00
        joan::orig::as_ingame_frame(this)->free_backgrounds();  // 0x006f20f0
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider0)->release();  // 0x00629830
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider1)->release();
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider2)->release();
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider3)->release();
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider4)->release();
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider8)->release();
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider9)->release();
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider5)->release();
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider5)->release();  // twice in the original
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider7)->release();
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider10)->release();
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider11)->release();
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6f::reset", 0x006f1f00, &H::reset_6f1f00);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

void* const kWindows = obj(d::g_texture_array);

// Window-image load with the "current window" saved / restored (0x60a720 / 0x60a740).
u32 window_save() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kWindows)->mode()); }                // 0x0060a720
void window_set(u32 v) { joan::orig::as_texture_manager(kWindows)->set_mode_60a740(joan::orig::arg::u32(v)); }         // 0x0060a740
jo::Texture* window_load(const char* name) { return joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(kWindows)->load(joan::orig::arg::str(name), 1, 0u)); }  // 0x006099c0

JOAN_COUNTER(c_igamebg_load, "IGameBg::load");

struct H {
    // ------------------------------------------------------------------ 0x006f1ff0
    u32 igamebg_load() {
        c_igamebg_load.hit();
        const u32 saved = window_save();
        window_set(1);
        if (jo::self<jo::SlideMenu>(this)->background[0] == nullptr) jo::self<jo::SlideMenu>(this)->background[0] = window_load(str(d::s_igamebg1));  // "igamebg1"
        if (jo::self<jo::SlideMenu>(this)->background[1] == nullptr) jo::self<jo::SlideMenu>(this)->background[1] = window_load(str(d::s_igamebg2));  // "igamebg2"
        if (jo::self<jo::SlideMenu>(this)->background[2] == nullptr) jo::self<jo::SlideMenu>(this)->background[2] = window_load(str(d::s_igamebg3));  // "igamebg3"
        if (jo::self<jo::SlideMenu>(this)->background[3] == nullptr) jo::self<jo::SlideMenu>(this)->background[3] = window_load(str(d::s_igamebg4));  // "igamebg4"
        window_set(saved);
        return 0;  // Textures::set_mode returns nothing; callers (0x006f2505, SlideMenu::start) discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("IGameBg::load", 0x006f1ff0, &H::igamebg_load);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

void* const kTextures = obj(d::g_texture_array);
void* const kSprites = obj(d::g_sprites_a460d8);
void* const kFont = obj(d::g_font_goudy14_22_white);

char* strncpy_(void* d, const char* s, std::uint32_t n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }

void texture_release(void* t) { joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(t)); }  // 0x0060a550

JOAN_COUNTER(c_title, "Obj6f2::set_title");

struct H {
    void* self() { return this; }

    // 0x006f20f0
    void* release_6f20f0() {
        jo::IngameFrame* const fr = jo::self<jo::IngameFrame>(this);
        void** const slots[4] = {&fr->igamebg1, &fr->igamebg2, &fr->igamebg3, &fr->igamebg4};
        for (void** const slot : slots) {
            if (*slot != nullptr) {
                texture_release(*slot);
                *slot = nullptr;
            }
        }
        return self();
    }
    // 0x006f2ac0
    std::uintptr_t set_title(const char* s) {
        c_title.hit();
        strncpy_(jo::self<jo::SlideMenu>(this)->title, s, 0x1du);
        joan::orig::as_image2d(&jo::self<jo::SlideMenu>(this)->slider7)->set_name(joan::orig::arg::str(s));  // 0x006298c0
        if (strlen_(s) == 0) return 0;
        const std::int32_t width = static_cast<std::int16_t>(
            joan::orig::as_font(kFont)->measure(joan::orig::arg::str(s), -1, 0, 0, joan::orig::arg::f32(0x3f800000u)));  // 0x00624f00
        const std::int32_t title_w = jo::view<std::int16_t>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(kSprites)->find(str(d::s_title))))->width);  // 0x00620b40 'title'
        std::int32_t left = (title_w - width) / 2 + 0x138;
        left -= jo::view<std::int16_t>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(kSprites)->find(str(d::s_l_arrow))))->width);  // 0x00620b40 'l_arrow'
        left -= 2;
        const std::int32_t right = (title_w + width) / 2 + 0x13a;
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider10)->move_to(static_cast<std::int16_t>(left), 0x36, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061e020
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider11)->move_to_61e020(joan::orig::arg::i16(static_cast<std::int16_t>(right)), joan::orig::arg::i16(0x36), joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x3f800000u), static_cast<char>(joan::orig::arg::i32(1)), 0u));  // 0x0061e020
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj6f2::release", 0x006f20f0, &H::release_6f20f0);
B91("Obj6f2::set_title", 0x006f2ac0, &H::set_title);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {

JOAN_COUNTER(c_menu_reset, "Menu::reset_fades");
JOAN_COUNTER(c_menu_slide, "Menu::slide_out");

struct H {
    // 0x006f21c0 Menu::reset_fades(mode): 0 all, 1 first group, 2 second group.
    std::uintptr_t menu_reset(std::int8_t mode) {
        c_menu_reset.hit();
        auto fade = [](jo::Widget& slider, std::int32_t a) {
            return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_label(&slider)->draw(joan::orig::arg::u32(a), 7u));  // 0x00629bd0
        };
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::SlideMenu>(this)->open)) == 0) return reinterpret_cast<std::uintptr_t>(this);
        std::uintptr_t r;
        if (mode == 0 || mode == 1) {
            fade(jo::self<jo::SlideMenu>(this)->slider4, 0xa);
            r = fade(jo::self<jo::SlideMenu>(this)->slider8, 5);
        } else {
            r = static_cast<std::uintptr_t>(static_cast<std::int32_t>(mode));  // eax = movsx mode
        }
        if (mode != 0 && mode != 2) return r;
        fade(jo::self<jo::SlideMenu>(this)->slider9, 5);
        fade(jo::self<jo::SlideMenu>(this)->slider0, 5);
        fade(jo::self<jo::SlideMenu>(this)->slider1, 5);
        fade(jo::self<jo::SlideMenu>(this)->slider5, 1);
        if (joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(jo::self<jo::SlideMenu>(this)->title))) != 0) {  // strlen
            fade(jo::self<jo::SlideMenu>(this)->slider6, 1);
            fade(jo::self<jo::SlideMenu>(this)->slider7, 1);
            fade(jo::self<jo::SlideMenu>(this)->slider10, 1);
            fade(jo::self<jo::SlideMenu>(this)->slider11, 1);
        }
        fade(jo::self<jo::SlideMenu>(this)->slider2, 1);
        return fade(jo::self<jo::SlideMenu>(this)->slider3, 1);
    }
    // 0x006f26e0 Menu::slide_out.
    std::uint32_t menu_slide_out() {
        c_menu_slide.hit();
        jo::self<jo::SlideMenu>(this)->sliding = 3;
        auto slide = [](jo::Widget& slider, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
            joan::orig::as_widget(&slider)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), 0.0f, 0, 0x64u);  // 0x0061e020
        };
        auto fade = [](jo::Widget& slider, std::uint32_t speed, std::uint32_t last) {
            joan::orig::as_widget(&slider)->scale_to(0, joan::orig::arg::f32(speed), 0.0f, 0, joan::orig::arg::u32(last));  // 0x0061f1b0
        };
        slide(jo::self<jo::SlideMenu>(this)->slider0, 0xfffffe00u, 0u, 0x45a00000u);
        slide(jo::self<jo::SlideMenu>(this)->slider1, 0x400u, 0u, 0x45a00000u);
        slide(jo::self<jo::SlideMenu>(this)->slider2, 0u, 0xffffff48u, 0x44660000u);
        slide(jo::self<jo::SlideMenu>(this)->slider3, 0u, 0x300u, 0x43a00000u);
        fade(jo::self<jo::SlideMenu>(this)->slider4, 0x40a00000u, 0x64u);
        fade(jo::self<jo::SlideMenu>(this)->slider8, 0x40a00000u, 0x64u);
        fade(jo::self<jo::SlideMenu>(this)->slider9, 0x40a00000u, 0x64u);
        fade(jo::self<jo::SlideMenu>(this)->slider5, 0x41200000u, 0u);
        fade(jo::self<jo::SlideMenu>(this)->slider6, 0x41200000u, 0u);
        fade(jo::self<jo::SlideMenu>(this)->slider7, 0x41200000u, 0u);
        fade(jo::self<jo::SlideMenu>(this)->slider10, 0x41200000u, 0u);
        fade(jo::self<jo::SlideMenu>(this)->slider11, 0x41200000u, 0u);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_sound(img::obj(d::g_sfx_player))->play_def(&img::global<const char>(d::s_MENU_SLIDE_OUT)));  // 0x004ceef0 "MENU_SLIDE_OUT"
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Menu::reset_fades", 0x006f21c0, &H::menu_reset);
B("Menu::slide_out", 0x006f26e0, &H::menu_slide_out);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

JOAN_COUNTER(c_panel_update, "Panel::update_6f2340");

bool eq_or_unordered(float a, float b) { return !(a < b) && !(a > b); }

struct H {
    // 0x006f2340
    void update_6f2340() {
        if (jo::view<i8>(jo::self<jo::SlideMenu>(this)->sliding) == 0 || jo::view<i8>(jo::self<jo::SlideMenu>(this)->sliding) == 2) return;
        c_panel_update.hit();
        static jo::Image2D jo::SlideMenu::* const kChildren[12] = {&jo::SlideMenu::slider0, &jo::SlideMenu::slider1, &jo::SlideMenu::slider2, &jo::SlideMenu::slider3, &jo::SlideMenu::slider4, &jo::SlideMenu::slider8, &jo::SlideMenu::slider9, &jo::SlideMenu::slider5, &jo::SlideMenu::slider6, &jo::SlideMenu::slider7, &jo::SlideMenu::slider10, &jo::SlideMenu::slider11};
        for (i32 k = 0; k < 12; ++k) joan::orig::as_widget(&(jo::self<jo::SlideMenu>(this)->*kChildren[k]))->update();  // 0x0061f3b0
        if (jo::view<i8>(jo::self<jo::SlideMenu>(this)->sliding) == 1) {
            if (eq_or_unordered(jo::self<jo::SlideMenu>(this)->slider5.scale /* x86 +0x3d4 */, global<float>(d::flt_1))) {
                jo::self<jo::SlideMenu>(this)->sliding = 2;
                joan::orig::as_shop(obj(d::g_game_a5d380))->leave();  // 0x006cdfa0
            }
        } else if (jo::view<i8>(jo::self<jo::SlideMenu>(this)->sliding) == 3) {
            if (eq_or_unordered(jo::self<jo::SlideMenu>(this)->slider4.scale /* x86 +0x31d */, global<float>(d::k_9cf5c4))) {
                jo::self<jo::SlideMenu>(this)->sliding = 0;
                jo::self<jo::SlideMenu>(this)->open = 0;
                joan::orig::as_ingame_frame(this)->free_backgrounds();  // 0x006f20f0
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Panel::update_6f2340", 0x006f2340, &H::update_6f2340);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b101_15.cpp (game/b101_15)
namespace {
namespace game_b101_15 {

void button_anim(void* b, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    joan::orig::as_widget(b)->scale_to(joan::orig::arg::i16(a0), joan::orig::arg::f32(a1), joan::orig::arg::f32(a2), static_cast<char>(a3), joan::orig::arg::u32(a4));  // 0x0061f1b0
}
void button_move(void* b, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    joan::orig::as_widget(b)->move_to(joan::orig::arg::i16(a0), joan::orig::arg::i16(a1), joan::orig::arg::f32(a2), joan::orig::arg::f32(a3), static_cast<char>(a4), joan::orig::arg::u32(a5));  // 0x0061e020
}

JOAN_COUNTER(c_slide_start, "SlideMenu::start");

struct H {
    // 0x006f24d0 (ret)
    u32 slide_start() {
        c_slide_start.hit();
        if (static_cast<i32>(jo::self<jo::SlideMenu>(this)->open) == 0) joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_MENU_SLIDE_IN));  // 0x004ceef0 MENU_SLIDE_IN
        jo::self<jo::SlideMenu>(this)->sliding = 1;
        jo::view<u8>(jo::self<jo::SlideMenu>(this)->open) = 1;
        joan::orig::as_ingame_frame(this)->load_backgrounds();  // 0x006f1ff0
        button_move(&jo::self<jo::SlideMenu>(this)->slider0, 0u, 0u, 0x45a00000u, 0u, 0u, 0x64u);
        button_move(&jo::self<jo::SlideMenu>(this)->slider1, 0x200u, 0u, 0x45a00000u, 0u, 0u, 0x64u);
        button_move(&jo::self<jo::SlideMenu>(this)->slider2, 0u, 0u, 0x44660000u, 0u, 0u, 0u);
        button_move(&jo::self<jo::SlideMenu>(this)->slider3, 0u, 0x2c0u, 0x43a00000u, 0u, 0u, 0u);
        button_anim(&jo::self<jo::SlideMenu>(this)->slider4, 1u, 0x40a00000u, 0u, 0u, 0u);
        button_anim(&jo::self<jo::SlideMenu>(this)->slider8, 1u, 0x40a00000u, 0u, 0u, 0u);
        button_anim(&jo::self<jo::SlideMenu>(this)->slider9, 1u, 0x40a00000u, 0u, 0u, 0u);
        button_anim(&jo::self<jo::SlideMenu>(this)->slider5, 1u, 0x41200000u, 0u, 0u, 0xc8u);
        button_anim(&jo::self<jo::SlideMenu>(this)->slider6, 1u, 0x41200000u, 0u, 0u, 0xc8u);
        button_anim(&jo::self<jo::SlideMenu>(this)->slider7, 1u, 0x41200000u, 0u, 0u, 0xc8u);
        button_anim(&jo::self<jo::SlideMenu>(this)->slider10, 1u, 0x41200000u, 0u, 0u, 0xc8u);
        button_anim(&jo::self<jo::SlideMenu>(this)->slider11, 1u, 0x41200000u, 0u, 0u, 0xc8u);
        return 0;  // Widget::scale_to returns nothing; every caller of 0x006f24d0 discards the result
    }
};

#define B15(name, addr, fn) JOAN_REPLACE("game/b101_15", name, addr, fn)
B15("SlideMenu::start", 0x006f24d0, &H::slide_start);
#undef B15

}  // namespace game_b101_15
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

JOAN_COUNTER(c_sliders, "Hud::init_sliders");

struct H {
    void init_sliders() {  // 0x006f28c0
        c_sliders.hit();
        jo::self<jo::SlideMenu>(this)->sliding = 0;
        jo::view<u8>(jo::self<jo::SlideMenu>(this)->open) = 0;
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider0)->move_to(-512, 0, joan::orig::arg::f32(0x45a00000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061e020
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider1)->move_to(0x400, 0, joan::orig::arg::f32(0x45a00000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061e020
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider2)->move_to(0, -184, joan::orig::arg::f32(0x44660000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061e020
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider3)->move_to(0, 0x300, joan::orig::arg::f32(0x43a00000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061e020
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider4)->scale_to(0, joan::orig::arg::f32(0x40a00000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061f1b0
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider8)->scale_to(0, joan::orig::arg::f32(0x40a00000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061f1b0
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider9)->scale_to(0, joan::orig::arg::f32(0x40a00000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061f1b0
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider5)->scale_to(0, joan::orig::arg::f32(0x41200000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061f1b0
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider6)->scale_to(0, joan::orig::arg::f32(0x41200000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061f1b0
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider7)->scale_to(0, joan::orig::arg::f32(0x41200000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061f1b0
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider10)->scale_to(0, joan::orig::arg::f32(0x41200000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061f1b0
        joan::orig::as_widget(&jo::self<jo::SlideMenu>(this)->slider11)->scale_to(0, joan::orig::arg::f32(0x41200000u), joan::orig::arg::f32(0x3f800000u), 1, 0u);  // 0x0061f1b0
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Hud::init_sliders", 0x006f28c0, &H::init_sliders);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::IngameFrame::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_10::H::ctor_6f1660>(this)); }  // 0x6f1660
auto joan::orig::IngameFrame::release() -> void { joan::bind::member<&::game_b93_06::H::reset_6f1f00>(this); }  // 0x6f1f00
auto joan::orig::IngameFrame::load_backgrounds() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_16::H::igamebg_load>(this)); }  // 0x6f1ff0
auto joan::orig::IngameFrame::free_backgrounds() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::release_6f20f0>(this)); }  // 0x6f20f0
auto joan::orig::Menu::reset_fades(std::int8_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06::H::menu_reset>(this, a0)); }  // 0x6f21c0
auto joan::orig::Panel::update_6f2340() -> void { joan::bind::member<&::game_b108_02::H::update_6f2340>(this); }  // 0x6f2340
auto joan::orig::IngameFrame::slide_in() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_15::H::slide_start>(this)); }  // 0x6f24d0
auto joan::orig::Menu::slide_out() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06::H::menu_slide_out>(this)); }  // 0x6f26e0
auto joan::orig::Hud::init_sliders() -> void { joan::bind::member<&::game_b94_08::H::init_sliders>(this); }  // 0x6f28c0
auto joan::orig::IngameFrame::set_title(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_04::H::set_title>(this, a0)); }  // 0x6f2ac0
