// game/unknown_armory.cpp: functions whose original source file is unknown (formerly game/_unplaced/armory.cpp;
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

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;
using i16 = std::int16_t;
using u16 = std::uint16_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

constexpr u32 kOne = 0x3f800000u;  // 1.0f

using i8 = std::int8_t;

using joan::hook::x87::ftol;

}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using joan::orig::helpers::memset_;  // memset

using joan::orig::helpers::widget_ctor;  // 0x00620ee0
void text_ctor(void* p) { joan::orig::as_image2d(p)->construct(); }    // 0x00629400

JOAN_COUNTER(c_ctor_6b94e0, "Screen::ctor_6b94e0");

struct H {
    // ------------------------------------------------------------------ 0x006b94e0 (ItemScreen)
    void* ctor_6b94e0() {
        c_ctor_6b94e0.hit();
        text_ctor(&jo::self<jo::ItemScreen>(this)->label0);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label1);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label2);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label3);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label4);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label5);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label6);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label7);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label8);
        widget_ctor(&jo::self<jo::ItemScreen>(this)->button);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label9);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label10);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label11);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label12);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label13);
        text_ctor(&jo::self<jo::ItemScreen>(this)->label14);
        memset_(this, 0, static_cast<u32>(reinterpret_cast<u8*>(&jo::self<jo::ItemScreen>(this)->label0) - reinterpret_cast<u8*>(this)));  // the head before label0: 0x28 on x86
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen::ctor_6b94e0", 0x006b94e0, &H::ctor_6b94e0);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {


JOAN_COUNTER(c_item_dtor, "ItemScreen::dtor");

struct H {
    void item_dtor() {  // 0x006b96b0
        c_item_dtor.hit();
        jo::ItemScreen* const self = jo::self<jo::ItemScreen>(this);
        joan::orig::as_armory_screen(this)->release();  // 0x006ba3e0
        joan::orig::as_widget(&self->label14)->dtor_6294c0();  // 0x006294c0
        joan::orig::as_widget(&self->label13)->dtor_6294c0();
        joan::orig::as_widget(&self->label12)->dtor_6294c0();
        joan::orig::as_widget(&self->label11)->dtor_6294c0();
        joan::orig::as_widget(&self->label10)->dtor_6294c0();
        joan::orig::as_widget(&self->label9)->dtor_6294c0();
        joan::orig::as_widget(&self->button)->dtor_6211a0();  // 0x006211a0
        joan::orig::as_widget(&self->label8)->dtor_6294c0();
        joan::orig::as_widget(&self->label7)->dtor_6294c0();
        joan::orig::as_widget(&self->label6)->dtor_6294c0();
        joan::orig::as_widget(&self->label5)->dtor_6294c0();
        joan::orig::as_widget(&self->label4)->dtor_6294c0();
        joan::orig::as_widget(&self->label3)->dtor_6294c0();
        joan::orig::as_widget(&self->label2)->dtor_6294c0();
        joan::orig::as_widget(&self->label1)->dtor_6294c0();
        joan::orig::as_widget(&self->label0)->dtor_6294c0();
    }

    u8 byte_0() { return jo::self<jo::ItemScreen>(this)->open; }      // 0x006ba890 / 0x006e6810
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("ItemScreen::dtor", 0x006b96b0, &H::item_dtor);
B94("X::byte_0_6ba890", 0x006ba890, &H::byte_0);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b116_13.cpp (game/b116_13)
namespace {
namespace game_b116_13 {


constexpr u32 kSix = 0x40c00000u;   // 6.0f
constexpr u32 kFarX = 0x455ac000u;  // 3500.0f
constexpr u32 kFarY = 0xc4bb8000u;  // -1500.0f

void* const kMouse = obj(d::g_mouse);
void* const kText = obj(d::g_strings);
void* const kImages = obj(d::g_bmp_res_armory);

using joan::orig::helpers::game_new;  // operator new

// Colour record 0x0042a260(r, g, b, a) on a 4-byte local; returns its dword.
u32 colour(i32 r, i32 g, i32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260
    return jo::view<u32>(*reinterpret_cast<const jo::Colour*>(p));  // the four bytes as one dword
}

// Image size record of `name` (0x00620b40 on a resource set).
u8* image(void* set, const void* name) { return joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(set)->find(joan::orig::arg::str(name))); }  // 0x00620b40

// 0x00629660 image TextDesc with the standard trailing args (3, 0, 2, 0, -1 x4, 0, -1, -1, 0, 1, 1).
void image_desc(void* d, i32 x, i32 y, img::Addr name, u32 c) {
    joan::orig::as_label(d)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::mptr(kImages), joan::orig::arg::str(str(name)), joan::orig::arg::i32(c), 3, 0, 2, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(// 0x00629660
                  -1), 0, 1, 1);
}
// 0x00629530 text TextDesc with the standard trailing args (0, 0, 2, 0, 1.0f, 1.0f, 0, 1, 1).
void text_desc(void* d, i32 x, i32 y, img::Addr font, const char* text, i32 a5, u32 c) {
    joan::orig::as_image2d(d)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), static_cast<jo::Font2d*>(img::obj(font)), text, joan::orig::arg::i32(a5), joan::orig::arg::i32(c), 0, 0, 2, 0, joan::orig::arg::i32(kOne), joan::orig::arg::i32(kOne), 0, 1, 1);  // 0x00629530
}
void text_box(void* d, i32 x, i32 y) {
    joan::orig::as_label(d)->set_layout(joan::orig::arg::u32(x), joan::orig::arg::u32(y), 2u, 1u, 1u, 0u, joan::orig::arg::u32(-1));  // 0x006299b0
}
void anim(void* d, i32 a, u32 speed, i32 c, i32 e, i32 f) {
    joan::orig::as_widget(d)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(speed), joan::orig::arg::f32(c), static_cast<char>(e), joan::orig::arg::u32(f));  // 0x0061f1b0
}
void slide(void* d, i32 x, i32 y, u32 a, u32 b, i32 flag, i32 delay) {
    joan::orig::as_widget(d)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(a), joan::orig::arg::f32(b), static_cast<char>(flag), joan::orig::arg::u32(delay));  // 0x0061e020
}

JOAN_COUNTER(c_setup, "ItemScreen::setup_6b9870");
JOAN_COUNTER(c_open, "ItemScreen::open_6ba4b0");
JOAN_COUNTER(c_hide, "ItemScreen::hide_6bb160");
JOAN_COUNTER(c_close, "ItemScreen::close_6bb420");

struct H {
    // ------------------------------------------------------------------ 0x006b9870
    std::uintptr_t setup_6b9870() {
        c_setup.hit();
        image_desc(&jo::self<jo::ItemScreen>(this)->label1, 0x1c, 0xf8, d::s_frame, colour(0, 0, 0, 0xff));  // "frame"
        image_desc(&jo::self<jo::ItemScreen>(this)->label0, 0x1c, 0xf8, d::s_frame_bg, colour(0, 0, 0, 0xff));  // "frame_bg"

        const i16 title_w = jo::view<i16>(jo::as_bmp_entry(image(kImages, str(d::s_title)))->width);  // "title"
        const i32 w2 = jo::view<i16>(jo::as_bmp_entry(image(kImages, str(d::s_title)))->width);
        const i16 title_x = static_cast<i16>((joan::canvas::size_read(0x006b9969) - w2) / 2);
        const char* const caption = joan::orig::arg::as<const char*>(joan::orig::as_menu(kText)->armory_1());  // 0x004de3b0
        const i16 text_w =
            static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(caption, -1, 0, 0, joan::orig::arg::f32(kOne))));  // 0x00624f00
        const i16 text_x = static_cast<i16>(static_cast<i32>(title_x) + (static_cast<i32>(title_w) - text_w) / 2);

        image_desc(&jo::self<jo::ItemScreen>(this)->label2, title_x, 7, d::s_title, colour(0, 0, 0, 0xff));  // "title"
        {
            const u32 c = colour(0, 0, 0, 0xff);
            const char* const t = joan::orig::as_menu(kText)->armory_1();  // 0x004de3b0
            text_desc(&jo::self<jo::ItemScreen>(this)->label3, text_x, 0xb, d::g_font_goudy14_22_white, t, 0, c);
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            const i32 x = static_cast<i32>(text_x) - 2 - jo::view<i16>(jo::as_bmp_entry(image(kImages, str(d::s_l_arrow)))->width);
            image_desc(&jo::self<jo::ItemScreen>(this)->label4, x, 0xd, d::s_l_arrow, c);  // "l_arrow"
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            image_desc(&jo::self<jo::ItemScreen>(this)->label5, static_cast<i32>(text_x) + text_w + 2, 0xd, d::s_r_arrow, c);  // "r_arrow"
        }
        image_desc(&jo::self<jo::ItemScreen>(this)->label6, 0, 0, d::s_bg1, colour(0, 0, 0, 0xff));      // "bg1"
        image_desc(&jo::self<jo::ItemScreen>(this)->label7, 0, 0, d::s_bg2, colour(0, 0, 0, 0xff));      // "bg2"
        image_desc(&jo::self<jo::ItemScreen>(this)->label8, 0, 0x2c8, d::s_LONGBAR, colour(0, 0, 0, 0xff));  // "LONGBAR"

        {
            const i32 x2 = (joan::canvas::size_read(0x006b9c4e) - 0xfa) / 2 + 0xf9;
            const i32 x1 = (joan::canvas::size_read(0x006b9c68) - 0xfa) / 2;
            joan::orig::as_button(&jo::self<jo::ItemScreen>(this)->button)->create(joan::orig::arg::i32(x1), 0x2c8, joan::orig::arg::i32(x2), 0x2e6, fn::code_value(fn::item_mark_draw_6bbcd0), 1, 1, 0, 1, 1, joan::orig::arg::i32(// 0x00621540
                          1));                                                                         // cb 0x006bbcd0
            const u32 c = colour(0xff, 0xff, 0xff, 0xff);
            const char* const t = joan::orig::as_menu(kText)->armory_3();                                         // 0x004de410
            joan::orig::as_button(&jo::self<jo::ItemScreen>(this)->button)->set_text(t, img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(c), 1, 2, 0, 2, 2);         // 0x00623ce0
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            const char* const t = joan::orig::as_txt_res(kText)->armory_text_2();  // 0x004de3e0
            text_desc(&jo::self<jo::ItemScreen>(this)->label9, 0, 0x40, d::g_font_goudy_bold_18, t, 1, c);
            text_box(&jo::self<jo::ItemScreen>(this)->label9, joan::canvas::size_read(0x006b9d49) - 1, 0x40);
        }
        text_desc(&jo::self<jo::ItemScreen>(this)->label10, 0, 0x64, d::g_font_goudy_bold_48, img::str(d::s_sp_9d05f0), 4, colour(0, 0, 0, 0xff));
        text_box(&jo::self<jo::ItemScreen>(this)->label10, joan::canvas::size_read(0x006b9dca) - 1, 0xe6);
        text_desc(&jo::self<jo::ItemScreen>(this)->label11, 0x34, 0x124, d::g_font_goudy_20, img::str(d::s_sp_9d05f0), 4, colour(0, 0, 0, 0xff));
        text_box(&jo::self<jo::ItemScreen>(this)->label11, 0x1cb, 0x1c3);
        text_desc(&jo::self<jo::ItemScreen>(this)->label12, 0x34, 0x1d0, d::g_font_goudy_18, img::str(d::s_sp_9d05f0), 4, colour(0, 0, 0, 0xff));
        text_box(&jo::self<jo::ItemScreen>(this)->label12, 0x1cb, 0x253);
        image_desc(&jo::self<jo::ItemScreen>(this)->label13, 0x9c, 0xec, d::s_fancybar, colour(0, 0, 0, 0xff));  // "fancybar"
        {
            const u32 c = colour(0, 0, 0, 0xff);
            const char* const t = joan::orig::as_txt_res(kText)->armory_text_4();  // 0x004de440
            text_desc(&jo::self<jo::ItemScreen>(this)->label14, 0x9c, 0xec, d::g_font_goudy12_bold_black, t, 1, c);
            text_box(&jo::self<jo::ItemScreen>(this)->label14, 0x164, 0x110);
        }

        {
            void* const m = game_new(static_cast<u32>(sizeof(jo::Scene)));  // 0xd5 on x86
            jo::view<void*>(jo::self<jo::ItemScreen>(this)->scene) = m != nullptr ? joan::orig::as_scene(m)->construct() : nullptr;  // 0x005e9a00
        }
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::ItemScreen>(this)->scene))->reset();  // 0x005e9ac0
        {
            void* const m = game_new(static_cast<u32>(sizeof(jo::Camera)));  // 0x4ca on x86
            jo::view<void*>(jo::self<jo::ItemScreen>(this)->camera) = m != nullptr ? joan::orig::as_camera(m)->construct() : nullptr;  // 0x00613650
        }
        joan::orig::as_sys_en(jo::view<void*>(jo::self<jo::ItemScreen>(this)->camera))->init_613730();  // 0x00613730
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::ItemScreen>(this)->camera))->set_viewport(static_cast<jo::Surface*>(img::obj(d::g_vga_back)), img::obj(d::g_a56ef0), 0x200u, 0xc0u, 0x3dfu, joan::orig::arg::u32(// 0x00613bf0
                      0x29f));
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::ItemScreen>(this)->camera))->set_cf(0u, 1u, joan::orig::arg::u32(global<u32>(d::g_init_black)));  // 0x00613d30
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::ItemScreen>(this)->camera))->set_ranges(joan::orig::arg::f32(kOne), joan::orig::arg::f32(0x46000400u), joan::orig::arg::f32(0x46000400u));  // 0x00613ca0
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::ItemScreen>(this)->camera))->set_scene(jo::self<jo::ItemScreen>(this)->scene);                  // 0x00613db0
        u32 box[6] = {0xc61c4000u, 0xc61c4000u, 0xc61c4000u, 0x461c4000u, 0x461c4000u, 0x461c4000u};
        jo::self<jo::ItemScreen>(this)->shadow_scene = joan::orig::arg::as<jo::SceneS*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::ItemScreen>(this)->scene))->add_node());  // 0x005ea460
        joan::orig::as_scene_source(jo::view<void*>(jo::self<jo::ItemScreen>(this)->shadow_scene))->reset(joan::orig::arg::mptr(box));            // 0x005ef880
        u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::ItemScreen>(this)->scene))->add_light());   // 0x005eb5d0
        joan::orig::as_camera(r)->init(0u);                            // 0x005c58a0
        return joan::orig::arg::as<u32>(joan::orig::as_color(&jo::as_light(r)->colour)->set_bgr(joan::orig::arg::mptr(img::addr(d::g_init_white))));    // 0x005e9430
    }

    // ------------------------------------------------------------------ 0x006ba4b0
    H* open_6ba4b0() {
        c_open.hit();
        jo::self<jo::ItemScreen>(this)->open = 1;
        joan::orig::as_armory_screen(this)->slide_out();  // 0x006bb160
        joan::orig::as_armory_screen(this)->refresh();  // 0x006ba8b0
        joan::orig::as_cursor(kMouse)->reset_pos();  // 0x00592c30
        if (global<u8>(d::g_game_running) != 0) {
            global<u8>(d::g_game_running) = 0;
            global<u16>(d::g_saved_game_speed) = static_cast<u16>(ftol(static_cast<double>(global<float>(d::g_game_speed))));
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(0u);  // 0x0042f930
        }
        static jo::Label jo::ItemScreen::* const kAnim[8] = {&jo::ItemScreen::label6, &jo::ItemScreen::label7, &jo::ItemScreen::label2, &jo::ItemScreen::label3,
                                                             &jo::ItemScreen::label4, &jo::ItemScreen::label5, &jo::ItemScreen::label9, &jo::ItemScreen::label10};
        for (i32 i = 0; i < 8; ++i) anim(&(jo::self<jo::ItemScreen>(this)->*kAnim[i]), 1, kSix, 0, 0, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label0, 0x1c, 0xf8, kFarX, kFarY, 0, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label1, 0x1c, 0xf8, kFarX, kFarY, 0, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label11, 0x34, 0x124, kFarX, kFarY, 0, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label12, 0x34, 0x1d0, kFarX, kFarY, 0, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label13, 0x9c, 0xec, kFarX, kFarY, 0, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label14, 0x9c, 0xec, kFarX, kFarY, 0, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label8, 0, 0x2c8, kFarX, kFarY, 0, 0);
        slide(&jo::self<jo::ItemScreen>(this)->button, (joan::canvas::size_read(0x006ba6ef) - 0xfa) / 2, 0x2c8, kFarX, kFarY, 0, 0);
        if (jo::self<jo::ItemScreen>(this)->actor != nullptr) {
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::ItemScreen>(this)->actor))->set_faded(static_cast<char>(joan::orig::arg::i32(1)));                              // 0x005ae590
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ItemScreen>(this)->actor->plane))->set_fade(0x00ffffffu);  // 0x005d5e80
        }
        jo::self<jo::ItemScreen>(this)->u32_8 = global<u32>(d::g_game_time_ms);
        jo::self<jo::ItemScreen>(this)->shown = 1;
        return this;
    }

    // ------------------------------------------------------------------ 0x006bb160 / 0x006bb420
    void slide_out(i32 flag, std::uintptr_t size_read_at) {
        static jo::Label jo::ItemScreen::* const kAnim[8] = {&jo::ItemScreen::label6, &jo::ItemScreen::label7, &jo::ItemScreen::label2, &jo::ItemScreen::label3,
                                                             &jo::ItemScreen::label4, &jo::ItemScreen::label5, &jo::ItemScreen::label9, &jo::ItemScreen::label10};
        for (i32 i = 0; i < 8; ++i) anim(&(jo::self<jo::ItemScreen>(this)->*kAnim[i]), 0, kSix, 0, flag, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label0, -0x1d8, 0xf8, kFarX, kFarY, flag, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label1, -0x1d8, 0xf8, kFarX, kFarY, flag, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label11, -0x1c0, 0x124, kFarX, kFarY, flag, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label12, -0x1c0, 0x1d0, kFarX, kFarY, flag, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label13, -0x158, 0xec, kFarX, kFarY, flag, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label14, -0x158, 0xec, kFarX, kFarY, flag, 0);
        slide(&jo::self<jo::ItemScreen>(this)->label8, 0x400, 0x2c8, kFarX, kFarY, flag, 0);
        slide(&jo::self<jo::ItemScreen>(this)->button, (joan::canvas::size_read(size_read_at) - 0xfa) / 2 + 0x400, 0x2c8, kFarX, kFarY,
              flag, 0);
    }
    H* hide_6bb160() {
        c_hide.hit();
        slide_out(1, 0x006bb35e);
        jo::self<jo::ItemScreen>(this)->shown = 0;
        return this;
    }
    u32 close_6bb420() {
        c_close.hit();
        if (jo::view<i8>(jo::self<jo::ItemScreen>(this)->open) == 0) return 0;
        if (jo::view<i8>(jo::self<jo::ItemScreen>(this)->shown) == 2 && joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::ItemScreen>(this)->button)->mouse(0x1b, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {  // 0x006218c0
            slide_out(0, 0x006bb661);
            jo::self<jo::ItemScreen>(this)->u32_8 = global<u32>(d::g_game_time_ms);
            jo::self<jo::ItemScreen>(this)->shown = 3;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b116_13", name, addr, fn)
B("ItemScreen::setup_6b9870", 0x006b9870, &H::setup_6b9870);
B("ItemScreen::open_6ba4b0", 0x006ba4b0, &H::open_6ba4b0);
B("ItemScreen::hide_6bb160", 0x006bb160, &H::hide_6bb160);
B("ItemScreen::close_6bb420", 0x006bb420, &H::close_6bb420);
#undef B

}  // namespace game_b116_13
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {


JOAN_COUNTER(c_free_children, "Obj6b::free_children");
JOAN_COUNTER(c_init6b, "Obj6b::init");

struct H {
    void free_children() {  // 0x006ba3e0
        c_free_children.hit();
        joan::orig::as_armory_screen(this)->release_item_view();  // 0x006bb020
        jo::self<jo::ItemScreen>(this)->shadow_scene = nullptr;
        if (jo::view<void*>(jo::self<jo::ItemScreen>(this)->scene) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ItemScreen>(this)->scene);
            if (p != nullptr) joan::orig::as_scene(p)->destroy(1u);  // 0x004201f0
            jo::self<jo::ItemScreen>(this)->scene = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::ItemScreen>(this)->camera) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ItemScreen>(this)->camera);
            if (p != nullptr) joan::orig::as_camera(p)->destroy(1u);  // 0x00562910
            jo::self<jo::ItemScreen>(this)->camera = nullptr;
        }
    }

    void init_6ba810() {  // 0x006ba810
        c_init6b.hit();
        jo::self<jo::ItemScreen>(this)->open = 0;
        jo::self<jo::ItemScreen>(this)->shown = 0;
        joan::orig::as_armory_screen(this)->release_item_view();              // 0x006bb020
        joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();   // 0x0042fef0
        if (global<u8>(d::g_game_running) == 0) {
            global<u8>(d::g_game_running) = 1;
            const float v = static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)));  // exact
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(v));  // 0x0042f930
        }
    }

    void set_pair(i16 a, i16 b) {  // 0x006bbc90
        jo::self<jo::ItemScreen>(this)->item_id = a;
        jo::view<i32>(jo::self<jo::ItemScreen>(this)->u32_4) = static_cast<i32>(b);
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6b::free_children", 0x006ba3e0, &H::free_children);
B93("Obj6b::init", 0x006ba810, &H::init_6ba810);
B93("Obj6b::set_pair", 0x006bbc90, &H::set_pair);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b111_07.cpp (fix/uninit)
namespace {
namespace game_b111_07 {

[[maybe_unused]] void* to_ptr(u32 v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
u32 to_u32(const void* p) { return joan::call::pointer_low_dword(p); }  // frame word of FramePtr: x86 value only, x64 keeps the pointer in FramePtr::p

void* new_(u32 size) { return joan::crt::operator_new(joan::orig::arg::u32(size)); }  // operator new
using joan::orig::helpers::strlen_;  // strlen
void* new_obj(u32 size, std::uintptr_t ctor) {
    void* const mem = new_(size);
    return mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
}

JOAN_COUNTER(c_armory_refresh, "ArmoryPanel::refresh");

// ---------------------------------------------------------------------------------- 0x006ba8b0
// Original frame of 0x006ba8b0 from [ebp-0x2a0] to [ebp-0xc] (exclusive), plus padding.
constexpr i32 kArmoryPad = 0x1000;
struct alignas(4) ArmoryFrame {
    u8 bytes[0x2a0 - 0xc + kArmoryPad];
    u8* at_ebp(i32 off) { return bytes + (0x2a0 - off); }
    u32& dw(i32 off) { return *reinterpret_cast<u32*>(at_ebp(off)); }
};
// A pointer local the original keeps in this frame ([ebp-off]): on x86 the frame word is the variable (a format
// with missing arguments can read it); on x64 the pointer is kept here and the word gets its low 32 bits.
struct FramePtr {
    u32& slot;
#if UINTPTR_MAX != 0xffffffffu
    void* p = nullptr;
#endif
    void set(void* v) {
        slot = to_u32(v);
#if UINTPTR_MAX != 0xffffffffu
        p = v;
#endif
    }
    void* get() const {
#if UINTPTR_MAX == 0xffffffffu
        return to_ptr(slot);
#else
        return p;
#endif
    }
};
void set_text(jo::Label* label, const void* text) {
    joan::orig::as_image2d(label)->set_name(joan::orig::arg::str(text));  // 0x006298c0
}

struct H {
    // ------------------------------------------------------------------------------ 0x006ba8b0
    void armory_refresh() {
        c_armory_refresh.hit();
        ArmoryFrame fr;
        std::memset(&fr, 0, sizeof fr);  // fix/uninit: missing format arguments read 0
        FramePtr self_slot{fr.dw(0x10)};
        FramePtr item_slot{fr.dw(0x14)};
        char* const path = reinterpret_cast<char*>(fr.at_ebp(0x118));
#if UINTPTR_MAX == 0xffffffffu
        u8* const file = fr.at_ebp(0x228);
#else
        // x64: File is 0x112 bytes (0x10e on x86) and would reach path at ebp-0x118: kept outside the frame.
        alignas(8) u8 file_x64[sizeof(jo::File)];
        u8* const file = file_x64;
#endif
        FramePtr buf_slot{fr.dw(0x22c)};
        self_slot.set(this);
        auto self = [&]() { return self_slot.get(); };
        auto item = [&]() { return item_slot.get(); };

        static const img::Addr kNames[3] = {d::s_armory_f, d::s_armory_b, d::s_barr_bg};  // armory_f / _b / barr_bg
        for (i32 k = 0; k < 3; ++k) {
            jo::Texture*& image = (&jo::as_item_screen(self())->image_a)[k];  // image_a / image_b / image_c
            if (image == nullptr) {
                image =
                    joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(str(kNames[k])), joan::orig::arg::i32(u32{1}), u32{0}));  // 0x006099c0
            }
        }
        item_slot.set(joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_item_screen(self())->item_id))))));  // 0x00425ce0
        set_text(&jo::as_item_screen(self())->label10, jo::as_item_def(item())->file_name);
        if (strlen_(jo::as_item_def(item())->str_25f) != 0) {
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(path)), str(d::s_ITEM_DES));           // strcpy "ITEM_DES\"
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(path)), joan::orig::arg::str(jo::as_item_def(item())->str_25f));  // strcat
            if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(path)) != 0) {  // 0x00590b80
                joan::orig::as_file(file)->construct();                                              // 0x004179c0
                joan::orig::as_file(file)->file_open(joan::orig::arg::str(path), joan::orig::arg::i32(u32{1}), static_cast<char>(u32{0}));  // 0x00586da0
                const u32 size = joan::orig::arg::as<u32>(joan::orig::as_file(file)->file_size());      // 0x00587f30
                buf_slot.set(new_(size + 1u));  // also [ebp-0x2a4] (below the copied frame, never read)
                const u32 n = joan::orig::arg::as<u32>(joan::orig::as_file(file)->file_size());  // 0x00587f30
                joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(buf_slot.get()), joan::orig::arg::u32(n));  // 0x005873a0
                const u32 end = joan::orig::arg::as<u32>(joan::orig::as_file(file)->file_size());  // 0x00587f30
                static_cast<u8*>(buf_slot.get())[end] = 0;
                set_text(&jo::as_item_screen(self())->label11, buf_slot.get());
                joan::orig::as_file(file)->destruct();  // 0x005872b0 dtor
            } else {
                set_text(&jo::as_item_screen(self())->label11, str(d::s_sp_9d05f0));
            }
        } else {
            set_text(&jo::as_item_screen(self())->label11, str(d::s_sp_9d05f0));
        }
        if (jo::as_item_def(item())->description != nullptr) {
            i32 count = 0;  // [ebp-0x26c] lies below the array: never overwritten
            std::uintptr_t format_args[10] = {};  // the format arguments, pointer-sized slots (text::format_5a4c30 reads them so)
            auto put = [&](u32 v) {
                if (count >= 0 && count < 10) format_args[count] = v;
                ++count;
            };
            put(joan::orig::arg::as<u32>(joan::orig::as_mod_table(item())->value_own(joan::orig::arg::i32(jo::as_item_screen(self())->u32_4))));  // 0x004a41c0
            for (i32 i = 0; i < jo::as_item_def(item())->effect_count; ++i) {
                const u8* const rows = reinterpret_cast<const u8*>(jo::as_item_def(item())->effects);
                const u32 lvl = jo::as_item_screen(self())->u32_4 - jo::view<u32>(jo::as_item_def(item())->min_level);
                const u32 k = *reinterpret_cast<const u32*>(rows + static_cast<u32>(i) * 0xcu + 8u);
                const u32 base = *reinterpret_cast<const u32*>(rows + static_cast<u32>(i) * 0xcu + 4u);
                put(base + k * lvl);
            }
            for (i32 i = 0; i < jo::as_item_def(item())->requirement_count; ++i) {
                const u8* const rows = reinterpret_cast<const u8*>(jo::as_item_def(item())->requirements);
                const u32 lvl = jo::as_item_screen(self())->u32_4 - jo::view<u32>(jo::as_item_def(item())->min_level);
                const u32 k = *reinterpret_cast<const u32*>(rows + static_cast<u32>(i) * 0x10u + 8u);
                const u32 base = *reinterpret_cast<const u32*>(rows + static_cast<u32>(i) * 0x10u + 4u);
                put(base + k * lvl);
            }
            const char* const text = joan::orig::text::format_5a4c30(jo::as_item_def(item())->description, static_cast<void*>(format_args));  // 0x005a4c30
            joan::orig::as_image2d(&jo::as_item_screen(self())->label12)->set_name(text);  // 0x006298c0
        } else {
            set_text(&jo::as_item_screen(self())->label12, str(d::s_sp_9d05f0));
        }
        joan::orig::as_item_def(item())->acquire_anim(joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}));         // 0x004a39f0
        const u32 id = joan::orig::arg::as<u32>(joan::orig::as_item_def(item())->sprite_id(joan::orig::arg::i32(u32{0})));  // 0x004a3f20
        jo::view<void*>(jo::as_item_screen(self())->actor) = new_obj(static_cast<u32>(sizeof(jo::SceneObject)), fn::code_value(fn::Actor_Actor_5aab20));       // 0x005aab20 (0x10c on x86)
        auto model = [&]() { return jo::view<void*>(jo::as_item_screen(self())->actor); };
        auto entry = [&]() { return joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(id))); };  // 0x0041a180
        {
            const u8* const en = entry();
            jo::as_scene_object(model())->def = reinterpret_cast<const jo::ModelEntry*>(en)->actor_def;
        }
        if (jo::view<u32>(jo::as_item_def(item())->slot_anims[0]) != 0) {
            jo::AnimDef* const* const r = joan::orig::arg::as<jo::AnimDef* const*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_item_def(item())->slot_anims[0]))));  // 0x004174f0
            jo::as_scene_object(model())->anim = *r;
        }
        joan::orig::as_scene_object(model())->init();  // 0x005aab70
        {
            const u8* const en = entry();
            joan::orig::as_actor(model())->set_skin(joan::orig::arg::u32(jo::view<u32>(reinterpret_cast<const jo::ModelEntry*>(en)->anim_db_id)));  // 0x005add40
        }
        joan::orig::as_actor(model())->set_position_5ac310(img::obj(d::g_pos));  // 0x005ac310
        jo::view<u32>(jo::as_scene_object(model())->scale) = kOne;
        joan::orig::as_scene_object(model())->set_yaw(joan::orig::arg::u32(u32{0}));  // 0x005ac6a0
        joan::orig::as_scene_object(model())->set_roll(joan::orig::arg::u32(u32{0}));  // 0x005ac980
        jo::view<u32>(jo::as_scene_object(model())->kind) = ~u32{0};
        jo::as_scene_object(model())->owner = nullptr;
        joan::orig::as_scene(jo::view<void*>(jo::as_item_screen(self())->shadow_scene))->attach(joan::orig::arg::mptr(model()));  // 0x005efc50
        if (jo::view<u32>(jo::as_item_def(item())->slot_anims[0]) != 0) {
            joan::orig::as_actor(model())->start_anim(joan::orig::arg::mptr(jo::as_scene_object(model())->anim), str(d::s_default_9d3d60), joan::orig::arg::u32(global<u32>(d::g_game_time_ms)), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(u32{0}), joan::orig::arg::u32(u32{0}));  // 0x005abe30 ("default")
        }
        for (i32 j = 0; j < reinterpret_cast<const jo::ModelEntry*>(entry())->morph_count; ++j) {
            const u8* const en = entry();
            void* const v = reinterpret_cast<const jo::ModelEntry*>(en)->morphs[j].morph;
            joan::orig::as_actor(model())->add_child(v, joan::orig::arg::u32(u32{0}));  // 0x005ae0c0
        }
        alignas(4) u32 rect[10] = {};                            // [ebp-0x2a0], fully written by 0x006140a0
        joan::orig::as_camera(jo::view<void*>(jo::as_item_screen(self())->camera))->frame(joan::orig::arg::mptr(rect));  // 0x006140a0
        if (jo::view<i16>(jo::as_item_def(item())->class_id) == 6) {
            if (jo::view<i16>(jo::as_item_def(item())->type_id) == 10) {
                rect[3] = 0x42700000u;  // 60.0f
                rect[7] = 0x43160000u;  // 150.0f
            } else {
                rect[3] = 0x42200000u;  // 40.0f
                rect[7] = 0x42c80000u;  // 100.0f
            }
        } else {
            rect[3] = 0x41f00000u;  // 30.0f
            rect[7] = 0x42aa0000u;  // 85.0f
        }
        joan::orig::as_camera(jo::view<void*>(jo::as_item_screen(self())->camera))->set_frame(joan::orig::arg::mptr(rect));  // 0x00614040
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

// fix/uninit: 0x006ba8b0 lets 0x005a4c30 read format arguments past the values written -> 0.
JOAN_REPLACE("fix/uninit", "ArmoryPanel::refresh_6ba8b0", 0x006ba8b0, &H::armory_refresh);

}  // namespace game_b111_07
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {


void* const kImages = obj(d::g_texture_array);
void* const kItems = obj(d::g_item_res);

void image_release(void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                    // 0x0060a550
std::uint8_t* item_rec(std::int16_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0

struct H {

    void release_at(jo::Texture*& image) {
        if (image != nullptr) {
            image_release(image);
            image = nullptr;
        }
    }

    // 0x006bb020
    void item_view_release() {
        release_at(jo::self<jo::ItemScreen>(this)->image_a);
        release_at(jo::self<jo::ItemScreen>(this)->image_b);
        release_at(jo::self<jo::ItemScreen>(this)->image_c);
        if (jo::self<jo::ItemScreen>(this)->item_id == 0 || jo::self<jo::ItemScreen>(this)->actor == nullptr) return;
        joan::orig::as_res(item_rec(jo::self<jo::ItemScreen>(this)->item_id))->release_slot(0, 0);  // 0x004a3e30
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::ItemScreen>(this)->shadow_scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ItemScreen>(this)->actor)));       // 0x005efdb0
        void* const p = jo::view<void*>(jo::self<jo::ItemScreen>(this)->actor);
        if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840
        jo::self<jo::ItemScreen>(this)->actor = nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("ItemView::release", 0x006bb020, &H::item_view_release);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

JOAN_COUNTER(c_panel_update, "Panel::update_6bb750");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x006bb750
    void panel_update() {
        c_panel_update.hit();
        if (jo::view<i8>(jo::self<jo::ItemScreen>(this)->open) == 0) return;
        if (jo::view<i8>(jo::self<jo::ItemScreen>(this)->shown) != 1 && jo::view<i8>(jo::self<jo::ItemScreen>(this)->shown) != 3) return;
        jo::ItemScreen* const s = jo::self<jo::ItemScreen>(this);
        // The original walks a table of the widgets' offsets (Labels and the Button, in this order).
        auto update = [](void* w) { joan::orig::as_widget(w)->update(); };  // 0x0061f3b0
        update(&s->label6); update(&s->label7); update(&s->label2); update(&s->label3);
        update(&s->label4); update(&s->label5); update(&s->label9); update(&s->label10);
        update(&s->label0); update(&s->label1); update(&s->label11); update(&s->label12);
        update(&s->label8); update(&s->button); update(&s->label13); update(&s->label14);
        const float a = 255.0f * jo::self<jo::ItemScreen>(this)->label6.scale;  // fld [0x9cfb68] / fmul (PC24 = float product)
        const u8 alpha = static_cast<u8>(joan::hook::x87::ftol(static_cast<double>(a)));
        const u32 color = 0x00ffffffu | (static_cast<u32>(alpha) << 24);
        joan::orig::as_plane(jo::self<jo::ItemScreen>(this)->actor->plane)->set_fade(joan::orig::arg::u32(color));  // 0x005d5e80
        // Busy while one of them still scales (Widget::scaling, labels 6, 7, 2-5, 9, 10) or moves (Widget::moving).
        const std::uint8_t* const busy[] = {&s->label6.scaling, &s->label7.scaling, &s->label2.scaling, &s->label3.scaling,
                                            &s->label4.scaling, &s->label5.scaling, &s->label9.scaling, &s->label10.scaling,
                                            &s->label0.moving,  &s->label1.moving,  &s->label11.moving, &s->label12.moving,
                                            &s->label8.moving,  &s->button.moving,  &s->label13.moving, &s->label14.moving};
        for (const std::uint8_t* const b : busy)
            if (static_cast<i8>(*b) != 0) return;
        const bool go = (alpha == 0xff && jo::view<i8>(jo::self<jo::ItemScreen>(this)->shown) == 1) || (alpha == 0 && jo::view<i8>(jo::self<jo::ItemScreen>(this)->shown) == 3);
        if (!go) return;
        if (jo::view<i8>(jo::self<jo::ItemScreen>(this)->shown) == 1) {
            jo::self<jo::ItemScreen>(this)->shown = 2;
            return;
        }
        jo::self<jo::ItemScreen>(this)->shown = 0;
        joan::orig::as_armory_screen(me())->close();  // 0x006ba810
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Panel::update_6bb750", 0x006bb750, &H::panel_update);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_ui5.cpp (game/ui5)
namespace {
namespace game_ui5 {


// 0x00622cc0 Button::draw(mode, flags), ret 8.
void button(void* b, std::uint32_t mode, std::uint32_t flags) { joan::orig::as_button(b)->draw(joan::orig::arg::u32(mode), static_cast<std::uint8_t>(flags)); }  // 0x00622cc0

JOAN_COUNTER(c_buttons, "Panel54c20::draw_buttons");

struct H {

    // 0x006bbac0
    void buttons_draw() {
        joan::canvas::ScreenScope canvas_scope(0x006bbac0);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_buttons.hit();
        if (static_cast<std::int8_t>(jo::self<jo::ItemScreen>(this)->open) == 0) return;
        static jo::Label jo::ItemScreen::* const kLabels[15] = {
            &jo::ItemScreen::label6, &jo::ItemScreen::label7, &jo::ItemScreen::label2,  &jo::ItemScreen::label3,  &jo::ItemScreen::label4,
            &jo::ItemScreen::label5, &jo::ItemScreen::label9, &jo::ItemScreen::label10, &jo::ItemScreen::label0,  &jo::ItemScreen::label1,
            &jo::ItemScreen::label11, &jo::ItemScreen::label12, &jo::ItemScreen::label13, &jo::ItemScreen::label14, &jo::ItemScreen::label8};
        for (std::int32_t i = 0; i < 15; ++i) joan::orig::as_label(&(jo::self<jo::ItemScreen>(this)->*kLabels[i]))->draw(3u, 7u);  // 0x00629bd0
        button(&jo::self<jo::ItemScreen>(this)->button, 3, 7);
    }

    // 0x006bbc50
    void camera_draw() {
        joan::canvas::ScreenScope canvas_scope(0x006bbc50);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        if (static_cast<std::int8_t>(jo::self<jo::ItemScreen>(this)->open) == 0) return;
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::ItemScreen>(this)->camera))->render();  // 0x00614760
    }
};

#define UI5(name, addr, fn) JOAN_REPLACE("game/ui5", name, addr, fn)
UI5("Panel54c20::draw_buttons", 0x006bbac0, &H::buttons_draw);
JOAN_CANVAS_SELF(0x006bbac0);  // ScreenScope in the body (no thunk wrapper)
UI5("Panel54c20::draw_camera", 0x006bbc50, &H::camera_draw);
JOAN_CANVAS_SELF(0x006bbc50);  // ScreenScope in the body (no thunk wrapper)
#undef UI5

}  // namespace game_ui5
}  // namespace

// ---------------------------------------- from modules/game_b109_15.cpp (game/b109_15)
namespace {
namespace game_b109_15 {

using joan::hook::x87::Fpu;

using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;
float fl(u32 bits) {
    float f;
    std::memcpy(&f, &bits, 4);
    return f;
}

constexpr u32 kOneF = 0x3f800000;  // 1.0f

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), ret 0x10; returns this.
u32 make_color(u32 r, u32 g, u32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*reinterpret_cast<const jo::Colour*>(p));  // the four bytes as one dword
}
// alpha = _ftol(255.0f * ctl->+0x11)
i32 alpha_of(const void* ctl) {
    const float a = global<float>(d::flt_255) * jo::as_button(ctl)->scale;
    return ftol(static_cast<double>(a));
}
// 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
void draw_sprite(i32 x, i32 y, u32 surf, img::Addr name, void* set, u32 color, i32 a7, i32 a8, i32 a9, i32 a10) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surf), img::str(name), set, joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, -1, -1);
}
// 0x00633e60 (thunk 0x0040807b) on 0xa46128, ret 0x30.
void fit_rect(const void* ctl, u32* f14, u32* f18, i16* s4, i16* s8, i16* sc, i16* s10) {
    joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::as_button(ctl)->x), joan::orig::arg::i16(jo::as_button(ctl)->y), joan::orig::arg::i16(jo::as_button(ctl)->x2), joan::orig::arg::i16(jo::as_button(ctl)->y2), joan::orig::arg::mptr(f14), joan::orig::arg::mptr(f18), joan::orig::arg::mptr(s4), joan::orig::arg::mptr(s8), joan::orig::arg::mptr(sc), joan::orig::arg::mptr(s10), joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
}
// 0x00590a20 (thunk 0x00401497) on 0xa45fa8 with (double)(1.0f * scale), result movsx ax.
i32 scale_round(u32 scale_bits) {
    const double d = Fpu{true}.mul(1.0, static_cast<double>(fl(scale_bits)));
    return joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(d)));
}
// 0x00624f00 (thunk 0x00405f97): text width (text, -1, 0, 0, 1.0f), ret 0x14.
i32 text_width(void* font, void* text) {
    return joan::orig::as_font(font)->measure(joan::orig::arg::str(text), -1, 0, 0, joan::orig::arg::f32(kOneF));
}
// 0x006253f0 (thunk 0x004076d0): (2, 1.0f), ret 8; result unused.
void text_height(void* font) { joan::orig::as_font(font)->text_height(joan::orig::arg::i16(i32{2}), joan::orig::arg::f32(kOneF)); }
// 0x00626130 (thunk 0x00402e0f): text in a box, ret 0x34.
void draw_text(void* font, i32 x, i32 y, i16 w, i32 h, u32 surf, void* text, u32 color) {
    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(surf), joan::orig::arg::str(text), joan::orig::arg::u32(color), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOneF), joan::orig::arg::f32(kOneF));
}

JOAN_COUNTER(c_itemmark, "item_mark_draw_6bbcd0");

// 0x006bbcd0
u32 JOAN_CDECL item_mark_draw(void* ctl, u32 surf, u32 flags) {
    c_itemmark.hit();
    if ((flags & 0xff) != 7 && ((flags & 0xff) & 1) != 0) return 0;
    i16 s4 = jo::as_button(ctl)->x;
    i16 s8 = jo::as_button(ctl)->y;
    i16 sc = jo::as_button(ctl)->x2;
    i16 s10 = jo::as_button(ctl)->y2;
    u32 f14 = kOneF, f18 = kOneF;
    if (jo::as_button(ctl)->fit != 0) fit_rect(ctl, &f14, &f18, &s4, &s8, &sc, &s10);
    void* const font = jo::as_button(ctl)->caption_box;
    if (jo::view<i8>(jo::as_button(ctl)->caption[0]) == 0) return 1;
    void* const text = jo::as_button(ctl)->caption;
    constexpr img::Addr kMark = d::s_MARK, kImg = d::g_bmp_res_armory;  // "MARK", image object address
    auto label = [&](u32 r, u32 g, u32 b) {
        const i32 ry = jo::as_button(ctl)->pressed != 0 ? scale_round(f18) : 0;
        const i32 rx = jo::as_button(ctl)->pressed != 0 ? scale_round(f14) : 0;
        const u32 color = make_color(r, g, b, alpha_of(ctl));
        draw_text(font, wadd(s4, rx), wadd(s8, ry) - 1, sc, static_cast<i32>(s10) - 1, surf, text, color);
    };
    if (jo::as_button(ctl)->pressed != 0) {
        const i32 w20 = text_width(font, text);
        text_height(font);
        u32 color = make_color(0xff, 0xff, 0xff, alpha_of(ctl));
        draw_sprite((wsub(static_cast<i32>(s4) + sc, w20)) / 2 - 0x14, static_cast<i32>(s8) + 8, surf, kMark, img::obj(kImg),
                    color, 3, 0, 2, 0);
        color = make_color(0xff, 0xff, 0xff, alpha_of(ctl));
        draw_sprite(wadd(wadd(w20, s4), sc) / 2 + 6, static_cast<i32>(s8) + 8, surf, kMark, img::obj(kImg), color, 3, 0, 2, 0);
        label(0, 0, 0);
        return 1;
    }
    if (jo::as_button(ctl)->pressed == 0 &&
        joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{s4}, i32{s8}, i32{sc}, i32{s10})) != 0) {  // 0x00593980
        const i32 w28 = text_width(font, text);
        text_height(font);
        u32 color = make_color(0xff, 0xff, 0xff, alpha_of(ctl));
        {
            const Fpu fpu{true};
            const double d = fpu.sub(fpu.mul(static_cast<double>(wsub(static_cast<i32>(s4) + sc, w28)), 0.5), 20.0);
            draw_sprite(ftol(d), static_cast<i32>(s8) + 8, surf, kMark, img::obj(kImg), color, 3, 0, 2, 0);
        }
        color = make_color(0xff, 0xff, 0xff, alpha_of(ctl));
        {
            const Fpu fpu{true};
            const double d = fpu.add(fpu.mul(static_cast<double>(wadd(wadd(w28, s4), sc)), 0.5), 6.0);
            draw_sprite(ftol(d), static_cast<i32>(s8) + 8, surf, kMark, img::obj(kImg), color, 3, 0, 2, 0);
        }
        label(0xff, 0xff, 0xff);
        return 1;
    }
    label(0xff, 0xff, 0xff);
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_15", name, addr, fn)
B("item_mark_draw_6bbcd0", 0x006bbcd0, &item_mark_draw);
#undef B

}  // namespace game_b109_15
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ArmoryScreen::refresh() -> void { joan::bind::member<&::game_b111_07::H::armory_refresh>(this); }  // 0x6ba8b0
auto joan::orig::ArmoryScreen::release_item_view() -> void { joan::bind::member<&::game_b92_06::H::item_view_release>(this); }  // 0x6bb020
auto joan::orig::ArmoryScreen::set_item(std::int16_t a0, std::int16_t a1) -> void { joan::bind::member<&::game_b93_06::H::set_pair>(this, a0, a1); }  // 0x6bbc90
