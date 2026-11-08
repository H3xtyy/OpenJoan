// SPDX-License-Identifier: GPL-3.0-or-later
// enparticle/peventsq.cpp: functions of the original d:\ww\enparticle\peventsq.cpp (re/attribution.csv). The file name
// is the Restaurant Empire 2 engine's (enparticle/PEventSq.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__
// string for it.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../game/stream_cache_api.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b93_05.cpp (game/b93_05)
namespace {
namespace game_b93_05 {

struct H {
    // 0x0067f210
    void init_once() {
        if (jo::self<jo::EffectKey>(this)->initialised == 0) {
            jo::self<jo::EffectKey>(this)->u32_141 = nullptr;
            jo::self<jo::EffectKey>(this)->initialised = 1;
        }
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b93_05", name, addr, fn)
GAME("ParticleEventSequence::init", 0x0067f210, &H::init_once);
#undef GAME

}  // namespace game_b93_05
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

JOAN_COUNTER(c_release, "Owner::release");

struct H {
    // 0x0067f250
    void release() {
        c_release.hit();
        if (jo::self<jo::EffectKey>(this)->initialised == 0) return;
        void* const list = &jo::self<jo::EffectKey>(this)->keys;
        for (std::int32_t i = joan::orig::as_dynarray(list)->size(); i > 0; --i) {  // 0x00417ad0
            void* const e = joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (e == nullptr) continue;
            const std::uintptr_t slot0 = (*static_cast<std::uintptr_t**>(e))[0];
            thiscall<void*>(slot0, e, 1);  // virtual deleting destructor
        }
        joan::orig::dynarray(list)->zap(0);  // 0x00584580
        if (jo::view<void*>(jo::self<jo::EffectKey>(this)->u32_141) != nullptr) {
            joan::orig::as_actor_def(jo::view<void*>(jo::self<jo::EffectKey>(this)->u32_141))->destroy(1u);  // 0x004b0c20 deleting dtor
            jo::self<jo::EffectKey>(this)->u32_141 = nullptr;
        }
        jo::self<jo::EffectKey>(this)->initialised = 0;
    }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("ParticleEventSequence::deinit", 0x0067f250, &H::release);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_fx5.cpp (game/fx6)
namespace {
namespace game_fx5 {

using joan::hook::global;

struct V3 {
    float x, y, z;
};

std::uint32_t now() { return global<std::uint32_t>(d::g_now); }  // game clock (ms)

using joan::call::import_call;

V3 load3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

// x87 comparisons (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool below_or_nan(double a, double b) { return !(a >= b); }
bool equal_or_nan(double a, double b) { return !(a < b || a > b); }

// Constants (.rdata).
float k360() { return global<float>(d::flt_360); }       // 360.0f
float kDeg() { return global<float>(d::flt_0_017453292); }       // pi / 180 (float)
double kTwoPi() { return global<double>(d::dbl_6_283185307179586); }   // 2 pi
double kNegTwoPi() { return global<double>(d::dbl_m6_283185307179586); }  // -2 pi

constexpr img::Addr kRandom(d::g_misc);
constexpr fn::Code kPoolCount = fn::ParticleArray_size;   // 0x006638b0
constexpr fn::Code kPoolFree = fn::SlotPool_free_at;    // 0x00663910
constexpr fn::Code kPoolSlot = fn::SlotPool_slot;    // 0x00663960
constexpr fn::Code kPoolAlive = fn::SlotPool_alive;   // 0x006638e0
constexpr fn::Code kPoolRelease = fn::ParticleArray_remove_particle_from_sector;  // 0x00663850
constexpr fn::Code kRotAxis = fn::init_rotation_axis;     // 0x0060bca0 rotation_axis(out, axis, angle)
constexpr fn::Code kRotZ = fn::init_rotation_z_60bc50;        // 0x0060bc50 matrix_rotation(out, angle)
constexpr fn::Code kRow1 = fn::matrix_get_down;        // 0x0060c1d0
constexpr fn::Code kAxisX = fn::matrix_get_right;       // 0x0060c180
constexpr fn::Code kNodeMove = fn::Actor_set_pos_5ac310;    // 0x005ac310
constexpr fn::Code kRandRange = fn::random_float_lo_float_hi;   // 0x005906f0 Random::range(lo, hi)

// 0x005906f0 Misc::random_float(float lo, float hi): the arguments are floats (given here as their bits) and the result is the st0 double (ilib/misc_fp.cpp)
float rand_range(std::uint32_t lo, std::uint32_t hi) { return static_cast<float>(thiscall<double>(kRandRange, img::obj(kRandom), joan::orig::arg::f32(lo), joan::orig::arg::f32(hi))); }

float length(const void* v) { return static_cast<float>(import_call<double>(d::iat_v3d_87_length, v)); }

JOAN_COUNTER(c_emitter, "PartEmitter::update");
JOAN_COUNTER(c_billboards, "PartBillboards::draw");

struct H {
    // 0x0067f380 PartEmitter::update(dt, pos, vel, matrix, follow kind, a, b, scene, slot),
    // ret 0x34: advance / kill the live particles, then spawn by rate (+9 / +0xd random range
    // per second) while the slot is on and started (+0x36 + delay +0x86).
    void emitter_update(float dt, V3 pos, V3 vel, void* m, char follow, std::int16_t a, std::int16_t b, void* scene,
                        std::uint8_t* slot) {
        c_emitter.hit();
        const joan::hook::x87::Fpu fpu{true};
        const std::int32_t kind = static_cast<std::int8_t>(follow);
        if (kind != 1 && kind == 2) {
            float inv[16];
            import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), m);           // matrix_inverse(out, in)
            import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(&vel), static_cast<void*>(inv));  // vector_transform
        }
        for (std::int32_t j = thiscall<std::int32_t>(kPoolCount, slot); j > 0; --j) {
            if (thiscall<std::int32_t>(kPoolFree, slot, j) != 0) continue;
            std::uint8_t* const p = thiscall<std::uint8_t*>(kPoolSlot, slot, j);
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_particle(p)->update(joan::orig::arg::f32(dt), pos, vel, static_cast<char>(joan::orig::arg::i32(follow)))) == 0) {  // 0x00662f60 Particle::update
                thiscall(kPoolRelease, slot, j);
                continue;
            }
            if (jo::self<jo::EffectKey>(this)->texture_number > 1 && static_cast<std::int8_t>(jo::self<jo::EffectKey>(this)->is_smo) == 0) {
                if (fpu.add(dt, jo::as_particle(p)->frame_time) >= jo::self<jo::EffectKey>(this)->texture_period) {
                    if (static_cast<std::int8_t>(jo::self<jo::EffectKey>(this)->rchange) != 0)
                        jo::as_particle(p)->frame =
                            static_cast<std::int16_t>(joan::orig::as_misc(img::obj(kRandom))->random(joan::orig::arg::i32(jo::self<jo::EffectKey>(this)->texture_number)));  // 0x00590540
                    else
                        jo::as_particle(p)->frame =
                            static_cast<std::int16_t>((static_cast<std::int32_t>(jo::as_particle(p)->frame) + 1) % jo::self<jo::EffectKey>(this)->texture_number);
                    jo::view<std::uint32_t>(jo::as_particle(p)->frame_time) = 0;
                } else {
                    jo::as_particle(p)->frame_time = static_cast<float>(fpu.add(dt, jo::as_particle(p)->frame_time));
                }
            }
            const float rx = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->gravity[0]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->gravity[3]));
            const float ry = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->gravity[1]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->gravity[4]));
            const float rz = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->gravity[2]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->gravity[5]));
            jo::as_particle(p)->vel[0] = static_cast<float>(fpu.add(fpu.mul(dt, rx), jo::as_particle(p)->vel[0]));
            jo::as_particle(p)->vel[1] = static_cast<float>(fpu.add(fpu.mul(dt, ry), jo::as_particle(p)->vel[1]));
            jo::as_particle(p)->vel[2] = static_cast<float>(fpu.add(fpu.mul(dt, rz), jo::as_particle(p)->vel[2]));
            if (static_cast<std::int8_t>(jo::self<jo::EffectKey>(this)->has_rotation) != 0) {
                jo::view<float>(jo::as_particle(p)->u32_38) = static_cast<float>(
                    fpu.add(fpu.mul(fpu.mul(fpu.mul(dt, jo::as_particle(p)->rot_speed), k360()), kDeg()), jo::view<float>(jo::as_particle(p)->u32_38)));
                if (!(static_cast<double>(jo::view<float>(jo::as_particle(p)->u32_38)) > kNegTwoPi()))
                    jo::view<float>(jo::as_particle(p)->u32_38) = static_cast<float>(fpu.add(jo::view<float>(jo::as_particle(p)->u32_38), kTwoPi()));
                if (static_cast<double>(jo::view<float>(jo::as_particle(p)->u32_38)) >= kTwoPi())
                    jo::view<float>(jo::as_particle(p)->u32_38) = static_cast<float>(fpu.sub(jo::view<float>(jo::as_particle(p)->u32_38), kTwoPi()));
                float rot[16];
                cdecl_call<void*>(kRotAxis, static_cast<void*>(rot), static_cast<void*>(jo::self<jo::EffectKey>(this)->rotation_axis), jo::view<float>(jo::as_particle(p)->u32_38));
                V3 tmp;
                V3 c = load3(joan::orig::arg::as<void*>(joan::orig::vec_add(static_cast<void*>(&tmp), static_cast<void*>(jo::self<jo::EffectKey>(this)->rotation_center), static_cast<void*>(&pos))));  // 0x0060cc20 add
                joan::orig::vec3_sub_assign(joan::orig::arg::mptr(static_cast<void*>(jo::as_particle(p)->pos)), joan::orig::arg::mptr(static_cast<void*>(&c)));  // 0x0060ce20 -=
                import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(jo::as_particle(p)->pos), static_cast<void*>(rot));  // point_transform
                V3 tmp2;
                V3 c2 = load3(joan::orig::arg::as<void*>(joan::orig::vec_add(static_cast<void*>(&tmp2), static_cast<void*>(jo::self<jo::EffectKey>(this)->rotation_center), static_cast<void*>(&pos))));
                joan::orig::vec_add_in_place(static_cast<void*>(jo::as_particle(p)->pos), static_cast<void*>(&c2));  // 0x0060cdc0 +=
                if (jo::view<void*>(jo::as_particle(p)->node) != nullptr) thiscall(kNodeMove, jo::view<void*>(jo::as_particle(p)->node), static_cast<void*>(jo::as_particle(p)->pos));
            }
            joan::orig::as_effect_key(this)->apply_keys(joan::orig::arg::mptr(static_cast<void*>(p)));  // 0x00681b10
        }
        if (jo::view<std::uint32_t>(jo::as_particle_pool(slot)->alive_) != 0) return;
        if (fpu.add(static_cast<double>(jo::view<std::uint32_t>(jo::as_particle_pool(slot)->last_emit)), jo::self<jo::EffectKey>(this)->time_delay) > static_cast<double>(now())) return;
        const float r = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_rate[0]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_rate[1]));
        std::int32_t n = joan::hook::x87::ftol(fpu.mul(r, dt));
        jo::as_particle_pool(slot)->carry = static_cast<float>(fpu.add(fpu.sub(fpu.mul(r, dt), static_cast<double>(n)), jo::as_particle_pool(slot)->carry));
        if (jo::as_particle_pool(slot)->carry > 1.0f) {
            n += joan::hook::x87::ftol(jo::as_particle_pool(slot)->carry);
            const std::int32_t whole = joan::hook::x87::ftol(jo::as_particle_pool(slot)->carry);
            jo::as_particle_pool(slot)->carry = static_cast<float>(fpu.sub(jo::as_particle_pool(slot)->carry, static_cast<double>(whole)));
        }
        for (std::int32_t q = 0; q < n; ++q) {
            if (thiscall<std::int32_t>(kPoolAlive, slot) >= jo::self<jo::EffectKey>(this)->num_particles) return;
            const std::int16_t kind16 = static_cast<std::int16_t>(static_cast<std::int8_t>(follow));
            if (kind == 3 || kind == 4) {
                const V3 s{static_cast<float>(fpu.add(fpu.div(fpu.mul(static_cast<double>(q), vel.x), static_cast<double>(n)), pos.x)),
                           static_cast<float>(fpu.add(fpu.div(fpu.mul(static_cast<double>(q), vel.y), static_cast<double>(n)), pos.y)),
                           static_cast<float>(fpu.add(fpu.div(fpu.mul(static_cast<double>(q), vel.z), static_cast<double>(n)), pos.z))};
                joan::orig::as_effect_key(this)->spawn(s, joan::orig::arg::f32(dt), joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::i16(kind16), joan::orig::arg::mptr(scene), joan::orig::arg::mptr(static_cast<void*>(slot)));  // 0x00681070 spawn
            } else {
                joan::orig::as_effect_key(this)->spawn(pos, joan::orig::arg::f32(dt), joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::i16(kind16), joan::orig::arg::mptr(scene), joan::orig::arg::mptr(static_cast<void*>(slot)));
            }
            if (jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->loop) != 0) jo::as_particle_pool(slot)->emitted = jo::as_particle_pool(slot)->emitted + 1;
            if (jo::as_particle_pool(slot)->emitted >= jo::self<jo::EffectKey>(this)->num_particles && jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->loop) != 0 && jo::view<std::uint32_t>(jo::as_particle_pool(slot)->alive_) == 0) {
                jo::view<std::uint32_t>(jo::as_particle_pool(slot)->alive_) = 1;
                return;
            }
        }
    }

    // 0x0067faf0 PartBillboards::draw(plane, ctx, scale (unused), matrix, a, b, dt, slot),
    // ret 0x20: one camera-facing quad (4 vertices of 0x18 bytes, 2 triangles) per live
    // particle into the effect plane, material +0xe1 (texture, blend a / b, +0x12c / +0x130),
    // frames +0x128 across u. Orientation by +0x69: 1 camera axes, 2 camera roll, 3 along
    // the particle velocity, 4 a fixed basis from +0x6a and the camera, other: tilted from
    // +0x6a; +0x8a spin.
    //
    // The kind 3 / 4 bases are 3x3 matrices built in a 4x4 local whose 4th column / row the
    // original leaves uninitialised; they only go to v3d vector_transform (direction
    // transform, used by the game on velocities), here zero.
    void billboards_draw(std::uint8_t* plane, void* ctx, std::uint32_t /*scale*/, void* m, std::int16_t a, std::int16_t b,
                         float dt, std::uint8_t* slot) {
        const std::int32_t alive = thiscall<std::int32_t>(kPoolAlive, slot);
        if (alive <= 0) return;
        c_billboards.hit();
        const joan::hook::x87::Fpu fpu{true};
        auto vt = [](V3* v, const void* mat) { import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(v), mat); };  // vector_transform
        auto cross = [](void* out, const void* u, const void* v) { import_call<void*>(d::iat_v3d_87_cross_product, out, u, v); };
        auto normalize_to = [](void* out, const void* v) { return import_call<void*>(d::iat_v3d_87_vector_normalize_2, out, v); };
        auto normalize = [](void* v) { import_call<void*>(d::iat_v3d_87_vector_normalize_1, v); };
        void* const texture = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(d::g_texture_array))->load(joan::orig::arg::str(static_cast<void*>(jo::self<jo::EffectKey>(this)->texture_name)), 1, 0u));  // 0x006099c0
        const std::int32_t mat = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_materials(1));  // 0x005cd470 Plane::add_material
        joan::orig::as_material(jo::as_plane(plane)->materials + mat)->set_texture(0, joan::orig::arg::mptr(texture));  // 0x005d5650
        auto material = [plane, mat]() { return jo::as_plane(plane)->materials + mat; };
        material()->blend = (static_cast<std::uint32_t>(static_cast<std::int32_t>(a)) | 0x10000u) |
                                             (static_cast<std::uint32_t>(static_cast<std::int32_t>(b)) << 4);
        material()->texture_mode = (jo::self<jo::EffectKey>(this)->u32_12c | 0x20000u) | jo::self<jo::EffectKey>(this)->u32_130;
        material()->pass_mask = 2;
        material()->raster = material()->raster & 0xfffff0ffu;
        std::uint8_t* const cam = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_camera(ctx)->bone()))->matrix());  // 0x00613de0 camera bone
        V3 right;
        V3 up;
        cdecl_call<void*>(kAxisX, static_cast<void*>(&right), static_cast<void*>(cam));
        cdecl_call<void*>(kRow1, static_cast<void*>(&up), static_cast<void*>(cam));
        V3 c0{-1.0f, 1.0f, 0.0f};
        V3 c1{1.0f, 1.0f, 0.0f};
        V3 c2{-1.0f, -1.0f, 0.0f};
        V3 c3{1.0f, -1.0f, 0.0f};
        const std::int32_t kind0 = static_cast<std::int8_t>(jo::self<jo::EffectKey>(this)->follow_camera);
        if (kind0 == 1) {
            c0 = V3{-right.x - up.x, -right.y - up.y, -right.z - up.z};
            c1 = V3{right.x - up.x, right.y - up.y, right.z - up.z};
            c2 = V3{-right.x + up.x, -right.y + up.y, -right.z + up.z};
            c3 = V3{right.x + up.x, right.y + up.y, right.z + up.z};
        } else if (kind0 == 2) {
            c0 = V3{0.0f, 1.0f, 1.0f};
            c1 = V3{0.0f, -1.0f, 1.0f};
            c2 = V3{0.0f, 1.0f, -1.0f};
            c3 = V3{0.0f, -1.0f, -1.0f};
            float frame[10];
            joan::orig::as_camera(ctx)->frame(joan::orig::arg::mptr(static_cast<void*>(frame)));  // 0x006140a0 Camera::frame
            const float roll = frame[4];
            float rz[16];
            cdecl_call<void*>(kRotZ, static_cast<void*>(rz), roll);
            vt(&c0, rz);
            vt(&c1, rz);
            vt(&c2, rz);
            vt(&c3, rz);
        } else if (kind0 != 3 && kind0 != 4) {
            const V3 zaxis = load3(img::obj(d::g_a18a60));
            normalize(jo::self<jo::EffectKey>(this)->orientation);
            V3 tilt;
            cross(&tilt, &zaxis, jo::self<jo::EffectKey>(this)->orientation);
            const float len = length(&tilt);
            if (static_cast<double>(len) > global<double>(d::dbl_0_00001)) {  // 1e-5
                const float angle = joan::orig::arg::as<float>(joan::orig::libc::asin(joan::orig::arg::f64(static_cast<double>(len))));  // asin
                V3 axis;
                void* const ax = normalize_to(&axis, &tilt);
                float rot[16];
                cdecl_call<void*>(kRotAxis, static_cast<void*>(rot), ax, angle);
                vt(&c0, rot);
                vt(&c1, rot);
                vt(&c2, rot);
                vt(&c3, rot);
            }
        }
        float inv[16];
        import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), m);  // matrix_inverse(out, in)
        vt(&c0, inv);
        vt(&c1, inv);
        vt(&c2, inv);
        vt(&c3, inv);
        vt(&right, inv);
        vt(&up, inv);
        std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_vertices(joan::orig::arg::i32(alive * 4)));  // 0x005cd7f0 Plane::add_vertices
        const std::int32_t tbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_triangles(joan::orig::arg::i32(alive * 2)));  // 0x005ce6f0 Plane::add_triangles
        jo::VertexF4* vp = jo::as_plane(plane)->vertices_f4 + vbase;
        jo::Tri16* ip = jo::as_plane(plane)->triangles + tbase;
        joan::stream_cache_api::indices_written(jo::as_plane(plane)->triangles);  // written below, in place
        std::uint8_t* const tri_mat = jo::view<std::uint8_t*>(jo::as_plane(plane)->triangle_materials) + tbase * 2;
        joan::orig::as_misc(img::obj(kRandom))->fill_word_591960(joan::orig::arg::mptr(static_cast<void*>(tri_mat)), joan::orig::arg::i32(mat), joan::orig::arg::i32(alive * 2));  // 0x00591960 fill words
        const V3 s0 = c0, s1 = c1, s2 = c2, s3 = c3;
        bool basis_done = false;
        float basis[16] = {};  // rows 0 / 1 / 2 set below; [3], [7], [11], row 3 never set (see above)
        std::int32_t drawn = 0;
        const std::int32_t count = thiscall<std::int32_t>(kPoolCount, slot);
        for (std::int32_t j = 1; j <= count; ++j) {
            if (thiscall<std::int32_t>(kPoolFree, slot, j) != 0) continue;
            std::uint8_t* const p = thiscall<std::uint8_t*>(kPoolSlot, slot, j);
            c0 = s0;
            c1 = s1;
            c2 = s2;
            c3 = s3;
            if (static_cast<std::int8_t>(jo::self<jo::EffectKey>(this)->follow_camera) == 3) {
                float vm[16] = {};  // rows 0 / 1 / 2 set below
                normalize_to(&vm[4], jo::as_particle(p)->vel);
                const V3 d{static_cast<float>(fpu.sub(fpu.sub(jo::as_matrix44(cam)->m[12], jo::as_particle(p)->pos[0]), jo::as_matrix44(m)->m[12])),
                           static_cast<float>(fpu.sub(fpu.sub(jo::as_matrix44(cam)->m[13], jo::as_particle(p)->pos[1]), jo::as_matrix44(m)->m[13])),
                           static_cast<float>(fpu.sub(fpu.sub(jo::as_matrix44(cam)->m[14], jo::as_particle(p)->pos[2]), jo::as_matrix44(m)->m[14]))};
                V3 side;
                cross(&side, jo::as_particle(p)->vel, &d);
                if (equal_or_nan(side.x, 0.0) && equal_or_nan(side.y, 0.0) && equal_or_nan(side.z, 0.0)) continue;
                normalize_to(&vm[0], &side);
                cross(&vm[8], &vm[0], &vm[4]);
                vt(&c0, vm);
                vt(&c1, vm);
                vt(&c2, vm);
                vt(&c3, vm);
            }
            if (static_cast<std::int8_t>(jo::self<jo::EffectKey>(this)->follow_camera) == 4) {
                if (!basis_done) {
                    V3 dir = load3(jo::self<jo::EffectKey>(this)->orientation);
                    normalize(&dir);
                    store_row(basis, 1, dir);
                    void* const bone = joan::orig::arg::as<void*>(joan::orig::as_camera(ctx)->matrix());  // 0x00616e90
                    V3 eye;
                    joan::orig::matrix_position(static_cast<void*>(&eye), joan::orig::arg::ptr(bone));  // 0x0060c310 matrix_position
                    float frame[10];
                    joan::orig::as_camera(ctx)->frame(joan::orig::arg::mptr(static_cast<void*>(frame)));  // 0x006140a0 Camera::frame
                    const double ex = eye.x;
                    const double k = global<double>(d::dbl_100);  // 100.0
                    V3 target;
                    target.x = static_cast<float>(fpu.add(fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(frame[4])))), k), ex));  // cos
                    const double ey = eye.y;
                    target.y = static_cast<float>(fpu.add(fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(frame[4])))), k), ey));  // sin
                    target.z = eye.z;
                    joan::orig::vec3_sub_assign(joan::orig::arg::mptr(static_cast<void*>(&eye)), joan::orig::arg::mptr(static_cast<void*>(&target)));  // 0x0060ce20 -=
                    normalize(&eye);
                    V3 r;
                    cross(&r, &dir, &eye);
                    store_row(basis, 0, r);
                    if ((joan::orig::arg::as<std::uint32_t>(joan::orig::vec_equal(static_cast<void*>(&r), joan::orig::arg::ptr(img::obj(d::g_pos)))) & 0xff) != 0)
                        continue;  // 0x0060ced0 vec_equal
                    V3 u;
                    cross(&u, &r, &dir);
                    if ((joan::orig::arg::as<std::uint32_t>(joan::orig::vec_equal(static_cast<void*>(&u), joan::orig::arg::ptr(img::obj(d::g_pos)))) & 0xff) != 0)
                        continue;
                    store_row(basis, 2, u);
                    basis_done = true;
                }
                vt(&c0, basis);
                vt(&c1, basis);
                vt(&c2, basis);
                vt(&c3, basis);
            }
            if (static_cast<std::int8_t>(jo::self<jo::EffectKey>(this)->self_rotation) != 0) {
                jo::as_particle(p)->self_angle = static_cast<float>(
                    fpu.add(fpu.mul(fpu.mul(fpu.mul(dt, jo::as_particle(p)->self_rot_speed), k360()), kDeg()), jo::as_particle(p)->self_angle));
                if (below_or_nan(jo::as_particle(p)->self_angle, 0.0)) jo::as_particle(p)->self_angle = static_cast<float>(fpu.add(jo::as_particle(p)->self_angle, kTwoPi()));
                if (static_cast<double>(jo::as_particle(p)->self_angle) >= kTwoPi())
                    jo::as_particle(p)->self_angle = static_cast<float>(fpu.sub(jo::as_particle(p)->self_angle, kTwoPi()));
                V3 axis;
                if (static_cast<std::int8_t>(jo::self<jo::EffectKey>(this)->follow_camera) != 0)
                    cross(&axis, &right, &up);
                else
                    axis = load3(jo::self<jo::EffectKey>(this)->self_rot_axis);
                float rot[16];
                cdecl_call<void*>(kRotAxis, static_cast<void*>(rot), static_cast<void*>(&axis), jo::as_particle(p)->self_angle);
                vt(&c0, rot);
                vt(&c1, rot);
                vt(&c2, rot);
                vt(&c3, rot);
            }
            const V3 pos = load3(jo::as_particle(p)->pos);
            std::uint8_t colour[4];
            std::memcpy(colour, jo::as_particle(p)->colour, 4);
            const float size = static_cast<float>(fpu.mul(jo::as_particle(p)->scale, 0.5f));  // 0x009cf77c
            float u0 = 0.0f;
            float du = 1.0f;
            if (jo::self<jo::EffectKey>(this)->texture_number > 1) {
                du = static_cast<float>(fpu.div(global<float>(d::flt_1), static_cast<double>(jo::self<jo::EffectKey>(this)->texture_number)));
                u0 = static_cast<float>(fpu.mul(static_cast<double>(jo::as_particle(p)->frame), du));
            }
            const std::uint8_t bgra[4] = {colour[2], colour[1], colour[0], colour[3]};
            auto emit = [&](const V3& c, float u, float v) {
                float vert[6];
                vert[0] = static_cast<float>(fpu.add(fpu.mul(c.x, size), pos.x));
                vert[1] = static_cast<float>(fpu.add(fpu.mul(c.y, size), pos.y));
                vert[2] = static_cast<float>(fpu.add(fpu.mul(c.z, size), pos.z));
                std::memcpy(&vert[3], bgra, 4);
                vert[4] = u;
                vert[5] = v;
                std::memcpy(vp, vert, sizeof vert);
                ++vp;
            };
            emit(c0, u0, 0.0f);
            emit(c2, u0, 1.0f);
            const float u1 = u0 + du;
            emit(c1, u1, 0.0f);
            emit(c3, u1, 1.0f);
            ip[0].v[0] = static_cast<std::uint16_t>(vbase);
            ip[0].v[1] = static_cast<std::uint16_t>(vbase + 1);
            ip[0].v[2] = static_cast<std::uint16_t>(vbase + 2);
            ip[1].v[0] = static_cast<std::uint16_t>(vbase + 2);
            ip[1].v[1] = static_cast<std::uint16_t>(vbase + 1);
            ip[1].v[2] = static_cast<std::uint16_t>(vbase + 3);
            vbase += 4;
            ip += 2;
            ++drawn;
        }
        if (alive - drawn > 0) {
            jo::as_plane(plane)->triangle_count = jo::as_plane(plane)->triangle_count - (alive - drawn) * 2;
            jo::as_plane(plane)->vertex_count = jo::as_plane(plane)->vertex_count - (alive - drawn) * 4;
        }
        joan::orig::as_plane(static_cast<void*>(plane))->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920 Plane::invalidate
        if (texture != nullptr) joan::orig::texture_ref::release(joan::orig::arg::mptr(texture));  // 0x0042cef0 TextureRef::release
    }
    static void store_row(float* mat, std::int32_t row, const V3& v) { std::memcpy(&mat[row * 4], &v, sizeof v); }
};

#define FX6(name, addr, fn) JOAN_REPLACE("game/fx6", name, addr, fn)
FX6("ParticleEventSequence::update_particles", 0x0067f380, &H::emitter_update);
FX6("ParticleEventSequence::append_particles_to_plane", 0x0067faf0, &H::billboards_draw);
#undef FX6

}  // namespace game_fx5
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::EffectKey::init_once() -> void { joan::bind::member<&::game_b93_05::H::init_once>(this); }  // 0x67f210
auto joan::orig::EffectKey::release() -> void { joan::bind::member<&::game_b83_04::H::release>(this); }  // 0x67f250
auto joan::orig::EffectKey::draw(void* a0, void* a1, std::uint32_t a2, void* a3, std::int16_t a4, std::int16_t a5, float a6, void* a7) -> void { joan::bind::member<&::game_fx5::H::billboards_draw>(this, a0, a1, a2, a3, a4, a5, a6, a7); }  // 0x67faf0
