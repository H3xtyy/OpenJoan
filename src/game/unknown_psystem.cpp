// game/unknown_psystem.cpp: functions whose original source file is unknown (formerly game/_unplaced/psystem.cpp;
// re/attribution.csv: between ww/enparticle/petoken.h and ww/enparticle/petoken.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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
#include "../render/overrides.hpp"
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

using joan::call::stdcall_call;

using joan::orig::helpers::u;

using joan::hook::global;

struct V3 {
    float x, y, z;
};

constexpr fn::Code kListCount = fn::List_count_417ad0;   // 0x00417ad0
constexpr fn::Code kListGet = fn::List_get_41b3f0;     // 0x0041b3f0

std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }

using joan::img::str;

using u8 = std::uint8_t;

using joan::orig::helpers::stricmp_;  // _stricmp

using joan::orig::helpers::addr;

}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {

struct H {
    // ------------------------------------------------------------------ 0x006758f0
    H* ctor_6758f0() {
        joan::orig::as_range(&jo::self<jo::EffectState>(this)->range)->construct();  // 0x0067a380
        std::memset(static_cast<void*>(this), 0, sizeof(jo::EffectState));                        // memset
        return this;
    }

    // ------------------------------------------------------------------ 0x0067a4e0
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("Obj::ctor_6758f0", 0x006758f0, &H::ctor_6758f0);
B("Obj::nop_67a4e0", 0x0067a4e0, &H::nop);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14 {

using joan::orig::helpers::at;

using joan::orig::helpers::op_delete;

JOAN_COUNTER(c_vec_delete, "Zone::vector_deleting_destructor");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // The u32 element count that parts_alloc keeps in the 4 bytes before the array (a byte-stream field, not a member).
    static std::uint32_t array_cookie(const std::uint8_t* array) {
        std::uint32_t n;
        std::memcpy(&n, array - 4, sizeof n);
        return n;
    }

    // 0x00675d70 Zone vector deleting destructor (ret 4); elements 0x3e bytes.
    void* zone_vec_delete(std::uint32_t flags) {
        c_vec_delete.hit();
        if ((flags & 2u) != 0) {
            joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(this)), static_cast<std::uint32_t>(sizeof(jo::ParticlePool)), joan::orig::arg::i32(array_cookie(self())),  // -4: the u32 element count new[] keeps before the array (parts_alloc), not a member
                                                           fn::code_value(fn::Obj663680_destructor));  // eh vector destructor iterator (0x00663680)
            if ((flags & 1u) != 0) op_delete(self() - 4);
            return self() - 4;
        }
        joan::orig::as_particle_pool(this)->destruct();  // 0x00663680
        if ((flags & 1u) != 0) op_delete(this);
        return this;
    }

    // 0x00675930 Zones::~Zones() (ret 0).
    std::uint32_t zones_dtor() {
        joan::orig::as_actor(this)->release_675b30(0u);                     // 0x00675b30
        joan::orig::as_range(&jo::self<jo::EffectState>(this)->range)->destruct();  // 0x0067a3e0
        return 0;  // void callee; callers discard the result
    }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("Zone::vector_deleting_destructor", 0x00675d70, &H::zone_vec_delete);
B82_14("Zones::~Zones", 0x00675930, &H::zones_dtor);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

struct H {
    // ------------------------------------------------------------------ 0x00675e10
    std::uintptr_t release_24() {  // EAX: this / the deleting dtor's result / 0
        std::uintptr_t r = reinterpret_cast<std::uintptr_t>(this);
        if (jo::view<void*>(jo::self<jo::EffectState>(this)->parts) != nullptr) {
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::EffectState>(this)->parts))->release();  // 0x005ccaa0
            void* const p = jo::view<void*>(jo::self<jo::EffectState>(this)->parts);
            r = p != nullptr ? reinterpret_cast<std::uintptr_t>(joan::orig::as_plane(p)->destroy(1u)) : 0;  // 0x00459c40 (deleting dtor)
        }
        jo::self<jo::EffectState>(this)->parts = nullptr;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("Owner::release_24", 0x00675e10, &H::release_24);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx4)
namespace {
namespace game_fx {

void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

struct H {
    std::uint8_t* part_slot(std::int32_t i) { return reinterpret_cast<std::uint8_t*>(&jo::self<jo::EffectState>(this)->pools[i - 1]); }
    void parts_stop() {  // 0x00677120
        joan::orig::as_plane(jo::view<void*>(jo::self<jo::EffectState>(this)->parts))->clear();  // 0x005cd2d0 Plane::clear
        for (std::int32_t i = list_count(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters)); i > 0; --i) {
            (void)list_get(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters), i);
            if (jo::view<void*>(jo::self<jo::EffectState>(this)->pools) != nullptr) {
                std::uint8_t* const slot = part_slot(i);
                joan::orig::as_particle_pool(slot)->release_all();  // 0x00663740
                jo::view<std::uint32_t>(jo::as_particle_pool(slot)->alive_) = 1;
            }
        }
    }
    void parts_alloc() {  // 0x0067a240
        if (jo::view<void*>(jo::self<jo::EffectState>(this)->pools) != nullptr) return;
        const std::int32_t n = list_count(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters));
        std::uint8_t* const block = joan::orig::arg::as<std::uint8_t*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(n) * static_cast<std::uint32_t>(sizeof(jo::ParticlePool)) + 4u)));  // operator new
        std::uint8_t* slots = nullptr;
        if (block != nullptr) {
            *reinterpret_cast<std::int32_t*>(block) = n;  // the element count new[] keeps before the array
            joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(block + 4)), static_cast<std::uint32_t>(sizeof(jo::ParticlePool)), joan::orig::arg::i32(n), fn::code_value(fn::Elem_Elem_663630), fn::code_value(fn::Obj663680_destructor));  // vector constructor: 0x00663630 / 0x00663680
            slots = block + 4;
        }
        jo::view<std::uint8_t*>(jo::self<jo::EffectState>(this)->pools) = slots;
        for (std::int32_t i = 1; i <= list_count(jo::view<void*>(jo::self<jo::EffectState>(this)->emitters)); ++i) joan::orig::as_particle_pool(part_slot(i))->init();  // 0x006636f0
    }
};

#define FX4(name, addr, fn) JOAN_REPLACE("game/fx4", name, addr, fn)
FX4("EffectState::parts_stop", 0x00677120, &H::parts_stop);
FX4("EffectState::parts_alloc", 0x0067a240, &H::parts_alloc);
#undef FX4

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

struct H {
    // 0x00677eb0 (ret 4)
    H* delete_677eb0(std::uint32_t flags) {
        joan::orig::as_effect_key(this)->destruct();  // 0x0067f100
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
    // 0x0067a3e0
    void nothing() {}
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("Obj677eb0::scalar_deleting_destructor", 0x00677eb0, &H::delete_677eb0);
B83_04("Obj67a3e0::nothing", 0x0067a3e0, &H::nothing);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_b86_01.cpp (game/b86_01)
namespace {
namespace game_b86_01 {

JOAN_COUNTER(c_assign, "Handle::assign");

struct H {
    // 0x00679300 Handle::assign(other). ret 4. +0x64 owns flag, +0x6c shared block (+0x68
    // ref count, +0x70 data), +0x70 data. The original's body before this is unreachable
    // (`xor eax,eax; test eax,eax; je`), so only the live tail is reproduced. eax at exit is
    // 0 on the early-out path, else the new +0x70.
    // this and other are EffectState objects (Actor::attach_model 0x0041a930 hands the instance the PemiScript's
    // compiled template): u32_64 owns, share_refs, shared_from, emitters. The Handle view (four words behind a
    // 0x60-byte gap) put them at other x64 offsets, so an instance never saw its template's emitters.
    std::uintptr_t assign(std::uint8_t* other) {
        c_assign.hit();
        jo::EffectState* const me = jo::self<jo::EffectState>(this);
        jo::EffectState* const src = jo::as_effect_state(other);
        if (jo::view<void*>(src->emitters) == nullptr) return 0;
        if (me->owns_emitters != 0) {
            joan::orig::as_effect_state(this)->clear_sequences();  // 0x00677e00
            void* const data = jo::view<void*>(me->emitters);
            if (data != nullptr) joan::orig::as_dynarray(data)->destroy(1u);  // 0x00463fc0 deleting dtor
            me->emitters = nullptr;
        } else {
            me->shared_from->share_refs = me->shared_from->share_refs - 1;
            me->emitters = nullptr;
            me->shared_from = nullptr;
        }
        me->owns_emitters = 0;
        if (src->owns_emitters != 0)
            me->shared_from = src;
        else
            me->shared_from = src->shared_from;
        me->shared_from->share_refs = me->shared_from->share_refs + 1;
        void* const data = jo::view<void*>(me->shared_from->emitters);
        jo::view<void*>(me->emitters) = data;
        return joan::orig::arg::as<std::uintptr_t>(data);
    }

    // 0x0067a1c0 set_name(src): CRT strcpy (0x007b2020) into this + 3. ret 4. eax = dest.
    char* set_name(const char* src) {
        return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(this) + 3)), joan::orig::arg::str(src));
    }
};

#define GAME86(name, addr, fn) JOAN_REPLACE("game/b86_01", name, addr, fn)
GAME86("Handle::assign", 0x00679300, &H::assign);
GAME86("Named::set_name", 0x0067a1c0, &H::set_name);
#undef GAME86

}  // namespace game_b86_01
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

char* strncpy_(void* d, const char* s, std::uint32_t n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }

struct H {
    // 0x0067a200
    void set_name_a6a28c(const char* s) {
        strncpy_(obj(d::g_a6a28c), s, 0xffu);
        global<std::uint8_t>(d::g_a6a38b) = 0;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("set_name_a6a28c", 0x0067a200, &H::set_name_a6a28c);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b111_15.cpp (fix/uninit)
namespace {
namespace game_b111_15 {

struct H {
    // ------------------------------------------------------------------ 0x0067a380 (fix)
    void* range_ctor() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Range));  // original: two uninitialised stack vec3 copies
        return this;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// two never-initialised stack vec3 copied into the object -> zero vectors
FIX("Range::Range", 0x0067a380, &H::range_ctor);
#undef FIX

}  // namespace game_b111_15
}  // namespace

// ---------------------------------------- from modules/game_b100_14.cpp (game/b100_14)
namespace {
namespace game_b100_14 {

struct Vec3 {
    u32 x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

struct H {
    // ------------------------------------------------------------------ 0x0067a400
    // Range: six 32-bit words (lo = +0x0..+0x8, hi = +0xc..+0x14), the same layout on x64.
    Vec3* combine(Vec3* out) {
        const jo::Range* const self = jo::self<jo::Range>(this);
        const Vec3 a = {jo::view<u32>(self->i32_0), self->u32_4, jo::view<u32>(self->current)};
        const Vec3 b = {jo::view<u32>(self->end), self->u32_10, self->u32_14};
        Vec3 tmp;  // [ebp-0x1c]
        const Vec3* r = joan::orig::arg::as<Vec3*>(joan::orig::lerp3(&tmp, a.x, a.y, a.z, b.x, b.y, b.z));  // 0x0067a500
        const Vec3 v = *r;
        *out = v;
        return out;
    }

    // ------------------------------------------------------------------ 0x0067a6c0 ...
    i32 is_timedelay() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_TIMEDELAY)) == 0 ? 1 : 0; }    // 'TIMEDELAY'
    i32 is_followcamera() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_FOLLOWCAMERA)) == 0 ? 1 : 0; } // 'FOLLOWCAMERA'
    i32 is_selfrotspeed() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_SELFROTSPEED)) == 0 ? 1 : 0; } // 'SELFROTSPEED'
    i32 is_rchange() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_RCHANGE)) == 0 ? 1 : 0; }      // 'RCHANGE'
    i32 is_alphaop() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_ALPHAOP)) == 0 ? 1 : 0; }      // 'ALPHAOP'
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_14", name, addr, fn)
B100("Box::combine", 0x0067a400, &H::combine);
B100("Token::is_TIMEDELAY", 0x0067a6c0, &H::is_timedelay);
B100("Token::is_FOLLOWCAMERA", 0x0067a840, &H::is_followcamera);
B100("Token::is_SELFROTSPEED", 0x0067a900, &H::is_selfrotspeed);
B100("Token::is_RCHANGE", 0x0067aa80, &H::is_rchange);
B100("Token::is_ALPHAOP", 0x0067ac40, &H::is_alphaop);
#undef B100

}  // namespace game_b100_14
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {

struct H {
    // 0x0067a4a0
    void* pair_ctor() {
        jo::view<std::uint32_t>(jo::self<jo::FloatRange>(this)->lo) = 0;
        jo::view<std::uint32_t>(jo::self<jo::FloatRange>(this)->hi) = 0;
        return this;
    }
    // 0x0067ac80
    void* holder_inner() { return joan::orig::arg::as<void*>(joan::orig::as_pool(&jo::self<jo::TokenList>(this)->tokens)->last_used()); }  // 0x004b9c30
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Pair::ctor", 0x0067a4a0, &H::pair_ctor);
B("Holder::inner", 0x0067ac80, &H::holder_inner);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b99_09.cpp (game/b99_09)
namespace {
namespace game_b99_09 {

static_assert(sizeof(V3) == 12, "V3");
struct S6 {
    u32 d[6];
};
static_assert(sizeof(S6) == 0x18, "S6");

JOAN_COUNTER(c_lerp3, "lerp3");

i32 strcmp_(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

V3* JOAN_CDECL lerp3(V3* out, u32 a, u32 b, u32 c, u32 ta, u32 tb, u32 tc) {  // 0x0067a500
    c_lerp3.hit();
    void* const m = obj(d::g_misc);
    const float x = joan::orig::arg::as<float>(joan::orig::as_misc(m)->random_float(joan::orig::arg::f32(a), joan::orig::arg::f32(ta)));  // 0x005906f0
    const float y = joan::orig::arg::as<float>(joan::orig::as_misc(m)->random_float(joan::orig::arg::f32(b), joan::orig::arg::f32(tb)));
    const float z = joan::orig::arg::as<float>(joan::orig::as_misc(m)->random_float(joan::orig::arg::f32(c), joan::orig::arg::f32(tc)));
    std::memcpy(&out->x, &x, 4);
    std::memcpy(&out->y, &y, 4);
    std::memcpy(&out->z, &z, 4);
    return out;
}

struct H {
    u32 is_kw(img::Addr s) { return strcmp_(jo::self<jo::Token>(this)->text, str(s)) == 0 ? 1u : 0u; }
    u32 is_sourceblending() { return is_kw(d::s_SOURCEBLENDING); }  // 0x0067a700
    u32 is_selfrotaxis() { return is_kw(d::s_SELFROTAXIS); }     // 0x0067a8c0
    u32 is_issmo() { return is_kw(d::s_ISSMO); }           // 0x0067aa40
    u32 is_spawndir() { return is_kw(d::s_SPAWNDIR); }        // 0x0067ab40

    u32 set_2c(S6 v) {  // 0x0067ad10
        std::memcpy(&jo::self<jo::EffectState>(this)->range, &v, sizeof v);
        joan::orig::as_range(&v)->destruct();  // 0x0067a3e0
        return 0;  // void callee; callers discard the result
    }
    u32 dtor_9dc5e4() {  // 0x0067b6b0
        jo::view<std::uintptr_t>(jo::self<jo::Gauge>(this)->vtable) = img::addr(d::vt_Gauge_9dc5e4);
        joan::orig::as_float_range(&jo::self<jo::Gauge>(this)->value)->destruct();  // 0x0067a4e0
        return joan::orig::arg::as<u32>(joan::orig::as_particle_key(this)->destruct());     // 0x0067d970
    }
    u32* get_11(u32* out) {  // 0x0067bfd0
        const u32 a = jo::view<u32>(jo::self<jo::EffectKey>(this)->emit_rate2[0]), b = jo::view<u32>(jo::self<jo::EffectKey>(this)->emit_rate2[1]);
        out[0] = a;
        out[1] = b;
        return out;
    }
    float get_124() { return jo::self<jo::EffectKey>(this)->texture_period; }  // 0x0067c390
    u32* get_a0(u32* out) {                      // 0x0067c680
        const u32 a = jo::view<u32>(jo::self<jo::EffectKey>(this)->rotation_speed[0]), b = jo::view<u32>(jo::self<jo::EffectKey>(this)->rotation_speed[1]);
        out[0] = a;
        out[1] = b;
        return out;
    }
    std::uintptr_t ptr_c0() { return reinterpret_cast<std::uintptr_t>(jo::self<jo::EffectKey>(this)->model_name); }  // 0x0067c8a0 (EAX = &model_name)
    u32* get_7a(u32* out) {                      // 0x0067ca20
        out[0] = jo::view<u32>(jo::self<jo::EffectKey>(this)->offset[0]);
        out[1] = jo::view<u32>(jo::self<jo::EffectKey>(this)->offset[1]);
        out[2] = jo::view<u32>(jo::self<jo::EffectKey>(this)->offset[2]);
        return out;
    }
};

#define B99(name, addr_, fn) JOAN_REPLACE("game/b99_09", name, addr_, fn)
B99("lerp3", 0x0067a500, &lerp3);
B99("Token::is_SOURCEBLENDING", 0x0067a700, &H::is_sourceblending);
B99("Token::is_SELFROTAXIS", 0x0067a8c0, &H::is_selfrotaxis);
B99("Token::is_ISSMO", 0x0067aa40, &H::is_issmo);
B99("Token::is_SPAWNDIR", 0x0067ab40, &H::is_spawndir);
B99("X::set_2c", 0x0067ad10, &H::set_2c);
B99("X::dtor_9dc5e4", 0x0067b6b0, &H::dtor_9dc5e4);
B99("X::get_11", 0x0067bfd0, &H::get_11);
B99("X::get_124", 0x0067c390, &H::get_124);
B99("X::get_a0", 0x0067c680, &H::get_a0);
B99("X::ptr_c0", 0x0067c8a0, &H::ptr_c0);
B99("X::get_7a", 0x0067ca20, &H::get_7a);
#undef B99

}  // namespace game_b99_09
}  // namespace

// ---------------------------------------- from modules/game_b95_07.cpp (game/b95_07)
namespace {
namespace game_b95_07 {

struct H {
    // ------------------------------------------------------------------ 0x0067a580 ..
    i32 is_position() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_POSITION)) == 0 ? 1 : 0; }       // 'POSITION'
    i32 is_destblending() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_DESTBLENDING)) == 0 ? 1 : 0; }   // 'DESTBLENDING'
    i32 is_has_rotation() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_HAS_ROTATION)) == 0 ? 1 : 0; }   // 'HAS_ROTATION'
    i32 is_texturenumber() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_TEXTURENUMBER)) == 0 ? 1 : 0; }  // 'TEXTURENUMBER'

    // ------------------------------------------------------------------ 0x0067ad70
    void* set_pair(i32 a, i32 b) {
        jo::view<i32>(jo::self<jo::ParticleKey>(this)->time_range.lo) = a;
        jo::view<i32>(jo::self<jo::ParticleKey>(this)->time_range.hi) = b;
        i32 pair[2];
        pair[0] = a;
        pair[1] = b;
        joan::orig::as_float_range(static_cast<void*>(pair))->destruct();  // 0x0067a4e0
        return this;
    }

    // ------------------------------------------------------------------ 0x0067b830
    void* dtor() {
        jo::view<std::uintptr_t>(jo::self<jo::Emitter>(this)->vtable) = img::addr(d::vt_Emitter);
        joan::orig::as_range(jo::self<jo::Emitter>(this)->lo)->destruct();  // 0x0067a3e0
        joan::orig::as_particle_key(this)->destruct();                                               // 0x0067d970
        return this;
    }

    // ------------------------------------------------------------------ 0x0067c060
    void* get_block(void* out) {
        std::memcpy(out, jo::self<jo::EffectKey>(this)->spawn_dir, 6 * 4);
        return out;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_07", name, addr, fn)
B95("Token::is_POSITION", 0x0067a580, &H::is_position);
B95("Token::is_DESTBLENDING", 0x0067a740, &H::is_destblending);
B95("Token::is_HAS_ROTATION", 0x0067a940, &H::is_has_rotation);
B95("Token::is_TEXTURENUMBER", 0x0067aac0, &H::is_texturenumber);
B95("Token::set_pair", 0x0067ad70, &H::set_pair);
B95("Emitter::~Emitter", 0x0067b830, &H::dtor);
B95("Obj::get_block", 0x0067c060, &H::get_block);
#undef B95

}  // namespace game_b95_07
}  // namespace

// ---------------------------------------- from modules/game_b97_07.cpp (game/b97_07)
namespace {
namespace game_b97_07 {

struct H {
    // ------------------------------------------------------------------ 0x0067a5c0 ..
    i32 is_lifetime() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_LIFETIME)) == 0 ? 1 : 0; }        // 'LIFETIME'
    i32 is_offset() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_OFFSET)) == 0 ? 1 : 0; }          // 'OFFSET'
    i32 is_rotation_speed() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_ROTATION_SPEED)) == 0 ? 1 : 0; }  // 'ROTATION_SPEED'
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_07", name, addr, fn)
B97("Token::is_LIFETIME", 0x0067a5c0, &H::is_lifetime);
B97("Token::is_OFFSET", 0x0067a800, &H::is_offset);
B97("Token::is_ROTATION_SPEED", 0x0067a980, &H::is_rotation_speed);
#undef B97

}  // namespace game_b97_07
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07 {

struct H {
    // ------------------------------------------------------------------ 0x0067a600 ..
    i32 is_emitrate() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_EMITRATE)) == 0 ? 1 : 0; }       // 'EMITRATE'
    i32 is_selfrotation() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_SELFROTATION)) == 0 ? 1 : 0; }   // 'SELFROTATION'
    i32 is_textureperiod() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_TEXTUREPERIOD)) == 0 ? 1 : 0; }  // 'TEXTUREPERIOD'

    // ------------------------------------------------------------------ 0x0067ad50
    i32 kind() { return jo::self<jo::ParticleKey>(this)->kind_; }

    // ------------------------------------------------------------------ 0x0067bf40
    i32* get_pair(i32* out) {
        const i32 a = jo::view<i32>(jo::self<jo::EffectKey>(this)->emit_rate[0]);
        const i32 b = jo::view<i32>(jo::self<jo::EffectKey>(this)->emit_rate[1]);
        out[0] = a;
        out[1] = b;
        return out;
    }

    // ------------------------------------------------------------------ 0x0067c4b0
    std::uintptr_t byte_8a() { return (u(this) & 0xffffff00u) | static_cast<u32>(jo::self<jo::EffectKey>(this)->self_rotation); }

    // ------------------------------------------------------------------ 0x0067c8d0
    void* field_102() { return jo::self<jo::EffectKey>(this)->name_102; }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("Token::is_EMITRATE", 0x0067a600, &H::is_emitrate);
B94("Token::is_SELFROTATION", 0x0067a880, &H::is_selfrotation);
B94("Token::is_TEXTUREPERIOD", 0x0067ab00, &H::is_textureperiod);
B94("Kind::kind", 0x0067ad50, &H::kind);
B94("Obj::get_pair", 0x0067bf40, &H::get_pair);
B94("Obj::byte_8a", 0x0067c4b0, &H::byte_8a);
B94("Obj::field_102", 0x0067c8d0, &H::field_102);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- from modules/game_b96_07.cpp (game/b96_07)
namespace {
namespace game_b96_07 {

struct H {
    // ------------------------------------------------------------------ 0x0067a640 ...
    i32 is_kw(img::Addr kw) { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(kw)) == 0 ? 1 : 0; }
    i32 is_NUMPARTICLES() { return is_kw(d::s_NUMPARTICLES); }
    i32 is_ORIR() { return is_kw(d::s_ORIR); }
    i32 is_ROTATION_CENTER() { return is_kw(d::s_ROTATION_CENTER); }
    i32 is_EMITRADIUS() { return is_kw(d::s_EMITRADIUS); }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_07", name, addr, fn)
B96("Token::is_NUMPARTICLES", 0x0067a640, &H::is_NUMPARTICLES);
B96("Token::is_ORIR", 0x0067a7c0, &H::is_ORIR);
B96("Token::is_ROTATION_CENTER", 0x0067a9c0, &H::is_ROTATION_CENTER);
B96("Token::is_EMITRADIUS", 0x0067ab80, &H::is_EMITRADIUS);
#undef B96

}  // namespace game_b96_07
}  // namespace

// ---------------------------------------- from modules/game_b98_07.cpp (game/b98_07)
namespace {
namespace game_b98_07 {

struct H {
    // ------------------------------------------------------------------ 0x0067a680..0x0067ac00
    i32 is_loop() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_LOOP)) == 0 ? 1 : 0; }
    i32 is_orientation() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_ORIENTATION)) == 0 ? 1 : 0; }
    i32 is_rotation_axis() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_ROTATION_AXIS)) == 0 ? 1 : 0; }
    i32 is_colorop() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_COLOROP)) == 0 ? 1 : 0; }

    // ------------------------------------------------------------------ 0x0067b230
    u32 emitter_dtor() {
        jo::view<std::uintptr_t>(jo::self<jo::Emitter>(this)->vtable) = img::addr(d::vt_Gauge_9dc5a8);
        joan::orig::as_float_range(jo::self<jo::Emitter>(this)->lo)->destruct();  // 0x0067a4e0
        return joan::orig::arg::as<u32>(joan::orig::as_particle_key(this)->destruct());                          // 0x0067d970
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_07", name, addr, fn)
B98("Token::is_LOOP", 0x0067a680, &H::is_loop);
B98("Token::is_ORIENTATION", 0x0067a780, &H::is_orientation);
B98("Token::is_ROTATION_AXIS", 0x0067aa00, &H::is_rotation_axis);
B98("Token::is_COLOROP", 0x0067ac00, &H::is_colorop);
B98("Emitter::~Emitter", 0x0067b230, &H::emitter_dtor);
#undef B98

}  // namespace game_b98_07
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

struct H {
    // ------------------------------------------------------------------ 0x0067abc0
    u32 is_gravity() { return stricmp_(jo::view<const char*>(jo::self<jo::Token>(this)->text), str(d::s_GRAVITY)) == 0 ? 1u : 0u; }  // "GRAVITY"

    // ------------------------------------------------------------------ 0x0067aee0
    u32 dtor_67aee0() {
        jo::view<std::uintptr_t>(jo::self<jo::GaugeSize>(this)->vtable) = img::addr(d::vt_Gauge_9dc580);
        joan::orig::as_float_range(&jo::self<jo::Gauge>(this)->value)->destruct();     // 0x0067a4e0
        return joan::orig::arg::as<u32>(joan::orig::as_particle_key(this)->destruct());  // 0x0067d970
    }

    // ------------------------------------------------------------------ 0x0067bb30
    u32 dtor_67bb30() {
        jo::view<std::uintptr_t>(jo::self<jo::Gauge>(this)->vtable) = img::addr(d::vt_Gauge_9dc620);
        joan::orig::as_float_range(&jo::self<jo::Gauge>(this)->value)->destruct();     // 0x0067a4e0
        return joan::orig::arg::as<u32>(joan::orig::as_particle_key(this)->destruct());  // 0x0067d970
    }

    // ------------------------------------------------------------------ 0x0067c180
    void* get_box_49(void* out) {
        std::memcpy(out, jo::self<jo::EffectKey>(this)->gravity, sizeof(jo::self<jo::EffectKey>(this)->gravity));
        return out;
    }

    // ------------------------------------------------------------------ 0x0067c590
    void* get_pair_97(void* out) {
        const u32 a = jo::view<u32>(jo::self<jo::EffectKey>(this)->self_rot_speed[0]);
        const u32 b = jo::view<u32>(jo::self<jo::EffectKey>(this)->self_rot_speed[1]);
        jo::view<u32>(jo::as_float_range(out)->lo) = a;
        jo::view<u32>(jo::as_float_range(out)->hi) = b;
        return out;
    }

    // ------------------------------------------------------------------ 0x0067c840
    u8 byte_13c() { return jo::self<jo::EffectKey>(this)->is_smo; }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::is_gravity", 0x0067abc0, &H::is_gravity);
B97("Obj67aee0::~dtor", 0x0067aee0, &H::dtor_67aee0);
B97("Obj67bb30::~dtor", 0x0067bb30, &H::dtor_67bb30);
B97("Obj::get_box_49", 0x0067c180, &H::get_box_49);
B97("Obj::get_pair_97", 0x0067c590, &H::get_pair_97);
B97("Obj::byte_13c", 0x0067c840, &H::byte_13c);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b111_01.cpp (game/b111_01)
namespace {
namespace game_b111_01 {

[[noreturn]] void throw_message(img::Addr text) {
    throw const_cast<char*>(str(text));  // throw (char*) (original/cxx_eh.hpp)
}

struct H {
    // ------------------------------------------------------------------ 0x0067acb0
    void* particle_next() {
        if (joan::orig::arg::as<i32>(joan::orig::as_range(&jo::self<jo::TokenList>(this)->tokens)->at_end()) != 0)  // 0x00642440
            throw_message(d::s_Unexpected_end_of_file);                       // "Unexpected end-of-file."
        joan::orig::as_reader(&jo::self<jo::TokenList>(this)->tokens)->next();          // 0x0063ee20
        return joan::orig::arg::as<void*>(joan::orig::as_pool(&jo::self<jo::TokenList>(this)->tokens)->last_used());   // 0x004b9c30
    }
};

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_01", name, addr, fn)
B111("ParticleEmitterTokenizer::get_next_token", 0x0067acb0, &H::particle_next);
#undef B111

}  // namespace game_b111_01
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using i8 = std::int8_t;

struct H {
    // ------------------------------------------------------------------ 0x0067adb0
    void* set_byte_8(i8 v) {
        jo::view<i8>(jo::self<jo::ParticleKey>(this)->fade) = v;
        return this;
    }

    // ------------------------------------------------------------------ 0x0067b9b0
    u32 dtor_67b9b0() {
        jo::view<std::uintptr_t>(jo::self<jo::Gauge>(this)->vtable) = img::addr(d::vt_Gauge_9dc60c);
        joan::orig::as_float_range(&jo::self<jo::Gauge>(this)->value)->destruct();           // 0x0067a4e0
        return joan::orig::arg::as<u32>(joan::orig::as_particle_key(this)->destruct());        // 0x0067d970
    }

    // ------------------------------------------------------------------ 0x0067c0f0
    void* get_box(void* out) {
        std::memcpy(out, jo::self<jo::EffectKey>(this)->emit_radius, sizeof(jo::self<jo::EffectKey>(this)->emit_radius));
        return out;
    }

    // ------------------------------------------------------------------ 0x0067c3f0
    i32 field_128() { return jo::self<jo::EffectKey>(this)->texture_number; }

    // ------------------------------------------------------------------ 0x0067c810
    void* addr_e1() { return jo::self<jo::EffectKey>(this)->texture_name; }

    // ------------------------------------------------------------------ 0x0067c960
    i32 field_134() { return jo::self<jo::EffectKey>(this)->num_particles; }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj::set_byte_8", 0x0067adb0, &H::set_byte_8);
B96("Obj67b9b0::~dtor", 0x0067b9b0, &H::dtor_67b9b0);
B96("Obj::get_box_67c0f0", 0x0067c0f0, &H::get_box);
B96("Obj::field_128", 0x0067c3f0, &H::field_128);
B96("Obj::addr_e1", 0x0067c810, &H::addr_e1);
B96("Obj::field_134", 0x0067c960, &H::field_134);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04 {

// SEH constructors 0x0067ade0..0x0067b5b0.
void* kind_ctor(void* self, img::Addr vtable, i32 kind) {
    joan::orig::as_particle_key(self)->construct();                                         // 0x0067d8c0
    joan::orig::as_float_range(&jo::as_gauge(self)->value)->construct();                // 0x0067a4a0
    jo::view<std::uintptr_t>(jo::as_particle_key(self)->vtable) = img::addr(vtable);
    jo::as_particle_key(self)->kind_ = kind;
    return self;
}

struct H {
    // ------------------------------------------------------------------ 0x0067ade0 ..
    void* kind0_ctor() { return kind_ctor(this, d::vt_Gauge_9dc580, 0); }
    void* kind2_ctor() { return kind_ctor(this, d::vt_Gauge_9dc5a8, 2); }
    void* kind3_ctor() { return kind_ctor(this, d::vt_Gauge_9dc5bc, 3); }
    void* kind4_ctor() { return kind_ctor(this, d::vt_Gauge_9dc5d0, 4); }
    void* kind5_ctor() { return kind_ctor(this, d::vt_Gauge_9dc5e4, 5); }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("Kind0::Kind0", 0x0067ade0, &H::kind0_ctor);
B93("Kind2::Kind2", 0x0067b130, &H::kind2_ctor);
B93("Kind3::Kind3", 0x0067b2b0, &H::kind3_ctor);
B93("Kind4::Kind4", 0x0067b430, &H::kind4_ctor);
B93("Kind5::Kind5", 0x0067b5b0, &H::kind5_ctor);
#undef B93

}  // namespace game_b93_04
}  // namespace

// ---------------------------------------- from modules/game_b105_18.cpp (game/b105_18)
namespace {
namespace game_b105_18 {

struct H {
    u8 op_true() { return 1; }                                     // 0x0067ae70 ... (mov al,1)

    // ------------------------------------------------------------------------ 0x0067b090
    u32 op_dtor() {
        jo::view<std::uintptr_t>(jo::self<jo::RenderOp>(this)->vtable) = img::addr(d::vt_Obj_9dc594);
        joan::orig::as_color_range(&jo::self<jo::RenderOp>(this)->value)->destruct();  // 0x0067b110
        return joan::orig::arg::as<u32>(joan::orig::as_particle_key(this)->destruct());                      // 0x0067d970
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_18", name, addr, fn)
B("RenderOp::true_67ae70", 0x0067ae70, &H::op_true);
B("RenderOp::true_67b020", 0x0067b020, &H::op_true);
B("RenderOp::dtor_67b090", 0x0067b090, &H::op_dtor);
B("RenderOp::true_67b1c0", 0x0067b1c0, &H::op_true);
B("RenderOp::true_67b340", 0x0067b340, &H::op_true);
B("RenderOp::true_67b4c0", 0x0067b4c0, &H::op_true);
B("RenderOp::true_67b640", 0x0067b640, &H::op_true);
B("RenderOp::true_67b7c0", 0x0067b7c0, &H::op_true);
B("RenderOp::true_67b940", 0x0067b940, &H::op_true);
B("RenderOp::true_67bac0", 0x0067bac0, &H::op_true);
B("RenderOp::true_67bc40", 0x0067bc40, &H::op_true);
#undef B

}  // namespace game_b105_18
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

void operator_delete(void* p) { joan::crt::operator_delete(joan::orig::arg::mptr(p)); }

// Scalar deleting destructor body: dtor, delete when flag & 1, returns this.
void* deleting_dtor(void* self, std::uintptr_t dtor, u32 flags) {
    thiscall<u32>(dtor, self);
    if ((flags & 1) != 0) operator_delete(self);
    return self;
}

struct H {
    // ------------------------------------------------------------------ deleting dtors
    void* ddtor_67ae90(u32 fl) { return deleting_dtor(this, fn::code_value(fn::Obj67aee0_dtor_dtor), fl); }  // 0x0067aee0
    void* ddtor_67b1e0(u32 fl) { return deleting_dtor(this, fn::code_value(fn::Emitter_dtor_67b230), fl); }  // 0x0067b230
    void* ddtor_67b360(u32 fl) { return deleting_dtor(this, fn::code_value(fn::Obj67b_dtor_67b3b0), fl); }  // 0x0067b3b0
    void* ddtor_67b4e0(u32 fl) { return deleting_dtor(this, fn::code_value(fn::Obj67b_dtor_67b530), fl); }  // 0x0067b530
    void* ddtor_67b660(u32 fl) { return deleting_dtor(this, fn::code_value(fn::X_dtor_9dc5e4), fl); }  // 0x0067b6b0
    void* ddtor_67b7e0(u32 fl) { return deleting_dtor(this, fn::code_value(fn::Emitter_dtor_67b830), fl); }  // 0x0067b830
    void* ddtor_67b960(u32 fl) { return deleting_dtor(this, fn::code_value(fn::Obj67b9b0_dtor_dtor), fl); }  // 0x0067b9b0
    void* ddtor_67bae0(u32 fl) { return deleting_dtor(this, fn::code_value(fn::Obj67bb30_dtor_dtor), fl); }  // 0x0067bb30
    void* ddtor_67bc60(u32 fl) { return deleting_dtor(this, fn::code_value(fn::X_dtor_9dc634), fl); }  // 0x0067bcb0
    void* ddtor_67bde0(u32 fl) { return deleting_dtor(this, fn::code_value(fn::Obj67b_dtor_67be30), fl); }  // 0x0067be30
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("ddtor_67ae90", 0x0067ae90, &H::ddtor_67ae90);
B103("ddtor_67b1e0", 0x0067b1e0, &H::ddtor_67b1e0);
B103("ddtor_67b360", 0x0067b360, &H::ddtor_67b360);
B103("ddtor_67b4e0", 0x0067b4e0, &H::ddtor_67b4e0);
B103("ddtor_67b660", 0x0067b660, &H::ddtor_67b660);
B103("ddtor_67b7e0", 0x0067b7e0, &H::ddtor_67b7e0);
B103("ddtor_67b960", 0x0067b960, &H::ddtor_67b960);
B103("ddtor_67bae0", 0x0067bae0, &H::ddtor_67bae0);
B103("ddtor_67bc60", 0x0067bc60, &H::ddtor_67bc60);
B103("ddtor_67bde0", 0x0067bde0, &H::ddtor_67bde0);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {


struct H {
    // ------------------------------------------------------------------ 0x0067af60
    void* ctor_67af60() {
        joan::orig::as_particle_key(this)->construct();            // 0x0067d8c0
        joan::orig::as_color_range(&jo::self<jo::RenderOp>(this)->value)->construct();  // 0x0067aff0
        jo::view<std::uintptr_t>(jo::self<jo::RenderOp>(this)->vtable) = img::addr(d::vt_Obj_9dc594);
        jo::view<u32>(jo::self<jo::RenderOp>(this)->kind_) = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("ScriptUnit::parse_640170", 0x00640170, &H::parse_unit);
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("ScriptChoice::parse_640bd0", 0x00640bd0, &H::parse_choice);
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("ScriptCond::parse_6454e0", 0x006454e0, &H::parse_cond);
B("Obj::ctor_67af60", 0x0067af60, &H::ctor_67af60);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b111_20.cpp (fix/uninit)
namespace {
namespace game_b111_20 {

struct H {
    // ------------------------------------------------------------ 0x0067aff0
    void* color_init() {
        jo::view<u32>(jo::self<jo::ScriptColor>(this)->lo) = 0;  // fix/uninit: original copies stack garbage [ebp-8]
        jo::view<u32>(jo::self<jo::ScriptColor>(this)->hi) = 0;  // fix/uninit: original copies stack garbage [ebp-0xc]
        return this;
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: colour pair stored as 0, 0 (two uninitialised stack dwords before).
F("ScriptColor::init_67aff0", 0x0067aff0, &H::color_init);
#undef F

}  // namespace game_b111_20
}  // namespace

// ---------------------------------------- from modules/game_b107_16.cpp (game/b107_16)
namespace {
namespace game_b107_16 {

struct H {
    // ------------------------------------------------------------------ 0x0067b040
    void* scalar_dtor_67b040(u32 flags) {
        joan::orig::as_particle_color_key(this)->destruct();  // 0x0067b090
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_16", name, addr, fn)
B("Obj::scalar_dtor_67b040", 0x0067b040, &H::scalar_dtor_67b040);
#undef B

}  // namespace game_b107_16
}  // namespace

// ---------------------------------------- from modules/game_b104_12.cpp (game/b104_12)
namespace {
namespace game_b104_12 {

struct H {
    void nop() {}                                                       // 0x0067b110
    std::uintptr_t flag_69() {                                                     // 0x0067c210
        return (reinterpret_cast<std::uintptr_t>(this) & 0xffffff00u) | jo::self<jo::EffectKey>(this)->follow_camera;
    }
    float get_76() { return jo::self<jo::EffectKey>(this)->orir; }                           // 0x0067c2e0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_12", name, addr, fn)
B("X::nop_67b110", 0x0067b110, &H::nop);
B("X::flag_69", 0x0067c210, &H::flag_69);
B("X::get_76", 0x0067c2e0, &H::get_76);
#undef B

}  // namespace game_b104_12
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {


struct H {
    // ------------------------------------------------------------------ SEH dtors 0x67b...
    u32 dtor_67b(img::Addr vptr) {
        jo::view<std::uintptr_t>(jo::self<jo::ParticleKey>(this)->vtable) = img::addr(vptr);
        joan::orig::as_float_range(&jo::self<jo::Gauge>(this)->value)->destruct();  // 0x0067a4e0
        return joan::orig::arg::as<u32>(joan::orig::as_particle_key(this)->destruct());       // 0x0067d970
    }
    u32 dtor_67b3b0() { return dtor_67b(d::vt_Gauge_9dc5bc); }
    u32 dtor_67b530() { return dtor_67b(d::vt_Gauge_9dc5d0); }
    u32 dtor_67be30() { return dtor_67b(d::vt_Gauge_9dc648); }

    // ------------------------------------------------------------------ 12-byte getters
    static void* copy12(void* out, const float (&src)[3]) {
        u32* const o = static_cast<u32*>(out);
        o[0] = jo::view<u32>(src[0]);
        o[1] = jo::view<u32>(src[1]);
        o[2] = jo::view<u32>(src[2]);
        return out;
    }
    void* get_vec_6a(void* out) { return copy12(out, jo::self<jo::EffectKey>(this)->orientation); }  // 0x0067c260
    void* get_vec_8b(void* out) { return copy12(out, jo::self<jo::EffectKey>(this)->self_rot_axis); }  // 0x0067c510
    void* get_vec_a8(void* out) { return copy12(out, jo::self<jo::EffectKey>(this)->rotation_center); }  // 0x0067c710

    H* set_130(u32 v) {  // 0x0067c930
        jo::self<jo::EffectKey>(this)->u32_130 = v;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("Obj67b::dtor_67b3b0", 0x0067b3b0, &H::dtor_67b3b0);
B("Obj67b::dtor_67b530", 0x0067b530, &H::dtor_67b530);
B("Obj67b::dtor_67be30", 0x0067be30, &H::dtor_67be30);
B("Obj67c::get_vec_6a", 0x0067c260, &H::get_vec_6a);
B("Obj67c::get_vec_8b", 0x0067c510, &H::get_vec_8b);
B("Obj67c::get_vec_a8", 0x0067c710, &H::get_vec_a8);
B("Obj67c::set_130", 0x0067c930, &H::set_130);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {


struct H {
    // 0x0067b730
    H* ctor_67b730() {
        joan::orig::as_particle_key(this)->construct();            // 0x0067d8c0
        joan::orig::as_range(jo::self<jo::Emitter>(this)->lo)->construct();  // 0x0067a380 (lo / hi)
        jo::view<std::uintptr_t>(jo::self<jo::Emitter>(this)->vtable) = img::addr(d::vt_Emitter);
        jo::view<u32>(jo::self<jo::Emitter>(this)->kind_) = 6;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Ctor_67b730", 0x0067b730, &H::ctor_67b730);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b93_05.cpp (game/b93_05)
namespace {
namespace game_b93_05 {

struct Pair8 {  // float members like the callers' (System V: one XMM register)
    float a, b;
};
struct Block24 {
    std::uint32_t d[6];
};
static_assert(sizeof(Pair8) == 8 && sizeof(Block24) == 24, "arg sizes");

struct H {
    // ---- EffectKey ctors
    H* key_ctor(img::Addr vtable, std::uint32_t kind) {
        joan::orig::as_particle_key(this)->construct();  // 0x0067d8c0
        joan::orig::as_float_range(&jo::self<jo::Gauge>(this)->value)->construct();  // 0x0067a4a0
        jo::view<std::uintptr_t>(jo::self<jo::ParticleKey>(this)->vtable) = img::addr(vtable);
        jo::view<std::uint32_t>(jo::self<jo::ParticleKey>(this)->kind_) = kind;
        return this;
    }
    H* ctor_67b8b0() { return key_ctor(d::vt_Gauge_9dc60c, 7); }
    H* ctor_67ba30() { return key_ctor(d::vt_Gauge_9dc620, 8); }
    H* ctor_67bbb0() { return key_ctor(d::vt_Gauge_9dc634, 9); }
    H* ctor_67bd30() { return key_ctor(d::vt_Gauge_9dc648, 0xa); }

    // 0x0067beb0 (hidden return pointer, ret 4)
    Pair8* get_1(Pair8* out) {
        out->a = jo::self<jo::EffectKey>(this)->lifetime[0];
        out->b = jo::self<jo::EffectKey>(this)->lifetime[1];

        return out;
    }

    // ---- 8-byte by-value setters (arg dtor 0x0067a4e0)
    static void set8(float (&dst)[2], Pair8& v) {
        dst[0] = v.a;
        dst[1] = v.b;
        joan::orig::as_float_range(&v)->destruct();  // 0x0067a4e0
    }
    void set_67bf00(Pair8 v) { set8(jo::self<jo::EffectKey>(this)->lifetime, v); }
    void set_67bf90(Pair8 v) { set8(jo::self<jo::EffectKey>(this)->emit_rate, v); }
    void set_67c020(Pair8 v) { set8(jo::self<jo::EffectKey>(this)->emit_rate2, v); }
    void set_67c5e0(Pair8 v) { set8(jo::self<jo::EffectKey>(this)->self_rot_speed, v); }
    void set_67c6d0(Pair8 v) { set8(jo::self<jo::EffectKey>(this)->rotation_speed, v); }

    // ---- 24-byte by-value setters (arg dtor 0x0067a3e0)
    static void set24(float (&dst)[6], Block24& v) {
        std::memcpy(dst, &v, sizeof dst);
        joan::orig::as_range(&v)->destruct();  // 0x0067a3e0
    }
    void set_67c0b0(Block24 v) { set24(jo::self<jo::EffectKey>(this)->spawn_dir, v); }
    void set_67c140(Block24 v) { set24(jo::self<jo::EffectKey>(this)->emit_radius, v); }
    void set_67c1d0(Block24 v) { set24(jo::self<jo::EffectKey>(this)->gravity, v); }

    // ---- 12-byte setters
    static void set12(float (&dst)[3], std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        jo::view<std::uint32_t>(dst[0]) = a;
        jo::view<std::uint32_t>(dst[1]) = b;
        jo::view<std::uint32_t>(dst[2]) = c;
    }
    void set_67c2a0(std::uint32_t a, std::uint32_t b, std::uint32_t c) { set12(jo::self<jo::EffectKey>(this)->orientation, a, b, c); }
    void set_67c550(std::uint32_t a, std::uint32_t b, std::uint32_t c) { set12(jo::self<jo::EffectKey>(this)->self_rot_axis, a, b, c); }
    void set_67c750(std::uint32_t a, std::uint32_t b, std::uint32_t c) { set12(jo::self<jo::EffectKey>(this)->rotation_center, a, b, c); }
    void set_67c7d0(std::uint32_t a, std::uint32_t b, std::uint32_t c) { set12(jo::self<jo::EffectKey>(this)->rotation_axis, a, b, c); }
    void set_67ca60(std::uint32_t a, std::uint32_t b, std::uint32_t c) { set12(jo::self<jo::EffectKey>(this)->offset, a, b, c); }

    // ---- dword setters
    void set_67c300(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->orir) = v; }
    void set_67c360(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->time_delay) = v; }
    void set_67c3c0(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->texture_period) = v; }
    void set_67c420(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->texture_number) = v; }
    void set_67c990(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->num_particles) = v; }
    void set_67c9f0(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::EffectKey>(this)->loop) = v; }

    // ---- byte setters
    void set_67c230(std::uint8_t v) { jo::self<jo::EffectKey>(this)->follow_camera = v; }
    void set_67c480(std::uint8_t v) { jo::self<jo::EffectKey>(this)->rchange = v; }
    void set_67c4e0(std::uint8_t v) { jo::self<jo::EffectKey>(this)->self_rotation = v; }
    void set_67c650(std::uint8_t v) { jo::self<jo::EffectKey>(this)->has_rotation = v; }
    void set_67c870(std::uint8_t v) { jo::self<jo::EffectKey>(this)->is_smo = v; }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b93_05", name, addr, fn)
GAME("EffectKey7::ctor", 0x0067b8b0, &H::ctor_67b8b0);
GAME("EffectKey8::ctor", 0x0067ba30, &H::ctor_67ba30);
GAME("EffectKey9::ctor", 0x0067bbb0, &H::ctor_67bbb0);
GAME("EffectKeyA::ctor", 0x0067bd30, &H::ctor_67bd30);
GAME("EffectKey::get_1", 0x0067beb0, &H::get_1);
GAME("EffectKey::set_1", 0x0067bf00, &H::set_67bf00);
GAME("EffectKey::set_9", 0x0067bf90, &H::set_67bf90);
GAME("EffectKey::set_11", 0x0067c020, &H::set_67c020);
GAME("EffectKey::set_19", 0x0067c0b0, &H::set_67c0b0);
GAME("EffectKey::set_31", 0x0067c140, &H::set_67c140);
GAME("EffectKey::set_49", 0x0067c1d0, &H::set_67c1d0);
GAME("EffectKey::set_69", 0x0067c230, &H::set_67c230);
GAME("EffectKey::set_6a", 0x0067c2a0, &H::set_67c2a0);
GAME("EffectKey::set_76", 0x0067c300, &H::set_67c300);
GAME("EffectKey::set_86", 0x0067c360, &H::set_67c360);
GAME("EffectKey::set_124", 0x0067c3c0, &H::set_67c3c0);
GAME("EffectKey::set_128", 0x0067c420, &H::set_67c420);
GAME("EffectKey::set_123", 0x0067c480, &H::set_67c480);
GAME("EffectKey::set_8a", 0x0067c4e0, &H::set_67c4e0);
GAME("EffectKey::set_8b", 0x0067c550, &H::set_67c550);
GAME("EffectKey::set_97", 0x0067c5e0, &H::set_67c5e0);
GAME("EffectKey::set_9f", 0x0067c650, &H::set_67c650);
GAME("EffectKey::set_a0", 0x0067c6d0, &H::set_67c6d0);
GAME("EffectKey::set_a8", 0x0067c750, &H::set_67c750);
GAME("EffectKey::set_b4", 0x0067c7d0, &H::set_67c7d0);
GAME("EffectKey::set_13c", 0x0067c870, &H::set_67c870);
GAME("EffectKey::set_134", 0x0067c990, &H::set_67c990);
GAME("EffectKey::set_138", 0x0067c9f0, &H::set_67c9f0);
GAME("EffectKey::set_7a", 0x0067ca60, &H::set_67ca60);
#undef GAME

}  // namespace game_b93_05
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

struct H {
    // ------------------------------------------------------------------------ small ones
    u32 dtor_9dc634() {  // 0x0067bcb0
        jo::view<std::uintptr_t>(jo::self<jo::Gauge>(this)->vtable) = img::addr(d::vt_Gauge_9dc634);
        joan::orig::as_float_range(&jo::self<jo::Gauge>(this)->value)->destruct();  // 0x0067a4e0
        return joan::orig::arg::as<u32>(joan::orig::as_particle_key(this)->destruct());     // 0x0067d970
    }
    float get_86() { return jo::self<jo::EffectKey>(this)->time_delay; }  // 0x0067c330
    std::uintptr_t byte_9f() {                            // 0x0067c620
        return (addr(this) & 0xffffff00u) | jo::self<jo::EffectKey>(this)->has_rotation;
    }
    std::uintptr_t set_12c(u32 v) {  // 0x0067c900 (EAX = this, pointer-wide on x64)
        jo::self<jo::EffectKey>(this)->u32_12c = v;
        return reinterpret_cast<std::uintptr_t>(this);
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::dtor_9dc634", 0x0067bcb0, &H::dtor_9dc634);
B98("X::get_86", 0x0067c330, &H::get_86);
B98("X::byte_9f", 0x0067c620, &H::byte_9f);
B98("X::set_12c", 0x0067c900, &H::set_12c);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

struct H {
    // 0x0067bdc0
    bool is_x_67bdc0() { return false; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
// Trivial functions.
B("Panel::is_x_67bdc0", 0x0067bdc0, &H::is_x_67bdc0);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

struct H {
    // ------------------------------------------------------------------ 0x0067c450
    std::uintptr_t byte_123() { return (u(this) & 0xffffff00u) | static_cast<u32>(jo::self<jo::EffectKey>(this)->rchange); }

    // ------------------------------------------------------------------ 0x0067c790
    i32* get_vec_b4(i32* out) {
        out[0] = jo::view<i32>(jo::self<jo::EffectKey>(this)->rotation_axis[0]);
        out[1] = jo::view<i32>(jo::self<jo::EffectKey>(this)->rotation_axis[1]);
        out[2] = jo::view<i32>(jo::self<jo::EffectKey>(this)->rotation_axis[2]);
        return out;
    }

    // ------------------------------------------------------------------ 0x0067c9c0
    i32 field_138() { return jo::self<jo::EffectKey>(this)->loop; }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Obj::byte_123", 0x0067c450, &H::byte_123);
B95("Obj::get_vec_b4", 0x0067c790, &H::get_vec_b4);
B95("Obj::field_138", 0x0067c9c0, &H::field_138);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::EffectState::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_05::H::ctor_6758f0>(this)); }  // 0x6758f0
auto joan::orig::Zones::dtor() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_14::H::zones_dtor>(this)); }  // 0x675930
auto joan::orig::Zone::vector_deleting_destructor(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_14::H::zone_vec_delete>(this, a0)); }  // 0x675d70
auto joan::orig::EffectState::parts_stop() -> void { joan::bind::member<&::game_fx::H::parts_stop>(this); }  // 0x677120
auto joan::orig::EffectKey::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_04::H::delete_677eb0>(this, a0)); }  // 0x677eb0
auto joan::orig::Handle::assign(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_01::H::assign>(this, a0)); }  // 0x679300
auto joan::orig::EffectState::set_name(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b86_01::H::set_name>(this, a0)); }  // 0x67a1c0
auto joan::orig::EffectState::set_error_text(const char* a0) -> void { joan::bind::member<&::game_b91_04::H::set_name_a6a28c>(this, a0); }  // 0x67a200
auto joan::orig::EffectState::parts_alloc() -> void { joan::bind::member<&::game_fx::H::parts_alloc>(this); }  // 0x67a240
auto joan::orig::Range::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_15::H::range_ctor>(this)); }  // 0x67a380
auto joan::orig::Range::destruct() -> void { joan::bind::member<&::game_b83_04::H::nothing>(this); }  // 0x67a3e0
auto joan::orig::Range::random_point(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_14::H::combine>(this, a0)); }  // 0x67a400
auto joan::orig::FloatRange::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::pair_ctor>(this)); }  // 0x67a4a0
auto joan::orig::FloatRange::destruct() -> void { joan::bind::member<&::game_b85_05::H::nop>(this); }  // 0x67a4e0
auto joan::orig::lerp3(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b99_09::lerp3>(a0, a1, a2, a3, a4, a5, a6)); }  // 0x67a500
auto joan::orig::TokenList::peek() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::holder_inner>(this)); }  // 0x67ac80
auto joan::orig::TokenList::next() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_01::H::particle_next>(this)); }  // 0x67acb0
auto joan::orig::ParticleKey::kind() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_07::H::kind>(this)); }  // 0x67ad50
auto joan::orig::ParticleKey::set_time_range(std::int32_t a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_07::H::set_pair>(this, a0, a1)); }  // 0x67ad70
auto joan::orig::ParticleKey::set_flag8(std::int8_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_08::H::set_byte_8>(this, a0)); }  // 0x67adb0
auto joan::orig::ColorRange::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_20::H::color_init>(this)); }  // 0x67aff0
auto joan::orig::ParticleColorKey::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_18::H::op_dtor>(this)); }  // 0x67b090
auto joan::orig::ColorRange::destruct() -> void { joan::bind::member<&::game_b104_12::H::nop>(this); }  // 0x67b110
auto joan::orig::EffectKey::get_1(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_05::H::get_1>(this, a0)); }  // 0x67beb0
auto joan::orig::EffectKey::set_69(std::uint8_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c230>(this, a0); }  // 0x67c230
auto joan::orig::EffectKey::set_6a(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b93_05::H::set_67c2a0>(this, a0, a1, a2); }  // 0x67c2a0
auto joan::orig::EffectKey::set_76(std::uint32_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c300>(this, a0); }  // 0x67c300
auto joan::orig::EffectKey::set_86(std::uint32_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c360>(this, a0); }  // 0x67c360
auto joan::orig::EffectKey::set_124(std::uint32_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c3c0>(this, a0); }  // 0x67c3c0
auto joan::orig::EffectKey::set_128(std::uint32_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c420>(this, a0); }  // 0x67c420
auto joan::orig::EffectKey::set_123(std::uint8_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c480>(this, a0); }  // 0x67c480
auto joan::orig::EffectKey::set_8a(std::uint8_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c4e0>(this, a0); }  // 0x67c4e0
auto joan::orig::EffectKey::set_8b(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b93_05::H::set_67c550>(this, a0, a1, a2); }  // 0x67c550
auto joan::orig::EffectKey::set_9f(std::uint8_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c650>(this, a0); }  // 0x67c650
auto joan::orig::EffectKey::set_a8(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b93_05::H::set_67c750>(this, a0, a1, a2); }  // 0x67c750
auto joan::orig::EffectKey::set_b4(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b93_05::H::set_67c7d0>(this, a0, a1, a2); }  // 0x67c7d0
auto joan::orig::EffectKey::set_13c(std::uint8_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c870>(this, a0); }  // 0x67c870
auto joan::orig::EffectKey::set_color_op(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::set_12c>(this, a0)); }  // 0x67c900
auto joan::orig::EffectKey::set_alpha_op(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_15::H::set_130>(this, a0)); }  // 0x67c930
auto joan::orig::EffectKey::set_134(std::uint32_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c990>(this, a0); }  // 0x67c990
auto joan::orig::EffectKey::set_138(std::uint32_t a0) -> void { joan::bind::member<&::game_b93_05::H::set_67c9f0>(this, a0); }  // 0x67c9f0
auto joan::orig::EffectKey::set_7a(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b93_05::H::set_67ca60>(this, a0, a1, a2); }  // 0x67ca60
