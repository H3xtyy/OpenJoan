// The key bindings page's control set: Keyboard / Gamepad (remap_set.hpp).
//
// A "< value >" selector of the game's, as the category selector left of it (OptMenu::credit_combo_100: a ComboBox in
// mode 2, 0x006b22f0 / 0x006b29b0), narrowed to the key column under it: the right arrow moves in and the bar (the
// PAGE_OT / PAGE_OV sprite, 0x1d0 wide) is drawn as its two ends, the middle left out (draw_bar, in place of
// PageButton::draw_cb 0x006b23b0). It slides with the category selector (CreditsScreen::layout 0x00748540 /
// 0x00748830).

#include "../callconv.hpp"
#include "remap_set.hpp"

#include <cstdint>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

namespace joan::optmenu::remap {
namespace {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::hook::global;
using joan::img::obj;
using M = jo::OptMenu;

constexpr u32 kOne = 0x3f800000u, k6 = 0x40c00000u;            // 1.0f, 6.0f
constexpr u32 k3500 = 0x455ac000u, kM1500 = 0xc4bb8000u;       // 3500.0f, -1500.0f
// Right of the category selector (x 0x74, 0x1d0 wide, y 0xec), to the right edge of the key column (0x244 + 0x146).
constexpr i32 kFull = 0x1d0;                     // a mode 2 selector's width (its bar sprite)
constexpr i32 kX = 0x24c, kW = 0x38a - kX, kY = 0xec;
constexpr i32 kFieldL = 0x4e, kFieldR = kFull - 0x17b;  // the bar sprite's dark text field: its columns 0x4e .. 0x17b

i32 dy() { return global<std::int8_t>(d::g_game_mode) == 9 ? -0x30 : 0; }  // the in-game menu sits 0x30 higher

struct State {
    void* list = nullptr;  // the ComboBox
    i32 mode = -1;         // the set shown, -1 = the input method's
};
State& state() {
    static State s;
    return s;
}

u32 colour(i32 alpha) {
    alignas(4) u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, alpha));  // 0x0042a260
}
void scale(void* w, i32 on, u32 speed, char flag, u32 delay) {
    joan::orig::as_widget(w)->scale_to(joan::orig::arg::i16(on), joan::orig::arg::f32(speed), 0.0f, flag, joan::orig::arg::u32(delay));  // 0x0061f1b0
}
void move(void* w, i32 x, i32 y, u32 f1, u32 f2, char flag, u32 delay) {
    joan::orig::as_widget(w)->move_to_61e020(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(f1), joan::orig::arg::f32(f2), flag, joan::orig::arg::u32(delay));  // 0x0061e020
}

// The input method's keyboard type on INPUT OPTIONS (selection 1 / 2 / 3 = KEYBOARD / KEYBOARD & MOUSE / JOYSTICK).
i32 method_mode(void* menu) {
    switch (jo::self<M>(menu)->key_combo_c0->selected) {
        case 1: return 0;
        case 3: return 1;
        default: return 3;
    }
}
const char* text_of(i32 mode) { return mode == 1 ? "Gamepad" : mode == 0 ? "Keyboard" : "Keyboard & Mouse"; }

// The selector's bar (its `slider` Button), as PageButton::draw_cb 0x006b23b0 but kW wide: the sprite's left half and
// its right end, then the caption centred.
i32 JOAN_CDECL draw_bar(u8* btn, u32 a, u32 flags) {
    if ((flags & 0xff) != 7 && (flags & 0xff & 1) != 0) return 0;
    jo::Button* const b = jo::as_button(btn);
    i16 x1 = b->x, y1 = b->y, x2 = b->x2, y2 = b->y2;
    float sx = 1.0f, sy = 1.0f;
    if (b->fit != 0)
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(static_cast<i32>(b->x)), joan::orig::arg::i16(static_cast<i32>(b->y)), joan::orig::arg::i16(static_cast<i32>(b->x2)), joan::orig::arg::i16(static_cast<i32>(b->y2)), &sx, &sy, &x1, &y1, &x2, &y2, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    const char* name = img::str(d::s_PAGE_OT);
    if (b->enabled != 0 &&
        (jo::view<std::int8_t>(b->held) != 0 || b->pressed != 0 ||
         joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(x1), static_cast<i32>(y1), static_cast<i32>(x2), static_cast<i32>(y2))) != 0))  // 0x00593980
        name = img::str(d::s_PAGE_OV);
    const u32 c = colour(static_cast<i32>(255.0f * b->scale));
    const i32 w = static_cast<i32>(b->x2) - static_cast<i32>(b->x);  // layout width
    const i32 half = w / 2;
    const i32 split = static_cast<i32>(x1) + static_cast<i32>(static_cast<float>(half) * sx + 0.5f);
    const i32 bottom = static_cast<i32>(y1) + static_cast<i32>(0x34 * sy + 0.5f) - 1;
    auto piece = [&](i32 dx0, i32 dx1, i32 u0, i32 u1) {  // source columns u0..u1 of the sprite into dx0..dx1
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(dx0), joan::orig::arg::i16(static_cast<i32>(y1)), a, joan::orig::arg::str(name), b->sprite_set, c, 3, 0, 2, 0,
                                                                         joan::orig::arg::i16(u0), -1, joan::orig::arg::i16(u1), -1, 0, joan::orig::arg::i16(dx1), joan::orig::arg::i16(bottom));  // 0x00633500
    };
    piece(x1, split - 1, 0, half - 1);
    piece(split, x2 - 1, kFull - (w - half), kFull - 1);
    if (jo::view<std::int8_t>(b->caption[0]) != 0) {
        // The caption in the dark field between the arrows (the sprite's columns kFieldL .. kFull - kFieldR), made
        // smaller when it is wider than the narrowed field ("Keyboard & Mouse").
        const float field = static_cast<float>(w - kFieldL - kFieldR - 8) * sx;
        const float tw = static_cast<float>(joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(b->caption), -1, 0, 0, 1.0f));  // 0x00624f00
        const float k = tw > field && tw > 0.0f ? field / tw : 1.0f;
        const i32 px = b->pressed != 0 ? 1 : 0;
        const float centre = static_cast<float>(x1) + static_cast<float>(kFieldL + (w - kFieldL - kFieldR) / 2) * sx;
        const i32 tx = static_cast<i32>(centre - tw * k / 2.0f) + px;
        const i32 ty = static_cast<i32>(y1) + 0xd + static_cast<i32>((1.0f - k) * 10.0f) + px;
        joan::orig::as_text(b->caption_box)->draw_line(joan::orig::arg::i32(tx), joan::orig::arg::i32(ty), joan::orig::arg::u32(a), joan::orig::arg::str(b->caption), joan::orig::arg::u32(c), 0xffffffff, 0, 1, 2, 0, k, k);  // 0x006255d0
    }
    return 1;
}

// A full-width selector made kW wide: the right arrow and the bar's right edge (and their hit areas) move in.
void narrow(void* list) {
    jo::ComboBox* const cb = jo::self<jo::ComboBox>(list);
    const i32 in = kFull - kW;
    auto shift = [](jo::Button* q) {  // `in` is a constant expression: no capture
        q->x = static_cast<i16>(q->x - in);
        q->x2 = static_cast<i16>(q->x2 - in);
        q->hit_rect[0] = static_cast<i16>(q->hit_rect[0] - in);
        q->hit_rect[2] = static_cast<i16>(q->hit_rect[2] - in);
    };
    shift(cb->arrow_down);
    cb->slider->x2 = static_cast<i16>(cb->slider->x2 - in);
    cb->slider->hit_rect[2] = static_cast<i16>(cb->slider->hit_rect[2] - in);
    cb->x2 = static_cast<i16>(cb->x2 - in);
    joan::orig::as_button(cb->slider)->set_callback(reinterpret_cast<std::uintptr_t>(&draw_bar));
}

}  // namespace

i32 mode(void* menu) {
    const State& st = state();
    return st.mode >= 0 ? st.mode : method_mode(menu);
}

void reset(void*) { state().mode = -1; }

void create(void* menu) {
    State& st = state();
    if (st.list != nullptr) return;
    void* const mem = joan::crt::operator_new(joan::orig::arg::u32(static_cast<u32>(sizeof(jo::ComboBox))));  // operator new
    if (mem == nullptr) return;
    st.list = thiscall<void*>(fn::code_value(fn::Widget_9dd7a0_ctor), mem);  // 0x006b22f0
    joan::orig::as_combo_box(st.list)->init_options(joan::orig::arg::u32(kX), joan::orig::arg::u32(kY), i32{2}, joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}));  // 0x006b29b0
    joan::orig::as_combo_box(st.list)->add_6b3930(joan::orig::arg::str("Keyboard"));  // 0x006b3930 (its text: draw)
    joan::orig::as_combo_box(st.list)->add_6b3930(joan::orig::arg::str("Gamepad"));
    joan::orig::as_popup(st.list)->set_text_6b44e0(1u);  // 0x006b44e0
    narrow(st.list);
    hide_now(menu);
}

void release(void*) {
    State& st = state();
    if (st.list == nullptr) return;
    thiscall<u32>(fn::code_value(fn::ComboBox_free_parts), st.list);  // as 0x00749e00 frees a ComboBox
    thiscall<u32>(fn::code_value(fn::Obj741240_scalar_deleting_dtor), st.list, 1u);
    st.list = nullptr;
}

void hide_now(void*) {
    State& st = state();
    if (st.list == nullptr) return;
    move(st.list, kX, dy() + 0x3e, kOne, 0, 1, 0);
    scale(st.list, 0, kOne, 1, 0);
}

void update(void*) {
    if (state().list != nullptr) joan::orig::as_widget(state().list)->update();  // 0x0061f3b0
}

void draw(void* menu) {
    State& st = state();
    if (st.list == nullptr) return;
    // The selection and the caption follow the set (the keyboard set is named after the input method's type).
    const i32 m = mode(menu);
    const u32 want = m == 1 ? 2u : 1u;
    if (jo::view<u32>(jo::self<jo::ComboBox>(st.list)->selected) != want) joan::orig::as_popup(st.list)->set_text_6b44e0(want);  // 0x006b44e0
    joan::orig::as_button(jo::self<jo::ComboBox>(st.list)->slider)->set_text(joan::orig::arg::str(text_of(m)), obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(colour(0xff)), i32{0}, i32{0}, i32{0}, i32{6}, i32{6});  // 0x00623ce0, as 0x006b44e0
    joan::orig::as_combo_box(st.list)->draw(1u, static_cast<std::uint8_t>(7));  // 0x006b3ec0
}

void slide_in(void*, u32 delay) {
    State& st = state();
    if (st.list == nullptr) return;
    scale(st.list, 1, k6, 0, delay);
    move(st.list, kX, dy() + kY, k3500, kM1500, 0, delay);
}
void slide_out(void*, u32 delay) {
    State& st = state();
    if (st.list == nullptr) return;
    scale(st.list, 0, k6, 0, delay);
    move(st.list, kX, dy() + 0x3e, k3500, kM1500, 0, delay);
}

bool input(void* menu) {
    State& st = state();
    if (st.list == nullptr) return false;
    // Hover as the category selector (0x00745b10): the arrows take the clicks while the pointer is on the row.
    const jo::ComboBox* const cb = jo::self<jo::ComboBox>(st.list);
    const bool over = joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(cb->x), joan::orig::arg::i32(cb->y), joan::orig::arg::i32(cb->x2), joan::orig::arg::i32(cb->y2))) != 0;  // 0x00593980
    thiscall<u32>(fn::code_value(fn::Obj6b3_set_flag), st.list, over ? 1 : 0);  // 0x006b3e50
    if (joan::orig::arg::as<i32>(joan::orig::as_menu(st.list)->step_6b4280()) == 0) return false;  // 0x006b4280: an arrow clicked
    st.mode = jo::view<u32>(cb->selected) == 2 ? 1 : (method_mode(menu) == 0 ? 0 : 3);
    return true;
}

}  // namespace joan::optmenu::remap
