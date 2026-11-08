// game/unknown_mission.cpp: functions whose original source file is unknown (formerly game/_unplaced/mission.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;

using joan::call::thiscall;

using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::img::str;

void* const kGameUnits = obj(d::g_units);  // 0x00428b70 missing / 0x0041b3c0 get
void* const kTypes = obj(d::g_types);      // 0x0042d0a0

using joan::hook::x87::ftol;

}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using joan::hook::global;

using joan::call::cdecl_call;

using u32 = std::uint32_t;

JOAN_COUNTER(c_reset_42d810, "Obj::reset_42d810");

void* memset_(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

using joan::orig::helpers::pi_from_atan;
void JOAN_CDECL init_a336d4() { global<float>(d::g_pi_a336d4) = pi_from_atan(); }

struct H {
    // ------------------------------------------------------------------ 0x0042d810
    void* reset_42d810() {
        c_reset_42d810.hit();
        memset_(this, 0, static_cast<u32>(sizeof(jo::Player)));  // 0x126e on x86
        jo::self<jo::Player>(this)->button_down[0] = 0;
        jo::self<jo::Player>(this)->button_down[1] = 0;
        jo::self<jo::Player>(this)->button_down[2] = 0;
        jo::self<jo::Player>(this)->button_down[3] = 0;
        jo::self<jo::Player>(this)->button_up[0] = 1;
        jo::self<jo::Player>(this)->button_up[1] = 1;
        jo::self<jo::Player>(this)->button_up[2] = 1;
        jo::self<jo::Player>(this)->button_up[3] = 1;
        jo::view<u32>(jo::self<jo::Player>(this)->mode) = 0;
        jo::self<jo::Player>(this)->riding = 0;
        jo::self<jo::Player>(this)->mounted = 0;
        jo::view<u16>(jo::self<jo::Player>(this)->squad_mode) = 0;
        jo::self<jo::Player>(this)->u16_7a = 0;
        jo::self<jo::Player>(this)->aiming = 1;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::reset_42d810", 0x0042d810, &H::reset_42d810);
B95("init_a336d4", 0x00430210, &init_a336d4);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

struct H {
    // 0x0042d8d0 List::empty_at(i)
    std::int32_t empty_at(std::int32_t i) { return joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)) == nullptr ? 1 : 0; }  // 0x0041b3f0
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("List::empty_at", 0x0042d8d0, &H::empty_at);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

using joan::hook::global;

// g_out = g_in / g_div (fld dword / fdiv dword / fstp dword).
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a336e0() { div_global(d::g_a336e0, d::g_pi_a336d4, d::flt_4); }  // 0x00430310

struct H {
    // 0x0042d900
    void free_18() {
        joan::orig::as_world(this)->leave_mode(static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x0042df50
        if (jo::view<void*>(jo::self<jo::Player>(this)->cam_zones) == nullptr) return;
        void* const p = jo::view<void*>(jo::self<jo::Player>(this)->cam_zones);
        if (p != nullptr) (void)joan::orig::arg::as<void*>(joan::orig::as_camera_zones(p)->destroy(1u));  // 0x00420150
        jo::view<void*>(jo::self<jo::Player>(this)->cam_zones) = nullptr;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::free_18", 0x0042d900, &H::free_18);
B94("init_a336e0", 0x00430310, &init_a336e0);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using joan::hook::global;

void* const kCameras = obj(d::g_camera_man);

using u32 = std::uint32_t;

void* const kUnits = obj(d::g_units);
void* const kScreen = obj(d::g_mouse);

struct Vec3 {
    float v[3];  // floats: the struct is passed by value to GroundObject::set_position (System V passes float members in XMM registers)
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* unit(u32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0

JOAN_COUNTER(c_finish_state, "Unit::finish_state_42d980");

struct H {
    // ------------------------------------------------------------------ 0x0042d980
    void finish_state() {
        c_finish_state.hit();
        joan::orig::as_parser(kScreen)->drain();  // 0x005959c0
        joan::orig::as_panel(this)->load_cursor();     // 0x0043f7c0
        const i32 mode = jo::self<jo::Player>(this)->mode;
        if (global<i32>(d::g_a48cef) == 0) {
            void* const cam = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCameras)->motion(0));  // 0x00563c80
            joan::orig::as_camera_motion(cam)->retarget(joan::orig::arg::mptr(&jo::self<jo::Player>(this)->camera));               // 0x00423f40
        }
        jo::self<jo::Player>(this)->u8_79 = 0;
        if (mode == 4)
            jo::view<u32>(jo::self<jo::Player>(this)->prev_mode) = global<u32>(d::g_cfg_control_scheme);
        else
            jo::view<u32>(jo::self<jo::Player>(this)->mode) = global<u32>(d::g_cfg_control_scheme);
        if (mode == 4) {
            jo::self<jo::Player>(this)->mode = 4;
            joan::orig::as_camera_man(kCameras)->set_mode(4);  // 0x00429860
            joan::orig::as_player(this)->init_rts();         // 0x0043f940
            const u32 k = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(kCameras)->target());              // 0x00563dc0
            void* const cam = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCameras)->motion(joan::orig::arg::i32(k)));     // 0x00563c80
            joan::orig::as_camera_motion(cam)->start(joan::orig::arg::mptr(&jo::self<jo::Player>(this)->camera), joan::orig::arg::mptr(&jo::self<jo::Player>(this)->camera));  // 0x00423ec0
        }
        void* const u = unit(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        if (static_cast<i32>(jo::as_unit(u)->action) == 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(u)->mounted()) == 0 && joan::orig::arg::as<i32>(joan::orig::as_world(u)->flag_1fd()) == 0 &&
            static_cast<i32>(jo::view<i8>(jo::as_unit(u)->dismounted_flag)) != -1) {
            joan::orig::as_unit(u)->set_action(1, 0, 1);  // 0x00501190
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->mounted()) == 0 && static_cast<i32>(jo::view<i8>(jo::as_unit(u)->dismounted_flag)) != -1) return;  // 0x00430190
        if (static_cast<i32>(jo::view<i8>(jo::as_unit(u)->mounted_flag)) == -1) {
            void* const leader = unit(jo::view<u32>(jo::as_unit(u)->mount_id));
            const float h = joan::orig::arg::as<float>(joan::orig::as_ground_object(leader)->heading());  // 0x004ff850
            joan::orig::as_unit(u)->set_target(joan::orig::arg::f32(h));                      // 0x004ff870
            joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(h / global<float>(d::flt_0_017453292)));  // 0x0050abd0
            jo::as_unit(u)->mounted_flag = 1;
            if (jo::as_unit(u)->model != nullptr) joan::orig::as_model_instance(jo::as_unit(u)->model)->set_10a(0u);  // 0x0042cf20
            joan::orig::as_unit(u)->play_idle(0, 1);                      // 0x00508b90
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_cell_object(u)->find_anim_state());              // 0x00504010
            joan::orig::as_cell_object(u)->set_anim_state(joan::orig::arg::i32(r), joan::orig::arg::i32(global<u32>(d::g_now)), joan::orig::arg::i32(-1));  // 0x004ff9f0
            jo::as_unit(u)->mount_busy = 0;
            return;
        }
        if (static_cast<i32>(jo::view<i8>(jo::as_unit(u)->dismounted_flag)) == -1) {
            void* const model = jo::as_unit(u)->actor;
            if (jo::view<u32>(jo::as_actor(model)->record) != 0)
                joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(u)->actor)->record)), joan::orig::arg::i16(0));  // 0x006ad980
            Vec3 pos;
            joan::orig::as_unit(u)->position(&pos);  // 0x004fedb0
            Vec3 dir;
            void* const frame = jo::view<void*>(jo::as_actor(jo::as_unit(u)->actor)->bone);
            std::memcpy(&dir.v[0], &jo::as_bone(frame)->local[4], sizeof(float));
            std::memcpy(&dir.v[1], &jo::as_actor(jo::as_unit(u)->actor)->bone->local[5], sizeof(float));
            std::memcpy(&dir.v[2], &jo::as_actor(jo::as_unit(u)->actor)->bone->local[6], sizeof(float));
            Vec3 tmp;
            const Vec3* const scaled = joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&tmp, &dir, 30.0f));  // 0x0060cd00
            Vec3 off = *scaled;
            joan::orig::vec_add_in_place(&pos, &off);  // 0x0060cdc0
            joan::orig::as_ground_object(u)->set_position(pos);          // 0x004fedf0
            float h = joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());   // 0x004ff850
            h = h - global<float>(d::g_pi_a336d4) / global<float>(d::flt_2);
            joan::orig::as_unit(u)->set_target(joan::orig::arg::f32(h));                              // 0x004ff870
            joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(h / global<float>(d::flt_0_017453292)));  // 0x0050abd0
            if (static_cast<i32>(jo::as_unit(u)->type_id) != 0) {
                void* const type = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->type_id))));  // 0x0042d0a0
                jo::as_actor(jo::as_unit(u)->actor)->aim_mode = jo::as_unit_type(type)->shape_kind;
            }
            jo::as_unit(u)->dismounted_flag = 1;
            if (jo::as_unit(u)->model != nullptr) joan::orig::as_model_instance(jo::as_unit(u)->model)->set_10a(1u);  // 0x0042cf20
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0)                                          // 0x0042d0e0
                joan::orig::as_unit(u)->set_action(0x2a, 0, 1);                                  // 0x00501190
            else
                joan::orig::as_unit(u)->play_idle(0, 1);                        // 0x00508b90
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_cell_object(u)->find_anim_state());                    // 0x00504010
            joan::orig::as_cell_object(u)->set_anim_state(joan::orig::arg::i32(r), joan::orig::arg::i32(global<u32>(d::g_now)), joan::orig::arg::i32(-1));  // 0x004ff9f0
            jo::as_unit(u)->mount_busy = 0;
            return;
        }
        joan::orig::as_unit(u)->set_action(0x43, 0, 1);                       // 0x00501190
        joan::orig::as_unit(unit(jo::view<u32>(jo::as_unit(u)->mount_id)))->set_action(1, 0, 1);  // 0x00501190
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Unit::finish_state_42d980", 0x0042d980, &H::finish_state);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b87_05.cpp (game/b87_05)
namespace {
namespace game_b87_05 {

using joan::hook::global;

using joan::call::cdecl_call;

void* const kStations = obj(d::g_nations);

std::uintptr_t u32(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }  // the pointer, whole (the result register is ignored by the callers)

void* const kAnims = obj(d::g_physic_sys);      // 0x006af310 / 0x006af370 / 0x006aee40
void* const kPanel = obj(d::g_player_hud);

std::uint32_t unit_missing(std::int32_t id) { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id))); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint32_t station_missing(std::int32_t id) { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_list(kStations)->is_null(joan::orig::arg::i32(id))); }  // 0x00484870
void* station_get(std::int32_t id) { return joan::orig::as_dynarray(kStations)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0047b600

JOAN_COUNTER(c_set_unit, "Player::set_unit");
JOAN_COUNTER(c_group_release, "Group::release");
JOAN_COUNTER(c_member_release, "Member::release");

struct H {
    // ------------------------------------------------------------ 0x0042f9c0 (ret 4)
    std::uintptr_t set_unit(std::int32_t id) {
        c_set_unit.hit();
        const std::uint32_t missing = unit_missing(id);
        if (missing != 0) return missing & 0xffff0000u;
        std::uint8_t* const unit = unit_get(id);       // ebp-0x8
        const std::int32_t old = jo::self<jo::Player>(this)->unit_id_;  // ebp-0xc
        jo::self<jo::Player>(this)->unit_id_ = id;
        global<std::int16_t>(d::g_a47c36) = static_cast<std::int16_t>(id);
        joan::orig::as_hud(kPanel)->update_health_state(joan::orig::arg::i32(id));  // 0x00703750
        char* const def = joan::orig::arg::as<char*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
        char model[0x100];  // ebp-0x8c (0x80 in the original)
        char base[0x100];   // ebp-0x110
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(model))), joan::orig::arg::str(jo::as_unit_type(def)->anim_name));  // strcpy
        char* dot = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(static_cast<char*>(model)), 0x2e));  // strrchr
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dot)), str(d::s_P_AM1));  // "_P.AM1"
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(base))), joan::orig::arg::str(static_cast<char*>(model)));
        dot = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(static_cast<char*>(base)), 0x2e));
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dot)), str(d::kEmpty));  // ""
        joan::orig::as_hud(kPanel)->nop_6fe480(static_cast<char*>(model), static_cast<char*>(base));  // 0x006fe480
        if (jo::as_unit(unit)->cell != nullptr && jo::view<void*>(jo::as_world_cell(jo::as_unit(unit)->cell)->camera_zones) != nullptr)
            joan::orig::as_camera_zones(jo::view<void*>(jo::as_world_cell(jo::as_unit(unit)->cell)->camera_zones))->set_unit(joan::orig::arg::u32(id));  // 0x00428b00
        if (unit_missing(old) == 0) {
            std::uint8_t* const prev = unit_get(old);  // ebp-0x114
            jo::as_unit(prev)->layer = 1;
            if (jo::as_unit(prev)->actor != nullptr) {
                std::uint8_t* const anim =
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(kAnims)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(prev)->actor)->record))));  // 0x006af310
                if (anim != nullptr) {
                    jo::as_shot(anim)->type = 1;
                    joan::orig::as_anim_table(kAnims)->restart_6af370(joan::orig::arg::u32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(prev)->actor)->record)), joan::orig::arg::u32(jo::as_shot(anim)->u32_27a /* x86 +0x27a; the Players list holds Shot records (List6b2040::add_new 0x006b2040 allocates sizeof(Shot); Body is a view that drifts on x64) */), joan::orig::arg::u32(static_cast<std::int16_t>(jo::as_shot(anim)->type)), 0u, 2u, 1u);                                                         // 0x006af370
                    joan::orig::as_unit(kAnims)->place_record(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(prev)->actor)->record)));  // 0x006b0450
                }
            }
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_world(prev)->flag_1fd()) == 0)  // 0x0042d690
                joan::orig::as_unit(prev)->play_idle(0, 1);               // 0x00508b90
        }
        jo::view<std::int32_t>(jo::as_unit(unit)->combo_time) = 0;
        if (jo::self<jo::Player>(this)->mode == 4)
            jo::as_unit(unit)->layer = 1;
        else
            jo::as_unit(unit)->layer = 0;
        std::uintptr_t eax = u32(unit);
        if (jo::self<jo::Player>(this)->mode != 4 && jo::as_unit(unit)->actor != nullptr) {
            std::uint8_t* const anim =
                joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(kAnims)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(unit)->actor)->record))));  // 0x006af310
            eax = u32(anim);
            if (anim != nullptr) {
                jo::as_shot(anim)->type = 0;
                eax = joan::orig::arg::as<std::uint32_t>(joan::orig::as_anim_table(kAnims)->restart_6af370(joan::orig::arg::u32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(unit)->actor)->record)), joan::orig::arg::u32(jo::as_shot(anim)->u32_27a /* x86 +0x27a; the Players list holds Shot records (List6b2040::add_new 0x006b2040 allocates sizeof(Shot); Body is a view that drifts on x64) */), joan::orig::arg::u32(static_cast<std::int16_t>(jo::as_shot(anim)->type)), 0u, 2u, 1u));  // 0x006af370
            }
        }
        return (eax & ~std::uintptr_t{0xffffu}) | 1u;
    }

    // ------------------------------------------------------------ 0x0042dd70
    std::uintptr_t group_release() {  // EAX: this / 0
        c_group_release.hit();
        if (jo::self<jo::Unit>(this)->group == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        for (std::int32_t i = static_cast<std::int32_t>(jo::as_group(jo::self<jo::Unit>(this)->group)->count); i > 0; --i) {
            const std::int32_t id = static_cast<std::int32_t>(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i - 1]);  // ebp-0xc
            if (unit_missing(id) != 0) continue;
            joan::orig::as_member(unit_get(id))->release();  // 0x0042fcb0
            jo::view<std::int16_t>(jo::as_unit(unit_get(id))->team_flag) = 1;
        }
        void* const group = jo::self<jo::Unit>(this)->group;
        joan::crt::operator_delete(joan::orig::arg::mptr(group));  // operator delete
        jo::self<jo::Unit>(this)->group = nullptr;
        std::uint32_t eax = station_missing(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side));
        if (eax == 0)  // eax stays 0: the callee is void and every caller ignores the result
            joan::orig::as_nation(station_get(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side)))->update_unit(this);  // 0x004b5e40
        return eax;
    }

    // ------------------------------------------------------------ 0x0042fcb0
    std::uintptr_t member_release() {  // EAX: this / 0. The object is a Unit (x86: leader_id +0x37, group_id +0x31, side +2, speech_off +0x2f; the Member view drifts on x64)
        c_member_release.hit();
        if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->leader_id) == 0) return reinterpret_cast<std::uintptr_t>(this);
        std::uint8_t* const leader = unit_get(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->leader_id));  // ebp-0x8
        joan::orig::as_group(jo::as_unit(leader)->group)->remove_member(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id));  // 0x005288f0
        jo::self<jo::Unit>(this)->leader_id = 0;
        jo::self<jo::Unit>(this)->group_id = 0;
        std::uint32_t eax = station_missing(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side));
        if (eax == 0)  // eax stays 0: the callee is void and every caller ignores the result
            joan::orig::as_nation(station_get(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side)))->update_unit(this);  // 0x004b5e40
        jo::self<jo::Unit>(this)->speech_off = 0;
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_05", name, addr, fn)
B("Player::set_unit", 0x0042f9c0, &H::set_unit);
B("Group::release", 0x0042dd70, &H::group_release);
B("Member::release", 0x0042fcb0, &H::member_release);
#undef B

}  // namespace game_b87_05
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::hook::global;

using u32 = std::uint32_t;

JOAN_COUNTER(c_screen_reset, "Screen::reset");

struct H {
    // ------------------------------------------------------------------------ 0x0042de70
    void screen_reset() {
        c_screen_reset.hit();
        const u32 saved = jo::view<u32>(jo::self<jo::Player>(this)->unit_id_);
        joan::orig::as_world(this)->leave_mode(static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x0042df50
        jo::view<u32>(jo::self<jo::Player>(this)->mode) = 0;
        jo::view<u32>(jo::self<jo::Player>(this)->unit_id_) = saved;
        jo::self<jo::Player>(this)->u8_7c = 0;
        joan::orig::as_panel(this)->load_cursor();  // 0x0043f7c0
        jo::view<u32>(jo::self<jo::Player>(this)->zoom_dist) = 0x442f0000u;        // 700.0f
        joan::orig::as_parser(obj(d::g_mouse))->drain();  // 0x005959c0
        if (global<i32>(d::g_cfg_control_scheme) == 1) {
            if (*global<i32*>(d::g_keys_a51806) != 0) {
                joan::orig::as_unit(this)->set_state(1, static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x0042e040
            } else {
                global<i32>(d::g_cfg_control_scheme) = 0;
                joan::orig::as_unit(this)->set_state(0, static_cast<std::int8_t>(joan::orig::arg::i32(0)));
            }
        } else {
            joan::orig::as_unit(this)->set_state(joan::orig::arg::i32(global<i32>(d::g_cfg_control_scheme)), static_cast<std::int8_t>(joan::orig::arg::i32(0)));
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Screen::reset", 0x0042de70, &H::screen_reset);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::hook::global;

using joan::call::cdecl_call;

// ---------------------------------------------------------------------- static initialisers
void init_pi(img::Addr g) {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(g) = static_cast<float>(t * 4.0);        // qword 0x9cf788 = 4.0 (exact)
}

static void JOAN_CDECL init_pi_a39d14() { init_pi(d::g_a39d14); }  // 0x0042df20

struct H {
    // ------------------------------------------------------------------ 0x004300e0
    u8 byte_81() { return jo::self<jo::Player>(this)->blocked; }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_pi_a39d14", 0x0042df20, &init_pi_a39d14);
B97("Obj::byte_81", 0x004300e0, &H::byte_81);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
namespace {
namespace game_b84_03 {

JOAN_COUNTER(c_leave_mode, "World::leave_mode");

struct H {
    // ------------------------------------------------------------------ 0x0042df50
    H* leave_mode(std::int8_t keep) {
        c_leave_mode.hit();
        if (static_cast<std::int32_t>(keep) == 0) {
            joan::orig::as_player(this)->reset_command();  // 0x0042a770
            joan::orig::as_player(this)->reset_mount();  // 0x0043c500
            joan::orig::as_player(this)->reset_squad_command();  // 0x0042d1d0
            jo::view<std::uint32_t>(jo::self<jo::Player>(this)->unit_id_) = 0;
        }
        switch (jo::view<std::uint32_t>(jo::self<jo::Player>(this)->mode)) {
            case 0: joan::orig::as_player(this)->leave_mode0(); break;  // 0x004343c0
            case 1: joan::orig::as_player(this)->leave_mode1(); break;  // 0x00432d10
            case 2: joan::orig::as_unit(this)->stop_438b00(); break;  // 0x00438b00
            case 3: joan::orig::as_player(this)->leave_mode3(); break;  // 0x00430460
            case 4: joan::orig::as_game(this)->start_rts(static_cast<std::int8_t>(joan::orig::arg::i32(keep))); break;  // 0x004404c0
            default: break;
        }
        joan::orig::as_parser(obj(d::g_mouse))->drain();  // 0x005959c0
        jo::self<jo::Player>(this)->blocked = 0;
        jo::self<jo::Player>(this)->u8_79 = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("World::leave_mode", 0x0042df50, &H::leave_mode);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

using joan::hook::global;

// ---- static initialisers / free functions ----
using joan::orig::helpers::div_init;
void JOAN_CDECL init_42e010() { div_init(d::g_a39d98, d::g_a39d14, d::flt_2); }
void JOAN_CDECL init_430270() { div_init(d::g_a336d8, d::g_pi_a336d4, d::flt_2); }

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("init_42e010", 0x0042e010, &init_42e010);
B("init_430270", 0x00430270, &init_430270);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using joan::hook::global;

JOAN_COUNTER(c_set_state, "Unit::set_state");
JOAN_COUNTER(c_state_sound, "Unit::state_sound");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x0042e040
    void set_state(std::int32_t state, std::int8_t b) {
        c_set_state.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->blocked) != 0) return;
        if (global<std::int32_t>(d::g_a48cef) == 2) return;
        if (jo::self<jo::Player>(this)->squad_mode != 0) return;
        if (jo::self<jo::Player>(this)->mode == 4 && state != jo::self<jo::Player>(this)->prev_mode) return;
        if (jo::self<jo::Player>(this)->mode == state) return;
        joan::orig::as_world(self())->leave_mode(static_cast<std::int8_t>(joan::orig::arg::i32(1)));  // 0x0042df50
        if (jo::self<jo::Player>(this)->mode == 4) {
            if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Player>(this)->unit_id_))) == 0) {  // 0x00428b70
                void* const u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
                joan::orig::as_unit(u)->release_slot();                                                            // 0x00528300
            }
        }
        void* const p = global<void*>(d::g_world);
        if (state == 4) {
            if (p != nullptr && jo::as_world(p)->scene != nullptr) joan::orig::as_scene(jo::view<void*>(jo::as_world(p)->scene))->set_6c(0u);  // 0x005ef400
        } else {
            if (p != nullptr && jo::as_world(p)->scene != nullptr) joan::orig::as_scene(jo::view<void*>(jo::as_world(p)->scene))->set_6c(1u);  // 0x005ef400
        }
        jo::self<jo::Player>(this)->blocked = 1;
        jo::view<std::int8_t>(jo::self<jo::Player>(this)->u8_79) = b;
        jo::self<jo::Player>(this)->prev_mode = jo::self<jo::Player>(this)->mode;
        jo::self<jo::Player>(this)->mode = state;
    }

    // 0x0042fd60
    float link_value_of() {
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Player>(this)->unit_id_))) != 0)  // 0x00428b70
            return global<float>(d::k_9cf5c4);
        void* const u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        return joan::orig::arg::as<float>(joan::orig::as_unit(u)->anim_phase());                                              // 0x005045f0
    }

    // 0x0042fef0
    void state_sound() {
        c_state_sound.hit();
        switch (jo::view<std::uint32_t>(jo::self<jo::Player>(this)->mode)) {
            case 0: joan::orig::as_cursor(obj(d::g_mouse))->set_pos(); break;  // 0x00592bd0
            case 1: joan::orig::as_cursor(obj(d::g_mouse))->set_pos(); break;  // 0x00592bd0
            case 2: joan::orig::as_cursor(obj(d::g_mouse))->reset_pos(); break;  // 0x00592c30
            case 3: joan::orig::as_cursor(obj(d::g_mouse))->set_pos(); break;  // 0x00592bd0
            case 4: jo::self<jo::Player>(this)->pick_kind = 0; break;
            default: break;
        }
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Unit::set_state", 0x0042e040, &H::set_state);
M("Unit::link_value_of", 0x0042fd60, &H::link_value_of);
M("Unit::state_sound", 0x0042fef0, &H::state_sound);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

JOAN_COUNTER(c_update_mode, "Game::update_mode");

struct H {
    // 0x0042e160 copies record i (1-based, 0x18c bytes) of +2 to +0xc (rep movsd, forward).
    std::uint32_t load_record(std::int32_t i) {
        const std::uint32_t off = static_cast<std::uint32_t>(i - 1) * static_cast<std::uint32_t>(sizeof(jo::LightPreset));
        const std::uint32_t* src = reinterpret_cast<const std::uint32_t*>(jo::view<std::uint8_t*>(jo::self<jo::LightSys>(this)->sets) + off);
        std::uint32_t* dst = reinterpret_cast<std::uint32_t*>(&jo::self<jo::LightSys>(this)->current);
        for (std::int32_t k = 0; k < static_cast<std::int32_t>(sizeof(jo::LightPreset) / 4); ++k) dst[k] = src[k];
        return off;
    }

    // 0x0042f620 Game::update_mode
    void update_mode() {
        c_update_mode.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->riding) != 0) joan::orig::as_player(this)->archer_icon_update();   // 0x0042a8a0
        if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->mounted) != 0) joan::orig::as_player(this)->draw_mounted();   // 0x0043c7b0
        if (jo::self<jo::Player>(this)->squad_mode != 0) joan::orig::as_player(this)->nop_42d2e0();  // 0x0042d2e0
        switch (jo::view<std::uint32_t>(jo::self<jo::Player>(this)->mode)) {
        case 0: joan::orig::as_player(this)->draw_mode0(); break;  // 0x00434f20
        case 1: joan::orig::as_player(this)->draw_mode1(); break;  // 0x004338f0
        case 2: joan::orig::as_player(this)->draw_mode2(); break;  // 0x004390f0
        case 3: joan::orig::as_player(this)->draw_mode3(); break;  // 0x00431550
        case 4: joan::orig::as_cursor(this)->draw(); break;  // 0x004450f0
        default: break;
        }
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Units::load_record", 0x0042e160, &H::load_record);
L82("Game::update_mode", 0x0042f620, &H::update_mode);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b109_19.cpp (game/b109_19)
namespace {
namespace game_b109_19 {

using joan::hook::global;

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

using u32 = std::uint32_t;

// Win32 API (stdcall) through its IAT slot.
template <class R = void, class... A>
R api_call(img::Addr slot, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}
using joan::orig::helpers::fbits;

struct V3 {  // 12 bytes copied as raw dwords
    u32 x, y, z;
};
struct Str {  // 0x005a1170 string object
    char b[0x194];
};
static_assert(sizeof(V3) == 12 && sizeof(Str) == 0x194, "sizes");

void* const kUnits = obj(d::g_units);
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_key, "Player::on_key_42e1a0");

struct H {
    // -------------------------------------------------------------- 0x0042e1a0
    static bool bound(i32 key, i32 action) {
        return joan::orig::arg::as<i32>(joan::orig::as_race_res(obj(d::g_keys_a48820))->test(joan::orig::arg::i32(key), joan::orig::arg::i32(action))) != 0;  // 0x004cb820
    }
    static bool save_blocked() {
        return static_cast<i8>(jo::self<jo::IngameMenu>(obj(d::g_ingame_menu))->open) != 0 ||  // 0x0042d160 (x86: byte +0x24; g_a59a98 is an IngameMenu, SaveMenu::busy_ drifts on x64)
               static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_inventory(obj(d::g_a56398))->is_open())) != 0 ||  // 0x00430340
               static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_save_menu(obj(d::g_save_menu))->is_open())) != 0 ||  // 0x00430370
               static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_opt_menu(obj(d::g_opt_menu))->is_open())) != 0 ||  // 0x004303a0
               static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_obj_menu(obj(d::g_objective_menu))->is_open())) != 0;    // 0x004303d0
    }
    static void quick_names(Str* name, Str* mission) {
        joan::orig::as_string(name)->construct();           // 0x005a1170
        joan::orig::as_string(name)->assign(str(d::s_Mission));           // 0x005a1440 "Mission"
        joan::orig::as_string(name)->append(str(d::s_QuickSave));               // 0x005a1550 "-QuickSave"
        joan::orig::as_string(name)->append(str(d::s_SAV_9cfb78));               // 0x005a1550 ".SAV"
        joan::orig::as_string(mission)->construct();        // 0x005a1170
        const char* m = joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(d::g_strings))->charmove_4());  // 0x004de600
        joan::orig::as_string(mission)->assign(joan::orig::arg::str(m));  // 0x005a1440
    }
    static const char* c_str(Str* s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(s)->c_str()); }  // 0x00417d90
    // Hero selection by binding (0x0042ea93 .. 0x0042ee53): true when handled.
    bool select_hero(i32 action, i32 slot) {
        if (!bound(jo::self<jo::Player>(this)->mode, action)) return false;
        u8* const u = unit_get(jo::self<jo::Player>(this)->unit_id_);
        if (jo::view<i8>(jo::self<jo::Player>(this)->u8_79) == 0 && global<i32>(d::g_mission) <= 1) return false;
        bool go;
        if (joan::orig::arg::as<i32>(joan::orig::as_world(u)->flag_1fd()) == 0 && jo::as_unit(u)->anim_kind != 0x4f && jo::as_unit(u)->anim_kind != 0x50 &&  // 0x0042d690
            jo::as_unit(u)->anim_kind != 0x51 && jo::as_unit(u)->anim_kind != 0x52) {
            go = true;
        } else {
            go = static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_unit(u)->dead())) != 0;  // 0x0042d0e0
        }
        if (!go) return false;
        joan::orig::as_player(this)->select_hero(joan::orig::arg::i32(slot));  // 0x00432300
        return true;
    }
    u32 on_key() {
        c_key.hit();
        if (global<i8>(d::g_a596ab) != 0) return 0;
        if (static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_state))->flag_744())) != 0 ||  // 0x004301c0
            static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_shop(obj(d::g_game_a5d380))->active())) != 0)    // 0x0042ced0
            return 0;
        if (jo::view<i8>(jo::self<jo::Player>(this)->blocked) != 0) return 0;
        if (jo::view<i8>(jo::self<jo::Player>(this)->u8_7c) != 0) return 0;
        if (global<i8>(d::g_a48ce0) != 0) return 0;
        if (static_cast<i8>(jo::self<jo::IngameMenu>(obj(d::g_ingame_menu))->open) != 0 ||  // 0x0042d160 (x86: byte +0x24; g_a59a98 is an IngameMenu, SaveMenu::busy_ drifts on x64)
            static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_obj_menu(obj(d::g_objective_menu))->is_open())) != 0 ||  // 0x004303d0
            static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_hud(obj(d::g_hud))->is_open())) != 0)    // 0x004303f0
            return 0;
        if (global<u32>(d::g_a48cef) == 2) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_mode(obj(d::g_screen_mgr))->check_6f5a70()) != 0) return 1;  // 0x006f5a70
        i32 key = jo::self<jo::Player>(this)->mode;
        if (key == 4) key = jo::self<jo::Player>(this)->prev_mode;

        if (bound(key, 0x35)) {  // quicksave
            if (save_blocked()) return 0;
            Str name;
            Str mission;
            quick_names(&name, &mission);
            u8* slot = joan::orig::arg::as<u8*>(joan::orig::as_paths(obj(d::g_saves))->find(joan::orig::arg::str(c_str(&name))));  // 0x00477640
            if (slot == nullptr) {
                const i16 index = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_save_dir(obj(d::g_saves))->add(joan::orig::arg::str(i32{0}))));  // 0x00477440
                slot = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_saves))->get_430410(joan::orig::arg::i16(index)));                                 // 0x00430410
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(slot + 3))), str(d::s_SAVE));  // strcpy "SAVE\\"  // x64: needs GameFile+0x3 char[0x105]
                std::strcat(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(slot + 3))), joan::orig::arg::str(c_str(&name)));  // strcat  // x64: needs GameFile+0x3 char[0x105]
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_game_file(slot)->name)), joan::orig::arg::str(c_str(&mission)));  // strcpy
            } else {
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_game_file(slot)->name)), joan::orig::arg::str(c_str(&mission)));  // strcpy
            }
            jo::as_game_file(slot)->u32_19a = global<u32>(d::g_mission);
            jo::as_game_file(slot)->u8_19e = 0;
            joan::orig::as_game_file(slot)->save();                                   // 0x00478180
            joan::orig::as_game_file(slot)->save_preview(joan::orig::arg::mptr(global<void*>(d::g_camera)));          // 0x004731e0
            joan::orig::as_list(obj(d::g_saves))->renumber(fn::code_value(fn::cmp_477bd0));       // 0x00477b10 (callback 0x00477bd0)
            joan::orig::as_screen(obj(d::g_quick_save_screen))->set_mode_6e4360(joan::orig::arg::u32(i32{0}));                // 0x006e4360
            return 1;
        }
        if (bound(key, 0x36)) {  // quickload
            if (save_blocked()) return 0;
            Str name;
            Str mission;
            quick_names(&name, &mission);
            u8* const slot = joan::orig::arg::as<u8*>(joan::orig::as_paths(obj(d::g_saves))->find(joan::orig::arg::str(c_str(&name))));  // 0x00477640
            if (slot != nullptr) {
                global<u8*>(d::g_a5d861) = slot;
                joan::orig::as_screen(obj(d::g_quick_save_screen))->set_mode_6e4360(joan::orig::arg::u32(i32{1}));  // 0x006e4360
            }
            return 1;
        }
        if (bound(key, 0x2e)) {  // fade toggle
            u32& mode = global<u32>(d::g_a4d767);
            if (mode == 0) {
                global<u8>(d::g_a4d765) = 1;
                global<u32>(d::g_a4d76b) = global<u32>(d::g_now);
                mode = 1;
            } else if (mode == 3) {
                global<u32>(d::g_a4d76b) = global<u32>(d::g_now);
                mode = 2;
            } else if (mode == 1) {
                mode = 2;
                const Fpu fpu{true};
                const double now = static_cast<double>(global<u32>(d::g_now));
                const double d = static_cast<double>(global<u32>(d::g_now) - global<u32>(d::g_a4d76b));
                const double k2000 = global<float>(d::flt_2000);
                const double k255 = global<float>(d::flt_255);
                const double k3000 = global<float>(d::flt_3000);
                double x = fpu.div(d, k2000);
                x = fpu.mul(x, k255);
                x = fpu.sub(k255, x);
                x = fpu.div(x, k255);
                x = fpu.mul(x, k3000);
                global<i32>(d::g_a4d76b) = ftol(fpu.sub(now, x));
            } else if (mode == 2) {
                mode = 1;
                const Fpu fpu{true};
                const double now = static_cast<double>(global<u32>(d::g_now));
                const double d = static_cast<double>(global<u32>(d::g_now) - global<u32>(d::g_a4d76b));
                const double k2000 = global<float>(d::flt_2000);
                const double k255 = global<float>(d::flt_255);
                const double k3000 = global<float>(d::flt_3000);
                double x = fpu.div(d, k3000);
                x = fpu.mul(x, k255);
                x = fpu.sub(k255, x);
                x = fpu.mul(x, k2000);
                x = fpu.div(x, k255);
                global<i32>(d::g_a4d76b) = ftol(fpu.sub(now, x));
                global<u8>(d::g_a4d765) = 1;
            }
            global<u8>(d::g_a4d766) = 1;
            return 1;
        }
        if ((global<u16>(d::g_key_modifiers) & 3) != 0) {  // debug: hero cheats
            static const i32 kActions[8] = {0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1f, 0x1e, 0x20};
            static const img::Addr kNames[8] = {d::s_JOAN_OF_ARC, d::s_JEAN_DE_METZ, d::s_LAHIRE_9cfb3c, d::s_ALENCON_9cfb30,
                                                     d::s_ORLEAN, d::s_charlie7, d::s_arthur_de_richemont, d::s_FLAVY};
            for (i32 i = 0; i < 8; ++i) {
                if (!bound(key, kActions[i])) continue;
                const u32 id = joan::orig::arg::as<u32>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(str(kNames[i]))));  // 0x005270b0
                const i32 unit = joan::orig::arg::as<i32>(joan::orig::as_units(kUnits)->find_by_3b(joan::orig::arg::i32(id)));                     // 0x0051fd50
                if (unit != 0) joan::orig::as_unit(unit_get(unit))->select_speech(joan::orig::arg::u32(i32{1}));              // 0x005121c0
                return 1;
            }
        }
        if (jo::self<jo::Player>(this)->mode != 4 && jo::view<i8>(jo::self<jo::Player>(this)->riding) == 0) {
            if (select_hero(0x18, 0)) return 1;
            if (select_hero(0x19, 1)) return 1;
            if (select_hero(0x1a, 2)) return 1;
            if (select_hero(0x1b, 3)) return 1;
            if (select_hero(0x1c, 4)) return 1;
            // [Game] AllHeroes: Command the Bastard of Orleans / Sir Arthur de Richemont / King Charles VII (keys 5..7
            // by default) take him over in action mode too; the original reads them only in RTS mode (army commands).
            if (joan::hook::settings().all_heroes != 0) {
                if (select_hero(0x1d, 5)) return 1;
                if (select_hero(0x1e, 6)) return 1;
                if (select_hero(0x1f, 7)) return 1;
            }
        }
        if (bound(key, 0x17) && (global<i32>(d::g_mission) > 4 || jo::view<i8>(jo::self<jo::Player>(this)->u8_79) != 0) && global<u32>(d::g_a48cef) != 1 &&
            global<u32>(d::g_a48cef) != 2) {
            if (jo::self<jo::Player>(this)->mode == 4) {
                joan::orig::as_unit(this)->set_state(joan::orig::arg::i32(jo::self<jo::Player>(this)->prev_mode), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x0042e040
            } else {
                joan::orig::as_unit(this)->set_state(i32{4}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x0042e040
            }
            return 1;
        }
        if (jo::self<jo::Player>(this)->mode != 4 && unit_missing(jo::self<jo::Player>(this)->unit_id_) == 0) {
            u8* const u = unit_get(jo::self<jo::Player>(this)->unit_id_);
            const i16 kind = jo::as_unit(u)->anim_kind;
            if (kind == 0x2a || jo::as_unit(u)->anim_kind == 0x3c || jo::as_unit(u)->anim_kind == 0x40 || jo::as_unit(u)->anim_kind == 0x4e ||
                jo::as_unit(u)->anim_kind == 0x41 || jo::as_unit(u)->anim_kind == 0x42) {
                jo::self<jo::Player>(this)->u32_a8 = 0;
                return 0;
            }
        }
        if (global<i8>(d::g_cheat_keys_enabled) != 0) {
            const float deg = global<float>(d::flt_0_017453292);
            if (global<u32>(d::g_key) == 0x37) {
                u32 cam[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x006140a0
                Str text;
                joan::orig::as_string(&text)->construct();  // 0x005a1170
                joan::orig::libc::sprintf(joan::orig::arg::str(c_str(&text)), str(d::s_camera_x_2f_y_2f_z_2f_h_dir), static_cast<double>(fbits(cam[1])), static_cast<double>(fbits(cam[2])), static_cast<double>(fbits(cam[3])), static_cast<double>(fbits(cam[4]) / deg), static_cast<double>(fbits(cam[5]) / deg), static_cast<double>(fbits(cam[6]) / deg), static_cast<double>(fbits(cam[7])));  // sprintf
                api_call(d::iat_KERNEL32_OutputDebugStringA, c_str(&text));  // OutputDebugStringA
                u8* const u = unit_get(jo::self<jo::Player>(this)->unit_id_);
                V3 pos{0, 0, 0};
                joan::orig::as_unit(u)->position(&pos);  // 0x004fedb0
                joan::orig::libc::sprintf(joan::orig::arg::str(c_str(&text)), str(d::s_X_2f_Y_2f_Z_2f), static_cast<double>(fbits(pos.x)), static_cast<double>(fbits(pos.y)), static_cast<double>(fbits(pos.z)));  // sprintf
                api_call(d::iat_KERNEL32_OutputDebugStringA, c_str(&text));        // OutputDebugStringA
                joan::orig::as_weather_system(obj(d::g_weather_system))->advance_time(i32{5});  // 0x0068c600
            } else if (global<u32>(d::g_key) == 0x26) {
                u32 motion[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(i32{3}));  // 0x00563c80
                joan::orig::as_camera_motion(m)->end(joan::orig::arg::mptr(static_cast<void*>(motion)));                    // 0x00423fe0
                Str text;
                joan::orig::as_string(&text)->construct();  // 0x005a1170
                joan::orig::libc::sprintf(joan::orig::arg::str(c_str(&text)), str(d::s_x_2f_y_2f_z_2f_h_dir_2f_v_di), static_cast<double>(fbits(motion[1])), static_cast<double>(fbits(motion[2])), static_cast<double>(fbits(motion[3])), static_cast<double>(fbits(motion[4]) / deg), static_cast<double>(fbits(motion[5]) / deg), static_cast<double>(fbits(motion[6])), static_cast<double>(fbits(motion[7])));  // sprintf
                api_call(d::iat_KERNEL32_OutputDebugStringA, c_str(&text));  // OutputDebugStringA
            }
        }
        switch (jo::view<u32>(jo::self<jo::Player>(this)->mode)) {
            case 0: return joan::orig::arg::as<u32>(joan::orig::as_player(this)->handle_input_4343e0());  // 0x004343e0
            case 1: return joan::orig::arg::as<u32>(joan::orig::as_ctrl(this)->update_432d30());  // 0x00432d30
            case 2: return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->input_438b30());  // 0x00438b30
            case 3: return joan::orig::arg::as<u32>(joan::orig::as_player(this)->handle_input_mode3());  // 0x00430480
            case 4: return joan::orig::arg::as<u32>(joan::orig::as_player(this)->handle_input());  // 0x00440b30
            default: return 0;
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_19", name, addr, fn)
B("Player::on_key_42e1a0", 0x0042e1a0, &H::on_key);
#undef B

}  // namespace game_b109_19
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using joan::hook::global;

using joan::call::cdecl_call;

using u32 = std::uint32_t;

std::uintptr_t uptr(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }

JOAN_COUNTER(c_create_sprite, "Unit::create_sprite_42f210");

struct H {
    // ------------------------------------------------------------------ 0x0042f210
    std::uintptr_t create_sprite() {
        c_create_sprite.hit();
        u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::view<u16>(jo::self<jo::MapObject>(this)->type_id))));  // 0x00425ce0
        void* const reg = obj(d::g_model_res_mgr);
        if (jo::self<jo::MapObject>(this)->actor == nullptr) {
            joan::orig::as_item_def(def)->acquire_anim(joan::orig::arg::i32(jo::self<jo::MapObject>(this)->busy), 0);                  // 0x004a39f0
            const u32 kind = joan::orig::arg::as<u32>(joan::orig::as_item_def(def)->sprite_id(joan::orig::arg::i32(jo::self<jo::MapObject>(this)->busy)));     // 0x004a3f20
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));            // 0x10c on x86; operator new
            void* const spr = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
            jo::view<void*>(jo::self<jo::MapObject>(this)->actor) = spr;
            u8* r = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(reg)->entry(joan::orig::arg::u32(kind)));  // 0x0041a180
            jo::self<jo::MapObject>(this)->actor->def = jo::as_model_entry(r)->actor_def;
            if (jo::view<u32>(jo::as_item_def(def)->slot_anims[jo::view<i32>(jo::self<jo::MapObject>(this)->busy)]) != 0) {
                jo::AnimDef** const e = joan::orig::arg::as<jo::AnimDef**>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_item_def(def)->slot_anims[jo::view<i32>(jo::self<jo::MapObject>(this)->busy)]))));  // 0x004174f0
                jo::self<jo::MapObject>(this)->actor->anim = *e;
            }
            joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->init();  // 0x005aab70
            r = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(reg)->entry(joan::orig::arg::u32(kind)));  // 0x0041a180
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->set_skin(joan::orig::arg::u32(jo::view<u32>(jo::as_model_entry(r)->anim_db_id)));          // 0x005add40
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::MapObject>(this)->pos)));  // 0x005ac310
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->scale) = jo::self<jo::MapObject>(this)->scale;
            joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->set_yaw(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::MapObject>(this)->yaw_)));  // 0x005ac6a0
            joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->set_roll(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::MapObject>(this)->roll_)));  // 0x005ac980
            if (static_cast<i32>(jo::view<i16>(jo::as_item_def(def)->type_id)) == 6)
                jo::view<u32>(jo::self<jo::MapObject>(this)->actor->kind) = 0xa;
            else
                jo::view<u32>(jo::self<jo::MapObject>(this)->actor->kind) = 1;
            jo::self<jo::MapObject>(this)->actor->owner = this;
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->set_byte_fc(static_cast<std::int8_t>(joan::orig::arg::i32(1)));  // 0x005acfa0
            if (jo::view<u32>(jo::as_item_def(def)->slot_anims[jo::view<i32>(jo::self<jo::MapObject>(this)->busy)]) != 0) {
                joan::orig::as_actor(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->start_anim(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor->anim), str(d::s_default_9d3d60), joan::orig::arg::u32(global<u32>(d::g_now)), 1u, 0u, 0u);  // 0x005abe30
                joan::orig::as_unit(this)->play_named_sound(str(d::s_default_9d3d60));    // 0x0049b4d0
            }
            for (i32 k = 0;; ++k) {
                u8* const rr = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(reg)->entry(joan::orig::arg::u32(kind)));  // 0x0041a180
                if (!(k < jo::as_model_entry(rr)->morph_count)) break;
                u8* const r2 = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(reg)->entry(joan::orig::arg::u32(kind)));  // 0x0041a180
                jo::Morph* const v = jo::as_model_entry(r2)->morphs[k].morph;
                // x64: Actor::add_child (0x005ae0c0) takes the Morph* as u32 (its generated signature)
                joan::orig::as_actor(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->add_child(v, 0u);  // 0x005ae0c0
            }
        }
        joan::orig::as_unit(this)->setup_light();  // 0x0049b7a0
        joan::orig::as_unit(this)->setup_glow();  // 0x0049c710
        std::uintptr_t eax = 0;
        for (i32 i = 0; i < 2; ++i) {
            eax = static_cast<std::uintptr_t>(i);
            if (jo::view<u32>(jo::as_item_def(def)->emitter_scripts[i]) == 0) continue;
            const float v = jo::as_item_def(def)->emitter_scale[i] + jo::self<jo::MapObject>(this)->yaw_;
            const u8 flag = jo::as_item_def(def)->emitter_flags[i];
            u8* const ev = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(def)->emitter_scripts[i]))));  // 0x0041b440
            const u32 h = joan::orig::arg::as<u32>(joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(jo::self<jo::MapObject>(this)->actor->scene, jo::as_pemi_script(ev)->effect, static_cast<void*>(jo::as_item_def(def)->emitter_pos + i * 3), static_cast<void*>(jo::self<jo::MapObject>(this)->pos), static_cast<std::int8_t>(joan::orig::arg::i32(flag)), joan::orig::arg::bits(v), 0, 0));  // 0x0041baf0
            jo::view<u16>(jo::self<jo::MapObject>(this)->sound_slots[i]) = static_cast<u16>(h);
            eax = uptr(def) + static_cast<std::uintptr_t>(i);
            if (jo::view<i8>(jo::as_item_def(def)->emitter_flags[i]) != 0) {
                void* const fx = joan::orig::as_dynarray(obj(d::g_particle_effect_array))->ptr_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i]));  // 0x0041bd60
                eax = joan::orig::arg::as<u32>(joan::orig::as_effect(fx)->set_owner(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor), joan::orig::arg::mptr(0u)));                  // 0x0041b0d0
            }
        }
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Unit::create_sprite", 0x0042f210, &H::create_sprite);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

using u32 = std::uint32_t;

struct H {
    u32 one_42f5b0(void*) { return 1; }
    bool false_4300c0() { return false; }
    bool true_430110() { return true; }
    bool true_430130() { return true; }
    bool true_430150() { return true; }
    bool true_430170() { return true; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("Obj::one_42f5b0", 0x0042f5b0, &H::one_42f5b0);
B("Obj::false_4300c0", 0x004300c0, &H::false_4300c0);
B("Obj::true_430110", 0x00430110, &H::true_430110);
B("Obj::true_430130", 0x00430130, &H::true_430130);
B("Obj::true_430150", 0x00430150, &H::true_430150);
B("Obj::true_430170", 0x00430170, &H::true_430170);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

struct H {
    // ------------------------------------------------------------------ 0x0042f5d0
    i32 kind_3_or_4() {
        const i32 k = jo::self<jo::Relation>(this)->attitude;
        return (k >= 3 && k <= 4) ? 1 : 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::kind_3_or_4", 0x0042f5d0, &H::kind_3_or_4);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using joan::hook::global;

using u32 = std::uint32_t;

JOAN_COUNTER(c_tick_42f700, "Mission::tick_42f700");

constexpr img::Addr kUnits = d::g_units;  // unit table (0x00428b70 check, 0x0041b3c0 get)

struct H {
    // -------------------------------------------------------------- 0x0042f700
    void tick_42f700() {
        c_tick_42f700.hit();
        if (global<i8>(d::g_a596ab) != 0) return;
        if (static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) & 0xffu) != 0) {  // 0x004301c0
            if (jo::self<jo::Player>(this)->mode == 4) jo::self<jo::Player>(this)->dragging = 0;
            return;
        }
        if (jo::self<jo::Player>(this)->mode == 4 && joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) == 0) {  // 0x00428b70
            void* const u = joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
            if (static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_unit(u)->dead()) & 0xffu) != 0)                  // 0x0042d0e0
                joan::orig::as_unit(this)->set_state(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->prev_mode)), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));                       // 0x0042e040
        }
        if (static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_player(this)->byte_81()) & 0xffu) != 0) {  // 0x004300e0
            u8 done = 0;
            switch (jo::view<u32>(jo::self<jo::Player>(this)->prev_mode)) {
                case 0: done = joan::orig::arg::as<u8>(joan::orig::as_player(this)->true_430110()); break;  // 0x00430110
                case 1: done = joan::orig::arg::as<u8>(joan::orig::as_player(this)->mode1_switch_done()); break;  // 0x00430130
                case 2: done = joan::orig::arg::as<u8>(joan::orig::as_player(this)->mode2_switch_done()); break;  // 0x00430150
                case 3: done = joan::orig::arg::as<u8>(joan::orig::as_player(this)->mode3_switch_done()); break;  // 0x00430170
                case 4: done = joan::orig::arg::as<u8>(joan::orig::as_world(this)->finish_level()); break;  // 0x00447860
                default: break;
            }
            if (static_cast<i8>(done) != 0) {
                jo::self<jo::Player>(this)->blocked = 0;
                switch (jo::view<u32>(jo::self<jo::Player>(this)->mode)) {
                    case 0: joan::orig::as_cursor(obj(d::g_mouse))->set_pos(); break;  // 0x00592bd0
                    case 1: joan::orig::as_cursor(obj(d::g_mouse))->set_pos(); break;
                    case 2: joan::orig::as_player(this)->enter_mode2(); break;  // 0x00438ad0
                    case 3:
                        joan::orig::as_player(this)->enter_mode3();  // 0x00430440
                        joan::orig::as_cursor(obj(d::g_mouse))->set_pos();
                        break;
                    case 4: joan::orig::as_player(this)->enter_rts(); break;  // 0x0043fc90
                    default: break;
                }
            }
        }
        if (jo::view<i8>(jo::self<jo::Player>(this)->riding) != 0) {
            joan::orig::as_order(this)->update_42ad50();  // 0x0042ad50
            return;
        }
        const u32 mode = jo::view<i8>(jo::self<jo::Player>(this)->blocked) != 0 ? jo::view<u32>(jo::self<jo::Player>(this)->prev_mode) : jo::view<u32>(jo::self<jo::Player>(this)->mode);
        switch (mode) {
            case 0: joan::orig::as_player(this)->tick_mode0(); break;  // 0x00434f40
            case 1: joan::orig::as_player(this)->tick_mode1(); break;  // 0x004338c0
            case 2: joan::orig::as_player(this)->tick_mode2(); break;  // 0x00439090
            case 3: joan::orig::as_player(this)->tick_mode3(); break;  // 0x00431520
            case 4: joan::orig::as_game(this)->rts_mode_442ef0(); break;  // 0x00442ef0
            default: break;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Mission::tick_42f700", 0x0042f700, &H::tick_42f700);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

using joan::hook::global;

struct H {
    // ------------------------------------------------------------------ 0x0042f930
    void set_speed(std::uint32_t speed_bits) {
        jo::view<std::uint32_t>(jo::self<jo::Wnd>(this)->speed) = speed_bits;
        const float v = jo::self<jo::Wnd>(this)->speed, zero = global<float>(d::k_9cf5c4);
        if (!(v < zero) && !(v > zero)) {  // equal or unordered (test ah,0x40)
            joan::orig::as_party(obj(d::g_sfx_player))->set_scale(0);  // 0x0045f6b0
            joan::orig::as_group(obj(d::g_names))->pause_sounds(static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x0049c080
            joan::orig::as_streams(obj(d::g_dialogue))->pause_all(static_cast<char>(joan::orig::arg::i32(0)));  // 0x0063c6d0
        } else {
            joan::orig::as_party(obj(d::g_sfx_player))->set_scale(static_cast<std::int32_t>(global<std::int16_t>(d::g_sfx_volume)));
            joan::orig::as_group(obj(d::g_names))->pause_sounds(static_cast<std::int8_t>(joan::orig::arg::i32(1)));
            joan::orig::as_streams(obj(d::g_dialogue))->pause_all(static_cast<char>(joan::orig::arg::i32(1)));
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Game::set_speed", 0x0042f930, &H::set_speed);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using joan::hook::global;

void* const kUnitTable = obj(d::g_units);

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

// ---------------------------------------------------------------------- static initialisers
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a39d9c() { div_global(d::g_a39d9c, d::g_a39d14, d::flt_3); }  // 0x0042fec0
static void JOAN_CDECL init_a336dc() { div_global(d::g_a336dc, d::g_pi_a336d4, d::flt_3); }  // 0x004302c0

struct H {
    // ------------------------------------------------------------------ 0x0042fdd0
    float unit_value_42fdd0() {
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return global<float>(d::k_9cf5c4);
        return joan::orig::arg::as<float>(joan::orig::as_unit(unit_get(jo::self<jo::Player>(this)->unit_id_))->build_progress());  // 0x00504760 (float x float st0)
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::unit_value_42fdd0", 0x0042fdd0, &H::unit_value_42fdd0);
B99("init_a39d9c", 0x0042fec0, &init_a39d9c);
B99("init_a336dc", 0x004302c0, &init_a336dc);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::hook::global;

using joan::hook::global;

JOAN_COUNTER(c_heading, "heading_to_degrees");

struct H {
    // 0x0042fe30
    float heading_to_degrees(const void* src, float add) {
        c_heading.hit();
        float x = jo::as_cam_params(src)->orbit[0] / global<float>(d::flt_0_017453292);
        while (!(x >= global<float>(d::k_9cf5c4))) x = x + global<float>(d::flt_360);  // 0.0 / 360.0
        float s = x + add;
        s = s - global<float>(d::flt_90);  // 90.0
        const std::int32_t r = joan::hook::x87::ftol(s) % 0x168;
        float y = static_cast<float>(r);
        while (!(y >= global<float>(d::k_9cf5c4))) y = y + global<float>(d::flt_360);
        return y;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("heading_to_degrees", 0x0042fe30, &H::heading_to_degrees);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
namespace {
namespace game_b86_05 {

using joan::hook::global;

using joan::call::cdecl_call;

using joan::orig::helpers::stricmp_;

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* type_of(std::int16_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x0042d0a0

JOAN_COUNTER(c_is_hero, "Unit::is_hero");

// ---- 0x0042ff90 is_hero(id) (cdecl) ---------------------------------------------------------
std::int32_t JOAN_CDECL is_hero(std::int32_t id) {
    c_is_hero.hit();
    if (unit_missing(id) != 0) return 0;
    std::uint8_t* const unit = unit_get(id);
    std::uint8_t* const type = type_of(jo::as_unit(unit)->type_id);
    for (std::int32_t i = 0; i < 4; ++i) {
        if (stricmp_(&img::global<const char>(d::g_9f861c + i * 0x104), jo::as_unit_type(type)->name) == 0) return 1;
    }
    return 0;
}

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("Unit::is_hero", 0x0042ff90, &is_hero);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::orig::helpers::stricmp_;

// ---------------------------------------------------------------------- 0x00430050
static i32 JOAN_CDECL is_hero_name(const char* s) {
    for (i32 i = 0; i < 4; ++i) {
        if (stricmp_(str(d::g_9f861c + static_cast<u32>(i * 0x104)), s) == 0) return 1;  // "JOAN_OF_ARC", ...
    }
    return 0;
}

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("is_hero_name", 0x00430050, &is_hero_name);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    std::uint32_t unit_mounted() { return jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->mount_id); }                     // 0x00430190
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Unit::mounted", 0x00430190, &H::unit_mounted);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    std::uint8_t get_744() { return jo::self<jo::Game>(this)->cutscene_flag; }                                // 0x004301c0
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("get_byte_744", 0x004301c0, &H::get_744);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b105_01_2.cpp (game/b105_01)
namespace {
namespace game_b105_01_2 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- CRT initialiser wrappers: call the body directly, return its eax ----
template <fn::Code Body>
u32 JOAN_CDECL init_wrapper() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("init_0x004301f0", 0x004301f0, &init_wrapper<fn::init_a336d4>);
B("init_0x00430250", 0x00430250, &init_wrapper<fn::init_430270>);
B("init_0x004302a0", 0x004302a0, &init_wrapper<fn::init_a336dc>);
B("init_0x004302f0", 0x004302f0, &init_wrapper<fn::init_a336e0>);
#undef B

}  // namespace game_b105_01_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Player::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_01::H::reset_42d810>(this)); }  // 0x42d810
auto joan::orig::List::empty_at_42d8d0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves81::H::empty_at>(this, a0)); }  // 0x42d8d0
auto joan::orig::Player::release() -> void { joan::bind::member<&::game_b94_01::H::free_18>(this); }  // 0x42d900
auto joan::orig::Unit::finish_state_42d980() -> void { joan::bind::member<&::game_b108_13::H::finish_state>(this); }  // 0x42d980
auto joan::orig::Group::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::group_release>(this)); }  // 0x42dd70
auto joan::orig::Screen::reset() -> void { joan::bind::member<&::game_b93_01::H::screen_reset>(this); }  // 0x42de70
auto joan::orig::init_pi_a39d14() -> void { joan::bind::free<&::game_b97_01::init_pi_a39d14>(); }  // 0x42df20
auto joan::orig::World::leave_mode(std::int8_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_03::H::leave_mode>(this, a0)); }  // 0x42df50
auto joan::orig::init_42e010() -> void { joan::bind::free<&::game_b100_01::init_42e010>(); }  // 0x42e010
auto joan::orig::Unit::set_state(std::int32_t a0, std::int8_t a1) -> void { joan::bind::member<&::game_b90_03::H::set_state>(this, a0, a1); }  // 0x42e040
auto joan::orig::Units::load_record(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b::H::load_record>(this, a0)); }  // 0x42e160
auto joan::orig::Player::on_key_42e1a0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_19::H::on_key>(this)); }  // 0x42e1a0
auto joan::orig::Unit::create_sprite() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_03::H::create_sprite>(this)); }  // 0x42f210
auto joan::orig::Ai::cond_always_1_42f5b0(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_01::H::one_42f5b0>(this, a0)); }  // 0x42f5b0
auto joan::orig::Relation::is_kind_3_or_4() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_01::H::kind_3_or_4>(this)); }  // 0x42f5d0
auto joan::orig::Game::update_mode() -> void { joan::bind::member<&::game_leaves82b::H::update_mode>(this); }  // 0x42f620
auto joan::orig::Player::tick() -> void { joan::bind::member<&::game_b108_20::H::tick_42f700>(this); }  // 0x42f700
auto joan::orig::Game::set_speed(std::uint32_t a0) -> void { joan::bind::member<&::game_b83_07::H::set_speed>(this, a0); }  // 0x42f930
auto joan::orig::Player::set_unit(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::set_unit>(this, a0)); }  // 0x42f9c0
auto joan::orig::Member::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::member_release>(this)); }  // 0x42fcb0
auto joan::orig::Unit::link_value_of() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b90_03::H::link_value_of>(this)); }  // 0x42fd60
auto joan::orig::Player::unit_progress() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b99_01::H::unit_value_42fdd0>(this)); }  // 0x42fdd0
auto joan::orig::Player::heading_to_degrees(void* a0, float a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b91_02::H::heading_to_degrees>(this, a0, a1)); }  // 0x42fe30
auto joan::orig::init_a39d9c() -> void { joan::bind::free<&::game_b99_01::init_a39d9c>(); }  // 0x42fec0
auto joan::orig::Unit::state_sound() -> void { joan::bind::member<&::game_b90_03::H::state_sound>(this); }  // 0x42fef0
auto joan::orig::unit::is_hero(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b86_05::is_hero>(a0)); }  // 0x42ff90
auto joan::orig::is_hero_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_01::is_hero_name>(a0)); }  // 0x430050
auto joan::orig::Player::byte_81() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b97_01::H::byte_81>(this)); }  // 0x4300e0
auto joan::orig::Player::true_430110() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b105_01::H::true_430110>(this)); }  // 0x430110
auto joan::orig::Player::mode1_switch_done() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b105_01::H::true_430130>(this)); }  // 0x430130
auto joan::orig::Player::mode2_switch_done() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b105_01::H::true_430150>(this)); }  // 0x430150
auto joan::orig::Player::mode3_switch_done() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b105_01::H::true_430170>(this)); }  // 0x430170
auto joan::orig::Unit::mounted() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_world::H::unit_mounted>(this)); }  // 0x430190
auto joan::orig::Game::flag_744() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b82_18::H::get_744>(this)); }  // 0x4301c0
