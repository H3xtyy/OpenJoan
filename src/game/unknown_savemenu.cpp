// game/unknown_savemenu.cpp: functions whose original source file is unknown (formerly game/_unplaced/savemenu.cpp;
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

// The menu's ten slot buttons by index.
jo::Button& save_slot(const void* menu, i32 i) { return (&jo::self<jo::SaveMenu>(menu)->slots[0])[i]; }
// The 64-byte name buffer at +0x449d (after the 0x91-byte name_edit).
char* name_buf(const void* menu) { return const_cast<char*>(jo::self<jo::SaveMenu>(menu)->name); }
using u8 = std::uint8_t;

using u16 = std::uint16_t;

using joan::img::obj;

using joan::img::str;

using joan::hook::global;
using i8 = std::int8_t;

using i16 = std::int16_t;

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

// The widget's rectangle (+9 / +0xb / +0xd / +0xf) and scale, rescaled by 0x00633e60 when +6.
struct Rect {
    i16 x, y, w, h;  // [ebp-4] / [ebp-8] / [ebp-0xc] / [ebp-0x10]
    float sx, sy;    // [ebp-0x14] / [ebp-0x18]
};

}  // namespace

// ---------------------------------------- from modules/game_b102_16.cpp (game/b102_16)
namespace {
namespace game_b102_16 {

JOAN_COUNTER(c_ctor_74c8d0, "Screen::ctor_74c8d0");

struct H {
    // ------------------------------------------------------------------------ 0x0074c8d0
    void* ctor_74c8d0() {
        c_ctor_74c8d0.hit();
        using VecCtor = void(JOAN_STDCALL*)(void*, u32, i32, std::uintptr_t, std::uintptr_t);  // ctor / dtor: code addresses
        reinterpret_cast<VecCtor>(fn::code_value(fn::crt_eh_vector_constructor_iterator))(&jo::self<jo::SaveMenu>(this)->slots[0], static_cast<u32>(sizeof(jo::Button)), 10, fn::code_value(fn::Widget_ctor_620ee0), fn::code_value(fn::Widget_dtor_6211a0));
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->list)->m_9dd7a0_ctor();  // 0x006b22f0
        joan::orig::as_image2d(&jo::self<jo::SaveMenu>(this)->frame)->construct();  // 0x00629400
        joan::orig::as_image2d(&jo::self<jo::SaveMenu>(this)->pic)->construct();  // 0x00629400
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->info_bar)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_image2d(&jo::self<jo::SaveMenu>(this)->info)->construct();  // 0x00629400
        joan::orig::as_image2d(&jo::self<jo::SaveMenu>(this)->bar_top)->construct();  // 0x00629400
        joan::orig::as_image2d(&jo::self<jo::SaveMenu>(this)->bar_bottom)->construct();  // 0x00629400
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_286e)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_2bb4)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_2efa)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_3240)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_3586)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->mark)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_3c12)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_3f58)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_image2d(&jo::self<jo::SaveMenu>(this)->in_frame)->construct();  // 0x00629400
        joan::orig::as_image2d(&jo::self<jo::SaveMenu>(this)->in_bg)->construct();  // 0x00629400
        joan::orig::as_line_edit(&jo::self<jo::SaveMenu>(this)->name_edit)->construct();  // 0x0045b790
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->edit_button)->ctor_620ee0();  // 0x00620ee0
        joan::orig::as_scroll_bar(&jo::self<jo::SaveMenu>(this)->edit_scroll)->construct();  // 0x00761720
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_16", name, addr, fn)
B("Screen::ctor_74c8d0", 0x0074c8d0, &H::ctor_74c8d0);
#undef B

}  // namespace game_b102_16
}  // namespace

// ---------------------------------------- from modules/game_b109_07.cpp (game/b109_07)
namespace {
namespace game_b109_07 {

void* const kStrings = obj(d::g_strings);

JOAN_COUNTER(c_info_init, "InfoPanel::init_74cb10");

// 0x0042a260 colour (r, g, b, a) into a 4-byte local, dword read back.
u32 colour(u8 r, u8 g, u8 b, u8 a) {
    alignas(4) u8 slot[4];
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(p));
}
// 0x0061f1b0(0, 1.0f, 0, 1, 0)
void gadget_enable(void* g) { joan::orig::as_widget(g)->scale_to(0, 1.0f, 0.0f, 1, 0u); }
// 0x00629660 (image gadget, 19 args): x, y, style, name, colour, 3, 0, 2, 0, -1 x4, 0, -1, -1, 0, 1, 1.
void image_init(void* g, i32 x, i32 y, void* style, const char* name, u32 color) {
    joan::orig::as_label(g)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::mptr(style), joan::orig::arg::str(name), joan::orig::arg::i32(color), 3, 0, 2, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0, 1, 1);  // 0x00629660
}

struct H {
    // ------------------------------------------------------------------ 0x0074cb10
    void list_gadget(void* g, i32 x, i32 y, i32 w, i32 h, std::uintptr_t fn, u32 a6) {  // fn: the draw callback's code address
        joan::orig::as_button(g)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), fn, 1, joan::orig::arg::i32(a6), 0, 1, 1, 1);  // 0x00621540
    }
    void text_button(void* g, i32 x, i32 w, std::uintptr_t text_fn) {
        list_gadget(g, x, 0x2c8, w, 0x2e6, fn::code_value(fn::mark_draw_754220), 0u);  // 0x00754220
        const u32 c = colour(0xff, 0xff, 0xff, 0xff);
        void* const t = thiscall<void*>(text_fn, kStrings);
        joan::orig::as_button(g)->set_text(joan::orig::arg::str(t), img::obj(d::g_font_goudy_bold_18), joan::orig::arg::i32(c), 1, 2, 0, 2, 2);  // 0x00623ce0
        gadget_enable(g);
    }
    void info_init() {
        c_info_init.hit();
        jo::self<jo::SaveMenu>(this)->font = obj(d::g_bmp_res_savemenu);
        jo::self<jo::SaveMenu>(this)->sprites = nullptr;
        jo::self<jo::SaveMenu>(this)->font2 = obj(d::g_bmp_commele2);
        jo::self<jo::SaveMenu>(this)->sprites2 = nullptr;
        for (i32 i = 0; i < 10; ++i) {
            joan::orig::as_button(&save_slot(this, i))->create(0x74, 0x46, 0x244, 0x6a, fn::code_value(fn::SlotButton_draw_7538b0), 1, 0, 0, 1, 1, 1);  // 0x00621540 (0x007538b0)
            gadget_enable(&save_slot(this, i));
        }
        joan::orig::as_combo_box(&jo::self<jo::SaveMenu>(this)->list)->init_options(0x74u, 0x3eu, 2, 1u, 1u);  // 0x006b29b0
        gadget_enable(&jo::self<jo::SaveMenu>(this)->list);
        {
            const u32 c = colour(0, 0, 0, 0xff);
            image_init(&jo::self<jo::SaveMenu>(this)->frame, 0x80, -0x288, jo::self<jo::SaveMenu>(this)->font2, str(d::s_FRAME), c);  // FRAME
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            image_init(&jo::self<jo::SaveMenu>(this)->pic, 0x244, -0xa8, jo::self<jo::SaveMenu>(this)->font, str(d::s_PIC), c);  // PIC
        }
        gadget_enable(&jo::self<jo::SaveMenu>(this)->pic);
        joan::orig::as_button(&jo::self<jo::SaveMenu>(this)->info_bar)->create(0x264, 0x1e0, 0x390, 0x1fc, fn::code_value(fn::draw_infobar), 1, 0, 0, 1, 1, 1);  // 0x00621540 (0x00753440)
        gadget_enable(&jo::self<jo::SaveMenu>(this)->info_bar);
        {
            const u32 c = colour(0, 0, 0, 0xff);
            image_init(&jo::self<jo::SaveMenu>(this)->info, 0x264, 0x168, jo::self<jo::SaveMenu>(this)->font, str(d::s_INFO), c);  // INFO
        }
        gadget_enable(&jo::self<jo::SaveMenu>(this)->info);
        {
            const u32 c = colour(0, 0, 0, 0xff);
            image_init(&jo::self<jo::SaveMenu>(this)->bar_top, -0x400, 0xb4, jo::self<jo::SaveMenu>(this)->font2, str(d::s_LONGBAR), c);  // LONGBAR
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            image_init(&jo::self<jo::SaveMenu>(this)->bar_bottom, 0x400, 0x2c8, jo::self<jo::SaveMenu>(this)->font2, str(d::s_LONGBAR), c);  // LONGBAR
        }
        {   // 0x38cc: a6 = 1 (the others 0)
            list_gadget(&jo::self<jo::SaveMenu>(this)->mark, 0x1b5, 0x2c8, 0x24b, 0x2e6, fn::code_value(fn::mark_draw_754220), 1u);  // 0x00754220
            const u32 c = colour(0xff, 0xff, 0xff, 0xff);
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_save_menu(kStrings)->open_13());  // 0x004dc970
            joan::orig::as_button(&jo::self<jo::SaveMenu>(this)->mark)->set_text(joan::orig::arg::str(t), img::obj(d::g_font_goudy_bold_18), joan::orig::arg::i32(c), 1, 2, 0, 2, 2);  // 0x00623ce0
            gadget_enable(&jo::self<jo::SaveMenu>(this)->mark);
        }
        text_button(&jo::self<jo::SaveMenu>(this)->btn_286e, 0x106, 0x19c, fn::code_value(fn::SaveMenu_open_09));  // 0x004dc790
        text_button(&jo::self<jo::SaveMenu>(this)->btn_2efa, 0xed, 0x183, fn::code_value(fn::SaveMenu_open_02));   // 0x004dc640
        text_button(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0xed, 0x183, fn::code_value(fn::TxtRes_savemenu_01));   // 0x004dc610
        text_button(&jo::self<jo::SaveMenu>(this)->btn_3240, 0x1b5, 0x24b, fn::code_value(fn::SaveMenu_open_03));  // 0x004dc670
        text_button(&jo::self<jo::SaveMenu>(this)->btn_3586, 0x27d, 0x313, fn::code_value(fn::TxtRes_savemenu_05));  // 0x004dc6d0
        text_button(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0x106, 0x19c, fn::code_value(fn::Menu_savemenu_04));  // 0x004dc6a0
        text_button(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0x264, 0x2fa, fn::code_value(fn::TxtRes_savemenu_05));  // 0x004dc6d0
        {
            const u32 c = colour(0, 0, 0, 0xff);
            image_init(&jo::self<jo::SaveMenu>(this)->in_bg, 0x11e, 0x2d2, jo::self<jo::SaveMenu>(this)->font, str(d::s_IN_BG), c);  // IN_BG
        }
        gadget_enable(&jo::self<jo::SaveMenu>(this)->in_bg);
        {
            const u32 c = colour(0, 0, 0, 0xff);
            image_init(&jo::self<jo::SaveMenu>(this)->in_frame, 0xe4, 0x2c8, jo::self<jo::SaveMenu>(this)->font, str(d::s_IN_FRAME), c);  // IN_FRAME
        }
        gadget_enable(&jo::self<jo::SaveMenu>(this)->in_frame);
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(name_buf(this))), str(d::kEmpty));  // strcpy
        {
            alignas(4) u8 slot[4];
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_color(slot)->Color_dword(0xffffffffu));  // 0x005e9640
            const u32 c = jo::view<u32>(*jo::as_colour(p));
            joan::orig::as_edit_box(&jo::self<jo::SaveMenu>(this)->name_edit)->init(0x11e, 0x2d3, 0x2e2, name_buf(this), 0x40, joan::orig::arg::mptr(img::addr(d::g_font_goudy14_22_white)), joan::orig::arg::u32(c), 0, 2, 0, 1, 0);  // 0x0062d4f0
        }
        gadget_enable(&jo::self<jo::SaveMenu>(this)->name_edit);
        joan::orig::as_widget_child(&jo::self<jo::SaveMenu>(this)->name_edit.child)->disable();  // 0x006ca020
        jo::self<jo::SaveMenu>(this)->selected_slot = 0;
        jo::self<jo::SaveMenu>(this)->page = 0;
        jo::view<u8>(jo::self<jo::SaveMenu>(this)->state) = 0;
        jo::view<u8>(jo::self<jo::SaveMenu>(this)->sliding) = 0;
        jo::self<jo::SaveMenu>(this)->u8_4aff = 0;
        jo::self<jo::SaveMenu>(this)->u8_4b00 = 0;
        jo::self<jo::SaveMenu>(this)->save_mode = 0;
        jo::self<jo::SaveMenu>(this)->slide_pos = 0;
        jo::self<jo::SaveMenu>(this)->is_open_ = 0;
        jo::self<jo::SaveMenu>(this)->name_edit_state = 0;
        jo::self<jo::SaveMenu>(this)->load_done = 0;
        joan::orig::as_panel(this)->slide_in_74dbc0();  // 0x0074dbc0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_07", name, addr, fn)
B("InfoPanel::init", 0x0074cb10, &H::info_init);
#undef B

}  // namespace game_b109_07
}  // namespace

// ---------------------------------------- from modules/game_b104_16.cpp (game/b104_16)
namespace {
namespace game_b104_16 {

JOAN_COUNTER(c_init_children, "Panel::init_children");

struct H {
    // ------------------------------------------------------------------ 0x0074d790
    i32 init_children() {
        c_init_children.hit();
        auto* self = reinterpret_cast<std::uint8_t*>(this);
        joan::orig::as_panel(this)->slide_in_74dbc0();  // 0x0074dbc0
        joan::orig::as_save_menu(this)->free_images();  // 0x0074d9a0
        for (i32 i = 0; i < 10; ++i) joan::orig::as_menu_button(&save_slot(self, static_cast<i32>(i)))->clear();  // 0x006217e0
        joan::orig::as_combo_box(&jo::as_save_menu(self)->list)->free_parts();  // 0x006b3130
        joan::orig::as_image2d(&jo::as_save_menu(self)->frame)->release();  // 0x00629830
        joan::orig::as_image2d(&jo::as_save_menu(self)->pic)->release();  // 0x00629830
        joan::orig::as_menu_button(&jo::as_save_menu(self)->info_bar)->clear();  // 0x006217e0
        joan::orig::as_image2d(&jo::as_save_menu(self)->info)->release();  // 0x00629830
        joan::orig::as_image2d(&jo::as_save_menu(self)->bar_top)->release();  // 0x00629830
        joan::orig::as_image2d(&jo::as_save_menu(self)->bar_bottom)->release();  // 0x00629830
        joan::orig::as_menu_button(&jo::as_save_menu(self)->btn_286e)->clear();  // 0x006217e0
        joan::orig::as_menu_button(&jo::as_save_menu(self)->btn_2bb4)->clear();  // 0x006217e0
        joan::orig::as_menu_button(&jo::as_save_menu(self)->btn_2efa)->clear();  // 0x006217e0
        joan::orig::as_menu_button(&jo::as_save_menu(self)->btn_3240)->clear();  // 0x006217e0
        joan::orig::as_menu_button(&jo::as_save_menu(self)->btn_3586)->clear();  // 0x006217e0
        joan::orig::as_menu_button(&jo::as_save_menu(self)->mark)->clear();  // 0x006217e0
        joan::orig::as_menu_button(&jo::as_save_menu(self)->btn_3c12)->clear();  // 0x006217e0
        return joan::orig::arg::as<i32>(joan::orig::as_menu_button(&jo::as_save_menu(self)->btn_3f58)->clear());  // 0x006217e0
    }
};

#define B104(name, addr, fn) JOAN_REPLACE("game/b104_16", name, addr, fn)
B104("Panel::init_children", 0x0074d790, &H::init_children);
#undef B104

}  // namespace game_b104_16
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

void* const kSprites = obj(d::g_texture_array);

JOAN_COUNTER(c_load_save, "Menu::load_save");

i32 sprite_get_flag() { return joan::orig::arg::as<i32>(joan::orig::as_texture_manager(kSprites)->mode()); }                  // 0x0060a720
i32 sprite_set_flag(i32 v) { joan::orig::as_texture_manager(kSprites)->set_mode_60a740(joan::orig::arg::u32(v)); return 0; }  // 0x0060a740 (void; result unused)
void* sprite_load(img::Addr name) { return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kSprites)->load(joan::orig::arg::str(str(name)), 1, 0u)); }  // 0x006099c0

struct H {
    // ------------------------------------------------------------------ 0x0074d900
    i32 load_save() {
        c_load_save.hit();
        const i32 saved = sprite_get_flag();
        sprite_set_flag(1);
        if (jo::self<jo::SaveMenu>(this)->sprites == 0) jo::self<jo::SaveMenu>(this)->sprites = sprite_load(d::s_SAVEMENU);      // 'SAVEMENU'
        if (jo::self<jo::SaveMenu>(this)->sprites2 == 0) jo::self<jo::SaveMenu>(this)->sprites2 = sprite_load(d::s_COMMELE2);  // 'COMMELE2'
        return sprite_set_flag(saved);
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Menu::load_save", 0x0074d900, &H::load_save);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

using joan::orig::helpers::at;

void* const kImages = obj(d::g_texture_array);

void image_release(void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                    // 0x0060a550

struct H {
    static void release_handle(void*& h) {
        if (h != nullptr) {
            image_release(h);
            h = nullptr;
        }
    }

    // 0x0074d9a0
    void release_0_10_14() {
        release_handle(jo::self<jo::SaveMenu>(this)->sprites);
        release_handle(jo::self<jo::SaveMenu>(this)->sprites2);
        release_handle(jo::self<jo::SaveMenu>(this)->preview_texture);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Images::release_0_10_14", 0x0074d9a0, &H::release_0_10_14);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

JOAN_COUNTER(c_briefing, "Briefing::start");

struct H {
    // --------------------------------------------------------------------- 0x0074da40
    u32 briefing_start(u8 a, i8 b) {
        c_briefing.hit();
        jo::self<jo::SaveMenu>(this)->is_open_ = 1;
        jo::self<jo::SaveMenu>(this)->save_mode = a;
        jo::self<jo::SaveMenu>(this)->from_victory = 0;
        jo::self<jo::SaveMenu>(this)->from_defeat = 0;
        if (static_cast<i32>(b) == 1) {
            jo::self<jo::SaveMenu>(this)->from_victory = 1;
        } else if (static_cast<i32>(b) == 2) {
            jo::self<jo::SaveMenu>(this)->from_defeat = 1;
        }
        if (static_cast<i32>(global<i8>(d::g_game_mode)) == 9) joan::orig::as_ingame_frame(obj(d::g_hud))->slide_in();  // 0x006f24d0
        joan::orig::as_menu(this)->load_save();  // 0x0074d900
        if (global<u8>(d::g_game_running) != 0) {
            global<u8>(d::g_game_running) = 0;
            const i32 v = joan::hook::x87::ftol(static_cast<double>(global<float>(d::g_game_speed)));  // _ftol
            global<u16>(d::g_saved_game_speed) = static_cast<u16>(static_cast<u32>(v) & 0xffffu);
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::u32(i32{0}));  // 0x0042f930
        }
        jo::self<jo::SaveMenu>(this)->selected_slot = 0;
        jo::self<jo::SaveMenu>(this)->page = 0;
        jo::view<u8>(jo::self<jo::SaveMenu>(this)->state) = 0;
        jo::self<jo::SaveMenu>(this)->slide_pos = 0;
        jo::self<jo::SaveMenu>(this)->name_edit_state = 0;
        jo::self<jo::SaveMenu>(this)->load_done = 0;
        for (i32 i = 0; i < 10; ++i) jo::view<u8>(save_slot(this, i).pressed) = 0;
        return joan::orig::arg::as<u32>(joan::orig::as_screen(this)->layout_752970());  // 0x00752970
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Briefing::start", 0x0074da40, &H::briefing_start);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

// sound
void slide_sound() {
    joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_BUTTON_SLIDE_MANY));  // 0x004ceef0 'BUTTON_SLIDE_MANY'
}
u32 slide_dy() { return static_cast<i8>(global<u8>(d::g_game_mode)) == 9 ? 0xffffffd0u : 0u; }

JOAN_COUNTER(c_slide_74dbc0, "Panel::slide_in_74dbc0");

struct H {
    void slide_74dbc0() {
        c_slide_74dbc0.hit();
        void* const self = this;
        const u32 dy = slide_dy();
        u32 t = 0;
        if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->is_open_) != 0) slide_sound();
        joan::orig::as_widget(&jo::as_save_menu(self)->in_bg)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->in_frame)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->name_edit)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->btn_286e)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->btn_2bb4)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->btn_2efa)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->btn_3240)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->btn_3586)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->mark)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->btn_3c12)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->btn_3f58)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        if (jo::self<jo::SaveMenu>(this)->sliding == 1) {
            joan::orig::as_widget(&jo::as_save_menu(self)->mark)->move_to(0x1b5, joan::orig::arg::i16(dy + 0x2c8), joan::orig::arg::f32(0x3f800000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061e020
            joan::orig::as_widget(&jo::as_save_menu(self)->mark)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        } else {
            joan::orig::as_widget(&jo::as_save_menu(self)->mark)->move_to(0x264, joan::orig::arg::i16(dy + 0x2c8), joan::orig::arg::f32(0x3f800000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061e020
            joan::orig::as_widget(&jo::as_save_menu(self)->mark)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        }
        t = t + 0x32;
        joan::orig::as_widget(&jo::as_save_menu(self)->list)->scale_to(0x0, joan::orig::arg::f32(0x41400000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
        joan::orig::as_widget(&jo::as_save_menu(self)->list)->move_to(0x74, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, joan::orig::arg::u32(t));  // 0x0061e020
        t = t + 0x32;
        for (u32 i = 0; static_cast<i32>(i) < 10; ++i) {
            joan::orig::as_widget(&save_slot(self, static_cast<i32>(i)))->move_to(0x74, joan::orig::arg::i16(dy + 0x46), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, joan::orig::arg::u32(t));  // 0x0061e020
            joan::orig::as_widget(&save_slot(self, static_cast<i32>(i)))->scale_to(0x0, joan::orig::arg::f32(0x41400000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
            if (i == 6) {
                joan::orig::as_widget(&jo::as_save_menu(self)->info_bar)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
                joan::orig::as_widget(&jo::as_save_menu(self)->info_bar)->move_to(0x264, joan::orig::arg::i16(dy + 0x1e0), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, joan::orig::arg::u32(t));
                joan::orig::as_widget(&jo::as_save_menu(self)->info)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
                joan::orig::as_widget(&jo::as_save_menu(self)->info)->move_to(0x264, joan::orig::arg::i16(dy + 0x168), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, joan::orig::arg::u32(t));
            }
            if (i == 2) {
                joan::orig::as_widget(&jo::as_save_menu(self)->pic)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, joan::orig::arg::u32(t));  // 0x0061f1b0
                joan::orig::as_widget(&jo::as_save_menu(self)->pic)->move_to(0x244, joan::orig::arg::i16(dy - 0xa8), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, joan::orig::arg::u32(t));
            }
            t = t + 0x32;
        }
        joan::orig::as_widget(&jo::as_save_menu(self)->frame)->move_to(0x80, joan::orig::arg::i16(dy - 0x288), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, joan::orig::arg::u32(t));  // 0x0061e020
        joan::orig::as_widget(&jo::as_save_menu(self)->bar_top)->move_to(-1024, joan::orig::arg::i16(dy + 0xb4), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, joan::orig::arg::u32(t));
        joan::orig::as_widget(&jo::as_save_menu(self)->bar_bottom)->move_to(0x400, joan::orig::arg::i16(dy + 0x2c8), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, joan::orig::arg::u32(t));
        jo::self<jo::SaveMenu>(this)->slide_pos = 2;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Panel::slide_in_74dbc0", 0x0074dbc0, &H::slide_74dbc0);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b116_09.cpp (game/b116_09)
namespace {
namespace game_b116_09 {


JOAN_COUNTER(c_save, "SaveMenu::draw_74e170");

// 0x0042a260 (thunk 0x004098b8): Color(r, g, b, a) bytes into *slot; returns slot.
u32 color4(u32* slot, i32 r, i32 g, i32 b, i32 a) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
}
// 0x005e9640 (thunk 0x00407577): Color(argb dword) into *slot; returns slot.
u32 color1(u32* slot, i32 v) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(slot)->Color_dword(joan::orig::arg::u32(v)));  // 0x005e9640
}
// Text::draw_3 0x00626130 (thunk 0x00402e0f, ret 0x34).
void text3(img::Addr text_obj, i32 x1, i32 y1, i32 x2, i32 y2, const char* text, u32 color, i32 a7) {
    joan::orig::as_text(obj(text_obj))->draw_3(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(i32{2}), text, joan::orig::arg::u32(color), i32{0}, joan::orig::arg::i32(a7), i32{2}, i32{0}, 1.0f, 1.0f);  // 0x00626130
}

// ------------------------------------------------------------------ 0x0074e170
// The original frame [ebp-0x240, ebp) as typed slots (x86: at their [ebp-..] offsets, so an over-long name or time
// text lands where it did in the original) plus a spill area for overruns past ebp (dropped).
struct SaveFrame {
    u8 pad_240[0x48];
    u32 chapter;       // [ebp-0x1f8] chapter index - 1 (switch)
    char* image_name;  // [ebp-0x1f4] preview name without its folder
    u8 pad_1f0[0x10];
    u32 col_1e0;       // [ebp-0x1e0] Color slot
    u8 pad_1dc[0x4c];
    u32 col_190;       // [ebp-0x190]
    u8 pad_18c[4];
    u32 col_188;       // [ebp-0x188]
    u8 pad_184[4];
    u32 col_180;       // [ebp-0x180]
    u8 pad_17c[4];
    u32 col_178;       // [ebp-0x178]
    u8 pad_174[4];
    u32 col_170;       // [ebp-0x170]
    u8 pad_16c[0x14];
    char time_text[0x18];  // [ebp-0x158]
    float minutes;         // [ebp-0x140]
    u32 julian;            // [ebp-0x13c]
    u32 local_time[2];     // [ebp-0x138] FILETIME
    u32 file_time[2];      // [ebp-0x130] FILETIME
    u32 saved_403d4;       // [ebp-0x128]
    char* slash;           // [ebp-0x124] strrchr(name, '\\')
    char* dot;             // [ebp-0x120] strrchr(name, '.')
    char name[0x108];      // [ebp-0x11c] preview name
    u8* entry;             // [ebp-0x14] the save entry
    u8 pad_10[8];
    i32 dy;                // [ebp-0x8] y offset (-0x30 in mode 9)
    u8* self;              // [ebp-0x4] the menu
    u8 spill[0x400];
};
#if UINTPTR_MAX == 0xffffffffu  // the original (x86) frame
static_assert(offsetof(SaveFrame, chapter) == 0x240 - 0x1f8 && offsetof(SaveFrame, col_1e0) == 0x240 - 0x1e0 &&
                  offsetof(SaveFrame, col_190) == 0x240 - 0x190 && offsetof(SaveFrame, col_170) == 0x240 - 0x170 &&
                  offsetof(SaveFrame, time_text) == 0x240 - 0x158 && offsetof(SaveFrame, saved_403d4) == 0x240 - 0x128 &&
                  offsetof(SaveFrame, name) == 0x240 - 0x11c && offsetof(SaveFrame, entry) == 0x240 - 0x14 &&
                  offsetof(SaveFrame, self) == 0x240 - 0x4 && sizeof(SaveFrame) == 0x640,
              "frame");
#endif

struct H {
    // Title text (both paths): 0x004dc9a0 when +0x4afb == 1, else 0x004dc700.
    static void save_title(SaveFrame& fr) {
        u8* const self = fr.self;
        u32 slot = 0;
        const u32 color = color4(&slot, i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff});
        const char* text;
        if (static_cast<i32>(jo::as_save_menu(self)->sliding) == 1)
            text = joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(d::g_strings))->savemenu_14());  // 0x004dc9a0
        else
            text = joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(d::g_strings))->savemenu_06());  // 0x004dc700
        const i32 x = jo::as_save_menu(self)->bar_top.x;
        const i32 y = jo::as_save_menu(self)->bar_top.y;
        text3(d::g_font_goudy_bold_18, x, y, x + 0x400, y + 0x1e, text, color, 1);  // 0x00626130
    }
    template <class M>
    static void ctl629(SaveFrame& fr, M jo::SaveMenu::* m) {
        joan::orig::as_label(&(jo::as_save_menu(fr.self)->*m))->draw(joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{7}));  // 0x00629bd0
    }
    template <class M>
    static void ctl622(SaveFrame& fr, M jo::SaveMenu::* m) {
        joan::orig::as_button(&(jo::as_save_menu(fr.self)->*m))->draw(2u, 7);  // 0x00622cc0
    }
    static void ctl622_slot(SaveFrame& fr, i32 i) {
        joan::orig::as_button(&save_slot(fr.self, i))->draw(2u, 7);  // 0x00622cc0
    }
    template <class M>
    static void ctl6b3(SaveFrame& fr, M jo::SaveMenu::* m) {
        joan::orig::as_combo_box(&(jo::as_save_menu(fr.self)->*m))->draw(joan::orig::arg::u32(i32{2}), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{7})));  // 0x006b3ec0
    }
    static void save_controls(SaveFrame& fr) {
        ctl629(fr, &jo::SaveMenu::frame);
        for (i32 i = 0; i < 10; ++i) ctl622_slot(fr, i);
        ctl6b3(fr, &jo::SaveMenu::list);
        ctl629(fr, &jo::SaveMenu::pic);
        ctl622(fr, &jo::SaveMenu::info_bar);
        ctl629(fr, &jo::SaveMenu::info);
    }
    // Text::draw 0x00626e00 (thunk 0x0040a3ee, ret 0x3c) of the chapter name.
    static void chapter_text(SaveFrame& fr, std::uintptr_t getter) {
        u32 slot = 0;
        const u32 color = color1(&slot, i32{-1});
        const char* text = thiscall<const char*>(getter, obj(d::g_strings));
        const i32 off = fr.dy;
        joan::orig::as_text_renderer(obj(d::g_font_goudy14_22_white))->draw_wrapped(joan::orig::arg::u32(i32{0x264}), joan::orig::arg::u32(off + 0x291), joan::orig::arg::u32(i32{0x390}), joan::orig::arg::u32(off + 0x2c9), joan::orig::arg::u32(i32{2}), joan::orig::arg::str(text), joan::orig::arg::u32(color), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), 1.0f, 1.0f, joan::orig::arg::u32(i32{2}));  // 0x00626e00
    }
    static void save_entry(SaveFrame& fr) {
        // [ebp-0x14] = entry; [ebp-0x11c] name buffer.
        joan::orig::as_game_file(fr.entry)->preview_name(joan::orig::arg::str(fr.name));  // 0x00473550
        fr.dot = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(fr.name), i32{0x2e}));
        if (fr.dot != nullptr) *fr.dot = 0;
        fr.slash = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(fr.name), i32{0x5c}));
        if (jo::as_save_menu(fr.self)->preview_texture != 0) {
            fr.saved_403d4 = global<u32>(d::g_a403d4);
            global<u32>(d::g_a403d4) = 0;
            joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::as_save_menu(fr.self)->preview_texture));  // 0x0060a550
            global<u32>(d::g_a403d4) = fr.saved_403d4;
            jo::as_save_menu(fr.self)->preview_texture = 0;
        }
        if (fr.slash != nullptr)
            fr.image_name = fr.slash + 1;
        else
            fr.image_name = fr.name;
        void* const image = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(fr.image_name), i32{1}, joan::orig::arg::u32(i32{0})));  // 0x006099c0
        jo::as_save_menu(fr.self)->preview_texture = image;
        if (jo::as_save_menu(fr.self)->preview_texture != 0) {
            const u32 color = color1(&fr.col_170, i32{-1});
            joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->blit_image(joan::orig::arg::i16(i32{0x259}), joan::orig::arg::i16(fr.dy + 0xfd), joan::orig::arg::u32(i32{2}), joan::orig::arg::mptr(jo::as_save_menu(fr.self)->preview_texture), joan::orig::arg::u32(color), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0x13e}), joan::orig::arg::i16(i32{0xee}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{-1}), joan::orig::arg::i16(i32{-1}));  // 0x006332b0
        }
        {
            u8* const e = fr.entry;
            fr.file_time[0] = jo::as_game_file(e)->file_time[0];
            fr.file_time[1] = jo::as_game_file(e)->file_time[1];
        }
        {
            u8* const e = fr.entry;
            fr.local_time[0] = jo::as_game_file(e)->local_time[0];
            fr.local_time[1] = jo::as_game_file(e)->local_time[1];
        }
        fr.julian = joan::orig::arg::as<u32>(joan::orig::as_date(obj(d::g_date))->file_time_to_julian(joan::orig::arg::mptr(static_cast<void*>(fr.file_time))));  // 0x00580350
        fr.minutes = joan::orig::arg::as<float>(joan::orig::as_date(obj(d::g_date))->file_time_to_minutes(joan::orig::arg::mptr(static_cast<void*>(fr.local_time))));  // 0x005803c0
        {
            const u32 color = color1(&fr.col_178, i32{-1});
            const char* text = joan::orig::arg::as<const char*>(joan::orig::as_date(obj(d::g_date))->time_str(joan::orig::arg::str(fr.time_text), joan::orig::arg::f32(fr.minutes), static_cast<char>(joan::orig::arg::i32(i32{0})), static_cast<char>(joan::orig::arg::i32(i32{1}))));  // 0x00580510
            const i32 off = fr.dy;
            text3(d::g_font_goudy14_22_white, 0x264, off + 0x266, 0x390, off + 0x282, text, color, 0);  // 0x00626130
        }
        {
            const u32 color = color1(&fr.col_180, i32{-1});
            const char* text = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->savemenu_18());  // 0x004dca70
            const i32 off = fr.dy;
            text3(d::g_font_goudy12_bold_black, 0x264, off + 0x258, 0x390, off + 0x274, text, color, 1);  // 0x00626130
        }
        {
            const u32 color = color1(&fr.col_188, i32{-1});
            const char* text = joan::orig::arg::as<const char*>(joan::orig::as_date(obj(d::g_date))->date_str(joan::orig::arg::i32(fr.julian), i32{0}, i32{0}));  // 0x0057fc30
            const i32 off = fr.dy;
            text3(d::g_font_goudy14_22_white, 0x264, off + 0x23e, 0x390, off + 0x25a, text, color, 0);  // 0x00626130
        }
        {
            const u32 color = color1(&fr.col_190, i32{-1});
            const char* text = joan::orig::arg::as<const char*>(joan::orig::as_save_menu(obj(d::g_strings))->open_19());  // 0x004dcaa0
            const i32 off = fr.dy;
            text3(d::g_font_goudy12_bold_black, 0x264, off + 0x230, 0x390, off + 0x24c, text, color, 1);  // 0x00626130
        }
        fr.chapter = jo::as_game_file(fr.entry)->u32_19a - 1;
        switch (fr.chapter) {  // ja 7 -> default (unsigned)
            case 0: chapter_text(fr, fn::code_value(fn::SaveMenu_open_11)); break;  // 0x004dc7f0
            case 1: chapter_text(fr, fn::code_value(fn::SaveMenu_open_0c)); break;  // 0x004dc820
            case 2: chapter_text(fr, fn::code_value(fn::SaveMenu_open_0d)); break;  // 0x004dc850
            case 3: chapter_text(fr, fn::code_value(fn::TxtRes_savemenu_0e)); break;  // 0x004dc880
            case 4: chapter_text(fr, fn::code_value(fn::Menu_savemenu_0f)); break;  // 0x004dc8b0
            case 5: chapter_text(fr, fn::code_value(fn::SaveMenu_open_10)); break;  // 0x004dc8e0
            case 6: chapter_text(fr, fn::code_value(fn::TxtRes_savemenu_11)); break;  // 0x004dc910
            case 7: chapter_text(fr, fn::code_value(fn::SaveMenu_open_12)); break;  // 0x004dc940
            default: chapter_text(fr, fn::code_value(fn::Menu_savemenu_0a)); break; // 0x004dc7c0
        }
        {
            const u32 color = color1(&fr.col_1e0, i32{-1});
            const char* text = joan::orig::arg::as<const char*>(joan::orig::as_save_menu(obj(d::g_strings))->open_21());  // 0x004dc9d0
            const i32 off = fr.dy;
            text3(d::g_font_goudy12_bold_black, 0x264, off + 0x280, 0x390, off + 0x29c, text, color, 1);  // 0x00626130
        }
    }

    void save_draw(u32 /*unused*/) {
        joan::canvas::ScreenScope canvas_scope(0x0074e170);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->is_open_) == 0) return;
        c_save.hit();
        SaveFrame fr{};
        fr.self = static_cast<u8*>(static_cast<void*>(this));
        joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
        fr.dy = 0;
        if (static_cast<i32>(global<i8>(d::g_game_mode)) == 9) fr.dy = -0x30;
        if (static_cast<i32>(global<i8>(d::g_game_mode)) == 9) joan::orig::as_menu(obj(d::g_hud))->reset_fades(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x006f21c0
        if (jo::view<i16>(jo::as_save_menu(fr.self)->slide_pos) == 0) {
            save_controls(fr);
            ctl629(fr, &jo::SaveMenu::bar_top);
            save_title(fr);
            ctl629(fr, &jo::SaveMenu::bar_bottom);
            return;
        }
        save_controls(fr);
        {
            u8* s = fr.self;
            if (static_cast<i32>(jo::view<i8>(jo::as_save_menu(s)->page)) > 0 && static_cast<i32>(jo::view<i8>(jo::as_save_menu(s)->selected_slot)) > 0) {
                const i32 last = static_cast<i32>(jo::view<i8>(jo::as_save_menu(s)->selected_slot)) +
                                 (static_cast<i32>(jo::view<i8>(jo::as_save_menu(s)->page)) - 1) * 10 - 1;
                if (last < joan::orig::as_dynarray(obj(d::g_saves))->size()) {  // 0x00417ad0
                    s = fr.self;
                    const i32 index = static_cast<i32>(jo::view<i8>(jo::as_save_menu(s)->selected_slot)) + (static_cast<i32>(jo::view<i8>(jo::as_save_menu(s)->page)) - 1) * 10;
                    fr.entry = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_saves))->get_430410(joan::orig::arg::i16(index)));  // 0x00430410
                    if (static_cast<i32>(jo::view<i16>(jo::as_save_menu(fr.self)->slide_pos)) == 1) save_entry(fr);
                }
            }
        }
        ctl629(fr, &jo::SaveMenu::bar_top);
        save_title(fr);
        ctl629(fr, &jo::SaveMenu::bar_bottom);
        ctl622(fr, &jo::SaveMenu::btn_286e);
        ctl622(fr, &jo::SaveMenu::btn_2bb4);
        ctl622(fr, &jo::SaveMenu::btn_2efa);
        ctl622(fr, &jo::SaveMenu::btn_3240);
        ctl622(fr, &jo::SaveMenu::btn_3586);
        ctl622(fr, &jo::SaveMenu::mark);
        ctl622(fr, &jo::SaveMenu::btn_3c12);
        ctl622(fr, &jo::SaveMenu::btn_3f58);
        if (static_cast<i32>(jo::view<i8>(jo::as_save_menu(fr.self)->name_edit_state)) == 2) {
            ctl629(fr, &jo::SaveMenu::in_frame);
            ctl629(fr, &jo::SaveMenu::in_bg);
            joan::orig::as_text_box(&jo::as_save_menu(fr.self)->name_edit)->draw(joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{7}));  // 0x0062d730
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b116_09", name, addr, fn)
B("SaveMenu::draw_74e170", 0x0074e170, &H::save_draw);
JOAN_CANVAS_SELF(0x0074e170);  // ScreenScope in the body (no thunk wrapper)
#undef B

}  // namespace game_b116_09
}  // namespace

// ---------------------------------------- from modules/game_b110_09.cpp (game/b110_09)
namespace {
namespace game_b110_09 {

char* crt_strcpy(char* dst, const char* src) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dst)), joan::orig::arg::str(src)); }

JOAN_COUNTER(c_save_menu, "SaveMenu::update");

constexpr img::Addr kSaves = d::g_saves;

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ======================================================================= 0x0074f230
    void btn(void* g, i32 visible) {
        joan::orig::as_widget(g)->scale_to(joan::orig::arg::i16(visible), 6.0f, 0.0f, 0, 0u);  // 0x0061f1b0
    }
    void place(void* g, i32 x, i32 y) {
        joan::orig::as_widget(g)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 500.0f, 0.0f, 0, 0u);  // 0x0061e020
    }
    i32 clicked(void* g, i32 key) {
        return joan::orig::arg::as<i32>(joan::orig::as_ui_button(g)->mouse(joan::orig::arg::i32(key), 0, joan::orig::arg::i32(-1), 0, 0, 0));  // 0x006218c0
    }
    u8& slot_flag(i32 slot) { return save_slot(this, slot).held; }
    i32 sel() { return jo::view<i8>(jo::self<jo::SaveMenu>(this)->selected_slot); }
    i32 page() { return jo::view<i8>(jo::self<jo::SaveMenu>(this)->page); }
    i32 sel_index() { return sel() + (page() - 1) * 10; }

    // Buttons 0x4355 / 0x429e / 0x440c (name edit) hidden or shown at y = base + dy.
    void edit_buttons(i32 visible, i32 yoff, i32 d1, i32 d2, i32 d3) {
        btn(&jo::self<jo::SaveMenu>(this)->in_bg, visible);
        place(&jo::self<jo::SaveMenu>(this)->in_bg, 0x11e, yoff + d1);
        btn(&jo::self<jo::SaveMenu>(this)->in_frame, visible);
        place(&jo::self<jo::SaveMenu>(this)->in_frame, 0xe4, yoff + d2);
        btn(&jo::self<jo::SaveMenu>(this)->name_edit, visible);
        place(&jo::self<jo::SaveMenu>(this)->name_edit, 0x11e, yoff + d3);
    }
    void edit_buttons_hide(i32 yoff) { edit_buttons(0, yoff, 0x2d2, 0x2c8, 0x2d3); }
    void edit_buttons_show(i32 yoff) { edit_buttons(1, yoff, 0x29e, 0x294, 0x29f); }

    // Load the save game at index idx (> 0).
    void load_save(i32 idx_word) {
        void* s = joan::orig::arg::as<void*>(joan::orig::as_units(obj(kSaves))->get_430410(joan::orig::arg::i16(static_cast<i16>(idx_word))));  // 0x00430410
        joan::orig::as_progress(obj(d::g_progress))->set_image(joan::orig::as_game_file(s));                                     // 0x007563f0
    }

    // Fill save record s (name already set) and write it.
    void write_save(u8* s) {
        crt_strcpy(jo::as_game_file(s)->name, name_buf(this));
        jo::as_game_file(s)->u32_19a = global<u32>(d::g_mission);
        jo::as_game_file(s)->u8_19e = 0;
        joan::orig::as_game_file(s)->save();                              // 0x00478180
        joan::orig::as_game_file(s)->save_preview(joan::orig::arg::mptr(global<jo::Camera*>(d::g_camera)));     // 0x004731e0
        joan::orig::as_save_dir(obj(kSaves))->clear();                    // 0x00477030
        joan::orig::as_save_dir(obj(kSaves))->init();                    // 0x00476fa0
        slot_flag(sel() - 1) = 0;
        jo::view<i8>(jo::self<jo::SaveMenu>(this)->selected_slot) = 0;
        joan::orig::as_save_menu(this)->pages_753060(1u);  // 0x00753060
    }

    // Name edit -> save: shared tail of the two save paths.
    void after_save(i32 yoff) {
        btn(&jo::self<jo::SaveMenu>(this)->mark, 1);
        btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0);
        btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0);
        edit_buttons_hide(yoff);
        jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 0;
        btn(&jo::self<jo::SaveMenu>(this)->btn_286e, 1);
        btn(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0);
        btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
        btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
    }

    void select_slot_buttons(i32 load_mode) {
        if (load_mode) {
            btn(&jo::self<jo::SaveMenu>(this)->btn_2efa, 1);
        } else {
            btn(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 1);
        }
        btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 1);
        btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 1);
    }

    void start_name_edit() {
        crt_strcpy(name_buf(this),
                   reinterpret_cast<const char*>(save_slot(this, sel() - 1).caption));
        joan::orig::as_widget_child(&jo::self<jo::SaveMenu>(this)->name_edit.child)->enable();  // 0x006ca000
        joan::orig::as_line_edit(&jo::self<jo::SaveMenu>(this)->name_edit)->set_focus();  // 0x0045bf80
        jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 2;
    }

    void close_menu() {
        joan::orig::as_panel(this)->slide_in_74dbc0();  // 0x0074dbc0
        if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->from_victory) != 0) global<i32>(d::g_screen_pending) = 1;
    }

    bool update_slots() {
        for (i32 i = 0; i < 10; ++i) {
            if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) > 0) break;
            jo::Button* slot = &save_slot(this, i);
            if (joan::orig::arg::as<i32>(joan::orig::as_button(slot)->hit_622730(0)) != 0) {                 // 0x00622730
                const i32 count = joan::orig::as_dynarray(obj(kSaves))->size();  // 0x00417ad0
                if (page() == 0) return true;
                if (i + (page() - 1) * 10 + 1 <= count && jo::self<jo::SaveMenu>(this)->sliding == 1) {
                    const i32 idx = i + (page() - 1) * 10 + 1;
                    if (idx > 0) {
                        load_save(idx);
                        jo::view<i8>(jo::self<jo::SaveMenu>(this)->load_done) = 1;
                        slot_flag(i) = 0;
                        jo::view<i8>(jo::self<jo::SaveMenu>(this)->save_mode) = 0;
                        jo::view<i8>(jo::self<jo::SaveMenu>(this)->from_victory) = 0;
                        joan::orig::as_panel(this)->slide_in_74dbc0();  // 0x0074dbc0
                    }
                }
                continue;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(slot)->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0)) == 0) continue;  // 0x006218c0
            const i32 count = joan::orig::as_dynarray(obj(kSaves))->size();  // 0x00417ad0
            if (page() == 0) return true;
            if (i + (page() - 1) * 10 + 1 > count) {
                slot_flag(sel() - 1) = 0;
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->selected_slot) = 0;
                if (jo::self<jo::SaveMenu>(this)->sliding == 1) {
                    btn(&jo::self<jo::SaveMenu>(this)->mark, 1);
                    btn(&jo::self<jo::SaveMenu>(this)->btn_2efa, 0);
                } else {
                    btn(&jo::self<jo::SaveMenu>(this)->mark, 1);
                    btn(&jo::self<jo::SaveMenu>(this)->btn_286e, 1);
                    btn(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0);
                }
                btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0);
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 0;
                btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
                return true;
            }
            if (sel() != i + 1) {
                const bool load = jo::self<jo::SaveMenu>(this)->sliding == 1;
                slot_flag(sel() - 1) = 0;
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->selected_slot) = static_cast<i8>(i + 1);
                slot_flag(i) = 1;
                btn(&jo::self<jo::SaveMenu>(this)->mark, 0);
                if (!load) btn(&jo::self<jo::SaveMenu>(this)->btn_286e, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0);
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 0;
                select_slot_buttons(load);
            }
            return true;
        }
        return false;
    }

    // Selected slot, sub-state 1: delete confirmation.
    void confirm_delete() {
        if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0xd) != 0 && page() > 0) {
            u8* s = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(kSaves))->get_430410(joan::orig::arg::i16(sel_index())));  // 0x00430410
            // Original frame: .BMP name at ebp-0x248 directly followed by the file object at
            // ebp-0x140; kept contiguous so an over-long name spills into the file object
            // exactly like the original.
            struct alignas(4) DeleteFrame {
                char bmp[0x108];
                u8 file[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger File
            } fr = {};
            static_assert(offsetof(DeleteFrame, file) == 0x108 && sizeof(DeleteFrame) >= 0x218, "frame layout");  // 0x218 bytes on x86
            u8* const file = fr.file;
            joan::orig::as_file(file)->construct();  // 0x004179c0
            joan::orig::delete_file(joan::orig::arg::str(s + 3));  // 0x00587f60 (DeleteFileA)
            joan::orig::as_game_file(s)->preview_name(joan::orig::arg::str(fr.bmp));  // 0x00473550
            joan::orig::delete_file(joan::orig::arg::str(fr.bmp));     // 0x00587f60
            joan::orig::as_save_dir(obj(kSaves))->remove(joan::orig::arg::i32(sel_index()));  // 0x00477520
            slot_flag(sel() - 1) = 0;
            jo::view<i8>(jo::self<jo::SaveMenu>(this)->selected_slot) = 0;
            joan::orig::as_save_menu(this)->pages_753060(joan::orig::arg::u32(page()));  // 0x00753060
            btn(&jo::self<jo::SaveMenu>(this)->mark, 1);
            if (jo::self<jo::SaveMenu>(this)->sliding == 0) btn(&jo::self<jo::SaveMenu>(this)->btn_286e, 1);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0);
            jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 0;
            btn(&jo::self<jo::SaveMenu>(this)->btn_2efa, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
            joan::orig::as_file(file)->destruct();  // 0x005872b0
        }
        if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0x1b) != 0) {
            btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0);
            jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 0;
            select_slot_buttons(jo::self<jo::SaveMenu>(this)->sliding == 1);
        }
    }

    // Selected slot, sub-state 2: overwrite with the edited name.
    void overwrite_save(i32 yoff) {
        if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0xd) != 0) {
            u8* s = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(kSaves))->get_430410(joan::orig::arg::i16(sel_index())));  // 0x00430410
            write_save(s);
            after_save(yoff);
        }
        if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0x1b) != 0) {
            btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0);
            edit_buttons_hide(yoff);
            jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 0;
            select_slot_buttons(jo::self<jo::SaveMenu>(this)->sliding == 1);
        }
        joan::orig::as_text(&jo::self<jo::SaveMenu>(this)->name_edit)->poll_62dc70(1u);  // 0x0062dc70
    }

    // No slot selected, sub-state 2: new save with the edited name.
    void new_save(i32 yoff) {
        if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0xd) != 0) {
            char name[0x80] = {};
            crt_strcpy(name, &img::global<const char>(d::s_joan));  // "joan"
            crt_strcpy(name, joan::orig::arg::as<char*>(joan::orig::as_save_dir(obj(kSaves))->file_name(joan::orig::arg::str(name), 0u)));  // 0x00477770
            const i16 id = joan::orig::arg::as<i16>(joan::orig::as_save_dir(obj(kSaves))->add(joan::orig::arg::str(0)));         // 0x00477440
            u8* s = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(kSaves))->get_430410(joan::orig::arg::i16(id)));                // 0x00430410
            crt_strcpy(reinterpret_cast<char*>(s + 3), name);
            write_save(s);
            after_save(yoff);
        }
        if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0x1b) != 0) {
            btn(&jo::self<jo::SaveMenu>(this)->mark, 1);
            btn(&jo::self<jo::SaveMenu>(this)->btn_286e, 1);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0);
            edit_buttons_hide(yoff);
            jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 0;
            btn(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
        }
        joan::orig::as_text(&jo::self<jo::SaveMenu>(this)->name_edit)->poll_62dc70(1u);  // 0x0062dc70
    }

    // Load mode, no sub-state.
    void idle_load_buttons() {
        if (sel() > 0) {
            if (clicked(&jo::self<jo::SaveMenu>(this)->btn_2efa, 0) != 0) {
                if (page() > 0) {
                    const i32 idx = sel_index();
                    if (idx > 0) {
                        load_save(idx);
                        jo::view<i8>(jo::self<jo::SaveMenu>(this)->load_done) = 1;
                        slot_flag(sel() - 1) = 0;
                        jo::view<i8>(jo::self<jo::SaveMenu>(this)->save_mode) = 0;
                        jo::view<i8>(jo::self<jo::SaveMenu>(this)->from_victory) = 0;
                        joan::orig::as_panel(this)->slide_in_74dbc0();  // 0x0074dbc0
                    }
                }
                return;
            }
            if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3240, 0) != 0) {
                btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 1);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 1);
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 1;
                btn(&jo::self<jo::SaveMenu>(this)->btn_2efa, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
                return;
            }
            if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3586, 0x1b) != 0) {
                slot_flag(sel() - 1) = 0;
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->selected_slot) = 0;
                btn(&jo::self<jo::SaveMenu>(this)->mark, 1);
                btn(&jo::self<jo::SaveMenu>(this)->btn_2efa, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
            }
            return;
        }
        if (jo::view<i16>(jo::self<jo::SaveMenu>(this)->slide_pos) != 2 && clicked(&jo::self<jo::SaveMenu>(this)->mark, 0x1b) != 0) close_menu();
    }

    bool save_menu_update() {
        c_save_menu.hit();
        if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->is_open_) == 0) return false;
        i32 yoff = 0;
        if (global<i8>(d::g_game_mode) == 9) yoff = -0x30;
        if (global<i8>(d::g_game_mode) == 9 && global<i8>(d::g_menu_frame_state) != 2) return true;
        if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit.moving) != 0 || jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit.scaling) != 0 || jo::view<i8>(jo::self<jo::SaveMenu>(this)->frame.moving) != 0 || jo::view<i8>(jo::self<jo::SaveMenu>(this)->bar_top.moving) != 0 ||
            jo::view<i8>(jo::self<jo::SaveMenu>(this)->mark.scaling) != 0 || jo::view<i8>(jo::self<jo::SaveMenu>(this)->btn_3c12.scaling) != 0)
            return true;

        if (jo::view<i16>(jo::self<jo::SaveMenu>(this)->slide_pos) == 1 && jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) != 2 && jo::view<i8>(jo::self<jo::SaveMenu>(this)->from_defeat) == 0 && jo::view<i8>(jo::self<jo::SaveMenu>(this)->save_mode) == 0 &&
            joan::orig::arg::as<i32>(joan::orig::as_mode(obj(d::g_screen_mgr))->check_6f5a70()) != 0) {  // 0x006f5a70
            joan::orig::as_panel(this)->slide_in_74dbc0();                          // 0x0074dbc0
            if (global<i32>(d::g_screen_pending) == 3 || global<i32>(d::g_screen_pending) == 4)
                joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
            jo::view<i8>(jo::self<jo::SaveMenu>(this)->from_victory) = 0;
            return true;
        }

        if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) == 0 && joan::orig::arg::as<i32>(joan::orig::as_menu(&jo::self<jo::SaveMenu>(this)->list)->step_6b4280()) > 0) {  // 0x006b4280
            if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->page) != 0) jo::view<i8>(jo::self<jo::SaveMenu>(this)->page) = static_cast<i8>(jo::self<jo::SaveMenu>(this)->list.selected);
            if (sel() > 0) {
                slot_flag(sel() - 1) = 0;
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->selected_slot) = 0;
            }
            if (jo::self<jo::SaveMenu>(this)->sliding == 1) {
                btn(&jo::self<jo::SaveMenu>(this)->mark, 1);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0);
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 0;
                btn(&jo::self<jo::SaveMenu>(this)->btn_2efa, 0);
            } else {
                btn(&jo::self<jo::SaveMenu>(this)->mark, 1);
                btn(&jo::self<jo::SaveMenu>(this)->btn_286e, 1);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0);
                edit_buttons_hide(yoff);
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 0;
                btn(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0);
            }
            btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
            joan::orig::as_save_menu(this)->fill_list(joan::orig::arg::i32(page()));  // 0x00753250
            return true;
        }

        if (update_slots()) return true;

        if (sel() > 0) {
            if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) == 1) {
                confirm_delete();
                return true;
            }
            if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) == 2) {
                overwrite_save(yoff);
                return true;
            }
        } else if (jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) == 2) {
            new_save(yoff);
            return true;
        }

        if (jo::self<jo::SaveMenu>(this)->sliding == 1) {
            idle_load_buttons();
            return true;
        }
        if (sel() > 0) {
            if (clicked(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0) != 0) {
                btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 1);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 1);
                edit_buttons_show(yoff);
                start_name_edit();
                btn(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
                return true;
            }
            if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3240, 0) != 0) {
                btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 1);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 1);
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->name_edit_state) = 1;
                btn(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
                return true;
            }
            if (clicked(&jo::self<jo::SaveMenu>(this)->btn_3586, 0x1b) != 0) {
                slot_flag(sel() - 1) = 0;
                jo::view<i8>(jo::self<jo::SaveMenu>(this)->selected_slot) = 0;
                btn(&jo::self<jo::SaveMenu>(this)->mark, 1);
                btn(&jo::self<jo::SaveMenu>(this)->btn_286e, 1);
                btn(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3240, 0);
                btn(&jo::self<jo::SaveMenu>(this)->btn_3586, 0);
            }
            return true;
        }
        if (clicked(&jo::self<jo::SaveMenu>(this)->btn_286e, 0) != 0) {
            btn(&jo::self<jo::SaveMenu>(this)->btn_286e, 0);
            btn(&jo::self<jo::SaveMenu>(this)->mark, 0);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3c12, 1);
            btn(&jo::self<jo::SaveMenu>(this)->btn_3f58, 1);
            edit_buttons_show(yoff);
            start_name_edit();
            return true;
        }
        if (jo::view<i16>(jo::self<jo::SaveMenu>(this)->slide_pos) != 2 && clicked(&jo::self<jo::SaveMenu>(this)->mark, 0x1b) != 0) close_menu();
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b110_09", name, addr, fn)
B("SaveMenu::update", 0x0074f230, &H::save_menu_update);
#undef B

}  // namespace game_b110_09
}  // namespace

// ---------------------------------------- from modules/game_b109_15.cpp (game/b109_15)
namespace {
namespace game_b109_15 {

constexpr u32 kOneF = 0x3f800000;  // 1.0f

JOAN_COUNTER(c_slide, "SaveMenu::slide_751750");

struct H {
    // ------------------------------------------------------------------ 0x00751750
    struct SlideFrame {
        char text[0x20];  // [ebp-0x34]
        i32 i;            // [ebp-0x14]
        i32 count;        // [ebp-0x10]
        i32 j;            // [ebp-0xc]
        i32 dy;           // [ebp-0x8]
        void* self;       // [ebp-0x4]
    };
#if UINTPTR_MAX == 0xffffffffu  // the original (x86) frame
    static_assert(sizeof(SlideFrame) == 0x34, "slide frame");
#endif

    static void slide_to(void* g, i32 x, i32 y, u32 a3, u32 a4, i32 a5, i32 a6) {
        joan::orig::as_widget(g)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(a3), joan::orig::arg::f32(a4), static_cast<char>(a5), joan::orig::arg::u32(a6));  // 0x0061e020
    }
    static void slide_speed(void* g, u32 speed) {
        joan::orig::as_widget(g)->scale_to(1, joan::orig::arg::f32(speed), 0.0f, 0, 0u);  // 0x0061f1b0
    }
    static void step(void* g) { joan::orig::as_widget(g)->update(); }  // 0x0061f3b0
    template <auto... M>
    static void step_all(void* self) { (step(&(jo::as_save_menu(self)->*M)), ...); }
    static void reset_state(void* self) {
        jo::as_save_menu(self)->selected_slot = 0;
        jo::as_save_menu(self)->page = 0;
        jo::view<u8>(jo::as_save_menu(self)->state) = 0;
        jo::as_save_menu(self)->u8_4aff = 0;
        jo::as_save_menu(self)->u8_4b00 = 0;
        jo::as_save_menu(self)->save_mode = 0;
        jo::as_save_menu(self)->slide_pos = 0;
        jo::as_save_menu(self)->name_edit_state = 0;
        joan::orig::as_screen(self)->layout_752970();  // 0x00752970
    }
    static void resume_timer() {
        if (global<u8>(d::g_game_running) == 0) {
            global<u8>(d::g_game_running) = 1;
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(static_cast<float>(global<i16>(d::g_saved_game_speed))));  // 0x0042f930
        }
    }

    void slide() {
        c_slide.hit();
        SlideFrame fr;
        std::memset(&fr, 0, sizeof fr);
        fr.self = this;
        if (jo::view<i8>(jo::as_save_menu(fr.self)->is_open_) == 0) return;
        constexpr u32 kF1 = 0x455ac000, kF2 = 0xc4bb8000;
        fr.dy = 0;
        if (global<i8>(d::g_game_mode) == 9) fr.dy = -0x30;
        const i32 state = jo::view<i16>(jo::as_save_menu(fr.self)->slide_pos);
        if (state == 0) {
            void* self = fr.self;
            if (jo::as_save_menu(self)->frame.y == fr.dy - 0x288 && jo::view<i8>(jo::as_save_menu(self)->frame.moving) == 0) {
                slide_to(&jo::as_save_menu(self)->frame, 0x80, fr.dy + 0x98, kF1, kF2, 0, 0);
                slide_to(&jo::as_save_menu(self)->bar_top, 0, fr.dy + 0xb4, kF1, kF2, 0, 0);
                slide_to(&jo::as_save_menu(self)->bar_bottom, 0, fr.dy + 0x2c8, kF1, kF2, 0, 0);
                joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_BUTTON_SLIDE_MANY));  // 0x004ceef0 "BUTTON_SLIDE_MANY"
            }
            if (!(jo::as_save_menu(self)->frame.y < fr.dy + 0x94) && jo::as_save_menu(self)->slots[9].y == fr.dy + 0x46 &&
                jo::view<i8>(jo::as_save_menu(self)->slots[9].moving) == 0) {
                slide_to(&jo::as_save_menu(self)->slots[9], 0x74, fr.dy + 0x294, kF1, kF2, 0, 0);
                slide_speed(&jo::as_save_menu(self)->slots[9], 0x41400000);
            }
            for (fr.j = 9; fr.j >= 1; --fr.j) {
                const i32 j = fr.j;
                const double av = static_cast<double>(save_slot(self, j).y);
                const Fpu fpu{true};
                const double bv = fpu.add(static_cast<double>(fr.dy + 0x46), fpu.mul(static_cast<double>(j * 0x28 + 0xe6), 0.5));
                if (bv > av) continue;
                const i32 k = j - 1;
                if (save_slot(self, k).y != fr.dy + 0x46) continue;
                if (jo::view<i8>(save_slot(self, k).moving) != 0) continue;
                slide_to(&save_slot(self, k), 0x74, fr.dy + k * 0x28 + 0x12c, kF1, kF2, 0, 0);
                slide_speed(&save_slot(self, k), 0x41400000);
                if (k == 5) {
                    slide_speed(&jo::as_save_menu(self)->pic, 0x40800000);
                    slide_to(&jo::as_save_menu(self)->pic, 0x244, fr.dy + 0xe8, kF1, kF2, 0, 0);
                } else if (k == 1) {
                    slide_speed(&jo::as_save_menu(self)->info_bar, 0x40800000);
                    slide_to(&jo::as_save_menu(self)->info_bar, 0x264, fr.dy + 0x208, kF1, kF2, 0, 0);
                    slide_speed(&jo::as_save_menu(self)->info, 0x40800000);
                    slide_to(&jo::as_save_menu(self)->info, 0x264, fr.dy + 0x230, kF1, kF2, 0, 0);
                }
            }
            {
                const double av = static_cast<double>(jo::as_save_menu(self)->slots[0].y);
                const double bv = Fpu{true}.add(static_cast<double>(fr.dy + 0x46), global<double>(d::dbl_9df7d0));
                if (!(bv > av) && jo::as_save_menu(self)->list.y == fr.dy + 0x3e && jo::view<i8>(jo::as_save_menu(self)->list.moving) == 0) {
                    slide_to(&jo::as_save_menu(self)->list, 0x74, fr.dy + 0xec, kF1, kF2, 0, 0);
                    slide_speed(&jo::as_save_menu(self)->list, 0x41400000);
                }
            }
            const float one = global<float>(d::flt_1);
            auto eq_or_nan = [](float x, float y) { return !(x < y || x > y); };
            if (eq_or_nan(jo::as_save_menu(self)->info_bar.scale, one) && jo::as_save_menu(self)->info_bar.y == fr.dy + 0x208 &&
                jo::as_save_menu(self)->list.y == fr.dy + 0xec && eq_or_nan(jo::as_save_menu(self)->list.scale, one)) {
                jo::as_save_menu(self)->slide_pos = 1;
                fr.count = joan::orig::as_dynarray(obj(d::g_saves))->size();  // 0x00417ad0
                if (fr.count % 10 > 0) {
                    jo::view<u8>(jo::as_save_menu(self)->state) = static_cast<u8>(fr.count / 10 + 1);
                } else {
                    jo::view<u8>(jo::as_save_menu(self)->state) = static_cast<u8>(fr.count / 10);
                }
                if (jo::as_save_menu(self)->state > 0) {
                    for (fr.i = 1; fr.i <= static_cast<i32>(jo::as_save_menu(fr.self)->state); ++fr.i) {
                        const char* const s = joan::orig::as_save_menu(obj(d::g_strings))->open_22(joan::orig::arg::u32(fr.i), joan::orig::arg::u32(// 0x004dca00
                                                    static_cast<i32>(jo::as_save_menu(fr.self)->state)));
                        joan::orig::libc::sprintf(joan::orig::arg::str(static_cast<char*>(fr.text)), joan::orig::arg::str(s));  // sprintf(buf, s)
                        joan::orig::as_combo_box(&jo::as_save_menu(fr.self)->list)->add_6b3930(joan::orig::arg::str(static_cast<void*>(fr.text)));  // 0x006b3930
                    }
                    jo::as_save_menu(fr.self)->page = 1;
                    joan::orig::as_popup(&jo::as_save_menu(fr.self)->list)->set_text_6b44e0(joan::orig::arg::u32(i32{1}));  // 0x006b44e0
                    joan::orig::as_save_menu(fr.self)->fill_list(i32{1});               // 0x00753250
                } else {
                    const char* const s = joan::orig::as_save_menu(obj(d::g_strings))->open_22(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));
                    joan::orig::as_combo_box(&jo::as_save_menu(fr.self)->list)->add_6b3930(joan::orig::arg::str(s));
                    jo::as_save_menu(fr.self)->page = 0;
                    joan::orig::as_save_menu(fr.self)->fill_list(i32{0});
                    joan::orig::as_popup(&jo::as_save_menu(fr.self)->list)->set_text_6b44e0(joan::orig::arg::u32(i32{1}));
                }
                self = fr.self;
                if (jo::as_save_menu(self)->sliding == 1) {
                    slide_to(&jo::as_save_menu(self)->mark, 0x1b5, fr.dy + 0x2c8, kOneF, 0, 1, 0);
                    slide_speed(&jo::as_save_menu(self)->mark, 0x40c00000);
                } else {
                    slide_to(&jo::as_save_menu(self)->mark, 0x264, fr.dy + 0x2c8, kOneF, 0, 1, 0);
                    slide_speed(&jo::as_save_menu(self)->mark, 0x40c00000);
                    slide_to(&jo::as_save_menu(self)->btn_286e, 0x106, fr.dy + 0x2c8, kOneF, 0, 1, 0);
                    slide_speed(&jo::as_save_menu(self)->btn_286e, 0x40c00000);
                }
            }
            self = fr.self;
            for (fr.j = 0; fr.j < 10; ++fr.j) step(&save_slot(self, fr.j));
            step_all<&jo::SaveMenu::list, &jo::SaveMenu::frame, &jo::SaveMenu::pic, &jo::SaveMenu::info_bar, &jo::SaveMenu::info, &jo::SaveMenu::bar_top, &jo::SaveMenu::bar_bottom, &jo::SaveMenu::btn_286e, &jo::SaveMenu::btn_2bb4, &jo::SaveMenu::btn_2efa, &jo::SaveMenu::btn_3240, &jo::SaveMenu::btn_3586, &jo::SaveMenu::mark, &jo::SaveMenu::btn_3c12, &jo::SaveMenu::btn_3f58>(self);
            return;
        }
        void* const self = fr.self;
        if (state == 1) {
            step_all<&jo::SaveMenu::btn_286e, &jo::SaveMenu::btn_2bb4, &jo::SaveMenu::btn_2efa, &jo::SaveMenu::btn_3240, &jo::SaveMenu::btn_3586, &jo::SaveMenu::mark, &jo::SaveMenu::btn_3c12, &jo::SaveMenu::btn_3f58, &jo::SaveMenu::in_bg, &jo::SaveMenu::in_frame, &jo::SaveMenu::name_edit>(self);
            return;
        }
        if (state == 2) {
            if (jo::as_save_menu(self)->frame.y == fr.dy - 0x288 && jo::as_save_menu(self)->bar_bottom.x == 0x400) jo::as_save_menu(self)->slide_pos = 3;
            for (i32 j = 0; j < 10; ++j) step(&save_slot(self, j));
            step_all<&jo::SaveMenu::list, &jo::SaveMenu::frame, &jo::SaveMenu::pic, &jo::SaveMenu::info_bar, &jo::SaveMenu::info, &jo::SaveMenu::bar_top, &jo::SaveMenu::bar_bottom, &jo::SaveMenu::in_bg, &jo::SaveMenu::in_frame, &jo::SaveMenu::name_edit, &jo::SaveMenu::btn_286e, &jo::SaveMenu::btn_2bb4, &jo::SaveMenu::btn_2efa, &jo::SaveMenu::btn_3240, &jo::SaveMenu::btn_3586, &jo::SaveMenu::mark, &jo::SaveMenu::btn_3c12, &jo::SaveMenu::btn_3f58>(self);
            return;
        }
        if (state != 3) return;
        if (jo::view<i8>(jo::as_save_menu(self)->load_done) != 0) {
            if (global<i8>(d::g_menu_frame_state) == 2) {
                joan::orig::as_menu(obj(d::g_hud))->slide_out();  // 0x006f26e0
            } else if (global<i8>(d::g_menu_frame_state) == 0) {
                jo::as_save_menu(self)->is_open_ = 0;
                joan::orig::as_save_menu(self)->free_images();  // 0x0074d9a0
                jo::as_save_menu(self)->load_done = 0;
                joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();  // 0x0042fef0
                reset_state(self);
                jo::as_save_menu(self)->from_victory = 0;
                jo::as_save_menu(self)->from_defeat = 0;
                joan::orig::as_msg_log(obj(d::g_portraits))->reset();            // 0x004b31c0
                joan::orig::as_mode(obj(d::g_game_a50088))->set_499540(joan::orig::arg::u32(i32{0xc}));  // 0x00499540
                joan::orig::as_triple(obj(d::g_screen_mgr))->clear();            // 0x006f5e90
            }
            return;
        }
        if (jo::view<i8>(jo::as_save_menu(self)->from_defeat) != 0) {
            jo::as_save_menu(self)->is_open_ = 0;
            joan::orig::as_save_menu(self)->free_images();              // 0x0074d9a0
            joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();  // 0x0042fef0
            reset_state(self);
            joan::orig::as_menu(obj(d::g_objective_menu))->open_7226a0(i32{2});  // 0x007226a0
            jo::as_save_menu(self)->from_victory = 0;
            jo::as_save_menu(self)->from_defeat = 0;
            return;
        }
        if (global<i8>(d::g_game_mode) == 9) {
            if (jo::view<i8>(jo::as_save_menu(self)->save_mode) == 2) {
                joan::orig::as_menu(obj(d::g_objective_menu))->open_7226a0(i32{2});  // 0x007226a0
                jo::as_save_menu(self)->is_open_ = 0;
                joan::orig::as_save_menu(self)->free_images();
                joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();
                reset_state(self);
                jo::as_save_menu(self)->from_victory = 0;
                return;
            }
            if (global<u32>(d::g_screen_pending) != 0 && global<u32>(d::g_screen_pending) != 2) {
                jo::as_save_menu(self)->is_open_ = 0;
                joan::orig::as_save_menu(self)->free_images();
                reset_state(self);
                joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
                jo::as_save_menu(self)->from_victory = 0;
                jo::as_save_menu(self)->from_defeat = 0;
                return;
            }
            if (global<i8>(d::g_menu_frame_state) == 2) {
                joan::orig::as_menu(obj(d::g_hud))->slide_out();  // 0x006f26e0
                return;
            }
            if (global<i8>(d::g_menu_frame_state) != 0) return;
            if (jo::view<i8>(jo::as_save_menu(self)->save_mode) == 1) joan::orig::as_screen(obj(d::g_game_over_screen))->close_6b8af0();  // 0x006b8af0
            jo::as_save_menu(self)->is_open_ = 0;
            joan::orig::as_save_menu(self)->free_images();
            joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();
            resume_timer();
            reset_state(self);
            jo::as_save_menu(self)->from_victory = 0;
            if (global<u32>(d::g_screen_pending) == 2) {
                joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
            } else {
                joan::orig::as_triple(obj(d::g_screen_mgr))->clear();  // 0x006f5e90
            }
            return;
        }
        jo::as_save_menu(self)->is_open_ = 0;
        joan::orig::as_save_menu(self)->free_images();
        joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();
        resume_timer();
        reset_state(self);
        jo::as_save_menu(self)->from_victory = 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_15", name, addr, fn)
B("SaveMenu::slide_751750", 0x00751750, &H::slide);
#undef B

}  // namespace game_b109_15
}  // namespace

// ---------------------------------------- from modules/game_b109_19.cpp (game/b109_19)
namespace {
namespace game_b109_19 {

u32 color(i32 r, i32 g, i32 b, i32 a) {  // 0x0042a260 colour ctor (ret 0x10)
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));
}

JOAN_COUNTER(c_layout, "Screen::layout_752970");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }
    // -------------------------------------------------------------- 0x00752970
    void place(void* g, i32 x, i32 y) { joan::orig::as_widget(g)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 1.0f, 0.0f, 1, 0u); }  // 0x0061e020
    std::uintptr_t place_r(void* g, i32 x, i32 y) {
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_widget(g)->move_to_61e020(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 1.0f, joan::orig::arg::f32(i32{0}), static_cast<char>(joan::orig::arg::i32(i32{1})), joan::orig::arg::u32(i32{0})));  // 0x0061e020
    }
    void show(void* g) { joan::orig::as_widget(g)->scale_to(0, 1.0f, 0.0f, 1, 0u); }  // 0x0061f1b0
    std::uintptr_t layout() {
        c_layout.hit();
        i32 dy = 0;
        if (global<i8>(d::g_game_mode) == 9) dy = -0x30;
        for (i32 i = 0; i < 10; ++i) {
            const u32 c = color(0, 0, 0, 0xff);
            joan::orig::as_button(&save_slot(this, i))->set_text(str(d::kEmpty), obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(c), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
            save_slot(this, i).held = 0;
            place(&save_slot(this, i), 0x74, dy + 0x46);
            show(&save_slot(this, i));
        }
        for (i32 i = jo::self<jo::SaveMenu>(this)->list.item_count; i >= 1; --i) joan::orig::as_combo_box(&jo::self<jo::SaveMenu>(this)->list)->remove_from(joan::orig::arg::i32(i));  // 0x006b3b90
        place(&jo::self<jo::SaveMenu>(this)->list, 0x74, dy + 0x3e);
        show(&jo::self<jo::SaveMenu>(this)->list);
        place(&jo::self<jo::SaveMenu>(this)->frame, 0x80, dy - 0x288);
        show(&jo::self<jo::SaveMenu>(this)->pic);
        place(&jo::self<jo::SaveMenu>(this)->pic, 0x244, dy - 0xa8);
        show(&jo::self<jo::SaveMenu>(this)->info_bar);
        place(&jo::self<jo::SaveMenu>(this)->info_bar, 0x264, dy + 0x1e0);
        show(&jo::self<jo::SaveMenu>(this)->info);
        place(&jo::self<jo::SaveMenu>(this)->info, 0x264, dy + 0x168);
        place(&jo::self<jo::SaveMenu>(this)->bar_top, -0x400, dy + 0xb4);
        place(&jo::self<jo::SaveMenu>(this)->bar_bottom, 0x400, dy + 0x2c8);
        place(&jo::self<jo::SaveMenu>(this)->btn_286e, 0x106, dy + 0x2c8);
        show(&jo::self<jo::SaveMenu>(this)->btn_286e);
        place(&jo::self<jo::SaveMenu>(this)->btn_2bb4, 0xed, dy + 0x2c8);
        show(&jo::self<jo::SaveMenu>(this)->btn_2bb4);
        place(&jo::self<jo::SaveMenu>(this)->btn_2efa, 0xed, dy + 0x2c8);
        show(&jo::self<jo::SaveMenu>(this)->btn_2efa);
        place(&jo::self<jo::SaveMenu>(this)->btn_3240, 0x1b5, dy + 0x2c8);
        show(&jo::self<jo::SaveMenu>(this)->btn_3240);
        place(&jo::self<jo::SaveMenu>(this)->btn_3586, 0x27d, dy + 0x2c8);
        show(&jo::self<jo::SaveMenu>(this)->btn_3586);
        place(&jo::self<jo::SaveMenu>(this)->mark, 0x1b5, dy + 0x2c8);
        show(&jo::self<jo::SaveMenu>(this)->mark);
        place(&jo::self<jo::SaveMenu>(this)->btn_3c12, 0x106, dy + 0x2c8);
        show(&jo::self<jo::SaveMenu>(this)->btn_3c12);
        place(&jo::self<jo::SaveMenu>(this)->btn_3f58, 0x264, dy + 0x2c8);
        show(&jo::self<jo::SaveMenu>(this)->btn_3f58);
        place(&jo::self<jo::SaveMenu>(this)->name_edit, 0x11e, dy + 0x2d3);
        show(&jo::self<jo::SaveMenu>(this)->name_edit);
        joan::orig::as_widget_child(&jo::self<jo::SaveMenu>(this)->name_edit.child)->disable();  // 0x006ca020
        show(&jo::self<jo::SaveMenu>(this)->in_bg);
        place(&jo::self<jo::SaveMenu>(this)->in_bg, 0x11e, dy + 0x2d2);
        show(&jo::self<jo::SaveMenu>(this)->in_frame);
        return place_r(&jo::self<jo::SaveMenu>(this)->in_frame, 0xe4, dy + 0x2c8);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_19", name, addr, fn)
B("Screen::layout_752970", 0x00752970, &H::layout);
#undef B

}  // namespace game_b109_19
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

JOAN_COUNTER(c_save_pages, "SaveMenu::pages_753060");

struct H {
    // ------------------------------------------------------------------ 0x00753060
    u32 save_pages(u32 a) {
        c_save_pages.hit();
        for (i32 k = jo::self<jo::SaveMenu>(this)->list.item_count; k >= 1; --k) {
            joan::orig::as_combo_box(&jo::self<jo::SaveMenu>(this)->list)->remove_from(joan::orig::arg::i32(k));  // 0x006b3b90
        }
        const i32 count = joan::orig::as_dynarray(obj(d::g_saves))->size();  // 0x00417ad0
        if (count % 10 > 0) {
            jo::self<jo::SaveMenu>(this)->state = static_cast<i8>(count / 10 + 1);
        } else {
            jo::self<jo::SaveMenu>(this)->state = static_cast<i8>(count / 10);
        }
        if (static_cast<i32>(jo::self<jo::SaveMenu>(this)->state) > 0) {
            // Original frame above the [ebp-0x30] sprintf buffer: loop counter [ebp-0x10], dead
            // count [ebp-0xc], dead [ebp-0x8], saved this [ebp-0x4] (re-read after every sprintf).
            // A text overflowing the buffer lands in the same fields as in the original, up to
            // the saved ebp / return address (which would crash the original as well).
            struct Frame {
                char text[0x20];
                i32 i;
                i32 count;
                i32 dead8;
                void* self;
            } f{};
#if UINTPTR_MAX == 0xffffffffu  // the original (x86) frame
            static_assert(sizeof(Frame) == 0x30, "frame");
#endif
            f.count = count;
            f.self = this;
            for (f.i = 1; f.i <= static_cast<i32>(jo::as_save_menu(f.self)->state); ++f.i) {
                const char* const s = joan::orig::as_save_menu(obj(d::g_strings))->open_22(joan::orig::arg::u32(f.i), joan::orig::arg::u32(// 0x004dca00
                                            static_cast<i32>(jo::as_save_menu(f.self)->state)));
                joan::orig::libc::sprintf(joan::orig::arg::str(static_cast<char*>(f.text)), joan::orig::arg::str(s));  // sprintf(buf, s)
                joan::orig::as_combo_box(&jo::as_save_menu(f.self)->list)->add_6b3930(joan::orig::arg::str(static_cast<void*>(f.text)));  // 0x006b3930
            }
            const u32 page = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->limit_int(joan::orig::arg::i32(a), i32{1}, joan::orig::arg::i32(// 0x00590aa0
                                           static_cast<i32>(jo::as_save_menu(f.self)->state))));
            jo::as_save_menu(f.self)->page = static_cast<u8>(page);
            joan::orig::as_popup(&jo::as_save_menu(f.self)->list)->set_text_6b44e0(joan::orig::arg::u32(page));  // 0x006b44e0
            joan::orig::as_save_menu(f.self)->fill_list(joan::orig::arg::i32(page));  // 0x00753250
            return 0;  // void callee; callers ignore the result
        }
        const char* const s = joan::orig::as_save_menu(obj(d::g_strings))->open_22(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));
        joan::orig::as_combo_box(&jo::self<jo::SaveMenu>(this)->list)->add_6b3930(joan::orig::arg::str(s));
        jo::self<jo::SaveMenu>(this)->page = 0;
        joan::orig::as_save_menu(this)->fill_list(i32{0});
        return joan::orig::arg::as<u32>(joan::orig::as_popup(&jo::self<jo::SaveMenu>(this)->list)->set_text_6b44e0(joan::orig::arg::u32(i32{1})));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("SaveMenu::pages_753060", 0x00753060, &H::save_pages);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {


void* const kFont = obj(d::g_font_goudy14_22_white);

std::int32_t list_count(void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0
std::uint32_t colour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) {
    std::uint32_t tmp = 0;
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&tmp)->construct(std::uint32_t{r}, std::uint32_t{g}, std::uint32_t{b}, std::uint32_t{a}));  // 0x0042a260
}

JOAN_COUNTER(c_fill, "Obj753::fill");

struct H {
    void* self() { return this; }

    // 0x00753250
    void line_text(std::int32_t line, const char* text) {
        const std::uint32_t c = colour(0, 0, 0, 0xff);
        joan::orig::as_button(&save_slot(this, line))->set_text(joan::orig::arg::str(text), kFont, joan::orig::arg::i32(c), 1, 2, 0, 2, 2);  // 0x00623ce0
    }
    void fill_753250(std::int32_t page) {
        c_fill.hit();
        if (page == 0) {
            for (std::int32_t i = 0; i < 10; ++i) line_text(i, str(d::kEmpty));
            return;
        }
        std::int16_t n = 0;
        const std::int32_t count = list_count(obj(d::g_saves));
        for (std::int32_t k = (page - 1) * 10 + 1; k <= count; ++k) {
            if (!(n < 10)) break;
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_units(obj(d::g_saves))->get_430410(joan::orig::arg::i16(static_cast<std::int16_t>(k))));  // 0x00430410
            line_text(n, jo::as_game_file(e)->name);
            n = static_cast<std::int16_t>(n + 1);
        }
        for (std::int32_t i = n; i < 10; ++i) line_text(i, str(d::kEmpty));
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj753::fill", 0x00753250, &H::fill_753250);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

JOAN_COUNTER(c_draw_infobar, "draw_infobar_753440");

void* const kRes = obj(d::g_misc);
void* const kText = obj(d::g_sprites_a46128);

u32 rgba(u8* frame_slot, float alpha_scale) {
    // fld 255.0f; fmul [p+0x11] (float product); _ftol; 0x0042a260(0xff, 0xff, 0xff, a)
    const float prod = global<float>(d::flt_255) * alpha_scale;
    const i32 a = joan::hook::x87::ftol(static_cast<double>(prod));
    u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_colour(frame_slot)->construct(static_cast<u8>(0xff), static_cast<u8>(0xff), static_cast<u8>(0xff), static_cast<u8>(a)));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(r));
}
// 0x00633500 (text draw, 17 args) with the common tail 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1.
void text_draw(i32 x, i32 y, u32 target, const char* font, jo::BmpRes* style, u32 color) {
    joan::orig::as_sprite_queue(kText)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(target), joan::orig::arg::str(font), joan::orig::arg::mptr(style), joan::orig::arg::u32(color), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}

// ---------------------------------------------------------------------- 0x00753440
i32 JOAN_CDECL draw_infobar(u8* g, u32 target, u32 flags) {
    const u32 fl = flags & 0xff;
    if (fl != 7 && (fl & 1) != 0) return 0;
    c_draw_infobar.hit();
    const joan::hook::x87::Fpu fpu{true};
    alignas(4) u32 w4 = jo::view<u16>(jo::as_button(g)->x);
    alignas(4) u32 w8 = jo::view<u16>(jo::as_button(g)->y);
    alignas(4) u32 wc = jo::view<u16>(jo::as_button(g)->x2);
    alignas(4) u32 w10 = jo::view<u16>(jo::as_button(g)->y2);
    alignas(4) float f14 = 1.0f;
    alignas(4) float f18 = 1.0f;
    if (jo::as_button(g)->fit != 0)
        joan::orig::as_sprite2d(kText)->fit_rect(joan::orig::arg::i16(jo::view<u16>(jo::as_button(g)->x)), joan::orig::arg::i16(jo::view<u16>(jo::as_button(g)->y)), joan::orig::arg::i16(jo::view<u16>(jo::as_button(g)->x2)), joan::orig::arg::i16(jo::view<u16>(jo::as_button(g)->y2)), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    const i32 x4 = static_cast<i16>(w4), y8 = static_cast<i16>(w8), xc = static_cast<i16>(wc);
    void* const font = obj(d::g_font_goudy12_bold_black);
    void* const strs = obj(d::g_strings);
    alignas(4) u8 col[4];
    {
        const u32 c = rgba(col, jo::as_button(g)->scale);
        text_draw(x4, y8, target, str(d::s_INFOBAR), global<jo::BmpRes*>(d::g_a411e8), c);
    }
    void* const s1 = joan::orig::arg::as<void*>(joan::orig::as_menu(strs)->savemenu_17());  // 0x004dca40
    const i32 tw = joan::orig::as_font(font)->measure(joan::orig::arg::str(s1), -1, 0, 0, 1.0f);  // 0x00624f00
    joan::orig::as_font(font)->text_height(joan::orig::arg::i16(2), 1.0f);                            // 0x006253f0
    {
        const u32 c = rgba(col, jo::as_button(g)->scale);
        jo::BmpRes* const style = global<jo::BmpRes*>(d::g_a411e8);
        const i32 y = y8 + 8;
        const i32 iv = static_cast<i32>(static_cast<u32>(x4) + static_cast<u32>(xc) - static_cast<u32>(tw));
        double v = fpu.mul(static_cast<double>(iv), global<double>(d::dbl_0_5));
        v = fpu.sub(v, global<double>(d::dbl_20));
        v = fpu.sub(v, global<double>(d::dbl_8));
        text_draw(joan::hook::x87::ftol(v), y, target, str(d::s_DIAMOND), style, c);
    }
    {
        const u32 c = rgba(col, jo::as_button(g)->scale);
        jo::BmpRes* const style = global<jo::BmpRes*>(d::g_a411e8);
        const i32 y = y8 + 8;
        const i32 iv = static_cast<i32>(static_cast<u32>(tw) + static_cast<u32>(x4) + static_cast<u32>(xc));
        double v = fpu.mul(static_cast<double>(iv), global<double>(d::dbl_0_5));
        v = fpu.add(v, global<double>(d::dbl_9ddb80));
        text_draw(joan::hook::x87::ftol(v), y, target, str(d::s_DIAMOND), style, c);
    }
    i32 dy = 0, dx = 0;
    if (jo::as_button(g)->pressed != 0) {
        const float s = global<float>(d::flt_1) * f18;
        dy = joan::orig::arg::as<i16>(joan::orig::as_misc(kRes)->round(joan::orig::arg::f64(static_cast<double>(s))));  // 0x00590a20
    }
    if (jo::as_button(g)->pressed != 0) {
        const float s = global<float>(d::flt_1) * f14;
        dx = joan::orig::arg::as<i16>(joan::orig::as_misc(kRes)->round(joan::orig::arg::f64(static_cast<double>(s))));  // 0x00590a20
    }
    const u32 c = rgba(col, jo::as_button(g)->scale);
    void* const s2 = joan::orig::arg::as<void*>(joan::orig::as_menu(strs)->savemenu_17());  // 0x004dca40
    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(static_cast<i32>(static_cast<i16>(w4)) + dx), static_cast<i32>(static_cast<u32>(static_cast<i32>(static_cast<i16>(w8))) + static_cast<u32>(dy) - 2u), joan::orig::arg::i32(static_cast<u16>(wc)), joan::orig::arg::i32(static_cast<i32>(static_cast<i16>(w10)) - 2), joan::orig::arg::u32(target), joan::orig::arg::str(s2), joan::orig::arg::u32(c), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x00626130
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("draw_infobar", 0x00753440, &draw_infobar);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b109_08.cpp (game/b109_08)
namespace {
namespace game_b109_08 {


using joan::hook::x87::ftol;

u32 hi(u32 v) { return v & 0xffff0000u; }
u32 lo16(i16 v) { return static_cast<u32>(static_cast<u16>(v)); }

JOAN_COUNTER(c_slot_draw, "SlotButton::draw_7538b0");

constexpr u32 kOne = 0x3f800000u;  // 1.0f
void* const kSprites = obj(d::g_sprites_a46128);

// ---- shared draw helpers ----------------------------------------------------------------
// 0x0042a260 (thunk 0x004098b8, ret 0x10): packs r, g, b, a into the 4-byte colour object.
u32 packed_colour(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));
}
// 0x00633500 (thunk 0x00401bbd, ret 0x44) with the trailing (3, 0, mode, 0, -1 x4, 0, -1, -1).
void sprite(u32 x, u32 y, u32 a3, const char* name, jo::BmpRes* sheet, u32 colour, i32 mode) {
    joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), joan::orig::arg::mptr(sheet), joan::orig::arg::u32(colour), 3, 0, joan::orig::arg::i16(mode), 0, -1, -1, -1, -1, 0, -1, -1);
}
// 0x00593980 (thunk 0x00403e4f, ret 0x10): mouse inside the rectangle.
i32 mouse_in(i16 x, i16 y, i16 w, i16 h) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(x), static_cast<i32>(y), static_cast<i32>(w), static_cast<i32>(h)));
}
// Scaled widgets: (short)0x00590a20(0xa45fa8, (double)(1.0f * scale)) when +0x8a is set, else 0.
i32 press_offset(const u8* p, float scale) {
    if (jo::as_button(p)->pressed == 0) return 0;
    const float v = global<float>(d::flt_1) * scale;
    return static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(v)))));  // 0x00590a20
}
// 0x00626130 (thunk 0x00402e0f, ret 0x34): text in a box.
void box_text(void* font, i32 x, i32 y, u32 w, u32 h, u32 a5, const void* text, u32 colour, u32 a8, u32 a9, u32 a10,
              u32 a11) {
    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(a5), joan::orig::arg::str(text), joan::orig::arg::u32(colour), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9), joan::orig::arg::i32(a10), joan::orig::arg::i32(a11), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
}
Rect widget_rect(u8* p) {
    Rect r{jo::as_button(p)->x, jo::as_button(p)->y, jo::as_button(p)->x2, jo::as_button(p)->y2, 1.0f, 1.0f};
    if (jo::as_button(p)->fit != 0) {
        joan::orig::as_sprite2d(kSprites)->fit_rect(joan::orig::arg::i16(jo::as_button(p)->x), joan::orig::arg::i16(jo::as_button(p)->y), joan::orig::arg::i16(jo::as_button(p)->x2), joan::orig::arg::i16(jo::as_button(p)->y2), &r.sx, &r.sy, &r.x, &r.y, &r.w, &r.h, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    }
    return r;
}

// ---- 0x007538b0: slot button draw callback (cdecl) ----------------------------------------
u32 JOAN_CDECL slot_draw(u8* p, u32 arg, u32 flags) {
    const u32 fl = flags & 0xff;
    if (fl != 7 && (fl & 1) != 0) return 0;
    c_slot_draw.hit();
    Rect r = widget_rect(p);
    auto alpha = [&]() {
        const float v = global<float>(d::flt_255) * jo::as_button(p)->scale;  // 255.0f * fade
        return static_cast<u32>(ftol(static_cast<double>(v)));
    };
    auto face = [&](img::Addr name) {
        const u32 c = packed_colour(0xff, 0xff, 0xff, alpha());
        jo::BmpRes* const sheet = global<jo::BmpRes*>(d::g_a411f8);
        sprite(lo16(r.x), hi(c) | lo16(r.y), arg, str(name), sheet, c, 2);
    };
    if (jo::as_button(p)->pressed != 0) {
        face(d::s_SLOT_DW);  // "SLOT_DW"
    } else if (mouse_in(r.x, r.y, r.w, r.h) != 0 && global<i8>(d::g_a45cd4) == 0) {
        face(d::s_SLOT_OV);  // "SLOT_OV"
    } else if (jo::view<i8>(jo::as_button(p)->held) != 0) {
        face(d::s_SLOT_OV);  // "SLOT_OV"
    } else {
        face(d::s_SLOT_OT);  // "SLOT_OT"
    }
    if (jo::view<i8>(jo::as_button(p)->caption[0]) == 0) return 1;
    char* const text = jo::as_button(p)->caption;
    void* const font = obj(d::g_font_goudy14_22_white);
    auto label = [&](u32 rgb) {
        const i32 dy = press_offset(p, r.sy);
        const i32 dx = press_offset(p, r.sx);
        const u32 c = packed_colour(rgb, rgb, rgb, alpha());
        box_text(font, static_cast<i32>(r.x) + dx, static_cast<i32>(r.y) + dy - 2, lo16(r.w),  // x64: the original's upper word was text's address; draw_3 reads a word

                 static_cast<u32>(static_cast<i32>(r.h) - 2), arg, text, c, 0, 1, 2, 0);
    };
    if (jo::view<i8>(jo::as_button(p)->held) != 0) {
        const i32 n = static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_font(font)->measure(joan::orig::arg::str(text), -1, 0, 0, joan::orig::arg::f32(kOne))));  // 0x00624f00
        joan::orig::as_font(font)->text_height(joan::orig::arg::i16(2), joan::orig::arg::f32(kOne));  // 0x006253f0 (result unused)
        const Fpu fpu{true};
        {
            const u32 c = packed_colour(0xff, 0xff, 0xff, alpha());
            jo::BmpRes* const sheet = global<jo::BmpRes*>(d::g_a411f8);
            const i32 v = static_cast<i32>(static_cast<u32>(static_cast<i32>(r.x)) +
                                           static_cast<u32>(static_cast<i32>(r.w)) - static_cast<u32>(n));
            const double xm = fpu.sub(fpu.sub(fpu.mul(static_cast<double>(v), global<double>(d::dbl_0_5)),
                                              global<double>(d::dbl_20)),
                                      global<double>(d::dbl_4));  // v * 0.5 - 20.0 - 4.0
            sprite(static_cast<u32>(ftol(xm)), static_cast<u32>(static_cast<i32>(r.y) + 8), arg, str(d::s_MARK), sheet,
                   c, 0);  // "MARK"
        }
        {
            const u32 c = packed_colour(0xff, 0xff, 0xff, alpha());
            jo::BmpRes* const sheet = global<jo::BmpRes*>(d::g_a411f8);
            const i32 v = static_cast<i32>(static_cast<u32>(n) + static_cast<u32>(static_cast<i32>(r.x)) +
                                           static_cast<u32>(static_cast<i32>(r.w)));
            const double xm =
                fpu.add(fpu.mul(static_cast<double>(v), global<double>(d::dbl_0_5)), global<double>(d::dbl_9ddb80));
            sprite(static_cast<u32>(ftol(xm)), static_cast<u32>(static_cast<i32>(r.y) + 8), arg, str(d::s_MARK), sheet,
                   c, 0);  // "MARK"
        }
        label(0xff);
    } else if (jo::as_button(p)->pressed == 0 && mouse_in(r.x, r.y, r.w, r.h) != 0 && global<i8>(d::g_a45cd4) == 0) {
        label(0xff);
    } else {
        label(0);
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_08", name, addr, fn)
B("SlotButton::draw_7538b0", 0x007538b0, &slot_draw);
#undef B

}  // namespace game_b109_08
}  // namespace

// ---------------------------------------- from modules/game_b109_02.cpp (game/b109_02)
namespace {
namespace game_b109_02 {

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), ret 0x10; returns this.
u32 make_color(i32 r, i32 g, i32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::as_colour(p));
}

JOAN_COUNTER(c_mark, "mark_draw_754220");

// 0x00590a20 (thunk 0x00401497) on 0xa45fa8: scale(double) -> word.
i32 scale_word(float s) {
    const float p = global<float>(d::flt_1) * s;
    return static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(p)))));
}
i32 ctl_alpha(void* ctl) {
    const float p = global<float>(d::flt_255) * jo::as_button(ctl)->scale;
    return joan::hook::x87::ftol(p);
}
void draw_mark(i32 x, i32 y, u32 surf, u32 color) {
    jo::BmpRes* const res = global<jo::BmpRes*>(d::g_a411f8);
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surf), str(d::s_MARK), joan::orig::arg::mptr(res), joan::orig::arg::u32(color), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// The two "MARK" sprites left / right of the caption (tw = caption width).
void draw_marks(void* ctl, u32 surf, i32 x, i32 y, i32 w, i32 tw) {
    const Fpu F{true};
    const u32 c1 = make_color(0xff, 0xff, 0xff, ctl_alpha(ctl));
    const i32 left = joan::hook::x87::ftol(
        F.sub(F.mul(static_cast<double>(x + w - tw), global<double>(d::dbl_0_5)), global<double>(d::dbl_20)));
    draw_mark(left, y + 8, surf, c1);
    const u32 c2 = make_color(0xff, 0xff, 0xff, ctl_alpha(ctl));
    const i32 right = joan::hook::x87::ftol(
        F.add(F.mul(static_cast<double>(tw + x + w), global<double>(d::dbl_0_5)), global<double>(d::dbl_9ddb80)));
    draw_mark(right, y + 8, surf, c2);
}
void draw_caption(void* ctl, u32 surf, i32 x, i32 y, i32 w, i32 h, float sx, float sy, i32 shade) {
    const i32 oy = jo::as_button(ctl)->pressed != 0 ? scale_word(sy) : 0;
    const i32 ox = jo::as_button(ctl)->pressed != 0 ? scale_word(sx) : 0;
    const u32 c = make_color(shade, shade, shade, ctl_alpha(ctl));
    joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_3(joan::orig::arg::i32(x + ox), joan::orig::arg::i32(y + oy - 1), static_cast<i32>(static_cast<i16>(w)), joan::orig::arg::i32(h - 1), joan::orig::arg::u32(surf), joan::orig::arg::str(jo::as_button(ctl)->caption), joan::orig::arg::u32(c), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}));  // 0x00626130
}
void caption_size(void* ctl, i32& tw) {
    tw = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(jo::as_button(ctl)->caption), -1, 0, 0, joan::orig::arg::f32(u32{0x3f800000}))));                       // 0x00624f00
    joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(i32{2}), joan::orig::arg::f32(u32{0x3f800000}));         // 0x006253f0 (unused)
}

u32 JOAN_CDECL mark_draw(void* ctl, u32 surf, u32 flags) {
    if ((flags & 0xff) != 7 && ((flags & 0xff) & 1) != 0) return 0;
    c_mark.hit();
    i16 x = jo::as_button(ctl)->x, y = jo::as_button(ctl)->y, w = jo::as_button(ctl)->x2, h = jo::as_button(ctl)->y2;
    float sx = 1.0f, sy = 1.0f;
    if (jo::as_button(ctl)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::as_button(ctl)->x), joan::orig::arg::i16(jo::as_button(ctl)->y), joan::orig::arg::i16(jo::as_button(ctl)->x2), joan::orig::arg::i16(jo::as_button(ctl)->y2), &sx, &sy, &x, &y, &w, &h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    if (jo::view<i8>(jo::as_button(ctl)->caption[0]) == 0) return 1;
    if (jo::as_button(ctl)->pressed != 0) {
        i32 tw = 0;
        caption_size(ctl, tw);
        draw_marks(ctl, surf, x, y, w, tw);
        draw_caption(ctl, surf, x, y, w, h, sx, sy, 0);
        return 1;
    }
    if (jo::as_button(ctl)->pressed == 0 && joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(x), static_cast<i32>(y), static_cast<i32>(w), static_cast<i32>(h))) != 0) {  // 0x00593980
        i32 tw = 0;
        caption_size(ctl, tw);
        draw_marks(ctl, surf, x, y, w, tw);
        draw_caption(ctl, surf, x, y, w, h, sx, sy, 0xff);
        return 1;
    }
    draw_caption(ctl, surf, x, y, w, h, sx, sy, 0xff);
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_02", name, addr, fn)
B("mark_draw_754220", 0x00754220, &mark_draw);
#undef B

}  // namespace game_b109_02
}  // namespace

// ---------------------------------------- from modules/game_b109_04.cpp (game/b109_04)
namespace {
namespace game_b109_04 {

using joan::orig::helpers::gf;

void* const kScreen = obj(d::g_sprites_a46128);
void* const kRound = obj(d::g_misc);

JOAN_COUNTER(c_mbut, "draw_mbut_754a80");

// --------------------------------------------------------------------- 0x00754a80
u32 colour_ff(u32* slot) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));  // 0x0042a260
}
void mbut_sprite(i32 x, i32 y, u32 ctx, std::uintptr_t name) {
    u32 tmp = 0;
    const u32 c = colour_ff(&tmp);
    joan::orig::as_sprite_queue(kScreen)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(str(name)), joan::orig::arg::mptr(global<jo::BmpRes*>(d::g_a411e8)), joan::orig::arg::u32(c), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
i32 round_half(float scale) {  // 0x00590a20((double)(1.0f * scale)), movsx ax
    const float v = gf(d::flt_1) * scale;
    return static_cast<i32>(static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_misc(kRound)->round(joan::orig::arg::f64(static_cast<double>(v))))));
}
void mbut_label(u8* button, u32 ctx, i16 w4, i16 w8, i16 wc, i16 w10, float f14, float f18) {
    const i32 dy = jo::as_button(button)->pressed != 0 ? round_half(f18) : 0;
    const i32 dx = jo::as_button(button)->pressed != 0 ? round_half(f14) : 0;
    u32 tmp = 0;
    const u32 c = *joan::orig::arg::as<u32*>(joan::orig::as_color(&tmp)->Color_dword(joan::orig::arg::u32(u32{0xff003366})));  // 0x005e9640
    joan::orig::as_text(obj(d::g_font_a489c8))->draw_3(joan::orig::arg::i32(static_cast<i32>(w4) + dx), joan::orig::arg::i32(static_cast<i32>(w8) + dy), joan::orig::arg::i32(// 0x00626130
                  static_cast<i32>(wc)), static_cast<i32>(w10), joan::orig::arg::u32(ctx), joan::orig::arg::str(jo::as_button(button)->caption), joan::orig::arg::u32(c), i32{0}, i32{0}, i32{2}, i32{0}, 1.0f, 1.0f);
}

static u32 JOAN_CDECL draw_mbut(u8* button, u32 ctx, u32 state_arg) {
    c_mbut.hit();
    const u32 state = state_arg & 0xff;
    if (state != 7 && (state & 1) != 0) return 0;
    i16 w4 = jo::as_button(button)->x;
    i16 w8 = jo::as_button(button)->y;
    i16 wc = jo::as_button(button)->x2;
    i16 w10 = jo::as_button(button)->y2;
    float f14 = 1.0f;
    float f18 = 1.0f;
    if (jo::as_button(button)->fit != 0) {
        joan::orig::as_sprite2d(kScreen)->fit_rect(joan::orig::arg::i16(// 0x00633e60
                      static_cast<i32>(jo::as_button(button)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->y2)), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
    }
    auto hover = [&]() -> bool {
        return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(w4), static_cast<i32>(w8), joan::orig::arg::i32(// 0x00593980
                             static_cast<i32>(wc)), static_cast<i32>(w10))) != 0;
    };
    std::uintptr_t face;
    if (jo::as_button(button)->pressed != 0)
        face = img::addr(d::s_MBUT_DW);  // MBUT_DW
    else if (hover())
        face = img::addr(d::s_MBUT_OV);  // MBUT_OV
    else
        face = img::addr(d::s_MBUT_OT);  // MBUT_OT
    mbut_sprite(static_cast<i32>(w4), static_cast<i32>(w8), ctx, face);
    mbut_sprite(static_cast<i32>(w4) - 4, static_cast<i32>(w8) - 4, ctx, img::addr(d::s_MBUT_CON));  // MBUT_CON
    if (jo::view<i8>(jo::as_button(button)->caption[0]) != 0) {
        // both branches of the original draw the same label (the hover test is still made)
        if (jo::as_button(button)->pressed == 0) (void)hover();
        mbut_label(button, ctx, w4, w8, wc, w10, f14, f18);
    }
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_04", name, addr, fn)
B("draw_mbut_754a80", 0x00754a80, &draw_mbut);
#undef B

}  // namespace game_b109_04
}  // namespace

// ---------------------------------------- from modules/game_b109_17.cpp (game/b109_17)
namespace {
namespace game_b109_17 {

JOAN_COUNTER(c_scroll, "draw_scroll_button_755120");

// ------------------------------------------------------------------ shared draw helpers
// Colour object 0x0042a260 (r, g, b, a) through thunk 0x004098b8; returns the packed dword.
u32 rgba(i32 r, i32 g, i32 b, i32 a) {
    u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&slot)->construct(r, g, b, a));  // 0x0042a260
}
// Colour object 0x005e9640 (packed argb) through thunk 0x00407577.
u32 argb(u32 v) {
    u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&slot)->Color_dword(joan::orig::arg::u32(v)));  // 0x005e9640
}
// Sprite 0x00633500 on 0xa46128 (17 args, ret 0x44).
void sprite(i32 x, i32 y, u32 ctx, const char* name, u32 tex, u32 color, i32 e7, i32 e8, i32 e9, i32 e10) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(name), joan::orig::arg::mptr(tex), joan::orig::arg::u32(color), joan::orig::arg::i16(e7), joan::orig::arg::i16(e8), joan::orig::arg::i16(e9), joan::orig::arg::i16(e10), -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// Boxed text 0x00626130 (13 args, ret 0x34).
void text_box(img::Addr font, i32 x, i32 y, i32 w, i32 h, u32 ctx, const char* s, u32 color, i32 e8, i32 e9,
              i32 e10, i32 e11) {
    joan::orig::as_text(obj(font))->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(ctx), s, joan::orig::arg::u32(color), joan::orig::arg::i32(e8), joan::orig::arg::i32(e9), joan::orig::arg::i32(e10), joan::orig::arg::i32(e11), 1.0f, 1.0f);  // 0x00626130
}
// Text width 0x00624f00 (s, -1, 0, 0, 1.0f), ret 0x14; ax.
u16 text_width(img::Addr font, const char* s) {
    return static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_font(obj(font))->measure(joan::orig::arg::str(s), -1, 0, 0, 1.0f)));  // 0x00624f00
}
// Pressed offset 0x00590a20 on 0xa45fa8 (double), movsx ax.
i32 press_offset(float scale) {
    const double v = static_cast<double>(global<float>(d::flt_1) * scale);
    return static_cast<i16>(static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(v)))));  // 0x00590a20
}

Rect button_rect(u8* b) {
    Rect r{jo::as_button(b)->x, jo::as_button(b)->y, jo::as_button(b)->x2, jo::as_button(b)->y2, 1.0f, 1.0f};
    if (jo::as_button(b)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(// 0x00633e60
                      static_cast<i32>(jo::as_button(b)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->y2)), &r.sx, &r.sy, &r.x, &r.y, &r.w, &r.h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
    }
    return r;
}
bool skip_state(u32 state_arg) {
    const u32 state = state_arg & 0xff;
    return state != 7 && (state & 1) != 0;
}
bool hover(const Rect& r) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(r.x), static_cast<i32>(r.y), static_cast<i32>(r.w), static_cast<i32>(r.h))) != 0;  // 0x00593980
}

// ------------------------------------------------------------------ 0x00755120
void scroll_sprite(i32 x, i32 y, u32 ctx, img::Addr name) {
    const u32 c = rgba(0xff, 0xff, 0xff, 0xff);
    sprite(x, y, ctx, str(name), global<u32>(d::g_a411f4), c, 3, 0, 0, 0);
}
void scroll_label(u8* b, const Rect& r, u32 ctx) {
    const i32 oy = jo::as_button(b)->pressed != 0 ? press_offset(r.sy) : 0;  // ebp-0x68 / -0x70
    const i32 ox = jo::as_button(b)->pressed != 0 ? press_offset(r.sx) : 0;  // ebp-0x6c / -0x74
    const u32 c = argb(0xff003366);
    text_box(d::g_font_a489c8, static_cast<i32>(r.x) + ox, static_cast<i32>(r.y) + oy, r.w, r.h, ctx,
             reinterpret_cast<const char*>(jo::as_button(b)->caption), c, 0, 0, 2, 0);
}

static u32 JOAN_CDECL draw_scroll_button_755120(u8* b, u32 ctx, u32 state) {
    c_scroll.hit();
    if (skip_state(state)) return 0;
    const Rect r = button_rect(b);
    const i32 x = r.x, y = r.y;
    if (jo::as_button(b)->pressed != 0) {
        scroll_sprite(x, y, ctx, d::s_SBUT_DW);          // SBUT_DW
        scroll_sprite(x - 4, y - 4, ctx, d::s_SBUT_CON);  // SBUT_CON
    } else if (jo::as_button(b)->pressed == 0 && hover(r)) {
        scroll_sprite(x, y, ctx, d::s_SBUT_OV);  // SBUT_OV
        text_width(d::g_font_a489c8, reinterpret_cast<const char*>(jo::as_button(b)->caption));  // result only feeds a dead local
        scroll_sprite(x - 4, y - 4, ctx, d::s_SBUT_CON);  // SBUT_CON
    } else {
        scroll_sprite(x, y, ctx, d::s_SBUT_OT);          // SBUT_OT
        scroll_sprite(x - 4, y - 4, ctx, d::s_SBUT_CON);  // SBUT_CON
    }
    if (jo::view<i8>(jo::as_button(b)->caption[0]) != 0) {
        if (jo::as_button(b)->pressed == 0 && hover(r)) {
            scroll_label(b, r, ctx);
        } else {
            scroll_label(b, r, ctx);
        }
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_17", name, addr, fn)
B("draw_scroll_button_755120", 0x00755120, &draw_scroll_button_755120);
#undef B

}  // namespace game_b109_17
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Panel::init_children() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_16::H::init_children>(this)); }  // 0x74d790
auto joan::orig::Menu::load_save() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08::H::load_save>(this)); }  // 0x74d900
auto joan::orig::SaveMenu::free_images() -> void { joan::bind::member<&::game_b92_06::H::release_0_10_14>(this); }  // 0x74d9a0
auto joan::orig::SaveMenu::open(std::uint8_t a0, std::int8_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::briefing_start>(this, a0, a1)); }  // 0x74da40
auto joan::orig::Panel::slide_in_74dbc0() -> void { joan::bind::member<&::game_b103_16::H::slide_74dbc0>(this); }  // 0x74dbc0
auto joan::orig::Screen::layout_752970() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_19::H::layout>(this)); }  // 0x752970
auto joan::orig::SaveMenu::pages_753060(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_11::H::save_pages>(this, a0)); }  // 0x753060
auto joan::orig::SaveMenu::fill_list(std::int32_t a0) -> void { joan::bind::member<&::game_b91_04::H::fill_753250>(this, a0); }  // 0x753250
