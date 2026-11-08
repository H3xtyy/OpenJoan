// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/rgb.cpp: functions of the original d:\ww\en3d\rgb.cpp (re/attribution.csv). The file name is the Restaurant
// Empire 2 engine's (en3d/rgb.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include <cstdint>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    std::uint32_t* colour_to_argb(std::uint32_t* out) {  // 0x005e91e0
        *out = (static_cast<std::uint32_t>(self()[3]) << 24) | (static_cast<std::uint32_t>(self()[0]) << 16) |
               (static_cast<std::uint32_t>(self()[1]) << 8) | self()[2];
        return out;
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
HELPER16("RGBAColor::export_to", 0x005e91e0, &H::colour_to_argb);
#undef HELPER16

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- Colour --------------------------------------------------------------------------------------
    float* colour_to_float(float* out) {  // 0x005e9260: B, G, R, A bytes -> R, G, B, A floats / 255
        const float k = global<float>(d::flt_0_003921569);  // 1 / 255
        out[2] = static_cast<float>(self()[0]) * k;
        out[1] = static_cast<float>(self()[1]) * k;
        out[0] = static_cast<float>(self()[2]) * k;
        out[3] = static_cast<float>(self()[3]) * k;
        return out;
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("BGRAColor::export_to", 0x005e9260, &H::colour_to_float);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Colour::to_argb(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::colour_to_argb>(this, a0)); }  // 0x5e91e0
auto joan::orig::Colour::to_float_5e9260(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers6::H::colour_to_float>(this, a0)); }  // 0x5e9260
