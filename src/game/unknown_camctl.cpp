// game/unknown_camctl.cpp: functions whose original source file is unknown (formerly game/_unplaced/camctl.cpp;
// re/attribution.csv: between ww/world_s.cpp and ww/ilib/angle.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../interfac/view_range.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using i32 = std::int32_t;

using u8 = std::uint8_t;

using joan::img::obj;

using joan::hook::x87::Fpu;

using joan::call::stdcall_call;

using joan::orig::helpers::at;

}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_37() {  // 0x00561560
        memset_(this, 0, static_cast<u32>(sizeof(jo::CameraMan)));  // 0x37 bytes on x86
        return this;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("Struct37::zero", 0x00561560, &H::zero_37);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

using joan::call::thiscall;

// ------------------------------------------------------------------------------- thiscall
struct H {
    // 0x00561590
    u32 call_5624c0() { joan::orig::as_camera_man(this)->release(); return 0; }  // 0x005624c0 (void; atexit handler, result ignored)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("Obj::call_5624c0", 0x00561590, &H::call_5624c0);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b109_15.cpp (game/b109_15)
namespace {
namespace game_b109_15 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using joan::hook::x87::ftol;

constexpr u32 kOneF = 0x3f800000;  // 1.0f

JOAN_COUNTER(c_cutinit, "CutMan::init_5615c0");

struct H {
    // ------------------------------------------------------------------ 0x005615c0
    void slider(img::Addr slot, i32 y, i32 max, std::int32_t factor, bool percent) {
        void* m = joan::crt::operator_new(static_cast<u32>(sizeof(jo::Scroll)));  // the slider is a Scroll (0x92 on x86)
        void* s = m != nullptr ? joan::orig::as_slider(m)->construct() : nullptr;  // 0x0062b6c0
        global<void*>(slot) = s;
        joan::orig::as_text_box(global<void*>(slot))->init(joan::orig::arg::i16(i32{0x64}), joan::orig::arg::i16(y), joan::orig::arg::i16(i32{0xc8}), joan::orig::arg::i16(y + 0xa), joan::orig::arg::i16(i32{5}), // 0x0062b8d0
                      fn::code_value(fn::Button_draw_frame_cb), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})));
        joan::orig::as_scroll(global<void*>(slot))->set_62be10(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(max), joan::orig::arg::u32(i32{1}));  // 0x0062be10
        // the original reads field20() (0x0047b5e0, = this+0x20) + 0x30..0x5c: slider_factors[factor]
        jo::VsCache* const cache = global<jo::VsCache*>(d::g_vs_cache);
        void* const sl = global<void*>(slot);
        i32 v;
        if (!percent) {
            v = ftol(Fpu{true}.mul(static_cast<double>(jo::as_scroll(sl)->max), static_cast<double>(cache->slider_factors[factor])));
        } else {
            const Fpu fpu{true};
            const float p = cache->slider_factors[factor] + global<float>(d::flt_1);  // + 1.0f
            const double m2 = fpu.mul(static_cast<double>(p), static_cast<double>(jo::as_scroll(sl)->max));
            v = ftol(fpu.div(m2, static_cast<double>(global<float>(d::flt_2))));  // / 2.0f
        }
        joan::orig::as_scroll(global<void*>(slot))->set_pos(joan::orig::arg::i32(v));  // 0x0062be50
    }

    void cut_init() {
        c_cutinit.hit();
        joan::orig::as_camera_man(this)->release();  // 0x005624c0
        {
            void* m = joan::crt::operator_new(static_cast<u32>(sizeof(jo::Camera)));  // 0x4ca on x86
            void* cam = m != nullptr ? joan::orig::as_camera(m)->construct() : nullptr;  // 0x00613650
            jo::self<jo::CameraFollow>(this)->camera = static_cast<jo::Camera*>(cam);
        }
        joan::orig::as_sys_en(jo::self<jo::CameraFollow>(this)->camera)->init_613730();  // 0x00613730
        joan::orig::as_camera(jo::self<jo::CameraFollow>(this)->camera)->set_viewport(joan::orig::as_surface(obj(d::g_vga_back)), obj(d::g_a56ef0), joan::orig::arg::u32(// 0x00613bf0
                      global<u32>(d::g_a48880)), joan::orig::arg::u32(global<u32>(d::g_a48884)), joan::orig::arg::u32(global<u32>(d::g_a48888)), joan::orig::arg::u32(global<u32>(d::g_a4888c)));
        joan::orig::as_camera(jo::self<jo::CameraFollow>(this)->camera)->set_scene(jo::as_world(global<void*>(d::g_world))->scene);  // 0x00613db0
        joan::orig::as_camera(jo::self<jo::CameraFollow>(this)->camera)->set_ranges(joan::orig::arg::f32(u32{0x40800000}), joan::orig::arg::f32(u32{0x45800000}), joan::orig::arg::f32(u32{0x46800000}));  // 0x00613ca0
        {
            const u32 fog = 0xff74b7feu;  // bytes fe b7 74 ff
            if (global<u32>(d::g_a52623) != 0) {
                joan::orig::as_camera(jo::self<jo::CameraFollow>(this)->camera)->set_cf(joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(fog));  // 0x00613d30
            } else {
                joan::orig::as_camera(jo::self<jo::CameraFollow>(this)->camera)->set_cf(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(fog));
            }
        }
        jo::view<u32>(jo::self<jo::CameraFollow>(this)->height) = 0x45800000;
        jo::view<u32>(jo::self<jo::CameraFollow>(this)->target_distance) = 0x45800000;
        {
            u8 white[4] = {0xff, 0xff, 0xff, 0xff};
            joan::orig::as_camera(jo::self<jo::CameraFollow>(this)->camera)->set_fog_colour(joan::orig::arg::mptr(static_cast<void*>(white)));  // 0x00616c10
        }
        joan::orig::as_viewport(jo::self<jo::CameraFollow>(this)->camera)->set_58(i32{2});  // 0x00616a60
        {
            const float far_plane = jo::self<jo::CameraFollow>(this)->height + global<float>(d::flt_500);  // + 500.0f
            joan::orig::as_viewport(jo::self<jo::CameraFollow>(this)->camera)->set_size(joan::orig::arg::i32(u32{0x44610000}), joan::orig::arg::bits(far_plane));  // 0x00616a00
        }
        joan::orig::as_camera(jo::self<jo::CameraFollow>(this)->camera)->set_light(joan::orig::arg::mptr(i32{1}));  // 0x006169d0
        joan::orig::as_camera(jo::self<jo::CameraFollow>(this)->camera)->set_fog_range(joan::orig::arg::u32(u32{0x3c23d70a}), joan::orig::arg::u32(u32{0x41a00000}), joan::orig::arg::u32(i32{0}));  // 0x00616a90
        jo::view<u32>(jo::self<jo::CameraFollow>(this)->height) = 0x45800000;
        jo::view<u32>(jo::self<jo::CameraFollow>(this)->target_distance) = 0x45800000;
        joan::optmenu::clamp_view_ranges();  // widened draw distance range (interfac/view_range.hpp)
        jo::self<jo::CameraFollow>(this)->far_limit = static_cast<float>(global<i32>(d::g_cfg_far_limit));
        jo::view<u32>(jo::self<jo::CameraFollow>(this)->key_move[2]) = 1;
        jo::view<u32>(jo::self<jo::CameraFollow>(this)->key_move[1]) = 1;
        jo::view<u32>(jo::self<jo::CameraFollow>(this)->key_move[0]) = 1;
        if (global<u32>(d::g_a52623) != 0 && global<void*>(d::g_vs_cache) != nullptr) {
            slider(d::g_a39a18, 0x32, 0x64, 7, false);
            slider(d::g_a39a1c, 0x46, 0x64, 8, false);
            slider(d::g_a39a20, 0x5a, 0x64, 9, false);
            slider(d::g_a39a24, 0x6e, 0x64, 10, false);
            slider(d::g_a39a28, 0x82, 0x64, 11, false);
            slider(d::g_a39a2c, 0xa0, 0x64, 3, false);
            slider(d::g_a39a30, 0xb4, 0x64, 4, false);
            slider(d::g_a39a34, 0xc8, 0x64, 5, false);
            slider(d::g_a39a38, 0xe6, 0xc8, 0, true);
            slider(d::g_a39a3c, 0xfa, 0xc8, 1, true);
            slider(d::g_a39a40, 0x10e, 0xc8, 2, true);
        }
        if (jo::self<jo::CameraFollow>(this)->motions == nullptr) {
            u8* block = static_cast<u8*>(joan::crt::operator_new(static_cast<u32>(4 + 6 * sizeof(jo::CameraMotion))));  // 0x4b4 on x86
            u8* arr = nullptr;
            if (block != nullptr) {
                const u32 count_cookie = 6;  // the new[] element count cookie (u32 on x86), not an object member
                std::memcpy(block, &count_cookie, sizeof(count_cookie));
                joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(block + 4)), static_cast<u32>(sizeof(jo::CameraMotion)), i32{6},  // eh vector ctor (0xc8-byte elements on x86)
                                   fn::code_value(fn::Obj_clear_c8), fn::code_value(fn::Obj4235d0_dtor));
                arr = block + 4;
            }
            jo::self<jo::CameraFollow>(this)->motions = reinterpret_cast<jo::CameraMotion*>(arr);
        }
        u32 key[10];
        joan::orig::as_camera_params(static_cast<void*>(key))->construct();  // 0x00616d50
        key[8] = kOneF;  // [ebp-0x20]
        for (i32 i = 0; i < 6; ++i) {
            joan::orig::as_camera_motion(&jo::self<jo::CameraFollow>(this)->motions[i])->init();  // 0x00423600
            joan::orig::as_camera_motion(&jo::self<jo::CameraFollow>(this)->motions[i])->start(joan::orig::arg::mptr(static_cast<void*>(key)), joan::orig::arg::mptr(static_cast<void*>(key)));  // 0x00423ec0
        }
        jo::self<jo::CameraFollow>(this)->follow_target = 3;
        void* const e0 = jo::self<jo::CameraFollow>(this)->motions;
        jo::as_camera_motion(e0)->skip_collision = 0;
        jo::view<u32>(jo::as_camera_motion(e0)->collide_[0]) = 0x3edf66f3;
        jo::view<u32>(jo::as_camera_motion(e0)->collide_[2]) = 0x42c80000;
        jo::view<u32>(jo::as_camera_motion(e0)->collide_[1]) = 0x41f00000;
        jo::view<u32>(jo::as_camera_motion(e0)->collide_[3]) = 0x3f490fdb;
        jo::view<u32>(jo::as_camera_motion(e0)->collide_[5]) = 0x41200000;
        jo::view<u32>(jo::as_camera_motion(e0)->collide_[4]) = 0x41c80000;
        jo::self<jo::CameraFollow>(this)->active = 1;
    }

    // ------------------------------------------------------------------ 0x00751750
    struct SlideFrame {
        char text[0x20];  // [ebp-0x34]
        i32 i;            // [ebp-0x14]
        i32 count;        // [ebp-0x10]
        i32 j;            // [ebp-0xc]
        i32 dy;           // [ebp-0x8]
        void* self;       // [ebp-0x4]
    };
#if UINTPTR_MAX == 0xffffffffu  // the original (x86) frame
    static_assert(sizeof(SlideFrame) == 0x34, "slide frame");
#endif
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_15", name, addr, fn)
B("CutMan::init_5615c0", 0x005615c0, &H::cut_init);
#undef B

}  // namespace game_b109_15
}  // namespace

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
namespace {
namespace game_b84_02 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;                      // operator delete

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // 0x00562910 scalar deleting destructor (dtor 0x00613700), ret 4.
    void* sdd_562910(std::uint32_t flags) {
        joan::orig::as_camera(this)->destruct();  // 0x00613700
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
    // 0x00562960 scalar deleting destructor (dtor 0x0062b7d0), ret 4.
    void* sdd_562960(std::uint32_t flags) {
        joan::orig::as_slider(this)->destruct();  // 0x0062b7d0
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
    // 0x00562420 vector deleting destructor (0xc8-byte elements, dtor 0x004235d0), ret 4.
    void* vdd_562420(std::uint32_t flags) {
        if ((flags & 2) != 0) {
            joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(this)), static_cast<std::uint32_t>(sizeof(jo::CameraMotion)), joan::orig::arg::i32(f<std::int32_t>(-4))  /* x64: the new[] count cookie before the array */, fn::code_value(fn::Obj4235d0_dtor));  // eh vector dtor
            if ((flags & 1) != 0) game_delete(reinterpret_cast<std::uint8_t*>(this) - 4);
            return reinterpret_cast<std::uint8_t*>(this) - 4;
        }
        joan::orig::as_camera_motion(this)->destruct();  // 0x004235d0
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("Obj562420::vector_deleting_dtor", 0x00562420, &H::vdd_562420);
B84("Obj562960::scalar_deleting_dtor", 0x00562960, &H::sdd_562960);
B84("Obj562910::scalar_deleting_dtor", 0x00562910, &H::sdd_562910);
#undef B84

}  // namespace game_b84_02
}  // namespace

// ---------------------------------------- from modules/game_b83_02.cpp (game/b83_02)
namespace {
namespace game_b83_02 {

using joan::call::thiscall;

using joan::hook::global;

JOAN_COUNTER(c_release_tex, "Water::release_textures");

struct H {
    // 0x005624c0 Water::release_textures (registry name; this = the CameraMan g_a48cc0: CameraFollow::active /
    // motions / camera).
    void release_textures() {
        c_release_tex.hit();
        if (jo::self<jo::CameraMan>(this)->active != 0) {
            void* const a = jo::self<jo::CameraMan>(this)->motions;
            if (a != nullptr) joan::orig::as_camera_motion(a)->vector_delete(3u);  // 0x00562420
            jo::self<jo::CameraMan>(this)->motions = nullptr;
            void* const b = jo::self<jo::CameraMan>(this)->camera;
            if (b != nullptr) joan::orig::as_camera(b)->destroy(1u);  // 0x00562910
            jo::self<jo::CameraMan>(this)->camera = nullptr;
            for (img::Addr g = d::g_a39a18; g <= d::g_a39a40; g += 4) {
                if (global<void*>(g) != nullptr) {
                    void* const t = global<void*>(g);
                    if (t != nullptr) joan::orig::as_scroll(t)->destroy(1u);  // 0x00562960
                    global<void*>(g) = nullptr;
                }
            }
        }
        jo::self<jo::CameraMan>(this)->active = 0;
    }
};

#define B83(name, addr, fn) JOAN_REPLACE("game/b83_02", name, addr, fn)
B83("Water::release_textures", 0x005624c0, &H::release_textures);
#undef B83

}  // namespace game_b83_02
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using joan::call::thiscall;

using i16 = std::int16_t;
using u16 = std::uint16_t;

using joan::hook::global;
using i8 = std::int8_t;

struct Xform {  // 0x28-byte camera transform copied by 0x00423fb0 / 0x00423ec0 / 0x00423f40
    float v[10];
};
static_assert(sizeof(Xform) == 0x28, "Xform");

JOAN_COUNTER(c_camkeys, "CamCtl::update_keys");

struct H {
    // ------------------------------------------------------------------------ 0x00562f40
    void* cam(i32 i) { return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(this)->motion(joan::orig::arg::i32(i))); }  // 0x00563c80
    void camkeys() {
        c_camkeys.hit();
        if (jo::self<jo::CameraFollow>(this)->active == 0) return;
        for (i32 i = 0; i < 6; ++i) {
            if (i == 0) continue;
            Xform t;
            joan::orig::as_camera_motion(cam(i))->current(&t);      // 0x00423fb0
            joan::orig::as_camera_motion(cam(i))->start(&t, &t);  // 0x00423ec0
        }
        joan::orig::as_player(obj(d::g_player_unit_recno))->on_key_42e1a0();  // 0x0042e1a0
        if (global<i32>(d::g_key) == 0x5700) {
            jo::view<u32>(jo::self<jo::CameraFollow>(this)->key_move[0]) = jo::view<u32>(jo::self<jo::CameraFollow>(this)->key_move[0]) == 0 ? 1u : 0u;
            jo::view<u32>(jo::self<jo::CameraFollow>(this)->key_move[1]) = jo::view<u32>(jo::self<jo::CameraFollow>(this)->key_move[1]) == 0 ? 1u : 0u;
            jo::view<u32>(jo::self<jo::CameraFollow>(this)->key_move[2]) = jo::view<u32>(jo::self<jo::CameraFollow>(this)->key_move[2]) == 0 ? 1u : 0u;
            return;
        }
        if (global<i8>(d::g_cheat_keys_enabled) != 0) {
            if (global<i32>(d::g_key) == 0x66) {
                if (jo::view<i32>(jo::self<jo::CameraFollow>(this)->follow_target) == 3) {
                    joan::orig::as_camera_man(this)->restore_mode();  // 0x004297d0
                } else {
                    joan::orig::as_camera_man(this)->set_mode(3);  // 0x00429860
                    Xform t;
                    joan::orig::as_camera(jo::view<void*>(jo::self<jo::CameraFollow>(this)->camera))->frame(&t);  // 0x006140a0
                    joan::orig::as_camera_motion(cam(3))->start(&t, &t);      // 0x00423ec0
                }
                return;
            }
            if (global<i32>(d::g_key) == 0x5d) {
                jo::self<jo::CameraFollow>(this)->far_limit = jo::self<jo::CameraFollow>(this)->far_limit + global<float>(d::flt_1000);
            } else if (global<i32>(d::g_key) == 0x5b) {
                jo::self<jo::CameraFollow>(this)->far_limit = jo::self<jo::CameraFollow>(this)->far_limit - global<float>(d::flt_1000);
                if (!(jo::self<jo::CameraFollow>(this)->far_limit >= global<float>(d::flt_1000))) jo::view<u32>(jo::self<jo::CameraFollow>(this)->far_limit) = 0x447a0000u;  // 1000.0f
            }
        }
        if (jo::view<i32>(jo::self<jo::CameraFollow>(this)->follow_target) != 3) return;
        i16 step = 1;
        if ((global<u16>(d::g_key_modifiers) & 0xcu) != 0) step = static_cast<i16>(step * 5);
        if ((global<u16>(d::g_key_modifiers) & 0x30u) != 0) step = static_cast<i16>(step * 0x14);
        Xform t;
        void* const holder = &jo::self<jo::CameraFollow>(this)->motions[3];
        joan::orig::as_camera_motion(holder)->current(&t);  // 0x00423fb0
        const Fpu fpu{true};
        const double s = static_cast<double>(static_cast<i32>(step));
        if (global<u16>(d::g_rts_scroll_bits) != 0) {
            if ((global<u16>(d::g_key_modifiers) & 3u) == 0) {
                const double k = global<float>(d::flt_0_017453292);
                if ((global<u16>(d::g_rts_scroll_bits) & 1u) != 0) {
                    t.v[4] = static_cast<float>(fpu.add(fpu.mul(s, k), t.v[4]));
                    if (t.v[4] > global<float>(d::flt_3_1415927)) t.v[4] = t.v[4] - global<float>(d::flt_6_2831855);
                }
                if ((global<u16>(d::g_rts_scroll_bits) & 2u) != 0) {
                    t.v[4] = static_cast<float>(fpu.sub(t.v[4], fpu.mul(s, k)));
                    if (!(t.v[4] >= global<float>(d::flt_m3_1415927))) t.v[4] = t.v[4] + global<float>(d::flt_6_2831855);
                }
                if ((global<u16>(d::g_rts_scroll_bits) & 4u) != 0) {
                    t.v[5] = static_cast<float>(fpu.sub(t.v[5], fpu.mul(s, k)));
                    if (!(t.v[5] >= global<float>(d::flt_m3_1415927))) t.v[5] = t.v[5] + global<float>(d::flt_6_2831855);
                }
                if ((global<u16>(d::g_rts_scroll_bits) & 8u) != 0) {
                    t.v[5] = static_cast<float>(fpu.add(fpu.mul(s, k), t.v[5]));
                    if (t.v[5] > global<float>(d::flt_3_1415927)) t.v[5] = t.v[5] - global<float>(d::flt_6_2831855);
                }
            } else {
                const float neg = static_cast<float>(-static_cast<i32>(step));
                const float pos = static_cast<float>(static_cast<i32>(step));
                if ((global<u16>(d::g_rts_scroll_bits) & 1u) != 0) joan::orig::as_camera_params(&t)->rotate(joan::orig::arg::f32(neg), 0.0f);  // 0x00616dd0
                if ((global<u16>(d::g_rts_scroll_bits) & 2u) != 0) joan::orig::as_camera_params(&t)->rotate(joan::orig::arg::f32(pos), 0.0f);
                if ((global<u16>(d::g_rts_scroll_bits) & 4u) != 0) joan::orig::as_camera_params(&t)->rotate(0.0f, joan::orig::arg::f32(neg));
                if ((global<u16>(d::g_rts_scroll_bits) & 8u) != 0) joan::orig::as_camera_params(&t)->rotate(0.0f, joan::orig::arg::f32(pos));
            }
        }
        const i32 key2 = global<i32>(d::g_a51db5);
        if (key2 == 0x61)
            t.v[3] = static_cast<float>(fpu.add(s, t.v[3]));
        else if (key2 == 0x7a)
            t.v[3] = static_cast<float>(fpu.sub(t.v[3], s));
        if (global<i32>(d::g_key) == 0x72) joan::orig::as_camera_params(&t)->construct();  // 0x00616d50
        joan::orig::as_camera_motion(&jo::self<jo::CameraFollow>(this)->motions[3])->retarget(&t);                     // 0x00423f40
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("CamCtl::update_keys", 0x00562f40, &H::camkeys);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;


JOAN_COUNTER(c_camera_5634e0, "Camera::update_5634e0");

constexpr img::Addr kUnits = d::g_units;  // unit table (0x00428b70 check, 0x0041b3c0 get)

struct H {
    // -------------------------------------------------------------- 0x005634e0
    void camera_update() {
        c_camera_5634e0.hit();
        if (jo::self<jo::CameraFollow>(this)->active == 0) return;
        joan::orig::as_player(obj(d::g_player_unit_recno))->tick();  // 0x0042f700
        joan::orig::as_light_sys(obj(d::g_light_sys))->tick_weather(jo::as_world(global<void*>(d::g_world))->scene);  // 0x004a7f70
        joan::orig::as_cloud_layers(obj(d::g_cloud_layers))->update_position(joan::orig::arg::mptr(jo::self<jo::CameraFollow>(this)->camera));                                  // 0x00459c90
        auto node = [this]() { return jo::self<jo::CameraFollow>(this)->camera; };  // re-read at every use, as the original
        jo::CamParams view;
        std::memcpy(&view, &node()->params, sizeof view);
        jo::view<u32>(view.orbit[3]) = 0;  // mov [ebp-0x10],0 lands inside the copy ([ebp-0x2c] + 0x1c); 0x005a0110 reads it
        if (global<void*>(d::g_sound3d_listener) != nullptr) joan::orig::as_sound3dlistener(global<void*>(d::g_sound3d_listener))->set_from_view(joan::orig::arg::mptr(&view));  // 0x005a0110
        if (jo::self<jo::CameraFollow>(this)->light != nullptr && joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) == 0) {
            void* const u = joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));
            jo::view<u32>(jo::self<jo::CameraFollow>(this)->light->position[0]) = jo::view<u32[3]>(jo::as_unit(u)->position_)[0];
            jo::view<u32>(jo::self<jo::CameraFollow>(this)->light->position[1]) = jo::view<u32[3]>(jo::as_unit(u)->position_)[1];
            jo::self<jo::CameraFollow>(this)->light->position[2] = jo::view<float[3]>(jo::as_unit(u)->position_)[2] + global<float>(d::flt_9cfbe0);
            joan::orig::as_light(jo::self<jo::CameraFollow>(this)->light)->invalidate();  // 0x005c5c00
        }
        const float ticks = static_cast<float>(static_cast<std::int64_t>(global<u32>(d::g_frame_step_ms)));
        const double clamped =
            joan::orig::arg::as<double>(joan::orig::as_misc(obj(d::g_misc))->limit_double(joan::orig::arg::f64(static_cast<double>(ticks)), joan::orig::arg::f64(20.0), joan::orig::arg::f64(50.0)));  // 0x00590b00
        const float step = static_cast<float>(clamped) / global<float>(d::flt_1000) * global<float>(d::flt_500);
        float target = jo::self<jo::CameraFollow>(this)->target_distance;
        const float g = global<float>(d::g_calendar_minute_of_day);
        const u8 wide = (g > global<float>(d::flt_1080) || !(g > global<float>(d::flt_360))) ? 1 : 0;
        u8 zoom = 1;
        if ((joan::orig::arg::as<u32>(joan::orig::as_bytes(obj(d::g_weather_system))->at(i32{2})) & 0xffu) == 0 &&  // 0x0041bf10
            (joan::orig::arg::as<u32>(joan::orig::as_bytes(obj(d::g_weather_system))->at(i32{1})) & 0xffu) == 0)
            zoom = 0;
        if (wide != 0) target = target * global<float>(d::flt_0_7);
        if (zoom != 0) target = target * global<float>(d::flt_0_7);
        u32 limit_bits = jo::view<u32>(jo::self<jo::CameraFollow>(this)->far_limit);  // [ebp-0x48]: raw dword copy, pushed raw to 0x00613ca0
        float limit = 0.0f;
        std::memcpy(&limit, &limit_bits, 4);
        // Fog (start = pitch, end = camera height) and the far plane it cuts to (height + 200) scale with the draw
        // distance (interfac/view_range.hpp): 1x at its minimum, where they sit as in the original whatever its draw
        // distance setting, up to 4x at its maximum. Without this the fog hides everything past ~4096.
        const float fog_k = limit / static_cast<float>(joan::optmenu::kDrawMin);
        const float top = limit - global<float>(d::flt_200);
        if (top > target) {
            // keep target
        } else {
            target = limit - global<float>(d::flt_200);
        }
        float& height = jo::self<jo::CameraFollow>(this)->height;
        if (target > height) {
            if (!(height + global<float>(d::flt_100) >= target)) height = step + height;
        } else if (!(target >= height)) {
            if (height - global<float>(d::flt_100) > target) height = height - step;
        }
        if (jo::as_camera(node())->light_ != nullptr) {
            if (!(height * fog_k + global<float>(d::flt_200) >= limit)) {
                limit = height * fog_k + global<float>(d::flt_200);  // the original: height + 200
                std::memcpy(&limit_bits, &limit, 4);
            }
            if (!(limit >= height)) height = limit - global<float>(d::flt_200);
        }
        const double diff = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(jo::as_camera(node())->far_perspective - limit)));  // 0x00427d60
        if (diff >= 1.0) {  // fcomp + test ah,1 jne: skipped when below or unordered
            joan::orig::as_camera(node())->set_ranges(joan::orig::arg::f32(u32{0x40800000}), joan::orig::arg::f32(limit_bits), joan::orig::arg::f32(u32{0x46800000}));  // 0x00613ca0
        }
        float pitch;
        if (height >= global<float>(d::flt_2000)) {  // test ah,1 jne: below or unordered computes
            pitch = 900.0f;  // 0x44610000
        } else {
            pitch = global<float>(d::flt_900) - (global<float>(d::flt_2000) - height) * global<float>(d::flt_0_5);
            if (!(pitch >= global<float>(d::flt_1))) pitch = 1.0f;
        }
        if (wide != 0) pitch = pitch * global<float>(d::flt_0_8);
        if (zoom != 0) pitch = pitch * global<float>(d::flt_0_8);
        if (!(pitch >= global<float>(d::flt_1))) pitch = 1.0f;
        u32 pitch_bits = 0, height_bits = 0;
        const float fog_start = pitch * fog_k, fog_end = height * fog_k;  // the original: pitch, height
        std::memcpy(&pitch_bits, &fog_start, 4);
        std::memcpy(&height_bits, &fog_end, 4);
        joan::orig::as_viewport(node())->set_size(joan::orig::arg::i32(pitch_bits), joan::orig::arg::i32(height_bits));  // 0x00616a00
        jo::as_camera(node())->u8_cc = 1;
        jo::as_camera(node())->near_threshold = jo::self<jo::CameraFollow>(this)->height / global<float>(d::flt_4);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Camera::update_5634e0", 0x005634e0, &H::camera_update);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b87_06.cpp (game/b87_06)
namespace {
namespace game_b87_06 {

struct H {
    // ------------------------------------------------------------ 0x005639f0 (ret 4)
    void set_8(std::int32_t v) { jo::self<jo::CameraFollow>(this)->i32_8 = v; }
};

#define B06(name, addr, fn) JOAN_REPLACE("game/b87_06", name, addr, fn)
B06("0x005639f0 set_8", 0x005639f0, &H::set_8);
#undef B06

}  // namespace game_b87_06
}  // namespace

// ---------------------------------------- from modules/game_world_2.cpp (game/world2)
namespace {
namespace game_world_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;


constexpr img::Addr kGameUnits(d::g_units);

JOAN_COUNTER(c_follow_start, "CameraFollow::start");

struct H {
    // 0x00563a20 CameraFollow::start(unit): blend the camera (+4 motion, +0xc camera) from its
    // current frame to a view of the unit, unless one is running (+0x2f) or the unit is gone.
    std::int32_t follow_start(std::int32_t unit_id) {
        if (jo::self<jo::CameraFollow>(this)->follow_target != 0 || unit_id == 0 || joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(unit_id)) != 0) return 1;  // 0x00428b70
        c_follow_start.hit();
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(unit_id)));  // 0x0041b3c0
        joan::orig::as_unit_list(img::obj(d::g_a45f08))->select(static_cast<std::int32_t>(jo::as_unit(unit)->world->world_id));  // 0x00561350
        std::uint32_t state[10];
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(this)->motion(0)))->end(joan::orig::arg::mptr(static_cast<void*>(state)));  // 0x00563c80 / 0x00423fe0
        joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(static_cast<void*>(&state[1])));                            // 0x004fedb0
        float z;
        std::memcpy(&z, &state[3], 4);
        z = z + 50.0f;
        std::memcpy(&state[3], &z, 4);
        if (global<std::int32_t>(d::g_control_mode) != 3)
            state[5] = joan::orig::arg::as<std::uint32_t>(joan::orig::as_unit(unit)->mounted()) != 0 ? global<std::uint32_t>(d::g_a0bbec) : global<std::uint32_t>(d::g_a0bbe8);  // 0x00430190
        state[7] = joan::orig::arg::as<std::uint32_t>(joan::orig::as_unit(unit)->mounted()) != 0 ? global<std::uint32_t>(d::g_a0bbf4) : global<std::uint32_t>(d::g_a0bbf0);
        if ((joan::orig::arg::as<std::int32_t>(joan::orig::state_differs(joan::orig::arg::mptr(static_cast<void*>(state)), joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CameraFollow>(this)->motions->to)))) & 0xff) != 0)  // 0x0041a900
            joan::orig::as_camera_motion(jo::view<void*>(jo::self<jo::CameraFollow>(this)->motions))->start(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CameraFollow>(this)->camera->params)), joan::orig::arg::mptr(static_cast<void*>(state)));  // 0x00423ec0
        return 1;
    }
};

#define W2(name, addr, fn) JOAN_REPLACE("game/world2", name, addr, fn)
W2("CameraFollow::start", 0x00563a20, &H::follow_start);
#undef W2

}  // namespace game_world_2
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    // ---- game/world1 -------------------------------------------------------------------
    std::uint32_t follow_target() { return jo::self<jo::CameraFollow>(this)->follow_target; }                    // 0x00563dc0
    void* motion(std::int32_t i) { return &jo::self<jo::CameraFollow>(this)->motions[i]; }              // 0x00563c80
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("CameraFollow::target", 0x00563dc0, &H::follow_target);
W1("CameraFollow::motion", 0x00563c80, &H::motion);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::CameraMan::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_04::H::zero_37>(this)); }  // 0x561560
auto joan::orig::CameraMan::init() -> void { joan::bind::member<&::game_b109_15::H::cut_init>(this); }  // 0x5615c0
auto joan::orig::CameraMotion::vector_delete(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_02::H::vdd_562420>(this, a0)); }  // 0x562420
auto joan::orig::CameraMan::release() -> void { joan::bind::member<&::game_b83_02::H::release_textures>(this); }  // 0x5624c0
auto joan::orig::Camera::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_02::H::sdd_562910>(this, a0)); }  // 0x562910
auto joan::orig::Scroll::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_02::H::sdd_562960>(this, a0)); }  // 0x562960
auto joan::orig::CameraMan::update_keys() -> void { joan::bind::member<&::game_b108_17::H::camkeys>(this); }  // 0x562f40
auto joan::orig::CameraMan::set_8(std::int32_t a0) -> void { joan::bind::member<&::game_b87_06::H::set_8>(this, a0); }  // 0x5639f0
auto joan::orig::CameraFollow::start(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world_2::H::follow_start>(this, a0)); }  // 0x563a20
