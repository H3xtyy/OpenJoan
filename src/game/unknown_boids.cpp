// game/unknown_boids.cpp: functions whose original source file is unknown (formerly game/_unplaced/boids.cpp;
// re/attribution.csv: between ww/battle.cpp and ww/camman.cpp). They move to their original file when evidence names it
// (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::img::obj;

using joan::hook::global;

using i32 = std::int32_t;

using joan::call::import_call;

using i16 = std::int16_t;
using u8 = std::uint8_t;

void* const kKinds = obj(d::g_kinds);

using u16 = std::uint16_t;

i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

using u32 = std::uint32_t;

struct RV3 {  // 12-byte vector copied as raw dwords
    u32 x, y, z;
};

using joan::call::cdecl_call;

struct Vec3 {
    float x, y, z;
};

using i8 = std::int8_t;

// The neighbour list of the flocking code: the original keeps 15 Boid* in the image at 0x00a33650 (60 bytes of
// 4-byte slots; the image layout has it as u8[72], which cannot hold 15 pointers of 8 bytes). It is scratch state:
// reset_group() empties it and pick_nearest() refills it before group_dir() reads it, so a static array of ours holds
// it on every target (nothing else reads 0x00a33650).
jo::Boid* g_neighbours[15];

}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

static_assert(sizeof(RV3) == 12, "RV3");

JOAN_COUNTER(c_group_dir, "Particle::group_dir");

using joan::orig::helpers::memset_;

struct H {

    H* ctor_4203e0() {
        jo::self<jo::Boid>(this)->vtable = obj(d::vt_Obj_9cf828);  // vtable (cleared again by the memset, as in the original)
        memset_(this, 0, static_cast<u32>(sizeof(jo::Boid)));  // 0x7f bytes on x86
        return this;
    }

    // ------------------------------------------------------------------ 0x00421c50
    RV3* group_dir(RV3* out) {
        c_group_dir.hit();
        u8* const k = joan::orig::arg::as<u8*>(joan::orig::as_list(kKinds)->at_word(joan::orig::arg::i16(sx16(jo::view<u16>(jo::self<jo::Boid>(this)->kind)))));  // 0x004231f0
        float acc[3] = {0.0f, 0.0f, 0.0f};                                  // ebp-0x14
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Boid>(this)->neighbour_count); ++i) {
            u8* const p = reinterpret_cast<u8*>(g_neighbours[i]);  // image g_a33650 on x86
            if (p != nullptr) joan::orig::vec_add_in_place(static_cast<void*>(acc), static_cast<void*>(jo::as_boid(p)->pos));  // 0x0060cdc0
        }
        acc[0] = acc[0] / static_cast<float>(static_cast<i32>(jo::self<jo::Boid>(this)->neighbour_count));
        acc[1] = acc[1] / static_cast<float>(static_cast<i32>(jo::self<jo::Boid>(this)->neighbour_count));
        acc[2] = acc[2] / static_cast<float>(static_cast<i32>(jo::self<jo::Boid>(this)->neighbour_count));
        RV3 tmp;  // ebp-0x3c
        RV3 v = *joan::orig::arg::as<RV3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(acc), static_cast<void*>(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos))));  // 0x0060cc90
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&v));                  // normalize in place
        joan::orig::vec_scale_in_place(static_cast<void*>(&v), joan::orig::arg::f32(jo::view<u32>(jo::as_flock(k)->max_urgency)));  // 0x0060ce80 (scale)
        out->x = v.x;
        out->y = v.y;
        out->z = v.z;
        return out;
    }

    // ------------------------------------------------------------------ 0x00422c30 / 0x00423e70
    void reset_group() {
        for (i32 i = 0; i < 0xf; ++i) g_neighbours[i] = nullptr;  // image g_a33650 on x86
        jo::view<u16>(jo::self<jo::Boid>(this)->neighbour_count) = 0;
        jo::self<jo::Boid>(this)->nearest = nullptr;
        jo::view<u32>(jo::self<jo::Boid>(this)->nearest_dist) = 0x497423f0;  // 999999.0f
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::ctor_4203e0", 0x004203e0, &H::ctor_4203e0);
B99("Particle::group_dir", 0x00421c50, &H::group_dir);
B99("Particle::reset_group", 0x00422c30, &H::reset_group);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

void* const kRandom = obj(d::g_misc);

JOAN_COUNTER(c_particle_init, "Particle::init");
JOAN_COUNTER(c_relink, "Link::relink");

struct H {

    // ------------------------------------------------------------------ 0x00420420
    H* particle_init(i32 kind, i32 b) {
        c_particle_init.hit();
        jo::view<u16>(jo::self<jo::Boid>(this)->kind) = static_cast<u16>(kind);
        jo::self<jo::Boid>(this)->u16_a = static_cast<u16>(b);
        u8* k = joan::orig::arg::as<u8*>(joan::orig::as_list(kKinds)->at_word(joan::orig::arg::i16(sx16(jo::view<u16>(jo::self<jo::Boid>(this)->kind)))));  // 0x004231f0
        jo::view<u32>(jo::self<jo::Boid>(this)->radius) = jo::view<u32>(jo::as_flock(k)->def_range);
        k = joan::orig::arg::as<u8*>(joan::orig::as_list(kKinds)->at_word(joan::orig::arg::i16(sx16(jo::view<u16>(jo::self<jo::Boid>(this)->kind)))));  // 0x004231f0
        u32 box[6];                                                 // ebp-0x20
        std::memcpy(box, jo::as_flock(k)->box, sizeof(box));
        float bf[6];
        std::memcpy(bf, box, sizeof(bf));
        volatile float dead_x = joan::orig::arg::as<float>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(bf[3] - bf[0]))));  // fabs, ebp-0x24
        volatile float dead_z = joan::orig::arg::as<float>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(bf[5] - bf[2]))));  // fabs, ebp-0x28
        (void)dead_x;
        (void)dead_z;
        jo::self<jo::Boid>(this)->pos[0] = joan::orig::arg::as<float>(joan::orig::as_misc(kRandom)->random_float(joan::orig::arg::f32(box[0]), joan::orig::arg::f32(box[3])));  // 0x005906f0
        jo::self<jo::Boid>(this)->pos[1] = joan::orig::arg::as<float>(joan::orig::as_misc(kRandom)->random_float(joan::orig::arg::f32(box[1]), joan::orig::arg::f32(box[4])));
        jo::self<jo::Boid>(this)->pos[2] = joan::orig::arg::as<float>(joan::orig::as_misc(kRandom)->random_float(joan::orig::arg::f32(box[2]), joan::orig::arg::f32(box[5])));
        jo::self<jo::Boid>(this)->vel[0] = joan::orig::arg::as<float>(joan::orig::as_misc(kRandom)->random_float(joan::orig::arg::f32(0x3dcccccdu), joan::orig::arg::f32(0x40400000u)));  // 0.1f, 3.0f
        jo::self<jo::Boid>(this)->vel[1] = joan::orig::arg::as<float>(joan::orig::as_misc(kRandom)->random_float(joan::orig::arg::f32(0x3dcccccdu), joan::orig::arg::f32(0x40400000u)));
        if (joan::orig::arg::as<double>(joan::orig::as_misc(kRandom)->rand_unit()) > static_cast<double>(global<float>(d::flt_0_5))) {  // 0x00590630 > 0.5f
            jo::self<jo::Boid>(this)->vel[0] = jo::self<jo::Boid>(this)->vel[0] * global<float>(d::flt_m1);                              // * -1.0f
        }
        if (joan::orig::arg::as<double>(joan::orig::as_misc(kRandom)->rand_unit()) > static_cast<double>(global<float>(d::flt_0_5))) {
            jo::self<jo::Boid>(this)->vel[1] = jo::self<jo::Boid>(this)->vel[1] * global<float>(d::flt_m1);
        }
        jo::self<jo::Boid>(this)->speed = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<void*>(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel))));  // length
        jo::view<u32>(jo::self<jo::Boid>(this)->dir[2]) = 0;
        jo::view<u32>(jo::self<jo::Boid>(this)->dir[1]) = 0;
        jo::view<u32>(jo::self<jo::Boid>(this)->dir[0]) = 0;
        jo::view<u16>(jo::self<jo::Boid>(this)->neighbour_count) = 0;
        jo::self<jo::Boid>(this)->nearest = nullptr;
        jo::view<u32>(jo::self<jo::Boid>(this)->nearest_dist) = 0x497423f0;  // 999999.0f
        jo::self<jo::Boid>(this)->u16_3c = 0;
        jo::self<jo::Boid>(this)->u32_42 = 0;
        jo::self<jo::Boid>(this)->u32_4a = 0x497423f0;
        jo::self<jo::Boid>(this)->prev = nullptr;
        jo::self<jo::Boid>(this)->next_ = nullptr;
        jo::view<u8>(jo::self<jo::Boid>(this)->avoiding) = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00421b60
    u32 reset_three() {
        joan::orig::as_input(obj(d::g_mouse))->poll();                // 0x00594f90
        joan::orig::as_sound_stream(obj(d::g_snd))->update();                // 0x0059b1c0
        joan::orig::as_ambient_streams(obj(d::g_mus))->update();    // 0x005866d0
        return 0;  // void callee; callers discard the result
    }

    // ------------------------------------------------------------------ 0x00423030
    u32 relink() {
        c_relink.hit();
        if (jo::self<jo::Boid>(this)->next_ == nullptr && jo::self<jo::Boid>(this)->prev == nullptr) {
            joan::orig::as_boid(this)->set_next(nullptr);               // 0x00422f70
            joan::orig::as_boid(this)->set_prev(nullptr);   // 0x00422fc0
            return 0;  // void callees; callers discard the result
        }
        if (jo::self<jo::Boid>(this)->next_ == nullptr) {
            joan::orig::as_boid(jo::self<jo::Boid>(this)->prev)->set_next(nullptr);  // 0x00422f70
            return 0;
        }
        if (jo::self<jo::Boid>(this)->prev == nullptr) {
            joan::orig::as_boid(jo::self<jo::Boid>(this)->next_)->set_prev(nullptr);  // 0x00422fc0
            return 0;
        }
        joan::orig::as_boid(jo::self<jo::Boid>(this)->prev)->set_next(jo::self<jo::Boid>(this)->next_);
        joan::orig::as_boid(jo::self<jo::Boid>(this)->next_)->set_prev(jo::self<jo::Boid>(this)->prev);  // 0x00422fc0
        return 0;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Particle::init", 0x00420420, &H::particle_init);
B98("Game::reset_three", 0x00421b60, &H::reset_three);
B98("Link::relink", 0x00423030, &H::relink);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

using joan::orig::helpers::at;

static_assert(sizeof(Vec3) == 12, "Vec3");

void widget_dtor(void* w) { joan::orig::as_widget(w)->dtor_6211a0(); }  // 0x006211a0

JOAN_COUNTER(c_pick_nearest, "Cam::pick_nearest");

// ---- static initialisers / free functions ----
using joan::orig::helpers::div_init;
void JOAN_CDECL init_422a00() { div_init(d::g_a3a29c, d::g_pi_a3a294, d::flt_3); }

struct H {
    // 0x00420620
    void dtor_420620() {
        joan::orig::as_panel(this)->reset_6f60d0();  // 0x006f60d0
        // `this` is the AddScreen at 0x00a56398 (destroyed at exit): its two buttons, +0x368 / +0x22 on x86
        widget_dtor(&jo::self<jo::AddScreen>(this)->cb_button);
        widget_dtor(&jo::self<jo::AddScreen>(this)->add_button);
    }

    // 0x004219b0
    Vec3* target_dir(Vec3* out) {
        void* const entry = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_kinds))->at_word(joan::orig::arg::i16(jo::self<jo::Boid>(this)->kind)));  // 0x004231f0
        Vec3 v;  // ebp-0x14
        std::memcpy(&v, jo::self<jo::Boid>(this)->nearest->vel, 12);
        import_call<Vec3*>(d::iat_v3d_87_vector_normalize_1, &v);                                 // normalize in place
        joan::orig::vec_scale_in_place(&v, joan::orig::arg::f32(jo::as_flock(entry)->max_urgency));         // 0x0060ce80
        *out = v;
        return out;
    }

    // 0x00421ba0
    std::int32_t pick_nearest(void* first) {
        c_pick_nearest.hit();
        void* node = first;  // ebp-0xc
        joan::orig::as_boid(this)->reset_neighbours();  // 0x00422c30
        while (node != nullptr) {
            const float d = joan::orig::arg::as<float>(joan::orig::as_unit(this)->distance_to_422ca0(joan::orig::arg::mptr(node)));  // 0x00422ca0 (fld dword)
            const float big = global<float>(d::flt_999999);
            // fcomp, test ah,0x40 jne: skip when equal or unordered.
            if (d < big || d > big) {
                joan::orig::as_boid(this)->push_neighbour(joan::orig::as_boid(node));  // 0x00422bd0
                if (!(d >= jo::self<jo::Boid>(this)->nearest_dist)) {      // test ah,1 je: d < +0x46 or NaN
                    jo::self<jo::Boid>(this)->nearest_dist = d;
                    jo::view<void*>(jo::self<jo::Boid>(this)->nearest) = node;
                }
            }
            node = joan::orig::arg::as<void*>(joan::orig::as_boid(node)->next());  // 0x00422fa0
        }
        return jo::self<jo::Boid>(this)->neighbour_count;
    }

    // 0x00423190
    void dtor_423190() {
        jo::self<jo::Boid>(this)->vtable = obj(d::vt_Obj_9cf828);
        joan::orig::as_boid(this)->release_actor();  // 0x004207d0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("Obj::dtor_420620", 0x00420620, &H::dtor_420620);
B("Cam::target_dir", 0x004219b0, &H::target_dir);
B("Cam::pick_nearest", 0x00421ba0, &H::pick_nearest);
B("init_422a00", 0x00422a00, &init_422a00);
B("Obj::dtor_423190", 0x00423190, &H::dtor_423190);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

using joan::hook::x87::Fpu;

using joan::orig::helpers::addr;

constexpr Fpu kFpu{true};

void copy3(void* dst, const void* src) { std::memcpy(dst, src, 12); }

JOAN_COUNTER(c_init_420690, "Shot::init_420690");
JOAN_COUNTER(c_aim_422d80, "Shot::aim_422d80");

struct H {
    // ---------------------------------------------------------------- 0x00420690
    std::uintptr_t init_420690(u32 type, u32 sub, const u32* pos, const u32* vel, const u32* dir) {
        c_init_420690.hit();
        jo::view<u16>(jo::self<jo::Boid>(this)->kind) = static_cast<u16>(type);
        jo::self<jo::Boid>(this)->u16_a = static_cast<u16>(sub);
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_kinds))->at_word(joan::orig::arg::i16(jo::self<jo::Boid>(this)->kind)));  // 0x004231f0
        jo::view<u32>(jo::self<jo::Boid>(this)->radius) = jo::view<u32>(jo::as_flock(rec)->def_range);
        copy3(jo::self<jo::Boid>(this)->pos, pos);
        copy3(jo::self<jo::Boid>(this)->vel, vel);
        copy3(jo::self<jo::Boid>(this)->dir, dir);
        jo::self<jo::Boid>(this)->speed = static_cast<float>(import_call<double>(d::iat_v3d_87_length, vel));  // length
        jo::view<u16>(jo::self<jo::Boid>(this)->neighbour_count) = 0;
        jo::self<jo::Boid>(this)->nearest = nullptr;
        jo::view<u32>(jo::self<jo::Boid>(this)->nearest_dist) = 0x497423f0u;
        jo::self<jo::Boid>(this)->u16_3c = 0;
        jo::self<jo::Boid>(this)->u32_42 = 0;
        jo::self<jo::Boid>(this)->u32_4a = 0x497423f0u;
        jo::self<jo::Boid>(this)->prev = nullptr;
        jo::self<jo::Boid>(this)->next_ = nullptr;
        jo::view<u8>(jo::self<jo::Boid>(this)->avoiding) = 0;
        return addr(this);
    }

    // ---------------------------------------------------------------- 0x00422d80
    void aim_422d80() {
        c_aim_422d80.hit();
        u8* const vel = reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel);
        float t44[3], v38[3], v1c[3], v28[3], t5c[3], v50[3];
        copy3(v38, joan::orig::arg::as<float*>(joan::orig::vec_sub(joan::orig::arg::mptr(t44), joan::orig::arg::ptr(vel), joan::orig::arg::ptr(jo::self<jo::Boid>(this)->prev_vel))));  // 0x0060cc90 a - b
        import_call<void*>(d::iat_v3d_87_cross_product, v1c, vel, v38);                          // cross_product
        import_call<void*>(d::iat_v3d_87_cross_product, v28, v1c, vel);
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, v28);                                    // vector_normalize
        copy3(v50, joan::orig::arg::as<float*>(joan::orig::vec_sub(joan::orig::arg::mptr(t5c), joan::orig::arg::ptr(vel), joan::orig::arg::ptr(jo::self<jo::Boid>(this)->prev_vel))));
        const float d = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, v50, v28));  // dot_product
        u32 pitch_bits;
        if (!(d < 0.0f) && !(d > 0.0f)) {  // test ah,0x40: equal or unordered
            pitch_bits = 0;
        } else {
            u32 g_bits[2] = {0x40000000u, 0x40239d01u};
            double g;
            std::memcpy(&g, g_bits, 8);
            const double a = joan::orig::arg::as<double>(joan::orig::libc::atan2(joan::orig::arg::f64(g), joan::orig::arg::f64(static_cast<double>(d))));  // atan2 (64-bit st0)
            const float pitch = static_cast<float>(kFpu.add(-a, static_cast<double>(global<float>(d::flt_1_570795))));
            std::memcpy(&pitch_bits, &pitch, 4);
        }
        const double z = static_cast<double>(jo::self<jo::Boid>(this)->vel[2]);
        const float sq = jo::self<jo::Boid>(this)->vel[1] * jo::self<jo::Boid>(this)->vel[1] + jo::self<jo::Boid>(this)->vel[0] * jo::self<jo::Boid>(this)->vel[0];
        const double root = joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sq))));  // sqrt
        const double q = kFpu.div(z, root);                                          // fdivr
        const float elev = -joan::orig::arg::as<float>(joan::orig::libc::atan(joan::orig::arg::f64(q)));                         // atan, fchs
        const float yaw = joan::orig::arg::as<float>(joan::orig::libc::atan2(joan::orig::arg::f64(static_cast<double>(jo::self<jo::Boid>(this)->vel[0])), joan::orig::arg::f64(static_cast<double>(jo::self<jo::Boid>(this)->vel[1]))));  // atan2
        jo::self<jo::Boid>(this)->dir[0] = elev;
        jo::view<u32>(jo::self<jo::Boid>(this)->dir[1]) = pitch_bits;
        jo::self<jo::Boid>(this)->dir[2] = yaw;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Shot::init_420690", 0x00420690, &H::init_420690);
B("Shot::aim_422d80", 0x00422d80, &H::aim_422d80);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

using joan::orig::helpers::game_delete;                                    // operator delete

struct H {
    // scalar deleting destructors
    void* sdd_420790(std::uint32_t flags) {
        joan::orig::as_player(this)->dtor();  // 0x00687180
        if (flags & 1) game_delete(this);
        return this;
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("sdd_00420790", 0x00420790, &H::sdd_420790);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_distance, "Unit::distance_to");

struct H {
    // ------------------------------------------------------------ 0x004207d0
    void holder_release() {
        if (jo::view<void*>(jo::self<jo::Boid>(this)->actor) == nullptr) return;
        void* const p = jo::view<void*>(jo::self<jo::Boid>(this)->actor);
        if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840 (scalar deleting dtor)
        jo::view<void*>(jo::self<jo::Boid>(this)->actor) = nullptr;
    }

    // ------------------------------------------------------------ 0x00422ca0 (ret 4)
    float distance_to(std::uint8_t* other) {
        c_distance.hit();
        std::uint8_t* const type =
            joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(obj(d::g_kinds))->at_word(joan::orig::arg::i16(static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::self<jo::Boid>(this)->kind)))));  // 0x004231f0
        if (reinterpret_cast<std::uint8_t*>(this) == other) return global<float>(d::flt_999999);  // 999999
        Vec3 tmp;
        Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::self<jo::Boid>(this)->pos), joan::orig::arg::ptr(jo::self<jo::Boid>(other)->pos)));  // 0x0060cc90
        const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &d));  // length
        if (jo::as_flock(type)->use_truth != 0) return len;
        if (jo::self<jo::Boid>(this)->radius > len) return len;  // test ah,0x41 jne -> <= / unordered gives 999999
        return global<float>(d::flt_999999);
    }

    // ------------------------------------------------------------ small setters / getters
    void set_66(jo::Boid* v) { jo::self<jo::Boid>(this)->next_ = v; }  // 0x00422f70
    void set_6a(jo::Boid* v) { jo::self<jo::Boid>(this)->prev = v; }  // 0x00422fc0
    std::int32_t count_57() { return joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->groups))->size(); }  // 0x004231c0 -> 0x00417ad0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Holder::release", 0x004207d0, &H::holder_release);
B("Unit::distance_to", 0x00422ca0, &H::distance_to);
B("Obj::set_66", 0x00422f70, &H::set_66);
B("Obj::set_6a", 0x00422fc0, &H::set_6a);
B("Obj::count_57", 0x004231c0, &H::count_57);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    H* delete_420840(std::uint32_t flags) {                                              // 0x00420840
        joan::orig::as_scene_object(this)->destruct();  // 0x004230f0
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }
    void dtor_4230f0() { joan::orig::as_actor(this)->release_5aaf10(); }                                  // 0x004230f0 -> 0x005aaf10
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("delete_420840", 0x00420840, &H::delete_420840);
W1("dtor_4230f0", 0x004230f0, &H::dtor_4230f0);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b107_18.cpp (game/b107_18)
namespace {
namespace game_b107_18 {

// 12-byte vector (raw bits, no float conversions when copied / passed by value).
struct V3 {
    u32 a, b, c;
};
static_assert(sizeof(V3) == 12, "V3");
struct S24 {
    u32 d[6];
};
static_assert(sizeof(S24) == 24, "S24");

V3 v3_at(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof(v));
    return v;
}
void v3_put(void* p, const V3& v) { std::memcpy(p, &v, sizeof(v)); }
float fbits(u32 v) {
    float f;
    std::memcpy(&f, &v, 4);
    return f;
}

double v3d_length(const void* v) { return import_call<double>(d::iat_v3d_87_length, v); }
void v3d_normalize(void* v) { import_call<void*>(d::iat_v3d_87_vector_normalize_1, v); }

JOAN_COUNTER(c_proj_update, "Projectile::update_420890");

struct H {

    // ---- 0x00420890 ----
    // accumulate one force vector: 0x00422b90(&acc, v) (result length st0 discarded)
    void add_force(u32* acc, std::uintptr_t getter) {
        V3 tmp{0, 0, 0};
        const V3 v = *thiscall<const V3*>(getter, this, &tmp);
        // by value: Vec::add_length (below, b94_01) takes three floats (XMM registers on x64), so pass floats, not u32
        struct FloatV3 {
            float x, y, z;
        } fv;
        static_assert(sizeof(FloatV3) == sizeof(V3));
        std::memcpy(&fv, &v, sizeof fv);
        joan::orig::as_boid(this)->add_length(joan::orig::arg::mptr(acc), fv);  // 0x00422b90
    }

    void update_420890(u32 arg1, jo::Boid* arg2) {
        c_proj_update.hit();
        void* const desc = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_kinds))->at_word(joan::orig::arg::i16(jo::self<jo::Boid>(this)->kind)));  // 0x004231f0
        u32 acc[3] = {0, 0, 0};
        v3_put(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->prev_pos), v3_at(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos)));
        joan::orig::vec_add_in_place(joan::orig::arg::mptr(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos)), joan::orig::arg::ptr(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel)));  // 0x0060cdc0
        joan::orig::as_boid(this)->pick_nearest(arg2);                                                   // 0x00421ba0
        if (jo::self<jo::Boid>(this)->neighbour_count != 0 && jo::self<jo::Boid>(this)->avoiding == 0) {
            add_force(acc, fn::code_value(fn::Obj_push_dir));  // 0x00421810
            add_force(acc, fn::code_value(fn::Cam_target_dir));  // 0x004219b0
            add_force(acc, fn::code_value(fn::Particle_group_dir));  // 0x00421c50
        }
        if (jo::as_flock(desc)->react_enemy != 0 && jo::self<jo::Boid>(this)->avoiding == 0) {
            joan::orig::as_group(this)->count_near_421a50(joan::orig::arg::i32(arg1));  // 0x00421a50
            add_force(acc, fn::code_value(fn::Unit_offset_to_target));        // 0x00421730
        }
        if (jo::self<jo::Boid>(this)->avoiding == 0) add_force(acc, fn::code_value(fn::Obj_wander_dir));  // 0x00421510

        // fcomp [desc+0x1b], test ah,0x41 / jne: body when length > limit (not NaN)
        if (v3d_length(acc) > static_cast<double>(jo::as_flock(desc)->speed_urgency)) {
            v3d_normalize(acc);
            joan::orig::vec_scale_in_place(static_cast<void*>(acc), joan::orig::arg::f32(jo::view<u32>(jo::as_flock(desc)->speed_urgency)));  // 0x0060ce80
        }
        v3_put(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->prev_vel), v3_at(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel)));
        joan::orig::vec_add_in_place(joan::orig::arg::mptr(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel)), static_cast<void*>(acc));  // 0x0060cdc0
        jo::self<jo::Boid>(this)->vel[2] = jo::self<jo::Boid>(this)->vel[2] * jo::as_flock(desc)->max_urgency2;
        jo::self<jo::Boid>(this)->speed = static_cast<float>(v3d_length(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel)));
        if (jo::self<jo::Boid>(this)->speed > jo::as_flock(desc)->max_speed) {
            v3d_normalize(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel));
            joan::orig::vec_scale_in_place(static_cast<void*>(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel)), joan::orig::arg::f32(jo::view<u32>(jo::as_flock(desc)->max_speed)));  // 0x0060ce80
            jo::view<u32>(jo::self<jo::Boid>(this)->speed) = jo::view<u32>(jo::as_flock(desc)->max_speed);
        }
        if (jo::self<jo::Boid>(this)->avoiding == 0) {
            const u8 r = joan::orig::arg::as<u8>(joan::orig::as_boid(this)->hits_box());  // 0x00421dd0
            if (r != 0) {
                S24 m;
                std::memcpy(&m, jo::as_flock(desc)->box, sizeof(m));
                V3 t0{0, 0, 0};
                const V3 d = *joan::orig::arg::as<const V3*>(joan::orig::as_box(&m)->centre(&t0));  // 0x0060d3a0
                V3 t1{0, 0, 0};
                const V3 s = *joan::orig::arg::as<const V3*>(joan::orig::vec_sub(&t1, &d, joan::orig::arg::ptr(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos))));  // 0x0060cc90
                v3_put(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->avoid_dir), s);
                jo::view<u8>(jo::self<jo::Boid>(this)->avoiding) = 1;
            }
        }
        V3 ta{0, 0, 0};
        const V3 ahead = *joan::orig::arg::as<const V3*>(joan::orig::vec_scale(&ta, joan::orig::arg::ptr(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel)), joan::orig::arg::f32(0x459c4000u)));  // 0x0060cd00 (*5000)
        V3 tb{0, 0, 0};
        const V3 target = *joan::orig::arg::as<const V3*>(joan::orig::vec_add(&tb, joan::orig::arg::ptr(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos)), &ahead));  // 0x0060cc20
        const V3 pos = v3_at(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos));
        V3 tc{0, 0, 0};
        const V3 dir = *joan::orig::arg::as<const V3*>(joan::orig::vec_sub(&tc, &pos, &target));  // 0x0060cc90

        u32 angle = 0;                      // ByteAngle (1 byte used)
        joan::orig::as_angle(&angle)->construct();  // 0x00576120 (empty ctor)
        const float rad = joan::orig::arg::as<float>(joan::orig::libc::atan2_f(joan::orig::arg::f32(dir.b), joan::orig::arg::f32(dir.a)));  // 0x00421020 atan2 wrapper
        joan::orig::as_angle(&angle)->set(joan::orig::arg::f32(rad));                                // 0x00576240
        joan::orig::as_byte_angle(&angle)->octant();                                 // 0x00576370 (result dead)
        const float heading = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->radians());        // 0x00576280
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::Boid>(this)->actor))->set_position_5ac310(joan::orig::arg::mptr(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos)));              // 0x005ac310
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Boid>(this)->actor))->set_yaw(joan::orig::arg::bits(heading));                     // 0x005ac6a0

        if (jo::self<jo::Boid>(this)->avoiding != 0) {
            jo::self<jo::Boid>(this)->speed = static_cast<float>(v3d_length(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->avoid_dir)));
            if (jo::self<jo::Boid>(this)->speed > fbits(0x3f800000u)) {  // [0x9cf784] 1.0
                v3d_normalize(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->avoid_dir));
                jo::view<u32>(jo::self<jo::Boid>(this)->speed) = 0x3f800000u;
            }
            V3 t2{0, 0, 0};
            const V3 a = *joan::orig::arg::as<const V3*>(joan::orig::vec_scale(&t2, joan::orig::arg::ptr(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->avoid_dir)), joan::orig::arg::f32(jo::view<u32>(jo::self<jo::Boid>(this)->avoid_blend))));  // 0x0060cd00
            const float rest = global<float>(d::flt_1) - jo::self<jo::Boid>(this)->avoid_blend;
            V3 t3{0, 0, 0};
            const V3 b = *joan::orig::arg::as<const V3*>(joan::orig::vec_scale(&t3, joan::orig::arg::ptr(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel)), joan::orig::arg::f32(rest)));  // 0x0060cd00
            V3 t4{0, 0, 0};
            const V3 sum = *joan::orig::arg::as<const V3*>(joan::orig::vec_add(&t4, &b, &a));  // 0x0060cc20
            v3_put(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->vel), sum);
            // fcomp [0x9cf784], test ah,1 / jne: add when < 1.0 or NaN
            if (!(jo::self<jo::Boid>(this)->avoid_blend >= global<float>(d::flt_1))) {
                jo::self<jo::Boid>(this)->avoid_blend = jo::self<jo::Boid>(this)->avoid_blend + global<float>(d::flt_0_05);  // 0.05
                jo::view<u8>(jo::self<jo::Boid>(this)->avoiding) = 1;
            } else {
                jo::view<u32>(jo::self<jo::Boid>(this)->avoid_blend) = 0;
                jo::view<u8>(jo::self<jo::Boid>(this)->avoiding) = 0;
            }
        } else {
            jo::view<u8>(jo::self<jo::Boid>(this)->avoiding) = 0;
        }
    }

    // ---- 0x007069f0 ----
    struct Vtx {
        float x, y, z;
        u32 nx, ny, nz;
        u8 col[4];
        u32 u, v;
    };
    static_assert(sizeof(Vtx) == 0x24, "Vtx");
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_18", name, addr, fn)
B("Projectile::update_420890", 0x00420890, &H::update_420890);
#undef B

}  // namespace game_b107_18
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using joan::img::str;

template <class R = void, class... A>
R import_stdcall(img::Addr slot, A... args) {  // Win32 import through its IAT slot
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

JOAN_COUNTER(c_winmain, "WinMain");

// ---- 0x00420ea0 WinMain ------------------------------------------------------------------
i32 JOAN_STDCALL win_main(void* instance, void* /*prev*/, char* /*cmd*/, i32 /*show*/) {
    c_winmain.hit();
    if (joan::orig::arg::as<i32>(joan::orig::check_v3d_module_447150()) != 0) return 0;  // direct call
    if (import_call<i32>(d::iat_v3d_87_vertexf_check_cpu) == 0) {         // v3d vertexf_check_cpu
        import_stdcall<i32>(d::iat_USER32_MessageBoxA, static_cast<void*>(nullptr), str(d::s_V3d_87_invalid), str(d::s_Wars_Warriors_Joan_of_Arc),
                            0u);  // MessageBoxA "V3d_87 invalid"
        return 0;
    }
    void* const config = obj(d::g_a57fb8);
    joan::orig::as_config(config)->init_nop();                                    // 0x00459e70
    if (joan::orig::arg::as<i32>(joan::orig::as_config(config)->load(str(d::s_CONFIG_DAT))) == 0) {  // 0x0045aaf0 "CONFIG.DAT"
        global<u8>(d::g_a5d871) = 1;
        joan::orig::as_profiler(obj(d::g_cpu_usage))->measure_speed();  // 0x0045c8c0
        joan::orig::as_config(config)->reset();           // 0x00459e30
    }
    u32 memory_status[8] = {};  // MEMORYSTATUS at [ebp-0x20], never read
    import_stdcall<void>(d::iat_KERNEL32_GlobalMemoryStatus, static_cast<void*>(memory_status));  // GlobalMemoryStatus
    if (joan::orig::arg::as<i32>(joan::orig::as_game(obj(d::g_game_a50088))->init(instance)) == 0) return 0;  // 0x004d6c50
    com_call<u32>(global<void*>(d::g_game_device), 4);  // GetAvailableTextureMem, result unused
    joan::orig::as_error_log(obj(d::g_err))->set_callback(fn::code_value(fn::error_callback_nop));               // 0x0056dd30
    joan::orig::as_error_log(obj(d::g_err))->set_idle_callbacks(fn::code_value(fn::game_tick_4189a0), fn::code_value(fn::Game_tick_418bc0));  // 0x004171b0
    joan::orig::as_misc(obj(d::g_misc))->randomize_5904e0();                            // 0x005904e0
    joan::orig::as_misc(obj(d::g_a50490))->randomize_5904e0();                            // 0x005904e0
    joan::orig::as_config(config)->check_caps();                                     // 0x0045b240
    if (static_cast<i8>(global<u8>(d::g_cfg_shadow_quality)) == 2) {
        joan::orig::as_render_target(obj(d::g_sys3d))->create_depth(0x400u, 0x400u);  // 0x006032d0
    }
    joan::orig::as_texture(obj(d::g_sys3d))->create_603400(0x200u, 0x100u);  // 0x00603400
    joan::orig::as_game(obj(d::g_game_state))->reset_479720();                // 0x00479720
    joan::orig::as_screen(obj(d::g_game_a50088))->close_4d6ce0();                // 0x004d6ce0
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("WinMain", 0x00420ea0, &win_main);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

void* list_get(const void* list, std::int32_t i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

struct H {
    std::int32_t is_missing(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }  // 0x00456ca0 / 0x004217e0
    void* get(std::int32_t i) { return list_get(this, i); }                                // 0x00456c70 / 0x00420ff0
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("List4217e0::is_missing", 0x004217e0, &H::is_missing);
B85("List420ff0::get", 0x00420ff0, &H::get);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b104_01.cpp (game/b104_01)
namespace {
namespace game_b104_01 {

using joan::orig::helpers::fbits;

struct FVec {
    float x, y, z;
};
static_assert(sizeof(FVec) == 12, "by-value 12-byte argument");

JOAN_COUNTER(c_orient_421060, "Obj::orient_421060");

void set_yaw(void* o, float a) { joan::orig::as_scene_object(o)->set_yaw(joan::orig::arg::bits(a)); }    // 0x005ac6a0
void set_roll(void* o, float a) { joan::orig::as_scene_object(o)->set_pitch(joan::orig::arg::bits(a)); }   // 0x005ac810
bool nz(float v) { return v < 0.0f || v > 0.0f; }                       // fcomp / test ah,0x40 jne -> skip
double angle_between(FVec a, FVec v) {
    const float dot = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, &a, &v));  // dot_product
    const float la = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &a));       // length
    const float q = dot / la;
    const float lv = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &v));
    const float q2 = q / lv;
    return joan::orig::arg::as<double>(joan::orig::libc::acos(joan::orig::arg::f64(static_cast<double>(q2))));  // acos (CRT), stored as qword
}

struct H {
    // ------------------------------------------------------------ 0x00421060
    void orient_421060() {
        c_orient_421060.hit();
        FVec v10 = jo::view<FVec>(jo::self<jo::Boid>(this)->vel);
        FVec v1c = jo::view<FVec>(jo::self<jo::Boid>(this)->vel);
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, &v10);  // vector_normalize (in place)
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, &v1c);
        const float x = v1c.x, y = v1c.y, z = v1c.z;
        if (x > 0.0f) {
            set_yaw(jo::view<void*>(jo::self<jo::Boid>(this)->actor), 0.0f);
        } else if (!(x >= 0.0f)) {
            set_yaw(jo::view<void*>(jo::self<jo::Boid>(this)->actor), fbits(0x40490fdb));
        }
        if (y > 0.0f) {
            set_yaw(jo::view<void*>(jo::self<jo::Boid>(this)->actor), fbits(0x3fc90fdb));
        } else if (!(y >= 0.0f)) {
            set_yaw(jo::view<void*>(jo::self<jo::Boid>(this)->actor), fbits(0xbfc90fdb));
        }
        if (z > 0.0f) {
            set_roll(jo::view<void*>(jo::self<jo::Boid>(this)->actor), fbits(0xbfc90fdb));
        } else if (!(z >= 0.0f)) {
            set_roll(jo::view<void*>(jo::self<jo::Boid>(this)->actor), fbits(0x3fc90fdb));
        }
        if (nz(x) && nz(y)) {
            const FVec a{fbits(0x3f800000), 0.0f, 0.0f};
            const FVec v{x, y, 0.0f};
            const double r = angle_between(a, v);
            if (!(y >= 0.0f)) {
                set_yaw(jo::view<void*>(jo::self<jo::Boid>(this)->actor), static_cast<float>(-r));
            } else {
                set_yaw(jo::view<void*>(jo::self<jo::Boid>(this)->actor), static_cast<float>(r));
            }
        }
        if (nz(x) && nz(z)) {
            const FVec a{x, 0.0f, 0.0f};
            const FVec v{x, 0.0f, z};
            const double r = angle_between(a, v);
            if (z > 0.0f) {
                set_roll(jo::view<void*>(jo::self<jo::Boid>(this)->actor), static_cast<float>(-r));
            } else {
                set_roll(jo::view<void*>(jo::self<jo::Boid>(this)->actor), static_cast<float>(r));
            }
            return;
        }
        if (nz(y) && nz(z)) {
            const FVec a{0.0f, y, 0.0f};
            const FVec v{0.0f, y, z};
            const double r = angle_between(a, v);
            if (z > 0.0f) {
                set_roll(jo::view<void*>(jo::self<jo::Boid>(this)->actor), static_cast<float>(-r));
            } else {
                set_roll(jo::view<void*>(jo::self<jo::Boid>(this)->actor), static_cast<float>(r));
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_01", name, addr, fn)
B("Obj::orient_421060", 0x00421060, &H::orient_421060);
#undef B

}  // namespace game_b104_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

struct H {
    // 0x00421420
    void panel_dtor() {
        joan::orig::as_screen(this)->shutdown_6e6d20();  // 0x006e6d20
        // x86: four Buttons of 0x346 bytes at +0, +0x346, +0x68c, +0x9d2 (the object is the Glory screen, bss 0xa40470)
        jo::Glory* const self = jo::self<jo::Glory>(this);
        joan::orig::as_widget(&self->button3)->dtor_6211a0();  // 0x006211a0
        joan::orig::as_widget(&self->button2)->dtor_6211a0();
        joan::orig::as_widget(&self->button1)->dtor_6211a0();
        joan::orig::as_widget(&self->button0)->dtor_6211a0();
    }

    // 0x00422b90
    double add_length(V3* v, V3 d) {
        joan::orig::vec_add_in_place(joan::orig::arg::mptr(v), &d);  // 0x0060cdc0
        const double r = import_call<double>(d::iat_v3d_87_length, v);  // length
        return r;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Panel::~Panel", 0x00421420, &H::panel_dtor);
B94("Vec::add_length", 0x00422b90, &H::add_length);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
namespace {
namespace game_b86_05 {

struct H {
    // ---- 0x004214b0 destructor: eax = 0x0054b460's.
    std::uint32_t dtor_4214b0() {
        joan::orig::as_world(this)->cleanup();                              // 0x00440320
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_bmp_res(&jo::self<jo::World>(this)->bitmaps)->free_thunk());  // 0x0054b460
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("Obj4214b0::dtor", 0x004214b0, &H::dtor_4214b0);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kRand = obj(d::g_misc);

JOAN_COUNTER(c_wander, "Obj::wander_dir");

void JOAN_CDECL init_a3a298() { global<float>(d::g_a3a298) = global<float>(d::g_pi_a3a294) / global<float>(d::flt_2); }
void JOAN_CDECL init_a3a074() { global<float>(d::g_a3a074) = global<float>(d::g_pi_a3a06c) / global<float>(d::flt_3); }

struct H {
    // ------------------------------------------------------------------ 0x00421510
    V3* wander_dir(V3* out) {
        c_wander.hit();
        u8* const kind = joan::orig::arg::as<u8*>(joan::orig::as_list(kKinds)->at_word(joan::orig::arg::i16(jo::self<jo::Boid>(this)->kind)));  // 0x004231f0
        V3 v = jo::view<V3>(jo::self<jo::Boid>(this)->vel);
        const float d = (jo::self<jo::Boid>(this)->speed - jo::as_flock(kind)->half_speed) / jo::as_flock(kind)->max_speed;
        float s = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(d)))));  // fabs
        if (!(s >= jo::as_flock(kind)->max_urgency)) s = jo::as_flock(kind)->max_urgency;  // < or NaN
        if (s > jo::as_flock(kind)->max_urgency2) s = jo::as_flock(kind)->max_urgency2;
        const float r = joan::orig::arg::as<float>(joan::orig::as_misc(kRand)->rand_unit());  // 0x00590630
        const float sign = !(d >= global<float>(d::k_9cf5c4)) ? -1.0f : 1.0f;  // < 0 or NaN -> -1
        if (!(r >= global<float>(d::flt_0_45))) {
            v.x = sign * jo::as_flock(kind)->max_urgency + v.x;
        } else if (!(r >= global<float>(d::flt_0_9))) {
            v.y = sign * jo::as_flock(kind)->max_urgency + v.y;
        } else {
            v.z = sign * jo::as_flock(kind)->max_urgency + v.z;
        }
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&v));  // normalize in place
        const float sign2 = d > global<float>(d::k_9cf5c4) ? -1.0f : 1.0f;
        joan::orig::vec_scale_in_place(static_cast<void*>(&v), joan::orig::arg::f32(sign2 * s));  // 0x0060ce80
        *out = v;
        return out;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::wander_dir", 0x00421510, &H::wander_dir);
B96("init_a3a298", 0x00421d90, &init_a3a298);
B96("init_a3a074", 0x004230c0, &init_a3a074);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units {

constexpr img::Addr kGameUnits(d::g_units);

struct H {
    void* leader() {  // 0x004216d0: itself when it leads (+0x33), else its leader (+0x37)
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->leads()) != 0) return this;  // 0x0044a660
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(img::obj(kGameUnits))->empty_at_b(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->leader_id))) != 0) return nullptr;
        return joan::orig::arg::as<void*>(joan::orig::as_list(img::obj(kGameUnits))->get_b(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->leader_id)));
    }
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("Unit::leader", 0x004216d0, &H::leader);
#undef U1

}  // namespace game_units
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

static_assert(sizeof(RV3) == 12, "RV3");

JOAN_COUNTER(c_offset_to_target, "Unit::offset_to_target");

RV3 vsub(const void* a, const void* b) {  // 0x0060cc90 (a - b), result copied out of the temp
    RV3 tmp;
    return *joan::orig::arg::as<RV3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}

struct H {

    // ------------------------------------------------------------------ 0x00421730
    RV3* offset_to_target(RV3* out) {
        c_offset_to_target.hit();
        u8* const kind = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_kinds))->at_word(joan::orig::arg::i16(sx16(jo::view<u16>(jo::self<jo::Boid>(this)->kind)))));  // 0x004231f0
        RV3 r = {0, 0, 0};
        if (!(jo::view<float>(jo::self<jo::Boid>(this)->u32_4a) >= jo::as_flock(kind)->keep_away)) {  // a < b or unordered
            r = vsub(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos), reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->u32_42->pos));
        }
        *out = r;
        return out;
    }

    // ------------------------------------------------------------------ 0x00422bd0
    void push_a33650(jo::Boid* v) {
        if (static_cast<i32>(jo::self<jo::Boid>(this)->neighbour_count) < 0xf) {
            g_neighbours[static_cast<i32>(jo::self<jo::Boid>(this)->neighbour_count)] = v;  // image g_a33650 on x86
            jo::view<u16>(jo::self<jo::Boid>(this)->neighbour_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Boid>(this)->neighbour_count) + 1u);
        }
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Unit::offset_to_target", 0x00421730, &H::offset_to_target);
B97("Obj::push_a33650", 0x00422bd0, &H::push_a33650);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_push_dir, "Obj::push_dir");
JOAN_COUNTER(c_seg_hit, "Obj::segment_hit_in_range");

struct H {

    // ------------------------------------------------------------------ 0x00421810
    V3* push_dir(V3* out) {
        c_push_dir.hit();
        u8* const kind = joan::orig::arg::as<u8*>(joan::orig::as_list(kKinds)->at_word(joan::orig::arg::i16(jo::self<jo::Boid>(this)->kind)));  // 0x004231f0
        float t = jo::self<jo::Boid>(this)->nearest_dist / jo::as_flock(kind)->separate;
        V3 tmp;
        const V3* const d = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->nearest->pos)), static_cast<void*>(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos))));  // 0x0060cc90
        V3 v = *d;
        if (!(t >= jo::as_flock(kind)->max_urgency)) t = jo::as_flock(kind)->max_urgency;  // < or NaN
        if (t > jo::as_flock(kind)->max_urgency2) t = jo::as_flock(kind)->max_urgency2;
        if (!(jo::self<jo::Boid>(this)->nearest_dist >= jo::as_flock(kind)->separate)) {  // < or NaN
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&v));  // normalize in place
            joan::orig::vec_scale_in_place(static_cast<void*>(&v), joan::orig::arg::f32(-t));  // 0x0060ce80
        } else if (jo::self<jo::Boid>(this)->nearest_dist > jo::as_flock(kind)->separate) {
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&v));
            joan::orig::vec_scale_in_place(static_cast<void*>(&v), joan::orig::arg::f32(t));
        } else {
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&v));
            joan::orig::vec_scale_in_place(static_cast<void*>(&v), 0.0f);
        }
        *out = v;
        return out;
    }

    // ------------------------------------------------------------------ 0x00422a40
    bool segment_hit_in_range(float* t, void* seg, void* verts, void* u, void* v) {
        c_seg_hit.hit();
        if (joan::orig::arg::as<i32>(joan::orig::ray_triangle(joan::orig::arg::mptr(t), joan::orig::arg::mptr(seg), joan::orig::arg::mptr(verts), joan::orig::arg::mptr(u), joan::orig::arg::mptr(v))) == 0) return false;  // 0x0060e810
        if (!(*t >= global<float>(d::k_9cf5c4))) return false;  // < 0 or NaN
        if (*t > global<float>(d::flt_1)) return false;      // > 1
        V3 at_hit;
        joan::orig::segment_point_at(static_cast<void*>(&at_hit), joan::orig::arg::ptr(seg), joan::orig::arg::f32(*t));  // 0x0060de40
        V3 tmp;
        const V3* const d = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(&at_hit), static_cast<void*>(reinterpret_cast<u8*>(jo::self<jo::Boid>(this)->pos))));  // 0x0060cc90
        V3 diff = *d;
        const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&diff)));  // fstp float
        u8* const kind = joan::orig::arg::as<u8*>(joan::orig::as_list(kKinds)->at_word(joan::orig::arg::i16(jo::self<jo::Boid>(this)->kind)));  // 0x004231f0
        const float limit = (jo::self<jo::Flock>(kind)->box[3] - jo::self<jo::Flock>(kind)->box[0]) / global<float>(d::flt_6);
        if (len > limit) return false;
        return true;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::push_dir", 0x00421810, &H::push_dir);
B95("Obj::segment_hit_in_range", 0x00422a40, &H::segment_hit_in_range);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {

std::uint8_t* world() { return global<std::uint8_t*>(d::g_world); }

struct H {
    void attach_a() {  // 0x00421960
        if (static_cast<std::int8_t>(jo::self<jo::UnitDraw>(this)->label_attached) != 0) return;
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_) == nullptr) return;
        joan::orig::as_scene(jo::as_world(world())->scene)->register_material(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_)));  // 0x005eddc0
        jo::self<jo::UnitDraw>(this)->label_attached = 1;
    }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Unit::attach_a", 0x00421960, &H::attach_a);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b106_16.cpp (game/b106_16)
namespace {
namespace game_b106_16 {

float K(img::Addr a) { return global<float>(a); }    // float constant in .rdata

JOAN_COUNTER(c_count_421a50, "Group::count_near_421a50");

struct H {
    // 0x00421a50
    i32 count_near_421a50(i32 skip) {
        c_count_421a50.hit();
        jo::self<jo::Boid>(this)->u16_3c = 0;
        jo::self<jo::Boid>(this)->u32_42 = 0;
        jo::self<jo::Boid>(this)->u32_4a = 0x497423f0u;  // 999999.0f
        for (i32 i = 1;; ++i) {
            void* g = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_kinds))->at_word(joan::orig::arg::i16(jo::self<jo::Boid>(this)->kind)));  // 0x004231f0
            const i32 n = joan::orig::arg::as<i16>(joan::orig::as_flock(g)->member_count());                           // 0x004231c0
            if (i > n) break;
            if (i == skip) continue;
            g = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_kinds))->at_word(joan::orig::arg::i16(jo::self<jo::Boid>(this)->kind)));  // 0x004231f0
            void* const list = joan::orig::arg::as<void*>(joan::orig::as_list(g)->at_46e590(joan::orig::arg::i16(static_cast<i16>(i))));  // 0x0046e590
            for (void* m = joan::orig::arg::as<void*>(joan::orig::as_flock_group(list)->first_member()); m != nullptr;          // 0x0046dd50
                 m = joan::orig::arg::as<void*>(joan::orig::as_boid(m)->next())) {                               // 0x00422fa0
                const float d = joan::orig::arg::as<float>(joan::orig::as_unit(this)->distance_to_422ca0(joan::orig::arg::mptr(m)));                 // 0x00422ca0
                if (d < K(d::flt_999999) || d > K(d::flt_999999)) {  // not equal, not unordered
                    jo::self<jo::Boid>(this)->u16_3c = static_cast<u16>(jo::self<jo::Boid>(this)->u16_3c + 1);
                    if (!(d >= jo::view<float>(jo::self<jo::Boid>(this)->u32_4a))) {  // < or unordered
                        jo::view<float>(jo::self<jo::Boid>(this)->u32_4a) = d;
                        jo::view<void*>(jo::self<jo::Boid>(this)->u32_42) = m;
                    }
                }
            }
        }
        return jo::view<i16>(jo::self<jo::Boid>(this)->u16_3c);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_16", name, addr, fn)
B("Group::count_near_421a50", 0x00421a50, &H::count_near_421a50);
#undef B

}  // namespace game_b106_16
}  // namespace

// ---------------------------------------- from modules/game_b106_09.cpp (game/b106_09)
namespace {
namespace game_b106_09 {
using joan::orig::helpers::as_float;

JOAN_COUNTER(c_hits_box, "Mover::hits_box_421dd0");

struct H {
    // ------------------------------------------------------------------------ 0x00421dd0
    u32 hits_box() {
        c_hits_box.hit();
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_kinds))->at_word(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::self<jo::Boid>(this)->kind)))));  // 0x004231f0
        u32 b[6];  // [ebp-0x28]: min x y z, max x y z
        std::memcpy(b, jo::self<jo::Flock>(rec)->box, sizeof b);
        // Wrap one coordinate of the position that lies outside the box.
        for (int k = 0; k < 3; ++k) {
            const float p = jo::self<jo::Boid>(this)->pos[k];
            if (p > as_float(b[3 + k])) {
                jo::view<u32>(jo::self<jo::Boid>(this)->pos[k]) = b[k];
                break;
            }
            if (!(jo::self<jo::Boid>(this)->pos[k] >= as_float(b[k]))) {
                jo::view<u32>(jo::self<jo::Boid>(this)->pos[k]) = b[3 + k];
                break;
            }
        }
        // Segment: pos, pos + dir * 5000.
        u32 tmp11c[3], tmp128[3], v110[3];
        const u32* r = joan::orig::arg::as<u32*>(joan::orig::vec_scale(joan::orig::arg::mptr(tmp11c), jo::self<jo::Boid>(this)->vel, joan::orig::arg::f32(u32{0x459c4000})));  // 0x0060cd00
        std::memcpy(v110, r, sizeof v110);
        r = joan::orig::arg::as<u32*>(joan::orig::vec_add(joan::orig::arg::mptr(tmp128), jo::self<jo::Boid>(this)->pos, joan::orig::arg::ptr(v110)));  // 0x0060cc20
        u32 seg[6];  // [ebp-0x64]
        std::memcpy(seg + 3, r, 3 * sizeof(u32));
        std::memcpy(seg, jo::self<jo::Boid>(this)->pos, 3 * sizeof(u32));
        // Corners: bit 0 = max x, bit 1 = max y, bit 2 = max z.
        u32 corner[8][3];
        for (int c = 0; c < 8; ++c) {
            corner[c][0] = (c & 1) ? b[3] : b[0];
            corner[c][1] = (c & 2) ? b[4] : b[1];
            corner[c][2] = (c & 4) ? b[5] : b[2];
        }
        static const u8 kTris[12][3] = {
            {0, 1, 2}, {1, 3, 2}, {0, 5, 1}, {0, 4, 5}, {1, 5, 3}, {3, 5, 7},
            {4, 6, 5}, {5, 6, 7}, {0, 2, 4}, {2, 6, 4}, {2, 3, 6}, {3, 7, 6},
        };
        u32 out68 = 0, out6c = 0, out70 = 0;  // always written by 0x0060e810 before being read
        u32 tri[9];                           // [ebp-0x94]
        u32 res = 0;
        for (int t = 0; t < 12; ++t) {
            for (int v = 0; v < 3; ++v) std::memcpy(tri + v * 3, corner[kTris[t][v]], 3 * sizeof(u32));
            // bool result: the only caller (0x00421dd0's facade user) reads the low byte.
            res = joan::orig::arg::as<u32>(joan::orig::as_boid(this)->segment_hit_in_range(&out68, seg, tri, &out6c, &out70));  // 0x00422a40
            if (static_cast<u8>(res) != 0) return res;
        }
        return res;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_09", name, addr, fn)
B("Mover::hits_box_421dd0", 0x00421dd0, &H::hits_box);
#undef B

}  // namespace game_b106_09
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
JOAN_COUNTER(c_activate, "Unit::activate_4227c0");

struct H {

    // ------------------------------------------------------------------ 0x004227c0
    std::uintptr_t activate_4227c0() {
        c_activate.hit();
        jo::self<jo::MapObject>(this)->refresh_depth = static_cast<u8>(jo::self<jo::MapObject>(this)->refresh_depth + 1);
        if (jo::view<i8>(jo::self<jo::MapObject>(this)->active) != 0) {
            const u8 v = static_cast<u8>(jo::self<jo::MapObject>(this)->refresh_depth - 1);
            jo::self<jo::MapObject>(this)->refresh_depth = v;
            return (reinterpret_cast<std::uintptr_t>(this) & ~std::uintptr_t{0xff}) | v;
        }
        jo::self<jo::MapObject>(this)->active = 1;
        virtual_call<u32>(this, 0x10);
        virtual_call<u32>(this, 0x7c);
        void* a = jo::self<jo::MapObject>(this)->cell;
        void* b = jo::self<jo::MapObject>(this)->scene;
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->world) != nullptr) joan::orig::as_map(jo::view<void*>(jo::self<jo::MapObject>(this)->world))->find_cell(&a, &b, joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->pos));  // 0x0054a2c0
        if (b != nullptr) {
            jo::self<jo::MapObject>(this)->scene = static_cast<jo::Scene*>(b);
            if (jo::self<jo::MapObject>(this)->actor != nullptr) joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->scene))->attach(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor));  // 0x005efc50
            if (jo::self<jo::MapObject>(this)->actor_b != nullptr) joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->scene))->attach(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor_b));
            for (i32 i = 0; i < 2; ++i) {
                if (joan::orig::as_dynarray(obj(d::g_particle_effect_array))->empty_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i])) != 0) continue;  // 0x0041bd90
                u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_particle_effect_array))->ptr_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i])));  // 0x0041bd60
                void* const v = joan::orig::arg::as<void*>(joan::orig::as_effect_state(&jo::as_model_obj(rec)->fx)->object());  // 0x0041b280 (ModelObj::fx is at +0x2b on x86)
                joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->scene))->attach(joan::orig::arg::mptr(v));
            }
        }
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::MapObject>(this)->type_id))));  // 0x00425ce0
        if (jo::view<i16>(jo::as_item_def(type)->need_collision) != 0 && jo::view<u32>(jo::self<jo::MapObject>(this)->actor->record) == 0) {
            u32 r;
            if (jo::view<i16>(jo::as_item_def(type)->class_id) == 4)
                r = joan::orig::arg::as<u32>(joan::orig::as_army(obj(d::g_physic_sys))->fire_shot(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor), joan::orig::arg::u32(u32{0x4b189680}), joan::orig::arg::u32(u32{0x14}), joan::orig::arg::u32(u32{0}), joan::orig::arg::u32(u32{2}), joan::orig::arg::u32(u32{0})));  // 0x006adbf0
            else
                r = joan::orig::arg::as<u32>(joan::orig::as_army(obj(d::g_physic_sys))->fire_shot(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor), joan::orig::arg::u32(u32{0x42700000}), joan::orig::arg::u32(u32{0}), joan::orig::arg::u32(u32{0}), joan::orig::arg::u32(u32{2}), joan::orig::arg::u32(u32{0})));
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->record) = r;
        }
        virtual_call<u32>(this, 0x2c);
        jo::self<jo::MapObject>(this)->refresh_depth = static_cast<u8>(jo::self<jo::MapObject>(this)->refresh_depth - 1);
        return reinterpret_cast<std::uintptr_t>(this);
    }
};

JOAN_REPLACE("game/b108_07", "Unit::activate_4227c0", 0x004227c0, &H::activate_4227c0);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

struct H {
    // 0x00422b50
    H* ctor_1f6() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Place));  // memset (0x1f6 bytes on x86)
        return this;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj1f6::Obj1f6", 0x00422b50, &H::ctor_1f6);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b87_05.cpp (game/b87_05)
namespace {
namespace game_b87_05 {

struct H {
    // ------------------------------------------------------------ 0x00422f10
    std::uint32_t squad_clear() {
        joan::orig::as_path(&jo::self<jo::Unit>(this)->path)->clear();  // 0x004eeca0
        jo::self<jo::Unit>(this)->path_index = 0;
        // eax = the second 0x004eeca0 result (only ecx / edx are used afterwards).
        const std::uint32_t eax = joan::orig::arg::as<std::uint32_t>(joan::orig::as_path(&jo::self<jo::Unit>(this)->path_b)->clear());
        jo::self<jo::Unit>(this)->i32_3ea = 0;
        jo::self<jo::Unit>(this)->i32_3f2 = 0;
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_05", name, addr, fn)
B("Squad::clear", 0x00422f10, &H::squad_clear);
#undef B

}  // namespace game_b87_05
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00422fa0
    jo::Boid* get_66() { return jo::self<jo::Boid>(this)->next_; }

    // 0x004231f0
    void* at_word(std::int16_t index) {
        return joan::orig::as_dynarray(self())->ptr_at(static_cast<std::int32_t>(index));  // 0x0041b3f0
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::get_66", 0x00422fa0, &H::get_66);
M("List::at_word", 0x004231f0, &H::at_word);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

struct H {
    // Boid accessors (no callers left; the neighbouring 0x422xxx-0x4231xx functions are Boid's)
    void* sub_422ff0() { return jo::self<jo::Boid>(this)->dir; }
    void* sub_423010() { return jo::self<jo::Boid>(this)->pos; }
    void* self_423120() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("Obj::sub_422ff0", 0x00422ff0, &H::sub_422ff0);
B("Obj::sub_423010", 0x00423010, &H::sub_423010);
B("Obj::self_423120", 0x00423120, &H::self_423120);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using joan::orig::helpers::op_delete;

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    void* scalar_dtor_423140(u32 flags) {
        joan::orig::as_boid(this)->destruct();  // 0x00423190
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::scalar_dtor_423140", 0x00423140, &H::scalar_dtor_423140);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b115_03.cpp (game/b115_03)
//
//  * module game/b115_03 (cdecl, ret 0; the other wrappers of the module are in camman.cpp):
//      crt_atan2 0x00421020 (float y, float x) -> CRT atan2 0x007b2240
//
// MSVC 6 /Od frames: each float argument `fld dword; fstp qword` (exact), call the CRT, `fst dword [ebp-4]` (dead
// store, no pop), return the CRT's st0. The C++ returns the CRT result as the double its slot call gives (the
// facade of the wrapper returns double too); re/p1/gamefloat compares both with the original CRT code.
namespace {
namespace game_b115_03 {

double JOAN_CDECL crt_atan2(float y, float x) {  // 0x00421020
    return joan::orig::arg::as<double>(joan::orig::libc::atan2(joan::orig::arg::f64(static_cast<double>(y)), joan::orig::arg::f64(static_cast<double>(x))));
}

#define R(name, addr, fn) JOAN_REPLACE("game/b115_03", name, addr, fn)
R("crt_atan2", 0x00421020, &crt_atan2);
#undef R

}  // namespace game_b115_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Boid::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_01::H::ctor_4203e0>(this)); }  // 0x4203e0
auto joan::orig::Boid::init(std::int32_t a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_01::H::particle_init>(this, a0, a1)); }  // 0x420420
auto joan::orig::Trail::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves81::H::sdd_420790>(this, a0)); }  // 0x420790
auto joan::orig::Boid::release_actor() -> void { joan::bind::member<&::game_b92_01::H::holder_release>(this); }  // 0x4207d0
auto joan::orig::SceneObject::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::delete_420840>(this, a0)); }  // 0x420840
auto joan::orig::Boid::update(std::uint32_t a0, Boid* a1) -> void { joan::bind::member<&::game_b107_18::H::update_420890>(this, a0, a1); }  // 0x420890
auto joan::orig::DynArray::get_420ff0(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::get>(this, a0)); }  // 0x420ff0
auto joan::orig::World::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_05::H::dtor_4214b0>(this)); }  // 0x4214b0
auto joan::orig::Unit::leader() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_units::H::leader>(this)); }  // 0x4216d0
auto joan::orig::DynArray::is_empty_at(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_06::H::is_missing>(this, a0)); }  // 0x4217e0
auto joan::orig::Unit::attach_a() -> void { joan::bind::member<&::game_leaves80a::H::attach_a>(this); }  // 0x421960
auto joan::orig::Group::count_near_421a50(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b106_16::H::count_near_421a50>(this, a0)); }  // 0x421a50
auto joan::orig::Game::reset_three() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::reset_three>(this)); }  // 0x421b60
auto joan::orig::Boid::pick_nearest(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_01::H::pick_nearest>(this, a0)); }  // 0x421ba0
auto joan::orig::init_a3a298() -> void { joan::bind::free<&::game_b96_01::init_a3a298>(); }  // 0x421d90
auto joan::orig::Boid::hits_box() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_09::H::hits_box>(this)); }  // 0x421dd0
auto joan::orig::init_422a00() -> void { joan::bind::free<&::game_b100_01::init_422a00>(); }  // 0x422a00
auto joan::orig::Boid::segment_hit_in_range(void* a0, void* a1, void* a2, void* a3, void* a4) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b95_01::H::segment_hit_in_range>(this, a0, a1, a2, a3, a4)); }  // 0x422a40
auto joan::orig::Place::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b88_06::H::ctor_1f6>(this)); }  // 0x422b50
auto joan::orig::Boid::push_neighbour(Boid* a0) -> void { joan::bind::member<&::game_b97_01::H::push_a33650>(this, a0); }  // 0x422bd0
auto joan::orig::Boid::reset_neighbours() -> void { joan::bind::member<&::game_b99_01::H::reset_group>(this); }  // 0x422c30
auto joan::orig::Unit::distance_to_422ca0(void* a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b92_01::H::distance_to>(this, a0)); }  // 0x422ca0
auto joan::orig::Squad::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::squad_clear>(this)); }  // 0x422f10
auto joan::orig::Boid::set_next(Boid* a0) -> void { joan::bind::member<&::game_b92_01::H::set_66>(this, a0); }  // 0x422f70
auto joan::orig::Boid::next() -> Boid* { return joan::bind::conv<Boid*>(joan::bind::member<&::game_b90_03::H::get_66>(this)); }  // 0x422fa0
auto joan::orig::Boid::set_prev(Boid* a0) -> void { joan::bind::member<&::game_b92_01::H::set_6a>(this, a0); }  // 0x422fc0
auto joan::orig::Boid::unlink() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::relink>(this)); }  // 0x423030
auto joan::orig::init_a3a074() -> void { joan::bind::free<&::game_b96_01::init_a3a074>(); }  // 0x4230c0
auto joan::orig::SceneObject::destruct() -> void { joan::bind::member<&::game_world::H::dtor_4230f0>(this); }  // 0x4230f0
auto joan::orig::Boid::destruct() -> void { joan::bind::member<&::game_b100_01::H::dtor_423190>(this); }  // 0x423190
auto joan::orig::Flock::member_count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_01::H::count_57>(this)); }  // 0x4231c0
auto joan::orig::List::at_word(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_03::H::at_word>(this, a0)); }  // 0x4231f0
