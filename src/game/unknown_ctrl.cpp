// game/unknown_ctrl.cpp: functions whose original source file is unknown (formerly game/_unplaced/ctrl.cpp;
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
#include <algorithm>
#include <cmath>

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
#include "pad_feedback.hpp"
#include "../platform/input.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::hook::global;

using joan::img::obj;

using i32 = std::int32_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::call::import_call;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;                      // operator delete

using joan::orig::helpers::at;

using joan::orig::helpers::scalar_delete;

struct H {
    std::int32_t ref_add(std::int16_t id) {  // 0x00437af0
        std::uint8_t* const r = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(this)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x0041a180
        // x64: needs ModelResEntry (ModelResMgr::entry record) +0x10 i32 refs, +0x14 u8 loaded
        at<std::int32_t>(r, 0x10) += 1;
        if (at<std::int32_t>(r, 0x10) == 1) at<std::uint8_t>(r, 0x14) = 1;
        return at<std::int32_t>(r, 0x10);
    }

    void* sdd_436780(std::uint32_t flags) { joan::orig::as_water(this)->clear(); scalar_delete(this, flags); return this; }  // 0x00546b20

    void nop0() {}                                                               // 0x004343c0 / 0x00432d10 / 0x00430460
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Refs::add", 0x00437af0, &H::ref_add);
B85("Obj436780::sdd", 0x00436780, &H::sdd_436780);
B85("Obj4343c0::nop", 0x004343c0, &H::nop0);
B85("Obj432d10::nop", 0x00432d10, &H::nop0);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b109_18.cpp (game/b109_18)
namespace {
namespace game_b109_18 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

// x87 compare outcomes (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool lt_or_nan(double a, double b) { return !(a >= b); }
bool gt(double a, double b) { return a > b; }

JOAN_COUNTER(c_update, "Ctrl::update_432d30");
JOAN_COUNTER(c_pad_look, "Ctrl::pad_look_camera (pad right stick)");

// The pad's right stick in the joystick type: turns (x) and tilts (y) the follow camera directly, the hero keeps its
// heading. -1..1 each, from platform::input::pad_look; at full tilt 180 degrees a second around, 90 up / down.
// The game's mouse look (PlayerControl::mouse_look 0x00431790 / [Fixes] MouseTurn) only does whole mouse counts at
// a time and turns the hero with the camera.
// On foot the camera then holds the angle the stick left it at: the game swings the follow camera's yaw / pitch by
// itself (walls, corners, the pitch's way back to its rest), which with the camera on the right stick reads as the
// camera moving untouched. It lets go while a camera zone of the map has the camera (scripted views), the camera
// swings behind the hero (follow_mode), another view is up, or on horseback / riding.
struct PadLook {
    bool held = false;
    float yaw = 0.0f, pitch = 0.0f;
    u32 last_ms = 0;
};
PadLook g_pad_look;

void pad_look_camera(void* self, float x, float y) {
    PadLook& h = g_pad_look;
    const u32 now = global<u32>(d::g_now);
    const bool fresh = !h.held || now - h.last_ms > 250u;  // the pad camera was not running (another input type ...)
    h.held = false;
    h.last_ms = now;
    if (jo::view<std::int8_t>(jo::self<jo::Player>(self)->follow_mode) != 0) return;  // swinging behind the hero
    void* const views = obj(d::g_camera_man);
    const i32 view = joan::orig::arg::as<i32>(joan::orig::as_camera_follow(views)->target());  // 0x00563dc0
    static u32 last_log = 0;
    const bool log = now - last_log >= 1000u;
    if (view != 0) {
        if (log && (x != 0.0f || y != 0.0f)) {
            last_log = now;
            joan::hook::log_line("[Pad] right stick: camera view %d, not the follow camera - no look", view);
        }
        return;
    }
    const u32 id = jo::view<u32>(jo::self<jo::Player>(self)->unit_id_);
    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(id)) != 0) return;  // 0x00428b70
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
    const bool on_foot = jo::view<i8>(jo::self<jo::Player>(self)->riding) == 0 && jo::view<i8>(jo::self<jo::Player>(self)->mounted) == 0;
    bool zone = false;  // a camera zone of the map steers the camera (Ctrl::turn_4352f0's test)
    if (jo::as_unit(unit)->cell != nullptr && jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->camera_zones) != nullptr)
        zone = joan::orig::arg::as<i16>(joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->camera_zones))->zone_free()) != 0;  // 0x00427d10
    if (zone) return;
    c_pad_look.hit();
    alignas(4) std::uint8_t cam[0x28];  // 0x00423fe0: position, yaw, pitch, distance, scale
    void* const motion = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(views)->motion(i32{0}));  // 0x00563c80
    joan::orig::as_camera_motion(motion)->end(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x00423fe0
    float& yaw = jo::as_cam_params(cam)->orbit[0];
    float& pitch = jo::as_cam_params(cam)->orbit[1];
    if (on_foot && !fresh) {
        float drift = yaw - h.yaw;
        if (drift > 3.1415927f) drift -= 6.2831855f;
        if (drift < -3.1415927f) drift += 6.2831855f;
        if (log && (std::fabs(drift) > 0.02f || std::fabs(pitch - h.pitch) > 0.02f)) {
            last_log = now;
            joan::hook::log_line("[Pad] camera held: the game moved it by yaw %.1f pitch %.1f degrees", drift * 57.29578f,
                                 (pitch - h.pitch) * 57.29578f);
        }
        yaw = h.yaw;
        pitch = h.pitch;
    }
    const float dt = static_cast<float>(std::min<u32>(global<u32>(d::g_frame_elapsed_ms), 100u)) / 1000.0f;  // this frame, ms
    const float invert = global<std::int8_t>(d::g_cfg_invert_look) != 0 ? -1.0f : 1.0f;  // the game's own invert-mouse option
    yaw -= x * 3.1415927f * dt;
    if (yaw < -3.1415927f) yaw += 6.2831855f;
    if (yaw > 3.1415927f) yaw -= 6.2831855f;
    pitch += y * 1.5707964f * dt * invert;
    if (pitch > global<float>(d::flt_0_61086524)) pitch = global<float>(d::flt_0_61086524);    // the mouse's limits
    if (pitch < global<float>(d::flt_m0_34906584)) pitch = global<float>(d::flt_m0_34906584);
    if (on_foot) {
        h.held = true;
        h.yaw = yaw;
        h.pitch = pitch;
        joan::padfx::g_camera_hold = {motion, yaw, pitch, now};
    }
    joan::orig::as_camera_motion(motion)->retarget(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x00423f40
    if (log && (x != 0.0f || y != 0.0f)) {
        last_log = now;
        joan::hook::log_line("[Pad] right stick %.2f %.2f: yaw %.1f pitch %.1f (frame %u ms)", x, y, yaw * 57.29578f, pitch * 57.29578f,
                             global<u32>(d::g_frame_elapsed_ms));
    }
}

struct H {
    // ------------------------------------------------------------------ 0x00432d30
    i32 key_down(i32 k) { return joan::orig::arg::as<i32>(joan::orig::as_bindings(obj(d::g_keys_a48820))->state(i32{1}, joan::orig::arg::u32(k))); }  // 0x004cb6d0
    i32 key_hit(i32 k) { return joan::orig::arg::as<i32>(joan::orig::as_race_res(obj(d::g_keys_a48820))->test(i32{1}, joan::orig::arg::i32(k))); }   // 0x004cb820
    void* unit() { return joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))); }  // 0x0041b3c0

    void special_attack(i32 code) {
        void* const u = unit();
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(u)->can_act())) == 0) return;  // 0x00500ba0
        if (jo::as_unit(u)->action == 1) return;
        if (jo::as_unit(u)->anim_kind == 5) return;
        if (jo::as_unit(u)->anim_kind == 6) return;
        if (jo::as_unit(u)->anim_set == nullptr) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::as_unit(u)->anim_set)->index_of_416dc0(joan::orig::arg::i32(code))) == 0) return;  // 0x00416dc0
        joan::orig::as_unit(unit())->set_action(joan::orig::arg::i32(code), i32{0}, i32{1});           // 0x00501190
    }

    i16 update() {
        c_update.hit();
        if (*static_cast<void* const*>(global<void*>(d::g_keys_a51806)) == nullptr) return 0;
        const i32 v8 = key_down(0x12);
        const i32 vc = key_down(2);
        i32 v10 = key_hit(0xd);
        const i32 v14 = key_down(0xc);
        i32 v18 = key_hit(0xe);
        i32 v1c = 0;
        i32 v20 = 0;
        if (v10 != 0 && v18 != 0) {
            v20 = 1;
            v10 = 0;
            v18 = 0;
        }
        i8 v28 = 0;
        if (key_hit(0x30) != 0) v28 = 1;
        if (key_hit(0x31) != 0) v28 = 2;
        i32 v2c = 0;  // uninitialised in the original when v1c == 0 (dead, see header)
        if (key_down(0x10) != 0) {
            jo::view<u32>(jo::self<jo::Player>(this)->jump_hold_ms) += global<u32>(d::g_frame_step_ms);
            if (jo::self<jo::Player>(this)->jump_hold_ms >= global<i32>(d::g_jumpmaxd)) {
                v1c = 1;
                v2c = jo::self<jo::Player>(this)->jump_hold_ms;
                jo::self<jo::Player>(this)->jump_hold_ms = 0;
            }
        } else if (jo::self<jo::Player>(this)->jump_hold_ms > 0) {
            v1c = 1;
            v2c = jo::self<jo::Player>(this)->jump_hold_ms;
            jo::self<jo::Player>(this)->jump_hold_ms = 0;
        }
        const i32 v30 = joan::orig::arg::as<i32>(joan::orig::as_input(obj(d::g_keys_a51806))->direction_598040());  // 0x00598040
        const float f34 = joan::orig::arg::as<float>(joan::orig::as_pad(this)->stick_angle_433af0());         // 0x00433af0
        const i32 v38 = joan::orig::arg::as<i32>(joan::orig::as_joystick(this)->deflection_level());             // 0x00433930
        const float f3c = joan::orig::arg::as<float>(joan::orig::as_unit(this)->link_value_of());         // 0x0042fd60
        // The joystick type turns the hero / camera only with the left stick (turn_4352f0 below); the keyboard types
        // turn with the mouse (PlayerControl::mouse_look 0x00431790, from this frame's mouse movement). The pad's right
        // stick moves as the mouse does ([Platform] PadCamera, platform/sdl_input.cpp): with it the right stick turns
        // and tilts the camera alone, and the left stick only moves the hero (relative to the camera).
        float look_x = 0.0f, look_y = 0.0f;
        const bool pad_camera = joan::platform::input::pad_look(look_x, look_y);
        if (pad_camera) pad_look_camera(this, look_x, look_y);
        i32 v40 = 0;
        i8 b44 = 0, b48 = 0, b4c = 0, b50 = 0;
        if (jo::self<jo::Player>(this)->last_dir_key != 0 && v38 <= 1) jo::self<jo::Player>(this)->dir_tap_armed = 1;
        if (!gt(85.0f, f34) && !gt(f34, 95.0f) && v38 == 3) {
            v40 = 4;
        } else if (!gt(175.0f, f34) && !gt(f34, 185.0f) && v38 == 3 && vc != 0) {
            v40 = 6;
        } else if (!gt(265.0f, f34) && !gt(f34, 275.0f) && v38 == 3 && vc != 0) {
            v40 = 5;
        } else if ((!gt(f34, 5.0f) || !lt_or_nan(f34, 355.0f)) && v38 == 3 && vc != 0) {
            v40 = 7;
        }
        if (v40 == jo::self<jo::Player>(this)->last_dir_key && jo::self<jo::Player>(this)->dir_tap_armed != 0 &&
            global<u32>(d::g_now) - jo::self<jo::Player>(this)->last_dir_key_ms < 0x12cu) {
            if (v40 == 4) {
                b44 = 1;
            } else if (v40 == 5) {
                b48 = 1;
            } else if (v40 == 7) {
                b4c = 1;
            } else if (v40 == 6) {
                b50 = 1;
            }
        }
        if (v40 != 0) {
            jo::self<jo::Player>(this)->last_dir_key = v40;
            jo::self<jo::Player>(this)->last_dir_key_ms = global<u32>(d::g_now);
            jo::self<jo::Player>(this)->dir_tap_armed = 0;
        }
        if (key_hit(0x13) != 0) joan::orig::as_order(this)->consume_43b490(joan::orig::arg::u32(i32{0}));  // 0x0043b490
        if (key_hit(0x14) != 0 && (global<u16>(d::g_key_modifiers) & 0x30) == 0) joan::orig::as_unit(this)->use_spiritual_item_43b990(i32{0});  // 0x0043b990
        if (jo::view<i8>(jo::self<jo::Player>(this)->riding) != 0) return joan::orig::arg::as<i16>(joan::orig::as_player(this)->command(joan::orig::arg::u32(v8), joan::orig::arg::u32(v10), joan::orig::arg::bits(f34), joan::orig::arg::u32(v38)));  // 0x0042a810
        if (jo::view<i8>(jo::self<jo::Player>(this)->mounted) != 0) {
            joan::orig::as_camera(this)->step_height(static_cast<i32>(v28));  // 0x00438720
            const i32 k = key_hit(0x11);
            return joan::orig::arg::as<i16>(joan::orig::as_player(this)->dispatch_keys(joan::orig::arg::u32(k), joan::orig::arg::u32(v10), joan::orig::arg::u32(v18), joan::orig::arg::u32(vc), joan::orig::arg::bits(f34), joan::orig::arg::u32(v30), joan::orig::arg::u32(v38)));  // 0x0043c5c0
        }
        if (jo::self<jo::Player>(this)->squad_mode != 0) return joan::orig::arg::as<i16>(joan::orig::as_player(this)->squad_act(joan::orig::arg::u32(v1c), joan::orig::arg::u32(v10), joan::orig::arg::u32(vc), joan::orig::arg::bits(f34), joan::orig::arg::u32(v30), joan::orig::arg::u32(v38)));  // 0x0042d270
        if (key_hit(0x11) != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(this)->recharge_43bef0()) == 0 &&  // 0x0043bef0
            joan::orig::arg::as<i32>(joan::orig::as_order(this)->spawn_effect()) == 0 &&                          // 0x0043bd40
            joan::orig::arg::as<i32>(joan::orig::as_unit(this)->try_link_43be40()) == 0 &&                          // 0x0043be40
            joan::orig::arg::as<i32>(joan::orig::as_player(this)->link_nearest_enemy()) == 0 &&                          // 0x0043c210
            joan::orig::arg::as<i32>(joan::orig::as_order(this)->fire_43bc50()) == 0) {                          // 0x0043bc50
            joan::orig::as_unit(this)->start_6c0310_43bfe0();                                // 0x0043bfe0
            joan::orig::as_order(this)->alencon_43c080();                                // 0x0043c080
        }
        if (global<u8>(d::g_game_running) == 0) return 0;
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->start_42b3c0(joan::orig::arg::u32(v8))) != 0) return 1;  // 0x0042b3c0
        joan::orig::as_camera(this)->step_height(static_cast<i32>(v28));  // 0x00438720
        if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->stub_zero(joan::orig::arg::u32(v14))) != 0) return 1;  // 0x00436380
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->find_target_4363a0(joan::orig::arg::u32(v14))) != 0) return 1;  // 0x004363a0
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(this)->try_flee(joan::orig::arg::i32(v14))) != 0) return 1;  // 0x00436250
        if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->order_update(joan::orig::arg::i32(v10), joan::orig::arg::i32(v18), static_cast<i32>(b44))) != 0) return 1;  // 0x004367c0
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->try_437910(joan::orig::arg::u32(v20))) != 0) return 1;  // 0x00437910
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) == 0) {  // 0x00428b70
            void* const u = unit();
            if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_e_14()) == 0) {  // 0x005091d0
                if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->try_mount(joan::orig::arg::i32(key_hit(0x11)))) != 0) return 1;  // 0x0043c7d0
                if (joan::orig::arg::as<i16>(joan::orig::as_squad(this)->try_rejoin_42d300(joan::orig::arg::i32(key_hit(0x11)))) != 0) return 1;  // 0x0042d300
                if (joan::orig::arg::as<i16>(joan::orig::as_unit(this)->launch_437b60(joan::orig::arg::i32(v1c), joan::orig::arg::i32(v38), joan::orig::arg::f32(f34), joan::orig::arg::f32(static_cast<float>(v2c)))) != 0) return 1;  // 0x00437b60
            }
        }
        if (b48 != 0) {
            special_attack(0x15);
            return 1;
        }
        if (b4c != 0) {
            special_attack(0x16);
            return 1;
        }
        if (b50 != 0) {
            special_attack(0x17);
            return 1;
        }
        joan::orig::as_player(this)->set_follow(joan::orig::arg::i32(vc));  // 0x00436190
        if (vc != 0 && v38 != 0) {
            i32 a = 0, b = 0, c = 0, d = 0;
            if (v30 == 0x25) {
                a = 1;
            } else if (v30 == 0x14) {
                b = 1;
            } else if (v30 == 0x1c) {
                c = 1;
            } else if (v30 == 0x22) {
                d = 1;
            } else if (v30 == 0xa) {
                a = 1;
                c = 1;
            } else if (v30 == 0xb) {
                a = 1;
                d = 1;
            } else if (v30 == 0xc) {
                b = 1;
                c = 1;
            } else if (v30 == 0xd) {
                b = 1;
                d = 1;
            }
            joan::orig::as_player(this)->order_direction(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d));  // 0x00438110
            return 1;
        }
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) == 0) {  // 0x00428b70
            void* const u = unit();
            if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_e_14()) != 0) joan::orig::as_unit(u)->play_idle(i32{0}, i32{1});  // 0x00508b90
        }
        joan::padfx::g_stick_keeps_camera = pad_camera;
        joan::orig::as_ctrl(this)->turn_4352f0(joan::orig::arg::f32(f34), joan::orig::arg::i32(v38), joan::orig::arg::f32(f3c));  // 0x004352f0
        joan::padfx::g_stick_keeps_camera = false;
        joan::orig::as_player(this)->unit_order(joan::orig::arg::i32(v30), joan::orig::arg::i32(vc));         // 0x00437ed0
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_18", name, addr, fn)
B("Ctrl::update_432d30", 0x00432d30, &H::update);
#undef B

}  // namespace game_b109_18
}  // namespace

// ---------------------------------------- from modules/game_b101_01.cpp (game/b101_01)
namespace {
namespace game_b101_01 {

using u32 = std::uint32_t;

using joan::orig::helpers::as_float;
u32 as_bits(float v) {
    u32 bits;
    std::memcpy(&bits, &v, 4);
    return bits;
}

void* const kEntities = obj(d::g_units);
void* const kTypes = obj(d::g_item_res);
void* const kRecords = obj(d::g_a5ccd0);
void* const kFollow = obj(d::g_camera_man);
void* const kMap = obj(d::g_physic_sys);
void* const kSound = obj(d::g_critical_hud);

u8* type_record(u16 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0
i32 entity_missing(u32 id) { return joan::orig::as_dynarray(kEntities)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* entity(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kEntities)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct Vec3 {
    u32 x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_try_contact, "Unit::try_contact_433670");
JOAN_COUNTER(c_update_a2, "Unit::update_a2_434270");
JOAN_COUNTER(c_launch, "Unit::launch_437b60");

struct H {
    // ------------------------------------------------------------------ 0x00433670
    u8 try_contact_433670(i16 id) {
        c_try_contact.hit();
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_list(kRecords)->at_485f20(joan::orig::arg::u32(static_cast<i32>(id))));  // 0x00485f20
        if (jo::as_trigger(rec)->flags != 0x100) return 1;
        if (jo::self<jo::MapObject>(this)->actor == nullptr) return 1;
        if (jo::self<jo::MapObject>(this)->actor->anim == nullptr) return 1;
        u8* const t = type_record(jo::view<u16>(jo::self<jo::MapObject>(this)->type_id));
        if (reinterpret_cast<std::uintptr_t>(t) + offsetof(jo::ItemDef, anim_names) == 0) return 1;  // &t->anim_names == null (+0x87 on x86)
        if (jo::as_item_def(t)->anim_names[0] == nullptr) return 1;
        if (jo::self<jo::MapObject>(this)->actor->anim_flags == 0) {
            const float left = joan::orig::arg::as<float>(joan::orig::as_timer(this)->remaining_41d280());  // 0x0041d280
            const float zero = global<float>(d::k_9cf5c4);
            // fcomp; test ah,0x40; je out: continue when equal or unordered
            if ((left < zero) || (left > zero)) return 1;
        }
        u32 box[6];  // point box: both corners = position +0x56
        std::memcpy(&box[3], jo::self<jo::MapObject>(this)->pos, 12);
        std::memcpy(&box[0], &box[3], 12);
        i32 count = 0;
        void** const nodes = joan::orig::arg::as<void**>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->world->scene))->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x005ef610
        i32 found = 0;
        for (i32 i = 0; i < count; ++i) {
            u8* const head = joan::orig::arg::as<u8*>(joan::orig::as_scene_s(nodes[i])->first_actor());  // 0x005effa0
            u8* p = head;
            if (p == nullptr) continue;
            do {
                if (jo::as_actor(p)->kind == 0xb) {
                    u8* const u = static_cast<u8*>(jo::as_actor(p)->owner);
                    float d[3];
                    d[0] = jo::as_trigger(rec)->center[0] - jo::view<float[3]>(jo::as_unit(u)->position_)[0];
                    d[1] = jo::as_trigger(rec)->center[1] - jo::view<float[3]>(jo::as_unit(u)->position_)[1];
                    d[2] = jo::as_trigger(rec)->center[2] - jo::view<float[3]>(jo::as_unit(u)->position_)[2];
                    const float ls = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(d)));  // length_sq
                    const float rr = jo::as_trigger(rec)->range * jo::as_trigger(rec)->range;
                    // fcompp (st0 = rr, st1 = ls); test ah,1; jne next: go on when rr >= ls
                    if (rr >= ls) {
                        if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_special())) == 0) {  // 0x00508ab0
                            found = 1;
                            break;
                        }
                    }
                }
                p = reinterpret_cast<u8*>(jo::as_actor(p)->scene_prev);
            } while (p != nullptr && p != head);
            if (found != 0) break;
        }
        if (found == 0) return 1;
        u8* const a = jo::view<u8*>(jo::self<jo::MapObject>(this)->actor);
        joan::orig::as_actor(a)->start_anim(joan::orig::arg::mptr(jo::as_actor(a)->anim), jo::as_item_def(t)->anim_names[0], joan::orig::arg::u32(global<u32>(d::g_now)), 0u, 0u, 0u);  // 0x005abe30
        joan::orig::as_unit(this)->play_named_sound(jo::as_item_def(t)->anim_names[0])
;  // 0x0049b4d0
        return 1;
    }

    // ------------------------------------------------------------------ 0x00434270
    void update_a2_434270() {
        c_update_a2.hit();
        if (static_cast<i32>(global<i8>(d::g_a5802d)) != 0) {
            jo::view<u32>(jo::self<jo::Player>(this)->attack_target_id) = 0;
            return;
        }
        if (entity_missing(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)) != 0) return;
        u8* const e = entity(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        const i32 k = static_cast<i32>(jo::as_unit(e)->anim_kind);
        if (!(k == 0x1c || k == 0x1e || k == 0x49 || k == 0x53)) {
            jo::view<u32>(jo::self<jo::Player>(this)->attack_target_id) = 0;
            return;
        }
        const i32 m = static_cast<i32>(jo::as_unit(e)->prev_action);
        if (m == 0x1c || m == 0x1e || m == 0x49 || m == 0x53) return;
        jo::view<u32>(jo::self<jo::Player>(this)->attack_target_id) = joan::orig::arg::as<u32>(joan::orig::as_cmd(this)->pick_target());  // 0x00433c20
        if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_player(this)->target_close())) != 0) {  // 0x00433f40
            joan::orig::as_player(this)->face_target();  // 0x00434080
        }
    }

    // ------------------------------------------------------------------ 0x00437b60
    i16 launch_437b60(i32 a8, i32 a12, float a16, float a20) {  // a16 / a20: floats (XMM registers on x64; the facade declares them float)
        c_launch.hit();
        if (entity_missing(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)) != 0) return 0;
        u8* const e = entity(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        u32 tmp[10];  // [ebp-0x30], 0x28 bytes written by 0x00423fe0
        {
            const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(kFollow)->target());               // 0x00563dc0
            void* const o = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kFollow)->motion(joan::orig::arg::i32(idx)));        // 0x00563c80
            joan::orig::as_camera_motion(o)->end(joan::orig::arg::mptr(static_cast<void*>(tmp)));          // 0x00423fe0
        }
        if (a8 != 0 && static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(e)->can_act())) != 0 &&  // 0x00500ba0
            static_cast<i32>(jo::as_unit(e)->action) != 1 && static_cast<i32>(jo::as_unit(e)->anim_kind) != 5 &&
            static_cast<i32>(jo::as_unit(e)->anim_kind) != 6) {
            u32 angle = 0;  // one-byte angle object at [ebp-0x34]
            joan::orig::as_angle(&angle)->construct();                                     // 0x00576120
            const float h = joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(joan::orig::arg::mptr(static_cast<void*>(tmp)), a16));  // 0x0042fe30
            joan::orig::as_angle(&angle)->set_byte(joan::orig::arg::f32(h));                                         // 0x005762c0
            i32 n = 0x578;
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_players(kMap)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->record))));  // 0x006af310
            if (p != nullptr && jo::as_shot(p)->state.sub != nullptr && jo::as_shot(p)->state.zone != 0) {
                const float ca = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos()) * global<float>(d::flt_1);  // 0x00576ac0
                const float sa = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin()) * global<float>(d::flt_1);  // 0x005768d0
                const float zero = 0.0f;
                const u32 pz = jo::view<u32[3]>(jo::as_unit(e)->position_)[2], py = jo::view<u32[3]>(jo::as_unit(e)->position_)[1], px = jo::view<u32[3]>(jo::as_unit(e)->position_)[0];
                const u32 side = joan::orig::arg::as<u32>(joan::orig::as_body(p)->side());  // 0x006b1dd0 (passed on; point_to_cell ignores it)
                float out[3] = {0.0f, 0.0f, 0.0f};
                joan::orig::as_map(kMap)->point_to_cell(joan::orig::arg::mptr(static_cast<void*>(out)), joan::orig::arg::mptr(jo::as_shot(p)->state.sub), joan::orig::arg::i32(jo::as_shot(p)->state.zone), joan::orig::arg::i32(side), joan::orig::arg::f32(px), joan::orig::arg::f32(py), joan::orig::arg::i32(pz));  // 0x006ae500
                const float dot = out[0] * ca + out[1] * sa + out[2] * zero;
                // fcomp 0; test ah,1; je skip: taken when dot < 0 or unordered
                if (!(dot >= global<float>(d::k_9cf5c4))) {
                    n = ftol(Fpu{true}.mul(static_cast<double>(n), static_cast<double>(out[2])));
                }
            }
            const float s1 = static_cast<float>(static_cast<i32>(static_cast<u32>(n) * static_cast<u32>(a12)));
            jo::as_unit(e)->throw_target[0] = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos()) * s1;  // 0x00576ac0
            const float s2 = static_cast<float>(static_cast<i32>(static_cast<u32>(n) * static_cast<u32>(a12)));
            jo::as_unit(e)->throw_target[1] = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin()) * s2;  // 0x005768d0
            const i32 limit = global<i32>(d::g_jumpmaxd);
            // fild limit; fcomp a20; test ah,0x41; jne: limit < a20, equal or unordered -> (float)limit
            u32 hbits;
            if (static_cast<double>(limit) > static_cast<double>(a20)) {
                hbits = as_bits(a20);
            } else {
                hbits = as_bits(static_cast<float>(limit));
            }
            const Fpu fpu{true};
            const double c = global<float>(d::flt_5000);
            const double v = fpu.add(fpu.div(fpu.mul(c, as_float(hbits)), static_cast<double>(global<i32>(d::g_jumpmaxd))), c);
            jo::as_unit(e)->throw_target[2] = static_cast<float>(v);
            Vec3 pos;
            std::memcpy(&pos, jo::as_unit(e)->throw_target, 12);
            joan::orig::as_unit(e)->set_view(joan::orig::arg::u32(pos.x), joan::orig::arg::u32(pos.y), joan::orig::arg::u32(pos.z));  // 0x00505cc0
            joan::orig::as_unit(e)->set_action(0x1a, 0, 1);        // 0x00501190
            joan::orig::as_critical_hud(kSound)->input(static_cast<std::int8_t>(joan::orig::arg::i32(0x44)), static_cast<std::int8_t>(joan::orig::arg::i32(0)));      // 0x00462810
            jo::self<jo::Player>(this)->u32_a8 = 0;
            return 1;
        }
        if (a8 != 0) jo::view<u32>(jo::self<jo::Player>(this)->jump_hold_ms) = 0;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_01", name, addr, fn)
B("Unit::try_contact_433670", 0x00433670, &H::try_contact_433670);
B("Unit::update_a2_434270", 0x00434270, &H::update_a2_434270);
B("Unit::launch_437b60", 0x00437b60, &H::launch_437b60);
#undef B

}  // namespace game_b101_01
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

using u32 = std::uint32_t;

// eax residue of 0x00434ce0 (a constructor returning this): its only caller is the static-init stub obj_call, which drops it
u32 addr(const void* p) { return joan::call::register_low_bits(p); }
using joan::orig::helpers::fbits;

JOAN_COUNTER(c_ctor_434ce0, "Obj::ctor_434ce0");
JOAN_COUNTER(c_dir_angle, "Input::dir_angle_434f70");

struct H {
    // ---------------------------------------------------------------- 0x00431520 / 0x004338c0 / 0x00434f40
    u32 dtor_pair() {
        joan::orig::as_player(this)->update_camera();             // 0x004384e0
        joan::orig::as_unit(this)->update_a2_434270();            // 0x00434270
        return 0;  // void callee; both callers (mission.cpp tick_mode0 / tick_mode1) discard the result
    }

    // ---------------------------------------------------------------- 0x00434ce0
    u32 ctor_434ce0() {
        c_ctor_434ce0.hit();
        joan::orig::as_ctl2d(this)->construct();  // 0x00628a30
        // button0 .. button12 and img_main .. img_joan are consecutive
        for (i32 k = 0; k < 13; ++k) joan::orig::as_widget(reinterpret_cast<u8*>(&jo::self<jo::MainMenu>(this)->button0) + k * static_cast<i32>(sizeof(jo::Button)))->ctor_620ee0();    // 0x00620ee0
        for (i32 k = 0; k < 13; ++k) joan::orig::as_image2d(reinterpret_cast<u8*>(&jo::self<jo::MainMenu>(this)->img_main) + k * static_cast<i32>(sizeof(jo::Image2D)))->construct();   // 0x00629400
        jo::self<jo::MainMenu>(this)->vtable = img::obj(d::vt_MainMenu);
        return addr(this);
    }

    // ---------------------------------------------------------------- 0x00434f70
    float dir_angle_434f70() {
        c_dir_angle.hit();
        const u32 k = global<u16>(d::g_rts_scroll_bits);
        u32 bits;
        if ((k & 1) && (k & 4)) bits = 0x43070000u;       // 135
        else if ((k & 2) && (k & 4)) bits = 0x42340000u;  // 45
        else if (k & 4) bits = 0x42b40000u;               // 90
        else if ((k & 1) && (k & 8)) bits = 0x43610000u;  // 225
        else if ((k & 2) && (k & 8)) bits = 0x439d8000u;  // 315
        else if (k & 1) bits = 0x43340000u;               // 180
        else if (k & 2) bits = 0x43b40000u;               // 360
        else if (k & 8) bits = 0x43870000u;               // 270
        else bits = 0xbf800000u;                          // -1
        return fbits(bits);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Obj::dtor_4338c0", 0x004338c0, &H::dtor_pair);
B("Obj::ctor_434ce0", 0x00434ce0, &H::ctor_434ce0);
B("Obj::dtor_434f40", 0x00434f40, &H::dtor_pair);
B("Input::dir_angle_434f70", 0x00434f70, &H::dir_angle_434f70);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    void update_4338f0() {  // 0x004338f0
        if (jo::self<jo::Player>(this)->riding != 0) joan::orig::as_player(this)->archer_icon_update();  // 0x0042a8a0
    }

    void empty() {}
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("Obj4338f0::update", 0x004338f0, &H::update_4338f0);
B82("empty_00434f20", 0x00434f20, &H::empty);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b115_08.cpp (fix/uninit)
namespace {
namespace game_b115_08 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;

JOAN_COUNTER(c_level, "Joystick::deflection_level_433930");

struct H {
    // ------------------------------------------------------------------ 0x00433930 (fix)
    i32 deflection_level() {
        c_level.hit();
        void* const js = obj(d::g_keys_a51806);
        i32 min0 = 0, max0 = 0, min1 = 0, max1 = 0;
        const i32 ok0 = joan::orig::arg::as<i32>(joan::orig::as_pad(js)->axis_range(0, &min0, &max0));  // 0x00597e00
        const i32 ok1 = joan::orig::arg::as<i32>(joan::orig::as_pad(js)->axis_range(1, &min1, &max1));
        const i32 mid0 = wadd(max0, min0) / 2;
        const i32 mid1 = wadd(max1, min1) / 2;
        const i32 pos0 = joan::orig::arg::as<i32>(joan::orig::as_pad(js)->axis(0));  // 0x00597c80
        const i32 pos1 = joan::orig::arg::as<i32>(joan::orig::as_pad(js)->axis(1));
        const i32 range0 = wsub(max0, min0);
        const i32 range1 = wsub(max1, min1);
        const i32 a0 = static_cast<i32>(static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(wsub(pos0, mid0))))) << 4);  // abs
        // fix/uninit: unmapped axis (outs never written) -> 0 instead of dividing by garbage
        const i32 q0 = ok0 != 0 ? a0 / range0 : 0;
        const i32 a1 = static_cast<i32>(static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(wsub(pos1, mid1))))) << 4);
        const i32 q1 = ok1 != 0 ? a1 / range1 : 0;
        const i32 m = q0 > q1 ? q0 : q1;
        if (m >= 5) return 3;
        if (m >= 3) return 2;
        if (m >= 2) return 1;
        return 0;
    }
};

#define FIX(name, address, fn) JOAN_REPLACE("fix/uninit", name, address, fn)
// fix/uninit: unmapped joystick axis (0x00597e00 outs unwritten) contributes 0 instead of garbage
FIX("Joystick::deflection_level", 0x00433930, &H::deflection_level);
#undef FIX

}  // namespace game_b115_08
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using u32 = std::uint32_t;

struct RV3 {  // 12-byte vector copied as raw dwords
    u32 x, y, z;
};
static_assert(sizeof(RV3) == 12, "RV3");

JOAN_COUNTER(c_aim_player, "Unit::aim_at_player");

struct H {
    // ------------------------------------------------------------------ 0x00433a90
    i32 before_now(void* /*unused*/) { return jo::self<jo::Ai>(this)->current.expire_time < global<u32>(d::g_now) ? 1 : 0; }

    // ------------------------------------------------------------------ 0x004360d0
    void aim_at_player(i32 id) {
        c_aim_player.hit();
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(id)));  // 0x006af310
        if (p == nullptr) {
            jo::view<u32>(jo::self<jo::Unit>(this)->aim_heading) = jo::view<u32>(jo::self<jo::Unit>(this)->heading);
            return;
        }
        RV3 tmp;  // ebp-0x2c
        RV3 v = *joan::orig::arg::as<RV3*>(joan::orig::vec_scale(&tmp, joan::orig::arg::ptr(&jo::as_shot(p)->state.pos_x), joan::orig::arg::f32(0x42386666u)));  // 0x0060cd00 (v * 46.1f): the record's +0x1f6 = Shot::state.pos_x..z (the 0x006af310 list holds Shot records; read as a Player blob before, 4 bytes off on x64)
        const double base = static_cast<double>(jo::self<jo::Unit>(this)->heading);               // ebp-0x34
        const double r = joan::orig::arg::as<double>(joan::orig::as_unit(this)->heading_to(&v));              // 0x00505a00 (st0)
        const joan::hook::x87::Fpu fpu{true};
        double t = fpu.mul(r, 3.141592653589793);  // qword 0x9cfbf8
        t = fpu.div(t, 180.0);                    // qword 0x9cfbe8
        t = fpu.sub(base, t);                     // fsubr
        jo::self<jo::Unit>(this)->aim_heading = static_cast<float>(t);
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::before_now", 0x00433a90, &H::before_now);
B98("Unit::aim_at_player", 0x004360d0, &H::aim_at_player);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

// g_out = g_in / g_div (fld dword / fdiv dword / fstp dword).
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a3cb7c() { div_global(d::g_a3cb7c, d::g_pi_a3cb6c, d::flt_4); }  // 0x00433ac0

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("init_a3cb7c", 0x00433ac0, &init_a3cb7c);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b111_19.cpp (fix/uninit)
namespace {
namespace game_b111_19 {

using u32 = std::uint32_t;

using joan::orig::helpers::gf;  // float constant in .rdata
using joan::orig::helpers::wrap_add;
using joan::orig::helpers::wrap_sub;

JOAN_COUNTER(c_stick, "Pad::stick_angle_433af0 (fix/uninit)");

// ---- fix/uninit: 0x00433af0 ------------------------------------------------------------------
struct H {
    float stick_angle() {
        c_stick.hit();
        void* const js = obj(d::g_keys_a51806);
        // fix/uninit: 0x00597e00 writes these only when the axis is mapped (byte +0xb2 / +0xb3
        // of *0xa51806 != 0); the original then averages stack garbage. 0 = unmapped axis
        // centred (0x00597c80 returns 0 for it).
        i32 min0 = 0, max0 = 0, min1 = 0, max1 = 0;  // [ebp-0xc] / [ebp-0x8], [ebp-0x14] / [ebp-0x10]
        joan::orig::as_pad(js)->axis_range(i32{0}, &min0, &max0);  // 0x00597e00
        joan::orig::as_pad(js)->axis_range(i32{1}, &min1, &max1);
        const i32 c0 = wrap_add(max0, min0) / 2;  // cdq; sub; sar 1
        const i32 c1 = wrap_add(max1, min1) / 2;
        const i32 v0 = joan::orig::arg::as<i32>(joan::orig::as_pad(js)->axis(i32{0}));  // 0x00597c80
        const i32 v1 = joan::orig::arg::as<i32>(joan::orig::as_pad(js)->axis(i32{1}));
        const i32 dx = wrap_sub(v0, c0);
        const i32 dy = wrap_sub(v1, c1);
        if (dx == 0 && dy < 0) return gf(d::flt_90);  // 90
        if (dx == 0 && dy > 0) return gf(d::flt_270);  // 270
        if (dx == 0 && dy == 0) return gf(d::flt_m1);  // -1
        u32 angle = 0;  // [ebp-0x34]: 1-byte angle object
        joan::orig::as_angle(&angle)->construct();  // 0x00576120 (empty ctor)
        const float fdy = static_cast<float>(wrap_sub(0, dy));  // fild; fstp dword
        const float fdx = static_cast<float>(dx);
        joan::orig::as_angle(&angle)->from_atan2(joan::orig::arg::f32(fdx), joan::orig::arg::f32(fdy));  // 0x00576da0 (atan2 -> byte)
        return joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->byte_to_float());     // 0x00576300 (byte * k at PC24, st0)
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: 0x00433af0 - unmapped joystick axis: min / max out-params start at 0 (centred).
F("Pad::stick_angle_433af0", 0x00433af0, &H::stick_angle);
#undef F

}  // namespace game_b111_19
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::call::cdecl_call;

void* const kUnitTable = obj(d::g_units);

// ---------------------------------------------------------------------- static initialisers
void init_pi(img::Addr g) {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(g) = static_cast<float>(t * 4.0);        // qword 0x9cf788 = 4.0 (exact)
}

using joan::orig::helpers::at;

using u32 = std::uint32_t;

struct RV3 {  // 12-byte vector copied as raw dwords
    u32 x, y, z;
};
static_assert(sizeof(RV3) == 12, "RV3");

JOAN_COUNTER(c_pick_target, "Cmd::pick_target");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
RV3 vsub(const void* a, const void* b) {  // 0x0060cc90 (a - b), result copied out of the temp
    RV3 tmp;
    return *joan::orig::arg::as<RV3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}
i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

static void JOAN_CDECL init_pi_a336fc() { init_pi(d::g_a336fc); }  // 0x004389a0

struct H {
    // ------------------------------------------------------------------ 0x00433c20
    i32 pick_target() {
        c_pick_target.hit();
        if (unit_missing(jo::self<jo::Player>(this)->attack_target_id) == 0) return jo::self<jo::Player>(this)->attack_target_id;
        if (jo::view<i8>(jo::self<jo::Player>(this)->mounted) != 0 || jo::view<i8>(jo::self<jo::Player>(this)->riding) != 0) return 0;
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        u8* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        if (jo::as_unit(unit)->actor == nullptr) return 0;
        i32 best_id = 0;           // ebp-0xc
        float best = 99999.0f;     // ebp-0x10 (0x47c34f80)
        joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::view<u8*>(jo::as_unit(unit)->actor))->bone))->update();  // 0x0060ee60
        RV3 dir;  // ebp-0x1c
        dir.x = jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::as_unit(unit)->actor))->bone->local[0]);
        dir.y = jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::as_unit(unit)->actor))->bone->local[1]);
        dir.z = jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::as_unit(unit)->actor))->bone->local[2]);
        RV3 pos = global<RV3>(d::g_pos);  // ebp-0x28
        joan::orig::as_unit(unit)->position(&pos);   // 0x004fedb0
        for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i > 0; --i) {  // 0x00417ad0
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (o == nullptr || i == jo::self<jo::Player>(this)->unit_id_) continue;
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(o)->hostile_to(joan::orig::arg::i16(sx16(jo::view<u16[2]>(jo::self<jo::Player>(this)->unit_id_)[0])))) == 0) continue;  // 0x005094d0
            if (jo::as_unit(o)->side == 0) continue;
            RV3 p;  // ebp-0x3c
            joan::orig::as_unit(o)->position(&p);  // 0x004fedb0
            RV3 w = vsub(&p, &pos);       // ebp-0x48
            w.z = 0;
            const float ls = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<void*>(&w)));  // length_sq
            if (ls > global<float>(d::flt_22500)) continue;                              // 22500.0f
            RV3 n;  // ebp-0x58
            import_call<void*>(d::iat_v3d_87_vector_normalize_2, static_cast<void*>(&n), static_cast<void*>(&w));  // normalize(out, in)
            // raw st0 compared (no float store): taken as double so no denormal flush to 0
            if (!(import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(&dir), static_cast<void*>(&n)) >
                  static_cast<double>(global<float>(d::k_9cf5c4))))  // dot > 0.0f
                continue;
            const float t = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(&w), static_cast<void*>(&dir)));
            const float t2 = t * t;
            const float d = joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(ls - t2)));  // 0x004256d0 sqrt (stored)
            if (!(d >= best)) {  // d < best or unordered
                best_id = i;
                best = d;
            }
        }
        return best_id;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Cmd::pick_target", 0x00433c20, &H::pick_target);
B97("init_pi_a336fc", 0x004389a0, &init_pi_a336fc);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b87_05.cpp (game/b87_05)
namespace {
namespace game_b87_05 {

using joan::img::str;

void* const kCams = obj(d::g_camera_man);

// eax residue of Player::update_camera (0x004384e0) and Player::set_follow (0x00436190): every caller drops the result
// (ctrl, playerctl, rts, esfuncs, camcmd; uinput's tick_mode2 -> mission.cpp discards it)
std::uint32_t u32(const void* p) { return joan::call::register_low_bits(p); }

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

void* const kGameUnits = obj(d::g_units);  // 0x00428b70 missing / 0x0041b3c0 get

float deg() { return global<float>(d::flt_0_017453292); }  // 0.0174533f

std::uint32_t unit_missing(std::int32_t id) { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id))); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
void* camera(std::int32_t i) { return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCams)->motion(joan::orig::arg::i32(i))); }              // 0x00563c80

JOAN_COUNTER(c_update_camera, "Player::update_camera");
JOAN_COUNTER(c_set_follow, "Player::set_follow");
JOAN_COUNTER(c_set_owner, "Place::set_owner");
JOAN_COUNTER(c_append, "Selection::append");
JOAN_COUNTER(c_group_init, "Group::init");

struct H {
    // ------------------------------------------------------------ 0x004384e0
    std::uint32_t update_camera() {
        c_update_camera.hit();
        const std::uint32_t missing = unit_missing(jo::self<jo::Player>(this)->unit_id_);
        if (missing != 0) return missing;
        std::uint8_t* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);  // ebp-0x8
        if (global<std::int32_t>(d::g_a48cef) != 0) return u32(unit);
        std::uint32_t cam[10] = {};                           // ebp-0x30 (heading = cam[4], ebp-0x20)
        joan::orig::as_camera_motion(camera(0))->end(&cam[0]);  // 0x00423fe0
        if (jo::self<jo::Player>(this)->mode == 3 && static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->mounted)) != 0 &&
            static_cast<std::int32_t>(global<std::int8_t>(d::g_a5802c)) == 1) {
            cam[4] = jo::view<std::uint32_t>(jo::as_unit(unit)->heading);
            if (jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->camera_zones) != nullptr) {
                const std::uint32_t r = joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->camera_zones))->zone_free());  // 0x00427d10 (ax)
                if (static_cast<std::int16_t>(r) == 0) joan::orig::as_camera_motion(camera(0))->retarget(&cam[0]);  // 0x00423f40
            }
            const std::int32_t state = static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->follow_mode));
            if (state == 1) jo::self<jo::Player>(this)->follow_mode = 0;
            return static_cast<std::uint32_t>(state);
        }
        std::uint32_t a34 = 0, a38 = 0, a3c = 0;  // ebp-0x34 / -0x38 / -0x3c (angle objects)
        joan::orig::as_angle(&a34)->construct_577190();       // 0x00577190
        joan::orig::as_angle(&a38)->construct_577190();
        joan::orig::as_angle(&a3c)->construct_577190();
        float heading;
        std::memcpy(&heading, &cam[4], 4);
        joan::orig::as_angle(&a34)->set_degrees(joan::orig::arg::f32(heading / deg()));       // 0x005773e0
        joan::orig::as_angle(&a38)->set_degrees(joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Player>(this)->follow_param)));  // float bits
        std::uint32_t eax = thiscall<std::uint32_t>(fn::Angle_set_degrees, &a3c, 40.0f);  // 0x42200000: set_degrees takes a float
        if (static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->follow_mode)) == 0) return eax;
        const std::uint8_t reached = joan::orig::arg::as<std::uint8_t>(joan::orig::as_angle(&a34)->turn_toward(joan::orig::arg::u32(a38), joan::orig::arg::u32(a3c)));  // 0x00577a20 (ebp-0x40)
        const float turned = joan::orig::arg::as<float>(joan::orig::as_angle(&a34)->degrees()) * deg();                   // 0x00577450
        std::memcpy(&cam[4], &turned, 4);
        void* const link = jo::as_unit(unit)->cell;
        eax = u32(link);
        if (jo::view<void*>(jo::as_cell(link)->camera_zones) != nullptr) {
            const std::uint32_t r = joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->camera_zones))->zone_free());  // 0x00427d10
            eax = static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(r)));
            if (eax == 0) eax = joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera_motion(camera(0))->retarget(&cam[0]));  // 0x00423f40
        }
        if (reached != 0) {
            eax = u32(this);
            if (static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->follow_mode)) == 1) jo::self<jo::Player>(this)->follow_mode = 0;
        }
        return eax;
    }

    // ------------------------------------------------------------ 0x00436190 (ret 4)
    std::uint32_t set_follow(std::int32_t on) {
        c_set_follow.hit();
        if (on == 0) {
            const std::int32_t state = static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->follow_mode));
            if (state == 2) jo::self<jo::Player>(this)->follow_mode = 1;
            return static_cast<std::uint32_t>(state);
        }
        const std::uint32_t missing = unit_missing(jo::self<jo::Player>(this)->unit_id_);
        if (missing != 0) return missing;
        std::uint8_t* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        const float heading = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading()) / deg();  // 0x004ff850
        if (static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->follow_mode)) == 0 || static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->follow_mode)) == 1)
            jo::self<jo::Player>(this)->follow_mode = 2;
        jo::self<jo::Player>(this)->follow_param = heading;
        return u32(this);
    }

    // ------------------------------------------------------------ 0x004366f0 (ret 4)
    void* set_owner(jo::World* owner) {
        c_set_owner.hit();
        if (jo::self<jo::Place>(this)->world == owner) return this;
        if (jo::view<void*>(jo::self<jo::Place>(this)->cell) != nullptr) joan::orig::as_place(jo::view<void*>(jo::self<jo::Place>(this)->cell))->unlink_owner(joan::orig::arg::mptr(this));  // 0x0043edb0
        if (jo::view<void*>(jo::self<jo::Place>(this)->model) != nullptr && jo::view<void*>(jo::self<jo::Place>(this)->model->scene) != nullptr)
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::Place>(this)->model->scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Place>(this)->model)));  // 0x005efdb0
        jo::self<jo::Place>(this)->world = owner;
        return this;

    }

    // ------------------------------------------------------------ 0x00438420 (ret 8)
    void* append(const Vec3* pos, std::uint32_t value) {
        c_append.hit();
        if (!(jo::self<jo::WaypointList>(this)->count < 0xd)) return this;
        float* const slot = &jo::self<jo::WaypointList>(this)->positions[jo::self<jo::WaypointList>(this)->count * 3];
        jo::view<std::uint32_t>(slot[0]) = jo::view<std::uint32_t>(pos->x);
        jo::view<std::uint32_t>(slot[1]) = jo::view<std::uint32_t>(pos->y);
        jo::view<std::uint32_t>(slot[2]) = jo::view<std::uint32_t>(pos->z);
        jo::self<jo::WaypointList>(this)->values[jo::self<jo::WaypointList>(this)->count] = value;
        jo::self<jo::WaypointList>(this)->count = jo::self<jo::WaypointList>(this)->count + 1;
        return this;
    }

    // ------------------------------------------------------------ 0x00433ea0 (ret 0xc)
    std::uint32_t group_init(std::int16_t id, std::int8_t kind, const char* name) {
        c_group_init.hit();
        jo::self<jo::Place>(this)->id = id;
        jo::self<jo::Place>(this)->kind = kind;
        // strncpy without a terminator into the fixed-size field, as the original does
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(&jo::self<jo::Place>(this)->name[0])), joan::orig::arg::str(name), 0x20u);  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        jo::self<jo::Place>(this)->name_end = 0;
        joan::orig::as_unit(this)->set_52(0x190);                                 // 0x00469330
        joan::orig::as_unit(this)->set_56(0x190);                                 // 0x00469380
        jo::view<std::int32_t>(jo::self<jo::Place>(this)->timer) = 0;
        jo::self<jo::Place>(this)->spawn_count = 0;
        return 0;  // void callee; the one caller (world_s.cpp, 0x00433ea0) discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_05", name, addr, fn)
B("Player::update_camera", 0x004384e0, &H::update_camera);
B("Player::set_follow", 0x00436190, &H::set_follow);
B("Place::set_owner", 0x004366f0, &H::set_owner);
B("Selection::append", 0x00438420, &H::append);
B("Group::init", 0x00433ea0, &H::group_init);
#undef B

}  // namespace game_b87_05
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_units_close, "Pair::units_close");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

void JOAN_CDECL init_a33700() { global<float>(d::g_a33700) = global<float>(d::g_a336fc) / global<float>(d::flt_2); }

struct H {
    // ------------------------------------------------------------------ 0x00433f40
    i16 units_close() {
        c_units_close.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        if (unit_missing(jo::self<jo::Player>(this)->attack_target_id) != 0) return 0;
        u8* const a = unit_get(jo::self<jo::Player>(this)->unit_id_);
        u8* const b = unit_get(jo::self<jo::Player>(this)->attack_target_id);
        V3 pa;
        V3 pb;
        joan::orig::as_unit(a)->position(joan::orig::arg::mptr(static_cast<void*>(&pa)));  // 0x004fedb0
        joan::orig::as_unit(b)->position(joan::orig::arg::mptr(static_cast<void*>(&pb)));
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(b)->dead()) != 0) return 0;  // 0x0042d0e0
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(a)->dead()) != 0) return 0;
        V3 tmp;
        const V3* const d = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(&pb), static_cast<void*>(&pa)));  // 0x0060cc90
        V3 diff = *d;
        const double len = import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&diff));
        if (len > static_cast<double>(global<float>(d::flt_9cfbe0))) return 0;  // > 150
        return 1;                                                            // <= 150 or NaN
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Pair::units_close", 0x00433f40, &H::units_close);
B95("init_a33700", 0x00438a00, &init_a33700);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset
void stop_unit(void* unit) { joan::orig::as_unit(unit)->play_idle(0, 1); }      // 0x00508b90

// Order constructors: base 0x00416a50, vtable, memset(this + 0xc2, 0, n).
using joan::orig::helpers::order_ctor;

struct H {
    void* order_434040() { return order_ctor(this, d::vt_Obj_9d3bec, static_cast<u32>(sizeof(jo::Order45) - sizeof(jo::MapObject))); }  // 0x45 on x86
    void* order_434380() { return order_ctor(this, d::vt_Obj_9d3ab4, static_cast<u32>(sizeof(jo::Order0c_a) - sizeof(jo::MapObject))); }  // 0xc on x86
    void* order_4352b0() { return order_ctor(this, d::vt_Obj_9d3b50, static_cast<u32>(sizeof(jo::Order0c_b) - sizeof(jo::MapObject))); }  // 0xc on x86

    // ------------------------------------------------------------------------ 0x00434210
    void stop_unit_order(void* unit) {
        stop_unit(unit);
        jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Order45::ctor", 0x00434040, &H::order_434040);
B93("Order::stop_unit", 0x00434210, &H::stop_unit_order);
B93("Order0c_a::ctor", 0x00434380, &H::order_434380);
B93("Order0c_b::ctor", 0x004352b0, &H::order_4352b0);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

using joan::call::cdecl_call;

using joan::orig::helpers::at;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kUnits = obj(d::g_units);

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
void unit_pos(void* u, Vec3* out) { joan::orig::as_unit(u)->position(joan::orig::arg::mptr(out)); }                               // 0x004fedb0

JOAN_COUNTER(c_face, "Face::update");

// ---- static initialisers / free functions ----
using joan::orig::helpers::div_init;
void JOAN_CDECL init_438aa0() { div_init(d::g_a33708, d::g_a336fc, d::flt_4); }

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // 0x00434080
    void face_update() {
        c_face.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return;
        if (unit_missing(jo::self<jo::Player>(this)->attack_target_id) != 0) return;
        std::uint8_t* const a = unit_get(jo::self<jo::Player>(this)->unit_id_);     // ebp-0x8
        std::uint8_t* const b = unit_get(jo::self<jo::Player>(this)->attack_target_id);  // ebp-0xc
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(b)->dead()) != 0) return;   // 0x0042d0e0
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(a)->dead()) != 0) return;
        Vec3 pa, pb, tmp;  // ebp-0x18, ebp-0x24, ebp-0x74
        unit_pos(a, &pa);
        unit_pos(b, &pb);
        const Vec3* const diff = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &pb, &pa));  // 0x0060cc90
        Vec3 d = *diff;                                                         // ebp-0x68
        const Vec3* const nn = import_call<Vec3*>(d::iat_v3d_87_vector_normalize_1, &d);              // normalize in place
        const Vec3 n = *nn;                                                     // ebp-0x30
        const std::int32_t heading = ftol(joan::orig::arg::as<float>(joan::orig::as_unit(a)->float_1e()));     // 0x0050ac00 (fld dword)
        const Fpu fpu{true};
        const double ang = joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(n.y), joan::orig::arg::f32(n.x)));  // 0x00421020 atan2
        const float deg = static_cast<float>(fpu.div(ang, global<float>(d::flt_0_017453292)));
        joan::orig::as_unit(a)->set_heading(joan::orig::arg::f32(deg));  // 0x0050abd0
        if (jo::self<jo::Player>(this)->mode != 3) return;
        void* const cams = obj(d::g_camera_man);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera_follow(cams)->target()) != 0) return;  // 0x00563dc0
        jo::CamParams buf;                                          // ebp-0x5c (filled by 0x00423fe0)
        const std::int32_t idx = joan::orig::arg::as<std::int32_t>(joan::orig::as_camera_follow(cams)->target());
        void* const cam = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(cams)->motion(joan::orig::arg::i32(idx)));  // 0x00563c80
        joan::orig::as_camera_motion(cam)->end(joan::orig::arg::mptr(&buf));                            // 0x00423fe0
        const float h = joan::orig::arg::as<float>(joan::orig::as_unit(a)->float_1e());
        double v = fpu.sub(h, static_cast<double>(heading));
        v = fpu.mul(v, global<float>(d::flt_0_017453292));
        v = fpu.add(v, buf.orbit[0]);
        buf.orbit[0] = static_cast<float>(v);
        void* const cam0 = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(cams)->motion(0));
        joan::orig::as_camera_motion(cam0)->retarget(joan::orig::arg::mptr(&buf));  // 0x00423f40
    }

    // 0x004384a0
    Vec3* pos_4384a0(Vec3* out) {
        std::memcpy(out, jo::self<jo::StreamSlot>(this)->pos, 12);  // +0xc on x86: behind the StreamSlot's source pointer (the SoundSlot view read +0xc on x64 too)
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("Face::update", 0x00434080, &H::face_update);
B("Obj::pos_4384a0", 0x004384a0, &H::pos_4384a0);
B("init_438aa0", 0x00438aa0, &init_438aa0);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b109_01.cpp (game/b109_01)
namespace {
namespace game_b109_01 {

void* const kKeys = obj(d::g_keys_a48820);

using u32 = std::uint32_t;

JOAN_COUNTER(c_input, "Player::handle_input_4343e0");

void* const kUnits = obj(d::g_units);
i32 key_down(i32 k) { return joan::orig::arg::as<i32>(joan::orig::as_race_res(kKeys)->test(0, joan::orig::arg::i32(k))); }  // 0x004cb820
i32 key_hit(i32 k) { return joan::orig::arg::as<i32>(joan::orig::as_bindings(kKeys)->state(0, joan::orig::arg::u32(k))); }   // 0x004cb6d0
void* unit(u32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 unit_missing(u32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70

struct H {
    // -------------------------------------------------------------- 0x004343e0
    u16 handle_input() {
        c_input.hit();
        i32 v8 = 0, vc = 0, v10 = 0, v14 = 0, v18 = 0, v1c = 0, v20 = 0;
        if (key_down(0x12) != 0) v8 = 1;
        if (key_down(0xd) != 0) v10 = 1;
        if (key_down(0xc) != 0) v14 = 1;
        if (key_hit(0xc) != 0) v14 = 1;
        if (key_down(0xe) != 0) v18 = 1;
        if (key_down(0xf) != 0) v20 = 1;
        if (key_hit(3) != 0) vc = 1;
        i8 v28 = 0;
        if (key_down(0x30) != 0) v28 = 1;
        if (key_down(0x31) != 0) v28 = 2;
        float v2c = 0.0f;  // uninitialised in the original unless v1c is set; only read then
        if (key_hit(0x10) != 0) {
            jo::self<jo::Player>(this)->jump_hold_ms = static_cast<i32>(jo::view<u32>(jo::self<jo::Player>(this)->jump_hold_ms) + global<u32>(d::g_frame_step_ms));
            if (!(jo::self<jo::Player>(this)->jump_hold_ms < global<i32>(d::g_jumpmaxd))) {
                v1c = 1;
                v2c = static_cast<float>(jo::self<jo::Player>(this)->jump_hold_ms);
                jo::self<jo::Player>(this)->jump_hold_ms = 0;
            }
        } else if (jo::self<jo::Player>(this)->jump_hold_ms > 0) {
            v1c = 1;
            v2c = static_cast<float>(jo::self<jo::Player>(this)->jump_hold_ms);
            jo::self<jo::Player>(this)->jump_hold_ms = 0;
        }
        const i32 v30 = key_hit(4);
        const i32 v34 = key_hit(5);
        const i32 v38 = key_hit(8);
        const i32 v3c = key_hit(9);
        i32 v40 = 0;
        if (v30 != 0 || v38 != 0 || v3c != 0) v40 = 0x25;
        else if (v34 != 0) v40 = 0x14;
        const i32 dirs = joan::orig::arg::as<i32>(joan::orig::as_player(this)->direction_angle(joan::orig::arg::i32(v30), joan::orig::arg::i32(v34), joan::orig::arg::i32(v38), joan::orig::arg::i32(v3c)));  // 0x00431570
        const float v44 = static_cast<float>(dirs);
        const i32 v48 = (v30 == 0 && v34 == 0 && v38 == 0 && v3c == 0) ? 0 : 3;
        const float v4c = joan::orig::arg::as<float>(joan::orig::as_unit(this)->link_value_of());  // 0x0042fd60
        i32 v50 = 0, v54 = 0;
        if (global<i32>(d::g_key) != 0) {
            if (key_down(4) != 0) {
                v54 = 4;
            } else if (vc != 0) {
                if (key_down(5) != 0) v54 = 5;
                else if (key_down(8) != 0) v54 = 6;
                else if (key_down(9) != 0) v54 = 7;
                else v54 = 0;
            } else {
                v54 = 0;
            }
        }
        if (jo::self<jo::Player>(this)->last_dir_key != 0 && global<u32>(d::g_now) - jo::self<jo::Player>(this)->last_dir_key_ms < 0x12c &&
            jo::self<jo::Player>(this)->last_dir_key == v54)
            v50 = jo::self<jo::Player>(this)->last_dir_key;
        if (global<i32>(d::g_key) != 0) {
            jo::self<jo::Player>(this)->last_dir_key = v54;
            jo::self<jo::Player>(this)->last_dir_key_ms = global<u32>(d::g_now);
        }
        const i32 v58 = v50 == 4 ? 1 : 0;
        const bool v5c = v50 == 5, v60 = v50 == 6, v64 = v50 == 7;
        if (key_down(0x13) != 0) joan::orig::as_order(this)->consume_43b490(0u);  // 0x0043b490
        if (key_down(0x14) != 0 && (global<u16>(d::g_key_modifiers) & 0x30) == 0) joan::orig::as_unit(this)->use_spiritual_item_43b990(0);  // 0x0043b990
        if (jo::view<i8>(jo::self<jo::Player>(this)->riding) != 0)
            return static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_player(this)->command(joan::orig::arg::u32(v8), joan::orig::arg::u32(v10), joan::orig::arg::bits(v44), joan::orig::arg::u32(v48))));  // 0x0042a810
        if (jo::view<i8>(jo::self<jo::Player>(this)->mounted) != 0) {
            joan::orig::as_camera(this)->step_height(static_cast<i32>(v28));  // 0x00438720
            const i32 k = key_down(0x11);
            return static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_player(this)->dispatch_keys(joan::orig::arg::u32(k), joan::orig::arg::u32(v10), joan::orig::arg::u32(v18), joan::orig::arg::u32(vc), joan::orig::arg::bits(v44), joan::orig::arg::u32(v40), joan::orig::arg::u32(v48))));  // 0x0043c5c0
        }
        if (jo::self<jo::Player>(this)->squad_mode != 0) {
            const i32 k = key_down(0x11);
            return static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_player(this)->squad_act(joan::orig::arg::u32(k), joan::orig::arg::u32(v10), joan::orig::arg::u32(vc), joan::orig::arg::bits(v44), joan::orig::arg::u32(v40), joan::orig::arg::u32(v48))));  // 0x0042d270
        }
        if (key_down(0x11) != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->recharge_43bef0()) == 0 &&      // 0x0043bef0
                joan::orig::arg::as<i32>(joan::orig::as_order(this)->spawn_effect()) == 0 &&      // 0x0043bd40
                joan::orig::arg::as<i32>(joan::orig::as_unit(this)->try_link_43be40()) == 0 &&      // 0x0043be40
                joan::orig::arg::as<i32>(joan::orig::as_player(this)->link_nearest_enemy()) == 0 &&      // 0x0043c210
                joan::orig::arg::as<i32>(joan::orig::as_order(this)->fire_43bc50()) == 0) {      // 0x0043bc50
                joan::orig::as_unit(this)->start_6c0310_43bfe0();             // 0x0043bfe0
                joan::orig::as_order(this)->alencon_43c080();             // 0x0043c080
            }
        }
        if (global<u8>(d::g_game_running) == 0) return 0;
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->start_42b3c0(joan::orig::arg::u32(v8))) != 0) return 1;  // 0x0042b3c0
        joan::orig::as_player(this)->set_follow(joan::orig::arg::i32(vc));                      // 0x00436190
        joan::orig::as_camera(this)->step_height(static_cast<i32>(v28));   // 0x00438720
        if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->stub_zero(joan::orig::arg::u32(v14))) != 0) return 1;  // 0x00436380
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->find_target_4363a0(joan::orig::arg::u32(v14))) != 0) return 1;  // 0x004363a0
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(this)->try_flee(joan::orig::arg::i32(v14))) != 0) return 1;  // 0x00436250
        if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->order_update(joan::orig::arg::i32(v10), joan::orig::arg::i32(v18), joan::orig::arg::i32(v58))) != 0) return 1;  // 0x004367c0
        if (joan::orig::arg::as<i16>(joan::orig::as_order(this)->try_437910(joan::orig::arg::u32(v20))) != 0) return 1;  // 0x00437910
        if (unit_missing(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)) == 0) {
            void* const u = unit(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
            if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_e_14()) == 0) {  // 0x005091d0
                i32 k = key_down(0x11);
                if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->try_mount(joan::orig::arg::i32(k))) != 0) return 1;  // 0x0043c7d0
                k = key_down(0x11);
                if (joan::orig::arg::as<i16>(joan::orig::as_squad(this)->try_rejoin_42d300(joan::orig::arg::i32(k))) != 0) return 1;  // 0x0042d300
                if (static_cast<u32>(v30) + static_cast<u32>(v34) + static_cast<u32>(v38) + static_cast<u32>(v3c) == 1u &&
                    joan::orig::arg::as<i16>(joan::orig::as_unit(this)->launch_437b60(joan::orig::arg::i32(v1c), joan::orig::arg::i32(v48), joan::orig::arg::f32(v44), joan::orig::arg::f32(v2c))) != 0)  // 0x00437b60
                    return 1;
            }
        }
        if (v5c) {
            combo_attack(0x15);
            return 1;
        }
        if (v64) {
            combo_attack(0x16);
            return 1;
        }
        if (v60) {
            combo_attack(0x17);
            return 1;
        }
        const i32 v78 = key_hit(3);
        if (v78 != 0 && v40 != 0) {
            joan::orig::as_player(this)->set_follow(1);                    // 0x00436190
            joan::orig::as_player(this)->order_direction(joan::orig::arg::i32(v30), joan::orig::arg::i32(v34), joan::orig::arg::i32(v38), joan::orig::arg::i32(v3c));   // 0x00438110
            return 1;
        }
        if (unit_missing(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)) == 0) {
            void* const u = unit(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
            if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_e_14()) != 0) joan::orig::as_unit(u)->play_idle(0, 1);  // 0x005091d0 / 0x00508b90
        }
        joan::orig::as_ctrl(this)->turn_4352f0(joan::orig::arg::f32(v44), joan::orig::arg::i32(v48), joan::orig::arg::f32(v4c));  // 0x004352f0
        joan::orig::as_player(this)->unit_order(joan::orig::arg::i32(v40), joan::orig::arg::i32(v78));       // 0x00437ed0
        return 0;
    }
    void combo_attack(i32 code) {
        void* const u = unit(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->can_act()) != 0 && jo::as_unit(u)->action != 1 && jo::as_unit(u)->anim_kind != 5 &&
            jo::as_unit(u)->anim_kind != 6)  // 0x00500ba0
            joan::orig::as_unit(unit(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)))->set_action(joan::orig::arg::i32(code), 0, 1);  // 0x00501190
    }

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
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_01", name, addr, fn)
B("Player::handle_input_4343e0", 0x004343e0, &H::handle_input);
#undef B

}  // namespace game_b109_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::orig::helpers::delete_;  // operator delete

// ---------------------------------------------------------------------- static initialisers
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a33704() { div_global(d::g_a33704, d::g_a336fc, d::flt_3); }  // 0x00438a50

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    H* sdtor(std::uintptr_t dtor, u32 flags) {
        thiscall(dtor, this);
        if ((flags & 1) != 0) delete_(this);
        return this;
    }
    H* sdtor_4350b0(u32 flags) { return sdtor(fn::code_value(fn::Sound3d_call_nop), flags); }  // 0x0059fe10
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::scalar_dtor_4350b0", 0x004350b0, &H::sdtor_4350b0);
B99("init_a33704", 0x00438a50, &init_a33704);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using u32 = std::uint32_t;

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_order_icon, "UnitView::order_icon");

struct H {
    // ------------------------------------------------------------------ 0x004350f0
    i32 order_icon() {
        c_order_icon.hit();
        (void)joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x00428b70
        void* const unit = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        if (jo::self<jo::Player>(this)->mode != 1) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0) return 0x46;  // 0x00430190
            if (joan::orig::arg::as<i32>(joan::orig::as_world(unit)->flag_1fd()) != 0) return 0x2f;  // 0x0042d690
            return 0xb;
        }
        const u32 kind = joan::orig::arg::as<u32>(joan::orig::as_joystick(this)->deflection_level());  // 0x00433930
        switch (kind) {
        case 0: {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0) return 0x43;
            if (joan::orig::arg::as<i32>(joan::orig::as_world(unit)->flag_1fd()) == 0) return 1;
            switch (static_cast<i32>(jo::as_unit(unit)->anim_kind) - 0x5b) {
            case 0:
            case 4:
            case 6:
                return 0x5a;
            case 2:
            case 3:
            case 8:
            case 11:
                return 0x59;
            default:
                return 1;
            }
        }
        case 1:
        case 2:
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0) return 0x44;
            if (joan::orig::arg::as<i32>(joan::orig::as_world(unit)->flag_1fd()) != 0) return 0x2f;
            return 0xa;
        case 3:
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0) return 0x46;
            if (joan::orig::arg::as<i32>(joan::orig::as_world(unit)->flag_1fd()) != 0) return 0x2f;
            return 0xb;
        default:
            return 1;
        }
    }
};

JOAN_REPLACE("game/b108_07", "UnitView::order_icon", 0x004350f0, &H::order_icon);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b118_04.cpp (game/b118_04)
namespace {
namespace game_b118_04 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00435280 Action::stop_unit
    // push 1; push 0; ecx = unit; call 0x00405c04 (-> 0x00508b90); eax = this; [eax+0x1d] = 0; ret 4.
    H* stop_unit(void* unit) {
        joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
        // mov dword [eax+0x1d], 0: `this` is the unit's Order list (Unit::orders; the dispatch table of
        // Action::dispatch 0x004bb660 calls this), +0x1d its current order's kind: no order. (x64: after the DynArray's
        // 8-byte body_buf, so the x86 offset hit the pointer's upper half: a cut pointer once the heap lay above 4 GB.)
        jo::self<jo::Order>(this)->current.kind = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00438930 Obj::set_target_438930
    // ret 0xc. movsx ecx, word [this+4]; cmp ecx, -1; jne -> eax stays this. `this` is a MouseCursor (ilib/mousecr.cpp:
    // it calls MouseCursor::set 0x00596db0 on itself): it sets the default icon and its hot spot, and shows it when no
    // shape is selected. `bitmap` arrives as the generated facade's 32-bit word (x64: zero-extended; no C++ caller).
    u32 set_target_438930(u32 bitmap, u32 hot_x, u32 hot_y) {
        jo::self<jo::MouseCursor>(this)->default_bitmap = reinterpret_cast<jo::Bitmap*>(static_cast<std::uintptr_t>(bitmap));
        jo::self<jo::MouseCursor>(this)->default_hot_x = static_cast<i32>(hot_x);
        jo::self<jo::MouseCursor>(this)->default_hot_y = static_cast<i32>(hot_y);
        if (static_cast<i32>(jo::self<jo::MouseCursor>(this)->cur_shape) == -1) return joan::orig::arg::as<u32>(joan::orig::as_cursor(this)->set_596db0(joan::orig::arg::i32(-1)));  // 0x00596db0
        return joan::call::register_low_bits(this);  // eax = this; no caller in src uses the result
    }
};

JOAN_REPLACE("game/b118_04", "Action::stop_unit", 0x00435280, &H::stop_unit);
JOAN_REPLACE("game/b118_04", "Obj::set_target_438930", 0x00438930, &H::set_target_438930);

}  // namespace game_b118_04
}  // namespace

// ---------------------------------------- from modules/game_b106_16.cpp (game/b106_16)
namespace {
namespace game_b106_16 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

float K(img::Addr a) { return global<float>(a); }    // float constant in .rdata
double KD(img::Addr a) { return global<double>(a); }  // qword constant in .rdata
u32 bits(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    return b;
}

struct View {  // 0x28-byte state copied by 0x00423fe0 / read by 0x0042fe30, 0x00423f40
    u32 d[10];
};
static_assert(sizeof(View) == 0x28, "view state");

constexpr u32 kMinusOne = 0xbf800000u;  // -1.0f, "no angle"
constexpr img::Addr kView(d::g_camera_man);

JOAN_COUNTER(c_turn_4352f0, "Ctrl::turn_4352f0");

i32 view_mode() { return joan::orig::arg::as<i32>(joan::orig::as_camera_follow(img::obj(kView))->target()); }  // 0x00563dc0

// Heading step shared by four blocks of 0x004352f0 (field +0x10 of the view state).
void heading_step(View& st, float angle) {
    if (joan::padfx::g_stick_keeps_camera) return;  // the pad's right stick has the camera (Ctrl::update_432d30)
    float& h = jo::view<float>(st.d[4]);  // CamParams::orbit[0]
    float d;
    if (!(angle >= K(d::flt_180)))  // < 180 or NaN
        d = angle - K(d::flt_90);  // - 90
    else
        d = K(d::flt_270) - angle;  // 270 -
    const Fpu fpu{true};
    const float step = static_cast<float>(
        fpu.mul(fpu.mul(static_cast<double>(global<u32>(d::g_frame_elapsed_ms)), static_cast<double>(K(d::flt_0_000034906585))),
                static_cast<double>(d)));
    h = h + step;
    if (!(h >= K(d::flt_m3_1415927))) h = h + K(d::flt_6_2831855);  // < -pi (or NaN): += 2pi
    if (h > K(d::flt_3_1415927)) h = h - K(d::flt_6_2831855);      // > pi: -= 2pi
}

struct H {
    // 0x004352f0
    void turn_4352f0(float angle, i32 mode, float limit) {
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return;  // 0x00428b70
        c_turn_4352f0.hit();
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x0041b3c0
        View st;
        {
            const i32 m = view_mode();
            void* const v = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(img::obj(kView))->motion(joan::orig::arg::i32(m)));  // 0x00563c80
            joan::orig::as_camera_motion(v)->end(&st);                           // 0x00423fe0
        }
        {
            const i32 s = jo::as_unit(unit)->anim_kind;
            if (s == 0x2b || s == 0x2c || s == 0x2d || s == 0x51 || s == 0x52 || s == 0x4f || s == 0x50) return;
        }
        auto unit_target = [&]() -> void* {
            return joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(unit)->actor)->record)));  // 0x006af310
        };
        auto busy = [&]() -> i32 {
            return joan::orig::arg::as<i16>(joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->camera_zones))->zone_free());  // 0x00427d10
        };
        auto set_heading = [&](float v) { joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(v)); };  // 0x0050abd0
        auto heading = [&]() -> float { return joan::orig::arg::as<float>(joan::orig::as_unit(unit)->float_1e()); };  // 0x0050ac00
        auto rel_angle = [&]() -> float { return joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(&st, joan::orig::arg::f32(angle))); };  // 0x0042fe30

        if (jo::as_unit(unit)->anim_kind == 0x18) {
            u32 c34 = 0;  // 1-byte angle object (ctor 0x00576120 writes nothing)
            joan::orig::as_angle(&c34)->construct();  // 0x00576120
            if (bits(angle) != kMinusOne)
                joan::orig::as_angle(&c34)->set_byte(joan::orig::arg::f32(rel_angle()));  // 0x005762c0
            else
                joan::orig::as_angle(&c34)->set_byte(joan::orig::arg::f32(0u));
            unit_target();
            if (busy() == 0 && mode != 0 && bits(angle) != kMinusOne && view_mode() != 3) {
                set_heading(joan::orig::arg::as<float>(joan::orig::as_angle(&c34)->byte_to_float()));  // 0x00576300
                if (view_mode() == 0) heading_step(st, angle);
            }
        }
        if (jo::as_unit(unit)->anim_kind == 0x1a && limit > K(d::flt_180)) {
            u32 c40 = 0;
            joan::orig::as_angle(&c40)->construct();  // 0x00576120
            if (bits(angle) != kMinusOne)
                joan::orig::as_angle(&c40)->set_byte(joan::orig::arg::f32(rel_angle()));  // 0x005762c0
            else
                joan::orig::as_angle(&c40)->set_byte(joan::orig::arg::f32(0u));
            void* const p44 = unit_target();
            if (busy() == 0 && mode != 0 && bits(angle) != kMinusOne && view_mode() != 3) {
                set_heading(joan::orig::arg::as<float>(joan::orig::as_angle(&c40)->byte_to_float()));  // 0x00576300
                if (view_mode() == 0) heading_step(st, angle);
            }
            const float s1 = static_cast<float>(static_cast<i32>(static_cast<u32>(mode) * 0x578u));
            jo::as_shot(p44)->state.move_target[0] = joan::orig::arg::as<float>(joan::orig::as_angle(&c40)->cos()) * s1;  // 0x00576ac0
            const float s2 = static_cast<float>(static_cast<i32>(static_cast<u32>(mode) * 0x578u));
            jo::as_shot(p44)->state.move_target[1] = joan::orig::arg::as<float>(joan::orig::as_angle(&c40)->sin()) * s2;  // 0x005768d0
        }
        if (mode != 0 && bits(angle) != kMinusOne && jo::as_unit(unit)->anim_kind == 0x1a) {
            set_heading(rel_angle());
            if (busy() == 0 && view_mode() == 0) heading_step(st, angle);
        }
        if (jo::as_unit(unit)->action == 1) {
            void* const p50 = unit_target();
            if (jo::as_shot(p50)->state.vec0_z > K(d::k_9cf5c4)) {
                jo::view<u16>(jo::self<jo::Player>(this)->turn) = 1;
            } else if (!(jo::as_shot(p50)->state.vec0_z > K(d::k_9cf5c4)) && jo::self<jo::Player>(this)->turn == 1 &&
                       joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->can_act()) != 0) {  // 0x00500ba0
                jo::as_unit(unit)->armour_flags[0] = 0;
                jo::as_unit(unit)->armour_flags[1] = 0;
                jo::as_unit(unit)->armour_flags[2] = 0;
                jo::as_unit(unit)->armour_flags[3] = 0;
                jo::as_unit(unit)->armour_flags[4] = 0;
                jo::as_unit(unit)->armour_flags[5] = 0;
                if (jo::as_unit(unit)->combo_state != -1) jo::view<u8>(jo::as_unit(unit)->combo_state) = 0;
                jo::view<u16>(jo::as_unit(unit)->combo_queued) = 0;
                joan::orig::as_unit(unit)->set_action(6, 0, 1);  // 0x00501190
                jo::self<jo::Player>(this)->u32_a8 = 0;
                jo::view<u16>(jo::self<jo::Player>(this)->turn) = 2;
            }
        }
        if (view_mode() == 0) {
            void* const v = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(img::obj(kView))->motion(0));  // 0x00563c80
            joan::orig::as_camera_motion(v)->retarget(&st);                           // 0x00423f40
        }
        if (jo::as_unit(unit)->anim_kind == 0x1c && mode != 0 && bits(angle) != kMinusOne) {
            u32 c54 = 0, c58 = 0, c7c = 0;
            joan::orig::as_angle(&c54)->construct();                 // 0x00576120
            joan::orig::as_angle(&c58)->construct();                 // 0x00576120
            joan::orig::as_angle(&c54)->set_byte(joan::orig::arg::f32(jo::self<jo::Player>(this)->prev_move_angle));      // 0x005762c0
            joan::orig::as_angle(&c58)->set_byte(joan::orig::arg::f32(angle));                 // 0x005762c0
            const float v5c = joan::orig::arg::as<float>(joan::orig::as_player(this)->unit_progress());  // 0x0042fdd0
            const float v60 = joan::orig::arg::as<float>(joan::orig::as_unit(this)->link_value_of());  // 0x0042fd60
            const u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_byte_angle(&c58)->diff(&c7c, &c54));  // 0x00576430
            u32 c64 = *r;
            const float v68 = joan::orig::arg::as<float>(joan::orig::as_angle(&c64)->byte_to_float());  // 0x00576300
            if (!(v68 >= K(d::flt_10))) return;  // < 10 (or NaN)
            if (v68 > K(d::flt_350)) return;      // > 350
            if (v5c - v60 > K(d::flt_50) && v60 > K(d::flt_50)) return;
            if (!(v68 >= K(d::flt_180))) {   // < 180
                if (v68 > K(d::flt_45)) angle = jo::self<jo::Player>(this)->prev_move_angle + K(d::flt_45);  // + 45
            } else if (!(v68 >= K(d::flt_315))) {  // < 315
                angle = jo::self<jo::Player>(this)->prev_move_angle - K(d::flt_45);
            }
            if (!(angle > K(d::flt_m90)))  // <= -90 (or NaN)
                angle = angle + K(d::flt_360);
            else if (!(angle < K(d::flt_450)))  // >= 450
                angle = angle - K(d::flt_360);
        }
        if (mode != 0 && bits(angle) != kMinusOne) {
            if (jo::as_unit(unit)->action != 1) {
                set_heading(rel_angle());
                if (jo::view<i8>(jo::self<jo::Player>(this)->follow_mode) == 0 && view_mode() == 0) {
                    heading_step(st, angle);
                    void* const v = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(img::obj(kView))->motion(0));  // 0x00563c80
                    joan::orig::as_camera_motion(v)->retarget(&st);                           // 0x00423f40
                }
            } else if (jo::self<jo::Player>(this)->turn == 2) {
                const float v70 = rel_angle();
                const float k30 = K(d::flt_30), k180 = K(d::flt_180), k360 = K(d::flt_360);
                if (heading() > v70) {
                    if (heading() - v70 > k180) {
                        if ((heading() - v70) + k360 > k30)
                            set_heading(heading() + k30);
                        else
                            set_heading(v70);
                    } else {
                        if (heading() - v70 > k30)
                            set_heading(heading() - k30);
                        else
                            set_heading(v70);
                    }
                } else {
                    if (v70 - heading() > k180) {
                        const float vb0 = v70 + k360;
                        if (vb0 - heading() > k30)
                            set_heading(heading() - k30);
                        else
                            set_heading(v70);
                    } else {
                        if (v70 - heading() > k30)
                            set_heading(heading() + k30);
                        else
                            set_heading(v70);
                    }
                }
                if (!(heading() >= K(d::k_9cf5c4))) {  // < 0 (or NaN)
                    set_heading(heading() + k360);
                } else if (heading() > k360) {
                    set_heading(heading() - k360);
                }
                void* const p74 = unit_target();
                const float xx = jo::view<float>(jo::as_shot(p74)->state.vec0_x) * jo::view<float>(jo::as_shot(p74)->state.vec0_x);
                const float yy = jo::as_shot(p74)->state.vec0_y * jo::as_shot(p74)->state.vec0_y;
                const float sum = xx + yy;
                const float r78 = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sum)))));  // sqrt
                const Fpu fpu{true};
                const double rad1 = fpu.div(fpu.mul(static_cast<double>(heading()), KD(d::dbl_3_141592653589793)), KD(d::dbl_9cfbe8));
                const double c = joan::orig::arg::as<double>(joan::orig::libc::cos_f(joan::orig::arg::f32(static_cast<float>(rad1))));  // 0x00425670 cos
                jo::view<float>(jo::as_shot(p74)->state.vec0_x) = static_cast<float>(fpu.mul(c, static_cast<double>(r78)));
                const double rad2 = fpu.div(fpu.mul(static_cast<double>(heading()), KD(d::dbl_3_141592653589793)), KD(d::dbl_9cfbe8));
                const double s = joan::orig::arg::as<double>(joan::orig::libc::sin_f(joan::orig::arg::f32(static_cast<float>(rad2))));  // 0x004256a0 sin
                jo::as_shot(p74)->state.vec0_y = static_cast<float>(fpu.mul(s, static_cast<double>(r78)));
                jo::view<u16>(jo::self<jo::Player>(this)->turn) = 0;
            }
        }
        if (mode != -1) jo::self<jo::Player>(this)->prev_move_angle = angle;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_16", name, addr, fn)
B("Ctrl::turn_4352f0", 0x004352f0, &H::turn_4352f0);
#undef B

}  // namespace game_b106_16
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_dtor_435ea0, "Obj::~Obj_435ea0");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x00435ea0
    void dtor_435ea0() {
        c_dtor_435ea0.hit();
        jo::self<jo::MainMenu>(this)->vtable = img::obj(d::vt_MainMenu);
        joan::orig::as_widget(this)->poll();  // 0x00628bd0
        // the 13 images (0x335b down to 0x2ac7 on x86) and the 13 buttons (0x2781 down to 0x39) in the original's order,
        // through the MainMenu's typed members (the offsets move on x64)
        jo::MainMenu* const m = jo::self<jo::MainMenu>(this);
        jo::Image2D* const images[] = {&m->img_joan, &m->img_logo, &m->img_logo_bar, &m->img_stripe, &m->img_sphere, &m->img_lea_r, &m->img_lea_l, &m->img_gold_r, &m->img_gold_l, &m->img_alab_s, &m->img_alab_r, &m->img_alab_l, &m->img_main};
        for (jo::Image2D* const w : images) joan::orig::as_widget(w)->dtor_6294c0();  // 0x006294c0
        jo::Button* const buttons[] = {&m->button12, &m->button11, &m->button10, &m->button9, &m->button8, &m->button7, &m->button6, &m->button5, &m->button4, &m->button3, &m->button2, &m->button1, &m->button0};
        for (jo::Button* const w : buttons) joan::orig::as_widget(w)->dtor_6211a0();  // 0x006211a0
        joan::orig::as_ctl2d(this)->destruct();  // 0x00628ad0
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::~Obj_435ea0", 0x00435ea0, &H::dtor_435ea0);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

using joan::orig::helpers::s16;
using joan::orig::helpers::s8;

void* const kUnits = obj(d::g_units);   // 0x00428b70 / 0x0041b3c0 / 0x0041b3f0 / 0x0051fdd0 / 0x00417ad0

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_flee, "Unit::try_flee");
JOAN_COUNTER(c_finish, "Order::finish");
JOAN_COUNTER(c_height, "Camera::step_height");

struct H {
    // ------------------------------------------------------------ 0x00436250 (ret 4)
    std::int16_t try_flee(std::int32_t mode) {
        c_flee.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        std::uint8_t* const u = unit_get(jo::self<jo::Player>(this)->unit_id_);
        if (mode == 0 && (s16(jo::view<std::uint16_t>(jo::as_unit(u)->anim_kind)) == 0x2b || s16(jo::view<std::uint16_t>(jo::as_unit(u)->anim_kind)) == 0x2d)) {
            joan::orig::as_unit(u)->play_idle(0, 1);  // 0x00508b90
            return 0;
        }
        if (mode == 0) return 0;
        if (s8(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(u)->can_act())) == 0) return 0;  // 0x00500ba0
        if (s16(jo::view<std::uint16_t>(jo::as_unit(u)->action)) == 1) return 0;
        if (s16(jo::view<std::uint16_t>(jo::as_unit(u)->anim_kind)) == 0x2b || s16(jo::view<std::uint16_t>(jo::as_unit(u)->anim_kind)) == 0x2d) return 1;
        joan::orig::as_unit(u)->set_action(0x2b, 0, 1);  // 0x00501190
        jo::self<jo::Player>(this)->u32_a8 = 0;
        joan::orig::as_critical_hud(obj(d::g_critical_hud))->input(static_cast<std::int8_t>(joan::orig::arg::i32(0x43)), static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x00462810
        return 1;
    }

    // ------------------------------------------------------------ 0x00436380 (ret 4)
    std::int16_t stub_zero(std::uint32_t) { return 0; }

    // ------------------------------------------------------------ 0x004378a0 (ret 4)
    void order_finish(std::uint8_t* u) {
        c_finish.hit();
        joan::orig::as_squad(u)->clear();  // 0x00422f10
        jo::as_unit(u)->wait_count = jo::as_unit(u)->wait_limit;
        joan::orig::as_unit(u)->play_idle(0, 1);  // 0x00508b90
        jo::view<std::uint32_t>(jo::self<jo::Order>(this)->current.kind) = 0;
    }

    // ------------------------------------------------------------ 0x00438720 (ret 4)
    void step_height(std::int32_t dir) {
        c_height.hit();
        if (dir == 0) return;
        void* const views = obj(d::g_camera_man);
        std::uint32_t rec[10];  // ebp-0x2c, 0x28 bytes; height at +0x1c (ebp-0x10)
        const std::int32_t idx = joan::orig::arg::as<std::int32_t>(joan::orig::as_camera_follow(views)->target());       // 0x00563dc0
        void* const view = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(views)->motion(joan::orig::arg::i32(idx)));               // 0x00563c80
        joan::orig::as_camera_motion(view)->end(joan::orig::arg::mptr(static_cast<void*>(rec)));        // 0x00423fe0
        float h;
        std::memcpy(&h, &rec[7], 4);
        const joan::hook::x87::Fpu fpu{true};
        double step = fpu.mul(static_cast<double>(global<std::uint32_t>(d::g_frame_elapsed_ms)), global<double>(d::dbl_0_6));
        // A wheel notch (120 counts, one frame) keeps its 30-frames-per-second step of 20 units at any frame rate.
        if (const std::int32_t wheel = global<std::int32_t>(d::g_mouse_wheel); wheel != 0 && joan::hook::settings().fix_wheel_zoom)
            step = std::abs(static_cast<double>(wheel)) * 20.0 / 120.0;
        if (dir == 1) {
            h = static_cast<float>(fpu.sub(h, step));
        } else {
            h = static_cast<float>(fpu.add(h, step));
        }
        if (!(h >= global<float>(d::flt_9cfc38))) h = 80.0f;  // test ah,1 je: less or unordered
        if (h > global<float>(d::flt_350)) h = 350.0f;     // test ah,0x41 jne
        global<float>(d::g_a0bbf0) = h;
        global<std::uint32_t>(d::g_a0bbf4) = global<std::uint32_t>(d::g_a0bbf0);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Unit::try_flee", 0x00436250, &H::try_flee);
B("Obj::stub_zero", 0x00436380, &H::stub_zero);
B("Order::finish", 0x004378a0, &H::order_finish);
B("Camera::step_height", 0x00438720, &H::step_height);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b104_01.cpp (game/b104_01)
namespace {
namespace game_b104_01 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;
// eax residue (a unit/anim pointer) of 0x004363a0: only the low word reaches the callee (arg::i16)
u32 addr(const void* p) { return joan::call::register_low_bits(p); }

struct FVec {
    float x, y, z;
};
static_assert(sizeof(FVec) == 12, "by-value 12-byte argument");

JOAN_COUNTER(c_find_4363a0, "Order::find_target_4363a0");
JOAN_COUNTER(c_try_437910, "Order::try_437910");

constexpr img::Addr kUnits = d::g_units;

i32 unit_dead(u32 id) { return joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* unit_find(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3f0
u8 unit_alive(void* rec) { return joan::orig::arg::as<u8>(joan::orig::as_unit(rec)->can_act()); }                // 0x00500ba0
void unit_anim(void* rec, u32 a, u32 b, u32 c) { joan::orig::as_unit(rec)->set_action(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c)); }  // 0x00501190
i32 inv_count(void* list, u32 kind, u32 id) { return joan::orig::arg::as<i32>(joan::orig::as_modifiers(list)->sum(joan::orig::arg::i32(kind), joan::orig::arg::i32(id))); }  // 0x00497e10
u32 g_now() { return global<u32>(d::g_now); }
using joan::orig::helpers::cf;

FVec vsub(const void* a, const void* b) {  // 0x0060cc90 (out, a, b) -> *out
    FVec t;
    return *joan::orig::arg::as<FVec*>(joan::orig::vec_sub(&t, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}

struct H {
    // ------------------------------------------------------------ destructors
    u32 dtor_4380a0() {
        jo::self<jo::Wall>(this)->vtable = img::obj(d::vt_Obj_9d3c88);
        joan::orig::as_wall(this)->release();          // 0x00487460
        return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->destruct());   // 0x00415790
    }

    // ------------------------------------------------------------ 0x004363a0
    i16 find_target_4363a0(u32 arg) {
        c_find_4363a0.hit();
        if (unit_dead(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) return 0;
        u8* rec = unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        if (arg != 0 && unit_alive(rec) != 0 && jo::as_unit(rec)->anim_kind != 0x2b && jo::as_unit(rec)->anim_kind != 0x2d &&
            jo::as_unit(rec)->anim_kind != 0x3a && jo::as_unit(rec)->action != 1) {
            // fall through
        } else {
            return 0;
        }
        u8 found = 0;
        if (jo::as_unit(rec)->anim_set == nullptr) return 0;
        const u32 k = joan::orig::arg::as<u32>(joan::orig::as_list(jo::as_unit(rec)->anim_set)->index_of_416dc0(0x22));  // 0x00416dc0
        if (k == 0) return 0;
        u8* p = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(rec)->anim_set)->entry(joan::orig::arg::u32(k)));  // 0x004168d0
        const u32 v14 = jo::view<u32>(jo::as_anim_group_rec(p)->model_ref);
        if (v14 == 0) return 0;
        u8* ty = joan::orig::arg::as<u8*>(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16((addr(p) & 0xffff0000u) | (v14 & 0xffffu))));  // 0x00416f20
        float hi = static_cast<float>(jo::self<jo::ModelSlot>(ty)->times[6]);
        ty = joan::orig::arg::as<u8*>(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16(v14 & 0xffffu)));
        float lo = static_cast<float>(jo::self<jo::ModelSlot>(ty)->times[7]);
        if (!(hi >= lo)) {
            const float t = hi;
            hi = lo;
            lo = t;
        }
        for (i32 i = joan::orig::as_dynarray(obj(kUnits))->size(); i > 0; --i) {  // 0x00417ad0
            u8* q = unit_find(static_cast<u32>(i));
            if (q == nullptr) continue;
            const u32 self = jo::view<u32>(jo::self<jo::Player>(this)->unit_id_);
            if (self == static_cast<u32>(i)) continue;
            u32 hi16;  // upper half of edx at the push (word read by the callee)
            const i16 t = jo::as_unit(q)->anim_kind;
            if (t == 0x40) {
                hi16 = self;
            } else if (t == 0x41) {
                hi16 = addr(q);
            } else if (t == 0x42 || t == 0x29) {
                hi16 = 0;
            } else {
                continue;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(rec)->hostile_to(joan::orig::arg::i16((hi16 & 0xffff0000u) | (static_cast<u32>(i) & 0xffffu)))) == 0)
                continue;  // 0x005094d0
            if (jo::view<i8>(jo::as_unit(q)->downed_hit) != 0) continue;
            float a = joan::orig::arg::as<float>(joan::orig::as_unit(rec)->heading_to_unit(joan::orig::arg::i32(i)));  // 0x00505960
            if (!(a >= lo)) continue;
            if (a > hi) continue;
            if (!(a >= lo) || a > hi) {  // never true here (kept from the original)
                if (!(a >= cf(d::k_9cf5c4))) {
                    a = a + cf(d::flt_360);
                    if (a > hi) continue;
                } else {
                    a = a - cf(d::flt_360);
                    if (!(a >= lo)) continue;
                }
            }
            FVec v34;
            joan::orig::as_unit(q)->position(&v34);  // 0x004fedb0
            u8* me = unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
            FVec d = vsub(jo::as_unit(me)->position_, &v34);
            if (!(static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, &d)) > cf(d::flt_3600))) {  // length_sq <= 3600
                found = 1;
                break;
            }
        }
        if (found != 0) {
            if (joan::orig::arg::as<u8>(joan::orig::as_unit(rec)->has_order_for(0x22u)) != 0) {  // 0x00500d40
                unit_anim(rec, 0x22, 0, 1);
                jo::self<jo::Player>(this)->u32_a8 = 0;
                return 1;
            }
        }
        return 0;
    }

    // ------------------------------------------------------------ 0x00437910
    i16 try_437910(u32 arg) {
        c_try_437910.hit();
        if (unit_dead(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) return 0;
        u8* rec = unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        const i32 v0c = jo::as_unit(rec)->mana;
        const i32 v10 = jo::as_unit(rec)->hit_points;
        if (joan::orig::arg::as<i32>(joan::orig::as_scenario(obj(d::g_mission))->mode()) == 1) return arg != 0 ? 1 : 0;  // 0x004cd510
        const Fpu fpu{true};
        const i32 a = joan::orig::arg::as<i32>(joan::orig::as_unit(rec)->value_149());  // 0x00523700
        const float f20 = static_cast<float>(
            fpu.div(fpu.mul(static_cast<double>(a), static_cast<double>(global<i32>(d::g_unit_egatk_mp))), cf(d::flt_100)));
        const i32 b = inv_count(jo::as_unit(rec)->modifiers, 0x15, static_cast<u32>(static_cast<i32>(jo::as_unit(rec)->id)));
        const i32 v14 = ftol(fpu.div(fpu.mul(static_cast<double>(b), f20), cf(d::flt_100)));
        const i32 c = joan::orig::arg::as<i32>(joan::orig::as_unit(rec)->hit_points_max());  // 0x00523320
        const i32 v18 = ftol(
            fpu.div(fpu.mul(static_cast<double>(c), static_cast<double>(global<i32>(d::g_a3cd1c))), cf(d::flt_100)));
        if (arg == 0) return 0;
        if (unit_alive(rec) == 0) return 0;
        if (jo::as_unit(rec)->action == 1) return 0;
        if (jo::as_unit(rec)->combo_time != 0) return 0;
        if (v0c < v14) return 0;
        if (v10 <= v18) return 0;
        if (joan::orig::arg::as<u8>(joan::orig::as_unit(rec)->has_order_for(0x1eu)) == 0) {               // 0x00500d40
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(rec)->find_upgrade_kind_d()) == 0) return 0;         // 0x0050d4b0
        }
        unit_anim(rec, 0x1e, 0, 1);
        jo::self<jo::Player>(this)->u32_a8 = 0;
        joan::orig::as_critical_hud(obj(d::g_critical_hud))->input(static_cast<std::int8_t>(joan::orig::arg::i32(0x45u)), static_cast<std::int8_t>(joan::orig::arg::i32(0u)));         // 0x00462810
        joan::orig::as_unit(rec)->set_145(joan::orig::arg::i32(jo::as_unit(rec)->mana - v14));     // 0x004fe5d0
        joan::orig::as_unit(rec)->set_value_135(joan::orig::arg::i32(jo::as_unit(rec)->hit_points - v18));     // 0x004fe510
        jo::view<u32>(jo::as_unit(rec)->last_ep_regen_ms) = g_now();
        jo::self<jo::Player>(this)->order_state = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_01", name, addr, fn)
B("Order::find_target_4363a0", 0x004363a0, &H::find_target_4363a0);
B("Order::try_437910", 0x00437910, &H::try_437910);
B("Obj::dtor_4380a0", 0x004380a0, &H::dtor_4380a0);
#undef B

}  // namespace game_b104_01
}  // namespace

// ---------------------------------------- from modules/game_b115_11.cpp (fix/uninit)
namespace {
namespace game_b115_11 {

using u32 = std::uint32_t;

// eax residue of 0x004367c0: its callers read ax only (playerctl.cpp, uinput.cpp) or drop it
u32 addr_of(const void* p) { return joan::call::register_low_bits(p); }

JOAN_COUNTER(c_order, "UnitCtl::order_update_4367c0 (fix/uninit)");

struct H {
    // ======================================================== 0x004367c0
    // Unit controller: button state machines for A / B / A+B, then special moves and combos.
    // Returns the original's full eax (callers read ax; one passes eax on).
    u32 order_update(i32 a, i32 b, i32 c) {
        c_order.hit();
        void* const units = obj(d::g_units);
        const i32 missing = joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_));  // 0x00428b70
        if (missing != 0) return static_cast<u32>(missing) & 0xffff0000u;
        u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        const u32 self_hi = addr_of(this) & 0xffff0000u;
        auto cnt = [u]() { return static_cast<i32>(jo::as_unit(u)->combo_state); };
        auto ready = [u]() { return joan::orig::arg::as<i8>(joan::orig::as_unit(u)->can_act()); };  // 0x00500ba0
        auto list = [u]() { return reinterpret_cast<u8*>(jo::as_unit(u)->anim_set); };
        auto entry = [](u8* l, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_anim_set(l)->entry(joan::orig::arg::u32(i))); };  // 0x004168d0
        auto act = [u](i32 x, i32 y, i32 z) { return thiscall<u32>(fn::Unit_set_action, u, x, y, z); };  // 0x00501190
        auto sfx = [](i32 x, i32 y) { joan::orig::as_critical_hud(obj(d::g_critical_hud))->input(static_cast<std::int8_t>(joan::orig::arg::i32(x)), static_cast<std::int8_t>(joan::orig::arg::i32(y))); };  // 0x00462810
        auto pay = [u](i32 v) { joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(v)); };  // 0x004fe5d0
        auto scaled = [u](i32 cost) {  // [unit+0x1c1] cost modifier (percent)
            if (jo::as_unit(u)->modifiers != nullptr) {
                const i32 pct = joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::as_unit(u)->modifiers)->sum(0x15, static_cast<i32>(jo::as_unit(u)->id)));  // 0x00497e10
                cost = static_cast<i32>(static_cast<u32>(pct) * static_cast<u32>(cost)) / 100;
            }
            return cost;
        };
        if (cnt() <= 0) {
            jo::self<jo::Player>(this)->button_up[0] = 1;
            jo::self<jo::Player>(this)->button_up[1] = 1;
            jo::self<jo::Player>(this)->button_up[2] = 1;
            jo::self<jo::Player>(this)->button_up[3] = 1;
        }
        // Button state bytes: 0 idle, 1 pressed, 2 released, 3 held.
        // k: the button (button_down[k] state, button_up[k] flag)
        auto press = [this](i32 k) {
            if (jo::view<i8>(jo::self<jo::Player>(this)->button_down[k]) == 0) {
                jo::self<jo::Player>(this)->button_down[k] = 1;
            } else if (jo::view<i8>(jo::self<jo::Player>(this)->button_down[k]) == 2) {
                jo::self<jo::Player>(this)->button_down[k] = 3;
            }
        };
        auto release = [this, &cnt](i32 k) {
            if (jo::view<i8>(jo::self<jo::Player>(this)->button_down[k]) == 1) {
                jo::self<jo::Player>(this)->button_down[k] = 2;
            } else if (jo::view<i8>(jo::self<jo::Player>(this)->button_down[k]) == 3) {
                jo::self<jo::Player>(this)->button_down[k] = 2;
            } else if (cnt() > 0) {
                jo::self<jo::Player>(this)->button_down[k] = 2;
            }
            jo::self<jo::Player>(this)->button_up[k] = 1;
        };
        if (a != 0) {
            press(0);
        } else {
            release(0);
        }
        if (b != 0) {
            press(1);
        } else {
            release(1);
        }
        if (a != 0 && b != 0) {
            press(2);
        } else if (a == 0 && b == 0) {
            release(2);
        }
        if (cnt() == -1 && global<u32>(d::g_now) - jo::as_unit(u)->combo_time > 0xfa) {
            jo::as_unit(u)->combo_time = 0;
            jo::view<u8>(jo::as_unit(u)->combo_state) = 0;
        }
        const u32 now = global<u32>(d::g_now);  // [ebp-0xc]
        auto in_window = [this, u, now]() {
            const u32 d = now - jo::self<jo::Player>(this)->press_time;
            return d >= jo::as_unit(u)->combo_min_time && d < jo::as_unit(u)->combo_max_time;
        };
        // ---- special 0x53 (button c)
        if (jo::as_unit(u)->combo_time == 0 && ready() != 0 && jo::as_unit(u)->action != 1 && c != 0) {
            const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_list(list())->index_of_416dc0(0x53));  // 0x00416dc0
            if (idx != 0 && joan::orig::arg::as<i8>(joan::orig::as_unit(u)->has_order_for(joan::orig::arg::u32(0x53))) != 0) {  // 0x00500d40
                const i32 cost = scaled(jo::as_anim_group_rec(entry(list(), idx))->min_mana);
                if (jo::as_unit(u)->mana >= cost) {
                    pay(jo::as_unit(u)->mana - cost);
                    act(0x53, 0, 1);
                    jo::self<jo::Player>(this)->u32_a8 = 0;
                    return self_hi | 1u;
                }
            }
        }
        // ---- button a
        if (a != 0 && jo::as_unit(u)->combo_time == 0) {
            if (ready() != 0) {
                if (jo::as_unit(u)->action != 1) {
                    if (c != 0) {
                        const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_list(list())->index_of_416dc0(0x53));
                        if (idx != 0 && jo::as_unit(u)->mana >= jo::as_anim_group_rec(entry(list(), idx))->min_mana) {
                            pay(jo::as_unit(u)->mana - jo::as_anim_group_rec(entry(list(), idx))->min_mana);
                            act(0x53, 0, 1);
                            jo::self<jo::Player>(this)->u32_a8 = 0;
                            return self_hi | 1u;
                        }
                    } else {
                        sfx(0x41, 1);
                        act(0x1c, 0x41, 1);
                        jo::self<jo::Player>(this)->u32_a8 = 0;
                        const u32 t = global<u32>(d::g_now);
                        jo::self<jo::Player>(this)->press_time = t;
                        jo::self<jo::Player>(this)->order_state = 0;
                        return (t & 0xffff0000u) | 1u;
                    }
                } else if (jo::view<i8>(jo::as_unit(u)->armour_flags[0]) == 0 && jo::as_unit(u)->anim_kind != 0x1a && jo::as_unit(u)->anim_kind != 0x18 &&
                           global<u32>(d::g_now) - jo::as_unit(u)->view > 0x32) {
                    const u32 r = act(0x1c, 0x44, 1);
                    jo::self<jo::Player>(this)->u32_a8 = 0;
                    jo::self<jo::Player>(this)->order_state = 0;
                    return (r & 0xffff0000u) | 1u;
                }
            } else if (cnt() > 0 && jo::view<i8>(jo::self<jo::Player>(this)->button_down[0]) == 3 && jo::view<i8>(jo::self<jo::Player>(this)->button_up[0]) == 1) {
                jo::self<jo::Player>(this)->button_up[0] = 0;
                if (in_window()) {
                    jo::view<u16>(jo::self<jo::Player>(this)->combo_step) = 1;
                    jo::self<jo::Player>(this)->order_state = 1;
                    jo::view<u16>(jo::as_unit(u)->combo_queued) = 1;
                    return (addr_of(u) & 0xffff0000u) | 1u;
                }
            } else if (jo::self<jo::Player>(this)->combo_count > 1 && jo::view<i8>(jo::self<jo::Player>(this)->button_down[0]) == 3 && jo::view<i8>(jo::self<jo::Player>(this)->button_up[0]) == 1) {
                jo::self<jo::Player>(this)->button_up[0] = 0;
                if (in_window()) {
                    jo::view<u16>(jo::self<jo::Player>(this)->combo_step) = 1;
                    jo::self<jo::Player>(this)->order_state = 1;
                    jo::view<u16>(jo::as_unit(u)->combo_queued) = 1;
                    return self_hi | 1u;
                }
            }
        }
        // ---- button b
        if (b != 0 && jo::as_unit(u)->combo_time == 0) {
            if (ready() != 0) {
                if (jo::as_unit(u)->action != 1) {
                    const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_anim_table(list())->find(0x1c, joan::orig::arg::str(obj(d::s_B))));  // 0x00416e30
                    if (idx != 0) {
                        const i32 cost = scaled(jo::as_anim_group_rec(entry(list(), idx))->min_mana);
                        if (jo::as_unit(u)->mana >= cost) {
                            pay(jo::as_unit(u)->mana - cost);
                            sfx(0x42, 1);
                            act(0x1c, 0x42, 1);
                            jo::self<jo::Player>(this)->u32_a8 = 0;
                            jo::self<jo::Player>(this)->press_time = global<u32>(d::g_now);
                            jo::self<jo::Player>(this)->order_state = 0;
                            return self_hi | 1u;
                        }
                    }
                } else {
                    const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_anim_table(list())->find(0x1d, joan::orig::arg::str(obj(d::s_E_9cfc2c))));
                    if (idx != 0) {
                        u8* const e = entry(list(), idx);
                        if (joan::orig::arg::as<i32>(joan::orig::as_anim_set_entry(e)->is_yes()) != 0 ||                // 0x00416ed0
                            joan::orig::arg::as<i32>(joan::orig::as_unit(u)->find_link(0xa, joan::orig::arg::str(0))) != 0) {        // 0x0050d3d0
                            const i32 cost = scaled(jo::as_anim_group_rec(e)->min_mana);
                            if (jo::as_unit(u)->mana >= cost) {
                                pay(jo::as_unit(u)->mana - cost);
                                act(0x1d, 0x45, 1);
                                jo::self<jo::Player>(this)->u32_a8 = 0;
                                jo::self<jo::Player>(this)->press_time = global<u32>(d::g_now);
                                jo::self<jo::Player>(this)->order_state = 0;
                                return self_hi | 1u;
                            }
                        }
                    }
                }
            } else if (cnt() > 0 && jo::view<i8>(jo::self<jo::Player>(this)->button_down[1]) == 3 && jo::view<i8>(jo::self<jo::Player>(this)->button_up[1]) == 1) {
                jo::self<jo::Player>(this)->button_up[1] = 0;
                if (in_window()) {
                    jo::view<u16>(jo::self<jo::Player>(this)->combo_step) = 2;
                    jo::self<jo::Player>(this)->order_state = 1;
                    jo::view<u16>(jo::as_unit(u)->combo_queued) = 1;
                    return self_hi | 1u;
                }
            } else if (jo::self<jo::Player>(this)->combo_count > 1 && jo::view<i8>(jo::self<jo::Player>(this)->button_down[1]) == 3 && jo::view<i8>(jo::self<jo::Player>(this)->button_up[1]) == 1) {
                jo::self<jo::Player>(this)->button_up[1] = 0;
                if (in_window()) {
                    jo::view<u16>(jo::self<jo::Player>(this)->combo_step) = 2;
                    jo::self<jo::Player>(this)->order_state = 1;
                    jo::view<u16>(jo::as_unit(u)->combo_queued) = 1;
                    return self_hi | 1u;
                }
            }
        }
        // ---- combo resolution once the input window has passed
        if (now - jo::self<jo::Player>(this)->press_time >= jo::as_unit(u)->combo_max_time && jo::view<i8>(jo::self<jo::Player>(this)->order_state) == 1) {
            u8* const l = list();  // [ebp-0x30]
            i32 found = 0;         // [ebp-0x34] (fix/uninit: the original leaves it unset when no
                                   // entry matches the four keys)
            if (jo::as_unit(u)->anim_kind == 0x1b) {
                found = 1;
            } else {
                for (i32 i = jo::as_anim_set(l)->group_count; i > 0; --i) {
                    u8* const e = entry(l, i);
                    if (jo::view<i32>(jo::as_anim_group_rec(e)->is_jump) != jo::as_unit(u)->action || jo::as_anim_group_rec(e)->kind != jo::as_unit(u)->anim_kind ||
                        jo::as_anim_group_rec(e)->weapon_kind != jo::as_unit(u)->anim_item_using || jo::as_anim_group_rec(e)->direction != jo::as_unit(u)->anim_special)
                        continue;
                    found = i;
                    i32 k = 0;
                    for (; k < cnt(); ++k) {
                        if (reinterpret_cast<const i8*>(e)[static_cast<i32>(offsetof(jo::AnimGroupRec, name_8)) + k] != reinterpret_cast<const i8*>(u)[static_cast<i32>(offsetof(jo::Unit, armour_flags)) + k]) {  // the combo letters: AnimGroupRec::name_8[k] / Unit::armour_flags[k], as base + (offset + k) like the original (k may pass name_8)
                            found = 0;
                            break;
                        }
                    }
                    if (found == 0) continue;
                    // Next combo letter from the pressed button (+0x9b: 1 'A', 2 'B', 3 'C',
                    // 4 none). fix/uninit: for any other value the original compares with the
                    // never-written [ebp-0x38]; here the entry does not match.
                    const i16 btn = jo::self<jo::Player>(this)->combo_step;
                    i8 letter = 0;
                    bool have = true;
                    if (btn == 1) {
                        letter = 0x41;
                    } else if (btn == 2) {
                        letter = 0x42;
                    } else if (btn == 3) {
                        letter = 0x43;
                    } else {
                        have = false;
                        found = 0;
                    }
                    if (!have || reinterpret_cast<const i8*>(e)[static_cast<i32>(offsetof(jo::AnimGroupRec, name_8)) + k] != letter) found = 0;
                    if (found == 0) continue;
                    for (++k; k < 6; ++k) {
                        if (reinterpret_cast<const i8*>(e)[static_cast<i32>(offsetof(jo::AnimGroupRec, name_8)) + k] != 0) {
                            found = 0;
                            break;
                        }
                    }
                    if (found != 0) {
                        if (joan::orig::arg::as<i32>(joan::orig::as_anim_set_entry(e)->is_yes()) != 0) break;            // 0x00416ed0
                        if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->has_item_kind9(joan::orig::arg::str(e + 8))) != 0) break;     // 0x0050d170
                    }
                    found = 0;
                }
            }
            if (found != 0) {
                const i16 btn = jo::self<jo::Player>(this)->combo_step;
                if (btn == 1) {
                    act(0x1c, 0x41, 1);
                    jo::self<jo::Player>(this)->u32_a8 = 0;
                    sfx(0x41, 0);
                } else if (btn == 2) {
                    joan::orig::as_modifiers(jo::as_unit(u)->modifiers)->slot(1);  // 0x004974f0 (unused)
                    joan::orig::as_modifiers(jo::as_unit(u)->modifiers)->slot(0);
                    const i32 cost = scaled(jo::as_anim_group_rec(entry(list(), found))->min_mana);
                    if (jo::as_unit(u)->mana >= cost) {
                        act(0x1c, 0x42, 1);
                        jo::self<jo::Player>(this)->u32_a8 = 0;
                        pay(jo::as_unit(u)->mana - cost);
                        sfx(0x42, 0);
                    }
                } else if (btn == 3) {
                    act(0x1c, 0x43, 1);
                    jo::self<jo::Player>(this)->u32_a8 = 0;
                    sfx(0x43, 0);
                }
                jo::self<jo::Player>(this)->press_time = global<u32>(d::g_now);
                jo::self<jo::Player>(this)->order_state = 0;
                jo::view<u16>(jo::as_unit(u)->combo_queued) = 0;
                return self_hi | 1u;
            }
            jo::self<jo::Player>(this)->button_down[0] = 0;
            jo::self<jo::Player>(this)->button_down[1] = 0;
            jo::self<jo::Player>(this)->button_down[2] = 0;
            jo::self<jo::Player>(this)->button_down[3] = 0;
            jo::self<jo::Player>(this)->order_state = 0;
            jo::view<u16>(jo::as_unit(u)->combo_queued) = 0;
            return self_hi | 1u;
        }
        const u32 d = now - jo::self<jo::Player>(this)->press_time;
        if (d < jo::as_unit(u)->combo_max_time) return d & 0xffff0000u;
        const u32 r = thiscall<u32>(fn::Unit_can_act_500ba0, u);  // 0x00500ba0
        if (static_cast<i8>(r) == 0) return r & 0xffff0000u;
        if (cnt() != 0) return addr_of(u) & 0xffff0000u;
        jo::self<jo::Player>(this)->button_down[0] = 0;
        jo::self<jo::Player>(this)->button_down[1] = 0;
        jo::self<jo::Player>(this)->button_down[2] = 0;
        jo::self<jo::Player>(this)->button_down[3] = 0;
        return self_hi;
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: [ebp-0x34] starts at 0 (no matching combo entry); +0x9b outside 1..4 -> no letter match.
F("UnitCtl::order_update_4367c0", 0x004367c0, &H::order_update);
#undef F

}  // namespace game_b115_11
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_big_dtor, "Big::~Big_4375a0");

struct H {
    // ------------------------------------------------------------------ 0x004375a0
    u32 big_dtor() {
        c_big_dtor.hit();
        // this: the SaveMenu (b_a411d0); every child through its typed member (the x86 offsets 0x2c .. 0x4823 moved on x64)
        joan::orig::as_panel(this)->init_children();                // 0x0074d790
        joan::orig::as_scroll_bar(&jo::self<jo::SaveMenu>(this)->edit_scroll)->destruct();  // 0x007617c0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->edit_button)->dtor_6211a0();  // 0x006211a0
        joan::orig::as_line_edit(&jo::self<jo::SaveMenu>(this)->name_edit)->destruct();  // 0x0045b880
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->in_bg)->dtor_6294c0();  // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->in_frame)->dtor_6294c0();  // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_3f58)->dtor_6211a0();  // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_3c12)->dtor_6211a0();
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->mark)->dtor_6211a0();
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_3586)->dtor_6211a0();
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_3240)->dtor_6211a0();
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_2efa)->dtor_6211a0();
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_2bb4)->dtor_6211a0();
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->btn_286e)->dtor_6211a0();
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->bar_bottom)->dtor_6294c0();  // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->bar_top)->dtor_6294c0();
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->info)->dtor_6294c0();
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->info_bar)->dtor_6211a0();  // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->pic)->dtor_6294c0();  // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::SaveMenu>(this)->frame)->dtor_6294c0();
        joan::orig::as_combo_box(&jo::self<jo::SaveMenu>(this)->list)->destruct();  // 0x006b2340
        using VecDtor = u32(JOAN_STDCALL*)(void*, u32, i32, std::uintptr_t);
        return reinterpret_cast<VecDtor>(fn::code_value(fn::crt_eh_vector_destructor_iterator))(jo::self<jo::SaveMenu>(this)->slots, static_cast<u32>(sizeof(jo::Button)), 0xa,
                                                     std::uintptr_t{fn::code_value(fn::Widget_dtor_6211a0)});  // eh vector dtor iterator
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("Big::~Big_4375a0", 0x004375a0, &H::big_dtor);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_units5.cpp (game/units5)
namespace {
namespace game_units5 {

JOAN_COUNTER(c_attach_object, "Cell::attach_object");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // `this` through a volatile copy: the cell functions are called with a null cell (detach
    // only), and the compiler may otherwise fold `this == nullptr` to false.
    std::uint8_t* maybe_null_self() {
        std::uint8_t* volatile p = self();
        return p;
    }

    // 0x00437770 (ret 4)
    void attach_object(std::uint8_t* o) {
        std::uint8_t* const cell = maybe_null_self();
        c_attach_object.hit();
        std::uint8_t* const old = jo::view<std::uint8_t*>(jo::as_unit(o)->cell);
        if (cell == old) return;
        if (old != nullptr) {
            std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_unit(o)->cell_prev);
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_unit(o)->cell_next);
            if (prev != nullptr) jo::view<std::uint8_t*>(jo::as_unit(prev)->cell_next) = next;
            else jo::view<std::uint8_t*>(jo::as_cell(old)->units_head) = next;
            if (next != nullptr) jo::view<std::uint8_t*>(jo::as_unit(next)->cell_prev) = prev;
            else jo::view<std::uint8_t*>(jo::as_cell(old)->units_tail) = prev;
            jo::view<std::uint8_t*>(jo::as_unit(o)->cell_prev) = nullptr;
            jo::view<std::uint8_t*>(jo::as_unit(o)->cell_next) = nullptr;
            jo::view<std::uint8_t*>(jo::as_unit(o)->cell) = nullptr;
        }
        if (cell == nullptr) return;
        std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_cell(cell)->units_head);
        jo::view<std::uint8_t*>(jo::as_cell(cell)->units_head) = o;
        if (next != nullptr) jo::view<std::uint8_t*>(jo::as_unit(next)->cell_prev) = o;
        else jo::view<std::uint8_t*>(jo::as_cell(cell)->units_tail) = o;
        jo::view<std::uint8_t*>(jo::as_unit(o)->cell_prev) = nullptr;
        jo::view<std::uint8_t*>(jo::as_unit(o)->cell_next) = next;
        jo::view<std::uint8_t*>(jo::as_unit(o)->cell) = cell;
    }
};

#define U5(name, addr, fn) JOAN_REPLACE("game/units5", name, addr, fn)
U5("Cell::attach_object", 0x00437770, &H::attach_object);
#undef U5

}  // namespace game_units5
}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
namespace {
namespace game_b88_05 {

JOAN_COUNTER(c_update_station, "Place::update_station");

struct H {
    // ------------------------------------------------------------ 0x00437e20
    std::uintptr_t update_station() {
        c_update_station.hit();
        if (jo::view<void*>(jo::self<jo::Place>(this)->cell) != nullptr) {
            const std::uint32_t inside =
                joan::orig::arg::as<std::uint32_t>(joan::orig::as_aabb(jo::self<jo::Place>(this)->cell->box)->contains(joan::orig::arg::ptr(jo::self<jo::Place>(this)->position_))) & 0xffu;  // 0x0060d5f0
            if (inside != 0) return inside;
        }
        if (jo::view<void*>(jo::self<jo::Place>(this)->world) == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        std::int32_t found = 0;  // ebp-0x8
        std::uint8_t box[24];    // ebp-0x20
        std::memcpy(box, jo::self<jo::Place>(this)->box, 24);
        void** const r = joan::orig::arg::as<void**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::Place>(this)->world))->cells_in_box(joan::orig::arg::mptr(box), &found));  // 0x00429090
        if (found <= 0) return reinterpret_cast<std::uintptr_t>(r);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_cell(*r)->attach_place(joan::orig::arg::mptr(this)));  // 0x0043ec80
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("Place::update_station", 0x00437e20, &H::update_station);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_order_437ed0, "Mission::unit_order_437ed0");

constexpr img::Addr kUnits = d::g_units;  // unit table (0x00428b70 check, 0x0041b3c0 get)

struct H {
    // -------------------------------------------------------------- 0x00437ed0
    u32 unit_order(i32 a8, i32 ac) {
        c_order_437ed0.hit();
        u32 r = joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x00428b70
        if (r != 0) return r;
        void* const u = joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        r = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->can_act());  // 0x00500ba0 (char; callers discard the result)
        if (static_cast<i8>(r & 0xffu) == 0) return r;
        i16 cmd = joan::orig::arg::as<i16>(joan::orig::as_player(this)->order_icon());  // 0x004350f0
        u8 alt = 0;
        i16 kind = jo::as_unit(u)->anim_kind;
        if (jo::view<i8>(jo::self<jo::Player>(this)->riding) != 0) {
            alt = 1;
            kind = jo::as_unit(u)->anim_kind_alt;
        }
        if (ac != 0) {
            if (cmd == 0xb) cmd = 0xa;
            else if (cmd == 0x46) cmd = 0x44;
        }
        if (a8 != 0 && kind != 0x2b && kind != 0x2c && kind != 0x2d && jo::as_unit(u)->action != 1) {
            if (cmd == 0xb && jo::view<i32>(jo::self<jo::Player>(this)->u32_a8) == 0) jo::self<jo::Player>(this)->u32_a8 = global<u32>(d::g_now);
            // void callees; every caller of unit_order (playerctl 0x00430480, ctrl 0x00432d30 / 0x004343e0 / 0x00438110)
            // discards its result.
            if (alt != 0) {
                joan::orig::as_unit(u)->set_formation_slot_5124b0(joan::orig::arg::i16(static_cast<u16>(cmd)));  // 0x005124b0
                return 0;
            }
            joan::orig::as_unit(u)->set_action(joan::orig::arg::i32(static_cast<u16>(cmd)), i32{0}, i32{1});  // 0x00501190
            return 0;
        }
        if (cmd == 1) jo::self<jo::Player>(this)->u32_a8 = 0;
        if (kind != 0xb && kind != 9 && kind != 0x46 && kind != 0x44 && kind != 0x45 && kind != 0xa)
            return static_cast<u32>(static_cast<i32>(kind));
        if (a8 != 0) return static_cast<u32>(static_cast<i32>(kind));
        if (cmd == 0xb) jo::self<jo::Player>(this)->u32_a8 = 0;
        if (alt != 0) {
            joan::orig::as_unit(u)->set_formation_slot_5124b0(joan::orig::arg::i16(u16{1}));  // 0x005124b0
            return 0;
        }
        joan::orig::as_unit(u)->play_idle(i32{0}, i32{1});  // 0x00508b90
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Mission::unit_order_437ed0", 0x00437ed0, &H::unit_order);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using u32 = std::uint32_t;

constexpr img::Addr kUnits(d::g_units);

JOAN_COUNTER(c_order_pair, "Unit::order_pair_438110");

struct H {
    // ------------------------------------------------------------------ 0x00438110
    bool order_pair(i32 a, i32 b, i32 c, i32 d) {
        c_order_pair.hit();
        if (joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return false;  // 0x00428b70
        void* const unit = joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->can_act()) == 0) return false;                        // 0x00500ba0
        if (static_cast<i32>(jo::as_unit(unit)->action) == 1) return false;
        const bool A = a != 0, B = b != 0, C = c != 0, D = d != 0;
        if (A && !B && !C && !D) {
            joan::orig::as_player(this)->unit_order(i32{0xc}, i32{1});  // 0x00437ed0
            return true;
        }
        i32 code = 0;
        if (!A && B && !C && !D) code = 0x10;
        else if (!A && !B && C && !D) code = 0xe;
        else if (!A && !B && !C && D) code = 0xf;
        else if (A && !B && C && !D) code = 0x11;
        else if (A && !B && !C && D) code = 0x12;
        else if (!A && B && C && !D) code = 0x13;
        else if (!A && B && !C && D) code = 0x14;
        else return false;
        if (jo::view<i8>(jo::self<jo::Player>(this)->riding) != 0) {
            joan::orig::as_unit(unit)->set_formation_slot_5124b0(joan::orig::arg::i16(code));  // 0x005124b0
        } else {
            joan::orig::as_unit(unit)->set_action(joan::orig::arg::i32(code), i32{0}, i32{1});  // 0x00501190
        }
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Unit::order_pair_438110", 0x00438110, &H::order_pair);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x004386b0
    u32 update_e4_4386b0() {
        if (jo::view<i16>(jo::self<jo::Wall>(this)->u16_e4) == 0) return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->start_effect(static_cast<char>(joan::orig::arg::i32(0))));  // 0x00478b40
        if (jo::self<jo::Wall>(this)->u32_c6 == 0) {
            jo::self<jo::Wall>(this)->u16_e4 = 0;
            return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->start_effect(static_cast<char>(joan::orig::arg::i32(0))));  // 0x00478b40
        }
        return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->start_effect(static_cast<char>(joan::orig::arg::i32(1))));  // 0x00478b40
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Unit::update_e4_4386b0", 0x004386b0, &H::update_e4_4386b0);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;
using joan::orig::helpers::fild;
using joan::orig::helpers::fd;

constexpr Fpu kFpu{true};

void* const kUnits = obj(d::g_units);

void* unit(u32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 unit_missing(u32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70

JOAN_COUNTER(c_try_rally, "Order::try_rally_438820");

struct H {
    // ------------------------------------------------------------------ 0x00438820
    i16 try_rally(i32 arg) {
        c_try_rally.hit();
        if (arg == 0) return 0;
        if (unit_missing(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)) != 0) return 0;
        void* const u = unit(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        const std::int32_t target = joan::orig::as_cell_object(u)->find_anim_state();  // 0x00504010 (an anim entry index)
        if (target == 0) return 0;
        const float limit = joan::orig::arg::as<float>(joan::orig::as_unit(this)->link_value_of());   // 0x0042fd60
        const double base = joan::orig::arg::as<double>(joan::orig::as_player(this)->unit_progress());  // 0x0042fdd0 (raw st0)
        double x = kFpu.mul(base, kFpu.div(fild(global<i32>(d::g_a33744)), fd(d::flt_1000)));
        x = kFpu.div(x, fild(static_cast<i32>(jo::self<jo::Player>(this)->combo_count)));
        if (!(x >= static_cast<double>(limit))) return 0;
        void* const slot = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::as_unit(u)->anim_set)->entry(static_cast<std::uint32_t>(target)));  // 0x004168d0
        if (static_cast<i32>(static_cast<i8>(jo::as_anim_group_rec(slot)->u8_f)) == 0x20) return 0;
        joan::orig::as_unit(u)->set_action(0x1b, 0, 1);  // 0x00501190
        jo::view<u8>(jo::self<jo::Player>(this)->combo_count) = static_cast<u8>(jo::view<u8>(jo::self<jo::Player>(this)->combo_count) + 1);
        jo::self<jo::Player>(this)->order_state = 1;
        jo::self<jo::Player>(this)->press_time = global<u32>(d::g_now);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Order::try_rally_438820", 0x00438820, &H::try_rally);
#undef B

}  // namespace game_b108_13
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
B("init_0x00438980", 0x00438980, &init_wrapper<fn::init_pi_a336fc>);
B("init_0x004389e0", 0x004389e0, &init_wrapper<fn::init_a33700>);
B("init_0x00438a30", 0x00438a30, &init_wrapper<fn::init_a33704>);
B("init_0x00438a80", 0x00438a80, &init_wrapper<fn::init_438aa0>);
#undef B

}  // namespace game_b105_01_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Player::leave_mode1() -> void { joan::bind::member<&::game_b85_06::H::nop0>(this); }  // 0x432d10
auto joan::orig::Ctrl::update_432d30() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b109_18::H::update>(this)); }  // 0x432d30
auto joan::orig::Player::tick_mode1() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_01::H::dtor_pair>(this)); }  // 0x4338c0
auto joan::orig::Player::draw_mode1() -> void { joan::bind::member<&::game_b82_18::H::update_4338f0>(this); }  // 0x4338f0
auto joan::orig::Joystick::deflection_level() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b115_08::H::deflection_level>(this)); }  // 0x433930
auto joan::orig::Action::before_now(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_01::H::before_now>(this, a0)); }  // 0x433a90
auto joan::orig::init_a3cb7c() -> void { joan::bind::free<&::game_b94_01::init_a3cb7c>(); }  // 0x433ac0
auto joan::orig::Pad::stick_angle_433af0() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b111_19::H::stick_angle>(this)); }  // 0x433af0
auto joan::orig::Cmd::pick_target() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_01::H::pick_target>(this)); }  // 0x433c20
auto joan::orig::Group::init(std::int16_t a0, std::int8_t a1, const char* a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::group_init>(this, a0, a1, a2)); }  // 0x433ea0
auto joan::orig::Player::target_close() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b95_01::H::units_close>(this)); }  // 0x433f40
auto joan::orig::Player::face_target() -> void { joan::bind::member<&::game_b100_01::H::face_update>(this); }  // 0x434080
auto joan::orig::Order::stop_unit(void* a0) -> void { joan::bind::member<&::game_b93_01::H::stop_unit_order>(this, a0); }  // 0x434210
auto joan::orig::Unit::update_a2_434270() -> void { joan::bind::member<&::game_b101_01::H::update_a2_434270>(this); }  // 0x434270
auto joan::orig::Player::leave_mode0() -> void { joan::bind::member<&::game_b85_06::H::nop0>(this); }  // 0x4343c0
auto joan::orig::Player::handle_input_4343e0() -> std::uint16_t { return joan::bind::conv<std::uint16_t>(joan::bind::member<&::game_b109_01::H::handle_input>(this)); }  // 0x4343e0
auto joan::orig::Player::draw_mode0() -> void { joan::bind::member<&::game_b82_18::H::empty>(this); }  // 0x434f20
auto joan::orig::Player::tick_mode0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_01::H::dtor_pair>(this)); }  // 0x434f40
auto joan::orig::Sound3DListener::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_01::H::sdtor_4350b0>(this, a0)); }  // 0x4350b0
auto joan::orig::Player::order_icon() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_07::H::order_icon>(this)); }  // 0x4350f0
auto joan::orig::Ctrl::turn_4352f0(float a0, std::int32_t a1, float a2) -> void { joan::bind::member<&::game_b106_16::H::turn_4352f0>(this, a0, a1, a2); }  // 0x4352f0
auto joan::orig::Unit::aim_at_player(std::int32_t a0) -> void { joan::bind::member<&::game_b98_01::H::aim_at_player>(this, a0); }  // 0x4360d0
auto joan::orig::Player::set_follow(std::int32_t a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b87_05::H::set_follow>(this, a0)); }  // 0x436190
auto joan::orig::Unit::try_flee(std::int32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b92_01::H::try_flee>(this, a0)); }  // 0x436250
auto joan::orig::Player::stub_zero(std::uint32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b92_01::H::stub_zero>(this, a0)); }  // 0x436380
auto joan::orig::Order::find_target_4363a0(std::uint32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b104_01::H::find_target_4363a0>(this, a0)); }  // 0x4363a0
auto joan::orig::Place::set_owner(World* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b87_05::H::set_owner>(this, a0)); }  // 0x4366f0
auto joan::orig::Water::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::sdd_436780>(this, a0)); }  // 0x436780
auto joan::orig::Player::order_update(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b115_11::H::order_update>(this, a0, a1, a2)); }  // 0x4367c0
auto joan::orig::SaveMenu::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_01::H::big_dtor>(this)); }  // 0x4375a0
auto joan::orig::Cell::attach_object(void* a0) -> void { joan::bind::member<&::game_units5::H::attach_object>(this, a0); }  // 0x437770
auto joan::orig::Order::finish(void* a0) -> void { joan::bind::member<&::game_b92_01::H::order_finish>(this, a0); }  // 0x4378a0
auto joan::orig::Order::try_437910(std::uint32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b104_01::H::try_437910>(this, a0)); }  // 0x437910
auto joan::orig::ModelResMgr::add_ref(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_06::H::ref_add>(this, a0)); }  // 0x437af0
auto joan::orig::Place::update_station() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_05::H::update_station>(this)); }  // 0x437e20
auto joan::orig::Player::unit_order(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::unit_order>(this, a0, a1)); }  // 0x437ed0
auto joan::orig::Player::order_direction(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b108_11::H::order_pair>(this, a0, a1, a2, a3)); }  // 0x438110
auto joan::orig::Place::add_parking(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b87_05::H::append>(this, a0, a1)); }  // 0x438420
auto joan::orig::SoundSlot::position(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_01::H::pos_4384a0>(this, a0)); }  // 0x4384a0
auto joan::orig::Player::update_camera() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::update_camera>(this)); }  // 0x4384e0
auto joan::orig::Camera::step_height(std::int32_t a0) -> void { joan::bind::member<&::game_b92_01::H::step_height>(this, a0); }  // 0x438720
auto joan::orig::Order::try_rally_438820(std::int32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b108_13::H::try_rally>(this, a0)); }  // 0x438820
