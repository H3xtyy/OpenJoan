// game/unknown_us_tactics.cpp: functions whose original source file is unknown (formerly game/_unplaced/us_tactics.cpp;
// re/attribution.csv: between ww/upgrares.cpp and ww/uscrres.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cmath>

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
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::str;

using i16 = std::int16_t;

using joan::img::obj;

const char* const kAttack = str(d::s_ATTACK_9cf380);  // "ATTACK"
const char* const kActive = str(d::s_ACTIVE);  // "ACTIVE"
const char* const kIdle = str(d::s_IDLE);    // "IDLE"

using joan::hook::global;

struct Obj {};

// Virtual call without arguments (thiscall) through the vtable slot (byte offset).
void* vcall0(void* o, std::int32_t offset) {
    using Virtual = void* (Obj::*)();
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(o)[joan::call::x86_vslot(offset)], sizeof(void*));
    return (reinterpret_cast<Obj*>(o)->*method)();
}

void* const kObjects = obj(d::g_items);

void unit_stop(i32 id) { joan::orig::script::unit_528300(joan::orig::arg::i32(id)); }                      // 0x00652370

using joan::hook::x87::Fpu;

void* const kRandom = obj(d::g_misc);

}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

JOAN_COUNTER(c_attack_idle, "Ai::attack_or_idle");

struct H {
    // --------------------------------------------------------------------- 0x00543c10
    i32 attack_or_idle(u8* unit) {
        c_attack_idle.hit();
        const i32 id = static_cast<i32>(jo::as_unit(unit)->id);
        joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(d::s_T), i32{0}, i32{0}, i32{0}, i32{0}, i32{0});  // 0x006517c0
        i32 range = 0x640;
        if (jo::as_unit(unit)->actor_c != nullptr) {
            const u8* const sq = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(unit)->actor_c)->record))));  // 0x006af310
            if (jo::self<jo::Shot>(sq)->type == 0xb) range = 0x9c4;
        }
        if (joan::orig::arg::as<i32>(joan::orig::unit::guard_check(joan::orig::arg::i16(id), str(d::s_F_9d0068))) == 0) return 1;  // 0x00656bb0
        i32 target = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(d::s_S_9d7e34), joan::orig::arg::i32(range), i32{0}, i32{0}, i32{0}, i32{0x41}));  // 0x006517c0
        if (target == 0) {
            target = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(d::s_E_9cfc2c), joan::orig::arg::i32(range), i32{0}, i32{0}, i32{0}, i32{0x41}));  // 0x006517c0
        }
        const i32 other = joan::orig::arg::as<i32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(id), str(d::s_EC), joan::orig::arg::u32(i32{0xbb8}), joan::orig::arg::u32(i32{0})));  // 0x006519d0
        if (target != 0 || other != 0) joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ATTACK_9cf380));  // 0x006515d0 "ATTACK"
        if (target == 0 && other == 0) joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ACTIVE));  // 0x006515d0 "ACTIVE"

        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) != 0) {  // 0x00651590
            if (other != 0) {
                const i32 v = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(other)));  // 0x00653000
                joan::orig::group_order(joan::orig::arg::i32(id), i32{0xa}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{3})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(v), i32{0}, i32{0}, i32{0}, i32{0x9c4});  // 0x00661340
                joan::orig::group_order(joan::orig::arg::i32(id), i32{2}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{2})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), i32{2}, joan::orig::arg::i32(other), i32{0}, i32{0}, i32{0x5dc});  // 0x00661340
                return 1;
            }
            if (target != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(target))) == 0) {  // 0x005130a0
                    const i32 v = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(target)));  // 0x00652ee0
                    joan::orig::group_order(joan::orig::arg::i32(id), i32{0xa}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{3})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(v), i32{0}, i32{0}, i32{0}, i32{0x9c4});  // 0x00661340
                    joan::orig::as_order(this)->run_c(joan::orig::arg::mptr(unit), i32{1}, joan::orig::arg::i32(target));  // 0x00544c40
                    return 1;
                }
                joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ACTIVE));  // 0x006515d0 "ACTIVE"
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))) != 0) {  // 0x00651550
            const i32 t1 = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{1}));  // 0x00652190
            const i32 t2 = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{2}));  // 0x00652190
            const i32 t3 = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), i32{3}));  // 0x00652190
            joan::orig::group_order(joan::orig::arg::i32(id), i32{7}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(t1), joan::orig::arg::i32(t2), joan::orig::arg::i32(t3), i32{0}, i32{0x7530});  // 0x00661340
            if (joan::orig::arg::as<i32>(joan::orig::unit::distance_to(joan::orig::arg::i16(id), joan::orig::arg::i32(t1), joan::orig::arg::i32(t2), joan::orig::arg::i32(t3))) <= 0xc8) joan::orig::script::unit_528300(joan::orig::arg::i32(id));  // 0x0065b800 / 0x00652370
            joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_IDLE));  // 0x006515d0 "IDLE"
        }
        return 1;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Ai::attack_or_idle", 0x00543c10, &H::attack_or_idle);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

JOAN_COUNTER(c_think_siege, "Ai::think_siege");
JOAN_COUNTER(c_think_attack, "Ai::think_attack");

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
void* const kUnitTable = obj(d::g_units);

struct H {
    // ------------------------------------------------------------------ 0x00543fd0
    i32 think_siege(u8* unit) {
        c_think_siege.hit();
        const i32 id = jo::as_unit(unit)->id;
        find_target(id, str(d::s_T), 0, 0, 0, 0, 0);  // [ebp-0x18], never read
        i32 v10 = 0;
        if (joan::orig::arg::as<i32>(joan::orig::script::leader_id(joan::orig::arg::i32(id))) > 0) {  // 0x006569f0
            v10 = find_target(id, str(d::s_L), 0x3e8, 0, 0, 0, 0);
        }
        if (joan::orig::arg::as<i32>(joan::orig::unit::guard_check(joan::orig::arg::i16(id), str(d::s_F_9d0068))) == 0) return 1;  // 0x00656bb0
        v10 = find_target(id, str(d::s_S_9d7e34), 0x640, 0, 0, 0, 0x41);
        if (v10 == 0) v10 = find_target(id, str(d::s_E_9cfc2c), 0x640, 0, 0, 0x5a, 0x41);
        i32 v14 = find_near(id, str(d::s_EC), 0xc80, 0);
        if (v10 != 0 || v14 != 0) set_state(id, kAttack);
        if (v10 == 0 && v14 == 0) set_state(id, kActive);
        if (is_ready(id) != 0) {
            if (v14 != 0) {
                order(id, 2, 2, 0, 2, v14, 0, 0, 0x1388);
                return 1;
            }
            if (v10 != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(v10))) == 0) {  // 0x005130a0
                    joan::orig::as_order(this)->run_d(joan::orig::arg::mptr(unit), 1, joan::orig::arg::i32(v10));  // 0x00545d00
                    return 1;
                }
                set_state(id, kActive);
            }
        }
        if (is_idle(id) != 0) {
            if (unit_slot(id, 1) == 0) {
                v14 = find_near(id, str(d::s_EC), 0xfa0, 0);
                if (v14 > 0) {
                    order(id, 0xd, 1, 0, v14, 0x320, 0x190, 0, 0x7530);
                    return 1;
                }
            }
            set_state(id, kIdle);
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00544500
    i32 think_attack(u8* unit) {
        c_think_attack.hit();
        const i32 id = jo::as_unit(unit)->id;
        const i32 target = unit_slot(id, 1);
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(id))) != 0 ||                // 0x006512d0
            joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(target)) != 0 ||  // 0x00428b70
            joan::orig::arg::as<i32>(joan::orig::unit::idle(joan::orig::arg::i32(target))) != 0) {            // 0x0065c8d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(id));  // 0x00652370
            return 1;
        }
        const i32 n = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), static_cast<i32>(static_cast<i16>(target))));  // 0x00653150
        if (n >= static_cast<i32>(static_cast<u32>(unit_stat(id, 4)) * 3u)) return 1;
        set_state(id, kAttack);
        if (is_ready(id) == 0) return 1;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(target))) == 0) {        // 0x005130a0
            const i32 r = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(id), joan::orig::arg::i32(target)));  // 0x00652ee0
            order(id, 0xa, 4, 0, r, 0, 0, 0, 0x9c4);
            return joan::orig::arg::as<i32>(joan::orig::as_order(this)->run_d(joan::orig::arg::mptr(unit), 1, joan::orig::arg::i32(target)));  // 0x00545d00
        }
        set_state(id, kActive);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Ai::think_siege", 0x00543fd0, &H::think_siege);
B("Ai::think_attack", 0x00544500, &H::think_attack);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b104_07.cpp (game/b104_07)
namespace {
namespace game_b104_07 {

i32 can_melee(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))); }               // 0x00651590
i32 can_move(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(id))); }                // 0x00651550

i32 range_b280(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }    // 0x0065b280


i32 unit_target(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }   // 0x00652190
i32 unit_busy(i32 id) { return joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(id))); }               // 0x006512d0
void set_state(i32 id, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }   // 0x006515d0
i32 aim_3000(i32 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }       // 0x00653000
i32 dist_3060(i32 id, i32 t) { return joan::orig::arg::as<i32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }      // 0x00653060
i32 target_b6a0(i32 t) { return joan::orig::arg::as<i32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(t))); }  // 0x0065b6a0
i32 target_b700(i32 t) { return joan::orig::arg::as<i32>(joan::orig::object_is_type1(joan::orig::arg::i32(t))); }  // 0x0065b700
i32 target_b760(i32 t) { return joan::orig::arg::as<i32>(joan::orig::object_is_type4(joan::orig::arg::i32(t))); }  // 0x0065b760
void order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}

i32 obj_dead(i32 t) { return joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(t)); }      // 0x00425d50
u8* obj_get(i32 t) { return joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(t))); }      // 0x00425d20

// 0x00544690 approach order: 0xd with range_b280(id, 2) + ftol(rec+0x24 / 2.0f).
void approach_order(i32 id, i32 t, const u8* rec) {
    const double half = Fpu{true}.div(static_cast<double>(jo::view<i16>(jo::self<jo::ItemDef>(rec)->stats[0])),
                                      static_cast<double>(global<float>(d::flt_2)));  // 2.0f
    const i32 h = joan::hook::x87::ftol(half);
    const i32 r = range_b280(id, 2);
    order(id, 0xd, 1, 0, t, static_cast<i32>(static_cast<u32>(r) + static_cast<u32>(h)), 0x190, 0,
          0x7530);
}

struct Turn {
    u32 v;
};
static_assert(sizeof(Turn) == 4, "Turn");

JOAN_COUNTER(c_state_544690, "Ai::state_544690");
JOAN_COUNTER(c_state_545d50, "Ai::state_545d50");

struct H {
    // ------------------------------------------------------------------ 0x00544690
    u32 state_544690(i16* unit) {
        c_state_544690.hit();
        const i32 id = static_cast<i32>(*unit);
        const i32 t = unit_target(id, 1);
        if (unit_busy(id) != 0 || obj_dead(t) != 0) {
            unit_stop(id);
            return 1;
        }
        i32 range = 0x3e7;
        const i32 kind = static_cast<i32>(jo::as_map_object(obj_get(t))->class_id);
        if (kind == 1) {
            range = jo::as_gate(vcall0(obj_get(t), 0x4c))->hp;  // slot 0x4c returns the Gate
        } else if (kind == 4) {
            range = jo::view<i32>(jo::as_wall(vcall0(obj_get(t), 0x58))->u32_c6);  // slot 0x58 returns this only in the Wall vtable 0x009d3c88 (0 elsewhere)
        }
        if (range <= 0) {
            unit_stop(id);
            return 1;
        }
        const u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(obj_get(t))->type_id))));  // 0x00425ce0
        const i32 d = dist_3060(id, t);
        if (d >= range_b280(id, 2)) {
            approach_order(id, t, rec);
            return 1;
        }
        set_state(id, kAttack);
        if (can_melee(id) != 0) {
            if (target_b6a0(t) == 0 && target_b700(t) == 0 && target_b760(t) == 0) {
                unit_stop(id);
                return 1;
            }
            const i32 aim = aim_3000(id, t);
            Turn ta, tb, tmp;
            joan::orig::as_angle(&ta)->construct_577190();                              // 0x00577190
            const float heading = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading());      // 0x004ff850
            joan::orig::as_angle(&ta)->set_wrapped(joan::orig::arg::f32(heading));                     // 0x00577340
            joan::orig::as_angle(&tb)->construct_577190();                              // 0x00577190
            joan::orig::as_angle(&tb)->set_degrees(joan::orig::arg::f32(static_cast<float>(aim)));     // 0x005773e0
            Turn* const diff = joan::orig::arg::as<Turn*>(joan::orig::as_turn(&tb)->sub(&tmp, &ta));  // 0x005775b0
            const double deg = joan::orig::arg::as<double>(joan::orig::as_angle(diff)->degrees());         // 0x00577450
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->round(joan::orig::arg::f64(deg)));         // 0x00590a20
            if (r <= 1 || r >= 0x167)
                order(id, 2, 3, 0, 2, t, 0, 0, 0x5dc);
            else
                order(id, 0xa, 3, 0, aim, 0, 0, 0, 0x9c4);
        }
        if (can_move(id) != 0) approach_order(id, t, rec);
        return 1;
    }

    // ------------------------------------------------------------------ 0x00545d50
    u32 state_545d50(i16* unit) {
        c_state_545d50.hit();
        const i32 id = static_cast<i32>(*unit);
        const i32 a = unit_target(id, 1);
        const i32 c = unit_target(id, 3);
        order(id, 0xe, 0x63, 1, a, 0, 0, 0, 0x7530);
        if (joan::orig::arg::as<i32>(joan::orig::script::group_contains(joan::orig::arg::i32(a), joan::orig::arg::i32(id))) == 1 || c <= 0) {  // 0x00653f30
            joan::orig::units::detach_656410(joan::orig::arg::i32(id));                      // 0x00656410
            joan::orig::script_unit_508b90(joan::orig::arg::i16(id));                      // 0x00657610
            unit_stop(id);
            joan::orig::script_send_order(joan::orig::arg::i32(id), i32{0xb}, i32{0x63}, i32{0}, i32{0}, i32{0}, i32{0});  // 0x006619d0
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_07", name, addr, fn)
B("Ai::state_544690", 0x00544690, &H::state_544690);
B("Ai::state_545d50", 0x00545d50, &H::state_545d50);
#undef B

}  // namespace game_b104_07
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

JOAN_COUNTER(c_attack_step, "Ai::attack_step");

struct H {
    // ------------------------------------------------------------------------ 0x00544aa0
    i32 attack_step(const i16* unit) {
        c_attack_step.hit();
        const i32 v = *unit;
        const i32 target = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(v), 1));  // 0x00652190
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(v))) != 0) {             // 0x006512d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(v));                    // 0x00652370
            return 1;
        }
        const i32 n = joan::orig::arg::as<i32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(v), joan::orig::arg::i32(target)));  // 0x00653060
        if (n >= joan::orig::arg::as<i32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(v), 4)) * 3) return 1;  // 0x0065b280
        joan::orig::script::set_mode(joan::orig::arg::i32(v), str(d::s_ATTACK_9cf380));       // 0x006515d0 "ATTACK"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(v))) != 0) {             // 0x00651590
            if (joan::orig::arg::as<i32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(target))) == 0 &&    // 0x0065b6a0
                joan::orig::arg::as<i32>(joan::orig::object_is_type1(joan::orig::arg::i32(target))) == 0 &&    // 0x0065b700
                joan::orig::arg::as<i32>(joan::orig::object_is_type4(joan::orig::arg::i32(target))) == 0) {    // 0x0065b760
                joan::orig::script::unit_528300(joan::orig::arg::i32(v));                // 0x00652370
                return 1;
            }
            const i32 w = joan::orig::arg::as<i32>(joan::orig::script_heading_delta(joan::orig::arg::i32(v), joan::orig::arg::i32(target)));  // 0x00653000
            joan::orig::group_order(joan::orig::arg::i32(v), 0xa, static_cast<std::int8_t>(joan::orig::arg::i32(4)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i32(w), 0, 0, 0, 0x9c4);  // 0x00661340
            joan::orig::group_order(joan::orig::arg::i32(v), 2, static_cast<std::int8_t>(joan::orig::arg::i32(3)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), 2, joan::orig::arg::i32(target), 0, 0, 0x5dc);  // 0x00661340
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("Ai::attack_step", 0x00544aa0, &H::attack_step);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

void* const kTerrain = obj(d::g_types);

JOAN_COUNTER(c_run_return, "Order::run_return");

i32 abs_(i32 v) { return joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(v))); }                                    // abs
i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
i32 order_661340(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {  // 0x00661340
    return joan::orig::arg::as<i32>(joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));
}

struct H {
    i32 run_c(void* unit, i32 a, i32 b) {                                          // 0x00544c40
        if (random(10) < 4) order_661340(static_cast<i32>(jo::as_unit(unit)->id), 2, 2, 0, a, b, 0, 0, 0x5dc);
        return 1;
    }
    i32 run_d(void* unit, i32 a, i32 b) {  // 0x00545d00
        order_661340(static_cast<i32>(jo::as_unit(unit)->id), 2, 2, 0, a, b, 0, 0, 0x1388);
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00545ea0
    i32 run_return(u8* unit) {
        c_run_return.hit();
        const Fpu fpu{true};
        const i32 id = static_cast<i32>(jo::as_unit(unit)->id);
        const i16 sid = static_cast<i16>(id);
        if (joan::orig::arg::as<i32>(joan::orig::script::is_building(joan::orig::arg::i32(sid))) != 0) {  // 0x006512d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(sid));         // 0x00652370
            return 1;
        }
        const i32 x = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(sid), i32{1}));  // 0x00652190
        const i32 y = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(sid), i32{2}));
        const i32 z = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(sid), i32{3}));
        order_661340(id, 7, 0x63, 0, x, y, z, 0, 0x4e20);
        float r = static_cast<float>(global<i32>(d::g_waypoint_reach_dist));
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0 ||  // 0x00430190
            static_cast<i32>(jo::self<jo::UnitType>(joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTerrain)->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id))))->unit_class) == 8 ||  // 0x0042d0a0
            jo::as_unit(unit)->move_mode > 1) {
            r = static_cast<float>(static_cast<i32>(static_cast<u32>(global<i32>(d::g_waypoint_reach_dist)) * 5u));
        }
        const i32 dz = joan::hook::x87::ftol(fpu.sub(z, jo::view<float[3]>(jo::as_unit(unit)->position_)[2]));
        if (abs_(dz) > 0x1e) return 1;
        const double dx = fpu.sub(x, jo::view<float[3]>(jo::as_unit(unit)->position_)[0]);
        const double dy = fpu.sub(y, jo::view<float[3]>(jo::as_unit(unit)->position_)[1]);
        const double sum = fpu.add(fpu.mul(dx, dx), fpu.mul(dy, dy));
        const double rr = fpu.mul(r, r);
        if (rr >= sum) joan::orig::script::unit_528300(joan::orig::arg::i32(sid));  // 0x00652370
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00546050
    i32 run_e(void* unit) {
        const i32 id = static_cast<i32>(jo::as_unit(unit)->id);
        const i16 sid = static_cast<i16>(id);
        joan::orig::script::formation_slot(joan::orig::arg::i32(sid), i32{1});                    // 0x00652190 (unused)
        const i32 b = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(sid), i32{1}));      // 0x00652190
        const i32 c = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(sid), i32{2}));      // 0x00652190
        order_661340(id, 0xe, 1, c, b, 0, 0, 0, 0xbb8);
        return 1;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Order::run_c", 0x00544c40, &H::run_c);
B92("Order::run_d", 0x00545d00, &H::run_d);
B92("Order::run_return", 0x00545ea0, &H::run_return);
B92("Order::run_e", 0x00546050, &H::run_e);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

using u16 = std::uint16_t;

JOAN_COUNTER(c_nearest_archer, "Order::nearest_archer");

struct H {
    // ------------------------------------------------------------------------ 0x00544cb0
    i32 nearest_archer(void* unit) {
        c_nearest_archer.hit();
        const i16 id = jo::as_unit(unit)->id;
        joan::orig::script::set_target_mode(joan::orig::arg::i32(id), str(d::s_NEAREST_ARCHER));  // 0x00651690 'NEAREST_ARCHER'
        const i32 target = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(d::s_E_9cfc2c), 0xbb8, 0, 0, 0xa, 0x41));  // 0x006517c0
        if (target != 0) joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ATTACK_9cf380));  // 0x006515d0 'ATTACK'
        if (target == 0) joan::orig::script::set_mode(joan::orig::arg::i32(id), str(d::s_ACTIVE));  // 0x006515d0 'ACTIVE'
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(id))) == 0) return 1;                 // 0x00651590
        if (target == 0) return 1;
        const i32 d = joan::orig::arg::as<i32>(joan::orig::script_unit_value(joan::orig::arg::i16(id), joan::orig::arg::i32(target)));  // 0x00652f80
        if (d < 10 && d > -10) return joan::orig::arg::as<i32>(joan::orig::as_order(this)->run_f(joan::orig::arg::mptr(unit), 1u, joan::orig::arg::u32(target)));  // 0x00544e00
        jo::view<u16>(jo::as_unit(unit)->target_building) = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Order::nearest_archer", 0x00544cb0, &H::nearest_archer);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

struct H {
    // ------------------------------------------------------------------------ 0x00544e00
    i32 run_f(void* unit, u32 a, u32 b) {
        joan::orig::group_order(static_cast<i32>(jo::as_unit(unit)->id), i32{2}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{2})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(// 0x00661340
                        a), joan::orig::arg::i32(b), i32{0}, i32{0}, i32{0x5dc});
        return 1;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Order::run_f", 0x00544e00, &H::run_f);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {


const char* const kTag = str(d::s_M_9d006c);

// cdecl unit helpers (short id arguments read with movsx word by the callees).
u32 unit_target(i32 id, i32 k) { return joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }   // 0x00652190
i32 target_ok_b700(u32 t) { return joan::orig::arg::as<i32>(joan::orig::object_is_type1(joan::orig::arg::i32(t))); }           // 0x0065b700
i32 target_flag_24a0(u32 t) { return joan::orig::arg::as<i32>(joan::orig::object_d1(joan::orig::arg::i32(t))); }         // 0x006524a0
i32 dist_3060(i32 id, u32 t) { return joan::orig::arg::as<i32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }      // 0x00653060
void set_state(i32 id, const char* s) { joan::orig::script::set_mode(joan::orig::arg::i32(id), joan::orig::arg::str(s)); }   // 0x006515d0
u32 aim_3000(i32 id, u32 t) { return joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }       // 0x00653000
i32 has_tag(i32 id, const char* s) { return joan::orig::arg::as<i32>(joan::orig::unit::guard_check(joan::orig::arg::i16(id), joan::orig::arg::str(s))); }  // 0x00656bb0
i32 angle_f0f0(i32 id, u32 t) { return joan::orig::arg::as<i32>(joan::orig::script_unit_heading_to(joan::orig::arg::i32(id), joan::orig::arg::i32(t))); }     // 0x0065f0f0
void order(i32 id, i32 a, i32 b, i32 c, u32 d, u32 e, i32 f, i32 g, i32 h) {
    joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h));  // 0x00661340
}

i32 obj_dead(u32 t) { return joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(t)); }  // 0x00425d50

JOAN_COUNTER(c_ai_544e50, "Ai::state_544e50");
JOAN_COUNTER(c_ai_5450b0, "Ai::state_5450b0");

struct H {
    // "ACTIVE" + order 9 with the unit's targets 1 and 2 (target 2 fetched first).
    static void activate(i32 id) {
        set_state(id, kActive);
        const u32 t2 = unit_target(id, 2);
        const u32 t1 = unit_target(id, 1);
        order(id, 9, 1, 0, t1, t2, 0, 0, 0x7530);
    }

    // ------------------------------------------------------------------ 0x00544e50
    u32 state_544e50(i16* unit) {
        c_ai_544e50.hit();
        const i32 id = static_cast<i32>(*unit);
        const u32 tgt = unit_target(id, 3);
        set_state(id, kIdle);
        if (target_ok_b700(tgt) == 0 || target_flag_24a0(tgt) != 0) {
            activate(id);
            return 1;
        }
        if (dist_3060(id, tgt) < 0x12c) {
            if (has_tag(id, kTag) != 0) {
                const u32 r = aim_3000(id, tgt);
                order(id, 0xa, 4, 0, r, 0, 0, 0, 0x9c4);
            } else {
                const i32 a = angle_f0f0(id, tgt);
                if (a < -0x1e || a > 0x1e) return 1;
            }
            set_state(id, kAttack);
            return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flag_f_retreat(joan::orig::arg::mptr(unit)));  // 0x00545380
        }
        if (has_tag(id, kTag) != 0) activate(id);
        return 1;
    }

    // ------------------------------------------------------------------ 0x005450b0
    u32 state_5450b0(i16* unit) {
        c_ai_5450b0.hit();
        const i32 id = static_cast<i32>(*unit);
        const u32 tgt = unit_target(id, 1);
        if (obj_dead(tgt) != 0) {
            unit_stop(id);
            return 1;
        }
        set_state(id, kIdle);
        i32 range = 0x3e7;
        i16 reach = 0;
        const i32 kind = static_cast<i32>(jo::as_map_object(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(tgt)))->class_id);  // 0x00425d20
        if (kind == 1) {
            if (target_flag_24a0(tgt) != 0) return 1;
            void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(tgt));  // 0x00425d20
            range = jo::as_gate(vcall0(o, 0x4c))->hp;  // slot 0x4c returns the Gate
            reach = 0x12c;
        } else if (kind == 4) {
            void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(tgt));  // 0x00425d20
            range = jo::view<i32>(jo::as_wall(vcall0(o, 0x58))->u32_c6);  // slot 0x58 returns this only in the Wall vtable 0x009d3c88 (0 elsewhere)
            reach = 0x190;
        }
        if (range <= 0) {
            unit_stop(id);
            return 1;
        }
        if (dist_3060(id, tgt) < static_cast<i32>(reach)) {
            if (has_tag(id, kTag) != 0) {
                const u32 r = aim_3000(id, tgt);
                order(id, 0xa, 4, 0, r, 0, 0, 0, 0x9c4);
            } else {
                const i32 a = angle_f0f0(id, tgt);
                if (a < -0x1e || a > 0x1e) return 1;
            }
            set_state(id, kAttack);
            joan::orig::as_order(this)->flag_f_retreat(joan::orig::arg::mptr(unit));  // 0x00545380 (result ignored)
            return 1;
        }
        if (has_tag(id, kTag) != 0) {
            set_state(id, kActive);
            order(id, 0xd, 1, 0, tgt, 0x64, 0x64, 0, 0x7530);
            return 1;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("Ai::state_544e50", 0x00544e50, &H::state_544e50);
B("Ai::state_5450b0", 0x005450b0, &H::state_5450b0);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b101_08.cpp (game/b101_08)
namespace {
namespace game_b101_08 {

void* const kTokens = obj(d::g_misc);  // also the random generator object
void* const kUnits = obj(d::g_units);

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

i32 random(i32 n) { return joan::orig::as_misc(kTokens)->random(joan::orig::arg::i32(n)); }  // 0x00590540
i32 sym(const char* s) { return joan::orig::arg::as<i32>(joan::orig::script_rgb_component(joan::orig::arg::str(s), i32{1})); }  // 0x006511f0
i32 order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    return joan::orig::arg::as<i32>(joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));  // 0x00661340
}
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
void unit_pos(void* unit, Vec3* out) { joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(out)); }  // 0x004fedb0

JOAN_COUNTER(c_flag_f, "Order::flag_f_retreat");
JOAN_COUNTER(c_scatter, "Order::scatter_from_e");

struct H {
    // ------------------------------------------------------------------ 0x00545380
    u32 flag_f_retreat(void* unit) {
        c_flag_f.hit();
        const i32 id = static_cast<i32>(jo::as_unit(unit)->id);
        if (joan::orig::arg::as<i32>(joan::orig::unit::guard_check(joan::orig::arg::i16(static_cast<i16>(id)), str(d::s_F_9d0068))) == 0) return 1;  // 0x00656bb0 "F"
        if (random(10) < 5) {
            const i32 v = sym(str(d::s_A));  // "A"
            order(id, 1, 2, 0, v, 0, 0, 0, 0x5dc);
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00545590
    u32 scatter_from_e(void* unit) {
        c_scatter.hit();
        const i32 id = static_cast<i32>(jo::as_unit(unit)->id);
        const i32 near = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(static_cast<i16>(id)), str(d::s_E_9cfc2c), joan::orig::arg::i32(// 0x006517c0 "E"
                                         i32{0x258}), i32{0}, i32{0}, i32{0}, i32{0}));
        if (near > 0 && unit_missing(near) == 0) {
            u8* const other = unit_get(near);
            Vec3 a;
            Vec3 b;
            unit_pos(other, &a);
            unit_pos(unit, &b);
            const float dy = b.y - a.y;
            const float dx = b.x - a.x;
            const float q = dy / dx;
            // atan's st0 is stored straight to a float (fstp dword): float return, no double rounding.
            const float ang = joan::orig::arg::as<float>(joan::orig::libc::atan(joan::orig::arg::f64(static_cast<double>(q))));  // atan
            i32 r1 = static_cast<i32>(static_cast<u32>(random(0x190)) - 0x190u);
            const i32 r2 = random(0x190);
            const Fpu fpu{true};
            const double s = joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(ang))));  // sin
            const i32 ox = joan::hook::x87::ftol(fpu.mul(s, static_cast<double>(r2)));
            const double c = joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(ang))));  // cos
            const i32 oy = joan::hook::x87::ftol(fpu.mul(c, static_cast<double>(r2)));
            if (r1 > 0) r1 = static_cast<i32>(static_cast<u32>(r1) + 100u);
            if (r1 < 0) r1 = static_cast<i32>(static_cast<u32>(r1) - 100u);
            if (random(5) <= 3) {
                joan::orig::group_release(joan::orig::arg::i32(id));  // 0x006618f0
                order(id, 8, -1, 0, ox, oy, r1, 0, 0x7d0);
            } else {
                joan::orig::group_release(joan::orig::arg::i32(id));  // 0x006618f0
                order(id, 0x16, 0x14, 0, 1, 0, 1, 0, 0x7d0);
            }
            return 1;
        }
        if (random(100) >= 0x5a) {
            i32 x = static_cast<i32>(static_cast<u32>(random(0xc8)) - 100u);
            i32 y = static_cast<i32>(static_cast<u32>(random(0xc8)) - 100u);
            i32 z = static_cast<i32>(static_cast<u32>(random(0xc8)) - 100u);
            if (x > 0) x = static_cast<i32>(static_cast<u32>(x) + 0x50u);
            if (x < 0) x = static_cast<i32>(static_cast<u32>(x) - 0x50u);
            if (y > 0) y = static_cast<i32>(static_cast<u32>(y) + 0x50u);
            if (y < 0) y = static_cast<i32>(static_cast<u32>(y) - 0x50u);
            if (z > 0) z = static_cast<i32>(static_cast<u32>(z) + 0x50u);
            if (z < 0) z = static_cast<i32>(static_cast<u32>(z) - 0x50u);
            order(id, 8, -1, 0, x, y, z, 0, 0x1388);
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_08", name, addr, fn)
B("Order::flag_f_retreat", 0x00545380, &H::flag_f_retreat);
B("Order::scatter_from_e", 0x00545590, &H::scatter_from_e);
#undef B

}  // namespace game_b101_08
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

JOAN_COUNTER(c_guard_step, "Ai::guard_step");

// ---- script AI helpers (cdecl, short ids read as words by the callees) ----
i32 formation_slot(i32 id, i32 k) { return joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(id), joan::orig::arg::i32(k))); }  // 0x00652190
i32 group_order(i32 id, i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h) {
    return joan::orig::arg::as<i32>(joan::orig::group_order(joan::orig::arg::i32(id), joan::orig::arg::i32(a), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g), joan::orig::arg::i32(h)));  // 0x00661340
}

struct H {
    // ------------------------------------------------------------------------ 0x00545430
    i32 guard_step(const i16* pid) {
        c_guard_step.hit();
        const i32 id = static_cast<i32>(*pid);
        const i32 s1 = formation_slot(id, 1);
        i32 s2 = formation_slot(id, 2);
        if (s2 == 0) s2 = 0xb4;
        const i32 dist = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(id), static_cast<i32>(static_cast<i16>(s1))));  // 0x00653150
        const i32 busy = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), str(d::s_T), 0, 0, 0, 0, 0));       // 0x006517c0
        if (dist > 0x190) {
            group_order(id, 0x10, 5, 0, s1, s2, 0xc8, 0, 0x1388);
            joan::orig::script::set_2d7(joan::orig::arg::i32(id), 0);  // 0x006558d0
            return 1;
        }
        if (busy == 0) group_order(id, 0x10, -1, 0, s1, s2, 0xc8, 0, 0x1388);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Ai::guard_step", 0x00545430, &H::guard_step);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

JOAN_COUNTER(c_flee, "Order::flee");

struct H {
    // ------------------------------------------------------------------------ 0x00545900
    i32 flee(const i16* who) {
        c_flee.hit();
        const i32 v = *who;  // ebp-0x8
        const i32 a = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(v), i32{1}));  // 0x00652190
        i32 b = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(v), i32{2}));        // 0x00652190
        if (b == 0) b = 0xb4;
        const i32 dist = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(v), static_cast<i32>(static_cast<i16>(a))));  // 0x00653150
        const i32 busy = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(v), str(d::s_T), i32{0}, i32{0}, i32{0}, i32{0}, i32{0}));  // 0x006517c0
        if (dist > 0x190) {
            joan::orig::group_order(joan::orig::arg::i32(v), i32{0x10}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{5})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(a), joan::orig::arg::i32(b), i32{0xc8}, i32{0}, i32{0x4e20});            // 0x00661340
            joan::orig::script::set_2d7(joan::orig::arg::i32(v), i32{0});  // 0x006558d0
            return 1;
        }
        if (busy == 0) {
            joan::orig::group_order(joan::orig::arg::i32(v), i32{0x10}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{-1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(a), joan::orig::arg::i32(b), i32{0xc8}, i32{0}, i32{0x4e20});  // 0x00661340
        }
        return 1;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("Order::flee", 0x00545900, &H::flee);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

JOAN_COUNTER(c_wander, "Order::random_wander");

struct H {
    // ------------------------------------------------------------------------ 0x00545a60
    i32 random_wander(void* unit) {
        c_wander.hit();
        const i32 v = static_cast<i32>(jo::as_unit(unit)->id);
        if (joan::orig::as_misc(kRandom)->random(i32{0x64}) >= 0x5a) {  // 0x00590540
            const i32 x = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(v), i32{1}));      // 0x00652190
            const i32 y = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(v), i32{2}));
            const i32 r = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(v), i32{3}));
            const u32 r2 = static_cast<u32>(r) << 1;
            const i32 dx = static_cast<i32>(static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::script_random(joan::orig::arg::i32(r2)))) - static_cast<u32>(r));  // 0x00651010
            const i32 dy = static_cast<i32>(static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::script_random(joan::orig::arg::i32(static_cast<u32>(r) << 1)))) -
                                            static_cast<u32>(r));
            joan::orig::group_order(joan::orig::arg::i32(v), i32{7}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{-1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(// 0x00661340
                            static_cast<i32>(static_cast<u32>(x) + static_cast<u32>(dx))), static_cast<i32>(static_cast<u32>(y) + static_cast<u32>(dy)), i32{0}, i32{0}, i32{0x7530});
        }
        return 1;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("Order::random_wander", 0x00545a60, &H::random_wander);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

JOAN_COUNTER(c_ai_start, "UnitAI::start");

// ------------------------------------------------------------------------------- thiscall
struct H {
    // 0x00545b80
    i32 ai_start(i16* idp) {
        c_ai_start.hit();
        const i32 id = static_cast<i32>(*idp);
        const i16 sid = static_cast<i16>(id);
        const i32 r = joan::orig::arg::as<i32>(joan::orig::script::formation_slot(joan::orig::arg::i32(sid), i32{1}));  // 0x00652190
        const i32 target = joan::orig::arg::as<i32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(sid), str(d::s_EP), i32{0x4000}, i32{0}, joan::orig::arg::i32(r), i32{0}, joan::orig::arg::i32(// 0x006517c0
                                           i32{0x41})));
        if (target != 0) joan::orig::script::set_mode(joan::orig::arg::i32(sid), str(d::s_ATTACK_9cf380));  // 0x006515d0 "ATTACK"
        if (target == 0) joan::orig::script::set_mode(joan::orig::arg::i32(sid), str(d::s_ACTIVE));  // 0x006515d0 "ACTIVE"
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_is1(joan::orig::arg::i32(sid))) != 0) {                         // 0x00651590
            if (target == 0) return 1;
            const i32 v = joan::orig::arg::as<i32>(joan::orig::script::angle_to(joan::orig::arg::i32(sid), joan::orig::arg::i32(target)));  // 0x00652ee0
            joan::orig::group_order(joan::orig::arg::i32(id), i32{0xa}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{3})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::i32(v), i32{0}, i32{0}, i32{0}, joan::orig::arg::i32(// 0x00661340
                            i32{0x9c4}));
            return joan::orig::arg::as<i32>(joan::orig::as_order(this)->run_d(joan::orig::arg::mptr(idp), i32{1}, joan::orig::arg::i32(target)));  // 0x00545d00
        }
        if (joan::orig::arg::as<i32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(sid))) != 0) joan::orig::script::set_mode(joan::orig::arg::i32(sid), str(d::s_IDLE));  // 0x00651550, "IDLE"
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("UnitAI::start", 0x00545b80, &H::ai_start);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
//
//    - Unit::ai_state_543800 0x00543800 / Unit::ai_state_5442d0 0x005442d0 (ret 4): script AI state
//      handlers (0x006517c0 / 0x0065b280 / 0x00661340 ...). The last one returns 0x00544c40's eax on
//      its "attack" path, 1 elsewhere.
namespace {
namespace game_b108_20 {

using u16 = std::uint16_t;

JOAN_COUNTER(c_ai_543800, "Unit::ai_state_543800");
JOAN_COUNTER(c_ai_5442d0, "Unit::ai_state_5442d0");

constexpr img::Addr kUnits = d::g_units;  // unit table (0x00428b70 check, 0x0041b3c0 get)

// ---- script AI helpers (cdecl) ----
u32 script_query(u16 id, const void* key, u32 a, u32 b, u32 c, u32 d, u32 e) {  // 0x006517c0
    return joan::orig::arg::as<u32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(id), static_cast<const char*>(key), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e)));
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
const void* kKeyAny = obj(d::s_E_9cfc2c);

struct H {
    // -------------------------------------------------------------- 0x00543800
    u32 ai_state_543800(void* unit) {
        c_ai_543800.hit();
        const i32 id = jo::as_unit(unit)->id;
        const u16 wid = static_cast<u16>(id);
        u32 target = 0, ranged = 0;
        script_query(wid, obj(d::s_T), 0, 0, 0, 0, 0);
        if (joan::orig::arg::as<i32>(joan::orig::script::leader_id(joan::orig::arg::i32(id))) > 0) {  // 0x006569f0
            target = script_query(wid, obj(d::s_L), 0x3e8, 0, 0, 0, 0);
            if (target == 0) {
                order(id, 0xf, 2, 1, 0, 0, 0, 0, 0x2710);
                joan::orig::script::set_2d7(joan::orig::arg::i32(wid), i32{0});  // 0x006558d0 (push 0)
                return 1;
            }
        }
        u32 range = 0x640;
        if (jo::as_unit(unit)->actor_c != nullptr) {
            void* const t = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(unit)->actor_c)->record)));  // 0x006af310
            if (jo::self<jo::Shot>(t)->type == 0xb) range = 0x9c4;
        }
        if (joan::orig::arg::as<u32>(joan::orig::unit::guard_check(joan::orig::arg::i16(wid), joan::orig::arg::str(obj(d::s_F_9d0068)))) == 0) return 1;  // 0x00656bb0
        target = script_query(wid, obj(d::s_S_9d7e34), range, 0, 0, 0, 0x41);
        if (target == 0) target = script_query(wid, kKeyAny, range, 0, 0, 0, 0x41);
        ranged = joan::orig::arg::as<u32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(wid), str(d::s_EC), joan::orig::arg::u32(i32{0xbb8}), joan::orig::arg::u32(i32{0})));  // 0x006519d0
        if (target != 0 || ranged != 0) set_state(wid, kAttack);
        if (target == 0 && ranged == 0) set_state(wid, kActive);
        if (is_active(wid) != 0) {
            if (ranged != 0) {
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(wid), joan::orig::arg::i32(ranged)));  // 0x00653000
                order(id, 0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                order(id, 2, 2, 0, 2, ranged, 0, 0, 0x5dc);
                return 1;
            }
            if (target != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(target))) == 0) {  // 0x005130a0
                    const u32 r = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(wid), joan::orig::arg::i32(target)));  // 0x00652ee0
                    order(id, 0xa, 3, 0, r, 0, 0, 0, 0x9c4);
                    joan::orig::as_order(this)->run_c(joan::orig::arg::mptr(unit), i32{1}, joan::orig::arg::i32(target));  // 0x00544c40
                    return 1;
                }
                set_state(wid, kActive);
            }
        }
        if (is_idle(wid) != 0) {
            if (slot_of(wid, 1) == 0) {
                const u32 r = joan::orig::arg::as<u32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(wid), str(d::s_EC), joan::orig::arg::u32(i32{0xfa0}), joan::orig::arg::u32(i32{0})));
                if (static_cast<i32>(r) > 0) {
                    order(id, 0xd, 1, 0, r, 0x320, 0x190, 0, 0x7530);
                    return 1;
                }
            }
            set_state(wid, kIdle);
        }
        return 1;
    }

    // -------------------------------------------------------------- 0x005442d0
    u32 ai_state_5442d0(void* unit) {
        c_ai_5442d0.hit();
        const i32 id = jo::as_unit(unit)->id;
        const u16 wid = static_cast<u16>(id);
        const u32 slot = slot_of(wid, 1);
        if (joan::orig::arg::as<u32>(joan::orig::script::is_building(joan::orig::arg::i32(wid))) != 0 || joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(slot))) != 0 ||  // 0x006512d0 / 0x00428b70
            joan::orig::arg::as<u32>(joan::orig::unit::idle(joan::orig::arg::i32(slot))) != 0) {                                                    // 0x0065c8d0
            joan::orig::script::unit_528300(joan::orig::arg::i32(wid));  // 0x00652370
            return 1;
        }
        const i32 dist = joan::orig::arg::as<i32>(joan::orig::script::distance(joan::orig::arg::i32(wid), joan::orig::arg::i32(static_cast<u16>(slot))));  // 0x00653150
        if (dist >= static_cast<i32>(range_of(id, 2))) {
            const u32 r = range_of(id, 2);
            order(id, 0xc, 1, 0, slot, r - 0x7d0, 0x7d0, 0, 0x7530);
            return 1;
        }
        set_state(wid, kAttack);
        if (is_active(wid) != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->line_of_sight(joan::orig::arg::i32(slot))) == 0) {  // 0x005130a0
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(wid), joan::orig::arg::i32(slot)));  // 0x00652ee0
                order(id, 0xa, 4, 0, r, 0, 0, 0, 0x9c4);
                return joan::orig::arg::as<u32>(joan::orig::as_order(this)->run_c(joan::orig::arg::mptr(unit), i32{1}, joan::orig::arg::i32(slot)));  // 0x00544c40 (its eax)
            }
            set_state(wid, kActive);
        }
        if (is_idle(wid) != 0) {
            const u32 r = range_of(id, 2);
            order(id, 0xc, 1, 0, slot, r - 0x7d0, 0x7d0, 0, 0x7530);
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Unit::ai_state_543800", 0x00543800, &H::ai_state_543800);
B("Unit::ai_state_5442d0", 0x005442d0, &H::ai_state_5442d0);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Order::run_c(void* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_04::H::run_c>(this, a0, a1, a2)); }  // 0x544c40
auto joan::orig::Order::run_f(void* a0, std::uint32_t a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_02::H::run_f>(this, a0, a1, a2)); }  // 0x544e00
auto joan::orig::Order::flag_f_retreat(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_08::H::flag_f_retreat>(this, a0)); }  // 0x545380
auto joan::orig::Order::run_d(void* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_04::H::run_d>(this, a0, a1, a2)); }  // 0x545d00
