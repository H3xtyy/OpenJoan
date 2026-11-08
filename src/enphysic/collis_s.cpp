// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/collis_s.cpp: functions of the original d:\ww\enphysic\collis_s.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enphysic/collis_s.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
// for it.

#include "../fp/fma.hpp"
#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b107_13.cpp (game/b107_13)
namespace {
namespace game_b107_13 {

struct Blk12 {
    u32 d[3];
};
struct Blk24 {
    u32 d[6];
};

// 0x006a0d20: args +8 a, +0xc b (24), +0x24 c (12), +0x30 d (accessed in place).
u32 JOAN_CDECL neg_call(void* a, Blk24 b, Blk12 c, Blk24 d) {
    Blk12 tmp{};
    const Blk12* r = joan::orig::arg::as<const Blk12*>(joan::orig::vec_negate(&tmp, &c));  // 0x0060cbc0
    Blk12 neg = *r;
    // result unused: 0x006a0d20 has no caller (code or data)
    return joan::orig::arg::as<u32>(joan::orig::sweep_sphere_box_697140(joan::orig::arg::mptr(a), &d, &neg, &b));  // 0x00697140
}
#define B(name, addr, fn) JOAN_REPLACE("game/b107_13", name, addr, fn)
B("box_sphere_collision", 0x006a0d20, &neg_call);
#undef B

}  // namespace game_b107_13
}  // namespace
