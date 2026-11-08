// game/unknown_ai_behav.cpp: functions whose original source file is unknown (formerly game/_unplaced/ai_behav.cpp;
// re/attribution.csv: between ww/natres.cpp and ww/pemires.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cstddef>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "unit_stuck.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::img::obj;

struct Vec3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers
    float x, y, z;
};

using joan::orig::helpers::u;

using joan::call::cdecl_call;

using joan::img::str;

using joan::hook::x87::Fpu;

using joan::call::import_call;

using joan::orig::helpers::units;

struct Vec {
    float x, y, z;
};

// A 12-byte vector kept as raw dwords, as the three floats the callee of a by-value slot call declares (System V
// passes three floats in XMM registers, three dwords in integer registers: caller and callee must agree).
template <class To, class From>
To rebits(const From& from) {
    static_assert(sizeof(To) == sizeof(From));
    To to;
    std::memcpy(&to, &from, sizeof to);
    return to;
}

}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

struct H {
    // 0x004bab50
    H* obj_ctor() {
        joan::orig::dynarray(this)->construct(0x21, 0xa, 0);  // 0x005843d0
        std::memset(static_cast<void*>(&jo::self<jo::Order>(this)->current), 0, sizeof(jo::OrderEntry));                        // memset
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Obj::Obj_4bab50", 0x004bab50, &H::obj_ctor);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
namespace {
namespace game_b100_05 {


void* const kEntities = obj(d::g_units);
void* const kGroups = obj(d::g_types);

u8* entity(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kEntities)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_is_idle, "Unit::is_idle");
JOAN_COUNTER(c_split, "Army::split");

struct H {

    // ------------------------------------------------------------------ 0x004baba0
    u32 dtor_4baba0() {
        joan::orig::as_order(this)->clear();          // 0x004bac30
        joan::orig::as_dynarray(this)->destruct_5844b0();   // 0x005844b0
        return 0;  // void callee; callers discard the result
    }

    // ------------------------------------------------------------------ 0x004bba00
    u8 byte_at(i8 sel, i8 idx) {
        i32 off;
        if (sel == 1) {
            off = 0x5;
        } else if (sel == 2) {
            off = 0x9;
        } else if (sel == 3) {
            off = 0xd;
        } else {
            return 0;
        }
        const u32 shift = (static_cast<u32>(4 - static_cast<i32>(idx)) << 3) & 31u;  // sar masks cl
        return static_cast<u8>(static_cast<u32>(*reinterpret_cast<const i32*>(reinterpret_cast<const u8*>(&jo::self<jo::OrderEntry>(this)->kind) + off) >> shift) & 0xffu);  // off: target / radius / spread
    }

    // ------------------------------------------------------------------ 0x004c0e80
    i32 ret1_4c0e80(void*) { return 1; }

    // ------------------------------------------------------------------ 0x004c1150
    i32 is_idle(void* unit) {
        c_is_idle.hit();
        if (jo::self<jo::Task>(this)->current.expire_time < global<u32>(d::g_now)) return 1;
        const double v = joan::orig::arg::as<double>(joan::orig::as_unit(unit)->anim_phase());  // 0x005045f0
        const double k = global<float>(d::k_9cf5c4);
        return (v < k || v > k) ? 0 : 1;  // test ah,0x40: equal or unordered -> 1
    }

    // ------------------------------------------------------------------ 0x004c15e0
    u32 order_18(void* unit) {
        joan::orig::as_unit(unit)->set_action(0x18, 0, 1);  // 0x00501190
        return 0;  // void callee; Action::dispatch discards the result
    }

    // ------------------------------------------------------------------ 0x004c1890
    void nop_4c1890(u32) {}

    // ------------------------------------------------------------------ 0x004c09a0
    void split(u8* unit) {
        c_split.hit();
        u8* const group = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->nation());  // 0x00509400
        if (group == nullptr) {
            jo::self<jo::Army>(this)->u32_1d = 0;
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->leads()) == 0) {  // 0x0044a660
            jo::self<jo::Army>(this)->u32_1d = 0;
            return;
        }
        i32 i = static_cast<i32>(global<i16>(d::g_a48d8c));
        for (; i > 0; --i) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kGroups)->get(joan::orig::arg::i16(static_cast<i16>(i))));  // 0x0042d0a0
            if (static_cast<i32>(jo::as_unit_type(e)->race) == static_cast<i32>(jo::as_unit(unit)->race_id) &&
                static_cast<i32>(jo::as_unit_type(e)->rank) == 0)
                break;
        }
        if (i <= 0) {
            jo::self<jo::Army>(this)->u32_1d = 0;
            return;
        }
        // Original frame: ids[20] at ebp-0x64, then n, entity, index, group, this (ebp-0x14 ..
        // ebp-4). Kept as one array of dwords for the ids, n and the index; the group and this pointers (dwords
        // of the frame on x86) stay pointers here.
        enum { kN = 20, kE = 21, kI = 22 };
        u32 fr[32] = {};
        fr[kI] = static_cast<u32>(i);
        fr[kN] = 0;
        while (static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count) < 0x14 &&
               joan::orig::arg::as<i32>(joan::orig::as_nation(group)->spend(0, 0, 0)) !=
                   0) {  // 0x004b5bf0
            const i32 id = joan::orig::arg::as<i32>(joan::orig::as_units(kEntities)->create_51dfc0(static_cast<i32>(fr[kI])));  // 0x0051dfc0
            u8* const o = entity(id);
            joan::orig::as_unit(o)->set_level_state(0);                                  // 0x005089e0
            joan::orig::as_unit(o)->set_side(joan::orig::arg::i32(jo::as_unit(unit)->side));                 // 0x00509250
            joan::orig::as_unit(o)->set_group(jo::as_unit(unit)->world);               // 0x004fed00
            joan::orig::as_unit(o)->link_to(static_cast<i32>(jo::as_unit(unit)->id));  // 0x00438f80
            jo::view<u16>(jo::as_unit(o)->layer) = 1;
            fr[fr[kN]] = static_cast<u32>(id);
            fr[kN] = fr[kN] + 1;
        }
        Vec3 pos;
        joan::orig::as_unit(unit)->position(&pos);           // 0x004fedb0
        const float r = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading());  // 0x004ff850
        for (i32 k = 0; k < static_cast<i32>(fr[kN]); ++k) {
            u8* const o = entity(static_cast<i32>(fr[k]));
            Vec3 tmp;
            const Vec3* const sum = joan::orig::arg::as<const Vec3*>(joan::orig::vec_add(&tmp, &pos, joan::orig::arg::ptr(jo::as_unit(o)->formation_offset)));  // 0x0060cc20
            const Vec3 v = *sum;
            joan::orig::as_ground_object(o)->set_position(rebits<Vec>(v));  // 0x004fedf0
            joan::orig::as_unit(o)->set_target(joan::orig::arg::f32(r));  // 0x004ff870
        }
        jo::as_army(this)->u32_1d = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::dtor_4baba0", 0x004baba0, &H::dtor_4baba0);
B("Packed::byte_at", 0x004bba00, &H::byte_at);
B("Army::split", 0x004c09a0, &H::split);
B("Obj::ret1_4c0e80", 0x004c0e80, &H::ret1_4c0e80);
B("Unit::is_idle", 0x004c1150, &H::is_idle);
B("Obj::order_18", 0x004c15e0, &H::order_18);
B("Obj::nop_4c1890", 0x004c1890, &H::nop_4c1890);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

struct H {
    void nop0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Obj::nop_4bac10", 0x004bac10, &H::nop0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {


JOAN_COUNTER(c_pack, "pack_chars_12");

// 0x004bbac0
char* JOAN_CDECL pack_chars_12(char* s, u32* a, u32* b, u32* c) {
    c_pack.hit();
    *a = 0;
    *b = 0;
    *c = 0;
    i32 i = 0;
    for (;; ++i) {
        const i32 ch = static_cast<i8>(s[i]);
        if (ch == 0) break;
        const u32 v = static_cast<u32>(ch);
        if (i < 4) {
            *a |= v << ((3 - i) * 8);
        } else if (i < 8) {
            *b |= v << ((7 - i) * 8);
        } else if (i < 0xc) {
            *c |= v << ((0xb - i) * 8);
        }
    }
    return s + i;  // eax left by the original
}

struct H {

    u32 clear_1() { joan::orig::as_dynarray(this)->zap_584580(1); return 0; }  // 0x004bac30 -> 0x00584580 (void; result unused)

    i32 type_ok_1c_49(void* unit) {  // 0x004c0d40
        const i32 k = jo::as_unit(unit)->anim_kind;
        if (k == 0x1c || static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x49) return 0;
        return 1;
    }
    i32 type_ok_2b_2d(void* unit) {  // 0x004c0ee0
        const i32 k = jo::as_unit(unit)->anim_kind;
        if (k == 0x2b || static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x2d) return 0;
        return 1;
    }
    i32 always_1(void*) { return 1; }  // 0x004c11b0 (compare of +0x3a with dword 0xa500ba is dead)
    void* clear_8() {               // 0x004c1940
        jo::self<jo::PemiRes>(this)->loaded = 0;
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("X::clear_1", 0x004bac30, &H::clear_1);
B99("pack_chars_12", 0x004bbac0, &pack_chars_12);
B99("X::type_ok_1c_49", 0x004c0d40, &H::type_ok_1c_49);
B99("X::type_ok_2b_2d", 0x004c0ee0, &H::type_ok_2b_2d);
B99("X::always_1", 0x004c11b0, &H::always_1);
B99("X::clear_8", 0x004c1940, &H::clear_8);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {


JOAN_COUNTER(c_event_add, "EventList::add");
JOAN_COUNTER(c_check_unit, "Obj::check_unit");

i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct Entry21 {
    u8 b[0x24];  // 0x004bb950 writes 0x21 bytes; list element size 0x21
};

struct H {
    // ------------------------------------------------------------------ 0x004bac60
    i32 event_add(void* a1, i32 a2, i32 a3, i32 a4, i32 a5, i32 a6, i32 a7, i32 a8, i32 a9, i32 a10) {
        c_event_add.hit();
        Entry21 e;
        joan::orig::as_order_entry(&e)->set(joan::orig::arg::u32(a2), static_cast<std::uint8_t>(joan::orig::arg::u32(a3)), joan::orig::arg::u32(a4), joan::orig::arg::u32(a5), joan::orig::arg::u32(a6), joan::orig::arg::u32(a7), joan::orig::arg::u32(a8), joan::orig::arg::u32(a9), joan::orig::arg::u32(a10));  // 0x004bb950
        if ((joan::orig::arg::as<u32>(joan::orig::as_order_entry(&jo::self<jo::Order>(this)->current)->same_id(&e)) & 0xffu) != 0) {  // 0x004bb9d0
            if (jo::self<jo::Order>(this)->current.kind == 4) {
                jo::view<i32>(jo::self<jo::Order>(this)->current.expire_time) = a8;
            } else if (jo::self<jo::Order>(this)->current.kind == 0xc) {
                if (!(jo::self<jo::Order>(this)->current.target == a4 && jo::self<jo::Order>(this)->current.radius == a5 && jo::self<jo::Order>(this)->current.spread == a6)) {
                    joan::orig::as_order(this)->finish(joan::orig::arg::mptr(a1));  // 0x004378a0
                    jo::self<jo::Order>(this)->current.target = a4;
                    jo::self<jo::Order>(this)->current.radius = a5;
                    jo::self<jo::Order>(this)->current.spread = a6;
                    jo::self<jo::Order>(this)->current.started = a7;
                    jo::self<jo::Order>(this)->current.kind = 0xc;
                }
            }
            return 0;
        }
        i32 pos = list_count(this) + 1;
        for (i32 i = list_count(this); i > 0; --i) {
            u8* const item = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_484570(joan::orig::arg::i32(i)));  // 0x00484570
            if ((joan::orig::arg::as<u32>(joan::orig::as_order_entry(&e)->same_id(joan::orig::arg::mptr(item))) & 0xffu) != 0) return 0;  // 0x004bb9d0
            if (jo::as_order_entry(e.b)->kind == jo::as_order_entry(item)->kind) {
                jo::as_order_entry(item)->target = jo::as_order_entry(e.b)->target;
                jo::as_order_entry(item)->radius = jo::as_order_entry(e.b)->radius;
                jo::as_order_entry(item)->spread = jo::as_order_entry(e.b)->spread;
                jo::as_order_entry(item)->started = jo::as_order_entry(e.b)->started;
                jo::view<i32>(jo::as_order_entry(item)->expire_time) = jo::view<i32>(jo::as_order_entry(e.b)->expire_time);
                return 1;
            }
            if (joan::orig::arg::as<i32>(joan::orig::cmp_byte4_desc(joan::orig::arg::mptr(static_cast<void*>(item)), joan::orig::arg::mptr(static_cast<void*>(&e)))) > 0) pos = i;
        }
        joan::orig::dynarray(this)->insert_at(joan::orig::arg::i32(pos), &e);  // 0x005848d0
        return pos;
    }

    // ------------------------------------------------------------------ 0x004bf5f0
    std::uintptr_t check_unit(void* unit) {
        c_check_unit.hit();
        const i32 k = static_cast<i32>(jo::as_unit(unit)->anim_kind);
        if (k == 0x40 || k == 0x41 || k == 0x42 || k == 0x3c || k == 0x29 || k == 0x4e) {
            jo::self<jo::Order>(this)->current.kind = 0;
            return u(this);
        }
        if (k != 0xb && k != 0xa && k != 9) {
            std::uintptr_t r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_unit(unit)->set_213(0u));  // 0x00500d10
            const i32 s = jo::self<jo::Order>(this)->current.target;
            if (s >= 0) {
                if (s <= 1) {
                    joan::orig::as_unit(unit)->set_action(0xa, 0, 1);  // 0x00501190
                } else if (s == 2) {
                    joan::orig::as_unit(unit)->set_action(0xb, 0, 1);  // 0x00501190
                }
            }
            return r;  // the Ai::update table call discards the result
        }
        const double x = joan::orig::arg::as<double>(joan::orig::as_unit(unit)->anim_phase());  // 0x005045f0 (st0)
        const double zero = static_cast<double>(global<float>(d::k_9cf5c4));
        // test ah,0x40 je: equal or unordered -> 0x00501190.
        if (!(x < zero) && !(x > zero)) {
            joan::orig::as_unit(unit)->set_action(static_cast<i32>(jo::as_unit(unit)->anim_kind), 0, 1);  // 0x00501190
            return 0;  // void callee; result unused
        }
        const u32 t = jo::self<jo::Order>(this)->current.expire_time;
        if (t < global<u32>(d::g_now)) jo::self<jo::Order>(this)->current.kind = 0;
        return t;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("EventList::add", 0x004bac60, &H::event_add);
B98("Obj::check_unit", 0x004bf5f0, &H::check_unit);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
namespace {
namespace game_b95_03 {


JOAN_COUNTER(c_action_name, "Action::from_name");
JOAN_COUNTER(c_update_state, "Ai::update_unit_state");

using joan::orig::helpers::stricmp_;  // _stricmp

// ------------------------------------------------------------------------------- cdecl
i32 JOAN_CDECL cmp_byte4_desc(const void* a, const void* b) {  // 0x004bae70
    const i32 x = jo::view<i8>(jo::as_order_entry(a)->priority);
    const i32 y = jo::view<i8>(jo::as_order_entry(b)->priority);
    if (x == y) return 0;
    return x >= y ? -1 : 1;
}

struct ActionName {
    img::Addr name;
    i32 value;
};
const ActionName kActions[23] = {
    {d::s_COMBO_ATTACK, 0x01},  // COMBO_ATTACK
    {d::s_DOWN_ATTACK, 0x13},  // DOWN_ATTACK
    {d::s_ENERGY_ATTACK, 0x14},  // ENERGY_ATTACK
    {d::s_RUSH_ATTACK, 0x15},  // RUSH_ATTACK
    {d::s_SHOOT, 0x02},  // SHOOT
    {d::s_APPROACH_ENEMY, 0x0c},  // APPROACH_ENEMY
    {d::s_APPROACH_ITEM, 0x0d},  // APPROACH_ITEM
    {d::s_APPROACH_PLACE, 0x0e},  // APPROACH_PLACE
    {d::s_FOLLOW_FORMATION, 0x0f},  // FOLLOW_FORMATION
    {d::s_FOLLOW_UNIT, 0x10},  // FOLLOW_UNIT
    {d::s_BLOCK, 0x04},  // BLOCK
    {d::s_JUMP, 0x03},  // JUMP
    {d::s_TURN, 0x0a},  // TURN
    {d::s_MOVE, 0x05},  // MOVE
    {d::s_SHIFT, 0x06},  // SHIFT
    {d::s_MOVE_TO_TARGET, 0x07},  // MOVE_TO_TARGET
    {d::s_MOVE_TO_RELATIVE_TARGET, 0x08},  // MOVE_TO_RELATIVE_TARGET
    {d::s_MOVE_ON_TRACK, 0x09},  // MOVE_ON_TRACK
    {d::s_IDLE, 0x0b},  // IDLE
    {d::s_GATE_OPEN, 0x11},  // GATE_OPEN
    {d::s_RECRUIT_TROOP, 0x12},  // RECRUIT_TROOP
    {d::s_SPECIAL_MOVEMENT, 0x16},  // SPECIAL_MOVEMENT
    {img::kNoAddr, 0},   // terminator (not a name)
};

i32 JOAN_CDECL action_from_name(const char* name) {  // 0x004bbbc0
    c_action_name.hit();
    for (i32 i = 0; kActions[i].name != img::kNoAddr; ++i) {
        if (stricmp_(name, str(kActions[i].name)) == 0) return kActions[i].value;
    }
    return 0;
}

struct H {
    // ------------------------------------------------------------------------ 0x004bf780
    void update_unit_state(void* unit) {
        c_update_state.hit();
        const i32 kind = jo::as_unit(unit)->anim_kind;
        if (kind == 0x40 || kind == 0x41 || kind == 0x42 || kind == 0x3c || kind == 0x29 || kind == 0x4e ||
            kind == 0x22 || kind == 0x1e || kind == 0x1f || kind == 0x20 || kind == 0x21) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            return;
        }
        if (kind == 0x0e || kind == 0x0f || kind == 0x10 || kind == 0x15 || kind == 0x16 || kind == 0x17) {
            const double phase = joan::orig::arg::as<double>(joan::orig::as_unit(unit)->anim_phase());  // 0x005045f0
            if (phase == 0.0 || phase != phase) {                     // fcomp 0.0f, test ah,0x40 je
                joan::orig::as_unit(unit)->play_idle(0, 1);               // 0x00508b90
                jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            } else if (jo::self<jo::Order>(this)->current.expire_time < global<u32>(d::g_now)) {
                joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
                jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            }
            return;
        }
        joan::orig::as_unit(unit)->set_213(0u);  // 0x00500d10
        switch (jo::view<u32>(jo::self<jo::Order>(this)->current.target)) {
            case 1: joan::orig::as_unit(unit)->set_action(0x0e, 0, 1); break;  // 0x00501190
            case 2: joan::orig::as_unit(unit)->set_action(0x0f, 0, 1); break;
            case 3: joan::orig::as_unit(unit)->set_action(0x10, 0, 1); break;
            case 4: joan::orig::as_unit(unit)->set_action(0x15, 0, 1); break;
            default: break;
        }
    }

    // ------------------------------------------------------------------------ 0x004c0f90
    u32 kind_ok(void* unit) {
        const i32 kind = jo::as_unit(unit)->anim_kind;
        return (kind == 0xb || kind == 0xa || kind == 9) ? 0u : 1u;
    }

    // ------------------------------------------------------------------------ 0x004c15a0
    void* stop_unit(void* unit) {
        joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
        jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("cmp_byte4_desc", 0x004bae70, &cmp_byte4_desc);
B95("Action::from_name", 0x004bbbc0, &action_from_name);
B95("Ai::update_unit_state", 0x004bf780, &H::update_unit_state);
B95("Ai::kind_ok", 0x004c0f90, &H::kind_ok);
B95("Ai::stop_unit", 0x004c15a0, &H::stop_unit);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {


void unit_resume(void* u) { joan::orig::as_squad(u)->clear(); }  // 0x00422f10
void unit_set_order(void* u) { joan::orig::as_unit(u)->play_idle(0, 1); }  // 0x00508b90
void unit_special(void* u) { joan::orig::as_unit(u)->start_paired_anim(); }  // 0x004533e0

JOAN_COUNTER(c_finish_14e0, "Cmd::finish_4c14e0");
JOAN_COUNTER(c_finish_1670, "Cmd::finish_4c1670");
JOAN_COUNTER(c_finish_17a0, "Cmd::finish_4c17a0");

// unit "finish" tail shared by 0x004c14e0 / 0x004c1670: resume unless the unit has a live
// group (+0x3c9, +0x339 -> +0x5c) with more than one member and +0x3e6 < 1.
void finish_resume(std::uint8_t* u) {
    if (jo::view<std::int32_t>(jo::as_unit(u)->name_index) == 0 || jo::as_unit(u)->cell == nullptr ||
        jo::as_world_cell(jo::as_unit(u)->cell)->tracks == nullptr) {
        unit_resume(u);
        return;
    }
    const std::int32_t n = joan::orig::arg::as<std::int32_t>(joan::orig::as_list(&jo::as_unit(u)->path)->count_66e8a0());  // 0x0066e8a0
    if (n <= 1 || jo::as_unit(u)->path_index >= 1) unit_resume(u);
}
bool special_kind(std::uint8_t* u) {
    if (jo::as_unit(u)->mount_id == 0) return false;
    const std::int32_t k = jo::as_unit(u)->anim_kind;
    return k == 0x44 || k == 0x45 || k == 0x46;
}

struct H {
    // 0x004baee0
    std::uint32_t forward_584a10(std::int32_t a) { return thiscall<std::uint32_t>(fn::linkout_584a10, this, a); }  // 0x00584a10

    // 0x004bb8d0
    std::int32_t contains(std::int32_t id) {
        if (jo::self<jo::Order>(this)->current.kind == id) return 1;
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            const std::int32_t* const e = joan::orig::arg::as<std::int32_t*>(joan::orig::as_list(this)->at_484570(joan::orig::arg::i32(i)));  // 0x00484570
            if (*e == id) return 1;
        }
        return 0;
    }

    // 0x004c0c90
    std::int32_t accepts(std::uint8_t* u) {
        const std::int32_t k = jo::as_unit(u)->anim_kind;
        if (k == 0x1e || k == 0x1f || k == 0x20 || k == 0x21) return 0;
        return 1;
    }

    // 0x004c1270
    void finish_1270(std::uint8_t* u) {
        unit_set_order(u);
        jo::self<jo::Ai>(this)->current.kind = 0;
    }
    // 0x004c1390
    void finish_1390(std::uint8_t* u) {
        unit_resume(u);
        jo::view<std::int32_t>(jo::as_unit(u)->wait_count) = jo::view<std::int32_t>(jo::as_unit(u)->wait_limit);
        unit_set_order(u);
        jo::self<jo::Ai>(this)->current.kind = 0;
    }
    // 0x004c14e0
    void finish_14e0(std::uint8_t* u) {
        c_finish_14e0.hit();
        finish_resume(u);
        jo::view<std::int32_t>(jo::as_unit(u)->wait_count) = jo::view<std::int32_t>(jo::as_unit(u)->wait_limit);
        unit_set_order(u);
        jo::self<jo::Ai>(this)->current.kind = 0;
    }
    // 0x004c1670
    void finish_1670(std::uint8_t* u) {
        c_finish_1670.hit();
        if (jo::self<jo::Ai>(this)->current.started != 0) jo::view<std::int32_t>(jo::as_unit(u)->name_index) = 0;
        if (special_kind(u))
            unit_special(u);
        else
            unit_set_order(u);
        finish_resume(u);
        jo::view<std::int32_t>(jo::as_unit(u)->wait_count) = jo::view<std::int32_t>(jo::as_unit(u)->wait_limit);
        jo::self<jo::Ai>(this)->current.kind = 0;
    }
    // 0x004c17a0
    void finish_17a0(std::uint8_t* u) {
        c_finish_17a0.hit();
        jo::view<std::int32_t>(jo::as_unit(u)->name_index) = 0;
        if (special_kind(u))
            unit_special(u);
        else
            unit_set_order(u);
        unit_resume(u);
        jo::view<std::int32_t>(jo::as_unit(u)->wait_count) = jo::view<std::int32_t>(jo::as_unit(u)->wait_limit);
        jo::self<jo::Ai>(this)->current.kind = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("List::forward_584a10", 0x004baee0, &H::forward_584a10);
B("List::contains", 0x004bb8d0, &H::contains);
B("Cmd::accepts", 0x004c0c90, &H::accepts);
B("Cmd::finish_4c1270", 0x004c1270, &H::finish_1270);
B("Cmd::finish_4c1390", 0x004c1390, &H::finish_1390);
B("Cmd::finish_4c14e0", 0x004c14e0, &H::finish_14e0);
B("Cmd::finish_4c1670", 0x004c1670, &H::finish_1670);
B("Cmd::finish_4c17a0", 0x004c17a0, &H::finish_17a0);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

struct H {
    // ------------------------------------------------------------------------ 0x004baf10
    i32 copy_first() {
        if (joan::orig::as_dynarray(this)->size() == 0) return 0;  // 0x00417ad0
        const u8* const src = joan::orig::arg::as<const u8*>(joan::orig::as_list(this)->at_484570(1));  // 0x00484570
        std::memcpy(&jo::self<jo::Order>(this)->current, src, sizeof(jo::OrderEntry));
        joan::orig::as_list(this)->forward_584a10(1);  // 0x004baee0
        return 1;
    }

    // ------------------------------------------------------------------------ 0x004c0c40
    i32 type_ok(const void* p) {
        const i32 t = jo::as_unit(p)->anim_kind;
        return (t == 0x1c || t == 0x49) ? 0 : 1;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("X::copy_first", 0x004baf10, &H::copy_first);
B96("X::type_ok", 0x004c0c40, &H::type_ok);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {


JOAN_COUNTER(c_ai, "Ai::update_4baf80");

struct H {
    // ------------------------------------------------------------------ 0x004baf80
    void ai_update_4baf80(void* unit) {
        c_ai.hit();
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            void* const order = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_484570(joan::orig::arg::i32(i)));  // 0x00484570
            if (jo::as_order_entry(order)->expire_time < global<u32>(d::g_now)) joan::orig::as_list(this)->forward_584a10(joan::orig::arg::i32(i));  // 0x004baee0
        }
        const i32 kind = jo::as_unit(unit)->anim_kind;
        if (kind == 0x40 || kind == 0x41 || kind == 0x42 || kind == 0x3c || kind == 0x29 || kind == 0x4e) return;
        if (jo::view<u32>(jo::self<jo::Order>(this)->current.kind) == 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_order(this)->pop_first()) == 0) return;  // 0x004baf10
        } else if (joan::orig::as_dynarray(this)->size() != 0) {
            void* const first = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_484570(i32{1}));
            if (static_cast<i32>(jo::view<i8>(jo::as_order_entry(first)->priority)) > static_cast<i32>(jo::view<i8>(jo::self<jo::Order>(this)->current.priority)) &&
                joan::orig::arg::as<i32>(joan::orig::as_order(this)->check_condition(unit)) != 0) {  // 0x004bb3e0
                joan::orig::as_action(this)->dispatch(joan::orig::arg::mptr(unit));         // 0x004bb660
                joan::orig::as_order(this)->pop_first();               // 0x004baf10
            }
        }
        static constexpr fn::Code kTable[22] = {
            fn::Combo_update, fn::ArmyOrder_on_4bd090, fn::Task_update_bf510, fn::UnitOrder_check_4bf240, fn::Obj_check_unit_4bf5f0, fn::Ai_update_unit_state, fn::kNoCode, fn::kNoCode,
            fn::Order_step_patrol_4bfd70, fn::X_check_target, fn::Obj_check_unit_4c25a0, fn::kNoCode,          fn::Order_step_follow_4bdbc0, fn::Squad_think_4bdf90, fn::Escort_update_4be770, fn::Escort_update_4bed40,
            fn::Light_trigger, fn::Army_split, fn::ArmyOrder_on_4bc8a0, fn::ArmyOrder_on_4bca60, fn::ArmyOrder_on_4bcdb0, fn::ArmyOrder_on_4c2680};
        const u32 k = jo::view<u32>(jo::self<jo::Order>(this)->current.kind) - 1;
        if (k > 0x15) return;
        if (k == 6) {  // type 7: integer position
            const float x = static_cast<float>(jo::self<jo::Order>(this)->current.target);
            const float y = static_cast<float>(jo::self<jo::Order>(this)->current.radius);
            const float z = static_cast<float>(jo::self<jo::Order>(this)->current.spread);
            joan::orig::as_order(this)->step_move_4bfa10(joan::orig::arg::mptr(unit), x, y, z);  // 0x004bfa10
        } else if (k == 7) {  // type 8: integer offset + unit position
            const Fpu fpu{true};
            const float x = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::Order>(this)->current.target), reinterpret_cast<const float*>(jo::as_unit(unit)->position_)[0]));
            const float y = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::Order>(this)->current.radius), reinterpret_cast<const float*>(jo::as_unit(unit)->position_)[1]));
            const float z = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::Order>(this)->current.spread), reinterpret_cast<const float*>(jo::as_unit(unit)->position_)[2]));
            joan::orig::as_order(this)->step_move_4bfa10(joan::orig::arg::mptr(unit), x, y, z);  // 0x004bfa10
        } else if (k == 11) {  // type 12
            joan::orig::as_ai(this)->follow_4bd420(joan::orig::arg::mptr(unit), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.target)));  // 0x004bd420
        } else {
            thiscall<u32>(kTable[k], this, unit);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Ai::update_4baf80", 0x004baf80, &H::ai_update_4baf80);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b106_05.cpp (game/b106_05)
namespace {
namespace game_b106_05 {


struct H {
    // ------------------------------------------------------------------------ 0x004bb3e0
    u32 dispatch(void* unit) {
        switch (jo::view<u32>(jo::self<jo::Ai>(this)->current.kind)) {
            case 0: return 1;
            case 1: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_not_kind_1c_49(joan::orig::arg::mptr(unit)));   // 0x004c0c40
            case 2: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_kind_allowed(joan::orig::arg::mptr(unit)));   // 0x004c0d90
            case 3: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_unit_not_moving(joan::orig::arg::mptr(unit)));   // 0x004c0f30
            case 4: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_not_kind_2b_2d(joan::orig::arg::mptr(unit)));   // 0x004c0ee0
            case 5: return joan::orig::arg::as<u32>(joan::orig::as_ai(this)->kind_ok(joan::orig::arg::mptr(unit)));   // 0x004c0f90
            case 6: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_true_6(joan::orig::arg::mptr(unit)));   // 0x004c0ff0
            case 7: return joan::orig::arg::as<u32>(joan::orig::as_ai(this)->cond_always_1_425b60(joan::orig::arg::mptr(unit)));   // 0x00425b60
            case 8: return joan::orig::arg::as<u32>(joan::orig::as_ai(this)->cond_always_1_425b60(joan::orig::arg::mptr(unit)));   // 0x00425b60
            case 9: return joan::orig::arg::as<u32>(joan::orig::as_ai(this)->cond_always_1_42f5b0(joan::orig::arg::mptr(unit)));   // 0x0042f5b0
            case 10: return joan::orig::arg::as<u32>(joan::orig::as_action(this)->before_now(joan::orig::arg::mptr(unit)));  // 0x00433a90
            case 11: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_timer_done(joan::orig::arg::mptr(unit)));  // 0x004c1090
            case 12: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_true_c(joan::orig::arg::mptr(unit)));  // 0x004c0e40
            case 13: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_true_d(joan::orig::arg::mptr(unit)));  // 0x004c0e60
            case 14: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_true_e(joan::orig::arg::mptr(unit)));  // 0x004c0e80
            case 15: return joan::orig::arg::as<u32>(joan::orig::as_ai(this)->cond_always_1_429b70(joan::orig::arg::mptr(unit)));  // 0x00429b70
            case 16: return joan::orig::arg::as<u32>(joan::orig::as_ai(this)->cond_always_1_42a1a0(joan::orig::arg::mptr(unit)));  // 0x0042a1a0
            case 17: return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->is_idle_or_zero(joan::orig::arg::mptr(unit)));  // 0x004c10f0
            case 18: return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->is_idle(joan::orig::arg::mptr(unit)));  // 0x004c1150
            case 19: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_not_kind_1c_49_b(joan::orig::arg::mptr(unit)));  // 0x004c0d40
            case 20: return joan::orig::arg::as<u32>(joan::orig::as_cmd(this)->accepts(joan::orig::arg::mptr(unit)));  // 0x004c0c90
            case 21: return joan::orig::arg::as<u32>(joan::orig::as_action(this)->not_kind_53(joan::orig::arg::mptr(unit)));  // 0x0043bbc0
            case 22: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->cond_true_16(joan::orig::arg::mptr(unit)));  // 0x004c11b0
            default: return 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_05", name, addr, fn)
B("Combo::dispatch", 0x004bb3e0, &H::dispatch);
#undef B

}  // namespace game_b106_05
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {


JOAN_COUNTER(c_dispatch, "Action::dispatch");

// 0x004bb660 handler thunks for +0x1d = 1..22.
constexpr fn::Code kHandlers[22] = {
    fn::Obj_release_unit,  //  1: 0x004c11f0
    fn::Action_stop_unit,  //  2: 0x00435280
    fn::Obj_order_18,  //  3: 0x004c15e0
    fn::Ai_stop_unit,  //  4: 0x004c15a0
    fn::call_508b90,  //  5: 0x0045c430
    fn::Obj_call_508b90,  //  6: 0x0043fbe0
    fn::Cmd_finish_4c1670,  //  7: 0x004c1670
    fn::Cmd_finish_4c1670,  //  8: 0x004c1670
    fn::Cmd_finish_4c17a0,  //  9: 0x004c17a0
    fn::X_stop_unit,  // 10: 0x004c1860
    fn::Obj_nop_4c1890,  // 11: 0x004c1890
    fn::Order_finish,  // 12: 0x004378a0
    fn::Cmd_finish_4c1390,  // 13: 0x004c1390
    fn::Order_finish_unit,  // 14: 0x0043da10
    fn::Holder_release_unit,  // 15: 0x004c1480
    fn::Cmd_finish_4c14e0,  // 16: 0x004c14e0
    fn::Obj_nop_42d410,  // 17: 0x0042d410
    fn::Obj_nop_425c50,  // 18: 0x00425c50
    fn::Handler_reset_1d,  // 19: 0x004c1230
    fn::Cmd_finish_4c1270,  // 20: 0x004c1270
    fn::Order_stop_unit,  // 21: 0x00434210
    fn::Task_stop_unit,  // 22: 0x004c18f0
};

struct H {
    // ---- 0x004bb660 Action::dispatch(arg) ret 4 --------------------------------------------
    // EAX: the handler's result, or `this` when +0x1d is out of range.
    std::uint32_t dispatch(void* arg) {
        c_dispatch.hit();
        std::uint32_t r = joan::call::register_low_bits(this);  // every caller discards the result
        const std::uint32_t k = jo::view<std::uint32_t>(jo::self<jo::Order>(this)->current.kind) - 1u;
        if (k <= 0x15u) r = thiscall<std::uint32_t>(kHandlers[k], this, arg);
        jo::view<std::uint32_t>(jo::self<jo::Order>(this)->current.kind) = 0;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Action::dispatch", 0x004bb660, &H::dispatch);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {


JOAN_COUNTER(c_release_unit, "Holder::release_unit");

struct H {
    // ------------------------------------------------------------------------ 0x004bb950
    u32 record_set(u32 a0, u8 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8) {
        jo::view<u32>(jo::self<jo::OrderEntry>(this)->kind) = a0;
        jo::self<jo::OrderEntry>(this)->priority = a1;
        jo::view<u32>(jo::self<jo::OrderEntry>(this)->target) = a2;
        jo::view<u32>(jo::self<jo::OrderEntry>(this)->radius) = a3;
        jo::view<u32>(jo::self<jo::OrderEntry>(this)->spread) = a4;
        jo::view<u32>(jo::self<jo::OrderEntry>(this)->started) = a5;
        jo::view<u32>(jo::self<jo::OrderEntry>(this)->arg_15) = a7;
        jo::view<u32>(jo::self<jo::OrderEntry>(this)->arg_19) = a8;
        jo::self<jo::OrderEntry>(this)->expire_time = a6;
        return a8;
    }

    // ------------------------------------------------------------------------ 0x004c1480
    void release_unit(void* unit) {
        c_release_unit.hit();
        joan::orig::as_squad(unit)->clear();  // 0x00422f10
        jo::as_unit(unit)->wait_count = jo::as_unit(unit)->wait_limit;
        joan::orig::as_unit(unit)->play_idle(i32{0}, i32{1});  // 0x00508b90
        jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Record::set", 0x004bb950, &H::record_set);
B93("Holder::release_unit", 0x004c1480, &H::release_unit);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {


struct H {
    // ------------------------------------------------------------------ 0x004bb9d0
    std::uintptr_t same_id(const void* other) {
        const u32 b = jo::self<jo::OrderEntry>(this)->kind == jo::as_order_entry(other)->kind ? 1u : 0u;
        return (u(this) & ~std::uintptr_t{0xff}) | b;
    }

    // ------------------------------------------------------------------ 0x004c0d90
    i32 kind_allowed(const void* unit) {
        const i32 k = static_cast<i32>(jo::as_unit(unit)->anim_kind);
        if (k == 0x30 || k == 0x32 || k == 0x33 || k == 0x36 || k == 0x38 || k == 0x39 || k == 0x34) return 0;
        return 1;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Obj::same_id", 0x004bb9d0, &H::same_id);
B97("Obj::kind_allowed", 0x004c0d90, &H::kind_allowed);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b115_08.cpp (game/b115_08)
namespace {
namespace game_b115_08 {

using i64 = std::int64_t;
using uptr = std::uintptr_t;

template <class T>
T& at(const void* p, i32 offset) {
    return *reinterpret_cast<T*>(static_cast<u8*>(const_cast<void*>(p)) + offset);
}
uptr addr(const void* p) { return reinterpret_cast<uptr>(p); }
u8* ptr(uptr a) { return reinterpret_cast<u8*>(a); }

JOAN_COUNTER(c_combo, "Combo::update_4bbf20");

// ---------------------------------------------------------------- 0x004bbf20 frame
// ebp-0x30 .. ebp+0x400 of Combo::update (see the header).
struct ComboFrame {
    static constexpr i32 kLow = 0x30;
    static constexpr i32 kSize = 0x30 + 0x400;
    alignas(4) u8 b[kSize];
    u8* p(i32 ebp_off) { return b + kLow + ebp_off; }
    template <class T>
    T& v(i32 ebp_off) {
        return at<T>(b, kLow + ebp_off);
    }
    void put(uptr a, u8 value) {
        if (a >= addr(b) && a < addr(b) + kSize - 1) b[a - addr(b)] = value;  // last byte stays 0
    }
};

struct H {
    // ------------------------------------------------------------------ 0x004bbf20
    void combo_reset(u8* unit) {
        jo::view<u8>(jo::as_unit(unit)->combo_state) = 0;
        jo::as_unit(unit)->armour_flags[0] = 0;
        joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
        jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
    }
    static u32 combo_key(uptr self, i32 i) {  // 0x004bba00 on self+0x1d
        // u8 result: every use reads the low byte only (i8 compares, set_action's i8 push argument, a stored byte).
        return joan::orig::arg::as<u32>(joan::orig::as_order_entry(&jo::as_ai(ptr(self))->current)->byte_at(static_cast<i8>(i / 4 + 1), static_cast<i8>(i % 4 + 1)));  // 0x004bba00
    }
    // !(v < 0) && !(v > 0): fcomp + test ah,0x40 (equal or unordered)
    static bool zero_or_nan(double v) { return !(v < 0.0) && !(v > 0.0); }
    double combo_time(u8* unit) { return joan::orig::arg::as<double>(joan::orig::as_unit(unit)->anim_phase()); }  // 0x005045f0
    void combo_update(u8* unit) {
        c_combo.hit();
        {
            const i32 s = jo::as_unit(unit)->anim_kind;
            if (s == 0x40 || s == 0x41 || s == 0x42 || s == 0x3c || s == 0x29 || s == 0x4e || s == 0x22 ||
                s == 0x1e || s == 0x1f || s == 0x20 || s == 0x21) {
                jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
                return;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0) {  // 0x00430190
            if (jo::as_unit(unit)->anim_kind == 0x49) {
                const float v = static_cast<float>(combo_time(unit));
                if (jo::as_unit(unit)->anim_set == nullptr) {
                    combo_reset(unit);
                    return;
                }
                u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(unit)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit(unit)->anim_entry))));  // 0x004168d0
                const u32 delta = global<u32>(d::g_now) - jo::as_actor(jo::as_unit(unit)->actor)->start_time;
                if (zero_or_nan(static_cast<double>(v))) {
                    combo_reset(unit);
                    return;
                }
                if (static_cast<i64>(jo::as_anim_group_rec(e)->combo_max) > static_cast<i64>(delta)) return;
                if (jo::self<jo::Ai>(this)->current.spread > 0) {
                    jo::self<jo::Ai>(this)->current.spread = jo::self<jo::Ai>(this)->current.spread - 1;
                    joan::orig::as_unit(unit)->set_action(0x49, 0x41, 1);  // 0x00501190
                    if (std::strcmp(joan::orig::arg::str(jo::as_unit(unit)->armour_flags), str(d::s_AAA)) == 0) {  // strcmp "AAA"
                        jo::view<u8>(jo::as_unit(unit)->combo_state) = 1;
                        jo::as_unit(unit)->armour_flags[1] = 0;
                        jo::as_unit(unit)->armour_flags[2] = 0;
                    }
                    return;
                }
                combo_reset(unit);
                return;
            }
            {
                const i32 s = jo::as_unit(unit)->anim_kind;
                if (s == 0x4a || s == 0x4b || s == 0x4c || s == 0x4d) {
                    if (jo::self<jo::Ai>(this)->current.expire_time < global<u32>(d::g_now)) jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
                    return;
                }
            }
            if (jo::as_unit(unit)->anim_kind == 0x2a || static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->dead())) != 0) {  // 0x0042d0e0
                jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
                return;
            }
            jo::self<jo::Ai>(this)->current.spread = jo::self<jo::Ai>(this)->current.spread - 1;
            joan::orig::as_unit(unit)->set_action(0x49, 0x41, 1);
            return;
        }
        if (jo::as_unit(unit)->anim_kind == 0x1c) {
            const uptr self = addr(this);
            i32 i = 0;
            for (; i < jo::as_unit(unit)->combo_state; ++i) {
                const i32 k = jo::view<i8>(jo::as_unit(unit)->armour_flags[i]);
                if (k != static_cast<i8>(combo_key(self, i))) break;
            }
            if (i != jo::as_unit(unit)->combo_state || static_cast<i8>(combo_key(self, i)) == 0) {
                if (zero_or_nan(combo_time(unit))) combo_reset(unit);
                return;
            }
            const float v2 = static_cast<float>(combo_time(unit));  // [ebp-0x1c]
            if (jo::as_unit(unit)->actor == nullptr) {
                joan::orig::as_unit(unit)->set_action(0x1c, joan::orig::arg::i32(combo_key(self, i)), 1);
                return;
            }
            if (jo::as_unit(unit)->anim_set == nullptr) {
                combo_reset(unit);
                return;
            }
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(unit)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit(unit)->anim_entry))));  // [ebp-0x24]
            const u32 delta = global<u32>(d::g_now) - jo::as_actor(jo::as_unit(unit)->actor)->start_time;  // [ebp-0x28]
            if (zero_or_nan(combo_time(unit))) {
                combo_reset(unit);
                return;
            }
            if (static_cast<i64>(jo::as_anim_group_rec(e)->combo_max) > static_cast<i64>(delta)) return;
            ComboFrame fr;
            std::memset(fr.b, 0, sizeof fr.b);
            fr.v<u32>(-0x28) = delta;
            fr.v<u8*>(-0x24) = e;  // x64: 8 bytes, its high half is overwritten by [ebp-0x20] below (never read back)
            fr.v<u32>(-0x20) = 0;
            fr.v<float>(-0x1c) = v2;
            fr.v<i32>(-0x18) = i;
            fr.v<uptr>(-0x4) = self;
            fr.v<u8*>(8) = unit;  // argument slot [ebp+8]
            std::memset(static_cast<void*>(fr.p(-0x30)), 0, 8u);                       // memset
            {  // strcpy(buf, unit+0x227), emulated into the frame block
                const u8* s = jo::as_unit(unit)->armour_flags;
                uptr d = addr(fr.p(-0x30));
                for (;;) {
                    const u8 c = *s++;
                    fr.put(d++, c);
                    if (c == 0) break;
                }
            }
            const u32 key = combo_key(fr.v<uptr>(-0x4), fr.v<i32>(-0x18));
            {
                u8* const un = fr.v<u8*>(8);
                const i32 n = jo::as_unit(un)->combo_state;
                fr.put(addr(fr.p(-0x30)) + static_cast<uptr>(static_cast<u32>(n)), static_cast<u8>(key));
            }
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_anim_table(jo::as_unit(fr.v<u8*>(8))->anim_set)->find(0x1c, joan::orig::arg::str(static_cast<void*>(fr.p(-0x30)))));  // 0x00416e30
            if (r != 0) {
                const u32 k2 = combo_key(fr.v<uptr>(-0x4), fr.v<i32>(-0x18));
                joan::orig::as_unit(fr.v<u8*>(8))->set_action(0x1c, joan::orig::arg::i32(k2), 1);
            }
            return;
        }
        {
            const i32 s = jo::as_unit(unit)->anim_kind;
            if (s == 0x23 || s == 0x24 || s == 0x25 || s == 0x26 || s == 0x27 || s == 0x28) {
                if (jo::self<jo::Ai>(this)->current.expire_time < global<u32>(d::g_now)) jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
                return;
            }
        }
        if (jo::as_unit(unit)->anim_kind == 0x2a || static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->dead())) != 0) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (jo::as_unit(unit)->action == 1) {
            if (jo::self<jo::Ai>(this)->current.expire_time < global<u32>(d::g_now)) jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (jo::self<jo::Ai>(this)->current.expire_time < global<u32>(d::g_now)) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->is_kind_special())) == 0) return;  // 0x00508ab0
        jo::view<u8>(jo::as_unit(unit)->combo_state) = 0;
        jo::as_unit(unit)->armour_flags[0] = 0;
        u8 buf[8];
        std::memset(static_cast<void*>(buf), 0, 8u);                       // memset
        buf[0] = static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_order_entry(&jo::self<jo::Ai>(this)->current)->byte_at(1, 1)));  // 0x004bba00
        if (jo::as_unit(unit)->anim_set == nullptr) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_anim_table(jo::as_unit(unit)->anim_set)->find(0x1c, joan::orig::arg::str(static_cast<void*>(buf)))) == 0) return;
        joan::orig::as_unit(unit)->set_action(0x1c, joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_order_entry(&jo::self<jo::Ai>(this)->current)->byte_at(1, 1))), 1);  // 0x004bba00 (set_action reads its low byte)
        joan::orig::as_squad(unit)->clear();  // 0x00422f10
        jo::as_unit(unit)->wait_count = jo::as_unit(unit)->wait_limit;
    }
};

#define B(name, address, fn) JOAN_REPLACE("game/b115_08", name, address, fn)
B("Combo::update", 0x004bbf20, &H::combo_update);
#undef B

}  // namespace game_b115_08
}  // namespace

// ---------------------------------------- from modules/game_b104_04.cpp (game/b104_04)
namespace {
namespace game_b104_04 {

using joan::orig::helpers::as_float;

void* const kUnits = obj(d::g_units);
void* const kArmies = obj(d::g_items);
u32 clock_ms() { return global<u32>(d::g_now); }

void* game_memset(void* p, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

i32 unit_dead(u32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_4bc8a0, "ArmyOrder::on_4bc8a0");
JOAN_COUNTER(c_4bca60, "ArmyOrder::on_4bca60");
JOAN_COUNTER(c_4bcdb0, "ArmyOrder::on_4bcdb0");
JOAN_COUNTER(c_4bd090, "ArmyOrder::on_4bd090");
JOAN_COUNTER(c_4c02b0, "Ballista::reset_4c02b0");

// unit (army member) helpers shared by the order handlers
i16 kind(const u8* u) { return jo::as_unit(u)->anim_kind; }
bool kind_busy(const u8* u) {  // 0x40 / 0x41 / 0x42 / 0x3c / 0x29 / 0x4e, tested in this order
    return kind(u) == 0x40 || kind(u) == 0x41 || kind(u) == 0x42 || kind(u) == 0x3c || kind(u) == 0x29 ||
           kind(u) == 0x4e;
}
bool kind_1e_21(const u8* u) {
    return kind(u) == 0x1e || kind(u) == 0x1f || kind(u) == 0x20 || kind(u) == 0x21;
}
float unit_wait(u8* u) { return joan::orig::arg::as<float>(joan::orig::as_unit(u)->anim_phase()); }  // 0x005045f0
// [0x215] == 0x1c && byte [0x233]: wait time == 0 (or NaN) -> clear [0x233] / [0x227]
void clear_idle_wait(u8* u) {
    if (kind(u) == 0x1c && jo::as_unit(u)->combo_state != 0) {
        const float v = unit_wait(u);
        const float z = global<float>(d::k_9cf5c4);
        if (!(v < z || v > z)) {  // test ah,0x40 jz skip: equal or unordered
            jo::view<u8>(jo::as_unit(u)->combo_state) = 0;
            jo::as_unit(u)->armour_flags[0] = 0;
        }
    }
}
void set_order(u8* u, i32 order) { joan::orig::as_unit(u)->set_action(joan::orig::arg::i32(order), 0, 1); }  // 0x00501190

struct H {
    // ---------------------------------------------------------------- 0x004bc8a0
    void on_4bc8a0(u8* u) {
        c_4bc8a0.hit();
        unit_wait(u);  // result stored to a dead local
        if (kind(u) == 0x22 || jo::self<jo::Ai>(this)->current.expire_time < clock_ms() || kind_busy(u)) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->mounted()) != 0 ||                // 0x00430190
            joan::orig::arg::as<i32>(joan::orig::as_world(u)->flag_1fd()) != 0 ||                // 0x0042d690
            joan::orig::arg::as<i8>(joan::orig::as_unit(u)->can_act()) == 0 ||                 // 0x00500ba0
            joan::orig::arg::as<i8>(joan::orig::as_unit(u)->has_order_for(joan::orig::arg::u32(i32{0x22}))) == 0) {      // 0x00500d40
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        clear_idle_wait(u);
        set_order(u, 0x22);
    }

    // ---------------------------------------------------------------- 0x004bca60
    void on_4bca60(u8* u) {
        c_4bca60.hit();
        unit_wait(u);
        if (jo::self<jo::Ai>(this)->current.expire_time < clock_ms() && joan::orig::arg::as<i32>(joan::orig::as_cmd(this)->accepts(joan::orig::arg::mptr(u))) != 0) {  // 0x004c0c90
            joan::orig::as_cmd(this)->finish_4c1270(joan::orig::arg::mptr(u));                                       // 0x004c1270
            return;
        }
        if (kind_1e_21(u) || kind_busy(u)) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->mounted()) != 0 || joan::orig::arg::as<i32>(joan::orig::as_world(u)->flag_1fd()) != 0 ||
            joan::orig::arg::as<i8>(joan::orig::as_unit(u)->has_order_for(joan::orig::arg::u32(i32{0x1e}))) == 0) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        clear_idle_wait(u);
        if (joan::orig::arg::as<i32>(joan::orig::unit_check_652c10(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(u)->id))))) == 0) {  // 0x00652c10 (reads a word)
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        const Fpu fpu{true};
        const i32 a = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->value_149());  // 0x00523700
        const float pct = static_cast<float>(
            fpu.div(fpu.mul(static_cast<double>(a), static_cast<double>(global<i32>(d::g_unit_egatk_mp))),
                    static_cast<double>(global<float>(d::flt_100))));
        const i32 b = joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::as_unit(u)->modifiers)->sum(i32{0x15}, static_cast<i32>(jo::as_unit(u)->id)));  // 0x00497e10
        const i32 d1 = joan::hook::x87::ftol(fpu.div(fpu.mul(static_cast<double>(b), static_cast<double>(pct)),
                                                      static_cast<double>(global<float>(d::flt_100))));
        const i32 c = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->hit_points_max());  // 0x00523320
        const i32 d2 = joan::hook::x87::ftol(
            fpu.div(fpu.mul(static_cast<double>(c), static_cast<double>(global<i32>(d::g_a3cd1c))),
                    static_cast<double>(global<float>(d::flt_100))));
        set_order(u, 0x1e);
        i32 apply = 1;
        if (unit_dead(global<u32>(d::g_player_unit_recno)) == 0) {
            u8* const rec = unit_rec(global<u32>(d::g_player_unit_recno));
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(rec)->hostile_to(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(u)->id))))) != 0) apply = 0;  // 0x005094d0
        }
        if (apply != 0) {
            joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(jo::as_unit(u)->mana - d1));  // 0x004fe5d0
            joan::orig::as_unit(u)->set_value_135(joan::orig::arg::i32(jo::as_unit(u)->hit_points - d2));  // 0x004fe510
        }
        jo::view<u32>(jo::as_unit(u)->last_ep_regen_ms) = clock_ms();
    }

    // ---------------------------------------------------------------- 0x004bcdb0
    void on_4bcdb0(u8* u) {
        c_4bcdb0.hit();
        unit_wait(u);
        if (jo::self<jo::Ai>(this)->current.expire_time < clock_ms() && joan::orig::arg::as<i32>(joan::orig::as_action(this)->not_kind_53(joan::orig::arg::mptr(u))) != 0) {  // 0x0043bbc0
            joan::orig::as_order(this)->stop_unit(joan::orig::arg::mptr(u));                                       // 0x00434210
            return;
        }
        if (kind_1e_21(u) || kind_busy(u)) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->mounted()) != 0 || joan::orig::arg::as<i32>(joan::orig::as_world(u)->flag_1fd()) != 0 ||
            joan::orig::arg::as<i8>(joan::orig::as_unit(u)->can_act()) == 0 || joan::orig::arg::as<i8>(joan::orig::as_unit(u)->has_order_for(joan::orig::arg::u32(i32{0x53}))) == 0) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        clear_idle_wait(u);
        if (jo::as_unit(u)->anim_set == nullptr) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_list(jo::as_unit(u)->anim_set)->index_of_416dc0(i32{0x53}));  // 0x00416dc0
        i32 cost = jo::as_anim_group_rec(joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(u)->anim_set)->entry(joan::orig::arg::u32(idx))))->min_mana;  // 0x004168d0
        if (jo::as_unit(u)->modifiers != nullptr) {
            const i32 m = joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::as_unit(u)->modifiers)->sum(i32{0x15}, static_cast<i32>(jo::as_unit(u)->id)));  // 0x00497e10
            cost = static_cast<i32>(static_cast<u32>(m) * static_cast<u32>(cost)) / 100;
        }
        if (jo::as_unit(u)->mana < cost) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(jo::as_unit(u)->mana - cost));  // 0x004fe5d0
        set_order(u, 0x53);
    }

    // ---------------------------------------------------------------- 0x004bd090
    void on_4bd090(u8* u) {
        c_4bd090.hit();
        if (kind_busy(u)) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (jo::self<jo::Ai>(this)->current.spread == 1) {
            if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_special()) != 0) jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;  // 0x00508ab0
            return;
        }
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_special()) != 0) {
            if (jo::self<jo::Ai>(this)->current.expire_time < clock_ms()) {
                jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
                return;
            }
            if (jo::self<jo::Ai>(this)->current.target == 1) {
                jo::view<u16>(jo::as_unit(u)->look_b) = 0;
                if (unit_dead(jo::view<u32>(jo::self<jo::Ai>(this)->current.radius)) == 0) {
                    jo::view<u16>(jo::as_unit(u)->look_a) = static_cast<u16>(jo::self<jo::Ai>(this)->current.radius);
                    const float v = joan::orig::arg::as<float>(joan::orig::as_unit(u)->heading_to_unit(static_cast<i32>(jo::as_unit(u)->look_a)));  // 0x00505960
                    if (!(v >= global<float>(d::flt_m10))) return;  // test ah,1 jnz: below or unordered
                    if (v > global<float>(d::flt_10)) return;      // test ah,0x41 jz: above
                } else {
                    jo::view<u16>(jo::as_unit(u)->look_a) = 0;
                }
            } else if (jo::self<jo::Ai>(this)->current.target == 2) {
                jo::view<u16>(jo::as_unit(u)->look_a) = 0;
                if (joan::orig::as_units(kArmies)->list.empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.radius))) == 0) {  // 0x00425d50
                    jo::view<u16>(jo::as_unit(u)->look_b) = static_cast<u16>(jo::self<jo::Ai>(this)->current.radius);
                    const float v = joan::orig::arg::as<float>(joan::orig::as_unit(u)->face_object(static_cast<i32>(jo::as_unit(u)->look_b)));  // 0x005059b0
                    if (!(v >= global<float>(d::flt_m10))) return;
                    if (v > global<float>(d::flt_10)) return;
                } else {
                    jo::view<u16>(jo::as_unit(u)->look_b) = 0;
                }
            }
            jo::view<u8>(jo::as_unit(u)->combo_state) = 0;
            jo::as_unit(u)->armour_flags[0] = 0;
            set_order(u, 0x30);
            joan::orig::as_squad(u)->clear();  // 0x00422f10
            jo::as_unit(u)->wait_count = jo::as_unit(u)->wait_limit;
            jo::self<jo::Ai>(this)->current.spread = 1;
            return;
        }
        if (kind(u) == 0x2a || joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0) {  // 0x0042d0e0
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (kind(u) == 0x30 || kind(u) == 0x32 || kind(u) == 0x33 || kind(u) == 0x36 || kind(u) == 0x38 ||
            kind(u) == 0x39)
            return;
        if (jo::self<jo::Ai>(this)->current.expire_time < clock_ms()) jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
    }

    // ---------------------------------------------------------------- 0x004c02b0
    // LightSys::start_transition (the object is the LightSys, typed "Ballista" before): state_b is cleared,
    // then set to the per-second steps from the current preset (+0xc) to special preset idx (+0x8).
    void* reset_4c02b0(i32 idx, u32 t_bits, u32 c_bits) {
        c_4c02b0.hit();
        jo::LightSys* const ls = jo::self<jo::LightSys>(this);
        jo::LightState& to = ls->state_b;
        game_memset(&to, 0, sizeof(jo::LightState));
        joan::orig::as_light_params(&to.preset.params)->construct();  // 0x00617ee0
        jo::view<u32>(to.fade) = t_bits;
        jo::view<u32>(to.scale) = c_bits;
        ls->state_a.scale = global<float>(d::flt_1) - to.scale;
        const jo::LightPreset* const src = &ls->specials[idx - 1];
        const jo::LightPreset& cur = ls->current;
        const float t = as_float(t_bits);
        for (i32 i = 0; i < to.preset.light_count; ++i) {
            to.preset.lights[i].colour[0] = (src->lights[i].colour[0] - cur.lights[i].colour[0]) / t;
            to.preset.lights[i].colour[1] = (src->lights[i].colour[1] - cur.lights[i].colour[1]) / t;
            to.preset.lights[i].colour[2] = (src->lights[i].colour[2] - cur.lights[i].colour[2]) / t;
            to.preset.lights[i].colour[3] = (src->lights[i].colour[3] - cur.lights[i].colour[3]) / t;
            to.preset.lights[i].enabled = src->lights[i].enabled;
        }
        for (i32 k = 0; k < 5; ++k) to.preset.params.scale_a[k] = (src->params.scale_a[k] - cur.params.scale_a[k]) / t;
        for (i32 k = 0; k < 3; ++k) to.preset.params.scale_b[k] = (src->params.scale_b[k] - cur.params.scale_b[k]) / t;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_04", name, addr, fn)
B("ArmyOrder::on_4bc8a0", 0x004bc8a0, &H::on_4bc8a0);
B("ArmyOrder::on_4bca60", 0x004bca60, &H::on_4bca60);
B("ArmyOrder::on_4bcdb0", 0x004bcdb0, &H::on_4bcdb0);
B("ArmyOrder::on_4bd090", 0x004bd090, &H::on_4bd090);
B("Ballista::reset_4c02b0", 0x004c02b0, &H::reset_4c02b0);
#undef B

}  // namespace game_b104_04
}  // namespace

// ---------------------------------------- from modules/game_b109_03.cpp (game/b109_03)
namespace {
namespace game_b109_03 {

using joan::orig::helpers::wrap_add;
using joan::orig::helpers::wrap_mul;

struct V3 {
    float x, y, z;
};

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(id))); }      // 0x0041b3c0
using joan::orig::helpers::random;  // 0x00590540

JOAN_COUNTER(c_follow, "Ai::follow_4bd420");

// ============================================================================ 0x004bd420
// Waypoint radius used twice by 0x004bd420.
float follow_radius(u8* unit) {
    float r = static_cast<float>(global<i32>(d::g_waypoint_reach_dist));
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0 ||  // 0x00430190
        static_cast<i32>(jo::as_unit_type(joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id)))))->unit_class) == 8 ||  // 0x0042d0a0
        jo::as_unit(unit)->move_mode > 1)
        r = static_cast<float>(static_cast<i32>(global<u32>(d::g_waypoint_reach_dist) << 1));
    return r;
}

struct H {
    u8* me() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x004bd420
    void follow_4bd420(u8* unit, i32 target) {
        c_follow.hit();
        u8* const self = me();
        const i32 kind = jo::as_unit(unit)->anim_kind;
        if (kind == 0x40 || kind == 0x41 || kind == 0x42 || kind == 0x3c || kind == 0x29 || kind == 0x4e) {
            jo::as_order(self)->current.kind = 0;
            return;
        }
        if (unit_missing(target) != 0 || joan::orig::arg::as<i8>(joan::orig::as_unit(unit_get(target))->dead()) != 0 ||  // 0x0042d0e0
            !(jo::as_order(self)->current.expire_time >= global<u32>(d::g_now))) {
            joan::orig::as_order(self)->finish(joan::orig::arg::mptr(unit));  // 0x004378a0
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_kind_510f80()) != 0) return;  // 0x00510f80
        u8* const tu = unit_get(target);
        V3 a{};
        V3 b{};
        joan::orig::as_unit(unit)->position(&a);  // 0x004fedb0
        joan::orig::as_unit(tu)->position(&b);
        V3 t = b;
        const i32 rnd = random(jo::as_order(self)->current.spread);
        float dist = 0.0f;
        {
            V3 tmp{};
            V3 dv = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &a, &b));  // 0x0060cc90
            dist = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, &dv));  // length_sq
        }
        {
            const i32 r1 = wrap_add(jo::as_order(self)->current.radius, jo::as_order(self)->current.spread);
            const i32 k1 = wrap_mul(r1, r1);
            if (static_cast<double>(k1) >= static_cast<double>(dist)) {
                const i32 k2 = wrap_mul(jo::as_order(self)->current.radius, jo::as_order(self)->current.radius);
                if (static_cast<double>(k2) < static_cast<double>(dist)) {
                    joan::orig::as_order(self)->finish(joan::orig::arg::mptr(unit));
                    return;
                }
            }
        }
        const Fpu fpu{true};
        const double q = fpu.div(fpu.mul(static_cast<double>(joan::orig::arg::as<float>(joan::orig::as_ground_object(tu)->heading())),  // 0x004ff850
                                         static_cast<double>(global<float>(d::flt_180))),
                                 global<double>(d::dbl_3_141592653589793));
        const float rel = joan::orig::arg::as<float>(joan::orig::as_unit(tu)->heading_to_unit(static_cast<i32>(jo::as_unit(unit)->id)));  // 0x00505960
        float ang = static_cast<float>(fpu.sub(q, static_cast<double>(rel)));
        const i32 jitter = random(0x10e) - 0x87;
        ang = static_cast<float>(fpu.add(static_cast<double>(jitter), static_cast<double>(ang)));
        u32 angle = 0;
        joan::orig::as_angle(&angle)->construct();  // 0x00576120
        joan::orig::as_angle(&angle)->set_byte(joan::orig::arg::f32(ang));  // 0x005762c0
        {
            const float rx = static_cast<float>(wrap_add(jo::as_order(self)->current.radius, rnd));
            const float cx = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());  // 0x00576ac0
            const float m = cx * rx;
            t.x = m + t.x;
        }
        {
            const float ry = static_cast<float>(wrap_add(jo::as_order(self)->current.radius, rnd));
            const float cy = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());  // 0x005768d0
            const float m = cy * ry;
            t.y = m + t.y;
        }
        if (jo::as_order(self)->current.started == 0) {
            std::memcpy(jo::as_unit(unit)->follow_target, &t, sizeof t);
            jo::as_order(self)->current.started = 1;
        } else {
            std::memcpy(&t, jo::as_unit(unit)->follow_target, sizeof t);
        }
        {
            const float ex = a.x - t.x;
            const float ey = a.y - t.y;
            const float sx = ex * ex;
            const float sy = ey * ey;
            dist = sx + sy;
        }
        {
            const float r = follow_radius(unit);
            if (r * r >= dist) {  // test ah,1 jne (continue when < or NaN)
                joan::orig::as_order(self)->finish(joan::orig::arg::mptr(unit));
                return;
            }
        }
        i32 hold = 500;
        if (jo::as_unit(unit)->rank >= 3) {
            V3 tmp{};
            V3 dv = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::as_unit(unit)->position_), &t));
            const double ls = import_call<double>(d::iat_v3d_87_length_sq, &dv);
            if (ls >= static_cast<double>(global<float>(d::flt_2250000))) hold = 1000;
        }
        {
            V3 tmp{};
            V3 dv = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &b, &t));
            dist = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, &dv));
        }
        bool keep = false;
        {
            const i32 r1 = wrap_add(jo::as_order(self)->current.radius, jo::as_order(self)->current.spread);
            const i32 k1 = wrap_mul(r1, r1);
            if (static_cast<double>(k1) >= static_cast<double>(dist)) {
                const i32 k2 = wrap_mul(jo::as_order(self)->current.radius, jo::as_order(self)->current.radius);
                if (static_cast<double>(k2) < static_cast<double>(dist)) keep = true;
            }
        }
        if (!keep) {
            joan::orig::as_squad(unit)->clear();  // 0x00422f10
            jo::view<i32>(jo::as_unit(unit)->wait_count) = jo::view<i32>(jo::as_unit(unit)->wait_limit);
            jo::as_order(self)->current.started = 0;
        }
        const i8 res = joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->move_to(t, joan::orig::arg::u32(hold), joan::orig::arg::u32(jo::as_order(self)->current.arg_15 != 0 ? i32{1} : i32{0}), joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(jo::as_order(self)->current.arg_19)))));  // 0x00505dd0
        if (static_cast<i32>(res) != 3) {
            jo::as_order(self)->current.arg_15 = 0;
        } else if (unit_missing(jo::as_order(self)->current.arg_19) != 0) {
            jo::as_order(self)->current.arg_15 = 0;
        } else {
            u8* const u = unit_get(jo::as_order(self)->current.arg_19);
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0) jo::as_order(self)->current.arg_15 = 0;
            u8* const o = jo::view<u8*>(jo::as_unit(u)->orders);
            if (o != nullptr && jo::as_order(o)->current.kind != 0xf) jo::as_order(self)->current.arg_15 = 0;
        }
        void* const path = &jo::as_unit(unit)->path;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(path)->count_66e8a0()) == 0) return;  // 0x0066e8a0
        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_list(path)->count_66e8a0());
        const float* const wp = joan::orig::arg::as<const float*>(joan::orig::as_list(path)->from_end(joan::orig::arg::i32(n)));  // 0x0066e8d0
        {
            const float ex = wp[0] - a.x;
            const float ey = wp[1] - a.y;
            const float sx = ex * ex;
            const float sy = ey * ey;
            dist = sx + sy;
        }
        const float r = follow_radius(unit);
        if (r * r >= dist) joan::orig::as_order(self)->finish(joan::orig::arg::mptr(unit));
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_03", name, addr, fn)
B("Ai::follow_4bd420", 0x004bd420, &H::follow_4bd420);
#undef B

}  // namespace game_b109_03
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {


static_assert(sizeof(Vec) == 12, "vec3 by value");

void* const kUnits = obj(d::g_units);
void* const kRandom = obj(d::g_misc);

i32 rnd(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540

JOAN_COUNTER(c_follow, "Order::step_follow_4bdbc0");
JOAN_COUNTER(c_move, "Order::step_move_4bfa10");

// Shared tail of 0x004bdbc0 / 0x004bfa10 (from the 0x00505dd0 call on).
void order_tail(void* self, void* unit, Vec target, i32 limit) {
    const i8 res = joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->move_to(target, joan::orig::arg::u32(limit), joan::orig::arg::u32(static_cast<i32>(jo::view<u32>(jo::as_order(self)->current.arg_15) != 0 ? 1 : 0)), joan::orig::arg::u32(static_cast<i16>(jo::as_order(self)->current.arg_19))));  // 0x00505dd0
    if (res != 3) {
        jo::view<u32>(jo::as_order(self)->current.arg_15) = 0;
        return;
    }
    if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::as_order(self)->current.arg_19))) != 0) {  // 0x00428b70
        jo::view<u32>(jo::as_order(self)->current.arg_15) = 0;
        return;
    }
    void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_order(self)->current.arg_19)));  // 0x0041b3c0
    if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0) jo::view<u32>(jo::as_order(self)->current.arg_15) = 0;              // 0x0042d0e0
    if (jo::as_unit(u)->orders != nullptr && jo::as_order(jo::as_unit(u)->orders)->current.kind != 0xf) jo::view<u32>(jo::as_order(self)->current.arg_15) = 0;
}

bool excluded_kind(void* unit) {
    const i32 k = jo::as_unit(unit)->anim_kind;
    return k == 0x40 || k == 0x41 || k == 0x42 || k == 0x3c || k == 0x29 || k == 0x4e;
}

struct H {
    // ------------------------------------------------------------------------ 0x004bdbc0
    void step_follow(void* unit) {
        c_follow.hit();
        if (excluded_kind(unit)) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            return;
        }
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.target))) != 0 ||  // 0x00425d50
            jo::self<jo::Order>(this)->current.expire_time < global<u32>(d::g_now)) {
            joan::orig::as_cmd(this)->finish_4c1390(joan::orig::arg::mptr(unit));  // 0x004c1390
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_kind_510f80()) != 0) return;  // 0x00510f80
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->is_kind_special()) == 0) return;  // 0x00508ab0
        void* const leader = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.target)));  // 0x00425d20
        Vec a{}, b{};
        joan::orig::as_unit(unit)->position(&a);    // 0x004fedb0
        joan::orig::as_map_object(leader)->position(&b);  // 0x00499930
        const i32 r = rnd(static_cast<i32>(jo::view<u32>(jo::self<jo::Order>(this)->current.spread)));
        Vec t{};
        const Vec d = *joan::orig::arg::as<Vec*>(joan::orig::vec_sub(&t, &a, &b));  // 0x0060cc90
        const float l2 = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, &d));     // length_sq
        const u32 s = static_cast<u32>(jo::self<jo::Order>(this)->current.radius + r);
        const i32 prod = static_cast<i32>(s * s);
        if (static_cast<double>(prod) >= static_cast<double>(l2)) {  // fcompp, C0 clear
            joan::orig::as_cmd(this)->finish_4c1390(joan::orig::arg::mptr(unit));  // 0x004c1390
            return;
        }
        const Fpu fpu{true};
        const float ang = static_cast<float>(rnd(0x168));
        u32 angle = 0;  // 1-byte angle object (0x00576120 ctor is empty)
        joan::orig::as_angle(&angle)->construct();       // 0x00576120
        joan::orig::as_angle(&angle)->set_byte(joan::orig::arg::f32(ang));   // 0x005762c0
        const float rx = static_cast<float>(jo::self<jo::Order>(this)->current.radius + r);
        const double c = joan::orig::arg::as<double>(joan::orig::as_angle(&angle)->cos());  // 0x00576ac0
        b.x = static_cast<float>(fpu.add(fpu.mul(c, rx), b.x));
        const float ry = static_cast<float>(jo::self<jo::Order>(this)->current.radius + r);
        const double sn = joan::orig::arg::as<double>(joan::orig::as_angle(&angle)->sin());  // 0x005768d0
        b.y = static_cast<float>(fpu.add(fpu.mul(sn, ry), b.y));
        i32 limit = 500;
        if (jo::as_unit(unit)->rank >= 3) {
            Vec t2{};
            const Vec d2 = *joan::orig::arg::as<Vec*>(joan::orig::vec_sub(&t2, joan::orig::arg::ptr(jo::as_unit(unit)->position_), &b));  // 0x0060cc90
            const float l = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, &d2));                    // length_sq
            if (l >= global<float>(d::flt_2250000)) limit = 1000;
        }
        order_tail(this, unit, b, limit);
    }

    // ------------------------------------------------------------------------ 0x004bfa10
    void step_move(void* unit, float x, float y, float z) {
        c_move.hit();
        if (excluded_kind(unit)) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            jo::as_unit(unit)->name_index = 0;
            return;
        }
        if (jo::self<jo::Order>(this)->current.expire_time < global<u32>(d::g_now)) {
            joan::orig::as_cmd(this)->finish_4c1670(joan::orig::arg::mptr(unit));  // 0x004c1670
            return;
        }
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->is_kind_special()) == 0) return;  // 0x00508ab0
        Vec v{};
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0) {  // 0x00430190
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(unit)->mount_id)));  // 0x0041b3c0
            joan::orig::as_unit(u)->position(&v);                                          // 0x004fedb0
        } else {
            joan::orig::as_unit(unit)->position(&v);  // 0x004fedb0
        }
        const float dx = v.x - x;
        const float dy = v.y - y;
        const float d = dx * dx + dy * dy;
        bool wide = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0;  // 0x00430190
        if (!wide) {
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
            wide = jo::as_unit_type(p)->unit_class == 8 || jo::as_unit(unit)->move_mode > 1;
        }
        const u32 g = global<u32>(d::g_waypoint_reach_dist);
        const i32 lim = wide ? static_cast<i32>(((g << 1) * g) << 1) : static_cast<i32>(g * g);
        if (static_cast<double>(lim) >= static_cast<double>(d)) {  // fcomp, C0 clear
            joan::orig::as_cmd(this)->finish_4c1670(joan::orig::arg::mptr(unit));  // 0x004c1670
            return;
        }
        i32 limit = 500;
        if (jo::view<u32>(jo::self<jo::Order>(this)->current.started) != 0) jo::as_unit(unit)->name_index = jo::view<u32>(jo::self<jo::Order>(this)->current.started);
        if (jo::as_unit(unit)->rank >= 3 && d >= global<float>(d::flt_2250000)) limit = 1000;
        order_tail(this, unit, Vec{x, y, z}, limit);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Order::step_follow_4bdbc0", 0x004bdbc0, &H::step_follow);
B("Order::step_move_4bfa10", 0x004bfa10, &H::step_move);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b109_13.cpp (game/b109_13)
namespace {
namespace game_b109_13 {

using joan::orig::helpers::fbits;

struct V3 {  // 12 bytes copied as raw dwords
    u32 x, y, z;
};
static_assert(sizeof(V3) == 12, "sizes");

V3 load_v3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

constexpr img::Addr kUnits(d::g_units);

JOAN_COUNTER(c_think, "Squad::think_4bdf90");

struct H {
    // -------------------------------------------------------------- 0x004bdf90
    void think(u8* unit) {
        c_think.hit();
        const i32 kind = jo::as_unit(unit)->anim_kind;
        if (kind == 0x40 || kind == 0x41 || kind == 0x42 || kind == 0x3c || kind == 0x29 || kind == 0x4e) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_place_array))->is_missing_ca0(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.target)))) != 0 ||  // 0x00456ca0
            jo::self<jo::Order>(this)->current.expire_time < global<u32>(d::g_now)) {
            joan::orig::as_order(this)->finish_unit(joan::orig::arg::mptr(unit));  // 0x0043da10
            return;
        }
        if (jo::as_unit(unit)->name_index != 0 && joan::orig::arg::as<i32>(joan::orig::as_list(&jo::as_unit(unit)->path)->count_66e8a0()) != 0) return;  // 0x0066e8a0
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_kind_510f80()) != 0) return;                                       // 0x00510f80
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->is_kind_special()) == 0) return;                                       // 0x00508ab0
        u8* const grp = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.target))));         // 0x00456c70
        u32 pos[3] = {0, 0, 0};
        joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(pos));  // 0x004fedb0
        u32 tgt[3] = {0, 0, 0};
        joan::orig::as_place(grp)->position(joan::orig::arg::mptr(tgt));  // 0x004c21c0
        const Fpu fpu{true};
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.radius)))) != 0) {  // 0x0044cf80
            i32 idx = 0;
            if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_place(grp)->box)->contains(joan::orig::arg::ptr(pos))) != 0) {  // 0x0060d5f0
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->leads()) != 0) {            // 0x0044a660
                    idx = joan::orig::arg::as<i32>(joan::orig::as_squad(grp)->slot_of(static_cast<i32>(jo::as_unit(unit)->id)));  // 0x0045e620
                    if (idx != 0) {
                        if (jo::as_place(grp)->slot_unit[idx] != static_cast<i32>(jo::as_unit(unit)->id)) {
                            joan::orig::as_place(grp)->set_slot(joan::orig::arg::i32(idx), joan::orig::arg::u32(static_cast<i32>(jo::as_unit(unit)->id)));  // 0x004c2540
                            joan::orig::as_group(jo::as_unit(unit)->group)->arrange(i32{1}, i32{1});           // 0x004fa0f0
                        } else {
                            joan::orig::as_place(grp)->set_slot(joan::orig::arg::i32(idx), joan::orig::arg::u32(static_cast<i32>(jo::as_unit(unit)->id)));
                        }
                        tgt[0] = joan::orig::arg::as<const u32*>(joan::orig::as_place(grp)->slot_pos(joan::orig::arg::i32(idx)))[0];  // 0x004c2470 (x, y)
                        tgt[1] = joan::orig::arg::as<const u32*>(joan::orig::as_place(grp)->slot_pos(joan::orig::arg::i32(idx)))[1];
                    }
                } else {
                    u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->leader());  // 0x004216d0
                    if (t != nullptr && jo::as_leader(t)->slot != 0 && jo::as_leader(t)->i16_4 == jo::as_place(grp)->index) {
                        idx = jo::as_leader(t)->slot;
                        u32 out[3] = {0, 0, 0};
                        const u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->target_point(joan::orig::arg::mptr(out), joan::orig::arg::mptr(i32{0})));  // 0x00448870
                        tgt[0] = reinterpret_cast<const u32*>(r)[0];  // the point (x, y, z)
                        tgt[1] = reinterpret_cast<const u32*>(r)[1];
                        tgt[2] = reinterpret_cast<const u32*>(r)[2];
                    }
                }
            }
            if (idx != 0) {
                float dist = 0.0f;  // assigned on both paths below
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0) {  // 0x00430190
                    const float tx = fbits(tgt[0]), ty = fbits(tgt[1]);
                    const float a = reinterpret_cast<const float*>(jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(unit)->mount_id)))))->position_)[0] - tx;
                    const float b = (reinterpret_cast<const float*>(jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(unit)->mount_id)))))->position_)[0] - tx) * a;
                    const float c = reinterpret_cast<const float*>(jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(unit)->mount_id)))))->position_)[1] - ty;
                    dist = (reinterpret_cast<const float*>(jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(unit)->mount_id)))))->position_)[1] - ty) * c + b;
                } else {
                    const float tx = fbits(tgt[0]), ty = fbits(tgt[1]);
                    const float dx = (reinterpret_cast<const float*>(jo::as_unit(unit)->position_)[0] - tx) * (reinterpret_cast<const float*>(jo::as_unit(unit)->position_)[0] - tx);
                    const float dy = (reinterpret_cast<const float*>(jo::as_unit(unit)->position_)[1] - ty) * (reinterpret_cast<const float*>(jo::as_unit(unit)->position_)[1] - ty);
                    dist = dx + dy;
                }
                float range = static_cast<float>(global<i32>(d::g_waypoint_reach_dist));
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0 ||
                    jo::as_unit_type(joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id)))))->unit_class == 8 ||  // 0x0042d0a0
                    jo::as_unit(unit)->move_mode > 1) {
                    range = static_cast<float>(static_cast<i32>(global<u32>(d::g_waypoint_reach_dist) << 1));
                }
                if (range * range >= dist) {  // fcomp, test ah,1 jne: less or unordered skips
                    joan::orig::as_order(this)->finish_unit(joan::orig::arg::mptr(unit));  // 0x0043da10
                    const float v = joan::orig::arg::as<float>(joan::orig::as_place(grp)->slot_angle(joan::orig::arg::i32(idx)));  // 0x004c24b0
                    const double q = fpu.mul(fpu.div(v, global<double>(d::dbl_3_141592653589793)), global<double>(d::dbl_9cfbe8));
                    joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(static_cast<float>(q)));  // 0x0050abd0
                    return;
                }
            } else {
                if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_place(grp)->box)->contains(joan::orig::arg::ptr(pos))) != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->leads()) != 0) {
                    joan::orig::as_order(this)->finish_unit(joan::orig::arg::mptr(unit));
                }
                const i32 rx = joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(joan::hook::x87::ftol(jo::as_place(grp)->box[3] - jo::as_place(grp)->box[0])));  // 0x00590540
                const float fx = static_cast<float>(fpu.add(static_cast<double>(rx), jo::as_place(grp)->box[0]));
                std::memcpy(&tgt[0], &fx, 4);
                const i32 ry = joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(joan::hook::x87::ftol(jo::as_place(grp)->box[4] - jo::as_place(grp)->box[1])));
                const float fy = static_cast<float>(fpu.add(static_cast<double>(ry), jo::as_place(grp)->box[1]));
                std::memcpy(&tgt[1], &fy, 4);
            }
        } else {
            u8* const area = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.radius))));  // 0x0043c450
            if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_facility(area)->bounds)->contains(joan::orig::arg::ptr(pos))) != 0) {
                joan::orig::as_order(this)->finish_unit(joan::orig::arg::mptr(unit));
                return;
            }
            joan::orig::as_facility(area)->get_position(joan::orig::arg::mptr(tgt));  // 0x0046d670
            const i32 rx = joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(joan::hook::x87::ftol(jo::as_facility(area)->bounds[3] - jo::as_facility(area)->bounds[0])));
            const float fx = static_cast<float>(fpu.add(static_cast<double>(rx), jo::as_facility(area)->bounds[0]));
            std::memcpy(&tgt[0], &fx, 4);
            const i32 ry = joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(joan::hook::x87::ftol(jo::as_facility(area)->bounds[4] - jo::as_facility(area)->bounds[1])));
            const float fy = static_cast<float>(fpu.add(static_cast<double>(ry), jo::as_facility(area)->bounds[1]));
            std::memcpy(&tgt[1], &fy, 4);
        }
        // 0x004be46a
        i32 duration = 500;
        u32 tmp[3] = {0, 0, 0};
        const u8* const diff = joan::orig::arg::as<u8*>(joan::orig::vec_sub(joan::orig::arg::mptr(tmp), joan::orig::arg::ptr(jo::as_unit(unit)->position_), joan::orig::arg::ptr(tgt)));  // 0x0060cc90
        const V3 d = load_v3(diff);
        const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(&d)));  // length_sq
        if (!(len > global<float>(d::flt_250000))) {
            duration = 500;
        } else if (!(len > global<float>(d::flt_2250000))) {
            duration = 800;
        } else if (!(len > global<float>(d::flt_6250000))) {
            duration = 1000;
        } else if (!(len > global<float>(d::flt_12250000))) {
            duration = 1500;
        } else {
            duration = 2000;
        }
        V3 target;
        std::memcpy(&target, tgt, sizeof target);
        const i8 res = joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->move_to(rebits<Vec>(target), joan::orig::arg::u32(duration), joan::orig::arg::u32(static_cast<i32>(jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) != 0)), joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(jo::self<jo::Order>(this)->current.arg_19)))));  // 0x00505dd0
        if (res != 3) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) = 0;
            return;
        }
        if (joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.arg_19))) != 0) {  // 0x00428b70
            jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) = 0;
            return;
        }
        u8* const target_unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.arg_19))));
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(target_unit)->dead())) != 0) jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) = 0;  // 0x0042d0e0
        if (jo::as_unit(target_unit)->orders != nullptr && jo::as_order(jo::as_unit(target_unit)->orders)->current.kind != 0xf) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) = 0;
        }
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_13", name, addr, fn)
B("Squad::think_4bdf90", 0x004bdf90, &H::think);
#undef B

}  // namespace game_b109_13
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using joan::orig::helpers::as_float;
// (x - px)^2 + (y - py)^2, each x87 op on float operands at PC24 (fsub, fsub, fmulp, ..., faddp).
float dist2(float x, float y, float px, float py) {
    const float a = x - px, b = x - px;
    const float m1 = a * b;
    const float c = y - py, d = y - py;
    const float m2 = c * d;
    return m1 + m2;
}

// [Fixes] UnitStuck (not in the original game; joan::hook::settings().fix_unit_stuck).
//
// Escort::update_4be770 keeps a soldier at its formation spot next to its leader: within the arrival radius the order
// ends, otherwise it asks Unit::move_to (0x00505dd0) for a route to the spot, every update. The spot comes from the
// leader's formation (0x00448870) without any look at the ground, so it can lie off the walkable area - on a bridge's
// parapet, in the water beside it. The path search (Nav::find_path_66cb40) then returns its "as close as possible"
// route, which ends at the edge right in front of the soldier: the soldier reaches it at once, the route is done, and
// the next update asks again - the soldier plays its run animation against the wall until the order expires.
// (Recording ref-x86-2026-11-1a, mission 5 at the castle bridge: units 27 and 99 for 1,700 frames.)
//
// With the fix the spot is first moved onto the walkable area of the soldier's own path space near the leader
// (snap_spot), and a route too short to bring the soldier within the arrival radius of the spot counts as arrival
// (no_progress).
namespace unit_stuck {

/// Whether the local point (lx, ly) (path-space coordinates: world minus the cell origin) lies inside a polygon of the
/// path space `grid`, by the containment test the path search uses for its goal (Nav::find_region_66bb40 0x0066bb40,
/// clearance 0).
bool walkable(void* grid, float lx, float ly) {
    return joan::orig::as_path_space(grid)->find_region(lx, ly, 0.0f) != 0;  // 0x0066bb40
}

/// Moves the formation spot (x, y) (world units) onto the walkable area of `unit`'s path space near `leader`: first
/// along the line toward the leader (16 steps, never closer than kLeaderGap to the leader, who stands there), then, if
/// no point of the line qualifies, to the walkable point nearest the spot on rings around the leader (kLeaderGap,
/// 1.5x, 2x; 8 directions). Unchanged when the spot is walkable already, when the unit has no loaded cell or no other
/// leader, or when nothing qualifies (the arrival test below then stops the unit where it gets).
void snap_spot(u8* unit, const u8* leader, float& x, float& y) {
    constexpr float kLeaderGap = 64.0f;  // two body radii (Gameplay.res BODY_R = 32, 0x00a3cc9c)
    if (leader == nullptr || leader == unit) return;
    void* const cell = jo::as_unit(unit)->cell;
    if (cell == nullptr || jo::view<i8>(jo::as_cell(cell)->loaded) == 0) return;
    void* const grid = joan::orig::arg::as<void*>(joan::orig::as_world_cell(cell)->grid(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->layer))));  // 0x005600c0
    if (grid == nullptr || jo::as_path_space(grid)->zones == nullptr) return;
    const float ox = static_cast<float>(jo::as_cell(cell)->origin_x), oy = static_cast<float>(jo::as_cell(cell)->origin_y);
    if (walkable(grid, x - ox, y - oy)) return;
    const float lx = reinterpret_cast<const float*>(jo::as_unit(leader)->position_)[0];
    const float ly = reinterpret_cast<const float*>(jo::as_unit(leader)->position_)[1];
    for (int i = 1; i < 16; ++i) {
        const float t = static_cast<float>(i) / 16.0f;
        const float qx = x + (lx - x) * t, qy = y + (ly - y) * t;
        if ((qx - lx) * (qx - lx) + (qy - ly) * (qy - ly) < kLeaderGap * kLeaderGap) break;
        if (walkable(grid, qx - ox, qy - oy)) {
            x = qx;
            y = qy;
            return;
        }
    }
    constexpr float kDiag = 0.70710678f;
    static constexpr float kDir[8][2] = {{1, 0}, {kDiag, kDiag}, {0, 1}, {-kDiag, kDiag}, {-1, 0}, {-kDiag, -kDiag}, {0, -1}, {kDiag, -kDiag}};
    bool found = false;
    float best = 0.0f, bx = x, by = y;
    for (const float ring : {1.0f, 1.5f, 2.0f}) {
        for (const auto& d : kDir) {
            const float qx = lx + d[0] * kLeaderGap * ring, qy = ly + d[1] * kLeaderGap * ring;
            if (!walkable(grid, qx - ox, qy - oy)) continue;
            const float e = (qx - x) * (qx - x) + (qy - y) * (qy - y);
            if (!found || e < best) {
                found = true;
                best = e;
                bx = qx;
                by = qy;
            }
        }
    }
    if (found) {
        x = bx;
        y = by;
    }
}

/// Whether the route Unit::move_to (0x00505dd0) just gave `unit` is no longer than `r` (its length, summed into
/// Unit +0x40e by move_to) while the spot is farther than `r`: following it cannot bring the unit within the arrival
/// radius - the spot is out of reach and the unit is as close as the walkable area lets it get.
bool no_progress(const u8* unit, float r) {
    const float length = jo::view<float>(jo::as_unit(unit)->path_remaining);
    return !(length > r);
}

}  // namespace unit_stuck

JOAN_COUNTER(c_4be770, "Escort::update_4be770");

struct H {
    // 0x004be770
    void escort_update(u8* a) {
        c_4be770.hit();
        void* const units = obj(d::g_units);
        const i32 k = jo::as_unit(a)->anim_kind;
        if (k == 0x40 || k == 0x41 || k == 0x42 || k == 0x3c || k == 0x29 || k == 0x4e) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        const u32 now_ms = global<u32>(d::g_now);
        if (jo::self<jo::Ai>(this)->current.expire_time < now_ms) {
            joan::orig::as_order(this)->release_unit(joan::orig::arg::mptr(a));  // 0x004c1480
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(a)->is_kind_510f80()) != 0) return;  // 0x00510f80
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(a)->is_kind_special()) == 0) return;  // 0x00508ab0
        float f10, fc;
        u32 f8;
        if (jo::self<jo::Ai>(this)->current.spread == 1) {
            f10 = static_cast<float>(jo::self<jo::Ai>(this)->current.target);
            fc = static_cast<float>(jo::self<jo::Ai>(this)->current.radius);
            f8 = reinterpret_cast<const u32*>(jo::as_unit(a)->position_)[2];
        } else {
            u32 tmp[3] = {0, 0, 0};
            const u32* const r = joan::orig::arg::as<const u32*>(joan::orig::as_unit(a)->target_point(joan::orig::arg::mptr(tmp), joan::orig::arg::mptr(0)));  // 0x00448870
            const u32 r0 = r[0], r1 = r[1], r2 = r[2];
            f10 = as_float(r0);
            fc = as_float(r1);
            f8 = r2;
            jo::self<jo::Ai>(this)->current.target = joan::hook::x87::ftol(f10);
            jo::self<jo::Ai>(this)->current.radius = joan::hook::x87::ftol(fc);
            jo::self<jo::Ai>(this)->current.spread = 1;
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_unit(a)->leader());  // 0x004216d0
            if (p != nullptr && jo::view<u8*>(jo::as_unit(p)->group) != nullptr) {
                u8* const q = jo::view<u8*>(jo::as_unit(p)->group);
                jo::as_group(q)->u32_54 = jo::view<u32>(jo::as_group(q)->angle);
                u8* const src = jo::view<u8*>(jo::as_unit(p)->group);
                u8* const dst = jo::view<u8*>(jo::as_unit(p)->group);
                jo::view<u32>(jo::as_group(dst)->front_x) = jo::view<u32>(jo::as_group(src)->x);  // the group's position into front_x / front_y / u32_50
                jo::view<u32>(jo::as_group(dst)->front_y) = jo::view<u32>(jo::as_group(src)->y);
                jo::as_group(dst)->u32_50 = jo::view<u32>(jo::as_group(src)->z);
            }
        }
        const bool fix_stuck = joan::hook::settings().fix_unit_stuck;
        if (fix_stuck) unit_stuck::snap_spot(a, joan::orig::arg::as<u8*>(joan::orig::as_unit(a)->leader()), f10, fc);  // 0x004216d0
        float d18;
        const float* from = reinterpret_cast<const float*>(jo::as_unit(a)->position_);  // where the distance is measured from
        const bool mounted = joan::orig::arg::as<i32>(joan::orig::as_unit(a)->mounted()) != 0;  // 0x00430190 (a lookup: the original calls it twice)
        if (mounted) {
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(a)->mount_id))));  // 0x0041b3c0
            from = reinterpret_cast<const float*>(jo::as_unit(u)->position_);
            d18 = dist2(f10, fc, reinterpret_cast<const float*>(jo::as_unit(u)->position_)[0], reinterpret_cast<const float*>(jo::as_unit(u)->position_)[1]);
        } else {
            d18 = dist2(f10, fc, reinterpret_cast<const float*>(jo::as_unit(a)->position_)[0], reinterpret_cast<const float*>(jo::as_unit(a)->position_)[1]);
        }
        float r20 = static_cast<float>(global<i32>(d::g_waypoint_reach_dist));
        bool twice;
        if (mounted)
            twice = true;
        else if (jo::as_unit_type(joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(a)->type_id))))->unit_class == 8)  // 0x0042d0a0
            twice = true;
        else
            twice = jo::as_unit(a)->move_mode > 1;
        if (twice) r20 = static_cast<float>(static_cast<i32>(static_cast<u32>(global<i32>(d::g_waypoint_reach_dist)) << 1));
        const float rr = r20 * r20;
        // In range: the order ends, the unit faces the way its leader's group faces.
        auto arrive = [&]() {
            joan::orig::as_order(this)->release_unit(joan::orig::arg::mptr(a));  // 0x004c1480
            u8* const leader = joan::orig::arg::as<u8*>(joan::orig::as_unit(a)->leader());  // 0x004216d0 (a lookup: the original calls it twice)
            if (leader != nullptr && jo::as_unit(a)->layer != 0) {
                u8* const q = jo::view<u8*>(jo::as_unit(leader)->group);
                const joan::hook::x87::Fpu fpu{true};
                const double deg = fpu.mul(fpu.div(jo::as_group(q)->angle, global<double>(d::dbl_3_141592653589793)), global<double>(d::dbl_9cfbe8));
                joan::orig::as_unit(a)->set_heading(joan::orig::arg::f32(static_cast<float>(deg)));  // 0x0050abd0
            }
        };
        if (rr >= d18) {
            arrive();
            return;
        }
        // Out of range (or NaN).
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.arg_19))) != 0) {  // 0x00428b70
            jo::view<u32>(jo::self<jo::Ai>(this)->current.arg_15) = 0;
        } else {
            u8* const u24 = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.arg_19))));  // 0x0041b3c0
            if (joan::orig::arg::as<u8>(joan::orig::as_unit(u24)->dead()) != 0) jo::view<u32>(jo::self<jo::Ai>(this)->current.arg_15) = 0;       // 0x0042d0e0
            if (jo::as_unit(u24)->orders == nullptr ||
                joan::orig::arg::as<i32>(joan::orig::as_list(jo::as_unit(u24)->orders)->contains(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.kind)))) == 0)  // 0x004bb8d0
                jo::view<u32>(jo::self<jo::Ai>(this)->current.arg_15) = 0;
            const float d28 = dist2(reinterpret_cast<const float*>(jo::as_unit(a)->position_)[0], reinterpret_cast<const float*>(jo::as_unit(a)->position_)[1], reinterpret_cast<const float*>(jo::as_unit(u24)->position_)[0], reinterpret_cast<const float*>(jo::as_unit(u24)->position_)[1]);
            if (!(static_cast<double>(global<i32>(d::g_a3cc74)) > static_cast<double>(d28))) jo::view<u32>(jo::self<jo::Ai>(this)->current.arg_15) = 0;
        }
        const i32 t = jo::as_unit(a)->target_building;
        u8* const w = joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(t)) == 0
                          ? joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(t)))  // 0x0041b3c0 (the original reads it twice)
                          : nullptr;
        if (w != nullptr && joan::orig::arg::as<u8>(joan::orig::as_unit(w)->dead()) == 0) {
            const float d30 = dist2(reinterpret_cast<const float*>(jo::as_unit(a)->position_)[0], reinterpret_cast<const float*>(jo::as_unit(a)->position_)[1], reinterpret_cast<const float*>(jo::as_unit(w)->position_)[0], reinterpret_cast<const float*>(jo::as_unit(w)->position_)[1]);
            if (!(d30 >= d18)) {
                joan::orig::as_order(this)->release_unit(joan::orig::arg::mptr(a));  // 0x004c1480
                return;
            }
        }
        // [Fixes] UnitStuck (game/unit_stuck.hpp): the spot this unit could not get to from here - arrived.
        if (fix_stuck && joan::hook::settings().fix_unit_stuck_watch && joan::unit_stuck::gave_up(a, jo::as_unit(a)->id, from[0], from[1], f10, fc, now_ms)) {
            arrive();
            return;
        }
        i32 v34 = 500;
        if (d18 >= global<float>(d::flt_2250000)) v34 = 1000;
        Vec3 pos;  // x / y the floats themselves, z the dword's bits
        pos.x = f10;
        pos.y = fc;
        std::memcpy(&pos.z, &f8, 4);
        const i8 res = joan::orig::arg::as<i8>(joan::orig::as_unit(a)->move_to(rebits<Vec>(pos), joan::orig::arg::u32(v34), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Ai>(this)->current.arg_15 != 0)), joan::orig::arg::u32(static_cast<i16>(jo::self<jo::Order>(this)->current.arg_19))));  // 0x00505dd0
        if (res == 3) return;
        if (res != 0) jo::view<u32>(jo::self<jo::Ai>(this)->current.arg_15) = 0;
        // [Fixes] UnitStuck: as close as the walkable area lets it get - arrived (see unit_stuck above).
        if (fix_stuck && res == 1 && unit_stuck::no_progress(a, r20)) arrive();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Escort::update_4be770", 0x004be770, &H::escort_update);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

float fconst(img::Addr a) { return img::global<const float>(a); }
double dconst(img::Addr a) { return img::global<const double>(a); }
u32 fbits(float f) {
    u32 u;
    std::memcpy(&u, &f, 4);
    return u;
}

// 12-byte vector passed by value (copied as raw dwords like the original's mov copies).
struct V3 {
    u32 x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_escort, "Escort::update_4bed40");

// Squared planar distance (float ops, x87 order: (a-b)*(a-b) + (c-d)*(c-d)).
float dist2(const void* a, const void* b) {
    const float dx = reinterpret_cast<const float*>(jo::as_unit(a)->position_)[0] - reinterpret_cast<const float*>(jo::as_unit(b)->position_)[0];
    const float dy = reinterpret_cast<const float*>(jo::as_unit(a)->position_)[1] - reinterpret_cast<const float*>(jo::as_unit(b)->position_)[1];
    return dx * dx + dy * dy;
}
float dist2xy(float x, float y, const void* b) {
    const float dx = x - reinterpret_cast<const float*>(jo::as_unit(b)->position_)[0];
    const float dy = y - reinterpret_cast<const float*>(jo::as_unit(b)->position_)[1];
    return dx * dx + dy * dy;
}

struct H {
    // ------------------------------------------------------------------ 0x004bed40
    void escort_update(void* p) {
        c_escort.hit();
        {
            const i32 s = jo::as_unit(p)->anim_kind;
            if (s == 0x40 || s == 0x41 || s == 0x42 || s == 0x3c || s == 0x29 || s == 0x4e) {
                jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
                return;
            }
        }
        if (jo::self<jo::Ai>(this)->current.expire_time < global<u32>(d::g_now)) {
            joan::orig::as_cmd(this)->finish_4c14e0(joan::orig::arg::mptr(p));  // 0x004c14e0
            return;
        }
        if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.target))) != 0) {
            joan::orig::as_cmd(this)->finish_4c14e0(joan::orig::arg::mptr(p));
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(p)->is_kind_510f80()) != 0) return;                    // 0x00510f80
        if (static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_unit(p)->is_kind_special())) == 0) return;  // 0x00508ab0
        void* const lead = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.target)));
        const Fpu fpu{true};
        alignas(4) u8 ang[4] = {0, 0, 0, 0};
        joan::orig::as_angle(static_cast<void*>(ang))->construct_577190();  // 0x00577190
        const float heading = static_cast<float>(
            fpu.add(fpu.mul(static_cast<double>(jo::self<jo::Ai>(this)->current.radius), fconst(d::flt_0_017453292)), jo::as_unit(lead)->heading));
        joan::orig::as_angle(static_cast<void*>(ang))->set_wrapped(joan::orig::arg::f32(heading));  // 0x00577340
        const double c = joan::orig::arg::as<double>(joan::orig::as_turns(static_cast<void*>(ang))->cos_577b50());  // 0x00577b50 (cos st0)
        const float px = static_cast<float>(
            fpu.add(fpu.mul(c, static_cast<double>(jo::self<jo::Ai>(this)->current.spread)), reinterpret_cast<const float*>(jo::as_unit(lead)->position_)[0]));
        const double s = joan::orig::arg::as<double>(joan::orig::as_turns(static_cast<void*>(ang))->sin_577b10());  // 0x00577b10 (sin st0)
        const float py = static_cast<float>(
            fpu.add(fpu.mul(s, static_cast<double>(jo::self<jo::Ai>(this)->current.spread)), reinterpret_cast<const float*>(jo::as_unit(lead)->position_)[1]));
        const u32 pz = reinterpret_cast<const u32*>(jo::as_unit(lead)->position_)[2];
        float d;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(p)->mounted()) != 0) {  // 0x00430190
            void* const t = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(p)->mount_id)));
            d = dist2xy(px, py, t);
        } else {
            d = dist2xy(px, py, p);
        }
        float r = static_cast<float>(global<i32>(d::g_waypoint_reach_dist));
        bool wide = joan::orig::arg::as<i32>(joan::orig::as_unit(p)->mounted()) != 0;
        if (!wide) {
            const void* const def = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(p)->type_id)))));  // 0x0042d0a0
            wide = jo::as_unit_type(def)->unit_class == 8 || jo::as_unit(p)->move_mode > 1;
        }
        if (wide) r = static_cast<float>(static_cast<i32>(global<u32>(d::g_waypoint_reach_dist) << 1));
        if (r * r >= d) {
            joan::orig::as_cmd(this)->finish_4c14e0(joan::orig::arg::mptr(p));
            if (joan::orig::arg::as<void*>(joan::orig::as_unit(p)->leader()) != nullptr) {  // 0x004216d0
                const float a = static_cast<float>(
                    fpu.mul(fpu.div(jo::as_unit(lead)->heading, dconst(d::dbl_3_141592653589793)), dconst(d::dbl_9cfbe8)));
                joan::orig::as_unit(p)->set_heading(joan::orig::arg::f32(a));  // 0x0050abd0
            }
            return;
        }
        const float d2 = dist2(p, lead);
        const i32 r2 = static_cast<i32>(static_cast<u32>(jo::self<jo::Ai>(this)->current.spread) * static_cast<u32>(jo::self<jo::Ai>(this)->current.spread));
        if (static_cast<double>(r2) > static_cast<double>(d2)) {
            joan::orig::as_cmd(this)->finish_4c14e0(joan::orig::arg::mptr(p));
            return;
        }
        if (joan::orig::as_dynarray(units())->empty_at(static_cast<i32>(jo::as_unit(p)->target_building)) == 0) {
            void* const u = joan::orig::as_dynarray(units())->ptr_at(static_cast<i32>(jo::as_unit(p)->target_building));
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead())) == 0) {  // 0x0042d0e0
                void* const u2 = joan::orig::as_dynarray(units())->ptr_at(static_cast<i32>(jo::as_unit(p)->target_building));
                const float d3 = dist2(p, u2);
                if (!(d3 >= d)) {
                    joan::orig::as_cmd(this)->finish_4c14e0(joan::orig::arg::mptr(p));
                    return;
                }
            }
        }
        const u32 t = !(d2 >= fconst(d::flt_2250000)) ? 0x1f4u : 0x3e8u;
        V3 v;
        v.x = fbits(px);
        v.y = fbits(py);
        v.z = pz;
        const u32 flag = jo::view<u32>(jo::self<jo::Ai>(this)->current.arg_15) != 0 ? 1u : 0u;
        const i32 k = static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(p)->move_to(rebits<Vec>(v), joan::orig::arg::u32(t), joan::orig::arg::u32(flag), joan::orig::arg::u32(static_cast<u32>(static_cast<u16>(jo::self<jo::Ai>(this)->current.arg_19))))));  // 0x00505dd0
        if (k == 3) return;
        if (k != 0) jo::view<u32>(jo::self<jo::Ai>(this)->current.arg_15) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Escort::update_4bed40", 0x004bed40, &H::escort_update);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b107_13.cpp (game/b107_13)
namespace {
namespace game_b107_13 {


JOAN_COUNTER(c_check, "UnitOrder::check_4bf240");

struct H {
    // group notify block shared by two paths of 0x004bf240
    static void notify_group(u8* unit) {
        u8* g = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
        if (jo::view<u32>(jo::as_unit_type(g)->script_id) == 0) return;
        u8* g2 = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
        void* grp = joan::orig::arg::as<void*>(joan::orig::as_uscr_res(obj(d::g_uscr_res))->entry(joan::orig::arg::i32(static_cast<i16>(jo::as_unit_type(g2)->script_id))));  // 0x00484540
        joan::orig::as_ai(grp)->dispatch_546970(jo::as_unit(unit));                                          // 0x00546970
    }

    // 0x004bf240
    void check_4bf240(u8* unit) {
        c_check.hit();
        const i32 st = static_cast<i32>(jo::as_unit(unit)->anim_kind);
        if (st == 0x40 || st == 0x41 || st == 0x42 || st == 0x3c || st == 0x29 || st == 0x4e ||
            joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0) {  // 0x00430190
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (jo::view<u32>(jo::self<jo::Ai>(this)->current.target) != 0 && joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.target))) != 0) {  // 0x00428b70
            joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            notify_group(unit);
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_list(&jo::as_unit(unit)->path)->count_66e8a0()) != 0) {  // 0x0066e8a0
            joan::orig::as_squad(unit)->clear();                 // 0x00422f10
            jo::as_unit(unit)->wait_count = jo::as_unit(unit)->wait_limit;
        }
        const i32 st2 = static_cast<i32>(jo::as_unit(unit)->anim_kind);
        if (st2 != 0x2b && st2 != 0x2d) {
            joan::orig::as_unit(unit)->set_action(0x2b, 0, 1);  // 0x00501190
            return;
        }
        if (!(jo::self<jo::Ai>(this)->current.expire_time < global<u32>(d::g_now))) return;
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.target))) == 0) {  // 0x00428b70
            const i16* p = joan::orig::arg::as<const i16*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.target))));  // 0x0041b3c0
            const i16 other = *p;
            if (joan::orig::arg::as<i32>(joan::orig::script::in_arc(joan::orig::arg::i32(jo::as_unit(unit)->id), joan::orig::arg::i32(other))) != 0) return;  // 0x00651430
            joan::orig::as_unit(unit)->play_idle(0, 1);                                  // 0x00508b90
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            notify_group(unit);
        } else {
            joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_13", name, addr, fn)
B("UnitOrder::check_4bf240", 0x004bf240, &H::check_4bf240);
#undef B

}  // namespace game_b107_13
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {


JOAN_COUNTER(c_task_bf510, "Task::update_bf510");

struct H {
    // ------------------------------------------------------------------------ 0x004bf510
    void task_bf510(u8* unit) {
        c_task_bf510.hit();
        const i32 k = static_cast<i32>(jo::as_unit(unit)->anim_kind);
        if (k == 0x40 || k == 0x41 || k == 0x42 || k == 0x3c || k == 0x29 || k == 0x4e) {
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (static_cast<i32>(jo::as_unit(unit)->action) != 1) {
            joan::orig::as_unit(unit)->set_action(i32{0x1a}, i32{0}, i32{1});  // 0x00501190
            return;
        }
        if (jo::self<jo::Ai>(this)->current.expire_time < global<u32>(d::g_now)) jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
    }

    // ------------------------------------------------------------------------ 0x004c1090
    i32 task_c1090(void* unit) {
        if (jo::self<jo::Ai>(this)->current.expire_time < global<u32>(d::g_now)) return 1;
        const double v = joan::orig::arg::as<double>(joan::orig::as_unit(unit)->anim_phase());  // 0x005045f0
        const double z = global<float>(d::k_9cf5c4);
        return (v < z || v > z) ? 0 : 1;  // test ah,0x40: equal or unordered -> 1
    }

    // ------------------------------------------------------------------------ 0x004c18f0
    void task_stop_unit(u8* unit) {
        jo::view<u8>(jo::as_unit(unit)->combo_state) = 0;
        jo::as_unit(unit)->armour_flags[0] = 0;
        joan::orig::as_unit(unit)->play_idle(i32{0}, i32{1});  // 0x00508b90
        jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Task::update_bf510", 0x004bf510, &H::task_bf510);
B94("Task::done_c1090", 0x004c1090, &H::task_c1090);
B94("Task::stop_unit", 0x004c18f0, &H::task_stop_unit);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b111_16.cpp (game/b111_16)
namespace {
namespace game_b111_16 {

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}


static_assert(sizeof(Vec) == 12, "vec3 by value");

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);

JOAN_COUNTER(c_patrol, "Order::step_patrol_4bfd70");

struct H {
    // -------------------------------------------------------------- 0x004bfd70
    void step_patrol(u8* a1) {
        if (jo::view<u32>(jo::self<jo::Order>(this)->current.radius) > 1u && !fix_uninit()) {
            // +0x26 outside 0 / 1: the original passes an uninitialised Vec3 - run its body.
            thiscall<u32>(fn::Order_step_patrol_4bfd70_body, this, a1);
            return;
        }
        c_patrol.hit();
        if (jo::view<u32>(jo::self<jo::Order>(this)->current.target) == 0 || jo::as_unit(a1)->cell == nullptr || jo::as_world_cell(jo::as_unit(a1)->cell)->tracks == nullptr) {
            joan::orig::as_cmd(this)->finish_4c17a0(joan::orig::arg::mptr(a1));  // 0x004c17a0
            return;
        }
        if (jo::self<jo::Order>(this)->current.expire_time < global<u32>(d::g_now)) {
            joan::orig::as_cmd(this)->finish_4c17a0(joan::orig::arg::mptr(a1));
            return;
        }
        void* const t = joan::orig::arg::as<void*>(joan::orig::as_name_record(jo::view<void*>(jo::as_world_cell(jo::as_unit(a1)->cell)->tracks))->get_c());  // 0x00453cc0
        u8* const path = joan::orig::arg::as<u8*>(joan::orig::as_table52(t)->slot(joan::orig::arg::i16(static_cast<i16>(jo::self<jo::Order>(this)->current.target))));                   // 0x0049ab40
        if (jo::as_path_graph(path)->node_count < 2) {
            joan::orig::as_cmd(this)->finish_4c17a0(joan::orig::arg::mptr(a1));
            return;
        }
        Vec pos{};
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(a1)->mounted()) != 0) {  // 0x00430190
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(a1)->mount_id)));
            joan::orig::as_unit(u)->position(&pos);
        } else {
            joan::orig::as_unit(a1)->position(&pos);
        }
        Vec tgt{};  // [ebp-0x20]; +0x26 outside 0 / 1 (fix/uninit only): never written, 0
        if (jo::view<u32>(jo::self<jo::Order>(this)->current.radius) == 0) {
            std::memcpy(&tgt, jo::as_path_graph(path)->nodes[0].pos, 12);
        } else if (jo::view<u32>(jo::self<jo::Order>(this)->current.radius) == 1) {
            const u32 last = static_cast<u32>(jo::as_path_graph(path)->node_count) - 1u;
            std::memcpy(&tgt, jo::as_path_graph(path)->nodes[last].pos, 12);
        }
        const float ex = pos.x - tgt.x, ey = pos.y - tgt.y;
        const float d = ex * ex + ey * ey;
        if (jo::view<u32>(jo::self<jo::Order>(this)->current.spread) != 0) {
            bool big = joan::orig::arg::as<i32>(joan::orig::as_unit(a1)->mounted()) != 0;
            if (!big) {
                u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(a1)->type_id)));  // 0x0042d0a0
                big = jo::as_unit_type(type)->unit_class == 8 || jo::as_unit(a1)->move_mode > 1;
            }
            const u32 g = global<u32>(d::g_waypoint_reach_dist);
            const u32 k = big ? ((g * 2u) * g) * 2u : g * g;
            if (static_cast<double>(static_cast<i32>(k)) >= static_cast<double>(d)) {  // !(k < d or NaN)
                jo::view<u32>(jo::self<jo::Order>(this)->current.radius) = jo::view<u32>(jo::self<jo::Order>(this)->current.radius) == 0 ? 1u : 0u;
            }
        }
        i32 limit = 500;
        jo::as_unit(a1)->name_index = jo::view<u32>(jo::self<jo::Order>(this)->current.target);
        if (jo::as_unit(a1)->rank >= 3 && d >= global<float>(d::flt_2250000)) limit = 1000;
        const i8 res = joan::orig::arg::as<i8>(joan::orig::as_unit(a1)->move_to(tgt, joan::orig::arg::u32(limit), joan::orig::arg::u32(static_cast<i32>(jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) != 0 ? 1 : 0)), joan::orig::arg::u32(static_cast<i16>(jo::self<jo::Order>(this)->current.arg_19))));  // 0x00505dd0 (low word of arg_19)
        if (res != 3) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) = 0;
            return;
        }
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.arg_19))) != 0) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) = 0;
            return;
        }
        u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Order>(this)->current.arg_19))));
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0) jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) = 0;  // 0x0042d0e0
        if (jo::as_unit(u)->orders != nullptr && jo::as_order(jo::as_unit(u)->orders)->current.kind != 0xf) jo::view<u32>(jo::self<jo::Order>(this)->current.arg_15) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_16", name, addr, fn)
B("Order::step_patrol_4bfd70", 0x004bfd70, &H::step_patrol);
#undef B

}  // namespace game_b111_16
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {


static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_attach, "Obj::attach_4c0680");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x004c0680
    u8 attach(i16 id) {
        c_attach.hit();
        void* d = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(i32{id})));  // 0x00485f20
        void* c = joan::orig::arg::as<void*>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->get(joan::orig::arg::i16(jo::as_trigger(d)->target_id)));  // 0x004d0820
        if (global<void*>(d::g_sound3d_listener) == nullptr) return 0;
        if (c == nullptr) return 0;
        if (jo::as_trigger(d)->expire_time == 0) {
            const Vec3 v = *reinterpret_cast<const Vec3*>(jo::as_trigger(d)->center);
            if (joan::orig::arg::as<u8>(joan::orig::as_sounds(this)->any_slot_at(rebits<Vec>(v))) != 0) return 1;  // 0x004d00e0
        }
        const float r = jo::as_trigger(d)->range;
        const float c0 = global<float>(d::k_9cf5c4);
        if (r < c0 || r > c0) {  // fcomp; test ah,0x40 (equal / unordered skips)
            Vec3 tmp{0, 0, 0};
            Vec3* pd = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::as_listener(global<void*>(d::g_sound3d_listener))->position), joan::orig::arg::ptr(jo::as_trigger(d)->center)));  // 0x0060cc90
            Vec3 w = *pd;
            const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &w));
            if (len > jo::as_trigger(d)->range) return 1;  // test ah,0x41
        }
        i32 k = joan::orig::arg::as<i16>(joan::orig::as_party(this)->first_free());  // 0x0045f450
        if (k == 0) {
            const Vec3 v = *reinterpret_cast<const Vec3*>(jo::as_listener(global<void*>(d::g_sound3d_listener))->position);
            k = joan::orig::arg::as<i16>(joan::orig::as_sound_list(this)->release_farthest(v));  // 0x00493340
        }
        Vec3 pos = *reinterpret_cast<const Vec3*>(jo::as_trigger(d)->center);
        void* s = joan::orig::arg::as<void*>(joan::orig::as_wave_res(c)->buffer());  // 0x004d09c0
        if (s == nullptr) return 0;
        if (!(k > 0)) return 0;
        void* e = joan::orig::arg::as<void*>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(static_cast<i16>(k))));  // 0x004cf690
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->queue(joan::orig::arg::mptr(s));                                         // 0x0059f460
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_looping(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u8>(jo::as_wave_res(c)->flags & 3))));     // 0x0059fa80
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_min_gain(joan::orig::arg::f32(i32{0}));                                    // 0x0059f770
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_max_gain(joan::orig::arg::f32(u32{0x3f800000}));                           // 0x0059f7c0
        const Fpu fpu{true};
        double sc = fpu.mul(static_cast<double>(jo::self<jo::Sounds>(this)->volume), static_cast<double>(jo::as_wave_res(c)->volume));
        sc = fpu.mul(sc, static_cast<double>(global<float>(d::flt_0_01)));
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_gain(joan::orig::arg::bits(static_cast<float>(sc)));  // 0x0059f700
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_max_distance(joan::orig::arg::u32(jo::view<u32>(jo::as_wave_res(c)->max_dist)));        // 0x0059f920
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_reference_distance(joan::orig::arg::u32(jo::view<u32>(jo::as_wave_res(c)->min_dist)));        // 0x0059f970
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_relative(static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})));                  // 0x0059fae0
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_rolloff(joan::orig::arg::u32(jo::view<u32>(jo::as_wave_res(c)->rolloff)));        // 0x0059f9c0
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_position(&pos);                    // 0x0059fb40
        joan::orig::as_sound_slot_entry(e)->set_id(joan::orig::arg::u32(i32{jo::view<i16>(jo::as_wave_res(c)->index)}));               // 0x004d0550
        joan::orig::as_sound_slot_entry(e)->set_key(joan::orig::arg::u32(i32{id}));                            // 0x004d06b0
        const Vec3 v2 = *reinterpret_cast<const Vec3*>(jo::as_trigger(d)->center);
        joan::orig::as_sound_slot_entry(e)->set_position(joan::orig::arg::bits(v2.x), joan::orig::arg::bits(v2.y), joan::orig::arg::bits(v2.z));  // 0x004d0770 (dword arguments: the floats' bits)
        joan::orig::as_sound(jo::as_sound_slot_entry(e)->sound)->play();    // 0x0059f620
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Obj::attach_4c0680", 0x004c0680, &H::attach);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

struct H {
    // ---- tiny ----
    i32 true1(void*) { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::true_4c0e40", 0x004c0e40, &H::true1);
B("Obj::true_4c0e60", 0x004c0e60, &H::true1);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
namespace {
namespace game_b98_03 {


// 0x005045f0 (float in st0, taken raw); true when == 0.0 or unordered.
bool speed_zero_or_nan(void* u) {
    const double v = joan::orig::arg::as<double>(joan::orig::as_unit(u)->anim_phase());  // 0x005045f0
    return !(v < 0.0 || v > 0.0);                      // fcomp [0x9cf5c4] = 0.0f; test ah,0x40
}

struct H {
    // ------------------------------------------------------------------------ 0x004c0f30
    u32 unit_not_moving(void* u) {
        if (static_cast<i32>(jo::as_unit(u)->action) != 1) return 1;
        return speed_zero_or_nan(u) ? 1u : 0u;
    }

    // ------------------------------------------------------------------------ 0x004c11f0
    void* release_unit(void* u) {
        joan::orig::as_unit(u)->play_idle(0, 1);  // 0x00508b90
        jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
        return this;  // eax = this, as the original leaves it
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("Obj::unit_not_moving", 0x004c0f30, &H::unit_not_moving);
B98("Obj::release_unit", 0x004c11f0, &H::release_unit);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {


struct H {
    // ------------------------------------------------------------------------ 0x004c0ff0
    i32 always_1(void*) { return 1; }

    // ------------------------------------------------------------------------ 0x004c1230
    void* reset_1d(void* o) {
        joan::orig::as_unit(o)->play_idle(0, 1);  // 0x00508b90
        jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("Handler::always_1", 0x004c0ff0, &H::always_1);
B96("Handler::reset_1d", 0x004c1230, &H::reset_1d);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
namespace {
namespace game_b97_03 {


struct H {
    // ------------------------------------------------------------------------ 0x004c10f0
    i32 is_idle_or_zero(void* unit) {
        if (jo::self<jo::Task>(this)->current.expire_time < global<u32>(d::g_now)) return 1;
        const double v = joan::orig::arg::as<double>(joan::orig::as_unit(unit)->anim_phase());  // 0x005045f0
        // fcomp 0.0f; test ah,0x40; je -> 0: equal or unordered gives 1.
        if (v == 0.0 || v != v) return 1;
        return 0;
    }

    // ------------------------------------------------------------------------ 0x004c1860
    u32 stop_unit(void* unit) { joan::orig::as_unit(unit)->play_idle(0, 1); return 0; }  // 0x00508b90 (void; Action::dispatch discards the result)
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("Unit::is_idle_or_zero", 0x004c10f0, &H::is_idle_or_zero);
B97("X::stop_unit", 0x004c1860, &H::stop_unit);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Order::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::obj_ctor>(this)); }  // 0x4bab50
auto joan::orig::Order::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_05::H::dtor_4baba0>(this)); }  // 0x4baba0
auto joan::orig::Order::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_03::H::clear_1>(this)); }  // 0x4bac30
auto joan::orig::Order::add(void* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_02::H::event_add>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x4bac60
auto joan::orig::cmp_byte4_desc(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_03::cmp_byte4_desc>(a0, a1)); }  // 0x4bae70
auto joan::orig::List::forward_584a10(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_03::H::forward_584a10>(this, a0)); }  // 0x4baee0
auto joan::orig::Order::pop_first() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_02::H::copy_first>(this)); }  // 0x4baf10
auto joan::orig::Ai::update_4baf80(void* a0) -> void { joan::bind::member<&::game_b108_04::H::ai_update_4baf80>(this, a0); }  // 0x4baf80
auto joan::orig::Order::check_condition(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_05::H::dispatch>(this, a0)); }  // 0x4bb3e0
auto joan::orig::Action::dispatch(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_06::H::dispatch>(this, a0)); }  // 0x4bb660
auto joan::orig::List::contains(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_03::H::contains>(this, a0)); }  // 0x4bb8d0
auto joan::orig::OrderEntry::set(std::uint32_t a0, std::uint8_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_02::H::record_set>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x4bb950
auto joan::orig::OrderEntry::same_id(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_02::H::same_id>(this, a0)); }  // 0x4bb9d0
auto joan::orig::OrderEntry::byte_at(std::int8_t a0, std::int8_t a1) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b100_05::H::byte_at>(this, a0, a1)); }  // 0x4bba00
auto joan::orig::pack_chars_12(const char* a0, void* a1, void* a2, void* a3) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_b99_03::pack_chars_12>(a0, a1, a2, a3)); }  // 0x4bbac0
auto joan::orig::action::from_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_03::action_from_name>(a0)); }  // 0x4bbbc0
auto joan::orig::Ai::follow_4bd420(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_b109_03::H::follow_4bd420>(this, a0, a1); }  // 0x4bd420
auto joan::orig::Order::step_move_4bfa10(void* a0, float a1, float a2, float a3) -> void { joan::bind::member<&::game_b108_14::H::step_move>(this, a0, a1, a2, a3); }  // 0x4bfa10
auto joan::orig::WeatherSystem::start_transition(std::int32_t a0, std::uint32_t a1, std::uint32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b104_04::H::reset_4c02b0>(this, a0, a1, a2)); }  // 0x4c02b0
auto joan::orig::SoundList::play_zone_sound(std::int16_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_19::H::attach>(this, a0)); }  // 0x4c0680
auto joan::orig::Order::cond_not_kind_1c_49(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_02::H::type_ok>(this, a0)); }  // 0x4c0c40
auto joan::orig::Cmd::accepts(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_03::H::accepts>(this, a0)); }  // 0x4c0c90
auto joan::orig::Order::cond_not_kind_1c_49_b(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_03::H::type_ok_1c_49>(this, a0)); }  // 0x4c0d40
auto joan::orig::Order::cond_kind_allowed(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_02::H::kind_allowed>(this, a0)); }  // 0x4c0d90
auto joan::orig::Order::cond_true_c(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b105_06::H::true1>(this, a0)); }  // 0x4c0e40
auto joan::orig::Order::cond_true_d(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b105_06::H::true1>(this, a0)); }  // 0x4c0e60
auto joan::orig::Order::cond_true_e(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_05::H::ret1_4c0e80>(this, a0)); }  // 0x4c0e80
auto joan::orig::Order::cond_not_kind_2b_2d(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_03::H::type_ok_2b_2d>(this, a0)); }  // 0x4c0ee0
auto joan::orig::Order::cond_unit_not_moving(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_03::H::unit_not_moving>(this, a0)); }  // 0x4c0f30
auto joan::orig::Ai::kind_ok(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_03::H::kind_ok>(this, a0)); }  // 0x4c0f90
auto joan::orig::Order::cond_true_6(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_03::H::always_1>(this, a0)); }  // 0x4c0ff0
auto joan::orig::Order::cond_timer_done(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_02::H::task_c1090>(this, a0)); }  // 0x4c1090
auto joan::orig::Unit::is_idle_or_zero(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_03::H::is_idle_or_zero>(this, a0)); }  // 0x4c10f0
auto joan::orig::Unit::is_idle(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_05::H::is_idle>(this, a0)); }  // 0x4c1150
auto joan::orig::Order::cond_true_16(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_03::H::always_1>(this, a0)); }  // 0x4c11b0
auto joan::orig::Cmd::finish_4c1270(void* a0) -> void { joan::bind::member<&::game_b92_03::H::finish_1270>(this, a0); }  // 0x4c1270
auto joan::orig::Cmd::finish_4c1390(void* a0) -> void { joan::bind::member<&::game_b92_03::H::finish_1390>(this, a0); }  // 0x4c1390
auto joan::orig::Order::release_unit(void* a0) -> void { joan::bind::member<&::game_b93_02::H::release_unit>(this, a0); }  // 0x4c1480
auto joan::orig::Cmd::finish_4c14e0(void* a0) -> void { joan::bind::member<&::game_b92_03::H::finish_14e0>(this, a0); }  // 0x4c14e0
auto joan::orig::Cmd::finish_4c1670(void* a0) -> void { joan::bind::member<&::game_b92_03::H::finish_1670>(this, a0); }  // 0x4c1670
auto joan::orig::Cmd::finish_4c17a0(void* a0) -> void { joan::bind::member<&::game_b92_03::H::finish_17a0>(this, a0); }  // 0x4c17a0
auto joan::orig::PemiRes::reset() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_03::H::clear_8>(this)); }  // 0x4c1940
