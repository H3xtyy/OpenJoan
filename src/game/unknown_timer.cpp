// game/unknown_timer.cpp: functions whose original source file is unknown (formerly game/_unplaced/timer.cpp;
// re/attribution.csv: between ww/ilib/flc.cpp and ww/ilib/gameset.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b99_06.cpp (game/b99_06)
namespace {
namespace game_b99_06 {

JOAN_COUNTER(c_timer_init, "Timer::init");

struct H {
    // 0x0058a830
    void timer_init(i32 a, i32 b, u32 c, u32 d) {
        c_timer_init.hit();
        jo::self<jo::Timer>(this)->initialized = c;
        jo::self<jo::Timer>(this)->rate = a;
        jo::self<jo::Timer>(this)->rate_ms = 1000 / a;
        jo::self<jo::Timer>(this)->rate2 = b;
        jo::self<jo::Timer>(this)->rate2_ms = 1000 / b;
        jo::self<jo::Timer>(this)->paused = 0;
        jo::self<jo::Timer>(this)->time_offset = d;
        jo::self<jo::Timer>(this)->lag_ms = 0;
        jo::self<jo::Timer>(this)->lag_frames = 0;
        jo::self<jo::Timer>(this)->rate_ticks = 0;
        jo::self<jo::Timer>(this)->on_time_ticks = 0;
        jo::self<jo::Timer>(this)->skip_frame = 0;
        jo::self<jo::Timer>(this)->pause_marked = 0;
        jo::self<jo::Timer>(this)->fps_time = d;
        jo::self<jo::Timer>(this)->frame_count = 0;
        jo::self<jo::Timer>(this)->fps = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_06", name, addr, fn)
B("Timer::init", 0x0058a830, &H::timer_init);
#undef B

}  // namespace game_b99_06
}  // namespace

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
namespace {
namespace game_b103_08 {

struct H {
    i32 set_rate_58a920(i32 r) {
        jo::self<jo::Timer>(this)->rate = r;
        const i32 q = 1000 / r;
        jo::self<jo::Timer>(this)->rate_ms = q;
        return q;
    }
    i32 set_rate_58a960(i32 r) {
        jo::self<jo::Timer>(this)->rate2 = r;
        const i32 q = 1000 / r;
        jo::self<jo::Timer>(this)->rate2_ms = q;
        return q;
    }
    H* pause_on() {
        jo::self<jo::Timer>(this)->paused = 1;
        return this;
    }
    H* pause_off() {
        jo::self<jo::Timer>(this)->paused = 0;
        return this;
    }
    H* mark_58aa00(u32 t) {
        if (jo::self<jo::Timer>(this)->pause_marked == 0) {
            jo::self<jo::Timer>(this)->pause_start = t;
            jo::self<jo::Timer>(this)->pause_marked = 1;
        }
        return this;
    }
    H* mark_58aa40(u32 t) {
        if (jo::self<jo::Timer>(this)->pause_marked != 0) {
            jo::self<jo::Timer>(this)->time_offset = jo::self<jo::Timer>(this)->time_offset + (t - jo::self<jo::Timer>(this)->pause_start);
            jo::self<jo::Timer>(this)->pause_marked = 0;
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Flc::set_rate_58a920", 0x0058a920, &H::set_rate_58a920);
B("Flc::set_rate_58a960", 0x0058a960, &H::set_rate_58a960);
B("Flc::pause_on", 0x0058a9a0, &H::pause_on);
B("Flc::pause_off", 0x0058a9d0, &H::pause_off);
B("Flc::mark_58aa00", 0x0058aa00, &H::mark_58aa00);
B("Flc::mark_58aa40", 0x0058aa40, &H::mark_58aa40);
#undef B

}  // namespace game_b103_08
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
namespace {
namespace game_b97_05 {

struct H {
    // 0x0058aa90
    std::uint32_t free_space(std::uint32_t pos) {
        if (jo::self<jo::FpsTimer>(this)->paused > 0) return 0;
        const std::uint32_t v = jo::self<jo::FpsTimer>(this)->use_alt != 0 ? jo::self<jo::FpsTimer>(this)->frame_ms_alt : jo::self<jo::FpsTimer>(this)->frame_ms;
        if (pos - jo::self<jo::FpsTimer>(this)->last_time > v) return 0;
        return v - (pos - jo::self<jo::FpsTimer>(this)->last_time);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("Ring::free_space", 0x0058aa90, &H::free_space);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

using joan::hook::global;

JOAN_COUNTER(c_fps, "FpsTimer::update");

struct H {
    // ------------------------------------------------------------------------ 0x0058ab10
    void fps_update(u32 now) {
        c_fps.hit();
        const u32 limit = (jo::self<jo::FpsTimer>(this)->use_alt != 0) ? jo::self<jo::FpsTimer>(this)->frame_ms_alt : jo::self<jo::FpsTimer>(this)->frame_ms;
        if (jo::self<jo::FpsTimer>(this)->paused > 0) {
            jo::self<jo::FpsTimer>(this)->lag = jo::self<jo::FpsTimer>(this)->lag + (now - jo::self<jo::FpsTimer>(this)->last_time);
        } else if (now - jo::self<jo::FpsTimer>(this)->last_time > limit) {
            jo::self<jo::FpsTimer>(this)->lag = jo::self<jo::FpsTimer>(this)->lag + (now - jo::self<jo::FpsTimer>(this)->last_time - limit);
        }
        jo::self<jo::FpsTimer>(this)->lag = 0;
        jo::self<jo::FpsTimer>(this)->skip_frame = (jo::self<jo::FpsTimer>(this)->paused < jo::self<jo::FpsTimer>(this)->max_lag_frames && jo::self<jo::FpsTimer>(this)->lag > limit) ? 1u : 0u;
        jo::self<jo::FpsTimer>(this)->last_time = now;
        if (now - jo::self<jo::FpsTimer>(this)->fps_time >= 1000u) {
            const joan::hook::x87::Fpu fpu{true};
            const double frames = static_cast<double>(jo::self<jo::FpsTimer>(this)->frame_count);          // fild qword (hi 0)
            const double span = static_cast<double>(static_cast<i32>(now - jo::self<jo::FpsTimer>(this)->fps_time));  // fidiv
            const double q = fpu.div(frames, span);
            jo::self<jo::FpsTimer>(this)->fps = static_cast<float>(fpu.mul(q, global<float>(d::flt_1000)));  // 1000.0
            jo::self<jo::FpsTimer>(this)->fps_time = now;
            jo::self<jo::FpsTimer>(this)->frame_count = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("FpsTimer::update", 0x0058ab10, &H::fps_update);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

struct H {
    // 0x0058ac80 / 0x0058acb0
    std::int32_t empty_14() { return jo::view<std::int32_t>(jo::self<jo::Timer>(this)->paused) == 0 ? 1 : 0; }
    std::int32_t empty_2c() { return jo::view<std::int32_t>(jo::self<jo::Timer>(this)->skip_frame) == 0 ? 1 : 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Obj::empty_14", 0x0058ac80, &H::empty_14);
B("Obj::empty_2c", 0x0058acb0, &H::empty_2c);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

using joan::call::thiscall;

JOAN_COUNTER(c_stats_tick, "Stats::tick");

struct H {
    // 0x0058ace0
    void stats_tick() {
        c_stats_tick.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_timer(this)->flag_14_clear()) != 0) jo::view<std::int32_t>(jo::self<jo::Timer>(this)->rate_ticks) += 1;  // 0x0058ac80
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_timer(this)->not_lagging()) != 0) jo::view<std::int32_t>(jo::self<jo::Timer>(this)->on_time_ticks) += 1;  // 0x0058acb0
        const std::uint32_t n = jo::self<jo::Timer>(this)->paused != 0 ? jo::view<std::uint32_t>(jo::self<jo::Timer>(this)->rate2_ms) : jo::view<std::uint32_t>(jo::self<jo::Timer>(this)->rate_ms);
        if (jo::self<jo::Timer>(this)->skip_frame != 0) {
            jo::view<std::int32_t>(jo::self<jo::Timer>(this)->lag_frames) += 1;
            jo::self<jo::Timer>(this)->lag_ms = jo::self<jo::Timer>(this)->lag_ms - n;
            jo::self<jo::Timer>(this)->lag_ms = jo::self<jo::Timer>(this)->lag_ms < n ? jo::self<jo::Timer>(this)->lag_ms : n;
        } else {
            jo::self<jo::Timer>(this)->lag_frames = 0;
        }
        jo::view<std::int32_t>(jo::self<jo::Timer>(this)->frame_count) += 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("Stats::tick", 0x0058ace0, &H::stats_tick);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Timer::init(std::int32_t a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3) -> void { joan::bind::member<&::game_b99_06::H::timer_init>(this, a0, a1, a2, a3); }  // 0x58a830
auto joan::orig::Timer::remaining_ms(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_05::H::free_space>(this, a0)); }  // 0x58aa90
auto joan::orig::Timer::update_fps(std::uint32_t a0) -> void { joan::bind::member<&::game_b100_10::H::fps_update>(this, a0); }  // 0x58ab10
auto joan::orig::Timer::flag_14_clear() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_05::H::empty_14>(this)); }  // 0x58ac80
auto joan::orig::Timer::not_lagging() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_05::H::empty_2c>(this)); }  // 0x58acb0
auto joan::orig::Stats::tick() -> void { joan::bind::member<&::game_b95_05::H::stats_tick>(this); }  // 0x58ace0
