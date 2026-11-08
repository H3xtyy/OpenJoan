// game/unknown_formmove.cpp: functions whose original source file is unknown (formerly game/_unplaced/formmove.cpp;
// re/attribution.csv: between ww/enphysic/collisio.cpp and ww/enpath/pathspr.cpp). They move to their original file
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
#include "pad_feedback.hpp"
#include "animal_pen.hpp"

namespace jo = joan::orig;

namespace {
// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return joan::call::import_call<float>(slot, args...);
#else
    return static_cast<float>(joan::call::import_call<double>(slot, args...));
#endif
}
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::call::import_call;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;

using joan::hook::x87::ftol;

using u32 = std::uint32_t;

using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b110_05.cpp (game/b110_05)
namespace {
namespace game_b110_05 {

using i8 = std::int8_t;

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

void cp(void* dst, const void* src, std::size_t n) { std::memcpy(dst, src, n); }

struct V3 {
    u32 v[3];
};
static_assert(sizeof(V3) == 12, "by-value vector");

// The original stack frame: local at [ebp - off] lives at b[kSize - off].
constexpr i32 kSize = 0x6c8;
struct Frame {
    alignas(16) u8 b[kSize];
};

JOAN_COUNTER(c_step, "FormMove::step_6a45a0");
JOAN_COUNTER(c_step_orig, "FormMove::step_6a45a0 (ran original: bound kind not 1..3)");

bool bound_kind_ok(const u8* actor) {
    const u32 k = jo::as_actor(actor)->aim_mode;
    return k == 1 || k == 2 || k == 3;
}

struct H {
    void step_6a45a0(void* e_arg, void* form, u32 now, float dt, void* path_ctx) {
        c_step.hit();
        u8* const e = static_cast<u8*>(e_arg);
        u8* const s = reinterpret_cast<u8*>(&jo::as_shot(e)->state);  // ebp-0x8
        u8* const F = static_cast<u8*>(form);

        // Entry state, for the restart through the original body (see header).
        const u32 entry_29e = jo::as_shot(e)->blocked;
        const i16 entry_278 = jo::view<i16>(jo::as_shot(e)->idle_frames);
        u8 entry_s0[12];
        cp(entry_s0, s, 12);

        const float k15 = global<float>(d::flt_0_00015);    // 0.00015
        const float kZero = global<float>(d::k_9cf5c4);  // 0
        const float kHalf = global<float>(d::flt_0_5);  // 0.5
        const float kOne = global<float>(d::flt_1);   // 1
        const float kThree = global<float>(d::flt_3); // 3
        const float k1000 = global<float>(d::flt_1000);  // 1000
        const float kCell = global<float>(d::flt_46_1);  // 46.1
        const float kH = global<float>(d::flt_0_021691974);     // 0.0216919...
        const float kLow = global<float>(d::flt_m9999_9);   // -9999.9
        const float k09 = global<float>(d::flt_0_9);    // 0.9
        const float k0015 = global<float>(d::flt_0_0015);  // 0.0015
        const float kG = global<float>(d::flt_m9_81);     // -9.81
        const double kNear = global<double>(d::dbl_1_7353579750096775);
        const float k6000 = global<float>(d::flt_9dd748);
        const float k3000 = global<float>(d::flt_3000);

        Frame fr;
        std::memset(&fr, 0, sizeof fr);
        // Frame dwords that hold pointers, as typed locals (a pointer does not fit the x86 slot on x64):
        void* owner_160 = nullptr;  // [ebp-0x160] the rider's unit (types 6 / 7)
        void* sub_1a8 = nullptr;    // [ebp-0x1a8] BodyState::sub (out argument of the ground query)
        auto P = [&](i32 o) -> u8* { return fr.b + kSize - o; };
        auto Lf = [&](i32 o) -> float& { return *reinterpret_cast<float*>(P(o)); };
        auto Ld = [&](i32 o) -> u32& { return *reinterpret_cast<u32*>(P(o)); };
        auto Lw = [&](i32 o) -> i16& { return *reinterpret_cast<i16*>(P(o)); };
        auto Lb = [&](i32 o) -> u8& { return *P(o); };
        // 0x0060cxxx vector helpers (cdecl, out first, return out); the result is copied to the
        // local `dst` like the original's `mov ecx,[eax]; mov [ebp-dst],ecx ...`.
        auto vsub = [&](i32 out, i32 dst, const void* a, const void* b) {  // 0x0060cc90
            cp(P(dst), joan::orig::arg::as<u8*>(joan::orig::vec_sub(joan::orig::arg::mptr(P(out)), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b))), 12);
        };
        auto vadd = [&](i32 out, i32 dst, const void* a, const void* b) {  // 0x0060cc20
            cp(P(dst), joan::orig::arg::as<u8*>(joan::orig::vec_add(joan::orig::arg::mptr(P(out)), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b))), 12);
        };
        auto vscale = [&](i32 out, i32 dst, const void* v, auto k) {  // 0x0060cd00 (out, v, float k)
            cp(P(dst), joan::orig::arg::as<u8*>(joan::orig::vec_scale(joan::orig::arg::mptr(P(out)), joan::orig::arg::ptr(v), joan::orig::arg::f32(k))), 12);
        };
        auto vscale2 = [&](i32 out, i32 dst, auto k, const void* v) {  // 0x0060cd60 (out, float k, v)
            cp(P(dst), joan::orig::arg::as<u8*>(joan::orig::vec_scaled(joan::orig::arg::mptr(P(out)), joan::orig::arg::f32(k), joan::orig::arg::ptr(v))), 12);
        };
        auto vneg = [&](i32 out, i32 dst, const void* v) {  // 0x0060cbc0
            cp(P(dst), joan::orig::arg::as<u8*>(joan::orig::vec_negate(joan::orig::arg::mptr(P(out)), joan::orig::arg::ptr(v))), 12);
        };
        auto undo_and_flag = [&]() {  // 0x6a6563 / 0x6a68f0 common tail
            jo::view<u32>(jo::as_shot(e)->saved.move_len) = jo::view<u32>(jo::as_shot(e)->state.move_len);
            jo::view<u32>(jo::as_shot(e)->saved.goal_len) = jo::view<u32>(jo::as_shot(e)->state.goal_len);
            jo::as_shot(e)->saved.vel_len = jo::as_shot(e)->state.vel_len;
            cp(&jo::as_shot(e)->state, &jo::as_shot(e)->saved, sizeof(jo::BodyState));
            jo::view<i16>(jo::as_shot(e)->hit) = 1;
        };
        auto ground_query = [&](void* p3, void* p4) {  // 0x006adfd0 (thunk 0x00409372), ret 0x28
            joan::orig::as_players(F)->pick(joan::orig::arg::mptr(P(0xf4)), joan::orig::arg::mptr(P(0xc4)), joan::orig::arg::mptr(p3), joan::orig::arg::mptr(p4), joan::orig::arg::u32(static_cast<i32>(Lw(0x164))), joan::orig::arg::bits(Lf(0x110) * kCell), joan::orig::arg::bits(Lf(0x10c) * kCell), joan::orig::arg::bits(Lf(0x108) * kCell), joan::orig::arg::u32(Ld(0x168)), joan::orig::arg::i32(jo::view<u32>(jo::as_shot(e)->type)));
        };

        // ------------------------------------------------------------ 0x6a45bd idle counter
        jo::as_shot(e)->blocked = 0;
        if (jo::view<u32>(jo::as_shot(e)->type) != 0) {
            if (jo::view<i16>(jo::as_shot(e)->state.move_flag) == 0 && !(jo::as_shot(e)->state.move_len > k15) && jo::view<i16>(jo::as_shot(e)->state.vel_flag) == 0 && !(jo::view<float>(jo::as_shot(e)->state.vel_len) > k15) && jo::view<i16>(jo::as_shot(e)->state.goal_flag) == 0 &&
                !(jo::as_shot(e)->state.goal_len > k15))
                jo::view<i16>(jo::as_shot(e)->idle_frames) = static_cast<i16>(jo::view<i16>(jo::as_shot(e)->idle_frames) + 1);
            else
                jo::view<i16>(jo::as_shot(e)->idle_frames) = 0;
            if (static_cast<i32>(jo::view<i16>(jo::as_shot(e)->idle_frames)) >= 1000) {
                jo::view<i16>(jo::as_shot(e)->hit) = 0;
                return;
            }
        }

        // ------------------------------------------------------------ 0x6a4689 pending bump
        Ld(0xc) = 0;
        if (jo::view<i16>(jo::as_shot(e)->hit) != 0) {
            Ld(0xc) = 1;
            jo::as_shot(e)->state.vec0_x = 0;
            jo::view<u32>(jo::as_shot(e)->state.vec0_y) = 0;
            jo::view<u32>(jo::as_shot(e)->state.vec0_z) = 0;
            if (jo::as_shot(e)->hit_obj != 0) {
                u8* const ent = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::as_shot(e)->hit_obj)));  // 0x006af310
                // [ebp-0x10] = ent in the original: never read here (not mirrored: a pointer does not fit the x86 slot on x64)
                if (ent != nullptr) {
                    if ((!bound_kind_ok(jo::view<u8*>(jo::as_shot(e)->actor)) || !bound_kind_ok(jo::view<u8*>(jo::as_shot(ent)->actor))) && !fix_uninit()) {
                        // Uninitialised bound centre in the original: restart there.
                        c_step_orig.hit();
                        jo::as_shot(e)->blocked = entry_29e;
                        jo::view<i16>(jo::as_shot(e)->idle_frames) = entry_278;
                        cp(s, entry_s0, 12);
                        thiscall<u32>(fn::FormMove_step_6a45a0_body, this, e_arg, form, now, dt, path_ctx);  // original body
                        return;
                    }
                    u8* a = jo::view<u8*>(jo::as_shot(e)->actor);  // also kept in [ebp-0x2c] by the original (never read; not mirrored)
                    Ld(0x654) = jo::as_actor(a)->aim_mode;
                    if (Ld(0x654) == 1) {
                        cp(P(0x1c), jo::as_actor(a)->aim_sphere, 12);
                    } else if (Ld(0x654) == 2) {
                        Lf(0x1c) = (jo::as_actor(a)->aim_box[0] + jo::as_actor(a)->aim_box[3]) * kHalf;
                        Lf(0x18) = (jo::as_actor(a)->aim_box[1] + jo::as_actor(a)->aim_box[4]) * kHalf;
                        Lf(0x14) = (jo::as_actor(a)->aim_box[2] + jo::as_actor(a)->aim_box[5]) * kHalf;
                    } else if (Ld(0x654) == 3) {
                        cp(P(0x1c), &jo::as_actor(a)->collide_kind, 12);  // +0xe4..+0xef: the cylinder centre
                    }
                    {
                        void* m = joan::orig::arg::as<void*>(joan::orig::as_actor(a)->matrix(joan::orig::arg::mptr(P(0x2d0))));  // 0x005ace10
                        import_call<void*>(d::iat_v3d_87_point_transform_3, P(0x38), P(0x1c), m);  // point_transform(out, in, m)
                    }
                    a = jo::view<u8*>(jo::as_shot(ent)->actor);
                    Ld(0x658) = jo::as_actor(a)->aim_mode;
                    if (Ld(0x658) == 1) {
                        cp(P(0x28), jo::as_actor(a)->aim_sphere, 12);
                    } else if (Ld(0x658) == 2) {
                        void* m = joan::orig::arg::as<void*>(joan::orig::as_actor(a)->matrix(joan::orig::arg::mptr(P(0x310))));  // 0x005ace10
                        import_call<void*>(d::iat_v3d_87_matrix_inverse_2, P(0x78), m);           // matrix_inverse(out, in)
                        import_call<void*>(d::iat_v3d_87_point_transform_3, P(0x84), P(0x38), P(0x78));
                        if (!(Lf(0x84) > jo::as_actor(a)->aim_box[0]))
                            Ld(0x28) = jo::view<u32>(jo::as_actor(a)->aim_box[0]);
                        else if (!(jo::as_actor(a)->aim_box[3] > Lf(0x84)))
                            Ld(0x28) = jo::view<u32>(jo::as_actor(a)->aim_box[3]);
                        else
                            Ld(0x28) = Ld(0x84);
                        if (!(Lf(0x80) > jo::as_actor(a)->aim_box[1]))
                            Ld(0x24) = jo::view<u32>(jo::as_actor(a)->aim_box[1]);
                        else if (!(jo::as_actor(a)->aim_box[4] > Lf(0x80)))
                            Ld(0x24) = jo::view<u32>(jo::as_actor(a)->aim_box[4]);
                        else
                            Ld(0x24) = Ld(0x80);
                        Lf(0x20) = (jo::as_actor(a)->aim_box[2] + jo::as_actor(a)->aim_box[5]) * kHalf;
                    } else if (Ld(0x658) == 3) {
                        cp(P(0x28), &jo::as_actor(a)->collide_kind, 12);  // +0xe4..+0xef: the cylinder centre
                    }
                    {
                        void* m = joan::orig::arg::as<void*>(joan::orig::as_actor(a)->matrix(joan::orig::arg::mptr(P(0x350))));  // 0x005ace10
                        import_call<void*>(d::iat_v3d_87_point_transform_3, P(0x90), P(0x28), m);
                    }
                    vsub(0x368, 0x35c, P(0x90), P(0x38));  // -> ebp-0x35c
                    cp(P(0x9c), P(0x35c), 12);
                    Ld(0x94) = 0;
                    Ld(0xa8) = jo::as_shot(e)->state.vec0_x;
                    Ld(0xa4) = jo::view<u32>(jo::as_shot(e)->state.vec0_y);
                    Ld(0xa0) = 0;
                    Lf(0xac) = Lf(0x9c) * Lf(0xa8) + Lf(0x98) * Lf(0xa4);
                    if (Lf(0xac) > kZero) {
                        Lf(0xb0) = Lf(0xac) / (Lf(0x9c) * Lf(0x9c) + Lf(0x98) * Lf(0x98));
                        Lf(0xa8) = Lf(0xa8) - Lf(0x9c) * Lf(0xb0);
                        Lf(0xa4) = Lf(0xa4) - Lf(0x98) * Lf(0xb0);
                        jo::as_shot(e)->state.vec0_x = Ld(0xa8);
                        jo::view<u32>(jo::as_shot(e)->state.vec0_y) = Ld(0xa4);
                        jo::view<u32>(jo::as_shot(e)->state.vec0_z) = Ld(0xa0);
                        jo::view<u32>(jo::as_shot(e)->state.move_len) = 0;
                        jo::as_shot(e)->state.move_start = 0;
                    }
                }
            }
            if (jo::view<i16>(jo::as_shot(e)->state.vel_flag) != 0) jo::as_shot(e)->state.vel_len = 0xbf800000u;  // -1
        }

        // ------------------------------------------------------------ 0x6a4abe locals
        Ld(0xb4) = 0xc61c3f9au;  // -9999.9
        Ld(0xb8) = 0xc61c3f9au;
        Ld(0xbc) = 0xc61c3f9au;
        Ld(0xc0) = 0x3a83126fu;  // 0.001
        Ld(0xc4) = 0;
        Ld(0xc8) = 0;
        Ld(0xcc) = 0;
        Ld(0xd0) = 0;
        if (Ld(0xc) != 0 && jo::view<float>(jo::as_shot(e)->state.vel_len) > kZero) {
            Ld(0xd4) = 0;
        } else if (jo::view<u8*>(jo::as_shot(e)->actor) != nullptr && jo::view<u32>(jo::as_shot(e)->actor->kind) == 0xb && jo::as_shot(e)->actor->owner != nullptr) {
            Lf(0xd4) = static_cast<float>(jo::as_unit(jo::as_shot(e)->actor->owner)->gravity_level);
        } else {
            Lf(0xd4) = joan::orig::arg::as<float>(joan::orig::as_players(F)->time_scale());  // 0x006aabb0
        }
        Lw(0xd8) = 1;
        Lw(0xdc) = 1;
        cp(P(0xe8), obj(d::g_pos), 12);
        Ld(0xf4) = 0;
        Ld(0xf0) = 0;
        Ld(0xec) = 0x3f800000u;
        Ld(0x100) = 0;
        Ld(0xfc) = 0;
        Ld(0xf8) = 0x3dcccccdu;
        Ld(0x104) = jo::as_shot(e)->u32_27a;
        cp(P(0x110), &jo::as_shot(e)->state.pos_x, 12);
        cp(P(0x11c), s, 12);
        cp(P(0x128), jo::as_shot(e)->state.vec_c, 12);
        cp(P(0x138), jo::as_shot(e)->state.orient, 16);
        Lw(0x148) = 0;
        Ld(0x14c) = 0xbf800000u;
        Lf(0x150) = joan::orig::arg::as<float>(joan::orig::as_unit(this)->hit_factor(joan::orig::arg::mptr(e_arg), joan::orig::arg::u32(i32{1})));  // 0x006aa9e0
        Lf(0x154) = joan::orig::arg::as<float>(joan::orig::as_unit(this)->hit_factor(joan::orig::arg::mptr(e_arg), joan::orig::arg::u32(i32{0})));
        Ld(0x158) = 0x3e99999au;  // 0.3
        Lb(0x15c) = 0;
        owner_160 = nullptr;
        Lw(0x164) = joan::orig::arg::as<i16>(joan::orig::as_body(e_arg)->side());  // 0x006b1dd0
        {
            const u32 t = jo::view<u32>(jo::as_shot(e)->type);
            if (t == 9 || t == 0xa || t == 0xb || t == 0xe || t == 0xf || t == 0x11) {
                if (!(jo::as_shot(e)->extent[1] >= jo::as_shot(e)->extent[0]))
                    Ld(0x660) = jo::view<u32>(jo::as_shot(e)->extent[0]);
                else
                    Ld(0x660) = jo::view<u32>(jo::as_shot(e)->extent[1]);
                Lf(0x65c) = Lf(0x660) / kOne;
            } else {
                Ld(0x65c) = 0;
            }
            Ld(0x168) = Ld(0x65c);
            if (jo::view<u32>(jo::as_shot(e)->type) == 7 || jo::view<u32>(jo::as_shot(e)->type) == 6) {
                if (!(jo::as_shot(e)->extent[1] >= jo::as_shot(e)->extent[0]))
                    Ld(0x664) = jo::view<u32>(jo::as_shot(e)->extent[0]);
                else
                    Ld(0x664) = jo::view<u32>(jo::as_shot(e)->extent[1]);
                Lf(0x168) = Lf(0x664) / kThree;
            }
            if (jo::view<u32>(jo::as_shot(e)->type) == 6 || jo::view<u32>(jo::as_shot(e)->type) == 7) owner_160 = jo::as_shot(e)->actor->owner;
        }
        Ld(0x184) = 0x3f800000u;
        if (Lw(0x164) == 0) Ld(0x184) = 0x41a00000u;  // 20
        if (dt > Lf(0x184)) {
            Lf(0xc0) = dt * Lf(0xc0) / Lf(0x184);
            dt = Lf(0x184);
            if (!(Lf(0xc0) >= kOne))
                Ld(0x668) = Ld(0xc0);
            else
                Ld(0x668) = 0x3f800000u;
            Ld(0xc0) = Ld(0x668);
        }
        cp(&jo::as_shot(e)->saved, &jo::as_shot(e)->state, sizeof(jo::BodyState));

        // ------------------------------------------------------------ 0x6a4ea4 timers
        if (jo::as_shot(e)->state.move_len > kZero) {
            if (jo::view<i16>(jo::as_shot(e)->state.move_flag) == 0 && !(now < jo::as_shot(e)->state.move_start)) {
                if (jo::as_shot(e)->state.move_start == global<u32>(d::g_a5762e)) {
                    Ld(0xb4) = 0;
                } else if (jo::as_shot(e)->state.move_start == now) {
                    Lf(0xb4) = dt - kOne;
                } else {
                    const Fpu fpu{true};
                    double t = fpu.sub(static_cast<double>(jo::as_shot(e)->state.move_start),
                                       static_cast<double>(global<i32>(d::g_a5762e)));  // fild qword, fisub
                    t = fpu.sub(t, kOne);
                    const float d = Lf(0xc0) * k1000;
                    Lf(0xb4) = static_cast<float>(fpu.div(t, d));
                }
            } else if (jo::view<i16>(jo::as_shot(e)->state.move_flag) != 0) {
                Ld(0xb4) = 0;
            }
        }
        if (jo::view<float>(jo::as_shot(e)->state.vel_len) > kZero) Ld(0xb8) = 0;
        if (jo::as_shot(e)->state.goal_len < kZero || jo::as_shot(e)->state.goal_len > kZero) Ld(0xbc) = 0;  // fcomp, C3 clear

        if (jo::as_players(F)->cell_count > 0) {
            if (jo::view<u32>(jo::as_shot(e)->type) != 0 && jo::view<i16>(jo::as_shot(e)->u16_2ac) != 0) {
                jo::as_shot(e)->state.sub = nullptr;
                jo::as_shot(e)->state.zone = 0;
                jo::view<i16>(jo::as_shot(e)->u16_2ac) = 0;
            }
            if (jo::as_shot(e)->state.sub == nullptr || jo::as_shot(e)->state.zone == 0) {
                Ld(0xc4) = jo::as_shot(e)->state.cell;
                ground_query(&jo::as_shot(e)->state.sub, &jo::as_shot(e)->state.zone);
                jo::as_shot(e)->state.cell = Ld(0xc4);
            }
            Lf(0xc4) = jo::view<float>(jo::as_shot(e)->state.cell) * kOne / kCell;
        }
        {
            const i32 y = ftol(Lf(0x10c) * kCell);
            const i32 x = ftol(Lf(0x110) * kCell);
            joan::orig::as_point(P(0x18c))->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y));  // 0x004eeb80 (dead pair)
        }
        if (jo::as_shot(e)->state.sub != nullptr && jo::as_shot(e)->state.zone != 0) {
            joan::orig::as_players(F)->pick_point(joan::orig::arg::mptr(P(0xf4)), joan::orig::arg::mptr(P(0xc4)), joan::orig::arg::mptr(jo::as_shot(e)->state.sub), joan::orig::arg::u32(jo::as_shot(e)->state.zone), joan::orig::arg::u32(static_cast<i32>(Lw(0x164))), joan::orig::arg::f32(Lf(0x110) * kCell), joan::orig::arg::f32(Lf(0x10c) * kCell), joan::orig::arg::bits(Lf(0x108) * kCell), joan::orig::arg::i32(jo::view<u32>(jo::as_shot(e)->type)));  // 0x006ae2a0
            Lf(0xc4) = Lf(0xc4) * kH;
        }
        Ld(0x190) = 1;

        // ------------------------------------------------------------ 0x6a5222 sub-step loop
        while (dt > kZero) {
            Lw(0x194) = joan::orig::arg::as<i16>(joan::orig::as_players(F)->word_b1a());  // 0x006aabe0
            Lw(0x148) = 0;
            cp(P(0x1a0), P(0x110), 12);
            if (Lf(0x14c) > kZero) {
                Ld(0xc0) = Ld(0x14c);
                Ld(0x14c) = 0xbf800000u;
            }
            if (!(dt >= kOne)) Lf(0xc0) = Lf(0xc0) * dt;

            // jump impulse (+0xea)
            if (jo::as_shot(e)->state.move_len > k15 && !(Lf(0xb4) > kZero) && Lf(0xb4) > kLow) {
                if (Lf(0xc0) * k1000 > jo::as_shot(e)->state.move_len && jo::as_shot(e)->state.move_len > k15) {
                    Ld(0x14c) = Ld(0xc0);
                    Lf(0xc0) = jo::as_shot(e)->state.move_len / k1000;
                }
                vscale(0x380, 0x374, jo::as_shot(e)->state.move_target, kOne / Lf(0x104));  // -> ebp-0x374
                cp(P(0xe8), P(0x374), 12);
                jo::view<i16>(jo::as_shot(e)->state.move_flag) = 1;
                jo::as_shot(e)->state.move_len = jo::as_shot(e)->state.move_len - Lf(0xc0) * k1000;
                if (Ld(0x190) == 0) cp(P(0x11c), obj(d::g_pos), 12);
                Ld(0x190) = 1;
            } else {
                cp(P(0xe8), obj(d::g_pos), 12);
                Ld(0x190) = 0;
            }
            if (!(jo::view<float>(jo::as_shot(e)->state.vel_len) > kZero) && jo::view<i16>(jo::as_shot(e)->state.vel_flag) != 0 && jo::as_shot(e)->u16_2a2 == 0) {
                jo::view<i16>(jo::as_shot(e)->state.vel_flag) = 0;
                Ld(0x11c) = 0;
                Ld(0x118) = 0;
            }
            if (!(jo::as_shot(e)->state.goal_len > kZero) && jo::view<i16>(jo::as_shot(e)->state.goal_flag) != 0) {
                jo::view<i16>(jo::as_shot(e)->state.goal_flag) = 0;
                Ld(0x11c) = 0;
                Ld(0x118) = 0;
            }
            // push impulse (+0x104)
            if (jo::as_shot(e)->state.goal_len > kZero && !(Lf(0xbc) > kZero) && Lf(0xbc) > kLow) {
                if (Lf(0xc0) * k1000 > jo::as_shot(e)->state.goal_len && jo::as_shot(e)->state.goal_len > k15) {
                    Ld(0x14c) = Ld(0xc0);
                    Lf(0xc0) = jo::as_shot(e)->state.goal_len / k1000;
                }
                vsub(0x3a4, 0x398, jo::as_shot(e)->state.goal, P(0x11c));  // -> ebp-0x398
                vscale(0x3b0, 0x38c, P(0x398), kOne / Lf(0xc0));  // -> ebp-0x38c
                cp(P(0xe8), P(0x38c), 12);
                Ld(0xe0) = 0;
                jo::view<i16>(jo::as_shot(e)->state.goal_flag) = 1;
                jo::as_shot(e)->state.goal_len = jo::as_shot(e)->state.goal_len - Lf(0xc0) * k1000;
                Lb(0x15c) = 1;
                jo::as_shot(e)->blocked = 1;
            }
            // knock impulse (+0x11e)
            if (jo::view<float>(jo::as_shot(e)->state.vel_len) > kZero && !(Lf(0xb8) > kZero) && Lf(0xb8) > kLow) {
                if (Lf(0xc0) * k1000 > jo::view<float>(jo::as_shot(e)->state.vel_len) && jo::view<float>(jo::as_shot(e)->state.vel_len) > k15) {
                    Ld(0x14c) = Ld(0xc0);
                    Lf(0xc0) = jo::view<float>(jo::as_shot(e)->state.vel_len) / k1000;
                }
                if (Lb(0x15c) == 0) {
                    if (Lf(0xec) > k09) {
                        if (jo::as_shot(e)->u16_2a2 != 0) {
                            Lf(0x1a4) = v3d_float(d::iat_v3d_87_length, P(0x11c));  // length
                            import_call<void*>(d::iat_v3d_87_vector_normalize_2, P(0x174), s);            // normalize(out, in)
                            import_call<void*>(d::iat_v3d_87_vector_normalize_2, P(0x180), jo::as_shot(e)->state.vel);
                            Ld(0x178) = Ld(0x16c);
                            import_call<void*>(d::iat_v3d_87_vector_normalize_1, P(0x180));  // normalize in place
                            if (jo::view<float>(jo::as_shot(e)->i32_2a4) - Lf(0x1a4) > kZero) {
                                vscale(0x3c8, 0x3bc, P(0x180), Ld(0x1a4));  // -> ebp-0x3bc
                                cp(P(0x11c), P(0x3bc), 12);
                                vscale(0x3e0, 0x3d4, P(0x180), jo::view<u32>(jo::as_shot(e)->i32_2a8));  // -> ebp-0x3d4
                                cp(P(0xe8), P(0x3d4), 12);
                            }
                        } else {
                            vsub(0x404, 0x3f8, jo::as_shot(e)->state.vel, P(0x11c));  // -> ebp-0x3f8
                            vscale(0x410, 0x3ec, P(0x3f8), kOne / Lf(0xc0));  // -> ebp-0x3ec
                            cp(P(0xe8), P(0x3ec), 12);
                            Ld(0xe0) = 0;
                        }
                    }
                    jo::view<i16>(jo::as_shot(e)->state.vel_flag) = 1;
                    jo::view<float>(jo::as_shot(e)->state.vel_len) = jo::view<float>(jo::as_shot(e)->state.vel_len) - Lf(0xc0) * k1000;
                }
            }
            // 0x6a594a
            sub_1a8 = jo::as_shot(e)->state.sub;
            Ld(0x1ac) = jo::as_shot(e)->state.zone;
            if (!(static_cast<i32>(Lw(0xdc)) < static_cast<i32>(Lw(0xd8))) && jo::as_players(F)->cell_count > 0 &&
                jo::view<u32>(jo::as_shot(e)->type) == 0 && jo::view<i16>(jo::as_shot(e)->u16_2ac) != 0) {
                Ld(0xc4) = jo::as_shot(e)->state.cell;
                ground_query(&sub_1a8, P(0x1ac));
                jo::as_shot(e)->state.cell = Ld(0xc4);
                Lf(0xc4) = Lf(0xc4) * kH;
                jo::view<i16>(jo::as_shot(e)->u16_2ac) = 0;
                Lw(0xdc) = 0;
            }
            if (!(Lf(0x108) >= Lf(0xc4))) {
                Ld(0x114) = 0;
                Ld(0x108) = Ld(0xc4);
            } else if (!(Lf(0xc4) + k0015 >= Lf(0x108))) {
                Lw(0x194) = 0;
            } else {
                Ld(0x1b0) = 1;
            }
            // gravity and velocity
            Ld(0x1bc) = 0;
            Ld(0x1b8) = 0;
            Ld(0x1b4) = 0xc11cf5c3u;
            Ld(0x1c8) = 0;
            Ld(0x1c4) = 0;
            Lf(0x1c0) = kG * Lf(0xd4);
            Lf(0xe8) = Lf(0xe8) + Lf(0x1c8);
            Lf(0xe4) = Lf(0xe4) + Lf(0x1c4);
            Lf(0xe0) = Lf(0xe0) + Lf(0x1c0);
            vneg(0x434, 0x428, P(0x11c));               // -> ebp-0x428
            vscale(0x440, 0x41c, P(0x428), Ld(0x158));  // -> ebp-0x41c
            cp(P(0x1d4), P(0x41c), 12);
            Lf(0xe8) = Lf(0xe8) + Lf(0x1d4);
            Lf(0xe4) = Lf(0xe4) + Lf(0x1d0);
            Lf(0xe0) = Lf(0xe0) + Lf(0x1cc);
            Lf(0xc8) = v3d_float(d::iat_v3d_87_dot_product, P(0x1bc), P(0xf4));  // dot_product
            Lf(0xcc) = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(Ld(0xc8))));             // 0x00427d60 fabs
            vscale2(0x47c, 0x470, Ld(0xc8), P(0xf4));                              // -> ebp-0x470
            vscale(0x488, 0x464, P(0x470), Ld(0xc0));                              // -> ebp-0x464
            vscale2(0x494, 0x458, static_cast<float>(static_cast<i32>(Lw(0x194))), P(0x464));  // -> ebp-0x458
            vscale(0x4ac, 0x4a0, P(0xe8), Ld(0xc0));                               // -> ebp-0x4a0
            vsub(0x4b8, 0x44c, P(0x4a0), P(0x458));                                // -> ebp-0x44c
            cp(P(0x1e0), P(0x44c), 12);
            if (!(joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(Ld(0x11c)))) > k15) && !(joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(Ld(0x118)))) > k15) &&
                !(joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(Ld(0x114)))) > k15)) {
                Lf(0xd0) = static_cast<float>(static_cast<i32>(Lw(0x194))) * (kHalf * Lf(0x154) * Lf(0xcc) * Lf(0xc0));
            } else {
                Lf(0xd0) = static_cast<float>(static_cast<i32>(Lw(0x194))) * (kHalf * Lf(0x150) * Lf(0xcc) * Lf(0xc0));
            }
            if (Lf(0xd0) > kOne) Ld(0xd0) = 0x3f800000u;
            Ld(0x1ec) = 0;
            Ld(0x1e8) = 0;
            Ld(0x1e4) = 0;
            cp(P(0x1fc), P(0x138), 16);
            vscale(0x4e8, 0x4dc, P(0x11c), kOne - Lf(0xd0));  // -> ebp-0x4dc
            vadd(0x4f4, 0x4d0, P(0x4dc), P(0x1e0));          // -> ebp-0x4d0
            vscale(0x500, 0x4c4, P(0x4d0), kOne / (kOne + Lf(0xd0)));  // -> ebp-0x4c4
            cp(P(0x208), P(0x4c4), 12);
            vadd(0x53c, 0x530, P(0x11c), P(0x208));  // -> ebp-0x530
            vscale2(0x548, 0x524, 0x3f000000u, P(0x530));  // 0.5 -> ebp-0x524
            vscale(0x554, 0x518, P(0x524), Ld(0xc0));  // -> ebp-0x518
            vadd(0x560, 0x50c, P(0x110), P(0x518));  // -> ebp-0x50c
            cp(P(0x214), P(0x50c), 12);
            Ld(0x144) = static_cast<u32>(ftol(Lf(0x214) * kCell));
            Ld(0x140) = static_cast<u32>(ftol(Lf(0x210) * kCell));
            Ld(0x13c) = static_cast<u32>(ftol(Lf(0x20c) * kCell));
            if (jo::view<u32>(jo::as_shot(e)->state.cell_pos[0]) != Ld(0x144) || jo::view<u32>(jo::as_shot(e)->state.cell_pos[1]) != Ld(0x140) || jo::view<u32>(jo::as_shot(e)->state.cell_pos[2]) != Ld(0x13c)) Lw(0x148) = 1;
            sub_1a8 = jo::as_shot(e)->state.sub;
            Ld(0x1ac) = jo::as_shot(e)->state.zone;

            // 0x6a610e: cell change handling
            const u32 t = jo::view<u32>(jo::as_shot(e)->type);
            bool free_move;  // 0x6a615a path
            if (t == 1 && !(jo::as_shot(e)->state.goal_len > kZero))
                free_move = true;
            else
                free_move = t == 6 && owner_160 != nullptr && jo::view<u32>(jo::as_unit(owner_160)->rider_id) == 0;
            if (free_move && (t == 9 || t == 0xa || t == 0xb || t == 0xc || t == 0xd || t == 0xe || t == 0xf ||
                              t == 0x10 || t == 0x11 || t == 0x13))
                free_move = false;
            if (free_move) {
                if (Lw(0x148) != 0) {
                    jo::view<u32>(jo::as_shot(e)->state.cell_pos[0]) = Ld(0x144);
                    jo::view<u32>(jo::as_shot(e)->state.cell_pos[1]) = Ld(0x140);
                    jo::view<u32>(jo::as_shot(e)->state.cell_pos[2]) = Ld(0x13c);
                    jo::view<i16>(jo::as_shot(e)->u16_2ac) = 1;
                }
                cp(P(0x110), P(0x214), 12);
            } else {
                const i16 blocked = thiscall<i16>(
                    fn::PhysicSystem_check_path_pass_through, F, &jo::as_shot(e)->state.sub, &jo::as_shot(e)->state.zone, static_cast<i32>(Lw(0x164)), jo::view<u32>(jo::as_shot(e)->state.cell_pos[0]), jo::view<u32>(jo::as_shot(e)->state.cell_pos[1]), jo::view<u32>(jo::as_shot(e)->state.cell_pos[2]),
                    jo::as_shot(e)->state.cell, joan::orig::arg::bits(Lf(0x214) * kCell), joan::orig::arg::bits(Lf(0x210) * kCell), joan::orig::arg::bits(Lf(0x20c) * kCell), Ld(0x168), t);  // 0x006ae610
                // (the three positions as dword bits: the wrapper takes integer arguments, so on System V x86-64 floats would go to XMM registers it never reads)
                if (static_cast<i32>(blocked) != 0) {
                    jo::view<u32>(jo::as_shot(e)->state.cell_pos[0]) = Ld(0x144);
                    jo::view<u32>(jo::as_shot(e)->state.cell_pos[1]) = Ld(0x140);
                    jo::view<u32>(jo::as_shot(e)->state.cell_pos[2]) = Ld(0x13c);
                    jo::view<i16>(jo::as_shot(e)->u16_2ac) = 1;
                    cp(P(0x110), P(0x214), 12);
                } else if (jo::as_shot(e)->state.zone == 0) {
                    bool slide = false;  // 0x6a642c
                    if (jo::view<float>(jo::as_shot(e)->state.vel_len) > kZero &&
                        joan::orig::arg::as<u8>(joan::orig::vector_differs(joan::orig::arg::mptr(static_cast<void*>(jo::as_shot(e)->state.vel)), joan::orig::arg::mptr(obj(d::g_pos)))) != 0)  // 0x0060cf40
                        slide = true;
                    else if (jo::as_shot(e)->state.goal_len > kZero &&
                             joan::orig::arg::as<u8>(joan::orig::vector_differs(joan::orig::arg::mptr(static_cast<void*>(jo::as_shot(e)->state.goal)), joan::orig::arg::mptr(obj(d::g_pos)))) != 0)
                        slide = true;
                    else {
                        const u32 t2 = jo::view<u32>(jo::as_shot(e)->type);
                        slide = t2 == 9 || t2 == 0xa || t2 == 0xb || t2 == 0xe || t2 == 0xf || t2 == 0x11 || t2 == 7 ||
                                t2 == 6;
                    }
                    bool done = false;
                    if (slide) {
                        // Both branches of the ebp-0x168 > 0 test pick ebp-0x214.
                        V3 v;  // [ebp-0x688] = &[ebp-0x214] in the original (only read back here)
                        cp(&v, P(0x214), 12);
                        const i16 r = joan::orig::as_unit(this)->steer_6a9570(e_arg, form, sub_1a8, joan::orig::arg::i32(Ld(0x1ac)), P(0x110), joan::orig::arg::f32(v.v[0]),
                                                    joan::orig::arg::f32(v.v[1]), joan::orig::arg::f32(v.v[2]), P(0x11c), joan::orig::arg::f32(Ld(0xc0)), joan::orig::arg::i32(joan::orig::arg::bits(dt)));  // 0x006a9570
                        done = static_cast<i32>(r) != 0;
                    }
                    if (!done) {
                        if ((jo::view<i16>(jo::as_shot(e)->state.move_flag) != 0 && jo::as_shot(e)->state.move_target[2] > kZero) || Lf(0x200) < kZero || Lf(0x200) > kZero) {
                            jo::as_shot(e)->state.sub = sub_1a8;
                            jo::as_shot(e)->state.zone = Ld(0x1ac);
                            jo::view<u32>(jo::as_shot(e)->state.cell_pos[2]) = Ld(0x13c);
                            jo::view<u32>(jo::as_shot(e)->state.move_target[0]) = 0;
                            jo::view<u32>(jo::as_shot(e)->state.move_target[1]) = 0;
                            Ld(0x208) = 0;
                            Ld(0x204) = 0;
                            jo::view<i16>(jo::as_shot(e)->u16_2ac) = 1;
                            Ld(0x108) = Ld(0x20c);
                        } else {
                            undo_and_flag();
                            return;
                        }
                    }
                } else if (Lf(0x108) >= Lf(0xc4)) {
                    Ld(0x110) = Ld(0x1a0);
                    Ld(0x10c) = Ld(0x19c);
                    V3 v;
                    cp(&v, P(0x214), 12);
                    joan::orig::as_unit(this)->steer_6a9570(e_arg, form, sub_1a8, joan::orig::arg::i32(Ld(0x1ac)), P(0x110), joan::orig::arg::f32(v.v[0]), joan::orig::arg::f32(v.v[1]),
                                  joan::orig::arg::f32(v.v[2]), P(0x208), joan::orig::arg::f32(Ld(0xc0)), joan::orig::arg::i32(joan::orig::arg::bits(dt)));  // 0x006a9570
                    if (Lf(0x20c) >= Lf(0xc4)) {
                        Ld(0x108) = Ld(0x20c);
                        jo::view<u32>(jo::as_shot(e)->state.cell_pos[2]) = Ld(0x13c);
                    } else {
                        Ld(0x108) = Ld(0xc4);
                        jo::view<u32>(jo::as_shot(e)->state.cell_pos[2]) = static_cast<u32>(ftol(jo::view<float>(jo::as_shot(e)->state.cell)));
                    }
                }
            }
            // 0x6a669f
            cp(P(0x11c), P(0x208), 12);
            cp(P(0x138), P(0x1fc), 16);
            cp(P(0x128), P(0x1ec), 12);
            Lw(0xdc) = static_cast<i16>(Lw(0xdc) + 1);
            if (!(Lf(0x14c) >= kZero))
                dt = dt - kOne;
            else
                dt = dt - Lf(0xc0) / Lf(0x14c);
            if (Lf(0xb4) > kZero) Lf(0xb4) = Lf(0xb4) - kOne;
            if (Lf(0xb8) > kZero) Lf(0xb8) = Lf(0xb8) - kOne;
            if (Lf(0xbc) > kZero) Lf(0xbc) = Lf(0xbc) - kOne;
        }

        // ------------------------------------------------------------ 0x6a67d0 final height
        if (jo::as_players(F)->cell_count > 0 && (jo::view<i16>(jo::as_shot(e)->u16_2ac) != 0 || jo::as_shot(e)->state.zone == 0)) {
            Ld(0xc4) = jo::as_shot(e)->state.cell;
            ground_query(&jo::as_shot(e)->state.sub, &jo::as_shot(e)->state.zone);
            jo::as_shot(e)->state.cell = Ld(0xc4);
            Lf(0xc4) = Lf(0xc4) * kH;
            jo::view<i16>(jo::as_shot(e)->u16_2ac) = 0;
        }
        bool reset = jo::as_shot(e)->state.zone == 0;
        if (!reset) {
            const float d = jo::as_shot(e)->state.pos_z - Lf(0x108);
            const double m = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(d))));  // CRT fabs
            reset = m >= kNear;  // jne on C0: below or NaN keeps the state
        }
        if (reset) {
            // [Fixes] AnimalPen (game/animal_pen.hpp): a step undone for ending on no ground marks the body.
            if (jo::as_shot(e)->state.zone == 0 && joan::hook::settings().fix_animal_pen && jo::as_shot(e)->actor != nullptr)
                joan::animal_pen::mark_off_mesh(e, jo::as_shot(e)->actor->owner);
            jo::view<u32>(jo::as_shot(e)->saved.goal[0]) = 0;
            jo::view<u32>(jo::as_shot(e)->saved.goal[1]) = 0;
            jo::view<u32>(jo::as_shot(e)->saved.goal[2]) = 0;
            undo_and_flag();
            jo::view<i16>(jo::as_shot(e)->u16_2ac) = 0;
            return;
        }

        // ------------------------------------------------------------ 0x6a6975 bump hand-over
        if (jo::view<i16>(jo::as_shot(e)->hit) != 0) {
            jo::view<i16>(jo::as_shot(e)->hit) = 0;
            Ld(0x220) = 0;
            Ld(0x21c) = 0;
            Ld(0x218) = 0;
            if (jo::as_shot(e)->hit_obj != 0) {
                u8* const ent = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::as_shot(e)->hit_obj)));  // 0x006af310
                // [ebp-0x224 / -0x228 / -0x22c] = ent, u1, u2 in the original: never read (not mirrored)
                if (ent != nullptr && jo::view<u32>(jo::as_shot(ent)->type) != 0 && !(jo::as_shot(ent)->state.goal_len > kZero) &&
                    jo::view<i16>(jo::as_shot(ent)->state.goal_flag) == 0) {
                    u8* const u1 = jo::view<u8*>(jo::as_shot(e)->actor->owner);
                    u8* const u2 = jo::view<u8*>(jo::as_shot(ent)->actor->owner);
                    auto special = [](i32 v) {
                        return v == 0xa || v == 0xb || v == 0xe || v == 0xf || v == 0x10 || v == 0x11 || v == 0x12 ||
                               v == 0x13 || v == 0x14;
                    };
                    const bool go = special(jo::as_unit(u1)->anim_kind) || special(jo::as_unit(u1)->prev_action) || Lb(0x15c) != 0;
                    if (go && static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_game(u2)->byte_2e1())) == 0) {  // 0x00432b90
                        const u32 t2 = jo::view<u32>(jo::as_shot(ent)->type);
                        if (t2 != 0x11 && t2 != 0x10 && t2 != 0x13 && t2 != 0xf && t2 != 0xb && t2 != 7 && t2 != 6 &&
                            jo::as_shot(ent)->blocked == 0) {
                            vsub(0x578, 0x56c, jo::as_unit(u2)->position_, jo::as_unit(jo::as_shot(e)->actor->owner)->position_);  // -> ebp-0x56c
                            cp(P(0x238), P(0x56c), 12);
                            vsub(0x590, 0x584, P(0x110), &jo::as_shot(e)->state.pos_x);  // -> ebp-0x584
                            cp(P(0x244), P(0x584), 12);
                            Ld(0x230) = 0;
                            Lf(0x248) = v3d_float(d::iat_v3d_87_length, P(0x244));  // length
                            if (Lf(0x248) >= k15) {
                                import_call<void*>(d::iat_v3d_87_cross_product, P(0x254), P(0x244), P(0x238));  // cross(out, a, b)
                                Ld(0x258) = Ld(0x244);
                                Ld(0x244) = Ld(0x240);
                                Ld(0x240) = Ld(0x258);
                                Ld(0x23c) = 0;
                                if (Lf(0x24c) > kZero)
                                    Lf(0x244) = -Lf(0x244);
                                else
                                    Lf(0x240) = -Lf(0x240);
                                vscale(0x5b4, 0x5a8, P(0x244), kOne / Lf(0x248));  // -> ebp-0x5a8
                                vscale(0x5c0, 0x59c, P(0x5a8), 0x40e00000u);       // 7 -> ebp-0x59c
                                cp(P(0x264), P(0x59c), 12);
                                const u32 rate = global<u32>(d::g_frame_step_ms) << 2;
                                joan::orig::as_player(ent)->start_move(joan::orig::arg::mptr(P(0x264)), joan::orig::arg::i32(global<u32>(d::g_now)), joan::orig::arg::u32(rate));  // 0x006b12a0
                                jo::as_shot(ent)->blocked = 1;
                            }
                        }
                    }
                }
            }
            jo::as_shot(e)->hit_obj = 0;
        }

        // ------------------------------------------------------------ 0x6a6eba ground flag
        if (Lf(0xc4) + k15 >= Lf(0x108)) {  // jne on C0: NaN takes the else path
            Ld(0x114) = 0;
            Ld(0x108) = Ld(0xc4);
            jo::view<i16>(jo::as_shot(e)->state.on_ground) = 1;
        } else if (jo::view<float>(jo::as_shot(e)->state.vel_len) > kZero) {
            jo::view<i16>(jo::as_shot(e)->state.on_ground) = 1;
        } else if (Lf(0xc4) + kHalf > Lf(0x108)) {
            jo::view<i16>(jo::as_shot(e)->state.on_ground) = 1;
        } else if (Ld(0xc) == 0) {
            jo::view<i16>(jo::as_shot(e)->state.on_ground) = 0;
        }
        // 0x006accf0 leaves the unstored z / sqrt(..) quotient in st0 (extended exponent): taken as
        // double so a tiny negative value cannot round to -0 before the compare.
        if (!(joan::orig::arg::as<double>(joan::orig::grid::slope(joan::orig::arg::mptr(P(0xf4)))) >= static_cast<double>(kZero))) jo::view<i16>(jo::as_shot(e)->state.on_ground) = 0;
        cp(&jo::as_shot(e)->state.pos_x, P(0x110), 12);
        cp(s, P(0x11c), 12);
        cp(jo::as_shot(e)->state.orient, P(0x138), 16);
        cp(jo::as_shot(e)->state.vec_c, P(0x128), 12);
        if (jo::view<i16>(jo::as_shot(e)->hit) != 0) return;

        // ------------------------------------------------------------ 0x6a7019 path cells
        if (joan::orig::arg::as<u8>(joan::orig::vector_differs(joan::orig::arg::mptr(static_cast<void*>(P(0x110))), joan::orig::arg::mptr(static_cast<void*>(&jo::as_shot(e)->saved.pos_x)))) == 0) return;
        vsub(0x5e4, 0x5d8, P(0x110), &jo::as_shot(e)->saved.pos_x);         // -> ebp-0x5d8
        vscale(0x5f0, 0x5cc, P(0x5d8), 0x42386666u);     // 46.1 -> ebp-0x5cc
        cp(P(0x270), P(0x5cc), 12);
        vscale(0x608, 0x5fc, P(0x110), 0x42386666u);     // -> ebp-0x5fc
        if (joan::orig::arg::as<u32>(joan::orig::as_players(path_ctx)->collide_68de40(form, joan::orig::arg::mptr(e_arg), P(0x5fc), joan::orig::arg::mptr(P(0x270)), joan::orig::arg::mptr(P(0x274)))) == 0) {  // 0x0068de40
            vscale(0x620, 0x614, P(0x110), 0x42386666u);  // -> ebp-0x614
            if (joan::orig::arg::as<u32>(joan::orig::as_scene(path_ctx)->segment_blocked(joan::orig::arg::mptr(e_arg), joan::orig::arg::mptr(P(0x614)), joan::orig::arg::mptr(P(0x270)))) == 0) return;  // 0x00692c10
        }
        jo::view<u32>(jo::as_shot(e)->saved.move_len) = jo::view<u32>(jo::as_shot(e)->state.move_len);
        jo::view<u32>(jo::as_shot(e)->saved.goal_len) = jo::view<u32>(jo::as_shot(e)->state.goal_len);
        jo::as_shot(e)->saved.vel_len = jo::as_shot(e)->state.vel_len;
        cp(&jo::as_shot(e)->saved, &jo::as_shot(e)->state, 12);  // u32_0 / vec0_y / vec0_z
        cp(&jo::as_shot(e)->state, &jo::as_shot(e)->saved, sizeof(jo::BodyState));
        jo::view<i16>(jo::as_shot(e)->state.on_ground) = static_cast<i16>(jo::view<i16>(jo::as_shot(e)->state.on_ground) + 1);
        if (jo::as_shot(e)->hit_obj == 0) return;
        u8* const ent = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::as_shot(e)->hit_obj)));  // 0x006af310
        // [ebp-0x278 / -0x27c / -0x280] = ent, u2, u1 in the original: never read (not mirrored)
        u8* const u2 = jo::view<u8*>(jo::as_shot(ent)->actor->owner);
        u8* const u1 = jo::view<u8*>(jo::as_shot(e)->actor->owner);
        vsub(0x638, 0x62c, jo::as_unit(u2)->position_, jo::as_unit(jo::as_shot(e)->actor->owner)->position_);  // -> ebp-0x62c
        cp(P(0x28c), P(0x62c), 12);
        Lf(0x290) = v3d_float(d::iat_v3d_87_length, static_cast<void*>(jo::as_shot(e)->state.move_target));  // length
        if (!(jo::as_shot(e)->state.move_len > kZero)) return;
        if (!(jo::as_shot(e)->state.move_end > global<u32>(d::g_now))) return;
        if (!(Lf(0x290) < kZero || Lf(0x290) > kZero)) return;  // C3: zero or NaN
        if (static_cast<i32>(jo::as_unit(u1)->action) == 1) return;
        {
            const i32 st = jo::as_unit(u1)->anim_kind;
            if (st == 0x18 || st == 0x1a || st == 5 || st == 6) return;
        }
        {
            u8* const info = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u2)->type_id))));  // 0x0042d0a0
            if (jo::self<jo::UnitType>(info)->kind != 0) return;
        }
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, P(0x28c));  // normalize in place
        vscale(0x650, 0x644, P(0x28c), Ld(0x290));         // -> ebp-0x644
        joan::orig::as_body(ent)->init_target(joan::orig::arg::mptr(P(0x644)), joan::orig::arg::u32(global<u32>(d::g_now) - global<u32>(d::g_frame_step_ms)), joan::orig::arg::u32(i32{0x64}));  // 0x006b11a0
        if (joan::orig::arg::as<u32>(joan::orig::as_unit(u2)->mounted()) != 0) return;  // 0x00430190
        if (static_cast<i32>(jo::as_unit(u1)->anim_kind) == 1) return;
        if (!(Lf(0x290) >= k6000) && !(jo::as_shot(e)->state.move_target[2] >= k3000)) {
            if (Lf(0x290) > k3000) joan::orig::as_unit(u2)->set_action(i32{0x23}, i32{0}, i32{1});  // 0x00501190
        } else {
            joan::orig::as_unit(u2)->pick_idle_anim(i32{0});  // 0x00509000
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

JOAN_REPLACE("game/b110_05", "FormMove::step_6a45a0", 0x006a45a0, &H::step_6a45a0);

}  // namespace game_b110_05
}  // namespace

// ---------------------------------------- from modules/game_b109_04.cpp (game/b109_04)
namespace {
namespace game_b109_04 {

using joan::orig::helpers::gf;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");
struct V4 {
    u32 d[4];
};
static_assert(sizeof(V4) == 16, "V4");

void* const kSoldiers = obj(d::g_units);
void* const kModels = obj(d::g_physic_sys);

JOAN_COUNTER(c_ballistic, "Ballistic::step_6a8060");

u8* soldier(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 soldier_gone(u32 id) { return joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70

// ------------------------------------------------------------------------------------ H
struct H {
    // ---------------------------------------------------------------- 0x006a8060
    u16 ballistic_6a8060(u8* p, void* a, u32 now, float dt, void* world) {
        c_ballistic.hit();
        if (jo::view<i16>(jo::as_shot(p)->hit) != 0) return 0;
        u8* const p8 = reinterpret_cast<u8*>(&jo::as_shot(p)->state);
        std::memcpy(&jo::as_shot(p)->saved, &jo::as_shot(p)->state, sizeof(jo::BodyState));  // rep movsd 0x4d + movsw
        float fc = -9999.9f;   // 0xc61c3f9a
        float f10 = -9999.9f;
        float f14 = 0.001f;    // 0x3a83126f
        float f18 = 0.0f;
        const float f24 = 0.0f;
        V3 v38{global<float>(d::g_pos), global<float>(d::g_a69cdc), global<float>(d::g_a69ce0)};
        V3 v44{0.0f, 0.0f, 0.1f};  // 0x3dcccccd
        const float f54 = jo::view<float>(jo::as_shot(p)->u32_27a);
        V3 v60;
        std::memcpy(&v60, &jo::as_shot(p)->state.pos_x, sizeof v60);  // pos_x, pos_y, pos_z
        V3 v6c;
        std::memcpy(&v6c, &jo::as_shot(p)->state.vec0_x, sizeof v6c);  // u32_0, vec0_y, vec0_z
        V3 v78 = jo::view<V3>(jo::as_shot(p)->state.vec_c);
        V4 v88 = jo::view<V4>(jo::as_body_state(p8)->orient);
        float f8c = -1.0f;
        const float f90 = 0.3f;  // 0x3e99999a
        float f1c = 0.0f;  // uninitialised in the original for kinds the caller never passes
        float f20 = 0.0f;
        switch (jo::as_shot(p)->type) {
            case 2:
                f1c = global<float>(d::g_throw_arw_gl);
                f20 = 2.0f;
                break;
            case 3:
                if (jo::view<i16>(jo::as_body_state(p8)->on_ground) != 0) return 1;
                f1c = global<float>(d::g_throw_proj_gl);
                f20 = 5.0f;
                break;
            case 4:
                if (jo::view<i16>(jo::as_body_state(p8)->on_ground) != 0) return 1;
                f1c = global<float>(d::g_throw_expl_gl);
                f20 = 5.0f;
                break;
            case 5:
                if (jo::view<i16>(jo::as_body_state(p8)->on_ground) != 0) return 1;
                f1c = global<float>(d::g_throw_stc_gl);
                f20 = 5.0f;
                break;
            case 0x12:
                if (jo::view<i16>(jo::as_body_state(p8)->on_ground) != 0) return 1;
                f1c = global<float>(d::g_throw_can_gl);
                f20 = 5.0f;
                break;
            default: break;
        }
        const float k0 = gf(d::k_9cf5c4);
        const float k1 = gf(d::flt_1);
        const float k1000 = gf(d::flt_1000);
        if (dt > f20) {
            f14 = dt * f14 / f20;
            dt = f20;
            f14 = !(f14 >= k1) ? f14 : 1.0f;
        }
        if (jo::as_body_state(p8)->move_len > k0) {
            if (jo::view<i16>(jo::as_body_state(p8)->move_flag) == 0 && now >= jo::as_body_state(p8)->move_start) {
                if (jo::as_body_state(p8)->move_start == global<u32>(d::g_a5762e)) {
                    fc = 0.0f;
                } else if (jo::as_body_state(p8)->move_start == now) {
                    fc = dt - k1;
                } else {
                    const Fpu fpu{true};
                    double t = fpu.sub(static_cast<double>(jo::as_body_state(p8)->move_start), static_cast<double>(global<i32>(d::g_a5762e)));
                    t = fpu.sub(t, static_cast<double>(k1));
                    const float d = f14 * k1000;
                    fc = static_cast<float>(fpu.div(t, static_cast<double>(d)));
                }
            } else if (jo::view<i16>(jo::as_body_state(p8)->move_flag) != 0) {
                fc = 0.0f;
            }
        }
        if (jo::view<float>(jo::as_body_state(p8)->vel_len) > k0) f10 = 0.0f;  // both arms of the original store 0
        {
            const i32 iy = ftol(static_cast<double>(v60.y * gf(d::flt_46_1)));
            const i32 ix = ftol(static_cast<double>(v60.x * gf(d::flt_46_1)));
            u32 v98[2] = {0, 0};
            joan::orig::as_point(v98)->set(joan::orig::arg::u32(ix), joan::orig::arg::u32(iy));  // 0x004eeb80
        }
        while (dt > k0) {
            if (f8c > k0) {
                f14 = f8c;
                f8c = -1.0f;
            }
            if (!(dt >= k1)) f14 = f14 * dt;
            if (jo::as_body_state(p8)->move_len > gf(d::flt_0_00015) && !(fc > k0) && fc > gf(d::flt_m9999_9)) {
                if (f14 * k1000 > jo::as_body_state(p8)->move_len && jo::as_body_state(p8)->move_len > gf(d::flt_0_00015)) {
                    f8c = f14;
                    f14 = jo::as_body_state(p8)->move_len / k1000;
                }
                const float inv = k1 / f54;
                V3 t{0.0f, 0.0f, 0.0f};
                v38 = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t, joan::orig::arg::ptr(jo::as_body_state(p8)->move_target), joan::orig::arg::f32(inv)));  // 0x0060cd00
                jo::view<i16>(jo::as_body_state(p8)->move_flag) = 1;
                jo::as_body_state(p8)->move_len = jo::as_body_state(p8)->move_len - f14 * k1000;
            } else {
                v38 = V3{global<float>(d::g_pos), global<float>(d::g_a69cdc), global<float>(d::g_a69ce0)};
            }
            if (jo::view<float>(jo::as_body_state(p8)->vel_len) > k0 && !(f10 > k0) && f10 > gf(d::flt_m9999_9)) {
                if (f14 * k1000 > jo::view<float>(jo::as_body_state(p8)->vel_len) && jo::view<float>(jo::as_body_state(p8)->vel_len) > gf(d::flt_0_00015)) {
                    f8c = f14;
                    f14 = jo::view<float>(jo::as_body_state(p8)->vel_len) / k1000;
                }
                V3 t0{0.0f, 0.0f, 0.0f};
                const V3 v194 = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&t0, joan::orig::arg::ptr(jo::as_body_state(p8)->vel), &v6c));  // 0x0060cc90
                const float inv = k1 / f14;
                V3 t1{0.0f, 0.0f, 0.0f};
                v38 = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t1, &v194, joan::orig::arg::f32(inv)));  // 0x0060cd00
                jo::view<i16>(jo::as_body_state(p8)->vel_flag) = 1;
                jo::view<float>(jo::as_body_state(p8)->vel_len) = jo::view<float>(jo::as_body_state(p8)->vel_len) - f14 * k1000;
            }
            V3 va4{0.0f, 0.0f, gf(d::flt_m9_81) * f1c};
            v38.x = v38.x + va4.x;
            v38.y = v38.y + va4.y;
            v38.z = v38.z + va4.z;
            {
                const i32 kind = jo::as_shot(p)->type;
                if (!(kind >= 2 && (kind <= 5 || kind == 0x12))) {
                    V3 t0{0.0f, 0.0f, 0.0f};
                    const V3 n = *joan::orig::arg::as<V3*>(joan::orig::vec_negate(&t0, &v6c));  // 0x0060cbc0
                    V3 t1{0.0f, 0.0f, 0.0f};
                    const V3 vb0 = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t1, &n, joan::orig::arg::f32(f90)));  // 0x0060cd00
                    if (f1c < k0 || f1c > k0) {
                        v38.x = v38.x + vb0.x;
                        v38.y = v38.y + vb0.y;
                        v38.z = v38.z + vb0.z;
                    }
                }
            }
            f18 = v3d_float(d::iat_v3d_87_dot_product, &va4, &v44);  // dot_product
            (void)joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(f18)));           // 0x00427d60 (fabs; result dead)
            V3 t2{0.0f, 0.0f, 0.0f};
            const V3 vbc = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t2, &v38, joan::orig::arg::f32(f14)));
            const V3 vc8{0.0f, 0.0f, 0.0f};
            const V4 vd8 = v88;
            V3 t3{0.0f, 0.0f, 0.0f};
            const V3 v218 = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t3, &v6c, joan::orig::arg::f32(k1 - f24)));
            V3 t4{0.0f, 0.0f, 0.0f};
            const V3 v20c = *joan::orig::arg::as<V3*>(joan::orig::vec_add(&t4, &v218, &vbc));  // 0x0060cc20
            V3 t5{0.0f, 0.0f, 0.0f};
            const V3 ve4 = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t5, &v20c, joan::orig::arg::f32(k1 / (k1 + f24))));
            V3 t6{0.0f, 0.0f, 0.0f};
            const V3 v26c = *joan::orig::arg::as<V3*>(joan::orig::vec_add(&t6, &v6c, &ve4));
            V3 t7{0.0f, 0.0f, 0.0f};
            const V3 v260 = *joan::orig::arg::as<V3*>(joan::orig::vec_scaled(&t7, 0.5f, &v26c));  // 0x0060cd60
            V3 t8{0.0f, 0.0f, 0.0f};
            const V3 v254 = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t8, &v260, joan::orig::arg::f32(f14)));
            V3 t9{0.0f, 0.0f, 0.0f};
            const V3 vf0 = *joan::orig::arg::as<V3*>(joan::orig::vec_add(&t9, &v60, &v254));
            v60 = vf0;
            v6c = ve4;
            v88 = vd8;
            v78 = vc8;
            if (!(f8c >= k0))
                dt = dt - k1;
            else
                dt = dt - f14 / f8c;
            if (fc > k0) fc = fc - k1;
            if (f10 > k0) f10 = f10 - k1;
        }
        joan::orig::as_body(p)->side();  // 0x006b1dd0 (result dead)
        V3 t0{0.0f, 0.0f, 0.0f};
        const V3 v2b4 = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&t0, &v60, joan::orig::arg::ptr(&jo::as_shot(p)->saved.pos_x)));
        V3 t1{0.0f, 0.0f, 0.0f};
        V3 v100 = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t1, &v2b4, 46.1f));  // 0x42386666
        if (v3d_float(d::iat_v3d_87_length, &v100) > k0) {
            u8* const p104 = jo::view<u8*>(jo::as_shot(p)->actor->owner);
            V3 t2{0.0f, 0.0f, 0.0f};
            V3 v2d8 = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t2, &v60, 46.1f));
            float f108 = 0.0f;  // written by 0x0068de40 whenever it returns non-zero
            u32 r10c = joan::orig::arg::as<u32>(joan::orig::as_players(world)->collide_68de40(a, joan::orig::arg::mptr(p), &v2d8, &v100, &f108));  // 0x0068de40
            if (r10c == jo::self<jo::Missile>(p104)->shooter_record) {
                r10c = 0;
                jo::view<i16>(jo::as_shot(p)->hit) = 0;
            }
            if (r10c != 0) {
                u8* const q110 = joan::orig::arg::as<u8*>(joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(r10c)));  // 0x006af310
                if (jo::view<u8*>(jo::as_shot(q110)->actor) != nullptr && jo::as_shot(q110)->actor->kind == 0xb) {
                    u8* const q114 = jo::view<u8*>(jo::as_shot(q110)->actor->owner);
                    if (soldier_gone(jo::self<jo::Missile>(p104)->shooter_unit_id) == 0) {
                        u8* const e = soldier(jo::self<jo::Missile>(p104)->shooter_unit_id);
                        bool drop = true;
                        if (joan::orig::arg::as<i32>(joan::orig::as_unit(e)->hostile_to(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(q114)->id)))) != 0) {  // 0x005094d0
                            const i32 k = static_cast<i32>(jo::as_unit(q114)->anim_kind);
                            drop = jo::as_shot(q110)->type == 9 || k == 0x40 || k == 0x41 || k == 0x42 || k == 0x3c ||
                                   k == 0x29 || k == 0x4e;
                        }
                        if (drop) {
                            r10c = 0;
                            jo::view<i16>(jo::as_shot(p)->hit) = 0;
                        }
                    }
                }
            }
            V3 t3{0.0f, 0.0f, 0.0f};
            V3 v2f0 = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t3, joan::orig::arg::ptr(&jo::as_shot(p)->saved.pos_x), 46.1f));
            float f11c = 0.0f;  // written by 0x00693750 whenever it returns non-zero
            const u32 r120 = joan::orig::arg::as<u32>(joan::orig::as_collision_grid(world)->projectile_hit(joan::orig::arg::mptr(p), &v2f0, &v100, &f11c));  // 0x00693750
            if (r10c != 0) {
                if (r120 == 0 || !(f108 >= f11c)) {
                    if (joan::orig::arg::as<u8>(joan::orig::as_marker(p104)->impact(joan::orig::arg::i32(r10c))) != 0) return 0;  // 0x00514c40
                }
            }
            if (r120 != 0) {
                joan::orig::as_marker(p104)->hit_effects(joan::orig::arg::i32(r120));  // 0x004fbc90
                return 0;
            }
        }
        // 0x00429090 reads a box (min, max): the degenerate box {v144, v138} (the original keeps the point and its copy adjacent, [ebp-0x144] / [ebp-0x138]).
        const V3 pt{v60.x * gf(d::flt_46_1), v60.y * gf(d::flt_46_1), v60.z * gf(d::flt_46_1)};
        V3 v144_box[2] = {pt, pt};
        const V3& v144 = v144_box[0];
        const V3& v138 = v144_box[1];
        u32 f148 = 0;
        u8** const r14c = joan::orig::arg::as<u8**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(v144_box, &f148));  // 0x00429090
        jo::view<i16>(jo::as_body_state(p8)->on_ground) = 1;
        if (f148 != 0) {
            u8* const r150 = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(*r14c)->grid(joan::orig::arg::i16(i32{0})));  // 0x005600c0
            if (r150 != nullptr) {
                const Fpu fpu{true};
                const float f154 = static_cast<float>(fpu.div(
                    fpu.sub(static_cast<double>(v144.x), static_cast<double>(jo::as_world_cell(*r14c)->origin_x)),
                    static_cast<double>(jo::as_path_space(r150)->cell_w)));
                const float f158 = static_cast<float>(fpu.div(
                    fpu.sub(static_cast<double>(v144.y), static_cast<double>(jo::as_world_cell(*r14c)->origin_y)),
                    static_cast<double>(jo::as_path_space(r150)->cell_h)));
                const i32 i0 = ftol(static_cast<double>(f154));
                const i32 i1 = ftol(static_cast<double>(f158));
                const u32 even = ((static_cast<u32>(i0) + static_cast<u32>(i1)) & 1u) == 0 ? 1u : 0u;
                const float h = joan::orig::arg::as<float>(joan::orig::as_terrain(jo::view<void*>(jo::as_path_space(r150)->grid))->height_4c7b70(joan::orig::arg::f32(f154), joan::orig::arg::f32(f158), static_cast<char>(joan::orig::arg::i32(even))));  // 0x004c7b70
                if (!(h > v138.z)) jo::view<i16>(jo::as_body_state(p8)->on_ground) = 0;
            }
        }
        std::memcpy(&jo::as_shot(p)->state.pos_x, &v60, sizeof v60);
        std::memcpy(&jo::as_shot(p)->state.vec0_x, &v6c, sizeof v6c);
        jo::view<V4>(jo::as_body_state(p8)->orient) = v88;
        jo::view<V3>(jo::as_shot(p)->state.vec_c) = v78;
        return 1;
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_04", name, addr, fn)
B("Ballistic::step_6a8060", 0x006a8060, &H::ballistic_6a8060);
#undef B

}  // namespace game_b109_04
}  // namespace

// ---------------------------------------- from modules/game_b115_12.cpp (fix/uninit)
namespace {
namespace game_b115_12 {

using joan::orig::helpers::fbits;

constexpr img::Addr kHero(d::g_player_unit_recno);

JOAN_COUNTER(c_steer, "Unit::steer_6a9570");

i32 hero_id() { return joan::orig::arg::as<i32>(joan::orig::as_player(img::obj(kHero))->unit_id()); }                  // 0x004203c0

struct H {
    // -------------------------------------------------------------- 0x006a9570
    struct FV {
        float x, y, z;
    };
    static_assert(sizeof(FV) == 12, "sizes");
    static bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }  // fcomp; test ah,0x40
    static FV copy_ret(const void* p) {  // 3 dwords copied from a returned vector pointer
        FV v;
        std::memcpy(&v, p, sizeof v);
        return v;
    }
    static double crt_fabs(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }
    static i32 vec_ne(const void* a) { return static_cast<i32>(joan::orig::arg::as<u8>(joan::orig::vector_differs(joan::orig::arg::mptr(a), joan::orig::arg::mptr(obj(d::g_pos))))); }  // 0x0060cf40
    static void set_len(FV* v, float len) { joan::orig::vec_scale_in_place(joan::orig::arg::mptr(v), joan::orig::arg::f32(len)); }                                  // 0x0060ce80
    static float vlen(const void* v) { return v3d_float(d::iat_v3d_87_length, v); }
    i16 steer(u8* unit, u8* ctx, u8* p10, i32 a14, float* pos, float tx, float ty, float tz, FV* out, float step,
              i32 /*unused*/) {
        c_steer.hit();
        u8* const nav = reinterpret_cast<u8*>(&jo::as_shot(unit)->state);  // [ebp-0x14]
        if (eq_or_nan(pos[0], tx) && eq_or_nan(pos[1], ty)) {
            jo::as_shot(unit)->state.sub = p10;
            jo::view<i32>(jo::as_shot(unit)->state.zone) = a14;
            return 1;
        }
        u8* const zone = joan::orig::arg::as<u8*>(joan::orig::placements::find(joan::orig::arg::mptr(jo::as_players(ctx)->cells), joan::orig::arg::i32(jo::view<u32>(jo::as_players(ctx)->cell_count)), joan::orig::object<joan::orig::PathSpace>(p10)));  // 0x004a2b20
        if (zone == nullptr) return 0;
        u8* const path = joan::orig::arg::as<u8*>(joan::orig::as_billboard(jo::view<void*>(jo::as_path_space(p10)->zones))->item(joan::orig::arg::i32(a14)));  // 0x00637da0
        alignas(4) u8 obj30[8];
        joan::orig::as_point(obj30)->construct();  // 0x004eea60
        bool found = false;                  // [ebp-0x34]
        float best = 0.0f;                   // [ebp-0x38], read only once found
        FV e48 = {0.0f, 0.0f, 0.0f};         // [ebp-0x48], written with found
        FV e54 = {0.0f, 0.0f, 0.0f};         // [ebp-0x54]
        const float k = global<float>(d::flt_46_1);
        // placements::find 0x004a2b20 returns the RWorldCell (as uti3d/pathsp.cpp); +0x10 on x86 = its origin fx, fy
        const jo::RWorldCell* const cell = jo::as_rworld_cell(zone);
        const FV p = {pos[0] * k - cell->fx, pos[1] * k - cell->fy, pos[2] * k};
        const FV t = {tx * k - cell->fx, ty * k - cell->fy, tz * k};
        const i16 w70 = joan::orig::arg::as<i16>(joan::orig::as_body(unit)->side());  // 0x006b1dd0
        float f74 = 0.0f;
        {
            const i32 kind = jo::as_shot(unit)->type;
            if (kind == 9 || jo::as_shot(unit)->type == 0xa || jo::as_shot(unit)->type == 0xb || jo::as_shot(unit)->type == 0xe ||
                jo::as_shot(unit)->type == 0xf || jo::as_shot(unit)->type == 0x11) {
                const float m = !(jo::as_shot(unit)->extent[1] >= jo::as_shot(unit)->extent[0]) ? jo::as_shot(unit)->extent[0]
                                                                                    : jo::as_shot(unit)->extent[1];
                f74 = m / global<float>(d::flt_1);
            }
            if (jo::as_shot(unit)->type == 7 || jo::as_shot(unit)->type == 6) {
                const float m = !(jo::as_shot(unit)->extent[1] >= jo::as_shot(unit)->extent[0]) ? jo::as_shot(unit)->extent[0]
                                                                                    : jo::as_shot(unit)->extent[1];
                f74 = m / global<float>(d::flt_3);
            }
        }
        const float k0 = global<float>(d::k_9cf5c4);
        if (eq_or_nan(f74, k0)) {
            for (i32 i = 1; i <= jo::as_pfd_zone(path)->point_count; ++i) {
                const i32 ia = i - 1;
                const i32 ib = (i - jo::as_pfd_zone(path)->point_count) != 0 ? i : 0;
                const jo::PfdPoint* const pa = jo::as_pfd_zone(path)->points[ia].item;
                const FV va = {static_cast<float>(pa->xyz[0]), static_cast<float>(pa->xyz[1]),
                               static_cast<float>(pa->xyz[2])};
                const jo::PfdPoint* const pb = jo::as_pfd_zone(path)->points[ib].item;
                const FV vb = {static_cast<float>(pb->xyz[0]), static_cast<float>(pb->xyz[1]),
                               static_cast<float>(pb->xyz[2])};
                float a88 = 0.0f, a80 = 0.0f, a84 = 0.0f;
                if (joan::orig::tri_normal_2d(p.x, p.y, p.z, t.x, t.y, t.z, va.x, va.y, va.z, vb.x, vb.y, vb.z, &a88, &a80, &a84) == 0) continue;  // 0x00664c60
                if (!(a84 >= k0)) continue;
                if (a84 > a88) continue;
                if (a80 > a88) continue;
                if (found && !(a80 > best)) continue;
                found = true;
                best = a80;
                FV tmp1;
                e48 = copy_ret(joan::orig::arg::as<void*>(joan::orig::vec_add(&tmp1, &va, joan::orig::arg::ptr(&cell->fx))));  // 0x0060cc20
                FV tmp2;
                e54 = copy_ret(joan::orig::arg::as<void*>(joan::orig::vec_add(&tmp2, &vb, joan::orig::arg::ptr(&cell->fx))));
            }
        } else {
            FV b4 = {0.0f, 0.0f, 0.0f};  // written by 0x00671fb0 unless it returns early
            FV c0 = {0.0f, 0.0f, 0.0f};
            const bool closed = jo::as_pfd_zone(path)->blocked == 1;  // early "return 0" of 0x00671fb0 (last argument 0)
            const u8 r = joan::orig::arg::as<u8>(joan::orig::as_path(path)->check_671fb0(joan::orig::arg::bits(p.x), joan::orig::arg::bits(p.y), joan::orig::arg::bits(t.x), joan::orig::arg::bits(t.y), &b4, &c0, joan::orig::arg::f32(f74), joan::orig::arg::u32(i32{0})));  // 0x00671fb0
            if (r == 0 && !closed) {  // fix/uninit: the closed case is "no blocking edge" (header)
                found = true;
                FV tmp1;
                e48 = copy_ret(joan::orig::arg::as<void*>(joan::orig::vec_add(&tmp1, &b4, joan::orig::arg::ptr(&cell->fx))));
                FV tmp2;
                e54 = copy_ret(joan::orig::arg::as<void*>(joan::orig::vec_add(&tmp2, &c0, joan::orig::arg::ptr(&cell->fx))));
            }
        }
        FV dcc = {tx - pos[0], ty - pos[1], 0.0f};
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, &dcc);  // normalize in place
        FV dir = {0.0f, 0.0f, 0.0f};           // [ebp-0x24]
        const Fpu fpu{true};
        const double far_limit = img::global<const double>(d::dbl_0_021691974687620966);
        if (found) {
            FV d8 = {e48.x, e48.y, 0.0f};
            FV e4 = {e54.x, e54.y, 0.0f};
            FV tmp;
            FV n20c = copy_ret(joan::orig::arg::as<void*>(joan::orig::vec_sub(&tmp, &e4, &d8)));  // 0x0060cc90
            FV f0 = copy_ret(import_call<void*>(d::iat_v3d_87_vector_normalize_1, &n20c));
            const float f4 = v3d_float(d::iat_v3d_87_dot_product, &dcc, &f0);  // dot
            if (!(f4 >= k0)) {
                FV tmp3;
                f0 = copy_ret(joan::orig::arg::as<void*>(joan::orig::vec_negate(&tmp3, &f0)));  // 0x0060cbc0
            }
            import_call<void*>(d::iat_v3d_87_vector_normalize_2, &dir, &f0);  // normalize(out, in)
            if (vec_ne(jo::as_body_state(nav)->vel) != 0 && jo::view<float>(jo::as_body_state(nav)->vel_len) > k0) {
                set_len(&dir, vlen(jo::as_body_state(nav)->vel));
            } else if (vec_ne(jo::as_body_state(nav)->goal) != 0) {
                set_len(&dir, vlen(jo::as_body_state(nav)->goal));
            } else {
                set_len(&dir, vlen(jo::as_body_state(nav)->vel));
            }
            set_len(&dir, static_cast<float>(crt_fabs(static_cast<double>(f4))));
            if (vec_ne(&dir) != 0) {
                float q[3] = {dir.x * step + pos[0], dir.y * step + pos[1], dir.z * step + pos[2]};
                i32 n = 2;
                while (!(crt_fabs(static_cast<double>(q[0] - pos[0])) >= far_limit) &&
                       !(crt_fabs(static_cast<double>(q[1] - pos[1])) >= far_limit) && n < 100) {
                    q[0] = static_cast<float>(fpu.add(fpu.mul(static_cast<double>(dir.x * step), static_cast<double>(n)),
                                                      static_cast<double>(pos[0])));
                    q[1] = static_cast<float>(fpu.add(fpu.mul(static_cast<double>(dir.y * step), static_cast<double>(n)),
                                                      static_cast<double>(pos[1])));
                    q[2] = static_cast<float>(fpu.add(fpu.mul(static_cast<double>(dir.z * step), static_cast<double>(n)),
                                                      static_cast<double>(pos[2])));
                    ++n;
                }
                i32 grid[3] = {0, 0, 0};
                if (move_to(unit, ctx, nav, p10, a14, w70, f74, k, q, grid)) {
                    commit(unit, nav, out, dir, pos, q, grid);
                    if (crt_fabs(static_cast<double>(f4)) > img::global<const double>(d::dbl_0_707)) {
                        turn(unit, dir);
                    }
                    return 1;
                }
            }
        }
        // 0x006aa1c3: rotate the heading in growing steps until a free direction is found.
        float ang = fbits(0x3c23d70au);
        for (;;) {
            ang = -ang;
            if (ang > k0) ang = ang + global<float>(d::flt_0_2);
            const float lim = global<float>(d::g_pi_a6a4e8) / global<float>(d::flt_2);
            if (!(lim >= ang)) return 0;
            alignas(4) u8 rot[64];
            joan::orig::matrix_rotation(joan::orig::arg::mptr(rot), joan::orig::arg::f32(ang));  // 0x0060bc50
            if (vec_ne(jo::as_body_state(nav)->vel) != 0) {
                dir = jo::view<FV>(jo::as_body_state(nav)->vel);
            } else if (vec_ne(jo::as_body_state(nav)->goal) != 0) {
                dir = jo::view<FV>(jo::as_body_state(nav)->goal);
            } else {
                dir = img::global<const FV>(d::g_pos);
            }
            import_call<void*>(d::iat_v3d_87_vector_transform_2, &dir, rot);  // vector_transform
            FV v190;
            import_call<void*>(d::iat_v3d_87_vector_normalize_2, &v190, &dir);
            if (!(v3d_float(d::iat_v3d_87_dot_product, &dcc, &v190) >= k0)) continue;
            if (joan::orig::arg::as<u8>(joan::orig::vec_equal(&dir, joan::orig::arg::ptr(obj(d::g_pos)))) != 0) continue;  // 0x0060ced0
            float q[3] = {dir.x * step + pos[0], dir.y * step + pos[1], dir.z * step + pos[2]};
            const double r1 = fpu.div(far_limit, crt_fabs(static_cast<double>(dir.x * step)));
            const double r2 = fpu.div(far_limit, crt_fabs(static_cast<double>(dir.y * step)));
            double r = 0.0;
            if (r2 > r1) {
                r = fpu.div(far_limit, crt_fabs(static_cast<double>(dir.x * step)));
            } else {
                r = fpu.div(far_limit, crt_fabs(static_cast<double>(dir.y * step)));
            }
            const i32 steps = ftol(r);
            q[0] = static_cast<float>(fpu.add(fpu.mul(static_cast<double>(dir.x * step), static_cast<double>(steps)),
                                              static_cast<double>(pos[0])));
            q[1] = static_cast<float>(fpu.add(fpu.mul(static_cast<double>(dir.y * step), static_cast<double>(steps)),
                                              static_cast<double>(pos[1])));
            i32 grid[3] = {0, 0, 0};
            if (!move_to(unit, ctx, nav, p10, a14, w70, f74, k, q, grid)) continue;
            commit(unit, nav, out, dir, pos, q, grid);
            return 1;
        }
    }
    // 0x006a9e5c / 0x006aa43f: records the target and asks 0x006ae610 whether the cell is free;
    // the grid position (target * k, _ftol) is computed before the call and stored on success.
    static bool move_to(u8* unit, u8* ctx, u8* nav, u8* p10, i32 a14, i16 w70, float f74, float k, const float* q,
                        i32* grid) {
        jo::as_body_state(nav)->sub = p10;
        jo::view<i32>(jo::as_body_state(nav)->zone) = a14;
        grid[0] = ftol(static_cast<double>(q[0] * k));
        grid[1] = ftol(static_cast<double>(q[1] * k));
        grid[2] = ftol(static_cast<double>(q[2] * k));
        const i16 ok = joan::orig::arg::as<i16>(joan::orig::as_players(ctx)->find_slot_wrapper(&jo::as_body_state(nav)->sub, &jo::as_body_state(nav)->zone, joan::orig::arg::u32(static_cast<i32>(w70)), joan::orig::arg::u32(jo::view<u32>(jo::as_body_state(nav)->cell_pos[0])), joan::orig::arg::u32(jo::view<u32>(jo::as_body_state(nav)->cell_pos[1])), joan::orig::arg::u32(jo::view<u32>(jo::as_body_state(nav)->cell_pos[2])), joan::orig::arg::u32(jo::as_body_state(nav)->cell), joan::orig::arg::bits(q[0] * k), joan::orig::arg::bits(q[1] * k), joan::orig::arg::bits(q[2] * k), joan::orig::arg::bits(f74), joan::orig::arg::u32(jo::view<u32>(jo::as_shot(unit)->type))));  // 0x006ae610
        return ok != 0;
    }
    static void commit(u8* unit, u8* nav, FV* out, const FV& dir, float* pos, const float* q, const i32* grid) {
        jo::view<FV>(jo::as_body_state(nav)->vel) = dir;
        std::memcpy(out, img::obj(d::g_pos), 12);
        jo::view<i32>(jo::as_body_state(nav)->cell_pos[0]) = grid[0];
        jo::view<i32>(jo::as_body_state(nav)->cell_pos[1]) = grid[1];
        jo::view<i32>(jo::as_body_state(nav)->cell_pos[2]) = grid[2];
        jo::as_shot(unit)->u16_2ac = 1;
        pos[0] = q[0];
        pos[1] = q[1];
        pos[2] = q[2];
    }
    // 0x006a9fea: turns the unit (+8 -> +0xc object) towards the new heading; the hero also
    // updates the camera yaw (0x00563c80 / 0x00423fe0 / 0x00423f40).
    static void turn(u8* unit, FV dir) {
        u8* const o108 = jo::view<u8*>(jo::as_shot(unit)->actor->owner);
        dir.z = 0.0f;
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, &dir);
        alignas(4) u8 a10c[4], a110[4], a118[4], t234[4], t238[4], t23c[4];
        joan::orig::as_angle(a10c)->construct_577190();  // 0x00577190
        joan::orig::as_angle(a10c)->from_xy(joan::orig::arg::f64(static_cast<double>(dir.x)), joan::orig::arg::f64(static_cast<double>(dir.y)));  // 0x00577b90
        joan::orig::as_angle(a110)->construct_577190();
        joan::orig::as_angle(a110)->set_wrapped(joan::orig::arg::f32(jo::view<u32>(jo::as_unit(o108)->heading)));  // 0x00577340
        float d = joan::orig::arg::as<float>(joan::orig::as_angle(joan::orig::arg::as<void*>(joan::orig::as_turn(a10c)->sub(joan::orig::arg::mptr(t234), joan::orig::arg::mptr(a110))))->degrees());  // 0x005775b0 / 0x00577450
        if (d > global<float>(d::flt_180)) d = d - global<float>(d::flt_360);
        if (!(d >= global<float>(d::flt_m180))) d = d + global<float>(d::flt_360);
        joan::orig::as_angle(a118)->construct_577190();
        const u32 sign = d > global<float>(d::k_9cf5c4) ? 0x40000000u : 0xbf800000u;
        joan::orig::as_angle(a118)->set_degrees(joan::orig::arg::f32(sign));  // 0x005773e0
        const float yaw = joan::orig::arg::as<float>(joan::orig::as_angle(joan::orig::arg::as<void*>(joan::orig::as_angle(a110)->add(joan::orig::arg::mptr(t238), joan::orig::arg::mptr(a118))))->degrees());  // 0x00577560
        joan::orig::as_unit(o108)->set_heading(joan::orig::arg::f32(yaw));                                                         // 0x0050abd0
        if (static_cast<i32>(jo::as_unit(o108)->id) != hero_id()) return;
        // The pad's right stick holds the camera (joystick type, Ctrl::update_432d30): the hero sliding along a wall
        // turns, the camera stays where the stick left it (the keyboard types keep the camera behind the hero).
        if (global<u32>(d::g_now) - joan::padfx::g_camera_hold.ms <= 100u && joan::padfx::g_camera_hold.motion != nullptr) return;
        alignas(4) u8 cam[0x28];
        const std::int32_t view = joan::orig::arg::as<std::int32_t>(joan::orig::as_camera_follow(obj(d::g_camera_man))->target());         // 0x00563dc0 (the current motion index)
        void* const c1 = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(view));     // 0x00563c80
        joan::orig::as_camera_motion(c1)->end(joan::orig::arg::mptr(cam));                                     // 0x00423fe0
        const float v = joan::orig::arg::as<float>(joan::orig::as_angle(joan::orig::arg::as<void*>(joan::orig::as_angle(a110)->add(joan::orig::arg::mptr(t23c), joan::orig::arg::mptr(a118))))->degrees());
        jo::self<jo::CamPose>(cam)->target[0] = v * global<float>(d::flt_0_017453292);
        void* const c2 = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(i32{0}));
        joan::orig::as_camera_motion(c2)->retarget(joan::orig::arg::mptr(cam));  // 0x00423f40
    }
};

// 0x006a9570: 0x00671fb0's early "return 0" (closed path) leaves its out vectors unwritten; it is
// treated as "no blocking edge".
JOAN_REPLACE("fix/uninit", "Unit::steer_6a9570", 0x006a9570, &H::steer);

}  // namespace game_b115_12
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

struct H {
    float hit_factor(void* unit, u32 flag) {  // 0x006aa9e0
        const i32 kind = jo::view<i16>(jo::as_shot(unit)->tag);
        if (kind == 1 || kind == 2) {
            return static_cast<i16>(flag) != 0 ? global<float>(d::flt_0_9) : global<float>(d::flt_1);
        }
        return global<float>(d::k_9cf5c4);
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Unit::hit_factor", 0x006aa9e0, &H::hit_factor);
#undef B94

}  // namespace game_b94_08
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
B("init_6aaa60", 0x006aaa60, &static_init<fn::init_a6a4e8>);
B("init_6aaac0", 0x006aaac0, &static_init<fn::init_a6a4ec>);
B("init_6aab10", 0x006aab10, &static_init<fn::Globals_div_a6a4e8>);
B("init_6aab60", 0x006aab60, &static_init<fn::scale_6aab80>);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

static float pi_from_atan() {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(t * 4.0);                     // qword 0x9cf788 = 4.0
}
static void JOAN_CDECL init_a6a4e8() { global<float>(d::g_pi_a6a4e8) = pi_from_atan(); }  // 0x006aaa80

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("init_a6a4e8", 0x006aaa80, &init_a6a4e8);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

// 0x006aaae0
void JOAN_CDECL init_a6a4ec() { global<float>(d::g_a6a4ec) = global<float>(d::g_pi_a6a4e8) / global<float>(d::flt_2); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("init_a6a4ec", 0x006aaae0, &init_a6a4ec);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

void JOAN_CDECL div_a6a4e8() {  // 0x006aab30
    global<float>(d::g_a6a4f0) = global<float>(d::g_pi_a6a4e8) / global<float>(d::flt_3);
}

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("Globals::div_a6a4e8", 0x006aab30, &div_a6a4e8);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

static void JOAN_CDECL scale_6aab80() { global<float>(d::g_a6a4f4) = global<float>(d::g_pi_a6a4e8) / global<float>(d::flt_4); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("scale_6aab80", 0x006aab80, &scale_6aab80);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

struct H {
    // ------------------------------------------------------------------ 0x006aabb0
    float field_b16() { return jo::self<jo::Players>(this)->time_scale_; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::field_b16", 0x006aabb0, &H::field_b16);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

struct H {
    // ------------------------------------------------------------------ 0x006aabe0
    u32 word_b1a() { return (joan::call::register_low_bits(this) & 0xffff0000u) | static_cast<u32>(jo::view<u16>(jo::self<jo::Players>(this)->i16_b1a)); }  // eax: upper half of this; its only caller (formmove.cpp:392) keeps ax
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj::word_b1a", 0x006aabe0, &H::word_b1a);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MoveSolver::form_step(void* a0, void* a1, std::uint32_t a2, float a3, void* a4) -> void { joan::bind::member<&::game_b110_05::H::step_6a45a0>(this, a0, a1, a2, a3, a4); }  // 0x6a45a0
auto joan::orig::MoveSolver::ballistic_step(void* a0, void* a1, std::uint32_t a2, float a3, void* a4) -> std::uint16_t { return joan::bind::conv<std::uint16_t>(joan::bind::member<&::game_b109_04::H::ballistic_6a8060>(this, a0, a1, a2, a3, a4)); }  // 0x6a8060
auto joan::orig::Unit::steer_6a9570(void* a0, void* a1, void* a2, std::int32_t a3, void* a4, float a5, float a6, float a7, void* a8, float a9, std::int32_t a10) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b115_12::H::steer>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)); }  // 0x6a9570
auto joan::orig::Unit::hit_factor(void* a0, std::uint32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b94_08::H::hit_factor>(this, a0, a1)); }  // 0x6aa9e0
auto joan::orig::Players::time_scale() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b99_10::H::field_b16>(this)); }  // 0x6aabb0
auto joan::orig::Players::word_b1a() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_08::H::word_b1a>(this)); }  // 0x6aabe0
