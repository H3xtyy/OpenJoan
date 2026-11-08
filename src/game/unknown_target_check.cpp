// game/unknown_target_check.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_0049cc10.cpp; re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
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

using joan::call::cdecl_call;

using joan::call::thiscall;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;


using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

// ---------------------------------------------------------------------- static initialisers
template <fn::Code Worker>
void JOAN_CDECL static_init() { cdecl_call<void>(Worker); }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("init_49cc10", 0x0049cc10, &static_init<fn::init_a39c98>);
B("init_49ced0", 0x0049ced0, &static_init<fn::init_a39c9c>);
B("init_49d400", 0x0049d400, &static_init<fn::init_49d420>);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

using u8 = std::uint8_t;

using joan::orig::helpers::addr;  // the result register (the callers ignore it)

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_update_49cc30, "Obj::update_49cc30");

void JOAN_CDECL init_49d1e0() { joan::orig::init_a39ca0(); }

struct H {
    // ------------------------------------------------------------------ 0x0049cc30
    std::uintptr_t update_49cc30() {
        c_update_49cc30.hit();
        if (jo::view<i32>(jo::self<jo::Order0c_b>(this)->u32_c6) == 1) {
            const u32 busy = joan::orig::arg::as<u32>(joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::self<jo::Order0c_b>(this)->u32_ca)));  // 0x00428b70
            if (busy != 0) return busy;
            u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::self<jo::Order0c_b>(this)->u32_ca)));                 // 0x0041b3c0
            u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(q)->type_id)));  // 0x0042d0a0
            if (static_cast<i32>(jo::as_unit_type(t)->unit_class) != 8) return addr(t);
            if (static_cast<i32>(jo::as_unit_type(t)->kind) != 8) return addr(t);
            if (jo::as_unit(q)->siege_tower_state == 1) {
                if (joan::orig::arg::as<u32>(joan::orig::as_unit(this)->play_effect(static_cast<char>(joan::orig::arg::i32(0)))) == 0) return 0;  // 0x00477360
                jo::view<u32>(jo::as_unit(q)->siege_tower_state) = 2;
                return addr(q);
            }
            if (jo::as_unit(q)->siege_tower_state != 3) return addr(t);
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_unit(this)->play_effect(static_cast<char>(joan::orig::arg::i32(1))));  // 0x00477360
            if (r != 0) jo::view<u32>(jo::as_unit(q)->siege_tower_state) = 0;
            return r;
        }
        if (jo::view<i32>(jo::self<jo::Order0c_b>(this)->u32_c6) != 2) return addr(this);
        const u32 busy = joan::orig::arg::as<u32>(joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::self<jo::Order0c_b>(this)->u32_ca)));  // 0x00428b70
        if (busy != 0) return busy;
        u8* const s = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Order0c_b>(this)->u32_ca)));  // 0x00425d20
        const u32 on = jo::view<i32>(jo::as_map_object(s)->busy) > 0 ? 1u : 0u;
        return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->play_effect(static_cast<char>(joan::orig::arg::i32(on))));  // 0x00477360
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::update_49cc30", 0x0049cc30, &H::update_49cc30);
B("init_49d1e0", 0x0049d1e0, &init_49d1e0);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::hook::x87::Fpu;

float pi_value() {
    constexpr Fpu fpu{true};
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(fpu.mul(a, global<double>(d::dbl_4)));
}

void JOAN_CDECL init_a39c98() { global<float>(d::g_a39c98) = pi_value(); }

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("init_a39c98", 0x0049cd80, &init_a39c98);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

void* const kText = obj(d::g_misc);

JOAN_COUNTER(c_check_target, "X::check_target");

struct H {
    // ------------------------------------------------------------------------ 0x0049cef0
    void check_target(void* unit) {
        c_check_target.hit();
        auto kind = [unit]() { return static_cast<i32>(jo::as_unit(unit)->anim_kind); };
        if (kind() == 0x40 || kind() == 0x3c || kind() == 0x40 || kind() == 0x41 || kind() == 0x42 ||
            kind() == 0x3c || kind() == 0x29 || kind() == 0x4e) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_kind_510f80()) != 0) {  // 0x00510f80
            jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            return;
        }
        void* const t = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
        if (static_cast<i32>(jo::as_unit_type(t)->unit_class) == 8 && static_cast<i32>(jo::as_unit_type(t)->kind) != 6 &&
            static_cast<i32>(jo::as_unit_type(t)->kind) != 0xa) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(1)))) == 0) {  // 0x0050d550
                jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
                return;
            }
        }
        const double z = joan::orig::arg::as<double>(joan::orig::as_unit(unit)->anim_phase());  // 0x005045f0
        if (!(z < 0.0 || z > 0.0)) {  // equal or unordered
            joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
        }
        float a0c, a10, o18;
        joan::orig::as_angle(&a0c)->construct_577190();                                   // 0x00577190
        joan::orig::as_angle(&a0c)->set_wrapped(joan::orig::arg::f32(joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading())));   // 0x00577340 (0x004ff850)
        joan::orig::as_angle(&a10)->construct_577190();                                   // 0x00577190
        joan::orig::as_angle(&a10)->set_degrees(joan::orig::arg::f32(static_cast<float>(jo::self<jo::Order>(this)->current.target)));   // 0x005773e0
        float* const diff = joan::orig::arg::as<float*>(joan::orig::as_turn(&a10)->sub(&o18, &a0c));  // 0x005775b0
        const double deg = joan::orig::arg::as<double>(joan::orig::as_angle(diff)->degrees());              // 0x00577450
        const i32 k = joan::orig::arg::as<i32>(joan::orig::as_misc(kText)->round(joan::orig::arg::f64(deg)));               // 0x00590a20
        if (k <= 1 || k >= 0x167) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            return;
        }
        joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(static_cast<float>(jo::self<jo::Order>(this)->current.target)));  // 0x0050abd0
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("X::check_target", 0x0049cef0, &H::check_target);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

// ---------------------------------------------------------------- static initialisers
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a39c9c() { div_global(d::g_a39c9c, d::g_a39c98, d::flt_2); }  // 0x0049d110

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("init_a39c9c", 0x0049d110, &init_a39c9c);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

// ---------------------------------------------------------------------------- 0x0049d420
static void JOAN_CDECL init_49d420() { global<float>(d::g_a39ca4) = global<float>(d::g_a39c98) / global<float>(d::flt_4); }

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("init_49d420", 0x0049d420, &init_49d420);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::hook::global;

static void JOAN_CDECL init_a39ca0() { global<float>(d::g_a39ca0) = global<float>(d::g_a39c98) / global<float>(d::flt_3); }  // 0x0049d390

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("init_a39ca0", 0x0049d390, &init_a39ca0);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_a39ca0() -> void { joan::bind::free<&::game_b100_04::init_a39ca0>(); }  // 0x49d390
