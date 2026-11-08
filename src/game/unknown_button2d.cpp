// game/unknown_button2d.cpp: functions whose original source file is unknown (formerly game/_unplaced/button2d.cpp;
// re/attribution.csv: between ww/en2d/bmpres.cpp and ww/en2d/font2d.cpp). They move to their original file when
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
#include <limits>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../platform/input.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::hook::global;

using joan::img::str;

using i8 = std::int8_t;
using i16 = std::int16_t;

// CRT
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }

using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b101_12.cpp (game/b101_12)
namespace {
namespace game_b101_12 {


JOAN_COUNTER(c_ctor_620ee0, "Widget::ctor_620ee0");

struct H {
    // 0x00620ee0
    void* ctor_620ee0() {
        c_ctor_620ee0.hit();
        joan::orig::as_gadget(this)->ctor();  // 0x0061de00
        jo::view<std::uintptr_t>(jo::self<jo::Button>(this)->vtable) = img::addr(d::vt_Button);
        jo::view<u32>(jo::self<jo::Button>(this)->state) = 0;
        jo::view<u8>(jo::self<jo::Button>(this)->shown) = 0;
        jo::view<u16>(jo::self<jo::Button>(this)->x) = 0xffff;
        jo::view<u16>(jo::self<jo::Button>(this)->y) = 0xffff;
        jo::view<u16>(jo::self<jo::Button>(this)->x2) = 0xffff;
        jo::view<u16>(jo::self<jo::Button>(this)->y2) = 0xffff;
        jo::self<jo::Button>(this)->momentary = 0;
        jo::view<u8>(jo::self<jo::Button>(this)->pressed) = 0;
        jo::self<jo::Button>(this)->drawn = 0;
        jo::self<jo::Button>(this)->held = 0;
        jo::self<jo::Button>(this)->hover = 0;
        jo::self<jo::Button>(this)->click_sound = 0;
        jo::view<u16>(jo::self<jo::Button>(this)->hotkey) = 0;
        jo::self<jo::Button>(this)->sprite_set = nullptr;
        for (i32 i = 0; i < 4; ++i) {
            jo::view<u8>(jo::self<jo::Button>(this)->states[i].sprite[0]) = 0;
            joan::orig::as_colour(&jo::self<jo::Button>(this)->states[i].colour)->set_argb(0u);  // 0x005e94f0
            jo::view<u8[4]>(jo::self<jo::Button>(this)->states[i].colour)[3] = 0xff;  // alpha
            jo::view<u16>(jo::self<jo::Button>(this)->states[i].a20) = 0;
            jo::view<u16>(jo::self<jo::Button>(this)->states[i].a24) = 2;
            jo::view<u16>(jo::self<jo::Button>(this)->states[i].a28) = 0;
        }
        jo::view<u8>(jo::self<jo::Button>(this)->press_sound) = 0;
        jo::view<u8>(jo::self<jo::Button>(this)->help_sprite[0]) = 0;
        jo::view<u8>(jo::self<jo::Button>(this)->tooltip[0]) = 0;
        jo::view<u8>(jo::self<jo::Button>(this)->help_text[0]) = 0;
        jo::view<u8>(jo::self<jo::Button>(this)->caption[0]) = 0;
        jo::self<jo::Button>(this)->caption_box = nullptr;
        joan::orig::as_colour(&jo::self<jo::Button>(this)->caption_colour)->set_argb(0u);  // 0x005e94f0
        jo::view<u8[4]>(jo::self<jo::Button>(this)->caption_colour)[3] = 0xff;  // alpha
        jo::view<u16>(jo::self<jo::Button>(this)->caption_330) = 0;
        jo::view<u16>(jo::self<jo::Button>(this)->caption_332) = 2;
        jo::view<u16>(jo::self<jo::Button>(this)->caption_334) = 0;
        jo::view<u16>(jo::self<jo::Button>(this)->press_dx) = 0;
        jo::view<u16>(jo::self<jo::Button>(this)->press_dy) = 0;
        jo::self<jo::Button>(this)->callback = 0;
        jo::self<jo::Button>(this)->u32_33e = 0;
        jo::self<jo::Button>(this)->u32_342 = 0;
        return this;
    }
};

#define R12(name, addr, fn) JOAN_REPLACE("game/b101_12", name, addr, fn)
R12("Widget::ctor_620ee0", 0x00620ee0, &H::ctor_620ee0);
#undef R12

}  // namespace game_b101_12
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
namespace {
namespace game_b90_05 {

std::size_t c_strlen(const char* s) { return std::strlen(joan::orig::arg::str(s)); }
char* c_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_button_create, "Button::create");
JOAN_COUNTER(c_set_text, "Button::set_text");

struct H {
    // 0x006211a0
    void* widget_dtor() {
        jo::view<std::uintptr_t>(jo::self<jo::Button>(this)->vtable) = img::addr(d::vt_Button);
        return joan::orig::arg::as<void*>(joan::orig::as_gadget(this)->destruct());  // 0x0061dfb0
    }

    // 0x00621540
    std::int32_t button_create(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::uintptr_t id,  // id: the draw callback (code pointer)
                               std::int32_t b6, std::int32_t b7, std::int32_t b8, std::int32_t b9,
                               std::int32_t b10, std::int32_t b11) {
        c_button_create.hit();
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->x) = static_cast<std::uint16_t>(x);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->hit_rect[0]) = static_cast<std::uint16_t>(x);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->y) = static_cast<std::uint16_t>(y);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->hit_rect[1]) = static_cast<std::uint16_t>(y);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->x2) = static_cast<std::uint16_t>(w);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->hit_rect[2]) = static_cast<std::uint16_t>(w);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->y2) = static_cast<std::uint16_t>(h);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->hit_rect[3]) = static_cast<std::uint16_t>(h);
        jo::self<jo::Button>(this)->momentary = static_cast<std::uint8_t>(b6);
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->pressed) = static_cast<std::uint8_t>(b7);
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->fit) = static_cast<std::uint8_t>(b8);
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->enabled) = static_cast<std::uint8_t>(b9);
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->i8_8) = static_cast<std::uint8_t>(b10);
        jo::self<jo::Button>(this)->click_sound = static_cast<std::uint8_t>(b11);
        jo::self<jo::Button>(this)->drawn = 0;
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->shape) = 0;
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->i8_8e) = 1;
        jo::self<jo::Button>(this)->callback = id;
        const char* const back = &img::global<const char>(d::s_MENU_BACK_1);  // "MENU-BACK-1"
        if (c_strlen(back) <= 0x20u) c_strcpy(reinterpret_cast<char*>(&jo::self<jo::Button>(this)->press_sound), back);
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->shown) = 1;
        return 1;
    }

    // 0x006226e0
    std::int32_t button_enabled() {
        // this: the button's Child sub-object (Button+4)
        if (jo::self<jo::Child>(this)->visible != 0 && jo::self<jo::Child>(this)->enabled != 0) return 1;
        return 0;
    }

    // 0x00623ce0 (void; eax on exit reproduced: this, or this with its low word replaced by a8)
    std::uintptr_t button_set_text(const char* text, void* a2, std::int32_t a3, std::int32_t a4,
                                  std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8) {
        c_set_text.hit();
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Button>(this)->shown)) == 0) return reinterpret_cast<std::uintptr_t>(this);  // eax only (void in the original)
        if (text != nullptr && c_strlen(text) <= 0x100u) c_strcpy(jo::self<jo::Button>(this)->caption, text);
        jo::self<jo::Button>(this)->caption_box = a2;
        jo::view<std::int32_t>(jo::self<jo::Button>(this)->caption_colour) = a3;
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->caption_330) = static_cast<std::uint16_t>(a4);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->caption_332) = static_cast<std::uint16_t>(a5);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->caption_334) = static_cast<std::uint16_t>(a6);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->press_dx) = static_cast<std::uint16_t>(a7);
        jo::view<std::uint16_t>(jo::self<jo::Button>(this)->press_dy) = static_cast<std::uint16_t>(a8);
        return (reinterpret_cast<std::uintptr_t>(this) & ~std::uintptr_t{0xffff}) | static_cast<std::uint16_t>(a8);  // eax only
    }
};

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("Widget::~Widget", 0x006211a0, &H::widget_dtor);
B9005("Button::create", 0x00621540, &H::button_create);
B9005("Button::enabled", 0x006226e0, &H::button_enabled);
B9005("Button::set_text", 0x00623ce0, &H::button_set_text);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- from modules/en3d_b87_02.cpp (game/b87_02)
namespace {
namespace en3d_b87_02 {

std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

std::uint32_t game_strlen(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
void game_strcpy(void* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(d))), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_button_setup, "MenuButton::setup");

struct H {
    // 0x006211d0
    std::int32_t button_setup(std::int16_t x, std::int16_t y, void* sprites, const char* n0, const char* n1,
                              const char* n2, const char* n3, std::uint8_t b0, std::uint8_t b1, std::uint8_t b2,
                              std::uint8_t b3, std::uint8_t b4, std::uint8_t b5) {
        c_button_setup.hit();
        if (sprites == nullptr) return 0;
        jo::self<jo::Button>(this)->sprite_set = static_cast<jo::BmpRes*>(sprites);
        void* frame = nullptr;
        if (n0 != nullptr) {
            frame = joan::orig::arg::as<void*>(joan::orig::as_bmp_res(sprites)->find(joan::orig::arg::str(n0)));  // 0x00620b40
        } else if (n1 != nullptr) {
            frame = joan::orig::arg::as<void*>(joan::orig::as_bmp_res(sprites)->find(joan::orig::arg::str(n1)));
        } else if (n2 != nullptr) {
            frame = joan::orig::arg::as<void*>(joan::orig::as_bmp_res(sprites)->find(joan::orig::arg::str(n2)));
        } else if (n3 != nullptr) {
            frame = joan::orig::arg::as<void*>(joan::orig::as_bmp_res(sprites)->find(joan::orig::arg::str(n3)));
        }
        if (frame == nullptr) return 0;
        jo::self<jo::Button>(this)->x = x;
        jo::self<jo::Button>(this)->hit_rect[0] = x;
        jo::self<jo::Button>(this)->y = y;
        jo::self<jo::Button>(this)->hit_rect[1] = y;
        jo::self<jo::Button>(this)->x2 = static_cast<std::int16_t>(add(add(x, jo::view<std::int16_t>(jo::as_bmp_entry(frame)->width)), -1));
        jo::self<jo::Button>(this)->hit_rect[2] = jo::self<jo::Button>(this)->x2;
        jo::self<jo::Button>(this)->y2 = static_cast<std::int16_t>(add(add(y, jo::view<std::int16_t>(jo::as_bmp_entry(frame)->height)), -1));
        jo::self<jo::Button>(this)->hit_rect[3] = jo::self<jo::Button>(this)->y2;
        if (n0 != nullptr && game_strlen(n0) <= 8) game_strcpy(&jo::view<std::uint8_t>(jo::self<jo::Button>(this)->states[0].sprite[0]), n0);
        if (n1 != nullptr && game_strlen(n1) <= 8) game_strcpy(&jo::view<std::uint8_t>(jo::self<jo::Button>(this)->states[1].sprite[0]), n1);
        if (n2 != nullptr && game_strlen(n2) <= 8) game_strcpy(&jo::view<std::uint8_t>(jo::self<jo::Button>(this)->states[2].sprite[0]), n2);
        if (n3 != nullptr && game_strlen(n3) <= 8) game_strcpy(&jo::view<std::uint8_t>(jo::self<jo::Button>(this)->states[3].sprite[0]), n3);
        jo::self<jo::Button>(this)->momentary = b0;
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->pressed) = b1;
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->fit) = b2;
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->enabled) = b3;
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->i8_8) = b4;
        jo::self<jo::Button>(this)->click_sound = b5;
        jo::self<jo::Button>(this)->drawn = 0;
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->shape) = 0;
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->i8_8e) = 1;
        for (std::int32_t i = 0; i < 4; ++i) jo::view<std::uint16_t>(jo::self<jo::Button>(this)->states[i].a28) = 0;
        if (n1 != nullptr && game_strlen(n1) <= 8) game_strcpy(&jo::view<std::uint8_t>(jo::self<jo::Button>(this)->help_sprite[0]), n1);
        if (game_strlen(str(d::s_MENU_BACK_1)) <= 0x20) game_strcpy(&jo::view<std::uint8_t>(jo::self<jo::Button>(this)->press_sound), str(d::s_MENU_BACK_1));  // 'MENU-BACK-1'
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->shown) = 1;
        return 1;
    }

    // 0x006217e0
    std::int32_t button_clear() {
        jo::self<jo::Button>(this)->sprite_set = nullptr;
        jo::self<jo::Button>(this)->caption_box = nullptr;
        jo::self<jo::Button>(this)->callback = 0;
        jo::self<jo::Button>(this)->u32_342 = 0;
        jo::view<std::uint8_t>(jo::self<jo::Button>(this)->shown) = 0;
        return 1;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b87_02", name, addr, fn)
GAME("MenuButton::setup", 0x006211d0, &H::button_setup);
GAME("MenuButton::clear", 0x006217e0, &H::button_clear);
#undef GAME

}  // namespace en3d_b87_02
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

using joan::orig::helpers::addr_of;

char* crt_strcpy(void* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_button_init, "Button::init");
JOAN_COUNTER(c_set_item, "Button::set_item");

struct H {

    // ------------------------------------------------------------------ 0x006216a0
    u32 button_init(u16 l, u16 t, u16 r, u16 b, u8 a5, u8 a6, u8 a7, u8 a8, u8 a9) {
        c_button_init.hit();
        jo::view<u16>(jo::self<jo::Button>(this)->x) = l;
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[0]) = l;
        jo::view<u16>(jo::self<jo::Button>(this)->y) = t;
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[1]) = t;
        jo::view<u16>(jo::self<jo::Button>(this)->x2) = r;
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[2]) = r;
        jo::view<u16>(jo::self<jo::Button>(this)->y2) = b;
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[3]) = b;
        jo::self<jo::Button>(this)->momentary = a5;
        jo::view<u8>(jo::self<jo::Button>(this)->pressed) = a6;
        jo::view<u8>(jo::self<jo::Button>(this)->fit) = a7;
        jo::view<u8>(jo::self<jo::Button>(this)->enabled) = a8;
        jo::view<u8>(jo::self<jo::Button>(this)->i8_8) = a9;
        jo::self<jo::Button>(this)->drawn = 0;
        jo::view<u8>(jo::self<jo::Button>(this)->shape) = 0;
        jo::view<u8>(jo::self<jo::Button>(this)->i8_8e) = 1;
        const char* const name = &img::global<const char>(d::s_MENU_BACK_1);  // 'MENU-BACK-1'
        if (crt_strlen(name) <= 0x20) crt_strcpy(reinterpret_cast<u8*>(&jo::self<jo::Button>(this)->press_sound), name);
        jo::view<u8>(jo::self<jo::Button>(this)->shown) = 1;
        return 1;
    }

    // ------------------------------------------------------------------ 0x00621850
    H* reset_rect() {
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[0]) = jo::view<u16>(jo::self<jo::Button>(this)->x);
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[1]) = jo::view<u16>(jo::self<jo::Button>(this)->y);
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[2]) = jo::view<u16>(jo::self<jo::Button>(this)->x2);
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[3]) = jo::view<u16>(jo::self<jo::Button>(this)->y2);
        return this;
    }

    // ------------------------------------------------------------------ 0x00623490
    std::uintptr_t set_hit_rect(i32 l, i32 t, i32 r, i32 b, u32 relative) {
        jo::view<u8>(jo::self<jo::Button>(this)->shape) = 0;
        if ((relative & 0xff) != 0) {
            jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[0]) = static_cast<u16>(static_cast<i32>(jo::self<jo::Button>(this)->x) + l);
            jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[1]) = static_cast<u16>(static_cast<i32>(jo::self<jo::Button>(this)->y) + t);
            jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[2]) = static_cast<u16>(static_cast<i32>(jo::self<jo::Button>(this)->x) + r);
            const u32 eax = static_cast<u32>(static_cast<i32>(jo::self<jo::Button>(this)->y) + b);
            jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[3]) = static_cast<u16>(eax);
            return eax;
        }
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[0]) = static_cast<u16>(l);
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[1]) = static_cast<u16>(t);
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[2]) = static_cast<u16>(r);
        jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[3]) = static_cast<u16>(b);
        return (addr_of(this) & ~std::uintptr_t{0xffff}) | (static_cast<u32>(b) & 0xffffu);
    }

    // ------------------------------------------------------------------ 0x00623a80
    std::uintptr_t set_item(u32 idx, const char* name, u32 a, u16 b, u16 c, u16 d) {
        c_set_item.hit();
        if (jo::self<jo::Button>(this)->shown == 0) return addr_of(this);  // eax = this (mov eax,[ebp-4])
        jo::ButtonState& st = jo::self<jo::Button>(this)->states[idx];
        if (name != nullptr && crt_strlen(name) <= 8) crt_strcpy(st.sprite, name);
        st.colour = a;
        jo::view<u16>(st.a20) = b;
        jo::view<u16>(st.a24) = c;
        jo::view<u16>(st.a28) = d;
        return (a & 0xffff0000u) | d;
    }

    // ------------------------------------------------------------------ 0x00623b60
    std::uintptr_t set_name(const char* name) {
        if (jo::self<jo::Button>(this)->shown == 0) return addr_of(this);
        if (name == nullptr) return addr_of(this);
        const u32 n = crt_strlen(name);
        if (n > 0x20) return n;
        return addr_of(crt_strcpy(jo::self<jo::Button>(this)->states[0].sprite, name));  // up to 0x20 bytes from states[0].sprite (as the original)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("Button::init", 0x006216a0, &H::button_init);
B("Button::reset_rect", 0x00621850, &H::reset_rect);
B("Button::set_hit_rect", 0x00623490, &H::set_hit_rect);
B("Button::set_item", 0x00623a80, &H::set_item);
B("Button::set_name", 0x00623b60, &H::set_name);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b106_18.cpp (game/b106_18)
namespace {
namespace game_b106_18 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_mouse, "UiButton::mouse");

void* const kRandom = obj(d::g_misc);
void* const kUiInput = obj(d::g_mouse);
void* const kKeys = obj(d::g_keys_a51806);
void* const kScaler = obj(d::g_sprites_a46128);
void* const kFrame = obj(d::g_help_box);
void* const kSound = obj(d::g_sfx_player);

struct H {
    // ------------------------------------------------------------ 0x006218c0
    // Hotkey / key-code match shared by the "press" paths: 0 when nothing matched.
    i32 hotkey_or_char(i32 a8, i32 ac, i32 a10, bool set70, i32 r_char) {
        const i8 hot = static_cast<i8>(a10);
        if (hot >= 0 && joan::orig::arg::as<i8>(joan::orig::as_ai(kKeys)->is_ready(static_cast<i32>(hot))) != 0) {  // 0x00597ed0
            if (set70) jo::self<jo::Button>(this)->state = 3;
            return 4;
        }
        if (global<i32>(d::g_key) != 0) {
            const i32 u = joan::orig::arg::as<i32>(joan::orig::libc::toupper(joan::orig::arg::i32(global<i32>(d::g_key))));  // toupper
            if (u != 0 &&
                (u == a8 || u == ac || u == static_cast<i32>(static_cast<u32>(jo::view<u16>(jo::self<jo::Button>(this)->hotkey))))) {
                if (set70) jo::self<jo::Button>(this)->state = global<i32>(d::g_key);
                return r_char;
            }
        }
        return 0;
    }

    i32 mouse(i32 a8, i32 ac, i32 a10, i32 a14, i32 a18, i32 a1c) {
        c_mouse.hit();
        if (jo::self<jo::Button>(this)->shown == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_button(reinterpret_cast<u8*>(this) + sizeof(void*))->enabled_slot()) == 0) return 0;  // 0x006226e0 (+4 on x86: after the leading pointer)
        if (jo::view<i8>(jo::self<jo::Button>(this)->drawn) == 0) return 0;
        i16 s8 = jo::self<jo::Button>(this)->hit_rect[0];
        i16 sc = jo::self<jo::Button>(this)->hit_rect[1];
        i16 s10 = jo::self<jo::Button>(this)->hit_rect[2];
        i16 s14 = jo::self<jo::Button>(this)->hit_rect[3];
        i16 s18 = -1;
        i16 s1c = -1;
        u32 s20 = 0x3f800000u;
        u32 s24 = 0x3f800000u;
        if (jo::self<jo::Button>(this)->shape == 0) {
            if (jo::self<jo::Button>(this)->fit != 0) {
                joan::orig::as_sprite2d(kScaler)->fit_rect(joan::orig::arg::i16(jo::self<jo::Button>(this)->hit_rect[0]), joan::orig::arg::i16(jo::self<jo::Button>(this)->hit_rect[1]), joan::orig::arg::i16(jo::self<jo::Button>(this)->hit_rect[2]), joan::orig::arg::i16(jo::self<jo::Button>(this)->hit_rect[3]), &s20, &s24, &s8, &sc, &s10, &s14, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
            }
            if (jo::self<jo::Button>(this)->i8_8e == 1) {
                joan::orig::as_help_box(kFrame)->set(static_cast<i32>(s8), static_cast<i32>(sc), static_cast<i32>(s10), static_cast<i32>(s14), joan::orig::arg::str(jo::self<jo::Button>(this)->help_sprite), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1));  // 0x00489920
            } else if (jo::self<jo::Button>(this)->i8_8e == 2) {
                joan::orig::as_help_box(kFrame)->set2(static_cast<i32>(s8), static_cast<i32>(sc), static_cast<i32>(s10), static_cast<i32>(s14), joan::orig::arg::str(jo::self<jo::Button>(this)->tooltip), joan::orig::arg::str(jo::self<jo::Button>(this)->help_text), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1));  // 0x00489b30
            }
        } else if (jo::self<jo::Button>(this)->shape == 1) {
            s8 = jo::self<jo::Button>(this)->tri[0];
            sc = jo::self<jo::Button>(this)->tri[1];
            s10 = jo::self<jo::Button>(this)->tri[2];
            s14 = jo::self<jo::Button>(this)->tri[3];
            s18 = jo::self<jo::Button>(this)->tri[4];
            s1c = jo::self<jo::Button>(this)->tri[5];
            if (jo::self<jo::Button>(this)->fit != 0) {
                joan::orig::as_sprite2d(kScaler)->fit_triangle(joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[0]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[1]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[2]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[3]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[4]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[5]), &s20, &s24, &s8, &sc, &s10, &s14, &s18, &s1c);  // 0x006340e0
            }
            if (jo::self<jo::Button>(this)->i8_8e == 1) {
                joan::orig::as_help_box(kFrame)->set(static_cast<i32>(s8), static_cast<i32>(sc), static_cast<i32>(s10), static_cast<i32>(s14), joan::orig::arg::str(jo::self<jo::Button>(this)->help_sprite), static_cast<i32>(s18), static_cast<i32>(s1c));  // 0x00489920
            } else if (jo::self<jo::Button>(this)->i8_8e == 2) {
                joan::orig::as_help_box(kFrame)->set2(static_cast<i32>(s8), static_cast<i32>(sc), static_cast<i32>(s10), static_cast<i32>(s14), joan::orig::arg::str(jo::self<jo::Button>(this)->tooltip), joan::orig::arg::str(jo::self<jo::Button>(this)->help_text), static_cast<i32>(s18), static_cast<i32>(s1c));  // 0x00489b30
            }
        }
        const i32 x0 = s8, y0 = sc, x1 = s10, y1 = s14, x2 = s18, y2 = s1c;
        // in rect (0x005945d0 plain / 0x005947c0 with the second rect), button 0 / 1
        auto in4 = [&](i32 b) { return joan::orig::arg::as<i32>(joan::orig::as_parser(kUiInput)->rect_value(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(b))); };
        auto in6 = [&](i32 b) { return joan::orig::arg::as<i32>(joan::orig::as_input(kUiInput)->click_count_in(joan::orig::arg::u32(x0), joan::orig::arg::u32(y0), joan::orig::arg::u32(x1), joan::orig::arg::u32(y1), joan::orig::arg::u32(x2), joan::orig::arg::u32(y2), joan::orig::arg::i32(b))); };
        i32 r = 0;
        if (jo::view<i8>(jo::self<jo::Button>(this)->momentary) != 0) {
            if (jo::self<jo::Button>(this)->pressed == 0) {
                if (jo::self<jo::Button>(this)->shape == 0) {
                    if (in4(0) != 0) {
                        r = 1;
                        jo::self<jo::Button>(this)->state = 0;
                    } else if (a14 != 0 && in4(1) != 0) {
                        r = 2;
                        jo::self<jo::Button>(this)->state = 1;
                    } else {
                        r = hotkey_or_char(a8, ac, a10, true, 3);
                    }
                } else if (jo::self<jo::Button>(this)->shape == 1) {
                    if (in6(0) != 0) {
                        r = 1;
                        jo::self<jo::Button>(this)->state = 0;
                    } else if (a14 != 0 && in6(1) != 0) {
                        r = 2;
                        jo::self<jo::Button>(this)->state = 1;
                    } else {
                        r = hotkey_or_char(a8, ac, a10, true, 3);
                    }
                }
                if (r != 0) {
                    jo::self<jo::Button>(this)->pressed = 1;
                    if (jo::view<i8>(jo::self<jo::Button>(this)->click_sound) != 0) joan::orig::as_sound(kSound)->play_def(str(d::s_IF_BUTTON_MOUSE_DOWN));  // 0x004ceef0
                    global<u32>(d::g_a69d14) = joan::orig::arg::as<u32>(joan::orig::as_misc(kRandom)->get_time_591050());  // 0x00591050 (time)
                }
                return 0;
            }
            i32 r30 = 0;
            if (jo::self<jo::Button>(this)->shape == 0) {
                if (jo::self<jo::Button>(this)->state == 0) {
                    if (joan::orig::arg::as<i32>(joan::orig::as_parser(kUiInput)->rect_mode2(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), 0)) != 0) {  // 0x00594900
                        r = 1;
                    } else if (joan::orig::arg::as<i32>(joan::orig::as_joystick(kUiInput)->pressed_kind(0)) != 0 ||  // 0x00594a10
                               global<i32>(d::g_mouse_left_down) == 0) {
                        r = -1;
                    } else if (joan::orig::arg::as<i32>(joan::orig::as_point(kUiInput)->inside(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1))) != 0) {  // 0x00593980
                        r30 = a1c;
                        r = r30;
                    }
                } else if (jo::self<jo::Button>(this)->state == 1) {
                    if (joan::orig::arg::as<i32>(joan::orig::as_parser(kUiInput)->rect_mode2(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), 1)) != 0) {
                        r = 2;
                    } else if (joan::orig::arg::as<i32>(joan::orig::as_joystick(kUiInput)->pressed_kind(1)) != 0 || global<i32>(d::g_a516d4) == 0) {
                        r = -2;
                    } else if (joan::orig::arg::as<i32>(joan::orig::as_point(kUiInput)->inside(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1))) != 0) {
                        r30 = a1c;
                        r = r30;
                    }
                } else if (jo::self<jo::Button>(this)->state == 3) {
                    if (joan::orig::arg::as<i8>(joan::orig::as_ai(kKeys)->is_ready(static_cast<i32>(static_cast<i8>(a10)))) == 0) r = 4;
                } else if (global<i32>(d::g_a51db5) != jo::self<jo::Button>(this)->state) {
                    r = 3;
                }
            } else if (jo::self<jo::Button>(this)->shape == 1) {
                if (jo::self<jo::Button>(this)->state == 0) {
                    if (joan::orig::arg::as<i32>(joan::orig::as_input(kUiInput)->region_hit(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), 0)) != 0) {  // 0x00594ab0
                        r = 1;
                    } else if (joan::orig::arg::as<i32>(joan::orig::as_joystick(kUiInput)->pressed_kind(0)) != 0 || global<i32>(d::g_mouse_left_down) == 0) {
                        r = -1;
                    } else if (joan::orig::arg::as<i32>(joan::orig::as_triangle(kUiInput)->contains(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::i32(global<i32>(d::g_mouse_x)), joan::orig::arg::i32(global<i32>(d::g_mouse_y)))) != 0) {  // 0x00596140
                        r30 = a1c;
                        r = r30;
                    }
                } else if (jo::self<jo::Button>(this)->state == 1) {
                    if (joan::orig::arg::as<i32>(joan::orig::as_input(kUiInput)->region_hit(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), 1)) != 0) {
                        r = 2;
                    } else if (joan::orig::arg::as<i32>(joan::orig::as_joystick(kUiInput)->pressed_kind(1)) != 0 || global<i32>(d::g_a516d4) == 0) {
                        r = -2;
                    } else if (joan::orig::arg::as<i32>(joan::orig::as_triangle(kUiInput)->contains(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::i32(global<i32>(d::g_mouse_x)), joan::orig::arg::i32(global<i32>(d::g_mouse_y)))) != 0) {
                        r30 = a1c;
                        r = r30;
                    }
                } else if (jo::self<jo::Button>(this)->state == 3) {
                    if (joan::orig::arg::as<i8>(joan::orig::as_ai(kKeys)->is_ready(static_cast<i32>(static_cast<i8>(a10)))) == 0) r = 4;
                } else if (global<i32>(d::g_a51db5) != jo::self<jo::Button>(this)->state) {
                    r = 3;
                }
            }
            if (r == 0) return r;
            jo::self<jo::Button>(this)->state = 0;
            if (r30 != 0) {
                const u32 dt = joan::orig::arg::as<u32>(joan::orig::as_misc(kRandom)->get_time_591050()) - global<u32>(d::g_a69d14);  // 0x00591050
                return dt >= 0x190u ? -1 : 0;
            }
            jo::self<jo::Button>(this)->pressed = 0;
            return r > 0 ? r : 0;
        }
        if (jo::self<jo::Button>(this)->shape == 0) {
            if (in4(0) != 0) {
                r = 1;
            } else if (a14 != 0 && in4(1) != 0) {
                r = 2;
            } else {
                r = hotkey_or_char(a8, ac, a10, false, 3);
            }
        } else if (jo::self<jo::Button>(this)->shape == 1) {
            if (in6(0) != 0) {
                r = 1;
            } else if (a14 != 0 && in6(1) != 0) {
                r = 2;
            } else {
                r = hotkey_or_char(a8, ac, a10, false, 3);
            }
        }
        if (r != 0) {
            if (jo::view<i8>(jo::self<jo::Button>(this)->click_sound) != 0) joan::orig::as_sound(kSound)->play_def(str(d::s_IF_BUTTON_MOUSE_DOWN));  // "IF_BUTTON_MOUSE_DOWN"
            if (a18 != 0) {
                jo::self<jo::Button>(this)->pressed = 1;
            } else {
                jo::self<jo::Button>(this)->pressed = jo::self<jo::Button>(this)->pressed == 0 ? 1 : 0;
            }
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_18", name, addr, fn)
B("UiButton::mouse", 0x006218c0, &H::mouse);
#undef B

}  // namespace game_b106_18
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

JOAN_COUNTER(c_btn_hit, "Button::hit_622730");

struct Volume8 {  // 8-byte by-value volume argument of 0x0059ebd0 (two dwords)
    u32 a, b;
};

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x00622730
    i32 button_hit(i32 allow_alt) {
        c_btn_hit.hit();
        if (jo::self<jo::Button>(this)->shown == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_button(&jo::self<jo::Button>(this)->child)->enabled_slot()) == 0) return 0;  // 0x006226e0
        if (jo::view<i8>(jo::self<jo::Button>(this)->drawn) == 0) return 0;
        i16 s8 = jo::self<jo::Button>(this)->hit_rect[0];
        i16 sc = jo::self<jo::Button>(this)->hit_rect[1];
        i16 s10 = jo::self<jo::Button>(this)->hit_rect[2];
        i16 s14 = jo::self<jo::Button>(this)->hit_rect[3];
        i16 s18 = -1;
        i16 s1c = -1;
        u32 f20 = 0x3f800000, f24 = 0x3f800000;
        void* const hl = obj(d::g_help_box);
        if (jo::self<jo::Button>(this)->shape == 0) {
            if (jo::self<jo::Button>(this)->fit != 0) {
                joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::self<jo::Button>(this)->hit_rect[0]), joan::orig::arg::i16(jo::self<jo::Button>(this)->hit_rect[1]), joan::orig::arg::i16(jo::self<jo::Button>(this)->hit_rect[2]), joan::orig::arg::i16(jo::self<jo::Button>(this)->hit_rect[3]), &f20, &f24, &s8, &sc, &s10, &s14, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
            }
            if (jo::self<jo::Button>(this)->i8_8e == 1) {
                joan::orig::as_help_box(hl)->set(i32{s8}, i32{sc}, i32{s10}, i32{s14}, joan::orig::arg::str(jo::self<jo::Button>(this)->help_sprite), i32{-1}, i32{-1});  // 0x00489920
            } else if (jo::self<jo::Button>(this)->i8_8e == 2) {
                joan::orig::as_help_box(hl)->set2(i32{s8}, i32{sc}, i32{s10}, i32{s14}, joan::orig::arg::str(jo::self<jo::Button>(this)->tooltip), joan::orig::arg::str(jo::self<jo::Button>(this)->help_text), i32{-1}, i32{-1});  // 0x00489b30
            }
        } else if (jo::self<jo::Button>(this)->fit != 0 && jo::self<jo::Button>(this)->shape == 1) {
            s8 = jo::self<jo::Button>(this)->tri[0];
            sc = jo::self<jo::Button>(this)->tri[1];
            s10 = jo::self<jo::Button>(this)->tri[2];
            s14 = jo::self<jo::Button>(this)->tri[3];
            s18 = jo::self<jo::Button>(this)->tri[4];
            s1c = jo::self<jo::Button>(this)->tri[5];
            if (jo::self<jo::Button>(this)->fit != 0) {
                joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_triangle(joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[0]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[1]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[2]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[3]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[4]), joan::orig::arg::i16(jo::self<jo::Button>(this)->tri[5]), &f20, &f24, &s8, &sc, &s10, &s14, &s18, &s1c);  // 0x006340e0
            }
            if (jo::self<jo::Button>(this)->i8_8e == 1) {
                joan::orig::as_help_box(hl)->set(i32{s8}, i32{sc}, i32{s10}, i32{s14}, joan::orig::arg::str(jo::self<jo::Button>(this)->help_sprite), i32{s18}, i32{s1c});  // 0x00489920
            } else if (jo::self<jo::Button>(this)->i8_8e == 2) {
                joan::orig::as_help_box(hl)->set2(i32{s8}, i32{sc}, i32{s10}, i32{s14}, joan::orig::arg::str(jo::self<jo::Button>(this)->tooltip), joan::orig::arg::str(jo::self<jo::Button>(this)->help_text), i32{s18}, i32{s1c});  // 0x00489b30
            }
        }
        i32 res = 0;
        void* const mouse = obj(d::g_mouse);
        if (jo::self<jo::Button>(this)->shape == 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_joystick(mouse)->in_rect(i32{s8}, i32{sc}, i32{s10}, i32{s14}, i32{0})) != 0) {  // 0x00594360
                res = 1;
                jo::view<u32>(jo::self<jo::Button>(this)->state) = 0;
            } else if (allow_alt != 0 &&
                       joan::orig::arg::as<i32>(joan::orig::as_joystick(mouse)->in_rect(i32{s8}, i32{sc}, i32{s10}, i32{s14}, i32{1})) != 0) {
                res = 2;
                jo::view<u32>(jo::self<jo::Button>(this)->state) = 1;
            }
        } else if (jo::self<jo::Button>(this)->shape == 1) {
            if (joan::orig::arg::as<i32>(joan::orig::as_input(mouse)->double_click_in(joan::orig::arg::u32(i32{s8}), joan::orig::arg::u32(i32{sc}), joan::orig::arg::u32(i32{s10}), joan::orig::arg::u32(i32{s14}), joan::orig::arg::u32(i32{s18}), joan::orig::arg::u32(i32{s1c}), i32{0})) !=
                0) {  // 0x00594490
                res = 1;
                jo::view<u32>(jo::self<jo::Button>(this)->state) = 0;
            } else if (allow_alt != 0 && joan::orig::arg::as<i32>(joan::orig::as_input(mouse)->double_click_in(joan::orig::arg::u32(i32{s8}), joan::orig::arg::u32(i32{sc}), joan::orig::arg::u32(i32{s10}), joan::orig::arg::u32(i32{s14}), joan::orig::arg::u32(i32{s18}), joan::orig::arg::u32(i32{s1c}), i32{1})) != 0) {
                res = 2;
                jo::view<u32>(jo::self<jo::Button>(this)->state) = 1;
            }
        }
        if (res == 0) return 0;
        jo::view<u8>(jo::self<jo::Button>(this)->pressed) = 1;
        if (jo::self<jo::Button>(this)->press_sound != 0) {
            // the sound name at +0xe3
            joan::orig::as_sound_effects(obj(d::g_sound_effects))->play(reinterpret_cast<const char*>(&jo::self<jo::Button>(this)->press_sound), Volume8{global<u32>(d::g_a67628), global<u32>(d::g_a6762c)});  // 0x0059ebd0
        }
        global<u32>(d::g_a69d10) = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050());  // 0x00591050
        return res;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Button::hit_622730", 0x00622730, &H::button_hit);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b82_11.cpp (game/b82_11)
namespace {
namespace game_b82_11 {

struct H {
    // ---------------------------------------------------------------- 0x00623360
    std::int32_t visible() { return jo::self<jo::Child>(this)->visible; }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b82_11", name, addr, fn)
B11("Button::draw", 0x00622cc0, &joan::orig::Button::draw);
B11("Child::visible", 0x00623360, &H::visible);
#undef B11

}  // namespace game_b82_11
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

JOAN_COUNTER(c_rect3, "Ctl::set_rect3");

struct H {
    // ------------------------------------------------------------------ 0x00623380
    void set_rect3(i32 a, i32 b, i32 c, i32 d, i32 e, i32 g, u32 offset) {
        c_rect3.hit();
        jo::view<u8>(jo::self<jo::Button>(this)->shape) = 1;
        if ((offset & 0xff) != 0) {
            jo::view<u16>(jo::self<jo::Button>(this)->tri[0]) = static_cast<u16>(static_cast<i32>(jo::self<jo::Button>(this)->x) + a);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[1]) = static_cast<u16>(static_cast<i32>(jo::self<jo::Button>(this)->y) + b);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[2]) = static_cast<u16>(static_cast<i32>(jo::self<jo::Button>(this)->x) + c);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[3]) = static_cast<u16>(static_cast<i32>(jo::self<jo::Button>(this)->y) + d);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[4]) = static_cast<u16>(static_cast<i32>(jo::self<jo::Button>(this)->x) + e);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[5]) = static_cast<u16>(static_cast<i32>(jo::self<jo::Button>(this)->y) + g);
        } else {
            jo::view<u16>(jo::self<jo::Button>(this)->tri[0]) = static_cast<u16>(a);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[1]) = static_cast<u16>(b);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[2]) = static_cast<u16>(c);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[3]) = static_cast<u16>(d);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[4]) = static_cast<u16>(e);
            jo::view<u16>(jo::self<jo::Button>(this)->tri[5]) = static_cast<u16>(g);
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Ctl::set_rect3", 0x00623380, &H::set_rect3);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

std::uintptr_t bits(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }

JOAN_COUNTER(c_set_rect, "Window2d::set_rect");

struct H {
    std::uintptr_t self() { return bits(this); }

    // ------------------------------------------------------------------ 0x00623580
    std::uintptr_t set_rect(u32 x, u32 y, u32 x2, u32 y2, u32 relative) {
        c_set_rect.hit();
        const i16 w = static_cast<i16>(static_cast<i32>(jo::self<jo::Button>(this)->x2) - static_cast<i32>(jo::self<jo::Button>(this)->x) + 1);
        const i16 h = static_cast<i16>(static_cast<i32>(jo::self<jo::Button>(this)->y2) - static_cast<i32>(jo::self<jo::Button>(this)->y) + 1);
        const u16 xw = static_cast<u16>(x), yw = static_cast<u16>(y);
        const u16 x2w = static_cast<u16>(x2), y2w = static_cast<u16>(y2);
        auto addw = [](i16& m, u16 v) { jo::view<u16>(m) = static_cast<u16>(jo::view<u16>(m) + v); };
        auto end = [](const i16& m, i16 size) {
            return static_cast<u16>(static_cast<i32>(m) + static_cast<i32>(size) - 1);
        };
        const i32 kind = static_cast<i32>(jo::self<jo::Button>(this)->shape);
        if ((relative & 0xff) != 0) {
            if (kind == 0) {
                addw(jo::self<jo::Button>(this)->x, xw);
                addw(jo::self<jo::Button>(this)->y, yw);
                addw(jo::self<jo::Button>(this)->hit_rect[0], xw);
                addw(jo::self<jo::Button>(this)->hit_rect[1], yw);
                if (x2 == 0xffffffffu) {
                    jo::view<u16>(jo::self<jo::Button>(this)->x2) = end(jo::self<jo::Button>(this)->x, w);
                    jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[2]) = end(jo::self<jo::Button>(this)->hit_rect[0], w);
                } else {
                    addw(jo::self<jo::Button>(this)->x2, x2w);
                    addw(jo::self<jo::Button>(this)->hit_rect[2], x2w);
                }
                if (y2 == 0xffffffffu) {
                    jo::view<u16>(jo::self<jo::Button>(this)->y2) = end(jo::self<jo::Button>(this)->y, h);
                    jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[3]) = end(jo::self<jo::Button>(this)->hit_rect[1], h);
                } else {
                    addw(jo::self<jo::Button>(this)->y2, y2w);
                    addw(jo::self<jo::Button>(this)->hit_rect[3], y2w);
                }
            } else if (kind == 1) {
                addw(jo::self<jo::Button>(this)->tri[0], xw);
                addw(jo::self<jo::Button>(this)->tri[1], yw);
                addw(jo::self<jo::Button>(this)->tri[2], xw);
                addw(jo::self<jo::Button>(this)->tri[3], yw);
                addw(jo::self<jo::Button>(this)->tri[4], xw);
                addw(jo::self<jo::Button>(this)->tri[5], yw);
                addw(jo::self<jo::Button>(this)->x, xw);
                addw(jo::self<jo::Button>(this)->y, yw);
                addw(jo::self<jo::Button>(this)->x2, x2w);
                addw(jo::self<jo::Button>(this)->y2, y2w);
            }
            return self();
        }
        const std::uintptr_t hi = self() & ~std::uintptr_t{0xffff};
        if (kind == 0) {
            jo::view<u16>(jo::self<jo::Button>(this)->x) = xw;
            jo::view<u16>(jo::self<jo::Button>(this)->y) = yw;
            jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[0]) = xw;
            jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[1]) = yw;
            if (x2 == 0xffffffffu) {
                jo::view<u16>(jo::self<jo::Button>(this)->x2) = end(jo::self<jo::Button>(this)->x, w);
                jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[2]) = end(jo::self<jo::Button>(this)->hit_rect[0], w);
            } else {
                jo::view<u16>(jo::self<jo::Button>(this)->x2) = x2w;
                jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[2]) = x2w;
            }
            if (y2 == 0xffffffffu) {
                jo::view<u16>(jo::self<jo::Button>(this)->y2) = end(jo::self<jo::Button>(this)->y, h);
                jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[3]) = end(jo::self<jo::Button>(this)->hit_rect[1], h);
                return static_cast<u32>(static_cast<i32>(h));  // movsx eax, word [ebp-0xc]
            }
            jo::view<u16>(jo::self<jo::Button>(this)->y2) = y2w;
            jo::view<u16>(jo::self<jo::Button>(this)->hit_rect[3]) = y2w;
            return hi | y2w;  // mov ax, [ebp+0x14] over eax = this
        }
        if (kind == 1) {
            const u16 dy = static_cast<u16>(y2 - y), dx = static_cast<u16>(x2 - x);
            addw(jo::self<jo::Button>(this)->tri[0], dx);
            addw(jo::self<jo::Button>(this)->tri[1], dy);
            addw(jo::self<jo::Button>(this)->tri[2], dx);
            addw(jo::self<jo::Button>(this)->tri[3], dy);
            addw(jo::self<jo::Button>(this)->tri[4], dx);
            addw(jo::self<jo::Button>(this)->tri[5], dy);
            jo::view<u16>(jo::self<jo::Button>(this)->x) = xw;
            jo::view<u16>(jo::self<jo::Button>(this)->y) = yw;
            jo::view<u16>(jo::self<jo::Button>(this)->x2) = x2w;
            jo::view<u16>(jo::self<jo::Button>(this)->y2) = y2w;
            return hi | y2w;
        }
        return static_cast<u32>(kind);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Window2d::set_rect", 0x00623580, &H::set_rect);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {


char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

struct H {
    // ------------------------------------------------------------------ 0x00623bd0
    void set_name8(const char* s) {
        if (static_cast<i32>(jo::self<jo::Button>(this)->shown) == 0) return;
        if (s != nullptr && !(crt_strlen(s) > 8)) crt_strcpy(jo::self<jo::Button>(this)->help_sprite, s);
        jo::view<u8>(jo::self<jo::Button>(this)->i8_8e) = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::set_name8", 0x00623bd0, &H::set_name8);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

JOAN_COUNTER(c_set_texts, "Widget::set_texts");

char* strncpy_(char* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }  // strncpy

struct H {

    // ------------------------------------------------------------------------ 0x00623c40
    void set_texts(const char* a, const char* b) {
        c_set_texts.hit();
        if (jo::self<jo::Button>(this)->shown == 0) return;
        if (a != nullptr) {
            strncpy_(jo::self<jo::Button>(this)->tooltip, a, 0x50);
            jo::view<u8>(jo::self<jo::Button>(this)->tooltip[80]) = 0;
        }
        if (b != nullptr) {
            strncpy_(jo::self<jo::Button>(this)->help_text, b, 0xc8);
            jo::view<u8>(jo::self<jo::Button>(this)->help_text[200]) = 0;
        }
        jo::view<u8>(jo::self<jo::Button>(this)->i8_8e) = 2;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Widget::set_texts", 0x00623c40, &H::set_texts);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b82_11.cpp (game/b82_11)
//
//  Button::draw 0x00622cc0 (registered above as a joan::orig::Button member, declared in original/ui.hpp).
//
// Display hooks: none of these addresses is in canvas.txt or 0x006c2120..0x006c27c0, and none
// of the bodies holds a patched size read. Every sprite / text call goes through the original
// thunks (0x0040807b / 0x0040641f / 0x0040920f sprite queue, 0x00403f30 / 0x00402e0f /
// 0x00405ee3 / 0x00403bb1 / 0x0040a3ee text), so canvas_hooks still sees them; only
// SpriteQueue::draw_named 0x00633500 (no canvas wrapper; its own 0x00632a70 call goes through
// the thunk) is called directly (refactor R7b).
//
// Statement-by-statement C++ versions of the MSVC 6 functions. Word / byte arguments the
// original pushes as `mov r16/r8, mem; push r32` (garbage upper bits) are passed sign- or
// zero-extended: the callees read the low word / byte only (0x00627e80 masks with 0xffff).
// x87: `fild int; fmul float` (PC24) through x87::Fpu, _ftol through x87::ftol; the atan / cos /
// sin sequence as in game_ui2.cpp (Widget::move_to), with the float_tie re-run for the
// `fst dword` of the 64-bit atan result.
namespace {
using joan::hook::x87::Fpu;

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue
constexpr img::Addr kMath = d::g_misc;     // 0x00590a20 (round) / 0x00591050 (time)
constexpr img::Addr kMouse = d::g_mouse;

JOAN_COUNTER(c_button, "Button::draw");

// fild int ; fmul float (PC24) ; fstp qword ; 0x00590a20 on 0x00a45fa8 (ret 8) ; movsx ax.
std::int32_t scaled(std::int32_t v, float s) {
    const double d = Fpu{true}.mul(static_cast<double>(v), static_cast<double>(s));
    return static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(kMath))->round(joan::orig::arg::f64(d))));  // 0x00590a20
}

// fild (byte) ; fmul float [+0x11] (PC24) ; _ftol ; low byte.
std::uint8_t scale_alpha(std::uint32_t a, float k) {
    return static_cast<std::uint8_t>(
        joan::hook::x87::ftol(Fpu{true}.mul(static_cast<double>(a), static_cast<double>(k))));
}

}  // namespace

namespace joan::orig {

// ---------------------------------------------------------------- 0x00622cc0
std::int32_t Button::draw(std::uint32_t arg, std::uint8_t flags) {
    if (shown == 0) return 0;
    c_button.hit();
    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_widget_child(&child)->visible()) == 0) {  // 0x00623360
        drawn = 0;
        return 0;
    }
    // The pad's D-pad hops between the buttons of the menu on screen (platform/input.hpp menu_widget).
    if (enabled != 0 && scale > 0.5f) joan::platform::input::menu_widget(this->x, this->y, x2, y2);
    if (callback != 0) {
        using ButtonCallback = std::int32_t(JOAN_CDECL*)(void*, std::uint32_t, std::uint32_t);
        const ButtonCallback cb = reinterpret_cast<ButtonCallback>(callback);
        if (cb(this, arg, flags & 0xff) != 0) {
            drawn = 1;
            return 1;
        }
        return 0;
    }
    const std::uint32_t fl = flags & 0xff;
    if (fl != 7 && (fl & 2) != 0) return 0;
    std::int16_t px = this->x, py = this->y, w = x2, h = y2;
    float sx = 1.0f, sy = 1.0f;  // [ebp-0x18], [ebp-0x1c]
    if (fit != 0) {
        joan::orig::as_sprite2d(obj(kSprites))->fit_rect(joan::orig::arg::i16(static_cast<std::int32_t>(this->x)), joan::orig::arg::i16(static_cast<std::int32_t>(this->y)), joan::orig::arg::i16(static_cast<std::int32_t>(x2)), joan::orig::arg::i16(static_cast<std::int32_t>(y2)), &sx, &sy, &px, &py, &w, &h, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    }
    std::int32_t draw_state = 0;  // [ebp-0x20]
    if (enabled == 0) {
        draw_state = 3;
        hover = 0;
    } else if (pressed != 0) {
        draw_state = 1;
    } else {
        draw_state = 0;
        if (shape == 0) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_point(obj(kMouse))->inside(static_cast<std::int32_t>(px), static_cast<std::int32_t>(py), static_cast<std::int32_t>(w), static_cast<std::int32_t>(h))) != 0)  // 0x00593980
                draw_state = 2;
        } else if (shape == 1) {
            std::int16_t p0 = tri[0], p1 = tri[1], p2 = tri[2];
            std::int16_t p3 = tri[3], p4 = tri[4], p5 = tri[5];
            if (fit != 0) {
                joan::orig::as_sprite2d(obj(kSprites))->fit_triangle(joan::orig::arg::i16(static_cast<std::int32_t>(tri[0])), joan::orig::arg::i16(static_cast<std::int32_t>(tri[1])), joan::orig::arg::i16(static_cast<std::int32_t>(tri[2])), joan::orig::arg::i16(static_cast<std::int32_t>(tri[3])), joan::orig::arg::i16(static_cast<std::int32_t>(tri[4])), joan::orig::arg::i16(static_cast<std::int32_t>(tri[5])), &sx, &sy, &p0, &p1, &p2, &p3, &p4, &p5);  // 0x006340e0
            }
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_triangle(obj(kMouse))->contains(static_cast<std::int32_t>(p0), static_cast<std::int32_t>(p1), static_cast<std::int32_t>(p2), static_cast<std::int32_t>(p3), static_cast<std::int32_t>(p4), static_cast<std::int32_t>(p5), joan::orig::arg::i32(global<std::uint32_t>(d::g_mouse_x)), joan::orig::arg::i32(global<std::uint32_t>(d::g_mouse_y)))) != 0)  // 0x00596140
                draw_state = 2;
        }
        if (static_cast<std::int8_t>(hover) == 0 && draw_state == 2) hover = 1;
        if (draw_state == 0) hover = 0;
    }
    if (states[draw_state].sprite[0] == 0 || sprite_set == nullptr) return 0;
    if ((fl & 1) != 0) {
        std::uint32_t colour = states[draw_state].colour;  // [ebp-0x3c], alpha byte [ebp-0x39]
        const std::uint8_t alpha = scale_alpha(colour >> 24, scale);
        colour = (colour & 0x00ffffffu) | (static_cast<std::uint32_t>(alpha) << 24);
        joan::orig::as_sprite_queue(obj(kSprites))->draw_named(px, py, arg, states[draw_state].sprite, sprite_set, colour, 3,
                                                               states[draw_state].a20, states[draw_state].a24, states[draw_state].a28, -1, -1, -1, -1, 0,
                                                               w, h);  // 0x00633500
    }
    drawn = 1;
    if (caption[0] != 0 && caption_box != nullptr && (fl & 4) != 0) {
        std::uint32_t colour = caption_colour;  // [ebp-0x40], alpha byte [ebp-0x3d]
        const std::uint8_t alpha = scale_alpha(colour >> 24, scale);
        colour = (colour & 0x00ffffffu) | (static_cast<std::uint32_t>(alpha) << 24);
        std::int32_t dy = 0, dx = 0;  // [ebp-0x50], [ebp-0x58]
        if (pressed != 0) dy = scaled(static_cast<std::int32_t>(press_dy), sy);
        if (pressed != 0) dx = scaled(static_cast<std::int32_t>(press_dx), sx);
        joan::orig::as_text_renderer(caption_box)->draw_wrapped(joan::orig::arg::u32(static_cast<std::int32_t>(px) + dx), joan::orig::arg::u32(static_cast<std::int32_t>(py) + dy), joan::orig::arg::u32(static_cast<std::int32_t>(w)), joan::orig::arg::u32(static_cast<std::int32_t>(h)), joan::orig::arg::u32(arg), joan::orig::arg::str(static_cast<const char*>(caption)), joan::orig::arg::u32(colour), 2u, 0u, joan::orig::arg::u32(static_cast<std::int32_t>(caption_330)), joan::orig::arg::u32(static_cast<std::int32_t>(caption_332)), joan::orig::arg::u32(static_cast<std::int32_t>(caption_334)), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy), 2u);  // 0x00626e00
    }
    return 1;
}

}  // namespace joan::orig

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Widget::ctor_620ee0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b101_12::H::ctor_620ee0>(this)); }  // 0x620ee0
auto joan::orig::Widget::dtor_6211a0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_05::H::widget_dtor>(this)); }  // 0x6211a0
auto joan::orig::MenuButton::setup(std::int16_t a0, std::int16_t a1, void* a2, const char* a3, const char* a4, const char* a5, const char* a6, std::uint8_t a7, std::uint8_t a8, std::uint8_t a9, std::uint8_t a10, std::uint8_t a11, std::uint8_t a12) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b87_02::H::button_setup>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)); }  // 0x6211d0
auto joan::orig::Button::create(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::uintptr_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_05::H::button_create>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)); }  // 0x621540
auto joan::orig::MenuButton::clear() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b87_02::H::button_clear>(this)); }  // 0x6217e0
auto joan::orig::UiButton::mouse(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b106_18::H::mouse>(this, a0, a1, a2, a3, a4, a5)); }  // 0x6218c0
auto joan::orig::Button::enabled_slot() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_05::H::button_enabled>(this)); }  // 0x6226e0
auto joan::orig::Button::hit_622730(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_19::H::button_hit>(this, a0)); }  // 0x622730
auto joan::orig::WidgetChild::visible() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_11::H::visible>(this)); }  // 0x623360
auto joan::orig::Button::set_hit_triangle(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::uint32_t a6) -> void { joan::bind::member<&::game_b96_06::H::set_rect3>(this, a0, a1, a2, a3, a4, a5, a6); }  // 0x623380
auto joan::orig::Button::set_rect(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_12::H::set_rect>(this, a0, a1, a2, a3, a4)); }  // 0x623580
auto joan::orig::Button::set_name8(const char* a0) -> void { joan::bind::member<&::game_b100_11::H::set_name8>(this, a0); }  // 0x623bd0
auto joan::orig::Widget::set_texts(const char* a0, const char* a1) -> void { joan::bind::member<&::game_b95_06::H::set_texts>(this, a0, a1); }  // 0x623c40
auto joan::orig::Button::set_text(const char* a0, void* a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_05::H::button_set_text>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x623ce0
