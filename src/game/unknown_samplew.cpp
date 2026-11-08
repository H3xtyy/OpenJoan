// game/unknown_samplew.cpp: functions whose original source file is unknown (formerly game/_unplaced/samplew.cpp;
// re/attribution.csv: between ww/gamefile.cpp and ww/gameplay.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using joan::hook::global;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::call::thiscall;

using joan::img::str;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b107_06.cpp (game/b107_06)
namespace {
namespace game_b107_06 {

using joan::call::cdecl_call;

// ---- static initialiser pairs ----
template <fn::Code First, fn::Code Second>
i32 JOAN_CDECL init_pair() {
    cdecl_call<i32>(First);
    return cdecl_call<i32>(Second);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_06", name, addr, fn)
B("init_pair_479450", 0x00479450, (&init_pair<fn::ctor_a39350, fn::init_479490>));
#undef B

}  // namespace game_b107_06
}  // namespace

// ---------------------------------------- from modules/game_b105_03.cpp (game/b105_03)
namespace {
namespace game_b105_03 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

JOAN_COUNTER(c_teardown, "Mission::teardown_47b2b0");

// ---------------------------------------------------------------- static ctor / atexit dtor
template <fn::Code Thunk, auto Object>
u32 JOAN_CDECL call_on_global() {
    return thiscall<u32>(Thunk, obj(Object));
}

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}

    // ---------------------------------------------------------------- 0x0047b2b0
    u32 teardown_47b2b0() {
        c_teardown.hit();
        joan::orig::as_anim_def(obj(d::g_test_game_animation_def))->release(0u);  // 0x005b4170
        joan::orig::as_skin(obj(d::g_test_game_skin))->clear();     // 0x005f46a0
        joan::orig::as_sound3d(obj(d::g_weather_system))->release(static_cast<char>(joan::orig::arg::i32(1)));  // 0x00689d00
        joan::orig::as_list(obj(d::g_particle_effect_array))->destroy_41b530();     // 0x0041b530
        joan::orig::as_world(obj(d::g_world_sample))->cleanup();     // 0x00440320
        for (i32 n = joan::orig::arg::as<i32>(joan::orig::as_scene(obj(d::g_scene))->get_24()); n > 0; --n) {  // 0x005ea6c0
            void* const list = joan::orig::arg::as<void*>(joan::orig::as_scene(obj(d::g_scene))->group(joan::orig::arg::i32(n)));  // 0x005ea6e0
            if (list == nullptr) break;
            void* item = joan::orig::arg::as<void*>(joan::orig::as_scene_node(list)->attachments());  // 0x005efff0
            while (item != nullptr) {
                void* const next = joan::orig::arg::as<void*>(joan::orig::get24(joan::orig::arg::mptr(item)));  // 0x005f0020
                joan::orig::as_scene(list)->detach(joan::orig::arg::mptr(item));                   // 0x005efdb0
                if (jo::as_actor(item)->owner == nullptr) {
                    joan::orig::as_scene_object(item)->destroy(1u);  // 0x00420840 scalar deleting dtor
                }
                item = next;
            }
        }
        joan::orig::as_actor_def(obj(d::g_a39398))->release();  // 0x005b03d0
        if (global<void*>(d::g_camera_motion) != nullptr) {
            joan::orig::as_camera_motion(global<void*>(d::g_camera_motion))->free_list();  // 0x004236d0
            void* const p = global<void*>(d::g_camera_motion);
            if (p != nullptr) joan::orig::as_camera_man(p)->destroy(1u);  // 0x0047b4c0 scalar deleting dtor
            global<void*>(d::g_camera_motion) = nullptr;
        }
        if (global<void*>(d::g_path_space) != nullptr) {
            joan::orig::as_players(obj(d::g_physic_sys))->remove_cell(global<jo::PathSpace*>(d::g_path_space), nullptr);  // 0x006ad450
            void* const p = global<void*>(d::g_path_space);
            if (p != nullptr) joan::orig::as_pfd_data(p)->destroy(1u);  // 0x004201a0 scalar deleting dtor
            global<void*>(d::g_path_space) = nullptr;
        }
        return joan::orig::arg::as<u32>(joan::orig::as_actor_def(obj(d::g_a39708))->release());  // 0x005b03d0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_03", name, addr, fn)
B("Obj::stub_47b510", 0x0047b510, &H::stub0);
B("Obj::stub_47b530", 0x0047b530, &H::stub0);
B("Obj::stub_47b550", 0x0047b550, &H::stub0);
B("Obj::stub_47b570", 0x0047b570, &H::stub0);
B("Obj::stub_47b590", 0x0047b590, &H::stub0);
// static ctors / atexit dtors
B("ctor_a39350", 0x00479470, (&call_on_global<fn::Skin_Skin, d::g_test_game_skin>));  // 0x005f45f0
B("dtor_a39350", 0x004794c0, (&call_on_global<fn::Skin_dtor, d::g_test_game_skin>));  // 0x005f4620
B("ctor_a391c0", 0x00479500, (&call_on_global<fn::AnimLoader_AnimLoader, d::g_test_game_animation_def>));  // 0x005b2390
B("dtor_a391c0", 0x00479550, (&call_on_global<fn::Obj_call_5b4170, d::g_test_game_animation_def>));  // 0x00418c30
B("ctor_a39670", 0x00479590, (&call_on_global<fn::Battle_init, d::g_world_sample>));  // 0x0041ea20
B("dtor_a39670", 0x004795e0, (&call_on_global<fn::Obj4214b0_dtor, d::g_world_sample>));  // 0x004214b0
B("ctor_a39398", 0x00479620, (&call_on_global<fn::ActorDef_ActorDef, d::g_a39398>));  // 0x005af390
B("dtor_a39398", 0x00479670, (&call_on_global<fn::wrap_0047b5b0, d::g_a39398>));  // 0x0047b5b0
B("ctor_a39708", 0x004796b0, (&call_on_global<fn::ActorDef_ActorDef, d::g_a39708>));  // 0x005af390
B("dtor_a39708", 0x00479700, (&call_on_global<fn::wrap_0047b5b0, d::g_a39708>));  // 0x0047b5b0
// mission teardown
B("Mission::teardown_47b2b0", 0x0047b2b0, &H::teardown_47b2b0);
#undef B

}  // namespace game_b105_03
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------- cdecl
void JOAN_CDECL init_479490() { joan::crt::atexit(fn::code_ptr(fn::dtor_a39350)); }  // atexit

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("init_479490", 0x00479490, &init_479490);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b107_13.cpp (game/b107_13)
namespace {
namespace game_b107_13 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- static initialisers: two direct calls, eax of the second ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init2() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_13", name, addr, fn)
B("init_4794e0", 0x004794e0, (&init2<fn::ctor_a391c0, fn::init_479520>));
#undef B

}  // namespace game_b107_13
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------------- 0x00479520
static i32 JOAN_CDECL init_479520() { return joan::crt::atexit(fn::code_ptr(fn::dtor_a391c0)); }  // atexit

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("init_479520", 0x00479520, &init_479520);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b107_15.cpp (game/b107_15)
namespace {
namespace game_b107_15 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- CRT initialiser pairs (eax of the second call, ignored by _initterm) ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_15", name, addr, fn)
B("init_479570", 0x00479570, (&init_pair<fn::ctor_a39670, fn::init_4795b0>));
#undef B

}  // namespace game_b107_15
}  // namespace

// ---------------------------------------- from modules/game_b100_03.cpp (game/b100_03)
namespace {
namespace game_b100_03 {

using joan::call::cdecl_call;

// 0x004795b0
void JOAN_CDECL init_4795b0() { joan::crt::atexit(fn::code_ptr(fn::dtor_a39670)); }  // atexit

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_03", name, addr, fn)
B100("init_4795b0", 0x004795b0, &init_4795b0);
#undef B100

}  // namespace game_b100_03
}  // namespace

// ---------------------------------------- from modules/game_b107_19.cpp (game/b107_19)
namespace {
namespace game_b107_19 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- static initialiser pairs ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_19", name, addr, fn)
B("init_pair_479600", 0x00479600, (&init_pair<fn::ctor_a39398, fn::init_479640>));
#undef B

}  // namespace game_b107_19
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------- cdecl
i32 JOAN_CDECL init_479640() { return joan::crt::atexit(fn::code_ptr(fn::dtor_a39398)); }  // atexit

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("init_479640", 0x00479640, &init_479640);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b107_20.cpp (game/b107_20)
namespace {
namespace game_b107_20 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- CRT initialiser-table entries ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_20", name, addr, fn)
B("init_479690", 0x00479690, (&init_pair<fn::ctor_a39708, fn::init_atexit_479700>));
#undef B

}  // namespace game_b107_20
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using joan::call::cdecl_call;

static void JOAN_CDECL init_atexit_479700() { joan::crt::atexit(fn::code_ptr(fn::dtor_a39708)); }  // 0x004796d0

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("init_atexit_479700", 0x004796d0, &init_atexit_479700);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using u32 = std::uint32_t;

struct H {
    // -------------------------------------------------------------- 0x00479720
    u32 reset_479720() {
        joan::orig::as_game(this)->reset();  // 0x0046f1c0
        joan::orig::as_game(this)->load_resources_46f3a0();  // 0x0046f3a0
        joan::orig::as_game(this)->load_arrays();  // 0x00470d40
        joan::orig::as_calendar(obj(d::g_calendar))->start(joan::orig::arg::u32(i32{0x7d2}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}));  // 0x00495290
        joan::orig::as_game(obj(d::g_game_a50088))->on_mode(i32{0});                     // 0x0042ca90
        joan::orig::as_game(this)->end_session(i32{0});                                // 0x00471010
        joan::orig::as_game(this)->reset_all_470630();                                        // 0x00470630
        return joan::orig::arg::as<u32>(joan::orig::as_game(this)->shutdown_46f200());                                 // 0x0046f200
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Game::reset_479720", 0x00479720, &H::reset_479720);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b109_01.cpp (game/b109_01)
namespace {
namespace game_b109_01 {

using i16 = std::int16_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::x87::ftol;

using joan::orig::helpers::at;
using joan::orig::helpers::fbits;
using joan::orig::helpers::game_new;  // operator new
// operator new + thiscall ctor (no args) as `p = new T` (null when the allocation failed).
void* new_obj(u32 size, std::uintptr_t ctor_thunk) {
    void* const mem = game_new(size);
    return mem != nullptr ? thiscall<void*>(ctor_thunk, mem) : nullptr;
}

struct V3 {  // 12 bytes passed by value (float members: System V XMM)
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_sample, "Game::init_sample_world_4797a0");

void* const kUnits = obj(d::g_units);
void* unit(u32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 unit_missing(u32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70

// ---------------------------------------------------------------- 0x004797a0 helpers
void* const kRes = obj(d::g_texture_array);
void* const kCam = obj(d::g_view3d);
void* const kScene = obj(d::g_scene);
void* sky_tex(img::Addr name, i32 a) { return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kRes)->load(joan::orig::arg::str(str(name)), joan::orig::arg::i32(a), 0u)); }  // 0x006099c0

struct Slider {
    img::Addr global;
    i32 y, y2, range, factor;  // factor: index into VsCache::slider_factors
    bool scaled;  // ((f + 1) * v) / 2 instead of v * f
};

struct H {
    // -------------------------------------------------------------- 0x0045d330
    // Original frame [ebp-0x150]..[ebp-0x10]: the 0x18-byte time buffer is followed by the
    // locals it can overrun (tod, date, local / system FILETIME - dead after the format call)
    // and then the live file object, exactly as in the original.
    struct PerfFrame {
        char tbuf[0x18];  // [ebp-0x150]
        float tod;        // [ebp-0x138]
        u32 date;         // [ebp-0x134]
        u32 lft[2];       // [ebp-0x130]
        u32 ft[2];        // [ebp-0x128]
        u8 file[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];   // [ebp-0x120] (x64: the larger File)
    };
    static_assert(offsetof(PerfFrame, file) == 0x30 && sizeof(PerfFrame) >= 0x140, "PerfFrame: no padding between the frame slots");  // 0x140 bytes on x86

    // -------------------------------------------------------------- 0x004797a0
    u32 init_sample_world() {
        c_sample.hit();
        float bbox[6] = {fbits(0xc47a0000), fbits(0xc47a0000), fbits(0xc47a0000),
                         fbits(0x447a0000), fbits(0x447a0000), fbits(0x447a0000)};
        joan::orig::as_scene(kScene)->setup(joan::orig::arg::mptr(static_cast<void*>(bbox)));                         // 0x005e9e50
        joan::orig::as_scene(kScene)->set_fog_flag(static_cast<i32>(global<i16>(d::g_a39a06)));       // 0x005ee140
        joan::orig::as_model_list(obj(d::g_particle_effect_array))->load_stub();                                          // 0x0041b510
        if (global<jo::PathSpace*>(d::g_path_space) == nullptr) global<jo::PathSpace*>(d::g_path_space) = static_cast<jo::PathSpace*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::PathSpace)), fn::code_value(fn::PfdData_PfdData)));  // 0x00664fd0
        joan::orig::as_path_space(global<jo::PathSpace*>(d::g_path_space))->load(str(d::s_map_sample_pfd));  // 0x00670980 "map\\sample.pfd"
        joan::orig::as_players(obj(d::g_physic_sys))->add_cell(global<jo::PathSpace*>(d::g_path_space), nullptr, 0, 0, joan::orig::arg::mptr(static_cast<void*>(bbox)));  // 0x006ad270
        joan::orig::as_world(obj(d::g_world_sample))->init(joan::orig::arg::u16(1), str(d::s_sampleW), str(d::s_sample_world), joan::orig::arg::mptr(0));  // 0x0044e8c0
        joan::orig::as_world(obj(d::g_world_sample))->setup_sky();                                           // 0x0049ae00
        if (global<u32>(d::g_a52623) != 0) {
            static const Slider kSliders[] = {
                {d::g_a39a18, 0x32, 0x3c, 0x64, 7, false},  {d::g_a39a1c, 0x46, 0x50, 0x64, 8, false},
                {d::g_a39a20, 0x5a, 0x64, 0x64, 9, false},  {d::g_a39a24, 0x6e, 0x78, 0x64, 10, false},
                {d::g_a39a28, 0x82, 0x8c, 0x64, 11, false}, {d::g_a39a2c, 0xa0, 0xaa, 0x64, 3, false},
                {d::g_a39a30, 0xb4, 0xbe, 0x64, 4, false},  {d::g_a39a34, 0xc8, 0xd2, 0x64, 5, false},
                {d::g_a39a38, 0xe6, 0xf0, 0xc8, 0, true},   {d::g_a39a3c, 0xfa, 0x104, 0xc8, 1, true},
                {d::g_a39a40, 0x10e, 0x118, 0xc8, 2, true},
            };
            const Fpu fpu{true};
            for (const Slider& s : kSliders) {
                global<jo::Scroll*>(s.global) = static_cast<jo::Scroll*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::Scroll)), fn::code_value(fn::Widget62b_ctor)));  // 0x0062b6c0
                joan::orig::as_text_box(global<jo::Scroll*>(s.global))->init(joan::orig::arg::i16(0x64), joan::orig::arg::i16(s.y), joan::orig::arg::i16(0xc8), joan::orig::arg::i16(s.y2), joan::orig::arg::i16(5), fn::code_value(fn::Button_draw_frame_cb), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x0062b8d0
                joan::orig::as_scroll(global<jo::Scroll*>(s.global))->set_62be10(0u, joan::orig::arg::u32(s.range), 1u);  // 0x0062be10
                jo::VsCache* const cache = global<jo::VsCache*>(d::g_vs_cache);  // the original reads field20() (0x0047b5e0, = this+0x20) + 0x30..0x5c
                const double n = static_cast<double>(global<jo::Scroll*>(s.global)->max);
                const double f = static_cast<double>(cache->slider_factors[s.factor]);
                double v;
                if (!s.scaled) {
                    v = fpu.mul(n, f);  // fild; fmul dword
                } else {
                    v = fpu.add(f, static_cast<double>(global<float>(d::flt_1)));  // + 1.0
                    v = fpu.mul(v, n);                                               // fimul
                    v = fpu.div(v, static_cast<double>(global<float>(d::flt_2)));  // / 2.0
                }
                joan::orig::as_scroll(global<jo::Scroll*>(s.global))->set_pos(joan::orig::arg::i32(ftol(v)));  // 0x0062be50
            }
        }
        const i32 v2c = 1, v30 = 2, v34 = 3;
        // 0x0047b600 (ret 4) takes the last push, 0x0042a1c0 (ret 8) the two before it.
        joan::orig::as_nation(joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(joan::orig::arg::i32(v2c)))->set_relation_value(joan::orig::arg::i32(v34), joan::orig::arg::u32(-0x4d));
        joan::orig::as_nation(joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(joan::orig::arg::i32(v34)))->set_relation_value(joan::orig::arg::i32(v2c), joan::orig::arg::u32(-0x4d));
        joan::orig::as_nation(joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(joan::orig::arg::i32(v2c)))->set_relation_value(joan::orig::arg::i32(v30), joan::orig::arg::u32(-0x4d));
        joan::orig::as_nation(joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(joan::orig::arg::i32(v30)))->set_relation_value(joan::orig::arg::i32(v2c), joan::orig::arg::u32(-0x4d));
        const i32 v38 = joan::orig::arg::as<i32>(joan::orig::as_units(kUnits)->create_51dfc0(1));  // 0x0051dfc0
        global<jo::Unit*>(d::g_a399f0) = static_cast<jo::Unit*>(unit(static_cast<u32>(v38)));
        joan::orig::as_cell_object(global<jo::Unit*>(d::g_a399f0))->set_anim_state(1, joan::orig::arg::i32(global<u32>(d::g_now)), joan::orig::arg::i32(-1));  // 0x004ff9f0
        void* v3c = nullptr;  // the plane, written by select_mesh
        void* v40 = nullptr;  // select_mesh writes a pointer (the bone) here: pointer-sized, or x64 clobbers the neighbouring local
        global<u32>(d::g_now) = 1;
        joan::orig::as_actor(global<jo::Unit*>(d::g_a399f0)->actor)->select_mesh(joan::orig::arg::mptr(kCam), 1, &v3c, &v40);  // 0x005ad060
        joan::orig::as_plane(global<jo::Unit*>(d::g_a399f0)->actor->plane)->set_f8(1u);     // 0x005d5ee0
        joan::orig::as_plane(global<jo::Unit*>(d::g_a399f0)->actor->plane)->invalidate(joan::orig::arg::u32(-1));    // 0x005cf920
        global<u32>(d::g_now) = 0;
        joan::orig::as_unit(global<jo::Unit*>(d::g_a399f0))->set_group(static_cast<jo::World*>(obj(d::g_world_sample)));  // 0x004fed00
        joan::orig::as_ground_object(global<jo::Unit*>(d::g_a399f0))->set_position(V3{4561.0f, 4014.0f, 0.0f});  // 0x004fedf0
        joan::orig::as_unit(global<jo::Unit*>(d::g_a399f0))->set_side(joan::orig::arg::i32(v2c));  // 0x00509250 (word arg)
        {
            jo::Actor* p = global<jo::Unit*>(d::g_a399f0)->actor;
            jo::view<u32>(jo::as_actor(p)->aim_sphere[0]) = 0;
            jo::view<u32>(jo::as_actor(p)->aim_sphere[1]) = 0;
            jo::view<u32>(jo::as_actor(p)->aim_sphere[2]) = 0x427c0000;  // 63.0f
            p = global<jo::Unit*>(d::g_a399f0)->actor;
            jo::view<u32>(jo::as_actor(p)->aim_sphere[3]) = 0x41a00000;  // 20.0f
            p = global<jo::Unit*>(d::g_a399f0)->actor;
            jo::as_actor(p)->aim_mode = 1;
        }
        global<u16>(d::g_a47c36) = static_cast<u16>(v38);
        joan::orig::as_player(obj(d::g_player_unit_recno))->set_unit(joan::orig::arg::i32(v38));              // 0x0042f9c0
        joan::orig::as_players(obj(d::g_physic_sys))->set_timer(joan::orig::arg::i16(1));                // 0x006ad900
        joan::orig::as_unit(global<jo::Unit*>(d::g_a399f0))->sync_shadow();        // 0x00447600
        for (i32 id = joan::orig::as_dynarray(kUnits)->size(); id > 0; --id) {  // 0x00417ad0
            if (unit_missing(static_cast<u32>(id)) != 0) continue;
            u8* const u = static_cast<u8*>(unit(static_cast<u32>(id)));
            if (jo::as_unit(u)->group_id == 1) {
                joan::orig::as_group(global<jo::Unit*>(d::g_a399f0)->group)->add_unit(static_cast<i32>(jo::as_unit(u)->id));  // 0x005287d0
                jo::view<u16>(jo::as_unit(u)->leader_id) = jo::view<u16>(jo::as_unit(global<jo::Unit*>(d::g_a399f0))->id);
            }
        }
        for (i32 pass = 0; pass < 2; ++pass) {
            const i32 v68 = joan::orig::arg::as<i32>(joan::orig::as_units(obj(d::g_items))->create_49ea90(1u));  // 0x0049ea90
            global<jo::MapObject*>(d::g_a399f8) = joan::orig::arg::as<jo::MapObject*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(v68)));  // 0x00425d20
            // pushed via ax (word read by the callee)
            if (pass == 0) joan::orig::as_cell_object(global<jo::Unit*>(d::g_a399f0))->attach_unit(joan::orig::arg::i16(static_cast<i32>(static_cast<u16>(v68))), 2, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
            else joan::orig::as_cell_object(global<jo::Unit*>(d::g_a399f0))->attach_unit(joan::orig::arg::i16(static_cast<i32>(static_cast<u16>(v68))), 3, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(0));
            if (global<u32>(d::g_a52637) != 0) {
                jo::Material* const mesh = global<jo::MapObject*>(d::g_a399f8)->actor->plane->materials;
                mesh->texture_mode = 0xc;
                void* const tex = sky_tex(d::s_SKY1, 2);       // "SKY1"
                joan::orig::as_material(mesh)->set_texture(1, joan::orig::arg::mptr(tex));        // 0x005d5650
                joan::orig::texture_ref::release(joan::orig::arg::mptr(tex));               // 0x0042cef0
            }
        }
        if (global<jo::CameraMotion*>(d::g_camera_motion) == nullptr) global<jo::CameraMotion*>(d::g_camera_motion) = static_cast<jo::CameraMotion*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::CameraMotion)), fn::code_value(fn::Obj_clear_c8)));  // 0x00423590
        joan::orig::as_camera_motion(global<jo::CameraMotion*>(d::g_camera_motion))->init();  // 0x00423600
        global<i32>(d::g_a39a00) = v38;
        global<u8>(d::g_a39a04) = 1;
        alignas(4) u8 fog[4] = {0, 0x28, 0x0a, 0xff};
        joan::orig::as_camera(kCam)->set_fog_colour(joan::orig::arg::mptr(static_cast<void*>(fog)));  // 0x00616c10
        joan::orig::as_viewport(kCam)->set_58(2);                        // 0x00616a60
        joan::orig::as_viewport(kCam)->set_size(joan::orig::arg::bits(fbits(0x453b8000)), joan::orig::arg::bits(fbits(0x455ac000)));  // 0x00616a00
        joan::orig::as_camera(kCam)->set_viewport(0u, 0u, joan::orig::arg::u32(global<i32>(d::g_a48880)), joan::orig::arg::u32(global<i32>(d::g_a48884)), joan::orig::arg::u32(global<i32>(d::g_a48888)), joan::orig::arg::u32(global<i32>(d::g_a4888c)));  // 0x00613bf0 Camera::set_viewport
        joan::orig::as_camera(kCam)->set_ranges(joan::orig::arg::f32(fbits(0x3f800000)), joan::orig::arg::f32(fbits(0x45800800)), joan::orig::arg::f32(fbits(0x46800200)));  // 0x00613ca0
        joan::orig::as_camera(kCam)->set_scene(static_cast<jo::Scene*>(kScene))
;  // 0x00613db0
        u8* const sky = static_cast<u8*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::SkyBox)), fn::code_value(fn::SkyBox_SkyBox)));  // 0x005f5100
        if (global<u32>(d::g_a52623) != 0) {
            alignas(alignof(void*)) u8 file[((sizeof(jo::FileMem) > 0x11c ? sizeof(jo::FileMem) : 0x11c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger FileMem
            joan::orig::as_file_mem(file)->construct();                        // 0x00587f80
            joan::orig::as_file_mem(file)->open_588000(str(d::s_test_sky_mesh_SMO), 1, static_cast<char>(joan::orig::arg::i32(0)));           // 0x00588000 "test\\sky_mesh.SMO"
            joan::orig::as_plane(sky)->load_5ddd70(joan::orig::arg::mptr(static_cast<void*>(file)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)));      // 0x005ddd70
            for (i32 i = 0; i < jo::as_plane(sky)->material_count; ++i) {
                jo::as_plane(sky)->materials[i].texture_mode = 0x10;
                jo::as_plane(sky)->materials[i].blend = 2;
            }
            joan::orig::as_plane(sky)->set_100(1u);  // 0x005d6e20
            joan::orig::as_file_mem(file)->destruct();  // 0x00587fd0
        } else {
            joan::orig::as_sky_box(sky)->init(1);  // 0x005f51b0
            const i32 part = joan::orig::arg::as<i32>(joan::orig::as_plane(sky)->add_materials(1));  // 0x005cd470
            jo::Material* const mat = &jo::as_plane(sky)->materials[static_cast<u32>(part)];
            joan::orig::as_material(mat)->construct();  // 0x005d52a0
            mat->blend = 2;
            void* const tex = sky_tex(d::s_SKY1, 2);  // "SKY1"
            joan::orig::as_material(mat)->set_texture(0, joan::orig::arg::mptr(tex));    // 0x005d5650
            joan::orig::texture_ref::release(joan::orig::arg::mptr(tex));          // 0x0042cef0
            joan::orig::as_sky_box(sky)->build(joan::orig::arg::f32(fbits(0x43fa0000)), joan::orig::arg::i32(part), 0x3f);  // 0x005f5240
        }
        joan::orig::as_plane(sky)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(sky)->sort_by_material(1);   // 0x005cf1d0
        joan::orig::as_scene(kScene)->set_sky(joan::orig::arg::mptr(static_cast<void*>(sky)));  // 0x005ea060
        void* node = joan::orig::arg::as<void*>(joan::orig::as_scene(kScene)->add_node());             // 0x005ea460
        joan::orig::as_scene_source(node)->reset(joan::orig::arg::mptr(static_cast<void*>(bbox)));    // 0x005ef880
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
        u8* const quad = static_cast<u8*>(mem != nullptr ? joan::orig::as_plane(mem)->construct(1u, 0u) : nullptr);  // 0x005cbaf0
        joan::orig::as_plane(quad)->init(0, 0, 0, 1u);  // 0x005cc7a0
        const i32 part = joan::orig::arg::as<i32>(joan::orig::as_plane(quad)->add_materials(1));  // 0x005cd470
        jo::Material* const mat = &jo::as_plane(quad)->materials[static_cast<u32>(part)];
        joan::orig::as_material(mat)->construct();  // 0x005d52a0
        alignas(4) u8 col[4] = {0, 0xf0, 0, 0xff};
        joan::orig::as_material(mat)->set_colour0(joan::orig::arg::mptr(static_cast<void*>(col)));  // 0x005d5540
        void* tex = sky_tex(d::s_TEST, 1);                       // "TEST"
        joan::orig::as_material(mat)->set_texture(0, joan::orig::arg::mptr(tex));                   // 0x005d5650
        joan::orig::as_texture_manager(kRes)->release(joan::orig::arg::mptr(tex));                     // 0x0060a550
        tex = sky_tex(d::s_TRANS1, 1);                             // "TRANS1"
        joan::orig::as_material(mat)->set_texture(1, joan::orig::arg::mptr(tex));                   // 0x005d5650
        joan::orig::as_texture_manager(kRes)->release(joan::orig::arg::mptr(tex));                     // 0x0060a550
        mat->blend = 1;
        mat->texture_mode = 2;
        {
            const float z = fbits(0xc1a00000);   // -20
            const float hi = fbits(0x42c80000);  // 100
            const float d4 = 0.0f, d8 = 0.0f;
            const float c = global<float>(d::k_9cf5c4);  // 0.0
            alignas(4) u32 vtx[36];
            auto put = [&](i32 k, float x, float y, u32 color, u32 tu, u32 tv) {
                u32* const v = vtx + k * 9;
                std::memcpy(&v[0], &x, 4);
                std::memcpy(&v[1], &y, 4);
                std::memcpy(&v[2], &z, 4);
                v[3] = 0;
                v[4] = 0;
                v[5] = 0x3f800000;
                v[6] = color;
                v[7] = tu;
                v[8] = tv;
            };
            put(0, c + d4, c + d8, 0xfff0f0f0u, 0, 0);
            put(1, hi + d4, c + d8, 0xfff08080u, 0, 0x3f800000);
            put(2, c + d4, hi + d8, 0xff8080f0u, 0x3f800000, 0);
            put(3, hi + d4, hi + d8, 0xff80f080u, 0x3f800000, 0x3f800000);
            const u16 idx[6] = {0, 1, 2, 2, 1, 3};
            const i32 base = joan::orig::arg::as<i32>(joan::orig::as_plane(quad)->add_vertices(4));  // 0x005cd7f0
            std::memcpy(joan::orig::arg::mptr(&jo::as_plane(quad)->vertices_f1[base]), static_cast<const void*>(vtx), u32{0x90});  // memcpy
            joan::orig::as_plane(quad)->set_format(2);  // 0x005cdc70
            const i32 tri = joan::orig::arg::as<i32>(joan::orig::as_plane(quad)->add_triangles(2));  // 0x005ce6f0
            for (i32 i = 0; i < 6; ++i)
                jo::as_plane(quad)->triangles[tri + i / 3].v[i % 3] =
                    static_cast<u16>(static_cast<u32>(base) + idx[i]);
            for (i32 i = 0; i < 2; ++i)
                jo::as_plane(quad)->triangle_materials[tri + i].value =
                    static_cast<u16>(part);
        }
        joan::orig::as_plane(quad)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(quad)->sort_by_material(1);   // 0x005cf1d0
        joan::orig::as_scene_source(node)->set_38(joan::orig::arg::mptr(static_cast<void*>(quad)));  // 0x005efbe0
        joan::orig::as_scene(kScene)->build_tree();                          // 0x005ea730
        node = joan::orig::arg::as<void*>(joan::orig::as_scene(kScene)->add_node());                 // 0x005ea460
        joan::orig::as_scene_source(node)->reset(joan::orig::arg::mptr(static_cast<void*>(bbox)));  // 0x005ef880
        return thiscall<u32>(fn::Scene_build_sector_struct, kScene);                   // 0x005ea730
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_01", name, addr, fn)
B("Game::init_sample_world_4797a0", 0x004797a0, &H::init_sample_world);
#undef B

}  // namespace game_b109_01
}  // namespace

// ---------------------------------------- from modules/game_b83_06.cpp (game/b83_06)
namespace {
namespace game_b83_06 {

using joan::call::cdecl_call;

// Line buffers with a pad in front (buf[-1] write on an empty line) and behind (read_line
// 0x00589f10 stores the NUL at buf[n], and n == N for the 0x40 / 0x240 reads; in the original
// that byte lands in a dead / later-overwritten local, strip_last then terminates at N - 1).
template <std::uint32_t N>
struct alignas(4) Line {
    char pad[4];
    char b[N];
    char tail[4];
};

JOAN_COUNTER(c_node, "Scene::create_node");

std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
using joan::orig::helpers::stricmp_;
double fabs_(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }
double cos_(double v) { return joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(v))); }
double sin_(double v) { return joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(v))); }

char* token(void* r) { return joan::orig::as_file_txt(r)->get_token(1); }                     // 0x00589c70
void read_line(void* r, char* buf, std::int32_t n) { joan::orig::as_file_txt(r)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(n)); }  // 0x00589f10
void strip_last(char* buf) { buf[static_cast<std::int32_t>(strlen_(buf)) - 1] = 0; }

struct H {
    // ------------------------------------------------------------------ 0x0047ad40
    std::int32_t create_node(void* reader) {
        c_node.hit();
        const Fpu fpu{true};
        const double k32 = global<float>(d::flt_32);  // 32.0f
        const float k2 = global<float>(d::flt_2);    // 2.0f
        Line<0x40> name;
        Line<0x100> line;
        token(reader);
        token(reader);
        read_line(reader, name.b, 0x40);
        strip_last(name.b);
        token(reader);
        read_line(reader, name.b, 0x40);
        strip_last(name.b);
        read_line(reader, line.b, 0xff);
        read_line(reader, line.b, 0xff);
        token(reader);
        const float x0 = static_cast<float>(fpu.mul(atof_(token(reader)), k32));
        token(reader);
        const float y0 = static_cast<float>(fpu.mul(atof_(token(reader)), k32));
        token(reader);
        const float z0 = static_cast<float>(fpu.mul(atof_(token(reader)), k32));
        static_cast<void>(z0);  // overwritten by the box corners below
        std::uint64_t scale_bits = 0x3ff0000000000000ull;  // 1.0
        if (stricmp_(token(reader), str(d::s_HD)) == 0) scale_bits = 0x3f91df46a2529d39ull;  // pi / 180
        double scale;
        std::memcpy(&scale, &scale_bits, sizeof scale);
        const float angle = static_cast<float>(fpu.mul(atof_(token(reader)), scale));
        token(reader);
        const float ext_a = static_cast<float>(fpu.mul(atof_(token(reader)), k32));  // [ebp-0x16c]
        token(reader);
        const float ext_b = static_cast<float>(fpu.mul(atof_(token(reader)), k32));  // [ebp-0x168]
        token(reader);
        atof_(token(reader));  // stored, then overwritten with 1000.0f (dead)
        read_line(reader, line.b, 0xff);

        float box[6];  // [ebp-0x184]: corner 0 (x, y, z), corner 1 (x, y, z)
        // corner 0
        {
            const double t1 = fpu.mul(fabs_(cos_(angle)), ext_a / k2);
            double s = fpu.mul(fabs_(sin_(angle)), ext_b / k2);
            s = fpu.add(s, t1);
            box[0] = static_cast<float>(fpu.sub(x0, s));
        }
        {
            const double t1 = fpu.mul(fabs_(cos_(angle)), ext_b / k2);
            double s = fpu.mul(fabs_(sin_(angle)), ext_a / k2);
            s = fpu.add(s, t1);
            box[1] = static_cast<float>(fpu.sub(y0, s));
        }
        box[2] = -5000.0f;  // 0xc59c4000
        // corner 1
        {
            const double t1 = fpu.mul(fabs_(cos_(angle)), ext_a / k2);
            double s = fpu.mul(fabs_(sin_(angle)), ext_b / k2);
            s = fpu.add(s, t1);
            box[3] = static_cast<float>(fpu.add(s, x0));
        }
        {
            const double t1 = fpu.mul(fabs_(cos_(angle)), ext_b / k2);
            double s = fpu.mul(fabs_(sin_(angle)), ext_a / k2);
            s = fpu.add(s, t1);
            box[4] = static_cast<float>(fpu.add(s, y0));
        }
        box[5] = 5000.0f;  // 0x459c4000

        const std::int32_t list = joan::orig::arg::as<std::int32_t>(joan::orig::as_map(this)->add_cells(1));        // 0x00547bf0
        void* const nodes = joan::orig::arg::as<void*>(joan::orig::as_map(this)->cell(joan::orig::arg::i32(list)));                  // 0x00548600
        const std::int32_t iy = joan::hook::x87::ftol(box[1]);
        const std::int32_t ix = joan::hook::x87::ftol(box[0]);
        joan::orig::as_map(nodes)->load_res(joan::orig::arg::str(static_cast<char*>(name.b)), joan::orig::arg::mptr(static_cast<float*>(box)), joan::orig::arg::u32(ix), joan::orig::arg::u32(iy));  // 0x00439ff0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_06", name, addr, fn)
B("Scene::create_node", 0x0047ad40, &H::create_node);
#undef B

}  // namespace game_b83_06
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

struct H {
    // ------------------------------------------------------------------------ 0x0047b4c0
    void* scalar_dtor_b(u32 flags) {
        joan::orig::as_camera_motion(this)->destruct();  // 0x004235d0
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(this));  // operator delete
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("ObjB::scalar_dtor", 0x0047b4c0, &H::scalar_dtor_b);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

struct H {
    // 0x0047b5b0 / 0x00456fb0 wrappers
    std::uint32_t wrap_47b5b0() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_actor_def(this)->release()); }  // 0x005b03d0
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("wrap_0047b5b0", 0x0047b5b0, &H::wrap_47b5b0);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

struct H {
    void* field20() { return jo::self<jo::VsCache>(this)->gap_20; }  // 0x0047b5e0
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("field20", 0x0047b5e0, &H::field20);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units {

struct H {
    void* list_get_wrap(std::int32_t i) { return joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_41b3f0(joan::orig::arg::i32(i))); }  // 0x0047b600
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("List::get_h", 0x0047b600, &H::list_get_wrap);
#undef U1

}  // namespace game_units
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Game::reset_479720() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::reset_479720>(this)); }  // 0x479720
auto joan::orig::Scene::create_node(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_06::H::create_node>(this, a0)); }  // 0x47ad40
auto joan::orig::CameraMan::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_02::H::scalar_dtor_b>(this, a0)); }  // 0x47b4c0
auto joan::orig::ActorDef::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves81::H::wrap_47b5b0>(this)); }  // 0x47b5b0
auto joan::orig::VsCache::field20() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::field20>(this)); }  // 0x47b5e0
auto joan::orig::List::get_h(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_units::H::list_get_wrap>(this, a0)); }  // 0x47b600
