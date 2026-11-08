// SPDX-License-Identifier: GPL-3.0-or-later
// enparticle/pemitter.cpp: functions of the original d:\ww\enparticle\pemitter.cpp (re/attribution.csv). The file name
// is the Restaurant Empire 2 engine's (enparticle/PEmitter.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__
// string for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/cxx_eh.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::img::obj;


using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::hook::global;

struct V3 {
    float x, y, z;
};

constexpr fn::Code kListCount = fn::List_count_417ad0;   // 0x00417ad0
constexpr fn::Code kListGet = fn::List_get_41b3f0;     // 0x0041b3f0

std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }

using joan::img::str;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {

JOAN_COUNTER(c_particle_init, "ParticleHost::init");

using joan::orig::helpers::game_new;  // operator new

struct H {
    // ------------------------------------------------------------------ 0x006759a0
    void particle_init() {
        c_particle_init.hit();
        if (jo::self<jo::EffectState>(this)->initialised != 0) return;
        void* p = game_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
        void* sys = p != nullptr ? joan::orig::as_plane(p)->construct(4u, 1u) : nullptr;  // 0x005cbaf0
        jo::view<void*>(jo::self<jo::EffectState>(this)->parts) = sys;
        joan::orig::as_plane(jo::view<void*>(jo::self<jo::EffectState>(this)->parts))->init(0, 0, 0, 1u);  // 0x005cc7a0
        jo::self<jo::EffectState>(this)->parts->list_kind = 6;
        if (jo::view<void*>(jo::self<jo::EffectState>(this)->parts) == nullptr) joan::orig::as_error_log(obj(d::g_err))->mem_error();  // 0x00586af0
        void* a = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
        void* arr = a != nullptr ? joan::orig::dynarray(a)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x1e, 0) : nullptr;  // 0x005843d0 (emitter pointers: 4 on x86)
        jo::view<void*>(jo::self<jo::EffectState>(this)->emitters) = arr;
        jo::view<std::int32_t>(jo::self<jo::EffectState>(this)->owns_emitters) = 1;
        jo::view<std::int32_t>(jo::self<jo::EffectState>(this)->share_refs) = 0;
        jo::self<jo::EffectState>(this)->shared_from = nullptr;
        jo::self<jo::EffectState>(this)->pools = nullptr;
        jo::self<jo::EffectState>(this)->visible = 1;
        jo::self<jo::EffectState>(this)->initialised = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("ParticleEmitter::init", 0x006759a0, &H::particle_init);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14 {

JOAN_COUNTER(c_cont_clear, "Container::clear");

struct H {
    // 0x00677e00 Container::clear() (ret 0): `this` is an EffectState (the "Container" view's +0x70 list is
    // EffectState::emitters; typed as EffectState so it moves with the x64 layout).
    std::uint32_t cont_clear() {
        c_cont_clear.hit();
        for (std::int32_t i = joan::orig::as_dynarray(jo::self<jo::EffectState>(this)->emitters)->size(); i > 0; --i) {  // 0x00417ad0
            void* const e = joan::orig::as_dynarray(jo::self<jo::EffectState>(this)->emitters)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            joan::orig::as_effect_key(e)->release();                                       // 0x0067f250
            if (e != nullptr) joan::orig::as_effect_key(e)->destroy(1u);           // 0x00677eb0
        }
        joan::orig::as_dynarray(jo::self<jo::EffectState>(this)->emitters)->zap_584580(1);  // 0x00584580
        return 0;  // void callee; callers discard the result
    }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("ParticleEmitter::deinit_event_sequences", 0x00677e00, &H::cont_clear);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_leaves82a.cpp (game/leaves82a)
namespace {
namespace game_leaves82a {

JOAN_COUNTER(c_actor_release, "Actor::release");

struct H {
    // 0x00675b30 Actor::release(keep_model): +0 active byte, +0x74 (delete 3), +0x70 attached
    // object (+0x64 owned: detach 0x00677e00 and delete; else drop the reference +0x6c->+0x68),
    // +0x28 node (unlinked from +0x18, 0x005aaf10, deleted; its +0x90 model is spared), +0x24
    // model (freed unless keep_model or it is the node's model).
    std::uintptr_t actor_release(std::uint32_t keep_model) {  // EAX = this (pointer-wide on x64)
        c_actor_release.hit();
        if (jo::self<jo::EffectState>(this)->initialised == 0) return reinterpret_cast<std::uintptr_t>(this);  // EAX = this
        if (jo::view<void*>(jo::self<jo::EffectState>(this)->pools) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::EffectState>(this)->pools);
            if (p != nullptr) joan::orig::as_zone(p)->vector_deleting_destructor(3u);  // 0x00675d70
            jo::view<void*>(jo::self<jo::EffectState>(this)->pools) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::EffectState>(this)->emitters) != nullptr) {
            if (jo::self<jo::EffectState>(this)->owns_emitters != 0) {
                joan::orig::as_effect_state(this)->clear_sequences();  // 0x00677e00
                void* const p = jo::view<void*>(jo::self<jo::EffectState>(this)->emitters);
                if (p != nullptr) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0
                jo::view<void*>(jo::self<jo::EffectState>(this)->emitters) = nullptr;
            } else {
                jo::view<std::int32_t>(jo::self<jo::EffectState>(this)->shared_from->share_refs) = jo::view<std::int32_t>(jo::self<jo::EffectState>(this)->shared_from->share_refs) - 1;
                jo::view<void*>(jo::self<jo::EffectState>(this)->emitters) = nullptr;
                jo::view<void*>(jo::self<jo::EffectState>(this)->shared_from) = nullptr;
                jo::self<jo::EffectState>(this)->owns_emitters = 1;
            }
        }
        void* node_model = nullptr;
        if (jo::view<void*>(jo::self<jo::EffectState>(this)->object_) != nullptr) {
            node_model = jo::view<void*>(jo::self<jo::EffectState>(this)->object_->plane);
            if (jo::view<void*>(jo::self<jo::EffectState>(this)->object_->scene) != nullptr)
                joan::orig::as_scene(jo::view<void*>(jo::self<jo::EffectState>(this)->object_->scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::EffectState>(this)->object_)));  // 0x005efdb0
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::EffectState>(this)->object_))->release_5aaf10();                                         // 0x005aaf10
            void* const p = jo::view<void*>(jo::self<jo::EffectState>(this)->object_);
            if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840
        }
        jo::view<void*>(jo::self<jo::EffectState>(this)->object_) = nullptr;
        if ((keep_model & 0xff) == 0 && jo::view<void*>(jo::self<jo::EffectState>(this)->parts) != nullptr && jo::view<void*>(jo::self<jo::EffectState>(this)->parts) != node_model) {
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::EffectState>(this)->parts))->release();  // 0x005ccaa0
            void* const p = jo::view<void*>(jo::self<jo::EffectState>(this)->parts);
            if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);  // 0x00459c40
        }
        jo::view<void*>(jo::self<jo::EffectState>(this)->parts) = nullptr;
        jo::self<jo::EffectState>(this)->initialised = 0;
        return reinterpret_cast<std::uintptr_t>(this);  // EAX = this
    }
};

#define LEAVES82A(name, addr, fn) JOAN_REPLACE("game/leaves82a", name, addr, fn)
LEAVES82A("ParticleEmitter::deinit", 0x00675b30, &H::actor_release);
#undef LEAVES82A

}  // namespace game_leaves82a
}  // namespace

// ---------------------------------------- from modules/game_fx5.cpp (game/fx5)
namespace {
namespace game_fx5 {

using joan::call::import_call;

V3 load3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}
float bits_f(std::uint32_t u) {
    float f;
    std::memcpy(&f, &u, sizeof f);
    return f;
}
double bits_d(std::uint64_t u) {
    double d;
    std::memcpy(&d, &u, sizeof d);
    return d;
}

// x87 comparisons (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool below_or_nan(double a, double b) { return !(a >= b); }
bool equal_or_nan(double a, double b) { return !(a < b || a > b); }

// Constants (.rdata).
float k360() { return global<float>(d::flt_360); }       // 360.0f
float kDeg() { return global<float>(d::flt_0_017453292); }       // pi / 180 (float)
float kPiF() { return global<float>(d::flt_3_1415927); }       // pi (float)
double kHalfPi() { return global<double>(d::dbl_1_5707963267948966); }  // pi / 2
double kTwoPi() { return global<double>(d::dbl_6_283185307179586); }   // 2 pi

constexpr img::Addr kIdentity(d::g_identity_matrix);
constexpr fn::Code kPoolCount = fn::ParticleArray_size;   // 0x006638b0
constexpr fn::Code kPoolFree = fn::SlotPool_free_at;    // 0x00663910
constexpr fn::Code kPoolSlot = fn::SlotPool_slot;    // 0x00663960
constexpr fn::Code kRotAxis = fn::init_rotation_axis;     // 0x0060bca0 rotation_axis(out, axis, angle)
constexpr fn::Code kRotZ = fn::init_rotation_z_60bc50;        // 0x0060bc50 matrix_rotation(out, angle)
constexpr fn::Code kRow1 = fn::matrix_get_down;        // 0x0060c1d0
constexpr fn::Code kAxisX = fn::matrix_get_right;       // 0x0060c180
constexpr fn::Code kNodeAngleA = fn::Actor_set_h_dir;  // 0x005ac6a0
constexpr fn::Code kNodeAngleB = fn::Actor_set_r_dir;  // 0x005ac980
constexpr fn::Code kNodeAngleC = fn::Actor_set_v_dir;  // 0x005ac810
constexpr fn::Code kNodeAngles = fn::Actor_set_hvr_dir;  // 0x005acaf0

std::uint8_t* list_get(const void* list, std::int32_t i) { return thiscall<std::uint8_t*>(kListGet, list, i); }

float length(const void* v) { return static_cast<float>(import_call<double>(d::iat_v3d_87_length, v)); }
float dot(const void* a, const void* b) { return static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, a, b)); }
void* matrix_multiply(void* a, const void* b) { return import_call<void*>(d::iat_v3d_87_matrix_multiply_2, a, b); }

// acos(dot(u, v) / |u| / |v|) as stored to a double local (dot and the first quotient are
// stored to floats).
double acos_between(V3 u, V3 v) {
    const joan::hook::x87::Fpu fpu{true};
    const float d = dot(&u, &v);
    const float t = static_cast<float>(fpu.div(d, length(&u)));
    const double q = fpu.div(t, length(&v));
    return joan::orig::arg::as<double>(joan::orig::libc::acos(joan::orig::arg::f64(q)));  // acos
}

JOAN_COUNTER(c_parts, "EffectState::update_parts");

struct H {
    // 0x00675e90 EffectState::update_parts(dt, ctx), ret 8.
    void update_parts(float dt, void* ctx) {
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_effect_state(this)->started()) == 0) return;  // 0x0041b1e0 started
        c_parts.hit();
        const joan::hook::x87::Fpu fpu{true};
        joan::orig::as_plane(jo::view<void*>(jo::self<jo::EffectState>(this)->parts))->clear();  // 0x005cd2d0 Plane::clear
        joan::orig::as_effect_state(this)->parts_alloc();            // 0x0067a240 parts_alloc
        for (std::int32_t i = list_count(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters)); i > 0; --i) {
            std::uint8_t* const part = list_get(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters), i);
            std::uint8_t* const slot = reinterpret_cast<std::uint8_t*>(&jo::self<jo::EffectState>(this)->pools[i - 1]);
            if (static_cast<std::int8_t>(jo::self<jo::EffectState>(this)->visible) != 0 && static_cast<std::int8_t>(jo::as_effect_key(part)->is_smo) == 0) {
                V3 tmp;
                const V3 d = load3(joan::orig::arg::as<void*>(joan::orig::vec_sub(static_cast<void*>(&tmp), static_cast<void*>(jo::self<jo::EffectState>(this)->prev_pos), static_cast<void*>(jo::self<jo::EffectState>(this)->pos))));  // 0x0060cc90 sub
                void* const scene = jo::self<jo::EffectState>(this)->object_->scene;
                const std::int16_t w62 = jo::self<jo::EffectState>(this)->i16_62;
                const std::int16_t w60 = jo::self<jo::EffectState>(this)->i16_60;
                const char follow = static_cast<char>(jo::self<jo::EffectState>(this)->visible);
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::self<jo::EffectState>(this)->object_->bone)->matrix());
                const V3 zero = load3(img::obj(d::g_pos));
                joan::orig::as_effect_key(part)->update(joan::orig::arg::f32(dt), zero, d, joan::orig::arg::mptr(m), static_cast<char>(joan::orig::arg::i32(follow)), joan::orig::arg::i16(w60), joan::orig::arg::i16(w62), joan::orig::arg::mptr(scene), joan::orig::arg::mptr(static_cast<void*>(slot)));  // 0x0067f380
            } else {
                void* const scene = jo::self<jo::EffectState>(this)->object_->scene;
                const std::int16_t w62 = jo::self<jo::EffectState>(this)->i16_62;
                const std::int16_t w60 = jo::self<jo::EffectState>(this)->i16_60;
                const char follow = static_cast<char>(jo::self<jo::EffectState>(this)->visible);
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::self<jo::EffectState>(this)->object_->bone)->matrix());
                const V3 zero = load3(img::obj(d::g_pos));
                const V3 pos = load3(jo::self<jo::EffectState>(this)->pos);
                joan::orig::as_effect_key(part)->update(joan::orig::arg::f32(dt), pos, zero, joan::orig::arg::mptr(m), static_cast<char>(joan::orig::arg::i32(follow)), joan::orig::arg::i16(w60), joan::orig::arg::i16(w62), joan::orig::arg::mptr(scene), joan::orig::arg::mptr(static_cast<void*>(slot)));  // 0x0067f380
            }
            if (ctx == nullptr) continue;
            if (static_cast<std::int8_t>(jo::as_effect_key(part)->is_smo) == 0) {
                const std::int16_t w62 = jo::self<jo::EffectState>(this)->i16_62;
                const std::int16_t w60 = jo::self<jo::EffectState>(this)->i16_60;
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::self<jo::EffectState>(this)->object_->bone)->matrix());
                joan::orig::as_effect_key(part)->draw(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::EffectState>(this)->parts)), joan::orig::arg::mptr(ctx), joan::orig::arg::u32(jo::self<jo::EffectState>(this)->yaw), joan::orig::arg::mptr(m), joan::orig::arg::i16(w60), joan::orig::arg::i16(w62), joan::orig::arg::f32(dt), joan::orig::arg::mptr(static_cast<void*>(slot)));  // 0x0067faf0
                continue;
            }
            float frame[10];
            joan::orig::as_camera(ctx)->frame(joan::orig::arg::mptr(static_cast<void*>(frame)));  // 0x006140a0 Camera::frame
            for (std::int32_t j = thiscall<std::int32_t>(kPoolCount, slot); j > 0; --j) {
                if (thiscall<std::int32_t>(kPoolFree, slot, j) != 0) continue;
                std::uint8_t* const p = thiscall<std::uint8_t*>(kPoolSlot, slot, j);
                void* const node = jo::as_particle(p)->node;
                const std::int32_t kind = static_cast<std::int8_t>(jo::as_effect_key(part)->follow_camera);
                if (kind == 0) {
                    thiscall(kNodeAngleA, node, jo::self<jo::EffectState>(this)->yaw);
                } else if (kind == 1) {
                    thiscall(kNodeAngleA, node, joan::orig::arg::bits(static_cast<float>(fpu.sub(frame[4], kHalfPi()))));
                    thiscall(kNodeAngleB, node, joan::orig::arg::bits(-frame[5]));
                } else if (kind == 2) {
                    thiscall(kNodeAngleA, node, joan::orig::arg::bits(static_cast<float>(fpu.sub(frame[4], kHalfPi()))));
                } else if (kind == 3 || kind == 4) {
                    // Heading of the vector (+0 x, +4 y) and pitch from +8 / length.
                    const float* const v = kind == 3 ? jo::as_particle(p)->vel : jo::as_effect_key(part)->orientation;
                    float a;
                    if (equal_or_nan(v[0], 0.0)) {
                        if (v[1] > 0.0f)
                            a = bits_f(0x3fc90fdbu);
                        else if (below_or_nan(v[1], 0.0))
                            a = bits_f(0x4096cbe4u);
                        else
                            a = 0.0f;
                    } else {
                        a = joan::orig::arg::as<float>(joan::orig::libc::atan(joan::orig::arg::f64(fpu.div(v[1], v[0]))));  // atan
                    }
                    if (below_or_nan(v[0], 0.0)) a = static_cast<float>(fpu.add(a, kPiF()));
                    const double q = fpu.div(v[2], length(v));
                    const float b = static_cast<float>(fpu.sub(joan::orig::arg::as<double>(joan::orig::libc::asin(joan::orig::arg::f64(q))), kHalfPi()));  // asin
                    thiscall(kNodeAngleA, node, joan::orig::arg::bits(a));  // the setters take the angle's dword (x64: an integer register)
                    thiscall(kNodeAngleC, node, joan::orig::arg::bits(-b));
                }
                if (static_cast<std::int8_t>(jo::as_effect_key(part)->self_rotation) == 0) continue;
                jo::as_particle(p)->self_angle = static_cast<float>(
                    fpu.add(fpu.mul(fpu.mul(fpu.mul(dt, jo::as_particle(p)->self_rot_speed), k360()), kDeg()), jo::as_particle(p)->self_angle));
                if (below_or_nan(jo::as_particle(p)->self_angle, 0.0)) jo::as_particle(p)->self_angle = static_cast<float>(fpu.add(jo::as_particle(p)->self_angle, kTwoPi()));
                if (static_cast<double>(jo::as_particle(p)->self_angle) >= kTwoPi())
                    jo::as_particle(p)->self_angle = static_cast<float>(fpu.sub(jo::as_particle(p)->self_angle, kTwoPi()));
                if (static_cast<std::int8_t>(jo::as_effect_key(part)->follow_camera) != 0) {
                    thiscall(kNodeAngleB, node, jo::view<std::uint32_t>(jo::as_particle(p)->self_angle));
                    continue;
                }
                spin_free(part, p, node);
            }
        }
    }

    // The +0x8a spin of an unoriented particle (0x0067641c..0x00676d50): the part direction
    // (+0x6a) as yaw / pitch, combined with the spin about the axis +0x8b, decomposed back
    // into the node's three angles.
    static void spin_free(std::uint8_t* part, std::uint8_t* p, void* node) {
        const joan::hook::x87::Fpu fpu{true};
        float spin[16];
        V3 axis = load3(jo::as_effect_key(part)->self_rot_axis);
        cdecl_call<void*>(kRotAxis, static_cast<void*>(spin), static_cast<void*>(&axis), jo::as_particle(p)->self_angle);
        V3 out_axis{1.0f, 0.0f, 0.0f};
        float out_angle = 0.0f;  // written by 0x0060c3b0 before use
        const V3 dir = load3(jo::as_effect_key(part)->orientation);
        double yaw = 0.0;
        double roll = 0.0;
        if (dir.x > 0.0f)
            yaw = 0.0;
        else if (below_or_nan(dir.x, 0.0))
            yaw = bits_d(0x400921fb54442d18ull);
        if (dir.y > 0.0f)
            yaw = bits_d(0x3ff921fb54442d18ull);
        else if (below_or_nan(dir.y, 0.0))
            yaw = bits_d(0xbff921fb54442d18ull);
        if (dir.z > 0.0f)
            roll = bits_d(0xbff921fb54442d18ull);
        else if (below_or_nan(dir.z, 0.0))
            roll = bits_d(0x3ff921fb54442d18ull);
        if (!equal_or_nan(dir.x, 0.0) && !equal_or_nan(dir.y, 0.0)) {
            const double a = acos_between(V3{1.0f, 0.0f, 0.0f}, V3{dir.x, dir.y, 0.0f});
            yaw = below_or_nan(dir.y, 0.0) ? -a : a;
        }
        if (!equal_or_nan(dir.x, 0.0) && !equal_or_nan(dir.z, 0.0)) {
            const double a = acos_between(V3{dir.x, 0.0f, 0.0f}, V3{dir.x, 0.0f, dir.z});
            roll = dir.z > 0.0f ? -a : a;
        } else if (!equal_or_nan(dir.y, 0.0) && !equal_or_nan(dir.z, 0.0)) {
            const double a = acos_between(V3{0.0f, dir.y, 0.0f}, V3{0.0f, dir.y, dir.z});
            roll = dir.z > 0.0f ? -a : a;
        }
        float m[16];
        float rz[16];
        float ra[16];
        V3 row;
        std::memcpy(m, img::obj(kIdentity), sizeof m);
        cdecl_call<void*>(kRotZ, static_cast<void*>(rz), static_cast<float>(yaw));
        matrix_multiply(m, rz);
        cdecl_call<void*>(kRow1, static_cast<void*>(&row), static_cast<void*>(m));
        cdecl_call<void*>(kRotAxis, static_cast<void*>(ra), static_cast<void*>(&row), static_cast<float>(roll));
        matrix_multiply(m, ra);
        matrix_multiply(m, spin);
        std::memcpy(spin, m, sizeof spin);
        V3 ax;
        cdecl_call<void*>(kAxisX, static_cast<void*>(&ax), static_cast<void*>(spin));
        const float a1 = joan::orig::arg::as<float>(joan::orig::libc::atan2_f(joan::orig::arg::f32(ax.y), joan::orig::arg::f32(ax.x)));  // 0x00421020 atan2
        const float a2 = joan::orig::arg::as<float>(joan::orig::libc::asin_f(joan::orig::arg::f32(-ax.z)));       // 0x00425610 asin
        std::memcpy(m, img::obj(kIdentity), sizeof m);
        float inv[16];
        std::memcpy(inv, spin, sizeof inv);
        import_call<void*>(d::iat_v3d_87_matrix_inverse_1, static_cast<void*>(inv));  // matrix_inverse (in place)
        cdecl_call<void*>(kRotZ, static_cast<void*>(rz), a1);
        matrix_multiply(m, rz);
        cdecl_call<void*>(kRow1, static_cast<void*>(&row), static_cast<void*>(m));
        cdecl_call<void*>(kRotAxis, static_cast<void*>(ra), static_cast<void*>(&row), a2);
        matrix_multiply(m, ra);
        void* const rest = matrix_multiply(inv, m);
        joan::orig::matrix_axis_angle(joan::orig::arg::mptr(rest), joan::orig::arg::mptr(static_cast<void*>(&out_axis)), joan::orig::arg::mptr(static_cast<void*>(&out_angle)));  // 0x0060c3b0
        // Sign of the angle from the largest axis component.
        auto fabs_d = [](float v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(v)))); };  // fabs
        const double ax_abs = fabs_d(out_axis.x);
        double big;
        if (below_or_nan(fabs_d(out_axis.y), ax_abs))
            big = fabs_d(out_axis.x);
        else
            big = fabs_d(out_axis.y);
        float big_f = static_cast<float>(big);
        double big2;
        if (below_or_nan(fabs_d(out_axis.z), big))
            big2 = big_f;
        else
            big2 = fabs_d(out_axis.z);
        big_f = static_cast<float>(big2);
        if (equal_or_nan(fabs_d(out_axis.x), big_f)) {
            if (fpu.mul(out_axis.x, ax.x) > 0.0) out_angle = -out_angle;
        } else if (equal_or_nan(fabs_d(out_axis.y), big_f)) {
            if (fpu.mul(out_axis.y, ax.y) > 0.0) out_angle = -out_angle;
        } else if (equal_or_nan(fabs_d(out_axis.z), big_f)) {
            if (fpu.mul(out_axis.z, ax.z) > 0.0) out_angle = -out_angle;
        }
        if (!equal_or_nan(jo::as_effect_key(part)->orir, 0.0))
            out_angle = static_cast<float>(fpu.add(fpu.mul(jo::as_effect_key(part)->orir, kDeg()), out_angle));
        thiscall(kNodeAngles, node, joan::orig::arg::bits(a1), joan::orig::arg::bits(a2), joan::orig::arg::bits(out_angle));
    }
};

#define FX5(name, addr, fn) JOAN_REPLACE("game/fx5", name, addr, fn)
FX5("ParticleEmitter::update", 0x00675e90, &H::update_parts);
#undef FX5

}  // namespace game_fx5
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx4)
namespace {
namespace game_fx {

std::uint32_t now() { return global<std::uint32_t>(d::g_now); }  // game clock (ms)

void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

JOAN_COUNTER(c_slots, "Effect::slots_alive");

struct H {
    std::uint8_t* part_slot(std::int32_t i) { return reinterpret_cast<std::uint8_t*>(&jo::self<jo::EffectState>(this)->pools[i - 1]); }
    void parts_hide() {  // 0x006771c0
        joan::orig::as_effect_state(this)->parts_alloc();  // 0x0067a240
        for (std::int32_t i = list_count(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters)); i > 0; --i) {
            (void)list_get(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters), i);
            jo::view<std::uint32_t>(jo::as_particle_pool(part_slot(i))->alive_) = 1;
        }
    }
    // EAX on return: the list count when it is <= 0, else the clock value stored by the last iteration.
    std::uint32_t parts_start() {  // 0x00677250
        joan::orig::as_effect_state(this)->parts_alloc();
        std::int32_t i = list_count(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters));
        std::uint32_t r = static_cast<std::uint32_t>(i);
        for (; i > 0; --i) {
            (void)list_get(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters), i);
            std::uint8_t* const slot = part_slot(i);
            jo::view<std::uint32_t>(jo::as_particle_pool(slot)->alive_) = 0;
            r = now();
            jo::view<std::uint32_t>(jo::as_particle_pool(slot)->last_emit) = r;
        }
        return r;
    }
    std::int32_t parts_alive() {  // 0x006772f0
        c_slots.hit();
        std::int32_t sum = 0;
        for (std::int32_t i = list_count(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters)); i > 0; --i) {
            (void)list_get(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters), i);
            if (jo::view<void*>(jo::self<jo::EffectState>(this)->pools) != nullptr) sum += joan::orig::arg::as<std::int32_t>(joan::orig::as_particle_pool(part_slot(i))->alive());  // 0x006638e0
        }
        return sum;
    }
};

#define FX4(name, addr, fn) JOAN_REPLACE("game/fx4", name, addr, fn)
FX4("ParticleEmitter::stop_emit", 0x006771c0, &H::parts_hide);
FX4("ParticleEmitter::start_emit", 0x00677250, &H::parts_start);
FX4("ParticleEmitter::get_num_of_active_particles", 0x006772f0, &H::parts_alive);
#undef FX4

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b109_05.cpp (game/b109_05)
namespace {
namespace game_b109_05 {

void* new_(u32 size) { return joan::crt::operator_new(joan::orig::arg::u32(size)); }  // operator new

JOAN_COUNTER(c_prop_create, "ParticleProp::create");

void* new_obj(u32 size, std::uintptr_t ctor) {
    void* const mem = new_(size);
    return mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
}

struct H {
    // ------------------------------------------------------------------------------ 0x00677730
    static void* make(u32 size, std::uintptr_t ctor) { return new_obj(size, ctor); }
    void* prop_create(const char* name) {
        c_prop_create.hit();
        alignas(4) u8 s[0x194];
        std::memset(s, 0, sizeof s);
        joan::orig::as_string(s)->construct();  // 0x005a1170
        joan::orig::as_string(s)->assign(joan::orig::arg::str(name));  // 0x005a1440
        joan::orig::libc::_strupr(joan::orig::arg::str(joan::orig::as_string(s)->c_str()));  // _strupr(0x00417d90())
        auto find = [&](img::Addr key) {
            return joan::orig::as_string(s)->at(joan::orig::arg::str(str(key))) >= 0;  // 0x005a13a0
        };
        auto len = [&]() { return joan::orig::arg::as<i32>(joan::orig::as_name(s)->length()); };  // 0x0045e110
        if (find(d::s_SIZE)) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::Kind0_Kind0));                 // SIZE 0x0067ade0
        if (find(d::s_COLOR_9dc064) && len() == 5) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::Obj_ctor_67af60));   // COLOR 0x0067af60
        if (find(d::s_COLORRED)) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::Kind2_Kind2));                 // COLORRED 0x0067b130
        if (find(d::s_COLORGREEN)) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::Kind3_Kind3));                 // COLORGREEN 0x0067b2b0
        if (find(d::s_COLORBLUE)) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::Kind4_Kind4));                 // COLORBLUE 0x0067b430
        if (find(d::s_COLORALPHA)) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::Kind5_Kind5));                 // COLORALPHA 0x0067b5b0
        if (find(d::s_VELOCITY) && len() == 8) return make(static_cast<u32>(0x33 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::Ctor_67b730));   // VELOCITY 0x0067b730
        if (find(d::s_VELOCITYX)) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::EffectKey7_ctor));                 // VELOCITYX 0x0067b8b0
        if (find(d::s_VELOCITYY)) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::EffectKey8_ctor));                 // VELOCITYY 0x0067ba30
        if (find(d::s_VELOCITYZ)) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::EffectKey9_ctor));                 // VELOCITYZ 0x0067bbb0
        if (find(d::s_EVENTTIMER)) return make(static_cast<u32>(0x23 - 0x1b + sizeof(jo::ParticleKey)), fn::code_value(fn::EffectKeyA_ctor));                 // EVENTTIMER 0x0067bd30
        return nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_05", name, addr, fn)
B("ParticleEmitter::event_factory", 0x00677730, &H::prop_create);
#undef B

}  // namespace game_b109_05
}  // namespace

// ---------------------------------------- from modules/game_b111_02.cpp (game/b111_02)

namespace {
namespace game_b111_02 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::img::str;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::obj;

using joan::hook::global;

u8* peek(void* ts) { return joan::orig::arg::as<u8*>(joan::orig::as_token_list(ts)->peek()); }  // 0x0067ac80
u8* next(void* ts) { return joan::orig::arg::as<u8*>(joan::orig::as_token_list(ts)->next()); }  // 0x0067acb0

// thiscall through the object's vtable slot.
template <class R = void, class... A>
R vcall(const void* obj, int slot, A... args) {
    const std::uintptr_t fn = (*reinterpret_cast<std::uintptr_t* const*>(obj))[slot];
    return thiscall<R>(fn, obj, args...);
}

// throw (char*) (original/cxx_eh.hpp).
[[noreturn]] void fail(img::Addr message) { throw static_cast<char*>(&img::global<char>(message)); }
[[noreturn]] void fail(char* message) { throw message; }

using joan::orig::helpers::type;
const char* text(const u8* tok) { return jo::as_token(tok)->text; }
i32 strlen_(const char* s) { return joan::orig::arg::as<i32>(std::strlen(joan::orig::arg::str(s))); }  // strlen
using joan::orig::helpers::strcmp_;  // strcmp
// 0x0058f050 on the string helper 0x00a45fa8: dst = src[start-1 .. start-1+count) + NUL.
void substr(char* dst, const char* src, i32 start, i32 count) {
    joan::orig::as_misc(img::obj(d::g_misc))->str_cut_58f050(joan::orig::arg::str(dst), joan::orig::arg::str(src), joan::orig::arg::i32(start), joan::orig::arg::i32(count));
}
// Predicate on the property token (bool in al; the original tests only al).
bool is(std::uintptr_t thunk, const u8* tok) { return thiscall<u8>(thunk, tok) != 0; }

// 0x191-byte fixed string (ctor-with-text 0x005a11a0 thunk 0x00404548, append 0x005a1550 thunk 0x00401d84).
struct Msg {
    alignas(4) char text[0x194];
};
void msg_init(Msg& m, const char* s) { joan::orig::as_string(m.text)->construct(joan::orig::arg::str(s)); }
void msg_append(Msg& m, const char* s) { joan::orig::as_string(m.text)->append(joan::orig::arg::str(s)); }

// 8-byte {min, max} range, game ctor / dtor (empty dtor; owned on unwind like the original).
struct Range {
    u32 v[2];
    Range() { joan::orig::as_float_range(this)->construct(); }  // 0x0067a4a0
    ~Range() { joan::orig::as_float_range(this)->destruct(); }        // 0x0067a4e0
    Range(const Range&) = delete;
    Range& operator=(const Range&) = delete;
};
// Range returned by 0x0067beb0 (Sequence, hidden return pointer); destroyed like Range.
struct ReturnedRange {
    u32 v[2];
    u8* ret;
    explicit ReturnedRange(void* seq) : v{0, 0} { ret = joan::orig::arg::as<u8*>(joan::orig::as_effect_key(seq)->get_1(joan::orig::arg::mptr(v))); }  // 0x0067beb0
    ~ReturnedRange() { joan::orig::as_float_range(v)->destruct(); }                                               // 0x0067a4e0
    ReturnedRange(const ReturnedRange&) = delete;
    ReturnedRange& operator=(const ReturnedRange&) = delete;
};
// 24-byte XYZ range (mins, maxs), passed by value to the setters (bitwise copy, rep movs).
struct Xyz {
    u32 d[6];
};
struct Xyz12 {  // float members like the callee's (System V: XMM registers)
    float d[3];
};
// The two dwords of a Range, passed by value to the 8-byte setters (same stack bytes as two dword pushes).
struct Range8 {  // float members like the callee's (System V: one XMM register)
    float d[2];
};
Range8 range8_of(const u32 (&v)[2]) {
    Range8 o;
    std::memcpy(&o, v, sizeof o);
    return o;
}
struct XyzRange {
    Xyz v;
    XyzRange() { joan::orig::as_range(&v)->construct(); }  // 0x0067a380
    ~XyzRange() { joan::orig::as_range(&v)->destruct(); }        // 0x0067a3e0
    XyzRange(const XyzRange&) = delete;
    XyzRange& operator=(const XyzRange&) = delete;
    Xyz12 first() const {
        Xyz12 o;
        std::memcpy(&o, v.d, sizeof o);
        return o;
    }
};
// Tokenizer object of 0x00677390's frame (0x2c bytes: vptr, +4 8-byte member, +0xc dynamic array).
struct Tokenizer {
    alignas(sizeof(void*)) u8 raw[((sizeof(jo::TokenList) > 0x2c ? sizeof(jo::TokenList) : 0x2c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // 0x2c on x86
    Tokenizer() { joan::orig::as_token_list(raw)->construct(); }  // 0x0067caa0
    ~Tokenizer() { joan::orig::as_token_list(raw)->destruct(); }        // 0x0067cb70
    Tokenizer(const Tokenizer&) = delete;
    Tokenizer& operator=(const Tokenizer&) = delete;
};
// operator new block owned until the constructor returns (unwind action: operator delete).
struct NewBlock {
    void* p;
    bool armed;
    ~NewBlock() {
        if (armed) joan::crt::operator_delete(joan::orig::arg::mptr(p));  // operator delete
    }
};

float bits_f(u32 b) {
    float f = 0.0f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}
i32 ftol_(u32 b) { return joan::hook::x87::ftol(static_cast<double>(bits_f(b))); }  // fld dword; _ftol

// The original's catch (...) (MSVC 6 frame handler) also took structured exceptions; the ports' catch (...) takes C++
// exceptions only. Nothing on the two try paths faults with a well-formed script: the token readers check their end
// (0x0067acb0: "Unexpected end-of-file." is a C++ throw), compile rejects scripts of fewer than 2 tokens before the
// first peek, Tokenizer::split 0x006840d0 always appends a final token, a NULL event from create_key is reported with
// a C++ throw, and every parse error is a C++ throw. A fault there would be a bug of the port (crash handler instead
// of "Unhandled exception ..."); the test54 gate replays the default run through both catchers.

JOAN_COUNTER(c_compile, "ParticleSystem::compile");
JOAN_COUNTER(c_block, "ParticleSystem::parse_block");
JOAN_COUNTER(c_seq, "ParticleSystem::parse_sequence");

// ------------------------------------------------------------------ 0x00678320 helpers
// 'Cannot specify any sequence properties after specifying events.' then "=".
void seq_prop_head(void* ts, bool events_seen) {
    if (events_seen) fail(d::s_Cannot_specify_any_sequence);
    if (type(next(ts)) != 2) fail(d::s_Expected_equals_sign);  // 'Expected equals sign!'
}

void seq_numeric(u8* seq, void* ts, const u8* prop) {
    Range r;  // state 0
    next(ts);
    joan::orig::as_parser(ts)->range(joan::orig::arg::mptr(r.v));  // 0x0067cbf0
    if (is(fn::code_value(fn::Token_is_LIFETIME), prop)) {                 // 0x0067a5c0
        joan::orig::as_effect_key(seq)->set_1(range8_of(r.v));  // 0x0067bf00
    } else if (is(fn::code_value(fn::Token_is_EMITRATE), prop)) {          // 0x0067a600
        joan::orig::as_effect_key(seq)->set_9(range8_of(r.v));  // 0x0067bf90
        joan::orig::as_effect_key(seq)->set_11(range8_of(r.v));  // 0x0067c020
    } else if (is(fn::code_value(fn::Token_is_NUMPARTICLES), prop)) {          // 0x0067a640
        joan::orig::as_effect_key(seq)->set_134(joan::orig::arg::u32(ftol_(r.v[1])));  // 0x0067c990
    } else if (is(fn::code_value(fn::Token_is_LOOP), prop)) {          // 0x0067a680
        joan::orig::as_effect_key(seq)->set_138(joan::orig::arg::u32(ftol_(r.v[0])));  // 0x0067c9f0
    } else if (is(fn::code_value(fn::Token_is_FOLLOWCAMERA), prop)) {          // 0x0067a840
        joan::orig::as_effect_key(seq)->set_69(static_cast<std::uint8_t>(joan::orig::arg::u32(ftol_(r.v[0]))));  // 0x0067c230
    } else if (is(fn::code_value(fn::Token_is_SELFROTATION), prop)) {          // 0x0067a880
        joan::orig::as_effect_key(seq)->set_8a(static_cast<std::uint8_t>(joan::orig::arg::u32(ftol_(r.v[0]))));  // 0x0067c4e0
    } else if (is(fn::code_value(fn::Token_is_SELFROTSPEED), prop)) {          // 0x0067a900
        joan::orig::as_effect_key(seq)->set_97(range8_of(r.v));  // 0x0067c5e0
    } else if (is(fn::code_value(fn::Token_is_HAS_ROTATION), prop)) {          // 0x0067a940
        joan::orig::as_effect_key(seq)->set_9f(static_cast<std::uint8_t>(joan::orig::arg::u32(ftol_(r.v[0]))));  // 0x0067c650
    } else if (is(fn::code_value(fn::Token_is_ISSMO), prop)) {          // 0x0067aa40
        joan::orig::as_effect_key(seq)->set_13c(static_cast<std::uint8_t>(joan::orig::arg::u32(ftol_(r.v[0]))));  // 0x0067c870
    } else if (is(fn::code_value(fn::Token_is_RCHANGE), prop)) {          // 0x0067aa80
        joan::orig::as_effect_key(seq)->set_123(static_cast<std::uint8_t>(joan::orig::arg::u32(ftol_(r.v[0]))));  // 0x0067c480
    } else if (is(fn::code_value(fn::Token_is_TEXTURENUMBER), prop)) {          // 0x0067aac0
        joan::orig::as_effect_key(seq)->set_128(joan::orig::arg::u32(ftol_(r.v[0])));  // 0x0067c420
    } else if (is(fn::code_value(fn::Token_is_TEXTUREPERIOD), prop)) {          // 0x0067ab00
        joan::orig::as_effect_key(seq)->set_124(joan::orig::arg::u32(r.v[0]));      // 0x0067c3c0 (raw dword)
    } else if (is(fn::code_value(fn::Token_is_TIMEDELAY), prop)) {          // 0x0067a6c0
        joan::orig::as_effect_key(seq)->set_86(joan::orig::arg::bits(bits_f(r.v[0]) * 1000.0f));  // 0x0067c360; fmul dword [0x009cf778] = 1000.0f
    } else if (is(fn::code_value(fn::Token_is_ROTATION_SPEED), prop)) {          // 0x0067a980
        joan::orig::as_effect_key(seq)->set_a0(range8_of(r.v));  // 0x0067c6d0
    } else if (is(fn::code_value(fn::Token_is_ORIR), prop)) {          // 0x0067a7c0
        joan::orig::as_effect_key(seq)->set_76(joan::orig::arg::u32(r.v[0]));      // 0x0067c300 (raw dword)
    } else {
        fail(d::s_Unknown_sequence_numeric_pro);  // 'Unknown sequence numeric property!'
    }
}

void seq_vector(u8* seq, void* ts, const u8* prop) {
    XyzRange r;  // state 1
    next(ts);
    joan::orig::as_parser(ts)->xyz_range(joan::orig::arg::mptr(r.v.d));  // 0x0067cdf0
    if (is(fn::code_value(fn::Token_is_SPAWNDIR), prop)) {                    // 0x0067ab40
        joan::orig::as_effect_key(seq)->set_19(r.v);            // 0x0067c0b0 (ret 0x18)
    } else if (is(fn::code_value(fn::Token_is_EMITRADIUS), prop)) {             // 0x0067ab80
        joan::orig::as_effect_key(seq)->set_31(r.v);            // 0x0067c140
    } else if (is(fn::code_value(fn::Obj_is_gravity), prop)) {             // 0x0067abc0
        joan::orig::as_effect_key(seq)->set_49(r.v);            // 0x0067c1d0
    } else if (is(fn::code_value(fn::Token_is_ORIENTATION), prop)) {             // 0x0067a780
        joan::orig::as_effect_key(seq)->set_6a(r.v.d[0], r.v.d[1], r.v.d[2]);      // 0x0067c2a0 (ret 0xc)
    } else if (is(fn::code_value(fn::Token_is_SELFROTAXIS), prop)) {             // 0x0067a8c0
        joan::orig::as_effect_key(seq)->set_8b(r.v.d[0], r.v.d[1], r.v.d[2]);      // 0x0067c550
    } else if (is(fn::code_value(fn::Token_is_ROTATION_CENTER), prop)) {             // 0x0067a9c0
        joan::orig::as_effect_key(seq)->set_a8(r.v.d[0], r.v.d[1], r.v.d[2]);      // 0x0067c750
    } else if (is(fn::code_value(fn::Token_is_ROTATION_AXIS), prop)) {             // 0x0067aa00
        joan::orig::as_effect_key(seq)->set_b4(r.v.d[0], r.v.d[1], r.v.d[2]);      // 0x0067c7d0
    } else if (is(fn::code_value(fn::Token_is_OFFSET), prop)) {             // 0x0067a800
        joan::orig::as_effect_key(seq)->set_7a(r.v.d[0], r.v.d[1], r.v.d[2]);      // 0x0067ca60
    } else {
        fail(d::s_Unknown_sequence_vector_prop);  // 'Unknown sequence vector property!'
    }
}

// Frame bytes [ebp-0xc8, ebp-0x14) of the original 0x00678320 that its quoted-name copies
// (0x0058f050) can reach: texture-2 name [ebp-0xc8] (0x28 bytes), texture name [ebp-0x9c]
// (0x28), sequence name [ebp-0x48] (0x28), "events seen" byte [ebp-0x18]. A copy longer than
// its 0x28-byte buffer runs on into the following locals exactly as in the original; every
// other slot it can reach is dead at that point (lengths, prop / token pointers, the two mode
// dwords written by 0x0067d490 / 0x0067d590 before any read, the ranges re-initialised by their
// ctors 0x0067a4a0 / 0x0067a380 and fully rewritten by 0x0067cbf0 / 0x0067cdf0), so only the
// events byte matters and it lives at its original offset here. A copy reaching [ebp-0x14]
// (this) and beyond corrupts the original's frame (this / saved esp / SEH link): undefined there,
// we only keep it inside the 0x200-byte array (string tokens are at most 256 chars, lexer
// 0x006840d0).
struct SeqFrame {
    alignas(4) u8 b[0x200];
    static constexpr int kTexture2 = 0xc8 - 0xc8;
    static constexpr int kTexture = 0xc8 - 0x9c;
    static constexpr int kName = 0xc8 - 0x48;
    static constexpr int kEvents = 0xc8 - 0x18;
    char* buf(int off) { return reinterpret_cast<char*>(b + off); }
    bool events() const { return b[kEvents] != 0; }
};

// Texture properties: "= \"<file>\"", name copied without its quotes, then the setter.
void seq_texture(u8* seq, void* ts, SeqFrame& fr, int buf, std::uintptr_t setter) {
    seq_prop_head(ts, fr.events());
    const u8* f = next(ts);
    if (type(f) != 8) fail(d::s_Expected_filename_after_text);  // 'Expected filename after texture sequence property.'
    char* const file = fr.buf(buf);
    const i32 n = strlen_(text(f));
    substr(file, text(f), 3, n - 4);
    thiscall(setter, seq, file);
    next(ts);
}

// try body of the event line (state 4); no objects with destructors here.
void event_body(u8* self, u8* seq, void* ts, const u8* tok, const Range& when, u8 fade, u8** pev) {
    *pev = joan::orig::arg::as<u8*>(joan::orig::as_effect_state(self)->create_key(joan::orig::arg::str(text(tok))));  // 0x00677730
    u8* const ev = *pev;
    if (ev == nullptr) fail(d::s_Unknown_event_type_or_there);  // 'Unknown event type, or there was an error creating this event.'
    if (fade != 0) {
        if (vcall<u8>(ev, 2) == 0) fail(d::s_Fading_is_not_supported_on_t);  // 'Fading is not supported on this event.'
    }
    vcall(ev, 3, ts);
    jo::as_particle_key(ev)->owner = jo::as_effect_key(seq);
    joan::orig::as_particle_key(ev)->set_time_range(joan::orig::arg::i32(when.v[0]), joan::orig::arg::i32(when.v[1]));  // 0x0067ad70
    joan::orig::as_particle_key(ev)->set_flag8(static_cast<std::int8_t>(joan::orig::arg::i32(fade)));                   // 0x0067adb0 (reads the byte only)
    joan::orig::dynarray(&jo::as_effect_key(seq)->keys)->linkin(joan::orig::arg::ptr(pev));  // 0x00584620
}

// Event line: time spec, event keyword, event body (try / catch of FuncInfo 0x009f06f8).
void seq_event(u8* self, u8* seq, void* ts) {
    Range when;  // state 2: [ebp-0xd4]
    u8 fade = 0;  // [ebp-0xd8]
    {
        ReturnedRange init(seq);  // state 3
        joan::orig::as_parser(ts)->time_spec(joan::orig::arg::mptr(when.v), &fade, 0u, joan::orig::arg::u32(jo::view<u32>(jo::as_float_range(init.ret)->hi)));  // 0x0067d340
    }
    const u8* tok = peek(ts);
    if (type(tok) != 0x1c && type(tok) != 0x1d && type(tok) != 0x1e)
        fail(d::s_Expecting_particle_property);  // 'Expecting particle property after time specifier!'
    u8* ev = nullptr;  // [ebp-0xdc]
    try {  // state 4
        event_body(self, seq, ts, tok, when, fade, &ev);
    } catch (char* msg) {  // funclet 0x00678e28
        if (ev != nullptr) vcall<void*>(ev, 0, 1);  // scalar deleting dtor
        fail(msg);
    } catch (...) {  // funclet 0x00678e93
        if (ev != nullptr) vcall<void*>(ev, 0, 1);
        fail(d::s_Unhandled_exception_creating);  // 'Unhandled exception creating event!'
    }
}

// ------------------------------------------------------------------ 0x00677390 helpers
// try body of compile (states 0..1).
void compile_body(u8* self, const char* script, Msg& version) {
    char name[0x200];  // [ebp-0x68] (0x24 bytes in the original), written and never read
    Tokenizer tk;      // state 1
    joan::orig::as_script_tokenizer(tk.raw)->split(joan::orig::arg::str(script));  // 0x006840d0
    joan::orig::as_counter(&jo::as_token_list(tk.raw)->tokens)->clamp_to_one();    // 0x004b9d40
    if (joan::orig::as_dynarray(&jo::as_token_list(tk.raw)->tokens)->size() < 2)  // 0x00417ad0
        fail(d::s_This_script_is_too_small_to);  // 'This script is too small to be valid.'
    const u8* tok = peek(tk.raw);
    if (type(tok) != 9) fail(d::s_First_word_must_be_ParticleS);  // 'First word must be ParticleSystem.'
    tok = next(tk.raw);
    if (type(tok) != 8) fail(d::s_Must_name_particle_system);  // 'Must name particle system.'
    const i32 len = strlen_(text(tok));
    substr(name, text(tok), 2, len - 2);
    joan::orig::as_effect_state(self)->set_name(joan::orig::arg::str(text(tok)));  // 0x0067a1c0
    tok = next(tk.raw);
    if (type(tok) != 1) fail(d::s_Must_have_version_number);  // 'Must have version number.'
    if (strcmp_(text(tok), str(d::s_1_00)) != 0) {  // '1.00'
        msg_init(version, str(d::s_Version_number_mismatched_Th));  // 'Version number mismatched. ...'
        msg_append(version, str(d::s_1_00));
        msg_append(version, str(d::s_dot_9d36e8));
        fail(version.text);
    }
    tok = next(tk.raw);
    if (type(tok) != 6) fail(d::s_Missing_opening_brace_for_Pa);  // 'Missing opening brace for ParticleSystem block.'
    joan::orig::as_effect_state(self)->parse_block(joan::orig::arg::mptr(tk.raw));    // 0x00677f00
}

struct H {
    // ------------------------------------------------------------------ 0x00677390
    bool compile(const char* script) {
        c_compile.hit();
        u8* const self = reinterpret_cast<u8*>(this);
        joan::orig::as_effect_state(self)->clear_sequences();                    // 0x00677e00
        joan::orig::as_effect_state(self)->set_error_text(str(d::s_0_error_s_you_re_good_to_go));  // 0x0067a200 "0 error(s), you're good to go!"
        Msg version;  // [ebp-0x200], may be thrown: kept outside the try
        try {  // states 0..1
            compile_body(self, script, version);
        } catch (char* msg) {  // funclet 0x006775d2
            joan::orig::as_effect_state(self)->set_error_text(joan::orig::arg::str(msg));  // 0x0067a200
            joan::orig::as_effect_state(self)->clear_sequences();       // 0x00677e00
            return false;
        } catch (...) {  // funclet 0x006775fe
            joan::orig::as_effect_state(self)->set_error_text(str(d::s_Unhandled_exception_in_Parti));  // 'Unhandled exception in ParticleEmitter::compile.'
            joan::orig::as_effect_state(self)->clear_sequences();
            return false;
        }
        u32 tmp[3] = {0, 0, 0};
        const u32* r = joan::orig::arg::as<u32*>(joan::orig::as_range(&jo::as_effect_state(self)->range)->random_point(joan::orig::arg::mptr(tmp)));  // 0x0067a400 (hidden return)
        jo::view<u32>(jo::as_effect_state(self)->pos[0]) = r[0];
        jo::view<u32>(jo::as_effect_state(self)->pos[1]) = r[1];
        jo::view<u32>(jo::as_effect_state(self)->pos[2]) = r[2];
        return true;
    }

    // ------------------------------------------------------------------ 0x00677f00
    void parse_block(void* ts) {
        c_block.hit();
        u8* const self = reinterpret_cast<u8*>(this);
        bool sequences = false;  // [ebp-0x14]
        Msg unexpected;          // [ebp-0x1d4]
        next(ts);
        for (;;) {
            const u8* tok = peek(ts);
            if (type(tok) == 7) return;
            const u8* const prop = tok;  // [ebp-0x1c]
            const i32 kind = type(tok);
            if (kind == 0xa) {
                sequences = true;
                void* const mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::EffectKey)));  // operator new (0x162 on x86)
                u8* seq = nullptr;
                {
                    NewBlock guard{mem, true};  // state 2
                    if (mem != nullptr) seq = joan::orig::arg::as<u8*>(joan::orig::as_panel(mem)->ctor_67ef90());  // 0x0067ef90
                    guard.armed = false;
                }
                u8* local = seq;  // [ebp-0x40]
                joan::orig::as_effect_key(local)->init_once();                       // 0x0067f210
                joan::orig::dynarray(jo::as_effect_state(self)->emitters)->linkin(&local);  // 0x00584620
                joan::orig::as_effect_state(self)->parse_sequence(joan::orig::arg::mptr(local), joan::orig::arg::mptr(ts));             // 0x00678320
            } else if (kind == 0x17) {
                if (sequences) fail(d::s_Cannot_specify_any_particle);  // 'Cannot specify any particle system properties after specifying sequences.'
                if (type(next(ts)) != 2) fail(d::s_Expected_equals_sign);  // 'Expected equals sign!'
                XyzRange r;  // state 0
                next(ts);
                joan::orig::as_parser(ts)->xyz_range(joan::orig::arg::mptr(r.v.d));  // 0x0067cdf0
                if (is(fn::code_value(fn::Token_is_POSITION), prop))              // 0x0067a580
                    joan::orig::as_effect_state(self)->set_position_67ad10(r.v);   // 0x0067ad10 (ret 0x18)
                else
                    fail(d::s_Unknown_particle_system_prop);  // 'Unknown particle system property!'
            } else if (kind == 0x18) {
                if (sequences) fail(d::s_Cannot_specify_any_particle);
                if (type(next(ts)) != 2) fail(d::s_Expected_equals_sign);
                Range r;  // state 1
                next(ts);
                joan::orig::as_parser(ts)->range(joan::orig::arg::mptr(r.v));  // 0x0067cbf0
                if (is(fn::code_value(fn::Token_is_SOURCEBLENDING), prop))                          // 0x0067a700
                    joan::orig::as_effect_state(self)->set_source_blend(joan::orig::arg::i16(ftol_(r.v[0])));     // 0x0041b320
                else if (is(fn::code_value(fn::Token_is_DESTBLENDING), prop))                     // 0x0067a740
                    joan::orig::as_effect_state(self)->set_dest_blend(joan::orig::arg::i16(ftol_(r.v[0])));     // 0x0041b370
                else
                    fail(d::s_Unknown_particle_system_prop);
            } else {
                msg_init(unexpected, str(d::s_Unexpected));  // 'Unexpected "'
                msg_append(unexpected, text(tok));
                msg_append(unexpected, str(d::s_in_Particle_System_Block));  // '" in Particle System Block!'
                fail(unexpected.text);
            }
        }
    }

    // ------------------------------------------------------------------ 0x00678320
    void parse_sequence(u8* seq, void* ts) {
        c_seq.hit();
        u8* const self = reinterpret_cast<u8*>(this);
        SeqFrame fr;          // [ebp-0xc8 .. ebp-0x14), see SeqFrame
        fr.b[SeqFrame::kEvents] = 0;  // [ebp-0x18] "events seen"
        Msg unexpected;       // [ebp-0x274]
        const u8* tok = next(ts);
        if (type(tok) != 8) fail(d::s_Must_name_particle_sequence);  // 'Must name particle sequence block!'
        const i32 len = strlen_(text(tok));
        substr(fr.buf(SeqFrame::kName), text(tok), 3, len - 4);
        joan::orig::as_effect_key(seq)->set_name(joan::orig::arg::str(fr.buf(SeqFrame::kName)));  // 0x00681e10
        tok = next(ts);
        if (type(tok) != 6) fail(d::s_Expected_opening_brace_for_p);  // 'Expected opening brace for particle sequence block!'
        next(ts);
        for (;;) {
            tok = peek(ts);
            if (type(tok) == 7) break;
            const u8* const prop = tok;  // [ebp-0x20]
            switch (static_cast<u32>(type(tok)) - 0xeu) {
            case 24 - 14:  // numeric property
                seq_prop_head(ts, fr.events());
                seq_numeric(seq, ts, prop);
                break;
            case 25 - 14:  // vector property
                seq_prop_head(ts, fr.events());
                seq_vector(seq, ts, prop);
                break;
            case 26 - 14: {  // color op
                seq_prop_head(ts, fr.events());
                next(ts);
                u32 mode = 0;  // [ebp-0x70], always written by 0x0067d490 (or it throws)
                joan::orig::as_parser(ts)->color_op(&mode);  // 0x0067d490
                if (is(fn::code_value(fn::Token_is_COLOROP), prop))              // 0x0067ac00
                    joan::orig::as_effect_key(seq)->set_color_op(joan::orig::arg::u32(mode));   // 0x0067c900
                else
                    fail(d::s_Unknown_sequence_color_op_mo);  // 'Unknown sequence color op mode property!'
                break;
            }
            case 27 - 14: {  // alpha op
                seq_prop_head(ts, fr.events());
                next(ts);
                u32 mode = 0;  // [ebp-0x74], always written by 0x0067d590 (or it throws)
                joan::orig::as_parser(ts)->alpha_op(&mode);  // 0x0067d590
                if (is(fn::code_value(fn::Token_is_ALPHAOP), prop))              // 0x0067ac40
                    joan::orig::as_effect_key(seq)->set_alpha_op(joan::orig::arg::u32(mode));   // 0x0067c930
                else
                    fail(d::s_Unknown_sequence_alpha_op_mo);  // 'Unknown sequence alpha op mode property!'
                break;
            }
            case 14 - 14:  // texture
                seq_texture(seq, ts, fr, SeqFrame::kTexture, fn::code_value(fn::Object_set_name_e1));  // 0x00681cd0
                break;
            case 15 - 14:  // second texture
                seq_texture(seq, ts, fr, SeqFrame::kTexture2, fn::code_value(fn::ModelHolder_load_681d10));  // 0x00681d10
                break;
            case 16 - 14:
            case 18 - 14:
            case 19 - 14:
            case 20 - 14:  // event
                fr.b[SeqFrame::kEvents] = 1;
                seq_event(self, seq, ts);
                break;
            default:
                msg_init(unexpected, str(d::s_Unexpected));  // 'Unexpected "'
                msg_append(unexpected, text(tok));
                msg_append(unexpected, str(d::s_in_Sequence_Block));  // '" in Sequence Block!'
                fail(unexpected.text);
            }
        }
        joan::orig::as_effect_key(seq)->randomize_key_times();  // 0x00680eb0
        joan::orig::as_effect_key(seq)->sort_keys();  // 0x00680f40
        joan::orig::as_effect_key(seq)->link_keys();  // 0x00680f70
        next(ts);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_02", name, addr, fn)
B("ParticleEmitter::compile", 0x00677390, &H::compile);
B("ParticleEmitter::process_particle_system_block", 0x00677f00, &H::parse_block);
B("ParticleEmitter::process_event_sequence_block", 0x00678320, &H::parse_sequence);
#undef B

}  // namespace game_b111_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::EffectState::init() -> void { joan::bind::member<&::game_b85_05::H::particle_init>(this); }  // 0x6759a0
auto joan::orig::Actor::release_675b30(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82a::H::actor_release>(this, a0)); }  // 0x675b30
auto joan::orig::EffectState::update_parts(float a0, void* a1) -> void { joan::bind::member<&::game_fx5::H::update_parts>(this, a0, a1); }  // 0x675e90
auto joan::orig::EffectState::parts_hide() -> void { joan::bind::member<&::game_fx::H::parts_hide>(this); }  // 0x6771c0
auto joan::orig::EffectState::parts_start() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_fx::H::parts_start>(this)); }  // 0x677250
auto joan::orig::EffectState::parts_alive() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx::H::parts_alive>(this)); }  // 0x6772f0
auto joan::orig::EffectState::compile(const char* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b111_02::H::compile>(this, a0)); }  // 0x677390
auto joan::orig::EffectState::create_key(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b109_05::H::prop_create>(this, a0)); }  // 0x677730
auto joan::orig::EffectState::clear_sequences() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_14::H::cont_clear>(this)); }  // 0x677e00
auto joan::orig::EffectState::parse_block(void* a0) -> void { joan::bind::member<&::game_b111_02::H::parse_block>(this, a0); }  // 0x677f00
auto joan::orig::EffectState::parse_sequence(void* a0, void* a1) -> void { joan::bind::member<&::game_b111_02::H::parse_sequence>(this, a0, a1); }  // 0x678320
