// game/unknown_volpan.cpp: functions whose original source file is unknown (formerly game/_unplaced/volpan.cpp;
// re/attribution.csv: between ww/ilib/txtres.cpp and ww/ilib/vqueue.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cstddef>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

using joan::call::cdecl_call;

template <fn::Code Body>
u32 JOAN_CDECL crt_init() {
    return cdecl_call<u32>(Body);  // direct call in the original (no thunk)
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("crt_init_5a80c0", 0x005a80c0, &crt_init<fn::init_5a80e0>);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
namespace {
namespace game_b98_05 {

void JOAN_CDECL init_5a80e0() {
    joan::orig::as_vol_pan(obj(d::g_a67628))->construct_5a8280(0x64u, 0u);  // 0x005a8280
}

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("init_5a80e0", 0x005a80e0, &init_5a80e0);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

struct H {
    // 0x005a8110
    H* pair_set(std::uint32_t a, std::uint32_t b) {
        jo::view<std::uint32_t>(jo::self<jo::Force>(this)->volume) = a;
        jo::view<std::uint32_t>(jo::self<jo::Force>(this)->pan) = b;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("Pair::set", 0x005a8110, &H::pair_set);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

struct H {
    // ------------------------------------------------------------------ 0x005a8150..0x005a8600
    H* from_pct(const i32* src) {
        jo::view<u32>(jo::self<jo::Force>(this)->volume) = static_cast<u32>(src[0]) * 0x64u - 0x2710u;
        jo::self<jo::Force>(this)->pan = src[1];
        return this;
    }
    H* pair_ctor(u32 a, u32 b) {
        jo::view<u32>(jo::self<jo::Force>(this)->volume) = a;
        jo::view<u32>(jo::self<jo::Force>(this)->pan) = b;
        return this;
    }
    H* to_pct(const i32* src) {
        jo::self<jo::Force>(this)->volume = static_cast<i32>(static_cast<u32>(src[0]) + 0x2710u) / 0x64;
        jo::self<jo::Force>(this)->pan = src[1];
        return this;
    }
    H* pair_ctor_b(u32 a, u32 b) {
        jo::view<u32>(jo::self<jo::Force>(this)->volume) = a;
        jo::view<u32>(jo::self<jo::Force>(this)->pan) = b;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Pair::from_pct", 0x005a8150, &H::from_pct);
B("Pair::ctor", 0x005a81f0, &H::pair_ctor);
B("Pair::to_pct", 0x005a8230, &H::to_pct);
B("Pair::ctor_b", 0x005a8600, &H::pair_ctor_b);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

struct H {
    // 0x005a81a0
    void* volume_ctor(const std::int32_t* src) {
        jo::self<jo::Volume>(this)->attenuation = joan::orig::arg::as<std::int32_t>(joan::orig::as_sound(obj(d::g_audio))->attenuation(joan::orig::arg::i32(src[0])));  // 0x0057c960
        jo::self<jo::Volume>(this)->value = src[1];
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("Volume::ctor", 0x005a81a0, &H::volume_ctor);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

struct H {
    void* pair_set(u32 a, u32 b) {  // 0x005a8280
        jo::view<u32>(jo::self<jo::Force>(this)->volume) = a;
        jo::view<u32>(jo::self<jo::Force>(this)->pan) = b;
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Pair::set", 0x005a8280, &H::pair_set);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b102_09.cpp (game/b102_09)
namespace {
namespace game_b102_09 {

using joan::hook::global;

using joan::hook::x87::Fpu;

using joan::call::import_call;

JOAN_COUNTER(c_force_pair, "Force::from_pair");
JOAN_COUNTER(c_force_scaled, "Force::from_pair_scaled");
JOAN_COUNTER(c_force_point, "Force::from_point");

struct H {
    // ------------------------------------------------------------------------ 0x005a82c0
    static i32 iabs(i32 v) { return v >= 0 ? v : static_cast<i32>(0u - static_cast<u32>(v)); }
    i32 pair_max(const i32* p, i32* d_out) {
        const i32 d = static_cast<i32>(static_cast<u32>(p[0]) - static_cast<u32>(p[1]));
        const i32 s = static_cast<i32>(static_cast<u32>(p[0]) + static_cast<u32>(p[1]));
        const i32 a = iabs(d), b = iabs(s);
        *d_out = d;
        return a >= b ? a : b;
    }
    void set_dir(i32 d) {
        if (d >= 100)
            jo::self<jo::Force>(this)->pan = 10000;
        else if (d <= -100)
            jo::self<jo::Force>(this)->pan = -10000;
        else
            jo::self<jo::Force>(this)->pan = d * 100;
    }
    void* from_pair(const i32* p) {
        c_force_pair.hit();
        i32 d = 0;
        const i32 m = pair_max(p, &d);
        if (m <= 0x28)
            jo::self<jo::Force>(this)->volume = 100 - static_cast<i32>(static_cast<u32>(m) * 100u) / 100;
        else
            jo::self<jo::Force>(this)->volume = 0;
        set_dir(d);
        return this;
    }

    // ------------------------------------------------------------------------ 0x005a83e0
    void* from_pair_scaled(const i32* p, i32 div, i32 limit) {
        c_force_scaled.hit();
        i32 d = 0;
        const i32 m = pair_max(p, &d);
        if (m <= limit)
            jo::self<jo::Force>(this)->volume = 100 - static_cast<i32>(static_cast<u32>(m) * 100u) / div;
        else
            jo::self<jo::Force>(this)->volume = 0;
        set_dir(d);
        return this;
    }

    // ------------------------------------------------------------------------ 0x005a8500
    void* from_point(void* o, const void* v) {
        c_force_point.hit();
        void* const m = joan::orig::arg::as<void*>(joan::orig::as_camera(o)->matrix_ee());  // 0x00616ec0
        float out[3];
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(out), v, m);  // point_transform(out, in, m)
        const float ls = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(out)));  // length_sq
        const Fpu fpu{true};
        if (ls > global<float>(d::flt_640000)) {  // 640000
            jo::self<jo::Force>(this)->volume = 0;
        } else {
            const double k100 = global<float>(d::flt_100);  // 100
            const double t = fpu.div(fpu.mul(static_cast<double>(ls), k100),
                                     static_cast<double>(global<float>(d::flt_2560000)));  // 2560000
            jo::self<jo::Force>(this)->volume = joan::hook::x87::ftol(fpu.sub(k100, t));
        }
        const float x = out[0];
        if (!(x < global<float>(d::flt_400) || x != x)) {  // x >= 400
            jo::self<jo::Force>(this)->pan = 10000;
        } else if (!(x > global<float>(d::flt_m400))) {  // x <= -400 or NaN
            jo::self<jo::Force>(this)->pan = -10000;
        } else {
            jo::self<jo::Force>(this)->pan = joan::hook::x87::ftol(
                fpu.mul(static_cast<double>(global<float>(d::flt_25)), static_cast<double>(x)));  // 25
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("Force::from_pair", 0x005a82c0, &H::from_pair);
B("Force::from_pair_scaled", 0x005a83e0, &H::from_pair_scaled);
B("Force::from_point", 0x005a8500, &H::from_point);
#undef B

}  // namespace game_b102_09
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::VolPan::construct(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_05::H::pair_set>(this, a0, a1)); }  // 0x5a8110
auto joan::orig::Volume::ctor(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_05::H::volume_ctor>(this, a0)); }  // 0x5a81a0
auto joan::orig::VolPan::construct_5a8280(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_03::H::pair_set>(this, a0, a1)); }  // 0x5a8280
