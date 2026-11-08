// game/unknown_scrlbox.cpp: functions whose original source file is unknown (formerly game/_unplaced/scrlbox.cpp;
// re/attribution.csv: between ww/interfac/optmenui.cpp and end). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

struct H {
    // ------------------------------------------------------------------ 0x0075fb00
    u32 dtor_75fb00() {
        jo::self<jo::ScrollBox>(this)->vtable = img::obj(d::vt_ScrollBox);
        joan::orig::as_panel(this)->destroy_children();         // 0x00760440
        return joan::orig::arg::as<u32>(joan::orig::as_gadget(this)->destruct());  // 0x0061dfb0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::dtor_75fb00", 0x0075fb00, &H::dtor_75fb00);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

using u8 = std::uint8_t;

i32 JOAN_CDECL arrow_on_click(void* p) {
    jo::as_scroll(p)->pressed = 1;  // p: the Scroll whose callback this is
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("ArrowBtn::on_click_75fb70", 0x0075fb70, &arrow_on_click);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using i8 = std::int8_t;

using i16 = std::int16_t;

using joan::img::obj;

using joan::img::str;

using joan::hook::x87::ftol;

JOAN_COUNTER(c_btn_page, "Button::draw_75fba0");

// 0x0042a260 (thunk 0x004098b8, ret 0x10): builds a 4-byte colour in `c`, returns &c.
u32 make_color(i32 a, i32 b, i32 c, i32 d) {
    u32 color = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&color)->construct(a, b, c, d));
}

// ------------------------------------------------- 0x006edce0 / 0x0075fba0 shared pieces
struct Rect16 {
    i16 x, y, w, h;
    float sx, sy;
};

Rect16 button_layout(void* w) {
    Rect16 r{};
    r.x = jo::as_button(w)->x;
    r.y = jo::as_button(w)->y;
    r.w = jo::as_button(w)->x2;
    r.h = jo::as_button(w)->y2;
    r.sx = 1.0f;
    r.sy = 1.0f;
    if (jo::as_button(w)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(i32{jo::as_button(w)->x}), joan::orig::arg::i16(i32{jo::as_button(w)->y}), joan::orig::arg::i16(i32{jo::as_button(w)->x2}), joan::orig::arg::i16(i32{jo::as_button(w)->y2}), &r.sx, &r.sy, &r.x, &r.y, &r.w, &r.h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    if (jo::view<i8>(jo::as_button(w)->caption[0]) != 0) {
        // Text width / height queries; the results are dead locals in the original.
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(jo::as_button(w)->caption), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(i32{2}), 1.0f);                                   // 0x006253f0
    }
    return r;
}

i32 alpha_of(void* w) { return ftol(255.0f * jo::as_button(w)->scale); }  // 0x9cfb68 = 255.0f

// 0x00633500 (thunk 0x00401bbd, ret 0x44) with the fixed tail (3, 0, 2, 0, -1 x4, 0, -1, -1).
void draw_sprite(const Rect16& r, u32 param, const char* name, jo::BmpRes* set, u32 color) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(i32{r.x}), joan::orig::arg::i16(i32{r.y}), joan::orig::arg::u32(param), joan::orig::arg::str(name), set, joan::orig::arg::u32(color), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);
}

bool hovered(const Rect16& r) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{r.x}, i32{r.y}, i32{r.w}, i32{r.h})) != 0;  // 0x00593980
}

static u32 JOAN_CDECL button_draw_75fba0(void* w, u32 param, u32 flags) {
    c_btn_page.hit();
    if ((flags & 0xff) != 7 && (flags & 1) != 0) return 0;
    Rect16 r = button_layout(w);
    if (jo::as_button(w)->enabled != 0) {
        const char* name = str(d::s_PAGE_OV);  // 'PAGE_OV'
        bool over = true;
        if (jo::view<i8>(jo::as_button(w)->held) == 0 && jo::as_button(w)->pressed == 0) over = hovered(r);
        if (!over) name = str(d::s_PAGE_OT);  // 'PAGE_OT'
        const u32 c = make_color(0xff, 0xff, 0xff, alpha_of(w));
        draw_sprite(r, param, name, jo::as_button(w)->sprite_set, c);
    } else {
        const u32 c = make_color(0xff, 0xff, 0xff, alpha_of(w));
        draw_sprite(r, param, str(d::s_PAGE_OT), jo::as_button(w)->sprite_set, c);  // 'PAGE_OT'
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Button::draw_75fba0", 0x0075fba0, &button_draw_75fba0);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b104_16.cpp (game/b104_16)
namespace {
namespace game_b104_16 {

using u32 = std::uint32_t;

using i16 = std::int16_t;
using i32 = std::int32_t;

JOAN_COUNTER(c_relayout, "ScrollBox::relayout");

struct H {
    // the child the ScrollBox member M points to (read again at every use, as the original does)
    template <auto M>
    void* p() { return jo::self<jo::ScrollBox>(this)->*M; }

    // ------------------------------------------------------------------ 0x007605d0
    // Child M: keep its width/height, move its origin to this->(+9,+b) + (dx,dy).
    template <auto M>
    void move_child(i32 dx, i32 dy) {
        i32 d = static_cast<i32>(jo::as_button(p<M>())->x2) - static_cast<i32>(jo::as_button(p<M>())->x);
        jo::as_button(p<M>())->x = static_cast<i16>(static_cast<i32>(jo::self<jo::ScrollBox>(this)->x) + dx);
        jo::as_button(p<M>())->x2 = static_cast<i16>(static_cast<i32>(jo::as_button(p<M>())->x) + d);
        d = static_cast<i32>(jo::as_button(p<M>())->y2) - static_cast<i32>(jo::as_button(p<M>())->y);
        jo::as_button(p<M>())->y = static_cast<i16>(static_cast<i32>(jo::self<jo::ScrollBox>(this)->y) + dy);
        jo::as_button(p<M>())->y2 = static_cast<i16>(static_cast<i32>(jo::as_button(p<M>())->y) + d);
    }
    template <auto M>
    void copy_own_rect() {
        jo::as_button(p<M>())->hit_rect[0] = jo::as_button(p<M>())->x;
        jo::as_button(p<M>())->hit_rect[2] = jo::as_button(p<M>())->x2;
        jo::as_button(p<M>())->hit_rect[1] = jo::as_button(p<M>())->y;
        jo::as_button(p<M>())->hit_rect[3] = jo::as_button(p<M>())->y2;
        jo::view<u32>(jo::as_button(p<M>())->scale) = jo::view<u32>(jo::self<jo::ScrollBox>(this)->scale);
    }
    void* relayout() {
        if (jo::self<jo::ScrollBox>(this)->built == 0) return this;
        c_relayout.hit();
        jo::as_button(p<&jo::ScrollBox::body>())->x = jo::self<jo::ScrollBox>(this)->x;
        jo::as_button(p<&jo::ScrollBox::body>())->x2 = jo::self<jo::ScrollBox>(this)->x2;
        jo::as_button(p<&jo::ScrollBox::body>())->y = jo::self<jo::ScrollBox>(this)->y;
        jo::as_button(p<&jo::ScrollBox::body>())->y2 = jo::self<jo::ScrollBox>(this)->y2;
        jo::as_button(p<&jo::ScrollBox::body>())->hit_rect[0] = jo::self<jo::ScrollBox>(this)->x;
        jo::as_button(p<&jo::ScrollBox::body>())->hit_rect[2] = jo::self<jo::ScrollBox>(this)->x2;
        jo::as_button(p<&jo::ScrollBox::body>())->hit_rect[1] = jo::self<jo::ScrollBox>(this)->y;
        jo::as_button(p<&jo::ScrollBox::body>())->hit_rect[3] = jo::self<jo::ScrollBox>(this)->y2;
        jo::view<u32>(jo::as_button(p<&jo::ScrollBox::body>())->scale) = jo::view<u32>(jo::self<jo::ScrollBox>(this)->scale);

        move_child<&jo::ScrollBox::bar>(0x50, 0xc);
        jo::view<u32>(jo::as_button(p<&jo::ScrollBox::bar>())->scale) = jo::view<u32>(jo::self<jo::ScrollBox>(this)->scale);

        move_child<&jo::ScrollBox::up>(0x23, 0xb);
        copy_own_rect<&jo::ScrollBox::up>();

        move_child<&jo::ScrollBox::down>(0x186, 0xb);
        copy_own_rect<&jo::ScrollBox::down>();
        return this;
    }
};

#define B104(name, addr, fn) JOAN_REPLACE("game/b104_16", name, addr, fn)
B104("ScrollBox::relayout", 0x007605d0, &H::relayout);
#undef B104

}  // namespace game_b104_16
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

using joan::hook::global;
using u32 = std::uint32_t;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::hook::x87::Fpu;

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

struct H {
    // ------------------------------------------------------------------ 0x00760aa0
    i32 scaled_760aa0() {
        u8* const p = jo::view<u8*>(jo::self<jo::ScrollBox>(this)->bar);
        const double a = static_cast<double>(jo::as_scroll(p)->pos);                                    // fild
        const double d = static_cast<double>(static_cast<i32>(jo::view<u32>(jo::as_scroll(p)->max) - jo::view<u32>(jo::as_scroll(p)->min)));  // fild
        const Fpu fpu{true};
        const double q = fpu.div(global<float>(d::flt_300), d);  // fdivr: 300 / d
        return ftol(fpu.mul(a, q));                               // fmulp
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::scaled_760aa0", 0x00760aa0, &H::scaled_760aa0);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

struct H {
    // 0x00760b00
    void set_flag_760b00(std::int8_t c) {
        if (c > 0) {
            jo::self<jo::ScrollBox>(this)->highlight = 1;
            jo::self<jo::ScrollBox>(this)->body->held = 1;
        } else {
            jo::self<jo::ScrollBox>(this)->highlight = 0;
            jo::self<jo::ScrollBox>(this)->body->held = 0;
        }
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj760::set_flag", 0x00760b00, &H::set_flag_760b00);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using joan::hook::global;
using i8 = std::int8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::img::str;

using joan::hook::x87::Fpu;

using joan::orig::helpers::fild;
using joan::orig::helpers::fd;

constexpr Fpu kFpu{true};

void* const kScreen = obj(d::g_mouse);
void* const kScaler = obj(d::g_sprites_a46128);

// 0x0042a260 Color::Color(r, g, b, a) on a 4-byte local; returns this
u32 color(u32 r, u32 g, u32 b, i32 a) {
    u32 c[2] = {0, 0};
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(c)->construct(r, g, b, a));
}
i32 alpha_of(const void* p) { return joan::hook::x87::ftol(global<float>(d::flt_255) * jo::as_scroll_box(p)->scale); }
i32 box_visible(i32 x0, i32 y0, i32 x1, i32 y1) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(kScreen)->inside(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1)));  // 0x00593980
}

JOAN_COUNTER(c_hl_bar, "HlBar::draw");

struct H {
    // ------------------------------------------------------------------ 0x00760b70
    void hl_draw(i32 b, i32 c) {
        c_hl_bar.hit();
        const i32 state = static_cast<i32>(static_cast<u8>(c));
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::ScrollBox>(this)->bar))->try_activate(static_cast<char>(joan::orig::arg::i32(7)));  // 0x0062bd30
        joan::orig::as_button(jo::view<void*>(jo::self<jo::ScrollBox>(this)->body))->draw(joan::orig::arg::u32(b), static_cast<std::uint8_t>(state));  // 0x00622cc0
        void* const p8c = jo::view<void*>(jo::self<jo::ScrollBox>(this)->body);
        if (box_visible(static_cast<i32>(jo::as_button(p8c)->x), static_cast<i32>(jo::self<jo::ScrollBox>(this)->body->y), static_cast<i32>(jo::self<jo::ScrollBox>(this)->body->x2),
                        static_cast<i32>(jo::self<jo::ScrollBox>(this)->body->y2)) != 0 ||
            jo::self<jo::ScrollBox>(this)->body->pressed != 0 || jo::view<i8>(jo::self<jo::ScrollBox>(this)->body->held) != 0) {
            joan::orig::as_button(jo::view<void*>(jo::self<jo::ScrollBox>(this)->up))->draw(joan::orig::arg::u32(b), static_cast<std::uint8_t>(state));  // 0x00622cc0
            joan::orig::as_button(jo::view<void*>(jo::self<jo::ScrollBox>(this)->down))->draw(joan::orig::arg::u32(b), static_cast<std::uint8_t>(state));  // 0x00622cc0
        }
        void* const bar = jo::view<void*>(jo::self<jo::ScrollBox>(this)->bar);
        const i32 span = static_cast<i32>(jo::view<u32>(jo::as_scroll(bar)->max) - jo::view<u32>(jo::as_scroll(bar)->min));
        double q = kFpu.div(fild(jo::as_scroll(bar)->pos), fild(span));
        q = kFpu.mul(q, fd(d::flt_300));
        q = kFpu.add(q, fd(d::flt_3));
        const i32 len = joan::hook::x87::ftol(q);
        {
            const i32 alpha = alpha_of(this);
            const u32 col = color(0xff, 0xff, 0xff, alpha);
            joan::orig::as_sprite_queue(kScaler)->draw_named(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::ScrollBox>(this)->x) + 0x4c), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::ScrollBox>(this)->y) + 0xa), joan::orig::arg::u32(b), str(d::s_hl_bar), joan::orig::arg::mptr(jo::self<jo::ScrollBox>(this)->u32_78), joan::orig::arg::u32(col), 3, 0, 2, 0, 0, 0, joan::orig::arg::i16(len), 0x18, 0, -1, -1);  // 0x00633500 "hl_bar"
        }
        const i32 alpha = alpha_of(this);
        const u32 col = color(0xff, 0xff, 0xff, alpha);
        const i32 y = static_cast<i32>(jo::self<jo::ScrollBox>(this)->y) + 8;
        const i32 x0 = static_cast<i32>(jo::self<jo::ScrollBox>(this)->x);
        const i32 x = static_cast<i32>(static_cast<u32>(x0) + static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::as_scroll_box(this)->thumb_offset())) + 0x4au);  // 0x00760aa0
        joan::orig::as_sprite_queue(kScaler)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(b), str(d::s_hl_mark), joan::orig::arg::mptr(jo::self<jo::ScrollBox>(this)->u32_78), joan::orig::arg::u32(col), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500 "hl_mark"
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("HlBar::draw", 0x00760b70, &H::hl_draw);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b107_15.cpp (game/b107_15)
namespace {
namespace game_b107_15 {

using joan::hook::global;
using i8 = std::int8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::hook::x87::Fpu;

using joan::hook::global;
using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::orig::helpers::fild;

constexpr Fpu kFpu{true};

JOAN_COUNTER(c_slider_step, "Slider::step_760df0");

struct H {
    // ------------------------------------------------------------------------ 0x00760df0
    i32 auto_repeat_rate() {
        void* const s = jo::view<void*>(jo::self<jo::ScrollBox>(this)->bar);
        const i32 range = static_cast<i32>(jo::view<u32>(jo::as_scroll(s)->max) - jo::view<u32>(jo::as_scroll(s)->min));
        return ftol(kFpu.div(global<double>(d::dbl_9df8a0), kFpu.mul(fild(range), global<double>(d::dbl_0_4))));
    }
    void set_pos(i32 v) { joan::orig::as_scroll_box(this)->set_value(joan::orig::arg::u32(v)); }  // 0x00761240
    bool hit(void* b) {
        return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(static_cast<i32>(jo::as_button(b)->x)), joan::orig::arg::i32(static_cast<i32>(jo::as_button(b)->y)), joan::orig::arg::i32(static_cast<i32>(jo::as_button(b)->x2)), joan::orig::arg::i32(static_cast<i32>(jo::as_button(b)->y2)))) != 0;  // 0x00593980
    }
    i32 step_760df0() {
        c_slider_step.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_text_box(jo::view<void*>(jo::self<jo::ScrollBox>(this)->bar))->drag()) != 0) return 1;  // 0x0062b9e0
        joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::ScrollBox>(this)->body))->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0);  // 0x006218c0
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::ScrollBox>(this)->up))->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {
            if (jo::self<jo::ScrollBox>(this)->bar->pos > jo::self<jo::ScrollBox>(this)->bar->min) set_pos(jo::self<jo::ScrollBox>(this)->bar->pos - 1);
            jo::self<jo::ScrollBox>(this)->step_time = 0;
            return 1;
        }
        if (jo::self<jo::ScrollBox>(this)->up->pressed != 0 && hit(jo::view<void*>(jo::self<jo::ScrollBox>(this)->up))) {
            if (jo::self<jo::ScrollBox>(this)->step_delay >= 300u) {
                jo::self<jo::ScrollBox>(this)->step_time = static_cast<i32>(jo::view<u32>(jo::self<jo::ScrollBox>(this)->step_time) + global<u32>(d::g_frame_elapsed_ms));
                const i32 rate = auto_repeat_rate();
                if (!(jo::self<jo::ScrollBox>(this)->step_time < rate)) {
                    if (jo::self<jo::ScrollBox>(this)->bar->pos > jo::self<jo::ScrollBox>(this)->bar->min)
                        set_pos(jo::self<jo::ScrollBox>(this)->bar->pos - 1);
                    jo::self<jo::ScrollBox>(this)->step_time = static_cast<i32>(jo::view<u32>(jo::self<jo::ScrollBox>(this)->step_time) - static_cast<u32>(rate));
                }
                return 1;
            }
            jo::self<jo::ScrollBox>(this)->step_delay = jo::self<jo::ScrollBox>(this)->step_delay + global<u32>(d::g_frame_elapsed_ms);
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::ScrollBox>(this)->down))->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {
            if (jo::self<jo::ScrollBox>(this)->bar->pos < jo::self<jo::ScrollBox>(this)->bar->max) set_pos(jo::self<jo::ScrollBox>(this)->bar->pos + 1);
            return 1;
        }
        if (jo::self<jo::ScrollBox>(this)->down->pressed != 0 && hit(jo::view<void*>(jo::self<jo::ScrollBox>(this)->down))) {
            if (jo::self<jo::ScrollBox>(this)->step_delay >= 300u) {
                jo::self<jo::ScrollBox>(this)->step_time = static_cast<i32>(jo::view<u32>(jo::self<jo::ScrollBox>(this)->step_time) + global<u32>(d::g_frame_elapsed_ms));
                const i32 rate = auto_repeat_rate();
                if (!(jo::self<jo::ScrollBox>(this)->step_time < rate)) {
                    if (jo::self<jo::ScrollBox>(this)->bar->pos < jo::self<jo::ScrollBox>(this)->bar->max)
                        set_pos(jo::self<jo::ScrollBox>(this)->bar->pos + 1);
                    jo::self<jo::ScrollBox>(this)->step_time = static_cast<i32>(jo::view<u32>(jo::self<jo::ScrollBox>(this)->step_time) - static_cast<u32>(rate));
                }
                return 1;
            }
            jo::self<jo::ScrollBox>(this)->step_delay = jo::self<jo::ScrollBox>(this)->step_delay + global<u32>(d::g_frame_elapsed_ms);
            return 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_15", name, addr, fn)
B("Slider::step_760df0", 0x00760df0, &H::step_760df0);
#undef B

}  // namespace game_b107_15
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04_2 {

struct H {
    // 0x00761270
    std::uint32_t get_761270() { return jo::view<std::uint32_t>(jo::self<jo::ScrollBox>(this)->bar->pos); }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj761::get", 0x00761270, &H::get_761270);
#undef B91

}  // namespace game_b91_04_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ScrollBox::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::dtor_75fb00>(this)); }  // 0x75fb00
auto joan::orig::ScrollBox::thumb_offset() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_16::H::scaled_760aa0>(this)); }  // 0x760aa0
auto joan::orig::ScrollBox::set_highlight(std::int8_t a0) -> void { joan::bind::member<&::game_b91_04::H::set_flag_760b00>(this, a0); }  // 0x760b00
auto joan::orig::ScrollBox::draw(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b108_13::H::hl_draw>(this, a0, a1); }  // 0x760b70
auto joan::orig::Slider::step_760df0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b107_15::H::step_760df0>(this)); }  // 0x760df0
auto joan::orig::ScrollBox::value() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_04_2::H::get_761270>(this)); }  // 0x761270
