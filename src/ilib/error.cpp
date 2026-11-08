// SPDX-License-Identifier: GPL-3.0-or-later
// ilib/error.cpp: functions of the original d:\ww\ilib\error.cpp (re/attribution.csv). The file name is the Restaurant
// Empire 2 engine's (ilib/error.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdarg>
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

using joan::hook::global;

using joan::img::str;

// KERNEL32 / USER32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

constexpr img::Addr kOutputDebugStringA = d::iat_KERNEL32_OutputDebugStringA;
constexpr img::Addr kShowCursor = d::iat_USER32_ShowCursor;
constexpr img::Addr kMessageBoxA = d::iat_USER32_MessageBoxA;

}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
//
//    - Error::mem_error (0x00586af0) and Error::fatal (0x00586cb0, printf-style): message box
//      "Wars & Warriors: Joan of Arc", then exit(-2) (only once: returns when already shown).
//      Win32 calls through the game's IAT slots.
namespace {
namespace game_leaves80b {

template <class R = void, class... A>
R win32_call(img::Addr slot, A... args) {  // stdcall Win32 import through its IAT slot
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}

JOAN_COUNTER(c_mem_error, "Error::mem_error");

// Tail shared by 0x00586af0 / 0x00586cb0 (both inline it): callbacks +0 / +4 run before
// the message box (called with ecx = the error object, as the original leaves it), +8 after.
void error_box(void* err, const char* text) {
    if (jo::view<std::uintptr_t>(jo::as_error(err)->on_report) != 0) thiscall(jo::view<std::uintptr_t>(jo::as_error(err)->on_report), err);
    win32_call<void>(kOutputDebugStringA, text);
    win32_call<void>(kOutputDebugStringA, &img::global<const char>(d::kNl));
    win32_call<std::int32_t>(kShowCursor, 1);
    win32_call<std::int32_t>(kMessageBoxA, global<void*>(d::g_main_hwnd), text,
                             &img::global<const char>(d::s_Wars_Warriors_Joan_of_Arc), 0x10u);  // "Wars & Warriors: Joan of Arc", MB_ICONHAND
    win32_call<std::int32_t>(kShowCursor, 0);
    if (jo::view<std::uintptr_t>(jo::as_error(err)->fn_8) != 0) thiscall(jo::view<std::uintptr_t>(jo::as_error(err)->fn_8), err);
    joan::orig::libc::exit(joan::orig::arg::u32(-2));  // exit
}

struct H {
    // 0x00586af0 (this = the error object)
    void mem_error() {
        c_mem_error.hit();
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_error_busy)) != 0) return;
        global<std::uint8_t>(d::g_error_busy) = 1;
        if (jo::view<std::uintptr_t>(jo::self<jo::Error>(this)->on_fatal) != 0) thiscall(jo::view<std::uintptr_t>(jo::self<jo::Error>(this)->on_fatal), this);
        error_box(this, &img::global<const char>(d::s_Insufficient_Memory_executio));  // "Insufficient Memory, execution interrupt."
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Error::mem", 0x00586af0, &H::mem_error);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (game/b83_08)
namespace {
namespace en3d_b83_08 {

JOAN_COUNTER(c_warning, "warning_box");

// 0x00586bc0 (cdecl, varargs)
void JOAN_CDECL warning_box(void* log, const char* fmt, ...) {
    c_warning.hit();
    if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_error_busy)) != 0) return;
    global<std::uint8_t>(d::g_error_busy) = 1;
    char text[0x190];
    va_list args;
    va_start(args, fmt);
    joan::orig::libc::vsprintf(joan::orig::arg::str(static_cast<char*>(text)), joan::orig::arg::str(fmt), joan::orig::arg::str(static_cast<void*>(args)));  // vsprintf
    va_end(args);
    if (jo::as_error(log)->on_report != nullptr) reinterpret_cast<void(JOAN_CDECL*)()>(jo::view<std::uintptr_t>(jo::as_error(log)->on_report))();
    win_call(kOutputDebugStringA, static_cast<const char*>(text));
    win_call(kOutputDebugStringA, str(d::kNl));  // "\n"
    win_call(kShowCursor, 1);
    win_call(kMessageBoxA, global<void*>(d::g_main_hwnd), static_cast<const char*>(text), str(d::s_Wars_Warriors_Joan_of_Arc), 0x10u);  // "Wars & Warriors: Joan of Arc"
    win_call(kShowCursor, 0);
    if (jo::as_error(log)->fn_8 != nullptr) reinterpret_cast<void(JOAN_CDECL*)()>(jo::view<std::uintptr_t>(jo::as_error(log)->fn_8))();
    global<std::uint8_t>(d::g_error_busy) = 0;
}

#define GAME(name, addr, fn) JOAN_REPLACE("game/b83_08", name, addr, fn)
GAME("Error::msg", 0x00586bc0, &warning_box);
#undef GAME

}  // namespace en3d_b83_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ErrorLog::mem_error() -> void { joan::bind::member<&::game_leaves80b::H::mem_error>(this); }  // 0x586af0
