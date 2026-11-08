// game/unknown_scrlbar.cpp: functions whose original source file is unknown (formerly game/_unplaced/scrlbar.cpp;
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

#include "../lib/crt/crt.hpp"
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

using u8 = std::uint8_t;
using u16 = std::uint16_t;

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::img::str;

char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

using i8 = std::int8_t;

using joan::img::obj;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b101_16.cpp (game/b101_16)
namespace {
namespace game_b101_16 {

struct H {
    // ------------------------------------------------------------------ 0x00761720
    void* ctor_761720() {
        joan::orig::as_text_box(&jo::self<jo::ScrollBar>(this)->vbar)->construct();   // 0x0062c610
        joan::orig::as_slider(&jo::self<jo::ScrollBar>(this)->hbar)->construct();  // 0x0062b6c0
        jo::view<u16>(jo::self<jo::ScrollBar>(this)->mode) = 0xffff;
        jo::self<jo::ScrollBar>(this)->has_thumb = 0;
        jo::self<jo::ScrollBar>(this)->has_track = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x007617c0
    std::uintptr_t dtor_7617c0() {
        joan::orig::as_scroll_bar(this)->reset();              // 0x00762880
        joan::orig::as_slider(&jo::self<jo::ScrollBar>(this)->hbar)->destruct();  // 0x0062b7d0
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_scroll(&jo::self<jo::ScrollBar>(this)->vbar)->destruct());  // 0x0062c720 (eax left as is)

    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_16", name, addr, fn)
B("ModeList::ctor_761720", 0x00761720, &H::ctor_761720);
B("ModeList::dtor_7617c0", 0x007617c0, &H::dtor_7617c0);
#undef B

}  // namespace game_b101_16
}  // namespace

// ---------------------------------------- from modules/game_b102_16.cpp (game/b102_16)
namespace {
namespace game_b102_16 {

using joan::call::cdecl_call;


using joan::orig::helpers::op_new;  // operator new

// new(n) + default ctor, null when new fails (SEH guard of the original not needed).
void* new_620ee0() {
    void* const m = op_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
    return m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(m)->ctor_620ee0()) : nullptr;  // 0x00620ee0
}

struct Args27 {
    u32 v[27];
};
static_assert(sizeof(Args27) == 0x6c, "Args27");

JOAN_COUNTER(c_setup_slot, "Slot::setup");

struct H {
    // ------------------------------------------------------------------------ 0x00761840
    u32 set_27(Args27 a) {
        for (i32 k = 0; k < 27; ++k) jo::view<u32>(jo::self<jo::ScrollBar>(this)->metrics[k]) = a.v[k];
        return a.v[25];  // eax = [ebp+0x6c]
    }

    // --------------------------------------------------- 0x00762120 / 0x007622c0 / 0x00762460
    i32 setup_slot(jo::SbButton& sb, u32 x, u32 y, u32 c, u32 d, const char* s1, const char* s2,
                   const char* s3) {
        c_setup_slot.hit();
        sb.x0 = static_cast<i32>(static_cast<i16>(x));
        sb.y0 = static_cast<i32>(static_cast<i16>(y));
        sb.x1 = static_cast<i32>(static_cast<i16>(c));
        sb.y1 = static_cast<i32>(static_cast<i16>(d));
        crt_strcpy(sb.img_out, s1);
        crt_strcpy(sb.img_over, s2);
        crt_strcpy(sb.img_down, s3);
        jo::view<void*>(sb.button) = new_620ee0();
        // push edx (dx = x, upper = this), push ecx (cx = y, upper = s1 pointer)
        const u32 ax = x & 0xffffu;  // x64: the original's upper word was `this` (eax leftover), setup() reads a word (arg::i16)
        const u32 ay = y & 0xffffu;  // (upper word: the s1 pointer)
        joan::orig::as_menu_button(sb.button)->setup(joan::orig::arg::i16(ax), joan::orig::arg::i16(ay), joan::orig::arg::mptr(jo::self<jo::ScrollBar>(this)->owner), joan::orig::arg::str(s1), joan::orig::arg::str(s2), joan::orig::arg::str(s3), joan::orig::arg::str(// 0x006211d0
                      0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)));
        joan::orig::as_widget(sb.button)->reset_child();  // 0x0062b660
        jo::view<u8>(sb.button->fit) = 0;
        return 1;
    }
    i32 setup_762120(u32 x, u32 y, u32 c, u32 d, const char* s1, const char* s2, const char* s3) {
        return setup_slot(jo::self<jo::ScrollBar>(this)->button_0, x, y, c, d, s1, s2, s3);
    }
    i32 setup_7622c0(u32 x, u32 y, u32 c, u32 d, const char* s1, const char* s2, const char* s3) {
        return setup_slot(jo::self<jo::ScrollBar>(this)->button_1, x, y, c, d, s1, s2, s3);
    }
    i32 setup_762460(u32 x, u32 y, u32 c, u32 d, const char* s1, const char* s2, const char* s3) {
        return setup_slot(jo::self<jo::ScrollBar>(this)->button_2, x, y, c, d, s1, s2, s3);
    }

    // ------------------------------------------------------------------------ 0x00763c20
    i32 current_763c20() {
        const i32 k = static_cast<i32>(jo::self<jo::ScrollBar>(this)->mode);
        if (k == -1) return -1;
        if (k == 0) return jo::self<jo::ScrollBar>(this)->hbar.pos;
        if (k == 1) return jo::self<jo::ScrollBar>(this)->vbar.pos;
        return -1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_16", name, addr, fn)
B("Obj::set_27", 0x00761840, &H::set_27);
B("Slot::setup_762120", 0x00762120, &H::setup_762120);
B("Slot::setup_7622c0", 0x007622c0, &H::setup_7622c0);
B("Slot::setup_762460", 0x00762460, &H::setup_762460);
B("Obj::current_763c20", 0x00763c20, &H::current_763c20);
#undef B

}  // namespace game_b102_16
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

struct S27 {
    u32 d[27];
};
static_assert(sizeof(S27) == 0x6c, "S27");

JOAN_COUNTER(c_set_27, "X::set_27");

struct H {
    u32 set_27(S27 v) {  // 0x00761990
        c_set_27.hit();
        for (i32 i = 0; i < 27; ++i) jo::view<u32>(jo::self<jo::ScrollBar>(this)->metrics[i]) = v.d[i];
        return v.d[25];  // last eax load in the original: mov eax,[ebp+0x6c]
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::set_27", 0x00761990, &H::set_27);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b104_16.cpp (game/b104_16)
namespace {
namespace game_b104_16 {

JOAN_COUNTER(c_build, "ScrollBar::build");

void game_strcpy(char* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }  // strcpy

struct H {
    // ------------------------------------------------------------------ 0x00761ae0
    i32 build(jo::BmpRes* a1, void* a2, i32 vertical, i32 x, i32 y, i32 a6, i32 a7, i32 a8, i32 a9) {  // a2: the texture handle stored at +4 (a dword on x86)
        c_build.hit();
        char b14[16], b24[16], b34[16], b44[16], b54[16], b64[16];
        const char* bull = str(d::s_sb_bull);  // 'sb_bull'
        if (vertical == 0) {
            game_strcpy(b14, str(d::s_sblt_out));  // 'sblt_out'
            game_strcpy(b24, str(d::s_sblt_ov));  // 'sblt_ov'
            game_strcpy(b34, str(d::s_sblt_dw));  // 'sblt_dw'
            game_strcpy(b44, str(d::s_sbrt_out));  // 'sbrt_out'
            game_strcpy(b54, str(d::s_sbrt_ov));  // 'sbrt_ov'
            game_strcpy(b64, str(d::s_sbrt_dw));  // 'sbrt_dw'
            joan::orig::as_scroll_bar(this)->init(a1, a2, joan::orig::arg::u16(i32{0}));  // 0x007620c0

            i32 vx = x + jo::self<jo::ScrollBar>(this)->metrics[10] + jo::self<jo::ScrollBar>(this)->metrics[2];
            i32 vy = y + jo::self<jo::ScrollBar>(this)->metrics[11] + jo::self<jo::ScrollBar>(this)->metrics[3];
            joan::orig::as_slot(this)->setup_762120(joan::orig::arg::u32(vx), joan::orig::arg::u32(vy), joan::orig::arg::u32(vx + jo::self<jo::ScrollBar>(this)->metrics[0]), joan::orig::arg::u32(vy + jo::self<jo::ScrollBar>(this)->metrics[1]), joan::orig::arg::str(// 0x00762120
                          static_cast<char*>(b14)), joan::orig::arg::str(static_cast<char*>(b34)), joan::orig::arg::str(static_cast<char*>(b24)));
            vx = x + jo::self<jo::ScrollBar>(this)->metrics[12] + jo::self<jo::ScrollBar>(this)->metrics[2];
            vy = y + jo::self<jo::ScrollBar>(this)->metrics[13] + jo::self<jo::ScrollBar>(this)->metrics[3];
            joan::orig::as_slot(this)->setup_7622c0(joan::orig::arg::u32(vx), joan::orig::arg::u32(vy), joan::orig::arg::u32(vx + jo::self<jo::ScrollBar>(this)->metrics[0]), joan::orig::arg::u32(vy + jo::self<jo::ScrollBar>(this)->metrics[1]), joan::orig::arg::str(// 0x007622c0
                          static_cast<char*>(b44)), joan::orig::arg::str(static_cast<char*>(b64)), joan::orig::arg::str(static_cast<char*>(b54)));
            vx = x + jo::self<jo::ScrollBar>(this)->metrics[8] + jo::self<jo::ScrollBar>(this)->metrics[6];
            vy = y + jo::self<jo::ScrollBar>(this)->metrics[9] + jo::self<jo::ScrollBar>(this)->metrics[7];
            joan::orig::as_slot(this)->setup_762460(joan::orig::arg::u32(vx), joan::orig::arg::u32(vy), joan::orig::arg::u32(vx + jo::self<jo::ScrollBar>(this)->metrics[4]), joan::orig::arg::u32(vy + jo::self<jo::ScrollBar>(this)->metrics[5]), joan::orig::arg::str(// 0x00762460
                          bull), joan::orig::arg::str(bull), joan::orig::arg::str(bull));
            vx = x + jo::self<jo::ScrollBar>(this)->metrics[14];
            vy = y + jo::self<jo::ScrollBar>(this)->metrics[15];
            joan::orig::as_scroll_bar(this)->set_thumb(joan::orig::arg::i16(vx), joan::orig::arg::i16(vy), joan::orig::arg::i16(vx), joan::orig::arg::i16(vy), str(d::s_sb_con));  // 0x00762600 'sb_con'
            joan::orig::as_scroll_bar(this)->set_arrows(joan::orig::arg::i16(// 0x00762690
                          x + jo::self<jo::ScrollBar>(this)->metrics[16]), joan::orig::arg::i16(y + jo::self<jo::ScrollBar>(this)->metrics[17]), joan::orig::arg::i16(x + jo::self<jo::ScrollBar>(this)->metrics[16] + jo::self<jo::ScrollBar>(this)->metrics[18]), joan::orig::arg::i16(y + jo::self<jo::ScrollBar>(this)->metrics[19] + jo::self<jo::ScrollBar>(this)->metrics[22]), str(d::s_sbhb_lt), str(d::s_sbhb_ce), str(d::s_sbhb_rt), joan::orig::arg::u32(jo::self<jo::ScrollBar>(this)->metrics[21]), joan::orig::arg::u32(jo::self<jo::ScrollBar>(this)->metrics[22]));  // sbhb_lt / ce / rt
            vx = x + jo::self<jo::ScrollBar>(this)->metrics[23];
            vy = y + jo::self<jo::ScrollBar>(this)->metrics[7];
            joan::orig::as_scroll_bar(this)->setup_track(joan::orig::arg::i16(vx), joan::orig::arg::i16(vy), joan::orig::arg::i16(vx + jo::self<jo::ScrollBar>(this)->metrics[24]), joan::orig::arg::i16(vy + jo::self<jo::ScrollBar>(this)->metrics[5]), joan::orig::arg::u32(a6), joan::orig::arg::u32(a7), joan::orig::arg::u32(a8), joan::orig::arg::u32(a9));  // 0x00762770
        } else {
            game_strcpy(b14, str(d::s_sbup_out));  // 'sbup_out'
            game_strcpy(b24, str(d::s_sbup_ov));  // 'sbup_ov'
            game_strcpy(b34, str(d::s_sbup_dw));  // 'sbup_dw'
            game_strcpy(b44, str(d::s_sbdw_out));  // 'sbdw_out'
            game_strcpy(b54, str(d::s_sbdw_ov));  // 'sbdw_ov'
            game_strcpy(b64, str(d::s_sbdw_dw));  // 'sbdw_dw'
            joan::orig::as_scroll_bar(this)->init(a1, a2, joan::orig::arg::u16(i32{1}));  // 0x007620c0

            i32 vx = x + jo::self<jo::ScrollBar>(this)->metrics[10] + jo::self<jo::ScrollBar>(this)->metrics[2];
            i32 vy = y + jo::self<jo::ScrollBar>(this)->metrics[11] + jo::self<jo::ScrollBar>(this)->metrics[3];
            joan::orig::as_slot(this)->setup_762120(joan::orig::arg::u32(vx), joan::orig::arg::u32(vy), joan::orig::arg::u32(vx + jo::self<jo::ScrollBar>(this)->metrics[0]), joan::orig::arg::u32(vy + jo::self<jo::ScrollBar>(this)->metrics[1]), joan::orig::arg::str(// 0x00762120
                          static_cast<char*>(b14)), joan::orig::arg::str(static_cast<char*>(b34)), joan::orig::arg::str(static_cast<char*>(b24)));
            vx = x + jo::self<jo::ScrollBar>(this)->metrics[12] + jo::self<jo::ScrollBar>(this)->metrics[2];
            vy = y + jo::self<jo::ScrollBar>(this)->metrics[13] + jo::self<jo::ScrollBar>(this)->metrics[3];
            joan::orig::as_slot(this)->setup_7622c0(joan::orig::arg::u32(vx), joan::orig::arg::u32(vy), joan::orig::arg::u32(vx + jo::self<jo::ScrollBar>(this)->metrics[0]), joan::orig::arg::u32(vy + jo::self<jo::ScrollBar>(this)->metrics[1]), joan::orig::arg::str(// 0x007622c0
                          static_cast<char*>(b44)), joan::orig::arg::str(static_cast<char*>(b64)), joan::orig::arg::str(static_cast<char*>(b54)));
            vx = x + jo::self<jo::ScrollBar>(this)->metrics[8] + jo::self<jo::ScrollBar>(this)->metrics[6];
            vy = y + jo::self<jo::ScrollBar>(this)->metrics[9] + jo::self<jo::ScrollBar>(this)->metrics[7];
            joan::orig::as_slot(this)->setup_762460(joan::orig::arg::u32(vx), joan::orig::arg::u32(vy), joan::orig::arg::u32(vx + jo::self<jo::ScrollBar>(this)->metrics[4]), joan::orig::arg::u32(vy + jo::self<jo::ScrollBar>(this)->metrics[5]), joan::orig::arg::str(// 0x00762460
                          bull), joan::orig::arg::str(bull), joan::orig::arg::str(bull));
            joan::orig::as_scroll_bar(this)->set_arrows(joan::orig::arg::i16(// 0x00762690
                          x + jo::self<jo::ScrollBar>(this)->metrics[16]), joan::orig::arg::i16(y + jo::self<jo::ScrollBar>(this)->metrics[17]), joan::orig::arg::i16(x + jo::self<jo::ScrollBar>(this)->metrics[16] + jo::self<jo::ScrollBar>(this)->metrics[22]), joan::orig::arg::i16(y + jo::self<jo::ScrollBar>(this)->metrics[17] + jo::self<jo::ScrollBar>(this)->metrics[19]), str(d::s_sbvb_top), str(d::s_sbvb_ce), str(d::s_sbvb_dw), joan::orig::arg::u32(jo::self<jo::ScrollBar>(this)->metrics[21]), joan::orig::arg::u32(jo::self<jo::ScrollBar>(this)->metrics[22]));  // sbvb_top / ce / dw
            vx = x + jo::self<jo::ScrollBar>(this)->metrics[8] + jo::self<jo::ScrollBar>(this)->metrics[6];
            vy = y + jo::self<jo::ScrollBar>(this)->metrics[17];
            joan::orig::as_scroll_bar(this)->setup_track(joan::orig::arg::i16(vx), joan::orig::arg::i16(vy), joan::orig::arg::i16(vx + jo::self<jo::ScrollBar>(this)->metrics[4]), joan::orig::arg::i16(vy + jo::self<jo::ScrollBar>(this)->metrics[26]), joan::orig::arg::u32(a6), joan::orig::arg::u32(a7), joan::orig::arg::u32(a8), joan::orig::arg::u32(a9));  // 0x00762770
        }
        return 1;
    }
};

#define B104(name, addr, fn) JOAN_REPLACE("game/b104_16", name, addr, fn)
B104("ScrollBar::build", 0x00761ae0, &H::build);
#undef B104

}  // namespace game_b104_16
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

JOAN_COUNTER(c_set_label, "Obj::set_label_762690");
JOAN_COUNTER(c_setup_762770, "Obj::setup_762770");
JOAN_COUNTER(c_reset_762880, "Obj::reset_762880");

struct H {
    // Button release: optional pre-call, scalar deleting dtor(1) when non-null, button = 0.
    void release(jo::SbButton& sb, std::uintptr_t pre, std::uintptr_t del) {
        if (sb.button == nullptr) return;
        if (pre != 0) thiscall<u32>(pre, sb.button);
        void* const p = sb.button;
        if (p != nullptr) thiscall<u32>(del, p, 1u);
        sb.button = nullptr;
    }

    // ------------------------------------------------------------------ 0x007620c0
    i32 init_7620c0(jo::BmpRes* a, void* b, u16 c) {
        jo::self<jo::ScrollBar>(this)->owner = a;
        jo::self<jo::ScrollBar>(this)->texture = static_cast<jo::Texture*>(b);
        jo::view<u16>(jo::self<jo::ScrollBar>(this)->mode) = c;
        jo::self<jo::ScrollBar>(this)->u32_19a = 0;
        return 1;
    }

    // ------------------------------------------------------------------ 0x00762690
    i32 set_label_762690(i16 a, i16 b, i16 c, i16 d, const char* s1, const char* s2, const char* s3, u32 x,
                         u32 y) {
        c_set_label.hit();
        jo::self<jo::ScrollBar>(this)->track_rect[0] = a;
        jo::self<jo::ScrollBar>(this)->track_rect[1] = b;
        jo::self<jo::ScrollBar>(this)->track_rect[2] = c;
        jo::self<jo::ScrollBar>(this)->track_rect[3] = d;
        crt_strcpy(&jo::self<jo::ScrollBar>(this)->track_img[0], s1);
        crt_strcpy(&jo::self<jo::ScrollBar>(this)->track_img[16], s2);
        crt_strcpy(&jo::self<jo::ScrollBar>(this)->track_img[32], s3);
        jo::self<jo::ScrollBar>(this)->track_x = x;
        jo::self<jo::ScrollBar>(this)->track_y = y;
        jo::self<jo::ScrollBar>(this)->has_track = 1;
        return 1;
    }

    // ------------------------------------------------------------------ 0x00762770
    i32 setup_762770(i16 a, i16 b, i16 c, i16 d, u32 e, u32 g, u32 h, u32 k) {
        c_setup_762770.hit();
        const i32 mode = jo::self<jo::ScrollBar>(this)->mode;
        if (mode == 1) {
            void* const sub = &jo::self<jo::ScrollBar>(this)->vbar;
            joan::orig::as_text_box(sub)->setup(joan::orig::arg::u16(static_cast<i32>(a)), joan::orig::arg::u16(static_cast<i32>(b)), joan::orig::arg::u16(static_cast<i32>(c)), joan::orig::arg::u16(static_cast<i32>(d)), joan::orig::arg::u32(e), fn::code_value(fn::Key_filter_763c90), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));  // 0x0062c750
            joan::orig::as_scroll(sub)->set_62cd40(joan::orig::arg::u32(g), joan::orig::arg::u32(h), joan::orig::arg::u32(k));                          // 0x0062cd40
            jo::view<u8>(jo::self<jo::ScrollBar>(this)->vbar.fit) = 0;
        } else if (mode == 0) {
            void* const sub = &jo::self<jo::ScrollBar>(this)->hbar;
            joan::orig::as_text_box(sub)->init(joan::orig::arg::i16(static_cast<i32>(a)), joan::orig::arg::i16(static_cast<i32>(b)), joan::orig::arg::i16(static_cast<i32>(c)), joan::orig::arg::i16(static_cast<i32>(d)), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(e))), fn::code_value(fn::Key_filter_763cd0), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));                                                // 0x0062b8d0
            joan::orig::as_scroll(sub)->set_62be10(joan::orig::arg::u32(g), joan::orig::arg::u32(h), joan::orig::arg::u32(k));                          // 0x0062be10
            jo::view<u8>(jo::self<jo::ScrollBar>(this)->hbar.fit) = 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00762880
    i32 reset_762880() {
        c_reset_762880.hit();
        jo::self<jo::ScrollBar>(this)->owner = nullptr;
        jo::self<jo::ScrollBar>(this)->texture = nullptr;
        const i32 mode = jo::self<jo::ScrollBar>(this)->mode;
        if (mode == 1)
            joan::orig::as_scroll(&jo::self<jo::ScrollBar>(this)->vbar)->reset_8e();  // 0x0062c8f0
        else if (mode == 0)
            joan::orig::as_text_box(&jo::self<jo::ScrollBar>(this)->hbar)->reset_62b9a0();  // 0x0062b9a0
        release(jo::self<jo::ScrollBar>(this)->button_0, 0, fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));  // 0x006b33a0
        release(jo::self<jo::ScrollBar>(this)->button_1, 0, fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        release(jo::self<jo::ScrollBar>(this)->button_2, 0, fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::init_7620c0", 0x007620c0, &H::init_7620c0);
B("Obj::set_label_762690", 0x00762690, &H::set_label_762690);
B("Obj::setup_762770", 0x00762770, &H::setup_762770);
B("Obj::reset_762880", 0x00762880, &H::reset_762880);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

struct H {
    // ------------------------------------------------------------------ 0x00762600
    i32 set_label_762600(i16 a, i16 b, i16 c, i16 d, const char* s) {
        jo::self<jo::ScrollBar>(this)->thumb_rect[0] = a;
        jo::self<jo::ScrollBar>(this)->thumb_rect[1] = b;
        jo::self<jo::ScrollBar>(this)->thumb_rect[2] = c;
        jo::self<jo::ScrollBar>(this)->thumb_rect[3] = d;
        crt_strcpy(jo::self<jo::ScrollBar>(this)->thumb_img, s);
        jo::self<jo::ScrollBar>(this)->has_thumb = 1;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::set_label_762600", 0x00762600, &H::set_label_762600);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b107_10.cpp (game/b107_10)
namespace {
namespace game_b107_10 {

using joan::hook::global;

JOAN_COUNTER(c_scroller_step, "Scroller::step_762a00");

struct H {
    // ------------------------------------------------------------------ 0x00762a00
    // f = f_num / (f_den / (r - +0xeb)) at PC24, then clamp(+0xeb + rnd(f), +0xeb, r).
    i32 drag_target(float num, float den) {
        const Fpu fpu{true};
        const i32 r1 = joan::orig::arg::as<i32>(joan::orig::as_edit_box(&jo::self<jo::ScrollBar>(this)->vbar)->bottom_line());  // 0x0062d480
        const i32 n = r1 - jo::self<jo::ScrollBar>(this)->vbar.min;
        const double t = fpu.div(static_cast<double>(den), static_cast<double>(n));
        const float f = static_cast<float>(fpu.div(static_cast<double>(num), t));
        const i32 r2 = joan::orig::arg::as<i32>(joan::orig::as_edit_box(&jo::self<jo::ScrollBar>(this)->vbar)->bottom_line());  // 0x0062d480
        const i32 lo = jo::self<jo::ScrollBar>(this)->vbar.min;
        const i32 v = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(f))));  // 0x00590a20
        const i32 x = jo::self<jo::ScrollBar>(this)->vbar.min + v;
        return joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->limit_int(joan::orig::arg::i32(x), joan::orig::arg::i32(lo), joan::orig::arg::i32(r2)));  // 0x00590aa0
    }

    static i32 rate(i32 n) {
        const Fpu fpu{true};
        return joan::hook::x87::ftol(fpu.div(1000.0, fpu.mul(static_cast<double>(n), 0.4)));
    }

    // Auto-repeat of a button (+0x1a2 previous: dir -1, +0x1e6 next: dir +1).
    // Returns 1 (handled), or 0 when the mode is neither 0 nor 1 after the delay elapsed.
    i32 repeat_step(i32 dir) {
        jo::self<jo::ScrollBar>(this)->u32_19a = jo::self<jo::ScrollBar>(this)->u32_19a + global<u32>(d::g_frame_elapsed_ms);
        const i32 mode = jo::self<jo::ScrollBar>(this)->mode;
        if (mode == 1) {
            const i32 lim = rate(jo::self<jo::ScrollBar>(this)->vbar.max - jo::self<jo::ScrollBar>(this)->vbar.min);
            if (!(jo::view<i32>(jo::self<jo::ScrollBar>(this)->u32_19a) < lim)) {
                joan::orig::as_edit_box(&jo::self<jo::ScrollBar>(this)->vbar)->set_top(joan::orig::arg::i32(jo::self<jo::ScrollBar>(this)->vbar.pos + dir));  // 0x0062cd80
                jo::view<i32>(jo::self<jo::ScrollBar>(this)->u32_19a) = jo::view<i32>(jo::self<jo::ScrollBar>(this)->u32_19a) - lim;
            }
            return 1;
        }
        if (mode == 0) {
            const i32 lim = rate(jo::self<jo::ScrollBar>(this)->hbar.max - jo::self<jo::ScrollBar>(this)->hbar.min);
            if (!(jo::view<i32>(jo::self<jo::ScrollBar>(this)->u32_19a) < lim)) {
                joan::orig::as_scroll(&jo::self<jo::ScrollBar>(this)->hbar)->set_pos(joan::orig::arg::i32(jo::self<jo::ScrollBar>(this)->hbar.pos + dir));  // 0x0062be50
                jo::view<i32>(jo::self<jo::ScrollBar>(this)->u32_19a) = jo::view<i32>(jo::self<jo::ScrollBar>(this)->u32_19a) - lim;
            }
            return 1;
        }
        return 0;
    }

    // One button: 1 = handled (return 1), 0 = not handled, -1 = fell through (mode not 0/1).
    i32 button(jo::SbButton& sb, i32 dir) {
        void* const b = sb.button;
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(b)->mouse(i32{0}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
            const i32 mode = jo::self<jo::ScrollBar>(this)->mode;
            if (mode == 0) {
                joan::orig::as_scroll(&jo::self<jo::ScrollBar>(this)->hbar)->set_pos(joan::orig::arg::i32(jo::self<jo::ScrollBar>(this)->hbar.pos + dir));  // 0x0062be50
            } else if (mode == 1) {
                joan::orig::as_edit_box(&jo::self<jo::ScrollBar>(this)->vbar)->set_top(joan::orig::arg::i32(jo::self<jo::ScrollBar>(this)->vbar.pos + dir));  // 0x0062cd80
            }
            jo::self<jo::ScrollBar>(this)->u32_19a = 0;
            return 1;
        }
        void* const b2 = sb.button;
        if (jo::as_button(b2)->pressed == 0) return 0;
        const i32 a4 = sb.button->y2;
        const i32 a3 = sb.button->x2;
        const i32 a2 = sb.button->y;
        const i32 a1 = sb.button->x;
        if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4))) == 0) return 0;  // 0x00593980
        if (jo::self<jo::ScrollBar>(this)->u32_19e < 0x12cu) {
            jo::self<jo::ScrollBar>(this)->u32_19e = jo::self<jo::ScrollBar>(this)->u32_19e + global<u32>(d::g_frame_elapsed_ms);
            return 1;
        }
        return repeat_step(dir) != 0 ? 1 : -1;
    }

    // Mode-1 drag block; returns the flag ([ebp-0x8]) at 0x00762e2e.
    i32 drag_block() {
        const i32 d = jo::self<jo::ScrollBar>(this)->button_2.y1 - jo::self<jo::ScrollBar>(this)->button_2.y0;
        i16 w10 = jo::self<jo::ScrollBar>(this)->vbar.x;
        i16 w14 = static_cast<i16>(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.y) - d / 2);
        i16 w18 = jo::self<jo::ScrollBar>(this)->vbar.x2;
        i16 w1c = static_cast<i16>(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.y2) + d / 2);
        float f20 = 1.0f;
        float f24 = 1.0f;
        if (jo::self<jo::ScrollBar>(this)->vbar.fit != 0) {
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(// 0x00633e60
                          static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.x)), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.y)), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.x2)), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.y2)), &f20, &f24, &w10, &w14, &w18, &w1c, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
        }
        i32 flag = 0;
        if (jo::view<i8>(jo::self<jo::ScrollBar>(this)->vbar.dragging) == 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_mode(static_cast<i32>(w10), static_cast<i32>(w14), static_cast<i32>(w18), static_cast<i32>(w1c), i32{0})) != 0) {  // 0x005940f0
                const i16 s28 = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_table(obj(d::g_mouse))->at_676(i32{0})));  // 0x0043c3f0
                if (static_cast<i32>(s28) >= jo::self<jo::ScrollBar>(this)->button_2.button->y &&
                    static_cast<i32>(s28) <= jo::self<jo::ScrollBar>(this)->button_2.button->y2) {
                    jo::self<jo::ScrollBar>(this)->vbar.dragging = 1;
                    jo::view<i16>(jo::self<jo::ScrollBar>(this)->vbar.drag_x) = s28;
                    jo::self<jo::ScrollBar>(this)->vbar.drag_pos = jo::view<u32>(jo::self<jo::ScrollBar>(this)->vbar.pos);
                    flag = 2;
                } else {
                    const i32 hi = jo::self<jo::ScrollBar>(this)->button_2.button->y2;
                    const i32 lo = jo::self<jo::ScrollBar>(this)->button_2.button->y;
                    const i16 s2c = static_cast<i16>(static_cast<i32>(s28) - (hi - lo) / 2);
                    const float f50 = static_cast<float>(static_cast<i32>(s2c) - jo::self<jo::ScrollBar>(this)->vbar.y);
                    const float f58 = static_cast<float>(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.y2) - jo::self<jo::ScrollBar>(this)->vbar.y);
                    jo::self<jo::ScrollBar>(this)->vbar.pos = drag_target(f50, f58);
                    jo::self<jo::ScrollBar>(this)->vbar.dragging = 1;
                    jo::view<i16>(jo::self<jo::ScrollBar>(this)->vbar.drag_x) = s28;
                    jo::self<jo::ScrollBar>(this)->vbar.drag_pos = jo::view<u32>(jo::self<jo::ScrollBar>(this)->vbar.pos);
                }
                flag = 1;
            }
        } else {
            if (global<u32>(d::g_mouse_left_down) == 0) jo::self<jo::ScrollBar>(this)->vbar.dragging = 0;
            const i32 hi = jo::self<jo::ScrollBar>(this)->button_2.button->y2;
            const i32 lo = jo::self<jo::ScrollBar>(this)->button_2.button->y;
            const i32 i38 = global<i32>(d::g_mouse_y) - (hi - lo) / 2;
            jo::self<jo::ScrollBar>(this)->vbar.drag_pos = jo::view<u32>(jo::self<jo::ScrollBar>(this)->vbar.pos);
            const float f64 = static_cast<float>(i38 - jo::self<jo::ScrollBar>(this)->vbar.y);
            const float f6c = static_cast<float>(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.y2) - jo::self<jo::ScrollBar>(this)->vbar.y);
            jo::self<jo::ScrollBar>(this)->vbar.pos = drag_target(f64, f6c);
            flag = (jo::self<jo::ScrollBar>(this)->vbar.drag_pos != jo::view<u32>(jo::self<jo::ScrollBar>(this)->vbar.pos)) ? 1 : 2;
        }
        return flag;
    }

    i32 scroller_step() {
        c_scroller_step.hit();
        const i32 mode = jo::self<jo::ScrollBar>(this)->mode;
        if (mode == -1) return 0;
        bool check_list = false;
        if (mode == 0) {
            check_list = true;
        } else if (mode == 1) {
            i32 ok = 1;
            if (jo::view<i8>(jo::self<jo::ScrollBar>(this)->vbar.enabled) == 0) ok = 0;
            if (joan::orig::arg::as<i32>(joan::orig::as_button(&jo::self<jo::ScrollBar>(this)->vbar.child)->enabled_slot()) == 0) ok = 0;  // 0x006226e0
            if (jo::view<i8>(jo::self<jo::ScrollBar>(this)->vbar.pressed) == 0) ok = 0;
            if (ok == 1) {
                if (drag_block() > 0) return 1;
            }
            check_list = true;
        }
        if (check_list) {
            if (joan::orig::arg::as<i32>(joan::orig::as_text_box(&jo::self<jo::ScrollBar>(this)->hbar)->drag()) != 0) return 1;  // 0x0062b9e0
        }
        const i32 prev = button(jo::self<jo::ScrollBar>(this)->button_0, -1);
        if (prev == 1) return 1;
        const i32 next = button(jo::self<jo::ScrollBar>(this)->button_1, 1);
        if (next == 1) return 1;
        jo::self<jo::ScrollBar>(this)->u32_19e = 0;
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b107_10", name, addr, fn)
B("Scroller::step_762a00", 0x00762a00, &H::scroller_step);
#undef B

}  // namespace game_b107_10
}  // namespace

// ---------------------------------------- from modules/game_b115_09.cpp (game/b115_09)
namespace {
namespace game_b115_09 {

using joan::hook::x87::ftol;

// The low word of an i32 member, sign-extended (the original reads it with movsx word).
i32 low_sw(const i32& v) { return static_cast<i32>(*reinterpret_cast<const i16*>(&v)); }

JOAN_COUNTER(c_slots, "HudSlots::draw_763530");

// ======================================================================= 0x00763530
struct HSlots {
    u32 colour() {  // 0x005e9640(-1) on a stack temp, dword read back
        u32 tmp = 0;
        return *joan::orig::arg::as<u32*>(joan::orig::as_color(&tmp)->Color_dword(joan::orig::arg::u32(i32{-1})));
    }
    void sprite(i32 x, i32 y, i32 ctx, const char* name, u32 color, i32 a10, i32 a11, i32 a12, i32 a13, i32 a16, i32 a17) {
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(name), joan::orig::arg::mptr(jo::self<jo::ScrollBar>(this)->owner), joan::orig::arg::u32(color), 3, 0, 0, joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), -1, 0, joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));  // 0x00633500
    }
    u32 draw(i32 ctx, i32 /*unused*/) {
        c_slots.hit();
        if (static_cast<i32>(jo::self<jo::ScrollBar>(this)->mode) == -1) return 0;
        if (jo::view<i8>(jo::self<jo::ScrollBar>(this)->has_thumb) != 0) {
            const u32 c = colour();
            sprite(low_sw(jo::self<jo::ScrollBar>(this)->thumb_rect[0]), low_sw(jo::self<jo::ScrollBar>(this)->thumb_rect[1]), ctx, jo::self<jo::ScrollBar>(this)->thumb_img, c, 1, -1, -1, -1, -1, -1);
        }
        if (jo::view<i8>(jo::self<jo::ScrollBar>(this)->has_track) != 0) {
            const i32 mode = static_cast<i32>(jo::self<jo::ScrollBar>(this)->mode);
            if (mode == 0) {
                const u32 c1 = colour();
                sprite(low_sw(jo::self<jo::ScrollBar>(this)->track_rect[0]), low_sw(jo::self<jo::ScrollBar>(this)->track_rect[1]), ctx, jo::self<jo::ScrollBar>(this)->track_img, c1, 1, -1, -1, -1, -1, -1);
                const u32 c2 = colour();
                sprite(jo::self<jo::ScrollBar>(this)->track_rect[0] + jo::view<i32>(jo::self<jo::ScrollBar>(this)->track_x), low_sw(jo::self<jo::ScrollBar>(this)->track_rect[1]), ctx, jo::self<jo::ScrollBar>(this)->track_img + 0x10, c2, 1, 1, -1,
                       jo::self<jo::ScrollBar>(this)->track_rect[2] - jo::self<jo::ScrollBar>(this)->track_rect[0] + 1, jo::self<jo::ScrollBar>(this)->track_rect[2] - jo::view<i32>(jo::self<jo::ScrollBar>(this)->track_x), jo::self<jo::ScrollBar>(this)->track_rect[3] - 1);
                const u32 c3 = colour();
                sprite(jo::self<jo::ScrollBar>(this)->track_rect[2] - jo::view<i32>(jo::self<jo::ScrollBar>(this)->track_x), low_sw(jo::self<jo::ScrollBar>(this)->track_rect[1]), ctx, jo::self<jo::ScrollBar>(this)->track_img + 0x20, c3, 1, -1, -1, -1, -1, -1);
            } else if (mode == 1) {
                const u32 c1 = colour();
                sprite(low_sw(jo::self<jo::ScrollBar>(this)->track_rect[0]), low_sw(jo::self<jo::ScrollBar>(this)->track_rect[1]), ctx, jo::self<jo::ScrollBar>(this)->track_img, c1, 1, -1, -1, -1, -1, -1);
                const u32 c2 = colour();
                sprite(low_sw(jo::self<jo::ScrollBar>(this)->track_rect[0]), jo::self<jo::ScrollBar>(this)->track_rect[1] + jo::view<i32>(jo::self<jo::ScrollBar>(this)->track_y), ctx, jo::self<jo::ScrollBar>(this)->track_img + 0x10, c2, 1, -1, -1, -1,
                       jo::self<jo::ScrollBar>(this)->track_rect[0] + jo::view<i32>(jo::self<jo::ScrollBar>(this)->track_x), low_sw(jo::self<jo::ScrollBar>(this)->track_rect[3]));
                const u32 c3 = colour();
                sprite(low_sw(jo::self<jo::ScrollBar>(this)->track_rect[0]), jo::self<jo::ScrollBar>(this)->track_rect[3] - jo::view<i32>(jo::self<jo::ScrollBar>(this)->track_y), ctx, jo::self<jo::ScrollBar>(this)->track_img + 0x20, c3, 1, -1, -1, -1, -1, -1);
            }
        }
        joan::orig::as_panel(jo::self<jo::ScrollBar>(this)->button_0.button)->reset_6b7cf0();                     // 0x006b7cf0
        joan::orig::as_button(jo::self<jo::ScrollBar>(this)->button_0.button)->draw(joan::orig::arg::u32(ctx), 7);        // 0x00622cc0
        joan::orig::as_panel(jo::self<jo::ScrollBar>(this)->button_1.button)->reset_6b7cf0();
        joan::orig::as_button(jo::self<jo::ScrollBar>(this)->button_1.button)->draw(joan::orig::arg::u32(ctx), 7);
        const i32 dx = jo::self<jo::ScrollBar>(this)->button_2.x1 - jo::self<jo::ScrollBar>(this)->button_2.x0;  // [ebp-0x8]
        const i32 dy = jo::self<jo::ScrollBar>(this)->button_2.y1 - jo::self<jo::ScrollBar>(this)->button_2.y0;  // [ebp-0xc]
        const i32 mode = static_cast<i32>(jo::self<jo::ScrollBar>(this)->mode);
        const Fpu fpu{true};
        if (mode == 1) {
            joan::orig::as_button(&jo::self<jo::ScrollBar>(this)->vbar)->fire(static_cast<std::uint8_t>(joan::orig::arg::u32(i32{7})));  // 0x0062cc60
            if (jo::self<jo::ScrollBar>(this)->vbar.max >= jo::self<jo::ScrollBar>(this)->vbar.page) {
                const float span = static_cast<float>(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.y2) - static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.y));  // [ebp-0x60]
                const i32 pos = joan::orig::arg::as<i32>(joan::orig::as_edit_box(&jo::self<jo::ScrollBar>(this)->vbar)->bottom_line());              // 0x0062d480
                const float frac =
                    static_cast<float>(fpu.div(static_cast<double>(span), static_cast<double>(pos - jo::self<jo::ScrollBar>(this)->vbar.min)));
                const float off = static_cast<float>(fpu.mul(static_cast<double>(jo::self<jo::ScrollBar>(this)->vbar.pos), static_cast<double>(frac)));
                const double a = fpu.add(static_cast<double>(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.y)), static_cast<double>(off));
                const double b = fpu.mul(static_cast<double>(dy), 0.5);
                const i32 y = ftol(fpu.sub(a, b));
                joan::orig::as_button(jo::self<jo::ScrollBar>(this)->button_2.button)->set_rect(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::ScrollBar>(this)->vbar.x)), joan::orig::arg::u32(y), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{0}));  // 0x00623580
                joan::orig::as_panel(jo::self<jo::ScrollBar>(this)->button_2.button)->reset_6b7cf0();                                         // 0x006b7cf0
                joan::orig::as_button(jo::self<jo::ScrollBar>(this)->button_2.button)->draw(joan::orig::arg::u32(ctx), 7);                             // 0x00622cc0
            }
        } else if (mode == 0) {
            joan::orig::as_widget(&jo::self<jo::ScrollBar>(this)->hbar)->try_activate(static_cast<char>(joan::orig::arg::i32(i32{7})));  // 0x0062bd30
            double v = static_cast<double>(static_cast<i32>(jo::self<jo::ScrollBar>(this)->hbar.x2) - static_cast<i32>(jo::self<jo::ScrollBar>(this)->hbar.x));
            v = fpu.div(v, static_cast<double>(jo::self<jo::ScrollBar>(this)->hbar.max - jo::self<jo::ScrollBar>(this)->hbar.min + 1));
            v = fpu.mul(v, static_cast<double>(jo::self<jo::ScrollBar>(this)->hbar.pos));
            const float off = static_cast<float>(v);  // [ebp-0x18]
            const i32 y = static_cast<i32>(jo::self<jo::ScrollBar>(this)->hbar.y);
            const double a = fpu.add(static_cast<double>(static_cast<i32>(jo::self<jo::ScrollBar>(this)->hbar.x)), static_cast<double>(off));
            const double b = fpu.mul(static_cast<double>(dx), 0.6);
            const i32 x = ftol(fpu.sub(a, b));
            joan::orig::as_button(jo::self<jo::ScrollBar>(this)->button_2.button)->set_rect(joan::orig::arg::u32(x), joan::orig::arg::u32(y), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{0}));  // 0x00623580
            joan::orig::as_panel(jo::self<jo::ScrollBar>(this)->button_2.button)->reset_6b7cf0();                                  // 0x006b7cf0
            joan::orig::as_button(jo::self<jo::ScrollBar>(this)->button_2.button)->draw(joan::orig::arg::u32(ctx), 7);                      // 0x00622cc0
        }
        return 1;
    }
};

static_assert(sizeof(i16) == 2 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b115_09", name, addr, fn)
B("HudSlots::draw_763530", 0x00763530, &HSlots::draw);
#undef B

}  // namespace game_b115_09
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

i32 JOAN_CDECL key_filter(u32, u32 flags) { return (flags & 0xff & 2) != 0 ? 1 : 0; }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Key::filter_763c90", 0x00763c90, &key_filter);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ScrollBar::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b101_16::H::ctor_761720>(this)); }  // 0x761720
auto joan::orig::ScrollBar::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_16::H::dtor_7617c0>(this)); }  // 0x7617c0
auto joan::orig::ScrollBar::build(BmpRes* a0, void* a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_16::H::build>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x761ae0
auto joan::orig::ScrollBar::init(BmpRes* a0, void* a1, std::uint16_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_16::H::init_7620c0>(this, a0, a1, a2)); }  // 0x7620c0
auto joan::orig::Slot::setup_762120(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, const char* a4, const char* a5, const char* a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_16::H::setup_762120>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x762120
auto joan::orig::Slot::setup_7622c0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, const char* a4, const char* a5, const char* a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_16::H::setup_7622c0>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x7622c0
auto joan::orig::Slot::setup_762460(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, const char* a4, const char* a5, const char* a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_16::H::setup_762460>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x762460
auto joan::orig::ScrollBar::set_thumb(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, const char* a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_10::H::set_label_762600>(this, a0, a1, a2, a3, a4)); }  // 0x762600
auto joan::orig::ScrollBar::set_arrows(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, const char* a4, const char* a5, const char* a6, std::uint32_t a7, std::uint32_t a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_16::H::set_label_762690>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x762690
auto joan::orig::ScrollBar::setup_track(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_16::H::setup_762770>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x762770
auto joan::orig::ScrollBar::reset() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_16::H::reset_762880>(this)); }  // 0x762880
auto joan::orig::ScrollBar::update() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b107_10::H::scroller_step>(this)); }  // 0x762a00
