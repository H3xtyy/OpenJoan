// game/unknown_syswnd.cpp: functions whose original source file is unknown (formerly game/_unplaced/syswnd.cpp;
// re/attribution.csv: between ww/ilib/sound3d.cpp and ww/ilib/txtres.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include "../platform/win32_types.hpp"
#include <cstring>
#include <cstdint>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;

using joan::hook::global;
using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

struct H {
    // 0x005a1b60
    H* ctor_5a1b60() {
        jo::self<jo::Dev>(this)->vtable = img::obj(d::vt_Dev);
        std::memset(static_cast<void*>(&jo::self<jo::Dev>(this)->instance), 0, sizeof(jo::Dev) - offsetof(jo::Dev, instance));  // memset +0x4, 0x10
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("Obj::ctor_5a1b60", 0x005a1b60, &H::ctor_5a1b60);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b106_18.cpp (game/b106_18)
namespace {
namespace game_b106_18 {

struct H {
    // ------------------------------------------------------------ 0x005a1ba0
    u32 dtor_9d8f08() {
        jo::self<jo::Dev>(this)->vtable = img::obj(d::vt_Dev);
        return joan::orig::arg::as<u32>(joan::orig::as_sys_window(this)->close());  // 0x005a1c40
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_18", name, addr, fn)
B("Obj_9d8f08::dtor", 0x005a1ba0, &H::dtor_9d8f08);
#undef B

}  // namespace game_b106_18
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

using i8 = std::int8_t;

struct Obj {};
using joan::orig::helpers::virtual_call;

struct H {
    u32 zero0() { return 0; }

    // ---------------------------------------------------------------- 0x005a1bd0
    u32 open_5a1bd0(void* a) {
        jo::self<jo::Dev>(this)->instance = a;
        jo::self<jo::Dev>(this)->closing = 0;
        if (virtual_call<u32>(this, 0x8) == 0) return 0;
        if (joan::orig::arg::as<u32>(joan::orig::as_sys_window(this)->on_open()) == 0) return 0;  // 0x005a1d40
        jo::self<jo::Dev>(this)->is_open = 1;
        return 1;
    }
    // ---------------------------------------------------------------- 0x005a1c40
    std::uintptr_t close_5a1c40() {
        if (static_cast<i8>(jo::self<jo::Dev>(this)->closing) == 0) return virtual_call<u32>(this, 0xc);
        if (static_cast<i8>(jo::self<jo::Dev>(this)->is_open) != 0) {
            joan::orig::as_sys_window(this)->on_close();  // 0x005a1d60
            jo::self<jo::Dev>(this)->is_open = 0;
        }
        return reinterpret_cast<std::uintptr_t>(this);

    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Dev::open_5a1bd0", 0x005a1bd0, &H::open_5a1bd0);
B("Dev::close_5a1c40", 0x005a1c40, &H::close_5a1c40);
B("Dev::false_5a1ca0", 0x005a1ca0, &H::zero0);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

// Win32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

constexpr img::Addr kDestroyWindow = d::iat_USER32_DestroyWindow;
constexpr img::Addr kGetMessageA = d::iat_USER32_GetMessageA;
constexpr img::Addr kTranslateMessage = d::iat_USER32_TranslateMessage;
constexpr img::Addr kDispatchMessageA = d::iat_USER32_DispatchMessageA;

JOAN_COUNTER(c_destroy, "Window::destroy");

struct H {
    // ------------------------------------------------------------------------ 0x005a1cc0
    // eax: this when +8 is 0 at the second test, else 0 (the GetMessageA result ending the loop).
    void* destroy_window() {
        c_destroy.hit();
        if (jo::self<jo::Window>(this)->hwnd != nullptr) win_call<i32>(kDestroyWindow, jo::self<jo::Window>(this)->hwnd);
        jo::self<jo::Window>(this)->is_open = 0;
        alignas(void*) u8 msg[sizeof(void*) == 4 ? 0x1c : 0x30];  // MSG (x64: 0x30 bytes, hwnd / wParam / lParam 8 each)
        // +8 is tested once; the loop (0x005a1d1a -> 0x005a1cf2) only ends on GetMessageA == 0.
        if (jo::self<jo::Window>(this)->hwnd != nullptr) {
            for (;;) {
                if (win_call<i32>(kGetMessageA, msg, static_cast<void*>(nullptr), 0u, 0u) == 0) break;
                win_call<i32>(kTranslateMessage, msg);
                win_call<i32>(kDispatchMessageA, msg);
            }
            return nullptr;
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Window::destroy", 0x005a1cc0, &H::destroy_window);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
namespace {
namespace game_b104_09 {

struct H {
    void stub_void() {}
    i32 true_5a1d40() { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("Obj::true_5a1d40", 0x005a1d40, &H::true_5a1d40);
B("Obj::stub_5a1d60", 0x005a1d60, &H::stub_void);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

struct H {
    // 0x005a1d80
    void mark_d() {
        if (jo::view<std::int8_t>(jo::self<jo::Dev>(this)->marked) == 0) jo::self<jo::Dev>(this)->marked = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Obj::mark_d", 0x005a1d80, &H::mark_d);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

struct H {
    // 0x005a1db0
    void unmark_d() {
        if (jo::view<std::int8_t>(jo::self<jo::Dev>(this)->marked) != 0) jo::self<jo::Dev>(this)->marked = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("Obj::unmark_d", 0x005a1db0, &H::unmark_d);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
namespace {
namespace game_b97_05 {

using joan::call::stdcall_import;
// Virtual thiscall through the window's vtable (pointer-sized entries in the data image), slot = entry index.
template <class R = void, class... A>
R vcall(void* p, std::int32_t slot, A... args) {
    return thiscall<R>(static_cast<const std::uintptr_t*>(jo::self<jo::Window>(p)->vtable)[slot], p, args...);
}

JOAN_COUNTER(c_wndproc, "Window::wndproc");

struct H {
    // 0x005a1e20
    std::int32_t wndproc(void* hwnd, std::uint32_t msg, std::uint32_t wparam, std::uint32_t lparam) {
        c_wndproc.hit();
        switch (msg) {
            case 0x1:  // WM_CREATE
                jo::self<jo::Window>(this)->hwnd = hwnd;
                break;
            case 0x2:  // WM_DESTROY
                jo::self<jo::Window>(this)->hwnd = nullptr;
                jo::self<jo::Window>(this)->closing = 1;
                vcall(this, 0);   // +0x0
                jo::self<jo::Window>(this)->closing = 0;
                stdcall_import(d::iat_USER32_PostQuitMessage, 0);  // PostQuitMessage
                break;
            case 0xf:  // WM_PAINT
                jo::self<jo::Window>(this)->need_paint = 1;
                break;
            case 0x1c:  // WM_ACTIVATEAPP
                jo::self<jo::Window>(this)->active = static_cast<std::uint8_t>(wparam);
                if (jo::view<std::int8_t>(jo::self<jo::Window>(this)->active) != 0) {
                    vcall(this, 5);   // +0x14
                    jo::self<jo::Window>(this)->need_paint = 1;
                } else {
                    vcall(this, 4);   // +0x10
                }
                break;
            case 0x50:  // WM_INPUTLANGCHANGEREQUEST
                return 0;
            default:
                break;
        }
        // DefWindowProcA(HWND, UINT, WPARAM, LPARAM) -> LRESULT: pointer-sized on x64 (the game's 32-bit values widen)
        return static_cast<std::int32_t>(stdcall_import<std::intptr_t>(d::iat_USER32_DefWindowProcA, hwnd, msg, std::uintptr_t{wparam}, static_cast<std::intptr_t>(static_cast<std::int32_t>(lparam))));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("Window::wndproc", 0x005a1e20, &H::wndproc);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SysWindow::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_05::H::ctor_5a1b60>(this)); }  // 0x5a1b60
auto joan::orig::SysWindow::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_18::H::dtor_9d8f08>(this)); }  // 0x5a1ba0
auto joan::orig::SysWindow::open(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_16::H::open_5a1bd0>(this, a0)); }  // 0x5a1bd0
auto joan::orig::SysWindow::close() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_16::H::close_5a1c40>(this)); }  // 0x5a1c40
auto joan::orig::SysWindow::destroy_window() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10::H::destroy_window>(this)); }  // 0x5a1cc0
auto joan::orig::SysWindow::on_open() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_09::H::true_5a1d40>(this)); }  // 0x5a1d40
auto joan::orig::SysWindow::on_close() -> void { joan::bind::member<&::game_b104_09::H::stub_void>(this); }  // 0x5a1d60
auto joan::orig::SysWindow::set_suspended() -> void { joan::bind::member<&::game_b94_05::H::mark_d>(this); }  // 0x5a1d80
auto joan::orig::SysWindow::clear_suspended() -> void { joan::bind::member<&::game_b95_05::H::unmark_d>(this); }  // 0x5a1db0
auto joan::orig::SysWindow::wndproc(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_05::H::wndproc>(this, a0, a1, a2, a3)); }  // 0x5a1e20
