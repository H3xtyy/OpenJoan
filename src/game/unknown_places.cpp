// game/unknown_places.cpp: functions whose original source file is unknown (formerly game/_unplaced/places.cpp;
// re/attribution.csv: between ww/pemires.cpp and ww/plasma.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdarg>

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

using joan::img::obj;

using u8 = std::uint8_t;
using u16 = std::uint16_t;

using joan::hook::global;

using i16 = std::int16_t;

void* const kWorld = obj(d::g_nations);

}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

using joan::call::thiscall;

struct H {
    void wrap_4c2110() { joan::orig::as_place(this)->detach(); }  // 0x00425a40
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Wrap::f_4c2110", 0x004c2110, &H::wrap_4c2110);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using u32 = std::uint32_t;

struct H {
    u32 get_pos(u32* out) {  // 0x004c21c0
        out[0] = jo::view<u32>(jo::self<jo::Place>(this)->position_[0]);
        out[1] = jo::view<u32>(jo::self<jo::Place>(this)->position_[1]);
        out[2] = jo::view<u32>(jo::self<jo::Place>(this)->position_[2]);
        return jo::view<u32>(jo::self<jo::Place>(this)->position_[2]);
    }
    void* set_1ce(u32 v) {  // 0x004c46f0
        jo::self<jo::Place>(this)->captured = v;
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("X::get_pos", 0x004c21c0, &H::get_pos);
B99("X::set_1ce", 0x004c46f0, &H::set_1ce);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

struct H {
    float value_4c2240() { return jo::view<float>(jo::self<jo::Place>(this)->angle); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::value_4c2240", 0x004c2240, &H::value_4c2240);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b87_01.cpp (game/b87_01)
namespace {
namespace game_b87_01 {

using joan::call::thiscall;

using joan::orig::helpers::stricmp_;

JOAN_COUNTER(c_link_place, "Faction::link_place");
JOAN_COUNTER(c_places_alloc, "Places::alloc");
JOAN_COUNTER(c_places_find, "Places::find");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // ---- 0x004c39d0 Faction::link_place(place) ret 4 ----------------------------------------
    void link_place(std::uint8_t* place) {
        c_link_place.hit();
        std::uint8_t* volatile me = self();
        std::uint8_t* const owner = jo::view<std::uint8_t*>(jo::as_facility(place)->owner);
        if (me == owner) return;
        if (owner != nullptr) {
            std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_facility(place)->prev);
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_facility(place)->next);
            if (prev != nullptr) {
                jo::view<std::uint8_t*>(jo::as_facility(prev)->next) = next;
            } else {
                jo::view<std::uint8_t*>(jo::as_place(owner)->facility_head_) = next;
            }
            if (next != nullptr) {
                jo::view<std::uint8_t*>(jo::as_facility(next)->prev) = prev;
            } else {
                jo::view<std::uint8_t*>(jo::as_place(owner)->facility_tail) = prev;
            }
            jo::view<std::uint8_t*>(jo::as_facility(place)->prev) = nullptr;
            jo::view<std::uint8_t*>(jo::as_facility(place)->next) = nullptr;
            jo::view<std::uint8_t*>(jo::as_facility(place)->owner) = nullptr;
        }
        std::uint8_t* const o = me;
        if (o != nullptr) {
            // prev = 0 (insert at the head)
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_place(o)->facility_head_);
            jo::view<std::uint8_t*>(jo::as_place(o)->facility_head_) = place;
            if (next != nullptr) {
                jo::view<std::uint8_t*>(jo::as_facility(next)->prev) = place;
            } else {
                jo::view<std::uint8_t*>(jo::as_place(o)->facility_tail) = place;
            }
            jo::view<std::uint8_t*>(jo::as_facility(place)->prev) = nullptr;
            jo::view<std::uint8_t*>(jo::as_facility(place)->next) = next;
            jo::view<std::uint8_t*>(jo::as_facility(place)->owner) = o;
        }
    }

    // ---- 0x004c5370 Places::alloc() -----------------------------------------------------------
    std::int32_t places_alloc() {
        c_places_alloc.hit();
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Place)));  // 0x1f6 on x86; operator new
        void* rec = mem != nullptr ? joan::orig::as_place(mem)->construct() : nullptr;  // 0x00422b50
        joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&rec));  // 0x005857a0
        const std::int32_t n = joan::orig::as_dynarray(this)->cursor();  // 0x00417520
        jo::as_place(rec)->index = static_cast<std::int16_t>(n);
        return static_cast<std::int32_t>(jo::as_place(rec)->index);
    }

    // ---- 0x004c54a0 Places::find(name) ret 4 ------------------------------------------------
    std::int32_t places_find(const char* name) {
        c_places_find.hit();
        for (std::int32_t i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {  // 0x00417ad0
            std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (p == nullptr) continue;
            if (stricmp_(jo::as_place(p)->name, name) == 0) return i;
        }
        return 0;
    }

    // ---- 0x004c22b0 Place::add_entrance(vec*) ret 4 -----------------------------------------
    void add_entrance(const std::uint8_t* v) {
        if (jo::self<jo::Place>(this)->entrance_count >= 8) return;
        std::memcpy(&jo::self<jo::Place>(this)->entrances[jo::self<jo::Place>(this)->entrance_count * 3], v, 12);
        jo::self<jo::Place>(this)->entrance_count = jo::self<jo::Place>(this)->entrance_count + 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_01", name, addr, fn)
B("Faction::link_place", 0x004c39d0, &H::link_place);
B("Places::alloc", 0x004c5370, &H::places_alloc);
B("Places::find", 0x004c54a0, &H::places_find);
B("Place::add_entrance", 0x004c22b0, &H::add_entrance);
#undef B

}  // namespace game_b87_01
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

struct H {
    // 0x004c2470
    void* rec_at_c(std::int32_t i) { return &jo::self<jo::Place>(this)->slot_pos_[(static_cast<std::ptrdiff_t>(i) - 1) * 3]; }
    // 0x004c24b0
    float float_at(std::int32_t i) { return jo::self<jo::Place>(this)->slot_yaw[i]; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Rec::at_c", 0x004c2470, &H::rec_at_c);
M("Rec::float_at", 0x004c24b0, &H::float_at);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
namespace {
namespace game_b97_03 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004c2540
    i32 set_slot(i32 i, u32 v) {
        jo::view<u32>(jo::self<jo::Place>(this)->slot_unit[i]) = v;
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(v)));  // 0x0041b3c0
        jo::view<u16>(jo::as_unit(p)->place_index) = jo::view<u16>(jo::self<jo::Place>(this)->index);
        jo::view<u16>(jo::as_unit(p)->place_slot) = static_cast<u16>(i);
        return 1;
    }

    // ------------------------------------------------------------------------ 0x004c51d0
    u32 dtor_c5() {
        joan::orig::as_list(this)->delete_all_4c5260();                // 0x004c5260
        return thiscall<u32>(fn::dtor_DynArrayB, this);    // 0x00585710
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("X::set_slot", 0x004c2540, &H::set_slot);
B97("X::dtor_c5", 0x004c51d0, &H::dtor_c5);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
namespace {
namespace game_b98_03 {

using joan::call::thiscall;

using u32 = std::uint32_t;

// 0x005045f0 (float in st0, taken raw); true when == 0.0 or unordered.
bool speed_zero_or_nan(void* u) {
    const double v = joan::orig::arg::as<double>(joan::orig::as_unit(u)->anim_phase());  // 0x005045f0
    return !(v < 0.0 || v > 0.0);                      // fcomp [0x9cf5c4] = 0.0f; test ah,0x40
}

JOAN_COUNTER(c_check_unit, "Obj::check_unit");
JOAN_COUNTER(c_find_owned, "List::find_owned");

struct H {
    // ------------------------------------------------------------------------ 0x004c25a0
    void check_unit(void* u) {
        c_check_unit.hit();
        const i32 t = jo::as_unit(u)->anim_kind;
        if (t == 0x40 || t == 0x41 || t == 0x42 || t == 0x3c || t == 0x29 || t == 0x4e) {
            jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
            return;
        }
        if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_special())) == 0) {  // 0x00508ab0
            joan::orig::as_unit(u)->play_idle(0, 1);                           // 0x00508b90
            return;
        }
        if (speed_zero_or_nan(u)) {
            joan::orig::as_unit(u)->play_idle(0, 1);  // 0x00508b90
            return;
        }
        if (jo::self<jo::Order>(this)->current.expire_time < global<u32>(d::g_now)) jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
    }

    // ------------------------------------------------------------------------ 0x004c5530
    i32 find_owned(const void* a, void* b) {  // a: a position, b: the World (pointers; Phase 2 unplaced-b-4)
        c_find_owned.hit();
        for (i32 i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {  // 0x00417ad0
            void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (e == nullptr) continue;
            if (b != jo::as_place(e)->world) continue;
            if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::as_place(e)->box)->contains(a)) & 0xff) != 0) return i;  // 0x0060d5f0
        }
        return 0;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("Obj::check_unit", 0x004c25a0, &H::check_unit);
B98("List::find_owned", 0x004c5530, &H::find_owned);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b104_04.cpp (game/b104_04)
namespace {
namespace game_b104_04 {

using joan::call::thiscall;

using u32 = std::uint32_t;


void* const kUnits = obj(d::g_units);
void* const kArmies = obj(d::g_items);
u32 clock_ms() { return global<u32>(d::g_now); }

i32 unit_dead(u32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_4c2680, "ArmyOrder::on_4c2680");
JOAN_COUNTER(c_4c3330, "Squad::tick_4c3330");

// unit (army member) helpers shared by the order handlers
i16 kind(const u8* u) { return jo::as_unit(u)->anim_kind; }
float unit_wait(u8* u) { return joan::orig::arg::as<float>(joan::orig::as_unit(u)->anim_phase()); }  // 0x005045f0
void set_order(u8* u, i32 order) { joan::orig::as_unit(u)->set_action(joan::orig::arg::i32(order), 0, 1); }  // 0x00501190
void stop_unit(u8* u) { joan::orig::as_unit(u)->play_idle(0, 1); }                   // 0x00508b90

struct H {
    // ---------------------------------------------------------------- 0x004c2680
    // 0x00425d20 record: [+0x86] != 0 || [[+0x86] + 4] != 0 -> 0x005abe30(...)
    void on_4c2680(u8* u) {
        c_4c2680.hit();
        if (jo::self<jo::Ai>(this)->current.expire_time < clock_ms()) {
            jo::view<u8>(jo::as_unit(u)->combo_state) = 0;
            jo::as_unit(u)->armour_flags[0] = 0;
            stop_unit(u);
            jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            return;
        }
        if (static_cast<i32>(kind(u)) == jo::self<jo::Ai>(this)->current.target + 0x6b) {
            const float v = unit_wait(u);
            const float z = global<float>(d::k_9cf5c4);
            if (v < z || v > z) return;  // only equal or unordered goes on
            if (jo::self<jo::Ai>(this)->current.spread > 0) {
                set_order(u, jo::self<jo::Ai>(this)->current.target + 0x6b);
                jo::self<jo::Ai>(this)->current.spread = jo::self<jo::Ai>(this)->current.spread - 1;
                fire_army(u);
            } else {
                jo::view<u8>(jo::as_unit(u)->combo_state) = 0;
                jo::as_unit(u)->armour_flags[0] = 0;
                stop_unit(u);
                jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0;
            }
            return;
        }
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_special()) == 0) return;  // 0x00508ab0
        if (!(jo::self<jo::Ai>(this)->current.spread > 0)) return;
        set_order(u, jo::self<jo::Ai>(this)->current.target + 0x6b);
        jo::view<u32>(jo::self<jo::Ai>(this)->current.kind) = 0x16;
        jo::self<jo::Ai>(this)->current.spread = jo::self<jo::Ai>(this)->current.spread - 1;
        fire_army(u);
    }
    void fire_army(u8* u) {
        if (joan::orig::as_units(kArmies)->list.empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.radius))) != 0) return;  // 0x00425d50
        u8* const army = joan::orig::arg::as<u8*>(joan::orig::as_units(kArmies)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Ai>(this)->current.radius))));  // 0x00425d20
        if (jo::as_map_object(army)->actor == nullptr) {
            // the original dereferences the null pointer here ([0 + 4])
            const std::uintptr_t p = reinterpret_cast<std::uintptr_t>(jo::as_map_object(army)->actor);
#if defined(__GNUC__) && !defined(__clang__)
// TODO(fix): the original reads [0 + 4] of a null actor and faults (real null read, kept as is)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
            if (*reinterpret_cast<void* const volatile*>(p + offsetof(jo::Actor, anim)) == nullptr) return;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        }
        const u32 slot = joan::orig::arg::as<u32>(joan::orig::as_cell_object(u)->find_anim_state());                            // 0x00504010
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(u)->anim_set)->entry(joan::orig::arg::u32(slot)));     // 0x004168d0
        const u32 now = clock_ms();
        void* const fx = jo::view<void*>(jo::as_map_object(army)->actor);
        joan::orig::as_actor(fx)->start_anim(joan::orig::arg::mptr(jo::as_actor(fx)->anim), jo::as_anim_group_rec(rec)->bip_name, joan::orig::arg::u32(now), 0u, 0u, 0u);  // 0x005abe30
    }

    // ---------------------------------------------------------------- 0x004c3330
    void tick_4c3330() {
        c_4c3330.hit();
        for (i32 i = 1; i <= jo::self<jo::Place>(this)->slot_count; ++i) {
            u32& slot = jo::view<u32>(jo::self<jo::Place>(this)->slot_unit[i]);
            if (slot == 0) continue;
            if (unit_dead(slot) != 0) {
                slot = 0;
                continue;
            }
            u8* const rec = unit_rec(slot);
            if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::self<jo::Place>(this)->box)->contains(joan::orig::arg::ptr(jo::as_unit(rec)->position_))) == 0) {  // 0x0060d5f0
                slot = 0;
                jo::view<u16>(jo::as_unit(rec)->place_index) = 0;
                jo::view<u16>(jo::as_unit(rec)->place_slot) = 0;
            }
        }
        jo::self<jo::Place>(this)->timer = jo::self<jo::Place>(this)->timer + global<u32>(d::g_frame_step_ms);
        if (jo::self<jo::Place>(this)->timer < 0xea60u) return;
        const i32 cap = joan::orig::arg::as<i32>(joan::orig::as_place(this)->recruit_cap());  // 0x004693b0
        if (cap > joan::orig::arg::as<i32>(joan::orig::as_place(this)->recruit_pool())) {      // 0x00469360
            joan::orig::as_unit(this)->set_52(joan::orig::arg::i32(joan::orig::arg::as<i32>(joan::orig::as_place(this)->recruit_pool()) + 1));  // 0x00469330
        }
        jo::self<jo::Place>(this)->timer = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_04", name, addr, fn)
B("ArmyOrder::on_4c2680", 0x004c2680, &H::on_4c2680);
B("Squad::tick_4c3330", 0x004c3330, &H::tick_4c3330);
#undef B

}  // namespace game_b104_04
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
namespace {
namespace game_b100_05 {

using joan::call::thiscall;

using u32 = std::uint32_t;


void* const kGroups = obj(d::g_types);

JOAN_COUNTER(c_count_in_range, "Army::count_in_range");

struct H {
    // ------------------------------------------------------------------ 0x004c2b50
    i32 count_in_range(const char* id) {  // id: an owner name (a script string value; Phase 2 unplaced-b-4)
        c_count_in_range.hit();
        if (id == nullptr) return 0;
        const u32 owner = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(id));  // 0x004b91a0
        const i32 kind = joan::orig::arg::as<i32>(joan::orig::as_list(kWorld)->find_by_word2(joan::orig::arg::i32(owner)));          // 0x004b67e0
        if (jo::self<jo::Place>(this)->world == nullptr) return 0;
        i32 count = 0;
        u8* p10 = nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kWorld)->is_null(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Place>(this)->nation)))) == 0)       // 0x00484870
            p10 = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kWorld)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Place>(this)->nation))));  // 0x0047b600
        i32 n = 0;
        u8** const list = joan::orig::arg::as<u8**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::Place>(this)->world))->cells_in_box(joan::orig::arg::mptr(jo::self<jo::Place>(this)->box), &n));  // 0x00429090
        for (i32 j = 0; j < n; ++j) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(list[j])->get_8c());  // 0x0054ee10
            u8* cur = first;
            if (cur == nullptr) continue;
            do {
                if (jo::as_unit(cur)->hit_points > 0 || jo::view<u32>(jo::as_unit(cur)->state_120) != 0) {
                    const u8 in = joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::self<jo::Place>(this)->box)->contains(joan::orig::arg::ptr(jo::as_unit(cur)->position_)));  // 0x0060d5f0
                    if (in != 0) {
                        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kGroups)->get(joan::orig::arg::i16(jo::as_unit(cur)->type_id)));  // 0x0042d0a0
                        const i32 s = static_cast<i32>(jo::as_unit_type(e)->unit_class);
                        if (s != 0 && s != 8 && s != 9) {
                            const i32 t = static_cast<i32>(jo::as_unit_type(e)->kind);
                            if (t != 7 && t != 2 && t != 3 && jo::as_unit(cur)->rank >= 4) {
                                if (static_cast<i32>(jo::as_unit(cur)->side) == kind) ++count;
                            }
                        }
                    }
                }
                cur = jo::view<u8*>(jo::as_unit(cur)->cell_prev);
            } while (cur != nullptr && cur != first);
        }
        joan::orig::as_place(this)->facility_head();  // 0x004c48b0 (result unused)
        (void)*static_cast<volatile i16*>(reinterpret_cast<i16*>(p10));  // movsx eax, word [p10]
        return count;
    }

    // ------------------------------------------------------------------ 0x004c51a0
    H* ctor_4c51a0() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x1e, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Army::count_in_range", 0x004c2b50, &H::count_in_range);
B("Obj::ctor_4c51a0", 0x004c51a0, &H::ctor_4c51a0);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b101_05.cpp (game/b101_05)
namespace {
namespace game_b101_05 {

using joan::call::thiscall;

using u32 = std::uint32_t;

// Word read that may hit a null pointer exactly where the original faults.
i32 read_short(const void* p) {
    return static_cast<i32>(*reinterpret_cast<const volatile i16*>(reinterpret_cast<std::uintptr_t>(p)));
}

JOAN_COUNTER(c_find_enemy_unit, "Army::find_enemy_unit");
JOAN_COUNTER(c_find_own_unit, "Army::find_own_unit");
JOAN_COUNTER(c_clear_list_1f2, "Army::clear_list_1f2");

struct H {
    // ------------------------------------------------------------------ shared cell walk
    // Lists the cells around +0x3a; for each non-null chain head (0x0054ee10) walks the +0x33d
    // ring of units, calling `visit` on the units that are active (+0x135 > 0 or +0x120 != 0)
    // and inside +0x3a (0x0060d5f0). visit returns true to stop (result stored in *out).
    void* own_side() {
        void* side = nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kWorld)->is_null(joan::orig::arg::i32(jo::self<jo::Place>(this)->nation))) == 0) {  // 0x00484870
            side = joan::orig::as_dynarray(kWorld)->ptr_at(joan::orig::arg::i32(jo::self<jo::Place>(this)->nation));  // 0x0047b600
        }
        return side;
    }
    template <class V>
    bool walk_cells(V visit, i32* out) {
        i32 n;
        u8** const list = joan::orig::arg::as<u8**>(joan::orig::as_world(jo::self<jo::Place>(this)->world)->cells_in_box(joan::orig::arg::mptr(jo::self<jo::Place>(this)->box), &n));  // 0x00429090
        for (i32 i = 0; i < n; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(list[i])->get_8c());  // 0x0054ee10
            if (first == nullptr) continue;
            u8* p = first;
            do {
                if (jo::as_unit(p)->hit_points > 0 || jo::as_unit(p)->state_120 != 0) {
                    const u32 in = joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::self<jo::Place>(this)->box)->contains(joan::orig::arg::ptr(jo::as_unit(p)->position_)));  // 0x0060d5f0
                    if ((in & 0xffu) != 0 && visit(p, out)) return true;
                }
                p = jo::view<u8*>(jo::as_unit(p)->cell_prev);
            } while (p != nullptr && p != first);
        }
        return false;
    }

    // ------------------------------------------------------------------ 0x004c2d90
    i32 find_enemy_unit() {
        c_find_enemy_unit.hit();
        if (jo::self<jo::Place>(this)->world == nullptr) return 0;
        void* const mine = own_side();
        i32 result = 0;
        if (walk_cells(
                [&](u8* p, i32* out) {
                    if (jo::as_unit(p)->side == 0) return false;
                    const i32 ps = static_cast<i32>(jo::as_unit(p)->side);
                    const i32 ms = read_short(mine);
                    if (joan::orig::arg::as<i32>(joan::orig::as_players(kWorld)->hostile(joan::orig::arg::i32(ms), joan::orig::arg::i32(ps))) == 0) return false;  // 0x004b6860
                    *out = static_cast<i32>(jo::as_unit(p)->id);
                    return true;
                },
                &result))
            return result;
        return 0;
    }

    // ------------------------------------------------------------------ 0x004c3040
    i32 find_own_unit() {
        c_find_own_unit.hit();
        if (jo::self<jo::Place>(this)->world == nullptr) return 0;
        void* const mine = own_side();
        i32 result = 0;
        if (walk_cells(
                [&](u8* p, i32* out) {
                    if (jo::as_unit(p)->side == 0) return false;
                    if (read_short(mine) != static_cast<i32>(jo::as_unit(p)->side)) return false;
                    *out = static_cast<i32>(jo::as_unit(p)->id);
                    return true;
                },
                &result))
            return result;
        return 0;
    }

    // ------------------------------------------------------------------ 0x004c36c0
    void nop_4c36c0() {}

    // ------------------------------------------------------------------ 0x004c40f0
    H* clear_list_1f2() {
        c_clear_list_1f2.hit();
        u8* p = jo::view<u8*>(jo::self<jo::Place>(this)->facility_head_);
        while (p != nullptr) {
            u8* const next = jo::view<u8*>(jo::as_facility(p)->next);
            joan::orig::as_list(this)->unlink(joan::orig::arg::mptr(p));  // 0x004c3d80
            p = next;
        }
        jo::self<jo::Place>(this)->facility_head_ = nullptr;
        jo::self<jo::Place>(this)->facility_tail = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_05", name, addr, fn)
B("Army::find_enemy_unit", 0x004c2d90, &H::find_enemy_unit);
B("Army::find_own_unit", 0x004c3040, &H::find_own_unit);
B("Obj::nop_4c36c0", 0x004c36c0, &H::nop_4c36c0);
B("Army::clear_list_1f2", 0x004c40f0, &H::clear_list_1f2);
#undef B

}  // namespace game_b101_05
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
namespace {
namespace game_b95_03 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_unlink, "List::unlink");

void JOAN_CDECL init_pi_derived() {  // 0x004c50d0
    global<float>(d::g_a3a070) = global<float>(d::g_pi_a3a06c) / global<float>(d::flt_2);
}

struct H {
    // ------------------------------------------------------------------------ 0x004c3d80
    void* unlink(u8* node) {
        c_unlink.hit();
        u8* const owner = jo::view<u8*>(jo::as_facility(node)->owner);
        if (owner != nullptr) {
            u8* const prev = jo::view<u8*>(jo::as_facility(node)->prev);
            u8* const next = jo::view<u8*>(jo::as_facility(node)->next);
            if (prev != nullptr)
                jo::view<u8*>(jo::as_facility(prev)->next) = next;
            else
                jo::view<u8*>(jo::as_place(owner)->facility_head_) = next;
            if (next != nullptr)
                jo::view<u8*>(jo::as_facility(next)->prev) = prev;
            else
                jo::view<u8*>(jo::as_place(owner)->facility_tail) = prev;
            jo::as_facility(node)->prev = nullptr;
            jo::as_facility(node)->next = nullptr;
            jo::as_facility(node)->owner = nullptr;
        }
        return node;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("List::unlink", 0x004c3d80, &H::unlink);
B95("init_pi_derived", 0x004c50d0, &init_pi_derived);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kUnits = obj(d::g_items);
void* const kUnitList = obj(d::g_unitList);

JOAN_COUNTER(c_place_units, "Formation::place_units");

struct H {
    // ------------------------------------------------------------------------ 0x004c4440
    void place_units(i32 kind, u32 side) {
        c_place_units.hit();
        for (i32 i = joan::orig::as_dynarray(kUnitList)->size(); i > 0; --i) {  // 0x00417ad0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitList)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (e == nullptr) continue;
            if (static_cast<i32>(jo::as_map_object(e)->type_id) != kind) continue;
            const u8 in = joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::self<jo::Place>(this)->box)->contains(joan::orig::arg::ptr(jo::as_map_object(e)->pos)));  // 0x0060d5f0
            if (in == 0) continue;
            const V3 pos = jo::view<V3>(jo::as_map_object(e)->pos);
            const float a = joan::orig::arg::as<float>(joan::orig::as_map_object(e)->yaw());  // 0x00499e20
            const float b = joan::orig::arg::as<float>(joan::orig::as_unit(e)->get_66());  // 0x0049a8c0
            const float c = joan::orig::arg::as<float>(joan::orig::as_map_object(e)->roll());  // 0x0049ad10
            joan::orig::as_units(kUnits)->remove_49f370(joan::orig::arg::i32(i));           // 0x0049f370
            const u32 id = joan::orig::arg::as<u32>(joan::orig::as_units(kUnits)->create_49ea90(joan::orig::arg::u32(side)));    // 0x0049ea90
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
            joan::orig::as_map_object(u)->set_world(jo::self<jo::Place>(this)->world);             // 0x004994b0
            joan::orig::as_map_object(u)->set_position(pos);                       // 0x0041ff50
            joan::orig::as_map_object(u)->set_yaw(joan::orig::arg::bits(a));                         // 0x00499fd0
            joan::orig::as_unit(u)->set_66(joan::orig::arg::f32(b));                         // 0x0049ab00
            joan::orig::as_map_object(u)->set_roll(joan::orig::arg::bits(c));                         // 0x0049adc0
            jo::view<u16>(jo::as_map_object(u)->side) = static_cast<u16>(jo::self<jo::Place>(this)->nation);
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Formation::place_units", 0x004c4440, &H::place_units);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

struct H {
    // 0x004c48b0
    void* get_1ee() { return jo::self<jo::Place>(this)->facility_tail; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::get_1ee", 0x004c48b0, &H::get_1ee);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {

using u32 = std::uint32_t;

// ------------------------------------------------------------------------------- cdecl
void* JOAN_CDECL get_71(const void* p) { return jo::as_facility(p)->prev; }  // 0x004c4a60

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("get_71", 0x004c4a60, &get_71);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

using joan::call::thiscall;

using u32 = std::uint32_t;

JOAN_COUNTER(c_refresh_4c55d0, "UnitList::refresh_4c55d0");

struct H {
    void* get1f2() { return jo::self<jo::Place>(this)->facility_head_; }
    void nop() {}

    // ---- unit list helpers ----
    i32 count() { return joan::orig::as_dynarray(this)->size(); }  // 0x00417ad0

    // ------------------------------------------------------------------------ 0x004c55d0
    i32 refresh_4c55d0() {
        c_refresh_4c55d0.hit();
        i32 i = 1;
        for (;;) {
            const i32 n = count();
            if (i > n) return n;
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_missing_ca0(joan::orig::arg::i32(i))) == 0) {                    // 0x00456ca0
                void* e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_c70(joan::orig::arg::i32(i)));              // 0x00456c70
                joan::orig::as_squad(e)->tick_4c3330();                                 // 0x004c3330
            }
            ++i;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::get1f2_4c4be0", 0x004c4be0, &H::get1f2);
B("Obj::nop_4c5240", 0x004c5240, &H::nop);
B("UnitList::refresh_4c55d0", 0x004c55d0, &H::refresh_4c55d0);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
namespace {
namespace game_b102_04 {

using u32 = std::uint32_t;

// ---------------------------------------------------------------------------- 0x004c4c00
static void* JOAN_CDECL get_75(const void* p) { return jo::as_facility(p)->next; }

// ---------------------------------------------------------------------------- 0x004c4d50
static void JOAN_CDECL init_4c4d50() { joan::orig::init_pi(); }  // direct call in the original

// ---------------------------------------------------------------------------- 0x004c4f30
static void JOAN_CDECL init_4c4f30() { joan::orig::init_pi_derived(); }  // direct call in the original

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("get_75", 0x004c4c00, &get_75);
B("init_4c4d50", 0x004c4d50, &init_4c4d50);
B("init_4c4f30", 0x004c4f30, &init_4c4f30);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
namespace {
namespace game_b94_03 {

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL init_pi() {  // 0x004c4eb0
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_pi_a3a06c) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("init_pi", 0x004c4eb0, &init_pi);
#undef B94

}  // namespace game_b94_03
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
namespace {
namespace game_b103_04 {

using joan::call::thiscall;

using u32 = std::uint32_t;

JOAN_COUNTER(c_refresh, "List::refresh_4c5640");

// ------------------------------------------------------------------ static initialisers
void JOAN_CDECL init_4c5100() { joan::orig::init_a3a074(); }
void JOAN_CDECL init_4c5150() { joan::orig::init_a3a078(); }

struct H {
    // ------------------------------------------------------------------------ 0x004c5430
    u32 remove_4c5430(i32 i) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_c70(joan::orig::arg::i32(i)));  // 0x00456c70
        if (p != nullptr) joan::orig::as_place(p)->destroy(1u);     // 0x004c5320 scalar dtor
        return thiscall<u32>(fn::linkout_5858c0, this, i);             // 0x005858c0
    }

    // ------------------------------------------------------------------------ 0x004c5640
    u32 refresh_4c5640() {
        c_refresh.hit();
        for (i32 i = 1;; ++i) {
            const u32 n = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->size());  // 0x00417ad0
            if (i > static_cast<i32>(n)) return n;
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_missing_ca0(joan::orig::arg::i32(i))) != 0) continue;  // 0x00456ca0
            joan::orig::as_place(joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_c70(joan::orig::arg::i32(i))))->nop_4c36c0();  // 0x004c36c0, 0x00456c70
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("init_4c5100", 0x004c5100, &init_4c5100);
B("init_4c5150", 0x004c5150, &init_4c5150);
B("List::remove_4c5430", 0x004c5430, &H::remove_4c5430);
B("List::refresh_4c5640", 0x004c5640, &H::refresh_4c5640);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
namespace {
namespace game_b84_03 {

using joan::call::thiscall;

using joan::call::thiscall;

JOAN_COUNTER(c_delete_all, "List::delete_all");

struct H {
    // ------------------------------------------------------------------ 0x004c5260
    std::uint32_t delete_all_4c5260() {
        c_delete_all.hit();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_missing_ca0(joan::orig::arg::i32(i))) != 0) continue;           // 0x00456ca0
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_c70(joan::orig::arg::i32(i)));                      // 0x00456c70
            if (p != nullptr) joan::orig::as_place(p)->destroy(1u);                       // 0x004c5320
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));                     // 0x005858c0
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        return 0;  // callers (0x00471010, 0x0047ff50, 0x004c51d0 and their replacements) discard the result; 0x00585d20 returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("List::delete_all_4c5260", 0x004c5260, &H::delete_all_4c5260);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::call::thiscall;

using joan::orig::helpers::game_delete;                      // operator delete

using joan::orig::helpers::scalar_delete;

struct H {
    void* sdd_4c5320(std::uint32_t flags) { joan::orig::as_wrap(this)->f_4c2110(); scalar_delete(this, flags); return this; }  // 0x004c2110
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj4c5320::sdd", 0x004c5320, &H::sdd_4c5320);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Wrap::f_4c2110() -> void { joan::bind::member<&::game_b86_06::H::wrap_4c2110>(this); }  // 0x4c2110
auto joan::orig::Place::position(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_03::H::get_pos>(this, a0)); }  // 0x4c21c0
auto joan::orig::Place::add_entrance(void* a0) -> void { joan::bind::member<&::game_b87_01::H::add_entrance>(this, a0); }  // 0x4c22b0
auto joan::orig::Place::slot_pos(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::rec_at_c>(this, a0)); }  // 0x4c2470
auto joan::orig::Place::slot_angle(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b89_06::H::float_at>(this, a0)); }  // 0x4c24b0
auto joan::orig::Place::set_slot(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_03::H::set_slot>(this, a0, a1)); }  // 0x4c2540
auto joan::orig::Army::count_in_range(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_05::H::count_in_range>(this, a0)); }  // 0x4c2b50
auto joan::orig::Squad::tick_4c3330() -> void { joan::bind::member<&::game_b104_04::H::tick_4c3330>(this); }  // 0x4c3330
auto joan::orig::Place::nop_4c36c0() -> void { joan::bind::member<&::game_b101_05::H::nop_4c36c0>(this); }  // 0x4c36c0
auto joan::orig::Place::link_facility(void* a0) -> void { joan::bind::member<&::game_b87_01::H::link_place>(this, a0); }  // 0x4c39d0
auto joan::orig::List::unlink(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_03::H::unlink>(this, a0)); }  // 0x4c3d80
auto joan::orig::Formation::place_units(std::int32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b93_02::H::place_units>(this, a0, a1); }  // 0x4c4440
auto joan::orig::Place::set_1ce(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_03::H::set_1ce>(this, a0)); }  // 0x4c46f0
auto joan::orig::Place::facility_head() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_03::H::get_1ee>(this)); }  // 0x4c48b0
auto joan::orig::get_71(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b96_03::get_71>(a0)); }  // 0x4c4a60
auto joan::orig::init_pi() -> void { joan::bind::free<&::game_b94_03::init_pi>(); }  // 0x4c4eb0
auto joan::orig::init_pi_derived() -> void { joan::bind::free<&::game_b95_03::init_pi_derived>(); }  // 0x4c50d0
auto joan::orig::List::delete_all_4c5260() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_03::H::delete_all_4c5260>(this)); }  // 0x4c5260
auto joan::orig::Place::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::sdd_4c5320>(this, a0)); }  // 0x4c5320
auto joan::orig::PlaceList::alloc() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b87_01::H::places_alloc>(this)); }  // 0x4c5370
auto joan::orig::PlaceList::find(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b87_01::H::places_find>(this, a0)); }  // 0x4c54a0
auto joan::orig::List::find_owned(const void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_03::H::find_owned>(this, a0, a1)); }  // 0x4c5530
