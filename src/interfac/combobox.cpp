// interfac/combobox.cpp: functions of the original d:\ww\interfac\combobox.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstddef>
#include <cstdarg>
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::call::cdecl_call;

using u8 = std::uint8_t;

using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
//
//    - ComboBox::free_parts 0x006b3130 (ret 0).
//
//  - No direct callers (vtables / dead code): everything except 0x00689ca0 and 0x006b3130.
//
//  - Functions whose eax is not obviously void return the value the original leaves in eax
//    (0x00666d00, 0x00675e10, 0x00689ca0, 0x0068ae90, 0x006b3130, 0x006822d0, 0x006824a0).
namespace {
namespace game_b102_14 {

JOAN_COUNTER(c_combo_free, "ComboBox::free_parts");

struct H {
    // ------------------------------------------------------------------ 0x006b3130
    void* free_parts() {
        c_combo_free.hit();
        if (jo::self<jo::ComboBox>(this)->created == 0) return this;
        jo::self<jo::ComboBox>(this)->font = nullptr;
        jo::self<jo::ComboBox>(this)->u32_88 = nullptr;
        if (jo::view<void*>(jo::self<jo::ComboBox>(this)->button) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ComboBox>(this)->button);
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(1u);  // 0x006b33a0
            jo::self<jo::ComboBox>(this)->button = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::ComboBox>(this)->list) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ComboBox>(this)->list);
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b3420(1u);  // 0x006b3420
            jo::self<jo::ComboBox>(this)->list = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_up) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_up);
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(1u);
            jo::self<jo::ComboBox>(this)->arrow_up = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_down) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_down);
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(1u);
            jo::self<jo::ComboBox>(this)->arrow_down = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::ComboBox>(this)->slider) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ComboBox>(this)->slider);
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(1u);
            jo::self<jo::ComboBox>(this)->slider = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::ComboBox>(this)->items) != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a223dc)) + 0x27;
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ComboBox>(this)->items)), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_interfac_combobox_cpp)), joan::orig::arg::i32(line));  // 0x0058ed70 free
            jo::self<jo::ComboBox>(this)->items = nullptr;
        }
        jo::self<jo::ComboBox>(this)->open = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("ComboBox::free_parts", 0x006b3130, &H::free_parts);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
//
//      0x006b33a0 ComboBox::`scalar deleting destructor' (ret 4)
namespace {
namespace game_b91_04 {

using joan::orig::helpers::game_delete;  // operator delete

struct H {
    void* self() { return this; }

    // 0x006b33a0
    void* combobox_scalar_dtor(std::uint32_t flags) {
        joan::orig::as_widget(self())->dtor_6211a0();  // 0x006211a0
        if ((flags & 1) != 0) game_delete(self());
        return self();
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("ComboBox::scalar_dtor", 0x006b33a0, &H::combobox_scalar_dtor);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
//
//      0x006b33f0 ComboBox::set_b2(v)               (thiscall, ret 4)
namespace {
namespace game_b97_08 {

struct H {
    // ------------------------------------------------------------------ 0x006b33f0
    // `this` is the combo box's ListBox (ComboBox::list); v is the item draw callback (a code address, called by
    // the list draw 0x006f8970 in interfac/list.cpp; ListBox+0xb2 is a pointer-sized member).
    void set_b2(std::uintptr_t v) { jo::self<jo::ListBox>(this)->draw_callback = reinterpret_cast<void*>(v); }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("ComboBox::set_b2", 0x006b33f0, &H::set_b2);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
//
//    - ComboBox::scalar_dtor 0x006b3420 (flags), ret 4: 0x006f8050, delete when flags & 1.
namespace {
namespace game_b101_14 {

using joan::orig::helpers::op_delete;                  // operator delete

struct H {
    // ------------------------------------------------------------------ 0x006b3420
    H* scalar_dtor(u32 flags) {
        joan::orig::as_list(this)->dtor_6f8050();  // 0x006f8050
        if ((flags & 1u) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("ComboBox::scalar_dtor", 0x006b3420, &H::scalar_dtor);
#undef B

}  // namespace game_b101_14
}  // namespace

// ---------------------------------------- from modules/game_b104_13.cpp (game/b104_13)
//
//      0x006b3470 Obj6b3470::move_children       (thiscall, ret 0, vtable 0x9dd7a0 slot) -
//                 shifts the three child rects by the own position delta.
//
//  - 0x006b3470 returns `this` on the early exit and (int)(short)[+0xb] otherwise (last eax).
namespace {
namespace game_b104_13 {

JOAN_COUNTER(c_move_children, "Obj6b3470::move_children");

struct H {
    // ------------------------------------------------------------------ 0x006b3470
    static void shift_xy(u8* r, i32 dx, i32 dy, bool x_first_pairs) {
        // x fields +9 +0xd +0x81 +0x85, y fields +0xb +0xf +0x83 +0x87 (original store order kept)
        jo::Button* const q = jo::as_button(r);
        auto add = [](i16& v, i32 delta) { v = static_cast<i16>(static_cast<u32>(static_cast<i32>(v)) + static_cast<u32>(delta)); };
        if (x_first_pairs) {  // 0x98 order: 9, d, b, f, 81, 85, 83, 87
            add(q->x, dx);
            add(q->x2, dx);
            add(q->y, dy);
            add(q->y2, dy);
            add(q->hit_rect[0], dx);
            add(q->hit_rect[2], dx);
            add(q->hit_rect[1], dy);
            add(q->hit_rect[3], dy);
        } else {  // 0x90 / 0x94 order: 9, b, d, f, 81, 83, 85, 87
            add(q->x, dx);
            add(q->y, dy);
            add(q->x2, dx);
            add(q->y2, dy);
            add(q->hit_rect[0], dx);
            add(q->hit_rect[1], dy);
            add(q->hit_rect[2], dx);
            add(q->hit_rect[3], dy);
        }
    }
    std::uintptr_t move_children() {
        if (jo::self<jo::ComboBox>(this)->created == 0) return reinterpret_cast<std::uintptr_t>(this);
        c_move_children.hit();
        const i32 dx = static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::ComboBox>(this)->x)) - static_cast<u32>(jo::self<jo::ComboBox>(this)->last_x));
        const i32 dy = static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::ComboBox>(this)->y)) - static_cast<u32>(jo::self<jo::ComboBox>(this)->last_y));
        if (jo::self<jo::ComboBox>(this)->style == 2) {
            shift_xy(jo::view<u8*>(jo::self<jo::ComboBox>(this)->slider), dx, dy, true);
            jo::view<u32>(jo::self<jo::ComboBox>(this)->slider->scale) = jo::view<u32>(jo::self<jo::ComboBox>(this)->scale);
            shift_xy(jo::view<u8*>(jo::self<jo::ComboBox>(this)->arrow_up), dx, dy, false);
            jo::view<u32>(jo::self<jo::ComboBox>(this)->arrow_up->scale) = jo::view<u32>(jo::self<jo::ComboBox>(this)->scale);
            shift_xy(jo::view<u8*>(jo::self<jo::ComboBox>(this)->arrow_down), dx, dy, false);
            jo::view<u32>(jo::self<jo::ComboBox>(this)->arrow_down->scale) = jo::view<u32>(jo::self<jo::ComboBox>(this)->scale);
        }
        jo::self<jo::ComboBox>(this)->last_x = static_cast<i32>(jo::self<jo::ComboBox>(this)->x);
        const i32 y = static_cast<i32>(jo::self<jo::ComboBox>(this)->y);
        jo::self<jo::ComboBox>(this)->last_y = y;
        return static_cast<std::uintptr_t>(static_cast<u32>(y));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_13", name, addr, fn)
B("Obj6b3470::move_children", 0x006b3470, &H::move_children);
#undef B

}  // namespace game_b104_13
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
//
//      0x006b3900 X::set_33a                (thiscall(v), ret 4)
namespace {
namespace game_b94_08 {

struct H {
    void set_33a(std::uintptr_t v) { jo::self<jo::Button>(this)->callback = v; }  // 0x006b3900 (Button::set_callback: a code address, full width)
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::set_33a", 0x006b3900, &H::set_33a);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
//
//    - ComboBox::add_6b3930 0x006b3930 (ret 4, combobox.cpp).
namespace {
namespace game_b108_10 {

JOAN_COUNTER(c_combo_add, "ComboBox::add_6b3930");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x006b3930
    u32 combo_add(const char* text) {
        c_combo_add.hit();
        if (jo::view<u32>(jo::self<jo::ComboBox>(this)->created) == 0) return 0;
        if (jo::view<u32>(jo::self<jo::ComboBox>(this)->style) == 2) {
            if (jo::view<u32>(jo::self<jo::ComboBox>(this)->item_count) == 0) {
                jo::self<jo::ComboBox>(this)->items = joan::orig::arg::as<u8*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(i32{0x191}), str(d::s_d_ww_interfac_combobox_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a223e0)) + 0xb)));  // 0x0058e7b0
            } else {
                jo::self<jo::ComboBox>(this)->items =
                    joan::orig::arg::as<u8*>(joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::self<jo::ComboBox>(this)->items), joan::orig::arg::u32((jo::view<u32>(jo::self<jo::ComboBox>(this)->item_count) + 1u) * 0x191u), str(d::s_d_ww_interfac_combobox_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a223e0)) + 0xe)));  // 0x0058eb30
            }
            void* const item = jo::view<u32>(jo::self<jo::ComboBox>(this)->item_count) * 0x191u + jo::self<jo::ComboBox>(this)->items;
            char* const dst = joan::orig::as_string(item)->c_str();  // 0x00417d90
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dst)), joan::orig::arg::str(text));  // strcpy
            jo::view<u32>(jo::self<jo::ComboBox>(this)->item_count) += 1;
            if (jo::view<u32>(jo::self<jo::ComboBox>(this)->selected) == 0) jo::view<u32>(jo::self<jo::ComboBox>(this)->selected) = jo::view<u32>(jo::self<jo::ComboBox>(this)->item_count);
            return jo::view<u32>(jo::self<jo::ComboBox>(this)->item_count);
        }
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_list(jo::self<jo::ComboBox>(this)->list)->append(joan::orig::arg::mptr(text)));  // 0x006f8670
        if (jo::view<u32>(jo::self<jo::ComboBox>(this)->selected) == 0) joan::orig::as_popup(me())->set_text_6b44e0(joan::orig::arg::u32(r));  // 0x006b44e0
        return r;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("ComboBox::add_6b3930", 0x006b3930, &H::combo_add);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
//
//    - ComboBox::find_item_6b3ad0 0x006b3ad0 (name), ret 4 (no references: dead code): +0x70 == 0
//      -> 0; mode +0x74 == 2: linear strcmp over the +0x9c records (0x191 bytes, count +0xa0),
//      match -> index + 1, no match -> this (eax left by the original); else List 0x006f87b0.
namespace {
namespace game_b102_15 {

using joan::orig::helpers::addr;

i32 strcmp_(const void* a, const void* b) { return std::strcmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // strcmp

JOAN_COUNTER(c_find_item, "ComboBox::find_item_6b3ad0");

struct H {
    // ------------------------------------------------------------------ 0x006b3ad0
    std::uintptr_t find_item_6b3ad0(const char* name) {
        c_find_item.hit();
        if (jo::view<u32>(jo::self<jo::ComboBox>(this)->created) == 0) return 0;
        if (jo::self<jo::ComboBox>(this)->style != 2) return joan::orig::arg::as<u32>(joan::orig::as_names(jo::view<void*>(jo::self<jo::ComboBox>(this)->list))->find_6f87b0(joan::orig::arg::str(name)));  // 0x006f87b0
        for (i32 i = 0; i < jo::self<jo::ComboBox>(this)->item_count; ++i) {
            u8* const rec = jo::self<jo::ComboBox>(this)->items + i * 0x191;
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(rec)->c_str());  // 0x00417d90
            if (strcmp_(name, s) == 0) return static_cast<u32>(i + 1);
        }
        return addr(this);  // eax = this on loop exit
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("ComboBox::find_item_6b3ad0", 0x006b3ad0, &H::find_item_6b3ad0);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
//
//      0x006b3b90 ComboBox::remove_from(int)        (thiscall, ret 4)
namespace {
namespace game_b108_18 {

JOAN_COUNTER(c_combo_remove, "ComboBox::remove_from");

struct H {
    // ------------------------------------------------------------------ 0x006b3b90
    void combo_remove_from(i32 n) {
        c_combo_remove.hit();
        if (jo::self<jo::ComboBox>(this)->created == 0) return;
        if (jo::self<jo::ComboBox>(this)->style == 2) {
            for (i32 i = 0; i < jo::self<jo::ComboBox>(this)->item_count; ++i) {
                if (i + 1 < n) continue;
                if (i + 1 < jo::self<jo::ComboBox>(this)->item_count) {
                    char* const src = joan::orig::as_string(jo::self<jo::ComboBox>(this)->items + static_cast<u32>(i + 1) * 0x191u)->c_str();  // 0x00417d90
                    char* const dst = joan::orig::as_string(jo::self<jo::ComboBox>(this)->items + static_cast<u32>(i) * 0x191u)->c_str();
                    // false positive: adjacent 0x191-byte item strings, the source never overlaps the destination
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrestrict"
#endif
                    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dst)), joan::orig::arg::str(static_cast<const char*>(src)));  // strcpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
                    continue;
                }
                jo::self<jo::ComboBox>(this)->item_count -= 1;
                if (jo::self<jo::ComboBox>(this)->item_count > 0) {
                    jo::view<void*>(jo::self<jo::ComboBox>(this)->items) = joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ComboBox>(this)->items)), joan::orig::arg::u32(static_cast<u32>(jo::self<jo::ComboBox>(this)->item_count + 1) * 0x191u), str(d::s_d_ww_interfac_combobox_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a223e4)) + 0x12));  // 0x0058eb30
                } else {
                    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ComboBox>(this)->items)), str(d::s_d_ww_interfac_combobox_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a223e4)) + 0x15));  // 0x0058ed70
                    jo::view<void*>(jo::self<jo::ComboBox>(this)->items) = nullptr;
                }
            }
            return;
        }
        joan::orig::as_list(jo::view<void*>(jo::self<jo::ComboBox>(this)->list))->remove_6f8830(joan::orig::arg::i32(n));  // 0x006f8830
        if (n == jo::self<jo::ComboBox>(this)->selected) {
            joan::orig::as_popup(this)->set_text_6b44e0(joan::orig::arg::u32(jo::self<jo::ComboBox>(this)->list->count != 0 ? 0 : 1));  // 0x006b44e0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("ComboBox::remove_from", 0x006b3b90, &H::combo_remove_from);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
//
//    - Obj::dtor_6b2340 0x006b2340: vtable 0x9dd7a0, 0x6b3130, then 0x61dfb0 (its eax returned).
//
//  - SEH frames (0x68bbb0, 0x6b2340, 0x6c92e0) only guard new + ctor: normal path reproduced.
namespace {
namespace game_b103_15 {

using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------------ 0x006b2340
    u32 dtor_6b2340() {
        jo::self<jo::ComboBox>(this)->vtable = img::obj(d::vt_Obj6b3470);
        joan::orig::as_combo_box(this)->free_parts();         // 0x006b3130
        return joan::orig::arg::as<u32>(joan::orig::as_gadget(this)->destruct());  // 0x0061dfb0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::dtor_6b2340", 0x006b2340, &H::dtor_6b2340);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
//
//    - Popup::set_text_6b44e0 0x006b44e0 (ret 4): text 0x00623ce0 (fmt or 0x00417d90 of the
//      0x194-byte string from 0x006b3d90 / 0x006f9360); its eax returned.
namespace {
namespace game_b108_04 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
JOAN_COUNTER(c_popup_text, "Popup::set_text_6b44e0");

// 0x0042a260 (thunk 0x004098b8, ret 0x10): builds a 4-byte colour in `c`, returns &c.
u32 make_color(i32 a, i32 b, i32 c, i32 d) {
    u32 color = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&color)->construct(a, b, c, d));
}

struct H {
    // ------------------------------------------------------------------ 0x006b44e0
    u32 set_text_6b44e0(u32 index) {
        c_popup_text.hit();
        jo::view<u32>(jo::self<jo::ComboBox>(this)->selected) = index;
        const bool mode2 = jo::view<u32>(jo::self<jo::ComboBox>(this)->style) == 2;
        auto text_button = [this, mode2]() { return mode2 ? jo::self<jo::ComboBox>(this)->slider : jo::self<jo::ComboBox>(this)->button; };  // read after the text is built, as in the original
        void* const font = img::obj(mode2 ? img::Addr{d::g_font_goudy14_22_white} : img::Addr{d::g_font_a489c8});
        if (index == 0) {
            const u32 c = make_color(0xff, 0xff, 0xff, 0xff);
            return joan::orig::arg::as<u32>(joan::orig::as_button(text_button())->set_text(img::str(d::kEmpty), font, joan::orig::arg::i32(c), i32{0}, i32{0}, i32{0}, i32{6}, i32{6}));  // 0x00623ce0
        }
        const u32 c = make_color(0xff, 0xff, 0xff, 0xff);
        alignas(4) u8 s[0x194];
        void* const got = mode2 ? joan::orig::arg::as<void*>(joan::orig::as_popup(this)->name_string_6b3d90(joan::orig::arg::mptr(static_cast<void*>(s)), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::ComboBox>(this)->selected))))  // 0x006b3d90
                                : joan::orig::as_list(jo::self<jo::ComboBox>(this)->list)->item(joan::orig::arg::mptr(static_cast<void*>(s)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::ComboBox>(this)->selected)));  // 0x006f9360
        const char* const chars = joan::orig::arg::as<const char*>(joan::orig::as_string(got)->c_str());  // 0x00417d90
        return joan::orig::arg::as<u32>(joan::orig::as_button(text_button())->set_text(chars, font, joan::orig::arg::i32(c), i32{0}, i32{0}, i32{0}, i32{6}, i32{6}));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Popup::set_text_6b44e0", 0x006b44e0, &H::set_text_6b44e0);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

using joan::call::thiscall;

using u8 = std::uint8_t;

using u32 = std::uint32_t;

struct H {
    u8* self() { return static_cast<u8*>(static_cast<void*>(this)); }

    // ------------------------------------------------------------------ 0x006b22f0
    void* widget_9dd7a0_ctor() {
        joan::orig::as_gadget(this)->ctor();  // 0x0061de00
        jo::view<std::uintptr_t>(jo::self<jo::ComboBox>(this)->vtable) = img::addr(d::vt_Obj6b3470);
        // The ComboBox members after the Gadget base: +0x70 .. the end (0x3d bytes on x86).
        jo::ComboBox* const c = jo::self<jo::ComboBox>(this);
        std::memset(static_cast<void*>(&c->created), 0, static_cast<std::size_t>(reinterpret_cast<u8*>(c + 1) - reinterpret_cast<u8*>(&c->created)));
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Widget_9dd7a0::ctor", 0x006b22f0, &H::widget_9dd7a0_ctor);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::hook::global;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using i16 = std::int16_t;

using joan::img::str;

using i8 = std::int8_t;

using joan::hook::x87::ftol;

using u32 = std::uint32_t;

void* const kSprites = obj(d::g_sprites_a46128);
void* const kFonts = obj(d::g_font_goudy14_22_white);
void* const kInput = obj(d::g_mouse);
void* const kRandom = obj(d::g_misc);

JOAN_COUNTER(c_page_cb, "PageButton::draw_cb");

// ---------------------------------------------------------------------------- draw helpers
// ftol(255 * button alpha) -> 0x0042a260 colour (r, g, b, a) through a 4-byte stack object.
u32 white_alpha(const u8* btn) {
    const i32 alpha = ftol(static_cast<double>(global<float>(d::flt_255) * jo::as_button(btn)->scale));
    u32 colour_obj = 0;
    const u32* const c = joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour_obj)->construct(0xffu, 0xffu, 0xffu, static_cast<u32>(alpha)));  // 0x0042a260
    return *c;
}
// 0x00633500 named sprite (17 args), the trailing 11 constant in all callers here.
void sprite(i32 x, i32 y, u32 a, const char* name, jo::BmpRes* tex, u32 colour) {
    joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a), joan::orig::arg::str(name), tex, joan::orig::arg::u32(colour), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// Button rectangle +9/+0xb/+0xd/+0xf and the optional 640x480 rescale 0x00633e60.
struct Rect {
    i16 x1, y1, x2, y2;
    float sx, sy;  // [ebp-0x14], [ebp-0x18]
};
Rect button_rect(const u8* btn) {
    Rect r{jo::as_button(btn)->x, jo::as_button(btn)->y, jo::as_button(btn)->x2, jo::as_button(btn)->y2, 1.0f, 1.0f};
    if (jo::as_button(btn)->fit != 0)
        joan::orig::as_sprite2d(kSprites)->fit_rect(joan::orig::arg::i16(static_cast<i32>(jo::as_button(btn)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(btn)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(btn)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(btn)->y2)), &r.sx, &r.sy, &r.x1, &r.y1, &r.x2, &r.y2, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    return r;
}

i32 JOAN_CDECL page_draw_cb(u8* btn, u32 a, u32 flags) {  // 0x006b23b0
    if ((flags & 0xff) != 7 && (flags & 0xff & 1) != 0) return 0;
    c_page_cb.hit();
    Rect r = button_rect(btn);
    i32 text_w = 0;  // [ebp-0x1c]
    if (jo::view<i8>(jo::as_button(btn)->caption[0]) != 0) {
        text_w = static_cast<i32>(joan::orig::as_font(kFonts)->measure(joan::orig::arg::str(jo::as_button(btn)->caption), -1, 0, 0, 1.0f));  // 0x00624f00
        joan::orig::as_font(kFonts)->text_height(joan::orig::arg::i16(2), 1.0f);  // 0x006253f0 (height, unused)
    }
    const char* name = str(d::s_PAGE_OT);  // "PAGE_OT"
    if (jo::as_button(btn)->enabled != 0) {
        if (jo::view<i8>(jo::as_button(btn)->held) != 0) {
            name = str(d::s_PAGE_OV);  // "PAGE_OV"
        } else if (jo::as_button(btn)->pressed != 0) {
            name = str(d::s_PAGE_OV);
        } else if (joan::orig::arg::as<i32>(joan::orig::as_point(kInput)->inside(static_cast<i32>(r.x1), static_cast<i32>(r.y1), static_cast<i32>(r.x2), static_cast<i32>(r.y2))) != 0) {  // 0x00593980
            name = str(d::s_PAGE_OV);
        }
    }
    {
        const u32 colour = white_alpha(btn);
        sprite(r.x1, r.y1, a, name, jo::as_button(btn)->sprite_set, colour);
    }
    if (jo::view<i8>(jo::as_button(btn)->caption[0]) != 0) {
        i32 dy = 0;  // [ebp-0x54]
        if (jo::as_button(btn)->pressed != 0)
            dy = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_misc(kRandom)->round(joan::orig::arg::f64(static_cast<double>(global<float>(d::flt_1) * r.sy)))));  // 0x00590a20
        i32 dx = 0;  // [ebp-0x58]
        if (jo::as_button(btn)->pressed != 0)
            dx = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_misc(kRandom)->round(joan::orig::arg::f64(static_cast<double>(global<float>(d::flt_1) * r.sx)))));  // 0x00590a20
        const u32 colour = white_alpha(btn);
        const i32 x = static_cast<i32>(r.x1) + (static_cast<i32>(r.x2) - static_cast<i32>(r.x1)) / 2 - text_w / 2 + dx;
        const i32 y = static_cast<i32>(r.y1) + dy + 0xd;
        joan::orig::as_text(jo::as_button(btn)->caption_box)->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(a), joan::orig::arg::str(jo::as_button(btn)->caption), joan::orig::arg::u32(colour), 0xffffffff, 0, 1, 2, 0, 1.0f, 1.0f);  // 0x006255d0
    }
    return 1;
}

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("PageButton::draw_cb", 0x006b23b0, &page_draw_cb);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b109_04.cpp (game/b109_04)
namespace {
namespace game_b109_04 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using i16 = std::int16_t;

using joan::img::str;

using u32 = std::uint32_t;

JOAN_COUNTER(c_optbox, "OptBox::init_6b29b0");

// --------------------------------------------------------------------- 0x00754a80
u32 colour_ff(u32* slot) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));  // 0x0042a260
}

// ------------------------------------------------------------------------------------ H
struct H {
    // ---------------------------------------------------------------- 0x006b29b0
    void* new_widget() {  // operator new 0x346 + 0x00620ee0
        void* const m = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
        return m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(m)->ctor_620ee0()) : nullptr;
    }
    void* new_list() {  // operator new 0xb6 + 0x006f8010
        void* const m = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::ListBox)));  // 0xb6 on x86
        return m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_list(m)->construct_6f8010()) : nullptr;
    }
    void option_pair(i32 x, i32 y, i32 d, img::Addr out, img::Addr dw, img::Addr ov, i32 cols,
                     img::Addr box, img::Addr lout, img::Addr ldw, img::Addr lov) {
        jo::view<void*>(jo::self<jo::ComboBox>(this)->button) = new_widget();
        jo::view<void*>(jo::self<jo::ComboBox>(this)->list) = new_list();
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::ComboBox>(this)->button))->setup(joan::orig::arg::i16(x + d), joan::orig::arg::i16(y + d), joan::orig::arg::mptr(jo::self<jo::ComboBox>(this)->font), joan::orig::arg::str(str(out)), joan::orig::arg::str(str(dw)), joan::orig::arg::str(str(ov)), joan::orig::arg::str(// 0x006211d0
                      i32{0}), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})));
        joan::orig::as_grid(jo::view<void*>(jo::self<jo::ComboBox>(this)->list))->setup_6f80c0(joan::orig::arg::u32(x), joan::orig::arg::u32(y + 0x1e), joan::orig::arg::u32(x + 0x64), joan::orig::arg::u32(y + 0x12c), joan::orig::arg::u32(i32{0}), joan::orig::arg::mptr(jo::self<jo::ComboBox>(this)->font), // 0x006f80c0
                      jo::self<jo::ComboBox>(this)->u32_88, joan::orig::arg::u32(cols), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0x1c}), joan::orig::arg::str(str(box)), joan::orig::arg::str(str(lout)), joan::orig::arg::str(str(ldw)), joan::orig::arg::str(str(lov)), joan::orig::arg::mptr(i32{0}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}));
    }
    void* optbox_init_6b29b0(u32 x_arg, u32 y_arg, i32 mode, u32 a, u32 b) {
        c_optbox.hit();
        const i32 x = static_cast<i32>(static_cast<i16>(x_arg));
        const i32 y = static_cast<i32>(static_cast<i16>(y_arg));
        jo::self<jo::ComboBox>(this)->last_x = x;
        jo::self<jo::ComboBox>(this)->last_y = y;
        jo::self<jo::ComboBox>(this)->x = static_cast<i16>(x_arg);
        jo::self<jo::ComboBox>(this)->y = static_cast<i16>(y_arg);
        jo::self<jo::ComboBox>(this)->style = mode;
        jo::self<jo::ComboBox>(this)->font = static_cast<jo::BmpRes*>(img::obj(mode == 2 ? img::Addr{d::g_bmp_commele2} : img::Addr{d::g_bmp_commele}));
        jo::self<jo::ComboBox>(this)->u32_88 = static_cast<jo::Font2d*>(img::obj(d::g_text_a5d140));
        if (mode == 0) {
            option_pair(x, y, 4, d::s_scmc_out, d::s_scmc_dw, d::s_scmc_ov, 4, d::s_soptbox, d::s_sopt_out, d::s_sopt_dw,
                        d::s_sopt_ov);  // scmc_* / soptbox
        } else if (mode == 1) {
            option_pair(x, y, 2, d::s_lcm_out, d::s_lcm_dw, d::s_lcm_ov, 2, d::s_loptbox, d::s_lopt_out, d::s_lopt_dw,
                        d::s_lopt_ov);  // lcm_* / loptbox
        } else if (mode == 2) {
            jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_up) = new_widget();
            jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_down) = new_widget();
            jo::view<void*>(jo::self<jo::ComboBox>(this)->slider) = new_widget();
            joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_up))->setup(joan::orig::arg::i16(x + 0x23), joan::orig::arg::i16(y + 0xb), joan::orig::arg::mptr(jo::self<jo::ComboBox>(this)->font), str(d::s_aw_l_ot), str(d::s_aw_l_dw), str(d::s_aw_l_ov), joan::orig::arg::str(i32{0}), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})));
            joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_down))->setup(joan::orig::arg::i16(x + 0x186), joan::orig::arg::i16(y + 0xb), joan::orig::arg::mptr(jo::self<jo::ComboBox>(this)->font), str(d::s_aw_r_ot), str(d::s_aw_r_dw), str(d::s_aw_r_ov), joan::orig::arg::str(i32{0}), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})));
            jo::self<jo::ComboBox>(this)->slider->sprite_set = jo::self<jo::ComboBox>(this)->font;
            joan::orig::as_button(jo::view<void*>(jo::self<jo::ComboBox>(this)->slider))->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(x + 0x1d0), joan::orig::arg::i32(y + 0x34), fn::code_value(fn::PageButton_draw_cb), joan::orig::arg::i32(// 0x00621540
                          i32{1}), i32{0}, i32{0}, i32{1}, i32{1}, i32{1});
            joan::orig::as_popup(this)->set_text_6b44e0(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::ComboBox>(this)->selected)));  // 0x006b44e0
            jo::view<u32>(jo::self<jo::ComboBox>(this)->item_count) = 0;
            jo::self<jo::ComboBox>(this)->items = nullptr;
            jo::self<jo::ComboBox>(this)->x2 = static_cast<i16>(static_cast<i32>(jo::self<jo::ComboBox>(this)->x) + 0x1d0);
            jo::self<jo::ComboBox>(this)->y2 = static_cast<i16>(static_cast<i32>(jo::self<jo::ComboBox>(this)->y) + 0x34);
        }
        if (mode == 0 || mode == 1) {
            joan::orig::as_button(jo::view<void*>(jo::self<jo::ComboBox>(this)->button))->set_callback(fn::code_value(fn::draw_item_6b4700));  // 0x006b3900
            joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::ComboBox>(this)->list))->set_b2(fn::code_value(fn::list_draw_6b49b0));  // 0x006b33f0
            u32 tmp = 0;
            const u32 c = colour_ff(&tmp);
            joan::orig::as_list(jo::view<void*>(jo::self<jo::ComboBox>(this)->list))->set_geometry(static_cast<jo::Font2d*>(img::obj(d::g_font_a489c8)), joan::orig::arg::u32(c), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{6}), joan::orig::arg::u32(// 0x006f8510
                          i32{6}));
            jo::self<jo::ComboBox>(this)->open = 0;
        }
        jo::view<u8>(jo::self<jo::ComboBox>(this)->enabled) = static_cast<u8>(a);
        jo::view<u8>(jo::self<jo::ComboBox>(this)->i8_8) = static_cast<u8>(b);
        jo::view<u32>(jo::self<jo::ComboBox>(this)->created) = 1;
        return this;
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_04", name, addr, fn)
B("OptBox::init_6b29b0", 0x006b29b0, &H::optbox_init_6b29b0);
#undef B

}  // namespace game_b109_04
}  // namespace

// ---------------------------------------- from modules/game_b111_07.cpp (game/b111_07)
namespace {
namespace game_b111_07 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_name_string, "Popup::name_string");

struct H {
    // ------------------------------------------------------------------------------ 0x006b3d90
    void* name_string(void* out, u32 idx) {
        c_name_string.hit();
        alignas(4) u8 s[0x194];
        std::memset(s, 0, sizeof s);      // bytes after the NUL are never read (see the header)
        joan::orig::as_string(s)->construct();  // 0x005a1170
        if (jo::view<u32>(jo::self<jo::ComboBox>(this)->created) == 0) {
            joan::orig::as_string(out)->construct_copy(joan::orig::as_string(static_cast<void*>(s)));  // 0x005a11f0
            return out;
        }
        if (jo::view<u32>(jo::self<jo::ComboBox>(this)->style) == 2) {
            u8* const src = jo::self<jo::ComboBox>(this)->items + (idx - 1u) * 0x191u;
            joan::orig::as_string(out)->construct_copy(joan::orig::as_string(src));  // 0x005a11f0
            return out;
        }
        alignas(4) u8 t[0x194];
        std::memset(t, 0, sizeof t);
        void* const r = joan::orig::arg::as<void*>(joan::orig::as_list(jo::self<jo::ComboBox>(this)->list)->item(joan::orig::arg::mptr(static_cast<void*>(t)), joan::orig::arg::i32(idx)));  // 0x006f9360
        joan::orig::as_string(out)->construct_copy(joan::orig::as_string(r));  // 0x005a11f0
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_07", name, addr, fn)
B("Popup::name_string_6b3d90", 0x006b3d90, &H::name_string);
#undef B

}  // namespace game_b111_07
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04_2 {

struct H {
    // 0x006b3e50
    void set_flag_6b3e50(std::int8_t c) {
        if (c > 0) {
            jo::self<jo::ComboBox>(this)->pressed = 1;
            jo::self<jo::ComboBox>(this)->slider->held = 1;
        } else {
            jo::self<jo::ComboBox>(this)->pressed = 0;
            jo::self<jo::ComboBox>(this)->slider->held = 0;
        }
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj6b3::set_flag", 0x006b3e50, &H::set_flag_6b3e50);
#undef B91

}  // namespace game_b91_04_2
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;

void* const kText = obj(d::g_sprites_a46128);
void* const kKeys = obj(d::g_mouse);

using joan::orig::helpers::strcpy_;
char* strcat_(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

u32 keys_93980(i32 a, i32 b, i32 c, i32 d) { return joan::orig::arg::as<u32>(joan::orig::as_point(kKeys)->inside(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d))); }  // 0x00593980

JOAN_COUNTER(c_spell_draw, "SpellBtn::draw");

struct H {
    // ------------------------------------------------------------------------ 0x006b3ec0
    static u32 colour_ff() {
        u32 c = 0;
        return *joan::orig::arg::as<const u32*>(joan::orig::as_color(&c)->Color_dword(0xffffffffu));  // 0x005e9640
    }
    static void text_out(i32 x, i32 y, u32 a, const char* s, jo::BmpRes* font, u32 colour) {
        joan::orig::as_sprite_queue(kText)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a), joan::orig::arg::str(s), font, joan::orig::arg::u32(colour), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
    }
    void spell_draw(u32 a, u8 flag) {
        c_spell_draw.hit();
        if (jo::self<jo::ComboBox>(this)->style == 2) {
            joan::orig::as_button(jo::view<void*>(jo::self<jo::ComboBox>(this)->slider))->draw(joan::orig::arg::u32(a), static_cast<std::uint8_t>(flag));  // 0x00622cc0
            joan::orig::as_button(jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_up))->draw(joan::orig::arg::u32(a), static_cast<std::uint8_t>(flag));
            joan::orig::as_button(jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_down))->draw(joan::orig::arg::u32(a), static_cast<std::uint8_t>(flag));
            return;
        }
        joan::orig::as_button(jo::view<void*>(jo::self<jo::ComboBox>(this)->button))->draw(joan::orig::arg::u32(a), static_cast<std::uint8_t>(flag));  // 0x00622cc0
        if (jo::self<jo::ComboBox>(this)->style == 0) {
            const u32 col = colour_ff();
            text_out(*reinterpret_cast<const i16*>(&jo::self<jo::ComboBox>(this)->last_x), *reinterpret_cast<const i16*>(&jo::self<jo::ComboBox>(this)->last_y), a, str(d::s_scmc_con), jo::self<jo::ComboBox>(this)->font, col);  // "scmc_con"
        } else if (jo::self<jo::ComboBox>(this)->style == 1) {
            const u32 col = colour_ff();
            text_out(*reinterpret_cast<const i16*>(&jo::self<jo::ComboBox>(this)->last_x), *reinterpret_cast<const i16*>(&jo::self<jo::ComboBox>(this)->last_y), a, str(d::s_lcm_con), jo::self<jo::ComboBox>(this)->font, col);  // "lcm_con"
        }
        char name[16];
        if (jo::self<jo::ComboBox>(this)->open != 0)
            strcpy_(name, str(d::s_sbup));  // "sbup_"
        else
            strcpy_(name, str(d::s_sbdw));  // "sbdw_"
        u8* const btn = jo::view<u8*>(jo::self<jo::ComboBox>(this)->button);
        if (jo::as_button(btn)->pressed != 0) {
            strcat_(name, str(d::s_dw));  // "dw"
        } else if (keys_93980(jo::self<jo::ComboBox>(this)->button->x, jo::self<jo::ComboBox>(this)->button->y, jo::self<jo::ComboBox>(this)->button->x2,
                              jo::self<jo::ComboBox>(this)->button->y2) != 0) {
            strcat_(name, str(d::s_ov));  // "ov"
        } else {
            strcat_(name, str(d::s_out));  // "out"
        }
        if (jo::self<jo::ComboBox>(this)->style == 0) {
            const u32 col = colour_ff();
            text_out(static_cast<i32>(jo::self<jo::ComboBox>(this)->button->x) + 0xe2, jo::self<jo::ComboBox>(this)->button->y, a, name,
                     jo::self<jo::ComboBox>(this)->font, col);
        } else if (jo::self<jo::ComboBox>(this)->style == 1) {
            const u32 col = colour_ff();
            text_out(static_cast<i32>(jo::self<jo::ComboBox>(this)->button->x) + 0x11c, jo::self<jo::ComboBox>(this)->button->y, a, name,
                     jo::self<jo::ComboBox>(this)->font, col);
        }
        if (jo::self<jo::ComboBox>(this)->open != 0) joan::orig::as_list(jo::view<void*>(jo::self<jo::ComboBox>(this)->list))->draw(joan::orig::arg::u32(a), joan::orig::arg::u32(flag));  // 0x006f8970
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("SpellBtn::draw", 0x006b3ec0, &H::spell_draw);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;

using joan::orig::helpers::add32;
i32 sub32(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(b)); }

JOAN_COUNTER(c_menu_step, "Menu::step_6b4280");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x006b4280
    i32 step_6b4280() {
        c_menu_step.hit();
        auto pressed = [](void* button) {
            return joan::orig::arg::as<i32>(joan::orig::as_ui_button(button)->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0));  // 0x006218c0
        };
        if (jo::self<jo::ComboBox>(this)->style == 2) {
            if (pressed(jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_up)) != 0) {
                if (jo::self<jo::ComboBox>(this)->selected > 1) joan::orig::as_popup(this)->set_text_6b44e0(joan::orig::arg::u32(sub32(jo::self<jo::ComboBox>(this)->selected, 1)));  // 0x006b44e0
                else joan::orig::as_popup(this)->set_text_6b44e0(joan::orig::arg::u32(jo::self<jo::ComboBox>(this)->item_count));
                return 1;
            }
            if (pressed(jo::view<void*>(jo::self<jo::ComboBox>(this)->arrow_down)) != 0) {
                if (jo::self<jo::ComboBox>(this)->selected < jo::self<jo::ComboBox>(this)->item_count) joan::orig::as_popup(this)->set_text_6b44e0(joan::orig::arg::u32(add32(jo::self<jo::ComboBox>(this)->selected, 1)));
                else joan::orig::as_popup(this)->set_text_6b44e0(1u);
                return 2;
            }
            return 0;
        }
        if (pressed(jo::view<void*>(jo::self<jo::ComboBox>(this)->button)) > 0) {
            jo::self<jo::ComboBox>(this)->open = jo::self<jo::ComboBox>(this)->open == 0 ? 1u : 0u;
            jo::view<u32>(jo::self<jo::ComboBox>(this)->list->pressed) = 0;
            return 1;
        }
        if (static_cast<i8>(jo::view<u8>(jo::self<jo::ComboBox>(this)->button->pressed)) != 0) return 0;
        if (jo::self<jo::ComboBox>(this)->open == 0) return 0;
        const i32 sel = joan::orig::arg::as<i32>(joan::orig::as_grid(jo::view<void*>(jo::self<jo::ComboBox>(this)->list))->hit_6f8e40());  // 0x006f8e40
        if (sel == -1) return 1;
        if (sel > 0) {
            jo::self<jo::ComboBox>(this)->open = 0;
            jo::view<u32>(jo::self<jo::ComboBox>(this)->list->pressed) = 0;
            joan::orig::as_popup(this)->set_text_6b44e0(joan::orig::arg::u32(sel));
            return 1;
        }
        if (sel != 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_cursor(obj(d::g_mouse))->flag_66c()) != 0) {  // 0x006b4dd0
            jo::self<jo::ComboBox>(this)->open = 0;
            jo::view<u32>(jo::self<jo::ComboBox>(this)->list->pressed) = 0;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Menu::step_6b4280", 0x006b4280, &H::step_6b4280);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;

JOAN_COUNTER(c_draw_item, "draw_item_6b4700");

// ------------------------------------------------------------------ 0x006b4700
static u32 JOAN_CDECL draw_item(void* b, u32 a) {
    c_draw_item.hit();
    jo::Button* const bt = jo::as_button(b);
    i32 sel = 0;
    if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(bt->x), joan::orig::arg::i32(bt->y), joan::orig::arg::i32(bt->x2), joan::orig::arg::i32(bt->y2))) != 0) {  // 0x00593980
        sel = 2;
        if (jo::as_button(b)->pressed != 0) sel = 1;
    }
    const jo::ButtonState& st = bt->states[sel];
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(bt->x), joan::orig::arg::i16(bt->y), joan::orig::arg::u32(a), joan::orig::arg::str(st.sprite), joan::orig::arg::mptr(jo::as_button(b)->sprite_set), joan::orig::arg::u32(st.colour), 3, joan::orig::arg::i16(st.a20), joan::orig::arg::i16(st.a24), joan::orig::arg::i16(st.a28), -1, -1, -1, -1, 0, joan::orig::arg::i16(bt->x2), joan::orig::arg::i16(bt->y2));  // 0x00633500
    if (jo::view<i8>(jo::as_button(b)->caption[0]) != 0) {
        const img::Addr font = jo::as_button(b)->pressed != 0 ? img::Addr{d::g_text_a58ac0} : img::Addr{d::g_font_a489c8};
        joan::orig::as_text(obj(font))->draw_line(joan::orig::arg::i32(bt->x + bt->press_dx), joan::orig::arg::i32(bt->y + bt->press_dy), joan::orig::arg::u32(a), joan::orig::arg::str(// 0x006255d0
                      bt->caption), joan::orig::arg::u32(jo::as_button(b)->caption_colour), i32{-1}, i32{0}, joan::orig::arg::i32(bt->caption_330), joan::orig::arg::i32(bt->caption_332), joan::orig::arg::i32(bt->caption_334), 1.0f, 1.0f);
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("draw_item_6b4700", 0x006b4700, &draw_item);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;

using joan::orig::helpers::imul;

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), ret 0x10; returns this.
u32 make_color(u32 r, u32 g, u32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::as_colour(p));
}

JOAN_COUNTER(c_listdraw, "list_draw_6b49b0");

// 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
void draw_sprite(i32 x, i32 y, u32 surf, const void* a4, void* a5, u32 a6, i32 a7, i32 a8, i32 a9, i32 a10) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surf), joan::orig::arg::str(a4), joan::orig::arg::mptr(a5), joan::orig::arg::u32(a6), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, -1, -1);
}

// 0x006b49b0
u32 JOAN_CDECL list_draw(void* ctl, u32 surf, u32 flags) {
    c_listdraw.hit();
    const i32 first = jo::as_list_box(ctl)->scroll != nullptr ? jo::as_list_box(ctl)->scroll->pos : 1;
    if ((flags & 0xff) == 7 || ((flags & 0xff) & 1) != 0) {
        for (i32 i = 0; i < joan::orig::arg::as<i32>(joan::orig::as_list(ctl)->page_count()); ++i) {  // 0x006f8900
            const i32 x = jo::as_list_box(ctl)->x0;
            const i32 y = jo::as_list_box(ctl)->y0 + imul(jo::as_list_box(ctl)->row_height, i);
            i32 k = 1;
            if (i == joan::orig::arg::as<i32>(joan::orig::as_list(ctl)->hover_index())) {  // 0x006f93b0
                k = 3;
                if (!(first + i > jo::as_list_box(ctl)->count) && jo::view<u32>(jo::as_list_box(ctl)->pressed) != 0) k = 2;
            }
            const jo::ListBoxColumn* const col = &jo::as_list_box(ctl)->column0 + k;  // column0 .. column3 are consecutive
            if (jo::view<i8>(col->name[0]) != 0 && jo::as_list_box(ctl)->bitmap != nullptr) {
                draw_sprite(x + jo::as_list_box(ctl)->text_x, y + jo::as_list_box(ctl)->text_y, surf, col->name,
                            jo::as_list_box(ctl)->bitmap, col->colour, 3, jo::view<i16>(col->u16_d),
                            jo::view<i16>(col->u16_f), jo::view<i16>(col->u16_11));
            }
            if (jo::view<i8>(jo::as_list_box(ctl)->column0.name[0]) != 0 && jo::as_list_box(ctl)->bitmap != nullptr) {
                draw_sprite(static_cast<i16>(x), static_cast<i16>(y), surf, jo::as_list_box(ctl)->column0.name,
                            jo::as_list_box(ctl)->bitmap, jo::as_list_box(ctl)->column0.colour, 3, jo::view<i16>(jo::as_list_box(ctl)->column0.u16_d), jo::view<i16>(jo::as_list_box(ctl)->column0.u16_f),
                            jo::view<i16>(jo::as_list_box(ctl)->column0.u16_11));
            }
        }
    }
    if ((flags & 0xff) == 7 || ((flags & 0xff) & 4) != 0) {
        for (i32 j = 0; j < joan::orig::arg::as<i32>(joan::orig::as_list(ctl)->page_count()); ++j) {  // 0x006f8900
            if (first + j > jo::as_list_box(ctl)->count) break;
            void* ent = &jo::as_list_box(ctl)->items[first + j - 1];
            void* dest = jo::as_list_box_item(ent)->selected == 3 ? obj(d::g_text_a58ac0) : obj(d::g_font_a489c8);
            const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
            const i32 y = jo::as_list_box(ctl)->y0 + jo::as_list_box(ctl)->text_y + imul(j, jo::as_list_box(ctl)->row_height) + jo::view<i16>(jo::as_list_box(ctl)->style[4]);
            const i32 x = jo::as_list_box(ctl)->x0 + jo::as_list_box(ctl)->text_x + jo::view<i16>(jo::as_list_box(ctl)->style[3]);
            joan::orig::as_text(dest)->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(surf), static_cast<const char*>(ent), joan::orig::arg::u32(color), i32{-1}, i32{0}, i32{0}, i32{0}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}));  // 0x006255d0
        }
    }
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("list_draw_6b49b0", 0x006b49b0, &list_draw);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08_2 {

using i32 = std::int32_t;

using i8 = std::int8_t;

struct H {
    // ------------------------------------------------------------------ 0x006b4dd0
    i32 byte_66c() { return jo::view<i8>(jo::self<jo::Input>(this)->has_mouse_event); }  // `this` is the Input (+0x66c has_mouse_event)
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Obj::byte_66c", 0x006b4dd0, &H::byte_66c);
#undef B95

}  // namespace game_b95_08_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Widget::m_9dd7a0_ctor() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_08::H::widget_9dd7a0_ctor>(this)); }  // 0x6b22f0
auto joan::orig::ComboBox::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_15::H::dtor_6b2340>(this)); }  // 0x6b2340
auto joan::orig::ComboBox::init_options(std::uint32_t a0, std::uint32_t a1, std::int32_t a2, std::uint32_t a3, std::uint32_t a4) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b109_04::H::optbox_init_6b29b0>(this, a0, a1, a2, a3, a4)); }  // 0x6b29b0
auto joan::orig::ComboBox::free_parts() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b102_14::H::free_parts>(this)); }  // 0x6b3130
auto joan::orig::ComboBox::scalar_dtor_6b33a0(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::combobox_scalar_dtor>(this, a0)); }  // 0x6b33a0
auto joan::orig::ComboBox::set_b2(std::uintptr_t a0) -> void { joan::bind::member<&::game_b97_08::H::set_b2>(this, a0); }  // 0x6b33f0
auto joan::orig::ComboBox::scalar_dtor_6b3420(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b101_14::H::scalar_dtor>(this, a0)); }  // 0x6b3420
auto joan::orig::Button::set_callback(std::uintptr_t a0) -> void { joan::bind::member<&::game_b94_08::H::set_33a>(this, a0); }  // 0x6b3900
auto joan::orig::ComboBox::add_6b3930(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_10::H::combo_add>(this, a0)); }  // 0x6b3930
auto joan::orig::ComboBox::remove_from(std::int32_t a0) -> void { joan::bind::member<&::game_b108_18::H::combo_remove_from>(this, a0); }  // 0x6b3b90
auto joan::orig::Popup::name_string_6b3d90(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_07::H::name_string>(this, a0, a1)); }  // 0x6b3d90
auto joan::orig::ComboBox::set_highlight(std::int8_t a0) -> void { joan::bind::member<&::game_b91_04_2::H::set_flag_6b3e50>(this, a0); }  // 0x6b3e50
auto joan::orig::ComboBox::draw(std::uint32_t a0, std::uint8_t a1) -> void { joan::bind::member<&::game_b108_17::H::spell_draw>(this, a0, a1); }  // 0x6b3ec0
auto joan::orig::Menu::step_6b4280() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_08::H::step_6b4280>(this)); }  // 0x6b4280
auto joan::orig::Popup::set_text_6b44e0(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_04::H::set_text_6b44e0>(this, a0)); }  // 0x6b44e0
auto joan::orig::Cursor::flag_66c() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08_2::H::byte_66c>(this)); }  // 0x6b4dd0
