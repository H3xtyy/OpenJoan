// The options menu's GRAPHICS OPTIONS page and the openjoan.ini rows of VIDEO OPTIONS (graphics_page.hpp).
//
// The widgets are the game's own classes, made and driven as the original pages make and drive theirs
// (interfac/optmenui.cpp): a row's label is a Button (0x00620ee0, drawn by 0x0073e990), its value a ComboBox in
// mode 2 (0x006b22f0 / 0x006b29b0: "< value >" with arrows); the GRAPHICS OPTIONS button is a Button drawn by
// 0x0073ee40 like the four page buttons. Slides use Widget::scale_to 0x0061f1b0 / move_to 0x0061e020 with the
// original pages' speeds and delays.

#include "graphics_page.hpp"

#include "../platform/win32_types.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../display.hpp"
#include "../original/layout_access.hpp"
#include "../render/ambient_occlusion.hpp"
#include "../render/water.hpp"
#include "../platform/ini.hpp"
#include "../platform/window.hpp"

namespace joan::gld {
void apply_live_settings();  // gl/device.hpp
}

namespace jo = joan::orig;

namespace joan::optmenu::graphics {
namespace {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::hook::global;
using joan::img::obj;
using M = jo::OptMenu;

constexpr u32 kOne = 0x3f800000u, k4 = 0x40800000u, k6 = 0x40c00000u;                // 1.0f, 4.0f, 6.0f
constexpr u32 k3500 = 0x455ac000u, kM1500 = 0xc4bb8000u, k65535 = 0x477fff00u;    // 3500.0f, -1500.0f, 65535.0f
constexpr i32 kLabelX = -0x264, kLabelIn = -100, kValueX = 0x1c0;

i32 dy() { return global<std::int8_t>(d::g_game_mode) == 9 ? -0x30 : 0; }  // the in-game menu sits 0x30 higher

// ---------------------------------------------------------------- the game's widgets
u32 white() {
    alignas(4) u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));  // 0x0042a260
}
void* make(u32 size, std::uintptr_t ctor) {
    void* const mem = joan::crt::operator_new(joan::orig::arg::u32(size));  // operator new
    return mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
}
// A Button at (x0, y0)-(x1, y1) drawn by `draw_cb`, with `text` in the menu font.
void* make_button(i32 x0, i32 y0, i32 x1, i32 y1, std::uintptr_t draw_cb, const char* text) {
    void* const b = make(static_cast<u32>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0));  // 0x00620ee0
    if (b == nullptr) return nullptr;
    joan::orig::as_button(b)->create(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), draw_cb, i32{1}, i32{0}, i32{0}, i32{1}, i32{1}, i32{1});  // 0x00621540
    joan::orig::as_button(b)->set_text(joan::orig::arg::str(text), obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(white()), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
    return b;
}
void* make_list() {
    void* const c = make(static_cast<u32>(sizeof(jo::ComboBox)), fn::code_value(fn::Widget_9dd7a0_ctor));  // 0x006b22f0
    if (c == nullptr) return nullptr;
    joan::orig::as_combo_box(c)->init_options(joan::orig::arg::u32(kValueX), joan::orig::arg::u32(i32{0x3e}), i32{2}, joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}));  // 0x006b29b0
    return c;
}
void scale(void* w, i32 on, u32 speed, char flag, u32 delay) {
    joan::orig::as_widget(w)->scale_to(joan::orig::arg::i16(on), joan::orig::arg::f32(speed), 0.0f, flag, joan::orig::arg::u32(delay));  // 0x0061f1b0
}
void move(void* w, i32 x, i32 y, u32 f1, u32 f2, char flag, u32 delay) {
    joan::orig::as_widget(w)->move_to_61e020(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(f1), joan::orig::arg::f32(f2), flag, joan::orig::arg::u32(delay));  // 0x0061e020
}
// One release block of 0x00749e00: pre-release, then the scalar deleting destructor.
void free_widget(void*& w, fn::Code pre, fn::Code del) {
    if (w == nullptr) return;
    thiscall<u32>(fn::code_value(pre), w);
    thiscall<u32>(fn::code_value(del), w, 1u);
    w = nullptr;
}
bool hovered(void* label) {
    const i16 y0 = jo::self<jo::Button>(label)->y, y1 = jo::self<jo::Button>(label)->y2;
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{0}, joan::orig::arg::i32(y0), i32{0x400}, joan::orig::arg::i32(y1))) != 0;  // 0x00593980
}

// ---------------------------------------------------------------- the rows
enum Id { kResolution, kBorderless, kVSync, kMaxFps, kUiScale, kAnisotropy, kMsaa, kTextureFilter, kAo, kWater, kRows };

struct Choice {
    std::string text;
    i32 value = 0;       // the setting's number
    std::string str;     // Resolution: the setting's text
};

struct Row {
    const char* label = "";
    i32 page = 1;        // 1 VIDEO, kPage GRAPHICS
    i32 slot = 0;        // row on its page (graphics_row / video_row)
    std::vector<Choice> choices;
    i32 selected = 1;    // 1-based, as ComboBox::selected
    void* button = nullptr;
    void* list = nullptr;
};

struct State {
    Row rows[kRows];
    void* tab = nullptr;
    bool built = false, dirty = false;
};
State& state() {
    static State s;
    return s;
}

std::string lower(std::string s) {
    for (char& c : s) c = static_cast<char>(c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c);
    return s;
}

// The primary monitor's modes of at least 1024x720, smallest first.
std::vector<std::pair<i32, i32>> monitor_modes() {
    std::vector<std::pair<i32, i32>> modes;
    for (const joan::platform::ScreenMode& sm : joan::platform::display_modes(0)) {
        const auto m = std::make_pair(static_cast<i32>(sm.width), static_cast<i32>(sm.height));
        if (m.first >= 1024 && m.second >= 720 && std::find(modes.begin(), modes.end(), m) == modes.end()) modes.push_back(m);
    }
    std::sort(modes.begin(), modes.end(), [](const auto& a, const auto& b) { return a.first != b.first ? a.first < b.first : a.second < b.second; });
    return modes;
}

void add(Row& r, const char* text, i32 value) { r.choices.push_back({text, value, {}}); }
// The choice holding `value`, adding "<value><suffix>" when the ini has one the list does not offer.
i32 pick(Row& r, i32 value, const char* suffix) {
    for (std::size_t i = 0; i < r.choices.size(); ++i)
        if (r.choices[i].value == value) return static_cast<i32>(i) + 1;
    r.choices.push_back({std::to_string(value) + suffix, value, {}});
    return static_cast<i32>(r.choices.size());
}

void define_rows() {
    State& st = state();
    const auto& s = joan::hook::settings();
    auto row = [&](Id id, const char* label, i32 page, i32 slot) -> Row& {
        Row& r = st.rows[id];
        r = Row{};
        r.label = label;
        r.page = page;
        r.slot = slot;
        return r;
    };
    {
        Row& r = row(kResolution, "Resolution (restart)", 1, 0);
        r.choices.push_back({"Desktop", 0, "desktop"});
        r.choices.push_back({"1024x768", 0, "original"});
        for (const auto& [w, h] : monitor_modes()) {
            if (w == 1024 && h == 768) continue;
            const std::string t = std::to_string(w) + "x" + std::to_string(h);
            r.choices.push_back({t, 0, t});
        }
        const std::string cur = lower(s.resolution);
        r.selected = 0;
        for (std::size_t i = 0; i < r.choices.size() && r.selected == 0; ++i)
            if (r.choices[i].str == cur || (cur == "1024x768" && r.choices[i].str == "original")) r.selected = static_cast<i32>(i) + 1;
        if (r.selected == 0) {  // a size the monitor does not report (scaled to it)
            r.choices.push_back({s.resolution, 0, cur});
            r.selected = static_cast<i32>(r.choices.size());
        }
    }
    {
        Row& r = row(kBorderless, "Borderless (restart)", 1, 1);
        add(r, "Off", 0);
        add(r, "On", 1);
        r.selected = s.borderless ? 2 : 1;
    }
    {
        Row& r = row(kVSync, "VSync", 1, 5);
        add(r, "Off", 0);
        add(r, "On", 1);
        add(r, "Adaptive", -1);
        r.selected = pick(r, s.vsync, "");
    }
    {
        Row& r = row(kMaxFps, "Max FPS", 1, 6);
        add(r, "Monitor", -1);
        add(r, "Off", 0);
        for (const i32 f : {30, 60, 75, 90, 120, 144, 165, 240}) r.choices.push_back({std::to_string(f), f, {}});
        r.selected = pick(r, s.max_fps < 0 ? -1 : s.max_fps, "");
    }
    {
        // [Display] UiScale in hundredths (0 = auto: from the resolution, display.cpp).
        Row& r = row(kUiScale, "UI Scale", 1, 7);
        add(r, "Auto", 0);
        auto text = [](i32 v) {
            char t[16];
            std::snprintf(t, sizeof t, v % 100 == 0 ? "%.0fx" : (v % 10 == 0 ? "%.1fx" : "%.2fx"), v / 100.0);
            return std::string(t);
        };
        for (const i32 v : {100, 125, 150, 175, 200, 250, 300, 350, 400}) r.choices.push_back({text(v), v, {}});
        const i32 cur = s.ui_scale <= 0.0f ? 0 : static_cast<i32>(s.ui_scale * 100.0f + 0.5f);
        r.selected = 0;
        for (std::size_t i = 0; i < r.choices.size() && r.selected == 0; ++i)
            if (r.choices[i].value == cur) r.selected = static_cast<i32>(i) + 1;
        if (r.selected == 0) {  // a scale of the ini the list does not offer
            r.choices.push_back({text(cur), cur, {}});
            r.selected = static_cast<i32>(r.choices.size());
        }
    }
    {
        Row& r = row(kAnisotropy, "Anisotropy", kPage, 0);
        add(r, "Off", 0);
        for (const i32 a : {2, 4, 8, 16}) r.choices.push_back({std::to_string(a) + "x", a, {}});
        r.selected = pick(r, s.anisotropy < 2 ? 0 : s.anisotropy, "x");
    }
    {
        Row& r = row(kMsaa, "MSAA (restart)", kPage, 1);
        add(r, "Off", 0);
        for (const i32 a : {2, 4, 8}) r.choices.push_back({std::to_string(a) + "x", a, {}});
        r.selected = pick(r, s.msaa < 2 ? 0 : s.msaa, "x");
    }
    {
        Row& r = row(kTextureFilter, "Texture Filter", kPage, 2);
        add(r, "Off", 0);
        add(r, "Trilinear", 1);
        r.selected = s.texture_filter == 1 ? 2 : 1;
    }
    {
        Row& r = row(kAo, "Ambient Occlusion", kPage, 3);
        add(r, "Off", 0);
        add(r, "On", 1);
        r.selected = s.ao_mode != 0 ? 2 : 1;
    }
    {
        Row& r = row(kWater, "Water", kPage, 4);
        add(r, "Original", 0);
        add(r, "Modern", 1);
        r.selected = s.water != 0 ? 2 : 1;
    }
}

i32 label_y(const Row& r) { return dy() + (r.page == 1 ? video_row(r.slot) : graphics_row(r.slot)); }
const Choice& chosen(const Row& r) { return r.choices[static_cast<std::size_t>(std::clamp<i32>(r.selected, 1, static_cast<i32>(r.choices.size())) - 1)]; }

// The row's value into the settings (and the renderer where it can change live).
void apply(Id id) {
    auto& s = joan::hook::mutable_settings();
    const Choice& c = chosen(state().rows[id]);
    switch (id) {
        case kResolution: s.resolution = c.str; break;
        case kBorderless: s.borderless = c.value != 0; break;
        case kVSync: s.vsync = c.value; break;
        case kMaxFps: s.max_fps = c.value; break;
        case kUiScale:
            s.ui_scale = c.value <= 0 ? 0.0f : static_cast<float>(c.value) / 100.0f;
            joan::display::set_ui_scale(s.ui_scale);  // the HUD is placed with it every frame: at once
            break;
        case kAnisotropy: s.anisotropy = c.value; break;
        case kMsaa: s.msaa = c.value; break;
        case kTextureFilter: s.texture_filter = c.value; break;
        case kAo:
            s.ao_mode = c.value;
            joan::render::ao_params().mode = c.value;
            break;
        case kWater:
            s.water = c.value;
            joan::render::water_params().mode = c.value;
            break;
        default: break;
    }
    if (id == kVSync || id == kMaxFps || id == kAnisotropy || id == kTextureFilter) joan::gld::apply_live_settings();
    state().dirty = true;
}

void set_choice(Id id, i32 selected) {
    Row& r = state().rows[id];
    r.selected = selected;
    if (r.list != nullptr) joan::orig::as_popup(r.list)->set_text_6b44e0(joan::orig::arg::u32(selected));  // 0x006b44e0
}
// The row's choice holding `value` (or `str`), 0 when none.
i32 find_choice(Id id, i32 value, const char* str = nullptr) {
    const Row& r = state().rows[id];
    for (std::size_t i = 0; i < r.choices.size(); ++i)
        if (str != nullptr ? r.choices[i].str == str : r.choices[i].value == value) return static_cast<i32>(i) + 1;
    return 0;
}

template <class F>
void each_on(i32 page, F f) {
    for (i32 i = 0; i < kRows; ++i)
        if (state().rows[i].page == page && state().rows[i].button != nullptr && state().rows[i].list != nullptr) f(state().rows[i]);
}

// Slides like the original pages (Panel::layout_items 0x0074bc50 / slide-out 0x0074c2e0).
void row_in(Row& r, u32 delay) {
    const i32 y = label_y(r);
    scale(r.button, 1, k4, 0, 0x1f4);
    move(r.button, kLabelIn, y, k3500, kM1500, 0, 0x1f4);
    scale(r.list, 1, k4, 0, delay);
    move(r.list, kValueX, y - 7, k3500, kM1500, 0, delay);
}
void row_out(Row& r, u32 delay) {
    scale(r.button, 0, k4, 0, 0);
    move(r.button, -612, label_y(r), k3500, kM1500, 0, 0);
    scale(r.list, 0, k4, 0, delay);
    move(r.list, kValueX, dy() + 0x3e, k3500, kM1500, 0, delay);
}
// The original value widgets slide in bottom row first, 0x32 ms apart, the top row last at 0x384.
u32 in_delay(i32 slot, i32) { return static_cast<u32>(0x384 - slot * 0x32); }

}  // namespace

// ---------------------------------------------------------------- graphics_page.hpp
void create(void* menu) {
    State& st = state();
    if (st.built) return;
    define_rows();
    st.tab = make_button(0x100, 0x3e, 0x300, 0x7e, fn::code_value(fn::draw_cs_73ee40), "GRAPHICS OPTIONS");  // as the four page buttons
    if (st.tab != nullptr) scale(st.tab, 0, kOne, 1, 0);
    for (Row& r : st.rows) {
        const i32 y = (r.page == 1 ? video_row(r.slot) : graphics_row(r.slot));
        r.button = make_button(kLabelX, y, -0x64, y + 0x38, fn::code_value(fn::draw_unit_info), r.label);  // 0x0073e990
        r.list = make_list();
        if (r.list == nullptr) continue;
        for (const Choice& c : r.choices) joan::orig::as_combo_box(r.list)->add_6b3930(joan::orig::arg::str(c.text.c_str()));  // 0x006b3930
        joan::orig::as_popup(r.list)->set_text_6b44e0(joan::orig::arg::u32(r.selected));  // 0x006b44e0
    }
    st.built = true;
    hide_now(menu);
}

void release(void*) {
    State& st = state();
    free_widget(st.tab, fn::MenuButton_clear, fn::ComboBox_scalar_dtor_6b33a0);  // a Button, as 0x00749e00 frees them
    for (Row& r : st.rows) {
        free_widget(r.button, fn::MenuButton_clear, fn::ComboBox_scalar_dtor_6b33a0);
        free_widget(r.list, fn::ComboBox_free_parts, fn::Obj741240_scalar_deleting_dtor);  // a ComboBox
    }
    st.built = false;
}

void hide_now(void*) {
    State& st = state();
    if (!st.built) return;
    if (st.tab != nullptr) {  // as 0x0073ded0 the four page buttons
        move(st.tab, 0x100, dy() + 0x3e, kOne, 0, 1, 0);
        scale(st.tab, 0, kOne, 1, 0);
    }
    for (Row& r : st.rows) {
        if (r.button != nullptr) {
            move(r.button, kLabelX, label_y(r), kOne, 0, 1, 0);
            scale(r.button, 0, kOne, 1, 0);
        }
        if (r.list != nullptr) {
            move(r.list, kValueX, dy() + 0x3e, kOne, 0, 1, 0);
            scale(r.list, 0, kOne, 1, 0);
        }
    }
}

void update(void*) {
    State& st = state();
    if (!st.built) return;
    if (st.tab != nullptr) joan::orig::as_widget(st.tab)->update();  // 0x0061f3b0
    for (Row& r : st.rows) {
        if (r.button != nullptr) joan::orig::as_widget(r.button)->update();
        if (r.list != nullptr) joan::orig::as_widget(r.list)->update();
    }
}

void draw(void*) {
    State& st = state();
    if (!st.built) return;
    if (st.tab != nullptr) joan::orig::as_button(st.tab)->draw(2u, 7);  // 0x00622cc0, as OptionMenu::draw the page buttons
    for (Row& r : st.rows) {
        if (r.list != nullptr) joan::orig::as_combo_box(r.list)->draw(1u, static_cast<std::uint8_t>(7));  // 0x006b3ec0
        if (r.button != nullptr) joan::orig::as_button(r.button)->draw(1u, 7);                            // 0x00622cc0
    }
}

void tab_in(void*, u32 delay) {
    State& st = state();
    if (st.tab == nullptr) return;
    scale(st.tab, 1, k6, 0, delay);
    move(st.tab, 0x100, dy() + kTabY[1], k3500, kM1500, 0, delay);
}
void tab_out(void*, u32 delay) {
    State& st = state();
    if (st.tab == nullptr) return;
    scale(st.tab, 0, k6, 0, delay);
    move(st.tab, 0x100, dy() + 0x3e, k3500, kM1500, 0, delay);
}
bool tab_clicked(void*) {
    State& st = state();
    if (st.tab == nullptr) return false;
    return joan::orig::arg::as<i32>(joan::orig::as_ui_button(st.tab)->mouse(i32{0}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0;  // 0x006218c0
}

void video_in(void*) {
    each_on(1, [](Row& r) { row_in(r, in_delay(r.slot, 8)); });
}
void video_out(void*) {
    each_on(1, [](Row& r) { row_out(r, static_cast<u32>(r.slot * 0x32)); });
}

void graphics_in(void* menu) {
    each_on(kPage, [](Row& r) { row_in(r, in_delay(r.slot, 10)); });
    // The original rows moved here from VIDEO OPTIONS (their widgets are the game's: snd_btn_64 / snd_slider_60 ...).
    auto moved = [&](void* label, void* value, i32 slot) {
        const i32 y = dy() + graphics_row(slot);
        scale(label, 1, k4, 0, 0x1f4);
        move(label, kLabelIn, y, k3500, kM1500, 0, 0x1f4);
        scale(value, 1, k4, 0, in_delay(slot, 10));
        move(value, kValueX, y - 7, k3500, kM1500, 0, in_delay(slot, 10));
    };
    M* const m = jo::self<M>(menu);
    moved(m->snd_btn_64, m->snd_slider_60, 5);  // Draw Distance
    moved(m->snd_btn_7c, m->snd_combo_78, 6);   // Dynamic Grass
    moved(m->snd_btn_6c, m->snd_slider_68, 7);  // Dynamic Grass Draw Distance
    moved(m->snd_btn_74, m->snd_slider_70, 8);  // Dynamic Grass Draw Density
    moved(m->snd_btn_84, m->snd_combo_80, 9);   // Shadow
    void* const def = jo::self<M>(menu)->snd_btn_88;
    void* const back = jo::self<M>(menu)->snd_btn_8c;
    scale(def, 1, k6, 0, 0x1f4);
    move(def, 0x183, dy() + 0x2c8, k65535, 0, 0, 0x1f4);
    scale(back, 1, k6, 0, 0x1f4);
    move(back, 0x219, dy() + 0x2c8, k65535, 0, 0, 0x1f4);
    move(jo::self<M>(menu)->title, 0x1cc, dy() + kGraphicsFrameY, k3500, kM1500, 0, 0x1f4);  // the panel behind the value column
}
void graphics_out(void*) {
    each_on(kPage, [](Row& r) { row_out(r, static_cast<u32>(r.slot * 0x32)); });
}

bool input(void*, i32 page) {
    bool changed = false;
    each_on(page, [&](Row& r) {
        // Row hover as the original rows (Options::update_74a700): the label lights, the list takes the arrows.
        const bool h = hovered(r.button);
        jo::self<jo::Button>(r.button)->held = static_cast<u8>(h ? 1 : 0);
        thiscall<u32>(fn::code_value(fn::Obj6b3_set_flag), r.list, h ? 1 : 0);  // 0x006b3e50
        if (changed) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_menu(r.list)->step_6b4280()) == 0) return;  // 0x006b4280: an arrow clicked
        r.selected = static_cast<i32>(jo::view<u32>(jo::self<jo::ComboBox>(r.list)->selected));
        apply(static_cast<Id>(&r - state().rows));
        changed = true;
    });
    return changed;
}

void reset_defaults(void*, i32 page) {
    // The values of a openjoan.ini without these keys (registry.hpp).
    auto set = [&](Id id, i32 sel) {
        if (state().rows[id].page != page || sel == 0) return;
        set_choice(id, sel);
        apply(id);
    };
    set(kResolution, find_choice(kResolution, 0, "desktop"));
    set(kBorderless, find_choice(kBorderless, 1));
    set(kVSync, find_choice(kVSync, 0));
    set(kMaxFps, find_choice(kMaxFps, -1));
    set(kUiScale, find_choice(kUiScale, 0));
    set(kAnisotropy, find_choice(kAnisotropy, 16));
    set(kMsaa, find_choice(kMsaa, 0));
    set(kTextureFilter, find_choice(kTextureFilter, 1));
    set(kAo, find_choice(kAo, 1));
    set(kWater, find_choice(kWater, 1));
}

void save() {
    State& st = state();
    if (!st.dirty) return;
    st.dirty = false;
    const auto& s = joan::hook::settings();
    const std::string ini = joan::hook::ini_path();
    auto put = [&](const char* section, const char* key, const std::string& value) {
        joan::platform::ini::write_string(section, key, value.c_str(), ini.c_str());
    };
    put("Display", "Resolution", s.resolution);
    {
        char ui[16] = "auto";
        if (s.ui_scale > 0.0f) std::snprintf(ui, sizeof ui, "%g", static_cast<double>(s.ui_scale));
        put("Display", "UiScale", ui);
    }
    put("Graphics", "Borderless", s.borderless ? "1" : "0");
    put("Graphics", "VSync", std::to_string(s.vsync));
    put("Graphics", "MaxFps", std::to_string(s.max_fps));
    put("Graphics", "Anisotropy", std::to_string(s.anisotropy));
    put("Graphics", "MSAA", std::to_string(s.msaa));
    put("Graphics", "TextureFilter", std::to_string(s.texture_filter));
    put("Graphics", "AmbientOcclusion", std::to_string(s.ao_mode));
    put("Graphics", "Water", std::to_string(s.water));
    joan::hook::log_line("[Options] graphics settings written to openjoan.ini");
}

const char* title() { return "GRAPHICS OPTIONS"; }

}  // namespace joan::optmenu::graphics
