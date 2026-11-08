// game/unknown_i_obj0d.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_obj0d.cpp;
// re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
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

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::hook::global;
using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

// ---- static initialisers (CRT init table) ----
void JOAN_CDECL init_491e50() { joan::orig::init_a39c5c(); }
void JOAN_CDECL init_491f00() { joan::orig::init_a39c60(); }
void JOAN_CDECL init_492010() { joan::orig::init_a39c64(); }
void JOAN_CDECL init_4920f0() { joan::orig::init_a39c68(); }

struct H {
    // ------------------------------------------------------------------ empty vtable methods
    void nop0() {}
    void nop1(u32) {}

    // ------------------------------------------------------------------ 0x00491e00 / 0x00492590
    i32 zero() { return 0; }

    // ------------------------------------------------------------------ 0x00492340
    std::uintptr_t init_492340(u32 type) { return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->set_kind(joan::orig::arg::i16(type))); }  // 0x004178e0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("init_491e50", 0x00491e50, &init_491e50);
B("init_491f00", 0x00491f00, &init_491f00);
B("init_492010", 0x00492010, &init_492010);
B("init_4920f0", 0x004920f0, &init_4920f0);
B("Obj::init_492340", 0x00492340, &H::init_492340);
B("Obj::nop_492390", 0x00492390, &H::nop0);
B("Obj::nop_4923e0", 0x004923e0, &H::nop0);
B("Obj::nop_492410", 0x00492410, &H::nop0);
B("Obj::nop_4924a0", 0x004924a0, &H::nop1);
B("Obj::zero_492590", 0x00492590, &H::zero);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

float pi_value() {
    constexpr Fpu fpu{true};
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(fpu.mul(a, global<double>(d::dbl_4)));
}

void JOAN_CDECL init_a39c5c() { global<float>(d::g_pi_a39c5c) = pi_value(); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("init_a39c5c", 0x00491ec0, &init_a39c5c);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

// ---------------------------------------------------------------- static initialisers
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a39c60() { div_global(d::g_a39c60, d::g_pi_a39c5c, d::flt_2); }  // 0x00491f40

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("init_a39c60", 0x00491f40, &init_a39c60);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

void JOAN_CDECL init_a39c64() { global<float>(d::g_a39c64) = global<float>(d::g_pi_a39c5c) / global<float>(d::flt_3); }

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("init_a39c64", 0x00492080, &init_a39c64);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

void JOAN_CDECL init_a39c68() {  // 0x00492150
    global<float>(d::g_a39c68) = global<float>(d::g_pi_a39c5c) / global<float>(d::flt_4);  // 4.0f
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("init_a39c68", 0x00492150, &init_a39c68);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

using joan::img::obj;

using u8 = std::uint8_t;

using joan::orig::helpers::addr;

void* const kMarkers = obj(d::g_physic_sys);

JOAN_COUNTER(c_place_492180, "Cursor::place_492180");

struct H {
    // ------------------------------------------------------------------ 0x00492180
    std::uintptr_t place_492180() {
        c_place_492180.hit();
        if (jo::self<jo::Unit>(this)->model == nullptr) return addr(this);
        u32 pos[3];
        joan::orig::as_unit(this)->position(joan::orig::arg::mptr(static_cast<void*>(pos)));  // 0x004fedb0
        if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
            u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_players(kMarkers)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
            if (jo::as_shot(q)->state.zone != 0) {
                const float h = static_cast<float>(
                    Fpu{true}.add(jo::view<float>(jo::as_shot(q)->state.cell), global<double>(d::dbl_0_2)));  // + 0.2
                return thiscall<u32>(fn::Actor_set_position_5ac4b0, jo::self<jo::Unit>(this)->model, pos[0], pos[1], joan::orig::arg::bits(h));  // 0x005ac4b0 (z as float bits: the callee takes dwords)
            }
            return thiscall<u32>(fn::Actor_set_position_5ac4b0, jo::self<jo::Unit>(this)->model, pos[0], pos[1], pos[2]);
        }
        return thiscall<u32>(fn::Actor_set_position_5ac4b0, jo::self<jo::Unit>(this)->model, pos[0], pos[1], pos[2]);
    }

    // ------------------------------------------------------------------ 0x004924f0
    float value_4924f0() { return joan::orig::arg::as<float>(joan::orig::as_timer(this)->remaining_41d280()); }  // 0x0041d280
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Cursor::place_492180", 0x00492180, &H::place_492180);
B("Obj::value_4924f0", 0x004924f0, &H::value_4924f0);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b107_05.cpp (game/b107_05)
namespace {
namespace game_b107_05 {

struct H {
    // 0x004922e0
    void* scalar_dtor_4922e0(u32 flags) {
        joan::orig::as_order(this)->destruct_45();  // 0x0043a980
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_05", name, addr, fn)
B("Obj::scalar_dtor_4922e0", 0x004922e0, &H::scalar_dtor_4922e0);
#undef B

}  // namespace game_b107_05
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

struct H {
    // ------------------------------------------------------------------ forwarders
    template <fn::Code Thunk>
    u32 forward() { return thiscall<u32>(Thunk, this); }

    // ------------------------------------------------------------------ trivial
    H* self() { return this; }
    void nop1(u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Obj::forward_492460", 0x00492460, &H::forward<fn::Unit_release_419950>);  // 0x00419950
B("Obj::forward_492370", 0x00492370, &H::forward<fn::Unit_release_4251b0>);  // 0x004251b0
B("Obj::self_4925c0", 0x004925c0, &H::self);
B("Obj::nop_492480", 0x00492480, &H::nop1);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

struct H {
    // ------------------------------------------------------------------ small wrappers
    u32 base_42f210() { return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->create_sprite()); }  // 0x004923c0 -> 0x0042f210
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("X::base_42f210", 0x004923c0, &H::base_42f210);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_a39c5c() -> void { joan::bind::free<&::game_b97_02::init_a39c5c>(); }  // 0x491ec0
auto joan::orig::init_a39c60() -> void { joan::bind::free<&::game_b95_02::init_a39c60>(); }  // 0x491f40
auto joan::orig::init_a39c64() -> void { joan::bind::free<&::game_b98_02::init_a39c64>(); }  // 0x492080
auto joan::orig::init_a39c68() -> void { joan::bind::free<&::game_b96_02::init_a39c68>(); }  // 0x492150
