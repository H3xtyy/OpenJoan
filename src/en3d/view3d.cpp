// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/view3d.cpp: functions of the original d:\ww\en3d\view3d.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (en3d/View3d.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../canvas_api.hpp"
#include "../registry.hpp"
#include "../freecam.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../render/shadow_map.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::hook::global;

using u8 = std::uint8_t;

using joan::hook::x87::Fpu;

using joan::call::import_call;

using Device = joan::d3d8::IDirect3DDevice8;

[[maybe_unused]] Device* game_device() { return global<Device*>(d::g_game_device); }

using joan::img::obj;

using joan::call::stdcall_call;

}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04 {

using joan::call::thiscall;

JOAN_COUNTER(c_release_scene, "Camera::release_scene");

struct H {
    // 0x006146e0
    std::uint32_t release_scene() {
        c_release_scene.hit();
        joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        if (jo::view<void*>(jo::self<jo::Camera>(this)->reorder) == nullptr) return joan::call::register_low_bits(this);  // eax = this; callers ignore it (misc_473140) or pass it to a caller with no C++ user (Obj::cam_check_4255c0)
        joan::orig::as_reorder(jo::view<void*>(jo::self<jo::Camera>(this)->reorder))->reset_batches();                                 // 0x005e6430
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::Camera>(this)->scene))->draw(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Camera>(this)->reorder)), joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x005eb8d0
        joan::orig::as_reorder(jo::view<void*>(jo::self<jo::Camera>(this)->reorder))->purge();                                  // 0x005e64e0
        return 0;  // void callee; callers ignore the result
    }

    // 0x00613d30
    void set_cf(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        jo::view<std::uint8_t>(jo::self<jo::Camera>(this)->clear_target) = static_cast<std::uint8_t>(a);
        jo::view<std::uint8_t>(jo::self<jo::Camera>(this)->clear_depth) = static_cast<std::uint8_t>(b);
        jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->clear_colour) = c;
    }

    // 0x00613700 / 0x0060fb10 / 0x005f4670 / 0x005e5570
    std::uint32_t dtor_613700() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_model(this)->release_parts()); }  // 0x00613a10
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("View3D::render_ordered", 0x006146e0, &H::release_scene);
E3D("View3D::set_clear_type", 0x00613d30, &H::set_cf);
E3D("View3D::~View3D", 0x00613700, &H::dtor_613700);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

using joan::call::thiscall;

using joan::call::cdecl_call;

// COM method: vtable slot at byte offset `off`, stdcall with the interface as first argument.
template <class R = std::int32_t, class... A>
R com_call(void* iface, std::uint32_t off, A... args) {
    const std::uintptr_t* vt = *reinterpret_cast<std::uintptr_t* const*>(iface);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vt[joan::call::x86_vslot(off)])(iface, args...);
}

using joan::orig::helpers::op_new;
using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_init_613730, "SysEn::init_613730");

struct H {
    // ------------------------------------------------------------------ 0x00613730
    u32 init_613730() {
        c_init_613730.hit();
        void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::Bone)));  // 0xa0 on x86
        void* const sub = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_bone(mem)->Bone_60eae0(static_cast<void*>(this), fn::code_ptr(fn::set_flag_cb)))  // 0x0060eae0
                                         : nullptr;
        jo::view<void*>(jo::self<jo::Camera>(this)->bone_) = sub;
        joan::orig::as_camera_params(&jo::self<jo::Camera>(this)->params)->construct();  // 0x00616d50
        jo::self<jo::Camera>(this)->u32_28 = 0;
        jo::view<u32>(jo::self<jo::Camera>(this)->near_plane) = 0x3f800000;
        jo::view<u32>(jo::self<jo::Camera>(this)->far_perspective) = 0x46000400;
        jo::view<u32>(jo::self<jo::Camera>(this)->far_ortho) = 0x46800200;
        if (jo::self<jo::Camera>(this)->params.projection == 1)
            jo::view<u32>(jo::self<jo::Camera>(this)->far_plane) = jo::view<u32>(jo::self<jo::Camera>(this)->far_ortho);
        else
            jo::view<u32>(jo::self<jo::Camera>(this)->far_plane) = jo::view<u32>(jo::self<jo::Camera>(this)->far_perspective);  // 0 and default
        jo::view<u8>(jo::self<jo::Camera>(this)->clear_target) = 1;
        jo::view<u8>(jo::self<jo::Camera>(this)->clear_depth) = 1;
        jo::view<u32>(jo::self<jo::Camera>(this)->clear_colour) = global<u32>(d::g_init_black);
        joan::orig::as_camera(this)->dirty();  // 0x00614170
        u32 material[0x44 / 4];
        crt_memset(material, 0, sizeof material);
        material[4] = 0x3f800000;  // ambient
        material[0] = material[4];  // diffuse
        material[5] = 0x3f800000;
        material[1] = material[5];
        material[6] = 0x3f800000;
        material[2] = material[6];
        material[7] = 0x3f800000;
        material[3] = material[7];
        void* const device = global<void*>(d::g_game_device);
        com_call<i32>(device, 0xa8, static_cast<void*>(material));
        jo::self<jo::Camera>(this)->light_ = nullptr;
        jo::view<u32>(jo::self<jo::Camera>(this)->fog_kind) = 0;
        jo::view<u32>(jo::self<jo::Camera>(this)->fog_start) = 0;
        jo::view<u32>(jo::self<jo::Camera>(this)->fog_end) = 0x461c4000;
        jo::view<u32>(jo::self<jo::Camera>(this)->fog_density) = 0x3ba3d70a;
        jo::view<u32>(jo::self<jo::Camera>(this)->fog_height_a) = 0;
        jo::view<u32>(jo::self<jo::Camera>(this)->fog_height_b) = 0;
        jo::view<u32>(jo::self<jo::Camera>(this)->fog_colour) = jo::view<u32>(jo::self<jo::Camera>(this)->clear_colour);
        void* const mem2 = op_new(static_cast<std::uint32_t>(sizeof(jo::Fog)));  // 0x50 on x86
        void* const cache = mem2 != nullptr ? joan::orig::as_fog(mem2)->construct() : nullptr;  // 0x005bfd80
        jo::view<void*>(jo::self<jo::Camera>(this)->fog) = cache;
        return joan::orig::arg::as<u32>(joan::orig::as_fog(jo::view<void*>(jo::self<jo::Camera>(this)->fog))->init());  // 0x005bfde0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("View3D::init", 0x00613730, &H::init_613730);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b116_11.cpp (game/b116_11)
namespace {
namespace game_b116_11 {

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    // 0x00613bf0
    u32 set_viewport(jo::Surface* a, void* b, u32 x1, u32 y1, u32 x2, u32 y2) {
        {
            // canvas: what the entry wrapper did (canvas_api.hpp) - the 6 arguments as 32-bit words and the return
            // address, before anything else runs (x1..y2 are read and written; args[0..1] are the two pointer slots of
            // the layout: display::camera_viewport never reads them, so they are 0 rather than a cut pointer).
            std::int32_t args[6] = {0, 0,
                                    static_cast<i32>(x1), static_cast<i32>(y1), static_cast<i32>(x2), static_cast<i32>(y2)};
            joan::canvas::set_viewport_args(args, JOAN_CANVAS_RETURN_ADDRESS());
            x1 = static_cast<u32>(args[2]), y1 = static_cast<u32>(args[3]), x2 = static_cast<u32>(args[4]), y2 = static_cast<u32>(args[5]);
        }
        if (a != nullptr) jo::self<jo::Camera>(this)->target = a;
        if (b != nullptr) jo::self<jo::Camera>(this)->depth_target = b;
        jo::view<u32>(jo::self<jo::Camera>(this)->view_x1) = x1;
        jo::view<u32>(jo::self<jo::Camera>(this)->view_y1) = y1;
        jo::view<u32>(jo::self<jo::Camera>(this)->view_x2) = x2;
        jo::view<u32>(jo::self<jo::Camera>(this)->view_y2) = y2;
        jo::view<u32>(jo::self<jo::Camera>(this)->view_width) = jo::view<u32>(jo::self<jo::Camera>(this)->view_x2) - jo::view<u32>(jo::self<jo::Camera>(this)->view_x1) + 1u;
        jo::view<u32>(jo::self<jo::Camera>(this)->view_height) = jo::view<u32>(jo::self<jo::Camera>(this)->view_y2) - jo::view<u32>(jo::self<jo::Camera>(this)->view_y1) + 1u;
        joan::orig::as_camera(this)->dirty();  // 0x00614170
        return 0;  // void callee; callers ignore the result
    }
};

#define B(name, address, fn) JOAN_REPLACE("game/b116_11", name, address, fn)
B("View3D::set_view_area", 0x00613bf0, &H::set_viewport);
JOAN_CANVAS_SELF(0x00613bf0);  // set_viewport_args in the body (no entry wrapper)
#undef B

}  // namespace game_b116_11
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
namespace {
namespace game_b90_05 {

using joan::call::thiscall;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);

std::uint32_t bits(float v) {
    std::uint32_t b;
    std::memcpy(&b, &v, 4);
    return b;
}
float from_bits(std::uint32_t b) {
    float v;
    std::memcpy(&v, &b, 4);
    return v;
}

JOAN_COUNTER(c_screen_ray, "Camera::screen_ray");

struct H {
    // 0x00613ca0: each value > 0 (not NaN) is stored, then 0x00614170.
    void set_ranges(float a, float b, float c) {
        if (a > global<float>(d::k_9cf5c4)) jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->near_plane) = bits(a);
        if (b > global<float>(d::k_9cf5c4)) jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->far_perspective) = bits(b);
        if (c > global<float>(d::k_9cf5c4)) jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->far_ortho) = bits(c);
        joan::orig::as_camera(this)->dirty();  // 0x00614170
    }

    // 0x006154d0: near / far points (out[0], out[1]) of the ray through screen pixel (x, y).
    float* screen_ray(std::int32_t x, std::int32_t y, float* out) {
        c_screen_ray.hit();
        const Fpu fpu{true};
        joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        Vec3 v1, v2;
        const float s = jo::self<jo::Camera>(this)->projection[11];
        const float zero = global<float>(d::k_9cf5c4);
        if (s < zero || s > zero) {  // fcomp; test ah,0x40; jne -> else branch on equal / NaN
            const float hx = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::Camera>(this)->view_width), global<float>(d::flt_2)));
            const float hy = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::Camera>(this)->view_height), global<float>(d::flt_2)));
            double t = fpu.sub(static_cast<double>(x), hx);
            t = fpu.div(t, hx);
            t = fpu.mul(t, jo::self<jo::Camera>(this)->projection[11]);
            t = fpu.div(t, jo::self<jo::Camera>(this)->projection[0]);
            const float tx = static_cast<float>(t);
            t = fpu.sub(static_cast<double>(y), hy);
            t = fpu.div(t, hy);
            t = fpu.mul(t, jo::self<jo::Camera>(this)->projection[11]);
            t = fpu.div(t, jo::self<jo::Camera>(this)->projection[5]);
            const float ty = static_cast<float>(t);
            const std::uint32_t near_bits = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->near_plane);
            const float near_d = from_bits(near_bits);
            v1.x = static_cast<float>(fpu.mul(tx, near_d));
            v1.y = static_cast<float>(fpu.mul(ty, near_d));
            std::memcpy(&v1.z, &near_bits, 4);
            const std::uint32_t far_bits = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->far_plane);
            const float far_d = from_bits(far_bits);
            v2.x = static_cast<float>(fpu.mul(tx, far_d));
            v2.y = static_cast<float>(fpu.mul(ty, far_d));
            std::memcpy(&v2.z, &far_bits, 4);
        } else {
            const float hx = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::Camera>(this)->view_width), global<float>(d::flt_2)));
            const float hy = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::Camera>(this)->view_height), global<float>(d::flt_2)));
            double t = fpu.sub(static_cast<double>(x), hx);
            t = fpu.div(t, hx);
            t = fpu.div(t, jo::self<jo::Camera>(this)->projection[0]);
            const float tx = static_cast<float>(t);
            t = fpu.sub(static_cast<double>(y), hy);
            t = fpu.div(t, hy);
            t = fpu.div(t, jo::self<jo::Camera>(this)->projection[5]);
            const float ty = static_cast<float>(t);
            const std::uint32_t near_bits = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->near_plane);
            std::memcpy(&v1.x, &tx, 4);
            std::memcpy(&v1.y, &ty, 4);
            std::memcpy(&v1.z, &near_bits, 4);
            const std::uint32_t far_bits = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->far_plane);
            std::memcpy(&v2.x, &tx, 4);
            std::memcpy(&v2.y, &ty, 4);
            std::memcpy(&v2.z, &far_bits, 4);
        }
        void* m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Camera>(this)->bone_))->matrix());  // 0x0060ef80
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(out), static_cast<void*>(&v1), m);  // point_transform
        m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Camera>(this)->bone_))->matrix());  // 0x0060ef80
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(out + 3), static_cast<void*>(&v2), m);
        return out;
    }
};

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("View3D::set_clip_distance", 0x00613ca0, &H::set_ranges);
B9005("View3D::pixel_ray", 0x006154d0, &H::screen_ray);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

using joan::call::thiscall;

struct H {
    void camera_set_scene(jo::Scene* scene) { jo::self<jo::Camera>(this)->scene = scene; }          // 0x00613db0
    void camera_dirty() { jo::self<jo::Camera>(this)->dirty_ = 1; }                                   // 0x00614170
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("View3D::set_scene", 0x00613db0, &H::camera_set_scene);
W1("View3D::mark_changed", 0x00614170, &H::camera_dirty);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

using joan::call::thiscall;

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // --- Camera (camera code after vgalock.cpp) -----------------------------------------------------
    void* camera_bone() {  // 0x00613de0
        joan::orig::as_camera(self())->update_matrices();  // 0x006141a0 update
        return jo::view<void*>(jo::self<jo::Camera>(this)->bone_);
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("View3D::get_frame", 0x00613de0, &H::camera_bone);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/game_b101_12.cpp (game/b101_12)
namespace {
namespace game_b101_12 {

JOAN_COUNTER(c_clamp, "Angles::clamp");

struct H {
    // 0x00613e10
    i32 clamp(u8* p, i32 allow) {
        c_clamp.hit();
        if (jo::as_cam_params(p)->projection != 0 && jo::as_cam_params(p)->projection != 1) return 0;
        i32 changed = 0;
        // fcomp + test ah,1 je: clamp when p < lo or unordered.
        if (!(jo::as_cam_params(p)->position[0] >= jo::self<jo::Camera>(this)->angle_min[0])) {
            if (allow == 0) return 0;
            jo::view<u32>(jo::as_cam_params(p)->position[0]) = jo::view<u32>(jo::self<jo::Camera>(this)->angle_min[0]);
            changed = 1;
        }
        // test ah,0x41 jne: clamp only when p > hi.
        if (jo::as_cam_params(p)->position[0] > jo::self<jo::Camera>(this)->angle_max[0]) {
            if (allow == 0) return 0;
            jo::view<u32>(jo::as_cam_params(p)->position[0]) = jo::view<u32>(jo::self<jo::Camera>(this)->angle_max[0]);
            changed = 1;
        }
        if (!(jo::as_cam_params(p)->position[1] >= jo::self<jo::Camera>(this)->angle_min[1])) {
            if (allow == 0) return 0;
            jo::view<u32>(jo::as_cam_params(p)->position[1]) = jo::view<u32>(jo::self<jo::Camera>(this)->angle_min[1]);
            changed = 1;
        }
        if (jo::as_cam_params(p)->position[1] > jo::self<jo::Camera>(this)->angle_max[1]) {
            if (allow == 0) return 0;
            jo::view<u32>(jo::as_cam_params(p)->position[1]) = jo::view<u32>(jo::self<jo::Camera>(this)->angle_max[1]);
            changed = 1;
        }
        if (!(jo::as_cam_params(p)->position[2] >= jo::self<jo::Camera>(this)->angle_extra[0])) {
            if (allow == 0) return 0;
            jo::view<u32>(jo::as_cam_params(p)->position[2]) = jo::view<u32>(jo::self<jo::Camera>(this)->angle_extra[0]);
            changed = 1;
        }
        if (jo::as_cam_params(p)->position[2] > jo::self<jo::Camera>(this)->angle_extra[1]) {
            if (allow == 0) return 0;
            jo::view<u32>(jo::as_cam_params(p)->position[2]) = jo::view<u32>(jo::self<jo::Camera>(this)->angle_extra[1]);
            changed = 1;
        }
        // Fails when x < lo or unordered, or x > hi.
        const double a = jo::as_cam_params(p)->orbit[1];
        if (!(a >= global<double>(d::dbl_m1_5725416560468908)) || a > global<double>(d::dbl_1_5725416560468908)) return 0;
        const double b = jo::as_cam_params(p)->orbit[2];
        if (!(b >= global<double>(d::dbl_m0_0017453292519943296)) || b > global<double>(d::dbl_0_0017453292519943296)) return 0;
        return changed + 1;
    }
};

#define R12(name, addr, fn) JOAN_REPLACE("game/b101_12", name, addr, fn)
R12("View3D::validate_cam_setting", 0x00613e10, &H::clamp);
#undef R12

}  // namespace game_b101_12
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

using joan::call::thiscall;

struct H {
    // 0x00614110
    void set_int(std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t d, std::int32_t e, std::int32_t g) {
        jo::self<jo::Camera>(this)->angle_min[0] = static_cast<float>(a);
        jo::self<jo::Camera>(this)->angle_min[1] = static_cast<float>(b);
        jo::self<jo::Camera>(this)->angle_max[0] = static_cast<float>(c);
        jo::self<jo::Camera>(this)->angle_max[1] = static_cast<float>(d);
        jo::self<jo::Camera>(this)->angle_extra[0] = static_cast<float>(e);
        jo::self<jo::Camera>(this)->angle_extra[1] = static_cast<float>(g);
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("View3D::set_cam_setting", 0x00614110, &H::set_int);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {

using joan::call::thiscall;

using joan::call::cdecl_call;

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

JOAN_COUNTER(c_frustum, "Camera::update_matrices");

// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    // 0x006141a0: rebuild view / projection / viewport and the frustum planes when dirty.
    std::int32_t camera_update_matrices() {
        c_frustum.hit();
        if (jo::self<jo::Camera>(this)->matrices_valid != 0) return 0;
        // The original leaves the projection uninitialised for a kind other than 0 / 1:
        // keep that case on the original body.
        // Batch 118/03: with fix/uninit enabled (module_enabled, read on the first call) the C++
        // version runs instead with that projection all zero (the far plane +0x48 is left as is,
        // as the original's switch default does).
        if (jo::view<std::int8_t>(jo::self<jo::Camera>(this)->dirty_) != 0 && jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->params.projection) > 1 && !fix_uninit()) {
            using Fn = std::int32_t (H::*)();
            return (this->*joan::hook::original<Fn>(fn::Camera_update_matrices_body))();
        }
        void* const bone = jo::view<void*>(jo::self<jo::Camera>(this)->bone_);
        if (bone) joan::orig::as_bone(bone)->update();  // 0x0060ee60 Bone::update
        if (jo::view<std::int8_t>(jo::self<jo::Camera>(this)->dirty_) == 0) return 0;

        std::memset(&jo::self<jo::Camera>(this)->viewport_x, 0, 0x18);  // viewport_x .. viewport_max_z
        jo::self<jo::Camera>(this)->viewport_x = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->view_x1);
        jo::self<jo::Camera>(this)->viewport_y = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->view_y1);
        jo::self<jo::Camera>(this)->viewport_width = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->view_width);
        jo::self<jo::Camera>(this)->viewport_height = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->view_height);
        jo::self<jo::Camera>(this)->viewport_min_z = 0.0f;
        jo::self<jo::Camera>(this)->viewport_max_z = 1.0f;

        joan::orig::orbit_matrix(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Camera>(this)->bone_)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->params.orbit[0])), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->params.orbit[1])), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->params.orbit[2])), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->params.orbit[3])));                                   // 0x0060bd00
        import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, jo::view<void*>(jo::self<jo::Camera>(this)->bone_), jo::self<jo::Camera>(this)->params.position);        // matrix_multiply_translation
        joan::orig::as_bone(jo::view<void*>(jo::self<jo::Camera>(this)->bone_))->invalidate();                                 // 0x0060ee10
        import_call<void*>(d::iat_v3d_87_matrix_inverse_2, jo::self<jo::Camera>(this)->inv_bone_matrix, joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Camera>(this)->bone_))->matrix()));  // matrix_inverse

        float camera_matrix[16], view[16];
        joan::orig::matrix_negate_row1(joan::orig::arg::mptr(camera_matrix), joan::orig::arg::mptr(joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Camera>(this)->bone_))->matrix())));  // 0x0060c000
        joan::orig::d3dx::matrix_inverse(joan::orig::arg::mptr(view), joan::orig::arg::mptr(static_cast<float*>(nullptr)), joan::orig::arg::mptr(camera_matrix));        // D3DXMatrixInverse
        std::memcpy(jo::self<jo::Camera>(this)->matrix_22e_, camera_matrix, 64);
        std::memcpy(jo::self<jo::Camera>(this)->view_, view, 64);

        float projection[16] = {};  // never written for kinds > 1 (fix/uninit only): 0
        if (jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->params.projection) == 0) {
            jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->far_plane) = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->far_perspective);
            const float fov = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(static_cast<double>(jo::self<jo::Camera>(this)->params.fov_tan)))));  // atan
            const float aspect = static_cast<float>(jo::self<jo::Camera>(this)->view_width) / static_cast<float>(jo::self<jo::Camera>(this)->view_height);
            joan::orig::d3dx::matrix_perspective_fov_lh(joan::orig::arg::mptr(projection), joan::orig::arg::f32(fov), joan::orig::arg::f32(aspect), joan::orig::arg::f32(jo::self<jo::Camera>(this)->near_plane), joan::orig::arg::f32(jo::self<jo::Camera>(this)->far_plane));  // D3DXMatrixPerspectiveFovLH
        } else if (jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->params.projection) == 1) {
            jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->far_plane) = jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->far_ortho);
            const float w = static_cast<float>(jo::self<jo::Camera>(this)->view_width) * jo::self<jo::Camera>(this)->params.aspect;
            const float h = static_cast<float>(jo::self<jo::Camera>(this)->view_height) * jo::self<jo::Camera>(this)->params.aspect;
            joan::orig::d3dx::matrix_ortho_lh(joan::orig::arg::mptr(projection), joan::orig::arg::f32(w), joan::orig::arg::f32(h), joan::orig::arg::f32(jo::self<jo::Camera>(this)->near_plane), joan::orig::arg::f32(jo::self<jo::Camera>(this)->far_plane));  // D3DXMatrixOrthoLH
        }
        std::memcpy(jo::self<jo::Camera>(this)->projection, projection, 64);
        std::memcpy(jo::self<jo::Camera>(this)->d3d_projection, projection, 64);
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, jo::self<jo::Camera>(this)->view_projection, jo::self<jo::Camera>(this)->inv_bone_matrix, jo::self<jo::Camera>(this)->projection);   // matrix_multiply
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, jo::self<jo::Camera>(this)->matrix_1ae, jo::self<jo::Camera>(this)->view_, jo::self<jo::Camera>(this)->d3d_projection);

        float viewport[16];
        std::memcpy(viewport, img::obj(d::g_identity_matrix), 64);  // identity
        viewport[0] = static_cast<float>(jo::self<jo::Camera>(this)->view_width) / 2.0f;
        viewport[5] = static_cast<float>(jo::self<jo::Camera>(this)->view_height) / 2.0f;
        viewport[10] = jo::self<jo::Camera>(this)->viewport_max_z - jo::self<jo::Camera>(this)->viewport_min_z;
        viewport[12] = static_cast<float>(jo::self<jo::Camera>(this)->view_width) / 2.0f;
        viewport[13] = static_cast<float>(jo::self<jo::Camera>(this)->view_height) / 2.0f;
        std::memcpy(&viewport[14], &jo::self<jo::Camera>(this)->viewport_min_z, 4);
        import_call<void*>(d::iat_v3d_87_matrix_multiply_2, jo::self<jo::Camera>(this)->view_projection, viewport);  // matrix_multiply in place
        joan::orig::as_camera(this)->build_frustum();                                 // 0x00616460 frustum planes
        jo::self<jo::Camera>(this)->dirty_ = 0;
        return 1;
    }
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("View3D::recalc_change", 0x006141a0, &H::camera_update_matrices);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

using joan::call::thiscall;

constexpr img::Addr kSysEn(d::g_sys3d);

JOAN_COUNTER(c_begin_frame, "Camera::begin_frame");

struct H {
    // 0x00614630
    std::int32_t begin_frame() {
        c_begin_frame.hit();
        if (jo::view<void*>(jo::self<jo::Camera>(this)->bone_) == nullptr || jo::view<void*>(jo::self<jo::Camera>(this)->scene) == nullptr || jo::view<void*>(jo::self<jo::Camera>(this)->target) == nullptr || jo::self<jo::Camera>(this)->depth_target == nullptr) return 0;
        if (jo::self<jo::Camera>(this)->view_x1 < 0 || jo::self<jo::Camera>(this)->view_y1 < 0) return 0;
        if (jo::self<jo::Camera>(this)->view_x2 >= joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(jo::view<void*>(jo::self<jo::Camera>(this)->target))->get_1c())) return 0;  // 0x0060fa50
        if (jo::self<jo::Camera>(this)->view_y2 >= joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(jo::view<void*>(jo::self<jo::Camera>(this)->target))->get_20())) return 0;  // 0x0060fa70
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_d3d(img::obj(kSysEn))->device_lost()) != 0) return 0;                          // 0x005ff420 device lost
        return 1;
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("View3D::can_render", 0x00614630, &H::begin_frame);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/camera_render.cpp (en3d/camera.cpp)
namespace {
namespace camera_render {

using joan::call::thiscall;

using joan::call::cdecl_call;

Device* dev() { return global<Device*>(d::g_game_device); }

JOAN_COUNTER(c_camera_render, "Camera::render");

struct Vec3 {
    float x, y, z;
};
// The game's vector helpers (cdecl, result through the first pointer), kept as plain C++:
// they are single float operations per component, which the game runs with the x87 at
// 24-bit precision - the same rounding as these float expressions.
Vec3 add(const Vec3& a, const Vec3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }  // 0x0060cc20
Vec3 scale(float s, const Vec3& v) { return {s * v.x, s * v.y, s * v.z}; }           // 0x0060cd60 / 0x0060cd00
const Vec3& vec(const float (&v)[3]) { return *reinterpret_cast<const Vec3*>(v); }
// exp / pow through the game's wrappers (0x00615160 / 0x00428810) so they round identically.
float game_exp(float x) { return joan::orig::arg::as<float>(joan::orig::exp_float(joan::orig::arg::f32(x))); }
float game_pow(float x, float y) { return joan::orig::arg::as<float>(joan::orig::libc::pow_f(joan::orig::arg::f32(x), joan::orig::arg::f32(y))); }
// A colour channel 0..1 -> byte: above 255 -> 255, below 0 (or NaN) -> 0, else truncated (_ftol).
std::uint8_t channel(float c) {
    const float v = c * 255.0f;
    if (!(v <= 255.0f) && v == v) return 0xff;
    if (v < 0.0f || v != v) return 0;
    return static_cast<std::uint8_t>(static_cast<std::int64_t>(v));
}

// Sky colour from single scattering (Rayleigh + Henyey-Greenstein Mie) towards the view direction.
void scattering_colour(void* camera, std::uint8_t bgra[4]) {
    Vec3 view;
    joan::orig::matrix_axis_z(&view, joan::orig::arg::ptr(joan::orig::arg::as<const float*>(joan::orig::as_bone(jo::as_camera(camera)->bone_)->matrix())));  // 0x0060c220
    view.z = 0.0f;
    reinterpret_cast<void*(JOAN_CDECL*)(Vec3*)>(global<std::uintptr_t>(d::iat_v3d_87_vector_normalize_1))(&view);  // vector_normalize
    const jo::LightParams* atm;
    if (joan::orig::arg::as<int>(joan::orig::as_weather_system(img::obj(d::g_weather_system))->active_lightning()) != 0) {  // 0x0068c570 lightning active
        const std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_table(img::obj(d::g_light_sys))->find_4a9530(joan::orig::arg::str(&img::global<const char>(d::s_Lightning))));  // "Lightning"
        atm = &global<jo::LightPreset*>(d::g_a47e10)[i - 1].params;
    } else {
        atm = &img::global<const jo::LightParams>(d::g_a47f40);
    }
    const std::uintptr_t dot_product = global<std::uintptr_t>(d::iat_v3d_87_dot_product);
#if UINTPTR_MAX == 0xffffffffu
    // x86: the result is st0 either way; read as float it is rounded once, as the original's fstp dword.
    const float cos_theta = reinterpret_cast<float(JOAN_CDECL*)(const Vec3*, const Vec3*)>(dot_product)(&view, &vec(atm->sun_dir));
#else
    // x64: v3d's dot_product returns a double (lib/v3d/v3d87.cpp) in xmm0; read as a float the low half of the double
    // came back (garbage): the rain sky / fog colour flickered (owner).
    const float cos_theta = static_cast<float>(reinterpret_cast<double(JOAN_CDECL*)(const Vec3*, const Vec3*)>(dot_product)(&view, &vec(atm->sun_dir)));
#endif
    const float rayleigh = atm->scale_a[0], mie = atm->scale_a[1], g = atm->scale_a[3];
    const Vec3 beta_sum = add(vec(atm->vec_18), vec(atm->vec_24));
    const Vec3 mie_ext = scale(mie * 100.0f, beta_sum);
    const Vec3 rayleigh_ext = scale(rayleigh * 100.0f, vec(atm->vec_c));
    const Vec3 extinction = scale(atm->scale_a[4], add(rayleigh_ext, mie_ext));
    const Vec3 rayleigh_in = scale(5.968310356f * rayleigh, vec(atm->vec_c));  // 3 / (16 pi) * 100
    const Vec3 mie_in = scale(7.957746983f * mie, vec(atm->vec_18));            // 1 / (4 pi) * 100
    const float hg_num = 1.0f - g * g;
    const float hg = hg_num / game_pow((1.0f + g) + 2.0f * g * cos_theta, 1.5f);
    Vec3 c = add(scale(cos_theta * cos_theta + 1.0f, rayleigh_in), scale(hg, mie_in));
    const float distance = static_cast<float>(global<std::int32_t>(d::g_a19174));  // 2000, exact as float
    c.x = (1.0f - game_exp(-extinction.x * distance)) / extinction.x * atm->scale_b[0] * c.x;
    c.y = (1.0f - game_exp(-extinction.y * distance)) / extinction.y * atm->scale_b[1] * c.y;
    c.z = (1.0f - game_exp(-extinction.z * distance)) / extinction.z * atm->scale_b[2] * c.z;
    const float intensity = atm->scale_a[2] * atm->f32_48;
    c = {intensity * c.x, intensity * c.y, intensity * c.z};  // 0x0060ce80 (v * s per component)
    bgra[2] = channel(c.x);
    bgra[1] = channel(c.y);
    bgra[0] = channel(c.z);
    bgra[3] = 0xff;
}

bool scattering_sky(void* camera) {
    void* const sky = jo::as_camera(camera)->scene->sky;
    return sky != nullptr && jo::as_sky_box(sky)->materials->texture_mode == 0x10;
}

struct Camera {
    void render();
};

// [Debug] FreecamKey (freecam.hpp): the forward (z) and right (x) axes of the camera at these angles, as
// update_matrices builds them (orbit_matrix, no roll, no distance).
void freecam_axes(float yaw, float pitch, float* forward, float* right) {
    float m[16];
    joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::f32(yaw), joan::orig::arg::f32(pitch), joan::orig::arg::f32(0.0f),
                             joan::orig::arg::f32(0.0f));
    for (int i = 0; i < 3; ++i) forward[i] = m[8 + i], right[i] = m[i];
}

// While the free camera is on, the main camera (a perspective view of the world at least 640 pixels wide -
// not the minimap, portraits or a 1024x768 screen's small views) is drawn from its pose: its parameters are
// swapped in for the render and the game's put back after it (with the matrices rebuilt either way).
struct FreecamScope {
    jo::Camera* cam = nullptr;
    std::uint8_t params[40], prev[40];
    explicit FreecamScope(void* camera) {
        jo::Camera* const c = jo::self<jo::Camera>(camera);
        if (!joan::freecam::active() || jo::view<std::uint32_t>(c->params.projection) != 0 || c->view_width < 640 || c->scene == nullptr) return;
        // The game camera's eye: `distance` (orbit[3]) back from its position along the view.
        joan::freecam::Pose start{};
        float forward[3], right[3];
        freecam_axes(c->params.orbit[0], c->params.orbit[1], forward, right);
        for (int i = 0; i < 3; ++i) start.position[i] = c->params.position[i] - forward[i] * c->params.orbit[3];
        start.yaw = c->params.orbit[0];
        start.pitch = c->params.orbit[1];
        joan::freecam::Pose p{};
        if (!joan::freecam::pose(start, &freecam_axes, p)) return;
        cam = c;
        std::memcpy(params, &c->params, sizeof params);
        std::memcpy(prev, &c->prev_params, sizeof prev);
        for (int i = 0; i < 3; ++i) c->params.position[i] = p.position[i];
        c->params.orbit[0] = p.yaw;
        c->params.orbit[1] = p.pitch;
        c->params.orbit[2] = 0.0f;
        c->params.orbit[3] = 0.0f;
        c->dirty_ = 1;
    }
    ~FreecamScope() {
        if (!cam) return;
        std::memcpy(&cam->params, params, sizeof params);
        std::memcpy(&cam->prev_params, prev, sizeof prev);
        cam->dirty_ = 1;
        joan::orig::as_camera(cam)->update_matrices();  // the game's own matrices again (picking, sound)
    }
};

// 0x00614760, thiscall, ret 0.
void Camera::render() {
    c_camera_render.hit();
    const FreecamScope freecam(this);
    if (void* const t = global<void*>(d::g_vs_cache)) {
        jo::as_vs_cache(t)->u32_0 = 1;
        jo::as_vs_cache(t)->u32_4 = 1;
    }
    joan::orig::as_fog(jo::view<void*>(jo::self<jo::Camera>(this)->fog))->begin();  // 0x005c0e50
    joan::orig::as_camera(this)->update_matrices();                    // 0x006141a0 frustum
    dev()->SetTransform(2, jo::self<jo::Camera>(this)->view_);  // VIEW
    dev()->SetTransform(3, jo::self<jo::Camera>(this)->d3d_projection);  // PROJECTION
    dev()->SetViewport(&jo::self<jo::Camera>(this)->viewport_x);
    std::uint8_t sky_colour[4] = {};  // B, G, R, A as a D3DCOLOR in memory
    if (scattering_sky(this)) {
        scattering_colour(this, sky_colour);
        std::uint8_t rgba[4] = {sky_colour[2], sky_colour[1], sky_colour[0], sky_colour[3]};
        joan::orig::as_camera(this)->set_fog_colour(joan::orig::arg::mptr(rgba));  // 0x00616c10 camera fog / clear colour (R, G, B, A bytes)
    }
    if (jo::self<jo::Camera>(this)->clear_target != 0 || jo::self<jo::Camera>(this)->clear_depth != 0) {
        std::uint32_t flags = 0, colour = 0;
        if (jo::self<jo::Camera>(this)->clear_target != 0) {
            flags |= 1;  // target
            if (scattering_sky(this))
                joan::orig::as_colour(sky_colour)->to_d3d(&colour);  // 0x005e93b0 colour -> D3DCOLOR
            else
                joan::orig::as_colour(&jo::self<jo::Camera>(this)->clear_colour)->to_d3d(&colour);
        }
        if (jo::self<jo::Camera>(this)->clear_depth != 0) {
            flags |= 2;  // z-buffer
            if (global<std::uint32_t>(d::g_stencil_bits) != 0) flags |= 4;  // stencil
        }
        dev()->Clear(0, nullptr, flags, colour, 1.0f, 0);
    }
    void* const scene = jo::view<void*>(jo::self<jo::Camera>(this)->scene);
    if (global<std::int8_t>(d::g_cfg_shadow_quality) == 2 && global<void*>(d::g_a52737) != nullptr && scene &&
        joan::orig::arg::as<int>(joan::orig::as_scene(scene)->active()) != 0)  // 0x005eb8b0
        joan::orig::as_camera(this)->water_receivers();             // 0x00617370
    if (jo::view<void*>(jo::self<jo::Camera>(this)->scene) && joan::orig::arg::as<int>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Camera>(this)->scene))->active()) != 0) {
        if (dev()->BeginScene() != 0) return;
        jo::view<std::uint8_t>(jo::self<jo::Camera>(this)->matrices_valid) = 1;
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::Camera>(this)->scene))->render(joan::orig::arg::mptr(this));  // Scene::render 0x005ebac0
        std::uint8_t material[0x44] = {};
        const float one = 1.0f;
        for (int i = 0; i < 4; ++i) std::memcpy(material + i * 4, &one, 4);  // diffuse = white
        dev()->SetMaterial(material);
        dev()->EndScene();
        std::memcpy(&jo::self<jo::Camera>(this)->prev_params, &jo::self<jo::Camera>(this)->params, 40);
        jo::view<std::uint8_t>(jo::self<jo::Camera>(this)->matrices_valid) = 0;
    }
}

JOAN_REPLACE("en3d/camera.cpp", "View3D::render", 0x00614760, &Camera::render);

}  // namespace camera_render
}  // namespace

// ---------------------------------------- from modules/game_b111_09.cpp (game/b111_09)
namespace {
namespace game_b111_09 {

using joan::call::thiscall;

using joan::call::cdecl_call;

struct H {
    // ------------------------------------------------------------------ 0x00615190
    void set_sub(i32 on) {
        if (on != 0) {
            if (jo::self<jo::Camera>(this)->reorder != nullptr) return;
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Reorder)));  // operator new (0xb2 on x86)
            void* const o = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_reorder(mem)->ctor()) : nullptr;  // 0x005e6000
            jo::view<void*>(jo::self<jo::Camera>(this)->reorder) = o;
            joan::orig::as_reorder(jo::view<void*>(jo::self<jo::Camera>(this)->reorder))->begin_frame();  // 0x005e61a0
            return;
        }
        if (jo::self<jo::Camera>(this)->reorder == nullptr) return;
        void* const o = jo::view<void*>(jo::self<jo::Camera>(this)->reorder);
        if (o != nullptr) joan::orig::as_reorder(o)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00613b00 scalar deleting dtor
        jo::self<jo::Camera>(this)->reorder = nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_09", name, addr, fn)
B("View3D::enable_render_order", 0x00615190, &H::set_sub);
#undef B

}  // namespace game_b111_09
}  // namespace

// ---------------------------------------- from modules/en3d_helpers3.cpp (en3d/helpers3)
namespace {
namespace en3d_helpers3 {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::orig::helpers::at;

bool below_or_nan(float a, float b) { return !(a >= b); }

struct H {
    // 0x006156b0: box against the frustum's bounding box, then the extra and the 6 frustum
    // planes: 0 outside, 1 crossing, 2 inside.
    std::int32_t camera_box_in_view(const void* box) {
        if (jo::self<jo::Camera>(this)->matrices_valid == 0) joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        if (!joan::orig::arg::as<bool>(joan::orig::as_box(jo::self<jo::Camera>(this)->frustum_bounds)->overlaps(joan::orig::arg::mptr(box)))) return 0;          // 0x0060d6b0
        std::int32_t result = 2;
        for (std::int32_t i = 0; i < jo::self<jo::Camera>(this)->extra_plane_count; ++i) {
            float lo, hi;
            joan::orig::box_plane_extent(joan::orig::arg::mptr(&jo::self<jo::Camera>(this)->extra_planes[i * 4]), joan::orig::arg::mptr(box), &lo, &hi);  // 0x0060dc80
            if (below_or_nan(hi, 0.0f)) return 0;
            if (below_or_nan(lo, 0.0f)) result = 1;
        }
        for (std::int32_t i = 0; i < 6; ++i) {
            float lo, hi;
            joan::orig::box_plane_extent(joan::orig::arg::mptr(&jo::self<jo::Camera>(this)->frustum_planes[i * 4]), joan::orig::arg::mptr(box), &lo, &hi);
            if (below_or_nan(hi, 0.0f)) return 0;
            if (below_or_nan(lo, 0.0f)) result = 1;
        }
        return result;
    }

    // 0x00616030: 1 when the box is entirely in front of every plane of one of the scene's
    // occluders (0x94 bytes each: +0x38 planes, +0x88 count).
    std::int32_t camera_box_occluded(const void* box) {
        void* const scene = jo::view<void*>(jo::self<jo::Camera>(this)->scene);
        if (scene == nullptr) return 0;
        const std::int32_t count = joan::orig::arg::as<std::int32_t>(joan::orig::as_scene(scene)->occluder_count());  // 0x005ef3b0
        for (std::int32_t i = 0; i < count; ++i) {
            std::uint8_t* const occluder = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene(scene)->occluder(joan::orig::arg::i32(i)));  // 0x005eed90
            std::int32_t state = 2;
            const std::uint8_t* plane = occluder + 0x38;
            for (std::int32_t k = at<std::int32_t>(occluder, 0x88); k > 0; --k, plane += 16) {
                float lo, hi;
                joan::orig::box_plane_extent(joan::orig::arg::mptr(plane), joan::orig::arg::mptr(box), &lo, &hi);
                if (below_or_nan(lo, 0.0f)) {
                    state = 0;
                    break;
                }
            }
            if (state == 2) return 1;
        }
        return 0;
    }
};

#define HELPER3(name, addr, fn) JOAN_REPLACE("en3d/helpers3", name, addr, fn)
HELPER3("View3D::is_inside_view_6156b0", 0x006156b0, &H::camera_box_in_view);
HELPER3("View3D::is_occluded", 0x00616030, &H::camera_box_occluded);
#undef HELPER3

}  // namespace en3d_helpers3
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers21)
namespace {
namespace en3d_helpers20 {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::orig::helpers::plane_distance;

JOAN_COUNTER(c_occluder_visible, "Camera::occluder_visible");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // --- en3d/helpers21 ---------------------------------------------------------------------
    // 0x00615ba0 Camera::occluder_visible(count, points, unused): 0 when every point is behind
    // one frustum plane (+0x326) or all 8 frustum corners (+0x2c6) are on one side of the
    // points' plane.
    std::int32_t occluder_visible(std::int32_t count, const std::uint8_t* points, std::int32_t) {
        c_occluder_visible.hit();
        if (jo::self<jo::Camera>(this)->matrices_valid == 0) joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        for (std::int32_t p = 0; p < 6; ++p) {
            std::int32_t outside = 2;
            for (std::int32_t i = 0; i < count; ++i) {
                const float d = plane_distance(&jo::self<jo::Camera>(this)->frustum_planes[p * 4], points + i * 0xc);
                if (d >= 0.0f) {
                    outside = 0;
                    break;
                }
            }
            if (outside == 2) return 0;
        }
        float plane[4];
        joan::orig::as_plane_equation(plane)->init_425700();                                        // 0x00425700
        joan::orig::plane_from_points(joan::orig::arg::mptr(plane), joan::orig::arg::ptr(points), joan::orig::arg::ptr(points + 0xc), joan::orig::arg::ptr(points + 0x18));  // 0x0060d920
        std::int32_t front = 0, back = 0;
        for (std::int32_t k = 0; k < 8; ++k) {
            const float d = plane_distance(plane, reinterpret_cast<std::uint8_t*>(jo::self<jo::Camera>(this)->frustum_corners) + k * 0xc);
            if (d > 0.0f) ++front;
            if (!(d >= 0.0f)) ++back;
        }
        if (front == 8 || back == 8) return 0;
        return 1;
    }
};

#define HELPER21(name, addr, fn) JOAN_REPLACE("en3d/helpers21", name, addr, fn)
HELPER21("View3D::is_inside_view_615ba0", 0x00615ba0, &H::occluder_visible);
#undef HELPER21

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

using joan::call::thiscall;

struct Vec3 {
    float x, y, z;
};

struct H {
    // --- Camera --------------------------------------------------------------------------------------------
    // 0x00615d70: the box's bounding sphere (centre, half the diagonal) near test (0x00615e10).
    std::int32_t camera_box_near(const void* box) {
        float sphere[4];
        Vec3 c, d;
        const Vec3 centre = *joan::orig::arg::as<Vec3*>(joan::orig::as_box(box)->centre(&c));  // 0x0060d3a0
        std::memcpy(sphere, &centre, 12);
        const Vec3 diagonal = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&d, joan::orig::arg::ptr(static_cast<const std::uint8_t*>(box) + 0x0c), joan::orig::arg::ptr(box)));  // 0x0060cc90
        Vec3 v = diagonal;
        sphere[3] = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &v)) / 2.0f;  // length
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(this)->sphere_near(joan::orig::arg::mptr(sphere)));
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("View3D::is_near_camera", 0x00615d70, &H::camera_box_near);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/en3d_helpers18.cpp (en3d/helpers18)
namespace {
namespace en3d_helpers18 {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::orig::helpers::plane_distance;

using joan::orig::helpers::at;

void plane_from_points(void* out, const void* a, const void* b, const void* c) {
    joan::orig::plane_from_points(joan::orig::arg::mptr(out), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b), joan::orig::arg::ptr(c));  // 0x0060d920
}

JOAN_COUNTER(c_occluder_planes, "Camera::occluder_planes");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // 0x00616150 Camera::occluder_planes(occluder): occluder = 4 corners (+0, 12 bytes each),
    // planes +0x38 (16 bytes each), count +0x88. Front plane faces away from the eye; a side
    // plane through the eye and each edge is kept unless all 8 frustum corners (+0x2c6) are in
    // front of it. Returns 0 when the eye is on the occluder plane or 0x00615ba0 rejects it.
    std::int32_t occluder_planes(std::uint8_t* occ) {
        c_occluder_planes.hit();
        float eye[3];
        joan::orig::matrix_position(joan::orig::arg::mptr(eye), joan::orig::arg::ptr(joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Camera>(this)->bone_))->matrix())));  // 0x0060c310 of the bone matrix
        float plane[4];
        joan::orig::as_plane_equation(plane)->init_425700();  // 0x00425700
        plane_from_points(plane, occ, occ + 0xc, occ + 0x18);
        float d = plane_distance(plane, eye);
        if (!(d < 0.0f || d > 0.0f)) return 0;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(this)->occluder_visible(4, joan::orig::arg::mptr(occ), 2)) == 0) return 0;  // 0x00615ba0
        std::int32_t step = 1;
        if (d > 0.0f) {
            step = -1;
            plane_from_points(plane, occ, occ + 0x18, occ + 0xc);
            d = plane_distance(plane, eye);
        }
        if (d > -1.0f) {  // 0x9cf834
            float normal[3];
            joan::orig::vec3_sub_assign(joan::orig::arg::mptr(eye), joan::orig::arg::mptr(joan::orig::arg::as<void*>(joan::orig::plane_normal(joan::orig::arg::mptr(normal), joan::orig::arg::mptr(plane)))));  // eye -= normalize(plane.xyz)
            d = plane_distance(plane, eye);
        }
        std::memcpy(occ + 0x38, plane, 16);
        at<std::int32_t>(occ, 0x88) = 1;  // x64: needs Mesh+0x88 i32
        for (std::int32_t i = 0; i < 4; ++i) {
            std::int32_t j = i + step;
            if (j >= 4)
                j -= 4;
            else if (j < 0)
                j += 4;
            float side[4];
            joan::orig::as_plane_equation(side)->init_425700();
            plane_from_points(side, eye, occ + i * 0xc, occ + j * 0xc);
            std::int32_t all_in_front = 1;
            for (std::int32_t k = 0; k < 8; ++k) {
                if (!(plane_distance(side, reinterpret_cast<std::uint8_t*>(jo::self<jo::Camera>(this)->frustum_corners) + k * 0xc) > 0.0f)) {
                    all_in_front = 0;
                    break;
                }
            }
            if (all_in_front == 0) {
                std::memcpy(occ + 0x38 + at<std::int32_t>(occ, 0x88) * 16, side, 16);  // x64: needs Mesh+0x88 i32, Mesh+0x38 f32[20]
                at<std::int32_t>(occ, 0x88) = at<std::int32_t>(occ, 0x88) + 1;
            }
        }
        return 1;
    }
};

#define HELPER18(name, addr, fn) JOAN_REPLACE("en3d/helpers18", name, addr, fn)
HELPER18("View3D::calc_occluded_plane", 0x00616150, &H::occluder_planes);
#undef HELPER18

}  // namespace en3d_helpers18
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

struct H {
    // ------------------------------------------------------------------------ 0x00613ba0
    i32 is_ready() {
        if (jo::self<jo::Camera>(this)->bone_ == nullptr || jo::self<jo::Camera>(this)->target == nullptr || jo::self<jo::Camera>(this)->depth_target == nullptr) return 0;
        return 1;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("View3D::is_inited", 0x00613ba0, &H::is_ready);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers4.cpp (en3d/helpers4)
namespace {
namespace en3d_helpers4 {

using joan::call::thiscall;

using joan::call::cdecl_call;

// The camera's near plane - its real one while Scene::render widens the frustum for shadow casters
// (render::near_plane_override; not part of the original game).
const float* real_near_plane(void* camera) {
    const joan::render::NearPlaneOverride& o = joan::render::near_plane_override();
    return o.camera == camera ? o.plane : jo::self<jo::Camera>(camera)->frustum_planes;
}
using joan::orig::helpers::plane_distance;

using joan::orig::helpers::at;

bool below_or_nan(float a, float b) { return !(a >= b); }
bool below_or_nan(double a, double b) { return !(a >= b); }

struct Vec3 {
    float x, y, z;
};

struct H {
    // 0x00615ed0: 1 when the sphere is entirely in front of every plane of one occluder.
    std::int32_t camera_sphere_occluded(const float* s, float extra) {
        void* const scene = jo::view<void*>(jo::self<jo::Camera>(this)->scene);
        if (scene == nullptr) return 0;
        const float x = s[0], y = s[1], z = s[2];
        const float r = extra + s[3];
        const std::int32_t count = joan::orig::arg::as<std::int32_t>(joan::orig::as_scene(scene)->occluder_count());  // 0x005ef3b0
        for (std::int32_t i = 0; i < count; ++i) {
            std::uint8_t* const occluder = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene(scene)->occluder(joan::orig::arg::i32(i)));  // 0x005eed90
            std::int32_t state = 2;
            const float* p = reinterpret_cast<const float*>(occluder + 0x38);
            for (std::int32_t k = at<std::int32_t>(occluder, 0x88); k > 0; --k, p += 4) {  // x64: needs Mesh+0x88 i32 (plane count; planes f32[4][5] at +0x38)
                const float d = x * p[0] + y * p[1] + z * p[2] + p[3];
                if (below_or_nan(d, r)) {
                    state = 0;
                    break;
                }
            }
            if (state == 2) return 1;
        }
        return 0;
    }
    // 0x00615e10: the sphere's near side is closer than +0xc8 to the near plane.
    std::int32_t camera_sphere_near(const float* s) {
        return below_or_nan(plane_distance(real_near_plane(this), s) - s[3], jo::self<jo::Camera>(this)->near_threshold) ? 1 : 0;
    }

    // 0x006153c0: world point -> pixel (truncated) and depth; true when on screen and depth < 1.
    std::int32_t camera_project(const Vec3* p, std::int32_t* x, std::int32_t* y, float* depth) {
        joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        Vec3 out, in = *p;
        float m[16];
        std::memmove(m, jo::self<jo::Camera>(this)->view_projection, 64);
        joan::orig::d3dx::vec3_transform_coord(&out, &in, joan::orig::arg::mptr(m));  // D3DXVec3TransformCoord
        *x = static_cast<std::int32_t>(static_cast<std::int64_t>(out.x));  // _ftol
        const std::int32_t px = *x;
        *y = static_cast<std::int32_t>(static_cast<std::int64_t>(out.y));
        const std::int32_t py = *y;
        const float z = out.z;
        if (depth) *depth = z;
        return px >= 0 && py >= 0 && px < jo::self<jo::Camera>(this)->view_width && py < jo::self<jo::Camera>(this)->view_height &&
                       below_or_nan(static_cast<double>(z), 1.0)
                   ? 1
                   : 0;
    }
};

#define HELPER4(name, addr, fn) JOAN_REPLACE("en3d/helpers4", name, addr, fn)
HELPER4("View3D::is_occluded_615ed0", 0x00615ed0, &H::camera_sphere_occluded);
HELPER4("View3D::is_near_camera_615e10", 0x00615e10, &H::camera_sphere_near);
HELPER4("View3D::calc_zoom_pos", 0x006153c0, &H::camera_project);
#undef HELPER4

}  // namespace en3d_helpers4
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Camera::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_04::H::dtor_613700>(this)); }  // 0x613700
auto joan::orig::SysEn::init_613730() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_06::H::init_613730>(this)); }  // 0x613730
auto joan::orig::Camera::set_ranges(float a0, float a1, float a2) -> void { joan::bind::member<&::game_b90_05::H::set_ranges>(this, a0, a1, a2); }  // 0x613ca0
auto joan::orig::Camera::set_cf(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::en3d_b85_04::H::set_cf>(this, a0, a1, a2); }  // 0x613d30
auto joan::orig::Camera::set_scene(Scene* a0) -> void { joan::bind::member<&::game_world::H::camera_set_scene>(this, a0); }  // 0x613db0
auto joan::orig::Camera::bone() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::camera_bone>(this)); }  // 0x613de0
auto joan::orig::Viewport::set_int(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> void { joan::bind::member<&::game_b91_03::H::set_int>(this, a0, a1, a2, a3, a4, a5); }  // 0x614110
auto joan::orig::Camera::dirty() -> void { joan::bind::member<&::game_world::H::camera_dirty>(this); }  // 0x614170
auto joan::orig::Camera::update_matrices() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers2::H::camera_update_matrices>(this)); }  // 0x6141a0
auto joan::orig::Camera::begin_frame() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves80b::H::begin_frame>(this)); }  // 0x614630
auto joan::orig::Camera::release_scene() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_04::H::release_scene>(this)); }  // 0x6146e0
auto joan::orig::Camera::screen_ray(std::int32_t a0, std::int32_t a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_05::H::screen_ray>(this, a0, a1, a2)); }  // 0x6154d0
auto joan::orig::Camera::box_in_view(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers3::H::camera_box_in_view>(this, a0)); }  // 0x6156b0
auto joan::orig::Camera::occluder_visible(std::int32_t a0, void* a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers20::H::occluder_visible>(this, a0, a1, a2)); }  // 0x615ba0
auto joan::orig::Camera::box_near(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers6::H::camera_box_near>(this, a0)); }  // 0x615d70
auto joan::orig::Camera::box_occluded(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers3::H::camera_box_occluded>(this, a0)); }  // 0x616030
auto joan::orig::Camera::occluder_planes(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers18::H::occluder_planes>(this, a0)); }  // 0x616150
auto joan::orig::Camera::is_ready() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_06::H::is_ready>(this)); }  // 0x613ba0
auto joan::orig::Camera::project(void* a0, void* a1, void* a2, void* a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers4::H::camera_project>(this, a0, a1, a2, a3)); }  // 0x6153c0
auto joan::orig::Camera::sphere_near(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers4::H::camera_sphere_near>(this, a0)); }  // 0x615e10
auto joan::orig::Camera::sphere_occluded(void* a0, float a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers4::H::camera_sphere_occluded>(this, a0, a1)); }  // 0x615ed0
