// game/unknown_msgbox.cpp: functions whose original source file is unknown (formerly game/_unplaced/msgbox.cpp;
// re/attribution.csv: between ww/ilib/vqueue.cpp and ww/elib/jpeg.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
namespace {
namespace game_b104_09 {

struct H {
    void stub_void() {}
    H* self_5a8ba0() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("Obj::self_5a8ba0", 0x005a8ba0, &H::self_5a8ba0);
B("Obj::stub_5a8bc0", 0x005a8bc0, &H::stub_void);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

using u8 = std::uint8_t;

JOAN_COUNTER(c_msgbox, "Game::message_box");

struct H {
    // --------------------------------------------------------------------- 0x005a8be0
    i32 message_box(const char* text, void* hwnd, u32 flags) {
        c_msgbox.hit();
        void* wnd = hwnd;
        if (wnd == nullptr) wnd = global<void*>(d::g_main_hwnd);
        alignas(4) u8 guard[8] = {};
        joan::orig::as_cursor(guard)->restore();  // 0x006134d0
        if (flags == 0) {
            flags |= 0x34;
            if (wnd == nullptr) flags |= 0x2000;
        }
        using MsgBox = i32(JOAN_STDCALL*)(void*, const char*, const char*, u32);
        const MsgBox box = reinterpret_cast<MsgBox>(img::global<std::uintptr_t>(d::iat_USER32_MessageBoxA));  // USER32!MessageBoxA
        const i32 r = box(wnd, text, str(d::s_Wars_Warriors_Joan_of_Arc), flags);
        const i32 result = (r == 1 || r == 6) ? 1 : 0;
        joan::orig::as_cursor(guard)->hide();  // 0x00613580
        return result;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Game::message_box", 0x005a8be0, &H::message_box);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

template <class R = void, class... A>
R api_call(img::Addr iat_slot, A... args) {  // Win32 import through its IAT slot
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(iat_slot))(args...);
}

struct H {
    // ------------------------------------------------------------------ 0x005a8cf0
    void message_box(const char* text, void* hwnd, u32 flags) {
        if (hwnd == nullptr) hwnd = global<void*>(d::g_main_hwnd);
        u32 guard[2] = {0, 0};             // [ebp-0xc] cursor guard (4 bytes used)
        joan::orig::as_cursor(guard)->restore();  // 0x006134d0
        if (flags == 0) {
            flags |= 0x20u;
            if (hwnd == nullptr) flags |= 0x2000u;
        }
        api_call<i32>(d::iat_USER32_MessageBoxA, hwnd, text, str(d::s_Wars_Warriors_Joan_of_Arc), flags);  // MessageBoxA
        joan::orig::as_cursor(guard)->hide();  // 0x00613580
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Util::message_box", 0x005a8cf0, &H::message_box);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Game::message_box(const char* a0, void* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_16::H::message_box>(this, a0, a1, a2)); }  // 0x5a8be0
auto joan::orig::MessageBoxHelper::show(const char* a0, void* a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b108_13::H::message_box>(this, a0, a1, a2); }  // 0x5a8cf0
