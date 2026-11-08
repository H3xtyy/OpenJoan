// game/unknown_angle8.cpp: functions whose original source file is unknown (formerly game/_unplaced/angle8.cpp;
// re/attribution.csv: between ww/ilib/angle.cpp and ww/ilib/angle.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
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
    // 0x00576120
    H* angle_ctor() { return this; }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b85_04", name, addr, fn)
GAME("Angle::Angle", 0x00576120, &H::angle_ctor);
#undef GAME

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

struct H {
    // 0x00576140
    H* byte_ctor(std::int8_t v) {
        jo::view<std::int8_t>(jo::self<jo::ByteVal>(this)->value) = v;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("ByteVal::ByteVal", 0x00576140, &H::byte_ctor);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b103_07.cpp (game/b103_07)
namespace {
namespace game_b103_07 {

struct H {
    // ------------------------------------------------------------ ByteVal
    // 0x00576170
    void* bv_set(i32 v) {
        jo::self<jo::ByteVal>(this)->value = static_cast<u8>(v);
        return this;
    }
    // 0x005761a0
    u32 bv_get() { return jo::self<jo::ByteVal>(this)->value; }
    // 0x005761d0
    i32 bv_set_fraction(float v) {
        const i32 r = joan::hook::x87::ftol(static_cast<double>(v * global<float>(d::flt_256)));  // 256
        jo::self<jo::ByteVal>(this)->value = static_cast<u8>(r);
        return r;
    }
    // 0x00576210 (fild byte * float 1/256: exact)
    float bv_fraction() {
        const joan::hook::x87::Fpu fpu{true};
        return static_cast<float>(
            fpu.mul(static_cast<double>(static_cast<i32>(jo::self<jo::ByteVal>(this)->value)), static_cast<double>(global<float>(d::flt_9d8638))));
    }
    // 0x00576340
    u32 bv_set_shl5(u32 v) {
        const u32 r = (v & 0xffu) << 5;
        jo::self<jo::ByteVal>(this)->value = static_cast<u8>(r);
        return r;
    }
    // 0x005763a0
    u8* bv_neg(u8* out) {
        u8 tmp[4];
        const u8* const r =
            joan::orig::arg::as<u8*>(joan::orig::as_byte_angle(static_cast<void*>(tmp))->construct(static_cast<std::int8_t>(joan::orig::arg::i32(-static_cast<i32>(jo::self<jo::ByteVal>(this)->value)))));  // 0x00576140
        *out = *r;
        return out;
    }
    // 0x005763e0
    u8* bv_add(u8* out, const u8* other) {
        u8 tmp[4];
        const u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_byte_angle(static_cast<void*>(tmp))->construct(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i32>(jo::self<jo::ByteVal>(this)->value) + static_cast<i32>(*other)))));  // 0x00576140
        *out = *r;
        return out;
    }
    // 0x005764e0 .. 0x005765e0
    u32 bv_eq(const u8* o) { return static_cast<i32>(jo::self<jo::ByteVal>(this)->value) == static_cast<i32>(*o) ? 1u : 0u; }
    u32 bv_ne(const u8* o) { return static_cast<i32>(jo::self<jo::ByteVal>(this)->value) != static_cast<i32>(*o) ? 1u : 0u; }
    u32 bv_gt(const u8* o) { return static_cast<i32>(jo::self<jo::ByteVal>(this)->value) > static_cast<i32>(*o) ? 1u : 0u; }
    u32 bv_lt(const u8* o) { return static_cast<i32>(jo::self<jo::ByteVal>(this)->value) < static_cast<i32>(*o) ? 1u : 0u; }
    u32 bv_ge(const u8* o) { return static_cast<i32>(jo::self<jo::ByteVal>(this)->value) >= static_cast<i32>(*o) ? 1u : 0u; }
    // 0x00576660
    u8* bv_fold(u8* out) {
        if (static_cast<i32>(jo::self<jo::ByteVal>(this)->value) <= 0x80) {
            *out = jo::self<jo::ByteVal>(this)->value;
            return out;
        }
        u8 tmp[4];
        const u8* const r =
            joan::orig::arg::as<u8*>(joan::orig::as_byte_angle(static_cast<void*>(tmp))->construct(static_cast<std::int8_t>(joan::orig::arg::i32(-static_cast<i32>(jo::self<jo::ByteVal>(this)->value)))));  // 0x00576140
        *out = *r;
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("ByteVal::set", 0x00576170, &H::bv_set);
B("ByteVal::get", 0x005761a0, &H::bv_get);
B("ByteVal::set_fraction", 0x005761d0, &H::bv_set_fraction);
B("ByteVal::fraction", 0x00576210, &H::bv_fraction);
B("ByteVal::set_shl5", 0x00576340, &H::bv_set_shl5);
B("ByteVal::neg", 0x005763a0, &H::bv_neg);
B("ByteVal::add", 0x005763e0, &H::bv_add);
B("ByteVal::eq", 0x005764e0, &H::bv_eq);
B("ByteVal::ne", 0x00576520, &H::bv_ne);
B("ByteVal::gt", 0x00576560, &H::bv_gt);
B("ByteVal::lt", 0x005765a0, &H::bv_lt);
B("ByteVal::ge", 0x005765e0, &H::bv_ge);
B("ByteVal::fold", 0x00576660, &H::bv_fold);
#undef B

}  // namespace game_b103_07
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

struct H {
    void* angle_from_atan2(float a, float b) {  // 0x00576da0
        // fpatan st0 (64-bit) is stored straight to a float: take it as float (one rounding).
        const float r = joan::orig::arg::as<float>(joan::orig::libc::atan2(joan::orig::arg::f64(static_cast<double>(b)), joan::orig::arg::f64(static_cast<double>(a))));
        joan::orig::as_angle(this)->set(joan::orig::arg::f32(r));  // 0x00576240
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Angle::from_atan2", 0x00576da0, &H::angle_from_atan2);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
namespace {
namespace game_b97_05 {

struct H {
    // 0x00576370
    std::int32_t octant() { return static_cast<std::int32_t>((jo::self<jo::ByteAngle>(this)->value + 0xfu) & 0xffu) >> 5; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("ByteAngle::octant", 0x00576370, &H::octant);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

struct H {
    // 0x00576430
    std::uint8_t* angle_diff(std::uint8_t* out, const std::uint8_t* other) {
        std::uint32_t tmp = 0;
        const std::int32_t d = static_cast<std::int32_t>(jo::self<jo::ByteAngle>(this)->value) - static_cast<std::int32_t>(*other);
        const std::uint8_t* const r = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_byte_angle(&tmp)->construct(static_cast<std::int8_t>(joan::orig::arg::i32(d))));  // 0x00576140
        *out = *r;
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("ByteAngle::diff", 0x00576430, &H::angle_diff);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

struct H {
    // 0x00576480
    void* angle_add(const std::uint8_t* other) {
        jo::self<jo::ByteAngle>(this)->value = static_cast<std::uint8_t>(jo::self<jo::ByteAngle>(this)->value + *other);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("ByteAngle::add", 0x00576480, &H::angle_add);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

struct H {
    // 0x005764b0
    H* angle_sub(const std::uint8_t* other) {
        jo::self<jo::ByteAngle>(this)->value = static_cast<std::uint8_t>(jo::self<jo::ByteAngle>(this)->value - *other);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("ByteAngle::sub", 0x005764b0, &H::angle_sub);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b99_06.cpp (game/b99_06)
namespace {
namespace game_b99_06 {

struct H {
    // 0x00576620
    bool le(const u8* other) { return static_cast<i32>(jo::self<jo::ByteAngle>(this)->value) <= static_cast<i32>(*other); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_06", name, addr, fn)
B("ByteAngle::le", 0x00576620, &H::le);
#undef B

}  // namespace game_b99_06
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
namespace {
namespace game_b98_05 {

JOAN_COUNTER(c_angle_distance, "ByteAngle::distance");

struct H {
    // 0x005766d0
    std::uint8_t* angle_distance(std::uint8_t* out, const std::uint8_t* other) {
        c_angle_distance.hit();
        std::uint32_t tmp = 0;  // [ebp-8], fully written below
        joan::orig::as_angle(&tmp)->construct();  // 0x00576120 (returns this)
        const std::int32_t a = jo::self<jo::ByteAngle>(this)->value;
        const std::int32_t b = *other;
        std::uint8_t d;
        if (!(a < b)) {
            d = static_cast<std::uint8_t>(a - b);
        } else {
            d = static_cast<std::uint8_t>(b - a);
        }
        std::uint8_t r;
        if (static_cast<std::int32_t>(d) > 0x80) {
            r = static_cast<std::uint8_t>(-static_cast<std::int32_t>(d));
        } else {
            r = d;
        }
        *out = r;
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("ByteAngle::distance", 0x005766d0, &H::angle_distance);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b102_06.cpp (game/b102_06)
namespace {
namespace game_b102_06 {

JOAN_COUNTER(c_step_towards, "ByteAngle::step_towards");

struct H {
    // 0x005767b0
    bool step_towards(u32 target, u32 step) {
        c_step_towards.hit();
        u32 diff = 0;  // 1-byte ByteAngle written by 0x005766d0
        void* const d = joan::orig::arg::as<void*>(joan::orig::as_byte_angle(this)->distance(&diff, &target));  // 0x005766d0
        if (joan::orig::arg::as<u8>(joan::orig::as_byte_angle(d)->le(&step)) != 0) {  // 0x00576620
            jo::self<jo::ByteAngle>(this)->value = static_cast<u8>(target);
            return true;
        }
        const i32 cur = jo::self<jo::ByteAngle>(this)->value;
        const i32 t = static_cast<i32>(target & 0xffu);
        if (cur >= t) {
            const u8 delta = static_cast<u8>(cur - t);
            if (delta > 0x80)
                joan::orig::as_byte_angle(this)->add(&step);  // 0x00576480
            else
                joan::orig::as_byte_angle(this)->sub(&step);  // 0x005764b0
        } else {
            const u8 delta = static_cast<u8>(t - cur);
            if (delta > 0x80)
                joan::orig::as_byte_angle(this)->sub(&step);  // 0x005764b0
            else
                joan::orig::as_byte_angle(this)->add(&step);  // 0x00576480
        }
        return false;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_06", name, addr, fn)
B("ByteAngle::step_towards", 0x005767b0, &H::step_towards);
#undef B

}  // namespace game_b102_06
}  // namespace

// ---------------------------------------- from modules/game_b102_07.cpp (game/b102_07)
namespace {
namespace game_b102_07 {

using joan::img::obj;

const void* cptr(img::Addr address) { return img::obj(address); }
u32 with_al(u32 eax, u8 al) { return (eax & 0xffffff00u) | al; }

using joan::orig::helpers::crt_abs;

JOAN_COUNTER(c_nearest, "SinTable::nearest");
JOAN_COUNTER(c_dir8, "dir8_from_delta");
JOAN_COUNTER(c_dir16, "dir16_from_delta");

// ------------------------------------------------------------------ 0x00576ed0
u32 JOAN_CDECL dir8_from_delta(i32 x0, i32 y0, i32 x1, i32 y1) {
    c_dir8.hit();
    const u32 ax = static_cast<u32>(crt_abs(static_cast<i32>(static_cast<u32>(x1) - static_cast<u32>(x0))));
    const u32 ay = static_cast<u32>(crt_abs(static_cast<i32>(static_cast<u32>(y1) - static_cast<u32>(y0))));
    const u32 m = ax > ay ? ax : ay;
    const u32 ax2 = ax << 1, ay2 = ay << 1;
    if (x1 == x0) return with_al(static_cast<u32>(x1), y1 > y0 ? 4 : 0);
    if (x1 < x0) {
        if (y1 > y0) {
            const u32 e = static_cast<u32>(y1);
            if (!(ax2 > m)) return with_al(e, 4);
            if (!(ay2 > m)) return with_al(e, 6);
            return with_al(e, 5);
        }
        if (!(ax2 > m)) return with_al(ax2, 0);
        if (!(ay2 > m)) return with_al(ax2, 6);
        return with_al(ax2, 7);
    }
    if (y1 > y0) {
        if (!(ax2 > m)) return with_al(ax2, 4);
        if (!(ay2 > m)) return with_al(ax2, 2);
        return with_al(ax2, 3);
    }
    if (!(ax2 > m)) return with_al(static_cast<u32>(x1), 0);
    if (!(ay2 > m)) return with_al(ay2, 2);
    return with_al(ay2, 1);
}

// ------------------------------------------------------------------ 0x00577020
u32 JOAN_CDECL dir16_from_delta(i32 x0, i32 y0, i32 x1, i32 y1) {
    c_dir16.hit();
    const i32 dx = static_cast<i32>(static_cast<u32>(x1) - static_cast<u32>(x0));
    const i32 adx = crt_abs(dx);
    const i32 dy = static_cast<i32>(static_cast<u32>(y1) - static_cast<u32>(y0));
    const i32 ady = crt_abs(dy);
    auto mul = [](i32 v, i32 k) { return static_cast<i32>(static_cast<u32>(v) * static_cast<u32>(k)); };
    if (!(mul(ady, 2) < mul(adx, 3))) {
        if (!(dy > 0)) {
            const i32 e = mul(dx, -5);
            if (!(e > dy)) return with_al(static_cast<u32>(e), 0x8);
            if (!(mul(dx, 5) > dy)) return with_al(static_cast<u32>(e), 0xf);
            return with_al(static_cast<u32>(e), 0);
        }
        if (!(mul(dx, 5) < dy)) return with_al(static_cast<u32>(ady), 0xb);
        const i32 e = mul(dx, -5);
        if (!(e < dy)) return with_al(static_cast<u32>(e), 0xc);
        return with_al(static_cast<u32>(e), 0x4);
    }
    if (!(mul(adx, 2) < mul(ady, 3))) {
        if (!(dx > 0)) {
            const i32 e = mul(dy, -5);
            if (!(e > dx)) return with_al(static_cast<u32>(e), 0xd);
            if (!(mul(dy, 5) > dx)) return with_al(static_cast<u32>(e), 0xe);
            return with_al(static_cast<u32>(e), 0x6);
        }
        if (!(mul(dy, 5) < dx)) return with_al(static_cast<u32>(ady), 0xa);
        const i32 e = mul(dy, -5);
        if (!(e < dx)) return with_al(static_cast<u32>(e), 0x9);
        return with_al(static_cast<u32>(e), 0x2);
    }
    // eax still holds abs(dy) from the second abs call here.
    if (dx > 0) return with_al(static_cast<u32>(ady), dy > 0 ? 0x3 : 0x1);
    return with_al(static_cast<u32>(ady), dy > 0 ? 0x5 : 0x7);
}

struct H {
    // -------------------------------------------------------------- 0x00576e00
    void* nearest(float x) {
        c_nearest.hit();
        if (global<i32>(d::g_angle_init_table_flag) == 0) joan::orig::angle::init_tables();  // 0x00576cb0
        const float* t = static_cast<const float*>(cptr(d::g_a641f0));
        u32 lo = 0, hi = 0x20;
        while (hi - lo > 1) {
            const u32 mid = (lo + hi) >> 1;
            if (x > t[mid])
                lo = mid;
            else
                hi = mid;
        }
        const float below = x - t[lo];
        const float above = t[hi] - x;
        jo::self<jo::SinTable>(this)->value = !(above >= below) ? static_cast<u8>(hi) : static_cast<u8>(lo);
        return this;
    }
};

#define B102(name, addr, fn) JOAN_REPLACE("game/b102_07", name, addr, fn)
B102("SinTable::nearest", 0x00576e00, &H::nearest);
B102("dir8_from_delta", 0x00576ed0, &dir8_from_delta);
B102("dir16_from_delta", 0x00577020, &dir16_from_delta);
#undef B102

}  // namespace game_b102_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Angle::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04::H::angle_ctor>(this)); }  // 0x576120
auto joan::orig::ByteAngle::construct(std::int8_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::byte_ctor>(this, a0)); }  // 0x576140
auto joan::orig::ByteAngle::octant() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_05::H::octant>(this)); }  // 0x576370
auto joan::orig::ByteAngle::diff(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_05::H::angle_diff>(this, a0, a1)); }  // 0x576430
auto joan::orig::ByteAngle::add(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_05::H::angle_add>(this, a0)); }  // 0x576480
auto joan::orig::ByteAngle::sub(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_05::H::angle_sub>(this, a0)); }  // 0x5764b0
auto joan::orig::ByteAngle::le(void* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b99_06::H::le>(this, a0)); }  // 0x576620
auto joan::orig::ByteAngle::distance(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_05::H::angle_distance>(this, a0, a1)); }  // 0x5766d0
auto joan::orig::Angle::from_atan2(float a0, float a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_03::H::angle_from_atan2>(this, a0, a1)); }  // 0x576da0
