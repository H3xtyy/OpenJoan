// game/unknown_objm_strat.cpp: functions whose original source file is unknown (formerly game/_unplaced/objm_strat.cpp;
// re/attribution.csv: between ww/interfac/objmenub.cpp and ww/interfac/objmenuw.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>

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

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::str;

using joan::orig::helpers::op_new;  // operator new

using u8 = std::uint8_t;

using joan::img::obj;

using i16 = std::int16_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_16.cpp (game/b102_16)
namespace {
namespace game_b102_16 {

// new(n) + default ctor, null when new fails (SEH guard of the original not needed).
void* new_620ee0() {
    void* const m = op_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
    return m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(m)->ctor_620ee0()) : nullptr;  // 0x00620ee0
}

JOAN_COUNTER(c_build_735680, "Panel::build_735680");

struct H {
    // ------------------------------------------------------------------------ 0x00735680
    i32 build_735680() {
        c_build_735680.hit();
        jo::view<void*>(jo::self<jo::StrategicDlg>(this)->strat_up) = new_620ee0();
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::StrategicDlg>(this)->strat_up))->setup(joan::orig::arg::i16(0xa9), joan::orig::arg::i16(0xc2), joan::orig::arg::mptr(img::addr(d::g_bmp_commele2)), str(d::s_aw_u_ot), joan::orig::arg::str(// 0x006211d0
                      str(d::s_aw_u_dw)), str(d::s_aw_u_ov), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::StrategicDlg>(this)->strat_up))->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061f1b0
        jo::view<void*>(jo::self<jo::StrategicDlg>(this)->strat_down) = new_620ee0();
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::StrategicDlg>(this)->strat_down))->setup(joan::orig::arg::i16(0xa9), joan::orig::arg::i16(0x26c), joan::orig::arg::mptr(img::addr(d::g_bmp_commele2)), str(d::s_aw_d_ot), joan::orig::arg::str(// 0x006211d0
                      str(d::s_aw_d_dw)), str(d::s_aw_d_ov), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::StrategicDlg>(this)->strat_down))->scale_to(0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0), 1, 0u);  // 0x0061f1b0
        return 0;  // Widget::scale_to returns nothing; callers (0x0071e35b, ObjMenu) discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_16", name, addr, fn)
B("Panel::build_735680", 0x00735680, &H::build_735680);
#undef B

}  // namespace game_b102_16
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

constexpr fn::Code kDel6b33a0 = fn::ComboBox_scalar_dtor_6b33a0;  // 0x006b33a0

struct H {
    // ------------------------------------------------------------------ 0x00735850
    std::uintptr_t release_735850() {  // EAX = this / the deleting dtor's result (pointer-wide on x64)
        joan::orig::as_screen(this)->free_slots_and_parts();  // 0x0071f360
        std::uintptr_t eax = reinterpret_cast<std::uintptr_t>(this);
        if (jo::self<jo::CampScreen>(this)->strat_up != nullptr) {
            void* p = jo::self<jo::CampScreen>(this)->strat_up;
            eax = thiscall<std::uintptr_t>(kDel6b33a0, p, 1u);  // 0x006b33a0 (p != 0 here)
            jo::self<jo::CampScreen>(this)->strat_up = nullptr;
        }
        if (jo::self<jo::CampScreen>(this)->strat_down != nullptr) {
            void* p = jo::self<jo::CampScreen>(this)->strat_down;
            thiscall<u32>(kDel6b33a0, p, 1u);  // 0x006b33a0
            jo::self<jo::CampScreen>(this)->strat_down = nullptr;
            eax = reinterpret_cast<std::uintptr_t>(this);
        }
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Obj::release_735850", 0x00735850, &H::release_735850);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using i8 = std::int8_t;

void* const kUtil = obj(d::g_misc);

// CRT
char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
char* crt_strcat(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

// 0x006218c0 Button::clicked(a, 0, -1, 0, 0, 0)
i32 clicked(void* button, i32 a = 0) { return joan::orig::arg::as<i32>(joan::orig::as_ui_button(button)->mouse(joan::orig::arg::i32(a), 0, joan::orig::arg::i32(-1), 0, 0, 0)); }

JOAN_COUNTER(c_strategic, "StrategicDlg::update");

struct H {
    // ------------------------------------------------------------------ 0x00736120
    u8 strategic_update() {
        c_strategic.hit();
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->visible) != 8) return 0;
        if (jo::view<i32>(jo::self<jo::CampScreen>(this)->strat_first) > 0 && clicked(jo::view<void*>(jo::self<jo::CampScreen>(this)->strat_up)) != 0)
            joan::orig::as_obj_menu(this)->set_strategy_page(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::CampScreen>(this)->strat_first) - 1));  // 0x00736520
        if (jo::view<i32>(jo::self<jo::CampScreen>(this)->strat_first) < 6 && clicked(jo::view<void*>(jo::self<jo::CampScreen>(this)->strat_down)) != 0)
            joan::orig::as_obj_menu(this)->set_strategy_page(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::CampScreen>(this)->strat_first) + 1));  // 0x00736520
        if (clicked(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]), 0x1b) != 0) {
            joan::orig::as_camp_screen(this)->set_state_721a60(0u);  // 0x00721a60
            return 1;
        }
        for (i32 i = 0; i < 10; ++i) {
            if (clicked(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i])) == 0) continue;
            jo::self<jo::CampScreen>(this)->strat_selected = static_cast<i32>(static_cast<u32>(i) + jo::self<jo::CampScreen>(this)->strat_first);
            void* const dlg = obj(d::g_popup_a52df8);
            joan::orig::as_panel(dlg)->set_pos(joan::orig::arg::i16(0x263), joan::orig::arg::i16(0x1d4));  // 0x006d50e0
            joan::orig::as_popup(dlg)->set_text_6d5230(nullptr, nullptr);          // 0x006d5230
            joan::orig::as_popup(dlg)->set_title(nullptr, nullptr);          // 0x006d52c0
            joan::orig::as_popup(dlg)->set_buttons(0u);             // 0x006d5340
            joan::orig::as_popup(dlg)->load_gpopup();                // 0x006d3be0
            if (jo::view<void*>(jo::self<jo::CampScreen>(this)->strategy_text) != nullptr) {
                void* const old = jo::view<void*>(jo::self<jo::CampScreen>(this)->strategy_text);
                if (old != nullptr) joan::orig::as_file_txt(old)->destroy(1u);  // 0x00419280 deleting dtor
                jo::self<jo::CampScreen>(this)->strategy_text = nullptr;
            }
            char path[0x104];  // [ebp-0x118]
            crt_strcpy(path, str(d::s_DES_strategic));  // "DES\\strategic_"
            crt_strcat(path, joan::orig::arg::as<char*>(joan::orig::as_misc(kUtil)->format_int_58fca0(joan::orig::arg::i32(jo::self<jo::CampScreen>(this)->strat_selected + 1), 4)));  // 0x0058fca0
            crt_strcat(path, str(d::s_txt));  // ".txt"
            void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::FileTxt)));  // 0x19a on x86
            void* reader = nullptr;
            if (mem != nullptr) reader = joan::orig::as_file_txt(mem)->construct(joan::orig::arg::str(static_cast<char*>(path)));  // 0x00589550
            jo::view<void*>(jo::self<jo::CampScreen>(this)->strategy_text) = reader;
            break;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("StrategicDlg::update", 0x00736120, &H::strategic_update);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

struct H {
    // ------------------------------------------------------------------------ 0x00736480
    u32 update_pair() {
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->strat_up)->update();         // 0x0061f3b0
        return joan::orig::arg::as<u32>(joan::orig::as_widget(jo::self<jo::CampScreen>(this)->strat_down)->update());  // 0x0061f3b0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Pair::update_736480", 0x00736480, &H::update_pair);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

constexpr u32 kOne = 0x3f800000u;    // 1.0f

// 0x0061f1b0 Widget::set_anim(a, b, c, d, e)
void anim(void* w, i32 a, u32 b, i32 c, i32 d, i32 e) { joan::orig::as_widget(w)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e)); }

struct H {
    // ------------------------------------------------------------------ 0x007364c0
    i32 reset_two() {
        anim(jo::view<void*>(jo::self<jo::CampScreen>(this)->strat_up), 0, kOne, 0, 1, 0);
        anim(jo::view<void*>(jo::self<jo::CampScreen>(this)->strat_down), 0, kOne, 0, 1, 0);
        return 0;  // Widget::scale_to returns nothing; callers (0x00721968, ObjMenu) discard the result
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Obj::reset_two", 0x007364c0, &H::reset_two);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

JOAN_COUNTER(c_set_page, "X::set_page");

struct H {
    u32 set_page(i32 first) {  // 0x00736520 (eax = result of the last Button::set_text)
        c_set_page.hit();
        jo::view<i32>(jo::self<jo::CampScreen>(this)->strat_first) = first;
        u32 r = 0;
        for (i32 i = 0; i < 10; ++i) {
            const i32 idx = static_cast<i32>(static_cast<u32>(first) + static_cast<u32>(i));
            u32 tmp = 0;
            const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(0xff, 0xff, 0xff, 0xff));  // 0x0042a260
            void* const text = joan::orig::arg::as<void*>(joan::orig::as_obj_menu(obj(d::g_strings))->open_n(joan::orig::arg::i32(idx)));           // 0x004dc2f0
            r = joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]))->set_text(joan::orig::arg::str(text), img::obj(d::g_font_goudy12_bold_black), joan::orig::arg::i32(colour), 1, 2, 0, 2, 2);  // 0x00623ce0
        }
        return r;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::set_page", 0x00736520, &H::set_page);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

JOAN_COUNTER(c_reset_items, "X::reset_items");

const u32* color_ctor(void* c, u32 r, u32 g, u32 b, u32 a) {
    return joan::orig::arg::as<const u32*>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260 (returns this)
}
void anim_a(void* p, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    joan::orig::as_widget(p)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(f));  // 0x0061e020
}
void anim_b(void* p, u32 a, u32 b, u32 c, u32 d, u32 e) {
    joan::orig::as_widget(p)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
}

struct H {
    // ------------------------------------------------------------------------ 0x007365e0
    u32 reset_items() {
        c_reset_items.hit();
        jo::view<u32>(jo::self<jo::CampScreen>(this)->strat_selected) = 0xffffffffu;
        anim_b(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]), 1, 0x40c00000, 0, 0, 0xc8);
        anim_b(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar), 1, 0x40c00000, 0, 0, 0);
        anim_a(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar), 0x19, 0xb8, 0x455ac000, 0xc4bb8000, 0, 0);
        anim_b(jo::view<void*>(jo::self<jo::CampScreen>(this)->strat_up), 1, 0x40800000, 0, 0, 0xc8);
        anim_b(jo::view<void*>(jo::self<jo::CampScreen>(this)->strat_down), 1, 0x40800000, 0, 0, 0xc8);
        for (i32 i = 0; i < 10; ++i) {
            anim_b(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]), 1, 0x40800000, 0, 0, 0xc8);
            u32 tmp = 0;
            const u32* const p = color_ctor(&tmp, 0xff, 0xff, 0xff, 0xff);
            const u32 color = *p;
            joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]))->set_text(str(d::kEmpty), obj(d::g_font_goudy12_bold_black), joan::orig::arg::i32(color), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
        }
        return joan::orig::arg::as<u32>(joan::orig::as_obj_menu(this)->set_strategy_page(i32{0}));  // 0x00736520
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::reset_items", 0x007365e0, &H::reset_items);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

// 0x0061f1b0 (ret 0x14), raw dwords as pushed by the original.
using joan::orig::helpers::anim_set;
using joan::orig::helpers::anim_set6;  // 6.0f
// 0x0061e020 (ret 0x18)
void anim_move(void* o, std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t e,
               std::uint32_t f) {
    joan::orig::as_widget(o)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(f));
}

struct H {
    // 0x00736770
    void reset_736770() {
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]));
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar));
        anim_move(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar), 0xfffffe9cu, 0xb8u, 0x455ac000u, 0xc4bb8000u, 0, 0);
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->strat_up));
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->strat_down));
        for (std::int32_t i = 0; i < 10; ++i) anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Screen::reset_736770", 0x00736770, &H::reset_736770);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

JOAN_COUNTER(c_load_strategy, "Obj::load_strategy");

struct H {
    // ------------------------------------------------------------------ 0x00736880
    void load_strategy() {
        c_load_strategy.hit();
        if (jo::self<jo::CampScreen>(this)->strategy_texture != nullptr) return;
        void* const res = obj(d::g_texture_array);
        const i32 saved = joan::orig::arg::as<i32>(joan::orig::as_texture_manager(res)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(res)->set_mode(1u);                      // 0x0060a740
        jo::self<jo::CampScreen>(this)->strategy_texture = joan::orig::as_texture_manager(res)->load(str(d::s_STRATEGY), 1, 0u);  // 0x006099c0 "STRATEGY"
        joan::orig::as_texture_manager(res)->set_mode(joan::orig::arg::u32(saved));                  // 0x0060a740
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj::load_strategy", 0x00736880, &H::load_strategy);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

void* const kRes = obj(d::g_texture_array);

JOAN_COUNTER(c_free_strategy, "Obj::free_strategy");

struct H {
    // ------------------------------------------------------------------ 0x00736900
    void free_strategy() {
        c_free_strategy.hit();
        if (jo::self<jo::CampScreen>(this)->strategy_texture != nullptr) {
            joan::orig::as_texture_manager(kRes)->release(jo::self<jo::CampScreen>(this)->strategy_texture);  // 0x0060a550
            jo::self<jo::CampScreen>(this)->strategy_texture = nullptr;
        }
        joan::orig::as_popup(obj(d::g_popup_a52df8))->free_gpopup();  // 0x006d3c60
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->strategy_text) != nullptr) {
            joan::orig::as_file_txt(jo::view<void*>(jo::self<jo::CampScreen>(this)->strategy_text))->destroy(1u);  // 0x00419280 deleting dtor
            jo::self<jo::CampScreen>(this)->strategy_text = nullptr;
        }
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::free_strategy", 0x00736900, &H::free_strategy);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b111_11.cpp (fix/uninit)
namespace {
namespace game_b111_11 {

using joan::hook::global;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;


constexpr u32 kOne = 0x3f800000;  // 1.0f bits

JOAN_COUNTER(c_panel8, "Panel8::draw_735930");

// ============================================================================ thiscall methods
// 0x0042a260 (thunk 0x004098b8): 4-byte colour from r, g, b, a.
u32 make_color(u32 r, u32 g, u32 b, u32 a) {
    u8 col[4] = {0, 0, 0, 0};
    return jo::view<u32>(*jo::as_colour(joan::orig::arg::as<u8*>(joan::orig::as_colour(col)->construct(r, g, b, a))));
}
// 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
void draw_sprite(i32 x, i32 y, i32 layer, const char* name, img::Addr res, u32 color, i32 a7, i32 a8, i32 a9, i32 a10, i32 a11,
                 i32 a12, i32 a13, i32 a14, i32 a15, i32 a16, i32 a17) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(layer), joan::orig::arg::str(name), img::obj(res), joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(a15), joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));
}
using joan::orig::helpers::strcpy_;
char* strcat_(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

struct H {
    // ------------------------------------------------------------------ 0x00735930 (fix/uninit)
    void panel8_draw(u32 /*flag*/) {
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->visible) != 8) return;
        c_panel8.hit();
        joan::orig::as_label(jo::self<jo::CampScreen>(this)->side_bar)->draw(joan::orig::arg::u32(i32{3}), joan::orig::arg::u32(i32{7}));  // 0x00629bd0
        if (jo::view<i32>(jo::self<jo::CampScreen>(this)->strat_first) > 0) joan::orig::as_button(jo::self<jo::CampScreen>(this)->strat_up)->draw(3u, 7);  // 0x00622cc0
        if (jo::view<i32>(jo::self<jo::CampScreen>(this)->strat_first) < 6) joan::orig::as_button(jo::self<jo::CampScreen>(this)->strat_down)->draw(3u, 7);
        for (i32 i = 0; i < 10; ++i)
            joan::orig::as_button(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i])->draw(3u, 7);
        if (jo::self<jo::CampScreen>(this)->strat_selected == -1) return;
        joan::orig::as_popup(obj(d::g_popup_a52df8))->draw(i32{3}, i32{0x184}, i32{0xb8});  // 0x006d3cb0
        {
            u8 tmp[4] = {0, 0, 0, 0};
            const u32 color = jo::view<u32>(*jo::as_color(joan::orig::arg::as<u8*>(joan::orig::as_color(tmp)->Color_dword(joan::orig::arg::u32(i32{-1})))));  // 0x005e9640
            const char* const text = joan::orig::as_obj_menu(obj(d::g_strings))->open_n(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::CampScreen>(this)->strat_selected)));  // 0x004dc2f0
            joan::orig::as_text_renderer(obj(d::g_font_goudy_bold_20))->draw_wrapped(joan::orig::arg::u32(i32{0x1b0}), joan::orig::arg::u32(i32{0xc4}), joan::orig::arg::u32(i32{0x3ba}), joan::orig::arg::u32(i32{0x110}), joan::orig::arg::u32(i32{3}), text, joan::orig::arg::u32(color), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), joan::orig::arg::u32(i32{2}));  // 0x00626e00
        }
        if (static_cast<i16>(jo::as_bmp_entry(joan::orig::as_bmp_res(obj(d::g_set))->find(str(d::s_GOLDBAR)))->u16_32) != 0) {  // 0x00620b40 "GOLDBAR"
            const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
            draw_sprite(0x1b0, 0x110, 3, str(d::s_GOLDBAR), d::g_set, color, 3, 0, 0, 0, -1, -1, -1, 0x20b, 0, 0x3ba, -1);
        } else {
            const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
            draw_sprite(0x1b0, 0x110, 3, str(d::s_GOLDBAR), d::g_set, color, 3, 0, 0, 0, -1, -1, 0x20b, -1, 0, 0x3ba, -1);
        }
        img::Addr font = d::g_font_goudy_bold_14;  // [ebp-0x200]
        u16 y = 0x11c;          // [ebp-0x204]
        // [ebp-0x1fc], 0x1f4 bytes + read_line's NUL at buf[0x1f4] (a dead local in the original).
        // fix/uninit: zeroed, so a "/" line draws an empty text instead of uninitialised stack.
        char buf[0x1f8];
        std::memset(buf, 0, sizeof buf);
        char* const text = buf + 2;
        while (joan::orig::arg::as<i32>(joan::orig::as_file_txt(jo::self<jo::CampScreen>(this)->strategy_text)->at_eof()) == 0) {          // 0x0046f060 not at end
            joan::orig::as_file_txt(jo::self<jo::CampScreen>(this)->strategy_text)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), i32{0x1f4});  // 0x00589f10 read_line
            if (buf[0] == '~') {
                y = static_cast<u16>(y + static_cast<u16>(global<i16>(d::g_a48cb4) + 2));
                continue;
            }
            if (buf[0] != '/') continue;
            const u32 sel = static_cast<u32>(static_cast<i32>(buf[1]) - 0x4e);
            if (sel <= 0x26u) {
                switch (sel + 0x4eu) {
                    case 'S':
                    case 's': font = d::g_font_goudy_bold_14; break;
                    case 'T':
                    case 't': font = d::g_font_goudy_12; break;
                    case 'N':
                    case 'n': font = d::g_font_goudy_italic_12; break;
                    default: break;
                }
            }
            if (jo::self<jo::CampScreen>(this)->strat_selected == 0) {
                i32 pl = global<i32>(d::g_control_mode);
                if (pl == 4) pl = global<i32>(d::g_control_base_mode);
                // Table::lookup returns the key's name (a pointer, or 0): passed pointer-wide, since format_positional
                // reads a string argument with va_arg(const char*) (a u32 in an x64 vararg slot leaves the upper half undefined).
                const char* const v = joan::orig::arg::as<const char*>(joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(pl), i32{0x17}));  // 0x004cb5b0
                strcpy_(text, joan::orig::arg::as<const char*>(joan::orig::format_positional(joan::orig::arg::str(text), v)));             // 0x005a6660
            } else if (jo::self<jo::CampScreen>(this)->strat_selected == 1) {
                i32 pl = global<i32>(d::g_control_mode);
                if (pl == 4) pl = global<i32>(d::g_control_base_mode);
                const char* const v3 = joan::orig::arg::as<const char*>(joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(pl), i32{0x33}));
                const char* const v2 = joan::orig::arg::as<const char*>(joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(pl), i32{0x32}));
                const char* const v1 = joan::orig::arg::as<const char*>(joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(pl), i32{0x31}));
                const char* const v0 = joan::orig::arg::as<const char*>(joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(pl), i32{0x30}));
                strcpy_(text, joan::orig::arg::as<const char*>(joan::orig::format_positional(joan::orig::arg::str(text), v0, v1, v2, v3)));
            }
            {
                const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
                joan::orig::as_text(obj(font))->draw(i32{0x234}, static_cast<i32>(static_cast<i16>(y)), i32{0x3ba}, i32{0x2a3}, joan::orig::arg::u32(i32{2}), text, joan::orig::arg::u32(color), i32{2}, i32{1}, i32{1}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), i32{1}, i32{-1});  // 0x00626320
            }
            i16 lines = 0, extra = 0;  // [ebp-0x210] / [ebp-0x214], written by 0x00627d80
            joan::orig::as_text_box(obj(font))->measure_lines(i32{0x234}, static_cast<i32>(static_cast<i16>(y)), i32{0x3ba}, i32{0x2a3}, joan::orig::arg::str(text), i32{2}, i32{1}, &lines, &extra, joan::orig::arg::u32(kOne), joan::orig::arg::u32(kOne));  // 0x00627d80
            const i32 n = lines;
            const i32 step = n * static_cast<i32>(jo::as_font2d(obj(font))->line_height) + n * 2 - 2;
            y = static_cast<u16>(y + static_cast<u16>(step));
        }
        joan::orig::as_file_txt(jo::self<jo::CampScreen>(this)->strategy_text)->rewind();  // 0x0058a290 rewind
        for (i32 k = 0; k < static_cast<i32>(global<i8>(d::k_9df500 + static_cast<u32>(jo::self<jo::CampScreen>(this)->strat_selected))); ++k) {
            char name[32];  // [ebp-0x220] (12 bytes in the original; "ST<1..>_<1..2>" fits)
            strcpy_(name, str(d::s_ST));  // "ST"
            strcat_(name, joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(jo::self<jo::CampScreen>(this)->strat_selected + 1), i32{4})));  // 0x0058fca0
            strcat_(name, str(d::s_us_9cf774));  // "_"
            strcat_(name, joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(k + 1), i32{4})));
            const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
            draw_sprite(0x1b0, k * 0x84 + 0x11c, 2, name, d::g_bmp_res_strategy, color, 3, 0, 0, 0, -1, -1, -1, -1, 0,
                        -1, -1);
        }
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: zeroed line buffer, so a "/" line draws "" instead of uninitialised stack.
FIX("Panel8::draw_735930", 0x00735930, &H::panel8_draw);
#undef FIX

}  // namespace game_b111_11
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Panel::build_735680() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_16::H::build_735680>(this)); }  // 0x735680
auto joan::orig::ObjMenu::free_strategy() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_20::H::release_735850>(this)); }  // 0x735850
auto joan::orig::ObjMenu::draw_strategy(std::uint32_t a0) -> void { joan::bind::member<&::game_b111_11::H::panel8_draw>(this, a0); }  // 0x735930
auto joan::orig::ObjMenu::update_strategy() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_13::H::strategic_update>(this)); }  // 0x736120
auto joan::orig::ObjMenu::update_strategy_arrows() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_14::H::update_pair>(this)); }  // 0x736480
auto joan::orig::ObjMenu::reset_strategy_arrows() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08::H::reset_two>(this)); }  // 0x7364c0
auto joan::orig::ObjMenu::set_strategy_page(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_08::H::set_page>(this, a0)); }  // 0x736520
auto joan::orig::ObjMenu::enter_strategy() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::reset_items>(this)); }  // 0x7365e0
auto joan::orig::Screen::reset_736770() -> void { joan::bind::member<&::game_b92_06::H::reset_736770>(this); }  // 0x736770
auto joan::orig::ObjMenu::load_strategy_image() -> void { joan::bind::member<&::game_b96_08::H::load_strategy>(this); }  // 0x736880
auto joan::orig::ObjMenu::free_strategy_image() -> void { joan::bind::member<&::game_b97_08::H::free_strategy>(this); }  // 0x736900
