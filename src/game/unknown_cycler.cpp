// game/unknown_cycler.cpp: functions whose original source file is unknown (formerly game/_unplaced/misc_634e40.cpp;
// re/attribution.csv: between ww/en2d/widget2d.cpp and ww/uti3d/blkarrb.cpp). They move to their original file when
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
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i16 = std::int16_t;
using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b101_12.cpp (game/b101_12)
namespace {
namespace game_b101_12 {

using joan::hook::global;
using i8 = std::int8_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using u16 = std::uint16_t;

JOAN_COUNTER(c_cycler, "Cycler::step");

struct H {
    void sort_once() {
        // x64 (unreached): items is an array of pointers to shorts (cmp_short_pp compares *a[i]; 4-byte elements on x86) and
        // Cycler::current holds one of them; both need pointer width. Nothing in the game constructs a Cycler (no caller of
        // Cycler::step in src), so this path is not reached; left as is until the layout spec has a pointer-width `current`.
        if (jo::self<jo::Cycler>(this)->sorted == 0) {
            joan::orig::libc::qsort(joan::orig::arg::mptr(jo::self<jo::Cycler>(this)->items), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Cycler>(this)->count)), 4u, reinterpret_cast<void*>(fn::code_value(fn::cmp_short_pp)));  // qsort
            jo::self<jo::Cycler>(this)->sorted = 1;
        }
    }

    // 0x00634e40
    std::uintptr_t cycler_step() {  // returns the current item (a pointer; u32 on x86)
        c_cycler.hit();
        if (static_cast<i32>(jo::self<jo::Cycler>(this)->count) <= 1) return 0;
        const u16 mode = static_cast<u16>(global<u16>(d::g_a51d1a) & 3);
        const i8 prev_key = static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_ai(obj(d::g_keys_a51806))->is_ready(8)));  // 0x00597ed0
        const i8 next_key = static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_ai(obj(d::g_keys_a51806))->is_ready(9)));
        i8 dir = 0;
        if (global<i32>(d::g_key) == 9) {
            dir = mode != 0 ? -1 : 1;
        } else if (prev_key != 0 && jo::self<jo::Cycler>(this)->prev_key == 0) {
            dir = -1;
        } else if (next_key != 0 && jo::self<jo::Cycler>(this)->next_key == 0) {
            dir = 1;
        }
        jo::self<jo::Cycler>(this)->prev_key = prev_key;
        jo::self<jo::Cycler>(this)->next_key = next_key;
        if (dir == -1) {
            sort_once();
            for (i16 i = 0; static_cast<i32>(i) < static_cast<i32>(jo::self<jo::Cycler>(this)->count); i = static_cast<i16>(i + 1)) {
                void** const a = jo::view<void**>(jo::self<jo::Cycler>(this)->items);
                if (a[i] != jo::self<jo::Cycler>(this)->current) continue;
                void* const v = static_cast<i32>(i) > 0 ? a[i - 1] : a[static_cast<i32>(jo::self<jo::Cycler>(this)->count) - 1];
                jo::self<jo::Cycler>(this)->current = v;
                return reinterpret_cast<std::uintptr_t>(v);
            }
            jo::self<jo::Cycler>(this)->current = jo::view<void**>(jo::self<jo::Cycler>(this)->items)[static_cast<i32>(jo::self<jo::Cycler>(this)->count) - 1];
            return reinterpret_cast<std::uintptr_t>(jo::self<jo::Cycler>(this)->current);
        }
        if (dir == 1) {
            sort_once();
            for (i16 i = 0; static_cast<i32>(i) < static_cast<i32>(jo::self<jo::Cycler>(this)->count); i = static_cast<i16>(i + 1)) {
                void** const a = jo::view<void**>(jo::self<jo::Cycler>(this)->items);
                if (a[i] != jo::self<jo::Cycler>(this)->current) continue;
                void* const v = static_cast<i32>(i) < static_cast<i32>(jo::self<jo::Cycler>(this)->count) - 1 ? a[i + 1] : a[0];
                jo::self<jo::Cycler>(this)->current = v;
                return reinterpret_cast<std::uintptr_t>(v);
            }
            jo::self<jo::Cycler>(this)->current = jo::view<void**>(jo::self<jo::Cycler>(this)->items)[0];
            return reinterpret_cast<std::uintptr_t>(jo::self<jo::Cycler>(this)->current);
        }
        return 0;
    }
};

#define R12(name, addr, fn) JOAN_REPLACE("game/b101_12", name, addr, fn)
R12("Cycler::step", 0x00634e40, &H::cycler_step);
#undef R12

}  // namespace game_b101_12
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

// ------------------------------------------------------------------ 0x00635130
i32 JOAN_CDECL cmp_short_pp(i16** a, i16** b) {
    const i16* const pa = *a;
    const i16* const pb = *b;
    if (static_cast<i32>(*pa) < static_cast<i32>(*pb)) return -1;
    if (static_cast<i32>(*pa) > static_cast<i32>(*pb)) return 1;
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("cmp_short_pp", 0x00635130, &cmp_short_pp);
#undef B

}  // namespace game_b102_12
}  // namespace
