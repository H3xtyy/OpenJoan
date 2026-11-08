// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/light.cpp: functions of the original d:\ww\en3d\light.cpp (re/attribution.csv). The file name is the Restaurant
// Empire 2 engine's (en3d/light.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::img::obj;

using u32 = std::uint32_t;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/en3d_helpers36.cpp (en3d/helpers36)
namespace {
namespace en3d_helpers36 {

struct H {
    // 0x005c5830 Light::Light(). ret 0, eax = this.
    void* light_ctor() {
        jo::self<jo::Light>(this)->bone_ = nullptr;
        jo::self<jo::Light>(this)->children = nullptr;
        return this;
    }
};

#define HELPER36(name, addr, fn) JOAN_REPLACE("en3d/helpers36", name, addr, fn)
HELPER36("Light::Light", 0x005c5830, &H::light_ctor);
#undef HELPER36

}  // namespace en3d_helpers36
}  // namespace

// ---------------------------------------- from modules/en3d_helpers32.cpp (en3d/helpers32)
namespace {
namespace en3d_helpers32 {

JOAN_COUNTER(c_camera_init, "Camera::init");

void* op_new(std::uint32_t size) { return joan::crt::operator_new(joan::orig::arg::u32(size)); }

struct H {
    // 0x005c58a0 Camera::init(type).
    std::uint32_t camera_init(std::uint32_t type) {
        c_camera_init.hit();
        jo::self<jo::Light>(this)->type = type;
        switch (jo::self<jo::Light>(this)->type) {
            case 0:
            case 1:
                jo::view<void*>(jo::self<jo::Light>(this)->bone_) = nullptr;
                break;
            case 2:
            case 3: {
                void* ctl = op_new(static_cast<std::uint32_t>(sizeof(jo::Bone)));  // 0xa0 on x86
                if (ctl != nullptr)
                    ctl = joan::orig::arg::as<void*>(joan::orig::as_bone(ctl)->Bone_60eae0(static_cast<void*>(this), joan::orig::arg::mptr(fn::code_value(fn::cb_5c5b30))));  // 0x0060eae0
                jo::view<void*>(jo::self<jo::Light>(this)->bone_) = ctl;
                break;
            }
            default:
                break;
        }
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->position[0]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->position[1]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->position[2]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->orbit_a) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->orbit_b) = 0x3fc90fdbu;  // pi/2
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->orbit_c) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->theta) = 0x3fc90fdbu;
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->phi) = 0x3fc90fdbu;
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->range) = 0x461c4000u;  // 10000
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->f32_30) = 0x3f800000u;
        joan::orig::as_color(&jo::self<jo::Light>(this)->colour)->set_bgr(joan::orig::arg::mptr(img::obj(d::g_init_white)));  // 0x005e9430
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->bounds[0]) = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->position[0]);
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->bounds[1]) = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->position[1]);
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->bounds[2]) = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->position[2]);
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->bounds[3]) = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->range);
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->attenuation0) = 0x3f800000u;
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->attenuation1) = 0x3f800000u;
        jo::view<std::uint32_t>(jo::self<jo::Light>(this)->attenuation2) = 0;
        jo::self<jo::Light>(this)->casts_shadows = 0;
        jo::self<jo::Light>(this)->active_ = 1;
        jo::self<jo::Light>(this)->flag_59_ = 0;
        joan::orig::as_light(this)->invalidate();  // 0x005c5c00
        return 0;  // void callee; Camera::init's callers drop the eax
    }
};

#define HELPER32(name, addr, fn) JOAN_REPLACE("en3d/helpers32", name, addr, fn)
HELPER32("Light::init", 0x005c58a0, &H::camera_init);
#undef HELPER32

}  // namespace en3d_helpers32
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers20)
namespace {
namespace en3d_helpers20 {

using joan::call::import_call;

JOAN_COUNTER(c_light_update, "Light::update");

struct H {
    // --- en3d/helpers20 ---------------------------------------------------------------------
    void light_invalidate() { jo::self<jo::Light>(this)->dirty = 1; }  // 0x005c5c00

    // 0x005c5c30 Light::update: rebuilds the D3DLIGHT8 at +0x5e when dirty (+0x5a).
    std::int32_t light_update() {
        c_light_update.hit();
        if (jo::view<void*>(jo::self<jo::Light>(this)->bone_) != nullptr && joan::orig::arg::as<std::int32_t>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Light>(this)->bone_))->update()) != 0) light_invalidate_thunk();
        if (jo::self<jo::Light>(this)->dirty == 0) return 0;
        std::memset(joan::orig::arg::mptr(jo::self<jo::Light>(this)->d3d_light), 0, sizeof(jo::self<jo::Light>(this)->d3d_light));  // memset
        float m[16];
        joan::orig::orbit_matrix(joan::orig::arg::mptr(m), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Light>(this)->orbit_a)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Light>(this)->orbit_b)), joan::orig::arg::f32(0u), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Light>(this)->orbit_c)));  // 0x0060bd00
        import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, m, jo::self<jo::Light>(this)->position);  // matrix_multiply_translation
        if (jo::view<void*>(jo::self<jo::Light>(this)->bone_) != nullptr) {
            std::memcpy(jo::view<void*>(jo::self<jo::Light>(this)->bone_), m, 64);
            joan::orig::as_bone(jo::view<void*>(jo::self<jo::Light>(this)->bone_))->invalidate();  // 0x0060ee10
        }
        void* const ambient = img::obj(d::g_init_black);
        // k: D3DLIGHT8 dword of the colour (1 diffuse +4, 5 specular +0x14, 9 ambient +0x24)
        auto to_float_colour = [&](std::int32_t k) { joan::orig::as_colour(&jo::self<jo::Light>(this)->colour)->to_float_5e9130(joan::orig::arg::mptr(&jo::self<jo::Light>(this)->d3d_light[k]), joan::orig::arg::f32(jo::self<jo::Light>(this)->f32_30)); };  // 0x005e9130
        auto global_colour = [&](std::int32_t k) { joan::orig::as_colour(ambient)->to_float_5e9260(joan::orig::arg::mptr(&jo::self<jo::Light>(this)->d3d_light[k])); };                        // 0x005e9260
        switch (jo::self<jo::Light>(this)->type) {
            case 0:
                jo::self<jo::Light>(this)->d3d_light[0] = 3;
                to_float_colour(9);
                global_colour(1);
                global_colour(5);
                break;
            case 1:
                jo::self<jo::Light>(this)->d3d_light[0] = 3;
                global_colour(9);
                to_float_colour(1);
                to_float_colour(5);
                break;
            case 2:
                jo::self<jo::Light>(this)->d3d_light[0] = 2;
                global_colour(9);
                to_float_colour(1);
                to_float_colour(5);
                break;
            case 3:
                jo::self<jo::Light>(this)->d3d_light[0] = 1;
                global_colour(9);
                to_float_colour(1);
                to_float_colour(5);
                break;
            default:
                break;
        }
        float v[3];
        if (jo::view<void*>(jo::self<jo::Light>(this)->bone_) != nullptr) {
            joan::orig::as_bone(jo::view<void*>(jo::self<jo::Light>(this)->bone_))->update();  // 0x0060ee60 Bone::update
            joan::orig::matrix_position(joan::orig::arg::mptr(v), joan::orig::arg::ptr(joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Light>(this)->bone_))->matrix())));  // 0x0060c310 position
            std::memcpy(&jo::self<jo::Light>(this)->d3d_light[13], v, 12);
            joan::orig::matrix_axis_z(joan::orig::arg::mptr(v), joan::orig::arg::ptr(joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Light>(this)->bone_))->matrix())));  // 0x0060c220 direction
            std::memcpy(&jo::self<jo::Light>(this)->d3d_light[16], v, 12);
        } else {
            joan::orig::matrix_position(joan::orig::arg::mptr(v), joan::orig::arg::ptr(m));
            std::memcpy(&jo::self<jo::Light>(this)->d3d_light[13], v, 12);
            joan::orig::matrix_axis_z(joan::orig::arg::mptr(v), joan::orig::arg::ptr(m));
            std::memcpy(&jo::self<jo::Light>(this)->d3d_light[16], v, 12);
        }
        jo::self<jo::Light>(this)->d3d_light[19] = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->range);  // range
        jo::self<jo::Light>(this)->d3d_light[20] = 0x3f800000;              // falloff 1.0
        jo::self<jo::Light>(this)->d3d_light[21] = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->attenuation0);  // attenuation
        jo::self<jo::Light>(this)->d3d_light[22] = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->attenuation1);
        jo::self<jo::Light>(this)->d3d_light[23] = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->attenuation2);
        jo::self<jo::Light>(this)->d3d_light[24] = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->theta);  // theta
        jo::self<jo::Light>(this)->d3d_light[25] = jo::view<std::uint32_t>(jo::self<jo::Light>(this)->phi);  // phi
        jo::self<jo::Light>(this)->dirty = 0;
        return 1;
    }
    void light_invalidate_thunk() { joan::orig::as_light(this)->invalidate(); }  // 0x005c5c00
};

#define HELPER20(name, addr, fn) JOAN_REPLACE("en3d/helpers20", name, addr, fn)
#define HELPER21(name, addr, fn) JOAN_REPLACE("en3d/helpers21", name, addr, fn)
HELPER20("Light::mark_changed", 0x005c5c00, &H::light_invalidate);
HELPER21("Light::recalc_change", 0x005c5c30, &H::light_update);
#undef HELPER20
#undef HELPER21

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint32_t light_active() {  // 0x005c6070
        if (jo::self<jo::Light>(this)->active_ == 0 || jo::self<jo::Light>(this)->flag_59_ != 0) return 0;  // 0x005c6790 reads +0x59
        switch (jo::self<jo::Light>(this)->type) {
        case 0:
        case 1:
            return 1;
        case 2:
        case 3:
            return jo::self<jo::Light>(this)->bone_ != nullptr ? 1 : 0;
        default:
            return 0;
        }
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Light::can_render", 0x005c6070, &H::light_active);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_helpers7.cpp (en3d/helpers7)
namespace {
namespace en3d_helpers7 {

using Device = joan::d3d8::IDirect3DDevice8;

Device* game_device() { return global<Device*>(d::g_game_device); }
constexpr img::Addr kSysEn(d::g_sys3d);

// Ambient light: the sum of the ambient lights (0x005c65e0) as D3DCOLOR (0x005e91e0) -> AMBIENT.
void apply_ambient() {
    std::uint32_t colour = joan::orig::arg::as<std::uint32_t>(joan::orig::lights_ambient());
    std::uint32_t d3d = 0;
    joan::orig::as_colour(&colour)->to_argb(&d3d);
    game_device()->SetRenderState(139, d3d);  // AMBIENT
}

struct H {
    // --- Light (morph.cpp area, 0x005c6xxx) -------------------------------------------------------
    // +0x00 type (0 = ambient), +0x5e D3DLIGHT8, +0xc6 device light index (-1 none).
    // 0x005c6110: switch on - ambient lights join the ambient list (0xa6785c, count 0xa6789c),
    // the others get a device light index (SysEn 0x005f8030 / free 0x005f80f0).
    void light_enable() {
        joan::orig::as_light(this)->update();  // 0x005c5c30 update
        if (jo::self<jo::Light>(this)->type == 0) {
            const std::int32_t n = global<std::int32_t>(d::g_ambient_light_count);
            (&img::global<void*>(d::g_ambient_light_list))[n] = this;
            global<std::int32_t>(d::g_ambient_light_count) = n + 1;
            apply_ambient();
            return;
        }
        jo::self<jo::Light>(this)->enabled = -1;
        const std::uint32_t index = joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(img::obj(kSysEn))->light_slot());
        if (game_device()->SetLight(index, jo::self<jo::Light>(this)->d3d_light) == 0 && game_device()->LightEnable(index, 1) == 0) {
            jo::view<std::uint32_t>(jo::self<jo::Light>(this)->enabled) = index;
            return;
        }
        joan::orig::as_sys_en(img::obj(kSysEn))->light_slot_free(joan::orig::arg::i32(index));
    }
    // 0x005c6250: switch off.
    void light_disable() {
        if (jo::self<jo::Light>(this)->type == 0) {
            void** const list = &img::global<void*>(d::g_ambient_light_list);
            const std::int32_t n = global<std::int32_t>(d::g_ambient_light_count);
            if (list[n - 1] == this) {
                global<std::int32_t>(d::g_ambient_light_count) = n - 1;
            } else {
                std::int32_t i = n;
                while (i > 0 && list[i - 1] != this) --i;
                if (i > 0) {
                    joan::orig::as_misc(img::obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(img::obj(d::g_ambient_light_list)), joan::orig::arg::i32(global<std::int32_t>(d::g_ambient_light_count)), static_cast<std::int32_t>(sizeof(void*)), joan::orig::arg::i32(i));  // 0x00591070 remove element
                    global<std::int32_t>(d::g_ambient_light_count) = global<std::int32_t>(d::g_ambient_light_count) - 1;
                }
            }
            apply_ambient();
            return;
        }
        if (jo::self<jo::Light>(this)->enabled == -1) return;
        game_device()->LightEnable(jo::view<std::uint32_t>(jo::self<jo::Light>(this)->enabled), 0);
        joan::orig::as_sys_en(img::obj(kSysEn))->light_slot_free(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Light>(this)->enabled)));
        jo::self<jo::Light>(this)->enabled = -1;
    }
};

#define HELPER7(name, addr, fn) JOAN_REPLACE("en3d/helpers7", name, addr, fn)
HELPER7("Light::render_begin", 0x005c6110, &H::light_enable);
HELPER7("Light::render_end", 0x005c6250, &H::light_disable);
#undef HELPER7

}  // namespace en3d_helpers7
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
//
//    - small: Obj::get_4 0x005c5be0.
namespace {
namespace game_b104_10 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

template <class T>
T& at(const void* p, i32 offset) {
    return *reinterpret_cast<T*>(static_cast<std::uint8_t*>(const_cast<void*>(p)) + offset);
}

struct H {
    jo::Bone* get_4() { return jo::self<jo::Light>(this)->bone_; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("Light::get_frame", 0x005c5be0, &H::get_4);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Light::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::light_ctor>(this)); }  // 0x5c5830
auto joan::orig::Camera::init(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers32::H::camera_init>(this, a0)); }  // 0x5c58a0
auto joan::orig::Light::invalidate() -> void { joan::bind::member<&::en3d_helpers20::H::light_invalidate>(this); }  // 0x5c5c00
auto joan::orig::Light::update() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers20::H::light_update>(this)); }  // 0x5c5c30
auto joan::orig::Light::active() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::light_active>(this)); }  // 0x5c6070
auto joan::orig::Light::enable() -> void { joan::bind::member<&::en3d_helpers7::H::light_enable>(this); }  // 0x5c6110
auto joan::orig::Light::disable() -> void { joan::bind::member<&::en3d_helpers7::H::light_disable>(this); }  // 0x5c6250
auto joan::orig::Light::bone() -> Bone* { return joan::bind::conv<Bone*>(joan::bind::member<&::game_b104_10::H::get_4>(this)); }  // 0x5c5be0
