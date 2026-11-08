// game/unknown_vgalock_mb.cpp: functions whose original source file is unknown (formerly game/_unplaced/vgalock_mb.cpp;
// re/attribution.csv: between ww/en3d/vgalock.cpp and ww/en3d/view3d.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
template <class R = void, class... A>
R win32_call(img::Addr slot, A... args) {  // stdcall Win32 import through its IAT slot
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}
using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

constexpr img::Addr kSetCursorPos = d::iat_USER32_SetCursorPos;
constexpr img::Addr kShowCursor = d::iat_USER32_ShowCursor;

JOAN_COUNTER(c_cursor, "Cursor::restore");

struct H {
    // 0x006134d0 Cursor::restore
    void* cursor_restore() {
        c_cursor.hit();
        win32_call<std::int32_t>(kSetCursorPos, global<std::int32_t>(d::g_mouse_x), global<std::int32_t>(d::g_mouse_y));
        jo::view<std::int32_t>(jo::self<jo::Cursor>(this)->u32_0) = static_cast<std::int32_t>(global<std::int8_t>(d::g_a4860a));
        if (jo::view<std::int32_t>(jo::self<jo::Cursor>(this)->u32_0) == 0) joan::orig::as_cursor(obj(d::g_mouse))->set_pos();  // 0x00592bd0
        if (global<std::int32_t>(d::g_sys3d) >= 2) joan::orig::as_sys_en(obj(d::g_sys3d))->flush_extra();  // 0x005ff180
        if (global<std::int32_t>(d::g_sys3d) >= 3 && (joan::orig::arg::as<std::uint32_t>(joan::orig::as_canvas(obj(d::g_vga_back))->valid()) & 0xff) != 0)  // 0x0056ddc0
            joan::orig::as_bit_writer(obj(d::g_vga_back))->put();  // 0x0060ff70
        win32_call<std::int32_t>(kShowCursor, 1);
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Cursor::restore", 0x006134d0, &H::cursor_restore);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using u32 = std::uint32_t;
using i32 = std::int32_t;

JOAN_COUNTER(c_mouse_hide, "Mouse::hide_613580");

struct alignas(4) Point {
    i32 x, y;
};

struct H {
    // ------------------------------------------------------------------ 0x00613580
    u32 mouse_hide() {
        c_mouse_hide.hit();
        Point pt{global<i32>(d::g_mouse_x), global<i32>(d::g_mouse_y)};  // see header note
        win32_call<i32>(d::iat_USER32_GetCursorPos, &pt);                            // GetCursorPos
        global<i32>(d::g_mouse_x) = pt.x;
        global<i32>(d::g_mouse_y) = pt.y;
        win32_call<i32>(d::iat_USER32_ShowCursor, 0);  // ShowCursor(FALSE)
        if (global<i32>(d::g_sys3d) >= 2) joan::orig::as_sys_en(obj(d::g_sys3d))->flush_all();  // 0x005ff210
        if (global<i32>(d::g_sys3d) >= 3 && (joan::orig::arg::as<u32>(joan::orig::as_canvas(obj(d::g_vga_back))->valid()) & 0xff) != 0)  // 0x0056ddc0
            joan::orig::as_image(obj(d::g_vga_back))->pop_lock();                                                  // 0x0060ffd0
        if (jo::self<jo::Cursor>(this)->u32_0 == 0) joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
        joan::orig::as_input(obj(d::g_mouse))->pop_event();                // 0x005932e0
        return joan::orig::arg::as<u32>(joan::orig::as_input(obj(d::g_mouse))->pop_event());         // 0x005932e0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Mouse::hide", 0x00613580, &H::mouse_hide);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Cursor::restore() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_06::H::cursor_restore>(this)); }  // 0x6134d0
auto joan::orig::Cursor::hide() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_03::H::mouse_hide>(this)); }  // 0x613580
