// game/unknown_scroll2d.cpp: functions whose original source file is unknown (formerly game/_unplaced/scroll2d.cpp;
// re/attribution.csv: between ww/en2d/popupwin.cpp and ww/en2d/txtbox2d.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

using i8 = std::int8_t;

using joan::call::cdecl_call;

using joan::img::obj;

// A word local living in its own dword stack slot (only the low word is meaningful).
struct WordSlot {
    i16 v;
    i16 pad;
};

void* const kScreen = obj(d::g_sprites_a46128);

using joan::hook::x87::Fpu;

using joan::hook::global;


}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

JOAN_COUNTER(c_widget62b_ctor, "Widget62b::ctor");
JOAN_COUNTER(c_draw_aligned, "Text::draw_aligned");

struct H {
    // ------------------------------------------------------------------------ 0x0062b6c0
    void* widget62b_ctor() {
        c_widget62b_ctor.hit();
        joan::orig::as_gadget(this)->ctor();  // 0x0061de00
        jo::self<jo::Derived>(this)->vtable = img::obj(d::vt_Obj_9da7d4);
        jo::self<jo::Derived>(this)->enabled = 0;
        jo::view<u16>(jo::self<jo::Derived>(this)->x) = 0xffff;
        jo::view<u16>(jo::self<jo::Derived>(this)->y) = 0xffff;
        jo::view<u16>(jo::self<jo::Derived>(this)->x2) = 0xffff;
        jo::view<u16>(jo::self<jo::Derived>(this)->y2) = 0xffff;
        jo::self<jo::Derived>(this)->id = 0;
        jo::self<jo::Derived>(this)->pressed = 0;
        jo::view<u8>(jo::self<jo::Derived>(this)->extra) = 0xff;
        jo::view<u32>(jo::self<jo::Derived>(this)->min) = 0;
        jo::view<u32>(jo::self<jo::Derived>(this)->max) = 0;
        jo::view<u32>(jo::self<jo::Derived>(this)->page) = 0;
        jo::view<u32>(jo::self<jo::Derived>(this)->pos) = 0;
        jo::self<jo::Derived>(this)->dragging = 0;
        jo::self<jo::Derived>(this)->drag_x = 0;
        jo::self<jo::Derived>(this)->drag_y = 0;
        jo::self<jo::Derived>(this)->drag_pos = 0;
        jo::self<jo::Derived>(this)->callback = nullptr;
        return this;
    }

    // ------------------------------------------------------------------------ 0x0062c5a0
    u32 scroll_value() {
        if (joan::orig::arg::as<i32>(joan::orig::as_scroll(this)->at_end_62c040()) != 0) return jo::view<u32>(jo::self<jo::Scroll>(this)->min);  // 0x0062c040
        const u32 v = jo::view<u32>(jo::self<jo::Scroll>(this)->max) - jo::view<u32>(jo::self<jo::Scroll>(this)->page) + static_cast<u32>(static_cast<i32>(jo::self<jo::Scroll>(this)->extra));
        return joan::orig::arg::as<u32>(joan::orig::clamp_int(joan::orig::arg::i32(v), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Scroll>(this)->min)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Scroll>(this)->max))));  // 0x0062c200
    }

    // ------------------------------------------------------------------------ 0x0062d630
    void* draw_aligned(i16 a1, i16 a2, i16 a3, i16 a4, char* a5, i16 a6, u8* a7, u32 a8, i16 a9, i16 a10,
                     i16 a11, u32 flags, u8 a13) {
        c_draw_aligned.hit();
        const i32 h = *static_cast<volatile i16*>(&jo::as_font2d(a7)->glyph_height);
        i16 y = 0;
        switch (flags & 0xf0) {
            case 0x10: y = a2; break;
            case 0x20: {
                const i32 d = static_cast<i32>(a4) - static_cast<i32>(a2) + 1 - h;
                y = static_cast<i16>(static_cast<i32>(a2) + d / 2);
                break;
            }
            case 0x30: y = static_cast<i16>(static_cast<i32>(a4) - h + 1); break;
            default: break;
        }
        return joan::orig::as_edit_box(this)->init(joan::orig::arg::i32(a1), joan::orig::arg::i32(y), joan::orig::arg::i32(a3), a5, joan::orig::arg::i32(a6), joan::orig::arg::mptr(a7), joan::orig::arg::u32(a8), joan::orig::arg::i32(a9), joan::orig::arg::i32(a10), joan::orig::arg::i32(a11), joan::orig::arg::i32(flags & 0xf), joan::orig::arg::i32(a13));  // 0x0062d4f0
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Widget62b::ctor", 0x0062b6c0, &H::widget62b_ctor);
B95("Scroll::value", 0x0062c5a0, &H::scroll_value);
B95("Text::draw_aligned", 0x0062d630, &H::draw_aligned);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {

struct H {
    // ------------------------------------------------------------------ 0x0062b7d0
    void dtor_62b7d0() {
        jo::self<jo::Derived>(this)->vtable = img::obj(d::vt_Obj_9da7d4);
        joan::orig::as_gadget(this)->destruct();  // 0x0061dfb0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("Derived::dtor_62b7d0", 0x0062b7d0, &H::dtor_62b7d0);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

JOAN_COUNTER(c_slider_init, "Slider::init");
JOAN_COUNTER(c_scroll_init, "Scroll::init");

struct H {
    // ------------------------------------------------------------------ 0x0062b800
    u32 slider_init(u16 l, u16 t, u16 r, u16 b, u32 a5, std::uintptr_t a6, u8 a7, u8 a8, u8 a9) {
        c_slider_init.hit();
        jo::view<u16>(jo::self<jo::Scroll>(this)->x) = l;
        jo::view<u16>(jo::self<jo::Scroll>(this)->y) = t;
        jo::view<u16>(jo::self<jo::Scroll>(this)->x2) = r;
        jo::view<u16>(jo::self<jo::Scroll>(this)->y2) = b;
        jo::self<jo::Scroll>(this)->id = 0;
        jo::view<u8>(jo::self<jo::Scroll>(this)->fit) = a7;
        jo::view<u8>(jo::self<jo::Scroll>(this)->Widget::enabled) = a8;
        jo::view<u8>(jo::self<jo::Scroll>(this)->i8_8) = a9;
        jo::self<jo::Scroll>(this)->pressed = 0;
        jo::view<u8>(jo::self<jo::Scroll>(this)->extra) = 1;
        jo::view<u32>(jo::self<jo::Scroll>(this)->page) = a5;
        jo::self<jo::Scroll>(this)->dragging = 0;
        jo::self<jo::Scroll>(this)->callback = reinterpret_cast<void*>(a6);
        jo::self<jo::Scroll>(this)->enabled = 1;
        return 1;
    }

    // ------------------------------------------------------------------ 0x0062c820
    u32 scroll_init(u16 l, u16 t, u16 r, u16 b, u16 a5, std::uintptr_t a6, u8 a7, u8 a8, u8 a9) {
        c_scroll_init.hit();
        jo::view<u16>(jo::self<jo::Scroll>(this)->x) = l;
        jo::view<u16>(jo::self<jo::Scroll>(this)->y) = t;
        jo::view<u16>(jo::self<jo::Scroll>(this)->x2) = r;
        jo::view<u16>(jo::self<jo::Scroll>(this)->y2) = b;
        jo::self<jo::Scroll>(this)->id = a5;
        jo::view<u8>(jo::self<jo::Scroll>(this)->fit) = a7;
        jo::view<u8>(jo::self<jo::Scroll>(this)->Widget::enabled) = a8;
        jo::view<u8>(jo::self<jo::Scroll>(this)->i8_8) = a9;
        jo::self<jo::Scroll>(this)->pressed = 0;
        jo::view<u8>(jo::self<jo::Scroll>(this)->extra) = 0;
        jo::view<u32>(jo::self<jo::Scroll>(this)->page) = 0;
        jo::self<jo::Scroll>(this)->dragging = 0;
        jo::self<jo::Scroll>(this)->callback = reinterpret_cast<void*>(a6);
        jo::self<jo::Scroll>(this)->enabled = 1;
        return 1;
    }

    // ------------------------------------------------------------------ 0x0062bf00 / 0x0062bfa0 / 0x0062ce30
    i32 clamp_pos() {
        const i32 hi = jo::self<jo::Scroll>(this)->max - jo::self<jo::Scroll>(this)->page + static_cast<i32>(jo::self<jo::Scroll>(this)->extra);
        if (jo::self<jo::Scroll>(this)->pos > hi) jo::self<jo::Scroll>(this)->pos = jo::self<jo::Scroll>(this)->max - jo::self<jo::Scroll>(this)->page + static_cast<i32>(jo::self<jo::Scroll>(this)->extra);
        if (jo::self<jo::Scroll>(this)->pos < jo::self<jo::Scroll>(this)->min) jo::self<jo::Scroll>(this)->pos = jo::self<jo::Scroll>(this)->min;
        return jo::self<jo::Scroll>(this)->pos;
    }
    i32 set_min(u32 v) {
        jo::view<u32>(jo::self<jo::Scroll>(this)->min) = v;
        return clamp_pos();
    }
    i32 set_max(u32 v) {
        jo::view<u32>(jo::self<jo::Scroll>(this)->max) = v;
        return clamp_pos();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("Slider::init", 0x0062b800, &H::slider_init);
B("Slider::set_min", 0x0062bf00, &H::set_min);
B("Slider::set_max", 0x0062bfa0, &H::set_max);
B("Scroll::init", 0x0062c820, &H::scroll_init);
B("Scroll::set_min", 0x0062ce30, &H::set_min);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

JOAN_COUNTER(c_textbox_init, "TextBox::init");

struct H {
    void* base_dtor_62c720() {
        jo::self<jo::Scroll>(this)->vtable = img::obj(d::vt_Obj_9da7d8);
        joan::orig::as_gadget(this)->destruct();  // 0x0061dfb0
        return this;  // EAX: 0x0061dfb0 leaves this
    }

    // 0x0062b8d0
    std::int32_t textbox_init(std::int16_t x, std::int16_t y, std::int16_t x2, std::int16_t y2, std::int16_t w71,
                              std::uintptr_t d8e, std::uint8_t b6, std::uint8_t b7, std::uint8_t b8) {
        c_textbox_init.hit();
        jo::self<jo::Scroll>(this)->x = x;
        jo::self<jo::Scroll>(this)->y = y;
        jo::self<jo::Scroll>(this)->x2 = x2;
        jo::self<jo::Scroll>(this)->y2 = y2;
        jo::view<std::int16_t>(jo::self<jo::Scroll>(this)->id) = w71;
        jo::view<std::uint8_t>(jo::self<jo::Scroll>(this)->fit) = b6;
        jo::view<std::uint8_t>(jo::self<jo::Scroll>(this)->Widget::enabled) = b7;
        jo::view<std::uint8_t>(jo::self<jo::Scroll>(this)->i8_8) = b8;
        jo::self<jo::Scroll>(this)->pressed = 0;
        jo::view<std::uint8_t>(jo::self<jo::Scroll>(this)->extra) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->page) = 0;
        jo::self<jo::Scroll>(this)->dragging = 0;
        jo::self<jo::Scroll>(this)->callback = reinterpret_cast<void*>(d8e);
        jo::self<jo::Scroll>(this)->enabled = 1;
        return 1;
    }

    // 0x0062be10 / 0x0062cd40
    void scroll_set(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->min) = a;
        jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->max) = b;
        jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->pos) = c;
    }

    // 0x0062c040 / 0x0062cf70
    std::int32_t scroll_at_end() {
        const std::int32_t v = static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->max) - jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->min) +
                                                         static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->extra)));
        return v <= 0 ? 1 : 0;
    }

    // 0x0062ced0
    void scroll_set_end(std::uint32_t end) {
        jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->max) = end;
        auto limit = [&]() {
            return static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->max) - jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->page) +
                                             static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->extra)));
        };
        if (jo::self<jo::Scroll>(this)->pos > limit()) jo::self<jo::Scroll>(this)->pos = limit();
        if (jo::self<jo::Scroll>(this)->pos < jo::self<jo::Scroll>(this)->min) jo::self<jo::Scroll>(this)->pos = jo::self<jo::Scroll>(this)->min;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("TextBox::init", 0x0062b8d0, &H::textbox_init);
B91("Scroll::set_62be10", 0x0062be10, &H::scroll_set);
B91("Scroll::at_end_62c040", 0x0062c040, &H::scroll_at_end);
B91("dtor_body_62c720", 0x0062c720, &H::base_dtor_62c720);
B91("Scroll::set_62cd40", 0x0062cd40, &H::scroll_set);
B91("Scroll::set_end", 0x0062ced0, &H::scroll_set_end);
B91("Scroll::at_end_62cf70", 0x0062cf70, &H::scroll_at_end);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

static_assert(sizeof(WordSlot) == 4, "WordSlot");

void* const kMath = obj(d::g_misc);

JOAN_COUNTER(c_thumb_pos, "TextBox::thumb_pos");

struct H {
    // ------------------------------------------------------------------ 0x0062b9a0
    i32 reset_62b9a0() {
        jo::self<jo::Scroll>(this)->callback = nullptr;
        jo::self<jo::Scroll>(this)->enabled = 0;
        return 1;
    }

    // ------------------------------------------------------------------ 0x0062cfb0
    u32 thumb_pos() {
        c_thumb_pos.hit();
        WordSlot s8{jo::self<jo::Scroll>(this)->x, 0};
        WordSlot sc{jo::self<jo::Scroll>(this)->y, 0};
        WordSlot s10{jo::self<jo::Scroll>(this)->x2, 0};
        WordSlot s14{jo::self<jo::Scroll>(this)->y2, 0};
        float f18 = 1.0f;
        float f1c = 1.0f;
        if (jo::self<jo::Scroll>(this)->fit != 0) {
            joan::orig::as_sprite2d(kScreen)->fit_rect(joan::orig::arg::i16(jo::self<jo::Scroll>(this)->x), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->y), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->x2), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->y2), &f18, &f1c, &s8, &sc, &s10, &s14, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
        }
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_scroll(this)->at_end_62cf70());  // 0x0062cf70
        if (r != 0) return (r & 0xffff0000u) | static_cast<u16>(sc.v);
        const i32 l14 = s14.v;
        const i32 lc = sc.v;
        i32 esi = sc.v;
        i32 edi = jo::self<jo::Scroll>(this)->pos - jo::self<jo::Scroll>(this)->min;
        i32 ebx = static_cast<i32>(s14.v) - static_cast<i32>(sc.v) + 1;
        const Fpu fpu{true};
        const double d = fpu.mul(static_cast<double>(jo::view<i16>(jo::self<jo::Scroll>(this)->id)), static_cast<double>(f1c));
        const i32 rr = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_misc(kMath)->round(joan::orig::arg::f64(d))));  // 0x00590a20
        ebx = static_cast<i32>(static_cast<u32>(ebx) - static_cast<u32>(rr));
        edi = static_cast<i32>(static_cast<u32>(edi) * static_cast<u32>(ebx));
        const i32 div = jo::self<jo::Scroll>(this)->max - jo::self<jo::Scroll>(this)->min + static_cast<i32>(jo::self<jo::Scroll>(this)->extra);
        esi = static_cast<i32>(static_cast<u32>(esi) + static_cast<u32>(edi / div));
        return joan::orig::arg::as<u32>(joan::orig::clamp_int(joan::orig::arg::i32(esi), joan::orig::arg::i32(lc), joan::orig::arg::i32(l14)));  // 0x0062c200
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("TextBox::reset_62b9a0", 0x0062b9a0, &H::reset_62b9a0);
B("TextBox::thumb_pos", 0x0062cfb0, &H::thumb_pos);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

static_assert(sizeof(WordSlot) == 4, "WordSlot");

void* const kMouse = obj(d::g_mouse);

JOAN_COUNTER(c_drag, "TextBox::drag");
JOAN_COUNTER(c_setup, "TextBox::setup");

struct H {
    // ------------------------------------------------------------------ 0x0062b9e0
    i32 drag() {
        c_drag.hit();
        if (jo::view<i8>(jo::self<jo::Scroll>(this)->enabled) == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_button(&jo::self<jo::Scroll>(this)->child)->enabled_slot()) == 0) return 0;  // 0x006226e0
        if (jo::view<i8>(jo::self<jo::Scroll>(this)->pressed) == 0) return 0;
        WordSlot s8{jo::self<jo::Scroll>(this)->x, 0};
        WordSlot sc{jo::self<jo::Scroll>(this)->y, 0};
        WordSlot s10{jo::self<jo::Scroll>(this)->x2, 0};
        WordSlot s14{jo::self<jo::Scroll>(this)->y2, 0};
        float f18 = 1.0f;
        float f1c = 1.0f;
        if (jo::self<jo::Scroll>(this)->fit != 0) {
            joan::orig::as_sprite2d(kScreen)->fit_rect(joan::orig::arg::i16(jo::self<jo::Scroll>(this)->x), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->y), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->x2), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->y2), &f18, &f1c, &s8, &sc, &s10, &s14, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
        }
        i32 result = 0;
        if (jo::view<i8>(jo::self<jo::Scroll>(this)->dragging) == 0) {
            const i16 w24 = joan::orig::arg::as<i16>(joan::orig::as_edit_box(this)->cursor_column());  // 0x0062c080
            const i16 w28 = joan::orig::arg::as<i16>(joan::orig::as_gadget(this)->scroll_pos());  // 0x0062c250
            if (joan::orig::arg::as<i32>(joan::orig::as_parser(kMouse)->rect_mode(static_cast<i32>(s8.v), static_cast<i32>(sc.v), static_cast<i32>(s10.v), static_cast<i32>(s14.v), 0)) != 0) {  // 0x005940f0
                const i16 w2c = joan::orig::arg::as<i16>(joan::orig::as_table(kMouse)->at_672(0));  // 0x0043c3c0
                if (static_cast<i32>(w2c) >= static_cast<i32>(w24) && static_cast<i32>(w2c) <= static_cast<i32>(w28)) {
                    jo::self<jo::Scroll>(this)->dragging = 1;
                    jo::view<i16>(jo::self<jo::Scroll>(this)->drag_x) = w2c;
                    jo::view<i16>(jo::self<jo::Scroll>(this)->drag_y) = w24;
                    jo::self<jo::Scroll>(this)->drag_pos = jo::view<u32>(jo::self<jo::Scroll>(this)->pos);
                    result = 2;  // overwritten below, as in the original
                } else {
                    const i32 half = (static_cast<i32>(w28) - static_cast<i32>(w24)) / 2;
                    const i16 w30 = static_cast<i16>(static_cast<i32>(w2c) - half);
                    jo::view<u32>(jo::self<jo::Scroll>(this)->pos) = joan::orig::arg::as<u32>(joan::orig::as_text_box(this)->scroll_x(joan::orig::arg::i16(w30)));  // 0x0062c410
                    jo::self<jo::Scroll>(this)->dragging = 1;
                    jo::view<i16>(jo::self<jo::Scroll>(this)->drag_x) = w2c;
                    jo::view<i16>(jo::self<jo::Scroll>(this)->drag_y) = joan::orig::arg::as<i16>(joan::orig::as_edit_box(this)->cursor_column());  // 0x0062c080
                    jo::self<jo::Scroll>(this)->drag_pos = jo::view<u32>(jo::self<jo::Scroll>(this)->pos);
                }
                result = 1;
            }
        } else {
            if (global<i32>(d::g_mouse_left_down) == 0) jo::self<jo::Scroll>(this)->dragging = 0;
            const i32 v = global<i32>(d::g_mouse_x) - static_cast<i32>(jo::view<i16>(jo::self<jo::Scroll>(this)->drag_x)) + static_cast<i32>(jo::view<i16>(jo::self<jo::Scroll>(this)->drag_y));
            jo::self<jo::Scroll>(this)->drag_pos = jo::view<u32>(jo::self<jo::Scroll>(this)->pos);
            jo::view<u32>(jo::self<jo::Scroll>(this)->pos) = joan::orig::arg::as<u32>(joan::orig::as_text_box(this)->scroll_x(joan::orig::arg::i16(static_cast<i16>(v))));  // 0x0062c410
            result = (jo::self<jo::Scroll>(this)->drag_pos != jo::view<u32>(jo::self<jo::Scroll>(this)->pos)) ? 1 : 2;
        }
        return result;
    }

    // ------------------------------------------------------------------ 0x0062c750
    i32 setup(u16 x0, u16 y0, u16 x1, u16 y1, u32 a5, std::uintptr_t a6, u8 b7, u8 b8, u8 b9) {
        c_setup.hit();
        jo::view<u16>(jo::self<jo::Scroll>(this)->x) = x0;
        jo::view<u16>(jo::self<jo::Scroll>(this)->y) = y0;
        jo::view<u16>(jo::self<jo::Scroll>(this)->x2) = x1;
        jo::view<u16>(jo::self<jo::Scroll>(this)->y2) = y1;
        jo::self<jo::Scroll>(this)->id = 0;
        jo::view<u8>(jo::self<jo::Scroll>(this)->fit) = b7;
        jo::view<u8>(jo::self<jo::Scroll>(this)->Widget::enabled) = b8;
        jo::view<u8>(jo::self<jo::Scroll>(this)->i8_8) = b9;
        jo::self<jo::Scroll>(this)->pressed = 0;
        jo::view<u8>(jo::self<jo::Scroll>(this)->extra) = 1;
        jo::view<u32>(jo::self<jo::Scroll>(this)->page) = a5;
        jo::self<jo::Scroll>(this)->dragging = 0;
        jo::self<jo::Scroll>(this)->callback = reinterpret_cast<void*>(a6);
        jo::self<jo::Scroll>(this)->enabled = 1;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("TextBox::drag", 0x0062b9e0, &H::drag);
B("TextBox::setup", 0x0062c750, &H::setup);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_ui7.cpp (game/ui7)
namespace {
namespace game_ui7 {

struct H {
    // 0x0062bd30 Widget::try_activate. ret 4.
    std::int32_t try_activate(char arg) {
        if (static_cast<std::int8_t>(jo::self<jo::Scroll>(this)->enabled) == 0) return 0;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_widget_child(&jo::self<jo::Scroll>(this)->child)->visible()) == 0) {  // 0x00623360
            jo::self<jo::Scroll>(this)->pressed = 0;
            return 0;
        }
        if (jo::self<jo::Scroll>(this)->callback == nullptr) return 0;
        using Callback = std::int32_t(JOAN_CDECL*)(void*, char);
        if (reinterpret_cast<Callback>(jo::self<jo::Scroll>(this)->callback)(this, arg) == 0) return 0;  // a code address
        jo::self<jo::Scroll>(this)->pressed = 1;
        return 1;
    }
};

#define U7(name, addr, fn) JOAN_REPLACE("game/ui7", name, addr, fn)
U7("Widget::try_activate", 0x0062bd30, &H::try_activate);
#undef U7

}  // namespace game_ui7
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

struct H {
    u32 set_62bdd0(u32 a, u32 b) {
        joan::orig::as_scroll(this)->set_pos(joan::orig::arg::i32(a));                             // 0x0062be50
        return joan::orig::arg::as<u32>(joan::orig::as_widget(this)->try_activate(static_cast<char>(joan::orig::arg::i32(static_cast<u8>(b)))));     // 0x0062bd30 (reads the byte)
    }
    u32 set_62cd00(u32 a, u32 b) {
        joan::orig::as_edit_box(this)->set_top(joan::orig::arg::i32(a));                             // 0x0062cd80
        return joan::orig::arg::as<u32>(joan::orig::as_button(this)->fire(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u8>(b)))));     // 0x0062cc60 (reads the byte)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::set_62bdd0", 0x0062bdd0, &H::set_62bdd0);
B("Obj::set_62cd00", 0x0062cd00, &H::set_62cd00);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
namespace {
namespace game_b90_05 {

struct H {
    // 0x0062be50
    std::int32_t scroll_set_pos(std::int32_t pos) {
        jo::self<jo::Scroll>(this)->pos = pos;
        if (jo::self<jo::Scroll>(this)->pos > jo::self<jo::Scroll>(this)->max - jo::self<jo::Scroll>(this)->page + static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Scroll>(this)->extra)))
            jo::self<jo::Scroll>(this)->pos = jo::self<jo::Scroll>(this)->max - jo::self<jo::Scroll>(this)->page + static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Scroll>(this)->extra));
        if (jo::self<jo::Scroll>(this)->pos < jo::self<jo::Scroll>(this)->min) jo::self<jo::Scroll>(this)->pos = jo::self<jo::Scroll>(this)->min;
        return jo::self<jo::Scroll>(this)->pos;
    }
};

// 0x0062c200
std::int32_t JOAN_CDECL clamp_int(std::int32_t v, std::int32_t lo, std::int32_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("Scroll::set_pos", 0x0062be50, &H::scroll_set_pos);
B9005("clamp_int", 0x0062c200, &clamp_int);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
namespace {
namespace game_b92_05 {

void* const kMath = obj(d::g_misc);

JOAN_COUNTER(c_cursor_column, "EditBox::cursor_column");
JOAN_COUNTER(c_editbox_init, "EditBox::init");

struct H {
    // Screen-rect scaling shared by 0x0062c080 / 0x00630d60 (0x00633e60, ret 0x30).
    void scale_rect(float* sx, float* sy, std::int16_t* l, std::int16_t* t, std::int16_t* r, std::int16_t* b) {
        joan::orig::as_sprite2d(kScreen)->fit_rect(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->x)), joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->y)), joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->x2)), joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->y2)), joan::orig::arg::mptr(sx), joan::orig::arg::mptr(sy), joan::orig::arg::mptr(l), joan::orig::arg::mptr(t), joan::orig::arg::mptr(r), joan::orig::arg::mptr(b), joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));
    }

    // 0x0062c080
    std::uint32_t cursor_column() {
        c_cursor_column.hit();
        std::int16_t l = jo::self<jo::Scroll>(this)->x, t = jo::self<jo::Scroll>(this)->y, r = jo::self<jo::Scroll>(this)->x2, b = jo::self<jo::Scroll>(this)->y2;
        float sx = 1.0f, sy = 1.0f;
        if (jo::self<jo::Scroll>(this)->fit != 0) scale_rect(&sx, &sy, &l, &t, &r, &b);
        const std::uint32_t chk = joan::orig::arg::as<std::uint32_t>(joan::orig::as_scroll(this)->at_end_62c040());  // 0x0062c040
        if (chk != 0) return (chk & 0xffff0000u) | static_cast<std::uint16_t>(l);
        const joan::hook::x87::Fpu fp{true};
        const std::int32_t left = l;
        const std::uint32_t span = static_cast<std::uint32_t>(jo::self<jo::Scroll>(this)->pos - jo::self<jo::Scroll>(this)->min);
        const double w = fp.mul(static_cast<double>(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::Scroll>(this)->id))), static_cast<double>(sx));
        const std::int16_t cw = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kMath)->round(joan::orig::arg::f64(w))));  // 0x00590a20
        const std::uint32_t width = static_cast<std::uint32_t>(static_cast<std::int32_t>(r) - left + 1 - static_cast<std::int32_t>(cw));
        const std::int32_t num = static_cast<std::int32_t>(span * width);
        const std::int32_t den = jo::self<jo::Scroll>(this)->max - jo::self<jo::Scroll>(this)->min + static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->extra);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::clamp_int(joan::orig::arg::i32(left + num / den), joan::orig::arg::i32(left), static_cast<std::int32_t>(r)));  // 0x0062c200
    }

    // 0x0062cd80
    std::int32_t set_top(std::int32_t v) {
        jo::self<jo::Scroll>(this)->pos = v;
        if (jo::self<jo::Scroll>(this)->pos > jo::self<jo::Scroll>(this)->max - jo::self<jo::Scroll>(this)->page + static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->extra))
            jo::self<jo::Scroll>(this)->pos = jo::self<jo::Scroll>(this)->max - jo::self<jo::Scroll>(this)->page + static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->extra);
        if (jo::self<jo::Scroll>(this)->pos < jo::self<jo::Scroll>(this)->min) jo::self<jo::Scroll>(this)->pos = jo::self<jo::Scroll>(this)->min;
        return jo::self<jo::Scroll>(this)->pos;
    }

    // 0x0062d480
    std::int32_t bottom_line() {
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_scroll(this)->at_end_62cf70()) != 0) return jo::self<jo::Scroll>(this)->min;  // 0x0062cf70
        return joan::orig::arg::as<std::int32_t>(joan::orig::clamp_int(joan::orig::arg::i32(jo::self<jo::Scroll>(this)->max - jo::self<jo::Scroll>(this)->page + static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->extra)), joan::orig::arg::i32(jo::self<jo::Scroll>(this)->min), joan::orig::arg::i32(jo::self<jo::Scroll>(this)->max)));  // 0x0062c200
    }

    // 0x0062d4f0
    void* editbox_init(std::int32_t x, std::int32_t y, std::int32_t r, char* text, std::int32_t w18, std::uint8_t* font,
                      std::uint32_t d20, std::int32_t w24, std::int32_t w28, std::int32_t w2c, std::int32_t b30, std::int32_t b34) {
        c_editbox_init.hit();
        jo::self<jo::EditBox>(this)->x = static_cast<std::int16_t>(x);
        jo::self<jo::EditBox>(this)->y = static_cast<std::int16_t>(y);
        jo::self<jo::EditBox>(this)->x2 = static_cast<std::int16_t>(r);
        jo::self<jo::EditBox>(this)->y2 = static_cast<std::int16_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(y)) +
                                                         static_cast<std::int32_t>(jo::as_font2d(font)->glyph_height) - 1);
        jo::self<jo::EditBox>(this)->text = text;
        jo::view<std::uint8_t*>(jo::self<jo::EditBox>(this)->font) = font;
        jo::view<std::uint32_t>(jo::self<jo::EditBox>(this)->colour) = d20;
        jo::self<jo::EditBox>(this)->text_7c = static_cast<std::int16_t>(w24);
        jo::self<jo::EditBox>(this)->text_7e = static_cast<std::int16_t>(w28);
        jo::self<jo::EditBox>(this)->text_80 = static_cast<std::int16_t>(w2c);
        jo::self<jo::EditBox>(this)->align = static_cast<std::uint8_t>(b30);
        jo::view<std::uint8_t>(jo::self<jo::EditBox>(this)->fit) = static_cast<std::uint8_t>(b34);
        jo::view<std::uint8_t>(jo::self<jo::EditBox>(this)->focused) = 0;
        jo::self<jo::EditBox>(this)->u8_84 = 0;
        jo::self<jo::EditBox>(this)->max_len = static_cast<std::int16_t>(w18);
        jo::self<jo::EditBox>(this)->cursor = 0;
        jo::self<jo::EditBox>(this)->caret_colour = jo::view<std::uint32_t>(jo::self<jo::EditBox>(this)->colour);
        jo::self<jo::EditBox>(this)->sel_anchor = 0;
        jo::self<jo::EditBox>(this)->view_start = 0;
        return this;  // EAX: this (last store via [ebp-4])
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("EditBox::cursor_column", 0x0062c080, &H::cursor_column);
B("EditBox::set_top", 0x0062cd80, &H::set_top);
B("EditBox::bottom_line", 0x0062d480, &H::bottom_line);
B("EditBox::init", 0x0062d4f0, &H::editbox_init);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

JOAN_COUNTER(c_scroll_pos, "Gadget::scroll_pos");

struct H {
    // 0x0062c250 Gadget::scroll_pos
    std::int16_t scroll_pos() {
        c_scroll_pos.hit();
        std::int16_t l8 = jo::self<jo::Scroll>(this)->x;
        std::int16_t lc = jo::self<jo::Scroll>(this)->y;
        std::int16_t l10 = jo::self<jo::Scroll>(this)->x2;
        std::int16_t l14 = jo::self<jo::Scroll>(this)->y2;
        float l18 = 1.0f;
        float l1c = 1.0f;
        if (jo::self<jo::Scroll>(this)->fit != 0)
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::self<jo::Scroll>(this)->x), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->y), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->x2), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->y2), &l18, &l1c, &l8, &lc, &l10, &l14, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_scroll(this)->at_end_62c040()) != 0) return l10;  // 0x0062c040
        const std::int32_t a3 = l10;
        const std::int32_t a2 = l8;
        std::uint32_t esi = static_cast<std::uint32_t>(static_cast<std::int32_t>(l8));
        const double s1 = static_cast<double>(static_cast<float>(jo::view<std::int16_t>(jo::self<jo::Scroll>(this)->id)) * l18);
        esi += static_cast<std::uint32_t>(static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(s1)))));  // 0x00590a20
        std::uint32_t edi = jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->pos) + jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->page) - jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->min);
        std::uint32_t ebx = static_cast<std::uint32_t>(static_cast<std::int32_t>(l10) - static_cast<std::int32_t>(l8) + 1);
        const double s2 = static_cast<double>(static_cast<float>(jo::view<std::int16_t>(jo::self<jo::Scroll>(this)->id)) * l18);
        ebx -= static_cast<std::uint32_t>(static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(s2)))));
        edi *= ebx;
        const std::uint32_t d = jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->max) - jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->min) + static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->extra));
        const std::int32_t q = static_cast<std::int32_t>(edi) / static_cast<std::int32_t>(d);
        const std::int32_t a1 = static_cast<std::int32_t>(esi + static_cast<std::uint32_t>(q) - 1u);
        return static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::clamp_int(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3))));  // 0x0062c200 clamp
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Gadget::scroll_pos", 0x0062c250, &H::scroll_pos);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

JOAN_COUNTER(c_scroll_x, "TextBox::scroll_x");
JOAN_COUNTER(c_scroll_y, "TextBox::scroll_y");

struct H {
    // shared body of 0x0062c410 (vertical == false) and 0x0062d2f0 (vertical == true).
    std::uint32_t scroll(std::int16_t v, bool vertical) {
        std::int16_t l8 = jo::self<jo::Scroll>(this)->x, lc = jo::self<jo::Scroll>(this)->y, l10 = jo::self<jo::Scroll>(this)->x2, l14 = jo::self<jo::Scroll>(this)->y2;
        float l18 = 1.0f, l1c = 1.0f;
        if (jo::self<jo::Scroll>(this)->fit != 0) {
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(static_cast<std::uint32_t>(jo::view<std::uint16_t>(jo::self<jo::Scroll>(this)->x))), joan::orig::arg::i16(static_cast<std::uint32_t>(jo::view<std::uint16_t>(jo::self<jo::Scroll>(this)->y))), joan::orig::arg::i16(static_cast<std::uint32_t>(jo::view<std::uint16_t>(jo::self<jo::Scroll>(this)->x2))), joan::orig::arg::i16(static_cast<std::uint32_t>(jo::view<std::uint16_t>(jo::self<jo::Scroll>(this)->y2))), &l18, &l1c, &l8, &lc, &l10, &l14, joan::orig::arg::i16(0x280u), joan::orig::arg::i16(0x1e0u));  // 0x00633e60
        }
        const std::int16_t lo = vertical ? lc : l8;
        const std::int16_t hi = vertical ? l14 : l10;
        const float scale = vertical ? l1c : l18;
        const std::uint32_t a = static_cast<std::uint32_t>(static_cast<std::int32_t>(v) - lo) *
                                (jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->max) - jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->min) +
                                 static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Scroll>(this)->extra)));
        std::uint32_t esi = static_cast<std::uint32_t>(static_cast<std::int32_t>(hi) - lo + 1);
        const joan::hook::x87::Fpu fpu{true};
        const double arg = fpu.mul(static_cast<double>(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::Scroll>(this)->id))), scale);
        const std::int16_t m = static_cast<std::int16_t>(joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(arg))));  // 0x00590a20
        esi -= static_cast<std::uint32_t>(static_cast<std::int32_t>(m));
        const std::int32_t den = static_cast<std::int32_t>(esi);
        const std::int32_t num = static_cast<std::int32_t>(esi + a * 2u);
        const std::int32_t c = (num / den) / 2;
        const std::uint32_t r = vertical ? joan::orig::arg::as<std::uint32_t>(joan::orig::as_edit_box(this)->bottom_line())   // 0x0062d480
                                         : joan::orig::as_scroll(this)->value();  // 0x0062c5a0
        return joan::orig::arg::as<std::uint32_t>(joan::orig::clamp_int(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->min) + static_cast<std::uint32_t>(c)), joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Scroll>(this)->min)), joan::orig::arg::i32(r)));  // 0x0062c200
    }
    // 0x0062c410
    std::uint32_t scroll_x(std::int16_t x) {
        c_scroll_x.hit();
        return scroll(x, false);
    }
    // 0x0062d2f0
    std::uint32_t scroll_y(std::int16_t y) {
        c_scroll_y.hit();
        return scroll(y, true);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("TextBox::scroll_x", 0x0062c410, &H::scroll_x);
B("TextBox::scroll_y", 0x0062d2f0, &H::scroll_y);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

struct H {
    // ------------------------------------------------------------------ 0x0062c610
    H* ctor_62c610() {
        joan::orig::as_gadget(this)->ctor();  // 0x0061de00
        jo::self<jo::Scroll>(this)->vtable = img::obj(d::vt_Obj_9da7d8);
        jo::self<jo::Scroll>(this)->enabled = 0;
        jo::view<u16>(jo::self<jo::Scroll>(this)->x) = 0xffff;
        jo::view<u16>(jo::self<jo::Scroll>(this)->y) = 0xffff;
        jo::view<u16>(jo::self<jo::Scroll>(this)->x2) = 0xffff;
        jo::view<u16>(jo::self<jo::Scroll>(this)->y2) = 0xffff;
        jo::self<jo::Scroll>(this)->id = 0;
        jo::self<jo::Scroll>(this)->pressed = 0;
        jo::view<u8>(jo::self<jo::Scroll>(this)->extra) = 0xff;
        jo::view<u32>(jo::self<jo::Scroll>(this)->min) = 0;
        jo::view<u32>(jo::self<jo::Scroll>(this)->max) = 0;
        jo::view<u32>(jo::self<jo::Scroll>(this)->page) = 0;
        jo::view<u32>(jo::self<jo::Scroll>(this)->pos) = 0;
        jo::self<jo::Scroll>(this)->dragging = 0;
        jo::self<jo::Scroll>(this)->drag_x = 0;
        jo::self<jo::Scroll>(this)->drag_y = 0;
        jo::self<jo::Scroll>(this)->drag_pos = 0;
        jo::self<jo::Scroll>(this)->callback = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::ctor_62c610", 0x0062c610, &H::ctor_62c610);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

struct H {
    // ------------------------------------------------------------------ 0x0062c8f0
    i32 reset_8e() {
        jo::self<jo::Scroll>(this)->callback = nullptr;
        jo::self<jo::Scroll>(this)->enabled = 0;
        return 1;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Ctl::reset_8e", 0x0062c8f0, &H::reset_8e);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b101_12.cpp (game/b101_12)
namespace {
namespace game_b101_12 {

JOAN_COUNTER(c_scroller, "Scroller::update");

struct H {
    // 0x0062c930
    i32 scroller_update() {
        c_scroller.hit();
        if (jo::view<i8>(jo::self<jo::Scroll>(this)->enabled) == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_button(&jo::self<jo::Scroll>(this)->child)->enabled_slot()) == 0) return 0;  // 0x006226e0
        if (jo::view<i8>(jo::self<jo::Scroll>(this)->pressed) == 0) return 0;
        i16 x0 = jo::self<jo::Scroll>(this)->x;
        i16 y0 = jo::self<jo::Scroll>(this)->y;
        i16 x1 = jo::self<jo::Scroll>(this)->x2;
        i16 y1 = jo::self<jo::Scroll>(this)->y2;
        u32 sx = 0x3f800000;
        u32 sy = 0x3f800000;
        if (jo::self<jo::Scroll>(this)->fit != 0) {
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::self<jo::Scroll>(this)->x), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->y), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->x2), joan::orig::arg::i16(jo::self<jo::Scroll>(this)->y2), &sx, &sy, &x0, &y0, &x1, &y1, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
        }
        i32 result = 0;
        if (jo::view<i8>(jo::self<jo::Scroll>(this)->dragging) == 0) {
            const i16 lo = joan::orig::arg::as<i16>(joan::orig::as_text_box(this)->thumb_pos());  // 0x0062cfb0
            const i16 hi = joan::orig::arg::as<i16>(joan::orig::as_widget(this)->text_bottom());  // 0x0062d130
            if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_mode(static_cast<i32>(x0), static_cast<i32>(y0), static_cast<i32>(x1), static_cast<i32>(y1), 0)) != 0) {  // 0x005940f0
                const i16 pos = joan::orig::arg::as<i16>(joan::orig::as_table(obj(d::g_mouse))->at_676(0));       // 0x0043c3f0
                if (static_cast<i32>(pos) >= static_cast<i32>(lo) && static_cast<i32>(pos) <= static_cast<i32>(hi)) {
                    jo::self<jo::Scroll>(this)->dragging = 1;
                    jo::view<i16>(jo::self<jo::Scroll>(this)->drag_x) = pos;
                    jo::view<i16>(jo::self<jo::Scroll>(this)->drag_y) = lo;
                    jo::self<jo::Scroll>(this)->drag_pos = jo::view<u32>(jo::self<jo::Scroll>(this)->pos);
                } else {
                    const i16 target = static_cast<i16>(static_cast<i32>(pos) -
                                                        (static_cast<i32>(hi) - static_cast<i32>(lo)) / 2);
                    jo::view<u32>(jo::self<jo::Scroll>(this)->pos) = joan::orig::arg::as<u32>(joan::orig::as_text_box(this)->scroll_y(joan::orig::arg::i16(target)));  // 0x0062d2f0
                    jo::self<jo::Scroll>(this)->dragging = 1;
                    jo::view<i16>(jo::self<jo::Scroll>(this)->drag_x) = pos;
                    jo::view<i16>(jo::self<jo::Scroll>(this)->drag_y) = joan::orig::arg::as<i16>(joan::orig::as_text_box(this)->thumb_pos());
                    jo::self<jo::Scroll>(this)->drag_pos = jo::view<u32>(jo::self<jo::Scroll>(this)->pos);
                }
                result = 1;  // (the in-range branch's 2 is overwritten by 1 in the original)
            }
            return result;
        }
        if (global<i32>(d::g_mouse_left_down) == 0) jo::self<jo::Scroll>(this)->dragging = 0;
        const u32 pos = global<u32>(d::g_mouse_y);
        jo::self<jo::Scroll>(this)->drag_pos = jo::view<u32>(jo::self<jo::Scroll>(this)->pos);
        jo::view<u32>(jo::self<jo::Scroll>(this)->pos) = joan::orig::arg::as<u32>(joan::orig::as_text_box(this)->scroll_y(joan::orig::arg::i16(static_cast<i16>(pos))));
        return jo::self<jo::Scroll>(this)->drag_pos != jo::view<u32>(jo::self<jo::Scroll>(this)->pos) ? 1 : 2;
    }
};

#define R12(name, addr, fn) JOAN_REPLACE("game/b101_12", name, addr, fn)
R12("Scroller::update", 0x0062c930, &H::scroller_update);
#undef R12

}  // namespace game_b101_12
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

JOAN_COUNTER(c_button_fire, "Button::fire");

struct H {
    i32 button_fire(u8 arg) {  // 0x0062cc60
        if (static_cast<i8>(jo::self<jo::Scroll>(this)->enabled) == 0) return 0;
        c_button_fire.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_widget_child(&jo::self<jo::Scroll>(this)->child)->visible()) == 0) {  // 0x00623360
            jo::self<jo::Scroll>(this)->pressed = 0;
            return 0;
        }
        if (jo::self<jo::Scroll>(this)->callback == nullptr) return 0;
        // push ecx with cl = arg: upper ecx = this + 4 (left by 0x00623360, which keeps ecx).
        // The pushed dword is ecx (32-bit); the callback reads only cl, the upper bits are the original's garbage
        // (x64: not reproduced, the pointer does not fit).
        const u32 pushed = arg;

        using Cb = i32(JOAN_CDECL*)(void*, u32);
        if (reinterpret_cast<Cb>(jo::self<jo::Scroll>(this)->callback)(this, pushed) != 0) {  // a code address
            jo::self<jo::Scroll>(this)->pressed = 1;
            return 1;
        }
        return 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Button::fire", 0x0062cc60, &H::button_fire);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b100_12.cpp (game/b100_12)
namespace {
namespace game_b100_12 {

void* const kScale = obj(d::g_misc);

// 2D scale of a widget rectangle (0x00633e60 on 0xa46128, ret 0x30).
void scale_rect(i16 x, i16 y, u32 w, u32 h, float* sx, float* sy, i16* px, i16* py, i16* pw, i16* ph) {
    joan::orig::as_sprite2d(kScreen)->fit_rect(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::i16(w), joan::orig::arg::i16(h), joan::orig::arg::mptr(sx), joan::orig::arg::mptr(sy), joan::orig::arg::mptr(px), joan::orig::arg::mptr(py), joan::orig::arg::mptr(pw), joan::orig::arg::mptr(ph), joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));
}
i16 round_scaled(i16 v, float scale) {
    const double d = Fpu{true}.mul(static_cast<double>(v), static_cast<double>(scale));
    return joan::orig::arg::as<i16>(joan::orig::as_misc(kScale)->round(joan::orig::arg::f64(d)));  // 0x00590a20
}

JOAN_COUNTER(c_text_bottom, "Widget::text_bottom");

struct H {
    // ------------------------------------------------------------------ 0x0062d130
    u32 text_bottom() {
        c_text_bottom.hit();
        i16 x = jo::self<jo::Scroll>(this)->x, y = jo::self<jo::Scroll>(this)->y, w = jo::self<jo::Scroll>(this)->x2, h = jo::self<jo::Scroll>(this)->y2;
        float sx = 1.0f, sy = 1.0f;
        if (jo::self<jo::Scroll>(this)->fit != 0) scale_rect(jo::self<jo::Scroll>(this)->x, jo::self<jo::Scroll>(this)->y, static_cast<u16>(jo::self<jo::Scroll>(this)->x2),
                                        static_cast<u16>(jo::self<jo::Scroll>(this)->y2), &sx, &sy, &x, &y, &w, &h);
        if (joan::orig::arg::as<i32>(joan::orig::as_scroll(this)->at_end_62cf70()) != 0) return static_cast<u16>(h);  // 0x0062cf70
        const i32 hi = h, lo = y;
        const i32 a = y + static_cast<i32>(round_scaled(jo::view<i16>(jo::self<jo::Scroll>(this)->id), sy));
        u32 num = static_cast<u32>(jo::self<jo::Scroll>(this)->pos) + static_cast<u32>(jo::self<jo::Scroll>(this)->page) - static_cast<u32>(jo::self<jo::Scroll>(this)->min);
        u32 span = static_cast<u32>(static_cast<i32>(h) - static_cast<i32>(y) + 1);
        span -= static_cast<u32>(static_cast<i32>(round_scaled(jo::view<i16>(jo::self<jo::Scroll>(this)->id), sy)));
        num *= span;
        const i32 den = static_cast<i32>(static_cast<u32>(jo::self<jo::Scroll>(this)->max) - static_cast<u32>(jo::self<jo::Scroll>(this)->min) +
                                         static_cast<u32>(static_cast<i32>(jo::self<jo::Scroll>(this)->extra)));
        const i32 q = static_cast<i32>(num) / den;
        const i32 v = static_cast<i32>(static_cast<u32>(a) + static_cast<u32>(q) - 1u);
        return joan::orig::arg::as<u32>(joan::orig::clamp_int(joan::orig::arg::i32(v), joan::orig::arg::i32(lo), joan::orig::arg::i32(hi)));  // 0x0062c200
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("Widget::text_bottom", 0x0062d130, &H::text_bottom);
#undef B

}  // namespace game_b100_12
}  // namespace

// ---------------------------------------- from modules/game_b82_11.cpp (game/b82_11)
namespace {
namespace game_b82_11 {

constexpr img::Addr kSprites = d::g_sprites_a46128;   // 2D sprite queue

constexpr img::Addr kMath = d::g_misc;     // 0x00590a20 (round) / 0x00591050 (time)

JOAN_COUNTER(c_edit, "TextBox::draw");

std::int32_t s16(std::uint32_t v) { return static_cast<std::int16_t>(v); }

struct H {
    // ---------------------------------------------------------------- 0x0062d730
    void draw_edit(std::uint32_t arg, std::uint32_t show_text, std::uint32_t show_caret, std::uint32_t flags) {
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_widget_child(&jo::self<jo::TextBox>(this)->child)->visible()) == 0) return;  // 0x00623360
        const std::uint32_t fl = flags & 0xff;
        if (fl != 7 && (fl & 1) != 0) return;
        c_edit.hit();
        std::int16_t x = jo::self<jo::TextBox>(this)->x, y = jo::self<jo::TextBox>(this)->y, w = jo::self<jo::TextBox>(this)->x2;
        std::int16_t h = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_text_box(this)->bottom()));  // 0x0062ea30
        float sx = 1.0f, sy = 1.0f;  // [ebp-0x18], [ebp-0x1c]
        if (jo::self<jo::TextBox>(this)->fit != 0) {
            const std::int32_t full_h = joan::orig::arg::as<std::int32_t>(joan::orig::as_text_box(this)->bottom());  // 0x0062ea30 (eax pushed whole)
            joan::orig::as_sprite2d(obj(kSprites))->fit_rect(joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->x))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->y))), joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->x2))), joan::orig::arg::i16(full_h), &sx, &sy, &x, &y, &w, &h, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
        }
        joan::orig::as_text_box(this)->clamp_cursor();  // 0x0062ea70
        if ((show_text & 0xff) != 0 && (fl & 4) != 0) {
            const std::uint8_t kind = jo::self<jo::TextBox>(this)->align;
            if (kind == 1 || kind == 2 || kind == 3) {
                const std::int32_t alpha = joan::hook::x87::ftol(
                    Fpu{true}.mul(static_cast<double>(jo::self<jo::TextBox>(this)->colour[3]), static_cast<double>(jo::self<jo::TextBox>(this)->scale)));
                std::uint32_t tmp = 0;  // [ebp-0x40] / [ebp-0x48] / [ebp-0x50]
                const std::uint32_t colour =
                    *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&tmp)->construct(static_cast<std::uint32_t>(jo::self<jo::TextBox>(this)->colour[0]), static_cast<std::uint32_t>(jo::self<jo::TextBox>(this)->colour[1]), static_cast<std::uint32_t>(jo::self<jo::TextBox>(this)->colour[2]), alpha));  // 0x0042a260
                if (kind == 1) {
                    joan::orig::as_text(jo::view<void*>(jo::self<jo::TextBox>(this)->font))->draw_line(static_cast<std::int32_t>(x), static_cast<std::int32_t>(y), joan::orig::arg::u32(arg), jo::self<jo::TextBox>(this)->text, joan::orig::arg::u32(colour), static_cast<std::int32_t>(w), 0, joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->text_7c))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->text_7e))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->text_80))), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x006255d0
                } else if (kind == 2) {
                    joan::orig::as_text(jo::view<void*>(jo::self<jo::TextBox>(this)->font))->draw_3(static_cast<std::int32_t>(x), static_cast<std::int32_t>(y), static_cast<std::int32_t>(w), static_cast<std::int32_t>(h), joan::orig::arg::u32(arg), jo::self<jo::TextBox>(this)->text, joan::orig::arg::u32(colour), 0, joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->text_7c))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->text_7e))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->text_80))), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x00626130
                } else {
                    joan::orig::as_text(jo::view<void*>(jo::self<jo::TextBox>(this)->font))->draw_2(static_cast<std::int32_t>(w), static_cast<std::int32_t>(y), joan::orig::arg::u32(arg), jo::self<jo::TextBox>(this)->text, joan::orig::arg::u32(colour), 0, joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->text_7c))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->text_7e))), joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->text_80))), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x00626080
                }
            }
        }
        if ((fl & 2) == 0) return;
        if (jo::self<jo::TextBox>(this)->focused == 0) return;
        const std::int32_t now = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(kMath))->get_time_591050());  // 0x00591050
        const std::int32_t dt = now - global<std::int32_t>(d::g_a69f28);
        if (dt >= 400 || dt <= 0) global<std::int32_t>(d::g_a69f28) = now;
        if (dt >= 200 && (show_caret & 0xff) != 0) {
            const std::int32_t cx =
                static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_text_box(this)->cursor_x_62e6e0(joan::orig::arg::i16(s16(jo::view<std::uint16_t>(jo::self<jo::TextBox>(this)->cursor))))));  // 0x0062e6e0
            if (cx <= static_cast<std::int32_t>(w) && cx >= static_cast<std::int32_t>(x)) {
                joan::orig::as_sprite2d(obj(kSprites))->queue_line(joan::orig::arg::i16(cx), joan::orig::arg::i16(static_cast<std::int32_t>(y)), joan::orig::arg::i16(cx), joan::orig::arg::i16(static_cast<std::int32_t>(h)), joan::orig::arg::i32(arg), joan::orig::arg::u32(jo::self<jo::TextBox>(this)->caret_colour), joan::orig::arg::u32(jo::self<jo::TextBox>(this)->caret_colour));  // 0x00632100
            }
        }
        const std::int32_t first = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_text_box(this)->sel_min()));  // 0x00631570
        const std::int32_t last = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_text_box(this)->sel_max()));   // 0x006315d0
        if (first >= last) return;
        const std::int32_t x1 = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_text_box(this)->cursor_x_62e6e0(joan::orig::arg::i16(first))));  // 0x0062e6e0
        const std::int32_t x2 = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_text_box(this)->cursor_x_62e6e0(joan::orig::arg::i16(last))));   // 0x0062e6e0
        joan::orig::as_sprite2d(obj(kSprites))->queue_line(joan::orig::arg::i16(x1), joan::orig::arg::i16(static_cast<std::int32_t>(h)), joan::orig::arg::i16(x2), joan::orig::arg::i16(static_cast<std::int32_t>(h)), joan::orig::arg::i32(arg), joan::orig::arg::u32(jo::self<jo::TextBox>(this)->caret_colour), joan::orig::arg::u32(jo::self<jo::TextBox>(this)->caret_colour));  // 0x00632100
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b82_11", name, addr, fn)
B11("TextBox::draw", 0x0062d730, &H::draw_edit);
#undef B11

}  // namespace game_b82_11
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Slider::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_06::H::widget62b_ctor>(this)); }  // 0x62b6c0
auto joan::orig::Slider::destruct() -> void { joan::bind::member<&::game_b85_05::H::dtor_62b7d0>(this); }  // 0x62b7d0
auto joan::orig::TextBox::init(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::int16_t a4, std::uintptr_t a5, std::uint8_t a6, std::uint8_t a7, std::uint8_t a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_03::H::textbox_init>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x62b8d0
auto joan::orig::TextBox::reset_62b9a0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_07::H::reset_62b9a0>(this)); }  // 0x62b9a0
auto joan::orig::TextBox::drag() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_06::H::drag>(this)); }  // 0x62b9e0
auto joan::orig::Widget::try_activate(char a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_ui7::H::try_activate>(this, a0)); }  // 0x62bd30
auto joan::orig::Scroll::set_62be10(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b91_03::H::scroll_set>(this, a0, a1, a2); }  // 0x62be10
auto joan::orig::Scroll::set_pos(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_05::H::scroll_set_pos>(this, a0)); }  // 0x62be50
auto joan::orig::Scroll::at_end_62c040() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_03::H::scroll_at_end>(this)); }  // 0x62c040
auto joan::orig::EditBox::cursor_column() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_05::H::cursor_column>(this)); }  // 0x62c080
auto joan::orig::clamp_int(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::clamp_int>(a0, a1, a2)); }  // 0x62c200
auto joan::orig::Gadget::scroll_pos() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b94_06::H::scroll_pos>(this)); }  // 0x62c250
auto joan::orig::TextBox::scroll_x(std::int16_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_06::H::scroll_x>(this, a0)); }  // 0x62c410
auto joan::orig::Scroll::value() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_06::H::scroll_value>(this)); }  // 0x62c5a0
auto joan::orig::TextBox::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_11::H::ctor_62c610>(this)); }  // 0x62c610
auto joan::orig::Scroll::destruct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_03::H::base_dtor_62c720>(this)); }  // 0x62c720
auto joan::orig::TextBox::setup(std::uint16_t a0, std::uint16_t a1, std::uint16_t a2, std::uint16_t a3, std::uint32_t a4, std::uintptr_t a5, std::uint8_t a6, std::uint8_t a7, std::uint8_t a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_06::H::setup>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x62c750
auto joan::orig::Scroll::reset_8e() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_06::H::reset_8e>(this)); }  // 0x62c8f0
auto joan::orig::Button::fire(std::uint8_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_03::H::button_fire>(this, a0)); }  // 0x62cc60
auto joan::orig::Scroll::set_62cd40(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b91_03::H::scroll_set>(this, a0, a1, a2); }  // 0x62cd40
auto joan::orig::EditBox::set_top(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05::H::set_top>(this, a0)); }  // 0x62cd80
auto joan::orig::Scroll::set_end(std::uint32_t a0) -> void { joan::bind::member<&::game_b91_03::H::scroll_set_end>(this, a0); }  // 0x62ced0
auto joan::orig::Scroll::at_end_62cf70() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_03::H::scroll_at_end>(this)); }  // 0x62cf70
auto joan::orig::TextBox::thumb_pos() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_07::H::thumb_pos>(this)); }  // 0x62cfb0
auto joan::orig::Widget::text_bottom() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_12::H::text_bottom>(this)); }  // 0x62d130
auto joan::orig::TextBox::scroll_y(std::int16_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_06::H::scroll_y>(this, a0)); }  // 0x62d2f0
auto joan::orig::EditBox::bottom_line() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05::H::bottom_line>(this)); }  // 0x62d480
auto joan::orig::EditBox::init(std::int32_t a0, std::int32_t a1, std::int32_t a2, char* a3, std::int32_t a4, void* a5, std::uint32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10, std::int32_t a11) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_05::H::editbox_init>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }  // 0x62d4f0
auto joan::orig::Text::draw_aligned(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, char* a4, std::int16_t a5, void* a6, std::uint32_t a7, std::int16_t a8, std::int16_t a9, std::int16_t a10, std::uint32_t a11, std::uint8_t a12) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_06::H::draw_aligned>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)); }  // 0x62d630
auto joan::orig::TextBox::draw(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> void { joan::bind::member<&::game_b82_11::H::draw_edit>(this, a0, a1, a2, a3); }  // 0x62d730
