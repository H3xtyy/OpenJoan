// SPDX-License-Identifier: GPL-3.0-or-later
// enparticle/particle.cpp: functions of the original d:\ww\enparticle\particle.cpp (re/attribution.csv). The file name
// is the Restaurant Empire 2 engine's (enparticle/Particle.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__
// string for it.

#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdarg>

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

using joan::img::obj;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_fx5.cpp (game/fx6)
namespace {
namespace game_fx5 {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::call::import_call;

struct V3 {
    float x, y, z;
};

using joan::hook::x87::ftol;

V3 load3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

// x87 comparisons (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool below_or_nan(double a, double b) { return !(a >= b); }

constexpr img::Addr kRandom(d::g_misc);
constexpr fn::Code kRotZ = fn::init_rotation_z_60bc50;        // 0x0060bc50 matrix_rotation(out, angle)
constexpr fn::Code kNodeAngleA = fn::Actor_set_h_dir;  // 0x005ac6a0
constexpr fn::Code kNodeMove = fn::Actor_set_pos_5ac310;    // 0x005ac310

JOAN_COUNTER(c_particle, "Particle::update");

struct H {
    // 0x00662f60 Particle::update(dt, pos, vel, follow kind), ret 0x20: age (+8 of +4 life,
    // +0xc start delay, +0x10), move (offset +0x48 by velocity +0x60, emitter motion spread
    // by a random factor when following, world position +0x54), size +0x1c, colour bytes
    // +0x24..+0x27 by rates +0x28..+0x34, acceleration +0x6c; pushes it to the scene node
    // (+0x18). Returns 0 when the particle has expired.
    std::uint8_t particle_update(float dt, V3 pos, V3 vel, char follow) {
        c_particle.hit();
        const joan::hook::x87::Fpu fpu{true};
        if (jo::self<jo::Particle>(this)->delay > 0.0f) {
            dt = static_cast<float>(fpu.add(dt, jo::self<jo::Particle>(this)->delay));
            jo::view<std::uint32_t>(jo::self<jo::Particle>(this)->delay) = 0;
        }
        jo::self<jo::Particle>(this)->age = static_cast<float>(fpu.add(dt, jo::self<jo::Particle>(this)->age));
        jo::self<jo::Particle>(this)->time = static_cast<float>(fpu.add(dt, jo::self<jo::Particle>(this)->time));
        if (!below_or_nan(jo::self<jo::Particle>(this)->age, jo::self<jo::Particle>(this)->lifetime)) return 0;
        jo::self<jo::Particle>(this)->offset[0] = static_cast<float>(fpu.add(fpu.mul(dt, jo::self<jo::Particle>(this)->vel[0]), jo::self<jo::Particle>(this)->offset[0]));
        jo::self<jo::Particle>(this)->offset[1] = static_cast<float>(fpu.add(fpu.mul(dt, jo::self<jo::Particle>(this)->vel[1]), jo::self<jo::Particle>(this)->offset[1]));
        jo::self<jo::Particle>(this)->offset[2] = static_cast<float>(fpu.add(fpu.mul(dt, jo::self<jo::Particle>(this)->vel[2]), jo::self<jo::Particle>(this)->offset[2]));
        const std::int32_t kind = static_cast<std::int8_t>(follow);
        if (kind != 0) {
            const std::int32_t n = joan::hook::x87::ftol(
                fpu.add(fpu.add(fpu.mul(vel.x, vel.x), fpu.mul(vel.y, vel.y)), fpu.mul(vel.z, vel.z)));
            const std::int32_t root = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(img::obj(kRandom))->sqrt_int_590340(joan::orig::arg::i32(n)));  // 0x00590340 isqrt
            const float rootf = static_cast<float>(root);
            float spread = joan::orig::arg::as<float>(joan::orig::as_misc(img::obj(kRandom))->limit_double(joan::orig::arg::f64(fpu.mul(rootf, global<float>(d::flt_10))), joan::orig::arg::f64(1.0), joan::orig::arg::f64(40.0)));  // 0x00590b00 clamp
            if (kind == 3) spread = 1.0f;
            jo::self<jo::Particle>(this)->offset[0] = static_cast<float>(fpu.add(fpu.div(vel.x, spread), jo::self<jo::Particle>(this)->offset[0]));
            jo::self<jo::Particle>(this)->offset[1] = static_cast<float>(fpu.add(fpu.div(vel.y, spread), jo::self<jo::Particle>(this)->offset[1]));
            jo::self<jo::Particle>(this)->offset[2] = static_cast<float>(fpu.add(fpu.div(vel.z, spread), jo::self<jo::Particle>(this)->offset[2]));
            if (jo::view<void*>(jo::self<jo::Particle>(this)->node) != nullptr) {
                void* const node = jo::view<void*>(jo::self<jo::Particle>(this)->node);
                V3 o = load3(jo::self<jo::Particle>(this)->offset);
                const float angle = joan::orig::arg::as<float>(joan::orig::as_scene_object(node)->yaw());  // 0x005ac7e0
                float rot[16];
                cdecl_call<void*>(kRotZ, static_cast<void*>(rot), angle);
                import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&o), static_cast<void*>(rot));  // point_transform
                jo::self<jo::Particle>(this)->pos[0] = pos.x + o.x;
                jo::self<jo::Particle>(this)->pos[1] = pos.y + o.y;
                jo::self<jo::Particle>(this)->pos[2] = pos.z + o.z;
                if (kind == 3 || kind == 4) jo::self<jo::Particle>(this)->pos[2] = pos.z + o.z;
            } else {
                jo::self<jo::Particle>(this)->pos[0] = pos.x + jo::self<jo::Particle>(this)->offset[0];
                jo::self<jo::Particle>(this)->pos[1] = pos.y + jo::self<jo::Particle>(this)->offset[1];
                jo::self<jo::Particle>(this)->pos[2] = pos.z + jo::self<jo::Particle>(this)->offset[2];
                if (kind == 3 || kind == 4) jo::self<jo::Particle>(this)->pos[2] = pos.z + jo::self<jo::Particle>(this)->offset[2];
            }
        } else {
            V3 tmp;
            V3 step = load3(joan::orig::arg::as<void*>(joan::orig::vec_scale(static_cast<void*>(&tmp), static_cast<void*>(jo::self<jo::Particle>(this)->vel), joan::orig::arg::f32(dt))));  // 0x0060cd00 scale
            joan::orig::vec_add_in_place(static_cast<void*>(jo::self<jo::Particle>(this)->pos), static_cast<void*>(&step));  // 0x0060cdc0 +=
        }
        jo::self<jo::Particle>(this)->scale = static_cast<float>(fpu.add(fpu.mul(dt, jo::self<jo::Particle>(this)->scale_rate), jo::self<jo::Particle>(this)->scale));
        float c[4];
        for (std::int32_t k = 0; k < 4; ++k)
            c[k] = static_cast<float>(fpu.add(static_cast<double>(jo::self<jo::Particle>(this)->colour[k]), fpu.mul(dt, jo::self<jo::Particle>(this)->colour_rate[k])));
        for (std::int32_t k = 0; k < 4; ++k) {
            std::uint32_t v;
            if (!below_or_nan(c[k], global<float>(d::flt_255)))  // 255.0f
                v = 0xff;
            else if (c[k] > 0.0f)
                v = static_cast<std::uint32_t>(joan::hook::x87::ftol(c[k])) & 0xff;
            else
                v = 0;
            jo::self<jo::Particle>(this)->colour[k] = static_cast<std::uint8_t>(v);
        }
        jo::self<jo::Particle>(this)->vel[0] = static_cast<float>(fpu.add(fpu.mul(dt, jo::self<jo::Particle>(this)->accel[0]), jo::self<jo::Particle>(this)->vel[0]));
        jo::self<jo::Particle>(this)->vel[1] = static_cast<float>(fpu.add(fpu.mul(dt, jo::self<jo::Particle>(this)->accel[1]), jo::self<jo::Particle>(this)->vel[1]));
        jo::self<jo::Particle>(this)->vel[2] = static_cast<float>(fpu.add(fpu.mul(dt, jo::self<jo::Particle>(this)->accel[2]), jo::self<jo::Particle>(this)->vel[2]));
        if (jo::view<void*>(jo::self<jo::Particle>(this)->node) != nullptr) {
            void* const node = jo::view<void*>(jo::self<jo::Particle>(this)->node);
            thiscall(kNodeMove, node, static_cast<void*>(jo::self<jo::Particle>(this)->pos));
            jo::view<std::uint32_t>(jo::as_actor(node)->scale) = jo::view<std::uint32_t>(jo::self<jo::Particle>(this)->scale);
            thiscall(kNodeAngleA, node, joan::orig::arg::bits(joan::orig::arg::as<float>(joan::orig::as_scene_object(node)->yaw())));  // 0x005ac7e0
            void* const plane = jo::as_actor(node)->plane;
            std::uint32_t packed = 0;
            joan::orig::as_colour(static_cast<void*>(&packed))->from_rgba(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Particle>(this)->colour)));  // 0x005e95b0
            joan::orig::as_plane(plane)->set_fade(joan::orig::arg::u32(packed));  // 0x005d5e80 Plane::set_fade
        }
        return 1;
    }

    // 0x00663800 SlotPool::alloc: index of a new slot, marked used.
    std::int32_t pool_alloc() {
        joan::orig::dynarray_b(this)->linkin(nullptr);                              // 0x005857a0
        const std::int32_t index = joan::orig::as_dynarray(this)->cursor();  // 0x00417520
        std::uint32_t* const used = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_pool(this)->last_used());  // 0x004b9c30
        *used = 1;
        return index;
    }
};

#define FX6(name, addr, fn) JOAN_REPLACE("game/fx6", name, addr, fn)
FX6("Particle::update", 0x00662f60, &H::particle_update);
FX6("ParticleArray::add_particle", 0x00663800, &H::pool_alloc);
#undef FX6

}  // namespace game_fx5
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

using joan::call::thiscall;

// Thunks.
constexpr fn::Code kListCount = fn::List_count_417ad0;   // 0x00417ad0

std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }

struct H {
    std::int32_t pool_count() { return list_count(this); }         // 0x006638b0
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("ParticleArray::size", 0x006638b0, &H::pool_count);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx_2 {

struct H {
    void pool_release(std::int32_t i) {                            // 0x00663850
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_particle_pool(this)->slot(joan::orig::arg::i32(i)));      // 0x00663960
        joan::orig::as_particle(p)->clear_model();                                   // 0x00662ed0
        *static_cast<std::uint32_t*>(p) = 0;
        joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));    // 0x005858c0
    }
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("ParticleArray::remove_particle_from_sector", 0x00663850, &H::pool_release);
#undef FX1

}  // namespace game_fx_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ParticlePool::alloc() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx5::H::pool_alloc>(this)); }  // 0x663800
auto joan::orig::ParticlePool::count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx::H::pool_count>(this)); }  // 0x6638b0
auto joan::orig::ParticlePool::release(std::int32_t a0) -> void { joan::bind::member<&::game_fx_2::H::pool_release>(this, a0); }  // 0x663850
