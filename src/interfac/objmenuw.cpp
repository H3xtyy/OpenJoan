// interfac/objmenuw.cpp: functions of the original d:\ww\interfac\objmenuw.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include <cmath>
#include "../x87.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b115_08.cpp (fix/uninit)
//
//  - 0x0073a320 ObjMenu::siege_help() (thiscall, ret 0; one caller 0x00721166, eax unused;
//    "interfac/objmenuw.cpp"): siege-engine preview / help window. [ebp-0x23c] (1-based index
//    of the idle animation, entry [0] == 0 && [4] == 1) is written only when the search finds
//    one, then used unconditionally (0x004b2730 / 0x004168d0 / 0x004174f0 / 0x005abe30 set the
//    preview model's "F_IDLE" animation from it). Fix: no idle entry -> the animation is not
//    set (those four getters / the setter are skipped). Everything else exact.
namespace {
namespace game_b115_08 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;
using joan::orig::helpers::wadd;

JOAN_COUNTER(c_help, "ObjMenu::siege_help_73a320");

struct H {
    // ------------------------------------------------------------------ 0x0073a320 (fix)
    bool siege_help() {
        c_help.hit();
        void* const help = obj(d::g_popup_a52df8);
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->visible) != 9) return false;
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->help_open) != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_popup(help)->poll_buttons()) != 0 && joan::orig::arg::as<i32>(joan::orig::as_popup(help)->result()) == 4) {  // 0x006d5120 / 0x006d5370
                jo::self<jo::CampScreen>(this)->help_open = 0;
                joan::orig::as_popup(help)->free_gpopup();  // 0x006d3c60
            }
            return true;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]))->mouse(0x1b, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {  // 0x006218c0
            joan::orig::as_camp_screen(this)->set_state_721a60(0u);  // 0x00721a60
            return true;
        }
        void* const types = obj(d::g_types);
        void* const mem = obj(d::g_mem);
        for (i32 i = 0; i < 6; ++i) {
            if (jo::view<i8>(jo::self<jo::CampScreen>(this)->siege_flags[i]) != 1) continue;  // +0x1ad..+0x1b2: six consecutive u8 flags
            if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::arg::as<void*>(jo::self<jo::CampScreen>(this)->siege_buttons[i]))->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0)) == 0) continue;
            if (jo::view<i32>(jo::self<jo::CampScreen>(this)->siege_selected) == i) return true;
            char path[0x100];
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(path)), str(d::s_DES));  // strcpy "DES\"
            static const img::Addr kNames[6] = {d::s_BATTERRAM, d::s_CROSSBOW, d::s_BOMBARD, d::s_CANNON, d::s_FAUCONNEAU, d::s_TREBUCHET};
            static const img::Addr kFiles[6] = {d::s_battering_ram_txt, d::s_crossbow_txt, d::s_bombard_txt, d::s_cannon_txt, d::s_fauconneau_txt, d::s_trebuchet_txt};
            const u32 id = joan::orig::arg::as<u32>(joan::orig::as_unit_types(types)->find(joan::orig::arg::str(str(kNames[i]))));  // 0x005270b0
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(path)), joan::orig::arg::str(str(kFiles[i])));  // strcat
            jo::UnitType* const rec = jo::as_unit_type(joan::orig::as_records_203(types)->get(joan::orig::arg::i16(id)));  // 0x0042d0a0
            alignas(alignof(void*)) u8 file[sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110];  // a File: 0x112 bytes on x64
            joan::orig::as_file(static_cast<void*>(file))->construct();  // 0x004179c0 File::File
            joan::orig::as_file(static_cast<void*>(file))->file_open(joan::orig::arg::str(static_cast<const char*>(path)), 1, static_cast<char>(0));  // 0x00586da0 open
            const i32 line = global<i16>(d::g_a2b59c);
            if (jo::self<jo::CampScreen>(this)->siege_desc_text != nullptr) {
                joan::orig::as_mem(mem)->del(jo::self<jo::CampScreen>(this)->siege_desc_text, str(d::s_d_ww_interfac_objmenuw_cpp), joan::orig::arg::i32(line + 0x59));  // 0x0058ed70
                jo::self<jo::CampScreen>(this)->siege_desc_text = nullptr;
            }
            const i32 size = wadd(joan::orig::as_file(static_cast<void*>(file))->file_size(), 1);   // 0x00587f30
            jo::self<jo::CampScreen>(this)->siege_desc_text = joan::orig::as_mem(mem)->add(joan::orig::arg::u32(wadd(size, 1)), str(d::s_d_ww_interfac_objmenuw_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2b59c)) + 0x5d));  // 0x0058e7b0
            std::memset(jo::self<jo::CampScreen>(this)->siege_desc_text, 0, joan::orig::arg::u32(wadd(size, 1)));  // memset
            joan::orig::as_file(static_cast<void*>(file))->file_read(jo::self<jo::CampScreen>(this)->siege_desc_text, joan::orig::arg::u32(size));  // 0x005873a0 read
            joan::orig::as_file(static_cast<void*>(file))->file_close();                         // 0x00587260 close
            if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model) != nullptr) {
                joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene))->remove_5ea2f0(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model)));  // 0x005ea2f0
                void* const old = jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model);
                if (old != nullptr) joan::orig::as_scene_object(old)->destroy(1u);  // 0x00420840 deleting dtor
                jo::self<jo::CampScreen>(this)->siege_model = nullptr;
                jo::self<jo::CampScreen>(this)->siege_actor_def = nullptr;
                if (jo::view<i32>(jo::self<jo::CampScreen>(this)->siege_model_id) > 0) joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(jo::self<jo::CampScreen>(this)->siege_model_id));  // 0x004b16a0
                jo::self<jo::CampScreen>(this)->siege_model_id = 0;
                jo::self<jo::CampScreen>(this)->siege_model_flags = 0;
            }
            jo::self<jo::CampScreen>(this)->siege_model_id = jo::view<u32>(jo::as_unit_type(joan::orig::as_records_203(types)->get(joan::orig::arg::i16(id)))->model_ids[0]);
            jo::self<jo::CampScreen>(this)->siege_model_flags = jo::view<u32>(jo::as_unit_type(joan::orig::as_records_203(types)->get(joan::orig::arg::i16(id)))->model_flags[0]);
            joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->set_params(joan::orig::arg::i16(jo::self<jo::CampScreen>(this)->siege_model_id), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(jo::self<jo::CampScreen>(this)->siege_model_flags != 0 ? 1 : 0))), static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::i16(0));  // 0x004b0ec0
            joan::orig::as_model_res(obj(d::g_model_res_mgr))->acquire(joan::orig::arg::i16(jo::self<jo::CampScreen>(this)->siege_model_id));  // 0x004b0f40
            void* const raw = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86; operator new
            void* const model = raw != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(raw)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
            jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model) = model;
            jo::self<jo::CampScreen>(this)->siege_actor_def = jo::as_model_entry(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(jo::self<jo::CampScreen>(this)->siege_model_id)))->actor_def;  // 0x0041a180
            jo::self<jo::CampScreen>(this)->siege_model->def = static_cast<jo::ActorDef*>(jo::self<jo::CampScreen>(this)->siege_actor_def);
            joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model))->init();  // 0x005aab70
            jo::self<jo::CampScreen>(this)->siege_model->kind = -1;
            jo::self<jo::CampScreen>(this)->siege_model->owner = nullptr;
            jo::Plane* const mdl = jo::self<jo::CampScreen>(this)->siege_model->plane;
            for (i32 j = 0; j < mdl->material_count; ++j) {
                u32 col0 = 0;
                void* const c0 = joan::orig::as_colour(&col0)->construct(0, 0, 0, 0);  // 0x0042a260
                joan::orig::as_colour(&mdl->materials[j].specular)->from_rgba(joan::orig::arg::mptr(c0));      // 0x005e95b0
                mdl->materials[j].power = 0;
                u32 col1 = 0;
                void* const c1 = joan::orig::as_colour(&col1)->construct(0xff, 0xff, 0xff, 0xff);
                joan::orig::as_colour(&mdl->materials[j].ambient)->from_rgba(joan::orig::arg::mptr(c1));
            }
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene))->add_actor(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model)));  // 0x005ea1f0
            joan::orig::as_model_instance(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model))->set_10a(1u);                 // 0x0042cf20
            joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model))->set_yaw(0u);                 // 0x005ac6a0
            void* const anims = obj(d::g_anim_res_mgr);
            jo::AnimSet* const set = jo::as_anim_set(joan::orig::as_anim_res_mgr(anims)->entry(joan::orig::arg::u32(jo::view<u32>(rec->model_flags[0]))));  // 0x004b2730
            i32 idle = 0;
            bool found = false;
            for (i32 k = set->group_count; k > 0; --k) {
                jo::AnimGroupRec* const a = jo::as_anim_group_rec(joan::orig::as_anim_set(set)->entry(joan::orig::arg::u32(k)));  // 0x004168d0
                if (a->is_jump == 0 && jo::view<u32>(a->kind) == 1) {
                    idle = k;
                    found = true;
                    break;
                }
            }
            if (found) {  // fix/uninit: without an idle entry the original uses a garbage index
                u8* const set2 = joan::orig::arg::as<u8*>(joan::orig::as_anim_res_mgr(anims)->entry(joan::orig::arg::u32(jo::view<u32>(rec->model_flags[0]))));
                jo::AnimGroupRec* const a = jo::as_anim_group_rec(joan::orig::as_anim_set(set2)->entry(joan::orig::arg::u32(idle)));
                jo::AnimDef* const anim = jo::as_anim_slot(joan::orig::as_anim_res_mgr(anims)->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(a->anim_id))))->anim;  // 0x004174f0
                joan::orig::as_actor(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model))->start_anim(joan::orig::arg::mptr(anim), str(d::s_F_IDLE), joan::orig::arg::u32(global<u32>(d::g_now)), 1u, 1u, 0x43fa0000u);  // 0x005abe30 (500.0f)
            }
            float cam[10];
            std::memset(cam, 0, sizeof cam);
            joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->frame(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x006140a0
            const float two = global<float>(d::flt_2);
            cam[1] = (rec->shape_box[0] + rec->shape_box[3]) / two;
            cam[2] = (rec->shape_box[1] + rec->shape_box[4]) / two;
            cam[3] = 20.0f;
            cam[4] = 0.0f;
            std::memcpy(&cam[5], "\xc2\xb8\x32\x3e", 4);  // 0x3e32b8c2
            u32 dist = 0;
            switch (i) {
                case 0:
                    dist = 0x442f0000u;
                    cam[3] = cam[3] + global<float>(d::flt_70);
                    break;
                case 1:
                    dist = 0x43480000u;
                    cam[3] = cam[3] + global<float>(d::flt_10);
                    break;
                case 2:
                    dist = 0x43af0000u;
                    cam[3] = cam[3] + global<float>(d::flt_20);
                    break;
                case 3:
                    dist = 0x43960000u;
                    break;
                case 4:
                    dist = 0x43480000u;
                    break;
                default:
                    dist = 0x446d8000u;
                    cam[3] = cam[3] + global<float>(d::flt_230);
                    break;
            }
            std::memcpy(&cam[7], &dist, 4);
            joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->set_frame(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x00614040
            jo::view<i32>(jo::self<jo::CampScreen>(this)->siege_selected) = i;
            jo::self<jo::CampScreen>(this)->u32_1bf = 0;
            joan::orig::as_file(static_cast<void*>(file))->destruct();  // 0x005872b0 File::~File
            return true;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->p_171))->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0)) == 0) return false;
        alignas(alignof(void*)) u8 file[sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110];  // a File: 0x112 bytes on x64
        joan::orig::as_file(static_cast<void*>(file))->construct();
        char path[0x100];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(path)), str(d::s_DES));
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(path)), str(d::s_siege_help_txt));  // "siege_help.txt"
        joan::orig::as_file(static_cast<void*>(file))->file_open(joan::orig::arg::str(static_cast<const char*>(path)), 1, static_cast<char>(0));
        const i32 size = wadd(joan::orig::as_file(static_cast<void*>(file))->file_size(), 1);
        void* const text = joan::orig::as_mem(mem)->add(joan::orig::arg::u32(wadd(size, 1)), str(d::s_d_ww_interfac_objmenuw_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2b59c)) + 0xec));
        std::memset(joan::orig::arg::mptr(text), 0, joan::orig::arg::u32(wadd(size, 1)));
        joan::orig::as_file(static_cast<void*>(file))->file_read(joan::orig::arg::mptr(text), joan::orig::arg::u32(size));
        joan::orig::as_popup(help)->set_text_6d5230(joan::orig::arg::str(text), static_cast<jo::Font2d*>(obj(d::g_font_goudy12_bold_black)));                          // 0x006d5230
        const char* const fnt = joan::orig::as_txt_res(obj(d::g_strings))->objmenu_4a();                 // 0x004dc550
        joan::orig::as_popup(help)->set_title(joan::orig::arg::str(fnt), static_cast<jo::Font2d*>(obj(d::g_font_goudy_bold_14)));                           // 0x006d52c0
        joan::orig::as_panel(help)->set_pos(joan::orig::arg::i16(0x25f), joan::orig::arg::i16(-1));                                      // 0x006d50e0
        joan::orig::as_popup(help)->set_buttons(4u);                                              // 0x006d5340
        joan::orig::as_popup(help)->load_gpopup();                                                 // 0x006d3be0
        joan::orig::as_mem(mem)->del(joan::orig::arg::mptr(text), str(d::s_d_ww_interfac_objmenuw_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2b59c)) + 0xf6));
        jo::self<jo::CampScreen>(this)->help_open = 1;
        joan::orig::as_file(static_cast<void*>(file))->destruct();
        return true;
    }
};

#define FIX(name, address, fn) JOAN_REPLACE("fix/uninit", name, address, fn)
// fix/uninit: no idle animation entry ([ebp-0x23c] unwritten) -> preview animation not set
FIX("ObjMenu::siege_help", 0x0073a320, &H::siege_help);
#undef FIX

}  // namespace game_b115_08
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_spinner, "Spinner::tick");

struct H {
    // ------------------------------------------------------------------ 0x0073b040
    void spinner_tick() {
        c_spinner.hit();
        if (jo::view<i32>(jo::self<jo::CampScreen>(this)->siege_selected) < 0) return;
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model) == nullptr) return;
        jo::self<jo::CampScreen>(this)->u32_1bf = jo::self<jo::CampScreen>(this)->u32_1bf + global<u32>(d::g_frame_elapsed_ms);
        if (jo::self<jo::CampScreen>(this)->u32_1bf < 0x32u) return;
        jo::self<jo::CampScreen>(this)->u32_1bf = jo::self<jo::CampScreen>(this)->u32_1bf - 0x32u;
        const float k = global<float>(d::flt_0_017453292);
        float v = joan::orig::arg::as<float>(joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model))->yaw()) / k;  // 0x005ac7e0
        v = v + global<float>(d::flt_1);                           // 1.0
        if (v >= global<float>(d::flt_360)) v = v - global<float>(d::flt_360);  // 360 (NaN skips)
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model))->set_yaw(joan::orig::arg::bits(v * k));  // 0x005ac6a0
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Spinner::tick", 0x0073b040, &H::spinner_tick);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::img::str;

using joan::call::thiscall;

JOAN_COUNTER(c_init_73b2e0, "SiegeDebrief::init_73b2e0");

constexpr img::Addr kUnits = d::g_units;
i32 unit_dead(i32 id) { return joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_rec(i32 id) { return joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0

struct H {
    void nop() {}

    void init_73b2e0() {
        c_init_73b2e0.hit();
        static const img::Addr kShort[6] = {d::s_batt, d::s_cros, d::s_bomb,
                                                 d::s_cann, d::s_fauc, d::s_treb};
        for (i32 i = 0; i < 6; ++i) {  // switch (i) cases 0..5 in table order
            char* dst = jo::self<jo::Button>(jo::self<jo::CampScreen>(this)->siege_buttons[i])->states[0].sprite;  // +0x97
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dst)), joan::orig::arg::str(&img::global<const char>(kShort[i])));  // strcpy
        }
        const auto type_of = [](img::Addr name) {
            return joan::orig::arg::as<i32>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(img::addr(name))));  // 0x005270b0
        };
        const i32 batt = type_of(d::s_BATTERRAM);  // 'BATTERRAM'
        const i32 cros = type_of(d::s_CROSSBOW);  // 'CROSSBOW'
        const i32 bomb = type_of(d::s_BOMBARD);  // 'BOMBARD'
        const i32 cann = type_of(d::s_CANNON);  // 'CANNON'
        const i32 fauc = type_of(d::s_FAUCONNEAU);  // 'FAUCONNEAU'
        const i32 treb = type_of(d::s_TREBUCHET);  // 'TREBUCHET'
        for (i32 k = joan::orig::as_dynarray(obj(kUnits))->size(); k >= 0; --k) {  // 0x00417ad0
            if (unit_dead(k) != 0) continue;
            if (jo::as_unit(unit_rec(k))->type_id == batt) {
                jo::self<jo::CampScreen>(this)->siege_flags[0] = 1;
            } else if (jo::as_unit(unit_rec(k))->type_id == cros) {
                jo::self<jo::CampScreen>(this)->siege_flags[1] = 1;
            } else if (jo::as_unit(unit_rec(k))->type_id == bomb) {
                jo::self<jo::CampScreen>(this)->siege_flags[2] = 1;
            } else if (jo::as_unit(unit_rec(k))->type_id == cann) {
                jo::self<jo::CampScreen>(this)->siege_flags[3] = 1;
            } else if (jo::as_unit(unit_rec(k))->type_id == fauc) {
                jo::self<jo::CampScreen>(this)->siege_flags[4] = 1;
            } else if (jo::as_unit(unit_rec(k))->type_id == treb) {
                jo::self<jo::CampScreen>(this)->siege_flags[5] = 1;
            }
        }
        void* bank = obj(d::g_texture_array);
        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(bank)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(bank)->set_mode(1u);                // 0x0060a740
        if (jo::self<jo::CampScreen>(this)->siege_tex_debrief == 0)
            jo::self<jo::CampScreen>(this)->siege_tex_debrief = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(bank)->load(joan::orig::arg::str(img::addr(d::s_debrief)), 1, 0u));  // 0x006099c0 'debrief'
        if (jo::self<jo::CampScreen>(this)->siege_tex_siege == 0)
            jo::self<jo::CampScreen>(this)->siege_tex_siege = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(bank)->load(joan::orig::arg::str(img::addr(d::s_mb_siege)), 1, 0u));  // 'mb_siege'
        joan::orig::as_texture_manager(bank)->set_mode(joan::orig::arg::u32(saved));  // 0x0060a740
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Screen::nop_73b140", 0x0073b140, &H::nop);
B("SiegeDebrief::init_73b2e0", 0x0073b2e0, &H::init_73b2e0);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b104_15.cpp (game/b104_15)
namespace {
namespace game_b104_15 {

using u32 = std::uint32_t;

using u8 = std::uint8_t;

struct H {
    // ------------------------------------------------------------------------ 0x0073b160
    std::uintptr_t reset_73b160() {
        joan::orig::as_obj_menu(this)->init_siege();  // 0x0073b2e0
        // +0x70 = menu_buttons[7] (the siege button), scale_to(1, 6.0f, 0.0f, 0, 200)
        const std::uintptr_t r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_widget(jo::self<jo::CampScreen>(this)->menu_buttons[7])->scale_to_61f1b0(joan::orig::arg::i16(1), joan::orig::arg::f32(0x40c00000u), joan::orig::arg::f32(0u), static_cast<char>(0), joan::orig::arg::u32(0xc8)));  // 0x0061f1b0
        jo::self<jo::CampScreen>(this)->help_open = 0;
        jo::self<jo::CampScreen>(this)->siege_selected = 0xffffffffu;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("Screen::reset_73b160", 0x0073b160, &H::reset_73b160);
#undef B

}  // namespace game_b104_15
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::obj;

using u16 = std::uint16_t;

using joan::call::thiscall;

JOAN_COUNTER(c_reset_model, "X::reset_model");

struct H {
    void reset_model() {  // 0x0073b1c0
        c_reset_model.hit();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]))->scale_to(0, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);  // 0x0061f1b0
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model) != nullptr) {
            joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene))->remove_5ea2f0(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model)));  // 0x005ea2f0
            void* const o = jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model);
            if (o != nullptr) joan::orig::as_scene_object(o)->destroy(1u);  // 0x00420840
            jo::self<jo::CampScreen>(this)->siege_model = nullptr;
            jo::self<jo::CampScreen>(this)->siege_actor_def = nullptr;
            if (jo::view<i32>(jo::self<jo::CampScreen>(this)->siege_model_id) > 0) {
                const std::uintptr_t arg = (reinterpret_cast<std::uintptr_t>(this) & 0xffff0000u) | static_cast<u16>(jo::self<jo::CampScreen>(this)->siege_model_id);  // low word (bits 16..31 are `this`' as the original left them; the callee reads a short)
                joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(arg));  // 0x004b16a0
            }
            jo::self<jo::CampScreen>(this)->siege_model_id = 0;
            jo::self<jo::CampScreen>(this)->siege_model_flags = 0;
        }
        joan::orig::as_obj_menu(this)->free_siege_images();  // 0x0073b6e0
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::reset_model", 0x0073b1c0, &H::reset_model);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

using i32 = std::int32_t;

using joan::img::obj;


void* const kRes = obj(d::g_texture_array);

void res_free(void* handle) { joan::orig::as_texture_manager(kRes)->release(joan::orig::arg::mptr(handle)); }  // 0x0060a550

struct H {
    void free_1b3() {  // 0x0073b6e0
        if (jo::self<jo::CampScreen>(this)->siege_tex_debrief != nullptr) {
            res_free(jo::self<jo::CampScreen>(this)->siege_tex_debrief);
            jo::self<jo::CampScreen>(this)->siege_tex_debrief = nullptr;
        }
        if (jo::self<jo::CampScreen>(this)->siege_tex_siege != nullptr) {
            res_free(jo::self<jo::CampScreen>(this)->siege_tex_siege);
            jo::self<jo::CampScreen>(this)->siege_tex_siege = nullptr;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Res2::free_1b3", 0x0073b6e0, &H::free_1b3);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using u16 = std::uint16_t;

using joan::hook::x87::Fpu;

using joan::img::str;

using joan::hook::x87::ftol;

using joan::call::thiscall;
u32 hi(u32 v) { return v & 0xffff0000u; }

JOAN_COUNTER(c_button_draw, "Button::draw_73b770");

// ---- shared draw helpers ----------------------------------------------------------------
void* const kSprites = obj(d::g_sprites_a46128);
// 0x0042a260 (thunk 0x004098b8, ret 0x10): packs r, g, b, a into the 4-byte colour object.
u32 packed_colour(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));
}
// 0x00633500 (thunk 0x00401bbd, ret 0x44) sprite queue, with the 11 trailing constants
// (3, 0, mode, 0, -1, -1, -1, -1, 0, -1, -1) every caller here pushes.
void sprite(u32 x, u32 y, u32 a3, const void* name, std::uintptr_t sheet, u32 colour, i32 mode) {
    joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), joan::orig::arg::mptr(sheet), joan::orig::arg::u32(colour), 3, 0, joan::orig::arg::i16(mode), 0, -1, -1, -1, -1, 0, -1, -1);
}

// ---- 0x0073b770: button draw callback (cdecl) --------------------------------------------
u32 JOAN_CDECL button_draw(jo::Widget* w, u32 arg, u32 flags) {
    const u32 fl = flags & 0xff;
    if (fl != 7 && (fl & 1) != 0) return 0;
    c_button_draw.hit();
    i16 x = w->x;
    i16 y = w->y;
    i16 ww = w->x2;
    i16 h = w->y2;
    u32 sx = 0x3f800000u;  // [ebp-0x14]
    u32 sy = 0x3f800000u;  // [ebp-0x18]
    if (w->fit != 0) {
        joan::orig::as_sprite2d(kSprites)->fit_rect(joan::orig::arg::i16(w->x), joan::orig::arg::i16(w->y), joan::orig::arg::i16(w->x2), joan::orig::arg::i16(w->y2), &sx, &sy, &x, &y, &ww, &h, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    }
    auto colour = [&]() {
        const i32 alpha = ftol(Fpu{true}.mul(static_cast<double>(global<float>(d::flt_255)),
                                             static_cast<double>(w->scale)));
        return packed_colour(0xff, 0xff, 0xff, static_cast<u32>(alpha));
    };
    const u32 c1 = colour();
    sprite(static_cast<u32>(static_cast<i32>(x) + 5), static_cast<u32>(static_cast<i32>(y) + 5), arg, jo::self<jo::Button>(w)->states[0].sprite,  // +0x97: w is a Button (0x00620ee0)
          
           img::addr(d::g_bmp_objmenu), c1, 2);
    if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(x), static_cast<i32>(y), static_cast<i32>(ww), static_cast<i32>(h))) != 0) {  // 0x00593980
        const u32 c2 = colour();
        sprite(hi(c2) | static_cast<u16>(x), hi(c2) | static_cast<u16>(y), arg, str(d::s_sm_fh), img::addr(d::g_bmp_debrief), c2, 2);
    } else {
        const u32 c3 = colour();
        sprite(hi(c3) | static_cast<u16>(x), hi(c3) | static_cast<u16>(y), arg, str(d::s_sm_f), img::addr(d::g_bmp_debrief), c3, 2);
    }
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Button::draw_73b770", 0x0073b770, &button_draw);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ObjMenu::update_siege() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b115_08::H::siege_help>(this)); }  // 0x73a320
auto joan::orig::ObjMenu::spin_siege_model() -> void { joan::bind::member<&::game_b96_08::H::spinner_tick>(this); }  // 0x73b040
auto joan::orig::Screen::nop_73b140() -> void { joan::bind::member<&::game_b103_16::H::nop>(this); }  // 0x73b140
auto joan::orig::Screen::reset_73b160() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15::H::reset_73b160>(this)); }  // 0x73b160
auto joan::orig::ObjMenu::leave_siege() -> void { joan::bind::member<&::game_b94_08::H::reset_model>(this); }  // 0x73b1c0
auto joan::orig::ObjMenu::init_siege() -> void { joan::bind::member<&::game_b103_16::H::init_73b2e0>(this); }  // 0x73b2e0
auto joan::orig::ObjMenu::free_siege_images() -> void { joan::bind::member<&::game_b93_06::H::free_1b3>(this); }  // 0x73b6e0
