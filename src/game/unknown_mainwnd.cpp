// game/unknown_mainwnd.cpp: functions whose original source file is unknown (formerly game/_unplaced/mainwnd.cpp;
// re/attribution.csv: between ww/speech_s.cpp and ww/t_scrres.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/records.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::cdecl_call;

using joan::img::obj;
using joan::img::str;

using joan::orig::helpers::at;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b107_16.cpp (game/b107_16)
namespace {
namespace game_b107_16 {

using u32 = std::uint32_t;

// ---- static initialiser pairs (direct calls, no thunks) ----
template <fn::Code Ctor, fn::Code Reg>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(Ctor);
    return cdecl_call<u32>(Reg);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_16", name, addr, fn)
B("init_4d6a70", 0x004d6a70, (&init_pair<fn::init_4d6a90, fn::init_4d6ab0>));
#undef B

}  // namespace game_b107_16
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_demo_4d7ba0, "Music::init_demo_4d7ba0");

std::uintptr_t JOAN_CDECL init_4d6a90() { return joan::orig::arg::as<u32>(joan::orig::as_video_capture(obj(d::g_video_capture))->construct()); }  // 0x006862d0
std::uintptr_t JOAN_CDECL dtor_4d6ae0() { return joan::orig::arg::as<u32>(joan::orig::as_video_capture(obj(d::g_video_capture))->shutdown()); }  // 0x00686320
std::uintptr_t JOAN_CDECL init_4d6b20() { return joan::orig::arg::as<u32>(joan::orig::as_image(obj(d::g_a3a2d8))->construct()); }  // 0x0060fad0
std::uintptr_t JOAN_CDECL dtor_4d6b70() { return joan::orig::arg::as<u32>(joan::orig::as_image(obj(d::g_a3a2d8))->dtor()); }  // 0x0060fb10

struct H {
    // ------------------------------------------------------------------------ 0x004d7ba0
    u32 init_demo_4d7ba0() {
        c_demo_4d7ba0.hit();
        joan::orig::as_window(this)->nop_603ad0();  // 0x00603ad0
        joan::orig::as_game(this)->find_cd_drive();  // 0x004d7ca0
        return joan::orig::arg::as<u32>(joan::orig::as_game(this)->resolve_cd_path(str(d::s_MUSIC_DEMO_WAV), str(d::s_MUSIC_9d6248), joan::orig::arg::str(jo::self<jo::Wnd>(this)->music_path)));  // 0x004d7bf0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("init_4d6a90", 0x004d6a90, &init_4d6a90);
B("dtor_4d6ae0", 0x004d6ae0, &dtor_4d6ae0);
B("init_4d6b20", 0x004d6b20, &init_4d6b20);
B("dtor_4d6b70", 0x004d6b70, &dtor_4d6b70);
B("Music::init_demo_4d7ba0", 0x004d7ba0, &H::init_demo_4d7ba0);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b100_06.cpp (game/b100_06)
namespace {
namespace game_b100_06 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;

void crt_atexit(const void* fn) { joan::crt::atexit(fn); }                      // atexit


using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x004d6b90
    H* ctor_4d6b90() {
        joan::orig::as_sys_en(this)->ctor_6035c0();  // 0x006035c0
        jo::self<jo::Wnd>(this)->vtable = img::obj(d::vt_Screen);
        jo::Wnd* const w = jo::self<jo::Wnd>(this);
        crt_memset(w->u8_20, 0, static_cast<u32>(reinterpret_cast<u8*>(w) + sizeof(jo::Wnd) - w->u8_20));  // u8_20 .. the end (0x133 bytes on i686)
        return this;
    }
};

static void JOAN_CDECL init_4d6ab0() { crt_atexit(fn::code_ptr(fn::dtor_4d6ae0)); }  // 0x004d6ab0

#define B(name, addr, fn) JOAN_REPLACE("game/b100_06", name, addr, fn)
B("init_4d6ab0", 0x004d6ab0, &init_4d6ab0);
B("Obj::ctor_4d6b90", 0x004d6b90, &H::ctor_4d6b90);
#undef B

}  // namespace game_b100_06
}  // namespace

// ---------------------------------------- from modules/game_b107_17.cpp (game/b107_17)
namespace {
namespace game_b107_17 {

using u32 = std::uint32_t;

// ---- static initialisers / destructor stubs ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_17", name, addr, fn)
B("init_4d6b00", 0x004d6b00, (&init_pair<fn::init_4d6b20, fn::static_init_4d6b40>));
#undef B

}  // namespace game_b107_17
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {

using i32 = std::int32_t;

void crt_atexit(const void* fn) { joan::crt::atexit(fn); }                      // atexit

void JOAN_CDECL init_4d6b40() { crt_atexit(fn::code_ptr(fn::dtor_4d6b70)); }  // 0x004d6b40

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("static_init_4d6b40", 0x004d6b40, &init_4d6b40);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x004d6be0
    u32 dtor_4d6be0() {
        jo::self<jo::Wnd>(this)->vtable = img::obj(d::vt_Screen);
        joan::orig::as_screen(this)->close_4d6ce0();           // 0x004d6ce0
        return joan::orig::arg::as<u32>(joan::orig::as_sys_en(this)->dtor());  // 0x00603670
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Obj::dtor_4d6be0", 0x004d6be0, &H::dtor_4d6be0);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;

using i16 = std::int16_t;

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x004d6c50
    u32 init_4d6c50(void* arg) {
        if (joan::orig::arg::as<i32>(joan::orig::as_window(this)->open(arg)) == 0) return 0;  // 0x00603710
        if (joan::orig::arg::as<i32>(joan::orig::as_game(this)->load_fonts_4d6fe0()) == 0) return 0;       // 0x004d6fe0
        jo::view<u32>(jo::self<jo::Wnd>(this)->frame_scale) = 0x3f800000u;
        jo::view<u32>(jo::self<jo::Wnd>(this)->speed) = 0x3f800000u;
        jo::self<jo::Wnd>(this)->i16_48 = 1;
        jo::self<jo::Wnd>(this)->u8_4a = 1;
        jo::self<jo::Wnd>(this)->music_flag = 1;
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Obj::init_4d6c50", 0x004d6c50, &H::init_4d6c50);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using i32 = std::int32_t;

// Virtual slot at the x86 byte offset `off` of `self`'s vtable (thiscall; slot off / 4, pointer-sized entries).
std::uintptr_t vslot(const void* self, std::int32_t off) {
    return (*static_cast<const std::uintptr_t* const*>(self))[joan::call::x86_vslot(off)];  // vtable at +0
}

JOAN_COUNTER(c_close_4d6ce0, "Screen::close_4d6ce0");

struct H {
    // ------------------------------------------------------------------ 0x004d6ce0
    std::uintptr_t close_4d6ce0() {
        c_close_4d6ce0.hit();
        if (global<u32>(d::g_video_playing) != 0) {
            joan::orig::as_image(obj(d::g_a3a2d8))->release();  // 0x0060fd20
            joan::orig::as_video_capture(obj(d::g_video_capture))->release();  // 0x00686f50
            global<u32>(d::g_video_playing) = 0;
        }
        if (global<void*>(d::g_a5d878) != nullptr) {
            void* const p = global<void*>(d::g_a5d878);
            if (p != nullptr) thiscall<u32>(vslot(p, 0), p, i32{1});
            global<void*>(d::g_a5d878) = nullptr;
        }
        if (jo::view<i8>(jo::self<jo::Wnd>(this)->closing) == 0) {
            return thiscall<std::uintptr_t>(vslot(this, 0xc), this);
        }
        if (jo::view<i8>(jo::self<jo::Wnd>(this)->is_open) == 0) return reinterpret_cast<std::uintptr_t>(static_cast<void*>(this));
        jo::self<jo::Wnd>(this)->u8_10 = 1;
        joan::orig::as_handle(obj(d::g_scenarios))->release(joan::orig::arg::u32(i32{0}));  // 0x0041c780
        joan::orig::as_game(obj(d::g_game_state))->reset_all_470630();          // 0x00470630
        joan::orig::as_game(obj(d::g_game_state))->shutdown_46f200();          // 0x0046f200
        joan::orig::as_mode(this)->set_499540(joan::orig::arg::u32(i32{0}));             // 0x00499540
        joan::orig::as_game(this)->release_globals();                     // 0x004d7890
        return joan::orig::arg::as<u32>(joan::orig::as_window(this)->activate());              // 0x00603780
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Screen::close_4d6ce0", 0x004d6ce0, &H::close_4d6ce0);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
namespace {
namespace game_b102_04 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using i32 = std::int32_t;

using u16 = std::uint16_t;

JOAN_COUNTER(c_window_create, "Window::create");

// ---------------------------------------------------------------------------- 0x004d7d60
// The window class's procedure (WNDPROC: WPARAM / LPARAM / LRESULT are pointer-sized on x64). The game's message
// chain below (Wnd::on_message -> Window::proc -> Window::wndproc) keeps the original 32-bit parameters: it reads
// wparam as a number (WM_ACTIVATEAPP, WM_SYSCOMMAND) and never dereferences lparam.
static std::intptr_t JOAN_STDCALL wndproc(void* hwnd, u32 msg, std::uintptr_t wparam, std::intptr_t lparam) {
    return joan::orig::arg::as<i32>(joan::orig::as_game(obj(d::g_game_a50088))->on_message(hwnd, joan::orig::arg::i32(msg), static_cast<u32>(wparam), static_cast<u32>(lparam)));  // 0x004d7ef0
}

// Win32 shapes used by Window::create (called through the game's IAT slots).
struct WndClassA {
    u32 style;
    const void* wndproc;
    i32 cls_extra;
    i32 wnd_extra;
    void* instance;
    void* icon;
    void* cursor;
    void* background;
    const char* menu;
    const char* name;
};
static_assert(sizeof(WndClassA) == (sizeof(void*) == 4 ? 40 : 72), "WNDCLASSA");
struct Rect {
    i32 left, top, right, bottom;
};
using LoadIconFn = void*(JOAN_STDCALL*)(void*, const char*);
using GetStockObjectFn = void*(JOAN_STDCALL*)(i32);
using RegisterClassFn = u16(JOAN_STDCALL*)(const WndClassA*);
using AdjustWindowRectExFn = i32(JOAN_STDCALL*)(Rect*, u32, i32, u32);
using CreateWindowExFn = void*(JOAN_STDCALL*)(u32, const char*, const char*, u32, i32, i32, i32, i32, void*, void*, void*, void*);
using HwndFn = i32(JOAN_STDCALL*)(void*);
using SetFocusFn = void*(JOAN_STDCALL*)(void*);

struct H {
    // ------------------------------------------------------------------ 0x004d6e00
    i32 window_create() {
        c_window_create.hit();
        WndClassA wc;
        wc.style = 8;
        wc.wndproc = fn::code_ptr(fn::wndproc);
        wc.cls_extra = 0;
        wc.wnd_extra = 0;
        wc.instance = jo::self<jo::Window>(this)->instance;
        wc.icon = reinterpret_cast<LoadIconFn>(global<void*>(d::iat_USER32_LoadIconA))(jo::self<jo::Window>(this)->instance, str(0x66));
        wc.background = reinterpret_cast<GetStockObjectFn>(global<void*>(d::iat_GDI32_GetStockObject))(4);
        wc.cursor = nullptr;
        wc.menu = nullptr;
        wc.name = str(d::s_JoanOfArc);  // "JoanOfArc"
        if (reinterpret_cast<RegisterClassFn>(global<void*>(d::iat_USER32_RegisterClassA))(&wc) == 0) return 0;
        const u32 ex_style = 0x40000;
        const u32 style = 0x12ca0000;
        const i32 w = at<i32>(joan::orig::arg::as<u8*>(joan::orig::table_a016a0(0)), 4);  // 0x004b08f0  x64: needs <table_a016a0 record>+0x4 i32 (class unknown)
        const i32 h = at<i32>(joan::orig::arg::as<u8*>(joan::orig::table_a016a0(0)), 8);  // 0x004b08f0  x64: needs <table_a016a0 record>+0x8 i32
        Rect rc;
        rc.left = 0;
        rc.right = w;
        rc.top = 0;
        rc.bottom = h;
        reinterpret_cast<AdjustWindowRectExFn>(global<void*>(d::iat_USER32_AdjustWindowRectEx))(&rc, style, 0, ex_style);
        jo::self<jo::Window>(this)->hwnd = reinterpret_cast<CreateWindowExFn>(global<void*>(d::iat_USER32_CreateWindowExA))(
            ex_style, str(d::s_JoanOfArc), str(d::s_Wars_Warriors_Joan_of_Arc), style, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, nullptr,
            nullptr, jo::self<jo::Window>(this)->instance, nullptr);
        if (jo::self<jo::Window>(this)->hwnd == nullptr) return 0;
        reinterpret_cast<HwndFn>(global<void*>(d::iat_USER32_UpdateWindow))(jo::self<jo::Window>(this)->hwnd);      // UpdateWindow
        reinterpret_cast<SetFocusFn>(global<void*>(d::iat_USER32_SetFocus))(jo::self<jo::Window>(this)->hwnd);  // SetFocus
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("Window::create", 0x004d6e00, &H::window_create);
B("wndproc", 0x004d7d60, &wndproc);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
namespace {
namespace game_b103_04 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    std::uintptr_t destroy_4d6fb0() { return joan::orig::arg::as<u32>(joan::orig::as_sys_window(this)->destroy_window()); }  // 0x005a1cc0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("Wnd::destroy_4d6fb0", 0x004d6fb0, &H::destroy_4d6fb0);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b109_05.cpp (game/b109_05)
namespace {
namespace game_b109_05 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using i32 = std::int32_t;

i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }    // atoi

JOAN_COUNTER(c_load_fonts, "Game::load_fonts");

// ---------------------------------------------------------------------------------- 0x004d6fe0
// Bitmap font: 0x00623e90(name, w, h, a, b, c, d, e, f).
void bitmap_font(img::Addr font, img::Addr name, u32 w, u32 h, u32 a, u32 b, u32 c, u32 d, u32 e,
                 u32 f) {
    joan::orig::as_font(obj(font))->init_623e90(joan::orig::arg::str(str(name)), joan::orig::arg::i16(w), joan::orig::arg::i16(h), joan::orig::arg::i16(a), joan::orig::arg::u16(b), joan::orig::arg::u16(c), joan::orig::arg::i16(d), joan::orig::arg::u16(e), joan::orig::arg::u16(f));  // 0x00623e90
}
// TTF font: 0x00624810(name, 1, 0).
void ttf_font(img::Addr font, img::Addr name) {
    joan::orig::as_font(obj(font))->init(joan::orig::arg::str(str(name)), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(u32{0}));  // 0x00624810
}

// Original frame from [ebp-0x2d0] (file reader) to [ebp-0x10] (saved ecx, exclusive); members in the x86 order and,
// on i686, at the x86 offsets. The FileTxt slot is sized from the type (0x19a + 2 on i686, larger on x64).
struct alignas(4) FontFrame {
    u8 file[(sizeof(jo::FileTxt) + 2 + 3) & ~std::size_t{3}];  // [ebp-0x2d0]
    char line[0x78];                                            // [ebp-0x134]
    i32 b;                                                      // [ebp-0xbc]
    i32 a;                                                      // [ebp-0xb8]
    char num[0x50];                                             // [ebp-0xb4]
    char name[0x50];                                            // [ebp-0x64]
    u32 saved;                                                  // [ebp-0x14]
};
static_assert(sizeof(void*) != 4 || (offsetof(FontFrame, line) == 0x2d0 - 0x134 && offsetof(FontFrame, num) == 0x2d0 - 0xb4 &&
                                     offsetof(FontFrame, saved) == 0x2d0 - 0x14 && sizeof(FontFrame) == 0x2d0 - 0x10),
              "x86 frame layout");

u32 load_fonts_body() {
    c_load_fonts.hit();
    FontFrame fr;
    std::memset(&fr, 0, sizeof fr);
    u8* const file = fr.file;
    char* const line = fr.line;
    char* const num = fr.num;
    char* const name = fr.name;
    u32& saved = fr.saved;
    i32& a = fr.a;
    i32& b = fr.b;

    saved = global<u32>(d::g_a403d4);
    global<u32>(d::g_a403d4) = 0;
    joan::orig::as_script_node(obj(d::g_a4d598))->set_4(nullptr);  // 0x0063e630
    joan::orig::as_dialogue(obj(d::g_dialogue))->text_y();         // 0x0063c000
    joan::orig::as_audio(obj(d::g_audio))->init();         // 0x0057a440
    joan::orig::as_sound3d(obj(d::g_sound3d_system))->init_5a0330();         // 0x005a0330
    joan::orig::as_sound_stream(obj(d::g_snd))->init();         // 0x0059ad30
    joan::orig::as_ambient_streams(obj(d::g_mus))->init();         // 0x00585fd0
    joan::orig::as_sound_effects(obj(d::g_sound_effects))->load();         // 0x0045a350
    if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(str(d::s_RESOURCE_CODEPAGE_RES)) != 0) {  // 0x00590b80 exists
        joan::orig::as_file_txt(file)->construct(str(d::s_RESOURCE_CODEPAGE_RES));                 // 0x00589550 ctor
        joan::orig::as_file_txt(file)->read_line(static_cast<char*>(joan::orig::arg::mptr(name)), i32{0x50});  // 0x00589f10 read_line
        joan::orig::as_file_txt(file)->read_line(static_cast<char*>(joan::orig::arg::mptr(num)), i32{0x50});
        a = atoi_(num);
        joan::orig::as_file_txt(file)->read_line(static_cast<char*>(joan::orig::arg::mptr(num)), i32{0x50});
        b = atoi_(num);
        if (joan::orig::arg::as<i32>(joan::orig::as_file_txt(file)->at_eof()) == 0) {         // 0x0046f060
            joan::orig::as_file_txt(file)->read_line(static_cast<char*>(joan::orig::arg::mptr(line)), i32{0x78});  // 0x00589f10
        } else {
            line[0] = 0;
        }
        joan::orig::code_page::setup(joan::orig::arg::str(name), joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::str(line));  // 0x0058b2d0
        joan::orig::as_file_txt(file)->destruct();            // 0x00589890 dtor
    }
    joan::orig::as_txt_res(obj(d::g_txt))->load_basic_txr();  // 0x004da620
    joan::orig::as_txt_res(obj(d::g_strings))->load_interface_txr();  // 0x004de9c0
    bitmap_font(d::g_text_a5d140, d::s_testfont, 0x10, 0x10, 0x25, 0x21, 0x7e, 1, 1, 0);  // testfont
    bitmap_font(d::g_font_verdana_10, d::s_font_verdana_10, 0xc, 0xc, 0x27, 0x21, 0xff, 1, 1, 0);
    bitmap_font(d::g_font_verdana_10_bold, d::s_font_verdana_10_bold, 0xc, 0xc, 0x27, 0x21, 0xff, 1, 1, 0);
    bitmap_font(d::g_font_verdana_12, d::s_font_verdana_12, 0xe, 0xe, 0x22, 0x21, 0xff, 1, 1, 0);
    bitmap_font(d::g_font_a53f28, d::s_font_verdana_12_bold, 0xe, 0xe, 0x22, 0x21, 0xff, 1, 1, 0);
    bitmap_font(d::g_a507c8, d::s_verdana_10_regular, 0xc, 0xc, 0x27, 0x21, 0x7e, 1, 1, 0);
    bitmap_font(d::g_font_verdana_12_regular_black_out, d::s_verdana_12_regular_black_out, 0xe, 0xe, 0x22, 0x21, 0x7e, 1, 1, 0);
    bitmap_font(d::g_a52888, d::s_font_perrygothic_10_gold, 0x14, 0x14, 0x25, 0x21, 0xff, 1, 0, 0);
    bitmap_font(d::g_font_a47d18, d::s_font_verdana_10_gold, 0x14, 0x14, 0x25, 0x21, 0xff, 1, 1, 0);
    bitmap_font(d::g_a460e8, d::s_24pt_large_text, 0x29, 0x29, 0x18, 0x21, 0x7a, 1, 1, 0);
    bitmap_font(d::g_font_18pt_attribute_numbers_out, d::s_18pt_attribute_numbers_out, 0x17, 0x17, 0x19, 0x22, 0x39, 1, 1, 0);
    bitmap_font(d::g_font_18pt_attribute_numbers_over, d::s_18pt_attribute_numbers_over, 0x17, 0x17, 0x19, 0x22, 0x39, 1, 1, 0);
    bitmap_font(d::g_font_18pt_dark_numbers, d::s_18pt_dark_numbers, 0x17, 0x17, 0x19, 0x22, 0x39, 1, 1, 0);
    bitmap_font(d::g_font_18pt_light_numbers, d::s_18pt_light_numbers, 0x17, 0x17, 0x19, 0x22, 0x39, 1, 1, 0);
    bitmap_font(d::g_font_24pt_large_numbers_gold, d::s_24pt_large_numbers_gold, 0x1f, 0x1f, 0x19, 0x21, 0x39, 1, 1, 0);
    bitmap_font(d::g_a57f40, d::s_24pt_large_numbers_light, 0x1f, 0x1f, 0x19, 0x21, 0x39, 1, 1, 0);
    bitmap_font(d::g_dialogs, d::s_Goudy12_18_inner_emboss, 0x11, 0x11, 0xe, 0x21, 0xff, 1, 0, 0);
    bitmap_font(d::g_font_a489c8, d::s_Goudy12_18_black, 0x11, 0x11, 0xe, 0x21, 0xff, 1, 0, 0);
    bitmap_font(d::g_text_a58ac0, d::s_Goudy12_18_yellow, 0x11, 0x11, 0xe, 0x21, 0xff, 1, 0, 0);
    bitmap_font(d::g_font_verdana_12_gold_black_outline, d::s_verdana_12_gold_black_outlin, 0xe, 0xe, 0x22, 0x21, 0x7e, 1, 1, 0);
    bitmap_font(d::g_font_celtic_10pt_english, d::s_Celtic_10pt_English, 0x11, 0x11, 0x1c, 0x21, 0x7a, 1, 0, 0);
    bitmap_font(d::g_a506a0, d::s_Celtic_10pt_French, 0x11, 0x11, 0x1c, 0x21, 0x7a, 1, 0, 0);
    bitmap_font(d::g_font_celtic_18pt_black, d::s_Celtic_18pt_Black, 0x1f, 0x1f, 0x20, 0x21, 0x7a, 1, 0, 0);
    bitmap_font(d::g_a596d8, d::s_Verdana_12_English, 0xe, 0xe, 0x22, 0x21, 0xff, 1, 0, 0);
    bitmap_font(d::g_a52808, d::s_Verdana_12_french, 0xe, 0xe, 0x22, 0x21, 0xff, 1, 0, 0);
    bitmap_font(d::g_font_goudy18_27_white, d::s_Goudy18_27_white, 0x1a, 0x1a, 0x12, 0x21, 0xff, 1, 0, 0);
    bitmap_font(d::g_font_a5cdc0, d::s_verdana12_reg_outline, 0x10, 0x10, 0x3c, 0x21, 0xff, 1, 1, 0);
    bitmap_font(d::g_font_goudy14_22_white, d::s_Goudy14_22_white, 0x1a, 0x1a, 0x12, 0x21, 0xff, 1, 0, 0);
    bitmap_font(d::g_font_goudy12_bold_black, d::s_Goudy12_Bold_Black, 0x11, 0x11, 0xe, 0x21, 0xff, 1, 0, 0);
    bitmap_font(d::g_font_goudy12_18_target_unit, d::s_Goudy12_18_target_unit, 0x11, 0x11, 0xe, 0x21, 0xff, 1, 0, 0);
    ttf_font(d::g_font_goudy_bold_24, d::s_Goudy_bold_24);  // Goudy_bold_24
    ttf_font(d::g_font_goudy_bold_gold_24, d::s_Goudy_Bold_Gold_24);
    ttf_font(d::g_font_goudy_bold_20, d::s_Goudy_bold_20);
    ttf_font(d::g_font_goudy_bold_18, d::s_Goudy_bold_18);
    ttf_font(d::g_a4d518, d::s_Goudy_bold_10);
    ttf_font(d::g_font_times_10, d::s_Times_10);
    ttf_font(d::g_a59738, d::s_Goudy_10);
    ttf_font(d::g_font_goudy_bold_48, d::s_Goudy_bold_48);
    ttf_font(d::g_font_goudy_18, d::s_Goudy_18);
    ttf_font(d::g_font_goudy_20, d::s_Goudy_20);
    ttf_font(d::g_font_goudy_bold_14, d::s_goudy_bold_14);
    ttf_font(d::g_a4d4d8, d::s_goudy_bold_italic_12);
    ttf_font(d::g_fontNum, d::s_goudy_bold_italic_18);
    ttf_font(d::g_font_goudy_bold_italic_24, d::s_goudy_bold_italic_24);
    ttf_font(d::g_font_goudy_12, d::s_goudy_12);
    ttf_font(d::g_font_goudy_italic_12, d::s_goudy_italic_12);
    ttf_font(d::g_font_dauphin_14_bold, d::s_dauphin_14_bold);  // dauphin_14_bold
    joan::orig::as_help_box(obj(d::g_help_box))->load(str(d::s_HELP_RES));  // 0x00488400 HELP.RES
    joan::orig::as_game_set(obj(d::g_game_set))->init_58adf0();                   // 0x0058adf0
    const u32 table = joan::orig::arg::as<u32>(joan::orig::as_game_set(obj(d::g_game_set))->find_set_58b250(str(d::s_STD)));  // 0x0058b250 "STD"
    joan::orig::as_game_set(obj(d::g_game_set))->open_set_58b080(joan::orig::arg::i32(table));                              // 0x0058b080
    global<u32>(d::g_a403d4) = saved;
    return 1;
}

struct H {
    // ------------------------------------------------------------------------------ 0x004d6fe0
    u32 load_fonts() { return load_fonts_body(); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_05", name, addr, fn)
B("Game::load_fonts_4d6fe0", 0x004d6fe0, &H::load_fonts);
#undef B

}  // namespace game_b109_05
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using i32 = std::int32_t;

template <class R = void, class... A>
R com_call(void* object, u32 offset, A... args) {  // COM: stdcall, this first
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>((*static_cast<std::uintptr_t**>(object))[joan::call::x86_vslot(offset)])(
        object, args...);
}

JOAN_COUNTER(c_release_globals, "Game::release_globals");
JOAN_COUNTER(c_video_copy, "video_copy_frame");

// ---- 0x004d7890 table ----
struct GlobalCall {
    img::Addr object;
    fn::Code thunk;
};
constexpr GlobalCall kRelease[] = {
    {d::g_dialogue, fn::Obj_close_63c0a0},  // 0x0063c0a0
    {d::g_a4d598, fn::Gadget_destroy_tree},  // 0x0063f2c0
    {d::g_sound_effects, fn::Se_free},  // 0x0059e0c0
    {d::g_audio, fn::Audio_stop_57a6c0},  // 0x0057a6c0
    {d::g_sound3d_system, fn::Sound3D_release_5a04e0},  // 0x005a04e0
    {d::g_game_set, fn::deinit_58ae30},  // 0x0058ae30
    {d::g_help_box, fn::Help_clear},  // 0x004884c0
    {d::g_text_a5d140, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_verdana_10, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_verdana_10_bold, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_verdana_12, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_a53f28, fn::Font2d_release},  // 0x00624dc0
    {d::g_a507c8, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_verdana_12_regular_black_out, fn::Font2d_release},  // 0x00624dc0
    {d::g_a52888, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_a47d18, fn::Font2d_release},  // 0x00624dc0
    {d::g_a460e8, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_18pt_attribute_numbers_out, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_18pt_attribute_numbers_over, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_18pt_dark_numbers, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_18pt_light_numbers, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_24pt_large_numbers_gold, fn::Font2d_release},  // 0x00624dc0
    {d::g_a57f40, fn::Font2d_release},  // 0x00624dc0
    {d::g_dialogs, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_a489c8, fn::Font2d_release},  // 0x00624dc0
    {d::g_text_a58ac0, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_celtic_10pt_english, fn::Font2d_release},  // 0x00624dc0
    {d::g_a506a0, fn::Font2d_release},  // 0x00624dc0
    {d::g_a596d8, fn::Font2d_release},  // 0x00624dc0
    {d::g_a52808, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_celtic_18pt_black, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_verdana_12_gold_black_outline, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy18_27_white, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_a5cdc0, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy14_22_white, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy12_bold_black, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy12_18_target_unit, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_bold_24, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_bold_gold_24, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_bold_20, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_bold_18, fn::Font2d_release},  // 0x00624dc0
    {d::g_a4d518, fn::Font2d_release},  // 0x00624dc0
    {d::g_a59738, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_times_10, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_bold_48, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_18, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_20, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_bold_14, fn::Font2d_release},  // 0x00624dc0
    {d::g_a4d4d8, fn::Font2d_release},  // 0x00624dc0
    {d::g_fontNum, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_bold_italic_24, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_12, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_goudy_italic_12, fn::Font2d_release},  // 0x00624dc0
    {d::g_font_dauphin_14_bold, fn::Font2d_release},  // 0x00624dc0
    {d::g_txt, fn::TextRes_free_all},  // 0x005a2070
    {d::g_strings, fn::TextRes_free_all},  // 0x005a2070
    {d::g_a57fb8, fn::Obj_load_config},  // 0x00459e90
};
static_assert(sizeof(kRelease) / sizeof(kRelease[0]) == 57, "57 teardown calls");

// ------------------------------------------------------------------ 0x004d7f80 (cdecl)
u32 JOAN_CDECL video_copy_frame() {
    c_video_copy.hit();
    void* const video = obj(d::g_a3a2d8);
    joan::orig::as_bit_writer(video)->put();  // 0x0060ff70
    void* const device = global<void*>(d::g_game_device);
    if (com_call<i32>(device, 0x78, global<void*>(d::g_a3a2d8)) == 0) {
        joan::orig::as_bit_writer(video)->push_bit();                     // 0x00610040
        const i32 rows = joan::orig::arg::as<i32>(joan::orig::as_camera(video)->get_20());    // 0x0060fa70
        joan::orig::as_camera(video)->get_1c();                     // 0x0060fa50 (result unused)
        const u32 pitch = global<u32>(d::g_a3a2b4) * global<u32>(d::g_a3a2b8);
        std::uint8_t* dst = global<std::uint8_t*>(d::g_a3a2c0) + pitch * (global<u32>(d::g_a3a2bc) - 1u);
        const u32 step = joan::orig::arg::as<u32>(joan::orig::as_image(video)->pitch());  // 0x004d8520
        const std::uint8_t* src = joan::orig::arg::as<const std::uint8_t*>(joan::orig::as_image(video)->bits());         // 0x004d84e0
        const u32 len = global<u32>(d::g_a3a2b4) * global<u32>(d::g_a3a2b8);
        for (i32 i = 0; i < rows; ++i) {
            std::memcpy(joan::orig::arg::mptr(dst), joan::orig::arg::ptr(src), joan::orig::arg::u32(len));
            dst -= pitch;
            src += step;
        }
        const u32 frame = global<u32>(d::g_a3a310);
        joan::orig::as_video_capture(obj(d::g_video_capture))->write_frame(joan::orig::arg::mptr(0), joan::orig::arg::i32(frame * 100u / 3u));  // 0x00687060
        global<u32>(d::g_a3a310) = global<u32>(d::g_a3a310) + 1u;
        joan::orig::as_bit_writer(video)->drop();  // 0x006100a0
    }
    return joan::orig::arg::as<u32>(joan::orig::as_image(video)->pop_lock());  // 0x0060ffd0
}

struct H {
    // ------------------------------------------------------------------ 0x004d7890
    u32 release_globals() {
        c_release_globals.hit();
        u32 r = 0;
        for (const GlobalCall& c : kRelease) r = thiscall<u32>(c.thunk, obj(c.object));
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Game::release_globals", 0x004d7890, &H::release_globals);
B("video_copy_frame", 0x004d7f80, &video_copy_frame);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // 0x004d7b70
    u32 call_603fd0() { return joan::orig::arg::as<u32>(joan::orig::as_sound(this)->restart()); }  // 0x00603fd0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Obj::call_603fd0", 0x004d7b70, &H::call_603fd0);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
namespace {
namespace game_b97_03 {

using u32 = std::uint32_t;

using i32 = std::int32_t;

JOAN_COUNTER(c_resolve_path, "X::resolve_path");

using joan::orig::helpers::strcpy_;  // strcpy
char* strcat_(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }  // strcat

struct H {
    // ------------------------------------------------------------------------ 0x004d7bf0
    i32 resolve_path(const char* a, const char* src, char* dst) {
        c_resolve_path.hit();
        if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(a) != 0) {  // 0x00590b80
            strcpy_(dst, src);
            return 1;
        }
        if (jo::view<i8>(jo::self<jo::Wnd>(this)->cd_drive) != 0) {
            strcpy_(dst, str(d::s_D_9d6250));  // "D:\"
            strcat_(dst, src);
            dst[0] = jo::self<jo::Wnd>(this)->cd_drive;
            return 1;
        }
        strcpy_(dst, str(d::kEmpty));  // ""
        return 0;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("X::resolve_path", 0x004d7bf0, &H::resolve_path);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b99_04.cpp (game/b99_04)
namespace {
namespace game_b99_04 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::global;

using i32 = std::int32_t;

JOAN_COUNTER(c_find_drive, "Cd::find_drive");

struct H {
    // ------------------------------------------------------------------------ 0x004d7ca0
    // The game CD's drive (+0x4b, 0 = none): CD support removed - the game runs from its installed files, so there is
    // no drive scan (the original looked for a CD-ROM drive C:..Z: holding JOAN.EXE).
    void find_drive() {
        c_find_drive.hit();
        jo::view<u8>(jo::self<jo::Wnd>(this)->cd_drive) = 0;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_04", name, addr, fn)
B99("Cd::find_drive", 0x004d7ca0, &H::find_drive);
#undef B99

}  // namespace game_b99_04
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    void* set_3c(std::uint32_t v) {  // 0x004d7e70
        jo::view<std::uint8_t>(jo::self<jo::Wnd>(this)->next_mode) = static_cast<std::uint8_t>(v);
        return this;  // eax = this
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("set_byte_3c", 0x004d7e70, &H::set_3c);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
namespace {
namespace game_b101_06 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using i32 = std::int32_t;

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x004d7ef0
    i32 on_message(void* a, i32 msg, u32 c, u32 d) {
        if (msg == 0x10) {
            global<u8>(d::g_a501d9) = 1;
            return 0;
        }
        return joan::orig::arg::as<i32>(joan::orig::as_window(this)->proc(a, joan::orig::arg::u32(msg), joan::orig::arg::u32(c), joan::orig::arg::i32(d)));  // 0x00603e20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("Wnd::on_message", 0x004d7ef0, &H::on_message);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
namespace {
namespace game_b95_03 {

using u32 = std::uint32_t;

struct H {
    void* get_c() { return jo::self<jo::Surface>(this)->bits; }    // 0x004d84e0
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("X::get_c", 0x004d84e0, &H::get_c);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
namespace {
namespace game_b94_03 {

using u32 = std::uint32_t;

struct H {

    u32 get_10() { return jo::view<u32>(jo::self<jo::Surface>(this)->pitch); }    // 0x004d8520
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("X::get_10", 0x004d8520, &H::get_10);
#undef B94

}  // namespace game_b94_03
}  // namespace

// ---------------------------------------- from modules/game_b104_05.cpp (game/b104_05)
namespace {
namespace game_b104_05 {

using u32 = std::uint32_t;

struct H {

    // ------------------------------------------------------------------------ 0x004d8560
    void* clear_44_4c() {
        jo::self<jo::Input>(this)->acc_x = 0;  // this: the Input 0x00a516b0
        jo::self<jo::Input>(this)->acc_y = 0;
        jo::self<jo::Input>(this)->acc_z = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_05", name, addr, fn)
B("Obj::clear_44_4c", 0x004d8560, &H::clear_44_4c);
#undef B

}  // namespace game_b104_05
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {

struct H {

    // 0x004d8bb0
    std::uint8_t byte_6ae() { return jo::self<jo::UnitAttScreen>(this)->is_open; }
    // 0x004d8be0
    void* slot68(std::int32_t i) {
        return &jo::self<jo::MusicRes>(this)->tracks[i - 1];
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Obj::byte_6ae", 0x004d8bb0, &H::byte_6ae);
B("Table68::slot", 0x004d8be0, &H::slot68);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Game::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_06::H::ctor_4d6b90>(this)); }  // 0x4d6b90
auto joan::orig::Game::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_15::H::dtor_4d6be0>(this)); }  // 0x4d6be0
auto joan::orig::Game::init(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_12::H::init_4d6c50>(this, a0)); }  // 0x4d6c50
auto joan::orig::Screen::close_4d6ce0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_11::H::close_4d6ce0>(this)); }  // 0x4d6ce0
auto joan::orig::Game::load_fonts_4d6fe0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_05::H::load_fonts>(this)); }  // 0x4d6fe0
auto joan::orig::Game::release_globals() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_13::H::release_globals>(this)); }  // 0x4d7890
auto joan::orig::Game::resolve_cd_path(const char* a0, const char* a1, const char* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_03::H::resolve_path>(this, a0, a1, a2)); }  // 0x4d7bf0
auto joan::orig::Game::find_cd_drive() -> void { joan::bind::member<&::game_b99_04::H::find_drive>(this); }  // 0x4d7ca0
auto joan::orig::Game::on_message(void* a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_06::H::on_message>(this, a0, a1, a2, a3)); }  // 0x4d7ef0
auto joan::orig::video_copy_frame() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b108_13::video_copy_frame>()); }  // 0x4d7f80
auto joan::orig::Image::bits() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_03::H::get_c>(this)); }  // 0x4d84e0
auto joan::orig::Image::pitch() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_03::H::get_10>(this)); }  // 0x4d8520
auto joan::orig::Cursor::clear_deltas() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b104_05::H::clear_44_4c>(this)); }  // 0x4d8560
auto joan::orig::Panel::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b92_03::H::byte_6ae>(this)); }  // 0x4d8bb0
auto joan::orig::MusicRes::track(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_03::H::slot68>(this, a0)); }  // 0x4d8be0
