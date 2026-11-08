// game/unknown_i_obj02.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_obj02.cpp;
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
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

using joan::call::thiscall;

using joan::orig::helpers::op_delete;  // operator delete

// ---------------------------------------------------------------------- static initialisers
template <fn::Code Worker>
void JOAN_CDECL static_init() { cdecl_call<void>(Worker); }

struct H {
    // ------------------------------------------------------------------ deleting destructors
    template <fn::Code Dtor>
    H* deleting_dtor(u32 flags) {
        thiscall<u32>(Dtor, this);
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }

    // ------------------------------------------------------------------ forwarders
    template <fn::Code Thunk>
    u32 forward() { return thiscall<u32>(Thunk, this); }
    // st0 passed through unrounded as double (exact for any PC24 st0, incl. out-of-float-range).
    double remaining_4907c0() { return joan::orig::arg::as<double>(joan::orig::as_timer(this)->remaining_41d280()); }  // 0x0041d280
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("init_4903f0", 0x004903f0, &static_init<fn::init_pi_a39c34>);
B("init_490450", 0x00490450, &static_init<fn::init_a39c38>);
B("init_4904a0", 0x004904a0, &static_init<fn::init_a39c3c>);
B("init_4904f0", 0x004904f0, &static_init<fn::init_a39c40>);
B("Obj::deleting_dtor_490560", 0x00490560, &H::deleting_dtor<fn::Obj_dtor_43c750>);  // 0x0043c750
B("Obj::forward_4906e0", 0x004906e0, &H::forward<fn::Unit_release_419950>);  // 0x00419950
B("Obj::remaining_4907c0", 0x004907c0, &H::remaining_4907c0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

void JOAN_CDECL init_pi_a39c34() {  // 0x00490410
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a39c34) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("init_pi_a39c34", 0x00490410, &init_pi_a39c34);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

// --------------------------------------------------------------------------------- cdecl
void JOAN_CDECL init_a39c3c() { global<float>(d::g_a39c3c) = global<float>(d::g_a39c34) / 3.0f; }  // 0x004904c0

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("init_a39c3c", 0x004904c0, &init_a39c3c);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

void JOAN_CDECL init_a39c40() { global<float>(d::g_a39c40) = global<float>(d::g_a39c34) / global<float>(d::flt_4); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("init_a39c40", 0x00490510, &init_a39c40);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

struct H {
    // ------------------------------------------------------------------ empty vtable methods
    void nop0() {}
    void nop1(u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::nop_490610", 0x00490610, &H::nop0);
B("Obj::nop_490660", 0x00490660, &H::nop0);
B("Obj::nop_490690", 0x00490690, &H::nop0);
B("Obj::nop_490700", 0x00490700, &H::nop1);
B("Obj::nop_490750", 0x00490750, &H::nop1);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::hook::global;

static void JOAN_CDECL init_a39c38() { global<float>(d::g_a39c38) = global<float>(d::g_a39c34) / global<float>(d::flt_2); }  // 0x00490470

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("init_a39c38", 0x00490470, &init_a39c38);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MouseCursor::time_left() -> double { return joan::bind::conv<double>(joan::bind::member<&::game_b105_05::H::remaining_4907c0>(this)); }  // 0x4907c0
