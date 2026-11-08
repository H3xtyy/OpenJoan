// game/unknown_calendar.cpp: functions whose original source file is unknown (formerly game/_unplaced/calendar.cpp;
// re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::img::obj;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::at;

using joan::orig::helpers::memset_;

JOAN_COUNTER(c_open_495290, "Obj::open_495290");

struct H {
    // ------------------------------------------------------------------ 0x00495230
    void* zero_50() {
        memset_(this, 0, 0x50);
        return this;
    }

    // ------------------------------------------------------------------ 0x00495290
    void* open_495290(u32 a, u32 b, u32 c) {
        c_open_495290.hit();
        joan::orig::as_calendar(this)->clear();  // 0x004953b0
        memset_(this, 0, 0x50);
        jo::view<u32>(jo::self<jo::Calendar>(this)->day_of_month) = c;
        jo::view<u32>(jo::self<jo::Calendar>(this)->month) = b;
        jo::view<u32>(jo::self<jo::Calendar>(this)->year) = a;
        jo::self<jo::Calendar>(this)->start_julian = joan::orig::arg::as<u32>(joan::orig::as_date(obj(d::g_date))->julian(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Calendar>(this)->year)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Calendar>(this)->month)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Calendar>(this)->day_of_month))));  // 0x0057f990
        jo::self<jo::Calendar>(this)->start_year = jo::view<u32>(jo::self<jo::Calendar>(this)->year);
        jo::view<u32>(jo::self<jo::Calendar>(this)->day) = jo::self<jo::Calendar>(this)->start_julian;
        jo::view<u32>(jo::self<jo::Calendar>(this)->day_of_year) = joan::orig::arg::as<u32>(joan::orig::as_date(obj(d::g_date))->day_year(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Calendar>(this)->year)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Calendar>(this)->month)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Calendar>(this)->day_of_month))));  // 0x0057fef0
        jo::view<u32>(jo::self<jo::Calendar>(this)->time_of_day) = 0;
        jo::view<u32>(jo::self<jo::Calendar>(this)->minutes_per_day) = 0x44b40000u;  // 1440.0f
        jo::view<u32>(jo::self<jo::Calendar>(this)->time_prev) = jo::view<u32>(jo::self<jo::Calendar>(this)->time_of_day);
        jo::self<jo::Calendar>(this)->start_time = joan::orig::arg::as<u32>(joan::orig::libc::time(joan::orig::arg::mptr(static_cast<void*>(nullptr))));  // time(NULL)
        jo::self<jo::Calendar>(this)->u32_40 = 0;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Obj::zero_50", 0x00495230, &H::zero_50);
B95("Obj::open_495290", 0x00495290, &H::open_495290);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

using joan::call::thiscall;

struct H {
    u32 call_4953b0() { joan::orig::as_calendar(this)->clear(); return 0; }                   // 0x004953b0 (void; result unused: static dtor)
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("X::call_4953b0", 0x00495260, &H::call_4953b0);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

using joan::hook::global;

using joan::call::thiscall;

using joan::orig::helpers::at;

JOAN_COUNTER(c_release_321, "Obj::release_321");

struct H {
    void nop_4953b0() {}

    // ------------------------------------------------------------------------ 0x004956f0
    void release_321() {
        c_release_321.hit();
        const u32 elapsed = global<u32>(d::g_now) - jo::as_actor(jo::self<jo::Unit>(this)->model_b)->start_time;
        void* const owner = jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->model_b)->anim);
        void* const anim = joan::orig::arg::as<void*>(joan::orig::as_anim_set(owner)->frame(joan::orig::arg::i32(jo::as_actor(jo::self<jo::Unit>(this)->model_b)->action), joan::orig::arg::u32(i32{0})));  // 0x005b5dd0
        const double span = joan::orig::arg::as<double>(joan::orig::as_range(anim)->span());  // 0x0042d130 (raw st0)
        if (!(static_cast<double>(elapsed) >= span)) return;  // fcompp, test ah,1 jne
        if (jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->model_b)->scene) != nullptr) {
            joan::orig::as_scene(jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->model_b)->scene))->detach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->model_b));  // 0x005efdb0
        }
        void* const p = jo::self<jo::Unit>(this)->model_b;
        if (p != nullptr) joan::orig::as_scene_object(p)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00420840 deleting dtor
        jo::self<jo::Unit>(this)->model_b = nullptr;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::nop_4953b0", 0x004953b0, &H::nop_4953b0);
B94("Obj::release_321", 0x004956f0, &H::release_321);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using joan::call::thiscall;

using joan::orig::helpers::at;
using joan::orig::helpers::add32;

void* const kTypes = obj(d::g_nations);

JOAN_COUNTER(c_next_day, "Calendar::next_day_4953d0");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x004953d0
    void next_day_4953d0() {
        c_next_day.hit();
        void* const cal = obj(d::g_date);
        jo::self<jo::Calendar>(this)->day_of_month = add32(jo::self<jo::Calendar>(this)->day_of_month, 1);
        if (jo::self<jo::Calendar>(this)->day_of_month > 0x1e) jo::self<jo::Calendar>(this)->day_of_month = 0x1e;
        jo::self<jo::Calendar>(this)->day = add32(jo::self<jo::Calendar>(this)->day, 1);
        jo::self<jo::Calendar>(this)->weekday = jo::self<jo::Calendar>(this)->day % 7;
        jo::self<jo::Calendar>(this)->cycle_day30 = add32(jo::self<jo::Calendar>(this)->cycle_day30, 1) % 0x1e;
        if (joan::orig::arg::as<i32>(joan::orig::as_date_info(cal)->month_495810(joan::orig::arg::i32(jo::self<jo::Calendar>(this)->day))) != jo::self<jo::Calendar>(this)->month) {  // 0x00495810
            jo::self<jo::Calendar>(this)->day_of_month = 1;
            jo::self<jo::Calendar>(this)->month = joan::orig::arg::as<i32>(joan::orig::as_date_info(cal)->month_495810(joan::orig::arg::i32(jo::self<jo::Calendar>(this)->day)));
            jo::self<jo::Calendar>(this)->cycle_month30 = add32(jo::self<jo::Calendar>(this)->cycle_month30, 1) % 0x1e;
            jo::self<jo::Calendar>(this)->cycle_month6 = add32(jo::self<jo::Calendar>(this)->cycle_month6, 1) % 6;
            jo::self<jo::Calendar>(this)->cycle_month12 = add32(jo::self<jo::Calendar>(this)->cycle_month12, 1) % 0xc;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_date_info(cal)->year_4957e0(joan::orig::arg::i32(jo::self<jo::Calendar>(this)->day))) != jo::self<jo::Calendar>(this)->year) {  // 0x004957e0
            jo::self<jo::Calendar>(this)->month = 1;
            jo::self<jo::Calendar>(this)->year = joan::orig::arg::as<i32>(joan::orig::as_date_info(cal)->year_4957e0(joan::orig::arg::i32(jo::self<jo::Calendar>(this)->day)));
            jo::self<jo::Calendar>(this)->years = add32(jo::self<jo::Calendar>(this)->years, 1);
            jo::self<jo::Calendar>(this)->cycle_year30 = add32(jo::self<jo::Calendar>(this)->cycle_year30, 1) % 0x1e;
            jo::self<jo::Calendar>(this)->cycle_year10 = add32(jo::self<jo::Calendar>(this)->cycle_year10, 1) % 0xa;
        }
        jo::self<jo::Calendar>(this)->day_of_year = joan::orig::arg::as<i32>(joan::orig::as_date(cal)->day_year(joan::orig::arg::i32(jo::self<jo::Calendar>(this)->year), joan::orig::arg::i32(jo::self<jo::Calendar>(this)->month), joan::orig::arg::i32(jo::self<jo::Calendar>(this)->day_of_month)));  // 0x0057fef0
        joan::orig::as_units(kTypes)->refresh_4b68d0();            // 0x004b68d0
        joan::orig::as_light_sys(obj(d::g_light_sys))->roll_timers();   // 0x0045b160
        joan::orig::as_talk(obj(d::g_queue))->expire();   // 0x004e42e0
        if (jo::self<jo::Calendar>(this)->day_of_month != 1) return;
        joan::orig::as_list(kTypes)->process_unflagged_4b6960();  // 0x004b6960
        if (jo::self<jo::Calendar>(this)->month != 1) return;
        joan::orig::as_list(kTypes)->update_4b69d0();  // 0x004b69d0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Calendar::next_day_4953d0", 0x004953d0, &H::next_day_4953d0);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

struct H {
    // ------------------------------------------------------------------ 0x00495620
    void nop_495620() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::nop_495620", 0x00495620, &H::nop_495620);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::call::thiscall;

using joan::call::thiscall;

struct H {
    // 0x004957e0 / 0x00495810
    std::int32_t forward_59(std::int32_t a) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_date(this)->get_date(joan::orig::arg::i32(a), static_cast<char>(joan::orig::arg::i32(0x59)))); }  // 0x0057fa80
    std::int32_t forward_4d(std::int32_t a) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_date(this)->get_date(joan::orig::arg::i32(a), static_cast<char>(joan::orig::arg::i32(0x4d)))); }  // 0x0057fa80
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("forward_57fa80_59", 0x004957e0, &H::forward_59);
B("forward_57fa80_4d", 0x00495810, &H::forward_4d);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

using joan::call::thiscall;

struct H {
    // 0x00495840
    std::uint32_t alloc44(std::uint32_t n) { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_date(this)->get_date(joan::orig::arg::i32(n), static_cast<char>(joan::orig::arg::i32(0x44)))); }  // 0x0057fa80
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Pool::alloc44", 0x00495840, &H::alloc44);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using i16 = std::int16_t;

struct H {
    // ------------------------------------------------------------------------ 0x004958f0
    u8* entry_33(u32 idx) {
        return reinterpret_cast<u8*>(jo::self<jo::SdateRes>(this)->dates + (static_cast<i32>(static_cast<i16>(idx)) - 1));
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Table51::entry", 0x004958f0, &H::entry_33);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::hook::global;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x004956a0
    float wrap_delta(float x) {
        float t = jo::self<jo::Calendar>(this)->time_prev - x;
        if (!(t >= global<float>(d::k_9cf5c4))) {
            float w = jo::self<jo::Calendar>(this)->minutes_per_day - jo::self<jo::Calendar>(this)->time_of_day;
            w = w + t;
            t = w;
        }
        return t;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Track::wrap_delta", 0x004956a0, &H::wrap_delta);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Calendar::start(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_02::H::open_495290>(this, a0, a1, a2)); }  // 0x495290
auto joan::orig::Calendar::clear() -> void { joan::bind::member<&::game_b94_02::H::nop_4953b0>(this); }  // 0x4953b0
auto joan::orig::Calendar::next_day() -> void { joan::bind::member<&::game_b108_08::H::next_day_4953d0>(this); }  // 0x4953d0
auto joan::orig::Track::wrap_delta(float a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b100_04::H::wrap_delta>(this, a0)); }  // 0x4956a0
auto joan::orig::Unit::release_anim_321() -> void { joan::bind::member<&::game_b94_02::H::release_321>(this); }  // 0x4956f0
auto joan::orig::DateInfo::year_4957e0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::forward_59>(this, a0)); }  // 0x4957e0
auto joan::orig::DateInfo::month_495810(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::forward_4d>(this, a0)); }  // 0x495810
auto joan::orig::Pool::alloc44(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_02::H::alloc44>(this, a0)); }  // 0x495840
auto joan::orig::SDateRes::entry(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_03::H::entry_33>(this, a0)); }  // 0x4958f0
