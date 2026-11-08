// game/unknown_shot.cpp: functions whose original source file is unknown (formerly game/_unplaced/shot.cpp;
// re/attribution.csv: between ww/enphysic/rbody.cpp and ww/interfac/combobox.cpp). They move to their original file
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
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
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

using joan::hook::global;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using i16 = std::int16_t;
using u16 = std::uint16_t;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

[[maybe_unused]] constexpr img::Addr kGameUnits(d::g_units);

}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

u32 bits(float v) {
    u32 r;
    std::memcpy(&r, &v, 4);
    return r;
}
// Vec3 (12 bytes), passed by pointer or by value.
struct Vec3 {
    u32 v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");


struct H {
    // ------------------------------------------------------------------------ 0x006b1d20
    i32 vec_6b1d20(void* out) {
        const float z = jo::self<jo::Shot>(this)->extent[2] / global<float>(d::flt_2);
        Vec3* const o = static_cast<Vec3*>(out);
        o->v[0] = 0;
        o->v[1] = 0;
        o->v[2] = bits(z);
        return 0;
    }

};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::vec_6b1d20", 0x006b1d20, &H::vec_6b1d20);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
namespace {
namespace game_b101_14 {

JOAN_COUNTER(c_reset, "Camera::reset_state");

struct H {
    // ------------------------------------------------------------------ 0x006b0d10
    void reset_state(u8* s) {
        c_reset.hit();
        u32 identity[16];
        std::memcpy(identity, img::obj(d::g_identity_matrix), sizeof identity);
        for (i32 i = 0; i < 3; ++i) jo::view<u32>(jo::as_body_state(s)->cell_pos[i]) = 0;
        jo::as_body_state(s)->zone = 0;
        jo::as_body_state(s)->sub = 0;
        const auto zero3 = [](void* p) {  // a 3-float vector of the BodyState = the image's zero vector (g_pos)
            u32* const v = static_cast<u32*>(p);
            v[0] = global<u32>(d::g_pos);
            v[1] = global<u32>(d::g_a69cdc);
            v[2] = global<u32>(d::g_a69ce0);
        };
        zero3(&jo::as_body_state(s)->vec0_x);
        zero3(jo::as_body_state(s)->vec_c);
        jo::view<u32>(jo::as_body_state(s)->angle_x) = 0;
        jo::view<u32>(jo::as_body_state(s)->angle_y) = 0;
        jo::view<u32>(jo::as_body_state(s)->angle_z) = 0;
        u32 quat[4];
        const u32* const q = joan::orig::arg::as<u32*>(joan::orig::as_rect(quat)->set_5bed90(0x3f800000u, 0u, 0u, 0u));  // 0x005bed90
        std::memcpy(jo::as_body_state(s)->orient, q, 16);
        jo::as_body_state(s)->cell = 0;
        jo::as_body_state(s)->on_ground = 1;
        joan::orig::as_body(this)->inertia_tensor(joan::orig::arg::mptr(jo::self<jo::Shot>(this)->inertia_scale), joan::orig::arg::mptr(identity), joan::orig::arg::mptr(jo::as_body_state(s)->inertia));  // 0x006b1970
        joan::orig::as_body(this)->inverse_inertia_tensor(joan::orig::arg::mptr(jo::self<jo::Shot>(this)->inertia_scale), joan::orig::arg::mptr(identity), joan::orig::arg::mptr(jo::as_body_state(s)->inv_inertia));  // 0x006b1bd0
        jo::as_body_state(s)->u16_134 = 0;
        zero3(jo::as_body_state(s)->move_target);
        jo::as_body_state(s)->move_start = 0;
        jo::as_body_state(s)->move_end = 0;
        jo::view<u32>(jo::as_body_state(s)->move_len) = 0;
        jo::as_body_state(s)->move_flag = 0;
        zero3(jo::as_body_state(s)->goal);
        jo::view<u32>(jo::as_body_state(s)->goal_start) = 0;
        jo::as_body_state(s)->goal_end = 0;
        jo::view<u32>(jo::as_body_state(s)->goal_len) = 0;
        jo::as_body_state(s)->goal_flag = 0;
        zero3(jo::as_body_state(s)->f32_128);
        zero3(jo::as_body_state(s)->vel);
        jo::as_body_state(s)->vel_start = 0;
        jo::as_body_state(s)->vel_end = 0;
        jo::as_body_state(s)->vel_len = 0;
        jo::as_body_state(s)->vel_flag = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("Camera::reset_state", 0x006b0d10, &H::reset_state);
#undef B

}  // namespace game_b101_14
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

struct H {
    float* scaled_vec(float* out) {  // 0x006b1030
        out[0] = jo::self<jo::Shot>(this)->state.pos_x * global<float>(d::flt_46_1);
        out[1] = jo::self<jo::Shot>(this)->state.pos_y * global<float>(d::flt_46_1);
        out[2] = jo::self<jo::Shot>(this)->state.pos_z * global<float>(d::flt_46_1);
        return out;
    }

};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6b::scaled_vec", 0x006b1030, &H::scaled_vec);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

using joan::call::import_call;

// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return import_call<float>(slot, args...);
#else
    return static_cast<float>(import_call<double>(slot, args...));
#endif
}

std::uint32_t u(std::int32_t v) { return static_cast<std::uint32_t>(v); }
using joan::orig::helpers::s;

struct V3f {
    float x, y, z;
};
static_assert(sizeof(V3f) == 12, "v3");

JOAN_COUNTER(c_inertia, "scale_inertia");

struct H {
    // 0x006b10a0
    H* mover_start(const V3f* v, std::int32_t a, std::int32_t b) {
        std::memcpy(jo::self<jo::Shot>(this)->state.vel, v, 12);
        jo::view<std::int32_t>(jo::self<jo::Shot>(this)->state.vel_start) = a;
        jo::view<float>(jo::self<jo::Shot>(this)->state.vel_len) = static_cast<float>(static_cast<double>(u(b)));  // fild qword (hi 0)
        jo::view<std::int32_t>(jo::self<jo::Shot>(this)->state.vel_end) = s(u(a) + u(b));
        jo::view<std::int16_t>(jo::self<jo::Shot>(this)->state.vel_flag) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.move_target[0]) = global<std::uint32_t>(d::g_pos);
        jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.move_target[1]) = global<std::uint32_t>(d::g_a69cdc);
        jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.move_target[2]) = global<std::uint32_t>(d::g_a69ce0);
        jo::self<jo::Shot>(this)->state.move_start = 0;
        jo::self<jo::Shot>(this)->state.move_end = 0;
        jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.move_len) = 0;
        jo::view<std::int16_t>(jo::self<jo::Shot>(this)->state.move_flag) = 0;
        return this;
    }

    // 0x006b1970
    float* scale_inertia(const V3f* a, const float* m, float* out) {
        c_inertia.hit();
        for (int i = 0; i < 16; ++i) jo::view<std::uint32_t>(out[i]) = global<std::uint32_t>(d::g_identity_matrix + i * 4);  // rep movsd
        const V3f v1{m[0] * m[0], m[1] * m[1], m[2] * m[2]};
        const V3f v2{m[4] * m[4], m[5] * m[5], m[6] * m[6]};
        const V3f v3{m[8] * m[8], m[9] * m[9], m[10] * m[10]};
        const V3f v4{m[0] * m[4], m[1] * m[5], m[2] * m[6]};
        const V3f v5{m[0] * m[8], m[1] * m[9], m[2] * m[10]};
        const V3f v6{m[4] * m[8], m[5] * m[9], m[6] * m[10]};
        out[0] = v3d_float(d::iat_v3d_87_dot_product, a, &v1);  // dot_product
        out[5] = v3d_float(d::iat_v3d_87_dot_product, a, &v2);
        out[10] = v3d_float(d::iat_v3d_87_dot_product, a, &v3);
        out[1] = v3d_float(d::iat_v3d_87_dot_product, a, &v4);
        out[2] = v3d_float(d::iat_v3d_87_dot_product, a, &v5);
        out[6] = v3d_float(d::iat_v3d_87_dot_product, a, &v6);
        std::uint32_t w;
        std::memcpy(&w, &out[1], 4);
        std::memcpy(&out[4], &w, 4);
        std::memcpy(&w, &out[2], 4);
        std::memcpy(&out[8], &w, 4);
        std::memcpy(&w, &out[6], 4);
        std::memcpy(&out[9], &w, 4);
        return out;
    }

};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Mover::start", 0x006b10a0, &H::mover_start);
B("scale_inertia", 0x006b1970, &H::scale_inertia);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_rec_init, "Rec::init");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x006b11a0
    void* rec_init(const Vec3* v, std::uint32_t a, std::uint32_t b) {
        c_rec_init.hit();
        jo::view<Vec3>(jo::self<jo::Shot>(this)->state.move_target) = *v;
        jo::self<jo::Shot>(this)->state.move_start = a;
        jo::self<jo::Shot>(this)->state.move_len = static_cast<float>(b);  // fild qword {b, 0}: unsigned
        jo::self<jo::Shot>(this)->state.move_end = a + b;
        jo::self<jo::Shot>(this)->state.move_flag = 0;
        jo::view<Vec3>(jo::self<jo::Shot>(this)->state.vel) = global<Vec3>(d::g_pos);
        jo::self<jo::Shot>(this)->state.vel_start = 0;
        jo::self<jo::Shot>(this)->state.vel_end = 0;
        jo::self<jo::Shot>(this)->state.vel_len = 0;
        jo::self<jo::Shot>(this)->state.vel_flag = 0;
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Rec::init", 0x006b11a0, &H::rec_init);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

struct H {
    void* scale_inv(const float* v, void* a, void* b) {  // 0x006b1bd0
        const float one = global<float>(d::flt_1);
        float inv[3];
        inv[0] = one / v[0];
        inv[1] = one / v[1];
        inv[2] = one / v[2];
        return joan::orig::as_body(this)->inertia_tensor(joan::orig::arg::mptr(static_cast<void*>(inv)), a, b);  // 0x006b1970
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::scale_inv", 0x006b1bd0, &H::scale_inv);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

float k_one() { return global<float>(d::flt_1); }    // 1.0f

JOAN_COUNTER(c_inertia, "Body::box_inertia");

struct H {
    // ------------------------------------------------------------------ 0x006b1c40
    float* box_inertia(float m, const float* size, float* out) {
        c_inertia.hit();
        const float k = global<float>(d::flt_46_1);  // 46.1f
        const float x = size[0] * k_one() / k;
        const float y = size[0] * k_one() / k;  // [+0] again (original)
        const float z = size[0] * k_one() / k;
        out[0] = y * y + z * z;
        out[1] = x * x + z * z;
        out[2] = x * x + y * y;
        joan::orig::vec_scale_in_place(joan::orig::arg::mptr(out), joan::orig::arg::f32(m / global<float>(d::flt_12)));  // 0x0060ce80, / 12.0f
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("Body::box_inertia", 0x006b1c40, &H::box_inertia);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b87_06.cpp (game/b87_06)
namespace {
namespace game_b87_06 {

struct H {
    // ------------------------------------------------------------ 0x006b1d80 (ret 0xc)
    void set_2a2(std::int16_t a, std::int32_t b, std::int32_t c) {
        jo::self<jo::Shot>(this)->u16_2a2 = a;
        jo::self<jo::Shot>(this)->i32_2a4 = b;
        jo::self<jo::Shot>(this)->i32_2a8 = c;
    }
};

#define B06(name, addr, fn) JOAN_REPLACE("game/b87_06", name, addr, fn)
B06("0x006b1d80 set_2a2", 0x006b1d80, &H::set_2a2);
#undef B06

}  // namespace game_b87_06
}  // namespace

// ---------------------------------------- from modules/game_players1.cpp (game/players1)
namespace {
namespace game_players1 {

struct H {
    // 0x006b1ff0 Record::`scalar deleting destructor'(flags) (ret 4).
    void* record_delete(std::uint32_t flags) {
        joan::orig::as_body(this)->destruct();  // 0x006b08c0 destructor
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define P1(name, addr, fn) JOAN_REPLACE("game/players1", name, addr, fn)
P1("Record::scalar_delete", 0x006b1ff0, &H::record_delete);
#undef P1

}  // namespace game_players1
}  // namespace

// ---------------------------------------- from modules/game_b104_13.cpp (game/b104_13)
namespace {
namespace game_b104_13 {

JOAN_COUNTER(c_add_new, "List6b2040::add_new");

using joan::orig::helpers::new_;  // operator new

struct H {
    // ------------------------------------------------------------------ 0x006b2040
    u32 add_new(void* a, jo::Actor* b, i32 c, const void* d, i32 e, i32 g) {
        c_add_new.hit();
        void* const p = new_(static_cast<std::uint32_t>(sizeof(jo::Shot)));  // 0x2b2 on x86
        void* o = p != nullptr ? joan::orig::as_body(p)->construct() : nullptr;  // 0x006b0830
        joan::orig::as_body(o)->init(a, b, joan::orig::arg::u32(c), joan::orig::arg::mptr(d), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(e))), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(g))));  // 0x006b08e0
        joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&o));  // 0x005857a0
        const u32 n = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        jo::as_shot(o)->id = n;
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_13", name, addr, fn)
B("List6b2040::add_new", 0x006b2040, &H::add_new);
#undef B

}  // namespace game_b104_13
}  // namespace

// ---------------------------------------- from modules/game_units5.cpp (game/units5)
namespace {
namespace game_units5 {

struct H {
    // 0x006b2130 (ret 4); returns 0x005858c0's eax.
    std::uint32_t remove(std::int32_t id) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_record_list(this)->get(joan::orig::arg::u32(id)));  // 0x006b0760
        if (p != nullptr) joan::orig::as_body(p)->destroy(1u);    // 0x006b1ff0 scalar deleting destructor
        return thiscall<std::uint32_t>(fn::linkout_5858c0, this, id);   // 0x005858c0
    }
};

#define U5(name, addr, fn) JOAN_REPLACE("game/units5", name, addr, fn)
U5("RecordList::remove", 0x006b2130, &H::remove);
#undef U5

}  // namespace game_units5
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("init_6b21a0", 0x006b21a0, &static_init<fn::init_a6a538>);
B("init_6b2200", 0x006b2200, &static_init<fn::init_a6a53c>);
B("init_6b2250", 0x006b2250, &static_init<fn::init_a6a540>);
B("init_6b22a0", 0x006b22a0, &static_init<fn::init_a6a544>);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

static void JOAN_CDECL init_a6a544() { global<float>(d::g_a6a544) = global<float>(d::g_a6a538) / global<float>(d::flt_4); }  // 0x006b22c0
static float pi_from_atan() {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(t * 4.0);                     // qword 0x9cf788 = 4.0
}
static void JOAN_CDECL init_a6a538() { global<float>(d::g_a6a538) = pi_from_atan(); }  // 0x006b21c0

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("init_a6a538", 0x006b21c0, &init_a6a538);
B("init_a6a544", 0x006b22c0, &init_a6a544);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

// 0x006b2220
void JOAN_CDECL init_a6a53c() { global<float>(d::g_a6a53c) = global<float>(d::g_a6a538) / global<float>(d::flt_2); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("init_a6a53c", 0x006b2220, &init_a6a53c);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

static void JOAN_CDECL init_a6a540() { global<float>(d::g_a6a540) = global<float>(d::g_a6a538) / global<float>(d::flt_3); }  // 0x006b2270

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("init_a6a540", 0x006b2270, &init_a6a540);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Camera::reset_state(void* a0) -> void { joan::bind::member<&::game_b101_14::H::reset_state>(this, a0); }  // 0x6b0d10
auto joan::orig::Body::scaled_position(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_06::H::scaled_vec>(this, a0)); }  // 0x6b1030
auto joan::orig::Body::start(void* a0, std::int32_t a1, std::int32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_06::H::mover_start>(this, a0, a1, a2)); }  // 0x6b10a0
auto joan::orig::Body::init_target(void* a0, std::uint32_t a1, std::uint32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::rec_init>(this, a0, a1, a2)); }  // 0x6b11a0
auto joan::orig::Body::inertia_tensor(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_06::H::scale_inertia>(this, a0, a1, a2)); }  // 0x6b1970
auto joan::orig::Body::inverse_inertia_tensor(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_08::H::scale_inv>(this, a0, a1, a2)); }  // 0x6b1bd0
auto joan::orig::Body::set_2a2(std::int16_t a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b87_06::H::set_2a2>(this, a0, a1, a2); }  // 0x6b1d80
auto joan::orig::Body::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_players1::H::record_delete>(this, a0)); }  // 0x6b1ff0
auto joan::orig::RecordList::add_new(void* a0, void* a1, std::int32_t a2, const void* a3, std::int32_t a4, std::int32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_13::H::add_new>(this, a0, a1, a2, a3, a4, a5)); }  // 0x6b2040
auto joan::orig::RecordList::remove(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_units5::H::remove>(this, a0)); }  // 0x6b2130
