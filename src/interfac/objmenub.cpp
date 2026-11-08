// interfac/objmenub.cpp: functions of the original d:\ww\interfac\objmenub.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

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

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b104_15.cpp (game/b104_15)
//
//      0x007246f0 ObjMenu::open_brief             (thiscall, ret 0) - interfac/objmenub.cpp: loads
//                 SCRIPT\SCENARIO\<name>.OBJ into the text box, MISSION_BRIEF_n sound, text
//                 layout 0x006299b0 / 0x00627d80; returns ~File eax (0x005872b0).
//
//  - eax: the callers at 0x0073cf80 / 0x0073cf8d (0x00741cd0 / 0x00747bc0) and 0x007220a4 /
//    0x007220cb / 0x007223c3 (0x007246f0 / 0x00728a90 / 0x0073b160) jump to their epilogue, so
//    their eax is that of these functions; every replacement returns exactly the eax of the
//    original's last call. 0x00725440's eax is a path-dependent stale value; its only caller
//    (0x0071ee0a) ignores it, so it returns void.
//
//  - 0x007246f0: File (0x004179c0, 0x110 bytes in the frame) and path string (0x005a1170,
//    0x194 bytes, never destroyed in the original - kept so) as locals; SEH frame only protects
//    the File dtor; no callee throws (checked), normal path reproduced.
namespace {
namespace game_b104_15 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using C = jo::CampScreen;

// File (0x004179c0 ctor) and path string (0x005a1170 ctor) as used by the original frames.
struct alignas(alignof(void*)) File {  // 0x110 in the x86 frames; sizeof(File) = 0x112 on x64
    u8 bytes[((sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};
struct alignas(4) Path {
    u8 bytes[0x194];
};

JOAN_COUNTER(c_open_brief, "ObjMenu::open_brief");

struct H {
    template <class W>
    void f5(W* C::*off, u32 a, u32 b, u32 c, u32 d, u32 e) { f5w(jo::self<C>(this)->*off, a, b, c, d, e); }
    template <class W>
    static void f5w(W* w, u32 a, u32 b, u32 c, u32 d, u32 e) {
        joan::orig::as_widget(w)->scale_to_61f1b0(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), e);  // 0x0061f1b0
    }

    // ------------------------------------------------------------------------ 0x007246f0
    std::uintptr_t open_brief() {
        c_open_brief.hit();
        f5w(jo::self<C>(this)->menu_buttons[7], 1, 0x40c00000u, 0, 0, 0xc8);
        f5(&C::brief_up, 1, 0x40800000u, 0, 0, 0xc8);
        f5(&C::brief_down, 1, 0x40800000u, 0, 0, 0xc8);
        File file;
        joan::orig::as_file(&file)->construct();  // 0x004179c0
        jo::view<u16>(jo::self<jo::CampScreen>(this)->mission) = global<u16>(d::g_mission);
        i8 loaded = 0;
        if (global<i32>(d::g_stage_index) > 0) {
            Path path;
            joan::orig::as_string(&path)->construct();             // 0x005a1170
            joan::orig::as_string(&path)->assign(str(d::s_SCRIPT_SCENARIO));             // 0x005a1440 "SCRIPT\SCENARIO\"
            const char* const name = global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].texts + 0x104;
            joan::orig::as_string(&path)->append(joan::orig::arg::str(name));      // 0x005a1550
            joan::orig::as_string(&path)->append(str(d::s_OBJ));                 // 0x005a1550 ".OBJ"
            const char* full = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
            if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(full)) != 0) {  // 0x00590b80
                const char* full2 = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
                if (joan::orig::as_file(&file)->file_open(joan::orig::arg::str(full2), 1, static_cast<char>(0)) != 0) {  // 0x00586da0 open
                    const i32 line = static_cast<i32>(global<i16>(d::g_a29328)) + 0x19;
                    const i32 size = joan::orig::as_file(&file)->file_size();   // 0x00587f30
                    char* const text = joan::orig::arg::as<char*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size + 1), str(d::s_d_ww_interfac_objmenub_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
                    const i32 size2 = joan::orig::as_file(&file)->file_size();   // 0x00587f30
                    joan::orig::as_file(&file)->file_read(joan::orig::arg::mptr(text), joan::orig::arg::u32(size2));  // 0x005873a0 read
                    const i32 size3 = joan::orig::as_file(&file)->file_size();   // 0x00587f30
                    text[size3] = 0;
                    joan::orig::as_image2d(jo::self<C>(this)->brief_text)->set_name(joan::orig::arg::str(text));  // 0x006298c0
                    const i32 line2 = static_cast<i32>(global<i16>(d::g_a29328)) + 0x1f;
                    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(text), str(d::s_d_ww_interfac_objmenub_cpp), joan::orig::arg::i32(line2));  // 0x0058ed70
                    loaded = 1;
                }
                joan::orig::as_file(&file)->file_close();   // 0x00587260 close
            }
        }
        if (loaded == 1) {
            const u32 k = static_cast<u32>(static_cast<i32>(jo::self<jo::CampScreen>(this)->mission) - 1);
            if (k <= 7) {
                static const img::Addr kBrief[8] = {d::s_MISSION_BRIEF_1, d::s_MISSION_BRIEF_2, d::s_MISSION_BRIEF_3, d::s_MISSION_BRIEF_4,
                                                         d::s_MISSION_BRIEF_5, d::s_MISSION_BRIEF_6, d::s_MISSION_BRIEF_7, d::s_MISSION_BRIEF_8};
                joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(kBrief[k]));  // 0x004ceef0 MISSION_BRIEF_n
            }
        }
        if (loaded == 0) {
            const char* const t = joan::orig::as_obj_menu(obj(d::g_strings))->open_0f();  // 0x004dbd30
            joan::orig::as_image2d(jo::self<C>(this)->brief_text)->set_name(joan::orig::arg::str(t));      // 0x006298c0
        }
        jo::view<u16>(jo::self<jo::CampScreen>(this)->brief_line) = 0xfffc;
        joan::orig::as_label(jo::self<C>(this)->brief_text)->set_layout(0x314u, 0x262u, 2u, joan::orig::arg::u32(static_cast<u32>(static_cast<i32>(jo::self<jo::CampScreen>(this)->brief_line))), 1u, 0u, 0xffffffffu);  // 0x006299b0
        {
            jo::Image2D* const box = jo::self<C>(this)->brief_text;
            u32 lines = 0;  // [ebp-0x2c4]: written (word) by 0x00627d80 before any read
            const u32 a28 = jo::view<u32>(box->scale_y), a24 = jo::view<u32>(box->scale_x);
            const u32 a1c = static_cast<u32>(static_cast<i32>(box->wrap_a4));
            char* const a18 = box->text;
            joan::orig::as_text_box(box->font)->measure_lines(0x106, 0xd2, 0x314, 0x262, joan::orig::arg::str(a18), joan::orig::arg::i32(a1c), 0, joan::orig::arg::mptr(&jo::self<C>(this)->brief_px), joan::orig::arg::mptr(&lines), joan::orig::arg::u32(a24), joan::orig::arg::u32(a28));  // 0x00627d80
        }
        f5(&C::brief_text, 1, 0x40800000u, 0, 0, 0xc8);
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_file(&file)->destruct_5872b0());  // 0x005872b0 ~File
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("ObjMenu::open_brief", 0x007246f0, &H::open_brief);
#undef B

}  // namespace game_b104_15
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
//
//      0x00724bf0 Briefing::start()                             thiscall (jump table 0x00724d6b)
namespace {
namespace game_b92_06 {

void* const kSounds = obj(d::g_sfx_player);

// 0x0061f1b0 (ret 0x14), raw dwords as pushed by the original.
using joan::orig::helpers::anim_set;
using joan::orig::helpers::anim_set6;  // 6.0f

std::uint32_t u(std::int32_t v) { return static_cast<std::uint32_t>(v); }

JOAN_COUNTER(c_brief, "Briefing::start");

struct H {
    // 0x00724bf0
    void briefing_start() {
        c_brief.hit();
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]));
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_up));
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_down));
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_text));
        jo::self<jo::CampScreen>(this)->brief_line = static_cast<std::int16_t>(-4);
        jo::self<jo::CampScreen>(this)->brief_px = 0;
        static const img::Addr kNames[8] = {d::s_MISSION_BRIEF_1, d::s_MISSION_BRIEF_2, d::s_MISSION_BRIEF_3, d::s_MISSION_BRIEF_4,
                                                 d::s_MISSION_BRIEF_5, d::s_MISSION_BRIEF_6, d::s_MISSION_BRIEF_7, d::s_MISSION_BRIEF_8};  // MISSION_BRIEF_1..8
        const std::uint32_t k = u(static_cast<std::int32_t>(jo::self<jo::CampScreen>(this)->mission)) - 1u;
        if (k > 7u) return;
        char name[0x44];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(name))), joan::orig::arg::str(str(kNames[k])));  // strcpy
        std::uint8_t* const snd = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(obj(d::g_sound_def_table))->find_select(static_cast<const char*>(name)));  // 0x004d0d80
        joan::orig::as_scenario(kSounds)->trigger_slot(static_cast<std::int32_t>(static_cast<std::int16_t>(jo::self<jo::WaveRes>(snd)->index)));  // 0x004cecd0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Briefing::start", 0x00724bf0, &H::briefing_start);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ObjMenu::open_brief() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15::H::open_brief>(this)); }  // 0x7246f0
auto joan::orig::ObjMenu::reset_briefing() -> void { joan::bind::member<&::game_b92_06::H::briefing_start>(this); }  // 0x724bf0
