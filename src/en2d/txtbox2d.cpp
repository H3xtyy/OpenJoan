// en2d/txtbox2d.cpp: functions of the original d:\ww\en2d\txtbox2d.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cmath>
#include <limits>
#include <cstddef>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

void* const kMouse = obj(d::g_mouse);

u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }

using joan::orig::helpers::strlen_;

}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
//
//    - TxtBox2d::key_input 0x0062dd10 (escape_ok), ret 4: text box editing keys.
namespace {
namespace game_b103_12 {


void* const kHeap = obj(d::g_mem);

char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void* crt_memmove(void* d, const void* s, u32 n) { return joan::orig::arg::as<void*>(joan::orig::libc::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::mptr(s), joan::orig::arg::u32(n))); }

JOAN_COUNTER(c_key_input, "TxtBox2d::key_input");

struct H {
    // ------------------------------------------------------------------ 0x0062dd10
    char* text() { return jo::self<jo::TextBox>(this)->text; }
    i32 len() { return static_cast<i32>(crt_strlen(text())); }
    i32 sel_start() { return static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_text_box(this)->sel_min())); }  // 0x00631570
    i32 sel_end() { return static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_text_box(this)->sel_max())); }    // 0x006315d0
    // Removes the selection [a, b) and puts the caret at a.
    void cut_selection() {
        const i32 a = sel_start();
        const i32 b = sel_end();
        const u32 n = static_cast<u32>(len() - b + 1);
        crt_memmove(text() + a, text() + b, n);
        jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = static_cast<u16>(a);
        jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = static_cast<u16>(a);
    }
    u32 key_input(u32 escape_ok) {
        c_key_input.hit();
        if (jo::self<jo::TextBox>(this)->focused == 0) return 0;
        u32 key = 0;
        u32 mode = 0;  // [ebp-0xc]: only read when key != 0, and set whenever key is
        if (joan::orig::arg::as<i32>(joan::orig::as_input(kMouse)->flag_6aa()) != 0) {  // 0x00631770
            key = global<u32>(d::g_key);
            jo::view<u16>(jo::self<jo::TextBox>(this)->view_start) = 1;
            mode = global<u16>(d::g_a51d1a) & 3u;
        } else {
            const i16 count = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_key(kMouse)->repeat_count()));  // 0x00595fb0
            if (static_cast<i32>(count) > static_cast<i32>(jo::self<jo::TextBox>(this)->view_start)) {
                key = global<u32>(d::g_a51db5);
                jo::view<u16>(jo::self<jo::TextBox>(this)->view_start) = static_cast<u16>(jo::view<u16>(jo::self<jo::TextBox>(this)->view_start) + 1);
                mode = global<u16>(d::g_key_modifiers) & 3u;
            }
        }
        if (key == 0) return 0;
        const char* const file = str(d::s_d_ww_en2d_txtbox2d_cpp);  // "d:\ww\en2d\txtbox2d.cpp"
        if (key >= 0x20 && key <= 0xff) {
            if (len() == static_cast<i32>(jo::self<jo::TextBox>(this)->max_len) - 1) return key;
            if (key == 0x7e) return key;
            const i32 s = sel_start();
            const i32 e = sel_end();
            if (len() - (e - s) + 1 > static_cast<i32>(jo::self<jo::TextBox>(this)->max_len)) return key;
            char* const backup = joan::orig::arg::as<char*>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::TextBox>(this)->max_len) + 1), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a19768)) + 0x2a)));  // 0x0058e7b0
            crt_strcpy(backup, text());
            const u32 n = static_cast<u32>(len() - e + 1);
            crt_memmove(text() + s + 1, text() + e, n);
            text()[s] = static_cast<char>(key);
            const i32 width = static_cast<i32>(static_cast<i16>(
                joan::orig::arg::as<u32>(joan::orig::as_font(jo::view<void*>(jo::self<jo::TextBox>(this)->font))->measure(joan::orig::arg::str(text()), -1, 0, 0, 1.0f))));  // 0x00624f00
            if (width >= static_cast<i32>(jo::self<jo::TextBox>(this)->x2) - static_cast<i32>(jo::self<jo::TextBox>(this)->x) + 1) {
                crt_strcpy(text(), backup);
            } else {
                jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = static_cast<u16>(s + 1);
                jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = jo::view<u16>(jo::self<jo::TextBox>(this)->cursor);
            }
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(backup), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a19768)) + 0x37));  // 0x0058ed70
            return key;
        }
        if (key == 0x5300) {  // Delete
            if (joan::orig::arg::as<i32>(joan::orig::as_text_box(this)->sel_length_631630()) != 0) {  // 0x00631630
                cut_selection();
            } else if (len() > static_cast<i32>(jo::self<jo::TextBox>(this)->cursor)) {
                const u32 n = static_cast<u32>(len() - static_cast<i32>(jo::self<jo::TextBox>(this)->cursor));
                char* const src = text() + static_cast<i32>(jo::self<jo::TextBox>(this)->cursor) + 1;
                crt_memmove(text() + static_cast<i32>(jo::self<jo::TextBox>(this)->cursor), src, n);
            }
            return key;
        }
        if (key == 8) {  // Backspace
            if (joan::orig::arg::as<i32>(joan::orig::as_text_box(this)->sel_length_631630()) != 0) {  // 0x00631630
                cut_selection();
            } else if (static_cast<i32>(jo::self<jo::TextBox>(this)->cursor) > 0) {
                const u32 n = static_cast<u32>(len() - static_cast<i32>(jo::self<jo::TextBox>(this)->cursor) + 1);
                char* const src = text() + static_cast<i32>(jo::self<jo::TextBox>(this)->cursor);
                crt_memmove(text() + static_cast<i32>(jo::self<jo::TextBox>(this)->cursor) - 1, src, n);
                jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = static_cast<u16>(jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) - 1);
                jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = jo::view<u16>(jo::self<jo::TextBox>(this)->cursor);
            }
            return key;
        }
        if (key == 0x4b00) {  // Left
            if (static_cast<i32>(jo::self<jo::TextBox>(this)->cursor) > 0) jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = static_cast<u16>(jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) - 1);
            if (mode == 0) jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = jo::view<u16>(jo::self<jo::TextBox>(this)->cursor);
            return key;
        }
        if (key == 0x4d00) {  // Right
            const i32 caret = static_cast<i32>(jo::self<jo::TextBox>(this)->cursor);
            if (caret < len()) jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = static_cast<u16>(jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) + 1);
            if (mode == 0) jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = jo::view<u16>(jo::self<jo::TextBox>(this)->cursor);
            return key;
        }
        if (key == 0x4800) return key;  // Up
        if (key == 0x5000) return key;  // Down
        if (key == 0x4700) {            // Home
            jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = 0;
            if (mode == 0) jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = 0;
            return key;
        }
        if (key == 0x4f00) {  // End
            jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = static_cast<u16>(crt_strlen(text()));
            if (mode == 0) jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = jo::view<u16>(jo::self<jo::TextBox>(this)->cursor);
            return key;
        }
        if ((escape_ok & 0xff) != 0 && key == 0x1b) {
            joan::orig::as_edit(this)->clear();  // 0x0062e900
            return key;
        }
        if (key == 0xd) {
            jo::view<u8>(jo::self<jo::TextBox>(this)->focused) = 0;
            return key;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("TxtBox2d::key_input", 0x0062dd10, &H::key_input);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b100_12.cpp (game/b100_12)
//
//    - Button::update 0x0062e490 / Button2::update 0x006304f0 (ret 0): optional scaling, mouse
//      rectangle test 0x5945d0 on 0xa516b0, press / release state (+0x83 / +0x84), notify
//      0x62e9a0 / 0x630d60 with the mouse position; returns 1 when handled.
//
//  - 0x006304f0: 0x43c3f0 is called before 0x43c3c0 (argument evaluation order kept).
namespace {
namespace game_b100_12 {


void* const kScreen = obj(d::g_sprites_a46128);

i32 mouse_in_rect(i32 x1, i32 y1, i32 x2, i32 y2) {
    return joan::orig::arg::as<i32>(joan::orig::as_parser(kMouse)->rect_value(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), 0));                      // 0x005945d0
}
// 2D scale of a widget rectangle (0x00633e60 on 0xa46128, ret 0x30).
void scale_rect(i16 x, i16 y, u32 w, u32 h, float* sx, float* sy, i16* px, i16* py, i16* pw, i16* ph) {
    joan::orig::as_sprite2d(kScreen)->fit_rect(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::i16(w), joan::orig::arg::i16(h), joan::orig::arg::mptr(sx), joan::orig::arg::mptr(sy), joan::orig::arg::mptr(px), joan::orig::arg::mptr(py), joan::orig::arg::mptr(pw), joan::orig::arg::mptr(ph), joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));
}

JOAN_COUNTER(c_button_update, "Button::update");
JOAN_COUNTER(c_button2_update, "Button2::update");

struct H {
    // ------------------------------------------------------------------ 0x0062e490
    i32 button_update() {
        c_button_update.hit();
        i16 x = jo::self<jo::TextBox>(this)->x, y = jo::self<jo::TextBox>(this)->y, w = jo::self<jo::TextBox>(this)->x2;
        i16 h = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_text_box(this)->bottom()));  // 0x0062ea30
        float sx = 1.0f, sy = 1.0f;
        if (jo::self<jo::TextBox>(this)->fit != 0) {
            const u32 h2 = joan::orig::arg::as<u32>(joan::orig::as_text_box(this)->bottom());  // 0x0062ea30
            scale_rect(jo::self<jo::TextBox>(this)->x, jo::self<jo::TextBox>(this)->y, static_cast<u16>(jo::self<jo::TextBox>(this)->x2), h2, &sx, &sy, &x, &y, &w, &h);
        }
        if (jo::self<jo::TextBox>(this)->focused == 0) {
            if (mouse_in_rect(x, y, w, h) == 0) return 0;
            jo::view<u8>(jo::self<jo::TextBox>(this)->focused) = 1;
            joan::orig::as_text_box(this)->fit_length(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_table(kMouse)->at_672(0))));  // 0x0062e9a0(0x0043c3c0)
            jo::self<jo::TextBox>(this)->sel_anchor = jo::self<jo::TextBox>(this)->cursor;
            jo::self<jo::TextBox>(this)->u8_84 = 0;
            return 1;
        }
        if (jo::view<i8>(jo::self<jo::TextBox>(this)->u8_84) == 0) {
            const i32 r = mouse_in_rect(x, y, w, h);
            if (r == 1) {
                joan::orig::as_text_box(this)->fit_length(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_table(kMouse)->at_672(0))));  // 0x0062e9a0(0x0043c3c0)
                jo::self<jo::TextBox>(this)->sel_anchor = jo::self<jo::TextBox>(this)->cursor;
                jo::self<jo::TextBox>(this)->u8_84 = 1;
                return 1;
            }
            if (r > 1) {
                joan::orig::as_text_box(this)->reset_len_62e950();  // 0x0062e950
                return 1;
            }
            return 0;
        }
        if (global<i32>(d::g_mouse_left_down) == 0) jo::self<jo::TextBox>(this)->u8_84 = 0;
        joan::orig::as_text_box(this)->fit_length(joan::orig::arg::i32(global<u32>(d::g_mouse_x)));  // 0x0062e9a0
        return 1;
    }

    // ------------------------------------------------------------------ 0x006304f0
    i32 button2_update() {
        c_button2_update.hit();
        i16 x = jo::self<jo::TextBox>(this)->x, y = jo::self<jo::TextBox>(this)->y, w = jo::self<jo::TextBox>(this)->x2, h = jo::self<jo::TextBox>(this)->y2;
        float sx = 1.0f, sy = 1.0f;
        if (jo::self<jo::TextBox>(this)->fit != 0) scale_rect(jo::self<jo::TextBox>(this)->x, jo::self<jo::TextBox>(this)->y, static_cast<u16>(jo::self<jo::TextBox>(this)->x2),
                                        static_cast<u16>(jo::self<jo::TextBox>(this)->y2), &sx, &sy, &x, &y, &w, &h);
        if (jo::self<jo::TextBox>(this)->focused == 0) {
            if (mouse_in_rect(x, y, w, h) == 0) return 0;
            jo::view<u8>(jo::self<jo::TextBox>(this)->focused) = 1;
            const u32 my = joan::orig::arg::as<u32>(joan::orig::as_table(kMouse)->at_676(0));  // 0x0043c3f0
            const u32 mx = joan::orig::arg::as<u32>(joan::orig::as_table(kMouse)->at_672(0));  // 0x0043c3c0
            joan::orig::as_text_box(this)->fit_cursor(joan::orig::arg::i32(mx), joan::orig::arg::i32(my));              // 0x00630d60
            jo::self<jo::TextBox>(this)->sel_anchor = jo::self<jo::TextBox>(this)->cursor;
            jo::self<jo::TextBox>(this)->u8_84 = 0;
            return 1;
        }
        if (jo::view<i8>(jo::self<jo::TextBox>(this)->u8_84) == 0) {
            const i32 r = mouse_in_rect(x, y, w, h);
            if (r == 1) {
                const u32 my = joan::orig::arg::as<u32>(joan::orig::as_table(kMouse)->at_676(0));  // 0x0043c3f0
                const u32 mx = joan::orig::arg::as<u32>(joan::orig::as_table(kMouse)->at_672(0));  // 0x0043c3c0
                joan::orig::as_text_box(this)->fit_cursor(joan::orig::arg::i32(mx), joan::orig::arg::i32(my));              // 0x00630d60
                jo::self<jo::TextBox>(this)->sel_anchor = jo::self<jo::TextBox>(this)->cursor;
                jo::self<jo::TextBox>(this)->u8_84 = 1;
                return 1;
            }
            if (r > 1) {
                joan::orig::as_text_box(this)->reset_len_630d10();  // 0x00630d10
                return 1;
            }
            return 0;
        }
        if (global<i32>(d::g_mouse_left_down) == 0) jo::self<jo::TextBox>(this)->u8_84 = 0;
        joan::orig::as_text_box(this)->fit_cursor(joan::orig::arg::i32(global<u32>(d::g_mouse_x)), joan::orig::arg::i32(global<u32>(d::g_mouse_y)));  // 0x00630d60
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("Button::update", 0x0062e490, &H::button_update);
B("Button2::update", 0x006304f0, &H::button2_update);
#undef B

}  // namespace game_b100_12
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
//
//      - TextBox::cursor_x 0x0062e6e0 (index) -> short (alignment +0x82: 1 left, 2 centre,
//        3 right; -1 otherwise), optional 640x480 fit (+6) via fit_rect. ret 4.
//
//      - TextBox::clamp_cursor 0x0062ea70, TextBox::bottom 0x0062ea30, TextBox::sel_max
//        0x006315d0, TextBox::sel_min 0x00631570. ret 0.
namespace {
namespace game_b83_05 {

std::int16_t low16(std::int32_t v) { return static_cast<std::int16_t>(static_cast<std::uint16_t>(static_cast<std::uint32_t>(v))); }

constexpr img::Addr kSprites(d::g_sprites_a46128);
constexpr fn::Code kFitRect = fn::Sprite2D_fit_rect;      // 0x00633e60
constexpr std::uint32_t kOne = 0x3f800000u;          // 1.0f

JOAN_COUNTER(c_cursor, "TextBox::cursor_x");

struct H {
    // 0x0062e6e0. ret 4.
    std::int16_t cursor_x(std::int16_t index) {
        c_cursor.hit();
        std::int16_t a = jo::self<jo::TextBox>(this)->x;
        std::int16_t b = jo::self<jo::TextBox>(this)->y;
        std::int16_t c = jo::self<jo::TextBox>(this)->x2;
        std::int16_t d = low16(joan::orig::arg::as<std::int32_t>(joan::orig::as_text_box(this)->bottom()));  // 0x0062ea30
        std::uint32_t sx = kOne, sy = kOne;
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::TextBox>(this)->fit)) != 0) {
            const std::int16_t bottom = low16(joan::orig::arg::as<std::int32_t>(joan::orig::as_text_box(this)->bottom()));
            thiscall<std::int32_t>(kFitRect, img::obj(kSprites), jo::self<jo::TextBox>(this)->x, jo::self<jo::TextBox>(this)->y, jo::self<jo::TextBox>(this)->x2, bottom,
                                   reinterpret_cast<float*>(&sx), reinterpret_cast<float*>(&sy), &a, &b, &c, &d,
                                   static_cast<std::int16_t>(0x280), static_cast<std::int16_t>(0x1e0));
        }
        auto width = [&](std::int16_t n) {  // 0x00624f00 Font::width(text, n, 0, 0, scale)
            return static_cast<std::int32_t>(
                joan::orig::as_font(jo::view<void*>(jo::self<jo::TextBox>(this)->font))->measure(joan::orig::arg::str(jo::view<const char*>(jo::self<jo::TextBox>(this)->text)), joan::orig::arg::i16(n), 0, 0, joan::orig::arg::f32(sx)));
        };
        switch (jo::self<jo::TextBox>(this)->align) {
            case 1: {
                const std::int32_t base = a;
                return low16(base + width(index));
            }
            case 2: {
                std::int32_t base = a;
                std::int32_t room = static_cast<std::int32_t>(c) - static_cast<std::int32_t>(a);
                room -= width(-1);
                base += room / 2;
                base += width(index);
                return low16(base);
            }
            case 3: {
                std::int32_t base = c;
                base -= width(-1);
                base += width(index);
                return low16(base);
            }
            default: return -1;
        }
    }

    // 0x0062ea70. ret 0.
    void clamp_cursor() {
        const std::int16_t len = low16(joan::orig::arg::as<std::int32_t>(std::strlen(joan::orig::arg::str(jo::view<const char*>(jo::self<jo::TextBox>(this)->text)))));  // CRT strlen
        if (static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->cursor) > static_cast<std::int32_t>(len)) jo::self<jo::TextBox>(this)->cursor = len;
        if (static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->sel_anchor) > static_cast<std::int32_t>(len)) jo::self<jo::TextBox>(this)->sel_anchor = len;
    }

    // 0x0062ea30. ret 0.
    std::int32_t text_bottom() {
        return static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->y) + static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->font->glyph_height) - 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("TextBox::cursor_x", 0x0062e6e0, &H::cursor_x);
B("TextBox::clamp_cursor", 0x0062ea70, &H::clamp_cursor);
B("TextBox::bottom", 0x0062ea30, &H::text_bottom);
#undef B

}  // namespace game_b83_05
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x0062e900 Edit::clear           (thiscall, ret 0).
namespace {
namespace game_b93_03 {


struct H {
    void edit_clear() {  // 0x0062e900
        *jo::view<u8*>(jo::self<jo::TextBox>(this)->text) = 0;
        jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = 0;
        jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Edit::clear", 0x0062e900, &H::edit_clear);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - TxtBox2d::reset_len 0x0062e950 (ret 0): +0x87 = strlen(+0x70), +0x8d = 0.
namespace {
namespace game_b99_07 {


struct H {
    // ------------------------------------------------------------------ 0x0062e950
    H* reset_len() {
        jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = static_cast<u16>(joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(jo::view<const char*>(jo::self<jo::TextBox>(this)->text)))));
        jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("TxtBox2d::reset_len", 0x0062e950, &H::reset_len);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
//
//    - TxtBox2d::fit_length 0x0062e9a0 (int w), ret 4: +0x87 = strlen(+0x70), decremented
//      while > 0 and w < 0x62e6e0(len).
//
//  - Void functions whose eax is not meaningful (0x5b96a0, 0x5bfa70 - its only caller
//    0x5bfbd0 ignores eax, 0x612dd0, 0x618090, 0x62e9a0, 0x603570 - _initterm entry) return
//    void. 0x62b510 is tail-called by 0x62b5e0 (which returns its eax), so it returns this /
//    the last 0x60a740 result exactly.
namespace {
namespace game_b98_06 {


JOAN_COUNTER(c_fit_length, "TxtBox2d::fit_length");

struct H {
    // ------------------------------------------------------------------ 0x0062e9a0
    void fit_length(i32 w) {
        c_fit_length.hit();
        jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = static_cast<u16>(crt_strlen(jo::view<const char*>(jo::self<jo::TextBox>(this)->text)));
        while (jo::self<jo::TextBox>(this)->cursor > 0) {
            const i16 r = joan::orig::arg::as<i16>(joan::orig::as_text_box(this)->cursor_x_62e6e0(joan::orig::arg::i16(jo::self<jo::TextBox>(this)->cursor)));  // 0x0062e6e0
            if (w >= static_cast<i32>(r)) break;
            jo::self<jo::TextBox>(this)->cursor = static_cast<i16>(jo::self<jo::TextBox>(this)->cursor - 1);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("TxtBox2d::fit_length", 0x0062e9a0, &H::fit_length);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
//
//    - TextBox2d::init_62eaf0 0x0062eaf0 (ret 0x34, txtbox2d.cpp, no references - dead code).
namespace {
namespace game_b108_10 {


JOAN_COUNTER(c_txtbox_init, "TextBox2d::init_62eaf0");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x0062eaf0
    u32 txtbox_init(u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, jo::Font2d* a7, u32 a8, u32 a9, u32 a10, u32 a11,
                    u32 a12, u32 a13) {
        c_txtbox_init.hit();
        jo::view<u16>(jo::self<jo::TextBox>(this)->x) = static_cast<u16>(a1);
        jo::view<u16>(jo::self<jo::TextBox>(this)->y) = static_cast<u16>(a2);
        jo::view<u16>(jo::self<jo::TextBox>(this)->x2) = static_cast<u16>(a3);
        jo::view<u16>(jo::self<jo::TextBox>(this)->y2) = static_cast<u16>(a4);
        jo::self<jo::TextBox>(this)->font = a7;
        jo::view<u32>(jo::self<jo::TextBox>(this)->colour) = a8;
        jo::view<u16>(jo::self<jo::TextBox>(this)->text_7c) = static_cast<u16>(a9);
        jo::view<u16>(jo::self<jo::TextBox>(this)->text_7e) = static_cast<u16>(a10);
        jo::view<u16>(jo::self<jo::TextBox>(this)->text_80) = static_cast<u16>(a11);
        jo::self<jo::TextBox>(this)->align = static_cast<u8>(a12);
        jo::view<u8>(jo::self<jo::TextBox>(this)->fit) = static_cast<u8>(a13);
        jo::view<u8>(jo::self<jo::TextBox>(this)->focused) = 0;
        jo::self<jo::TextBox>(this)->u8_84 = 0;
        jo::view<u16>(jo::self<jo::TextBox>(this)->max_len) = static_cast<u16>(a6);
        jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = 0;
        jo::self<jo::TextBox>(this)->caret_colour = jo::view<u32>(jo::self<jo::TextBox>(this)->colour);
        jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = 0;
        jo::view<u16>(jo::self<jo::TextBox>(this)->view_start) = 0;
        jo::self<jo::TextBox>(this)->line_starts = nullptr;
        jo::view<u8>(jo::self<jo::TextBox>(this)->lines_valid) = 0;
        const i32 rows = static_cast<i32>(jo::self<jo::TextBox>(this)->y2) - static_cast<i32>(jo::self<jo::TextBox>(this)->y) + 1;
        const i32 lh = static_cast<i32>((jo::self<jo::TextBox>(this)->font)->line_height) + 2;
        jo::view<u16>(jo::self<jo::TextBox>(this)->line_count) = static_cast<u16>(rows / lh);
        jo::self<jo::TextBox>(this)->text = joan::orig::arg::as<char*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(a6)) + 1), str(d::s_d_ww_en2d_txtbox2d_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a1976c)) + 0x2f)));  // 0x0058e7b0
        return joan::orig::arg::as<u32>(joan::orig::as_text_box(me())->set_text_631450(joan::orig::arg::str(a5)));  // 0x00631450
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("TextBox2d::init_62eaf0", 0x0062eaf0, &H::txtbox_init);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
//
//    - TextBox::free 0x0062eca0 (ret 0): releases +0x91 / +0x70 via 0x0058ed70.
namespace {
namespace game_b102_12 {

using joan::orig::helpers::addr_of;

// Memory manager 0xa48838: free(ptr, file, line) 0x0058ed70.
u32 mm_free(void* p, const char* file, i32 line) {
    return thiscall<u32>(fn::del, obj(d::g_mem), p, file, line);  // 0x0058ed70
}

struct H {
    // ------------------------------------------------------------------ 0x0062eca0
    std::uintptr_t textbox_free() {
        const char* const file = &img::global<const char>(d::s_d_ww_en2d_txtbox2d_cpp);  // 'd:\ww\en2d\txtbox2d.cpp'
        if (jo::self<jo::TextBox>(this)->line_starts != nullptr) {
            mm_free(jo::view<void*>(jo::self<jo::TextBox>(this)->line_starts), file, static_cast<i32>(global<i16>(d::g_a19770)) + 3);
            jo::self<jo::TextBox>(this)->line_starts = nullptr;
            jo::view<u8>(jo::self<jo::TextBox>(this)->lines_valid) = 0;
        }
        if (jo::self<jo::TextBox>(this)->text != nullptr) {
            const u32 r = mm_free(jo::view<void*>(jo::self<jo::TextBox>(this)->text), file, static_cast<i32>(global<i16>(d::g_a19770)) + 10);
            jo::self<jo::TextBox>(this)->text = nullptr;
            return r;
        }
        return addr_of(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("TextBox::free", 0x0062eca0, &H::textbox_free);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b107_06.cpp (game/b107_06)
//
//    - TextBox2d::draw 0x0062ed60 (thiscall, 4 args, ret 0x10; en2d/txtbox2d.cpp): DEAD CODE
//      (no call sites, no data references to it or its thunk 0x00404c0a); eax is stale in the
//      original, returned as void. Text lines (modes 1..3 of +0x82 & 0xf: 0x006255d0 /
//      0x00626130 / 0x00626080) and the cursor / selection rectangles (0x00632100).
namespace {
namespace game_b107_06 {

using joan::hook::x87::Fpu;


JOAN_COUNTER(c_txtbox_draw, "TextBox2d::draw");

struct H {
    void txtbox_draw(u32 surf, u32 lines_flag, u32 cursor_flag, u32 flags);
};

// ---- 0x0062ed60 ----
i16 scale_round(i32 v, float scale) {
    const double d = Fpu{true}.mul(static_cast<double>(v), static_cast<double>(scale));  // fild; fmul float
    return joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(d)));                                // 0x00590a20
}
i16 col_of(void* self, i16 pos) { return joan::orig::arg::as<i16>(joan::orig::as_text_box(self)->cursor_x_630770(joan::orig::arg::i16(pos))); }  // 0x00630770
i16 row_of(void* self, i16 pos) { return joan::orig::arg::as<i16>(joan::orig::as_text_box(self)->line_y(joan::orig::arg::i16(pos))); }  // 0x00630ab0
i32 font_height(void* self, float sy) { return scale_round(jo::as_text_box(self)->font->glyph_height, sy); }
i16 line_pos(void* self, i32 k) { return jo::view<i16*>(jo::as_text_box(self)->line_starts)[k]; }
char* text_of(void* self) { return jo::as_text_box(self)->text; }

// 0x00632100 on 0xa46128 (ret 0x1c): rectangle (x1, y1) .. (x2, y2) with colour +0x89.
void rect(i16 x1, i32 y1, i16 x2, i32 y2, u32 surf, u32 c1, u32 c2) {
    joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue_line(joan::orig::arg::i16(x1), joan::orig::arg::i16(y1), joan::orig::arg::i16(x2), joan::orig::arg::i16(y2), joan::orig::arg::i32(surf), joan::orig::arg::u32(c1), joan::orig::arg::u32(c2));
}
// Selection rectangle with two height scales; the original evaluates (in push order) +0x89 twice,
// then the height for y2, then the height for y1.
void sel_rect(void* self, u32 surf, float sy, i32 x1, i32 y, i32 x2) {
    const u32 c2 = jo::as_text_box(self)->caret_colour;
    const u32 c1 = jo::as_text_box(self)->caret_colour;
    const i32 h_y2 = font_height(self, sy);
    const i32 h_y1 = font_height(self, sy);
    rect(static_cast<i16>(x1), y + h_y1 - 1, static_cast<i16>(x2), y + h_y2 - 1, surf, c1, c2);
}

void H::txtbox_draw(u32 surf, u32 lines_flag, u32 cursor_flag, u32 flags) {
    c_txtbox_draw.hit();
    void* const self = this;
    if (joan::orig::arg::as<i32>(joan::orig::as_widget_child(reinterpret_cast<u8*>(this) + sizeof(void*))->visible()) == 0) return;  // 0x00623360 (+4 on x86: after the leading pointer)
    const u32 f = flags & 0xff;
    if (f != 7 && (f & 1) != 0) return;

    i16 x0 = jo::self<jo::TextBox>(this)->x;
    i16 y0 = jo::self<jo::TextBox>(this)->y;
    i16 x1 = jo::self<jo::TextBox>(this)->x2;
    i16 y1 = jo::self<jo::TextBox>(this)->y2;
    float sx = 1.0f;
    float sy = 1.0f;
    if (jo::self<jo::TextBox>(this)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::self<jo::TextBox>(this)->x), joan::orig::arg::i16(jo::self<jo::TextBox>(this)->y), joan::orig::arg::i16(jo::self<jo::TextBox>(this)->x2), joan::orig::arg::i16(jo::self<jo::TextBox>(this)->y2), &sx, &sy, &x0, &y0, &x1, &y1, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    }
    joan::orig::as_edit(this)->clamp_cursor();  // 0x006313d0
    const i32 lh = scale_round(static_cast<i32>(jo::self<jo::TextBox>(this)->font->line_height) + 2, sy);

    if ((lines_flag & 0xff) != 0 && (f & 4) != 0) {
        i32 y = row_of(this, 0);
        const u32 mode = jo::self<jo::TextBox>(this)->align & 0xfu;
        if (mode >= 1 && mode <= 3) {
            auto emit = [&](const char* p) {
                void* const font = jo::view<void*>(jo::as_text_box(self)->font);
                const u32 c78 = jo::view<u32>(jo::as_text_box(self)->colour);
                const i16 c7c = jo::as_text_box(self)->text_7c;
                const i16 c7e = jo::as_text_box(self)->text_7e;
                const i16 c80 = jo::as_text_box(self)->text_80;
                if (mode == 1) {
                    joan::orig::as_text(font)->draw_line(joan::orig::arg::i32(x0), joan::orig::arg::i32(static_cast<i16>(y)), joan::orig::arg::u32(surf), p, joan::orig::arg::u32(c78), joan::orig::arg::i32(x1), 0, joan::orig::arg::i32(c7c), joan::orig::arg::i32(c7e), joan::orig::arg::i32(c80), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x006255d0
                } else if (mode == 2) {
                    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(x0), joan::orig::arg::i32(static_cast<i16>(y)), joan::orig::arg::i32(x1), joan::orig::arg::i32(y + lh - 1), joan::orig::arg::u32(surf), p, joan::orig::arg::u32(c78), 0, joan::orig::arg::i32(c7c), joan::orig::arg::i32(c7e), joan::orig::arg::i32(c80), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x00626130
                } else {
                    joan::orig::as_text(font)->draw_2(joan::orig::arg::i32(x1), joan::orig::arg::i32(static_cast<i16>(y)), joan::orig::arg::u32(surf), p, joan::orig::arg::u32(c78), 0, joan::orig::arg::i32(c7c), joan::orig::arg::i32(c7e), joan::orig::arg::i32(c80), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x00626080
                }
            };
            const char* p = text_of(this);
            for (i32 i = 0; i < static_cast<i32>(jo::self<jo::TextBox>(this)->lines_valid); ++i) {
                const char c0 = text_of(this)[line_pos(this, i)];
                text_of(this)[line_pos(this, i)] = 0;
                const char c1 = text_of(this)[line_pos(this, i) - 1];
                if (text_of(this)[line_pos(this, i) - 1] == 10) text_of(this)[line_pos(this, i) - 1] = 0;
                emit(p);
                text_of(this)[line_pos(this, i)] = c0;
                text_of(this)[line_pos(this, i) - 1] = c1;
                y += lh;
                p = text_of(this) + line_pos(this, i);
            }
            emit(p);
        }
    }

    if ((f & 2) == 0) return;
    if (jo::self<jo::TextBox>(this)->focused == 0) return;
    const i32 now = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050());  // 0x00591050
    const i32 d = now - global<i32>(d::g_a69f2c);
    if (!(d < 400 && d > 0)) global<i32>(d::g_a69f2c) = now;
    if (d >= 200 && (cursor_flag & 0xff) != 0) {
        const i32 cx = col_of(this, jo::self<jo::TextBox>(this)->cursor);
        const i32 cy = row_of(this, jo::self<jo::TextBox>(this)->cursor);
        if (cx <= x1 && cx >= x0) {
            const u32 c2 = jo::self<jo::TextBox>(this)->caret_colour;
            const u32 c1 = jo::self<jo::TextBox>(this)->caret_colour;
            const i32 h = font_height(this, sy);
            rect(static_cast<i16>(cx), cy, static_cast<i16>(cx), cy + h - 1, surf, c1, c2);
        }
    }

    const i32 s = joan::orig::arg::as<i16>(joan::orig::as_edit(this)->sel_min());  // 0x00631670
    const i32 e = joan::orig::arg::as<i16>(joan::orig::as_edit(this)->sel_max());  // 0x006316d0
    if (s >= e) return;
    const i32 sx_col = col_of(this, static_cast<i16>(s));
    const i32 s_row = row_of(this, static_cast<i16>(s));
    const i32 e_col = col_of(this, static_cast<i16>(e));
    const i32 e_row = row_of(this, static_cast<i16>(e));
    i32 px = sx_col;
    i32 py = row_of(this, 0);
    i32 k = 0;
    for (; py < s_row; ++k) py = row_of(this, line_pos(this, k));
    while (py < e_row) {
        const i32 ex = col_of(this, static_cast<i16>(static_cast<i32>(line_pos(this, k)) - 1));
        if (px < ex) sel_rect(this, surf, sy, px, py, ex);
        px = col_of(this, line_pos(this, k));
        py = row_of(this, line_pos(this, k));
        ++k;
    }
    sel_rect(this, surf, sy, px, py, e_col);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_06", name, addr, fn)
B("TextBox2d::draw", 0x0062ed60, &H::txtbox_draw);
#undef B

}  // namespace game_b107_06
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
//
//    - TextBox::step_62fab0 0x0062fab0 (ret 4, no references: dead).
namespace {
namespace game_b108_11 {


JOAN_COUNTER(c_textbox, "TextBox::step_62fab0");

struct H {
    // ------------------------------------------------------------------ 0x0062fab0 (dead)
    u32 textbox_step(u32 flag) {
        c_textbox.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_button(&jo::self<jo::TextBox>(this)->child)->enabled_slot()) == 0) return 0;  // 0x006226e0
        joan::orig::as_edit(this)->clamp_cursor();                               // 0x006313d0
        const u16 s87 = jo::view<u16>(jo::self<jo::TextBox>(this)->cursor);
        const u16 s8d = jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor);
        const void* const file = str(d::s_d_ww_en2d_txtbox2d_cpp);
        const i32 line1 = static_cast<i32>(global<i16>(d::g_a19774)) + 0xd;
        const u32 len = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(jo::self<jo::TextBox>(this)->text)));  // strlen
        void* copy = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(len + 1u), joan::orig::arg::str(file), joan::orig::arg::i32(line1));  // 0x0058e7b0
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(copy)), joan::orig::arg::str(jo::self<jo::TextBox>(this)->text));  // strcpy
        const u32 r14 = joan::orig::arg::as<u32>(joan::orig::as_text_box(this)->key_62fc60(joan::orig::arg::u32(static_cast<u32>(static_cast<u8>(flag)))));  // 0x0062fc60
        if (!(static_cast<i32>(jo::self<jo::TextBox>(this)->lines_valid) < static_cast<i32>(jo::self<jo::TextBox>(this)->line_count))) {
            jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = s87;
            jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = s8d;
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::TextBox>(this)->text)), joan::orig::arg::str(copy));
            joan::orig::as_text_box(this)->rebuild_lines();  // 0x00630f50
        }
        const i32 line2 = static_cast<i32>(global<i16>(d::g_a19774)) + 0x1d;
        joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(copy), joan::orig::arg::str(file), joan::orig::arg::i32(line2));  // 0x0058ed70
        copy = nullptr;
        const u32 r = r14 != 0 ? r14 : joan::orig::arg::as<u32>(joan::orig::as_text_box(this)->update_mouse());  // 0x006304f0
        if (r != 0) {
            global<i32>(d::g_a69f2c) = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050()) - 0xc8;  // 0x00591050
        }
        return r;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("TextBox::step_62fab0", 0x0062fab0, &H::textbox_step);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b109_14.cpp (game/b109_14)
//
//    - TextBox::key_62fc60 0x0062fc60 (thiscall(char allow_esc), ret 4, en2d/txtbox2d.cpp):
//      keyboard input of an edit box (insert, Delete 0x5300, Backspace 8, cursor keys 0x4b00 /
//      0x4d00 / 0x4800 / 0x5000 / 0x4700 / 0x4f00, Escape 0x1b).
//
//  - 0x0062fc60: the cursor-mode local (ebp-0xc) is only assigned when a key code is taken; it is
//    only read when that code is non-zero, so its initial value is dead (0 here).
namespace {
namespace game_b109_14 {

i32 sx16(i16 v) { return static_cast<i32>(v); }

JOAN_COUNTER(c_key, "TextBox::key_62fc60");

// ------------------------------------------------------------------ 0x0062fc60 helpers
char* tb_text(const void* tb) { return jo::as_text_box(tb)->text; }
i32 tb_len(const void* tb) { return static_cast<i32>(joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(tb_text(tb))))); }  // strlen
void tb_move(char* dst, const char* src, i32 n) { joan::orig::libc::memcpy(joan::orig::arg::mptr(dst), joan::orig::arg::mptr(src), joan::orig::arg::u32(n)); }

struct H {
    // -------------------------------------------------------------- 0x0062fc60
    void sync_mark(u32 mode) {
        if (mode == 0) jo::self<jo::TextBox>(this)->sel_anchor = jo::self<jo::TextBox>(this)->cursor;
    }
    void delete_selection() {
        const i32 s = sx16(joan::orig::arg::as<i16>(joan::orig::as_edit(this)->sel_min()));  // 0x00631670
        const i32 e = sx16(joan::orig::arg::as<i16>(joan::orig::as_edit(this)->sel_max()));  // 0x006316d0
        const i32 n = tb_len(this) - e + 1;
        tb_move(tb_text(this) + s, tb_text(this) + e, n);
        jo::self<jo::TextBox>(this)->cursor = static_cast<i16>(s);
        jo::self<jo::TextBox>(this)->sel_anchor = static_cast<i16>(s);
    }
    u32 key(u32 allow_esc) {
        c_key.hit();
        if (jo::self<jo::TextBox>(this)->focused == 0) return 0;
        u32 code = 0;
        i16 x = jo::self<jo::TextBox>(this)->x;
        i16 y = jo::self<jo::TextBox>(this)->y;
        i16 w = jo::self<jo::TextBox>(this)->x2;
        i16 h = jo::self<jo::TextBox>(this)->y2;
        float fx = 1.0f;
        float fy = 1.0f;
        if (jo::self<jo::TextBox>(this)->fit != 0) {
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(// 0x00633e60
                          sx16(jo::self<jo::TextBox>(this)->x)), joan::orig::arg::i16(sx16(jo::self<jo::TextBox>(this)->y)), joan::orig::arg::i16(sx16(jo::self<jo::TextBox>(this)->x2)), joan::orig::arg::i16(sx16(jo::self<jo::TextBox>(this)->y2)), &fx, &fy, &x, &y, &w, &h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
        }
        u32 mode = 0;  // dead unless a code was taken (see notes)
        if (joan::orig::arg::as<i32>(joan::orig::as_input(obj(d::g_mouse))->flag_6aa()) != 0) {  // 0x00631770
            code = global<u32>(d::g_key);
            jo::self<jo::TextBox>(this)->view_start = 1;
            mode = static_cast<u32>(global<u16>(d::g_a51d1a)) & 3;
        } else {
            const i16 r = joan::orig::arg::as<i16>(joan::orig::as_key(obj(d::g_mouse))->repeat_count());  // 0x00595fb0
            if (sx16(r) > sx16(jo::self<jo::TextBox>(this)->view_start)) {
                code = global<u32>(d::g_a51db5);
                jo::self<jo::TextBox>(this)->view_start = static_cast<i16>(jo::self<jo::TextBox>(this)->view_start + 1);
                mode = static_cast<u32>(global<u16>(d::g_key_modifiers)) & 3;
            }
        }
        if (code == 0) return 0;
        if ((code >= 0x20 && code <= 0xff) || code == 0xd || code == 0xa) {
            if (code == 0xd) {
                if (!(static_cast<i32>(jo::self<jo::TextBox>(this)->lines_valid) < sx16(jo::self<jo::TextBox>(this)->line_count) - 1)) return 0;
                code = 0xa;
            }
            const i32 s = sx16(joan::orig::arg::as<i16>(joan::orig::as_edit(this)->sel_min()));  // 0x00631670
            const i32 e = sx16(joan::orig::arg::as<i16>(joan::orig::as_edit(this)->sel_max()));  // 0x006316d0
            if (tb_len(this) - (e - s) + 1 > sx16(jo::self<jo::TextBox>(this)->max_len)) return code;
            const i32 n = tb_len(this) - e + 1;
            tb_move(tb_text(this) + s + 1, tb_text(this) + e, n);
            tb_text(this)[s] = static_cast<char>(static_cast<u8>(code));
            jo::self<jo::TextBox>(this)->cursor = static_cast<i16>(s + 1);
            jo::self<jo::TextBox>(this)->sel_anchor = jo::self<jo::TextBox>(this)->cursor;
            joan::orig::as_text_box(this)->rebuild_lines();  // 0x00630f50
            return code;
        }
        if (code == 0x5300) {
            if (joan::orig::arg::as<i32>(joan::orig::as_text_box(this)->sel_length_631730()) != 0) {  // 0x00631730
                delete_selection();
            } else if (tb_len(this) > sx16(jo::self<jo::TextBox>(this)->cursor)) {
                const i32 n = tb_len(this) - sx16(jo::self<jo::TextBox>(this)->cursor);
                const i32 p = sx16(jo::self<jo::TextBox>(this)->cursor);
                tb_move(tb_text(this) + p, tb_text(this) + p + 1, n);
            }
            joan::orig::as_text_box(this)->rebuild_lines();  // 0x00630f50
            return code;
        }
        if (code == 8) {
            if (joan::orig::arg::as<i32>(joan::orig::as_text_box(this)->sel_length_631730()) != 0) {  // 0x00631730
                delete_selection();
            } else if (sx16(jo::self<jo::TextBox>(this)->cursor) > 0) {
                const i32 n = tb_len(this) - sx16(jo::self<jo::TextBox>(this)->cursor) + 1;
                const i32 p = sx16(jo::self<jo::TextBox>(this)->cursor);
                tb_move(tb_text(this) + p - 1, tb_text(this) + p, n);
                jo::self<jo::TextBox>(this)->cursor = static_cast<i16>(jo::self<jo::TextBox>(this)->cursor - 1);
                jo::self<jo::TextBox>(this)->sel_anchor = jo::self<jo::TextBox>(this)->cursor;
            }
            joan::orig::as_text_box(this)->rebuild_lines();  // 0x00630f50
            return code;
        }
        if (code == 0x4b00) {
            if (sx16(jo::self<jo::TextBox>(this)->cursor) > 0) jo::self<jo::TextBox>(this)->cursor = static_cast<i16>(jo::self<jo::TextBox>(this)->cursor - 1);
            sync_mark(mode);
            return code;
        }
        if (code == 0x4d00) {
            const i32 p = sx16(jo::self<jo::TextBox>(this)->cursor);
            if (p < tb_len(this)) jo::self<jo::TextBox>(this)->cursor = static_cast<i16>(jo::self<jo::TextBox>(this)->cursor + 1);
            sync_mark(mode);
            return code;
        }
        if (code == 0x4800 || code == 0x5000) {
            const i32 line_y = sx16(joan::orig::arg::as<i16>(joan::orig::as_text_box(this)->line_y(joan::orig::arg::i16(sx16(jo::self<jo::TextBox>(this)->cursor)))));  // 0x00630ab0
            const i32 lh = sx16((jo::self<jo::TextBox>(this)->font)->line_height);
            const i32 ty = code == 0x4800 ? line_y - (lh + 2) : line_y + lh + 2;
            const i32 cx = sx16(joan::orig::arg::as<i16>(joan::orig::as_text_box(this)->cursor_x_630770(joan::orig::arg::i16(sx16(jo::self<jo::TextBox>(this)->cursor)))));  // 0x00630770
            joan::orig::as_text_box(this)->fit_cursor(joan::orig::arg::i32(cx), joan::orig::arg::i32(ty));                                           // 0x00630d60
            sync_mark(mode);
            return 0;
        }
        if (code == 0x4700 || code == 0x4f00) {
            const i32 line_y = sx16(joan::orig::arg::as<i16>(joan::orig::as_text_box(this)->line_y(joan::orig::arg::i16(sx16(jo::self<jo::TextBox>(this)->cursor)))));  // 0x00630ab0
            joan::orig::as_text_box(this)->fit_cursor(joan::orig::arg::i32(code == 0x4700 ? sx16(x) : sx16(w)), joan::orig::arg::i32(line_y));         // 0x00630d60
            sync_mark(mode);
            return code;
        }
        if ((allow_esc & 0xff) != 0 && code == 0x1b) {
            joan::orig::as_text_box(this)->clear();  // 0x00630cc0
            joan::orig::as_text_box(this)->rebuild_lines();  // 0x00630f50
            return code;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_14", name, addr, fn)
B("TextBox::key_62fc60", 0x0062fc60, &H::key);
#undef B

}  // namespace game_b109_14
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
//
//  * module game/b91_03:
//    TextCursor::advanced 0x0058c120, to_lower 0x0058e330, empty methods 0x0058e720 /
//    0x0059d1b0, Stream::forward 0x005939f0, Joystick::in_rect 0x00594360,
//    Joystick::pressed_kind 0x00594a10, Key::repeat_count 0x00595fb0, word getters
//    0x005979a0 / 0x005979c0, Pad::axis 0x00597c80, Pad::axis_range 0x00597e00,
//    same16 0x00598700, Pair::clear 0x005a1730, Bitmap::address 0x005a93b0,
//    MusicStream::release 0x005a98a0, MusicStream::read_header 0x005a9ad0,
//    MusicStream::reset 0x005a9b40, MusicStream::rewind 0x005aa040, Actor::set_byte_fd
//    0x005ad000, Table::get 0x005af330, Table::missing 0x005af360, AnimDef::lookup_word
//    0x005b4a00, Names::get 0x005ba910, Rect::set 0x005bed90 / 0x00612ee0, Array::at1
//    0x005ea410, RenderTarget::create_depth 0x006032d0, RenderTarget::release_depth
//    0x00603380, z_plane_hit 0x0060e6a0, BitWriter::put 0x0060ff70, BitWriter::drop
//    0x006100a0, Viewport::set_int 0x00614110, Sound3d::set 0x00616a90, getter 0x00616b60,
//    Dialog::reset_name 0x00616f80, base dtor bodies 0x00628ad0 / 0x0062c720,
//    TextBox::init 0x0062b8d0, Scroll::set 0x0062be10 / 0x0062cd40, Scroll::at_end
//    0x0062c040 / 0x0062cf70, Scroll::set_end 0x0062ced0, TextBox::cursor_x 0x00630770,
//    TextBox::line_y 0x00630ab0, getter 0x00631770, Table16::entry 0x00638150.
namespace {
namespace game_b91_03 {


constexpr img::Addr kClock(d::g_misc);
constexpr img::Addr kScreen(d::g_sprites_a46128);

JOAN_COUNTER(c_cursor_x, "TextBox::cursor_x");
JOAN_COUNTER(c_line_y, "TextBox::line_y");

struct H {
    // Line index of a text position: first i with pos < breaks[i] (+0x91 shorts, count +0x95).
    std::int32_t line_of(std::int16_t pos) {
        std::int32_t i = 0;
        for (; i < jo::self<jo::TextBox>(this)->lines_valid; ++i) {
            if (pos < jo::self<jo::TextBox>(this)->line_starts[i].value) break;
        }
        return i;
    }
    void scale_box(std::int16_t* x, std::int16_t* y, std::int16_t* x2, std::int16_t* y2, float* sx, float* sy) {
        joan::orig::as_sprite2d(img::obj(kScreen))->fit_rect(joan::orig::arg::i16(jo::self<jo::TextBox>(this)->x), joan::orig::arg::i16(jo::self<jo::TextBox>(this)->y), joan::orig::arg::i16(jo::self<jo::TextBox>(this)->x2), joan::orig::arg::i16(jo::self<jo::TextBox>(this)->y2), joan::orig::arg::mptr(sx), joan::orig::arg::mptr(sy), joan::orig::arg::mptr(x), joan::orig::arg::mptr(y), joan::orig::arg::mptr(x2), joan::orig::arg::mptr(y2), joan::orig::arg::i16(static_cast<std::int16_t>(0x280)), joan::orig::arg::i16(static_cast<std::int16_t>(0x1e0)));  // 0x00633e60
    }
    std::int16_t text_width(char* text, std::int16_t n, std::uint32_t scale_bits) {
        return joan::orig::as_font(jo::view<void*>(jo::self<jo::TextBox>(this)->font))->measure(joan::orig::arg::str(text), joan::orig::arg::i16(n), 0, 0, joan::orig::arg::f32(scale_bits));  // 0x00624f00
    }

    // 0x00630770 x of the cursor at text position pos (alignment +0x82 low nibble).
    std::int16_t cursor_x(std::int16_t pos) {
        c_cursor_x.hit();
        char* line = jo::self<jo::TextBox>(this)->text;
        std::int32_t i = 0;
        for (; i < jo::self<jo::TextBox>(this)->lines_valid; ++i) {
            const std::int16_t brk = jo::self<jo::TextBox>(this)->line_starts[i].value;
            if (pos < brk) break;
            line = jo::self<jo::TextBox>(this)->text + brk;
        }
        char saved = 0;  // uninitialised in the original, only used when i < count
        if (i < jo::self<jo::TextBox>(this)->lines_valid) {
            const std::int32_t brk = jo::self<jo::TextBox>(this)->line_starts[i].value;
            saved = jo::self<jo::TextBox>(this)->text[brk];
            jo::self<jo::TextBox>(this)->text[brk] = 0;
        }
        std::int16_t result = -1;
        const std::int32_t n = static_cast<std::int32_t>(pos) - static_cast<std::int32_t>(line - jo::self<jo::TextBox>(this)->text);
        std::int16_t x = jo::self<jo::TextBox>(this)->x, y = jo::self<jo::TextBox>(this)->y, x2 = jo::self<jo::TextBox>(this)->x2, y2 = jo::self<jo::TextBox>(this)->y2;
        float sx = 1.0f, sy = 1.0f;
        if (jo::self<jo::TextBox>(this)->fit != 0) scale_box(&x, &y, &x2, &y2, &sx, &sy);
        std::uint32_t sx_bits;
        std::memcpy(&sx_bits, &sx, 4);
        const std::int16_t n16 = static_cast<std::int16_t>(n);
        switch (jo::self<jo::TextBox>(this)->align & 0xf) {
        case 1: {
            const std::int32_t base = x;
            result = static_cast<std::int16_t>(base + text_width(line, n16, sx_bits));
            break;
        }
        case 2: {
            std::int32_t esi = x;
            std::int32_t edi = static_cast<std::int32_t>(x2) - static_cast<std::int32_t>(x);
            edi -= text_width(line, -1, sx_bits);
            esi += edi / 2;
            esi += text_width(line, n16, sx_bits);
            result = static_cast<std::int16_t>(esi);
            break;
        }
        case 3: {
            std::int32_t esi = x2;
            esi -= text_width(line, -1, sx_bits);
            esi += text_width(line, n16, sx_bits);
            result = static_cast<std::int16_t>(esi);
            break;
        }
        default:
            break;
        }
        if (i < jo::self<jo::TextBox>(this)->lines_valid) {
            const std::int32_t brk = jo::self<jo::TextBox>(this)->line_starts[i].value;
            jo::self<jo::TextBox>(this)->text[brk] = saved;
        }
        return result;
    }

    // 0x00630ab0 y of the line holding text position pos (vertical alignment high nibble).
    std::int16_t line_y(std::int16_t pos) {
        c_line_y.hit();
        const std::int32_t i = line_of(pos);
        std::int16_t x = jo::self<jo::TextBox>(this)->x, y = jo::self<jo::TextBox>(this)->y, x2 = jo::self<jo::TextBox>(this)->x2, y2 = jo::self<jo::TextBox>(this)->y2;
        float sx = 1.0f, sy = 1.0f;
        if (jo::self<jo::TextBox>(this)->fit != 0) scale_box(&x, &y, &x2, &y2, &sx, &sy);
        // fild (short +0x34 + 2), fmul sy: exact int, one float-rounded product.
        const std::int32_t font_h = static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->font->line_height) + 2;
        const double scaled = static_cast<double>(static_cast<float>(font_h) * sy);
        const std::int32_t h = joan::orig::arg::as<std::int16_t>(joan::orig::as_misc(img::obj(kClock))->round(joan::orig::arg::f64(scaled)));  // 0x00590a20 round
        const std::int32_t count = jo::self<jo::TextBox>(this)->lines_valid;
        switch (jo::self<jo::TextBox>(this)->align & 0xf0) {
        case 0x10:
            return static_cast<std::int16_t>(static_cast<std::int32_t>(y) + h * i);
        case 0x20: {
            std::int32_t e = static_cast<std::int32_t>(y2) - static_cast<std::int32_t>(y) - h * (count + 1);
            return static_cast<std::int16_t>(static_cast<std::int32_t>(y) + e / 2 + h * i);
        }
        case 0x30:
            return static_cast<std::int16_t>(static_cast<std::int32_t>(y2) - h * (count + 1 - i));
        default:
            return -1;
        }
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("TextBox::cursor_x", 0x00630770, &H::cursor_x);
B91("TextBox::line_y", 0x00630ab0, &H::line_y);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
//
//  * Gadget::set_rect 0x00628b00 (virtual +0 init), Gadget::scroll_pos 0x0062c250,
//    TextBox::clear 0x00630cc0, StringTable::size 0x00638d50, Obj::byte0 0x0063e320.
//
//  * 0x005e4430 / 0x00630cc0 leave eax = this in the original; returned here.
namespace {
namespace game_b94_06 {


struct H {
    // 0x00630cc0 TextBox::clear
    void* text_clear() {
        *jo::view<std::uint8_t*>(jo::self<jo::TextBox>(this)->text) = 0;
        jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->cursor) = 0;
        jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->sel_anchor) = 0;
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("TextBox::clear", 0x00630cc0, &H::text_clear);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x00630d10 TxtBox::update_len                (thiscall, ret 0)
namespace {
namespace game_b96_06 {


struct H {
    // ------------------------------------------------------------------ 0x00630d10
    void update_len() {
        jo::view<u16>(jo::self<jo::TextBox>(this)->cursor) = static_cast<u16>(strlen_(jo::view<const char*>(jo::self<jo::TextBox>(this)->text)));
        jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor) = 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("TxtBox::update_len", 0x00630d10, &H::update_len);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x00630d60 TextBox::fit_cursor(int, int)                 thiscall, ret 8
namespace {
namespace game_b92_05 {


void* const kScreen = obj(d::g_sprites_a46128);

JOAN_COUNTER(c_fit_cursor, "TextBox::fit_cursor");

struct H {
    // Screen-rect scaling shared by 0x0062c080 / 0x00630d60 (0x00633e60, ret 0x30).
    void scale_rect(float* sx, float* sy, std::int16_t* l, std::int16_t* t, std::int16_t* r, std::int16_t* b) {
        joan::orig::as_sprite2d(kScreen)->fit_rect(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->x)), joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->y)), joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->x2)), joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->y2)), joan::orig::arg::mptr(sx), joan::orig::arg::mptr(sy), joan::orig::arg::mptr(l), joan::orig::arg::mptr(t), joan::orig::arg::mptr(r), joan::orig::arg::mptr(b), joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));
    }

    // 0x00630d60
    void fit_cursor(std::int32_t col, std::int32_t limit) {
        c_fit_cursor.hit();
        std::int32_t idx = static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->lines_valid) - 1;
        jo::self<jo::TextBox>(this)->cursor = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(std::strlen(joan::orig::arg::str(jo::view<const char*>(jo::self<jo::TextBox>(this)->text)))));  // strlen
        while (idx >= 0) {
            const std::int16_t w = static_cast<std::int16_t>(
                joan::orig::arg::as<std::int16_t>(joan::orig::as_text_box(this)->line_y(joan::orig::arg::i16(jo::self<jo::TextBox>(this)->cursor))));  // 0x00630ab0
            if (limit >= static_cast<std::int32_t>(w)) break;
            jo::self<jo::TextBox>(this)->cursor = static_cast<std::int16_t>(static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->line_starts[idx].value) - 1);
            --idx;
        }
        std::int16_t l = jo::self<jo::TextBox>(this)->x, t = jo::self<jo::TextBox>(this)->y, r = jo::self<jo::TextBox>(this)->x2, b = jo::self<jo::TextBox>(this)->y2;
        float sx = 1.0f, sy = 1.0f;
        if (jo::self<jo::TextBox>(this)->fit != 0) scale_rect(&sx, &sy, &l, &t, &r, &b);
        if (col == static_cast<std::int32_t>(r)) return;
        for (;;) {
            const std::int32_t lim = idx < 0 ? 0 : static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->line_starts[idx].value);
            if (!(static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->cursor) > lim)) return;
            const std::int16_t x = static_cast<std::int16_t>(
                joan::orig::arg::as<std::int16_t>(joan::orig::as_text_box(this)->cursor_x_630770(joan::orig::arg::i16(jo::self<jo::TextBox>(this)->cursor))));  // 0x00630770
            if (col >= static_cast<std::int32_t>(x)) return;
            jo::self<jo::TextBox>(this)->cursor = static_cast<std::int16_t>(jo::self<jo::TextBox>(this)->cursor - 1);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("TextBox::fit_cursor", 0x00630d60, &H::fit_cursor);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
//
//      0x00630f50 TextBox2d::rebuild_lines (thiscall, ret 0)
namespace {
namespace game_b108_17 {


void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_rebuild, "TextBox2d::rebuild_lines");

struct H {
    // ------------------------------------------------------------------------ 0x00630f50
    void rebuild_lines() {
        c_rebuild.hit();
        const char* const file = str(d::s_d_ww_en2d_txtbox2d_cpp);  // "d:\ww\en2d\txtbox2d.cpp"
        {
            const i32 line = static_cast<i32>(global<i16>(d::g_a19778)) + 1;
            const u32 n = strlen_(jo::view<const char*>(jo::self<jo::TextBox>(this)->text)) * 2 + 2;
            jo::view<void*>(jo::self<jo::TextBox>(this)->line_starts) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::TextBox>(this)->line_starts)), joan::orig::arg::u32(n), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
        }
        jo::view<u8>(jo::self<jo::TextBox>(this)->lines_valid) = 0;
        joan::orig::as_text(this)->wrap_631080(joan::orig::arg::str(jo::view<const char*>(jo::self<jo::TextBox>(this)->text)), 0u);  // 0x00631080
        if (static_cast<i32>(jo::self<jo::TextBox>(this)->lines_valid) > 0) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a19778)) + 8;
            const u32 n = static_cast<u32>(static_cast<i32>(jo::self<jo::TextBox>(this)->lines_valid) * 2 + 2);
            jo::view<void*>(jo::self<jo::TextBox>(this)->line_starts) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::TextBox>(this)->line_starts)), joan::orig::arg::u32(n), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
        } else {
            const i32 line = static_cast<i32>(global<i16>(d::g_a19778)) + 0xb;
            joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::TextBox>(this)->line_starts)), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::TextBox>(this)->line_starts = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("TextBox2d::rebuild_lines", 0x00630f50, &H::rebuild_lines);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_poll, "Text::poll_62dc70");

struct H {
    // ------------------------------------------------------------------ 0x0062dc70
    i32 poll(u32 flag) {
        c_poll.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_button(reinterpret_cast<u8*>(this) + sizeof(void*))->enabled_slot()) == 0) return 0;  // 0x006226e0 (+4 on x86: after the leading pointer)
        joan::orig::as_text_box(this)->clamp_cursor();  // 0x0062ea70
        const i32 key = joan::orig::arg::as<i32>(joan::orig::as_text_box(this)->key_input(joan::orig::arg::u32(flag & 0xffu)));  // 0x0062dd10
        const i32 r = key != 0 ? key : joan::orig::arg::as<i32>(joan::orig::as_button(this)->update());  // 0x0062e490
        if (r != 0) global<i32>(d::g_a69f28) = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050()) - 0xc8;  // 0x00591050
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Text::poll_62dc70", 0x0062dc70, &H::poll);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
//
//    - Text::wrap_631080 0x00631080 (ret 8): recursive word wrap storing break offsets into the
//      word array +0x91 (count byte +0x95), widths by 0x00624f00 on font +0x74.
namespace {
namespace game_b108_15 {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::str;

constexpr u32 kOne = 0x3f800000u;  // 1.0f

JOAN_COUNTER(c_wrap, "Text::wrap_631080");

struct H {
    // ------------------------------------------------------------------ 0x00631080
    i32 wrap_width(const char* s, i32 n) {
        return static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_font(jo::self<jo::TextBox>(this)->font)->measure(joan::orig::arg::str(s), joan::orig::arg::i16(static_cast<u32>(n)), 0, 0, joan::orig::arg::f32(kOne))));
    }
    i32 wrap_limit() { return jo::self<jo::TextBox>(this)->x2 - jo::self<jo::TextBox>(this)->x + 1; }
    void wrap_break(const char* s, i32 off, i32 idx) {
        const i32 pos = off + idx + 1;
        jo::view<u16>(jo::self<jo::TextBox>(this)->line_starts[jo::self<jo::TextBox>(this)->lines_valid].value) = static_cast<u16>(pos);
        jo::view<u8>(jo::self<jo::TextBox>(this)->lines_valid) = static_cast<u8>(jo::view<u8>(jo::self<jo::TextBox>(this)->lines_valid) + 1u);
        joan::orig::as_text(this)->wrap_631080(joan::orig::arg::str(s + idx + 1), joan::orig::arg::u32(static_cast<u32>(pos)));  // 0x00631080
    }
    void wrap(const char* s, u32 off_arg) {
        c_wrap.hit();
        const i32 off = static_cast<i16>(off_arg);
        const i16 len = static_cast<i16>(joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))));  // strlen
        for (i16 i = 0; i < len; i = static_cast<i16>(i + 1)) {
            if (s[i] == '\n') {
                wrap_break(s, off, i);
                return;
            }
            const i32 w = wrap_width(s, i + 1);
            if (w < wrap_limit()) continue;
            for (i16 j = static_cast<i16>(len - 1); j >= 0; j = static_cast<i16>(j - 1)) {
                if (s[j] != ' ' && s[j] != '\t') continue;
                const i32 wj = wrap_width(s, j + 1);
                if (wj >= wrap_limit()) continue;
                wrap_break(s, off, j);
                return;
            }
            for (i16 k = static_cast<i16>(len - 1); k >= 0; k = static_cast<i16>(k - 1)) {
                const i32 wk = wrap_width(s, k + 1);
                if (wk >= wrap_limit()) continue;
                wrap_break(s, off, k);
                return;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Text::wrap_631080", 0x00631080, &H::wrap);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x006313d0 Edit::clamp_cursor    (thiscall, ret 0) - strlen(+0x70).
namespace {
namespace game_b93_03_2 {

using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

u32 strlen_(const void* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }  // strlen

struct H {
    void edit_clamp_cursor() {  // 0x006313d0
        const i16 len = static_cast<i16>(strlen_(jo::view<void*>(jo::self<jo::TextBox>(this)->text)));
        if (static_cast<i32>(jo::self<jo::TextBox>(this)->cursor) > static_cast<i32>(len)) jo::self<jo::TextBox>(this)->cursor = len;
        if (static_cast<i32>(jo::self<jo::TextBox>(this)->sel_anchor) > static_cast<i32>(len)) jo::self<jo::TextBox>(this)->sel_anchor = len;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Edit::clamp_cursor", 0x006313d0, &H::edit_clamp_cursor);
#undef B93

}  // namespace game_b93_03_2
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
//
//    - TextBox::set_text 0x00631450 (thiscall text, ret 4): strncpy into the box buffer, 0x00630f50,
//      truncates the last line; returns what the original leaves in eax.
namespace {
namespace game_b108_09 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::str;
using joan::orig::helpers::addr_of;

JOAN_COUNTER(c_631450, "TextBox::set_text");

struct H {
    // 0x00631450: returns what the original leaves in eax.
    std::uintptr_t set_text(const char* text) {
        c_631450.hit();
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::TextBox>(this)->text)), joan::orig::arg::str(text), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::TextBox>(this)->max_len)));  // strncpy
        jo::self<jo::TextBox>(this)->text[static_cast<i32>(jo::self<jo::TextBox>(this)->max_len)] = 0;
        joan::orig::as_text_box(this)->rebuild_lines();  // 0x00630f50
        const i32 lines = jo::self<jo::TextBox>(this)->line_count;
        if (lines <= 0) return addr_of(this);
        const i32 cur = jo::self<jo::TextBox>(this)->lines_valid;
        if (cur < static_cast<i32>(jo::self<jo::TextBox>(this)->line_count)) return static_cast<u32>(cur);
        auto line_end = [this]() {
            return static_cast<i32>(jo::view<i16*>(jo::self<jo::TextBox>(this)->line_starts)[static_cast<i32>(jo::self<jo::TextBox>(this)->line_count) - 1]);
        };
        jo::self<jo::TextBox>(this)->text[line_end()] = 0;
        if (jo::self<jo::TextBox>(this)->text[line_end() - 1] != 0x0a) return addr_of(this);
        jo::self<jo::TextBox>(this)->text[line_end()] = 0;
        return addr_of(jo::self<jo::TextBox>(this)->text);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("TextBox::set_text", 0x00631450, &H::set_text);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
//
//      - TextBox::clamp_cursor 0x0062ea70, TextBox::bottom 0x0062ea30, TextBox::sel_max
//        0x006315d0, TextBox::sel_min 0x00631570. ret 0.
namespace {
namespace game_b83_05_2 {


struct H {
    // 0x006315d0 / 0x00631570. ret 0.
    std::int16_t sel_max() {
        return static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->sel_anchor) > static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->cursor) ? jo::self<jo::TextBox>(this)->sel_anchor
                                                                                                                    : jo::self<jo::TextBox>(this)->cursor;
    }
    std::int16_t sel_min() {
        return static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->sel_anchor) < static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->cursor) ? jo::self<jo::TextBox>(this)->sel_anchor
                                                                                                                    : jo::self<jo::TextBox>(this)->cursor;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("TextBox::sel_max", 0x006315d0, &H::sel_max);
B("TextBox::sel_min", 0x00631570, &H::sel_min);
#undef B

}  // namespace game_b83_05_2
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
//
//    - Obj::height_631630 0x00631630: (short)+0x87 - (short)+0x8d.
namespace {
namespace game_b97_06 {


struct H {
    // 0x00631630
    std::int32_t height_631630() {
        return static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->cursor) - static_cast<std::int32_t>(jo::self<jo::TextBox>(this)->sel_anchor);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::height_631630", 0x00631630, &H::height_631630);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
//
//  * module game/b91_03:
//    TextCursor::advanced 0x0058c120, to_lower 0x0058e330, empty methods 0x0058e720 /
//    0x0059d1b0, Stream::forward 0x005939f0, Joystick::in_rect 0x00594360,
//    Joystick::pressed_kind 0x00594a10, Key::repeat_count 0x00595fb0, word getters
//    0x005979a0 / 0x005979c0, Pad::axis 0x00597c80, Pad::axis_range 0x00597e00,
//    same16 0x00598700, Pair::clear 0x005a1730, Bitmap::address 0x005a93b0,
//    MusicStream::release 0x005a98a0, MusicStream::read_header 0x005a9ad0,
//    MusicStream::reset 0x005a9b40, MusicStream::rewind 0x005aa040, Actor::set_byte_fd
//    0x005ad000, Table::get 0x005af330, Table::missing 0x005af360, AnimDef::lookup_word
//    0x005b4a00, Names::get 0x005ba910, Rect::set 0x005bed90 / 0x00612ee0, Array::at1
//    0x005ea410, RenderTarget::create_depth 0x006032d0, RenderTarget::release_depth
//    0x00603380, z_plane_hit 0x0060e6a0, BitWriter::put 0x0060ff70, BitWriter::drop
//    0x006100a0, Viewport::set_int 0x00614110, Sound3d::set 0x00616a90, getter 0x00616b60,
//    Dialog::reset_name 0x00616f80, base dtor bodies 0x00628ad0 / 0x0062c720,
//    TextBox::init 0x0062b8d0, Scroll::set 0x0062be10 / 0x0062cd40, Scroll::at_end
//    0x0062c040 / 0x0062cf70, Scroll::set_end 0x0062ced0, TextBox::cursor_x 0x00630770,
//    TextBox::line_y 0x00630ab0, getter 0x00631770, Table16::entry 0x00638150.
namespace {
namespace game_b91_03_2 {


struct H {

    std::uint32_t get6aa() { return jo::self<jo::Input>(this)->scan_code; }  // 0x00631770
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("get6aa", 0x00631770, &H::get6aa);
#undef B91

}  // namespace game_b91_03_2
}  // namespace

// ---------------------------------------- from modules/game_b93_03_3.cpp (game/b93_03)
namespace {
namespace game_b93_03_3 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;


u32 low_word_into(u32 eax, u16 ax) { return (eax & 0xffff0000u) | ax; }
// eax still holds `this` when the original returns: only ax is the result, the high word is the register
// leftover (its low 32 bits on x64; callers read the u16).
u32 eax_this(const void* p) { return joan::call::register_low_bits(p); }

struct H {
    u32 edit_sel_min() {  // 0x00631670
        const i32 a = jo::self<jo::TextBox>(this)->sel_anchor;
        const i32 b = jo::self<jo::TextBox>(this)->cursor;
        if (a < b) return low_word_into(static_cast<u32>(b), jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor));
        return low_word_into(eax_this(this), jo::view<u16>(jo::self<jo::TextBox>(this)->cursor));
    }
    u32 edit_sel_max() {  // 0x006316d0
        const i32 a = jo::self<jo::TextBox>(this)->sel_anchor;
        const i32 b = jo::self<jo::TextBox>(this)->cursor;
        if (a > b) return low_word_into(static_cast<u32>(b), jo::view<u16>(jo::self<jo::TextBox>(this)->sel_anchor));
        return low_word_into(eax_this(this), jo::view<u16>(jo::self<jo::TextBox>(this)->cursor));
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Edit::sel_min", 0x00631670, &H::edit_sel_min);
B93("Edit::sel_max", 0x006316d0, &H::edit_sel_max);
#undef B93

}  // namespace game_b93_03_3
}  // namespace

// ---------------------------------------- from modules/game_b99_08.cpp (game/b99_08)
namespace {
namespace game_b99_08 {

using i16 = std::int16_t;
using i32 = std::int32_t;


struct H {
    // ------------------------------------------------------------------ 0x00631730
    i32 diff_87_8d() { return static_cast<i32>(jo::self<jo::TextBox>(this)->cursor) - static_cast<i32>(jo::self<jo::TextBox>(this)->sel_anchor); }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_08", name, addr, fn)
B99("Obj::diff_87_8d", 0x00631730, &H::diff_87_8d);
#undef B99

}  // namespace game_b99_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Text::poll_62dc70(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_11::H::poll>(this, a0)); }  // 0x62dc70
auto joan::orig::TextBox::key_input(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_12::H::key_input>(this, a0)); }  // 0x62dd10
auto joan::orig::Button::update() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_12::H::button_update>(this)); }  // 0x62e490
auto joan::orig::TextBox::cursor_x_62e6e0(std::int16_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b83_05::H::cursor_x>(this, a0)); }  // 0x62e6e0
auto joan::orig::Edit::clear() -> void { joan::bind::member<&::game_b93_03::H::edit_clear>(this); }  // 0x62e900
auto joan::orig::TextBox::reset_len_62e950() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07::H::reset_len>(this)); }  // 0x62e950
auto joan::orig::TextBox::fit_length(std::int32_t a0) -> void { joan::bind::member<&::game_b98_06::H::fit_length>(this, a0); }  // 0x62e9a0
auto joan::orig::TextBox::bottom() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_05::H::text_bottom>(this)); }  // 0x62ea30
auto joan::orig::TextBox::clamp_cursor() -> void { joan::bind::member<&::game_b83_05::H::clamp_cursor>(this); }  // 0x62ea70
auto joan::orig::TextBox::key_62fc60(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_14::H::key>(this, a0)); }  // 0x62fc60
auto joan::orig::TextBox::update_mouse() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_12::H::button2_update>(this)); }  // 0x6304f0
auto joan::orig::TextBox::cursor_x_630770(std::int16_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b91_03::H::cursor_x>(this, a0)); }  // 0x630770
auto joan::orig::TextBox::line_y(std::int16_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b91_03::H::line_y>(this, a0)); }  // 0x630ab0
auto joan::orig::TextBox::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_06::H::text_clear>(this)); }  // 0x630cc0
auto joan::orig::TextBox::reset_len_630d10() -> void { joan::bind::member<&::game_b96_06::H::update_len>(this); }  // 0x630d10
auto joan::orig::TextBox::fit_cursor(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b92_05::H::fit_cursor>(this, a0, a1); }  // 0x630d60
auto joan::orig::TextBox::rebuild_lines() -> void { joan::bind::member<&::game_b108_17::H::rebuild_lines>(this); }  // 0x630f50
auto joan::orig::Text::wrap_631080(const char* a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b108_15::H::wrap>(this, a0, a1); }  // 0x631080
auto joan::orig::Edit::clamp_cursor() -> void { joan::bind::member<&::game_b93_03_2::H::edit_clamp_cursor>(this); }  // 0x6313d0
auto joan::orig::TextBox::set_text_631450(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_09::H::set_text>(this, a0)); }  // 0x631450
auto joan::orig::TextBox::sel_min() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b83_05_2::H::sel_min>(this)); }  // 0x631570
auto joan::orig::TextBox::sel_max() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b83_05_2::H::sel_max>(this)); }  // 0x6315d0
auto joan::orig::TextBox::sel_length_631630() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_06::H::height_631630>(this)); }  // 0x631630
auto joan::orig::Edit::sel_min() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_03_3::H::edit_sel_min>(this)); }  // 0x631670
auto joan::orig::Edit::sel_max() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_03_3::H::edit_sel_max>(this)); }  // 0x6316d0
auto joan::orig::TextBox::sel_length_631730() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_08::H::diff_87_8d>(this)); }  // 0x631730
auto joan::orig::Input::flag_6aa() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_03_2::H::get6aa>(this)); }  // 0x631770
