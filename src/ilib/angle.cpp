// SPDX-License-Identifier: GPL-3.0-or-later
// ilib/angle.cpp: functions of the original d:\ww\ilib\angle.cpp (re/attribution.csv). The file name is the Restaurant
// Empire 2 engine's (ilib/angle.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

using joan::hook::global;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (game/b85_04)
namespace {
namespace en3d_b85_04 {

struct H {
    // 0x00576ac0
    float angle_cos() {
        if (global<std::int32_t>(d::g_angle_init_table_flag) == 0) joan::orig::angle::init_tables();  // 0x00576cb0
        const float* const t = &img::global<const float>(d::g_angle_sin_table);
        const std::uint32_t b = jo::self<jo::ByteAngle>(this)->value;
        const std::uint32_t i = b & 0x3fu;
        switch (b & 0xc0u) {
            case 0x00: return t[0x40 - i];
            case 0x40: return -t[i];
            case 0x80: return -t[0x40 - i];
            default: return t[i];
        }
    }

    // 0x005768d0
    float angle_sin() {
        if (global<std::int32_t>(d::g_angle_init_table_flag) == 0) joan::orig::angle::init_tables();  // 0x00576cb0
        const float* const t = &img::global<const float>(d::g_angle_sin_table);
        const std::uint32_t b = jo::self<jo::ByteAngle>(this)->value;
        const std::uint32_t i = b & 0x3fu;
        switch (b & 0xc0u) {
            case 0x00: return t[b];
            case 0x40: return t[0x40 - i];
            case 0x80: return -t[i];
            default: return -t[0x40 - i];
        }
    }

    // 0x00576240
    void angle_set(float a) {
        const joan::hook::x87::Fpu fpu{true};
        const double v = fpu.mul(static_cast<double>(a), 40.74366543152521);  // qword 0x009d8640
        jo::self<jo::ByteAngle>(this)->value = static_cast<std::uint8_t>(joan::hook::x87::ftol(v));
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b85_04", name, addr, fn)
GAME("AngleI::cos", 0x00576ac0, &H::angle_cos);
GAME("AngleI::sin", 0x005768d0, &H::angle_sin);
GAME("AngleI::set_radian", 0x00576240, &H::angle_set);
#undef GAME

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (game/b86_04)
namespace {
namespace en3d_b86_04 {

JOAN_COUNTER(c_angle_init, "Angle::init_tables");

struct H {
    // 0x00576cb0 (cdecl, no arguments)
    static void JOAN_CDECL angle_init() {
        c_angle_init.hit();
        if (global<std::int32_t>(d::g_angle_init_table_flag) != 0) return;
        const joan::hook::x87::Fpu fpu{true};
        const double step = global<double>(d::dbl_0_02454369260617026);  // pi / 128
        for (std::int32_t i = 0; i < 0x40; ++i) {
            const double x = fpu.mul(static_cast<double>(i), step);
            global<float>(d::g_angle_sin_table + i * 4) = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(x)));  // sin, fstp dword
        }
        global<std::uint32_t>(d::g_a641ec) = 0x3f800000u;
        global<std::int32_t>(d::g_angle_init_table_flag) = 1;
        for (std::int32_t i = 0; i < 0x20; ++i) {
            const double x = fpu.mul(static_cast<double>(i), step);
            global<float>(d::g_a641f0 + i * 4) = joan::orig::arg::as<float>(joan::orig::libc::tan(joan::orig::arg::f64(x)));  // tan, fstp dword
        }
        global<std::uint32_t>(d::g_a64270) = 0x3f800000u;
        global<std::int32_t>(d::g_angle_init_table_flag) = 1;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b86_04", name, addr, fn)
GAME("AngleI::init_tables", 0x00576cb0, &H::angle_init);
#undef GAME

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

struct H {
    float angle_radians() {  // 0x00576280
        const Fpu fpu{true};
        return static_cast<float>(fpu.mul(static_cast<double>(jo::self<jo::ByteAngle>(this)->value), 0.02454369260617026));  // qword 0x9d8650
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("AngleI::get_radian", 0x00576280, &H::angle_radians);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

struct H {
    // 0x005762c0
    void set_byte(float a) {
        const joan::hook::x87::Fpu fpu{true};
        const double v = fpu.mul(static_cast<double>(a), static_cast<double>(global<float>(d::flt_0_7111111)));
        jo::self<jo::ByteAngle>(this)->value = static_cast<std::uint8_t>(joan::hook::x87::ftol(v));
    }

    // 0x00576300
    float byte_to_float() {
        const joan::hook::x87::Fpu fpu{true};
        return static_cast<float>(fpu.mul(static_cast<double>(jo::self<jo::ByteAngle>(this)->value), global<double>(d::dbl_9d8660)));
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("AngleI::set_degree", 0x005762c0, &H::set_byte);
M("AngleI::get_degree", 0x00576300, &H::byte_to_float);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Angle::set(float a0) -> void { joan::bind::member<&::en3d_b85_04::H::angle_set>(this, a0); }  // 0x576240
auto joan::orig::Angle::sin() -> float { return joan::bind::conv<float>(joan::bind::member<&::en3d_b85_04::H::angle_sin>(this)); }  // 0x5768d0
auto joan::orig::Angle::cos() -> float { return joan::bind::conv<float>(joan::bind::member<&::en3d_b85_04::H::angle_cos>(this)); }  // 0x576ac0
auto joan::orig::angle::init_tables() -> void { joan::bind::free<&::en3d_b86_04::H::angle_init>(); }  // 0x576cb0
auto joan::orig::Angle::radians() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b93_03::H::angle_radians>(this)); }  // 0x576280
auto joan::orig::Angle::set_byte(float a0) -> void { joan::bind::member<&::game_b90_04::H::set_byte>(this, a0); }  // 0x5762c0
auto joan::orig::Angle::byte_to_float() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b90_04::H::byte_to_float>(this)); }  // 0x576300
