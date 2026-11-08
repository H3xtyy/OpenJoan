// game/unknown_cam_angles.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_423220.cpp; re/attribution.csv: between ww/battle.cpp and ww/camman.cpp). They move to their
// original file when evidence names it (Phase 3).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::call::thiscall;

}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::img::str;

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

// ---------------------------------------------------------------------------- 0x00423220
static i32 JOAN_CDECL bone_id_of_name(const char* name) {
    for (i32 i = 0; i < 10; ++i) {
        if (stricmp_(str(d::g_9f748c + i * 0x38), name) == 0) return global<i32>(d::g_9f7488 + i * 0x38);
    }
    return 0;
}

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("bone_id_of_name", 0x00423220, &bone_id_of_name);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

using joan::call::import_call;

using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::orig::helpers::fbits;

void copy3(void* dst, const void* src) { std::memcpy(dst, src, 12); }

JOAN_COUNTER(c_from_angles, "Cam::from_angles_423290");
JOAN_COUNTER(c_to_angles, "Cam::to_angles_423400");

struct H {
    // ---------------------------------------------------------------- 0x00423290
    std::uintptr_t from_angles_423290(const u8* src) {  // EAX = this (pointer-wide on x64)
        c_from_angles.hit();
        const float c14 = joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(jo::as_cam_params(src)->orbit[1]))));  // cos
        const float c10 = joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(jo::as_cam_params(src)->orbit[0]))));
        const float s14 = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(jo::as_cam_params(src)->orbit[1]))));  // sin
        const float s10 = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(jo::as_cam_params(src)->orbit[0]))));
        u32 dist_bits;
        const float r = jo::as_cam_params(src)->orbit[3];
        if (!(r < 0.0f) && !(r > 0.0f))  // equal or unordered
            dist_bits = 0x3f800000u;
        else
            dist_bits = jo::view<u32>(jo::as_cam_params(src)->orbit[3]);
        const float dist = fbits(dist_bits);
        jo::self<jo::CamPose>(this)->u32_0 = jo::view<u32>(jo::as_cam_params(src)->projection);
        jo::self<jo::CamPose>(this)->eye[0] = jo::as_cam_params(src)->position[0] - dist * c14 * c10;
        jo::self<jo::CamPose>(this)->eye[1] = jo::as_cam_params(src)->position[1] - dist * c14 * s10;
        jo::self<jo::CamPose>(this)->eye[2] = dist * s14 + jo::as_cam_params(src)->position[2];
        u32 tmp[3];
        std::memcpy(tmp, jo::as_cam_params(src)->position, 12);
        std::memcpy(jo::self<jo::CamPose>(this)->target, tmp, 12);
        jo::self<jo::CamPose>(this)->u32_1c = jo::view<u32>(jo::as_cam_params(src)->orbit[2]);
        jo::self<jo::CamPose>(this)->u32_20 = jo::view<u32>(jo::as_cam_params(src)->fov_tan);
        jo::self<jo::CamPose>(this)->u32_24 = jo::view<u32>(jo::as_cam_params(src)->aspect);
        return reinterpret_cast<std::uintptr_t>(this);
    }

    // ---------------------------------------------------------------- 0x00423400
    u32* to_angles_423400(u32* out) {
        c_to_angles.hit();
        u32 v[3] = {jo::view<u32>(jo::self<jo::CamPose>(this)->target[0]), jo::view<u32>(jo::self<jo::CamPose>(this)->target[1]), jo::view<u32>(jo::self<jo::CamPose>(this)->eye[2])};
        u32 rec[10];
        rec[0] = jo::self<jo::CamPose>(this)->u32_0;
        rec[1] = jo::view<u32>(jo::self<jo::CamPose>(this)->target[0]);
        rec[2] = jo::view<u32>(jo::self<jo::CamPose>(this)->target[1]);
        rec[3] = jo::view<u32>(jo::self<jo::CamPose>(this)->target[2]);
        {
            const float dx = jo::self<jo::CamPose>(this)->target[0] - jo::self<jo::CamPose>(this)->eye[0];
            const float dy = jo::self<jo::CamPose>(this)->target[1] - jo::self<jo::CamPose>(this)->eye[1];
            const float a = joan::orig::arg::as<float>(joan::orig::libc::atan2(joan::orig::arg::f64(static_cast<double>(dy)), joan::orig::arg::f64(static_cast<double>(dx))));  // atan2
            std::memcpy(&rec[4], &a, 4);
        }
        {
            float t50[3], d[3];
            copy3(d, joan::orig::arg::as<float*>(joan::orig::vec_sub(joan::orig::arg::mptr(t50), joan::orig::arg::ptr(v), joan::orig::arg::ptr(jo::self<jo::CamPose>(this)->eye))));  // 0x0060cc90
            const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, d));               // length
            const float dz = jo::self<jo::CamPose>(this)->eye[2] - jo::self<jo::CamPose>(this)->target[2];
            const float a = joan::orig::arg::as<float>(joan::orig::libc::atan2(joan::orig::arg::f64(static_cast<double>(dz)), joan::orig::arg::f64(static_cast<double>(len))));
            std::memcpy(&rec[5], &a, 4);
        }
        rec[6] = jo::self<jo::CamPose>(this)->u32_1c;
        {
            float t68[3], d[3];
            copy3(d, joan::orig::arg::as<float*>(joan::orig::vec_sub(joan::orig::arg::mptr(t68), joan::orig::arg::ptr(jo::self<jo::CamPose>(this)->eye), joan::orig::arg::ptr(jo::self<jo::CamPose>(this)->target))));
            const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, d));
            std::memcpy(&rec[7], &len, 4);
        }
        rec[8] = jo::self<jo::CamPose>(this)->u32_20;
        rec[9] = jo::self<jo::CamPose>(this)->u32_24;
        std::memcpy(out, rec, 40);
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Cam::from_angles_423290", 0x00423290, &H::from_angles_423290);
B("Cam::to_angles_423400", 0x00423400, &H::to_angles_423400);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b87_05.cpp (game/b87_05)
namespace {
namespace game_b87_05 {


struct H {
    // ------------------------------------------------------------ 0x004233c0 (ret 4)
    std::uintptr_t set_angle(std::uint32_t angle) {  // EAX = this / 0
        jo::self<jo::Place>(this)->angle = angle;
        if (jo::view<void*>(jo::self<jo::Place>(this)->model) == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Place>(this)->model))->set_yaw(joan::orig::arg::u32(angle));  // 0x005ac6a0
        return 0;  // callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_05", name, addr, fn)
B("Place::set_angle", 0x004233c0, &H::set_angle);
#undef B

}  // namespace game_b87_05
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

std::uint8_t* world() { return global<std::uint8_t*>(d::g_world); }

struct H {
    void unit_detach_a() {                                                               // 0x00423540
        if (static_cast<std::int8_t>(jo::self<jo::UnitDraw>(this)->label_attached) == 0) return;
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_) == nullptr) return;
        joan::orig::as_scene(jo::view<void*>(jo::as_world(world())->scene))->remove_sorted(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_)));
        jo::self<jo::UnitDraw>(this)->label_attached = 0;
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("UnitDraw::detach_a", 0x00423540, &H::unit_detach_a);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;

struct H {
    // ------------------------------------------------------------------ 0x00423590 / 0x004586c0
    H* clear_c8() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::CameraMotion)));  // 0xc8 on x86 (new 0x0047a4f3 and the 6-element array of CameraFollow::motions)
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::clear_c8", 0x00423590, &H::clear_c8);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::bone_id_of_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_01::bone_id_of_name>(a0)); }  // 0x423220
auto joan::orig::Place::set_angle(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::set_angle>(this, a0)); }  // 0x4233c0
auto joan::orig::UnitDraw::detach_a() -> void { joan::bind::member<&::game_world::H::unit_detach_a>(this); }  // 0x423540
