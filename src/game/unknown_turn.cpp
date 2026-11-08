// game/unknown_turn.cpp: functions whose original source file is unknown (formerly game/_unplaced/turn.cpp;
// re/attribution.csv: between ww/ilib/angle.cpp and ww/ilib/audio.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
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

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::hook::global;

double modf_(double x, double* ip) { return joan::orig::arg::as<double>(joan::orig::libc::modf(joan::orig::arg::f64(x), joan::orig::arg::mptr(ip))); }

}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

const float k360f = 360.0f;  // 0x009cf9c8
const float k0f = 0.0f;      // 0x009cf5c4
const float k1f = 1.0f;      // 0x009cf784

JOAN_COUNTER(c_set_deg, "Angle::set_degrees");

struct H {
    // 0x005773e0 - fld float, fdiv qword 360.0 (PC24), modf, fstp float; += 1.0f when < 0 / NaN.
    void set_degrees(float deg) {
        c_set_deg.hit();
        const joan::hook::x87::Fpu fpu{true};
        double whole = 0.0;
        const double q = fpu.div(static_cast<double>(deg), 360.0);
        jo::self<jo::Angle>(this)->value = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::modf(joan::orig::arg::f64(q), &whole)));  // modf
        if (!(jo::self<jo::Angle>(this)->value >= k0f)) jo::self<jo::Angle>(this)->value = jo::self<jo::Angle>(this)->value + k1f;
    }

    // 0x00577450
    float degrees() { return jo::self<jo::Angle>(this)->value * k360f; }

    // 0x00577190
    H* self() { return this; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Angle::set_degrees", 0x005773e0, &H::set_degrees);
M("Angle::degrees", 0x00577450, &H::degrees);
M("Obj::self", 0x00577190, &H::self);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

struct H {
    // 0x005771b0
    void* turn_ctor(float x) {
        double whole;
        jo::self<jo::Turn>(this)->value = joan::orig::arg::as<float>(joan::orig::libc::modf(joan::orig::arg::f64(static_cast<double>(x)), &whole));  // modf
        if (!(jo::self<jo::Turn>(this)->value >= 0.0f)) jo::self<jo::Turn>(this)->value = jo::self<jo::Turn>(this)->value + 1.0f;
        return this;
    }

    // 0x005775b0
    std::uint32_t* turn_sub(std::uint32_t* out, const float* b) {
        std::uint32_t tmp[2];
        const std::uint32_t* const r =
            joan::orig::arg::as<std::uint32_t*>(joan::orig::as_turn(static_cast<void*>(tmp))->construct(joan::orig::arg::f32(jo::self<jo::Turn>(this)->value - *b)));  // 0x005771b0
        *out = *r;
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Turn::Turn", 0x005771b0, &H::turn_ctor);
B("Turn::sub", 0x005775b0, &H::turn_sub);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b103_07.cpp (game/b103_07)
namespace {
namespace game_b103_07 {

using i32 = std::int32_t;

using i8 = std::int8_t;

struct H {
    // ------------------------------------------------------------ Turn
    // 0x00577220 / 0x005772b0
    void turn_set(float v) {
        double whole;
        jo::self<jo::Turn>(this)->value = joan::orig::arg::as<float>(joan::orig::libc::modf(joan::orig::arg::f64(static_cast<double>(v)), &whole));  // modf
        // fcomp 0.0 / test ah,1 / je: add 1 when < 0 or unordered.
        if (!(jo::self<jo::Turn>(this)->value >= global<float>(d::k_9cf5c4))) jo::self<jo::Turn>(this)->value = jo::self<jo::Turn>(this)->value + global<float>(d::flt_1);
    }
    // 0x00577480 (fild of a small int * 0.125f: exact)
    i32 turn_set_eighths(i32 v) {
        const i32 s = static_cast<i8>(v);
        const joan::hook::x87::Fpu fpu{true};
        jo::self<jo::Turn>(this)->value =
            static_cast<float>(fpu.mul(static_cast<double>(s), static_cast<double>(global<float>(d::flt_0_125))));
        return s;
    }
    // 0x005774c0
    i32 turn_octant() {
        const float x = jo::self<jo::Turn>(this)->value * global<float>(d::flt_8) + global<float>(d::flt_0_5);  // * 8 + 0.5
        const double fl = joan::orig::arg::as<double>(joan::orig::libc::floor(joan::orig::arg::f64(static_cast<double>(x))));            // floor
        const i32 c = static_cast<i8>(joan::hook::x87::ftol(fl));
        return c % 8;  // and 0x80000007 / jns / dec / or -8 / inc = signed remainder
    }
    // 0x00577520
    u32* turn_neg(u32* out) {
        u32 tmp[2];
        const u32* const r = joan::orig::arg::as<u32*>(joan::orig::as_turn(static_cast<void*>(tmp))->construct(joan::orig::arg::f32(-jo::self<jo::Turn>(this)->value)));  // 0x005771b0
        *out = *r;
        return out;
    }
    // 0x005776d0: test ah,0x40 je -> 1 when equal or unordered.
    bool turn_eq(const float* o) { return !(jo::self<jo::Turn>(this)->value < *o || jo::self<jo::Turn>(this)->value > *o); }
    // 0x00577720: test ah,0x40 jne -> 1 when ordered and not equal.
    bool turn_ne(const float* o) { return jo::self<jo::Turn>(this)->value < *o || jo::self<jo::Turn>(this)->value > *o; }
    // 0x005777c0: test ah,1 je -> 1 when less or unordered.
    bool turn_lt(const float* o) { return !(jo::self<jo::Turn>(this)->value >= *o); }
    // 0x00577810: test ah,1 jne -> 1 when greater-or-equal (ordered).
    bool turn_ge(const float* o) { return jo::self<jo::Turn>(this)->value >= *o; }
    // 0x005778b0: test ah,0x41 je -> fold when v > 0.5.
    u32* turn_fold(u32* out) {
        if (!(jo::self<jo::Turn>(this)->value > global<float>(d::flt_0_5))) {
            *out = jo::view<u32>(jo::self<jo::Turn>(this)->value);
            return out;
        }
        u32 tmp[2];
        const u32* const r = joan::orig::arg::as<u32*>(joan::orig::as_turn(static_cast<void*>(tmp))->construct(joan::orig::arg::f32(global<float>(d::flt_1) - jo::self<jo::Turn>(this)->value)));  // 0x005771b0
        *out = *r;
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Turn::set_577220", 0x00577220, &H::turn_set);
B("Turn::set_5772b0", 0x005772b0, &H::turn_set);
B("Turn::set_eighths", 0x00577480, &H::turn_set_eighths);
B("Turn::octant", 0x005774c0, &H::turn_octant);
B("Turn::neg", 0x00577520, &H::turn_neg);
B("Turn::eq", 0x005776d0, &H::turn_eq);
B("Turn::ne", 0x00577720, &H::turn_ne);
B("Turn::lt", 0x005777c0, &H::turn_lt);
B("Turn::ge", 0x00577810, &H::turn_ge);
B("Turn::fold", 0x005778b0, &H::turn_fold);
#undef B

}  // namespace game_b103_07
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15 {

struct H {
    // 0x00577290 / 0x00577320
    float get_float() { return jo::self<jo::Turn>(this)->value; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Obj::get_float_577290", 0x00577290, &H::get_float);
B("Obj::get_float_577320", 0x00577320, &H::get_float);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

struct H {
    // 0x00577340
    void set_wrapped(float a) {
        const joan::hook::x87::Fpu fpu{true};
        double ip = 0.0;
        const double q = fpu.div(static_cast<double>(a), global<double>(d::dbl_6_283185307179586));  // 2pi
        jo::self<jo::Angle>(this)->value = static_cast<float>(modf_(q, &ip));
        // fcomp 0.0 / test ah,1 je: wrap when < 0 or unordered.
        if (!(jo::self<jo::Angle>(this)->value >= global<float>(d::k_9cf5c4))) jo::self<jo::Angle>(this)->value = jo::self<jo::Angle>(this)->value + global<float>(d::flt_1);
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Angle::set_wrapped", 0x00577340, &H::set_wrapped);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

struct H {
    // 0x005773b0
    float to_radians() { return jo::self<jo::Angle>(this)->value * global<float>(d::flt_6_2831855); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Angle::to_radians", 0x005773b0, &H::to_radians);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

struct H {
    // ------------------------------------------------------------------------ 0x00577560
    u32* angle_add(u32* out, const float* other) {
        u32 tmp;
        const float sum = jo::self<jo::Angle>(this)->value + *other;
        const u32* r = joan::orig::arg::as<const u32*>(joan::orig::as_turn(&tmp)->construct(joan::orig::arg::f32(sum)));  // 0x005771b0
        *out = *r;
        return out;
    }

    // ------------------------------------------------------------------------ 0x00577770
    bool angle_gt(const float* other) { return jo::self<jo::Angle>(this)->value > *other; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Angle::add", 0x00577560, &H::angle_add);
B("Angle::gt", 0x00577770, &H::angle_gt);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00577650
    void* sub_frac(const float* other) {
        double ip = 0.0;
        jo::self<jo::Angle>(this)->value = static_cast<float>(modf_(static_cast<double>(jo::self<jo::Angle>(this)->value - *other), &ip));
        // fcomp 0.0 / test ah,1 je: wrap when < 0 or unordered.
        if (!(jo::self<jo::Angle>(this)->value >= global<float>(d::k_9cf5c4))) jo::self<jo::Angle>(this)->value = jo::self<jo::Angle>(this)->value + global<float>(d::flt_1);
        return self();
    }

    // 0x00577600
    void* add_frac(const float* other) {
        double ip = 0.0;
        jo::self<jo::Angle>(this)->value = static_cast<float>(modf_(static_cast<double>(jo::self<jo::Angle>(this)->value + *other), &ip));
        return self();
    }

    // 0x00577860: fcomp / test ah,0x41 je -> 0 when a > b.
    bool not_greater(const float* other) { return !(jo::self<jo::Angle>(this)->value > *other); }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Angle::sub_frac", 0x00577650, &H::sub_frac);
M("Angle::add_frac", 0x00577600, &H::add_frac);
M("Angle::not_greater", 0x00577860, &H::not_greater);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b89_05.cpp (game/b89_05)
namespace {
namespace game_b89_05 {

struct H {
    // ------------------------------------------------------------ 0x00577920 (ret 8)
    std::uint32_t* angle_distance(std::uint32_t* out, const float* other) {
        const float self = jo::self<jo::Angle>(this)->value;
        std::uint32_t tmp = 0;
        float d;
        if (self >= *other) {  // fcomp; test ah,1 jne -> less or unordered takes the second branch
            d = jo::self<jo::Angle>(this)->value - *other;
        } else {
            d = *other - jo::self<jo::Angle>(this)->value;
        }
        if (d > 0.5f) {  // test ah,0x41 jne -> <= or unordered keeps d
            *out = *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_turn(&tmp)->construct(joan::orig::arg::f32(1.0f - d)));  // 0x005771b0
        } else {
            *out = *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_turn(&tmp)->construct(joan::orig::arg::f32(d)));  // 0x005771b0
        }
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b89_05", name, addr, fn)
B("Angle::distance", 0x00577920, &H::angle_distance);
#undef B

}  // namespace game_b89_05
}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
namespace {
namespace game_b88_05 {

using joan::orig::helpers::as_float;

JOAN_COUNTER(c_turn, "Angle::turn_toward");

struct H {
    // ------------------------------------------------------------ 0x00577a20 (ret 8)
    std::uint8_t turn_toward(std::uint32_t target, std::uint32_t step) {
        c_turn.hit();
        std::uint8_t diff[8] = {};  // ebp-0x8, written by 0x00577920
        void* const d = joan::orig::arg::as<void*>(joan::orig::as_angle(this)->distance(joan::orig::arg::mptr(diff), &target));  // 0x00577920
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_angle(d)->not_greater(&step)) != 0) {          // 0x00577860
            jo::view<std::uint32_t>(jo::self<jo::Angle>(this)->value) = target;
            return 1;
        }
        const float self = jo::self<jo::Angle>(this)->value;
        const float t = as_float(target);
        if (self >= t) {  // fcomp; test ah,1 jne -> (self < t or NaN) takes the else branch
            const float delta = self - t;
            if (delta > 0.5f) {  // test ah,0x41 jne -> <= or NaN
                joan::orig::as_angle(this)->add_frac(&step);  // 0x00577600
            } else {
                joan::orig::as_angle(this)->sub_frac(&step);  // 0x00577650
            }
        } else {
            const float delta = t - jo::self<jo::Angle>(this)->value;
            if (delta > 0.5f) {
                joan::orig::as_angle(this)->sub_frac(&step);  // 0x00577650
            } else {
                joan::orig::as_angle(this)->add_frac(&step);  // 0x00577600
            }
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("Angle::turn_toward", 0x00577a20, &H::turn_toward);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

struct H {
    // 0x00577b90
    void* from_xy(double x, double y) {
        const float a = joan::orig::arg::as<float>(joan::orig::libc::atan2(joan::orig::arg::f64(y), joan::orig::arg::f64(x)));  // atan2
        joan::orig::as_angle(this)->set_wrapped(joan::orig::arg::f32(a));  // 0x00577340
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Angle::from_xy", 0x00577b90, &H::from_xy);
#undef B

}  // namespace game_b90_02_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Angle::construct_577190() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b88_06::H::self>(this)); }  // 0x577190
auto joan::orig::Turn::construct(float a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02::H::turn_ctor>(this, a0)); }  // 0x5771b0
auto joan::orig::Angle::set_wrapped(float a0) -> void { joan::bind::member<&::game_b90_04::H::set_wrapped>(this, a0); }  // 0x577340
auto joan::orig::Angle::to_radians() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b94_05::H::to_radians>(this)); }  // 0x5773b0
auto joan::orig::Angle::set_degrees(float a0) -> void { joan::bind::member<&::game_b88_06::H::set_degrees>(this, a0); }  // 0x5773e0
auto joan::orig::Angle::degrees() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b88_06::H::degrees>(this)); }  // 0x577450
auto joan::orig::Angle::add(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10::H::angle_add>(this, a0, a1)); }  // 0x577560
auto joan::orig::Turn::sub(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02::H::turn_sub>(this, a0, a1)); }  // 0x5775b0
auto joan::orig::Angle::add_frac(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::add_frac>(this, a0)); }  // 0x577600
auto joan::orig::Angle::sub_frac(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::sub_frac>(this, a0)); }  // 0x577650
auto joan::orig::Angle::gt(void* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b100_10::H::angle_gt>(this, a0)); }  // 0x577770
auto joan::orig::Angle::not_greater(void* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b89_06::H::not_greater>(this, a0)); }  // 0x577860
auto joan::orig::Angle::distance(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_05::H::angle_distance>(this, a0, a1)); }  // 0x577920
auto joan::orig::Angle::turn_toward(std::uint32_t a0, std::uint32_t a1) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b88_05::H::turn_toward>(this, a0, a1)); }  // 0x577a20
auto joan::orig::Angle::from_xy(double a0, double a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02_2::H::from_xy>(this, a0, a1)); }  // 0x577b90
