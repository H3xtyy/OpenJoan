// game/unknown_camera3d.cpp: functions whose original source file is unknown (formerly game/_unplaced/camera3d.cpp;
// re/attribution.csv: between ww/en3d/view3d.cpp and ww/en3d/vshader2.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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

Device* game_device() { return global<Device*>(d::g_game_device); }

using joan::img::obj;

using joan::call::stdcall_call;

}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

using joan::orig::helpers::memset_;

JOAN_COUNTER(c_ctor613650, "Obj::ctor_613650");

struct H {
    // 0x00613650: four 6 x 0x10 arrays (eh vector ctor 0x00449af0, element ctor thunk
    // 0x0040a6c3 -> 0x00425700), then memset(this, 0, 0x4ca).
    void* ctor_613650() {
        c_ctor613650.hit();
        using VecCtor = void(JOAN_STDCALL*)(void*, std::uint32_t, std::uint32_t, void*);
        const VecCtor vec_ctor = reinterpret_cast<VecCtor>(static_cast<std::uintptr_t>(fn::code_value(fn::vector_constructor_iterator)));
        vec_ctor(jo::self<jo::Camera>(this)->frustum_planes, 0x10, 6, fn::code_ptr(fn::Plane4_Plane4_425700));
        vec_ctor(jo::self<jo::Camera>(this)->extra_planes, 0x10, 6, fn::code_ptr(fn::Plane4_Plane4_425700));
        vec_ctor(jo::self<jo::Camera>(this)->clip_planes_world, 0x10, 6, fn::code_ptr(fn::Plane4_Plane4_425700));
        vec_ctor(jo::self<jo::Camera>(this)->clip_planes_view, 0x10, 6, fn::code_ptr(fn::Plane4_Plane4_425700));
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::Camera)));  // 0x4ca bytes on x86
        return this;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Obj::ctor_613650", 0x00613650, &H::ctor_613650);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

using joan::call::thiscall;

u32 JOAN_CDECL set_flag_cb(void* p) { joan::orig::as_camera(p)->dirty(); return 0; }  // 0x00614170 (bone callback, called as void)

JOAN_REPLACE("game/b105_17", "set_flag_cb", 0x006139f0, &set_flag_cb);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b86_01.cpp (en3d/b86_01)
namespace {
namespace game_b86_01 {

using joan::call::thiscall;

JOAN_COUNTER(c_release_parts, "Model::release_parts");

struct H {
    // 0x00613a10 Model::release_parts(). ret 0. Scalar deleting destructors (flag 1) of +0xdc
    // (0x00613b00), +0 (0x005ab400), +0x54 (0x00613b50), each field cleared after. eax = the
    // last delete's result or 0.
    // The object is the Camera (ctor 0x00613650): read through jo::Camera, not the partial RenderView layout (three
    // pointers with untyped gaps between them), whose x64 offsets fell short of the Camera's (reorder 0xe4 vs 0xf4) and
    // handed Reorder::destruct garbage on the first x64 shutdown.
    std::uintptr_t release_parts() {
        c_release_parts.hit();
        void* const a = jo::view<void*>(jo::self<jo::Camera>(this)->reorder);
        if (a != nullptr) joan::orig::as_reorder(a)->destroy(1u);  // 0x00613b00
        jo::view<void*>(jo::self<jo::Camera>(this)->reorder) = nullptr;
        void* const b = jo::view<void*>(jo::self<jo::Camera>(this)->bone_);
        if (b != nullptr) joan::orig::as_bone(b)->delete_(1u);  // 0x005ab400
        jo::view<void*>(jo::self<jo::Camera>(this)->bone_) = nullptr;
        void* const c = jo::view<void*>(jo::self<jo::Camera>(this)->fog);
        std::uintptr_t r = 0;
        if (c != nullptr) r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_fog(c)->destroy(1u));  // 0x00613b50
        jo::view<void*>(jo::self<jo::Camera>(this)->fog) = nullptr;
        return r;
    }
};

#define EN3D86(name, addr, fn) JOAN_REPLACE("en3d/b86_01", name, addr, fn)
EN3D86("Model::release_parts", 0x00613a10, &H::release_parts);
#undef EN3D86

}  // namespace game_b86_01
}  // namespace

// ---------------------------------------- from modules/en3d_b87_02.cpp (en3d/b87_02)
namespace {
namespace en3d_b87_02 {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;  // operator delete

struct H {
    // 0x00613b50
    void* del_613b50(std::uint32_t flags) {
        joan::orig::as_fog(this)->destruct();  // 0x005bfdb0
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }

    // 0x00613b00
    void* del_613b00(std::uint32_t flags) {
        joan::orig::as_reorder(this)->destruct();  // 0x005e6110
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b87_02", name, addr, fn)
EN3D("Object::scalar_delete_613b50", 0x00613b50, &H::del_613b50);
EN3D("Object::scalar_delete_613b00", 0x00613b00, &H::del_613b00);
#undef EN3D

}  // namespace en3d_b87_02
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
namespace {
namespace game_b90_05 {

using joan::call::thiscall;

void* c_memset(void* p, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x006152b0
    // `this` is the camera (every caller passes 0x00a48ccc): its viewport, after the four pointers at the head.
    std::int32_t to_cell(std::int32_t x, std::int32_t y, std::int32_t* ox, std::int32_t* oy) {
        const jo::Camera* const c = jo::self<jo::Camera>(this);
        *ox = x - c->view_x1;
        *oy = y - c->view_y1;
        if (x < c->view_x1 || y < c->view_y1 || x > c->view_x2 || y > c->view_y2) return 0;
        return 1;
    }

    // 0x00616a00
    void viewport_set_size(std::int32_t w, std::int32_t h) {
        jo::view<std::int32_t>(jo::self<jo::Camera>(this)->fog_start) = w;
        jo::view<std::int32_t>(jo::self<jo::Camera>(this)->fog_end) = h;
        joan::orig::as_fog(jo::view<void*>(jo::self<jo::Camera>(this)->fog))->set(joan::orig::arg::u32(jo::view<std::int32_t>(jo::self<jo::Camera>(this)->fog_colour)), joan::orig::arg::f32(w), joan::orig::arg::f32(h));  // 0x005c0c30
    }
    // 0x00616a60
    void set_58(std::int32_t v) { jo::self<jo::Camera>(this)->fog_kind = v; }

    // 0x00616d50 (eax = this on exit)
    void* cam_params_init() {
        c_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::CamParams)));  // 0x28 bytes on x86
        jo::view<std::uint32_t>(jo::self<jo::CamParams>(this)->orbit[0]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::CamParams>(this)->orbit[1]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::CamParams>(this)->orbit[2]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::CamParams>(this)->fov_tan) = 0x3f800000u;
        jo::view<std::uint32_t>(jo::self<jo::CamParams>(this)->aspect) = 0x3f800000u;
        jo::view<std::uint32_t>(jo::self<jo::CamParams>(this)->orbit[3]) = 0x42c80000u;
        return this;
    }
};

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("Grid::to_cell", 0x006152b0, &H::to_cell);
B9005("Viewport::set_size", 0x00616a00, &H::viewport_set_size);
B9005("Viewport::set_58", 0x00616a60, &H::set_58);
B9005("CamParams::init", 0x00616d50, &H::cam_params_init);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

using joan::call::thiscall;

struct H {
    H* set_cc(u8 v) {  // 0x00613d80
        jo::self<jo::Camera>(this)->u8_cc = v;
        return this;
    }
    H* set_28(u32 v) {  // 0x006140e0
        jo::self<jo::Camera>(this)->u32_28 = v;
        return this;
    }
    u8* sub_12e() {  // 0x00616ef0
        joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        return reinterpret_cast<u8*>(jo::self<jo::Camera>(this)->projection);
    }
    u8* sub_1ae() {  // 0x00616fb0
        joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        return reinterpret_cast<u8*>(jo::self<jo::Camera>(this)->matrix_1ae);
    }
    H* set_d8(u32 v) {  // 0x00617040
        jo::self<jo::Camera>(this)->multiply_blend = v;
        return this;
    }
    u32 get_d8() { return jo::self<jo::Camera>(this)->multiply_blend; }  // 0x00617070
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::set_cc", 0x00613d80, &H::set_cc);
B("Obj::set_28", 0x006140e0, &H::set_28);
B("Obj::sub_12e", 0x00616ef0, &H::sub_12e);
B("Obj::sub_1ae", 0x00616fb0, &H::sub_1ae);
B("Obj::set_d8", 0x00617040, &H::set_d8);
B("Obj::get_d8", 0x00617070, &H::get_d8);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

using joan::call::thiscall;

struct H {
    void camera_set_frame(const void* frame) {                                           // 0x00614040
        std::memcpy(&jo::self<jo::Camera>(this)->prev_params, &jo::self<jo::Camera>(this)->params, 0x28);
        std::memcpy(&jo::self<jo::Camera>(this)->params, frame, 0x28);
        joan::orig::as_camera(this)->dirty();  // 0x00614170
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Camera::set_frame", 0x00614040, &H::camera_set_frame);
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
    H* camera_set_light(void* light) {  // 0x006169d0
        jo::self<jo::Camera>(this)->light_ = light;
        return this;
    }
    void* camera_light() { return jo::self<jo::Camera>(this)->light_; }  // 0x00616b40
    std::uint8_t* camera_matrix_ee() {                // 0x00616ec0
        joan::orig::as_camera(self())->update_matrices();
        return reinterpret_cast<std::uint8_t*>(jo::self<jo::Camera>(this)->inv_bone_matrix);
    }
    std::uint8_t* camera_view() {  // 0x00616f20
        joan::orig::as_camera(self())->update_matrices();
        return reinterpret_cast<std::uint8_t*>(jo::self<jo::Camera>(this)->view_);
    }
    std::uint8_t* camera_matrix_22e() {  // 0x00616f50
        joan::orig::as_camera(self())->update_matrices();
        return reinterpret_cast<std::uint8_t*>(jo::self<jo::Camera>(this)->matrix_22e_);
    }
    std::uint32_t camera_flags() {  // 0x00616fe0
        std::uint32_t flags = 0;
        if (jo::self<jo::Camera>(this)->light_ != nullptr) flags |= jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_kind);
        if (jo::self<jo::Camera>(this)->multiply_blend != 0) flags |= 0x20;
        return flags;
    }
    void camera_push_plane(const std::uint8_t* plane) {  // 0x006170a0 (max 6 clip planes)
        if (jo::self<jo::Camera>(this)->extra_plane_count >= 6) return;
        std::memcpy(&jo::self<jo::Camera>(this)->extra_planes[jo::self<jo::Camera>(this)->extra_plane_count * 4], plane, 16);
        ++jo::self<jo::Camera>(this)->extra_plane_count;
    }
    H* camera_clear_planes() {  // 0x00617120
        jo::self<jo::Camera>(this)->extra_plane_count = 0;
        return this;
    }
    std::uint8_t* camera_plane(std::int32_t i, std::uint8_t* out) {  // 0x006171a0
        std::memcpy(out, &jo::self<jo::Camera>(this)->extra_planes[i * 4], 16);
        return out;
    }
    std::int32_t camera_plane_count() { return jo::self<jo::Camera>(this)->extra_plane_count; }  // 0x006171f0
    void camera_end_clip() {                                               // 0x00617cf0
        if (jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->clip_planes_loaded) == 0) return;
        game_device()->SetRenderState(152, 0);  // CLIPPLANEENABLE
        jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->clip_planes_loaded) = 0;
    }
    std::uint32_t camera_617d50() { joan::orig::as_camera(self())->apply_clip_planes(); return 0; }  // 0x00617d50 -> 0x00617af0 (void; result unused)
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Camera::set_light", 0x006169d0, &H::camera_set_light);
HELPER("Camera::light", 0x00616b40, &H::camera_light);
HELPER("Camera::matrix_ee", 0x00616ec0, &H::camera_matrix_ee);
HELPER("Camera::view", 0x00616f20, &H::camera_view);
HELPER("Camera::matrix_22e", 0x00616f50, &H::camera_matrix_22e);
HELPER("Camera::flags", 0x00616fe0, &H::camera_flags);
HELPER("Camera::push_plane", 0x006170a0, &H::camera_push_plane);
HELPER("Camera::clear_planes", 0x00617120, &H::camera_clear_planes);
HELPER("Camera::plane", 0x006171a0, &H::camera_plane);
HELPER("Camera::plane_count", 0x006171f0, &H::camera_plane_count);
HELPER("Camera::end_clip", 0x00617cf0, &H::camera_end_clip);
HELPER("Camera::617d50", 0x00617d50, &H::camera_617d50);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

struct H {

    void* camera_frame(void* out) {                                // 0x006140a0: 10 dwords at +0xa0
        std::memcpy(out, &jo::self<jo::Camera>(this)->params, 0x28);
        return out;
    }
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("Camera::frame", 0x006140a0, &H::camera_frame);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

using joan::call::thiscall;

struct Vec3 {  // float members: Fog::set_range (0x005c0a70) takes two by value, and System V x86-64 passes floats in XMM registers (dwords go in integer registers)
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);

struct H {
    // 0x00616a90
    std::uint32_t sound_set(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_density) = a;
        jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_height_b) = b;
        jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_height_a) = c;
        const Vec3 v1 = jo::view<Vec3[2]>(jo::self<jo::Camera>(this)->scene->params)[1];
        const Vec3 v0 = jo::view<Vec3[2]>(jo::self<jo::Camera>(this)->scene->params)[0];
        joan::orig::as_fog(jo::view<void*>(jo::self<jo::Camera>(this)->fog))->set_range(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_colour), joan::orig::arg::f32(a), joan::orig::arg::f32(c), joan::orig::arg::f32(b), v0, v1);  // 0x005c0a70
        return 0;  // void callee; callers ignore the result
    }

    std::uint32_t get58() { return jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_kind); }    // 0x00616b60

    // 0x00616f80
    std::uint8_t* reset_name() {
        joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        return reinterpret_cast<std::uint8_t*>(jo::self<jo::Camera>(this)->d3d_projection);
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Sound3d::set", 0x00616a90, &H::sound_set);
B91("get58", 0x00616b60, &H::get58);
B91("Dialog::reset_name", 0x00616f80, &H::reset_name);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {

using joan::call::thiscall;

using joan::call::cdecl_call;

bool below_or_nan(float a, float b) { return !(a >= b); }  // x87 C0

struct Vec3 {
    float x, y, z;
};

// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    // 0x00615840: 0 outside, 1 crossing, 2 inside (extra planes, then the 6 frustum planes).
    std::int32_t camera_sphere_visible(const float* centre, float radius) {
        if (jo::self<jo::Camera>(this)->matrices_valid == 0) joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        const float x = centre[0], y = centre[1], z = centre[2];
        std::int32_t result = 2;
        const float neg = -radius;
        for (std::int32_t i = 0; i < jo::self<jo::Camera>(this)->extra_plane_count; ++i) {
            const float* p = &jo::self<jo::Camera>(this)->extra_planes[i * 4];
            const float d = x * p[0] + y * p[1] + z * p[2] + p[3];
            if (below_or_nan(d, neg)) return 0;
            if (below_or_nan(d, radius)) result = 1;
        }
        for (std::int32_t i = 0; i < 6; ++i) {
            const float* p = &jo::self<jo::Camera>(this)->frustum_planes[i * 4];
            const float d = x * p[0] + y * p[1] + z * p[2] + p[3];
            if (below_or_nan(d, neg)) return 0;
            if (below_or_nan(d, radius)) result = 1;
        }
        return result;
    }

    // 0x00617af0: load the enabled user clip planes (world or transformed set, by the global
    // 0xa52569) unless that set is already loaded (+0x4c6: 1 world, 2 transformed).
    void camera_apply_clip_planes() {
        const bool transformed = global<std::uint8_t>(d::g_a52569) != 0;
        if (jo::self<jo::Camera>(this)->clip_planes_loaded == 1 && !transformed) return;
        if (jo::self<jo::Camera>(this)->clip_planes_loaded == 2 && transformed) return;
        for (std::uint32_t i = 0; i < 6; ++i) {
            if ((jo::self<jo::Camera>(this)->clip_plane_mask & (1u << i)) == 0) continue;
            void* const plane = &(transformed ? jo::self<jo::Camera>(this)->clip_planes_view : jo::self<jo::Camera>(this)->clip_planes_world)[i * 4];
            game_device()->SetClipPlane(i, joan::orig::arg::as<const float*>(joan::orig::as_plane_equation(plane)->data()));  // 0x005fe900
        }
        game_device()->SetRenderState(152, jo::self<jo::Camera>(this)->clip_plane_mask);  // CLIPPLANEENABLE
        jo::self<jo::Camera>(this)->clip_planes_loaded = transformed ? 2 : 1;
    }

    // 0x00616c10: fog colour from R, G, B, A bytes (Camera::render's sky colour), then the fog
    // object (+0x54) gets colour / range (+0x60 / +0x64) and colour / +0x68..+0x70 / the
    // scene's two vectors.
    void camera_set_fog_colour(const std::uint8_t* rgba) {
        const std::uint32_t colour = static_cast<std::uint32_t>(rgba[3]) << 24 | static_cast<std::uint32_t>(rgba[0]) << 16 |
                                     static_cast<std::uint32_t>(rgba[1]) << 8 | rgba[2];
        joan::orig::as_colour(&jo::self<jo::Camera>(this)->fog_colour)->set(joan::orig::arg::u32(colour));  // 0x005e9610
        jo::self<jo::Camera>(this)->fog_colour.a = rgba[3];
        jo::self<jo::Camera>(this)->fog_colour.b = rgba[0];
        jo::self<jo::Camera>(this)->fog_colour.g = rgba[1];
        jo::self<jo::Camera>(this)->fog_colour.r = rgba[2];
        joan::orig::as_fog(jo::view<void*>(jo::self<jo::Camera>(this)->fog))->set(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_colour)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_start)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_end)));  // 0x005c0c30
        void* const scene = jo::view<void*>(jo::self<jo::Camera>(this)->scene);
        const Vec3 a = jo::view<Vec3[2]>(jo::as_scene(scene)->params)[0], b = jo::view<Vec3[2]>(jo::as_scene(scene)->params)[1];
        joan::orig::as_fog(jo::view<void*>(jo::self<jo::Camera>(this)->fog))->set_range(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_colour)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_density)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_height_a)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_height_b)), a, b);  // 0x005c0a70
    }
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Camera::sphere_visible", 0x00615840, &H::camera_sphere_visible);
HELPER2("Camera::apply_clip_planes", 0x00617af0, &H::camera_apply_clip_planes);
HELPER2("Camera::set_fog_colour", 0x00616c10, &H::camera_set_fog_colour);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

using joan::call::thiscall;

struct H {
    // 0x00615340
    // `this` is the camera (0x00a48ccc, lightsys.cpp): its viewport origin and size.
    std::int32_t grid_to_local(std::int32_t x, std::int32_t y, std::int32_t* out_x, std::int32_t* out_y) {
        const jo::Camera* const c = jo::self<jo::Camera>(this);
        *out_x = x + c->view_x1;
        *out_y = y + c->view_y1;
        return (x >= 0 && x < c->view_width && y >= 0 && y < c->view_height) ? 1 : 0;
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Grid::to_local", 0x00615340, &H::grid_to_local);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

using joan::call::thiscall;

using joan::call::cdecl_call;

float JOAN_CDECL exp_float(float x) {  // 0x00615160
    return static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::exp(joan::orig::arg::f64(static_cast<double>(x)))));
}

struct H {
    void* camera_matrix() {                                                          // 0x00616e90
        joan::orig::as_camera(this)->update_matrices();                    // 0x006141a0 update_matrices
        return joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Camera>(this)->bone_))->matrix());  // 0x0060ef80 bone matrix
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
HELPER16("exp_float", 0x00615160, &exp_float);
HELPER16("Camera::matrix", 0x00616e90, &H::camera_matrix);
#undef HELPER16

}  // namespace en3d_helpers16
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

struct Vec3 {
    float x, y, z;
};

struct H {
    void camera_plane(std::int32_t i, float* out) {  // 0x00617150: frustum plane i (+0x326)
        std::memcpy(out, &jo::self<jo::Camera>(this)->frustum_planes[i * 4], 16);
    }

    // --- Camera sphere tests (sphere = centre xyz + radius) ---------------------------------------
    // 0x006159f0: like 0x00615840 with radius = extra + sphere radius.
    std::int32_t camera_sphere_in_view(const float* s, float extra) {
        if (jo::self<jo::Camera>(this)->matrices_valid == 0) joan::orig::as_camera(this)->update_matrices();  // 0x006141a0
        const float x = s[0], y = s[1], z = s[2];
        const float r = extra + s[3];
        std::int32_t result = 2;
        const float neg = -r;
        for (std::int32_t i = 0; i < jo::self<jo::Camera>(this)->extra_plane_count; ++i) {
            const float* p = &jo::self<jo::Camera>(this)->extra_planes[i * 4];
            const float d = x * p[0] + y * p[1] + z * p[2] + p[3];
            if (below_or_nan(d, neg)) return 0;
            if (below_or_nan(d, r)) result = 1;
        }
        for (std::int32_t i = 0; i < 6; ++i) {
            const float* p = &jo::self<jo::Camera>(this)->frustum_planes[i * 4];
            const float d = x * p[0] + y * p[1] + z * p[2] + p[3];
            if (below_or_nan(d, neg)) return 0;
            if (below_or_nan(d, r)) result = 1;
        }
        return result;
    }
    // 0x00615e80: distance of the point from the near plane.
    float camera_near_distance(const float* p) {
        if (jo::self<jo::Camera>(this)->matrices_valid == 0) joan::orig::as_camera(this)->update_matrices();
        return plane_distance(real_near_plane(this), p);
    }
};

#define HELPER4(name, addr, fn) JOAN_REPLACE("en3d/helpers4", name, addr, fn)
HELPER4("Camera::plane", 0x00617150, &H::camera_plane);
HELPER4("Camera::sphere_in_view", 0x006159f0, &H::camera_sphere_in_view);
HELPER4("Camera::near_distance", 0x00615e80, &H::camera_near_distance);
#undef HELPER4

}  // namespace en3d_helpers4
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

using joan::call::thiscall;

struct H {
    // --- Camera --------------------------------------------------------------------------------------------
    // 0x00616b80: fog for this camera - per position (0x005c0460 with +0x58, the argument and
    // the camera position) when +0x5c and 0xa5255d & 2, else the plain fog state (0x005c09a0).
    std::uint32_t camera_apply_fog(std::uint32_t arg) {
        if (jo::self<jo::Camera>(this)->light_ != nullptr && (global<std::uint32_t>(d::g_a5255d) & 2) != 0)
            return joan::orig::as_fog(jo::view<void*>(jo::self<jo::Camera>(this)->fog))->apply(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Camera>(this)->fog_kind)), joan::orig::arg::i32(arg), jo::self<jo::Camera>(this)->params.position[0], jo::self<jo::Camera>(this)->params.position[1], jo::self<jo::Camera>(this)->params.position[2]);  // 0x005c0460
        joan::orig::as_fog(jo::view<void*>(jo::self<jo::Camera>(this)->fog))->off();  // 0x005c09a0
        return 0;  // void callee; callers ignore the result
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("Camera::apply_fog", 0x00616b80, &H::camera_apply_fog);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/en3d_helpers3_2.cpp (en3d/helpers3)
namespace {
namespace en3d_helpers3_2 {

using joan::orig::helpers::differs;

JOAN_COUNTER(c_build_frustum, "Camera::build_frustum");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // --- Camera (layout in en3d_helpers2.cpp) ------------------------------------------------------
    // 0x00616460: the 8 view-frustum corners in camera space (+0x15a = projection _34: 0 means
    // orthographic), to world space (+0x2c6, 8 x Vec3), their bounding box (+0x386) and the 6
    // frustum planes (+0x326) through them.
    void camera_build_frustum() {
        c_build_frustum.hit();
        float c[8][3];
        const float n = jo::self<jo::Camera>(this)->near_plane, fa = jo::self<jo::Camera>(this)->far_plane;
        if (differs(jo::self<jo::Camera>(this)->projection[11], 0.0f)) {
            const float sx = jo::self<jo::Camera>(this)->projection[11] / jo::self<jo::Camera>(this)->projection[0];
            const float sy = jo::self<jo::Camera>(this)->projection[11] / jo::self<jo::Camera>(this)->projection[5];
            const float corners[8][3] = {
                {-n * sx, -n * sy, n},    {n * sx, -n * sy, n},    {n * sx, n * sy, n},    {-n * sx, n * sy, n},
                {-fa * sx, -fa * sy, fa}, {fa * sx, -fa * sy, fa}, {fa * sx, fa * sy, fa}, {-fa * sx, fa * sy, fa},
            };
            std::memcpy(c, corners, sizeof c);
        } else {
            const float ax = 1.0f / jo::self<jo::Camera>(this)->projection[0];
            const float ay = 1.0f / jo::self<jo::Camera>(this)->projection[5];
            const float corners[8][3] = {
                {-ax, -ay, n},  {ax, -ay, n},  {ax, ay, n},  {-ax, ay, n},
                {-ax, -ay, fa}, {ax, -ay, fa}, {ax, ay, fa}, {-ax, ay, fa},
            };
            std::memcpy(c, corners, sizeof c);
        }
        std::uint8_t* const world = reinterpret_cast<std::uint8_t*>(jo::self<jo::Camera>(this)->frustum_corners);  // +0x2c6 on x86
        for (int i = 0; i < 8; ++i)
            import_call<void*>(d::iat_v3d_87_point_transform_3, world + i * 12, c[i], joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Camera>(this)->bone_))->matrix()));  // point_transform
        joan::orig::as_aabb(jo::self<jo::Camera>(this)->frustum_bounds)->set_point(joan::orig::arg::ptr(world));  // 0x0060d430
        for (int i = 1; i < 8; ++i) joan::orig::as_aabb(jo::self<jo::Camera>(this)->frustum_bounds)->extend(joan::orig::arg::ptr(world + i * 12));  // 0x0060d490
        auto corner = [world](int i) { return world + i * 12; };
        const int planes[6][3] = {{0, 1, 2}, {4, 7, 6}, {1, 0, 4}, {2, 1, 5}, {3, 2, 6}, {0, 3, 7}};
        for (int p = 0; p < 6; ++p)
            joan::orig::plane_from_points(joan::orig::arg::mptr(&jo::self<jo::Camera>(this)->frustum_planes[p * 4]), joan::orig::arg::ptr(corner(planes[p][0])), joan::orig::arg::ptr(corner(planes[p][1])), joan::orig::arg::ptr(corner(planes[p][2])));  // 0x0060d920
    }
};

#define HELPER3(name, addr, fn) JOAN_REPLACE("en3d/helpers3", name, addr, fn)
HELPER3("Camera::build_frustum", 0x00616460, &H::camera_build_frustum);
#undef HELPER3

}  // namespace en3d_helpers3_2
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_rotate, "Obj::rotate_616dd0");

struct H {
    // ------------------------------------------------------------------ 0x00616dd0
    void rotate_616dd0(float a, float b) {
        c_rotate.hit();
        const Fpu fpu{true};
        const float t1 = jo::self<jo::CamParams>(this)->orbit[0] / global<float>(d::flt_1);
        const float cs = joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(fpu.sub(global<double>(d::dbl_3_141592653589793), t1))));  // cos
        const float t2 = jo::self<jo::CamParams>(this)->orbit[0] / global<float>(d::flt_1);
        const float sn = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(fpu.sub(global<double>(d::dbl_3_141592653589793), t2))));  // sin
        jo::self<jo::CamParams>(this)->position[0] = (cs * b + sn * a) + jo::self<jo::CamParams>(this)->position[0];
        jo::self<jo::CamParams>(this)->position[1] = (cs * a - sn * b) + jo::self<jo::CamParams>(this)->position[1];
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::rotate_616dd0", 0x00616dd0, &H::rotate_616dd0);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2_2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2_2 {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::call::thiscall;
using joan::call::cdecl_call;
bool below_or_nan(float a, float b) { return !(a >= b); }  // x87 C0

// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    // 0x00617220: the same for an axis-aligned box; frustum plane 0 (near) is not tested.
    std::int32_t camera_box_visible(const void* box) {
        joan::orig::as_camera(this)->update_matrices();  // 0x006141a0 (unconditional here)
        std::int32_t result = 2;
        for (std::int32_t i = 0; i < jo::self<jo::Camera>(this)->extra_plane_count; ++i) {
            float lo, hi;
            joan::orig::box_plane_extent(joan::orig::arg::mptr(&jo::self<jo::Camera>(this)->extra_planes[i * 4]), joan::orig::arg::mptr(box), &lo, &hi);  // 0x0060dc80
            if (below_or_nan(hi, 0.0f)) return 0;
            if (below_or_nan(lo, 0.0f)) result = 1;
        }
        for (std::int32_t i = 1; i < 6; ++i) {
            float lo, hi;
            joan::orig::box_plane_extent(joan::orig::arg::mptr(&jo::self<jo::Camera>(this)->frustum_planes[i * 4]), joan::orig::arg::mptr(box), &lo, &hi);
            if (below_or_nan(hi, 0.0f)) return 0;
            if (below_or_nan(lo, 0.0f)) result = 1;
        }
        return result;
    }
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Camera::box_visible", 0x00617220, &H::camera_box_visible);
#undef HELPER2

}  // namespace en3d_helpers2_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers23)
namespace {
namespace en3d_helpers22 {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::orig::helpers::at;

std::int32_t ftol(float v) { return joan::hook::x87::ftol(static_cast<double>(v)); }

JOAN_COUNTER(c_water, "Camera::water_receivers");

struct H {
    // 0x00617370 Camera::water_receivers: for each receiver region of the scene (0x005ef690)
    // whose 2D box contains the eye or lies within 2000 units (truncated squared edge
    // distances, 0x00664d70), recompute the projected texture matrix (0x005d8650) of every lit
    // material whose texture stage is a projected (type 4) texture.
    void water_receivers() {
        c_water.hit();
        std::int32_t count = 0;
        std::uint8_t** const regions = joan::orig::arg::as<std::uint8_t**>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Camera>(this)->scene))->regions(joan::orig::arg::mptr(static_cast<void*>(this)), &count));  // 0x005ef690
        if (regions == nullptr) return;
        float eye[3];
        joan::orig::matrix_position(joan::orig::arg::mptr(eye), joan::orig::arg::ptr(joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Camera>(this)->bone_))->matrix())));  // 0x0060c310
        const float x = eye[0], y = eye[1];
        for (std::int32_t i = count - 1; i >= 0; --i) {
            std::uint8_t* const plane = jo::view<std::uint8_t*>(jo::as_scene_s(regions[i])->plane);
            const std::uint8_t* r = regions[i];
            auto x0 = [&] { return jo::view<float>(jo::as_scene_s(regions[i])->desc[0]); };  // the sector box (min xyz, max xyz)
            auto y0 = [&] { return jo::view<float>(jo::as_scene_s(regions[i])->desc[1]); };
            auto x1 = [&] { return jo::view<float>(jo::as_scene_s(regions[i])->desc[3]); };
            auto y1 = [&] { return jo::view<float>(jo::as_scene_s(regions[i])->desc[4]); };
            (void)r;
            bool near = false;
            if (!(x >= x0()) || x > x1() || !(y >= y0()) || y > y1()) {
                auto edge = [&](float ax, float ay, float bx, float by) {
                    return joan::orig::arg::as<float>(joan::orig::point_segment_dist2(joan::orig::arg::f32(ax), joan::orig::arg::f32(ay), joan::orig::arg::f32(bx), joan::orig::arg::f32(by), joan::orig::arg::f32(x), joan::orig::arg::f32(y)));  // 0x00664d70
                };
                std::int32_t m = ftol(edge(x0(), y0(), x1(), y0()));
                float best = static_cast<float>(m);
                float v = edge(x1(), y0(), x1(), y1()) > best ? static_cast<float>(m) : edge(x1(), y0(), x1(), y1());
                m = ftol(v);
                best = static_cast<float>(m);
                v = edge(x0(), y1(), x1(), y1()) > best ? static_cast<float>(m) : edge(x0(), y1(), x1(), y1());
                m = ftol(v);
                best = static_cast<float>(m);
                v = edge(x0(), y0(), x0(), y1()) > best ? static_cast<float>(m) : edge(x0(), y0(), x0(), y1());
                m = ftol(v);
                near = m < 4000000;
            } else {
                near = true;
            }
            if (!near) continue;
            if (plane == nullptr || joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->visible()) == 0) continue;  // 0x005d0480
            for (std::int32_t mi = jo::as_plane(plane)->material_count - 1; mi >= 0; --mi) {
                std::uint8_t* const material = reinterpret_cast<std::uint8_t*>(&jo::as_plane(plane)->materials[mi]);  // 0x54-byte records on x86 (Plane::materials is a Material array: x64 stride sizeof(Material))
                if (!(jo::view<float>(jo::as_material(material)->u32_18) > 0.0f)) continue;
                std::int32_t stage = -1;
                if ((jo::as_material(material)->texture_mode & 0x1000000) != 0) {
                    const std::uint8_t* const anim =
                        joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_db(img::obj(d::g_texture_effect_res))->entry(joan::orig::arg::u32(jo::as_material(material)->texture_mode & 0xffffu)));  // 0x004b27b0
                    const std::uint32_t set = (jo::as_material(material)->texture_mode >> 0x10) & 0xff;
                    for (std::int32_t s = 0; s < jo::view<std::int16_t>(jo::as_tex_script_rec(anim)->stage_count[set]); ++s) {
                        if (jo::as_tex_script_rec(anim)->projected_flags[(set << 2) + s] != 0) {
                            stage = s;
                            break;
                        }
                    }
                } else {
                    stage = 0;
                }
                if (stage == -1) continue;
                std::uint8_t* const texture = jo::view<std::uint8_t*>(jo::as_material(material)->textures[stage]);
                if (texture == nullptr || jo::view<std::int32_t>(jo::as_texture(texture)->usage) != 4) continue;
                float matrix[16];
                if ((joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(plane)->water_texture_5d8650(joan::orig::arg::u32(mi), static_cast<void*>(this), static_cast<void*>(texture), static_cast<void*>(matrix))) & 0xff) == 0)  // 0x005d8650
                    continue;
                std::memcpy(jo::view<std::uint8_t*>(jo::as_plane(plane)->material_matrices) + (mi << 6), matrix, 64);
                jo::view<std::int32_t*>(jo::as_plane(plane)->material_ids)[mi] = stage;
            }
        }
    }
};

#define HELPER23(name, addr, fn) JOAN_REPLACE("en3d/helpers23", name, addr, fn)
HELPER23("Camera::water_receivers", 0x00617370, &H::water_receivers);
#undef HELPER23

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/game_b111_13.cpp (game/b111_13)
namespace {
namespace game_b111_13 {

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------------ 0x006179a0
    void lights_set(u32 idx, u32 enable, const u8* a, const u8* b) {
        // idx > 5: the original ORs / clears an uninitialised stack mask - original body.
        if (idx > 5) {
            thiscall(fn::Lights_set_body, this, idx, enable, a, b);
            return;
        }
        const u32 mask = 1u << idx;
        if ((enable & 0xffu) != 0) {
            jo::self<jo::Camera>(this)->clip_plane_mask = jo::self<jo::Camera>(this)->clip_plane_mask | mask;
            // a / b: plane equations (4 floats, copied as dwords)
            const u32* const pa = reinterpret_cast<const u32*>(a);
            const u32* const pb = reinterpret_cast<const u32*>(b);
            float* const d = &jo::self<jo::Camera>(this)->clip_planes_world[idx * 4];
            jo::view<u32>(d[0]) = pa[0];
            jo::view<u32>(d[1]) = pa[1];
            jo::view<u32>(d[2]) = pa[2];
            jo::view<u32>(d[3]) = pa[3];
            float* const d2 = &jo::self<jo::Camera>(this)->clip_planes_view[idx * 4];
            jo::view<u32>(d2[0]) = pb[0];
            jo::view<u32>(d2[1]) = pb[1];
            jo::view<u32>(d2[2]) = pb[2];
            jo::view<u32>(d2[3]) = pb[3];
        } else {
            jo::self<jo::Camera>(this)->clip_plane_mask = jo::self<jo::Camera>(this)->clip_plane_mask & ~mask;
        }
    }
};

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_13", name, addr, fn)
B111("Lights::set", 0x006179a0, &H::lights_set);
#undef B111

}  // namespace game_b111_13
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Camera::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_04::H::ctor_613650>(this)); }  // 0x613650
auto joan::orig::Model::release_parts() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_01::H::release_parts>(this)); }  // 0x613a10
auto joan::orig::Reorder::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b87_02::H::del_613b00>(this, a0)); }  // 0x613b00
auto joan::orig::Fog::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b87_02::H::del_613b50>(this, a0)); }  // 0x613b50
auto joan::orig::Camera::set_frame(void* a0) -> void { joan::bind::member<&::game_world::H::camera_set_frame>(this, a0); }  // 0x614040
auto joan::orig::Camera::frame(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::camera_frame>(this, a0)); }  // 0x6140a0
auto joan::orig::exp_float(float a0) -> float { return joan::bind::conv<float>(joan::bind::free<&::en3d_helpers16::exp_float>(a0)); }  // 0x615160
auto joan::orig::Grid::to_cell(std::int32_t a0, std::int32_t a1, void* a2, void* a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_05::H::to_cell>(this, a0, a1, a2, a3)); }  // 0x6152b0
auto joan::orig::Grid::to_local(std::int32_t a0, std::int32_t a1, void* a2, void* a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves80b::H::grid_to_local>(this, a0, a1, a2, a3)); }  // 0x615340
auto joan::orig::Camera::sphere_visible(void* a0, float a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers2::H::camera_sphere_visible>(this, a0, a1)); }  // 0x615840
auto joan::orig::Camera::sphere_in_view(void* a0, float a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers4::H::camera_sphere_in_view>(this, a0, a1)); }  // 0x6159f0
auto joan::orig::Camera::near_distance(void* a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::en3d_helpers4::H::camera_near_distance>(this, a0)); }  // 0x615e80
auto joan::orig::Camera::build_frustum() -> void { joan::bind::member<&::en3d_helpers3_2::H::camera_build_frustum>(this); }  // 0x616460
auto joan::orig::Camera::set_light(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::camera_set_light>(this, a0)); }  // 0x6169d0
auto joan::orig::Viewport::set_size(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b90_05::H::viewport_set_size>(this, a0, a1); }  // 0x616a00
auto joan::orig::Viewport::set_58(std::int32_t a0) -> void { joan::bind::member<&::game_b90_05::H::set_58>(this, a0); }  // 0x616a60
auto joan::orig::Camera::set_fog_range(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_03::H::sound_set>(this, a0, a1, a2)); }  // 0x616a90
auto joan::orig::Camera::light() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::camera_light>(this)); }  // 0x616b40
auto joan::orig::Camera::get_58() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_03::H::get58>(this)); }  // 0x616b60
auto joan::orig::Camera::apply_fog(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers6::H::camera_apply_fog>(this, a0)); }  // 0x616b80
auto joan::orig::Camera::set_fog_colour(void* a0) -> void { joan::bind::member<&::en3d_helpers2::H::camera_set_fog_colour>(this, a0); }  // 0x616c10
auto joan::orig::CameraParams::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_05::H::cam_params_init>(this)); }  // 0x616d50
auto joan::orig::CameraParams::rotate(float a0, float a1) -> void { joan::bind::member<&::game_b100_11::H::rotate_616dd0>(this, a0, a1); }  // 0x616dd0
auto joan::orig::Camera::matrix() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::camera_matrix>(this)); }  // 0x616e90
auto joan::orig::Camera::matrix_ee() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::camera_matrix_ee>(this)); }  // 0x616ec0
auto joan::orig::Camera::view() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::camera_view>(this)); }  // 0x616f20
auto joan::orig::Camera::matrix_22e() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::camera_matrix_22e>(this)); }  // 0x616f50
auto joan::orig::Dialog::reset_name() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_03::H::reset_name>(this)); }  // 0x616f80
auto joan::orig::Camera::flags() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::camera_flags>(this)); }  // 0x616fe0
auto joan::orig::Camera::push_plane(void* a0) -> void { joan::bind::member<&::en3d_helpers::H::camera_push_plane>(this, a0); }  // 0x6170a0
auto joan::orig::Camera::clear_planes() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::camera_clear_planes>(this)); }  // 0x617120
auto joan::orig::Camera::plane_617150(std::int32_t a0, void* a1) -> void { joan::bind::member<&::en3d_helpers4::H::camera_plane>(this, a0, a1); }  // 0x617150
auto joan::orig::Camera::plane_6171a0(std::int32_t a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::camera_plane>(this, a0, a1)); }  // 0x6171a0
auto joan::orig::Camera::plane_count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers::H::camera_plane_count>(this)); }  // 0x6171f0
auto joan::orig::Camera::box_visible(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers2_2::H::camera_box_visible>(this, a0)); }  // 0x617220
auto joan::orig::Camera::water_receivers() -> void { joan::bind::member<&::en3d_helpers22::H::water_receivers>(this); }  // 0x617370
auto joan::orig::Camera::set_clip_plane(std::uint32_t a0, std::uint32_t a1, void* a2, void* a3) -> void { joan::bind::member<&::game_b111_13::H::lights_set>(this, a0, a1, a2, a3); }  // 0x6179a0
auto joan::orig::Camera::apply_clip_planes() -> void { joan::bind::member<&::en3d_helpers2::H::camera_apply_clip_planes>(this); }  // 0x617af0
auto joan::orig::Camera::end_clip() -> void { joan::bind::member<&::en3d_helpers::H::camera_end_clip>(this); }  // 0x617cf0
auto joan::orig::Camera::m_617d50() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::camera_617d50>(this)); }  // 0x617d50
