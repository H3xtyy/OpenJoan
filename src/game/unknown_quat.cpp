// game/unknown_quat.cpp: functions whose original source file is unknown (formerly game/_unplaced/quat.cpp;
// re/attribution.csv: between ww/enphysic/physmath.cpp and ww/enphysic/physys.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

using joan::call::import_call;

using i16 = std::int16_t;

float k_zero() { return global<float>(d::k_9cf5c4); }   // 0.0f
float k_one() { return global<float>(d::flt_1); }    // 1.0f
float sqrt_f(float x) { return joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); }  // 0x004256d0 (stored)

JOAN_COUNTER(c_solve, "Quadratic::solve");
JOAN_COUNTER(c_quat, "Quat::from_rotvec");

struct H {
    // ------------------------------------------------------------------ 0x006acb40
    i16 solve(float* r1, float* r2, float a, float b, float c) {
        c_solve.hit();
        if (a == 0.0f || a != a) return 0;  // test ah,0x40 jne: equal or unordered
        const float bb = b * b;
        float ac = global<float>(d::flt_4) * a;  // 4.0f
        ac = ac * c;
        const float disc = bb - ac;
        const float inv = k_one() / (global<float>(d::flt_2) * a);  // 1 / (2a)
        if (disc == k_zero() || disc != disc) {
            *r2 = -b * inv;
            *reinterpret_cast<u32*>(r1) = *reinterpret_cast<u32*>(r2);
            return 1;
        }
        if (!(disc > k_zero())) return 0;  // test ah,0x41 jne
        const float s = sqrt_f(disc);
        *r1 = (-b + s) * inv;
        *r2 = (-b - s) * inv;
        return 2;
    }
    // ------------------------------------------------------------------ 0x006acc40
    void* quat_from_rotvec(void* out, const float* v) {
        c_quat.hit();
        float d = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<const void*>(v), static_cast<const void*>(v)));  // dot
        if (k_zero() == d || d != d) return out;  // test ah,0x40 jne: equal or unordered
        d = sqrt_f(d);
        const float s = k_one() / d;
        u32 tmp[3];
        const u32* const scaled = joan::orig::arg::as<const u32*>(joan::orig::vec_scale(static_cast<void*>(tmp), joan::orig::arg::ptr(v), joan::orig::arg::f32(s)));  // 0x0060cd00
        u32 axis[3];
        axis[0] = scaled[0];
        axis[1] = scaled[1];
        axis[2] = scaled[2];
        joan::orig::quat_from_axis_angle(joan::orig::arg::mptr(out), joan::orig::arg::mptr(static_cast<void*>(axis)), joan::orig::arg::f32(d));  // 0x0060c7a0
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("Quadratic::solve", 0x006acb40, &H::solve);
B("Quat::from_rotvec", 0x006acc40, &H::quat_from_rotvec);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("init_6acd80", 0x006acd80, &static_init<fn::Globals_init_pi>);
B("init_6acde0", 0x006acde0, &static_init<fn::init_a6a514>);
B("init_6ace30", 0x006ace30, &static_init<fn::init_a6a518>);
B("init_6ace80", 0x006ace80, &static_init<fn::init_a6a51c>);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

void JOAN_CDECL init_pi() {  // 0x006acda0
    const Fpu fpu{true};
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a6a510) = static_cast<float>(fpu.mul(t, global<double>(d::dbl_4)));  // 4.0
}

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("Globals::init_pi", 0x006acda0, &init_pi);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

static void JOAN_CDECL init_a6a514() { global<float>(d::g_a6a514) = global<float>(d::g_a6a510) / global<float>(d::flt_2); }  // 0x006ace00

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("init_a6a514", 0x006ace00, &init_a6a514);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

// 0x006ace50
void JOAN_CDECL init_a6a518() { global<float>(d::g_a6a518) = global<float>(d::g_a6a510) / global<float>(d::flt_3); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("init_a6a518", 0x006ace50, &init_a6a518);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

static void JOAN_CDECL init_a6a51c() { global<float>(d::g_a6a51c) = global<float>(d::g_a6a510) / global<float>(d::flt_4); }  // 0x006acea0

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("init_a6a51c", 0x006acea0, &init_a6a51c);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
