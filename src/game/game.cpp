// game/game.cpp: functions of the original d:\ww\game.cpp (re/attribution.csv).
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
#include "sound_slots.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../interfac/view_range.hpp"
#include "../en3d/anim_cache.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::u;

void* const kMem = obj(d::g_mem);

using joan::call::cdecl_call;

using u16 = std::uint16_t;

using joan::orig::helpers::line_of;

}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
//
//      0x0046f200 Game::shutdown_46f200         (thiscall, ret 0) - frees +0x71c / +0x728 (game.cpp).
//
//      0x00472000 Obj::set_bit(n, v)            (thiscall, ret 8)
//
//      0x00472540 Obj::clear_mask               (thiscall, ret 0) - returns this.
namespace {
namespace game_b99_02 {

JOAN_COUNTER(c_game_shutdown, "Game::shutdown_46f200");
JOAN_COUNTER(c_set_bit, "Obj::set_bit_472000");

// 0x0058ed70 Mem::free(ptr, file, line) ret 0xc.
i32 mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(p, file, line); return 0; }  // void; both callers ignore the result

struct H {
    // ------------------------------------------------------------------ 0x0046f200
    std::uintptr_t game_shutdown() {
        c_game_shutdown.hit();
        if (jo::view<i8>(jo::self<jo::Game>(this)->active) == 0) return u(this);
        if (jo::view<void*>(jo::self<jo::Game>(this)->word_set) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::Game>(this)->word_set), str(d::s_d_ww_game_cpp), static_cast<i32>(global<i16>(d::g_9fed60)) + 6);
            jo::self<jo::Game>(this)->word_set = nullptr;
        }
        jo::view<u32>(jo::self<jo::Game>(this)->word_set_count) = 0;
        jo::view<u32>(jo::self<jo::Game>(this)->word_set_cap) = 0;
        if (jo::view<void*>(jo::self<jo::Game>(this)->word_stack) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::Game>(this)->word_stack), str(d::s_d_ww_game_cpp), static_cast<i32>(global<i16>(d::g_9fed60)) + 0xd);
            jo::self<jo::Game>(this)->word_stack = nullptr;
        }
        jo::view<u32>(jo::self<jo::Game>(this)->word_stack_count) = 0;
        jo::view<u32>(jo::self<jo::Game>(this)->word_stack_cap) = 0;
        joan::orig::as_audio(obj(d::g_audio))->release_buffers();     // 0x0057dd30
        joan::orig::as_slot(obj(d::g_snd))->release();     // 0x0059ae00
        joan::orig::as_slots(obj(d::g_mus))->release(0);  // 0x005861a0
        joan::orig::as_group(obj(d::g_names))->stop_sounds();     // 0x004aec10
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_dialogue))->release_all());  // 0x0063c800
        jo::self<jo::Game>(this)->active = 0;
        return r;
    }

    // ------------------------------------------------------------------ 0x00472000
    u32 set_bit(i32 n, i32 v) {
        c_set_bit.hit();
        const u32 sh = static_cast<u32>(n - 1) & 31u;
        const u32 mask = ~(1u << sh);
        const u32 bit = (static_cast<u32>(v) & 0xffu) << sh;
        if (jo::view<i32>(jo::self<jo::Game>(this)->scenario.id) != 0) {
            u32& w = jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1].u32_18a;
            const u32 r = (w & mask) | bit;
            jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1].u32_18a = r;
            return r;
        }
        const u32 r = (jo::self<jo::Game>(this)->scenario.local.u32_18a & mask) | bit;
        jo::self<jo::Game>(this)->scenario.local.u32_18a = r;
        return r;
    }

    // ------------------------------------------------------------------ 0x00472540
    std::uintptr_t clear_mask() {
        if (jo::view<i32>(jo::self<jo::Game>(this)->scenario.id) != 0)
            jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1].u32_18e = 0;
        else
            jo::self<jo::Game>(this)->scenario.local.u32_18e = 0;
        return u(this);
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Game::shutdown_46f200", 0x0046f200, &H::game_shutdown);
B99("Obj::set_bit_472000", 0x00472000, &H::set_bit);
B99("Obj::clear_mask_472540", 0x00472540, &H::clear_mask);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b107_19.cpp (game/b107_19)
//
//    - Game::init_once_46f350 0x0046f350 (ret 0): byte +1 guard; 0x00472ee0, 0x0051c680,
//      0x005525b0, 0x00547140, 0x00454090 (all cdecl, no args), byte +1 = 1. eax = this when
//      already done, else 0x00454090's eax (the only caller, 0x0046f3f8, ignores eax).
namespace {
namespace game_b107_19 {

struct H {
    // ------------------------------------------------------------------ 0x0046f350
    u32 init_once_46f350() {
        if (static_cast<i8>(jo::self<jo::Game>(this)->init_done) != 0) return joan::call::register_low_bits(this);
        joan::orig::load_difficulty_settings();                // 0x00472ee0
        joan::orig::load_unit_constants();                // 0x0051c680
        joan::orig::load_throw_ini();                // 0x005525b0
        joan::orig::load_weather_ini();                // 0x00547140
        joan::orig::load_charmove_params();  // 0x00454090
        jo::self<jo::Game>(this)->init_done = 1;
        return 0;  // EAX garbage in the original; the only caller (0x0046f3f8) ignores it
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_19", name, addr, fn)
B("Game::init_once_46f350", 0x0046f350, &H::init_once_46f350);
#undef B

}  // namespace game_b107_19
}  // namespace

// ---------------------------------------- from modules/game_b109_01.cpp (game/b109_01)
//
//    - Game::load_resources_46f3a0 0x0046f3a0 (thiscall, ret 0; returns this): one-time load of
//      all resource tables / interfaces with "Loading ..." progress lines (0x00471580).
//
//  - 0x0046f3a0 calls 0x006c2120 and 0x004797a0 calls Camera::set_viewport 0x00613bf0 through
//    their thunks: the display hook only rescales callers inside 0x006c2120..0x006c27c0.
namespace {
namespace game_b109_01 {

using joan::orig::helpers::fbits;

JOAN_COUNTER(c_load, "Game::load_resources_46f3a0");

struct H {
    // -------------------------------------------------------------- 0x0045d330
    // Original frame [ebp-0x150]..[ebp-0x10]: the 0x18-byte time buffer is followed by the
    // locals it can overrun (tod, date, local / system FILETIME - dead after the format call)
    // and then the live file object, exactly as in the original.
    struct PerfFrame {
        char tbuf[0x18];  // [ebp-0x150]
        float tod;        // [ebp-0x138]
        u32 date;         // [ebp-0x134]
        u32 lft[2];       // [ebp-0x130]
        u32 ft[2];        // [ebp-0x128]
        u8 file[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];   // [ebp-0x120] (x64: the larger File)
    };
    static_assert(offsetof(PerfFrame, file) == 0x30 && sizeof(PerfFrame) >= 0x140, "PerfFrame: no padding between the frame slots");  // 0x140 bytes on x86

    // -------------------------------------------------------------- 0x0046f3a0
    void progress(img::Addr text) { joan::orig::as_game(this)->log_progress(str(text)); }  // 0x00471580
    static void run(std::uintptr_t thunk, img::Addr object) { thiscall<u32>(thunk, obj(object)); }

    void* load_resources() {
        if (jo::view<i8>(jo::self<jo::Game>(this)->resources_loaded) != 0) return this;
        c_load.hit();
        const u32 saved = global<u32>(d::g_a403d4);
        global<u32>(d::g_a403d4) = 0;
        joan::orig::as_game(this)->load_interface_res();  // 0x0046fc30
        progress(d::s_Loading_game_play_res);
        joan::orig::as_ini(obj(d::g_steer))->load_res(joan::orig::arg::mptr(str(d::s_RESOURCE_GAMEPLAY_RES)), 0);  // 0x0047c520 "RESOURCE\\GAMEPLAY.RES"
        joan::orig::as_game(this)->init_once_46f350();  // 0x0046f350
        progress(d::s_Loading_music_res);
        run(fn::code_value(fn::Obj_init_4b32b0), d::g_music_res);  // 0x004b32b0
        progress(d::s_Loading_sound_res);
        run(fn::code_value(fn::Obj_fwd_4d0bf0), d::g_sound_def_table);  // 0x004d0bf0
        progress(d::s_Loading_sound_man);
        run(fn::code_value(fn::Obj_reset_4ceb20), d::g_sfx_player);  // 0x004ceb20
        progress(d::s_Loading_bone_name_res);
        joan::orig::as_bone_tree(obj(d::g_bone_name_res))->build(joan::orig::arg::mptr(img::addr(d::g_9f7488)), 0xa);  // 0x005ba570
        progress(d::s_Loading_particle_emitter_res);
        run(fn::code_value(fn::Obj_init_4c19a0), d::g_pemi_res);  // 0x004c19a0
        progress(d::s_Loading_model_res);
        run(fn::code_value(fn::Obj_init_4b09c0), d::g_model_res_mgr);  // 0x004b09c0
        progress(d::s_Loading_animation_script_res);
        run(fn::code_value(fn::AnimScript_stop_and_mark), d::g_model_refs);  // 0x00418e90
        progress(d::s_Loading_animation_res);
        run(fn::code_value(fn::Obj417020_init), d::g_anim_res_mgr);  // 0x00417020
        progress(d::s_Loading_s_date_res);
        run(fn::code_value(fn::SDate_ensure_loaded), d::g_sdate_res);  // 0x004ce030
        progress(d::s_Loading_physis_sys);
        run(fn::code_value(fn::PhysicSystem_init), d::g_physic_sys);  // 0x006ad020
        joan::orig::as_players(obj(d::g_physic_sys))->set_b16(joan::orig::arg::bits(fbits(0x40400000)));  // 0x00472eb0 (3.0f)
        progress(d::s_Loading_race_res);
        run(fn::code_value(fn::Obj_once_4ca310), d::g_race_res);  // 0x004ca310
        progress(d::s_Loading_nation_res);
        run(fn::code_value(fn::Obj_init_4b8f50), d::g_nation_res);  // 0x004b8f50
        progress(d::s_Loading_item_res);
        run(fn::code_value(fn::ItemRes_load_49f780), d::g_item_res);  // 0x0049f780
        progress(d::s_Loading_drop_item_res);
        run(fn::code_value(fn::DropItemRes_reload), d::g_randList);  // 0x00463a10
        progress(d::s_Loading_sell_item_res);
        run(fn::code_value(fn::Obj_reset_4ce390), d::g_sell_item_res);  // 0x004ce390
        progress(d::s_Loading_grass_res);
        run(fn::code_value(fn::GrassRes_load_and_mark), d::g_a50680);  // 0x00487cd0
        progress(d::s_Loading_level_res);
        run(fn::code_value(fn::Obj_init_4a4bf0), d::g_level_res);  // 0x004a4bf0
        progress(d::s_Loading_upgrade_res);
        run(fn::code_value(fn::Obj_init_531650), d::g_upgrade_res);  // 0x00531650
        progress(d::s_Loading_unit_script_res);
        run(fn::code_value(fn::Obj_call_648b80), d::g_scripts);  // 0x00648b50
        run(fn::code_value(fn::Obj_init_5461d0), d::g_uscr_res);  // 0x005461d0
        progress(d::s_Loading_unit_res);
        run(fn::code_value(fn::UnitRes_load_and_mark_524d90), d::g_types);  // 0x00524d90
        progress(d::s_Updating_item_res);
        run(fn::code_value(fn::ItemTable_load_items), d::g_item_res);  // 0x004a49a0
        progress(d::s_Loading_recruit_res);
        run(fn::code_value(fn::Obj_reset_4ca6d0), d::g_recruit_res);  // 0x004ca6d0
        progress(d::s_Loading_texture_effect_res);
        run(fn::code_value(fn::ScrRes_init_4deae0), d::g_texture_effect_res);  // 0x004deae0
        progress(d::s_Loading_game_file_array);
        run(fn::code_value(fn::SaveDir_init), d::g_saves);  // 0x00476fa0
        progress(d::s_Loading_profile_array);
        joan::orig::as_profile_list(obj(d::g_profile_list))->refresh(1);  // 0x004c9cd0
        progress(d::s_Loading_main_game_interface_9d2404);
        run(fn::code_value(fn::Panel_init_6f94f0), d::g_player_hud);  // 0x006f94f0
        progress(d::s_Loading_option_menu_interfac_9d23e0);
        run(fn::code_value(fn::OptPanel_init_73ba80), d::g_opt_menu);  // 0x0073ba80
        progress(d::s_Loading_save_menu_interface_9d23bc);
        run(fn::code_value(fn::InfoPanel_init), d::g_save_menu);  // 0x0074cb10
        progress(d::s_Loading_game_over_interface_9d2398);
        run(fn::code_value(fn::Screen_create_buttons), d::g_game_over_screen);  // 0x006b6c10
        progress(d::s_Loading_objective_menu_inter);
        run(fn::code_value(fn::ObjMenu_build_71d2d0), d::g_objective_menu);  // 0x0071d2d0
        progress(d::s_Loading_ingame_menu_interfac);
        run(fn::code_value(fn::LoadScreen_init_6f2d70), d::g_ingame_menu);  // 0x006f2d70
        progress(d::s_Loading_main_menu_interface_9d2328);
        joan::orig::as_gadget(obj(d::g_main_menu))->set_rect(joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(0), joan::orig::arg::i16(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x00628b00
        progress(d::s_Loading_critical_attack_cont);
        joan::orig::as_critical_hud(obj(d::g_critical_hud))->setup_combos(static_cast<std::int8_t>(joan::orig::arg::i32(1)));  // 0x0045f860
        progress(d::s_Loading_popup_window);
        {
            const i32 y = (global<i32>(d::g_screen_h) - 0x96) / 2;
            const i32 x = (global<i32>(d::g_screen_w) - 0x137) / 2;
            joan::orig::as_popup(obj(d::g_popup_a50808))->open(joan::orig::arg::u32(x), joan::orig::arg::u32(y), 0x137u, 0x96u, 1u, static_cast<jo::Font2d*>(img::obj(d::g_font_a53f28)), 1u, 0xc8u);  // 0x0062a670
        }
        progress(d::s_Loading_inventory_interface_9d22b4);
        run(fn::code_value(fn::Obj6d65d0_init), d::g_a488e0);  // 0x006d65d0
        progress(d::s_Loading_unit_attribute_inter_9d228c);
        run(fn::code_value(fn::AddScreen_ctor), d::g_a56398);  // 0x006f5fe0
        progress(d::s_Loading_attack_combo_interfa_9d2264);
        run(fn::code_value(fn::Panel_init_6ef3d0), d::g_combat_screen);  // 0x006ef3d0
        progress(d::s_Loading_military_balance_int_9d2238);
        run(fn::code_value(fn::X_init_af4c0), d::g_message_indicator);  // 0x004af4c0
        progress(d::s_Loading_cut_scene_interface);
        run(fn::code_value(fn::Obj6ca090_reset), d::g_game_a5d380);  // 0x006ca090
        progress(d::s_Loading_message_array_interf);
        run(fn::code_value(fn::MessageBox_init), d::g_names);  // 0x0046c360
        progress(d::s_Loading_message_log_interfac_9d21c8);
        run(fn::code_value(fn::LongBar_init_6e0e40), d::g_msg_log);  // 0x006e0e40
        progress(d::s_Loading_upgrade_interface);
        run(fn::code_value(fn::Obj_nop_6e6080), d::g_roster_screen);  // 0x006e6080
        progress(d::s_Loading_scenario_start_inter);
        run(fn::code_value(fn::X_set_flag_75f580), d::g_start_objectives_screen);  // 0x0075f580
        progress(d::s_Loading_scenario_end_interfa);
        run(fn::code_value(fn::X_set_flag_755840), d::g_next_objectives_screen);  // 0x00755840
        progress(d::s_Loading_buy_interface);
        run(fn::code_value(fn::Shop_camera_setup_6c2120), d::g_buy);  // 0x006c2120
        progress(d::s_Loading_kill_interface);
        run(fn::code_value(fn::Obj_ctor_6df0f0), d::g_goal_panel);  // 0x006df0f0
        progress(d::s_Loading_quick_save_interface_9d20fc);
        run(fn::code_value(fn::QuickBar_init_6e33e0), d::g_quick_save_screen);  // 0x006e33e0
        progress(d::s_Loading_quit_interface);
        run(fn::code_value(fn::QuestionMark_init), d::g_quit_popup);  // 0x006e4650
        progress(d::s_Loading_ending_interface);
        run(fn::code_value(fn::Panel_init_good_fin), d::g_ending_screen);  // 0x006ce100
        progress(d::s_Loading_short_message_interf);
        run(fn::code_value(fn::QuickBar_init), d::g_sms);  // 0x006e4f10
        progress(d::s_Loading_letter_interface);
        run(fn::code_value(fn::Obj6e04c0_init), d::g_letter);  // 0x006e04c0
        progress(d::s_Loading_victory_interface);
        run(fn::code_value(fn::EndScreen_ctor), d::g_victory_screen);  // 0x006e89a0
        progress(d::s_Loading_vict_interface);
        run(fn::code_value(fn::Menu4_init_6e68f0), d::g_glory_screen);  // 0x006e68f0
        progress(d::s_Loading_intro_interface);
        run(fn::code_value(fn::X_ctor_6d5a70), d::g_video_player);  // 0x006d5a70
        progress(d::s_Loading_generic_message_inte_9d1ff8);
        run(fn::code_value(fn::X_ctor_6d2330), d::g_talk_prompt);  // 0x006d2330
        progress(d::s_Loading_barrack_interface);
        run(fn::code_value(fn::BarrackInterface_init), d::g_barracks_screen);  // 0x006bc530
        progress(d::s_Loading_workshop_interface);
        run(fn::code_value(fn::WorkInterface_WorkInterface), d::g_workshop_screen);  // 0x006e8f40
        progress(d::s_Loading_command_prompt);
        {
            // 0x005e9640 (ret 4) takes only the last of the six pushes; the other five are the
            // trailing arguments of 0x0045b590 (ret 0x2c).
            alignas(4) u8 color[4] = {0, 0, 0, 0};
            const u32 v10 = *joan::orig::arg::as<u32*>(joan::orig::as_color(color)->Color_dword(joan::orig::arg::u32(-1)));  // 0x005e9640
            const i32 w = static_cast<i32>(global<u16>(d::g_screen_w));  // pushed via dx (word read by the callee)
            joan::orig::as_panel(obj(d::g_chat_box))->init(0, 0, joan::orig::arg::i32(w), 2, joan::orig::arg::mptr(img::addr(d::g_text_a5d140)), joan::orig::arg::i32(v10), 1, 0, 0, 0x11, 0);  // 0x0045b590
        }
        progress(d::s_Loading_remap_key_system);
        run(fn::code_value(fn::KeyMap_load_4cae70), d::g_keys_a48820);  // 0x004cae70
        progress(d::s_Loading_help_box);
        run(fn::code_value(fn::Obj_set_owner_a47c88), d::g_tooltip);  // 0x006b8d50
        progress(d::s_Loading_in_game_menu_backgro_9d1f40);
        run(fn::code_value(fn::Frame_init_6f1940), d::g_hud);  // 0x006f1940
        progress(d::s_Loading_armory_interface);
        run(fn::code_value(fn::ItemScreen_setup_6b9870), d::g_armory_screen);  // 0x006b9870
        progress(d::s_Loading_generic_popup_interf_9d1ef8);
        run(fn::code_value(fn::PopupScreen_init_buttons), d::g_popup_a52df8);  // 0x006d3940
        progress(d::s_Loading_credits_interface);
        run(fn::code_value(fn::Screen_create_backgrounds), d::g_credits_screen);  // 0x006b4f30
        joan::anim_cache::build_all();  // [Game] AnimCache: every animation not cached yet converted now, not in play
        joan::orig::as_game(this)->log_progress(nullptr);  // 0x00471580(0)
        jo::self<jo::Game>(this)->resources_loaded = 1;
        global<u32>(d::g_a403d4) = saved;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_01", name, addr, fn)
B("Game::load_resources_46f3a0", 0x0046f3a0, &H::load_resources);
#undef B

}  // namespace game_b109_01
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
//
//      0x0046fa80 Unit::nearest_target_b(int range)   (|dz| <= 100 test, fabs CRT); ret 4
//
//      0x00471580 nop(int)                   empty; ret 4
//
//      0x00471c80 Unit::nearest_target_c(int range)   ret 4
//
//      0x004729c0 WordList::push(short v)    grows by 10 through 0x0058eb30 ("d:\\ww\\game.cpp"); ret 4
//
//  - 0x0046fa80: fabs(cz - sz) (float sub at PC24, then double) compared with 100.0 (qword
//    0x9d03c0): skip when > 100.
namespace {
namespace game_b92_02 {

using joan::call::import_call;
struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_target_b, "Unit::nearest_target_b");
JOAN_COUNTER(c_target_c, "Unit::nearest_target_c");

// Common tail of the target searches: vector to the candidate, length, angle and the tests.
// Returns true when the candidate becomes the best one.
bool target_better(void* self, void* cand, const Vec3* spos, float* best) {
    Vec3 cpos;
    joan::orig::as_unit(cand)->position(joan::orig::arg::mptr(static_cast<void*>(&cpos)));  // 0x004fedb0 position
    Vec3 tmp;
    const Vec3* const d = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &cpos, joan::orig::arg::ptr(spos)));  // 0x0060cc90 sub
    Vec3 diff = *d;
    const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&diff)));  // length
    const float ang = joan::orig::arg::as<float>(joan::orig::as_unit(self)->heading_to_unit(static_cast<std::int32_t>(jo::as_unit(cand)->id)));  // 0x00505960
    const float hi = 90.0f, lo = -90.0f;
    if (len > *best) return false;
    if (!(ang >= lo)) return false;
    if (ang > hi) return false;
    *best = len;
    return true;
}

struct H {
    // 0x0046fa80
    std::int32_t target_b(std::int32_t range) {
        c_target_b.hit();
        Vec3 spos;
        joan::orig::as_unit(this)->position(joan::orig::arg::mptr(static_cast<void*>(&spos)));  // 0x004fedb0
        float best = static_cast<float>(range);
        std::int32_t found = 0;
        if (jo::self<jo::Unit>(this)->cell == nullptr) return 0;
        for (void* c = joan::orig::arg::as<void*>(joan::orig::as_world_cell(jo::self<jo::Unit>(this)->cell)->get_8c()); c != nullptr; c = jo::as_unit(c)->cell_prev) {  // 0x0054ee10
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(c)->hostile_to(joan::orig::arg::i16(static_cast<std::int32_t>(global<std::int16_t>(d::g_player_unit_recno))))) == 0)
                continue;  // 0x005094d0
            if (jo::as_unit(c)->bribe_state != 1) continue;
            if (c == static_cast<void*>(this)) continue;
            if (static_cast<std::int32_t>(jo::as_unit(c)->id) == global<std::int32_t>(d::g_player_unit_recno)) continue;
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(c)->dead()) != 0) continue;  // 0x0042d0e0
            Vec3 cpos;
            joan::orig::as_unit(c)->position(joan::orig::arg::mptr(static_cast<void*>(&cpos)));  // 0x004fedb0
            const float dz = cpos.z - spos.z;
            const double adz = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(dz))));  // fabs
            if (adz > global<double>(d::dbl_100)) continue;                                 // 100.0
            Vec3 tmp;
            const Vec3* const d = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &cpos, &spos));  // 0x0060cc90 sub
            Vec3 diff = *d;
            const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&diff)));  // length
            const float ang =
                joan::orig::arg::as<float>(joan::orig::as_unit(this)->heading_to_unit(static_cast<std::int32_t>(jo::as_unit(c)->id)));  // 0x00505960
            const float hi = 90.0f, lo = -90.0f;
            if (len > best) continue;
            if (!(ang >= lo)) continue;
            if (ang > hi) continue;
            best = len;
            found = jo::as_unit(c)->id;
        }
        return found;
    }

    // 0x00471580
    void nop(const char*) {}

    // 0x00471c80
    std::int32_t target_c(std::int32_t range) {
        c_target_c.hit();
        Vec3 spos;
        joan::orig::as_unit(this)->position(joan::orig::arg::mptr(static_cast<void*>(&spos)));  // 0x004fedb0
        float best = static_cast<float>(range);
        std::int32_t found = 0;
        if (jo::self<jo::Unit>(this)->cell == nullptr) return 0;
        for (void* c = joan::orig::arg::as<void*>(joan::orig::as_world_cell(jo::self<jo::Unit>(this)->cell)->get_8c()); c != nullptr; c = jo::as_unit(c)->cell_prev) {  // 0x0054ee10
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(c)->hostile_to(joan::orig::arg::i16(static_cast<std::int32_t>(global<std::int16_t>(d::g_player_unit_recno))))) != 0)
                continue;                                              // 0x005094d0
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_game(c)->byte_2e1()) == 0) continue;  // 0x00432b90
            if (jo::as_unit(c)->can_interact != 0) continue;
            if (c == static_cast<void*>(this)) continue;
            if (static_cast<std::int32_t>(jo::as_unit(c)->id) == global<std::int32_t>(d::g_player_unit_recno)) continue;
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(c)->dead()) != 0) continue;  // 0x0042d0e0
            if (target_better(this, c, &spos, &best)) found = jo::as_unit(c)->id;
        }
        return found;
    }

    // 0x004729c0
    void words_push(std::int32_t v) {
        if (jo::self<jo::Game>(this)->word_stack_cap <= jo::self<jo::Game>(this)->word_stack_count) {
            jo::self<jo::Game>(this)->word_stack_cap = jo::self<jo::Game>(this)->word_stack_cap + 10;
            const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_9fed74)) + 4;
            jo::self<jo::Game>(this)->word_stack = thiscall<jo::ArrI16*>(
                fn::resize_58eb30, obj(d::g_mem), static_cast<void*>(jo::self<jo::Game>(this)->word_stack),
                static_cast<std::uint32_t>(jo::self<jo::Game>(this)->word_stack_cap) << 1, str(d::s_d_ww_game_cpp), line);  // 0x0058eb30
        }
        jo::view<std::uint16_t>(jo::self<jo::Game>(this)->word_stack[jo::self<jo::Game>(this)->word_stack_count].value) = static_cast<std::uint16_t>(v);
        jo::self<jo::Game>(this)->word_stack_count = jo::self<jo::Game>(this)->word_stack_count + 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Unit::nearest_target_b", 0x0046fa80, &H::target_b);
B("nop_471580", 0x00471580, &H::nop);
B("Unit::nearest_target_c", 0x00471c80, &H::target_c);
B("WordList::push", 0x004729c0, &H::words_push);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b106_08.cpp (game/b106_08)
//
//    0x0046fc30 Game::load_interface_res      (thiscall, ret 0) - once (byte +3): switches the
//               resource archive to "INTERFAC" (0x0058b250 / 0x0058b140 / 0x0058b080), logs
//               "Loading ..._res" (0x00471580) and loads ~65 interface resources
//               (0x00620a50 on each global resource object), restores the archive.
namespace {
namespace game_b106_08 {

JOAN_COUNTER(c_load_res, "Game::load_interface_res");

void* const kArchive = obj(d::g_game_set);

// ---------------------------------------------------------------------------
// 0x0046fc30 helpers
// ---------------------------------------------------------------------------
void log(void* self, img::Addr msg) { joan::orig::as_game(self)->log_progress(str(msg)); }        // 0x00471580
void load(img::Addr res, img::Addr name) { joan::orig::as_bmp_res(obj(res))->load(joan::orig::arg::str(str(name))); }  // 0x00620a50

struct H {
    // 0x0046fc30
    void load_interface_res() {
        c_load_res.hit();
        void* const self = this;
        if (static_cast<i32>(jo::view<std::int8_t>(jo::self<jo::Game>(this)->interface_loaded)) != 0) return;
        const i16 previous = global<i16>(d::g_a56b19);
        global<i16>(d::g_interfac_set_id) = joan::orig::arg::as<i16>(joan::orig::as_game_set(kArchive)->find_set_58b250(str(d::s_INTERFAC)));  // 0x0058b250 "INTERFAC"
        if (static_cast<i32>(global<i16>(d::g_interfac_set_id)) != static_cast<i32>(previous)) {
            joan::orig::as_game_set(kArchive)->close_set_58b140();                                                         // 0x0058b140
            joan::orig::as_game_set(kArchive)->open_set_58b080(static_cast<i32>(global<i16>(d::g_interfac_set_id)));            // 0x0058b080
        }
        log(self, d::s_Loading_lens_flare_res);
        load(d::g_bmp_res_lflare, d::s_LFLARE);
        log(self, d::s_Loading_popup_win_res);
        load(d::g_bmp_res_popupwin, d::s_POPUPWIN);
        log(self, d::s_Loading_main_game_interface_9d3234);
        load(d::g_bmp_res_maingame, d::s_MAINGAME);
        log(self, d::s_Loading_main_game_exp_interf);
        load(d::g_bmp_res_mgexp, d::s_MGEXP);
        log(self, d::s_Loading_main_game_hurt_inter);
        load(d::g_bmp_res_hurt, d::s_HURT);
        log(self, d::s_Loading_main_game_tui_interf);
        load(d::g_bmp_res_tui, d::s_TUI);
        log(self, d::s_Loading_main_menu_interface_9d3168);
        if (global<i32>(d::g_a48858) == 0) {
            load(d::g_bmp_res_menu1024, d::s_MENU1024);
        } else {
            load(d::g_bmp_res_menu1024, d::s_MENU1024);
        }
        load(d::g_bmp_res_menu_b, d::s_MENU_B);
        log(self, d::s_Loading_option_menu_interfac_9d312c);
        load(d::g_bmp_res_optmenu, d::s_OPTMENU);
        log(self, d::s_Loading_objective_menu_res);
        load(d::g_bmp_objmenu, d::s_OBJMENU);
        log(self, d::s_Loading_option_menu_front_pa);
        load(d::g_bmp_res_optmenuf, d::s_OPTMENUF);
        log(self, d::s_Loading_option_menu_back_pan);
        load(d::g_bmp_res_optmenub, d::s_OPTMENUB);
        log(self, d::s_Loading_ingame_menu_res);
        load(d::g_bmp_res_igamemnu, d::s_IGAMEMNU);
        log(self, d::s_Loading_scenario_load_bg_res);
        if (global<i32>(d::g_screen_w) == 800) {
            load(d::g_bmp_res_ld_bg, d::s_LD800BG);
        } else {
            load(d::g_bmp_res_ld_bg, d::s_LD1024BG);
        }
        log(self, d::s_Loading_scenario_load_cp_res);
        if (global<i32>(d::g_screen_w) == 800) {
            load(d::g_bmp_res_ld_cp, d::s_LD800CP);
        } else {
            load(d::g_bmp_res_ld_cp, d::s_LD1024CP);
        }
        log(self, d::s_Loading_scenario_load_bg1_re);
        load(d::g_bmp_res_sl_bg1, d::s_SL_BG1);
        log(self, d::s_Loading_scenario_load_bg2_re);
        load(d::g_bmp_res_sl_bg2, d::s_SL_BG2);
        log(self, d::s_Loading_scenario_load_com_re);
        load(d::g_bmp_res_sl_com, d::s_SL_COM);
        log(self, d::s_Loading_scenario_load_m1_res);
        load(d::g_bmp_res_m1, d::s_M1);
        log(self, d::s_Loading_scenario_load_m2_res);
        load(d::g_bmp_res_m2, d::s_M2);
        log(self, d::s_Loading_scenario_load_m3_res);
        load(d::g_bmp_res_m3, d::s_M3);
        log(self, d::s_Loading_scenario_load_m4_res);
        load(d::g_bmp_res_m4, d::s_M4);
        log(self, d::s_Loading_scenario_load_m5_res);
        load(d::g_bmp_res_m5, d::s_M5);
        log(self, d::s_Loading_scenario_load_m6_res);
        load(d::g_bmp_res_m6, d::s_M6);
        log(self, d::s_Loading_scenario_load_m7_res);
        load(d::g_bmp_res_m7, d::s_M7);
        log(self, d::s_Loading_scenario_load_m8_res);
        load(d::g_bmp_res_m8, d::s_M8);
        log(self, d::s_Loading_vict_interface_res);
        load(d::g_bmp_res_vict, d::s_VICT);
        log(self, d::s_Loading_intro_interface_res);
        load(d::g_a5cd60, d::s_INTRO);
        log(self, d::s_Loading_critical_interface_r);
        load(d::g_bmp_res_critical, d::s_CRITICAL);
        log(self, d::s_Loading_critical_fatal_inter);
        load(d::g_bmp_res_criti_f, d::s_CRITI_F);
        log(self, d::s_Loading_critical_kill_interf);
        load(d::g_bmp_res_criti_k, d::s_CRITI_K);
        log(self, d::s_Loading_inventory_interface_9d2d18);
        load(d::g_bmp_res_invent, d::s_INVENT);
        log(self, d::s_Loading_unit_attribute_inter_9d2ce4);
        load(d::g_bmp_res_iunitatt, d::s_IUNITATT);
        log(self, d::s_Loading_attack_combo_interfa_9d2cac);
        load(d::g_bmp_res_iattcomb, d::s_IATTCOMB);
        log(self, d::s_Loading_military_balance_int_9d2c70);
        load(d::g_bmp_res_milibal, d::s_MILIBAL);
        log(self, d::s_Loading_message_interface_re);
        load(d::g_icons, d::s_MESSAGE);
        log(self, d::s_Loading_message_log_interfac_9d2c0c);
        load(d::g_bmp_res_msglog, d::s_MSGLOG);
        log(self, d::s_Loading_mini_map_icon_res);
        load(d::g_heroIcons, d::s_MAPICON);
        log(self, d::s_Loading_rts_cursor_res);
        load(d::g_bmp_res_rtsicon, d::s_RTSICON);
        log(self, d::s_Loading_buy_interface_res);
        load(d::g_bmp_res_buy, d::s_BUY);
        log(self, d::s_Loading_kill_interface_res);
        load(d::g_bmp_res_kill, d::s_KILL);
        log(self, d::s_Loading_quick_save_interface_9d2b48);
        load(d::g_bmp_res_qsave, d::s_QSAVE);
        log(self, d::s_Loading_quit_interface_res);
        load(d::g_bmp_res_quit, d::s_QUIT);
        log(self, d::s_Loading_ending_interface_res);
        load(d::g_bmp_ending, d::s_ENDING);
        log(self, d::s_Loading_letter_interface_res);
        load(d::g_bmp_res_letter, d::s_LETTER);
        log(self, d::s_Loading_intro_interface_res);
        load(d::g_a5cd60, d::s_INTRO);
        log(self, d::s_Loading_victory_interface_re);
        load(d::g_bmp_res_victory, d::s_VICTORY);
        log(self, d::s_Loading_victory2_interface_r);
        load(d::g_bmp_res_victory2, d::s_VICTORY2);
        log(self, d::s_Loading_victory3_interface_r);
        load(d::g_bmp_res_victory3, d::s_VICTORY3);
        log(self, d::s_Loading_victory_element_inte);
        load(d::g_bmp_res_victorye, d::s_VICTORYE);
        log(self, d::s_Loading_generic_message_inte_9d29bc);
        load(d::g_bmp_res_gmsg, d::s_GMSG);
        log(self, d::s_Loading_barrack_interface_re);
        load(d::g_sheet, d::s_BARRACK);
        log(self, d::s_Loading_common_element_inter);
        load(d::g_bmp_commele, d::s_COMMELE);
        log(self, d::s_Loading_common_element_2_int);
        load(d::g_bmp_commele2, d::s_COMMELE2);
        log(self, d::s_Loading_name_bar_interface_r);
        load(d::g_a527b8, d::s_NAME_BAR);
        log(self, d::s_Loading_save_menu_interface_9d28c0);
        load(d::g_bmp_res_savemenu, d::s_SAVEMENU);
        log(self, d::s_Loading_game_over_interface_9d288c);
        load(d::g_bmp_res_gameover, d::s_GAMEOVER);
        log(self, d::s_Loading_minimap_military_pos);
        load(d::g_sprites_a528c8, d::s_MINI_MP);
        log(self, d::s_Loading_help_box_res);
        load(d::g_bmp_res_helpbox, d::s_HELPBOX);
        log(self, d::s_Loading_in_game_menu_backgro_9d27f0);
        load(d::g_sprites_a460d8, d::s_INGAME_B);
        log(self, d::s_Loading_armory_interface_res);
        load(d::g_bmp_res_armory, d::s_ARMORY);
        log(self, d::s_Loading_mission_debrief_inte);
        load(d::g_bmp_debrief, d::s_DEBRIEF);
        log(self, d::s_Loading_generic_popup_interf_9d2750);
        load(d::g_set, d::s_GPOPUP);
        log(self, d::s_Loading_strategic_command_re);
        load(d::g_bmp_res_strategy, d::s_STRATEGY);
        log(self, d::s_Loading_credits_interface_re);
        load(d::g_bmp_credits, d::s_CREDITS);
        if (static_cast<i32>(global<i16>(d::g_interfac_set_id)) != static_cast<i32>(previous)) {
            joan::orig::as_game_set(kArchive)->close_set_58b140();                               // 0x0058b140
            joan::orig::as_game_set(kArchive)->open_set_58b080(static_cast<i32>(previous));  // 0x0058b080
        }
        jo::self<jo::Game>(this)->interface_loaded = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_08", name, addr, fn)
B("Game::load_interface_res", 0x0046fc30, &H::load_interface_res);
#undef B

}  // namespace game_b106_08
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
//
//      0x00470430 Unit::react_mount(char)                  (thiscall, ret 4)
namespace {
namespace game_b97_01 {

void* const kUnitTable = obj(d::g_units);
void* const kPlayers = obj(d::g_physic_sys);

JOAN_COUNTER(c_react_mount, "Unit::react_mount");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8 anim_busy(void* u) { return joan::orig::arg::as<u8>(joan::orig::as_unit(u)->can_act()); }  // 0x00500ba0
void set_anim(void* u, i32 a) { joan::orig::as_unit(u)->set_action(joan::orig::arg::i32(a), 0, 1); }  // 0x00501190

struct H {
    // ------------------------------------------------------------------ 0x00470430
    void react_mount(i32 flag) {
        c_react_mount.hit();
        if (unit_missing(jo::self<jo::Unit>(this)->mount_id) != 0) return;
        u8* const u = unit_get(jo::self<jo::Unit>(this)->mount_id);
        if (anim_busy(u) == 0) return;
        const i32 k = jo::self<jo::Unit>(this)->anim_kind;
        if (k == 0x2b || k == 0x2c || k == 0x2d || k == 0x4f || k == 0x50 || k == 0x51 || k == 0x52) return;
        if (static_cast<i32>(jo::self<jo::Unit>(this)->action) == 1) return;
        if (static_cast<i32>(jo::as_unit(u)->action) == 1) return;
        if (anim_busy(u) == 0) return;
        void* const pl = joan::orig::as_players(kPlayers)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(u)->actor)->record)));  // 0x006af310
        if (static_cast<i8>(flag) != 0) {
            if (anim_busy(this) != 0 && static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) != 0x49) set_anim(this, 0x44);
            if (static_cast<i32>(jo::as_unit(u)->anim_kind) != 0xa) {
                set_anim(u, 0xa);
                joan::orig::as_body(pl)->set_2a2(joan::orig::arg::i16(1), 0x40a00000, 0x42200000);  // 0x006b1d80 (1, 5.0f, 40.0f)
            }
        } else {
            if (anim_busy(this) != 0 && static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) != 0x49) set_anim(this, 0x45);
            if (static_cast<i32>(jo::as_unit(u)->anim_kind) != 0x10) {
                joan::orig::as_players(kPlayers)->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(u)->actor)->record)), joan::orig::arg::i16(1));  // 0x006ad980
                set_anim(u, 0x10);
                joan::orig::as_body(pl)->set_2a2(joan::orig::arg::i16(0), 0x40a00000, 0x42200000);  // 0x006b1d80 (0, 5.0f, 40.0f)
            }
        }
        jo::view<u32>(jo::self<jo::Unit>(this)->paired_anim_start_ms) = 0;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Unit::react_mount", 0x00470430, &H::react_mount);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
//
//    - Game::reset_all 0x00470630 (ret 0, game.cpp, 68 subsystem resets + 0x004709d0 on this,
//      order script-verified against the original; eax unused by both callers 0x00479770 /
//      0x004d6d8f).
namespace {
namespace game_b108_05 {

JOAN_COUNTER(c_reset_all, "Game::reset_all_470630");

struct H {
    // ------------------------------------------------------------------ 0x00470630
    void reset_all() {
        c_reset_all.hit();
        if (jo::view<i8>(jo::self<jo::Game>(this)->resources_loaded) == 0) return;
        struct Call {
            std::uintptr_t thunk;
            img::Addr self;
        };
        static const Call first[] = {
            {fn::code_value(fn::Screen_close_6f9720), d::g_player_hud}, {fn::code_value(fn::Widget_poll), d::g_main_menu}, {fn::code_value(fn::Buffers_release), d::g_critical_hud}, {fn::code_value(fn::Shop_release_6c27c0), d::g_buy},
            {fn::code_value(fn::Obj6d_reset_c), d::g_goal_panel}, {fn::code_value(fn::Panel_reset_6e37d0), d::g_quick_save_screen}, {fn::code_value(fn::Panel_reset_6e4720), d::g_quit_popup}, {fn::code_value(fn::Obj6c_reset), d::g_ending_screen},
            {fn::code_value(fn::Panel_reset_6e50c0), d::g_sms}, {fn::code_value(fn::Letter_free), d::g_letter}, {fn::code_value(fn::Obj6e_reset), d::g_victory_screen}, {fn::code_value(fn::Screen_shutdown_6e6d20), d::g_glory_screen},
            {fn::code_value(fn::Obj6d_wrap), d::g_video_player}, {fn::code_value(fn::Obj6d_reset), d::g_talk_prompt}, {fn::code_value(fn::Barracks_release), d::g_barracks_screen}, {fn::code_value(fn::Workshop_release), d::g_workshop_screen},
            {fn::code_value(fn::X_reset_6d6650), d::g_a488e0}, {fn::code_value(fn::Panel_reset_6f60d0), d::g_a56398}, {fn::code_value(fn::Screen4188_free_buttons), d::g_combat_screen}, {fn::code_value(fn::Obj6ca0d0_release), d::g_game_a5d380},
            {fn::code_value(fn::Obj_release_05), d::g_message_indicator}, {fn::code_value(fn::Obj75f_init), d::g_start_objectives_screen}, {fn::code_value(fn::Obj756_release), d::g_progress}, {fn::code_value(fn::Obj755_init), d::g_next_objectives_screen},
            {fn::code_value(fn::List_clear_4734a0), d::g_names}, {fn::code_value(fn::Obj_release_children), d::g_msg_log}, {fn::code_value(fn::Obj6b_reset), d::g_credits_screen}, {fn::code_value(fn::PopupWin_release), d::g_popup_a50808},
            {fn::code_value(fn::Obj73c410_release_all), d::g_opt_menu}, {fn::code_value(fn::ObjMenu_release_71e800), d::g_objective_menu}, {fn::code_value(fn::Obj6f_reset_b), d::g_ingame_menu}, {fn::code_value(fn::Panel_init_children), d::g_save_menu},
            {fn::code_value(fn::ItemScreen_release), d::g_game_over_screen}, {fn::code_value(fn::Obj6b_reset_b), d::g_tooltip}, {fn::code_value(fn::Obj6f_reset), d::g_hud}, {fn::code_value(fn::Obj6b_free_children), d::g_armory_screen},
            {fn::code_value(fn::Obj6d_reset_b), d::g_popup_a52df8}, {fn::code_value(fn::BmpRes_free), d::g_bmp_credits}, {fn::code_value(fn::VideoFile_deinit), d::g_video_file},
        };
        static const Call second[] = {
            {fn::code_value(fn::Obj_close_59ad70), d::g_snd}, {fn::code_value(fn::Obj_close_586010), d::g_mus}, {fn::code_value(fn::X_reset_4ceb60), d::g_sfx_player}, {fn::code_value(fn::MusicRes_free), d::g_music_res},
            {fn::code_value(fn::Array_clear), d::g_sound_def_table}, {fn::code_value(fn::TextureEffectRes_deinit), d::g_texture_effect_res}, {fn::code_value(fn::DropItemRes_free_all), d::g_randList}, {fn::code_value(fn::GrassRes_free), d::g_a50680},
            {fn::code_value(fn::SItemRes_release), d::g_sell_item_res}, {fn::code_value(fn::RecruitRes_release), d::g_recruit_res}, {fn::code_value(fn::ItemRes_release_49f620), d::g_item_res}, {fn::code_value(fn::UnitRes_release_524d00), d::g_types},
            {fn::code_value(fn::UScrRes_free), d::g_uscr_res}, {fn::code_value(fn::List_clear_648b80), d::g_scripts}, {fn::code_value(fn::UpgraRes_free), d::g_upgrade_res}, {fn::code_value(fn::LevelRes_free), d::g_level_res},
            {fn::code_value(fn::NatRes_free), d::g_nation_res}, {fn::code_value(fn::RaceRes_free), d::g_race_res},
        };
        static const Call third[] = {
            {fn::code_value(fn::PhysicSystem_deinit), d::g_physic_sys}, {fn::code_value(fn::SdateRes_free), d::g_sdate_res}, {fn::code_value(fn::PemiRes_clear), d::g_pemi_res}, {fn::code_value(fn::ModelRes_free_all), d::g_model_res_mgr},
            {fn::code_value(fn::AnimRes_free), d::g_anim_res_mgr}, {fn::code_value(fn::AnimBuf_release), d::g_model_refs}, {fn::code_value(fn::StrTable_free_all), d::g_steer}, {fn::code_value(fn::BoneNameRes_deinit), d::g_bone_name_res},
            {fn::code_value(fn::GameFileList_clear), d::g_saves}, {fn::code_value(fn::UnitList_clear_4c9f00), d::g_profile_list}, {fn::code_value(fn::RemapKeys_free), d::g_keys_a48820},
        };
        for (const Call& c : first) thiscall<u32>(c.thunk, obj(c.self));
        global<u32>(d::g_movie_start_ms) = 0;
        for (const Call& c : second) thiscall<u32>(c.thunk, obj(c.self));
        joan::orig::as_game(this)->reset_all();  // 0x004709d0
        for (const Call& c : third) thiscall<u32>(c.thunk, obj(c.self));
        jo::self<jo::Game>(this)->resources_loaded = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Game::reset_all_470630", 0x00470630, &H::reset_all);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
//
//      0x00470920 TalkList::purge(int)             (thiscall, ret 4) - (dead code).
namespace {
namespace game_b102_02 {

JOAN_COUNTER(c_talk_purge, "TalkList::purge");

struct H {
    // 0x00470920
    i32 talk_purge(i32 who) {
        c_talk_purge.hit();
        i32 i = joan::orig::arg::as<i32>(joan::orig::as_talk(this)->count());  // 0x004b8c40
        for (; i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_talk(this)->is_deleted(joan::orig::arg::i32(i))) != 0) continue;   // 0x004e4110
            u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_talk(this)->get_msg(joan::orig::arg::i32(i)));         // 0x004e3fe0
            if (static_cast<i32>(jo::as_treaty(m)->from) == who || static_cast<i32>(jo::as_treaty(m)->to) == who ||
                joan::orig::arg::as<i32>(joan::orig::as_treaty(m)->valid()) == 0)                    // 0x004e3450
                joan::orig::as_talk(this)->remove_message(joan::orig::arg::i32(i));                        // 0x0046ad90
        }
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("TalkList::purge", 0x00470920, &H::talk_purge);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
//
//      0x004709d0 Game::reset_all                   (thiscall, ret 0) - 64 x 0x00620ab0 + 0x0045b7c0.
namespace {
namespace game_b101_02 {

JOAN_COUNTER(c_reset_all, "Game::reset_all");

struct ResetEntry {
    img::Addr object;
    fn::Code thunk;
};
constexpr ResetEntry kResets[] = {
    {d::g_bmp_res_lflare, fn::BmpRes_free}, {d::g_bmp_res_popupwin, fn::BmpRes_free}, {d::g_bmp_res_maingame, fn::BmpRes_free}, {d::g_bmp_res_mgexp, fn::BmpRes_free},
    {d::g_bmp_res_hurt, fn::BmpRes_free}, {d::g_bmp_res_tui, fn::BmpRes_free}, {d::g_bmp_res_menu1024, fn::BmpRes_free}, {d::g_bmp_res_menu_b, fn::BmpRes_free},
    {d::g_bmp_res_critical, fn::BmpRes_free}, {d::g_bmp_res_criti_f, fn::BmpRes_free}, {d::g_bmp_res_criti_k, fn::BmpRes_free}, {d::g_bmp_res_invent, fn::BmpRes_free},
    {d::g_bmp_res_iunitatt, fn::BmpRes_free}, {d::g_bmp_res_iattcomb, fn::BmpRes_free}, {d::g_bmp_res_milibal, fn::BmpRes_free}, {d::g_icons, fn::BmpRes_free},
    {d::g_bmp_res_msglog, fn::BmpRes_free}, {d::g_heroIcons, fn::BmpRes_free}, {d::g_bmp_res_rtsicon, fn::BmpRes_free}, {d::g_bmp_res_buy, fn::BmpRes_free},
    {d::g_bmp_res_kill, fn::BmpRes_free}, {d::g_bmp_res_qsave, fn::BmpRes_free}, {d::g_bmp_res_quit, fn::BmpRes_free}, {d::g_bmp_ending, fn::BmpRes_free},
    {d::g_bmp_res_letter, fn::BmpRes_free}, {d::g_a5cd60, fn::BmpRes_free}, {d::g_bmp_res_victory, fn::BmpRes_free}, {d::g_bmp_res_victory2, fn::BmpRes_free},
    {d::g_bmp_res_victory3, fn::BmpRes_free}, {d::g_bmp_res_victorye, fn::BmpRes_free}, {d::g_bmp_res_gmsg, fn::BmpRes_free}, {d::g_sheet, fn::BmpRes_free},
    {d::g_bmp_commele, fn::BmpRes_free}, {d::g_bmp_commele2, fn::BmpRes_free}, {d::g_bmp_res_optmenu, fn::BmpRes_free}, {d::g_bmp_res_optmenuf, fn::BmpRes_free},
    {d::g_bmp_res_optmenub, fn::BmpRes_free}, {d::g_bmp_objmenu, fn::BmpRes_free}, {d::g_bmp_res_igamemnu, fn::BmpRes_free}, {d::g_bmp_res_ld_bg, fn::BmpRes_free},
    {d::g_bmp_res_ld_cp, fn::BmpRes_free}, {d::g_bmp_res_sl_bg1, fn::BmpRes_free}, {d::g_bmp_res_sl_bg2, fn::BmpRes_free}, {d::g_bmp_res_sl_com, fn::BmpRes_free},
    {d::g_bmp_res_m1, fn::BmpRes_free}, {d::g_bmp_res_m2, fn::BmpRes_free}, {d::g_bmp_res_m3, fn::BmpRes_free}, {d::g_bmp_res_m4, fn::BmpRes_free},
    {d::g_bmp_res_m5, fn::BmpRes_free}, {d::g_bmp_res_m6, fn::BmpRes_free}, {d::g_bmp_res_m7, fn::BmpRes_free}, {d::g_bmp_res_m8, fn::BmpRes_free},
    {d::g_bmp_res_vict, fn::BmpRes_free}, {d::g_a5cd60, fn::BmpRes_free}, {d::g_a527b8, fn::BmpRes_free}, {d::g_bmp_res_savemenu, fn::BmpRes_free},
    {d::g_bmp_res_gameover, fn::BmpRes_free}, {d::g_chat_box, fn::Obj_free_4}, {d::g_sprites_a528c8, fn::BmpRes_free}, {d::g_bmp_res_helpbox, fn::BmpRes_free},
    {d::g_sprites_a460d8, fn::BmpRes_free}, {d::g_bmp_res_armory, fn::BmpRes_free}, {d::g_bmp_debrief, fn::BmpRes_free}, {d::g_set, fn::BmpRes_free},
    {d::g_bmp_res_strategy, fn::BmpRes_free},
};  // 0x404cc8 -> 0x00620ab0, 0x405ae2 -> 0x0045b7c0
static_assert(sizeof(kResets) / sizeof(kResets[0]) == 65);

struct H {
    // ------------------------------------------------------------------ 0x004709d0
    std::uintptr_t reset_all() {
        c_reset_all.hit();
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Game>(this)->interface_loaded)) == 0) return u(this);  // eax = this (movsx went to ecx)
        u32 r = 0;
        for (const ResetEntry& e : kResets) r = thiscall<u32>(e.thunk, obj(e.object));
        jo::self<jo::Game>(this)->interface_loaded = 0;
        return r;
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("Game::reset_all", 0x004709d0, &H::reset_all);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
//
//  * module game/world1: small leaves - camera-motion state copies, scene / unit list
//    wrappers, string-table lookup (0x005a3090), device lost check (0x005ff420), scalar
//    deleting destructors, the scene's sorted pointer array (0x005eded0), streamed-sound
//    pause / resume (0x0063c6d0, 0x00470c90, 0x004ced90).
namespace {
namespace game_world {

struct H {
    void sound_resume(std::int32_t id) {                                                 // 0x00470c90
        if (id == 0) return;
        for (std::int16_t w = 1; w <= joan::sound_slots::count(); w = static_cast<std::int16_t>(w + 1)) {
            void** const e = joan::orig::arg::as<void**>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(w)));  // 0x004cf690
            if (*e == nullptr) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sound_slot_entry(e)->id()) != id) continue;  // 0x004d0440
            if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_sound3d(*e)->state())) == 3) continue;  // 0x0059fc90
            joan::orig::as_sound(*e)->pause();  // 0x0059f650
            return;
        }
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Sounds::resume", 0x00470c90, &H::sound_resume);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b106_07.cpp (game/b106_07)
//
//      0x00470d40 Game::load_arrays             (thiscall, ret 0) - once ([+4] byte guard):
//                 clears flags, and unless [+0x17] == 6 logs 'Loading ...' (0x00471580) and
//                 runs the 17 array loaders in order, then 0x004641a0([0xa58058], [0xa58057]).
//
//  - 0x00470d40: the byte arg of 0x004641a0 is pushed as `mov al, [0xa58057]; push eax`, the
//    upper 24 bits being whatever eax 0x00471580 (an empty ret 4 stub) leaves, i.e. the eax of
//    the trigger_system loader in the original; the callee only reads the byte (mov al,[ebp+0xc]),
//    so the upper bits are irrelevant. No caller reads 0x00470d40's eax.
namespace {
namespace game_b106_07 {

JOAN_COUNTER(c_load_arrays, "game/b106_07 Game::load_arrays");

// ---- 0x00470d40 tables -------------------------------------------------------------------
struct Loader {
    img::Addr message;  // 'Loading ...'
    fn::Code thunk;
    img::Addr object;
};
constexpr Loader kLoaders[] = {
    {d::s_Loading_modify_light_array, fn::Obj_nop_4b0510, d::g_lights},  // modify_light_array      0x004b0510
    {d::s_Loading_world_array, fn::X_reset_2e, d::g_a45f08},  // world_array             0x00561050
    {d::s_Loading_nation_array, fn::Town_add_all_buildings, d::g_nations},  // nation_array            0x004b64c0
    {d::s_Loading_place_array, fn::Obj_nop_4c5240, d::g_place_array},  // place_array             0x004c5240
    {d::s_Loading_facility_array, fn::Obj_nop_46cda0, d::g_places},  // facility_array          0x0046cda0
    {d::s_Loading_territory_array, fn::Obj_nop_4e4990, d::g_territory_array},  // territory_array         0x004e4990
    {d::s_Loading_unit_array, fn::Selection_init, d::g_units},  // unit_array              0x0051dcc0
    {d::s_Loading_item_array, fn::Obj_nop_49e9a0, d::g_items},  // item_array              0x0049e9a0
    {d::s_Loading_modify_item_array, fn::Obj_nop_4af9e0, d::g_a50530},  // modify_item_array       0x004af9e0
    {d::s_Loading_throw_item_array, fn::Obj_nop_4e9c30, d::g_orders},  // throw_item_array        0x004e9c30
    {d::s_Loading_water_array, fn::Obj_nop_546d60, d::g_water_array},  // water_array             0x00546d60
    {d::s_Loading_water_wave_array, fn::WaterWave_load, d::g_actor_list},  // water_wave_array        0x0056ed40
    {d::s_Loading_talk_res, fn::Obj_register_4e3740, d::g_queue},  // talk_res                0x004e3740
    {d::s_Loading_news_array, fn::Obj_reset_4b9250, d::g_tasks},  // news_array              0x004b9250
    {d::s_Loading_animation_particle_e, fn::Obj_nop, d::g_particle_effect_array},  // animation_particle_...  0x0041b510
    {d::s_Loading_glowing_light_array, fn::Obj_stub_4876b0, d::g_slots_a3ce90},  // glowing_light_array     0x004876b0
    {d::s_Loading_trigger_system, fn::Obj_nop_4eedf0, d::g_a5ccd0},  // trigger_system          0x004eedf0
};

struct H {
    // 0x00470d40
    void load_arrays() {
        if (jo::view<std::int8_t>(jo::self<jo::Game>(this)->arrays_loaded) != 0) return;
        c_load_arrays.hit();
        jo::self<jo::Game>(this)->end_state = 0;
        jo::self<jo::Game>(this)->cutscene_flag = 0;
        jo::self<jo::Game>(this)->u8_71b = 0;
        jo::self<jo::Game>(this)->end_delay = 0;
        jo::self<jo::Game>(this)->end_since = 0;
        global<u8>(d::g_a4d765) = 0;
        if (jo::self<jo::Game>(this)->scenario.id != 6) {
            jo::view<u16>(jo::self<jo::Game>(this)->end_condition_count) = 0;
            global<u16>(d::g_a4d80f) = 0;
            for (const Loader& l : kLoaders) {
                joan::orig::as_game(this)->log_progress(img::str(l.message));  // 0x00471580
                thiscall<u32>(l.thunk, obj(l.object));
            }
            joan::orig::as_game(this)->log_progress(img::str(d::s_Loading_dynamic_grass));  // 0x00471580 'Loading dynamic_grass'
            const u32 b = global<u8>(d::g_cfg_grass_enable);  // the upper bits were the void callee's eax; DynGrass::init reads the low byte only
            joan::optmenu::clamp_view_ranges();  // widened grass distance range (interfac/view_range.hpp)
            joan::orig::as_grass(obj(d::g_dyn_grass))->init(joan::orig::arg::i32(global<u32>(d::g_cfg_grass_density)), joan::orig::arg::i32(b));  // 0x004641a0
        }
        jo::self<jo::Game>(this)->arrays_loaded = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_07", name, addr, fn)
B("Game::load_arrays", 0x00470d40, &H::load_arrays);
#undef B

}  // namespace game_b106_07
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
//
//    - Obj::scan_470f80 0x00470f80 (ret 0): for i = 1..12 entries 0x004cf690(i) with a live
//      object, +0x2d == 0, 0x0059fd60 > 0 and (char)0x0059fc90 == 4: 0x004cea40(entry).
namespace {
namespace game_b108_04 {

JOAN_COUNTER(c_scan, "Obj::scan_470f80");

struct H {
    // ------------------------------------------------------------------ 0x00470f80
    void scan_470f80() {
        c_scan.hit();
        for (i16 i = 1; i <= joan::sound_slots::count(); ++i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(i32{i})));  // 0x004cf690
            if (jo::as_stream_slot(e)->source == nullptr) continue;
            if (jo::as_stream_slot(e)->source->looping != 0) continue;
            if (joan::orig::arg::as<i32>(joan::orig::as_sound3d(jo::as_stream_slot(e)->source)->queued_59fd60()) <= 0) continue;                // 0x0059fd60
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_sound3d(jo::as_stream_slot(e)->source)->state())) != 4) continue;  // 0x0059fc90
            joan::orig::as_stream_slot(e)->release();                                              // 0x004cea40
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Obj::scan_470f80", 0x00470f80, &H::scan_470f80);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b83_06.cpp (game/b83_06)
//
//    - Game::end_session 0x00471010 (keep_units) ret 4 (game.cpp): manager shutdown calls,
//      unit / slot cleanup and state reset.
namespace {
namespace game_b83_06 {

JOAN_COUNTER(c_end, "Game::end_session");

struct H {
    // ------------------------------------------------------------------ 0x00471010
    void end_session(std::int32_t keep_units) {
        if (jo::view<std::int8_t>(jo::self<jo::Game>(this)->arrays_loaded) == 0) return;
        c_end.hit();
        joan::orig::as_group(obj(d::g_names))->stop_sounds();  // 0x004aec10
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_shop(obj(d::g_game_a5d380))->active()) != 0) {  // 0x0042ced0
            global<std::uint8_t>(d::g_a5d384) = 0;
            global<std::uint32_t>(d::g_a5d38b) = 0;
            global<std::uint8_t>(d::g_a5d393) = 0;
            if (joan::orig::arg::as<std::int16_t>(joan::orig::as_track(global<void*>(d::g_a5d394))->is_type1()) != 0)  // 0x00428aa0
                joan::orig::as_track(global<void*>(d::g_a5d394))->restart();                     // 0x004289e0
            joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(joan::orig::arg::i32(global<std::uint32_t>(d::g_a5d3b0)));  // 0x00429860
            joan::orig::as_group(obj(d::g_names))->set_paused(static_cast<std::int8_t>(joan::orig::arg::i32(0)));                                  // 0x0049b5e0
            joan::orig::as_game(this)->set_744(static_cast<char>(joan::orig::arg::i32(0)));                                             // 0x004725f0
            global<std::uint32_t>(d::g_a5d38f) = 0;
            joan::orig::as_cutscene(obj(d::g_game_a5d380))->release();  // 0x006ca0d0
            joan::orig::as_cutscene(obj(d::g_game_a5d380))->reset();  // 0x006ca090
        }
        joan::orig::as_goal_panel(obj(d::g_goal_panel))->close_first();  // 0x006dff10
        joan::orig::as_goal_panel(obj(d::g_goal_panel))->close_second();  // 0x006e01d0
        joan::orig::as_letter(obj(d::g_letter))->close();  // 0x006e09d0
        joan::orig::as_victory_screen(obj(d::g_victory_screen))->close();  // 0x006e8cf0
        joan::orig::as_glory_screen(obj(d::g_glory_screen))->close();  // 0x006e7b20
        joan::orig::as_video_player(obj(d::g_video_player))->stop();  // 0x006d6230
        joan::orig::as_talk_prompt(obj(d::g_talk_prompt))->hide();  // 0x006d3480
        joan::orig::as_hud(obj(d::g_player_hud))->reset_be();  // 0x00707df0
        joan::orig::as_message_list(obj(d::g_names))->reset();  // 0x004ae6c0
        if (jo::view<std::int32_t>(jo::self<jo::Game>(this)->scenario.id) != 5 || keep_units == 0) {
            joan::orig::as_world(obj(d::g_player_unit_recno))->leave_mode(static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x0042df50
            joan::orig::as_sound3d(obj(d::g_weather_system))->release(static_cast<char>(joan::orig::arg::i32(0)));  // 0x00689d00
            joan::orig::as_cloud_layers(obj(d::g_cloud_layers))->release();     // 0x00459b70
            joan::orig::as_list(obj(d::g_particle_effect_array))->destroy_41b530();     // 0x0041b530
            joan::orig::as_news_list(obj(d::g_tasks))->clear_wrap();     // 0x004b9290
            joan::orig::as_talk(obj(d::g_queue))->nop_4e3790();     // 0x004e3790
            joan::orig::as_waters(obj(d::g_water_array))->clear_all();     // 0x00546d80
            joan::orig::as_wave_list(obj(d::g_actor_list))->clear_all();     // 0x0056f010
            joan::orig::as_missile_list(obj(d::g_orders))->delete_all();     // 0x004e9f10
            joan::orig::as_list(obj(d::g_a50530))->clear_4afa00();     // 0x004afa00
            joan::orig::as_item_list(obj(d::g_items))->clear();     // 0x0049e9c0
            joan::orig::as_units(obj(d::g_units))->clear_all();     // 0x0051df00
            joan::orig::as_list(obj(d::g_places))->destroy_46cdc0();     // 0x0046cdc0
            joan::orig::as_list(obj(d::g_place_array))->delete_all_4c5260();     // 0x004c5260
            joan::orig::as_list(obj(d::g_territory_array))->delete_all_4e49b0();     // 0x004e49b0
            joan::orig::as_list(obj(d::g_nations))->clear_4b6530();     // 0x004b6530
            global<std::int16_t>(d::g_a3ce48) = 0;
            joan::orig::as_world_array(obj(d::g_a45f08))->clear_all();  // 0x00561090
            joan::orig::as_list(obj(d::g_lights))->clear_4b0530();  // 0x004b0530
            joan::orig::as_markers(obj(d::g_slots_a3ce90))->delete_all();  // 0x004876d0
            joan::orig::as_list(obj(d::g_a5ccd0))->delete_all_eee10();  // 0x004eee10
            joan::orig::as_grass(obj(d::g_dyn_grass))->release();  // 0x00464390
            for (std::int32_t i = 0; i < 0x28; ++i) joan::orig::as_end_condition(&jo::self<jo::Game>(this)->end_conditions[i])->release();  // 0x00467260
            jo::self<jo::Game>(this)->end_condition_count = 0;
            global<std::int16_t>(d::g_a4d80f) = 0;
        }
        if (jo::view<std::int32_t>(jo::self<jo::Game>(this)->scenario.id) == 5 && keep_units != 0) {
            for (std::int32_t i = 0; i < 0x28; ++i) {
                if (jo::self<jo::Game>(this)->end_conditions[i].used != 0 && jo::self<jo::Game>(this)->end_conditions[i].result != 0 &&
                    jo::self<jo::Game>(this)->end_conditions[i].state != 3)
                    joan::orig::as_ending(this)->start(joan::orig::arg::i32(i));  // 0x00471f90
            }
            if (jo::self<jo::Game>(this)->scenario.squad_count != 0) {
                for (std::int32_t j = 0; j < static_cast<std::int32_t>(jo::self<jo::Game>(this)->scenario.squad_count); ++j) {
                    void* const p = jo::view<void*>(jo::self<jo::Game>(this)->scenario.squads[j].unit);
                    if (p != nullptr) joan::orig::as_unit(p)->destroy(1u);  // 0x00471530
                }
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Game>(this)->scenario.squads)), str(d::s_d_ww_game_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_9fed64)) + 0x57));  // 0x0058ed70 free
                jo::self<jo::Game>(this)->scenario.squads = nullptr;
                jo::self<jo::Game>(this)->scenario.squad_count = 0;
            }
            for (std::int32_t n = joan::orig::as_dynarray(obj(d::g_units))->size(); n > 0; --n) {  // 0x00417ad0
                std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(n)));  // 0x0041b3f0
                if (u != nullptr) {
                    jo::as_unit(u)->kills = 0;
                    jo::as_unit(u)->kills_high_rank = 0;
                    jo::as_unit(u)->squad_kills = 0;
                    jo::as_unit(u)->squad_losses = 0;
                    jo::view<std::int16_t>(jo::as_unit(u)->crit_hits) = 0;
                    jo::view<std::int16_t>(jo::as_unit(u)->crit_kills) = 0;
                }
            }
        }
        joan::orig::as_scenario(&jo::self<jo::Game>(this)->scenario)->reset();  // 0x004cc850
        global<std::uint8_t>(d::g_a4d765) = 0;
        jo::self<jo::Game>(this)->end_state = 0;
        jo::self<jo::Game>(this)->arrays_loaded = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_06", name, addr, fn)
B("Game::end_session", 0x00471010, &H::end_session);
#undef B

}  // namespace game_b83_06
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
//
//    - List::renumber 0x00477b10 (n) ret 4, Ending::start 0x00471f90 (i) ret 4,
//      ModelSlot::add_ref 0x00419cf0, AnimResMgr::add_ref 0x00417af0 (id) ret 4,
//      AnimSet::add_ref 0x004152b0, scalar deleting dtor 0x00471530, Obj::set_pos 0x00456fe0
//      (V3) ret 0xc, ModelObj::ctor 0x0041a1b0, AnimResMgr::idle 0x00417db0 (id) ret 4,
//      min 0x0046f020 (cdecl), Obj::ctor 0x00456eb0, ModelSlot::idle 0x00419ff0,
//      Names::at 0x00464100 (i) ret 4, setters 0x004725f0 / 0x004571b0, 0x00418c30.
namespace {
namespace game_b84_05 {

using joan::orig::helpers::game_delete;                      // operator delete

JOAN_COUNTER(c_ending, "Ending::start");

struct H {
    void set_744(char v) { jo::view<char>(jo::self<jo::Game>(this)->cutscene_flag) = v; }               // 0x004725f0
    void* deleting_dtor_471530(std::uint32_t flags) {  // 0x00471530
        joan::orig::as_unit(this)->destruct();  // 0x00416580
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }

    // ---- 0x00471f90 Ending::start(i) ----------------------------------------------------------
    std::int32_t ending_start(std::int32_t i) {
        c_ending.hit();
        if (!(static_cast<std::int32_t>(jo::self<jo::Game>(this)->end_condition_count) > 0)) return 0;
        joan::orig::as_hud(obj(d::g_player_hud))->remove_slot(joan::orig::arg::u32(i));  // 0x007072c0
        joan::orig::as_end_condition(&jo::self<jo::Game>(this)->end_conditions[i])->release();  // 0x00467260
        return static_cast<std::int32_t>(jo::self<jo::Game>(this)->end_condition_count);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("Ending::start", 0x00471f90, &H::ending_start);
B("Obj::deleting_dtor_471530", 0x00471530, &H::deleting_dtor_471530);
B("Obj::set_744", 0x004725f0, &H::set_744);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
//
//    - Obj::wait_4715a0 0x004715a0 (ret 4, no references found): 0x0042ca90 loop on 0xa50088
//      while byte [0xa50098] == 4. Returns movsx [0xa50098].
namespace {
namespace game_b108_19 {

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x004715a0
    i32 wait_4715a0(u32 arg) {
        if (global<i8>(d::g_a50098) != 0) return global<i8>(d::g_a50098);
        global<i8>(d::g_a50098) = 0;
        for (;;) {
            joan::orig::as_game(obj(d::g_game_a50088))->on_mode(joan::orig::arg::i32(arg));  // 0x0042ca90
            const i32 v = global<i8>(d::g_a50098);
            if (v != 4) return v;
            arg = 1;
            global<i8>(d::g_a50098) = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Obj::wait_4715a0", 0x004715a0, &H::wait_4715a0);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b109_12.cpp (game/b109_12)
//
//    - Mission::tick_471620 0x00471620 (thiscall, ret 0; called on 0xa58f90): timers at +0x304 /
//      +0x2b4[4][5] count down by [0xa500be] (unsigned, clamped at 0), active objective virtual
//      +0xc(0), objective table (+0x309 entries of 0x1a bytes at +0x30b) completion / failure,
//      +0x73c delay vs [0xa500ba], final 0x006e7b50 / 0x006e7900 on 0xa40470. eax unused by caller.
namespace {
namespace game_b109_12 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

JOAN_COUNTER(c_tick, "Mission::tick_471620");

struct H {
    // ---- 0x00471620 -------------------------------------------------------------------------
    void tick() {
        c_tick.hit();
        void* const self = this;
        if (joan::orig::arg::as<i32>(joan::orig::as_scenario(&jo::as_game(self)->scenario)->mode()) == 2) {  // 0x004cd510
            if (jo::as_game(self)->mode2_timer > global<u32>(d::g_frame_step_ms))
                jo::as_game(self)->mode2_timer = jo::as_game(self)->mode2_timer - global<u32>(d::g_frame_step_ms);
            else
                jo::as_game(self)->mode2_timer = 0;
        } else {
            for (i32 i = 0; i < 4; ++i) {
                for (i32 j = 0; j < 5; ++j) {
                    u32& e = jo::as_game(self)->mode_timers[i * 5 + j];
                    if (e > global<u32>(d::g_frame_step_ms))
                        e = e - global<u32>(d::g_frame_step_ms);
                    else
                        e = 0;
                }
            }
        }
        if (global<i8>(d::g_game_mode) != 5 && global<i8>(d::g_flag_a59298) != 2) {
            if (jo::as_game(self)->scenario.id != 0) {
                jo::ScenarioEntry* const slot = &jo::as_game(self)->scenario.entries[jo::view<i32>(jo::as_game(self)->scenario.index) - 1];
                if (slot->script != nullptr) virtual_call<void>(jo::view<u8*>(slot->script), 0xc, i32{0});
            } else if (jo::as_game(self)->scenario.local.script != nullptr) {
                virtual_call<void>(jo::view<u8*>(jo::as_game(self)->scenario.local.script), 0xc, i32{0});
            }
        }
        if (jo::as_game(self)->scenario.id != 0 && jo::as_game(self)->scenario.index != jo::as_game(self)->scenario.target_stage) {
            joan::orig::as_battle(obj(d::g_scenarios))->load_scenario_41cc80(joan::orig::arg::i32(jo::as_game(self)->scenario.id), joan::orig::arg::i32(jo::as_game(self)->scenario.target_stage), i32{0});  // 0x0041cc80
            joan::orig::as_battle(obj(d::g_scenarios))->start_scenario();                                                   // 0x0041c810
            joan::orig::as_game(obj(d::g_game_a50088))->set_state(joan::orig::arg::u32(i32{9}));                                           // 0x004d7e70
            return;
        }
        joan::orig::as_timer(&jo::as_game(self)->scenario)->tick();  // 0x004cd5d0
        auto ent = [self](i32 i) { return &jo::as_game(self)->end_conditions[i]; };
        auto count = [self]() { return static_cast<i32>(jo::as_game(self)->end_condition_count); };
        if (jo::view<i8>(jo::as_game(self)->end_state) == 0 && jo::as_game(self)->end_condition_count != 0) {
            bool all = true;
            for (i32 i = 0; i < count(); ++i) {
                jo::EndCon* const e = ent(i);
                if (e->text == nullptr) continue;
                if (e->state != 2 && e->state != 0) continue;
                if (e->result != 1 || e->rewarded != 0) continue;
                e->rewarded = 1;
                if (e->reward_gold != 0) global<u32>(d::g_get_a596c4) = global<u32>(d::g_get_a596c4) + e->reward_gold;
                if (e->reward_exp == 0) continue;
                for (i32 k = joan::orig::as_dynarray(obj(d::g_units))->size(); k > 0; --k) {  // 0x00417ad0
                    u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(k)));  // 0x0041b3f0
                    if (u == nullptr) continue;
                    if (jo::as_unit(u)->rank < 4) continue;
                    if (joan::orig::arg::as<i32>(joan::orig::unit::is_hero(joan::orig::arg::i32(k))) == 0) continue;  // 0x0042ff90
                    joan::orig::as_unit(u)->add_experience(joan::orig::arg::i32(e->reward_exp), static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x005229a0
                }
            }
            for (i32 i = 0; i < count(); ++i) {
                jo::EndCon* const e = ent(i);
                if (e->used == 0) continue;
                if (e->text == nullptr) continue;
                if (e->state != 0) continue;
                if (e->result != 1) {
                    all = false;
                    break;
                }
            }
            for (i32 i = 0; i < count(); ++i) {
                jo::EndCon* const e = ent(i);
                if (e->text == nullptr) continue;
                if (e->used == 0) continue;
                if (e->state == 1 && e->result == 2) {
                    jo::as_game(self)->end_state = 2;
                    all = false;
                }
            }
            if (all) {
                if (jo::as_game(self)->end_since == 0) jo::as_game(self)->end_since = global<u32>(d::g_now);
            } else {
                jo::as_game(self)->end_since = 0;
            }
        }
        if (jo::view<i8>(jo::as_game(self)->end_state) != 0) return;
        if (jo::as_game(self)->end_since == 0) return;
        if (global<u32>(d::g_now) - jo::as_game(self)->end_since < jo::as_game(self)->end_delay) return;
        if (joan::orig::arg::as<i8>(joan::orig::as_game(self)->flag_744()) != 0) return;  // 0x004301c0
        jo::as_game(self)->end_state = 1;
        for (i32 i = 0; i < count(); ++i) {
            jo::EndCon* const e = ent(i);
            if (e->used == 0) continue;
            if (e->state == 3) e->result = 1;
        }
        if (jo::as_game(self)->scenario.id != 5 && jo::as_game(self)->end_condition_count != 0) {
            for (i32 i = 0; i < count(); ++i) {
                jo::EndCon* const e = ent(i);
                if (e->used == 0) continue;
                if (e->text == nullptr) continue;
                if (e->state == 2 && e->result == 0) e->result = 2;
            }
        }
        if (joan::orig::arg::as<i8>(joan::orig::as_glory_screen(obj(d::g_glory_screen))->is_open()) == 0)  // 0x006e7b50
            joan::orig::as_screen(obj(d::g_glory_screen))->win_6e7900();       // 0x006e7900
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_12", name, addr, fn)
B("Mission::tick_471620", 0x00471620, &H::tick);
#undef B

}  // namespace game_b109_12
}  // namespace

// ---------------------------------------- from modules/game_b100_03.cpp (game/b100_03)
//
//      0x00471e20 SlotOwner::add_slot        (thiscall, ret 0) - word count +0x309 (< 0x28),
//                 memset 0x1a slot, slot word +0xf = 1, 0x007078c0(0xa4d618, count - 1).
//
//      0x00472150 Player::set_flag_bit       (thiscall(n, byte v), ret 8)
//
//      0x00472370 Player::flag_bit           (thiscall(n), ret 4) - returns al.
//
//      0x004728f0 Game::copy_word_table      (thiscall, ret 0) - 0x004ca290(0xa47ca8), realloc
//                 +0x71c to (count + 10) words, memcpy count words.
namespace {
namespace game_b100_03 {

JOAN_COUNTER(c_add_slot, "SlotOwner::add_slot");
JOAN_COUNTER(c_set_flag_bit, "Player::set_flag_bit");
JOAN_COUNTER(c_copy_words, "Game::copy_word_table");

using joan::orig::helpers::memset_;        // memset
using joan::orig::helpers::memcpy_;  // memcpy

struct H {

    // ------------------------------------------------------------------------ 0x00471e20
    i32 add_slot() {
        c_add_slot.hit();
        if (static_cast<i32>(jo::self<jo::Game>(this)->end_condition_count) >= 0x28) return 0;
        memset_(&jo::self<jo::Game>(this)->end_conditions[jo::self<jo::Game>(this)->end_condition_count], 0, static_cast<u32>(sizeof(jo::EndCon)));
        jo::self<jo::Game>(this)->end_conditions[jo::self<jo::Game>(this)->end_condition_count].used = 1;
        jo::self<jo::Game>(this)->end_condition_count = static_cast<i16>(jo::self<jo::Game>(this)->end_condition_count + 1);
        joan::orig::as_hud(obj(d::g_player_hud))->append_log_entry(joan::orig::arg::i32(static_cast<i32>(jo::self<jo::Game>(this)->end_condition_count) - 1));  // 0x007078c0
        return static_cast<i32>(jo::self<jo::Game>(this)->end_condition_count);
    }

    // ------------------------------------------------------------------------ 0x00472150
    void set_flag_bit(i32 n, u32 v) {
        c_set_flag_bit.hit();
        const u32 sh = static_cast<u32>(n - 1) & 31u;
        const u32 mask = ~(1u << sh);
        const u32 bits = (v & 0xffu) << sh;
        if (jo::self<jo::Game>(this)->scenario.id != 0) {
            jo::ScenarioEntry* const rec = &jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1];
            rec->u32_18e = (rec->u32_18e & mask) | bits;
        } else {
            jo::self<jo::Game>(this)->scenario.local.u32_18e = (jo::self<jo::Game>(this)->scenario.local.u32_18e & mask) | bits;
        }
    }

    // ------------------------------------------------------------------------ 0x00472370
    u8 flag_bit(i32 n) {
        const u32 sh = static_cast<u32>(n - 1) & 31u;
        if (jo::self<jo::Game>(this)->scenario.id != 0) {
            jo::ScenarioEntry* const rec = &jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1];
            return static_cast<u8>((jo::view<i32>(rec->u32_192) >> sh) & 1);
        }
        return static_cast<u8>((jo::view<i32>(jo::self<jo::Game>(this)->scenario.local.u32_192) >> sh) & 1);
    }

    // ------------------------------------------------------------------------ 0x004728f0
    void copy_word_table() {
        c_copy_words.hit();
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
        if (p == nullptr) return;
        jo::self<jo::Game>(this)->word_set_count = jo::as_profile(p)->list_a_count;
        jo::self<jo::Game>(this)->word_set_cap = jo::as_profile(p)->list_a_count + 10;
        const i32 line = line_of(d::g_9fed70, 8);
        jo::view<void*>(jo::self<jo::Game>(this)->word_set) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Game>(this)->word_set)), joan::orig::arg::u32(jo::self<jo::Game>(this)->word_set_cap << 1), str(d::s_d_ww_game_cpp), joan::orig::arg::i32(line));  // 0x0058eb30 "d:\ww\game.cpp"
        memcpy_(jo::view<void*>(jo::self<jo::Game>(this)->word_set), jo::view<void*>(jo::as_profile(p)->list_a), static_cast<u32>(jo::self<jo::Game>(this)->word_set_count << 1));
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_03", name, addr, fn)
B100("SlotOwner::add_slot", 0x00471e20, &H::add_slot);
B100("Player::set_flag_bit", 0x00472150, &H::set_flag_bit);
B100("Player::flag_bit", 0x00472370, &H::flag_bit);
B100("Game::copy_word_table", 0x004728f0, &H::copy_word_table);
#undef B100

}  // namespace game_b100_03
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
//
//      0x00471ef0 Obj::pop_entry                (thiscall, ret 0) - word +0x309 count, 0x007072c0 /
//
//      0x004724c0 Obj::get_bit(n)               (thiscall, ret 4) - al = bit n-1 (upper eax as left).
//
//      0x00472c40 Obj::pull_buffer              (thiscall, ret 0) - reverse of 0x00472b00.
namespace {
namespace game_b98_02 {

JOAN_COUNTER(c_pop_entry, "Obj::pop_entry");
JOAN_COUNTER(c_pull_buffer, "Obj::pull_buffer");

using joan::orig::helpers::crt_memcpy;

struct H {
    // ------------------------------------------------------------------ 0x00471ef0
    i32 pop_entry() {
        c_pop_entry.hit();
        if (!(static_cast<i32>(jo::self<jo::Game>(this)->end_condition_count) > 0)) return 0;
        joan::orig::as_hud(obj(d::g_player_hud))->remove_slot(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Game>(this)->end_condition_count) - 1));  // 0x007072c0
        joan::orig::as_end_condition(&jo::self<jo::Game>(this)->end_conditions[static_cast<i32>(jo::self<jo::Game>(this)->end_condition_count) - 1])->release();  // 0x00467260
        jo::self<jo::Game>(this)->end_condition_count = static_cast<i16>(jo::self<jo::Game>(this)->end_condition_count - 1);
        return static_cast<i32>(jo::self<jo::Game>(this)->end_condition_count);
    }

    // ------------------------------------------------------------------ 0x004724c0
    u32 get_bit(i32 n) {
        const u32 sh = static_cast<u32>(n - 1) & 31u;
        if (jo::view<i32>(jo::self<jo::Game>(this)->scenario.id) != 0) {
            const u32 v = jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1].u32_196;
            return (static_cast<u32>(n - 1) & 0xffffff00u) | ((v >> sh) & 1u);
        }
        return (jo::self<jo::Game>(this)->scenario.local.u32_196 >> sh) & 1u;
    }

    // ------------------------------------------------------------------ 0x00472c40
    std::uintptr_t pull_buffer() {
        c_pull_buffer.hit();
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
        if (rec == nullptr) return 0;
        jo::self<jo::Game>(this)->word_stack_count = jo::as_profile(rec)->list_b_count;
        jo::self<jo::Game>(this)->word_stack_cap = jo::as_profile(rec)->list_b_count + 10;
        const i32 line = static_cast<i32>(global<i16>(d::g_9fed7c)) + 8;
        jo::view<void*>(jo::self<jo::Game>(this)->word_stack) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Game>(this)->word_stack)), joan::orig::arg::u32(jo::self<jo::Game>(this)->word_stack_cap << 1), str(d::s_d_ww_game_cpp), joan::orig::arg::i32(line));  // 0x0058eb30
        return u(crt_memcpy(jo::view<void*>(jo::self<jo::Game>(this)->word_stack), jo::view<void*>(jo::as_profile(rec)->list_b), static_cast<u32>(jo::self<jo::Game>(this)->word_stack_count << 1)));
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::pop_entry", 0x00471ef0, &H::pop_entry);
B98("Obj::get_bit_4724c0", 0x004724c0, &H::get_bit);
B98("Obj::pull_buffer", 0x00472c40, &H::pull_buffer);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
//
//      0x004720d0 Player::flag_bit(n)            (thiscall, ret 4)   - al = bit n-1.
//
//      0x00472620 ShortSet::add(short)           (thiscall, ret 4)   - grows by 10 (0x0058eb30).
//
//  - eax on exit is reproduced also where the callers probably ignore it (this / last callee
//    result / argument), e.g. 0x00472620, 0x00492e20, 0x004b0480, 0x004b0640.
//
//  - 0x004720d0: the `mov al, dl` path keeps the upper 24 bits of (n - 1) in eax; shifts use
//    cl & 31 like `sar r32, cl`.
namespace {
namespace game_b95_02 {

JOAN_COUNTER(c_short_add, "ShortSet::add");

struct H {
    // ------------------------------------------------------------------ 0x004720d0
    u32 flag_bit(i32 n) {
        const u32 sh = static_cast<u32>(n - 1) & 31;
        if (jo::self<jo::Game>(this)->scenario.id != 0) {
            const u32 bits = jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1].u32_18a;
            const u32 bit = static_cast<u32>(static_cast<i32>(bits) >> sh) & 1;
            return (static_cast<u32>(n - 1) & 0xffffff00u) | bit;
        }
        return static_cast<u32>(jo::view<i32>(jo::self<jo::Game>(this)->scenario.local.u32_18a) >> sh) & 1;
    }

    // ------------------------------------------------------------------ 0x00472620
    i32 short_add(i16 v) {
        c_short_add.hit();
        for (i32 i = 0; i < jo::self<jo::Game>(this)->word_set_count; ++i) {
            const i32 e = jo::self<jo::Game>(this)->word_set[i].value;
            if (e == static_cast<i32>(v)) return e;
        }
        if (!(jo::self<jo::Game>(this)->word_set_cap > jo::self<jo::Game>(this)->word_set_count)) {
            jo::self<jo::Game>(this)->word_set_cap = jo::self<jo::Game>(this)->word_set_cap + 10;
            jo::view<void*>(jo::self<jo::Game>(this)->word_set) = joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Game>(this)->word_set)), joan::orig::arg::u32(jo::self<jo::Game>(this)->word_set_cap << 1), str(d::s_d_ww_game_cpp), joan::orig::arg::i32(global<i16>(d::g_9fed68) + 0xb));  // 0x0058eb30
        }
        jo::self<jo::Game>(this)->word_set[jo::self<jo::Game>(this)->word_set_count].value = v;
        const i32 c = jo::self<jo::Game>(this)->word_set_count + 1;
        jo::self<jo::Game>(this)->word_set_count = c;
        return c;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Player::flag_bit", 0x004720d0, &H::flag_bit);
B95("ShortSet::add", 0x00472620, &H::short_add);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
//
//      0x00472220 Player::flag_bit(n)          (thiscall, ret 4)    - bit n-1 of +0x18e of the
//                 0x1b2 record (+0x27)[+0x1b - 1] when +0x17, else of +0x1b9 (byte result).
//
//      0x004727b0 Game::copy_words_to          (thiscall, ret 0)    - 0x004ca290 target, realloc /
//                 free +0x2c, copies +0x71c (count +0x720 words).
//
//  - 0x00472220 returns its result in al; eax's upper bytes are reproduced too (those of n-1 on
//    the record path, 0 on the +0x1b9 path).
namespace {
namespace game_b96_02 {

JOAN_COUNTER(c_copy_words, "Game::copy_words_to");

struct H {
    // ------------------------------------------------------------------------ 0x00472220
    u32 flag_bit(i32 n) {
        const u32 sh = static_cast<u32>(n - 1);
        if (jo::self<jo::Game>(this)->scenario.id != 0) {
            const u32 v = jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1].u32_18e;
            return (sh & 0xffffff00u) | ((v >> (sh & 31u)) & 1u);
        }
        return (jo::self<jo::Game>(this)->scenario.local.u32_18e >> (sh & 31u)) & 1u;
    }

    // ------------------------------------------------------------------------ 0x004727b0
    void copy_words_to() {
        c_copy_words.hit();
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
        if (p == nullptr) return;
        if (jo::view<u32>(jo::self<jo::Game>(this)->word_set_count) != 0) {
            const i32 line = line_of(d::g_9fed6c, 8);
            jo::view<void*>(jo::as_profile(p)->list_a) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::as_profile(p)->list_a)), joan::orig::arg::u32(jo::self<jo::Game>(this)->word_set_count << 1), str(d::s_d_ww_game_cpp), joan::orig::arg::i32(line));  // 0x0058eb30 "game.cpp"
            jo::view<u32>(jo::as_profile(p)->list_a_count) = jo::view<u32>(jo::self<jo::Game>(this)->word_set_count);
            std::memcpy(joan::orig::arg::mptr(jo::view<void*>(jo::as_profile(p)->list_a)), joan::orig::arg::ptr(jo::view<void*>(jo::self<jo::Game>(this)->word_set)), joan::orig::arg::u32(jo::self<jo::Game>(this)->word_set_count << 1));  // memmove
        } else if (jo::as_profile(p)->list_a != nullptr) {
            jo::view<u32>(jo::as_profile(p)->list_a_count) = jo::view<u32>(jo::self<jo::Game>(this)->word_set_count);
            joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(jo::view<void*>(jo::as_profile(p)->list_a)), str(d::s_d_ww_game_cpp), joan::orig::arg::i32(line_of(d::g_9fed6c, 0x11)));  // 0x0058ed70
            jo::as_profile(p)->list_a = nullptr;
        } else {
            jo::view<u32>(jo::as_profile(p)->list_a_count) = jo::view<u32>(jo::self<jo::Game>(this)->word_set_count);
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("Player::flag_bit", 0x00472220, &H::flag_bit);
B96("Game::copy_words_to", 0x004727b0, &H::copy_words_to);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
//
//      0x004722a0 Obj::set_bit           (thiscall(n, byte), ret 8)
namespace {
namespace game_b94_01 {

struct H {
    // 0x004722a0
    void set_bit(i32 n, i32 v) {
        const u32 sh = static_cast<u32>(n - 1) & 31u;
        const u32 mask = ~(1u << sh);
        const u32 bits = (static_cast<u32>(v) & 0xffu) << sh;
        if (jo::view<i32>(jo::self<jo::Game>(this)->scenario.id) != 0) {
            jo::ScenarioEntry* const e = &jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1];
            e->u32_192 = (e->u32_192 & mask) | bits;
        } else {
            jo::self<jo::Game>(this)->scenario.local.u32_192 = (jo::self<jo::Game>(this)->scenario.local.u32_192 & mask) | bits;
        }
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::set_bit", 0x004722a0, &H::set_bit);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
//
//      0x004723f0 Obj::set_bit(n, v)            (thiscall, ret 8) - bit n-1 of +0x1c1 or of the
//                 record [+0x27] + ([+0x1b]-1)*0x1b2 + 0x196 when +0x17 != 0.
//
//      0x00472b00 Obj::push_buffer              (thiscall, ret 0) - copy +0x728 (+0x72c words) to the
//
//  - 0x004723f0: x86 shifts mask cl to 5 bits; reproduced with & 31. eax (the stored value) is
//    returned.
//
//  - 0x00472b00 / 0x0047be60 / 0x0047c250 / 0x0049f830 / 0x004af950: eax as the original leaves it
//    is returned (memcpy result / callee result / loop index).
namespace {
namespace game_b97_02 {

JOAN_COUNTER(c_push_buffer, "Obj::push_buffer");

using joan::orig::helpers::crt_memcpy;

struct H {
    // ------------------------------------------------------------------ 0x004723f0
    std::uintptr_t set_bit(i32 n, u32 v) {
        const u32 sh = static_cast<u32>(n - 1) & 31u;
        const u32 mask = ~(1u << sh);
        const u32 bits = (v & 0xffu) << sh;
        if (jo::view<i32>(jo::self<jo::Game>(this)->scenario.id) != 0) {
            u32& slot = jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1].u32_196;
            const u32 r = (slot & mask) | bits;
            slot = r;
            return r;
        }
        const u32 r = (jo::self<jo::Game>(this)->scenario.local.u32_196 & mask) | bits;
        jo::self<jo::Game>(this)->scenario.local.u32_196 = r;
        return r;
    }

    // ------------------------------------------------------------------ 0x00472b00
    std::uintptr_t push_buffer() {
        c_push_buffer.hit();
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
        if (rec == nullptr) return 0;
        if (jo::self<jo::Game>(this)->word_stack_count != 0) {
            const i32 line = static_cast<i32>(global<i16>(d::g_9fed78)) + 8;
            jo::view<void*>(jo::as_profile(rec)->list_b) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::as_profile(rec)->list_b)), joan::orig::arg::u32(jo::self<jo::Game>(this)->word_stack_count << 1), str(d::s_d_ww_game_cpp), joan::orig::arg::i32(line));  // 0x0058eb30
            jo::as_profile(rec)->list_b_count = jo::self<jo::Game>(this)->word_stack_count;
            return u(crt_memcpy(jo::view<void*>(jo::as_profile(rec)->list_b), jo::view<void*>(jo::self<jo::Game>(this)->word_stack), static_cast<u32>(jo::self<jo::Game>(this)->word_stack_count << 1)));
        }
        if (jo::view<void*>(jo::as_profile(rec)->list_b) != nullptr) {
            jo::as_profile(rec)->list_b_count = jo::self<jo::Game>(this)->word_stack_count;
            const i32 line = static_cast<i32>(global<i16>(d::g_9fed78)) + 0x11;
            joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(jo::view<void*>(jo::as_profile(rec)->list_b)), str(d::s_d_ww_game_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::view<void*>(jo::as_profile(rec)->list_b) = nullptr;
            return u(rec);
        }
        jo::as_profile(rec)->list_b_count = jo::self<jo::Game>(this)->word_stack_count;
        return static_cast<u32>(jo::self<jo::Game>(this)->word_stack_count);
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Obj::set_bit_4723f0", 0x004723f0, &H::set_bit);
B97("Obj::push_buffer", 0x00472b00, &H::push_buffer);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b103_02.cpp (game/b103_02)
//
//      0x004725a0 Obj::current_entry               (thiscall, ret 0) - [+0x27] + ([+0x1b]-1)*0x1b2
//                 or this + 0x2b.
namespace {
namespace game_b103_02 {

struct H {

    // ------------------------------------------------------------------ 0x004725a0
    u8* current_entry() {
        if (jo::view<i32>(jo::self<jo::Game>(this)->scenario.id) != 0) return reinterpret_cast<u8*>(&jo::self<jo::Game>(this)->scenario.entries[jo::view<i32>(jo::self<jo::Game>(this)->scenario.index) - 1]);
        return reinterpret_cast<u8*>(&jo::self<jo::Game>(this)->scenario.local);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("Obj::current_entry", 0x004725a0, &H::current_entry);
#undef B

}  // namespace game_b103_02
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
//
//    - Obj::has_word 0x00472740 (short), ret 4 -> bool: words [+0x71c][0..[+0x720]).
namespace {
namespace game_b90_03 {

struct H {
    // 0x00472740
    bool has_word(std::int16_t v) {
        for (std::int32_t i = 0; i < jo::self<jo::Game>(this)->word_set_count; ++i) {
            if (static_cast<std::int32_t>(jo::view<std::int16_t*>(jo::self<jo::Game>(this)->word_set)[i]) == static_cast<std::int32_t>(v)) return true;
        }
        return false;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::has_word", 0x00472740, &H::has_word);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
//
//      0x0046f090 Game::Game_46f090             (thiscall, ret 0) - SEH frame; vector ctor + memset.
//
//  - 0x0046f090: the eh vector constructor iterator 0x007b2f98 is stdcall (ret 0x14); the
//    original memsets the whole 0x745 bytes after constructing the members (kept).
//
//  - SEH frames (0x0045f7f0, 0x00469600, 0x0046f090, 0x00487640, 0x004877e0): only the
//    normal path is reproduced (no game code here throws).
namespace {
namespace game_b99_02_2 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_game_ctor, "Game::Game_46f090");

void* crt_memset(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // ------------------------------------------------------------------ 0x0046f090
    void* game_ctor() {
        c_game_ctor.hit();
        joan::orig::as_scenario(&jo::self<jo::Game>(this)->scenario)->construct();  // 0x004cc7c0
        // eh vector constructor iterator (stdcall): 0x28 elements of 0x1a bytes (sizeof EndCon on x86); ctor / dtor
        // are code pointers.
        reinterpret_cast<void(JOAN_STDCALL*)(void*, u32, i32, void*, void*)>(fn::code_value(fn::crt_eh_vector_constructor_iterator))(
            jo::self<jo::Game>(this)->end_conditions, static_cast<u32>(sizeof(jo::EndCon)), 0x28, fn::code_ptr(fn::Obj_init_4671f0) /* 0x004671f0 */, fn::code_ptr(fn::Obj_call_467260_467230) /* 0x00467230 */);
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::Game)));  // 0x745 on x86
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Game::Game_46f090", 0x0046f090, &H::game_ctor);
#undef B99

}  // namespace game_b99_02_2
}  // namespace

// ---------------------------------------- from modules/game_b100_03.cpp (game/b100_03)
//
//      0x0046f130 SlotOwner::dtor            (thiscall, ret 0) - SEH frame (normal path only):
//                 0x0046f200(this), eh vector dtor iterator 0x007b3054(this+0x30b, 0x1a, 0x28,
//                 0x00409e0d), 0x004cc800(this+0x17).
//
//      0x0046f1c0 SlotOwner::reset           (thiscall, ret 0) - byte flag at +0 set -> 0x0046f200.
namespace {
namespace game_b100_03_2 {

using joan::call::thiscall;
using joan::call::stdcall_call;
using i8 = std::int8_t;

JOAN_COUNTER(c_slot_dtor, "SlotOwner::dtor");

struct H {
    // ------------------------------------------------------------------------ 0x0046f130
    void slot_dtor() {
        c_slot_dtor.hit();
        joan::orig::as_game(this)->shutdown_46f200();  // 0x0046f200
        joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Game>(this)->end_conditions)), static_cast<std::uint32_t>(sizeof(jo::EndCon)), 0x28, fn::code_value(fn::Obj_call_467260_467230));  // eh vector dtor (0x00467230)
        joan::orig::as_scenario(&jo::self<jo::Game>(this)->scenario)->reset_wrap();  // 0x004cc800
    }

    // ------------------------------------------------------------------------ 0x0046f1c0
    void slot_reset() {
        if (jo::view<i8>(jo::self<jo::Game>(this)->active) != 0) joan::orig::as_game(this)->shutdown_46f200();  // 0x0046f200
        jo::self<jo::Game>(this)->active = 1;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_03", name, addr, fn)
B100("SlotOwner::dtor", 0x0046f130, &H::slot_dtor);
B100("SlotOwner::reset", 0x0046f1c0, &H::slot_reset);
#undef B100

}  // namespace game_b100_03_2
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x00472eb0 Obj::set_b16            (thiscall(v), ret 4)
namespace {
namespace game_b94_02 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x00472eb0
    void set_b16(u32 v) { jo::view<u32>(jo::self<jo::Players>(this)->time_scale_) = v; }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::set_b16", 0x00472eb0, &H::set_b16);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
//
//      0x00472a90 ShortList::contains(short)                   thiscall, ret 4, al
namespace {
namespace game_b91_02 {

struct H {
    // 0x00472a90
    std::uint8_t contains(std::int16_t v) {
        for (std::int32_t i = 0; i < jo::self<jo::Game>(this)->word_stack_count; ++i)
            if (jo::self<jo::Game>(this)->word_stack[i].value == v) return 1;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("ShortList::contains", 0x00472a90, &H::contains);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// 0x00472d10 play_sound(id): 0x0065bed0(0x004dd0e0(id), 0x004dd120(id), 1) - 0x004dd120 first.
// thiscall (this unused), ret 4.
struct HS {
    std::uint32_t play_sound(std::uint32_t id) {
        const char* const a = joan::orig::as_txt_res(obj(d::g_strings))->autosave_text_2(joan::orig::arg::u32(id));  // 0x004dd120
        const char* const b = joan::orig::as_txt_res(obj(d::g_strings))->autosave_text_1(joan::orig::arg::u32(id));  // 0x004dd0e0
        return joan::orig::arg::as<std::uint32_t>(joan::orig::save_list::add(joan::orig::arg::str(b), joan::orig::arg::str(a), 1u));                              // 0x0065bed0
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("play_sound", 0x00472d10, &HS::play_sound);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b105_03.cpp (game/b105_03)
namespace {
namespace game_b105_03 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

// ---------------------------------------------------------------- CRT initialisers
template <fn::Code Callee>
u32 JOAN_CDECL init_call() {
    return cdecl_call<u32>(Callee);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_03", name, addr, fn)
// CRT initialisers
B("init_472d60", 0x00472d60, &init_call<fn::init_pi_472d80>);
B("init_472dc0", 0x00472dc0, &init_call<fn::init_a39170>);
B("init_472e10", 0x00472e10, &init_call<fn::init_a39174>);
B("init_472e60", 0x00472e60, &init_call<fn::init_a39178>);
#undef B

}  // namespace game_b105_03
}  // namespace

// ---------------------------------------- from modules/game_b94_01_2.cpp (game/b94_01)
namespace {
namespace game_b94_01_2 {

using joan::call::cdecl_call;

using joan::hook::global;

using joan::hook::x87::Fpu;

// 0x00472d80: (float)(atan(1.0) * 4.0) (CRT atan st0, fmul qword).
static void JOAN_CDECL init_pi() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));
    global<float>(d::g_a3916c) = static_cast<float>(Fpu{true}.mul(a, global<double>(d::dbl_4)));
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("init_pi", 0x00472d80, &init_pi);
#undef B94

}  // namespace game_b94_01_2
}  // namespace

// ---------------------------------------- from modules/game_b99_02_3.cpp (game/b99_02)
namespace {
namespace game_b99_02_3 {

using joan::hook::global;

void JOAN_CDECL init_a39170() { global<float>(d::g_a39170) = global<float>(d::g_a3916c) / global<float>(d::flt_2); }

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("init_a39170", 0x00472de0, &init_a39170);
#undef B99

}  // namespace game_b99_02_3
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02_2 {

using joan::hook::global;

// ---------------------------------------------------------------- static initialisers
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a39174() { div_global(d::g_a39174, d::g_a3916c, d::flt_3); }  // 0x00472e30

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("init_a39174", 0x00472e30, &init_a39174);
#undef B95

}  // namespace game_b95_02_2
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02_2 {

using joan::hook::global;

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL init_a39178() {  // 0x00472e80
    global<float>(d::g_a39178) = global<float>(d::g_a3916c) / global<float>(d::flt_4);  // 4.0f
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("init_a39178", 0x00472e80, &init_a39178);
#undef B96

}  // namespace game_b96_02_2
}  // namespace

// ---------------------------------------- from modules/game_b100_03.cpp (game/b100_03)
namespace {
namespace game_b100_03_3 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using joan::hook::global;

using joan::img::str;

using i32 = std::int32_t;

JOAN_COUNTER(c_difficulty, "load_difficulty_settings");

// ------------------------------------------------------------------------------- cdecl
// 0x00472ee0
void JOAN_CDECL load_difficulty() {
    c_difficulty.hit();
    void* const ini = obj(d::g_steer);
    auto value = [ini](img::Addr section, img::Addr key) -> double {
        void* const sec = joan::orig::arg::as<void*>(joan::orig::as_ini(ini)->section(joan::orig::arg::str(str(section))));             // 0x0047cbf0
        char* const e = joan::orig::arg::as<char*>(joan::orig::as_ini(sec)->value(joan::orig::arg::str(str(key))));                  // 0x0047c180
        return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(static_cast<const char*>(e + 9))));      // atof
    };
    global<i32>(d::g_a39180) = joan::hook::x87::ftol(value(d::s_MUSIC_9d3508, d::s_BATTLE_R));  // MUSIC BATTLE_R
    global<float>(d::g_a391ac) = static_cast<float>(value(d::s_GAME, d::s_EASY_D));   // GAME EASY_D
    global<float>(d::g_a3918c) = static_cast<float>(value(d::s_GAME, d::s_NORM_D));   // NORM_D
    global<float>(d::g_a391b0) = static_cast<float>(value(d::s_GAME, d::s_DIFF_D));   // DIFF_D
    global<float>(d::g_a39194) = static_cast<float>(value(d::s_GAME, d::s_VDIFF_D));   // VDIFF_D
    global<float>(d::g_a39184) = static_cast<float>(value(d::s_GAME, d::s_EASY_A));   // EASY_A
    global<float>(d::g_a3919c) = static_cast<float>(value(d::s_GAME, d::s_NORM_A));   // NORM_A
    global<float>(d::g_a391a8) = static_cast<float>(value(d::s_GAME, d::s_DIFF_A));   // DIFF_A
    global<float>(d::g_a39190) = static_cast<float>(value(d::s_GAME, d::s_VDIFF_A));   // VDIFF_A
    global<float>(d::g_a391a4) = static_cast<float>(value(d::s_GAME, d::s_EASY_A));   // EASY_A
    global<float>(d::g_a39188) = static_cast<float>(value(d::s_GAME, d::s_NORM_A));   // NORM_A
    global<float>(d::g_a391a0) = static_cast<float>(value(d::s_GAME, d::s_DIFF_A));   // DIFF_A
    global<float>(d::g_a39198) = static_cast<float>(value(d::s_GAME, d::s_VDIFF_A));   // VDIFF_A
}

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_03", name, addr, fn)
B100("load_difficulty_settings", 0x00472ee0, &load_difficulty);
#undef B100

}  // namespace game_b100_03_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Game::Game_46f090() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_02_2::H::game_ctor>(this)); }  // 0x46f090
auto joan::orig::Game::reset() -> void { joan::bind::member<&::game_b100_03_2::H::slot_reset>(this); }  // 0x46f1c0
auto joan::orig::Game::shutdown_46f200() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::game_shutdown>(this)); }  // 0x46f200
auto joan::orig::Game::init_once_46f350() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_19::H::init_once_46f350>(this)); }  // 0x46f350
auto joan::orig::Game::load_resources_46f3a0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b109_01::H::load_resources>(this)); }  // 0x46f3a0
auto joan::orig::Unit::nearest_target_b(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_02::H::target_b>(this, a0)); }  // 0x46fa80
auto joan::orig::Game::load_interface_res() -> void { joan::bind::member<&::game_b106_08::H::load_interface_res>(this); }  // 0x46fc30
auto joan::orig::Unit::react_mount(std::int32_t a0) -> void { joan::bind::member<&::game_b97_01::H::react_mount>(this, a0); }  // 0x470430
auto joan::orig::Game::reset_all_470630() -> void { joan::bind::member<&::game_b108_05::H::reset_all>(this); }  // 0x470630
auto joan::orig::Game::reset_all() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_02::H::reset_all>(this)); }  // 0x4709d0
auto joan::orig::Sounds::resume(std::int32_t a0) -> void { joan::bind::member<&::game_world::H::sound_resume>(this, a0); }  // 0x470c90
auto joan::orig::Game::load_arrays() -> void { joan::bind::member<&::game_b106_07::H::load_arrays>(this); }  // 0x470d40
auto joan::orig::Game::end_session(std::int32_t a0) -> void { joan::bind::member<&::game_b83_06::H::end_session>(this, a0); }  // 0x471010
auto joan::orig::Unit::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_05::H::deleting_dtor_471530>(this, a0)); }  // 0x471530
auto joan::orig::Game::log_progress(const char* a0) -> void { joan::bind::member<&::game_b92_02::H::nop>(this, a0); }  // 0x471580
auto joan::orig::Unit::nearest_target_c(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_02::H::target_c>(this, a0)); }  // 0x471c80
auto joan::orig::Game::add_end_condition() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::add_slot>(this)); }  // 0x471e20
auto joan::orig::Game::pop_end_condition() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_02::H::pop_entry>(this)); }  // 0x471ef0
auto joan::orig::Ending::start(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::ending_start>(this, a0)); }  // 0x471f90
auto joan::orig::Game::set_flag_1b5(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::set_bit>(this, a0, a1)); }  // 0x472000
auto joan::orig::Player::flag_bit_4720d0(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_02::H::flag_bit>(this, a0)); }  // 0x4720d0
auto joan::orig::Player::set_flag_bit(std::int32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b100_03::H::set_flag_bit>(this, a0, a1); }  // 0x472150
auto joan::orig::Player::flag_bit_472220(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_02::H::flag_bit>(this, a0)); }  // 0x472220
auto joan::orig::Game::set_flag_1bd(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b94_01::H::set_bit>(this, a0, a1); }  // 0x4722a0
auto joan::orig::Player::flag_bit_472370(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b100_03::H::flag_bit>(this, a0)); }  // 0x472370
auto joan::orig::Game::set_flag_1c1(std::int32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_02::H::set_bit>(this, a0, a1)); }  // 0x4723f0
auto joan::orig::Game::get_flag_1c1(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_02::H::get_bit>(this, a0)); }  // 0x4724c0
auto joan::orig::Game::clear_flags_1b9() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::clear_mask>(this)); }  // 0x472540
auto joan::orig::Game::set_744(char a0) -> void { joan::bind::member<&::game_b84_05::H::set_744>(this, a0); }  // 0x4725f0
auto joan::orig::Game::add_word(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_02::H::short_add>(this, a0)); }  // 0x472620
auto joan::orig::Game::has_word(std::int16_t a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b90_03::H::has_word>(this, a0)); }  // 0x472740
auto joan::orig::Game::copy_words_to() -> void { joan::bind::member<&::game_b96_02::H::copy_words_to>(this); }  // 0x4727b0
auto joan::orig::Game::copy_word_table() -> void { joan::bind::member<&::game_b100_03::H::copy_word_table>(this); }  // 0x4728f0
auto joan::orig::Game::push_word_728(std::int32_t a0) -> void { joan::bind::member<&::game_b92_02::H::words_push>(this, a0); }  // 0x4729c0
auto joan::orig::Game::has_word_728(std::int16_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b91_02::H::contains>(this, a0)); }  // 0x472a90
auto joan::orig::Game::push_buffer() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_02::H::push_buffer>(this)); }  // 0x472b00
auto joan::orig::Game::pull_buffer() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_02::H::pull_buffer>(this)); }  // 0x472c40
auto joan::orig::Players::set_b16(std::uint32_t a0) -> void { joan::bind::member<&::game_b94_02::H::set_b16>(this, a0); }  // 0x472eb0
auto joan::orig::load_difficulty_settings() -> void { joan::bind::free<&::game_b100_03_3::load_difficulty>(); }  // 0x472ee0
