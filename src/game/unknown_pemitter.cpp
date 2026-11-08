// game/unknown_pemitter.cpp: functions whose original source file is unknown (formerly game/_unplaced/pemitter.cpp;
// re/attribution.csv: between ww/enparticle/peventsq.cpp and ww/enpath/spindex.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
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
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "stream_cache_api.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::obj;

using joan::img::str;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

JOAN_COUNTER(c_panel, "Panel::ctor_67ef90");

struct H {
    // ------------------------------------------------------------------ 0x0067ef90
    void* ctor_67ef90() {
        c_panel.hit();
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->lifetime)->construct();   // 0x0067a4a0
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->emit_rate)->construct();
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->emit_rate2)->construct();
        joan::orig::as_range(jo::self<jo::EffectKey>(this)->spawn_dir)->construct();  // 0x0067a380
        joan::orig::as_range(jo::self<jo::EffectKey>(this)->emit_radius)->construct();
        joan::orig::as_range(jo::self<jo::EffectKey>(this)->gravity)->construct();
        joan::orig::as_float_range(&jo::self<jo::EffectKey>(this)->range_61)->construct();
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->self_rot_speed)->construct();
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->rotation_speed)->construct();
        joan::orig::dynarray(&jo::self<jo::EffectKey>(this)->keys)->construct(static_cast<i32>(sizeof(void*)), i32{0x10}, i32{0});  // 0x005843d0 (key pointers: 4 on x86)
        joan::orig::fill_except(joan::orig::arg::mptr(obj(d::g_misc)), joan::orig::arg::str(static_cast<void*>(this)), i32{0}, joan::orig::arg::u32(static_cast<u32>(sizeof(jo::EffectKey))), i32{1}, &jo::self<jo::EffectKey>(this)->keys, static_cast<i32>(sizeof(jo::DynArray)));  // 0x005919a0 (0x162 / 0x1d on x86)
        jo::self<jo::PartEmitter>(this)->u32_12c = 0;
        jo::self<jo::PartEmitter>(this)->u32_130 = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Panel::ctor_67ef90", 0x0067ef90, &H::ctor_67ef90);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {

JOAN_COUNTER(c_dtor_67f100, "Obj67f100::~Obj");

struct H {
    void dtor_67f100() {
        c_dtor_67f100.hit();
        joan::orig::dynarray(&jo::self<jo::EffectKey>(this)->keys)->destruct();  // 0x005844b0
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->rotation_speed)->destruct();   // 0x0067a4e0
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->self_rot_speed)->destruct();
        joan::orig::as_float_range(&jo::self<jo::EffectKey>(this)->range_61)->destruct();
        joan::orig::as_range(jo::self<jo::EffectKey>(this)->gravity)->destruct();   // 0x0067a3e0
        joan::orig::as_range(jo::self<jo::EffectKey>(this)->emit_radius)->destruct();
        joan::orig::as_range(jo::self<jo::EffectKey>(this)->spawn_dir)->destruct();
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->emit_rate2)->destruct();
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->emit_rate)->destruct();
        joan::orig::as_float_range(jo::self<jo::EffectKey>(this)->lifetime)->destruct();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Obj67f100::~Obj", 0x0067f100, &H::dtor_67f100);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b93_05.cpp (game/b93_05)
namespace {
namespace game_b93_05 {

struct H {

    // 0x00681cd0 / 0x00681e10
    void name_e1(const char* s) {
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::EffectKey>(this)->texture_name)), joan::orig::arg::str(s)); }
    void name_102(const char* s) {
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::EffectKey>(this)->name_102)), joan::orig::arg::str(s)); }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b93_05", name, addr, fn)
GAME("Object::set_name_e1", 0x00681cd0, &H::name_e1);
GAME("Object::set_name_102", 0x00681e10, &H::name_102);
#undef GAME

}  // namespace game_b93_05
}  // namespace

// ---------------------------------------- from modules/game_fx5.cpp (game/fx6)
namespace {
namespace game_fx5 {

using joan::hook::global;

struct V3 {
    float x, y, z;
};


constexpr fn::Code kListCount = fn::List_count_417ad0;   // 0x00417ad0
constexpr fn::Code kListGet = fn::List_get_41b3f0;     // 0x0041b3f0

std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }

struct Obj {};
using joan::call::import_call;


// x87 comparisons (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool below_or_nan(double a, double b) { return !(a >= b); }
bool equal_or_nan(double a, double b) { return !(a < b || a > b); }

// Constants (.rdata).
float kDeg() { return global<float>(d::flt_0_017453292); }       // pi / 180 (float)

constexpr img::Addr kRandom(d::g_misc);
constexpr fn::Code kPoolSlot = fn::SlotPool_slot;    // 0x00663960
constexpr fn::Code kNodeAngleA = fn::Actor_set_h_dir;  // 0x005ac6a0
constexpr fn::Code kNodeAngleB = fn::Actor_set_r_dir;  // 0x005ac980
constexpr fn::Code kNodeAngleC = fn::Actor_set_v_dir;  // 0x005ac810
constexpr fn::Code kNodeMove = fn::Actor_set_pos_5ac310;    // 0x005ac310
constexpr fn::Code kRandRange = fn::random_float_lo_float_hi;   // 0x005906f0 Random::range(lo, hi)

std::uint8_t* list_get(const void* list, std::int32_t i) { return thiscall<std::uint8_t*>(kListGet, list, i); }
// 0x005906f0 Misc::random_float(float lo, float hi): the arguments are floats (given here as their bits) and the result is the st0 double (ilib/misc_fp.cpp)
float rand_range(std::uint32_t lo, std::uint32_t hi) { return static_cast<float>(thiscall<double>(kRandRange, img::obj(kRandom), joan::orig::arg::f32(lo), joan::orig::arg::f32(hi))); }

float length(const void* v) { return static_cast<float>(import_call<double>(d::iat_v3d_87_length, v)); }
float dot(const void* a, const void* b) { return static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, a, b)); }

// acos(dot(u, v) / |u| / |v|) as stored to a double local (dot and the first quotient are
// stored to floats).
double acos_between(V3 u, V3 v) {
    const joan::hook::x87::Fpu fpu{true};
    const float d = dot(&u, &v);
    const float t = static_cast<float>(fpu.div(d, length(&u)));
    const double q = fpu.div(t, length(&v));
    return joan::orig::arg::as<double>(joan::orig::libc::acos(joan::orig::arg::f64(q)));  // acos
}

JOAN_COUNTER(c_spawn, "PartEmitter::spawn");
JOAN_COUNTER(c_keys, "PartEmitter::apply_keys");

struct H {
    // 0x00681070 PartEmitter::spawn(pos, dt, a, b, follow kind, scene, slot), ret 0x24: a new
    // particle in the slot pool with random life / offset / velocity / spin, start delay
    // fmod(random(0, dt), life); the part's keys (+0x145, by time +0x11) up to its age are
    // applied; emitter parts (+0x13c) get a scene node (0x10c bytes) oriented along +0x6a.
    // (The original has an SEH frame only for the node constructor's unwinding.)
    void emitter_spawn(V3 pos, float dt, std::int16_t a, std::int16_t b, std::int16_t follow, void* scene, std::uint8_t* slot) {
        c_spawn.hit();
        const joan::hook::x87::Fpu fpu{true};
        const std::int32_t index = joan::orig::arg::as<std::int32_t>(joan::orig::as_particle_pool(slot)->alloc());  // 0x00663800 SlotPool::alloc
        std::uint8_t* const p = thiscall<std::uint8_t*>(kPoolSlot, slot, index);
        joan::orig::as_particle(p)->reset();  // 0x00662e20 Particle::reset
        jo::as_particle(p)->lifetime = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->lifetime[0]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->lifetime[1]));
        const std::int32_t kind = follow;
        if (kind == 3 || kind == 4) {
            jo::as_particle(p)->offset[0] = static_cast<float>(fpu.add(fpu.add(rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[0]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[3])), jo::self<jo::EffectKey>(this)->offset[0]), pos.x));
            jo::as_particle(p)->offset[1] = static_cast<float>(fpu.add(fpu.add(rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[1]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[4])), jo::self<jo::EffectKey>(this)->offset[1]), pos.y));
            jo::as_particle(p)->offset[2] = static_cast<float>(fpu.add(fpu.add(rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[2]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[5])), jo::self<jo::EffectKey>(this)->offset[2]), pos.z));
            std::memcpy(jo::as_particle(p)->pos, jo::as_particle(p)->offset, 12);
        } else {
            jo::as_particle(p)->offset[0] = static_cast<float>(fpu.add(rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[0]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[3])), jo::self<jo::EffectKey>(this)->offset[0]));
            jo::as_particle(p)->offset[1] = static_cast<float>(fpu.add(rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[1]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[4])), jo::self<jo::EffectKey>(this)->offset[1]));
            jo::as_particle(p)->offset[2] = static_cast<float>(fpu.add(rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[2]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->emit_radius[5])), jo::self<jo::EffectKey>(this)->offset[2]));
            jo::as_particle(p)->pos[0] = pos.x + jo::as_particle(p)->offset[0];
            jo::as_particle(p)->pos[1] = pos.y + jo::as_particle(p)->offset[1];
            jo::as_particle(p)->pos[2] = pos.z + jo::as_particle(p)->offset[2];
        }
        jo::as_particle(p)->vel[0] = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->spawn_dir[0]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->spawn_dir[3]));
        jo::as_particle(p)->vel[1] = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->spawn_dir[1]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->spawn_dir[4]));
        jo::as_particle(p)->vel[2] = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->spawn_dir[2]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->spawn_dir[5]));
        jo::as_particle(p)->rot_speed = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->rotation_speed[0]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->rotation_speed[1]));
        jo::as_particle(p)->self_rot_speed = rand_range(jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->self_rot_speed[0]), jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->self_rot_speed[1]));
        if (!equal_or_nan(jo::as_particle(p)->lifetime, 0.0)) {
            const double life = jo::as_particle(p)->lifetime;
            std::uint32_t dt_bits;
            std::memcpy(&dt_bits, &dt, sizeof dt_bits);
            const double r = rand_range(0u, dt_bits);
            jo::as_particle(p)->delay = joan::orig::arg::as<float>(joan::orig::libc::fmod(joan::orig::arg::f64(r), joan::orig::arg::f64(life)));  // fmod
        }
        jo::view<std::uint32_t>(jo::as_particle(p)->time) = 0;
        jo::view<std::uint32_t>(jo::as_particle(p)->age) = 0;
        const std::int32_t keys = list_count(&jo::self<jo::EffectKey>(this)->keys);
        std::int32_t i = 1;
        for (; i <= keys; ++i) {
            std::uint8_t* const key = list_get(&jo::self<jo::EffectKey>(this)->keys, i);
            if (jo::as_particle_key(key)->time_ > jo::as_particle(p)->time) break;
            using Virtual = void (Obj::*)(void*);
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(key)[1], sizeof(void*));
            (reinterpret_cast<Obj*>(key)->*method)(static_cast<void*>(p));
        }
        if (static_cast<std::int8_t>(jo::self<jo::EffectKey>(this)->is_smo) != 0) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86; operator new
            jo::view<void*>(jo::as_particle(p)->node) = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20 SceneObject::SceneObject
            std::uint8_t* const node = jo::view<std::uint8_t*>(jo::as_particle(p)->node);
            jo::as_actor(node)->def = jo::self<jo::EffectKey>(this)->u32_141;
            joan::orig::as_scene_object(node)->init();                                         // 0x005aab70
            joan::orig::as_scene(scene)->attach(joan::orig::arg::mptr(static_cast<void*>(node)));              // 0x005efc50 Scene::attach
            thiscall(kNodeMove, node, static_cast<void*>(jo::as_particle(p)->pos));
            joan::orig::as_actor(node)->set_faded(static_cast<char>(joan::orig::arg::i32(1)));                                      // 0x005ae590
            jo::view<std::uint32_t>(jo::as_actor(node)->scale) = jo::view<std::uint32_t>(jo::as_particle(p)->scale);
            jo::view<std::uint32_t>(jo::as_actor(node)->kind) = 2;
            std::uint8_t* const plane = jo::view<std::uint8_t*>(jo::as_actor(node)->plane);
            std::uint32_t packed = 0;
            joan::orig::as_colour(static_cast<void*>(&packed))->from_rgba(joan::orig::arg::mptr(static_cast<void*>(jo::as_particle(p)->colour)));  // 0x005e95b0
            joan::orig::as_plane(static_cast<void*>(plane))->set_fade(joan::orig::arg::u32(packed));                         // 0x005d5e80 Plane::set_fade
            for (std::int32_t k = 0; k < jo::as_plane(plane)->material_count; ++k) {
                const std::uint32_t blend = (static_cast<std::uint32_t>(static_cast<std::int32_t>(a)) | 0x10000u) |
                                            (static_cast<std::uint32_t>(static_cast<std::int32_t>(b)) << 4);
                jo::as_plane(plane)->materials[k].blend = blend;
                jo::as_plane(plane)->materials[k].texture_mode =
                    (jo::self<jo::EffectKey>(this)->u32_12c | 0x20000u) | jo::self<jo::EffectKey>(this)->u32_130;
                jo::as_plane(plane)->materials[k].pass_mask = 2;
            }
            joan::orig::as_plane(static_cast<void*>(plane))->invalidate(1u);  // 0x005cf920 Plane::invalidate
            if (jo::self<jo::EffectKey>(this)->orientation[0] > 0.0f)
                thiscall(kNodeAngleA, node, 0u);
            else if (below_or_nan(jo::self<jo::EffectKey>(this)->orientation[0], 0.0))
                thiscall(kNodeAngleA, node, 0x40490fdbu);
            if (jo::self<jo::EffectKey>(this)->orientation[1] > 0.0f)
                thiscall(kNodeAngleA, node, 0x3fc90fdbu);
            else if (below_or_nan(jo::self<jo::EffectKey>(this)->orientation[1], 0.0))
                thiscall(kNodeAngleA, node, 0xbfc90fdbu);
            if (jo::self<jo::EffectKey>(this)->orientation[2] > 0.0f)
                thiscall(kNodeAngleC, node, 0xbfc90fdbu);
            else if (below_or_nan(jo::self<jo::EffectKey>(this)->orientation[2], 0.0))
                thiscall(kNodeAngleC, node, 0x3fc90fdbu);
            if (!equal_or_nan(jo::self<jo::EffectKey>(this)->orientation[0], 0.0) && !equal_or_nan(jo::self<jo::EffectKey>(this)->orientation[1], 0.0)) {
                const double t = acos_between(V3{1.0f, 0.0f, 0.0f}, V3{jo::self<jo::EffectKey>(this)->orientation[0], jo::self<jo::EffectKey>(this)->orientation[1], 0.0f});
                if (below_or_nan(jo::self<jo::EffectKey>(this)->orientation[1], 0.0))
                    thiscall(kNodeAngleA, node, joan::orig::arg::bits(static_cast<float>(-t)));
                else
                    thiscall(kNodeAngleA, node, joan::orig::arg::bits(static_cast<float>(t)));
            }
            if (!equal_or_nan(jo::self<jo::EffectKey>(this)->orientation[0], 0.0) && !equal_or_nan(jo::self<jo::EffectKey>(this)->orientation[2], 0.0)) {
                const double t = acos_between(V3{jo::self<jo::EffectKey>(this)->orientation[0], 0.0f, 0.0f}, V3{jo::self<jo::EffectKey>(this)->orientation[0], 0.0f, jo::self<jo::EffectKey>(this)->orientation[2]});
                if (jo::self<jo::EffectKey>(this)->orientation[2] > 0.0f)
                    thiscall(kNodeAngleC, node, joan::orig::arg::bits(static_cast<float>(-t)));
                else
                    thiscall(kNodeAngleC, node, joan::orig::arg::bits(static_cast<float>(t)));
            } else if (!equal_or_nan(jo::self<jo::EffectKey>(this)->orientation[1], 0.0) && !equal_or_nan(jo::self<jo::EffectKey>(this)->orientation[2], 0.0)) {
                const double t = acos_between(V3{0.0f, jo::self<jo::EffectKey>(this)->orientation[1], 0.0f}, V3{0.0f, jo::self<jo::EffectKey>(this)->orientation[1], jo::self<jo::EffectKey>(this)->orientation[2]});
                if (jo::self<jo::EffectKey>(this)->orientation[2] > 0.0f)
                    thiscall(kNodeAngleC, node, joan::orig::arg::bits(static_cast<float>(-t)));
                else
                    thiscall(kNodeAngleC, node, joan::orig::arg::bits(static_cast<float>(t)));
            }
            if (!equal_or_nan(jo::self<jo::EffectKey>(this)->orir, 0.0)) thiscall(kNodeAngleB, node, joan::orig::arg::bits(static_cast<float>(fpu.mul(jo::self<jo::EffectKey>(this)->orir, kDeg()))));
        } else {
            jo::as_particle(p)->node = nullptr;
        }
        jo::as_particle(p)->key_index = static_cast<std::int16_t>(i);
    }


    // 0x00681b10 PartEmitter::apply_keys(p): from the particle's key index (+0x78) apply the
    // keys (+0x145, time +0x11) whose time has come; a key that changes the particle's
    // time (+0x10) restarts the scan at the first key not before it.
    void apply_keys(std::uint8_t* p) {
        c_keys.hit();
        const std::int32_t n = list_count(&jo::self<jo::EffectKey>(this)->keys);
        std::int32_t i = jo::as_particle(p)->key_index;
        for (; i <= n; ++i) {
            std::uint8_t* const key = list_get(&jo::self<jo::EffectKey>(this)->keys, i);
            const float before = jo::as_particle(p)->time;
            if (joan::orig::arg::as<float>(joan::orig::as_particle_key(key)->time()) > jo::as_particle(p)->time) break;  // 0x00681e50 Key::time
            using Virtual = void (Obj::*)(void*);
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(key)[1], sizeof(void*));
            (reinterpret_cast<Obj*>(key)->*method)(static_cast<void*>(p));
            if (equal_or_nan(jo::as_particle(p)->time, before)) continue;
            for (std::int32_t k = 1; k <= n; ++k) {
                std::uint8_t* const other = list_get(&jo::self<jo::EffectKey>(this)->keys, k);
                if (!below_or_nan(joan::orig::arg::as<float>(joan::orig::as_particle_key(other)->time()), jo::as_particle(p)->time)) {
                    i = k - 1;
                    break;
                }
            }
        }
        jo::as_particle(p)->key_index = static_cast<std::int16_t>(i);
    }
    float key_time() { return jo::self<jo::ParticleKey>(this)->time_; }  // 0x00681e50
};

#define FX6(name, addr, fn) JOAN_REPLACE("game/fx6", name, addr, fn)
FX6("PartEmitter::spawn", 0x00681070, &H::emitter_spawn);
FX6("PartEmitter::apply_keys", 0x00681b10, &H::apply_keys);
FX6("PartKey::time", 0x00681e50, &H::key_time);
#undef FX6

}  // namespace game_fx5
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

using u16 = std::uint16_t;


JOAN_COUNTER(c_update_entries, "Obj680::update_entries");
JOAN_COUNTER(c_link_entries, "Obj680::link_entries");

struct H {
    // ------------------------------------------------------------------ 0x00680eb0
    i32 update_entries() {
        c_update_entries.hit();
        void* const list = &jo::self<jo::EffectKey>(this)->keys;
        i32 i = joan::orig::as_dynarray(list)->size();  // 0x00417ad0
        for (; i > 0; --i) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            const float v = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->random_float(joan::orig::arg::f32(jo::view<u32>(jo::as_particle_key(e)->time_range.lo)), joan::orig::arg::f32(jo::view<u32>(jo::as_particle_key(e)->time_range.hi))));  // 0x005906f0
            joan::orig::as_particle_key(e)->set_time(joan::orig::arg::bits(v));  // 0x00681e70
        }
        return i;
    }

    // ------------------------------------------------------------------ 0x00680f70
    i32 link_entries() {
        c_link_entries.hit();
        void* const list = &jo::self<jo::EffectKey>(this)->keys;
        const i32 n = joan::orig::as_dynarray(list)->size();  // 0x00417ad0
        i32 i = 1;
        for (; i <= n; ++i) {
            for (i32 j = i + 1; j <= n; ++j) {
                u8* const a = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
                u8* const b = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(j)));
                const u32 ka = joan::orig::arg::as<u32>(joan::orig::as_particle_key(a)->kind());  // 0x0067ad50
                if (ka != joan::orig::arg::as<u32>(joan::orig::as_particle_key(b)->kind())) continue;
                if ((joan::orig::arg::as<u32>(joan::orig::as_particle_key(b)->flag8()) & 0xffu) == 0) continue;  // 0x0067ef70
                jo::view<u16>(jo::as_particle_key(a)->next) = static_cast<u16>(j);
                break;
            }
        }
        return n < 1 ? n : i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("Obj680::update_entries", 0x00680eb0, &H::update_entries);
B("Obj680::link_entries", 0x00680f70, &H::link_entries);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

struct H {
    // ------------------------------------------------------------------ 0x00680f40
    u32 sort_145() {
        return thiscall<u32>(fn::quick_sort, &jo::self<jo::EffectKey>(this)->keys, fn::code_value(fn::by_key_compare));  // 0x005850b0
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj::sort_145", 0x00680f40, &H::sort_145);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

// ------------------------------------------------------------------ cdecl
// 0x00681c50: qsort comparator on the float key +0x11 (0x00681e50) of *a / *b.
i32 JOAN_CDECL by_key_compare(void** a, void** b) {
    const float ka = joan::orig::arg::as<float>(joan::orig::as_particle_key(*a)->time());  // 0x00681e50
    const float kb = joan::orig::arg::as<float>(joan::orig::as_particle_key(*b)->time());
    if (ka == kb || ka != ka || kb != kb) return 0;  // test ah,0x40: equal or unordered
    if (!(ka > kb)) return -1;                        // test ah,0x41 jne
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("by_key_compare", 0x00681c50, &by_key_compare);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
namespace {
namespace game_b92_05 {

void crt_strcpy(void* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(d))), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_model_load, "ModelHolder::load");

struct H {
    // 0x00681d10
    void model_load(const char* name) {
        c_model_load.hit();
        if (static_cast<std::int8_t>(*name) == 0) return;
        crt_strcpy(reinterpret_cast<std::uint8_t*>(jo::self<jo::EffectKey>(this)->model_name), name);
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::ActorDef)));  // 0x2d8 on x86; operator new
        void* const m = mem != nullptr ? joan::orig::as_actor_def(mem)->construct() : nullptr;  // 0x005af390
        jo::view<void*>(jo::self<jo::EffectKey>(this)->u32_141) = m;
        joan::orig::as_actor_def(jo::self<jo::EffectKey>(this)->u32_141)->load2(str(d::s_MODEL_9d3750), joan::orig::arg::str(reinterpret_cast<std::uint8_t*>(jo::self<jo::EffectKey>(this)->model_name)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), 0, 0u);  // 0x005af3d0 ('MODEL\')
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("ModelHolder::load", 0x00681d10, &H::model_load);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

struct H {
    // ------------------------------------------------------------------ 0x00681e70
    void set_11(i32 v) { jo::view<i32>(jo::self<jo::ParticleKey>(this)->time_) = v; }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Obj::set_11", 0x00681e70, &H::set_11);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Panel::ctor_67ef90() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_04::H::ctor_67ef90>(this)); }  // 0x67ef90
auto joan::orig::EffectKey::destruct() -> void { joan::bind::member<&::game_b84_04::H::dtor_67f100>(this); }  // 0x67f100
auto joan::orig::EffectKey::randomize_key_times() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_15::H::update_entries>(this)); }  // 0x680eb0
auto joan::orig::EffectKey::sort_keys() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_08::H::sort_145>(this)); }  // 0x680f40
auto joan::orig::EffectKey::link_keys() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_15::H::link_entries>(this)); }  // 0x680f70
auto joan::orig::EffectKey::apply_keys(void* a0) -> void { joan::bind::member<&::game_fx5::H::apply_keys>(this, a0); }  // 0x681b10
auto joan::orig::EffectKey::set_name(const char* a0) -> void { joan::bind::member<&::game_b93_05::H::name_102>(this, a0); }  // 0x681e10
auto joan::orig::ParticleKey::time() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_fx5::H::key_time>(this)); }  // 0x681e50
auto joan::orig::ParticleKey::set_time(std::int32_t a0) -> void { joan::bind::member<&::game_b95_08::H::set_11>(this, a0); }  // 0x681e70
