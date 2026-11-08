// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/vga.cpp: functions of the original d:\ww\en3d\vga.cpp (re/attribution.csv). The file name is the Restaurant
// Empire 2 engine's (en3d/vga.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using joan::call::thiscall;
using joan::call::stdcall_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using u16 = std::uint16_t;

using joan::img::obj;

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

JOAN_COUNTER(c_display_attach, "Display::attach");

struct H {
    // ------------------------------------------------------------------ 0x0060f5c0
    i32 display_attach(void* device) {
        c_display_attach.hit();
        jo::view<void*>(jo::self<jo::Display>(this)->device) = device;
        com_call<u32>(jo::view<void*>(jo::self<jo::Display>(this)->device), 1);  // AddRef
        jo::self<jo::Display>(this)->u8_2 = 0;
        jo::self<jo::Display>(this)->state = 3;
        jo::self<jo::Display>(this)->flag = 0;
        jo::self<jo::Display>(this)->pixel_format = -1;
        u32 mode[4] = {};  // D3DDISPLAYMODE (format +0xc read only on success)
        if (com_call<i32>(jo::view<void*>(jo::self<jo::Display>(this)->device), 8, static_cast<void*>(mode)) == 0) {  // GetDisplayMode
            jo::self<jo::Display>(this)->pixel_format = joan::orig::arg::as<i32>(joan::orig::format::bit_info(joan::orig::arg::u32(mode[3])));              // 0x005fff90
        }
        if (jo::self<jo::Display>(this)->pixel_format == -1) {
            joan::orig::warning_box(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Cannot_determine_the_pixel_f));  // 0x00586bc0 log
            jo::self<jo::Display>(this)->pixel_format = 3;
        }
        joan::orig::set_pixel_format_5706f0(joan::orig::arg::i32(jo::self<jo::Display>(this)->pixel_format));  // 0x005706f0
        joan::orig::build_shade_tables(joan::orig::arg::i32(jo::self<jo::Display>(this)->pixel_format));  // 0x0056fdb0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Vga::init_existing", 0x0060f5c0, &H::display_attach);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::DisplayPalette::attach(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_18::H::display_attach>(this, a0)); }  // 0x60f5c0
