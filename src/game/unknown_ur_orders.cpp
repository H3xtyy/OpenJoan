// game/unknown_ur_orders.cpp: functions whose original source file is unknown (formerly game/_unplaced/ur_orders.cpp;
// re/attribution.csv: between ww/upgrares.cpp and ww/uscrres.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cmath>

#include "../lib/crt/crt.hpp"
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

using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::str;

using i16 = std::int16_t;

using joan::img::obj;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::global;

using u16 = std::uint16_t;

void* const kUnits = obj(d::g_units);

void unit_stop(i32 id) { joan::orig::script::unit_528300(joan::orig::arg::i32(id)); }                      // 0x00652370
i32 dist_3150(i32 id, i32 t16) { return joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), joan::orig::arg::i32(t16))); }  // 0x00653150
i32 can_melee(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))); }               // 0x00651590
i32 can_move(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))); }                // 0x00651550
i32 check_1430(i32 id, i32 t16) { return joan::orig::arg::as<i32>(joan::orig::script::in_arc(joan::orig::arg::i32(id), joan::orig::arg::i32(t16))); }  // 0x00651430

const char* const kNearestHigh = str(d::s_NEAREST_HIGH_PRIORITY);  // "NEAREST_HIGH_PRIORITY"
const char* const kAny = str(d::s_T);

// cdecl unit helpers (short id arguments read with movsx word by the callees).
i32 nearest(i32 id, const char* kind, i32 range, i32 a, i32 b, i32 c, i32 d) {
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), kind, joan::orig::arg::i32(range), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d)));  // 0x006517c0
}
i32 range_b280(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }    // 0x0065b280
i32 dist_b800(i32 id, i32 a, i32 b, i32 c) { return joan::orig::arg::as<i32>(joan::orig::unit::distance_to(joan::orig::arg::i16(id), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c))); }  // 0x0065b800

void* const kTypes = obj(d::g_types);

}  // namespace

// ---------------------------------------- from modules/game_b90_01.cpp (game/b90_01)
namespace {
namespace game_b90_01 {

void* const kRandom = obj(d::g_misc);
const char* const kNearest = str(d::s_NEAREST);  // "NEAREST"
const char* const kWeakest = str(d::s_WEAKEST);  // "WEAKEST"
const char* const kDefault = str(d::s_DEFAULT);  // "DEFAULT"
const char* const kCamp = str(d::s_CAMP);     // "CAMP"
const char* const kM = str(d::s_M_9d006c);        // "M"
const char* const kF = str(d::s_F_9d0068);        // "F"
const char* const kE = str(d::s_E_9cfc2c);        // "E"

JOAN_COUNTER(c_run1, "Order::run_1");
JOAN_COUNTER(c_run2, "Order::run_2");
JOAN_COUNTER(c_run3, "Order::run_3");
JOAN_COUNTER(c_run4, "Order::run_4");
JOAN_COUNTER(c_run5, "Order::run_5");
JOAN_COUNTER(c_run6, "Order::run_6");
JOAN_COUNTER(c_run7, "Order::run_7");
JOAN_COUNTER(c_run8, "Order::run_8");
JOAN_COUNTER(c_run9, "Order::run_9");
JOAN_COUNTER(c_run10, "Order::run_10");
JOAN_COUNTER(c_run11, "Order::run_11");
JOAN_COUNTER(c_run12, "Order::run_12");
JOAN_COUNTER(c_run13, "Order::run_13");
JOAN_COUNTER(c_run14, "Order::run_14");
JOAN_COUNTER(c_run15, "Order::run_15");

i32 unit_id(const void* unit) { return static_cast<i32>(*static_cast<const i16*>(unit)); }
bool unit_busy(void* unit) {  // 0x0042d0e0 (char, movsx + test)
    return static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(unit)->dead())) != 0;
}
void set_target_mode(i32 id, const char* kw) { joan::orig::script::set_target_mode(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::str(kw)); }  // 0x00651690
void set_mode2(i32 id, const char* kw) { joan::orig::unit_set_mood(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::str(kw)); }        // 0x0065c050
i32 state_of(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::formation(joan::orig::arg::i32(static_cast<i16>(id)))); }                         // 0x006520d0
i32 check_6512d0(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(static_cast<i16>(id)))); }                     // 0x006512d0
i32 flag_of(i32 id, const char* f) { return joan::orig::arg::as<i32>(joan::orig::unit::guard_check(joan::orig::arg::i16(static_cast<i16>(id)), joan::orig::arg::str(f))); }         // 0x00656bb0
void call_6618f0(i32 id) { joan::orig::group_release(joan::orig::arg::i32(id)); }                                              // 0x006618f0
i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
i32 call_6517c0(i32 id, const char* f) {                                                                   // 0x006517c0
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(static_cast<i16>(id)), f, i32{0x64}, i32{0}, i32{0}, i32{0}, i32{0}));
}
void call_661340(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {  // 0x00661340
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));
}

// Shared preamble of 0x00531f90..0x00532ef0. Returns true when the handler returns 1 here.
bool preamble(void* unit, const char* kw, std::uint32_t& state) {
    const i32 id = unit_id(unit);  // ebp-0x8
    if (unit_busy(unit)) return true;
    set_target_mode(id, kw);
    state = static_cast<std::uint32_t>(state_of(id));  // ebp-0xc
    if (check_6512d0(id) != 0) {
        const i32 r = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(static_cast<i16>(id)), i32{1}));  // 0x00652190, ebp-0x14
        if (state == 7 && r == 0) {
            joan::orig::script::unit_528300(joan::orig::arg::i32(static_cast<i16>(id)));         // 0x00652370
            const i32 other = joan::orig::arg::as<i32>(joan::orig::script::leader_id(joan::orig::arg::i32(id)));          // 0x006569f0, ebp-0x10
            if (other != 0 && state_of(other) == 7)
                joan::orig::script::unit_528300(joan::orig::arg::i32(static_cast<i16>(other)));  // 0x00652370
        }
        return true;
    }
    return false;
}

// rand(200) - 100, pushed away from 0 by 50 (0x00533b10).
i32 offset() {
    i32 v = random(0xc8) - 0x64;
    return v;
}
void spread(i32& v) {
    if (v > 0) v += 0x32;
    if (v < 0) v -= 0x32;
}

struct H {
    i32 call(std::uintptr_t thunk, void* unit) { return thiscall<i32>(thunk, this, unit); }
    bool has_leader(void* unit) { return joan::orig::arg::as<void*>(joan::orig::as_unit(unit)->leader()) != nullptr; }  // 0x004216d0

    // ------------------------------------------------------------------------ 0x00531f90
    i32 run1(void* unit) {
        c_run1.hit();
        std::uint32_t state = 0;
        if (preamble(unit, kNearest, state)) return 1;
        switch (state) {
        case 0: case 1: case 9: case 18: return call(fn::code_value(fn::AiState_think_534090), unit);  // 0x00534090
        case 10:
            if (has_leader(unit)) return call(fn::code_value(fn::AiState_think_534090), unit);       // 0x00534090
            return call(fn::code_value(fn::Ai_state_5377b0), unit);                             // 0x005377b0
        case 3: case 4: case 5: return 1;
        case 6: return call(fn::code_value(fn::fn_534880), unit);   // 0x00534880
        case 7: return 1;
        case 11: return call(fn::code_value(fn::Ai_state_545d50), unit);  // 0x00545d50
        case 12: return call(fn::code_value(fn::Order_run_return), unit);  // 0x00545ea0
        case 13: return call(fn::code_value(fn::Ai_state_535870), unit);  // 0x00535870
        case 17: return call(fn::code_value(fn::Ai_state_535a20), unit);  // 0x00535a20
        case 14: return call(fn::code_value(fn::Order_run_e), unit);  // 0x00546050
        case 15: return call(fn::code_value(fn::unit_ai_sound_slot), unit);  // 0x00535020
        case 19: return call(fn::code_value(fn::Ai_state_535e30), unit);  // 0x00535e30
        case 101: case 102: case 103: case 104: case 105: return 0;
        default: return call(fn::code_value(fn::AiState_think_534090), unit);  // 0x00534090
        }
    }

    // ------------------------------------------------------------------------ 0x005322c0
    i32 run2(void* unit) {
        c_run2.hit();
        std::uint32_t state = 0;
        if (preamble(unit, kNearest, state)) return 1;
        switch (state) {
        case 0: case 3: case 4: return call(fn::code_value(fn::AiTask_pick_attack_target), unit);  // 0x00536390
        case 1: return call(fn::code_value(fn::AI_decide_536a40), unit);                  // 0x00536a40
        case 5: return 1;
        case 6: return call(fn::code_value(fn::UnitAi_think_attack_a), unit);                  // 0x005370d0
        case 7: return 1;
        case 10: case 18: return call(fn::code_value(fn::Ai_state_5377b0), unit);        // 0x005377b0
        case 11: return call(fn::code_value(fn::Ai_state_545d50), unit);                 // 0x00545d50
        case 12: return call(fn::code_value(fn::Order_run_return), unit);                 // 0x00545ea0
        case 13: return call(fn::code_value(fn::Unit_script_attack), unit);                 // 0x00537f90
        case 17: return call(fn::code_value(fn::AiState_think_538140), unit);                 // 0x00538140
        case 14: return call(fn::code_value(fn::Order_run_e), unit);                 // 0x00546050
        case 19: return call(fn::code_value(fn::Ai_state_538730), unit);                 // 0x00538730
        default: return 1;
        }
    }

    // ------------------------------------------------------------------------ 0x00532520
    i32 run3(void* unit) {
        c_run3.hit();
        std::uint32_t state = 0;
        if (preamble(unit, kNearest, state)) return 1;
        switch (state) {
        case 0: case 3: case 4: return call(fn::code_value(fn::Unit_react_538c90), unit);  // 0x00538c90
        case 1: return call(fn::code_value(fn::UnitAi_step_539a40), unit);                  // 0x00539a40
        case 5: return 1;
        case 6: return call(fn::code_value(fn::Ai_choose_attack), unit);                  // 0x0053a060
        case 7: return 1;
        case 10: case 18: return call(fn::code_value(fn::Order_attack_ai), unit);        // 0x005392d0
        case 11: return call(fn::code_value(fn::Ai_state_545d50), unit);                 // 0x00545d50
        case 12: return call(fn::code_value(fn::Order_run_return), unit);                 // 0x00545ea0
        case 13: return call(fn::code_value(fn::Order_attack_53a6d0), unit);                 // 0x0053a6d0
        case 17: return call(fn::code_value(fn::Unit_order_attack_53a9d0), unit);                 // 0x0053a9d0
        case 14: return call(fn::code_value(fn::Order_run_e), unit);                 // 0x00546050
        case 19: return call(fn::code_value(fn::Ai_state_53b1b0), unit);                 // 0x0053b1b0
        default: return 1;
        }
    }

    // ------------------------------------------------------------------------ 0x00532780
    i32 run4(void* unit) {
        c_run4.hit();
        std::uint32_t state = 0;
        if (preamble(unit, kNearest, state)) return 1;
        switch (state) {
        case 0: case 1: case 9: case 18: return call(fn::code_value(fn::Ai_state_53b610), unit);  // 0x0053b610
        case 10:
            if (has_leader(unit)) return call(fn::code_value(fn::Ai_state_53b610), unit);       // 0x0053b610
            return call(fn::code_value(fn::UnitAi_think_attack_b), unit);                             // 0x0053e9d0
        case 3: case 4: case 5: return 1;
        case 6: return call(fn::code_value(fn::Ai_state_53beb0), unit);   // 0x0053beb0
        case 7: return 1;
        case 11: return call(fn::code_value(fn::Ai_state_545d50), unit);  // 0x00545d50
        case 12: return call(fn::code_value(fn::Order_run_return), unit);  // 0x00545ea0
        case 13: return call(fn::code_value(fn::Ai_state_53c700), unit);  // 0x0053c700
        case 17: return call(fn::code_value(fn::Ai_state_53c960), unit);  // 0x0053c960
        case 14: return call(fn::code_value(fn::Order_run_e), unit);  // 0x00546050
        case 19: return call(fn::code_value(fn::Ai_state_53cda0), unit);  // 0x0053cda0
        default: return call(fn::code_value(fn::Ai_state_53b610), unit);  // 0x0053b610
        }
    }

    // ------------------------------------------------------------------------ 0x00532a00
    i32 run5(void* unit) {
        c_run5.hit();
        std::uint32_t state = 0;
        if (preamble(unit, kNearest, state)) return 1;
        switch (state) {
        case 0: case 3: case 4: return call(fn::code_value(fn::Ai_state_53d3b0), unit);  // 0x0053d3b0
        case 1: return call(fn::code_value(fn::UnitAI_think_53db10), unit);                  // 0x0053db10
        case 5: return 1;
        case 6: return call(fn::code_value(fn::UnitAI_think_attack), unit);                  // 0x0053e240
        case 7: return 1;
        case 10: case 18: return call(fn::code_value(fn::UnitAi_think_attack_b), unit);        // 0x0053e9d0
        case 11: return call(fn::code_value(fn::Ai_state_545d50), unit);                 // 0x00545d50
        case 12: return call(fn::code_value(fn::Order_run_return), unit);                 // 0x00545ea0
        case 13: return call(fn::code_value(fn::AiState_think_53f260), unit);                 // 0x0053f260
        case 17: return call(fn::code_value(fn::Order_attack_simple), unit);                 // 0x0053f4c0
        case 14: return call(fn::code_value(fn::Order_run_e), unit);                 // 0x00546050
        case 19: return call(fn::code_value(fn::Ai_state_53fa60), unit);                 // 0x0053fa60
        default: return 1;
        }
    }

    // ------------------------------------------------------------------------ 0x00532c60
    i32 run6(void* unit) {
        c_run6.hit();
        std::uint32_t state = 0;
        if (preamble(unit, kWeakest, state)) return 1;
        switch (state) {
        case 0: case 1: case 9: case 18: return call(fn::code_value(fn::Ai_think_540070), unit);  // 0x00540070
        case 10:
            if (has_leader(unit)) return call(fn::code_value(fn::Ai_think_540070), unit);       // 0x00540070
            return call(fn::code_value(fn::Ai_state_542820), unit);                             // 0x00542820
        case 3: case 4: case 5: return 1;
        case 6: return call(fn::code_value(fn::script_540640), unit);   // 0x00540640
        case 8: return call(fn::code_value(fn::Script_nearest_archer), unit);   // 0x00540c10
        case 7: return 1;
        case 11: return call(fn::code_value(fn::Ai_state_545d50), unit);  // 0x00545d50
        case 12: return call(fn::code_value(fn::Order_run_return), unit);  // 0x00545ea0
        case 13: return call(fn::code_value(fn::Unit_attack_540e80), unit);  // 0x00540e80
        case 17: return call(fn::code_value(fn::Order_attack_step), unit);  // 0x005410b0
        case 14: return call(fn::code_value(fn::Order_run_e), unit);  // 0x00546050
        case 19: return call(fn::code_value(fn::Unit_ai_state_541300), unit);  // 0x00541300
        default: return call(fn::code_value(fn::Ai_think_540070), unit);  // 0x00540070 (2, 15, 16, > 0x13)
        }
    }

    // ------------------------------------------------------------------------ 0x00532ef0
    i32 run7(void* unit) {
        c_run7.hit();
        std::uint32_t state = 0;
        if (preamble(unit, kWeakest, state)) return 1;
        switch (state) {
        case 0: case 9: case 18: return call(fn::code_value(fn::Ai_state_541600), unit);  // 0x00541600
        case 3: case 4: case 5: return 1;
        case 10: return call(fn::code_value(fn::Ai_state_542820), unit);  // 0x00542820
        case 6: return call(fn::code_value(fn::Ai_think_archer), unit);   // 0x00541b10
        case 1: return call(fn::code_value(fn::unit_ai_archer), unit);   // 0x005420a0
        case 8: return call(fn::code_value(fn::ArcherCaptain_setup_5425d0), unit);   // 0x005425d0
        case 7: return 1;
        case 11: return call(fn::code_value(fn::Ai_state_545d50), unit);  // 0x00545d50
        case 12: return call(fn::code_value(fn::Order_run_return), unit);  // 0x00545ea0
        case 13: return call(fn::code_value(fn::Ai_attack_or_activate), unit);  // 0x00542e60
        case 17: return call(fn::code_value(fn::Ai_attack_step_543090), unit);  // 0x00543090
        case 14: return call(fn::code_value(fn::Order_run_e), unit);  // 0x00546050
        case 19: return call(fn::code_value(fn::Script_archer_cover), unit);  // 0x00543330
        default: return call(fn::code_value(fn::Ai_state_541600), unit);  // 0x00541600 (2, 15, 16, > 0x13)
        }
    }

    // ------------------------------------------------------------------------ 0x00533170
    i32 run8(void* unit) {
        c_run8.hit();
        const i32 id = unit_id(unit);
        set_target_mode(id, kWeakest);
        if (unit_busy(unit)) return 1;
        if (check_6512d0(id) != 0) return 1;
        const i32 m = flag_of(id, kM);  // ebp-0x14
        const i32 f = flag_of(id, kF);  // ebp-0x18
        if (m == 0 && f == 0) {
            call_6618f0(id);
            return 1;
        }
        const i32 state = state_of(id);  // ebp-0x1c
        if (state == 7) return 1;
        if (m != 0) {
            if (state == 0xc) return call(fn::code_value(fn::Order_run_return), unit);   // 0x00545ea0
            if (state == 0xe) return call(fn::code_value(fn::Order_run_e), unit);   // 0x00546050
            if (state == 0x13) return call(fn::code_value(fn::Ai_attack_or_idle), unit);  // 0x00543c10
        }
        if (f == 0) return 1;
        if (state == 0 || state == 0xb) return call(fn::code_value(fn::Unit_ai_state_543800), unit);  // 0x00543800
        if (state == 1) return call(fn::code_value(fn::Unit_ai_state_543800), unit);                  // 0x00543800
        if (state == 0xd) return call(fn::code_value(fn::Unit_ai_state_5442d0), unit);                // 0x005442d0
        if (state == 0x11) return call(fn::code_value(fn::Ai_state_544690), unit);               // 0x00544690
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00533370
    i32 run9(void* unit) {
        c_run9.hit();
        const i32 id = unit_id(unit);  // ebp-0xc
        set_target_mode(id, kWeakest);
        if (unit_busy(unit)) return 1;
        if (check_6512d0(id) != 0) return 1;
        const i32 state = state_of(id);  // ebp-0x8
        if (state == 7) return 1;
        if (state == 0) return call(fn::code_value(fn::Order_nearest_archer), unit);  // 0x00544cb0
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00533440
    i32 run10(void* unit) {
        c_run10.hit();
        const i32 id = unit_id(unit);
        set_target_mode(id, kWeakest);
        if (joan::orig::arg::as<i32>(joan::orig::unit_state_is_42(joan::orig::arg::i32(static_cast<i16>(id)))) != 0) return 1;  // 0x00653c60
        if (check_6512d0(id) != 0) return 1;
        const i32 m = flag_of(id, kM);  // ebp-0x14
        const i32 f = flag_of(id, kF);  // ebp-0x18
        if (m == 0 && f == 0) {
            call_6618f0(id);
            return 1;
        }
        const i32 state = state_of(id);  // ebp-0x1c
        if (state == 0xc || state == 0x13) {
            if (m != 0) return call(fn::code_value(fn::Order_run_return), unit);  // 0x00545ea0
            return 1;
        }
        if (state == 0x11 && f != 0) return call(fn::code_value(fn::Ai_state_5450b0), unit);  // 0x005450b0
        if (state == 0x65) return call(fn::code_value(fn::Ai_state_544e50), unit);            // 0x00544e50
        return 1;
    }

    // ------------------------------------------------------------------------ 0x005335e0
    i32 run11(void* unit) {
        c_run11.hit();
        const i32 id = unit_id(unit);
        set_target_mode(id, kNearest);
        if (joan::orig::arg::as<i32>(joan::orig::unit_state_is_42(joan::orig::arg::i32(static_cast<i16>(id)))) != 0) return 1;  // 0x00653c60
        if (check_6512d0(id) != 0) return 1;
        if (call_6517c0(id, kE) == 0) return 1;  // ebp-0xc
        if (random(0x64) > 0x46)                 // ebp-0x10, signed (jle)
            call_661340(id, 0x16, 7, 0, 2, 0, 1, 0, 0x2710);
        return 1;
    }

    // ------------------------------------------------------------------------ 0x005336e0
    i32 run12(void* unit) {
        c_run12.hit();
        const i32 id = unit_id(unit);
        set_target_mode(id, kNearest);
        set_mode2(id, kDefault);
        if (jo::as_unit(unit)->frozen == 0 &&
            joan::orig::arg::as<i32>(joan::orig::game_flag_744()) != 0)  // 0x0065b130
            return 1;
        if (unit_busy(unit)) return 1;
        const i32 state = state_of(id);  // ebp-0xc
        if (state == 0) return 1;
        if (state == 7) return 1;
        if (state == 0xa) return call(fn::code_value(fn::Ai_guard_step), unit);  // 0x00545430
        if (state == 0xe) return call(fn::code_value(fn::Order_run_e), unit);  // 0x00546050
        if (state == 0xc) return call(fn::code_value(fn::Order_run_return), unit);  // 0x00545ea0
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00533810
    i32 run13(void* unit) {
        c_run13.hit();
        const i32 id = unit_id(unit);
        set_target_mode(id, kNearest);
        set_mode2(id, kDefault);
        if (unit_busy(unit)) return 1;
        const i32 state = state_of(id);  // ebp-0xc
        if (state == 0) return call(fn::code_value(fn::Order_scatter_from_e), unit);  // 0x00545590
        if (state == 7) return 1;
        if (state == 0xa) call(fn::code_value(fn::Order_flee), unit);       // 0x00545900 (falls through, eax dropped)
        if (state == 0xe) return call(fn::code_value(fn::Order_run_e), unit);   // 0x00546050
        if (state == 0x65) return call(fn::code_value(fn::Order_random_wander), unit);  // 0x00545a60
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00533920
    i32 run14(void* unit) {
        c_run14.hit();
        const i32 id = unit_id(unit);
        set_target_mode(id, kWeakest);
        if (unit_busy(unit)) return 1;
        if (check_6512d0(id) != 0) return 1;
        const i32 m = flag_of(id, kM);  // ebp-0x14
        const i32 f = flag_of(id, kF);  // ebp-0x18
        if (m == 0 && f == 0) {
            call_6618f0(id);
            return 1;
        }
        const i32 state = state_of(id);  // ebp-0x1c
        if (state == 7) return 1;
        if (m != 0) {
            if (state == 0xc || state == 0x13) return call(fn::code_value(fn::Order_run_return), unit);  // 0x00545ea0
            if (state == 0xe) return call(fn::code_value(fn::Order_run_e), unit);                   // 0x00546050
        }
        if (f == 0) return 1;
        if (state == 0 || state == 0xb) return call(fn::code_value(fn::Ai_think_siege), unit);  // 0x00543fd0
        if (state == 1) return call(fn::code_value(fn::UnitAI_start), unit);                  // 0x00545b80
        if (state == 0xd) return call(fn::code_value(fn::Ai_think_attack), unit);                // 0x00544500
        if (state == 0x11) return call(fn::code_value(fn::Ai_attack_step_544aa0), unit);               // 0x00544aa0
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00533b10
    i32 run15(void* unit) {
        c_run15.hit();
        const i32 id = unit_id(unit);
        set_target_mode(id, kNearest);
        set_mode2(id, kDefault);
        if (unit_busy(unit)) return 1;
        bool go = false;
        bool camp = true;
        if (check_6512d0(id) != 0) {
            go = true;
        } else if (call_6517c0(id, kE) != 0 && random(0x64) > 0x50) {
            go = true;
        } else if (call_6517c0(id, kF) != 0 && random(0x64) > 0x5f) {
            go = true;
        } else if (random(0x64) >= 0x50) {
            go = true;
            camp = false;
        }
        if (!go) return 1;
        i32 dx = offset();
        i32 dy = offset();
        i32 dz = offset();
        spread(dx);
        spread(dy);
        spread(dz);
        if (camp) set_mode2(id, kCamp);
        call_661340(id, 8, -1, 0, dx, dy, dz, 0, 0x1388);
        return 1;
    }
};

#define B90(name, addr, fn) JOAN_REPLACE("game/b90_01", name, addr, fn)
B90("Order::run_1", 0x00531f90, &H::run1);
B90("Order::run_2", 0x005322c0, &H::run2);
B90("Order::run_3", 0x00532520, &H::run3);
B90("Order::run_4", 0x00532780, &H::run4);
B90("Order::run_5", 0x00532a00, &H::run5);
B90("Order::run_6", 0x00532c60, &H::run6);
B90("Order::run_7", 0x00532ef0, &H::run7);
B90("Order::run_8", 0x00533170, &H::run8);
B90("Order::run_9", 0x00533370, &H::run9);
B90("Order::run_10", 0x00533440, &H::run10);
B90("Order::run_11", 0x005335e0, &H::run11);
B90("Order::run_12", 0x005336e0, &H::run12);
B90("Order::run_13", 0x00533810, &H::run13);
B90("Order::run_14", 0x00533920, &H::run14);
B90("Order::run_15", 0x00533b10, &H::run15);
#undef B90

}  // namespace game_b90_01
}  // namespace

// ---------------------------------------- from modules/game_b106_06.cpp (game/b106_06)
namespace {
namespace game_b106_06 {

JOAN_COUNTER(c_think_534090, "AiState::think_534090");
JOAN_COUNTER(c_think_538140, "AiState::think_538140");
JOAN_COUNTER(c_think_53f260, "AiState::think_53f260");

// ---- unit AI helpers (all cdecl) ----
u32 order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    return joan::orig::arg::as<u32>(joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));  // 0x00661340
}
i32 script_value(i16 id, img::Addr name, i32 a, i32 b, i32 c, i32 d, i32 e) {
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(name), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));  // 0x006517c0
}
void set_state(i16 id, img::Addr name) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(str(name))); }  // 0x006515d0
i32 unit_target(i16 id, i32 v) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(v))); }               // 0x00652190
u32 unit_6512d0(i16 id) { return joan::orig::arg::as<u32>(joan::orig::script::is_building(joan::orig::arg::i32(id))); }                         // 0x006512d0
u32 unit_652370(i16 id) { return joan::orig::arg::as<u32>(joan::orig::script::unit_528300(joan::orig::arg::i32(id))); }                         // 0x00652370
u32 unit_651590(i16 id) { return joan::orig::arg::as<u32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))); }                         // 0x00651590
u32 unit_651550(i16 id) { return joan::orig::arg::as<u32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))); }                         // 0x00651550
u32 unit_65b6a0(i32 t) { return joan::orig::arg::as<u32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(t))); }                           // 0x0065b6a0
i32 unit_653000(i16 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }               // 0x00653000
i32 unit_652ee0(i16 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }               // 0x00652ee0
i32 unit_65b280(i32 id, i32 v) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(v))); }               // 0x0065b280
i32 random_590540(i32 n) { return joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(n)); }  // 0x00590540

constexpr img::Addr kAttack = d::s_ATTACK_9cf380;   // "ATTACK"
constexpr img::Addr kActive = d::s_ACTIVE;   // "ACTIVE"
constexpr img::Addr kIdle = d::s_IDLE;     // "IDLE"
constexpr img::Addr kNearest = d::s_NEAREST_HIGH_PRIORITY;  // "NEAREST_HIGH_PRIORITY"

struct H {
    // 0x00534090
    u32 think_534090(i16* arg) {
        c_think_534090.hit();
        const i32 id = *arg;
        const i16 sid = static_cast<i16>(id);
        i32 v10 = 0, v14 = 0, v1c = 0, v20 = 0, v24 = 0;
        v1c = script_value(sid, d::s_T, 0, 0, 0, 0, 0);
        if (joan::orig::arg::as<i32>(joan::orig::script::leader_id(joan::orig::arg::i32(id))) > 0) {  // 0x006569f0
            v20 = script_value(sid, d::s_L, 0x3e8, 0, 0, 0, 0);
            if (v20 == 0) {
                order(id, 0xf, 2, 1, 0, 0, 0, 0, 0x2710);
                joan::orig::script::set_2d7(joan::orig::arg::i32(sid), joan::orig::arg::i32(static_cast<i16>(0)));  // 0x006558d0
                return 1;
            }
            if (v1c == 0) order(id, 0xf, -2, 1, 0, 0, 0, 0, 0x2710);
        }
        v10 = script_value(sid, d::s_E_9cfc2c, 0x96, 0, 0, 0, 0);
        if (v10 != 0) set_state(sid, kAttack);  // || [ebp-0x18] (always 0)
        if (v10 == 0) {
            set_state(sid, kActive);
            v10 = script_value(sid, d::s_E_9cfc2c, 0xc8, 0, 0, 0, 0);
            const i32 c = random_590540(0x32);
            if (v10 != 0 && c == 0) set_state(sid, kAttack);
        }
        if (unit_651590(sid) != 0) {
            if (v10 != 0) {
                const i32 v = unit_652ee0(sid, v10);
                order(id, 0xa, 3, 0, v, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_a(joan::orig::arg::mptr(arg)));  // 0x00535c20
            }
            return 1;  // [ebp-0x18] branch unreachable
        }
        if (unit_651550(sid) == 0) return 1;
        if (v20 > 0) v24 = script_value(static_cast<i16>(v20), d::s_T, 0, 0, 0, 0, 0);
        joan::orig::script::set_target_mode(joan::orig::arg::i32(sid), joan::orig::arg::str(str(kNearest)));  // 0x00651690
        if (unit_target(sid, 1) != 0) {
            const i32 t = unit_65b280(id, 1);
            v10 = script_value(sid, d::s_E_9cfc2c, t / 2, 1, 0, 0, 0);
        } else {
            const i32 t = unit_65b280(id, 1);
            v10 = script_value(sid, d::s_E_9cfc2c, t, 1, 0, 0, 0);
        }
        {
            const i32 t = unit_65b280(id, 1);
            v14 = script_value(sid, d::s_E_9cfc2c, t / 2, 0, 0, 0, 0);
        }
        if (v14 > 0) {
            void* u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(v14));  // 0x0041b3c0
            void* k = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));  // 0x0042d0a0
            if (jo::self<jo::UnitType>(k)->unit_class != 5 && jo::self<jo::UnitType>(k)->unit_class != 6) {
                const i32 c = random_590540(0xa);
                if (c < 1) {
                    order(id, 0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
                } else if (c < 5) {
                    const i32 v = unit_652ee0(sid, v10);
                    order(id, 0xa, 1, 0, v, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 1, 0, 0, 0, 0x5dc);
                } else if (c < 9) {
                    const i32 v = unit_652ee0(sid, v10);
                    order(id, 0xa, 1, 0, v, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 2, 0, 0, 0, 0x5dc);
                } else if (c < 10) {
                    const i32 v = unit_652ee0(sid, v10);
                    order(id, 0xa, 1, 0, v, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
                }
                return 1;
            }
        }
        if (v10 > 0) {
            order(id, 0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if ((v20 == 0 || v24 > 0) && unit_target(sid, 1) == 0) {
            joan::orig::script::set_target_mode(joan::orig::arg::i32(sid), joan::orig::arg::str(str(kNearest)));  // 0x00651690
            // both branches of `if ([ebp-0x1c] != 0)` are identical
            const i32 t = unit_65b280(id, 2);
            v10 = script_value(sid, d::s_E_9cfc2c, t, 1, 0, 0, 0);
            if (v10 > 0) {
                order(id, 0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        if (unit_target(sid, 1) == 0 && v1c > 0) {
            const i32 s = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(sid), joan::orig::arg::i32(static_cast<i16>(v1c))));  // 0x00653150
            const i32 t = unit_65b280(id, 3);
            if (!(s > t)) {
                order(id, 0xc, 1, 1, v1c, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        set_state(sid, kIdle);
        return 1;
    }

    // 0x00538140
    u32 think_538140(i16* arg) {
        c_think_538140.hit();
        const i32 id = *arg;
        const i16 sid = static_cast<i16>(id);
        const i32 target = unit_target(sid, 1);
        if (unit_6512d0(sid) != 0) {
            unit_652370(sid);
            return 1;
        }
        if (unit_65b6a0(target) == 0 || joan::orig::arg::as<u32>(joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(target))) != 0) {  // 0x00425d50
            unit_652370(sid);
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(sid), joan::orig::arg::i32(target))) >= 0x96) {  // 0x00653060
            order(id, 0xd, 1, 0, target, 0x64, 0x32, 0, 0x7530);
            return 1;
        }
        set_state(sid, kAttack);
        if (unit_651590(sid) != 0) {
            if (unit_65b6a0(target) == 0) {
                unit_652370(sid);
                return 1;
            }
            const i32 v = unit_653000(sid, target);
            order(id, 0xa, 4, 0, v, 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(arg)));  // 0x00538340
        }
        if (unit_651550(sid) != 0) order(id, 0xd, 1, 0, target, 0x64, 0x32, 0, 0x7530);
        return 1;
    }

    // 0x0053f260
    u32 think_53f260(i16* arg) {
        c_think_53f260.hit();
        const i32 id = *arg;
        const i16 sid = static_cast<i16>(id);
        const i32 target = unit_target(sid, 1);
        if (unit_6512d0(sid) != 0 || joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(target))) != 0 ||  // 0x00428b70
            joan::orig::arg::as<u32>(joan::orig::unit::idle(joan::orig::arg::i32(target))) != 0) {                                        // 0x0065c8d0
            unit_652370(sid);
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(sid), joan::orig::arg::i32(static_cast<i16>(target)))) >= 0x96) {  // 0x00653150
            order(id, 0xc, 1, 0, target, 0x64, 0x32, 0, 0x7530);
            return 1;
        }
        set_state(sid, kAttack);
        if (unit_651590(sid) != 0) {
            if (joan::orig::arg::as<u32>(joan::orig::script::in_arc(joan::orig::arg::i32(sid), joan::orig::arg::i32(static_cast<i16>(target)))) != 0) {  // 0x00651430
                const i32 r = random_590540(0x64);
                const i32 lim = joan::orig::arg::as<i16>(joan::orig::as_game(arg)->scaled_a3cc90());  // 0x00512070
                if (!(r > lim)) {
                    const i32 v = unit_652ee0(sid, target);
                    order(id, 0xa, 5, 0, v, 0, 0, 0, 0x9c4);
                    order(id, 4, 5, 0, target, 0, 0, 0, 0x3e8);
                    return 1;
                }
            }
            const i32 v = unit_652ee0(sid, target);
            order(id, 0xa, 4, 0, v, 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(arg)));  // 0x0053f670
        }
        if (unit_651550(sid) != 0) order(id, 0xc, 1, 0, target, 0x64, 0x32, 0, 0x7530);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_06", name, addr, fn)
B("AiState::think_534090", 0x00534090, &H::think_534090);
B("AiState::think_538140", 0x00538140, &H::think_538140);
B("AiState::think_53f260", 0x0053f260, &H::think_53f260);
#undef B

}  // namespace game_b106_06
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using i8 = std::int8_t;

JOAN_COUNTER(c_ai_sound_slot, "unit_ai_sound_slot_535020");
JOAN_COUNTER(c_ai_archer, "unit_ai_archer_5420a0");

struct H {
    // ------------------------------------------------------------------ 0x00535020
    static void sound_slot_block(i32 id, i32 c, i32 slot, const char* name) {
        const u32 a = joan::orig::arg::as<u32>(joan::orig::script_hardpoint_x(joan::orig::arg::i32(static_cast<i16>(c)), joan::orig::arg::i32(slot)));  // 0x00657110
        const u32 b = joan::orig::arg::as<u32>(joan::orig::script_hardpoint_y(joan::orig::arg::i32(static_cast<i16>(c)), joan::orig::arg::i32(slot)));  // 0x00657240
        const u32 d = joan::orig::arg::as<u32>(joan::orig::script_hardpoint_z(joan::orig::arg::i32(static_cast<i16>(c)), joan::orig::arg::i32(slot)));  // 0x00657370
        joan::orig::group_order(joan::orig::arg::i32(id), 7, static_cast<std::int8_t>(joan::orig::arg::i32(0xa)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(d), 0, 0x2710);         // 0x00661340
        joan::orig::script_unit_take_slot(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::i32(static_cast<i16>(c)), joan::orig::arg::str(name), joan::orig::arg::i32(global<u32>(d::g_waypoint_reach_dist)));  // 0x00657670
    }
    static i32 sound_slot_get(u8* p, i32 id, i32 c, const char* name, i32 mode) {
        i32 slot;
        if (jo::as_unit(p)->slot_index == -1) {
            slot = joan::orig::arg::as<i32>(joan::orig::seat::find_free_656dc0(joan::orig::arg::i16(static_cast<i16>(id)), joan::orig::arg::i16(static_cast<i16>(c)), joan::orig::arg::str(name)));  // 0x00656dc0
            jo::as_unit(p)->slot_index = slot;
            if (mode >= 0) jo::view<jo::TaskSlot>(jo::as_unit(p)->task).arg_9 = static_cast<u32>(mode);
        } else {
            slot = jo::as_unit(p)->slot_index;
        }
        return slot;
    }
    static void unit_reset(u8* p, i32 id) {
        jo::view<u32>(jo::as_unit(p)->weapon_slot) = 0;
        jo::view<u32>(jo::as_unit(p)->weapon) = 0;
        jo::as_unit(p)->slot_index = -1;
        joan::orig::as_unit(p)->release_slot();                    // 0x00528300
        joan::orig::as_cell_object(p)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));   // 0x0050a480
        joan::orig::unit_refresh_items(joan::orig::arg::i16(static_cast<i16>(id)));  // 0x0065b9b0
    }
    u32 ai_sound_slot(u8* p) {
        c_ai_sound_slot.hit();
        const i32 id = jo::as_unit(p)->id;
        const i32 c = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(static_cast<i16>(id)), 1));  // 0x00652190
        if (joan::orig::arg::as<i32>(joan::orig::script_unit_384(joan::orig::arg::i16(static_cast<i16>(id)))) > 0) {         // 0x00656af0
            const double r = joan::orig::arg::as<double>(joan::orig::script_target_hardpoint_dist(joan::orig::arg::i32(id)));              // 0x0065ddb0
            if (!(static_cast<double>(global<i32>(d::g_waypoint_reach_dist)) >= r)) {
                const u32 v = joan::orig::arg::as<u32>(joan::orig::script_unit_388(joan::orig::arg::i32(id)));                // 0x0065dfa0
                joan::orig::script_unit_release_slots(joan::orig::arg::i16(static_cast<i16>(c)), joan::orig::arg::i32(v));         // 0x00657cc0
                joan::orig::group_release(joan::orig::arg::i32(id));                             // 0x006618f0
                return 1;
            }
            if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(p)->weapon))) != 0) {  // 0x00428b70
                unit_reset(p, id);
            } else {
                void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(p)->weapon)));  // 0x0041b3c0
                if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0) unit_reset(p, id);                // 0x0042d0e0
            }
            joan::orig::group_release(joan::orig::arg::i32(id));  // 0x006618f0
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script_unit_38c(joan::orig::arg::i16(static_cast<i16>(c)))) != 1) return 1;  // 0x00656b50
        const char* const n6c = str(d::s_M_9d006c);
        const char* const n68 = str(d::s_F_9d0068);
        if (joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(static_cast<i16>(id)), 2)) == 0) {  // 0x00652190
            i32 slot = sound_slot_get(p, id, c, n6c, -1);
            if (slot >= 0) {
                sound_slot_block(id, c, slot, n6c);
            } else {
                slot = sound_slot_get(p, id, c, n68, 1);
                if (slot >= 0) sound_slot_block(id, c, slot, n68);
            }
        } else {
            i32 slot = sound_slot_get(p, id, c, n68, -1);
            if (slot >= 0) {
                sound_slot_block(id, c, slot, n68);
            } else {
                slot = sound_slot_get(p, id, c, n6c, 0);
                if (slot >= 0) sound_slot_block(id, c, slot, n6c);
            }
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x005420a0
    static i32 lookup7(i32 id, const char* tag, i32 a3, i32 a5, i32 a7) {
        // 0x006517c0(id, tag, a3, 0, a5, 0, a7)
        return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(static_cast<i16>(id)), tag, joan::orig::arg::i32(a3), 0, joan::orig::arg::i32(a5), 0, joan::orig::arg::i32(a7)));
    }
    static i32 res(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }  // 0x0065b280
    u32 ai_archer(u8* p) {
        c_ai_archer.hit();
        const i32 id = jo::as_unit(p)->id;
        i32 c10 = 0, c14 = 0, c18 = 0, c1c = 0;
        c1c = lookup7(id, str(d::s_T), 0, 0, 0);
        const i32 c20 = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(static_cast<i16>(id)), 1));  // 0x00652190
        const char* const tag = str(d::s_E_9cfc2c);
        c10 = lookup7(id, tag, res(id, 2), c20, 0x41);  // both branches of the original are identical
        c14 = lookup7(id, tag, res(id, 1), c20, 0x41);
        {
            u8* const cls = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::view<u16>(jo::as_unit(p)->type_id))));  // 0x0042d0a0
            if (joan::crt::stricmp(joan::orig::arg::str(static_cast<const char*>(jo::as_unit_type(cls)->name)), str(d::s_ENGLISH_EX_ARCHER_CAPTAIN)) == 0) {
                c18 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(static_cast<i16>(id)), str(d::s_EGC), joan::orig::arg::u32(res(id, 2)), 0u));  // 0x006519d0
                if (c18 == 0)
                    c18 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(static_cast<i16>(id)), str(d::s_EK), joan::orig::arg::u32(res(id, 2)), 0u));
            }
        }
        if (c14 != 0) {
            const i32 r = res(id, 1);
            joan::orig::group_order(joan::orig::arg::i32(id), 0xc, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(c14), joan::orig::arg::i32(r), 0x190, 0, 0x7530);  // 0x00661340
            return 1;
        }
        if (c10 != 0 || c18 != 0) joan::orig::script::set_mode(joan::orig::arg::i32(static_cast<i16>(id)), str(d::s_ATTACK_9cf380));  // 0x006515d0
        if (c10 == 0 && c18 == 0) joan::orig::script::set_mode(joan::orig::arg::i32(static_cast<i16>(id)), str(d::s_ACTIVE));
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(static_cast<i16>(id)))) != 0) {  // 0x00651590
            if (c10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(p)->line_of_sight(joan::orig::arg::i32(c10))) == 0) {  // 0x005130a0
                    const u32 r = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::i32(c10)));  // 0x00652ee0
                    joan::orig::group_order(joan::orig::arg::i32(id), 0xa, static_cast<std::int8_t>(joan::orig::arg::i32(3)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(r), 0, 0, 0, 0x9c4);         // 0x00661340
                    joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(p), 1, joan::orig::arg::i32(c10));                            // 0x005432a0
                    return 1;
                }
                joan::orig::script::set_mode(joan::orig::arg::i32(static_cast<i16>(id)), str(d::s_ACTIVE));  // 0x006515d0
            }
            if (c18 != 0) {
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::i32(c18)));  // 0x00653000
                joan::orig::group_order(joan::orig::arg::i32(id), 0xa, static_cast<std::int8_t>(joan::orig::arg::i32(3)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(r), 0, 0, 0, 0x9c4);         // 0x00661340
                joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(p), 2, joan::orig::arg::i32(c18));                            // 0x005432a0
                return 1;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(static_cast<i16>(id)))) == 0) return 1;  // 0x00651550
        c10 = lookup7(id, tag, res(id, 2), 0, 0x41);  // both branches identical
        if (c10 > 0) {
            const i32 r = res(id, 2);
            joan::orig::group_order(joan::orig::arg::i32(id), 0xc, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(c10), static_cast<i32>(static_cast<u32>(r) - 0x190u), 0x190, 0, 0x7530);  // 0x00661340
            return 1;
        }
        if (c1c != 0) {
            const i32 cnt = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::i32(static_cast<i16>(c1c))));  // 0x00653150
            if (!(cnt > res(id, 3))) {
                const i32 r = res(id, 2);
                joan::orig::group_order(joan::orig::arg::i32(id), 0xc, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::i32(c1c), static_cast<i32>(static_cast<u32>(r) - 0x190u), 0x190, 0, 0x7530);  // 0x00661340
                return 1;
            }
        }
        if (c10 == 0) joan::orig::group_order(joan::orig::arg::i32(id), 0xe, static_cast<std::int8_t>(joan::orig::arg::i32(0)), static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::i32(c20), 0, 0, 0, 0x7530);  // 0x00661340
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("unit_ai_sound_slot", 0x00535020, &H::ai_sound_slot);
B("unit_ai_archer", 0x005420a0, &H::ai_archer);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

JOAN_COUNTER(c_script_535640, "script_535640");
JOAN_COUNTER(c_script_540640, "script_540640");

// ------------------------------------------------------------------ 0x00535640 (dead)
static void spawn_535640(i32 id, i32 n) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(n), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0x5a})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), i32{0x145}, i32{0}, i32{0}, i32{0}, joan::orig::arg::i32(// 0x00661340
                    i32{0x1388}));
}
static void reinforce_535640(i32 id, i32 kind) {
    joan::orig::group_order(joan::orig::arg::i32(id), i32{0x16}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0x46})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(kind), i32{0}, i32{1}, i32{0}, i32{0x9c40});
    const u32 g = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{1}));  // 0x00652190
    joan::orig::script_post_event_65c530(joan::orig::arg::u32(g), i32{0});                 // 0x0065c530
    joan::orig::script::unit_528300(joan::orig::arg::i32(id));                        // 0x00652370
}

// ------------------------------------------------------------------ 0x00540640
i32 script_get(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }  // 0x0065b280
u32 find_6517c0(i32 id, img::Addr s, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) {
    return joan::orig::arg::as<u32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), img::str(s), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7)));  // 0x006517c0
}
void state_6515d0(i32 id, img::Addr s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(str(s))); }  // 0x006515d0

struct H {
    // ------------------------------------------------------------------ 0x00535640 (dead)
    u32 script_535640(void* a) {
        c_script_535640.hit();
        const i32 id = static_cast<i32>(jo::as_unit(a)->id);
        const i32 r1 = joan::orig::arg::as<i32>(joan::orig::unit::distance_to(joan::orig::arg::i16(id), i32{0x5a22}, i32{0x164c}, i32{-0x69}));  // 0x0065b800
        const i32 r2 = joan::orig::arg::as<i32>(joan::orig::unit::distance_to(joan::orig::arg::i16(id), i32{0x5997}, i32{0x1714}, i32{-0x6f}));
        if (r1 < 10 || r2 < 10) {
            if (r1 < 10) spawn_535640(id, 10);
            if (r2 < 10) spawn_535640(id, 10);
            joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{1});  // 0x00652190
            if (r1 < 10) reinforce_535640(id, 0xb);
            if (r2 < 10) reinforce_535640(id, 0xd);
        } else if (r1 < r2) {
            joan::orig::group_order(joan::orig::arg::i32(id), i32{7}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0x6e})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), i32{0x5a22}, i32{0x164c}, i32{-0x69}, i32{0}, i32{0x1388});
        } else {
            joan::orig::group_order(joan::orig::arg::i32(id), i32{7}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0x6e})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), i32{0x5997}, i32{0x1714}, i32{-0x6f}, i32{0}, i32{0x1388});
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00540640
    u32 script_540640(void* a) {
        c_script_540640.hit();
        const i32 id = static_cast<i32>(jo::as_unit(a)->id);
        u32 r10 = 0;
        u32 r14 = 0;
        u32 r18 = 0;
        const u32 r20 = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{1}));  // 0x00652190
        joan::orig::script::group_angle(joan::orig::arg::i32(r20));                          // 0x006547a0
        const u32 r1c = find_6517c0(id, d::s_T, 0, 0, 0, 0, 0);
        if (joan::orig::arg::as<i32>(joan::orig::script::leader_id(joan::orig::arg::i32(id))) > 0) {  // 0x006569f0
            r10 = find_6517c0(id, d::s_L, 0x3e8, 0, 0, 0, 0);
            if (r1c == 0) {
                joan::orig::group_order(joan::orig::arg::i32(id), i32{0xf}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{-2})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), i32{0}, i32{0}, i32{0}, i32{0}, joan::orig::arg::i32(// 0x00661340
                                i32{0x2710}));
            }
        }
        {
            const i32 v = script_get(id, 2);
            r10 = find_6517c0(id, d::s_EP, static_cast<u32>(v), 0, r20, 0, 0x41);
        }
        {
            const i32 v = script_get(id, 1);
            r14 = find_6517c0(id, d::s_EP, static_cast<u32>(v), 0, r20, 0, 0x41);
        }
        const char* nm = static_cast<const char*>(joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(a)->type_id))))) + 4;  // 0x0042d0a0
        if (joan::crt::stricmp(joan::orig::arg::str(nm), str(d::s_ENGLISH_EX_ARCHER)) == 0) {
            const i32 v = script_get(id, 2);
            r18 = joan::orig::arg::as<u32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), str(d::s_EGC), joan::orig::arg::u32(v), joan::orig::arg::u32(i32{0})));  // 0x006519d0 "EGC"
            if (r18 == 0) {
                const i32 v2 = script_get(id, 2);
                r18 = joan::orig::arg::as<u32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), str(d::s_EK), joan::orig::arg::u32(v2), joan::orig::arg::u32(i32{0})));
            }
        }
        if (r14 != 0) {
            const i32 v = script_get(id, 1);
            joan::orig::group_order(joan::orig::arg::i32(id), i32{0xc}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(r14), joan::orig::arg::i32(v), i32{0x190}, i32{0}, i32{0x7530});
            return 1;
        }
        if (r10 != 0 || r18 != 0) state_6515d0(id, d::s_ATTACK_9cf380);  // "ATTACK"
        if (r10 == 0 && r18 == 0) state_6515d0(id, d::s_ACTIVE);  // "ACTIVE"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {               // 0x00651590
            joan::orig::script::group_set_angle(joan::orig::arg::i32(r20), i32{0});              // 0x006547e0
            if (r10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(a)->line_of_sight(joan::orig::arg::i32(r10))) == 0) {  // 0x005130a0
                    const u32 t = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(r10)));  // 0x00652ee0
                    joan::orig::group_order(joan::orig::arg::i32(id), i32{0xa}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{3})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(t), i32{0}, i32{0}, i32{0}, i32{0x9c4});
                    joan::orig::as_order(this)->run_a(joan::orig::arg::mptr(a), i32{1}, joan::orig::arg::i32(r10));  // 0x00541270
                    return 1;
                }
                state_6515d0(id, d::s_ACTIVE);  // "ACTIVE"
            }
            if (r18 != 0) {
                const u32 t = joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(r18)));  // 0x00653000
                joan::orig::group_order(joan::orig::arg::i32(id), i32{0xa}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{3})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(t), i32{0}, i32{0}, i32{0}, i32{0x9c4});
                joan::orig::as_order(this)->run_a(joan::orig::arg::mptr(a), i32{2}, joan::orig::arg::i32(r18));
                return 1;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) == 0) return 1;  // 0x00651550
        {
            const i32 v = script_get(id, 2);
            r10 = find_6517c0(id, d::s_EP, static_cast<u32>(v), 1, r20, 0, 0x41);
        }
        if (static_cast<i32>(r10) > 0) {
            const i32 v = script_get(id, 2) - 0x190;
            joan::orig::group_order(joan::orig::arg::i32(id), i32{0xc}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(r10), joan::orig::arg::i32(v), i32{0x190}, i32{0}, i32{0x7530});
            return 1;
        }
        if (r1c != 0 && joan::orig::arg::as<i32>(joan::orig::script::group_contains(joan::orig::arg::i32(r20), joan::orig::arg::i32(r1c))) != 0) {  // 0x00653f30
            const i32 s = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), static_cast<i32>(static_cast<i16>(r1c))));  // 0x00653150
            if (!(s > script_get(id, 3))) {
                const i32 v = script_get(id, 2) - 0x190;
                joan::orig::group_order(joan::orig::arg::i32(id), i32{0xc}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), joan::orig::arg::i32(r1c), joan::orig::arg::i32(v), i32{0x190}, i32{0}, i32{0x7530});
                return 1;
            }
        }
        state_6515d0(id, d::s_IDLE);  // "IDLE"
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("script_535640", 0x00535640, &H::script_535640);
B("script_540640", 0x00540640, &H::script_540640);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

void* const kObjects = obj(d::g_items);

void* const kRandom = obj(d::g_misc);

const char* const kAttack = str(d::s_ATTACK_9cf380);  // "ATTACK"

// cdecl unit helpers (short id arguments read with movsx word by the callees).
u32 unit_target(i32 id, i32 k) { return joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }   // 0x00652190
i32 unit_busy(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(id))); }               // 0x006512d0
i32 target_gone(u32 t) { return joan::orig::arg::as<i32>(joan::orig::unit::idle(joan::orig::arg::i32(t))); }               // 0x0065c8d0
i32 target_alive(u32 t) { return joan::orig::arg::as<i32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(t))); }              // 0x0065b6a0
i32 dist_3060(i32 id, u32 t) { return joan::orig::arg::as<i32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }      // 0x00653060
void set_state(i32 id, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }   // 0x006515d0
u32 aim_2ee0(i32 id, u32 t) { return joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }       // 0x00652ee0
u32 aim_3000(i32 id, u32 t) { return joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }       // 0x00653000
void order(i32 id, i32 a, i32 b, i32 c, u32 d, u32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}

i32 unit_dead(u32 t) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(t)); }  // 0x00428b70
i32 obj_dead(u32 t) { return joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(t)); }  // 0x00425d50

i32 sx(u32 v) { return static_cast<i32>(static_cast<i16>(v)); }

JOAN_COUNTER(c_ai_535870, "Ai::state_535870");
JOAN_COUNTER(c_ai_535a20, "Ai::state_535a20");
JOAN_COUNTER(c_ai_53c700, "Ai::state_53c700");
JOAN_COUNTER(c_ai_53c960, "Ai::state_53c960");

struct H {
    // ------------------------------------------------------------------ 0x00535870
    u32 state_535870(i16* unit) {
        c_ai_535870.hit();
        const i32 id = static_cast<i32>(*unit);
        const u32 tgt = unit_target(id, 1);
        if (unit_busy(id) != 0 || unit_dead(tgt) != 0 || target_gone(tgt) != 0) {
            unit_stop(id);
            return 1;
        }
        if (dist_3150(id, sx(tgt)) < 0xc8) {
            set_state(id, kAttack);
        } else {
            order(id, 0xc, 1, 0, tgt, 0x64, 0x64, 0, 0x7530);
            return 1;
        }
        if (can_melee(id) != 0) {
            const u32 r = aim_2ee0(id, tgt);
            order(id, 0xa, 4, 0, r, 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_a(joan::orig::arg::mptr(unit)));  // 0x00535c20
        }
        if (can_move(id) != 0) order(id, 0xc, 1, 0, tgt, 0x64, 0x64, 0, 0x7530);
        return 1;
    }

    // ------------------------------------------------------------------ 0x00535a20
    u32 state_535a20(i16* unit) {
        c_ai_535a20.hit();
        const i32 id = static_cast<i32>(*unit);
        const u32 tgt = unit_target(id, 1);
        if (unit_busy(id) != 0) {
            unit_stop(id);
            return 1;
        }
        if (target_alive(tgt) == 0 || obj_dead(tgt) != 0) {
            unit_stop(id);
            return 1;
        }
        if (dist_3060(id, tgt) < 0x96) {
            set_state(id, kAttack);
        } else {
            order(id, 0xd, 1, 0, tgt, 0x64, 0x32, 0, 0x7530);
            return 1;
        }
        if (can_melee(id) != 0) {
            if (target_alive(tgt) == 0) {
                unit_stop(id);
                return 1;
            }
            const u32 r = aim_3000(id, tgt);
            order(id, 0xa, 4, 0, r, 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_a(joan::orig::arg::mptr(unit)));  // 0x00535c20
        }
        if (can_move(id) != 0) order(id, 0xd, 1, 0, tgt, 0x64, 0x32, 0, 0x7530);
        return 1;
    }

    // ------------------------------------------------------------------ 0x0053c700
    u32 state_53c700(i16* unit) {
        c_ai_53c700.hit();
        const i32 id = static_cast<i32>(*unit);
        const u32 tgt = unit_target(id, 1);
        if (unit_busy(id) != 0 || unit_dead(tgt) != 0 || target_gone(tgt) != 0) {
            unit_stop(id);
            return 1;
        }
        if (dist_3150(id, sx(tgt)) < 0x96) {
            set_state(id, kAttack);
        } else {
            order(id, 0xc, 1, 0, tgt, 0x64, 0x32, 0, 0x7530);
            return 1;
        }
        if (can_melee(id) != 0) {
            if (check_1430(id, sx(tgt)) != 0) {
                const i32 roll = static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::as_misc(kRandom)->random(100)));  // 0x00590540
                const i32 v = sx(joan::orig::arg::as<u32>(joan::orig::as_game(unit)->scaled_a3cc90()));                          // 0x00512070
                if (!(roll > v)) {
                    const u32 r = aim_2ee0(id, tgt);
                    order(id, 0xa, 4, 0, r, 0, 0, 0, 0x9c4);
                    order(id, 4, 4, 0, tgt, 0, 0, 0, 0x3e8);
                    return 1;
                }
            }
            const u32 r = aim_2ee0(id, tgt);
            order(id, 0xa, 4, 0, r, 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_b(joan::orig::arg::mptr(unit)));  // 0x0053cb10
        }
        if (can_move(id) != 0) order(id, 0xc, 1, 0, tgt, 0x64, 0x32, 0, 0x7530);
        return 1;
    }

    // ------------------------------------------------------------------ 0x0053c960
    u32 state_53c960(i16* unit) {
        c_ai_53c960.hit();
        const i32 id = static_cast<i32>(*unit);
        const u32 tgt = unit_target(id, 1);
        if (unit_busy(id) != 0) {
            unit_stop(id);
            return 1;
        }
        if (dist_3060(id, tgt) < 0x96) {
            set_state(id, kAttack);
        } else {
            order(id, 0xd, 1, 0, tgt, 0x64, 0x32, 0, 0x7530);
            return 1;
        }
        if (can_melee(id) != 0) {
            if (target_alive(tgt) == 0) {
                unit_stop(id);
                return 1;
            }
            const u32 r = aim_3000(id, tgt);
            order(id, 0xa, 4, 0, r, 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_b(joan::orig::arg::mptr(unit)));  // 0x0053cb10
        }
        if (can_move(id) != 0) order(id, 0xd, 1, 0, tgt, 0x64, 0x32, 0, 0x7530);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("Ai::state_535870", 0x00535870, &H::state_535870);
B("Ai::state_535a20", 0x00535a20, &H::state_535a20);
B("Ai::state_53c700", 0x0053c700, &H::state_53c700);
B("Ai::state_53c960", 0x0053c960, &H::state_53c960);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b101_08.cpp (game/b101_08)
namespace {
namespace game_b101_08 {

void* const kTokens = obj(d::g_misc);  // also the random generator object

i32 random(i32 n) { return joan::orig::as_misc(kTokens)->random(joan::orig::arg::i32(n)); }  // 0x00590540
i32 sym(const char* s) { return joan::orig::arg::as<i32>(joan::orig::script_rgb_component(joan::orig::arg::str(s), i32{1})); }  // 0x006511f0
i32 order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    return joan::orig::arg::as<i32>(joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));  // 0x00661340
}

// Formation order on the symbol s ("A" / "AA" / "B"), then 0x00661340(id, 6, 1, 0, 3) on a
// rand(100) < 90 roll (shared by 0x00535c20 / 0x0053cb10).
void flee_order(i32 id, const char* s) {
    const i32 v = sym(s);
    order(id, 1, 2, 0, v, 0, 0, 0, 0x5dc);
    if (random(100) < 0x5a) order(id, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
}

// 0x00535c20 (n = 3, third = 0) / 0x0053cb10 (n = 5, third = "B").
u32 flee_common(void* unit, i32 n, const char* third) {
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_kind_510f80()) != 0) return 0;  // 0x00510f80
    const i32 id = static_cast<i32>(jo::as_unit(unit)->id);
    const i32 r = random(100);
    if (r < static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->scaled_2df()))) {      // 0x00511f80
        jo::view<u16>(jo::as_unit(unit)->attack_chance) = joan::orig::arg::as<u16>(joan::orig::as_unit(unit)->recruit_delay());     // 0x00511e70
        const i32 k = random(n);
        if (k == 0)
            flee_order(id, str(d::s_A));  // "A"
        else if (k <= 2)
            flee_order(id, str(d::s_AA));  // "AA"
        else if (third != nullptr)
            flee_order(id, third);
        return 1;
    }
    if (r >= 0x5a) {
        const i32 k = random(3);
        order(id, 6, 2, 0, static_cast<i32>(static_cast<u32>(k) + 1u), 0, 0, 0, 0x5dc);
    }
    return 1;
}

JOAN_COUNTER(c_flee_a, "Order::flee_a");
JOAN_COUNTER(c_flee_b, "Order::flee_b");

struct H {
    // ------------------------------------------------------------------ 0x00535c20
    u32 flee_a(void* unit) {
        c_flee_a.hit();
        return flee_common(unit, 3, nullptr);
    }

    // ------------------------------------------------------------------ 0x0053cb10
    u32 flee_b(void* unit) {
        c_flee_b.hit();
        return flee_common(unit, 5, str(d::s_B));  // "B"
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_08", name, addr, fn)
B("Order::flee_a", 0x00535c20, &H::flee_a);
B("Order::flee_b", 0x0053cb10, &H::flee_b);
#undef B

}  // namespace game_b101_08
}  // namespace

// ---------------------------------------- from modules/game_b104_07.cpp (game/b104_07)
namespace {
namespace game_b104_07 {

void* const kRandom = obj(d::g_misc);

const char* const kAttack = str(d::s_ATTACK_9cf380);  // "ATTACK"
const char* const kIdle = str(d::s_IDLE);    // "IDLE"
const char* const kActive = str(d::s_ACTIVE);  // "ACTIVE"
const char* const kEnemy = str(d::s_E_9cfc2c);

i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540

i32 unit_target(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }   // 0x00652190
void set_state(i32 id, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }   // 0x006515d0
void set_target_mode(i32 id, const char* s) { joan::orig::script::set_target_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }  // 0x00651690
i32 aim_2ee0(i32 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }       // 0x00652ee0
void order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}

i32 sx(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

// Shared tail of 0x00535e30 / 0x0053cda0 / 0x0053fa60 from the "movable" test (0x00535f82).
u32 state_move_tail(i32 id) {
    if (can_move(id) == 0) return 1;
    set_target_mode(id, kNearestHigh);
    const i32 r1 = range_b280(id, 1);
    const i32 t = nearest(id, kEnemy, r1, 1, 0, 0, 0);
    const i32 r2 = range_b280(id, 1);
    const i32 t2 = nearest(id, kEnemy, r2 / 2, 0, 0, 0, 0);
    if (t2 > 0) {
        u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(t2)));  // 0x0041b3c0
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->type_id))));  // 0x0042d0a0
        const i32 kind = static_cast<i32>(jo::self<jo::UnitType>(rec)->unit_class);
        if (kind != 5 && kind != 6) {
            const i32 r = random(10);
            if (r < 1) {
                order(id, 0xc, 1, 0, t, 0x3c, 0x3c, 0, 0x7530);
            } else if (r < 5) {
                order(id, 0xa, 1, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
                order(id, 6, 1, 0, 1, 0, 0, 0, 0x5dc);
            } else if (r < 9) {
                order(id, 0xa, 1, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
                order(id, 6, 1, 0, 2, 0, 0, 0, 0x5dc);
            } else if (r < 10) {
                order(id, 0xa, 1, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
                order(id, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
            }
            return 1;
        }
    }
    if (t > 0) {
        order(id, 0xc, 1, 0, t, 0x3c, 0x3c, 0, 0x7530);
        return 1;
    }
    const i32 a = unit_target(id, 1);
    const i32 b = unit_target(id, 2);
    const i32 c = unit_target(id, 3);
    order(id, 7, 0, 0, a, b, c, 0, 0x7530);
    if (dist_b800(id, a, b, c) <= 0xc8) unit_stop(id);
    set_state(id, kIdle);
    return 1;
}

// Shared head of 0x00535e30 / 0x0053cda0 / 0x0053fa60 (up to 0x00535f2b): returns the target.
i32 state_pick_target(i32 id) {
    nearest(id, kAny, 0, 0, 0, 0, 0);  // stored to a dead local
    i32 t = nearest(id, kEnemy, 0x96, 0, 0, 0, 0);
    if (t != 0) {
        set_state(id, kAttack);
    } else {
        set_state(id, kActive);
        t = nearest(id, kEnemy, 0xc8, 0, 0, 0, 0);
        const i32 r = random(0x32);
        if (t != 0 && r == 0) set_state(id, kAttack);
    }
    return t;
}

JOAN_COUNTER(c_state_535e30, "Ai::state_535e30");
JOAN_COUNTER(c_state_53cda0, "Ai::state_53cda0");
JOAN_COUNTER(c_state_53fa60, "Ai::state_53fa60");

struct H {
    // ------------------------------------------------------------------ 0x00535e30
    u32 state_535e30(i16* unit) {
        c_state_535e30.hit();
        const i32 id = static_cast<i32>(*unit);
        const i32 t = state_pick_target(id);
        if (can_melee(id) != 0 && t != 0) {
            order(id, 0xa, 3, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_a(joan::orig::arg::mptr(unit)));  // 0x00535c20
        }
        return state_move_tail(id);
    }

    // ------------------------------------------------------------------ 0x0053cda0 / 0x0053fa60
    u32 state_53cda0_body(i16* unit) {
        const i32 id = static_cast<i32>(*unit);
        const i32 t = state_pick_target(id);
        if (can_melee(id) != 0 && t != 0) {
            if (check_1430(id, sx(t)) != 0) {
                const i32 r = random(0x64);
                const i32 m = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90()));  // 0x00512070
                if (r <= m) {
                    order(id, 0xa, 4, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
                    order(id, 4, 4, 0, t, 0, 0, 0, 0x3e8);
                    return 1;
                }
            }
            order(id, 0xa, 3, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_b(joan::orig::arg::mptr(unit)));  // 0x0053cb10
        }
        return state_move_tail(id);
    }
    u32 state_53cda0(i16* unit) {
        c_state_53cda0.hit();
        return state_53cda0_body(unit);
    }
    u32 state_53fa60(i16* unit) {
        c_state_53fa60.hit();
        return state_53cda0_body(unit);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_07", name, addr, fn)
B("Ai::state_535e30", 0x00535e30, &H::state_535e30);
B("Ai::state_53cda0", 0x0053cda0, &H::state_53cda0);
B("Ai::state_53fa60", 0x0053fa60, &H::state_53fa60);
#undef B

}  // namespace game_b104_07
}  // namespace

// ---------------------------------------- from modules/game_b106_14.cpp (game/b106_14)
namespace {
namespace game_b106_14 {

JOAN_COUNTER(c_pick_target, "AiTask::pick_attack_target");

// ---------------------------------------------------------------- 0x00536390 helpers
i32 query(i16 id, const char* key, i32 a, i32 b, i32 c, i32 d, i32 e) {
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), key, joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));  // 0x006517c0
}
const char* rs(img::Addr a) { return &img::global<const char>(a); }
void order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}
i32 ai_get(i32 id, i32 what) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(what))); }  // 0x0065b280

struct H {
    // 0x00536390
    u32 pick_attack_target(void* task) {
        c_pick_target.hit();
        const i16 id = jo::as_unit(task)->id;
        const i32 idl = id;
        i32 near = 0;   // [ebp-0x10]
        const i32 z = 0;  // [ebp-0x18]: never written
        const i32 high = query(id, rs(d::s_T), 0, 0, 0, 0, 0);  // [ebp-0x1c]
        near = query(id, rs(d::s_E_9cfc2c), 0x96, 0, 0, 0, 0);
        if (near != 0 || z != 0) joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(rs(d::s_ATTACK_9cf380)));  // 0x006515d0 "ATTACK"
        if (near == 0 && z == 0) {
            joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(rs(d::s_ACTIVE)));  // "ACTIVE"
            near = query(id, rs(d::s_E_9cfc2c), 0xc8, 0, 0, 0, 0);
            const i32 busy = joan::orig::as_misc(obj(d::g_misc))->random(0x32);  // 0x00590540
            if (near != 0 && busy == 0) joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(rs(d::s_ATTACK_9cf380)));  // "ATTACK"
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {  // 0x00651590
            if (near != 0) {
                const i32 t = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(near)));  // 0x00652ee0
                order(idl, 0xa, 3, 0, t, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(task)));  // 0x00538340
            }
            if (z != 0) {
                const i32 t = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(z)));  // 0x00653000
                order(idl, 0xa, 3, 0, t, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(task)));  // 0x00538340
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) == 0) return 1;  // 0x00651550
        joan::orig::script::set_target_mode(joan::orig::arg::i32(id), joan::orig::arg::str(rs(d::s_NEAREST_HIGH_PRIORITY)));     // 0x00651690 "NEAREST_HIGH_PRIORITY"
        if (joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), 1)) != 0) {      // 0x00652190
            near = query(id, rs(d::s_E_9cfc2c), ai_get(idl, 1) / 2, 1, 0, 0, 0);
        } else {
            near = query(id, rs(d::s_E_9cfc2c), ai_get(idl, 1), 1, 0, 0, 0);
        }
        const i32 cand = query(id, rs(d::s_E_9cfc2c), ai_get(idl, 1) / 2, 0, 0, 0, 0);  // [ebp-0x14]
        if (cand > 0) {
            void* unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(cand));  // 0x0041b3c0
            void* o = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
            if (static_cast<i32>(jo::self<jo::UnitType>(o)->unit_class) != 5 && static_cast<i32>(jo::self<jo::UnitType>(o)->unit_class) != 6) {
                const i32 cnt = joan::orig::as_misc(obj(d::g_misc))->random(0xa);  // 0x00590540
                if (cnt < 1) {
                    order(idl, 0xc, 1, 0, near, 0x3c, 0x3c, 0, 0x7530);
                } else if (cnt < 5) {
                    order(idl, 0xa, 1, 0, joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(near))), 0, 0, 0, 0x9c4);
                    order(idl, 6, 1, 0, 1, 0, 0, 0, 0x5dc);
                } else if (cnt < 9) {
                    order(idl, 0xa, 1, 0, joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(near))), 0, 0, 0, 0x9c4);
                    order(idl, 6, 1, 0, 2, 0, 0, 0, 0x5dc);
                } else if (cnt < 10) {
                    order(idl, 0xa, 1, 0, joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(near))), 0, 0, 0, 0x9c4);
                    order(idl, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
                }
                return 1;
            }
        }
        if (near > 0) {
            order(idl, 0xc, 1, 0, near, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), 1)) != 0) return 1;  // 0x00652190
        joan::orig::script::set_target_mode(joan::orig::arg::i32(id), joan::orig::arg::str(rs(d::s_NEAREST_HIGH_PRIORITY)));       // "NEAREST_HIGH_PRIORITY"
        if (high != 0) {
            near = query(id, rs(d::s_E_9cfc2c), ai_get(idl, 2), 1, 0, 0, 0);
        } else {
            near = query(id, rs(d::s_E_9cfc2c), ai_get(idl, 2), 1, 0, 0x87, 0);
        }
        if (near > 0) {
            order(idl, 0xc, 1, 0, near, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (high != 0) {
            const i32 dist = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), joan::orig::arg::i32(static_cast<i16>(high))));  // 0x00653150
            if (!(dist > ai_get(idl, 3))) order(idl, 0xc, 1, 1, high, 0x3c, 0x3c, 0, 0x7530);
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_14", name, addr, fn)
B("AiTask::pick_attack_target", 0x00536390, &H::pick_attack_target);
#undef B

}  // namespace game_b106_14
}  // namespace

// ---------------------------------------- from modules/game_b106_16.cpp (game/b106_16)
namespace {
namespace game_b106_16 {

JOAN_COUNTER(c_decide_536a40, "AI::decide_536a40");

struct H {
    // 0x00536a40
    u32 decide_536a40(i16* unit_id) {
        c_decide_536a40.hit();
        const i32 v8 = *unit_id;
        const i16 w = static_cast<i16>(v8);
        const img::Addr kE = d::s_E_9cfc2c;  // "E"
        const i32 v18 = 0;           // never written by the original
        i32 v10 = 0, vc = 0;
        auto query = [&](img::Addr s, i32 a3, i32 a4, i32 a5, i32 a6, i32 a7) -> i32 {
            return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(w), img::str(s), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7)));  // 0x006517c0
        };
        auto set_state = [&](img::Addr s) { joan::orig::script::set_mode(joan::orig::arg::i32(w), joan::orig::arg::str(img::addr(s))); };  // 0x006515d0
        auto order = [&](i32 a2, i32 a3, i32 a4, i32 a5, i32 a6, i32 a7, i32 a8, i32 a9) {
            joan::orig::group_order(joan::orig::arg::i32(v8), joan::orig::arg::i32(a2), static_cast<std::int8_t>(joan::orig::arg::i32(a3)), static_cast<std::int8_t>(joan::orig::arg::i32(a4)), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9));  // 0x00661340
        };
        auto count = [&](i32 k) -> i32 { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(v8), joan::orig::arg::i32(k))); };  // 0x0065b280
        auto rnd = [&](i32 n) -> i32 { return joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(n)); };  // 0x00590540

        const i32 v1c = query(d::s_T, 0, 0, 0, 0, 0);  // "T"
        v10 = query(kE, 0x96, 0, 0, 0, 0);
        if (v10 != 0 || v18 != 0) set_state(d::s_ATTACK_9cf380);  // ATTACK
        if (v10 == 0 && v18 == 0) {
            set_state(d::s_ACTIVE);  // ACTIVE
            v10 = query(kE, 0xc8, 0, 0, 0, 0);
            vc = rnd(0x32);
            if (v10 != 0 && vc == 0) set_state(d::s_ATTACK_9cf380);  // ATTACK
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(w))) != 0) {  // 0x00651590
            if (v10 != 0) {
                const i32 r = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(v10)));  // 0x00652ee0
                order(0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(unit_id)));  // 0x00538340
            }
            if (v18 != 0) {
                const i32 r = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(w), joan::orig::arg::i32(v18)));  // 0x00653000
                order(0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(unit_id)));  // 0x00538340
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(w))) == 0) return 1;  // 0x00651550
        joan::orig::script::set_target_mode(joan::orig::arg::i32(w), joan::orig::arg::str(img::addr(d::s_NEAREST_HIGH_PRIORITY)));  // 0x00651690 NEAREST_HIGH_PRIORITY
        {
            const i32 c = count(1);
            v10 = query(kE, c, 1, 0, 0, 0);
        }
        i32 v14;
        {
            const i32 c = count(1);
            v14 = query(kE, c / 2, 0, 0, 0, 0);
        }
        if (v14 > 0) {
            const u8* const u = joan::orig::arg::as<const u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(v14)));  // 0x0041b3c0
            const void* const v20 = joan::orig::arg::as<const void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));  // 0x0042d0a0
            if (jo::self<jo::UnitType>(v20)->unit_class != 5 && jo::self<jo::UnitType>(v20)->unit_class != 6) {
                const i32 v24 = rnd(0xa);
                if (v24 < 1) {
                    order(0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
                } else if (v24 < 5 || v24 < 9 || v24 < 10) {
                    const i32 kind = v24 < 5 ? 1 : (v24 < 9 ? 2 : 3);
                    const i32 r = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(v10)));  // 0x00652ee0
                    order(0xa, 1, 0, r, 0, 0, 0, 0x9c4);
                    order(6, 1, 0, kind, 0, 0, 0, 0x5dc);
                }
                return 1;
            }
        }
        if (v10 > 0) {
            order(0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        joan::orig::script::set_target_mode(joan::orig::arg::i32(w), joan::orig::arg::str(img::addr(d::s_NEAREST_HIGH_PRIORITY)));  // 0x00651690 NEAREST_HIGH_PRIORITY
        if (v1c != 0) {
            const i32 c = count(2);
            v10 = query(kE, c, 1, 0, 0, 0);
        } else {
            const i32 c = count(2);
            v10 = query(kE, c, 1, 0, 0x87, 0);
        }
        if (v10 > 0) {
            order(0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (v1c != 0) {
            const i32 e = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(w), joan::orig::arg::i32(static_cast<i16>(v1c))));  // 0x00653150
            const i32 c = count(3);
            if (!(e > c)) {
                order(0xc, 1, 1, v1c, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        set_state(d::s_IDLE);  // IDLE
        const i32 r = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(w), 1));  // 0x00652190
        order(0xe, 0, 1, r, 0, 0, 0, 0x7530);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_16", name, addr, fn)
B("AI::decide_536a40", 0x00536a40, &H::decide_536a40);
#undef B

}  // namespace game_b106_16
}  // namespace

// ---------------------------------------- from modules/game_b106_15.cpp (game/b106_15)
namespace {
namespace game_b106_15 {

JOAN_COUNTER(c_think_a, "UnitAi::think_attack_a");
JOAN_COUNTER(c_think_b, "UnitAi::think_attack_b");

// ---- shared helpers -----------------------------------------------------------------------
void* const kRandom = obj(d::g_misc);
i32 random_below(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
u8* unit_type(i16 type) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(type))); }  // 0x0042d0a0

// AI script helpers (cdecl).
i32 ai_get(i16 u, i32 what) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(u), joan::orig::arg::i32(what))); }  // 0x00652190
i32 ai_find(i16 u, const char* what, i32 a, i32 b, i32 c, i32 d, i32 e) {      // 0x006517c0
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(u), what, joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));
}
void ai_set_state(i16 u, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(u), joan::orig::arg::str(s)); }       // 0x006515d0
void ai_set_search(i16 u, const char* s) { joan::orig::script::set_target_mode(joan::orig::arg::i32(u), joan::orig::arg::str(s)); }      // 0x00651690
i32 ai_range(i32 u, i32 kind) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(u), joan::orig::arg::i32(kind))); }  // 0x0065b280
i32 ai_order(i32 u, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {   // 0x00661340
    return joan::orig::arg::as<i32>(joan::orig::group_order(joan::orig::arg::i32(u), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));
}
i32 ai_attack_kind(i16 u, i32 target) { return joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(u), joan::orig::arg::i32(target))); }  // 0x00652ee0
i32 ai_attack_kind_b(i16 u, i32 target) { return joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(u), joan::orig::arg::i32(target))); }  // 0x00653000

constexpr img::Addr kTargetName = d::s_EP;
constexpr img::Addr kNearest = d::s_NEAREST_HIGH_PRIORITY;  // "NEAREST_HIGH_PRIORITY"
constexpr img::Addr kAttack = d::s_ATTACK_9cf380;   // "ATTACK"
constexpr img::Addr kActive = d::s_ACTIVE;   // "ACTIVE"
constexpr img::Addr kIdle = d::s_IDLE;     // "IDLE"

// The "half range" target block shared by 0x005370d0 (0x00537336..0x5374c3) and 0x0053e9d0
// (0x0053edc3..0x53ef50). Returns true when the function returns 1 there.
bool ai_half_range_block(i32 u, i32 s10, i32 s14) {
    if (!(s14 > 0)) return false;
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(s14)));  // 0x0041b3c0
    u8* const type = unit_type(jo::as_unit(unit)->type_id);
    if (static_cast<i32>(jo::self<jo::UnitType>(type)->unit_class) == 5) return false;
    if (static_cast<i32>(jo::self<jo::UnitType>(type)->unit_class) == 6) return false;
    const i32 r = random_below(10);
    if (r < 1) {
        ai_order(u, 0xc, 1, 0, s10, 0x3c, 0x3c, 0, 0x7530);
    } else if (r < 5) {
        ai_order(u, 0xa, 1, 0, ai_attack_kind(static_cast<i16>(u), s10), 0, 0, 0, 0x9c4);
        ai_order(u, 6, 1, 0, 1, 0, 0, 0, 0x5dc);
    } else if (r < 9) {
        ai_order(u, 0xa, 1, 0, ai_attack_kind(static_cast<i16>(u), s10), 0, 0, 0, 0x9c4);
        ai_order(u, 6, 1, 0, 2, 0, 0, 0, 0x5dc);
    } else if (r < 10) {
        ai_order(u, 0xa, 1, 0, ai_attack_kind(static_cast<i16>(u), s10), 0, 0, 0, 0x9c4);
        ai_order(u, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
    }
    return true;
}

struct H {
    // ---- 0x005370d0 -------------------------------------------------------------------
    i32 think_attack_a(i16* unit) {
        c_think_a.hit();
        const i32 u = *unit;
        const i16 uw = static_cast<i16>(u);
        i32 s10 = 0, s14 = 0;
        const i32 s18 = 0;
        i32 s1c = 0;
        const i32 leader = ai_get(uw, 1);
        joan::orig::script::group_angle(joan::orig::arg::i32(leader));  // 0x006547a0
        s1c = ai_find(uw, str(d::s_T), 0, 0, 0, 0, 0);
        s10 = ai_find(uw, str(kTargetName), 0x96, 0, leader, 0, 0);
        if (s10 != 0 || s18 != 0) ai_set_state(uw, str(kAttack));
        if (s10 == 0 && s18 == 0) {
            ai_set_state(uw, str(kActive));
            s10 = ai_find(uw, str(kTargetName), 0xc8, 0, leader, 0, 0);
            const i32 c = random_below(0x32);
            if (s10 != 0 && c == 0) ai_set_state(uw, str(kAttack));
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(uw))) != 0) {  // 0x00651590
            joan::orig::script::group_set_angle(joan::orig::arg::i32(leader), 0);   // 0x006547e0
            if (s10 != 0) {
                ai_order(u, 0xa, 4, 0, ai_attack_kind(uw, s10), 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<i32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(unit)));  // 0x00538340
            }
            if (s18 != 0) {
                ai_order(u, 0xa, 3, 0, ai_attack_kind_b(uw, s18), 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<i32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(unit)));
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(uw))) == 0) return 1;  // 0x00651550
        ai_set_search(uw, str(kNearest));
        s10 = ai_find(uw, str(kTargetName), ai_range(u, 1), 1, leader, 0, 0);
        s14 = ai_find(uw, str(kTargetName), ai_range(u, 1) / 2, 0, leader, 0, 0);
        if (ai_half_range_block(u, s10, s14)) return 1;
        if (s10 > 0) {
            ai_order(u, 0xc, 1, 0, s10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        ai_set_search(uw, str(kNearest));
        if (s1c != 0) s10 = ai_find(uw, str(kTargetName), ai_range(u, 2), 1, leader, 0, 0);
        else s10 = ai_find(uw, str(kTargetName), ai_range(u, 2), 1, leader, 0x87, 0);
        if (s10 > 0) {
            ai_order(u, 0xc, 1, 1, s10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (s1c != 0 && joan::orig::arg::as<i32>(joan::orig::script::group_contains(joan::orig::arg::i32(leader), joan::orig::arg::i32(s1c))) != 0) {  // 0x00653f30
            const i32 d = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(uw), joan::orig::arg::i32(static_cast<i16>(s1c))));  // 0x00653150
            if (!(d > ai_range(u, 3))) {
                ai_order(u, 0xc, 1, 1, s1c, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        ai_set_state(uw, str(kIdle));
        ai_order(u, 0xe, 0, 1, leader, 0, 0, 0, 0x7530);
        return 1;
    }

    // ---- 0x0053e9d0 -------------------------------------------------------------------
    i32 think_attack_b(i16* unit) {
        c_think_b.hit();
        const i32 u = *unit;
        const i16 uw = static_cast<i16>(u);
        i32 s10 = 0, s14 = 0;
        const i32 s18 = 0;
        i32 s1c = 0, s20 = 0;
        const i32 leader = ai_get(uw, 1);  // [ebp-0x24]
        i32 leash = ai_get(uw, 2);         // [ebp-0x28]
        if (leash == 0) leash = 0xb4;
        i32 dist;  // [ebp-0x2c]
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(leader)) == 0) {  // 0x00428b70
            u8* const l = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(leader)));  // 0x0041b3c0
            const float dx = jo::view<float[3]>(jo::as_unit(unit)->position_)[0] - jo::view<float[3]>(jo::as_unit(l)->position_)[0];
            const float dy = jo::view<float[3]>(jo::as_unit(unit)->position_)[1] - jo::view<float[3]>(jo::as_unit(l)->position_)[1];
            const float sum = dx * dx + dy * dy;
            dist = joan::hook::x87::ftol(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sum)))));
        } else {
            dist = 0;
        }
        const i32 radius = joan::orig::arg::as<i32>(joan::orig::unit::rank_value(joan::orig::arg::i16(static_cast<i16>(leader))));  // 0x006592c0
        if (dist > radius + 0x96) {
            ai_order(u, 0x10, 0, 1, leader, leash, radius, 0, 0x1388);
            joan::orig::script::set_2d7(joan::orig::arg::i32(uw), joan::orig::arg::i32(static_cast<i16>(0)));  // 0x006558d0
            return 1;
        }
        s1c = ai_find(uw, str(d::s_T), 0, 0, 0, 0, 0);
        s10 = ai_find(uw, str(d::s_E_9cfc2c), 0x64, 0, 0, 0, 0);
        if (s10 != 0 || s18 != 0) ai_set_state(uw, str(kAttack));
        if (s10 == 0 && s18 == 0) {
            ai_set_state(uw, str(kActive));
            s10 = ai_find(uw, str(d::s_E_9cfc2c), 0x96, 0, 0, 0, 0);
            const i32 c = random_below(0x32);
            if (s10 != 0 && c == 0) ai_set_state(uw, str(kAttack));
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(uw))) != 0) {  // 0x00651590
            if (s10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::script::in_arc(joan::orig::arg::i32(uw), joan::orig::arg::i32(static_cast<i16>(s10)))) != 0) {  // 0x00651430
                    const i32 r = random_below(0x64);
                    const i32 limit = joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90());  // 0x00512070
                    if (!(r > limit)) {
                        ai_order(u, 0xa, 4, 0, ai_attack_kind(uw, s10), 0, 0, 0, 0x9c4);
                        ai_order(u, 4, 4, 0, s10, 0, 0, 0, 0x3e8);
                        return 1;
                    }
                }
                ai_order(u, 0xa, 3, 0, ai_attack_kind(uw, s10), 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<i32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(unit)));  // 0x0053f670
            }
            if (s18 != 0) {
                ai_order(u, 0xa, 3, 0, ai_attack_kind_b(uw, s18), 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<i32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(unit)));
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(uw))) == 0) return 1;  // 0x00651550
        if (leader != 0) s20 = ai_find(static_cast<i16>(leader), str(d::s_T), 0, 0, 0, 0, 0);
        ai_set_search(uw, str(kNearest));
        s10 = ai_find(uw, str(d::s_E_9cfc2c), ai_range(u, 1), 1, 0, 0, 0);
        s14 = ai_find(uw, str(d::s_E_9cfc2c), ai_range(u, 1) / 2, 0, 0, 0, 0);
        if (ai_half_range_block(u, s10, s14)) return 1;
        if (s10 > 0) {
            ai_order(u, 0xc, 1, 0, s10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (s20 != 0) {
            ai_set_search(uw, str(kNearest));
            if (s1c != 0) s10 = ai_find(uw, str(d::s_E_9cfc2c), ai_range(u, 2), 0, 0, 0, 0);
            else s10 = ai_find(uw, str(d::s_E_9cfc2c), ai_range(u, 2), 0, 0, 0, 0);
            if (s10 > 0) {
                ai_order(u, 0xc, 1, 1, s10, 0x46, 0x46, 0, 0x7530);
                return 1;
            }
            if (s1c != 0) {
                const i32 d = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(uw), joan::orig::arg::i32(static_cast<i16>(s1c))));  // 0x00653150
                if (!(d > ai_range(u, 3))) {
                    ai_order(u, 0xc, 1, 1, s1c, 0x46, 0x46, 0, 0x7530);
                    return 1;
                }
            }
        }
        ai_set_state(uw, str(kIdle));
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_15", name, addr, fn)
B("UnitAi::think_attack_a", 0x005370d0, &H::think_attack_a);
B("UnitAi::think_attack_b", 0x0053e9d0, &H::think_attack_b);
#undef B

}  // namespace game_b106_15
}  // namespace

// ---------------------------------------- from modules/game_b106_20.cpp (game/b106_20)
namespace {
namespace game_b106_20 {

void* const kRandom = obj(d::g_misc);

JOAN_COUNTER(c_state_5377b0, "Ai::state_5377b0");

struct H {
    // ------------------------------------------------------------------ 0x005377b0
    u32 state_5377b0(const i16* unit) {
        c_state_5377b0.hit();
        const i32 id = *unit;  // [ebp-0x8] (movsx)
        auto nearest = [](i32 who, img::Addr kind, i32 range, i32 a, i32 b, i32 c, i32 d) {
            return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(who), str(kind), joan::orig::arg::i32(range), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d)));  // 0x006517c0
        };
        auto unit_target = [](i32 who, i32 k) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(who), joan::orig::arg::i32(k))); };  // 0x00652190
        auto set_state = [](i32 who, img::Addr s) { joan::orig::script::set_mode(joan::orig::arg::i32(who), joan::orig::arg::str(str(s))); };  // 0x006515d0
        auto set_target_mode = [](i32 who, img::Addr s) { joan::orig::script::set_target_mode(joan::orig::arg::i32(who), joan::orig::arg::str(str(s))); };  // 0x00651690
        auto range_b280 = [](i32 who, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(who), joan::orig::arg::i32(k))); };  // 0x0065b280
        auto aim_2ee0 = [](i32 who, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(who), joan::orig::arg::i32(t))); };     // 0x00652ee0
        auto order = [](i32 who, i32 a, i32 b, i32 c, i32 d, i32 e, i32 g, i32 h, i32 k) {
            joan::orig::group_order(joan::orig::arg::i32(who), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(g), joan::orig::arg::i32(h), joan::orig::arg::i32(k));  // 0x00661340
        };
        auto random = [](i32 m) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(m)); };  // 0x00590540
        constexpr img::Addr kAnyAddr = d::s_T, kEnemy = d::s_E_9cfc2c;
        constexpr img::Addr kAttack = d::s_ATTACK_9cf380, kActive = d::s_ACTIVE, kIdle = d::s_IDLE,
                                 kNearestHighAddr = d::s_NEAREST_HIGH_PRIORITY;

        const i32 t24 = unit_target(id, 1);
        i32 t28 = unit_target(id, 2);
        if (t28 == 0) t28 = 0xb4;
        i32 d2c;
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(t24)) == 0) {  // 0x00428b70
            const u8* const tu = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(t24)));  // 0x0041b3c0
            const u8* const me = reinterpret_cast<const u8*>(unit);
            const float dx = jo::view<float[3]>(jo::as_unit(me)->position_)[0] - jo::view<float[3]>(jo::as_unit(tu)->position_)[0];
            const float dy = jo::view<float[3]>(jo::as_unit(me)->position_)[1] - jo::view<float[3]>(jo::as_unit(tu)->position_)[1];
            const float sum = dx * dx + dy * dy;
            d2c = joan::hook::x87::ftol(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sum)))));  // sqrt, _ftol
        } else {
            d2c = 0;
        }
        const i32 r34 = joan::orig::arg::as<i32>(joan::orig::unit::rank_value(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(t24)))));  // 0x006592c0
        if (d2c > static_cast<i32>(static_cast<u32>(r34) + 0xc8u)) {
            order(id, 0x10, 0, 1, t24, t28, r34, 0, 0x1388);
            joan::orig::script::set_2d7(joan::orig::arg::i32(id), i32{0});  // 0x006558d0
            return 1;
        }
        const i32 e1c = nearest(id, kAnyAddr, 0, 0, 0, 0, 0);
        i32 e10 = nearest(id, kEnemy, 0x96, 0, 0, 0, 0);
        if (e10 != 0) set_state(id, kAttack);  // || [ebp-0x18] (always 0)
        if (e10 == 0) {
            set_state(id, kActive);
            e10 = nearest(id, kEnemy, 0xc8, 0, 0, 0, 0);
            const i32 rr = random(0x32);
            if (e10 != 0 && rr == 0) set_state(id, kAttack);
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {  // 0x00651590
            if (e10 != 0) {
                order(id, 0xa, 3, 0, aim_2ee0(id, e10), 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(unit)));  // 0x00538340
            }
            return 1;  // [ebp-0x18] == 0: the 0x00653000 branch is dead
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) == 0) return 1;  // 0x00651550
        i32 e20 = 0;
        if (t24 != 0) e20 = nearest(static_cast<i16>(t24), kAnyAddr, 0, 0, 0, 0, 0);
        set_target_mode(id, kNearestHighAddr);
        const i32 ra = range_b280(id, 1);
        e10 = nearest(id, kEnemy, ra, 1, 0, 0, 0);
        const i32 rb = range_b280(id, 1);
        const i32 e14 = nearest(id, kEnemy, rb / 2, 0, 0, 0, 0);
        if (e14 > 0) {
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(e14)));  // 0x0041b3c0
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->type_id))));  // 0x0042d0a0
            const i32 kind = jo::self<jo::UnitType>(rec)->unit_class;
            if (kind != 5 && kind != 6) {
                const i32 r = random(10);
                if (r < 1) {
                    order(id, 0xc, 1, 0, e10, 0x3c, 0x3c, 0, 0x7530);
                } else if (r < 5) {
                    order(id, 0xa, 1, 0, aim_2ee0(id, e10), 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 1, 0, 0, 0, 0x5dc);
                } else if (r < 9) {
                    order(id, 0xa, 1, 0, aim_2ee0(id, e10), 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 2, 0, 0, 0, 0x5dc);
                } else if (r < 10) {
                    order(id, 0xa, 1, 0, aim_2ee0(id, e10), 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
                }
                return 1;
            }
        }
        if (e10 > 0) {
            order(id, 0xc, 1, 0, e10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (e20 != 0) {
            set_target_mode(id, kNearestHighAddr);
            if (e1c != 0) {
                e10 = nearest(id, kEnemy, range_b280(id, 2), 0, 0, 0, 0);
            } else {
                e10 = nearest(id, kEnemy, range_b280(id, 2), 0, 0, 0x87, 0);
            }
            if (e10 > 0) {
                order(id, 0xc, 1, 1, e10, 0x46, 0x46, 0, 0x7530);
                return 1;
            }
            if (e1c != 0) {
                const i32 dd = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), static_cast<i32>(static_cast<i16>(e1c))));  // 0x00653150
                if (!(dd > range_b280(id, 3))) {
                    order(id, 0xc, 1, 1, e1c, 0x46, 0x46, 0, 0x7530);
                    return 1;
                }
            }
        }
        set_state(id, kIdle);
        return 1;
    }
};

JOAN_REPLACE("game/b106_20", "Ai::state_5377b0", 0x005377b0, &H::state_5377b0);

}  // namespace game_b106_20
}  // namespace

// ---------------------------------------- from modules/game_b106_05.cpp (game/b106_05)
namespace {
namespace game_b106_05 {

JOAN_COUNTER(c_attack, "Unit::script_attack");

struct H {
    // ------------------------------------------------------------------------ 0x00537f90
    u32 script_attack(i16* idp) {
        c_attack.hit();
        const i32 id = *idp;
        const i16 sid = static_cast<i16>(id);
        const i32 target = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(sid), 1));  // 0x00652190
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(sid))) != 0 ||                                  // 0x006512d0
            joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(target)) != 0 ||  // 0x00428b70
            joan::orig::arg::as<i32>(joan::orig::unit::idle(joan::orig::arg::i32(target))) != 0) {                               // 0x0065c8d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(sid));                                         // 0x00652370
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(sid), joan::orig::arg::i32(static_cast<i16>(target)))) >= 200) {  // 0x00653150
            joan::orig::group_order(joan::orig::arg::i32(id), 0xc, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(target), 0x64, 0x64, 0, 0x7530);  // 0x00661340
            return 1;
        }
        joan::orig::script::set_mode(joan::orig::arg::i32(sid), str(d::s_ATTACK_9cf380));  // 0x006515d0 "ATTACK"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(sid))) != 0) {         // 0x00651590
            const i32 r = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(sid), joan::orig::arg::i32(target)));           // 0x00652ee0
            joan::orig::group_order(joan::orig::arg::i32(id), 0xa, static_cast<std::int8_t>(joan::orig::arg::i32(4)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(r), 0, 0, 0, 0x9c4);    // 0x00661340
            return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(idp)));                      // 0x00538340
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(sid))) != 0) {  // 0x00651550
            joan::orig::group_order(joan::orig::arg::i32(id), 0xc, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(target), 0x64, 0x64, 0, 0x7530);  // 0x00661340
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_05", name, addr, fn)
B("Unit::script_attack", 0x00537f90, &H::script_attack);
#undef B

}  // namespace game_b106_05
}  // namespace

// ---------------------------------------- from modules/game_b105_07.cpp (game/b105_07)
namespace {
namespace game_b105_07 {

void* const kRandom = obj(d::g_misc);
void* const kKinds = obj(d::g_types);

JOAN_COUNTER(c_voice_538340, "Unit::voice_react_538340");
JOAN_COUNTER(c_voice_53abd0, "Unit::voice_react_53abd0");
JOAN_COUNTER(c_voice_53f670, "Unit::voice_react_53f670");

i32 rnd(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
// 0x00661340 (cdecl, 9 args)
void say(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 delay) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(delay));
}
u32 sample(std::uintptr_t s) { return joan::orig::arg::as<u32>(joan::orig::script_rgb_component(joan::orig::arg::str(str(s)), 1)); }  // 0x006511f0
i32 chk_652aa0(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::get_unit_flag22(joan::orig::arg::i16(id))); }
i32 chk_652820(i32 id) { return joan::orig::arg::as<i32>(joan::orig::unit_has_enemy_near(joan::orig::arg::i16(id))); }
i32 chk_652c10(i32 id) { return joan::orig::arg::as<i32>(joan::orig::unit_check_652c10(joan::orig::arg::i16(id))); }
i32 chk_652b00(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script_unit_busy(joan::orig::arg::i32(id))); }
i16 threshold(void* u) { return joan::orig::arg::as<i16>(joan::orig::as_unit(u)->scaled_2df()); }  // 0x00511f80
i16 value_511e70(void* u) { return joan::orig::arg::as<i16>(joan::orig::as_unit(u)->recruit_delay()); }

// 0x00538340 / 0x0053f670
i32 voice_react(void* u, i32 delay, img::Addr s4, i32 n4) {
    const i32 id = jo::as_unit(u)->id;
    i32 r = rnd(100);
    i32 b = 0;
    const i32 a = chk_652aa0(id);
    if (a != 0) b = chk_652820(id);
    if (r < 0x3c && a != 0 && b != 0) {
        say(id, 0x13, 2, 0, 0, 0, 0, 0, delay);
        return 1;
    }
    if (r < static_cast<i32>(threshold(u))) {
        jo::as_unit(u)->attack_chance = value_511e70(u);
        r = rnd(0xe);
        std::uintptr_t s = 0;
        i32 n = 0;
        if (r <= 2) {
            s = img::addr(d::s_A);
            n = 1;
        } else if (r <= 4) {
            s = img::addr(d::s_AA);
            n = 2;
        } else if (r <= 5) {
            s = img::addr(d::s_AAA);  // 'AAA'
            n = 3;
        } else if (r <= 0xc) {
            s = img::addr(s4);
            n = n4;
        } else {
            if (r == 0xd && chk_652c10(id) != 0) say(id, 0x14, 2, 0, 0, 0, 0, 0, delay);
            return 1;
        }
        const u32 x = sample(s);
        say(id, 1, 2, 0, static_cast<i32>(x), 0, n, 0, delay);
        r = rnd(100);
        if (r < 0x5a) say(id, 6, 1, 0, 3, 0, 0, 0, delay);
        return 1;
    }
    if (r >= 0x5a) {
        r = rnd(3);
        say(id, 6, 2, 0, r + 1, 0, 0, 0, delay);
    }
    return 1;
}

struct H {
    // 0x00538340
    i32 voice_538340(void* u) {
        c_voice_538340.hit();
        return voice_react(u, 0x5dc, d::s_AAB, 4);  // 'AAB'
    }
    // 0x0053f670
    i32 voice_53f670(void* u) {
        c_voice_53f670.hit();
        return voice_react(u, 0x3e8, d::s_B, 1);
    }

    // 0x0053abd0
    i32 voice_53abd0(void* u) {
        c_voice_53abd0.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->is_kind_510f80()) != 0) return 0;  // 0x00510f80
        const i32 id = jo::as_unit(u)->id;
        i32 r = rnd(100);
        i32 b = 0;
        const i32 a = chk_652aa0(id);
        if (a != 0) b = chk_652820(id);
        if (r < 0x3c && a != 0 && b != 0) {
            say(id, 0x13, 2, 0, 0, 0, 0, 0, 0x5dc);
            return 1;
        }
        if (r < 0x3c && global<i32>(d::g_mission) == 7) {
            const i32 kind = jo::as_unit(u)->type_id;
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(kind)));  // 0x0042d0a0
            if (joan::crt::stricmp(joan::orig::arg::str(rec + 4), str(d::s_MARTINGOUGH)) == 0) {  // _stricmp 'MARTINGOUGH'
                const u32 v = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->value_149());                    // 0x00523700
                joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(v));                               // 0x004fe5d0
                if (chk_652b00(id) != 0) {
                    say(id, 0x15, 2, 0, 0, 0, 0, 0, 0x1388);
                    return 1;
                }
            }
        }
        if (!(r < static_cast<i32>(threshold(u)))) return 1;

        i32 flag = 0;
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno))) == 0 &&  // 0x00428b70
            static_cast<i32>(jo::as_unit(u)->id) != global<i32>(d::g_player_unit_recno)) {
            const i32 self_id = jo::as_unit(u)->id;
            void* const sel = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno)));  // 0x0041b3c0
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(sel)->hostile_to(joan::orig::arg::i16(self_id))) != 0) {                                // 0x005094d0
                if (jo::as_unit(u)->markers == 0 || jo::as_unit(u)->markers == -2) flag = 1;
            }
        }
        i32 k = 2;
        const i32 limit = flag != 0 ? 0x64 : 0x78;
        if (static_cast<i32>(jo::as_unit(u)->attack_chance) >= limit) {
            const float m = flag != 0 ? 1.3f : 1.0f;  // 0x3fa66666 / 0x3f800000
            const float roll = static_cast<float>(rnd(200));
            const i32 t = threshold(u);
            if (!(static_cast<float>(t) * m < roll)) {
                r = 0x15;
                k = 3;
            }
        } else {
            r = rnd(0x18);
        }
        jo::as_unit(u)->attack_chance = value_511e70(u);

        std::uintptr_t s = 0;
        i32 n = 0;
        if (r <= 4) {
            s = img::addr(d::s_A);
            n = 1;
        } else if (r <= 8) {
            s = img::addr(d::s_AA);
            n = 2;
        } else if (r <= 0xc) {
            s = img::addr(d::s_AAA);  // 'AAA'
            n = 3;
        } else if (r <= 0x10) {
            s = img::addr(d::s_B);
            n = 1;
        } else if (r <= 0x12) {
            s = img::addr(d::s_AAAA);  // 'AAAA'
            n = 4;
        } else if (r <= 0x14) {
            s = img::addr(d::s_AAAB);  // 'AAAB'
            n = 4;
        } else if (r == 0x15) {
            if (chk_652c10(id) != 0) {
                say(id, 0x14, k, 0, 0, 0, 0, 0, 0x1388);
                return 1;
            }
            s = img::addr(d::s_B);
            n = 1;
        } else if (r <= 0x17) {
            if (chk_652b00(id) != 0) {
                say(id, 0x15, k, 0, 0, 0, 0, 0, 0x1388);
                return 1;
            }
            s = img::addr(d::s_A);
            n = 1;
        } else {
            return 1;
        }
        const u32 x = sample(s);
        say(id, 1, k, 0, static_cast<i32>(x), 0, n, 0, 0x5dc);
        return 1;
    }
};

// CRT initialiser forwarders (direct calls)
template <fn::Code Target>
u32 JOAN_CDECL forward() {
    return cdecl_call<u32>(Target);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_07", name, addr, fn)
B("Unit::voice_react_538340", 0x00538340, &H::voice_538340);
B("Unit::voice_react_53abd0", 0x0053abd0, &H::voice_53abd0);
B("Unit::voice_react_53f670", 0x0053f670, &H::voice_53f670);
B("init_5436b0", 0x005436b0, &forward<fn::Globals_pi_a3cdb4>);
B("init_543710", 0x00543710, &forward<fn::init_a3cdb8>);
B("init_543760", 0x00543760, &forward<fn::div_a3cdbc>);
B("init_5437b0", 0x005437b0, &forward<fn::init_a3cdc0>);
#undef B

}  // namespace game_b105_07
}  // namespace

// ---------------------------------------- from modules/game_b106_03.cpp (game/b106_03)
namespace {
namespace game_b106_03 {

using joan::orig::helpers::random;  // 0x00590540

JOAN_COUNTER(c_react_538c90, "Unit::react_538c90");

// ---- 0x00538c90 helpers -----------------------------------------------------------------------
i32 var_get(i16 id, img::Addr name, i32 a, i32 b, i32 c, i32 d, i32 e) {  // 0x006517c0
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(name), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));
}
void set_state(i16 id, img::Addr name) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(str(name))); }  // 0x006515d0
void order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {                // 0x00661340
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));
}
i32 stat_65b280(i32 id, i32 which) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(which))); }  // 0x0065b280

struct H {
    // ---- 0x00538c90 ------------------------------------------------------------------------
    i32 react(u8* unit) {
        c_react_538c90.hit();
        const i32 id = jo::as_unit(unit)->id;
        const i16 w = static_cast<i16>(id);
        i32 rnd = 0;
        i32 r10 = 0;
        const i32 r14 = 0;  // never set in the original
        i32 r18 = 0;
        r18 = var_get(w, d::s_T, 0, 0, 0, 0, 0);
        r10 = var_get(w, d::s_E_9cfc2c, 0x96, 0, 0, 0, 0);
        if (r10 != 0 || r14 != 0) set_state(w, d::s_ATTACK_9cf380);  // "ATTACK"
        if (r10 == 0 && r14 == 0) {
            set_state(w, d::s_ACTIVE);  // "ACTIVE"
            r10 = var_get(w, d::s_E_9cfc2c, 0xc8, 0, 0, 0, 0);
            rnd = random(0x32);
            if (r10 != 0 && rnd == 0) set_state(w, d::s_ATTACK_9cf380);  // "ATTACK"
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(w))) != 0) {  // 0x00651590
            if (r10 != 0) {
                bool attack = false;
                if (joan::orig::arg::as<i32>(joan::orig::script::in_arc(joan::orig::arg::i32(w), joan::orig::arg::i32(static_cast<i16>(r10)))) != 0) {  // 0x00651430
                    const i32 k = jo::as_unit(unit)->anim_kind;
                    if (k != 0x1e && k != 0x1f && k != 0x20 && k != 0x21) {
                        const i32 ra = random(0xc8);
                        const i32 va = joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->scaled_2df());  // 0x00511f80
                        if (ra > va) {
                            const i32 rb = random(0x64);
                            const i32 vb = joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90());  // 0x00512070
                            if (rb <= vb) attack = true;
                        } else {
                            jo::as_unit(unit)->attack_chance = 0xc8;
                        }
                    }
                }
                const i32 x = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(r10)));  // 0x00652ee0
                order(id, 0xa, 3, 0, x, 0, 0, 0, 0x9c4);
                if (attack) {
                    order(id, 4, 2, 0, r10, 0, 0, 0, 0x3e8);
                    return 1;
                }
                return joan::orig::arg::as<i32>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(static_cast<void*>(unit))));  // 0x0053abd0
            }
            if (r14 != 0) {  // dead in the original (r14 is always 0)
                const i32 x = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(w), joan::orig::arg::i32(r14)));  // 0x00653000
                order(id, 0xa, 3, 0, x, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<i32>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(static_cast<void*>(unit))));  // 0x0053abd0
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(w))) == 0) return 1;  // 0x00651550
        if (joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(w), 1)) != 0) {      // 0x00652190
            const i32 v = stat_65b280(id, 1);
            r10 = var_get(w, d::s_E_9cfc2c, v / 2, 1, 0, 0, 0);
        } else {
            const i32 v = stat_65b280(id, 1);
            r10 = var_get(w, d::s_E_9cfc2c, v, 1, 0, 0, 0);
        }
        if (r10 > 0) {
            order(id, 0xc, 1, 0, r10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        {
            const i32 v = stat_65b280(id, 2);
            const i32 r1c = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->nearest_building(str(d::s_E_9cfc2c), joan::orig::arg::i32(v)));  // 0x00467c30
            if (r1c != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_kind_510f80()) == 0) {               // 0x00510f80
                const i32 ra = random(0x64);
                const i32 va = joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90());  // 0x00512070
                if (ra <= va) {
                    const i32 x = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(w), joan::orig::arg::i32(r1c)));  // 0x00653000
                    order(id, 0xa, 1, 0, x, 0, 0, 0, 0x9c4);
                    order(id, 4, 1, 0, 0, 0, 0, 0, 0x7d0);
                }
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(w), 1)) != 0) return 1;  // 0x00652190
        joan::orig::script::set_target_mode(joan::orig::arg::i32(w), str(d::s_NEAREST_HIGH_PRIORITY));        // 0x00651690 "NEAREST_HIGH_PRIORITY"
        {
            const i32 v = stat_65b280(id, 2);
            r10 = var_get(w, d::s_E_9cfc2c, v, 1, 0, 0, 0);  // same call on both branches of [ebp-0x18]
        }
        if (r10 > 0) {
            order(id, 0xc, 1, 1, r10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (r18 != 0) {
            order(id, 0xc, 1, 1, r18, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_03", name, addr, fn)
B("Unit::react_538c90", 0x00538c90, &H::react);
#undef B

}  // namespace game_b106_03
}  // namespace

// ---------------------------------------- from modules/game_b106_19.cpp (game/b106_19)
namespace {
namespace game_b106_19 {

void* const kRandom = obj(d::g_misc);
void* const kSoldiers = obj(d::g_units);

JOAN_COUNTER(c_attack_ai, "Order::attack_ai");
JOAN_COUNTER(c_attack_simple, "Order::attack_simple");

// ---------------------------------------------------------------- script helpers (cdecl)
i32 sc_get(i32 v, i32 n) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(v), joan::orig::arg::i32(n))); }                // 0x00652190
void sc_state(i32 v, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(v), joan::orig::arg::str(s)); }             // 0x006515d0
i32 sc_651590(i32 v) { return joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(v))); }                         // 0x00651590
i32 sc_651550(i32 v) { return joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(v))); }                         // 0x00651550
i32 sc_653000(i32 v, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(v), joan::orig::arg::i32(t))); }               // 0x00653000
i32 sc_652ee0(i32 v, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(v), joan::orig::arg::i32(t))); }               // 0x00652ee0
i32 sc_65b280(i32 v, i32 n) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(v), joan::orig::arg::i32(n))); }               // 0x0065b280
i32 sc_find(i32 v, img::Addr key, i32 a, i32 b, i32 c, i32 d, i32 e) {
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(v), img::str(key), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));  // 0x006517c0
}
void order(i32 v, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(v), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}
i32 rnd(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540

struct H {
    // ------------------------------------------------------------------------ 0x005392d0
    u32 attack_ai(u8* unit) {
        c_attack_ai.hit();
        const i32 v = jo::as_unit(unit)->id;
        i32 found = 0;            // [ebp-0x10]
        const i32 found2 = 0;     // [ebp-0x14], never written
        i32 primary = 0;          // [ebp-0x18]
        i32 tgt_primary = 0;      // [ebp-0x1c]
        const i32 tgt = sc_get(v, 1);
        i32 range = sc_get(v, 2);
        if (range == 0) range = 0xb4;
        i32 dist;
        if (joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(tgt)) == 0) {  // 0x00428b70
            u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(tgt)));  // 0x0041b3c0
            const float dx = jo::view<float[3]>(jo::as_unit(unit)->position_)[0] - jo::view<float[3]>(jo::as_unit(t)->position_)[0];
            const float dz = jo::view<float[3]>(jo::as_unit(unit)->position_)[1] - jo::view<float[3]>(jo::as_unit(t)->position_)[1];
            const float sq = dx * dx + dz * dz;
            dist = joan::hook::x87::ftol(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sq)))));  // sqrt, _ftol
        } else {
            dist = 0;
        }
        const i32 leash = joan::orig::arg::as<i32>(joan::orig::unit::rank_value(joan::orig::arg::i16(tgt)));  // 0x006592c0
        if (dist > static_cast<i32>(static_cast<u32>(leash) + 0xc8u)) {
            order(v, 0x10, 0, 1, tgt, range, leash, 0, 0x1388);
            joan::orig::script::set_2d7(joan::orig::arg::i32(v), i32{0});  // 0x006558d0
            return 1;
        }
        primary = sc_find(v, d::s_T, 0, 0, 0, 0, 0);
        found = sc_find(v, d::s_E_9cfc2c, 0x96, 0, 0, 0, 0);
        if (found != 0 || found2 != 0) sc_state(v, str(d::s_ATTACK_9cf380));  // "ATTACK"
        if (found == 0 && found2 == 0) {
            sc_state(v, str(d::s_ACTIVE));  // "ACTIVE"
            found = sc_find(v, d::s_E_9cfc2c, 0xc8, 0, 0, 0, 0);
            const i32 roll = rnd(0x32);
            if (found != 0 && roll == 0) sc_state(v, str(d::s_ATTACK_9cf380));  // "ATTACK"
        }
        if (sc_651590(v) != 0) {
            if (found != 0) {
                bool direct = false;  // true: 0x00539601
                if (joan::orig::arg::as<i32>(joan::orig::script::in_arc(joan::orig::arg::i32(v), joan::orig::arg::i32(found))) == 0) {  // 0x00651430
                    direct = true;
                } else {
                    const i32 t = jo::as_unit(unit)->anim_kind;
                    if (t == 0x1e || t == 0x1f || t == 0x20 || t == 0x21) {
                        direct = true;
                    } else {
                        const i32 r1 = rnd(0xc8);
                        if (!(r1 > static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->scaled_2df())))) {  // 0x00511f80
                            jo::as_unit(unit)->attack_chance = 0xc8;
                            direct = true;
                        } else {
                            const i32 r2 = rnd(0x64);
                            if (r2 > static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90()))) {  // 0x00512070
                                direct = true;
                            } else {
                                const i32 p = sc_652ee0(v, found);
                                order(v, 0xa, 3, 0, p, 0, 0, 0, 0x9c4);
                                order(v, 4, 2, 0, found, 0, 0, 0, 0x3e8);
                                return 1;
                            }
                        }
                    }
                }
                (void)direct;
                const i32 p = sc_652ee0(v, found);
                order(v, 0xa, 3, 0, p, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(unit)));  // 0x0053abd0
            }
            if (found2 != 0) {  // dead: [ebp-0x14] is always 0
                const i32 p = sc_653000(v, found2);
                order(v, 0xa, 3, 0, p, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(unit)));  // 0x0053abd0
            }
            return 1;
        }
        if (sc_651550(v) == 0) return 1;
        {
            const i32 key = sc_65b280(v, 2);
            const i32 near = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->nearest_building(str(d::s_E_9cfc2c), joan::orig::arg::i32(key)));  // 0x00467c30
            if (near != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_kind_510f80()) == 0) {                 // 0x00510f80
                const i32 r = rnd(0x64);
                if (!(r > static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90())))) {     // 0x00512070
                    const i32 p = sc_653000(v, near);
                    order(v, 0xa, 1, 0, p, 0, 0, 0, 0x9c4);
                    order(v, 4, 1, 0, 0, 0, 0, 0, 0x7d0);
                }
            }
        }
        if (tgt != 0) tgt_primary = sc_find(tgt, d::s_T, 0, 0, 0, 0, 0);
        {
            const i32 key = sc_65b280(v, 1);
            found = sc_find(v, d::s_E_9cfc2c, key, 1, 0, 0, 0);
        }
        if (found != 0) {
            order(v, 0xc, 1, 0, found, 0x46, 0x46, 0, 0x7530);
            return 1;
        }
        if (tgt_primary != 0) {
            joan::orig::script::set_target_mode(joan::orig::arg::i32(v), str(d::s_NEAREST_HIGH_PRIORITY));  // 0x00651690 "NEAREST_HIGH_PRIORITY"
            if (primary != 0) {
                const i32 key = sc_65b280(v, 2);
                found = sc_find(v, d::s_E_9cfc2c, key, 0, 0, 0, 0);
            } else {
                const i32 key = sc_65b280(v, 2);
                found = sc_find(v, d::s_E_9cfc2c, key, 0, 0, 0, 0);
            }
            if (found > 0) {
                order(v, 0xc, 1, 1, found, 0x46, 0x46, 0, 0x7530);
                return 1;
            }
            if (primary != 0) {
                order(v, 0xc, 1, 1, primary, 0x46, 0x46, 0, 0x7530);
                return 1;
            }
        }
        sc_state(v, str(d::s_IDLE));  // "IDLE"
        return 1;
    }

    // ------------------------------------------------------------------------ 0x0053f4c0
    u32 attack_simple(u8* unit) {
        c_attack_simple.hit();
        const i32 v = jo::as_unit(unit)->id;
        const i32 tgt = sc_get(v, 1);
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(v))) != 0) {  // 0x006512d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(v));          // 0x00652370
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(v), joan::orig::arg::i32(tgt))) >= 0x96) {  // 0x00653060
            order(v, 0xd, 1, 0, tgt, 0x64, 0x32, 0, 0x7530);
            return 1;
        }
        sc_state(v, str(d::s_ATTACK_9cf380));  // "ATTACK"
        if (sc_651590(v) != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(tgt))) == 0) {  // 0x0065b6a0
                joan::orig::script::unit_528300(joan::orig::arg::i32(v));           // 0x00652370
                return 1;
            }
            const i32 p = sc_653000(v, tgt);
            order(v, 0xa, 4, 0, p, 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(unit)));  // 0x0053f670
        }
        if (sc_651550(v) != 0) order(v, 0xd, 1, 0, tgt, 0x64, 0x32, 0, 0x7530);
        return 1;
    }
};

#define B106(name, addr, fn) JOAN_REPLACE("game/b106_19", name, addr, fn)
B106("Order::attack_ai", 0x005392d0, &H::attack_ai);
B106("Order::attack_simple", 0x0053f4c0, &H::attack_simple);
#undef B106

}  // namespace game_b106_19
}  // namespace

// ---------------------------------------- from modules/game_b106_10.cpp (game/b106_10)
namespace {
namespace game_b106_10 {

JOAN_COUNTER(c_ai_step, "UnitAi::step_539a40");

constexpr img::Addr kRandom = d::g_misc;

// ---------------------------------------------------------------- 0x00539a40
i32 ai_query(i16 w, img::Addr name, u32 a, u32 b) {  // 0x006517c0 (7 args)
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(w), img::str(name), joan::orig::arg::i32(a), joan::orig::arg::i32(b), 0, 0, 0));
}
void ai_set_state(i16 w, img::Addr name) { joan::orig::script::set_mode(joan::orig::arg::i32(w), joan::orig::arg::str(str(name))); }  // 0x006515d0
void ai_order(i32 id, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g, u32 h) {  // 0x00661340
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));
}
i32 rnd(u32 n) { return joan::orig::as_misc(obj(kRandom))->random(joan::orig::arg::i32(n)); }  // 0x00590540

struct H {
    // 0x00539a40
    u32 ai_step(u8* unit) {
        c_ai_step.hit();
        const i32 id = jo::as_unit(unit)->id;
        const i16 w = static_cast<i16>(id);
        u32 chance = 0;
        i32 target = 0;
        const i32 other = 0;  // [ebp-0x14]: never written
        i32 first = ai_query(w, d::s_T, 0, 0);
        target = ai_query(w, d::s_E_9cfc2c, 0x96, 0);
        if (target != 0 || other != 0) ai_set_state(w, d::s_ATTACK_9cf380);  // "ATTACK"
        if (target == 0 && other == 0) {
            ai_set_state(w, d::s_ACTIVE);  // "ACTIVE"
            target = ai_query(w, d::s_E_9cfc2c, 0xc8, 0);
            chance = static_cast<u32>(rnd(0x32));
            if (target != 0 && chance == 0) ai_set_state(w, d::s_ATTACK_9cf380);  // "ATTACK"
        }
        if (joan::orig::arg::as<u32>(joan::orig::script::mode_is1(joan::orig::arg::i32(w))) != 0) {  // 0x00651590
            if (target != 0) {
                bool attack_path = false;
                if (joan::orig::arg::as<u32>(joan::orig::script::in_arc(joan::orig::arg::i32(w), joan::orig::arg::i32(static_cast<i16>(target)))) != 0) {  // 0x00651430
                    i32 kind = jo::as_unit(unit)->anim_kind;
                    if (kind != 0x1e && jo::as_unit(unit)->anim_kind != 0x1f && jo::as_unit(unit)->anim_kind != 0x20 &&
                        jo::as_unit(unit)->anim_kind != 0x21) {
                        i32 r = rnd(0xc8);
                        i32 v = joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->scaled_2df());  // 0x00511f80
                        if (!(r > v)) {
                            jo::as_unit(unit)->attack_chance = 0xc8;
                        } else {
                            r = rnd(0x64);
                            v = joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90());  // 0x00512070
                            if (!(r > v)) attack_path = true;
                        }
                    }
                }
                if (attack_path) {
                    u32 t = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(target)));  // 0x00652ee0
                    ai_order(id, 0xa, 3, 0, t, 0, 0, 0, 0x9c4);
                    ai_order(id, 4, 2, 0, static_cast<u32>(target), 0, 0, 0, 0x3e8);
                    return 1;
                }
                u32 t = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(target)));  // 0x00652ee0
                ai_order(id, 0xa, 3, 0, t, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(unit)));  // 0x0053abd0
            }
            if (other != 0) {  // dead
                u32 t = joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(w), joan::orig::arg::i32(other)));  // 0x00653000
                ai_order(id, 0xa, 3, 0, t, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(unit)));  // 0x0053abd0
            }
            return 1;
        }
        if (joan::orig::arg::as<u32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(w))) == 0) return 1;  // 0x00651550
        u32 g = joan::orig::arg::as<u32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 2));       // 0x0065b280
        i32 near_ = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->nearest_building(joan::orig::arg::str(img::addr(d::s_E_9cfc2c)), joan::orig::arg::i32(g)));  // 0x00467c30
        if (near_ != 0 && joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->is_kind_510f80()) == 0) {    // 0x00510f80
            i32 r = rnd(0x64);
            i32 v = joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90());  // 0x00512070
            if (!(r > v)) {
                u32 t = joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(w), joan::orig::arg::i32(near_)));  // 0x00653000
                ai_order(id, 0xa, 1, 0, t, 0, 0, 0, 0x9c4);
                ai_order(id, 4, 1, 0, 0, 0, 0, 0, 0x7d0);
            }
        }
        g = joan::orig::arg::as<u32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 1));  // 0x0065b280
        target = ai_query(w, d::s_E_9cfc2c, g, 1);
        if (target > 0) {
            ai_order(id, 0xc, 1, 0, static_cast<u32>(target), 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        joan::orig::script::set_target_mode(joan::orig::arg::i32(w), str(d::s_NEAREST_HIGH_PRIORITY));  // 0x00651690 "NEAREST_HIGH_PRIORITY"
        // both branches of `if (first)` are identical in the original
        g = joan::orig::arg::as<u32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 2));  // 0x0065b280
        target = ai_query(w, d::s_E_9cfc2c, g, 1);
        if (target > 0) {
            ai_order(id, 0xc, 1, 1, static_cast<u32>(target), 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (first != 0) {
            ai_order(id, 0xc, 1, 1, static_cast<u32>(first), 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        ai_set_state(w, d::s_IDLE);                     // "IDLE"
        u32 t = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(w), 1));      // 0x00652190
        ai_order(id, 0xe, 0, 1, t, 0, 0, 0, 0x7530);
        return 1;
    }

  private:
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_10", name, addr, fn)
B("UnitAi::step_539a40", 0x00539a40, &H::ai_step);
#undef B

}  // namespace game_b106_10
}  // namespace

// ---------------------------------------- from modules/game_b106_13.cpp (game/b106_13)
namespace {
namespace game_b106_13 {

JOAN_COUNTER(c_choose_attack, "Ai::choose_attack");

// ======================= Ai::choose_attack 0x0053a060 =======================
std::int32_t find(std::int32_t id, img::Addr what, std::int32_t a, std::int32_t b, std::int32_t c,
                  std::int32_t d, std::int32_t e) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(what), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));  // 0x006517c0
}
void set_state(std::int32_t id, img::Addr s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(str(s))); }  // 0x006515d0
void order(std::int32_t id, std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t d, std::int32_t e,
           std::int32_t f, std::int32_t g, std::int32_t h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}
std::int32_t rnd(std::int32_t n) { return joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(n)); }  // 0x00590540

struct Ai {
    std::uint32_t choose_attack(void* unit) {
        c_choose_attack.hit();
        const std::int32_t id = jo::as_unit(unit)->id;  // [ebp-0x8]
        std::int32_t target = 0;                            // [ebp-0x10]
        const std::int32_t none = 0;                        // [ebp-0x14], never written
        const std::int32_t army = joan::orig::arg::as<std::int32_t>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), 1));  // 0x00652190
        joan::orig::script::group_angle(joan::orig::arg::i32(army));                             // 0x006547a0
        const std::int32_t prio = find(id, d::s_T, 0, 0, 0, 0, 0);          // [ebp-0x18]
        target = find(id, d::s_EP, 0x96, 0, army, 0, 0);
        if (target != 0 || none != 0) set_state(id, d::s_ATTACK_9cf380);  // 'ATTACK'
        if (target == 0 && none == 0) {
            set_state(id, d::s_ACTIVE);  // 'ACTIVE'
            target = find(id, d::s_EP, 0xc8, 0, army, 0, 0);
            const std::int32_t r = rnd(0x32);
            if (target != 0 && r == 0) set_state(id, d::s_ATTACK_9cf380);  // 'ATTACK'
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {  // 0x00651590
            joan::orig::script::group_set_angle(joan::orig::arg::i32(army), 0);      // 0x006547e0
            if (target != 0) {
                bool direct = false;
                if (joan::orig::arg::as<std::int32_t>(joan::orig::script::in_arc(joan::orig::arg::i32(id), static_cast<std::int32_t>(static_cast<std::int16_t>(target)))) != 0 &&  // 0x00651430
                    jo::as_unit(unit)->anim_kind != 0x1e && jo::as_unit(unit)->anim_kind != 0x1f &&
                    jo::as_unit(unit)->anim_kind != 0x20 && jo::as_unit(unit)->anim_kind != 0x21) {
                    const std::int32_t r1 = rnd(0xc8);
                    if (r1 > static_cast<std::int16_t>(joan::orig::arg::as<std::uint16_t>(joan::orig::as_unit(unit)->scaled_2df()))) {  // 0x00511f80
                        const std::int32_t r2 = rnd(0x64);
                        if (!(r2 > static_cast<std::int16_t>(joan::orig::arg::as<std::uint16_t>(joan::orig::as_game(unit)->scaled_a3cc90())))) direct = true;  // 0x00512070
                    } else {
                        jo::as_unit(unit)->attack_chance = 0xc8;
                    }
                }
                if (direct) {
                    const std::int32_t t = joan::orig::arg::as<std::int32_t>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(target)));  // 0x00652ee0
                    order(id, 0xa, 3, 0, t, 0, 0, 0, 0x9c4);
                    order(id, 4, 2, 0, target, 0, 0, 0, 0x3e8);
                    return 1;
                }
                const std::int32_t t = joan::orig::arg::as<std::int32_t>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(target)));  // 0x00652ee0
                order(id, 0xa, 4, 0, t, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<std::uint32_t>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(unit)));  // 0x0053abd0
            }
            if (none != 0) {  // dead in the original ([ebp-0x14] is always 0), kept
                const std::int32_t t = joan::orig::arg::as<std::int32_t>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(none)));  // 0x00653000
                order(id, 0xa, 3, 0, t, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<std::uint32_t>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(unit)));  // 0x0053abd0
            }
            return 1;
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) == 0) return 1;  // 0x00651550
        const std::int32_t kind = joan::orig::arg::as<std::int32_t>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 2));  // 0x0065b280
        const std::int32_t foe = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->nearest_building(str(d::s_E_9cfc2c), joan::orig::arg::i32(kind)));  // 0x00467c30
        if (foe != 0 && joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->is_kind_510f80()) == 0) {  // 0x00510f80
            const std::int32_t r = rnd(0x64);
            if (!(r > static_cast<std::int16_t>(joan::orig::arg::as<std::uint16_t>(joan::orig::as_game(unit)->scaled_a3cc90())))) {  // 0x00512070
                const std::int32_t t = joan::orig::arg::as<std::int32_t>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(foe)));  // 0x00653000
                order(id, 0xa, 1, 0, t, 0, 0, 0, 0x9c4);
                order(id, 4, 1, 0, 0, 0, 0, 0, 0x7d0);
            }
        }
        target = find(id, d::s_EP, joan::orig::arg::as<std::int32_t>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 1)), 1, army, 0, 0);
        if (target > 0) {
            order(id, 0xc, 1, 0, target, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        joan::orig::script::set_target_mode(joan::orig::arg::i32(id), str(d::s_NEAREST_HIGH_PRIORITY));  // 0x00651690 'NEAREST_HIGH_PRIORITY'
        if (prio != 0) {
            target = find(id, d::s_EP, joan::orig::arg::as<std::int32_t>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 2)), 1, army, 0, 0);
        } else {
            target = find(id, d::s_EP, joan::orig::arg::as<std::int32_t>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 2)), 1, army, 0, 0);
        }
        if (target > 0) {
            order(id, 0xc, 1, 1, target, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (prio != 0 && joan::orig::arg::as<std::int32_t>(joan::orig::script::group_contains(joan::orig::arg::i32(army), joan::orig::arg::i32(prio))) != 0) {  // 0x00653f30
            order(id, 0xc, 1, 1, prio, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        set_state(id, d::s_IDLE);  // 'IDLE'
        order(id, 0xe, 0, 1, army, 0, 0, 0, 0x7530);
        return 1;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b106_13", name, addr, fn)
M("Ai::choose_attack", 0x0053a060, &Ai::choose_attack);
#undef M

}  // namespace game_b106_13
}  // namespace

// ---------------------------------------- from modules/game_b106_08.cpp (game/b106_08)
namespace {
namespace game_b106_08 {

JOAN_COUNTER(c_attack, "Order::attack_53a6d0");

struct H {
    // 0x0053a6d0
    u32 attack(u8* unit) {
        c_attack.hit();
        const i32 id = static_cast<i32>(jo::as_unit(unit)->id);
        const i16 wid = static_cast<i16>(id);
        const i32 target = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(wid), 1));  // 0x00652190
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(wid))) != 0 ||               // 0x006512d0
            joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(target)) != 0 ||  // 0x00428b70
            joan::orig::arg::as<i32>(joan::orig::unit::idle(joan::orig::arg::i32(target))) != 0) {            // 0x0065c8d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(wid));                      // 0x00652370
            return 1;
        }
        const i16 wtarget = static_cast<i16>(target);
        if (joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(wid), joan::orig::arg::i32(wtarget))) >= 200) {  // 0x00653150
            joan::orig::group_order(joan::orig::arg::i32(id), 0xc, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(target), 100, 100, 0, 30000);  // 0x00661340
            return 1;
        }
        joan::orig::script::set_mode(joan::orig::arg::i32(wid), str(d::s_ATTACK_9cf380));  // 0x006515d0 "ATTACK"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(wid))) != 0) {         // 0x00651590
            if (joan::orig::arg::as<i32>(joan::orig::script::in_arc(joan::orig::arg::i32(wid), joan::orig::arg::i32(wtarget))) != 0) {  // 0x00651430
                const i32 kind = static_cast<i32>(jo::as_unit(unit)->anim_kind);
                if (kind != 0x1e && kind != 0x1f && kind != 0x20 && kind != 0x21) {
                    const i32 r1 = joan::orig::as_misc(obj(d::g_misc))->random(200);  // 0x00590540
                    const i32 v1 = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->scaled_2df()));  // 0x00511f80
                    if (r1 <= v1) {
                        jo::as_unit(unit)->attack_chance = 200;
                    } else {
                        const i32 r2 = joan::orig::as_misc(obj(d::g_misc))->random(100);  // 0x00590540
                        const i32 v2 = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90()));  // 0x00512070
                        if (r2 <= v2) {
                            const u32 a = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(wid), joan::orig::arg::i32(target)));  // 0x00652ee0
                            joan::orig::group_order(joan::orig::arg::i32(id), 0xa, static_cast<std::int8_t>(joan::orig::arg::i32(3)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(a), 0, 0, 0, 0x9c4);  // 0x00661340
                            joan::orig::group_order(joan::orig::arg::i32(id), 4, static_cast<std::int8_t>(joan::orig::arg::i32(2)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(target), 0, 0, 0, 0x3e8);
                            return 1;
                        }
                    }
                }
            }
            const u32 a = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(wid), joan::orig::arg::i32(target)));           // 0x00652ee0
            joan::orig::group_order(joan::orig::arg::i32(id), 0xa, static_cast<std::int8_t>(joan::orig::arg::i32(4)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(a), 0, 0, 0, 0x9c4);  // 0x00661340
            return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(unit)));                     // 0x0053abd0
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(wid))) != 0) {  // 0x00651550
            joan::orig::group_order(joan::orig::arg::i32(id), 0xc, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(target), 100, 100, 0, 30000);  // 0x00661340
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_08", name, addr, fn)
B("Order::attack_53a6d0", 0x0053a6d0, &H::attack);
#undef B

}  // namespace game_b106_08
}  // namespace

// ---------------------------------------- from modules/game_b106_09.cpp (game/b106_09)
namespace {
namespace game_b106_09 {

JOAN_COUNTER(c_order_attack, "Unit::order_attack_53a9d0");

struct H {
    // ------------------------------------------------------------------------ 0x0053a9d0
    u32 order_attack(void* unit) {
        c_order_attack.hit();
        const i32 id = jo::as_unit(unit)->id;  // [ebp-0x8]
        const u32 w = static_cast<u32>(id);
        const u32 target = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(w), 1));  // 0x00652190
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(w))) != 0) {             // 0x006512d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(w));                     // 0x00652370
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(target))) == 0 ||                         // 0x0065b6a0
            joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(target)) != 0) {  // 0x00425d50
            joan::orig::script::unit_528300(joan::orig::arg::i32(w));  // 0x00652370
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(w), joan::orig::arg::i32(target))) >= 0x96) {  // 0x00653060
            joan::orig::group_order(joan::orig::arg::i32(w), 0xd, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(target), 0x64, 0x32, 0, 0x7530);  // 0x00661340
            return 1;
        }
        joan::orig::script::set_mode(joan::orig::arg::i32(w), str(d::s_ATTACK_9cf380));  // 0x006515d0 'ATTACK'
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(w))) != 0) {         // 0x00651590
            if (joan::orig::arg::as<i32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(target))) == 0) {  // 0x0065b6a0
                joan::orig::script::unit_528300(joan::orig::arg::i32(w));              // 0x00652370
                return 1;
            }
            const u32 d = joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(w), joan::orig::arg::i32(target)));                  // 0x00653000
            joan::orig::group_order(joan::orig::arg::i32(w), 0xa, static_cast<std::int8_t>(joan::orig::arg::i32(4)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(d), 0, 0, 0, 0x9c4);          // 0x00661340
            return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(unit)));                          // 0x0053abd0
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(w))) != 0) {  // 0x00651550
            joan::orig::group_order(joan::orig::arg::i32(w), 0xd, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(target), 0x64, 0x32, 0, 0x7530);  // 0x00661340
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_09", name, addr, fn)
B("Unit::order_attack_53a9d0", 0x0053a9d0, &H::order_attack);
#undef B

}  // namespace game_b106_09
}  // namespace

// ---------------------------------------- from modules/game_b106_11.cpp (game/b106_11)
namespace {
namespace game_b106_11 {

void* const kRandom = obj(d::g_misc);

i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540

JOAN_COUNTER(c_state_53b1b0, "Ai::state_53b1b0");

// ------------------------------------------------------------------ 0x0053b1b0 helpers
const char* const kAttack = str(d::s_ATTACK_9cf380);  // "ATTACK"
const char* const kIdle = str(d::s_IDLE);    // "IDLE"
const char* const kActive = str(d::s_ACTIVE);  // "ACTIVE"
const char* const kEnemy = str(d::s_E_9cfc2c);

i32 unit_target(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }   // 0x00652190
void set_state(i32 id, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }   // 0x006515d0
i32 aim_2ee0(i32 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }       // 0x00652ee0
void order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}
i32 sx(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct H {
    // ------------------------------------------------------------------ 0x0053b1b0
    i32 state_53b1b0(i16* unit) {
        c_state_53b1b0.hit();
        const i32 id = static_cast<i32>(unit[0]);
        nearest(id, kAny, 0, 0, 0, 0, 0);  // stored to a dead local
        i32 t = nearest(id, kEnemy, 0x96, 0, 0, 0, 0);
        if (t != 0) {
            set_state(id, kAttack);
        } else {
            set_state(id, kActive);
            t = nearest(id, kEnemy, 0xc8, 0, 0, 0, 0);
            const i32 r = random(0x32);
            if (t != 0 && r == 0) set_state(id, kAttack);
        }
        if (can_melee(id) != 0 && t != 0) {
            if (check_1430(id, sx(t)) != 0) {
                const i16 kind = jo::as_unit(unit)->anim_kind;
                if (kind != 0x1e && kind != 0x1f && kind != 0x20 && kind != 0x21) {
                    const i32 r = random(0xc8);
                    if (r <= static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->scaled_2df()))) {  // 0x00511f80
                        jo::as_unit(unit)->attack_chance = 0xc8;
                    } else {
                        const i32 r2 = random(0x64);
                        if (r2 <= static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90()))) {  // 0x00512070
                            order(id, 0xa, 3, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
                            order(id, 4, 2, 0, t, 0, 0, 0, 0x3e8);
                            return 1;
                        }
                    }
                }
            }
            order(id, 0xa, 3, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<i32>(joan::orig::as_unit(this)->voice_react_53abd0(joan::orig::arg::mptr(unit)));  // 0x0053abd0
        }
        if (can_move(id) != 0) {
            const i32 range = range_b280(id, 1);
            t = nearest(id, kEnemy, range, 1, 0, 0, 0);
            if (t > 0) {
                order(id, 0xc, 1, 0, t, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
            const i32 a = unit_target(id, 1);
            const i32 b = unit_target(id, 2);
            const i32 c = unit_target(id, 3);
            order(id, 7, 0, 0, a, b, c, 0, 0x7530);
            if (dist_b800(id, a, b, c) <= 0xc8) unit_stop(id);
            set_state(id, kIdle);
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_11", name, addr, fn)
B("Ai::state_53b1b0", 0x0053b1b0, &H::state_53b1b0);
#undef B

}  // namespace game_b106_11
}  // namespace

// ---------------------------------------- from modules/game_b106_17.cpp (game/b106_17)
namespace {
namespace game_b106_17 {

JOAN_COUNTER(c_53b610, "Ai::state_53b610");
JOAN_COUNTER(c_53d3b0, "Ai::state_53d3b0");

void* const kRandom = obj(d::g_misc);

const char* const kAttack = str(d::s_ATTACK_9cf380);       // "ATTACK"
const char* const kIdle = str(d::s_IDLE);         // "IDLE"
const char* const kActive = str(d::s_ACTIVE);       // "ACTIVE"
const char* const kEnemy = str(d::s_E_9cfc2c);
const char* const kGroup = str(d::s_L);

// ---- AI script helpers (cdecl) ----------------------------------------------------------
i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
i32 unit_target(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }        // 0x00652190
void set_state(i32 id, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }        // 0x006515d0
void set_target_mode(i32 id, const char* s) { joan::orig::script::set_target_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }  // 0x00651690
i32 aim_2ee0(i32 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }            // 0x00652ee0
i32 aim_3000(i32 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }            // 0x00653000
i32 group_size(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::leader_id(joan::orig::arg::i32(id))); }                    // 0x006569f0
void group_reset(i32 id, i32 k) { joan::orig::script::set_2d7(joan::orig::arg::i32(id), joan::orig::arg::i32(k)); }              // 0x006558d0
void order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}
i32 sx16(i32 v) { return static_cast<i16>(v); }

// The shared "charge or formation" block of the AI handlers (target e14 > 0 and its type's
// kind not 5 / 6). Returns false when the block is not taken.
bool charge_block(i32 id, i32 e10, i32 e14) {
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(e14)));  // 0x0041b3c0
    u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
    if (static_cast<i32>(jo::self<jo::UnitType>(type)->unit_class) == 5) return false;
    if (static_cast<i32>(jo::self<jo::UnitType>(type)->unit_class) == 6) return false;
    const i32 r = random(0xa);
    if (r < 1) {
        order(id, 0xc, 1, 0, e10, 0x3c, 0x3c, 0, 0x7530);
    } else if (r < 5) {
        const i32 a = aim_2ee0(id, e10);
        order(id, 0xa, 1, 0, a, 0, 0, 0, 0x9c4);
        order(id, 6, 1, 0, 1, 0, 0, 0, 0x5dc);
    } else if (r < 9) {
        const i32 a = aim_2ee0(id, e10);
        order(id, 0xa, 1, 0, a, 0, 0, 0, 0x9c4);
        order(id, 6, 1, 0, 2, 0, 0, 0, 0x5dc);
    } else if (r < 0xa) {
        const i32 a = aim_2ee0(id, e10);
        order(id, 0xa, 1, 0, a, 0, 0, 0, 0x9c4);
        order(id, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
    }
    return true;
}

struct H {
    // 0x0053b610
    u32 state_53b610(i16* unit) {
        c_53b610.hit();
        const i32 id = static_cast<i32>(*unit);
        i32 e10 = 0, e14 = 0, e1c = 0, e20 = 0, e24 = 0;
        const i32 e18 = 0;  // never written in the original
        e1c = nearest(id, kAny, 0, 0, 0, 0, 0);
        if (group_size(id) > 0) {
            e20 = nearest(id, kGroup, 0x3e8, 0, 0, 0, 0);
            if (e20 == 0) {
                order(id, 0xf, 2, 1, 0, 0, 0, 0, 0x2710);
                group_reset(id, 0);
                return 1;
            }
            if (e1c == 0) order(id, 0xf, -2, 1, 0, 0, 0, 0, 0x2710);
        }
        e10 = nearest(id, kEnemy, 0x64, 0, 0, 0, 0);
        if (e10 != 0 || e18 != 0) set_state(id, kAttack);
        if (e10 == 0 && e18 == 0) {
            set_state(id, kActive);
            e10 = nearest(id, kEnemy, 0x96, 0, 0, 0, 0);
            const i32 r = random(0x32);
            if (e10 != 0 && r == 0) set_state(id, kAttack);
        }
        if (can_melee(id) != 0) {
            if (e10 != 0) {
                if (check_1430(id, sx16(e10)) != 0) {
                    const i32 r = random(0x64);
                    const i32 v = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90()));  // 0x00512070
                    if (!(r > v)) {
                        const i32 a = aim_2ee0(id, e10);
                        order(id, 0xa, 4, 0, a, 0, 0, 0, 0x9c4);
                        order(id, 4, 4, 0, e10, 0, 0, 0, 0x3e8);
                        return 1;
                    }
                }
                const i32 a = aim_2ee0(id, e10);
                order(id, 0xa, 3, 0, a, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_b(joan::orig::arg::mptr(unit)));  // 0x0053cb10
            }
            if (e18 != 0) {
                const i32 a = aim_3000(id, e18);
                order(id, 0xa, 3, 0, a, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_b(joan::orig::arg::mptr(unit)));  // 0x0053cb10
            }
            return 1;
        }
        if (can_move(id) == 0) return 1;
        if (e20 > 0) e24 = nearest(sx16(e20), kAny, 0, 0, 0, 0, 0);
        set_target_mode(id, kNearestHigh);
        if (unit_target(id, 1) != 0) {
            e10 = nearest(id, kEnemy, range_b280(id, 1) / 2, 1, 0, 0, 0);
        } else {
            e10 = nearest(id, kEnemy, range_b280(id, 1), 1, 0, 0, 0);
        }
        e14 = nearest(id, kEnemy, range_b280(id, 1) / 2, 0, 0, 0, 0);
        if (e14 > 0 && charge_block(id, e10, e14)) return 1;
        if (e10 > 0) {
            order(id, 0xc, 1, 0, e10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (e20 == 0 || e24 > 0) {
            if (unit_target(id, 1) == 0) {
                set_target_mode(id, kNearestHigh);
                if (e1c != 0) {
                    e10 = nearest(id, kEnemy, range_b280(id, 2), 1, 0, 0, 0);
                } else {
                    e10 = nearest(id, kEnemy, range_b280(id, 2), 1, 0, 0x87, 0);
                }
                if (e10 > 0) {
                    order(id, 0xc, 1, 0, e10, 0x3c, 0x3c, 0, 0x7530);
                    return 1;
                }
            }
        }
        if (unit_target(id, 1) == 0 && e1c > 0) {
            const i32 dist = dist_3150(id, sx16(e1c));
            const i32 r = range_b280(id, 3);
            if (!(dist > r)) {
                order(id, 0xc, 1, 1, e1c, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        set_state(id, kIdle);
        return 1;
    }

    // 0x0053d3b0
    u32 state_53d3b0(i16* unit) {
        c_53d3b0.hit();
        const i32 id = static_cast<i32>(*unit);
        i32 e10 = 0, e14 = 0, e1c = 0;
        const i32 e18 = 0;  // never written in the original
        e1c = nearest(id, kAny, 0, 0, 0, 0, 0);
        e10 = nearest(id, kEnemy, 0x64, 0, 0, 0, 0);
        if (e10 != 0 || e18 != 0) set_state(id, kAttack);
        if (e10 == 0 && e18 == 0) {
            set_state(id, kActive);
            e10 = nearest(id, kEnemy, 0x96, 0, 0, 0, 0);
            const i32 r = random(0x32);
            if (e10 != 0 && r == 0) set_state(id, kAttack);
        }
        if (can_melee(id) != 0) {
            if (e10 != 0) {
                if (check_1430(id, sx16(e10)) != 0) {
                    const i32 r = random(0x64);
                    const i32 v = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90()));  // 0x00512070
                    if (!(r > v)) {
                        const i32 a = aim_2ee0(id, e10);
                        order(id, 0xa, 4, 0, a, 0, 0, 0, 0x9c4);
                        order(id, 4, 4, 0, e10, 0, 0, 0, 0x3e8);
                        return 1;
                    }
                }
                const i32 a = aim_2ee0(id, e10);
                order(id, 0xa, 3, 0, a, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(unit)));  // 0x0053f670
            }
            if (e18 != 0) {
                const i32 a = aim_3000(id, e18);
                order(id, 0xa, 3, 0, a, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(unit)));  // 0x0053f670
            }
            return 1;
        }
        if (can_move(id) == 0) return 1;
        set_target_mode(id, kNearestHigh);
        if (unit_target(id, 1) != 0) {
            e10 = nearest(id, kEnemy, range_b280(id, 1) / 2, 1, 0, 0, 0);
        } else {
            e10 = nearest(id, kEnemy, range_b280(id, 1), 1, 0, 0, 0);
        }
        e14 = nearest(id, kEnemy, range_b280(id, 1) / 2, 0, 0, 0, 0);
        if (e14 > 0 && charge_block(id, e10, e14)) return 1;
        if (e10 > 0) {
            order(id, 0xc, 1, 0, e10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (unit_target(id, 1) != 0) return 1;
        set_target_mode(id, kNearestHigh);
        if (e1c != 0) {
            e10 = nearest(id, kEnemy, range_b280(id, 2), 1, 0, 0, 0);
        } else {
            e10 = nearest(id, kEnemy, range_b280(id, 2), 1, 0, 0, 0);
        }
        if (e10 > 0) {
            order(id, 0xc, 1, 1, e10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (e1c == 0) return 1;
        const i32 dist = dist_3150(id, sx16(e1c));
        const i32 r = range_b280(id, 3);
        if (dist > r) return 1;
        order(id, 0xc, 1, 1, e1c, 0x3c, 0x3c, 0, 0x7530);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_17", name, addr, fn)
B("Ai::state_53b610", 0x0053b610, &H::state_53b610);
B("Ai::state_53d3b0", 0x0053d3b0, &H::state_53d3b0);
#undef B

}  // namespace game_b106_17
}  // namespace

// ---------------------------------------- from modules/game_b106_04.cpp (game/b106_04)
namespace {
namespace game_b106_04 {

JOAN_COUNTER(c_ai_think, "UnitAI::think_53db10");

struct H {
    // -----------------------------------------------------------------------------------
    // 0x00525080
    struct Frame525 {         // ebp-0x16c .. ebp-0xc of the original
        char b16c[0x20];      // ebp-0x16c
        u8 pad14c[4];         // ebp-0x14c (NUL of the 0x20 read)
        char b148[0x28];      // ebp-0x148
        u8 pad120[4];         // ebp-0x120 (NUL of the 0x28 read)
        char b11c[8];         // ebp-0x11c
        u8 pad114[4];         // ebp-0x114 (NUL of the 8 read)
        char b110[0x100];     // ebp-0x110
        i32 i;                // ebp-0x10 (NUL of the 0x100 read lands in its low byte)
    };
    static_assert(sizeof(Frame525) == 0x160 && offsetof(Frame525, i) == 0x15c, "frame");

    // -----------------------------------------------------------------------------------
    // 0x0053db10
    u32 ai_think(void* unit) {
        c_ai_think.hit();
        const i32 id = jo::as_unit(unit)->id;  // ebp-0x8 (movsx)
        const i16 sid = static_cast<i16>(id);
        auto query = [sid](img::Addr name, i32 a3, i32 a4) {  // 0x006517c0
            return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(sid), str(name), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), 0, 0, 0));
        };
        auto set_state = [sid](img::Addr name) { joan::orig::script::set_mode(joan::orig::arg::i32(sid), joan::orig::arg::str(str(name))); };  // 0x006515d0
        auto order = [](i32 a1, i32 a2, i32 a3, i32 a4, i32 a5, i32 a6, i32 a7, i32 a8, i32 a9) {
            joan::orig::group_order(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), static_cast<std::int8_t>(joan::orig::arg::i32(a3)), static_cast<std::int8_t>(joan::orig::arg::i32(a4)), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9));  // 0x00661340
        };
        auto rnd = [](i32 n) { return joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(n)); };  // 0x00590540
        i32 v0c = 0;
        i32 v10 = 0;
        i32 v14 = 0;
        const i32 v18 = 0;  // never written in the original
        i32 v1c = 0;
        v1c = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(sid), str(d::s_T), 0, 0, 0, 0, 0));
        v10 = query(d::s_E_9cfc2c, 0x64, 0);
        if (v10 != 0 || v18 != 0) set_state(d::s_ATTACK_9cf380);  // "ATTACK"
        if (v10 == 0 && v18 == 0) {
            set_state(d::s_ACTIVE);  // "ACTIVE"
            v10 = query(d::s_E_9cfc2c, 0x96, 0);
            v0c = rnd(0x32);
            if (v10 != 0 && v0c == 0) set_state(d::s_ATTACK_9cf380);  // "ATTACK"
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(sid))) != 0) {  // 0x00651590
            if (v10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::script::in_arc(joan::orig::arg::i32(sid), joan::orig::arg::i32(static_cast<i16>(v10)))) != 0) {  // 0x00651430
                    const i32 r = rnd(0x64);
                    const i16 s = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_game(unit)->scaled_a3cc90()));  // 0x00512070
                    if (!(r > static_cast<i32>(s))) {
                        const i32 t = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(sid), joan::orig::arg::i32(v10)));  // 0x00652ee0
                        order(id, 0xa, 4, 0, t, 0, 0, 0, 0x9c4);
                        order(id, 4, 4, 0, v10, 0, 0, 0, 0x3e8);
                        return 1;
                    }
                }
                const i32 t = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(sid), joan::orig::arg::i32(v10)));  // 0x00652ee0
                order(id, 0xa, 3, 0, t, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(unit)));  // 0x0053f670
            }
            if (v18 != 0) {
                const i32 t = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(sid), joan::orig::arg::i32(v18)));  // 0x00653000
                order(id, 0xa, 3, 0, t, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(unit)));  // 0x0053f670
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(sid))) == 0) return 1;  // 0x00651550
        {
            const i32 r = joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 1));  // 0x0065b280
            v10 = query(d::s_E_9cfc2c, r, 1);
        }
        {
            const i32 r = joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 1));  // 0x0065b280
            v14 = query(d::s_E_9cfc2c, r / 2, 0);
        }
        if (v14 > 0) {
            void* const u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(v14));  // 0x0041b3c0
            void* const ty = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));  // 0x0042d0a0
            if (jo::self<jo::UnitType>(ty)->unit_class != 5 && jo::self<jo::UnitType>(ty)->unit_class != 6) {
                const i32 r = rnd(0xa);
                if (r < 1) {
                    order(id, 0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
                } else if (r < 10) {
                    const i32 level = r < 5 ? 1 : (r < 9 ? 2 : 3);
                    const i32 t = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(sid), joan::orig::arg::i32(v10)));  // 0x00652ee0
                    order(id, 0xa, 1, 0, t, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, level, 0, 0, 0, 0x5dc);
                }
                return 1;
            }
        }
        if (v10 > 0) {
            order(id, 0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        joan::orig::script::set_target_mode(joan::orig::arg::i32(sid), str(d::s_NEAREST_HIGH_PRIORITY));  // 0x00651690 "NEAREST_HIGH_PRIORITY"
        {
            // Both branches on v1c are identical in the original.
            const i32 r = joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 2));  // 0x0065b280
            v10 = query(d::s_E_9cfc2c, r, 1);
        }
        if (v10 > 0) {
            order(id, 0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (v1c != 0) {
            const i32 a = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(sid), joan::orig::arg::i32(static_cast<i16>(v1c))));  // 0x00653150
            const i32 b = joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), 3));                       // 0x0065b280
            if (!(a > b)) {
                order(id, 0xc, 1, 1, v1c, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        set_state(d::s_IDLE);  // "IDLE"
        const i32 t = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(sid), 1));  // 0x00652190
        order(id, 0xe, 0, 1, t, 0, 0, 0, 0x7530);
        return 1;
    }
};

JOAN_REPLACE("game/b106_04", "UnitAI::think_53db10", 0x0053db10, &H::ai_think);

}  // namespace game_b106_04
}  // namespace

// ---------------------------------------- from modules/game_b106_18.cpp (game/b106_18)
namespace {
namespace game_b106_18 {

JOAN_COUNTER(c_think, "UnitAI::think_attack");

void* const kRandom = obj(d::g_misc);

// ---------------------------------------------------------------- 0x0053e240 helpers
i32 group_find(i16 w, const char* name, i32 a, i32 b, i32 c, i32 d, i32 e) {  // 0x006517c0
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(w), name, joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));
}
void set_state(i16 w, const char* name) { joan::orig::script::set_mode(joan::orig::arg::i32(w), joan::orig::arg::str(name)); }  // 0x006515d0
void order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {    // 0x00661340
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));
}
i32 rnd(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
i32 count_of(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }  // 0x0065b280

struct H {
    // ------------------------------------------------------------ 0x0053e240
    i32 think_attack(void* unit) {
        c_think.hit();
        const i32 id = jo::as_unit(unit)->id;
        const i16 w = static_cast<i16>(id);
        i32 v10 = 0;
        const i32 v18 = 0;  // never written in the original
        i32 v1c = 0;
        const i32 g20 = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(w), 1));  // 0x00652190
        joan::orig::script::group_angle(joan::orig::arg::i32(g20));                    // 0x006547a0
        v1c = group_find(w, str(d::s_T), 0, 0, 0, 0, 0);
        v10 = group_find(w, str(d::s_EP), 0x64, 0, g20, 0, 0);
        if (v10 != 0 || v18 != 0) set_state(w, str(d::s_ATTACK_9cf380));  // "ATTACK"
        if (v10 == 0 && v18 == 0) {
            set_state(w, str(d::s_ACTIVE));  // "ACTIVE"
            v10 = group_find(w, str(d::s_EP), 0x96, 0, g20, 0, 0);
            const i32 vc = rnd(0x32);
            if (v10 != 0 && vc == 0) set_state(w, str(d::s_ATTACK_9cf380));  // "ATTACK"
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(w))) != 0) {  // 0x00651590
            joan::orig::script::group_set_angle(joan::orig::arg::i32(g20), 0);     // 0x006547e0
            if (v10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::script::in_arc(joan::orig::arg::i32(w), joan::orig::arg::i32(static_cast<i16>(v10)))) != 0) {  // 0x00651430
                    const i32 r = rnd(0x64);
                    const i32 lim = joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90());  // 0x00512070
                    if (!(r > lim)) {
                        const i32 e = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(v10)));  // 0x00652ee0
                        order(id, 0xa, 5, 0, e, 0, 0, 0, 0x9c4);
                        order(id, 4, 5, 0, v10, 0, 0, 0, 0x3e8);
                        return 1;
                    }
                }
                const i32 e = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(v10)));  // 0x00652ee0
                order(id, 0xa, 4, 0, e, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<i32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(unit)));  // 0x0053f670
            }
            if (v18 != 0) {
                const i32 e = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(w), joan::orig::arg::i32(v18)));  // 0x00653000
                order(id, 0xa, 3, 0, e, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<i32>(joan::orig::as_unit(this)->voice_react_53f670(joan::orig::arg::mptr(unit)));  // 0x0053f670
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(w))) == 0) return 1;  // 0x00651550
        joan::orig::script::set_target_mode(joan::orig::arg::i32(w), str(d::s_NEAREST_HIGH_PRIORITY));     // 0x00651690 "NEAREST_HIGH_PRIORITY"
        {
            const i32 n = count_of(id, 1);
            v10 = group_find(w, str(d::s_EP), n, 1, g20, 0, 0);
        }
        i32 v14;
        {
            const i32 n = count_of(id, 1);
            v14 = group_find(w, str(d::s_EP), n / 2, 0, g20, 0, 0);
        }
        if (v14 > 0) {
            void* const rec = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(v14));  // 0x0041b3c0
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(rec)->type_id)));  // 0x0042d0a0
            if (jo::self<jo::UnitType>(type)->unit_class != 5 && jo::self<jo::UnitType>(type)->unit_class != 6) {
                const i32 r = rnd(0xa);
                if (r < 1) {
                    order(id, 0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
                } else if (r < 5) {
                    const i32 e = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(v10)));
                    order(id, 0xa, 1, 0, e, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 1, 0, 0, 0, 0x5dc);
                } else if (r < 9) {
                    const i32 e = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(v10)));
                    order(id, 0xa, 1, 0, e, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 2, 0, 0, 0, 0x5dc);
                } else if (r < 10) {
                    const i32 e = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(v10)));
                    order(id, 0xa, 1, 0, e, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
                }
                return 1;
            }
        }
        if (v10 > 0) {
            order(id, 0xc, 1, 0, v10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        joan::orig::script::set_target_mode(joan::orig::arg::i32(w), str(d::s_NEAREST_HIGH_PRIORITY));  // "NEAREST_HIGH_PRIORITY"
        if (v1c != 0) {
            const i32 n = count_of(id, 2);
            v10 = group_find(w, str(d::s_EP), n, 1, g20, 0, 0);
        } else {
            const i32 n = count_of(id, 2);
            v10 = group_find(w, str(d::s_EP), n, 1, g20, 0, 0);
        }
        if (v10 > 0) {
            order(id, 0xc, 1, 1, v10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (v1c != 0 && joan::orig::arg::as<i32>(joan::orig::script::group_contains(joan::orig::arg::i32(g20), joan::orig::arg::i32(v1c))) != 0) {  // 0x00653f30
            const i32 s = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(w), joan::orig::arg::i32(static_cast<i16>(v1c))));  // 0x00653150
            const i32 lim = count_of(id, 3);
            if (!(s > lim)) {
                order(id, 0xc, 1, 1, v1c, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        set_state(w, str(d::s_IDLE));  // "IDLE"
        order(id, 0xe, 0, 1, g20, 0, 0, 0, 0x7530);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_18", name, addr, fn)
B("UnitAI::think_attack", 0x0053e240, &H::think_attack);
#undef B

}  // namespace game_b106_18
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

JOAN_COUNTER(c_think_540070, "Ai::think_540070");

// ---- unit AI helpers (cdecl) ----
i32 script_value(i32 id, img::Addr name, i32 a, i32 b, i32 c, i32 d, i32 e) {
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(static_cast<i16>(id)), str(name), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));  // 0x006517c0
}
u32 order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    return joan::orig::arg::as<u32>(joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));  // 0x00661340
}
i32 stat_b280(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }  // 0x0065b280
void set_state(i32 id, img::Addr name) {
    joan::orig::script::set_mode(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::str(str(name)));  // 0x006515d0
}
constexpr img::Addr kAttack = d::s_ATTACK_9cf380;  // "ATTACK"
constexpr img::Addr kActive = d::s_ACTIVE;  // "ACTIVE"
constexpr img::Addr kIdle = d::s_IDLE;    // "IDLE"
constexpr img::Addr kEnemy = d::s_E_9cfc2c;

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x00540070
    u32 think_540070(i16* arg) {
        c_think_540070.hit();
        const i32 id = *arg;
        i32 v10 = 0, v14 = 0, v18 = 0, v1c = 0;
        v1c = script_value(id, d::s_T, 0, 0, 0, 0, 0);
        if (joan::orig::arg::as<i32>(joan::orig::script::leader_id(joan::orig::arg::i32(id))) > 0) {  // 0x006569f0
            v10 = script_value(id, d::s_L, 0x3e8, 0, 0, 0, 0);
            if (v10 == 0) {
                order(id, 0xf, 2, 1, 0, 0, 0, 0, 0x2710);
                joan::orig::script::set_2d7(joan::orig::arg::i32(static_cast<i16>(id)), i32{0});  // 0x006558d0
                return 1;
            }
            if (v1c == 0) order(id, 0xf, -2, 1, 0, 0, 0, 0, 0x2710);
        }
        // both branches of `if (v1c != 0)` are identical
        v10 = script_value(id, kEnemy, stat_b280(id, 2), 0, 0, 0, 0x41);
        v14 = script_value(id, kEnemy, stat_b280(id, 1), 0, 0, 0, 0x41);
        {
            u8* const kind = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(arg)->type_id))));  // 0x0042d0a0
            // _stricmp: the EGC lookups run only for ENGLISH_EX_ARCHER (jne skips them).
            if (joan::crt::stricmp(joan::orig::arg::str(kind + 4), str(d::s_ENGLISH_EX_ARCHER)) == 0) {  // "ENGLISH_EX_ARCHER"
                v18 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(static_cast<i16>(id)), str(d::s_EGC), joan::orig::arg::u32(stat_b280(id, 2)), joan::orig::arg::u32(i32{0})));  // 0x006519d0 "EGC"
                if (v18 == 0)
                    v18 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(static_cast<i16>(id)), str(d::s_EK), joan::orig::arg::u32(stat_b280(id, 2)), joan::orig::arg::u32(i32{0})));  // 0x006519d0
            }
        }
        if (v14 != 0) {
            order(id, 0xc, 1, 0, v14, stat_b280(id, 1), 0x190, 0, 0x7530);
            return 1;
        }
        if (v10 != 0 || v18 != 0) set_state(id, kAttack);
        if (v10 == 0 && v18 == 0) set_state(id, kActive);
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(static_cast<i16>(id)))) != 0) {  // 0x00651590
            if (v10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(arg)->line_of_sight(joan::orig::arg::i32(v10))) == 0) {  // 0x005130a0
                    const i32 v = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::i32(v10)));  // 0x00652ee0
                    order(id, 0xa, 3, 0, v, 0, 0, 0, 0x9c4);
                    joan::orig::as_order(me())->run_a(joan::orig::arg::mptr(arg), i32{1}, joan::orig::arg::i32(v10));  // 0x00541270
                    return 1;
                }
                set_state(id, kActive);
            }
            if (v18 != 0) {
                const i32 v = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::i32(v18)));  // 0x00653000
                order(id, 0xa, 3, 0, v, 0, 0, 0, 0x9c4);
                joan::orig::as_order(me())->run_a(joan::orig::arg::mptr(arg), i32{2}, joan::orig::arg::i32(v18));  // 0x00541270
                return 1;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(static_cast<i16>(id)))) == 0) return 1;  // 0x00651550
        if (joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(static_cast<i16>(id)), i32{1})) == 0) {  // 0x00652190
            // both branches of `if (v1c != 0)` are identical
            v10 = script_value(id, kEnemy, stat_b280(id, 2), 1, 0, 0, 0x41);
            if (v10 > 0) {
                order(id, 0xc, 1, 0, v10, stat_b280(id, 2) - 0x190, 0x190, 0, 0x7530);
                return 1;
            }
            if (v1c != 0) {
                const i32 s = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(static_cast<i16>(id)), joan::orig::arg::i32(static_cast<i16>(v1c))));  // 0x00653150
                const i32 t = stat_b280(id, 3);
                if (!(s > t)) {
                    order(id, 0xc, 1, 1, v1c, stat_b280(id, 2) - 0x190, 0x190, 0, 0x7530);
                    return 1;
                }
            }
        }
        set_state(id, kIdle);
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Ai::think_540070", 0x00540070, &H::think_540070);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

JOAN_COUNTER(c_nearest_archer, "Script::nearest_archer");
JOAN_COUNTER(c_archer_cover, "Script::archer_cover");

// ------------------------------------------------------------------ script helpers (cdecl)
u32 group_of(i32 id, i32 n) { return joan::orig::arg::as<u32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(n))); }  // 0x0065b280
void set_mode(i32 id, const char* mode) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(mode)); }  // 0x006515d0
void order(i32 id, i32 a, i32 b, i32 c, u32 d, u32 e, u32 f, i32 g, i32 h) {  // 0x00661340
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));
}

struct H {
    // ------------------------------------------------------------------ 0x00540c10
    i32 nearest_archer(u8* unit) {
        c_nearest_archer.hit();
        const i32 id = jo::as_unit(unit)->id;
        i32 target = 0;
        i32 egc = 0;
        const u32 sel = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{1}));  // 0x00652190
        joan::orig::script::set_target_mode(joan::orig::arg::i32(id), str(d::s_NEAREST_ARCHER));          // 0x00651690 "NEAREST_ARCHER"
        target = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(d::s_E_9cfc2c), joan::orig::arg::i32(sel), i32{0}, i32{0}, i32{0}, i32{0x41}));  // 0x006517c0
        const char* const type_name =
            joan::orig::arg::as<const char*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id)))) + 4;  // 0x0042d0a0
        if (joan::crt::stricmp(joan::orig::arg::str(type_name), str(d::s_ENGLISH_EX_ARCHER)) == 0) {  // strcmp "ENGLISH_EX_ARCHER"
            u32 g = group_of(id, 2);
            egc = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), str(d::s_EGC), joan::orig::arg::u32(g), joan::orig::arg::u32(i32{0})));  // 0x006519d0 "EGC"
            if (egc == 0) {
                g = group_of(id, 2);
                egc = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), str(d::s_EK), joan::orig::arg::u32(g), joan::orig::arg::u32(i32{0})));  // "EK"
            }
        }
        if (target != 0 || egc != 0) set_mode(id, str(d::s_ATTACK_9cf380));  // "ATTACK"
        if (target == 0 && egc == 0) set_mode(id, str(d::s_ACTIVE));   // "ACTIVE"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) == 0) return 1;           // 0x00651590
        if (target != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(target))) == 0) {  // 0x005130a0
            const u32 e = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(target)));           // 0x00652ee0
            order(id, 0xa, 3, 0, e, 0, 0, 0, 0x9c4);
            joan::orig::as_order(this)->run_a(joan::orig::arg::mptr(unit), i32{1}, joan::orig::arg::i32(target));  // 0x00541270
        }
        if (egc != 0) {
            const u32 e = joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(egc)));  // 0x00653000
            order(id, 0xa, 3, 0, e, 0, 0, 0, 0x9c4);
            joan::orig::as_order(this)->run_a(joan::orig::arg::mptr(unit), i32{2}, joan::orig::arg::i32(egc));
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00543330
    i32 archer_cover(u8* unit) {
        c_archer_cover.hit();
        const i32 id = jo::as_unit(unit)->id;
        i32 target = 0;
        i32 cover = 0;
        i32 never = 0;  // [ebp-0x18]: never assigned in the original
        joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(d::s_T), i32{0}, i32{0}, i32{0}, i32{0}, i32{0});  // 0x006517c0
        u32 g = group_of(id, 2);
        target = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(d::s_E_9cfc2c), joan::orig::arg::i32(g), i32{0}, i32{0}, i32{0}, i32{0x41}));
        g = group_of(id, 1);
        cover = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(d::s_E_9cfc2c), joan::orig::arg::i32(g), i32{0}, i32{0}, i32{0}, i32{0x41}));
        if (cover != 0) {
            g = group_of(id, 1);
            order(id, 0xc, 1, 0, static_cast<u32>(cover), g, 0x190, 0, 0x7530);
            return 1;
        }
        if (target != 0 || never != 0) set_mode(id, str(d::s_ATTACK_9cf380));  // "ATTACK"
        if (target == 0 && never == 0) set_mode(id, str(d::s_ACTIVE));   // "ACTIVE"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {                     // 0x00651590
            if (target != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(target))) == 0) {  // 0x005130a0
                    const u32 e = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(target)));  // 0x00652ee0
                    order(id, 0xa, 3, 0, e, 0, 0, 0, 0x9c4);
                    joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), i32{1}, joan::orig::arg::i32(target));  // 0x005432a0
                    return 1;
                }
                set_mode(id, str(d::s_ACTIVE));
            }
            if (never != 0) {
                const u32 e = joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(never)));  // 0x00653000
                order(id, 0xa, 3, 0, e, 0, 0, 0, 0x9c4);
                joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), i32{2}, joan::orig::arg::i32(never));
                return 1;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) != 0) {  // 0x00651550
            const u32 a = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{1}));  // 0x00652190
            const u32 b = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{2}));
            const u32 c = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{3}));
            order(id, 7, 0, 0, a, b, c, 0, 0x7530);
            if (joan::orig::arg::as<i32>(joan::orig::unit::distance_to(joan::orig::arg::i16(id), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c))) <= 0xc8) joan::orig::script::unit_528300(joan::orig::arg::i32(id));  // 0x0065b800 / 0x00652370
            set_mode(id, str(d::s_IDLE));  // "IDLE"
        }
        return 1;
    }
};

JOAN_REPLACE("game/b108_07", "Script::nearest_archer", 0x00540c10, &H::nearest_archer);
JOAN_REPLACE("game/b108_07", "Script::archer_cover", 0x00543330, &H::archer_cover);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

JOAN_COUNTER(c_540e80, "Unit::attack_540e80");

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x00540e80
    static void order_attack(i32 id, i32 target) {
        // pushes 0x7530, 0, 0x190 come before the 0x0065b280 call; only the call order matters.
        const i32 w = joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), i32{2})) - 0x190;  // 0x0065b280
        joan::orig::group_order(joan::orig::arg::i32(id), i32{0xc}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(target), joan::orig::arg::i32(w), i32{0x190}, i32{0}, i32{0x7530});  // 0x00661340
    }
    u32 attack_540e80(i16* p) {
        c_540e80.hit();
        const i32 id = *p;  // word args below are read as words by every callee
        const i32 t = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{1}));  // 0x00652190
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(id))) != 0 ||               // 0x006512d0
            joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(t)) != 0 ||  // 0x00428b70
            joan::orig::arg::as<i32>(joan::orig::unit::idle(joan::orig::arg::i32(t))) != 0) {                // 0x0065c8d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(id));                      // 0x00652370
            return 1;
        }
        const i32 s = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), static_cast<i32>(static_cast<i16>(t))));  // 0x00653150
        if (s >= joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), i32{2}))) {  // 0x0065b280
            order_attack(id, t);
            return 1;
        }
        joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ATTACK_9cf380));  // 0x006515d0 "ATTACK"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {         // 0x00651590
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(p)->line_of_sight(joan::orig::arg::i32(t))) == 0) {     // 0x005130a0
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(t)));  // 0x00652ee0
                joan::orig::group_order(joan::orig::arg::i32(id), i32{0xa}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{4})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(r), i32{0}, i32{0}, i32{0}, i32{0x9c4});  // 0x00661340
                joan::orig::as_order(this)->run_a(joan::orig::arg::mptr(p), i32{1}, joan::orig::arg::i32(t));  // 0x00541270
            } else {
                joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ACTIVE));  // "ACTIVE"
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) != 0) order_attack(id, t);  // 0x00651550
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Unit::attack_540e80", 0x00540e80, &H::attack_540e80);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

JOAN_COUNTER(c_attack_step, "Order::attack_step");

// 0x00661340 order dispatch used by 0x005410b0 twice
void order_attack_move(i32 v, i32 c) {
    const i32 t = joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(v), i32{2})) - 0x190;  // 0x0065b280
    joan::orig::group_order(joan::orig::arg::i32(v), i32{0xd}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(c), joan::orig::arg::i32(t), i32{0x190}, i32{0}, i32{0x7530});  // 0x00661340
}

struct H {
    // ------------------------------------------------------------------------ 0x005410b0
    i32 attack_step(void* unit) {
        c_attack_step.hit();
        const i32 v = static_cast<i32>(jo::as_unit(unit)->id);
        const i32 c = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(v), i32{1}));  // 0x00652190
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(v))) != 0) {             // 0x006512d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(v));                    // 0x00652370
            return 1;
        }
        const i32 have = joan::orig::arg::as<i32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(v), joan::orig::arg::i32(c)));  // 0x00653060
        if (!(have < joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(v), i32{2})))) {  // 0x0065b280
            order_attack_move(v, c);
            return 1;
        }
        joan::orig::script::set_mode(joan::orig::arg::i32(v), str(d::s_ATTACK_9cf380));  // 0x006515d0 "ATTACK"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(v))) != 0) {        // 0x00651590
            if (joan::orig::arg::as<i32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(c))) == 0) {    // 0x0065b6a0
                joan::orig::script::unit_528300(joan::orig::arg::i32(v));           // 0x00652370
                return 1;
            }
            joan::orig::as_order(this)->run_a(joan::orig::arg::mptr(unit), i32{2}, joan::orig::arg::i32(c));  // 0x00541270
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(v))) != 0) order_attack_move(v, c);  // 0x00651550
        return 1;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("Order::attack_step", 0x005410b0, &H::attack_step);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

void* const kRandom = obj(d::g_misc);

i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
i32 order_661340(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {  // 0x00661340
    return joan::orig::arg::as<i32>(joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));
}
// Order handlers 0x00541270 / 0x005432a0.
i32 run_rand_morale(void* unit, i32 a, i32 b) {
    const i32 r = random(10);
    const i32 m = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->scaled_2df()));  // 0x00511f80
    if (r < m) {
        jo::view<u16>(jo::as_unit(unit)->attack_chance) = joan::orig::arg::as<u16>(joan::orig::as_unit(unit)->recruit_delay());  // 0x00511e70
        order_661340(static_cast<i32>(jo::as_unit(unit)->id), 2, 2, 0, a, b, 0, 0, 0x5dc);
    }
    return 1;
}

struct H {
    // ------------------------------------------------------------------------ order handlers
    i32 run_a(void* unit, i32 a, i32 b) { return run_rand_morale(unit, a, b); }  // 0x00541270
    i32 run_b(void* unit, i32 a, i32 b) { return run_rand_morale(unit, a, b); }  // 0x005432a0
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Order::run_a", 0x00541270, &H::run_a);
B92("Order::run_b", 0x005432a0, &H::run_b);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

JOAN_COUNTER(c_ai541, "Ai::state_541600");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x00541600
    u32 ai_state(void* unit) {
        c_ai541.hit();
        const i32 id = jo::as_unit(unit)->id;
        auto find = [&](img::Addr what, i32 a3, i32 a4, i32 a7) {
            return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), img::str(what), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), i32{0}, i32{0}, joan::orig::arg::i32(a7)));  // 0x006517c0
        };
        auto range = [&](i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); };  // 0x0065b280
        auto order = [&](i32 a2, i32 a3, i32 a4, i32 a5, i32 a6, i32 a7, i32 a8, i32 a9) {
            joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a2), static_cast<std::int8_t>(joan::orig::arg::i32(a3)), static_cast<std::int8_t>(joan::orig::arg::i32(a4)), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9));  // 0x00661340
        };
        auto set_state = [&](img::Addr name) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(img::addr(name))); };  // 0x006515d0
        i32 p1c = find(d::s_T, 0, 0, 0);
        i32 p10 = find(d::s_E_9cfc2c, range(2), 0, 0x41);  // both branches of the original are identical
        const i32 p14 = find(d::s_E_9cfc2c, range(1), 0, 0x41);
        i32 p18 = 0;
        const u8* nm = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
        if (joan::crt::stricmp(joan::orig::arg::str(nm + 4), str(d::s_ENGLISH_EX_ARCHER_CAPTAIN)) == 0) {  // _stricmp ENGLISH_EX_ARCHER_CAPTAIN
            p18 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), str(d::s_EGC), joan::orig::arg::u32(range(2)), joan::orig::arg::u32(i32{0})));  // 0x006519d0 "EGC"
            if (p18 == 0) p18 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), str(d::s_EK), joan::orig::arg::u32(range(2)), joan::orig::arg::u32(i32{0})));
        }
        if (p14 != 0) {
            const i32 r = range(1);
            order(0xc, 1, 0, p14, r, 0x190, 0, 0x7530);
            return 1;
        }
        if (p10 != 0 || p18 != 0) set_state(d::s_ATTACK_9cf380);   // "ATTACK"
        if (p10 == 0 && p18 == 0) set_state(d::s_ACTIVE);   // "ACTIVE"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {          // 0x00651590
            if (p10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(p10))) == 0) {  // 0x005130a0
                    const i32 r = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(p10)));  // 0x00652ee0
                    order(0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                    joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), i32{1}, joan::orig::arg::i32(p10));  // 0x005432a0
                    return 1;
                }
                set_state(d::s_ACTIVE);  // "ACTIVE"
            }
            if (p18 != 0) {
                const i32 r = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(p18)));  // 0x00653000
                order(0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), i32{2}, joan::orig::arg::i32(p18));
                return 1;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) == 0) return 1;  // 0x00651550
        if (joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{1})) == 0) {  // 0x00652190
            p10 = find(d::s_E_9cfc2c, range(2), 1, 0x41);
            if (p10 > 0) {
                const i32 r = range(2);
                order(0xc, 1, 0, p10, r - 0x190, 0x190, 0, 0x7530);
                return 1;
            }
            if (p1c != 0) {
                const i32 a = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), joan::orig::arg::i32(p1c)));  // 0x00653150 (words)
                const i32 b = range(3);
                if (!(a > b)) {
                    const i32 r = range(2);
                    order(0xc, 1, 1, p1c, r - 0x190, 0x190, 0, 0x7530);
                    return 1;
                }
            }
        }
        set_state(d::s_IDLE);  // "IDLE"
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Ai::state_541600", 0x00541600, &H::ai_state);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

JOAN_COUNTER(c_think_archer, "Ai::think_archer");

// ---- script helpers (cdecl, through their thunks) ----------------------------------------
i32 unit_slot(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }  // 0x00652190
i32 unit_stat(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }  // 0x0065b280
i32 find_target(i32 id, const char* tag, i32 a, i32 b, i32 c, i32 d, i32 e) {
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), tag, joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));  // 0x006517c0
}
i32 find_near(i32 id, const char* tag, i32 a, i32 b) {
    return joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), tag, joan::orig::arg::u32(a), joan::orig::arg::u32(b)));  // 0x006519d0
}
void set_state(i32 id, const char* state) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(state)); }  // 0x006515d0
i32 is_ready(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))); }                      // 0x00651590
i32 is_idle(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))); }                       // 0x00651550
void order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}
const char* const kAttack = str(d::s_ATTACK_9cf380);  // "ATTACK"
const char* const kActive = str(d::s_ACTIVE);  // "ACTIVE"
const char* const kIdle = str(d::s_IDLE);    // "IDLE"

struct H {
    // ------------------------------------------------------------------ 0x00541b10
    i32 think_archer(u8* unit) {
        c_think_archer.hit();
        const i32 id = jo::as_unit(unit)->id;
        const i32 v1c = find_target(id, str(d::s_T), 0, 0, 0, 0, 0);
        const i32 v20 = unit_slot(id, 1);
        joan::orig::script::group_angle(joan::orig::arg::i32(v20));  // 0x006547a0, result unused
        i32 v10 = find_target(id, str(d::s_EP), unit_stat(id, 2), 0, v20, 0, 0x41);  // both arms identical
        const i32 v14 = find_target(id, str(d::s_EP), unit_stat(id, 1), 0, v20, 0, 0x41);
        i32 v18 = 0;
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
        if (joan::crt::stricmp(joan::orig::arg::str(type + 4), str(d::s_ENGLISH_EX_ARCHER_CAPTAIN)) == 0) {  // _stricmp ENGLISH_EX_ARCHER_CAPTAIN
            v18 = find_near(id, str(d::s_EGC), unit_stat(id, 2), 0);     // "EGC"
            if (v18 == 0) v18 = find_near(id, str(d::s_EK), unit_stat(id, 2), 0);
        }
        if (v14 != 0) {
            order(id, 0xc, 1, 0, v14, unit_stat(id, 1), 0x190, 0, 0x7530);
            return 1;
        }
        if (v10 != 0 || v18 != 0) set_state(id, kAttack);
        if (v10 == 0 && v18 == 0) set_state(id, kActive);
        if (is_ready(id) != 0) {
            joan::orig::script::group_set_angle(joan::orig::arg::i32(v20), 0);  // 0x006547e0, result unused
            if (v10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(v10))) == 0) {               // 0x005130a0
                    const i32 r = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(v10)));         // 0x00652ee0
                    order(id, 0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                    joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), 1, joan::orig::arg::i32(v10));  // 0x005432a0
                    return 1;
                }
                set_state(id, kActive);
            }
            if (v18 != 0) {
                const i32 r = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(v18)));  // 0x00653000
                order(id, 0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), 2, joan::orig::arg::i32(v18));  // 0x005432a0
                return 1;
            }
        }
        if (is_idle(id) != 0) {
            v10 = find_target(id, str(d::s_EP), unit_stat(id, 2), 1, v20, 0, 0x41);  // both arms identical
            if (v10 > 0) {
                order(id, 0xc, 1, 0, v10, static_cast<i32>(static_cast<u32>(unit_stat(id, 2)) - 0x190u), 0x190, 0,
                      0x7530);
                return 1;
            }
            if (v1c != 0 && joan::orig::arg::as<i32>(joan::orig::script::group_contains(joan::orig::arg::i32(v20), joan::orig::arg::i32(v1c))) != 0) {  // 0x00653f30
                const i32 n = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), static_cast<i32>(static_cast<i16>(v1c))));  // 0x00653150
                if (!(n > unit_stat(id, 3))) {
                    order(id, 0xc, 1, 1, v1c, static_cast<i32>(static_cast<u32>(unit_stat(id, 2)) - 0x190u), 0x190,
                          0, 0x7530);
                    return 1;
                }
            }
            set_state(id, kIdle);
            if (v10 == 0) order(id, 0xe, 0, 1, v20, 0, 0, 0, 0x7530);
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Ai::think_archer", 0x00541b10, &H::think_archer);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

JOAN_COUNTER(c_5425d0, "ArcherCaptain::setup_5425d0");

struct H {
    // 0x005425d0
    i32 archer_captain_setup(u8* unit) {
        c_5425d0.hit();
        const i32 id = jo::as_unit(unit)->id;
        const i16 w = static_cast<i16>(id);
        i32 r10 = 0, r14 = 0;
        const u32 v18 = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(w), 1));                                 // 0x00652190
        joan::orig::script::set_target_mode(joan::orig::arg::i32(w), str(d::s_NEAREST_ARCHER));                                   // 0x00651690 'NEAREST_ARCHER'
        r10 = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(w), str(d::s_E_9cfc2c), joan::orig::arg::i32(v18), 0, 0, 0, 0x41));         // 0x006517c0
        const char* const kind = joan::orig::arg::as<const char*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id))) + 4;  // 0x0042d0a0
        if (joan::crt::stricmp(joan::orig::arg::str(kind), str(d::s_ENGLISH_EX_ARCHER_CAPTAIN)) == 0) {      // 'ENGLISH_EX_ARCHER_CAPTAIN'
            r14 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(w), str(d::s_EGC), joan::orig::arg::u32(v18), 0u));  // 0x006519d0 'EGC'
            if (r14 == 0) r14 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(w), str(d::s_EK), joan::orig::arg::u32(v18), 0u));
        }
        if (r10 != 0 || r14 != 0) joan::orig::script::set_mode(joan::orig::arg::i32(w), str(d::s_ATTACK_9cf380));  // 0x006515d0 'ATTACK'
        if (r10 == 0 && r14 == 0) joan::orig::script::set_mode(joan::orig::arg::i32(w), str(d::s_ACTIVE));   // 'ACTIVE'
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(w))) == 0) return 1;                          // 0x00651590
        if (r10 != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(r10))) == 0) {                // 0x005130a0
            const i32 g = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(w), joan::orig::arg::i32(r10)));                      // 0x00652ee0
            joan::orig::group_order(joan::orig::arg::i32(id), 10, static_cast<std::int8_t>(joan::orig::arg::i32(3)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(g), 0, 0, 0, 0x9c4);            // 0x00661340
            joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), 1, joan::orig::arg::i32(r10));                               // 0x005432a0
        }
        if (r14 != 0) {
            const i32 g = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(w), joan::orig::arg::i32(r14)));            // 0x00653000
            joan::orig::group_order(joan::orig::arg::i32(id), 10, static_cast<std::int8_t>(joan::orig::arg::i32(3)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(g), 0, 0, 0, 0x9c4);  // 0x00661340
            joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), 2, joan::orig::arg::i32(r14));                     // 0x005432a0
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("ArcherCaptain::setup_5425d0", 0x005425d0, &H::archer_captain_setup);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b109_06.cpp (game/b109_06)
namespace {
namespace game_b109_06 {

using joan::hook::x87::ftol;

u8* unit_type(void* u) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id))); }  // 0x0042d0a0
i32 unit_missing(u32 k) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(k)); }  // 0x00428b70
u8* unit_rec(u32 k) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(k))); }  // 0x0041b3c0

JOAN_COUNTER(c_ai542, "Ai::state_542820");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // ------------------------------------------------------------------ 0x00542820
    u32 ai_state(void* unit) {
        c_ai542.hit();
        const i32 id = jo::as_unit(unit)->id;
        auto find = [&](img::Addr what, i32 a3, i32 a4, i32 a7) {
            return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), img::str(what), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), i32{0}, i32{0}, joan::orig::arg::i32(a7)));  // 0x006517c0
        };
        auto range = [&](i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); };  // 0x0065b280
        auto order = [&](i32 a2, i32 a3, i32 a4, i32 a5, i32 a6, i32 a7, i32 a8, i32 a9) {
            joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a2), static_cast<std::int8_t>(joan::orig::arg::i32(a3)), static_cast<std::int8_t>(joan::orig::arg::i32(a4)), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9));  // 0x00661340
        };
        auto set_state = [&](img::Addr name) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(img::addr(name))); };  // 0x006515d0
        const i32 target = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{1}));  // 0x00652190
        i32 limit = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{2}));
        if (limit == 0) limit = 0xb4;
        i32 dist = 0;
        if (unit_missing(static_cast<u32>(target)) == 0) {
            u8* const t = unit_rec(static_cast<u32>(target));
            const float dx1 = jo::view<float[3]>(jo::as_unit(unit)->position_)[0] - jo::view<float[3]>(jo::as_unit(t)->position_)[0];
            const float dx2 = jo::view<float[3]>(jo::as_unit(unit)->position_)[0] - jo::view<float[3]>(jo::as_unit(t)->position_)[0];
            const float xx = dx1 * dx2;
            const float dy1 = jo::view<float[3]>(jo::as_unit(unit)->position_)[1] - jo::view<float[3]>(jo::as_unit(t)->position_)[1];
            const float dy2 = jo::view<float[3]>(jo::as_unit(unit)->position_)[1] - jo::view<float[3]>(jo::as_unit(t)->position_)[1];
            const float yy = dy1 * dy2;
            const float sq = xx + yy;
            dist = ftol(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sq)))));  // CRT sqrt, _ftol
        }
        const i32 reach = joan::orig::arg::as<i32>(joan::orig::unit::rank_value(joan::orig::arg::i16(target)));  // 0x006592c0 (word)
        if (dist > reach + 0x96) {
            joan::orig::group_order(joan::orig::arg::i32(id), i32{0x10}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{-1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(target), joan::orig::arg::i32(limit), joan::orig::arg::i32(reach), i32{0}, i32{0x1388});  // 0x00661340
            joan::orig::script::set_2d7(joan::orig::arg::i32(id), i32{0});  // 0x006558d0
            return 1;
        }
        const i32 p1c = find(d::s_T, 0, 0, 0);
        i32 p10 = find(d::s_E_9cfc2c, range(2), 0, 0x41);  // both branches of the original are identical
        const i32 p14 = find(d::s_E_9cfc2c, range(1), 0, 0x41);
        i32 p18 = 0;
        const u8* nm = unit_type(unit);
        if (joan::crt::stricmp(joan::orig::arg::str(nm + 4), str(d::s_ENGLISH_EX_ARCHER_CAPTAIN)) == 0) {  // _stricmp ENGLISH_EX_ARCHER_CAPTAIN
            p18 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), str(d::s_EGC), joan::orig::arg::u32(range(2)), joan::orig::arg::u32(i32{0})));  // 0x006519d0 "EGC"
            if (p18 == 0) p18 = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), str(d::s_EK), joan::orig::arg::u32(range(2)), joan::orig::arg::u32(i32{0})));
        }
        if (p14 != 0) {
            const i32 r = range(1);
            order(0xc, 1, 0, p14, r, 0x190, 0, 0x7530);
            return 1;
        }
        if (p10 != 0 || p18 != 0) set_state(d::s_ATTACK_9cf380);  // "ATTACK"
        if (p10 == 0 && p18 == 0) set_state(d::s_ACTIVE);  // "ACTIVE"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {         // 0x00651590
            if (p10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(p10))) == 0) {          // 0x005130a0
                    const i32 r = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(p10)));  // 0x00652ee0
                    order(0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                    joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), i32{1}, joan::orig::arg::i32(p10));  // 0x005432a0
                    return 1;
                }
                set_state(d::s_ACTIVE);  // "ACTIVE"
            }
            if (p18 != 0) {
                const i32 r = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(p18)));  // 0x00653000
                order(0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), i32{2}, joan::orig::arg::i32(p18));
                return 1;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) == 0) return 1;  // 0x00651550
        p10 = find(d::s_E_9cfc2c, range(2), 1, 0x41);           // both branches identical
        if (p10 > 0) {
            const i32 r = range(2);
            order(0xc, 1, 0, p10, r - 0x190, 0x190, 0, 0x7530);
            return 1;
        }
        if (p1c != 0) {
            const i32 a = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), joan::orig::arg::i32(p1c)));  // 0x00653150 (words)
            const i32 b = range(3);
            if (!(a > b)) {
                const i32 r = range(2);
                order(0xc, 1, 1, p1c, r - 0x190, 0x190, 0, 0x7530);
                return 1;
            }
        }
        set_state(d::s_IDLE);  // "IDLE"
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_06", name, addr, fn)
B("Ai::state_542820", 0x00542820, &H::ai_state);
#undef B

}  // namespace game_b109_06
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

JOAN_COUNTER(c_attack, "Ai::attack_or_activate");

struct H {
    // ------------------------------------------------------------------------ 0x00542e60
    void spawn_attack(i16 id, i32 u) {  // 0x00661340(id, 0xc, 1, 0, u, n - 0x190, 0x190, 0, 30000)
        const i32 n = joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(static_cast<i32>(id), 2));  // 0x0065b280
        joan::orig::group_order(static_cast<i32>(id), 0xc, static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(u), joan::orig::arg::i32(n - 0x190), 0x190, 0, 0x7530);
    }
    u32 attack_or_activate(void* unit) {
        c_attack.hit();
        const i16 id = jo::as_unit(unit)->id;
        const i32 u = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), 1));  // 0x00652190
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(id))) != 0 ||          // 0x006512d0
            joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(u)) != 0 ||  // 0x00428b70
            joan::orig::arg::as<i32>(joan::orig::unit::idle(joan::orig::arg::i32(u))) != 0) {           // 0x0065c8d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(id));                 // 0x00652370
            return 1;
        }
        const i32 have = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), joan::orig::arg::i32(static_cast<i16>(u))));  // 0x00653150
        if (!(have < joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(static_cast<i32>(id), 2)))) {   // 0x0065b280
            spawn_attack(id, u);
            return 1;
        }
        joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ATTACK_9cf380));  // 0x006515d0 "ATTACK"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {         // 0x00651590
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(u))) == 0) {  // 0x005130a0
                const i32 t = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(u)));  // 0x00652ee0
                joan::orig::group_order(static_cast<i32>(id), 0xa, static_cast<std::int8_t>(joan::orig::arg::i32(4)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(t), 0, 0, 0, 0x9c4);  // 0x00661340
                joan::orig::as_order(this)->run_b(joan::orig::arg::mptr(unit), 1, joan::orig::arg::i32(u));  // 0x005432a0
            } else {
                joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ACTIVE));  // 0x006515d0 "ACTIVE"
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) != 0) spawn_attack(id, u);  // 0x00651550
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("Ai::attack_or_activate", 0x00542e60, &H::attack_or_activate);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

JOAN_COUNTER(c_attack_step, "Ai::attack_step");

// ---- script AI helpers (cdecl, short ids read as words by the callees) ----
i32 formation_slot(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }  // 0x00652190
i32 group_order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    return joan::orig::arg::as<i32>(joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));  // 0x00661340
}
i32 range_for_level(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }  // 0x0065b280

struct H {
    // ------------------------------------------------------------------------ 0x00543090
    i32 attack_step(const i16* pid) {
        c_attack_step.hit();
        const i32 id = static_cast<i32>(*pid);
        const i32 slot = formation_slot(id, 1);
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(id))) != 0) {  // 0x006512d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(id));         // 0x00652370
            return 1;
        }
        const i32 r = joan::orig::arg::as<i32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(id), joan::orig::arg::i32(slot)));  // 0x00653060
        if (r < range_for_level(id, 2)) {
            joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ATTACK_9cf380));  // 0x006515d0 "ATTACK"
        } else {
            const i32 rr = range_for_level(id, 2);
            group_order(id, 0xd, 1, 0, slot, static_cast<i32>(static_cast<u32>(rr) - 0x190u), 0x190, 0, 0x7530);
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {     // 0x00651590
            if (joan::orig::arg::as<i32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(slot))) == 0) {  // 0x0065b6a0
                joan::orig::script::unit_528300(joan::orig::arg::i32(id));         // 0x00652370
                return 1;
            }
            const i32 t = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(slot)));  // 0x00653000
            group_order(id, 0xa, 4, 0, t, 0, 0, 0, 0x9c4);
            group_order(id, 2, 3, 0, 2, slot, 0, 0, 0x5dc);
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) != 0) {  // 0x00651550
            const i32 rr = range_for_level(id, 2);
            group_order(id, 0xd, 1, 0, slot, static_cast<i32>(static_cast<u32>(rr) - 0x190u), 0x190, 0, 0x7530);
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Ai::attack_step", 0x00543090, &H::attack_step);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

using joan::hook::x87::Fpu;

void JOAN_CDECL pi_a3cdb4() {  // 0x005436d0
    const Fpu fpu{true};
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_pi_a3cdb4) = static_cast<float>(fpu.mul(a, global<double>(d::dbl_4)));  // 4.0
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("Globals::pi_a3cdb4", 0x005436d0, &pi_a3cdb4);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

void JOAN_CDECL init_a3cdb8() {  // 0x00543730
    global<float>(d::g_a3cdb8) = global<float>(d::g_pi_a3cdb4) / global<float>(d::flt_2);  // 2.0f
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("init_a3cdb8", 0x00543730, &init_a3cdb8);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

void JOAN_CDECL div_a3cdbc() {  // 0x00543780
    global<float>(d::g_a3cdbc) = global<float>(d::g_pi_a3cdb4) / global<float>(d::flt_3);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("div_a3cdbc", 0x00543780, &div_a3cdbc);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

void JOAN_CDECL init_a3cdc0() { global<float>(d::g_a3cdc0) = global<float>(d::g_pi_a3cdb4) / 4.0f; }  // 0x005437d0

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("init_a3cdc0", 0x005437d0, &init_a3cdc0);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using joan::call::cdecl_call;

using joan::call::thiscall;
using joan::img::str;

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_think, "UnitAI::think_534880");

// ---- AI helpers (cdecl script functions, thunks) ----
i32 unit_query(i16 id, img::Addr kw, i32 a, i32 b, i32 c, i32 d, i32 e) {
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(kw), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));  // 0x006517c0
}
i32 order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    return joan::orig::arg::as<i32>(joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));  // 0x00661340
}
void set_state(i16 id, img::Addr kw) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(str(kw))); }  // 0x006515d0
i32 stat_b280(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }               // 0x0065b280
using joan::orig::helpers::random;  // 0x00590540
i32 aim_2ee0(i16 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }                // 0x00652ee0
i32 aim_3000(i16 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }                // 0x00653000
void set_target_mode(i16 id, img::Addr kw) { joan::orig::script::set_target_mode(joan::orig::arg::i32(id), joan::orig::arg::str(str(kw))); }  // 0x00651690

struct H {
    // 0x00534880
    i32 think(i16* idp) {
        c_think.hit();
        const i32 id = static_cast<i32>(*idp);
        const i16 sid = static_cast<i16>(id);
        i32 e10 = 0;
        i32 e14 = 0;
        const i32 e18 = 0;  // never written in the original
        i32 e1c = 0;
        i32 e20 = 0;
        i32 e24 = 0;
        const i32 r28 = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(sid), i32{1}));  // 0x00652190
        joan::orig::script::group_angle(joan::orig::arg::i32(r28));                          // 0x006547a0 ([ebp-0x2c], unused)
        e1c = unit_query(sid, d::s_T, 0, 0, 0, 0, 0);
        if (joan::orig::arg::as<i32>(joan::orig::script::leader_id(joan::orig::arg::i32(id))) > 0) {  // 0x006569f0
            e20 = unit_query(sid, d::s_L, 1000, 0, 0, 0, 0);
            if (e1c == 0) order(id, 0xf, -2, 1, 0, 0, 0, 0, 10000);
        }
        e10 = unit_query(sid, d::s_EP, 0x96, 0, r28, 0, 0);
        if (e10 != 0 || e18 != 0) set_state(sid, d::s_ATTACK_9cf380);  // "ATTACK"
        if (e10 == 0 && e18 == 0) {
            set_state(sid, d::s_ACTIVE);  // "ACTIVE"
            e10 = unit_query(sid, d::s_EP, 200, 0, r28, 0, 0);
            const i32 ec = random(0x32);
            if (e10 != 0 && ec == 0) set_state(sid, d::s_ATTACK_9cf380);  // "ATTACK"
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(sid))) != 0) {  // 0x00651590
            joan::orig::script::group_set_angle(joan::orig::arg::i32(r28), i32{0});  // 0x006547e0 ([ebp-0x2c], unused)
            if (e10 != 0) {
                const i32 v = aim_2ee0(sid, e10);
                order(id, 0xa, 3, 0, v, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<i32>(joan::orig::as_order(this)->flee_a(joan::orig::arg::mptr(idp)));  // 0x00535c20
            }
            if (e18 != 0) {
                const i32 v = aim_3000(sid, e18);
                order(id, 0xa, 3, 0, v, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<i32>(joan::orig::as_order(this)->flee_a(joan::orig::arg::mptr(idp)));  // 0x00535c20
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(sid))) == 0) return 1;  // 0x00651550
        if (e20 > 0) e24 = unit_query(static_cast<i16>(e20), d::s_T, 0, 0, 0, 0, 0);
        set_target_mode(sid, d::s_NEAREST_HIGH_PRIORITY);  // "NEAREST_HIGH_PRIORITY"
        {
            const i32 k = stat_b280(id, 1);
            e10 = unit_query(sid, d::s_EP, k, 1, r28, 0, 0);
        }
        {
            const i32 k = stat_b280(id, 1) / 2;
            e14 = unit_query(sid, d::s_EP, k, 0, r28, 0, 0);
        }
        if (e14 > 0) {
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(e14)));  // 0x0041b3c0
            u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));  // 0x0042d0a0
            if (static_cast<i32>(jo::self<jo::UnitType>(g)->unit_class) != 5 && static_cast<i32>(jo::self<jo::UnitType>(g)->unit_class) != 6) {
                const i32 r = random(10);
                if (r < 1) {
                    order(id, 0xc, 1, 0, e10, 0x3c, 0x3c, 0, 0x7530);
                } else if (r < 5) {
                    const i32 v = aim_2ee0(sid, e10);
                    order(id, 0xa, 1, 0, v, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 1, 0, 0, 0, 0x5dc);
                } else if (r < 9) {
                    const i32 v = aim_2ee0(sid, e10);
                    order(id, 0xa, 1, 0, v, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 2, 0, 0, 0, 0x5dc);
                } else if (r < 10) {
                    const i32 v = aim_2ee0(sid, e10);
                    order(id, 0xa, 1, 0, v, 0, 0, 0, 0x9c4);
                    order(id, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
                }
                return 1;
            }
        }
        if (e10 > 0) {
            order(id, 0xc, 1, 0, e10, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (e20 == 0 || e24 > 0) {
            set_target_mode(sid, d::s_NEAREST_HIGH_PRIORITY);  // "NEAREST_HIGH_PRIORITY"
            // both arms of the original's `[ebp-0x1c] != 0` test are identical
            const i32 k = stat_b280(id, 2);
            e10 = unit_query(sid, d::s_EP, k, 1, r28, 0, 0);
            if (e10 > 0) {
                order(id, 0xc, 1, 0, e10, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        if (e1c != 0 && joan::orig::arg::as<i32>(joan::orig::script::group_contains(joan::orig::arg::i32(r28), joan::orig::arg::i32(e1c))) != 0) {  // 0x00653f30
            const i32 d = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(sid), joan::orig::arg::i32(static_cast<i16>(e1c))));  // 0x00653150
            const i32 lim = stat_b280(id, 3);
            if (!(d > lim)) {
                order(id, 0xc, 1, 1, e1c, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        order(id, 0xe, -3, 0, r28, 0, 0, 0, 0x7530);
        set_state(sid, d::s_IDLE);  // "IDLE"
        return 1;
    }
};

JOAN_REPLACE(M, "UnitAI::think_534880", 0x00534880, &H::think);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- from modules/game_b106_12.cpp (game/b106_12)
namespace {
namespace game_b106_12 {

using joan::call::cdecl_call;

using joan::call::thiscall;
using joan::img::str;

using joan::img::obj;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_state_538730, "Ai::state_538730");
JOAN_COUNTER(c_state_53beb0, "Ai::state_53beb0");

// ------------------------------------------------------------------ AI helpers
void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kRandom = obj(d::g_misc);

const char* const kAttack = str(d::s_ATTACK_9cf380);       // "ATTACK"
const char* const kIdle = str(d::s_IDLE);         // "IDLE"
const char* const kActive = str(d::s_ACTIVE);       // "ACTIVE"
const char* const kNearestHigh = str(d::s_NEAREST_HIGH_PRIORITY);  // "NEAREST_HIGH_PRIORITY"
const char* const kEnemy = str(d::s_E_9cfc2c);        // "E"
const char* const kAny = str(d::s_T);
const char* const kLeader = str(d::s_L);       // "L"
const char* const kEnemyP = str(d::s_EP);       // "EP"

i32 sx(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }
i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
i32 nearest(i32 id, const char* kind, i32 range, i32 a, i32 b, i32 c, i32 d) {
    return joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), kind, joan::orig::arg::i32(range), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d)));  // 0x006517c0
}
i32 unit_target(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }   // 0x00652190
void unit_stop(i32 id) { joan::orig::script::unit_528300(joan::orig::arg::i32(id)); }                      // 0x00652370
void set_state(i32 id, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }   // 0x006515d0
void set_target_mode(i32 id, const char* s) { joan::orig::script::set_target_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }  // 0x00651690
i32 can_melee(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))); }               // 0x00651590
i32 can_move(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))); }                // 0x00651550
i32 range_b280(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }    // 0x0065b280
i32 aim_2ee0(i32 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }       // 0x00652ee0
i32 aim_3000(i32 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }       // 0x00653000
i32 check_1430(i32 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::in_arc(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }     // 0x00651430
i32 dist_b800(i32 id, i32 a, i32 b, i32 c) { return joan::orig::arg::as<i32>(joan::orig::unit::distance_to(joan::orig::arg::i16(id), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c))); }  // 0x0065b800
void order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}

// Type kind of unit t (word +0x3b of the unit record -> type record word +2).
i32 unit_type_kind(i32 t) {
    u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(t)));  // 0x0041b3c0
    u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->type_id))));  // 0x0042d0a0
    return static_cast<i32>(jo::self<jo::UnitType>(rec)->unit_class);
}

// rand(10) charge / formation orders on target t (both handlers, identical code).
void charge_orders(i32 id, i32 t) {
    const i32 r = random(10);
    if (r < 1) {
        order(id, 0xc, 1, 0, t, 0x3c, 0x3c, 0, 0x7530);
    } else if (r < 5) {
        order(id, 0xa, 1, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
        order(id, 6, 1, 0, 1, 0, 0, 0, 0x5dc);
    } else if (r < 9) {
        order(id, 0xa, 1, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
        order(id, 6, 1, 0, 2, 0, 0, 0, 0x5dc);
    } else if (r < 10) {
        order(id, 0xa, 1, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
        order(id, 6, 1, 0, 3, 0, 0, 0, 0x5dc);
    }
}

struct H {
    // ------------------------------------------------------------------ 0x00538730
    u32 state_538730(i16* unit) {
        c_state_538730.hit();
        const i32 id = static_cast<i32>(*unit);
        nearest(id, kAny, 0, 0, 0, 0, 0);  // stored to a dead local
        i32 t = nearest(id, kEnemy, 0x96, 0, 0, 0, 0);
        if (t != 0) {
            set_state(id, kAttack);
        } else {
            set_state(id, kActive);
            t = nearest(id, kEnemy, 0xc8, 0, 0, 0, 0);
            const i32 r = random(0x32);
            if (t != 0 && r == 0) set_state(id, kAttack);
        }
        if (can_melee(id) != 0 && t != 0) {
            order(id, 0xa, 3, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_538340(joan::orig::arg::mptr(unit)));  // 0x00538340
        }
        if (can_move(id) == 0) return 1;
        set_target_mode(id, kNearestHigh);
        const i32 r1 = range_b280(id, 1);
        t = nearest(id, kEnemy, r1, 1, 0, 0, 0);
        const i32 r2 = range_b280(id, 1);
        const i32 t2 = nearest(id, kEnemy, r2 / 2, 0, 0, 0, 0);
        if (t2 > 0) {
            const i32 kind = unit_type_kind(t2);
            if (kind != 5 && kind != 6) {
                charge_orders(id, t);
                return 1;
            }
        }
        if (t > 0) {
            order(id, 0xc, 1, 0, t, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        const i32 a = unit_target(id, 1);
        const i32 b = unit_target(id, 2);
        const i32 c = unit_target(id, 3);
        order(id, 7, 0, 0, a, b, c, 0, 0x7530);
        if (dist_b800(id, a, b, c) <= 0xc8) unit_stop(id);
        set_state(id, kIdle);
        return 1;
    }

    // ------------------------------------------------------------------ 0x0053beb0
    u32 state_53beb0(i16* unit) {
        c_state_53beb0.hit();
        const i32 id = static_cast<i32>(*unit);
        const i32 v18 = 0;  // [ebp-0x18]: initialised, never written
        i32 n24 = 0;
        const i32 t28 = unit_target(id, 1);
        joan::orig::script::group_angle(joan::orig::arg::i32(t28));  // 0x006547a0, result dead
        const i32 n1c = nearest(id, kAny, 0, 0, 0, 0, 0);
        i32 n20 = 0;
        if (joan::orig::arg::as<i32>(joan::orig::script::leader_id(joan::orig::arg::i32(id))) > 0) {  // 0x006569f0
            n20 = nearest(id, kLeader, 0x3e8, 0, 0, 0, 0);
            if (n1c == 0) order(id, 0xf, -2, 1, 0, 0, 0, 0, 0x2710);
        }
        i32 t = nearest(id, kEnemyP, 0x64, 0, t28, 0, 0);
        if (t != 0 || v18 != 0) set_state(id, kAttack);
        if (t == 0 && v18 == 0) {
            set_state(id, kActive);
            t = nearest(id, kEnemyP, 0x96, 0, t28, 0, 0);
            const i32 r = random(0x32);
            if (t != 0 && r == 0) set_state(id, kAttack);
        }
        if (can_melee(id) != 0) {
            joan::orig::script::group_set_angle(joan::orig::arg::i32(t28), i32{0});  // 0x006547e0, result dead
            if (t != 0) {
                if (check_1430(id, sx(t)) != 0) {
                    const i32 r = random(0x64);
                    const i32 m = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_game(unit)->scaled_a3cc90()));  // 0x00512070
                    if (r <= m) {
                        order(id, 0xa, 4, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
                        order(id, 4, 4, 0, t, 0, 0, 0, 0x3e8);
                        return 1;
                    }
                }
                order(id, 0xa, 3, 0, aim_2ee0(id, t), 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_b(joan::orig::arg::mptr(unit)));  // 0x0053cb10
            }
            if (v18 != 0) {  // dead
                order(id, 0xa, 3, 0, aim_3000(id, v18), 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_b(joan::orig::arg::mptr(unit)));  // 0x0053cb10
            }
            return 1;
        }
        if (can_move(id) == 0) return 1;
        if (n20 > 0) n24 = nearest(sx(n20), kAny, 0, 0, 0, 0, 0);
        set_target_mode(id, kNearestHigh);
        const i32 r1 = range_b280(id, 1);
        t = nearest(id, kEnemyP, r1, 1, t28, 0, 0);
        const i32 r2 = range_b280(id, 1);
        const i32 t14 = nearest(id, kEnemyP, r2 / 2, 0, t28, 0, 0);
        if (t14 > 0) {
            const i32 kind = unit_type_kind(t14);
            if (kind != 5 && kind != 6) {
                charge_orders(id, t);
                return 1;
            }
        }
        if (t > 0) {
            order(id, 0xc, 1, 0, t, 0x3c, 0x3c, 0, 0x7530);
            return 1;
        }
        if (n20 == 0 || n24 > 0) {
            set_target_mode(id, kNearestHigh);
            const i32 r3 = range_b280(id, 2);
            t = n1c != 0 ? nearest(id, kEnemyP, r3, 1, t28, 0, 0) : nearest(id, kEnemyP, r3, 1, t28, 0x87, 0);
            if (t > 0) {
                order(id, 0xc, 1, 0, t, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        if (n1c != 0 && joan::orig::arg::as<i32>(joan::orig::script::group_contains(joan::orig::arg::i32(t28), joan::orig::arg::i32(n1c))) != 0) {  // 0x00653f30
            const i32 d = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), joan::orig::arg::i32(sx(n1c))));  // 0x00653150
            if (d <= range_b280(id, 3)) {
                order(id, 0xc, 1, 1, n1c, 0x3c, 0x3c, 0, 0x7530);
                return 1;
            }
        }
        order(id, 0xe, -3, 0, t28, 0, 0, 0, 0x7530);
        set_state(id, kIdle);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_12", name, addr, fn)
B("Ai::state_538730", 0x00538730, &H::state_538730);
B("Ai::state_53beb0", 0x0053beb0, &H::state_53beb0);
#undef B

}  // namespace game_b106_12
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
//
//    - Unit::ai_state_541300 0x00541300 (ret 4): script AI state handler (0x006517c0 / 0x0065b280 /
//      0x00661340 ...).
namespace {
namespace game_b108_20 {

JOAN_COUNTER(c_ai_541300, "Unit::ai_state_541300");

// ---- script AI helpers (cdecl) ----
u32 script_query(u16 id, const char* key, u32 a, u32 b, u32 c, u32 d, u32 e) {  // 0x006517c0
    return joan::orig::arg::as<u32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), key, joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));
}
u32 range_of(i32 id, i32 kind) { return joan::orig::arg::as<u32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(kind))); }  // 0x0065b280
void set_state(u16 id, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }         // 0x006515d0
void order(i32 id, i32 a, i32 b, i32 c, u32 d, u32 e, u32 f, i32 g, i32 h) {          // 0x00661340
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));
}
u32 is_active(u16 id) { return joan::orig::arg::as<u32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))); }   // 0x00651590
u32 is_idle(u16 id) { return joan::orig::arg::as<u32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))); }     // 0x00651550
u32 slot_of(u16 id, i32 n) { return joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(n))); }  // 0x00652190
const char* kAttack = str(d::s_ATTACK_9cf380);
const char* kActive = str(d::s_ACTIVE);
const char* kIdle = str(d::s_IDLE);
const char* kKeyAny = str(d::s_E_9cfc2c);

struct H {
    // -------------------------------------------------------------- 0x00541300
    u32 ai_state_541300(void* unit) {
        c_ai_541300.hit();
        const i32 id = jo::as_unit(unit)->id;
        const u16 wid = static_cast<u16>(id);
        script_query(wid, str(d::s_T), 0, 0, 0, 0, 0);
        const u32 r2 = range_of(id, 2);
        const u32 enemy = script_query(wid, kKeyAny, r2, 0, 0, 0, 0x41);
        const u32 r1 = range_of(id, 1);
        const u32 near = script_query(wid, kKeyAny, r1, 0, 0, 0, 0x41);
        if (near != 0) {
            const u32 r = range_of(id, 1);
            order(id, 0xc, 1, 0, near, r, 0x190, 0, 0x7530);
            return 1;
        }
        if (enemy != 0) set_state(wid, kAttack);
        else set_state(wid, kActive);
        if (is_active(wid) != 0 && enemy != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(enemy))) == 0) {  // 0x005130a0
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(wid), joan::orig::arg::i32(enemy)));  // 0x00652ee0
                order(id, 0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                joan::orig::as_order(this)->run_a(joan::orig::arg::mptr(unit), i32{1}, joan::orig::arg::i32(enemy));  // 0x00541270
                return 1;
            }
            set_state(wid, kActive);
        }
        if (is_idle(wid) != 0) {
            const u32 s1 = slot_of(wid, 1);
            const u32 s2 = slot_of(wid, 2);
            const u32 s3 = slot_of(wid, 3);
            order(id, 7, 0, 0, s1, s2, s3, 0, 0x7530);
            if (joan::orig::arg::as<i32>(joan::orig::unit::distance_to(joan::orig::arg::i16(wid), joan::orig::arg::i32(s1), joan::orig::arg::i32(s2), joan::orig::arg::i32(s3))) <= 0xc8)  // 0x0065b800
                joan::orig::script::unit_528300(joan::orig::arg::i32(wid));                       // 0x00652370
            set_state(wid, kIdle);
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Unit::ai_state_541300", 0x00541300, &H::ai_state_541300);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Order::run_1(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run1>(this, a0)); }  // 0x531f90
auto joan::orig::Order::run_2(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run2>(this, a0)); }  // 0x5322c0
auto joan::orig::Order::run_3(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run3>(this, a0)); }  // 0x532520
auto joan::orig::Order::run_4(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run4>(this, a0)); }  // 0x532780
auto joan::orig::Order::run_5(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run5>(this, a0)); }  // 0x532a00
auto joan::orig::Order::run_6(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run6>(this, a0)); }  // 0x532c60
auto joan::orig::Order::run_7(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run7>(this, a0)); }  // 0x532ef0
auto joan::orig::Order::run_8(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run8>(this, a0)); }  // 0x533170
auto joan::orig::Order::run_9(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run9>(this, a0)); }  // 0x533370
auto joan::orig::Order::run_10(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run10>(this, a0)); }  // 0x533440
auto joan::orig::Order::run_11(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run11>(this, a0)); }  // 0x5335e0
auto joan::orig::Order::run_12(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run12>(this, a0)); }  // 0x5336e0
auto joan::orig::Order::run_13(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run13>(this, a0)); }  // 0x533810
auto joan::orig::Order::run_14(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run14>(this, a0)); }  // 0x533920
auto joan::orig::Order::run_15(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_01::H::run15>(this, a0)); }  // 0x533b10
auto joan::orig::Order::flee_a(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_08::H::flee_a>(this, a0)); }  // 0x535c20
auto joan::orig::Unit::voice_react_538340(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b105_07::H::voice_538340>(this, a0)); }  // 0x538340
auto joan::orig::Unit::voice_react_53abd0(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b105_07::H::voice_53abd0>(this, a0)); }  // 0x53abd0
auto joan::orig::Order::flee_b(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_08::H::flee_b>(this, a0)); }  // 0x53cb10
auto joan::orig::Unit::voice_react_53f670(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b105_07::H::voice_53f670>(this, a0)); }  // 0x53f670
auto joan::orig::Order::run_a(void* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_04::H::run_a>(this, a0, a1, a2)); }  // 0x541270
auto joan::orig::Order::run_b(void* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_04::H::run_b>(this, a0, a1, a2)); }  // 0x5432a0
