// SPDX-License-Identifier: GPL-3.0-or-later
// enparticle/pevent.cpp: functions of the original d:\ww\enparticle\pevent.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enparticle/PEvent.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
// for it.

#include <cstdint>
#include "../original/helpers.hpp"

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/cxx_eh.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using u8 = std::uint8_t;

u8* next(void* ts) { return joan::orig::arg::as<u8*>(joan::orig::as_token_list(ts)->next()); }  // 0x0067acb0

}  // namespace

// ---------------------------------------- from modules/game_b111_03.cpp (game/b111_03)
namespace {
namespace game_b111_03 {

u8* peek(void* ts) { return joan::orig::arg::as<u8*>(joan::orig::as_token_list(ts)->peek()); }  // 0x0067ac80

// throw (char*) (original/cxx_eh.hpp).
[[noreturn]] void fail(img::Addr message) { throw static_cast<char*>(&img::global<char>(message)); }

using joan::orig::helpers::type;
const char* text(const u8* tok) { return jo::as_token(tok)->text; }
i32 stricmp_(const char* a, img::Addr b) {
    return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(&img::global<const char>(b)));  // _stricmp
}

JOAN_COUNTER(c_parse, "Gauge::parse");

// ------------------------------------------------------------------ Gauge::parse family
u32 gauge_parse(void* self, void* ts, img::Addr keyword, img::Addr msg_keyword,
                img::Addr msg_equals, std::uintptr_t value_parser) {
    c_parse.hit();
    if (stricmp_(text(peek(ts)), keyword) != 0) fail(msg_keyword);
    if (type(next(ts)) != 2) fail(msg_equals);
    next(ts);
    return thiscall<u32>(value_parser, ts, &jo::as_gauge(self)->value);  // +0x1b: the first member after the ParticleKey base in every key class
}
constexpr fn::Code kRange = fn::ParticleEmitterTokenizer_process_number;  // 0x0067cbf0
constexpr fn::Code kXyz = fn::ParticleEmitterTokenizer_process_vector;    // 0x0067cdf0
constexpr fn::Code kRgba = fn::ParticleEmitterTokenizer_process_color;   // 0x0067d050

struct H {
    u32 parse_size(void* ts) { return gauge_parse(this, ts, d::s_SIZE, d::s_Expecting_Size, d::s_Expecting_after_Size, fn::code_value(kRange)); }
    u32 parse_color(void* ts) { return gauge_parse(this, ts, d::s_COLOR_9dc064, d::s_Expecting_Color, d::s_Expecting_after_Color, fn::code_value(kRgba)); }
    u32 parse_red(void* ts) { return gauge_parse(this, ts, d::s_COLORRED, d::s_Expecting_ColorRed, d::s_Expecting_after_ColorRed, fn::code_value(kRange)); }
    u32 parse_green(void* ts) { return gauge_parse(this, ts, d::s_COLORGREEN, d::s_Expecting_ColorGreen, d::s_Expecting_after_ColorGreen, fn::code_value(kRange)); }
    u32 parse_blue(void* ts) { return gauge_parse(this, ts, d::s_COLORBLUE, d::s_Expecting_ColorBlue, d::s_Expecting_after_ColorBlue, fn::code_value(kRange)); }
    u32 parse_alpha(void* ts) { return gauge_parse(this, ts, d::s_COLORALPHA, d::s_Expecting_ColorAlpha, d::s_Expecting_after_ColorAlpha, fn::code_value(kRange)); }
    u32 parse_velocity(void* ts) { return gauge_parse(this, ts, d::s_VELOCITY, d::s_Expecting_Velocity, d::s_Expecting_after_Velocity, fn::code_value(kXyz)); }
    u32 parse_vel_x(void* ts) { return gauge_parse(this, ts, d::s_VELOCITYX, d::s_Expecting_VelocityX, d::s_Expecting_after_VelocityX, fn::code_value(kRange)); }
    u32 parse_vel_y(void* ts) { return gauge_parse(this, ts, d::s_VELOCITYY, d::s_Expecting_VelocityY, d::s_Expecting_after_VelocityY, fn::code_value(kRange)); }
    u32 parse_vel_z(void* ts) { return gauge_parse(this, ts, d::s_VELOCITYZ, d::s_Expecting_VelocityZ, d::s_Expecting_after_VelocityZ, fn::code_value(kRange)); }
    u32 parse_timer(void* ts) { return gauge_parse(this, ts, d::s_EVENTTIMER, d::s_Expecting_EventTimer, d::s_Expecting_after_EventTimer, fn::code_value(kRange)); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_03", name, addr, fn)
B("ParticleEventSize::process_token_stream", 0x0067e730, &H::parse_size);
B("ParticleEventColor::process_token_stream", 0x0067e7f0, &H::parse_color);
B("ParticleEventColorRed::process_token_stream", 0x0067e8b0, &H::parse_red);
B("ParticleEventColorGreen::process_token_stream", 0x0067e970, &H::parse_green);
B("ParticleEventColorBlue::process_token_stream", 0x0067ea30, &H::parse_blue);
B("ParticleEventColorAlpha::process_token_stream", 0x0067eaf0, &H::parse_alpha);
B("ParticleEventVelocity::process_token_stream", 0x0067ebb0, &H::parse_velocity);
B("ParticleEventVelocityX::process_token_stream", 0x0067ec70, &H::parse_vel_x);
B("ParticleEventVelocityY::process_token_stream", 0x0067ed30, &H::parse_vel_y);
B("ParticleEventVelocityZ::process_token_stream", 0x0067edf0, &H::parse_vel_z);
B("ParticleEventEventTimer::process_token_stream", 0x0067eeb0, &H::parse_timer);
#undef B

}  // namespace game_b111_03
}  // namespace
