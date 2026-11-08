// game/unknown_unit_idle.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_0049d490.cpp; re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their
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

using joan::call::cdecl_call;

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

void* const kUnits = obj(d::g_units);

struct Obj {};

void* const kUnitTypes = obj(d::g_types);

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

void JOAN_CDECL init_49d490() { joan::orig::init_pi_a39d14(); }
void JOAN_CDECL init_49d4f0() { joan::orig::init_42e010(); }
void JOAN_CDECL init_49d540() { joan::orig::init_a39d9c(); }
void JOAN_CDECL init_49d590() { joan::orig::init_a39da0(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("init_49d490", 0x0049d490, &init_49d490);
B("init_49d4f0", 0x0049d4f0, &init_49d4f0);
B("init_49d540", 0x0049d540, &init_49d540);
B("init_49d590", 0x0049d590, &init_49d590);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

void JOAN_CDECL init_a39da0() { global<float>(d::g_a39da0) = global<float>(d::g_a39d14) / global<float>(d::flt_4); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("init_a39da0", 0x0049d5c0, &init_a39da0);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using joan::hook::x87::Fpu;

using joan::hook::x87::ftol;

void* const kTypes = obj(d::g_items);

JOAN_COUNTER(c_trade, "X::trade_amount");

struct H {
    // ------------------------------------------------------------------------ 0x0049d5f0
    float trade_amount(i32 id, i32 b) {
        c_trade.hit();
        if (joan::orig::as_units(kTypes)->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0.0f;  // 0x00425d50
        void* const e = joan::orig::as_units(kTypes)->list.ptr_at(joan::orig::arg::i32(id));      // 0x00425d20
        const Fpu fpu{true};
        const float k100 = 100.0f;  // 0x9cf9c4
        const float c = static_cast<float>(jo::view<i32>(jo::as_map_object(e)->damage));
        const float t10 = static_cast<float>(
            static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::Unit>(this)->stats[4])) * global<u32>(d::g_a3cc00)));
        const float t20 = static_cast<float>(fpu.sub(k100, static_cast<double>(b)));
        const i32 lvl = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->level_sum());  // 0x005238c0
        double x = fpu.mul(static_cast<double>(lvl), t20);
        x = fpu.mul(x, static_cast<double>(static_cast<i32>(global<i16>(d::g_a3cce0))));
        x = fpu.div(x, k100);
        x = fpu.add(x, t10);
        float t14 = static_cast<float>(x);
        const i32 m = static_cast<i32>(jo::self<jo::Unit>(this)->stats[7]) + 0x64 - global<i32>(d::g_unit_mor_nor);
        t14 = static_cast<float>(fpu.div(fpu.mul(static_cast<double>(m), t14), k100));
        float t30;
        const double h = fpu.div(fpu.sub(c, t14), 2.0);
        if (!(h >= 0.0)) {
            t30 = 0.0f;
        } else {
            t30 = static_cast<float>(fpu.div(fpu.sub(c, t14), 2.0));
        }
        float t18 = static_cast<float>(fpu.add(fpu.div(c, 2.0), t30));
        const double r = joan::orig::arg::as<double>(joan::orig::as_unit(this)->stat16_factor());  // 0x00510f10
        t18 = static_cast<float>(fpu.mul(r, t18));
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) == 0) {  // 0x00428b70
            auto me_owner = []() {
                void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3c0
                return static_cast<i32>(jo::as_unit(u)->side);
            };
            auto scale = [&fpu, &t18](img::Addr g, float div) {
                const double q = fpu.div(static_cast<double>(global<i32>(g)), div);
                t18 = static_cast<float>(fpu.mul(q, t18));
            };
            if (global<i32>(d::g_mission) == 8) {
                i32 s = static_cast<i32>(jo::as_map_object(e)->side);
                if (s != me_owner()) scale(d::g_unit_f_attack, 250.0f);  // 0x9dd6d0
                s = static_cast<i32>(jo::self<jo::Unit>(this)->side);
                if (s != me_owner()) scale(d::g_unit_f_defend, k100);
            }
            i32 s = static_cast<i32>(jo::as_map_object(e)->side);
            if (s == me_owner()) scale(d::g_unit_f_attack, k100);
            s = static_cast<i32>(jo::self<jo::Unit>(this)->side);
            if (s == me_owner()) scale(d::g_unit_f_defend, k100);
        }
        const i32 left = ftol(fpu.sub(static_cast<double>(jo::self<jo::Unit>(this)->hit_points), t18));
        joan::orig::as_unit(this)->set_value_135(joan::orig::arg::i32(left));  // 0x004fe510
        return t18;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("X::trade_amount", 0x0049d5f0, &H::trade_amount);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b88_03.cpp (game/b88_03)
namespace {
namespace game_b88_03 {

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

std::uint8_t* unit_type(std::int16_t id) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kUnitTypes)->get(joan::orig::arg::i16(id)));  // 0x0042d0a0 (reads the word)
}

JOAN_COUNTER(c_idle_tick, "Unit::idle_tick");

struct H {
    // ------------------------------------------------------------------------ 0x0049d810
    void notify_type() {
        void* const target =
            joan::orig::arg::as<void*>(joan::orig::as_uscr_res(obj(d::g_uscr_res))->entry(joan::orig::arg::i32(static_cast<std::int16_t>(jo::as_unit_type(unit_type(jo::self<jo::Unit>(this)->type_id))->script_id))));  // 0x00484540
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_order(target)->dispatch(this)) == 0 && jo::self<jo::Unit>(this)->script != nullptr) {  // 0x005467b0
            joan::orig::as_script_block(jo::self<jo::Unit>(this)->script)->rewind();  // 0x0063ed40
            std::uint8_t* const o = jo::view<std::uint8_t*>(jo::self<jo::Unit>(this)->script);
            using Virtual = void (Obj::*)(void*);
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(o)[0xc / 4], sizeof(void*));
            (reinterpret_cast<Obj*>(o)->*method)(this);
        }
        jo::self<jo::Unit>(this)->task_kind = global<std::int32_t>(d::g_now);
    }

    void idle_tick() {
        c_idle_tick.hit();
        if ((sx16(jo::self<jo::Unit>(this)->id) == global<std::int32_t>(d::g_player_unit_recno) && sx16(jo::self<jo::Unit>(this)->layer) == 0) ||
            jo::self<jo::Unit>(this)->rider_id != 0) {
            joan::orig::as_unit(this)->clear_247();  // 0x004fd630
            return;
        }
        if (jo::self<jo::Unit>(this)->orders != nullptr && joan::orig::as_dynarray(jo::self<jo::Unit>(this)->orders)->size() != 0) return;  // 0x00417ad0
        if (sx16(jo::self<jo::Unit>(this)->think_forced) != 0) {
            joan::orig::as_unit(this)->poke();  // 0x00528440
            if (jo::as_unit_type(unit_type(jo::self<jo::Unit>(this)->type_id))->script_id != 0) notify_type();
            joan::orig::as_unit(this)->clear_247();  // 0x004fd630
            return;
        }
        const std::uint8_t* const type = unit_type(jo::self<jo::Unit>(this)->type_id);
        if (jo::self<jo::Unit>(this)->task_kind == 0) {
            const std::int32_t r = joan::orig::as_misc(obj(d::g_misc))->random(500);  // 0x00590540
            jo::self<jo::Unit>(this)->task_kind = static_cast<std::int32_t>(static_cast<std::uint32_t>(global<std::int32_t>(d::g_now)) +
                                                              static_cast<std::uint32_t>(r));
        }
        if (static_cast<std::uint32_t>(jo::self<jo::Unit>(this)->task_kind) + static_cast<std::uint32_t>(jo::as_unit_type(type)->idle_delay) >=
            global<std::uint32_t>(d::g_now))
            return;
        joan::orig::as_unit(this)->poke();  // 0x00528440
        if (jo::as_unit_type(unit_type(jo::self<jo::Unit>(this)->type_id))->script_id == 0) return;
        notify_type();
    }
};

#define B88(name, addr, fn) JOAN_REPLACE("game/b88_03", name, addr, fn)
B88("Unit::idle_tick", 0x0049d810, &H::idle_tick);
#undef B88

}  // namespace game_b88_03
}  // namespace

// ---------------------------------------- from modules/game_b109_18.cpp (game/b109_18)
namespace {
namespace game_b109_18 {

using i8 = std::int8_t;
using u16 = std::uint16_t;

using joan::img::str;

// x87 compare outcomes (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool lt_or_nan(double a, double b) { return !(a >= b); }

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_pickups, "Unit::check_pickups_49da30");

struct H {
    // ------------------------------------------------------------------ 0x0049da30
    // Pickup sound: only when the item type has a sound id (+0x17a).
    void pickup_sound(void* type, void* item) {
        if (jo::view<u32>(jo::as_item_def(type)->emitter_scripts[2]) == 0) return;
        const float vol = jo::as_item_def(type)->emitter_scale[2] + jo::as_map_object(item)->yaw_;
        const u8 flag = jo::as_item_def(type)->emitter_flags[2];
        void* const pos = jo::as_map_object(item)->pos;
        void* const name = &jo::as_item_def(type)->emitter_pos[2 * 3];
        void* const snd = joan::orig::arg::as<void*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(type)->emitter_scripts[2]))));  // 0x0041b440
        void* const bank = jo::as_pemi_script(snd)->effect;
        joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene, bank, name, pos, static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(flag))), joan::orig::arg::bits(vol), i32{0}, i32{0x1388});  // 0x0041baf0
    }
    void pickup_event() {
        const Vec3 pos = jo::view<Vec3>(jo::self<jo::Unit>(this)->position_);
        joan::orig::as_sound_list(obj(d::g_sfx_player))->play_effect(str(d::s_PICKUP), pos);  // 0x00494810 "PICKUP"
    }
    void remove_item(void* item) {
        joan::orig::as_units(obj(d::g_items))->remove_49f370(static_cast<i32>(jo::as_map_object(item)->index));  // 0x0049f370
    }
    void show_message(void* type, i32 value) {
        const char* const text = joan::orig::format_positional(joan::orig::arg::str(jo::as_item_def(type)->message_name), value);  // 0x005a6660
        const char* const msg = joan::orig::as_sms(obj(d::g_strings))->open_3(text);         // 0x004dd6a0
        joan::orig::as_sms(obj(d::g_sms))->add(joan::orig::arg::str(msg), i32{0}, i32{0});             // 0x006e5700
    }
    // Returns after handling one list entry `e` (the original jumps back to the list walk).
    void handle_entry(void* e) {
        void* const item = joan::orig::as_dynarray(obj(d::g_unitList))->ptr_at(static_cast<i32>(jo::as_world_item(e)->object_id));  // 0x0041b3f0
        if (item == nullptr) return;
        if (jo::as_map_object(item)->actor == nullptr) return;
        void* const type = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_map_object(item)->type_id)))));  // 0x00425ce0
        if (jo::view<i8>(jo::as_item_def(type)->pickable) == 0) return;
        if (jo::view<i16>(jo::as_item_def(type)->class_id) == 0xf) {
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::self<jo::Unit>(this)->type_id)))));  // 0x0042d0a0
            if (jo::as_unit_type(r)->is_hero == 0) return;
        }
        const float d2 = (jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0] - jo::as_map_object(item)->pos[0]) * (jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0] - jo::as_map_object(item)->pos[0]) +
                         (jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1] - jo::as_map_object(item)->pos[1]) * (jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1] - jo::as_map_object(item)->pos[1]);
        void* const body = jo::self<jo::Unit>(this)->actor;
        void* const ibody = jo::as_map_object(item)->actor;
        const float rr = jo::as_actor(body)->collide_radius * jo::as_actor(body)->scale + jo::as_actor(ibody)->aim_sphere[3] * jo::as_actor(ibody)->scale;
        if (lt_or_nan(rr * rr, d2)) return;
        const double dz = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[2] - jo::as_map_object(item)->pos[2]))));  // fabs
        if (lt_or_nan(jo::as_actor(jo::self<jo::Unit>(this)->actor)->collide_height, dz)) return;
        const i32 age = static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::as_unit(this)->hit_points_max()) - jo::view<u32>(jo::self<jo::Unit>(this)->hit_points));  // 0x00523320
        if (jo::view<i16>(jo::as_item_def(type)->type_id) == 0x15 && age > jo::as_item_def(type)->damage) {
            joan::orig::as_unit(this)->heal(joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(jo::as_world_item(e)->stack.item))), i32{0}, i32{1});  // 0x0050b4f0
            pickup_sound(type, item);
            pickup_event();
            remove_item(item);
            return;
        }
        if (jo::view<i16>(jo::as_item_def(type)->class_id) == 0xf && joan::crt::stricmp(joan::orig::arg::str(jo::view<const char*>(jo::as_item_def(type)->name)), str(d::s_S7_GOLDBAR)) == 0) {  // "S7_GOLDBAR"
            pickup_sound(type, item);
            pickup_event();
            remove_item(item);
            global<u32>(d::g_get_a591b6) += 1;
            const i32 flag = static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_sms(obj(d::g_sms))->head_flag())) == 1 ? 1 : 0;  // 0x006e6010
            void* msg;
            if (global<i32>(d::g_get_a591b6) == 1) {
                msg = joan::orig::arg::as<void*>(joan::orig::as_sms(obj(d::g_strings))->open_11(joan::orig::arg::u32(global<i32>(d::g_get_a591b6))));  // 0x004dd8a0
            } else {
                msg = joan::orig::arg::as<void*>(joan::orig::as_sms(obj(d::g_strings))->open_0c(joan::orig::arg::u32(global<i32>(d::g_get_a591b6))));  // 0x004dd8e0
            }
            joan::orig::as_sms(obj(d::g_sms))->add(joan::orig::arg::str(msg), joan::orig::arg::i32(flag), i32{1});  // 0x006e5700
            return;
        }
        if (jo::view<i16>(jo::as_item_def(type)->class_id) == 0xf &&
            joan::orig::arg::as<i32>(joan::orig::libc::strncmp(joan::orig::arg::str(jo::view<const char*>(jo::as_item_def(type)->name)), str(d::s_S7_food), joan::orig::arg::u32(u32{7}))) == 0) {  // "S7_food"
            if (global<i8>(d::g_a591c6) != 0) {
                pickup_sound(type, item);
                pickup_event();
                remove_item(item);
                global<u32>(d::g_get_a591c2) += 1;
            } else {
                const char* const msg = joan::orig::as_menu(obj(d::g_strings))->message_3();      // 0x004dd5b0
                joan::orig::as_message_list(obj(d::g_names))->post(msg, nullptr, joan::orig::arg::u32(i32{0}));  // 0x004ad710
            }
            return;
        }
        if (jo::view<i16>(jo::as_item_def(type)->class_id) == 8) {
            pickup_sound(type, item);
            pickup_event();
            const i32 count = jo::as_world_item(e)->stack.uses != 0 ? jo::as_world_item(e)->stack.uses : 1;
            void* const etype = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<u32>(jo::as_world_item(e)->stack.item))));  // 0x00425ce0
            const i32 value = static_cast<i32>(jo::view<u32>(jo::as_item_def(etype)->price) * static_cast<u32>(count));
            global<u32>(d::g_get_a596c4) += static_cast<u32>(value);
            remove_item(item);
            if (static_cast<i32>(jo::self<jo::Unit>(this)->id) == global<i32>(d::g_player_unit_recno)) show_message(type, value);
            return;
        }
        const u32 arg3 = jo::as_world_item(e)->stack.count;  // the original left the upper half of `this` in it; Inventory::add reads the low word only (unitdata.cpp)
        if (joan::orig::arg::as<i32>(joan::orig::as_inventory(jo::self<jo::Unit>(this)->modifiers)->add_495f40(joan::orig::arg::u32(static_cast<u32>(jo::as_world_item(e)->stack.item)), joan::orig::arg::u32(static_cast<u32>(jo::as_world_item(e)->stack.u16_4)), joan::orig::arg::u32(arg3), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->id)), joan::orig::arg::u32(jo::view<u32>(jo::as_world_item(e)->stack.uses)), joan::orig::arg::u32(jo::view<u32>(jo::as_world_item(e)->stack.max_uses)), joan::orig::arg::u32(jo::as_world_item(e)->stack.u32_e), joan::orig::arg::u32(i32{0}))) == 0)  // 0x00495f40
            return;
        pickup_sound(type, item);
        pickup_event();
        if (static_cast<i32>(jo::self<jo::Unit>(this)->id) == global<i32>(d::g_player_unit_recno)) show_message(type, static_cast<i32>(static_cast<i16>(jo::as_world_item(e)->stack.count)));
        if (jo::view<i8>(jo::as_item_def(type)->show_armory) != 0 &&
            (joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_state))->has_word_728(joan::orig::arg::i16(static_cast<u32>(jo::as_world_item(e)->stack.item)))) & 0xff) == 0) {  // 0x00472a90
            joan::orig::as_game(obj(d::g_game_state))->push_word_728(joan::orig::arg::i32(static_cast<u32>(jo::as_world_item(e)->stack.item)));  // 0x004729c0
            if (static_cast<i32>(jo::self<jo::Unit>(this)->id) == global<i32>(d::g_player_unit_recno)) {
                joan::orig::as_armory_screen(obj(d::g_armory_screen))->set_item(joan::orig::arg::i16(static_cast<u32>(jo::as_world_item(e)->stack.item)), joan::orig::arg::i16(static_cast<u32>(jo::as_world_item(e)->stack.u16_4)));  // 0x006bbc90
                joan::orig::as_armory_screen(obj(d::g_armory_screen))->open();  // 0x006ba4b0
            }
        }
        remove_item(item);
    }

    void check_pickups() {
        c_pickups.hit();
        if (!(jo::self<jo::Unit>(this)->rank > 3)) return;
        if (jo::self<jo::Unit>(this)->cell == nullptr || jo::view<i8>(jo::as_world_cell(jo::self<jo::Unit>(this)->cell)->loaded) == 0) return;
        if (jo::self<jo::Unit>(this)->move_mode > 3) return;
        if (jo::self<jo::Unit>(this)->actor == nullptr) return;
        void* const leader = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3f0
        if (leader == nullptr) return;
        if (jo::as_unit(leader)->side != jo::self<jo::Unit>(this)->side) return;
        alignas(4) float box[6];
        joan::orig::as_aabb(static_cast<void*>(box))->set_point(joan::orig::arg::ptr(jo::self<jo::Unit>(this)->position_));  // 0x0060d430
        void* body = jo::self<jo::Unit>(this)->actor;
        box[0] = box[0] - (jo::as_actor(body)->collide_radius * jo::as_actor(body)->scale + 50.0f);
        body = jo::self<jo::Unit>(this)->actor;
        box[1] = box[1] - (jo::as_actor(body)->collide_radius * jo::as_actor(body)->scale + 50.0f);
        box[2] = box[2] - jo::as_actor(jo::self<jo::Unit>(this)->actor)->collide_height;
        body = jo::self<jo::Unit>(this)->actor;
        box[3] = (jo::as_actor(body)->collide_radius * jo::as_actor(body)->scale + 50.0f) + box[3];
        body = jo::self<jo::Unit>(this)->actor;
        box[4] = (jo::as_actor(body)->collide_radius * jo::as_actor(body)->scale + 50.0f) + box[4];
        box[5] = box[5] + jo::as_actor(jo::self<jo::Unit>(this)->actor)->collide_height;
        i32 count = 0;
        void** const list = joan::orig::arg::as<void**>(joan::orig::as_world(jo::self<jo::Unit>(this)->world)->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x00429090
        for (i32 i = 0; i < count; ++i) {
            void* const o = list[i];
            if (jo::view<i8>(jo::as_world_cell(o)->loaded) == 0) continue;
            void* e = joan::orig::arg::as<void*>(joan::orig::as_world_cell(o)->get_9c());  // 0x0054f890
            while (e != nullptr) {
                void* const next = jo::view<void*>(jo::as_world_item(e)->cell_prev);
                handle_entry(e);
                e = next;
            }
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_18", name, addr, fn)
B("Unit::check_pickups_49da30", 0x0049da30, &H::check_pickups);
#undef B

}  // namespace game_b109_18
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

void* const kWorld = obj(d::g_nations);

JOAN_COUNTER(c_count_49e500, "Obj::count_49e500");

struct H {
    // ------------------------------------------------------------------ 0x0049e500
    i32 count_49e500(u32 unit_id) {
        c_count_49e500.hit();
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(unit_id)) != 0) return 0;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(unit_id)));  // 0x0041b3c0
        if (jo::self<jo::Place>(this)->world == nullptr) return 0;
        i32 count = 0;
        u8* group = nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kWorld)->is_null(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Place>(this)->nation)))) == 0)       // 0x00484870
            group = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kWorld)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Place>(this)->nation))));  // 0x0047b600
        i32 n = 0;
        u8** const cells = joan::orig::arg::as<u8**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::Place>(this)->world))->cells_in_box(joan::orig::arg::mptr(jo::self<jo::Place>(this)->box), &n));  // 0x00429090
        for (i32 i = 0; i < n; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(cells[i])->get_8c());  // 0x0054ee10
            u8* p = first;
            if (p == nullptr) continue;
            do {
                if (jo::as_unit(p)->hit_points > 0 || jo::view<u32>(jo::as_unit(p)->state_120) != 0) {
                    if (static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::self<jo::Place>(this)->box)->contains(static_cast<void*>(jo::as_unit(p)->position_)))) != 0) {  // 0x0060d5f0
                        u8* const ty = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kUnitTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(p)->type_id))));  // 0x0042d0a0
                        const i32 k = static_cast<i32>(jo::as_unit_type(ty)->unit_class);
                        const i32 c = static_cast<i32>(jo::as_unit_type(ty)->kind);
                        if (k != 0 && k != 8 && k != 9 && c != 7 && c != 2 && c != 3) {
                            if (static_cast<i32>(jo::as_unit(p)->side) != 0 &&
                                joan::orig::arg::as<i32>(joan::orig::as_players(kWorld)->hostile(static_cast<i32>(jo::as_unit(unit)->side), static_cast<i32>(jo::as_unit(p)->side))) != 0)  // 0x004b6860
                                ++count;
                        }
                    }
                }
                p = jo::view<u8*>(jo::as_unit(p)->cell_prev);
            } while (p != nullptr && p != first);
        }
        u8* c = joan::orig::arg::as<u8*>(joan::orig::as_place(this)->facility_head());  // 0x004c48b0
        const i16 g0 = *reinterpret_cast<volatile i16*>(group);
        if (static_cast<i32>(g0) == 0) return count;
        if (joan::orig::arg::as<i32>(joan::orig::as_players(kWorld)->hostile(static_cast<i32>(g0), static_cast<i32>(jo::as_unit(unit)->side))) == 0)
            return count;
        for (; c != nullptr; c = jo::view<u8*>(jo::as_facility(c)->prev)) {
            if (static_cast<i32>(jo::as_facility(c)->kind) != 1) continue;
            if (static_cast<i32>(jo::as_building(virtual_call<u8*>(c, 0x18))->unit_kind) != 0) ++count;
            for (i32 j = 0; j < static_cast<i32>(jo::as_building(virtual_call<u8*>(c, 0x18))->queue_n); ++j)
                count += static_cast<i32>(jo::as_building(virtual_call<u8*>(c, 0x18))->queue_count[j]);
        }
        return count;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Obj::count_49e500", 0x0049e500, &H::count_49e500);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_a39da0() -> void { joan::bind::free<&::game_b97_02::init_a39da0>(); }  // 0x49d5c0
auto joan::orig::Unit::trade_amount(std::int32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b99_03::H::trade_amount>(this, a0, a1)); }  // 0x49d5f0
auto joan::orig::Unit::idle_tick() -> void { joan::bind::member<&::game_b88_03::H::idle_tick>(this); }  // 0x49d810
auto joan::orig::Unit::check_pickups_49da30() -> void { joan::bind::member<&::game_b109_18::H::check_pickups>(this); }  // 0x49da30
auto joan::orig::Place::count_enemy_units(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_03::H::count_49e500>(this, a0)); }  // 0x49e500
