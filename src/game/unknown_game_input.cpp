// game/unknown_game_input.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_52f930.cpp; re/attribution.csv: between ww/unitres.cpp and ww/unitures.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b117_04.cpp (game/b117_04)
namespace {
namespace game_b117_04 {

using joan::call::thiscall;
using joan::img::obj;

using joan::hook::global;

using joan::call::cdecl_call;
using joan::img::str;

using joan::hook::x87::ftol;

using joan::hook::x87::Fpu;

using joan::hook::global;
using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;
using joan::call::import_call;
template <class R = void, class... A>
R win32_call(img::Addr slot, A... args) {  // stdcall Win32 import through its IAT slot
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}
template <class R = void, class... A>
R com_call(void* self, u32 slot, A... args) {  // COM method (stdcall, this first)
    void* const* const vtbl = joan::call::vtable_of(self);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vtbl[slot])(self, args...);
}
using joan::img::obj;
using joan::img::str;
u32 rd32(img::Addr a) {  // unaligned global dword
    u32 v;
    std::memcpy(&v, img::obj(a), sizeof v);
    return v;
}
u32 rd16(img::Addr a) {  // unaligned global word, zero-extended
    u16 v;
    std::memcpy(&v, img::obj(a), sizeof v);
    return v;
}

struct V3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers
    float x, y, z;
};
static_assert(sizeof(V3) == 12);

JOAN_COUNTER(c_input, "Game::process_input");

constexpr img::Addr kGame = d::g_game_a50088;
constexpr img::Addr kKey = d::g_key;  // current key code (+ modifier bits in the high byte)
constexpr img::Addr kUnits = d::g_units;
constexpr img::Addr kHero = d::g_player_unit_recno;
constexpr img::Addr kScenarios = d::g_scenarios;
constexpr img::Addr kLight = d::g_sys3d;

// Light rig nudge (keys 0x24 0x25 0x26 0x28 0x2a 0x5e): 0x00602bd0(x, y, z) + 0x00602cd0 on 0xa52420.
void light_set(u32 x, u32 y, u32 z) {
    joan::orig::as_sys_en(obj(kLight))->set_rgb_offsets(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(z));  // 0x00602bd0
    joan::orig::as_d3d(obj(kLight))->apply_gamma();           // 0x00602cd0
}

// Toggle of a dword flag: v = (v == 0).
void toggle(img::Addr a) { global<u32>(a) = global<u32>(a) == 0 ? 1u : 0u; }

// Slider object -> value (fild +0x81, fidiv +0x79 at PC24).
double slider_ratio(img::Addr slot) {
    const Fpu fpu{true};
    const void* const s = global<void*>(slot);
    const double num = static_cast<double>(jo::as_scroll(s)->pos);
    const void* const s2 = global<void*>(slot);  // reloaded by the original
    return fpu.div(num, static_cast<double>(jo::as_scroll(s2)->max));
}

// ---------------------------------------------------------------- key 0x79: world + script dialogs
// comdlg32 OPENFILENAMEA up to lpTemplateName (OPENFILENAME_SIZE_VERSION_400A), natural alignment as commdlg.h:
// 0x4c bytes on x86 (the original's lStructSize), 0x88 on x64.
struct OpenFileNameA {
    u32 lStructSize;
    void* hwndOwner;
    void* hInstance;
    const char* lpstrFilter;
    char* lpstrCustomFilter;
    u32 nMaxCustFilter;
    u32 nFilterIndex;
    char* lpstrFile;
    u32 nMaxFile;
    char* lpstrFileTitle;
    u32 nMaxFileTitle;
    const char* lpstrInitialDir;
    const char* lpstrTitle;
    u32 Flags;
    u16 nFileOffset;
    u16 nFileExtension;
    const char* lpstrDefExt;
    std::intptr_t lCustData;
    void* lpfnHook;
    const char* lpTemplateName;
};
static_assert(sizeof(void*) != 4 || sizeof(OpenFileNameA) == 0x4c, "OPENFILENAMEA (version 4.00, x86)");

// The original frame ebp-0x48c .. ebp-0x18, addressed by the ebp offset; the OPENFILENAMEA at ebp-0x64 is typed
// (the native struct on x64, where it is larger than the x86 slot).
struct DialogFrame {
    alignas(4) u8 b[0x48c - 0x64];
    OpenFileNameA ofn;  // [ebp-0x64]
    u8* at_ebp(u32 neg) { return b + (0x48c - neg); }  // neg > 0x64
};

void load_world_dialog() {
    DialogFrame fr;
    std::memset(static_cast<void*>(&fr), 0, sizeof fr);
    OpenFileNameA* const ofn = &fr.ofn;
    char* const world = reinterpret_cast<char*>(fr.at_ebp(0x16c));
    char* const cwd = reinterpret_cast<char*>(fr.at_ebp(0x274));
    u8* const res1 = fr.at_ebp(0x278);
    char* const script = reinterpret_cast<char*>(fr.at_ebp(0x380));
    u8* const res2 = fr.at_ebp(0x384);
    u8* const found = fr.at_ebp(0x388);
    char* const mapdir = reinterpret_cast<char*>(fr.at_ebp(0x48c));
    auto put = [](u8* p, u32 v) { std::memcpy(p, &v, 4); };
    auto get = [](const u8* p) {
        u32 v;
        std::memcpy(&v, p, 4);
        return v;
    };

    joan::orig::game_tick_4189a0();                  // 0x004189a0
    win32_call<i32>(d::iat_USER32_ShowCursor, i32{1});     // ShowCursor(TRUE)
    std::memset(static_cast<void*>(ofn), 0, sizeof *ofn);                 // memset (0x4c)
    world[0] = 0;
    ofn->lStructSize = static_cast<u32>(sizeof *ofn);  // 0x4c
    ofn->hwndOwner = global<void*>(d::g_main_hwnd);
    ofn->lpstrFilter = str(d::s_WorldFile_wor);        // 'WorldFile (*.wor)'
    ofn->lpstrFile = world;
    ofn->nMaxFile = 0x105;
    ofn->lpstrInitialDir = str(d::s_MAP_9cf818);       // 'MAP\'
    ofn->lpstrTitle = str(d::s_load_world);            // 'load world'
    ofn->Flags = 0x1004;
    joan::orig::libc::_getcwd(joan::orig::arg::str(cwd), 0x105);            // _getcwd
    put(res1, joan::orig::arg::as<u32>(joan::orig::comdlg32::get_open_file_name_a(joan::orig::arg::mptr(static_cast<void*>(ofn)))));  // GetOpenFileNameA
    joan::orig::libc::_chdir(joan::orig::arg::str(cwd));                     // _chdir

    std::memset(static_cast<void*>(ofn), 0, sizeof *ofn);                 // memset
    script[0] = 0;
    ofn->lStructSize = static_cast<u32>(sizeof *ofn);
    ofn->hwndOwner = global<void*>(d::g_main_hwnd);
    ofn->lpstrFilter = str(d::s_ScriptFile_sce);       // 'ScriptFile (*.sce)'
    ofn->lpstrFile = script;
    ofn->nMaxFile = 0x105;
    ofn->lpstrInitialDir = str(d::s_SCRIPT_SCENARIO);  // 'SCRIPT\SCENARIO\'
    ofn->lpstrTitle = str(d::s_load_script);           // 'load script'
    ofn->Flags = 0x1004;
    put(res2, joan::orig::arg::as<u32>(joan::orig::comdlg32::get_open_file_name_a(joan::orig::arg::mptr(static_cast<void*>(ofn)))));  // GetOpenFileNameA
    joan::orig::libc::_chdir(joan::orig::arg::str(cwd));                     // _chdir
    win32_call<i32>(d::iat_USER32_ShowCursor, i32{0});                  // ShowCursor(FALSE)
    joan::orig::game::tick_418bc0();                               // 0x00418bc0

    if (get(res1) == 0 || get(res2) == 0) return;
    {
        const u32 n = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(world)));  // strlen
        *(fr.at_ebp(0x170) + n) = 0;                       // [ebp + n - 0x170]: strip ".wor"
    }
    // x64: the found slot [ebp-0x388] keeps the low half of the strstr result (a pointer into world, this frame);
    // its upper half is world's (0 on x86).
    const std::uintptr_t hi_found = reinterpret_cast<std::uintptr_t>(world) & ~static_cast<std::uintptr_t>(0xffffffffu);
    joan::orig::libc::_getcwd(joan::orig::arg::str(mapdir), 0x104);          // _getcwd
    std::strcat(static_cast<char*>(joan::orig::arg::mptr(mapdir)), str(d::s_bslash));  // strcat "\"
    std::strcat(static_cast<char*>(joan::orig::arg::mptr(mapdir)), str(d::s_MAP_9cf818));  // strcat "MAP\"
    char* const up_dir = joan::orig::arg::as<char*>(joan::orig::libc::_strupr(joan::orig::arg::str(mapdir)));  // _strupr
    char* const up_world = joan::orig::arg::as<char*>(joan::orig::libc::_strupr(joan::orig::arg::str(world)));  // _strupr
    put(found, static_cast<u32>(joan::orig::arg::as<std::uintptr_t>(joan::orig::libc::strstr(joan::orig::arg::str(up_world), joan::orig::arg::str(up_dir)))));    // strstr (the slot is 4 bytes: low half, see hi_found)
    if (get(found) == 0) return;
    {
        const u32 n = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(mapdir)));  // strlen (may read the spilled found slot)
        put(found, get(found) + n);
    }
    {
        const u32 n = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(script)));  // strlen
        *(fr.at_ebp(0x384) + n) = 0;                        // [ebp + n - 0x384]: strip ".sce"
    }
    const std::uintptr_t slash = joan::orig::arg::as<std::uintptr_t>(joan::orig::libc::strrchr(joan::orig::arg::str(script), i32{0x5c}));  // strrchr '\'
    joan::orig::as_battle(obj(kScenarios))->load_scenario_41dfe0(joan::orig::arg::str(static_cast<std::uintptr_t>(get(found)) | hi_found), joan::orig::arg::str(slash + 1));       // 0x0041dfe0 load world + script
    joan::orig::as_game(obj(kGame))->set_state(joan::orig::arg::u32(i32{9}));                           // 0x004d7e70 Game::set_state
}

// ---------------------------------------------------------------- the developer key switch
void cheat_key(void* self) {
    const u32 key = rd32(kKey);
    switch (key) {
        case 0x0d:
            joan::orig::as_debug_overlay(obj(d::g_debug_overlay))->on_key();  // 0x004d2a20
            break;
        case 0x23:
            toggle(d::g_a39a14);
            if (global<u32>(d::g_a39a14) != 0) joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
            break;
        case 0x24: light_set(rd32(d::g_rgb_offset_r) + 1, rd32(d::g_rgb_offset_g), rd32(d::g_rgb_offset_b)); break;
        case 0x25: light_set(rd32(d::g_rgb_offset_r) - 1, rd32(d::g_rgb_offset_g), rd32(d::g_rgb_offset_b)); break;
        case 0x26: light_set(rd32(d::g_rgb_offset_r), rd32(d::g_rgb_offset_g) - 1, rd32(d::g_rgb_offset_b)); break;
        case 0x28: light_set(rd32(d::g_rgb_offset_r), rd32(d::g_rgb_offset_g), rd32(d::g_rgb_offset_b) - 1); break;
        case 0x2a: light_set(rd32(d::g_rgb_offset_r), rd32(d::g_rgb_offset_g), rd32(d::g_rgb_offset_b) + 1); break;
        case 0x5e: light_set(rd32(d::g_rgb_offset_r), rd32(d::g_rgb_offset_g) + 1, rd32(d::g_rgb_offset_b)); break;
        case 0x2b: {  // speed +5 (below 50)
            if (!(jo::as_wnd(self)->speed >= global<float>(d::flt_50)))  // < 50 or NaN
                jo::as_wnd(self)->speed = jo::as_wnd(self)->speed + global<float>(d::flt_5);  // + 5
            jo::view<u16>(jo::as_wnd(self)->i16_48) = static_cast<u16>(ftol(jo::as_wnd(self)->speed));
            break;
        }
        case 0x2d: {  // speed -5 (above 5), at least 1
            if (jo::as_wnd(self)->speed > global<float>(d::flt_5))
                jo::as_wnd(self)->speed = jo::as_wnd(self)->speed - global<float>(d::flt_5);
            jo::view<u16>(jo::as_wnd(self)->i16_48) = static_cast<u16>(ftol(jo::as_wnd(self)->speed));
            if (!(jo::as_wnd(self)->speed >= global<float>(d::g_a03838)))  // < 1 or NaN
                jo::view<u32>(jo::as_wnd(self)->speed) = global<u32>(d::g_a03838);
            break;
        }
        case 0x2f: {
            global<i8>(d::g_a4f83c) = static_cast<i8>(global<u8>(d::g_a4f83c) + 1);
            global<i8>(d::g_a4f83c) = static_cast<i8>(static_cast<i32>(global<i8>(d::g_a4f83c)) % 3);
            break;
        }
        case 0x30:  // pause off
            if (jo::as_wnd(self)->u8_4a != 0) {
                jo::as_wnd(self)->u8_4a = 0;
                jo::view<u16>(jo::as_wnd(self)->i16_48) = static_cast<u16>(ftol(jo::as_wnd(self)->speed));
                joan::orig::as_game(obj(kGame))->set_speed(joan::orig::arg::u32(u32{0}));  // 0x0042f930(0.0f)
            }
            break;
        case 0x31:  // pause on
            if (jo::as_wnd(self)->u8_4a == 0) {
                jo::as_wnd(self)->u8_4a = 1;
                const float speed = static_cast<float>(static_cast<i32>(jo::as_wnd(self)->i16_48));  // fild / fstp
                joan::orig::as_game(obj(kGame))->set_speed(joan::orig::arg::bits(speed));  // 0x0042f930
            }
            break;
        case 0x3b:
            joan::orig::as_grass(obj(d::g_dyn_grass))->set_enabled(static_cast<char>(joan::orig::arg::i32(global<i8>(d::g_a45d34) == 0 ? 1u : 0u)));  // 0x00464600
            break;
        case 0x3d: toggle(d::g_a3a2ac); break;
        case 0x47: case 0x67: toggle(d::g_draw_overlay_mesh); break;
        case 0x48: case 0x68: toggle(d::g_a0382c); break;
        case 0x4a: case 0x6a: toggle(d::g_a03830); break;
        case 0x4b: case 0x6b: toggle(d::g_a17878); break;
        case 0x49: {
            void* const p = global<void*>(d::g_world);
            if (p != nullptr && jo::as_world(p)->scene != nullptr) {
                const u32 on = joan::orig::arg::as<u32>(joan::orig::as_scene(jo::view<void*>(jo::as_world(p)->scene))->flag_68());  // 0x004d29c0
                joan::orig::as_scene(jo::view<void*>(jo::as_world(p)->scene))->set_68(joan::orig::arg::u32(on == 0 ? 1u : 0u));    // 0x005ef3d0
            }
            break;
        }
        case 0x69: {
            void* const p = global<void*>(d::g_world);
            if (p != nullptr && jo::as_world(p)->scene != nullptr) {
                const u32 on = joan::orig::arg::as<u32>(joan::orig::as_scene(jo::view<void*>(jo::as_world(p)->scene))->flag_6c());  // 0x004d2a00
                joan::orig::as_scene(jo::view<void*>(jo::as_world(p)->scene))->set_6c(joan::orig::arg::u32(on == 0 ? 1u : 0u));    // 0x005ef400
            }
            break;
        }
        case 0x62: joan::orig::as_ending(obj(d::g_ending_screen))->open(static_cast<std::int8_t>(joan::orig::arg::i32(u32{1}))); break;  // 0x006d19e0
        case 0x76: joan::orig::as_ending(obj(d::g_ending_screen))->open(static_cast<std::int8_t>(joan::orig::arg::i32(u32{0}))); break;  // 0x006d19e0
        case 0x6e: joan::orig::as_ending(obj(d::g_ending_screen))->reset(); break;          // 0x006d1d30
        case 0x6c: {
            const u32 v = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(u32{0x2710})));  // 0x00590540
            joan::orig::as_name_list(obj(d::g_player_hud))->add_6fe520(joan::orig::arg::i32(v));                              // 0x006fe520
            break;
        }
        case 0x6d:
            if (global<u32>(d::g_video_file) == 0) {
                joan::orig::as_video(obj(d::g_video_file))->open_684650(str(d::s_WW_CAP_WMV), static_cast<joan::orig::Sound3D*>(global<void*>(d::g_a5cf9c)));  // 0x00684650
                global<u32>(d::g_movie_start_ms) = 0;
                if (global<void*>(d::g_a5d2f8) != nullptr) {
                    joan::orig::as_sound3d(global<void*>(d::g_a5d2f8))->set_position(joan::orig::arg::mptr(obj(d::g_pos)));  // 0x0059fb40
                    joan::orig::as_sound3d(global<void*>(d::g_a5d2f8))->set_relative(static_cast<std::uint8_t>(joan::orig::arg::u32(u32{1})));           // 0x0059fae0
                }
            } else {
                joan::orig::as_video(obj(d::g_video_file))->close();  // 0x00686160
            }
            break;
        case 0x70:
            if (global<i32>(kHero) > 0 && joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(global<u32>(kHero)))) == 0) {  // 0x00428b70
                void* const unit = joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(global<u32>(kHero)));  // 0x0041b3c0
                void* const group = jo::as_unit(unit)->anim_set;
                for (i32 i = jo::as_anim_set(group)->group_count; i > 0; --i) {
                    void* const e = joan::orig::arg::as<void*>(joan::orig::as_anim_set(group)->entry(joan::orig::arg::u32(i)));  // 0x004168d0
                    jo::as_anim_group_rec(e)->u8_e = 0x59;
                }
            }
            break;
        case 0x74: {  // next mission
            global<u8>(d::g_a58f95) = 1;
            const u32 cur = rd32(d::g_mission);  // `add eax,1` wraps: unsigned (no signed-overflow UB)
            joan::orig::as_battle(obj(kScenarios))->load_scenario_41cc80(static_cast<i32>(cur + 1u), i32{1}, i32{0});  // 0x0041cc80
            joan::orig::as_game(obj(kGame))->set_state(joan::orig::arg::u32(i32{0xc}));                     // 0x004d7e70 Game::set_state
            break;
        }
        case 0x75: {  // fill mode
            void* const dev = global<void*>(d::g_game_device);
            com_call<i32>(dev, 0xc8 / 4, u32{8}, global<u32>(d::g_a0383c));  // SetRenderState(FILLMODE)
            global<i32>(d::g_a0383c) = global<i32>(d::g_a0383c) <= 1 ? 3 : global<i32>(d::g_a0383c) - 1;
            break;
        }
        case 0x79: load_world_dialog(); break;
        case 0x7c:
            if (joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(global<u32>(kHero)))) == 0) {  // 0x00428b70
                const V3 dir = global<V3>(d::g_pos);
                const u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(global<u32>(kHero))));  // 0x0041b3c0
                V3 pos;
                std::memcpy(&pos, jo::as_unit(unit)->position_, sizeof pos);
                joan::orig::as_markers(obj(d::g_slots_a3ce90))->add(joan::orig::arg::i32(u32{1}), pos, dir, joan::orig::arg::i32(u32{0x447a0000}), str(d::s_glow2dlight));  // 0x004877e0
            }
            break;
        case 0x4200: {  // texture.log
            void* const f = joan::orig::arg::as<void*>(joan::orig::libc::fopen(str(d::s_texture_log), str(d::s_w)));  // fopen
            jo::TextureManager* const textures = joan::orig::as_texture_manager(obj(d::g_texture_array));  // named_count +0, named +8 (pointer-wide on x64)
            for (i32 i = 1; i <= textures->named_count; ++i) {
                const jo::Texture* const e = textures->named[i - 1].p;
                if (e == nullptr) continue;
                joan::orig::libc::fprintf(joan::orig::arg::mptr(f), str(d::s_s), jo::as_texture(e)->name);  // fprintf "%s\n"
            }
            joan::orig::libc::fclose(joan::orig::arg::mptr(f));  // fclose
            break;
        }
        case 0x4300:
            joan::orig::as_profiler(obj(d::g_cpu_usage))->dump_log(str(d::s_cpuusage_log));  // 0x0045d330 'cpuusage.log'
            joan::orig::as_sys_en(obj(kLight))->dump_log(str(d::s_sys3d_log));      // 0x00601140 'sys3d.log'
            break;
        case 0x4700:
            if ((rd16(d::g_key_modifiers) & 3) != 0) joan::orig::as_debug_overlay(obj(d::g_debug_overlay))->update();  // 0x004d2a50
            break;
        default: break;
    }

    // Key combinations (0x00599240: key, modifiers word, wanted, flags).
    if (joan::orig::arg::as<u32>(joan::orig::keyboard::translate(joan::orig::arg::u32(rd32(d::g_last_key_scancode)), joan::orig::arg::u32(rd16(d::g_a51d1a)), joan::orig::arg::u32(u32{0x6c}), joan::orig::arg::u32(u32{0x104}))) != 0 &&
        joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(global<u32>(kHero)))) == 0) {  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(global<u32>(kHero))));  // 0x0041b3c0
        jo::view<u32>(jo::as_unit(unit)->state_120) = jo::view<u32>(jo::as_unit(unit)->state_120) + 1;
    }
    if (joan::orig::arg::as<u32>(joan::orig::keyboard::translate(joan::orig::arg::u32(rd32(d::g_last_key_scancode)), joan::orig::arg::u32(rd16(d::g_a51d1a)), joan::orig::arg::u32(u32{0x4400}), joan::orig::arg::u32(u32{8}))) != 0) {
        if (global<u32>(d::g_video_playing) == 0) {
            joan::orig::as_image(obj(d::g_a3a2d8))->create(joan::orig::arg::u32(global<u32>(d::g_screen_w)), joan::orig::arg::u32(global<u32>(d::g_screen_h)), joan::orig::arg::u32(u32{0}), joan::orig::arg::i32(u32{0xb}));  // 0x0060fb60
            global<u32>(d::g_a3a310) = 0;
            global<u32>(d::g_video_playing) = 1;
            joan::orig::as_video_capture(obj(d::g_video_capture))->init(str(d::s_WW_CAP_WMV), joan::orig::arg::i32(u32{0x1e}), joan::orig::arg::i32(u32{0xb}), joan::orig::arg::i32(global<u32>(d::g_screen_w)), joan::orig::arg::i32(global<u32>(d::g_screen_h)));  // 0x00686350 'WW_CAP.WMV'
        } else {
            joan::orig::as_video_capture(obj(d::g_video_capture))->release();  // 0x00686f50
            joan::orig::as_image(obj(d::g_a3a2d8))->release();  // 0x0060fd20
            global<u32>(d::g_video_playing) = 0;
        }
    }
}

// The modal screens: true when one of them took the frame's input.
bool al(std::uintptr_t thunk, img::Addr self) { return thiscall<u8>(thunk, obj(self)) != 0; }
bool ax(std::uintptr_t thunk, img::Addr self) { return thiscall<u32>(thunk, obj(self)) != 0; }

void process_input_body(void* self) {
    c_input.hit();
    joan::orig::as_input(obj(d::g_mouse))->pop_event();  // 0x005932e0 input update
    if (static_cast<i32>(global<i16>(d::g_a4860c)) == 2) joan::orig::as_cursor(obj(d::g_mouse_cursor))->set_596db0(joan::orig::arg::i32(u32{1}));  // 0x00596db0
    if (global<i8>(d::g_a59a6c) != 0 && rd32(kKey) == 0x7e) joan::orig::as_chat_box(obj(d::g_chat_box))->open();  // 0x0045b8b0

    if (joan::orig::arg::as<u32>(joan::orig::as_counter(obj(kScenarios))->at_least_10()) != 0) {  // 0x0041c7e0
        u32 mode = global<u32>(d::g_control_mode);
        if (mode == 4) mode = global<u32>(d::g_control_base_mode);
        if (joan::orig::arg::as<u32>(joan::orig::as_race_res(obj(d::g_keys_a48820))->test(joan::orig::arg::i32(mode), joan::orig::arg::i32(u32{0x37}))) != 0) {  // 0x004cb820
            joan::orig::as_game(self)->screenshot_603b20();                                       // 0x00603b20
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->sysdet_1());          // 0x004daef0
            joan::orig::as_sms(obj(d::g_sms))->add(joan::orig::arg::str(s), joan::orig::arg::i32(u32{2}), joan::orig::arg::i32(u32{0}));          // 0x006e5700
        }
    }

    if (al(fn::code_value(fn::X_flag_107e), d::g_victory_screen)) return;  // 0x006e8c50
    if (al(fn::code_value(fn::Popup_click_6e7880), d::g_glory_screen)) return;  // 0x006e7880
    if (ax(fn::code_value(fn::Menu_key_45bc10), d::g_chat_box)) return;  // 0x0045bc10
    if (al(fn::code_value(fn::Settings_click_73d830), d::g_opt_menu)) return;  // 0x0073d830
    if (al(fn::code_value(fn::Dialog_step_6e1250), d::g_msg_log)) return;  // 0x006e1250
    if (al(fn::code_value(fn::Letter_poll), d::g_letter)) return;  // 0x006e0830
    if (al(fn::code_value(fn::SaveMenu_update), d::g_save_menu)) return;  // 0x0074f230
    if (al(fn::code_value(fn::Menu_handle_6b7d50), d::g_game_over_screen)) return;  // 0x006b7d50
    if (static_cast<i32>(jo::as_wnd(self)->mode) == 9) {
        if (ax(fn::code_value(fn::ItemScreen_close_6bb420), d::g_armory_screen)) return;  // 0x006bb420
        if (al(fn::code_value(fn::Menu_update_720ca0), d::g_objective_menu)) return;  // 0x00720ca0
        if (al(fn::code_value(fn::Shop_mouse_6c6550), d::g_buy)) return;  // 0x006c6550
        if (al(fn::code_value(fn::RecruitPanel_update_6bf630), d::g_barracks_screen)) return;  // 0x006bf630
        if (al(fn::code_value(fn::Workshop_update_6ec480), d::g_workshop_screen)) return;  // 0x006ec480
        if (ax(fn::code_value(fn::Gadget_check_weather), d::g_quick_save_screen)) return;  // 0x006e3890
        if (al(fn::code_value(fn::Panel_update_6fd670), d::g_player_hud)) return;  // 0x006fd670
        joan::orig::as_list(obj(d::g_names))->advance_state();    // 0x0049ab70
        if (al(fn::code_value(fn::Menu_pass_through), d::g_game_a5d380)) return;  // 0x006ca5c0
        joan::orig::as_fade_title(obj(d::g_fade_title))->enter_pressed();    // 0x007615e0
        joan::orig::as_panel(obj(d::g_dialogue))->update_buttons(joan::orig::arg::u32(u32{0}));  // 0x0063c1c0
        if (al(fn::code_value(fn::SaveMenu_update_6f3c90), d::g_ingame_menu)) return;  // 0x006f3c90
        if (al(fn::code_value(fn::Panel_update_6f7780), d::g_a56398)) return;  // 0x006f7780
        if (al(fn::code_value(fn::X_byte_0_6e6810), d::g_roster_screen)) joan::orig::as_shop(obj(d::g_roster_screen))->hover_6e6490();  // 0x006e6810 / 0x006e6490
    }

    // Pending message box answer.
    if ((joan::orig::arg::as<u32>(joan::orig::as_popup(obj(d::g_popup_a50808))->has_answer()) & 0xff) != 0) {  // 0x004d2a70
        const i32 answer = joan::orig::arg::as<i32>(joan::orig::as_panel(obj(d::g_popup_a50808))->poll_62b050());  // 0x0062b050
        if (answer != 0) {
            const char* const id = joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(d::g_txt))->popup_1());  // 0x004da500
            if (std::strcmp(joan::orig::arg::str(global<const char*>(d::g_a50eb1)), joan::orig::arg::str(id)) != 0) {  // strcmp
                joan::orig::as_popup(obj(d::g_popup_a50808))->close();                       // 0x0062b620
                joan::orig::as_game(obj(kGame))->set_speed(joan::orig::arg::u32(global<u32>(d::g_a03838)));  // 0x0042f930
            } else if (answer == 1) {
                joan::orig::as_popup(obj(d::g_popup_a50808))->close();                       // 0x0062b620
                joan::orig::as_game(obj(kGame))->set_speed(joan::orig::arg::u32(global<u32>(d::g_a03838)));  // 0x0042f930
                if (joan::orig::arg::as<u32>(joan::orig::as_counter(obj(kScenarios))->at_least_10()) != 0) {        // 0x0041c7e0
                    joan::orig::as_handle(obj(kScenarios))->release(joan::orig::arg::u32(u32{0}));            // 0x0041c780
                    joan::orig::as_game(self)->set_state(joan::orig::arg::u32(i32{1}));                       // 0x004d7e70 Game::set_state
                }
            } else if (answer == -1) {
                joan::orig::as_popup(obj(d::g_popup_a50808))->close();                       // 0x0062b620
                joan::orig::as_game(obj(kGame))->set_speed(joan::orig::arg::u32(global<u32>(d::g_a03838)));  // 0x0042f930
            }
        }
    }

    if (global<i8>(d::g_cheat_keys_enabled) != 0) cheat_key(self);

    if (rd32(kKey) == 0x4700 && joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->is_file_exist(str(d::s_super_cheat_mode_sys))) != 0 &&  // 0x00590b80
        (rd16(d::g_key_modifiers) & 3) == 0)
        global<u8>(d::g_cheat_keys_enabled) = static_cast<u8>(global<u8>(d::g_cheat_keys_enabled) ^ 1);

    void* const st = obj(d::g_game_state);
    switch (static_cast<i32>(jo::as_wnd(self)->mode)) {
        case 1: joan::orig::as_game(st)->update_mode1(); break;   // 0x0047b7c0
        case 3: joan::orig::as_game(st)->toggle_47b8d0(); break;   // 0x0047b8d0
        case 4: joan::orig::as_game(st)->update_mode4(); break;   // 0x0047b980
        case 5: joan::orig::as_game(st)->update_mode5(); break;   // 0x0047ba00
        case 6: joan::orig::as_game(st)->update_mode6(); break;   // 0x0047baa0
        case 7: joan::orig::as_game(st)->update_mode7(); break;   // 0x0047bb20
        case 8: joan::orig::as_game(st)->update_mode8(); break;   // 0x0047bba0
        case 9: joan::orig::as_camera_man(obj(d::g_camera_man))->update_keys(); break;  // 0x00562f40
        case 11: joan::orig::as_game(st)->update_mode11(); break;  // 0x0047bc50
        case 12: joan::orig::as_game(st)->update_mode12(); break;  // 0x0047bd40
        case 13: joan::orig::as_game(st)->update_mode13(); break;  // 0x0047be30
        case 14: joan::orig::as_game(st)->update_mode14(); break;  // 0x0047bf40
        case 15: joan::orig::as_game(st)->update_mode15(); break;  // 0x0047c020
        default: break;                            // 2, 10, others
    }

    if (global<u32>(d::g_a39a14) == 0 || rd32(d::g_a52623) == 0) return;
    struct Slot {
        img::Addr slider, out;
        bool signed_unit;  // * 2 - 1
    };
    static const Slot kSlots[11] = {
        {d::g_a39a18, d::g_a47f8c, false}, {d::g_a39a1c, d::g_a47f90, false}, {d::g_a39a20, d::g_a47f94, false},
        {d::g_a39a24, d::g_a47f98, false}, {d::g_a39a28, d::g_a47f9c, false}, {d::g_a39a2c, d::g_a47f7c, false},
        {d::g_a39a30, d::g_a47f80, false}, {d::g_a39a34, d::g_a47f84, false}, {d::g_a39a38, d::g_a47f70, true},
        {d::g_a39a3c, d::g_a47f74, true},  {d::g_a39a40, d::g_a47f78, true},
    };
    const Fpu fpu{true};
    for (const Slot& s : kSlots) {
        if (global<void*>(s.slider) == nullptr) continue;  // the slider object pointers are pointer-wide slots
        if (joan::orig::arg::as<u32>(joan::orig::as_text_box(global<void*>(s.slider))->drag()) == 0) continue;  // 0x0062b9e0
        double v = slider_ratio(s.slider);
        if (s.signed_unit) v = fpu.sub(v + v, static_cast<double>(global<float>(d::flt_1)));  // fadd st,st; fsub 1.0
        global<float>(s.out) = static_cast<float>(v);
    }
    import_call<void*>(d::iat_v3d_87_vector_normalize_1, obj(d::g_a47f70));  // v3d_87 vector_normalize (in place)
}

struct H {
    void process_input() { process_input_body(this); }  // 0x0052f930 (thiscall, ret 0)
};

JOAN_REPLACE_YIELDING("game/b117_04", "Game::process_input", 0x0052f930, &H::process_input, "debug/menu");

}  // namespace game_b117_04
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

struct H {
    // 0x00530c60
    H* clear_34() {
        jo::self<jo::UnitTable>(this)->loaded = 0;
        return this;
    }
};

JOAN_REPLACE(M, "Obj::clear_34", 0x00530c60, &H::clear_34);

}  // namespace game_b103_06
}  // namespace
#undef M

namespace joan::game_b117_04 {
// For the debug menu (src/debug_menu.cpp), which replaces 0x0052f930 itself when installed.
void process_input(void* self) { ::game_b117_04::process_input_body(self); }
}  // namespace joan::game_b117_04
