// game/unknown_pgauge.cpp: functions whose original source file is unknown (formerly game/_unplaced/pgauge.cpp;
// re/attribution.csv: between ww/enparticle/petoken.cpp and ww/enparticle/pevent.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstddef>
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

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using u8 = std::uint8_t;

using joan::img::obj;

using joan::hook::global;
using i16 = std::int16_t;


}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

using joan::orig::helpers::memset_;

struct H {
    void* self() { return this; }

    // 0x0067d8c0
    void* ctor_67d8c0() {
        jo::self<jo::ParticleKey>(this)->time_range.construct();  // 0x0067a4a0
        jo::view<std::uintptr_t>(jo::self<jo::ParticleKey>(this)->vtable) = img::addr(d::vt_crt_9dcaa0);
        memset_(&jo::self<jo::ParticleKey>(this)->kind_, 0, static_cast<std::uint32_t>(sizeof(jo::ParticleKey) - offsetof(jo::ParticleKey, kind_)));  // +4 .. +0x1b
        jo::self<jo::ParticleKey>(this)->kind_ = -1;
        return self();
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj67d::Obj67d", 0x0067d8c0, &H::ctor_67d8c0);
#undef B91

}  // namespace game_b91_04
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
    void* ddtor_67d920(u32 fl) { return deleting_dtor(this, fn::code_value(fn::Obj9dcaa0_dtor), fl); }  // 0x0067d970
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("ddtor_67d920", 0x0067d920, &H::ddtor_67d920);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {

struct H {
    // 0x0067d970
    std::uint32_t obj_dtor() {
        jo::view<std::uintptr_t>(jo::self<jo::ParticleKey>(this)->vtable) = img::addr(d::vt_crt_9dcaa0);
        return thiscall<std::uint32_t>(fn::Obj_nop_67a4e0, &jo::self<jo::ParticleKey>(this)->time_range);  // 0x0067a4e0
    }
    // 0x0067ef70
    std::uint8_t flag8() { return jo::self<jo::ParticleKey>(this)->fade; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Obj9dcaa0::dtor", 0x0067d970, &H::obj_dtor);
B("Flag8::get", 0x0067ef70, &H::flag8);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

using joan::hook::x87::Fpu;

float k_one() { return global<float>(d::flt_1); }    // 1.0f

// Interpolated value of a key (0x005906f0 on the curve 0xa45fa8, args +0x1b / +0x1f).
float key_value(void* key) {
    return joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->random_float(joan::orig::arg::f32(jo::view<u32>(jo::as_gauge(key)->value.lo)), joan::orig::arg::f32(jo::view<u32>(jo::as_gauge(key)->value.hi))));
}
// Next key: 0x0041b3f0 on [+0x15] + 0x145 with index movsx word +0x19.
u8* next_key(void* self) {
    return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(&jo::as_particle_key(self)->owner->keys)->ptr_at(static_cast<i32>(jo::as_particle_key(self)->next)));  // 0x0041b3f0
}
bool key_cached(void* self) { return joan::orig::arg::as<u8>(joan::orig::as_particle_key(self)->flag8()) != 0; }  // 0x0067ef70

// Distance between this key and the next one (+0x11), 1.0 when 0 or NaN.
float key_span(void* self, void* next) {
    float d = jo::as_particle_key(next)->time_ - jo::as_particle_key(self)->time_;
    if (d == 0.0f || d != d) d = k_one();  // test ah,0x40 je: equal or unordered -> 1.0
    return d;
}

JOAN_COUNTER(c_lerp, "Gauge::lerp");
JOAN_COUNTER(c_lerp_byte, "Gauge::lerp_byte");

// Shared bodies of the 0x0067d9a0 / 0x0067ddd0 families.
void lerp_float(void* self, float& key, float& slope) {
    c_lerp.hit();
    if (!key_cached(self)) key = key_value(self);
    if (jo::as_particle_key(self)->next != 0) {
        u8* const next = next_key(self);
        const float c = key_value(next);
        const float d = key_span(self, next);
        slope = (c - key) / d;
    } else {
        jo::view<u32>(slope) = 0;
    }
}
void lerp_byte(void* self, u8& key, float& slope) {
    c_lerp_byte.hit();
    if (!key_cached(self)) {
        const float v = key_value(self);
        key = static_cast<u8>(joan::hook::x87::ftol(static_cast<double>(v)));  // _ftol
    }
    if (jo::as_particle_key(self)->next != 0) {
        u8* const next = next_key(self);
        const float c = key_value(next);
        const float d = key_span(self, next);
        const Fpu fpu{true};
        // fild (zero-extended byte); fsubr c; fdiv d
        const double r = fpu.div(fpu.sub(static_cast<double>(c), static_cast<double>(key)),
                                 static_cast<double>(d));
        slope = static_cast<float>(r);
    } else {
        jo::view<u32>(slope) = 0;
    }
}

struct H {
    // ------------------------------------------------------------------ 0x0067d9a0..
    void lerp_a(u8* out) { lerp_float(this, jo::as_particle(out)->scale, jo::as_particle(out)->scale_rate); }
    void lerp_b(u8* out) { lerp_float(this, jo::as_particle(out)->vel[0], jo::as_particle(out)->accel[0]); }
    void lerp_c(u8* out) { lerp_float(this, jo::as_particle(out)->vel[1], jo::as_particle(out)->accel[1]); }
    void lerp_d(u8* out) { lerp_float(this, jo::as_particle(out)->vel[2], jo::as_particle(out)->accel[2]); }
    void lerp_byte_a(u8* out) { lerp_byte(this, jo::as_particle(out)->colour[0], jo::as_particle(out)->colour_rate[0]); }
    void lerp_byte_b(u8* out) { lerp_byte(this, jo::as_particle(out)->colour[1], jo::as_particle(out)->colour_rate[1]); }
    void lerp_byte_c(u8* out) { lerp_byte(this, jo::as_particle(out)->colour[2], jo::as_particle(out)->colour_rate[2]); }
    void lerp_byte_d(u8* out) { lerp_byte(this, jo::as_particle(out)->colour[3], jo::as_particle(out)->colour_rate[3]); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("Gauge::lerp_a", 0x0067d9a0, &H::lerp_a);
B("Gauge::lerp_byte_a", 0x0067ddd0, &H::lerp_byte_a);
B("Gauge::lerp_byte_b", 0x0067dee0, &H::lerp_byte_b);
B("Gauge::lerp_byte_c", 0x0067dff0, &H::lerp_byte_c);
B("Gauge::lerp_byte_d", 0x0067e100, &H::lerp_byte_d);
B("Gauge::lerp_b", 0x0067e3f0, &H::lerp_b);
B("Gauge::lerp_c", 0x0067e4f0, &H::lerp_c);
B("Gauge::lerp_d", 0x0067e5f0, &H::lerp_d);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
namespace {
namespace game_b101_14 {

void* const kMath = obj(d::g_misc);

i32 rand_256() { return joan::orig::as_misc(kMath)->random(0x100); }  // 0x00590540
u8 rand_byte(u32 a, u32 b) { return joan::orig::arg::as<u8>(joan::orig::as_misc(kMath)->random_range(joan::orig::arg::i32(a), joan::orig::arg::i32(b))); }  // 0x00590690
float rand_float(u32 a, u32 b) { return joan::orig::arg::as<float>(joan::orig::as_misc(kMath)->random_float(joan::orig::arg::f32(a), joan::orig::arg::f32(b))); }  // 0x005906f0
u8 mix(u8 a, u8 b, i32 r) {
    return static_cast<u8>((static_cast<u32>(a) * static_cast<u32>(r) +
                            static_cast<u32>(b) * static_cast<u32>(0x100 - r)) >> 8);
}
// x87 "test ah,0x40 je": keep d unless it compares equal to 0 or unordered.
float nonzero_or_one(float d) {
    if (!(d < 0.0f) && !(d > 0.0f)) return 1.0f;
    return d;
}

JOAN_COUNTER(c_init_colour, "Emitter::init_colour");
JOAN_COUNTER(c_init_size, "Emitter::init_size");

struct H {
    // ------------------------------------------------------------------ 0x0067daa0
    void init_colour(u8* out) {
        c_init_colour.hit();
        if (joan::orig::arg::as<u8>(joan::orig::as_particle_key(this)->flag8()) == 0) {  // 0x0067ef70
            const i32 r = rand_256();
            jo::as_particle(out)->colour[0] = mix(jo::self<jo::RenderOp>(this)->value.lo[0], jo::self<jo::RenderOp>(this)->value.hi[0], r);
            jo::as_particle(out)->colour[1] = mix(jo::self<jo::RenderOp>(this)->value.lo[1], jo::self<jo::RenderOp>(this)->value.hi[1], r);
            jo::as_particle(out)->colour[2] = mix(jo::self<jo::RenderOp>(this)->value.lo[2], jo::self<jo::RenderOp>(this)->value.hi[2], r);
            jo::as_particle(out)->colour[3] = rand_byte(jo::self<jo::RenderOp>(this)->value.lo[3], jo::self<jo::RenderOp>(this)->value.hi[3]);
        }
        if (static_cast<i32>(jo::self<jo::RenderOp>(this)->next) == 0) {
            jo::view<u32>(jo::as_particle(out)->colour_rate[0]) = 0;
            jo::view<u32>(jo::as_particle(out)->colour_rate[1]) = 0;
            jo::view<u32>(jo::as_particle(out)->colour_rate[2]) = 0;
            jo::view<u32>(jo::as_particle(out)->colour_rate[3]) = 0;
            return;
        }
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(&jo::self<jo::RenderOp>(this)->owner->keys)->ptr_at(static_cast<i32>(jo::self<jo::RenderOp>(this)->next)));  // 0x0041b3f0
        const i32 r = rand_256();
        const u8 c0 = mix(jo::as_render_op(e)->value.lo[0], jo::as_render_op(e)->value.hi[0], r);
        const u8 c1 = mix(jo::as_render_op(e)->value.lo[1], jo::as_render_op(e)->value.hi[1], r);
        const u8 c2 = mix(jo::as_render_op(e)->value.lo[2], jo::as_render_op(e)->value.hi[2], r);
        const u8 c3 = rand_byte(jo::as_render_op(e)->value.lo[3], jo::as_render_op(e)->value.hi[3]);
        const float d = nonzero_or_one(jo::as_particle_key(e)->time_ - jo::self<jo::RenderOp>(this)->time_);
        jo::as_particle(out)->colour_rate[0] = static_cast<float>(static_cast<i32>(c0) - static_cast<i32>(jo::as_particle(out)->colour[0])) / d;
        jo::as_particle(out)->colour_rate[1] = static_cast<float>(static_cast<i32>(c1) - static_cast<i32>(jo::as_particle(out)->colour[1])) / d;
        jo::as_particle(out)->colour_rate[2] = static_cast<float>(static_cast<i32>(c2) - static_cast<i32>(jo::as_particle(out)->colour[2])) / d;
        jo::as_particle(out)->colour_rate[3] = static_cast<float>(static_cast<i32>(c3) - static_cast<i32>(jo::as_particle(out)->colour[3])) / d;
    }

    // ------------------------------------------------------------------ 0x0067e210
    void init_size(u8* out) {
        c_init_size.hit();
        if (joan::orig::arg::as<u8>(joan::orig::as_particle_key(this)->flag8()) == 0) {  // 0x0067ef70
            jo::as_particle(out)->vel[0] = rand_float(jo::view<u32>(jo::self<jo::Emitter>(this)->lo[0]), jo::view<u32>(jo::self<jo::Emitter>(this)->hi[0]));
            jo::as_particle(out)->vel[1] = rand_float(jo::view<u32>(jo::self<jo::Emitter>(this)->lo[1]), jo::view<u32>(jo::self<jo::Emitter>(this)->hi[1]));
            jo::as_particle(out)->vel[2] = rand_float(jo::view<u32>(jo::self<jo::Emitter>(this)->lo[2]), jo::view<u32>(jo::self<jo::Emitter>(this)->hi[2]));
        }
        if (static_cast<i32>(jo::self<jo::Emitter>(this)->next) == 0) {
            jo::view<u32>(jo::as_particle(out)->accel[0]) = 0;
            jo::view<u32>(jo::as_particle(out)->accel[1]) = 0;
            jo::view<u32>(jo::as_particle(out)->accel[2]) = 0;
            return;
        }
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(&jo::self<jo::Emitter>(this)->owner->keys)->ptr_at(static_cast<i32>(jo::self<jo::Emitter>(this)->next)));  // 0x0041b3f0
        const float s0 = rand_float(jo::view<u32>(jo::as_emitter(e)->lo[0]), jo::view<u32>(jo::as_emitter(e)->hi[0]));
        const float s1 = rand_float(jo::view<u32>(jo::as_emitter(e)->lo[1]), jo::view<u32>(jo::as_emitter(e)->hi[1]));
        const float s2 = rand_float(jo::view<u32>(jo::as_emitter(e)->lo[2]), jo::view<u32>(jo::as_emitter(e)->hi[2]));
        const float d = nonzero_or_one(jo::as_particle_key(e)->time_ - jo::self<jo::Emitter>(this)->time_);
        jo::as_particle(out)->accel[0] = (s0 - jo::as_particle(out)->vel[0]) / d;
        jo::as_particle(out)->accel[1] = (s1 - jo::as_particle(out)->vel[1]) / d;
        jo::as_particle(out)->accel[2] = (s2 - jo::as_particle(out)->vel[2]) / d;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("Emitter::init_colour", 0x0067daa0, &H::init_colour);
B("Emitter::init_size", 0x0067e210, &H::init_size);
#undef B

}  // namespace game_b101_14
}  // namespace

// ---------------------------------------- from modules/game_b111_03.cpp (game/b111_03)
namespace {
namespace game_b111_03 {

struct H {

    // 0x0067e6f0: out+0x10 = Random::range(this+0x1b, this+0x1f) (0x005906f0, float st0, PC24-rounded).
    void eval_timer(u8* out) {
        const u32 lo = jo::view<u32>(jo::self<jo::Gauge>(this)->value.lo);
        const u32 hi = jo::view<u32>(jo::self<jo::Gauge>(this)->value.hi);
        jo::as_particle(out)->time = joan::orig::arg::as<float>(joan::orig::as_misc(img::obj(d::g_misc))->random_float(joan::orig::arg::f32(lo), joan::orig::arg::f32(hi)));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_03", name, addr, fn)
B("Gauge::eval_timer", 0x0067e6f0, &H::eval_timer);
#undef B

}  // namespace game_b111_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ParticleKey::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::ctor_67d8c0>(this)); }  // 0x67d8c0
auto joan::orig::ParticleKey::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06::H::obj_dtor>(this)); }  // 0x67d970
auto joan::orig::ParticleKey::flag8() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b90_06::H::flag8>(this)); }  // 0x67ef70
