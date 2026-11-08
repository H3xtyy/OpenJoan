// game/unknown_credits.cpp: functions whose original source file is unknown (formerly game/_unplaced/credits.cpp;
// re/attribution.csv: between ww/interfac/combobox.cpp and ww/interfac/i_barr.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cmath>
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

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

using joan::call::stdcall_call;

using joan::img::str;

using joan::img::obj;

using joan::orig::helpers::at;

using i16 = std::int16_t;

constexpr u32 kOne = 0x3f800000u;  // 1.0f

using joan::orig::helpers::addr_of;

using joan::hook::global;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

using joan::orig::helpers::crt_memset;


struct H {
    // ------------------------------------------------------------------ 0x006b4e00
    H* ctor_6b4e00() {
        joan::orig::as_image2d(&jo::self<jo::Credits>(this)->backdrop)->construct();  // 0x00629400
        joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Credits>(this)->label_0)), static_cast<u32>(sizeof(jo::Label)), 5,  // eh vector ctor
                           fn::code_value(fn::Widget629_ctor), fn::code_value(fn::Widget_dtor_6294c0));                            // 0x629400 / 0x6294c0
        crt_memset(this, 0, static_cast<u32>(offsetof(jo::Credits, backdrop)));  // active, stage, images[12] (0x32 bytes on x86)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("Obj6b4::ctor", 0x006b4e00, &H::ctor_6b4e00);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

JOAN_COUNTER(c_dtor6b4ea0, "Obj6b4ea0::~dtor");

struct H {

    // ------------------------------------------------------------------ 0x006b4ea0
    u32 dtor_6b4ea0() {
        c_dtor6b4ea0.hit();
        joan::orig::as_credits(this)->release();  // 0x006b52b0
        joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Credits>(this)->label_0)), static_cast<u32>(sizeof(jo::Label)), 5, fn::code_value(fn::Widget_dtor_6294c0));  // eh vector destructor iterator, dtor 0x006294c0
        joan::orig::as_widget(&jo::self<jo::Credits>(this)->backdrop)->dtor_6294c0();  // 0x006294c0
        return 0;  // void callee; eax unused (atexit dtor)
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj6b4ea0::~dtor", 0x006b4ea0, &H::dtor_6b4ea0);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
namespace {
namespace game_b101_14 {


u32 colour(void* storage, u32 r, u32 g, u32 b, u32 a) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(storage)->construct(r, g, b, a));  // 0x0042a260
}

JOAN_COUNTER(c_backgrounds, "Screen::create_backgrounds");

struct H {
    // ------------------------------------------------------------------ 0x006b4f30
    void create_backgrounds() {
        c_backgrounds.hit();
        static const img::Addr kNames[6] = {d::s_bg1, d::s_t1, d::s_t2,
                                                 d::s_t3, d::s_t4, d::s_t5};
        for (i32 i = 0; i < 6; ++i) {
            // backdrop then label_0 .. label_4, sizeof(Label) (== sizeof(Image2D)) apart
            u8* const item = reinterpret_cast<u8*>(&jo::self<jo::Credits>(this)->backdrop) + i * static_cast<i32>(sizeof(jo::Label));
            u32 storage = 0;
            const u32 c = colour(&storage, 0, 0, 0, 0xff);
            const i32 x = i == 0 ? 0 : 0xfc;
            const i32 y = i == 0 ? 0 : 0x88;
            joan::orig::as_label(item)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::mptr(img::obj(d::g_bmp_credits)), joan::orig::arg::str(str(kNames[i])), joan::orig::arg::i32(c), 3, 0, 2, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);                                 // 0x00629660
            joan::orig::as_widget(item)->scale_to(1, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);           // 0x0061f1b0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("Screen::create_backgrounds", 0x006b4f30, &H::create_backgrounds);
#undef B

}  // namespace game_b101_14
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

struct H {

    void reset_6b52b0() {  // 0x006b52b0
        joan::orig::as_credits(this)->free_sprites();         // 0x006b5470
        joan::orig::as_image2d(&jo::self<jo::Credits>(this)->backdrop)->release();  // 0x00629830
        for (i32 i = 0; i < 5; ++i) joan::orig::as_image2d(&(&jo::self<jo::Credits>(this)->label_0)[i])->release();  // label_0 .. label_4
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6b::reset", 0x006b52b0, &H::reset_6b52b0);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

void* const kWindows = obj(d::g_texture_array);

// Window-image load with the "current window" saved / restored (0x60a720 / 0x60a740).
u32 window_save() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kWindows)->mode()); }                // 0x0060a720
void window_set(u32 v) { joan::orig::as_texture_manager(kWindows)->set_mode(v); } // 0x0060a740
void* window_load(const char* name) { return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kWindows)->load(joan::orig::arg::str(name), 1, 0u)); }  // 0x006099c0

JOAN_COUNTER(c_credits_load, "Credits::load");

struct H {
    // ------------------------------------------------------------------ 0x006b5320
    u32 credits_load() {
        c_credits_load.hit();
        const u32 saved = window_save();
        window_set(1);
        if (jo::self<jo::Credits>(this)->images[0] == 0) jo::self<jo::Credits>(this)->images[0] = window_load(str(d::s_credits_bg_1));    // "credits_bg_1"
        if (jo::self<jo::Credits>(this)->images[4] == 0) jo::self<jo::Credits>(this)->images[4] = window_load(str(d::s_credits_bg_mask_1));  // "credits_bg_mask_1"
        if (jo::self<jo::Credits>(this)->images[8] == 0) jo::self<jo::Credits>(this)->images[8] = window_load(str(d::s_credits1));  // "credits1"
        if (jo::self<jo::Credits>(this)->images[9] == 0) jo::self<jo::Credits>(this)->images[9] = window_load(str(d::s_credits2));  // "credits2"
        if (jo::self<jo::Credits>(this)->images[10] == 0) jo::self<jo::Credits>(this)->images[10] = window_load(str(d::s_credits3));  // "credits3"
        if (jo::self<jo::Credits>(this)->images[11] == 0) jo::self<jo::Credits>(this)->images[11] = window_load(str(d::s_credits_border));  // "credits_border"
        window_set(saved);
        return 0;  // void callee; Credits::load's result is dropped by its caller
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Credits::load", 0x006b5320, &H::credits_load);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

void* const kImages = obj(d::g_texture_array);

void image_release(void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                    // 0x0060a550

struct H {
    void release_at(std::int32_t k) {
        if (jo::self<jo::Credits>(this)->images[k] != 0) {
            image_release(jo::self<jo::Credits>(this)->images[k]);
            jo::self<jo::Credits>(this)->images[k] = 0;
        }
    }

    // 0x006b5470
    void release12() {
        for (std::int32_t k = 0; k < 12; ++k) release_at(k);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Images12::release", 0x006b5470, &H::release12);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b116_13.cpp (game/b116_13)
namespace {
namespace game_b116_13 {


void* const kMouse = obj(d::g_mouse);

// Image size record of `name` (0x00620b40 on a resource set).
u8* image(void* set, const void* name) { return joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(set)->find(joan::orig::arg::str(name))); }  // 0x00620b40

void anim(void* d, i32 a, u32 speed, i32 c, i32 e, i32 f) {
    joan::orig::as_widget(d)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(speed), joan::orig::arg::f32(c), static_cast<char>(e), joan::orig::arg::u32(f));  // 0x0061f1b0
}
void slide(void* d, i32 x, i32 y, u32 a, u32 b, i32 flag, i32 delay) {
    joan::orig::as_widget(d)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(a), joan::orig::arg::f32(b), static_cast<char>(flag), joan::orig::arg::u32(delay));  // 0x0061e020
}

JOAN_COUNTER(c_credits, "Credits::setup_6b56b0");

struct H {
    // ------------------------------------------------------------------ 0x006b56b0
    u32 credits_6b56b0() {
        c_credits.hit();
        joan::orig::as_credits(this)->load_sprites();    // 0x006b5320
        joan::orig::as_cursor(kMouse)->set_pos();  // 0x00592bd0
        jo::view<u8>(jo::self<jo::Credits>(this)->active) = 1;
        jo::view<u8>(jo::self<jo::Credits>(this)->stage) = 0;
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Credits>(this)->backdrop.sprite_name)), str(d::s_bg1));  // strcpy "bg1"
        anim(&jo::self<jo::Credits>(this)->backdrop, 0, kOne, 0, 1, 0);
        anim(&jo::self<jo::Credits>(this)->backdrop, 1, 0x40800000u, 0, 0, 0);  // 4.0f
        jo::Label* const labels = &jo::self<jo::Credits>(this)->label_0;  // label_0 .. label_4 are consecutive
        i32 y = 0x300;
        i32 i = 0;
        for (; i < 5; ++i) {
            jo::BmpEntry* const p = jo::as_bmp_entry(image(obj(d::g_bmp_credits), labels[i].sprite_name));
            i32 w;
            i32 h;
            if (jo::view<i16>(p->u16_32) != 0) {
                w = jo::view<i16>(p->height);
                h = jo::view<i16>(p->width);
            } else {
                w = jo::view<i16>(p->width);
                h = jo::view<i16>(p->height);
            }
            const i32 x = (joan::canvas::size_read(0x006b579b) - w) / 2;
            void* const line = &labels[i];
            slide(line, static_cast<i16>(x), static_cast<i16>(y), 0x42700000u, 0, 1, 0);  // 60.0f
            slide(line, static_cast<i16>(x), 0x88 - h, 0x42700000u, 0, 0, 0);
            y += h;
        }
        return static_cast<u32>(i);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b116_13", name, addr, fn)
B("Credits::setup_6b56b0", 0x006b56b0, &H::credits_6b56b0);
#undef B

}  // namespace game_b116_13
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

struct H {
    // ------------------------------------------------------------------ 0x006b5880
    u32 close_6b5880() {
        joan::orig::as_credits(this)->free_sprites();                     // 0x006b5470
        jo::view<u8>(jo::self<jo::Credits>(this)->active) = 0;
        return joan::orig::arg::as<u32>(joan::orig::as_cursor(obj(d::g_mouse))->reset_pos());  // 0x00592c30
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj6b5880::close", 0x006b5880, &H::close_6b5880);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

struct H {
    std::uintptr_t self_hi() { return addr_of(this) & ~std::uintptr_t{0xff}; }

    // ------------------------------------------------------------------------ 0x006b58c0
    std::uintptr_t byte0_6b58c0() { return self_hi() | jo::view<u8>(jo::self<jo::Credits>(this)->active); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::byte0_6b58c0", 0x006b58c0, &H::byte0_6b58c0);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

float fconst(img::Addr a) { return img::global<const float>(a); }

JOAN_COUNTER(c_credits, "Credits::update_6b5900");

struct H {
    // ------------------------------------------------------------------ 0x006b5900
    void credits_update() {
        c_credits.hit();
        if (jo::self<jo::Credits>(this)->active == 0) return;
        joan::orig::as_widget(&jo::self<jo::Credits>(this)->backdrop)->update();  // 0x0061f3b0
        if (jo::self<jo::Credits>(this)->stage >= 1) {
            for (i32 i = 0; i < 5; ++i) joan::orig::as_widget(&(&jo::self<jo::Credits>(this)->label_0)[i])->update();  // label_0 .. label_4
        }
        if (jo::self<jo::Credits>(this)->stage == 6) joan::orig::as_game(obj(d::g_game_a50088))->set_state(3u);  // 0x004d7e70
        if (jo::self<jo::Credits>(this)->stage == 5 && jo::view<i8>(jo::self<jo::Credits>(this)->backdrop.scaling) == 0) jo::view<u8>(jo::self<jo::Credits>(this)->stage) = static_cast<u8>(jo::view<u8>(jo::self<jo::Credits>(this)->stage) + 1u);
        if (jo::self<jo::Credits>(this)->stage == 4 && jo::view<i8>(jo::self<jo::Credits>(this)->label_4.moving) == 0) {
            joan::orig::as_widget(&jo::self<jo::Credits>(this)->backdrop)->scale_to(0, joan::orig::arg::f32(0x40800000u), 0.0f, 1, 0u);  // 0x0061f1b0
            jo::view<u8>(jo::self<jo::Credits>(this)->stage) = static_cast<u8>(jo::view<u8>(jo::self<jo::Credits>(this)->stage) + 1u);
        }
        {
            const float a = jo::self<jo::Credits>(this)->backdrop.scale;
            const float b = fconst(d::flt_1);
            const bool eq_or_nan = !(a < b) && !(a > b);
            if (eq_or_nan && jo::self<jo::Credits>(this)->stage <= 3) {
                jo::view<u8>(jo::self<jo::Credits>(this)->stage) = static_cast<u8>(jo::view<u8>(jo::self<jo::Credits>(this)->stage) + 1u);
                const u8 st = jo::view<u8>(jo::self<jo::Credits>(this)->stage);
                std::uintptr_t name = 0;
                if (st == 1) name = img::addr(d::s_bg2);       // "bg2"
                else if (st == 2) name = img::addr(d::s_bg3);  // "bg3"
                else if (st == 3) name = img::addr(d::s_bg4);  // "bg4"
                if (name != 0) {
                    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Credits>(this)->backdrop.sprite_name)), joan::orig::arg::str(str(name)));  // strcpy
                    joan::orig::as_widget(&jo::self<jo::Credits>(this)->backdrop)->scale_to(0, joan::orig::arg::f32(kOne), 0.0f, 1, 0u);
                    joan::orig::as_widget(&jo::self<jo::Credits>(this)->backdrop)->scale_to(1, joan::orig::arg::f32(0x3ea978d5u), 0.0f, 0, 0x32c8u);
                }
            }
        }
        if (jo::view<i8>(jo::self<jo::Credits>(this)->backdrop.scaling) == 0) return;
        void* const res = obj(d::g_texture_array);
        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(res)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(res)->set_mode(1u);                     // 0x0060a740
        auto load = [res, this](i32 k, img::Addr name) {
            if (jo::self<jo::Credits>(this)->images[k] == 0) jo::self<jo::Credits>(this)->images[k] = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(res)->load(joan::orig::arg::str(str(name)), 1, 0u));  // 0x006099c0
        };
        auto release = [res, this](i32 k) {
            if (jo::self<jo::Credits>(this)->images[k] != 0) {
                joan::orig::as_texture_manager(res)->release(joan::orig::arg::mptr(jo::self<jo::Credits>(this)->images[k]));  // 0x0060a550
                jo::self<jo::Credits>(this)->images[k] = 0;
            }
        };
        switch (static_cast<u32>(static_cast<i32>(jo::self<jo::Credits>(this)->stage) - 1)) {
            case 0:
                load(1, d::s_credits_bg_2);     // "credits_bg_2"
                load(5, d::s_credits_bg_mask_2);  // "credits_bg_mask_2"
                break;
            case 1:
                load(2, d::s_credits_bg_3);   // "credits_bg_3"
                load(6, d::s_credits_bg_mask_3);  // "credits_bg_mask_3"
                release(0);
                release(4);
                break;
            case 2:
                load(3, d::s_credits_bg_4);   // "credits_bg_4"
                load(7, d::s_credits_bg_mask_4);  // "credits_bg_mask_4"
                release(1);
                release(5);
                break;
            case 3:
                release(2);
                release(6);
                break;
            default:
                break;
        }
        joan::orig::as_texture_manager(res)->set_mode(joan::orig::arg::u32(saved));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Credits::update_6b5900", 0x006b5900, &H::credits_update);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b109_06.cpp (game/b109_06)
namespace {
namespace game_b109_06 {

using joan::hook::x87::ftol;

using joan::hook::x87::Fpu;

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
    return jo::view<u32>(*jo::as_colour(p));
}
// 0x00633500 on 0xa46128 (17 args, ret 0x44).
void draw_sprite(i32 x, i32 y, u32 surf, img::Addr name, void* pal, u32 color, i32 a7, i32 a8, i32 a9, i32 a10) {
    joan::orig::as_sprite_queue(kScreen)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surf), joan::orig::arg::str(img::addr(name)), joan::orig::arg::mptr(pal), joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, -1, -1);
}

JOAN_COUNTER(c_credits, "Credits::draw_6b5e60");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // ------------------------------------------------------------------ 0x006b5e60
    std::uintptr_t credits_draw() {
        c_credits.hit();
        if (jo::self<jo::Credits>(this)->active == 0) return addr_of(this);
        const Fpu fpu{true};
        auto alpha = [&]() { return ftol(fpu.mul(global<float>(d::flt_255), jo::self<jo::Credits>(this)->backdrop.scale)); };
        auto black = [&]() {  // 0x00632a70: whole screen, black
            const u32 color = make_color(0, 0, 0, 0xff);
            joan::orig::as_sprite2d(kScreen)->queue(i32{0}, i32{0}, joan::orig::arg::i32(global<i32>(d::g_screen_w) - 1), joan::orig::arg::i32(global<i32>(d::g_screen_h) - 1), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(color), joan::orig::arg::mptr(i32{0}), i32{3}, i32{1}, i32{1}, i32{0}, i32{-1}, i32{-1}, i32{-1}, i32{-1}, i32{0});
        };
        auto child = [&](void* w) { joan::orig::as_label(w)->draw(joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{7})); };  // 0x00629bd0
        auto bg = [&](img::Addr name) {
            const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
            draw_sprite(0, 0, 1, name, img::obj(d::g_bmp_credits), color, 3, 0, 0, 0);
        };
        auto mask = [&](img::Addr name, i32 tex) {  // tex: Credits::images index
            if (!(jo::self<jo::Credits>(this)->backdrop.scale < global<float>(d::k_9cf5c4)) && !(jo::self<jo::Credits>(this)->backdrop.scale > global<float>(d::k_9cf5c4))) {
                bg(name);  // alpha 0 (or NaN): opaque
                return;
            }
            const u32 color = make_color(0xff, 0xff, 0xff, alpha());
            draw_sprite(0, 0, 1, name, img::obj(d::g_bmp_credits), color, 3, 0, 0, 0);
            // 0x00a465f2 is Sprite2D::scene (+0x4ca) of the 2D sprite camera g_sprites_a46128.
            u8* const p = jo::view<u8*>(jo::as_scene_s(joan::orig::as_scene(jo::as_sprite2d(obj(d::g_sprites_a46128))->scene)->group(i32{1}))->plane);  // 0x005ea6e0
            jo::as_plane(p)->materials[jo::as_plane(p)->material_count - 1].texture_mode = 0x11;
            void* const m = &jo::as_plane(p)->materials[jo::as_plane(p)->material_count - 1];
            joan::orig::as_material(m)->set_texture(i32{1}, joan::orig::arg::mptr(jo::self<jo::Credits>(this)->images[tex]));  // 0x005d5650
        };
        switch (jo::self<jo::Credits>(this)->stage) {
            case 0:
                black();
                child(&jo::self<jo::Credits>(this)->backdrop);
                break;
            case 1:
                bg(d::s_bg1);  // "bg1"
                child(&jo::self<jo::Credits>(this)->backdrop);
                break;
            case 2:
                bg(d::s_bg2);  // "bg2"
                child(&jo::self<jo::Credits>(this)->backdrop);
                break;
            case 3:
                bg(d::s_bg3);  // "bg3"
                child(&jo::self<jo::Credits>(this)->backdrop);
                break;
            case 4:
                bg(d::s_bg4);  // "bg4"
                break;
            case 5:
            case 6:
                black();
                child(&jo::self<jo::Credits>(this)->backdrop);
                break;
            default:
                break;
        }
        for (i32 i = 0; i < 5; ++i)
            if (jo::view<i8>((&jo::self<jo::Credits>(this)->label_0)[i].moving) != 0) child(&(&jo::self<jo::Credits>(this)->label_0)[i]);  // label_0 .. label_4
        switch (jo::self<jo::Credits>(this)->stage) {
            case 1:
                mask(d::s_bg1_mask, 5);  // "bg1_mask"
                break;
            case 2:
                mask(d::s_bg2_mask, 6);  // "bg2_mask"
                break;
            case 3:
                mask(d::s_bg3_mask, 7);  // "bg3_mask"
                break;
            case 4:
                bg(d::s_bg4_mask);  // "bg4_mask"
                break;
            default:
                break;
        }
        const i8 b1 = jo::self<jo::Credits>(this)->stage;
        if (b1 == 0 || b1 == 5 || b1 == 6) {
            u32 color = make_color(0xff, 0xff, 0xff, alpha());
            draw_sprite(0, 0, 1, d::s_border, img::obj(d::g_bmp_credits), color, 3, 0, 2, 0);  // "border"
            color = make_color(0xff, 0xff, 0xff, alpha());
            draw_sprite(0x174, 0x18, 1, d::s_logo, img::obj(d::g_bmp_credits), color, 3, 0, 2, 0);  // "logo"
            color = make_color(0xff, 0xff, 0xff, alpha());
            draw_sprite(0x13c, 0x70, 1, d::s_bar_9dd9f8, img::obj(d::g_bmp_credits), color, 3, 0, 2, 0);  // "bar"
            color = make_color(0xff, 0xff, 0xff, alpha());
            const char* const text = joan::orig::as_menu(obj(d::g_strings))->credits_1();  // 0x004de960
            return joan::orig::arg::as<u32>(joan::orig::as_text(obj(d::g_font_goudy_bold_14))->draw_3(i32{0x13c}, i32{0x70}, i32{0x2c3}, i32{0x87}, joan::orig::arg::u32(i32{1}), text, joan::orig::arg::u32(color), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000})));  // 0x00626130
        }
        bg(d::s_border);  // "border" (opaque)
        u32 color = make_color(0xff, 0xff, 0xff, 0xff);
        draw_sprite(0x174, 0x18, 1, d::s_logo, img::obj(d::g_bmp_credits), color, 3, 0, 0, 0);  // "logo"
        color = make_color(0xff, 0xff, 0xff, 0xff);
        draw_sprite(0x13c, 0x70, 1, d::s_bar_9dd9f8, img::obj(d::g_bmp_credits), color, 3, 0, 0, 0);  // "bar"
        color = packed_color(0xffffffffu);
        const char* const text = joan::orig::as_txt_res(obj(d::g_strings))->credits_text_02();  // 0x004de990
        return joan::orig::arg::as<u32>(joan::orig::as_text(obj(d::g_font_goudy_bold_14))->draw_3(i32{0x13c}, i32{0x70}, i32{0x2c3}, i32{0x87}, joan::orig::arg::u32(i32{1}), text, joan::orig::arg::u32(color), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000})));  // 0x00626130
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_06", name, addr, fn)
B("Credits::draw_6b5e60", 0x006b5e60, &H::credits_draw);
#undef B

}  // namespace game_b109_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Credits::release() -> void { joan::bind::member<&::game_b93_06::H::reset_6b52b0>(this); }  // 0x6b52b0
auto joan::orig::Credits::load_sprites() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::credits_load>(this)); }  // 0x6b5320
auto joan::orig::Credits::free_sprites() -> void { joan::bind::member<&::game_b92_06::H::release12>(this); }  // 0x6b5470
auto joan::orig::Credits::draw() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_06::H::credits_draw>(this)); }  // 0x6b5e60
