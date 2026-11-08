// game/unknown_game_tick.cpp: functions whose original source file is unknown (formerly game/_unplaced/game_tick.cpp;
// re/attribution.csv: between ww/uti3d/t_scrres.cpp and ww/trackres.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../runtime/hitch.hpp"

#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

namespace {
// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return joan::call::import_call<float>(slot, args...);
#else
    return static_cast<float>(joan::call::import_call<double>(slot, args...));
#endif
}
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::import_call;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;

using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    H* delete_4ea370(std::uint32_t flags) {                                              // 0x004ea370
        joan::orig::as_missile(this)->destruct();  // 0x004e5a40
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }
    void dtor_4e5a40() { joan::orig::as_effect(this)->release(); }                                  // 0x004e5a40 -> 0x004551f0
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("delete_4ea370", 0x004ea370, &H::delete_4ea370);
W1("dtor_4e5a40", 0x004e5a40, &H::dtor_4e5a40);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kUnits = obj(d::g_items);
void* const kTypes = obj(d::g_item_res);

JOAN_COUNTER(c_marker_pos, "Marker::position");
JOAN_COUNTER(c_drop_side, "Table::drop_side");

struct H {
    // ------------------------------------------------------------------------ 0x004e5d90
    V3* marker_position(V3* out, void* actor) {
        c_marker_pos.hit();
        if (joan::orig::as_units(kUnits)->list.empty_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id)) != 0) {  // 0x00425d50
            *out = global<V3>(d::g_pos);
            return out;
        }
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id)));  // 0x00425d20
        if (jo::as_map_object(unit)->actor == nullptr) {
            *out = global<V3>(d::g_pos);
            return out;
        }
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::as_map_object(unit)->type_id)));  // 0x00425ce0
        V3 pos;
        joan::orig::as_actor(jo::view<void*>(jo::as_map_object(unit)->actor))->position(&pos);  // 0x005ac270
        if (actor != nullptr) {
            u8* const view = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->model_id)));  // 0x006af310
            float t = 0.0f;
            const float reach = 500.0f;
            if (view != nullptr) {
                const float len = v3d_float(d::iat_v3d_87_length, static_cast<const void*>(&jo::as_shot(view)->state));  // length of the velocity (Shot +0x142 on x86)
                if (len < 0.0f || len > 0.0f) {  // fcomp 0; test ah,0x40 jne: skip on equal / NaN
                    const float k = reach / len;
                    V3 seg[2];
                    seg[0] = pos;
                    seg[1].x = k * jo::view<float>(jo::as_shot(view)->state.vec0_x) + seg[0].x;
                    seg[1].y = k * jo::as_shot(view)->state.vec0_y + seg[0].y;
                    seg[1].z = k * jo::as_shot(view)->state.vec0_z + seg[0].z;
                    const i32 hit = joan::orig::arg::as<i32>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(actor)->plane))->segment_hit(&t, &seg[0]));  // 0x005d4a80
                    if (hit != 0) {
                        pos.x = (seg[1].x - seg[0].x) * t + pos.x;
                        pos.y = (seg[1].y - seg[0].y) * t + pos.y;
                        pos.z = (seg[1].z - seg[0].z) * t + pos.z;
                    }
                }
            }
        }
        if (jo::view<u32>(jo::as_item_def(type)->emitter_scripts[3]) != 0) {
            const u32 arg6 = jo::view<u32>(jo::as_item_def(type)->emitter_scale[3]);
            u8* const ev = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(type)->emitter_scripts[3]))));  // 0x0041b440
            // the 0x0041b440 entry is a PemiScript (g_a527a8 is the PemiRes, as animgrp.cpp); +0x24 on x86 = effect
            joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(jo::as_map_object(unit)->actor->scene, static_cast<void*>(jo::as_pemi_script(ev)->effect), &jo::as_item_def(type)->emitter_pos[9], &pos, static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), joan::orig::arg::i32(arg6), i32{0}, i32{0x7d0});  // 0x0041baf0
        }
        *out = pos;
        return out;
    }

    // ------------------------------------------------------------------------ 0x004ead00
    void drop_side(i32 side_arg) {
        c_drop_side.hit();
        const i32 side = static_cast<i32>(static_cast<i16>(side_arg));
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_f(joan::orig::arg::i32(i))) != 0) continue;   // 0x00485760
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->get_f(joan::orig::arg::i32(i)));          // 0x00485730
            if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_nations))->is_null(joan::orig::arg::i32(side))) != 0) {  // 0x00484870
                joan::orig::as_missile(e)->expire();                              // 0x004e6fb0
            } else if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::as_missile(e)->shooter_unit_id)) == 0) {  // 0x00428b70
                u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::as_missile(e)->shooter_unit_id)));  // 0x0041b3c0
                if (static_cast<i32>(jo::as_unit(r)->side) != side) joan::orig::as_missile(e)->expire();    // 0x004e6fb0
            }
        }
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Marker::position", 0x004e5d90, &H::marker_position);
B92("Table::drop_side", 0x004ead00, &H::drop_side);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b109_16.cpp (game/b109_16)
namespace {
namespace game_b109_16 {

using joan::orig::helpers::gf;  // float constant in .rdata

const Fpu fpu{true};

JOAN_COUNTER(c_tick, "Game::tick_4e5fc0");

struct H {
    // ---- 0x004e5fc0 ---------------------------------------------------------------------------
    static void call0(std::uintptr_t thunk, img::Addr self) { thiscall<u32>(thunk, obj(self)); }
    static bool call_al(std::uintptr_t thunk, img::Addr self) { return thiscall<u8>(thunk, obj(self)) != 0; }
    static void timed_68a170(u32 slot, void* target) {
        const float t = static_cast<float>(fpu.div(static_cast<double>(global<u32>(d::g_frame_elapsed_ms)), gf(d::flt_1000)));
        joan::orig::as_camera(obj(d::g_weather_system))->track_68a170(joan::orig::arg::bits(t), joan::orig::arg::mptr(target), joan::orig::arg::i32(slot));  // 0x0068a170
    }
    void game_tick() {  // 0x004e5fc0
        joan::hitch::Span hitch_span("game_tick");  // [Debug] HitchLog
        c_tick.hit();
        if (global<i8>(d::g_flag_a59298) == 0 && joan::orig::arg::as<i32>(joan::orig::as_counter(obj(d::g_scenarios))->at_least_10()) != 0) {  // 0x0041c7e0
            global<u32>(d::g_play_ms) = global<u32>(d::g_play_ms) + global<u32>(d::g_frame_elapsed_ms);
            global<u32>(d::g_mission_ms) = global<u32>(d::g_mission_ms) + global<u32>(d::g_frame_elapsed_ms);
        }
        const float speed = jo::self<jo::Wnd>(this)->speed;
        if ((speed < 0.0f || speed > 0.0f) && jo::view<i8>(jo::self<jo::Wnd>(this)->u8_151) == 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_counter(obj(d::g_scenarios))->at_least_10()) != 0 && global<i8>(d::g_game_mode) != 5) {  // 0x0041c7e0
                call0(fn::code_value(fn::Clock_advance_476bb0), d::g_calendar);  // 0x00476bb0
                call0(fn::code_value(fn::Mgr_update_4b0780), d::g_lights);  // 0x004b0780
                call0(fn::code_value(fn::List_update_items), d::g_a50530);  // 0x004afb20
                call0(fn::code_value(fn::UnitManager_update_units_51e280), d::g_units);  // 0x0051e280
                call0(fn::code_value(fn::Slots_advance), d::g_slots_a3ce90);  // 0x00487960
                call0(fn::code_value(fn::Obj_nop_547030), d::g_water_array);  // 0x00547030
                call0(fn::code_value(fn::Trigger_update_units), d::g_actor_list);  // 0x00431f20
                call0(fn::code_value(fn::DynGrass_update_464740), d::g_dyn_grass);  // 0x00464740
                call0(fn::code_value(fn::List_expire_4913c0), d::g_orders);  // 0x004913c0
                call0(fn::code_value(fn::Triggers_update_4ef090), d::g_a5ccd0);  // 0x004ef090
                call0(fn::code_value(fn::UnitList_refresh_4c55d0), d::g_place_array);  // 0x004c55d0
                call0(fn::code_value(fn::List_call_slot10_all), d::g_places);  // 0x0046d4a0
                call0(fn::code_value(fn::Zones_update), d::g_territory_array);  // 0x004e4bb0
                call0(fn::code_value(fn::Mission_tick_471620), d::g_game_state);  // 0x00471620
                call0(fn::code_value(fn::Obj_refresh_63c650), d::g_dialogue);  // 0x0063c650
            }
            if (jo::self<jo::Wnd>(this)->mode == 9) {
                call0(fn::code_value(fn::Hud_track_6fd8b0), d::g_player_hud);  // 0x006fd8b0
                call0(fn::code_value(fn::Fader_update_6ca660), d::g_game_a5d380);  // 0x006ca660
                call0(fn::code_value(fn::Timer_expire), d::g_fade_title);  // 0x00761630
                call0(fn::code_value(fn::Quest_check_d3590), d::g_talk_prompt);  // 0x006d3590
                call0(fn::code_value(fn::ObjScreen_update_721270), d::g_objective_menu);  // 0x00721270
                call0(fn::code_value(fn::Blink9e2_step), d::g_game_over_screen);  // 0x006b7f20
                call0(fn::code_value(fn::BlinkD28_step), d::g_glory_screen);  // 0x006e7cf0
                if (call_al(fn::code_value(fn::Obj_byte_6ae), d::g_a56398)) call0(fn::code_value(fn::Obj_nop_6f7d40), d::g_a56398);  // 0x004d8bb0 / 0x006f7d40
                if (call_al(fn::code_value(fn::X_byte_0_6e6810), d::g_roster_screen)) call0(fn::code_value(fn::Obj_nop_6e67b0), d::g_roster_screen);  // 0x006e6810 / 0x006e67b0
                if (call_al(fn::code_value(fn::X_byte_10), d::g_buy)) call0(fn::code_value(fn::BuyScreen_restart_idle), d::g_buy);  // 0x006c9040 / 0x006c8c00
                if (call_al(fn::code_value(fn::Obj_byte0_4af5d0), d::g_message_indicator)) {                                // 0x004af5d0
                    const u32 id = joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());         // 0x004203c0
                    joan::orig::as_message_indicator(obj(d::g_message_indicator))->on_event(joan::orig::arg::u32(id));                    // 0x004af590
                }
                if (call_al(fn::code_value(fn::X_byte_3f), d::g_names)) call0(fn::code_value(fn::FlockSys_flush_46e8d0), d::g_names);  // 0x004d6a10 / 0x0046e8d0
                if (joan::orig::arg::as<u8>(joan::orig::as_bytes(obj(d::g_weather_system))->at(i32{1})) != 0) {         // 0x0041bf10
                    if (jo::self<jo::Wnd>(this)->mode == 2)
                        timed_68a170(1, img::obj(d::g_view3d));
                    else
                        timed_68a170(1, global<void*>(d::g_camera));
                }
                call0(fn::code_value(fn::ThunderList_update), d::g_weather_system);                                // 0x0068c2a0
                if (joan::orig::arg::as<u8>(joan::orig::as_bytes(obj(d::g_weather_system))->at(i32{2})) != 0) {  // 0x0041bf10
                    if (jo::self<jo::Wnd>(this)->mode == 2)
                        timed_68a170(2, img::obj(d::g_view3d));
                    else
                        timed_68a170(2, global<void*>(d::g_camera));
                }
                call0(fn::code_value(fn::Obj_release_parts), d::g_weather_system);                                              // 0x0068a010
                if (joan::orig::arg::as<i32>(joan::orig::as_pool(obj(d::g_particle_effect_array))->alive_count()) != 0) call0(fn::code_value(fn::Group_purge_dead), d::g_particle_effect_array);  // 0x004aede0 / 0x0041bc60
            }
            const u8 st = jo::view<u8>(jo::self<jo::Wnd>(this)->mode);
            if (st == 9) {
                call0(fn::code_value(fn::Camera_update_5634e0), d::g_camera_man);  // 0x005634e0
                call0(fn::code_value(fn::Formation_update_6ad660), d::g_physic_sys);  // 0x006ad660
            } else if (st == 0xe) {
                call0(fn::code_value(fn::Movie_tick), d::g_video_player);  // 0x006d6370
            } else if (st == 0xf) {
                call0(fn::code_value(fn::Credits_update_6b5900), d::g_credits_screen);  // 0x006b5900
            }
        }
        if (jo::self<jo::Wnd>(this)->mode == 9 && jo::view<i8>(jo::self<jo::Wnd>(this)->u8_151) == 0) {
            call0(fn::code_value(fn::Intro_update_6bfe00), d::g_barracks_screen);  // 0x006bfe00
            call0(fn::code_value(fn::Cutscene_update_6ece80), d::g_workshop_screen);  // 0x006ece80
            call0(fn::code_value(fn::Panel_update_6f2340), d::g_hud);  // 0x006f2340
            call0(fn::code_value(fn::Intro_animate_6e3a90), d::g_quick_save_screen);  // 0x006e3a90
            call0(fn::code_value(fn::SaveMenu_slide_751750), d::g_save_menu);  // 0x00751750
            call0(fn::code_value(fn::Panel_slide_6e5280), d::g_sms);  // 0x006e5280
            call0(fn::code_value(fn::Panel_update_6f43e0), d::g_ingame_menu);  // 0x006f43e0
            call0(fn::code_value(fn::SlidePanel_update), d::g_opt_menu);  // 0x0073db40
            call0(fn::code_value(fn::ObjScreen_update_721270), d::g_objective_menu);  // 0x00721270
            call0(fn::code_value(fn::Obj_update_6e13c0), d::g_msg_log);  // 0x006e13c0
            call0(fn::code_value(fn::Panel_update_6bb750), d::g_armory_screen);  // 0x006bb750
        } else if (jo::self<jo::Wnd>(this)->mode == 3 && jo::view<i8>(jo::self<jo::Wnd>(this)->u8_151) == 0) {
            call0(fn::code_value(fn::Menu_update_70acb0), d::g_main_menu);  // 0x0070acb0
            call0(fn::code_value(fn::SaveMenu_slide_751750), d::g_save_menu);  // 0x00751750
            call0(fn::code_value(fn::SlidePanel_update), d::g_opt_menu);  // 0x0073db40
            call0(fn::code_value(fn::Panel_slide_6e5280), d::g_sms);  // 0x006e5280
        }
        call0(fn::code_value(fn::Intro_step_6e4a20), d::g_quit_popup);  // 0x006e4a20
        call0(fn::code_value(fn::Obj_refresh_6d1980), d::g_ending_screen);  // 0x006d1980
        if (jo::view<i8>(jo::self<jo::Wnd>(this)->u8_151) == 0) {
            call0(fn::code_value(fn::Obj_scan_470f80), d::g_sfx_player);  // 0x00470f80
            joan::orig::as_game(this)->update_music();  // 0x004e6fe0
        }
        if (jo::view<i8>(jo::self<jo::Wnd>(this)->u8_151) != 0 && !call_al(fn::code_value(fn::Obj6e_flag_be), d::g_quit_popup))  // 0x006e4d30
            joan::orig::as_quit_popup(obj(d::g_quit_popup))->show(joan::orig::arg::u32(i32{1}));            // 0x006e4c00
        if (jo::view<i8>(jo::self<jo::Wnd>(this)->u8_151) != 0 && call_al(fn::code_value(fn::X_byte_0_6ba890), d::g_armory_screen)) call0(fn::code_value(fn::Obj6b_init), d::g_armory_screen);  // 0x006ba890 / 0x006ba810
        if (jo::view<i8>(jo::self<jo::Wnd>(this)->u8_151) != 0 && call_al(fn::code_value(fn::Obj_get_6ae), d::g_a56398)) call0(fn::code_value(fn::Panel_close), d::g_a56398);  // 0x00430340 / 0x006f7e90
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_16", name, addr, fn)
B("Game::tick_4e5fc0", 0x004e5fc0, &H::game_tick);
#undef B

}  // namespace game_b109_16
}  // namespace

// ---------------------------------------- from modules/game_b107_16.cpp (game/b107_16)
namespace {
namespace game_b107_16 {

JOAN_COUNTER(c_aim_4e6710, "Obj::aim_point_4e6710");

struct Vec3 {
    float x, y, z;
};
struct Seg {
    Vec3 a, b;
};
static_assert(sizeof(Vec3) == 12 && sizeof(Seg) == 24, "layout");

struct H {
    // ------------------------------------------------------------------ 0x004e6710
    Vec3* aim_point_4e6710(Vec3* out, u8* actor) {
        c_aim_4e6710.hit();
        if (joan::orig::arg::as<u32>(joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id))) != 0) {  // 0x00425d50
            std::memcpy(out, img::obj(d::g_pos), sizeof(Vec3));
            return out;
        }
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id)));  // 0x00425d20
        if (jo::as_map_object(e)->actor == nullptr) {
            std::memcpy(out, img::obj(d::g_pos), sizeof(Vec3));
            return out;
        }
        (void)joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_map_object(e)->type_id));  // 0x00425ce0 (unused)
        Vec3 v = {};
        joan::orig::as_actor(jo::as_map_object(e)->actor)->position(&v);  // 0x005ac270
        if (actor != nullptr) {
            u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->model_id)));  // 0x006af310
            float t = 0.0f;
            const float range = 500.0f;  // 0x43fa0000
            if (q != nullptr) {
                const float len = v3d_float(d::iat_v3d_87_length, static_cast<void*>(&jo::as_shot(q)->state));  // Shot +0x142 on x86  // length
                const float zero = global<float>(d::k_9cf5c4);
                // fcomp + test ah,0x40 jne: skip when equal or unordered.
                if (len < zero || len > zero) {
                    const float k = range / len;
                    Seg s;
                    s.a = v;
                    s.b.x = k * jo::view<float>(jo::as_shot(q)->state.vec0_x) + s.a.x;
                    s.b.y = k * jo::as_shot(q)->state.vec0_y + s.a.y;
                    s.b.z = k * jo::as_shot(q)->state.vec0_z + s.a.z;
                    const u32 hit = joan::orig::arg::as<u32>(joan::orig::as_plane(jo::as_actor(actor)->plane)->segment_hit(&t, &s));  // 0x005d4a80
                    if (hit != 0) {
                        v.x = (s.b.x - s.a.x) * t + v.x;
                        v.y = (s.b.y - s.a.y) * t + v.y;
                        v.z = (s.b.z - s.a.z) * t + v.z;
                    }
                }
            }
        }
        *out = v;
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_16", name, addr, fn)
B("Obj::aim_point_4e6710", 0x004e6710, &H::aim_point_4e6710);
#undef B

}  // namespace game_b107_16
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

struct H {
    // 0x004e6fb0
    void* init_4e6fb0() {
        joan::orig::as_effect(this)->release();  // 0x004551f0
        jo::self<jo::Missile>(this)->stopped = 1;  // +0x24 (a Missile method: missile_methods.inc 0x004e6fb0)
        return this;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Obj::init_4e6fb0", 0x004e6fb0, &H::init_4e6fb0);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b109_03.cpp (game/b109_03)
namespace {
namespace game_b109_03 {

using joan::orig::helpers::wrap_add;
using joan::orig::helpers::wrap_mul;

using joan::orig::helpers::units;
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(id))); }      // 0x0041b3c0
using joan::orig::helpers::random;  // 0x00590540

JOAN_COUNTER(c_music, "Music::update_4e6fe0");

// ============================================================================ 0x004e6fe0
void* snd() { return obj(d::g_music_res); }
void* music() { return obj(d::g_snd); }
void* ambient() { return obj(d::g_mus); }
i32 pick(i32 a, i32 b, i32 c) { return joan::orig::arg::as<i32>(joan::orig::as_table(snd())->pick_random(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c))); }      // 0x004b3940
u8* track_info(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_music_res(snd())->track(joan::orig::arg::i32(id))); }                 // 0x004d8be0
void music_stop() { joan::orig::as_slot(music())->release(); }                               // 0x0059ae00
void music_play(i32 id, i32 f) { joan::orig::as_sound_stream(music())->play(joan::orig::arg::i32(id), joan::orig::arg::u32(f)); }           // 0x0059ae60
i32 music_is(i32 id) { return joan::orig::arg::as<i32>(joan::orig::as_sound_stream(music())->running(joan::orig::arg::i32(id))); }                 // 0x0059b070
void ambient_stop(i32 i) { joan::orig::as_slots(ambient())->release(joan::orig::arg::i32(i)); }                   // 0x005861a0
void ambient_play(i32 id, i32 i, i32 f) { joan::orig::as_ambient_streams(ambient())->play(joan::orig::arg::i32(id), joan::orig::arg::i32(i), joan::orig::arg::u32(f)); }  // 0x00586210
i32 ambient_busy(i32 id, i32 i) { return joan::orig::arg::as<i32>(joan::orig::as_ambient_streams(ambient())->running(joan::orig::arg::i32(id), joan::orig::arg::i32(i))); }  // 0x005864f0
bool cutscene_quiet() {  // 0x006e4510 on 0xa5d408 and its two flags
    return joan::orig::arg::as<i8>(joan::orig::as_quick_save_screen(obj(d::g_quick_save_screen))->flag_45e()) != 0 && global<i8>(d::g_a5d865) == 1 &&
           global<i8>(d::g_a5d860) == 0;
}
u32 ambient_due(i32 i) {  // [0xa5cd98 + i*4] + [0xa5cd90]
    return global<u32>(d::g_a5cd98 + i * 4) + global<u32>(d::g_a5cd90);
}

struct H {
    u8* me() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x004e6fe0
    void music_update_4e6fe0() {
        c_music.hit();
        u8* const self = me();
        i32 mode = 1;   // [ebp-0x8]
        i32 alarm = 0;  // [ebp-0xc]
        if (global<i32>(d::g_video_file) != 0) {
            music_stop();
            return;
        }
        const i32 st = jo::self<jo::Wnd>(self)->mode;
        if (st == 3 || st == 1 || st == 0 || st == 0xf) {
            const i32 id = pick(3, 0, global<i32>(d::g_mission));
            if (global<i32>(d::g_a5d0d4) == 0) {
                music_play(id, 1);
            } else if (global<i32>(d::g_a5d0d4) != id) {
                if (jo::self<jo::MusicTrack>(track_info(global<i32>(d::g_a5d0d4)))->kind != 3) music_play(id, 1);
            } else if (music_is(id) == 0) {
                music_play(id, 1);
            }
            mode = 3;
        } else if (cutscene_quiet()) {
            music_stop();
        } else if (jo::self<jo::Wnd>(self)->mode == 0xc) {
            music_stop();
        } else if (joan::orig::arg::as<i8>(joan::orig::as_victory_screen(obj(d::g_victory_screen))->is_open()) != 0) {  // 0x006e8d20
            music_stop();
            const i32 id = pick(3, 0, global<i32>(d::g_mission));
            if (music_is(id) == 0) music_play(id, 1);
            mode = 3;
        } else {
            mode = 1;
            if (jo::view<i32>(jo::self<jo::ScenarioEntry>(joan::orig::arg::as<u8*>(joan::orig::as_scenario(obj(d::g_mission))->current()))->kind) == 3) {  // 0x004cd840
                mode = 2;
            } else if (unit_missing(global<i32>(d::g_player_unit_recno)) == 0) {
                u8* const u = unit_get(global<i32>(d::g_player_unit_recno));
                void* const zones = obj(d::g_territory_array);
                if (joan::orig::arg::as<i32>(joan::orig::as_dynarray(zones)->is_empty_at(static_cast<i32>(jo::as_unit(u)->zone))) == 0) {  // 0x004217e0
                    u8* const z = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(zones)->get_420ff0(static_cast<i32>(jo::as_unit(u)->zone)));  // 0x00420ff0
                    if (jo::self<jo::ZoneDef>(z)->flag_47 != 0 && jo::view<i8>(jo::self<jo::ZoneDef>(z)->settle) == 4) mode = 8;
                }
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->find_target_54b480(str(d::s_E_9cfc2c), joan::orig::arg::i32(global<i32>(d::g_a39180)), i32{1}, i32{0}, i32{0}, i32{0})) != 0)  // 0x0054b480
                    mode = 2;
                if (joan::orig::arg::as<i32>(joan::orig::as_dynarray(zones)->is_empty_at(static_cast<i32>(jo::as_unit(u)->zone))) == 0) {
                    u8* const z = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(zones)->get_420ff0(static_cast<i32>(jo::as_unit(u)->zone)));
                    if (jo::self<jo::ZoneDef>(z)->flag_47 != 0 && jo::self<jo::ZoneDef>(z)->owner != -1) {
                        // the word is pushed with garbage upper bits; 0x00509590 reads the word only
                        if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->relation_to_side(joan::orig::arg::i16(static_cast<i32>(jo::view<i16[2]>(jo::self<jo::ZoneDef>(z)->owner)[0])))) != 0 ||  // 0x00509590
                            jo::self<jo::ZoneDef>(z)->flag_46 != 0)
                            mode = 2;
                        if (jo::view<i8>(jo::self<jo::ZoneDef>(z)->settle) == 3 || jo::view<i8>(jo::self<jo::ZoneDef>(z)->settle) == 4) alarm = 1;
                    }
                }
            }
            if (global<i8>(d::g_flag_a59298) == 2) mode = 6;
            if (global<i8>(d::g_flag_a59298) == 1) mode = 7;
            if (joan::orig::arg::as<i8>(joan::orig::as_ending(obj(d::g_ending_screen))->is_open()) != 0) {  // 0x006d1ef0
                if (global<i8>(d::g_a57ecb) == 0) mode = 9;
                else if (global<i8>(d::g_a57ecb) == 1) mode = 10;
            }
            const i32 id = pick(mode, 0, global<i32>(d::g_mission));
            if (global<i32>(d::g_a5d0d8) == 1) {
                if (music_is(id) == 0) {
                    if (global<i32>(d::g_a5d0d4) == 0 || jo::self<jo::MusicTrack>(track_info(global<i32>(d::g_a5d0d4)))->kind == 3 ||
                        mode == 7 || mode == 6) {
                        music_play(id, 1);
                    } else if (global<i32>(d::g_a5d0d4) != id) {
                        if (global<i32>(d::g_a5d0d4) == 0 || jo::self<jo::MusicTrack>(track_info(global<i32>(d::g_a5d0d4)))->kind != mode) {
                            if (mode == 1 && jo::self<jo::MusicTrack>(track_info(global<i32>(d::g_a5d0d4)))->kind == 2) {
                                if (global<i32>(d::g_a5d0e4) == 0) global<i32>(d::g_a5d0e4) = global<i32>(d::g_now);
                                if (global<u32>(d::g_now) - global<u32>(d::g_a5d0e4) > 0x1f40u)
                                    joan::orig::as_sound_stream(music())->set_mode(i32{2});  // 0x0059b330
                            } else {
                                joan::orig::as_sound_stream(music())->set_mode(i32{2});
                            }
                        }
                    } else {
                        music_play(id, 1);
                    }
                }
            } else if (global<i32>(d::g_a5d0d8) == 2) {
                if (music_is(0) == 0) music_play(id, 1);
            }
        }
        if (mode != 1) global<i32>(d::g_a5d0e4) = 0;

        for (i32 i = 0; i < 3; ++i) {
            const i32 s = jo::self<jo::Wnd>(self)->mode;
            if (s == 3 || jo::self<jo::Wnd>(self)->mode == 0xc || jo::self<jo::Wnd>(self)->mode == 0xe || global<i8>(d::g_flag_a59298) == 2 ||
                jo::self<jo::Wnd>(self)->mode == 1 || jo::self<jo::Wnd>(self)->mode == 0xf ||
                joan::orig::arg::as<i8>(joan::orig::as_ending(obj(d::g_ending_screen))->is_open()) != 0) {
                ambient_stop(i);
                continue;
            }
            if (cutscene_quiet()) {
                ambient_stop(i);
                continue;
            }
            if (joan::orig::arg::as<i8>(joan::orig::as_victory_screen(obj(d::g_victory_screen))->is_open()) != 0) {
                ambient_stop(i);
                continue;
            }
            if (joan::orig::arg::as<i8>(joan::orig::as_hud(obj(d::g_hud))->is_open()) != 0) {  // 0x004303f0
                ambient_stop(i);
                continue;
            }
            i32 sid = 0;  // [ebp-0x2c]
            if ((joan::orig::arg::as<u32>(joan::orig::as_bytes(obj(d::g_weather_system))->at(i32{1})) & 0xffu) == 0)  // 0x0041bf10
                sid = joan::orig::arg::as<i32>(joan::orig::as_music_res(snd())->find_track(i32{4}, joan::orig::arg::f32(global<i32>(d::g_calendar_minute_of_day))));  // 0x004b3c70
            else
                sid = pick(4, 4, 0);
            if (i == 1 && sid != 0) {
                if (global<u32>(d::g_game_time_ms) > ambient_due(i)) {
                    if (alarm != 0) {
                        sid = (mode == 2) ? pick(5, 7, 0) : pick(5, 6, 0);
                        global<i32>(d::g_a5cd90) = random(0x7530) + 0x7530;
                    } else if (mode == 2) {
                        sid = pick(5, 9, 0);
                        global<i32>(d::g_a5cd90) = random(0x7530) + 0x7530;
                    } else {
                        sid = 0;
                    }
                } else {
                    sid = 0;
                }
            }
            if (i == 2 && sid != 0) {
                if (alarm != 0) {
                    if (global<u32>(d::g_game_time_ms) > ambient_due(i)) {
                        sid = pick(5, 8, 0);
                        global<i32>(d::g_a5cd90) = random(0x7530) + 0x7530;
                    } else {
                        sid = 0;
                    }
                } else if (jo::self<jo::Wnd>(self)->music_flag != 0 && global<u32>(d::g_game_time_ms) > ambient_due(i)) {
                    const i32 sub = jo::self<jo::MusicTrack>(track_info(sid))->owner;
                    sid = pick(5, sub, 0);
                    global<i32>(d::g_a5cd90) = wrap_add(wrap_mul(random(0x7530), 2), 0xea60);
                } else {
                    sid = 0;
                }
            }
            if (sid != 0) {
                const i32 m = global<i32>(d::g_a5cd80 + i * 4);
                if (m == 1) {
                    if (ambient_busy(sid, i) == 0) {
                        if (global<i32>(d::g_a5cd74 + i * 4) != sid)
                            joan::orig::as_ambient_streams(ambient())->set_channel_mode(i32{2}, joan::orig::arg::i32(i));  // 0x005868f0
                        else
                            ambient_play(sid, i, i == 0 ? 1 : 0);
                    }
                } else {
                    if (ambient_busy(0, i) == 0) ambient_play(sid, i, i == 0 ? 1 : 0);
                }
            } else if (i == 0) {
                ambient_stop(i);
            }
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_03", name, addr, fn)
B("Music::update_4e6fe0", 0x004e6fe0, &H::music_update_4e6fe0);
#undef B

}  // namespace game_b109_03
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using u16 = std::uint16_t;

constexpr img::Addr kUnits(d::g_units);

JOAN_COUNTER(c_hire, "Unit::hire_4e8500");

struct H {
    // ------------------------------------------------------------------ 0x004e8500
    static float rank_factor(void* self) {
        const i32 r = static_cast<i32>(jo::self<jo::Unit>(self)->level) - 0x15;
        const float v = static_cast<float>(r) / 10.0f + 1.0f;  // exact int, PC24 ops on floats
        return !(v >= 0.5f) ? 0.5f : v;
    }

    bool hire(u32 id) {
        c_hire.hit();
        if (static_cast<i32>(jo::self<jo::Unit>(this)->bribe_state) != 1) return false;
        if (joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(id)) != 0) return false;  // 0x00428b70
        void* const unit = joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        const Fpu fpu{true};
        i32 cost = 0;
        {
            const u32 price = jo::self<jo::Unit>(this)->rank < 3 ? global<u32>(d::g_a3cd10) : global<u32>(d::g_a3cc54);
            const float f = rank_factor(this);
            const i32 base = static_cast<i32>(jo::view<u32>(jo::self<jo::Unit>(this)->hit_points) * price);
            cost = joan::hook::x87::ftol(fpu.mul(static_cast<double>(base), static_cast<double>(f)));
        }
        void* const names = obj(d::g_types);
        bool archer = false;
        {
            const char* n1 = jo::as_unit_type(joan::orig::arg::as<void*>(joan::orig::as_records_203(names)->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id)))))->name;  // 0x0042d0a0
            if (joan::crt::stricmp(joan::orig::arg::str(n1), str(d::s_MER_EX_ARCHER)) == 0) {
                archer = true;
            } else {
                const char* n2 = jo::as_unit_type(joan::orig::arg::as<void*>(joan::orig::as_records_203(names)->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id)))))->name;
                archer = joan::crt::stricmp(joan::orig::arg::str(n2), str(d::s_MER_EX_ARCHER_CAPTAIN)) == 0;
            }
        }
        if (archer) {
            cost = joan::hook::x87::ftol(fpu.mul(static_cast<double>(cost), static_cast<double>(1.6f)));
        }
        if (global<i32>(d::g_get_a596c4) < cost) {
            if (global<i8>(d::g_a591b5) == 0) {
                const char* const msg = joan::orig::as_menu(obj(d::g_strings))->message_2();               // 0x004dd580
                joan::orig::as_message_list(obj(d::g_names))->post(msg, nullptr, joan::orig::arg::u32(i32{0}));           // 0x004ad710
                global<u8>(d::g_a591b5) = static_cast<u8>(global<u8>(d::g_a591b5) + 1);
            }
            return false;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->id)))) == 0) return false;  // 0x005094d0
        global<i32>(d::g_get_a596c4) = global<i32>(d::g_get_a596c4) - cost;
        if (joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Unit>(this)->weapon))) == 0) {
            void* const other = joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Unit>(this)->weapon)));
            jo::view<u32>(jo::as_arr_i32(jo::as_unit(other)->mask_slots_b)[jo::self<jo::Unit>(this)->weapon_slot].value) = 0;
            jo::view<u32>(jo::as_arr_i32(jo::as_unit(other)->mask_slots)[jo::self<jo::Unit>(this)->weapon_slot].value) = 0;
            jo::view<u32>(jo::self<jo::Unit>(this)->weapon_slot) = 0;
            jo::view<u32>(jo::self<jo::Unit>(this)->weapon) = 0;
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(i32{0}), i32{0}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), joan::orig::arg::mptr(i32{0}));  // 0x0050a480
            joan::orig::unit_refresh_items(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->id)));                          // 0x0065b9b0
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->leads()) != 0 && jo::self<jo::Unit>(this)->group != nullptr) {  // 0x0044a660
            for (i32 i = 0; i < static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->count); ++i) {
                if (joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i]))) != 0) continue;
                void* const m = joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i])));
                joan::orig::as_member(m)->release();  // 0x0042fcb0
            }
        } else {
            joan::orig::as_member(this)->release();
        }
        joan::orig::as_unit(this)->set_side(joan::orig::arg::i32(static_cast<i32>(jo::as_unit(unit)->side)));  // 0x00509250
        joan::orig::as_unit(this)->release_slot();              // 0x00528300
        if (jo::self<jo::Unit>(this)->orders != nullptr) {
            if (jo::self<jo::Unit>(this)->orders->current.kind != 0) {
                joan::orig::as_action(jo::self<jo::Unit>(this)->orders)->dispatch(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004bb660
            }
            void* const p = jo::self<jo::Unit>(this)->orders;
            if (p != nullptr) joan::orig::as_ai(p)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00415220
            jo::self<jo::Unit>(this)->orders = nullptr;
            joan::orig::as_squad(this)->clear();  // 0x00422f10
        }
        jo::view<u16>(jo::self<jo::Unit>(this)->target_building) = 0;
        jo::view<u16>(jo::self<jo::Unit>(this)->target_object_id) = 0;
        jo::view<u16>(jo::self<jo::Unit>(this)->look_a) = 0;
        if (jo::self<jo::Unit>(this)->cell != nullptr) {
            for (void* q = joan::orig::arg::as<void*>(joan::orig::as_world_cell(jo::self<jo::Unit>(this)->cell)->get_8c()); q != nullptr;  // 0x0054ee10
                 q = jo::as_unit(q)->cell_prev) {
                if (jo::as_unit(q)->target_building == jo::self<jo::Unit>(this)->id) jo::view<u16>(jo::as_unit(q)->target_building) = 0;
                if (jo::as_unit(q)->look_a == jo::self<jo::Unit>(this)->id) jo::view<u16>(jo::as_unit(q)->look_a) = 0;
            }
        }
        if (jo::self<jo::Unit>(this)->rank < 3) {
            const i32 n = static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count);
            if (n < joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->morale_level())) {  // 0x00448080
                joan::orig::as_unit(this)->link_to(joan::orig::arg::i32(static_cast<i32>(jo::as_unit(unit)->id)));  // 0x00438f80
            }
        }
        jo::view<u16>(jo::self<jo::Unit>(this)->bribe_state) = 2;
        global<u32>(d::g_get_a591a9) = global<u32>(d::g_get_a591a9) + 1;
        global<u32>(d::g_get_a591ad) = global<u32>(d::g_get_a591ad) + static_cast<u32>(cost);
        joan::orig::as_sound(obj(d::g_sfx_player))->play_kind(str(d::s_RTS_OTHER_NARRATOR_001));  // 0x00492c40
        return true;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Unit::hire_4e8500", 0x004e8500, &H::hire);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b106_13.cpp (game/b106_13)
namespace {
namespace game_b106_13 {

// ======================= static initialiser 0x004e8a50 =======================
struct ArrInit {
    void* init() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);       // 0x005856a0 (pointers: 4 on x86)
        return this;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b106_13", name, addr, fn)
M("Array::init_static_4e8a50", 0x004e8a50, &ArrInit::init);
#undef M

}  // namespace game_b106_13
}  // namespace

// ---------------------------------------- from modules/game_b104_05.cpp (game/b104_05)
namespace {
namespace game_b104_05 {

struct H {
    // ------------------------------------------------------------------------ 0x004e95f0
    u32 dtor_4e95f0() {
        joan::orig::as_missile_list(this)->delete_all();         // 0x004e9f10
        return thiscall<u32>(fn::dtor_DynArrayB, this);  // 0x00585710 (eax left as the original does)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_05", name, addr, fn)
B("X::dtor_4e95f0", 0x004e95f0, &H::dtor_4e95f0);
#undef B

}  // namespace game_b104_05
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

JOAN_COUNTER(c_list_4ea370, "List4ea370::delete_all");

std::int32_t list_count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
void list_remove(void* list, std::int32_t i) { joan::orig::dynarray_b(list)->linkout(joan::orig::arg::i32(i)); }  // 0x005858c0

struct H {
    // 0x004e9f10 List::delete_all (entries deleted with 0x004ea370), then cleared.
    void list_4ea370_delete_all() {
        c_list_4ea370.hit();
        for (std::int32_t i = list_count(this); i >= 1; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->empty_at_f(joan::orig::arg::i32(i))) != 0) continue;  // 0x00485760
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_f(joan::orig::arg::i32(i)));           // 0x00485730
            if (e != nullptr) joan::orig::as_missile(e)->destroy(1u);          // 0x004ea370 delete
            list_remove(this, i);
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("List4ea370::delete_all", 0x004e9f10, &H::list_4ea370_delete_all);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

JOAN_COUNTER(c_add_new, "List::add_new_6c");

using joan::orig::helpers::new_;                            // operator new

struct H {
    // --------------------------------------------------------------------- 0x004ea700
    u32 add_new_6c(void* a, i16 w) {
        c_add_new.hit();
        void* const mem = new_(static_cast<std::uint32_t>(sizeof(jo::Missile)));  // 0x6c on x86
        void* p = mem != nullptr ? joan::orig::as_missile(mem)->construct() : nullptr;  // 0x00420110
        joan::orig::as_marker(p)->launch_from_unit(a, static_cast<i32>(w));                  // 0x00507fc0
        joan::orig::dynarray_b(this)->linkin(&p);                                      // 0x005857a0
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        jo::self<jo::Missile>(p)->index = v;
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("List::add_new_6c", 0x004ea700, &H::add_new_6c);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b101_07.cpp (game/b101_07)
namespace {
namespace game_b101_07 {

struct H {
    // ------------------------------------------------------------------ 0x004ea9c0
    u32 remove_delete(u32 idx) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_f(joan::orig::arg::i32(idx)));  // 0x00485730
        if (p != nullptr) joan::orig::as_missile(p)->destroy(1u);       // 0x004ea370
        return thiscall<u32>(fn::linkout_5858c0, this, idx);             // 0x005858c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_07", name, addr, fn)
B("List::remove_delete", 0x004ea9c0, &H::remove_delete);
#undef B

}  // namespace game_b101_07
}  // namespace

// ---------------------------------------- from modules/game_b103_05.cpp (game/b103_05)
namespace {
namespace game_b103_05 {

void JOAN_CDECL init_4eadc0() { joan::orig::init_pi_4eae10(); }
void JOAN_CDECL init_4eaee0() { joan::orig::init_a3cacc(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("init_4eadc0", 0x004eadc0, &init_4eadc0);
B("init_4eaee0", 0x004eaee0, &init_4eaee0);
#undef B

}  // namespace game_b103_05
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL init_pi_4eae10() {  // 0x004eae10
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));      // atan
    global<float>(d::g_a3cac8) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("init_pi_4eae10", 0x004eae10, &init_pi_4eae10);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL init_a3cacc() { global<float>(d::g_a3cacc) = global<float>(d::g_a3cac8) / 2.0f; }  // 0x004eaf50

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("init_a3cacc", 0x004eaf50, &init_a3cacc);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

// ---- static initialisers (direct calls, no thunk) ----
void JOAN_CDECL init_4eb180() { joan::orig::init_a3cad0(); }
void JOAN_CDECL init_4eb2f0() { joan::orig::set_quarter(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("init_4eb180", 0x004eb180, &init_4eb180);
B("init_4eb2f0", 0x004eb2f0, &init_4eb2f0);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL init_a3cad0() {  // 0x004eb220
    global<float>(d::g_a3cad0) = global<float>(d::g_a3cac8) / global<float>(d::flt_3);  // 3.0f
}

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("init_a3cad0", 0x004eb220, &init_a3cad0);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b100_07.cpp (game/b100_07)
namespace {
namespace game_b100_07 {

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL set_quarter() {  // 0x004eb410
    global<float>(d::g_a3cad4) = global<float>(d::g_a3cac8) / global<float>(d::flt_4);  // 4.0f
}

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_07", name, addr, fn)
B100("set_quarter", 0x004eb410, &set_quarter);
#undef B100

}  // namespace game_b100_07
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
namespace {
namespace game_b101_06 {

struct H {
    // ------------------------------------------------------------------------ 0x004e9c30
    void nop_4e9c30() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("Obj::nop_4e9c30", 0x004e9c30, &H::nop_4e9c30);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Missile::destruct() -> void { joan::bind::member<&::game_world::H::dtor_4e5a40>(this); }  // 0x4e5a40
auto joan::orig::Marker::position(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_04::H::marker_position>(this, a0, a1)); }  // 0x4e5d90
auto joan::orig::Game::tick_4e5fc0() -> void { joan::bind::member<&::game_b109_16::H::game_tick>(this); }  // 0x4e5fc0
auto joan::orig::Missile::expire() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_04::H::init_4e6fb0>(this)); }  // 0x4e6fb0
auto joan::orig::Game::update_music() -> void { joan::bind::member<&::game_b109_03::H::music_update_4e6fe0>(this); }  // 0x4e6fe0
auto joan::orig::Unit::hire(std::uint32_t a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b108_11::H::hire>(this, a0)); }  // 0x4e8500
auto joan::orig::MissileList::delete_all() -> void { joan::bind::member<&::game_leaves81::H::list_4ea370_delete_all>(this); }  // 0x4e9f10
auto joan::orig::Missile::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::delete_4ea370>(this, a0)); }  // 0x4ea370
auto joan::orig::List::add_new_6c(void* a0, std::int16_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::add_new_6c>(this, a0, a1)); }  // 0x4ea700
auto joan::orig::List::remove_delete_4ea9c0(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_07::H::remove_delete>(this, a0)); }  // 0x4ea9c0
auto joan::orig::Table::drop_side(std::int32_t a0) -> void { joan::bind::member<&::game_b92_04::H::drop_side>(this, a0); }  // 0x4ead00
auto joan::orig::init_pi_4eae10() -> void { joan::bind::free<&::game_b99_05::init_pi_4eae10>(); }  // 0x4eae10
auto joan::orig::init_a3cacc() -> void { joan::bind::free<&::game_b98_04::init_a3cacc>(); }  // 0x4eaf50
auto joan::orig::init_a3cad0() -> void { joan::bind::free<&::game_b97_04::init_a3cad0>(); }  // 0x4eb220
auto joan::orig::set_quarter() -> void { joan::bind::free<&::game_b100_07::set_quarter>(); }  // 0x4eb410
