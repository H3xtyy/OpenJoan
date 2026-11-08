// interfac/i_sms.cpp: functions of the original d:\ww\interfac\i_sms.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>
#include <limits>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstdarg>
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::cdecl_call;

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kSmsFile(d::s_d_ww_interfac_i_sms_cpp);  // "d:\ww\interfac\i_sms.cpp"

}  // namespace

// ---------------------------------------- from modules/game_ui1.cpp (game/ui1)
//
//      - Sms::add 0x006e5700 (interfac/i_sms.cpp, text, kind, replace): queues a message node
//        (0x14 bytes: +0 text copy, +4 kind, +8 replace flag, +0xc prev, +0x10 next) in the
//        list +0x184 (head) / +0x188 (tail) / +0x18c (count) and lays out the scroller planes
//        (+0x8, +0xbf). Called by World::draw for the loading message.
namespace {
namespace game_ui1 {

using joan::call::thiscall;

std::int32_t neg(std::int32_t v) { return static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(v)); }  // imul r, r, -1

JOAN_COUNTER(c_sms_add, "Sms::add");

struct H {
    // The scroller layout after the head message changed (+0x180 its width).
    void sms_layout_planes() {
        joan::orig::as_widget(&jo::self<jo::Sms>(this)->text)->scale_to(0, 1.0f, 0.0f, 1, 0u);                                   // 0x0061f1b0
        joan::orig::as_widget(&jo::self<jo::Sms>(this)->text)->move_to(joan::orig::arg::i16(neg(jo::view<std::int32_t>(jo::self<jo::Sms>(this)->u32_180))), 0x206, 1.0f, 0.0f, 1, 0u);   // 0x0061e020
        joan::orig::as_widget(&jo::self<jo::Sms>(this)->bar)->scale_to(0, 1.0f, 0.0f, 1, 0u);
        joan::orig::as_widget(&jo::self<jo::Sms>(this)->bar)->move_to(joan::orig::arg::i16(neg(jo::view<std::int32_t>(jo::self<jo::Sms>(this)->u32_180))), 0x204, 1.0f, 0.0f, 1, 0u);
    }
    std::int32_t sms_head_width() {
        void* const font = img::obj(d::g_font_goudy14_22_white);
        return joan::orig::as_font(font)->measure(joan::orig::arg::str(jo::view<const char*>(jo::as_sms_node(jo::view<void*>(jo::self<jo::Sms>(this)->head))->text)), -1, 0, 0, 1.0f);  // 0x00624f00
    }

    // 0x006e5700 Sms::add(text, kind, replace). ret 0xc.
    void sms_add(const char* text, std::int32_t kind, std::int32_t replace) {
        c_sms_add.hit();
        if (text == nullptr) return;
        if (joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(text))) == 0) return;  // strlen
        std::uint8_t* const node = joan::orig::arg::as<std::uint8_t*>(joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SmsNode))));  // operator new (0x14 on x86)
        jo::view<void*>(jo::as_sms_node(node)->text) = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(text))) + 1), joan::orig::arg::str(img::str(kSmsFile)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a25600)) + 8));  // 0x0058e7b0
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_sms_node(node)->text)), joan::orig::arg::str(text));  // strcpy
        jo::as_sms_node(node)->kind = kind;
        jo::as_sms_node(node)->replace = replace;
        jo::as_sms_node(node)->next = nullptr;
        jo::as_sms_node(node)->prev = nullptr;
        if (jo::self<jo::Sms>(this)->count != 0 && replace == 1) {
            std::uint8_t* cur = jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->head);
            while (cur != nullptr) {
                std::uint8_t* const next = jo::self<jo::Sms>(this)->count > 1 ? jo::view<std::uint8_t*>(jo::as_sms_node(cur)->next) : nullptr;
                if (jo::as_sms_node(cur)->replace == 1) {
                    if (cur == jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->head)) jo::self<jo::Sms>(this)->head = jo::as_sms_node(cur)->next;
                    else jo::as_sms_node(cur)->prev->next = jo::as_sms_node(cur)->next;
                    if (cur == jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->tail)) jo::self<jo::Sms>(this)->tail = jo::as_sms_node(cur)->prev;
                    else jo::as_sms_node(cur)->next->prev = jo::as_sms_node(cur)->prev;
                    joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::as_sms_node(cur)->text)), joan::orig::arg::str(img::str(kSmsFile)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a25600)) + 0x23));  // 0x0058ed70
                    joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(cur)));                                // operator delete
                    jo::self<jo::Sms>(this)->count = jo::self<jo::Sms>(this)->count - 1;
                    if (jo::self<jo::Sms>(this)->count == 0) {
                        jo::self<jo::Sms>(this)->head = nullptr;
                        jo::self<jo::Sms>(this)->tail = nullptr;
                    }
                }
                cur = next;
            }
        }
        if (jo::self<jo::Sms>(this)->count == 0) {
            jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->head) = node;
            jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->tail) = node;
            jo::as_sms_node(node)->prev = nullptr;
            jo::as_sms_node(node)->next = nullptr;
        } else if (kind == 1 || kind == 2) {
            if (jo::self<jo::Sms>(this)->shown != 0) joan::orig::as_sms(this)->pop_head();  // 0x006e5d00
            if (jo::self<jo::Sms>(this)->count == 0) {
                jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->head) = node;
                jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->tail) = node;
                jo::as_sms_node(node)->prev = nullptr;
                jo::as_sms_node(node)->next = nullptr;
            } else {
                jo::view<std::uint8_t*>(jo::as_sms_node(jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->head))->prev) = node;
                jo::as_sms_node(node)->prev = nullptr;
                jo::view<std::uint8_t*>(jo::as_sms_node(node)->next) = jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->head);
                jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->head) = node;
            }
            jo::self<jo::Sms>(this)->shown = 0;
            joan::orig::as_image2d(&jo::self<jo::Sms>(this)->text)->set_name(joan::orig::arg::str(jo::view<const char*>(jo::as_sms_node(jo::view<void*>(jo::self<jo::Sms>(this)->head))->text)));  // 0x006298c0
            jo::view<std::int32_t>(jo::self<jo::Sms>(this)->u32_180) = sms_head_width();
            sms_layout_planes();
        } else {
            jo::view<std::uint8_t*>(jo::as_sms_node(node)->prev) = jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->tail);
            jo::view<std::uint8_t*>(jo::as_sms_node(jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->tail))->next) = node;
            jo::self<jo::Sms>(this)->tail = jo::self<jo::Sms>(this)->tail->next;
        }
        jo::self<jo::Sms>(this)->count = jo::self<jo::Sms>(this)->count + 1;
        joan::orig::as_sms(this)->show(0);  // 0x006e5e60
        if (jo::self<jo::Sms>(this)->count != 1) return;
        jo::view<std::int32_t>(jo::self<jo::Sms>(this)->u32_180) = sms_head_width();
        joan::orig::as_image2d(&jo::self<jo::Sms>(this)->text)->set_name(joan::orig::arg::str(jo::view<const char*>(jo::as_sms_node(jo::view<void*>(jo::self<jo::Sms>(this)->head))->text)));  // 0x006298c0
        sms_layout_planes();
        jo::self<jo::Sms>(this)->shown = 0;
    }
};

#define U1(name, addr, fn) JOAN_REPLACE("game/ui1", name, addr, fn)
U1("Sms::add", 0x006e5700, &H::sms_add);
#undef U1

}  // namespace game_ui1
}  // namespace

// ---------------------------------------- from modules/game_ui2.cpp (game/ui2)
//
//      - Sms::pop_head 0x006e5d00 (interfac/i_sms.cpp): unlinks and frees the head message
//        node of the list +0x184 / +0x188 / +0x18c. ret 0.
namespace {
namespace game_ui2 {

JOAN_COUNTER(c_sms_pop, "Sms::pop_head");

struct H {
    // 0x006e5d00 Sms::pop_head. ret 0.
    void sms_pop_head() {
        c_sms_pop.hit();
        if (jo::self<jo::Sms>(this)->count == 0 || jo::view<void*>(jo::self<jo::Sms>(this)->head) == nullptr) return;
        std::uint8_t* const node = jo::view<std::uint8_t*>(jo::self<jo::Sms>(this)->head);
        jo::self<jo::Sms>(this)->head = jo::self<jo::Sms>(this)->head->next;
        jo::self<jo::Sms>(this)->count = jo::self<jo::Sms>(this)->count - 1;
        joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::as_sms_node(node)->text)), joan::orig::arg::str(img::str(kSmsFile)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a25604)) + 7));  // 0x0058ed70
        joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(node)));                            // operator delete
        if (jo::self<jo::Sms>(this)->count == 0) {
            jo::self<jo::Sms>(this)->tail = nullptr;
            jo::self<jo::Sms>(this)->head = nullptr;
        }
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/ui2", name, addr, fn)
U2("Sms::pop_head", 0x006e5d00, &H::sms_pop_head);
#undef U2

}  // namespace game_ui2
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
//
//    - Panel::slide_6e5280 0x006e5280 (ret 0; callers drop eax).
//
//  - 0x006e5280: fild int * qword -0.2 at PC24 via Fpu{true}; fcompp "less or unordered" skip.
namespace {
namespace game_b108_01 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;
using joan::orig::helpers::fbits;
// fcomp + fnstsw + test ah,0x40 jnz: equal or unordered.
bool eq_or_unord(float a, float b) { return !(a < b) && !(a > b); }

JOAN_COUNTER(c_slide, "Panel::slide_6e5280");

struct H {
    // -------------------------------------------------------------- 0x006e5280
    void panel_slide() {
        c_slide.hit();
        if (jo::view<i8>(jo::self<jo::Sms>(this)->visible) == 0) return;
        if (global<i8>(d::g_a5d384) != 0) return;
        if (global<i8>(d::g_a4fca8) != 0 || global<i8>(d::g_a48950) != 0 || global<i8>(d::g_a48564) != 0) return;
        const float one = global<float>(d::flt_1);
        if (jo::self<jo::Sms>(this)->shown == 0) {
            joan::orig::as_widget(&jo::self<jo::Sms>(this)->bar)->scale_to(1, joan::orig::arg::f32(fbits(0x3fc00000)), 0.0f, 0, 0u);  // 0x0061f1b0
            joan::orig::as_widget(&jo::self<jo::Sms>(this)->bar)->move_to(0, 0x204, joan::orig::arg::f32(fbits(0x4528c000)), joan::orig::arg::f32(fbits(0xc47a0000)), 0, 0u);  // 0x0061e020
            joan::orig::as_widget(&jo::self<jo::Sms>(this)->text)->scale_to(1, joan::orig::arg::f32(fbits(0x3fc00000)), 0.0f, 0, 0u);  // 0x0061f1b0
            joan::orig::as_widget(&jo::self<jo::Sms>(this)->text)->move_to(0, 0x206, joan::orig::arg::f32(fbits(0x4528c000)), joan::orig::arg::f32(fbits(0xc47a0000)), 0, 0u);  // 0x0061e020
            jo::self<jo::Sms>(this)->shown = 1;
        }
        if (jo::self<jo::Sms>(this)->shown == 1) jo::self<jo::Sms>(this)->shown = 2;
        if (jo::self<jo::Sms>(this)->shown == 2 && jo::self<jo::Sms>(this)->text.x == 0 && eq_or_unord(jo::self<jo::Sms>(this)->text.scale, one) &&
            jo::self<jo::Sms>(this)->bar.x == 0 && eq_or_unord(jo::self<jo::Sms>(this)->bar.scale, one)) {
            jo::self<jo::Sms>(this)->shown = 3;
            jo::self<jo::Sms>(this)->show_time = 0;
        }
        if (jo::self<jo::Sms>(this)->shown == 3) {
            if (jo::self<jo::Sms>(this)->show_time < jo::self<jo::Sms>(this)->duration) {
                jo::self<jo::Sms>(this)->show_time = jo::self<jo::Sms>(this)->show_time + global<u32>(d::g_frame_elapsed_ms);
            } else {
                joan::orig::as_widget(&jo::self<jo::Sms>(this)->text)->scale_to(0, joan::orig::arg::f32(fbits(0x3fc00000)), 0.0f, 0, 0u);  // 0x0061f1b0
                joan::orig::as_widget(&jo::self<jo::Sms>(this)->text)->move_to(joan::orig::arg::i16(static_cast<i32>(0u - jo::self<jo::Sms>(this)->u32_180)), 0x206, joan::orig::arg::f32(fbits(0)), joan::orig::arg::f32(fbits(0x44c80000)), 0, 0u);  // 0x0061e020
                jo::self<jo::Sms>(this)->shown = 4;
            }
        }
        if (jo::self<jo::Sms>(this)->shown == 4) {
            const Fpu fpu{true};
            const double target = fpu.mul(static_cast<double>(jo::view<i32>(jo::self<jo::Sms>(this)->u32_180)), global<double>(d::dbl_m0_2));
            if (!(target < static_cast<double>(static_cast<i32>(jo::self<jo::Sms>(this)->text.x)))) {
                joan::orig::as_widget(&jo::self<jo::Sms>(this)->bar)->scale_to(0, joan::orig::arg::f32(fbits(0x3fc00000)), 0.0f, 0, 0u);  // 0x0061f1b0
                joan::orig::as_widget(&jo::self<jo::Sms>(this)->bar)->move_to(joan::orig::arg::i16(static_cast<i32>(0u - jo::self<jo::Sms>(this)->u32_180)), 0x204, joan::orig::arg::f32(fbits(0)), joan::orig::arg::f32(fbits(0x44c80000)), 0, 0u);  // 0x0061e020
                jo::self<jo::Sms>(this)->shown = 5;
            }
        }
        if (jo::self<jo::Sms>(this)->shown == 5 &&
            static_cast<i32>(jo::self<jo::Sms>(this)->bar.x) == static_cast<i32>(0u - jo::self<jo::Sms>(this)->u32_180) &&
            static_cast<i32>(jo::self<jo::Sms>(this)->text.x) == static_cast<i32>(0u - jo::self<jo::Sms>(this)->u32_180)) {
            joan::orig::as_sms(this)->pop_head();  // 0x006e5d00
            jo::self<jo::Sms>(this)->shown = 0;
            if (jo::self<jo::Sms>(this)->count > 0) {
                joan::orig::as_image2d(&jo::self<jo::Sms>(this)->text)->set_name(joan::orig::arg::str(jo::as_sms_node(jo::self<jo::Sms>(this)->head)->text));  // 0x006298c0
                const u32 w = joan::orig::arg::as<u32>(joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(jo::as_sms_node(jo::self<jo::Sms>(this)->head)->text), -1, 0, 0, joan::orig::arg::f32(fbits(0x3f800000))));  // 0x00624f00
                jo::view<i32>(jo::self<jo::Sms>(this)->u32_180) = static_cast<i16>(w);
            }
            return;
        }
        if (jo::self<jo::Sms>(this)->count <= 0) return;
        if (jo::view<u32>(jo::as_sms_node(jo::self<jo::Sms>(this)->head)->kind) == 1 || jo::view<u32>(jo::as_sms_node(jo::self<jo::Sms>(this)->head)->kind) == 2 ||
            global<u8>(d::g_game_running) != 0) {
            joan::orig::as_widget(&jo::self<jo::Sms>(this)->text)->update();     // 0x0061f3b0
            joan::orig::as_widget(&jo::self<jo::Sms>(this)->bar)->update();  // 0x0061f3b0
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Panel::slide_6e5280", 0x006e5280, &H::panel_slide);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
//
//    - repeat_step 0x006e5e00, get_d32 0x006e7b50, menu reset 0x006f21c0, menu slide-out
//      0x006f26e0, Triple::clear 0x006f5e90, Screen::release 0x0070e3c0, screen offset
//      0x0070fa70, Marker::update 0x00714040, Marker::visible 0x00717650, set_90 0x007563f0.
namespace {
namespace game_b90_06 {

using joan::call::thiscall;

struct H {
    // 0x006e5e00 runs the step 0x006e5d00 (+0x18c) times, then clears +0x176.
    void* repeat_step() {
        for (std::int32_t i = 0; i < jo::self<jo::Sms>(this)->count; ++i) joan::orig::as_sms(this)->pop_head();  // 0x006e5d00
        jo::self<jo::Sms>(this)->shown = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Obj::repeat_step", 0x006e5e00, &H::repeat_step);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_ui2.cpp (game/ui2)
//
//      - Sms::show 0x006e5e60 (duration): +0x190 = 1, 0x006e5f50, +0x178 duration (3000 when
//        <= 0). ret 4.
namespace {
namespace game_ui2_2 {

using joan::call::thiscall;

struct H {
    // 0x006e5e60 Sms::show. ret 4.
    void sms_show(std::int32_t duration) {
        jo::self<jo::Sms>(this)->visible = 1;
        joan::orig::as_sms(this)->load_sound();  // 0x006e5f50
        if (duration > 0) jo::view<std::int32_t>(jo::self<jo::Sms>(this)->show_time) = duration;
        else jo::view<std::int32_t>(jo::self<jo::Sms>(this)->show_time) = 0xbb8;
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/ui2", name, addr, fn)
U2("Sms::show", 0x006e5e60, &H::sms_show);
#undef U2

}  // namespace game_ui2_2
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

using joan::orig::helpers::addr;

struct H {
    std::uintptr_t ctor_6e4e20() {  // 0x006e4e20; eax = this
        joan::orig::as_image2d(&jo::self<jo::Sms>(this)->text)->construct();  // 0x00629400
        joan::orig::as_image2d(&jo::self<jo::Sms>(this)->bar)->construct();
        jo::self<jo::Sms>(this)->sound = 0;
        jo::self<jo::Sms>(this)->font = nullptr;
        jo::view<u16>(jo::self<jo::Sms>(this)->shown) = 0;
        jo::self<jo::Sms>(this)->show_time = 0;
        jo::self<jo::Sms>(this)->duration = 0xbb8;
        jo::self<jo::Sms>(this)->head = nullptr;
        jo::self<jo::Sms>(this)->tail = nullptr;
        jo::view<u32>(jo::self<jo::Sms>(this)->count) = 0;
        return addr(this);
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::ctor_6e4e20", 0x006e4e20, &H::ctor_6e4e20);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

using joan::img::str;

JOAN_COUNTER(c_quickbar_init, "QuickBar::init");

struct H {
    // ------------------------------------------------------------------ 0x006e4f10
    void* quickbar_init() {
        c_quickbar_init.hit();
        jo::self<jo::Sms>(this)->font = img::obj(d::g_bmp_res_qsave);
        jo::self<jo::Sms>(this)->sound = 0;
        jo::view<u16>(jo::self<jo::Sms>(this)->shown) = 0;
        jo::self<jo::Sms>(this)->show_time = 0;
        jo::self<jo::Sms>(this)->duration = 0xbb8;
        {
            u32 colour_obj[2];
            const u32 c = *joan::orig::arg::as<u32*>(joan::orig::as_colour(colour_obj)->construct(0u, 0u, 0u, 0xffu));  // 0x0042a260
            joan::orig::as_image2d(&jo::self<jo::Sms>(this)->text)->create(0, 0x206, joan::orig::as_font2d(img::obj(d::g_font_goudy14_22_white)), str(d::s_temp), 0, joan::orig::arg::i32(c), joan::orig::arg::i32(// 0x00629530 "temp"
                     0u), 0, 2, 0, 0x3f800000, 0x3f800000, 0, 1, 1);
        }
        joan::orig::as_widget(&jo::self<jo::Sms>(this)->text)->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061f1b0
        {
            u32 colour_obj[2];
            const u32 c = *joan::orig::arg::as<u32*>(joan::orig::as_colour(colour_obj)->construct(0u, 0u, 0u, 0xffu));  // 0x0042a260
            joan::orig::as_label(&jo::self<jo::Sms>(this)->bar)->create(0, 0x204, joan::orig::arg::mptr(jo::self<jo::Sms>(this)->font), str(d::s_QLC_BAR), joan::orig::arg::i32(c), joan::orig::arg::i32(// 0x00629660 "QLC_BAR"
                     3u), 0, 2, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0, 1, 1);
        }
        joan::orig::as_widget(&jo::self<jo::Sms>(this)->bar)->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061f1b0
        jo::self<jo::Sms>(this)->head = nullptr;
        jo::self<jo::Sms>(this)->tail = nullptr;
        jo::view<u32>(jo::self<jo::Sms>(this)->count) = 0;
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("QuickBar::init", 0x006e4f10, &H::quickbar_init);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

struct H {
    void panel_reset_6e50c0() {  // 0x006e50c0
        joan::orig::as_sms(this)->free_sound();  // 0x006e5fc0
        joan::orig::as_image2d(&jo::self<jo::Sms>(this)->text)->release();   // 0x00629830
        joan::orig::as_image2d(&jo::self<jo::Sms>(this)->bar)->release();  // 0x00629830 (the Label's Image2D base)
        jo::view<u16>(jo::self<jo::Sms>(this)->shown) = 0;
        jo::self<jo::Sms>(this)->show_time = 0;
        jo::self<jo::Sms>(this)->duration = 0xbb8;
        joan::orig::as_sms(this)->clear();  // 0x006e5e00
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Panel::reset_6e50c0", 0x006e50c0, &H::panel_reset_6e50c0);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b116_07.cpp (game/b116_07)
namespace {
namespace game_b116_07 {

using joan::call::thiscall;
using joan::img::obj;

using joan::hook::global;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

i32 gsb(img::Addr a) { return static_cast<i32>(global<i8>(a)); }

JOAN_COUNTER(c_pickup, "PickupNotice::draw_6e5140");

struct H {
    // 0x006e5140
    void pickup_draw(u32 mode) {
        joan::canvas::ScreenScope canvas_scope(0x006e5140);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_pickup.hit();
        const i32 m = static_cast<i32>(static_cast<i8>(mode));
        if (static_cast<i32>(static_cast<i8>(jo::self<jo::Sms>(this)->visible)) == 0) return;
        if (gsb(d::g_a5d384) != 0) return;
        if (gsb(d::g_a4fca8) != 0 || gsb(d::g_a48950) != 0 || gsb(d::g_a48564) != 0) return;
        if (jo::self<jo::Sms>(this)->count <= 0) return;
        if (jo::as_sms_node(jo::self<jo::Sms>(this)->head)->kind == 2 && m == 2) {
            joan::orig::as_label(&jo::self<jo::Sms>(this)->bar)->draw(1u, 7u);  // 0x00629bd0
            joan::orig::as_label(&jo::self<jo::Sms>(this)->text)->draw(1u, 7u);
            return;
        }
        if (gsb(d::g_hud) != 0) return;
        if (joan::orig::arg::as<u8>(joan::orig::as_glory_screen(obj(d::g_glory_screen))->is_open()) != 0) return;  // 0x006e7b50
        if (joan::orig::arg::as<u8>(joan::orig::as_letter(obj(d::g_letter))->get_8()) != 0) return;  // 0x006e0a90
        if (m != 1) return;
        joan::orig::as_label(&jo::self<jo::Sms>(this)->bar)->draw(5u, 7u);
        joan::orig::as_label(&jo::self<jo::Sms>(this)->text)->draw(5u, 7u);
    }
};

JOAN_REPLACE("game/b116_07", "PickupNotice::draw", 0x006e5140, &H::pickup_draw);
JOAN_CANVAS_SELF(0x006e5140);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_07
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using u16 = std::uint16_t;

struct H {
    // ------------------------------------------------------------------ 0x006e5ec0
    void* reset_6e5ec0() {
        jo::self<jo::Sms>(this)->visible = 0;
        joan::orig::as_sms(this)->free_sound();  // 0x006e5fc0
        jo::view<u16>(jo::self<jo::Sms>(this)->shown) = 0;
        jo::self<jo::Sms>(this)->show_time = 0;
        jo::self<jo::Sms>(this)->duration = 0xbb8;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen::reset_6e5ec0", 0x006e5ec0, &H::reset_6e5ec0);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::addr_of;

struct H {
    u32 self_hi() { return addr_of(this) & 0xffffff00u; }

    // ------------------------------------------------------------------------ 0x006e5f20
    u32 byte190_6e5f20() { return self_hi() | jo::self<jo::Sms>(this)->visible; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::byte190_6e5f20", 0x006e5f20, &H::byte190_6e5f20);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_ui6.cpp (game/ui6)
namespace {
namespace game_ui6 {

using u32 = std::uint32_t;

using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_qsave, "Sms::load_sound");

struct H {
    // 0x006e5f50 Sms::load_sound. ret 0.
    void load_sound() {
        c_qsave.hit();
        const std::int32_t mode = joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(obj(d::g_texture_array))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(1u);                                      // 0x0060a740
        if (jo::self<jo::Sms>(this)->sound == 0) {
            jo::self<jo::Sms>(this)->sound = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(str(d::s_QSAVE), 1, 0u));  // 0x006099c0 'QSAVE'
        }
        joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(joan::orig::arg::u32(mode));
    }
};

#define U6(name, addr, fn) JOAN_REPLACE("game/ui6", name, addr, fn)
U6("Sms::load_sound", 0x006e5f50, &H::load_sound);
#undef U6

}  // namespace game_ui6
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

using i32 = std::int32_t;

using joan::img::obj;

void* const kRes = obj(d::g_texture_array);

void res_free(void* handle) { joan::orig::as_texture_manager(kRes)->release(handle); }  // 0x0060a550

struct H {
    void free1() {  // 0x006d66f0 / 0x006e45b0 / 0x006e4dd0 / 0x006e5fc0; this one on the Sms. TODO(re): 0x006e5fc0 frees +0 through TextureManager::release 0x0060a550, so the field is a texture handle, not a sound
        if (jo::self<jo::Sms>(this)->sound != nullptr) {
            res_free(jo::self<jo::Sms>(this)->sound);
            jo::self<jo::Sms>(this)->sound = nullptr;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Res1::free_6e5fc0", 0x006e5fc0, &H::free1);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x006e6010
    u8 byte_184_8() {
        if (jo::self<jo::Sms>(this)->head == nullptr) return 0;
        return static_cast<u8>(jo::self<jo::Sms>(this)->head->replace);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::byte_184_8", 0x006e6010, &H::byte_184_8);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Sms::construct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::ctor_6e4e20>(this)); }  // 0x6e4e20
auto joan::orig::Panel::reset_6e50c0() -> void { joan::bind::member<&::game_b94_08::H::panel_reset_6e50c0>(this); }  // 0x6e50c0
auto joan::orig::Sms::add(const char* a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_ui1::H::sms_add>(this, a0, a1, a2); }  // 0x6e5700
auto joan::orig::Sms::pop_head() -> void { joan::bind::member<&::game_ui2::H::sms_pop_head>(this); }  // 0x6e5d00
auto joan::orig::Sms::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::repeat_step>(this)); }  // 0x6e5e00
auto joan::orig::Sms::show(std::int32_t a0) -> void { joan::bind::member<&::game_ui2_2::H::sms_show>(this, a0); }  // 0x6e5e60
auto joan::orig::Sms::load_sound() -> void { joan::bind::member<&::game_ui6::H::load_sound>(this); }  // 0x6e5f50
auto joan::orig::Sms::free_sound() -> void { joan::bind::member<&::game_b93_06::H::free1>(this); }  // 0x6e5fc0
auto joan::orig::Sms::head_flag() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b100_16::H::byte_184_8>(this)); }  // 0x6e6010
