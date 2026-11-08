// SPDX-License-Identifier: GPL-3.0-or-later
// uti3d/water.cpp: functions of the original d:\ww\uti3d\water.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (uti3d/Water.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

struct H {
    struct Six {
        float v[6];  // float members like the caller's box (game/world_s.cpp create_water)
    };
    std::uintptr_t water_init(std::uint8_t kind, std::uint32_t, Six box, std::uint32_t a, std::uint32_t b, void* c) {  // 0x00546b50 (c: the Sector*)
        jo::self<jo::Water>(this)->u32_2c = 0;
        jo::self<jo::Water>(this)->bytes_0[2] = kind;
        jo::self<jo::Water>(this)->bytes_0[3] = 1;
        std::memcpy(jo::self<jo::Water>(this)->box, &box, sizeof box);  // the region box
        jo::self<jo::Water>(this)->u32_1c = a;
        jo::self<jo::Water>(this)->u32_20 = b;
        jo::self<jo::Water>(this)->u32_30 = c;
        return reinterpret_cast<std::uintptr_t>(c);

    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Water::init", 0x00546b50, &H::water_init);
#undef B85

}  // namespace game_b85_06
}  // namespace
