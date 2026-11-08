// SPDX-License-Identifier: GPL-3.0-or-later
// enparticle/peffect.cpp: functions of the original d:\ww\enparticle\peffect.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enparticle/PEffect.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
// for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../runtime/hitch.hpp"

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

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::obj;

using joan::hook::global;

using joan::call::cdecl_call;
using joan::img::str;

struct V3 {
    float x, y, z;
};

using joan::orig::helpers::game_new;  // operator new


}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

struct alignas(4) Angle {  // 0x00576120 (byte at +0)
    std::uint32_t b;
};

float table_cos(Angle& a) { return joan::orig::arg::as<float>(joan::orig::as_angle(&a)->cos()); }  // 0x00576ac0
float table_sin(Angle& a) { return joan::orig::arg::as<float>(joan::orig::as_angle(&a)->sin()); }  // 0x005768d0

JOAN_COUNTER(c_modelobj_init, "ModelObj::init");

struct H {
    // ---- 0x0041a740 ModelObj::init (8 args) ---------------------------------------------------
    void modelobj_init(joan::orig::Scene* a8, void* ac, const float* pos, const float* origin, char show,
                       std::uint32_t angle, std::int32_t model, std::int32_t model2) {
        c_modelobj_init.hit();
        jo::self<jo::ModelObj>(this)->start_time = global<std::int32_t>(d::g_now) + model;
        if (model2 != 0)
            jo::self<jo::ModelObj>(this)->end_time = global<std::int32_t>(d::g_now) + model2;
        else
            jo::self<jo::ModelObj>(this)->end_time = 0;
        jo::self<jo::ModelObj>(this)->u32_27 = 0;
        std::memcpy(jo::self<jo::ModelObj>(this)->pos, pos, 12);
        joan::orig::as_actor(this)->attach_model(ac, joan::orig::arg::mptr(pos), a8);  // 0x0041a930
        Angle ang{0};
        joan::orig::as_angle(&ang)->construct();  // 0x00576120
        jo::self<jo::ModelObj>(this)->angle = angle;
        joan::orig::as_angle(&ang)->set(joan::orig::arg::f32(angle));  // 0x00576240
        const float c1 = table_cos(ang);
        const float t18 = c1 * pos[0] + origin[0];
        const float s1 = table_sin(ang);
        const float s1y = s1 * pos[1];
        V3 v;
        v.x = t18 - s1y;
        const float c2 = table_cos(ang);
        const float t1c = c2 * pos[1] + origin[1];
        const float s2 = table_sin(ang);
        const float s2x = s2 * pos[0];
        v.y = s2x + t1c;
        v.z = origin[2] + pos[2];
        jo::view<char>(jo::self<jo::ModelObj>(this)->fx.visible) = show;
        if (jo::view<std::int8_t>(jo::self<jo::ModelObj>(this)->fx.visible) != 0) {
            void* const a1 = joan::orig::arg::as<void*>(joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->object());  // 0x0041b280
            joan::orig::as_scene_object(a1)->set_yaw(joan::orig::arg::u32(jo::self<jo::ModelObj>(this)->angle));               // 0x005ac6a0
            void* const a2 = joan::orig::arg::as<void*>(joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->object());
            joan::orig::as_actor(a2)->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(&v)));  // 0x005ac310
            jo::self<jo::ModelObj>(this)->fx.yaw = jo::self<jo::ModelObj>(this)->angle;
        }
        joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->set_position(v);  // 0x0041b2a0
        std::memcpy(jo::self<jo::ModelObj>(this)->fx.prev_pos, &v, 12);
    }

};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("ParticleEffect::reinit", 0x0041a740, &H::modelobj_init);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    std::uint32_t clear_41a260() {  // 0x0041a260
        joan::orig::as_effect(this)->set_owner(joan::orig::arg::mptr(0u), joan::orig::arg::mptr(0u));                            // 0x0041b0d0
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_actor(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->release_675b30(0u));  // 0x00675b30
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("ParticleEffect::deinit", 0x0041a260, &H::clear_41a260);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b90_02_2.cpp (game/b90_02)
namespace {
namespace game_b90_02_2 {

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_launch, "Projectile::launch");

struct H {
    // 0x0041a2a0
    void launch(std::int16_t kind, joan::orig::Scene* a2, std::int32_t i, std::int32_t unit_id) {
        c_launch.hit();
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16(kind)));  // 0x00416f20
        jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->start_time) = global<std::uint32_t>(d::g_now) + jo::view<std::uint32_t>(jo::as_model_slot(e)->emitter_a[i]);
        jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->end_time) = global<std::uint32_t>(d::g_now) + jo::view<std::uint32_t>(jo::as_model_slot(e)->emitter_b[i]);
        jo::self<jo::ModelObj>(this)->u32_27 = 0;
        void* const unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(unit_id));  // 0x0041b3c0
        Vec3 v;
        std::memcpy(&v, &jo::as_model_slot(e)->emitter_pos[i * 3], sizeof v);
        std::memcpy(jo::self<jo::ModelObj>(this)->pos, &v, sizeof v);
        alignas(4) std::uint8_t name[0x194];
        joan::orig::as_string(name)->construct();             // 0x005a1170 ctor
        joan::orig::as_string(name)->assign(joan::orig::arg::str(&jo::as_model_slot(e)->emitter_names[i * 0x21]));  // 0x005a1440 assign
        std::uint32_t id = 0;
        if (jo::as_model_slot(e)->emitter_default != 0) {
            Vec3 pos;
            joan::orig::as_unit(unit)->position(&pos);           // 0x004fedb0
            joan::orig::as_string(name)->append(str(d::s_us_9cf774));                // 0x005a1550 "_"
            const char* t = joan::orig::arg::as<const char*>(joan::orig::as_world_cell(jo::as_unit(unit)->cell)->terrain_at(pos));  // 0x00560ee0
            if (joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(t), str(d::s_mud))) != nullptr) {  // strstr "mud"
                joan::orig::as_string(name)->append(str(d::s_mud));
            } else {
                t = joan::orig::arg::as<const char*>(joan::orig::as_world_cell(jo::as_unit(unit)->cell)->terrain_at(pos));  // 0x00560ee0
                if (joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(t), str(d::s_snow))) != nullptr)  // strstr "snow"
                    joan::orig::as_string(name)->append(str(d::s_snow));
                else
                    joan::orig::as_string(name)->append(str(d::s_other));                // "other"
            }
            joan::orig::as_string(name)->append(str(d::s_txt));                // ".txt"
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(name)->c_str());  // 0x00417d90
            id = joan::orig::arg::as<std::uint32_t>(joan::orig::as_name_list(obj(d::g_pemi_res))->find_4c2060(joan::orig::arg::str(s), static_cast<std::int8_t>(joan::orig::arg::i32(1))));  // 0x004c2060
        } else {
            id = jo::view<std::uint32_t>(jo::as_model_slot(e)->emitter_txt_ids[i]);
        }
        void* const res = joan::orig::arg::as<void*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(id)));  // 0x0041b440
        joan::orig::as_actor(this)->attach_model(joan::orig::arg::mptr(jo::as_pemi_script(res)->effect), &v, a2);  // 0x0041a930
        std::uint32_t angle = 0;  // 1-byte object; ctor writes nothing
        joan::orig::as_angle(&angle)->construct();  // 0x00576120
        const float heading = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading());  // 0x004ff850
        joan::orig::as_angle(&angle)->set(joan::orig::arg::f32(heading));  // 0x00576240
        jo::view<float>(jo::self<jo::ModelObj>(this)->angle) = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading());  // 0x004ff850
        joan::orig::as_unit(unit)->position(&v);  // 0x004fedb0
        jo::as_unit(unit)->last_spawn_time = global<std::uint32_t>(d::g_now);
        const float* const ofs = &jo::as_model_slot(e)->emitter_pos[i * 3];
        const float t1 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos()) * ofs[0];  // 0x00576ac0
        v.x = (t1 - joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin()) * ofs[1]) + v.x;  // 0x005768d0
        const float t2 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos()) * ofs[1];  // 0x00576ac0
        v.y = (joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin()) * ofs[0] + t2) + v.y;  // 0x005768d0
        v.z = v.z + ofs[2];
        jo::self<jo::ModelObj>(this)->fx.visible = jo::view<std::uint8_t>(jo::as_model_slot(e)->emitter_bytes[i]);
        if (static_cast<std::int8_t>(jo::self<jo::ModelObj>(this)->fx.visible) != 0) {
            const std::uint32_t h = jo::self<jo::ModelObj>(this)->angle;  // float pushed as a raw dword
            void* const o1 = joan::orig::arg::as<void*>(joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->object());  // 0x0041b280
            joan::orig::as_scene_object(o1)->set_yaw(joan::orig::arg::u32(h));  // 0x005ac6a0 (float arg)
            void* const o2 = joan::orig::arg::as<void*>(joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->object());  // 0x0041b280
            joan::orig::as_actor(o2)->set_position_5ac310(&v);  // 0x005ac310
            jo::self<jo::ModelObj>(this)->fx.yaw = jo::self<jo::ModelObj>(this)->angle;
        }
        joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->set_position(v);  // 0x0041b2a0
        std::memcpy(jo::self<jo::ModelObj>(this)->fx.prev_pos, &v, sizeof v);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("ParticleEffect::init", 0x0041a2a0, &H::launch);
#undef B

}  // namespace game_b90_02_2
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

struct Vec4 {
    float x, y, z, w;
};

JOAN_COUNTER(c_attach, "Actor::attach_model");

struct H {

    // ---- 0x0041a930 Actor::attach_model ----------------------------------------------------
    void* attach_model(void* model, void*, void* scene) {
        c_attach.hit();
        void* const anim = reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx);
        joan::orig::as_effect_state(anim)->init();         // 0x006759a0
        joan::orig::as_handle(anim)->assign(joan::orig::arg::mptr(model));  // 0x00679300
        joan::orig::as_effect_state(anim)->set_name(joan::orig::arg::str(joan::orig::arg::as<std::uint32_t>(joan::orig::as_effect_state(model)->name())));  // 0x0067a1c0(0x0041b3a0)
        joan::orig::as_effect_state(anim)->set_source_blend(joan::orig::arg::i16(joan::orig::as_effect_state(model)->source_blend()));  // 0x0041b320(0x0041b300)
        joan::orig::as_effect_state(anim)->set_dest_blend(joan::orig::arg::i16(joan::orig::as_effect_state(model)->dest_blend()));  // 0x0041b370(0x0041b350)
        void* const body_mem = game_new(static_cast<std::uint32_t>(sizeof(jo::ActorDef)));  // 0x2d8 on x86
        void* const body = body_mem != nullptr ? joan::orig::as_actor_def(body_mem)->construct() : nullptr;  // 0x005af390
        Vec4 sphere;  // ebp-0x24
        std::memcpy(&sphere.x, &global<std::uint32_t>(d::g_pos), 4);
        std::memcpy(&sphere.y, &global<std::uint32_t>(d::g_a69cdc), 4);
        std::memcpy(&sphere.z, &global<std::uint32_t>(d::g_a69ce0), 4);
        const std::uint32_t k200 = 0x43480000u;  // 200.0f
        std::memcpy(&sphere.w, &k200, 4);
        joan::orig::as_actor_def(body)->create(joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->model(), static_cast<void*>(&sphere));  // 0x005b02f0(0x0041b230)
        void* const actor_mem = game_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86
        std::uint8_t* const actor = actor_mem != nullptr ? joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene_object(actor_mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        jo::view<void*>(jo::as_scene_object(actor)->def) = body;
        joan::orig::as_scene_object(actor)->init();                                          // 0x005aab70
        joan::orig::as_scene(scene)->attach(joan::orig::arg::mptr(static_cast<void*>(actor)));               // 0x005efc50
        joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->set_model(jo::as_scene_object(actor)->plane);  // 0x0041b200
        joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->set_actor(reinterpret_cast<jo::Actor*>(actor));       // 0x0041b250
        jo::view<std::uint32_t>(jo::as_scene_object(actor)->holder_kind) = 2;
        jo::view<void*>(jo::as_scene_object(actor)->holder) = reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx);
        jo::view<std::uint32_t>(jo::as_scene_object(actor)->kind) = 2;
        return actor;
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("ParticleEffect::load", 0x0041a930, &H::attach_model);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Effect::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_18::H::clear_41a260>(this)); }  // 0x41a260
auto joan::orig::Effect::launch(std::int16_t a0, Scene* a1, std::int32_t a2, std::int32_t a3) -> void { joan::bind::member<&::game_b90_02_2::H::launch>(this, a0, a1, a2, a3); }  // 0x41a2a0
auto joan::orig::Effect::init(Scene* a0, void* a1, void* a2, void* a3, char a4, std::uint32_t a5, std::int32_t a6, std::int32_t a7) -> void { joan::bind::member<&::game_b84_05::H::modelobj_init>(this, a0, a1, a2, a3, a4, a5, a6, a7); }  // 0x41a740
auto joan::orig::Actor::attach_model(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::attach_model>(this, a0, a1, a2)); }  // 0x41a930
