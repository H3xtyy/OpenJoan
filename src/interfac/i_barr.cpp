// interfac/i_barr.cpp: functions of the original d:\ww\interfac\i_barr.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cmath>
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b106_08.cpp (game/b106_08)
//
//    0x006bc530 BarrackInterface::init        (thiscall, ret 0, d:\ww\interfac\i_barr.cpp) -
//               creates the background (0x00629400 / 0x00629660), buttons (0x00620ee0 +
//               0x00621540 / 0x006211d0, 0x0061f1b0), four arrays of six buttons (Mem::add
//               0x0058e7b0) and two text buttons (0x00623ce0); clears +0x3d, +0x42, +0x5a.
//
//  - The operator new + ctor pairs of 0x006bc530 and the local Directory of 0x004c9cd0 have
//    SEH unwind entries only (no throw on the normal path); not reproduced.
namespace {
namespace game_b106_08 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_barr_init, "BarrackInterface::init");

constexpr u32 kP1 = 0x3f800000u;  // 1.0f

// ---------------------------------------------------------------------------
// 0x006bc530 helpers
// ---------------------------------------------------------------------------
using joan::orig::helpers::game_new;  // operator new
// new(0x346) + 0x00620ee0, or 0 when the allocation failed.
void* new_button() {
    void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
    return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(mem)->ctor_620ee0()) : nullptr;  // 0x00620ee0
}
void button_show(void* b) {  // 0x0061f1b0 (0, 1.0f, 0, 1, 0)
    joan::orig::as_widget(b)->scale_to(0, joan::orig::arg::f32(kP1), 0.0f, 1, 0u);
}
u32 colour(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    const u32* const c = joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
    return *c;
}
void** alloc_array(i32 line_offset) {  // Mem::add(0x18, "d:\ww\interfac\i_barr.cpp", line)
    const i32 line = static_cast<i32>(global<i16>(d::g_a232b0)) + line_offset;
    return joan::orig::arg::as<void**>(joan::orig::as_mem(obj(d::g_mem))->add(static_cast<std::uint32_t>(6 * sizeof(void*)) /* 6 button pointers: 0x18 on x86 */, str(d::s_d_ww_interfac_i_barr_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
}

struct H {

    // 0x006bc530
    void barrack_init() {
        c_barr_init.hit();
        jo::self<jo::Barracks>(this)->tex_barr = nullptr;
        jo::self<jo::Barracks>(this)->tex_bg = nullptr;
        jo::self<jo::Barracks>(this)->tex_commele = nullptr;
        {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Image2d)));  // 0xb7 on x86
            jo::view<void*>(jo::self<jo::Barracks>(this)->background) = mem != nullptr ? joan::orig::as_image2d(mem)->construct() : nullptr;  // 0x00629400
        }
        {
            const u32 c = colour(0, 0, 0, 0xff);
            joan::orig::as_label(jo::view<void*>(jo::self<jo::Barracks>(this)->background))->create(0, 0, joan::orig::arg::mptr(obj(d::g_sheet)), str(d::s_bg), joan::orig::arg::i32(c), 3, 0, 2, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);  // 0x00629660
        }
        button_show(jo::view<void*>(jo::self<jo::Barracks>(this)->background));
        jo::view<void*>(jo::self<jo::Barracks>(this)->info_frame) = new_button();
        joan::orig::as_button(jo::view<void*>(jo::self<jo::Barracks>(this)->info_frame))->create(0x400, 0x5b, 0x568, 0x29f, fn::code_value(fn::draw_info_frame_6c1180), 1, 0, 0, 1, 1, 0);  // 0x00621540
        button_show(jo::view<void*>(jo::self<jo::Barracks>(this)->info_frame));
        jo::view<void*>(jo::self<jo::Barracks>(this)->title_frame) = new_button();
        joan::orig::as_button(jo::view<void*>(jo::self<jo::Barracks>(this)->title_frame))->create(0x138, 7, 0x2c4, 0x23, fn::code_value(fn::draw_title_frame_6c1c70), 1, 0, 0, 1, 1, 0);  // 0x00621540
        button_show(jo::view<void*>(jo::self<jo::Barracks>(this)->title_frame));

        jo::view<void**>(jo::self<jo::Barracks>(this)->row_frames) = alloc_array(0x16);
        for (i32 i = 0; i < 6; ++i) {
            jo::view<void**>(jo::self<jo::Barracks>(this)->row_frames)[i] = new_button();
            joan::orig::as_button(jo::view<void**>(jo::self<jo::Barracks>(this)->row_frames)[i])->create(0x24, joan::orig::arg::i32(-100), 0x224, 0, fn::code_value(fn::ov_draw_6c0c00), 1, 0, 0, 1, 1, 0);  // 0x00621540
            button_show(jo::view<void**>(jo::self<jo::Barracks>(this)->row_frames)[i]);
        }
        jo::view<void**>(jo::self<jo::Barracks>(this)->row_icons) = alloc_array(0x22);
        for (i32 i = 0; i < 6; ++i) {
            jo::view<void**>(jo::self<jo::Barracks>(this)->row_icons)[i] = new_button();
            joan::orig::as_menu_button(jo::view<void**>(jo::self<jo::Barracks>(this)->row_icons)[i])->setup(joan::orig::arg::i16(0x1d8), joan::orig::arg::i16(i * 0x64 + 0x68), joan::orig::arg::mptr(obj(d::g_sheet)), str(d::s_ia_ov), str(d::s_ia_ov), str(d::s_ia_ov), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x006211d0 "ia_ov"
            button_show(jo::view<void**>(jo::self<jo::Barracks>(this)->row_icons)[i]);
        }
        jo::view<void**>(jo::self<jo::Barracks>(this)->add_buttons) = alloc_array(0x2b);
        for (i32 i = 0; i < 6; ++i) {
            jo::view<void**>(jo::self<jo::Barracks>(this)->add_buttons)[i] = new_button();
            joan::orig::as_menu_button(jo::view<void**>(jo::self<jo::Barracks>(this)->add_buttons)[i])->setup(joan::orig::arg::i16(0x1ec), joan::orig::arg::i16(i * 0x64 + 0x7c), joan::orig::arg::mptr(obj(d::g_bmp_commele2)), str(d::s_aw_r_ot), str(d::s_aw_r_dw), str(d::s_aw_r_ov), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x006211d0 aw_r_ot/dw/ov
            button_show(jo::view<void**>(jo::self<jo::Barracks>(this)->add_buttons)[i]);
        }
        jo::view<void**>(jo::self<jo::Barracks>(this)->remove_buttons) = alloc_array(0x34);
        for (i32 i = 0; i < 6; ++i) {
            jo::view<void**>(jo::self<jo::Barracks>(this)->remove_buttons)[i] = new_button();
            joan::orig::as_menu_button(jo::view<void**>(jo::self<jo::Barracks>(this)->remove_buttons)[i])->setup(joan::orig::arg::i16(0x1ec), joan::orig::arg::i16(i * 0x64 + 0x9c), joan::orig::arg::mptr(obj(d::g_bmp_commele2)), str(d::s_aw_l_ot), str(d::s_aw_l_dw), str(d::s_aw_l_ov), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x006211d0 aw_l_ot/dw/ov
            button_show(jo::view<void**>(jo::self<jo::Barracks>(this)->remove_buttons)[i]);
        }

        jo::view<void*>(jo::self<jo::Barracks>(this)->ok_button) = new_button();
        joan::orig::as_button(jo::view<void*>(jo::self<jo::Barracks>(this)->ok_button))->create(0x28b, 0x2be, 0x327, 0x2ea, fn::code_value(fn::label_6c0570), 1, 0, 0, 1, 1, 1);  // 0x00621540
        button_show(jo::view<void*>(jo::self<jo::Barracks>(this)->ok_button));
        {
            const u32 c = colour(0xff, 0xff, 0xff, 0xff);
            void* const text = joan::orig::arg::as<void*>(joan::orig::as_barrack(obj(d::g_strings))->open_1());  // 0x004dda00
            joan::orig::as_button(jo::view<void*>(jo::self<jo::Barracks>(this)->ok_button))->set_text(joan::orig::arg::str(text), obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(c), 1, 2, 0, 2, 2);  // 0x00623ce0
        }
        jo::view<void*>(jo::self<jo::Barracks>(this)->cancel_button) = new_button();
        joan::orig::as_button(jo::view<void*>(jo::self<jo::Barracks>(this)->cancel_button))->create(0x32e, 0x2be, 0x3ca, 0x2ea, fn::code_value(fn::label_6c0570), 1, 0, 0, 1, 1, 1);  // 0x00621540
        button_show(jo::view<void*>(jo::self<jo::Barracks>(this)->cancel_button));
        {
            const u32 c = colour(0xff, 0xff, 0xff, 0xff);
            void* const text = joan::orig::arg::as<void*>(joan::orig::as_barrack(obj(d::g_strings))->open_2());  // 0x004dda30
            joan::orig::as_button(jo::view<void*>(jo::self<jo::Barracks>(this)->cancel_button))->set_text(joan::orig::arg::str(text), obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(c), 1, 2, 0, 2, 2);  // 0x00623ce0
        }
        jo::self<jo::Barracks>(this)->u8_3d = 0;
        std::memset(jo::self<jo::Barracks>(this)->type_counts, 0, 0x18u);                        // memset
        std::memset(jo::self<jo::Barracks>(this)->orders, 0, 0x18u);                        // memset
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_08", name, addr, fn)
B("BarrackInterface::init", 0x006bc530, &H::barrack_init);
#undef B

}  // namespace game_b106_08
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
//
//    - Barracks::release 0x006bcf00 (interfac/i_barr.cpp) -> this: 0x6c04d0, frees +0x24
//      (0x629830 + delete 0x6bd440), +0x28/+0x2c, arrays +0xc..+0x18 (6 each, 0x6217e0 +
//      delete 0x6b33a0, then 0x58ed70 free with the file / line [0xa232b4] + 0x22 ..), +0x1c,
//      +0x20; byte +0x3d = 0, word +0x31 = -1.
//
//    - Workshop::release 0x006e9a50 (interfac/i_work.cpp) -> this: like 0x6bcf00 with 0x6ed5b0,
//      singles +0x28 (0x629830 / 0x6bd440), +0x2c, +0x30, arrays +0xc..+0x18 of 4 (line
//      [0xa264e8] + ..), singles +0x1c/+0x20/+0x24; dword +0x4d = 0, word +0x35 = -1.
namespace {
namespace game_b103_15 {

using u16 = std::uint16_t;

JOAN_COUNTER(c_barr_release, "Barracks::release");

// ---- release helpers (0x6bcf00 / 0x6e9a50) ----
// Single member: pre-call, scalar delete (flags 1), clear.
template <class T>
void release_one(T*& slot, std::uintptr_t pre, std::uintptr_t del) {
    if (slot == nullptr) return;
    thiscall<u32>(pre, static_cast<void*>(slot));
    void* const q = slot;
    if (q != nullptr) thiscall<void*>(del, q, 1);
    slot = nullptr;
}
// Array member of n pointers: 0x6217e0 + delete 0x6b33a0 on each, then 0x58ed70 free.
void release_arr(jo::ButtonPtrArray*& slot, i32 n, img::Addr line_global, i32 line_add,
                 img::Addr file) {
    if (slot == nullptr) return;
    for (i32 i = 0; i < n; ++i) {
        joan::orig::as_menu_button(slot->item[i])->clear();  // 0x006217e0
        void* const q = slot->item[i];
        if (q != nullptr) joan::orig::as_combo_box(q)->scalar_dtor_6b33a0(1u);  // 0x006b33a0
        slot->item[i] = nullptr;
    }
    const i32 line = static_cast<i32>(global<i16>(line_global)) + line_add;
    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(slot), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
    slot = nullptr;
}

struct H {
    // ------------------------------------------------------------------------ 0x006bcf00
    void* barr_release() {
        c_barr_release.hit();
        joan::orig::as_barracks_screen(this)->free_sprites();  // 0x006c04d0
        release_one(jo::self<jo::Barracks>(this)->background, fn::code_value(fn::Image2d_release), fn::code_value(fn::Sprite_scalar_dtor));  // 0x00629830 / 0x006bd440
        release_one(jo::self<jo::Barracks>(this)->info_frame, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));  // 0x006217e0 / 0x006b33a0
        release_one(jo::self<jo::Barracks>(this)->title_frame, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        release_arr(jo::self<jo::Barracks>(this)->row_frames, 6, d::g_a232b4, 0x22, d::s_d_ww_interfac_i_barr_cpp);
        release_arr(jo::self<jo::Barracks>(this)->row_icons, 6, d::g_a232b4, 0x2e, d::s_d_ww_interfac_i_barr_cpp);
        release_arr(jo::self<jo::Barracks>(this)->add_buttons, 6, d::g_a232b4, 0x3a, d::s_d_ww_interfac_i_barr_cpp);
        release_arr(jo::self<jo::Barracks>(this)->remove_buttons, 6, d::g_a232b4, 0x46, d::s_d_ww_interfac_i_barr_cpp);
        release_one(jo::self<jo::Barracks>(this)->ok_button, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        release_one(jo::self<jo::Barracks>(this)->cancel_button, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        jo::self<jo::Barracks>(this)->u8_3d = 0;
        jo::view<u16>(jo::self<jo::Barracks>(this)->hover_row) = 0xffff;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Barracks::release", 0x006bcf00, &H::barr_release);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
//
//      0x006bd440 Sprite::scalar_dtor       (thiscall(flags), ret 4)
namespace {
namespace game_b94_08 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::orig::helpers::delete_;                                // operator delete

struct H {
    H* sprite_scalar_dtor(u32 flags) {  // 0x006bd440
        joan::orig::as_widget(this)->dtor_6294c0();  // 0x006294c0
        if ((flags & 1) != 0) delete_(this);
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Sprite::scalar_dtor", 0x006bd440, &H::sprite_scalar_dtor);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
//
//      0x006c04d0 Res3::free            (thiscall, ret 0)
namespace {
namespace game_b93_06 {

using joan::img::obj;

using i32 = std::int32_t;

void* const kRes = obj(d::g_texture_array);

void res_free(void* handle) { joan::orig::as_texture_manager(kRes)->release(handle); }  // 0x0060a550

struct H {
    void free3() {  // 0x006c04d0
        if (jo::self<jo::Barracks>(this)->tex_barr != nullptr) {
            res_free(jo::self<jo::Barracks>(this)->tex_barr);
            jo::self<jo::Barracks>(this)->tex_barr = nullptr;
        }
        if (jo::self<jo::Barracks>(this)->tex_bg != nullptr) {
            res_free(jo::self<jo::Barracks>(this)->tex_bg);
            jo::self<jo::Barracks>(this)->tex_bg = nullptr;
        }
        if (jo::self<jo::Barracks>(this)->tex_commele != nullptr) {
            res_free(jo::self<jo::Barracks>(this)->tex_commele);
            jo::self<jo::Barracks>(this)->tex_commele = nullptr;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Res3::free", 0x006c04d0, &H::free3);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::crt_memset;

struct H {
    H* clear_82() {  // 0x006bc4f0
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::Barracks)));  // 0x82 on x86 (g_a48920, twelve pointers)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::clear_82", 0x006bc4f0, &H::clear_82);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

using joan::hook::global;
using joan::hook::x87::ftol;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::img::obj;
using joan::orig::helpers::addr;
using joan::orig::helpers::memset_;

using joan::hook::x87::Fpu;

JOAN_COUNTER(c_refresh_6bda00, "Unit::refresh_6bda00");

// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

struct H {

    void nop0() {}

    // 0x006bda00
    std::uintptr_t refresh_6bda00() {
        if (static_cast<i32>(jo::view<signed char>(jo::self<jo::Barracks>(this)->u8_3d)) != 0) return addr(this);
        c_refresh_6bda00.hit();
        const void* found = nullptr;  // the army (a pointer; only tested for null)
        if (joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->army))) == 0)  // 0x00428b70
            found = joan::orig::arg::as<const void*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->army)));  // 0x0041b3c0
        const u32 busy = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->place)));  // 0x0044cf80
        if (busy != 0) return busy;
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->place)));  // 0x0043c450
        void* const p = virtual_call<void*>(o, 0x24);
        jo::view<u32>(jo::self<jo::Barracks>(this)->gold) = global<u32>(d::g_get_a596c4);
        void* const q = jo::as_facility(p)->owner;
        const i32 a = joan::orig::arg::as<i32>(joan::orig::as_place(q)->recruit_pool());  // 0x00469360
        const i32 b = joan::orig::arg::as<i32>(joan::orig::as_place(q)->recruit_cap());  // 0x004693b0
        const Fpu fpu{true};
        const double c = global<double>(d::dbl_0_95);  // 0.95 (qword)
        jo::self<jo::Barracks>(this)->pool = ftol(fpu.sub(static_cast<double>(a), fpu.mul(static_cast<double>(b), c)));
        if (jo::self<jo::Barracks>(this)->pool < 0) jo::self<jo::Barracks>(this)->pool = 0;
        if (jo::self<jo::Barracks>(this)->pool > 0x14) jo::self<jo::Barracks>(this)->pool = 0x14;
        jo::view<u32>(jo::self<jo::Barracks>(this)->recruited) = 0;
        jo::view<u32>(jo::self<jo::Barracks>(this)->cost) = 0;
        if (found != nullptr) {
            joan::orig::as_stats(this)->count_unit(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->army));  // 0x006bd4b0
            jo::view<u32>(jo::self<jo::Barracks>(this)->army_size) = jo::self<jo::Barracks>(this)->type_counts[0] + jo::self<jo::Barracks>(this)->type_counts[1] + jo::self<jo::Barracks>(this)->type_counts[2] + jo::self<jo::Barracks>(this)->type_counts[3] + jo::self<jo::Barracks>(this)->type_counts[4] +
                           jo::self<jo::Barracks>(this)->type_counts[5];
        } else {
            memset_(jo::self<jo::Barracks>(this)->type_counts, 0, 0x18);
            memset_(jo::self<jo::Barracks>(this)->orders, 0, 0x18);
            jo::view<u32>(jo::self<jo::Barracks>(this)->army_size) = 0;
        }
        jo::self<jo::Barracks>(this)->u8_3d = 1;
        return addr(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("init_6c2010", 0x006c2010, &static_init<fn::Globals_color_a6a54c>);
B("Obj::nop_6bd490", 0x006bd490, &H::nop0);
B("Unit::refresh_6bda00", 0x006bda00, &H::refresh_6bda00);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b104_13.cpp (game/b104_13)
namespace {
namespace game_b104_13 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

// thiscall through the object's vtable slot `off`.
template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kPlaces = obj(d::g_places);

JOAN_COUNTER(c_count_unit, "Stats::count_unit");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* type_get(i16 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(w)))); }  // 0x0042d0a0
u8* place_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_list(kPlaces)->get_e(joan::orig::arg::i32(id))); }    // 0x0043c450
i32 list_count(void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0

struct H {

    // ------------------------------------------------------------------ 0x006bd4b0
    // Jump tables 0x6bd89c / 0x6bd8b4 / 0x6bd8cc: type-1 index 0..5 -> counter offset.
    // `this` is the Barracks (0x006bda00 refresh calls this on itself and sums type_counts).
    static i32 counter(i32 k) {
        static const i32 kIdx[6] = {0, 1, 3, 2, 4, 5};  // type_counts index (offsets 0x42 0x46 0x4e 0x4a 0x52 0x56)
        return kIdx[k];
    }
    i32 count_unit(i32 id) {
        if (unit_missing(id) != 0) return 0;
        c_count_unit.hit();
        u8* const u = unit_get(id);
        std::memset(static_cast<void*>(jo::self<jo::Barracks>(this)->type_counts), 0, 0x18u);  // memset
        std::memset(static_cast<void*>(jo::self<jo::Barracks>(this)->orders), 0, 0x18u);       // memset
        for (i32 i = 0; i < static_cast<i32>(jo::as_unit(u)->group->count); ++i) {
            const i32 mid = static_cast<i32>(jo::as_unit(u)->group->members[i]);
            if (unit_missing(mid) != 0) continue;
            u8* const m = unit_get(static_cast<i32>(jo::as_unit(u)->group->members[i]));
            const i32 k = static_cast<i32>(jo::as_unit_type(type_get(jo::as_unit(m)->type_id))->unit_class) - 1;
            if (static_cast<u32>(k) <= 5) jo::view<i32>(jo::self<jo::Barracks>(this)->type_counts[counter(k)]) += 1;
        }
        u8* const pl = place_get(jo::self<jo::Barracks>(this)->place);  // `this` is the Barracks (Stats view drifts on x64)
        u8* const rec = vcall<u8*>(pl, 0x24);
        u8* const pl2 = place_get(static_cast<i32>(jo::as_flag_facility(rec)->i16_91));
        u8* const d = vcall<u8*>(pl2, 0x18);
        for (i32 i = 0; i < static_cast<i32>(jo::as_building(d)->queue_n); ++i) {
            if (static_cast<i32>(jo::as_building(d)->unit_level) != static_cast<i32>(jo::as_unit(u)->group_id)) break;
            const i32 k = static_cast<i32>(jo::as_unit_type(type_get(jo::as_building(d)->queue_kind[i]))->unit_class) - 1;
            if (static_cast<u32>(k) <= 5) {
                jo::view<i32>(jo::self<jo::Barracks>(this)->type_counts[counter(k)]) = static_cast<i32>(static_cast<u32>(jo::view<i32>(jo::self<jo::Barracks>(this)->type_counts[counter(k)])) +
                                                         static_cast<u32>(static_cast<i32>(jo::as_building(d)->queue_count[i])));
            }
        }
        for (i32 i = 1; i <= list_count(jo::as_flag_facility(rec)->list_89); ++i) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::as_flag_facility(rec)->list_89)->get(joan::orig::arg::i32(i)));  // 0x00417200
            if (jo::view<i32>(jo::as_recruit_row(e)->value) != static_cast<i32>(jo::as_unit(u)->group_id)) break;
            const i32 k = static_cast<i32>(jo::as_unit_type(type_get(jo::as_recruit_row(e)->type))->unit_class) - 1;
            if (static_cast<u32>(k) <= 5) {
                jo::view<i32>(jo::self<jo::Barracks>(this)->type_counts[counter(k)]) =
                    static_cast<i32>(static_cast<u32>(jo::view<i32>(jo::self<jo::Barracks>(this)->type_counts[counter(k)])) + static_cast<u32>(jo::as_recruit_row(e)->count));
            }
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_13", name, addr, fn)
B("Stats::count_unit", 0x006bd4b0, &H::count_unit);
#undef B

}  // namespace game_b104_13
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::img::obj;
using joan::orig::helpers::addr;

using i16 = std::int16_t;

using joan::img::str;

JOAN_COUNTER(c_commit_units, "BuyScreen::commit_units");

struct H {
    // ------------------------------------------------------------------ 0x006bdc00
    std::uintptr_t commit_units() {
        c_commit_units.hit();
        void* const army_list = obj(d::g_units);
        void* const units = obj(d::g_places);
        u8* rec = nullptr;
        if (joan::orig::as_dynarray(army_list)->empty_at(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->army)) == 0)  // 0x00428b70
            rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(army_list)->ptr_at(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->army)));  // 0x0041b3c0
        const u32 busy = joan::orig::arg::as<u32>(joan::orig::as_list(units)->empty_at_e(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->place)));  // 0x0044cf80
        if (busy != 0) return busy;
        void* const o1 = joan::orig::arg::as<void*>(joan::orig::as_list(units)->get_e(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->place)));  // 0x0043c450
        u8* const army = virtual_call<u8*>(o1, 0x24);
        void* const o2 = joan::orig::arg::as<void*>(joan::orig::as_list(units)->get_e(static_cast<i32>(jo::as_flag_facility(army)->i16_91)));
        virtual_call<u32>(o2, 0x18);  // result unused
        static const img::Addr kNames[6] = {
            d::s_FRENCH_LIGHT_INFANTRY,  // "FRENCH_LIGHT_INFANTRY"
            d::s_FRENCH_HEAVY_INFANTRY,  // "FRENCH_HEAVY_INFANTRY"
            d::s_FRENCH_MACEMAN,  // "FRENCH_MACEMAN"
            d::s_FRENCH_POLEMAN,  // "FRENCH_POLEMAN"
            d::s_FRENCH_ARCHER,  // "FRENCH_ARCHER"
            d::s_FRENCH_CROSSBOW_MAN,  // "FRENCH_CROSSBOW_MAN"
        };
        for (i32 i = 0; i < 6; ++i) {
            if (jo::self<jo::Barracks>(this)->orders[i] == 0) continue;
            const u32 type = joan::orig::arg::as<u32>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(str(kNames[i]))));  // 0x005270b0
            const i32 type_w = static_cast<i32>(static_cast<i16>(type));
            if (rec != nullptr)
                joan::orig::as_recruit_facility(army)->add_order(joan::orig::arg::i16(type_w), joan::orig::arg::u32(static_cast<i32>(jo::as_unit(rec)->group_id)), joan::orig::arg::u32(// 0x00468cd0
                              jo::self<jo::Barracks>(this)->orders[i]));
            else
                joan::orig::as_recruit_facility(army)->add_order(joan::orig::arg::i16(type_w), 0x270fu, joan::orig::arg::u32(jo::self<jo::Barracks>(this)->orders[i]));
        }
        jo::self<jo::Barracks>(this)->gold = jo::self<jo::Barracks>(this)->gold - jo::self<jo::Barracks>(this)->cost;
        global<i32>(d::g_get_a596c4) = jo::self<jo::Barracks>(this)->gold;
        const i32 v = joan::orig::arg::as<i32>(joan::orig::as_place(jo::view<void*>(jo::as_facility(army)->owner))->recruit_pool());  // 0x00469360
        joan::orig::as_unit(jo::view<void*>(jo::as_facility(army)->owner))->set_52(joan::orig::arg::i32(v - jo::self<jo::Barracks>(this)->recruited));  // 0x00469330
        jo::view<u32>(jo::self<jo::Barracks>(this)->recruited) = 0;
        jo::view<u32>(jo::self<jo::Barracks>(this)->cost) = 0;
        return addr(this);  // eax = this (mov eax,[ebp-4] before the last store)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("BuyScreen::commit_units", 0x006bdc00, &H::commit_units);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_ui5.cpp (game/ui5)
namespace {
namespace game_ui5 {

using joan::hook::global;
using joan::hook::x87::ftol;
using i32 = std::int32_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using i16 = std::int16_t;

using joan::img::str;

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

// `mov eax, [obj]; mov ecx, obj; call [eax + index * 4]`: thiscall without arguments.
template <class R = void>
R virtual0(void* object, std::int32_t index) {
    using Fn = R (Obj::*)();
    const std::uintptr_t fn = reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(object)[index]);
    return (static_cast<Obj*>(object)->*joan::hook::original<Fn>(fn))();
}

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue
constexpr img::Addr kStrings = d::g_strings;  // game strings
constexpr img::Addr kTypes = d::g_types;    // unit type table
constexpr img::Addr kText = d::g_font_goudy14_22_white;     // text renderer

void strcpy_(char* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

// 0x0042a260 Color(r, g, b, a) on a 4-byte local, returned dword copied out.
std::uint32_t color(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    std::uint32_t* const p = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
    return *p;
}
// fld dword [0x009cfb68] (255.0f); fmul dword [o + 0x11]; call _ftol.
std::uint32_t alpha(const void* o) {
    const joan::hook::x87::Fpu fpu{true};
    return static_cast<std::uint32_t>(joan::hook::x87::ftol(
        fpu.mul(static_cast<double>(global<float>(d::flt_255)), static_cast<double>(jo::as_button(o)->scale))));
}
// 0x00622cc0 Button::draw(mode, flags), ret 8.
void button(void* b, std::uint32_t mode, std::uint32_t flags) { joan::orig::as_button(b)->draw(joan::orig::arg::u32(mode), static_cast<std::uint8_t>(flags)); }  // 0x00622cc0

// 0x00633500 SpriteQueue::draw_named(x, y, a3, name, sheet, colour, a7 .. a17), ret 0x44.
void sprite(std::int32_t x, std::int32_t y, std::int32_t a3, const char* name, void* sheet, std::uint32_t c,
            std::int32_t a9, std::int32_t a16, std::int32_t a17) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), sheet, joan::orig::arg::u32(c), 3, 0, joan::orig::arg::i16(a9), 0, -1, -1, -1, -1, 0, joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));  // 0x00633500
}
// The panel sprites: (x, y, 3, name, sheet, colour, 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1).
void icon(std::int32_t x, std::int32_t y, const char* name, img::Addr sheet, std::uint32_t c) {
    sprite(x, y, 3, name, img::obj(sheet), c, 2, -1, -1);
}
// 0x006255d0 Text::draw_line(x, y, size, text, colour, -1, 0, 1, 2, 0, 1.0f, 1.0f), ret 0x30.
void text_line(std::int32_t x, std::int32_t y, const char* s, std::uint32_t c) {
    joan::orig::as_text(obj(kText))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), 3u, s, joan::orig::arg::u32(c), joan::orig::arg::i32(-1), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x006255d0
}
// 0x00626320 Text::draw(x1, y1, x2, y2, 3, text, colour, 2, 1, 1, 1, 2, 0, 1.0f, 1.0f, 0, -1), ret 0x44.
void text_box(img::Addr renderer, std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2,
              const char* s, std::uint32_t c) {
    joan::orig::as_text(obj(renderer))->draw(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), 3u, s, joan::orig::arg::u32(c), 2, 1, 1, 1, 2, 0, 1.0f, 1.0f, 0, joan::orig::arg::i32(-1));  // 0x00626320
}
const char* str_units() { return joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(kStrings))->state3()); }  // 0x004dda60

// The panel's 6 unit types: 0x005270b0 names, icon names (0x10-byte buffer) and the summary icons.
const img::Addr kTypeNames[6] = {d::s_FRENCH_LIGHT_INFANTRY, d::s_FRENCH_HEAVY_INFANTRY, d::s_FRENCH_MACEMAN, d::s_FRENCH_POLEMAN, d::s_FRENCH_ARCHER, d::s_FRENCH_CROSSBOW_MAN};
const img::Addr kTypeIcons[6] = {d::s_FLINF, d::s_FHINF, d::s_FMACE, d::s_FPOLE, d::s_F_ARCH, d::s_FCROSS};
const img::Addr kCurrent[6] = {d::s_c_linf, d::s_c_hinf, d::s_c_mace, d::s_c_pole, d::s_c_arch, d::s_c_cross};  // c_*
const img::Addr kNew[6] = {d::s_n_linf, d::s_n_hinf, d::s_n_mace, d::s_n_pole, d::s_n_arch, d::s_n_cross};      // n_*

JOAN_COUNTER(c_army, "ArmyPanel::draw");

struct H {
    void* unit_button(jo::ButtonPtrArray* jo::Barracks::*array, std::int32_t i) { return (jo::self<jo::Barracks>(this)->*array)->item[i]; }
    void* box() { return jo::view<void*>(jo::self<jo::Barracks>(this)->info_frame); }
    std::int32_t box_x() { return static_cast<std::int32_t>(jo::as_button(box())->x); }
    std::int32_t box_y() { return static_cast<std::int32_t>(jo::as_button(box())->y); }

    // 0x006bdea0
    void army_draw() {
        joan::canvas::ScreenScope canvas_scope(0x006bdea0);  // canvas: laid out for 1024x768 (canvas_api.hpp)
        c_army.hit();
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Barracks>(this)->open)) == 0) return;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->place))) != 0) return;  // 0x0044cf80
        void* const army =
            virtual0<void*>(joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->place))), 9);  // 0x0043c450, +0x24
        // [ebp-0x11c] (type icon name) is only written on a type match: when the first listed
        // unit is not one of the six French types the original draws an uninitialised name.
        // Run the original body (left intact behind the redirected thunk) in that case. The
        // probe only uses pure getters (0x00417ad0 / 0x00417200 / 0x005270b0; 0x0043c450 and
        // the +0x24 virtual (0x00469310, returns this) are also repeated by the original).
        if (joan::orig::as_dynarray(jo::as_flag_facility(army)->list_79)->size() > 0) {  // 0x00417ad0
            const std::int16_t* const first = joan::orig::arg::as<std::int16_t*>(joan::orig::as_dynarray(jo::as_flag_facility(army)->list_79)->get(1));  // 0x00417200
            bool known = false;
            for (std::int32_t t = 0; t < 6 && !known; ++t)
                known = static_cast<std::int32_t>(*first) ==
                        joan::orig::arg::as<std::int32_t>(joan::orig::as_unit_types(obj(kTypes))->find(joan::orig::arg::str(str(kTypeNames[t]))));  // 0x005270b0
            if (!known && !fix_uninit()) {  // batch 118/03: fix/uninit runs the C++ version below
                thiscall(fn::ArmyPanel_draw_body, this);  // original body
                return;
            }
        }
        joan::orig::as_label(jo::view<void*>(jo::self<jo::Barracks>(this)->background))->draw(3u, 7u);  // 0x00629bd0
        button(jo::view<void*>(jo::self<jo::Barracks>(this)->title_frame), 3, 7);
        button(jo::view<void*>(jo::self<jo::Barracks>(this)->ok_button), 3, 7);
        button(jo::view<void*>(jo::self<jo::Barracks>(this)->cancel_button), 3, 7);

        char text[0x100];
        // [ebp-0x11c]: only written on a type match, kept from the previous unit otherwise (the
        // first unit always matches here, see the probe above, unless fix/uninit is enabled:
        // then a first unit of another type draws the empty name "" - 0x00633500 finds no
        // sprite for it and queues nothing - and later ones keep the previous name as before).
        char type_icon[0x10] = {};
        const std::int32_t count = joan::orig::as_dynarray(jo::as_flag_facility(army)->list_79)->size();  // 0x00417ad0
        for (std::int32_t i = 0; i < count; ++i) {
            button(unit_button(&jo::Barracks::row_icons, i), 3, 7);
            button(unit_button(&jo::Barracks::add_buttons, i), 3, 7);
            button(unit_button(&jo::Barracks::remove_buttons, i), 3, 7);
            button(unit_button(&jo::Barracks::row_frames, i), 3, 7);
            std::int16_t* const unit = joan::orig::arg::as<std::int16_t*>(joan::orig::as_dynarray(jo::as_flag_facility(army)->list_79)->get(joan::orig::arg::i32(i + 1)));  // 0x00417200
            for (std::int32_t t = 0; t < 6; ++t) {
                const std::int32_t id = static_cast<std::int32_t>(*unit);
                if (id == joan::orig::arg::as<std::int32_t>(joan::orig::as_unit_types(obj(kTypes))->find(joan::orig::arg::str(str(kTypeNames[t]))))) {  // 0x005270b0
                    strcpy_(type_icon, str(kTypeIcons[t]));
                    break;
                }
            }
            const std::int32_t id = static_cast<std::int32_t>(*unit);
            std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(obj(kTypes))->get(joan::orig::arg::i16(id)));  // 0x0042d0a0 (word)
            const char* const units = str_units();
            joan::orig::libc::sprintf(joan::orig::arg::str(text), str(d::s_s_d_s), jo::as_unit_type(def)->title_b4, jo::as_unit_type(def)->cost, units);  // sprintf '%s (%d %s)'
            {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(unit_button(&jo::Barracks::row_frames, i)));
                const std::int32_t y = static_cast<std::int32_t>(jo::as_button(unit_button(&jo::Barracks::row_frames, i))->y) + 0x10;
                const std::int32_t x = static_cast<std::int32_t>(jo::as_button(unit_button(&jo::Barracks::row_frames, i))->x) + 0x60;
                text_line(x, y, text, c);
            }
            std::int32_t x = static_cast<std::int32_t>(jo::as_button(unit_button(&jo::Barracks::row_frames, i))->x) + 0x60;
            std::int32_t y = static_cast<std::int32_t>(jo::as_button(unit_button(&jo::Barracks::row_frames, i))->y) + 0x40;
            for (std::int32_t k = 1; k <= static_cast<std::int32_t>(jo::as_unit_type(def)->strength); k += 2) {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(unit_button(&jo::Barracks::row_frames, i)));
                icon(x, y, str(d::s_att_r), d::g_sheet, c);  // 'att_r'
                x += 0x12;
            }
            x = static_cast<std::int32_t>(jo::as_button(unit_button(&jo::Barracks::row_frames, i))->x) + 0xc8;
            for (std::int32_t k = 1; k <= static_cast<std::int32_t>(jo::as_unit_type(def)->defense); k += 2) {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(unit_button(&jo::Barracks::row_frames, i)));
                icon(x, y, str(d::s_att_y), d::g_sheet, c);  // 'att_y'
                x += 0x12;
            }
            x = static_cast<std::int32_t>(jo::as_button(unit_button(&jo::Barracks::row_frames, i))->x) + 0x130;
            for (std::int32_t k = 1; k <= static_cast<std::int32_t>(jo::as_unit_type(def)->dexterity); k += 2) {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(unit_button(&jo::Barracks::row_frames, i)));
                icon(x, y, str(d::s_att_b), d::g_sheet, c);  // 'att_b'
                x += 0x12;
            }
            x = static_cast<std::int32_t>(jo::as_button(unit_button(&jo::Barracks::row_frames, i))->x) + 0x198;
            const char* const kind = static_cast<std::int32_t>(jo::as_unit_type(def)->item_stack) > 0
                                         ? joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(kStrings))->state5())   // 0x004ddac0
                                         : joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(kStrings))->state4());  // 0x004dda90
            {
                const std::uint32_t c = color(0, 0, 0, alpha(unit_button(&jo::Barracks::row_frames, i)));
                text_line(x, y, kind, c);
            }
            x = static_cast<std::int32_t>(jo::as_button(unit_button(&jo::Barracks::row_frames, i))->x) + 0x10;
            y = static_cast<std::int32_t>(jo::as_button(unit_button(&jo::Barracks::row_frames, i))->y) + 0x10;
            {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(unit_button(&jo::Barracks::row_frames, i)));
                icon(x, y, str(d::s_ICON_1), d::g_icons, c);  // 'ICON_1'
            }
            {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(unit_button(&jo::Barracks::row_frames, i)));
                icon(x, y, type_icon, d::g_icons, c);
            }
            {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(unit_button(&jo::Barracks::row_frames, i)));
                icon(x, y, str(d::s_ICON_2), d::g_icons, c);  // 'ICON_2'
            }
        }

        button(box(), 3, 7);
        void* const leader =
            jo::view<void*>(jo::as_facility(virtual0<std::uint8_t*>(joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->place))), 9))->owner);  // 0x0043c450, +0x24
        {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            const char* const name = jo::as_recruit_entry(joan::orig::as_recruit_res(obj(d::g_recruit_res))->group_at(joan::orig::arg::u32(static_cast<std::int32_t>(static_cast<std::int16_t>(jo::as_flag_facility(army)->group_a)))))->location;  // 0x00469520 (word); +0x2b on x86, after the rows pointer
            text_box(kText, box_x() + 0x16, box_y() + 0x2a, box_x() + 0x152, box_y() + 0x5c, name, c);
        }
        {
            const char* const fmt = joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(kStrings))->state6(joan::orig::arg::u32(jo::as_place(leader)->counter)));  // 0x004ddaf0
            joan::orig::libc::sprintf(joan::orig::arg::str(text), joan::orig::arg::str(fmt));  // sprintf(text, fmt)
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            text_box(d::g_font_goudy12_bold_black, box_x() + 0x16, box_y() + 0x5e, box_x() + 0x152, box_y() + 0x72, text, c);
        }
        {
            const char* const units = str_units();
            joan::orig::libc::sprintf(joan::orig::arg::str(text), str(d::s_d_s), jo::self<jo::Barracks>(this)->cost, units);  // '%d %s'
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            text_box(kText, box_x() + 0x16, box_y() + 0x1c2, box_x() + 0x152, box_y() + 0x1de, text, c);
        }
        {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            icon(box_x() + 0x100, box_y() + 0x19c, str(d::s_gold), d::g_sheet, c);  // 'gold'
        }
        {
            const char* const units = str_units();
            joan::orig::libc::sprintf(joan::orig::arg::str(text), str(d::s_d_s), jo::self<jo::Barracks>(this)->gold, units);  // '%d %s'
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            text_box(kText, box_x() + 0x16, box_y() + 0x1fe, box_x() + 0x152, box_y() + 0x21a, text, c);
        }
        {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            icon(box_x() + 0x100, box_y() + 0x1d8, str(d::s_gold), d::g_sheet, c);  // 'gold'
        }

        std::int32_t x = box_x() + 0x16;
        std::int32_t y = box_y() + 0x96;
        for (std::int32_t i = 0; i < jo::self<jo::Barracks>(this)->pool; ++i) {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            icon(x, y, str(d::s_CITIZEN), d::g_sheet, c);  // 'CITIZEN'
            x += 0x1e;
            if (i == 9) {
                x = box_x() + 0x16;
                y = box_y() + 0xbe;
            }
        }

        x = box_x() + 0x16;
        y = box_y() + 0x13b;
        std::int32_t n = 0;
        for (std::int32_t pass = 0; pass < 2; ++pass) {
            const img::Addr* const names = pass == 0 ? kCurrent : kNew;
            const std::uint32_t* const counts = pass == 0 ? jo::self<jo::Barracks>(this)->type_counts : jo::self<jo::Barracks>(this)->orders;
            for (std::int32_t t = 0; t < 6; ++t) {
                strcpy_(text, str(names[t]));
                for (std::int32_t j = 0; j < static_cast<std::int32_t>(counts[t]); ++j) {
                    const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
                    icon(x, y, text, d::g_sheet, c);
                    x += 0x1e;
                    if (n == 9) {
                        x = box_x() + 0x16;
                        y = box_y() + 0x163;
                    }
                    ++n;
                }
            }
        }
    }
};

#define UI5(name, addr, fn) JOAN_REPLACE("game/ui5", name, addr, fn)
UI5("ArmyPanel::draw", 0x006bdea0, &H::army_draw);
JOAN_CANVAS_SELF(0x006bdea0);  // ScreenScope in the body
#undef UI5

}  // namespace game_ui5
}  // namespace

// ---------------------------------------- from modules/game_b107_20.cpp (game/b107_20)
namespace {
namespace game_b107_20 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

JOAN_COUNTER(c_recruit, "RecruitPanel::update_6bf630");

struct H {
    // ---- 0x006bf630 helpers ----
    void* row(jo::ButtonPtrArray* jo::Barracks::*table, i32 i) { return (jo::self<jo::Barracks>(this)->*table)->item[i]; }
    void setup_row(i32 i, u32 id_first, u32 id_rest, u32 flag) {
        const u32 base = static_cast<u32>(i) * 100u;
        joan::orig::as_widget(row(&jo::Barracks::row_icons, i))->move_to(joan::orig::arg::i16(id_first), joan::orig::arg::i16(base + 0x68u), joan::orig::arg::f32(0x43960000u), joan::orig::arg::f32(0xc3480000u), 0, 0u);  // 0x0061e020
        joan::orig::as_widget(row(&jo::Barracks::row_icons, i))->scale_to(joan::orig::arg::i16(flag), joan::orig::arg::f32(0x41200000u), 0.0f, 0, 0u);                             // 0x0061f1b0
        joan::orig::as_widget(row(&jo::Barracks::add_buttons, i))->move_to(joan::orig::arg::i16(id_rest), joan::orig::arg::i16(base + 0x7cu), joan::orig::arg::f32(0x43960000u), joan::orig::arg::f32(0xc3480000u), 0, 0u);   // 0x0061e020
        joan::orig::as_widget(row(&jo::Barracks::add_buttons, i))->scale_to(joan::orig::arg::i16(flag), joan::orig::arg::f32(0x41200000u), 0.0f, 0, 0u);                             // 0x0061f1b0
        joan::orig::as_widget(row(&jo::Barracks::remove_buttons, i))->move_to(joan::orig::arg::i16(id_rest), joan::orig::arg::i16(base + 0x9cu), joan::orig::arg::f32(0x43960000u), joan::orig::arg::f32(0xc3480000u), 0, 0u);   // 0x0061e020
        joan::orig::as_widget(row(&jo::Barracks::remove_buttons, i))->scale_to(joan::orig::arg::i16(flag), joan::orig::arg::f32(0x41200000u), 0.0f, 0, 0u);                             // 0x0061f1b0
    }
    static i32 clicked(void* widget, u32 a) {
        return joan::orig::arg::as<i32>(joan::orig::as_ui_button(widget)->mouse(joan::orig::arg::i32(a), 0, 0xffffffff, 0, 0, 0));  // 0x006218c0
    }
    i32& type_count(i32 kind) { return jo::view<i32>(jo::self<jo::Barracks>(this)->orders[kind]); }
    static void add_to(i32& v, u32 d) { v = static_cast<i32>(static_cast<u32>(v) + d); }

    // 0x006bf630
    u8 recruit_update() {
        c_recruit.hit();
        if (jo::self<jo::Barracks>(this)->open == 0) return 0;
        void* const objects = obj(d::g_places);
        if (joan::orig::arg::as<i32>(joan::orig::as_list(objects)->empty_at_e(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->place))) != 0) return 0;  // 0x0044cf80
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(objects)->get_e(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->place)));     // 0x0043c450
        using Virtual = u8* (Obj::*)();
        JOAN_MEMBER_POINTER_CHECK(Virtual);
        Virtual method{};
        std::memcpy(&method, &joan::call::vtable_of(o)[0x24 / 4], sizeof(void*));
        u8* const owner = (reinterpret_cast<Obj*>(o)->*method)();
        void* pc = nullptr;
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->army)) == 0)  // 0x00428b70
            pc = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::self<jo::Barracks>(this)->army));  // 0x0041b3c0
        jo::self<jo::Barracks>(this)->hover_row = -1;
        const i32 count = joan::orig::as_dynarray(jo::as_flag_facility(owner)->list_79)->size();  // 0x00417ad0
        for (i32 i = 0; i < count; ++i) {
            bool ok = false;
            if (jo::self<jo::Barracks>(this)->state == 1) {
                u8* const e = static_cast<u8*>(row(&jo::Barracks::row_frames, i));
                ok = joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(jo::as_button(e)->x), static_cast<i32>(jo::as_button(e)->y), joan::orig::arg::i32(static_cast<i32>(jo::as_button(e)->x2) + 0x4c), static_cast<i32>(jo::as_button(e)->y2))) != 0;  // 0x00593980
            }
            if (ok) {
                jo::self<jo::Barracks>(this)->hover_row = static_cast<i16>(i);
                jo::as_button(row(&jo::Barracks::row_frames, i))->held = 1;
                setup_row(i, 0x224, 0x238, 1);
            } else {
                jo::as_button(row(&jo::Barracks::row_frames, i))->held = 0;
                setup_row(i, 0x1d8, 0x1ec, 0);
            }
        }
        if (jo::self<jo::Barracks>(this)->state != 1) return 1;
        if (clicked(jo::view<void*>(jo::self<jo::Barracks>(this)->cancel_button), 0x1b) != 0) {
            joan::orig::as_barracks_screen(this)->set_open(0);  // 0x006c0310
            return 1;
        }
        if (clicked(jo::view<void*>(jo::self<jo::Barracks>(this)->ok_button), 0xd) != 0) {
            joan::orig::as_barracks_screen(this)->commit_units();      // 0x006bdc00
            joan::orig::as_barracks_screen(this)->set_open(0);  // 0x006c0310
            return 1;
        }
        void* const types = obj(d::g_types);
        static const img::Addr kNames[6] = {d::s_FRENCH_LIGHT_INFANTRY, d::s_FRENCH_HEAVY_INFANTRY, d::s_FRENCH_MACEMAN, d::s_FRENCH_POLEMAN, d::s_FRENCH_ARCHER, d::s_FRENCH_CROSSBOW_MAN};
        i32 kind = 0;
        for (i32 j = 0; j < count; ++j) {
            i16* const unit = joan::orig::arg::as<i16*>(joan::orig::as_dynarray(jo::as_flag_facility(owner)->list_79)->get(joan::orig::arg::i32(j + 1)));  // 0x00417200
            for (i32 k = 0; k < 6; ++k) {
                const i32 v = *unit;
                if (v == joan::orig::arg::as<i32>(joan::orig::as_unit_types(types)->find(joan::orig::arg::str(&img::global<const char>(kNames[k]))))) {  // 0x005270b0
                    kind = k;
                    break;
                }
            }
            const i32 type_id = *unit;
            u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_records_203(types)->get(joan::orig::arg::i16(type_id)));  // 0x0042d0a0
            if (clicked(row(&jo::Barracks::add_buttons, j), 0) != 0) {
                if (pc != nullptr) {
                    const i32 limit = joan::orig::arg::as<i32>(joan::orig::as_unit(pc)->morale_level());  // 0x00448080
                    if (!(jo::self<jo::Barracks>(this)->army_size < limit)) return 1;
                }
                if (!(jo::self<jo::Barracks>(this)->pool > 0)) return 1;
                if (static_cast<i32>(jo::view<u32>(jo::self<jo::Barracks>(this)->cost) + jo::view<u32>(jo::as_unit_type(def)->cost)) > jo::self<jo::Barracks>(this)->gold) return 1;
                jo::view<u32>(jo::self<jo::Barracks>(this)->cost) = jo::view<u32>(jo::self<jo::Barracks>(this)->cost) + jo::view<u32>(jo::as_unit_type(def)->cost);
                add_to(jo::self<jo::Barracks>(this)->pool, 0xffffffffu);
                add_to(jo::self<jo::Barracks>(this)->recruited, 1u);
                add_to(jo::self<jo::Barracks>(this)->army_size, 1u);
                add_to(type_count(kind), 1u);
                return 1;
            }
            if (clicked(row(&jo::Barracks::remove_buttons, j), 0) != 0) {
                if (!(type_count(kind) > 0)) return 1;
                jo::view<u32>(jo::self<jo::Barracks>(this)->cost) = jo::view<u32>(jo::self<jo::Barracks>(this)->cost) - jo::view<u32>(jo::as_unit_type(def)->cost);
                add_to(jo::self<jo::Barracks>(this)->pool, 1u);
                add_to(jo::self<jo::Barracks>(this)->recruited, 0xffffffffu);
                add_to(jo::self<jo::Barracks>(this)->army_size, 0xffffffffu);
                add_to(type_count(kind), 0xffffffffu);
                return 1;
            }
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_20", name, addr, fn)
B("RecruitPanel::update_6bf630", 0x006bf630, &H::recruit_update);
#undef B

}  // namespace game_b107_20
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

JOAN_COUNTER(c_6bfe00, "Intro::update_6bfe00");

// ------------------------------------------------------------------ thiscall members
struct H {

    // 0x006bfe00
    static void upd(void* o) { joan::orig::as_widget(o)->update(); }  // 0x0061f3b0
    static void anim(void* o, i32 a1, i32 a2, u32 f1, u32 f2, i32 a5, i32 a6) {
        joan::orig::as_widget(o)->move_to(joan::orig::arg::i16(a1), joan::orig::arg::i16(a2), joan::orig::arg::f32(f1), joan::orig::arg::f32(f2), static_cast<char>(a5), joan::orig::arg::u32(a6));  // 0x0061e020
    }
    static void step(void* o, i32 dir) {
        joan::orig::as_widget(o)->scale_to(joan::orig::arg::i16(dir), joan::orig::arg::f32(u32{0x40c00000}), 0.0f, 0, 0u);  // 0x0061f1b0 (6.0f)
    }
    static bool is_zero_or_nan(float v, float c) { return !(v < c) && !(v > c); }
    void update_6bfe00() {
        c_6bfe00.hit();
        if (static_cast<i8>(jo::view<u8>(jo::self<jo::Barracks>(this)->open)) == 0) return;
        for (i32 i = 0; i < 6; ++i) {
            upd(static_cast<void*>(jo::self<jo::Barracks>(this)->row_frames->item[i]));
            upd(static_cast<void*>(jo::self<jo::Barracks>(this)->row_icons->item[i]));
            upd(static_cast<void*>(jo::self<jo::Barracks>(this)->add_buttons->item[i]));
            upd(static_cast<void*>(jo::self<jo::Barracks>(this)->remove_buttons->item[i]));
        }
        upd(jo::self<jo::Barracks>(this)->ok_button);
        upd(jo::self<jo::Barracks>(this)->cancel_button);
        upd(jo::self<jo::Barracks>(this)->background);
        upd(jo::self<jo::Barracks>(this)->info_frame);
        upd(jo::self<jo::Barracks>(this)->title_frame);
        const i32 state = jo::self<jo::Barracks>(this)->state;
        if (state == 0) {
            if (is_zero_or_nan(jo::self<jo::Barracks>(this)->background->scale, 0.0f)) {
                i32 delay = 0;
                for (i32 i = 5; i >= 0; --i) {
                    void* const o = static_cast<void*>(jo::self<jo::Barracks>(this)->row_frames->item[i]);
                    anim(o, 0x24, i * 0x64 + 0x68, 0x455ac000u, 0xc4bb8000u, 0, delay);
                    step(static_cast<void*>(jo::self<jo::Barracks>(this)->row_frames->item[i]), 1);
                    delay += 0x1e;
                }
                step(jo::self<jo::Barracks>(this)->ok_button, 1);
                step(jo::self<jo::Barracks>(this)->cancel_button, 1);
                step(jo::self<jo::Barracks>(this)->background, 1);
                step(jo::self<jo::Barracks>(this)->title_frame, 1);
                step(jo::self<jo::Barracks>(this)->info_frame, 1);
                anim(jo::self<jo::Barracks>(this)->info_frame, 0x27b, 0x5b, 0x455ac000u, 0xc4bb8000u, 0, 0);
            }
            u8* const first = reinterpret_cast<u8*>(jo::self<jo::Barracks>(this)->row_frames->item[0]);
            if (is_zero_or_nan(jo::as_button(first)->scale, 1.0f)) {
                u8* const f2 = reinterpret_cast<u8*>(jo::self<jo::Barracks>(this)->row_frames->item[0]);
                if (jo::as_button(f2)->y == 0x68) jo::self<jo::Barracks>(this)->state = 1;
            }
            return;
        }
        if (state == 1) return;
        if (state == 2) {
            if (is_zero_or_nan(jo::self<jo::Barracks>(this)->background->scale, 1.0f)) {
                i32 delay = 0;
                for (i32 i = 0; i < 6; ++i) {
                    void* const o = static_cast<void*>(jo::self<jo::Barracks>(this)->row_frames->item[i]);
                    anim(o, 0x24, -100, 0x455ac000u, 0xc4bb8000u, 0, delay);
                    step(static_cast<void*>(jo::self<jo::Barracks>(this)->row_frames->item[i]), 0);
                    delay += 0x1e;
                }
                step(jo::self<jo::Barracks>(this)->ok_button, 0);
                step(jo::self<jo::Barracks>(this)->cancel_button, 0);
                step(jo::self<jo::Barracks>(this)->background, 0);
                step(jo::self<jo::Barracks>(this)->title_frame, 0);
                step(jo::self<jo::Barracks>(this)->info_frame, 0);
                anim(jo::self<jo::Barracks>(this)->info_frame, 0x400, 0x5b, 0x455ac000u, 0xc4bb8000u, 0, 0);
            }
            u8* const last = reinterpret_cast<u8*>(jo::self<jo::Barracks>(this)->row_frames->item[5]);
            if (is_zero_or_nan(jo::as_button(last)->scale, 0.0f)) {
                u8* const l2 = reinterpret_cast<u8*>(jo::self<jo::Barracks>(this)->row_frames->item[5]);
                if (jo::as_button(l2)->y == -100) jo::self<jo::Barracks>(this)->state = 3;
            }
            return;
        }
        if (state != 3) return;
        jo::view<u8>(jo::self<jo::Barracks>(this)->open) = 0;
        joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();  // 0x0042fef0
        joan::orig::as_barracks_screen(this)->free_sprites();             // 0x006c04d0
        if (global<u8>(d::g_game_running) == 0) {
            global<u8>(d::g_game_running) = 1;
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(static_cast<float>(global<i16>(d::g_saved_game_speed))));  // 0x0042f930
        }
        joan::orig::as_cursor(obj(d::g_mouse))->set_pos();  // 0x00592bd0
        jo::self<jo::Barracks>(this)->state = 0;
        global<u32>(d::g_a5ba65) = 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Intro::update_6bfe00", 0x006bfe00, &H::update_6bfe00);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b106_15.cpp (game/b106_15)
namespace {
namespace game_b106_15 {

using joan::hook::global;
using joan::hook::x87::ftol;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;

using u16 = std::uint16_t;

JOAN_COUNTER(c_set_active, "Obj6c0310::set_active");

struct H {
    // ---- 0x006c0310 -------------------------------------------------------------------
    void set_active(i32 arg) {
        c_set_active.hit();
        if (!(static_cast<i32>(static_cast<i8>(arg)) > 0)) {
            if (static_cast<i32>(static_cast<i8>(jo::view<u8>(jo::self<jo::Barracks>(this)->open))) == 1) {
                jo::self<jo::Barracks>(this)->u8_3d = 0;
                jo::view<u16>(jo::self<jo::Barracks>(this)->hover_row) = 0xffff;
            }
            jo::view<u16>(jo::self<jo::Barracks>(this)->state) = 2;
            return;
        }
        joan::orig::as_unit(this)->refresh_6bda00();  // 0x006bda00
        jo::view<u8>(jo::self<jo::Barracks>(this)->open) = 1;
        joan::orig::as_barracks_screen(this)->load_sprites();  // 0x006c0400
        if (global<u8>(d::g_game_running) != 0) {
            global<u8>(d::g_game_running) = 0;
            global<u16>(d::g_saved_game_speed) = static_cast<u16>(joan::hook::x87::ftol(static_cast<double>(global<float>(d::g_game_speed))));
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(0u);  // 0x0042f930
        }
        joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
        jo::view<u16>(jo::self<jo::Barracks>(this)->state) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_15", name, addr, fn)
B("Obj6c0310::set_active", 0x006c0310, &H::set_active);
#undef B

}  // namespace game_b106_15
}  // namespace

// ---------------------------------------- from modules/game_b94_08_2.cpp (game/b94_08)
namespace {
namespace game_b94_08_2 {

using u8 = std::uint8_t;


struct H {
    u8 byte_30() { return jo::view<u8>(jo::self<jo::Barracks>(this)->open); }  // 0x006c03e0
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::byte_30", 0x006c03e0, &H::byte_30);
#undef B94

}  // namespace game_b94_08_2
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::obj;

using joan::img::str;

void* const kRes = obj(d::g_texture_array);

// 0x006099c0 resource load (3 args) through thunk 0x00404d13.
void* res_load(const char* name) { return joan::orig::as_texture_manager(kRes)->load(joan::orig::arg::str(name), 1, 0u); }

JOAN_COUNTER(c_load_barr, "Obj::load_barr");

struct H {
    // ------------------------------------------------------------------ 0x006c0400
    void load_barr() {
        c_load_barr.hit();
        const i32 saved = joan::orig::arg::as<i32>(joan::orig::as_texture_manager(kRes)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kRes)->set_mode(1u);                      // 0x0060a740
        if (jo::self<jo::Barracks>(this)->tex_barr == nullptr) jo::self<jo::Barracks>(this)->tex_barr = res_load(str(d::s_BARR));      // "BARR"
        if (jo::self<jo::Barracks>(this)->tex_bg == nullptr) jo::self<jo::Barracks>(this)->tex_bg = res_load(str(d::s_BARR_BG));      // "BARR_BG"
        if (jo::self<jo::Barracks>(this)->tex_commele == nullptr) jo::self<jo::Barracks>(this)->tex_commele = res_load(str(d::s_COMMELE2));      // "COMMELE2"
        joan::orig::as_texture_manager(kRes)->set_mode(joan::orig::arg::u32(saved));                  // 0x0060a740
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::load_barr", 0x006c0400, &H::load_barr);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b109_03.cpp (game/b109_03)
namespace {
namespace game_b109_03 {

using joan::hook::global;
using joan::hook::x87::ftol;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::img::str;


JOAN_COUNTER(c_label, "label_6c0570");

// ============================================================================ 0x006c0570
u32 label_color(u8* o) {
    const i32 alpha = ftol(static_cast<double>(global<float>(d::flt_255) * jo::as_button(o)->scale));
    u32 c = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c)->construct(i32{0xff}, i32{0xff}, i32{0xff}, alpha));  // 0x0042a260
}
void label_text(i32 x, i32 y, i32 font, img::Addr s, u32 color) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(font), joan::orig::arg::str(str(s)), obj(d::g_sheet), joan::orig::arg::u32(color), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}

static i32 JOAN_CDECL label_6c0570(u8* o, i32 font, i32 flags) {
    c_label.hit();
    const u32 fb = static_cast<u32>(flags) & 0xffu;
    if (fb != 7 && (fb & 1u) != 0) return 0;
    i16 w4 = jo::as_button(o)->x;
    i16 w8 = jo::as_button(o)->y;
    i16 wc = jo::as_button(o)->x2;
    i16 w10 = jo::as_button(o)->y2;
    float f14 = 1.0f;
    float f18 = 1.0f;
    if (jo::as_button(o)->fit != 0)
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(static_cast<i32>(jo::as_button(o)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(o)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(o)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(o)->y2)), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    if (jo::view<i8>(jo::as_button(o)->caption[0]) != 0) {
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(jo::as_button(o)->caption), -1, 0, 0, joan::orig::arg::f32(u32{0x3f800000}));  // 0x00624f00
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(i32{2}), joan::orig::arg::f32(u32{0x3f800000}));  // 0x006253f0
    }
    if (jo::as_button(o)->enabled != 0) {
        if (jo::as_button(o)->pressed != 0) {
            label_text(w4, w8, font, d::s_B_DW, label_color(o));  // 'B_DW'
        } else if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(w4), static_cast<i32>(w8), static_cast<i32>(wc), static_cast<i32>(w10))) != 0) {  // 0x00593980
            label_text(w4, w8, font, d::s_B_OV, label_color(o));  // 'B_OV'
        } else {
            label_text(w4, w8, font, d::s_B_OT, label_color(o));  // 'B_OT'
        }
        if (std::strcmp(joan::orig::arg::str(jo::as_button(o)->caption), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(d::g_strings))->open_1()))) == 0) {
            label_text(static_cast<i32>(w4) + 5, static_cast<i32>(w8) + 8, font, d::s_CHECK, label_color(o));  // 'CHECK'
        } else if (std::strcmp(joan::orig::arg::str(jo::as_button(o)->caption), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(d::g_strings))->open_2()))) ==
                   0) {
            label_text(static_cast<i32>(w4) + 5, static_cast<i32>(w8) + 8, font, d::s_CROSS_9ddc7c, label_color(o));  // 'CROSS'
        }
    } else {
        label_text(w4, w8, font, d::s_B_OT, label_color(o));  // 'B_OT'
    }
    if (jo::view<i8>(jo::as_button(o)->caption[0]) != 0) {
        i32 r60 = 0;
        i32 r64 = 0;
        if (jo::as_button(o)->pressed != 0)
            r60 = static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(global<float>(d::flt_1) * f18)))));  // 0x00590a20
        if (jo::as_button(o)->pressed != 0)
            r64 = static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(global<float>(d::flt_1) * f14)))));
        const i32 x = static_cast<i32>(w4) + r64 + 0x28;
        const i32 y = static_cast<i32>(w8) + r60 + 0xa;
        joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(font), joan::orig::arg::str(jo::as_button(o)->caption), joan::orig::arg::u32(label_color(o)), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}));  // 0x006255d0
    }
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_03", name, addr, fn)
B("label_6c0570", 0x006c0570, &label_6c0570);
#undef B

}  // namespace game_b109_03
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using joan::hook::global;
using joan::hook::x87::ftol;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::img::str;

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), ret 0x10; returns this.
u32 make_color(u32 r, u32 g, u32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::as_colour(p));
}

JOAN_COUNTER(c_ovdraw, "ov_draw_6c0c00");

// 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
void draw_sprite(i32 x, i32 y, u32 surf, const char* a4, void* a5, u32 a6, i32 a7, i32 a8, i32 a9, i32 a10) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surf), a4, a5, joan::orig::arg::u32(a6), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, -1, -1);
}

// 0x006c0c00
u32 JOAN_CDECL ov_draw(void* ctl, u32 surf, u32 flags) {
    c_ovdraw.hit();
    if ((flags & 0xff) != 7 && ((flags & 0xff) & 1) != 0) return 0;
    i16 s4 = jo::as_button(ctl)->x;
    i16 s8 = jo::as_button(ctl)->y;
    i16 sc = jo::as_button(ctl)->x2;
    i16 s10 = jo::as_button(ctl)->y2;
    u32 f14 = 0x3f800000, f18 = 0x3f800000;
    if (jo::as_button(ctl)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::as_button(ctl)->x), joan::orig::arg::i16(jo::as_button(ctl)->y), joan::orig::arg::i16(jo::as_button(ctl)->x2), joan::orig::arg::i16(jo::as_button(ctl)->y2), &f14, &f18, &s4, &s8, &sc, &s10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    if (jo::as_button(ctl)->enabled == 0) return 1;
    auto alpha = [&]() -> i32 {
        const float a = global<float>(d::flt_255) * jo::as_button(ctl)->scale;
        return joan::hook::x87::ftol(static_cast<double>(a));
    };
    auto icon = [&](i32 x, i32 y, img::Addr name) {
        const i32 a = alpha();
        const u32 color = make_color(0xff, 0xff, 0xff, a);
        draw_sprite(x, y, surf, img::str(name), img::obj(d::g_sheet), color, 3, 0, 2, 0);
    };
    if (jo::view<i8>(jo::as_button(ctl)->held) != 0) {
        icon(s4, s8, d::s_I_OV);  // "I_OV"
    } else if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{s4}, i32{s8}, i32{sc}, i32{s10})) != 0) {  // 0x00593980
        icon(s4, s8, d::s_I_OV);  // "I_OV"
    } else {
        icon(s4, s8, d::s_I_OT);  // "I_OT"
    }
    icon(s4 + 0x5c, s8 + 0x28, d::s_graybar);  // "graybar"
    auto bar = [&](std::uintptr_t text_fn, i32 dx) {
        const i32 a = alpha();
        const u32 color = make_color(0x20, 0x20, 0x20, a);
        const char* const text = thiscall<const char*>(text_fn, obj(d::g_strings));
        joan::orig::as_text(obj(d::g_font_goudy12_bold_black))->draw_line(joan::orig::arg::i32(s4 + dx), joan::orig::arg::i32(s8 + 0x30), joan::orig::arg::u32(surf), text, joan::orig::arg::u32(color), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}));  // 0x006255d0
    };
    bar(fn::code_value(fn::Barrack_open_07), 0x60);   // 0x004ddb30
    bar(fn::code_value(fn::Menu_barrack_8), 0xc8);   // 0x004ddb60
    bar(fn::code_value(fn::Menu_barrack_09), 0x130);  // 0x004ddb90
    bar(fn::code_value(fn::Barrack_open_10), 0x198);  // 0x004ddbc0
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("ov_draw_6c0c00", 0x006c0c00, &ov_draw);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b109_17.cpp (game/b109_17)
namespace {
namespace game_b109_17 {

using joan::hook::global;
using joan::hook::x87::ftol;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::img::str;

JOAN_COUNTER(c_info1, "draw_info_frame_6c1180");

// ------------------------------------------------------------------ shared draw helpers
// Colour object 0x0042a260 (r, g, b, a) through thunk 0x004098b8; returns the packed dword.
u32 rgba(i32 r, i32 g, i32 b, i32 a) {
    u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&slot)->construct(r, g, b, a));  // 0x0042a260
}
// fld [0x9cfb68] (255.0f); fmul [button+0x11]; _ftol.
i32 alpha_of(const u8* button) {
    return ftol(static_cast<double>(global<float>(d::flt_255) * jo::as_button(button)->scale));
}
// Sprite 0x00633500 on 0xa46128 (17 args, ret 0x44).
void sprite(i32 x, i32 y, u32 ctx, const char* name, void* tex, u32 color, i32 e7, i32 e8, i32 e9, i32 e10) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(name), tex, joan::orig::arg::u32(color), joan::orig::arg::i16(e7), joan::orig::arg::i16(e8), joan::orig::arg::i16(e9), joan::orig::arg::i16(e10), -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// Text 0x006255d0 (12 args, ret 0x30).
void text(img::Addr font, i32 x, i32 y, u32 ctx, const char* s, u32 color, i32 e6, i32 e7, i32 e8, i32 e9,
          i32 e10) {
    joan::orig::as_text(obj(font))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(ctx), s, joan::orig::arg::u32(color), joan::orig::arg::i32(e6), joan::orig::arg::i32(e7), joan::orig::arg::i32(e8), joan::orig::arg::i32(e9), joan::orig::arg::i32(e10), 1.0f, 1.0f);  // 0x006255d0
}

// Button rectangle (+9/+0xb/+0xd/+0xf words), scaled by 0x00633e60 when +6 is set.
struct Rect {
    i16 x, y, w, h;
    float sx, sy;  // ebp-0x14 / ebp-0x18
};
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

// Frame pieces of 0x006c1180 / 0x006ee790 (sprite tex 0xa5cf88).
void frame_sprite(const u8* b, i32 x, i32 y, u32 ctx, img::Addr name) {
    const u32 c = rgba(0xff, 0xff, 0xff, alpha_of(b));
    sprite(x, y, ctx, str(name), img::obj(d::g_sheet), c, 3, 0, 2, 0);
}
u32 grey(const u8* b) { return rgba(0x20, 0x20, 0x20, alpha_of(b)); }
void frame_text(const Rect& r, i32 dy, u32 ctx, const char* s, u32 color) {
    text(d::g_font_goudy14_22_white, static_cast<i32>(r.x) + 0x17, static_cast<i32>(r.y) + dy, ctx, s, color, -1, 0, 1, 2, 0);
}

// ------------------------------------------------------------------ 0x006c1180
static u32 JOAN_CDECL draw_info_frame_6c1180(u8* b, u32 ctx, u32 state) {
    c_info1.hit();
    if (skip_state(state)) return 0;
    const Rect r = button_rect(b);
    u8* sel = nullptr;  // ebp-0x20
    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<u32>(d::g_a48959))) == 0)  // 0x00428b70
        sel = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_a48959))));  // 0x0041b3c0
    if (jo::as_button(b)->enabled == 0) return 1;
    const i32 x = r.x, y = r.y;
    frame_sprite(b, x, y, ctx, d::s_FRAME);                // FRAME
    frame_sprite(b, x + 0x13, y + 0xf, ctx, d::s_titlebar);   // titlebar
    {
        const u32 c = grey(b);
        frame_text(r, 0x11, ctx, joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(d::g_strings))->open_0b()), c);  // 0x004ddbf0
    }
    frame_sprite(b, x + 0x13, y + 0x73, ctx, d::s_goldbar);  // goldbar
    frame_sprite(b, x + 0x13, y + 0x7f, ctx, d::s_titlebar);  // titlebar
    {
        const u32 c = grey(b);
        frame_text(r, 0x81, ctx, joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(d::g_strings))->open_0c()), c);  // 0x004ddc20
    }
    frame_sprite(b, x + 0x13, y + 0xf7, ctx, d::s_goldbar);  // goldbar
    if (sel != nullptr) {
        frame_sprite(b, x + 0x13, y + 0x103, ctx, d::s_titlebar);  // titlebar
        const u32 c = grey(b);
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_a48959))));  // 0x0041b3c0
        frame_text(r, 0x105, ctx, jo::as_unit(unit)->name_, c);  // +0x3d on x86
    }
    frame_sprite(b, x + 0x13, y + 0x11f, ctx, d::s_titlebar);  // titlebar
    const u32 count = sel != nullptr ? joan::orig::arg::as<u32>(joan::orig::as_unit(sel)->morale_level()) : u32{0x14};  // 0x00448080
    {
        const u32 c = grey(b);
        frame_text(r, 0x121, ctx, joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(d::g_strings))->open_0d(joan::orig::arg::u32(count))), c);  // 0x004ddc50
    }
    frame_sprite(b, x + 0x13, y + 0x19a, ctx, d::s_goldbar);  // goldbar
    frame_sprite(b, x + 0x13, y + 0x1a6, ctx, d::s_titlebar);  // titlebar
    {
        const u32 c = grey(b);
        frame_text(r, 0x1a8, ctx, joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(d::g_strings))->barrack_e()), c);  // 0x004ddc90
    }
    frame_sprite(b, x + 0x13, y + 0x1d6, ctx, d::s_goldbar);  // goldbar
    frame_sprite(b, x + 0x13, y + 0x1e2, ctx, d::s_titlebar);  // titlebar
    {
        const u32 c = grey(b);
        frame_text(r, 0x1e4, ctx, joan::orig::arg::as<const char*>(joan::orig::as_barrack(obj(d::g_strings))->state3()), c);  // 0x004dda60
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_17", name, addr, fn)
B("draw_info_frame_6c1180", 0x006c1180, &draw_info_frame_6c1180);
#undef B

}  // namespace game_b109_17
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

using joan::hook::global;
using joan::hook::x87::ftol;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::img::str;

JOAN_COUNTER(c_title, "draw_title_frame_6c1c70");

// ------------------------------------------------------------------ 0x006c1c70
static u32 button_color(u8* button, u32* slot) {
    const float a = global<float>(d::flt_255) * jo::as_button(button)->scale;
    const i32 alpha = joan::hook::x87::ftol(static_cast<double>(a));
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, alpha));  // 0x0042a260
}

static u32 JOAN_CDECL draw_title_frame(u8* button, u32 ctx, u32 state_arg) {
    c_title.hit();
    const u32 state = state_arg & 0xff;
    if (state != 7 && (state & 1) != 0) return 0;
    i16 w4 = jo::as_button(button)->x;
    i16 w8 = jo::as_button(button)->y;
    i16 wc = jo::as_button(button)->x2;
    i16 w10 = jo::as_button(button)->y2;
    float f14 = 1.0f;
    float f18 = 1.0f;
    if (jo::as_button(button)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(// 0x00633e60
                      static_cast<i32>(jo::as_button(button)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->y2)), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
    }
    const char* const font = joan::orig::as_menu(obj(d::g_strings))->barrack_0f();  // 0x004ddcc0
    const i32 w20 = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(font, -1, 0, 0, 1.0f)));  // 0x00624f00
    if (jo::as_button(button)->enabled == 0) return 1;
    u32 s28 = 0;
    const u32 c1 = button_color(button, &s28);
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(static_cast<i32>(w4)), joan::orig::arg::i16(static_cast<i32>(w8)), joan::orig::arg::u32(ctx), str(d::s_TITLE), img::obj(d::g_sheet), joan::orig::arg::u32(c1), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
    u32 s30 = 0;
    const u32 c2 = button_color(button, &s30);
    const i32 lx = (static_cast<i32>(w4) + static_cast<i32>(wc) - w20) / 2 - 0x48;
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(lx), joan::orig::arg::i16(static_cast<i32>(w8) + 4), joan::orig::arg::u32(ctx), str(d::s_L_ARROW), img::obj(d::g_sheet), joan::orig::arg::u32(c2), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
    u32 s38 = 0;
    const u32 c3 = button_color(button, &s38);
    const i32 rx = (w20 + static_cast<i32>(w4) + static_cast<i32>(wc)) / 2 + 4;
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(rx), joan::orig::arg::i16(static_cast<i32>(w8) + 4), joan::orig::arg::u32(ctx), str(d::s_R_ARROW), img::obj(d::g_sheet), joan::orig::arg::u32(c3), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
    u32 s40 = 0;
    const u32 c4 = button_color(button, &s40);
    const char* const font2 = joan::orig::as_menu(obj(d::g_strings))->barrack_0f();  // 0x004ddcc0
    joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_3(static_cast<i32>(w4), joan::orig::arg::i32(static_cast<i32>(w8) - 1), joan::orig::arg::i32(// 0x00626130
                  static_cast<i32>(wc)), joan::orig::arg::i32(static_cast<i32>(w10) - 5), joan::orig::arg::u32(ctx), font2, joan::orig::arg::u32(c4), i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("draw_title_frame_6c1c70", 0x006c1c70, &draw_title_frame);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using u32 = std::uint32_t;

using joan::img::obj;

std::uintptr_t color_ctor(void* c, u32 r, u32 g, u32 b, u32 a) {
    return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260 (returns this)
}

std::uintptr_t JOAN_CDECL color_a6a54c() {  // 0x006c2030
    return color_ctor(obj(d::g_a6a54c), 0xff, 0xff, 0xff, 0x40);
}

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("Globals::color_a6a54c", 0x006c2030, &color_a6a54c);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::BarracksScreen::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b103_15::H::barr_release>(this)); }  // 0x6bcf00
auto joan::orig::Image2D::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_08::H::sprite_scalar_dtor>(this, a0)); }  // 0x6bd440
auto joan::orig::Stats::count_unit(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_13::H::count_unit>(this, a0)); }  // 0x6bd4b0
auto joan::orig::Unit::refresh_6bda00() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b105_19::H::refresh_6bda00>(this)); }  // 0x6bda00
auto joan::orig::BarracksScreen::commit_units() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_15::H::commit_units>(this)); }  // 0x6bdc00
auto joan::orig::BarracksScreen::draw() -> void { joan::bind::member<&::game_ui5::H::army_draw>(this); }  // 0x6bdea0
auto joan::orig::BarracksScreen::set_open(std::int32_t a0) -> void { joan::bind::member<&::game_b106_15::H::set_active>(this, a0); }  // 0x6c0310
auto joan::orig::BarracksScreen::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b94_08_2::H::byte_30>(this)); }  // 0x6c03e0
auto joan::orig::BarracksScreen::load_sprites() -> void { joan::bind::member<&::game_b97_08::H::load_barr>(this); }  // 0x6c0400
auto joan::orig::BarracksScreen::free_sprites() -> void { joan::bind::member<&::game_b93_06::H::free3>(this); }  // 0x6c04d0
