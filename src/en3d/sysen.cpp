// en3d/sysen.cpp: functions of the original d:\ww\en3d\sysen.cpp (re/attribution.csv).
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
#include <limits>
#include <cstdarg>
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../canvas_api.hpp"
#include "../render/overrides.hpp"
#include "../d3d8/d3d8_types.hpp"
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

void* const kHeap = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - SysEn::ctor_6035c0 0x006035c0 (ret 0): base 0x5a1b60, vptr 0x9d99b0, memset(+0x14,
//      0xc), +0x1c = 0x58e7b0(0x10000, "sysen.cpp").
//
//  - SEH frames (0x5e6000, 0x5f0040, 0x6035c0, 0x605d80, 0x609620) only matter on exceptions;
//    normal paths reproduced.
namespace {
namespace game_b99_07 {


using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_sysen_ctor, "SysEn::ctor_6035c0");

struct H {
    // ------------------------------------------------------------------ 0x006035c0
    H* sysen_ctor() {
        c_sysen_ctor.hit();
        joan::orig::as_sys_window(this)->construct();  // 0x005a1b60
        jo::self<jo::SysEnWindow>(this)->vtable = img::obj(d::vt_Obj_9d99b0);
        crt_memset(jo::self<jo::SysEnWindow>(this)->u8_14, 0, static_cast<u32>(sizeof(jo::self<jo::SysEnWindow>(this)->u8_14) + sizeof(jo::self<jo::SysEnWindow>(this)->heap)));  // +0x14..+0x1f
        jo::self<jo::SysEnWindow>(this)->heap = joan::orig::as_mem(kHeap)->add(0x10000u, str(d::s_d_ww_en3d_sysen_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a1851c)) + 2));  // 0x0058e7b0
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("SysEn::ctor_6035c0", 0x006035c0, &H::sysen_ctor);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
//
//    - 0x00603670 SysEn::~SysEn               (thiscall, ret 0): vtable 0x9d99b0, free +0x1c
//      (sysen.cpp), 0x00603780, base dtor 0x005a1ba0 (eax returned). SEH: normal path.
namespace {
namespace game_b108_06 {


struct H {
    std::uintptr_t sysen_dtor() {  // 0x00603670
        jo::self<jo::SysEnWindow>(this)->vtable = img::obj(d::vt_Obj_9d99b0);
        joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::self<jo::SysEnWindow>(this)->heap), str(d::s_d_ww_en3d_sysen_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a18520)) + 1));  // 0x0058ed70 free (sysen.cpp)
        joan::orig::as_window(this)->activate();         // 0x00603780
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_sys_window(this)->destruct());  // 0x005a1ba0
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("SysEn::dtor", 0x00603670, &H::sysen_dtor);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b107_09.cpp (game/b107_09)
//
//    - 0x00603780 Obj::activate (thiscall, vtable 0x9d99b0 slot 0, also called by 0x004d6ce0
//      and 0x00603670): byte +0x12 == 0 -> virtual +0xc; else byte +0xf != 0 -> byte +0x10 = 1,
//      0x006039d0, 0x005a1c40. Returns exactly the eax the original leaves (the virtual's /
//      0x005a1c40's result, or this).
namespace {
namespace game_b107_09 {

using joan::call::thiscall;
// Virtual function `slot` (vtable index) of the object.
std::uintptr_t vslot(const void* o, std::int32_t slot) { return static_cast<const std::uintptr_t*>(jo::as_sys_en_window(o)->vtable)[slot]; }

using i8 = std::int8_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {
    // 0x00603780
    std::uintptr_t activate() {
        if (jo::view<i8>(jo::self<jo::SysEnWindow>(this)->closing) == 0) {
            return thiscall<std::uintptr_t>(vslot(this, 3), this);  // vtable +0xc
        }
        if (jo::view<i8>(jo::self<jo::SysEnWindow>(this)->is_open) == 0) {
            return reinterpret_cast<std::uintptr_t>(this);  // eax = this (register leftover)
        }
        jo::self<jo::SysEnWindow>(this)->u8_10 = 1;
        joan::orig::as_game(this)->shutdown_managers();         // 0x006039d0
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_sys_window(this)->close());  // 0x005a1c40
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_09", name, addr, fn)
B("Obj::activate", 0x00603780, &H::activate);
#undef B

}  // namespace game_b107_09
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::img::obj;

using joan::call::thiscall;

void JOAN_CDECL init_603530() {
    joan::orig::init_603550();
    joan::orig::init_603570();
}
void JOAN_CDECL atexit_6035a0() { joan::orig::as_jpeg_writer(obj(d::g_a69b40))->destruct(); }  // 0x005a8fa0

JOAN_REPLACE("game/b105_17", "init_603530", 0x00603530, &init_603530);
JOAN_REPLACE("game/b105_17", "atexit_6035a0", 0x006035a0, &atexit_6035a0);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
namespace {
namespace game_b104_10 {

using u32 = std::uint32_t;

using joan::img::obj;

using joan::call::thiscall;

std::uintptr_t JOAN_CDECL init_603550() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_jpeg_writer(obj(d::g_a69b40))->construct()); }  // 0x005a8d80

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("init_603550", 0x00603550, &init_603550);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

using i32 = std::int32_t;

using joan::call::cdecl_call;

// ---------------------------------------------------------------------------- 0x00603570
static void JOAN_CDECL init_603570() { joan::crt::atexit(fn::code_ptr(fn::atexit_6035a0)); }  // atexit

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("init_603570", 0x00603570, &init_603570);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;


JOAN_COUNTER(c_open_603710, "Obj::open_603710");

struct H {
    // --------------------------------------------------------------------- 0x00603710
    i32 open_603710(void* a) {
        c_open_603710.hit();
        jo::self<jo::SysEnWindow>(this)->u8_14[1] = 0;
        jo::self<jo::SysEnWindow>(this)->u8_14[4] = 1;
        using Virtual = u32 (Obj::*)();
        JOAN_MEMBER_POINTER_CHECK(Virtual);
        Virtual method{};
        std::memcpy(&method, &joan::call::vtable_of(this)[0x18 / 4], sizeof(void*));
        (reinterpret_cast<Obj*>(this)->*method)();
        if (joan::orig::arg::as<i32>(joan::orig::as_sys_window(this)->open(a)) == 0) return 0;  // 0x005a1bd0
        if (joan::orig::arg::as<i32>(joan::orig::as_game(this)->init_display_6037f0()) == 0) return 0;     // 0x006037f0
        return 1;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Obj::open_603710", 0x00603710, &H::open_603710);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using joan::img::obj;

using joan::call::cdecl_call;
using joan::img::str;

JOAN_COUNTER(c_init_display, "Game::init_display_6037f0");
JOAN_COUNTER(c_screenshot, "Game::screenshot_603b20");

struct H {
    // ------------------------------------------------------------------------ 0x006037f0
    i32 init_display() {
        c_init_display.hit();
        joan::orig::select_preset(i32{0});  // 0x004b0930
        if (joan::orig::arg::as<i32>(joan::orig::as_sys_en(obj(d::g_sys3d))->create()) == 0) return 0;  // 0x005f6620
        void* const pal = obj(d::g_vga);
        joan::orig::as_display_palette(pal)->attach(joan::orig::arg::mptr(global<void*>(d::g_game_device)));           // 0x0060f5c0
        joan::orig::as_display_palette(pal)->load(str(d::s_RESOURCE_PAL_STD_RES));                   // 0x0060f760 "RESOURCE\PAL_STD.RES"
        joan::orig::as_image(obj(d::g_vga_back))->attach_surface(joan::orig::arg::mptr(global<void*>(d::g_a5242c)));  // 0x0060fc20
        if (global<void*>(d::g_vga_back) == nullptr) return 0;
        joan::orig::as_display_palette(pal)->nop_60f960(obj(d::g_vga_back));                     // 0x0060f960
        joan::orig::as_image(obj(d::g_a56ef0))->attach_surface(joan::orig::arg::mptr(global<void*>(d::g_a52430)));  // 0x0060fc20
        if (global<void*>(d::g_vga_back) == nullptr) return 0;  // the original re-tests 0xa5cc58
        joan::orig::as_streamer(obj(d::g_async_file_loader))->init();  // 0x00578d70
        joan::orig::as_tex_dir(obj(d::g_texture_dir))->init();  // 0x00604110
        joan::orig::as_texture_manager(obj(d::g_texture_array))->build_paths();  // 0x00608e10
        joan::orig::as_scene(obj(d::g_scene))->reset();  // 0x005e9ac0
        joan::orig::as_sys_en(obj(d::g_view3d))->init_613730();  // 0x00613730
        joan::orig::as_camera(obj(d::g_view3d))->set_viewport(static_cast<joan::orig::Surface*>(obj(d::g_vga_back)), obj(d::g_a56ef0), joan::orig::arg::u32(global<u32>(d::g_a48880)), joan::orig::arg::u32(global<u32>(d::g_a48884)), joan::orig::arg::u32(global<u32>(d::g_a48888)), joan::orig::arg::u32(global<u32>(d::g_a4888c)));  // 0x00613bf0
        joan::orig::as_camera(obj(d::g_view3d))->set_scene(static_cast<joan::orig::Scene*>(obj(d::g_scene)));                             // 0x00613db0
        joan::orig::as_camera(obj(d::g_sprites_a46128))->build_631850();                                              // 0x00631850
        joan::orig::as_input(obj(d::g_mouse))->init_592380(jo::self<jo::SysEnWindow>(this)->instance, jo::self<jo::SysEnWindow>(this)->hwnd, nullptr);             // 0x00592380
        joan::orig::as_mouse_cursor(obj(d::g_mouse_cursor))->init_493eb0(joan::orig::arg::u32(global<u32>(d::g_a52448)));                    // 0x00493eb0
        joan::orig::as_cursor(obj(d::g_mouse_cursor))->set_596db0(i32{1});                                      // 0x00596db0
        joan::orig::as_mouse_cursor(obj(d::g_mouse_cursor))->move(joan::orig::arg::i32(global<u32>(d::g_mouse_x)), joan::orig::arg::i32(global<u32>(d::g_mouse_y)));  // 0x004918d0
        joan::orig::as_cursor(obj(d::g_mouse))->set_pos();                                              // 0x00592bd0
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00603b20
    void screenshot() {
        c_screenshot.hit();
        void* const fs = obj(d::g_misc);
        bool jpg = true;
        if (joan::orig::as_misc(fs)->is_file_exist(str(d::s_output_bmp_sys)) != 0) jpg = false;  // 0x00590b80 "output_bmp.sys"
        if (global<u32>(d::g_a526fb) == 0) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_d3d(obj(d::g_sys3d))->device_lost()) != 0) return;  // 0x005ff420
        alignas(4) char name[0x194];
        joan::orig::as_string(static_cast<char*>(name))->construct();  // 0x005a1170
        auto c_str = [&]() { return joan::orig::arg::as<const char*>(joan::orig::as_string(static_cast<char*>(name))->c_str()); };  // 0x00417d90
        auto append = [&](img::Addr s) { joan::orig::as_string(static_cast<char*>(name))->append(joan::orig::arg::str(str(s))); };  // 0x005a1550
        i32 n = 0;
        for (; n <= 999; ++n) {
            joan::orig::as_string(static_cast<char*>(name))->assign(str(d::s_ScreenShot));          // 0x005a1440 "ScreenShot\"
            if (joan::orig::arg::as<i32>(joan::orig::as_misc(fs)->is_dir_exist_590c00(joan::orig::arg::str(c_str()))) == 0) joan::orig::libc::_mkdir(joan::orig::arg::str(c_str()));  // 0x00590c00 / _mkdir
            append(d::s_JOAN_9d99e0);  // "JOAN"
            if (n < 10) {
                append(d::s_00);  // "00"
            } else if (n < 100) {
                append(d::s_0);  // "0"
            }
            joan::orig::as_string(static_cast<char*>(name))->append(joan::orig::arg::i32(n));  // 0x005a15d0
            append(jpg ? d::s_JPG : d::s_BMP);                   // ".JPG" / ".BMP"
            if (joan::orig::as_misc(fs)->is_file_exist(joan::orig::arg::str(c_str())) == 0) break;  // 0x00590b80
        }
        if (n > 999) return;
        alignas(alignof(void*)) u8 surf[sizeof(jo::Image)];  // an Image: 0x34 on x86, 0x40 on x64
        joan::orig::as_image(static_cast<u8*>(surf))->construct();  // 0x0060fad0
        joan::orig::as_image(static_cast<u8*>(surf))->create(joan::orig::arg::u32(global<u32>(d::g_screen_w)), joan::orig::arg::u32(global<u32>(d::g_screen_h)), joan::orig::arg::u32(i32{0}), i32{0xb});  // 0x0060fb60
        if (joan::orig::arg::as<u8>(joan::orig::as_canvas(static_cast<u8*>(surf))->valid()) != 0) {  // 0x0056ddc0
            void* const dev = global<void*>(d::g_game_device);
            void* const* const vt = joan::call::vtable_of(dev);
            const i32 hr = reinterpret_cast<i32(JOAN_STDCALL*)(void*, void*)>(vt[0x78 / 4])(dev, jo::as_image(static_cast<u8*>(surf))->surface);  // GetFrontBuffer
            if (hr == 0) {
                if (jpg) {
                    joan::orig::as_jpeg_writer(obj(d::g_a69b40))->write_surface(joan::orig::arg::str(c_str()), joan::orig::arg::mptr(static_cast<u8*>(surf)));  // 0x005a9040
                } else {
                    joan::orig::as_texture(static_cast<u8*>(surf))->save_bmp(joan::orig::arg::str(c_str()));  // 0x00611e70
                }
            }
        }
        joan::orig::as_image(static_cast<u8*>(surf))->dtor();  // 0x0060fb10
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Game::init_display_6037f0", 0x006037f0, &H::init_display);
B("Game::screenshot_603b20", 0x00603b20, &H::screenshot);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b104_10_2.cpp (game/b104_10)
namespace {
namespace game_b104_10_2 {

struct H {
    // eax untouched; callers ignore it: direct 0x004d7baf, and vtable 0x9d99b0 slot 6
    // (0x9d99c8, slot introduced by this class) called at 0x00603732.
    void nop_603ad0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("Obj::nop_603ad0", 0x00603ad0, &H::nop_603ad0);
#undef B

}  // namespace game_b104_10_2
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (en3d/helpers33)
namespace {
namespace game_leaves81 {

using joan::call::thiscall;

using joan::hook::global;

using joan::img::obj;

struct H {
    // 0x00603af0 Game::flush_loader: 0xa52420 flushed (0x005ff0e0) when 0xa526fb is set.
    void flush_loader() {
        if (global<std::int32_t>(d::g_a526fb) != 0) joan::orig::as_sys_en(img::obj(d::g_sys3d))->present();  // 0x005ff0e0
    }
};

#define T33(name, addr, fn) JOAN_REPLACE("en3d/helpers33", name, addr, fn)
T33("Game::flush_loader", 0x00603af0, &H::flush_loader);
#undef T33

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

// USER32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

struct H {
    // ------------------------------------------------------------------ 0x00603e20
    i32 window_proc(void* hwnd, u32 msg, u32 wp, i32 lp) {
        if (msg == 0x112 && wp == 0xf170) win_call<i32>(d::iat_USER32_ShowWindow, jo::self<jo::Window>(this)->hwnd, 6);  // ShowWindow
        if (msg == 0x50) return 0;
        return joan::orig::arg::as<i32>(joan::orig::as_sys_window(this)->wndproc(joan::orig::arg::mptr(hwnd), joan::orig::arg::u32(msg), joan::orig::arg::u32(wp), joan::orig::arg::u32(lp)));  // 0x005a1e20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Window::proc", 0x00603e20, &H::window_proc);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b105_17_2.cpp (game/b105_17)
namespace {
namespace game_b105_17_2 {

struct H {
    void nop() {}
};

JOAN_REPLACE("game/b105_17", "Obj::nop_603e90", 0x00603e90, &H::nop);

}  // namespace game_b105_17_2
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
namespace {
namespace game_b102_11 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

JOAN_COUNTER(c_screen_close, "Screen::close");

struct H {
    // 0x00603eb0 (ret), vtable slot
    u32 screen_close() {
        c_screen_close.hit();
        joan::orig::as_sys_window(this)->set_suspended();                      // 0x005a1d80
        joan::orig::as_sound(obj(d::g_snd))->set_volume_59b150(0);       // 0x0059b150
        joan::orig::as_volume(obj(d::g_mus))->set(0);       // 0x00586620
        joan::orig::as_group(obj(d::g_names))->pause_sounds(static_cast<std::int8_t>(joan::orig::arg::i32(0u)));       // 0x0049c080
        joan::orig::as_streams(obj(d::g_dialogue))->pause_all(static_cast<char>(joan::orig::arg::i32(0u)));       // 0x0063c6d0
        return joan::orig::arg::as<u32>(joan::orig::as_input(obj(d::g_mouse))->unacquire_all());  // 0x005929f0
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Screen::close", 0x00603eb0, &H::screen_close);
#undef B11

}  // namespace game_b102_11
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;

using joan::hook::global;

using joan::img::obj;

using i16 = std::int16_t;

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

JOAN_COUNTER(c_603f20, "Obj::activate_603f20");

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x00603f20 (vtable slot, 0x9d5d7c / 0x9d99c4)
    u32 activate_603f20() {
        c_603f20.hit();
        joan::orig::as_sys_window(this)->clear_suspended();  // 0x005a1db0
        virtual_call<u32>(this, 0x1c);
        joan::orig::as_sound(obj(d::g_mouse))->pause_all();                                              // 0x00592ae0
        joan::orig::as_sound(obj(d::g_snd))->set_volume_59b150(static_cast<i32>(global<i16>(d::g_music_volume)));  // 0x0059b150
        joan::orig::as_volume(obj(d::g_mus))->set(static_cast<i32>(global<i16>(d::g_a57ff4)));  // 0x00586620
        joan::orig::as_group(obj(d::g_names))->pause_sounds(static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})));                                     // 0x0049c080
        u32 r = thiscall<u32>(fn::Streams_pause_all, obj(d::g_dialogue), i32{1});                              // 0x0063c6d0
        if (global<i16>(d::g_a4860c) != 0) {
            r = joan::orig::arg::as<u32>(joan::orig::as_cursor(obj(d::g_mouse_cursor))->set_596db0(static_cast<i32>(global<i16>(d::g_a4860c))));  // 0x00596db0
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Obj::activate_603f20", 0x00603f20, &H::activate_603f20);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using joan::img::obj;

JOAN_COUNTER(c_sound_restart, "Sound::restart_603fd0");

struct H {
    // ------------------------------------------------------------------ 0x00603fd0
    static void com_release(img::Addr var) {
        void* const o = global<void*>(var);
        void* const* const vt = *static_cast<void* const* const*>(o);
        reinterpret_cast<u32(JOAN_STDCALL*)(void*)>(vt[2])(o);
    }
    u32 sound_restart() {
        c_sound_restart.hit();
        void* const mgr = obj(d::g_sys3d);
        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_d3d(mgr)->device_lost());  // 0x005ff420
        if (n <= 0) return static_cast<u32>(n);
        if (global<void*>(d::g_vga_back) != nullptr) {
            com_release(d::g_vga_back);
            global<void*>(d::g_vga_back) = nullptr;
        }
        if (global<void*>(d::g_a56ef0) != nullptr) {
            com_release(d::g_a56ef0);
            global<void*>(d::g_a56ef0) = nullptr;
        }
        u32 eax = joan::orig::arg::as<u32>(joan::orig::as_sys_en(mgr)->reset_device());  // 0x005ff4a0
        if (global<void*>(d::g_a5242c) == nullptr) return eax;
        eax = joan::orig::arg::as<u32>(joan::orig::as_image(obj(d::g_vga_back))->set_surface(joan::orig::arg::mptr(global<void*>(d::g_a5242c))));  // 0x0060fd70
        if (global<void*>(d::g_a52430) == nullptr) return eax;
        return joan::orig::arg::as<u32>(joan::orig::as_image(obj(d::g_a56ef0))->set_surface(joan::orig::arg::mptr(global<void*>(d::g_a52430))));  // 0x0060fd70
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Sound::restart", 0x00603fd0, &H::sound_restart);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b106_14.cpp (game/b106_14)
//
//    - Game::shutdown_managers 0x006039d0 (thiscall, ret 0): shutdown calls on 0xa48608,
//      0xa516b0, 0xa46128, 0xa46600, 0xa483f8, 0xa403a0, 0xa59718, 0xa5ce00; if 0xa4f888's
namespace {
namespace game_b106_14 {

using joan::hook::global;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_shutdown, "Game::shutdown_managers");

struct H {
    // 0x006039d0
    u32 shutdown_managers() {
        c_shutdown.hit();
        joan::orig::as_mouse_cursor(obj(d::g_mouse_cursor))->release_4941b0();  // 0x004941b0
        joan::orig::as_input(obj(d::g_mouse))->release();  // 0x00592830
        joan::orig::as_screen(obj(d::g_sprites_a46128))->close_631c60();  // 0x00631c60
        joan::orig::as_model(obj(d::g_view3d))->release_parts();  // 0x00613a10
        joan::orig::as_scene(obj(d::g_scene))->clear();  // 0x005e9b10
        joan::orig::as_texture_manager(obj(d::g_texture_array))->clear();  // 0x00609260
        joan::orig::as_tex_dir(obj(d::g_texture_dir))->clear();  // 0x00604150
        joan::orig::as_streamer(obj(d::g_async_file_loader))->shutdown();  // 0x00579040
        if (joan::orig::arg::as<u32>(joan::orig::as_display_palette(obj(d::g_vga))->is_attached()) != 0) {  // 0x0060f6d0
            joan::orig::as_model(obj(d::g_view3d))->release_parts();          // 0x00613a10
            joan::orig::as_scene(obj(d::g_scene))->clear();          // 0x005e9b10
            if (global<void*>(d::g_vga_back) != nullptr && global<u32>(d::g_a5cc60) != 0)
                joan::orig::as_image(obj(d::g_vga_back))->unlock();  // 0x0060ff20
            joan::orig::as_image(obj(d::g_a56ef0))->release();      // 0x0060fd20
            joan::orig::as_image(obj(d::g_vga_back))->release();      // 0x0060fd20
            joan::orig::as_display_palette(obj(d::g_vga))->reset();      // 0x0060f700
        }
        joan::orig::as_sys_en(obj(d::g_sys3d))->shutdown();  // 0x005f6770
        return 0;  // void callee; Game::shutdown_managers' caller drops the eax
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_14", name, addr, fn)
B("Game::shutdown_managers", 0x006039d0, &H::shutdown_managers);
#undef B

}  // namespace game_b106_14
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_603550() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_10::init_603550>()); }  // 0x603550
auto joan::orig::init_603570() -> void { joan::bind::free<&::game_b98_06::init_603570>(); }  // 0x603570
auto joan::orig::SysEn::ctor_6035c0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07::H::sysen_ctor>(this)); }  // 0x6035c0
auto joan::orig::SysEn::dtor() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b108_06::H::sysen_dtor>(this)); }  // 0x603670
auto joan::orig::Window::open(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_16::H::open_603710>(this, a0)); }  // 0x603710
auto joan::orig::Window::activate() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b107_09::H::activate>(this)); }  // 0x603780
auto joan::orig::Game::init_display_6037f0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_14::H::init_display>(this)); }  // 0x6037f0
auto joan::orig::Game::shutdown_managers() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_14::H::shutdown_managers>(this)); }  // 0x6039d0
auto joan::orig::Window::nop_603ad0() -> void { joan::bind::member<&::game_b104_10_2::H::nop_603ad0>(this); }  // 0x603ad0
auto joan::orig::Game::flush_loader() -> void { joan::bind::member<&::game_leaves81::H::flush_loader>(this); }  // 0x603af0
auto joan::orig::Game::screenshot_603b20() -> void { joan::bind::member<&::game_b108_14::H::screenshot>(this); }  // 0x603b20
auto joan::orig::Window::proc(void* a0, std::uint32_t a1, std::uint32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_11::H::window_proc>(this, a0, a1, a2, a3)); }  // 0x603e20
auto joan::orig::Sound::restart() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_03::H::sound_restart>(this)); }  // 0x603fd0
