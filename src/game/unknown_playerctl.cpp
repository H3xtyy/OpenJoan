// game/unknown_playerctl.cpp: functions whose original source file is unknown (formerly game/_unplaced/playerctl.cpp;
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
#include "../interfac/mouse_options.hpp"
#include "../registry.hpp"
#include "../runtime/hitch.hpp"

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

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using joan::hook::x87::ftol;

using i32 = std::int32_t;

using u8 = std::uint8_t;
using i8 = std::int8_t;
using i16 = std::int16_t;

using u32 = std::uint32_t;

struct Obj {};

using joan::orig::helpers::div_global;

using joan::hook::x87::Fpu;

struct Vec3 {
    float x, y, z;
};

}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

struct H {
    // 0x00430340 / 0x00430370 / 0x004303a0
    std::uint8_t get_6ae() { return jo::self<jo::UnitAttScreen>(this)->is_open; }
    std::uint8_t get_4b05() { return jo::self<jo::SaveMenu>(this)->is_open_; }
    std::uint8_t get_122() { return jo::view<std::uint8_t>(jo::self<jo::OptMenu>(this)->slide_state); }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::get_6ae", 0x00430340, &H::get_6ae);
M("Obj::get_4b05", 0x00430370, &H::get_4b05);
M("Obj::get_122", 0x004303a0, &H::get_122);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

struct H {
    std::uintptr_t self_bits() const { return reinterpret_cast<std::uintptr_t>(this); }

    // 0x004303d0 / 0x004303f0
    std::uintptr_t byte_get() { return (self_bits() & ~std::uintptr_t{0xffu}) | jo::self<jo::Byte>(this)->value; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Byte::get_4303d0", 0x004303d0, &H::byte_get);
B("Byte::get_4303f0", 0x004303f0, &H::byte_get);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

struct H {
    void* get_unit(std::int16_t i) {                                                                    // 0x00430410
        return joan::orig::as_dynarray(this)->ptr_at(static_cast<std::int32_t>(i));  // 0x0041b3f0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Units::get", 0x00430410, &H::get_unit);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

struct H {
    void nop_430440() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("Obj::nop_430440", 0x00430440, &H::nop_430440);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

struct H {
    void nop0() {}                                                               // 0x004343c0 / 0x00432d10 / 0x00430460
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj430460::nop", 0x00430460, &H::nop0);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b109_11.cpp (game/b109_11)
namespace {
namespace game_b109_11 {

using joan::call::cdecl_call;

using u16 = std::uint16_t;

void* const kKeys = obj(d::g_keys_a48820);

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_player, "PlayerCtl::update_430480");

struct H {
    // ------------------------------------------------------------ 0x00430480
    static i32 key(i32 code) { return joan::orig::arg::as<i32>(joan::orig::as_race_res(kKeys)->test(i32{3}, joan::orig::arg::i32(code))); }   // 0x004cb820
    static i32 held(i32 code) { return joan::orig::arg::as<i32>(joan::orig::as_bindings(kKeys)->state(i32{3}, joan::orig::arg::u32(code))); }  // 0x004cb6d0
    u32 unit_id() const { return jo::view<u32>(jo::self<jo::Player>(this)->unit_id_); }
    i32 unit_busy() const { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(unit_id())); }  // 0x00428b70
    u8* unit() const { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(unit_id()))); }  // 0x0041b3c0

    // Quick order (5 / 6 / 7 -> codes 0x15 / 0x16 / 0x17); returns 1.
    i16 quick_order(i32 code) {
        u8* const u = unit();
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->can_act()) != 0 && jo::as_unit(u)->action != 1 && jo::as_unit(u)->anim_kind != 5 &&  // 0x00500ba0
            jo::as_unit(u)->anim_kind != 6 && jo::as_unit(u)->anim_set != nullptr &&
            joan::orig::arg::as<i32>(joan::orig::as_list(jo::as_unit(u)->anim_set)->index_of_416dc0(joan::orig::arg::i32(code))) != 0) {  // 0x00416dc0
            joan::orig::as_unit(unit())->set_action(joan::orig::arg::i32(code), i32{0}, i32{1});  // 0x00501190
        }
        return 1;
    }

    i16 player_update() {
        c_player.hit();
        i32 v8 = 0, vc = 0, v10 = 0, v14 = 0, v18 = 0, v1c = 0, v20 = 0, v24 = 0;
        i32 v34 = 0, v38 = 0, v3c = 0, v40 = 0;
        const i32 k28 = key(0xd);
        const i32 k2c = key(0xe);
        i8 b44 = 0;
        if (global<i32>(d::g_mouse_wheel) > 0) b44 = 1;
        if (global<i32>(d::g_mouse_wheel) < 0) b44 = 2;
        if (key(0x30) != 0) b44 = 1;
        if (key(0x31) != 0) b44 = 2;
        if (key(0x12) != 0) v8 = 1;
        if (key(0x15) != 0) v24 = 1;
        if (held(4) != 0) v34 = 1;
        if (held(6) != 0) v3c = 1;
        if (held(5) != 0) v38 = 1;
        if (held(7) != 0) v40 = 1;
        if (key(0xf) != 0) v20 = 1;
        if (held(0xc) != 0) v14 = 1;
        if (k28 != 0) v10 = 1;
        if (k2c != 0) v18 = 1;
        float f48 = 0.0f;  // dead unless v1c (see header)
        if (held(0x10) != 0) {
            jo::self<jo::Player>(this)->jump_hold_ms += global<i32>(d::g_frame_step_ms);
            if (jo::self<jo::Player>(this)->jump_hold_ms >= global<i32>(d::g_jumpmaxd)) {
                v1c = 1;
                f48 = static_cast<float>(jo::self<jo::Player>(this)->jump_hold_ms);
                jo::self<jo::Player>(this)->jump_hold_ms = 0;
            }
        } else if (jo::self<jo::Player>(this)->jump_hold_ms > 0) {
            v1c = 1;
            f48 = static_cast<float>(jo::self<jo::Player>(this)->jump_hold_ms);
            jo::self<jo::Player>(this)->jump_hold_ms = 0;
        }
        i32 v4c = joan::orig::arg::as<i32>(joan::orig::as_player(this)->anim_code(joan::orig::arg::i32(v34), joan::orig::arg::i32(v38), joan::orig::arg::i32(v3c), joan::orig::arg::i32(v40)));  // 0x00431680
        i32 v50 = joan::orig::arg::as<i32>(joan::orig::as_player(this)->direction_angle(joan::orig::arg::i32(v34), joan::orig::arg::i32(v38), joan::orig::arg::i32(v3c), joan::orig::arg::i32(v40)));  // 0x00431570
        i32 v54 = (v34 == 0 && v38 == 0 && v3c == 0 && v40 == 0) ? 0 : 3;
        const float f58 = joan::orig::arg::as<float>(joan::orig::as_unit(this)->link_value_of());  // 0x0042fd60
        i32 v5c = 0;
        if (jo::self<jo::Player>(this)->last_dir_key != 0 && global<u32>(d::g_now) - jo::self<jo::Player>(this)->last_dir_key_ms < 0x12cu &&
            key(jo::self<jo::Player>(this)->last_dir_key) != 0)
            v5c = jo::self<jo::Player>(this)->last_dir_key;
        if (global<i32>(d::g_key) != 0) {
            if (key(4) != 0)
                jo::self<jo::Player>(this)->last_dir_key = 4;
            else if (key(5) != 0)
                jo::self<jo::Player>(this)->last_dir_key = 5;
            else if (key(6) != 0)
                jo::self<jo::Player>(this)->last_dir_key = 6;
            else if (key(7) != 0)
                jo::self<jo::Player>(this)->last_dir_key = 7;
            else
                jo::self<jo::Player>(this)->last_dir_key = 0;
            jo::self<jo::Player>(this)->last_dir_key_ms = global<u32>(d::g_now);
        }
        const i32 v60 = v5c == 4 ? 1 : 0;
        const bool b64 = v5c == 5;
        const bool b6c = v5c == 6;
        const bool b68 = v5c == 7;
        if (jo::view<i8>(jo::self<jo::Player>(this)->riding) != 0) {
            const float dir = joan::orig::arg::as<float>(joan::orig::as_scroll(this)->angle());  // 0x00432100
            v50 = ftol(static_cast<double>(dir));
            if (v50 != -1) {
                auto iabs = [](i32 v) { return joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(v))); };  // abs
                if (iabs(global<i32>(d::g_mouse_delta_x)) >= 3 || iabs(global<i32>(d::g_mouse_delta_y)) >= 3)
                    v54 = 3;
                else if (iabs(global<i32>(d::g_mouse_delta_x)) >= 2 || iabs(global<i32>(d::g_mouse_delta_y)) >= 2)
                    v54 = 2;
                else if (iabs(global<i32>(d::g_mouse_delta_x)) >= 1 || iabs(global<i32>(d::g_mouse_delta_y)) >= 1)
                    v54 = 1;
                else
                    v54 = 0;
            } else {
                v54 = 0;
            }
        }
        if (key(0x13) != 0) joan::orig::as_order(this)->consume_43b490(joan::orig::arg::u32(i32{0}));  // 0x0043b490
        if (key(0x14) != 0 && (global<u16>(d::g_key_modifiers) & 0x30) == 0)
            joan::orig::as_unit(this)->use_spiritual_item_43b990(i32{0});  // 0x0043b990

        if (jo::view<i8>(jo::self<jo::Player>(this)->riding) != 0) {
            joan::orig::as_player(this)->command(joan::orig::arg::u32(v8), joan::orig::arg::u32(v10), joan::orig::arg::bits(static_cast<float>(v50)), joan::orig::arg::u32(v54));  // 0x0042a810
            if (v3c != 0 || v40 != 0) {
                if (v3c != 0 && v40 != 0) return 1;
                joan::orig::as_player(this)->order_direction(joan::orig::arg::i32(v34), joan::orig::arg::i32(v38), joan::orig::arg::i32(v3c), joan::orig::arg::i32(v40));  // 0x00438110
            } else if (v38 != 0) {
                joan::orig::as_player(this)->order_direction(i32{0}, joan::orig::arg::i32(v38), i32{0}, i32{0});  // 0x00438110
            } else if (unit_busy() == 0) {
                joan::orig::as_player(this)->set_follow(i32{0});  // 0x00436190
                u8* const u = unit();
                if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_e_14()) != 0) joan::orig::as_unit(u)->set_formation_slot_5124b0(joan::orig::arg::i16(i32{1}));  // 0x005091d0 / 0x005124b0
            }
            if (v3c == 0 && v40 == 0 && v38 == 0) {
                const i32 run = held(0xa);
                joan::orig::as_player(this)->unit_order(joan::orig::arg::i32(v4c), joan::orig::arg::i32(run));  // 0x00437ed0
            }
            if (v34 != 0 || v38 != 0 || v3c != 0 || v40 != 0) {
                jo::self<jo::Player>(this)->manual_aim = 1;
                jo::view<u32>(jo::self<jo::Player>(this)->aim_lock_id) = 0;
                if (unit_busy() == 0) jo::as_unit(unit())->look_a = 0;
            }
            return 1;
        }
        if (jo::view<i8>(jo::self<jo::Player>(this)->mounted) != 0) {
            joan::orig::as_camera(this)->terrain_clamp_43f000(joan::orig::arg::u32(v50));                  // 0x0043f000
            joan::orig::as_camera(this)->step_height(static_cast<i32>(b44));  // 0x00438720
            vc = held(0xa);
            if (global<i8>(d::g_a5802c) == 0) {
                const i32 k11 = key(0x11);
                return joan::orig::arg::as<i16>(joan::orig::as_player(this)->dispatch_keys(joan::orig::arg::u32(k11), joan::orig::arg::u32(v10), joan::orig::arg::u32(v18), joan::orig::arg::u32(vc), joan::orig::arg::bits(static_cast<float>(v50)), joan::orig::arg::u32(v4c), joan::orig::arg::u32(v54)));  // 0x0043c5c0
            }
            if (v40 != 0)
                v50 = 0;
            else if (v3c != 0)
                v50 = 0xb4;
            else if (global<i32>(d::g_mouse_delta_x) > 0)
                v50 = 0;
            else if (global<i32>(d::g_mouse_delta_x) < 0)
                v50 = 0xb4;
            else if (v34 != 0)
                v50 = 0x10e;
            else if (v38 != 0)
                v50 = 0x5a;
            else
                v50 = -1;
            v4c = joan::orig::arg::as<i32>(joan::orig::as_player(this)->anim_code(joan::orig::arg::i32(v34), joan::orig::arg::i32(v38), joan::orig::arg::i32(v40), joan::orig::arg::i32(v3c)));  // 0x00431680
            const i32 k11 = key(0x11);
            return joan::orig::arg::as<i16>(joan::orig::as_player(this)->dispatch_keys(joan::orig::arg::u32(k11), joan::orig::arg::u32(v10), joan::orig::arg::u32(v18), joan::orig::arg::u32(vc), joan::orig::arg::bits(static_cast<float>(v50)), joan::orig::arg::u32(v4c), joan::orig::arg::u32(v54)));  // 0x0043c5c0
        }
        if (jo::self<jo::Player>(this)->squad_mode != 0) {
            const i32 k11 = key(0x11);
            return joan::orig::arg::as<i16>(joan::orig::as_player(this)->squad_act(joan::orig::arg::u32(k11), joan::orig::arg::u32(v10), joan::orig::arg::u32(vc), joan::orig::arg::bits(static_cast<float>(v50)), joan::orig::arg::u32(v4c), joan::orig::arg::u32(v54)));  // 0x0042d270
        }
        if (key(0x11) != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(this)->recharge_43bef0()) == 0 &&  // 0x0043bef0
            joan::orig::arg::as<i32>(joan::orig::as_order(this)->spawn_effect()) == 0 &&                     // 0x0043bd40
            joan::orig::arg::as<i32>(joan::orig::as_unit(this)->try_link_43be40()) == 0 &&                     // 0x0043be40
            joan::orig::arg::as<i32>(joan::orig::as_player(this)->link_nearest_enemy()) == 0 &&                     // 0x0043c210
            joan::orig::arg::as<i32>(joan::orig::as_order(this)->fire_43bc50()) == 0) {                     // 0x0043bc50
            joan::orig::as_unit(this)->start_6c0310_43bfe0();  // 0x0043bfe0
            joan::orig::as_order(this)->alencon_43c080();  // 0x0043c080
        }
        if (global<u8>(d::g_game_running) == 0) return 0;
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->start_42b3c0(joan::orig::arg::u32(v8))) != 0) return 1;  // 0x0042b3c0
        joan::orig::as_player(this)->set_follow(joan::orig::arg::i32(vc));                     // 0x00436190
        joan::orig::as_camera(this)->step_height(static_cast<i32>(b44));  // 0x00438720
        if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->stub_zero(joan::orig::arg::u32(v14))) != 0) return 1;  // 0x00436380
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->find_target_4363a0(joan::orig::arg::u32(v14))) != 0) return 1;  // 0x004363a0
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(this)->try_flee(joan::orig::arg::i32(v14))) != 0) return 1;  // 0x00436250
        const i32 v90 = (v20 != 0 || (k28 != 0 && k2c != 0)) ? 1 : 0;
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->try_437910(joan::orig::arg::u32(v90))) != 0) return 1;            // 0x00437910
        if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->order_update(joan::orig::arg::i32(v10), joan::orig::arg::i32(v18), joan::orig::arg::i32(v60))) != 0) return 1;  // 0x004367c0
        if (unit_busy() == 0) {
            u8* const u = unit();
            if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_e_14()) == 0) {  // 0x005091d0
                if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->try_mount(joan::orig::arg::i32(key(0x11)))) != 0) return 1;  // 0x0043c7d0
                if (joan::orig::arg::as<i16>(joan::orig::as_squad(this)->try_rejoin_42d300(joan::orig::arg::i32(key(0x11)))) != 0) return 1;  // 0x0042d300
                if (joan::orig::arg::as<i16>(joan::orig::as_unit(this)->launch_437b60(joan::orig::arg::i32(v1c), joan::orig::arg::i32(v54), joan::orig::arg::f32(static_cast<float>(v50)), joan::orig::arg::f32(f48))) != 0)  // 0x00437b60
                    return 1;
            }
        }
        if (b64) return quick_order(0x15);
        if (b68) return quick_order(0x16);
        if (b6c) return quick_order(0x17);
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->try_rally_438820(joan::orig::arg::i32(v24))) != 0) return 1;  // 0x00438820
        if (v3c != 0 || v40 != 0) {
            if (v3c != 0 && v40 != 0) return 1;
            joan::orig::as_player(this)->order_direction(joan::orig::arg::i32(v34), joan::orig::arg::i32(v38), joan::orig::arg::i32(v3c), joan::orig::arg::i32(v40));  // 0x00438110
        } else if (v38 != 0) {
            joan::orig::as_player(this)->order_direction(i32{0}, joan::orig::arg::i32(v38), i32{0}, i32{0});  // 0x00438110
        } else if (unit_busy() == 0) {
            joan::orig::as_player(this)->set_follow(i32{0});  // 0x00436190
            u8* const u = unit();
            if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_e_14()) != 0) joan::orig::as_unit(u)->play_idle(i32{0}, i32{1});  // 0x005091d0 / 0x00508b90
        }
        if (v3c == 0 && v40 == 0 && v38 == 0)
            joan::orig::as_ctrl(this)->turn_4352f0(joan::orig::arg::f32(static_cast<float>(v50)), joan::orig::arg::i32(v54), joan::orig::arg::f32(f58));  // 0x004352f0
        joan::orig::as_player(this)->mouse_look(joan::orig::arg::i32(v50));                                   // 0x00431790
        if (v3c == 0 && v40 == 0 && v38 == 0) {
            const i32 run = held(0xa);
            joan::orig::as_player(this)->unit_order(joan::orig::arg::i32(v4c), joan::orig::arg::i32(run));  // 0x00437ed0
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_11", name, addr, fn)
B("PlayerCtl::update_430480", 0x00430480, &H::player_update);
#undef B

}  // namespace game_b109_11
}  // namespace

// ---------------------------------------- from modules/game_units4.cpp (game/units4)
namespace {
namespace game_units4 {

// Virtual thiscall through vtable slot `offset` (bytes).
template <class R = void, class... A>
R virtual_call(void* object, std::int32_t offset, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(object)[joan::call::x86_vslot(offset)], sizeof(void*));
    return (static_cast<Obj*>(object)->*method)(args...);
}

constexpr img::Addr kEffects(d::g_particle_effect_array);  // effect registry (id -> effect)

std::int32_t effect_missing(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kEffects))->empty_at(joan::orig::arg::i32(id)); }  // 0x0041bd90
void* effect_get(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kEffects))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041bd60
void* effect_object(void* e) { return joan::orig::arg::as<void*>(joan::orig::as_effect_state(&jo::as_model_obj(e)->fx)->object()); }               // 0x0041b280 (the registry records are ModelObj: +0x2b fx)
void scene_detach(void* scene, void* node) { joan::orig::as_scene(scene)->detach(joan::orig::arg::mptr(node)); }                             // 0x005efdb0
void scene_attach(void* scene, void* node) { joan::orig::as_scene(scene)->attach(joan::orig::arg::mptr(node)); }                             // 0x005efc50

JOAN_COUNTER(c_view_update, "UnitView::update");

struct H {
    // 0x004311d0 UnitView::update()
    void view_update() {
        c_view_update.hit();
        void* cell = jo::view<void*>(jo::self<jo::MapObject>(this)->cell);
        void* scene = jo::view<void*>(jo::self<jo::MapObject>(this)->scene);
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->world) != nullptr)
            joan::orig::as_map(jo::view<void*>(jo::self<jo::MapObject>(this)->world))->find_cell(&cell, &scene, joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->pos));  // 0x0054a2c0
        if (scene != jo::view<void*>(jo::self<jo::MapObject>(this)->scene) && jo::view<void*>(jo::self<jo::MapObject>(this)->scene) != nullptr) {
            for (std::int32_t i = 0; i < 2; ++i) {
                if (effect_missing(static_cast<std::int32_t>(jo::self<jo::MapObject>(this)->sound_slots[i])) != 0) continue;
                void* const e = effect_get(static_cast<std::int32_t>(jo::self<jo::MapObject>(this)->sound_slots[i]));
                scene_detach(jo::view<void*>(jo::self<jo::MapObject>(this)->scene), effect_object(e));
            }
            if (jo::view<void*>(jo::self<jo::MapObject>(this)->actor_b) != nullptr) scene_detach(jo::view<void*>(jo::self<jo::MapObject>(this)->scene), jo::view<void*>(jo::self<jo::MapObject>(this)->actor_b));
            if (jo::view<void*>(jo::self<jo::MapObject>(this)->actor) != nullptr) scene_detach(jo::view<void*>(jo::self<jo::MapObject>(this)->scene), jo::view<void*>(jo::self<jo::MapObject>(this)->actor));
            jo::view<void*>(jo::self<jo::MapObject>(this)->scene) = nullptr;
        }
        if (cell != jo::view<void*>(jo::self<jo::MapObject>(this)->cell) && cell != nullptr) joan::orig::as_cell(cell)->attach_unit(joan::orig::arg::mptr(this));  // 0x0043b190
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->cell) != nullptr && jo::view<std::int8_t>(jo::self<jo::MapObject>(this)->cell->loaded) != 0 && jo::view<std::int8_t>(jo::self<jo::MapObject>(this)->active) == 0) {
            virtual_call(this, 0x1c);
            return;
        }
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->cell) != nullptr && jo::view<std::int8_t>(jo::self<jo::MapObject>(this)->cell->loaded) == 0 && jo::view<std::int8_t>(jo::self<jo::MapObject>(this)->active) != 0) {
            virtual_call(this, 0x20);
            return;
        }
        if (scene == jo::view<void*>(jo::self<jo::MapObject>(this)->scene)) return;
        jo::view<void*>(jo::self<jo::MapObject>(this)->scene) = scene;
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->actor) != nullptr) scene_attach(jo::view<void*>(jo::self<jo::MapObject>(this)->scene), jo::view<void*>(jo::self<jo::MapObject>(this)->actor));
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->actor_b) != nullptr) scene_attach(jo::view<void*>(jo::self<jo::MapObject>(this)->scene), jo::view<void*>(jo::self<jo::MapObject>(this)->actor_b));
        for (std::int32_t i = 0; i < 2; ++i) {
            if (effect_missing(static_cast<std::int32_t>(jo::self<jo::MapObject>(this)->sound_slots[i])) != 0) continue;
            void* const e = effect_get(static_cast<std::int32_t>(jo::self<jo::MapObject>(this)->sound_slots[i]));
            scene_attach(jo::view<void*>(jo::self<jo::MapObject>(this)->scene), effect_object(e));
        }
    }
};

#define U4(name, addr, fn) JOAN_REPLACE("game/units4", name, addr, fn)
U4("UnitView::update", 0x004311d0, &H::view_update);
#undef U4

}  // namespace game_units4
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

// Raw 12-byte vector passed by value (kept as integers so no float load can touch the bits).
struct Vec3Raw {  // float members: System V passes them in XMM registers, as the callee (game/*.cpp)
    float x, y, z;
};
static_assert(sizeof(Vec3Raw) == 12, "vector size");

JOAN_COUNTER(c_apply_kind, "Units::apply_kind");

struct H {
    // --------------------------------------------------------------------- 0x00431480
    i32 apply_kind(i32 kind, u32 arg, Vec3Raw a, Vec3Raw b) {
        c_apply_kind.hit();
        i32 i = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        for (; i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_f(joan::orig::arg::i32(i))) != 0) continue;  // 0x00485760
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->get_f(joan::orig::arg::i32(i)));       // 0x00485730
            if (jo::view<i32>(jo::as_marker(e)->u32_2a) != kind) continue;
            if (jo::view<i32>(jo::as_marker(e)->pos2[0]) != 0) continue;
            joan::orig::as_marker(e)->launch(joan::orig::arg::u32(arg), a, b);  // 0x005133e0
        }
        return i;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Units::apply_kind", 0x00431480, &H::apply_kind);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

struct H {
    // ---------------------------------------------------------------- 0x00431520 / 0x004338c0 / 0x00434f40
    u32 dtor_pair() {
        joan::orig::as_player(this)->update_camera();             // 0x004384e0
        return thiscall<u32>(fn::Unit_update_a2_434270, this);  // 0x00434270
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Obj::dtor_431520", 0x00431520, &H::dtor_pair);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    void empty() {}
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("empty_00431550", 0x00431550, &H::empty);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

JOAN_COUNTER(c_direction, "direction_angle");

struct H {
    float get_e1() { return jo::self<jo::Player>(this)->f32_e1; }               // 0x004322d0

    // ------------------------------------------------------------ 0x00431570 (ret 0x10)
    std::int32_t direction_angle(std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t d) {
        c_direction.hit();
        if (c != 0 && d != 0) {
            c = 0;
            d = 0;
        }
        if (c != 0 && a != 0) return 0x87;
        if (d != 0 && a != 0) return 0x2d;
        if (a != 0) return 0x5a;
        if (c != 0 && b != 0) return 0xe1;
        if (d != 0 && b != 0) return 0x13b;
        if (c != 0) return 0xb4;
        if (d != 0) return 0x168;
        if (b != 0) return 0x10e;
        return -1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("direction_angle", 0x00431570, &H::direction_angle);
B("Obj::get_e1", 0x004322d0, &H::get_e1);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

struct H {
    // ------------------------------------------------------------------ 0x00431650
    u32 call_477360_431650() {
        return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->play_effect(static_cast<char>(joan::orig::arg::i32(static_cast<i32>(static_cast<i8>(static_cast<u8>(jo::self<jo::Order0c_b>(this)->u32_c2)))))));  // 0x00477360
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Unit::call_477360_431650", 0x00431650, &H::call_477360_431650);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

void JOAN_CDECL init_a336f0() { global<float>(d::g_a336f0) = global<float>(d::g_a336e8) / global<float>(d::flt_3); }

struct H {
    // ------------------------------------------------------------------ 0x00431680
    i32 anim_code(i32 a, i32 b, i32 c, i32 d) {
        if (c != 0 && d != 0) {
            c = 0;
            d = 0;
        }
        if (c != 0 && a != 0) return 0xa;
        if (d != 0 && a != 0) return 0xb;
        if (a != 0) return 0x25;
        if (c != 0 && b != 0) return 0xc;
        if (d != 0 && b != 0) return 0xd;
        if (c != 0) return 0x1c;
        if (d != 0) return 0x22;
        if (b != 0) return 0x14;
        return 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("anim_code", 0x00431680, &H::anim_code);
B96("init_a336f0", 0x00432c90, &init_a336f0);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

static void JOAN_CDECL init_a3cb74() { div_global(d::g_a3cb74, d::g_pi_a3cb6c, d::flt_2); }  // 0x00431760 (/2)

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("init_a3cb74", 0x00431760, &init_a3cb74);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b115_04.cpp (game/b115_04)
namespace {
namespace game_b115_04 {

using joan::call::cdecl_call;

struct V3 {
    float x, y, z;
};

constexpr img::Addr kUnits(d::g_units);
constexpr img::Addr kCameraMan(d::g_camera_man);

float cf(img::Addr address) { return global<float>(address); }
constexpr Fpu kFpu{true};

JOAN_COUNTER(c_look, "PlayerControl::mouse_look");

struct H {
    // 0x00431790
    void mouse_look(i32 arg) {
        void* const self = this;
        if (joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(jo::as_player(self)->unit_id_)) != 0) return;  // 0x00428b70
        void* const unit = joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::as_player(self)->unit_id_));  // 0x0041b3c0
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->can_act()) == 0 &&  // 0x00500ba0
            static_cast<i32>(jo::as_unit(unit)->anim_kind) != 0x49 && static_cast<i32>(jo::as_unit(unit)->anim_kind) != 0x1c &&
            static_cast<i32>(jo::as_unit(unit)->anim_kind) != 0x1d)
            return;
        if (static_cast<i32>(jo::as_unit(unit)->action) == 1 || static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x51 ||
            static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x52 || static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x4f ||
            static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x50)
            return;
        c_look.hit();

        const float option = joan::orig::arg::as<float>(joan::orig::as_player(self)->look_sensitivity());  // 0x004322d0
        const float base = static_cast<float>(
            kFpu.add(kFpu.mul(kFpu.add(option, cf(d::flt_1)), global<double>(d::dbl_0_7)), global<double>(d::dbl_0_2)));
        float sens;  // [ebp-0x10]
        if (static_cast<i32>(jo::view<i8>(jo::as_player(self)->mounted)) != 0)
            sens = base * cf(d::flt_0_4);
        else
            sens = base * cf(d::flt_0_2);
        i32 turn = 0;     // [ebp-0x14]
        i32 heading = 0;  // [ebp-0x18]

        alignas(4) u8 cam[0x28];  // [ebp-0x40]: +4 target, +0x10 yaw, +0x14 pitch, +0x18 distance, +0x1c scale
        {
            const i32 index = joan::orig::arg::as<i32>(joan::orig::as_camera_follow(img::obj(kCameraMan))->target());       // 0x00563dc0
            void* const motion = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(img::obj(kCameraMan))->motion(joan::orig::arg::i32(index)));  // 0x00563c80
            joan::orig::as_camera_motion(motion)->end(joan::orig::arg::mptr(static_cast<void*>(cam)));       // 0x00423fe0
        }

        const float deg = cf(d::flt_0_017453292);
        if (static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x1c) {
            if (global<i32>(d::g_mouse_look_dx) > 0) {
                turn = ftol(kFpu.mul(static_cast<double>(global<i32>(d::g_mouse_look_dx)), sens));
                if (turn > 0x2d) turn = 0x2d;
                heading = ftol(kFpu.sub(joan::orig::arg::as<float>(joan::orig::as_unit(unit)->float_1e()), static_cast<double>(turn)));  // 0x0050ac00
                const i32 h = heading < 0 ? heading + 0x168 : heading;
                joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(static_cast<float>(h)));  // 0x0050abd0
                const i32 r = heading < 0 ? heading + 0x168 : heading;
                joan::orig::as_unit(unit)->set_target(joan::orig::arg::f32(static_cast<float>(kFpu.mul(static_cast<double>(r), deg))));  // 0x004ff870
            } else if (global<i32>(d::g_mouse_look_dx) < 0) {
                turn = ftol(kFpu.mul(static_cast<double>(global<i32>(d::g_mouse_look_dx)), sens));
                if (turn < -0x2d) turn = -0x2d;
                heading = ftol(kFpu.sub(joan::orig::arg::as<float>(joan::orig::as_unit(unit)->float_1e()), static_cast<double>(turn)));  // 0x0050ac00
                const i32 h = heading > 0x168 ? heading - 0x168 : heading;
                joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(static_cast<float>(h)));  // 0x0050abd0
                const i32 r = heading > 0 ? heading - 0x168 : heading;
                joan::orig::as_unit(unit)->set_target(joan::orig::arg::f32(static_cast<float>(kFpu.mul(static_cast<double>(r), deg))));  // 0x004ff870
            }
        } else {
            if (global<i32>(d::g_mouse_look_dx) > 0) {
                turn = ftol(kFpu.mul(static_cast<double>(global<i32>(d::g_mouse_look_dx)), sens));
                heading = ftol(kFpu.sub(joan::orig::arg::as<float>(joan::orig::as_unit(unit)->float_1e()), static_cast<double>(turn)));  // 0x0050ac00
                const i32 h = heading < 0 ? heading + 0x168 : heading;
                joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(static_cast<float>(h)));  // 0x0050abd0
                const i32 r = heading < 0 ? heading + 0x168 : heading;
                joan::orig::as_unit(unit)->set_target(joan::orig::arg::f32(static_cast<float>(kFpu.mul(static_cast<double>(r), deg))));  // 0x004ff870
            } else if (global<i32>(d::g_mouse_look_dx) < 0) {
                turn = ftol(kFpu.mul(static_cast<double>(global<i32>(d::g_mouse_look_dx)), sens));
                heading = ftol(kFpu.sub(joan::orig::arg::as<float>(joan::orig::as_unit(unit)->float_1e()), static_cast<double>(turn)));  // 0x0050ac00
                const i32 h = heading > 0x168 ? heading - 0x168 : heading;
                joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(static_cast<float>(h)));  // 0x0050abd0
                const i32 r = heading > 0 ? heading - 0x168 : heading;
                joan::orig::as_unit(unit)->set_target(joan::orig::arg::f32(static_cast<float>(kFpu.mul(static_cast<double>(r), deg))));  // 0x004ff870
            }
        }

        if (static_cast<i32>(jo::view<i8>(jo::as_player(self)->follow_mode)) != 0) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_camera_follow(img::obj(kCameraMan))->target()) != 0) return;  // 0x00563dc0

        float& yaw = jo::as_cam_params(cam)->orbit[0];    // [ebp-0x30]
        float& pitch = jo::as_cam_params(cam)->orbit[1];  // [ebp-0x2c]
        if (global<i32>(d::g_mouse_look_dx) != 0) {
            (void)arg;  // `arg > 0` selects one of two identical branches
            yaw = static_cast<float>(kFpu.sub(yaw, kFpu.mul(static_cast<double>(turn), deg)));
        }
        if (global<i32>(d::g_mouse_dy) != 0) {
            const u32 saved = jo::view<u32>(jo::as_cam_params(cam)->orbit[1]);  // [ebp-0x44]
            const float invert1 = global<i8>(d::g_cfg_invert_look) != 0 ? -1.0f : 1.0f;
            const i32 steps = ftol(kFpu.mul(static_cast<double>(global<i32>(d::g_mouse_dy)), sens));
            pitch = static_cast<float>(kFpu.add(kFpu.mul(kFpu.mul(static_cast<double>(steps), deg), invert1), pitch));
            if (pitch > cf(d::flt_0_61086524)) jo::view<u32>(jo::as_cam_params(cam)->orbit[1]) = 0x3f1c61aau;
            if (!(pitch >= cf(d::flt_m0_34906584))) jo::view<u32>(jo::as_cam_params(cam)->orbit[1]) = 0xbeb2b8c2u;
            const float invert2 = global<i8>(d::g_cfg_invert_look) != 0 ? -1.0f : 1.0f;
            if (!(kFpu.mul(static_cast<double>(global<i32>(d::g_mouse_dy)), invert2) >= cf(d::k_9cf5c4))) {
                float m[16];  // [ebp-0x84]
                joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::f32(jo::view<u32>(jo::as_cam_params(cam)->orbit[0])), joan::orig::arg::f32(jo::view<u32>(jo::as_cam_params(cam)->orbit[1])), joan::orig::arg::f32(jo::view<u32>(jo::as_cam_params(cam)->orbit[2])), joan::orig::arg::f32(0u));  // 0x0060bd00
                V3 axis;  // [ebp-0x90]
                joan::orig::matrix_axis_z(&axis, static_cast<void*>(m));  // 0x0060c220
                const u32 scale = jo::view<u32>(jo::as_cam_params(cam)->orbit[3]);                        // [ebp-0x94]
                V3 t1;
                const V3 scaled = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t1, &axis, joan::orig::arg::f32(scale)));  // 0x0060cd00
                V3 t2;
                const V3 pos_sum = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&t2, joan::orig::arg::ptr(jo::as_cam_params(cam)->position), &scaled));  // 0x0060cc90
                // 0x00429090 reads a box (min, max): the degenerate box {pos, pos} ([ebp-0xb8] and its copy [ebp-0xac]).
                V3 pos_box[2] = {pos_sum, pos_sum};
                V3& pos = pos_box[0];  // [ebp-0xb8] (copies [ebp-0xa0], [ebp-0xac] are the same value)
                i32 count = 0;     // [ebp-0xbc]
                void** const cells =
                    joan::orig::arg::as<void**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(pos_box, &count));  // 0x00429090
                if (count != 0) {
                    void* const tile = joan::orig::arg::as<void*>(joan::orig::as_world_cell(cells[0])->grid(joan::orig::arg::i16(0)));  // 0x005600c0
                    if (tile != nullptr) {
                        const float fx = static_cast<float>(
                            kFpu.div(kFpu.sub(pos.x, static_cast<double>(jo::as_cell(cells[0])->origin_x)),
                                     static_cast<double>(jo::as_path_space(tile)->cell_w)));
                        const float fy = static_cast<float>(
                            kFpu.div(kFpu.sub(pos.y, static_cast<double>(jo::as_cell(cells[0])->origin_y)),
                                     static_cast<double>(jo::as_path_space(tile)->cell_h)));
                        const i32 ix = ftol(fx);
                        const i32 iy = ftol(fy);
                        const i32 sum = static_cast<i32>(static_cast<u32>(ix) + static_cast<u32>(iy));
                        const i32 parity = (sum % 2) == 0 ? 1 : 0;
                        const float ground = joan::orig::arg::as<float>(joan::orig::as_terrain(jo::as_path_space(tile)->grid)->height_4c7b70(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), static_cast<char>(joan::orig::arg::i32(parity))));  // 0x004c7b70
                        if (ground > pos_sum.z) jo::view<u32>(jo::as_cam_params(cam)->orbit[1]) = saved;
                    }
                }
            }
        }
        void* const motion0 = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(img::obj(kCameraMan))->motion(0));  // 0x00563c80
        joan::orig::as_camera_motion(motion0)->retarget(joan::orig::arg::mptr(static_cast<void*>(cam)));      // 0x00423f40
    }
};

JOAN_REPLACE("game/b115_04", "PlayerControl::mouse_look", 0x00431790, &H::mouse_look);

}  // namespace game_b115_04
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kUnits = obj(d::g_units);

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
void unit_pos(void* u, Vec3* out) { joan::orig::as_unit(u)->position(joan::orig::arg::mptr(out)); }                               // 0x004fedb0
void mount_set_42cf20(void* m, std::int32_t v) { joan::orig::as_model_instance(m)->set_10a(joan::orig::arg::u32(v)); }                          // 0x0042cf20
std::uint8_t box_contains(const void* box, const Vec3* p) { return joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(box)->contains(joan::orig::arg::ptr(p))); }  // 0x0060d5f0

JOAN_COUNTER(c_update_units, "Trigger::update_units");

// ---- static initialisers / free functions ----
using joan::orig::helpers::div_init;
void JOAN_CDECL init_432ce0() { div_init(d::g_a336f4, d::g_a336e8, d::flt_4); }

struct H {
    // 0x00431f20
    void update_units() {
        c_update_units.hit();
        if (jo::self<jo::Trigger>(this)->active == 0) return;
        if (unit_missing(joan::orig::arg::as<std::int32_t>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id())) != 0) return;  // 0x004203c0
        std::uint8_t* const player = unit_get(joan::orig::arg::as<std::int32_t>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id()));  // ebp-0x14
        Vec3 pos;  // ebp-0x10
        unit_pos(player, &pos);
        const Fpu fpu{true};
        float box[6];  // ebp-0x2c: min xyz, max xyz
        box[0] = static_cast<float>(fpu.sub(pos.x, static_cast<double>(jo::self<jo::Trigger>(this)->radius)));
        box[1] = static_cast<float>(fpu.sub(pos.y, static_cast<double>(jo::self<jo::Trigger>(this)->radius)));
        box[2] = static_cast<float>(fpu.sub(pos.z, static_cast<double>(jo::self<jo::Trigger>(this)->radius)));
        box[3] = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::Trigger>(this)->radius), pos.x));
        box[4] = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::Trigger>(this)->radius), pos.y));
        box[5] = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::Trigger>(this)->radius), pos.z));
        for (std::int32_t n = joan::orig::as_dynarray(this)->size(); n > 0; --n) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->empty_at_56f3f0(joan::orig::arg::i32(n))) != 0) continue;            // 0x0056f3f0
            std::uint8_t* const o = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(this)->get_56f320(joan::orig::arg::i32(n)));      // 0x0056f320
            Vec3 v;  // ebp-0x38
            joan::orig::as_actor(o)->position(&v);  // 0x005ac270
            if (box_contains(box, &v) == 0) {
                mount_set_42cf20(o, 0);
                continue;
            }
            std::uint8_t* const region =
                joan::orig::arg::as<std::uint8_t*>(joan::orig::as_waters(obj(d::g_water_array))->find_at(&v, joan::orig::arg::i32(jo::self<jo::Trigger>(this)->radius)));  // 0x00469d10
            // The original reads [region + 0x30] here even when region is null (kept: faults the
            // same way; address formed as an integer so no null pointer arithmetic).
            if (jo::as_actor(o)->scene != nullptr && jo::as_actor(o)->scene != jo::as_unit(player)->scene &&
                jo::as_actor(o)->scene !=
                    *reinterpret_cast<void* volatile*>(reinterpret_cast<std::uintptr_t>(region) + offsetof(jo::Water, u32_30))) {
                joan::orig::as_scene(jo::as_actor(o)->scene_params0)->nop(
                    joan::orig::arg::mptr(o));  // 0x005eb590
            }
            if (region != nullptr)
                joan::orig::as_scene(jo::as_water(region)->u32_30)->attach(
                joan::orig::arg::mptr(o));  // 0x005efc50
            else
                joan::orig::as_scene(jo::as_unit(player)->scene)->attach(joan::orig::arg::mptr(o));
            mount_set_42cf20(o, 1);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("Trigger::update_units", 0x00431f20, &H::update_units);
B("init_432ce0", 0x00432ce0, &init_432ce0);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_scroll_angle, "Scroll::angle");

struct H {
    // ------------------------------------------------------------------ 0x00432100
    float scroll_angle() {
        c_scroll_angle.hit();
        const i32 x = global<i32>(d::g_mouse_delta_x);
        const i32 y = static_cast<i32>(0u - global<u32>(d::g_mouse_delta_y));
        if (x == 0 && y == 0) return global<float>(d::flt_m1);  // -1
        if (x == 0 && y > 0) return global<float>(d::flt_90);   // 90
        if (x == 0 && y < 0) return global<float>(d::flt_270);   // 270
        if (y == 0 && x > 0) return global<float>(d::k_9cf5c4);   // 0
        if (y == 0 && x < 0) return global<float>(d::flt_180);   // 180
        const Fpu fpu{true};
        const double q = fpu.div(static_cast<double>(y), static_cast<double>(x));  // fild / fidiv
        const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(q)));                        // atan
        float r = static_cast<float>(fpu.div(a, static_cast<double>(global<float>(d::flt_0_017453292))));
        if (x < 0 && y > 0) r = r + global<float>(d::flt_180);
        if (x < 0 && y < 0) r = r + global<float>(d::flt_180);
        if (!(r >= global<float>(d::k_9cf5c4))) r = r + global<float>(d::flt_360);  // < 0 or NaN
        return r;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Scroll::angle", 0x00432100, &H::scroll_angle);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::orig::helpers::memset_;  // memset

// Order constructors: base 0x00416a50, vtable, memset(this + 0xc2, 0, n).
void* order_ctor(void* self, img::Addr vtbl, u32 n) {
    joan::orig::as_map_object(self)->construct();  // 0x00416a50
    jo::view<std::uintptr_t>(jo::as_map_object(self)->vtable) = img::addr(vtbl);
    memset_(reinterpret_cast<u8*>(jo::as_map_object(self)) + sizeof(jo::MapObject), 0, n);  // the derived order's members after the MapObject base (+0xc2)
    return self;
}

struct H {
    // ------------------------------------------------------------------------ constructors
    void* order_432210() { return order_ctor(this, d::vt_Obj_9d3c88, static_cast<u32>(sizeof(jo::Wall) - sizeof(jo::MapObject))); }  // 0x2d on x86

    // ------------------------------------------------------------------------ 0x00432250
    void set_clamped(float v) {
        if (joan::optmenu::mouse::on() && v < -1.0f) {  // [Fixes] MouseOptions: below the original's minimum
            jo::self<jo::Player>(this)->f32_e1 = joan::optmenu::mouse::low_look_option(v);
            return;
        }
        if (!(v > -1.0f)) {  // <= -1 or NaN
            jo::view<u32>(jo::self<jo::Player>(this)->f32_e1) = 0xbf800000u;
        } else if (!(v < 1.0f)) {
            jo::view<u32>(jo::self<jo::Player>(this)->f32_e1) = 0x3f800000u;
        } else {
            jo::self<jo::Player>(this)->f32_e1 = v;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Order2d::ctor", 0x00432210, &H::order_432210);
B93("Order::set_clamped", 0x00432250, &H::set_clamped);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
namespace {
namespace game_b86_05 {

using joan::call::cdecl_call;

void* const kGameUnits = obj(d::g_units);  // unit manager
void* const kTypes = obj(d::g_types);      // unit types

using joan::orig::helpers::stricmp_;

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* type_of(std::int16_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x0042d0a0

static_assert(sizeof(Vec3) == 12);

JOAN_COUNTER(c_select_hero, "Player::select_hero");

// Shared pieces of 0x00432300.
void release_carrier(std::uint8_t* unit) {
    if (jo::as_unit(unit)->orders != nullptr) {
        joan::orig::as_action(jo::as_unit(unit)->orders)->dispatch(joan::orig::arg::mptr(static_cast<void*>(unit)));  // 0x004bb660
        joan::orig::dynarray(jo::as_unit(unit)->orders)->zap(1);                  // 0x00584580
    }
}
void order_clear(std::uint8_t* unit) {
    joan::orig::as_unit_task(&jo::as_unit(unit)->task)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 0u, 0u, 0u, 0u);  // 0x004e4520
}
// Saves the new unit's order (+0x2a5 -> +0x2be) and clears its group members' orders.
void take_orders(std::uint8_t* unit) {
    if (jo::as_unit(unit)->task.kind != 0 || jo::as_unit(unit)->task.target == 0) {
        std::memcpy(&jo::as_unit(unit)->task_next, &jo::as_unit(unit)->task, sizeof(jo::Unit::task));  // rep movsd x6 + movsb
        order_clear(unit);
    }
    if (jo::as_unit(unit)->group != nullptr) {
        for (std::int32_t j = static_cast<std::int32_t>(jo::as_group(jo::as_unit(unit)->group)->count) - 1; j >= 0; --j) {
            if (unit_missing(jo::as_group(jo::as_unit(unit)->group)->members[j]) != 0) continue;
            std::uint8_t* const member = unit_get(jo::as_group(jo::as_unit(unit)->group)->members[j]);
            release_carrier(member);
            order_clear(member);
        }
    }
}
// Restores the old unit's saved order (+0x2be -> +0x2a5).
void restore_order(std::uint8_t* old) {
    if (jo::as_unit(old)->task_next.kind != 0 || jo::as_unit(old)->task_next.target == 0) {
        std::memcpy(&jo::as_unit(old)->task, &jo::as_unit(old)->task_next, sizeof(jo::Unit::task));
        joan::orig::as_unit_task(&jo::as_unit(old)->task_next)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 1u, 0u, 0u, 0u);  // 0x004e4520
    }
}

struct H {
    // Selection switch common to both paths of 0x00432300.
    void switch_to(std::uint8_t* unit, std::int32_t id) {
        joan::orig::as_player(this)->switch_unit(1);  // 0x0042b6e0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->mounted()) != 0) {  // 0x00430190
            jo::self<jo::Player>(this)->mounted = 1;
        } else {
            jo::self<jo::Player>(this)->mounted = 0;
        }
        joan::orig::as_player(obj(d::g_player_unit_recno))->set_unit(joan::orig::arg::i32(id));  // 0x0042f9c0
        global<std::int32_t>(d::g_player_side) = jo::as_unit(unit)->side;
        joan::orig::as_hud(obj(d::g_player_hud))->update_health_state(joan::orig::arg::i32(id));                          // 0x00703750
        joan::orig::as_mini_map(obj(d::g_mini_map))->track_unit(joan::orig::arg::i16(global<std::int16_t>(d::g_player_unit_recno)));  // 0x0070fb70
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_8(static_cast<std::int32_t>(global<std::int16_t>(d::g_a47c36)));  // 0x005639f0
        if (jo::self<jo::Player>(this)->mode == 3) joan::orig::as_player(this)->set_follow(1);  // 0x00436190
    }

    // Hero `i` (0-based) of the Switch to / Command keys: the four of the game's table 0x9f8b10, then ([Game]
    // AllHeroes) the Bastard of Orleans, Richemont and Charles VII (unit types as the hero cheats name them).
    static const char* hero_name(std::int32_t i) {
        if (i < 4) return &img::global<const char>(d::g_9f8b10 + static_cast<std::uint32_t>(i) * 0x104u);
        static const img::Addr kMore[3] = {d::s_ORLEAN, d::s_arthur_de_richemont, d::s_charlie7};
        return joan::img::str(kMore[i - 4]);
    }
    static std::int32_t type_key(const char* name) {
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_unit_types(kTypes)->find(joan::orig::arg::str(name)));  // 0x005270b0
    }
    // The checks and the switch of the Switch character cycle below, for a start from hero 5..7.
    bool can_take_over(std::uint8_t* cur, std::int32_t id) {
        std::uint8_t* const unit = unit_get(id);
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(unit)->dead())) != 0) return false;  // 0x0042d0e0
        if (jo::as_unit(unit)->speech_off != 0 || jo::as_unit(unit)->markers == -1) return false;
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_game(unit)->byte_2e1())) != 0) return false;  // 0x00432b90
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(cur)->dead())) != 0) return true;
        const Vec3 pos = jo::view<Vec3>(jo::as_unit(unit)->position_);
        return static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_mini_map(obj(d::g_mini_map))->near_tracked_unit(pos.x, pos.y, joan::orig::arg::bits(pos.z)))) != 0;  // 0x00717a90
    }
    void take_over(std::uint8_t* cur, std::int32_t id) {
        std::uint8_t* const unit = unit_get(id);
        switch_to(unit, id);
        release_carrier(unit);
        joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
        if (jo::self<jo::Player>(this)->mode != 4) {
            take_orders(unit);
            restore_order(cur);
        }
    }

    // ---- 0x00432300 Player::select_hero ----------------------------------------------------
    void select_hero(std::int32_t hero) {
        c_select_hero.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return;
        std::uint8_t* const cur = unit_get(jo::self<jo::Player>(this)->unit_id_);
        type_of(jo::as_unit(cur)->type_id);  // result unused (ebp-0xc)
        std::int32_t slot = -1;
        for (std::int32_t i = 0; i < 4; ++i) {
            const char* const name = &img::global<const char>(d::g_9f8b10 + i * 0x104);
            if (stricmp_(jo::as_unit_type(type_of(jo::as_unit(cur)->type_id))->name, name) == 0) {
                slot = i;
                break;
            }
        }
        // [Game] AllHeroes: heroes 5..7 (the Bastard of Orleans, Richemont, Charles VII) can be played too.
        const bool all = joan::hook::settings().all_heroes != 0;
        if (slot == -1 && all) {
            for (std::int32_t i = 4; i < 7; ++i) {
                if (static_cast<std::int32_t>(jo::as_unit(cur)->type_id) == type_key(hero_name(i))) {
                    slot = i;
                    break;
                }
            }
        }
        if (slot == -1) return;
        if (hero == 0 && slot >= 4) {  // switch character from hero 5..7: the next of the original four that can take over
            for (std::int32_t next = 0; next < 4; ++next) {
                const std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_units(kGameUnits)->find_by_3b(joan::orig::arg::i32(type_key(hero_name(next)))));  // 0x0051fd50
                if (id != 0 && can_take_over(cur, id)) {
                    take_over(cur, id);
                    return;
                }
            }
            return;
        }
        if (hero == 0) {
            std::int32_t next = (slot + 1) % 4;
            while (next != slot) {
                const std::int32_t key = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit_types(kTypes)->find(joan::orig::arg::str(&img::global<const char>(d::g_9f8b10 + next * 0x104))));  // 0x005270b0
                const std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_units(kGameUnits)->find_by_3b(joan::orig::arg::i32(key)));  // 0x0051fd50
                if (id != 0) {
                    std::uint8_t* const unit = unit_get(id);
                    if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(unit)->dead())) == 0 &&  // 0x0042d0e0
                        jo::as_unit(unit)->speech_off == 0 &&
                        static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_game(unit)->byte_2e1())) == 0 &&  // 0x00432b90
                        jo::as_unit(unit)->markers != -1) {
                        bool ok = static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(cur)->dead())) != 0;  // 0x0042d0e0
                        if (!ok) {
                            const Vec3 pos = jo::view<Vec3>(jo::as_unit(unit)->position_);
                            ok = static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_mini_map(obj(d::g_mini_map))->near_tracked_unit(pos.x, pos.y, joan::orig::arg::bits(pos.z)))) != 0;  // 0x00717a90
                        }
                        if (ok) {
                            switch_to(unit, id);
                            release_carrier(unit);
                            joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
                            if (jo::self<jo::Player>(this)->mode != 4) {
                                take_orders(unit);
                                restore_order(cur);
                            }
                            return;
                        }
                    }
                }
                next = (next + 1) % 4;
                global<std::uint8_t>(d::g_a48ce2) = 1;
                global<std::uint8_t>(d::g_a48ce0) = 0;
            }
            return;
        }
        if (slot == hero - 1) return;
        if (hero > 4 && !all) return;
        const std::int32_t key = type_key(hero_name(hero - 1));  // 0x005270b0
        const std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_units(kGameUnits)->find_by_3b(joan::orig::arg::i32(key)));  // 0x0051fd50
        if (id == 0) return;
        if (jo::self<jo::Player>(this)->unit_id_ == id) return;
        std::uint8_t* const unit = unit_get(id);
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(unit)->dead())) != 0) return;  // 0x0042d0e0
        if (jo::as_unit(unit)->speech_off != 0) return;
        if (jo::as_unit(unit)->markers == -1) return;
        switch_to(unit, id);
        if (jo::self<jo::Player>(this)->mode != 4) {
            release_carrier(unit);
            joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
            take_orders(unit);
            restore_order(cur);
        }
        global<std::uint8_t>(d::g_a48ce2) = 1;
        global<std::uint8_t>(d::g_a48ce0) = 0;
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("Player::select_hero", 0x00432300, &H::select_hero);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_world4.cpp (game/world4)
namespace {
namespace game_world4 {

constexpr img::Addr kProgress(d::g_progress);

JOAN_COUNTER(c_load, "Cell::load");

struct H {
    static void progress_step() {
        joan::orig::as_progress(img::obj(kProgress))->update();  // 0x0075f3c0
        joan::orig::as_progress(img::obj(kProgress))->redraw();  // 0x0075f4f0
    }
    static void pump() {
        joan::orig::as_sound_stream(img::obj(d::g_snd))->update();  // 0x0059b1c0
        joan::orig::as_ambient_streams(img::obj(d::g_mus))->update();  // 0x005866d0
    }
    // 0x004329e0 Cell::load(boxes, count): +0x43 state; parts +0x94 (next +0xae, virtual
    // +0x1c), +0x8c (next +0x33d, 0x00442820), +0x9c (next +0x3e, 0x00456fb0).
    void cell_load(void* boxes, std::int32_t count) {
        joan::hitch::Span hitch_span("cell_load");  // [Debug] HitchLog
        global<std::int32_t>(d::g_a5026d) = 3;
        global<std::int32_t>(d::g_a50271) = 0;
        if (static_cast<std::int8_t>(jo::self<jo::Cell>(this)->loaded) != 0) return;
        c_load.hit();
        jo::self<jo::Cell>(this)->loaded = 1;
        global<std::int32_t>(d::g_a50271) = 1;
        progress_step();
        joan::orig::as_cell(this)->load_files(joan::orig::arg::mptr(boxes), joan::orig::arg::i32(count));  // 0x004f2f80
        pump();
        global<std::int32_t>(d::g_a50271) = 2;
        progress_step();
        for (std::uint8_t* p = jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->objects_tail); p != nullptr;) {
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_map_object(p)->cell_prev);
            using Virtual = void (Obj::*)();
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(p)[0x1c / 4], sizeof(void*));
            (reinterpret_cast<Obj*>(p)->*method)();
            pump();
            p = next;
        }
        global<std::int32_t>(d::g_a50271) = 3;
        progress_step();
        for (std::uint8_t* p = jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->units_tail); p != nullptr;) {
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_unit(p)->cell_prev);
            joan::orig::as_cell_object(p)->attach();  // 0x00442820
            pump();
            p = next;
        }
        for (std::uint8_t* p = jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->items_tail); p != nullptr;) {
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_world_item(p)->cell_prev);
            joan::orig::as_world_item(p)->attach_zone_456fb0();  // 0x00456fb0
            pump();
            p = next;
        }
    }
};

#define W4(name, addr, fn) JOAN_REPLACE("game/world4", name, addr, fn)
W4("Cell::load", 0x004329e0, &H::cell_load);
#undef W4

}  // namespace game_world4
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

struct H {
    // 0x00432b90: mov al, [+0x2e1] (upper bits of eax = this, as the original).
    std::uintptr_t byte_2e1() {
        return (reinterpret_cast<std::uintptr_t>(this) & ~std::uintptr_t{0xffu}) | jo::view<std::uint8_t>(jo::self<jo::Unit>(this)->stance);
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Game::byte_2e1", 0x00432b90, &H::byte_2e1);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b105_01_2.cpp (game/b105_01)
namespace {
namespace game_b105_01_2 {

using joan::call::cdecl_call;

using joan::call::cdecl_call;

// ---- CRT initialiser wrappers: call the body directly, return its eax ----
template <fn::Code Body>
u32 JOAN_CDECL init_wrapper() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("init_0x00432bc0", 0x00432bc0, &init_wrapper<fn::init_pi_a336e8>);
B("init_0x00432c20", 0x00432c20, &init_wrapper<fn::init_a336ec>);
B("init_0x00432c70", 0x00432c70, &init_wrapper<fn::init_a336f0>);
B("init_0x00432cc0", 0x00432cc0, &init_wrapper<fn::init_432ce0>);
#undef B

}  // namespace game_b105_01_2
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------- static initialisers
void init_pi(img::Addr g) {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(g) = static_cast<float>(t * 4.0);        // qword 0x9cf788 = 4.0 (exact)
}

static void JOAN_CDECL init_pi_a336e8() { init_pi(d::g_a336e8); }  // 0x00432be0

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_pi_a336e8", 0x00432be0, &init_pi_a336e8);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

static void JOAN_CDECL init_a336ec() { div_global(d::g_a336ec, d::g_a336e8, d::flt_2); }  // 0x00432c40

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("init_a336ec", 0x00432c40, &init_a336ec);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Inventory::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b90_03::H::get_6ae>(this)); }  // 0x430340
auto joan::orig::SaveMenu::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b90_03::H::get_4b05>(this)); }  // 0x430370
auto joan::orig::OptMenu::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b90_03::H::get_122>(this)); }  // 0x4303a0
auto joan::orig::ObjMenu::is_open() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_02::H::byte_get>(this)); }  // 0x4303d0
auto joan::orig::Hud::is_open() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_02::H::byte_get>(this)); }  // 0x4303f0
auto joan::orig::Units::get_430410(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::get_unit>(this, a0)); }  // 0x430410
auto joan::orig::Player::enter_mode3() -> void { joan::bind::member<&::game_b105_01::H::nop_430440>(this); }  // 0x430440
auto joan::orig::Player::leave_mode3() -> void { joan::bind::member<&::game_b85_06::H::nop0>(this); }  // 0x430460
auto joan::orig::Player::handle_input_mode3() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b109_11::H::player_update>(this)); }  // 0x430480
auto joan::orig::Unit::update_view() -> void { joan::bind::member<&::game_units4::H::view_update>(this); }  // 0x4311d0
auto joan::orig::Player::tick_mode3() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_01::H::dtor_pair>(this)); }  // 0x431520
auto joan::orig::Player::draw_mode3() -> void { joan::bind::member<&::game_b82_18::H::empty>(this); }  // 0x431550
auto joan::orig::Player::direction_angle(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_01::H::direction_angle>(this, a0, a1, a2, a3)); }  // 0x431570
auto joan::orig::Player::anim_code(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_01::H::anim_code>(this, a0, a1, a2, a3)); }  // 0x431680
auto joan::orig::init_a3cb74() -> void { joan::bind::free<&::game_b98_01::init_a3cb74>(); }  // 0x431760
auto joan::orig::Scroll::angle() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b95_01::H::scroll_angle>(this)); }  // 0x432100
auto joan::orig::Order::set_clamped(float a0) -> void { joan::bind::member<&::game_b93_01::H::set_clamped>(this, a0); }  // 0x432250
auto joan::orig::Player::select_hero(std::int32_t a0) -> void { joan::bind::member<&::game_b86_05::H::select_hero>(this, a0); }  // 0x432300
auto joan::orig::Cell::load(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_world4::H::cell_load>(this, a0, a1); }  // 0x4329e0
auto joan::orig::Game::byte_2e1() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b::H::byte_2e1>(this)); }  // 0x432b90
