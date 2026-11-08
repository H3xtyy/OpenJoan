// en2d/popupwin.cpp: functions of the original d:\ww\en2d\popupwin.cpp (re/attribution.csv).
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
#include <cstdarg>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b101_12.cpp (game/b101_12)
//
//    - PopUpWin::open 0x0062a670 (x, y, w, h, no_cancel, a6, no_ok, len), ret 0x20: OK /
//      cancel buttons (0x006211d0 / 0x00623bd0 / 0x0062b660), text buffer len + 1 (heap,
//      "popupwin.cpp", line [0xa19760] + 0x31). Returns the buffer (eax).
namespace {
namespace game_b101_12 {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;


void* const kHeap = obj(d::g_mem);

void* heap_alloc(i32 size, const char* file, i32 line) {
    return joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}

JOAN_COUNTER(c_popup_open, "PopUpWin::open");

struct H {
    // 0x0062a670
    void* popup_open(u32 x, u32 y, u32 w, u32 h, u32 no_cancel, void* a6, u32 no_ok, u32 len) {
        c_popup_open.hit();
        jo::self<jo::PopUpWin>(this)->flags = static_cast<u8>(jo::self<jo::PopUpWin>(this)->flags & 0xfe);
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->x) = static_cast<u16>(x);
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->y) = static_cast<u16>(y);
        if (static_cast<i32>(static_cast<i16>(w)) < 0x10d)
            jo::view<u16>(jo::self<jo::PopUpWin>(this)->w) = 0x10d;
        else
            jo::view<u16>(jo::self<jo::PopUpWin>(this)->w) = static_cast<u16>(w);
        if (static_cast<i32>(static_cast<i16>(h)) < 0x7f)
            jo::view<u16>(jo::self<jo::PopUpWin>(this)->h) = 0x7f;
        else
            jo::view<u16>(jo::self<jo::PopUpWin>(this)->h) = static_cast<u16>(h);
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->x2) = static_cast<u16>(static_cast<i32>(jo::self<jo::PopUpWin>(this)->x) + static_cast<i32>(jo::self<jo::PopUpWin>(this)->w) - 1);
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->y2) = static_cast<u16>(static_cast<i32>(jo::self<jo::PopUpWin>(this)->y) + static_cast<i32>(jo::self<jo::PopUpWin>(this)->h) - 1);
        jo::self<jo::PopUpWin>(this)->font = static_cast<jo::Font2d*>(a6);
        jo::self<jo::PopUpWin>(this)->sprite_set = img::obj(d::g_bmp_res_popupwin);
        joan::orig::as_menu_button(&jo::self<jo::PopUpWin>(this)->ok_button)->setup(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::PopUpWin>(this)->x2) - 0x47), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::PopUpWin>(this)->y2) - 0x28), joan::orig::arg::mptr(jo::self<jo::PopUpWin>(this)->sprite_set), str(d::s_OKU), str(d::s_okD), str(d::s_OKH), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));                                          // 0x006211d0
        joan::orig::as_button(&jo::self<jo::PopUpWin>(this)->ok_button)->set_name8(str(d::s_PopUpOk));  // 0x00623bd0 "PopUpOk"
        joan::orig::as_menu_button(&jo::self<jo::PopUpWin>(this)->cancel_button)->setup(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::PopUpWin>(this)->x2) - 0x29), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::PopUpWin>(this)->y2) - 0x28), joan::orig::arg::mptr(jo::self<jo::PopUpWin>(this)->sprite_set), str(d::s_cancelU), str(d::s_cancelD), str(d::s_cancelH), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));
        joan::orig::as_button(&jo::self<jo::PopUpWin>(this)->cancel_button)->set_name8(str(d::s_PopUpCan));  // "PopUpCan"
        jo::view<u8>(jo::self<jo::PopUpWin>(this)->has_ok) = static_cast<u8>(no_ok);
        if (jo::self<jo::PopUpWin>(this)->has_ok == 0) joan::orig::as_widget(&jo::self<jo::PopUpWin>(this)->ok_button)->reset_child();  // 0x0062b660
        jo::view<u8>(jo::self<jo::PopUpWin>(this)->has_cancel) = static_cast<u8>(no_cancel);
        if (jo::self<jo::PopUpWin>(this)->has_cancel == 0) joan::orig::as_widget(&jo::self<jo::PopUpWin>(this)->cancel_button)->reset_child();
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->text_len) = static_cast<u16>(len);
        const i32 line = static_cast<i32>(global<i16>(d::g_a19760)) + 0x31;
        void* const buf = heap_alloc(static_cast<i32>(static_cast<i16>(len)) + 1, str(d::s_d_ww_en2d_popupwin_cpp), line);
        jo::view<void*>(jo::self<jo::PopUpWin>(this)->text) = buf;
        return buf;
    }
};

#define R12(name, addr, fn) JOAN_REPLACE("game/b101_12", name, addr, fn)
R12("PopUpWin::open", 0x0062a670, &H::popup_open);
#undef R12

}  // namespace game_b101_12
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x0062a8e0 PopupWin::release()                           thiscall
namespace {
namespace game_b92_05 {

void* const kMem = obj(d::g_mem);
void* const kTexMgr = obj(d::g_texture_array);

// Memory manager free / realloc with source file and line (0x0058ed70 / 0x0058eb30).
void mem_free(void* p, img::Addr file, std::int32_t line) {
    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}
using joan::orig::helpers::line_base;

struct H {
    // 0x0062a8e0
    void popup_release() {
        joan::orig::as_menu_button(&jo::self<jo::PopUpWin>(this)->cancel_button)->clear();  // 0x006217e0
        joan::orig::as_menu_button(&jo::self<jo::PopUpWin>(this)->ok_button)->clear();
        if (jo::self<jo::PopUpWin>(this)->texture != nullptr) {
            joan::orig::as_texture_manager(kTexMgr)->release(joan::orig::arg::mptr(jo::self<jo::PopUpWin>(this)->texture));  // 0x0060a550
            jo::self<jo::PopUpWin>(this)->texture = nullptr;
        }
        if (jo::self<jo::PopUpWin>(this)->text != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::PopUpWin>(this)->text), d::s_d_ww_en2d_popupwin_cpp, line_base(d::g_a19764) + 0xe);  // 'd:\ww\en2d\popupwin.cpp'
            jo::self<jo::PopUpWin>(this)->text = nullptr;
        }
        jo::self<jo::PopUpWin>(this)->font = nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("PopupWin::release", 0x0062a8e0, &H::popup_release);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
//
//    - Obj::dtor_62a5f0 0x0062a5f0 (SEH frame for member dtors only).
namespace {
namespace game_b97_06 {

using joan::call::thiscall;

struct H {
    // 0x0062a5f0
    std::uintptr_t dtor_62a5f0() {
        joan::orig::as_popup(this)->release();  // 0x0062a8e0
        joan::orig::as_widget(&jo::self<jo::PopUpWin>(this)->cancel_button)->dtor_6211a0();  // 0x006211a0
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_widget(&jo::self<jo::PopUpWin>(this)->ok_button)->dtor_6211a0());  // 0x006211a0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::dtor_62a5f0", 0x0062a5f0, &H::dtor_62a5f0);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b82_11.cpp (game/b82_11)
//
//      - Widget::reset_child 0x0062b660 (ret 0): 0x0062b6a0 on +4, +0x8c = 0 (eax = this).
namespace {
namespace game_b82_11 {

using joan::call::thiscall;

struct H {
    // ---------------------------------------------------------------- 0x0062b660
    void* reset_child() {
        joan::orig::as_widget_child(&jo::self<jo::Button>(this)->child)->clear_flag();  // 0x0062b6a0
        jo::self<jo::Button>(this)->drawn = 0;
        return this;
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b82_11", name, addr, fn)
B11("Widget::reset_child", 0x0062b660, &H::reset_child);
#undef B11

}  // namespace game_b82_11
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using u8 = std::uint8_t;

void* crt_memset(void* d, i32 c, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(c), joan::orig::arg::u32(n)); }

struct H {
    // ------------------------------------------------------------------ 0x0062a560
    H* screen_ctor() {
        joan::orig::as_widget(&jo::self<jo::PopUpWin>(this)->ok_button)->ctor_620ee0();   // 0x00620ee0
        joan::orig::as_widget(&jo::self<jo::PopUpWin>(this)->cancel_button)->ctor_620ee0();  // 0x00620ee0
        crt_memset(this, 0, sizeof(jo::PopUpWin));
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("Screen::ctor_62a560", 0x0062a560, &H::screen_ctor);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_ui7.cpp (game/ui7)
namespace {
namespace game_ui7 {

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

constexpr img::Addr kSprites = d::g_sprites_a46128;   // 2D sprite queue

using i16 = std::int16_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_frame, "Panel::draw_frame");

// 0x0042a260 Color(r, g, b, a) on a 4-byte local (ret 0x10), the dword read back.
std::uint32_t color(std::int32_t r, std::int32_t g, std::int32_t b, std::int32_t a) {
    std::uint32_t c = 0;
    std::uint32_t* const p = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
    return *p;
}
std::uint32_t white() { return color(0xff, 0xff, 0xff, 0xff); }

// 0x00633500 named frame piece (17 args, ret 0x44).
void piece(std::int32_t x, std::int32_t y, std::uint32_t arg, img::Addr name, void* res, std::uint32_t colour,
           std::int32_t x2, std::int32_t y2) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(arg), joan::orig::arg::str(str(name)), joan::orig::arg::mptr(res), joan::orig::arg::u32(colour), 3, 0, 0, 0, -1, -1, -1, -1, 0, joan::orig::arg::i16(x2), joan::orig::arg::i16(y2));  // 0x00633500
}

struct H {
    // 0x0062a9a0 Panel::draw_frame. ret 8.
    void draw_frame(std::uint32_t arg, std::uint8_t flags) {
        if ((jo::self<jo::PopupWin>(this)->flags & 1) == 0) return;
        c_frame.hit();
        const std::uint32_t box_colour = white();  // ebp-0x8
        const std::uint32_t edge = white();        // ebp-0xc
        const std::int16_t cw = 0x10, rw = 0x67, th = 0x18, bh = 0x33;
        std::int16_t x = jo::self<jo::PopupWin>(this)->x;
        std::int16_t y = jo::self<jo::PopupWin>(this)->y;
        const std::int32_t x2 = jo::self<jo::PopupWin>(this)->x2;
        const std::int32_t y2 = jo::self<jo::PopupWin>(this)->y2;
        const bool pieces = (flags & 1) != 0;
        if (pieces) {
            const std::uint32_t c = white();
            piece(x, y, arg, d::s_TopLe, jo::self<jo::PopupWin>(this)->sprite_set, c, -1, -1);  // "TopLe"
        }
        x = static_cast<std::int16_t>(x + cw);
        if (pieces) piece(x, y, arg, d::s_TopCe, jo::self<jo::PopupWin>(this)->sprite_set, edge, x2 - rw - 1, -1);  // "TopCe"
        if (pieces) {
            const std::uint32_t c = white();
            piece(x2 - rw, y, arg, d::s_TopRi, jo::self<jo::PopupWin>(this)->sprite_set, c, -1, -1);  // "TopRi"
        }
        x = jo::self<jo::PopupWin>(this)->x;
        y = static_cast<std::int16_t>(y + th);
        if (pieces) piece(x, y, arg, d::s_MidLe, jo::self<jo::PopupWin>(this)->sprite_set, edge, -1, y2 - bh - 1);       // "MidLe"
        if (pieces) piece(x2 - rw, y, arg, d::s_MidRi, jo::self<jo::PopupWin>(this)->sprite_set, edge, -1, y2 - bh - 1);  // "MidRi"
        if (pieces)
            piece(static_cast<std::int32_t>(x) + cw, y, arg, d::s_Ce, jo::self<jo::PopupWin>(this)->sprite_set, edge, x2 - rw - 1, y2 - bh - 1);  // centre
        x = jo::self<jo::PopupWin>(this)->x;
        if (pieces) {
            const std::uint32_t c = white();
            piece(x, y2 - bh, arg, d::s_BotLe, jo::self<jo::PopupWin>(this)->sprite_set, c, -1, -1);  // "BotLe"
        }
        if (pieces) piece(static_cast<std::int32_t>(x) + cw, y2 - bh, arg, d::s_BotCe, jo::self<jo::PopupWin>(this)->sprite_set, edge, x2 - rw - 1, -1);  // "BotCe"
        if (pieces) {
            const std::uint32_t c = white();
            piece(x2 - rw, y2 - bh, arg, d::s_BotRi, jo::self<jo::PopupWin>(this)->sprite_set, c, -1, -1);  // "BotRi"
        }
        joan::orig::as_button(&jo::self<jo::PopupWin>(this)->ok_button)->draw(joan::orig::arg::u32(arg), static_cast<std::uint8_t>(flags));   // 0x00622cc0
        joan::orig::as_button(&jo::self<jo::PopupWin>(this)->cancel_button)->draw(joan::orig::arg::u32(arg), static_cast<std::uint8_t>(flags));  // 0x00622cc0
        if ((flags & 4) != 0) {
            joan::orig::as_text_renderer(jo::view<void*>(jo::self<jo::PopupWin>(this)->font))->draw_wrapped(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::PopupWin>(this)->x) + 0x14), joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::PopupWin>(this)->y) + 0x14), joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::PopupWin>(this)->x2) - 0x14), joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::PopupWin>(this)->y2) - 0x3c), joan::orig::arg::u32(arg), joan::orig::arg::str(jo::self<jo::PopupWin>(this)->text), joan::orig::arg::u32(box_colour), 2u, 0u, 1u, 2u, 0u, 1.0f, 1.0f, 2u);  // 0x00626e00
        }
    }
};

#define U7(name, addr, fn) JOAN_REPLACE("game/ui7", name, addr, fn)
U7("Panel::draw_frame", 0x0062a9a0, &H::draw_frame);
#undef U7

}  // namespace game_ui7
}  // namespace

// ---------------------------------------- from modules/game_b107_18.cpp (game/b107_18)
namespace {
namespace game_b107_18 {

using joan::call::thiscall;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_poll, "Panel::poll_62b050");

struct H {
    // ---- 0x0062b050 ----
    i32 poll_62b050() {
        c_poll.hit();
        if ((jo::self<jo::PopupWin>(this)->flags & 1) == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::PopUpWin>(this)->ok_button)->mouse(0xd, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {  // 0x006218c0
            jo::self<jo::PopupWin>(this)->flags = static_cast<u8>(jo::self<jo::PopupWin>(this)->flags & 0xfe);
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::PopUpWin>(this)->cancel_button)->mouse(0x1b, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {  // 0x006218c0
            jo::self<jo::PopupWin>(this)->flags = static_cast<u8>(jo::self<jo::PopupWin>(this)->flags & 0xfe);
            return -1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_value(static_cast<i32>(jo::self<jo::PopupWin>(this)->x), static_cast<i32>(jo::self<jo::PopupWin>(this)->y), static_cast<i32>(jo::self<jo::PopupWin>(this)->x2), static_cast<i32>(jo::self<jo::PopupWin>(this)->y2), 0)) != 0)  // 0x005945d0
            return 2;
        if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_mode2(static_cast<i32>(jo::self<jo::PopupWin>(this)->x), static_cast<i32>(jo::self<jo::PopupWin>(this)->y), static_cast<i32>(jo::self<jo::PopupWin>(this)->x2), static_cast<i32>(jo::self<jo::PopupWin>(this)->y2), 0)) != 0)  // 0x00594900
            return 2;
        return 0;
    }

    // ---- 0x007069f0 ----
    struct Vtx {
        float x, y, z;
        u32 nx, ny, nz;
        u8 col[4];
        u32 u, v;
    };
    static_assert(sizeof(Vtx) == 0x24, "Vtx");
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_18", name, addr, fn)
B("Panel::poll_62b050", 0x0062b050, &H::poll_62b050);
#undef B

}  // namespace game_b107_18
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

using joan::call::thiscall;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

struct H {
    i32 set_x(u32 v) {  // 0x0062b190
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->x) = static_cast<u16>(v);
        const i32 r = static_cast<i32>(jo::self<jo::PopUpWin>(this)->x) + static_cast<i32>(jo::self<jo::PopUpWin>(this)->w) - 1;
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->x2) = static_cast<u16>(r);
        return r;
    }
    i32 set_y(u32 v) {  // 0x0062b1e0
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->y) = static_cast<u16>(v);
        const i32 r = static_cast<i32>(jo::self<jo::PopUpWin>(this)->y) + static_cast<i32>(jo::self<jo::PopUpWin>(this)->h) - 1;
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->y2) = static_cast<u16>(r);
        return r;
    }
    i32 set_text(const char* s) {  // 0x0062b4b0
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::PopUpWin>(this)->text)), joan::orig::arg::str(s), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::PopUpWin>(this)->text_len)));  // strncpy
        const i32 n = static_cast<i32>(jo::self<jo::PopUpWin>(this)->text_len);
        jo::self<jo::PopUpWin>(this)->text[n] = 0;
        return n;
    }
    u32 show() {  // 0x0062b5e0
        jo::self<jo::PopupWin>(this)->flags = static_cast<u8>(jo::self<jo::PopupWin>(this)->flags | 1);
        return joan::orig::arg::as<u32>(joan::orig::as_popup(this)->ensure_window());  // 0x0062b510
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Rect16::set_x", 0x0062b190, &H::set_x);
B("Rect16::set_y", 0x0062b1e0, &H::set_y);
B("TextBox::set_text", 0x0062b4b0, &H::set_text);
B("Popup::show", 0x0062b5e0, &H::show);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

using joan::hook::global;

JOAN_COUNTER(c_set_w, "Slider::set_w_62b230");
JOAN_COUNTER(c_set_h, "Slider::set_h_62b300");

struct H {
    // ------------------------------------------------------------------ 0x0062b230 / 0x0062b300
    u32 move_children() {
        joan::orig::as_button(&jo::self<jo::PopUpWin>(this)->ok_button)->set_rect(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::PopUpWin>(this)->x2) - 0x47), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::PopUpWin>(this)->y2) - 0x28), joan::orig::arg::u32(-1), joan::orig::arg::u32(-1), 0u);  // 0x00623580
        return joan::orig::arg::as<u32>(joan::orig::as_button(&jo::self<jo::PopUpWin>(this)->cancel_button)->set_rect(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::PopUpWin>(this)->x2) - 0x29), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::PopUpWin>(this)->y2) - 0x28), joan::orig::arg::u32(-1), joan::orig::arg::u32(-1), 0u));
    }
    u32 set_w(u32 w) {
        c_set_w.hit();
        jo::self<jo::PopUpWin>(this)->x = static_cast<i16>((global<i32>(d::g_screen_w) - static_cast<i16>(w)) / 2);
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->w) = static_cast<u16>(w);
        jo::self<jo::PopUpWin>(this)->x2 = static_cast<i16>(static_cast<i32>(jo::self<jo::PopUpWin>(this)->x) + static_cast<i16>(w) - 1);
        return move_children();
    }
    u32 set_h(u32 h) {
        c_set_h.hit();
        jo::self<jo::PopUpWin>(this)->y = static_cast<i16>((global<i32>(d::g_screen_h) - static_cast<i16>(h)) / 2);
        jo::view<u16>(jo::self<jo::PopUpWin>(this)->h) = static_cast<u16>(h);
        jo::self<jo::PopUpWin>(this)->y2 = static_cast<i16>(static_cast<i32>(jo::self<jo::PopUpWin>(this)->y) + static_cast<i16>(h) - 1);
        return move_children();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Slider::set_w_62b230", 0x0062b230, &H::set_w);
B("Slider::set_h_62b300", 0x0062b300, &H::set_h);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

using joan::call::thiscall;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    void nop4(u32) {}

    // 0x0062b3d0
    u32 layout_62b3d0() {
        void* bar = jo::view<void*>(jo::self<jo::PopUpWin>(this)->font);
        joan::orig::as_font(bar)->measure(joan::orig::arg::str(jo::self<jo::PopUpWin>(this)->text), -1, joan::orig::arg::i16(static_cast<i32>(jo::self<jo::PopUpWin>(this)->w) - 0x28), 0, joan::orig::arg::f32(0x3f800000u));  // 0x00624f00
        const i16 h = joan::orig::as_font(jo::view<void*>(jo::self<jo::PopUpWin>(this)->font))->text_height(joan::orig::arg::i16(2u), joan::orig::arg::f32(0x3f800000u));  // 0x006253f0
        return joan::orig::arg::as<u32>(joan::orig::as_slider(this)->set_h_62b300(joan::orig::arg::u32(static_cast<i32>(h) + 0x50)));  // 0x0062b300
    }
};

JOAN_REPLACE("game/b105_17", "Slider::layout_62b3d0", 0x0062b3d0, &H::layout_62b3d0);
JOAN_REPLACE("game/b105_17", "Obj::nop4_62b490", 0x0062b490, &H::nop4);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

void* const kWindows = obj(d::g_texture_array);

struct H {
    // ------------------------------------------------------------------ 0x0062b510
    // eax is passed on by its tail caller 0x0062b5e0 (which has no callers): this when already set, else 0.
    std::uintptr_t ensure_window() {
        if (jo::self<jo::PopupWin>(this)->texture != nullptr) return reinterpret_cast<std::uintptr_t>(this);
        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kWindows)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kWindows)->set_mode(1u);                // 0x0060a740
        jo::self<jo::PopupWin>(this)->texture = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kWindows)->load(str(d::s_POPUPWIN), 1, 0u));  // 0x006099c0 "POPUPWIN"
        joan::orig::as_texture_manager(kWindows)->set_mode_60a740(saved);                     // 0x0060a740
        return 0;  // 0x0060a740 returns nothing; 0x0062b5e0, the only caller, has no callers
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Popup::ensure_window", 0x0062b510, &H::ensure_window);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

using joan::img::obj;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0062b590
    void release_15() {
        if (jo::self<jo::Ctl>(this)->u32_15 == nullptr) return;
        joan::orig::as_texture_manager(obj(d::g_texture_array))->release(jo::self<jo::Ctl>(this)->u32_15);  // 0x0060a550
        jo::self<jo::Ctl>(this)->u32_15 = nullptr;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Ctl::release_15", 0x0062b590, &H::release_15);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0062b620
    u32 clear_bit_e() {
        jo::self<jo::PopUpWin>(this)->flags = static_cast<u8>(jo::self<jo::PopUpWin>(this)->flags & 0xfe);
        joan::orig::as_popup(this)->release_texture();  // 0x0062b590
        return 0;  // 0x0062b590 returns nothing; callers of 0x0062b620 discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::clear_bit_e", 0x0062b620, &H::clear_bit_e);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
namespace {
namespace game_b83_05 {

struct H {
    // 0x0062b6a0. ret 0.
    void* clear_4() {
        jo::self<jo::WidgetChild>(this)->flag_4 = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("Overlay2D::clear_flag", 0x0062b6a0, &H::clear_4);
#undef B

}  // namespace game_b83_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Popup::open(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, Font2d* a5, std::uint32_t a6, std::uint32_t a7) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b101_12::H::popup_open>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x62a670
auto joan::orig::Popup::release() -> void { joan::bind::member<&::game_b92_05::H::popup_release>(this); }  // 0x62a8e0
auto joan::orig::Panel::draw_frame(std::uint32_t a0, std::uint8_t a1) -> void { joan::bind::member<&::game_ui7::H::draw_frame>(this, a0, a1); }  // 0x62a9a0
auto joan::orig::Panel::poll_62b050() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b107_18::H::poll_62b050>(this)); }  // 0x62b050
auto joan::orig::Slider::set_h_62b300(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_11::H::set_h>(this, a0)); }  // 0x62b300
auto joan::orig::Popup::ensure_window() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b98_06::H::ensure_window>(this)); }  // 0x62b510
auto joan::orig::Popup::release_texture() -> void { joan::bind::member<&::game_b96_06::H::release_15>(this); }  // 0x62b590
auto joan::orig::Popup::close() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_11::H::clear_bit_e>(this)); }  // 0x62b620
auto joan::orig::Widget::reset_child() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_11::H::reset_child>(this)); }  // 0x62b660
auto joan::orig::WidgetChild::clear_flag() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_05::H::clear_4>(this)); }  // 0x62b6a0
