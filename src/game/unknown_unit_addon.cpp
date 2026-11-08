// game/unknown_unit_addon.cpp: functions whose original source file is unknown (formerly game/_unplaced/unit_addon.cpp;
// re/attribution.csv: late incremental-link area, no evidence; late incremental-link area). They move to their original
// file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cmath>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

struct V3 {  // 12 bytes copied as raw dwords (internal use; by-value arguments go through F3)
    u32 x, y, z;
};
// The by-value vector argument as the callee declares it: float members (System V: XMM registers).
struct F3 {
    float x, y, z;
};
F3 f3(const V3& v) {
    F3 f;
    std::memcpy(&f, &v, sizeof f);
    return f;
}
V3 v3_of(const F3& f) {
    V3 v;
    std::memcpy(&v, &f, sizeof v);
    return v;
}

void* const kUnits = obj(d::g_units);

}  // namespace

// ---------------------------------------- from modules/game_b114_14.cpp (game/b114_14)
namespace {
namespace game_b114_14 {

using joan::hook::x87::ftol;
using u16 = std::uint16_t;

// thiscall through the object's vtable slot `off`.
template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}
using joan::orig::helpers::imul;
using joan::orig::helpers::add32;
void inc_global(img::Addr a) { global<u32>(a) = global<u32>(a) + 1u; }

struct V3f {
    float x, y, z;
};
struct Str {  // 0x005a1170 string object (frame gap [ebp-0x224] .. [ebp-0x90])
    char b[0x194];
};
struct Order {  // 0x004e4520 order record (writes 0x19 bytes; 0x1c-byte slot in the frame)
    u8 b[0x1c];
};
static_assert(sizeof(V3) == 12 && sizeof(V3f) == 12 && sizeof(Str) == 0x194 && sizeof(Order) == 0x1c, "sizes");

void* const kItems = obj(d::g_items);
void* const kItemTypes = obj(d::g_item_res);
void* const kClasses = obj(d::g_types);
void* const kNations = obj(d::g_nations);
void* const kMission = obj(d::g_mission);
const Fpu kFpu{true};

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* unit_class(const void* u) {                                              // 0x0042d0a0 (word)
    return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kClasses)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));
}
u8* item_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_units(kItems)->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20
u8* item_type(i16 t) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(t))); }  // 0x00425ce0 (word)
i32 mission_mode() { return joan::orig::arg::as<i32>(joan::orig::as_scenario(kMission)->mode()); }        // 0x004cd510
using joan::orig::helpers::stricmp_;
const char* nation_name(i32 nation) {  // 0x0047b600 record + 0x14
    return joan::orig::arg::as<const char*>(joan::orig::as_dynarray(kNations)->ptr_at(joan::orig::arg::i32(nation))) + 0x14;
}
void die_(void* u, i32 killer) { joan::orig::as_unit(u)->die_7c6240(joan::orig::arg::i32(killer)); }  // 0x007c6240 (recursion)

JOAN_COUNTER(c_die, "Unit::die_7c6240");

struct H {
    // Carried list +0x1c1 -> +list_off: drop the entries whose item type is 0xf.
    void drop_carried(jo::Grid* jo::UnitData::*list_field) {
        auto list = [&]() { return jo::self<jo::UnitData>(jo::self<jo::Unit>(this)->modifiers)->*list_field; };
        i32 i = joan::orig::as_dynarray(list())->size();  // 0x00417ad0
        for (; i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(list())->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00457f60
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(list())->get(joan::orig::arg::i32(i)));      // 0x00457e90
            if (static_cast<i32>(jo::view<i16>(jo::as_item_def(item_type(jo::as_inv_item(e)->type))->class_id)) != 0xf) continue;
            const i32 id = joan::orig::arg::as<i32>(joan::orig::as_spawner(this)->fire_512830(joan::orig::arg::i16(jo::as_inv_item(e)->type), joan::orig::arg::i16(jo::as_inv_item(e)->level), joan::orig::arg::i16(jo::as_inv_item(e)->count), joan::orig::arg::u32(jo::as_inv_item(e)->u32_6), joan::orig::arg::u32(jo::as_inv_item(e)->u32_a), joan::orig::arg::u32(jo::as_inv_item(e)->u32_e)));  // 0x00512830
            if (id != 0) {
                u8* const it = item_get(id);
                if (jo::as_map_object(it)->actor != nullptr && (jo::self<jo::Unit>(this)->markers == 1 || jo::self<jo::Unit>(this)->markers == -2)) {
                    joan::orig::as_model_instance(jo::as_map_object(it)->actor)->set_10a(0u);  // 0x0042cf20
                }
            }
            joan::orig::as_item_list(list())->remove(joan::orig::arg::i32(static_cast<i32>(jo::as_inv_item(e)->type)), 0);  // 0x00458430
        }
    }

    // Item id of a carried-list entry (0 when none).
    static i32 entry_item(const u8* e) {
        if (e == nullptr) return 0;
        return joan::orig::arg::as<i32>(joan::orig::as_units(kItems)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_held_item(e)->item)))));  // 0x0049ea90
    }
    // 0x0050a480(word id, item type +0xa, flag, 0) on `u`.
    static void drop_item(u8* u, i32 id, i32 flag) {
        u8* const it = item_get(id);
        u8* const t = item_type(jo::as_map_object(it)->type_id);
        joan::orig::as_cell_object(u)->attach_unit(joan::orig::arg::i16(static_cast<i16>(id)), joan::orig::arg::i32(static_cast<i32>(jo::view<i16>(jo::as_item_def(t)->bone))), static_cast<std::int8_t>(joan::orig::arg::i32(flag)), joan::orig::arg::mptr(0));  // 0x0050a480
    }

    // Releases passenger / follower `u` (shared body of the +0x37c / +0x378 branches);
    // `reset` is the unit whose +0x21d / 0x00508b90 are reset (u, or the killer for +0x378).
    void release(u8* u, u8* reset, u8* cls, i32 i, i32 killer) {
        joan::orig::as_unit(u)->release_slot();  // 0x00528300
        if (jo::as_unit(u)->orders != nullptr) {
            if (jo::view<u32>(jo::as_order(jo::as_unit(u)->orders)->current.kind) != 0) joan::orig::as_action(jo::as_unit(u)->orders)->dispatch(joan::orig::arg::mptr(u));  // 0x004bb660
            auto* const p = jo::as_unit(u)->orders;
            if (p != nullptr) joan::orig::as_ai(p)->destroy(1u);  // 0x00415220 (deleting dtor)
            jo::as_unit(u)->orders = nullptr;
            joan::orig::as_squad(u)->clear();  // 0x00422f10
        }
        jo::view<u16>(jo::as_unit(reset)->anim_special) = 0;
        joan::orig::as_unit(reset)->play_idle(0, 1);  // 0x00508b90
        if (static_cast<i32>(jo::self<jo::UnitType>(cls)->kind) == 0xb && (static_cast<i32>(static_cast<const i8*>(jo::self<jo::UnitType>(cls)->buffers[2])[i]) & 2) != 0 &&
            joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) == 0) {  // 0x0042d0e0
            jo::view<u32>(jo::as_unit(u)->state_120) = 0;
            joan::orig::as_unit(u)->set_value_135(0);  // 0x004fe510
            die_(u, killer);
            return;
        }
        if (static_cast<i32>(jo::as_modifiers(jo::as_unit(u)->modifiers)->hand_mode) == 0) return;
        if (static_cast<i32>(jo::as_modifiers(jo::as_unit(u)->modifiers)->hand_mode) <= 1) {
            i32 id = entry_item(joan::orig::arg::as<u8*>(joan::orig::as_modifiers(jo::as_unit(u)->modifiers)->slot(0)));  // 0x004974f0
            if (id != 0) drop_item(u, id, 0);
            id = entry_item(joan::orig::arg::as<u8*>(joan::orig::as_modifiers(jo::as_unit(u)->modifiers)->slot(1)));  // 0x004974f0
            if (id != 0) {
                vcall(item_get(id), 0x1c);
                drop_item(u, id, 1);
            }
        } else {
            const i32 id = entry_item(joan::orig::arg::as<u8*>(joan::orig::as_unit_data(jo::as_unit(u)->modifiers)->slot_a()));  // 0x00497560
            if (id != 0) drop_item(u, id, 1);
        }
    }

    // Body fade: +0x30 = -0.1f, +0x38 = time, +0x34 = 0xff or the plane colour's alpha.
    void fade(void* body) {
        jo::view<u32>(jo::as_actor(body)->fade) = 0xbdcccccdu;
        jo::as_actor(body)->fade_38 = global<u32>(d::g_now);
        if (jo::self<jo::Unit>(this)->markers != -1) {
            jo::view<u32>(jo::as_actor(body)->alpha) = 0xff;
        } else {
            u32 tmp = 0;
            const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_plane(jo::as_actor(body)->plane)->colour(&tmp));  // 0x005d5eb0
            jo::view<u32>(jo::as_actor(body)->alpha) = colour >> 24;
        }
    }
    void fade_linked(i16 jo::Unit::*id_field) {
        if (joan::orig::as_units(kItems)->list.empty_at(joan::orig::arg::i32(static_cast<i32>(jo::self<jo::Unit>(this)->*id_field))) != 0) return;  // 0x00425d50
        u8* const it = item_get(static_cast<i32>(jo::self<jo::Unit>(this)->*id_field));
        if (jo::as_map_object(it)->actor == nullptr) return;
        if (static_cast<i32>(jo::self<jo::Unit>(this)->layer) == 0) return;
        jo::as_actor(jo::as_map_object(it)->actor)->fade_38 = global<u32>(d::g_now);
        jo::view<u32>(jo::as_actor(jo::as_map_object(it)->actor)->fade) = 0xbdcccccdu;
        if (jo::self<jo::Unit>(this)->markers != -1) {
            jo::view<u32>(jo::as_actor(jo::as_map_object(it)->actor)->alpha) = 0xff;
        } else {
            u32 tmp = 0;
            const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_plane(jo::as_actor(jo::as_map_object(it)->actor)->plane)->colour(&tmp));  // 0x005d5eb0
            jo::view<u32>(jo::as_actor(jo::as_map_object(it)->actor)->alpha) = colour >> 24;
        }
        if (jo::as_map_object(it)->actor != nullptr) joan::orig::as_actor(jo::as_map_object(it)->actor)->set_byte_fd(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x005ad000
    }

    // Morale change of `u` by +/- the scaled kill value.
    static void morale(u8* u, i32 value, bool add) {
        const double v = kFpu.mul(static_cast<double>(value), static_cast<double>(global<float>(d::g_a3cd08)));
        float d = 0.0f;
        if (!(v >= static_cast<double>(global<float>(d::flt_1)))) {  // fcomp; test ah,1 (less or NaN)
            d = 1.0f;
        } else {
            d = static_cast<float>(kFpu.mul(static_cast<double>(value), static_cast<double>(global<float>(d::g_a3cd08))));
        }
        const double m = static_cast<double>(static_cast<i32>(jo::as_unit(u)->stats[7]));
        const double r = add ? kFpu.add(m, static_cast<double>(d)) : kFpu.sub(m, static_cast<double>(d));
        joan::orig::as_unit(u)->roll_morale(joan::orig::arg::i32(ftol(r)));  // 0x004fe8c0
    }

    // ------------------------------------------------------------------ 0x007c6240
    void die(i32 killer_id) {
        c_die.hit();
        u8* const killer = unit_get(killer_id);
        u8* const cls = unit_class(this);
        if (mission_mode() == 2 && jo::self<jo::Unit>(this)->formation_slot == 1 && jo::as_unit(killer)->formation_slot != 0) return;
        if (static_cast<i32>(jo::as_unit(killer)->bribe_state) == 2 && static_cast<i32>(jo::self<jo::Unit>(this)->side) != 0) {
            if (stricmp_(nation_name(static_cast<i32>(jo::self<jo::Unit>(this)->side)), str(d::s_English)) == 0) inc_global(d::g_get_a591b1);  // "English"
        }
        if (jo::self<jo::Unit>(this)->rank > 3) {
            if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
                if (jo::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->grid_16 != nullptr) drop_carried(&jo::UnitData::grid_16);
                if (jo::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->grid_1a != nullptr) drop_carried(&jo::UnitData::grid_1a);
            }
            joan::orig::as_scenario(kMission)->add_copy(joan::orig::arg::mptr(this));  // 0x004cda10
        }
        // Rider link.
        if (unit_missing(jo::self<jo::Unit>(this)->weapon) == 0) {
            u8* const mount = unit_get(jo::self<jo::Unit>(this)->weapon);
            u8* const mcls = unit_class(mount);
            if (jo::self<jo::Unit>(this)->weapon_slot >= 0 && jo::self<jo::Unit>(this)->weapon_slot < jo::self<jo::UnitType>(mcls)->slot_count &&
                jo::as_arr_i32(jo::as_unit(mount)->mask_slots)[jo::self<jo::Unit>(this)->weapon_slot].value == static_cast<i32>(jo::self<jo::Unit>(this)->id)) {
                jo::view<u32>(jo::as_arr_i32(jo::as_unit(mount)->mask_slots)[jo::self<jo::Unit>(this)->weapon_slot].value) = 0;
                jo::view<u32>(jo::as_arr_i32(jo::as_unit(mount)->mask_slots_b)[jo::self<jo::Unit>(this)->weapon_slot].value) = 0;
            }
        }
        // Passengers / followers.
        for (i32 i = jo::self<jo::UnitType>(cls)->slot_count - 1; i >= 0; --i) {
            if (unit_missing(jo::as_arr_i32(jo::self<jo::Unit>(this)->mask_slots_b)[i].value) == 0) {
                u8* const u = unit_get(jo::as_arr_i32(jo::self<jo::Unit>(this)->mask_slots_b)[i].value);
                jo::view<u32>(jo::as_unit(u)->slot_index) = 0xffffffffu;
                jo::view<u32>(jo::as_unit(u)->weapon_slot) = 0;
                jo::view<u32>(jo::as_unit(u)->weapon) = 0;
                release(u, u, cls, i, killer_id);
                jo::view<u32>(jo::as_arr_i32(jo::self<jo::Unit>(this)->mask_slots_b)[i].value) = 0;
            }
            if (unit_missing(jo::as_arr_i32(jo::self<jo::Unit>(this)->mask_slots)[i].value) == 0) {
                u8* const u = unit_get(jo::as_arr_i32(jo::self<jo::Unit>(this)->mask_slots)[i].value);
                jo::view<u32>(jo::as_unit(u)->weapon) = 0;
                jo::view<u32>(jo::as_unit(u)->weapon_slot) = 0;
                jo::view<u32>(jo::as_unit(u)->slot_index) = 0xffffffffu;
                release(u, killer, cls, i, killer_id);
                jo::view<u32>(jo::as_arr_i32(jo::self<jo::Unit>(this)->mask_slots_b)[i].value) = 0;  // sic: +0x37c
            }
        }
        // +0x1f9 link.
        if (unit_missing(jo::self<jo::Unit>(this)->rider_ref) == 0) {
            u8* const u = unit_get(jo::self<jo::Unit>(this)->rider_ref);
            if (jo::as_unit(u)->mount_ref == static_cast<i32>(jo::self<jo::Unit>(this)->id)) {
                jo::view<u32>(jo::as_unit(u)->mount_ref) = 0;
                jo::view<u32>(jo::as_unit(u)->state_120) = 0;
                joan::orig::as_unit(u)->set_value_135(0);  // 0x004fe510
                die_(u, killer_id);
                jo::view<u32>(jo::self<jo::Unit>(this)->rider_ref) = 0;
            }
        }
        // Killer experience and counters.
        if (jo::self<jo::Unit>(this)->rank < 3) {
            joan::orig::as_unit(killer)->set_145(joan::orig::arg::i32(add32(jo::as_unit(killer)->mana, global<i32>(d::g_a3cc60))));  // 0x004fe5d0
        } else if (jo::self<jo::Unit>(this)->rank >= 4) {
            joan::orig::as_unit(killer)->set_145(joan::orig::arg::i32(add32(jo::as_unit(killer)->mana, global<i32>(d::g_a3cc50))));  // 0x004fe5d0
        } else {
            joan::orig::as_unit(killer)->set_145(joan::orig::arg::i32(add32(jo::as_unit(killer)->mana, global<i32>(d::g_a3cc48))));  // 0x004fe5d0
        }
        jo::as_unit(killer)->kill_points = static_cast<u16>(jo::as_unit(killer)->kill_points + (jo::self<jo::Unit>(this)->rank < 3 ? 1u : 10u));
        jo::view<u16>(jo::as_unit(killer)->kills) = static_cast<u16>(jo::view<u16>(jo::as_unit(killer)->kills) + 1u);
        if (jo::self<jo::Unit>(this)->rank >= 3) jo::view<u16>(jo::as_unit(killer)->kills_high_rank) = static_cast<u16>(jo::view<u16>(jo::as_unit(killer)->kills_high_rank) + 1u);
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(killer)->leads()) == 0) {           // 0x0044a660
            u8* const fm = joan::orig::arg::as<u8*>(joan::orig::as_unit(killer)->leader());  // 0x004216d0
            if (fm != nullptr) jo::view<u16>(jo::as_unit(fm)->squad_kills) = static_cast<u16>(jo::view<u16>(jo::as_unit(fm)->squad_kills) + 1u);
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->leads()) == 0) {           // 0x0044a660
            u8* const fm = joan::orig::arg::as<u8*>(joan::orig::as_unit(this)->leader());  // 0x004216d0
            if (fm != nullptr) jo::view<u16>(jo::as_unit(fm)->squad_losses) = static_cast<u16>(jo::view<u16>(jo::as_unit(fm)->squad_losses) + 1u);
        }
        // Experience scale.
        float scale = 1.0f;
        if (jo::as_unit(killer)->crit_ready != 0) scale = global<float>(d::g_a3cbd4);
        if (static_cast<i32>(jo::as_unit(killer)->level) > static_cast<i32>(jo::self<jo::Unit>(this)->level)) {
            const i32 n = static_cast<i32>(jo::as_unit(killer)->level) - static_cast<i32>(jo::self<jo::Unit>(this)->level);
            const i32 p = static_cast<i32>(1u << (static_cast<u32>(n) & 31u));
            const double q = kFpu.div(static_cast<double>(global<float>(d::flt_1)), static_cast<double>(p));
            scale = static_cast<float>(kFpu.mul(q, static_cast<double>(scale)));
        }
        if (static_cast<i32>(jo::as_unit(killer)->id) == global<i32>(d::g_player_unit_recno) && global<i32>(d::g_control_mode) != 4) {
            i32 lv = static_cast<i32>(global<i16>(d::g_kill_combo)) / global<i32>(d::g_a3cccc);
            if (lv > global<i32>(d::g_a3cc3c)) lv = global<i32>(d::g_a3cc3c);
            double a = kFpu.mul(static_cast<double>(global<i32>(d::g_a3ccb4)), static_cast<double>(global<float>(d::flt_0_01)));
            a = kFpu.mul(a, static_cast<double>(lv));
            a = kFpu.add(a, static_cast<double>(global<float>(d::flt_1)));
            a = kFpu.mul(a, static_cast<double>(scale));
            scale = static_cast<float>(a);
        }
        float xp = 0.0f;
        {
            double a = kFpu.mul(static_cast<double>(jo::self<jo::UnitType>(cls)->xp_value), static_cast<double>(static_cast<i32>(jo::self<jo::Unit>(this)->level)));
            a = kFpu.mul(a, static_cast<double>(scale));
            if (a > static_cast<double>(global<float>(d::flt_10))) {  // test ah,0x41 jne: <=, = or NaN skip
                double b = kFpu.mul(static_cast<double>(jo::self<jo::UnitType>(cls)->xp_value), static_cast<double>(static_cast<i32>(jo::self<jo::Unit>(this)->level)));
                b = kFpu.mul(b, static_cast<double>(scale));
                xp = static_cast<float>(b);
            } else {
                xp = 10.0f;
            }
        }
        joan::orig::as_unit(killer)->add_experience(joan::orig::arg::i32(ftol(static_cast<double>(xp))), static_cast<std::int8_t>(joan::orig::arg::i32(1)), static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x005229a0
        u8* const grp = joan::orig::arg::as<u8*>(joan::orig::as_unit(this)->nation());                     // 0x00509400
        if (grp != nullptr) {
            const i32 a = static_cast<i32>(0u - static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hit_points_max())));  // 0x00523320 (neg)
            const i32 b = joan::orig::arg::as<i32>(joan::orig::unit_class(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id))));  // 0x00527dd0
            joan::orig::as_nation(grp)->add_1389(joan::orig::arg::i32(imul(a, b)));                        // 0x004b5520
        }
        const i32 r1 = joan::orig::arg::as<i32>(joan::orig::unit_class(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id))));    // 0x00527dd0
        const i32 r2 = joan::orig::arg::as<i32>(joan::orig::character_class(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(killer)->type_id))));    // 0x00527e60
        const i32 value = add32(r1, r2);
        for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (u == nullptr) continue;
            if (static_cast<i32>(jo::as_unit(killer)->side) != 0 && static_cast<i32>(jo::as_unit(u)->side) == static_cast<i32>(jo::as_unit(killer)->side)) {
                if (static_cast<i32>(jo::as_unit(killer)->group_id) != 0 && static_cast<i32>(jo::as_unit(u)->group_id) == static_cast<i32>(jo::as_unit(killer)->group_id)) morale(u, value, true);
            } else if (static_cast<i32>(jo::self<jo::Unit>(this)->side) != 0 && static_cast<i32>(jo::as_unit(u)->side) == static_cast<i32>(jo::self<jo::Unit>(this)->side) && static_cast<i32>(jo::self<jo::Unit>(this)->group_id) != 0 &&
                       static_cast<i32>(jo::as_unit(u)->group_id) == static_cast<i32>(jo::self<jo::Unit>(this)->group_id)) {
                morale(u, value, false);
            }
        }
        // Heading of the throw.
        u32 heading = 0;
        joan::orig::as_angle(&heading)->construct();  // 0x00576120
        double h = static_cast<double>(joan::orig::arg::as<float>(joan::orig::as_ground_object(killer)->heading()));  // 0x004ff850
        h = kFpu.mul(h, static_cast<double>(global<float>(d::flt_180)));
        h = kFpu.div(h, global<double>(d::dbl_3_141592653589793));
        const double dir = joan::orig::arg::as<double>(joan::orig::as_unit(killer)->heading_to_unit(joan::orig::arg::i32(static_cast<i32>(jo::self<jo::Unit>(this)->id))));  // 0x00505960
        const float angle = static_cast<float>(kFpu.sub(h, dir));
        joan::orig::as_angle(&heading)->set_byte(joan::orig::arg::f32(angle));  // 0x005762c0
        i32 dist = 0xbb8;
        i32 height = 0x1f4;
        if (jo::as_unit(killer)->anim_set != nullptr) {
            u8* const w = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(killer)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit(killer)->anim_entry))));  // 0x004168d0
            const u32 wid = jo::view<u32>(jo::as_anim_group_rec(w)->model_ref);
            if (wid != 0) {
                dist = jo::view<i32>(jo::as_model_slot(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16(static_cast<i16>(wid))))->attack_force);    // 0x00416f20
                height = jo::view<i32>(jo::as_model_slot(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16(static_cast<i16>(wid))))->vertical_attack_force);  // 0x00416f20
            }
        }
        joan::orig::as_unit(this)->spawn_projectile();  // 0x0050afe0
        // Death voice.
        if (jo::self<jo::Unit>(this)->markers != 1 && jo::self<jo::Unit>(this)->markers != -2 && jo::self<jo::Unit>(this)->rank >= 4) {
            Str s;
            joan::orig::as_string(&s)->construct();                                          // 0x005a1170
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_player(obj(d::g_player_unit_recno))->name_tag(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->id))));  // 0x00453da0
            if (name != nullptr) {
                joan::orig::as_string(&s)->assign(str(d::s_RTS_OTHER));            // 0x005a1440 "RTS_OTHER"
                joan::orig::as_string(&s)->append(joan::orig::arg::str(name));     // 0x005a1550
                const i32 r = joan::orig::as_misc(obj(d::g_misc))->random(3);  // 0x00590540
                if (r == 0) {
                    joan::orig::as_string(&s)->append(str(d::s_010));                // "010"
                } else if (r == 1) {
                    joan::orig::as_string(&s)->append(str(d::s_011));                // "011"
                } else if (r == 2) {
                    joan::orig::as_string(&s)->append(str(d::s_012));                // "012"
                }
                const V3 pos = jo::view<V3>(jo::self<jo::Unit>(this)->position_);
                const char* const p = joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str());  // 0x00417d90
                joan::orig::as_sound_list(obj(d::g_sfx_player))->play_effect(p, f3(pos));          // 0x00494810
            }
        }
        if (static_cast<i32>(jo::self<jo::Unit>(this)->id) == global<i32>(d::g_player_unit_recno)) {
            const i32 t = static_cast<i32>(jo::self<jo::Unit>(this)->type_id);
            if (t != joan::orig::arg::as<i32>(joan::orig::as_unit_types(kClasses)->find(str(d::s_JOAN_OF_ARC)))) {  // 0x005270b0 "JOAN_OF_ARC"
                joan::orig::as_player(obj(d::g_player_unit_recno))->select_hero(0);                     // 0x00432300
            }
        }
        joan::orig::as_unit(this)->release_slot();  // 0x00528300
        if (jo::self<jo::Unit>(this)->markers == 1) {
            jo::view<u32>(jo::self<jo::Unit>(this)->hit_value) = 0x7fffffffu;
            joan::orig::as_unit(this)->set_value_135(1);  // 0x004fe510
            joan::orig::as_group(this)->release();     // 0x0042dd70
            Order rec;
            joan::orig::as_unit_task(&rec)->set(7u, static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->task.priority) + 1)), 0u, 0u, 0u, 0u);  // 0x004e4520
            joan::orig::as_unit(this)->set_task(&rec);                                                 // 0x00527fb0
            jo::view<u32>(jo::self<jo::Unit>(this)->markers) = 0xffffffffu;
            return;
        }
        if (static_cast<i32>(jo::self<jo::UnitType>(cls)->unit_class) == 8) {
            if (stricmp_(nation_name(static_cast<i32>(jo::self<jo::Unit>(this)->side)), str(d::s_French)) == 0) {  // "French"
                // The two `push 0` before 0x004dd980 (ret 4, one arg) are args 2-3 of 0x006e5700.
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_sms(obj(d::g_strings))->open_0f(joan::orig::arg::str(jo::self<jo::UnitType>(cls)->title_b4)));  // 0x004dd980
                joan::orig::as_sms(obj(d::g_sms))->add(joan::orig::arg::str(m), 0, 0);                                  // 0x006e5700
            }
            const i32 k = static_cast<i32>(jo::self<jo::UnitType>(cls)->kind);
            if (k == 4 || k == 5 || k == 6 || k == 9 || k == 0xa || k == 0xb || k == 0xc) {
                joan::orig::as_unit(this)->set_anim_set(1);  // 0x00511060
            }
        }
        V3f push{};
        {
            const float fd = static_cast<float>(dist);
            const float c = joan::orig::arg::as<float>(joan::orig::as_angle(&heading)->cos());  // 0x00576ac0
            push.x = static_cast<float>(kFpu.mul(static_cast<double>(c), static_cast<double>(fd)));
            const float fd2 = static_cast<float>(dist);
            const float s = joan::orig::arg::as<float>(joan::orig::as_angle(&heading)->sin());  // 0x005768d0
            push.y = static_cast<float>(kFpu.mul(static_cast<double>(s), static_cast<double>(fd2)));
            push.z = static_cast<float>(height);
        }
        if (static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 0x4e) joan::orig::vec_scale_in_place(&push, 3.0f);  // 0x0060ce80
        if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0 && static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) != 0x3b) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
            joan::orig::as_body(e)->init_target(&push, joan::orig::arg::u32(global<u32>(d::g_now)), 0x64u);                                  // 0x006b11a0
        }
        if (static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) != 0x3b) joan::orig::as_unit(this)->set_action(0x2a, 0, 1);  // 0x00501190
        if (jo::self<jo::Unit>(this)->markers != -2) {
            if (!(mission_mode() == 2 && jo::self<jo::Unit>(this)->formation_slot == 1) && jo::self<jo::Unit>(this)->actor != nullptr) {
                jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode = 0;
                if (static_cast<i32>(jo::self<jo::Unit>(this)->id) != global<i32>(d::g_player_unit_recno)) {
                    fade(jo::self<jo::Unit>(this)->actor);
                    if (jo::self<jo::Unit>(this)->model != nullptr) fade(jo::self<jo::Unit>(this)->model);
                }
            }
            fade_linked(&jo::Unit::carried_a);
            fade_linked(&jo::Unit::carried_b);
        }
        if (jo::self<jo::Unit>(this)->draw != nullptr) {
            auto* const p = jo::self<jo::Unit>(this)->draw;
            if (p != nullptr) joan::orig::as_unit_draw(p)->destroy(1u);  // 0x00416640 (deleting dtor)
            jo::self<jo::Unit>(this)->draw = nullptr;
        }
        if (jo::self<jo::Unit>(this)->model != nullptr) {
            if (jo::as_scene_object(jo::self<jo::Unit>(this)->model)->scene != nullptr) {
                joan::orig::as_scene(jo::as_scene_object(jo::self<jo::Unit>(this)->model)->scene)->detach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->model));  // 0x005efdb0
            }
            auto* const p = jo::self<jo::Unit>(this)->model;
            if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840 (deleting dtor)
            jo::self<jo::Unit>(this)->model = nullptr;
        }
        if (jo::self<jo::Unit>(this)->actor != nullptr && static_cast<i32>(jo::self<jo::Unit>(this)->layer) != 0) joan::orig::as_actor(jo::self<jo::Unit>(this)->actor)->set_byte_fd(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x005ad000
        if (static_cast<i32>(jo::self<jo::UnitType>(cls)->unit_class) == 8 && static_cast<i32>(jo::self<jo::UnitType>(cls)->kind) == 8 && jo::self<jo::Unit>(this)->siege_tower_state == 2) joan::orig::as_unit(this)->emit_6afb10(static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x00510220
        if (static_cast<i32>(jo::self<jo::UnitType>(cls)->unit_class) == 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hostile_to(joan::orig::arg::i16(static_cast<i16>(global<u16>(d::g_player_unit_recno))))) == 0) {  // 0x005094d0
                inc_global(d::g_get_a5918d);
                if (static_cast<i32>(jo::self<jo::UnitType>(cls)->gender) == 0x46) inc_global(d::g_get_a59199);
            }
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b114_14", name, addr, fn)
B("Unit::die_7c6240", 0x007c6240, &H::die);
#undef B

}  // namespace game_b114_14
}  // namespace

// ---------------------------------------- from modules/game_b114_15.cpp (game/b114_15)
namespace {
namespace game_b114_15 {

using joan::call::import_call;
template <class R, class... A>
R virtual_call(void* object, std::int32_t offset, A... args) {
    using Fn = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Fn);
    Fn method{};
    std::memcpy(&method, &joan::call::vtable_of(object)[joan::call::x86_vslot(offset)], sizeof(void*));
    return (reinterpret_cast<Obj*>(object)->*method)(args...);
}
float as_float(u32 bits) {
    float f = 0.0f;
    std::memcpy(&f, &bits, 4);
    return f;
}
u32 as_bits(float f) {
    u32 b = 0;
    std::memcpy(&b, &f, 4);
    return b;
}

static_assert(sizeof(V3) == 12, "vec3");
V3 g_vec() { return {global<u32>(d::g_pos), global<u32>(d::g_a69cdc), global<u32>(d::g_a69ce0)}; }

void* const kTypes = obj(d::g_types);
void* const kGroups = obj(d::g_items);
void* const kDefs = obj(d::g_item_res);
void* const kSquads = obj(d::g_physic_sys);
void* const kOrders = obj(d::g_orders);
void* const kPlayer = obj(d::g_player_unit_recno);

u32 now() { return global<u32>(d::g_now); }
float k(img::Addr a) { return global<float>(a); }
// fld x; fcomp 0.0; fnstsw; test ah,0x40; je skip -> runs when equal or unordered.
bool eqz(float d) {
    const float z = k(d::k_9cf5c4);
    return !(d < z) && !(d > z);
}
i32 group_missing(i32 id) { return joan::orig::as_units(kGroups)->list.empty_at(joan::orig::arg::i32(id)); }  // 0x00425d50
u8* group(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_units(kGroups)->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20
u8* def16(i16 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kDefs)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0

JOAN_COUNTER(c_behaviour, "Unit::type_behaviour_7c7ab0");

struct H {
    u8* p2f5() { return jo::view<u8*>(jo::self<jo::Unit>(this)->actor); }
    bool is_player() { return static_cast<i32>(jo::self<jo::Unit>(this)->id) == global<i32>(d::g_player_unit_recno); }
    u8* unit_type() { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id))); }  // 0x0042d0a0
    u8* leader() { return joan::orig::arg::as<u8*>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->slot_e()); }           // 0x00497580
    void stop() { joan::orig::as_unit(this)->clear_flags(); }                              // 0x0050d0d0
    void idle() { joan::orig::as_unit(this)->play_idle(i32{0}, i32{1}); }              // 0x00508b90
    void act(i32 a) { joan::orig::as_unit(this)->set_action(joan::orig::arg::i32(a), i32{0}, i32{1}); }       // 0x00501190
    // 0x00504010 -> 0x004ff9f0(state, [0xa500ba], -1)
    void restart() {
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_cell_object(this)->find_anim_state());       // 0x00504010
        joan::orig::as_cell_object(this)->set_anim_state(joan::orig::arg::i32(v), joan::orig::arg::i32(now()), i32{-1});  // 0x004ff9f0
    }
    void restart_clear() {
        restart();
        jo::self<jo::Unit>(this)->mount_busy = 0;
    }
    void mark_idle() {
        jo::view<u8>(jo::self<jo::Unit>(this)->combo_state) = 0xff;
        jo::self<jo::Unit>(this)->combo_time = now();
    }
    i8 mounted() { return joan::orig::arg::as<i8>(joan::orig::as_unit(this)->dead()); }  // 0x0042d0e0

    // ---------------------------------------------------------------- switch 1 cases
    // 79, 80 (0x7c7bc1)
    void dismount_to(float dist) {
        if (!eqz(dist)) return;
        void* const rec = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Unit>(this)->mount_id)));  // 0x0041b3c0
        const float r = joan::orig::arg::as<float>(joan::orig::as_ground_object(rec)->heading());                     // 0x004ff850
        joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(r));                                   // 0x004ff870
        joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(r / k(d::flt_0_017453292)));                   // 0x0050abd0
        jo::self<jo::Unit>(this)->mounted_flag = 1;
        if (global<i32>(d::g_control_mode) == 3) joan::orig::as_player(kPlayer)->set_follow(i32{1});  // 0x00436190
        if (jo::self<jo::Unit>(this)->model != nullptr) joan::orig::as_model_instance(jo::self<jo::Unit>(this)->model)->set_10a(joan::orig::arg::u32(i32{0}));    // 0x0042cf20
        idle();
        restart_clear();
    }
    // 81 (0x7c7c8e, add) / 82 (0x7c7f2f, sub)
    void mount_step(float dist, bool second) {
        if (!eqz(dist)) return;
        if (jo::view<u32>(jo::as_actor(p2f5())->record) != 0)
            joan::orig::as_players(kSquads)->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(p2f5())->record)), joan::orig::arg::i16(i32{0}));  // 0x006ad980
        V3 pos{0, 0, 0};
        joan::orig::as_unit(this)->position(&pos);  // 0x004fedb0
        V3 dir{0, 0, 0};
        dir.x = jo::view<u32>(jo::as_actor(p2f5())->bone->local[4]);
        dir.y = jo::view<u32>(jo::as_actor(p2f5())->bone->local[5]);
        dir.z = jo::view<u32>(jo::as_actor(p2f5())->bone->local[6]);
        V3 buf{0, 0, 0};
        const V3* const sc = joan::orig::arg::as<V3*>(joan::orig::vec_scale(&buf, &dir, joan::orig::arg::f32(u32{0x41f00000})));  // 0x0060cd00 (30.0f)
        V3 tmp = *sc;
        if (second)
            joan::orig::vec3_sub_assign(&pos, &tmp);  // 0x0060ce20
        else
            joan::orig::vec_add_in_place(&pos, &tmp);  // 0x0060cdc0
        joan::orig::as_ground_object(this)->set_position(f3(pos));         // 0x004fedf0
        float r = joan::orig::arg::as<float>(joan::orig::as_ground_object(this)->heading());  // 0x004ff850
        float t = k(d::g_pi_a3cb6c) / k(d::flt_2);
        if (second) t = t + k(d::g_pi_a3cb6c);
        r = r - t;
        joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(r));                  // 0x004ff870
        joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(r / k(d::flt_0_017453292)));  // 0x0050abd0
        if (static_cast<i32>(jo::self<jo::Unit>(this)->type_id) != 0) {
            u8* const ty = unit_type();
            jo::as_actor(p2f5())->aim_mode = jo::self<jo::UnitType>(ty)->shape_kind;
            const i32 kind = jo::view<i32>(jo::as_actor(p2f5())->aim_mode);
            if (kind == 1) {
                std::memcpy(jo::as_actor(p2f5())->aim_sphere, jo::self<jo::UnitType>(ty)->shape_sphere, 16);
            } else if (kind == 2) {
                std::memcpy(jo::as_actor(p2f5())->aim_box, jo::self<jo::UnitType>(ty)->shape_box, 24);
            } else if (kind == 3) {
                std::memcpy(&jo::as_actor(p2f5())->collide_kind, jo::self<jo::UnitType>(ty)->shape_cylinder, 20);  // collide_kind .. collide_height
            }
        }
        jo::self<jo::Unit>(this)->dismounted_flag = 1;
        if (global<i32>(d::g_control_mode) == 3) joan::orig::as_player(kPlayer)->set_follow(i32{1});  // 0x00436190
        if (jo::self<jo::Unit>(this)->model != nullptr) joan::orig::as_model_instance(jo::self<jo::Unit>(this)->model)->set_10a(joan::orig::arg::u32(i32{1}));    // 0x0042cf20
        if (static_cast<i32>(mounted()) != 0)
            act(0x2a);
        else
            idle();
        restart_clear();
        jo::self<jo::Unit>(this)->u8_202 = 1;
    }
    // 1 (0x7c8223): the idle roll - now and then an idle unit plays IDLE2 or IDLE3.
    void random_attack(float dist) {
        float r = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->rand_unit_inclusive());                            // 0x00590660
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744())) != 0) r = r * k(d::flt_4);  // 0x004301c0
        if (dist > k(d::flt_100)) return;
        if (jo::self<jo::Unit>(this)->anim_set == nullptr) return;
        if (static_cast<i32>(jo::self<jo::Unit>(this)->action) != 0) return;
        if (r > idle_chance()) return;
        const float r2 = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->rand_unit_inclusive());  // 0x00590660
        const i32 a = joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(i32{2}));  // 0x00416dc0
        const i32 b = joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(i32{3}));
        const bool lt = !(r2 >= k(d::flt_0_5));  // less or unordered
        if (lt && a != 0) {
            act(2);
            restart_clear();
        } else if (!lt && b != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(this)->get_1e6()) == 0) {  // 0x004f2170
            act(3);
            restart_clear();
        } else if (a != 0) {
            act(2);
            restart_clear();
        } else if (b != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(this)->get_1e6()) == 0) {
            act(3);
            restart_clear();
        }
    }
    // The chance per call of the idle roll above. The original compares its roll with g_a3ccf0 once per update, so
    // at a high frame rate units played IDLE2 / IDLE3 (and their voice lines) far more often. [Fixes] IdleRate: the
    // chance per update follows the time since the last frame, so on average they come as at kIdleRefMs (60 fps)
    // whatever the frame rate. The roll itself is drawn as before (the game's random sequence is unchanged).
    static float idle_chance() {
        const float chance = k(d::g_a3ccf0);
        if (!joan::hook::settings().fix_idle_rate) return chance;
        constexpr double kIdleRefMs = 1000.0 / 60.0;
        static u32 frame = 0, prev = 0;  // g_now of this frame and of the previous one
        const u32 now = global<u32>(d::g_now);
        if (now != frame) {
            prev = frame;
            frame = now;
        }
        double dt = static_cast<double>(frame - prev);
        if (prev == 0 || dt <= 0.0 || dt > 250.0) dt = kIdleRefMs;
        if (!(chance > 0.0f) || !(chance < 1.0f)) return chance;
        return static_cast<float>(1.0 - std::pow(1.0 - static_cast<double>(chance), dt / kIdleRefMs));
    }
    // 62, 63 (0x7c846a)
    void squad_reset(float dist) {
        if (!eqz(dist)) return;
        if (jo::self<jo::Unit>(this)->actor != nullptr) {
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_players(kSquads)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(p2f5())->record))));  // 0x006af310
            jo::as_shot(p)->state.vec0_x = global<u32>(d::g_pos);
            jo::view<u32>(jo::as_shot(p)->state.vec0_y) = global<u32>(d::g_a69cdc);
            jo::view<u32>(jo::as_shot(p)->state.vec0_z) = global<u32>(d::g_a69ce0);
            jo::as_shot(p)->state.vel_len = 0xbf800000u;  // -1.0f
        }
        idle();
        restart_clear();
    }
    // 0x00431480 on 0xa48500 (id, a0, V3 +0x365, V3 b), ret 0x20
    void order_431480(i16 a0, F3 b) {
        const V3 v = jo::view<V3>(jo::self<jo::Unit>(this)->look_dir);
        joan::orig::as_throw_items(kOrders)->apply_kind(static_cast<i32>(jo::self<jo::Unit>(this)->id), joan::orig::arg::u32(static_cast<i32>(a0)), f3(v), b);
    }
    // 51, 56 (0x7c851c)
    void attack_group(float dist) {
        i32 range = 0x28a;
        u8* const ty = unit_type();
        if (static_cast<i32>(jo::self<jo::UnitType>(ty)->kind) == 10) {
            range = 0;
        } else if (static_cast<i32>(jo::self<jo::UnitType>(ty)->kind) == 4 || static_cast<i32>(jo::self<jo::UnitType>(ty)->kind) == 9) {
            range = 0x683;
        } else if (static_cast<i32>(jo::self<jo::UnitType>(ty)->kind) == 5) {
            range = 0x683;
        } else if (global<i32>(d::g_player_unit_recno) != static_cast<i32>(jo::self<jo::Unit>(this)->id) &&
                   group_missing(static_cast<i32>(jo::self<jo::Unit>(this)->carried_b)) == 0 &&
                   static_cast<i32>(jo::as_map_object(group(static_cast<i32>(jo::self<jo::Unit>(this)->carried_b)))->item_type) == 0xc) {
            range = 0x514;
        }
        // fild range; fcomp dist; test ah,1; jne skip -> runs when range >= dist.
        if (group_missing(static_cast<i32>(jo::self<jo::Unit>(this)->carried_a)) == 0 &&
            static_cast<double>(range) >= static_cast<double>(dist)) {
            if (!is_player()) joan::orig::as_unit(this)->update_look_dir();  // 0x0050aca0
            const i16 a0 = jo::self<jo::Unit>(this)->carried_a;
            if (is_player()) {
                u8* const l = leader();
                u8* const d = def16(jo::view<i16>(jo::as_held_item(l)->item));
                joan::orig::as_list(kOrders)->add_new_6c(static_cast<void*>(this), joan::orig::arg::i16(jo::view<i16>(jo::as_item_def(d)->type_id)));  // 0x004ea700
            } else {
                u8* const g = group(static_cast<i32>(a0));
                joan::orig::as_list(kOrders)->add_new_6c(static_cast<void*>(this), joan::orig::arg::i16(jo::as_map_object(g)->item_type));
            }
            if (is_player()) {
                order_431480(a0, f3(g_vec()));
            } else if (group_missing(static_cast<i32>(jo::self<jo::Unit>(this)->carried_b)) == 0) {
                if (static_cast<i32>(jo::as_map_object(group(static_cast<i32>(jo::self<jo::Unit>(this)->carried_b)))->item_type) == 0xc) {
                    order_431480(a0, f3(V3{0x40aac8b4u, 0u, 0x40f00000u}));
                } else {
                    order_431480(a0, f3(g_vec()));
                }
            } else {
                order_431480(a0, f3(g_vec()));
            }
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(i32{0}), i32{0}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::mptr(i32{0}));  // 0x0050a480
            if (group_missing(static_cast<i32>(a0)) == 0)
                joan::orig::as_units(kGroups)->remove_49f370(static_cast<i32>(a0));  // 0x0049f370
        }
        if (eqz(dist)) {
            stop();
            if (static_cast<i32>(jo::self<jo::UnitType>(unit_type())->kind) == 0xa) {
                act(0x34);
            } else {
                idle();
                restart();
            }
        }
    }
    i32 leader_def_a() { return static_cast<i32>(jo::view<i16>(jo::as_item_def(def16(jo::view<i16>(jo::as_held_item(leader())->item)))->bone)); }
    void* skeleton() { return jo::view<void*>(jo::as_actor(p2f5())->def->model->bones); }
    // bone transform + 0x004945c0 + normalize (cases 50 / 48)
    void aim_at_target(V3* out, F3 ptf, i32 bone_word, i32 kind) {
        V3 pt = v3_of(ptf);
        const i32 bi = joan::orig::arg::as<i32>(joan::orig::as_bone_table(skeleton())->find(bone_word));       // 0x005ba160
        u8* const bone = joan::orig::arg::as<u8*>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(p2f5())->plane))->bone_5cf8f0(joan::orig::arg::i32(bi)));  // 0x005cf8f0
        import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&pt), static_cast<void*>(jo::as_bone(bone)->matrix_));  // point_transform
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(jo::self<jo::Unit>(this)->look_a)));  // 0x0041b3c0
        const V3 rp = jo::view<V3>(jo::as_unit(rec)->position_);
        joan::orig::spawn_effect_kind(joan::orig::arg::mptr(out), f3(pt), f3(rp), joan::orig::arg::u32(kind));           // 0x004945c0
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(out));  // vector_normalize
    }
    void act_by_pitch(const V3& l) {
        if (!(as_float(l.z) >= k(d::flt_0_1)))  // less or unordered
            act(0x36);
        else
            act(0x39);
    }
    // 50 (0x7c89ef). Returns true -> 0x7c9351.
    bool aim_50(float dist) {
        V3 l = g_vec();
        if (!is_player() && joan::orig::as_dynarray(kUnits)->empty_at(static_cast<i32>(jo::self<jo::Unit>(this)->look_a)) == 0 &&  // 0x00428b70
            group_missing(static_cast<i32>(jo::self<jo::Unit>(this)->carried_a)) == 0) {
            const V3 pt = g_vec();  // [ebp-0xc4]
            i32 bw = jo::view<i32[3]>(jo::as_map_object(group(static_cast<i32>(jo::self<jo::Unit>(this)->carried_a)))->sound_slots)[2];  // +0x9e: the item's bone word (sound_slots[4..5])
            if (bw == 0) bw = leader_def_a();
            aim_at_target(&l, f3(pt), bw, 0);
        }
        if (eqz(dist)) {
            stop();
            act_by_pitch(l);
            return false;
        }
        return true;
    }
    // 0x7c8de1 / 0x7c8ff1: leader definition switch (+0x329)
    void swap_leader_def() {
        if (static_cast<i32>(jo::view<i16>(jo::as_held_item(leader())->item)) != jo::view<i32>(jo::self<jo::Unit>(this)->item_ref)) {
            joan::orig::as_item_def(def16(jo::view<i16>(jo::as_held_item(leader())->item)))->acquire_anim(i32{1}, i32{0});  // 0x004a39f0
            joan::orig::as_item_def(def16(jo::view<i16>(jo::as_held_item(leader())->item)))->acquire_anim(i32{2}, i32{0});
            if (jo::self<jo::Unit>(this)->item_ref != 0) {
                joan::orig::as_res(def16(jo::view<i16[2]>(jo::self<jo::Unit>(this)->item_ref)[0]))->release_slot(i32{1}, i32{0});  // 0x004a3e30
                joan::orig::as_res(def16(jo::view<i16[2]>(jo::self<jo::Unit>(this)->item_ref)[0]))->release_slot(i32{2}, i32{0});
            }
            jo::view<i32>(jo::self<jo::Unit>(this)->item_ref) = static_cast<i32>(jo::view<i16>(jo::as_held_item(leader())->item));
        }
    }
    // 0x7c8eac / 0x7c90bc: group of the leader, virtual +0x18(1), 0x0050a480, +0x9e = e0
    i32 regroup(i32 e0) {
        const i32 g = joan::orig::arg::as<i32>(joan::orig::as_units(kGroups)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_held_item(leader())->item)))));  // 0x0049ea90
        virtual_call<u32>(group(g), 0x18, i32{1});
        const i32 w = static_cast<i32>(jo::view<i16>(jo::as_item_def(def16(jo::as_map_object(group(g))->type_id))->bone));
        joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(g))), joan::orig::arg::i32(w), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::mptr(i32{0}));  // 0x0050a480
        jo::view<i32[3]>(jo::as_map_object(group(g))->sound_slots)[2] = e0;  // +0x9e (see aim_50)
        return g;
    }
    // 48 (0x7c8bef). Returns true -> 0x7c9351.
    bool aim_48(float dist) {
        V3 l = g_vec();
        if (!is_player() && joan::orig::as_dynarray(kUnits)->empty_at(static_cast<i32>(jo::self<jo::Unit>(this)->look_a)) == 0) {
            const V3 pt = g_vec();  // [ebp-0xec]
            (void)leader_def_a();   // [ebp-0xe0], rewritten before any read
            const i32 bw = leader_def_a();
            aim_at_target(&l, f3(pt), bw, 1);
        }
        if (!is_player()) {
            if (!(dist > k(d::flt_850)) && group_missing(static_cast<i32>(jo::self<jo::Unit>(this)->carried_a)) != 0) {
                const i32 e0 = leader_def_a();
                swap_leader_def();
                regroup(e0);
            }
        } else if (!(dist > k(d::k_9cf5c4))) {
            bool go = true;
            if (group_missing(static_cast<i32>(jo::self<jo::Unit>(this)->carried_a)) == 0) {
                const i32 a = static_cast<i32>(jo::as_map_object(group(static_cast<i32>(jo::self<jo::Unit>(this)->carried_a)))->type_id);
                if (a == static_cast<i32>(jo::view<i16>(jo::as_held_item(leader())->item))) go = false;
            }
            if (go) {
                joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(i32{0}), i32{0}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::mptr(i32{0}));  // 0x0050a480
                const i32 e0 = leader_def_a();
                swap_leader_def();
                const i32 g = regroup(e0);
                if (jo::as_map_object(group(g))->actor != nullptr)
                    joan::orig::as_model_instance(jo::view<void*>(jo::as_map_object(group(g))->actor))->set_10a(joan::orig::arg::u32(i32{0}));  // 0x0042cf20
            }
        }
        if (!eqz(dist)) return true;
        if (group_missing(static_cast<i32>(jo::self<jo::Unit>(this)->carried_a)) != 0) return false;
        if (is_player()) {
            stop();
            return false;
        }
        if (group_missing(static_cast<i32>(jo::self<jo::Unit>(this)->carried_b)) == 0) {
            if (static_cast<i32>(jo::as_map_object(group(static_cast<i32>(jo::self<jo::Unit>(this)->carried_b)))->item_type) == 0xc) {
                virtual_call<u32>(group(static_cast<i32>(jo::self<jo::Unit>(this)->carried_a)), 0x18, i32{3});
                joan::orig::as_player(this)->set_unit_state(joan::orig::arg::i16(jo::self<jo::Unit>(this)->carried_a), static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})));  // 0x0050a9e0
                act(0x32);
            } else {
                stop();
                act_by_pitch(l);
            }
        } else {
            stop();
            act_by_pitch(l);
        }
        return false;
    }
    // Returns true when the original continues at 0x7c9351.
    bool switch1(float dist) {
        switch (static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind)) {
            case 43:
            case 45:
                return false;
            case 44:
                if (eqz(dist)) {
                    stop();
                    idle();
                }
                return true;
            case 14: case 15: case 16: case 17: case 18:
            case 19: case 20: case 21: case 22: case 23:
                if (jo::view<u32>(jo::self<jo::Unit>(this)->rider_id) == 0 && eqz(dist)) {
                    stop();
                    idle();
                }
                return true;
            case 79:
            case 80:
                dismount_to(dist);
                return true;
            case 81:
                mount_step(dist, false);
                return true;
            case 82:
                mount_step(dist, true);
                return true;
            case 71:
            case 72:
                if (eqz(dist)) {
                    act(0x43);
                    restart_clear();
                }
                return true;
            case 1:
                random_attack(dist);
                return true;
            case 2:
            case 3:
                if (eqz(dist)) {
                    act(1);
                    restart_clear();
                }
                return true;
            case 62:
            case 63:
                squad_reset(dist);
                return true;
            case 51:
            case 56:
                attack_group(dist);
                return true;
            case 52:
                if (eqz(dist)) {
                    stop();
                    idle();
                    restart();
                }
                return true;
            case 54:
                if (eqz(dist)) {
                    stop();
                    if (!is_player()) act(0x33);
                    return false;
                }
                return true;
            case 57:
                if (eqz(dist)) {
                    stop();
                    if (!is_player()) act(0x38);
                    return false;
                }
                return true;
            case 50:
                return aim_50(dist);
            case 48:
                return aim_48(dist);
            case 26:
                if (static_cast<i32>(jo::self<jo::Unit>(this)->action) == 1) {
                    if (eqz(dist)) {
                        stop();
                        act(5);
                    }
                    return false;
                }
                return true;
            case 24:
            case 25:
                if (eqz(dist)) {
                    stop();
                    idle();
                }
                return false;
            default:
                return true;
        }
    }

    // ---------------------------------------------------------------- switch 2 / 3 helpers
    // 30 / 31 / 32: skill a (0x1f / 0x20 / 0x21) when available, else idle. dist == 0 checked by caller.
    void use_skill(i32 a) {
        if (jo::self<jo::Unit>(this)->anim_set != nullptr) {
            const i32 s = joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(joan::orig::arg::i32(a)));  // 0x00416dc0
            if (s != 0) {
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(s)));  // 0x004168d0
                if (joan::orig::arg::as<i32>(joan::orig::as_anim_set_entry(e)->is_yes()) != 0 ||                          // 0x00416ed0
                    joan::orig::arg::as<i32>(joan::orig::as_unit(this)->find_link(i32{0xd}, joan::orig::arg::str(i32{0}))) >= 1) {     // 0x0050d3d0
                    act(a);
                    return;
                }
            }
        }
        mark_idle();
        idle();
    }
    // 58 (0x7c96ae)
    void turn_58(float dist) {
        if (!eqz(dist)) return;
        jo::view<u8>(jo::self<jo::Unit>(this)->combo_state) = 0xff;
        const float r = joan::orig::arg::as<float>(joan::orig::as_ground_object(this)->heading());  // 0x004ff850
        const float deg = r / k(d::flt_0_017453292) + k(d::flt_180);
        if (deg > k(d::flt_360)) {
            joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(r + k(d::flt_3_1415927) - k(d::flt_6_2831855)));                     // 0x004ff870
            joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(r / k(d::flt_0_017453292) + k(d::flt_180) - k(d::flt_360)));     // 0x0050abd0
        } else {
            joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(r + k(d::flt_3_1415927)));
            joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(r / k(d::flt_0_017453292) + k(d::flt_180)));
        }
    }
    void after_dismount_timer() {  // 0x7c97af / 0x7ca9e2 body after the timer test
        joan::orig::as_unit(this)->set_morale(i32{0x64});  // 0x004fea80
        if (static_cast<i32>(mounted()) != 0) {
            jo::self<jo::Unit>(this)->prev_action = jo::self<jo::Unit>(this)->anim_kind;
            jo::self<jo::Unit>(this)->anim_kind = 0x2a;
            jo::self<jo::Unit>(this)->mount_busy = 0;
        } else {
            act(0x3c);
            jo::self<jo::Unit>(this)->downed_hit = 0;
        }
    }
    // Slide cases 91..100: z moves by k / duration * [0xa500b6], clamped to +0x20b -/+ k.
    V3 slide(bool up, u32 k0, u32 k_lahire, u32 k_alencon, u32 k_metz) {
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry))));          // 0x004168d0
        void* const anim_slot = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_anim_group_rec(e)->anim_id))));  // 0x004174f0
        void* const anim = jo::as_anim_slot(anim_slot)->anim;  // x64: the slot's first field is the AnimDef* (8 bytes)

        V3 pos{jo::view<u32[3]>(jo::self<jo::Unit>(this)->position_)[0], jo::view<u32[3]>(jo::self<jo::Unit>(this)->position_)[1], jo::view<u32[3]>(jo::self<jo::Unit>(this)->position_)[2]};
        u32 kb = k0;
        if (std::strcmp(joan::orig::arg::str(reinterpret_cast<const char*>(unit_type() + 4)), str(d::s_LAHIRE_9cfb3c)) == 0) {
            kb = k_lahire;  // "LAHIRE"
        } else if (std::strcmp(joan::orig::arg::str(reinterpret_cast<const char*>(unit_type() + 4)), str(d::s_ALENCON_9cfb30)) == 0) {
            kb = k_alencon;  // "ALENCON"
        } else if (std::strcmp(joan::orig::arg::str(reinterpret_cast<const char*>(unit_type() + 4)), str(d::s_JEAN_DE_METZ)) == 0) {
            kb = k_metz;  // "JEAN_DE_METZ"
        }
        void* const fr = joan::orig::arg::as<void*>(joan::orig::as_anim_set(anim)->frame(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(p2f5())->action)), joan::orig::arg::u32(i32{0})));  // 0x005b5dd0
        const float dur = joan::orig::arg::as<float>(joan::orig::as_range(fr)->span());                                   // 0x0042d130
        const float kk = as_float(kb);
        const Fpu fpu{true};
        const double t = fpu.mul(fpu.div(kk, dur), static_cast<double>(global<i32>(d::g_frame_elapsed_ms)));
        float z = as_float(pos.z);
        const float h = jo::self<jo::Unit>(this)->height_point[2];
        if (!up) {
            z = static_cast<float>(fpu.sub(z, t));
            const float d = h - z;
            if (d > kk) z = h - kk;
        } else {
            z = static_cast<float>(fpu.add(t, z));
            const float d = z - h;
            if (d > kk) z = kk + h;
        }
        pos.z = as_bits(z);
        joan::orig::as_ground_object(this)->set_position(f3(pos));  // 0x004fedf0
        return pos;
    }
    void slide_idle(float dist, bool up, u32 k0, u32 a, u32 b, u32 c) {
        slide(up, k0, a, b, c);
        if (eqz(dist)) {
            stop();
            idle();
        }
    }
    void slide_land(float dist, u32 k0, u32 a, u32 b, u32 c) {
        const V3 pos = slide(true, k0, a, b, c);
        if (eqz(dist)) {
            act(0x65);
            jo::view<u32>(jo::self<jo::Unit>(this)->height_point[0]) = pos.x;
            jo::view<u32>(jo::self<jo::Unit>(this)->height_point[1]) = pos.y;
            jo::view<u32>(jo::self<jo::Unit>(this)->height_point[2]) = pos.z;
        }
    }
    i8 parity_96() {
        u8* const g = group(jo::self<jo::Unit>(this)->held_item_id);
        u8* const d = def16(jo::as_map_object(g)->type_id);
        const i32 q = static_cast<i32>(jo::view<i16>(jo::as_item_def(d)->stats[1])) / 22;
        return static_cast<i8>(q % 2);
    }
    void release_101() {
        stop();
        joan::orig::as_unit(this)->hero_special(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->held_item_id)), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{1}));  // 0x004fab10
        idle();
        restart_clear();
    }
    void release_92() {
        stop();
        joan::orig::as_unit(this)->hero_special(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->held_item_id)), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}));  // 0x004fab10
        idle();
    }
    // Player's own unit: switch 2. Returns true -> tail 0x7ca795.
    bool switch2(float dist) {
        switch (static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind)) {
            case 28:
            case 27: case 29: case 34:
                if (eqz(dist)) mark_idle();
                return true;
            case 30:
                if (eqz(dist)) use_skill(0x1f);
                return true;
            case 31:
                if (eqz(dist)) use_skill(0x20);
                return true;
            case 32:
                if (eqz(dist)) use_skill(0x21);
                return true;
            case 33:
            case 83:
                if (eqz(dist)) {
                    mark_idle();
                    idle();
                }
                return true;
            case 58:
                turn_58(dist);
                return true;
            case 41: case 64: case 65: case 66: case 78:
                jo::view<u32>(jo::self<jo::Unit>(this)->fell_timer) = jo::view<u32>(jo::self<jo::Unit>(this)->fell_timer) - global<u32>(d::g_frame_step_ms);
                if (eqz(dist) && static_cast<i32>(mounted()) == 0) after_dismount_timer();
                return false;
            case 91:
                slide_idle(dist, false, 0x41affbe7u, 0x41b36873u, 0x41bb9db2u, 0x41affdf4u);
                return true;
            case 93:
                slide_idle(dist, false, 0x41afed91u, 0x41acb646u, 0x41a4624eu, 0x41affdf4u);
                return true;
            case 94:
                slide_idle(dist, false, 0x412ab021u, 0x40b24dd3u, 0x3fbf9db2u, 0x3fd1a9fcu);
                return true;
            case 95:
                slide_idle(dist, false, 0x412ab021u, 0x40be45a2u, 0x403cbc6au, 0x3fd1a9fcu);
                return true;
            case 97:
                slide_idle(dist, true, 0x41afed91u, 0x41acb646u, 0x41a4624eu, 0x41affdf4u);
                return true;
            case 99:
                slide_idle(dist, true, 0x41affbe7u, 0x41b36873u, 0x41bb9db2u, 0x41affdf4u);
                return true;
            case 102:
                if (eqz(dist)) {
                    stop();
                    idle();
                }
                return true;
            case 96: {
                const i8 p = parity_96();
                if (eqz(dist)) {
                    stop();
                    act(static_cast<i32>(p) != 0 ? 0x5f : 0x5e);
                }
                return true;
            }
            case 98:
                slide_land(dist, 0x412ab021u, 0x40b24dd3u, 0x3fbf9db2u, 0x3fd1a9fcu);
                return true;
            case 100:
                slide_land(dist, 0x412ab021u, 0x40be45a2u, 0x403cbc6au, 0x3fd1a9fcu);
                return true;
            case 101:
                if (eqz(dist)) release_101();
                return true;
            case 92:
                if (eqz(dist)) release_92();
                return true;
            default:
                return true;
        }
    }
    // 0x7ca795
    void tail(float dist) {
        const i32 t = static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind);
        if (t == 5 || static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 6 || static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 0x19 ||
            static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 8 || static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 0x1f ||
            static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 0x20 || static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 0x21)
            return;
        if (jo::view<u32>(jo::self<jo::Unit>(this)->held_item_id) != 0) return;
        if (!eqz(dist)) return;
        const i16 saved = jo::self<jo::Unit>(this)->anim_kind;
        idle();
        if (static_cast<i32>(saved) == 0x3a) restart_clear();
    }
    // Other units: switch 3 (0x7ca88f).
    void switch3(float dist) {
        switch (static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind)) {
            case 37: case 38: case 39: case 40: case 60: case 74: case 75: case 76: case 77:
            case 91: case 93: case 94: case 95: case 97: case 99: case 102:
                if (eqz(dist)) {
                    stop();
                    idle();
                }
                return;
            case 35:
            case 36:
                if (eqz(dist)) {
                    if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(i32{0x25})) != 0) {  // 0x00416dc0
                        act(0x25);
                    } else {
                        stop();
                        idle();
                    }
                }
                return;
            case 59:
                if (eqz(dist)) {
                    stop();
                    if (static_cast<i32>(mounted()) == 0) {
                        jo::as_actor(p2f5())->aim_mode = 3;
                        joan::orig::as_unit(this)->set_morale(i32{0x64});  // 0x004fea80
                        act(0x3c);
                    }
                }
                return;
            case 41: case 64: case 65: case 66: case 78:
                if (eqz(dist)) {
                    stop();
                    jo::view<u32>(jo::self<jo::Unit>(this)->fell_timer) = jo::view<u32>(jo::self<jo::Unit>(this)->fell_timer) - global<u32>(d::g_frame_step_ms);
                    if (jo::self<jo::Unit>(this)->fell_timer < 0) after_dismount_timer();
                }
                return;
            case 98:
            case 100:
                if (eqz(dist)) {
                    stop();
                    act(0x65);
                }
                return;
            case 96:
                if (eqz(dist)) {
                    stop();
                    act(0x5e);
                }
                return;
            case 101:
                if (eqz(dist)) release_101();
                return;
            case 92:
                if (eqz(dist)) release_92();
                return;
            case 30:
                if (eqz(dist)) use_skill(0x1f);
                return;
            case 31:
                if (eqz(dist)) use_skill(0x20);
                return;
            case 32:
                if (eqz(dist)) use_skill(0x21);
                return;
            case 33: case 34: case 83:
                if (eqz(dist)) {
                    mark_idle();
                    idle();
                }
                return;
            default:
                return;
        }
    }

    void type_behaviour() {
        c_behaviour.hit();
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Unit>(this)->mount_busy)) != 0) {
            const u32 v = joan::orig::arg::as<u32>(joan::orig::as_cell_object(this)->find_anim_state());  // 0x00504010
            if (v == 0) return;
            joan::orig::as_cell_object(this)->set_anim_state(joan::orig::arg::i32(v), joan::orig::arg::i32(now()), i32{-1});  // 0x004ff9f0
            jo::self<jo::Unit>(this)->mount_busy = 0;
            return;
        }
        joan::orig::as_cell_object(this)->find_anim_state();                         // 0x00504010 ([ebp-0xc], unused)
        const float dist = joan::orig::arg::as<float>(joan::orig::as_unit(this)->anim_phase());    // 0x005045f0
        if (!switch1(dist)) return;
        // 0x7c9351
        if (is_player() && static_cast<i32>(jo::self<jo::Unit>(this)->layer) == 0) {
            if (jo::as_actor(p2f5())->anim_flags != 0) return;
            if (static_cast<i32>(jo::self<jo::Unit>(this)->combo_queued) == 1) return;
            if (eqz(dist) && (static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 0x1c || static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 0x1d))
                stop();
            if (switch2(dist)) tail(dist);
            return;
        }
        switch3(dist);
    }
};

JOAN_REPLACE("game/b114_15", "Unit::type_behaviour_7c7ab0", 0x007c7ab0, &H::type_behaviour);

}  // namespace game_b114_15
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::die_7c6240(std::int32_t a0) -> void { joan::bind::member<&::game_b114_14::H::die>(this, a0); }  // 0x7c6240
auto joan::orig::Unit::type_behaviour_7c7ab0() -> void { joan::bind::member<&::game_b114_15::H::type_behaviour>(this); }  // 0x7c7ab0
