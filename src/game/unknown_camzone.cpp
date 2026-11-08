// game/unknown_camzone.cpp: functions whose original source file is unknown (formerly game/_unplaced/camzone.cpp;
// re/attribution.csv: between ww/camman.cpp and ww/camres.cpp). They move to their original file when evidence names it
// (Phase 3).
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
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;


using joan::call::cdecl_call;

using joan::hook::global;
using joan::img::obj;

struct V3 {
    float x, y, z;
};

using joan::call::import_call;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

using i32 = std::int32_t;

using u8 = std::uint8_t;

using i8 = std::int8_t;

struct Obj {};

using u32 = std::uint32_t;

using joan::call::stdcall_call;

// ---------------------------------------------------------------------- static initialisers
using joan::orig::helpers::div_global;

}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::img::obj;

using joan::call::cdecl_call;

JOAN_COUNTER(c_cam_init, "CamZones::init");

using joan::orig::helpers::game_new;      // operator new

struct H {
    // ---------------------------------------------------------------- cam zones / tracks
    void* zones_ctor() {  // 0x00425dc0
        std::memset(static_cast<void*>(this), 0, sizeof(jo::CamZones));  // memset (0x4c bytes on x86)
        return this;
    }
    // 0x00425e20 CamZones::init: +0x12 new 9-byte object, +0x34 = 50, 0x00428b00(0x004203c0()),
    // 0x00427cc0(1), +0x46 = 1.
    std::uint32_t zones_init() {
        c_cam_init.hit();
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::CamRes)));  // 9 on x86: CamTracks + CamRes::loaded (+8)
        void* const p = mem != nullptr ? joan::orig::as_cam_tracks(mem)->construct() : nullptr;  // 0x00428cf0
        jo::view<void*>(jo::self<jo::CamZones>(this)->tracks) = p;
        jo::view<std::uint32_t>(jo::self<jo::CamZones>(this)->height_offset) = 0x42480000u;  // 50.0f
        const std::uint32_t v = joan::orig::arg::as<std::uint32_t>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());  // 0x004203c0
        joan::orig::as_camera_zones(this)->set_unit(joan::orig::arg::u32(v));                                                 // 0x00428b00
        joan::orig::as_camera_zones(this)->set_track_set(joan::orig::arg::u32(1));          // 0x00427cc0
        jo::view<std::uint32_t>(jo::self<jo::CamZones>(this)->keys_per_ms) = 0x3f800000u;  // 1.0f
        return 0;  // callers discard the result
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("CamZones::CamZones", 0x00425dc0, &H::zones_ctor);
L82("CamZones::init", 0x00425e20, &H::zones_init);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    std::uint32_t fwd_425df0() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera_zones(this)->free_tracks()); }  // 0x00425ef0
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("fwd_00425df0", 0x00425df0, &H::fwd_425df0);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18_2 {

using joan::orig::helpers::game_delete;  // operator delete

struct H {
    // scalar deleting destructors
    void* sdd(std::uintptr_t dtor, std::uint32_t flags) {
        thiscall(dtor, this);
        if (flags & 1u) game_delete(this);
        return this;
    }
    void* sdd_425f70(std::uint32_t flags) { return sdd(fn::code_value(fn::Obj_forward_428d50), flags); }  // 0x00428d20
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("sdd_00425f70", 0x00425f70, &H::sdd_425f70);
#undef B82

}  // namespace game_b82_18_2
}  // namespace

// ---------------------------------------- from modules/game_camera.cpp (game/camera1)
namespace {
namespace game_camera {

using joan::img::obj;

struct V3 {
    float x, y, z;
};

constexpr img::Addr kCameraMan(d::g_camera_man);

constexpr img::Addr kGameUnits(d::g_units);

JOAN_COUNTER(c_zone, "CameraZone::update");

struct H {
    // Zones (+0 array of 0xaca-byte zones: +2 kind (0 sample, 1 scripted), +0x27 name,
    // +0x48 box; +4 count).
    // 0x00425fc0 CameraZone::update (this: +0 unit id, +0x12 zones, +0x16 enabled, +0x18
    // current zone (1-based), +0x28 last position, +0x34 height offset).
    std::int32_t zone_update() {
        c_zone.hit();
        if (jo::self<jo::CamZones>(this)->current_zone != 0) {
            std::uint8_t* const zone = reinterpret_cast<std::uint8_t*>(&jo::self<jo::CamZones>(this)->tracks->tracks[jo::self<jo::CamZones>(this)->current_zone - 1]);
            if (jo::view<std::int32_t>(jo::as_cam_track(zone)->kind) == 1) {
                joan::orig::as_camera_zones(this)->scripted(joan::orig::arg::mptr(static_cast<void*>(zone)));  // 0x00427d90 scripted
                return 1;
            }
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(img::obj(kGameUnits))->empty_at_b(joan::orig::arg::i32(jo::self<jo::CamZones>(this)->unit_id))) != 0 || jo::self<jo::CamZones>(this)->enabled == 0) return 0;  // 0x00428b70
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(img::obj(kGameUnits))->get_b(joan::orig::arg::i32(jo::self<jo::CamZones>(this)->unit_id)));                       // 0x0041b3c0
        V3 pos{global<float>(d::g_pos), global<float>(d::g_a69cdc), global<float>(d::g_a69ce0)};
        joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(static_cast<void*>(&pos)));  // 0x004fedb0
        if (jo::as_unit(unit)->actor != nullptr && jo::view<std::uint32_t>(jo::as_actor(jo::as_unit(unit)->actor)->record) != 0) {
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_players(img::obj(d::g_physic_sys))->get(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(unit)->actor)->record))));  // 0x006af310
            std::memcpy(&pos.z, &jo::as_shot(r)->state.cell, 4);  // Body +0x218 = Shot::state +0xd6 (a float height)
        }
        pos.z = pos.z + jo::self<jo::CamZones>(this)->height_offset;
        if ((joan::orig::arg::as<std::uint8_t>(joan::orig::vec_equal(static_cast<void*>(&pos), static_cast<void*>(jo::self<jo::CamZones>(this)->last_position))) & 0xff) != 0 &&  // 0x0060ced0
            jo::self<jo::CamZones>(this)->current_zone != 0)
            return 1;
        std::uint8_t* zone = nullptr;
        std::int32_t id = jo::self<jo::CamZones>(this)->current_zone;
        if (id != 0) zone = reinterpret_cast<std::uint8_t*>(&jo::self<jo::CamZones>(this)->tracks->tracks[id - 1]);
        if (id == 0 || (zone != nullptr && (joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(jo::as_cam_track(zone)->box)->contains(static_cast<void*>(&pos))) & 0xff) == 0))  // 0x0060d5f0
            id = joan::orig::arg::as<std::int32_t>(joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CamZones>(this)->tracks))->find_zone(joan::orig::arg::str(&img::global<const char>(d::s_SAMPLE)), pos));  // 0x00429bb0 "SAMPLE"
        if (id != 0) {
            std::uint8_t* const z = reinterpret_cast<std::uint8_t*>(&jo::self<jo::CamZones>(this)->tracks->tracks[id - 1]);
            if (jo::self<jo::CamZones>(this)->current_zone != id) {
                if (jo::self<jo::CamZones>(this)->current_zone == 0) joan::orig::as_camera_man(img::obj(kCameraMan))->set_mode(2);  // 0x00429860
                joan::orig::as_camera_zones(this)->enter(joan::orig::arg::mptr(static_cast<void*>(z)), pos);                // 0x004271f0 enter
                jo::self<jo::CamZones>(this)->current_zone = id;
            } else {
                joan::orig::as_camera_zones(this)->inside(joan::orig::arg::mptr(static_cast<void*>(z)), pos);  // 0x00426450 inside
            }
        } else if (jo::self<jo::CamZones>(this)->current_zone != 0) {
            joan::orig::as_camera_man(img::obj(kCameraMan))->set_mode(0);
            jo::self<jo::CamZones>(this)->current_zone = id;
            std::memcpy(jo::self<jo::CamZones>(this)->last_position, &pos, 12);
            return 0;
        }
        jo::self<jo::CamZones>(this)->current_zone = id;
        std::memcpy(jo::self<jo::CamZones>(this)->last_position, &pos, 12);
        return 1;
    }
};

#define C1(name, addr, fn) JOAN_REPLACE("game/camera1", name, addr, fn)
C1("CameraZone::update", 0x00425fc0, &H::zone_update);
#undef C1

}  // namespace game_camera
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using u8 = std::uint8_t;

struct H {

    // ------------------------------------------------------------------ 0x00426280
    void dtor_426280() {
        joan::orig::as_game_over(this)->release();            // 0x006b6f50
        joan::orig::as_widget(&jo::self<jo::GameOver>(this)->quit)->dtor_6211a0();  // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::GameOver>(this)->load)->dtor_6211a0();
        joan::orig::as_widget(this)->dtor_6211a0();
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::~Obj_426280", 0x00426280, &H::dtor_426280);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

// ---------------------------------------------------------------------- static initialisers
void init_pi(img::Addr g) {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(g) = static_cast<float>(t * 4.0);        // qword 0x9cf788 = 4.0 (exact)
}
static void JOAN_CDECL init_pi_a39c70() { init_pi(d::g_a39c70); }  // 0x00426300

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_pi_a39c70", 0x00426300, &init_pi_a39c70);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

// ---- static initialisers / free functions ----
using joan::orig::helpers::div_init;
void JOAN_CDECL init_426420() { div_init(d::g_a39c78, d::g_a39c70, d::flt_3); }

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("init_426420", 0x00426420, &init_426420);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_camzone.cpp (game/camera2)
namespace {
namespace game_camzone {

using joan::call::cdecl_call;

constexpr img::Addr kCameraMan(d::g_camera_man);

using joan::orig::helpers::at;

V3 load3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}
V3 sub(const V3& a, const V3& b) { return V3{a.x - b.x, a.y - b.y, a.z - b.z}; }
V3 normalized(V3 v) {
    import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&v));  // vector_normalize (in place)
    return v;
}
double length(V3 v) { return import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&v)); }
float dot(V3 a, V3 b) { return static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(&a), static_cast<void*>(&b))); }
bool equal_or_nan(float a, float b) { return !(a < b || a > b); }

constexpr float kDegToRad = 0.01745329238474369f;  // 0x9cf884

void* motion2() { return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(img::obj(kCameraMan))->motion(2)); }  // 0x00563c80
double crt_angle(float x) { return joan::orig::arg::as<double>(joan::orig::libc::asin_f(joan::orig::arg::f32(x))); }  // 0x00425610
double crt_atan2(float y, float x) { return joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(y), joan::orig::arg::f32(x))); }  // 0x00421020
float crt_abs(float x) { return joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(x))); }  // 0x00427d60 (stored directly)
double crt_abs_d(float x) { return joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(x))); }

JOAN_COUNTER(c_enter, "CameraZone::enter");
JOAN_COUNTER(c_inside, "CameraZone::inside");
JOAN_COUNTER(c_scripted, "CameraZone::scripted");

struct H {
    // Rail point i: +0x76 position, +0x82 tangent, +0x8e / +0x92 yaw / pitch, +0x96 / +0x9a /
    // +0x9e scripted extras.
    // CamTrack::keys[i], addressed as the original (z + i * sizeof(CamKey) + offsetof(CamTrack, keys)).
    static const jo::CamKey* rail_key(const std::uint8_t* z, std::int32_t i) {
        return reinterpret_cast<const jo::CamKey*>(z + i * static_cast<std::int32_t>(sizeof(jo::CamKey)) + static_cast<std::int32_t>(offsetof(jo::CamTrack, keys)));
    }
    static const float* point(const std::uint8_t* z, std::int32_t i) { return rail_key(z, i)->point; }
    static const float* tangent(const std::uint8_t* z, std::int32_t i) { return rail_key(z, i)->tangent; }
    V3 rail(const std::uint8_t* z, std::int32_t i, std::int32_t j, float t) {
        V3 out;
        joan::orig::as_camera_zones(this)->hermite(joan::orig::arg::mptr(static_cast<void*>(&out)), load3(point(z, i)), load3(tangent(z, i)), load3(point(z, j)), load3(tangent(z, j)), joan::orig::arg::f32(t));  // 0x004279b0 Hermite
        return out;
    }
    float lerp(float a, float b, float t) { return joan::orig::arg::as<float>(joan::orig::as_camera_zones(this)->lerp(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(t))); }  // 0x00427bb0
    float distance_error(const std::uint8_t* z, const V3& pos, const V3& p) {
        const float r = static_cast<float>(jo::as_cam_track(z)->view_distance);
        const float x = static_cast<float>(joan::hook::x87::Fpu{true}.sub(r, length(sub(pos, p))));
        return crt_abs(x);
    }
    // 0x004271f0 CameraZone::enter(zone, unit position): the rail point (parameter step
    // +0xac6) closest to the wanted distance (+0x60) whose view faces the unit becomes the
    // camera position (+0x1c / +0x20 segment, +0x24 parameter); the motion restarts there.
    void enter(std::uint8_t* z, V3 pos) {
        c_enter.hit();
        const joan::hook::x87::Fpu fpu{true};
        float state[10];
        joan::orig::as_camera_motion(motion2())->end(joan::orig::arg::mptr(static_cast<void*>(state)));  // 0x00423fe0
        V3 best{state[1], state[2], state[3]};
        V3 best_dir{global<float>(d::g_pos), global<float>(d::g_a69cdc), global<float>(d::g_a69ce0)};
        V3 prev{state[1], state[2], state[3]};
        const float step = jo::as_cam_track(z)->rail_step;
        float best_error = 10000.0f;
        bool found = false;
        for (std::int32_t i = 0; i < jo::as_cam_track(z)->key_count - 1; ++i) {
            for (float t = 0.0f; !(t > 1.0f); t = t + step) {
                const V3 p = rail(z, i, i + 1, t);
                const float error = distance_error(z, pos, p);
                V3 d = normalized(sub(p, prev));
                float yaw_deg, pitch_deg;
                if (jo::as_cam_track(z)->per_key_angles != 0) {
                    yaw_deg = lerp(rail_key(z, i)->yaw, rail_key(z, i + 1)->yaw, t);
                    pitch_deg = lerp(rail_key(z, i)->pitch, rail_key(z, i + 1)->pitch, t);
                } else {
                    yaw_deg = jo::as_cam_track(z)->yaw_deg;
                    pitch_deg = jo::as_cam_track(z)->pitch_deg;
                }
                const float pitch = static_cast<float>(fpu.add(crt_angle(-d.z), pitch_deg));
                const float yaw_off = yaw_deg * kDegToRad;
                const float yaw = static_cast<float>(fpu.add(crt_atan2(d.y, d.x), yaw_off));
                float m[16];
                joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::f32(yaw), joan::orig::arg::f32(pitch), joan::orig::arg::f32(0u), joan::orig::arg::f32(0u));  // 0x0060bd00 rotation
                joan::orig::matrix_axis_z(static_cast<void*>(&d), static_cast<void*>(m));  // 0x0060c220 forward
                const float facing = dot(d, normalized(sub(pos, p)));
                if (!(error > best_error) && !(facing < 0.0f || facing != facing)) {
                    best_error = error;
                    best = p;
                    best_dir = d;
                    jo::self<jo::CamZones>(this)->rail_segment = i;
                    jo::self<jo::CamZones>(this)->rail_segment_b = i + 1;
                    jo::self<jo::CamZones>(this)->rail_param = t;
                    found = true;
                }
                prev = p;
            }
        }
        (void)best_dir;
        if (!found) {
            jo::self<jo::CamZones>(this)->current_zone = 0;
            return;
        }
        state[1] = best.x;
        state[2] = best.y;
        state[3] = best.z;
        const V3 dir = normalized(sub(pos, best));
        const float pitch_raw = static_cast<float>(crt_angle(-dir.z));
        float yaw_deg, pitch_deg;
        if (jo::as_cam_track(z)->per_key_angles != 0) {
            yaw_deg = lerp(rail_key(z, jo::self<jo::CamZones>(this)->rail_segment)->yaw, rail_key(z, jo::self<jo::CamZones>(this)->rail_segment_b)->yaw, jo::self<jo::CamZones>(this)->rail_param);
            pitch_deg = lerp(rail_key(z, jo::self<jo::CamZones>(this)->rail_segment)->pitch, rail_key(z, jo::self<jo::CamZones>(this)->rail_segment_b)->pitch, jo::self<jo::CamZones>(this)->rail_param);
        } else {
            yaw_deg = jo::as_cam_track(z)->yaw_deg;
            pitch_deg = jo::as_cam_track(z)->pitch_deg;
        }
        state[4] = jo::as_cam_track(z)->face_unit != 0 ? static_cast<float>(crt_atan2(dir.y, dir.x)) : yaw_deg * kDegToRad;
        if (jo::as_cam_track(z)->free_pitch != 0) {
            state[5] = pitch_raw;
        } else if (jo::as_cam_track(z)->scale_pitch != 0) {
            const float scale = pitch_deg * kDegToRad;
            state[5] = static_cast<float>(fpu.mul(fpu.div(pitch_raw, crt_abs_d(pitch_raw)), scale));
        } else {
            state[5] = pitch_deg * kDegToRad;
        }
        state[6] = 0.0f;
        state[7] = 0.0f;
        joan::orig::as_camera_motion(motion2())->start(joan::orig::arg::mptr(static_cast<void*>(state)), joan::orig::arg::mptr(static_cast<void*>(state)));  // 0x00423ec0
    }

    // 0x00426450 CameraZone::inside(zone, unit position): moves the rail parameter one step
    // at a time (up to 100) backwards or forwards while the distance error decreases, then
    // retargets the motion.
    void inside(std::uint8_t* z, V3 pos) {
        c_inside.hit();
        const joan::hook::x87::Fpu fpu{true};
        float state[10];
        joan::orig::as_camera_motion(motion2())->end(joan::orig::arg::mptr(static_cast<void*>(state)));
        V3 best{state[1], state[2], state[3]};
        const float step = jo::as_cam_track(z)->rail_step;
        std::int16_t iterations = 0;
        float error = distance_error(z, pos, best);
        const V3 moving = normalized(sub(pos, load3(jo::self<jo::CamZones>(this)->last_position)));
        const float back_t = jo::self<jo::CamZones>(this)->rail_param - step > 0.0f ? jo::self<jo::CamZones>(this)->rail_param - step : 0.0f;
        const V3 back = rail(z, jo::self<jo::CamZones>(this)->rail_segment, jo::self<jo::CamZones>(this)->rail_segment_b, back_t);
        const float fwd_t = below_or_nan(step + jo::self<jo::CamZones>(this)->rail_param, 1.0f) ? step + jo::self<jo::CamZones>(this)->rail_param : 1.0f;
        const V3 fwd = rail(z, jo::self<jo::CamZones>(this)->rail_segment, jo::self<jo::CamZones>(this)->rail_segment_b, fwd_t);
        const V3 cur{state[1], state[2], state[3]};
        V3 back_dir = normalized(sub(back, cur));
        V3 fwd_dir = normalized(sub(fwd, cur));
        const V3 zero{global<float>(d::g_pos), global<float>(d::g_a69cdc), global<float>(d::g_a69ce0)};
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::vec_equal(static_cast<void*>(&back_dir), static_cast<const void*>(&zero))) != 0)  // 0x0060ced0
            back_dir = V3{-fwd_dir.x, -fwd_dir.y, -fwd_dir.z};
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::vec_equal(static_cast<void*>(&fwd_dir), static_cast<const void*>(&zero))) != 0)
            fwd_dir = V3{-back_dir.x, -back_dir.y, -back_dir.z};
        bool backwards = false;
        if (dot(moving, back_dir) > 0.0f) {
            const float d2 = dot(moving, fwd_dir);
            backwards = below_or_nan(d2, 0.0f);
        }
        if (backwards) {
            for (float t = jo::self<jo::CamZones>(this)->rail_param - step; iterations <= 100; t = t - step) {
                std::int32_t i = jo::self<jo::CamZones>(this)->rail_segment, j = jo::self<jo::CamZones>(this)->rail_segment_b;
                if (!(t > 0.0f)) {
                    if (i == 0) {
                        jo::self<jo::CamZones>(this)->rail_param = 0.0f;
                        break;
                    }
                    t = 1.0f - step;
                    --i;
                    --j;
                }
                const V3 p = rail(z, i, j, t);
                const float e = distance_error(z, pos, p);
                if (below_or_nan(error, e)) break;
                best = p;
                error = e;
                jo::self<jo::CamZones>(this)->rail_param = t;
                jo::self<jo::CamZones>(this)->rail_segment = i;
                jo::self<jo::CamZones>(this)->rail_segment_b = j;
                iterations = static_cast<std::int16_t>(iterations + 1);
            }
        } else {
            for (float t = step + jo::self<jo::CamZones>(this)->rail_param; iterations <= 100; t = t + step) {
                std::int32_t i = jo::self<jo::CamZones>(this)->rail_segment, j = jo::self<jo::CamZones>(this)->rail_segment_b;
                if (!below_or_nan(t, 1.0f)) {
                    if (j == jo::as_cam_track(z)->key_count - 1) {
                        jo::self<jo::CamZones>(this)->rail_param = 1.0f;
                        break;
                    }
                    t = step;
                    ++i;
                    ++j;
                }
                const V3 p = rail(z, i, j, t);
                const float e = distance_error(z, pos, p);
                if (below_or_nan(error, e)) break;
                best = p;
                jo::self<jo::CamZones>(this)->rail_param = t;
                error = e;
                jo::self<jo::CamZones>(this)->rail_segment = i;
                jo::self<jo::CamZones>(this)->rail_segment_b = j;
                iterations = static_cast<std::int16_t>(iterations + 1);
            }
        }
        state[1] = best.x;
        state[2] = best.y;
        state[3] = best.z;
        const V3 dir = normalized(sub(pos, V3{state[1], state[2], state[3]}));
        const float pitch_raw = static_cast<float>(crt_angle(-dir.z));
        float yaw_deg, pitch_deg;
        if (jo::as_cam_track(z)->per_key_angles != 0) {
            yaw_deg = lerp(rail_key(z, jo::self<jo::CamZones>(this)->rail_segment)->yaw, rail_key(z, jo::self<jo::CamZones>(this)->rail_segment_b)->yaw, jo::self<jo::CamZones>(this)->rail_param);
            pitch_deg = lerp(rail_key(z, jo::self<jo::CamZones>(this)->rail_segment)->pitch, rail_key(z, jo::self<jo::CamZones>(this)->rail_segment_b)->pitch, jo::self<jo::CamZones>(this)->rail_param);
        } else {
            yaw_deg = jo::as_cam_track(z)->yaw_deg;
            pitch_deg = jo::as_cam_track(z)->pitch_deg;
        }
        state[4] = jo::as_cam_track(z)->face_unit != 0 ? static_cast<float>(crt_atan2(dir.y, dir.x)) : yaw_deg * kDegToRad;
        if (jo::as_cam_track(z)->free_pitch != 0) {
            state[5] = pitch_raw;
        } else if (jo::as_cam_track(z)->scale_pitch != 0) {
            if (equal_or_nan(pitch_raw, 0.0f)) {
                state[5] = 0.0f;
            } else {
                const float scale = pitch_deg * kDegToRad;
                state[5] = static_cast<float>(fpu.mul(fpu.div(pitch_raw, crt_abs_d(pitch_raw)), scale));
            }
        } else {
            state[5] = pitch_deg * kDegToRad;
        }
        state[6] = 0.0f;
        state[7] = 0.0f;
        joan::orig::as_camera_motion(motion2())->retarget(joan::orig::arg::mptr(static_cast<void*>(state)));  // 0x00423f40 retarget
    }

    // 0x00427d90 CameraZone::scripted(zone): tracker +0x38 start / +0x3c end time, +0x40 key
    // base, +0x44 looping, +0x46 keys per ms, +0x4a hand back to motion 0. Rail keys +0x9e;
    // the camera height is kept above the terrain (0x004c7b70), the ground objects
    // (0x00469d10) and scene meshes hit by a vertical segment (smoothed by the frame time).
    void scripted(std::uint8_t* z) {
        c_scripted.hit();
        const joan::hook::x87::Fpu fpu{true};
        const std::uint32_t now = global<std::uint32_t>(d::g_now);
        if (now < jo::self<jo::CamZones>(this)->script_start) return;
        if (now > jo::self<jo::CamZones>(this)->script_end) {
            if (jo::self<jo::CamZones>(this)->looping == 0) {
                float current[10];
                joan::orig::as_camera_motion(motion2())->current(joan::orig::arg::mptr(static_cast<void*>(current)));  // 0x00423fb0
                jo::view<std::uint32_t>(jo::self<jo::CamZones>(this)->current_zone) = 0;
                jo::view<std::uint32_t>(jo::self<jo::CamZones>(this)->key_base) = 0;
                joan::orig::as_camera_man(img::obj(kCameraMan))->restore_mode();  // 0x004297d0 restore mode
                if (jo::self<jo::CamZones>(this)->hand_back != 0) joan::orig::as_camera_motion(global<void*>(d::g_a48cc4))->retarget(joan::orig::arg::mptr(static_cast<void*>(current)));  // 0x00423f40
                return;
            }
            jo::self<jo::CamZones>(this)->script_end = jo::self<jo::CamZones>(this)->script_end - jo::self<jo::CamZones>(this)->script_start + now;
            jo::self<jo::CamZones>(this)->script_start = now;
        }
        const std::int32_t key =
            joan::hook::x87::ftol(fpu.mul(static_cast<double>(now - jo::self<jo::CamZones>(this)->script_start), jo::self<jo::CamZones>(this)->keys_per_ms)) + jo::self<jo::CamZones>(this)->key_base;
        std::int32_t i = 0;
        for (; i < jo::as_cam_track(z)->key_count; ++i)
            if (!(rail_key(z, i)->time < key)) break;
        if (rail_key(z, i)->time < key) return;
        float t = 0.0f;
        if (i > 0)
            t = static_cast<float>(fpu.div(static_cast<double>(key - rail_key(z, i - 1)->time),
                                           static_cast<double>(rail_key(z, i)->time - rail_key(z, i - 1)->time)));
        joan::orig::as_camera_man(img::obj(kCameraMan))->set_mode(2);  // 0x00429860
        float s[10];
        joan::orig::as_camera_motion(motion2())->end(joan::orig::arg::mptr(static_cast<void*>(s)));  // 0x00423fe0
        if (i > 0) {
            const V3 p = rail(z, i - 1, i, t);
            s[1] = p.x;
            s[2] = p.y;
            s[3] = p.z;
            s[4] = lerp(rail_key(z, i - 1)->yaw, rail_key(z, i)->yaw, t) * kDegToRad;
            s[5] = lerp(rail_key(z, i - 1)->pitch, rail_key(z, i)->pitch, t) * kDegToRad;
            s[6] = lerp(rail_key(z, i - 1)->roll, rail_key(z, i)->roll, t) * kDegToRad;
            s[7] = lerp(rail_key(z, i - 1)->zoom, rail_key(z, i)->zoom, t);
        } else {
            std::memcpy(&s[1], point(z, 0), 12);
            s[4] = rail_key(z, 0)->yaw * kDegToRad;
            s[5] = rail_key(z, 0)->pitch * kDegToRad;
            s[6] = rail_key(z, 0)->roll * kDegToRad;
            s[7] = rail_key(z, 0)->zoom;
        }
        float box[6] = {s[1], s[2], s[3], s[1], s[2], s[3]};
        std::int32_t count = 0;
        std::uint8_t** const cells = joan::orig::arg::as<std::uint8_t**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x00429090
        if (count != 0) {
            std::uint8_t* const grid = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_world_cell(cells[0])->grid(joan::orig::arg::i16(0)));  // 0x005600c0
            bool have = false;
            float height = 0.0f;
            if (grid != nullptr) {
                const float gx = static_cast<float>(fpu.div(fpu.sub(box[0], static_cast<double>(jo::as_world_cell(cells[0])->origin_x)),
                                                            static_cast<double>(jo::as_path_space(grid)->cell_w)));
                const float gy = static_cast<float>(fpu.div(fpu.sub(box[1], static_cast<double>(jo::as_world_cell(cells[0])->origin_y)),
                                                            static_cast<double>(jo::as_path_space(grid)->cell_h)));
                const std::int32_t ix = joan::hook::x87::ftol(gx);
                const std::int32_t iy = joan::hook::x87::ftol(gy);
                have = true;
                const std::int32_t even = (ix + iy) % 2 == 0 ? 1 : 0;
                height = joan::orig::arg::as<float>(joan::orig::as_terrain(jo::view<void*>(jo::as_path_space(grid)->grid))->height_4c7b70(joan::orig::arg::f32(gx), joan::orig::arg::f32(gy), static_cast<char>(joan::orig::arg::i32(even))));  // 0x004c7b70 terrain height
            }
            std::uint8_t* const ground = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_waters(img::obj(d::g_water_array))->find_at(joan::orig::arg::mptr(static_cast<void*>(&s[1])), 0));  // 0x00469d10
            if (ground != nullptr) {
                if (have) {
                    if (jo::as_water(ground)->box[2] > height) height = jo::as_water(ground)->box[2];  // the region box z
                } else {
                    height = jo::as_water(ground)->box[2];
                    have = true;
                }
            }
            std::int32_t node_count = 0;
            void** const nodes = joan::orig::arg::as<void**>(joan::orig::as_scene(jo::as_world(global<void*>(d::g_world))->scene)->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &node_count));  // 0x005ef610
            float seg[6] = {box[0], box[1], box[2] + 1000.0f, box[0], box[1], box[2] - 1000.0f};
            std::uint8_t* hit = nullptr;
            float hit_t = 0.0f;
            for (std::int32_t n = node_count - 1; n >= 0; --n) {
                std::uint8_t* const node = static_cast<std::uint8_t*>(nodes[n]);
                if (joan::orig::arg::as<std::int32_t>(joan::orig::segment_hits_box(static_cast<void*>(seg), static_cast<void*>(jo::as_scene_s(node)->desc))) == 0) continue;  // 0x0060dfd0
                for (std::uint8_t* a = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene_node(node)->attachments()); a != nullptr; a = reinterpret_cast<std::uint8_t*>(jo::as_actor(a)->scene_next)) {  // 0x005efff0
                    std::uint8_t* const mesh = jo::view<std::uint8_t*>(jo::as_actor(a)->plane);
                    if (jo::as_actor(a)->kind != 1) continue;
                    if (mesh != nullptr && static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::as_plane(mesh)->vertex_format)) == 7) continue;
                    const std::uint8_t* const def =
                        joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(img::obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_map_object(jo::as_actor(a)->owner)->type_id)));  // 0x00425ce0
                    if (static_cast<std::int8_t>(jo::as_item_def(def)->has_path) != 2) continue;
                    if (mesh == nullptr) continue;
                    float abox[6];
                    joan::orig::as_actor(a)->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(abox)), 1u);  // 0x005acc80
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::segment_hits_sphere(joan::orig::arg::mptr(static_cast<void*>(seg)), joan::orig::arg::mptr(static_cast<void*>(abox)))) == 0) continue;  // 0x0060e3f0
                    float tt = 0.0f;
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->segment_hit(&tt, joan::orig::arg::mptr(static_cast<void*>(seg)))) != 0) {  // 0x005d4a80 ray hit
                        if (hit == nullptr || hit_t > tt) {
                            hit_t = tt;
                            hit = a;
                        }
                    }
                }
            }
            if (hit != nullptr) {
                float at_hit[3];
                joan::orig::segment_point_at(static_cast<void*>(at_hit), static_cast<void*>(seg), joan::orig::arg::f32(hit_t));  // 0x0060de40
                if (have) {
                    if (at_hit[2] > height) height = at_hit[2];
                } else {
                    height = at_hit[2];
                    have = true;
                }
            }
            if (have) {
                if (height > s[3]) {
                    s[3] = height;
                } else {
                    const double d = fpu.mul(height - s[3], static_cast<double>(global<std::int32_t>(d::g_frame_step_ms)));
                    s[3] = static_cast<float>(fpu.add(fpu.div(d, 1000.0), s[3]));
                }
            }
        }
        joan::orig::as_camera_motion(motion2())->retarget(joan::orig::arg::mptr(static_cast<void*>(s)));  // 0x00423f40
    }

    static bool below_or_nan(float a, float b) { return !(a >= b); }
};

#define C2(name, addr, fn) JOAN_REPLACE("game/camera2", name, addr, fn)
C2("CameraZone::enter", 0x004271f0, &H::enter);
C2("CameraZone::inside", 0x00426450, &H::inside);
C2("CameraZone::scripted", 0x00427d90, &H::scripted);
#undef C2

}  // namespace game_camzone
}  // namespace

// ---------------------------------------- from modules/game_b101_01.cpp (game/b101_01)
namespace {
namespace game_b101_01 {

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::orig::helpers::at;

void* const kLists = obj(d::g_particle_effect_array);

JOAN_COUNTER(c_release, "Unit::release_426f30");

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return *reinterpret_cast<T*>(reinterpret_cast<std::uint8_t*>(this) + off);
    }

    // ------------------------------------------------------------------ 0x00426f30
    void release_slots(i32 base) {  // base: byte offset of two of MapObject::sound_slots (typed layout)
        for (i32 i = 0; i < 2; ++i) {
            if (joan::orig::as_dynarray(kLists)->empty_at(static_cast<i32>(f<i16>(base + i * static_cast<i32>(sizeof(i16))))) != 0) continue;  // 0x0041bd90
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kLists)->ptr_at(static_cast<i32>(f<i16>(base + i * static_cast<i32>(sizeof(i16))))));  // 0x0041bd60
            if (jo::self<jo::MapObject>(this)->actor != nullptr && jo::as_model_obj(o)->owner == jo::self<jo::MapObject>(this)->actor) {  // the registry records are ModelObj
                joan::orig::as_effect(o)->set_owner(joan::orig::arg::mptr(0), joan::orig::arg::mptr(0));  // 0x0041b0d0
            }
        }
    }
    void release_426f30() {
        c_release.hit();
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::MapObject>(this)->active)) == 0) return;
        jo::self<jo::MapObject>(this)->active = 0;
        jo::self<jo::MapObject>(this)->refresh_depth = static_cast<u8>(jo::self<jo::MapObject>(this)->refresh_depth + 1);
        virtual_call(this, 0x30);
        release_slots(static_cast<i32>(offsetof(jo::MapObject, sound_slots)));  // 0x96 on x86
        release_slots(static_cast<i32>(offsetof(jo::MapObject, sound_slots) + 2 * sizeof(i16)));  // 0x9a
        if (jo::self<jo::MapObject>(this)->scene != nullptr) {
            for (i32 i = 0; i < 2; ++i) {
                if (joan::orig::as_dynarray(kLists)->empty_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i])) != 0) continue;  // 0x0041bd90
                u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kLists)->ptr_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i])));  // 0x0041bd60
                void* const id = joan::orig::arg::as<void*>(joan::orig::as_effect_state(&jo::as_model_obj(o)->fx)->object());  // 0x0041b280 (ModelObj::fx is at +0x2b on x86)
                joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->scene))->detach(joan::orig::arg::mptr(id));        // 0x005efdb0
            }
            if (jo::self<jo::MapObject>(this)->actor_b != nullptr) joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->scene))->detach(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor_b));
            if (jo::self<jo::MapObject>(this)->actor != nullptr) joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->scene))->detach(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor));
            jo::self<jo::MapObject>(this)->scene = nullptr;
        }
        virtual_call(this, 0x14);
        jo::self<jo::MapObject>(this)->refresh_depth = static_cast<u8>(jo::self<jo::MapObject>(this)->refresh_depth - 1);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_01", name, addr, fn)
B("Unit::release_426f30", 0x00426f30, &H::release_426f30);
#undef B

}  // namespace game_b101_01
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

// eax residue (this / a track pointer) of 0x00427170 and 0x00428910: no caller in src uses the slots' results
u32 addr(const void* p) { return joan::call::register_low_bits(p); }

constexpr Fpu kFpu{true};

JOAN_COUNTER(c_dtor_427830, "Big::dtor_427830");
JOAN_COUNTER(c_start_428910, "Anim::start_428910");

struct H {
    // ---------------------------------------------------------------- 0x00427170
    u32 clear_427170() {
        void* p = jo::view<void*>(jo::self<jo::Cell>(this)->units_head);
        while (p) {
            void* const next = jo::as_unit(p)->cell_next;
            joan::orig::as_unit(this)->unlink_owner(joan::orig::arg::mptr(p));  // 0x0043eb10
            p = next;
        }
        jo::self<jo::Cell>(this)->units_head = nullptr;
        jo::self<jo::Cell>(this)->units_tail = nullptr;
        return addr(this);
    }

    // ---------------------------------------------------------------- 0x00427830
    void dtor_427830() {
        c_dtor_427830.hit();
        joan::orig::as_combat_screen(this)->release();                // 0x006ef7c0
        joan::orig::as_scroll_bar(&jo::self<jo::CombatScreen>(this)->scroll_bar)->destruct();   // 0x007617c0
        joan::orig::as_widget(&jo::self<jo::CombatScreen>(this)->buttons_19)->dtor_6211a0();   // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::CombatScreen>(this)->buttons_18)->dtor_6211a0();
        joan::orig::as_widget(&jo::self<jo::CombatScreen>(this)->buttons_17)->dtor_6211a0();
        joan::orig::as_widget(&jo::self<jo::CombatScreen>(this)->buttons_16)->dtor_6211a0();
        // eh vector destructor iterator (stdcall)
        joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CombatScreen>(this)->buttons_13)), static_cast<std::uint32_t>(sizeof(jo::Button)), 3, fn::code_value(fn::Widget_dtor_6211a0));
        joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CombatScreen>(this)->buttons_3)), static_cast<std::uint32_t>(sizeof(jo::Button)), 10, fn::code_value(fn::Widget_dtor_6211a0));
        joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CombatScreen>(this)->buttons_0)), static_cast<std::uint32_t>(sizeof(jo::Button)), 3, fn::code_value(fn::Widget_dtor_6211a0));
    }

    // ---------------------------------------------------------------- 0x00428910
    u32 start_428910(u32 id, u32 start, u32 base) {
        c_start_428910.hit();
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CamZones>(this)->tracks))->find(joan::orig::arg::str(id)));                         // 0x00429c70
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::CamZones>(this)->tracks))->get(joan::orig::arg::i16(static_cast<i16>(r))));       // 0x00428b30
        if (jo::as_cam_track(p)->kind != 1) return addr(p) & 0xffff0000u;  // xor ax,ax
        jo::view<u32>(jo::self<jo::CamZones>(this)->current_zone) = r;
        jo::self<jo::CamZones>(this)->script_start = start;
        jo::view<u32>(jo::self<jo::CamZones>(this)->key_base) = base;
        const u32 idx = (jo::view<u32>(jo::as_cam_track(p)->key_count) - 1u) * static_cast<u32>(sizeof(jo::CamKey));  // keys[key_count - 1]
        const i32 v = static_cast<i32>(jo::view<u32>(reinterpret_cast<const jo::CamKey*>(p + idx + offsetof(jo::CamTrack, keys))->time) - jo::view<u32>(jo::self<jo::CamZones>(this)->key_base));
        const double q = kFpu.div(static_cast<double>(v), static_cast<double>(jo::self<jo::CamZones>(this)->keys_per_ms));
        jo::self<jo::CamZones>(this)->script_end = start + static_cast<u32>(ftol(q));
        return (addr(this) & 0xffff0000u) | 1u;  // mov ax,1
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("List::clear_427170", 0x00427170, &H::clear_427170);
B("Big::dtor_427830", 0x00427830, &H::dtor_427830);
B("Anim::start_428910", 0x00428910, &H::start_428910);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
namespace {
namespace game_b86_05 {

using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;  // operator delete

using joan::orig::helpers::at;
std::uint8_t* bytes(const void* p, std::int32_t offset = 0) {
    return static_cast<std::uint8_t*>(const_cast<void*>(p)) + offset;
}

JOAN_COUNTER(c_motion_start, "CamMotion::start");

struct H {
    // ---- 0x00428850 CamMotion::start(name, time) -> word -------------------------------------
    std::int16_t motion_start(const char* name, std::uint32_t time) {
        c_motion_start.hit();
        const std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CamZones>(this)->tracks))->find(joan::orig::arg::str(name)));  // 0x00429c70
        std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::CamZones>(this)->tracks))->get(joan::orig::arg::i16(static_cast<std::int16_t>(id))));  // 0x00428b30
        if (jo::view<std::int32_t>(jo::as_cam_track(rec)->kind) != 1) return 0;
        jo::self<jo::CamZones>(this)->current_zone = id;
        jo::self<jo::CamZones>(this)->script_start = time;
        jo::view<std::uint32_t>(jo::self<jo::CamZones>(this)->key_base) = 0;
        const std::int32_t last = reinterpret_cast<const jo::CamKey*>(rec + (jo::as_cam_track(rec)->key_count - 1) * static_cast<std::int32_t>(sizeof(jo::CamKey)) + offsetof(jo::CamTrack, keys))->time;  // keys[key_count - 1]
        const joan::hook::x87::Fpu fpu{true};
        const std::int32_t t = joan::hook::x87::ftol(fpu.div(static_cast<double>(last), static_cast<double>(jo::self<jo::CamZones>(this)->keys_per_ms)));
        jo::self<jo::CamZones>(this)->script_end = time + static_cast<std::uint32_t>(t);
        return 1;
    }

    // ---- 0x00427930 vector deleting destructor (0x38-byte elements, dtor 0x0054ac60). ret 4.
    void* vector_delete(std::uint32_t flags) {
        if ((flags & 2) != 0) {
            joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(this)), static_cast<std::uint32_t>(sizeof(jo::BoxTree)), joan::orig::arg::i32(at<std::int32_t>(this, -4))  /* x64: the new[] count cookie before the array */, fn::code_value(fn::fn_0x0054ac60_clear_34_4));  // eh vector destructor iterator
            void* const block = bytes(this, -4);
            if ((flags & 1) != 0) game_delete(block);
            return block;
        }
        joan::orig::as_box_tree(this)->reset_leaf();  // 0x0054ac60
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("CamMotion::start", 0x00428850, &H::motion_start);
B86("Obj427930::vector_delete", 0x00427930, &H::vector_delete);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_camera.cpp (game/camera1)
namespace {
namespace game_camera_2 {

JOAN_COUNTER(c_hermite2, "hermite_b");

struct H {
    float lerp(float a, float b, float t) { return (b - a) * t + a; }  // 0x00427bb0
    // 0x004279b0 Hermite(out, p0, m0, p1, m1, t).
    void* hermite(void* out, V3 p0, V3 m0, V3 p1, V3 m1, float t) {
        c_hermite2.hit();
        const float s = t * t;
        const float c = s * t;
        const float k_p1 = -2.0f * c + 3.0f * s;
        const float k_m1 = c - s;
        const float k_m0 = (c - 2.0f * s) + t;
        const float k_p0 = (2.0f * c - 3.0f * s) + 1.0f;
        const V3 a{k_p1 * p1.x, k_p1 * p1.y, k_p1 * p1.z};
        const V3 b{k_m1 * m1.x, k_m1 * m1.y, k_m1 * m1.z};
        const V3 d{k_m0 * m0.x, k_m0 * m0.y, k_m0 * m0.z};
        const V3 e{k_p0 * p0.x, k_p0 * p0.y, k_p0 * p0.z};
        V3 r{e.x + d.x, e.y + d.y, e.z + d.z};
        r = V3{r.x + b.x, r.y + b.y, r.z + b.z};
        r = V3{r.x + a.x, r.y + a.y, r.z + a.z};
        std::memcpy(out, &r, sizeof r);
        return out;
    }
};

#define C1(name, addr, fn) JOAN_REPLACE("game/camera1", name, addr, fn)
C1("lerp_b", 0x00427bb0, &H::lerp);
C1("hermite_b", 0x004279b0, &H::hermite);
#undef C1

}  // namespace game_camera_2
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::orig::helpers::at;

static_assert(sizeof(V3) == 12, "V3");

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_item_res);

JOAN_COUNTER(c_can_target, "Unit::can_target");

i32 unit_invalid(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------------ 0x00427b50
    void set_name_index(void* name) {
        if (name == nullptr) return;
        if (jo::self<jo::Unit>(this)->cell == nullptr || jo::view<void*>(jo::as_world_cell(jo::self<jo::Unit>(this)->cell)->tracks) == nullptr) return;
        void* const list = joan::orig::arg::as<void*>(joan::orig::as_name_record(jo::view<void*>(jo::as_world_cell(jo::self<jo::Unit>(this)->cell)->tracks))->get_c());  // 0x00453cc0
        jo::view<i32>(jo::self<jo::Unit>(this)->name_index) = joan::orig::arg::as<i32>(joan::orig::as_name_list(list)->find_4ec320(joan::orig::arg::str(name)));                             // 0x004ec320
    }

    // ------------------------------------------------------------------------ 0x00428600
    void* vcall_4c() {
        const std::uintptr_t fn = reinterpret_cast<const std::uintptr_t*>(jo::self<jo::MapObject>(this)->vtable)[0x4c / 4];  // vtable slot 19
        return thiscall<void*>(fn, this);
    }
    bool can_target(i32 id) {
        c_can_target.hit();
        if (unit_invalid(id) != 0) return false;
        u8* const unit = unit_get(id);
        if (static_cast<i32>(jo::view<i8>(jo::as_unit(unit)->in_scene)) == 0) return false;
        V3 upos;
        joan::orig::as_unit(unit)->position(&upos);  // 0x004fedb0
        if (static_cast<i32>(jo::self<jo::MapObject>(this)->side) != 0 &&
            static_cast<i32>(jo::self<jo::MapObject>(this)->side) != static_cast<i32>(jo::as_unit(unit)->side)) {
            if (static_cast<i32>(jo::self<jo::MapObject>(this)->class_id) != 1) return false;
            const float a = joan::orig::arg::as<float>(joan::orig::as_map_object(this)->heading_to(&upos));  // 0x0041af80
            if (a > -90.0f && a < 90.0f) return false;
            if (jo::as_gate(vcall_4c())->max_hp <= 0) return false;  // vtable slot 0x4c returns the Gate (max_hp at +0xc6 on x86)
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::self<jo::MapObject>(this)->type_id)));  // 0x00425ce0
            if (jo::view<i32>(jo::as_item_def(type)->struction_type) == 5) return false;
        }
        if (static_cast<i32>(jo::self<jo::MapObject>(this)->class_id) == 1 && jo::as_gate(vcall_4c())->max_hp > 0 &&
            jo::as_gate(vcall_4c())->hp == 0)  // the Gate: hp +0xc2, max_hp +0xc6 on x86
            return false;
        V3 mine;
        joan::orig::as_map_object(this)->position(&mine);  // 0x00499930
        V3 tmp;
        V3* const d = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &upos, &mine));  // 0x0060cc90 (a - b)
        V3 diff = *d;
        const float lsq = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(&diff)));  // length_sq
        if (lsq > global<float>(d::flt_40000)) return false;  // 40000
        return true;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Unit::set_name_index", 0x00427b50, &H::set_name_index);
B93("Unit::can_target", 0x00428600, &H::can_target);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b111_09.cpp (game/b111_09)
namespace {
namespace game_b111_09 {

using joan::call::cdecl_call;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

struct H {
    // ------------------------------------------------------------------ 0x00427be0
    float dist_to_line(Vec3 a, Vec3 b, Vec3 c) {
        Vec3 tmp;  // ebp-0x34
        const Vec3* const r = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &a, &b));  // 0x0060cc90
        Vec3 dv = *r;                                                       // ebp-0x10
        const float lsq = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(&dv)));  // length_sq
        const float dot = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<const void*>(&dv),
                                             static_cast<const void*>(&c)));  // dot_product
        const float p = joan::orig::arg::as<float>(joan::orig::libc::pow_f(joan::orig::arg::f32(dot), 2.0f));           // 0x00428810 pow
        const double csq = import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(&c));  // st0 for fdivr
        const float q = static_cast<float>(Fpu{true}.div(static_cast<double>(p), csq));
        const float d = lsq - q;
        return static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(d)))));  // sqrt
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_09", name, addr, fn)
B("Geo::dist_to_line_427be0", 0x00427be0, &H::dist_to_line);
#undef B

}  // namespace game_b111_09
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x00427c90
    u32 dtor_427c90() {
        joan::orig::as_map_object(this)->set_collision(0);           // 0x00493060
        return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->release_4251b0());  // 0x004251b0
    }
    // ------------------------------------------------------------------ 0x00428a20
    void* set_3c_428a20(i32 x) {
        const i32 d = static_cast<i32>(static_cast<u32>(x) - jo::view<u32>(jo::self<jo::CamZones>(this)->key_base));
        const Fpu fpu{true};
        const i32 q = ftol(fpu.div(static_cast<double>(d), static_cast<double>(jo::self<jo::CamZones>(this)->keys_per_ms)));
        jo::self<jo::CamZones>(this)->script_end = jo::self<jo::CamZones>(this)->script_start + static_cast<u32>(q);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Unit::dtor_427c90", 0x00427c90, &H::dtor_427c90);
B("Obj::set_3c_428a20", 0x00428a20, &H::set_3c_428a20);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18_3 {

struct H {
    void set_id(std::uint32_t id) {  // 0x00427cc0
        if (static_cast<std::int32_t>(static_cast<std::int16_t>(id)) != static_cast<std::int32_t>(jo::self<jo::CamZones>(this)->enabled)) {
            jo::view<std::uint32_t>(jo::self<jo::CamZones>(this)->current_zone) = 0;
        }
        jo::view<std::uint16_t>(jo::self<jo::CamZones>(this)->enabled) = static_cast<std::uint16_t>(id);
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("Obj427cc0::set_id", 0x00427cc0, &H::set_id);
#undef B82

}  // namespace game_b82_18_3
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

struct H {
    // 0x00427d10 (short return; callers movsx ax)
    std::int16_t slot_free() {
        if (jo::self<jo::CamZones>(this)->current_zone == 0) return 0;
        // push edx after mov dx,[+0x18]: the callee only reads the low word (movsx).
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::CamZones>(this)->tracks))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::view<std::uint16_t[2]>(jo::self<jo::CamZones>(this)->current_zone)[0]))));  // 0x00428b30
        if (jo::view<std::int32_t>(jo::as_cam_track(e)->kind) != 0) return 0;
        return 1;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj427::slot_free", 0x00427d10, &H::slot_free);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units {

template <class R>
R call_virtual(void* self, std::int32_t index, std::uint32_t arg) {
    using Fn = R (Obj::*)(std::uint32_t);
    JOAN_MEMBER_POINTER_CHECK(Fn);
    Fn method{};
    std::memcpy(&method, &joan::call::vtable_of(self)[index], sizeof(void*));
    return (static_cast<Obj*>(self)->*method)(arg);
}

struct H {
    void plate_delete() {  // 0x004287b0
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_) == nullptr) return;
        joan::orig::as_unit_draw(this)->detach_a();  // 0x00423540
        void* const p = jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_);
        if (p != nullptr) call_virtual<void*>(p, 0, 1u);  // scalar deleting destructor
        jo::self<jo::UnitDraw>(this)->plate_ = nullptr;
    }
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("UnitDraw::plate_delete", 0x004287b0, &H::plate_delete);
#undef U1

}  // namespace game_units
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

struct H {
    // ------------------------------------------------------------------------ 0x004289b0
    u32 vfn_4289b0() {
        joan::orig::as_unit(this)->create_sprite();           // 0x0042f210
        return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->set_collision(1));  // 0x00493060
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("Obj::vfn_4289b0", 0x004289b0, &H::vfn_4289b0);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
namespace {
namespace game_b84_03 {

struct H {
    // ------------------------------------------------------------------ 0x00428aa0
    std::int16_t is_type1() {
        if (jo::view<std::uint32_t>(jo::self<jo::CameraZone>(this)->current_zone) == 0) return 0;
        std::uint8_t* const t = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::CameraZone>(this)->tracks))->get(joan::orig::arg::i16(jo::view<std::int16_t[2]>(jo::self<jo::CameraZone>(this)->current_zone)[0])));  // 0x00428b30
        return jo::view<std::int32_t>(jo::as_cam_track(t)->kind) == 1 ? 1 : 0;
    }

    // ------------------------------------------------------------------ 0x004289e0
    H* restart() {
        jo::self<jo::CameraZone>(this)->script_end = global<std::uint32_t>(d::g_now);
        jo::self<jo::CameraZone>(this)->looping = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("Track::is_type1", 0x00428aa0, &H::is_type1);
B("Track::restart", 0x004289e0, &H::restart);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

struct H {
    // ------------------------------------------------------------------ 0x00428a70
    H* set_46(u32 v) {
        jo::view<u32>(jo::self<jo::CamZones>(this)->keys_per_ms) = v;
        return this;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::set_46", 0x00428a70, &H::set_46);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

constexpr fn::Code kListGet = fn::List_get_41b3f0;     // 0x0041b3f0

void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

struct H {
    std::int32_t list_empty_at(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }  // 0x0041bd90 / 0x00428b70 / 0x00487bb0

    void set_first(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::CamZones>(this)->unit_id) = v; }  // 0x00428b00
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("List::empty_at_b", 0x00428b70, &H::list_empty_at);
FX1("set_first", 0x00428b00, &H::set_first);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

struct H {
    std::uint8_t* track(std::int16_t i) { return reinterpret_cast<std::uint8_t*>(&jo::self<jo::CameraTracks>(this)->tracks[static_cast<std::int32_t>(i) - 1]); }  // 0x00428b30
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Tracks::get", 0x00428b30, &H::track);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

using joan::call::cdecl_call;

using joan::call::cdecl_call;

// ---- CRT initialiser wrappers: call the body directly, return its eax ----
template <fn::Code Body>
u32 JOAN_CDECL init_wrapper() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("init_0x00428ba0", 0x00428ba0, &init_wrapper<fn::init_a33698>);
B("init_0x00428c00", 0x00428c00, &init_wrapper<fn::init_a3369c>);
B("init_0x00428c50", 0x00428c50, &init_wrapper<fn::init_a336a0>);
B("init_0x00428ca0", 0x00428ca0, &init_wrapper<fn::init_a336a4>);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------------- static inits
using joan::orig::helpers::pi_from_atan;
void JOAN_CDECL init_a33698() { global<float>(d::g_a33698) = pi_from_atan(); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("init_a33698", 0x00428bc0, &init_a33698);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

static void JOAN_CDECL init_a3369c() { div_global(d::g_a3369c, d::g_a33698, d::flt_2); }  // 0x00428c20

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("init_a3369c", 0x00428c20, &init_a3369c);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01_2 {

static void JOAN_CDECL init_a336a0() { div_global(d::g_a336a0, d::g_a33698, d::flt_3); }  // 0x00428c70 (/3)

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_a336a0", 0x00428c70, &init_a336a0);
#undef B97

}  // namespace game_b97_01_2
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01_2 {

void JOAN_CDECL init_a336a4() { global<float>(d::g_a336a4) = global<float>(d::g_a33698) / global<float>(d::flt_4); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("init_a336a4", 0x00428cc0, &init_a336a4);
#undef B95

}  // namespace game_b95_01_2
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
//
//    - destroy helpers 0x004866b0 / 0x004eb570 / 0x00425ef0, scalar deleting destructors
//      0x00416640 / 0x00417250.
namespace {
namespace game_b83_07_2 {

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x00425ef0
    std::uintptr_t destroy_12() {
        if (jo::self<jo::CamZones>(this)->tracks == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CamZones>(this)->tracks))->free();  // 0x00428d50
        void* const p = jo::view<void*>(jo::self<jo::CamZones>(this)->tracks);
        std::uintptr_t r = 0;
        if (p != nullptr) r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_cam_tracks(p)->destroy(1u));  // 0x00425f70
        jo::self<jo::CamZones>(this)->tracks = nullptr;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Obj::destroy_12", 0x00425ef0, &H::destroy_12);
#undef B

}  // namespace game_b83_07_2
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
//
//    - CamZones::CamZones 0x00425dc0 (memset 0x4c), CamZones::init 0x00425e20,
//      CamZones::load 0x00426330 (path), Tracks::Tracks 0x004eb4f0 (memset 0x10),
//      Tracks::init 0x004eb550 (empty), Tracks::load 0x004eb640 (path).
namespace {
namespace game_leaves82b_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;
struct Vec12 {  // 12-byte struct passed by value: three floats (like CamRes::load in camres.cpp)
    float a, b, c;
};
static_assert(sizeof(Vec12) == 12, "Vec12");

JOAN_COUNTER(c_zones_load, "CamZones::load");

using joan::orig::helpers::game_new;      // operator new

struct H {

    // 0x00426330 CamZones::load(path): old +0x12 cleared (0x00428d50) and deleted, a new one
    // loaded with the origin +6 (12 bytes by value).
    std::uint32_t zones_load(const char* path) {
        c_zones_load.hit();
        if (jo::view<void*>(jo::self<jo::CamZones>(this)->tracks) != nullptr) {
            joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CamZones>(this)->tracks))->free();  // 0x00428d50
            void* const old = jo::view<void*>(jo::self<jo::CamZones>(this)->tracks);
            if (old != nullptr) joan::orig::as_cam_tracks(old)->destroy(1u);  // 0x00425f70 delete
        }
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::CamRes)));  // 9 on x86: CamTracks + CamRes::loaded (+8)
        void* const p = mem != nullptr ? joan::orig::as_cam_tracks(mem)->construct() : nullptr;  // 0x00428cf0
        jo::view<void*>(jo::self<jo::CamZones>(this)->tracks) = p;
        Vec12 origin;
        std::memcpy(&origin, jo::self<jo::CamZones>(this)->origin, 12);
        joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CamZones>(this)->tracks))->load(path, origin);  // 0x00428de0
        return 0;  // callers discard the result
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("CamZones::load", 0x00426330, &H::zones_load);
#undef L82

}  // namespace game_leaves82b_2
}  // namespace

// ---------------------------------------- from modules/game_b115_03.cpp (game/b115_03)
//
//  * module game/b115_03 (cdecl, ret 0; the other wrappers of the module are in camman.cpp):
//      crt_fabs  0x00427d60 (float)            -> CRT fabs  0x007b1f6b
//      crt_pow   0x00428810 (float x, float y) -> CRT pow   0x007b2719
//
// MSVC 6 /Od frames: each float argument `fld dword; fstp qword` (exact), call the CRT, `fst dword [ebp-4]` (dead
// store, no pop), return the CRT's st0. The C++ returns the CRT result as the double its slot call gives (the
// facade of the wrapper returns double too); re/p1/gamefloat compares both with the original CRT code.
namespace {
namespace game_b115_03 {

double JOAN_CDECL crt_1f6b(float x) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(x)))); }  // 0x00427d60
double JOAN_CDECL crt_pow(float x, float y) {  // 0x00428810
    return joan::orig::arg::as<double>(joan::orig::libc::pow(joan::orig::arg::f64(static_cast<double>(x)), joan::orig::arg::f64(static_cast<double>(y))));
}

#define R(name, addr, fn) JOAN_REPLACE("game/b115_03", name, addr, fn)
R("crt_7b1f6b", 0x00427d60, &crt_1f6b);
R("crt_pow", 0x00428810, &crt_pow);
#undef R

}  // namespace game_b115_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::CameraZones::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::zones_ctor>(this)); }  // 0x425dc0
auto joan::orig::CameraZones::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_18::H::fwd_425df0>(this)); }  // 0x425df0
auto joan::orig::CameraZones::init() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b::H::zones_init>(this)); }  // 0x425e20
auto joan::orig::CameraZones::free_tracks() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b83_07_2::H::destroy_12>(this)); }  // 0x425ef0
auto joan::orig::CamTracks::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18_2::H::sdd_425f70>(this, a0)); }  // 0x425f70
auto joan::orig::CameraZones::update() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_camera::H::zone_update>(this)); }  // 0x425fc0
auto joan::orig::init_pi_a39c70() -> void { joan::bind::free<&::game_b97_01::init_pi_a39c70>(); }  // 0x426300
auto joan::orig::CameraZones::load(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b_2::H::zones_load>(this, a0)); }  // 0x426330
auto joan::orig::init_426420() -> void { joan::bind::free<&::game_b100_01::init_426420>(); }  // 0x426420
auto joan::orig::BoxTree::vector_delete(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_05::H::vector_delete>(this, a0)); }  // 0x427930
auto joan::orig::Unit::set_name_index(void* a0) -> void { joan::bind::member<&::game_b93_01::H::set_name_index>(this, a0); }  // 0x427b50
auto joan::orig::CameraZones::lerp(float a0, float a1, float a2) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_camera_2::H::lerp>(this, a0, a1, a2)); }  // 0x427bb0
auto joan::orig::Unit::dtor_427c90() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_01::H::dtor_427c90>(this)); }  // 0x427c90
auto joan::orig::CameraZones::set_track_set(std::uint32_t a0) -> void { joan::bind::member<&::game_b82_18_3::H::set_id>(this, a0); }  // 0x427cc0
auto joan::orig::CameraZones::zone_free() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b88_06::H::slot_free>(this)); }  // 0x427d10
auto joan::orig::CameraZones::scripted(void* a0) -> void { joan::bind::member<&::game_camzone::H::scripted>(this, a0); }  // 0x427d90
auto joan::orig::MapObject::can_target(std::int32_t a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b93_01::H::can_target>(this, a0)); }  // 0x428600
auto joan::orig::UnitDraw::plate_delete() -> void { joan::bind::member<&::game_units::H::plate_delete>(this); }  // 0x4287b0
auto joan::orig::CameraZones::start_track(const char* a0, std::uint32_t a1) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b86_05::H::motion_start>(this, a0, a1)); }  // 0x428850
auto joan::orig::Track::restart() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_03::H::restart>(this)); }  // 0x4289e0
auto joan::orig::CameraZones::set_speed(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_01::H::set_46>(this, a0)); }  // 0x428a70
auto joan::orig::Track::is_type1() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b84_03::H::is_type1>(this)); }  // 0x428aa0
auto joan::orig::CameraZones::set_unit(std::uint32_t a0) -> void { joan::bind::member<&::game_fx::H::set_first>(this, a0); }  // 0x428b00
auto joan::orig::Tracks::get(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::track>(this, a0)); }  // 0x428b30
