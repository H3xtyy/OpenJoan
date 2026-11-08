// game/unknown_errhand.cpp: functions whose original source file is unknown (formerly game/_unplaced/errhand.cpp;
// re/attribution.csv: between ww/ilib/dynarrb.cpp and ww/ilib/error.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

struct H {
    void* ctor_586950() {  // 0x00586950
        joan::orig::_set_new_handler(joan::orig::arg::mptr(fn::code_value(fn::atexit_5869a0)));  // _set_new_handler
        jo::self<jo::Heap>(this)->on_fatal = nullptr;
        jo::self<jo::Heap>(this)->on_report = nullptr;
        jo::self<jo::Heap>(this)->fn_8 = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Heap::ctor", 0x00586950, &H::ctor_586950);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
namespace {
namespace game_b103_08 {

using i32 = std::int32_t;

using joan::img::obj;

// ---------------------------------------------------------------------------- 0x005869a0
static i32 JOAN_CDECL atexit_5869a0() {
    joan::orig::as_error_log(obj(d::g_err))->mem_error();  // 0x00586af0
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("atexit_5869a0", 0x005869a0, &atexit_5869a0);
#undef B

}  // namespace game_b103_08
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

JOAN_COUNTER(c_fatal, "fatal_error");

struct H {
    // 0x005869d0
    std::int32_t fatal_error(const char* msg, const char* file, std::uint32_t line) {
        c_fatal.hit();
        const std::int32_t busy = static_cast<std::int8_t>(global<std::uint8_t>(d::g_error_busy));
        if (busy != 0) return busy;
        global<std::uint8_t>(d::g_error_busy) = 1;
        if (jo::self<jo::Error>(this)->on_fatal != nullptr) thiscall(jo::view<std::uintptr_t>(jo::self<jo::Error>(this)->on_fatal), this);
        char text[0x400];
        using Sprintf = std::int32_t(JOAN_CDECL*)(char*, const char*, ...);
        const Sprintf sprintf_fn = reinterpret_cast<Sprintf>(fn::code_value(fn::crt_sprintf));
        if (msg != nullptr) {
            sprintf_fn(text, str(d::s_Error_s_File_s_Line_d), msg, file, line);  // "Error : %s\nFile : %s\nLine : %d\n"
        } else {
            sprintf_fn(text, str(d::s_Error_on_File_s_Line_d), file, line);  // "Error on File : %s\nLine : %d\n"
        }
        if (jo::self<jo::Error>(this)->on_report != nullptr) thiscall(jo::view<std::uintptr_t>(jo::self<jo::Error>(this)->on_report), this);
        win_call(kOutputDebugStringA, static_cast<const char*>(text));
        win_call(kOutputDebugStringA, str(d::kNl));  // "\n"
        win_call(kShowCursor, 1);
        win_call(kMessageBoxA, global<void*>(d::g_main_hwnd), static_cast<const char*>(text), str(d::s_Wars_Warriors_Joan_of_Arc),
                 0x10u);  // "Wars & Warriors: Joan of Arc"
        win_call(kShowCursor, 0);
        if (jo::self<jo::Error>(this)->fn_8 != nullptr) thiscall(jo::view<std::uintptr_t>(jo::self<jo::Error>(this)->fn_8), this);
        joan::orig::libc::exit(joan::orig::arg::u32(-2));  // exit
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("fatal_error", 0x005869d0, &H::fatal_error);
#undef B

}  // namespace game_b96_05
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

JOAN_COUNTER(c_fatal, "Error::fatal");

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

// 0x00586cb0 Error::fatal(err, fmt, ...): vsprintf into a 100-byte buffer (as the original;
// no bounds check), then the message box and exit(-2).
void JOAN_CDECL error_fatal(void* err, const char* fmt, ...) {
    c_fatal.hit();
    if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_error_busy)) != 0) return;
    global<std::uint8_t>(d::g_error_busy) = 1;
    if (jo::view<std::uintptr_t>(jo::as_error(err)->on_fatal) != 0) thiscall(jo::view<std::uintptr_t>(jo::as_error(err)->on_fatal), err);
    char text[100];
    va_list args;
    va_start(args, fmt);
    joan::orig::libc::vsprintf(joan::orig::arg::str(static_cast<char*>(text)), joan::orig::arg::str(fmt), joan::orig::arg::str(static_cast<void*>(args)));  // vsprintf
    va_end(args);
    error_box(err, text);
}

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Error::fatal", 0x00586cb0, &error_fatal);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ErrorLog::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10::H::ctor_586950>(this)); }  // 0x586950
auto joan::orig::ErrorLog::fatal_error(const char* a0, const char* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_05::H::fatal_error>(this, a0, a1, a2)); }  // 0x5869d0
